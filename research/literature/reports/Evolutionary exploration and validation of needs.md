# Evolutionary Exploration and Validation of Needs: A Survey

*Evidence-based decision making, causal analysis and subgroup discovery, complex systems, policies evolved against simulators, emerging evolutionary methods, formal reasoning, and needs assessment, for a search over statements of need that is not allowed to deceive itself.*

Status: `draft` (a survey for design, 2026-11-18). Activity: research, in service of use. It serves the needs domain (ADR 0125; its theory note `domains/needs/theory/needs-and-their-discovery.md`).

## 0. Scope, Method and Conventions

### 0.1 Purpose

The needs domain states a need as a sextuple $N = (G, D, I, \sigma, w, P)$: a group $G$ (a conjunction of conditions on units), a domain of satisfaction $D$, an indicator $I$, a standard $\sigma$ of one of Bradshaw's four kinds (normative, felt, expressed, comparative), a weight $w$, and optionally a provision $P$. A population of such statements is generated (by a language model, enumeration of the language, subgroup discovery, rules, simulation), tested (statistics with held-out confirmation and false-discovery control, causal analysis, agent-based simulation, formal rules), and evolved by a genetic algorithm whose every stage has interchangeable strategies. Robust statements are archived; old ones are retired after long inefficacy.

This survey collects what the literature offers each stage of that loop, in seven areas the user named, and one cross-cutting area that the design depends on most: false discovery under adaptive search (§3). For each area it gives the concepts, the main methods with what they assume and guarantee, key papers, what is implementable in C++ or Python and how it would serve the loop, and open problems. §10 draws a design from the whole; §11 ranks the most valuable methods by where they would go.

### 0.2 Verification Markers

Every bibliographic item carries one marker.

- **[V]** verified this session (2026-11-18) against a primary record: the DOI resolved on Crossref's API and its metadata (authors, title, venue, volume, pages) matched; or the arXiv API returned the record; or the official proceedings page (PMLR, ICLR, JMLR) was found. Volume, issue and pages are given only where the record gave them.
- **[S]** confirmed only through secondary sources (library catalogues, publisher listings, citing pages); the primary record was not read.
- **[M]** from prior knowledge, not verified this session. Treat as a lead, not a citation.

Summaries of a paper's *findings* come from its abstract where this report says "(abstract read)"; otherwise they come from prior knowledge of the paper and should be checked against the text before they are quoted.

### 0.3 What the Infrastructure Already Holds

The recommendations are fitted to what exists. Read on 2026-11-18:

| Library | Relevant holdings | Gaps that matter here |
|---|---|---|
| tyche | `testing` (Bonferroni, Holm, BH, BY, Storey's q-values, permutation, FastQTL's beta), `decision` (EVPI, EVSI, decision trees, POMDPs), `bayes` (conjugate, adaptive Metropolis, HMC, R̂), `uncertainty` (Latin hypercubes, Morris, Sobol'), `graphical`, `beliefs`, `mixed`, `latent`, `learning` | no online FDR, no e-values or e-processes, no reusable holdout, no subgroup discovery, no ABC or history matching, no emulators |
| peira | `graphs` (d-separation, back-door, front-door, adjustment sets), `quasi`, `weighting` (IPW, doubly robust), `surveys`, `sensitivity` (E-value, Manski, Rosenbaum, robustness value), `protocol` (pre-registered plans and deviations), `synthesis` (meta-analysis, transport), `small_areas` (Fay–Herriot), `detection` (occupancy, N-mixture, capture–recapture), `disclosure` | no heterogeneous-effect estimators (causal forests, DR- and R-learners, GenericML), no policy trees |
| nomos | `agents` (stated schedules, Gillespie, logit choice, Schelling), `appraisal` (MCDA, CBA), `measurement` (inequality, poverty, accessibility, composite indicators), `microsimulation` | no Alkire–Foster counting measure |
| heuresis | ask–tell protocol, islands, `selection` (lexicase, ε-lexicase), `diversity` (archives, MAP-Elites, novelty, niching), `multiobjective` (NSGA-II/III, MOEA/D, SMS-EMOA), `programs` (GP, grammatical evolution), `models` (GP surrogates), external proposers; the untracked `evolution/` (decision 0006 in progress) adds adaptive operator selection (probability matching, adaptive pursuit, UCB1, Thompson) and survival by aging | `coevolution` planned, not written |
| apodeixis | Dung's frameworks, ASPIC+, calculi as rule schemata, D(φ) under interpretation, Boolean minimisation | no defeasible deontic logic, no ILP or abduction |
| tekmerion | `warrant` (grading by declared schemes, Cartwright–Hardie), `reviews`, `recognition` (grades A, B, C), `interpretation`, networks and belief functions | none critical |
| zetesis | `sets` (calibration, consistency, coverage), `qca`, `coding`, `cases`, `tracing` | none critical |
| kategoria | finite categories, functors, Kan extensions, set-valued functors, presentations, monoidal theories, scripts | none critical |
| kinesis | `induction` (templates fitted, sparse discovery, candidates ranked on a held-out part, surrogates) | none critical |
| policy domain | `change`, `evidence`, `transfer`, `options`, `adoption` (EVSI over study size) | no robustness metrics over scenarios |
| needs domain | `groups` (conditions, canonical keys, subsumption, join and meet) | everything else is being written |

A search of the C++, Python and JavaScript of `foundations/`, `domains/` and `api/` found no subgroup discovery, exceptional model mining, PRIM, approximate Bayesian computation or history matching outside the needs domain's own `groups`.

### 0.4 The Contract Every Component Should Keep

One principle runs through §10–§11, and it is already the infrastructure's rule for kategoria, apodeixis and tekmerion: a component returns a *candidate with its evidence*, and a separate check that does not share its code decides its standing. Applied to the needs search, it has a statistical form: **the search may see the discovery data; a statement's standing is decided only by tests the search did not see, and the record of those tests says that they were stated before they were run.** §3 shows that the theory note's current safeguards fall short of this in two places (§12.2).

---

## 1. The Problem a Search for Needs Must Solve

### 1.1 A Statement of Need Is a Hypothesis Indexed by a Standard

A statement of need is a hypothesis about a population: that the units satisfying $G$ fall short on $I$ by the standard $\sigma$. It has three components that the literature treats separately.

1. **A descriptive claim:** the gap $\Delta$ is positive in the population the data represent. This is estimation and testing (§2, §3), with the special difficulty that $G$ was found by search (§4.5).
2. **A causal claim,** when a provision $P$ is declared: provision would close the gap. This is causal inference on observational data (§4).
3. **A normative claim:** the standard is the right one. Bradshaw's taxonomy says whose judgement makes a gap a need; the theories of need (§9.1) say which standards are defensible; argumentation (§8.3) can represent standards that conflict.

A fourth claim is sometimes made and must be kept apart: that a declared *mechanism* produces the gap (a model of seeking satisfiers within reach of capacity). That is the generative sufficiency of agent-based modelling (§5), which shows that a mechanism *can* produce the gap, not that it does.

### 1.2 Three Ways a Search for Needs Deceives Itself

1. **False discovery under adaptivity.** A search that evaluates $10^4$ statements on the same data finds large gaps by chance, and selection concentrates on them (Gelman and Loken's forking paths; Ioannidis's low prior odds). When the confirmation data also influence the search, even held-out tests lose their validity (Dwork et al. 2015). §3.
2. **Confounded gaps read as needs that provision would meet.** A group's gap may follow from something the data do not hold; a group found by maximising a gap is the group whose confounding is most favourable to the gap. §4.
3. **Overfitting to a model or a judge.** An intervention evolved against one simulator is tuned to its errors (§6); a hypothesis judged by a language model is tuned to the model's tastes (§7.9). The literature's answer in both cases is the same: evaluation outside what the search optimised against.

### 1.3 Where Each Body of Literature Enters the Loop

| Stage of the loop | Literature |
|---|---|
| Generation | subgroup discovery and EMM (§4.5), causal trees and GATES (§4.4), PRIM (§6), ILP and abduction (§8.1–8.2), defeasible rules (§8.4), simulation (§5), language models (§7.7–7.8) |
| Variation | grammatical evolution, LLM crossover (§7.7), refinement operators of subgroup discovery (§4.5) |
| Evaluation | quality measures (§4.5), heterogeneous effects (§4.4), calibrated simulation (§5), robustness over scenarios (§6) |
| Selection and survival | lexicase, novelty and QD, aging (§7.3–7.5), coevolution (§7.2), adaptive operator selection (§7.6) |
| Admission to the archive | online FDR, e-values, the reusable holdout, pre-registration (§3), grading (§2), argumentation (§8.3) |
| Retirement | anytime-valid monitoring (§3), aging (§7.5), revisable standing (§2) |
| Action on a need | value of information, MCDA, transport (§2), policy trees (§4.4), RDM (§6) |

---

## 2. Evidence-Based Decision Making

### 2.1 Concepts

Evidence-based decision making separates three things the needs domain should also separate: the **certainty** of a body of evidence for a claim; the **synthesis** of several pieces of evidence into one estimate; and the **decision** to act, which depends on the evidence and also on values, costs and the worth of learning more. GRADE made the first separation canonical: the certainty of the evidence and the strength of a recommendation are judged apart. Claxton (1999) made the third: a decision should follow expected net benefit, whatever the significance of the estimate, and the question of whether to collect more data is answered by the value of information, not by a p-value. Cartwright and Hardie (2012) added the question of transfer: that something holds *there* is evidence that it will hold *here* only with an argument about the support factors that make it hold.

For needs, the analogues are direct. A statement's *standing* (robust, supported, unsupported, refuted) is a certainty grade. Whether to act on a robust need is a decision. Whether to collect more data on a statement that is supported but not replicated is a value-of-information question. Whether a need found in one survey's population holds in another district is a transfer question.

### 2.2 Methods, Assumptions and Guarantees

| Method | Assumes | Gives |
|---|---|---|
| GRADE certainty | a pre-specified question; a body of studies | a grade (high, moderate, low, very low) by declared factors: risk of bias, inconsistency, indirectness, imprecision, publication bias; upgrades for large effects and dose–response |
| Random-effects meta-analysis (DerSimonian–Laird; $I^2$) | exchangeable study effects | a pooled effect, the between-study variance, and how much of the variation is heterogeneity |
| Bayes factors | proper priors under each model | the evidence ratio of two hypotheses; calibrated by its scale, sensitive to priors |
| Expected value of perfect and sample information (EVPI, EVSI, EVPPI) | a decision model with uncertain parameters and a value per outcome | the expected gain from resolving uncertainty, overall, for a parameter subset, or for a study of a given size |
| Regression-based EVPPI (Strong et al.) | a probabilistic sensitivity sample | EVPPI without nested Monte Carlo, by regressing net benefit on the parameters |
| MCDA (ISPOR task force; Keeney–Raiffa) | declared criteria, scales and weights; preferential independence for additive value | a ranking with its sensitivity to weights |
| Transportability (Pearl–Bareinboim) | a selection diagram marking where populations differ | whether, and by which formula, an effect identified in one population can be computed for another |
| Evidence-based policy (Cartwright–Hardie) | a causal principle and its support factors | an argument structure for "it will work here", with what must be checked locally |

### 2.3 Key Papers

- Guyatt, G. H., Oxman, A. D., Vist, G. E., Kunz, R., Falck-Ytter, Y., Alonso-Coello, P. and Schünemann, H. J. (2008). GRADE: an emerging consensus on rating quality of evidence and strength of recommendations. *BMJ* 336(7650), 924–926. https://doi.org/10.1136/bmj.39489.470347.AD [V]
- DerSimonian, R. and Laird, N. (1986). Meta-analysis in clinical trials. *Controlled Clinical Trials* 7(3), 177–188. https://doi.org/10.1016/0197-2456(86)90046-2 [V]
- Higgins, J. P. T. and Thompson, S. G. (2002). Quantifying heterogeneity in a meta-analysis. *Statistics in Medicine* 21(11), 1539–1558. https://doi.org/10.1002/sim.1186 [V]
- Kass, R. E. and Raftery, A. E. (1995). Bayes factors. *Journal of the American Statistical Association* 90(430), 773–795. https://doi.org/10.1080/01621459.1995.10476572 [V]
- Claxton, K. (1999). The irrelevance of inference: a decision-making approach to the stochastic evaluation of health care technologies. *Journal of Health Economics* 18(3), 341–364. https://doi.org/10.1016/S0167-6296(98)00039-3 [V]
- Ades, A. E., Lu, G. and Claxton, K. (2004). Expected value of sample information calculations in medical decision modeling. *Medical Decision Making* 24(2), 207–227. https://doi.org/10.1177/0272989X04263162 [V]
- Strong, M., Oakley, J. E. and Brennan, A. (2014). Estimating multiparameter partial expected value of perfect information from a probabilistic sensitivity analysis sample: a nonparametric regression approach. *Medical Decision Making* 34(3), 311–326. https://doi.org/10.1177/0272989X13505910 [V]
- Heath, A., Manolopoulou, I. and Baio, G. (2017). A review of methods for analysis of the expected value of information. *Medical Decision Making* 37(7), 747–758. https://doi.org/10.1177/0272989X17697692 [V]
- Thokala, P., Devlin, N., Marsh, K., Baltussen, R., Boysen, M., Kalo, Z. et al. (2016). Multiple criteria decision analysis for health care decision making — an introduction: Report 1 of the ISPOR MCDA Emerging Good Practices Task Force. *Value in Health* 19(1), 1–13. https://doi.org/10.1016/j.jval.2015.12.003 [V]
- Keeney, R. L. and Raiffa, H. (1993). *Decisions with Multiple Objectives: Preferences and Value Trade-offs*. Cambridge University Press (first published by Wiley, 1976). https://doi.org/10.1017/CBO9781139174084 [V]
- Cartwright, N. and Hardie, J. (2012). *Evidence-Based Policy: A Practical Guide to Doing It Better*. Oxford University Press. https://doi.org/10.1093/acprof:osobl/9780199841608.001.0001 [V]
- Pearl, J. and Bareinboim, E. (2014). External validity: from do-calculus to transportability across populations. *Statistical Science* 29(4), 579–595. https://doi.org/10.1214/14-STS486 [V]

### 2.4 What Is Implementable and How It Serves the Feature

Most of this is held already (tyche `decision`, peira `synthesis`, tekmerion `warrant`, nomos `appraisal`, the policy domain's `adoption` and `transfer`). What the needs feature needs is their composition, not new methods:

- **A grading scheme for needs** in tekmerion's `warrant`, declared as data like the policy studio's schemes, with GRADE's factors read for statements of need: *risk of bias* (the design of the data: a census, a probability survey, an administrative register, a convenience sample); *inconsistency* (the $I^2$ of the gap across strata, regions or waves, from peira `synthesis`); *indirectness* (how far the indicator is from the domain: a waiting list measures expressed need, not need); *imprecision* (the confirmation interval against a declared minimally important gap); *reporting bias* (for expressed need, the propensity to report, §9.3). The statement's standing (theory note, Definition 3) becomes one input to the grade, not the grade itself.
- **Pooling across strata.** When a statement is confirmed in several strata or waves, a random-effects pool with $I^2$ says whether the need is general or local, which is information the archive should keep.
- **Value of information for the archive.** A statement that is supported on discovery but not yet confirmed can be ranked by the expected value of confirming it (EVSI of a survey module or audit of size $n$ for the decision to provide in $G$), with tyche `decision` and the policy domain's `adoption`. This turns "which supported statements deserve new data" from a judgement into a computation.
- **MCDA for action.** Ranking robust needs by size, severity, equity and tractability is nomos `appraisal`, with declared weights and weight stability.
- **Transfer.** A need found in a survey's population is claimed for a district only through peira's transport or tekmerion's Cartwright–Hardie argument, with the support factors stated.

### 2.5 Open Problems

- GRADE and its kin assume pre-specified questions; there is no accepted grading of hypotheses that a machine *found*, where the search itself is a source of bias. §3 supplies the statistical half of an answer; the rest is open.
- Value of information is developed for decisions, not for discovery: the worth of learning which groups have needs, before any decision is framed, has no standard form.
- Evidence of different kinds (a statistical gap, a causal estimate, a simulation, a rule) has no agreed common scale. Bayesian networks and belief functions (tekmerion) and argumentation (apodeixis) are candidate frames; none is standard.

---

## 3. False Discovery Under Adaptive Search

This area was not among the seven named, but every one of them leads to it, and it decides whether the archive can be trusted.

### 3.1 Concepts

Three facts organise the literature.

1. **The prior odds of a machine-found statement are low.** Ioannidis (2005) showed that the probability that a significant finding is true depends on the prior odds and the power, and falls when many hypotheses are tried and flexibility is high. Simmons, Nelson and Simonsohn (2011) and Gelman and Loken (2014) showed how flexibility in analysis (which subgroup, which outcome, which threshold) inflates false positives even without intent. An evolutionary search is that flexibility automated and accelerated.
2. **Splitting the data restores validity at a price.** Cox (1975) analysed testing on a part of the data not used to choose the hypothesis. Selective inference conditions on the selection event instead (Fithian, Sun and Taylor), which is tractable for simple selection rules (the lasso, stepwise), not for an evolutionary search; so splitting is the practical route.
3. **Reusing the held-out part adaptively destroys its validity.** If the outcome of tests on the confirmation part feeds back into what is tried next, the confirmation part becomes training data. Dwork et al. (2015) gave a mechanism, Thresholdout, that lets a holdout be queried many times with guarantees derived from differential privacy; Blum and Hardt (2015) gave the Ladder for leaderboards. Both bound how much a sequence of adaptive queries can overfit the holdout.

Multiple testing then has two settings. In the **offline** setting a fixed family of $m$ hypotheses is tested at once: Benjamini–Hochberg controls the FDR under independence or positive dependence, Benjamini–Yekutieli under any dependence. In the **online** setting hypotheses arrive one by one and each decision must be made before the next arrives, never revised: α-investing (Foster and Stine), LORD (Javanmard and Montanari), SAFFRON (Ramdas et al.) and asynchronous variants (Zrnic et al.) control the FDR in that setting. An archive that admits statements as the search finds them is the online setting.

**E-values** (Vovk and Wang 2021) are non-negative statistics with expectation at most 1 under the null. They combine by averaging under any dependence and by multiplication under sequential independence; e-BH (Wang and Ramdas 2022) controls the FDR under *arbitrary* dependence with no correction; e-LOND (Xu and Ramdas 2024) does so online. **E-processes** extend them in time: by Ville's inequality, an e-process may be monitored continuously and stopped at any time without inflating the type-I error (safe anytime-valid inference; Ramdas, Grünwald, Vovk and Shafer 2023; Shafer's testing by betting; Grünwald, de Heide and Koolen's safe testing). This is what retirement on new data needs: a statement re-examined on every new wave of data can be revoked by a test whose error does not grow with the number of looks.

**Stability selection** (Meinshausen and Bühlmann 2010) bounds the expected number of falsely selected variables for selection under resampling, given an exchangeability condition and a selection procedure better than random guessing. **Partial pooling** (Gelman, Hill and Yajima 2012) answers multiplicity differently: a multilevel model shrinks the estimates of many groups towards each other, which controls errors of sign and magnitude rather than rejections.

### 3.2 Methods, Assumptions and Guarantees

| Method | Assumes | Guarantees |
|---|---|---|
| Data splitting (Cox; Rinaldo et al.) | the split independent of the data's outcome | valid tests and intervals for the hypotheses chosen on the other part; assumption-lean inference after selection (Rinaldo, Wasserman and G'Sell) |
| BH / BY / Storey | a fixed family; independence or PRDS (BH, Storey), any dependence (BY) | FDR ≤ q (π₀q for BH) |
| α-investing, LORD, SAFFRON | hypotheses in a fixed order, decisions final; independence of null p-values (LORD, SAFFRON) | (m)FDR control at every time |
| Asynchronous online testing | tests that start and finish at different times, with declared conflict sets | FDR control under local dependence |
| e-BH, e-LOND | valid e-values; any dependence | FDR control, offline (e-BH) or online (e-LOND); e-LOND is conservative |
| E-processes, confidence sequences | a test martingale or e-process under the null | type-I error ≤ α at any data-dependent stopping time |
| Thresholdout | a holdout queried by statistical queries, with a budget | answers that remain accurate for a number of adaptive queries far larger than plain reuse allows |
| Ladder | a leaderboard of submissions | leaderboard accuracy that degrades slowly with the number of adaptive submissions |
| Stability selection | exchangeable noise variables; selection better than random | bound on the expected number of false selections |
| Partial pooling | exchangeable groups in a multilevel model | shrinkage that reduces errors of sign and magnitude |

### 3.3 Key Papers

- Ioannidis, J. P. A. (2005). Why most published research findings are false. *PLoS Medicine* 2(8), e124. https://doi.org/10.1371/journal.pmed.0020124 [V]
- Simmons, J. P., Nelson, L. D. and Simonsohn, U. (2011). False-positive psychology: undisclosed flexibility in data collection and analysis allows presenting anything as significant. *Psychological Science* 22(11), 1359–1366. https://doi.org/10.1177/0956797611417632 [V]
- Gelman, A. and Loken, E. (2014). The statistical crisis in science. *American Scientist* 102(6), 460. https://doi.org/10.1511/2014.111.460 [V]
- Cox, D. R. (1975). A note on data-splitting for the evaluation of significance levels. *Biometrika* 62(2), 441–444. https://doi.org/10.1093/biomet/62.2.441 [V]
- Rinaldo, A., Wasserman, L. and G'Sell, M. (2019). Bootstrapping and sample splitting for high-dimensional, assumption-lean inference. *Annals of Statistics* 47(6), 3438–3469. https://doi.org/10.1214/18-AOS1784 [V]
- Fithian, W., Sun, D. and Taylor, J. (2014). Optimal inference after model selection. arXiv:1410.2597. https://arxiv.org/abs/1410.2597 [V]
- Benjamini, Y. and Hochberg, Y. (1995). Controlling the false discovery rate: a practical and powerful approach to multiple testing. *JRSS B* 57(1), 289–300. https://doi.org/10.1111/j.2517-6161.1995.tb02031.x [V]
- Foster, D. P. and Stine, R. A. (2008). α-investing: a procedure for sequential control of expected false discoveries. *JRSS B* 70(2), 429–444. https://doi.org/10.1111/j.1467-9868.2007.00643.x [V]
- Javanmard, A. and Montanari, A. (2018). Online rules for control of false discovery rate and false discovery exceedance. *Annals of Statistics* 46(2), 526–554. https://doi.org/10.1214/17-AOS1559 [V]
- Ramdas, A., Zrnic, T., Wainwright, M. and Jordan, M. (2018). SAFFRON: an adaptive algorithm for online control of the false discovery rate. *Proceedings of ICML 2018*, PMLR 80, 4286–4294. https://proceedings.mlr.press/v80/ramdas18a.html [V]
- Zrnic, T., Ramdas, A. and Jordan, M. I. (2021). Asynchronous online testing of multiple hypotheses. *Journal of Machine Learning Research* 22(33), 1–39. https://jmlr.org/papers/v22/19-910.html [V]
- Vovk, V. and Wang, R. (2021). E-values: calibration, combination and applications. *Annals of Statistics* 49(3), 1736–1754. https://doi.org/10.1214/20-AOS2020 [V]
- Wang, R. and Ramdas, A. (2022). False discovery rate control with e-values. *JRSS B* 84(3), 822–852. https://doi.org/10.1111/rssb.12489 [V]
- Xu, Z. and Ramdas, A. (2024). Online multiple testing with e-values. *Proceedings of AISTATS 2024*, PMLR 238, 3997–4005. https://proceedings.mlr.press/v238/xu24a.html [V]
- Ramdas, A., Grünwald, P., Vovk, V. and Shafer, G. (2023). Game-theoretic statistics and safe anytime-valid inference. *Statistical Science* 38(4), 576–601. https://doi.org/10.1214/23-STS894 [V]
- Shafer, G. (2021). Testing by betting: a strategy for statistical and scientific communication. *JRSS A* 184(2), 407–431. https://doi.org/10.1111/rssa.12647 [V]
- Grünwald, P., de Heide, R. and Koolen, W. (2024). Safe testing. *JRSS B* 86(5), 1091–1128. https://doi.org/10.1093/jrsssb/qkae011 [V]
- Dwork, C., Feldman, V., Hardt, M., Pitassi, T., Reingold, O. and Roth, A. (2015). The reusable holdout: preserving validity in adaptive data analysis. *Science* 349(6248), 636–638. https://doi.org/10.1126/science.aaa9375 [V]
- Blum, A. and Hardt, M. (2015). The Ladder: a reliable leaderboard for machine learning competitions. *Proceedings of ICML 2015*, PMLR 37, 1006–1014. https://proceedings.mlr.press/v37/blum15.html [V]
- Meinshausen, N. and Bühlmann, P. (2010). Stability selection. *JRSS B* 72(4), 417–473. https://doi.org/10.1111/j.1467-9868.2010.00740.x [V]
- Gelman, A., Hill, J. and Yajima, M. (2012). Why we (usually) don't have to worry about multiple comparisons. *Journal of Research on Educational Effectiveness* 5(2), 189–211. https://doi.org/10.1080/19345747.2011.618213 [V]

### 3.4 What Is Implementable and How It Serves the Feature

All of the following are small, exact, and general (they name nothing in nature), so they belong in tyche's `testing`:

- **Online FDR** (`online.hpp`): LORD (with its LORD++ form), SAFFRON, α-investing, and the asynchronous variant whose conflict sets match heuresis's asynchronous workers. Each takes p-values in admission order and returns final decisions with the wealth or threshold trajectory. *Use:* admission to the archive. Each statement that passes discovery is given one confirmation p-value; its admission is decided online, in order, and never revised by later admissions.
- **E-values and e-processes** (`evalues.hpp`): calibrators from p to e; e-BH; e-LOND; betting e-processes and confidence sequences for a bounded mean and for a difference of means (the gap). *Use:* (a) confirmation statistics for overlapping groups on the same confirmation data, which are dependent in unknown ways, where e-BH or e-LOND need no dependence assumption; (b) evidential retirement: each archived statement keeps an e-process on new waves of data for the hypothesis that its gap has closed, and is revoked when it crosses $1/\alpha$, however often it is looked at.
- **The reusable holdout** (`holdout.hpp`): Thresholdout with an explicit query budget and the Ladder. *Use:* any validation part that the search is allowed to consult (for example, to promote a statement to the archive's pool of parents) is queried only through this mechanism, and the budget is recorded.
- **Stability selection** with its bound, read for conditions of groups.
- **Pre-registration and sealing.** peira's `protocol` already records a plan against a report with deviations; archeion's journal already seals entries by hash chain. Each confirmation test should be written as a protocol entry and sealed *before* the confirmation data are read for that statement. This gives an auditable proof that the confirmation was not chosen after seeing it.

### 3.5 Open Problems

- Statements are logically related (one group nested in another, one standard a tightening of another). Structured FDR procedures for hierarchies exist (Yekutieli's tree FDR [M]), but none fits the lattice of conjunctions directly.
- Online FDR assumes decisions are final; an archive that revokes statements needs an account of what revocation does to the error rate of what remains.
- Splitting costs power, most for small groups, which are often the groups whose needs matter most. Power analysis per statement (the coverage below which no gap of minimally important size could be confirmed) should bound the language, not only the tests.
- Redundant near-duplicate statements inflate the count of discoveries without adding information; FDR counts statements, not distinct needs.

---

## 4. Causal Analysis and the Discovery of Subgroups

### 4.1 Concepts

Causal analysis distinguishes **identification** (whether a causal quantity is a function of the observed distribution, given assumptions stated as a graph or as potential outcomes), **estimation** (how to compute it well from finite data), and **discovery** (learning the graph itself, or the units for which an effect is large). For needs, three causal questions arise:

1. *Would provision close the gap in $G$?* The effect of $P$ on $I$ within $G$: identification by peira's back-door criterion on a declared graph, estimation by doubly robust or orthogonal methods (§4.4).
2. *For whom would provision do most?* Heterogeneous effects, and groups that describe them with valid inference (§4.4–4.5).
3. *Which groups have a gap at all?* This is descriptive, not causal, but the methods that find groups (subgroup discovery, exceptional model mining) and those that find groups with large effects (causal trees, causal rule sets, policy trees) are the same family with different targets, and share the same statistical hazard (§3).

### 4.2 Identification: Graphs and Potential Outcomes

Pearl's do-calculus and the back-door and front-door criteria (peira `graphs` holds them) and the potential-outcomes framework of Imbens and Rubin give equivalent identification results where both apply. Sensitivity analysis (peira `sensitivity`: E-values, Rosenbaum's bounds, Cinelli and Hazlett's robustness value) says how strong an unmeasured confounder would have to be to overturn a conclusion. *Use:* every causal-support test on a statement should carry its robustness value, so that "provision would close the gap" is stated with how fragile it is.

### 4.3 Causal Discovery and Its Reliability

| Algorithm | Assumes | Output and guarantee |
|---|---|---|
| PC (Spirtes, Glymour and Scheines) | causal sufficiency, faithfulness, correct conditional-independence tests | a CPDAG, consistent in the large-sample limit |
| FCI (Spirtes et al.; Zhang 2008) | faithfulness; latent confounders and selection allowed | a PAG; Zhang proved the orientation rules complete |
| GES (Chickering 2002) | the distribution has a perfect map that is a DAG; a consistent score | the equivalence class, optimal in the large-sample limit |
| LiNGAM (Shimizu et al. 2006) | linearity, non-Gaussian disturbances, no latent confounders | the full DAG, identified by independent component analysis |
| NOTEARS (Zheng et al. 2018) | a smooth score; acyclicity as a continuous constraint | a DAG by continuous optimisation; no identifiability guarantee of its own |

Reliability findings matter more than the algorithms here. Uhler, Raskutti, Bühlmann and Yu (2013) showed that the set of distributions close to violating faithfulness is not negligible, so constraint-based methods can fail with realistic sample sizes. Reisach, Seiler and Weichwald (2021) showed that common simulated benchmarks can be solved by sorting variables by their variance, so that reported performance of continuous-optimisation methods partly reflects the benchmarks' scale. Language models are no remedy: Kıcıman et al. (arXiv 2023) report strong performance on pairwise causal questions alongside unpredictable failures, and Jin et al. (arXiv 2023; abstract read) report "almost close to random performance" at inferring causation from correlational statements, with fine-tuned models failing out of distribution.

*Consequence for needs:* causal discovery should at most *propose* graphs, graded as proposals; the causal-support test should run on a declared graph, as the theory note already says.

### 4.4 Heterogeneous Effects and Their Inference

- **Honest causal trees** (Athey and Imbens 2016) split the sample: one part chooses the partition, the other estimates the effect in each leaf, which gives valid intervals for the leaves. This is §3's splitting principle inside a tree.
- **Causal forests** (Wager and Athey 2018) and **generalised random forests** (Athey, Tibshirani and Wager 2019) are honest ensembles with asymptotic normality.
- **Meta-learners:** S-, T- and X-learners (Künzel et al. 2019); the R-learner by Robinson's decomposition (Nie and Wager 2021); the DR-learner (Kennedy 2023), with error bounds close to an oracle's under stated conditions.
- **Double/debiased machine learning** (Chernozhukov et al. 2018): Neyman-orthogonal scores and cross-fitting give root-$n$ inference on low-dimensional parameters with machine-learned nuisance functions.
- **Generic machine learning inference** (Chernozhukov, Demirer, Duflo and Fernández-Val 2025): inference on features of any machine-learned proxy of the conditional effect, valid whatever the proxy's quality: the best linear predictor (BLP), sorted group average effects (GATES), and classification analysis (CLAN), which compares the covariates of the most and least affected groups, with inference aggregated over repeated splits.
- **Policy learning:** Athey and Wager (2021) learn treatment rules from doubly robust scores with regret bounds; `policytree` (Sverdrup et al. 2020) searches depth-$k$ trees exhaustively; optimal policy trees (Amram, Dunn and Zhuo 2022) by optimisation.

*Use for needs:* GATES and CLAN answer, with valid inference, the question "which kinds of units would provision help most", whose answer is a set of candidate groups; a policy tree of depth 2 is a strong baseline against which the evolutionary search's best groups can be compared.

### 4.5 The Discovery of Subgroups

**Subgroup discovery** (Klösgen 1996; Wrobel 1997) finds descriptions (conjunctions of conditions) whose units differ unusually on a target. Its parts are a description language, a quality measure, and a search. The needs domain's quality, "gap times a power of coverage", is Klösgen's family $q_a = n^a(\mu_G - \mu_0)$: $a = 1$ behaves like weighted relative accuracy, $a = 0.5$ like a z-score. Search is exhaustive with **optimistic estimates** that bound the quality of every refinement (Lemmerich, Atzmueller and Puppe 2016), by beam, or evolutionary (del Jesus et al. 2007; the multi-objective NMEEF-SD of Carmona et al. 2010). CN2-SD (Lavrač et al. 2004) adapted rule learning with weighted covering, so that later rules favour units not yet covered. Reviews: Herrera et al. (2011), Atzmueller (2015).

**Exceptional model mining** (Leman, Feelders and Knobbe 2008; Duivesteijn, Feelders and Knobbe 2016) generalises the target to a *model* fitted within the group (a correlation, a regression slope, a Bayesian network), and the quality to the distance between the group's model and the population's. For needs, the relation of demand to provision, or of an indicator to income, can be the model, so that a group is exceptional when its *relation* differs, not only its mean.

**Redundancy and diversity.** Beam searches return many near-identical groups. DSSD (van Leeuwen and Knobbe 2012) selects diverse subgroup sets by cover-based redundancy; robust subgroup lists (Proença, Grünwald, Bäck and van Leeuwen 2022) choose an ordered list by the minimum description length principle, which penalises both redundancy and statistically weak groups.

**Statistical soundness.** Webb (2007) compared two routes: direct adjustment of the significance level by the size of the search space, and evaluation on a holdout; Webb (2008) refined direct adjustment with layered critical values. Duivesteijn and Knobbe (2011) built the distribution of false discoveries by swap randomisation (permuting the target and rerunning the search) to give significance thresholds for a quality measure. Hämäläinen and Webb (2019) is a tutorial on statistically sound pattern discovery. The `pysubgroup` library (Lemmerich and Becker 2019) is a reference implementation in Python.

**Causal subgroups.** SIDES (Lipkovich et al. 2011) partitions recursively by differential effect with resampling-based multiplicity control; Virtual Twins (Foster, Taylor and Ruberg 2011) predicts both potential outcomes and fits a tree to their difference; Lipkovich, Dmitrienko and D'Agostino (2017) classify the methods. For observational data, causal interaction trees (Yang, Dahabreh and Steingrimsson 2021) split with inverse-probability, g-formula and doubly robust criteria; causal rule sets (Wang and Rudin 2022) describe enhanced-effect groups as rule sets with a Bayesian model; CURLS (Zhou et al. 2024) learns rules for subgroups with significant effects. Imai and Ratkovic (2013) estimate heterogeneity in randomised evaluations with a sparse support-vector method. A search found no paper joining exceptional model mining with causal effect estimation as such; the closest is EMM of mediation models (Lemmerich et al. [S]).

### 4.6 Key Papers

- Pearl, J. (2009). *Causality: Models, Reasoning, and Inference*, 2nd ed. Cambridge University Press. https://doi.org/10.1017/CBO9780511803161 [V]
- Imbens, G. W. and Rubin, D. B. (2015). *Causal Inference for Statistics, Social, and Biomedical Sciences*. Cambridge University Press. https://doi.org/10.1017/CBO9781139025751 [V]
- Spirtes, P., Glymour, C. and Scheines, R. (2001). *Causation, Prediction, and Search*, 2nd ed. MIT Press. https://doi.org/10.7551/mitpress/1754.001.0001 [V]
- Chickering, D. M. (2002). Optimal structure identification with greedy search. *Journal of Machine Learning Research* 3, 507–554. https://jmlr.org/papers/v3/chickering02b.html [V]
- Shimizu, S., Hoyer, P. O., Hyvärinen, A. and Kerminen, A. (2006). A linear non-Gaussian acyclic model for causal discovery. *Journal of Machine Learning Research* 7, 2003–2030. https://jmlr.org/papers/v7/shimizu06a.html [V]
- Zhang, J. (2008). On the completeness of orientation rules for causal discovery in the presence of latent confounders and selection bias. *Artificial Intelligence* 172(16–17), 1873–1896. https://doi.org/10.1016/j.artint.2008.08.001 [V]
- Zheng, X., Aragam, B., Ravikumar, P. and Xing, E. P. (2018). DAGs with NO TEARS: continuous optimization for structure learning. arXiv:1803.01422. https://arxiv.org/abs/1803.01422 [V]
- Glymour, C., Zhang, K. and Spirtes, P. (2019). Review of causal discovery methods based on graphical models. *Frontiers in Genetics* 10, 524. https://doi.org/10.3389/fgene.2019.00524 [V]
- Uhler, C., Raskutti, G., Bühlmann, P. and Yu, B. (2013). Geometry of the faithfulness assumption in linear models. *Annals of Statistics* 41(2), 436–463. https://doi.org/10.1214/12-AOS1080 [V]
- Reisach, A. G., Seiler, C. and Weichwald, S. (2021). Beware of the simulated DAG! Causal discovery benchmarks may be easy to game. arXiv:2102.13647. https://arxiv.org/abs/2102.13647 [V]
- Kıcıman, E., Ness, R., Sharma, A. and Tan, C. (2023). Causal reasoning and large language models: opening a new frontier for causality. arXiv:2305.00050. https://arxiv.org/abs/2305.00050 [V] (journal status not confirmed)
- Jin, Z., Liu, J., Lyu, Z., Poff, S., Sachan, M., Mihalcea, R. et al. (2023). Can large language models infer causation from correlation? arXiv:2306.05836. https://arxiv.org/abs/2306.05836 [V]
- Textor, J., van der Zander, B., Gilthorpe, M. S., Liśkiewicz, M. and Ellison, G. T. H. (2016). Robust causal inference using directed acyclic graphs: the R package 'dagitty'. *International Journal of Epidemiology* 45(6), 1887–1894. https://doi.org/10.1093/ije/dyw341 [V for DOI and authors (Crossref gives 2017, the date online); year, volume and pages from prior knowledge, M]
- Cinelli, C. and Hazlett, C. (2020). Making sense of sensitivity: extending omitted variable bias. *JRSS B* 82(1), 39–67. https://doi.org/10.1111/rssb.12348 [V]
- Athey, S. and Imbens, G. (2016). Recursive partitioning for heterogeneous causal effects. *PNAS* 113(27), 7353–7360. https://doi.org/10.1073/pnas.1510489113 [V]
- Wager, S. and Athey, S. (2018). Estimation and inference of heterogeneous treatment effects using random forests. *JASA* 113(523), 1228–1242. https://doi.org/10.1080/01621459.2017.1319839 [V]
- Athey, S., Tibshirani, J. and Wager, S. (2019). Generalized random forests. *Annals of Statistics* 47(2), 1148–1178. https://doi.org/10.1214/18-AOS1709 [V]
- Künzel, S. R., Sekhon, J. S., Bickel, P. J. and Yu, B. (2019). Metalearners for estimating heterogeneous treatment effects using machine learning. *PNAS* 116(10), 4156–4165. https://doi.org/10.1073/pnas.1804597116 [V]
- Nie, X. and Wager, S. (2021). Quasi-oracle estimation of heterogeneous treatment effects. *Biometrika* 108(2), 299–319. https://doi.org/10.1093/biomet/asaa076 [V]
- Kennedy, E. H. (2023). Towards optimal doubly robust estimation of heterogeneous causal effects. *Electronic Journal of Statistics* 17(2), 3008–3049. https://doi.org/10.1214/23-EJS2157 [V]
- Chernozhukov, V., Chetverikov, D., Demirer, M., Duflo, E., Hansen, C., Newey, W. and Robins, J. (2018). Double/debiased machine learning for treatment and structural parameters. *Econometrics Journal* 21(1), C1–C68. https://doi.org/10.1111/ectj.12097 [V]
- Chernozhukov, V., Demirer, M., Duflo, E. and Fernández-Val, I. (2025). Fisher–Schultz Lecture: generic machine learning inference on heterogeneous treatment effects in randomized experiments, with an application to immunization in India. *Econometrica* 93(4), 1121–1164. Preprint arXiv:1712.04802. https://arxiv.org/abs/1712.04802 [V for the preprint; journal volume and pages S, from EconPapers and MIT's repository]
- Athey, S. and Wager, S. (2021). Policy learning with observational data. *Econometrica* 89(1), 133–161. https://doi.org/10.3982/ECTA15732 [V]
- Sverdrup, E., Kanodia, A., Zhou, Z., Athey, S. and Wager, S. (2020). policytree: policy learning via doubly robust empirical welfare maximization over trees. *Journal of Open Source Software* 5(50), 2232. https://doi.org/10.21105/joss.02232 [V]
- Amram, M., Dunn, J. and Zhuo, Y. D. (2022). Optimal policy trees. *Machine Learning* 111(7), 2741–2768. https://doi.org/10.1007/s10994-022-06128-5 [V]
- Imai, K. and Ratkovic, M. (2013). Estimating treatment effect heterogeneity in randomized program evaluation. *Annals of Applied Statistics* 7(1), 443–470. https://doi.org/10.1214/12-AOAS593 [V]
- Klösgen, W. (1996). Explora: a multipattern and multistrategy discovery assistant. In Fayyad, U. M. et al. (eds), *Advances in Knowledge Discovery and Data Mining*. AAAI/MIT Press. [S; the page range 249–271 given in the theory note was not confirmed]
- Wrobel, S. (1997). An algorithm for multi-relational discovery of subgroups. *PKDD 1997*, LNCS 1263, 78–87. https://doi.org/10.1007/3-540-63223-9_108 [V; LNCS volume number M]
- Lavrač, N., Kavšek, B., Flach, P. and Todorovski, L. (2004). Subgroup discovery with CN2-SD. *Journal of Machine Learning Research* 5, 153–188. https://jmlr.org/papers/v5/lavrac04a.html [V]
- Herrera, F., Carmona, C. J., González, P. and del Jesus, M. J. (2011). An overview on subgroup discovery: foundations and applications. *Knowledge and Information Systems* 29(3), 495–525. https://doi.org/10.1007/s10115-010-0356-2 [V]
- Atzmueller, M. (2015). Subgroup discovery. *WIREs Data Mining and Knowledge Discovery* 5(1), 35–49. https://doi.org/10.1002/widm.1144 [V]
- Lemmerich, F., Atzmueller, M. and Puppe, F. (2016). Fast exhaustive subgroup discovery with numerical target concepts. *Data Mining and Knowledge Discovery* 30(3), 711–762. https://doi.org/10.1007/s10618-015-0436-8 [V; issue M]
- Lemmerich, F. and Becker, M. (2019). pysubgroup: easy-to-use subgroup discovery in Python. *ECML PKDD 2018*, LNCS, 658–662. https://doi.org/10.1007/978-3-030-10997-4_46 [V]
- del Jesus, M. J., González, P., Herrera, F. and Mesonero, M. (2007). Evolutionary fuzzy rule induction process for subgroup discovery: a case study in marketing. *IEEE Transactions on Fuzzy Systems* 15(4), 578–592. https://doi.org/10.1109/TFUZZ.2006.890662 [V]
- Carmona, C. J., González, P., del Jesus, M. J. and Herrera, F. (2010). NMEEF-SD: non-dominated multiobjective evolutionary algorithm for extracting fuzzy rules in subgroup discovery. *IEEE Transactions on Fuzzy Systems* 18(5), 958–970. https://doi.org/10.1109/TFUZZ.2010.2060200 [V; issue M]
- Leman, D., Feelders, A. and Knobbe, A. (2008). Exceptional model mining. *ECML PKDD 2008*, LNCS, 1–16. https://doi.org/10.1007/978-3-540-87481-2_1 [V]
- Duivesteijn, W., Feelders, A. J. and Knobbe, A. (2016). Exceptional model mining: supervised descriptive local pattern mining with complex target concepts. *Data Mining and Knowledge Discovery* 30(1), 47–98. https://doi.org/10.1007/s10618-015-0403-4 [V]
- van Leeuwen, M. and Knobbe, A. (2012). Diverse subgroup set discovery. *Data Mining and Knowledge Discovery* 25(2), 208–242. https://doi.org/10.1007/s10618-012-0273-y [V]
- Proença, H. M., Grünwald, P., Bäck, T. and van Leeuwen, M. (2022). Robust subgroup discovery. *Data Mining and Knowledge Discovery* 36, 1885–1970. https://doi.org/10.1007/s10618-022-00856-x [V]
- Webb, G. I. (2007). Discovering significant patterns. *Machine Learning* 68(1), 1–33. https://doi.org/10.1007/s10994-007-5006-x [V]
- Webb, G. I. (2008). Layered critical values: a powerful direct-adjustment approach to discovering significant patterns. *Machine Learning* 71(2–3), 307–323. https://doi.org/10.1007/s10994-008-5046-x [V]
- Duivesteijn, W. and Knobbe, A. (2011). Exploiting false discoveries — statistical validation of patterns and quality measures in subgroup discovery. *IEEE ICDM 2011*, 151–160. https://doi.org/10.1109/ICDM.2011.65 [V]
- Hämäläinen, W. and Webb, G. I. (2019). A tutorial on statistically sound pattern discovery. *Data Mining and Knowledge Discovery* 33(2), 325–377. https://doi.org/10.1007/s10618-018-0590-x [V]
- Lipkovich, I., Dmitrienko, A., Denne, J. and Enas, G. (2011). Subgroup identification based on differential effect search — a recursive partitioning method for establishing response to treatment in patient subpopulations. *Statistics in Medicine* 30(21), 2601–2621. https://doi.org/10.1002/sim.4289 [V]
- Foster, J. C., Taylor, J. M. G. and Ruberg, S. J. (2011). Subgroup identification from randomized clinical trial data. *Statistics in Medicine* 30(24), 2867–2880. https://doi.org/10.1002/sim.4322 [V]
- Lipkovich, I., Dmitrienko, A. and D'Agostino, R. B., Sr. (2017). Tutorial in biostatistics: data-driven subgroup identification and analysis in clinical trials. *Statistics in Medicine* 36(1), 136–196. https://doi.org/10.1002/sim.7064 [V; third author's name as Crossref truncates it, completed from prior knowledge, M]
- Yang, J., Dahabreh, I. J. and Steingrimsson, J. A. (2021). Causal interaction trees: finding subgroups with heterogeneous treatment effects in observational data. *Biometrics* 78, 624–635. https://doi.org/10.1111/biom.13432 [V]
- Wang, T. and Rudin, C. (2022). Causal rule sets for identifying subgroups with enhanced treatment effects. *INFORMS Journal on Computing* 34(3), 1626–1643. https://doi.org/10.1287/ijoc.2021.1143 [V]
- Zhou, J., Yang, L., Liu, X., Gu, X., Sun, L. and Chen, W. (2024). CURLS: causal rule learning for subgroups with significant treatment effect. *KDD 2024*, 4619–4630. https://doi.org/10.1145/3637528.3671951 [V]

### 4.7 What Is Implementable and How It Serves the Feature

- **Subgroup discovery as a general foundation module.** Description languages over a table, Klösgen's family and WRAcc, tight optimistic estimates for mean-shift qualities, beam and exhaustive search with pruning, DSSD's redundancy control, swap-randomisation thresholds (Duivesteijn–Knobbe), Webb's layered critical values, and EMM with a declared model class. It names nothing in nature, so it belongs in a foundation; the needs domain's `groups` already has the description language (conditions, canonical keys, subsumption), which would move down or be mirrored. *Use:* the strongest classical generator; its optimistic estimates also bound which refinements the evolutionary search need ever try; its swap-randomisation threshold gives the discovery-side significance that the search may use without touching confirmation data.
- **Heterogeneous effects in peira.** Honest causal trees and forests, the DR- and R-learners with cross-fitting, GenericML's BLP, GATES and CLAN, and exhaustive policy trees of depth 1–2. peira already holds IPW and the doubly robust estimator, the back-door criterion and sensitivity analysis, so these extend existing modules. *Use:* the causal-support test (theory note §3.4) estimated within $G$ with a declared adjustment set and reported with its robustness value; GATES and CLAN as a generator of candidate groups "where provision would do most"; policy trees as the baseline the evolutionary search must beat to be worth its cost.
- **Causal discovery** only as a proposer of graphs (PC and FCI in peira `graphs`, graded as proposals), never as the graph a test assumes.

### 4.8 Open Problems

- Valid inference after searching a lattice of overlapping conjunctions remains open beyond splitting and permutation thresholds; the groups that a search finds are correlated, nested and selected.
- Confounding may differ by subgroup; a group found by maximising an observational effect is the group where confounding most inflates it. Positivity often fails in small groups, so doubly robust estimates become unstable exactly where the search looks.
- Survey weights and design-based variance (peira `surveys`) are rarely built into subgroup discovery's quality measures.
- A group is a description, and descriptions of vulnerable people can stigmatise; the literature on fair subgroup discovery is thin.

---

## 5. Complex Systems for the Social Sciences

### 5.1 Concepts

Epstein (1999) stated the programme of **generative social science**: to explain a macroscopic regularity is to grow it from a population of agents with stated rules ("if you didn't grow it, you didn't explain it"). Generative sufficiency is weaker than truth: many mechanisms may grow the same pattern (equifinality). **Emergence** has three precise readings in the literature: *weak emergence* (Bedau 1997), a macro-state derivable only by simulation; *causal emergence* (Hoel, Albantakis and Tononi 2013), a macro-description with more effective information than its micro-description; and information-theoretic criteria computable from data (Rosas et al. 2020). Crutchfield's computational mechanics (1994) treats emergence as the appearance of new structure detectable by an observer's model class. Schelling's segregation (1971) is the canonical example, and nomos `agents` already holds it.

**Calibration and validation** of agent-based models has four strands:

1. **Pattern-oriented modelling** (Grimm et al. 2005): several patterns at different scales and levels together filter model structures and parameter values, which one pattern cannot.
2. **Approximate Bayesian computation** (Beaumont, Zhang and Balding 2002; Toni et al. 2009; Marin et al. 2012): a posterior over parameters from simulations whose summaries fall near the data's, with sequential Monte Carlo and model selection.
3. **Emulation and history matching** (Kennedy and O'Hagan 2001; Vernon, Goldstein and Bower 2010; Andrianakis et al. 2015): a Gaussian-process emulator of the simulator, an explicit model discrepancy, and the implausibility $I(x) = |z - \mathrm{E}[f(x)]| / \sqrt{\mathrm{Var}_{em}(x) + \mathrm{Var}_{disc} + \mathrm{Var}_{obs}}$, by which regions of parameter space are ruled out in waves. History matching asks only which parameters are *not implausible*, which is robust where a full posterior would overstate certainty.
4. **Simulation-based inference** with neural estimators (Cranmer, Brehmer and Louppe 2020): posteriors, likelihoods or likelihood ratios learnt from simulations. Benchmarks find no uniformly best method and that the metric chosen decides the ranking (Lueckmann et al. 2021, abstract read); learnt posteriors can be overconfident (Hermans et al. 2021), and simulation-based calibration checks an inference procedure by the uniformity of rank statistics (Talts et al. 2018).

For economic ABMs specifically: Fagiolo, Moneta and Windrum (2007) classify validation approaches; Grazzini, Richiardi and Tsionas (2017) estimate ABMs by Bayesian methods; Lamperti, Roventini and Sani (2018) use machine-learnt surrogates to explore parameter spaces cheaply; Platt (2020) compares calibration methods; Dyer et al. (2024) give black-box Bayesian inference for ABMs. Describing models by ODD (Grimm et al. 2006, 2020), aligning two implementations of one model ("docking"; Axtell et al. 1996) and choosing a sensitivity method (ten Broeke, van Voorn and Ligtenberg 2016) are the field's practices of reproducibility.

### 5.2 Key Papers

- Epstein, J. M. (1999). Agent-based computational models and generative social science. *Complexity* 4(5), 41–60. https://doi.org/10.1002/(SICI)1099-0526(199905/06)4:5<41::AID-CPLX9>3.0.CO;2-F [V]
- Schelling, T. C. (1971). Dynamic models of segregation. *Journal of Mathematical Sociology* 1(2), 143–186. https://doi.org/10.1080/0022250X.1971.9989794 [V]
- Bedau, M. A. (1997). Weak emergence. *Noûs* 31(s11) (Philosophical Perspectives 11), 375–399. https://doi.org/10.1111/0029-4624.31.s11.17 [V]
- Crutchfield, J. P. (1994). The calculi of emergence: computation, dynamics and induction. *Physica D* 75(1–3), 11–54. https://doi.org/10.1016/0167-2789(94)90273-9 [V]
- Hoel, E. P., Albantakis, L. and Tononi, G. (2013). Quantifying causal emergence shows that macro can beat micro. *PNAS* 110(49), 19790–19795. https://doi.org/10.1073/pnas.1314922110 [V]
- Rosas, F. E., Mediano, P. A. M., Jensen, H. J., Seth, A. K., Barrett, A. B., Carhart-Harris, R. L. and Bor, D. (2020). Reconciling emergences: an information-theoretic approach to identify causal emergence in multivariate data. *PLOS Computational Biology* 16(12), e1008289. https://doi.org/10.1371/journal.pcbi.1008289 [V]
- Grimm, V., Revilla, E., Berger, U., Jeltsch, F., Mooij, W. M., Railsback, S. F. et al. (2005). Pattern-oriented modeling of agent-based complex systems: lessons from ecology. *Science* 310(5750), 987–991. https://doi.org/10.1126/science.1116681 [V]
- Grimm, V., Berger, U., Bastiansen, F., Eliassen, S., Ginot, V., Giske, J. et al. (2006). A standard protocol for describing individual-based and agent-based models. *Ecological Modelling* 198(1–2), 115–126. https://doi.org/10.1016/j.ecolmodel.2006.04.023 [V]
- Grimm, V., Railsback, S. F., Vincenot, C. E., Berger, U., Gallagher, C., DeAngelis, D. L. et al. (2020). The ODD protocol for describing agent-based and other simulation models: a second update to improve clarity, replication, and structural realism. *JASSS* 23(2), 7. https://doi.org/10.18564/jasss.4259 [V]
- Axtell, R., Axelrod, R., Epstein, J. M. and Cohen, M. D. (1996). Aligning simulation models: a case study and results. *Computational and Mathematical Organization Theory* 1(2), 123–141. https://doi.org/10.1007/BF01299065 [V]
- Fagiolo, G., Moneta, A. and Windrum, P. (2007). A critical guide to empirical validation of agent-based models in economics: methodologies, procedures, and open problems. *Computational Economics* 30(3), 195–226. https://doi.org/10.1007/s10614-007-9104-4 [V]
- Beaumont, M. A., Zhang, W. and Balding, D. J. (2002). Approximate Bayesian computation in population genetics. *Genetics* 162(4), 2025–2035. https://doi.org/10.1093/genetics/162.4.2025 [V]
- Toni, T., Welch, D., Strelkowa, N., Ipsen, A. and Stumpf, M. P. H. (2009). Approximate Bayesian computation scheme for parameter inference and model selection in dynamical systems. *Journal of the Royal Society Interface* 6(31), 187–202. https://doi.org/10.1098/rsif.2008.0172 [V]
- Marin, J.-M., Pudlo, P., Robert, C. P. and Ryder, R. J. (2012). Approximate Bayesian computational methods. *Statistics and Computing* 22, 1167–1180. https://doi.org/10.1007/s11222-011-9288-2 [V]
- Kennedy, M. C. and O'Hagan, A. (2001). Bayesian calibration of computer models. *JRSS B* 63(3), 425–464. https://doi.org/10.1111/1467-9868.00294 [V]
- Vernon, I., Goldstein, M. and Bower, R. G. (2010). Galaxy formation: a Bayesian uncertainty analysis. *Bayesian Analysis* 5(4), 619–669. https://doi.org/10.1214/10-BA524 [V]
- Andrianakis, I., Vernon, I. R., McCreesh, N., McKinley, T. J., Oakley, J. E., Nsubuga, R. N., Goldstein, M. and White, R. G. (2015). Bayesian history matching of complex infectious disease models using emulation: a tutorial and a case study on HIV in Uganda. *PLoS Computational Biology* 11(1), e1003968. https://doi.org/10.1371/journal.pcbi.1003968 [V]
- Cranmer, K., Brehmer, J. and Louppe, G. (2020). The frontier of simulation-based inference. *PNAS* 117(48), 30055–30062. https://doi.org/10.1073/pnas.1912789117 [V]
- Lueckmann, J.-M., Boelts, J., Greenberg, D. S., Gonçalves, P. J. and Macke, J. H. (2021). Benchmarking simulation-based inference. *Proceedings of AISTATS 2021*, PMLR 130, 343–351. https://proceedings.mlr.press/v130/lueckmann21a.html [V]
- Hermans, J., Delaunoy, A., Rozet, F., Wehenkel, A., Begy, V. and Louppe, G. (2021). A trust crisis in simulation-based inference? Your posterior approximations can be unfaithful. arXiv:2110.06581. https://arxiv.org/abs/2110.06581 [V]
- Talts, S., Betancourt, M., Simpson, D., Vehtari, A. and Gelman, A. (2018). Validating Bayesian inference algorithms with simulation-based calibration. arXiv:1804.06788. https://arxiv.org/abs/1804.06788 [V]
- Grazzini, J., Richiardi, M. G. and Tsionas, M. (2017). Bayesian estimation of agent-based models. *Journal of Economic Dynamics and Control* 77, 26–47. https://doi.org/10.1016/j.jedc.2017.01.014 [V]
- Lamperti, F., Roventini, A. and Sani, A. (2018). Agent-based model calibration using machine learning surrogates. *Journal of Economic Dynamics and Control* 90, 366–389. https://doi.org/10.1016/j.jedc.2018.03.011 [V]
- Platt, D. (2020). A comparison of economic agent-based model calibration methods. *Journal of Economic Dynamics and Control* 113, 103859. https://doi.org/10.1016/j.jedc.2020.103859 [V]
- Dyer, J., Cannon, P., Farmer, J. D. and Schmon, S. M. (2024). Black-box Bayesian inference for agent-based models. *Journal of Economic Dynamics and Control* 161, 104827. https://doi.org/10.1016/j.jedc.2024.104827 [V]
- ten Broeke, G., van Voorn, G. and Ligtenberg, A. (2016). Which sensitivity analysis method should I use for my agent-based model? *JASSS* 19(1), 5. https://doi.org/10.18564/jasss.2857 [V]

### 5.3 What Is Implementable and How It Serves the Feature

The theory note's simulation test (§3.5) declares a model of seeking and asks whether the unmet demand it leaves in $G$ exceeds the standard. The literature says how to make that test mean something:

- **Calibrate before testing, by history matching against several patterns.** The seeking model (agents of each group seeking satisfiers within reach of capacity, on nomos `agents`, with Andersen's predisposing, enabling and need factors as its structure, §9.1) is matched to several observed patterns at once (usage rates by group, travel distances, waiting times, the provision's occupancy), as pattern-oriented modelling requires. History matching rules out implausible parameters in waves; what remains is the non-implausible region $\mathcal{X}$.
- **Report the gap over the region, not for one run.** The simulation supports a need in proportion to the share of $\mathcal{X}$ (weighted by the emulator's uncertainty) in which the gap exceeds the standard. A gap produced only in a corner of $\mathcal{X}$ is a possibility, not support.
- **Implement in tyche:** rejection ABC with regression adjustment and ABC-SMC with model selection (in `bayes`); simulation-based calibration as a check on any inference routine (in `bayes`); history matching with implausibility and waves, on GP emulators (in `uncertainty`; heuresis's `models` has GP surrogates on tyche's conditioning, which an emulator can share). Neural simulation-based inference is best borrowed at the Python edge (the `sbi` package [M]) behind the infrastructure's interface, as CLAUDE.md §2.2 allows.
- **Amortise.** Each statement cannot have its own calibration inside the evolutionary loop. The seeking model is calibrated once per data set; statements then query the calibrated ensemble for their group.

### 5.4 Open Problems

- Identifiability: social ABMs usually have more parameters than the patterns constrain; history matching makes this visible but does not cure it.
- Model discrepancy for social systems has no physical prior; its variance is often a guess, and implausibility depends on it.
- Validating emergent claims: whether a macro-pattern in the model and in the world arise from the same mechanism is not testable from the macro-pattern alone.
- Cost: calibration inside an evolutionary loop is prohibitive without emulators shared across statements.

---

## 6. Policies Evolved Against Simulators

### 6.1 Concepts

Under **deep uncertainty**, where the parties to a decision do not agree on the model, the probabilities or the values, the literature stopped asking for the optimal policy and asked for the **robust** one. **Exploratory modelling** (Bankes 1993) uses a model as a tool to explore an ensemble of plausible worlds, not to predict one. **Robust decision making** (Lempert, Popper and Bankes 2003; Lempert et al. 2006) proposes a candidate strategy, stress-tests it over a large ensemble of futures, and uses **scenario discovery** to find the conditions under which it fails, then revises it. Scenario discovery is done by **PRIM** (Friedman and Fisher 1999), which peels and pastes a box in the space of uncertain factors to trade coverage against density; Bryant and Lempert (2010) set its criteria (coverage, density, interpretability) and Kwakkel and Jaxa-Rozen (2016) extended it to heterogeneous uncertainties. **Many-objective robust decision making** (Kasprzyk et al. 2013) puts a many-objective evolutionary search in front of the stress test; open frameworks exist (Hadka et al. 2015; Kwakkel 2017's EMA Workbench). **Dynamic adaptive policy pathways** (Haasnoot et al. 2013) plan sequences of actions with tipping points. Robustness itself has many measures (regret, satisficing, percentiles of performance); McPhail et al. (2018) and Herman et al. (2015) showed that they rank the same options differently.

*A remark that matters for needs:* PRIM is a subgroup discovery algorithm. Its boxes are conjunctions of interval conditions, and its coverage and density are the coverage and precision of subgroup discovery. One implementation serves both the needs search (a box in the attribute space of units where the gap exceeds the standard) and the policy domain (a box in the space of uncertain factors where an option fails).

### 6.2 How the Literature Guards Against Overfitting to a Model

1. **Re-evaluation out of sample.** MORDM searches under one reference scenario and re-evaluates the Pareto set over an ensemble the search did not see (Kasprzyk et al. 2013).
2. **Robustness in the search.** Watson and Kasprzyk (2017) put deeply uncertain factors into the search; Bartholomew and Kwakkel (2020, abstract read) compared MORDM, multi-scenario MORDM and many-objective robust optimisation on the lake problem and found a trade-off: the more robustness the search considers, the more robust its solutions after re-evaluation and the worse they do in any single reference scenario, with multi-scenario search a middle ground at reasonable cost.
3. **Robust evolutionary optimisation.** Branke (1998) and the survey of Jin and Branke (2005) treat noise, perturbations of solutions and changing environments; Herrmann (1999) coevolves solutions and worst-case scenarios for minimax problems in two populations; domain randomisation trains on many randomised simulators so that the real world looks like one more variation (Tobin et al. 2017).
4. **Adversarial tests of the model itself.** Miller (1998) used a genetic algorithm to search for the small parameter changes that most change a model's conclusions (active nonlinear tests); Stonedahl and Wilensky applied evolutionary search to calibration and robustness checking in agent-based models, exposing in the Artificial Anasazi replication interactions missed by one-at-a-time sensitivity analysis [S], and to finding forms of flocking in parameter space (2011) [V].
5. **Validation with people not used in training.** The AI Economist (Zheng et al. 2022) designs taxes by two-level reinforcement learning in a simulated economy whose agents adapt to the policy; Democratic AI (Koster et al. 2022) trained a redistribution mechanism against agents cloned from human play and then put it to a vote among new human participants, who preferred it to the baselines. The second is the clearer guard: the final test was outside the simulator.

### 6.3 Key Papers

- Bankes, S. (1993). Exploratory modeling for policy analysis. *Operations Research* 41(3), 435–449. https://doi.org/10.1287/opre.41.3.435 [V]
- Lempert, R., Popper, S. and Bankes, S. (2003). *Shaping the Next One Hundred Years: New Methods for Quantitative, Long-Term Policy Analysis*. RAND MR-1626. https://doi.org/10.7249/MR1626 [V]
- Lempert, R. J., Groves, D. G., Popper, S. W. and Bankes, S. C. (2006). A general, analytic method for generating robust strategies and narrative scenarios. *Management Science* 52(4), 514–528. https://doi.org/10.1287/mnsc.1050.0472 [V]
- Friedman, J. H. and Fisher, N. I. (1999). Bump hunting in high-dimensional data. *Statistics and Computing* 9(2), 123–143. https://doi.org/10.1023/A:1008894516817 [V]
- Bryant, B. P. and Lempert, R. J. (2010). Thinking inside the box: a participatory, computer-assisted approach to scenario discovery. *Technological Forecasting and Social Change* 77(1), 34–49. https://doi.org/10.1016/j.techfore.2009.08.002 [V]
- Kwakkel, J. H. and Jaxa-Rozen, M. (2016). Improving scenario discovery for handling heterogeneous uncertainties and multinomial classified outcomes. *Environmental Modelling & Software* 79, 311–321. https://doi.org/10.1016/j.envsoft.2015.11.020 [V]
- Kasprzyk, J. R., Nataraj, S., Reed, P. M. and Lempert, R. J. (2013). Many objective robust decision making for complex environmental systems undergoing change. *Environmental Modelling & Software* 42, 55–71. https://doi.org/10.1016/j.envsoft.2012.12.007 [V]
- Hadka, D., Herman, J., Reed, P. and Keller, K. (2015). An open source framework for many-objective robust decision making. *Environmental Modelling & Software* 74, 114–129. https://doi.org/10.1016/j.envsoft.2015.07.014 [V]
- Kwakkel, J. H. (2017). The Exploratory Modeling Workbench: an open source toolkit for exploratory modeling, scenario discovery, and (multi-objective) robust decision making. *Environmental Modelling & Software* 96, 239–250. https://doi.org/10.1016/j.envsoft.2017.06.054 [V]
- Haasnoot, M., Kwakkel, J. H., Walker, W. E. and ter Maat, J. (2013). Dynamic adaptive policy pathways: a method for crafting robust decisions for a deeply uncertain world. *Global Environmental Change* 23(2), 485–498. https://doi.org/10.1016/j.gloenvcha.2012.12.006 [V]
- McPhail, C., Maier, H. R., Kwakkel, J. H., Giuliani, M., Castelletti, A. and Westra, S. (2018). Robustness metrics: how are they calculated, when should they be used and why do they give different results? *Earth's Future* 6(2), 169–191. https://doi.org/10.1002/2017EF000649 [V]
- Herman, J. D., Reed, P. M., Zeff, H. B. and Characklis, G. W. (2015). How should robustness be defined for water systems planning under change? *Journal of Water Resources Planning and Management* 141(10), 04015012. https://doi.org/10.1061/(ASCE)WR.1943-5452.0000509 [V]
- Watson, A. A. and Kasprzyk, J. R. (2017). Incorporating deeply uncertain factors into the many objective search process. *Environmental Modelling & Software* 89, 159–171. https://doi.org/10.1016/j.envsoft.2016.12.001 [V]
- Bartholomew, E. and Kwakkel, J. H. (2020). On considering robustness in the search phase of Robust Decision Making: a comparison of Many-Objective Robust Decision Making, multi-scenario Many-Objective Robust Decision Making, and Many Objective Robust Optimization. *Environmental Modelling & Software* 127, 104699. https://doi.org/10.1016/j.envsoft.2020.104699 [V]
- Branke, J. (1998). Creating robust solutions by means of evolutionary algorithms. *PPSN V*, LNCS, 119–128. https://doi.org/10.1007/BFb0056855 [V]
- Jin, Y. and Branke, J. (2005). Evolutionary optimization in uncertain environments — a survey. *IEEE Transactions on Evolutionary Computation* 9(3), 303–317. https://doi.org/10.1109/TEVC.2005.846356 [V]
- Herrmann, J. W. (1999). A genetic algorithm for minimax optimization problems. *Proceedings of the 1999 Congress on Evolutionary Computation (CEC99)*, 1099–1103. https://doi.org/10.1109/CEC.1999.782545 [V]
- Tobin, J., Fong, R., Ray, A., Schneider, J., Zaremba, W. and Abbeel, P. (2017). Domain randomization for transferring deep neural networks from simulation to the real world. *IEEE/RSJ IROS 2017*, 23–30. https://doi.org/10.1109/IROS.2017.8202133 [V]
- Miller, J. H. (1998). Active nonlinear tests (ANTs) of complex simulation models. *Management Science* 44(6), 820–830. https://doi.org/10.1287/mnsc.44.6.820 [V]
- Stonedahl, F. and Wilensky, U. (2010). Evolutionary robustness checking in the Artificial Anasazi model. AAAI Fall Symposium on Complex Adaptive Systems. https://ccl.northwestern.edu/2010/Stonedahl%20and%20Wilensky.pdf [S; venue from prior knowledge, M]
- Stonedahl, F. and Wilensky, U. (2011). Finding forms of flocking: evolutionary search in ABM parameter-spaces. *Multi-Agent-Based Simulation XI*, LNCS, 61–75. https://doi.org/10.1007/978-3-642-18345-4_5 [V]
- Zheng, S., Trott, A., Srinivasa, S., Parkes, D. C. and Socher, R. (2022). The AI Economist: taxation policy design via two-level deep multiagent reinforcement learning. *Science Advances* 8(18), eabk2607. https://doi.org/10.1126/sciadv.abk2607 [V]
- Koster, R., Balaguer, J., Tacchetti, A., Weinstein, A., Zhu, T., Hauser, O. et al. (2022). Human-centred mechanism design with Democratic AI. *Nature Human Behaviour* 6, 1398–1407. https://doi.org/10.1038/s41562-022-01383-x [V]
- Dennis, M., Jaques, N., Vinitsky, E., Bayen, A., Russell, S., Critch, A. and Levine, S. (2020). Emergent complexity and zero-shot transfer via unsupervised environment design. arXiv:2012.02096. https://arxiv.org/abs/2012.02096 [V; seventh author from prior knowledge, M]

### 6.4 What Is Implementable and How It Serves the Feature

- **PRIM** (peeling, pasting, the coverage–density trajectory, resampled stability of boxes, Bryant–Lempert's criteria) in the same foundation module as subgroup discovery (§4.7). *Use:* a generator of groups for the needs search; vulnerability analysis of options in the policy domain.
- **Robustness of a statement over analysis choices.** A need judged under one threshold $\tau$, one weighting and one indicator definition is one scenario. Evaluating each statement over an ensemble of reasonable analysis choices (thresholds in a declared interval, alternative weights, alternative indicator codings) and reporting satisficing robustness (the share of the ensemble in which the gap exceeds the standard) is RDM applied to the analysis itself. It is what the "multiverse analysis" of psychology proposes (Steegen et al. 2016 [M]).
- **Scenarios as lexicase cases.** heuresis's lexicase selection treats each scenario (or stratum, or resample) as a case, so that statements which hold across many cases in different orders survive; this is multi-scenario search without aggregating the scenarios into one number (§7.3).
- **Robustness metrics** (regret, satisficing, percentile) and re-evaluation over sampled futures in the policy domain's `options`.
- **Minimax coevolution** of interventions and adverse scenarios in heuresis's planned `coevolution` (§7.2).

### 6.5 Open Problems

- No method guards against a simulator that is wrong in a way no scenario represents; robustness is relative to the ensemble.
- The choice of robustness metric is a choice of values (risk aversion), and the literature shows it changes the answer.
- Policies in social systems change the behaviour they act on (the Lucas critique); learning agents (the AI Economist) address this inside the simulator, not outside it.

---

## 7. Emerging Evolutionary Methods

### 7.1 Concepts

The methods surveyed here change what fitness means. In classical evolution an individual has a fitness against a fixed objective. In **coevolution** fitness is relative to another population (opponents, collaborators, tests). In **lexicase selection** there is no aggregate fitness at all; individuals are compared case by case in random orders. In **novelty search and quality-diversity** fitness is (partly) distance from what has been found. In **open-ended evolution** the problems themselves evolve. In **age-layered and regularised evolution** age limits how long an individual may compete. In **adaptive operator selection** the operators compete for the right to make children. In **evolution driven by language models** the variation operator is a model that reads parents and writes children. Each of these is a relation among individuals rather than a property of one, which is what the needs search needs: a statement is valuable relative to the statements already found, the tests it survives, and the strata in which it holds.

### 7.2 Coevolution

Hillis (1990) coevolved sorting networks with the test cases (parasites) that broke them and obtained better networks than with fixed tests. Potter and De Jong (1994) introduced cooperative coevolution, in which subpopulations evolve parts of a solution evaluated together. Rosin and Belew (1997) introduced competitive fitness sharing, shared sampling and a hall of fame against forgetting. Ficici and Pollack (2001) cast coevolution as multi-objective (Pareto coevolution: each test is an objective); de Jong and Pollack (2004) showed that a small set of tests suffices to evaluate learners ideally and gave archives that keep it. Popovici, Bucci, Wiegand and de Jong (2012) is the reference on **solution concepts** and pathologies (cycling, disengagement when one population outruns the other, forgetting, overspecialisation). Two strands bear directly on hypotheses:

- **Coevolution of models and tests.** Bongard and Lipson (2005, 2007) evolve models of a system together with the experiments that best discriminate among them (tests chosen to maximise the disagreement of the current models), and recover nonlinear dynamical systems from few experiments. Schmidt and Lipson (2008) coevolve fitness predictors (small subsets of the data that predict full fitness), which cuts evaluation cost; Schmidt and Lipson (2009) distilled physical laws from data by symbolic regression.
- **Coevolution of programs and tests** (Arcuri and Yao 2008) for repairing software.

*For needs,* the tests population would be adverse strata: subsets of the discovery data (a region, a wave, a subpopulation, a resample) on which a statement's gap fails or reverses, evolved to break the current statements; statements are rewarded for surviving tests that break others (competitive fitness sharing). Bongard and Lipson's criterion picks, among candidate strata, those on which current statements disagree most, which are the most informative places to look. All of this must run on discovery data; the confirmation part never joins the tests' population.

### 7.3 Relations Among Individuals: Lexicase and Its Descendants

Lexicase selection (Spector 2012; Helmuth, Spector and Matheson 2015) shuffles the cases and filters the population case by case, keeping only the best on each, so that specialists on hard cases survive. ε-lexicase (La Cava, Spector and Danai 2016) keeps everyone within ε of the best on continuous errors; La Cava et al. (2019) analysed its selection probabilities and its relation to Pareto dominance (lexicase selects only from the Pareto front of the cases). Random down-sampling of cases (Hernandez et al. 2019; Helmuth and Spector 2022) reduces cost and often improves results; informed down-sampling (Boldi et al. 2024) chooses the cases that are not redundant; probabilistic lexicase (Ding, Pantridge and Spector 2023) samples from lexicase's distribution efficiently; lexicase has also been used in learning classifier systems (Aenugu and Spector 2019). heuresis already holds lexicase and ε-lexicase.

*For needs,* the cases are the units of robustness: strata, regions, waves and bootstrap replicates of the discovery data. A statement survives lexicase if, in some order of strata, it is among the best on each in turn; this keeps statements that hold strongly in a few strata (local needs) and statements that hold moderately everywhere, without one aggregate deciding. Lexicase gives no statistical guarantee; it promotes statements that generalise across cases, which is the behaviour the confirmation step will then test.

### 7.4 Novelty, Quality-Diversity and Open-Endedness

Novelty search (Lehman and Stanley 2011a) rewards distance in a behaviour space from an archive of what has been found and can outperform objective search on deceptive problems; novelty search with local competition (Lehman and Stanley 2011b) and MAP-Elites (Mouret and Clune 2015) keep the best individual of every kind, which Pugh, Soros and Stanley (2016) named **quality-diversity**. **Open-ended** evolution generates new problems as well as solutions: minimal criterion coevolution (Brant and Stanley 2017) coevolves mazes and solvers under a minimal criterion; POET (Wang, Lehman, Clune and Stanley 2019) and Enhanced POET (Wang et al. 2020) pair environments with agents and transfer agents between environments; unsupervised environment design (Dennis et al. 2020) generates environments at the frontier of an agent's ability. OMNI (Zhang, Lehman, Stanley and Clune 2024) and QDAIF (Bradley et al. 2024) use foundation models to judge which new tasks or outputs are interesting. Stanley (2019) argues why open-endedness matters.

*For needs,* the archive should be a QD archive: the best statement of every kind, with kinds given by descriptors such as the domain, the kind of standard, a coverage band and the number of conditions, so that the archive holds the strongest care need among the old as well as the strongest housing need among renters, rather than many variants of one. Novelty between statements is naturally the (weighted) Jaccard distance between their member sets, joined with differences of domain, indicator and standard; heuresis's `novelty` and `map_elites` take such descriptors.

### 7.5 Age and Regularisation

The age-layered population structure (ALPS; Hornby 2006) admits random newcomers continually in the lowest age layer and lets individuals compete only within their layer, which counters premature convergence. Regularised (aging) evolution (Real, Aggarwal, Huang and Le 2019) removes the oldest individual instead of the worst, which in architecture search matched or beat reinforcement learning and was more robust to noise in evaluation; the authors read aging as a regulariser, since only architectures that keep being re-derived persist. heuresis's in-progress `evolution` module holds survival by aging.

*For needs,* the user's rule (retire old statements after long inefficacy) is a form of aging. The literature supports it as a *search* device, and it should be kept apart from *evidential* revocation (§3.4): a statement removed from the breeding population by age may still stand in the archive, and one revoked by new data must leave both.

### 7.6 Adaptive Operator Selection

Adaptive operator selection learns which variation operator to apply from the rewards of the operators' children: probability matching and adaptive pursuit (Thierens 2005), multi-armed bandits (Fialho, Da Costa, Schoenauer and Sebag 2010, who found rank-based and extreme-value credit useful because rare large improvements matter most), within the broader field of parameter control (Karafotias, Hoogendoorn and Eiben 2015, who note that most control methods are tested on few problems and seldom used in practice). heuresis's in-progress `evolution/credit.hpp` holds probability matching, adaptive pursuit, UCB1 and Thompson sampling.

*For needs,* the generators (enumeration, subgroup discovery, PRIM, causal trees, rules, simulation, a language model) are arms. Credit must be computed from discovery-side quality only; if an operator's credit counted confirmation successes, the bandit would become a channel through which the confirmation part steers the search (§12.2). Non-stationarity is the rule (a language model's proposals are good early and repetitive later), so discounted or windowed credit is needed.

### 7.7 Evolution Driven by Language Models

- **FunSearch** (Romera-Paredes et al. 2024) evolves programs with a language model as mutation operator, an island model and an automatic evaluator; it found new constructions for the cap set problem and better bin-packing heuristics. Its reliability rests entirely on the evaluator, which is exact and cannot be argued with.
- **Evolution through large models** (ELM; Lehman et al. 2022/2024) and **language model crossover** (LMX; Meyerson et al. 2024) use a model as a mutation or crossover operator by few-shot prompting with parents.
- **EvoPrompt** (Guo et al. 2024) and **OPRO** (Yang et al. 2024) optimise prompts, OPRO by a meta-prompt holding the best solutions so far with their scores; both are ICLR 2024 papers.
- **Evolution of heuristics** (EoH; Liu et al. 2024) evolves natural-language "thoughts" with the code that implements them; **ReEvo** (Ye et al. 2024) adds reflection on why parents differ in fitness.
- **AlphaEvolve** (Novikov et al. 2025; a white paper on arXiv, no peer-reviewed version found) uses an ensemble of models, evaluator cascades and an evolutionary database; it reports, among others, a procedure for multiplying two 4×4 complex matrices with 48 scalar multiplications.
- **LLM-SR** (Shojaee et al. 2025) proposes equation skeletons whose parameters are then fitted to data, with benchmarks built to limit recitation.
- **LLM-based genetic programming:** Hemberg, Moskal and O'Reilly (2024) evolve code with a model as the operator. Wu et al. (2025) survey evolutionary computation in the era of language models.

The pattern of LLM-SR is the one that fits needs: the model proposes the *form* of a statement (a group's conditions, a domain, an indicator, a kind of standard), the data fix its *numbers* (thresholds, the gap), and an exact evaluator scores it.

### 7.8 Language Models as Generators and Testers of Hypotheses

- **From data and literature:** HypoGeniC (Zhou et al. 2024; abstract read) generates hypotheses from few examples and updates them iteratively with a reward inspired by multi-armed bandits, reporting accuracy gains over few-shot prompting; Liu et al. (2024) combine literature and data; Yang et al. (2023) generate open-domain hypotheses from text; Tong et al. (2024) join a causal graph mined from the psychological literature with a model to propose hypotheses.
- **Machine learning as a hypothesis generator:** Ludwig and Mullainathan (2024) used a model of judges' detention decisions to surface facial features that predicted them, translated these into hypotheses people could understand, and tested them on new data and with human subjects. The model proposes; new data decide.
- **Automated social science:** Manning, Zhu and Horton (2024; abstract read) state hypotheses as structural causal models, test them by simulations with language-model agents, and find that the model predicts the signs of effects but not their magnitudes, and predicts much better when it can condition on the fitted model.
- **Sequential falsification:** POPPER (Huang, Jin, Li, Li, Candès and Leskovec 2025) has agents design falsification experiments for a free-form hypothesis and aggregates the experiments' p-values, converted to e-values, sequentially, declaring rejection when the e-value exceeds $1/\alpha$; it reports type-I error control and performance comparable to human scientists in a tenth of the time (abstract read; mechanism from secondary summaries of the paper).
- **"AI scientist" systems:** the AI Scientist (Lu et al. 2024; a version published in *Nature* in 2026), the AI Scientist-v2 (Yamada et al. 2025), and the AI co-scientist (Gottweis et al. 2025, its title revised on arXiv to "Accelerating scientific discovery with Co-Scientist").
- **Language models as survey respondents:** Argyle et al. (2023) argued that conditioned models reproduce subgroups' response distributions ("algorithmic fidelity"); Bisbee et al. (2024; abstract read) found that their responses vary less than people's, that about half of the regression coefficients differ significantly from the survey's (a third of those with the opposite sign), and that results change with small changes of wording and over three months.

### 7.9 What Was Found About Reliability

| Finding | Source |
|---|---|
| Model-driven evolution works where an exact, cheap, ungameable evaluator exists | FunSearch, AlphaEvolve, LLM-SR, EoH |
| Models' self-evaluation of ideas fails, and their ideas lack diversity, though experts rate them more novel before execution | Si, Yang and Hashimoto (arXiv 2024; abstract read) |
| After execution, model ideas lose significantly more of their scores than experts' ideas on every metric, closing or reversing the gap | Si, Hashimoto and Yang (arXiv 2025; abstract read) |
| An "AI scientist" misjudged novelty, 42% of its experiments failed from coding errors, its manuscripts had a median of five citations, and some contained hallucinated numerical results | Beel, Kan and Baumgart (arXiv 2025; abstract read) |
| Models do not reliably correct their own reasoning without external feedback | Huang et al. (arXiv 2023) [content M] |
| Model judges show position, verbosity and self-enhancement biases | Zheng et al. (arXiv 2023) [content M] |
| The best systems solve 25% of a benchmark of 264 data-driven discovery tasks in six domains | Majumder et al. (DiscoveryBench, arXiv 2024; abstract read) |
| Models predict signs, not magnitudes, of social effects | Manning, Zhu and Horton (2024) |
| Synthetic survey respondents are less variable, unstable and biased in regressions | Bisbee et al. (2024) |
| Formal sequential testing over model-designed experiments can control type-I error | POPPER (2025) |

*Consequence for needs:* a language model may generate and vary statements, and may propose falsification checks; it must never grade them, never see confirmation results, and never stand in for felt need (synthetic respondents are not evidence of what people say). Its proposals enter as grade C, as the infrastructure already requires.

### 7.10 Key Papers

- Hillis, W. D. (1990). Co-evolving parasites improve simulated evolution as an optimization procedure. *Physica D* 42(1–3), 228–234. https://doi.org/10.1016/0167-2789(90)90076-2 [V]
- Potter, M. A. and De Jong, K. A. (1994). A cooperative coevolutionary approach to function optimization. *PPSN III*, LNCS, 249–257. https://doi.org/10.1007/3-540-58484-6_269 [V]
- Rosin, C. D. and Belew, R. K. (1997). New methods for competitive coevolution. *Evolutionary Computation* 5(1), 1–29. https://doi.org/10.1162/evco.1997.5.1.1 [V]
- Ficici, S. G. and Pollack, J. B. (2001). Pareto optimality in coevolutionary learning. *Advances in Artificial Life (ECAL 2001)*, LNCS, 316–325. https://doi.org/10.1007/3-540-44811-X_34 [V]
- de Jong, E. D. and Pollack, J. B. (2004). Ideal evaluation from coevolution. *Evolutionary Computation* 12(2), 159–192. https://doi.org/10.1162/106365604773955139 [V]
- Popovici, E., Bucci, A., Wiegand, R. P. and de Jong, E. D. (2012). Coevolutionary principles. In *Handbook of Natural Computing*, Springer, 987–1033. https://doi.org/10.1007/978-3-540-92910-9_31 [V]
- Bongard, J. C. and Lipson, H. (2005). Nonlinear system identification using coevolution of models and tests. *IEEE Transactions on Evolutionary Computation* 9(4), 361–384. https://doi.org/10.1109/TEVC.2005.850293 [V]
- Bongard, J. and Lipson, H. (2007). Automated reverse engineering of nonlinear dynamical systems. *PNAS* 104(24), 9943–9948. https://doi.org/10.1073/pnas.0609476104 [V]
- Schmidt, M. D. and Lipson, H. (2008). Coevolution of fitness predictors. *IEEE Transactions on Evolutionary Computation* 12(6), 736–749. https://doi.org/10.1109/TEVC.2008.919006 [V]
- Schmidt, M. and Lipson, H. (2009). Distilling free-form natural laws from experimental data. *Science* 324(5923), 81–85. https://doi.org/10.1126/science.1165893 [V]
- Arcuri, A. and Yao, X. (2008). A novel co-evolutionary approach to automatic software bug fixing. *IEEE CEC 2008*, 162–168. https://doi.org/10.1109/CEC.2008.4630793 [V]
- Spector, L. (2012). Assessment of problem modality by differential performance of lexicase selection in genetic programming: a preliminary report. *GECCO 2012 Companion*, 401–408. https://doi.org/10.1145/2330784.2330846 [V]
- Helmuth, T., Spector, L. and Matheson, J. (2015). Solving uncompromising problems with lexicase selection. *IEEE Transactions on Evolutionary Computation* 19(5), 630–643. https://doi.org/10.1109/TEVC.2014.2362729 [V]
- La Cava, W., Spector, L. and Danai, K. (2016). Epsilon-lexicase selection for regression. *GECCO 2016*, 741–748. https://doi.org/10.1145/2908812.2908898 [V]
- La Cava, W., Helmuth, T., Spector, L. and Moore, J. H. (2019). A probabilistic and multi-objective analysis of lexicase selection and ε-lexicase selection. *Evolutionary Computation* 27(3), 377–402. https://doi.org/10.1162/evco_a_00224 [V]
- Hernandez, J. G., Lalejini, A., Dolson, E. and Ofria, C. (2019). Random subsampling improves performance in lexicase selection. *GECCO 2019 Companion*, 2028–2031. https://doi.org/10.1145/3319619.3326900 [V]
- Helmuth, T. and Spector, L. (2022). Problem-solving benefits of down-sampled lexicase selection. *Artificial Life* 27(3–4), 183–203. https://doi.org/10.1162/artl_a_00341 [V]
- Boldi, R., Briesch, M., Sobania, D., Lalejini, A., Helmuth, T., Rothlauf, F., Ofria, C. and Spector, L. (2024). Informed down-sampled lexicase selection: identifying productive training cases for efficient problem solving. *Evolutionary Computation* 32(4), 307–337. https://doi.org/10.1162/evco_a_00346 [V]
- Ding, L., Pantridge, E. and Spector, L. (2023). Probabilistic lexicase selection. *GECCO 2023*, 1073–1081. https://doi.org/10.1145/3583131.3590375 [V]
- Aenugu, S. and Spector, L. (2019). Lexicase selection in learning classifier systems. *GECCO 2019*, 356–364. https://doi.org/10.1145/3321707.3321828 [V]
- Lehman, J. and Stanley, K. O. (2011a). Abandoning objectives: evolution through the search for novelty alone. *Evolutionary Computation* 19(2), 189–223. https://doi.org/10.1162/EVCO_a_00025 [V]
- Lehman, J. and Stanley, K. O. (2011b). Evolving a diversity of virtual creatures through novelty search and local competition. *GECCO 2011*, 211–218. https://doi.org/10.1145/2001576.2001606 [V]
- Mouret, J.-B. and Clune, J. (2015). Illuminating search spaces by mapping elites. arXiv:1504.04909. https://arxiv.org/abs/1504.04909 [V]
- Pugh, J. K., Soros, L. B. and Stanley, K. O. (2016). Quality diversity: a new frontier for evolutionary computation. *Frontiers in Robotics and AI* 3, 40. https://doi.org/10.3389/frobt.2016.00040 [V]
- Brant, J. C. and Stanley, K. O. (2017). Minimal criterion coevolution: a new approach to open-ended search. *GECCO 2017*, 67–74. https://doi.org/10.1145/3071178.3071186 [V]
- Wang, R., Lehman, J., Clune, J. and Stanley, K. O. (2019). Paired open-ended trailblazer (POET): endlessly generating increasingly complex and diverse learning environments and their solutions. arXiv:1901.01753. https://arxiv.org/abs/1901.01753 [V]
- Wang, R., Lehman, J., Rawal, A., Zhi, J., Li, Y., Clune, J. and Stanley, K. O. (2020). Enhanced POET: open-ended reinforcement learning through unbounded invention of learning challenges and their solutions. *Proceedings of ICML 2020*, PMLR 119, 9940–9951. https://proceedings.mlr.press/v119/wang20l.html [V]
- Stanley, K. O. (2019). Why open-endedness matters. *Artificial Life* 25(3), 232–235. https://doi.org/10.1162/artl_a_00294 [V]
- Zhang, J., Lehman, J., Stanley, K. and Clune, J. (2023). OMNI: open-endedness via models of human notions of interestingness. arXiv:2306.01711. https://arxiv.org/abs/2306.01711 [V; ICLR 2024 acceptance M]
- Bradley, H., Dai, A., Teufel, H., Zhang, J., Oostermeijer, K., Bellagente, M. et al. (2023). Quality-diversity through AI feedback. arXiv:2310.13032. https://arxiv.org/abs/2310.13032 [V; ICLR 2024 acceptance M]
- Hornby, G. S. (2006). ALPS: the age-layered population structure for reducing the problem of premature convergence. *GECCO 2006*, 815–822. https://doi.org/10.1145/1143997.1144142 [V]
- Real, E., Aggarwal, A., Huang, Y. and Le, Q. V. (2019). Regularized evolution for image classifier architecture search. *Proceedings of AAAI 2019* 33(01), 4780–4789. https://doi.org/10.1609/aaai.v33i01.33014780 [V]
- Thierens, D. (2005). An adaptive pursuit strategy for allocating operator probabilities. *GECCO 2005*, 1539–1546. https://doi.org/10.1145/1068009.1068251 [V]
- Fialho, Á., Da Costa, L., Schoenauer, M. and Sebag, M. (2010). Analyzing bandit-based adaptive operator selection mechanisms. *Annals of Mathematics and Artificial Intelligence* 60(1–2), 25–64. https://doi.org/10.1007/s10472-010-9213-y [V]
- Karafotias, G., Hoogendoorn, M. and Eiben, A. E. (2015). Parameter control in evolutionary algorithms: trends and challenges. *IEEE Transactions on Evolutionary Computation* 19(2), 167–187. https://doi.org/10.1109/TEVC.2014.2308294 [V]
- Romera-Paredes, B., Barekatain, M., Novikov, A., Balog, M., Kumar, M. P., Dupont, E. et al. (2024). Mathematical discoveries from program search with large language models. *Nature* 625(7995), 468–475. https://doi.org/10.1038/s41586-023-06924-6 [V]
- Lehman, J., Gordon, J., Jain, S., Ndousse, K., Yeh, C. and Stanley, K. O. (2022). Evolution through large models. arXiv:2206.08896. https://arxiv.org/abs/2206.08896 [V]
- Meyerson, E., Nelson, M. J., Bradley, H., Gaier, A., Moradi, A., Hoover, A. K. and Lehman, J. (2024). Language model crossover: variation through few-shot prompting. *ACM Transactions on Evolutionary Learning and Optimization* 4(4), 1–40. https://doi.org/10.1145/3694791 [V]
- Guo, Q., Wang, R., Guo, J., Li, B., Song, K., Tan, X., Liu, G., Bian, J. and Yang, Y. (2024). Connecting large language models with evolutionary algorithms yields powerful prompt optimizers (EvoPrompt). *ICLR 2024*. https://proceedings.iclr.cc/paper_files/paper/2024/hash/9156b0f6dfa9bbd18c79cc459ef5d61c-Abstract-Conference.html ; arXiv:2309.08532 [V]
- Yang, C., Wang, X., Lu, Y., Liu, H., Le, Q. V., Zhou, D. and Chen, X. (2024). Large language models as optimizers (OPRO). *ICLR 2024*. https://proceedings.iclr.cc/paper_files/paper/2024/hash/3339f19c5fcee3ad74502947a32be9e6-Abstract-Conference.html ; arXiv:2309.03409 [V]
- Liu, F., Tong, X., Yuan, M., Lin, X., Luo, F., Wang, Z., Lu, Z. and Zhang, Q. (2024). Evolution of heuristics: towards efficient automatic algorithm design using large language model. *Proceedings of ICML 2024*, PMLR 235, 32201–32223. https://proceedings.mlr.press/v235/liu24bs.html [V]
- Ye, H., Wang, J., Cao, Z., Berto, F., Hua, C., Kim, H. et al. (2024). ReEvo: large language models as hyper-heuristics with reflective evolution. arXiv:2402.01145. https://arxiv.org/abs/2402.01145 [V; NeurIPS 2024 acceptance M]
- Novikov, A., Vũ, N., Eisenberger, M., Dupont, E., Huang, P.-S., Wagner, A. Z. et al. (2025). AlphaEvolve: a coding agent for scientific and algorithmic discovery. arXiv:2506.13131. https://arxiv.org/abs/2506.13131 [V]
- Shojaee, P., Meidani, K., Gupta, S., Barati Farimani, A. and Reddy, C. K. (2025). LLM-SR: scientific equation discovery via programming with large language models. *ICLR 2025*. https://proceedings.iclr.cc/paper_files/paper/2025/hash/28df8e730c054c5331855fd4d5403ba9-Abstract-Conference.html ; arXiv:2404.18400 [V]
- Hemberg, E., Moskal, S. and O'Reilly, U.-M. (2024). Evolving code with a large language model. *Genetic Programming and Evolvable Machines* 25(2), 21. https://doi.org/10.1007/s10710-024-09494-2 [V]
- Wu, X., Wu, S.-H., Wu, J., Feng, L. and Tan, K. C. (2025). Evolutionary computation in the era of large language model: survey and roadmap. *IEEE Transactions on Evolutionary Computation* 29(2), 534–554. https://doi.org/10.1109/TEVC.2024.3506731 [V]
- Zhou, Y., Liu, H., Srivastava, T., Mei, H. and Tan, C. (2024). Hypothesis generation with large language models. arXiv:2404.04326. https://arxiv.org/abs/2404.04326 [V]
- Liu, H., Zhou, Y., Li, M., Yuan, C. and Tan, C. (2024). Literature meets data: a synergistic approach to hypothesis generation. arXiv:2410.17309. https://arxiv.org/abs/2410.17309 [V]
- Yang, Z., Du, X., Li, J., Zheng, J., Poria, S. and Cambria, E. (2023). Large language models for automated open-domain scientific hypotheses discovery. arXiv:2309.02726. https://arxiv.org/abs/2309.02726 [V; Findings of ACL 2024 M]
- Tong, S., Mao, K., Huang, Z., Zhao, Y. and Peng, K. (2024). Automating psychological hypothesis generation with AI: when large language models meet causal graph. *Humanities and Social Sciences Communications* 11, 896. https://doi.org/10.1057/s41599-024-03407-5 [V]
- Ludwig, J. and Mullainathan, S. (2024). Machine learning as a tool for hypothesis generation. *Quarterly Journal of Economics* 139(2), 751–827. https://doi.org/10.1093/qje/qjad055 [V]
- Manning, B. S., Zhu, K. and Horton, J. J. (2024). Automated social science: language models as scientist and subjects. arXiv:2404.11794. https://arxiv.org/abs/2404.11794 [V]
- Huang, K., Jin, Y., Li, R., Li, M. Y., Candès, E. and Leskovec, J. (2025). Automated hypothesis validation with agentic sequential falsifications. *Proceedings of ICML 2025*, PMLR 267, 25372–25437. https://proceedings.mlr.press/v267/huang25n.html [V]
- Lu, C., Lu, C., Lange, R. T., Foerster, J., Clune, J. and Ha, D. (2024). The AI Scientist: towards fully automated open-ended scientific discovery. arXiv:2408.06292. https://arxiv.org/abs/2408.06292 [V]
- Lu, C., Lu, C., Lange, R. T., Yamada, Y., Hu, S., Foerster, J., Ha, D. and Clune, J. (2026). Towards end-to-end automation of AI research. *Nature* 651(8107), 914–919. https://doi.org/10.1038/s41586-026-10265-5 [V]
- Yamada, Y., Lange, R. T., Lu, C., Hu, S., Lu, C., Foerster, J. et al. (2025). The AI Scientist-v2: workshop-level automated scientific discovery via agentic tree search. arXiv:2504.08066. https://arxiv.org/abs/2504.08066 [V]
- Gottweis, J., Weng, W.-H., Daryin, A., Tu, T., Sirkovic, P., Myaskovsky, A. et al. (2025). Towards an AI co-scientist (revised as "Accelerating scientific discovery with Co-Scientist"). arXiv:2502.18864. https://arxiv.org/abs/2502.18864 [V]
- Beel, J., Kan, M.-Y. and Baumgart, M. (2025). Evaluating Sakana's AI Scientist: bold claims, mixed results, and a promising future? arXiv:2502.14297. https://arxiv.org/abs/2502.14297 [V]
- Si, C., Yang, D. and Hashimoto, T. (2024). Can LLMs generate novel research ideas? A large-scale human study with 100+ NLP researchers. arXiv:2409.04109. https://arxiv.org/abs/2409.04109 [V; ICLR 2025 acceptance S]
- Si, C., Hashimoto, T. and Yang, D. (2025). The ideation–execution gap: execution outcomes of LLM-generated versus human research ideas. arXiv:2506.20803. https://arxiv.org/abs/2506.20803 [V]
- Majumder, B. P., Surana, H., Agarwal, D., Dalvi Mishra, B., Meena, A., Prakhar, A. et al. (2024). DiscoveryBench: towards data-driven discovery with large language models. arXiv:2407.01725. https://arxiv.org/abs/2407.01725 [V]
- Huang, J., Chen, X., Mishra, S., Zheng, H. S., Yu, A. W., Song, X. and Zhou, D. (2023). Large language models cannot self-correct reasoning yet. arXiv:2310.01798. https://arxiv.org/abs/2310.01798 [V]
- Zheng, L., Chiang, W.-L., Sheng, Y., Zhuang, S., Wu, Z., Zhuang, Y. et al. (2023). Judging LLM-as-a-judge with MT-Bench and Chatbot Arena. arXiv:2306.05685. https://arxiv.org/abs/2306.05685 [V]
- Argyle, L. P., Busby, E. C., Fulda, N., Gubler, J. R., Rytting, C. and Wingate, D. (2023). Out of one, many: using language models to simulate human samples. *Political Analysis* 31(3), 337–351. https://doi.org/10.1017/pan.2023.2 [V]
- Bisbee, J., Clinton, J. D., Dorff, C., Kenkel, B. and Larson, J. M. (2024). Synthetic replacements for human survey data? The perils of large language models. *Political Analysis* 32(4), 401–416. https://doi.org/10.1017/pan.2024.5 [V]
- Park, J. S., O'Brien, J., Cai, C. J., Morris, M. R., Liang, P. and Bernstein, M. S. (2023). Generative agents: interactive simulacra of human behavior. *UIST 2023*, 1–22. https://doi.org/10.1145/3586183.3606763 [V]

### 7.11 What Is Implementable and How It Serves the Feature

- **heuresis `coevolution`** (planned): competitive coevolution with fitness sharing and a hall of fame; Pareto coevolution with an archive of distinguishing tests; Bongard–Lipson test selection by disagreement; Herrmann's two-space minimax. In the needs search: a population of adverse strata of the discovery data against the population of statements.
- **Lexicase over strata** (exists): the needs domain supplies the cases (strata, regions, waves, bootstrap replicates of discovery data) and each statement's per-case gap.
- **QD archive of statements** (exists): MAP-Elites over declared descriptors; novelty by weighted Jaccard distance of member sets.
- **ALPS layers** (to add beside aging in `evolution`): continual injection of fresh statements from the generators, which counters the archive's tendency to breed only variants of early finds.
- **Bandit selection of generators** (exists in progress): with discounting, and credit from discovery-side quality only.
- **A language model as an operator, through the existing seam.** heuresis already accepts external proposers whose proposals enter as injected individuals with their origin; the API's `science.models` already asks a model and records grade C. The needs domain needs only a prompt contract: the model receives the statement language, the top statements by discovery quality with their discovery scores (OPRO's meta-prompt), and returns statements in the language, which the domain reads or refuses. It receives no confirmation data, no confirmation outcomes and no archive standing.
- **A POPPER-like falsifier** as a later step: a model proposes falsification checks of a statement (a stratum where it should fail if false, a placebo indicator), the domain runs them on discovery data, and their e-values accumulate as a discovery-side screen before confirmation.

### 7.12 Open Problems

- Coevolution's pathologies (cycling, disengagement, forgetting) are well described but have no general cure; their behaviour on statistical hypotheses rather than games is unstudied.
- Lexicase's selection pressure on statistical noise: on small strata, the best statement on a case is often the luckiest.
- The interaction of QD archives with FDR: the archive keeps the best of every kind, which is a selection per cell; confirmation must account for the number of cells.
- Language-model variation tends to collapse to familiar ideas (Si et al.); whether QD pressure counteracts it for hypotheses is untested.
- Memorisation: a model may "discover" a need it read in a published assessment. Held-out confirmation still tests it, but claims of novelty would be false (Beel et al.'s novelty misjudgements).

---

## 8. Formal Reasoning for Hypotheses and Norms

### 8.1 Inductive Logic Programming

Inductive logic programming (Muggleton 1991) learns logic programs that, with background knowledge, entail positive examples and not negative ones. Progol (Muggleton 1995) bounds the search by inverse entailment from a bottom clause; meta-interpretive learning (Muggleton, Lin and Tamaddoni-Nezhad 2015) invents predicates; ∂ILP (Evans and Grefenstette 2018) learns from noisy data by differentiable relaxation; Popper (Cropper and Morel 2021) learns from failures: each failed hypothesis yields constraints that prune its generalisations or specialisations, in a generate–test–constrain loop. Cropper and Dumančić (2022) review the field.

*For needs:* rule-shaped hypotheses over relational data (households, members, places, services) are ILP's home ground, but ILP needs labelled examples of need, which surveys rarely have. Its best use here is learning the *normative* rules implicit in past decisions (who was granted what), which belong to the law and policy domains, and then deriving normative needs from them (§8.4). Popper's constraint loop is the most transferable idea: a refuted statement should prune the region of the language it implies. The gap is not monotone under specialisation, but coverage is anti-monotone and optimistic estimates bound quality (§4.5), so some pruning is exact.

### 8.2 Abduction

Abductive logic programming (Kakas, Kowalski and Toni 1992) explains observations by assumptions (abducibles) consistent with integrity constraints. *For needs:* once a gap is found, abduction over a declared theory of access (Andersen's predisposing, enabling and need factors; Levesque's dimensions of accessibility, §9.1) proposes the mechanisms that could explain it (distance, cost, eligibility, awareness), each of which becomes a testable statement or a structure for the simulation test. This is generation of explanations, not evidence for them.

### 8.3 Argumentation

Dung (1995) defined abstract argumentation frameworks and their semantics (grounded, preferred, stable); ASPIC+ (Modgil and Prakken 2014) builds arguments from strict and defeasible rules with rebuttal, undercut and undermining, preferences, and rationality postulates; Prakken and Sartor (1997) made priorities themselves defeasible. Value-based argumentation (Bench-Capon 2003) makes an attack succeed or fail relative to an audience's ordering of values. Practical reasoning as presumptive argumentation (Atkinson and Bench-Capon 2007) gives the scheme "in circumstances R we should perform A to achieve S, which realises goal G and promotes value V", with critical questions as the ways to attack it; argumentation schemes (Walton, Reed and Macagno 2008) catalogue such patterns (from expert opinion, from consequences, from analogy).

*For needs,* three uses are direct. (1) **Whose standard.** Bradshaw's four standards can conflict: a group may have a normative need that its members do not feel, or a felt need no authority recognises. A value-based framework represents this exactly: each standard is backed by a value (expert judgement, autonomy, demand, equality), and a statement's standing is relative to an audience's ordering of those values, which the domain declares and never decides. (2) **From need to action.** A robust need with a provision is the premise of a practical argument for providing; its critical questions (is provision the means? are there side effects? does it demote another value?) are where causal support (§4) and simulation (§5) enter as attacks or defences. (3) **Combining evidence.** Statistical, causal, simulation and rule-based tests become arguments with undercutters (confounding undercuts a causal claim; reporting propensity undercuts an expressed-need gap), so that standing is computed by ASPIC+ rather than by an ad hoc rule. apodeixis holds Dung's frameworks and ASPIC+ already; tekmerion reads charts of evidence as ASPIC+ theories.

### 8.4 Defeasible Deontic Logic for Normative Standards

Defeasible logic has efficient proof theories and representation results (Antoniou, Billington, Governatori and Maher 2001). Governatori and Rotolo (2006) gave a Gentzen system for contrary-to-duty obligations with a connective of reparation ($a \otimes b$: $a$ is obligatory, and if it is violated, $b$); Governatori, Olivieri, Rotolo and Scannapieco (2013) computed strong and weak permissions; SPINdle (Lam and Governatori 2009) is a reasoner for it.

*For needs:* normative standards are rules with exceptions and reparations: a person who cannot dress alone is entitled to a care visit a week, unless they live in residential care; if no visit is provided, an allowance is owed. A normative need exists exactly where an obligation of provision is in force for a unit and is violated without its reparation. Defeasible deontic logic states this without hand-coding the exceptions into the indicator. The law domain's `norms` (rules, exceptions, open terms, counts-as links applied to a record of facts) already holds much of the machinery; the theory note's rule test (§3.6) should use it, or apodeixis should gain a defeasible deontic calculus with reparation chains.

### 8.5 Category-Theoretic Approaches

Ologs (Spivak and Kent 2012) represent a knowledge domain as a category whose objects are types, arrows are functional aspects and commutative diagrams are facts; instances are set-valued functors. Functorial data migration (Spivak 2012) moves instances between schemas by the functors Δ, Σ and Π (pullback and the left and right Kan extensions). *Compositional modelling* builds models from open parts: reaction networks composed at their boundaries (Baez and Pollard 2017); epidemic models composed by structured cospans and **stratified** by pullbacks over a type graph (Libkind, Baas, Halter, Patterson and Fairbanks 2022); attributed C-sets as data structures for such models (Patterson, Lynch and Fairbanks 2022). Causal models live in Markov categories (Fritz 2020), and interventions can be performed as surgery on string diagrams (Jacobs, Kissinger and Zanasi 2019). Fong and Spivak (2019) is the introduction.

*For needs:* (1) **The vocabulary as an olog**: units, groups, domains of satisfaction, satisfiers, indicators and standards, with Max-Neef's matrix (needs by existential categories, filled by satisfiers) as a schema rather than a table. kategoria can verify it, as it verifies the history domain's and the law domain's schemas. (2) **Archives that survive revision of the language.** When the statement language changes (a new kind of standard, a split domain), archived statements migrate by kategoria's Kan extensions, as the law domain's records already do between versions of its schema. (3) **Groups as stratifications.** A group that is a conjunction of categorical conditions is a fibre of a product of type systems, which is the pullback stratification of Libkind et al.; this is a precise way to say how a statement for "renters aged 75 or over" relates to statements for each factor, and could later support compositional models of seeking.

### 8.6 Key Papers

- Muggleton, S. (1991). Inductive logic programming. *New Generation Computing* 8(4), 295–318. https://doi.org/10.1007/BF03037089 [V]
- Muggleton, S. (1995). Inverse entailment and Progol. *New Generation Computing* 13(3–4), 245–286. https://doi.org/10.1007/BF03037227 [V]
- Muggleton, S. H., Lin, D. and Tamaddoni-Nezhad, A. (2015). Meta-interpretive learning of higher-order dyadic datalog: predicate invention revisited. *Machine Learning* 100(1), 49–73. https://doi.org/10.1007/s10994-014-5471-y [V]
- Evans, R. and Grefenstette, E. (2018). Learning explanatory rules from noisy data. *Journal of Artificial Intelligence Research* 61, 1–64. https://doi.org/10.1613/jair.5714 [V]
- Cropper, A. and Morel, R. (2021). Learning programs by learning from failures. *Machine Learning* 110(4), 801–856. https://doi.org/10.1007/s10994-020-05934-z [V]
- Cropper, A. and Dumančić, S. (2022). Inductive logic programming at 30: a new introduction. *Journal of Artificial Intelligence Research* 74, 765–850. https://doi.org/10.1613/jair.1.13507 [V]
- Kakas, A. C., Kowalski, R. A. and Toni, F. (1992). Abductive logic programming. *Journal of Logic and Computation* 2(6), 719–770. https://doi.org/10.1093/logcom/2.6.719 [V]
- Dung, P. M. (1995). On the acceptability of arguments and its fundamental role in nonmonotonic reasoning, logic programming and n-person games. *Artificial Intelligence* 77(2), 321–357. https://doi.org/10.1016/0004-3702(94)00041-X [V]
- Modgil, S. and Prakken, H. (2014). The ASPIC+ framework for structured argumentation: a tutorial. *Argument & Computation* 5(1), 31–62. https://doi.org/10.1080/19462166.2013.869766 [V]
- Prakken, H. and Sartor, G. (1997). Argument-based extended logic programming with defeasible priorities. *Journal of Applied Non-Classical Logics* 7(1–2), 25–75. https://doi.org/10.1080/11663081.1997.10510900 [V]
- Bench-Capon, T. J. M. (2003). Persuasion in practical argument using value-based argumentation frameworks. *Journal of Logic and Computation* 13(3), 429–448. https://doi.org/10.1093/logcom/13.3.429 [V]
- Atkinson, K. and Bench-Capon, T. (2007). Practical reasoning as presumptive argumentation using action based alternating transition systems. *Artificial Intelligence* 171(10–15), 855–874. https://doi.org/10.1016/j.artint.2007.04.009 [V]
- Walton, D., Reed, C. and Macagno, F. (2008). *Argumentation Schemes*. Cambridge University Press. https://doi.org/10.1017/CBO9780511802034 [V]
- Antoniou, G., Billington, D., Governatori, G. and Maher, M. J. (2001). Representation results for defeasible logic. *ACM Transactions on Computational Logic* 2(2), 255–287. https://doi.org/10.1145/371316.371517 [V]
- Governatori, G. and Rotolo, A. (2006). Logic of violations: a Gentzen system for reasoning with contrary-to-duty obligations. *Australasian Journal of Logic* 4, 193–215. https://ojs.victoria.ac.nz/ajl/article/view/1780 [V]
- Governatori, G., Olivieri, F., Rotolo, A. and Scannapieco, S. (2013). Computing strong and weak permissions in defeasible logic. *Journal of Philosophical Logic* 42(6), 799–829. https://doi.org/10.1007/s10992-013-9295-1 [V]
- Lam, H.-P. and Governatori, G. (2009). The making of SPINdle. *Rule Interchange and Applications (RuleML 2009)*, LNCS, 315–322. https://doi.org/10.1007/978-3-642-04985-9_29 [V]
- Spivak, D. I. and Kent, R. E. (2012). Ologs: a categorical framework for knowledge representation. *PLoS ONE* 7(1), e24274. https://doi.org/10.1371/journal.pone.0024274 [V]
- Spivak, D. I. (2012). Functorial data migration. *Information and Computation* 217, 31–51. https://doi.org/10.1016/j.ic.2012.05.001 [V]
- Baez, J. C. and Pollard, B. S. (2017). A compositional framework for reaction networks. *Reviews in Mathematical Physics* 29(9), 1750028. https://doi.org/10.1142/S0129055X17500283 [V]
- Libkind, S., Baas, A., Halter, M., Patterson, E. and Fairbanks, J. P. (2022). An algebraic framework for structured epidemic modelling. *Philosophical Transactions of the Royal Society A* 380(2233), 20210309. https://doi.org/10.1098/rsta.2021.0309 [V]
- Patterson, E., Lynch, O. and Fairbanks, J. (2022). Categorical data structures for technical computing. *Compositionality* 4, 5. https://doi.org/10.32408/compositionality-4-5 [V]
- Fritz, T. (2020). A synthetic approach to Markov kernels, conditional independence and theorems on sufficient statistics. *Advances in Mathematics* 370, 107239. https://doi.org/10.1016/j.aim.2020.107239 [V]
- Jacobs, B., Kissinger, A. and Zanasi, F. (2019). Causal inference by string diagram surgery. *FoSSaCS 2019*, LNCS, 313–329. https://doi.org/10.1007/978-3-030-17127-8_18 [V]
- Fong, B. and Spivak, D. I. (2019). *An Invitation to Applied Category Theory: Seven Sketches in Compositionality*. Cambridge University Press. https://doi.org/10.1017/9781108668804 [V]

### 8.7 What Is Implementable and Open Problems

Implementable, in order of value: value-based argumentation over standards and ASPIC+ standing in apodeixis (frameworks exist; value orderings and the practical-reasoning scheme are additions); defeasible deontic rules for normative standards through the law domain's `norms` or a new apodeixis calculus; the needs vocabulary as a kategoria schema with archives migrated by Kan extensions; Popper-style constraints from refuted statements in the needs search; abduction over a declared access theory as a generator; ILP for learning normative rules from decisions, later and elsewhere.

Open: argumentation with probabilistic evidence has several proposals and no standard; a "category of hypotheses" with tests as morphisms has not been worked out; ILP's handling of survey weights and noise is undeveloped; defeasible deontic logic over quantitative standards (thresholds on indicators) needs arithmetic beyond its propositional core.

---

## 9. Needs Assessment

### 9.1 Theories of Human Need

- **Bradshaw's taxonomy** (1972): normative, felt, expressed and comparative need, with the observation that a need recognised under several definitions at once is the strongest case [content M]. The working taxonomy of the field, and the standards of the needs domain.
- **Doyal and Gough** (1991): basic needs are physical health and autonomy, universal because their absence causes serious harm; intermediate needs (adequate food and water, housing, a non-hazardous environment, health care, security in childhood, significant primary relationships, physical and economic security, education, safe birth control and childbearing [list M]) are universal too, while their satisfiers vary by society. Gough (2015, 2017) carries the theory to sustainable welfare: needs are satiable and non-substitutable, unlike preferences.
- **Max-Neef** (1991): nine axiological needs (subsistence, protection, affection, understanding, participation, idleness, creation, identity, freedom) crossed with four existential categories (being, having, doing, interacting) give a matrix whose cells hold *satisfiers*; satisfiers can violate, pseudo-satisfy, inhibit, satisfy singly or synergistically [content M].
- **The capability approach** (Sen 1999; Nussbaum 2000; Robeyns 2005): what matters is what people are able to be and do (capabilities and functionings), and **conversion factors** (personal, social, environmental) decide what a resource becomes for a person. *Consequence:* a standard on resources (income, a service's presence) and a standard on functionings (being able to dress, to reach a clinic) can find different needs in the same data.
- **Multidimensional counting** (Alkire and Foster 2011; Alkire et al. 2015): a unit is deprived in indicator $j$ when below cutoff $z_j$, and poor when its weighted count of deprivations is at least $k$; the adjusted headcount $M_0 = H \times A$ (incidence times intensity) decomposes by subgroup and breaks down by dimension. This is a normative standard over several indicators at once, which the needs domain's language does not yet express. Ravallion (2011) criticises composite indices for the implicit trade-offs their weights impose. Townsend (1987) defined deprivation relative to the society's customary standards; Jarman (1983) built an area index of underprivilege from general practitioners' weights (a normative standard set by experts).
- **Access and use:** the inverse care law (Tudor Hart 1971), that the availability of good care varies inversely with need, is the first reason expressed need under-counts need; Andersen's behavioural model (1995) explains use by predisposing, enabling and need factors; Levesque, Harris and Russell (2013) set five dimensions of accessibility against five abilities of persons. Together they give the structure of a seeking model (§5.3).

### 9.2 Needs Assessment in Public Health and Planning

Witkin and Altschuld (1995) treat needs assessment as discrepancy analysis in three phases (pre-assessment, assessment, post-assessment) with priorities set before solutions [S]. In public health, Wright, Williams and Wilkinson (1998) open the BMJ series on health needs assessment, which stresses that need, demand and supply overlap and must be assessed together; the series is associated with epidemiological, comparative and corporate approaches [M]. Practice combines data on prevalence and services with the views of the people concerned and of professionals, which is Bradshaw's taxonomy made procedure.

### 9.3 Computational and Data-Driven Discovery of Need

- **Small area estimation** (Fay and Herriot 1979; Rao and Molina 2015) estimates indicators for areas too small for direct survey estimates, by borrowing strength from auxiliary data; **poverty mapping** at the unit level (Elbers, Lanjouw and Lanjouw 2003) imputes welfare into a census from a survey model. peira `small_areas` holds Fay–Herriot. *Use:* the gap of a statement whose group includes a small area should be estimated by small area methods, not directly.
- **Prediction of deprivation from new data:** satellite imagery (Jean et al. 2016), mobile phone metadata (Blumenstock, Cadamuro and On 2015), and phone-based targeting of cash transfers in Togo (Aiken et al. 2022). Brown, Ravallion and van de Walle (2018) found large exclusion errors in proxy means tests, so that many of the poorest are missed. *Use:* a predicted index is an indicator with known error, not a measurement of need.
- **311 and 211 as expressed need.** Minkoff (2016) found that 311 contacts in New York reflect both neighbourhood conditions and residents' propensity to contact government; Kontokosta and Hong (2021) showed that socio-spatial disparities in complaint behaviour bias decisions driven by 311 data; O'Brien, Sampson and Winship (2015) built ecometric measures of physical disorder from 311 records, addressing reporting rates that differ by neighbourhood. Among low-income callers to a 211 helpline, a social needs screener found needs the callers had not expressed (Verdecias et al. 2021). *Consequence:* an expressed-need standard needs a model of the propensity to express.
- **Needs from text:** Timoshenko and Hauser (2019) identified customer needs from user-generated content with machine learning that filters uninformative content [content M]. Felt need stated in text is evidence only when coded (zetesis `coding`) or graded; a model's reading of it is grade C.

### 9.4 Key Papers

- Bradshaw, J. (1972). The concept of social need. *New Society* 30 (496), 640–643. [S: both this title and "A taxonomy of social need" are cited for the same pages; a separate chapter, "A taxonomy of social need", appeared in McLachlan, G. (ed.), *Problems and Progress in Medical Care*, 7th series, Oxford University Press, 1972, 71–82]
- Doyal, L. and Gough, I. (1991). *A Theory of Human Need*. Macmillan. https://doi.org/10.1007/978-1-349-21500-3 [V]
- Gough, I. (2015). Climate change and sustainable welfare: the centrality of human needs. *Cambridge Journal of Economics* 39(5), 1191–1214. https://doi.org/10.1093/cje/bev039 [V]
- Gough, I. (2017). *Heat, Greed and Human Need: Climate Change, Capitalism and Sustainable Wellbeing*. Edward Elgar. https://doi.org/10.4337/9781785365119 [V]
- Max-Neef, M. A., with Elizalde, A. and Hopenhayn, M. (1991). *Human Scale Development: Conception, Application and Further Reflections*. Apex Press, New York. ISBN 094525735X. [S]
- Sen, A. (1999). *Development as Freedom*. Oxford University Press. [S]
- Nussbaum, M. C. (2000). *Women and Human Development: The Capabilities Approach*. Cambridge University Press. https://doi.org/10.1017/CBO9780511841286 [V]
- Robeyns, I. (2005). The capability approach: a theoretical survey. *Journal of Human Development* 6(1), 93–117. https://doi.org/10.1080/146498805200034266 [V]
- Alkire, S. and Foster, J. (2011). Counting and multidimensional poverty measurement. *Journal of Public Economics* 95(7–8), 476–487. https://doi.org/10.1016/j.jpubeco.2010.11.006 [V]
- Alkire, S., Foster, J., Seth, S., Santos, M. E., Roche, J. M. and Ballon, P. (2015). *Multidimensional Poverty Measurement and Analysis*. Oxford University Press. https://doi.org/10.1093/acprof:oso/9780199689491.001.0001 [V]
- Ravallion, M. (2011). On multidimensional indices of poverty. *Journal of Economic Inequality* 9(2), 235–248. https://doi.org/10.1007/s10888-011-9173-4 [V]
- Townsend, P. (1987). Deprivation. *Journal of Social Policy* 16(2), 125–146. https://doi.org/10.1017/S0047279400020341 [V]
- Jarman, B. (1983). Identification of underprivileged areas. *BMJ* 286(6379), 1705–1709. https://doi.org/10.1136/bmj.286.6379.1705 [V]
- Tudor Hart, J. (1971). The inverse care law. *The Lancet* 297(7696), 405–412. https://doi.org/10.1016/S0140-6736(71)92410-X [V]
- Andersen, R. M. (1995). Revisiting the behavioral model and access to medical care: does it matter? *Journal of Health and Social Behavior* 36(1), 1–10. https://doi.org/10.2307/2137284 [V for DOI and first page; last page M]
- Levesque, J.-F., Harris, M. F. and Russell, G. (2013). Patient-centred access to health care: conceptualising access at the interface of health systems and populations. *International Journal for Equity in Health* 12, 18. https://doi.org/10.1186/1475-9276-12-18 [V]
- Witkin, B. R. and Altschuld, J. W. (1995). *Planning and Conducting Needs Assessments: A Practical Guide*. Sage, Thousand Oaks. ISBN 9780803958104. [S]
- Wright, J., Williams, R. and Wilkinson, J. R. (1998). Development and importance of health needs assessment. *BMJ* 316(7140), 1310–1313. https://doi.org/10.1136/bmj.316.7140.1310 [V]
- Fay, R. E. and Herriot, R. A. (1979). Estimates of income for small places: an application of James–Stein procedures to census data. *JASA* 74(366a), 269–277. https://doi.org/10.1080/01621459.1979.10482505 [V]
- Rao, J. N. K. and Molina, I. (2015). *Small Area Estimation*, 2nd ed. Wiley. https://doi.org/10.1002/9781118735855 [V]
- Elbers, C., Lanjouw, J. O. and Lanjouw, P. (2003). Micro-level estimation of poverty and inequality. *Econometrica* 71(1), 355–364. https://doi.org/10.1111/1468-0262.00399 [V]
- Jean, N., Burke, M., Xie, M., Davis, W. M., Lobell, D. B. and Ermon, S. (2016). Combining satellite imagery and machine learning to predict poverty. *Science* 353(6301), 790–794. https://doi.org/10.1126/science.aaf7894 [V]
- Blumenstock, J., Cadamuro, G. and On, R. (2015). Predicting poverty and wealth from mobile phone metadata. *Science* 350(6264), 1073–1076. https://doi.org/10.1126/science.aac4420 [V]
- Aiken, E., Bellue, S., Karlan, D., Udry, C. and Blumenstock, J. E. (2022). Machine learning and phone data can improve targeting of humanitarian aid. *Nature* 603(7903), 864–870. https://doi.org/10.1038/s41586-022-04484-9 [V]
- Brown, C., Ravallion, M. and van de Walle, D. (2018). A poor means test? Econometric targeting in Africa. *Journal of Development Economics* 134, 109–124. https://doi.org/10.1016/j.jdeveco.2018.05.004 [V]
- Minkoff, S. L. (2016). NYC 311: a tract-level analysis of citizen–government contacting in New York City. *Urban Affairs Review* 52(2), 211–246. https://doi.org/10.1177/1078087415577796 [V]
- Kontokosta, C. E. and Hong, B. (2021). Bias in smart city governance: how socio-spatial disparities in 311 complaint behavior impact the fairness of data-driven decisions. *Sustainable Cities and Society* 64, 102503. https://doi.org/10.1016/j.scs.2020.102503 [V]
- O'Brien, D. T., Sampson, R. J. and Winship, C. (2015). Ecometrics in the age of big data: measuring and assessing "broken windows" using large-scale administrative records. *Sociological Methodology* 45(1), 101–147. https://doi.org/10.1177/0081175015576601 [V]
- Verdecias, N., Garg, R., Steensma, J., McQueen, A., Greer, R. and Kreuter, M. W. (2021). Expressed and unexpressed social needs in low-income adults in the U.S. *Health & Social Care in the Community* 29(5). https://doi.org/10.1111/hsc.13265 [V; pages not given by the record]
- Timoshenko, A. and Hauser, J. R. (2019). Identifying customer needs from user-generated content. *Marketing Science* 38(1), 1–20. https://doi.org/10.1287/mksc.2018.1123 [V]

### 9.5 What Is Implementable and How It Serves the Feature

- **A counting standard.** Alkire–Foster's dual cutoff as a fifth form of normative standard over several indicators (`normative count I_1 below z_1, …, I_J below z_J at k`), with $M_0$ and its decomposition by group, in nomos `measurement` (general), read by the needs domain's language.
- **A model of the propensity to express.** An expressed-need gap (demand over provision) is biased where the propensity to request differs between groups (inverse care law, 311 disparities). peira's `detection` (occupancy and N-mixture models: detection with probability below one) is a direct analogue: a need present but not reported is an undetected occupant. Alternatively, an ecometric model with area-level reporting propensities. Either turns the expressed standard from a count into an estimate with its reporting model declared.
- **Small area gaps** by peira's Fay–Herriot when a group is or includes a small area.
- **Bradshaw's combinations** as a derived standing: a group with statements robust under several standards (normative and felt, say) for the same domain is marked so, which is the strongest case in Bradshaw's own reading.
- **Capabilities as standards on functionings**, with conversion factors as conditions of the group: no new method, a discipline of the vocabulary.

### 9.6 Open Problems

- There is no agreed rule for combining Bradshaw's four standards, nor for resolving their conflicts; §8.3's value-based frameworks make conflicts explicit without resolving them.
- Data-driven needs discovery is rarely evaluated against an independent assessment of need; most studies predict an index (poverty, deprivation) rather than test statements of need.
- Expressed-need data (requests, complaints, waiting lists) are shaped by the services that record them; their bias is structural, not noise.
- Predictive targeting trades legitimacy for accuracy; a need found by a model that people cannot inspect is hard to defend as a standard.

---

## 10. A Design for the Search, Drawn from the Literature

The following composes the sections above into one loop. It is a proposal for the needs domain's design, not a description of what exists.

1. **Three parts of the data, by a declared key** (§3). *Discovery*: seen freely by the search. *Validation*: consulted by the search only through a reusable-holdout mechanism (Thresholdout or the Ladder) with a recorded budget, for example to decide which statements join the pool of parents. *Confirmation*: never seen by the search; each statement is confirmed once, by a test written into peira's `protocol` and sealed in archeion's journal before it is run.
2. **Generators as arms of a bandit** (§7.6), credited on discovery-side quality only: enumeration of the language; subgroup discovery with optimistic estimates, DSSD diversity and a swap-randomisation threshold (§4.5); PRIM (§6); GATES/CLAN and policy trees when a provision is declared (§4.4); derivation from defeasible normative rules (§8.4); zones of unmet demand from a history-matched seeking model (§5.3); abduction over an access theory (§8.2); a language model given OPRO's meta-prompt of discovery-scored statements (§7.7), its proposals of grade C.
3. **Evaluation on discovery** by several objectives: the standardised gap and Klösgen quality; coverage; per-stratum gaps for lexicase; satisficing robustness over a declared ensemble of analysis choices (§6.4); simplicity; novelty by weighted Jaccard distance; causal support where declared, with its robustness value (§4.2).
4. **Selection and survival** (§7.3–7.5): ε-lexicase over strata, or NSGA-II over the objectives; a MAP-Elites archive over (domain, kind of standard, coverage band, size); ALPS layers for newcomers and aging for retirement from breeding.
5. **Coevolution** (§7.2): a population of adverse strata of the discovery data, evolved to break current statements; Bongard–Lipson's disagreement criterion to choose the most informative strata.
6. **Admission to the archive** (§3): the confirmation p-value or e-value of each statement, in order of admission, decided by an online procedure (SAFFRON or LORD++ under independence; e-LOND under arbitrary dependence, which overlapping groups on one confirmation set make the safer default). Grading by a declared scheme in tekmerion's `warrant` (§2.4); standing among conflicting standards by value-based argumentation in apodeixis (§8.3).
7. **Retirement of two kinds** (§3.4, §7.5): *search retirement* by age or inefficacy, which removes a statement from breeding and leaves its standing; *evidential revocation* by an e-process on new waves of data crossing $1/\alpha$ for the hypothesis that the gap has closed, which removes it from the archive with the evidence. Both are recorded with their reasons.
8. **Action** (§2, §6): EVSI to choose which supported statements deserve new data; MCDA to rank robust needs; transport or Cartwright–Hardie arguments to claim them elsewhere; policy trees and RDM to choose provision robustly.

---

## 11. Where the Methods Would Go, Ranked by Value

Ranked by value to the needs feature: how much each strengthens the guarantee that an archived statement is a real need, weighed against effort. "New" means a module or file not present on 2026-11-18.

| Rank | Method | Where | Serves | Effort |
|---|---|---|---|---|
| 1 | Online FDR (LORD++, SAFFRON, α-investing, asynchronous) for admissions in order | tyche `testing`, new `online.hpp` | archive; guard | small |
| 2 | E-values and e-processes: calibrators, e-BH, e-LOND, betting confidence sequences for a mean and a difference | tyche `testing`, new `evalues.hpp` | archive under dependence; evidential retirement | small–medium |
| 3 | Reusable holdout (Thresholdout, Ladder) with budget accounting; three-way split protocol | tyche `testing`, new `holdout.hpp`; the split and its record in the needs domain with peira `protocol` and archeion's journal | guard against confirmation leaking into the search | small |
| 4 | Subgroup discovery and exceptional model mining: quality families, optimistic estimates, beam and exhaustive search, DSSD, swap-randomisation thresholds, Webb's layered critical values; PRIM in the same module | new tyche module (`patterns`), the description language moved down from the needs domain's `groups`; reused by the policy domain for scenario discovery | generation; discovery-side significance | medium |
| 5 | Heterogeneous effects: honest causal trees and forests, DR- and R-learners with cross-fitting, GenericML (BLP, GATES, CLAN), exhaustive policy trees | peira, new module (`heterogeneity`), on its `weighting`, `graphs` and `sensitivity` | causal support; generation of groups where provision helps | medium–large |
| 6 | Lexicase over strata, QD archive with Jaccard novelty, ALPS beside aging, discounted bandit credit | heuresis `selection`, `diversity`, `evolution` (mostly existing); descriptors and cases from the needs domain | selection; diversity of the archive | small |
| 7 | Coevolution of statements and adverse strata (fitness sharing, hall of fame, Pareto archive, Bongard–Lipson test choice, minimax two-space) | heuresis `coevolution` (planned) | selection; robustness before confirmation | medium |
| 8 | A language model as a proposer through heuresis's external-proposer seam and `science.models`, with an OPRO-style prompt contract and no access to confirmation | needs domain (prompt contract, reader), API | generation | small |
| 9 | Grading scheme for needs (GRADE's factors read for statements) and pooling across strata | tekmerion `warrant` (a declared scheme as data), peira `synthesis` | standing; transparency | small |
| 10 | Expressed-need correction by a reporting model (detection or ecometric) and small area gaps | peira `detection`, `small_areas` (existing), composed in the needs domain | testing of expressed and small-area statements | medium |
| 11 | Alkire–Foster counting standard | nomos `measurement` | a fifth form of normative standard | small |
| 12 | ABC (rejection, regression adjustment, SMC with model choice), simulation-based calibration, history matching with emulators | tyche `bayes` and `uncertainty`; seeking model on nomos `agents` | the simulation test made meaningful | medium–large |
| 13 | Value-based argumentation and the practical-reasoning scheme over standards | apodeixis `argumentation` (extension) | standing among conflicting standards | small–medium |
| 14 | Defeasible deontic rules with reparation for normative standards | law domain `norms`, or a new apodeixis calculus | rule-derived normative needs | medium |
| 15 | The needs vocabulary as a kategoria schema; archives migrated by Kan extensions | kategoria (existing), a `.cat` schema in the needs domain | revisability of the archive | small |
| 16 | EVSI per statement for new data collection; robustness metrics over scenarios | tyche `decision`, policy domain `adoption` and `options` | action; choosing what to confirm next | small |
| 17 | Robustness over analysis choices (satisficing over thresholds, weights, codings) | needs domain, on heuresis's evaluation of scenarios | evaluation | small |
| 18 | Popper-style constraints from refuted statements; abduction over an access theory | needs domain (constraints); apodeixis (abduction, later) | generation efficiency | medium |
| 19 | Causal discovery (PC, FCI) as a proposer of graphs, graded as proposals | peira `graphs` | optional support for declared graphs | medium |
| 20 | ILP for normative rules learnt from decisions | apodeixis, later | rules for law and policy rather than needs directly | large |

---

## 12. What Could Not Be Verified, and Corrections to the Theory Note

### 12.1 Unverified or Partly Verified Items

- **Bradshaw (1972):** two titles are cited for *New Society* 30, 640–643 ("The concept of social need" and "A taxonomy of social need"), and a chapter "A taxonomy of social need" appeared the same year in a volume edited by McLachlan (71–82). Only secondary sources were read. [S]
- **Klösgen (1996):** the chapter exists in Fayyad et al. (eds), AAAI/MIT Press; the page range 249–271 used in the theory note was not confirmed.
- **Max-Neef (1991), Sen (1999), Witkin and Altschuld (1995):** confirmed from catalogues and publishers' listings only. [S]
- **Stonedahl and Wilensky (2010)** on the Artificial Anasazi model: the paper exists (hosted by Northwestern's Center for Connected Learning); its venue (an AAAI Fall Symposium) is from prior knowledge.
- **AlphaEvolve:** no peer-reviewed version was found; it is an arXiv white paper.
- **Si, Yang and Hashimoto (2024):** ICLR 2025 acceptance seen only on an aggregator. **Kıcıman et al. (2023):** journal status not confirmed. **OMNI, QDAIF (ICLR 2024), ReEvo (NeurIPS 2024), Yang et al. (Findings of ACL 2024):** venues from prior knowledge.
- **Content marked [M]** in the text: Bradshaw's remark on combinations, Doyal and Gough's list of intermediate needs, Max-Neef's matrix and satisfier types, the epidemiological, comparative and corporate approaches, Steegen et al.'s multiverse analysis, Yekutieli's tree FDR, the `sbi` package, and the contents of Huang et al. (2023), Zheng et al. (2023) and Timoshenko and Hauser (2019).
- **POPPER's mechanism:** that p-values are converted to e-values and aggregated sequentially is from secondary summaries; the abstract says only that a sequential testing framework controls type-I error.
- **Lemmerich et al. on exceptional mediation models** was seen in a search result only; no causal-effect variant of EMM was found.

### 12.2 Two Points Where the Theory Note's Safeguards Fall Short

The theory note's §5 is right in its three safeguards; the literature sharpens two of them.

1. **Benjamini–Hochberg over "all statements ever admitted" is an offline procedure applied to an online process.** Re-running BH as the family grows can withdraw robust labels already given, and BH's guarantee needs independence or positive dependence, while the confirmation tests of overlapping groups on one confirmation set are dependent in unknown ways. An online procedure (LORD++ or SAFFRON) fits the order of admission; e-LOND or e-BH removes the dependence assumption (§3.1, §3.4).
2. **The confirmation part leaks if confirmed statements breed.** The note says the confirmation part is "never to guide the search", and also that kept statements "take part in later generations as parents". If admission to the archive depends on confirmation, and archived statements are parents, the confirmation outcomes steer the search, which is the adaptive reuse that Dwork et al. (2015) show invalidates held-out tests. Either parents must be chosen by discovery-side criteria alone (confirmation outcomes hidden from the search), or admission to the breeding pool must be decided on a separate validation part queried through a reusable-holdout mechanism, keeping the confirmation part fresh (§10, item 1). The same applies to operator credit (§7.6).

A third point is a refinement: stability under resampling (the note's third safeguard) gives a guarantee only with a calibrated threshold and stability selection's conditions; without them it is a useful screen, not a control of error (§3.1).
