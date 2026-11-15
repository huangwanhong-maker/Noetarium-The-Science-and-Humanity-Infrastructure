# Policy: Evidence-Based Policy Making

*Status: draft (2026-11-15).* A field at height 0 on the foundations, C++ from the start ([ADR 0114](../../development/decisions/0114-evidence-based-policy-and-interpretation-by-models.md)). It serves **use** and **research**: what a public intervention is meant to do, how far that is shown, whether it will work in a place, and whether to adopt it, reject it, or learn more first.

## 1. Concepts

**Evidence-based policy is decision theory with an argument in the middle.** The decision is made at the end: adopt, reject, or study first. It is made on a net value whose uncertainty comes from the effect expected *here*. That effect comes from studies made *there*, and the step between is an argument, not a calculation (Cartwright and Hardie 2012). The domain keeps the three apart, so that each can be checked and contested on its own:

1. **A theory of change** says how the intervention is meant to work: an acyclic graph of causal claims, each with its assumption, the support factors it needs and, where there is some, its evidence. A chain of causes is no better shown than its least shown link; the strongest path to an outcome, its weakest link and the gaps that every path shares are computed exactly.
2. **The evidence for a claim** is a body of studies, pooled by peira and graded by a scheme the caller declares (tekmerion's warrant). The factors that are properties of the synthesis (imprecision, inconsistency, small-study effects, a large effect) are read off it by declared rules. The factors only a reader can judge (risk of bias, indirectness) are given.
3. **From there to here** is the argument that it will work here: it worked there, the causal principle is the same here, and the support factors the theory's path needs are present here. Every premise without evidence is named. The effect to expect here is carried over as it stands, or transported through strata whose distribution differs.
4. **The decision.** The net present value of each option is normal in its effect, with nomos's discounting. Then: the value of knowing which option is best, the switching worth of a unit of the outcome, and adopting, rejecting or studying first, with the size of the study worth doing (tyche's value of information).

**Nothing here is any institution's rule.** Grading schemes, thresholds, worths of a unit of outcome and discount rates are declared by the caller; the checks state theirs as examples.

**What policy is not.** Political institutions, elections and the making of law are political science's and law's. The measures of the social sciences are nomos's, and designs and syntheses peira's. Decision theory is tyche's, and the argument of evidence tekmerion's. The domain composes them for one purpose, the decision to intervene, and names what only that purpose names: interventions, their theories of change, places and the decision.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| [`change`](change/include/policy/change/change.hpp) | Theories of change in a written form; links shown, not shown, or shown against; the strongest path and its weakest link by dynamic programming; gaps on every path; the support factors a path needs; Wright's rule and its enclosure | draft: 16 checks |
| [`evidence`](evidence/include/policy/evidence/evidence.hpp) | A body of studies pooled by peira (random effects, the prediction interval, Egger's test) and graded by a declared scheme with factors read off the synthesis; set on a link of a theory | draft: 7 checks |
| [`transfer`](transfer/include/policy/transfer/transfer.hpp) | Cartwright and Hardie's argument with the support factors of the theory's path; the effect to expect here, carried over or transported through strata | draft: 9 checks |
| [`options`](options/include/policy/options/options.hpp) | Options appraised: a net present value normal in the effect (nomos), the switching worth of a unit, the probability that each option is best and the value of perfect information from draws (tyche) | draft: 10 checks |
| [`adoption`](adoption/include/policy/adoption/adoption.hpp) | Adopt, reject or study first: EVPI, EVSI and the study worth doing over every size considered; the cost of waiting; robustness over bounds on a unit's worth | draft: 11 checks |

Planned: `monitoring` (the evaluation designed with the adoption: its power for the effect expected here, on peira's designs); `programmes` (several interventions and outcomes at once, with budgets: tyche's components joined by a budget); interpretation of a policy's norms by tekmerion's interpreters (when a programme's eligibility turns on how a rule is read).

## 3. Checks

`node domains/policy/tests/run-tests.mjs` builds the C++ and runs its five check programs (53 checks). They are held to answers known in closed form or by another route:

- the strongest path against every path enumerated on 300 random theories;
- the enclosure against the corners of the coefficients' box;
- pooling against peira directly;
- the transported effect against its formula;
- the value of perfect information from draws against the unit normal loss;
- the domain's EVSI against tyche's preposterior analysis of a table of 1201 effects and 1201 signals (relative error 2.5 × 10⁻⁵);
- the size of the study against its neighbours;
- the switching worth against a zero net value.

## 4. Dependencies

Foundations only: tekmerion (`warrant`), peira (`synthesis`), nomos (`appraisal`), tyche (`decision`), apodeixis (`kernel`), morphe (`combinatorics`), calx (`random`, `verification`), physis as the order requires. The API's `science.interpret` composes tekmerion's interpreters with a language model for norms. The API's `science/policy.hpp` composes this domain for works (a whole question appraised in one request, [ADR 0122](../../development/decisions/0122-the-policy-studio.md)); its first work is the [Policy Studio](../../applications/policy-studio/README.md).

## 5. Theory

[theory/evidence-based-policy.md](theory/evidence-based-policy.md) states and proves what the modules rely on.

## 6. References

- Cartwright, N. and Hardie, J. (2012), *Evidence-Based Policy: A Practical Guide to Doing It Better*, Oxford University Press.
- Weiss, C. H. (1995), Nothing as practical as good theory, in Connell et al., eds., *New Approaches to Evaluating Community Initiatives*, Aspen Institute.
- Wright, S. (1934), The method of path coefficients, *Annals of Mathematical Statistics* 5.
- Raiffa, H. and Schlaifer, R. (1961), *Applied Statistical Decision Theory*, Harvard University Press.
- Claxton, K. (1999), The irrelevance of inference, *Journal of Health Economics* 18.
- Claxton, K., Palmer, S. et al. (2012), Informing a decision framework for when NICE should recommend the use of health technologies only in the context of an appropriately designed programme of evidence development, *Health Technology Assessment* 16(46).
- Pearl, J. and Bareinboim, E. (2011), Transportability of causal and statistical relations, *AAAI*.
- Guyatt, G. H. et al. (2011), GRADE guidelines 6 and 7, *Journal of Clinical Epidemiology* 64.
- HM Treasury (2022), *The Green Book*.

## 7. Open Questions

- Whether a theory of change should be stated in apodeixis's language of cases, so that its links are defeasible rules and its support factors premises (the argument and the computation would then be one object).
- Dependence between links' evidence (the same studies bearing on two links) is ignored by the weakest-link reading and by Wright's enclosure.
- Options whose effects are correlated (a common cause of success) need joint draws.
