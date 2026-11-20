# allocation

*Status: draft (2026-11-20; [ADR 0136](../../development/decisions/0136-allocation-under-scarcity.md)). Field, height 1, on economics; C++ from the start.* What is allocated when there is not enough, to whom first, what and how much, and by what procedure: allocative justice, substantive and procedural, made computable without being made into one optimisation.

## 1. Purpose

- **Allocation is a composition of declared rules.** When a resource falls short, it is divided by stages applied in turn to what remains: who is eligible, a floor closed first, priority classes served in order, claims rationed by a rule whose properties are known, shares by points or in proportion, equalisation of needs against capacities. Each stage serves a named principle, declared by someone, for a stated reason. The domain runs a round and tells each entity what each stage gave it and why, and in what order the round reached it: who first, then who.
- **Principles are orders, and they conflict.** A round is one of many that could have been declared. The domain enumerates its alternatives (every rationing rule a stage could have used, every order of its stages), reads each as an outcome per person, and compares them under declared principles at once (nomos's `welfare`: leximin, prioritarian, sufficiency, opportunity). It reports the options no other beats under every principle, the pivots between them, and where a parameter flips a comparison. It never adds principles into one score: Fleurbaey and Tungodden's tyrannies forbid it, and the choice of precedence is a person's.
- **Procedure confers legitimacy, not correctness.** A round's record is archeion's hash-chained journal: published, consulted, applied, appealed, heard and revised, a lottery drawn from a seed committed before it. The domain checks the record against the checkable rules of procedural justice (Leventhal's six, accountability for reasonableness), recomputes an appeal for everyone similarly placed, and leaves relevance and ethicality to people.
- **Who finally gains is not who first receives.** Money directed at a place is re-spent, partly elsewhere, and partly capitalised into rents. The domain follows it: re-spending and LM3, capitalisation with exact intervals over declared responses, the return to generators on a network of flows (a reading proposed after Eglash's generative justice), floor and ceiling, and targeting.
- **Nothing in it names a place, a fund or an authority.** Those come from the works and the data library. No amount, threshold, weight or response in it is anyone's until a caller declares it.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `rounds` | A round: a resource, its amount, entities with attributes, and stages of seven kinds (eligibility, floor, priority, claims, proportional, points, equalisation), each with its principle, its source and the share of what remains it may use; the result by stage with reasons and exclusions; who was reached first; the attributes a round reads | draft |
| `verdicts` | A round against its alternatives (rules and orders) under declared principles: the distinct options, the undominated, the best under each principle, the pivots, the flips of a parameter, what is at stake for each entity among the undominated | draft |
| `procedure` | The canonical text and SHA-256 of a round and a result; the record as a sealed journal of acts (declare, consult, interest, oversee, allocate, appeal, review, commit, draw); appeals on a datum, a parameter or a stage, heard for everyone; lotteries from committed seeds, equal or weighted (Efraimidis and Spirakis), repeatable by anyone; the rules of procedural justice checked (record sealed, consistency, publicity, reasons, relevance, accuracy, bias suppression, representativeness, correctability, lottery, ethicality, enforcement) | draft |
| `incidence` | Capitalisation into rents with exact intervals for renters, owner-occupiers and landlords; re-spending, the local multiplier and LM3; the return to generators and the circulating share of a network's flows (economics's Hodge decomposition); floor and ceiling; targeting | draft |
| `seasons` | Allocation over time: rotation, histories of who was passed over, rules revised by monitoring (Ostrom's adaptive governance) | planned |

## 3. Dependencies

| Source | What allocation uses from it |
|---|---|
| nomos | `allocation` (rationing rules, the formulas of practice, reserves, assignment, division, budgeting), `welfare` (principles as orders, dominance, comparison), `games` (claims), `measurement` (its decision 0015) |
| archeion | `fixity`: the hash-chained journal and SHA-256 |
| apodeixis | `kernel`: verdicts that hold, fail or are undecided, with their reasons |
| economics | `circulation`: the Hodge decomposition of flows on a network, for the circulating share |
| calx, morphe, physis, tyche | declared as their order requires; calx's verification and random streams in the checks |

## 4. Checks

84 (`*/tests/check_*.cpp`, run by `tests/run-tests.mjs`):

- **Rounds (24).** The Talmud's estate of 200 by a claims stage; a floor in plenty and in scarcity (the acequias' switch); eligibility with its reason; priority classes; a claims stage counting earlier awards; banded points agreeing with nomos's; equalisation exact with no grant below zero; a stage's share; refusals by name; conservation and non-negativity on 200 random rounds.
- **Verdicts (14).** Claims of 10 and 100 on 60: the Talmud rule and adjusted proportional merged, equal losses dominated, equal awards and the Talmud rule undominated with a pivot between leximin and the headcount; equal claims giving one option of seven names; an order of stages dominating another; an unequal and an equal outcome exchanging places once as aversion rises; an entity with nobody counted left out and said to be.
- **Procedure (25).** Digests; a clean record; each fault found (applied before published, a result that does not follow, a datum without a source, an interest in what a stage gave, a group not consulted, an appeal not heard, a revision not applied, an entry altered); an appeal heard for everyone; lotteries repeatable, a quarter of first places for each of four over 4,000 seeds, and a quarter, a quarter and a half for weights 1, 1, 2.
- **Incidence (21).** Capitalisation's interval held to a grid over the box of elasticities, the value conserved at both ends; re-spending's rounds, total and LM3, the survey's example of 2.2; return ratios and capture; a pure cycle circulating wholly and a one-way flow not at all; flows of two kinds read apart; floor and ceiling; targeting.

## 5. Used By

- **The API's `science/allocation.hpp`** and `science.allocation`, which compose the domain for any work, with a language model's three roles at the edge.
- **The Urban Studio's plug-in *Allocation and justice***, on England's districts (IoD 2025), Melbourne's areas (the 2021 Census) and Warsaw's participatory budget (Pabulib).

## 6. Open Questions

- **Alternatives beyond rules and orders.** Variation enumerates rationing rules and orders of stages; varying a stage's kind, its shares and its thresholds, and searching the space of rounds with heuresis's quality-diversity archive (the survey's proposal), are not built.
- **Time.** A round is one period. Rotation, queues, histories of who was passed over, and rules revised by monitoring are the planned `seasons`.
- **Contestation as argument.** An appeal is recomputed; reading appeals as attacks in apodeixis's argumentation frameworks, so that a decision's standing is recomputed, is the survey's design and not yet built.
- **Incidence in equilibrium.** Capitalisation is read through declared elasticities; the urban-science domain's `housing` computes equilibria that could carry it, and the composition is not made.

## 7. References

Elster (1992), *Local Justice*; Moulin (2000, 2003); Thomson (2003, 2015); Fleurbaey and Tungodden (2010); Leventhal (1980); Daniels and Sabin (1997, 2008); Ostrom (1990); Eglash (2016); Santos (1998); Pathak, Sönmez, Ünver and Yenmez (2024); Peters, Pierczyński and Skowron (2021); Efraimidis and Spirakis (2006); Sacks (2002); Roback (1982); Kline and Moretti (2014). The infrastructure's survey *Allocative justice under scarcity* (`research/literature/reports/`) collects the literature.
