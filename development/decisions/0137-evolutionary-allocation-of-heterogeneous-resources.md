# 0137. Evolutionary Allocation of Heterogeneous Resources

- **Status:** Accepted, provisionally
- **Date:** 2026-11-20
- **Decided by:** the user. They asked for "a reusable/extensible universal envoluntionary resource allocation framework" in which "allocate A first then B not equal to B then A", the fitness may be "a economics value circulation simulation or even ABM, a LLM evaluator, a machine learning model, etc.", resource categories are "definable, formal verificable (by category theory foundation)" with "the representative algebraic space of the resource (like ... R^N, ... a set, ... other uncommon manifolds, etc)", and "generality is the most important thing"; asked for a survey (widened to agent simulation of economic circulation and to learning classical and quantum economic dynamical models from data) and "a well-organized implementation plan". They chose to build Stages 1–3 and review them first, and a declared city case as the first demonstration after the knapsack benchmark. After the review: "Continuer".

## 1. Context

The survey [Evolutionary heterogeneous resource allocation](../../research/literature/reports/Evolutionary%20heterogeneous%20resource%20allocation.md) (nine Parts and a synthesis) and the design [Evolutionary allocation of heterogeneous resources](../ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md) (eight stages) found:

- **Representation is where generality is won or lost.** Variation defined by a metric (Moraglio) composes over products of spaces when each part's distance is scaled by a declared unit; decoders bring redundancy and can exclude every optimum, so each must state its reach; Raidl and Gottlieb's controlled comparison on the multidimensional knapsack put a direct encoding with heuristic repair ahead of every decoder.
- **Order is a trace.** Allocation by grants is a word acting on a state; where grants commute, orders are one trace and should cost one evaluation; operators should keep the feature (precedence, adjacency, position) the objective depends on.
- **Resources are symmetric monoidal preorders.** Their laws can be verified with a stated scope (by construction, on generators, on a finite model, by search), and an allocation is a morphism whose lawfulness can be decided.
- **Evaluators establish different things.** A closed form, a simulation, a surrogate, a language model and a person cannot be summed; evidence should be ordered by tiers, and a model's opinion never enter fitness.

## 2. Decision

The framework is built in the foundations and composed in the allocation domain:

1. **kategoria's `resources`** (its decision 0004; 39 checks): resource types and their scoped verdicts, allocations as morphisms, the order of grants decided on finite systems, equivariance as naturality.
2. **heuresis's `spaces`** (its decision 0008; 83 checks): typed spaces with declared units, products, choices and constrained subspaces, laws for any space.
3. **morphe's normal forms of traces** (its decision 0021) and **heuresis's `sequential`** (its decision 0009; 48 checks): grant systems, commutation for allocation tested and declared, evaluation once per trace, decoders with their reach, operators by feature, words with fixed letter counts, Mallows's laws, the adaptivity bracket.
4. **heuresis's `assessment`** (its decision 0010; 30 checks), in the planned `noise`'s place: metrics with their uncertainty and standing, tiers before Pareto, races, cascades, a cache, surrogate agreement, Bradley–Terry.
5. **The allocation domain's `evolution`**, which declares heuresis and kategoria among its foundations: a problem of recipients, resources with their types and carriers (shares, claims, units, unit claims, items, fixed demands), attributes on spaces, an order (a permutation or random keys), precedence, a condition and an effect; the genotype composed from the spaces; decoding through a grant system; kategoria's verdict on every allocation as a verified constraint; the search on heuresis's loop keyed by requests, choices and the trace of what was granted; the kept compared under nomos's principles; order tested; the record sealed in archeion's journal; routes compared for the price of generality. Its theory note gains §6, with two propositions (what is decoded is lawful when kinds are disposable; one evaluation per allocation).

## 3. Alternatives Considered

- **One weighted fitness.** Refused by the survey: the order of evidence is stated by tiers, and principles are compared, never summed.
- **A genotype type for each problem.** General problems are declared as data at run time; one value tree, read by its space, serves them all.
- **A new domain for evolutionary allocation.** What it names is allocation; the domain holds rounds and verdicts already, and the search is one more way of finding allocations to judge.

## 4. Consequences

- The allocation domain has 96 checks (12 of them `evolution`'s); heuresis 753; kategoria 230; morphe's traces 19. What they found: §4.1.
- **Not built:** the edges (Stage 6: the API's composition, the drawings, the Urban Studio's tool); the economic evaluators (Stage 7: a conserving ledger, agents, density laws, learned laws); mechanisms run on words (Warsaw's budget); partial grants; policies evolved as programs.
- **Provisional:** the module names (`spaces`, `sequential`, `resources`, `assessment`, `evolution`); the city case's declarations, all illustrative.

### 4.1 What the Checks Found

- **A declared city case** (three areas; money claimed in units against a pool of five; two crews; two maintenance duties that may not be thrown away; a river site; a canopy's direction on the circle; a price that rises by a quarter after each job that spends money; area a's drainage before area b's works). All 153,600 discrete allocations were enumerated with the canopy facing south: 60,480 are lawful, and the best gives area b three units of money and both crews, area c two units, area a none (so its required first grant does not raise the price for the others), a duty each to a and b, and the site to a, in the order a, b, c. The search found it exactly, with the canopy within 10⁻⁴ radians of south; every allocation it kept is lawful, and 7,890 unlawful ones it met (duties held back, or left with an area whose grant was refused) were refused by kategoria's verdict as a verified constraint. Order mattered, with a witness at the first state; the record of 15 entries verifies, and an altered entry is found.
- **Found while building: the default strategy.** Survival of the best (μ + λ) settled in a deceptive basin (5.21 against the optimum's 5.33) on three seeds of seven, where reaching the optimum asks the money of two areas, the site and the order to change together. Age layers (ALPS) found the optimum on all fourteen seeds tried; restarts on stagnation and restricted tournaments on six of seven. ALPS is the composition's default.
- **Found while building: principles that cannot read an outcome.** A prioritarian principle with aversion needs outcomes above zero; when a kept allocation left an area with a negative benefit (a duty's burden), nomos refused the comparison. The search now records the refusal with its reason in the outcome and the record, and the case compares under the total, leximin and a shortfall below 1.
- **Keys:** four grants drawing on two pools have 24 orders in 4 traces of 6, as morphe counts, and 4 keys.
- **The multidimensional knapsack** (50 items, 5 constraints, tightness ½, generated as Chu and Beasley's; three instances, three seeds, 5,000 evaluations each): the direct encoding with repair by the LP relaxation's duals reached a mean gap of 1.25% to the LP bound; permutations decoded first-fit through the composition's grant system 5.14%; random keys 5.64%. Raidl and Gottlieb's ordering holds; the price of generality on this budget is 3.9 points, larger than their study's (0.33% against 0.61% and 0.63%), whose decoders were heuristically biased and whose budgets were larger.
