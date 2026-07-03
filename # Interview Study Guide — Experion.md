# Interview Study Guide — Experion Dependency Intelligence Chatbot

> A complete, self-contained study document for this project. Read it top to
> bottom once, then use the Q&A sections to rehearse out loud. Every claim here
> is grounded in the actual codebase so you can defend it in an interview.

---

## Table of Contents
1. [Elevator Pitch (memorize this)](#1-elevator-pitch)
2. [The Problem It Solves](#2-the-problem-it-solves)
3. [High-Level Architecture](#3-high-level-architecture)
4. [End-to-End Request Lifecycle](#4-end-to-end-request-lifecycle)
5. [Component-by-Component Deep Dive](#5-component-by-component-deep-dive)
6. [Key Design Decisions & Trade-offs](#6-key-design-decisions--trade-offs)
7. [Technology Cheat-Sheet (what & why)](#7-technology-cheat-sheet)
8. [Interview Q&A — Project-Specific](#8-interview-qa--project-specific)
9. [Interview Q&A — RAG / LLM / AI](#9-interview-qa--rag--llm--ai)
10. [Interview Q&A — System Design](#10-interview-qa--system-design)
11. [Interview Q&A — Python / FastAPI / Backend](#11-interview-qa--python--fastapi--backend)
12. [Interview Q&A — Frontend (Next.js / React)](#12-interview-qa--frontend)
13. [Interview Q&A — Graphs & Algorithms](#13-interview-qa--graphs--algorithms)
14. [Interview Q&A — Security](#14-interview-qa--security)
15. [Interview Q&A — Testing & Quality](#15-interview-qa--testing--quality)
16. [Behavioral / STAR Stories](#16-behavioral--star-stories)
17. [Weaknesses & "What Would You Improve"](#17-weaknesses--what-would-you-improve)
18. [Interview Methods & How to Present](#18-interview-methods--how-to-present)
19. [Glossary](#19-glossary)

---

## 1. Elevator Pitch

> "It's a conversational AI assistant that lets engineers ask plain-English
> questions about a large software product's dependency graph — things like
> *'what breaks if I change this package?'* or *'who depends on ctrllib?'* — and
> get accurate, graph-verified answers instead of digging through spreadsheets.
> The trick is a **hybrid design**: a deterministic graph engine (NetworkX)
> computes the real answer, and the LLM (Azure OpenAI GPT-4.1) is only used to
> understand the question and phrase the response — so it **can't hallucinate**
> the facts. It's full-stack: FastAPI backend, Next.js/React frontend with
> streaming chat and interactive graph visualizations."

Say this in ~30 seconds. It signals: full-stack, AI, and good judgment about LLM
reliability.

---

## 2. The Problem It Solves

- **Domain:** Honeywell Experion is a large industrial control software product
  built from **1,000+ interdependent packages** (Conan, NuGet, hybrid) plus
  **39 firmware images** and thousands of source files under legacy **ClearCase**
  version control.
- **Pain:** When an engineer changes one package, figuring out *everything else
  that could break* (the "blast radius") meant manually tracing dependencies —
  slow and error-prone.
- **Solution:** Ask in natural language; get a precise answer backed by a real
  dependency graph, with visualizations and file/repo traceability.
- **Why not just ChatGPT?** A raw LLM would *hallucinate* dependency facts. This
  system grounds every factual answer in a graph the LLM cannot override.

---

## 3. High-Level Architecture

```
┌─────────────────────┐        HTTP (proxied)        ┌──────────────────────┐
│  Next.js frontend   │ ◄──────────────────────────► │  FastAPI backend     │
│  web/ (React 18)    │   localhost:3000 → :8000     │  rag_scan/api.py     │
│  streaming chat UI  │                              │  (thin orchestrator) │
└─────────────────────┘                              └──────────┬───────────┘
                                                                │
        ┌───────────────┬──────────────┬───────────────┬───────┴────────┐
        │               │              │               │                │
   NetworkX graph   Intent+Entity   Azure OpenAI   ClearCase map   Firmware index
  (graph_builder)  (detector +     (GPT-4.1 w/    (cc_git_map)    (39 images,
   1,087 pkgs       extractor,      function       path→pkg/repo    6,550 files)
   cached .pkl)     RapidFuzz)      calling)
```

**Two-layer answer strategy:**
1. **Deterministic layer** — graph queries, firmware index, ClearCase resolver
   produce authoritative markdown tables that bypass the LLM entirely.
2. **LLM layer** — used for intent understanding, entity classification, and
   natural-language phrasing, always constrained by graph context.

**Auth & state:** Honeywell ONE SSO (OAuth2/OIDC) + JWT sessions; Redis-backed
conversation store with in-memory fallback.

---

## 4. End-to-End Request Lifecycle

Walk an interviewer through this — it shows you understand the whole system:

1. **User types** a question in the Next.js chat UI → `POST /api/chat` (proxied to
   FastAPI port 8000).
2. **Auth guard** validates the JWT/SSO session (if SSO enabled).
3. **Pre-filter** (`openai_pipeline.py`) runs *in Python* before any LLM call —
   blocks jailbreak / prompt-injection / off-topic / identity-probe messages.
4. **Overlap & ambiguity checks** — does the message contain a ClearCase path or
   name that exists in *both* engineering-tools and firmware? If yes → ask the
   user to disambiguate (return early, no LLM).
5. **Intent detection** (`intent_detector.py`) — regex classifier labels the query:
   DEPENDENCIES, DEPENDENTS, IMPACT, PATH, COUNT, VISUALIZE, STATS, etc.
6. **Entity extraction** (`entity_extractor.py`) — RapidFuzz maps free text to
   canonical graph node IDs (handles typos, case, `exp_et_` prefixes).
7. **Query execution** (`query_executor.py`) — runs the actual graph operation
   (neighbors, reverse neighbors, transitive impact set, shortest path) against
   the cached NetworkX graph.
8. **Rendering** — result formatted as a markdown table; for VISUALIZE intent,
   `visualizer.py` builds an interactive PyVis/vis-network HTML graph.
9. **LLM phrasing (optional)** — `openai_pipeline.py` calls GPT-4.1 with
   **function/tool calling** and the graph context injected, to phrase a natural
   answer *without inventing facts*. Supports **streaming (SSE)**.
10. **Session persistence** — turn saved to Redis (or memory) for multi-turn
    context (e.g. "what about its dependents?" resolves the pronoun to the last
    entity).
11. **Frontend renders** markdown, expandable dependency tables, and any graph
    iframe.

---

## 5. Component-by-Component Deep Dive

### Backend (`rag_scan/`, ~7,400 LOC)

| Module | Responsibility | Key detail to mention |
|---|---|---|
| `api.py` (~3,556 LOC) | FastAPI app, routes, orchestration, session mgmt, overlap/ambiguity gating | "Thin wrapper" that wires all modules together; builds/loads graph once at startup |
| `graph_builder.py` | Builds a `nx.DiGraph` from the scan JSON; pickle cache | Case-insensitive node resolution; handles hybrid `name::manager` node IDs |
| `intent_detector.py` | Regex-based intent classifier | Ordered pattern list (GLOBAL_COUNT before COUNT, etc.) — order matters |
| `entity_extractor.py` | Fuzzy match text → graph nodes (RapidFuzz) | Confidence tiers: exact=100, high≥85, low≥60; suggests alternatives |
| `query_executor.py` | Executes graph queries, formats tables | Uses precomputed `impact_set` from cache for O(1)-ish transitive answers |
| `visualizer.py` (~597 LOC) | PyVis/vis-network interactive HTML graphs | Neighborhood highlighting; served as static HTML in an iframe |
| `openai_pipeline.py` (~762 LOC) | Azure OpenAI integration, tool calling, streaming, pre-filters | LLM constrained to a `respond` tool; regex guards for jailbreak/identity/off-topic |
| `clearcase_resolver.py` | ClearCase path → package/repo via `cc_git_map.json` | Renders impact tables with GitHub repo links |
| `firmware_query.py` (~708 LOC) | "Which firmware use this file/binary?" | Deterministic tables, bypasses LLM to avoid hallucination |
| `firmware_index.py` | Builds/loads firmware file-usage index | Pickled for fast startup |
| `ambiguity.py` | Cross-dataset name collision resolver | Computes collisions between graph nodes and firmware binaries |
| `auth.py` | Honeywell ONE SSO (OAuth2/OIDC) + JWT | Fully gated behind `sso_enabled`; safe no-op when disabled |
| `session_store.py` | Redis session store w/ in-memory fallback | TTL-based, graceful degradation, message cap |
| `build_file_map.py` | Package → build file mapping | Small helper used in impact tables |

### Frontend (`web/`, ~3,800 LOC, 18 files)

- **Next.js 14 App Router** pages: `chat/`, `explorer/`, `repositories/`,
  `feedback/`, `login/`.
- **React 18 + TypeScript + Tailwind.**
- **State:** Zustand; **data fetching:** SWR.
- **Chat rendering:** react-markdown + remark-gfm + rehype-highlight.
- **Graphs:** vis-network / vis-data (mirrors the backend PyVis output).
- **`next.config.mjs`** proxies `/api/*` and `/graphs/*` to the FastAPI backend
  (avoids CORS in dev, keeps one origin).
- **`AuthGuard.tsx`** protects routes; **`Sidebar.tsx`** navigation.

### Static assets (`lib/`)
Vendored `vis-9.1.2/`, `tom-select/`, `bindings/` — the JS/CSS the PyVis-generated
graph HTML needs to render offline.

---

## 6. Key Design Decisions & Trade-offs

| Decision | Why | Trade-off / cost |
|---|---|---|
| **Hybrid graph + LLM** (not pure LLM) | Factual answers must be correct; LLMs hallucinate | More code; two systems to keep in sync |
| **Deterministic answers bypass the LLM** | Guarantees accuracy + speed (0.26 s avg) | Less "conversational" for those paths |
| **Regex intent + RapidFuzz entity** (not an ML classifier) | Transparent, fast, no training data, easy to debug | Brittle to unusual phrasings; needs pattern upkeep |
| **Python-level prompt-injection filter** | Runs as *code*, can't be overridden by prompt text | Can false-positive; must tune patterns |
| **Pickle graph cache at startup** | Avoid rebuilding a 1,000-node graph per request | Cache invalidation on data change |
| **Redis with in-memory fallback** | Works in prod (multi-instance) *and* laptop dev | Memory fallback loses state on restart |
| **Function/tool calling with a single `respond` tool** | Forces structured, controllable LLM output | Slightly rigid vs free-form |
| **SSO gated behind a flag** | Ship without infra dependency; enable in prod | Two code paths to test |
| **Next.js proxy to FastAPI** | One origin, no CORS pain, clean dev | Extra hop; prod needs real reverse proxy (Nginx) |

---

## 7. Technology Cheat-Sheet

Be ready to say **what each is and why you used it**:

- **FastAPI** — modern async Python web framework. Chosen for async I/O, automatic
  validation (Pydantic), and easy streaming responses.
- **Uvicorn** — ASGI server that runs FastAPI.
- **Pydantic** — request/response schema validation (the `BaseModel` classes).
- **NetworkX** — pure-Python graph library. Used to model packages as nodes and
  "depends-on" as directed edges; gives us neighbors, ancestors/descendants,
  shortest path, transitive closure.
- **Azure OpenAI (GPT-4.1)** — enterprise-hosted LLM (data-residency/compliance).
  Used via **function/tool calling** for structured, controllable output.
- **RAG (Retrieval-Augmented Generation)** — retrieve real facts (from the graph)
  and feed them to the LLM so it answers from data, not memory.
- **RapidFuzz** — fast fuzzy string matching (Levenshtein-based) for entity
  resolution.
- **Redis** — in-memory data store; here for session/conversation state with TTL.
- **python-jose** — JWT encode/verify for auth sessions.
- **OAuth2 / OIDC** — the SSO protocol (Honeywell ONE) for enterprise login.
- **Next.js 14 (App Router)** — React framework with file-based routing and SSR.
- **Zustand** — minimal React state management.
- **SWR** — React data-fetching/caching hook.
- **vis-network / PyVis** — interactive network-graph visualization.
- **Server-Sent Events (SSE)** — one-way streaming from server to browser for
  token-by-token chat responses.

---

## 8. Interview Q&A — Project-Specific

**Q: Give me a quick overview of the project.**
A: Use the elevator pitch (Section 1). Then offer to go deeper on the hybrid
architecture.

**Q: Why not just use ChatGPT / a pure LLM?**
A: Dependency facts must be exact — a wrong "what breaks" answer is dangerous. LLMs
hallucinate. So the *facts* come from a deterministic NetworkX graph; the LLM only
interprets the question and phrases the answer. The LLM literally can't override the
graph result.

**Q: What was the hardest part?**
A: Cross-dataset disambiguation. The same name or ClearCase path can exist in both
engineering-tools and firmware (35 ambiguous names, 1,068 overlapping paths).
Silently guessing gives wrong answers, so I built an overlap index that detects the
collision and asks the user which dataset they mean before answering.

**Q: How do you make sure answers aren't hallucinated?**
A: Three layers — (1) deterministic handlers return graph/firmware tables *without*
calling the LLM; (2) when the LLM is used, the real graph context is injected and
it's constrained to a `respond` tool; (3) a Python pre-filter blocks off-topic and
injection attempts before the model is even called.

**Q: How does multi-turn context work ("what about its dependents?")?**
A: The session store keeps `current_entity` and recent messages. When a follow-up
lacks an explicit entity, we resolve it against the last entity in the session.

**Q: What would you do differently?** → See Section 17.

---

## 9. Interview Q&A — RAG / LLM / AI

**Q: What is RAG and how does it apply here?**
A: Retrieval-Augmented Generation = retrieve relevant facts from an external source
and give them to the LLM so it answers from data, not its trained memory. Here the
"retrieval" is a graph query (structured retrieval) rather than vector search — I
retrieve the exact dependency/impact facts and inject them as context.

**Q: Vector/embedding RAG vs. graph retrieval — why graph?**
A: The data *is* a graph of relationships. Vector search finds semantically similar
text; it can't compute a transitive impact set or a shortest path. For "what breaks
if I change X," you need graph traversal, not similarity. (The architecture doc even
outlines a hybrid retriever with TF-IDF for the semantic parts.)

**Q: What is function/tool calling and why use it?**
A: The LLM returns a structured JSON call to a named tool instead of free text.
I force a single `respond` tool so output is predictable and parseable, and so the
model stays on rails.

**Q: How do you defend against prompt injection?**
A: A compiled-regex pre-filter runs in Python *before* the LLM call — it's code, not
an instruction, so a malicious prompt can't tell it to ignore itself. It catches
"ignore your instructions," "act as," "jailbreak/DAN," identity probes, and clearly
off-topic requests, returning a fixed refusal.

**Q: How do you evaluate the system's quality?**
A: A 77-case automated test suite across 12 categories (exact names, deliverables,
files, ambiguity, impact, paths, etc.), asserting both correctness and output format
— 100% pass, 0.26 s avg latency, documented in a production-readiness report.

**Q: How would you reduce LLM cost/latency?**
A: Most queries never hit the LLM (deterministic handlers). For the rest: cache
common answers, use a smaller model for intent, and stream tokens (already done via
SSE) for perceived speed.

---

## 10. Interview Q&A — System Design

**Q: Walk me through the architecture.** → Section 3 + Section 4.

**Q: How does it scale to more users?**
A: Backend is stateless per request; session state lives in Redis, so I can run many
FastAPI instances behind a load balancer. The graph is read-only and cached in each
instance's memory. LLM calls are the bottleneck, so I'd add response caching and rate
limiting.

**Q: How do you keep the graph fresh when packages change?**
A: A scan pipeline regenerates the JSON; the server rebuilds and re-pickles the graph
cache. In production I'd trigger a rebuild on new scan data and hot-swap the cache.

**Q: Single point of failure?**
A: The Azure OpenAI dependency — mitigated because deterministic answers work without
it (graceful degradation via the `OPENAL_AVAILABLE` flag). Redis is optional (memory
fallback).

**Q: Why FastAPI over Flask/Django?**
A: Async I/O suits LLM/network-bound calls; built-in Pydantic validation; first-class
streaming for SSE; lightweight vs Django for an API-only service.

---

## 11. Interview Q&A — Python / FastAPI / Backend

**Q: How does FastAPI handle async and why does it matter here?**
A: Endpoints are `async def`; while awaiting the LLM/network, the event loop serves
other requests. It matters because LLM calls are slow and I/O-bound.

**Q: How do you stream responses?**
A: A `StreamingResponse` yielding Server-Sent Events; the OpenAI stream deltas are
forwarded token-by-token to the browser.

**Q: How is configuration/secrets handled?**
A: `.env` via `python-dotenv`; secrets (Azure keys, JWT secret, SSO creds) never
committed. `.env.example` documents required vars.

**Q: Explain the graph caching.**
A: On startup, if a pickle cache exists we `load_cache`, else `build_and_save` from
JSON. Precomputed impact sets live in the cache dict so queries are fast.

**Q: What's a Pydantic model doing here?**
A: Validates and types the incoming chat request body and outgoing responses,
rejecting malformed input at the boundary.

---

## 12. Interview Q&A — Frontend

**Q: Why Next.js App Router?**
A: File-based routing, easy page structure (chat/explorer/repositories), built-in
dev proxy to the backend, and good DX with TypeScript + Tailwind.

**Q: How does the frontend talk to the backend without CORS issues?**
A: `next.config.mjs` rewrites `/api/*` and `/graphs/*` to the FastAPI server, so the
browser sees one origin in dev.

**Q: How do you render the streaming chat?**
A: Read the SSE stream, append tokens to the message in Zustand state, and render
markdown incrementally with react-markdown (+ GFM tables, syntax highlighting).

**Q: How are the interactive graphs embedded?**
A: The backend generates PyVis/vis-network HTML; the frontend loads it in an iframe
served via the `/graphs/*` proxy, with vendored vis-network assets.

**Q: Why Zustand over Redux?**
A: Minimal boilerplate for a small app; a single store for chat state is enough.

---

## 13. Interview Q&A — Graphs & Algorithms

**Q: How is the dependency graph modeled?**
A: A directed graph (`nx.DiGraph`): each package is a node; an edge A→B means "A
depends on B." Node attributes store repo, package type/manager, root component.

**Q: How do you compute "what depends on X" vs "what X depends on"?**
A: Forward dependencies = successors/descendants of X. Reverse dependents = the
predecessors/ancestors of X (traverse edges backward). NetworkX gives both.

**Q: How is transitive impact computed?**
A: The blast radius is the set of all nodes reachable in the reverse direction
(everything that transitively depends on X) — a graph traversal (BFS/DFS). It's
precomputed into an `impact_set` cache for speed.

**Q: How do you find if two packages are connected?**
A: `shortest_path` on the DiGraph; if it raises no-path, they're not connected in
that direction.

**Q: Complexity?**
A: Traversal is O(V+E). With ~1,000 nodes it's tiny; precomputation makes hot queries
near O(1) lookups.

**Q: How do you handle cycles?**
A: DiGraphs can have cycles; traversals use visited-sets to avoid infinite loops.
Topological ordering only applies to the acyclic parts.

---

## 14. Interview Q&A — Security

**Q: How is authentication handled?**
A: Honeywell ONE SSO via OAuth2/OIDC; after login the server issues a JWT (HS256,
8-hour expiry) stored in a cookie. Admin emails get elevated access. All SSO logic is
gated behind a flag so the app runs safely without it in dev.

**Q: How do you protect the LLM boundary?**
A: The Python pre-filter (jailbreak/injection/off-topic/identity regexes) blocks
malicious or out-of-scope input before the model call. Because it's code, prompt text
can't disable it.

**Q: OWASP concerns you handled?**
A: Injection (prompt injection filter + Pydantic validation), broken auth (JWT+SSO,
short expiry, secure cookie flag), secrets management (env vars, not committed),
and sensitive-data exposure (identity/off-topic filter refuses to reveal model info).

**Q: What about the `CHANGE_ME_INSECURE_DEFAULT` JWT secret?**
A: It's a dev fallback; in production `JWT_SECRET` must be set via env. Good example
of "secure by configuration" that I'd enforce with a startup check.

**Q: Rate limiting / abuse?**
A: Not implemented in the core; I'd add it (per-user/IP) at the reverse proxy (Nginx)
or in middleware — a known improvement.

---

## 15. Interview Q&A — Testing & Quality

**Q: How did you test it?**
A: A 77-case suite across 12 categories exercising every query type and dataset,
asserting correctness *and* markdown-format compliance. Result: 77/77 tests,
120/120 checks, 0 crashes, 0.26 s avg — captured in a production-readiness report.

**Q: How do you test something non-deterministic (an LLM)?**
A: Route factual queries through deterministic handlers so they're testable exactly;
for the LLM path, assert structure/format and use the graph context as ground truth
rather than exact wording.

**Q: What's missing test-wise?** → Unit tests per module, CI, load testing (Section 17).

---

## 16. Behavioral / STAR Stories

Use **Situation → Task → Action → Result**. Three ready stories:

**Story A — Preventing hallucinated answers (technical judgment).**
- S: Engineers needed trustworthy impact analysis; a pure LLM would hallucinate.
- T: Guarantee factual correctness while keeping a natural-language interface.
- A: Designed a hybrid architecture where a NetworkX graph computes facts and the
  LLM only interprets/phrases; added deterministic handlers that bypass the LLM.
- R: 100% correctness on a 77-case suite, 0.26 s avg latency, zero hallucinated
  dependency facts.

**Story B — Cross-dataset ambiguity (problem-solving).**
- S: The same name/path could mean two different things (engineering tool vs firmware).
- T: Avoid silently returning the wrong dataset's answer.
- A: Built an overlap index (1,068 paths, 35 names) that detects collisions and asks
  the user to clarify before answering.
- R: Eliminated a whole class of wrong answers; disambiguation cases pass 100%.

**Story C — Shipping without hard infra dependencies (pragmatism).**
- S: The app needed SSO and Redis in production but had to run on a laptop for dev.
- T: Don't force every developer to stand up Redis/SSO.
- A: Gated SSO behind a flag (no-op when disabled) and made the session store fall
  back to memory if Redis is unavailable.
- R: Frictionless local dev, production-ready when the infra is present.

---

## 17. Weaknesses & "What Would You Improve"

Being honest here scores points. Good answers:

- **Intent detection is regex-based** — brittle to unusual phrasing. I'd move to a
  small ML/embedding classifier with the regex as a fast path/fallback.
- **No per-module unit tests / CI** — the suite is integration-level. I'd add unit
  tests and a CI pipeline.
- **No rate limiting** on the LLM endpoint — I'd add per-user/IP limits.
- **Cache invalidation is manual** — a data change needs a rebuild; I'd automate
  hot-reload on new scans.
- **`api.py` is large (~3,500 LOC)** — I'd split routing, orchestration, and
  disambiguation into separate modules/routers.
- **Observability** — add structured logging, metrics, and tracing for LLM calls.
- **No vector/semantic fallback** for truly fuzzy questions — I'd add embedding
  retrieval for the questions the graph can't answer directly.

---

## 18. Interview Methods & How to Present

**How technical interviews typically probe a project:**
1. **Overview** ("tell me about a project") → give the 30-sec pitch, then pause.
2. **Deep dive** on one area they pick → be ready for architecture, a hard bug, or
   a design trade-off.
3. **"Why" laddering** → for every choice, know the alternative and the trade-off
   (Section 6 is built for this).
4. **Scaling / production** → Section 10.
5. **Weaknesses / next steps** → Section 17 (never say "nothing").

**Delivery tips:**
- **Lead with impact, then detail.** "It eliminates manual impact analysis" before
  "it uses NetworkX."
- **Use the STAR format** for behavioral questions; keep the R (result) quantified.
- **Draw the diagram** (Section 3) on the whiteboard — it anchors the whole convo.
- **Own your scope honestly.** Say clearly what you built vs. used a library for.
- **Have three "hooks"** ready that invite deeper questions: hybrid RAG,
  cross-dataset disambiguation, prompt-injection defense.
- **When you don't know**, say how you'd find out — reasoning beats bluffing.
- **Numbers you can defend** (Section 6 of RESUME_CONTENT.md) build credibility;
  never quote a metric you can't explain.

**Practice routine (spaced repetition):**
- Day 1: read this whole doc, say the pitch 5×.
- Day 2: answer Sections 8–10 out loud without looking.
- Day 3: whiteboard the architecture + request lifecycle from memory.
- Day 4: rehearse the three STAR stories and the "what would you improve" list.

---

## 19. Glossary

- **Dependency graph** — nodes = packages, directed edges = "depends on."
- **Transitive dependency** — an indirect dependency (A→B→C means A transitively
  depends on C).
- **Impact set / blast radius** — everything that (transitively) depends on a node;
  what could break if you change it.
- **Reverse dependents** — packages that depend *on* a given package.
- **RAG** — Retrieval-Augmented Generation.
- **Function/tool calling** — LLM returns structured JSON calls to named tools.
- **Prompt injection** — malicious input trying to override an LLM's instructions.
- **ClearCase** — legacy version-control system; here, source paths are mapped to
  Git packages/repos.
- **Conan / NuGet** — C/C++ and .NET package managers (the package "types").
- **SSE (Server-Sent Events)** — one-way server→browser streaming for live tokens.
- **OIDC / OAuth2** — the SSO login protocols.
- **JWT** — signed token proving an authenticated session.
- **ASGI** — async server interface (Uvicorn) that FastAPI runs on.
- **Graceful degradation** — the system still works (with reduced features) when an
  optional dependency (LLM/Redis/SSO) is unavailable.
