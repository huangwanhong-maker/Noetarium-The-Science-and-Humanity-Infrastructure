# Part 3: Representation Spaces and Frameworks for a Universal Evolutionary Search

Scope: theories that unify variation operators across spaces (Moraglio, Radcliffe, Surry, Rothlauf); evolution on manifolds and constrained spaces (Riemannian CMA-ES, tangent-space sampling, the simplex, sets, multisets, graphs); mixed and conditional spaces (CMA-ES with margin, CatCMA, MIES, mixed Bayesian optimisation); type- and grammar-driven derivation (strongly typed GP, polytypic GP, Boltzmann samplers, QuickCheck); and how existing frameworks represent heterogeneous search spaces (Nevergrad, pymoo, DEAP, ParadisEO, jMetal, HeuristicLab, ECJ, EvoTorch, pyribs, Optuna, ConfigSpace, Ax).

Marks: **[V]** = verified by reading the source text itself during this research (PDF text extracted, or the page fetched); **[S]** = from a search-result snippet or a secondary summary only; **[M]** = from memory, not checked in this session. Search date: 2026-11-20.

---

## Key Question 1: Is there a principled, space-independent definition of mutation and crossover (e.g. via a metric's balls and segments), and what conditions must a space satisfy for it to work well?

### Takeaway
Yes. Moraglio's geometric operators define mutation by the **balls** and crossover by the **segments** (metric intervals) of any metric on the search space. Radcliffe's forma analysis defines recombination from a chosen set of **equivalence relations**, through the properties respect, transmission and assortment. Both are representation-independent. Geometric crossover is a real restriction (some classical operators are provably non-geometric). Geometric mutation is not a restriction, since every mutation is geometric for some radius. The operators pay off only when the landscape is "smooth" or "concave" under the metric that induced them, and the product theorem lets geometric crossovers on parts compose into a geometric crossover on a product space. Neither theory guarantees performance: no-free-lunch applies.

### Cited Findings

**Moraglio's geometric framework (thesis, 2007)**
- Moraglio, *Towards a Geometric Unification of Evolutionary Algorithms*, PhD thesis, Dept. of Computer Science, University of Essex, November 2007, 392 pp., supervised by Riccardo Poli. It defines crossover and mutation geometrically from the search space's distance, without reference to a representation, and claims that all EAs with geometric crossover perform "convex search". [V] — [gp-bibliography record](https://gpbib.cs.ucl.ac.uk/gp-html/Moraglio_thesis.html); [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- The formal definitions (thesis §3.3) are as follows. [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
  - The image set of a k-ary operator is the set of offspring it produces with non-zero probability.
  - A unary operator M is a **geometric ε-mutation** on (C, d) if Im[M(p)] ⊆ B_d(p; ε), with ε the smallest such radius. The thesis remarks: "all mutations are geometric on any search space for some ε".
  - A binary operator CX is a **geometric crossover** if Im[CX(p1, p2)] ⊆ [p1; p2]_d, the metric segment {z : d(p1,z) + d(z,p2) = d(p1,p2)}.
  - The **uniform** variants draw uniformly from the ball or from the segment.
- Theorem 3.3.1: when d is a *graphic* distance (the path metric of a neighbourhood graph), each of the following determines the space uniquely: the neighbourhood graph, the uniform geometric mutation, the uniform geometric crossover, the set of balls, and the set of segments. Uniform geometric mutations and uniform geometric crossovers are therefore in one-to-one correspondence on graphic spaces. [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- Classical operators recognised as geometric. [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
  - Line recombination is geometric under Euclidean distance, and box recombination under Manhattan distance.
  - Discrete recombination is geometric under Manhattan distance, and also under Hamming distance extended to real vectors.
  - PMX is geometric under swap distance (Thm 5.7.1).
  - Cycle crossover is geometric under Hamming distance restricted to permutations (Thm 5.7.3), and also under swap distance (§5.8).
  - The thesis also covers homologous crossover for sequences (via edit distance and alignments, ch. 8) and for GP trees (ch. 7).
- **Graphs:** for the edge insertion/deletion edit distance, offspring are the intermediate graphs on a shortest edit path from one parent to the other. This is obtained by matching the parents' vertices and then adding and removing the minimum number of edges. [V] — [thesis PDF, §3](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Categorical versus ordinal integers** (§9.2.5): ordinal attributes take the absolute value distance |a − b|, and cardinal (categorical) attributes take the discrete metric. Under the discrete metric every segment is {a, b}, so "the only possible offspring of DM-GX are the parents". Hamming space H(n, |A|) is the product of discrete metrics. [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Product geometric crossover** (Thm 9.2.2). Any product geometric crossover, meaning each geometric crossover X_i applied to its own projection S_i, is geometric under the **sum** of the component distances (the Manhattan product metric). This rests on the product segment theorem: [a,b]_{d×d′} = [a,b]_δ with δ = d + d′. Corollary 9.2.3: the component crossovers need not be independent, since any dependency only shrinks the offspring pool. Named uses: [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
  - multi-crossover;
  - hybrid crossover;
  - hybrid-representation crossover (different representations per projection);
  - dependent crossover (e.g. a variable-size graph for a network's structure plus a variable-length real sequence for its weights, where "recombination of the first projection imposes constraints on the recombination of the second").
- **Bias on bounded spaces:** on a bounded box, uniform geometric crossover from uniformly distributed parents is biased toward the centre (illustrated on [0,100]). The thesis attributes this to the asymmetry (non-isotropy) of the space, not to the definition. [V] — [thesis PDF, §4](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Non-geometric crossovers exist**, so the definition is not tautological (§14.4). [V] — [thesis PDF](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
  - Extended line recombination is not geometric: convergence fails.
  - Koza's subtree swap crossover is not geometric: purity fails, since two copies of one parent can yield a different tree.
  - Davis's Order Crossover is not geometric: convergence fails, with a counterexample.
  - The thesis also notes that knowing an operator is geometric says nothing about its performance ("The no free lunch theorem rules"). Its design recipe is to pick a metric under which the landscape is smooth, then use that metric's geometric crossover.

**Convex search: when geometric crossover works**
- Moraglio and Sudholt, "Principled Design and Runtime Analysis of Abstract Convex Evolutionary Search", *Evolutionary Computation* (2015), DOI [10.1162/EVCO_a_00169](https://doi.org/10.1162/EVCO_a_00169). [V] — [accepted version](https://eprints.whiterose.ac.uk/93028/1/p1279-single.pdf)
  - All EAs with geometric crossover and no mutation do the same convex search, "regardless of the underlying representation, the specific selection mechanism, offspring distribution, search space, and problem at hand".
  - The paper studies runtime on (generalised) concave landscapes and gives a general runtime result instantiated for three search spaces.
  - Corollary: convex search optimises LeadingOnes in O(n log n) evaluations, faster than all unbiased unary black-box algorithms.
- The same paper surveys other unification attempts: Radcliffe's representation theory, Poli's unified schema theory, Stephens's coarse graining, Rothlauf's "popular but less formal" theory, and Rowe et al.'s group-theoretic theory built on Radcliffe's work. [V] — [accepted version](https://eprints.whiterose.ac.uk/93028/1/p1279-single.pdf)

**Geometric semantic GP**
- Moraglio, Krawiec and Johnson, "Geometric Semantic Genetic Programming", PPSN XII (2012), LNCS 7491. [V] for the content below; [S] for pages 21–31. — [PDF](https://www.cs.put.poznan.pl/kkrawiec/wiki/uploads/Research/2012PPSN.pdf)
  - Search operators act on program semantics (the vector of outputs on the training cases), not on syntax.
  - For problems whose fitness is a distance to target outputs, the landscape seen by the semantic operators "is always a cone by construction", hence potentially easy.
  - The cost: because offspring contain both parents, "the size of individuals grows exponentially with the number of generations", so offspring must be simplified (e.g. by Espresso, by computer algebra, or by formal methods).

**Radcliffe's forma analysis and representation-independent recombination**
- Radcliffe, "The Algebra of Genetic Algorithms", *Annals of Mathematics and Artificial Intelligence* 10:339–384 (1994), DOI [10.1007/BF01531276](https://doi.org/10.1007/BF01531276). The abstract says equivalence relations imposed on a search space give "a direct mechanism for harnessing knowledge about a search space ... to generate a genetic representation and operators", that "a single characterisation of a space leads to a unique genetic representation", and that random assorting recombination (RAR_w) is "in an important sense, a universal recombination operator". [V] — [PDF](https://www.stochasticsolutions.com/pdf/amai94.pdf)
- Definitions in the same paper. [V] — [PDF](https://www.stochasticsolutions.com/pdf/amai94.pdf)
  - **Purity** (Def. 40): identical parents produce identical children.
  - **Respect** (Def. 41): every child lies in every forma shared by both parents. Equivalently (Lemma 42), children lie in the parents' similarity set.
  - **Assortment** (Def. 44): any two compatible formae, one from each parent, can be combined in a child.
  - **Separability** (Def. 45): respect and assortment are compatible.
  - **Strict gene transmission** (Def. 47): every allele of a child is present in some parent. Lemma 48: transmission implies respect.
  - Theorem 63: RTR (random transmitting recombination) transmits and assorts g-separable genes. Theorem 69: likewise for RAR.
  - Observation 46: respect, assortment and separability depend only on the formae, not on genes or alleles.
- **Permutations as a negative example:** formae defined by sets of undirected edges (adjacency) are **non-separable**. A parent containing 1–2–3 and another containing 1–2–4 cannot be recombined so as both to respect the 1–2 edge and to assort the 2–3 and 2–4 edges. [V] — [PDF](https://www.stochasticsolutions.com/pdf/amai94.pdf)
- Radcliffe's own reading list of the forma papers. [V] for the list and its descriptions — [Stochastic Solutions forma papers](https://stochasticsolutions.com/formapapers)
  - Radcliffe 1991 (ICGA-4) "introduces the first representation-independent genetic operator – random respectful recombination (RRR)".
  - Radcliffe 1992 (PPSN 2) "Non-Linear Genetic Representations".
  - Radcliffe & Surry 1994 (FOGA III) "Fitness Variance of Formae", a measure for comparing representations.
  - Radcliffe & Surry 1995 (LNCS 1000) on a no-free-lunch theorem for representations.
  - Surry & Radcliffe 1996 (PPSN IV) "Formal Algorithms + Formal Representations = Search Strategies", on reusing the same formal algorithm across problem domains.
  - Surry & Radcliffe 1996 (FOGA IV) "Real Representations", formal representations of reals that relate standard genetic operators to evolution-strategy operators.
  - Radcliffe 1992 "Genetic Set Recombination", an operator for set-valued genotypes.

**Rothlauf: locality and redundancy (see also Key Question 5)**
- Rothlauf, *Representations for Genetic and Evolutionary Algorithms* (Springer, 2nd ed. 2006). It treats redundancy, scaling and locality as the core properties of a representation. [S] — [Springer](https://www.springer.com/us/book/9783540250593)
- Rothlauf's GECCO 2006 tutorial gives these definitions. [V] — [tutorial PDF](https://gpbib.cs.ucl.ac.uk/gecco2006etc/papers/tut103.pdf)
  - Locality is high when "neighboring genotypes correspond to neighboring phenotypes". Locality, causality and distance distortion "describe how well the metric on Φp fits to the metric on Φg".
  - Representations that change distances "modify the difficulty of the problem".
  - Phenotype–fitness mappings fall into three classes: positively correlated with distance to the optimum (easy for mutation-based search), uncorrelated, and negatively correlated (deceptive).

### Inferences
- Geometric crossover gives a constructive recipe for any space with a metric: implement "sample uniformly (or with a chosen bias) from the segment [x; y]_d". Geometric mutation gives "sample from the ball B(x, ε)".
- The recipe is efficient only where segments can be sampled cheaply, through a shortest-path structure (edit sequences, alignments, geodesics). For many edit distances on graphs, sampling segments requires solving a matching or alignment problem first.
- Two conditions emerge for a space to "work well" with geometric operators:
  1. The metric is one under which fitness is smooth or concave (high fitness–distance correlation, Rothlauf's class 1).
  2. Segments and balls are computable and samplable.
- Forma analysis adds a third condition: the chosen formae should be **separable**, or recombination cannot both respect and assort them. Adjacency formae on permutations show this fails for natural structures.
- The product theorem is the formal licence for **component-wise derivation** of crossover on product types: the result is geometric under the sum metric. It also exposes the implicit metric. Summing unscaled component distances is exactly what produces scale mismatch (Key Question 5).
- Moraglio's discrete-metric result explains why categorical genes cannot be "interpolated": their segments contain only the parents. Exploration for categories must come from mutation or from a distributional model (softmax, categorical distributions).

### Gaps
- I did not read Surry & Radcliffe's PPSN 1996 paper itself, only its description on Radcliffe's page. Its precise claims about reusing a formal algorithm across domains are unverified beyond that description.
- I did not obtain Rowe, Vose and Wright's group-theoretic representation papers. They are known here only through their mention in Moraglio & Sudholt.
- No source gives a general sufficient condition on (space, metric, fitness) for geometric crossover to beat mutation. Moraglio & Sudholt cover concave landscapes on three instantiated spaces only.

---

## Key Question 2: How do Riemannian/Lie-group evolution strategies move on a manifold (exponential map, retraction, parallel transport of the covariance), and which manifolds are supported in practice? (Also: the simplex, sets, multisets, graphs.)

### Takeaway
The standard scheme is **Riemannian adaptation**:
1. Keep the search distribution as coordinates in the tangent space T_mM at the current mean.
2. Sample there.
3. Map samples to M by the exponential map or a retraction.
4. Move the mean, then carry the covariance and evolution paths to the new tangent space by parallel or vector transport.

Colutto et al. (2010) did this on spheres. Manopt.jl provides a generic Riemannian CMA-ES for any manifold that supplies a retraction, a vector transport, a basis and coordinate maps. The main limitation is locality: the search is confined to a normal neighbourhood whose size is bounded by the injectivity radius, which shrinks with curvature. Lie groups such as SO(d) are also handled pragmatically, by running a Euclidean CMA-ES on the flat Lie algebra so(d) and exponentiating. For the simplex, there is no established evolutionary literature that uses Aitchison geometry. Dirichlet-centred mutation and renormalisation repair appear in applied papers.

### Cited Findings

**Riemannian CMA-ES**
- Colutto, Frühauf, Fuchs and Scherzer, "The CMA-ES on Riemannian Manifolds to Reconstruct Shapes in 3-D Voxel Images", *IEEE Transactions on Evolutionary Computation* 14(2):227–245 (2010). It extends CMA-ES to Riemannian manifolds, is tested on synthetic functions on the 2-sphere and on shape reconstruction from voxel data, and is compared against manifold gradient methods, tangent-space PSO and DE. [S] — [dblp](https://dblp.uni-trier.de/rec/journals/tec/ColuttoFFS10.html); [summary](https://synapsesocial.com/papers/W2110348055)
- Fong and Tino describe the general procedure, citing Colutto et al. as their [24]. [V] — [Fong & Tino, arXiv:1908.06783](https://arxiv.org/abs/1908.06783)
  - At each iteration "the optimization method determines a search direction on the tangent space centered at the current iterate".
  - "The next iterate is then obtained by tracing along the local geodesic ... via the Riemannian exponential map".
  - "The search information is subsequently parallel transported to (the tangent space of) the new search iterate".
  - "The decision space ... is thus given by the tangent (Euclidean) space, whereas the search space is given by the Riemannian manifold."
  - They list CMA-ES "on spherical manifolds [24]" and PSO on complete Riemannian manifolds as examples. Gradient methods may use a retraction, a first-order relaxation of the exponential map.
- Fong and Tino's critique of Riemannian adaptation. [V] — [arXiv:1908.06783](https://arxiv.org/abs/1908.06783)
  - The search region at x is bounded by the normal neighbourhood N_x.
  - "Manifolds with high scalar curvature will have small injectivity radius (thus small normal neighbourhoods)", so the CMA-ES advantage on multimodal problems "may not be reflected on the Riemannian version ... in more complex manifolds".
  - Their remedy, *Extended RSDFO*, builds mixture densities over totally bounded subsets. They claim monotone improvement of expected fitness and finite-step global convergence on connected compact Riemannian manifolds.
  - A later published version is linked as DOI 10.1007/978-3-031-04293-5 [S].
- **Manopt.jl's Riemannian CMA-ES** follows Hansen's tutorial "with basic Riemannian adaptations, related to transport of covariance matrix and its update vectors". [V] — [Manopt.jl CMA-ES](https://manoptjl.org/stable/solvers/cma_es/)
  - The covariance is stored in coordinates of a basis of T_{p_m}M, and samples are mapped to M with a configurable retraction.
  - When the mean moves, `eigenvector_transport!` carries the eigendecomposition with a configurable vector transport. This is described as an Ehresmann connection on M × SPD(n).
  - Requirements on the manifold: `retract!`, `vector_transport_to!`, `get_coordinates!`/`get_vector!` for a basis, `manifold_dimension`, `rand(M)`, `zero_vector`, `is_flat`.
  - Defaults: λ = 4 + ⌊3 log dim M⌋; stopping criteria mapped to Hansen's TolFun, TolX, TolXUp, ConditionCov, EqualFunValues.
  - It cites Colutto et al. 2010 as an earlier attempt and Hansen, "The CMA Evolution Strategy: A Tutorial", arXiv:1604.00772 (2023).
- Hansen's CMA-ES tutorial itself does not mention manifolds or Riemannian geometry: a text search of the PDF found no occurrence. [V] — [arXiv:1604.00772](https://arxiv.org/abs/1604.00772)
- On the sphere, vector transport can be taken as projection onto the new tangent space, and parallel transport as a rotation in the plane of the two points. [S] — [note on manifold optimisation](https://animadversio.github.io/academic_note/note-on-optimization-on-manifold) (a secondary source)
- Stiefel and Grassmann: a 2024 paper says that when Edelman et al. (1998) wrote, there was no known O(nd²) algorithm for Stiefel parallel transport. It gives an efficient formula, recovers the known Grassmann formula, and treats Grassmann as a quotient of Stiefel. [S] — [arXiv:2408.06054](https://arxiv.org/pdf/2408.06054)
- **Lie group via its algebra:** one paper optimises a rotation matrix by running CMA-ES on the flat space so(d) of dimension d(d−1)/2 and recovering W = exp(A). [S] — [arXiv:2501.18344](https://arxiv.org/pdf/2501.18344)
- **Information geometry of the search distribution** (not of the search space): [S]
  - xNES parametrises the covariance by the matrix exponential, so that updates happen in a vector space, and treats positive definite matrices as a Riemannian manifold under the Fisher metric — [NES, arXiv:1106.4487](https://arxiv.org/pdf/1106.4487).
  - Akimoto et al. give an information-geometric foundation of CMA-ES — [arXiv:1206.0730](https://ar5iv.labs.arxiv.org/html/1206.0730).

**The simplex (compositions and shares)**
- Aitchison, "The Statistical Analysis of Compositional Data", *JRSS B* 44(2):139–177 (1982), DOI [10.1111/j.2517-6161.1982.tb01195.x](https://doi.org/10.1111/j.2517-6161.1982.tb01195.x), introduced the log-ratio approach. A 2023 reappraisal is in *Statistical Science* 38(3):386–410. [S] — [reappraisal](https://ar5iv.labs.arxiv.org/html/2201.05197)
- Aitchison geometry makes the simplex a vector space under perturbation and powering, and the centred log-ratio maps it isometrically into a Hilbert space. [S] — [arXiv:2008.05410](https://arxiv.org/pdf/2008.05410)
- Evolutionary operators on the simplex found in the literature: [S]
  - Dirichlet sampling centred on the parent (symmetric, or with concentration proportional to the parent's weights), followed by projection to a minimum weight — [arXiv:2509.00510](https://arxiv.org/pdf/2509.00510).
  - Renormalisation after every operator, in a risk-parity GA — [arXiv:1411.7494](https://arxiv.org/pdf/1411.7494).
  - A GA "for optimization over a simplex" with a representation that generates feasible points directly, reported better than Cartesian chromosomes — [Unicamp](https://www.ime.unicamp.br/node/13386).
  - Random Dirichlet weights show "high run-to-run variability" and disruptive updates, reported in another context — [arXiv:2512.01249](https://arxiv.org/pdf/2512.01249).
- I found no paper that runs CMA-ES or another ES in Aitchison (ilr/clr) coordinates as a named method. See Gaps.

**Sets, multisets, sequences, graphs**
- Sets: Radcliffe 1992 "Genetic Set Recombination". [V] that it exists and its topic — [forma papers](https://stochasticsolutions.com/formapapers)
- Multisets as sequences: Moraglio generalises cycle crossover to permutations with repetitions and shows it is not geometric under swap distance (§5.8, §5.8.5). [V] — [thesis table of contents](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- Graphs: geometric crossover under edge insertion/deletion distance, and cycle crossovers for graph partitioning (thesis ch. 11). [V] — [thesis](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)

### Inferences
- A manifold "space" in a universal framework needs at least:
  - a chart at a point (tangent coordinates in an orthonormal basis);
  - exp, or a retraction, from T_xM to M;
  - a log, or an inverse retraction, for computing tangent displacements of selected samples;
  - a vector transport T_{x→y} for carrying covariance and paths;
  - an injectivity radius or trust radius, used to cap σ.
  Manopt's requirement list is a tested template for this.
- For compact Lie groups (SO(3)), sampling in so(3) and exponentiating at the current mean (x·exp(ξ)) is the usual construction. Left-translation gives a canonical transport (identify every tangent space with the Lie algebra), so covariance transport becomes trivial in the left-trivialised frame. This is my inference from standard Lie theory [M], consistent with the so(d)+CMA-ES pattern [S].
- For the simplex there are two coherent options:
  1. Treat Δ^{D−1} as a Euclidean space via the isometric log-ratio map. A Gaussian there is logistic-normal on the simplex, so any R^n ES applies unchanged and distance is Aitchison's.
  2. Use Dirichlet mutation centred on the parent, with concentration as the step-size analogue.
  Option 1 inherits CMA-ES's invariances and gives a metric for geometric crossover. Uniform crossover on the simplex can be done as a convex combination in ilr coordinates, which is the Aitchison segment.
- A geodesic segment on a manifold within the injectivity radius is a geometric-crossover segment under the Riemannian distance, so Moraglio's crossover extends to manifolds as "sample a point on the minimising geodesic". This is my inference, consistent with the thesis's metric definition.

### Gaps
- I could not read the Colutto et al. paper's text, so I do not know exactly how it transports the covariance on the sphere (parallel transport versus projection), or its step-size rule.
- I found no primary paper on natural evolution strategies whose search space (not distribution space) is a Riemannian manifold or Lie group, beyond the Fong & Tino framework and the pragmatic so(d) approach.
- No source verified for evolution strategies on Stiefel or Grassmann manifolds specifically. Manopt's generic CMA-ES should apply wherever its manifold library supplies the required maps, but I did not confirm which manifolds were tested.
- No primary evolutionary paper using Aitchison geometry was found. The simplex recommendations above are design inferences.

---

## Key Question 3: How do mature frameworks compose a heterogeneous genotype (product of typed parts) and attach variation operators to each part? (Includes mixed-integer ES and mixed-space Bayesian optimisation.)

### Takeaway
Three architectural patterns recur.
1. **Typed parameter trees with a standardised embedding** (Nevergrad). Each typed parameter maps bijectively to a block of R^n with a N(0, I) prior and also carries its own mutate/recombine. Containers (Tuple, Dict, Instrumentation) concatenate blocks, so any continuous optimiser can run on the embedding and structure-aware algorithms can use the per-parameter operators.
2. **Variables grouped by type, with a default operator per type** (pymoo MixedVariableGA). Individuals are dicts. Crossover and mutation are dispatched per variable class: SBX/PM for reals, the same with rounding for integers, uniform crossover and random reassignment for categories.
3. **A generic encoding type and composition of operator objects** (ParadisEO's EOT template; DEAP's toolbox registration; jMetal's CompositeSolution; HeuristicLab's MultiEncoding).

Hyperparameter-optimisation libraries add **conditional activation**: ConfigSpace conditions and forbidden clauses, Optuna's define-by-run, Ax's hierarchical `dependents`. Model-based methods make discreteness first-class by lower-bounding marginal probabilities (CMA-ES with margin) or by joint Gaussian–categorical distributions (CatCMA).

### Cited Findings

**Nevergrad**
- Rapin, Bennet, Centeno, Haziza, Moreau and Teytaud, "Open Source Evolutionary Structured Optimization", GECCO '20 Companion, DOI [10.1145/3377929.3398091](https://doi.org/10.1145/3377929.3398091). [V] for everything in this group — [PDF](https://research.facebook.com/file/4029628380500072/Open-Source-Evolutionary-Structured-Optimization.pdf)
  - A `Parameter` "defines the search space, but also possibly the mutation or recombination operators".
  - It offers two interfaces to optimisers: one "for converting the values in the search space to a standardized space, in which the data is linearized and reduced so that the initial prior ... is a Gaussian of mean 0 and standard deviation 1", and one "providing mutation and recombination methods".
  - The same mutation operator "can be used by arbitrary optimization algorithms".
- Parameter classes in three groups:
  - data: `Array`, `Scalar`, `Log`, with bounds and standard deviation;
  - choice: `Choice` for unordered and `TransitionChoice` for ordered categoricals;
  - containers: `Tuple`, `Dict` and `Instrumentation(*args, **kwargs)`, where non-Parameter members are constants.
- The bijection to R^n is "not strictly required". Parameter classes that support only the mutation/recombination interface drop "compatibility with part of the optimizer implementations".
- Worked embedding: Dict(Log, Array(2), Choice of 3) lives in R^6 (2 array coordinates, 3 choice weights, 1 linearised log).
- Mutations attached by type: Array gets Gaussian mutation, Log gets log-normal, Choice gets "Gaussian mutation of weights + sampling".
- Custom operators in the photonics case:
  - Gaussian, Cauchy and localised Gaussian noise, a layer-wise translation, and a jump of up to 5 layers;
  - a layer-wise 2-point crossover versus a "ravel" 2-point crossover that ignores the 2-D structure.
  Only a custom "Pairwise-ES" uses these; CMA-ES, PSO and DE "only use the mapping API". The same Parameter also records ancestry for analysis with HiPlot.
- Docs: `Choice` samples "as the softmax of the values on these dimensions". The docs warn: "this process is non-deterministic and makes the function evaluation noisy". `TransitionChoice` uses one index dimension so that transitions are local, and parametrization "is still a work in progress". [V] — [Nevergrad docs](https://facebookresearch.github.io/nevergrad/parametrization.html)
- Nevergrad's algorithm-selection wizard (Shiwa) has three relevant features. [V] — [Liu et al., "Versatile Black-Box Optimization", GECCO '20, arXiv:2004.14014](https://arxiv.org/abs/2004.14014)
  - It chooses algorithms by descriptors that include whether variables are discrete, ordered, and whether the domain is "metrizable". A domain is called metrizable "if it is equipped with a meaningful, non-binary, metric", and "any domain containing a discrete categorical variable is considered as non-metrizable".
  - It converts discrete problems to noisy continuous ones by softmax over one weight per value, judged to "perform well for discrete variables with more than 2 values".
  - Binary alphabets are left to classical discrete EAs.

**pymoo**
- Blank and Deb, "pymoo: Multi-objective Optimization in Python", *IEEE Access* 8:89497–89509 (2020), DOI [10.1109/ACCESS.2020.2990567](https://doi.org/10.1109/ACCESS.2020.2990567): "algorithms can be modified/extended by supplying custom operators". [V] — [arXiv:2002.04504](https://arxiv.org/abs/2002.04504)
- Mixed variables: a problem declares `vars = {name: Real(bounds) | Integer(bounds) | Choice(options) | Binary()}` and individuals are dicts keyed by name. `MixedVariableGA` "defines different operators for each variable type". Multi-objective use goes through `MixedVariableMating` or a survival operator such as `RankAndCrowdingSurvival`. [V] — [pymoo docs](https://pymoo.org/customization/mixed.html)
- Source of `pymoo/core/mixed.py` (main branch, read 2026-11-20). [V] — [source](https://github.com/anyoptimization/pymoo/blob/main/pymoo/core/mixed.py)
  - `MixedVariableMating` groups variables by class.
  - Default crossovers: Binary uses UX, Real uses SBX, Integer uses SBX with `vtype=float` and `RoundingRepair`, Choice uses UX.
  - Default mutations: BFM (bit flip), PM, PM with rounding, and `ChoiceRandomMutation`.
  - Each Choice variable is recombined on its own, while other types are recombined as a block.
  - It asserts that every crossover takes 2 parents and yields 2 offspring.
  - `MixedVariableSampling` calls each variable's own `sample`.
  - Duplicate elimination compares dicts for equality.

**DEAP, ParadisEO, jMetal, HeuristicLab, ECJ, EvoTorch, pyribs**
- DEAP. [V] — [DEAP tutorial](https://deap.readthedocs.io/en/master/tutorials/basic/part1.html)
  - `creator.create(name, base, **attrs)` builds individual and fitness classes, with fitness weights negative for minimising.
  - `toolbox.register(alias, func, *fixed_args)` works like `functools.partial`.
  - Initialisers: `initRepeat` (one generator n times), `initIterate` (a whole iterable, used for permutations and GP trees) and `initCycle` (cycles through a tuple of generators).
  - A heterogeneous individual is "of the form [int float int float ... int float]".
  - Operators are registered by the same mechanism; the operator tutorial itself was not read.
- ParadisEO, GECCO 2021 Companion, DOI 10.1145/3449726.3463276. [V] for the architecture below; [S] for the DOI — [arXiv:2105.00420](https://arxiv.org/abs/2105.00420)
  - Built on four design patterns: Functor, Strategy, Generic Type and Factory.
  - "Almost all the operators in Paradiseo are defined over a EOT template holding the encoding", so "the user can provide her own data structure, without major redesign" and "any operator deep in the call tree may have access to any specific interface of the encoding".
  - It favours composition over inheritance and provides generic encodings (numerical vectors, binary vectors, trees, permutations).
- jMetal: `Solution<T>` with Binary, Integer, Double, Permutation and Sequence solutions and `CompositeSolution`, where "A solution is composed of list of solutions, thus allowing to mix different types of encodings in a single solution." [V] — [jMetal docs](https://jmetal.readthedocs.io/en/latest/encodings.html). jMetalPy added its own CompositeSolution in v1.5.4. [S] — [PyPI](https://pypi.org/project/jmetalpy/1.5.4/)
- HeuristicLab's `MultiEncoding` is "a combined encoding of multiple simpler encodings", with `AddBinaryVector`, `AddRealVector` and `AddPermutation` builders that check names are unique, and it is used by its programmable problems. [S] — [source](https://dev.heuristiclab.com/trac.fcgi/browser/branches/ProgrammableProblem/HeuristicLab.Problems.Programmable/3.3/Encodings/MultiEncoding.cs?rev=11571); [arXiv:2310.14811](https://arxiv.org/pdf/2310.14811)
- ECJ: each subpopulation has individuals, a `Species` and a prototypical `Fitness`. Breeding pipelines connect selection and variation, and nearly all classes are chosen at run time from parameter files. [S] — [ECJ overview](https://people.cs.gmu.edu/~eclab/projects/ecj/docs/overview.html)
- EvoTorch (Toklu et al., 2023): a `Problem` declares the solution structure and dtype. Non-numeric solutions are stored in an `ObjectArray`. [S] — [arXiv:2302.12600](https://arxiv.org/abs/2302.12600)
- pyribs (Tjanaka et al., GECCO '23, DOI 10.1145/3583131.3590374): the RIBS architecture of archive, emitters and scheduler. It "focuses solely on optimizing fixed-dimensional continuous domains". [S] — [arXiv:2303.00191](https://arxiv.org/abs/2303.00191)

**Hyperparameter-optimisation search-space languages**
- Optuna (Akiba et al., KDD 2019): a "define-by-run API that allows users to construct the parameter search space dynamically". [V] — [arXiv:1907.10902](https://arxiv.org/abs/1907.10902). Its distributions are `FloatDistribution`, `IntDistribution` and `CategoricalDistribution`, and users "should not use distribution classes directly" but call `trial.suggest_*`. [V] — [Optuna docs](https://optuna.readthedocs.io/en/stable/reference/distributions.html)
- ConfigSpace. [V] — [ConfigSpace guide](https://automl.github.io/ConfigSpace/latest/guide/); [ConfigSpace](https://automl.github.io/ConfigSpace/latest/)
  - Conditions constrain "a child hyperparameter ... on its parent": `EqualsCondition`, `InCondition`, `OrConjunction`.
  - Forbidden clauses (`ForbiddenEqualsClause`, `ForbiddenAndConjunction`) exclude invalid combinations.
  - Conditional hyperparameters are sampled "only if their condition is met".
  - A warning: do not put Equals/In conditions on floats, because of rounding.
  - It is used by SMAC3, BOHB and auto-sklearn.
- Ax. [S] — [Ax #2197](https://github.com/facebook/ax/issues/2197); [Ax #140](https://github.com/facebook/Ax/issues/140); [Ax #2481](https://github.com/facebook/Ax/issues/2481); [Ax source](https://ax.readthedocs.io/en/0.5.0/_modules/ax/core/search_space.html)
  - A `ChoiceParameter` can map values to `dependents` (child parameters) to form a `HierarchicalSearchSpace`.
  - An older issue says the tree needed a single root, only Sobol generation was supported, and Bayesian optimisation worked by flattening.
  - Plotting fails for hierarchical spaces in one reported version.

**Mixed-integer and mixed-category model-based search**
- Hamano, Saito, Nomura and Shirakawa, "CMA-ES with Margin: Lower-Bounding Marginal Probability for Mixed-Integer Black-Box Optimization", GECCO '22, DOI [10.1145/3512290.3528827](https://doi.org/10.1145/3512290.3528827). [V] — [arXiv:2205.13482](https://arxiv.org/abs/2205.13482)
  - **The problem:** with plain discretisation, the variance of integer coordinates shrinks below the discretisation granularity before the optimum is reached, which is worse for binaries.
  - **The fix:** sample from N(m, σ²ACAᵀ) with a diagonal A, and correct A and m so that the probability of generating values other than the dominant integer stays ≥ α. For binaries the margin is α; for integers it is α/2 on each side.
  - The idea comes from PBIL's restriction of marginals to [1/N, 1 − 1/N]. Because the correction is an affine transform of the samples, the CMA-ES updates themselves are unchanged.
  - Their α sweep (α = N^{−m}λ^{−n}) showed all trials failing for large α (N^{−0.5} or λ^{−0.5}) and degradation below (Nλ)^{−1}. The repository recommends α = 1/(Nλ) [S] — [GitHub](https://github.com/EvoConJP/CMA-ES_with_Margin).
- Hamano, Nomura, Saito, Uchida and Shirakawa, "CatCMA with Margin for Single- and Multi-Objective Mixed-Variable Black-Box Optimization" (arXiv:2504.07884, 2025). [V] — [arXiv:2504.07884](https://arxiv.org/abs/2504.07884)
  - It optimises continuous, integer and categorical variables jointly with a joint distribution of a multivariate Gaussian and categorical distributions.
  - It reports beating state-of-the-art Bayesian optimisation on single-objective benchmarks.
  - It extends to multi-objective problems via Sofomore (COMO-CatCMAwM).
- Li, Emmerich, Eggermont, Bäck, Schütz, Dijkstra and Reiber, "Mixed Integer Evolution Strategies for Parameter Optimization", *Evolutionary Computation* 21(1):29–64 (2013), DOI 10.1162/EVCO_a_00059. [S] — [researchr](https://researchr.org/publication/LiEEBSDR13/bibtex)
- The CEGO implementation of MIES. [V] for CEGO's implementation; how far it departs from the paper is stated by CEGO — [CEGO optimMIES](https://search.r-project.org/CRAN/refmans/CEGO/html/optimMIES.html)
  - Reals: independent normal perturbations with per-parameter step sizes.
  - Ordinal integers: "the difference of geometric distributions".
  - Categoricals: "changed at random, with a self-adapted probability".
  - Recombination is dominant for solution parameters and intermediate for strategy parameters.
  - Out-of-bounds values are clipped, which differs from the paper's transformation.

**Mixed-space Bayesian optimisation** [S] for this group
- CoCaBO (Ru et al., ICML 2020): a multi-armed bandit chooses categorical values, a GP handles continuous values, and a kernel shares information across categorical variables — [arXiv:1906.08878](https://arxiv.org/abs/1906.08878).
- CASMOPOLITAN: trust regions and a transformed overlapped kernel with per-category lengthscales — [arXiv:2508.06847](https://arxiv.org/html/2508.06847v2).
- BOPS (Deshwal, Belakaria, Doppa and Kim, AAAI 2022): a GP with Kendall kernels (BOPS-T, with tractable acquisition optimisation and sublinear regret) or Mallows kernels (BOPS-H), beating COMBO — [arXiv:2112.01049](https://arxiv.org/abs/2112.01049).
- A survey of high-dimensional discrete-sequence BO notes "highly heterogeneous experimental set-ups ... and technical barriers for the replicability" — [arXiv:2406.04739](https://pdf.arxiv.org/pdf/2406.04739).

### Inferences
- Every mature framework separates three things:
  1. the **value** of a genotype part;
  2. its **variation operators**;
  3. an optional **embedding** into R^n for numeric optimisers.
  Nevergrad makes all three properties of the typed parameter, which is the closest existing design to a "typed representation space". pymoo dispatches operators by variable class instead.
- Containers are always **products**: tuple, dict, composite solution, multi-encoding. **Sums** (alternatives with different sub-structure) appear only in the hyperparameter-optimisation languages, as conditional activation. Evolutionary frameworks have no first-class sum type with branch-aware recombination.
- CMA-ES with margin and CatCMA show that, for model-based search, a heterogeneous product is best handled by a **joint distribution** with type-specific marginals and a floor on their entropy, rather than by type-specific operators bolted onto a Gaussian.
- MIES shows the operator-based alternative: per-type mutation with per-gene self-adapted step sizes or probabilities.

### Gaps
- I did not read the jMetal composite crossover/mutation implementation, the HeuristicLab operator wiring for MultiEncoding, ECJ's VectorSpecies per-gene settings, or the EvoTorch and pyribs papers in full. Those claims are [S].
- I did not verify how SMAC3 encodes inactive hyperparameters in its random-forest surrogate (I believe NaN or default imputation [M]), nor how Optuna's TPE handles conditional parameters (independent versus multivariate/group sampling [M]).
- I did not read Nevergrad's API for `set_mutation` with custom callables or its bounds methods (bouncing, clipping, arctan, tanh); the docs page fetched did not cover them.

---

## Key Question 4: Can operators and random generators be derived automatically from a type or grammar (algebraic data types, combinatorial species), and with what guarantees (uniformity, size control)?

### Takeaway
Generators can be derived with strong guarantees. A Boltzmann sampler is built mechanically from a specification by union, product, sequence, set and cycle. It is **exactly uniform among objects of the same size** and runs in time linear in the output, given generating-function values at the control parameter x. Expected size is tuned by x, and exact or windowed size is obtained by rejection. How well the size concentrates depends on the class: "bumpy", "flat" or "peaked" profiles, with trees peaked at small sizes.

QuickCheck's type-class generators (`Arbitrary`, `sized`, `oneof`/`frequency`) are the practical model for type-derived generation with a size bound, but carry **no uniformity guarantee**.

Variation operators are less developed. Strongly typed GP enforces type-correct crossover and mutation. Polytypic GP derives initialisation and subtree mutation for arbitrary ADTs at compile time, but not crossover.

### Cited Findings

**Boltzmann samplers and analytic combinatorics**
- Duchon, Flajolet, Louchard and Schaeffer, "Boltzmann Samplers for the Random Generation of Combinatorial Structures", *Combinatorics, Probability and Computing* 13(4–5):577–625 (2004), DOI [10.1017/S0963548304006315](https://doi.org/10.1017/S0963548304006315). [V] for everything in this group — [PDF](https://algo.inria.fr/flajolet/Publications/DuFlLoSc04.pdf)
  - A Boltzmann model on a class C depends on a real control parameter x > 0 and puts probability "essentially proportional to x^{|ω|}" on an object of size |ω|. In the ordinary (unlabelled) case P_x(γ) = x^{|γ|}/C(x); in the labelled case the exponential GF is used.
  - "objects with the same size invariably occur with the same probability"; conditioned on size n, the sampler "draws uniformly at random an object of size n".
  - Samplers "can be derived systematically (and simply)" for classes built from general-purpose constructions: disjoint union, Cartesian product and sequence in the unlabelled case, plus labelled product, sets and cycles in the labelled case.
  - **Theorem 3.1:** for an unlabelled class specified (possibly recursively) by unions, products and sequences, and for x in (0, ρ_C), given an oracle for the generating-function values at x, the assembled generator's cost in real-arithmetic operations is linear in the size of its output.
  - **Size control:** E_x(N) strictly increases in x, with x·dE_x(N)/dx = V_x(N). Examples: binary words at x = 0.49505 have mean size 100 and standard deviation 100.5; cyclic permutations at mean 100 have standard deviation 234.
  - Size profiles are "bumpy" (set partitions), "flat" near the critical value (surjections, words) or "peaked" at the origin (trees, which produce very small objects with high probability). Sections 6–7 exploit these profiles.
  - A tolerance "of a few per cent" usually suffices; "one can always pile up a rejection method" for exact size.
  - Complexity table: O(1) real constants of preprocessing memory, O(|ω|) per free generation, O(n) with tolerance, against the recursive method's O(n) tables of large integers and superlinear generation.
  - The paper compares itself to the recursive method of Nijenhuis–Wilf and of Flajolet–Zimmermann–Van Cutsem.
- Flajolet, Fusy and Pivoteau (2007) extend Boltzmann sampling to unlabelled structures with multiset, powerset and cycle constructions. [S] — [core.ac.uk listing](https://core.ac.uk/download/48335108.pdf)
- Flajolet and Sedgewick, *Analytic Combinatorics* (Cambridge UP, 2009): the symbolic method that maps constructions to generating-function equations, which underlies these samplers. [M] — [book PDF](https://algo.inria.fr/flajolet/Publications/book.pdf)
- Bendkowski, "Automatic compile-time synthesis of entropy-optimal Boltzmann samplers" (arXiv:2206.06668, 2022): a Template Haskell framework that synthesises "efficient, entropy-optimal samplers" for user-declared ADTs, letting users "control the mean size and constructor frequencies", with thousands of parameters and objects of millions in size. [V, abstract only] — [arXiv:2206.06668](https://arxiv.org/abs/2206.06668)

**Property-based-testing generators**
- Claessen and Hughes, "QuickCheck: A Lightweight Tool for Random Testing of Haskell Programs", ICFP 2000, pp. 268–279. [V] for the content below — [PDF](https://www.cs.tufts.edu/comp/150FP/archive/john-hughes/quick.pdf); [Chalmers record](https://research.chalmers.se/publication/237427)
  - `class Arbitrary a where arbitrary :: Gen a`.
  - `oneof` chooses among generators uniformly; `frequency` chooses with weights.
  - Generators carry a size bound through `sized :: (Int -> Gen a) -> Gen a`. The binary-tree example uses `frequency [(1, Leaf), (4, Branch (arbTree (n div 2)) (arbTree (n div 2)))]`.
  - The size bound is "an extra, global parameter which every test data generator may access". The authors "do not attempt to 'divide the size bound among the generators'".
  - The DOI is listed inconsistently as 10.1145/357766.351266 or 10.1145/351240.351266 [S].
- Shrinking is "mentioned almost as an afterthought" in the original paper. [S] — [commentary](https://alastairreid.github.io/RelatedWork/papers/claessen:icfp:2000)
- Later work on derived generators: [S]
  - Mista, Russo and Hughes (Haskell Symposium 2018) predict the constructor distribution of derived QuickCheck generators at compile time from branching-process theory — [PDF](https://users.cs.northwestern.edu/~robby/icfp2018/icfpws18haskell/icfpws18haskellmain-id21-p.pdf).
  - Claessen, Duregård and Pałka generate uniformly distributed values of *constrained* ADTs — [JFP PDF](https://cambridge.org/core/services/aop-cambridge-core/content/view/567438B9A7FABDD0F191FF65DAEA7005/S0956796815000143a.pdf/generating-constrained-random-data-with-uniform-distribution.pdf).
  - The `generic-random` library derives `Arbitrary` instances generically; its Boltzmann modules moved to `boltzmann-samplers` — [Stackage](https://www.stackage.org/package/generic-random).

**Type-driven GP**
- Montana, "Strongly Typed Genetic Programming", *Evolutionary Computation* 3(2):199–230 (1995), DOI [10.1162/evco.1995.3.2.199](https://doi.org/10.1162/evco.1995.3.2.199). It enforces data-type constraints on generated programs, and its generic functions and generic data types make it "more powerful than other approaches to type-constraint enforcement". It is demonstrated on least-squares regression, a Kalman filter, NTH and MAPCAR. [S] — [gp-bibliography](https://gpbib.cs.ucl.ac.uk/gp-html/montana_stgpEC.html)
- Swan, Krawiec and Ghani, "Polytypic Genetic Programming", EvoApplications 2017, LNCS pp. 66–81, DOI [10.1007/978-3-319-55792-2_5](https://doi.org/10.1007/978-3-319-55792-2_5). [V] — [White Rose eprint](https://eprints.whiterose.ac.uk/117964/)
  - Polytope (Scala) uses datatype-generic programming over products (Shapeless `HList`) and coproducts (`:+:`, `CNil`).
  - Operators are defined by induction over the atomic, product and coproduct cases, and Shapeless converts an ADT to its generic sum-of-products form.
  - It provides 'full' initialisation and subtree-replacing mutation for any ADT, with type rules "automatically enforced by the compiler, even for search operations such as mutation".
  - It removes the per-API boilerplate (EpochX needed 301 lines for the Boolean domain) with comparable runtime.
  - Limitation: "The current version of Polytope lacks both crossover and any built-in mechanism for bloat control", both of which the experiments showed to be desirable.
- Kocsis and Swan (J. Automated Reasoning, 2017): a mutation-like operator that uses proof search in the sequent calculus for semantics-preserving transformations between ADTs, with equational representations "derived automatically from the type declarations". [S] — [PMC](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6044322/)
- Ballesteros, Benac-Earle, Mariño, Fredlund and Herranz, "Generation of algebraic data type values using evolutionary algorithms", *JLAMP* 143:101022 (2025), DOI 10.1016/j.jlamp.2024.101022. It evolves values of arbitrary ADTs, including constrained ones (red-black trees), and reports better diversity and larger valid values than simple random sampling. [V, abstract page] — [gp-bibliography](https://gpbib.cs.ucl.ac.uk/gp-html/Ballesteros_2025_jlamp.html)

### Inferences
- For a typed genotype language with constructors {atom, product, sum, sequence, set/multiset, cycle}, a **Boltzmann sampler is derivable mechanically**: a Bernoulli choice for sums, independent calls for products, a geometric length for sequences, and Poisson-type draws for sets. It is uniform conditioned on size, with mean size set by x. This is the principled "random initialisation" and "random subtree" generator.
- Recursive types (trees) have peaked size distributions, so they need rejection or a singular-sampler tuning to reach a target size. This is a known pitfall of naive grow/full initialisation as well.
- QuickCheck shows the engineering pattern: a type class (or trait or interface) per type giving `arbitrary` under a size bound. The same dictionary can carry `shrink` (simplification toward smaller values), `mutate` and `recombine`.
- Shrinking is a natural **simplification/repair** operator for evolutionary search, e.g. the simplification GSGP requires and bloat control. This is my inference; no source evaluates QuickCheck shrinkers as EA operators.
- For variation operators derived from an ADT, the induction Polytope uses (atomic, product, coproduct cases) yields subtree mutation directly.
  - **Crossover** can be derived as "choose a position; if both parents have the same constructor there, recurse component-wise (product crossover); otherwise take one parent's subtree". This is homologous crossover and is geometric under a structural edit distance (Moraglio ch. 7–8 discuss homologous crossover as geometric).
  - Koza's subtree swap is not geometric. A derived crossover should therefore be homologous if geometric guarantees are wanted.

### Gaps
- I did not read Flajolet–Fusy–Pivoteau 2007 or the *Analytic Combinatorics* book this session, so the unlabelled multiset and powerset sampler details are [S]/[M].
- I found no paper that derives *both* mutation and crossover from an ADT declaration with a proven property (geometricity, respect or uniformity). This appears to be an open design space.
- I did not verify Hypothesis's (Python) internal shrinking model (shrinking the choice sequence rather than the value), which is relevant to designing generic shrinkers [M].

---

## Key Question 5: What are known pitfalls (low locality, redundancy, invalid offspring, scale mismatch between parts) when composing heterogeneous genotypes?

### Takeaway
The verified pitfalls:
- **low locality** (mutation becomes random search);
- **non-synonymous redundancy** (has the same effect);
- **non-separable formae** (recombination cannot both respect and assort);
- **variance collapse below the grid** for discretised integers (fixation);
- **noisy evaluation from stochastic decoding** of categoricals (softmax sampling);
- **centre bias** of crossover on bounded domains;
- **exponential growth** under semantic crossover;
- **locality limits on curved manifolds** (injectivity radius);
- **brittle handling of inactive/conditional parameters**;
- **the implicit sum metric of product operators**, which makes parts with large ranges dominate unless they are standardised.

### Cited Findings
- **Redundancy.** "Representations are redundant if the number of genotypes is larger than the number of phenotypes", which adds plateaus. [V] — [Rothlauf tutorial](https://gpbib.cs.ucl.ac.uk/gecco2006etc/papers/tut103.pdf)
  - The literature disagrees on whether redundancy helps or harms, and Rothlauf cites Knowles and Watson (2002) as showing that the hoped-for benefit of neutral networks "is not true".
  - The useful distinction is between **synonymous** redundancy (genotypes for the same phenotype are similar) and **non-synonymous** redundancy, which does "not allow guided search", makes "EA search becomes random", and has a "similar effect as low locality".
  - The model uses order of redundancy k_r and the over- or under-representation r of the optimum.
  - Conflict: one search summary stated redundancy as phenotypes exceeding genotypes. The primary tutorial says the reverse, so the tutorial is followed here.
- **Locality.** High-locality representations do not change problem difficulty, while low-locality ones make mutation-based search more random. [S] — [Rothlauf, GECCO 2003](https://www.cs.york.ac.uk/rts/docs/GECCO_2003/bibs/2724/27241608.htm). Representations that distort distances change difficulty. [V] — [tutorial](https://gpbib.cs.ucl.ac.uk/gecco2006etc/papers/tut103.pdf)
- **Non-separability.** Adjacency (edge) formae on permutations are non-separable. [V] — [Radcliffe 1994](https://www.stochasticsolutions.com/pdf/amai94.pdf)
- **Non-geometric crossovers.** Order crossover, subtree swap and extended line recombination have no associated metric landscape, so smoothness reasoning cannot be applied to them. [V] — [Moraglio thesis §14.4](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Discretisation fixation.** Under rounding of a Gaussian, integer coordinates' variance falls below the granularity before the optimum is reached, and the search stalls; binaries are worst. [V] — [Hamano et al. 2022](https://arxiv.org/abs/2205.13482)
- **Stochastic decoding.** Softmax decoding of `Choice` "is non-deterministic and makes the function evaluation noisy". [V] — [Nevergrad docs](https://facebookresearch.github.io/nevergrad/parametrization.html)
- **Bounded-domain bias.** Uniform geometric crossover on a box is biased toward the centre. [V] — [Moraglio thesis §4](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Size explosion.** In GSGP "the size of individuals grows exponentially with the number of generations", so offspring must be simplified. [V] — [GSGP](https://www.cs.put.poznan.pl/kkrawiec/wiki/uploads/Research/2012PPSN.pdf). Polytope lacked bloat control and crossover, and this hurt. [V] — [Polytope](https://eprints.whiterose.ac.uk/117964/)
- **Curvature.** High scalar curvature gives a small injectivity radius and therefore a restricted search region per iteration for Riemannian-adapted samplers. [V] — [Fong & Tino](https://arxiv.org/abs/1908.06783)
- **Conditional spaces.** [V] — [ConfigSpace guide](https://automl.github.io/ConfigSpace/latest/guide/)
  - Equality conditions on floats can misfire because of rounding.
  - Some states are simply invalid and must be excluded by forbidden clauses.
  In Ax, hierarchical spaces have had plotting failures and validation errors when inactive children are omitted or passed as None, and early versions supported Bayesian optimisation only by flattening. [S] — [Ax #140](https://github.com/facebook/Ax/issues/140); [Ax #2481](https://github.com/facebook/Ax/issues/2481)
- **Dependent parts.** In a "dependent crossover" (e.g. graph structure plus weight vector), recombining one projection constrains the other. Feasibility then needs coordinated operators or repair. [V] — [Moraglio thesis §9.2.4](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- **Structure-blind recombination.** Nevergrad's photonics study shows a "ravel" 2-point crossover that ignores the layer structure (it may take only one parent's permittivities), set against a layer-wise crossover that preserves it. [V] — [Rapin et al. 2020](https://research.facebook.com/file/4029628380500072/Open-Source-Evolutionary-Structured-Optimization.pdf)
- **Type dispatch assumptions.** pymoo's mixed mating assumes every crossover is 2-parent/2-offspring, and recombines each categorical separately (no joint dependency across categories). [V] — [pymoo source](https://github.com/anyoptimization/pymoo/blob/main/pymoo/core/mixed.py)
- **Repair by renormalisation** is the common simplex fix in portfolio GAs, and random Dirichlet weights can be disruptive. [S] — [arXiv:1411.7494](https://arxiv.org/pdf/1411.7494); [arXiv:2512.01249](https://arxiv.org/pdf/2512.01249)
- **Large margins hurt too.** Setting the margin too large (α ≈ N^{−0.5}) made every trial fail in Hamano et al.'s sweep. [V] — [Hamano et al. 2022](https://arxiv.org/abs/2205.13482)

### Inferences
- **Scale mismatch** follows formally from Moraglio's product theorem: product crossover is geometric under d₁ + d₂ + …, so whichever part has the largest distances dominates both the metric and the "segment". For mutation, one global σ over a concatenated embedding gives parts with different natural scales very different effective step sizes.
  - Fixes: per-part standardisation (Nevergrad's N(0,1) prior per block), per-part step sizes (MIES), or full covariance adaptation in a standardised embedding (CMA-ES with margin).
- **Invalid offspring** arise when the product is not the true space, i.e. under constraints, dependencies or conditionality. Three remedies appear in the sources:
  1. Construct offspring only in the valid set (closed operators, e.g. geometric crossovers on permutations, Dirichlet or ilr mutation on the simplex).
  2. Repair or project: rounding repair in pymoo, clipping in CEGO's MIES, renormalisation on the simplex, retraction on manifolds.
  3. Penalise or forbid: ConfigSpace's forbidden clauses.
- Inactive parts of a sum or conditional type should not drift silently. A framework needs a policy:
  - keep and mutate inactive values (they are latent and may help when reactivated);
  - reset them to defaults; or
  - exclude them from distances and from the model.
  ConfigSpace samples children only when their conditions hold. Ax's issues suggest that representing "inactive" consistently across components is error-prone.

### Gaps
- I found no systematic empirical study comparing scale-normalisation schemes for heterogeneous product genotypes in EAs.
- I did not verify Rothlauf's quantitative population-sizing model for redundant representations (Rothlauf & Goldberg 2003) beyond the tutorial's description.

---

## What this means for a universal framework

### Takeaway
A universal evolutionary framework should treat each representation as a **typed space object** that supplies:
- a standard set of capabilities (sample, mutate, recombine, distance, repair or project, local chart with retract and transport, standardised embedding, size, shrink);
- checkable **laws** (purity, segment and ball membership, retraction identities, idempotent repair).

Spaces compose by a small algebra of constructors (product, sum or choice with conditional activation, sequence, set and multiset, permutation, simplex, manifold, graph, constrained subspace). From this algebra, generators (Boltzmann samplers), product operators (geometric under the sum metric) and standardisation should be **derived generically**, while specialised spaces may override any capability.

### Cited Findings
The design below rests on the following verified elements, all cited above:
- geometric mutation and crossover via balls and segments, and the product theorem (Moraglio thesis);
- respect, transmission and assortment derived from formae, and RAR as universal recombination (Radcliffe 1994);
- convex-search runtime results (Moraglio & Sudholt 2015);
- tangent-space sampling, retraction and transport, with a concrete manifold interface (Manopt.jl; Fong & Tino);
- margins on discrete marginals (Hamano et al. 2022) and joint Gaussian–categorical models (CatCMAwM 2025);
- a standardised N(0, I) embedding plus per-parameter operators (Nevergrad);
- per-type operator dispatch (pymoo);
- generic encodings with composed operator objects (ParadisEO EOT; jMetal CompositeSolution; DEAP initCycle);
- conditions and forbidden clauses (ConfigSpace), define-by-run (Optuna) and dependents (Ax);
- uniform-given-size Boltzmann generation from constructions (Duchon et al. 2004);
- the `Arbitrary`/`sized` type-class pattern (QuickCheck);
- compile-time generic operators over sums of products (Polytope).

### Inferences: a concrete architecture

**1. The contract of a space.** A space `S` with value type `T` provides the following. Required capabilities first, then optional ones; a capability that is missing is declared so, and algorithms that need it refuse the space, as Nevergrad does with optimisers that need the R^n mapping.

Required:
- `sample(rng, size_hint) -> T`: a prior or initial distribution. For constructible types the default is a Boltzmann sampler.
- `mutate(rng, x, scale) -> T`: a geometric ε-mutation with ε controlled by `scale`. `scale` is a part-local, standardised step size.
- `distance(x, y) -> R≥0`: a metric. Its "natural unit" is declared, e.g. the expected distance between two prior samples, so that compositions can normalise.
- `is_valid(x)`: membership in the space.

Optional:
- `recombine(rng, parents) -> T`: by default a geometric crossover, i.e. a sample from the segment [x; y]_d. Declares its arity (pymoo hard-codes 2→2; a universal framework should not) and whether it is **geometric**, **pure** and **respectful**.
- `repair(x) -> T` or `project(x) -> T`: idempotent, returning x when x is valid. Examples: rounding, clipping, simplex projection, manifold projection.
- `chart`: for manifolds and other continuous spaces, `log_x(y) ∈ T_x`, `retract_x(v)` (exp or retraction), `transport_{x→y}(v)`, `basis(x)`, `dim`, `trust_radius(x)` (injectivity radius or a bound). This mirrors Manopt's requirements.
- `embed(x) -> R^k` and `unembed(z) -> T`: a standardised embedding with a N(0, I) prior, as Nevergrad does. It lets CMA-ES, DE and PSO work on any space. A space must state whether it is bijective, surjective with a decoder, or stochastic (softmax). Stochastic decoding makes evaluation noisy, so the framework should flag it and prefer deterministic argmax decoding with a margin.
- `marginal_floor(α)`: for discrete parts, a CMA-ES-with-margin-style lower bound on the probability of leaving the current value.
- `size(x)` and `shrink(x) -> Iterable[T]`: a measure of size for Boltzmann tuning and bloat control, and QuickCheck-style simplification toward smaller values. This is also the hook for GSGP-style simplification.

**2. Laws as checks** (property-based tests of every space, written once):
- purity: `recombine(x, x) = x`;
- geometricity: for every child c, `d(x,c) + d(c,y) = d(x,y)` within tolerance;
- `d(x, mutate(x, s)) ≤ ε(s)`;
- `repair(repair(x)) = repair(x)`, and `repair(x) = x` when x is valid;
- `retract_x(0) = x`;
- `log_x(retract_x(v)) ≈ v` within the trust radius;
- transport preserves inner products when it is parallel transport;
- `unembed(embed(x)) = x` for bijective embeddings;
- the metric axioms on random triples.
Moraglio's non-geometricity proofs use exactly purity and convergence failures, so the same checks diagnose user-supplied operators.

**3. Base spaces and their natural operators** (from the sources above):

| Space | Distance | Mutation | Recombination | Repair / chart |
|---|---|---|---|---|
| R^n (box) | Euclidean or Manhattan (stated) | Gaussian, per-part σ, CMA in the embedding | line, box or discrete recombination (all geometric) | clip or reflect; beware centre bias |
| Integers (ordinal) | \|a−b\| | difference of geometrics (MIES), or Gaussian+rounding with a margin | box or segment | rounding (pymoo RoundingRepair) |
| Categorical | discrete metric | resample with probability p (self-adapted, floor α) | discrete-metric geometric crossover: offspring ∈ {parents} | none |
| Ordered categorical | path metric on the index | local index steps (TransitionChoice) | segment of indices | none |
| Simplex Δ^{D−1} | Aitchison (ilr Euclidean) | Gaussian in ilr (logistic-normal), or Dirichlet centred on the parent | convex combination in ilr (the Aitchison segment) | ilr is a bijective chart; Euclidean projection only if the boundary matters |
| Sphere / SO(3) / Stiefel / Grassmann | geodesic | tangent Gaussian, then retract | sample on the minimising geodesic within the trust radius | retraction + transport (Manopt contract) |
| Permutation | swap, Hamming or Kendall (stated) | swaps or insertions (ball of the stated metric) | PMX or cycle crossover (geometric under swap/Hamming); avoid OX if geometric guarantees are wanted | closed operators |
| Set / multiset | symmetric difference | add, remove, swap | Radcliffe's set recombination (respect shared elements, assort the rest) | cardinality repair |
| Sequence (variable length) | edit distance | edit moves | alignment-based homologous crossover (geometric under edit distance) | none |
| Graph | edge insertion/deletion distance (after matching) | edge edits | match vertices, then take an intermediate graph on a shortest edit path | constraint repair |

**4. Constructors (the algebra of composition):**
- **Product** `Π_i S_i` (named or positional; Nevergrad Dict and Tuple, jMetal CompositeSolution).
  - Derived `distance = Σ_i w_i d_i / u_i`, where u_i is the declared unit of part i and w_i a weight (default 1).
  - Derived `recombine` is component-wise. It is geometric under the derived distance by Moraglio's Theorem 9.2.2, and stays geometric even if the components are coupled (Corollary 9.2.3).
  - Derived `mutate` mutates each part with probability p_i at a part-local scale (per-part self-adaptation as in MIES).
  - Derived `embed` concatenates part embeddings.
  - Derived `sample` is independent per part.
- **Sum / choice** `Σ_k S_k` (tagged union; conditional activation as in ConfigSpace, Ax `dependents`, Optuna define-by-run).
  - Derived `distance((k,x),(k′,y))` is `d_k(x,y)/u_k` if k = k′, else a declared constant c ≥ max within-branch diameter. This keeps it a metric.
  - Derived `mutate`: switch branch with probability p_switch, floored at α as with the margin; on a switch, sample the new branch's payload from its prior, or restore a cached latent payload. Otherwise mutate within the branch.
  - Derived `recombine`: same tag means recombine inside; different tags means return one parent. This is geometric under the distance above, since the segment between branches is {x, y}, exactly as with the discrete metric.
  - Policy for inactive payloads: keep latent, reset, or drop. Declared per sum, since the sources show this is a source of bugs.
- **Sequence** `Seq(S)`: a length distribution (geometric for Boltzmann), elementwise mutation, insert/delete, alignment-based crossover.
- **Set** `Set(S)` and **multiset** `MSet(S)`: Radcliffe-style respectful recombination; Boltzmann set and multiset constructions for sampling.
- **Constrained subspace** `S | φ`: a predicate with a declared repair. Otherwise rejection with a counter, as with ConfigSpace forbidden clauses.
- **Dependent product** `Σ_{x:A} B(x)`: the shape of the second part depends on the first, e.g. a graph plus weights. Recombination must be supplied jointly or by a coordinated rule (Moraglio's "dependent crossover").
- **Recursive types** (trees, grammars): generic subtree mutation and homologous crossover by induction over sums of products (Polytope's scheme with crossover added). Boltzmann sampling with rejection or singular tuning, because tree sizes are peaked.

**5. What to derive generically versus specialise:**
- *Derive:*
  - priors (Boltzmann samplers from the constructor tree, with x tuned to a target expected size and optional rejection to a window, uniform given size);
  - product and sum distances, mutations and crossovers (geometric by construction);
  - standardised embeddings (concatenation);
  - per-part step-size bookkeeping;
  - shrinkers (structural: drop sequence elements, replace subtrees with smaller ones of the same type);
  - law checks.
- *Specialise (override):*
  - operators exploiting problem structure (Nevergrad's layer-wise crossover beat structure-blind ones in its photonics case);
  - metric choice for permutations (it encodes which features count as building blocks; Radcliffe shows edge formae are non-separable);
  - manifold charts and transports, which must come from the geometry;
  - simplex charts.
- *Model-based algorithms* (CMA-ES, EDAs, Bayesian optimisation) should consume `embed` plus a **typed marginal policy**: Gaussian for continuous parts, margin-floored discretisation for integers, categorical distributions for choices (CatCMA). They should not use the operator interface. Operator-based algorithms (GA, ES with self-adaptation, island models) consume `mutate`/`recombine`. The same space serves both, as in Nevergrad.

**6. Guarding the known pitfalls by design:**
- *Scale mismatch:* declare units and normalise distances and step sizes per part.
- *Fixation of discrete parts:* floor the probability of leaving the current value (margin α ≈ 1/(Nλ) as recommended by Hamano et al.'s repository; never as large as N^{−0.5}).
- *Noise from stochastic decoding:* flag it, and prefer deterministic decoding.
- *Low locality and non-synonymous redundancy:* measure them empirically per space (fitness–distance correlation under the declared metric; how often a mutation leaves the phenotype unchanged) and report them.
- *Invalid offspring:* closed operators first, then repair, then rejection, with counts recorded.
- *Curvature:* cap σ by `trust_radius`.
- *Recursive bloat:* `size` plus `shrink`.
- *Centre bias of crossover on boxes:* document it, or use reflection-based variants.

### Gaps
- The sum-type metric and branch-switch rule above are my design proposals. No source evaluates them, though they follow Moraglio's discrete-metric reasoning and the margin idea.
- How to adapt a joint covariance across parts that have no common embedding (e.g. a manifold part plus a graph part) is open. CatCMA covers Gaussian plus categorical only; Riemannian CMA-ES covers one manifold.
- No source gives a generic, provably geometric crossover for dependent products or for recursive types beyond homologous crossover. Deriving one is an open problem.
