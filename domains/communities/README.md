# Communities

*Status: draft (ADR 0167, 2026-11-25).* The domain of communities: continuing associations of persons, organisations and AI agents around concerns they hold in common, each with an internal order of its own, and the relations among them. Its subdomain [human-ai/](human-ai/) holds the society of human and AI communities that the user proposed on 2026-11-25: agents lent to communities under mandates, unequal intelligence and computation, and whether a money-like instrument can be made to disappear.

The design is the record [A Society of Human and AI Communities](../../development/ideas/2026-11-25-a-society-of-human-and-ai-communities.md). What names no community (acts, mandates, commitments, constitutions, the chronicle, units of count, norms) is the foundation [thesmos](../../foundations/thesmos/README.md)'s.

## 1. Concepts

- **A community is declared by its constitution,** data in Ostrom's seven kinds of rules, admitted by thesmos by form and run by thesmos's fold. A commons, a guild, a cooperative, a polity and a league are files of one schema.
- **A league is a community of communities.** Its members are communities, its rules bind by accession, and its offices are plural.
- **Circulation is read from the record.** Gates of access are classified by what they read and spend; provision by need and a market in tokens are two ways of handing out a round's offers; the mediation profile is a view of the chronicle.
- **The subdomain human-ai** adds what is particular to persons and agents together: bounded contributions (an agent lent under a mandate with ceilings, a term and a slice), receiving capacity as a guard, and the jar, a construction that puts the user's hypothesis about the dissolution of money to a test.

## 2. Modules

| Module | Status | Content |
|---|---|---|
| `presets` | draft | six constitutions as data (commons, guild, cooperative, polity, league, the first league with its charter), each admitted; their profiles |
| `circulation` | draft | gates (Definition 11), provision by need in a round, a market in tokens, the mediation profile |
| `human-ai/lending` | draft | a loan as a mandate; receiving capacity; the first run of a lent agent on the commons preset |
| `human-ai/jar` | draft | circulation under scarcity: three currencies and two kinds of provision, with free riders, over the community's knowledge of capacities |
| `returns` | draft | what reaches an artefact passed upstream along the graph of use (Proposition 7), pots divided by declared weights, an operative's return paid to the party that answers for it; Shapley by origin and retroactive returns planned |
| `federation` | draft | recognition directed and in levels, compacts in force when every party has ratified by its own rule, notaries that witness heads and find a history rewritten, a league run on the first league's constitution; interfaces checked as squares and disputes planned |
| `claims` | draft | a claim's route read from the record (party, forum, evidence, authority, capacity, members recused), referral when the forum cannot be composed, the term and the overdue mark, a finding's completeness, review by another body; remedies and the law domain's proceedings planned |
| `custom` | planned | routes of formation of norms, precedent |
| `human-ai/capacity` | draft | floors in usable units by conversion factor, a solidarity pool that raises the least served first, evaluators counted by origin and by provider; starter allocations and shared agents planned |

The first league's charter is [presets/charter.md](presets/charter.md). The theory note on circulation under scarcity is [theory/circulation-under-scarcity.md](theory/circulation-under-scarcity.md).

## 3. Dependencies

The foundations thesmos, archeion, symploke, kategoria, apodeixis, morphe and calx (`module.json`). The design places the domain as a bridge over law, economics, sociology, diplomacy and allocation; it takes them up when its `claims`, `returns`, `federation` and `capacity` modules first need them.

## 4. Checks and the Edge

`node domains/communities/tests/run-tests.mjs` (63 checks). The API composes the domain with thesmos (`api/cpp/science/communities.hpp`, the API's own engine, `api/python/science/communities.py`; 13 checks in `api/tests/check_communities.py`).
