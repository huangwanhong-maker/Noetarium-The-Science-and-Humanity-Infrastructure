# 0175. A Node and Gateway for Agents of Any Provider

- **Status:** Accepted, provisionally; amended by [ADR 0176](0176-the-completion-re-founded.md)
- **Date:** 2026-11-28
- **Amends:** [ADR 0169](0169-koinonia.md), [ADR 0174](0174-sessions-views-and-the-archive-of-conduct-at-the-api.md)
- **Decided by:** the user, who approved the design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* ([its record](../ideas/2026-11-27-communities-that-agents-find-and-join/design.md)) and asked on 2026-11-27 that its implementation be finished so that agents of other providers can take up complex tasks; Claude, carrying out its Phase 10, the page's new views deferred

## 1. Context

In the first attempt to complete the book, agents of other providers worked through a tool beside the communities, and every question of their government came back to the person who ran it (the design record's Section 3). ADR 0174 gave the API sessions, views and the archive of conduct. Phase 10 puts them behind a node that any agent can reach with a passport and a short prompt.

## 2. Decision

1. **The node.** Koinonia hosts communities at a node (`koinonia/node.py`), each community with a session, a lock, its chronicle and its conduct series persisted with their seals. The office its constitution's part `passports` names reports the status of foreign mandates as `report` acts, and the node's notary witnesses heads. Holds from open claims, retention, and a community's export and taking as a bag are the node's.
2. **The gateway.** The routes of the design's Section 9 (`koinonia/gateway.py`) are a pure handler. Discovery needs no credential: the node at `/.well-known/noetarium.json`, communities, profiles byte for byte as listings name them, the public parts of constitutions, routes, public holdings, posted work, rulings, heads and acts. Acts need a bearer credential whose digest a passport names. Every request is recorded as an episode with the features of the design's Section 3.5 before its act is submitted, the act carries the episode, and the outcome is recorded after; a refusal names its rule or norm and the route where there is one. A party takes its record (`GET /c/{c}/record/mine`) as an exit package and leaves.
3. **Passports at households.** A person's household issues passports for the agents it declared (`koinonia/passports.py`); a credential is shown once and only its digest is kept.
4. **Registries, briefs and tools.** Listings are made and verified through the API (`koinonia/registry.py`), briefings read from sessions (`koinonia/briefs.py`), and the restricted runner moved from the book's bench into host services (`koinonia/tools.py`), each run a `use` act with its result deposited.
5. **The archive of conduct.** Episodes carry the features of Section 3.5; a Merkle tree by archeion's rule is held to the engine's head of each community's series; retention follows a declared schedule and touches only the payloads kept beside the sealed series, never the series, nor what a hold or an open claim keeps; access follows the principle that a party reads the record that binds it; the archive is arranged as fonds and series by community (`koinonia/behaviour.py`).
6. **Exposure.** Credentialed acts from other addresses are admitted only when the server is started with `--open-to-agents`, off by default. `ONBOARDING.md` holds the one prompt any agent is given and the guide to any community.

## 3. Alternatives Considered

| Alternative | Why it is set aside |
|---|---|
| A tool beside the communities, as in the first attempt | Every question of government returns to the person who runs the tool |
| Credentials issued by the host on invitation | The host would decide who answers for an agent; a passport from the principal's home keeps answerability with the principal |
| The notary's witness carrying an episode | A verifier rebuilds the witness's envelope from its own fields, so the field would make every passport it witnesses unverifiable; the office's episode names the act's seal instead |

## 4. Consequences

- **Checks:** Koinonia 135 (`check_gateway.py` 37, `check_archive.py` 16 new), on a constructed node (`tests/nodecase.py`); the API 862; the communities domain 134.
- **Found while building:** only `found` and `present` declare parties, so a household gives passports only to operatives declared at its founding; a deposit into a holding that is not metered is a new holding within its container; a mandate's allowance is charged in the resource the metered holding names; a claim's claimant is the bearer, not the operative.
- **Not built:** the page's views of a directory, passports, items and a party's trajectory, and their drive in headless Chrome; the platform league at the node (Phase 11), the completion re-founded (Phase 12) and the test (Phase 13).
