# NHI Governance Platform — Complete Study & Interview Guide

> One document to (1) put this project on your resume correctly, (2) understand every
> detail of the application, and (3) answer any question an interviewer can ask.
> Project: **AI-Powered Non-Human Identity (NHI) Governance Platform** (DevSecOps).

---

## PART 1 — RESUME (Research-Backed)

### How the resume is actually read (do this, in order)
1. **ATS parses first (98% of large firms).** It uses NLP + Named Entity Recognition to turn your resume into a searchable database, then recruiters **keyword-search** it (Boolean: `Python AND Azure NOT junior`). It does **not** auto-reject — it ranks by keyword match.
2. **Recruiter scans ~7 seconds.** Eyes hit the **top third**, **job titles**, and the **first word of each bullet**. They skim for **impact**, not duties.
3. **Interviewer later reads deeply** and asks you to defend every line. **Never write what you can't explain.**

### Rules that pass ATS + win the human
- Mirror the **exact keywords** from the job description (e.g., if they say "CI/CD", write "CI/CD" not "build pipelines").
- **Simple layout**: standard section headings, no tables/columns/text-boxes/graphics, one column. Save as **`.docx`** or a clean text-based **PDF**.
- **One line per bullet**, each = one achievement. Start with a **strong past-tense verb** (Built, Engineered, Designed, Automated, Integrated).
- Show **outcome/value**, not process. Keep it truthful.
- Put a **Skills line** with hard keywords so the search index finds you.

### Resume bullets (copy-ready — tight, no fluff)
**AI-Powered Non-Human Identity (NHI) Governance Platform — DevSecOps**
- Built an AI-driven DevSecOps platform that automates discovery, risk-scoring, and governance of non-human identities (GitHub PATs, deploy keys, Actions secrets, Azure service principals, SSL/TLS certificates).
- Engineered modular Python agents orchestrated by a LangChain GPT-4o agent for automated multi-cloud credential auditing.
- Designed a deterministic risk-scoring engine with GPT-4o explanations to prioritize and triage critical identity threats.
- Implemented policy-as-code rules (OPA/Rego-ready) enforcing credential rotation and least-privilege compliance.
- Automated CI/CD security pipelines in GitHub Actions with integrated SAST (Coverity) and SCA (Black Duck) scanning.
- Integrated real-time Slack, Jira, and email alerting to accelerate incident response on high-risk findings.
- Developed a React 19 + TypeScript dashboard with Recharts to visualize risk posture, inventory, and compliance trends.

**Skills line:** `Python · React · TypeScript · LangChain · GPT-4o · GitHub Actions · Azure · REST APIs · CI/CD · DevSecOps · Policy-as-Code · SAST/SCA · OWASP`

---

## PART 2 — APPLICATION DEEP-DIVE (know every detail)

### 2.1 The problem (why this exists)
Modern systems are run by **machines, not humans**. Every CI/CD pipeline, script, and cloud
workload authenticates with a **Non-Human Identity (NHI)** — a token, key, secret, or certificate.
NHIs now **outnumber human identities ~45:1**, are often **long-lived, over-privileged, and
unrotated**, and are a top breach vector. There was **no single pane** to see "which machine
credentials exist, how risky are they, and what do we do about them." This platform answers that.

### 2.2 What an NHI is (domain knowledge — expect this question)
| NHI type | What it is | Why risky |
|---|---|---|
| **PAT** (Personal Access Token) | GitHub token acting as a user | Broad scopes, often no expiry (classic) |
| **Deploy key** | SSH key giving a repo push/pull | Write keys = code injection risk |
| **Actions secret** | CI/CD secret (`${{ secrets.X }}`) | Rarely rotated, wide blast radius |
| **Azure Service Principal** | App identity in Azure AD | Client secrets/certs expire → outages or over-privilege |
| **SSL/TLS certificate** | Server identity | Expiry = outage; weak issuer = MITM |

### 2.3 What the platform does (one sentence)
**Discover → Score → Enrich → Report → Notify → Enforce**, on a schedule, across GitHub + Azure + SSL.

### 2.4 Architecture (high level)
```
GitHub Actions (scheduler/orchestrator)
        │
        ▼
Orchestrator ── dual mode:
  • Agent mode  = LangChain GPT-4o tool-calling agent (reasons over 4 tools)
  • Direct mode = deterministic Python calls (no LLM, no cost)
        │
  ┌─────┼─────────────────────────────────────┐
  ▼     ▼                 ▼                     ▼
cert_agent  github_nhi_agent  azure_sp_agent   (discovery)
  └─────┴─────────┬─────────┴─────────────────┘
                  ▼
           risk_scorer  (deterministic 0–100 + optional GPT-4o rationale)
                  ▼
           nhi_policies (7 rules R001–R007)
                  ▼
  ┌───────────────┼──────────────────────────┐
  ▼               ▼            ▼               ▼
report_generator  email    slack           jira
(HTML + JSON)   notifier  notifier        notifier
                  │
                  ▼
          React 19 dashboard (visualizes reports)
```

### 2.5 Tech stack & why
| Layer | Tech | Why |
|---|---|---|
| Backend agents | **Python 3.11** | Best cloud SDK + scripting ecosystem |
| AI orchestration | **LangChain + OpenAI GPT-4o** | Tool-calling agent + natural-language risk rationale |
| GitHub API | **`requests`** (thin client, not PyGithub) | Need raw headers (`X-OAuth-Scopes`) PyGithub hides |
| Config | **PyYAML** | Central `config.yaml`, no hard-coded thresholds |
| Frontend | **React 19 + TypeScript + Vite** | Fast SPA, type safety, HMR |
| Charts | **Recharts** + **Lucide** icons | Declarative visualizations |
| CI/CD | **GitHub Actions** (self-hosted + `ubuntu-latest`) | Native to where the code lives |
| Security scans | **Coverity (SAST)** + **Black Duck (SCA)** | Static + dependency vulnerability gates |

### 2.6 Component-by-component (what each does — be able to explain any)

**`cert_agent.py` — SSL/TLS checker**
- Pure stdlib `ssl` + `socket` (no `openssl` subprocess) → portable, no shell dependency.
- Connects, pulls the peer cert, parses `notBefore`/`notAfter`, computes `days_left`.
- Status tiers: `OK (>60d) → WARNING (≤60d) → CRITICAL (≤30d) → EXPIRED (<0) → ERROR`.
- Supports `host`, `host:port`, and dict entries with custom **SNI**. All errors non-blocking.

**`github_nhi_agent.py` — GitHub NHI discovery**
- Custom `GitHubClient` (Bearer auth, API `2022-11-28`, pagination).
- Discovers: **PAT** scopes (from `X-OAuth-Scopes` header on `/user`), **deploy keys**, **Actions secrets**, and **workflow secret refs** (regex `\$\{\{\s*secrets\.([A-Za-z_]\w*)\s*\}\}` over `.github/workflows/*.yml`).
- Clever detail: **fine-grained PATs return an empty scopes header**, classic PATs list scopes → that's how token type is detected.

**`azure_sp_agent.py` — Azure Service Principals**
- 3-tier fallback: **Azure SDK (`DefaultAzureCredential`) → Azure CLI (`az ad sp ...`) → silent skip**.
- Discovers SPs + client secrets + certs and their expiry; output shape matches GitHub findings for unified scoring.

**`risk_scorer.py` — the brain (deterministic 0–100)**
- `SCORE = Type weight (30) + Age (40) + Privilege (20) + SSL severity (10)`, capped at 100.
- Type weights: PAT 30, Deploy-Key-Write 30, Deploy-Key-Read 18, Actions-Secret 14, SSL 10.
- Age is **linear**: `min(40, 40 × age/threshold)`.
- Severity: `≥70 CRITICAL · ≥50 HIGH · ≥30 MEDIUM · else LOW`.
- **LLM enrichment is an optional overlay**: only findings `score ≥ llm_min_score (50)` get one GPT-4o sentence; skipped if no API key or disabled. **Score is always deterministic** (reproducible, no AI cost required).

**`nhi_policies.py` — policy-as-code engine**
- 7 named rule functions returning a `PolicyResult` dataclass; structured for a 1:1 **OPA/Rego** port.
- `R001` classic-PAT write-scope rotation · `R002` write deploy-key age · `R003` Actions-secret staleness · `R004` cert critical expiry · `R005` cert warning expiry · `R006` PAT `admin:org` = HIGH · `R007` any `score ≥ 70` = immediate action.
- Rules run over every finding; only violations returned, sorted by severity.

**`orchestrator/nhi_orchestrator.py` — conductor**
- **Agent mode:** builds 4 LangChain `@tool`s (ssl, github, scoring, policy) and a GPT-4o `AgentExecutor` (system prompt forces ordered tool calls → returns structured JSON).
- **Direct mode (`--no-llm`):** calls each module in sequence — fully deterministic.
- Saves JSON inventory + HTML report, fires notifications, and **exits non-zero on critical violations** (fails the CI job → visible signal).

**Notifiers** — `email_notifier` (SMTP/TLS, multipart + HTML report attachment), `slack_notifier` (Block Kit, emoji severity, rate-limited), `jira_notifier` (REST API v3, tickets only for CRITICAL/HIGH). All **optional + non-blocking** (missing creds = skip).

**`report_generator.py`** — styled HTML dashboard (risk posture cards, policy violations, SSL table, NHI table, workflow secret refs) + **report rotation** (keeps last 15).

**Dashboard (`dashboard/`)** — React 19 SPA: `App.tsx` sidebar nav + toast system; pages: Overview, NHI Inventory (Identities), Certificates, Policy Engine, Settings, plus Alerts/Audit Log/ML Analytics/Scans. Charts via Recharts. Currently reads typed mock data (`mockData.ts`, `types.ts`) — a real API swap is the next step.

### 2.7 Key design decisions & trade-offs (interviewers love these)
| Decision | Why | Trade-off |
|---|---|---|
| Deterministic score, LLM only as overlay | Reproducible, auditable, zero-cost mode | LLM adds nuance but never changes the number |
| Modular independent agents | Add/replace a cloud without touching others | More files vs. a monolith |
| Graceful degradation (skip on missing creds) | One agent's failure can't break the run | Silent skips need good logging |
| Central `config.yaml`, env overrides | No hard-coded secrets/thresholds | Must document all keys |
| Thin `requests` GitHub client | Access raw headers PyGithub hides | Reimplement pagination |
| Policy rules as Python functions | Readable now, portable to OPA/Rego later | Not yet a formal policy language |

### 2.8 Security posture (OWASP-aware)
- **Zero hard-coded secrets** — all via env vars / GitHub Secrets.
- **Least privilege** — workflow token is `contents: read`, `actions: read`.
- **TLS enforced** for email; **path validation** on attachments (no traversal).
- **SAST + SCA** gates in CI; secrets never logged.
- The tool itself hunts the exact risks it must avoid (over-privileged, stale, unrotated creds).

---

## PART 3 — SDLC: PLANNING, PHASES, DEVELOPMENT & TESTING

### 3.1 Planning & phased delivery
1. **Phase 1 – Discovery MVP:** SSL cert checker (replaced a legacy bash script with a Python-native agent).
2. **Phase 2 – GitHub NHIs:** PAT/deploy-key/secret discovery + workflow secret parsing.
3. **Phase 3 – Scoring & Policy:** unified 0–100 engine + 7 policy rules.
4. **Phase 4 – Multi-cloud:** Azure Service Principals with SDK→CLI fallback.
5. **Phase 5 – AI overlay:** LangChain GPT-4o orchestration + risk rationale.
6. **Phase 6 – Notify & Report:** HTML/JSON + Email/Slack/Jira.
7. **Phase 7 – Visualize:** React dashboard.
8. **Phase 8 – Automate:** GitHub Actions schedules + security-scan gates.

### 3.2 Development strategy
- **Separation of concerns:** discover / score / enforce / report / notify are independent modules.
- **Interface contract:** every agent returns the **same finding shape** → the scorer/report treat all NHIs uniformly.
- **Config-driven, not code-driven** thresholds.
- **Fail loud in CI, fail silent per-agent** — non-zero exit on critical, but one agent error never aborts the audit.

### 3.3 Testing strategy (how you'd prove it works)
- **Unit tests:** scoring math (age boundaries at 0 / threshold / >threshold), severity cutoffs (69 vs 70), each policy rule true/false path, cert status tiering, PAT classic-vs-fine-grained detection.
- **Mock external APIs:** `requests-mock`/`responses` for GitHub; fake TLS server or recorded certs for SSL; mock Azure SDK/CLI.
- **Integration:** run orchestrator in `--no-llm` mode end-to-end on fixtures → assert JSON inventory + HTML generated + correct exit code.
- **LLM isolation:** enrichment mocked (deterministic core stays testable without OpenAI).
- **Frontend:** component tests (React Testing Library) + type checks (`tsc`) + ESLint.
- **Security:** Coverity (SAST) + Black Duck (SCA) in the pipeline.
- **Edge cases:** expired cert, unreachable host, 403 on secrets, empty repo list, missing every optional credential.

### 3.4 Deployment
- **GitHub Actions** on schedule: NHI Governance (weekly Mon 00:00 UTC), Cert check (weekly), Azure SP (daily), plus a build/quality/security pipeline. Reports uploaded as **90-day artifacts**. Manual `workflow_dispatch` with `no_llm` / `skip_github` toggles.

---

## PART 4 — INTERVIEW PREP (every angle)

### 4.1 How to talk about the project (method)
- **STAR(R):** Situation → Task → Action → Result → Reflection. Prepare this project as one of your 3–5 core stories; interviewers reuse it for many questions.
- **60-second pitch (memorize the shape, not the words):**
  > "NHIs — machine credentials like tokens, keys, and certs — now outnumber humans and are a top breach vector, but there was no single view of them. I built a DevSecOps platform that **discovers** NHIs across GitHub and Azure, **scores** each 0–100 with a deterministic engine, optionally **explains** the risk with GPT-4o, then **reports and alerts** via Slack/Jira/email — all automated on GitHub Actions with SAST/SCA gates and a React dashboard. Result: continuous, prioritized visibility into machine-identity risk."
- **Tell-me-about-yourself → project → conflict** are the "Big Three"; have crisp answers ready.

### 4.2 Technical Q&A (core)
- **Why deterministic scoring if you have an LLM?** Reproducibility, auditability, no cost/latency, no hallucination. LLM only *explains*, never decides.
- **How is the score computed?** Type + Age (linear to threshold) + Privilege + SSL severity, capped 100; thresholds from `config.yaml`.
- **How do you detect a PAT's type?** Classic PATs list scopes in `X-OAuth-Scopes`; fine-grained PATs return that header empty.
- **Why raw `requests` over PyGithub?** Need raw response headers (scopes) and full control of pagination; PyGithub abstracts them away.
- **How do you find secrets used in workflows?** Regex over `.github/workflows/*.yml` for `${{ secrets.X }}`.
- **What if OpenAI/GitHub/Azure is down or unconfigured?** Graceful degradation — that agent skips, the rest still run; orchestrator falls back to direct mode without a key.
- **Where do secrets live?** Env vars / GitHub Secrets only — nothing hard-coded, nothing logged.
- **Why does the job exit non-zero on critical?** So CI visibly fails and forces action.
- **LangChain role?** Wraps the 4 agents as tools; a GPT-4o agent reasons and calls them in order, returning structured JSON.

### 4.3 System-design / scaling questions
- **Scale to an org (thousands of repos)?** Async/concurrent discovery (`asyncio`/thread pool), respect GitHub rate limits (ETags, backoff, conditional requests), shard by repo, cache unchanged results.
- **Persist history?** Move from flat JSON to a DB (Postgres) keyed by finding ID + timestamp → real trend lines and drift detection.
- **Real-time vs. batch?** Add webhooks (new deploy key/secret) for event-driven scans alongside the schedule.
- **Multi-tenant?** Per-tenant config + isolated credentials + row-level scoping.
- **Add AWS/GCP?** Implement a new discovery agent emitting the same finding shape — scorer/policy/report unchanged.

### 4.4 Security-specific questions
- **Biggest risk you monitor?** Over-privileged, long-lived, unrotated write credentials (PAT `admin:org`, write deploy keys).
- **OWASP relevance?** A01 Broken Access Control, A02 Cryptographic Failures (cert expiry), A05 Misconfiguration, A07 Auth failures, A09 Logging/Monitoring — the tool is a control for these.
- **How prevent the tool being an attack surface?** Read-only scopes, least-privilege workflow token, no secret logging, SAST/SCA, TLS, attachment path validation.

### 4.5 Frontend questions
- **Why React 19 + Vite + TS?** Fast HMR, type-safe finding/violation models (`types.ts`), component reuse.
- **State today?** Local `useState` + typed mock data; production step = fetch the JSON inventory from an API and render live.
- **Charts?** Recharts (line/radar/bar) for risk trend, posture radar, severity distribution.

### 4.6 Behavioral (STAR) stories from this project
- **Ambiguity:** "No off-the-shelf NHI scoring existed, so I designed a transparent additive 0–100 model from policy thresholds and validated it on boundary cases."
- **Trade-off/decision:** "Chose deterministic scoring with an optional LLM overlay so results stay auditable and cheap — LLM adds explanation, not the number."
- **Resilience:** "Made every integration optional and non-blocking so one missing credential can't break the whole audit."
- **Learning:** "Discovered classic vs. fine-grained PATs behave differently in the scopes header — I read the raw REST headers to detect it reliably."

### 4.7 "What would you improve?" (show self-awareness)
- Wire the dashboard to a **live API + database** (replace mock data).
- Add **automated tests + coverage gates** and a **CI test job**.
- **Async** discovery + rate-limit handling for org scale.
- Port policy rules to **OPA/Rego** for a formal policy engine.
- Add **auto-remediation** (open PRs to rotate/revoke) behind approvals.
- **Secrets scanning** (detect leaked tokens in code), not just inventory.

### 4.8 Curveballs (quick honest answers)
- **Can the LLM be wrong?** Yes — that's why it never sets the score; it only annotates.
- **Why not Splunk/CSPM?** Those are broad; this is **NHI-specific**, developer-owned, and free to run in CI.
- **Cost?** Deterministic mode is $0; LLM only fires on high-risk findings above a score gate.
- **Biggest challenge?** Designing one finding schema that unifies wildly different identity types so scoring/reporting stay simple.
- **What did you own end-to-end?** Discovery agents, scoring/policy engine, orchestration, notifications, CI/CD, and the dashboard.

---

### One-line summary to leave them with
> "It turns invisible, unmanaged machine credentials into a **scored, prioritized, automated** governance workflow — deterministic at its core, AI-assisted at the edges, and shipped through secure CI/CD."
