# Working Trajectory

An append-only log of the work on the society of human and AI agent communities, kept so that the work can be continued days later by any model. Read it after `handoff.md`. Add entries at the end; never rewrite earlier ones.

## 2026-11-28 03:35, Claude Code (Opus 5.5): Handoff

- **Task:** hand the project over, since this session's weekly usage was nearly spent.
- **Done in this session (2026-11-27 to 28):**
  - the design's Phases 1 to 10 and 12, with thesmos's decisions 0006 to 0014, kategoria's 0005 and kybernesis's 0007;
  - ADRs 0173, 0174, 0175 and 0176;
  - the foundation paper revised to 102 pages, its introduction without sectioning and its layout tightened.
- **Phases deferred:** Phase 11 (the platform league's operations) and the page's new views.
- **Phase 12 run:** carried out once on the node's real store (`data/cache/koinonia/node/`). 56 sections were accepted and 72 work items are open.
- **Fix found and made:** Phase 10's `registry.verify` read every listing as faulty. It now uses the notary's witness and head and names the registry. On a copy of the store the completion's listing verifies as current, witnessed and claimed.
- **Checks:**

  | Suite | Checks passing |
  |---|---|
  | thesmos | 345 |
  | communities | 134 |
  | API | 862 |
  | Koinonia | 153 |
  | Whole run after Phase 9 | 21,219 of 21,219 |

  A whole run after Phase 10 was nearly finished and passing.
- **Author's decisions:**
  - "quality first";
  - the foundation paper presents the design (built and planned), and the technical report covers the implementation;
  - the introduction should have no sectioning, and the layout must be tight;
  - continue the implementation with the weekly balance left;
  - the author will write book 2 with outside agents through the gateway.
- **Did not hold, recorded in the ADRs:**
  - the maintainer's removal by decision;
  - a trigger opening a review of a long-held office;
  - draws on budgets under transmission;
  - carried sections as review items;
  - the steward's deposits under Section 7.9.10.
- **Left:** items 1 to 7 of the handoff's Section 4, the papers of its Section 5, and the commit when the author says so. Nothing has been committed since ADR 0172.
- **Next step:** confirm Koinonia's server is running on port 9230 (restart it with `python applications/koinonia/serve.py --port 9230` if not). Then ask the author which open item to take, most likely support for the run of book 2, or the bench's tools at the node.

## 2026-11-28 04:00, Claude Code (Opus 5.5): A Script For The Agents Of Book 2

- **Task:** the author found issuing passports by hand too complex ("has script? ... or better have a UI") and wants to write the book first.
- **Done:** `applications/koinonia/book_agents.py`, which mints the author's principal credential for the run, issues each operative's passport through the running node, and writes `passport.json` and `prompt.txt` (the prompt of `ONBOARDING.md` filled in, with a line suggesting the position) to the settings folder; `--status` shows the completion's work by state. The handoff's Section 3 now uses it.
- **Checks:** run against a copy of the node on port 9232 (removed after): passports issued for `codex-a` and `claude-review`; the writer presented and admitted; the reviewer declined while no review item was open, as the completion's rules say, so the reviewer's prompt says to apply again when one opens. The real node on 9230 was not touched.
- **Left:** a page for issuing passports and watching the work (the page's views of Phase 10, deferred).
- **Next step:** the author runs `python applications/koinonia/book_agents.py` and starts the steward, then the writers, then the reviewers.

## 2026-11-28 04:10, Claude Code (Opus 5.5): Many Agents

- **Task:** the author: "The write count is fixed? I think We want allow more than 100 agent, even 200 for experiment."
- **Found:** a household declares operatives only at its founding (six in the author's); the households route always named a household after its person, so a second household of the author's collided; the node's rate is per principal (120 a minute), shared by all the author's agents.
- **Done:** `serve.py` takes a household's `name` at `/api/node/households` and a `--rate`; `book_agents.py --add N --provider P --prefix X --role writer|reviewer` founds `author-household-<prefix>` with N operatives and issues their passports, and writes `index.tsv`. Koinonia's server on 9230 restarted with `--rate 6000`.
- **Checks:** on a copy of the node (port 9232, removed after): 30 passports issued in 2 seconds; three presented at the completion and admitted as writers. Koinonia's suite was not re-run after these edits; run it before any commit.
- **Next step:** the author issues batches and launches agents; run `node applications/koinonia/tests/run-tests.mjs`.

## 2026-11-28 04:40, Claude Code (Opus 5.5): A Save Refused By Windows

- **Task:** the author's batch of 150 GPT writers failed after 50 passports with `RemoteDisconnected`; an earlier run had been interrupted.
- **Found:** the node's save moved `node.tmp` onto `node.json`, and Windows refused it (WinError 5) while another program held the file; every request also shared one temporary name, and saves of `node.json` were not serialised across communities.
- **Done:** `node.py` writes each file through `write_into` (a temporary file per thread, retried for about two seconds when Windows refuses) under a node-wide save lock; `book_agents.py` skips agents that already have a passport and a prompt (`--again` to reissue), retries a dropped connection, and keeps `index.tsv` to one row an agent. A stale `node.tmp` removed; the node restarted with `--rate 6000`.
- **State of the batches:** `author-household-claude` founded with 50 reviewers, all 50 prompts written; `author-household-gpt` founded with 150 operatives, but the GPT prompt folders are no longer in the settings folder (only an empty `gpt-028`), so the batch is to be run again; the passports issued before are mandates whose credentials are lost and go unused.
- **Checks:** Koinonia 153 of 153.
- **Next step:** the author runs `python applications/koinonia/book_agents.py --add 150 --provider openai --prefix gpt --role writer --model "GPT-6.2"` again.

## 2026-11-28 05:00, Claude Code (Opus 5.5): The Whole Run After Phase 10

- The whole run after Phase 10 (before Phase 12 and the later edits to Koinonia) ended with exit code 0, no failure, every domain alone passing. 21294 of 21294 checks passed in 85 suites; 17 group(s) skipped for missing data.
- Koinonia's suite after Phase 12 and the edits of 04:00 to 04:40: 153 of 153. Run the whole run again before any commit.

## 2026-11-28 12:20, Claude Code (Opus 5.5): The First Run Of 200 Agents Stalled; Commit And Push

- **Committed and pushed at the author's word** ("commit and push", then "yes" because the weekly balance was running out). The commits are nomos 6412bae, kategoria 7d8ecde, thesmos 8085a7f, kybernesis 28f438a and the root 308755d, with release notes of 2026-11-28 in each.
  - **Left out of the root:** `research/experiments/2026-11-26-koinonia-book/book-run/data/behaviour.jsonl` (131 MB, over GitHub's limit for one file, kept on this computer); the 2026-11-23 ideas; `development/ideas/2026-11-27-attractors-of-values-and-norms.md` (another idea of the author's, not this project's); `docs/reports/`, `docs/news/` and the "[DONT COMMIT]" note.
  - **The record:** a `.gitattributes` (`* -text`) keeps its bags byte for byte.
  - **Other work in the commit:** Koinonia then held work by another model (an OpenAI model through Codex, run by the author), namely `book_dashboard.py`, `book-dashboard.html`, `book_office.py`, `koinonia/book_roles.py`, `koinonia/dispatch/`, `koinonia/dispatch_waits.py`, `recover_dispatch.py` and their checks, with edits to `book_agents.py`, `completion.py` and `gateway.py`. Koinonia's suite passed 200 of 200 on that state, and it was committed as it stood.
- **The run, as the other model reported it to the author (11:46 to 12:08 JST):**
  - 200 agents presented their passports: 150 GPT writers and 50 GPT reviewers.
  - 31 of the 72 outstanding sections were delivered, all by OpenAI writers. All 31 waited for the steward's check, none had a review across providers, 41 stayed open and the overview waited.
  - The last action by a book agent was a reviewer's question at 09:13 JST. After that nothing moved.
- **Causes found:**
  1. The launch left out the steward (`author/claude-code`) and any reviewer of another provider than OpenAI. Nobody checked deliveries or posted review items, and the community's rule of review by another provider could not be met. The 50 Claude reviewers' passports were issued but not launched.
  2. The steward's passport lacked the powers its founding mandate granted (answering questions, posting review work, accepting). `book_agents.py` sends no `kinds`, so `passports.issue` falls back to its default list, which leaves those acts out. A steward running under that passport could not have done its work. The other model began fixing the issuer.
  3. Agents filed 196 questions, about missing evidence, unavailable statistical tools, and how to claim or deliver a section. None was answered, because the steward, whose route answers them, was absent.
  4. The dispatcher watched the section work board, while 70 writers waited on the question or application they had filed. Those items never appear on that board, so a reply would have been missed. The dashboard also counted 10 ended sessions as waiting. The other model repaired the dispatcher's waits and the gateway.
  5. The bench's tools are not at the node, so writers could not compute (handoff, Section 4, item 3).
- **The author's decisions:**
  - No real Claude steward can run now (Claude's balance).
  - The other model prepares the appointment of a GPT operative as steward, and the author decides the appointment under the completion's founding rules.
  - The 31 OpenAI deliveries still need a reviewer of another provider.
  - Implementation pauses for several days.
- **Left:**
  - Steward: decide and record the steward's appointment, then reissue its passport with its powers.
  - Review: launch a reviewer of a provider other than OpenAI, or have the community amend its rule of review by its own route.
  - Questions: answer the 196 questions on their routes.
  - Bench: host the bench's tools at the node.
  - Dispatcher: watch each agent's own items.
  - Launch: `book_agents.py` should give the steward the kinds its office needs (or read them from the founding mandate), and launch scripts should refuse to start writers without a steward and a reviewer of another provider.
- **Next step:** read the other model's own notes and edits (they may be later than this entry), confirm Koinonia's suite and the node's state, then resume with the steward's appointment.
