# Evolutionary Algorithms for Heterogeneous, Multi-Type Resource Allocation: Encodings, Variation, Repair and Evaluation

Markers: **[V]** verified by reading the source itself (abstract or full text) during this research; **[S]** from a search-result snippet or bibliographic record only; **[M]** from memory, not checked during this research (DOIs marked [M] may be wrong and must be checked before citation). Where a [M] claim is given a link, the link is a pointer for checking, not evidence.

## 1. Encodings That Handle Heterogeneity Without Losing Feasibility or Heritability

### Takeaway
No single encoding handles all kinds of heterogeneity. Each successful family puts feasibility in one of three places: in the genotype space (grouping, permutation and feasibility-preserving direct encodings), in a decoder (random keys, priority lists, weight-biased decoders), or in a repair step (direct encodings with heuristic repair). Each placement has a documented failure mode: redundancy and flat landscapes for decoders, the "feasibility problem" for penalties, and weak heritability for encodings whose genes have no stable meaning (ordinal encodings, assignment vectors under standard crossover).

### Cited Findings

**The catalogue of encodings and what each was built for**
- **Direct assignment and amount vectors.** Raidl and Gottlieb's direct encoding (DI) for the multidimensional knapsack problem (MKP) is a bit string with heuristic initialisation, repair and local improvement. Repair removes the least promising items until every constraint holds, then local improvement adds items. Both steps follow a heuristic item ordering taken from the LP relaxation, which is solved once in preprocessing. [V] — [Raidl & Gottlieb, TU Wien TR-186-1-04-05 (2004), journal version Evolutionary Computation 13(4):441–475 (2005)](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf); [dblp record](https://dblp.org/rec/journals/ec/RaidlG05)
- **Matrices of amounts** (for transportation-type allocation) have their own direct encodings and operators. The Raidl–Gottlieb reference list includes Eckert and Gottlieb, "Direct representation and variation operators for the fixed charge transportation problem" (PPSN 2002, pp. 77–87). [V that the work exists and is cited; its content is unchecked] — [Raidl & Gottlieb 2004 TR, references](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf). Michalewicz's matrix GA for the nonlinear transportation problem keeps row and column sums feasible under special crossover and mutation. [M] — [Michalewicz, Vignaux & Hobbs 1991, ORSA J. Computing 3(4):307–316, DOI 10.1287/ijoc.3.4.307 (unchecked)](https://doi.org/10.1287/ijoc.3.4.307)
- **Permutation decoders.** A permutation of all items is decoded first-fit: each item is added in the order given if it still fits. The permutation (PE) and ordinal (OR) representations therefore have search spaces of equal size. [V] — [Raidl & Gottlieb 2004](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Weight-biased decoding (WB).** Profits are multiplied by log-normally distributed weights (γ = 0.05) and the items are decoded by the same first-fit heuristic, so the genotype perturbs a greedy heuristic rather than naming a solution. [V] — [Raidl & Gottlieb 2004](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **All five MKP representations produce only solutions on the boundary of the feasible region.** That is deliberate: "the success of evolutionary algorithms for the MKP strongly depends on their ability to restrict or at least strongly focus the search on the boundary of the feasible region (Gottlieb, 1999)." [V] — [Raidl & Gottlieb 2004, §3.1](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Random keys.** A random key is "a randomly generated real number in the continuous interval [0,1)" (Bean 1994). A decoder is "a deterministic algorithm that takes, as input, a vector of random keys"; for a sequencing problem it sorts the keys and reads off the index order. [V] — [Londe, Pessoa, Andrade & Resende, "Biased random-key genetic algorithms: A review", arXiv 2312.00961](https://arxiv.org/html/2312.00961v2). Bean's paper is "Genetic algorithms and random keys for sequencing and optimization", ORSA Journal on Computing 6(2):154–160 (1994). [V citation, via the Raidl–Gottlieb reference list] — [TR references](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf); DOI 10.1287/ijoc.6.2.154 [M].
- **BRKGA.** "BRKGA was formally introduced by Gonçalves and Resende (2011a)." Its parts:
  - elite chromosomes are copied unchanged, the elite set intentionally smaller than the non-elite;
  - p_m mutants are generated afresh, as the initial population is;
  - crossover fills the remaining slots with one elite and one non-elite parent, by uniform crossover biased toward the elite parent (ρ > 0.5).

  "Usually, decoded solutions are feasible, but infeasibility can be dealt with by penalties." The representation makes the GA "independent of the problem it solves", which enables code reuse. [V] — [Londe et al., arXiv 2312.00961](https://arxiv.org/html/2312.00961v2). The tutorial is Gonçalves and Resende, J. Heuristics 17(5):487–525 (2011). [M] — [DOI 10.1007/s10732-010-9143-1 (unchecked)](https://doi.org/10.1007/s10732-010-9143-1)
- **BRKGA decoders are "mostly permutation-based or indicator-based".** The most common use of mathematical models in BRKGA is "inside the decoding process, which must be customized for each application." [V] — [Londe et al. 2025 review](https://arxiv.org/html/2312.00961v2). The review was published as EJOR 321(1):1–22 (2025), DOI 10.1016/j.ejor.2024.03.030. [S] — [RePEc record](https://econpapers.repec.org/RePEc:eee:ejores:v:321:y:2025:i:1:p:1-22)
- **The random-key optimizer (RKO, 2024–2025)** carries the idea beyond GAs. Solutions are random-key vectors "subsequently decoded into feasible solutions via problem-specific decoders". The metaheuristics, eight in the C++ framework, work only in key space and run alone or in parallel. It was tested on α-neighbourhood p-median, tree-of-hubs location and node-capacitated graph partitioning. Published in J. Heuristics (2025). [S] — [Chaves, Resende, Schuetz et al., arXiv 2411.04293](https://arxiv.org/html/2411.04293v3). A 2026 preprint applies RKO to mixed-integer programs with problem-specific decoders. [S] — [arXiv 2602.22173](https://arxiv.org/html/2602.22173v1)
- **Grouping encodings.** Falkenauer's grouping GA (GGA) is "a genetic algorithm heavily modified to suit the structure of grouping problems". Its operators act on groups (bins), not on items; bin packing, where items of various sizes are grouped into bins of fixed capacity, is the canonical case. [S] — [Falkenauer, "A hybrid grouping genetic algorithm for bin packing", J. Heuristics 2:5–30 (1996), DOI 10.1007/BF00226291](https://link.springer.com/doi/10.1007/BF00226291). The book is Falkenauer, *Genetic Algorithms and Grouping Problems* (Wiley, 1998). [M]
- **Grouping encodings carry over to multi-dimensional assignment.** Wilcox et al. formally defined multi-capacity bin packing, a generalisation of bin packing, and developed a "reordering grouping genetic algorithm (RGGA) to assign VMs to servers". [S] — [arXiv 2001.06585 (multifactorial EA for VM placement), citing Wilcox et al.](https://ar5iv.labs.arxiv.org/html/2001.06585). A fuzzy grouping GA was applied to a real VM placement problem in a healthcare cloud (power, utilisation, SLA violations). [S] — [Alharbe et al., Algorithms 15(4):128 (2022), DOI 10.3390/a15040128](https://www.mdpi.com/1999-4893/15/4/128)
- **Precedence-constrained multi-resource scheduling** standardly uses an activity list (a precedence-feasible permutation) decoded by a schedule generation scheme (SGS). Hartmann's 1998 GA is permutation-based on activity lists. [S] — [Hartmann, "A competitive genetic algorithm for resource-constrained project scheduling", Naval Research Logistics 45(7):733–750 (1998)](https://econpapers.repec.org/RePEc:wly:navres:v:45:y:1998:i:7:p:733-750). The alternative is random keys as activity priorities, decoded by a (modified parallel) SGS. [S] — [Mendes, Gonçalves & Resende, random-key GA for the RCPSP, Optimization Online 2005](https://optimization-online.org/2005/07/1169/)
- **Hybrid genotypes.** Mixed-integer evolution strategies (MIES) search spaces mixing real, ordinal integer and categorical parameters; categorical genes are mutated at random with a self-adapted probability. [S] — [Li, Emmerich, Eggermont, Bäck et al., Evolutionary Computation 21(1):29–64 (2013), DOI 10.1162/EVCO_a_00059](https://researchr.org/publication/LiEEBSDR13/bibtex); [CEGO `optimMIES` documentation](https://search.r-project.org/CRAN/refmans/CEGO/html/optimMIES.html)

**Known failure modes**
- **Redundancy of real-valued keys.** "WB and RK have the highest redundancy due to their real-valued genes"; PE and OR have identical, intermediate redundancy; DI the lowest. An earlier study "revealed that decoder-based EAs for the MKP suffer significantly from redundancy. The best option was to reduce the effects of redundancy by removing phenotypic duplicates." [V] — [Raidl & Gottlieb 2004, §6](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Weak locality and heritability of the ordinal encoding.** Among the EAs, OR "performed consistently worst (on 0.1% error level)". Its weakness "can be mainly explained by... weaker locality and heritability". [V] — [Raidl & Gottlieb 2004, §4.2, §10](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Flat landscapes and premature convergence in BRKGA.**
  - Flat landscapes: on Steiner triple covering "the solution landscape is relatively flat", and the remedy (lower bias, more mutants) cost "long computational times". In bin packing, Gonçalves and Resende (2013) changed the fitness to count the remaining space in bins, which "unflattens the landscape".
  - Poor fit for adjacency: vanilla BRKGA does poorly on the TSP because uniform crossover suits it badly.
  - Premature convergence: double elitism causes it, which "may lead to sub-optimal solutions when we can afford additional optimization time".

  [V] — [Londe et al. review, §6](https://arxiv.org/html/2312.00961v2)
- **Integer variables fixate in Gaussian samplers.** Integer variables stagnate in CMA-ES, and with binary variables "this stagnation more likely occurs because the granularity of the discretization becomes wider". Margin correction lower-bounds the marginal probability of generating other integer values. [S] — [Hamano et al., CMA-ES with Margin, arXiv 2205.13482 (GECCO 2022)](https://arxiv.org/abs/2205.13482v2)
- **Penalties lose feasibility.** "All authors who designed penalties for knapsack problems recognized the feasibility problem, i.e. the final population contains unfeasible solutions only." Gottlieb explains its origin through fitness segments and the initialisation routine. [S] — [Gottlieb, EvoWorkshops 2001, LNCS 2037:50–59, DOI 10.1007/3-540-45365-2_6](https://unpaywall.org/10.1007%2F3-540-45365-2_6)
- **Activity lists can be redundant in multi-mode settings.** One study argues they produce many redundant solutions that waste search effort. [S] — [search summary citing core.ac.uk 192752777](https://core.ac.uk/download/192752777.pdf)

### Inferences
- Heterogeneity across resource types (divisible or indivisible, different units) is best handled by **typed gene blocks, each with operators of its type**, rather than by one homogeneous string. Real genes for divisible amounts should be normalised (shares of a budget or capacity) so that units do not distort mutation step sizes. This is an inference from the MIES, pymoo and CatCMA designs (§3) and from the failure of encodings whose genes have no stable meaning (OR).
- Decoders "solve" feasibility at the price of redundancy and a weaker link between genotype and phenotype. The empirical remedy is phenotypic duplicate elimination, and a fitness that grades the inside of a plateau (Falkenauer's and Gonçalves–Resende's bin-packing fitness).
- Grouping encodings are the natural representation when what matters is *which items share a resource* (a server, a crew, a bin), not which resource gets which label. Assignment vectors under standard crossover break groups apart. This argument is Falkenauer's [M], and consistent with the later grouping-GA applications to VM placement [S].

### Gaps
- No study found that compares encodings on a problem with **several resource types where some are divisible and some indivisible** at once. The evidence is assembled from single-type problems (MKP, bin packing, RCPSP).
- Falkenauer's 1996 paper and 1998 book were not read. The argument that standard encodings of grouping problems are redundant and poorly heritable is from memory [M].
- Gonçalves and Resende (2011) was not read directly; its parameter recommendations (elite share, mutant share, ρ ≈ 0.7) are [M].

## 2. Direct Versus Indirect (Decoder-Based) Encodings: Comparative Evidence and Application Families

### Takeaway
The best controlled comparison (Raidl and Gottlieb on the MKP) found a **direct encoding with heuristic initialisation, repair and local improvement** best, and increasingly so on the largest instances. Among decoders, the heuristically biased ones (weight-biased, permutation, random keys) were close to one another and the ordinal one clearly worst. Locality, heritability and heuristic bias explained the ranking. In precedence-constrained multi-resource scheduling, the literature converges on activity lists with a serial SGS. Random keys are competitive and generic but not dominant.

### Cited Findings

**The controlled MKP comparison (Raidl & Gottlieb 2004/2005)**
- **Setup.**
  - Five representations in one steady-state EA: population 100, 10⁶ candidates per run, binary tournaments, phenotypic duplicates discarded, 30 runs per instance.
  - Instances: OR-Library Chu–Beasley CB1–CB9 (α = 0.5), and the Glover–Kochenberger GK01–GK11 (100–2,500 items, 15–100 constraints).
  - Quality measure: the gap to the LP relaxation's optimum.

  [V] — [Raidl & Gottlieb 2004, §4](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Average gaps over all instances.** DI 0.329%, WB 0.493%, PE 0.605%, RK 0.631%, OR 1.597%; random search 6.353%. [V] — [ibid., Table 2](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **The gap widens with size.** On the largest instance, GK11 (m = 100, n = 2,500), DI reached 0.052% against WB 0.867%, PE 1.237% and RK 1.246%. On GK09 (n = 1,500), DI reached 0.029% against RK 1.064%. "Especially on the largest instances GK07 to GK11, DI clearly outperformed all other approaches." [V] — [ibid.](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Permutations and random keys are statistically interchangeable.** "Differences between PE and RK are small and not consistent": RK was better on CB3, CB5, CB6 and CB9, PE on GK05 and GK07–GK10. [V] — [ibid.](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **The explanation.** "DI exhibits the highest heuristic bias and strongest locality and heritability... This combination of good properties explains why DI achieves the best performance... The weaker effectivity of the other considered EAs can be mainly explained by their weaker locality and heritability (in particular OR) and their weaker heuristic bias." [V] — [ibid., §10](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **A cheap method for comparing representations.** Static measurements on random solutions (mutation innovation, crossover innovation, crossover loss, and the fitness distribution of initialisation and repair as heuristic bias) predicted the dynamic behaviour. Only a phenotypic distance metric is needed to apply them to other problems. [V] — [ibid., §10](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **An earlier result on locality.** "Strong locality appears to be a necessary condition for high performance, and heuristic bias also strongly affects solution quality." [S] — [Gottlieb & Raidl, "Characterizing locality in decoder-based EAs for the MKP", Artificial Evolution 1999](https://www.ac.tuwien.ac.at/files/pub/gottlieb-99.pdf)

**The theory of representations**
- Rothlauf's book treats **redundancy, scaling and locality** as the basic properties of a representation and studies how each affects GEA performance. The second edition adds a chapter on direct representations. [S] — [Rothlauf, *Representations for Genetic and Evolutionary Algorithms*, Springer, 2nd ed. (2006), DOI 10.1007/3-540-32444-5](https://www.springer.com/us/book/9783540250593); [1st ed. record](https://link.springer.com/doi/10.1007/978-3-642-88094-0)
- Rothlauf defined locality for point mutation, and for any operator as "distance distortion". [S] — [Shastri & Frachtenberg, PeerJ CS 561 (2021)](https://peerj.com/articles/cs-561)
- Whether redundancy is useful is disputed: Ronald (1997) and Knowles and Watson (2002) against, Rothlauf and Goldberg (2003) on its effects. [V that the dispute is recorded] — [Raidl & Gottlieb 2004, §6](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf). Rothlauf and Goldberg's finding that *uniformly* redundant encodings are neutral, while non-uniform redundancy helps only when it over-represents good solutions, is [M].

**Application families and what was learned**
- **Single- and multi-mode resource-constrained project scheduling (RCPSP, MRCPSP).**
  - "Most RCPSP metaheuristics use the standard activity list, which appears to work best regardless of the search paradigm." [S] — [Springer chapter, DOI 10.1007/978-0-387-33768-5_10, as summarised by search](https://link.springer.com/doi/10.1007/978-0-387-33768-5_10)
  - Kolisch and Hartmann's standardised comparison updates their 2000 survey and evaluates most heuristics on one experimental design. [S] — [Kolisch & Hartmann, EJOR 174(1):23–37 (2006), DOI 10.1016/j.ejor.2005.01.065](https://econpapers.repec.org/RePEc:eee:ejores:v:174:y:2006:i:1:p:23-37). A secondary source says it found activity lists with a serial SGS the strongest population-based approach on 60- and 120-activity instances, with the parallel SGS better on large instances. [S] — [SciTePress 2012 paper citing the survey](https://www.scitepress.org/PublishedPapers/2012/37594/pdf/index.html)
  - Hartmann's 1998 direct comparison (activity list better than random key, better than priority-rule encoding, all with a serial SGS) is [M]. It is not verified here; the full text was blocked (403).
  - For the MRCPSP, Van Peteghem and Vanhoucke's bi-population GA extends the serial SGS with a mode-improvement procedure, and also treats preemption. [S] — [EJOR 201(2):409–418 (2010)](https://econpapers.repec.org/RePEc:eee:ejores:v:201:y:2010:i:2:p:409-418). The genotype is a pair (activity list, mode vector), a natural mixed permutation-plus-categorical genotype. [M]
  - Hartmann and Briskorn's 2022 survey classifies RCPSP extensions by generalisations of activities, precedence relations and resource constraints, with alternative objectives and multi-project scheduling. [S] — [EJOR 297(1):1–14 (2022), DOI 10.1016/j.ejor.2021.05.004](https://econpapers.repec.org/RePEc:eee:ejores:v:297:y:2022:i:1:p:1-14)
- **Multi-skill workforce assignment (MS-RCPSP).**
  - The iMOPSE dataset was built because PSPLIB does not reflect multi-skill scheduling. It has 36 artificial instances modelled on real enterprise projects, typically 100 tasks, 20 resources and 9 or 15 skill types. It ships with a generator, a validator, a visualiser and greedy and GA solvers. [S] — [Myszkowski et al., Soft Computing (2018), DOI 10.1007/s00500-017-2997-5](https://link.springer.com/article/10.1007/s00500-017-2997-5)
  - Methods tested on it include GRASP, GAs with specialised operators, co-evolution and TLBO; one co-evolutionary approach splits the genotype into task priorities and resource priorities. [S] — [same search results](https://link.springer.com/article/10.1007/s00500-017-2997-5)
  - Snauwaert and Vanhoucke (2023) proposed a classification scheme and new MS-RCPSP datasets, validated against seven empirical projects. [S] — [EJOR 307(1):1–19 (2023)](https://econpapers.repec.org/RePEc:eee:ejores:v:307:y:2023:i:1:p:1-19)
- **Cloud and VM placement as multi-dimensional bin packing.**
  - VM placement is a multi-capacity bin-packing problem: CPU, memory and bandwidth dedicated per item. [S] — [IJECE review of bin-packing algorithms for VM placement](https://ijece.iaescore.com/index.php/IJECE/article/download/10621/11802)
  - The Guzek–Bouvry–Talbi survey of EC for cloud resource management notes that the mapping problem reduces to NP-hard bin packing. [S] — [survey PDF](https://typeset.io/pdf/a-survey-of-evolutionary-computation-for-resource-management-3ix1n5uqnb.pdf). Venue IEEE Computational Intelligence Magazine 10(2) (2015). [M]
  - Xu and Fortes (GreenCom 2010) used a GA with fuzzy multi-objective evaluation (waste, power, thermal cost). [S] — [cited in Alharbe et al. 2022](https://www.mdpi.com/1999-4893/15/4/128)
  - Grouping GAs recur in this family: RGGA, Chen 2017, FGGA 2022. [S] — [arXiv 2001.06585](https://ar5iv.labs.arxiv.org/html/2001.06585); [Alharbe et al.](https://www.mdpi.com/1999-4893/15/4/128)
- **Budget and portfolio allocation.**
  - Project portfolio selection with interdependencies, qualitative benefits, several objectives and workforce constraints is the subject of an edited Springer volume (2022). [S] — [*Evolutionary and Memetic Computing for Project Portfolio Selection and Scheduling*](https://link.springer.com/978-3-030-88314-0)
  - Yu et al. solve multi-criteria selection with project interactions as a GA-based nonlinear integer programme. [S] — [Annals of OR (2012)](https://link.springer.com/article/10.1007/s10479-010-0819-6)
  - Medaglia, Graves and Ringuest's multi-objective EA for linearly constrained project selection under uncertainty is [M]: EJOR 179(3):869–894 (2007).
  - The MKP is the canonical capital-budgeting abstraction (m budget periods or resources), so the Raidl–Gottlieb findings apply directly. [M as interpretation]
- **Health care and emergency resources.**
  - Aytug and Saydam compare GA forms on large maximum expected covering location problems for ambulances. A citing source says two GAs, one with local search, outperformed an integer-programming approach and Daskin's heuristic. [S, secondhand] — [EJOR 141(3):480–494 (2002)](https://ideas.repec.org/a/eee/ejores/v141y2002i3p480-494.html)
  - GAs have been coupled with Larson's hypercube queueing model as the evaluator. [S] — [same results](https://ideas.repec.org/a/eee/ejores/v141y2002i3p480-494.html)
  - Age-group vaccine allocation in several stages: an improved SPEA2 on an age-stratified SEIR model, minimising onsets and severe cases. The optimal priority group changes with production capacity and vaccine effectiveness. [S] — [Wu, Wang & Xu, Frontiers in Public Health (2022), DOI 10.3389/fpubh.2022.934891](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC9493087/)
- **Energy and spectrum.** The binary-GA unit commitment of Kazarlis, Bakirtzis and Petridis is [M]: IEEE Trans. Power Systems 11(1):83–92 (1996). No spectrum-allocation comparison was verified.

### Inferences
- **"Direct beats indirect" is conditional.** It holds when the direct encoding is paired with problem-informed repair and local improvement, which is itself a decoder applied after variation. The decisive variables are heuristic bias, locality and heritability, not the label "direct" or "indirect".
- **Random keys pay for generality.** They are the cheapest way to obtain feasibility and generality: one GA engine, problem knowledge confined to the decoder. On the MKP they matched permutations but lost to well-built direct encodings, by a margin that grows with size. That is a cost a universal framework should expect and measure.
- **Precedence and time favour order genotypes.** Where the constraints concern order (precedence, time windows), priority and order genotypes decoded by an SGS dominate the literature. Where they concern packing, grouping genotypes and fill-based fitness do.

### Gaps
- Hartmann (1998) and Kolisch and Hartmann (2006) were not read in full, so the precise RCPSP representation ranking is [S]/[M].
- No verified head-to-head comparison of encodings for VM placement, ambulance location, ICU beds or spectrum allocation was found. The application literature mostly reports one encoding against classical heuristics.
- No 2023–2026 study was found that overturns the Raidl–Gottlieb ranking; the recent work (RKO, BRKGA variants) extends decoders without new controlled comparisons against direct encodings.

## 3. A Single Genotype Mixing Continuous Amounts, Integer Counts, Categorical Choices and Orderings

### Takeaway
Mature tools represent a mixed genotype as a **named product of typed variables, each type with its own sampling, mutation and crossover**:
- pymoo's `vars` dict with `Real`, `Integer`, `Choice` and `Binary`;
- MIES's real, ordinal and categorical vector;
- CatCMA's joint Gaussian × categorical distribution.

Since 2022–2025 the CMA-ES line has handled all three numeric and categorical types jointly, with margins that prevent fixation. Orderings (permutations) remain outside these mixed-variable frameworks; they enter through random keys or as a separate permutation block.

### Cited Findings
- **pymoo.**
  - Mixed variables are declared from `pymoo.core.variable`: `Real`, `Integer`, `Choice(options=[...])` and `Binary`, passed to the problem as a `vars` dictionary. In `_evaluate` each design is a dict keyed by variable name.
  - `MixedVariableGA` applies a different operator to each variable type.
  - Multi-objective use is either an existing algorithm with `MixedVariableMating`, or `MixedVariableGA(survival=RankAndCrowdingSurvival())`, NSGA-II's survival.
  - The page states no explicit limitations, and permutations are not among the variable types.

  [V] — [pymoo documentation, "Mixed Variable Problem"](https://pymoo.org/customization/mixed.html)
- **MIES.** Real, ordinal-integer and categorical parameters in one ES; categorical genes are mutated at random with a self-adapted probability. A reimplementation keeps one step size per parameter. [S] — [Li et al. 2013](https://researchr.org/publication/LiEEBSDR13/bibtex); [CEGO `optimMIES`](https://search.r-project.org/CRAN/refmans/CEGO/html/optimMIES.html)
- **CMA-ES with margin (2022).** It lower-bounds the marginal probabilities of generating integer values in the multivariate Gaussian, to avoid integer stagnation. [S] — [arXiv 2205.13482](https://arxiv.org/abs/2205.13482v2)
- **CatCMA with Margin (2025–2026).** It "jointly optimizes continuous, integer, and categorical variables". Integer domains may be discrete non-integer sets such as {0.01, 0.1, 1}.
  - Integers: a modified margin that also caps the marginal probability when no integer mutation succeeds, with integer centring.
  - Categories: a margin q_min on category probabilities prevents fixation, with promising values derived analytically.
  - Benchmarks include MVProximity, which has dependencies between variable types.
  - Results: "CatCMAwM finds better solutions compared to Bayesian optimization methods" (CASMOPOLITAN, SMAC3, TPE), except in some low-budget cases, at lower computation time. A multi-objective variant, COMO-CatCMAwM on Sofomore, is "competitive or superior" in hypervolume.

  [V] — [Hamano, Nomura, Saito, Uchida & Shirakawa, arXiv 2504.07884v6](https://arxiv.org/html/2504.07884v6). Venue GECCO 2025, per the search summary [S] — [search record](https://arxiv.org/pdf/2504.07884)
- **Mixed-variable ant colony optimisation.** ACO_MV extends ACO_R to mixed variables and proposes artificial mixed-variable benchmark functions. [S] — [search summary of Liao et al., IEEE TEVC 2014](https://www.academia.edu/166874160/Ant_Colony_Optimization_for_Mixed_Variable_Optimization_Problems)
- **Conditional and hierarchical spaces.** Variables are called "dimensional, hierarchical, conditional, or nested" by domain. Prager and Trautmann study landscape features of mixed-variable hyperparameter spaces by relaxing hierarchical constraints. [S] — [arXiv 2402.16467](https://arxiv.org/html/2402.16467v1). ES-ENAS reports that vanilla combinatorial EAs degrade when the continuous part of a hybrid space grows. [S] — [arXiv 2101.07415](https://arxiv.org/pdf/2101.07415)
- **Composite genotypes in BRKGA.** Multi-objective BRKGA uses NSGA-II-style selection (Roque et al. 2017; Chagas et al. 2021), and multi-population mp-BRKGA (Fontes et al. 2023). [V] — [Londe et al. review](https://arxiv.org/html/2312.00961v2)
- **Other frameworks.** DEAP individuals are arbitrary containers with operators registered per individual, so mixed genotypes are assembled by hand. jMetal offers a `CompositeSolution` joining, for example, double and binary solutions. ParadisEO uses templated genotypes. [M] — not checked in this research.

### Inferences
- A universal genotype should be a **record of typed blocks**: real amounts, integers (with or without order), categorical choices, permutations or priorities, and partitions or groupings. Each block owns its sampler, mutation and crossover, and recombination is applied block-wise. This is the common structure of pymoo, MIES and CatCMA; adding permutations and partitions extends it to scheduling and packing.
- Fixation of discrete genes is a documented failure in model-based samplers and must be prevented explicitly (margins), not left to chance.
- Conditional genes (a mode's parameters active only when that mode is chosen) are handled ad hoc. Keeping inactive genes adds neutral redundancy, which Raidl and Gottlieb found harmful when not countered by phenotypic duplicate elimination.

### Gaps
- No verified study evaluates permutation genes inside a mixed-variable ES or GA against keys decoded in the same framework.
- pymoo's operator defaults per type (SBX or PM for reals, rounding for integers, random resetting for choices) were not checked against its source code.
- No standard benchmark for conditional, hierarchical, mixed spaces was found (the search confirms the absence).

## 4. Constraint Handling for Budgets, Capacities, Indivisibility, Eligibility and Precedence

### Takeaway
For allocation under budgets and capacities, the best-supported practice is to **keep the search on or near the feasible boundary**: decoders that produce only feasible, maximal allocations, or repair that removes the least efficient items and then refills. Static penalties are weak for knapsack-like constraints. For general numerical constraints, the standard techniques are:
- Deb's feasibility rules: parameter-free, but prone to losing diversity;
- stochastic ranking;
- ε-constrained methods, the strongest in competitions [M].

Hard structural constraints (indivisibility, eligibility, precedence) belong in the representation or the decoder, not in penalties.

### Cited Findings
- **MKP constraint handling.** "The most promising approaches are based on heuristic decoders (Raidl, 1999) or repair algorithms combined with local improvement (Chu and Beasley, 1998; Raidl, 1998)." Success "strongly depends on [the] ability to restrict or at least strongly focus the search on the boundary of the feasible region (Gottlieb, 1999)." [V] — [Raidl & Gottlieb 2004, §3.1](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Repair biases the search.** Repair dominates construction at tight constraints, and local improvement at loose ones; how much each contributes depends on the tightness ratio. [V] — [Raidl & Gottlieb 2004, §7](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Chu and Beasley's GA** with a repair operator guided by surrogate-relaxation pseudo-utility ratios [V for existence and citation; repair details M] — [J. Heuristics 4:63–86 (1998), cited in the TR](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Penalties have a feasibility problem on knapsacks.** The final population contains only infeasible solutions; the cause lies in fitness segments and initialisation. [S] — [Gottlieb 2001](https://unpaywall.org/10.1007%2F3-540-45365-2_6). A further study found GAs with repair more efficient than GAs with penalties on knapsack problems of restrictive and average capacity. [S] — [Dagstuhl-hosted study, per search summary](https://drops.dagstuhl.de/opus/volltexte/2008/1482)
- **Salcedo-Sanz's survey** covers the most important repair heuristics in EAs, including repair in graphs and trees and in grouping GAs, with guidance on hybrid EAs. [S] — [Computer Science Review 3(3):175–192 (2009), DOI 10.1016/j.cosrev.2009.07.001](https://portal.mardi4nfdi.de/wiki/Publication:458493). One review notes that repair algorithms are problem-specific. [S] — [arXiv 2206.13802](https://arxiv.org/pdf/2206.13802)
- **Mezura-Montes and Coello Coello (2011)** classify constraint handling with nature-inspired algorithms: feasibility rules, ε-constrained methods, penalties, stochastic ranking, special operators, multi-objective concepts and ensembles. They flag constrained multi-objective optimisation as under-studied. [S] — [Swarm and Evolutionary Computation 1:173–194, DOI 10.1016/j.swevo.2011.10.001](https://www.bohrium.com/paper-details/constraint-handling-in-nature-inspired-numerical-optimization-past-present-and-future/813364261471387649-2458)
- **Stochastic ranking.** The objective is balanced against the penalty stochastically: with probability P_f two neighbours are compared by objective, otherwise by violation. P_f → 0 seeks feasibility; P_f → 1 is objective-only. Tested on 13 benchmarks with a (μ, λ)-ES. [S] — [Runarsson & Yao, IEEE TEC 4(3):284–294 (2000), DOI 10.1109/4235.873238](https://scholars.ln.edu.hk/en/publications/stochastic-ranking-for-constrained-evolutionary-optimization/)
- **Deb's feasibility rules** for tournament selection [M] — [Deb, CMAME 186(2–4):311–338 (2000), DOI 10.1016/S0045-7825(99)00389-8 (unchecked)](https://doi.org/10.1016/S0045-7825(99)00389-8):
  - a feasible solution beats an infeasible one;
  - between feasible ones the better objective wins;
  - between infeasible ones the smaller violation wins.

  Parameter-free; known to risk premature convergence when feasible regions are disconnected. [M]
- **ε-constrained differential evolution** (Takahama and Sakai) relaxes feasibility by a shrinking ε; the 2010 version with an archive and gradient mutation won the CEC 2010 constrained competition. [M]
- **Coello Coello's 2002 survey** of constraint-handling techniques, with a separate section on repair algorithms. [S] — [search summary](https://homes.di.unimi.it/cordone/courses/2024-ae/Lez23-Materiali/constraint-handling.pdf); CMAME 191(11–12):1245–1287 [M]
- **Feasibility-preserving operators for continuous bounds** (reflection, projection and similar) tested with PSO, DE and GAs. [S] — [Padhye et al., arXiv 1504.04421](https://arxiv.org/pdf/1504.04421)
- **In BRKGA, infeasibility is handled mainly by the decoder,** and only secondarily by penalties. "Exact constraints in genetic operators have not been tried"; "there is no unified framework for the application of matheuristics in BRKGA." [V] — [Londe et al. review](https://arxiv.org/html/2312.00961v2)
- **Precedence** in scheduling GAs is kept by precedence-feasible activity lists and an SGS that respects resource capacities per period, so every decoded schedule is feasible. [S] — [Hartmann 1998 record](https://econpapers.repec.org/RePEc:wly:navres:v:45:y:1998:i:7:p:733-750); [Mendes et al. random-key GA](https://optimization-online.org/2005/07/1169/). In multi-mode problems non-renewable resource infeasibility can arise from mode choice, and is typically handled by penalty or mode repair. [M]

### Inferences
- **Decide by constraint type:**
  - indivisibility belongs in integer, binary or grouping genes;
  - eligibility belongs in categorical genes whose domain is the eligible set, or a decoder that skips ineligible pairs;
  - precedence belongs in order genes and an SGS;
  - budgets and capacities belong in a boundary-seeking decoder or repair-plus-refill guided by an efficiency ratio (surrogate duals or LP reduced costs);
  - soft or numerical constraints go to feasibility rules, stochastic ranking or ε-constraints.
- Penalties should be the fallback, used only with a feasible or repaired initial population. Gottlieb's analysis implies initialisation matters for their success.

### Gaps
- Deb (2000), Coello (2002), Takahama and Sakai, and the Mezura-Montes–Coello full text were not read; their detailed recommendations and evidence are [S]/[M].
- No study was found comparing these techniques specifically on heterogeneous multi-resource allocation (budgets plus capacities plus indivisibility). Evidence comes from the MKP and from continuous benchmarks (the CEC suites).

## 5. Benchmark Problems and Instance Libraries for Heterogeneous Allocation

### Takeaway
The standard libraries are:
- OR-Library's MKP sets (Chu–Beasley, 270 instances) and Glover–Kochenberger's large MKPs;
- PSPLIB (single- and multi-mode RCPSP);
- MMLIB (multi-mode, including MMLIB+);
- iMOPSE and the 2023 Snauwaert–Vanhoucke sets for multi-skill scheduling.

Optima are known for small classes and bounds or best-known values for large ones. There is no standard library for allocations mixing divisible and indivisible resource types.

### Cited Findings
- **The Chu–Beasley MKP suite (OR-Library).** Ten instances for each m ∈ {5, 10, 30}, n ∈ {100, 250, 500} and tightness α ∈ {0.25, 0.5, 0.75} (270 in all), with capacities c_i = α·Σ_j r_ij. [V] — [Raidl & Gottlieb 2004, §4.1](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **Glover–Kochenberger GK01–GK11.** 100–2,500 items, 15–100 constraints, first referenced by Vasquez and Hao (2001). Gaps there were measured against LP bounds, not optima. [V] — [ibid.](https://www.ac.tuwien.ac.at/files/pub/raidl-04a.pdf)
- **PSPLIB** (Kolisch and Sprecher 1997): J30, J60, J90 and J120 single-mode sets, and multi-mode sets J10–J30. Optima are known for all J30 instances; many J60–J120 instances have only bounds or best-known values. [M] — [Kolisch & Sprecher, EJOR 96(1):205–216 (1997), DOI 10.1016/S0377-2217(96)00170-1 (unchecked)](https://doi.org/10.1016/S0377-2217(96)00170-1)
- **MMLIB.** MMLIB50, MMLIB100 and MMLIB+ (3,240 instances), introduced by Van Peteghem and Vanhoucke (2014). A mirror-based GA was later tested on 4,320 of these instances under a limit of 50,000 schedules. [S] — [Zamani 2019 summary, via search](https://ro.uow.edu.au/eispapers1/2293); [Van Peteghem & Vanhoucke 2014 is EJOR 235(1):62–72 [M]](https://repository.vlerick.com/entities/publication/549e756c-d7e1-4ad7-b412-df4646ed6a09)
- **iMOPSE** (multi-skill): 36 instances plus 6 small teaching instances, with generator, validator and visualiser. [S] — [Myszkowski et al. 2018](https://link.springer.com/article/10.1007/s00500-017-2997-5)
- **Snauwaert and Vanhoucke's MS-RCPSP datasets** (2023). [S] — [EJOR 307(1):1–19](https://econpapers.repec.org/RePEc:eee:ejores:v:307:y:2023:i:1:p:1-19)
- **Mixed-variable continuous benchmarks.** CatCMAwM's suite (SphereIntCOM, EllipsoidIntCLO, REllipsoidIntCLO, MVProximity, and the bi-objective DSIntLFTL) [V] — [arXiv 2504.07884](https://arxiv.org/html/2504.07884v6); ACO_MV's mixed-variable functions [S] — [search summary](https://www.academia.edu/166874160/Ant_Colony_Optimization_for_Mixed_Variable_Optimization_Problems)
- **Ambulance location.** Large maximum expected covering instances (Aytug & Saydam 2002) [S] — [IDEAS](https://ideas.repec.org/a/eee/ejores/v141y2002i3p480-494.html). There is no shared public library. [inference]
- **Vector bin packing (VM placement)**: instance sets such as Caprara and Toth's two-dimensional vector packing, and Panigrahy et al.'s heuristics study. [M]
- **Non-evolutionary baselines.** OR-Tools CP-SAT, with interval variables and cumulative constraints, and MIP solvers are the natural exact or hybrid baselines for these libraries. [M] — not checked in this research.

### Inferences
- A universal framework should be validated on at least one library per constraint kind:
  - MKP (budgets and capacities);
  - PSPLIB or MMLIB (precedence, renewable and non-renewable resources, modes);
  - iMOPSE (eligibility by skill);
  - vector bin packing (multi-dimensional capacities and grouping);
  - a mixed-variable continuous suite (divisible amounts with integer and categorical genes).

  Results should be reported as gaps to optima, best-known values or LP bounds, under equal evaluation budgets (schedules generated, as MMLIB studies do).

### Gaps
- PSPLIB's and MMLIB's current best-known values, and how many instances remain open, were not verified.
- No benchmark library was found for **heterogeneous** allocation in the user's sense (several resource types with different units, some divisible), nor for public-budget allocation.

## What This Means for a Universal Framework

These are design implications drawn from the evidence above, each tied to the findings that support it.

1. **The genotype is a typed record of blocks, not a string.** Block types:
   - `Real` amounts, normalised as shares of a budget or capacity so units do not matter;
   - `Integer` counts (ordered);
   - `Categorical` choices (an eligible set per gene);
   - `Permutation` or `Priority` (orderings, or random keys);
   - `Partition` (groupings).

   Each block has its own initialisation, mutation and crossover, applied block-wise; pymoo's `vars`, MIES and CatCMA share this structure (§3). Multi-objective survival (NSGA-II rank and crowding) composes on top, as in pymoo's `MixedVariableGA(survival=RankAndCrowdingSurvival())` [V].
2. **Two representation routes, chosen per problem and comparable in one engine:**
   - **Direct plus repair plus local improvement**, where a problem heuristic exists. It was best on the MKP, increasingly with size: an average gap of 0.33% against 0.49–1.60% for the decoders [V].
   - **Keys plus decoder** (BRKGA or RKO style) as the generic route. Problem knowledge sits only in the decoder; feasibility holds by construction; the same engine serves any problem [V/S].

   The framework should let one problem be run both ways and report the difference: the generality premium.
3. **Decoders should seek the boundary and be biased by a heuristic.** For budgets and capacities, decode in an order given by the genotype and add while capacity remains (first fit), or perturb an efficiency ratio (weight-biased decoding). Every decoded allocation is then feasible and maximal. Repair should drop the least efficient items first and then refill [V].
4. **Precedence and time go through order genes and a schedule generator.** Use a precedence-feasible activity list or priority keys decoded by a serial or parallel SGS that respects per-period capacities. Use a mode vector (categorical) for multi-mode tasks, with a mode-improvement step [S]. Treat the SGS as a pluggable decoder.
5. **Grouping problems use grouping operators.** When the question is which items share a resource (VMs on servers, staff in crews), use a group-based encoding with group-level crossover, and a fitness that grades within plateaus (the fill of bins), which "unflattens" the landscape [V/S].
6. **Constraint handling by kind of constraint** (§4):
   - indivisibility and eligibility in the gene domains;
   - precedence in the decoder;
   - budgets and capacities in a boundary-seeking decoder or repair;
   - soft or numerical constraints by Deb's rules, stochastic ranking (P_f ≈ 0.45 in Runarsson & Yao [M]) or ε-constraints;
   - penalties last, never with an infeasible initial population (the knapsack "feasibility problem") [S].
7. **Discrete genes need anti-fixation.** For model-based samplers (CMA-ES or EDAs), add margins on integer and categorical marginals (CMA-ES with Margin, CatCMAwM) [V/S]. For GAs, keep a mutant share or immigrants (BRKGA's mutants), with island models, shake and reset against premature convergence [V].
8. **Phenotypic duplicate elimination is on by default.** It counters the redundancy of keys and real genes, which was largest exactly for random keys and weight-biased encodings [V].
9. **The framework measures its representations.** Raidl and Gottlieb's static measures (mutation innovation, crossover innovation, crossover loss, heuristic bias), computed from random samples with a phenotypic distance, predicted EA performance. They are cheap diagnostics to run before committing to an encoding, and need only a distance on allocations [V].
10. **Benchmarks and baselines.**
    - Ship adapters for the OR-Library MKP (Chu–Beasley, GK), PSPLIB, MMLIB, iMOPSE and the Snauwaert–Vanhoucke sets, and mixed-variable suites.
    - Report gaps to optima, best-known values or LP bounds under equal evaluation budgets.
    - Compare against an exact or CP baseline (CP-SAT or MIP) wherever instance size allows [M for the solver side].
11. **Where the evidence is thin, the framework should say so.** Nothing verified compares encodings for genuinely heterogeneous (mixed divisible and indivisible, multi-unit) allocation, for conditional genotypes, or in the health-care, spectrum and public-budget families. A universal framework would itself produce that evidence by running the two routes of point 2 on the same instances.
