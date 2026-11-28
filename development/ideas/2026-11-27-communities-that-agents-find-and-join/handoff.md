# Handoff: The Society Of Human And AI Communities

Written 2026-11-28 by Claude Code (Opus 5.5) for the model that continues this project. The repository is Noetarium, at `c:\Users\Micro\Desktop\The Science` (Windows; the path has a space, so quote it). This handoff covers only the communities project inside it: the foundation thesmos, the communities domain, the API's composition of them, the work Koinonia, the book the communities wrote and its completion, and the papers about them. Read the repository's `CLAUDE.md` for the infrastructure's conventions; its §1 entry "Communities", its §3.1 row "thesmos" and its §9 bullets from "Passports and the household" to "The completion re-founded" summarise this project exactly.

## 1. The Project In Brief

The author (Wanhong Huang, the principal investigator; fluent in abstract algebra and formal language theory) asked on 2026-11-25 for a society of human and AI agent communities: heterogeneous communities with no centre, agents of any provider lent under mandates, money-like forms that may appear and dissolve. It became:

| Part | Where | Record |
|---|---|---|
| thesmos, a foundation of institutions (parties, mandates, acts, constitutions as data, the chronicle and its fold, holdings, passports, a taxonomy of behaviour, items and routes, posted work, compacts) | `foundations/thesmos` (a git submodule; its own `CLAUDE.md`, decisions 0001 to 0014, theory `docs/theory.md`) | ADR 0167 |
| The communities domain (twelve presets, circulation, returns, federation, claims, registries, organisations, trajectories; subdomain `human-ai/`) | `domains/communities` (theory notes in `theory/`) | ADR 0167, 0173 |
| The API's composition | `api/cpp/science/communities.hpp`, `api/cpp/science/community_readings.hpp`, `api/python/science/communities.py`, checks `api/tests/check_communities.py` | ADR 0174 |
| Koinonia, the work (port 9230): societies of communities, agents by rule and by Claude, a sealed behaviour journal, the book case, and since 2026-11-28 a node and gateway for agents of any provider | `applications/koinonia` | ADR 0169, 0170, 0172, 0175, 0176 |
| The redesign "Communities That Agents Of Any Provider Find, Join, Work In And Leave" (an international society with no centre), in 14 phases | `development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md`, with `taxonomy-of-behaviour-v2.md` and progress notes in `phase-9/`, `phase-10/`, `phase-12/` | ADR 0173 to 0176 |

## 2. State Of The Redesign

Gate A of the design was approved by the author in full on 2026-11-27.

| Phase | Content | State |
|---|---|---|
| 1 to 7 and the taxonomy's second version | thesmos's decisions 0006 to 0013, kategoria's 0005 (`ontologies`), kybernesis's 0007 | built |
| 8 | the communities domain: presets of every order, registries, organisations, trajectories; thesmos's decision 0014 (orders, terms, seats) | built (ADR 0173) |
| 9 | the API: sessions, views, the archive of conduct, exit packages, readings and power | built (ADR 0174) |
| 10 | Koinonia's node and gateway, passports at households, `ONBOARDING.md`, `--open-to-agents` | built (ADR 0175); the page's new views deferred |
| 11 | the platform league's operations (docket, releases, conformance, maintainer's office, the completion's accession) | not built |
| 12 | the completion re-founded as a community at the node, the first attempt carried | built (ADR 0176), and run once on the real store |
| 13 | the test with fresh agents of other providers (T1 to T4, measures M1 to M15) | the author runs it |

Checks, all passing at the last runs: thesmos 345, the communities domain 134, the API 862 (communities 62), Koinonia 153. The whole run after Phase 9 was 21,219 of 21,219; a whole run after Phase 10 was nearly finished and passing when this was written. Run it again: `node development/tools/run-tests.mjs` (about an hour or more; C++ builds in WSL).

**Everything since ADR 0172 is uncommitted**, in four repositories: `foundations/kategoria`, `foundations/thesmos`, `foundations/kybernesis` and the root. Commit only when the author says so (Section 7).

## 3. The Book And Its Completion

**2026-11-28 operational update.** The 200 GPT operatives were issued and dispatched. At 11:49 JST, 31 sections awaited checking, 41 remained open and the overview was waiting. The author reported that a real Claude steward was unavailable. Before changing an office or resuming a dispatcher, read the [200-agent incident, recovery commands and latest operational checkpoint](phase-12/200-agent-stall.md). The founding and migration counts below describe the earlier Phase 12 snapshot.

- **The book.** Four communities wrote *Seventeen Goals, Fifteen Years*, a global analysis of the SDGs, in a run of 227 rounds that ended when the model provider's credit ran out: 32 sections adopted, 27 withdrawn, 102 unwritten. Its communities' edition is set at `docs/reports/2026-11-26-koinonia-book/` (`build.ps1`, `book.pdf`; the typesetter is `applications/koinonia/koinonia/typeset.py`, with `--communities-only`). Never edit the society files under `data/cache/koinonia/societies/`.
- **The first attempt to complete it** worked through a tool beside the communities (`docs/reports/2026-11-26-koinonia-book/completion/`). Every question went to the author, which led to the redesign. It is now retired: its journal is closed and its server answers 410.
- **The completion re-founded** (Phase 12) is a community at Koinonia's node, beside the author's household and the platform league (a plain community holding the registry and acting as notary). 56 carried sections were accepted by the author's rule (adopt or minor from another provider); 67 are posted again (11 to revise, 56 to review); 5 were never written; the overview waits. 72 work items are open. The node's store is `data/cache/koinonia/node/`.
- **Writing book 2 with outside agents.** One command issues the passports and writes each agent's prompt: `python applications/koinonia/book_agents.py` (all six operatives the household declared; name some to issue only those; `--model codex-a="<model>"` to record the model; `--status` for the work by state; `--plain` for the constant prompt of the test). Passports and prompts are written to `%APPDATA%\noetarium\koinonia-agents\<name>\` (the prompt holds the agent's credential, printed nowhere). The operatives: `author/claude-code` (the steward, Anthropic), `author/codex-a`, `-b`, `-c` (writers, OpenAI), `author/claude-review` (reviewer of OpenAI sections, Anthropic), `author/codex-steward` (reviewer of Anthropic sections, OpenAI). Start the steward first and keep it running; it checks deliveries, posts review items and accepts. A reviewer is admitted only while a review item is open for its provider, so start reviewers after the first deliveries (their prompts say to apply again when one opens). Answer none of the agents' questions about the community. **Many agents (100 to 200):** a household declares its operatives only at its founding, so each further batch founds a household of the author's: `python applications/koinonia/book_agents.py --add 150 --provider openai --prefix gpt --role writer --model "<model>"` and `--add 50 --provider anthropic --prefix claude --role reviewer`; `index.tsv` in the settings folder lists every agent's prompt for a launcher. The node limits requests per principal (all the author's agents share one), so serve it with `--rate 6000` (`python applications/koinonia/serve.py --port 9230 --rate 6000`). The completion reviews 20 deliveries a round (its receiving capacity), which bounds throughput, not the number of agents. Tested on a copy of the node: 30 passports in 2 seconds, three presented and admitted as writers. Tested on a copy of the node: a writer's passport presented and its application admitted; a reviewer's declined while no review item was open.

## 4. Open Work, In The Author's Priority

1. **Keep Koinonia's server running** on port 9230: `python applications/koinonia/serve.py --port 9230 --rate 6000` from the repository root, with `PYTHONIOENCODING=utf-8`. Restart it whenever the node's store is changed by a script; a running server holds the store in memory.
2. **Support the run of book 2** if the author asks: read the completion's record, find what blocks agents, and fix the node or the gateway. Never act in the community on the agents' behalf.
3. **The bench's tools at the node.** The seven built-in tools and three admitted tools of the book's bench (`koinonia/bench.py`) are not declared as the completion's tools in `koinonia/tools.py`, so writers cannot compute through the gateway. The five unwritten sections need them.
4. **The completed edition of the book.** Typesetting from the node's accepted sections is not built. Adapt `typeset.py` to read the completion community's record (accepted sections and their reviews) and keep the independent notice page that separates the communities' work from the completion, naming each section's agent, model and provider.
5. **Phase 11**, the platform league's operations, and **Phase 13's** `measure.py` (Section 13 of the design).
6. **The page's views** (a directory, passports, items, a party's trajectory) in `applications/koinonia/src/app.js` and `styles.css`.
7. **Deferred at the API:** bindings and custom through `science.law` (needs an engine for the law domain's `sources`), voting power through `science.multilateral`, `organisations` at the API, readings of open terms.

## 5. The Papers

All untracked; build each with its `build.ps1` (XeLaTeX and BibTeX in WSL). Every number is written by `values.py` as `\setval` and asserted, so an undefined value stops the build.

| Paper | Folder | State |
|---|---|---|
| *Foundations Of A Society Of Human And AI Agent Communities: Towards Shared Labour Among Heterogeneous Subjects* (the design and theory, built and planned) | `docs/reports/2026-11-27-koinonia-foundations/` | revised 2026-11-28, 102 pages. Its status assertions stop the build when a planned phase lands (Phase 9, 10 and 12 have landed since its revision, so update its status table and assertions first). The author is to read its six open questions (law, ethics, a new social theory, normative acts among kinds of subject, epistemology, history and generativity) and decide four review items (priority wording, two precisions, weak dissolution, the priority claim) |
| *The Agent Community Infrastructure Of Noetarium: Design, Implementation And Case Studies* (technical report; covers the implementation) | `docs/reports/2026-11-27-koinonia-technical-report/` | v1 published by the author; v2, an appended revision on Phases 1 to 12, to be written now that the implementation is nearly done |
| *Values Formed In History: Humanist Evolutionary Evidence-Based Decision Making Among Communities Of Human And AI Agents* | `docs/reports/2026-11-27-humanist-evolutionary-decision/` | 48 pages; revise against `review/reading-for-error.md` (13 errors, 29 risks, 5 style) |
| *Credit and Its Dissolution in a Recorded Society of Human and AI Communities* | `docs/reports/2026-11-26-koinonia/` | a light pass under the rules below |
| The book, communities' edition | `docs/reports/2026-11-26-koinonia-book/` | finished; the completed edition waits on item 4 above |

The division the author set: the foundation paper presents the design and its theory; the technical report covers the implementation.

## 6. The Author's Rules

**Writing (strict, from the first draft).**
- Every heading, table column title and box title is a noun phrase in Title Case (every word capitalised), never a question, a claim or a sentence.
- No "not … but", no "rather than", no em dashes, no redundant explanation.
- No `itemize`, `enumerate` or `description` in papers: prose, or tables.
- Figures with no overlapping elements; check rendered pages.
- The foundation paper's introduction has no sectioning.
- Layout tight: no half-empty pages from pinned floats; the front note fits one page.
- Each paper opens, after the title page, with one page: a Note; Conditions Of Use (Research use, Rights); a Notice (the experiment by heterogeneous self-organised AI communities with their own political structures; scope; the licence CC BY 3.0 IGO unless the template follows a source with its own; related scholarship and priority); a Disclosure Of AI Assistance.
- An executive summary; if a model wrote it, the notice says so. GitHub links of the infrastructure and the communities domain in the note.
- The author's questions and positions are attributed and left open; the inquiry is the author's.

**Working.**
- Quality first; no hurry. Commit or push only at the author's word.
- A general method goes in a foundation, and a domain or work uses it; never re-implement.
- Model keys live only in `%APPDATA%\noetarium\models.json`: never read them aloud, print, log or commit them. Koinonia's model ledger is `%APPDATA%\noetarium\koinonia-budget.json`.
- Do not impersonate real organisations (a layout may follow a flagship report, never its name or branding).
- Never use the word "laboratory" for any part of the work.
- Treat the author's personal details as private.

## 7. How To Commit, When The Author Says So

Order: `foundations/kategoria`, then `foundations/thesmos`, then `foundations/kybernesis`, each committed inside its submodule first, then the root with the updated pointers. In each repository add a section to `development/release-notes/YYYY-MM-DD.md` (plain prose of what is new) and index the day in that folder's README. Commit with `git commit -s` (the author's `Signed-off-by`) and the co-author trailer of the model that did the work. Look at `git status` first: `data/cache/` is never committed; the papers under `docs/reports/` and some idea folders have been left untracked deliberately, so ask before adding them. Push only with the author's agreement (each repository pushes over the SSH alias `serendip_wanhong`).

## 8. Practical Pitfalls

- C++ builds run in WSL with g++ and CMake. An engine on many foundations takes about 10 minutes, and any change to a thesmos or domain header rebuilds the session engine. Run one heavy build at a time. If WSL hangs, stop stale `wsl.exe` processes and run `wsl --shutdown --force`.
- In Git Bash, compile commands need `MSYS_NO_PATHCONV=1`.
- PowerShell 5.1: `Out-File` writes a BOM; write JSON with `[IO.File]::WriteAllText(path, text, (New-Object Text.UTF8Encoding $false))`. Variables are case-insensitive (`$r` overwrites `$R`).
- Test servers on other ports than 9230.
- The domain's suite: `node domains/communities/tests/run-tests.mjs`; Koinonia's: `node applications/koinonia/tests/run-tests.mjs`; the API's: `node api/tests/run-tests.mjs`; thesmos alone: `node development/tools/run-cpp.mjs foundations/thesmos thesmos`.

## 9. The Working Trajectory

Whoever continues keeps an append-only log of the work in `development/ideas/2026-11-27-communities-that-agents-find-and-join/trajectory.md`: each entry with its date and time, task, what was done, files changed, checks run with counts, the author's decisions, what did not hold, what is left and the exact next step. This handoff and the trajectory together give the present state.

## 10. Files To Read First

1. `CLAUDE.md` (§1 "Communities", §3.1 "thesmos", §6, §7, §9 bullets named above).
2. `development/ideas/2026-11-27-communities-that-agents-find-and-join/design.md`, and the progress notes in its `phase-9/`, `phase-10/`, `phase-12/`.
3. `development/decisions/0173` to `0176`.
4. `applications/koinonia/ONBOARDING.md` and `README.md`.
5. `foundations/thesmos/CLAUDE.md` and `docs/theory.md` §10 onward, when touching the ground.
6. `trajectory.md` beside the design, once it exists.
