# 0136. Allocation under Scarcity: Who First, What and How Much, and by What Procedure

- **Status:** Accepted, provisionally
- **Date:** 2026-11-20
- **Decided by:** the user. They asked to move to "the allocative's substantive justice, and procedure justice problems. When resources (like finances) are limited, how should allocate resources, and allocate to which entity first, then which entity, and allocate what, how much, etc.", holding that "it is not an formal optimization, it needs consider humanist, welfare, justice, which are not always can be an optimization problems", that circular economics, generative justice (Eglash) and evolutionary economics could help, and asked for a survey and the useful ideas implemented first. Asked where it belongs, they chose a new domain `allocation` (a field at height 1 on economics), an Urban Studio plug-in, and as first cases participatory budgets (Pabulib), England's council funding (IoD 2025) and Melbourne. Then: "continuer".

## 1. Context

The infrastructure held claims problems and the Shapley value (nomos's `games`), matching and social choice, inequality and poverty measures, multi-criteria appraisal with SMAA, decisions under ambiguity (tyche), argumentation (apodeixis), sealed journals (archeion) and the Hodge decomposition of flows (economics). Nothing composed them into allocation. The survey [Allocative justice under scarcity](../../research/literature/reports/Allocative%20justice%20under%20scarcity.md) (five parts: theories of distributive and procedural justice; formal methods of fair allocation; urban and spatial justice and its economics; generative, circular and evolutionary economics; practice, safeguards, tools and language models) found:

- **Allocation is a composition of declared rules, not the maximisation of one objective.** Practice (Elster's local justice, health priority setting, social housing, fiscal equalisation, participatory budgets) composes constraints, floors, priorities, reserves and rationing rules whose properties are known.
- **Principles are orders that conflict, and their conflicts are theorems.** A complete aggregation falls into one of Fleurbaey and Tungodden's two tyrannies; partial orders, undominated sets and the pivots of a comparison replace the maximum.
- **Procedure confers legitimacy, not correctness.** Leventhal's rules and accountability for reasonableness give checkable criteria of a record; the failures (Robodebt, the Dutch childcare benefits, the 2020 English exam grades) were appeals that settled cases while the rule stood.
- **In the city the question is who finally gains**: incidence through re-spending and prices as well as where the money lands.
- **Generative, circular and evolutionary perspectives supply a direction, a shape, conditions and a repertoire, not a verdict.** Generative justice gives a direction (value back to its generators), the doughnut a shape (a floor under a ceiling), Ostrom conditions, evolutionary economics a repertoire of rules to compare.
- **Language models may organise, articulate, locate disagreement and draft, but must not choose, weigh or speak for the absent.**

## 2. Decision

1. **nomos's `allocation` and `welfare`** (its decision 0015; 50 checks): rationing rules beyond the claims problems, the formulas of practice (sufficiency first, banded points, equalisation), reserve systems with their axioms and every precedence, sequential and random assignment with Birkhoff's lotteries, fair division's tests and procedures, participatory budgeting with EJR; principles as orders, Lorenz, generalised Lorenz and sequential dominance, and options compared under every principle at once.
2. **A domain `allocation`**, a field at height 1 on economics, with nomos, apodeixis and archeion among its foundations, C++ from the start (84 checks; its note `domains/allocation/theory/allocation-under-scarcity.md`):
   - `rounds`: a round as stages of seven kinds (eligibility, floor, priority, claims, proportional, points, equalisation), each with its principle, its source and its share of what remains; the result by stage with reasons; who was reached first;
   - `verdicts`: a round against its alternatives (rules and orders) under declared principles: the undominated options, the best under each principle, the pivots, the flips, what is at stake;
   - `procedure`: the round's record in archeion's journal, appeals heard for everyone, lotteries from committed seeds (Efraimidis and Spirakis), and the rules of procedural justice decided, relevance and ethicality left to people;
   - `incidence`: capitalisation with exact intervals, re-spending and LM3, the return to generators with the circulating share on the economics domain's Hodge decomposition, floor and ceiling, targeting.
3. **The API's `science/allocation.hpp`** (run, judge, procedure, claims, budget, reserves, incidence) and `science.allocation`, with a language model's three roles at the edge: stages proposed (no numbers), an objection restated as an appeal, the statement of reasons drafted and refused if it states a number the result does not hold (30 checks).
4. **The data library:** `formats/pabulib.py` and `products/allocation_cases.py` (Warsaw's 2023 budget from Pabulib, kept in the cache; England's districts from the IoD 2025's File 7, OGL; the City of Melbourne's areas from the needs product); registry entries `pabulib` and `mhclg-iod-2025` (10 checks, among them Birmingham's 41.7% and Blackpool's 26.0% as the release's Table 7).
5. **The visualization layer's `allocation`** (19 checks): a round as a channel and its plots in the order reached; options ordered by dominance as a Hasse diagram; what is at stake; the sealed record with the rules as seals; a budget by group.
6. **The Urban Studio's plug-in *Allocation and justice*:** four tools, an engine, a server side, the view *Allocation*, three templates (England, Melbourne, Warsaw).

## 3. Alternatives Considered

- **One social welfare function, maximised.** Rejected for the reasons the survey gives; a weighted aggregate appears only where a person declares weights (nomos's SMAA), beside the undominated set.
- **A domain at height 0 on the foundations alone.** Rejected at the user's choice: the circulating share of a network's flows is the economics domain's decomposition.
- **Allocation as a module of economics or of urban science.** Rejected: allocation is decided in health, housing, schooling, research funding and law as well as in cities, and composes logic and archives that economics does not declare.
- **Quality-diversity search over rounds** (the survey's proposal for evolutionary economics). Deferred: the enumeration of rules and orders is exact for the rounds built; a search over stages' kinds, shares and thresholds on heuresis's archive is the next step.
- **Appeals as attacks in argumentation frameworks.** Deferred: an appeal is recomputed for everyone; reading it in apodeixis's frameworks so that a decision's standing is recomputed is designed in the survey and not built.

## 4. Consequences

- **What can now be asked of any allocation:** what each receives and why, stage by stage; who was reached first; which rounds no other beats under the principles declared, and where the choice turns on precedence; what is at stake for each party; whether its record meets the checkable rules of procedural justice; who finally gains.
- **What the first cases show** (illustrative declarations on real data):
  - **Warsaw 2023:** utilitarian greedy reproduces the city's selection of 22 projects; under it 7.9% of 78,904 voters saw none of their projects funded, under equal shares 4.7% (60 projects) and under Phragmén 4.4% (84); in Białołęka 16.6% against 12.9% under equal shares.
  - **England:** with a £1 bn fund whose claims of £80 a person exceed it, proportional rationing is undominated under the three principles read per income-deprived person, and equal awards to districts is dominated (its least £12.74 a person against £66.05); Porto Alegre's banded points, which give a small district as many points as a large one in the same bands, favour the small per person (the City of London £359, Birmingham £66). A fund to the most deprived decile of neighbourhoods reaches income-deprived people with 55% of its money, while 76% of them (10.1 m of 13.2 m) live elsewhere.
  - **Melbourne:** serving the areas with the highest share needing assistance first leaves fewer people in areas short of A$1,000 each; giving by low income first gives more to the least served per person; the choice turns on which principle is given precedence.
- **Not built:** `seasons` (rotation, histories, revision by monitoring); alternatives beyond rules and orders, searched by quality-diversity; appeals in argumentation frameworks; incidence through the urban-science domain's housing equilibria; Pol.is and deliberation; reserve systems and lotteries in the plug-in's page.
- **Provisional:** the domain's and the plug-in's names; the plug-in's colour; the examples' declarations.
