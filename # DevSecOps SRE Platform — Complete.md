# DevSecOps SRE Platform — Complete Study & Interview Guide

> One project, six deployable services, three integrated dashboards **plus an Engineering
> Intelligence (Unified Ops) module**. This guide is your single source of truth for the
> resume, the architecture, the "why", and every angle an interviewer can attack.

---

## 0. How to use this guide

1. **Memorize Section 1** (elevator pitch + numbers). You will repeat these in every interview.
2. **Section 2** = resume bullets (copy-paste, per role). Unified Ops bullets included.
3. **Sections 3–8** = deep knowledge so you can defend any bullet.
4. **Sections 9–11** = interview Q&A, system-design, behavioral/STAR.
5. **Section 12** = 1-page cheat sheet to skim 10 minutes before the call.

**Golden rule:** never claim a line you can't explain for 2 minutes. Every bullet below maps
to a real subsystem in Sections 3–8.

---

## 1. The 60-Second Elevator Pitch

> "I designed and built a production-grade **DevSecOps SRE platform** used to monitor a fleet of
> **400+ GitHub self-hosted runners across ~359 machines**. It's a full-stack system — **four
> React/TypeScript single-page apps** and **two async FastAPI (Python) backends** on
> **PostgreSQL and Prometheus**, behind **Nginx with TLS**. It unifies five capabilities:
> (1) **runner & host monitoring** with a novel runner→machine metric correlation, (2) a
> stateful **alerting engine** with email digests, (3) an **engineering-maturity** tracker with
> Excel-driven CI/CD scoring and an approval workflow, (4) **GitHub repository analytics** for
> PR cycle time and Copilot adoption, and (5) **Unified Ops / Engineering Intelligence**, which
> ingests security, code-quality and build-reliability metrics from five enterprise vendor tools
> — Coverity, Black Duck, SonarQube, GitHub and tech-stack — into one RBAC-scoped posture view.
> The whole thing has **SSO/JWT auth, role-based access control across 6–7 roles**, and
> **~139,000 lines of code**."

**Numbers to memorize**

| Metric | Value |
|--------|-------|
| Deployable services | 6 (2 backends, 4 SPAs) + Nginx + PostgreSQL + Prometheus |
| REST endpoints | 200+ across 24 routers (runner) + ~90 routes (maturity) |
| Runners / machines monitored | 400+ runners / ~359 machines (org HON-IA) |
| Vendor tools ingested (Unified Ops) | 5 — Coverity, Black Duck, SonarQube, GitHub, Tech-stack |
| Roles | 6 (runner) + 7 (maturity) |
| Codebase | ~48K LOC Python + ~91K LOC TypeScript ≈ 139K LOC |
| Databases | PostgreSQL (`public`, `maturity`, `unifiedops` schemas) + Prometheus TSDB |

---

## 2. Resume Section

### 2.1 Project title line

> **DevSecOps SRE Monitoring & Engineering-Intelligence Platform** — Full-Stack Engineer
> *React 18 · TypeScript · Python · FastAPI · PostgreSQL · Prometheus · Nginx · Docker · Ansible*

### 2.2 Master bullet list (pick 5–7 for a resume; all are defensible)

**Platform & architecture**
- Architected a production **DevSecOps SRE platform** — **4 React/TypeScript SPAs + 2 async
  FastAPI backends** on **PostgreSQL + Prometheus**, exposing **200+ REST endpoints across 24
  routers**, deployed behind **Nginx (TLS 1.2/1.3, HSTS, CSP, rate limiting)** and Docker Compose
  (~**139K LOC**).

**Runner / host monitoring**
- Built a **runner→host correlation engine** mapping a GitHub runner → Windows service →
  live `windows_exporter`/`node_exporter` metrics (CPU/RAM/disk), monitoring **400+ runners
  across ~359 machines** — health the GitHub API alone cannot provide.

**Alerting**
- Engineered a **stateful alerting engine** (threshold-watch sessions with hysteresis, no-data
  grace windows, per-rule digest batching, daily caps, master + per-admin email kill-switches)
  delivering scoped SMTP alerts without flapping or duplicate emails.

**Engineering maturity**
- Delivered an **engineering-maturity system** — monthly CI/CD submissions, an L1 approval
  workflow, and a **fuzzy Excel/CSV import** (multi-row headers, release-version dedup,
  auto-entity creation) feeding analytics heatmaps, trends, and Excel/PDF/CSV export.

**Unified Ops / Engineering Intelligence** ⭐ (the piece to emphasize)
- Built **Unified Ops ("Engineering Intelligence")** — a data-ingestion subsystem that pulls
  **security, code-quality, and build-reliability metrics from 5 enterprise vendor tools
  (Coverity, Black Duck, SonarQube, GitHub, tech-stack)** via their REST APIs into an **isolated
  `unifiedops` PostgreSQL schema**, and surfaces a single **RBAC-scoped posture dashboard**
  (Overview / Security / Quality / Build / Tech-stack).
- Designed a **config-driven ingestion pipeline**: seed JSON → parsed `project_config` (source of
  truth) → per-project vendor fetchers → idempotent **upserts (`ON CONFLICT DO UPDATE`)** into
  time-series tables, scheduled **daily via APScheduler** with an admin **"Refresh now"** path and
  per-source run tracking (`ingest_runs`).
- Implemented **multi-protocol vendor auth** — HTTP Basic over corporate **AD/LDAP** (Coverity,
  SonarQube), **bearer token** (Black Duck), and **PAT** (GitHub) — with credentials isolated in
  env config and lazily cached.
- Enforced **deny-by-default RBAC** across all Unified Ops reads (scoped by repository name;
  no-scope users match nothing — *fails closed*), verified with a dedicated security test harness.
- Added a **name-based cross-link** fusing Unified Ops programs with maturity projects
  (normalization + alias map) so a program's security/quality posture links to its maturity score.

**Repository analytics**
- Developed a **GitHub repository-analytics service** (PR cycle time, review engagement, Copilot
  adoption %, branch analysis) using a **disk-backed, shape-versioned stale-while-revalidate
  cache** with period-containment derivation to respect GitHub API rate limits.

**Security / RBAC / Auth**
- Implemented **SSO + JWT auth** (OAuth2 callback, httpOnly cookies, bcrypt) with **row-level
  RBAC** across program/machine/entity scopes and **6–7 roles**, hardened against account-takeover
  and OWASP Top-10 issues.

**Ops / automation**
- Automated remediation via an **Ansible action worker** (dry-run + risk gating) with **live
  Server-Sent-Events log streaming**, plus background collectors (15-min metric snapshots, runner
  polling) and scheduled retention jobs.

### 2.3 Two-line compact version (space-constrained resume)

> **DevSecOps SRE & Engineering-Intelligence Platform** — Full-stack monitoring & analytics
> platform (React/TS · FastAPI · PostgreSQL · Prometheus · Nginx). Built 200+ REST endpoints,
> runner→host metric correlation for 400+ runners, a stateful alerting engine, SSO/RBAC (7 roles),
> Excel-driven maturity tracking, GitHub PR/Copilot analytics, and **Unified Ops** — a
> multi-vendor (Coverity/Black Duck/SonarQube/GitHub) security & quality ingestion pipeline with
> deny-by-default RBAC.

### 2.4 Skills matrix (resume "Skills" line)

| Area | Technologies |
|------|-------------|
| Backend | Python 3.11, FastAPI, SQLAlchemy 2.0 (async), asyncpg, Pydantic v2, PyJWT, bcrypt, httpx, APScheduler |
| Frontend | React 18, TypeScript (strict), Vite, Tailwind CSS v3/v4, Recharts, TanStack Query, React Hook Form, Zod |
| Data/Infra | PostgreSQL (multi-schema), Prometheus/PromQL, node/process/windows exporters, Nginx, Docker, Ansible |
| Integrations | GitHub REST API, Coverity, Black Duck, SonarQube (LDAP), SMTP, OAuth2 SSO |
| Practices | Async/concurrent design, RBAC, caching & rate-limit strategy, ETL/ingestion, OWASP-aware security, TDD |

---

## 3. Complete System Architecture

### 3.1 Service map

```
                                   Nginx (:443 HTTPS, TLS1.2/1.3, HSTS, CSP, rate-limit)
                                             │
        ┌──────────────┬─────────────────────┼──────────────────────┬─────────────────────┐
        │ /            │ /maturity/          │ /repo-analytics/     │ /admin/             │
   Runner SPA     Engineering-Intel SPA   Repo-Analytics SPA     Admin Console SPA
  (React18/Vite8) (React18/Vite5)         (React18/Vite6)        (React18/Vite6)
        │              │  ▲                    │                     │
        │  /api/       │  │ /maturity-api/     │ /maturity-api/      │ /api/ + /maturity-api/
        ▼              ▼  │                    ▼                     ▼
  Main FastAPI (:8000) ── │ ─────────────  Maturity FastAPI (:8001) ────────────────────
  24 routers, 200+ eps    │                7 routers + Unified Ops + vendor-mappings
  runner/alert/ITSM/RBAC  │                maturity submissions, analytics, exports,
        │                 │                Unified Ops ingestion + reads
        ▼                 │                        │
  PostgreSQL `public`     │                 PostgreSQL `maturity` + `unifiedops` schemas
        │                 │                        │
        ▼                 │                        ├── vendor REST APIs (Coverity/BlackDuck/
  Prometheus (:9090) ─────┘                        │   SonarQube/GitHub) via httpx
   ├ node_exporter (:9100)  Linux host             └── APScheduler daily ingest (02:20)
   ├ process_exporter (:9256)
   └ windows_exporter (:9182) Windows hosts/runners
```

### 3.2 The two backends (why two?)

| | Main backend (`:8000`) | Maturity backend (`:8001`) |
|---|---|---|
| Framework | FastAPI + async SQLAlchemy ORM models | FastAPI + **raw SQL via `text()`** (reads existing tables) |
| Reload | `uvicorn --reload` | **no reload** (restart to load code) |
| Schema | PostgreSQL `public` | PostgreSQL `maturity` + `unifiedops` |
| Owns | runners, alerts, ITSM, machines, auth/SSO, notifications | maturity submissions, analytics, exports, **Unified Ops**, vendor mappings |
| Auth | issues the JWT (SSO + local) | **bridges** the same JWT cookie (shared SSO) |

**Talking point:** two processes, one identity. The maturity backend trusts the JWT minted by the
main backend (shared secret) — a lightweight SSO bridge instead of a second identity provider.

### 3.3 Data stores

- **PostgreSQL** — 3 logical schemas: `public` (runner/alert/ITSM/users), `maturity`
  (BU→GBE→Project→Component hierarchy + submissions), `unifiedops` (vendor metrics, isolated
  `MetaData(schema="unifiedops")` so its DDL never touches maturity tables).
- **Prometheus TSDB** — live time-series scraped from exporters; queried via PromQL over httpx.
- **Disk cache** — repo-analytics stale-while-revalidate cache (shape-versioned keys).

---

## 4. Component Deep-Dives

### 4.1 Runner & Host Monitoring (main backend)
- **Collector**: background thread polls the GitHub API (`/orgs/{org}/actions/runners` + runner
  groups) every cycle; first cycle over 404 runners / 54 groups takes ~10–15s.
- **Correlation engine (the standout feature)**: runner name `HON-RUNNER-01` → Windows service
  `actions.runner.<org>.HON-RUNNER-01` → which `windows_exporter` job hosts it → live host CPU/RAM/
  disk. Matching is done **in Python** (not PromQL regex) to dodge RE2 limitations.
- **Snapshots**: a 15-min asyncio task stores metric snapshots in PostgreSQL for trend/history.
- **Prometheus file_sd**: backend writes target JSON files so Prometheus auto-discovers runners.
- **Gotcha to mention**: "0 runners" was a **token permission** issue (fine-grained PAT missing
  *Self-hosted runners: Read*), not a code bug — I added a diagnostic that classifies 401 vs 403 vs
  rate-limit and surfaces the real reason in the UI.

### 4.2 Alerting Engine (main backend, `snapshot_collector.py`)
- **Single email authority**: `_evaluate_threshold_watch_sessions` is the *only* function that
  emails; a separate evaluator records history/auto-resolves. (Avoids double-send.)
- **Stateful sessions**: a breach opens a `ThresholdWatchSession`; email fires only after the
  breach holds for a **monitor window** and only once (`email_sent`).
- **Hysteresis**: session resolves only after the metric clears the threshold by a recovery margin
  (anti-flap).
- **No-data grace**: a missed scrape doesn't instantly resolve — time-based grace via
  `last_checked_at`.
- **Digest batching**: all machines of one rule crossing the window in a cycle → **one** email
  (RBAC-safe: batching is per-rule, so audiences never mix). Daily cap counts *distinct* email
  events.
- **Kill switches**: master `alert_emails_enabled` / `approval_emails_enabled` + per-admin opt-out,
  enforced at single choke points and refreshed every cycle.

### 4.3 Engineering Maturity (maturity backend)
- **Hierarchy**: Business Unit → GBE → Project → Component; monthly submissions store CI/CD metric
  scores (0/5/10/NA) + coverage + build times.
- **Scoring**: `SUM(scores) / (COUNT(non-NA) * 10)` → percentage (mirrors the Excel formula).
- **Approval workflow**: DRAFT → PENDING → APPROVED/REJECTED (single-level L1); draft privacy
  (private to author); bulk approve/reject/submit/cancel; full history table.
- **Excel/CSV import**: fuzzy 4-tier project/component matching, **multi-row split-header**
  assembly, release-version normalization/dedup (`R920.4/R930.2`), auto-create missing entities,
  dry-run preview.
- **RBAC**: everyone reads; writes gated by role; **row-level scope filter** so a project admin
  only sees their projects.

### 4.4 Repository Analytics (maturity backend + repo-analytics SPA)
- **Metrics**: PR cycle time, merge rate, review engagement, **Copilot comment/adoption %**,
  branch analysis (stale/active/protected), language mix.
- **Caching**: disk + DB + in-memory tiers; **shape-versioned keys** (`_SHAPE`) so a payload change
  can't serve stale-shape data to new UI; **period containment** (a 365-day result is a superset →
  derive 30/60/90 client-side, zero network).
- **Rate-limit discipline**: GitHub failures **fail loud, never cache empty**; a per-PR memo
  (content-addressed by `updated_at`) means a 365-day compute reuses a prior 90-day compute and only
  fetches the delta.

### 4.5 ⭐ Unified Ops / Engineering Intelligence (maturity backend `unifiedops/`)

**Purpose** — Give leadership a *single* engineering-intelligence view: security posture, code
quality, and build reliability across the whole software portfolio, pulled automatically from the
enterprise vendor tools teams already use, and scoped by RBAC so each user only sees their repos.

**The 5 data sources**

| Source | Tool | What it brings | Auth |
|--------|------|----------------|------|
| `coverity` | Coverity | Static-analysis outstanding issues (high/med/low, new/fixed/triaged/outstanding) | HTTP Basic (AD/LDAP) |
| `blackduck` | Black Duck | Open-source/SCA risk profile (critical/high vulns) | Bearer token |
| `sonar` | SonarQube | Coverage, quality-gate (derived Pass/Fail), metrics | HTTP Basic (AD/LDAP) |
| `build_ratio` | GitHub | Build reliability ratio from workflow runs | PAT |
| `techstack`/`languages` | GitHub | Language composition per repo | PAT |

**Ingestion pipeline (config-driven ETL)**

```
json_ingestion/*.json  (seed reference: 5 vendor shapes)
        │  config_seed.py parses each shape
        ▼
unifiedops.project_config   ← SOURCE OF TRUTH (per-project: vendor name/id, repo, branches,
        │                      group/lob/gbe/sbg, component_id, is_active)
        │  ingest.py: load_active_config()  → per row
        ▼
fetchers/*.py  (coverity/blackduck/sonar/github_build_ratio/languages)  → vendor REST via httpx
        │  idempotent  pg_insert(...).on_conflict_do_update
        ▼
unifiedops metric tables (time-series)         + ingest_runs (per-source run log)
        ▲
   APScheduler daily @ 02:20   OR   admin "Refresh now"  (POST /unified-ops/refresh[/{source}])
```

**Schema (isolated `unifiedops` schema, SQLAlchemy Core)** — 9 tables:
`coverity_data`, `blackduck_risk_profiles`, `sonar_metrics`, `github_metrics`, `languages`,
`project_groups`, `project_config` (source of truth), `component_vendor_mappings`, `ingest_runs`.

**Module layout (`fastapi_backend/unifiedops/`)**
- `settings.py` — env-driven, lru-cached vendor credentials/URLs (`has_*_creds`).
- `config_seed.py` — parses the 5 JSON shapes into config rows.
- `mappings.py` / `backfill_mappings.py` — materialize component↔vendor mappings.
- `fetchers/` — one per vendor (`base.py` shared).
- `ingest.py` — `REGISTRY`, `ALL_SOURCES`, `run_source`, `run_all`.
- `scheduler.py` — APScheduler start/shutdown (daily 02:20).
- `store.py` — `latest_runs`, `load_active_config`.
- `catalog.py` — proxy to vendor catalogs so the mapping UI can search & auto-fill project IDs.
- `http_client.py`, `schema.py`, `bootstrap.py`, `errors.py`.

**API surface** (`/api/unified-ops/…`, dual-slash, envelope responses)
- Operational: `GET /status`, `GET /sources` (admin), `POST /refresh/{source}`, `POST /refresh`.
- RBAC-scoped reads: `GET /coverity`, `/blackduck`, `/sonar`, `/build-ratio`, `/techstack`,
  `/trends`, `/hierarchy`, `/overview`.
- Vendor mappings (`vendor_mappings.py`): `GET/PUT/DELETE /components/{cid}/vendor-mappings`,
  `GET /unified-ops/catalog/{vendor}/projects|versions`, `GET /unified-ops/mappings/coverage`.

**RBAC — deny-by-default / fail-closed**
- `_scope(db, user)` reuses `repo_analytics._fetch_scoped_repo_rows` → `{global, bare, full,
  components}`. Global/local admins see everything; scoped users are filtered by **repo name**
  (`LOWER(col) IN bare_names`, else `AND 1=0` → matches nothing).
- Sonar has no repo name in its feed, so it's scoped via `project_config.component_id` — until an
  admin links a component, Sonar is **invisible to scoped users** (secure by default, not a bug).
- Verified with a dedicated harness (`_uo_rbac_test.py`): scoped users get a strict subset of admin;
  unauth → 401.

**Hierarchy reality (be precise in interview)** — every row carries `project_sbg` (constant `IA`),
`project_gbe` (constant `HPS`), and the only varying level `project_group` (~16 programs: DP, CT,
IFS, ExpOA, Trace, FieldPKS, PMD, AMT, …). So all rollups group by **`project_group`** ("program").

**Frontend** — 5 pages under `src/pages/unified-ops/`: **Overview, Security, Quality, Build,
TechStack**, sharing `UnifiedOpsShared.tsx` (grouping helpers `programOf/programsOf`, cross-link
`matchMaturityProject`) and `UnifiedOpsCharts.tsx` (history-aware `TrendPanel`).

**Cross-domain link** — Unified Ops (SBG=IA/GBE=HPS/program) and maturity (BU=PA/GBE=LSS) are
disjoint label-spaces; they're joined **only by repo name** (or by a name/alias map on the program
→ maturity project), so a program's security/quality posture links to its maturity assessment.

**Honest edges I can discuss** — 3 of 4 vendor feeds are single-snapshot today, so trend charts
show "only one snapshot so far" rather than a fake line; there's no config-CRUD API yet (adding a
row + re-ingest), which is the natural next enhancement.

### 4.6 Admin Console SPA (`admin/`)
- Standalone Vite6/React18/Tailwind-v4 app; cross-app administration: approvals + incidents,
  notifications, activity/audit, users & access (RBAC ScopePicker with derived program/machine
  levels), projects & mapping (incl. project merge), configuration (email switches), help centre.
- Stale-while-revalidate client cache; server + client pagination; presence/heartbeat.

### 4.7 Auth & RBAC (cross-cutting)
- JWT (HS256) in httpOnly cookie + body; bcrypt (direct, not passlib); 8-hour expiry.
- **Runner roles**: local_admin, global_admin, global_user, program_admin, program/project_user,
  pending.
- **Maturity roles**: VIEWER → DATA_ENTRY → APPROVER → PROJECT_ADMIN → GBE_ADMIN → BU_ADMIN →
  SUPER_ADMIN; runner roles map into these on login.
- **Security fix I can tell as a story**: an SSO account-takeover where email fallback matched a
  local account across providers — fixed by requiring `auth_provider='sso'` on all SSO lookups.

---

## 5. Purpose & Problem Statement (the "why")

- **Problem**: SRE/DevSecOps signal was fragmented — GitHub showed runner *status* but not host
  *health*; maturity was tracked in spreadsheets; security/quality/build metrics lived in five
  separate vendor portals; access wasn't scoped.
- **Solution**: one platform that (a) correlates runner status with real host metrics, (b) alerts
  on breaches without flapping, (c) digitizes maturity with an approval workflow, (d) analyzes PR
  throughput and Copilot adoption, and (e) **via Unified Ops, auto-ingests and unifies
  security/quality/build posture from all vendor tools** — all behind SSO + RBAC.
- **Impact framing**: replaced manual spreadsheet + multi-portal checking with a single
  automated, access-controlled pane of glass; near-real-time host health; audit trails everywhere.

---

## 6. Development Strategies & Patterns

- **Config-driven ETL** (Unified Ops): JSON seed → `project_config` source of truth → idempotent
  upserts. Adding a vendor project is data, not code.
- **Idempotency**: `ON CONFLICT DO UPDATE` everywhere in ingestion; re-running is safe.
- **Fail-closed security**: no-scope users match nothing (`AND 1=0`); GitHub/vendor failures never
  cache empty.
- **Cache-shape versioning**: bump `_SHAPE` when payload changes so old-shape data can't crash new
  UI (a real bug I fixed).
- **Single source of authority**: one email function, one active-chain filter, one cache-key
  builder — avoids divergent duplicate logic.
- **Schema isolation**: `unifiedops` `MetaData` is private, so its `create_all` never touches
  maturity tables.
- **Background work off the request path**: collectors, snapshot task, ingestion scheduler, action
  worker all run async/threaded so requests stay fast.
- **Graceful degradation**: many endpoints wrap DB queries in try/except and return safe fallbacks
  (zeros/empty) instead of 500s.

---

## 7. Testing Strategy (every angle)

| Layer | How it's tested |
|-------|-----------------|
| Unit (pure logic) | pytest for import parsing, release normalization, cache-key builders, scoring, month parsing |
| RBAC / security | dedicated harnesses (`_uo_rbac_test.py`, `_rbac_test.py`) assert scoped users get a strict subset; unauth → 401; deny-by-default |
| Integration (DB) | rolled-back transactions verify upserts/materialization (e.g. Sonar baseline inheritance) against real Postgres |
| API contract | unauth probe: 401 = route exists + RBAC; 404/405 = not loaded (used to catch stale uvicorn workers) |
| Frontend E2E | Playwright with `page.route` mocks; verify instant branch switch hits `/pr-analytics` exactly once; RBAC banners; dark-mode |
| Ingestion validation | CLI `python -m unifiedops.ingest --source X --limit N` proves live vendor connectivity + row counts |
| Data integrity | SQL probes (distinct dates for trends, mapping coverage, orphaned-submission checks) |

**Interview line:** "I test the *risk*, not just the code — RBAC and ingestion get dedicated
harnesses because a fail-open there is a data breach, and a bad upsert corrupts history."

---

## 8. Security (OWASP-aware talking points)

- **A01 Broken Access Control** → deny-by-default RBAC, row-level scope filters, fail-closed Sonar,
  verified by harness.
- **A02 Crypto** → bcrypt password hashing, JWT HS256, httpOnly + SameSite cookies, TLS at Nginx.
- **A03 Injection** → parameterized SQL everywhere (`text()` binds, `pg_insert`); a real bug where a
  regex char class `[-/:_]` was read as a bind param `:_` taught me to be careful with `text()`.
- **A07 Auth failures** → SSO takeover fix (provider-scoped lookups), stale-token handling.
- **Secrets** → vendor creds only in gitignored `.env`, lru-cached, never logged/committed.
- **Rate limiting / DoS** → Nginx rate limits; GitHub/vendor rate-limit handling that fails loud.

---

## 9. Interview Q&A — Technical

**Q: Walk me through the architecture.**
A: Use Section 3.1 — Nginx fronts 4 SPAs + 2 FastAPI backends; PostgreSQL (3 schemas) + Prometheus;
one JWT identity bridged across both backends.

**Q: Why two backends instead of one?**
A: Separation of concerns + independent deploy cadence. The main backend owns identity, runners,
alerts; the maturity backend owns maturity + Unified Ops and reads existing tables with raw SQL. They
share one JWT so it's still SSO.

**Q: What's the hardest/most interesting part?**
A: Two answers — the **runner→host correlation** (bridging GitHub status with real host metrics via
service-name matching in Python), and **Unified Ops ingestion** (config-driven ETL from 5 vendor
APIs with idempotent upserts, a daily scheduler, and fail-closed RBAC).

**Q: How does Unified Ops ingestion work end to end?** (expect this if it's on your resume)
A: Section 4.5 pipeline — seed JSON → `project_config` source of truth → `ingest.py` loads active
config → per-vendor fetcher calls the REST API over httpx → `ON CONFLICT DO UPDATE` upsert into the
`unifiedops` time-series table → `ingest_runs` records the run. Runs daily at 02:20 via APScheduler
or on-demand via the admin "Refresh now" endpoint.

**Q: How do you authenticate to five different vendors?**
A: Per-source strategy — HTTP Basic against corporate AD/LDAP for Coverity and SonarQube (bare
sAMAccountName, not UPN — a real gotcha that returned 401), bearer token for Black Duck, and a PAT
for GitHub. Creds are env-only and lru-cached; changing them requires a backend restart by design.

**Q: How is Unified Ops secured so a user only sees their repos?**
A: Deny-by-default. `_scope` resolves the user's repos; queries filter by repo name, and a no-scope
user gets `AND 1=0` (matches nothing). Sonar (no repo in feed) scopes via component mapping, so it's
invisible until linked — fail-closed. A harness proves scoped ⊂ admin.

**Q: Why isolate a separate `unifiedops` schema?**
A: Blast-radius + clarity. A private `MetaData(schema="unifiedops")` means `create_all` only ever
touches Unified Ops tables, never maturity, and DDL is fully-qualified regardless of search_path.

**Q: How do you keep ingestion idempotent / safe to re-run?**
A: Every write is `pg_insert(...).on_conflict_do_update` keyed on the natural unique constraint, so
re-ingesting a day updates rather than duplicates. Failures fail loud and never write empty rows.

**Q: How did you handle GitHub API rate limits (repo analytics)?**
A: Fail loud (never cache empty), per-PR content-addressed memo keyed by `updated_at`, and period
containment so a 365-day superset derives shorter windows client-side with zero extra calls.

**Q: How do you prevent alert email storms?**
A: Stateful watch sessions + monitor window + hysteresis + no-data grace + per-rule digest batching
+ daily cap counting distinct email events + master/per-admin kill switches.

**Q: Async or threads?**
A: FastAPI async for request handling and DB (asyncpg); the runner collector and action worker run
as background threads; snapshots and ingestion run on async/scheduler loops — all off the request
path.

**Q: How do you know a deploy actually shipped?**
A: I read the hash the built `index.html` references (Vite content-hash), not just the newest file —
additive Nginx copies leave stale chunks around.

## 9b. Interview Q&A — System Design (scale-ups they may ask)

- **"Make ingestion real-time":** move from daily cron to event-driven (webhooks where vendors
  support them) + a queue (e.g. Redis/RQ or Celery) with per-source workers; keep upserts idempotent.
- **"Scale to 10k runners":** shard the collector, cache runner→host maps, push snapshots to a
  time-series store, paginate/stream APIs, and add read replicas.
- **"Multi-region / HA":** stateless backends behind a load balancer, PostgreSQL primary+replica,
  externalize the disk cache to object storage/Redis.
- **"Add a 6th vendor":** implement one `fetchers/x.py` + a `config_seed` shape + a schema table;
  register in `REGISTRY`. No other code changes — that's the point of the config-driven design.

## 9c. Interview Q&A — Behavioral / STAR

**STAR 1 — Fail-closed RBAC (Unified Ops)**
- *S:* Unified Ops exposed multi-vendor security data; a leak across teams would be serious.
- *T:* Ensure scoped users can only ever see their own repos' metrics.
- *A:* Built `_scope` reuse + repo-name filters with `AND 1=0` for no-scope; scoped Sonar via
  component mapping (invisible until linked); wrote a harness asserting scoped ⊂ admin and unauth →
  401.
- *R:* Verified fail-closed behavior; no cross-tenant exposure; security is provable, not assumed.

**STAR 2 — Config-driven ingestion**
- *S:* Five vendor portals, manual checking, no single view.
- *T:* Automate and unify security/quality/build metrics.
- *A:* Designed seed-JSON → `project_config` → per-vendor fetchers → idempotent upserts, scheduled
  daily with an admin refresh and per-run tracking.
- *R:* One RBAC-scoped posture dashboard; adding a project is a config row, not a deploy.

**STAR 3 — Cache-shape bug (production white-screen)**
- *S:* New UI white-screened after deploy.
- *T:* Find why the frontend crashed on `x.toLocaleString()`.
- *A:* Root-caused a disk cache serving old-shape payloads to new code; added a `_SHAPE` version to
  cache keys + null-safe formatters.
- *R:* Deploys became shape-safe; degraded gracefully instead of crashing.

**STAR 4 — SSO account takeover**
- *S:* SSO login could resolve a *local* account via shared email.
- *T:* Close the takeover path without breaking local login.
- *A:* Required `auth_provider='sso'` on all SSO lookups; reclaimed polluted HIDs; kept local
  username/password path unchanged.
- *R:* SSO can never load a password account; verified against real colliding emails.

---

## 10. Likely Curveballs (and honest answers)

- **"Is Unified Ops real-time?"** — No; daily scheduler + on-demand refresh. 3 of 4 feeds are
  single-snapshot today, so I show honest "one snapshot" panels rather than fake trend lines. The
  build-ratio feed has ~181 daily points and trends correctly.
- **"Row-level fusion of maturity ↔ vendor metrics?"** — Designed (via `component_id` on
  `project_config`) but not yet populated; today the cross-link is name/alias-based. Next step is an
  admin linking UI.
- **"What would you refactor?"** — Add a config-CRUD API for Unified Ops, expose `project_lob`
  across all vendor reads, and move ingestion to a queue for real-time.
- **"What broke most often?"** — Stale uvicorn workers (no-reload maturity backend) and additive
  Nginx deploys leaving old chunks; I now verify via unauth 401 probes and the `index.html` hash.

---

## 11. Interview Method Notes (how tech interviews are run)

- **Formats**: recruiter screen → technical/coding → system design → behavioral → hiring-manager.
- **Project deep-dive**: they pick one resume bullet and drill 15–20 min. *Be ready to whiteboard
  Section 3.1 and the Unified Ops pipeline from memory.*
- **STAR** for behavioral (Situation-Task-Action-Result) — Section 9c.
- **Depth over breadth**: they follow "why" chains 3–4 levels. Every bullet here has a "why".
- **Own the tradeoffs**: state what you did *not* do and why (see Section 10) — shows judgment.
- **Numbers**: quantify (Section 1). "400+ runners", "5 vendor tools", "139K LOC".

---

## 12. One-Page Cheat Sheet (skim before the call)

- **Pitch**: full-stack DevSecOps SRE + Engineering-Intelligence platform; 4 SPAs + 2 FastAPI; PG +
  Prometheus; Nginx/Docker; ~139K LOC.
- **Runner→host**: runner name → Windows service → exporter host metrics (Python match).
- **Alerting**: watch sessions + hysteresis + no-data grace + per-rule digest + kill switches.
- **Maturity**: BU→GBE→Project→Component, 0/5/10/NA scoring, DRAFT→PENDING→APPROVED, fuzzy Excel
  import.
- **Repo analytics**: PR cycle time + Copilot %; shape-versioned SWR cache; period containment;
  fail-loud rate limits.
- **Unified Ops ⭐**: 5 vendors (Coverity/BlackDuck/Sonar/GitHub/tech-stack) → seed JSON →
  `project_config` → fetchers → idempotent upsert into isolated `unifiedops` schema; APScheduler
  daily 02:20 + admin refresh; multi-auth (LDAP Basic / token / PAT); deny-by-default RBAC by repo
  name (fail-closed); 5 pages (Overview/Security/Quality/Build/TechStack); group by `project_group`.
- **Auth/RBAC**: JWT httpOnly + bcrypt; 6 runner / 7 maturity roles; SSO bridge; takeover fix.
- **Security**: OWASP A01 fail-closed RBAC, parameterized SQL, secrets in .env, TLS at Nginx.
- **Testing**: pytest units, RBAC harnesses, DB rollback integration, Playwright E2E, ingest CLI.
- **Tradeoffs**: not real-time (daily), fusion link unpopulated, config-CRUD is future work.

```
```
```
```
```
```
```
```
```
