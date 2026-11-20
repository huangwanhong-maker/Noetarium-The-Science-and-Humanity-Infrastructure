# 0140. The Research Tracks of Evolutionary Allocation

- **Status:** Accepted, provisionally
- **Date:** 2026-11-21
- **Decided by:** the user, before sleeping: "please continue all remain stage if unfinish. Then commit and push." The design's last stage is its Stage 8, four research tracks (`development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`), each from a gap its survey found (`research/literature/reports/Evolutionary heterogeneous resource allocation.md`).

## 1. Context

Stage 8 named four tracks "as the user chooses": surrogate stand-ins for agent evaluators with checked coverage (Part VII §6.1, item 3); policies evolved as programs, compared with sequences on one problem, which the survey found nowhere (Parts I and II, gaps); quantum-like sequential judgement models nested over their classical rivals with the parameter-free tests, and the counterbalanced experiment designed (Part IX); latent-space network models with their identifiability stated (Part VII §6.1, item 2). The user asked for all that remained.

## 2. Decision

1. **Surrogates with calibrated intervals** (heuresis's decision 0011, `assessment/conformal.hpp`): split conformal intervals around a Gaussian process, plain or normalised, with their guarantee; screening that keeps every candidate whose interval could hold the best and runs only those in the true evaluator. In the allocation domain's `evaluators` (`surrogate.hpp`), a stand-in for the economy of places: allocations read as their amounts and their places in the order of grants, the coverage checked on runs held out from fitting and calibration.
2. **Policies against sequences** (heuresis's decision 0011, `sequential/policies.hpp`): a rule followed through a grant system, learned once for a family of instances, linear by CMA-ES or a program by genetic programming, set beside a sequence searched for each instance and each instance's best order by enumeration.
3. **The order of two questions** (tyche's decision 0032, `testing/question_order.hpp`): saturated, order-free, q = 0 and projective models fitted and nested; the QQ equality by its z-test and shown within a margin by two one-sided tests. **Counterbalanced orders** (peira's decision 0014, `design/order.hpp`): the samples each order needs to find an order effect, to reject the equality, and to show it.
4. **Latent positions with their group** (tyche's decision 0033, `networks/latent.hpp`): the adjacency spectral embedding, generalised to signatures (p, q); every statistic tested against the declared group before it is reported; Procrustes alignment; the scree's profile likelihood; the bipartite ordering that the economic complexity index is.

## 3. Alternatives Considered

- **Policies by reinforcement learning, value functions or neural networks.** Dispatching rules searched by the library's own CMA-ES and genetic programming answer the question the survey asked (sequences against policies on one problem); learned value functions are a later representation.
- **Quantum models with complex amplitudes or in higher dimension.** The equality holds for all of them; the plane is fitted to show a projective fit, and the claims are the tests', not the plane's.
- **Hoff's latent distance model.** Planned; the spectral embedding shows the discipline of the group first, in closed form.

## 4. Consequences

- **Checks:** heuresis's conformal 8 and policies 10 (heuresis 771); tyche's question order 14 and latent positions 13 (tyche 1777); peira's order 5 (peira 451); the allocation domain's evaluators 21 (121 in the domain).
- **What the checks found:**
  - conformal intervals of a Gaussian process cover new runs 90.2% of the time at 90% over 200 replications; a stand-in for the economy of places covers 76% of held-out runs at 80% (within sampling) and, screening sixty candidate allocations, runs two in the economy and keeps the best;
  - on knapsacks whose price rises with every grant, a policy learned once reaches 95.5% (linear) and 95.3% (a program, value · ratio²) of each held-out instance's best order, against sequences searched per instance reaching 93.5% with 10 evaluations and 98.1% with 30: the policy is worth its training where instances are many and evaluations dear;
  - the QQ equality does not single out quantum-like models: a symmetric anchoring keeps it exactly, an asymmetric Markov update of the first answer breaks it; with 100 respondents per order a true equality is not shown within 0.05, and with 2,000 it is;
  - positions of a random dot product graph are recovered as 1/√n; in a disassortative network distances between positions are not identified, which the test of the group says before anything is reported.
- **Not built:** the stand-in inside the evolutionary loop (a search that screens each generation); policies on the allocation domain's composition and on its city case; quantum-like models of allocation choices fitted to an experiment; Hoff's model and dynamic embeddings; the economic evaluators at the API and in the Urban Studio.
