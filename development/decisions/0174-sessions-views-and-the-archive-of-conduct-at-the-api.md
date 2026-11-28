# 0174. Sessions, Views and the Archive of Conduct at the API

- **Status:** Accepted, provisionally; amended by [ADR 0175](0175-a-node-and-gateway-for-agents-of-any-provider.md)
- **Date:** 2026-11-28
- **Amends:** [ADR 0173](0173-communities-of-every-order-registries-organisations-and-trajectories.md)
- **Decided by:** the user, who approved the design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* ([its record](../ideas/2026-11-27-communities-that-agents-find-and-join/design.md)) and, on 2026-11-27, asked that its implementation continue so that agents of other providers can take up complex tasks; Claude, carrying out its Phase 9 within a scope narrowed to what the gateway (Phase 10) and the completion re-founded (Phase 12) need

## 1. Context

ADR 0173 recorded Phases 1 to 8: the ground in thesmos and the communities domain. A gateway that serves outside agents cannot fold a community's whole record at every act, since the cost then grows with the square of the record (the design's Section 16). It needs views of a community for a party, each community's conduct kept beside its acts, and a party's record to take away. Phase 9 gives the API these.

## 2. Decision

1. **Sessions.** The API's engine of communities (`api/cpp/science/communities.hpp`) holds sessions: `communities.open` folds a record once, `step` applies acts by thesmos's `step`, `view` answers from the folded state, `close` ends it. A step names the head it was made on, and a step on a stale head is refused with the head in force. A session's outcomes are those of a full fold of the same record.
2. **Views.** A session answers a community's state, profile (with its order, its positions' terms, its bodies' seats, the registries it is listed in and its envoys), briefing, items, posted work, a party's mandate chain, the behaviour taxonomy in force with the policy of records, a party's exit, provenance, holdings, and its head with a proof of consistency.
3. **The archive of conduct.** Each community's behaviour journal is kept as a series of its own beside its chronicle. A step may carry conduct, so that the episode is recorded, then the act citing it, then the outcome. `communities.join.verify` checks the join of the two records in both directions (Definition 31) and names the place where it breaks. A party's exit package (`noetarium.exit/1`) holds both records with their proofs. `communities.exit.verify` checks it against both heads, and archeion packs it as BagIt. The records are written as W3C PROV.
4. **Readings in an engine of their own.** Value readings (the design's Definition 33, three declared mechanisms, acts under a reading left out by H7's marks) and measures of power are answered by `api/cpp/science/community_readings.hpp`, an engine apart, so that nomos and tekmerion stay out of the session engine. Every reading names both heads, the join's verdict and its own digest, so that a second reader reproduces it (Proposition 15). The communities domain's `trajectories` gains the value readings and a light header, `episodes.hpp`, which the session engine includes.
5. **Registries at the API.** `communities.registry` answers a community's profile document byte for byte with its digest, listings, their verification, a registry's entries, its faults and search.
6. **Koinonia on sessions.** `Society.submit` steps the community's session and makes no second fold.

## 3. Alternatives Considered

| Alternative | Why it is set aside |
|---|---|
| A full fold at every act | Its cost grows with the square of the record |
| One behaviour journal for the whole node | thesmos's join reads one journal against one chronicle; a series per community is the archive's arrangement by community and needs no change of the ground |
| Readings and power in the session engine | The engine would include nomos and tekmerion and grow slower to build on every change of a header |

## 4. Consequences

- **Checks:** the API's `check_communities.py` 62 (20 new), the API 862; the communities domain 134; Koinonia 78; the whole run 21,219 of 21,219 in 85 suites.
- **Found while building:** a journal entry's Merkle leaf binds its seal and `about`, and leaves out its when, who, what and note fields. An exit package therefore carries each entry with the seal before it, and verification recomputes the seal, so that an altered `who` is found.
- **Not built:** bindings and custom through `science.law`, which needs an engine for the law domain's `sources`; voting power through `science.multilateral`; the domain's `organisations` at the API; the domain's recognition, weighing, contribution and tenure at the API; readings of open terms. Phases 10 to 13 remain.
