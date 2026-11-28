# Phase 12 Progress (The Completion Re-Founded)

Started 2026-11-28 from the design's Phase 12 row, Sections 7.9.10, 11 and 15, ADR 0175 and Phase 10's notes.

## Plan

1. `koinonia/completion.py`: the founding sequence of Section 11.2 at a node: the author's household on its preset, the platform league
   on its preset as a plain community holding its registry (its docket, releases, conformance and maintainer's office are Phase 11's),
   the completion on Section 7.9.10's constitution, recognition of the league at the level of evidence, the steward's passport and
   presentation, the holdings at the founding by digest, the listing in the league's registry, the steward's briefing.
2. `carry_completion.py`: the first attempt read (never altered); the bag made and validated by archeion; the side journal as a holding
   of kind record and its entries as episodes (grade A where they carry digests); the decision `accept carried work` as the author made
   it; the carried sections, reviews and results as deposits; the remaining work posted; then the retirement (the closing entry by the
   journal's own sealing rule, `serve.py` answering 410).
3. `tests/check_completion.py` on a copy of the first attempt and a temporary store; registered in `tests/run-tests.mjs`.
4. One run against the node's real store (`data/cache/koinonia/node/`).

## Found While Probing (2026-11-28)

- The first attempt's journal holds 7,173 entries, not 7,172: a second `replay` was sealed at 17:32:15 after the design's count. The
  design's breakdown (3,386 requests, 2,043 runs, 1,222 reads, 240 checks, 159 takings, 98 reviews, 4 invitations) holds, with 17 runs
  refused, 2 releases and 2 replays besides.
- Of the 90 standing reviews, 67 bear on the section's current version (adopt 20, minor 36, revise 11) and 23 on an earlier one; 33
  checked sections have no review. The author's rule accepts 56; 67 carried sections are not accepted (11 to revise, 56 to review).
  Section 11.2's "16 enter returned" counted five revise verdicts on versions since replaced.
- An operative acts only under a mandate, and the node's office of status reports without one: the steward's office is held at the
  founding by the author (a person), who stays its holder for the reports; the agent that stewards presents a passport and is admitted.
- Entry "assigned by" is the act `admit` naming the position; `assign` with a position ends an assignment.
- Section 7.9.10 gives the steward no norm to deposit what it carries, and no amendment of the ground changes norms of access (the
  change keys cover conversions, open entry, decisions, receiving, compacts and work): the founding constitution adds the steward's
  norms of deposit within `first attempt`, `adopted`, `reviews`, `facts`, `plan`, `checks` and `record of the run`.
- A review item names a work item with a delivery in the community's own chronicle: carried sections, whose writers are not parties
  here, cannot be reviewed by a review item, nor enter "returned" (a state of a taking). Each carried section not accepted is posted
  as an item of kind `section` whose specification is the carried version.
- A second presentation by the same operative under a fresh passport is admitted.
- `registry.verify` (Phase 10) passes a head without its community and no witnesses, so it reads every listing as faulty; the
  completion verifies its listing with the notary's witness and head.

## Done

- `koinonia/completion.py`, `carry_completion.py`, `tests/check_completion.py` (17 checks, about 36 s on a copy), registered in
  `tests/run-tests.mjs`; README rows.
- The real run (2026-11-28) into `data/cache/koinonia/node/`: the household, the platform league (the node's notary, the author its
  registrar) and the completion; 129 sections of the plan, 123 carried with their files' digests (56 accepted within `adopted`, 67
  within `first attempt` and posted again: 11 to revise, 56 to review), 90 reviews, 125 results files and the replay's totals within
  `facts`; the bag (1,019 files) valid, SHA-256 of its tag manifest `49fc2a45…bd971`; 7,173 episodes (grade A for the 2,381 entries
  with digests); 72 items open and the overview waiting; the completion listed in the league's registry, checked by the registrar and
  verified current; the steward briefed; the first attempt's journal closed by entry 7,174 (`f72cab83…`), its `serve.py` answering
  410 with `http://127.0.0.1:9230/c/completion/profile`. The completion's head: 425 acts.
- The server on port 9230 was started before the store existed: it serves the completion once restarted.

## Not Built

- Phase 11: the league's docket, releases, conformance holding and offices by its assembly (the author is registrar meanwhile), the
  completion's accession and its steward's accreditation as envoy (the completion acts in the league as a community).
- The bench at the node: the seven built-in tools and the three the society admitted are not hosted, so writers cannot yet compute
  results through the gateway; `shelf` stays declared by reference.
- The journal's chain attested by a reviewer of another provider (no such party is present yet).
- Reviews of carried sections by another provider than their first writer's are asked in the items' notes and left to the steward's
  acceptance: the fold compares a review with the deliverer in the community's own chronicle.
- `complete.py` of the first attempt still writes its journal if run; only its server was retired.
