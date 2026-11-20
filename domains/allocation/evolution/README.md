# evolution: Heterogeneous Allocation Searched by Evolution

*Status: draft (2026-11-20; [ADR 0137](../../../development/decisions/0137-evolutionary-allocation-of-heterogeneous-resources.md)).*

## 1. Concepts

When what is given out is of many kinds at once (money that divides, crews in units, duties that may not be thrown away, sites that are distinct, a direction that has no algebra) and the order of giving matters (a price that rises after each job, a precedence, a stock that runs out), an allocation is a search over a space made of many spaces, read through an order, judged by evaluators of many kinds, and checked by the laws of what is allocated. This module composes the three foundations that hold those parts, and adds what names an allocation:

- **What is allocated** is declared by a kategoria resource type, verified when the problem is composed; a type that fails its laws is refused with the failing law. Every allocation found is checked by kategoria: each recipient's part is valid and the pool converts into the parts together. That verdict is a *verified constraint* in the search, so no unlawful allocation can be kept.
- **Where the search moves** is a product of heuresis's spaces, one for each resource's carrier (shares on a simplex, claims in a box, units in a composition, unit claims in an integer box, items as categories, fixed demands as data), each attribute's space, and the order (a permutation under Kendall's metric, or random keys).
- **How an order is read** is heuresis's grant system: a recipient's grant gives it all it requests when every pool it draws on holds it, its precedents are granted and the problem's condition allows it; a grant may change the problem's own quantities (a price). The serial decoder skips, defers or stops where a grant is not allowed.
- **How it is judged** is heuresis's assessment: evaluators with their kind and standing, ordered by tiers of evidence before Pareto. A model's opinion is never summed into fitness.
- **Each allocation is evaluated once:** its key is what the genotype requests and chooses with the trace of what it granted, so orders that differ only by grants that commute are one.

It answers the allocations kept, best first, each re-assessed and with its lawfulness; their comparison under declared principles (nomos's `welfare`), never one score; whether order mattered, with a witness; the redundancy the keys removed; and the search's record, sealed in archeion's journal.

## 2. Content

| Header | What it holds |
|---|---|
| `problem.hpp` | `Carrier`, `Resource`, `Attribute`, `Precedence`, `Problem`, `State`, `Allocation`, `Lawfulness`; `Composition` (verification of every type, the genotype, requests, the order, the grant system with footprints, `decode`, `independence`, `key`, `lawful`) |
| `search.hpp` | `evaluator` of decoded allocations; `lawfulness` as a verified constraint; `Settings`; `search` (the loop, the kept, the comparison, the order, the record); `compare_routes` for the price of generality |

## 3. Checks

`tests/check_evolution.cpp`; see ADR 0137 for what they found:

- a type whose declared conservation a conversion breaks is refused, with the law;
- a declared city case (three areas; money claimed in units; crews; maintenance duties that may not be thrown away; a site; a canopy's direction on the circle; a price rising after each job; a precedence): the optimum of every discrete allocation, found by enumeration, found by the search; every kept allocation lawful, the unlawful ones met refused by kategoria's verdict; the canopy facing south; the kept compared under three principles; order found to matter with a witness; the record verified and an altered entry found;
- four grants on two pools: 24 orders in 4 traces of 6, 4 keys;
- the multidimensional knapsack (instances generated as Chu and Beasley's): the direct encoding with LP-guided repair ahead of permutations and random keys decoded first-fit through the composition, as Raidl and Gottlieb found; the price of generality reported.

## 4. Dependencies

The domain's `rounds` (entities); kategoria's `resources`; heuresis's `spaces`, `sequential`, `assessment` and `evolution`; nomos's `welfare`; archeion's `fixity`; calx's linear programming (in the checks, for the knapsack's bound and duals).

## 5. Open Questions

- **Partial grants.** A grant is all or nothing; granting what remains of a claim is a further decoder rule.
- **Policies.** The order is a sequence; a policy that reads the state (evolved as heuresis's programs) is the design's research track.
- **Mechanisms run on words.** A picking sequence or a serial dictatorship as the genotype, its evaluation running nomos's mechanism, is the next case (Warsaw's budget).
