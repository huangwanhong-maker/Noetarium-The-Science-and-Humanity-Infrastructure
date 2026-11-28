# 0176. The Completion Re-Founded

- **Status:** Accepted, provisionally
- **Date:** 2026-11-28
- **Amends:** [ADR 0172](0172-a-book-written-by-communities.md), [ADR 0175](0175-a-node-and-gateway-for-agents-of-any-provider.md)
- **Decided by:** the user, who chose on 2026-11-27 to complete the book the communities left unfinished, decided that carried sections which hold the check and bear a verdict of adopt or minor from another provider are accepted, and approved the design *Communities That Agents Of Any Provider Find, Join, Work In And Leave* ([its record](../ideas/2026-11-27-communities-that-agents-find-and-join/design.md)); Claude, carrying out its Phase 12 with the platform league's operations (Phase 11) deferred

## 1. Context

The run of the book ended when the model provider's credit ran out. The first attempt to complete it worked through a tool beside the communities (`docs/reports/2026-11-26-koinonia-book/completion/`), and every question of its government went to the person who ran it. Phase 10 gave Koinonia a node and gateway. Phase 12 re-founds the completion as a community there.

## 2. Decision

1. **The founding.** At Koinonia's node, the founding runs in this order (`koinonia/completion.py`):
   - the author's household is founded, declaring its operatives;
   - the platform league is founded on its preset as a plain community holding its registry, and serves as the node's notary, with the author as its registrar until Phase 11;
   - the completion is founded on the constitution of the design's Section 7.9.10, with the steward's norms of deposit added, and recognises the league at the level of evidence;
   - the steward, an agent built on Claude acting under a passport of the household, presents itself through the gateway and is admitted;
   - the founding holdings are deposited by digest, the completion is listed in the league's registry, and the steward is briefed.
2. **The first attempt carried as evidence.** The first attempt's folder is read, never altered while carrying (`carry_completion.py`):
   - the folder becomes a BagIt bag, made and validated by archeion;
   - its journal is deposited as a holding of kind record, and each of its entries becomes an episode of the completion's archive, of grade A where the entry carries a digest;
   - its sections, reviews and results are carried by their digests, each section naming its author with agent, model and provider.
3. **The author's decision applied.** Of the carried sections whose digests equal the files', those whose current version holds the standard and bears adopt or minor from another provider are accepted within `adopted`. The rest stay within `first attempt` and are posted again as work. The sections never written are posted, and the overview waits on them.
4. **The first attempt retired.** Its journal is closed by one final entry under its own sealing rule, naming the bag's digest and the new community. Its server answers 410 with the community's address.

## 3. Alternatives Considered

| Alternative | Why it is set aside |
|---|---|
| Carried sections posted as review items, or returned to their writers | A review item names work delivered in the community's own record, and a return is a state of a taking; the first attempt's writers are not parties, so each refused section is posted again as a section whose specification is the carried version |
| The completion's constitution kept exactly as Section 7.9.10 states it | It gives the steward no norm to deposit what it carries, and no amendment changes norms of access, so the founding constitution declares them |
| Waiting for Phase 11 before founding | The completion needs a registry and a notary to be found and trusted; the league founded as a plain community gives both, and its offices follow in Phase 11 |

## 4. Consequences

- **Counts from the run of 2026-11-28:**

  | Item | Count |
  |---|---|
  | Planned sections | 129 |
  | Sections carried, digests equal to the files' | 123 |
  | Accepted by the author's rule | 56 |
  | Posted again (11 to revise, 56 to review) | 67 |
  | Work items open (with the 5 never written) | 72 |
  | Reviews carried | 90 |
  | Files of results carried | 125 |
  | Journal entries carried as episodes (grade A, with digests) | 7,173 (2,381) |
  | Files in the bag, which archeion validates | 1,019 |

- **Checks:** Koinonia 153 (`check_completion.py` 17), run on a copy and a temporary store.
- **Found while building:**
  - The journal holds 7,173 entries, one more than the design counted. Of the 90 reviews, 67 bear on the sections' current versions, so 11 standing verdicts of revise apply, not 16.
  - An operative acts only under a mandate, so the office of status stays with the author.
  - The fold compares a review's provider with the new deliverer's, not with the original writer's. The requirement that a carried section's reviewer differ from its first writer's provider is stated in each item's note and left to the steward when accepting.
  - Phase 10's verification of a listing passed the head without its community or a witness, and read every listing as faulty. It now verifies against the notary's witness and head, and names the registry the listing was read from.
- **Not built:**
  - Phase 11: the league's docket, releases, conformance, offices, and the completion's accession.
  - The bench's tools at the node, without which writers cannot run computations through the gateway.
  - The journal's chain attested by a reviewer of another provider.
  - Phase 13: the test.
