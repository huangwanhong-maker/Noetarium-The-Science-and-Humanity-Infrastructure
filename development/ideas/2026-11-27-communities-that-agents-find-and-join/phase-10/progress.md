# Phase 10 Progress (Koinonia's Gateway And The Archive Of Conduct)

Started 2026-11-28 from the design's Phase 10 row, Sections 3.5, 4.1, 8, 9 and 10, and Phase 9's progress notes.

## Plan

1. `koinonia/behaviour.py`: episodes with the features of Section 3.5; a Merkle tree by archeion's rule (RFC 6962, the leaf a seal and its
   fields), held to the engine's head of each community's series; retention by a declared schedule after archeion's `due`, applied to the
   payloads kept beside the sealed series and never to the series; access by the binding-record principle; arrangement as fonds and series.
2. `koinonia/tools.py`: the restricted runner moved from `bench.py` (which keeps `Restricted` for the book), two runs at once, the run as
   `use` acts and a `deposit` of the result.
3. `koinonia/node.py`, `passports.py`, `registry.py`, `briefs.py`, `gateway.py`: the node (a session and a lock a community, persisted with
   seals and series), households and passports (tokens shown once), the office of status, the routes of Section 9 as a pure handler.
4. `serve.py`: `/` (the page to a browser, the guide otherwise), `/.well-known/noetarium.json`, `/c/...`; `--open-to-agents`.
5. `ONBOARDING.md`, `README.md`; `tests/check_gateway.py`, `tests/check_archive.py`.

## Found While Probing (2026-11-28)

- A check's result is the field `result` (`pass` or `fail`), not `verdict`.
- A deposit into an existing holding that is not metered is a modification: a new version is a new holding `within` the container.
- A mandate's allowance is charged in the resource named by the metered holding (`bench runs`), not its unit.
- Only `found` and `present` declare parties: an operative a household did not declare at its founding cannot be given a passport there.

- A notary's witness act cannot carry the field `episode`: a verifier rebuilds its envelope from the witness's own fields, so the field
  would leave every passport it witnesses unverifiable. The office's episode is recorded as conduct naming the act's seal (a join one way).
- The briefing names deposits by their container (`within`), the holding being new; a claim's claimant is the bearer, not the operative.
- A passport's model is its holder's (`holder.model`).

## Done

- `behaviour.py`: `episode` (Section 3.5's features), `fields_of`, the Merkle tree (`merkle_root`, `prove`, `included`, `series_head`,
  `node_head`) held to the engine's head; `due` after archeion's rule, `Payloads` with `retain` and markers; `may_read`; `arrangement`.
- `tools.py` (the restricted runner moved from `bench.py`, which keeps `Restricted` for the book), `node.py` (sessions, persistence, the
  office of status, the notary's witness, limits, holds from open claims, `retain`, `export` and `take` of a bag), `passports.py`,
  `registry.py`, `briefs.py`, `gateway.py`; `serve.py` (`/` by Accept, `/.well-known/noetarium.json`, `/c/...`, `--open-to-agents`,
  `--store`, `--round`, `/api/node/households` and `/api/node/principal` from this computer); `ONBOARDING.md`; `README.md`.
- Checks: `check_gateway.py` 37, `check_archive.py` 16 (on `tests/nodecase.py`); Koinonia 135 of 135.
- Not built: the page's views (a directory, passports, items, a party's trajectory) and their drive in headless Chrome.
