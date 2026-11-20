# Fitness Evaluation from Heterogeneous Evaluators in Evolutionary Allocation

Scope: how the fitness of a candidate allocation or policy can come from evaluators of very different kinds (closed forms, input–output and CGE models, system dynamics, agent-based models, machine-learned surrogates, language models, people), and how a search can use them together. These notes were written for the design of a universal evolutionary resource-allocation framework with pluggable evaluators (in this repository, on heuresis).

Markers: **[V]** verified by reading the source itself during this research (where only the abstract page was read, this is said); **[S]** from a search snippet or a secondary summary only; **[M]** from memory and not checked here. DOIs or arXiv identifiers are given where known; a DOI marked [M] has not been checked.

Research date: 2026-11-20. About 45 tool calls; the full texts of AlphaEvolve (first 100k characters), FunSearch (PMC), Lamperti et al. (ar5iv), Lee et al. (JASSS) and the Rakshit–Konar–Das survey (author's manuscript, extracted locally) were read; the rest from abstract pages or snippets.

---

## Q1. The Interface an Evaluator Should Expose (Value, Uncertainty, Cost, Feasibility, Descriptors, Provenance)

### Takeaway
Every mature system reviewed converges on more than a scalar. FunSearch scores per input and aggregates, discarding programs that fail limits; AlphaEvolve's `evaluate` returns a *dictionary* of scalars, supports staged (cascaded) evaluation and adds LLM-graded properties as extra keys; Ax records a mean *and* a standard error per metric, with SEM 0 meaning noiseless and a missing SEM meaning "infer the noise". heuresis's own `Evaluation` already holds objectives, constraints, descriptors, cost, a failure status and its reason, but has no field for uncertainty, fidelity, the random stream used, or provenance. Those are the gaps a pluggable-evaluator framework must fill.

### Cited Findings

**Program-search systems (the LLM generates, a program scores)**
- FunSearch's evaluator is a function `evaluate` that "takes as input a candidate solution to the problem, and returns a score assessing it"; programs are run on several inputs (e.g. several cap-set dimensions n, several bin-packing instances) and per-input scores are combined by an aggregation function "such as the mean" [V] — [Romera-Paredes et al. 2024, Nature 625:468–475, DOI 10.1038/s41586-023-06924-6 (PMC10794145)](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/).
- FunSearch discards programs that fail to execute within "the imposed time and memory limits" or produce invalid outputs; it executes code in "a sandbox for safely executing generated code"; typically 15 LLM samplers and 150 CPU evaluators; about 10⁶ samples per result [V] — [same](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/).
- FunSearch clusters programs within an island by their per-input score *signature* (the vector of scores, not the aggregate), and every 4 hours discards the worse half of the islands and reseeds them from survivors [V] — [same](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/). The score vector thus doubles as a behavioural descriptor.
- FunSearch's authors name three properties of problems it suits: "availability of an efficient evaluator", "'rich' scoring feedback quantifying the improvements", and a skeleton isolating the part to evolve; proof generation is out of scope "because it is unclear how to provide a rich enough scoring signal" [V] — [same](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/).
- AlphaEvolve's evaluator is "typically implemented as a Python function, called evaluate, with a fixed input/output signature, returning a dictionary of scalars" [V] — [Novikov et al. 2025, arXiv:2506.13131](https://arxiv.org/html/2506.13131v1).
- AlphaEvolve supports an *evaluation cascade*: users supply test cases of increasing difficulty, and a candidate is "evaluated on the next stage only if [it achieves] sufficiently promising results" on earlier stages [V] — [same](https://arxiv.org/html/2506.13131v1).
- AlphaEvolve: properties hard to code (e.g. simplicity) "can be graded using separate LLM calls and added to the dictionary of scores to steer evolution"; evaluation is often "embarrassingly parallel" and distributed over a cluster [V] — [same](https://arxiv.org/html/2506.13131v1).
- AlphaEvolve's pipeline is asynchronous (asyncio: controller, LLM samplers, evaluation nodes), "optimized for throughput (rather than the speed of any one particular computation)" [V] — [same](https://arxiv.org/html/2506.13131v1).

**Adaptive-experimentation and optimisation libraries**
- Ax stores per-metric observations with columns `metric_name`, `arm_name`, `mean`, `sem` (and `step` for curves); if only a mean is passed, Ax assumes noisy data (additive zero-mean Gaussian) and infers the noise; passing SEM = 0.0 marks an observation noiseless [S] — [Ax documentation and source (0.5.0, 1.2–1.3)](https://ax.readthedocs.io/en/stable/service.html), [ax.metrics.noisy_function source](https://ax.readthedocs.io/en/0.5.0/_modules/ax/metrics/noisy_function.html).
- pymoo's `Problem._evaluate(x, out)` fills `out["F"]` (objectives), `out["G"]` (inequality constraints) and `out["H"]` (equality constraints) [M]; Optuna's trials report intermediate values so that a pruner (successive halving / Hyperband, Li et al. 2018, JMLR 18(185)) can stop poor trials early [M]; Nevergrad's ask–tell interface accepts a loss and constraint violations [M].

**heuresis (this repository's search foundation)**
- heuresis's `search::Evaluation` holds `objectives` (minimised), `constraints` (satisfied when ≤ 0), `descriptors` ("a behaviour, for diversity"), `cost` ("in the unit of the budget": evaluations, seconds or core-hours), `status` (ok / failed) and `reason` ("why it failed"); a failure "is worse than any evaluation that did not fail; it is never answered with a made-up value"; single-objective order is Deb's feasibility rule (Deb 2000, Comput. Methods Appl. Mech. Engrg. 186:311–338) [V] — `foundations/heuresis/search/include/heuresis/search/evaluation.hpp` (read locally).
- heuresis's CLAUDE.md lists a `noise` module as *planned*, and assigns "evaluators on clusters or devices, transports between islands, proposers that call a language model" to the infrastructure's edges and works, not to heuresis [V] — `foundations/heuresis/CLAUDE.md` (read locally).

**Surrogate literature (what a model-managed evaluator must report)**
- Jin et al. (2019) frame data-driven evolutionary optimisation as the case where evaluations are costly simulations or physical experiments and, in the extreme, "only historical data" exists with no new data obtainable during the search; model-management strategies differ by whether new data can be acquired [S] — [Jin, Wang, Chugh, Guo, Miettinen 2019, IEEE TEVC 23(3):442–458, DOI 10.1109/TEVC.2018.2869001](https://ore.exeter.ac.uk/repository/handle/10871/34011).
- Infill criteria used to decide which candidates go to the true evaluator (expected improvement, lower confidence bound) require the surrogate to report a predictive variance, as Kriging/GP does [M] — Jones, Schonlau, Welch 1998, J. Global Optim. 13:455–492 (EGO); Jin 2011, Swarm Evol. Comput. 1(2):61–70, DOI 10.1016/j.swevo.2011.05.001 [M].

### Inferences
- The minimal common record is: a vector of named metrics, each with a point value *and* an uncertainty (SEM or the raw replicates); constraint values; descriptors; the cost actually spent; a status with reason; and enough provenance to replay (evaluator identity and version, fidelity, seed or random-stream key, input digest). FunSearch's signature clustering and AlphaEvolve's dictionary show that keeping *per-case* and *per-property* scores, not just an aggregate, pays for itself in diversity.
- Two evaluation-control hooks recur and should be part of the interface rather than of each evaluator: (i) staged evaluation (a cascade, or Optuna-style intermediate reports enabling early stopping), and (ii) the ability to ask an evaluator for *more replicates* of an already-evaluated candidate (needed by resampling and racing, Q2).
- Ax's convention (SEM missing ⇒ unknown noise, SEM = 0 ⇒ deterministic) is a clean way to let closed forms and stochastic simulators share one record.

### Gaps
- No standard serial schema for evaluation records across EC frameworks was found (heuresis's own open decision 4, "a serial format for records", is still open). pymoo/Optuna/Nevergrad details above are [M].
- AlphaEvolve's second half (73k characters) was not read; whether it reports metrics' uncertainty or repeated evaluation was not checked.

---

## Q2. Replications per Candidate for Stochastic Simulations and ABMs, and Cost-Reducing Techniques with Known Guarantees

### Takeaway
There is no universal replication count: ABM practice commonly uses ≤ 100 runs per setting, a power analysis for a medium effect (d = 0.5, α = 0.05, power 0.8) asks for about 64–78 runs per arm, and stabilisation criteria can ask for 1,000. Inside an EA, *adaptive* allocation beats fixed allocation: dynamic resampling to a standard-error threshold, OCBA (asymptotically optimal probability of correct selection under normality), racing with Hoeffding or empirical-Bernstein bounds (a 1 − δ guarantee per comparison), and CMA-ES's rank-change uncertainty handling, which needs only a few extra evaluations. Common random numbers help when noise is positively correlated across candidates and can hurt otherwise. Machine-learned surrogates of ABMs and CGE models cut cost by 2–4 orders of magnitude but carry no guarantee beyond out-of-sample validation.

### Cited Findings

**Economic models as evaluators (input–output, SAM, CGE, system dynamics)**
- A MOEA was linked to a GAMS CGE model of Kenya (JRC DEMETRA, MCP formulation, on STAGE_DEV) to *calibrate* its elasticities so that its responses to AfCFTA trade shocks (2021–2035) match the global MAGNET/GTAP model; the response errors are separate objectives, motivated because "it is not straightforward to weigh GDP gains against welfare gains"; the authors state MOEAs "have not yet been implemented in the field of economy-wide/CGE modelling" (as of 2022) [V] — [extended abstract, "CGE model calibration using multi-objective evolutionary algorithms", 25th Annual Conference on Global Economic Analysis, 2022 (authors not shown in the extracted text)](https://www.gtap.agecon.purdue.edu/resources/res_download.asp?D_ID=11031).
- Britz and Storm train a deep-learning surrogate of a detailed recursive-dynamic single-country CGE model (Tanzania, 2018–2050) and optimise the timing of abatement and carbon-tax recycling on it; average R² across outputs 99.99%; "the surrogate model enables policy optimization not possible by using the CGE model directly"; generating the training sample is "computing-time intensive" [V, abstract page] — [Britz & Storm 2026, J. Global Econ. Analysis 11(1):49–85, DOI 10.21642/JGEA.110102AF](https://jgea.org/ojs/index.php/jgea/article/view/334).
- Brooks (2000) argued that GAs suit policy problems without closed-form solutions and can "decide" between alternative optima [S] — [J. Public Economic Theory 2(4):493–513](https://ideas.repec.org/a/bla/jpbect/v2y2000i4p493-513.html); Cicowiez, Decaluwé and Nabli (2017) use the CGE model as the constraint set of an optimisation (Argentina, PEP-1-1, e.g. minimise unemployment) [S] — [PEP slides](https://cepal.org/sites/default/files/events/files/cicowiez_decaluwe_y_nabli_-_partnership_for_economic_policy_0.pdf); a 2026 preprint uses Bayesian optimisation on the equilibrium manifold to handle multiple equilibria in policy optimisation [S] — [arXiv:2606.29299](https://arxiv.org/pdf/2606.29299).
- Input–output models with NSGA-II: Okola (2022) couples input–output theory with MATLAB's NSGA-II-based `gamultiobj` for a food–energy–water nexus [S] — [SCITEPRESS 2022](https://www.scitepress.org/Papers/2022/112715/112715.pdf); a MOEA ranks Sinaloa's (Mexico) economic sectors on an ELECTRE III outranking relation built on IO data [S] — [Redalyc](https://www.redalyc.org/pdf/1250/125043459005.pdf). Many environmentally extended IO studies instead solve multi-objective *linear* programmes (e.g. 487 sectors, 16 to regulate first) [S] — [AIChE 2015](https://aiche.org/conferences/aiche-annual-meeting/2015/proceeding/paper/692c-multi-objective-optimization-combined-input-output-and-eco-cost-assessment-decarbonizing). IO multipliers are linear and cheap, so exact LP/MOLP is often available and EAs are needed only once discrete or non-linear choices enter (inference).
- System dynamics: GAs were proposed to find SD policies automatically (Sholtes 1994, Albany thesis); Grossmann (2002) coupled Stella models with GAs and noted the GA "may need to run the model several thousand times", so fast code matters; Duggan's work produces non-dominated policy sets; Chen and Jeng (2004) recast SD policy design as GA training of a network-like structure [S] — [Albany SD archives](https://archives.albany.edu/concern/daos/dz0117801?locale=fr), [Duggan, Equation-based policy optimization for agent-oriented SD models](https://research.universityofgalway.ie/en/publications/equation-based-policy-optimization-for-agent-oriented-system-dyna-4/); a 2024 *Simulation* paper does multi-objective SD policy optimisation with GAs [S] — [Crossref 10.1177/00375497241301077](https://api.crossref.org/works/10.1177%2F00375497241301077).

**ABMs in the loop: optimisation, calibration, cost and replications**
- Oremland and Laubenbacher propose model reduction plus heuristic (Pareto) optimisation of ABMs, treating stochasticity as central, on two representative ABMs [S] — [JASSS 17(2):6, 2014](https://www.jasss.org/17/2/6.html); a standard PSO calibrated an influenza ABM (Miami) and outperformed Monte Carlo search [S] — [Michels, Kang, Wang, JASSS 25(2), 2022](https://ideas.repec.org/a/jas/jasssj/2013-88-2.html) (snippet's citation; record id mismatched, check before citing); Koopman-operator surrogates for multi-objective optimisation of agent-based systems [S] — [arXiv:2306.17666](https://arxiv.org/pdf/2306.17666).
- Lamperti, Roventini and Sani calibrate ABMs with an XGBoost surrogate trained by active sampling: a large Sobol pool of parameter vectors, a small seed labelled by the true ABM, and each round the surrogate labels the pool and a random subset of *predicted positives* is run in the ABM (entropy sampling when none are predicted), about log(budget) points a round [V] — [Lamperti, Roventini, Sani, arXiv:1703.10639 (JEDC 2018, 90:366–389, DOI 10.1016/j.jedc.2018.03.011 [M])](https://ar5iv.arxiv.org/html/1703.10639).
- Their calibration criterion is a statistical test, not a distance: Brock–Hommes is "positive" when a two-sample Kolmogorov–Smirnov test against S&P 500 returns has p > 0.05; the Island model when average growth > 2% and the exponential-power shape b ≤ 1 (fat tails) [V] — [same](https://ar5iv.arxiv.org/html/1703.10639).
- Results: the surrogate predicts "500X faster on average" (Brock–Hommes, 12 parameters) and "3750 times faster" (Island, 8 parameters); budgets 250–2,500 true evaluations; at 2,500, binary F1 ≈ 0.80 and ≈ 75% of positives found (Brock–Hommes), ≈ 90% of positives in the real-valued Island setting (MSE < 0.08); with 500 evaluations and 100 Monte Carlo runs per parameter vector, precision of predicted positives 94.72% (XGBoost), 99.35% with Platt scaling [V] — [same](https://ar5iv.arxiv.org/html/1703.10639).
- Lee et al.'s review of ABM output analysis: "sample sizes of 100 or less are common"; Lorscheid et al.'s (2012) coefficient-of-variation criterion picks the run count where successive CVs change by less than E and stay so (their example E = 0.01 gives n = 1,000), but the authors "urge some caution in using c_V" when outcome means are near 0 [V] — [Lee et al. 2015, JASSS 18(4):4](https://www.jasss.org/18/4/4.html).
- Lee et al.'s power analysis: n_min ≥ 2(s²/δ)(t_{ν,1−α/2} + t_{ν,1−β/2})², solved iteratively; for effect size 0.5, α = 0.05, power 0.8 the theoretical/empirical/Wilcoxon n_min were 64/65/68 (normal), 64/59/78 (exponential), 65/61/64 (Poisson), 64/65/70 (their Birth-Rate ABM); at d = 1.0, 18/19/26; the parametric formula "overestimates" n for skewed outputs and they recommend empirical power or the "more conservative Wilcoxon rank sum test" [V] — [same](https://www.jasss.org/18/4/4.html).
- Lee et al. warn the other way too: "A large sample size can easily classify a minute difference as being significant"; and output may be multimodal (separate attractors) so that it "cannot be sensibly described by merely its mean and variance"; output variance may differ across parameter settings, so run counts must be set for all settings of interest [V] — [same](https://www.jasss.org/18/4/4.html).

**Noisy fitness in EAs: resampling, racing, statistical selection**
- Rakshit, Konar and Das organise noisy evolutionary optimisation into five strategies: (i) explicit averaging by sampling, (ii) effective fitness estimation from noisy samples, (iii) implicit averaging by dynamic population sizing, (iv) improved search strategies, (v) robust selection [V] — [Rakshit, Konar, Das 2017, Swarm Evol. Comput. 33:18–45, DOI 10.1016/j.swevo.2016.09.002 (author's manuscript)](https://next.openmole.org/paper/rakshit2016.pdf).
- Static sampling gives every candidate the same sample size and cannot handle the trade-off between accuracy and run time; dynamic sampling allocates by fitness variance, periodicity, number of objectives, problem characteristics, or *comparison* between competitors (resample when two means are close) [V] — [same](https://next.openmole.org/paper/rakshit2016.pdf).
- Standard Error Dynamic Resampling (SEDR) starts each candidate at n_min and adds one sample at a time until the standard error of its mean falls below a threshold se_th (two parameters: n_min, se_th) [V] — [same](https://next.openmole.org/paper/rakshit2016.pdf).
- OCBA allocates a fixed computing budget T among candidates to maximise an approximate probability of correct selection; the survey classes it as "sequential, truly multi-objective, fitness variance dependent, comparative" sampling, with an OCBA-m variant for selecting the best m [V] — [same](https://next.openmole.org/paper/rakshit2016.pdf). (OCBA's original source: Chen et al. 2000, Discrete Event Dyn. Syst. [M].)
- On sample size vs population size: "a large sample size with a small population may fail to preserve quality solutions ... due to lack of explorative efficiency"; with n × NP held constant, implicit averaging "occasionally outperforms" explicit averaging; implicit averaging's costs are overhead, redundant re-evaluation, and dependence on a selection that noise can deceive [V] — [same](https://next.openmole.org/paper/rakshit2016.pdf).
- Robust selection includes statistical hypothesis tests between pairs, probabilistic selection, and Fieldsend and Everson's *rolling tide* multi-objective EA (IEEE TEVC); the survey's guidance is statistical-test selection on unimodal and probabilistic selection on multimodal landscapes [V] — [same](https://next.openmole.org/paper/rakshit2016.pdf).
- UH-CMA-ES (Hansen, Niederberger, Guzzella, Koumoutsakos 2009) separates uncertainty *measurement* (by rank changes among population members when some are re-evaluated) from *treatment*; it is "independent of the uncertainty distribution, prevents premature convergence ... and is well suited for online optimization as it requires only a small number of additional function evaluations"; applied to online tuning of feedback controllers on a combustion rig [S] — [IEEE TEVC 13(1):180–197, 2009, DOI 10.1109/TEVC.2008.924423 [M]](https://researchportal.ip-paris.fr/en/publications/a-method-for-handling-uncertainty-in-evolutionary-optimization-wi/). The treatments (increase evaluation effort or increase the step size σ when uncertainty is detected) are [M].
- Heidrich-Meisner and Igel add Hoeffding and empirical-Bernstein *races* to CMA-ES for direct policy search: the number of episodes per policy is adjusted individually and a race ends when the ranking needed for selection is established; the bounds give, with probability ≥ 1 − δ, the deviation between true and empirical fitness [S] — [ICML 2009, pp. 401–408, DOI 10.1145/1553374.1553426](https://icml.cc/imls/conferences/2009/papers/229.pdf). A preference-based racing variant uses only pairwise comparisons between rollouts [S] — [Busa-Fekete et al. 2014, Machine Learning, DOI 10.1007/s10994-014-5458-8](https://link.springer.com/doi/10.1007/s10994-014-5458-8).
- Racing is also the basis of algorithm configuration (F-Race, Birattari et al. 2002; irace, López-Ibáñez et al. 2016, Oper. Res. Perspect. 3:43–58, DOI 10.1016/j.orp.2016.09.002) [M]; indifference-zone ranking-and-selection procedures (e.g. Kim and Nelson 2001) guarantee probability of correct selection ≥ 1 − α when the best exceeds the rest by at least δ [M]; Branke, Chick and Schmidt (2007, Management Science 53(12):1916–1932) compare such procedures empirically [M].
- A simulation-based example joining a MOEA and budget allocation: NSGA-II with multi-objective computing budget allocation (MOCBA) over a discrete-event simulation of a Taiwanese emergency department, trading length of stay against resource cost [S] — [Health Care Manag. Sci. 20(1), 2017, DOI 10.1007/s10729-015-9335-1](https://ideas.repec.org/a/kap/hcarem/v20y2017i1d10.1007_s10729-015-9335-1.html).

**Common random numbers**
- An INRIA study of CRN and stratified sampling in noisy optimisation found CRN "very efficient" for some artificial noise models and detrimental for others; on a unit-commitment problem CRN gave most of the improvement while stratification was never harmful; CRN is particularly relevant to population-based methods [S] — [core.ac.uk PDF (title not retrieved, 403)](https://core.ac.uk/download/49487579.pdf).
- RACE-CMA (2026) combines two-stage racing with CRN seeds in CMA-ES so that every candidate sees the same perturbations, reducing rank errors [S] — [arXiv:2604.05792](https://arxiv.org/pdf/2604.05792); Bayesian optimisation with CRN via the knowledge gradient gives "significant performance gains with moderate added computational cost" [S] — [Pearce, Poloczek, Branke, Operations Research, DOI 10.1287/opre.2021.2208](https://pubsonline.informs.org/doi/fpi/10.1287/opre.2021.2208); CRN speeds finite-difference stochastic approximation from about k^−1/3 to k^−1/2 [S] — [Kleinman, Spall, Naiman, Management Science 45(11):1570 (1999; the snippet said 1997)](https://pubsonline.informs.org/doi/fpi/10.1287/mnsc.45.11.1570).

**Bayesian optimisation and multi-fidelity as the limit of surrogate assistance**
- When every evaluation is very expensive, surrogate-assisted EAs shade into Bayesian optimisation (GP posterior plus acquisition), which has regret guarantees for GP-UCB under a GP prior (Srinivas et al. 2010, ICML) [M]; multi-fidelity co-kriging (Kennedy and O'Hagan 2000, Biometrika 87(1):1–13) links cheap and expensive codes [M]; reviews: Shahriari et al. 2016, Proc. IEEE 104(1):148–175, DOI 10.1109/JPROC.2015.2494218 [M].

### Inferences
- For an ABM or stochastic economic simulator the replication count should be set *per comparison*, not per candidate: what matters is whether selection can tell candidates apart. Power analysis (Lee et al.) gives a starting n for a declared smallest meaningful difference δ; racing or SEDR then spends more only where ranks are in doubt. A fixed large n wastes budget and, with very large n, makes trivial differences "significant".
- Guarantees are of three kinds: PAC-style per race (Hoeffding/Bernstein, distribution-free but loose; Bernstein tighter when variance is small); probability of correct selection under normality and indifference zones (OCBA asymptotically, KN exactly); and none (surrogates, CRN). A framework should label each technique with which guarantee it carries.
- Lamperti et al. show that when the criterion is a *test* (KS p-value) the evaluator's output is itself uncertain and binary; a surrogate predicting it should be calibrated (Platt scaling raised precision from 94.7% to 99.4%).
- Multimodal ABM output means the evaluator should be able to return the replicate distribution, not just mean and SEM, so that quantiles or attractor shares can be objectives.

### Gaps
- No study was found that measures replication counts specifically for ABM-in-the-loop *evolutionary* policy optimisation (as opposed to calibration or one-off experiments).
- The UH-CMA-ES full text was not reachable (HAL blocked by bot protection); its treatment rules are [M].
- The INRIA CRN paper's title and authors were not retrieved.
- Evidence for EAs over CGE or SAM-multiplier evaluators is thin; the Kenya work is an extended abstract and did not report evaluation counts or run time in the part read.

---

## Q3. Reliability of Language Models as Evaluators of Solutions, and Safeguards

### Takeaway
The strongest results from LLM-driven evolution (FunSearch, AlphaEvolve, EoH, LLaMEA) all keep the LLM as *generator* and let a programmatic evaluator decide; AlphaEvolve uses LLM grades only for properties hard to code, as extra scores beside the programmatic ones, and its authors say LLM-provided evaluation is "not a setting we have optimized for". As judges, strong LLMs agree with human preferences at roughly human–human levels on chat tasks (> 80%, Zheng et al. 2023), but with position, verbosity and self-preference biases, large variation across tasks and datasets (JUDGE-BENCH), and vulnerability to universal adversarial phrases that can push absolute scores to the maximum, a direct risk under evolutionary optimisation pressure (Goodhart; Gao et al. on proxy over-optimisation). Safeguards with evidence: order swapping and aggregation, comparative rather than absolute scoring, a judge from a different model family, selective evaluation with escalation and a provable agreement guarantee, and prediction-powered inference to combine few human labels with many model scores.

### Cited Findings

**LLM generates, a program scores**
- FunSearch's LLM (Codey, on PaLM 2, frozen) is "the creative core" but "should instead be seen as a source of diverse (syntactically correct) programs"; the evaluator guards against incorrect ideas [V] — [Romera-Paredes et al. 2024](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/).
- AlphaEvolve's main limitation is that it needs automated evaluation; "it puts tasks that require manual experimentation out of our scope"; LLM-provided evaluation is possible but "not a setting we have optimized for" [V] — [Novikov et al. 2025](https://arxiv.org/html/2506.13131v1).
- AlphaEvolve reports a procedure for 4×4 complex matrices with 48 scalar multiplications, the first improvement over Strassen's algorithm in this setting in 56 years, plus data-centre scheduling and hardware-circuit improvements at Google [V, abstract] — [arXiv:2506.13131](https://arxiv.org/abs/2506.13131).
- AlphaEvolve: "optimizing for multiple metrics often improves results for the single target metric", attributed to more diverse high-performing programs in the prompts [V] — [same](https://arxiv.org/html/2506.13131v1).
- EoH evolves heuristics as natural-language "thoughts" and code together; it reports beating handcrafted heuristics and FunSearch on three combinatorial benchmarks, and a common human-designed online bin-packing baseline with a low LLM-query budget [V, abstract] — [Liu et al. 2024, arXiv:2401.02051 (ICML 2024 [M])](https://arxiv.org/abs/2401.02051).
- LLaMEA "iteratively generates, mutates and selects algorithms based on performance metrics and feedback from runtime evaluations"; its metaheuristics outperform CMA-ES and DE on 5-D BBOB and stay competitive at 10-D and 20-D [V, abstract] — [van Stein & Bäck, IEEE TEVC (accepted), arXiv:2405.20132](https://arxiv.org/abs/2405.20132).
- ReEvo (Ye et al., NeurIPS 2024, arXiv:2402.01145), EvoPrompt (Guo et al., ICLR 2024, arXiv:2309.08532) and OPRO (Yang et al., ICLR 2024, arXiv:2309.03409) likewise score candidates by task performance (instance objectives, dev-set accuracy), with the LLM proposing; OPRO feeds past solutions *with their scores* into the prompt [M].
- Surveys: Liu et al.'s systematic survey of LLMs for algorithm design (180+ papers; roles as optimiser, predictor, extractor, designer) [S] — [arXiv:2410.14716](https://arxiv.org/html/2410.14716v4); Zhang et al., LLMs for evolutionary optimisation [S] — [arXiv:2509.08269](https://arxiv.org/html/2509.08269v3); EC and LLMs [S] — [arXiv:2505.15741](https://arxiv.org/html/2505.15741v1).

**LLM scores (and describes) solutions**
- QDAIF uses LMs to both generate variation and "evaluate the quality and diversity of candidate text"; on creative writing it covers more of the search space with high-quality samples than non-QD controls, and human evaluation "validates reasonable agreement between AI and human evaluation" (no metric in the abstract) [V, abstract] — [Bradley et al., arXiv:2310.13032 (ICLR 2024 [M])](https://arxiv.org/abs/2310.13032).

**Reliability of LLM judges**
- Zheng et al.: GPT-4 as judge reaches "over 80% agreement" with human preferences on MT-Bench and Chatbot Arena, about the human–human level; named limitations are position bias, verbosity bias, self-enhancement bias and limited reasoning [V, abstract] — [Zheng et al. 2023, arXiv:2306.05685 (NeurIPS 2023 Datasets and Benchmarks [M])](https://arxiv.org/abs/2306.05685). Their mitigations (swapping positions, few-shot, chain-of-thought, reference-guided grading) are [M].
- Wang et al.: changing the order of candidates alone made Vicuna-13B beat ChatGPT on 66 of 80 queries with ChatGPT as evaluator; remedies are Multiple Evidence Calibration (evidence before score), Balanced Position Calibration (aggregate over orders), and Human-in-the-Loop Calibration (a balanced-position diversity entropy flags hard items for people) [V, abstract] — [Wang et al. 2023, arXiv:2305.17926](https://arxiv.org/abs/2305.17926).
- Panickssery, Bowman and Feng: LLMs (GPT-4, Llama 2) recognise their own outputs with non-trivial accuracy, and after fine-tuning self-recognition correlates linearly with the strength of self-preference bias; controlled experiments support causation [V, abstract] — [arXiv:2404.13076](https://arxiv.org/abs/2404.13076).
- JUDGE-BENCH (20 annotated NLP datasets, 11 LLMs): "substantial variance across models and datasets"; reliability depends on the property judged, the expertise of the human annotators and whether texts are human- or model-written; "LLMs should be carefully validated against human judgments before being used as evaluators" [V, abstract] — [Bavaresco et al., ACL 2025, arXiv:2406.18403](https://arxiv.org/abs/2406.18403).
- Raina, Liusie and Gales: short universal adversarial phrases appended to a text inflate LLM-judge scores, transferring from a surrogate to unseen judges, to the point that "maximum scores are predicted regardless of the assessed text"; judges are much more susceptible under absolute scoring than under comparative assessment [V, abstract] — [arXiv:2402.14016](https://arxiv.org/abs/2402.14016).
- Gao, Schulman and Hilton: optimising against a learned proxy reward too far "can hinder ground truth performance" (Goodhart's law); the gold-score curve has a different functional form for best-of-n and for RL, with coefficients scaling smoothly in reward-model size [V, abstract] — [arXiv:2210.10760 (ICML 2023 [M])](https://arxiv.org/abs/2210.10760).
- Norman, Rivera and Hughes (21 judges, ~541,000 judgments) report that the drop from exact-match agreement to Cohen's κ is 33–41 points on MT-Bench, judge rankings shift by up to 14 places across benchmarks, and test–retest reliability > 0.95 coexists with position bias > 0.10 — **but arXiv withdrew the paper (v2, 2026-11-03) as not meeting its quality standards; do not rely on its numbers** [V, abstract and withdrawal notice] — [arXiv:2606.19544](https://arxiv.org/abs/2606.19544).
- Surveys: Gu et al., "A Survey on LLM-as-a-Judge" (v6, Oct 2025), with a reliability-oriented meta-evaluation [S] — [arXiv:2411.15594](https://arxiv.org/pdf/2411.15594); scoring-based (absolute) judges show substantial scoring and self-bias, and are less studied than pairwise ones [S] — [arXiv:2506.22316](https://arxiv.org/pdf/2506.22316v3); "LLM-as-a-Meta-Judge" survey [S] — [arXiv:2601.17312](https://arxiv.org/html/2601.17312v1).
- Rubric-anchored and probability-weighted scoring (G-Eval, Liu et al. 2023, arXiv:2303.16634; Prometheus, Kim et al. ICLR 2024, arXiv:2310.08491) and repeated sampling with majority aggregation (self-consistency, Wang et al. ICLR 2023, arXiv:2203.11171) are the standard techniques for reducing variance of model judgments [M].

**Safeguards with guarantees**
- Trust or Escalate: a judge estimates its confidence and abstains below a threshold, which guarantees a chosen level of human agreement on the items it answers; "Simulated Annotators" improves calibration; "Cascaded Selective Evaluation" starts with a cheap model and escalates to a stronger one; on a Chatbot Arena subset where GPT-4 rarely reaches 80% agreement, a cascade starting from Mistral-7B guarantees > 80% agreement with almost 80% coverage [V, abstract] — [Jung, Brahman, Choi, arXiv:2407.18370 (ICLR 2025 [M])](https://arxiv.org/abs/2407.18370).
- Prediction-powered inference: a small gold-labelled set plus a model's predictions on many items yields valid confidence intervals for means, quantiles and regression coefficients with no assumption about the model; "more accurate predictions translate to smaller confidence intervals" [V, abstract] — [Angelopoulos et al., arXiv:2301.09633 (Science 382:669–674, 2023, DOI 10.1126/science.adi8811 [M])](https://arxiv.org/abs/2301.09633).

### Inferences
- The evidence separates two roles. *LLM as generator, program as judge* is well supported at scale (≈10⁶ samples in FunSearch). *LLM as judge inside the selection loop* is weakly supported for open-ended solution quality and is directly threatened by the optimiser: an EA is an adversary that will find phrases, formats or artefacts the judge over-rewards (Raina et al.), and proxy scores diverge from gold under optimisation (Gao et al.).
- Hence: LLM scores should enter as a *separate*, lower-standing objective or descriptor (AlphaEvolve's extra keys; QDAIF's descriptors), never merged into the programmatic fitness; comparative judgments should be preferred to absolute ones; candidates should be presented in both orders; the judge should not be the model that generated the candidates; and a human-labelled audit stream should track agreement (chance-corrected, e.g. κ) during the run, with selective evaluation or escalation when confidence is low.
- PPI is the natural way to report a *population-level* quantity (e.g. "share of the archive judged acceptable") from many LLM scores and a few human ones with valid intervals; it does not by itself make per-candidate selection reliable.

### Gaps
- No controlled study was found measuring how often LLM-judged fitness in an evolutionary loop is exploited (score inflation without gold improvement), as opposed to one-shot judging; QDAIF's full agreement figures and any hacking analysis were not read.
- No study was found of LLM judges evaluating resource *allocations* or policies specifically; all reliability evidence is from text, chat, NLP or code.
- Most LLM-judge reliability numbers are for 2023–2024 models; 2025–2026 model generations may differ.

---

## Q4. Combining Evaluators of Different Kinds and Costs (Cheap Surrogate, Expensive Simulation, Human) and Tracking Trust in Each

### Takeaway
Studies combine evaluators in four patterns: (1) *cascades* that pass a candidate to a costlier evaluator only if a cheaper one finds it promising (AlphaEvolve's staged tests; Trust or Escalate's cheap-to-strong judges; Optuna-style pruning); (2) *model management*, in which a surrogate pre-screens and the true evaluator is queried for a chosen subset, with the surrogate's reliability checked on re-evaluated points (Jin's surveys; Lamperti et al.'s active sampling; surrogate-assisted interactive EC); (3) *parallel objectives*, keeping each evaluator's output as its own metric (AlphaEvolve's dictionary; CGE calibration errors as separate objectives); and (4) *descriptor use*, where a judge's output defines the behaviour space of a quality-diversity archive (QDAIF). Trust is tracked by measuring each proxy against a more trusted reference on a sample (re-evaluation, human audit) and switching the proxy off when its reliability falls below a threshold. Asynchronous evaluation of evaluators with very different run times biases an EA toward cheap-to-evaluate regions unless corrected.

### Cited Findings

**Cascades and multi-fidelity**
- AlphaEvolve's cascade of test cases of rising difficulty prunes weak candidates early; LLM-graded properties are added as extra scores; multiple metrics improve the single target [V] — [Novikov et al. 2025](https://arxiv.org/html/2506.13131v1).
- Cascaded selective evaluation escalates from a cheap to a strong LLM judge while keeping a human-agreement guarantee [V, abstract] — [Jung et al. 2024](https://arxiv.org/abs/2407.18370).
- Successive halving / Hyperband allocate more budget (epochs, data, replications) to configurations that survive low-fidelity rounds [M] — Li et al. 2018, JMLR 18(185); co-kriging for multi-fidelity codes [M] — Kennedy & O'Hagan 2000.

**Surrogate model management and trust**
- Jin et al. (2019) present real-world case studies illustrating "different model management strategies for different categories of data-driven optimization problems" (offline with only historical data vs online with new samples) [S] — [Jin et al. 2019](https://ore.exeter.ac.uk/repository/handle/10871/34011). Jin (2011) distinguishes generation-based and individual-based model management and pre-selection [M] — Swarm Evol. Comput. 1(2):61–70.
- Lamperti et al. validate the ABM surrogate on a large out-of-sample set and improve precision of positive predictions with Platt-scaling calibration (94.72% → 99.35%) [V] — [arXiv:1703.10639](https://ar5iv.arxiv.org/html/1703.10639).
- Britz and Storm validate a CGE surrogate by R² (average 99.99%) before optimising on it [V, abstract] — [JGEA 2026](https://jgea.org/ojs/index.php/jgea/article/view/334).

**People as evaluators: interactive EC, fatigue, preference learning**
- Takagi's survey of interactive EC (about 250 papers, from graphic art and music to robotics and education) covers optimisation by subjective human evaluation, with interface research to reduce human fatigue [S] — [Takagi 2001, Proc. IEEE 89(9):1275–1296, DOI 10.1109/5.949485](https://gpbib.cs.ucl.ac.uk/gp-html/takagi_2001_ieee.html). A later overview by Takagi lists reducing user fatigue as one of three main IEC research directions [S] — [Kyushu University repository](https://catalog.lib.kyushu-u.ac.jp/opac_download_md/4486290/IntConf067.pdf).
- Surrogates of the user, trained on the individuals the person has rated, approximate that person's preferences: support-vector regression (Sun et al.); Llorà et al. (GECCO 2005) combine partial orderings, SVMs and synthetic fitness and report needing 3–7 times fewer user evaluations on a simple test problem; collaborative filtering from users with similar preferences (Gong et al.) [S] — [Llorà et al. GECCO 2005](https://gpbib.cs.ucl.ac.uk/gecco2005/docs/p1363.pdf), [summary sources](https://www.springerprofessional.de/en/adaptive-evaluation-strategy-based-on-surrogate-model/2819616).
- An adaptive evaluation strategy (AES-IGA) switches to the surrogate only when the person's fatigue exceeds a threshold *and* the surrogate's reliability exceeds a threshold; a semi-supervised IGA sends back to the person not only the best individuals but also the most *uncertain* ones; paired-comparison IGAs (PC-IGA) replace absolute ratings with comparisons [S] — [Springer chapter](https://www.springerprofessional.de/en/adaptive-evaluation-strategy-based-on-surrogate-model/2819616), [Quiroz et al. 2009](https://www.cse.unr.edu/~quiroz/inc/docs/trans2009.pdf).
- Pairwise preferences are modelled by Bradley–Terry/Thurstone models (Chatbot Arena ranks models from crowd pairwise votes; Chiang et al. 2024, arXiv:2403.04132), by GP preference learning (Chu & Ghahramani 2005), and searched by preferential Bayesian optimisation (Brochu, de Freitas, Ghosh 2007; González et al. 2017, ICML) or reward models learned from comparisons (Christiano et al. 2017, NeurIPS) [M].

**Asynchrony and heterogeneous evaluation times**
- Asynchronous EAs favour individuals with shorter evaluation times, because they are produced and reinserted more often (Martin et al. 2015); Yagoubi et al. (2011) found asynchronous MOEAs avoid costly regions of the Pareto front under heterogeneous evaluation times; the bias also affects model-based EAs (ECGA, GOMEA); Karns and Desell (GECCO 2025) propose a mitigation that reduces the bias; others use it as implicit parsimony pressure [S] — [Karns & Desell, GECCO 2025](https://www.se.rit.edu/~travis/papers/2025_gecco_time_bias.pdf), [Martin et al. 2015](https://scholarsmine.mst.edu/comsci_facwork/91), [arXiv:2303.15543](https://arxiv.org/abs/2303.15543), [OSTI 1845800](https://www.osti.gov/servlets/purl/1845800).

**Robustness to evaluator error**
- Digital-evolution practitioners document many cases of evolution exploiting bugs or loopholes in evaluators (Lehman et al. 2020, "The surprising creativity of digital evolution", Artificial Life 26(2):274–306, arXiv:1803.03453) [M]; the LLM-judge results above (Raina et al.; Gao et al.) are the same phenomenon for learned evaluators [V, abstracts].

### Inferences
- A sound combination keeps a *trust order* among evaluators (programmatic check ≻ calibrated simulation ≻ validated surrogate ≻ LLM judge, with people placed by the task: authoritative for preferences, fallible for facts) and lets a cheaper evaluator *filter* or *prioritise* but not *decide* where a more trusted one is available at acceptable cost.
- Trust should be a running statistic, not a constant: the rate at which a proxy's ranking agrees with the reference on re-evaluated or audited candidates (rank correlation for surrogates, κ for judges against people), with the proxy demoted when it falls below a declared threshold (AES-IGA's rule generalised).
- When evaluators of very different cost run asynchronously, the framework should either generate in generations (heuresis's generational algorithms are order-independent by design) or correct the time bias explicitly; otherwise cheap evaluators silently steer the search.

### Gaps
- No study was found that tracks trust in *several* heterogeneous evaluators simultaneously (surrogate + simulation + LLM + human) within one evolutionary run; the patterns above come from pairs.
- Takagi's specific fatigue numbers (e.g. typical generation limits) were not verified.

---

## Q5. Fairness and Equity as Objectives or Constraints in Evolutionary Allocation

### Takeaway
In evolutionary allocation studies, equity is most often a separate objective in a multi-objective search (usually NSGA-II): a Gini coefficient beside economic benefit in water allocation, accessibility vs equity in EMS siting, a resource-fairness term among five objectives in epidemic patient allocation. The formal literature warns that Gini minimisation alone can be satisfied by levelling down unless outcomes are shifted or efficiency is kept as an objective, and offers alternatives (generalised Gini, α-fairness from utilitarian through proportional to maximin). Treating objectives as separate also avoids weighing incommensurable quantities, the stated motive in the CGE calibration work.

### Cited Findings
- A water-allocation model uses the Gini coefficient for equity alongside economic efficiency and NSGA-II "to derive the Pareto front of the water resources allocation system" [S] — [Scientific Reports 2021, DOI 10.1038/s41598-021-04734-2](https://link.springer.com/10.1038/s41598-021-04734-2).
- An EMS station-placement preprint is bi-objective, accessibility vs equity, solved with NSGA-II [S] — [Research Square preprint](https://sciety.org/articles/activity/10.21203/rs.3.rs-7135511/v1); a COVID-19 patient-allocation study includes "emergency medical resource fairness" with severity, hospital level, matching benefit and distance, solved with NSGA-II [S] — [PMC9753027](https://pmc.ncbi.nlm.nih.gov/articles/PMC9753027).
- Ogryczak argues that with an appropriate outcome shift, Gini minimisation is consistent both with inequity minimisation and with outcome maximisation [S] — [EconPapers](https://econpapers.repec.org/RePEc:spr:oprchp:978-3-030-18500-8_11); finding an allocation minimising the Gini index is NP-hard [S] — [Fair Division Minimizing Inequality, arXiv:1810.04259](https://arxiv.org/pdf/1810.04259).
- The generalised Gini function can represent utilitarian and maximin objectives, and α-fairness with α = 1 is proportional fairness (log Nash welfare) [S] — [DECAF, arXiv:2502.04281](https://arxiv.org/pdf/2502.04281); fairness-aware multi-objective optimisation [S] — [arXiv:2207.12138](https://arxiv.org/pdf/2207.12138).
- A health-policy paper argues equity and efficiency "are, and ought to be, treated as complementary rather than conflicting considerations" [S] — [PMC8536061](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8536061/).
- The CGE calibration MOEA keeps objectives separate because weights cannot be given a priori ("not straightforward to weigh GDP gains against welfare gains"), and MOEAs "enable the user to visualise a wide spectrum of outcomes without imposing any a priori preferences" [V] — [GTAP 2022 extended abstract](https://www.gtap.agecon.purdue.edu/resources/res_download.asp?D_ID=11031).
- Deb's feasibility rule (feasible ≻ infeasible; infeasible compared by total violation) lets an equity floor be imposed as a hard constraint without penalty weights [V, as implemented and cited in heuresis] — `foundations/heuresis/search/include/heuresis/search/evaluation.hpp`.

### Inferences
- Equity should be represented in at least two of three ways the framework supports natively: as one or more objectives (Gini or Theil for dispersion, the worst-off group's outcome for maximin, a share for sufficiency), as a declared hard constraint (a floor per group) handled by feasibility rules, or as a descriptor so that a QD archive keeps allocations spanning the equity–efficiency range for a person to choose from.
- Because equity metrics are computed from an allocation's *distribution over units*, the evaluator should return that distribution (per region, group or household) so that alternative equity measures can be computed later without re-running an expensive simulator.
- With a stochastic evaluator, equity metrics are themselves noisy and often non-linear (a Gini of means ≠ mean of Ginis); which is meant must be declared.

### Gaps
- No survey of fairness objectives in evolutionary allocation was found; the examples are individual NSGA-II applications seen through snippets.
- No study was found combining equity objectives with stochastic ABM or CGE evaluators and resampling.

---

## What This Means for a Universal Framework

The proposals below are inferences from the findings above, set against heuresis's existing `search::Evaluation` (objectives, constraints, descriptors, cost, status, reason), its ask–tell protocol and keyed draws, and the infrastructure's practice of grading statements (A structured source, B text with passage, C language model) and of dual verdicts (standing and measure kept apart, as in kybernesis).

### 1. An Evaluator's Record

Extend heuresis's `Evaluation` with what the literature shows is needed, without changing its order (Deb's rule over the trusted objectives):

```
Evaluation
  metrics[k]:                       # one per named metric (objective, constraint or descriptor)
    name, role ∈ {objective, constraint, descriptor, report}
    value                           # point estimate, minimised for objectives; ≤ 0 satisfied for constraints
    uncertainty                     # SEM; 0 = deterministic; absent = unknown (Ax convention)
    n                               # replicates behind it
    replicates?                     # raw samples, optional (for quantiles, multimodality, re-aggregation)
    per_case?                       # per-instance / per-unit scores (FunSearch signature; equity by unit)
  cost                              # in the budget's unit (already in heuresis)
  status, reason                    # ok | failed | refused | abstained, with why (abstained: a judge below its confidence)
  provenance:
    evaluator_id, evaluator_version, kind ∈ {closed_form, simulation, abm, surrogate, language_model, person}
    fidelity                        # stage of a cascade or level of a multi-fidelity ladder
    stream_key                      # (purpose, epoch, index) of the draws used: replay and common random numbers
    input_digest                    # canonical digest of the genotype/allocation as evaluated
    standing ∈ {verified, computed, estimated, proposed}   # programmatic/closed form = verified; simulation = computed;
                                                            # surrogate = estimated; LLM or unvalidated person = proposed
    judge?                          # for LLM: model, prompt/rubric digest, temperature, order shown, all samples;
                                    # for a person: who, when, presentation order, session
```

The evaluator also declares, once, what it can do: deterministic or stochastic; whether it can return more replicates of the same candidate on request; whether it supports stages (cascade) or intermediate reports (pruning); its expected cost per call; and the metrics and their roles.

### 2. Noise Policy

- **Replicates on demand, not fixed.** Start each candidate at n_min (from a power calculation for the declared smallest meaningful difference δ: about 64–78 per arm for d = 0.5 at power 0.8 in Lee et al.; fewer for larger effects), then spend more only where selection is in doubt.
- **Selection procedures with stated guarantees:** a race (Hoeffding, or empirical Bernstein when variances are small) when a distribution-free 1 − δ guarantee is wanted; OCBA or an indifference-zone procedure when outputs are near normal; SEDR (resample to a standard-error threshold) as a simple default. Each run's record states which guarantee its selections carry.
- **Monitor rather than assume:** UH-CMA-ES's rank-change measure as a run-time diagnostic of whether noise is overwhelming selection, with treatments (more replicates, larger population, larger step) logged.
- **Common random numbers by construction:** keyed draws make CRN free (same stream_key for all candidates of a generation); keep it only where the measured variance of *differences* falls, since it can hurt.
- **Never more replicates than the decision needs:** very large n makes trivial differences "significant" (Lee et al.).
- **Multimodal outputs:** keep replicates so that quantiles, attractor shares or risk measures can be objectives.

### 3. Cost Policy

- **Cascades** (AlphaEvolve): feasibility and cheap programmatic checks first, then low-fidelity simulation, then full simulation, then (if any) human review; a candidate advances only if promising.
- **Surrogates under model management:** a surrogate may pre-screen or order candidates; the true evaluator is queried on a subset chosen by an infill rule; the surrogate is retrained and its agreement with the true evaluator is tracked on every re-evaluated point; for binary criteria (a calibration test), calibrate it (Platt scaling).
- **Budget in the evaluator's own unit** (seconds, core-hours, LLM tokens, minutes of a person's attention), as heuresis already allows.
- **Asynchrony with heterogeneous costs:** prefer generational algorithms (order-independent in heuresis), or correct the bias toward cheap-to-evaluate regions explicitly.

### 4. Caching and Reproducibility

- **Key** every evaluation by (input_digest, evaluator_id, evaluator_version, fidelity, stream_key); a cache hit returns the recorded evaluation, and a request for more replicates uses new stream indices.
- **Record** every evaluation, append only, never replaced: the infrastructure already keeps runs under the digest of their canonical JSON and seals them in archeion's journal; evaluations can be held the same way.
- **LLM evaluations** are not reproducible by seed: record the model identifier, prompt and rubric digests, temperature, every sample and the presentation order, and treat a re-query as a new replicate, not a cache miss.
- **People:** record who judged what, when, in which order and session, so that fatigue and order effects can be estimated afterwards.

### 5. Trust in a Language Model's Judgement

- **Generator first, judge last.** Use LLMs to propose (as in heuresis's decision 0007) and let programmatic or simulated evaluators decide wherever they exist (FunSearch, AlphaEvolve).
- **Where an LLM must score**, its metric has standing *proposed* (grade C) and enters only as a separate objective, a tie-breaker below the verified objectives, or a descriptor (QDAIF), never summed into the trusted fitness.
- **Pairwise rather than absolute** (more robust to adversarial phrases), with both orders shown and aggregated (Balanced Position Calibration); a rubric with anchors; k samples per judgment, with their spread reported as the metric's uncertainty.
- **A judge from a different model family** from the generator (self-preference).
- **An audit stream:** a random sample of LLM-judged candidates also judged by people (or by a programmatic check where one exists); agreement tracked chance-corrected (κ) through the run; the judge demoted when agreement falls below a declared level. Low-confidence items escalate (Trust or Escalate: cheap judge, then strong judge, then person). Population-level statements combining many model scores and few human labels use prediction-powered inference.
- **Goodhart watch:** the gap between the LLM score and the audited score over generations is itself reported, since optimisation pressure is expected to open it (Gao et al.; Raina et al.).

### 6. Trust in a Person's Judgement

- **Comparisons over ratings:** pairwise comparisons or choices from small subsets, aggregated by Bradley–Terry (or Thurstone), with ties and intransitivity kept.
- **A fatigue budget per session** (in judgments or minutes), with the presentation order randomised and recorded.
- **A learned surrogate of the person,** used only when fatigue exceeds a threshold and the surrogate's held-out agreement exceeds another (AES-IGA's rule). The most uncertain candidates, not only the best, are sent back to the person.
- **Authority by kind of question:** a person's judgment has standing *verified* for preferences and value judgments they are entitled to make, and *proposed* for factual claims that a model or a check can verify.

### 7. Combining Evaluators Without One Score

- **Tiers before trade-offs.** Lexicographic: (i) failures last; (ii) hard constraints from verified evaluators (programmatic checks, legal or budget limits, equity floors) by Deb's feasibility rule; (iii) Pareto dominance over the verified and computed objectives; (iv) estimated (surrogate) and proposed (LLM, unvalidated human) metrics only as tie-breakers, descriptors, or a second front shown beside the first.
- **Many objectives kept separate:** objectives keep their evaluator's identity, so a front can be read as "efficient on the CGE model, equitable on the ABM, acceptable to the panel". A dual verdict per candidate (a standing and a measure kept apart, kybernesis's convention) fits this exactly.
- **Quality-diversity over descriptors:** descriptors may come from any evaluator (per-case score signatures, equity position, an LLM's classification of the allocation's strategy); a MAP-Elites archive (Mouret & Clune 2015, arXiv:1504.04909 [M]) keeps the best of each kind for people to choose among.
- **Equity as its own objectives or constraints:** a dispersion index (Gini, Theil), the worst-off group's outcome (maximin) and a sufficiency share, computed from the per-unit outcomes the evaluator returns; its threshold, if a constraint, is declared and its source stated.
- **Robustness to evaluator error:** any candidate that wins only on proposed metrics is flagged; winners are re-evaluated at the highest fidelity with fresh streams before being reported; an evaluator's anomalous gains (large improvement in a proxy with none in the reference) trigger an audit.

*Remark.* heuresis's present `Evaluation` covers objectives, constraints, descriptors, cost and failures. The additions above (uncertainty and replicates, per-case values, fidelity, stream key, standing and provenance) and the policies (resampling, racing, CRN, cascade, surrogate management, audit) are what its planned `noise` module and the infrastructure's edges (evaluators that call simulators, language models or people) would need. Following heuresis's scope boundary, the record and the selection procedures belong in heuresis; evaluators that touch the world belong at the edges.
