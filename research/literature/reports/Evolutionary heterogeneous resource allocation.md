# Evolutionary Allocation of Heterogeneous Resources: Typed Spaces, Order, Verified Algebra and Pluggable Evaluators, with Agent, Learned and Quantum-Like Dynamics of Circulation. A Survey

*A survey of how a universal, extensible evolutionary framework for allocating heterogeneous resources should be designed: encodings, variation, repair and benchmarks for allocation of several kinds of resource at once; how the order of allocation arises and how evolutionary computation represents and searches it; representation spaces and their composition (real vectors, integers, categories, permutations, sets, sequences, graphs, simplices, manifolds); the algebra and category theory of resources, allocations and processes, and which of their laws can be decided mechanically; fitness from pluggable evaluators (closed forms, economic models, agent-based models, machine-learned surrogates, language models, people); agent simulation of economic circulation among households, firms, banks and governments; economic agents in learned and geometric spaces (latent spaces, densities, learned simulators, digital twins); classical dynamical laws learned from economic records; and quantum and quantum-like models of economic dynamics; with what the design should be, what must stay a person's judgement, and what a tool must never claim.*

Status: `draft` (a survey for design, 2026-11-20). Activity: research, in service of use. It serves the design of an evolutionary framework for allocating heterogeneous resources on heuresis, kategoria, kinesis, tyche, nomos, the economics domain and the allocation domain (ADR 0136). No decision record has yet been written for the framework itself.

## 0. Scope, Method and Markers

### 0.1 The Question

The request concerned a universal, extensible evolutionary framework for allocating heterogeneous resources, with four requirements:

1. the order of allocation matters: allocating A then B may differ from allocating B then A;
2. each resource type is definable with its own representation space (R^N, integers, sets, sequences, simplices, manifolds and others) and its own algebra (how amounts combine, how they are ordered, whether they divide);
3. each resource type, and what the framework does with it, is formally verifiable by category theory;
4. fitness comes from pluggable evaluators: closed forms, simulations of economic circulation, agent-based models, machine-learned surrogates, language-model evaluators and people.

It asked three further questions, each to be answered with what the methods can and cannot establish:

5. how agent simulations give insight into economic circulation;
6. what learned and geometric representations of agents (latent spaces, densities, learned dynamics) offer;
7. how classical, and quantum or quantum-like, economic dynamical models are learned from observed data.

The survey is written for an infrastructure whose foundations already hold much of what such a framework needs:

- **heuresis:** an evolutionary loop over any genotype with a strategy at every stage, the ask–tell protocol, proposals from outside credited, resumable state; real, integer, bit-string and permutation variation; many objectives and quality-diversity; Gaussian-process surrogates;
- **kategoria:** finite categories, preorders as categories, products, functors verified, symmetric monoidal theories with string diagrams and models in matrices, every claim answered as holds, fails or undecided;
- **kinesis:** declared and learned dynamical systems, sparse law discovery, balance laws of densities on graphs, transfer operators, detectors of critical transitions, open quantum systems;
- **tyche:** inference, Aitchison's geometry of compositions, learning;
- **nomos:** agent-based models, allocation rules, welfare orders;
- **the economics domain:** circulation of flows decomposed by a Hodge decomposition into driven, local and global parts; input–output models;
- **the allocation domain:** rounds of declared stages, verdicts under several principles, a sealed procedure, incidence.

*Remark.* The survey keeps four layers apart that the request runs together (§1.1).

- A *resource type* is an algebra: a carrier of amounts, a law of combination, an order of convertibility and declared properties such as divisibility.
- An *allocation space* is the set of ways a pool of that type can be distributed, with its geometry: a scaled simplex, a set of assignments, a set of words.
- A *genotype* is what the search varies; a decoder or a repair maps it to an allocation.
- An *evaluator* says how good an allocation is, with what uncertainty and with what standing.

The four are often collapsed into "an encoding". They have different laws, different verifications and different failure modes.

*Remark.* Two further distinctions run through all nine Parts.

- **Order in the effects against order in the representation.** Order-dependence is a property of what grants do to a state. Order in a genotype is a property of the representation. A set-valued allocation can be searched through permutations decoded greedily, and an order-dependent one through random keys (Part II §1, §4).
- **A scoped verdict against a verdict about the world.** A property established on a finite model, a finite presentation, a sample, a simulator or a learned law is established for that object only. Every positive verdict is scoped to what it was established on; refutations alone are definitive (Part IV §4; Part VII §6.3; Part VIII §6).

### 0.2 How the Survey Was Made

Nine surveys were written in parallel from the literature on 2026-11-20, each by a separate agent to one brief. They are kept whole as Parts I–IX:

- Part I, evolutionary encodings for heterogeneous, multi-type resource allocation: encodings that keep feasibility and heritability, direct against decoder-based encodings, mixed genotypes, constraint handling, and benchmark libraries;
- Part II, order-dependent allocation: where the order of grants changes the outcome and how it is modelled, permutation operators and the features they preserve, sequences against policies, redundancy, and search distributions over permutations;
- Part III, representation spaces and frameworks: geometric and forma-based definitions of variation, evolution on manifolds and the simplex, how mature frameworks compose heterogeneous genotypes, generators derived from types, and the pitfalls of composition;
- Part IV, the algebra and category theory of resources: resource theories as symmetric monoidal categories and ordered commutative monoids, allocations and conversions as morphisms, categorical structures for sequential allocation, what can be decided mechanically, and prior categorical work on evolutionary algorithms;
- Part V, pluggable fitness evaluators: the interface an evaluator should expose, replications and cost reduction for stochastic simulations, language models as evaluators, combining evaluators of different kinds and costs, and equity as objective or constraint;
- Part VI, agent simulation of economic circulation: what simulation reveals that accounts do not, stock–flow consistency, calibration and validation, learned agents as stand-ins for households and firms, and open code;
- Part VII, economic agents in learned spaces and dynamical systems: latent spaces and manifolds, densities and mean fields, learned simulators and their out-of-sample record, digital twins, and open code;
- Part VIII, learning classical dynamical-system models of economies: laws of motion learned from records, equation discovery, the verdict on chaos, stochastic dynamics and distributions, and reporting what the record cannot identify;
- Part IX, quantum and quantum-like dynamical models of economic phenomena: which make predictions classical models cannot, how they are fitted, quantum computation for economic dynamics, and non-commutativity as a model of order effects.

Each Part follows one form. Every section has a Takeaway, Cited Findings, Inferences and Gaps, so that what a source says is kept apart from what the survey infers from it. Each Part closes with a sixth section on what its findings mean for the design.

**Sources read in full or at length:**

- Part I: Raidl and Gottlieb's technical report on representations for the multidimensional knapsack problem, Londe et al.'s review of biased random-key genetic algorithms, CatCMA with Margin (arXiv v6) and pymoo's documentation;
- Part II: Starkweather et al. (1991), Larrañaga et al. (1999), Cicirello (2023), Chicano, Luque and Alba (2011), Rothlauf (GECCO 2003), Nguyen, Yang and Branke (2012) and the UPV/EHU note on estimation-of-distribution algorithms;
- Part III: Moraglio's thesis (2007), Moraglio and Sudholt (2015), Radcliffe (1994), Rothlauf's 2006 tutorial, Fong and Tino, Duchon, Flajolet, Louchard and Schaeffer (2004), Claessen and Hughes (2000), the Polytope paper, CMA-ES with Margin, CatCMA with Margin, and the documentation or source of Manopt.jl, Nevergrad, pymoo, DEAP, ParadisEO, jMetal, Optuna, ConfigSpace and CEGO;
- Part IV: Coecke, Fritz and Spekkens (2016), Fritz (2017), Jung et al., "Iris from the ground up" (2018), and copies of Moraglio's thesis and Radcliffe (1994) that another of the researchers had saved;
- Part V: AlphaEvolve (its first 100,000 characters), FunSearch, Lamperti, Roventini and Sani, Lee et al. (2015), Rakshit, Konar and Das (2017), and heuresis's own `evaluation.hpp` and CLAUDE.md;
- Part VI: Caiani et al. (2016), Soramäki et al. (2007) and Pichler et al. (2022);
- Part VII: Mealy, Farmer and Teytelboym (2019), Gu, Laurière, Merkel and Payne (2024), Cozzi and Pangallo, and Lamperti, Roventini and Sani;
- Part VIII: Barnett and Serletis (the 1998 working paper of their 2000 article) and LeBaron (1994); dozens of further references were checked against Crossref's metadata (about 54 occurrences of "[M; DOI checked]");
- Part IX: Wang et al. (2014, without its Supporting Information), Kvam, Busemeyer and Pleskac (2021) and Aaronson's post on the HSBC–IBM study.

The rest was read from abstract pages, records or search summaries, as each marker says.

**Sources that could not be reached:**

- Hartmann (1998) returned HTTP 403 (Part I §2);
- PNAS returned 403 for Wang et al.'s Supporting Information (Part IX §1);
- the *Science Advances* page of the AI Economist refused access (Part VI §4);
- HAL blocked UH-CMA-ES by bot protection (Part V §2);
- SSRN refused automated fetching of Khundadze's preprint (Part VIII §2);
- the author PDFs of Hoff, Raftery and Handcock (2002) were scanned images (Part VII §1);
- Poledna et al.'s full text was behind bot protection (Part VII §3.4);
- the INRIA paper on common random numbers returned 403 for its title (Part V §2);
- Colutto et al.'s Riemannian CMA-ES could not be read (Part III §2).

Each Part says what it could not read.

**How Section 1 relates to the Parts.**

- Section 1 is a synthesis, written after reading all nine Parts. It adds no reference that is not in them, and it cites the Parts (for example "Part III §2") rather than repeating their references.
- Inside a Part, a bare section reference (for example "§3" in Part II) means that Part's own section.
- The Parts overlap by design. FunSearch, Lamperti, Roventini and Sani's surrogates, the AI Economist, Poledna et al.'s Austrian model, Achdou et al.'s continuous-time model, Moraglio's thesis and CatCMA each appear in more than one Part. The discrepancies found between Parts are listed in §0.4.
- Part X is reserved for what is built.

### 0.3 Verification Markers

- **[V]** read at the source during this research: a full text, a section of it, or an abstract page. Several Parts say how deep the reading went:
  - Part IV writes [V text] for a full text with the definition or theorem number given, and [V abstract] for an abstract page;
  - Part V writes [V, abstract] or [V, abstract page];
  - Part VIII writes [V text] and [V abstract];
  - Part IX writes [V-abs] for an arXiv abstract page.
- **[S]** from a search snippet, a bibliographic record or a secondary summary only.
- **[M]** from memory, unchecked. Part VIII adds **[M; DOI checked]**: the reference exists as cited (title, journal, year, authors, volume and pages checked against Crossref's metadata), but its content was not read.

A marker attests what was read, not that a claim is true. Many [V] records are of abstracts, and an abstract's claim is the authors' claim. A DOI marked [M] may be wrong and must be checked before it is cited; a link given with an [M] claim is a pointer for checking, not evidence (Parts I and VI say so in their legends).

**Counts.** Table 0.1 gives the occurrences of each marker in each Part, counted by pattern on the notes.

- Repeated citations of one work are counted each time, so the counts are of occurrences, not of distinct works.
- They include each note's legend, and variant forms ("[V that the work exists]", "[V, abstract page]", "[S, secondhand]") are matched as their letter.
- They are approximate, to within a few per Part.

**Table 0.1.** Markers per Part (approximate occurrences).

| Part | [V] | of which abstract only, or full text | [S] | [M] | Distinct DOIs | Distinct arXiv identifiers |
|---|---|---|---|---|---|---|
| I | ≈ 38 | not distinguished | ≈ 58 | ≈ 32 | 21 | 10 |
| II | ≈ 43 | not distinguished | ≈ 46 | ≈ 32 | 17 | 9 |
| III | ≈ 60 | not distinguished (2 abstract only) | ≈ 34 | ≈ 7 | 16 | 25 |
| IV | ≈ 53 | ≈ 17 abstract; ≈ 34 full text | ≈ 26 | ≈ 19 | 14 | 28 |
| V | ≈ 48 | ≈ 17 abstract | ≈ 42 | ≈ 34 | 19 | 36 |
| VI | ≈ 26 | not distinguished | ≈ 36 | ≈ 30 | 28 | 23 |
| VII | ≈ 22 | not distinguished | ≈ 76 | ≈ 16 | 17 | 45 |
| VIII | ≈ 27 | ≈ 17 abstract; ≈ 9 full text | ≈ 39 | ≈ 59, of which ≈ 54 "[M; DOI checked]" | 86 | 21 |
| IX | ≈ 41 | ≈ 18 abstract | ≈ 49 | ≈ 18 | 18 | 28 |

*Caveat.* The records are mostly checked; the content much less.

- **Where the evidence is strong.** The texts read in full (§0.2) carry the strongest statements:
  - Raidl and Gottlieb's controlled comparison;
  - Starkweather's and Cicirello's operator studies;
  - Moraglio's geometric framework;
  - Coecke, Fritz and Spekkens' and Fritz's definitions and theorems;
  - Iris's resource algebras;
  - Caiani et al.'s stock–flow accounting;
  - Pichler et al.'s forecast;
  - Barnett and Serletis's verdict on chaos;
  - Wang et al.'s QQ test;
  - Kvam et al.'s open-system model.
- **Where it is weaker.** Much else rests on abstracts, among them most of the language-model, learned-simulator, neural-inference and quantum-computation results. Some rests on snippets, such as Dur et al., Tacchella et al., Poledna et al., the AI Economist's figures, Chapman and Pearson, and Gabaix, Lasry, Lions and Moll.
- **Formulas and theorem statements from memory are flagged where they are given:**
  - the Mallows and Plackett–Luce definitions and the Gumbel construction (Part II §5);
  - the facts of species (Part IV §1);
  - Deb's feasibility rules and ε-constrained differential evolution (Part I §4);
  - the theorem statements of Mayr and Meyer, of Bloom, and of Knuth and Bendix, known only from records or from memory (Part IV §4);
  - the dominance statements on active and non-delay schedules, which came through a citing source (Part II §4).
- **What must be done before they are relied on.** They must be re-derived or read before they become test fixtures. Reference values must be generated by running the reference libraries (HACT, pymoo, CMA-ES with Margin, the authors' codes), as the repository's practice requires.

### 0.4 Corrections and Discrepancies Found While Assembling

1. **Tacchella, Mazzilli and Pietronero (2018): two readings of "25%".**
   - Part VII §1.4 reports, from press accounts [S], that the fitness–GDP method beat the IMF's five-year forecasts "in about a quarter of cases".
   - Part VIII §1.5 reports, from snippets [S], that it beat them "by more than 25% on the available data", and adds the critics' points: the IMF benchmark is opaque, and the gain may partly reflect removing the IMF's biases.

   These are different claims, and neither Part read the paper. The paper decides.
2. **Lamperti, Roventini and Sani: one study, three markers and two sets of figures.** Parts V §2 and VII §3.2 read the full text (ar5iv) [V]; Part VI §3 saw only records [S] and marks the XGBoost surrogate as [M], which the other two Parts confirm.
   - *Figures in Part V:* at 2,500 true evaluations a binary F1 of about 0.80 and about 75% of positives found (Brock–Hommes), about 90% of positives in the Island setting, and a precision of 94.72% rising to 99.35% with Platt scaling.
   - *Figures in Part VII:* an F1 of about 0.80, and a true-positive rate of about 0.70 at 500 and 0.95 at 2,500 labelled points. It adds that the random seed was fixed across parameter sets in the main experiments, which bears on common random numbers.

   The true-positive rates at 2,500 differ unless they refer to different models or settings. The paper's tables decide. The JEDC DOI 10.1016/j.jedc.2018.03.011 is [M] in both Parts.
3. **The AI Economist's 16%.** Part VI §4 could not open the *Science Advances* page and marks the 16% improvement in the equality–productivity trade-off as [M]. Part VII §3.5 cites a PMC record (PMC9067926) for the same figure [S]. The stronger marker holds for the record; the figure has not been read in the text.
4. **Records checked in one Part and only remembered or seen secondarily in another.** The stronger marker holds; the Parts are left as written.
   - Achdou, Han, Lasry, Lions and Moll: the DOI 10.1093/restud/rdab002 is [M] in Part VII §2.1 and checked against Crossref in Part VIII §4.3.
   - Yakovenko and Rosser (2009): [V] in Part VI §1b; [M] in Part VII §2.2.
   - Rothlauf on redundancy: Part I §2 marks as [M] the finding that uniform redundancy is neutral and non-uniform redundancy helps only if it over-represents good solutions; Part II §4 read it in Rothlauf (GECCO 2003) [V]; Part III §5 read Rothlauf's tutorial [V].
   - CMA-ES with Margin: [S] in Part I §1; [V] in Part III §3, with DOI 10.1145/3512290.3528827.
   - Bean (1994): DOI 10.1287/ijoc.6.2.154 [M] in Part I §1; [S] from a scilit record in Part II §2.
   - FunSearch: [S] in Part II §3; [V], full text, in Part V §1.
   - Evolution of Heuristics (EoH): [M] in Part II §3; [V, abstract] in Part V §3.
   - Ballarin et al.'s multi-frequency echo state network: [S] in Part VII §3.3; [V abstract] in Part VIII §1.2.
   - "SciML in the Wild" (arXiv:2607.09684): [S] in Part VII §3.3; [V abstract], with the authors (Anand et al.), in Part VIII §1.5.
   - Radcliffe (1994): Part IV §5 confirmed no DOI or URL for the copy it read; Part III §1 gives DOI 10.1007/BF01531276 and a PDF [V].
5. **A gap in one Part closed by another.** Part I §3 did not check pymoo's operator defaults against its source. Part III §3 read `pymoo/core/mixed.py` [V], and the two agree in substance:
   - Binary: uniform crossover and bit-flip mutation;
   - Real: SBX and polynomial mutation;
   - Integer: SBX on floats with `RoundingRepair`, and polynomial mutation with rounding;
   - Choice: uniform crossover and random reassignment.

   Part III adds that pymoo asserts every crossover takes two parents and yields two offspring, and that it recombines each categorical variable on its own.
6. **Order Crossover: useful, but not geometric.** These are complementary, not contradictory. A design that wants geometric guarantees must give up OX and declare it.
   - Part II §2 records a tension between sources. The 1991 and 1999 studies describe OX1 as a relative-order operator; Cicirello finds OX effective for edges but not for precedences, which Part II explains by the contiguous block OX copies.
   - Part III §1 adds that Davis's Order Crossover is not geometric under any metric, since convergence fails (Moraglio §14.4).
   - Part II §6 lists OX among the adjacency operators. Part III §6 advises avoiding OX where geometric guarantees are wanted.
7. **Claessen and Hughes, "QuickCheck" (ICFP 2000): an unresolved DOI.** Part III §4 found it listed as 10.1145/357766.351266 or 10.1145/351240.351266 [S]. Part IV §4 gives 10.1145/351240.351266 [M]. Crossref decides.
8. **Sarafu: a paper named in the brief was not found.** The assignment of Part VI named a Sarafu paper by "Mattsson, Criscione and Ruiz"; no Sarafu paper with an author Ruiz was found. The circulation paper is Mattsson, Criscione and Takes (2023) [V]. The open dataset paper is remembered as Mattsson, Criscione and Ruddick (2022) [M] (Part VI §1d). Part VI also notes that a search summary said the network's structure changed after 2021, which the abstract of Criscione (2024) does not say.
9. **Inconsistencies inside sources, reported by the Parts.**
   - Soramäki et al.'s report on Fedwire says in its text that the 62 daily networks are from the first quarter of 2004, and in Table II's caption the fourth (Part VI §1c).
   - In Kvam, Busemeyer and Pleskac (2021), the estimated quantum–Markov weight α differs slightly between the text and Table 3 (0.23 or 0.285 in Experiment 1; 0.64 or 0.641 in Experiment 2). The Methods are inconsistent on whether the start-point variability was free (Part IX §2a).
   - BeforeIT.jl's licence is stated as AGPL-3.0 in arXiv v1 and the JuliaHub documentation, and as Apache 2.0 in a later PDF. The repository's LICENSE file decides (Part VI §5).
10. **Claims that changed between versions.**
    - Park et al.'s generative agents (arXiv:2411.10109). Version 1 said agents replicate General Social Survey answers "85% as accurately" as participants replicate their own. Version 4, retitled and revised on 16 November 2026, reports 72.8% accuracy against people's own 79.5% two-week test–retest agreement, and says that grounding did not reduce hyper-rationality in economic games (Part VI §4).
    - Poledna et al.'s working papers said the Austrian model could "in the long run significantly outperform" VAR and DSGE benchmarks. The published version says it "can compete with" them (Part VI §1a).
11. **A withdrawn source kept visible.** Norman, Rivera and Hughes (arXiv:2606.19544) reported large gaps between raw agreement and Cohen's κ for language-model judges. arXiv withdrew the paper on 2026-11-03 as not meeting its quality standards. Part V §3 keeps it, marked so that its numbers are not relied on.
12. **Mismatched or misdated records, reported by the Parts.**
    - The snippet's record identifier for Michels, Kang and Wang (JASSS 25(2), 2022) does not match the paper (Part V §2).
    - Kleinman, Spall and Naiman's *Management Science* 45(11) article is from 1999, though the snippet said 1997 (Part V §2).
    - Crossref misspells the first author of the BDS test as "Broock" (Part VIII §3).
    - Li, Plagborg-Møller and Wolf's comparison of local projections and VARs is in the *Journal of Econometrics* (2024) according to a snippet, while Crossref returned only the NBER and SSRN versions (Part VIII §1.1).
    - The authors and venue of "Classic-probability accounts of mirrored (quantum-like) order effects in human judgments" are remembered as Kellen, Singmann and Batchelder, *Decision* 2018, and were not confirmed (Part IX §1a).
13. **A discrepancy with the repository's earlier survey on precedence.** Part II §1 reports Dur, Kominers, Pathak and Sönmez (2018), from records [S]: lowering a reserve's precedence, or enlarging it, never reduces the number of reserve-group students admitted. The repository's earlier survey, *Allocative justice under scarcity* (its §0.4, item 10), read Pathak, Sönmez, Ünver and Yenmez in arXiv:2008.00374v3:
    - Dur et al.'s monotone statement holds with one preferential category;
    - Example 1 there shows a category gaining beneficiaries by being processed *earlier*;
    - Proposition 4 gives the conditions under which the monotone statement holds.

    Precedence is therefore not monotone in general, and a framework that searches over reserve precedence orders (Part II §6, item 9) must present the outcomes of every order as a set.
14. **Edits made in assembling.**
    - *Headings.* The sections of Parts II, III, VIII and IX were written as key questions. They are retitled here as declarative headings, and their "Key Question n" and "Qn" labels renumbered "n". Where a Takeaway opened with "Yes." or "No." in answer to its question (Part III §1, Part VIII §2, Part IX §3), the opening is rewritten to state the answer.
    - *Cross-references.* References between notes ("Part 2", "part 5", "another part") are rendered "Part II", "Part V", "Parts VI and VII". Each note's preamble becomes a remark on its scope and conventions; the words addressed to the report writer or the caller in the preambles of Parts VII and IX are reworded. The closing sections become §6 of each Part, and Part IV's headings are set in title case like the others.
    - *Typography.* Horizontal rules are removed, Part VI's bold markers are set plain, and spelling is made British outside quotations, link texts and code.

    No content was changed.
15. **Items unresolved rather than corrected.** Each Part's Gaps list them. Among them:
    - the precise ranking of RCPSP representations (Hartmann 1998; Kolisch and Hartmann 2006), from records and memory (Parts I §2 and II §4);
    - Falkenauer's argument that standard encodings of grouping problems are redundant and poorly heritable, from memory (Part I §1);
    - the Mallows–Ulam stability result, whose source paper is unclear from the snippet (Part II §5);
    - the full text of Gonda and Spekkens, of Meseguer and Montanari, and of the separation-logic papers (Part IV §1, §2);
    - HACT's reference values and Achdou et al.'s two-type closed form, which must be generated by running the authors' codes (Part VII §6);
    - the Supporting Information of Wang et al. 2014, and whether the 70 surveys were released as a table (Part IX §1, §5);
    - the licences of most listed code and data (Parts VI §5, VII §5, IX §5).

## 1. What the Nine Parts Say Together

The nine Parts agree on the shape of the answer before its details. **A universal framework is not one encoding with a fitness function.** It is a composition of typed layers, each with its own laws, its own way of being checked, and its own way of failing:

- **Resource types.** A resource type is an ordered commutative monoid with declared properties. Divisibility, distinctness, copyability and disposal are extra structure on that core, not separate theories. A symmetric monoidal category of conversions sits above it, with matrix models over a chosen semiring for feasibility, counting, cost or chance (Part IV §1, §2, §6).
- **Allocation spaces and genotypes.** Each is a typed object. It supplies sampling, mutation, a metric with a unit, validity and optionally recombination, repair, a chart and a standardised embedding, with laws that can be checked. Spaces compose by a small algebra: product, sum with conditional activation, sequence, set, multiset, permutation, simplex, manifold, graph and constrained subspace (Part III §6; Part I §6).
- **Feasibility** is placed in the gene domain, in a boundary-seeking decoder, or in repair, chosen by the kind of constraint. Penalties come last (Part I §4, §6).
- **Order.** The order of allocation is the non-commutativity of grants acting on a state. Which grants commute is declared or estimated as data, and the quotient it defines (a trace monoid) gives canonical forms that remove redundancy. Part II and Part IV reached this formulation independently (Part II §1, §6; Part IV §3).
- **Operators and search distributions** must preserve the feature the objective rewards: precedence, adjacency or position. The same choice reappears as the distance of a Mallows model (Part II §2, §5).
- **Sequences and policies.** A fixed sequence suffices only when the system is deterministic, fully known and solved for one instance. Otherwise a policy reacting to the state is evolved. The adaptivity gap measures what a sequence gives up (Part II §3).
- **Evaluators** return records: metrics with uncertainty, per-case values, cost, status and provenance, with a standing (verified, computed, estimated or proposed). They are combined in tiers, never summed. A language model may propose but is a weak and exploitable judge inside a selection loop (Part V).
- **Verification** yields four kinds of verdict: by construction, on a finite model, for a finite presentation, and on samples. Only refutations are definitive in every case. No prior categorical specification of evolutionary operators was found (Part IV §4, §5).
- **Economic dynamics** enter as evaluators, and each establishes a different thing:
  - an agent simulation with balance sheets and payment timing shows how the order of grants propagates through liquidity, network position and behaviour, provided money is conserved by construction (Part VI);
  - a declared density law gives exact consequences under its assumptions (Part VII §2, §6);
  - a learned operator measures order-dependence by a commutator within its training support (Part VII §6.2);
  - an ensemble of learned laws ranks orders only where the record identifies the nonlinear terms that create order effects (Part VIII §6);
  - a quantum-like model of judgement earns a place only where its parameter-free constraints are tested and hold (Part IX §4, §6).

What the literature does not supply is equally consistent across the Parts:

- a controlled comparison of encodings on genuinely heterogeneous allocation (Part I §5);
- any source that uses monoid or trace language for allocation (Part II §1);
- a categorical treatment of evolutionary operators (Part IV §5);
- a study tracking trust across several heterogeneous evaluators in one run (Part V §4);
- an agent model validated against measured circulation statistics (Part VI §3);
- a learned simulator evaluating allocation sequences (Part VII §6);
- a confirmed economic law found by equation discovery (Part VIII §2);
- a quantum-like model applied to allocation decisions (Part IX §4).

The framework proposed here is therefore novel in its combination, and its value is untested.

An infrastructure should build the framework as declared resource types with scoped verdicts, typed spaces and genotypes with derived operators, a grant system whose commutation structure is data, and evaluators that report what they established and with what standing. It should also keep a sealed record of every evaluation. It must never present a scoped verdict ("best found under evaluator E with budget B"; "holds on the finite model M"; "the ranking of orders is stable across the ensemble") as an unscoped one ("optimal", "lawful", "order does not matter").

### 1.1 A Framework of Four Typed Layers Replaces the Single Encoding

The Parts describe four layers whose separation is the framework's first design decision (Table 1.1). Collapsing them is the source of several documented failures:

- **Units in a mutation step.** Part III §5 finds that a product of parts with unscaled metrics lets the largest-ranged part dominate, which happens when units enter a mutation step.
- **Invalid offspring.** Part III §5 finds that a product taken as the true space produces invalid offspring when constraints or dependencies hold, which happens when an allocation's validity is ignored by its genotype.
- **Leaks.** Part VI §2 finds that an evolutionary search exploits unfunded money in a leaking simulation, which happens when an evaluator's conservation is ignored by the search.

**Table 1.1.** The four layers of an evolutionary allocation framework.

| Layer | What it is | Its laws | How they are checked | Parts |
|---|---|---|---|---|
| Resource type | A carrier S^K of amounts over a quantity semiring; combination + (quantity-like) or an idempotent join (quality-like); a preorder of convertibility generated by declared conversions; optional disposal per kind; an optional validity predicate; declared properties | Associativity, commutativity and unit; monotonicity of combination; conservation as homomorphisms; splittability (Riesz decomposition); refinement; validity closed under sub-bundles; frame-preserving updates | By construction (products of verified primitives preserve equational laws); on generators (homomorphisms); exhaustively on finite models; by bounded search with a witness or a monotone | IV §1, §2, §4, §6 |
| Allocation space | The species of distributions of a pool: weak compositions for identical units, assignments E^k for labelled items, a scaled simplex for divisible amounts, words for sequences, products for mixed pools | Metric axioms; membership; chart identities; uniform generation given size | Enumeration where small; sampled property checks otherwise | III §4, §6; IV §1, §6 |
| Genotype and decoder | A record of typed blocks (real shares, integers, categories with eligible domains, permutations or keys, partitions, sums), mapped to an allocation by a greedy feasible decoder, a schedule generation scheme, or repair | Purity, geometricity, respect and assortment of operators; idempotent repair; the decoder's dominance property (for example active against non-delay schedules) | Property checks of each space; static representation measures (mutation innovation, crossover loss, heuristic bias) before committing | I §1–§4, §6; II §2, §4, §6; III §1, §5, §6 |
| Evaluator | A map from an allocation, a sequence or a policy to a record of metrics with uncertainty, per-case values, cost, status and provenance | Conservation as a hard constraint; replay by a random-stream key; a trust order among evaluators; scoped claims | Audit against a more trusted evaluator; agreement tracked through the run; coverage checks of predictive intervals | V; VI §6; VII §6; VIII §6; IX §6 |

*Remark.* The layers meet at two interfaces, which carry most of the framework's correctness.

- **Resource type to allocation space.** The space is derived from the type. A resource with carrier ℕ^K gives weak compositions; one with a validity predicate forbidding x + x gives assignments of distinct items (Part IV §1, Inferences).
- **Genotype to evaluator.** The decoder's output is what the evaluator receives, and it is also the key under which evaluations are cached and redundancy is measured (Part II §4; Part V §6.4).

### 1.2 A Resource Type Is an Ordered Commutative Monoid with Declared Properties, and Divisibility, Distinctness and Copyability Are Extra Structure

**Two levels.** Part IV §1 finds the best-supported formal notion in two levels.

- *A resource theory* is a symmetric monoidal category. Its objects are resources, its morphisms the conversions that cost nothing, ∘ sequential and ⊗ parallel composition (Coecke, Fritz and Spekkens, Definition 2.1, [V text]).
- *Its decategorification* records only whether a conversion exists, and is an ordered commutative monoid (OCM): a set with an associative, commutative, unital + and a preorder such that x ≥ y implies x + z ≥ y + z (CFS Definition 4.1 and Theorem 4.2; Fritz Definition 3.1, [V text]).

**The law of combination carries the meaning, not the order.** CFS's "food" is ℕ² under addition and "proficiency" is ℕ² under componentwise max. They have the same order of convertibility, yet catalysis is impossible for food and possible for proficiency (Examples 4.3 and 4.4; Part IV §1.1). Consumable, quantity-like resources combine by +. Copyable, quality-like resources, such as information, skills and licences to use, combine by an idempotent join with a + a = a (Definition 4.16).

**Disposal must be declared, not assumed.** Fritz deliberately does not assume x ≥ 0, for four reasons (Part IV §1.2, [V text]):

- not everything can be discarded;
- if everything can be, no non-trivial conservation law exists;
- most results do not need the assumption;
- dropping it keeps self-duality.

Chores, wastes and obligations are resources that are not freely disposable. Bogomolnaia et al. warn that monotonicity guarantees which hold for goods fail for bads (Part IV §1.5, Inferences). A framework that allocates burdens as well as goods inherits this warning.

**Divisibility has two meanings that must be kept apart** (Part IV §1, Inferences).

- *Physical divisibility* is a semimodule action of ℚ≥0 or ℝ≥0 on a cone, with Riesz decomposition for splitting.
- *Asymptotic divisibility* is Fritz's many-copy regularisation, which adds formal fractions for rates.

The quantity semiring fixes physical divisibility:

- ℕ for indivisible identical units;
- (1/q)ℕ for granular amounts;
- ℚ≥0 or ℝ≥0 for divisible ones;
- ℤ or ℚ for signed balances and debts;
- 𝔹 under ∨, or [0, ∞) under max, for quality-like resources.

Mixed goods are the product ℕ^k × ℝ≥0^m, for which an envy-free allocation in Bei et al.'s generalised sense always exists (Part IV §1.5, [V abstract]).

**Distinct items need partiality.** A pool of distinct items is a finite set of labels with + as disjoint union, which is partial when an item cannot be in two bundles (Part IV §1, Inferences). Iris's resource algebras keep total algebra and rule out invalid combinations by a validity predicate closed under sub-combination, V(a·b) ⇒ V(a) (Part IV §2.4, [V text]). With such a predicate, "a labelled item is held at most once" becomes the law that x + x is invalid.

**The properties an allocation framework needs are already named, each a checkable law** (CFS, Definitions 4.10–4.22, [V text]):

- catalysis-free;
- non-interacting, which is Riesz decomposition, and is splittability: if the pool can meet the total demand, it can be split into sub-pools that meet each recipient's demand;
- quantity-like and quality-like;
- freely disposable and waste-free;
- refinement, which makes re-allocation between two splits well defined (Part IV §1.5, §2, Inferences).

Splittability holds for ℕ^k and ℝ≥0^k with the componentwise order [M, standard]. It fails when an indivisible unit, such as a machine, must serve two recipients. It is exactly the property under which allocation decomposes into independent per-recipient problems (Part IV §2, Inferences).

**Conservation is a homomorphism, and Petri nets are the presentation format.** A conserved quantity is a monoid homomorphism c to (ℤ, +) with c(pre_t) = c(post_t) for every generating conversion t (Fritz's atom count, Part IV §2.1). Fritz notes that a Petri net presents an ordered commutative monoid by generators and relations (Part IV §1.2, [V text]). A user-declared resource theory is therefore a Petri net:

- places are kinds;
- transitions are generating conversions;
- markings form ℕ^P;
- open Petri nets compose resource systems along interfaces (Part IV §2.2).

**Valuations are a separate layer.**

- *Additive valuations* are OCM homomorphisms, which are CFS's additive monotones, prices per kind (Part IV §1.3).
- *Submodular and gross-substitutes valuations* are characterised by exchange or concavity laws. Gross substitutes is M♮-concavity, and these laws can be checked exhaustively on small ground sets (Part IV §1.5).
- *Monotones serve as certificates of impossibility.* If M(a) < M(b), then a cannot be converted into b, but the converse fails in general (Part IV §1.3, §4).

Table 1.2 maps the kinds of resource the request names onto these structures.

**Table 1.2.** Kinds of resource, their algebra, allocation space and genotype.

| Kind | Carrier and combination | Order and disposal | Allocation space | Genotype block and natural operators | Parts |
|---|---|---|---|---|---|
| Indivisible identical units (beds, vehicles, seats) | ℕ^K under + | Componentwise; disposal declared per kind | Weak compositions of n units over k recipients, C(n+k−1, k−1) of them [M] | Integer genes: difference of geometric distributions (MIES), or Gaussian with rounding and a margin; repair to capacity | I §3, §4; III §3, §6; IV §6 |
| Granular amounts | (1/q)ℕ under + | Componentwise | A lattice in the scaled simplex | Integer genes on the grain | IV §6 |
| Divisible amounts (money, water, time) | ℝ≥0^K or ℚ≥0^K, a cone with a semimodule action | Componentwise | The simplex scaled by the pool | Real shares; Gaussian in isometric log-ratio coordinates (logistic-normal) or Dirichlet centred on the parent; crossover by convex combination in those coordinates (the Aitchison segment) | I §6; III §2, §6; IV §6 |
| Distinct labelled items (named staff, specific sites, specific frequencies) | Finite sets of labels under disjoint union, partial (x + x invalid) | Inclusion | Assignments E^k on the item set, k^\|U\| of them [M] | Categorical genes whose domain is the eligible set; set recombination that respects shared elements | I §4; III §6; IV §1, §6 |
| Groupings (which items share a server, a crew, a bin) | Partitions of the item set | — | Set partitions | Grouping genes with group-level crossover; a fitness that grades within plateaus | I §1, §6 |
| Copyable, quality-like (information, skills, licences to use) | 𝔹 under ∨, or [0, ∞) under max | a + a = a; supremal monotones | Sets of recipients; no conservation | Bit or set genes | IV §1 |
| Chores, wastes, obligations | Any of the carriers above | Not freely disposable (x ≱ 0) | As for goods | As for goods; monotonicity guarantees not inherited | IV §1 |
| Signed balances and debts | ℤ or ℚ under + | Conservation laws as homomorphisms | Transfers that sum to zero | Real or integer genes with a balancing decoder | IV §6; VI §2 |
| Sequences of grants, schedules | Words over a grant alphabet; a trace monoid when some grants commute | Generated by the grants' actions | Permutations, words, priority lists, policies | Permutations or keys with a decoder; operators by preserved feature; Plackett–Luce and Mallows models; priority functions evolved by genetic programming | II; IV §3 |
| Geometric quantities (orientations, subspaces, positions on a sphere) | A manifold, with no law of combination in general | — | The manifold | Tangent sampling, retraction, vector transport, step capped by a trust radius | III §2, §6 |
| Mixed pools | Products of the above | Componentwise | Products | Records of typed blocks | I §6; III §6; IV §6 |

*Remark.* The manifold row is an inference of this synthesis. Part IV's algebra concerns amounts that combine, and Part III's manifolds are representation spaces. A manifold-valued quantity, such as a direction, a subspace or a position on a sphere, generally has no law of combination. It therefore enters an allocation as an attribute with its own geometry, beside resource types that do combine, not as an ordered commutative monoid. A framework should say so rather than force a monoid onto it.

### 1.3 A Representation Space Is a Typed Object with Capabilities and Checkable Laws, and Spaces Compose by a Small Algebra

**Variation can be defined for any space with a metric.** Moraglio's geometric framework defines mutation by the balls and crossover by the segments [x; y]_d = {z : d(x,z) + d(z,y) = d(x,y)} of a metric on the search space, without reference to a representation (Part III §1, [V]). Three of its results matter for a universal framework.

1. **Geometric mutation is no restriction.** Every mutation is geometric for some radius.
2. **Geometric crossover is a real restriction.** Order Crossover, Koza's subtree swap and extended line recombination are provably non-geometric.
3. **Geometric crossovers on parts compose.** By the product theorem (Theorem 9.2.2), geometric crossovers on parts compose into a geometric crossover on the product under the sum of the component distances. The result holds even when the components are coupled (Corollary 9.2.3).

The product theorem is the formal licence for deriving crossover component-wise. It also exposes the implicit metric: summing unscaled component distances is what produces scale mismatch, so each part must declare a natural unit and be normalised by it (Part III §1, §5, Inferences).

**Three conditions decide whether geometric operators work well** (Part III §1, Inferences):

- the metric is one under which fitness is smooth or concave (Moraglio and Sudholt's convex-search runtimes, Part III §1, [V]);
- segments and balls can be computed and sampled;
- the formae, in Radcliffe's sense, are separable, so that recombination can both respect and assort them.

Adjacency formae on permutations fail the third condition (Radcliffe 1994, [V]). Under the discrete metric of a categorical gene every segment is {a, b}. Categories therefore cannot be interpolated, and exploration for them must come from mutation or from a distributional model (Part III §1, [V]).

**Manifolds need a chart, a retraction, a transport and a trust radius.** The standard Riemannian adaptation proceeds in four steps (Part III §2, [V]; the Manopt.jl interface is a tested template):

1. keep the search distribution in the tangent space at the mean;
2. sample there;
3. map the samples to the manifold by the exponential map or a retraction;
4. carry the covariance to the new tangent space by parallel or vector transport.

Its limit is locality. The search is confined to a normal neighbourhood whose size is bounded by the injectivity radius, which shrinks with curvature (Fong and Tino, [V]). Lie groups such as SO(d) are handled pragmatically, by running CMA-ES on the flat Lie algebra and exponentiating [S]. For the simplex, no evolutionary paper using Aitchison geometry was found (Part III §2, Gaps). Two options remain as design inferences:

- treat the simplex as Euclidean through the isometric log-ratio map, so that any R^n evolution strategy applies with Aitchison's distance;
- use Dirichlet mutation centred on the parent.

tyche's Aitchison geometry supplies the first directly.

**Mature frameworks separate three things** (Part III §3, Inferences):

- the value of a genotype part;
- its variation operators;
- an optional embedding into R^n.

Nevergrad makes all three properties of the typed parameter: each typed parameter maps to a block of R^n with a N(0, I) prior and also carries its own mutation and recombination (Part III §3, [V]). pymoo instead dispatches default operators by variable class, and ParadisEO, DEAP, jMetal and HeuristicLab compose generic encodings with operator objects. Containers are always products. Sums, meaning alternatives with different sub-structure, appear only in the hyperparameter-optimisation languages, as conditional activation (ConfigSpace's conditions and forbidden clauses, Optuna's define-by-run, Ax's dependents). No evolutionary framework has a first-class sum type with recombination aware of its branches (Part III §3, Inferences).

**Model-based and operator-based search use the same space differently.**

- *Model-based search* handles a heterogeneous product best by a joint distribution with type-specific marginals and a floor on their entropy. CMA-ES with Margin keeps the probability of leaving the current integer value at or above α. CatCMA joins Gaussian and categorical distributions, and reports beating Bayesian optimisation on its benchmarks, except in some low-budget cases (Part I §3; Part III §3, [V]).
- *Operator-based search* uses per-type mutation with self-adapted step sizes or probabilities (MIES).

The margin must be small: α ≈ 1/(Nλ) is recommended, and α ≈ N^{−0.5} made every trial fail (Part III §3, §5). Orderings stay outside the mixed-variable frameworks and enter through random keys or a separate permutation block (Part I §3).

**Generators can be derived with guarantees; variation operators less so.** A Boltzmann sampler built mechanically from a specification by union, product, sequence, set and cycle is exactly uniform among objects of the same size, runs in time linear in its output, and has its expected size tuned by one parameter (Duchon et al. 2004, Theorem 3.1, [V]). Recursive types have peaked size distributions, so they need rejection to reach a target size. QuickCheck's type-class generators give the engineering pattern of a generator under a size bound, with shrinking, but no uniformity. Strongly typed and polytypic genetic programming derive type-correct initialisation and subtree mutation. Polytope lacked crossover and bloat control, and both were missed in its experiments (Part III §4, [V]). Part III §6 proposes deriving crossover homologously, recursing component-wise where constructors agree and otherwise taking one parent's subtree, because homologous crossover is geometric under a structural edit distance where subtree swap is not.

**The contract of a space** (Part III §6) is the most concrete design in the nine Parts. A space provides required and optional capabilities, and a missing capability is declared so; algorithms that need it refuse the space.

- *Required:* `sample`, `mutate` at a part-local, standardised scale, `distance` with a declared natural unit, and `is_valid`.
- *Optional:*
  - `recombine`, with declared arity and whether it is geometric, pure and respectful (pymoo's hard-coded two parents and two offspring should not be inherited);
  - `repair` or `project`, idempotent;
  - a `chart` with log, retraction, transport, basis and trust radius;
  - `embed`, declared bijective, surjective with a decoder, or stochastic;
  - `marginal_floor`;
  - `size` and `shrink`.

The laws written once and checked for every space are:

- purity;
- geometricity;
- the ball bound of mutation;
- idempotence of repair;
- the retraction identities;
- inner products preserved by parallel transport;
- unembedding as the inverse of a bijective embedding;
- the metric axioms.

The constructors are product, sum (with a metric that puts branches a declared constant apart and a branch switch whose probability is floored), sequence, set and multiset, constrained subspace, dependent product and recursive types. Their derived operators are geometric by construction under the derived metric.

*Caveat.* The sum metric and branch-switch rule are Part III's proposals, unevaluated. A joint covariance across parts with no common embedding, such as a manifold part beside a graph part, is open. No provably geometric crossover is known for dependent products or recursive types beyond homologous crossover (Part III §6, Gaps).

### 1.4 Feasibility Belongs in the Gene Domain, the Decoder or Repair According to the Kind of Constraint

**Where feasibility can be placed.** Part I §1 finds that every successful family of encodings places feasibility in one of three places:

- in the genotype space (grouping, permutation and feasibility-preserving direct encodings);
- in a decoder (random keys, priority lists, weight-biased decoders);
- in a repair step.

Each placement has a documented failure mode:

- decoders suffer redundancy and flat landscapes;
- penalties suffer the "feasibility problem", a final population with only infeasible solutions;
- encodings whose genes have no stable meaning suffer weak heritability.

**The controlled comparison.** The best controlled comparison is Raidl and Gottlieb's on the multidimensional knapsack problem (Part I §2, [V]). It ran five representations in one steady-state evolutionary algorithm, with phenotypic duplicates discarded and 30 runs per instance. The average gaps to the LP bound were:

- direct encoding with heuristic initialisation, repair and local improvement: 0.329%;
- weight-biased decoding: 0.493%;
- permutations: 0.605%;
- random keys: 0.631%;
- ordinal encoding: 1.597%;
- random search: 6.353%.

On the largest instance (GK11, 2,500 items and 100 constraints), the direct encoding reached 0.052% against 1.246% for random keys. Heuristic bias, locality and heritability explained the ranking, not the labels "direct" and "indirect". Static measurements on random solutions (mutation innovation, crossover innovation, crossover loss and heuristic bias) predicted the dynamic behaviour, and need only a phenotypic distance (Part I §2).

**Two routes, one engine.** Part I §6 therefore proposes two representation routes that the framework can run on the same problem and compare. The difference between them is a *generality premium*, which the framework should measure.

- **Direct encoding, repair and local improvement,** where a problem heuristic exists.
- **Keys and a decoder** (BRKGA or the random-key optimiser), as the generic route, with problem knowledge confined to the decoder.

**The constraint decides the placement** (Part I §4, §6):

- *indivisibility* goes in integer, binary or grouping genes;
- *eligibility* goes in categorical genes whose domain is the eligible set, or in a decoder that skips ineligible pairs;
- *precedence* goes in order genes and a schedule generation scheme;
- *budgets and capacities* go in a boundary-seeking decoder, or in repair that drops the least efficient items and refills, guided by an efficiency ratio such as surrogate duals or LP reduced costs;
- *soft numerical constraints* go to Deb's feasibility rules, stochastic ranking or ε-constraints;
- *penalties* come last, and never with an infeasible initial population.

Decoders should seek the boundary of the feasible region, since all five knapsack representations succeeded by producing only boundary solutions (Part I §1, [V]). Phenotypic duplicate elimination should be on by default, since redundancy was largest for random keys and weight-biased encodings (Part I §6).

heuresis already orders single-objective evaluations by Deb's feasibility rule, and treats a failure as worse than any evaluation that did not fail, never answered with a made-up value (Part V §1, [V]). An equity floor can therefore be imposed as a hard constraint without penalty weights (Part V §5).

### 1.5 The Order of Allocation Is the Non-Commutativity of Grants, and Its Structure Is Declared as Data

**Where order comes from.** Part II §1 finds that the order of grants matters whenever a grant changes the state against which later grants are valued or made feasible:

- stock or capacity consumed (Starkweather's brewery allocation, the founding evolutionary case);
- prices that respond;
- learning and deterioration;
- spillovers and information revealed;
- precedence and setups that depend on the sequence;
- eligibility that changes;
- increasing returns that lock in a path (Arthur; David).

In mechanisms the order is itself a design parameter: picking sequences, serial dictatorship, and the precedence order of reserve categories (Part II §1).

**The formal core is a grant system.** Part II §1 and §6 give it, under the heading of a monoid-action formulation:

- a state space X;
- a grant alphabet G;
- a partial action δ : G × X ⇀ X, undefined for an infeasible grant, or a Markov kernel under uncertainty;
- an initial state;
- a value that is terminal or additive along the path.

An allocation is a word in G*, acting by δ_{g₁…g_k} = δ_{g_k} ∘ … ∘ δ_{g₁}. Its value depends only on the multiset of grants exactly when the actions commute on reachable states, so that the action factors through the free commutative monoid ℕ^G. A set or count vector is then the right representation, and permutation search is pure redundancy. When some pairs commute and others do not, an independence relation I ⊆ G × G turns G* into a trace monoid. Words in one trace give one outcome, and a canonical representative (a Foata or lexicographic normal form) is computed by sorting commuting neighbours (Part II §1, Inferences).

**Two Parts reached it independently.** Part IV §3 arrives at the same structure from category theory: deterministic sequential allocation is an action of the free monoid of steps on partial allocations, order-independent exactly when it factors through ℕ^Σ, partially commutative when it factors through a trace monoid. It adds that the independence relation is exactly the "frame" disjointness of separation logic. Two steps commute when they touch disjoint recipients or items, and Iris's frame-preserving updates make "independent" precise (Part IV §2.4, §3). Under uncertainty the steps are Kleisli arrows of the distribution monad, in a Markov category (Part IV §3). Neither Part found a source that uses this language for allocation, and both mark it as their synthesis (Part II §1, Gaps; Part IV §3, Gaps).

**Order-dependence is measured by commutators at every level of modelling.** Four Parts measure it the same way, each at its own level:

- **grant actions:** comparing δ_h(δ_g(x)) with δ_g(δ_h(x)) on sampled reachable states detects order-dependence empirically (Part II §1);
- **learned operators:** a transfer or Koopman operator K_a summarising simulated trajectories under each allocation gives the order effect of a sequence as the commutator K_{a2}K_{a1} − K_{a1}K_{a2}, within the region the data visited (Part VII §6.2);
- **learned laws:** under a linear time-invariant law with additive effects, order matters only through timing and discounting, by superposition, so genuine order effects (complementarity, hysteresis, triggering a regime) come only from nonlinear or state-dependent terms (Part VIII §6 B);
- **quantum-like models:** non-commuting projectors give order effects ⟨ψ|P_A P_B P_A|ψ⟩ − ⟨ψ|P_B P_A P_B|ψ⟩, but non-commutativity as such is not quantum, since two stochastic matrices generally do not commute either (Part IX §4).

The convergence has a consequence. The framework should declare, per resource system, which grants are known to commute, and test the rest by commutators at the level of whichever evaluator decides fitness.

**Redundancy comes in three kinds, each with its remedy** (Part II §4, Inferences):

1. *Commutation redundancy* (words in one trace): canonicalise to the trace normal form before evaluation, recombination and archiving, and cache by it. This is exact and synonymous by construction.
2. *Decoder redundancy* (a many-to-one decoder): evaluate once per phenotype by hashing the decoded allocation, and optionally write a canonical genotype back, as standardised random keys and justification do.
3. *Continuous-key redundancy:* random keys are uniformly redundant (each sorting cell has volume 1/n!), which Rothlauf's theory says is neutral.

Rothlauf's population-sizing result explains why the bias matters. The population needed to find the optimum scales as O(2^{k_r}/r), and redundancy helps only if good solutions are over-represented (Part II §4, [V]). The framework should therefore measure redundancy, as distinct phenotypes per sample and the frequency of good phenotypes, rather than assume it.

**Decoders decide which allocations are reachable.** The serial schedule generation scheme reaches the active schedules, which contain an optimum for regular objectives. The parallel scheme reaches only non-delay schedules, which may exclude every optimum [S] (Part II §4). Each decoder should state which dominance property it preserves.

**Mechanisms' orders are search spaces too.** A picking sequence is a word over agents, serial dictatorship a permutation of agents, and reserve precedence a permutation of categories (Part II §1, §6). Kalinowski, Narodytska and Walsh show that, with additive utilities and independence between agents, expected utilitarian welfare is maximised when agents take alternate turns (Part II §1, [V abstract]). Because the effect of precedence is not monotone in general (§0.4, item 13), the outcomes over every order should be presented as a set. The allocation domain's declared stages are the natural place for these orders.

### 1.6 Operators and Search Distributions Must Preserve the Feature the Objective Rewards

**Three features, three families of operator.** Part II §2 finds that permutation operators differ in what they pass on:

- absolute position (cycle crossover, PMX, position-based crossover);
- relative order or precedence (order-based crossovers, precedence-preservative crossover, NWOX, insertion and scramble mutations);
- adjacency (edge recombination, OX, reversal).

**Performance follows the feature the objective rewards.** Starkweather et al. compared operators on two problems with the same steady-state GA (Part II §2, [V]):

- *On a 30-city travelling salesman problem* (adjacency matters), tuned runs found the optimum 30 times in 30 with improved edge recombination, and never with cycle crossover.
- *On the 195-order warehouse allocation,* where inventory depletion makes relative order matter, the results were "almost the opposite": Syswerda's order operators did extremely well, while edge recombination needed 200,000 recombinations to match what PMX found in 20,000.

Cicirello's systematic study measures each operator against one distance per feature (exact match, cyclic edge distance, Kendall τ for precedences) and confirms the ranking per feature (Part II §2, [V]).

**What matters in allocation.** For allocation in which earlier grants change the value of later ones through a consumed state, the decisive feature is usually precedence. Adjacency matters only when transitions between consecutive grants carry costs, and position only when the slot itself carries value (Part II §2, Takeaway).

**Distributions mirror operators.** Search distributions over permutations come in four families (Part II §5):

- models over random keys;
- histogram models of positions or edges;
- distance-based Mallows and generalised Mallows models;
- Plackett–Luce.

The Mallows line's performance "strongly relies on the distance used", and no distance wins on every problem (Part II §5, [S]). That is the distribution-side mirror of the operator-side finding. Each Cayley distance on the symmetric group corresponds to a generating set, a mutation, a preserved feature, a Mallows distribution and an algebraic differential-evolution move:

- Kendall τ: adjacent transpositions and precedences;
- Cayley: transpositions and positions;
- Ulam: insertions and blocks.

Hamming distance is only approximately in this correspondence, and the table in Part II §5 says so.

**Plackett–Luce is the natural model of "who first".** Sampling it is itself a picking process. It is top-weighted, estimating early positions most sharply, which suits allocation in which early grants shape the state. It equals sorting log-weights plus Gumbel noise [M], so a Plackett–Luce model is a random-key model with a principled noise law (Part II §5, Inferences).

**The framework should learn which feature its objective rewards.** Part II §6 proposes two steps:

1. a feature probe, correlating fitness differences with per-feature distances on sampled pairs;
2. adaptive operator selection over a portfolio indexed by feature, with distributions behind the same sampler interface as operators, which fits heuresis's ask–tell protocol.

The choice between geometric guarantees and empirical strength (OX, §0.4 item 6) is declared, not hidden.

### 1.7 A Fixed Sequence Suffices Only for Known, Deterministic, Single Instances; Otherwise a Policy Is Evolved

**When a sequence suffices.** Part II §3 states the decision rule. Offer an open-loop sequence when three conditions hold:

- the grant actions are deterministic;
- the whole instance is known when the decision is made;
- the solution is wanted for this instance only.

In a deterministic system an optimal open-loop plan equals the optimal closed-loop one [M], so offering policies there only adds cost.

**When a policy is needed.** Offer a policy when any condition fails:

- outcomes are random and observed before the next grant;
- claims or resources arrive online;
- one rule must serve many instances, or be audited as a rule, as institutions publish dispatching rules, priority scores and eligibility rules.

**The loss from committing to a sequence is the adaptivity gap.** Dean, Goemans and Vondrák bound it by 4 for the stochastic knapsack [S]. It can be estimated before choosing: compare the best common sequence on sampled scenarios with the scenario-wise best, which bounds any policy from above (Part II §3, Inferences).

**A priority list decoded greedily is already semi-reactive.** The decoder skips grants that are infeasible or ineligible in the current state, so the list behaves as the stationary policy "grant to the first eligible in list order". A priority function of state features generalises it across instances (Part II §3, Inferences).

**Three routes to policies are established** (Part II §3):

- *priority and dispatching rules evolved by genetic programming,* the mature route for dynamic scheduling, with routing and sequencing as co-evolved rules where there are two decisions;
- *neural policies optimised by evolution strategies,* which scale to over a thousand parallel workers exchanging only scalars [V abstract];
- *programs proposed by a language model and filtered by an evaluator.* FunSearch's priority functions for online bin packing beat first-fit and best-fit on well-studied distributions, though an independent 2025 analysis questions how interpretable they are [S].

**Rolling-horizon re-optimisation** gives reactivity without a learned policy, at a computational cost per decision (Part II §3).

**When the future depends on the present solution.** Nguyen, Yang and Branke find that for problems with time-linkage the only effective course is to predict future changes and account for possible outcomes. An "anticipation" term that penalises inflexible states significantly improved dynamic job shops (Part II §3, [V]).

The agent simulations of Part VI make the case for policies concrete. When an allocation reaches agents through payments with timing and liquidity constraints, early recipients relieve or deepen others' queues, and a rule reacting to the state of queues and balances is the natural object (Part VI §6.2).

### 1.8 Evaluators Return Records with Uncertainty, Standing and Provenance, and Are Combined in Tiers, Never Summed

**What mature systems return.** Every mature system Part V §1 reviewed returns more than a scalar.

- *FunSearch* scores each input and aggregates the scores, discarding programs that fail limits. It clusters programs by their per-input score signature, which doubles as a behavioural descriptor [V].
- *AlphaEvolve* returns a dictionary of scalars, supports staged evaluation and adds properties graded by a language model as extra keys [V].
- *Ax* records a mean and a standard error per metric. A standard error of 0 means noiseless, and a missing one means "infer the noise" [S].

heuresis's `Evaluation` holds objectives, constraints, descriptors, cost, a status and its reason. It has no field for uncertainty, fidelity, the random stream used or provenance (Part V §1, [V], read locally). Part V §6.1 proposes the record that fills these gaps (Table 1.3).

**Table 1.3.** The evaluation record proposed in Part V §6.1, against heuresis's present `Evaluation`.

| Field | Content | In heuresis now |
|---|---|---|
| Metrics | Per named metric: role (objective, constraint, descriptor, report), value, uncertainty (0 deterministic, absent unknown), replicates behind it, optional raw replicates, optional per-case or per-unit values | Objectives, constraints, descriptors without uncertainty |
| Cost | In the budget's unit: evaluations, seconds, core-hours, tokens, minutes of a person's attention | Yes |
| Status | ok, failed, refused, abstained (a judge below its confidence), with the reason | ok or failed, with the reason |
| Provenance | Evaluator identity, version and kind (closed form, simulation, agent-based model, surrogate, language model, person); fidelity; random-stream key (purpose, epoch, index); digest of the input as evaluated | No |
| Standing | Verified (programmatic or closed form), computed (simulation), estimated (surrogate), proposed (language model or an unvalidated person) | No |
| Judge | For a language model: model, prompt and rubric digests, temperature, order shown, all samples. For a person: who, when, presentation order, session | No |

The evaluator also declares, once, what it can do:

- whether it is deterministic or stochastic;
- whether it can return more replicates of the same candidate on request;
- whether it supports stages or intermediate reports;
- its expected cost per call;
- its metrics and their roles.

Part V §6 places the record and the selection procedures in heuresis, and evaluators that touch the world (simulators on clusters, language models, people) at the infrastructure's edges, as heuresis's own scope says (Part V §1, [V]).

**Noise is handled per comparison, not per candidate** (Part V §2). There is no universal replication count. A power analysis for a medium effect (d = 0.5, α = 0.05, power 0.8) asks for about 64–78 runs per arm on Lee et al.'s distributions and agent-based model [V]. Very large counts make trivial differences "significant". Adaptive procedures carry stated guarantees:

- races with Hoeffding or empirical-Bernstein bounds give a distribution-free 1 − δ guarantee per comparison;
- OCBA and indifference-zone procedures give a probability of correct selection under normality;
- resampling to a standard-error threshold is a simple default;
- UH-CMA-ES's rank-change measure diagnoses when noise overwhelms selection;
- surrogates and common random numbers carry no guarantee.

heuresis's keyed draws make common random numbers free. They should be kept only where the measured variance of differences falls, since they can hurt [S]. When outputs are multimodal, as agent-based models' often are, the evaluator should return the distribution of replicates, so that quantiles or shares of attractors can be objectives (Part V §2, [V]).

**Cost is handled by cascades and model management** (Part V §4):

- *cascades* pass a candidate to a costlier evaluator only if a cheaper one finds it promising (AlphaEvolve's staged tests; cheap-to-strong judges);
- *surrogates* pre-screen candidates, and the true evaluator is queried on a chosen subset, with the surrogate's agreement tracked on every re-evaluated point. Surrogates cut costs by orders of magnitude: 500 and 3,750 times for Lamperti et al.'s agent-based models [V]; an average R² of 99.99% for Britz and Storm's surrogate of a computable general equilibrium model [V abstract].

**Asynchrony biases the search.** Asynchronous evaluation with very different run times favours cheap-to-evaluate regions. heuresis's generational algorithms, which do not depend on evaluation order, avoid it; otherwise the bias must be corrected explicitly (Part V §4, [S]).

**A language model is a strong generator and a weak, exploitable judge** (Part V §3).

- *As generator.* The strongest results of evolution driven by language models (FunSearch, AlphaEvolve, EoH, LLaMEA) keep the model as generator and let a program decide. AlphaEvolve's authors say evaluation by a language model is "not a setting we have optimized for" [V].
- *As judge.* Strong models agree with human preferences at roughly human–human levels on chat tasks (over 80%, Zheng et al., [V abstract]). They carry position, verbosity and self-preference biases. Swapping the order of candidates alone made one model beat another on 66 of 80 queries [V abstract]. Reliability varies widely across tasks and datasets (JUDGE-BENCH, [V abstract]).
- *Under optimisation.* Universal adversarial phrases can push absolute scores to the maximum regardless of the text, and judges are much more susceptible under absolute than comparative scoring [V abstract]. Optimising against a learned proxy too far hinders the true objective [V abstract].

An evolutionary search is exactly such an optimiser. The safeguards with evidence are:

- comparative rather than absolute scoring, with both orders shown and aggregated;
- a judge from a different model family from the generator;
- an audit stream of candidates also judged by people, with chance-corrected agreement tracked through the run;
- selective evaluation that escalates when confidence is low. Trust or Escalate guarantees over 80% human agreement with almost 80% coverage on a subset where GPT-4 alone rarely reaches 80% [V abstract];
- prediction-powered inference for population-level statements from many model scores and a few human labels [V abstract].

A language model's score has standing *proposed* (grade C, in the infrastructure's grading). It enters only as a separate objective, a tie-breaker or a descriptor, never summed into the trusted fitness, and the gap between its score and the audited score is reported over generations (Part V §6.5).

**People are authoritative for preferences and fallible for facts** (Part V §4, §6.6):

- comparisons over ratings, aggregated by Bradley–Terry;
- a fatigue budget per session, with presentation order randomised and recorded;
- a learned surrogate of the person used only when fatigue exceeds one threshold and the surrogate's held-out agreement exceeds another (AES-IGA's rule);
- the most uncertain candidates, not only the best, sent back to the person.

**Evaluators are combined in tiers** (Part V §6.7):

1. failures last;
2. hard constraints from verified evaluators (programmatic checks, legal or budget limits, equity floors) by Deb's rule;
3. Pareto dominance over verified and computed objectives;
4. estimated and proposed metrics only as tie-breakers, descriptors, or a second front shown beside the first.

Objectives keep their evaluator's identity, so that a front reads "efficient on the economic model, equitable on the agent model, acceptable to the panel". A dual verdict per candidate, with standing and measure kept apart in kybernesis's convention, fits this exactly. Winners are re-evaluated at the highest fidelity with fresh random streams before being reported.

**Equity is represented natively, not as a weight** (Part V §5). It enters in up to three ways:

- as objectives: a dispersion index, the worst-off group's outcome, a sufficiency share;
- as declared hard constraints with their sources;
- as descriptors, so that a quality-diversity archive keeps allocations across the equity–efficiency range for people to choose among.

Minimising a Gini coefficient alone can be satisfied by levelling down unless efficiency is kept as an objective [S]. The evaluator should return the distribution of outcomes over units, so that other measures can be computed later without re-running an expensive simulator. With a stochastic evaluator, a Gini of means differs from a mean of Ginis, and which is meant must be declared.

**Table 1.4.** Kinds of evaluator, their standing and what they can guarantee.

| Evaluator | Standing | Guarantee it can carry | Characteristic failure | Parts |
|---|---|---|---|---|
| Closed form or programmatic check | Verified | Exact, in exact arithmetic | A wrong formula; floating point in law checks | IV §4; V §6 |
| Input–output or SAM multipliers | Computed | Exact under fixed coefficients; often solved by linear programming | Static multipliers "only mildly useful" for predicting shock impacts | V §2; VI §1f |
| Computable general equilibrium, system dynamics | Computed | Consistency with the model | Multiple equilibria; thousands of runs per search | V §2 |
| Agent-based model with stock–flow consistency and payments | Computed, stochastic | Selection guarantees per comparison if replicated (races, OCBA); a conservation verdict | Leaks exploited by the search; noise; behaviour the record does not identify | V §2; VI |
| Declared density law (HJB–Kolmogorov, kinetic exchange) | Computed | Exact consequences under the model, conservation checked | A wrong mechanism; no identities or specific ties | VII §2, §6 |
| Ensemble of learned laws | Estimated | Whether a ranking is stable across the ensemble; out-of-support flags | Order effects resting on the worst-identified terms | VIII §6 |
| Learned operator (transfer, Koopman) | Estimated | Order-dependence within the training support | Needs many agents; no extrapolation | VII §2.4, §6.2 |
| Machine-learned surrogate | Estimated | None beyond validation; predictive intervals not calibrated in the literature | Wrong outside its training region; must be re-run in the true model | V §2, §4; VII §3 |
| Language-model judge | Proposed | None per candidate; population quantities by prediction-powered inference; selective evaluation with an agreement guarantee | Biases of position, verbosity and self-preference; adversarial phrases; Goodhart's law | V §3 |
| Person | Verified for preferences they are entitled to state; proposed for facts | Bradley–Terry aggregation; audit | Fatigue; order of presentation | V §4, §6.6 |
| Quantum-like model of a panel's judgement | Estimated | Parameter-free tests (QQ, Grand Reciprocity) on counterbalanced data | Classical repeat-choice models reproduce QQ-like order effects; Grand Reciprocity fails for non-degenerate models | IX §1, §4, §6 |

### 1.9 Verification Yields Four Kinds of Verdict, and Only Refutations Are Unscoped

**Four kinds of verdict.** Part IV §4 distinguishes four kinds, and its table in §6(c) assigns each law of the framework a method and a meaning.

1. **Proved by construction or by a universal property.** A carrier built from verified primitives by closure operations that preserve laws is lawful. By Birkhoff's theorem for equational laws [M] and Bloom's for inequational laws [S], products of lawful components are lawful. A homomorphism law reduces to finitely many checks on generators. If the order is generated by finitely many conversions xᵢ ≥ yᵢ, a monoid homomorphism is monotone if and only if it is monotone on each generator, and a conserved quantity needs checking only per transition. These are proofs for all elements, not samples. Part IV marks the argument as its own and elementary, to be checked.
2. **Decided on a finite model.** Every tuple of a finite carrier is checked, which establishes the law for that model only. Truncating ℕ to {0, …, c} needs partial addition, or a validity predicate, to stay lawful; saturating addition changes the theory.
3. **Decided for a finite presentation.** Convertibility in a finitely presented commutative monoid or Petri net is decidable but hard [S]. The word problem for commutative semigroups needs exponential space (Mayr and Meyer), and Petri-net reachability is Ackermann-complete (Czerwiński and Orlikowski; Leroux; Leroux and Schmitz). A practical tool answers in one of three ways:
   - "convertible", with a witness firing sequence;
   - "not convertible", with a monotone or invariant violated;
   - "undecided within the bound".
4. **Sampled.** Property-based tests are evidence, never proof. Under uniform sampling over a finite space they bound the fraction of violating tuples, not their existence [M].

**Refutations are definitive in all four cases; positive verdicts are scoped.** A finite "holds" names its model. A construction proof names its closure theorem. A sampled "no counterexample" states its sample size and distribution (Part IV §6).

**Laws that resist the universal route.** Laws of the ∀∃ form, such as Riesz decomposition and refinement, are preserved by products but not necessarily by sub-structures [M]. They must be checked or proved per construction. Laws over ℝ cannot be decided by enumeration. They need exact rationals, intervals or symbolic certificates such as Farkas multipliers, because floating-point addition is not associative (Part IV §4, Inferences).

**What kategoria can decide.** The infrastructure's category library already answers every claim as holds, fails or undecided, with the place and the reason. It decides claims on finite categories, functors found by exhaustive search, limits, set-valued functors and Kan extensions; completes presentations by Knuth–Bendix with derivations a small checker verifies; and decides equality of string diagrams in symmetric monoidal theories by isomorphism, with models in matrices (Part IV §4, from the repository's CLAUDE.md). The matrix models give exactly the graduation an allocation framework needs (Part IV §2, §6):

- over 𝔹 for feasibility;
- over ℕ for counting the processes;
- over (ℝ ∪ {∞}, min, +) for least cost;
- over ℝ≥0 with column-stochastic matrices for Markov kernels.

**What can be verified categorically, and how.** The framework's laws are then verifiable in scoped form:

- *the laws of a resource type,* by construction for products of primitive semirings, on finite models otherwise;
- *conservation and prices,* on generators;
- *splittability,* for primitive cones by proof, otherwise on finite fragments or by bounded search;
- *convertibility,* with a witness or a monotone;
- *equality of conversion processes,* by isomorphism of string diagrams, or by completion when the theory has laws (undecided if completion does not terminate);
- *validity and frame preservation,* on finite resource algebras or by Iris's generic lemmas for sums, products and finite maps;
- *valuation classes* (submodularity, gross substitutes as M♮-concavity), exhaustively on small ground sets.

**No prior categorical work on evolutionary operators was found.** Part IV §5 reports that targeted searches found no functorial or monadic account of crossover or mutation, and no categorical verification of the laws of evolutionary algorithms. The closest prior work is algebraic:

- Radcliffe's formae, equivalence relations that generate representations and operators;
- Rowe, Vose and Wright's symmetry groups and invariant operators;
- Moraglio's metric axioms.

These supply laws a categorical specification could state and decide on finite instances:

- *equivariance:* an operator commutes with a symmetry group's action, a naturality square;
- *geometricity:* offspring lie in the metric segment;
- *respect and assortment of formae;*
- *being a Markov kernel:* finite instances are stochastic matrices.

**Proposals, not literature.** Part IV §5 proposes that each resource type determines a search space, its allocation species, and that operators are natural transformations between species. Equivariance under renaming of items or agents is then naturality, mutation is a Markov kernel F → D(F), and crossover F × F → D(F). Part IV §3 proposes that an evolutionary algorithm is a stochastic selection function (X → R) → D(X), in Escardó and Oliva's sense. Both are proposals, not literature; the gap is genuine, and the framework could fill it.

*Remark.* A further use of kategoria follows from the infrastructure's own practice; this is an inference of the synthesis. kybernesis already admits a revision of an ontology of facts only with a mapping that kategoria decides is a functor of its kinds, and carries records along it with what is lost named. A revision of a declared resource type, splitting a kind or merging two kinds, would be admitted the same way. The mapping would have to be a functor that preserves combination and the generated order, and existing allocations would be carried along it with what is lost reported.

### 1.10 Agent Simulations Show the Timing, Cycles, Concentration and Regimes of Circulation, Provided Money Is Conserved by Construction

**What accounts cannot show.** Aggregate accounting (input–output tables, social accounting matrices, LM3) records how much flows between sectors in a period under fixed coefficients. It does not represent timing, queues, balance-sheet constraints, network topology or behavioural rules. Part VI §1 finds four documented gains from agent simulation, the first two resting on data and the last two mostly demonstrations that a mechanism suffices:

1. **Propagation depends on the production function and on inventories.** Pichler et al.'s dynamic input–output model forecast in real time a 21.5% fall in UK GDP in the second quarter of 2020. The Office for National Statistics later estimated 22.1%; the Bank of England had forecast about 30%. Under pure Leontief production the fall in gross output reaches 60%, and a linear production function would have underestimated the recession by several points [V]. Static output multipliers were "only mildly useful" for predicting impacts [S].
2. **Circulation lives in cycles that tables cannot see** [V]:
   - in Sarafu, a Kenyan community currency of about 40,000 users, circulation was "highly modular, geographically localized", and cycle analysis "supports the idea that circulation requires cycles";
   - in Fedwire, 75% of value passed through 66 of about 6,600 banks and 181 links, 25 of which form a clique, and 90% of value lay within the giant strongly connected component.
3. **Distribution and concentration emerge from exchange among identical agents.** Conservation alone gives the exponential Gibbs law in kinetic exchange models [V]. Macro models grow heterogeneity from homogeneous starts [V].
4. **Macro agent models produce endogenous cycles, regime switches and tipping points.** Linearised representative-agent models exclude these by construction. Gualdi et al. find a generic phase transition between a good economy and a regime of high unemployment [S].

**Stock–flow consistency must be enforced by construction** (Part VI §2, [V]). Caiani et al. give every agent a balance sheet and book every transaction as four entries, Copeland's quadruple entry. The transaction-flow matrix (rows and columns summing to zero) and the balance-sheet matrix (every financial asset someone's liability) then hold by construction, not by calibration. Money is created and destroyed only by identifiable acts:

- lending and repayment;
- government spending and taxation;
- central-bank purchases.

A model that lets money appear (a new firm's deposits from nowhere) or vanish (a bankrupt's debt simply deleted) creates "financial black holes". Their bias on credit, consumption and investment is hard to evaluate. Caiani et al. call the initial stock–flow calibration "a major black hole in the literature".

**Leaks are what a search finds first.** Part VI §2 draws the consequence for evolutionary allocation. An allocator that grants money or goods must record the counter-entry: the government's deficit, a loan, a transfer from a fund. Otherwise the search will exploit the leak, and candidates that allocate unfunded money will score best. This is the same kind of exploit as the AI Economist's agents gaming taxes (Part VI §4, [V]). Conservation is therefore a hard constraint of the evaluator: a candidate that wins by a leak is infeasible, not good (Part VI §6.2).

**How order shows up in a simulation.** The order in which an allocation reaches agents changes what follows exactly when the simulation has four things (Part VI §6.2):

- timing, so that a recipient spends before the next grant;
- liquidity constraints, so that early recipients relieve or deepen others' queues;
- network position, so that money given to a hub reaches more agents in fewer rounds;
- behaviour that depends on state.

The payment-system models, Sarafu's evidence on early adopters and local hubs, and the markets domain's "where new money goes first" all point the same way. An evaluator of order-dependent allocation should be an agent simulation with payment timing, not a static multiplier.

**Circulation is read from the posting log** (Part VI §6.4):

- *velocity and holding time,* as a distribution per instrument and class of agent, not only turnover over stock;
- *retention and leakage,* treated as a walk of money: the probability that a unit injected at a given agent is still within a region after k payments, and the expected number of local rounds before it leaks. LM3 is the three-step truncation. Each measure is computed by injection point, which is how allocation order shows up;
- *circulation against driven flow,* by the economics domain's Hodge decomposition of the aggregated payment network, beside combinatorial cycle measures (the share of value in strongly connected components, cycle participation, cyclic density). The literature measures cycles combinatorially and not by Hodge projections, so reporting both on one record compares two notions of circulation; this is an inference;
- *concentration:* Gini coefficients and top shares, the exponential bulk's temperature and the tail exponent, the core's share of value;
- *fragility:* queues, settlement delays and cascade sizes, read by kinesis's detectors of critical transitions with their verdicts of signal, no signal or undecidable.

For a Hodge decomposition the flows should come from the transaction-flow matrix, with creation and destruction as edges to and from a distinguished node ("outside", a bank, the government). The decomposition then stays meaningful when money is not conserved among the remaining agents (Part VI §2, Inferences).

**Calibration proceeds from accounts to behaviour** (Part VI §3):

1. **Accounts first.** The strongest evidence that simulation adds forecasting value comes from models anchored on accounts: Pichler et al., and Poledna et al., whose Austrian model "can compete with" VAR and DSGE benchmarks out of sample [S].
2. **Behaviour by inference, declared where unidentified.** Behavioural parameters come by Bayesian or neural simulation-based inference:
   - a simple likelihood-based Bayesian approach consistently beat simulated minimum-distance methods in Platt's benchmark, degrading for large models with many free parameters [S];
   - neural posterior and ratio estimation needs simulation budgets "orders of magnitude smaller", and its amortisation allows simulation-based calibration checks of the estimator itself [S].

   The parameters the record does not identify are declared with ranges.
3. **Surrogates for sweeps and search, never as the reported answer.**

Validation should test circulation measures against held-out transaction records, such as holding times, cycle shares and retention after k rounds. No study was found that does so (Part VI §3, Gaps).

**Learned agents fail in opposite ways** (Part VI §4).

- *Reinforcement-learning agents are too good at the reward.* The AI Economist's agents learned to game taxes [V]. They are useful as adversaries that probe an allocation rule for leaks.
- *Language-model agents reproduce many qualitative findings, but are more homogeneous than people and hyper-rational in incentivised games.* The best grounded case reaches 72.8% accuracy on General Social Survey items against people's 79.5% test–retest agreement, and grounding did not reduce hyper-rationality [V]. Since circulation depends on heterogeneity (who spends locally, who saves, who lends), homogeneous agents will tend to understate dispersion.

The safeguards follow the infrastructure's grading:

- a language model's behaviour rule is a grade-C proposal until validated against held-out human records;
- the share of model-generated behaviour is reported with every result;
- conclusions are reported as findings only if they hold across written, learned and language-model agents.

No inverse reinforcement learning or imitation learning of economic agents from transaction records was found. Records show what was done, not what was available, so the choice sets are unknown, a coarsening problem of the kind peira formalises (Part VI §4).

**What simulation can and cannot establish** (Part VI §6.5).

- *It can establish* three things:
  - sufficiency: a mechanism suffices to generate a pattern;
  - comparisons of allocation orders and policies under one calibrated, conservation-checked model, with the ranking's robustness across behavioural kinds and parameter ranges stated;
  - conditional forecasts where the model is anchored on accounts and the shock is well measured.
- *It cannot establish* four:
  - that a mechanism is the actual one, since many models reproduce the same stylised facts;
  - behaviour outside validated ranges, or by unvalidated agents;
  - results that depend on an accounting leak, or on a production function chosen for convenience;
  - causal effects in the world without a design, which belong to peira.

**Open code exists, though none in C++** (Part VI §5):

- BeforeIT.jl for the Austrian model;
- Caiani et al.'s benchmark in Java;
- black-it for calibration;
- Pichler et al.'s replication files;
- Sarafu's transaction dataset.

None combines stock–flow accounting, payment timing and circulation measures. The order Part VI recommends is to port the bookkeeping discipline and the initialisation anchored on accounts first, as checks over agents' balance sheets in nomos's agent toolkit; to hold the port to the reference implementations; and to validate on Sarafu first.

### 1.11 Learned and Geometric Representations Give Invariants, Densities and Fast Emulators, Each Valid Only Within Its Group or Support

**Latent spaces give invariants, not coordinates** (Part VII §1). A latent space turns a network of ties or flows into positions whose distances, curvature and orderings carry structure:

- transitivity and clustering (Hoff, Raftery and Handcock);
- hierarchy, as hyperbolic curvature of trade maps, which has grown since the First World War [V abstract];
- relatedness and the paths of development (the product space; economic complexity);
- substitution and complementarity, from inner products in consumer embeddings.

**Positions are identified only up to a group.**

- Under distance and random dot product models, positions are identified up to O(d), and translation for distance models.
- Under the generalised random dot product graph, they are identified up to the indefinite orthogonal group O(p, q). Distances are then not invariant, and distance-based procedures such as k-means are to be avoided [S].

Any analysis that reads individual coordinates, or compares coordinates across separately fitted years without alignment, reads something not identified. Procrustes alignment across time is a choice of gauge, and dynamics read from aligned positions depend on it (Part VII §1, Inferences).

**One-dimensional orderings are relaxed cuts, not theory.** Mealy, Farmer and Teytelboym show that the Economic Complexity Index is a rescaled Fiedler vector of a normalised Laplacian, an approximate solution of the NP-hard Normalised Cut problem [V]. Three further points follow:

- its link to income is empirical, not derived;
- real data "do not partition clearly into two components";
- results depend on the binary threshold of revealed comparative advantage.

The productive use of such coordinates is as the state space of a learned dynamical system judged by forecasts: Tacchella et al.'s fitness–GDP plane (§0.4, item 1), and Brummitt et al.'s finding that diversification precedes growth (Part VII §1.4).

**Densities are the better object under three conditions.** A density description is preferable when (Part VII §2):

- agents are exchangeable given a low-dimensional individual state;
- they interact through aggregates or random encounters;
- they are numerous enough for the law of large numbers.

Such descriptions include the Kolmogorov forward equation coupled to a Hamilton–Jacobi–Bellman equation in mean-field games and continuous-time heterogeneous-agent models, and Boltzmann and Fokker–Planck equations for kinetic exchange. Niemann, Klus and Schütte find that reduced models learned from agent simulations agree with analytical coarse-grainings only when there are enough agents [V]. A density is the worse object where identities, specific ties or a few large agents matter, as in supply chains and interbank networks.

**What the density models offer.**

- *Without aggregate shocks,* Achdou et al.'s finite-difference scheme for the HJB–Kolmogorov system is accurate and cheap. It proves a unique stationary equilibrium when the intertemporal elasticity of substitution is at least one, and gives a closed-form distribution for two income types [S].
- *Its forward operator is a balance law.* It is the transpose of the discretised generator, a conservative Markov generator on a grid: a balance law of a density on a graph of cells, with a drift given by the policy. That is kinesis's `balance` module (Part VII §6.1).
- *With aggregate shocks,* the density becomes a state. Deep global methods (DeepHAM's learned generalised moments, Maliar, Maliar and Winant's all-in-one operator, Gu et al.'s neural solutions of the master equation) "tame" the curse of dimensionality but do not remove it. Sampling the distribution is the hardest part, and networks can converge to "cheat solutions" that zero the derivatives [V]. Their accuracy evidence is mostly internal (residuals, agreement with classical benchmarks), not out-of-sample fit.

**Learned simulators carry three kinds of evidence, not to be pooled** (Part VII §3, Inferences):

1. **Fidelity to the simulator across held-out parameters.** Lamperti et al.'s surrogates (§0.4, item 2); Cozzi and Pangallo's graph diffusion networks, which reproduce individual behaviour in two canonical, non-economic agent models [V].
2. **Forecast accuracy against data for aggregates.** Poledna et al. [S]; Wiese et al., whose data-driven model calibrated for all 38 OECD countries beat the original and an AR(1) "by a highly statistically significant margin" [V abstract].
3. **Recovery of known truth in synthetic tests.**

Only the second speaks to reality, and only for the variables and horizons forecast. None tests counterfactuals, which have no out-of-sample truth except natural experiments. "Digital twin of an economy" in the found literature means a data-initialised agent model forecasting aggregates. Its claim to be a valid engine for counterfactuals rests on the plausibility of its behavioural rules, not on held-out evidence (Part VII §4, Inferences).

**Uncertainty is the weakest link.** Surrogates report point metrics and bands over repeated runs, simulation-based inference reports parameter posteriors, and diffusion-model surrogates report a predictive distribution without checked calibration. Conformal or coverage-checked predictive intervals for economic surrogates were not found (Part VII §3, Inferences).

**Preferences learned from behaviour are a set.** Inverse reinforcement learning and dynamic discrete choice share a non-identification result: many rewards rationalise the same behaviour [V]. A learned agent used to evaluate counterfactual allocations should report decisions that hold across that set (Part VII §3.5, §6.2).

**Part VII §6 builds in an order set by what can be checked against reference solutions:**

1. densities as balance laws on kinesis, held to HACT's finite-difference solutions and to closed-form stationary states;
2. latent space models on tyche with the identifiability group declared as data, every reported quantity checked for invariance under it, and the ECI as a corollary of the spectral embedding;
3. surrogate-assisted evaluation by active learning on heuresis, with calibrated intervals and a rule that any allocation chosen on a surrogate is re-run in the declared model;
4. learned laws and transfer operators on low-dimensional states, ranked on a part kept aside.

### 1.12 Classical Laws Learned from Economic Records Are Mostly Forecasters, and What the Record Cannot Identify Must Be Reported with Them

**The econometric tradition learns laws of motion three ways** (Part VIII §1):

- linear reduced forms with identified shocks (VARs, local projections, factor models);
- linear-Gaussian state-space likelihoods by the Kalman filter (nowcasting; estimated DSGE models);
- explicit regime processes (Markov switching, drifting parameters).

**The evidence that they recover something real out of sample is modest.**

- *Local projections and VARs* estimate the same impulse responses in population [M; DOI checked]. In samples they trade bias against variance, and Bayesian VARs are preferred for precision [V abstract].
- *Medium-scale DSGE models* forecast inflation and growth very poorly in absolute terms, and so do statistical and judgemental forecasts [S].
- *Nowcasting* with factor models is the clearest operational success.
- *Nonlinear machine-learning forecasts* help mainly in episodes of high uncertainty and financial stress, with "nonlinearity" the true game changer [V abstract].
- *Structure can hurt.* A 2026 cross-country stress test found that structurally constrained physics-informed and universal differential equations did worse than ARIMA and plain neural ODEs on sparse annual macro data: structural priors "can act as misregularizers" [V abstract].

Few methods give a law whose terms can be read and transplanted, and most "learned dynamics" papers in economics validate forecasts, not laws (Part VIII §1, Inferences).

**Equation discovery has not produced a confirmed economic law** (Part VIII §2). Its successes are recoveries of known models from synthetic data: the Goodwin cycle by SINDy, the Solow–Swan equation by OccamNet, EDMD on a simulated business-cycle model. The one real-data test, of SINDy and a SINDy-plus-UDE pipeline on US Goodwin variables from 1948 to 2025, found the key interaction term at no regularisation strength [S, an SSRN preprint]. The failure modes are those the debate on chaos exposed:

- short samples spanning several regimes;
- caricature laws at the observed scale;
- derivative estimation amplifying measurement noise.

A sparse law from economic data should be reported as one candidate in an ensemble, with inclusion probabilities, a linear comparator, an out-of-sample test and a synthetic-recovery test at the record's length and noise (Part VIII §2, §6).

**The verdict on chaos has been stable since the mid-1990s** (Part VIII §3, [V text]):

- strong evidence of nonlinearity;
- no credible evidence of low-dimensional deterministic chaos;
- little out-of-sample gain from nonlinear forecasting except in stress.

Barnett and Serletis add that no test can locate chaos "within the structure of the economic system" rather than in chaotic external shocks. The dominant nonlinearity is stochastic and depends on state: regimes and conditional heteroskedasticity. Learned economic laws should therefore be stochastic, not deterministic flows with noise added afterwards.

**Stochastic estimation is guaranteed only within its model class** (Part VIII §4).

- *Within the class:* Aït-Sahalia's closed-form likelihood expansions converge to the true maximum-likelihood estimator [S].
- *The decisive check is simulation from the fitted null.* Chapman and Pearson showed that celebrated nonlinear drifts of the short rate reappear when the same estimators are applied to paths of a linear-drift diffusion [S].
- *For distributions, transition speed is the informative test.* Many laws share a Pareto tail, but standard random-growth models generate transitions of top inequality about an order of magnitude too slow (Gabaix, Lasry, Lions and Moll, [S]).
- *Densities can be states.* Chang, Chen and Schorfheide's functional state-space model treats a cross-sectional density and the aggregates jointly, with the sampling error of estimated densities in the measurement equation [S].

**What the record cannot identify must be reported** (Part VIII §5):

- *structural non-identification:* parameters not determined by the observables at any sample size, shown by rank conditions;
- *practical non-identification:* a flat likelihood at this sample size, shown by profile likelihood or ensemble disagreement;
- *model-class non-identification* for equation discovery: several sparse laws fit equally well.

Identification problems are "widespread" in estimated macro models (Canova and Sala, [S]).

**The Lucas critique becomes a support condition.** A law learned under one policy regime is a reduced form valid for the range of policy variation in the record. An evaluator flags any proposed intervention outside that range as "out of support of the record". Because stability tests have low power (Rudebusch), "no break detected" must not be read as "invariant" (Part VIII §5, Inferences).

**Benchmarks are mandatory.** A learned law must beat four comparators out of sample, under Diebold–Mariano, Clark–West and Giacomini–White tests:

- a random walk;
- an AR or Bayesian VAR;
- a factor model;
- an equal-weight combination (in M4, 12 of the 17 most accurate methods were combinations of mostly statistical methods; Part VIII §5.4).

It must also reproduce impulse responses estimated by local projections, which it was not fitted to. A law whose responses fall outside the local-projection bands has learned forecasts, not dynamics (Part VIII §6, A.4).

**Learned laws as evaluators of order.** By superposition, order effects come only from the nonlinear, state-dependent terms that the economic record identifies worst. A learned law used as an evaluator of allocation orders must therefore do four things (Part VIII §6 B):

- evaluate with an ensemble (posterior draws, a bootstrap ensemble of sparse laws, a regime-switching posterior and the linear benchmark), and report whether the ranking of orders is stable across it, as a dual verdict;
- attribute every order effect to the terms that produce it, with their identification status, so that a preference resting on terms with low inclusion probability is "undecided by the record";
- flag allocations outside the record's support, widening or refusing their intervals, as tyche already withholds estimates outside their area of applicability;
- serve as a cheap inner-loop evaluator whose highly ranked orders are re-evaluated by a more structural one.

Part VIII §6 C gives a minimum reporting contract per method.

### 1.13 Quantum-Like Models Earn a Place Only Where Their Parameter-Free Constraints Are Tested and Hold

**One prediction has been tested at scale and largely held.** Of all quantum and quantum-like economic models, only the Lüders-projection model of question order makes a sharp, a priori, parameter-free prediction that has been tested at scale and largely held (Part IX §1).

- **The prediction.** The QQ equality, q = [p(AyBy) + p(AnBn)] − [p(ByAy) + p(BnAn)] = 0, follows from sequential projective measurement when nothing intervenes between the two questions.
- **The result.** Across 66 Pew surveys, the q-test statistics did not deviate from the null (p = 0.4625), while order effects did (p = 0.0004). Paired context effects correlated at r = −0.82 against a predicted slope of −1 [V].
- **Where it failed.** A Gallup pair violated the equality. The authors attribute this to information inserted between the questions and excluded it, and they concede the test is informative only when order effects are well above zero [V].

**The prediction is not exclusive to quantum probability, and the stronger law fails.** Classical repeat-choice models reproduce the mirrored, QQ-like order effects [S]. A correlated-prior Bayesian update model accounts for order effects with fewer parameters than its quantum counterparts, though by its symmetry assumptions it need not satisfy QQ [V-abs]. The full quantum law of reciprocity, the "Grand Reciprocity" equations, fails for the non-degenerate versions of existing quantum-like models in most cases [V-abs]. The disjunction effect that motivated interference models replicates poorly [S]. Contextuality in the Contextuality-by-Default sense, the stricter non-classical signature, was found in none of a broad range of behavioural and social datasets, poll order effects among them, with a few positive designed experiments [S].

**The open-system preference model makes a distinctive prediction, but its evidence is in-sample.** Kvam, Busemeyer and Pleskac's model, dρ/dt = −i(1−α)[H, ρ] + α·L(ρ), predicts that mean preference strength oscillates over deliberation time where a classical random walk approaches smoothly. Two experiments found systematic oscillation [V]. But the model nests the Markov one. It was fitted by least squares to mean trajectories and compared by adjusted R², not on held-out data. A classical model with a second-order, oscillatory component would be the comparator of equal flexibility (Part IX §1, §2, Inferences).

**Flexibility must be counted honestly** (Part IX §2, Inferences, a derivation). A Lindblad generator on a d-level system has d⁴ − d² real parameters (12 for d = 2), against d(d − 1) rates for a continuous-time Markov chain on d states (2 for d = 2). The classes of equal flexibility are:

- for time series, observable operator models (of which hidden quantum Markov models are a subclass), not hidden Markov models;
- for order effects, repeat-choice or correlated-prior models.

Classical outcome data identify at best the classical observation process. The Hilbert-space dimension, the basis and the phases are not identified without tomographically complete measurements, which choice experiments and markets do not supply.

**"Quantum finance" is classical stochastic mathematics in quantum notation, and quantum games are new games** (Part IX §1, Inferences).

- *Quantum finance.* Baaquie's field theory of forward rates and the quantum-oscillator models of returns are Gaussian random fields and Fokker–Planck operators rewritten as Schrödinger operators [M for the general point]. Their empirical claims are in-sample fits against named classical benchmarks, a fair test of the stochastic model and no test of anything quantum.
- *Quantum games* include an entangling device in their rules. They say nothing about behaviour in the original game, and the one experiment with people found that they did not play the predicted equilibria [S].

**Quantum computation offers no credible advantage for economic dynamics on real data** (Part IX §3).

- *Provable speedups need fault tolerance at scale.* The speedups are quadratic, for Monte Carlo pricing by amplitude estimation. They need about 8,000 logical qubits and a T-depth of 54 million executed in about a second [V-abs], or 4,700 logical qubits and 10⁹ T-gates at 45 MHz after later reductions [S]. Quadratic speedups are argued to be insufficient on early fault-tolerant machines [S].
- *The HSBC–IBM bond study* reported a relative gain of up to about 34%. It appeared only with data transformed on hardware and vanished under noiseless simulation; the authors suggest that hardware noise contributes to the effect [V-abs; V for Aaronson's critique].
- *Benchmarks of quantum machine learning* find out-of-the-box classical models beating twelve popular quantum models, while about 40% of surveyed papers claimed the reverse [S].

**Three verdicts must be kept separate** (Part IX §4, §6.2):

- *non-commutative,* which any sequential classical update with memory also is;
- *Lüders-consistent,* meaning the QQ and Grand Reciprocity constraints hold;
- *contextual,* by Contextuality-by-Default criteria.

**An experiment that separates the explanations.** Part IX §6.3 proposes one for allocation. Decision-makers allocate to A and to B in counterbalanced order, with no information between the two. The Lüders model predicts the QQ equality for each pair, while a classical model of resource state (a budget depleted) predicts order effects that do not satisfy it in general. The test separates "judgement constructed by eliciting it" from "the state of resources changed". Where the allocation is made by a mechanism (a picking sequence, an agenda), order-dependence is a theorem of the mechanism and needs no quantum model.

**What to build, and what not.** Part IX §6.1 recommends three things:

- the Lüders model of sequential judgement with QQ and Grand Reciprocity tests and its classical rivals beside it;
- the Kvam model as a kinesis declaration nested over its Markov limit, replicated first on its open OSF data;
- hidden quantum Markov models compared with tyche's hidden Markov models and with observable operator models of equal class.

It recommends against building Baaquie-style models or quantum oscillators as quantum, and against building quantum games as economics, and against any claim of computational advantage.

### 1.14 Each Kind of Evaluator Establishes a Different Thing About Order

The Parts converge on one reading of how evaluators bear on order: each locates the source of order effects differently, and each can establish something different (Table 1.5). A framework that lets evaluators plug in must carry, with each verdict on order, which evaluator gave it and what kind of claim it is.

**Table 1.5.** Evaluators of allocation order: where order effects come from, and what each can and cannot establish.

| Evaluator | Where order effects come from | Can establish | Cannot establish | Parts |
|---|---|---|---|---|
| Grant system with declared actions | Depletion, responsive prices, capacities, information revealed, changing eligibility | Exact order effects under the declared actions; commutation checked on reachable states; the trace quotient | That the world's actions are the declared ones | II §1, §6; IV §3 |
| Mechanism simulation (picking sequences, serial dictatorship, reserve precedence) | The mechanism's agenda | The outcomes of every order, as a set | Monotonicity of precedence in general | II §1, §6; §0.4 item 13 |
| Agent simulation with payments and balance sheets | Timing, liquidity queues, network position, behaviour that depends on state | Rankings of orders under one calibrated, conservation-checked model, robust across behavioural kinds and parameter ranges | That the mechanism is the actual one; behaviour outside validated ranges; any result that rests on a leak | VI §6.2, §6.5 |
| Declared density law | Maps on densities that do not commute (an allocation changes the density, the mean field, every later policy) | The exact order effect under the model, with conservation checked | That the economy behaves so | VII §6.2 |
| Learned linear operator (transfer, Koopman) | Its commutator K_{a2}K_{a1} − K_{a1}K_{a2} | Order-dependence of the simulator within the support of its training data | Anything outside that support; reliability with few agents | VII §2.4, §6.2 |
| Surrogate in the inner loop | Inherited from the simulator it emulates | A ranking within its validated region, with stated error | The final choice; positives must be re-run | V §6.3; VII §6.2 |
| Ensemble of learned laws | Nonlinear and state-dependent terms (under a linear time-invariant law, only timing and discounting) | Whether the ranking of orders is stable across the ensemble | A preference resting on terms with low inclusion probability; allocations outside the record's support | VIII §6 B |
| Learned agents (inverse reinforcement learning, multi-agent RL) | Behaviour responding to the allocation | Effects that hold across the identified set of preferences | One rationalising preference taken as the truth | VII §3.5, §6.2 |
| Lüders model of judgement | Judgement constructed by eliciting it (non-commuting projectors) | Whether counterbalanced data satisfy QQ and Grand Reciprocity | That minds are quantum; order effects caused by depleted resources or by information between the allocations | IX §4, §6.3 |

### 1.15 What the Design Should Be: Resource Types, Spaces, Order, Evaluators and Verification

The following points gather the Parts' design sections (Part I §6 to Part IX §6) into one design, and say where each piece belongs under the infrastructure's placement rules: general methods in the foundations, objects that name something in nature in a domain, and whatever touches the world at the edges and in works.

**1. Resource types as declared data, each property a scoped verdict** (Part IV §6). A resource type is a finite product of components. Each component declares:

- its kinds;
- its quantity semiring (ℕ, (1/q)ℕ, ℚ≥0 or ℝ≥0, ℤ or ℚ, 𝔹 under ∨ or max);
- its combination (+ or an idempotent join);
- its generating conversions, as Petri-net transitions;
- free disposal per kind (refused for chores and wastes);
- an optional validity predicate closed under sub-combination;
- its declared properties (cancellative, catalysis-free, splittable, refinement, quantity-like or quality-like, waste-free);
- its conservation laws as homomorphisms to ℤ.

Each property is answered by kategoria with a verdict scoped as in §1.9:

- by construction for products of verified primitive semirings;
- on generators for homomorphisms;
- exhaustively on finite models;
- by bounded search with a witness or a monotone for convertibility.

Exact rationals are used throughout, never floating point. The general structure (ordered commutative monoids, resource algebras, presentations, their checks) names no object in nature, so it belongs in a foundation. Part IV §6 places its verification with kategoria, which already holds symmetric monoidal theories and matrix models. A domain's resource types, such as hospital beds, money or water rights, are declared in that domain as data.

**2. Allocation spaces derived from resource types, with the contract of a space** (Part III §6; Part IV §6). The species of a resource type gives its allocation space:

- weak compositions for identical units;
- assignments for labelled items;
- a scaled simplex for divisible amounts;
- products for mixed pools.

The species serves enumeration of finite models, counting, and uniform sampling by a Boltzmann sampler. Each space implements Part III's contract (§1.3) and its law checks. Those checks are sampled evidence unless the space is finite or the law holds by construction, and the report says which. The divisible simplex uses tyche's Aitchison geometry as its chart. Manifold-valued attributes use the chart, retraction, transport and trust-radius interface. These belong in heuresis's representations.

**3. Genotypes as records of typed blocks, with two representation routes** (Part I §6; Part III §6). The block types are:

- real shares, normalised to a budget or capacity;
- integers;
- categories with eligible domains;
- permutations or random keys;
- partitions;
- sums with a floored branch switch and a declared policy for inactive payloads;
- sequences, sets and multisets;
- manifold attributes.

Each block owns its sampler, mutation and crossover, applied block-wise; product distances are normalised by declared units. Around the blocks:

- Margins prevent fixation of discrete marginals in model-based samplers, with α ≈ 1/(Nλ).
- Phenotypic duplicate elimination is on by default.
- Raidl and Gottlieb's static representation measures are computed before an encoding is committed.
- The direct route (repair and local improvement) and the keys-and-decoder route run on the same instances, and the generality premium is reported.

These extend heuresis's existing real, integer, bit-string and permutation variation.

**4. Order as a grant system whose commutation structure is data** (Part II §6; Part IV §3, §6).

- *The grant system.* A resource system declares its state type, grant alphabet, partial action or kernel, initial state and value.
- *Commutation.* An independence relation is declared, or estimated by commutator tests on sampled reachable states. If it is total, set or count encodings are used and no permutation is searched. Otherwise words are canonicalised to a trace normal form before evaluation, recombination and archiving, and evaluations are cached by it.
- *Decoders* are pluggable, each stating its dominance property: a generalised serial scheme, a parallel non-delay variant, greedy first-feasible.
- *Operators* are indexed by the feature they preserve, with a feature probe and adaptive operator selection. Plackett–Luce, Mallows and generalised Mallows (Kendall, Cayley, Ulam, Hamming), histogram and key models sit behind the same sampler interface, with spread exposed for diversity control and restarts.
- *Sequences or policies.* An adaptivity-gap estimator tells the user whether a sequence suffices. Policies are offered as priority functions over declared state features evolved by genetic programming, neural policies optimised by evolution strategies, and programs proposed by a language model and filtered by evaluators (heuresis's proposal channel, never a judge). Rolling-horizon re-optimisation is the middle option, with anticipation terms for problems with time-linkage.
- *Mechanisms.* Picking sequences and reserve precedence orders are genotypes in their own right, with outcomes over every order presented as a set. The allocation domain's declared stages carry them.

The general machinery belongs in heuresis. The allocation domain supplies the mechanisms and principles it searches over.

**5. The evaluation record, and noise, cost and trust policies** (Part V §6).

- *The record.* heuresis's `Evaluation` is extended with the record of Table 1.3, keeping its order: Deb's rule over the trusted objectives. Its planned `noise` module holds the selection procedures, each labelled with its guarantee:
  - replications from a power calculation for a declared smallest meaningful difference;
  - races, OCBA and resampling to a standard-error threshold;
  - the rank-change diagnostic;
  - common random numbers by keyed draws, kept only where they reduce the variance of differences.
- *Cost.* Cascades and surrogate model management with tracked agreement; generational algorithms where costs differ widely.
- *The archive.* Every evaluation is keyed by (input digest, evaluator identity, version, fidelity, random-stream key), recorded append-only and sealed in archeion's journal, as project archives already are.
- *Combination.* In tiers (§1.8), with dual verdicts in kybernesis's form and equity as objectives, constraints and descriptors computed from per-unit outcomes.
- *Placement.* Evaluators that call simulators on clusters, language models or people live at the edges and in works, under heuresis's own scope.

**6. An agent simulation of circulation as the first economic evaluator** (Part VI §6).

- *Balance sheets.* Every agent has one, holding real stocks and typed claims (deposits, loans, bonds, currency, reserves, payables, community-currency balances).
- *Postings.* A transaction is a posting of four entries, the only way any balance changes.
- *Money.* Creation and destruction are named acts with a responsible agent.
- *"Outside"* is a distinguished node, so that leakage is a flow, not a disappearance.
- *Conservation* is checked every step as a verdict. A run that fails is refused with the first failing posting named, as archeion's journal names the first entry that does not seal.
- *Timing.* Payments settle with explicit timing and liquidity queues.
- *Initial state.* It comes from accounts: an input–output table or social accounting matrix with sectoral balance sheets.
- *Behaviour.* Written rules, learned policies and grade-C language-model proposals stand behind one interface. Reinforcement-learning adversaries probe the allocation rule for leaks and gaming.
- *Fidelity.* The cascade runs from a static input–output or Markov-chain retention estimate, through a dynamic input–output model with inventories, to the full simulation.
- *Readings.* The circulation measures of §1.10 are read from the posting log, the economics domain's Hodge decomposition among them.

The general agent machinery belongs in nomos, whose `agents` already holds stated schedules, Gillespie simulation, draws by identity and event logs. The accounting and circulation measures name money and belong in the economics domain, beside its `circulation` and the markets domain's `money`. The first validation record is Sarafu's open dataset.

**7. Declared density laws and learned representations** (Part VII §6).

- *Density laws.* The HJB–Kolmogorov system and kinetic exchange models are built as balance laws of a density on a graph of cells in kinesis's `balance`. Whether `balance` admits a drift that depends on a solved policy is unchecked (Part VII §6, Gaps). The laws are held to HACT's reference solutions, generated by running the authors' codes, and to closed-form stationary states.
- *Latent space models* live on tyche, with the identifiability group as data.
- *Surrogates* come with conformal or Bayesian predictive intervals whose coverage is checked on held-out runs of the true model, and with mandatory re-evaluation of anything chosen on them.
- *Learned operators* (the transfer operator on a partition, EDMD) give commutator readings of order within their support.

**8. Learned classical laws with a reporting contract** (Part VIII §6).

- *A measurement layer* for kinesis's declarations: an observation equation, with likelihoods from tyche's Kalman, extended, unscented or particle filters.
- *Regime-switching and drifting-parameter laws,* sharing one representation with the detectors of critical transitions, so that a detected break becomes a regime in the law.
- *Mandatory baselines,* as declared objects, with tests of predictive accuracy.
- *Local projections* as a falsification test.
- *A Chapman–Pearson simulated null* for every nonparametric drift or diffusion.
- *Distribution dynamics* with transition speed reported.
- *Harris–Wilson spatial dynamics* with Bayesian or neural calibration.
- *Conformal wrappers* for forecasts.
- *Ensemble sparse discovery* with inclusion probabilities and synthetic recovery at the record's length.
- *The reporting contract* of Part VIII §6 C for every method.

These extend kinesis and tyche.

**9. Quantum-like models only as tested models of judgement** (Part IX §6).

- *The Lüders model* of sequential judgement, with QQ and Grand Reciprocity tests and classical rivals in the same module.
- *The Kvam open-system model* as a kinesis declaration over its Markov limit.
- *Hidden quantum Markov models* compared with hidden Markov and observable operator models.
- *Honest counts:* parameters counted, models nested, fits by likelihood, comparison by held-out log score, parameter-free predictions preregistered with peira's `protocol`, the three verdicts of §1.13 kept apart.
- *Not built as quantum:* quantum finance models are built, if at all, as the classical stochastic models they are.
- *No claim of computational advantage.* heuresis's exact emulation of quantum search, and an emulated amplitude-estimation pricer, may serve learning with resource estimates stated beside them.

**10. Benchmarks and baselines** (Part I §5, §6; Part VII §5; Part IX §5).

- *Adapters for allocation:* the OR-Library's multidimensional knapsack sets (Chu–Beasley, Glover–Kochenberger), PSPLIB, MMLIB, iMOPSE and the Snauwaert–Vanhoucke sets, vector bin packing, and the mixed-variable suites of CatCMA with Margin.
- *Benchmarks for the economic evaluators:* Krusell–Smith and Aiyagari for densities; Brock–Hommes and Islands for agent calibration; Schelling for emulators; Sarafu for circulation; the Pew files and Kvam's OSF data for quantum-like models.
- *Reporting:* gaps to optima, best-known values or LP bounds, under equal evaluation budgets, against an exact or constraint-programming baseline where size allows.
- *A missing library.* No library of genuinely heterogeneous allocation exists (Part I §5). The framework should produce that evidence itself, by running both representation routes on the same instances.

**11. An order of building, set by reuse and by what can be checked.** This order is an inference of the synthesis.

1. Resource-type declarations with kategoria's scoped verdicts, and allocation spaces derived from their species: the layer everything else stands on, checkable against finite models and closed forms.
2. Typed genotype blocks, decoders and the grant system with its independence relation and trace normal forms, validated on the benchmark libraries above.
3. The evaluation record and the noise, cost and trust policies in heuresis.
4. The agent simulation of circulation with stock–flow consistency, held to reference implementations and validated on Sarafu.
5. Declared density laws, held to reference solutions.
6. Learned laws with the reporting contract.
7. Quantum-like judgement models, only as tested models of judgement.

Each step can be checked before the next rests on it.

### 1.16 What Must Stay a Person's Judgement or Be Flagged

**What stays with people.** The Parts name the choices that carry the content of an allocation and must not be fixed silently by a default.

- **The algebra of a resource:**
  - whether a resource is freely disposable or a burden;
  - which conversions count;
  - which goods are of one kind, that is fungible;
  - whether an indivisible unit may serve two recipients (Part IV §1, §6).
- **The geometry of a space:**
  - the metric of each space, which encodes which features count as building blocks (Part III §6);
  - the units and weights of product distances, which decide which part dominates (Part III §5);
  - the alignment rule (gauge) for latent dynamics (Part VII §1).
- **The structure of order:**
  - an independence relation that cannot be estimated;
  - the precedence orders and categories of mechanisms (Part II §6).
- **The terms of comparison:**
  - the smallest meaningful difference, which sets replications;
  - the guarantee levels of selection (Part V §6.2);
  - the trust thresholds at which proxies are demoted (Part V §6.5).
- **The terms of equity:** which measures of equity, which thresholds and floors, and their sources (Part V §5).
- **The parameters of models:**
  - behavioural parameters the record does not identify, declared with ranges (Part VI §3);
  - the production function in propagation models, which moved Pichler et al.'s shock from a few points to 60% (Part VI §6.5);
  - identifying assumptions of structural VARs and the number of regimes (Part VIII §6 C).
- **The final choice** among a Pareto front or a quality-diversity archive. A person makes it, with reasons recorded through the allocation domain's sealed procedure, since a computed front is a repertoire, not a verdict (Part V §6.7).

**What a tool must flag in every report:**

- a candidate that wins only on proposed metrics (Part V §6.7);
- an allocation chosen on a surrogate and not re-run in the true model (Part V §6.3; Part VII §6.1);
- an allocation outside the record's support (Part VIII §6 B);
- a failed conservation check (Part VI §6.1);
- the share of model-generated behaviour (Part VI §4);
- stochastic decoding that makes evaluation noisy (Part III §3);
- a decoder that may exclude every optimum (Part II §4);
- a language-model judge whose audited agreement has fallen below its threshold, and the gap between its scores and the audited ones (Part V §6.5);
- a ranking of orders that rests on terms with low inclusion probability (Part VIII §6 B);
- a quantum-like fit that degenerates into its classical limit (Part IX §2);
- reliance on the withdrawn Norman, Rivera and Hughes paper, or on any source marked [M] (§0.4).

### 1.17 What a Tool Must Never Claim

The prohibitions of the nine Parts share one form: presenting a scoped verdict as an unscoped one. Gathered:

1. That an allocation is optimal or good without qualification. It may say "best found under evaluators E with budget B and selection guarantee g", or "non-dominated under the verified and computed objectives" (Part V §6.7; Part II §6).
2. That a law holds for a resource type when it was checked on a truncated finite model, on samples or in floating point. A finite "holds" names its model, a construction proof its closure theorem, a sampled "no counterexample" its sample size and distribution (Part IV §4, §6).
3. That a conversion is impossible without a monotone or invariant as certificate, or possible without a witness. "Undecided within the bound" is the honest answer to a hard instance (Part IV §4).
4. That an operator is geometric, respectful or equivariant without a check, or that the literature already holds a categorical specification of evolutionary operators (Part III §1; Part IV §5).
5. That the order of allocation is immaterial without commutation checked on reachable states. Also that a ranking of orders under a linear time-invariant law reveals complementarity, or that non-commutativity is evidence of quantumness (Part II §1; Part VIII §6; Part IX §4).
6. That a surrogate's or a language model's score is a finding. Also that a language model is a reliable judge inside an optimisation loop, or that a surrogate of a person can replace the person without held-out agreement (Part V §3, §6).
7. That a candidate winning by an accounting leak is good, or that a simulation result stands when its run failed conservation (Part VI §2, §6).
8. That a simulation identifies the actual mechanism, or that reproducing stylised facts validates a model. Also that a data-driven agent model's skill at forecasting aggregates validates its counterfactuals as a "digital twin" (Part VI §6.5; Part VII §4).
9. That language-model agents' behaviour is human behaviour beyond validated ranges, or that reinforcement-learning agents show what people do (Part VI §4).
10. That individual latent coordinates mean something, that the Economic Complexity Index is derived from theory, or that a hyperbolic embedding with no held-out test is predictive (Part VII §1).
11. That a learned economic law is "the" law, or that equation discovery has found an accepted economic law. Also that "no break detected" means invariance, or that a law learned under one policy regime holds outside the record's support (Part VIII §2, §5).
12. That a quantum-like model shows minds, markets or economies to be quantum, or that in-sample fit of a more flexible model is evidence. Also that a quantum finance model is quantum rather than classical stochastic, or that quantum computation offers an advantage for economic dynamics now (Part IX §1, §3, §6).
13. That minimising a Gini coefficient alone is equitable, or that a Gini of means is a mean of Ginis (Part V §5).

### 1.18 What the Survey Leaves Open

- **Heterogeneity itself.**
  - No study compares encodings on problems with several resource types at once, some divisible and some not.
  - No benchmark library exists for such allocation or for public budgets.
  - No verified head-to-head comparison of encodings exists for placing virtual machines, ambulances, beds or spectrum (Part I §1, §2, §5).
- **Order.**
  - No source uses monoid or trace-monoid language for allocation.
  - The trace canonicalisation, commutator tests and feature probe are untested proposals.
  - No head-to-head study of evolved sequences against evolved policies on one allocation problem was found (Part II §1, §3, §6).
- **Spaces.**
  - The sum-type metric is unevaluated.
  - No joint covariance is known across parts with no common embedding.
  - No provably geometric crossover is known for dependent products or recursive types beyond homologous crossover.
  - No evolution strategy in Aitchison coordinates exists as a named method.
  - No systematic study of scale normalisation exists (Part III §2, §5, §6).
- **Categories.**
  - No categorical specification of evolutionary algorithms was found, and none of fair division or of sequential mechanisms.
  - No result on the decidability of catalysis or on computing Fritz's regularised rates in finitely presented monoids was found.
  - Operators as natural transformations between species, and algorithms as stochastic selection functions, are proposals (Part IV §2, §4, §5).
- **Evaluators.**
  - No study tracks trust across several heterogeneous evaluators in one run.
  - No study measures how often fitness judged by a language model is exploited inside an evolutionary loop, or how language models judge allocations or policies.
  - Most reliability figures are for 2023–2024 models (Part V §3, §4).
- **Circulation.**
  - No agent model has been validated against measured circulation statistics.
  - No simulation of community wealth building or validation of LM3 against transaction records exists.
  - No imitation learning of economic agents from transaction records was found.
  - No open, maintained C++ macro model with stock–flow accounting exists (Part VI §1, §3, §4, §5).
- **Learned representations.**
  - No learned simulator has been used to evaluate allocation sequences.
  - No calibrated predictive intervals exist for economic surrogates.
  - No learned dynamics in embeddings of firms or households have been read for inequality or circulation with out-of-sample checks.
  - The commutator reading of order is a proposal (Part VII §3, §4, §6).
- **Learned laws.**
  - No recovered structure of a learned macro law has been tested out of sample.
  - No standard exists for reporting non-identification of the model class in equation discovery.
  - The economic record of neural ODEs, universal differential equations and reservoir computing is thin and recent (Part VIII §1, §6).
- **Quantum-like models.**
  - No such model has been applied to allocation decisions.
  - No comparison against a classical model of equal class exists on economic time series.
  - No Lindblad parameters have been estimated from economic data.
  - No preregistered replication of the QQ equality after 2014 was found (Part IX §1, §2, §4).
- **Unread texts and unverified formulas** (Parts I–IX, Gaps):
  - Falkenauer's grouping argument;
  - the ranking of RCPSP representations;
  - Deb's, Coello's and Takahama and Sakai's recommendations;
  - the full texts of Gonda and Spekkens, of Meseguer and Montanari and of the separation-logic papers;
  - the published Chang, Chen and Schorfheide and Gabaix, Lasry, Lions and Moll;
  - Poledna et al.'s error tables;
  - the AI Economist's figures;
  - Wang et al.'s Supporting Information;
  - Yukalov and Sornette's quarter law.
- **Licences:** of BeforeIT.jl, JMAB, Eurace@Unibi, the AI Economist's Foundation, ABIDES-Economist, and most code and data listed in Parts VI, VII and IX.

## Part I. Encodings, Variation, Repair and Evaluation for Heterogeneous, Multi-Type Resource Allocation

*Remark.* Part I's scope and conventions. Markers: **[V]** verified by reading the source itself (abstract or full text) during this research; **[S]** from a search-result snippet or bibliographic record only; **[M]** from memory, not checked during this research (DOIs marked [M] may be wrong and must be checked before citation). Where a [M] claim is given a link, the link is a pointer for checking, not evidence.

### 1. Encodings That Handle Heterogeneity Without Losing Feasibility or Heritability

#### Takeaway
No single encoding handles all kinds of heterogeneity. Each successful family puts feasibility in one of three places: in the genotype space (grouping, permutation and feasibility-preserving direct encodings), in a decoder (random keys, priority lists, weight-biased decoders), or in a repair step (direct encodings with heuristic repair). Each placement has a documented failure mode: redundancy and flat landscapes for decoders, the "feasibility problem" for penalties, and weak heritability for encodings whose genes have no stable meaning (ordinal encodings, assignment vectors under standard crossover).

#### Cited Findings

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
- **The random-key optimiser (RKO, 2024–2025)** carries the idea beyond GAs. Solutions are random-key vectors "subsequently decoded into feasible solutions via problem-specific decoders". The metaheuristics, eight in the C++ framework, work only in key space and run alone or in parallel. It was tested on α-neighbourhood p-median, tree-of-hubs location and node-capacitated graph partitioning. Published in J. Heuristics (2025). [S] — [Chaves, Resende, Schuetz et al., arXiv 2411.04293](https://arxiv.org/html/2411.04293v3). A 2026 preprint applies RKO to mixed-integer programs with problem-specific decoders. [S] — [arXiv 2602.22173](https://arxiv.org/html/2602.22173v1)
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

#### Inferences
- Heterogeneity across resource types (divisible or indivisible, different units) is best handled by **typed gene blocks, each with operators of its type**, rather than by one homogeneous string. Real genes for divisible amounts should be normalised (shares of a budget or capacity) so that units do not distort mutation step sizes. This is an inference from the MIES, pymoo and CatCMA designs (§3) and from the failure of encodings whose genes have no stable meaning (OR).
- Decoders "solve" feasibility at the price of redundancy and a weaker link between genotype and phenotype. The empirical remedy is phenotypic duplicate elimination, and a fitness that grades the inside of a plateau (Falkenauer's and Gonçalves–Resende's bin-packing fitness).
- Grouping encodings are the natural representation when what matters is *which items share a resource* (a server, a crew, a bin), not which resource gets which label. Assignment vectors under standard crossover break groups apart. This argument is Falkenauer's [M], and consistent with the later grouping-GA applications to VM placement [S].

#### Gaps
- No study found that compares encodings on a problem with **several resource types where some are divisible and some indivisible** at once. The evidence is assembled from single-type problems (MKP, bin packing, RCPSP).
- Falkenauer's 1996 paper and 1998 book were not read. The argument that standard encodings of grouping problems are redundant and poorly heritable is from memory [M].
- Gonçalves and Resende (2011) was not read directly; its parameter recommendations (elite share, mutant share, ρ ≈ 0.7) are [M].

### 2. Direct Versus Indirect (Decoder-Based) Encodings: Comparative Evidence and Application Families

#### Takeaway
The best controlled comparison (Raidl and Gottlieb on the MKP) found a **direct encoding with heuristic initialisation, repair and local improvement** best, and increasingly so on the largest instances. Among decoders, the heuristically biased ones (weight-biased, permutation, random keys) were close to one another and the ordinal one clearly worst. Locality, heritability and heuristic bias explained the ranking. In precedence-constrained multi-resource scheduling, the literature converges on activity lists with a serial SGS. Random keys are competitive and generic but not dominant.

#### Cited Findings

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

#### Inferences
- **"Direct beats indirect" is conditional.** It holds when the direct encoding is paired with problem-informed repair and local improvement, which is itself a decoder applied after variation. The decisive variables are heuristic bias, locality and heritability, not the label "direct" or "indirect".
- **Random keys pay for generality.** They are the cheapest way to obtain feasibility and generality: one GA engine, problem knowledge confined to the decoder. On the MKP they matched permutations but lost to well-built direct encodings, by a margin that grows with size. That is a cost a universal framework should expect and measure.
- **Precedence and time favour order genotypes.** Where the constraints concern order (precedence, time windows), priority and order genotypes decoded by an SGS dominate the literature. Where they concern packing, grouping genotypes and fill-based fitness do.

#### Gaps
- Hartmann (1998) and Kolisch and Hartmann (2006) were not read in full, so the precise RCPSP representation ranking is [S]/[M].
- No verified head-to-head comparison of encodings for VM placement, ambulance location, ICU beds or spectrum allocation was found. The application literature mostly reports one encoding against classical heuristics.
- No 2023–2026 study was found that overturns the Raidl–Gottlieb ranking; the recent work (RKO, BRKGA variants) extends decoders without new controlled comparisons against direct encodings.

### 3. A Single Genotype Mixing Continuous Amounts, Integer Counts, Categorical Choices and Orderings

#### Takeaway
Mature tools represent a mixed genotype as a **named product of typed variables, each type with its own sampling, mutation and crossover**:
- pymoo's `vars` dict with `Real`, `Integer`, `Choice` and `Binary`;
- MIES's real, ordinal and categorical vector;
- CatCMA's joint Gaussian × categorical distribution.

Since 2022–2025 the CMA-ES line has handled all three numeric and categorical types jointly, with margins that prevent fixation. Orderings (permutations) remain outside these mixed-variable frameworks; they enter through random keys or as a separate permutation block.

#### Cited Findings
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

#### Inferences
- A universal genotype should be a **record of typed blocks**: real amounts, integers (with or without order), categorical choices, permutations or priorities, and partitions or groupings. Each block owns its sampler, mutation and crossover, and recombination is applied block-wise. This is the common structure of pymoo, MIES and CatCMA; adding permutations and partitions extends it to scheduling and packing.
- Fixation of discrete genes is a documented failure in model-based samplers and must be prevented explicitly (margins), not left to chance.
- Conditional genes (a mode's parameters active only when that mode is chosen) are handled ad hoc. Keeping inactive genes adds neutral redundancy, which Raidl and Gottlieb found harmful when not countered by phenotypic duplicate elimination.

#### Gaps
- No verified study evaluates permutation genes inside a mixed-variable ES or GA against keys decoded in the same framework.
- pymoo's operator defaults per type (SBX or PM for reals, rounding for integers, random resetting for choices) were not checked against its source code.
- No standard benchmark for conditional, hierarchical, mixed spaces was found (the search confirms the absence).

### 4. Constraint Handling for Budgets, Capacities, Indivisibility, Eligibility and Precedence

#### Takeaway
For allocation under budgets and capacities, the best-supported practice is to **keep the search on or near the feasible boundary**: decoders that produce only feasible, maximal allocations, or repair that removes the least efficient items and then refills. Static penalties are weak for knapsack-like constraints. For general numerical constraints, the standard techniques are:
- Deb's feasibility rules: parameter-free, but prone to losing diversity;
- stochastic ranking;
- ε-constrained methods, the strongest in competitions [M].

Hard structural constraints (indivisibility, eligibility, precedence) belong in the representation or the decoder, not in penalties.

#### Cited Findings
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

#### Inferences
- **Decide by constraint type:**
  - indivisibility belongs in integer, binary or grouping genes;
  - eligibility belongs in categorical genes whose domain is the eligible set, or a decoder that skips ineligible pairs;
  - precedence belongs in order genes and an SGS;
  - budgets and capacities belong in a boundary-seeking decoder or repair-plus-refill guided by an efficiency ratio (surrogate duals or LP reduced costs);
  - soft or numerical constraints go to feasibility rules, stochastic ranking or ε-constraints.
- Penalties should be the fallback, used only with a feasible or repaired initial population. Gottlieb's analysis implies initialisation matters for their success.

#### Gaps
- Deb (2000), Coello (2002), Takahama and Sakai, and the Mezura-Montes–Coello full text were not read; their detailed recommendations and evidence are [S]/[M].
- No study was found comparing these techniques specifically on heterogeneous multi-resource allocation (budgets plus capacities plus indivisibility). Evidence comes from the MKP and from continuous benchmarks (the CEC suites).

### 5. Benchmark Problems and Instance Libraries for Heterogeneous Allocation

#### Takeaway
The standard libraries are:
- OR-Library's MKP sets (Chu–Beasley, 270 instances) and Glover–Kochenberger's large MKPs;
- PSPLIB (single- and multi-mode RCPSP);
- MMLIB (multi-mode, including MMLIB+);
- iMOPSE and the 2023 Snauwaert–Vanhoucke sets for multi-skill scheduling.

Optima are known for small classes and bounds or best-known values for large ones. There is no standard library for allocations mixing divisible and indivisible resource types.

#### Cited Findings
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

#### Inferences
- A universal framework should be validated on at least one library per constraint kind:
  - MKP (budgets and capacities);
  - PSPLIB or MMLIB (precedence, renewable and non-renewable resources, modes);
  - iMOPSE (eligibility by skill);
  - vector bin packing (multi-dimensional capacities and grouping);
  - a mixed-variable continuous suite (divisible amounts with integer and categorical genes).

  Results should be reported as gaps to optima, best-known values or LP bounds, under equal evaluation budgets (schedules generated, as MMLIB studies do).

#### Gaps
- PSPLIB's and MMLIB's current best-known values, and how many instances remain open, were not verified.
- No benchmark library was found for **heterogeneous** allocation in the user's sense (several resource types with different units, some divisible), nor for public-budget allocation.

### 6. What This Means for a Universal Framework

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

## Part II. Order-Dependent Allocation: How Order Arises, and How Evolutionary Computation Represents and Searches It

*Remark.* Part II's scope and conventions. Research notes for the design of a universal evolutionary resource-allocation framework. Compiled 2026-11-20.

**Markers.** [V] verified by reading the source text itself during this research (full text or the relevant passage); [S] from a search snippet, catalogue record or abstract summary only; [M] from memory, not checked during this research (bibliographic details given as remembered, no DOI unless one was seen). A DOI is given only where it appeared in a source seen during this research. Inferences are kept apart from findings and are mine.

**Sources read in full text during this research:** Starkweather, McDaniel, Mathias, Whitley and Whitley (1991); Larrañaga, Kuijpers, Murga, Inza and Dizdarevic (1999); Cicirello (2023, arXiv); Chicano, Luque and Alba (2011, arXiv); Rothlauf (GECCO 2003, TV mapping); Nguyen, Yang and Branke (2012, the Optimization Online preprint); the abstract-and-references note of Ceberio et al. (EDA research of UPV/EHU, 2011–2024); the arXiv abstracts of Salimans et al. (2017), Kalinowski, Narodytska and Walsh (2013) and Pathak et al. (2020/2024). Everything else is [S] or [M].

### 1. Settings in Which the Order of Grants Changes the Outcome, and the Formal Models of Order-Dependence

#### Takeaway
The order of grants matters whenever a grant changes the state against which later grants are valued or made feasible: stock or capacity consumed, prices or rents that respond, learning and deterioration, spillovers through a network, precedence and sequence-dependent setups, and eligibility that changes; in economies with increasing returns early grants can lock in a path (Arthur 1989; David 1985). In mechanisms the order is itself a design parameter: picking sequences, serial dictatorship, and the precedence order in which reserve categories are processed, which changes who is served (Dur, Kominers, Pathak and Sönmez 2018). Formally, the common model is a state-transition system in which each grant acts on a state (deterministically, or as a Markov kernel, giving an MDP); order-independence is exactly commutativity of the grants' actions.

#### Cited Findings

**(a) Depletion of stock: the founding evolutionary case is itself an allocation problem.**
- Starkweather et al. (1991) applied a GA to a real "allocation of beer production and inventory to the orders" at the Coors brewery: each individual is "a sequence of customer order numbers", mapped to loading docks by a scheduling simulator that fills orders from production and inventory; a good schedule "minimizes the average inventory in the system, and fills as many orders as possible"; 195 customer orders in the test period [V] — [Starkweather et al. 1991, ICGA, pp. 69–76](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf); bibliographic record [dblp](https://dblp.uni-trier.de/rec/conf/icga/StarkweatherMMWW91.html).
- The authors explain why order matters there: "the relative order in which product is used will clearly affect inventory more than adjacency. Adjacency would appear to be almost irrelevant in this domain" [V] — [Starkweather et al. 1991](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf).

**(b) Increasing returns, path dependence and lock-in.**
- Arthur (1989), "Competing technologies, increasing returns, and lock-in by historical events", Economic Journal 99(394):116–131, models agents adopting one of two technologies, each improving as it is adopted; the economy can be "locked in, by 'random' historical events, to a technological path that is not necessarily efficient", the outcome hard to predict and hard to reverse; the model generalises Pólya urn processes [S] — [RePEc/IDEAS record](https://ideas.repec.org/a/ecj/econjl/v99y1989i394p116-31.html); [JASSS appendix on Arthur's model](https://jasss.soc.surrey.ac.uk/12/1/6/appendixB/Arthur1989.html); Pólya-urn framing in a [Cambridge working paper](https://www.jbs.cam.ac.uk/fileadmin/user_upload/research/workingpapers/wp0720.pdf).
- David (1985), "Clio and the economics of QWERTY", American Economic Review 75(2):332–337 (Papers and Proceedings): lock-in of a standard through technical interrelatedness (compatibility between keyboard "hardware" and the typist's "software" of skill), economies of scale and quasi-irreversibility of investment [S] — [EH.net encyclopedia, "Path dependence"](https://eh.net/encyclopedia/path-dependence/); [Strathclyde discussion paper 13-10](https://t4.is.strath.ac.uk/media/1newwebsite/departmentsubject/economics/research/researchdiscussionpapers/2013/13-10FINAL.pdf). David's framing is disputed by later authors (noted in the same secondary sources) [S].

**(c) Precedence, capacities and sequence-dependent setups (scheduling).**
- In the resource-constrained project scheduling problem (RCPSP), a priority list is turned into a schedule by a schedule-generation scheme (SGS): Kolisch (1996), "Serial and parallel resource-constrained project scheduling methods revisited: theory and computation", EJOR 90(2):320–333, gives the class of schedules each scheme generates and compares them as single-pass and sampling methods; "the parallel method cannot be generally considered as superior" [S] — [IDEAS record](https://ideas.repec.org/a/eee/ejores/v90y1996i2p320-333.html). Here the order of activity grants interacts with resource capacities that are consumed per period.
- Sequence-dependent setup times and costs make adjacency (the immediately preceding job) part of the cost: Allahverdi, Ng, Cheng and Kovalyov (2008), "A survey of scheduling problems with setup times or costs", EJOR 187:985–1032 [M].
- Learning effects and deteriorating jobs make a job's processing time depend on its position or start time: Biskup (1999), "Single-machine scheduling with learning considerations", EJOR 115:173–178; Biskup (2008) review, EJOR 188:315–329 [M].

**(d) Prices that respond to earlier grants.**
- Optimal execution of large orders: temporary and permanent market impact make the price paid for later trades depend on earlier ones (Almgren and Chriss 2001, "Optimal execution of portfolio transactions", Journal of Risk 3:5–39) [M].

**(e) Network and spillover effects, and information revealed by grants.**
- Golovin and Krause (2011), "Adaptive submodularity: theory and applications in active learning and stochastic optimization", JAIR 42:427–486, extend submodularity to adaptive policies, in which each selection reveals an outcome that conditions the next; a greedy adaptive policy is near-optimal under adaptive submodularity; applications include sensing and viral marketing [S] — [JAIR](https://jair.org/index.php/jair/article/view/10731); [arXiv 1003.3967](https://arxiv.org/pdf/1003.3967v5.pdf).
- Sequence (non-set) submodularity, in which the value of a set of items depends on the order in which they are presented: Streeter and Golovin (2008, NeurIPS), Tschiatschek, Singla and Krause (2017, AAAI, "Selecting sequences of items via submodular maximization"), Alaei, Makhdoumi and Malekian (Management Science, 2021, "Maximizing sequence-submodular functions") [M].

**(f) Sequential mechanisms in fair division and social choice: order as a design parameter.**
- Picking sequences: Bouveret and Lang (2011), "A general elicitation-free protocol for allocating indivisible goods", IJCAI 2011: a central authority that does not know agents' preferences fixes a sequence of agents who pick in turn, and chooses the sequence to optimise a criterion (expected social welfare under assumptions on preferences), also treating strategic behaviour [S] — [IJCAI proceedings](https://ijcai.org/proceedings/2011/24). Picking sequences generalise serial dictatorship by allowing non-consecutive picks [S] — [Kalinowski et al. arXiv 1304.5892](https://arxiv.org/abs/1304.5892v2).
- Kalinowski, Narodytska and Walsh (2013), IJCAI 2013, pp. 227–232: with additive utilities and independence between agents, each agent's expected utility under a picking sequence is computable in polynomial time, and "the expected utilitarian social welfare is maximized when agents take alternate turns"; the mechanism remains optimal under strategic behaviour [V] (abstract) — [arXiv 1304.5892](https://arxiv.org/abs/1304.5892).
- Serial dictatorship and random serial dictatorship for house allocation: Abdulkadiroğlu and Sönmez (1998), Econometrica 66(3):689–701 [M]. Balanced alternation as a fair-division protocol: Brams and Taylor (1996/2000) [M].
- Sequential reserve processing: Dur, Kominers, Pathak and Sönmez (2018), "Reserve design: unintended consequences and the demise of Boston's walk zones", Journal of Political Economy 126(6):2457–2479, DOI 10.1086/699974: the precedence order in which reserved and open seats are processed and the size of a reserve are substitutes; lowering a reserve's precedence (processing it later) or enlarging it never reduces the number of reserve-group students admitted; in Boston data the two adjustments had similar quantitative effects; the precedence order had undercut the intended walk-zone policy, and transparency about it led to the elimination of walk-zone reserves [S] — [IDEAS record](https://ideas.repec.org/a/ucp/jpolec/doi10.1086-699974.html); [MIT Blueprint Labs summary](https://blueprintlabs.mit.edu/research/reserve-design-unintended-consequences-and-the-demise-of-bostons-walk-zones).
- Kominers and Sönmez (2016), "Matching with slot-specific priorities: theory", Theoretical Economics 11:683–710, gives the general theory of slots with their own priorities and a precedence order over slots [M].
- Pandemic rationing: Pathak, Sönmez, Ünver and Yenmez, "Fair allocation of vaccines, ventilators and antiviral treatments: leaving no ethical value behind in health care rationing" (Management Science 70(6):3999–4036, 2024; arXiv 2008.00374), propose reserve systems (a distinct priority order per category) as an alternative to a single priority order [V] (abstract); the working paper notes that this flexibility requires attention to "the processing order of reserve categories" and analyses sequential reserve matching alongside cutoff equilibria and "smart reserves" [S] — [arXiv 2008.00374](https://arxiv.org/abs/2008.00374); [NBER w26951](https://www.nber.org/papers/w26951.pdf); [RePEc record](https://econpapers.repec.org/RePEc:inm:ormnsc:v:70:y:2024:i:6:p:3999-4036).

**(g) Order across time in dynamic optimisation: time-linkage.**
- Nguyen, Yang and Branke (2012), "Evolutionary dynamic optimization: a survey of the state of the art", Swarm and Evolutionary Computation 6:1–24, DOI 10.1016/j.swevo.2012.05.001, classify dynamic problems by "time-linkage: whether the future behaviour of the problem depends on the current and/or the previous solutions found by the algorithm", by predictability and by visibility of changes; for time-linkage problems "it was suggested that the only way to solve the problems effectively is to predict future changes and take into account the possible future outcomes when solving the problems online" [V] — [Optimization Online preprint](https://optimization-online.org/wp-content/uploads/2012/05/3480.pdf).

**(h) Formal models used in the literature.**
- Markov decision processes (states, actions, transition kernels, rewards; policies as maps from states or histories to actions) are the standard model of sequential allocation under uncertainty (Puterman 1994, *Markov Decision Processes*) [M].
- The *adaptivity gap* (Dean, Goemans and Vondrák 2008, "Approximating the stochastic knapsack problem: the benefit of adaptivity", Mathematics of Operations Research 33(4):945–964) is the ratio of the expected value of an optimal adaptive policy to that of an optimal non-adaptive one (a fixed order of insertion); for stochastic knapsack it is at most 4; computing either optimal policy is NP-hard since both reduce to deterministic knapsack [S] — [MIT PDF](https://math.mit.edu/~goemans/PAPERS/DeanGoemansVondrak-2008-ApproximatingTheStochasticKnapsackProblem.pdf). A 2026 arXiv paper (Barak and Talgam-Cohen) studies semi-adaptive policies and improves the known adaptivity-gap bounds [S] — [arXiv 2602.24042](https://arxiv.org/pdf/2602.24042).

#### Inferences
- **A monoid-action formulation unifies the sources above.** Let X be the state space (stocks, capacities, prices, eligibilities, histories), G the alphabet of grants (resource, recipient, quantity, time). Each grant acts as a partial map δ_g : X ⇀ X (undefined when the grant is infeasible). The free monoid G* acts on X by δ_{g₁…g_k} = δ_{g_k} ∘ … ∘ δ_{g₁}; an allocation is a word w, its outcome δ_w(x₀), and its value either a terminal function f(δ_w(x₀)) or a path sum Σ r(x_{i−1}, g_i). The sources differ only in what part of the state a grant changes: stock (a), adoption counts with increasing returns (b), consumed capacity and the last job set up (c), the price (d), the information set (e), the remaining items and eligible agents (f).
- **Order-independence is commutativity.** The value depends only on the multiset of grants iff δ_g δ_h = δ_h δ_g on all reachable states, i.e. iff the action factors through the free commutative monoid ℕ^G; then a set or count vector is the right representation and permutation search is pure redundancy. A one-line commutator test, comparing δ_h(δ_g(x)) with δ_g(δ_h(x)) on sampled reachable states, detects order-dependence empirically.
- **Partial commutation gives the right quotient.** Usually some pairs commute (grants of unrelated resources to unrelated recipients) and others do not (two grants drawing on one stock). Declaring an independence relation I ⊆ G × G turns G* into a trace monoid G*/≈_I (Mazurkiewicz traces [M]); words in one trace are guaranteed to give one outcome, and a canonical representative (Foata or lexicographic normal form [M]) can be computed by sorting commuting neighbours. This is the formal version of "many sequences decode to one allocation" (§4).
- **Mechanisms are words over agents.** A picking sequence is a word in {1,…,n}^m; serial dictatorship is a permutation of agents; reserve precedence is a permutation of categories (or of slots). Each is a small search space in which an EA could search the mechanism itself under a welfare or equity criterion, with each candidate evaluated by simulation on a preference distribution.
- **Under uncertainty** δ_g becomes a Markov kernel and the system an MDP; a fixed word is then a non-adaptive (open-loop) policy, and the adaptivity gap measures what it gives up (§3).

#### Gaps
- I did not read the full texts of Arthur (1989), David (1985), Dur et al. (2018) or Pathak et al. (2024); the precise processing rules of sequential reserve matching (and the theorem statements) should be taken from the papers.
- I found no source that uses monoid or trace-monoid language for allocation; the formulation above is my synthesis, not a literature finding.
- Sequence-submodularity references are from memory.

### 2. Permutation Operators, the Features They Preserve (Position, Order, Adjacency), and Their Fit to Allocation in Which Earlier Grants Change the Value of Later Ones

#### Takeaway
Permutation operators differ in what they pass on: absolute position (cycle crossover, PMX, position-based), relative order or precedence (order-based crossovers, PPX, NWOX, insertion and scramble mutations), or adjacency/edges (edge recombination, OX, reversal/2-opt). Performance depends on which feature the objective rewards: on the TSP (adjacency) edge recombination wins, while on Starkweather's warehouse allocation (inventory depletion, relative order) the same ranking is almost reversed and order operators win. For allocation in which earlier grants change the value of later ones through a consumed or updated state, the decisive feature is usually precedence (relative order); adjacency matters only with sequence-dependent transitions (setups), and absolute position only when positions themselves carry value (time slots, picking turns).

#### Cited Findings

**Classical operators and their origins** (all [V] from the Larrañaga et al. table and text) — [Larrañaga et al. 1999, Artificial Intelligence Review 13(2):129–170, DOI 10.1023/A:1006529012972](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf); [Springer](https://link.springer.com/article/10.1023%2FA%3A1006529012972):
- Path representation crossovers: PMX (Goldberg and Lingle 1985), cycle crossover CX (Oliver, Smith and Holland 1987), order crossover OX1 (Davis 1985), order-based OX2 and position-based POS (Syswerda 1991), heuristic crossover (Grefenstette 1987), edge recombination ER (Whitley et al. 1989), sorted match (Brady 1985), maximal preservative MPX (Mühlenbein et al. 1988), voting recombination (Mühlenbein 1989), alternating positions (Larrañaga et al. 1996).
- Mutations: displacement (Michalewicz 1992), exchange (Banzhaf 1990), insertion (Fogel 1988), simple inversion (Holland 1975), inversion (Fogel 1990), scramble (Syswerda 1991).
- Other representations: adjacency (alternating edge, subtour chunks, heuristic crossovers), ordinal (Grefenstette et al. 1985, which lets classical crossover be used), matrix (Fox and McMahon 1987).

**What each preserves (classical descriptions).**
- PMX: "the absolute positions of some elements of both parents are preserved" [V] — [Larrañaga et al.](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf). Starkweather et al.: PMX may emphasise absolute position more than the order operators, and its repair substitution is "a mutation where neither adjacency, position, or relative order is preserved" [V] — [Starkweather et al.](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf).
- CX: "The absolute position of on average half of the elements of both parents are preserved"; Oliver et al. (1987) found CX better than PMX on the TSP [V] — [Larrañaga et al.](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf). Starkweather et al.: "cycle crossover always preserves the position of elements from one parent or the other" [V].
- OX1: exploits "that the order of cities (not their positions) are important", copying a subtour of one parent and "preserving the relative order of cities of the other parent" [V] — [Larrañaga et al.](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf). Starkweather et al.: one part of the child "inherits the order, adjacency and absolute position of part of one parent string, and the other part ... the relative order of the remaining elements from the other parent" [V].
- POS (Syswerda's "position" crossover): Starkweather et al. argue it "is really another order operator", less effective at preserving position than CX or PMX [V].
- ER: "emphasizes adjacency information instead of the order or position of the items" and treats breaking edges as unwanted mutation [V] — [Starkweather et al.](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf); [Larrañaga et al.](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf).
- PPX (precedence preservative crossover): Bierwirth, Mattfeld and Kopfer (1996), "On permutation representations for scheduling problems", PPSN IV, LNCS 1141, DOI 10.1007/3-540-61723-X_995, define three crossovers for a job-shop permutation representation differing in whether they preserve relative order, absolute order or position; PPX was developed independently for vehicle routing by Blanton and Wainwright (1993) [S] — [Springer](https://link.springer.com/doi/10.1007/3-540-61723-X_995); [Cicirello 2023 survey](https://arxiv.org/html/2311.14595v1).

**Empirical comparisons.**
- Starkweather et al. (1991), GENITOR steady-state GA, no mutation: on a 30-city blind TSP, tuned runs found the optimum 30/30 with improved ER, 25/30 OX1, 18/30 OX2, 18/30 POS, 1/30 PMX and never with CX (best 440, mean 490.3); "PMX and cycle crossover in particular converged too quickly" [V]. On the 195-order warehouse allocation the results "are almost the opposite": both of Syswerda's operators (OX2, POS) "did extremely well"; ER needed 200,000 recombinations to match what PMX found in 20,000, and both were inferior to OX2 and POS; ER search was "an order of magnitude slower than operators which stress relative order as opposed to adjacency" [V]. Conclusion: "operators which work well in problems where adjacency is important (e.g., the Traveling Salesman) may not be effective for other types of sequencing problems" [V] — [Starkweather et al. 1991](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf).
- Larrañaga et al. (1999), generational GA on 47 Spanish provincial capitals, Grötschel's 24 and 48 cities, Kruskal–Wallis at α = 0.05: across instances "the crossover operators ER, OX1, POS and OX2, likewise the mutation operators DM, IVM, and ISM were those which had the best results"; fastest to converge ER, PMX, OX1, POS (crossover) and SIM, SM (mutation); "the ones that had the best results and at the same time were the quickest, are ER, OX1 and POS" [V] — [Larrañaga et al.](https://www.dcs.gla.ac.uk/~pat/jchoco/gatsp/papers/aiRev99.pdf). They also report that on a Bayesian-network ordering problem (non-TSP) the best operators differed, endorsing Starkweather's domain-dependence claim [V].
- Cicirello (2023), "A survey and analysis of evolutionary operators for permutations" (arXiv 2311.14595, preprint; implementations in the Chips-n-Salsa Java library), distinguishes five features (positions, undirected edges, directed edges, precedences, cyclic precedences) and measures each operator on a "Permutation in a Haystack" problem with one distance per feature (exact match for positions, cyclic edge distance, cyclic r-type distance, Kendall τ for precedences, Lee distance for cyclic precedences), n = 100, 100 targets, 100 runs, against a swap-mutation-only baseline [V] — [arXiv 2311.14595](https://arxiv.org/html/2311.14595v1). Findings [V]:
  - positions: CX, PMX, UPMX, PBX beat the baseline;
  - undirected edges: only EER, ER, OX;
  - directed edges: "only OX performs substantially better";
  - precedences: CX, NWOX, UOBX, OX2, PPX, UPPX, PMX, UPMX, PBX;
  - mutations: swap is general-purpose; insertion suits edges and precedences, poor for positions; reversal/2-opt suit undirected edges; scramble suits precedences (most pairwise precedences survive); cycle mutation suits positions.
- *Tension between sources:* the 1991/1999 sources describe OX1 as an order (relative-order) operator, whereas Cicirello finds OX effective for edges but not for precedences on his haystack problem. The likely reason (inference) is that OX1 copies a contiguous block (which carries edges) and fills the rest in the other parent's order with wrap-around from the second cut, which scrambles precedences between block and remainder; Syswerda's OX2/POS, which choose non-contiguous positions, keep precedences better. Both sources agree that OX2 and POS are precedence-friendly.

**Random keys.**
- Bean (1994), "Genetic algorithms and random keys for sequencing and optimization", ORSA Journal on Computing 6(2):154–160, DOI 10.1287/ijoc.6.2.154: a solution is a vector of keys in [0,1), decoded (by sorting) into a sequence, so that any crossover on real vectors yields a feasible permutation and the GA becomes problem-independent; applications include multiple-machine scheduling, resource allocation and the QAP; INFORMS Journal on Computing Test of Time award (1991–1995 papers) [S] — [scilit record](https://scilit.com/publications/7c009b222f05fb6686182729dbc20ead); [INFORMS note](https://pubsonline.informs.org/doi/10.1287/ijoc.2023.ed.v35.n4).
- Biased random-key GAs (BRKGA): Gonçalves and Resende (2011), "Biased random-key genetic algorithms for combinatorial optimization", Journal of Heuristics, DOI 10.1007/s10732-010-9143-1 (elite parent biased uniform crossover, mutants as immigrants, a problem-specific decoder) [S for DOI; content M] — [DOI](https://www.doi.org/10.1007/S10732-010-9143-1).

**Landscape theory of permutation problems.**
- Elementary landscapes satisfy Grover's wave equation (the average of f over neighbours is an affine function of f(x) with constant k); consequence: "all maxima are greater than f̄ and all minima are less than f̄" [V] — [Chicano, Luque and Alba 2011, arXiv 1109.4875](https://ar5iv.labs.arxiv.org/html/1109.4875).
- The QAP "is not an elementary landscape when the swap neighborhood is considered" but is the sum of at most three elementary components with constants 2n, 2(n−1) and n; the TSP, a special case of the QAP, can have fewer; the decomposition gives the neighbourhood average in closed form and exact autocorrelation, and the authors suggest selecting by neighbourhood average, useful on plateaus [V] — [Chicano et al. 2011](https://ar5iv.labs.arxiv.org/html/1109.4875).
- A general method for finding elementary decompositions (Chicano, Whitley and Alba, Evolutionary Computation, 2011) [S] — [preprint, RIUMA](https://riuma.uma.es/bitstreams/4ab9776b-6503-47a2-8e5a-f8547e97ac12/download); the decomposition also holds for the linear ordering problem (Ceberio and colleagues) [S] — [core.ac.uk](https://core.ac.uk/download/288796529.pdf). Grover (1992, OR Letters) showed that TSP, graph partitioning and other classical problems are elementary under their natural neighbourhoods; Stadler (1996) gives the general Laplacian-eigenfunction theory; Whitley, Sutton and Howe (2008) "Understanding elementary landscapes" [M; the last cited in Chicano et al. [V]].

**Algebraic operators on the symmetric group.**
- Santucci, Baioletti and Milani (2016), "Algebraic differential evolution algorithm for the permutation flowshop scheduling problem with total flowtime criterion", IEEE TEVC 20(5):682–694, DOI 10.1109/TEVC.2015.2507785: a differential mutation defined for any combinatorial space given as a finitely generated group, specialised to permutations through a randomised bubble sort (the difference of two permutations is decomposed into adjacent transpositions, which can then be scaled and added); with a permutation crossover, biased selection, heuristic initialisation and a memetic restart, state-of-the-art results and new best-known flowtimes [S] — [dblp](https://dblp.uni-trier.de/rec/journals/tec/SantucciBM16.html); [Unpaywall](https://unpaywall.org/10.1109%2FTEVC.2015.2507785). Extended to makespan in AI Communications (2016) [S] — [preprint](https://ricerca.unistrapg.it/bitstream/20.500.12071/10911/1/aicom2015_rcra2014_santucci_final_version.pdf). Later generalised as an algebraic framework for swarm and evolutionary algorithms on combinatorial spaces (Santucci, Baioletti and Milani 2020, Swarm and Evolutionary Computation) [M].
- Geometric crossover (Moraglio and Poli): a crossover is geometric under a metric if offspring lie in the metric segment between the parents; PMX, CX and others are geometric under swap- or transposition-type distances, which ties operators to Cayley-graph metrics on the symmetric group [M].

#### Inferences
- **Map the source of order-dependence to the feature it rewards, then to operators.**
  - Stock, capacity, budget or eligibility consumed by earlier grants (Coors-type allocation, serial dictatorship, reserves): *precedence* (who comes before whom) → OX2/order-based, POS, PPX/UPPX, NWOX; insertion and scramble mutations; Kendall-τ geometry.
  - Sequence-dependent setups or transition costs between consecutive grants: *directed edges* → OX, edge-assembly or ER-type operators; reversal is unsafe for directed costs.
  - Value attached to the slot or turn itself (time windows, discount by position, picking turn index, deadline slots): *absolute position* → CX, PMX, PBX, cycle mutation; Hamming geometry.
  - Learning and deterioration effects depend on position and on predecessors' total processing: mixed; operators balancing position and precedence (PMX, PBX) or a portfolio.
- **A framework should know which features its objective rewards**; it can estimate this cheaply by correlating fitness differences with the per-feature distances (exact match, Kendall τ, edge distance) on sampled pairs, then weight an operator portfolio accordingly, or let an adaptive operator selection learn it (as Cicirello's adaptive EA does for rates).
- Landscape decompositions are known for QAP-, TSP- and LOP-type objectives; most allocation objectives with consumed state are not of these forms, so their landscapes are likely not elementary, but the same decomposition machinery (Fourier analysis on the symmetric group) applies in principle.

#### Gaps
- No full text of Bierwirth et al. (1996), Bean (1994), the BRKGA paper or the algebraic DE paper was read.
- No systematic comparison of permutation operators on resource-allocation objectives (as opposed to TSP, flowshop and the haystack problem) was found; Starkweather's warehouse problem is the closest, and it is 1991 and a single instance.
- Whether Cicirello's survey has appeared in a peer-reviewed venue was not checked.

### 3. When an Open-Loop Sequence Suffices, and When a Policy or Rule Reacting to the State after Each Grant Must Be Evolved

#### Takeaway
A fixed sequence suffices when the transition is deterministic and fully known at decision time, so nothing is learnt by waiting; then the best open-loop sequence is as good as the best policy. When outcomes are stochastic and revealed grant by grant, when jobs or claims arrive over time, or when one rule must serve many instances, a state-reactive policy is needed: the loss from committing to a sequence is the adaptivity gap (bounded by 4 for stochastic knapsack). Evolutionary computation offers three routes: priority and dispatching rules evolved by genetic programming (the mature route for dynamic scheduling), neural policies evolved by evolution strategies or neuroevolution (competitive with RL and highly parallel), and, since 2024, programs proposed by language models inside an evolutionary loop (FunSearch's online bin-packing heuristics). Between them stand open-loop sequences re-optimised on a rolling horizon, and priority lists that a decoder interprets reactively.

#### Cited Findings

**Policies as evolved priority rules (GP hyper-heuristics).**
- Branke, Nguyen, Pickardt and Zhang (2016), "Automated design of production scheduling heuristics: a review", IEEE TEVC 20(1):110–124, DOI 10.1109/TEVC.2015.2429314: production scheduling is a particularly popular application of hyper-heuristics that design heuristics (mostly dispatching rules), shown to be "effective, efficient, easy to implement, and reusable in different shop conditions"; the paper proposes a taxonomy and design guidelines and lists open questions [S] — [Wellington open access record](https://openaccess.wgtn.ac.nz/articles/journal_contribution/Automated_Design_of_Production_Scheduling_Heuristics_A_Review/13151315); [dblp](https://dblp.org/rec/journals/tec/BrankeNPZ16.html).
- Nguyen, Mei and Zhang (2017), "Genetic programming for production scheduling: a survey with a unified framework", Complex & Intelligent Systems 3:41–66 [S] — [Springer](https://link.springer.com/article/10.1007/s40747-017-0036-x).
- Zhang, Mei, Nguyen and Zhang (2024), "Survey on genetic programming and machine learning techniques for heuristic design in job shop scheduling", IEEE TEVC 28(1):147–167, DOI 10.1109/TEVC.2023.3255246: GP is valued because its flexible representation learns dispatching rules; reviews GP designs for different job-shop variants and the machine-learning techniques (feature selection, multitask learning, surrogates) adapted to make it effective and efficient [S] — [GP bibliography](https://gpbib.cs.ucl.ac.uk/gp-html/Zhang_ieeeTEVC.html). In dynamic flexible job shops two rules are evolved jointly (routing: which machine; sequencing: which job next) [M]; a 2026 arXiv paper studies the generalisation of GP-evolved rules in dynamic flexible job shops [S] — [arXiv 2601.15717](https://arxiv.org/pdf/2601.15717).
- GP evolution of online bin-packing heuristics (scoring functions that place each arriving item): Burke, Hyde, Kendall and Woodward (GECCO 2007 and later) [M].

**Policies by evolution strategies and neuroevolution.**
- Salimans, Ho, Chen, Sidor and Sutskever (2017), "Evolution strategies as a scalable alternative to reinforcement learning" (arXiv 1703.03864): ES is a black-box alternative to MDP-based RL (Q-learning, policy gradients); with common random numbers workers exchange only scalars, scaling "to over a thousand parallel workers"; 3D humanoid walking solved in 10 minutes; competitive Atari results after one hour; ES is invariant to action frequency, tolerant of delayed rewards and long horizons, and needs no temporal discounting or value-function approximation [V] (abstract) — [arXiv 1703.03864](https://arxiv.org/abs/1703.03864).
- Deep neuroevolution with simple GAs (Such et al. 2017) and the review by Stanley, Clune, Lehman and Miikkulainen (2019, Nature Machine Intelligence, "Designing neural networks through neuroevolution") [M].

**Policies as programs proposed by language models within evolution (recent).**
- Romera-Paredes et al. (2024), "Mathematical discoveries from program search with large language models", Nature 625:468–475, DOI 10.1038/s41586-023-06924-6 (online 14 December 2023): FunSearch evolves programs by prompting a frozen LLM with high-scoring programs from a database and filtering proposals by an automatic evaluator; for online bin packing the evolved part is a priority function, and it found heuristics that beat first-fit and best-fit on well-studied item-size distributions [S] — [PMC full text](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145); [DeepMind blog](https://deepmind.google/discover/blog/funsearch-making-new-discoveries-in-mathematical-sciences-using-large-language-models). An independent 2025 analysis questions how far the interpretability claim holds for those heuristics [S] — [arXiv 2510.27353](https://arxiv.org/pdf/2510.27353). Related LLM-in-the-loop heuristic design: Evolution of Heuristics (Liu et al., ICML 2024) and ReEvo (Ye et al., NeurIPS 2024) [M].

**When adaptivity pays.**
- Dean, Goemans and Vondrák (2008): adaptivity gap of stochastic knapsack at most 4 (an optimal fixed order of insertion loses at most a constant factor) [S] — [MIT PDF](https://math.mit.edu/~goemans/PAPERS/DeanGoemansVondrak-2008-ApproximatingTheStochasticKnapsackProblem.pdf).
- Golovin and Krause (2011): under adaptive submodularity a greedy adaptive policy is near-optimal; no polynomial algorithm does better in general under standard complexity assumptions [S] — [JAIR](https://jair.org/index.php/jair/article/view/10731).

**Dynamic and stochastic allocation with EAs.**
- Nguyen, Yang and Branke (2012) classify EA approaches to dynamic problems as introducing diversity after a change, maintaining diversity throughout, memory, prediction, self-adaptation and multiple populations [V]; for time-linkage problems prediction of future outcomes is needed; an "anticipation" approach for dynamic job shops penalises early machine idle time to move the system "into a flexible state", which "significantly improved the performance" [V] — [preprint](https://optimization-online.org/wp-content/uploads/2012/05/3480.pdf). (The anticipation work is Branke and Mattfeld 2005, "Anticipation and flexibility in dynamic scheduling", IJPR 43(15) [M].)
- Robustness in uncertain environments: Jin and Branke (2005), "Evolutionary optimization in uncertain environments — a survey", IEEE TEVC 9(3):303–317 [M]; robust optimisation over time (solutions kept across several changes) has a 2023 critical review [S] — [White Rose eprint](https://eprints.whiterose.ac.uk/202577/7/Robust_Optimization_Over_Time_A_Critical_Review.pdf).
- Rolling-horizon evolutionary algorithms evolve an open-loop action sequence, execute its first action, and re-plan at each step (Perez, Samothrakis, Lucas and Rohlfshagen, GECCO 2013) [M].
- Priority-rule-based scheduling with sampling: Kolisch (1996) finds the ranking of priority rules the same for single-pass and sampling, and sampling much better than single pass [S] — [IDEAS](https://ideas.repec.org/a/eee/ejores/v90y1996i2p320-333.html).

#### Inferences
- **Decision rule for the framework.** Offer an open-loop sequence when (i) the grant actions are deterministic, (ii) the whole instance is known at decision time, and (iii) the solution is for this instance only. Offer a policy when any fails: (i′) grant outcomes are random and observed before the next grant (adaptivity gap > 1), (ii′) claims or resources arrive online, (iii′) one rule must generalise across instances or be audited as a rule (dispatching rules, priority scores, eligibility rules are what institutions publish). In deterministic MDPs an optimal open-loop plan equals the optimal closed-loop one (standard fact [M]), so offering policies there only adds cost.
- **A priority list decoded by an SGS-like scheme is already semi-reactive**: the decoder skips infeasible or ineligible grants in the current state, so the list behaves as a stationary policy "grant to the first eligible in list order". A priority *function* of state features (GP tree, neural network, LLM-written program) generalises this from one instance to many; the list is the special case of a constant score per item.
- **Sequences can be closed-loop by re-planning**: rolling-horizon re-optimisation (model predictive control) gives reactivity without a learnt policy, at a computational cost per decision; evolved policies move that cost offline.
- **Estimate the adaptivity gap empirically before choosing**: on sampled scenarios, compare the best common sequence (one word evaluated on all scenarios) with the scenario-wise best (an upper bound on any policy, the "perfect information" value); a small gap means a sequence suffices.

#### Gaps
- No head-to-head study of evolved sequences versus evolved policies on the same allocation problem was found in this pass (the GP job-shop literature compares rules with rules and occasionally with metaheuristics on static instances).
- Comparisons of ES with deep RL on combinatorial allocation (rather than locomotion and Atari) were not found.
- The Branke et al. (2016) and Zhang et al. (2024) surveys were seen only as abstracts.

### 4. Redundant Encodings (Many Sequences Giving One Allocation): How They Are Handled, and When Redundancy Helps or Hurts

#### Takeaway
Redundancy is intrinsic to decoder-based allocation: an SGS maps many activity lists to one schedule, sorting maps a continuum of random-key vectors to one permutation, and grants that commute make whole classes of words equivalent. Theory (Rothlauf; Rothlauf and Goldberg) says redundancy is neutral when uniform, helps only if it over-represents good solutions and hurts if it under-represents them, and that non-synonymous redundancy (equivalent genotypes far apart) disrupts operators. In scheduling, the decoder also chooses which phenotypes are reachable: the serial SGS reaches the active schedules (which contain an optimum for regular objectives), the parallel SGS only non-delay schedules (which may exclude every optimum). Practice handles redundancy by choosing the decoder deliberately, by normalising genotypes (standardised random keys, justification), and by caching evaluations by phenotype.

#### Cited Findings
- Rothlauf (2003), "Population sizing for the redundant trivial voting mapping", GECCO 2003, LNCS 2724:1307–1319: the population size needed to find the optimum is proportional to O(2^{k_r}/r), where k_r is the order of redundancy and r the number of genotypic building blocks representing the optimal phenotypic block; "uniformly redundant representations do not result in a better performance of GAs. Only if the good solutions are overrepresented ... does GA performance increase", and performance decreases if they are underrepresented; "the redundant TV mapping can only be used beneficially for GA search if knowledge about the structure of the optimal solution exists" [V] — [GECCO 2003 paper](https://cs.york.ac.uk/rts/docs/GECCO_2003/papers/2724/27241307.pdf).
- Rothlauf and Goldberg (2003), "Redundant representations in evolutionary computation" (Evolutionary Computation 11(4):381–415 [M for volume]): distinguish synonymously redundant representations (genotypes of one phenotype are similar) from non-synonymous ones, which "do not allow genetic operators to work properly", and uniform from non-uniform redundancy [S] — [GECCO 2003 bibliography page](https://cs.york.ac.uk/rts/docs/GECCO_2003/bibs/2724/27241307.htm); [econbiz record](https://www.econbiz.de/Record/redundant-representations-in-evolutionary-computation-rothlauf-franz/10005868351). Rothlauf (2003) cites Knowles and Watson (2002), "On the utility of redundant encodings in mutation-based evolutionary search", PPSN VII, pp. 88–98, as indicating "that redundant representations do not increase EA performance", and that on NK-landscapes, H-IFF and MAX-SAT they "appeared to reduce EA performance", against earlier hopes that neutrality helps [V for Rothlauf's account; Knowles and Watson not read] — [Rothlauf 2003](https://cs.york.ac.uk/rts/docs/GECCO_2003/papers/2724/27241307.pdf).
- Sprecher, Kolisch and Drexl (1995), "Semi-active, active, and non-delay schedules for the resource-constrained project scheduling problem", EJOR 80(1):94–102: the serial SGS produces active schedules, the parallel SGS non-delay schedules; the set of active schedules contains an optimal schedule for regular objectives, while the non-delay set may exclude all optima [S, via a citing paper; check the original] — [IDEAS record](https://ideas.repec.org/a/eee/ejores/v80y1995i1p94-102.html); [Springer chapter citing it](https://rd.springer.com/chapter/10.1007/978-3-319-05443-8_1). Kolisch (1996) proves the classes generated by each scheme [S] — [IDEAS](https://ideas.repec.org/a/eee/ejores/v90y1996i2p320-333.html).
- Hartmann (1998), "A competitive genetic algorithm for resource-constrained project scheduling", Naval Research Logistics 45(7):733–750: a permutation (activity-list) GA decoded by an SGS, compared with two GA concepts from the literature and better than several heuristics [S] — [IDEAS](https://ideas.repec.org/a/wly/navres/v45y1998i7p733-750.html). Hartmann's 1999 book compares an activity-list GA with a random-key GA [S] (the result itself not seen).
- Kolisch and Hartmann (2006), "Experimental investigation of heuristics for resource-constrained project scheduling: an update", EJOR 174(1):23–37, DOI 10.1016/j.ejor.2005.01.065: classification and standardised computational comparison of heuristics, with features of good heuristics discussed [S] — [IDEAS](https://ideas.repec.org/a/eee/ejores/v174y2006i1p23-37.html). From memory [M]: the best heuristics were metaheuristics using the activity-list representation with the serial SGS (sometimes choosing between serial and parallel adaptively, as Hartmann's 2002 self-adapting GA), and forward–backward improvement ("justification") was a feature of many of the best.
- Justification (Valls, Ballestín and Quintanilla 2005, "Justification and RCPSP: a technique that pays", EJOR 165:375–386) shifts activities right then left, mapping a schedule to a canonical better one [M]. Standardised random keys (Debels, De Reyck, Leus and Vanhoucke 2006, EJOR 169:638–653) rewrite a key vector into the canonical vector of the schedule it decodes to, removing redundancy before recombination [M].
- The UPV/EHU group studied "the impact of problems such as symmetric problems on EDAs" (symmetry being one source of redundancy in permutation problems) [V for the topic; content not read] — [Ceberio et al., EDA note](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf).

#### Inferences
- **Three kinds of redundancy in order-dependent allocation**, each with its own remedy:
  1. *Commutation redundancy*: words in one trace (grants that commute) give one outcome. Remedy: canonicalise each word to a trace normal form before evaluation and recombination; cache fitness by normal form. This is exact, cheap, and synonymous by construction (adjacent swaps of independent grants).
  2. *Decoder redundancy*: the decoder (serial/parallel SGS, greedy "first feasible") is many-to-one. Remedy: evaluate once per phenotype (hash the decoded allocation); optionally write the phenotype back as a canonical genotype (Lamarckian normalisation, as standardised random keys and justification do).
  3. *Continuous-key redundancy*: random keys map a cone of [0,1)^n to each permutation. Sorting cells all have volume 1/n!, so random keys are uniformly redundant under uniform sampling; by Rothlauf's result this should be neutral for supply, while giving drift along neutral directions. Keys near ties are where small moves change the permutation (synonymity holds locally).
- **Redundancy is not only harmful.** Neutral networks let populations drift between equally valued words, which can help escape; Rothlauf's theory suggests a framework should measure redundancy (sample genotypes, decode, count distinct phenotypes and their frequencies, compare with the frequency of good phenotypes) rather than assume it, and remove it where it is non-uniform and biased away from good regions.
- **Restricting the decoder is a double-edged choice.** A decoder onto a dominant set (active schedules) shrinks the space without losing optima; a decoder onto a smaller set (non-delay) can be faster to search but may exclude every optimum. A framework should state, for each decoder, which dominance property it preserves, and offer both (as Hartmann's self-adapting GA does [M]).

#### Gaps
- The full text of Rothlauf and Goldberg (2003), Sprecher et al. (1995) and Kolisch and Hartmann (2006) was not read; the dominance statements on active and non-delay schedules come through a citing source.
- No study was found that measures commutation (trace) redundancy explicitly in allocation EAs; the trace-normal-form remedy is my proposal.

### 5. Search Distributions over Permutations (Plackett–Luce, Mallows, Generalised Mallows) Compared with Genetic Operators

#### Takeaway
Estimation-of-distribution algorithms over permutations use four families: models over random keys (continuous EDAs on keys, decoded by sorting), marginal histogram models of positions or edges (NHBSA, EHBSA), exponential distance-based ranking models (Mallows and generalised Mallows, under Kendall τ, Cayley, Ulam or Hamming distance), and sequential-choice models (Plackett–Luce). The Mallows line from the UPV/EHU group gave competitive results on permutation flowshop and QAP, but performance "strongly relies on the distance used", and no distance wins on all problems. Each distance corresponds to a generating set of the symmetric group, a mutation operator and a preserved feature, so distributions and operators are two views of one geometry; Plackett–Luce is the natural model when early positions (first grants) matter most, and it is exactly the law of sorted random keys with Gumbel noise.

#### Cited Findings
- Ceberio, Irurozki, Mendiburu and Lozano (2012), "A review on estimation of distribution algorithms in permutation-based combinatorial optimization problems", Progress in Artificial Intelligence 1:103–117, DOI 10.1007/s13748-011-0005-3: reviews EDAs applied to permutation problems and offers directions for probabilistic modelling over permutation spaces [S] — [UPV/EHU ADDI record](https://addi.ehu.eus/handle/10810/4764); [Springer PDF link](https://link.springer.com/content/pdf/10.1007/s13748-011-0005-3.pdf). From memory [M]: the reviewed families included random-key EDAs (e.g., Bosman and Thierens's IDEA variants), edge- and node-histogram models (Tsutsui's EHBSA and NHBSA), and adaptations of discrete EDAs with repair; histogram models did best among them, random-key EDAs poorly.
- Ceberio, Mendiburu and Lozano (2011), "Introducing the Mallows model on estimation of distribution algorithms", ICONIP 2011, LNCS, pp. 461–470 [V, bibliographic] — [reference list, Ceberio et al. note](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf).
- Ceberio, Irurozki, Mendiburu and Lozano (2014), "A distance-based ranking model estimation of distribution algorithm for the flowshop scheduling problem", IEEE TEVC 18(2):286–300: a general EDA on the generalised Mallows model applied to the permutation flowshop, whose performance the authors attribute to its ability to discover promising regions [V bibliographic; S content] — [reference list](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf); [IP Paris record](https://researchportal.ip-paris.fr/en/publications/a-distance-based-ranking-model-estimation-of-distribution-algorit/).
- Ceberio, Irurozki, Mendiburu and Lozano (2015), "A review of distances for the Mallows and Generalized Mallows estimation of distribution algorithms", Computational Optimization and Applications 62(2):545–564: compares Kendall τ, Cayley and Ulam distances under MM and GMM EDAs; "the performance of these algorithms strongly relies on the distance used" [V bibliographic; S content] — [IDEAS](https://ideas.repec.org/a/spr/coopap/v62y2015i2p545-564.html). A study on 120 instances of four permutation problems found "there is not just one metric that performs the best in all the problems", with the Mallows–Ulam EDA the most stable [S; the snippet does not make clear whether this is the 2015 paper or a 2014 companion] — [IP Paris record](https://researchportal.ip-paris.fr/en/publications/a-review-of-distances-for-the-mallows-and-generalized-mallows-est/).
- Ceberio, Mendiburu and Lozano (2013), "The Plackett–Luce ranking model on permutation-based optimization problems", IEEE CEC 2013, pp. 494–501, DOI 10.1109/CEC.2013.6557609 [V bibliographic; results not seen] — [reference list](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf); [researchr](https://researchr.org/publication/CeberioML13/bibtex).
- Mixtures of generalised Mallows models for the QAP (Ceberio, Santana, Mendiburu and Lozano, CEC 2015); kernels of Mallows models under the Hamming distance for the QAP (Arza, Pérez, Irurozki and Ceberio, Swarm and Evolutionary Computation); Mallows EDAs for bi- and tri-objective flowshop; a MATLAB toolbox, perm_mateda (Irurozki et al., ACM TOMS 44(4):47, 2018); alternative codifications of permutation solutions (Malagón, Irurozki and Ceberio, CEC 2020); generative models for probability-based combinatorial optimisation (Malagón et al., ACM TELO, 2024) [V bibliographic] — [Ceberio et al. note](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf).
- Model-based gradient search on permutations: Ceberio and Santucci (2023), "Model-based gradient search for permutation problems", ACM TELO 3(4):1–35 [V bibliographic] — [reference list](https://www.lix.polytechnique.fr/~doerr/eda30/CeberioGLMMSV__EDAs_theoretical_approaches_methodological_enhancements_and_applications.pdf); a GECCO 2024 paper by Santucci and Ceberio uses a Plackett–Luce model in model-based gradient search for permutation problems [S] — [GECCO 2024 preprint](https://ricerca.unistrapg.it/retrieve/03d1862b-8744-4f6c-9352-001c65e9e81d/_GECCO_2024____HOP.pdf).
- Model definitions [M]: Mallows (1957): P(σ) ∝ exp(−θ d(σ, σ₀)) with a central permutation σ₀ and spread θ; generalised Mallows (Fligner and Verducci 1986) decomposes d into n−1 components each with its own θ_j (available for Kendall, Cayley and Ulam-type decompositions); Plackett–Luce (Luce 1959; Plackett 1975): items have weights w_i, and a permutation is drawn by choosing the first item with probability w_i/Σw, then the next among the remaining, and so on. The Mallows model under Kendall τ decreases exponentially in the number of pairwise disagreements with the reference ranking [S] — [IJCAI 2022 paper](https://www.ijcai.org/proceedings/2022/0743.pdf).
- Sorting keys log w_i + Gumbel noise yields Plackett–Luce (the Gumbel-max construction; Yellott 1977) [M].
- Cicirello (2023) uses Kendall τ as the distance measuring precedence and exact-match distance for positions [V] — [arXiv 2311.14595](https://arxiv.org/html/2311.14595v1); algebraic DE uses decomposition into adjacent transpositions (bubble sort), i.e. Kendall-τ geodesics [S] — [dblp/Unpaywall](https://unpaywall.org/10.1109%2FTEVC.2015.2507785).

#### Inferences
- **One geometry, three views.** For each Cayley metric on the symmetric group there is a generating set, a mutation, a feature it preserves, a Mallows distribution and an algebraic-DE move:

  | Distance | Generators (unit move) | Mutation | Feature it respects | Natural for |
  |---|---|---|---|---|
  | Kendall τ | adjacent transpositions | adjacent swap / bubble steps | precedences (relative order) | grants competing for consumed state; priority lists |
  | Cayley | all transpositions | swap | positions and cycles | assignment |
  | Ulam (n − LIS) | insertions | insertion | precedences, blocks | priority lists with a few misplaced items |
  | Hamming | — (not a Cayley graph metric of a unit move) | cycle / position resets | absolute position | slot-valued allocation (QAP-like) |

  A Mallows model under d is the maximum-entropy distribution at a given expected d-distance from σ₀ [M], i.e. a smoothed mutation kernel around a centre; Ceberio et al.'s finding that "performance strongly relies on the distance" is the distribution-side mirror of Starkweather's and Cicirello's operator-side finding that the preserved feature must match the objective.
- **Plackett–Luce is a sequential-allocation model.** Sampling PL is itself a picking process (choose the first grant, then the next among those left), so its parameters directly express "who should be served early"; it is top-weighted (the first positions are estimated most sharply), which suits allocation where early grants shape the state for later ones. It also equals random keys with Gumbel noise, so a PL-EDA is a random-key EDA with a principled noise model, and its weights are learnable by gradient (as in Ceberio and Santucci's model-based gradient search).
- **Distributions versus operators.** Operators need no model and handle mixed features through portfolios; distributions give an explicit, sampleable, parallel-friendly search state (fits ask–tell), natural restarts and diversity control through θ, and estimable features (the GMM's θ_j say which positions are settled). Neither dominates in the literature found; a framework should offer both behind one sampler interface.

#### Gaps
- The PL CEC 2013 results and the 2012 review's conclusions were not read; the claims about which families performed best are from memory.
- No direct, controlled comparison of Mallows/PL EDAs with tuned permutation GAs on allocation-type objectives was found in this pass.
- Hamming is not a word metric for transpositions; the table's Hamming row is an approximation of the correspondence and should be stated carefully in any design text.

### 6. What This Means for a Universal Framework

#### Takeaway
Represent an allocation as a word of grants acting on a state, declare (or detect) which grants commute, and let a decoder turn any genotype (a word, a key vector, a priority function) into a feasible allocation; choose operators and distributions by the feature the objective rewards (precedence, adjacency or position); remove commutation and decoder redundancy by canonical forms and phenotype caching; and offer state-reactive policies (priority functions evolved by GP, ES or LLM-proposed programs) when grant outcomes are random and observed, when claims arrive online, or when a published rule is wanted.

#### Cited Findings (Support for the Design Points)
- Operator effectiveness depends on the feature the objective rewards: adjacency for TSP, relative order for inventory-depleting allocation [V] — [Starkweather et al. 1991](https://mat.uab.cat/~alseda/MasterOpt/A_Comparison_of_Genetic_Sequencing_Operators.pdf); per-feature operator rankings [V] — [Cicirello 2023](https://arxiv.org/html/2311.14595v1); per-distance EDA performance [S] — [Ceberio et al. 2015](https://ideas.repec.org/a/spr/coopap/v62y2015i2p545-564.html).
- Decoders choose the phenotype set (active versus non-delay) [S] — [Sprecher et al. 1995](https://ideas.repec.org/a/eee/ejores/v80y1995i1p94-102.html); [Kolisch 1996](https://ideas.repec.org/a/eee/ejores/v90y1996i2p320-333.html).
- Uniform redundancy is neutral, biased redundancy helps or hurts [V] — [Rothlauf 2003](https://cs.york.ac.uk/rts/docs/GECCO_2003/papers/2724/27241307.pdf).
- The value of adaptivity is bounded and measurable (adaptivity gap) [S] — [Dean et al. 2008](https://math.mit.edu/~goemans/PAPERS/DeanGoemansVondrak-2008-ApproximatingTheStochasticKnapsackProblem.pdf); evolved priority rules are the established policy representation in dynamic scheduling [S] — [Branke et al. 2016](https://openaccess.wgtn.ac.nz/articles/journal_contribution/Automated_Design_of_Production_Scheduling_Heuristics_A_Review/13151315); ES scales policy search [V] — [Salimans et al. 2017](https://arxiv.org/abs/1703.03864); LLM-proposed priority functions for online allocation [S] — [Romera-Paredes et al. 2024](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145).
- Time-linkage requires anticipating future consequences [V] — [Nguyen, Yang and Branke 2012](https://optimization-online.org/wp-content/uploads/2012/05/3480.pdf).
- In mechanisms, the order (picking sequence, reserve precedence) is a design parameter with large welfare and equity consequences [V/S] — [Kalinowski et al. 2013](https://arxiv.org/abs/1304.5892); [Dur et al. 2018](https://ideas.repec.org/a/ucp/jpolec/doi10.1086-699974.html).

#### Inferences (Design Recommendations)
1. **Representation.** The core object is a *grant system*: a state type X, a grant alphabet G, a partial action δ : G × X ⇀ X (or a kernel), an initial state, and a value (terminal or path-additive). An allocation is a word in G*; a set-valued allocation is the special case in which all grants commute. This covers depletion, responsive prices, capacities, learning, spillovers, precedence and setups, eligibility, and sequential mechanisms in one interface.
2. **Order structure as data.** Let the user declare an independence relation I (grants that commute), or estimate it by commutator tests on sampled reachable states. From I the framework derives: whether order matters at all (I total → use set/count encodings, no permutation search); the trace quotient; and a canonical normal form for each word.
3. **Genotypes and decoders.** Provide (a) words/permutations with a *greedy feasible decoder* (generalised serial SGS: walk the list, grant each item when feasible in the current state, else defer or skip by a declared rule), with a parallel (time-stepped, non-delay) variant and a statement of which dominance property each preserves; (b) random keys (and BRKGA) on the same decoders; (c) direct words with repair for small spaces. Keep decoders pluggable, since they define the reachable phenotypes.
4. **Operator library indexed by feature.** Precedence: OX2/order-based, POS, PPX/UPPX, NWOX; insertion, scramble, adjacent swap. Adjacency (directed and undirected): OX, ER/EER, edge-assembly where a problem-specific version exists; reversal (undirected only), block moves. Position: CX, PMX/UPMX, PBX; cycle mutation, swap. Provide a feature probe (fitness-difference versus per-feature distance correlation on sampled pairs) and adaptive operator selection over the portfolio.
5. **Search distributions behind the same sampler interface.** Plackett–Luce (top-weighted, the natural law for "who first"; Gumbel keys), Mallows and generalised Mallows under Kendall, Cayley, Ulam and Hamming, histogram models (NHBSA/EHBSA) and random-key Gaussian models; algebraic-DE moves on the symmetric group with adjacent-transposition or transposition generators. Expose θ (spread) for diversity control and restarts.
6. **Redundancy handling.** Canonicalise words to trace normal form before evaluation, recombination and archiving; hash decoded allocations to cache evaluations and to measure redundancy (distinct phenotypes per sample; frequency of top phenotypes); offer Lamarckian write-back of a canonical genotype (standardised random keys, justification-like normalisation) as an option; warn when a decoder restricts to a set that may exclude optima.
7. **Sequences or policies.** Default to sequences when the system is deterministic and fully known. Offer policies when outcomes are random and observed, claims arrive online, or a published rule is wanted. Policy genotypes: GP priority/scoring functions over declared state features (routing and sequencing as separate co-evolved rules where there are two decisions), neural policies optimised by ES, and programs proposed by a language model and filtered by the evaluator (a proposal channel, never a judge). Provide an adaptivity-gap estimator (best common sequence versus scenario-wise best) to tell the user which is warranted, and rolling-horizon re-optimisation as the middle option.
8. **Dynamics and robustness.** Evaluate sequences over scenario sets (mean, quantile, worst case), support change detection, memory and diversity reintroduction, and for time-linked problems include anticipation terms (penalise states that leave the system inflexible) or evaluate over a horizon.
9. **Mechanisms as search spaces.** Treat picking sequences (words over agents), serial-dictatorship orders and reserve precedence orders (permutations of categories or slots) as first-class genotypes whose evaluation simulates the mechanism on a preference or claim distribution, so that the framework can search for the order of a mechanism, not only for an allocation.

#### Gaps
- The design above rests on the operator, redundancy and policy literatures; I found no existing framework that combines declared commutation, trace canonicalisation, feature-indexed operators and a sequence-versus-policy test, so its value is untested.
- The quantitative claims about RCPSP best practice (activity list plus serial SGS plus forward–backward improvement) are from memory and should be checked against Kolisch and Hartmann (2006) before being relied on.

## Part III. Representation Spaces and Frameworks for a Universal Evolutionary Search

*Remark.* Part III's scope and conventions. Scope: theories that unify variation operators across spaces (Moraglio, Radcliffe, Surry, Rothlauf); evolution on manifolds and constrained spaces (Riemannian CMA-ES, tangent-space sampling, the simplex, sets, multisets, graphs); mixed and conditional spaces (CMA-ES with margin, CatCMA, MIES, mixed Bayesian optimisation); type- and grammar-driven derivation (strongly typed GP, polytypic GP, Boltzmann samplers, QuickCheck); and how existing frameworks represent heterogeneous search spaces (Nevergrad, pymoo, DEAP, ParadisEO, jMetal, HeuristicLab, ECJ, EvoTorch, pyribs, Optuna, ConfigSpace, Ax).

Marks: **[V]** = verified by reading the source text itself during this research (PDF text extracted, or the page fetched); **[S]** = from a search-result snippet or a secondary summary only; **[M]** = from memory, not checked in this session. Search date: 2026-11-20.

### 1. A Principled, Space-Independent Definition of Mutation and Crossover by a Metric's Balls and Segments, and the Conditions a Space Must Satisfy for It to Work Well

#### Takeaway
Such a definition exists. Moraglio's geometric operators define mutation by the **balls** and crossover by the **segments** (metric intervals) of any metric on the search space. Radcliffe's forma analysis defines recombination from a chosen set of **equivalence relations**, through the properties respect, transmission and assortment. Both are representation-independent. Geometric crossover is a real restriction (some classical operators are provably non-geometric). Geometric mutation is not a restriction, since every mutation is geometric for some radius. The operators pay off only when the landscape is "smooth" or "concave" under the metric that induced them, and the product theorem lets geometric crossovers on parts compose into a geometric crossover on a product space. Neither theory guarantees performance: no-free-lunch applies.

#### Cited Findings

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

**Rothlauf: locality and redundancy (see also §5)**
- Rothlauf, *Representations for Genetic and Evolutionary Algorithms* (Springer, 2nd ed. 2006). It treats redundancy, scaling and locality as the core properties of a representation. [S] — [Springer](https://www.springer.com/us/book/9783540250593)
- Rothlauf's GECCO 2006 tutorial gives these definitions. [V] — [tutorial PDF](https://gpbib.cs.ucl.ac.uk/gecco2006etc/papers/tut103.pdf)
  - Locality is high when "neighboring genotypes correspond to neighboring phenotypes". Locality, causality and distance distortion "describe how well the metric on Φp fits to the metric on Φg".
  - Representations that change distances "modify the difficulty of the problem".
  - Phenotype–fitness mappings fall into three classes: positively correlated with distance to the optimum (easy for mutation-based search), uncorrelated, and negatively correlated (deceptive).

#### Inferences
- Geometric crossover gives a constructive recipe for any space with a metric: implement "sample uniformly (or with a chosen bias) from the segment [x; y]_d". Geometric mutation gives "sample from the ball B(x, ε)".
- The recipe is efficient only where segments can be sampled cheaply, through a shortest-path structure (edit sequences, alignments, geodesics). For many edit distances on graphs, sampling segments requires solving a matching or alignment problem first.
- Two conditions emerge for a space to "work well" with geometric operators:
  1. The metric is one under which fitness is smooth or concave (high fitness–distance correlation, Rothlauf's class 1).
  2. Segments and balls are computable and samplable.
- Forma analysis adds a third condition: the chosen formae should be **separable**, or recombination cannot both respect and assort them. Adjacency formae on permutations show this fails for natural structures.
- The product theorem is the formal licence for **component-wise derivation** of crossover on product types: the result is geometric under the sum metric. It also exposes the implicit metric. Summing unscaled component distances is exactly what produces scale mismatch (§5).
- Moraglio's discrete-metric result explains why categorical genes cannot be "interpolated": their segments contain only the parents. Exploration for categories must come from mutation or from a distributional model (softmax, categorical distributions).

#### Gaps
- I did not read Surry & Radcliffe's PPSN 1996 paper itself, only its description on Radcliffe's page. Its precise claims about reusing a formal algorithm across domains are unverified beyond that description.
- I did not obtain Rowe, Vose and Wright's group-theoretic representation papers. They are known here only through their mention in Moraglio & Sudholt.
- No source gives a general sufficient condition on (space, metric, fitness) for geometric crossover to beat mutation. Moraglio & Sudholt cover concave landscapes on three instantiated spaces only.

### 2. How Riemannian and Lie-Group Evolution Strategies Move on a Manifold (Exponential Map, Retraction, Parallel Transport of the Covariance), the Manifolds Supported in Practice, and the Simplex, Sets, Multisets and Graphs

#### Takeaway
The standard scheme is **Riemannian adaptation**:
1. Keep the search distribution as coordinates in the tangent space T_mM at the current mean.
2. Sample there.
3. Map samples to M by the exponential map or a retraction.
4. Move the mean, then carry the covariance and evolution paths to the new tangent space by parallel or vector transport.

Colutto et al. (2010) did this on spheres. Manopt.jl provides a generic Riemannian CMA-ES for any manifold that supplies a retraction, a vector transport, a basis and coordinate maps. The main limitation is locality: the search is confined to a normal neighbourhood whose size is bounded by the injectivity radius, which shrinks with curvature. Lie groups such as SO(d) are also handled pragmatically, by running a Euclidean CMA-ES on the flat Lie algebra so(d) and exponentiating. For the simplex, there is no established evolutionary literature that uses Aitchison geometry. Dirichlet-centred mutation and renormalisation repair appear in applied papers.

#### Cited Findings

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

#### Inferences
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

#### Gaps
- I could not read the Colutto et al. paper's text, so I do not know exactly how it transports the covariance on the sphere (parallel transport versus projection), or its step-size rule.
- I found no primary paper on natural evolution strategies whose search space (not distribution space) is a Riemannian manifold or Lie group, beyond the Fong & Tino framework and the pragmatic so(d) approach.
- No source verified for evolution strategies on Stiefel or Grassmann manifolds specifically. Manopt's generic CMA-ES should apply wherever its manifold library supplies the required maps, but I did not confirm which manifolds were tested.
- No primary evolutionary paper using Aitchison geometry was found. The simplex recommendations above are design inferences.

### 3. How Mature Frameworks Compose a Heterogeneous Genotype (a Product of Typed Parts) and Attach Variation Operators to Each Part, with Mixed-Integer Evolution Strategies and Mixed-Space Bayesian Optimisation

#### Takeaway
Three architectural patterns recur.
1. **Typed parameter trees with a standardised embedding** (Nevergrad). Each typed parameter maps bijectively to a block of R^n with a N(0, I) prior and also carries its own mutate/recombine. Containers (Tuple, Dict, Instrumentation) concatenate blocks, so any continuous optimiser can run on the embedding and structure-aware algorithms can use the per-parameter operators.
2. **Variables grouped by type, with a default operator per type** (pymoo MixedVariableGA). Individuals are dicts. Crossover and mutation are dispatched per variable class: SBX/PM for reals, the same with rounding for integers, uniform crossover and random reassignment for categories.
3. **A generic encoding type and composition of operator objects** (ParadisEO's EOT template; DEAP's toolbox registration; jMetal's CompositeSolution; HeuristicLab's MultiEncoding).

Hyperparameter-optimisation libraries add **conditional activation**: ConfigSpace conditions and forbidden clauses, Optuna's define-by-run, Ax's hierarchical `dependents`. Model-based methods make discreteness first-class by lower-bounding marginal probabilities (CMA-ES with margin) or by joint Gaussian–categorical distributions (CatCMA).

#### Cited Findings

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
- Docs: `Choice` samples "as the softmax of the values on these dimensions". The docs warn: "this process is non-deterministic and makes the function evaluation noisy". `TransitionChoice` uses one index dimension so that transitions are local, and parametrisation "is still a work in progress". [V] — [Nevergrad docs](https://facebookresearch.github.io/nevergrad/parametrization.html)
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

#### Inferences
- Every mature framework separates three things:
  1. the **value** of a genotype part;
  2. its **variation operators**;
  3. an optional **embedding** into R^n for numeric optimisers.
  Nevergrad makes all three properties of the typed parameter, which is the closest existing design to a "typed representation space". pymoo dispatches operators by variable class instead.
- Containers are always **products**: tuple, dict, composite solution, multi-encoding. **Sums** (alternatives with different sub-structure) appear only in the hyperparameter-optimisation languages, as conditional activation. Evolutionary frameworks have no first-class sum type with branch-aware recombination.
- CMA-ES with margin and CatCMA show that, for model-based search, a heterogeneous product is best handled by a **joint distribution** with type-specific marginals and a floor on their entropy, rather than by type-specific operators bolted onto a Gaussian.
- MIES shows the operator-based alternative: per-type mutation with per-gene self-adapted step sizes or probabilities.

#### Gaps
- I did not read the jMetal composite crossover/mutation implementation, the HeuristicLab operator wiring for MultiEncoding, ECJ's VectorSpecies per-gene settings, or the EvoTorch and pyribs papers in full. Those claims are [S].
- I did not verify how SMAC3 encodes inactive hyperparameters in its random-forest surrogate (I believe NaN or default imputation [M]), nor how Optuna's TPE handles conditional parameters (independent versus multivariate/group sampling [M]).
- I did not read Nevergrad's API for `set_mutation` with custom callables or its bounds methods (bouncing, clipping, arctan, tanh); the docs page fetched did not cover them.

### 4. Operators and Random Generators Derived Automatically from a Type or Grammar (Algebraic Data Types, Combinatorial Species), and Their Guarantees (Uniformity, Size Control)

#### Takeaway
Generators can be derived with strong guarantees. A Boltzmann sampler is built mechanically from a specification by union, product, sequence, set and cycle. It is **exactly uniform among objects of the same size** and runs in time linear in the output, given generating-function values at the control parameter x. Expected size is tuned by x, and exact or windowed size is obtained by rejection. How well the size concentrates depends on the class: "bumpy", "flat" or "peaked" profiles, with trees peaked at small sizes.

QuickCheck's type-class generators (`Arbitrary`, `sized`, `oneof`/`frequency`) are the practical model for type-derived generation with a size bound, but carry **no uniformity guarantee**.

Variation operators are less developed. Strongly typed GP enforces type-correct crossover and mutation. Polytypic GP derives initialisation and subtree mutation for arbitrary ADTs at compile time, but not crossover.

#### Cited Findings

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

#### Inferences
- For a typed genotype language with constructors {atom, product, sum, sequence, set/multiset, cycle}, a **Boltzmann sampler is derivable mechanically**: a Bernoulli choice for sums, independent calls for products, a geometric length for sequences, and Poisson-type draws for sets. It is uniform conditioned on size, with mean size set by x. This is the principled "random initialisation" and "random subtree" generator.
- Recursive types (trees) have peaked size distributions, so they need rejection or a singular-sampler tuning to reach a target size. This is a known pitfall of naive grow/full initialisation as well.
- QuickCheck shows the engineering pattern: a type class (or trait or interface) per type giving `arbitrary` under a size bound. The same dictionary can carry `shrink` (simplification toward smaller values), `mutate` and `recombine`.
- Shrinking is a natural **simplification/repair** operator for evolutionary search, e.g. the simplification GSGP requires and bloat control. This is my inference; no source evaluates QuickCheck shrinkers as EA operators.
- For variation operators derived from an ADT, the induction Polytope uses (atomic, product, coproduct cases) yields subtree mutation directly.
  - **Crossover** can be derived as "choose a position; if both parents have the same constructor there, recurse component-wise (product crossover); otherwise take one parent's subtree". This is homologous crossover and is geometric under a structural edit distance (Moraglio ch. 7–8 discuss homologous crossover as geometric).
  - Koza's subtree swap is not geometric. A derived crossover should therefore be homologous if geometric guarantees are wanted.

#### Gaps
- I did not read Flajolet–Fusy–Pivoteau 2007 or the *Analytic Combinatorics* book this session, so the unlabelled multiset and powerset sampler details are [S]/[M].
- I found no paper that derives *both* mutation and crossover from an ADT declaration with a proven property (geometricity, respect or uniformity). This appears to be an open design space.
- I did not verify Hypothesis's (Python) internal shrinking model (shrinking the choice sequence rather than the value), which is relevant to designing generic shrinkers [M].

### 5. Known Pitfalls in Composing Heterogeneous Genotypes (Low Locality, Redundancy, Invalid Offspring, Scale Mismatch between Parts)

#### Takeaway
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

#### Cited Findings
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

#### Inferences
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

#### Gaps
- I found no systematic empirical study comparing scale-normalisation schemes for heterogeneous product genotypes in EAs.
- I did not verify Rothlauf's quantitative population-sizing model for redundant representations (Rothlauf & Goldberg 2003) beyond the tutorial's description.

### 6. What This Means for a Universal Framework

#### Takeaway
A universal evolutionary framework should treat each representation as a **typed space object** that supplies:
- a standard set of capabilities (sample, mutate, recombine, distance, repair or project, local chart with retract and transport, standardised embedding, size, shrink);
- checkable **laws** (purity, segment and ball membership, retraction identities, idempotent repair).

Spaces compose by a small algebra of constructors (product, sum or choice with conditional activation, sequence, set and multiset, permutation, simplex, manifold, graph, constrained subspace). From this algebra, generators (Boltzmann samplers), product operators (geometric under the sum metric) and standardisation should be **derived generically**, while specialised spaces may override any capability.

#### Cited Findings
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

#### Inferences: A Concrete Architecture

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

#### Gaps
- The sum-type metric and branch-switch rule above are my design proposals. No source evaluates them, though they follow Moraglio's discrete-metric reasoning and the margin idea.
- How to adapt a joint covariance across parts that have no common embedding (e.g. a manifold part plus a graph part) is open. CatCMA covers Gaussian plus categorical only; Riemannian CMA-ES covers one manifold.
- No source gives a generic, provably geometric crossover for dependent products or for recursive types beyond homologous crossover. Deriving one is an open problem.

## Part IV. The Algebra and Category Theory of Resources, Allocations and Processes

*Remark.* Part IV's scope and conventions. Markers: **[V]** verified by reading the source itself during this research (for arXiv items marked [V abstract], only the abstract page was read; for items marked [V text], the full text was read and the definition or theorem number is given); **[S]** from a search snippet or a secondary summary only; **[M]** from memory, not checked during this research. Researched 2026-11-20. Full texts read: Coecke–Fritz–Spekkens (2016), Fritz (2017), Jung et al., "Iris from the ground up" (2018), Moraglio's thesis (2007) and Radcliffe (1994). The last two came from copies another researcher had saved in the shared scratchpad.

### 1. The Most Suitable Formal Notion of a "Resource Type" for Allocation

#### Takeaway
The best-supported notion has two levels. A resource *theory* is a symmetric monoidal category (SMC), whose objects are resources and whose morphisms are the free conversions. Its decategorification, which records only whether a conversion exists, is an **ordered (preordered) commutative monoid** (OCM). Divisibility is extra structure on top of this: a semimodule action of ℚ≥0 or ℝ≥0, or a closure of the monoid under fractions. Copyable ("quality-like") and consumable ("quantity-like") resources differ only in their monoid law: idempotent `max`/`∨` against `+`. Distinct, labelled items are best described by a combinatorial species, or a partial commutative monoid, on top of the OCM. In practice a resource type should be a **product of typed components**: each kind gets an OCM over a quantity semiring, with declared properties (divisible, cancellative, disposable, splittable, idempotent) and an optional validity predicate that makes combination partial.

#### Cited Findings

**1.1 Resource theories as symmetric monoidal categories (Coecke, Fritz and Spekkens 2016)**
- Definition 2.1 [V text] says that "a resource theory is a symmetric monoidal category (D,◦,⊗,I)". In it, objects represent resources, and morphisms in D(A,B) are transformations of A into B "that can be implemented without any cost". The operation ◦ is sequential composition, ⊗ is parallel composition, and I is "the void resource". The authors stress that the difference between a resource theory and an SMC "is not a mathematical one, but rather one of interpretational nature". Published in *Information and Computation* 250 (2016) 59–86, DOI 10.1016/j.ic.2016.02.008. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Definition 2.2 [V text]: the **free resources** are {A : D(I,A) ≠ ∅}, the objects obtainable from nothing. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Definition 4.1 and Theorem 4.2 [V text]: a "theory of resource convertibility" (R,+,≽,0) is a set with a binary operation, a preorder and a unit. The operation is associative, commutative and unital up to the equivalence ≃ induced by the preorder, and a≽b, c≽d ⇒ a+c≽b+d. Every SMC yields one, with a≽b :⇔ D(a,b)≠∅ ("partial decategorification"). The paper also notes that this is "a symmetric monoidal category enriched over the poset of truth values {0,1}". — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Examples 4.3 and 4.4 [V text] show that the **combination law, not the order, carries the meaning**. "Food" is ℕ² with componentwise addition; "proficiency" is ℕ² with componentwise max. Both have the same convertibility order (a≥a′ and b≥b′), yet catalysis is impossible for food and possible for proficiency. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- The paper defines a set of qualitative properties [V text]. These are exactly the declarable properties an allocation framework needs:
  - Definition 4.10, *catalysis-free*: a+c≽b+c ⇒ a≽b.
  - Definition 4.11, *non-interacting*: a≽b₁+b₂ ⇒ ∃a₁,a₂ with a≃a₁+a₂, a₁≽b₁, a₂≽b₂. The paper notes that "in the theory of ordered abelian groups, this condition is also known as the Riesz decomposition property".
  - Definition 4.12, *quantity-like*: a₁+a₂≃b₁+b₂ and a₁≽b₁ ⇒ b₂≽a₂.
  - Definition 4.16, *quality-like*: a+a≃a.
  - Definitions 4.18 and 4.20, *freely disposable* (a≽0) and *waste-free* (every a is freely disposable).
  - Definition 4.22, the *Riesz interpolation property*.

  — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Theorem 4.13 [V text]: non-interacting and quantity-like together imply catalysis-free. Proposition 4.14 ("no-cloning") [V text]: in a quantity-like theory, a≽a+a ⇔ 0≽a. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Remark 4.19 [V text]: disposal can itself be costly ("nuclear waste"), so "the general theory of resource theories should not assume that all resources are freely disposable". — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Remark 4.7 [V text]: lattice-ordered groups, commutative quantales and the Cuntz semigroup all satisfy Definition 4.1. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)

**1.2 Ordered commutative monoids and their regularisations (Fritz 2017)**
- Definition 3.1 [V text]: an ordered commutative monoid is an ordered set with an associative, commutative + and a neutral 0 such that "x ≥ y ⇒ x + z ≥ y + z". Published in *MSCS* 27(6) (2017) 850–938, DOI 10.1017/S0960129515000444. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Fritz deliberately does **not** assume x ≥ 0 [V text]. He gives four reasons:
  - not everything can be discarded (nuclear waste);
  - if x ≥ 0 for all x, then A cannot have any non-trivial conservation law;
  - most results do not need the assumption;
  - dropping it keeps the self-duality: reversing the order gives another OCM.

  — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Remark 3.2 [V text] identifies the definition with others in the literature: the commutative pomonoid; the partially-ordered semimodule over the partially-ordered semiring ℕ; Lieb–Yngvason's axioms A1–A3; and unital commutative quantales, which are OCMs "in which the ordering is a complete lattice, and the addition preserves joins". A footnote states: "a Petri net can be understood as presentation of an ordered commutative monoid in terms of generators and relations". — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Fritz's Figure 2 [V text] is a chain of full reflective subcategories: OCM ⊃ ordered abelian groups ⊃ ordered ℚ-vector spaces ⊃ Archimedean ordered ℚ-vector spaces. Each inclusion has a left adjoint, a "regularization": catalytic (formal negatives), many-copy (formal fractions) and seed. The first two "complete the collection of resource objects by throwing in new 'imaginary' objects like formal negatives or formal fractions". Completing further to an Archimedean ℝ-vector space adds structure, because "the scalar multiplication by arbitrary reals really does add additional structure". — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Theorem 6.15 [V text, as summarised in Fritz's introduction]: in an Archimedean ordered ℚ-vector space, x ≥ y holds if and only if f(x) ≥ f(y) for all functionals f. Theorem 6.18 lifts this to OCMs with a generating pair. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Theorem 8.24 [V text]: the maximal regularised rate satisfies R^reg_max(x→y) = inf_f f(x)/f(y), over functionals with f(y)≠0, assuming x,y ≥ 0. This gives a new formula for the Shannon capacity of a graph. Section 9 relates embedding an OCM into ℝ, "an essentially unique way of assigning a value or price", to von Neumann–Morgenstern utility and to Lieb–Yngvason. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Fritz flags his limits [V text]. Remark 2.6 ("epsilonification", meaning approximate conversion) is not captured, and "there are resource theories with high relevance to probability theory and information theory which cannot be described in our framework". — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Fritz's "Abstract Vergleichsstellensätze for preordered semifields and semirings I" [S] proves that a semifield preorder is the intersection of its total extensions. It then derives a Vergleichsstellensatz for non-Archimedean preordered semirings, in which homomorphisms to the tropical reals matter, and recovers Strassen's Vergleichsstellensatz. *SIAM J. Appl. Algebra Geom.* 7(2) (2023) 505–547. — [arXiv:2003.13835](https://arxiv.org/abs/2003.13835); Part II: [arXiv:2112.05949](https://arxiv.org/abs/2112.05949)

**1.3 Monotones and their construction**
- Monotones are defined in CFS [V text]:
  - Definition 5.1: a monotone is M: R→ℝ with a≽b ⇒ M(a)≥M(b).
  - Proposition 5.2: a complete family of monotones always exists (indicator functions of up-sets).
  - Definition 5.3: an *additive* monotone is a homomorphism to (ℝ,+,≥), and a *supremal* monotone is a homomorphism to (ℝ,max,≥).
  - Example 5.4: on "food" an additive monotone is a non-negative price per kind.

  — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Definition 5.5 and Theorem 5.6 [V text]: the conversion rate is R(a→b) := sup{m/n : n·a ≽ m·b}, and every extensive monotone gives R(a→b)·M(b) ≤ M(a). A monotone's main use is detecting non-convertibility: if M(a)<M(b), then a⋡b, but "the converse is in general not true". — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Gonda and Spekkens [V abstract] give a general scheme for constructing monotones. The preorder of resources is mapped to a better-understood preorder, and known monotones are pulled back along the map. Contraction-based, weight and robustness measures all arise this way, "in an abstract framework where composition does not depend on the type of resource". *Compositionality* 5 (2023), DOI 10.32408/compositionality-5-7. — [arXiv:1912.07085](https://arxiv.org/abs/1912.07085)
- Gonda's thesis, *Resource Theories as Quantale Modules* (Waterloo 2021) [S], gives a framework based on quantale modules. Its constructions extend measures from a subset of resources and translate them between resource theories. — [arXiv:2112.02349](https://arxiv.org/abs/2112.02349)

**1.4 Catalysis**
- In CFS, Definition 4.8 [V text] says c is a catalyst for a,b if a⋡b but a+c≽b+c. "Only one copy of the catalyst is needed in order to transform any number of a's into b's." — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Lipka-Bartosik, Wilming and Ng, "Catalysis in quantum information theory", *Rev. Mod. Phys.* 96, 025005 (2024) [V abstract], review transformations made possible by an auxiliary degree of freedom that returns to its initial state. — [arXiv:2306.00798](https://arxiv.org/abs/2306.00798)

**1.5 Algebraic structures for amounts**
- *Refinement monoids* [S]: a+b = c+d implies a 2×2 matrix of elements whose row and column sums match. This implies Riesz decomposition, but not conversely in general. In cancellative, conical monoids (positive cones of partially ordered abelian groups), refinement, decomposition and interpolation are equivalent. — ["Tame and wild refinement monoids", arXiv:1405.7582](https://arxiv.org/abs/1405.7582) (authors not checked); [arXiv:math/0503155](https://arxiv.org/abs/math/0503155)
- *Semirings as resource annotations* [S]. Ghica and Smith ("Bounded linear types in a resource semiring", ESOP 2014) and Brunel, Gaboardi, Mazza and Zdancewic ("A core quantitative coeffect calculus", ESOP 2014) independently generalised Bounded Linear Logic's natural-number indices to elements of an arbitrary semiring. This line continues in graded modal types (Granule, ICFP 2019). — [Orchard, Liepelt, Eades, ICFP 2019](https://www.cs.kent.ac.uk/people/staff/dao7/publ/granule-icfp19.pdf); [nLab: graded linear logic](https://ncatlab.org/nlab/show/graded+linear+logic)
- CFS's closing section [V text] mentions a variant that replaces the preorder by "a cost function" with analogous axioms. That variant amounts to enrichment over costs instead of truth values. — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- *Mixed divisible and indivisible goods* [V abstract]. Bei, Li, Liu, Liu and Lu define envy-freeness for mixed goods (EFM), which generalises both EF and EF1, and prove an EFM allocation always exists. They give efficient algorithms for two agents, and for n agents with piecewise-linear valuations over the divisible goods; ε-EFM runs in time polynomial in the agents, the indivisible goods and 1/ε. *Artificial Intelligence* 293:103436 (2021). — [arXiv:1911.07048](https://arxiv.org/abs/1911.07048)
- *Goods against chores (a "mixed manna")* [S]. In Bogomolnaia, Moulin, Sandomirskiy and Yanovskaya, competitive division stays welfarist, but dividing a mixed manna "is either good news for everyone, or bad news for everyone". There can be multiple competitive utility profiles, and no single-valued selection is continuous or resource-monotonic. For goods, a bigger manna weakly benefits everyone under the competitive rule; no reasonable rule for bads is resource-monotonic. *Econometrica* 85(6) (2017) 1847–1871. — [DOI 10.3982/ECTA14564](https://doi.org/10.3982/ECTA14564)
- *Valuation hierarchy* [S]. Lehmann, Lehmann and Nisan prove the strict chain OXS ⊂ gross substitutes ⊂ submodular ⊂ XOS ⊂ complement-free. Decreasing marginal utility is the same as submodularity. Allocation among submodular valuations is NP-hard, with a greedy 2-approximation. *GEB* 55(2) (2006) 270–296. — [arXiv:cs/0202015](https://arxiv.org/abs/cs/0202015)
- *Algebraic characterisation of gross substitutes* [S]. Fujishige and Yang (2003) proved that gross substitutes is equivalent to M♮-concavity. Valuated matroids are the M-concave set functions. Murota and Shioura proved that M♮-concavity of a set function is equivalent to submodularity of its conjugate. — [Yokote 2023, JJIAM](https://link.springer.com/article/10.1007/s13160-023-00564-z); [Murota and Shioura, arXiv:1707.09091](https://arxiv.org/abs/1707.09091)
- *Ordered vector spaces and cones* [M]. Aliprantis and Tourky, *Cones and Duality* (AMS GSM 84, 2007), is the standard reference for cones, Riesz spaces and the Riesz decomposition property, in which divisible resources live as positive cones. Not checked.

**1.6 Species and algebraic data types for distinct items**
- Joyal, "Une théorie combinatoire des séries formelles", *Advances in Mathematics* 42(1) (1981) 1–82 [S], founded species of structures as the combinatorial interpretation of formal power series. — [DOI 10.1016/0001-8708(81)90052-9](https://doi.org/10.1016/0001-8708(81)90052-9)
- *Distinct against collective tokens* [V abstract]. Baez, Genovese, Master and Shulman's Σ-nets "give finer control over whether tokens are treated collectively or individually". Kock's whole-grain Petri nets, where tokens are individuals, are Σ-nets. — [arXiv:2101.04238](https://arxiv.org/abs/2101.04238)
- Standard facts of species [M], not re-checked in BLL 1998 or Flajolet–Sedgewick 2009:
  - a species is a functor F: 𝔹 → FinSet from finite sets and bijections;
  - the operations are sum, product (splitting the label set), composition (substitution) and derivative;
  - the basic species are sets E, linear orders L, cycles C, and partitions E∘E₊;
  - exponential generating functions are E(x)=eˣ, L(x)=1/(1−x), C(x)=−log(1−x);
  - unlabelled counting goes through the cycle index.

#### Inferences
- The OCM is the **right minimal core**. It is the exact decategorification of the SMC picture (CFS Theorem 4.2) and contains every law that "how amounts combine" and "how they are ordered" need. Everything else (divisibility, distinctness, partiality, copyability) can be declared as extra structure or properties on top of it.
- **Divisibility has two distinct meanings**, and a framework should keep them apart:
  - *physical divisibility* is a semimodule action of ℚ≥0 or ℝ≥0 on the carrier (a cone), with Riesz decomposition for splitting;
  - *asymptotic divisibility* is Fritz's many-copy regularisation (formal fractions) used for rates.

  Indivisible goods have the carrier ℕ^k, divisible goods ℝ≥0^m, and mixed goods (Bei et al.) are the product ℕ^k × ℝ≥0^m. A granularity 1/q gives (1/q)ℕ.
- **Copyable against consumable** is the monoid law: + for quantity-like resources, idempotent max/∨ for quality-like ones (CFS Examples 4.3 and 4.4). Quality-like resources (information, licences, skills) take supremal monotones; quantity-like resources take additive monotones (prices).
- **Chores are resources that are not freely disposable** (x ≱ 0). Fritz's refusal to assume x ≥ 0 is exactly what lets goods, chores and conserved quantities share one theory. Bogomolnaia et al. warn that monotonicity guarantees which hold for goods fail for bads.
- **Distinct items** need more than an OCM:
  - a pool of distinct items is a finite set of labels, with + as disjoint union, which is partial when an item cannot be in two bundles;
  - that structure is a partial commutative monoid, a separation algebra (§2 below);
  - the *space of allocations* of such a pool to k recipients is a species product: the structures of E^k on a label set U, k^|U| of them, generating function e^{kx};
  - allocations of identical units are weak compositions, C(n+k−1, k−1) of them.

  These are standard counts [M], not re-checked.
- **Valuations** are a separate layer from the resource algebra: they are maps from bundles to ℝ. Additive valuations are OCM homomorphisms, which are exactly CFS's additive monotones. Submodular and gross-substitutes valuations are characterised by exchange or concavity laws (M♮-concavity) rather than homomorphism laws, and these laws can be checked exhaustively on small ground sets.

#### Gaps
- I did not read Gonda–Spekkens beyond the abstract, so the exact form of their "framework where composition does not depend on the type of resource" is unverified.
- I did not find a source that defines a single formal object which unifies divisible, indivisible, labelled and copyable resources. The product construction proposed below is my synthesis, not a published definition.
- I did not open Bergeron–Labelle–Leroux (1998), Flajolet–Sedgewick (2009) or Yorgey's thesis, so the species facts above are [M].
- I did not find whether existence of catalysis is decidable in finitely presented OCMs.

### 2. Allocations and Conversions as Morphisms, and Their Laws

#### Takeaway
A conversion is a morphism in a resource SMC. Free conversions are an all-object sub-SMC (CFS's "partitioned process theory"), and convertibility a≽b is the existence of such a morphism. An allocation of a pool among recipients is a morphism `pool → r₁ ⊗ … ⊗ rₙ`. When ⊗ is the monoid +, this is a decomposition pool ≃ Σ rᵢ, or pool ≽ Σ rᵢ with disposal. Petri nets give the concrete, finitely presentable case: the free commutative monoid of markings, with transitions as generating conversions. Separation logic and Iris give the case where combination is *partial* (distinct or exclusive items) and conversions must be *frame-preserving*. The laws are:
- associativity, commutativity and unit of combination;
- monotonicity (x≥y ⇒ x+z ≥ y+z);
- conservation, as invariant homomorphisms;
- splittability (Riesz decomposition or refinement);
- for partial resources, validity closed under sub-bundles and frame preservation.

#### Cited Findings

**2.1 Free processes and resource theories built from process theories**
- CFS Definition 3.3 [V text]: a partitioned process theory is an SMC C of processes with a distinguished all-object-including sub-SMC C_free of free processes, "those which our agent can execute at no cost". Theorem 3.4 builds a resource theory of states from it. Theorem 3.12 builds a resource theory of processes, whose morphisms are "1-combs" of free processes around a hole (Lemma 3.11). — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- CFS on non-interaction [V text]: resource theories generated from a process theory are non-interacting "by definition, since in order to produce a collection of resource processes, one needs to produce each one individually". Non-interaction is the Riesz decomposition property (§1). — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531)
- Fritz, Example 2.9 [V text], gives conservation as an invariant: "the total number of atoms of any given element is a conservation law", as in Zn + 2HCl → ZnCl₂ + H₂. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)

**2.2 Petri nets as monoidal categories**
- Meseguer and Montanari, "Petri nets are monoids", *Information and Computation* 88(2) (1990) 105–155 [S]. A place/transition net is a graph with two operations, parallel and sequential composition of transitions. Morphisms map a transition to a whole computation. The category of nets has products, coproducts and a tensor making it symmetric monoidal closed. The conference version is LICS 1988. — [DOI 10.1016/0890-5401(90)90013-8](https://doi.org/10.1016/0890-5401(90)90013-8)
- Baez and Master, "Open Petri nets", *MSCS* 30 (2020) 314–341 [V abstract]. Open nets have input and output places given by a cospan of sets, compose by gluing, and are morphisms of a symmetric monoidal double category. The *operational* semantics (the category of processes a net can carry out) is computed compositionally; the *reachability* semantics states which output markings are reachable from an input marking. — [arXiv:1808.05415](https://arxiv.org/abs/1808.05415)
- Baez, Genovese, Master and Shulman, "Categories of nets" (LICS 2021) [V abstract]. Under their three execution semantics, pre-nets give strict monoidal categories; Σ-nets, including whole-grain nets, give symmetric strict monoidal categories; and Petri nets give commutative monoidal categories, through left adjoints. Every kind of net embeds into Σ-nets, compatibly with execution. — [arXiv:2101.04238](https://arxiv.org/abs/2101.04238)
- Fritz's footnote (§1.2 above) says a Petri net presents an ordered commutative monoid by generators and relations [V text]. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)

**2.3 Linear logic and bunched logics**
- Fritz, §10.1 [V text]: the OCM calculus is the multiplicative fragment of linear logic with ⊗ and ⅋ identified, and 1 and ⊥ identified. Linear logic proper is "much more expressive" and captures "choices" made by the agent or by an adversary, which is what its additive connectives express. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- Girard, "Linear logic", *Theoretical Computer Science* 50(1) (1987) 1–102, DOI 10.1016/0304-3975(87)90045-4 [M]. — [DOI](https://doi.org/10.1016/0304-3975(87)90045-4)
- O'Hearn and Pym, "The logic of bunched implications", *Bulletin of Symbolic Logic* 5(2) (1999) 215–244 [S]. BI has an additive (intuitionistic) implication and a multiplicative (linear) one side by side. Its resource semantics is a Kripke semantics over a partial, preordered commutative monoid, developed in Pym, O'Hearn and Yang, "Possible worlds and resources: the semantics of BI" (TCS 2004). — [Pym's BI page](https://cantab.net/users/david.pym/BI.htm); [Wikipedia: Bunched logic](https://en.wikipedia.org/wiki/Bunched_logic)
- Calcagno, O'Hearn and Yang, "Local action and abstract separation logic" (LICS 2007) [S]:
  - a separation algebra is a cancellative, partial commutative monoid (Σ,•,u), and equality is "strong" (both sides undefined, or both defined and equal);
  - σ₀#σ₁ iff σ₀•σ₁ is defined;
  - heaps under disjoint union are the standard model;
  - predicates are subsets of Σ, with separating conjunction the pointwise lift of •.

  — [LICS 2007 record](https://lics.siglog.org/archive/2007/CalcagnoOHearnYang-LocalActionandAbstr.html); [Oxford record](https://www.cs.ox.ac.uk/publications/publication4965-abstract.html)

**2.4 Iris resource algebras: user-defined resource types whose laws are verified**
- Jung, Krebbers, Jourdan, Bizjak, Birkedal and Dreyer, "Iris from the ground up", *JFP* 28, e20 (2018), DOI 10.1017/S0956796818000151. Figure 3 [V text] defines a resource algebra as a tuple (M, V: M→Prop, |−|: M→M?, ·). The operation is associative and commutative; a core |a| satisfies |a|·a = a, idempotence and monotonicity; validity is closed under sub-combination, V(a·b) ⇒ V(a); and inclusion is a ≼ b ≜ ∃c. b = a·c. — [Iris JFP](https://cs.au.dk/~birke/papers/iris-journal.pdf)
- Iris on partiality [V text]: "Every PCM is an RA, but not vice versa". "Instead of partiality, RAs use validity to rule out invalid combinations of ownership." The user of the logic "can pick the type M" for each ghost location, so resource types are user-defined, with their laws proved in Coq. — [Iris JFP](https://cs.au.dk/~birke/papers/iris-journal.pdf)
- Frame-preserving update [V text]: a ⇝ B ≜ ∀c? ∈ M?. V(a·c?) ⇒ ∃b∈B. V(b·c?). This is the law that a conversion must not invalidate the resources held by others (the "frame"). Frame-preserving updates work pointwise in products, and there are generic updates for sums, exclusive elements and finite maps. — [Iris JFP](https://cs.au.dk/~birke/papers/iris-journal.pdf)

**2.5 Games as morphisms**
- Ghani, Hedges, Winschel and Zahn, "Compositional game theory" [V abstract]. Open games are played relative to an environment and return "coutility" to it. They are morphisms of an SMC, composed in sequence or with ⊗ for simultaneous moves, drawn as string diagrams, and preserve Nash equilibria. Submitted to LICS 2018, where it appeared [M]. — [arXiv:1603.04641](https://arxiv.org/abs/1603.04641)

#### Inferences
- Convertibility of a single resource type gives an OCM, and that is enough when only feasibility matters. An allocation *procedure* needs the SMC itself, because only there do two different ways of reaching the same split differ: the morphisms count or name the processes. The matrix models of a symmetric monoidal theory over a semiring S give exactly this graduation:
  - S = 𝔹 gives feasibility (CFS's {0,1} enrichment);
  - S = ℕ counts the processes;
  - S = (ℝ∪{∞}, min, +) gives the least cost;
  - S = ℝ≥0 with column-stochastic matrices gives Markov kernels.
- **Laws an allocation morphism must satisfy**, each with its source:
  1. *Conservation*: Σᵢ rᵢ ≃ pool, or pool ≽ Σ rᵢ when disposal is free. A conserved quantity is a monoid homomorphism c to (ℤ,+) with c(pre_t) = c(post_t) for every generating conversion t (Fritz's atom count). [Inference from the cited definitions]
  2. *Monotonicity of combination* (Fritz Definition 3.1, CFS (7)): an allocation that is feasible stays feasible when the same extra goes to a recipient.
  3. *Associativity, commutativity and unit*: splitting into (r₁, (r₂, r₃)) and into ((r₁, r₂), r₃) are the same allocation up to the coherence isomorphisms (CFS Remark 2.3 on strictification).
  4. *Splittability* (Riesz decomposition, CFS Definition 4.11): if the pool can meet the total demand, it can be split into per-recipient sub-pools that each meet their demand.
     - It holds for ℕ^k and ℝ≥0^k with the componentwise order [M, standard].
     - It can fail when an indivisible unit, such as a machine, must serve two recipients.
     - It is the property under which allocation decomposes into independent per-recipient problems.
  5. *Refinement*: two different splits of one pool have a common refinement (refinement monoids). This is what makes "re-allocation" between allocations well-defined.
  6. *For partial resources*:
     - validity is closed under sub-bundles (Iris RA-VALID-OP);
     - an allocation step must be a frame-preserving update, so that a change to one recipient's bundle cannot invalidate another's;
     - distinct items can be held at most once, which is partiality or invalidity of x·x.
- **Petri nets are the natural "presentation format"** for a user-declared resource theory: places are kinds, transitions are generating conversions, and markings form the free commutative monoid ℕ^P (Fritz's footnote; Baez et al.). Open Petri nets (Baez–Master) are the compositional form, with resource systems glued along interfaces.

#### Gaps
- I found no paper that formalises *fair division* or *allocation mechanisms* specifically as morphisms in a resource SMC. The bridge between resource theories and the fair-division literature (Bei et al., Bogomolnaia et al.) appears to be open; I did not search exhaustively for "categorical fair division".
- I did not read Meseguer–Montanari, O'Hearn–Pym or Calcagno–O'Hearn–Yang in full; their details are [S].

### 3. Categorical Structures for Order-Dependent (Sequential, Stateful) Allocation

#### Takeaway
Sequential allocation is composition (◦) where simultaneous allocation is ⊗. Its standard algebraic carriers are:
- the free monoid Σ* of allocation steps acting on a state set (deterministic policies);
- Kleisli categories of a monad, giving powerset nondeterminism or distribution-monad stochasticity (Markov categories);
- lenses or optics, for parametrised processes with a backward pass (learning, optimisers, games' coutility);
- operads of wiring diagrams and doctrines of systems theory, for composing open stateful systems;
- the selection monad (X→R)→X, for search and backward induction.

Order-dependence is the failure of steps to commute. It can be expressed by asking whether the action factors through a partially commutative ("trace") monoid.

#### Cited Findings
- Fong, Spivak and Tuyéras, "Backprop as functor" (LICS 2019) [V abstract]. They define a category whose morphisms are update rules (learners) and show that gradient descent with backpropagation "defines a monoidal functor from a category of parametrised functions to this category of update rules". — [arXiv:1711.10455](https://arxiv.org/abs/1711.10455)
- Cruttwell, Gavranović, Ghani, Wilson and Zanasi, "Categorical foundations of gradient-based learning" [V abstract]. Their semantics is built from lenses, parametrised maps and reverse derivative categories, and covers ADAM, AdaGrad, Nesterov momentum, MSE and softmax cross-entropy. It extends beyond smooth settings to Boolean circuits and has a Python implementation. The arXiv page lists no journal; it appeared at ESOP 2022 [M]. — [arXiv:2103.01931](https://arxiv.org/abs/2103.01931)
- Gavranović, Lessard, Dudzik, von Glehn, Araújo and Veličković, "Categorical deep learning is an algebraic theory of all architectures" (ICML 2024, position paper) [V abstract]. They propose "the universal algebra of monads valued in a 2-category of parametric maps" as one theory for both constraints and implementations of architectures. — [arXiv:2402.15332](https://arxiv.org/abs/2402.15332)
- Spivak, "The operad of wiring diagrams" [V abstract]: wiring diagrams nest, and so are the morphisms of an operad. The paper uses them to structure database queries and circuits, and extends them to plug-and-play devices and recursion. — [arXiv:1305.0297](https://arxiv.org/abs/1305.0297)
- Myers, categorical systems theory [S]. The book is a work in progress on the author's site. A "doctrine" is a 2-functor from a theory category into monoidal double categories of systems. Systems compose in three ways: by setting parameters (lenses), by sharing variables (spans) and by exposing interfaces. See "Double categories of open dynamical systems" (2020) and Libkind and Myers, "Towards a double operadic theory of systems" (2025). — [Myers JMM 2023 talk](https://www.davidjaz.com/Talks/DJM-JMM-2023.pdf); [arXiv:2005.05956](https://arxiv.org/abs/2005.05956); [arXiv:2505.18329](https://arxiv.org/abs/2505.18329)
- Fritz, "A synthetic approach to Markov kernels, conditional independence and theorems on sufficient statistics", *Adv. Math.* 370 (2020) 107239 [V abstract]. Markov categories give a synthetic treatment of conditioning, disintegration, conditional independence and sufficient statistics, uniform across discrete and measure-theoretic probability, "following work of Golubtsov as well as Cho and Jacobs". — [arXiv:1908.07021](https://arxiv.org/abs/1908.07021); Cho and Jacobs [M]: [arXiv:1709.00322](https://arxiv.org/abs/1709.00322)
- Escardó and Oliva, "Selection functions, bar recursion and backward induction", *MSCS* 20 (2010) 127–168 [S]. J_R X = (X→R)→X is the selection monad, a strong monad. Products of selection functions compute backward induction, and finite bar recursion is a morphism available in any strong monad. Later work extends it to a selection monad transformer for backtracking search. — [DOI 10.1017/S0960129509990351](https://doi.org/10.1017/S0960129509990351); [author's PDF](https://martinescardo.github.io/papers/selection-escardo-oliva.pdf)
- In open games [V abstract], sequential composition and ⊗ ("simultaneous moves") are the two compositions, so the order of play is the order of composition. — [arXiv:1603.04641](https://arxiv.org/abs/1603.04641)
- Baez–Master's operational semantics is "a category of processes a net can carry out" [V abstract]: the free (commutative) monoidal category of firing sequences, whose composition is sequencing. — [arXiv:1808.05415](https://arxiv.org/abs/1808.05415)

#### Inferences
- **Deterministic sequential allocation**: a policy is an action of the free monoid Σ* of steps (assign item i to agent j; convert by rule t) on the state set S of partial allocations. The result is order-independent exactly when the action factors through the free commutative monoid ℕ^Σ. When only some steps commute (steps touching disjoint recipients or items), it factors through the free partially commutative monoid. That is the trace monoid of Cartier–Foata and Mazurkiewicz [M], and its independence relation is exactly the "frame" disjointness of separation logic. A finite fragment can be checked: the action commutes for each independent pair of generators at each reachable state.
- **Stochastic sequential allocation** (evolutionary operators, randomised mechanisms) lives in the Kleisli category of the distribution monad, which is a Markov category (Fritz 2020). Finite instances are column-stochastic matrices over ℝ≥0, which is a semiring model a matrix-based library can decide laws in.
- **Allocation with feedback** (stateful mechanisms, learning which allocation is good) has the shape of a lens or parametrised map (Fong–Spivak–Tuyéras; Cruttwell et al.). A forward pass allocates; a backward pass returns a utility or loss, which is the coutility of open games.
- **Search over allocations** is a selection function (X→R)→X in Escardó–Oliva's sense. Composing selection functions for sub-problems gives backward induction. An evolutionary algorithm is a stochastic selection function, i.e. a Kleisli arrow (X→R)→D(X). This is my inference; I found no paper stating it (§5).
- Traced monoidal categories (Joyal, Street and Verity 1996) [M] would express loops and feedback of resources, such as a catalyst returned or a stock carried to the next turn. I found nothing on their use for allocation.

#### Gaps
- I found no source that treats sequential allocation (picking sequences, serial dictatorship) categorically.
- Myers's book status and content are [S]; I did not read the book.
- I did not check whether Cruttwell et al. or open games give lawful conditions (lens laws) for optimisers that a library could decide on finite instances.

### 4. Laws That Can Be Decided Mechanically on Finite Fragments or Presentations, and What a Finite Verdict Establishes

#### Takeaway
Four kinds of verdict must be kept apart:
1. **Proved by construction or universal property.** The carrier is built from verified primitives by closure operations that preserve the laws (varieties, ordered varieties, free constructions), or a homomorphism law reduces to finitely many checks on generators.
2. **Decided on a finite model.** Every tuple of a finite carrier is checked. This establishes the law *for that model only*.
3. **Decided for a finite presentation.** Examples are convertibility in a finitely presented OCM or Petri net (decidable, but EXPSPACE-complete for commutative-semigroup equality and Ackermann-complete for Petri-net reachability) and equality by Knuth–Bendix when completion terminates.
4. **Sampled.** Property-based tests on samples are evidence, never proof.

Refutations (a counterexample, or a monotone M with M(a) < M(b)) are definitive in all four cases.

#### Cited Findings
- **The project's category library** [V text, this repository's CLAUDE.md, §3.1, the kategoria row]. kategoria answers every claim with a verdict: holds, fails or undecided, with the place and the reason. It decides claims on finite categories, functors (found by exhaustive search), limits and colimits, set-valued functors and Kan extensions. Presentations are completed by Knuth–Bendix on paths, "with derivations a small checker verifies". Symmetric monoidal theories have string diagrams "equal exactly when isomorphic" and "models in matrices". — `c:\Users\Micro\Desktop\The Science\CLAUDE.md` §3.1
- **Word problem for commutative semigroups** [S]. Mayr and Meyer, "The complexity of the word problems for commutative semigroups and polynomial ideals", *Adv. Math.* 46(3) (1982) 305–329: any decision procedure needs space exponential in the instance size, and a simple procedure achieves the bound. Finitely presented commutative semigroups are equivalent to reversible Petri nets or vector addition systems. — [MIT DSpace](https://dspace.mit.edu/handle/1721.1/149010); [Koppenhagen and Mayr 1996](https://mediatum.ub.tum.de/doc/1094422/1094422.pdf)
- **Petri-net reachability** [S] is decidable and Ackermann-complete:
  - the lower bound was found independently in 2021 by Czerwiński and Orlikowski ("Reachability in vector addition systems is Ackermann-complete", FOCS) and by Leroux ("The reachability problem for Petri nets is not primitive recursive");
  - the matching upper bound is Leroux and Schmitz (2019).

  — [arXiv:2104.13866](https://arxiv.org/abs/2104.13866); [arXiv:2104.12695](https://arxiv.org/abs/2104.12695); [Lasota, arXiv:2105.08551](https://arxiv.org/abs/2105.08551)
- **Ordered analogue of Birkhoff's HSP theorem** [S]. Bloom, "Varieties of ordered algebras", *JCSS* 13(2) (1976) 200–212: a class of ordered algebras is axiomatisable by term inequations iff it is closed under subalgebras, products and suitable homomorphic images. The paper also gives a complete inequational logic. A modern statement is in arXiv:1812.02016. — [DOI 10.1016/S0022-0000(76)80030-X](https://doi.org/10.1016/S0022-0000(76)80030-X); [arXiv:1812.02016](https://arxiv.org/abs/1812.02016)
- **Monotones as certificates of impossibility** [V text]: if M(a) < M(b) then a⋡b, but the converse fails in general. A complete family always exists, but it is indexed by R itself (CFS Proposition 5.2). In the Archimedean ℚ-vector-space regularisation, functionals are complete (Fritz Theorem 6.15). — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531); [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- **Laws proved generically, then instantiated** [V text]. Iris proves frame-preserving updates once for constructions (sum, exclusive, finite maps, pointwise in products), and every RA built from them inherits them. This is the "by construction" route in a proof assistant. — [Iris JFP](https://cs.au.dk/~birke/papers/iris-journal.pdf)
- Classical references, not checked [M]:
  - Birkhoff (1935): equational classes are exactly those closed under H, S and P;
  - Knuth and Bendix (1970), "Simple word problems in universal algebras": when completion terminates with a convergent system, the word problem is decided by normal forms, and it may fail to terminate;
  - Claessen and Hughes, "QuickCheck", ICFP 2000, DOI 10.1145/351240.351266: random testing of stated properties, which finds counterexamples but proves nothing;
  - for commutative presentations, Gröbner bases of binomial ideals play the part of Knuth–Bendix;
  - Petri-net place invariants (vectors y with yᵀC = 0, where C is the incidence matrix) are linear-algebra certificates of conservation.

#### Inferences
**What can be decided and how**
- *Laws of combination on a finite carrier* (associativity, commutativity, unit, idempotence, cancellativity, monotonicity of + in ≥) can be decided by exhaustive enumeration in O(|A|³) checks. The verdict "holds" means the declared finite structure is a model. It says nothing about an infinite carrier from which the finite one was truncated. Truncating ℕ to {0..c}, for instance, needs *partial* addition (a PCM, or a validity predicate as in Iris) to stay lawful; saturating addition changes the theory.
- *Homomorphism laws on free or presented carriers* reduce to finitely many checks, by the universal property:
  - an additive monotone on ℕ^k is determined by its values on the k generators (CFS Example 5.4);
  - if the order is generated by finitely many conversions xᵢ ≥ yᵢ, then a monoid homomorphism f is monotone iff f(xᵢ) ≥ f(yᵢ) for each generator. The pullback of ≥ along f is a compatible preorder, so it contains the generated preorder iff it contains the generators;
  - a conserved quantity likewise needs c(pre_t) = c(post_t) only per transition t.

  These are **proofs for all elements**, not samples. This argument is my own and should be checked, but it is elementary.
- *Equational and inequational laws of carriers built by products and sub-structures* are preserved by Birkhoff and Bloom closure. A product of lawful components is lawful, which is the "by construction" route for a product resource type. ∀∃-laws such as Riesz decomposition and refinement are not equational: products preserve them [M], but sub-structures need not. Horn laws such as cancellativity are preserved by products and sub-structures but not quotients [M]. Such laws must be checked or proved per construction.
- *Convertibility a ≽ b* in a finitely presented theory (Petri net or OCM) is decidable, but Ackermann-complete for reachability and EXPSPACE-complete for commutative-semigroup equality. A practical tool should therefore answer:
  - "convertible", with a *witness* firing sequence, which is a checkable certificate;
  - "not convertible", with a *monotone* or invariant violated, which is also checkable;
  - otherwise "undecided within the bound".

  This mirrors kategoria's holds, fails or undecided.
- *Equality of conversion processes* (morphisms) in a free SMC is decided by diagram isomorphism (kategoria). Equality modulo declared laws needs completion, which may not terminate: undecided is then an honest verdict.
- *Laws over ℝ* (divisible goods, real valuations) cannot be decided by enumeration. The options are:
  - prove them symbolically, e.g. linear (in)equalities by LP certificates such as Farkas multipliers [M];
  - test them on samples;
  - decide them on exact rational instances.

  Floating-point + is not associative, so a framework should use exact rationals or intervals for law checking.

**What each verdict establishes**
- *Holds on a finite model*: the model satisfies the law. It is a proof only if the model *is* the resource type, not a truncation of it.
- *Holds on generators, via the universal property*: proof for the whole free or presented structure.
- *Holds by construction from verified primitives*: proof, conditional on the primitives and on the closure theorem (HSP, Bloom).
- *Fails, with a counterexample or a monotone*: definitive refutation.
- *No counterexample in N samples*: evidence only. With uniform sampling over a finite space of size |X|, it bounds the fraction of violating tuples, not their existence [M].

#### Gaps
- I did not verify the precise theorem statements of Mayr–Meyer, Bloom or Knuth–Bendix (all [S] or [M]).
- I found no literature on the decidability or complexity of computing Fritz's regularised rates, or of deciding catalysis, for finitely presented OCMs.
- I did not find a published tool that verifies user-declared resource-type laws other than Iris, in Coq, which is proof-based, not decision-based.

### 5. Prior Work Using Category Theory to Specify or Verify Evolutionary Algorithms, Search Spaces or Genetic Operators

#### Takeaway
I found **no primary source that specifies evolutionary algorithms or genetic operators categorically**: no functorial or monadic account of crossover or mutation, and no categorical verification of EA laws. Several targeted searches returned nothing. The closest prior work is *algebraic*:
- Radcliffe's forma analysis: equivalence relations generate representations and operators;
- Rowe, Vose and Wright: permutation-group symmetries and invariant operators;
- Moraglio's geometric framework: operators defined by the axioms of a metric, with offspring in metric segments;
- Mitavskiy: comparing EAs via re-encodings.

Adjacent categorical work exists: categorical deep learning (monads in parametric maps), lenses for optimisers, and the selection monad for search. This is a genuine gap that a framework could fill.

#### Cited Findings
- My searches for "category theory" with "genetic algorithm", "evolutionary algorithm", "evolutionary computation", "genetic programming", functor and monad returned no categorical treatment of EA operators [V, my searches on 2026-11-20]. One search did surface a ResearchGate fragment on GA operators for IoT test synthesis via universal algebra and category theory; it had no clear authors or venue and is excluded as unverifiable. — [search result summary, ResearchGate](https://www.researchgate.net/publication/372633300_Category_Theory)
- Radcliffe, "The algebra of genetic algorithms", *Annals of Mathematics and Artificial Intelligence* 10 (1994) 339–384 [V text, abstract and overview]. It gives a rigorous formulation of forma analysis: knowledge about a search space, codified as equivalence relations on it, generates a genetic representation and operators. "A single characterisation of a space leads to a unique genetic representation", and random assorting recombination (RAR_w) is "a universal recombination operator". — citation as printed in the header of the copy read (no DOI or URL was confirmed during this research; locate it via the journal's volume 10)
- Rowe, Vose and Wright, "Group properties of crossover and mutation", *Evolutionary Computation* 10(2) (2002) 151–184 [S]. Search-space symmetries form a permutation group. Operators that respect them are described by a mixing matrix and permutation matrices, and invariant subsets generalise schemata. The group can induce a group structure on the search space. A later paper by the same authors, "Representation invariant genetic operators" (2010), characterises the transitive case. — [UMT ScholarWorks](https://scholarworks.umt.edu/cs_pubs/11); DOI 10.1162/106365602320169839
- Moraglio, *Towards a geometric unification of evolutionary algorithms* (PhD thesis, Essex 2007) [V text]:
  - Definition 3.3.3: CX is a geometric crossover on (C,d) if Im[CX(p₁,p₂)] ⊆ [p₁;p₂]_d, i.e. "offspring lay between parents";
  - a geometric ε-mutation has all offspring within ε of the parent;
  - the framework is "unification by abstraction" on "an axiomatic notion of distance". Category theory is cited only as the exemplar of unification by abstraction (Chapter 15), not used.

  — [Moraglio thesis](https://eden.dei.uc.pt/~moraglio/Thesis_final.pdf)
- Moraglio, "Abstract convex evolutionary search" (FOGA) [S]: evolutionary algorithms with geometric crossover and no mutation perform convex search. — [FOGA paper](https://eden.dei.uc.pt/~moraglio/foga24e-moraglio.pdf)
- Mitavskiy, "Comparing evolutionary computation techniques via their representation" (GECCO 2003) [S]: a framework for comparing techniques by their representations, which argues that any reasonable evolutionary search algorithm can be re-encoded as a "binary semi-genetic algorithm". I could not confirm whether it uses morphisms or categories. — [GECCO 2003 PDF](https://cs.york.ac.uk/rts/docs/GECCO_2003/papers/2723/27231196.pdf)
- Kozyrev, "Category theory and theory of evolution", *Lobachevskii J. Math.* (2023) [S], discusses *biological* evolution as computation, related to machine learning through functional programming. It is not about EA operators. — [Springer](https://link.springer.com/article/10.1134/S1995080223060252)
- Tuyéras, "Category theory for genetics I: mutations and sequence alignments" (*TAC* 2018) [S], is about genetics, not evolutionary search. — [arXiv:1805.07002](https://arxiv.org/abs/1805.07002)
- The adjacent categorical works are in §3: Gavranović et al. (2024) [V abstract]; Cruttwell et al. [V abstract]; Escardó–Oliva's selection monad [S].

#### Inferences
- The algebraic EA literature already supplies **laws** a categorical specification could state and decide:
  - Rowe–Vose–Wright give *equivariance*: an operator commutes with a symmetry group's action, a naturality square checkable on finite search spaces;
  - Moraglio gives *geometricity*: Im[CX(p₁,p₂)] ⊆ [p₁;p₂]_d, decidable on finite metric spaces;
  - Radcliffe gives *respect and assortment* of formae, conditions on equivalence relations;
  - a further natural law is that an operator is a Kleisli arrow of the distribution monad (a Markov kernel), whose finite instances are stochastic matrices.
- A typed account fits the user's resource framework. Each resource type determines a search space, its allocation species. Operators are then natural transformations between species, labelled-structure maps that commute with relabelling, so that equivariance under renaming of items or agents is naturality. Mutation is a Markov kernel F → D(F); crossover is F × F → D(F). This is my proposal, not found in the literature.

#### Gaps
- Coverage of GECCO, FOGA and ACT proceedings was by web search only; a systematic search of the ACT proceedings and dblp might find workshop papers I missed.
- I did not read Mitavskiy 2003, Rowe–Vose–Wright 2002 or Kozyrev 2023 in full.

### 6. What This Means for a Universal Framework

**(a) A resource type: carrier, combination, order and divisibility**

A *resource type* is a finite product of *components* T = T₁ × … × T_m, combined componentwise. Products preserve equational and inequational laws (Birkhoff, Bloom), so a lawful product follows from lawful components. Each component Tᵢ declares the following.
- **Kinds**: a finite set Kᵢ of kinds, the places of a Petri net or the labels of goods.
- **Quantity semiring** Sᵢ, which fixes divisibility:
  - ℕ: indivisible, identical units;
  - (1/q)ℕ: granular;
  - ℚ≥0 or ℝ≥0: divisible, a cone. The divisible components are semimodules, with x = n·(x/n) for all n.
  - ℤ or ℚ: signed, for conserved balances and debts;
  - 𝔹 under ∨, or ([0,∞), max): quality-like, copyable.

  The carrier of quantities is Sᵢ^{Kᵢ}.
- **Combination**: + (quantity-like) or an idempotent join (quality-like, a+a = a), as in CFS Examples 4.3 and 4.4.
- **Order**: a preorder ≥ generated by declared conversions xⱼ ≥ yⱼ, which are Petri-net transitions or free processes, together with optional free disposal (x ≥ 0 per kind; refused for chores and wastes, following Fritz and CFS Remark 4.19).
- **Validity** (optional): a predicate V closed under sub-combination, V(a+b) ⇒ V(a) (Iris RA). It expresses capacities, exclusivity and *distinct* items: a labelled item may be held at most once, so x+x is invalid. With it, combination is partial without leaving total algebra.
- **Declared properties**, each a checkable law: cancellative; catalysis-free; non-interacting (Riesz decomposition, i.e. *splittable*); refinement; quantity-like or quality-like; waste-free; conservation laws (homomorphisms c: T → ℤ with c(pre_t) = c(post_t)).
- **Space of allocations**: the species of how a pool is distributed:
  - labelled goods give E^k on the item set;
  - identical units give weak compositions;
  - divisible goods give a simplex scaled by the pool;
  - mixed pools give products of these.

  It is used to enumerate finite models, count them, and sample uniformly.

**(b) Allocations and sequential allocation as morphisms**

The resource type generates an SMC (free on the declared conversions, i.e. the operational semantics of the Petri net), its convertibility OCM (feasibility), and matrix models over a chosen semiring (𝔹 for feasibility, ℕ for counting, min-plus for cost, ℝ≥0 stochastic for random mechanisms).
- *Allocation* of pool p to recipients 1..n is a morphism p → r₁ ⊗ … ⊗ rₙ. Its laws are conservation (Σ rᵢ ≃ p, or ≤ p with disposal), validity of each rᵢ and of the whole, and coherence under regrouping.
- *Conversion* is a morphism a → b built from generating conversions and free processes.
- *Sequential allocation* is a word in Σ* (steps) acting on states, or a Kleisli arrow for random or nondeterministic steps. The policy is order-independent iff the action factors through ℕ^Σ, or through a trace monoid when only declared-independent steps commute. Frame-preserving updates make "independent" precise.
- *Stateful or learning allocators* are lenses or parametrised maps (Cruttwell et al.); search over allocations is a selection function (X→R)→D(X).

**(c) Which laws to verify, how, and what each verdict means**

| Law | Method | Verdict means |
|---|---|---|
| Associativity, commutativity and unit of + for primitive semirings (ℕ, ℤ, ℚ, max, ∨) | By construction: primitives verified once; products preserve the laws | Proof for every resource type built this way |
| Same laws for a user-defined finite carrier or truncation | Exhaustive check on the finite model | Proof for that finite model only, not for any infinite structure it approximates |
| Monotonicity of + w.r.t. a generated order | By construction: the generated order is compatible by definition | Proof |
| Additive monotones, prices and conservation laws (homomorphisms) | Check on generators and on each generating conversion (universal property) | Proof for all elements |
| Riesz decomposition and refinement (splittability) | Prove for primitive cones and ℕ^k [M]; otherwise check on finite fragments or by bounded search | Finite-fragment verdict only; a counterexample refutes definitively |
| Convertibility a ≽ b in a presented theory | Bounded search for a witness; monotones or invariants for impossibility | "Holds" with a witness, "fails" with a monotone, or "undecided" (the problem is Ackermann-complete in general) |
| Equality of processes (string diagrams) | Isomorphism of diagrams in the free SMC (kategoria); completion for theories with laws | Decided when free; undecided if completion does not terminate |
| Validity closed under sub-bundles; frame-preserving updates | Exhaustive check on a finite RA; generic lemmas for sum, product and finite-map constructions (Iris) | Proof per construction or per finite model |
| Laws over ℝ (divisible valuations, submodularity on continuous goods) | Exact rationals or symbolic certificates where possible; otherwise sampling | Sampling is evidence, never proof; floating point must not be used for law checks |
| Valuation classes (submodular, gross substitutes as M♮-concavity) on small ground sets | Exhaustive check of exchange inequalities over all subsets | Proof for that ground set |
| EA operators: Markov kernel, equivariance (naturality), geometricity | Stochastic-matrix checks, naturality squares and segment containment on finite instances | Proof for the finite instance; sampled evidence at scale |

Throughout, **refutations are definitive and positive verdicts are scoped**. A finite-model "holds" must say which model; a construction proof must name the closure theorem it relies on; a sampled "no counterexample" must state the sample size and distribution.

## Part V. Fitness Evaluation from Heterogeneous Evaluators in Evolutionary Allocation

*Remark.* Part V's scope and conventions. Scope: how the fitness of a candidate allocation or policy can come from evaluators of very different kinds (closed forms, input–output and CGE models, system dynamics, agent-based models, machine-learned surrogates, language models, people), and how a search can use them together. These notes were written for the design of a universal evolutionary resource-allocation framework with pluggable evaluators (in this repository, on heuresis).

Markers: **[V]** verified by reading the source itself during this research (where only the abstract page was read, this is said); **[S]** from a search snippet or a secondary summary only; **[M]** from memory and not checked here. DOIs or arXiv identifiers are given where known; a DOI marked [M] has not been checked.

Research date: 2026-11-20. About 45 tool calls; the full texts of AlphaEvolve (first 100k characters), FunSearch (PMC), Lamperti et al. (ar5iv), Lee et al. (JASSS) and the Rakshit–Konar–Das survey (author's manuscript, extracted locally) were read; the rest from abstract pages or snippets.

### 1. The Interface an Evaluator Should Expose (Value, Uncertainty, Cost, Feasibility, Descriptors, Provenance)

#### Takeaway
Every mature system reviewed converges on more than a scalar. FunSearch scores per input and aggregates, discarding programs that fail limits; AlphaEvolve's `evaluate` returns a *dictionary* of scalars, supports staged (cascaded) evaluation and adds LLM-graded properties as extra keys; Ax records a mean *and* a standard error per metric, with SEM 0 meaning noiseless and a missing SEM meaning "infer the noise". heuresis's own `Evaluation` already holds objectives, constraints, descriptors, cost, a failure status and its reason, but has no field for uncertainty, fidelity, the random stream used, or provenance. Those are the gaps a pluggable-evaluator framework must fill.

#### Cited Findings

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

#### Inferences
- The minimal common record is: a vector of named metrics, each with a point value *and* an uncertainty (SEM or the raw replicates); constraint values; descriptors; the cost actually spent; a status with reason; and enough provenance to replay (evaluator identity and version, fidelity, seed or random-stream key, input digest). FunSearch's signature clustering and AlphaEvolve's dictionary show that keeping *per-case* and *per-property* scores, not just an aggregate, pays for itself in diversity.
- Two evaluation-control hooks recur and should be part of the interface rather than of each evaluator: (i) staged evaluation (a cascade, or Optuna-style intermediate reports enabling early stopping), and (ii) the ability to ask an evaluator for *more replicates* of an already-evaluated candidate (needed by resampling and racing, §2).
- Ax's convention (SEM missing ⇒ unknown noise, SEM = 0 ⇒ deterministic) is a clean way to let closed forms and stochastic simulators share one record.

#### Gaps
- No standard serial schema for evaluation records across EC frameworks was found (heuresis's own open decision 4, "a serial format for records", is still open). pymoo/Optuna/Nevergrad details above are [M].
- AlphaEvolve's second half (73k characters) was not read; whether it reports metrics' uncertainty or repeated evaluation was not checked.

### 2. Replications per Candidate for Stochastic Simulations and ABMs, and Cost-Reducing Techniques with Known Guarantees

#### Takeaway
There is no universal replication count: ABM practice commonly uses ≤ 100 runs per setting, a power analysis for a medium effect (d = 0.5, α = 0.05, power 0.8) asks for about 64–78 runs per arm, and stabilisation criteria can ask for 1,000. Inside an EA, *adaptive* allocation beats fixed allocation: dynamic resampling to a standard-error threshold, OCBA (asymptotically optimal probability of correct selection under normality), racing with Hoeffding or empirical-Bernstein bounds (a 1 − δ guarantee per comparison), and CMA-ES's rank-change uncertainty handling, which needs only a few extra evaluations. Common random numbers help when noise is positively correlated across candidates and can hurt otherwise. Machine-learned surrogates of ABMs and CGE models cut cost by 2–4 orders of magnitude but carry no guarantee beyond out-of-sample validation.

#### Cited Findings

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

#### Inferences
- For an ABM or stochastic economic simulator the replication count should be set *per comparison*, not per candidate: what matters is whether selection can tell candidates apart. Power analysis (Lee et al.) gives a starting n for a declared smallest meaningful difference δ; racing or SEDR then spends more only where ranks are in doubt. A fixed large n wastes budget and, with very large n, makes trivial differences "significant".
- Guarantees are of three kinds: PAC-style per race (Hoeffding/Bernstein, distribution-free but loose; Bernstein tighter when variance is small); probability of correct selection under normality and indifference zones (OCBA asymptotically, KN exactly); and none (surrogates, CRN). A framework should label each technique with which guarantee it carries.
- Lamperti et al. show that when the criterion is a *test* (KS p-value) the evaluator's output is itself uncertain and binary; a surrogate predicting it should be calibrated (Platt scaling raised precision from 94.7% to 99.4%).
- Multimodal ABM output means the evaluator should be able to return the replicate distribution, not just mean and SEM, so that quantiles or attractor shares can be objectives.

#### Gaps
- No study was found that measures replication counts specifically for ABM-in-the-loop *evolutionary* policy optimisation (as opposed to calibration or one-off experiments).
- The UH-CMA-ES full text was not reachable (HAL blocked by bot protection); its treatment rules are [M].
- The INRIA CRN paper's title and authors were not retrieved.
- Evidence for EAs over CGE or SAM-multiplier evaluators is thin; the Kenya work is an extended abstract and did not report evaluation counts or run time in the part read.

### 3. Reliability of Language Models as Evaluators of Solutions, and Safeguards

#### Takeaway
The strongest results from LLM-driven evolution (FunSearch, AlphaEvolve, EoH, LLaMEA) all keep the LLM as *generator* and let a programmatic evaluator decide; AlphaEvolve uses LLM grades only for properties hard to code, as extra scores beside the programmatic ones, and its authors say LLM-provided evaluation is "not a setting we have optimized for". As judges, strong LLMs agree with human preferences at roughly human–human levels on chat tasks (> 80%, Zheng et al. 2023), but with position, verbosity and self-preference biases, large variation across tasks and datasets (JUDGE-BENCH), and vulnerability to universal adversarial phrases that can push absolute scores to the maximum, a direct risk under evolutionary optimisation pressure (Goodhart; Gao et al. on proxy over-optimisation). Safeguards with evidence: order swapping and aggregation, comparative rather than absolute scoring, a judge from a different model family, selective evaluation with escalation and a provable agreement guarantee, and prediction-powered inference to combine few human labels with many model scores.

#### Cited Findings

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

#### Inferences
- The evidence separates two roles. *LLM as generator, program as judge* is well supported at scale (≈10⁶ samples in FunSearch). *LLM as judge inside the selection loop* is weakly supported for open-ended solution quality and is directly threatened by the optimiser: an EA is an adversary that will find phrases, formats or artefacts the judge over-rewards (Raina et al.), and proxy scores diverge from gold under optimisation (Gao et al.).
- Hence: LLM scores should enter as a *separate*, lower-standing objective or descriptor (AlphaEvolve's extra keys; QDAIF's descriptors), never merged into the programmatic fitness; comparative judgments should be preferred to absolute ones; candidates should be presented in both orders; the judge should not be the model that generated the candidates; and a human-labelled audit stream should track agreement (chance-corrected, e.g. κ) during the run, with selective evaluation or escalation when confidence is low.
- PPI is the natural way to report a *population-level* quantity (e.g. "share of the archive judged acceptable") from many LLM scores and a few human ones with valid intervals; it does not by itself make per-candidate selection reliable.

#### Gaps
- No controlled study was found measuring how often LLM-judged fitness in an evolutionary loop is exploited (score inflation without gold improvement), as opposed to one-shot judging; QDAIF's full agreement figures and any hacking analysis were not read.
- No study was found of LLM judges evaluating resource *allocations* or policies specifically; all reliability evidence is from text, chat, NLP or code.
- Most LLM-judge reliability numbers are for 2023–2024 models; 2025–2026 model generations may differ.

### 4. Combining Evaluators of Different Kinds and Costs (Cheap Surrogate, Expensive Simulation, Human) and Tracking Trust in Each

#### Takeaway
Studies combine evaluators in four patterns: (1) *cascades* that pass a candidate to a costlier evaluator only if a cheaper one finds it promising (AlphaEvolve's staged tests; Trust or Escalate's cheap-to-strong judges; Optuna-style pruning); (2) *model management*, in which a surrogate pre-screens and the true evaluator is queried for a chosen subset, with the surrogate's reliability checked on re-evaluated points (Jin's surveys; Lamperti et al.'s active sampling; surrogate-assisted interactive EC); (3) *parallel objectives*, keeping each evaluator's output as its own metric (AlphaEvolve's dictionary; CGE calibration errors as separate objectives); and (4) *descriptor use*, where a judge's output defines the behaviour space of a quality-diversity archive (QDAIF). Trust is tracked by measuring each proxy against a more trusted reference on a sample (re-evaluation, human audit) and switching the proxy off when its reliability falls below a threshold. Asynchronous evaluation of evaluators with very different run times biases an EA toward cheap-to-evaluate regions unless corrected.

#### Cited Findings

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

#### Inferences
- A sound combination keeps a *trust order* among evaluators (programmatic check ≻ calibrated simulation ≻ validated surrogate ≻ LLM judge, with people placed by the task: authoritative for preferences, fallible for facts) and lets a cheaper evaluator *filter* or *prioritise* but not *decide* where a more trusted one is available at acceptable cost.
- Trust should be a running statistic, not a constant: the rate at which a proxy's ranking agrees with the reference on re-evaluated or audited candidates (rank correlation for surrogates, κ for judges against people), with the proxy demoted when it falls below a declared threshold (AES-IGA's rule generalised).
- When evaluators of very different cost run asynchronously, the framework should either generate in generations (heuresis's generational algorithms are order-independent by design) or correct the time bias explicitly; otherwise cheap evaluators silently steer the search.

#### Gaps
- No study was found that tracks trust in *several* heterogeneous evaluators simultaneously (surrogate + simulation + LLM + human) within one evolutionary run; the patterns above come from pairs.
- Takagi's specific fatigue numbers (e.g. typical generation limits) were not verified.

### 5. Fairness and Equity as Objectives or Constraints in Evolutionary Allocation

#### Takeaway
In evolutionary allocation studies, equity is most often a separate objective in a multi-objective search (usually NSGA-II): a Gini coefficient beside economic benefit in water allocation, accessibility vs equity in EMS siting, a resource-fairness term among five objectives in epidemic patient allocation. The formal literature warns that Gini minimisation alone can be satisfied by levelling down unless outcomes are shifted or efficiency is kept as an objective, and offers alternatives (generalised Gini, α-fairness from utilitarian through proportional to maximin). Treating objectives as separate also avoids weighing incommensurable quantities, the stated motive in the CGE calibration work.

#### Cited Findings
- A water-allocation model uses the Gini coefficient for equity alongside economic efficiency and NSGA-II "to derive the Pareto front of the water resources allocation system" [S] — [Scientific Reports 2021, DOI 10.1038/s41598-021-04734-2](https://link.springer.com/10.1038/s41598-021-04734-2).
- An EMS station-placement preprint is bi-objective, accessibility vs equity, solved with NSGA-II [S] — [Research Square preprint](https://sciety.org/articles/activity/10.21203/rs.3.rs-7135511/v1); a COVID-19 patient-allocation study includes "emergency medical resource fairness" with severity, hospital level, matching benefit and distance, solved with NSGA-II [S] — [PMC9753027](https://pmc.ncbi.nlm.nih.gov/articles/PMC9753027).
- Ogryczak argues that with an appropriate outcome shift, Gini minimisation is consistent both with inequity minimisation and with outcome maximisation [S] — [EconPapers](https://econpapers.repec.org/RePEc:spr:oprchp:978-3-030-18500-8_11); finding an allocation minimising the Gini index is NP-hard [S] — [Fair Division Minimizing Inequality, arXiv:1810.04259](https://arxiv.org/pdf/1810.04259).
- The generalised Gini function can represent utilitarian and maximin objectives, and α-fairness with α = 1 is proportional fairness (log Nash welfare) [S] — [DECAF, arXiv:2502.04281](https://arxiv.org/pdf/2502.04281); fairness-aware multi-objective optimisation [S] — [arXiv:2207.12138](https://arxiv.org/pdf/2207.12138).
- A health-policy paper argues equity and efficiency "are, and ought to be, treated as complementary rather than conflicting considerations" [S] — [PMC8536061](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC8536061/).
- The CGE calibration MOEA keeps objectives separate because weights cannot be given a priori ("not straightforward to weigh GDP gains against welfare gains"), and MOEAs "enable the user to visualise a wide spectrum of outcomes without imposing any a priori preferences" [V] — [GTAP 2022 extended abstract](https://www.gtap.agecon.purdue.edu/resources/res_download.asp?D_ID=11031).
- Deb's feasibility rule (feasible ≻ infeasible; infeasible compared by total violation) lets an equity floor be imposed as a hard constraint without penalty weights [V, as implemented and cited in heuresis] — `foundations/heuresis/search/include/heuresis/search/evaluation.hpp`.

#### Inferences
- Equity should be represented in at least two of three ways the framework supports natively: as one or more objectives (Gini or Theil for dispersion, the worst-off group's outcome for maximin, a share for sufficiency), as a declared hard constraint (a floor per group) handled by feasibility rules, or as a descriptor so that a QD archive keeps allocations spanning the equity–efficiency range for a person to choose from.
- Because equity metrics are computed from an allocation's *distribution over units*, the evaluator should return that distribution (per region, group or household) so that alternative equity measures can be computed later without re-running an expensive simulator.
- With a stochastic evaluator, equity metrics are themselves noisy and often non-linear (a Gini of means ≠ mean of Ginis); which is meant must be declared.

#### Gaps
- No survey of fairness objectives in evolutionary allocation was found; the examples are individual NSGA-II applications seen through snippets.
- No study was found combining equity objectives with stochastic ABM or CGE evaluators and resampling.

### 6. What This Means for a Universal Framework

The proposals below are inferences from the findings above, set against heuresis's existing `search::Evaluation` (objectives, constraints, descriptors, cost, status, reason), its ask–tell protocol and keyed draws, and the infrastructure's practice of grading statements (A structured source, B text with passage, C language model) and of dual verdicts (standing and measure kept apart, as in kybernesis).

#### 6.1 An Evaluator's Record

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

#### 6.2 Noise Policy

- **Replicates on demand, not fixed.** Start each candidate at n_min (from a power calculation for the declared smallest meaningful difference δ: about 64–78 per arm for d = 0.5 at power 0.8 in Lee et al.; fewer for larger effects), then spend more only where selection is in doubt.
- **Selection procedures with stated guarantees:** a race (Hoeffding, or empirical Bernstein when variances are small) when a distribution-free 1 − δ guarantee is wanted; OCBA or an indifference-zone procedure when outputs are near normal; SEDR (resample to a standard-error threshold) as a simple default. Each run's record states which guarantee its selections carry.
- **Monitor rather than assume:** UH-CMA-ES's rank-change measure as a run-time diagnostic of whether noise is overwhelming selection, with treatments (more replicates, larger population, larger step) logged.
- **Common random numbers by construction:** keyed draws make CRN free (same stream_key for all candidates of a generation); keep it only where the measured variance of *differences* falls, since it can hurt.
- **Never more replicates than the decision needs:** very large n makes trivial differences "significant" (Lee et al.).
- **Multimodal outputs:** keep replicates so that quantiles, attractor shares or risk measures can be objectives.

#### 6.3 Cost Policy

- **Cascades** (AlphaEvolve): feasibility and cheap programmatic checks first, then low-fidelity simulation, then full simulation, then (if any) human review; a candidate advances only if promising.
- **Surrogates under model management:** a surrogate may pre-screen or order candidates; the true evaluator is queried on a subset chosen by an infill rule; the surrogate is retrained and its agreement with the true evaluator is tracked on every re-evaluated point; for binary criteria (a calibration test), calibrate it (Platt scaling).
- **Budget in the evaluator's own unit** (seconds, core-hours, LLM tokens, minutes of a person's attention), as heuresis already allows.
- **Asynchrony with heterogeneous costs:** prefer generational algorithms (order-independent in heuresis), or correct the bias toward cheap-to-evaluate regions explicitly.

#### 6.4 Caching and Reproducibility

- **Key** every evaluation by (input_digest, evaluator_id, evaluator_version, fidelity, stream_key); a cache hit returns the recorded evaluation, and a request for more replicates uses new stream indices.
- **Record** every evaluation, append only, never replaced: the infrastructure already keeps runs under the digest of their canonical JSON and seals them in archeion's journal; evaluations can be held the same way.
- **LLM evaluations** are not reproducible by seed: record the model identifier, prompt and rubric digests, temperature, every sample and the presentation order, and treat a re-query as a new replicate, not a cache miss.
- **People:** record who judged what, when, in which order and session, so that fatigue and order effects can be estimated afterwards.

#### 6.5 Trust in a Language Model's Judgement

- **Generator first, judge last.** Use LLMs to propose (as in heuresis's decision 0007) and let programmatic or simulated evaluators decide wherever they exist (FunSearch, AlphaEvolve).
- **Where an LLM must score**, its metric has standing *proposed* (grade C) and enters only as a separate objective, a tie-breaker below the verified objectives, or a descriptor (QDAIF), never summed into the trusted fitness.
- **Pairwise rather than absolute** (more robust to adversarial phrases), with both orders shown and aggregated (Balanced Position Calibration); a rubric with anchors; k samples per judgment, with their spread reported as the metric's uncertainty.
- **A judge from a different model family** from the generator (self-preference).
- **An audit stream:** a random sample of LLM-judged candidates also judged by people (or by a programmatic check where one exists); agreement tracked chance-corrected (κ) through the run; the judge demoted when agreement falls below a declared level. Low-confidence items escalate (Trust or Escalate: cheap judge, then strong judge, then person). Population-level statements combining many model scores and few human labels use prediction-powered inference.
- **Goodhart watch:** the gap between the LLM score and the audited score over generations is itself reported, since optimisation pressure is expected to open it (Gao et al.; Raina et al.).

#### 6.6 Trust in a Person's Judgement

- **Comparisons over ratings:** pairwise comparisons or choices from small subsets, aggregated by Bradley–Terry (or Thurstone), with ties and intransitivity kept.
- **A fatigue budget per session** (in judgments or minutes), with the presentation order randomised and recorded.
- **A learned surrogate of the person,** used only when fatigue exceeds a threshold and the surrogate's held-out agreement exceeds another (AES-IGA's rule). The most uncertain candidates, not only the best, are sent back to the person.
- **Authority by kind of question:** a person's judgment has standing *verified* for preferences and value judgments they are entitled to make, and *proposed* for factual claims that a model or a check can verify.

#### 6.7 Combining Evaluators Without One Score

- **Tiers before trade-offs.** Lexicographic: (i) failures last; (ii) hard constraints from verified evaluators (programmatic checks, legal or budget limits, equity floors) by Deb's feasibility rule; (iii) Pareto dominance over the verified and computed objectives; (iv) estimated (surrogate) and proposed (LLM, unvalidated human) metrics only as tie-breakers, descriptors, or a second front shown beside the first.
- **Many objectives kept separate:** objectives keep their evaluator's identity, so a front can be read as "efficient on the CGE model, equitable on the ABM, acceptable to the panel". A dual verdict per candidate (a standing and a measure kept apart, kybernesis's convention) fits this exactly.
- **Quality-diversity over descriptors:** descriptors may come from any evaluator (per-case score signatures, equity position, an LLM's classification of the allocation's strategy); a MAP-Elites archive (Mouret & Clune 2015, arXiv:1504.04909 [M]) keeps the best of each kind for people to choose among.
- **Equity as its own objectives or constraints:** a dispersion index (Gini, Theil), the worst-off group's outcome (maximin) and a sufficiency share, computed from the per-unit outcomes the evaluator returns; its threshold, if a constraint, is declared and its source stated.
- **Robustness to evaluator error:** any candidate that wins only on proposed metrics is flagged; winners are re-evaluated at the highest fidelity with fresh streams before being reported; an evaluator's anomalous gains (large improvement in a proxy with none in the reference) trigger an audit.

*Remark.* heuresis's present `Evaluation` covers objectives, constraints, descriptors, cost and failures. The additions above (uncertainty and replicates, per-case values, fidelity, stream key, standing and provenance) and the policies (resampling, racing, CRN, cascade, surrogate management, audit) are what its planned `noise` module and the infrastructure's edges (evaluators that call simulators, language models or people) would need. Following heuresis's scope boundary, the record and the selection procedures belong in heuresis; evaluators that touch the world belong at the edges.

## Part VI. Agent Simulation of Economic Circulation: Money, Goods and Value among Households, Firms, Banks and Governments

*Remark.* Part VI's scope and conventions. Markers: **[V]** verified by reading the source itself (abstract, full text or the paper's PDF) during this research; **[S]** from a search-result snippet, bibliographic record or secondary summary only; **[M]** from memory, not checked during this research (details and DOIs marked [M] may be wrong and must be checked before citation; a link on an [M] claim is a pointer for checking, not evidence). Where the assignment named a source that could not be confirmed as described, this is said in place.

Scope: agent-based computational economics as it bears on circulation (who pays whom, how much is retained, how fast money turns over, where flows concentrate, how cycles and crises arise), the money-exchange and payment-network literatures, production networks as agents, calibration and inference, and learned rather than written behaviour. Order-dependent allocation in general is Part II's subject and the evaluator interface (replications, noise, language-model judges) Part V's; they are referred to, not repeated.

### 1. What Agent Simulation Reveals about Circulation that Aggregate Accounting Does Not

#### Takeaway
Aggregate accounting (input–output tables, social accounting matrices, LM3) records how much flows between sectors in a period under fixed coefficients; it does not represent the timing, queues, balance-sheet constraints, network topology or behavioural rules through which flows happen. Agent simulations add those, and the strongest documented gains are four: (a) shock propagation through production networks depends on the form of the production function and on inventories, so that static multipliers are "only mildly useful" for predicting impacts (Pichler et al. 2022, a real-time UK forecast within 0.6 percentage points of the outturn); (b) circulation lives in cycles, which aggregate tables cannot see but transaction networks show (Sarafu, Sardex, Fedwire's strongly connected core); (c) distribution and concentration emerge from exchange even among identical agents (kinetic exchange models; macro ABMs where heterogeneity grows from homogeneous starts); and (d) macro ABMs produce endogenous cycles, regime switches and tipping points that linearised representative-agent models exclude by construction. The evidence is strongest for (a) and (b), which rest on data; (c) and (d) are mostly demonstrations that a mechanism suffices to generate stylised facts.

#### Cited Findings

**1a. Macro agent-based models: cycles, fragility and distribution**
- **The Keynes+Schumpeter (K+S) family.** Dosi, Fagiolo and Roventini (2010), "Schumpeter meeting Keynes: A policy-friendly model of endogenous growth and business cycles", *JEDC* 34(9):1748–1767, bridge Keynesian demand generation and Schumpeterian technology-driven growth in one agent-based model; they report "profound complementarities" between demand factors and drivers of technical change, complementarity between Keynesian and Schumpeterian policies (milder fluctuations, lower unemployment) and two distinct growth "regimes" depending on whether innovation and demand conditions match [S] — [EconPapers record](https://econpapers.repec.org/RePEc:eee:dyncon:v:34:y:2010:i:9:p:1748-1767); [LEM WP 2008/21](https://www.lem.sssup.it/WPLem/files/2008-21.pdf). DOI 10.1016/j.jedc.2010.06.018 [M].
- **Distribution feeds back on circulation.** In the K+S model with credit, more unequal income distributions produce higher volatility, higher unemployment and more frequent crises; fiscal policy dampens this (Dosi, Fagiolo, Napoletano and Roventini 2013, "Income distribution, credit and fiscal policies in an agent-based Keynesian model", *JEDC* 37(8):1598–1625) [M] — [pointer](https://doi.org/10.1016/j.jedc.2013.05.001).
- **Tipping points as phase transitions.** Gualdi, Tarzia, Zamponi and Bouchaud (2015), "Tipping points in macroeconomic agent-based models", *JEDC* 50:29–61, characterise models by their phase diagram in parameter space; starting from the large fluctuations of Delli Gatti and colleagues' "Mark I" model, they find a generic phase transition between a "good economy" with low unemployment and a regime where unemployment is high, and introduce the minimal hybrid "Mark-0" (firms and households, no banks) [S] — [arXiv:1307.5319](https://arxiv.org/abs/1307.5319).
- **The CATS family.** Delli Gatti, Desiderio, Gaffeo, Cirillo and Gallegati, *Macroeconomics from the Bottom-up* (Springer 2011), and Delli Gatti et al. (2010), "The financial accelerator in an evolving credit network", *JEDC* 34(9):1627–1650, in which bankruptcies propagate through bank–firm credit links as avalanches; Assenza, Delli Gatti and Grazzini (2015), *JEDC* 50:5–28, add capital and credit [M] — [pointer](https://doi.org/10.1016/j.jedc.2010.06.019).
- **EURACE.** A large agent-based model of the European economy built for massively parallel simulation (Deissenberg, van der Hoog and Dawid 2008, *Applied Mathematics and Computation* 204(2):541–552); Cincotti, Raberto and Teglio (2010) show credit money and bank leverage generating endogenous instability in it; Eurace@Unibi (Dawid and colleagues) is its reproducible descendant [M] — [pointer](https://doi.org/10.1016/j.amc.2008.05.116).
- **AB-SFC as a benchmark for circulation with banks.** Caiani, Godin, Caverzasi, Gallegati, Kinsella and Stiglitz (2016), *JEDC* 69:375–408, DOI 10.1016/j.jedc.2016.06.001, start from what the 2008 crisis exposed in DSGE models and build a "fully decentralized" AB-SFC model of households, consumption firms, capital firms, banks, a government and a central bank. The model reproduces macro stylised facts (exponential nominal growth with moderate inflation, about 1.6–1.8% a year in the quasi steady state; pro-cyclical and lagging inflation; counter-cyclical and lagging mark-ups) and micro ones; heterogeneity among agents "emerges during the simulation" from perfectly homogeneous initial conditions through stochastic interaction and path-dependent competition [V] — [paper PDF (Columbia)](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf); [EconPapers](https://econpapers.repec.org/RePEc:eee:dyncon:v:69:y:2016:i:c:p:375-408).
- **Why representative-agent models miss this, in Caiani et al.'s own argument.** Log-linearisation around a steady state "forcibly imposed a stability condition" that rules out multiple equilibria, sudden transitions and tipping points; DSGE models neglect that banks create money by expanding their balance sheets, which "prevents to understand the causes of financial instability"; and they misrepresent outside money: absent cash advances, reserves and paper money held privately must equal the government bonds bought by the central bank [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **Forecasting parity with DSGE.** Poledna, Miess, Hommes and Rabitsch (2023), "Economic forecasting with an agent-based model", *European Economic Review* 151:104306, DOI 10.1016/j.euroecorev.2022.104306: an ABM covering all economic activities classified by ESA 2010, all sectors populated with millions of heterogeneous agents, calibrated to Austria from national accounts, input–output tables and business demography, "can compete with" VAR and DSGE benchmarks in out-of-sample forecasts and gives sector-level forecasts; applied to Austria's COVID-19 lockdowns [S] — [IIASA PURE](https://pure.iiasa.ac.at/18339); [UvA DARE](https://dare.uva.nl/id/1f799e58-e821-4db5-99b1-199771059634). The 2019–2020 working-paper versions claimed it could "in the long run significantly outperform" the benchmarks; the published version softens this to "compete with" [S] — [IIASA WP 20-001 record](https://pure.iiasa.ac.at/16268).

**1b. Money exchange as statistical mechanics, and the velocity of money**
- **Conservation gives the Gibbs law.** Drăgulescu and Yakovenko (2000), "Statistical mechanics of money", *Eur. Phys. J. B* 17:723, DOI 10.1007/s100510070114: "In a closed economic system, money is conserved"; by analogy with energy, the equilibrium distribution of money "must follow the exponential Gibbs law", with an effective temperature equal to the average money per agent; they show it emerging in simulations, discuss debt, a "thermal machine" extracting profit from a temperature difference, and models with broken time-reversal symmetry for which the Gibbs law does not hold [V] — [arXiv:cond-mat/0001432](https://arxiv.org/abs/cond-mat/0001432).
- **Review.** Yakovenko and Rosser (2009), "Colloquium: Statistical mechanics of money, wealth, and income", *Rev. Mod. Phys.* 81:1703, DOI 10.1103/RevModPhys.81.1703: the probability distribution of money is exponential for certain classes of models with interacting agents; data show a two-class structure, the majority following an exponential ("thermal") distribution that is stable in time and a small upper class following a power law ("superthermal") that is dynamic and out of equilibrium [V] — [arXiv:0905.1518](https://arxiv.org/abs/0905.1518).
- **What changes the distribution.** A saving propensity turns the exponential into a gamma-like law (Chakraborti and Chakrabarti 2000); allowing debt down to a limit lets balances go negative and raises the effective temperature, so that debt increases inequality; required reserve ratios change both the distribution and velocity (Xi, Ding and Wang 2005) [M] — [RMP review as pointer](https://doi.org/10.1103/RevModPhys.81.1703).
- **Velocity as reciprocal holding time.** Wang, Ding and Zhang (2003), "The circulation of money and holding time distribution", *Physica A* 324:665–677, DOI 10.1016/S0378-4371(03)00074-8, give velocity an explicit statistical formulation through the holding time of money; modelling circulation as a Poisson process gives a gamma-form holding-time distribution, and velocity depends on agents' share of money offered for exchange and on their number [S] — [arXiv:physics/0507147](https://arxiv.org/abs/physics/0507147). Wang and Qiu (2005), *Physica A* 353, express velocity as the reciprocal of the average holding time, the interval between exchanges for one unit of money, in a life-cycle model [S] — [arXiv:physics/0507159](https://arxiv.org/abs/physics/0507159).

**1c. Payment networks and liquidity**
- **Fedwire's topology.** Soramäki, Bech, Arnold, Glass and Beyeler (2007), "The topology of interbank payment flows", *Physica A* 379(1):317–333, DOI 10.1016/j.physa.2006.11.093 (NY Fed Staff Report 243, 2006). From 62 daily networks of commercial-bank transfers (the text says the first quarter of 2004; Table II's caption says the fourth quarter of 2004, an inconsistency in the report): average daily value $1.3 trillion in about 345,000 payments (peak day 644,000 payments, over $1.6 trillion); mean payment $3 million, median $30,000; first day about 6,600 nodes and 70,000 links; 75% of value carried by 66 nodes and 181 links, 25 of which form a clique to which the rest connect; 90% of value within the giant strongly connected component; connectivity about 0.3%, reciprocity about 21.5%, degree exponents γ_out ≈ 2.11 and γ_in ≈ 2.15; disassortative, small-world, highly clustered; topology "significantly altered" by the attacks of 11 September 2001 [V] — [NY Fed Staff Report 243 (PDF)](https://www.newyorkfed.org/medialibrary/media/research/staff_reports/sr243.pdf).
- **An agent-based RTGS.** Galbiati and Soramäki (2011), "An agent-based model of payment systems", *JEDC* 35(6):859–875 (BoE Working Paper 352, 2008): a multi-agent, multi-period simulation of real-time gross settlement in which banks choose how much costly liquidity to post; when a bank's liquidity runs out its payments queue until liquidity arrives from others, imposing delay costs; the model derives a demand curve for liquidity against delay costs [S] — [BoE WP 352](https://www.bankofengland.co.uk/working-paper/2008/an-agent-based-model-of-payment-systems); [EconPapers](https://econpapers.repec.org/RePEc:eee:dyncon:v:35:y:2011:i:6:p:859-875).
- **Liquidity-saving mechanisms.** In Galbiati and Soramäki's two-stream model (an RTGS stream and an LSM queue that offsets payments), banks choose liquidity and routing; with expensive liquidity the two-stream system is more efficient than plain RTGS; with cheap liquidity the second stream adds nothing because banks ignore it [S] — [chapter record](https://www.igi-global.com/chapter/liquidity-saving-mechanisms-and-bank-behavior-in-payment-systems/115355/). Martin and McAndrews (2008), "Liquidity-saving mechanisms", *J. Monetary Economics* 55(3):554–567, give the game-theoretic analysis [M] — [pointer](https://doi.org/10.1016/j.jmoneco.2008.01.008).
- **Congestion and cascades.** Beyeler, Glass, Bech and Soramäki (2007), "Congestion and cascades in payment systems", *Physica A* 384(2):693–718 [S] (bibliographic record) — [EconPapers](https://econpapers.repec.org/RePEc:eee:phsmap:v:384:y:2007:i:2:p:693-718). Its finding that low liquidity couples settlement to network topology, so that payments settle in correlated cascades whose sizes are broadly distributed, is [M].

**1d. Community and complementary currencies: circulation requires cycles**
- **Sarafu (Kenya).** Mattsson, Criscione and Takes (2023), "Circulation of a digital community currency", *Scientific Reports* 13:5864, DOI 10.1038/s41598-023-33184-1, propose a network method for studying circulation from digital transaction records and apply it to Sarafu during COVID-19 disruption, as a monetary flow network among about 40,000 users. Circulation was "highly modular, geographically localized" and among users of diverse livelihoods; within localised sub-populations cycle analysis "supports the idea that circulation requires cycles"; the sub-networks are consistently degree disassortative with evidence of preferential attachment; centrality points to early adopters, women's participation and community-based institutions as local hubs [V] — [arXiv:2207.08941](https://arxiv.org/abs/2207.08941v2). The period (January 2020 to June 2021) and the volume (293.7 million Sarafu) are [S] — [CEU record](https://research.ceu.edu/en/publications/circulation-of-a-digital-community-currency/).
- **Topological components.** Criscione (2024, rev. 2025), "Topological components in a community currency network": strongly connected components of several types are far more frequent than in randomised null models, so cycles play a key role in recirculation; some acyclic components look like collection from accounts active only once (possible misuse), others like one-off trials [V] — [arXiv:2409.13674](https://arxiv.org/abs/2409.13674). A search summary adds that the network's structure changed after 2021 [S]; the abstract read does not say so.
- **The assignment's "Mattsson, Criscione and Ruiz".** No Sarafu paper with an author Ruiz was found. The open dataset paper is, from memory, Mattsson, Criscione and Ruddick (2022), "Sarafu Community Inclusion Currency, 2020–2021", *Scientific Data* 9:426 (Will Ruddick of Grassroots Economics) [M] — [pointer](https://doi.org/10.1038/s41597-022-01539-4).
- **Sardex (Sardinia).** Iosifidis, Charette, Airoldi, Littera, Tassiulas and Christakis (2018), "Cyclic motifs in the Sardex monetary network", *Nature Human Behaviour* 2:822–829, DOI 10.1038/s41562-018-0450-0: 1,477 businesses and 48,170 transactions; with measures of "cyclic density" and "k-cycle centrality", geodesic transaction cycles are prevalent, and firms taking part in them show higher turnover and lower excess currency and debt accumulation [S] — [IDEAS](https://ideas.repec.org/a/nat/nathum/v2y2018i11d10.1038_s41562-018-0450-0.html); [author PDF](https://humannaturelab.net/sites/default/files/pdf/paper/Cyclic-motifs-in-the-Sardex-monetary-network.pdf). A 2025 *Scientific Reports* study extends cycle-motif participation to Honduran village networks, associating it with individual and collective wealth [S] — [TU Delft repository](https://repository.tudelft.nl/record/uuid:8dc8b8dc-9b10-45e1-b501-4249d70c738a).
- **WIR (Switzerland).** Stodder (2009), "Complementary credit networks and macroeconomic stability: Switzerland's Wirtschaftsring", *JEBO* 72(1):79–95, finds WIR turnover counter-cyclical (rising when national money is scarce); Stodder and Lietaer (2016), *Comparative Economic Studies* 58:570–605, extend this to balances, velocity and leverage [M] — [pointer](https://doi.org/10.1016/j.jebo.2009.06.002).

**1e. Local economies, LM3 and leakage**
- **LM3** (New Economics Foundation; Sacks 2002, *The Money Trail*) follows income through three rounds of local spending, scored from surveys of recipients' and suppliers' re-spending; its "leaky bucket" framing holds that stopping money leaking out matters more than pouring more in; it was refined with all 26 local authorities of North East England [S] — [NEF Consulting](https://www.nefconsulting.com/training-capacity-building/local-multiplier-3/); [CCEDNet](https://ccednet-rcdec.ca/resource/the-money-trail-measuring-your-impact-on-the-local-economy-using-lm3/).
- **Simulation of local multipliers.** Hoekstra, van Arkel and Leurs (2007), "Modeling local monetary flows in poor regions", *INDECS* 5(2):138–150, propose a multi-agent model of household groups whose spending priorities come from a social accounting matrix of the local economy, to simulate the multiplier; it is a research design, not a reported result [S] — [IDEAS](https://ideas.repec.org/a/zna/indecs/v5y2007i2p138-150.html). No published agent-based model built directly on LM3 or on community wealth building (the Preston model) was found in this research [S] (absence in search).

**1f. Production networks and supply chains as agents**
- **Theory of propagation.** Acemoglu, Carvalho, Ozdaglar and Tahbaz-Salehi (2012), "The network origins of aggregate fluctuations", *Econometrica* 80(5):1977–2016, DOI 10.3982/ECTA9623: when the input–output network is asymmetric (some sectors supply many), idiosyncratic sectoral shocks decay more slowly than 1/√n and can drive aggregate fluctuations [M] — [pointer](https://doi.org/10.3982/ECTA9623). Carvalho and Tahbaz-Salehi (2019), "Production networks: A primer", *Annual Review of Economics* 11:635–663, DOI 10.1146/annurev-economics-080218-030212, review input–output linkages as a channel through which micro shocks become aggregate fluctuations, with the empirical and quantitative work [S] — [EconPapers](https://econpapers.repec.org/RePEc:anr:reveco:v:11:y:2019:p:635-663); [author PDF](https://www.kellogg.northwestern.edu/faculty/alirezat/ProductionNetworksPrimer.pdf).
- **A dynamic input–output model with agent-like adjustment.** Pichler, Pangallo, del Rio-Chanona, Lafond and Farmer (2022), "Forecasting the propagation of pandemic shocks with a dynamic input–output model", *JEDC* 144:104527, DOI 10.1016/j.jedc.2022.104527 (open access): industries rest in a steady state matching national accounts and input–output tables until shocked; a "partially binding Leontief" production function separates critical from non-critical inputs (from a survey of industry analysts), inventories buffer shocks, and hiring is sluggish. In real time it forecast a 21.5% fall in UK GDP in 2020 Q2 against Q4 2019; the ONS later estimated 22.1% (the Bank of England had forecast about 30%; the median of several institutions about 16.6%). Under pure Leontief, gross output falls up to 60%; the linear production function "would have underestimated the UK recession by several percentage points"; the size-weighted sectoral correlation of predicted with actual output is 0.75; static network measures only partly explain the simulated impacts [V] — [PMC full text](https://pmc.ncbi.nlm.nih.gov/articles/PMC9472492). Its companion paper reports that output multipliers are "only mildly useful" for predicting shock impacts a priori [S] — [arXiv:2102.09608](https://arxiv.org/pdf/2102.09608).
- **Firm-level supply chains.** Inoue and Todo (2019), "Firm-level propagation of shocks through supply-chain networks", *Nature Sustainability* 2(9):841–847, DOI 10.1038/s41893-019-0351-x: an agent-based model after Henriet et al. (2012) on the actual supply chains of about one million Japanese firms, parameters estimated on the Great East Japan earthquake; the scale-free degree distribution speeds propagation, dense links within communities slow it, and harder substitution of damaged suppliers makes propagation much faster [S] — [IDEAS](https://ideas.repec.org/a/nat/natsus/v2y2019i9d10.1038_s41893-019-0351-x.html). The authors' slides put direct production losses at about ¥100 billion against about ¥11 trillion from propagation [S] — [NBER slides](https://conference.nber.org/conf_papers/f214012/f214012.slides.pdf).
- **Review.** Axtell and Farmer (2025), "Agent-based modeling in economics and finance: past, present, and future", *Journal of Economic Literature* 63(1):197–287 [M] — [pointer](https://doi.org/10.1257/jel.20221319).

#### Inferences
- **What aggregate accounting cannot show, with the evidence for each:**
  - *timing and liquidity* (payment ABMs: the same daily flows settle or queue depending on posted liquidity and routing);
  - *non-linearity of production* (Pichler: the choice between Leontief, linear and partially binding Leontief moves a quarterly shock from about 60% to a few points, and static multipliers predict poorly);
  - *cycles* (Sarafu, Sardex: circulation and firm performance are tied to cycles, which a sector-by-sector table aggregates away);
  - *concentration and its dynamics* (Fedwire's 25-bank clique; the exponential bulk and power-law tail of money);
  - *regime and phase* (K+S regimes, Mark-0's transition).
- **The Hodge decomposition already held in the infrastructure measures the part of a flow network that Sarafu and Sardex show matters.** The curl and harmonic parts are the circulating component. The literature measures cycles combinatorially (cycle participation, strongly connected components, cyclic density), not by Hodge projections. Reporting both on the same record would let the two notions of circulation be compared. This is an inference, not a published result.
- **Velocity is better treated as a distribution than a ratio.** The holding-time distribution (Wang et al.) and the strongly connected core (Fedwire: 90% of value) suggest reporting velocity per component and per class of agent, not only as GDP over money.
- **LM3 is a truncated walk.** Read as a Markov chain of money, LM3 is the expected local spend in the first three steps, and leakage is the absorption probability into "outside". An agent simulation can give the whole absorption profile (retention after k rounds; expected local rounds before leakage), and also its dependence on who receives the money first, which a survey-based LM3 cannot. This is an inference; no paper doing it was found.
- **The strongest evidence that simulation adds forecasting value comes from models anchored on accounts.** Pichler et al. and Poledna et al. both start from a steady state matching national accounts and input–output tables. Freely specified models mostly offer existence proofs of mechanisms.

#### Gaps
- **Velocity measured in a calibrated macro ABM.** No source read compares a velocity or holding-time distribution measured in such a model with transaction data; the econophysics results are for stylised exchange models.
- **Community wealth building.** No agent-based or simulation study of the Preston model or similar programmes was found, nor any validation of LM3 against transaction records.
- **Full texts not read.** The full texts of the payment-system ABMs (Galbiati and Soramäki; Beyeler et al.), the WIR studies and Inoue and Todo were not read; their findings above rest on records and summaries.
- **Comparisons with representative-agent models.** Head-to-head comparisons on the same data, beyond Poledna et al. (forecasts) and the K+S literature's stylised-fact matching, were not found in this research.

### 2. Stock–Flow Consistency and the Conservation of Money in Agent Models

#### Takeaway
In an agent model, stock–flow consistency is enforced by giving every agent a balance sheet. Every transaction is booked as four entries (Copeland's quadruple entry): each payment is a flow out of one agent and into another, each matched by changes in an asset and a liability. The aggregate transaction-flow matrix (rows and columns summing to zero) and the balance-sheet matrix (each financial asset someone's liability) then hold by construction, not by calibration. It matters because money is created and destroyed only by identifiable acts: lending, repayment, government spending and taxation, central-bank purchases. A model that lets money appear (a new firm's deposits from nowhere) or vanish (a bankrupt's debt simply deleted) creates "financial black holes" that bias credit, consumption and investment by an amount that is hard to evaluate. Kinetic exchange models are the limiting case: with no money creation, conservation alone fixes the equilibrium distribution.

#### Cited Findings
- **The principle.** Godley and Lavoie's SFC approach uses social accounting matrices "to ensure that every flow of payments comes from somewhere and goes somewhere and that every financial stocks is recorded as a liability for someone and an asset for someone, so that there are no financial black holes in the model"; Caiani et al. list three advantages: it suits modelling endogenous and government money, it accounts for the interrelation of agents' balance sheets, and its accounting logic disciplines model building [V] — [Caiani et al. 2016, PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf). The book is Godley and Lavoie (2007), *Monetary Economics: An Integrated Approach to Credit, Money, Income, Production and Wealth*, Palgrave Macmillan [M].
- **At the agent level.** Caiani et al. combine agent-based modelling with SFC accounting "based on the quadruple entry principle developed by Copeland (1949)", with "accounting consistency … implemented at very bottom level", so that micro flows build up into the economy's transaction-flow matrix. Stock–flow consistency implies "that the rows and columns of the Transaction Flow Matrix sum to 0" (Godley and Lavoie 2007), and the matrix's lower section (changes in financial assets) integrates it with the balance-sheet matrix [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **Where consistency usually breaks: entry, exit and initial stocks.** Caiani et al. show that endowing a new firm with deposits not obtained by a transfer from another agent breaks the quadruple-entry principle at the aggregate level, with consequences for banks' liquidity, cash advances from the central bank, wages and investment that are "difficult to evaluate". They call the initial stock–flow calibration of AB-SFC models a "major black hole in the literature". Their procedure: solve an aggregate SFC steady state (desired inflation 0.0075 a period, 8% unemployment, wage 5, capital stock 40,000); distribute each sector's stocks across agents symmetrically (8,000 households, 100 consumption firms, 20 capital firms, 10 banks, 1,360 public servants); and let heterogeneity emerge [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **Why ABMs without it are unreliable.** Caiani et al. argue that ABMs lacking integrated accounting risk inconsistencies in agents' balance sheets that greatly undermine the reliability of results. They argue conversely that traditional SFC models are too aggregated (households, banks, firms, the public sector) to track intra-sectoral flows or heterogeneity [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **Money as created by credit.** Caiani et al. stress that banks "can create money instantaneously by expanding their balance sheet", limited only by their own assessment of profitability and solvency, and that neglecting this "induces to dramatically underestimate" the consequences of financial instability [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **The closed-economy limit.** In kinetic exchange models money is conserved in each exchange, which fixes the stationary exponential distribution; models with broken time-reversal symmetry escape the Gibbs law [V] — [Drăgulescu and Yakovenko 2000](https://arxiv.org/abs/cond-mat/0001432). With debt, the conserved quantity is money minus debt and balances can go negative [M] — [Yakovenko and Rosser 2009](https://doi.org/10.1103/RevModPhys.81.1703).
- **SFC accounting in a forecasting ABM.** Poledna et al.'s Austrian model covers "all economic activities as classified by the European System of Accounts (ESA 2010)", initialised from national accounts and input–output tables [S] — [IIASA record](https://pure.iiasa.ac.at/18339). Whether it enforces a full transaction-flow matrix at agent level was not verified.
- **Tooling.** sfctools (Baldauf 2023, *JOSS* 8(87):4980) is a Python toolbox for SFC agent-based models with balance sheets and transaction-flow matrices built from agents' bookings [M]. Caiani et al.'s model code (JMAB, Java) is at github.com/S120/benchmark [V] (URL stated in the paper).

#### Inferences
- **Conservation is an invariant to check every step, not a property to hope for.**
  - *Checks.* The sum of all agents' net financial assets is zero, the real stocks being separate. Each transaction-flow row and column sums to zero. Each agent's change in net worth equals its saving plus revaluations. Money (deposits plus currency plus reserves) changes only by recorded creating and destroying acts.
  - *Purpose.* These are exactly checkable conditions. They suit a C++ core with checks, and they are the economic analogue of the conservation tests a balance-law module already makes.
- **The entry, exit and default rules are where allocation and conservation meet.** An allocator that grants money or goods to agents must record the counter-entry (the government's deficit, a loan, a transfer from a fund). Otherwise an evolutionary search will exploit the leak: candidates that "allocate" unfunded money will score best. This is the same kind of exploit as the AI Economist's tax gaming (§4).
- **The flows to decompose come from the accounts.** For a Hodge decomposition of the payment network, the flows should be the transaction-flow matrix's off-diagonal payments between agents (or between agent classes), with creation and destruction as edges to and from a distinguished node (a bank, the government, "outside"). Treating those as sources and sinks keeps the decomposition meaningful when money is not conserved among the remaining agents.

#### Gaps
- **Formal comparisons.** No published comparison was found that quantifies how much results change when an ABM's accounting leaks (for example, the same model with and without consistent entry).
- **Design choices in other models.** The exact SFC enforcement in EURACE, CATS and the Austrian ABM was not verified in this research.

### 3. Calibration, Validation and Inference for Economic Agent-Based Models

#### Takeaway
Economic ABMs are still most often calibrated by matching moments: simulated minimum distance, method of simulated moments, indirect inference. They are validated by reproducing lists of stylised facts. Since about 2017 three changes stand out:
- **Bayesian estimation:** likelihood-based Bayesian estimation, even with simple density estimates, consistently beat simulated minimum distance methods in Platt's (2020) comparison, though it degraded for large models with many free parameters.
- **Surrogates:** machine-learning surrogates (Lamperti, Roventini and Sani 2018) cut the cost of exploring parameter space.
- **Neural simulation-based inference:** neural posterior and ratio estimation (Dyer, Cannon, Farmer and Schmon 2024) recovers approximate posteriors with simulation budgets "orders of magnitude smaller", and its amortisation allows simulation-based calibration checks of the estimator itself.

Validation remains the weaker side. Fagiolo et al. (2019) frame it in three dimensions (comparing artificial with real data, calibrating and estimating, exploring parameter space), but reproducing stylised facts does not identify mechanisms.

#### Cited Findings
- **The validation framework.** Fagiolo, Guerini, Lamperti, Moneta and Roventini (2019), "Validation of agent-based models in economics and finance", in *Computer Simulation Validation* (Springer), pp. 763–787, DOI 10.1007/978-3-319-70766-2_31, review the literature since Windrum et al. (2007) under three dimensions: comparison of artificial and real-world data; calibration and estimation of parameters; parameter-space exploration [S] — [EconPapers](https://econpapers.repec.org/RePEc:hal:journl:halshs-02375423); [LEM WP 2017/23](https://www.lem.sssup.it/WPLem/files/2017-23.pdf).
- **Causal validation.** Guerini and Moneta (2017), "A method for agent-based models validation", *JEDC* 82:125–141, compare structural VARs identified on real and artificial data, testing whether the model reproduces causal structure and not only moments [S] (existence) — [INET WP 42](https://www.ineteconomics.org/uploads/papers/WP_42-Moneta.pdf); content [M].
- **A benchmark of calibration methods.** Platt (2020), "A comparison of economic agent-based model calibration methods", *JEDC* 113:103859, DOI 10.1016/j.jedc.2020.103859, notes that new methods are rarely benchmarked against old ones. In computational experiments, a simple likelihood-based approach to Bayesian estimation consistently outperformed several simulated minimum distance methods and gave reasonable estimates in many contexts; performance degraded mainly for a large-scale model with many parameters [S] — [arXiv:1902.05938](https://arxiv.org/abs/1902.05938); [IDEAS](https://ideas.repec.org/a/eee/dyncon/v113y2020ics0165188920300294.html).
- **Bayesian estimation of ABMs.** Grazzini, Richiardi and Tsionas (2017), "Bayesian estimation of agent-based models", *JEDC* 77:26–47, use kernel density estimates of the likelihood from simulated data; Delli Gatti and Grazzini (2020), *JEBO* 178:875–902, apply Bayesian estimation and forecasting to a macro ABM [M] — [pointer](https://doi.org/10.1016/j.jedc.2017.01.014).
- **Surrogates.** Lamperti, Roventini and Sani (2018), "Agent-based model calibration using machine learning surrogates", *JEDC* 90:366–389 (arXiv:1703.10639): an iterative active-learning surrogate approximates the map from parameters and initial conditions to outputs from a limited number of runs, combining supervised learning with intelligent sampling. On Brock and Hommes's asset-pricing model and the "Islands" growth model it is a fairly accurate proxy that substantially cuts computation for exploration and calibration [S] — [EconPapers](https://econpapers.repec.org/RePEc:eee:dyncon:v:90:y:2018:i:c:p:366-389); [ar5iv](https://ar5iv.arxiv.org/html/1703.10639). That the surrogate is gradient-boosted trees (XGBoost) classifying parameter points as meeting a calibration criterion or not is [M].
- **Neural simulation-based inference.** Dyer, Cannon, Farmer and Schmon (2024), "Black-box Bayesian inference for agent-based models", *JEDC* 161:104827, DOI 10.1016/j.jedc.2024.104827 (arXiv:2202.00625):
  - they test neural posterior estimation and neural density-ratio estimation on economic ABMs, motivated by older methods' restrictive assumptions or their need for "many hundreds of thousands of simulations";
  - the neural methods give state-of-the-art inference, with superior recovery of approximate ground-truth posteriors at simulation budgets "orders of magnitude smaller";
  - because the estimators are amortised, simulation-based calibration can verify their accuracy;
  - they propose criteria for benchmarking such procedures.

  [S] — [arXiv:2202.00625](https://arxiv.org/abs/2202.00625v1); [INET Oxford](https://www.inet.ox.ac.uk/publications/black-box-bayesian-inference-for-agent-based-models).
- **Learning latent micro-states, not only parameters.** Monti, Pangallo, De Francisci Morales and Bonchi (2023), *Scientific Reports*, "On learning agent-based models from data": most efforts calibrate parameters by matching summary moments (in housing, transaction prices), leaving agent-level variables such as income uncalibrated. They estimate latent micro-variables of a housing ABM by gradient-based expectation maximisation; on synthetic tests the correlation between true and learned traces is 0.5–0.9 depending on the setting [S] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC10247821/); [arXiv:2205.05052](https://ar5iv.labs.arxiv.org/html/2205.05052).
- **Calibration from accounts, not estimation.** Pichler et al. calibrate to a steady state matching national accounts and input–output tables and estimate the production function's critical inputs from an analyst survey; their forecast was real-time [V] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC9472492). Poledna et al. calibrate from national accounts, input–output tables and business demography [S] — [IIASA](https://pure.iiasa.ac.at/18339).
- **A toolkit.** Black-it (Bank of Italy; Benedetti and colleagues), "a ready-to-use and easy-to-extend calibration kit for agent-based models", *JOSS* 7(79):4622 (2022), AGPL-3.0, latest PyPI release 0.3.1 [S] — [docs](https://bancaditalia.github.io/black-it); [PyPI](https://pypi.org/project/black-it).

#### Inferences
- **A calibration ladder that matches the infrastructure's foundations:**
  - accounts first, as Pichler and Poledna do: the initial balance sheets and flows from national accounts, an input–output table or a payment record, as a consistent SFC steady state;
  - behavioural parameters by Bayesian or neural simulation-based inference, checked by simulation-based calibration (Dyer et al.) or by posterior predictive checks on held-out moments;
  - surrogates (Lamperti et al.) for sweeps and for evolutionary search, never as the reported answer.
- **What a run supports should be stated, as for any declared parameter.** Platt's finding that estimation degrades with many free parameters argues for declaring most behavioural parameters (with ranges) and estimating only the few the record identifies. This mirrors the infrastructure's practice of saying what a record does not identify.
- **Validation should test the circulation measures themselves, against held-out transaction records.** Examples: holding-time distributions, cycle share, retention after k rounds, the concentration of value in a strongly connected core. Matching GDP-type aggregates alone is not enough. Sarafu's open dataset and published payment-network statistics make this possible.

#### Gaps
- **Neural SBI on large models.** No source read applies neural SBI to a large SFC macro ABM such as K+S or the Austrian model; Dyer et al.'s experiments are on small and medium models (which ones was not verified).
- **Measured circulation as a target.** No study was found that validates an ABM against measured circulation statistics (velocity distributions, cycle structure) of a payment or community-currency record.
- **Sensitivity analysis.** Variance-based global sensitivity analysis in large macro ABMs (Sobol' indices on surrogates) is known to be practised, but no specific source was read in this research [M].

### 4. The Fidelity of Learned Agents (Reinforcement Learning and Language Models) as Stand-ins for Households and Firms

#### Takeaway
Reinforcement-learning agents are useful for finding what a rule-following population would do under a policy, and for exposing gaming. The AI Economist's planner and agents co-adapt, and the agents learn tax-avoidance strategies. Their behaviour is optimal for the reward written, not human. Language-model agents ("Homo silicus", EconAgent, generative agents of real people) reproduce many qualitative findings and individual survey answers well; the best case is 72.8% accuracy on General Social Survey items against people's own 79.5% two-week test-retest agreement. But they are systematically more homogeneous than people, they are "hyper-rational" in incentivised economic games, and grounding them in rich interviews does not cure that. They may also regurgitate memorised results. The recommended safeguards are micro-level validation against held-out human data, explicit correction of their bias, and treating their outputs as proposals until checked.

#### Cited Findings
- **The AI Economist.** Zheng, Trott, Srinivasa, Parkes and Socher, "The AI Economist: Optimal economic policy design via two-level deep reinforcement learning" (arXiv:2108.02755, a substantial extension of arXiv:2004.13332):
  - agents and a social planner learn together, the agents' behaviour adapting to the planner's policy and the planner's policy to the agents;
  - in simple one-step economies it recovers the optimal tax policy of economic theory;
  - in complex dynamic economies it improves utilitarian welfare and the equality–productivity trade-off over baselines, even though agents develop tax-gaming strategies.

  [V] — [arXiv:2108.02755](https://arxiv.org/abs/2108.02755). The published version is Zheng et al. (2022), "The AI Economist: Taxation policy design via two-level deep multiagent reinforcement learning", *Science Advances* 8(18):eabk2607, DOI 10.1126/sciadv.abk2607. Its reported 16% improvement in the equality–productivity trade-off over the Saez framework, its experiments with human participants, and the open "Foundation" simulation code (github.com/salesforce/ai-economist) are all [M]: the journal page refused access and the arXiv abstract gives no figures.
- **ABIDES-Economist.** Dwarakanath, Balch and Vyetrenko (2024, rev. 2025), arXiv:2402.09563: an agent-based simulator with heterogeneous households, firms, a central bank and a government. Agents follow domain rules or learn by reinforcement learning, with RL integrated "for all agents using the OpenAI Gym environment framework". The authors validate by showing learned behaviour matches micro and macro stylised facts, then test central-bank and government policies that outperform rule-based baselines. The abstract page does not state whether the code is open [V] — [arXiv:2402.09563](https://arxiv.org/abs/2402.09563).
- **Deep RL for heterogeneous-agent general equilibrium.** Hill, Bardoscia and Turrell (2021), arXiv:2103.16977, and Curry, Trott, Phade, Bai and Zheng (2022) use multi-agent RL to solve or analyse micro-founded general-equilibrium models with many agents [M] — [pointer](https://arxiv.org/abs/2103.16977).
- **Homo silicus.** Horton, Filippas and Manning, "Large language models as simulated economic agents: what can we learn from Homo silicus?" (arXiv:2301.07543; v1 January 2023 by Horton, v2 February 2026 with Filippas and Manning; no journal reference listed). Language models are implicit computational models of humans to which endowments, information and preferences can be assigned. Experiments derived from several prior studies give qualitatively similar results; where they differ, the differences suggest directions for research [V] — [arXiv:2301.07543](https://arxiv.org/abs/2301.07543). The paper also notes that some models show a strong status-quo bias and others barely any, that fidelity can be adjusted by prompting, and that models may "regurgitate problematic and memorized results" [S] — [arXiv HTML v2](https://arxiv.org/html/2301.07543v2).
- **EconAgent.** Li, Gao, Li, Li and Liao (2024), "EconAgent: Large language model-empowered agents for simulating macroeconomic activities", ACL 2024 (arXiv:2310.10436). Rule-based or neural agents make heterogeneous household and firm behaviour hard to capture. EconAgent's language-model agents decide how much to work and to consume in a simulated market; a perception module gives them distinct decision traits and a memory module lets them reflect on their past and on market trends. The authors report more realistic decisions and more plausible macroeconomic patterns than rule-based or learning-based agents [V] — [arXiv:2310.10436](https://arxiv.org/abs/2310.10436).
- **Generative agents of real people, and how the claim changed.** Park et al., arXiv:2411.10109:
  - *Version 1* (November 2024, "Generative agent simulations of 1,000 people"; 1,052 people interviewed for two hours) reported that agents replicate General Social Survey answers "85% as accurately as participants replicate their own answers two weeks later", and reduce accuracy biases across racial and ideological groups relative to agents given demographic descriptions [S] — [arXiv v1](https://arxiv.org/abs/2411.10109v1); [Stanford HAI](https://hai.stanford.edu/policy/simulating-human-behavior-with-ai-agents).
  - *The current version* (v4, last revised 16 November 2026, retitled "AI simulations grounded in rich self-reports increase accuracy and reduce distortion") reports GSS accuracy of 60.4% from demographics, 69.4% from interviews, 69.3% from surveys and 72.8% from both, against participants' own 79.5% two-week test-retest agreement. Combined grounding closes a median 53.5% of eight "distortion gaps". A fine-tuned open-weight model reaches 75.2% and closes 69.3%. Neither approach reduced "hyper-rationality" in economic games, and grounding improved personality prediction but not incentivised economic choices [V] — [arXiv:2411.10109](https://arxiv.org/abs/2411.10109).
- **Homogeneity and validation failures (2025–2026 preprints, summaries only).**
  - LLM agents' macroeconomic expectations match the shape of human distributions but are more homogeneous (Lin, Sun and Yan).
  - A market experiment from the Bank of Canada and the University of Amsterdam finds less heterogeneity than among human participants, and bounded rather than strictly rational expectations, depending on parameters.
  - An ACL 2026 Findings paper finds failures of trend alignment and of heterogeneity when agents are checked against survey data, and argues that the absence of micro-level validation undermines such simulations.
  - An Amazon-affiliated paper proposes learning a map from raw LLM choices to human-aligned ones, because naive inference on synthetic outputs is unreliable.

  [S] — [arXiv:2505.17648](https://arxiv.org/abs/2505.17648v4); [ACL Findings 2026](https://aclanthology.org/2026.findings-acl.1799.pdf); [IDEAS 2508.18600](https://ideas.repec.org/p/arx/papers/2508.18600.html); [Agentic Economic Modeling, arXiv:2510.25743](https://arxiv.org/html/2510.25743v1). Which summary belongs to which paper was not checked against the full texts.
- **Earlier warnings.**
  - Aher, Arriaga and Kalai (2023, ICML) find a "hyper-accuracy distortion" in simulated wisdom-of-crowds tasks.
  - Santurkar et al. (2023, ICML) find that models' opinions align with particular demographic groups.
  - Bisbee et al. (2024, *Political Analysis*) find that synthetic survey responses have too little variance and are sensitive to prompts and model versions.
  - Wang, Morgenstern and Dickerson (2025, *Nature Machine Intelligence*) find that LLMs replacing participants misportray and flatten identity groups.

  [M] — [Bisbee et al. pointer](https://doi.org/10.1017/pan.2024.5).
- **Behaviour learned from data rather than written.** No paper was found that applies inverse reinforcement learning or imitation learning to household or firm transaction records to obtain the agents of an economic ABM. The nearest work is latent-variable learning for ABMs (Monti et al. 2023) and inverse optimisation of household activity (travel) models [S] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC10247821/); [inverse optimisation survey, arXiv:2109.03920](https://arxiv.org/pdf/2109.03920).

#### Inferences
- **The two kinds of learned agent fail in opposite ways.**
  - *RL agents* are too good at the reward. They find the leaks: tax gaming, and, by inference, unfunded money where conservation is not enforced. They are useful as adversaries that test an allocation rule's robustness.
  - *LLM agents* are too uniform and too rational in incentivised choices, and are unvalidated by default. Since circulation and its concentration depend on heterogeneity (who spends locally, who saves, who lends), homogeneous agents will tend to understate dispersion. That covers holding times, the inequality of balances and cascade sizes.
- **Safeguards, by the infrastructure's own grading.**
  - A language model's behaviour rule enters as a proposal (grade C). It is admitted only after micro-level validation against held-out human records (survey items, experimental choices, or transaction histories).
  - The share of agents whose behaviour is model-generated is reported with every result.
  - Results are re-run with rule-based and with RL agents, and only conclusions robust across the three kinds are reported as findings. (Part V treats language models as evaluators; here they are the simulated actors, a different role.)
- **Learning behaviour from transaction records is an open opportunity** (imitation learning or inverse RL on records such as Sarafu's). It needs care: records show what was done, not what was available, so the choice sets are unknown. This is a coarsening problem of the kind peira formalises.

#### Gaps
- **The AI Economist's figures.** The Science Advances paper's figures and human-participant results could not be verified (access refused).
- **LLM agents and money.** No evaluation was found of LLM agents' fidelity in money and payment behaviour specifically: spending timing, local versus distant purchases, saving under liquidity constraints.
- **IRL or imitation from transaction records** for economic agents was not found.

### 5. Open Models and Code to Build On

#### Takeaway
There is enough open code to avoid writing a macro ABM from nothing:
- **The Austrian ABM** in BeforeIT.jl (Julia, Bank of Italy; licence stated inconsistently);
- **Caiani et al.'s AB-SFC benchmark** in JMAB (Java);
- **black-it** for calibration;
- **Pichler et al.'s dynamic input–output model**, with replication files;
- **frameworks:** Mesa (Python ABM) and sfctools (SFC accounting for agents);
- **the AI Economist's Foundation**, and ABIDES-Economist's description;
- **data:** Sarafu's open transaction dataset.

None of them is in C++, and none combines SFC accounting, payment-level timing and circulation measures in one model.

#### Cited Findings
- **BeforeIT.jl** (Glielmo, Devetak, Meligrana and Poledna, arXiv:2502.13267, February 2025; "Behavioural economic forecasting by the IT research unit of the Bank of Italy"): open-source Julia software for macro ABMs, with a ready Austria setup (`BeforeIT.AUSTRIA2010Q1`); the base model is that of Poledna et al. It is reported to be orders of magnitude faster than the original Matlab implementation and comparable to a highly optimised HPC one. Code at github.com/bancaditalia/BeforeIT.jl. The licence is stated inconsistently: AGPL-3.0 in arXiv v1 and the JuliaHub docs, Apache 2.0 in a later PDF; check the repository's LICENSE [S] — [arXiv:2502.13267](https://arxiv.org/abs/2502.13267v1); [JuliaHub docs](https://docs.juliahub.com/General/BeforeIT/stable/).
- **Caiani et al.'s benchmark AB-SFC model**: code at github.com/S120/benchmark (stated in the paper); full equations in an SSRN appendix (abstract id 2664125) [V] — [paper PDF](https://business.columbia.edu/sites/default/files-efs/imce-uploads/Joseph_Stiglitz/Agent%20based-stock%20flow.pdf).
- **Pichler et al.'s dynamic input–output model**: source code, replication files and data in an online repository (the paper's footnote 6; URL not read) [V] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC9472492).
- **black-it** (Bank of Italy; JOSS 2022; AGPL-3.0) for ABM calibration [S] — [docs](https://bancaditalia.github.io/black-it).
- **The AI Economist's Foundation** (Salesforce, open-source Python), a two-level RL economic simulation [M] — [pointer](https://github.com/salesforce/ai-economist).
- **ABIDES-Economist**: its abstract page does not state whether the code is open [V] — [arXiv:2402.09563](https://arxiv.org/abs/2402.09563). It builds on ABIDES (Byrd, Hybinette and Balch 2020), an open market simulator [M].
- **Frameworks.**
  - Mesa: Python ABM, with Mesa 3 described in *JOSS* (2025).
  - sfctools: Python SFC agent toolbox, *JOSS* 8(87):4980 (2023).
  - pysolve, and sfcr (R), for aggregate SFC models.
  - LSD (Laboratory for Simulation Development), used for K+S models.
  - Eurace@Unibi: code released with Dawid et al. (2019), *J. Evolutionary Economics* 29:467–538.

  [M] — [Mesa pointer](https://github.com/projectmesa/mesa).
- **Data.**
  - The Sarafu transaction dataset (2020–2021), released with a *Scientific Data* paper (2022) [M].
  - Fedwire statistics in the NY Fed report [V] — [SR 243](https://www.newyorkfed.org/medialibrary/media/research/staff_reports/sr243.pdf).
  - Sardex data are not public as far as found [M].

#### Inferences
- **What to port, and in what order:**
  - *The SFC bookkeeping discipline (Caiani et al.) and the account-anchored initialisation (Pichler; Poledna/BeforeIT)* matter more than any particular behavioural rule. Port them first, as C++ checks over agents' balance sheets in the agent-modelling toolkit (stated schedules, draws by identity, event logs).
  - *BeforeIT.jl, black-it and Pichler's model* serve as reference implementations against which a port is held to their outputs, in the infrastructure's practice of reproducing reference libraries' numbers.
  - *The Sarafu dataset* is the natural first validation record for circulation measures (cycles, retention, holding times), given its documented modularity and cycle structure.

#### Gaps
- **Unverified details.** Licences and current maintenance of JMAB, Eurace@Unibi, the Foundation and ABIDES-Economist were not verified.
- **Missing code.** No open, maintained C++ macro ABM with SFC accounting was found. No open payment-system ABM code was found (central banks' simulators, such as the Bank of Finland's, are distributed under their own terms) [M].

### 6. What This Means for the Infrastructure

The proposals below are inferences from the findings above. They are set against what the infrastructure holds:
- nomos's `agents`: stated schedules, Gillespie simulation, draws by identity, event logs;
- economics' `circulation`: flows on a network decomposed into driven, local-circulation and global-circulation parts;
- the markets domain's `money`: the round of money and where new money goes first;
- kinesis: dynamical systems, `transitions` among them;
- tyche (`bayes`, `uncertainty`), heuresis (evolution) and peira (designs).

#### 6.1 How to Build an Agent Simulation of Circulation

- **State: every agent a balance sheet.**
  - *Instruments.* Agents hold real stocks (goods, capital, inventories) and financial instruments. Each instrument is a typed claim with a holder and an issuer: deposit, loan, bond, currency, reserve, payable, community-currency balance.
  - *Postings.* A transaction is a posting of four entries (Caiani et al.'s quadruple entry). It is the only way any balance changes.
  - *Creation and destruction.* Money creation and destruction are named acts with a responsible agent: lending and repayment by a bank; spending and taxation by a government; purchases by a central bank; issuance by a community currency's mint.
  - *Distinguished node.* Everything else in the economy is a single distinguished node "outside", so that leakage is a flow, not a disappearance.
- **Relations: networks that carry flows, with time.** Supplier–customer links (Inoue and Todo's million-firm network; Pichler's industries), bank–customer and interbank links, employer–employee links, and the government's ties. Payments are settled with explicit timing and liquidity: a queue when a payer lacks liquidity, as in Galbiati and Soramäki's RTGS model, and optionally an offsetting stream (liquidity-saving mechanisms). Without timing, the order in which an allocation reaches agents cannot matter, and in reality it does.
- **Behaviour: three interchangeable kinds behind one interface.**
  - written rules (K+S, CATS and AB-SFC heuristics);
  - learned policies (RL, used also as adversaries that probe an allocation rule for leaks and gaming);
  - language-model proposals, grade C until validated (§4).

  Each declares what it observes and what it may do. Heterogeneity is a declared distribution, never a single representative.
- **Conservation, checked every step and reported as a verdict.**
  - net financial assets sum to zero over all agents and "outside";
  - each transaction-flow row and column sums to zero;
  - each agent's change in net worth equals its saving plus revaluations;
  - money changes only by named acts.

  A run that fails any check is refused with the first failing posting named, as the archive's journal names the first entry that does not seal. Entry, exit and default follow declared rules with their counter-entries, the places Caiani et al. found consistency most often breaks.
- **Initial state from accounts.** As in Pichler et al. and Poledna et al.: a steady state consistent with an input–output table or social accounting matrix and sectoral balance sheets. Stocks are then distributed across agents (symmetrically as in Caiani et al., or from microdata), and heterogeneity is left to emerge.

#### 6.2 Agent Simulations as Evaluators for Evolutionary Allocation, Where Order Matters

- **What makes order matter.** The order in which an allocation reaches agents changes what follows exactly when the simulation has:
  - timing, so that a recipient spends before the next grant;
  - liquidity constraints, so that early recipients relieve or deepen others' queues;
  - network position, so that money given to a hub reaches more agents in fewer rounds;
  - state-dependent behaviour (a constrained household spends locally at once; a liquid one saves or imports).

  The payment ABMs, the Sarafu evidence on early adopters and local hubs, and the markets domain's "where new money goes first" all point the same way. An evaluator for order-dependent allocation (Part II) should therefore be an agent simulation with payment timing, not a static multiplier.
- **The evaluator's answer.** For a candidate (a sequence of grants, or a policy reacting to state), it returns the circulation measures below as objectives, constraints or descriptors. It carries the conservation verdict as a hard constraint: a candidate that "wins" by a leak is infeasible, not good.
- **Replications and noise.** Common random numbers by keyed draws, and replications on demand, follow Part V. Agent simulations are noisy, and the differences between allocation orders may be small against run-to-run variation.
- **Surrogates and fidelities.** Surrogates (Lamperti et al.) may screen candidates; the full simulation decides survival. A cascade of fidelities is natural:
  1. a static input–output or Markov-chain retention estimate;
  2. a dynamic input–output model with inventories (Pichler);
  3. the full agent simulation with payments.
- **Adversaries.** RL agents trained against the allocation rule test whether a found allocation survives strategic responses. They play the role of the AI Economist's tax-gaming agents, made deliberate.

#### 6.3 Calibration

- **Accounts, then behaviour.** Declare the accounts (initial stocks and flows) and estimate only behavioural parameters the record identifies. Use Bayesian estimation (tyche's `bayes`) or neural simulation-based inference, with simulation-based calibration checks of the estimator (Dyer et al.). Keep the rest declared with ranges (Platt: estimation degrades with many free parameters).
- **Targets.** Calibrate to circulation statistics from transaction or payment records (Sarafu; payment-system summaries) as well as to macro aggregates. Hold out a part for validation, and test causal structure where possible (Guerini and Moneta's SVAR comparison).
- **Report identification.** Report what is not identified, exactly as kinesis's `scales` and `balance` modules report it.

#### 6.4 How to Read a Run: Circulation Measures to Compute

From the event log of every posting:

- **Velocity and holding time.** Turnover over mean stock, per instrument and per agent class. Also the holding-time distribution per unit of money (Wang et al.), not only its mean.
- **Retention and leakage.** Treat the log as a walk of money:
  - the probability that a unit injected at agent *i* is still within a region after *k* payments;
  - the expected number of local rounds before it leaks to "outside";
  - LM3 as the three-step truncation.

  Each is computed by where the money was injected, which is how allocation order shows up.
- **Circulation versus driven flow.** Apply economics' Hodge decomposition to the aggregated payment network of a window (driven, local-circulation and global-circulation parts). Also report combinatorial cycle measures: the share of value in strongly connected components (Fedwire: 90%), cycle participation and cyclic density (Iosifidis et al.), so the two notions of circulation can be compared.
- **Concentration.** Gini coefficients and top shares of balances and of throughput; the exponential bulk's "temperature" and the tail exponent (Yakovenko and Rosser); the core's share of value (Fedwire: 75% of value through 66 of about 6,600 banks); degree exponents, reciprocity and assortativity.
- **Fragility and crises.** Queue lengths and settlement delays; cascade sizes of defaults or of propagated shortfalls (Inoue and Todo's propagated against direct losses); and kinesis's detectors of critical transitions applied to the run's aggregates and structured states. Their verdicts are signal, no signal or undecidable, with error rates at the run's length.
- **Accounting invariants.** Sectoral net lending summing to zero; changes in money by creating act; the conservation verdict.

#### 6.5 What Simulation Can and Cannot Establish

- **It can establish:**
  - *Sufficiency.* A mechanism suffices to generate a pattern, such as endogenous cycles, a phase transition, cycles being necessary for circulation in a modular network, or concentration from identical agents.
  - *Comparisons under one model.* Allocation orders and policies can be ranked under a calibrated, conservation-checked model, with the ranking's robustness across behavioural kinds and parameter ranges stated.
  - *Conditional forecasts.* Where the model is anchored on accounts and the shock is well measured: Pichler et al.'s 21.5% against 22.1%; Poledna et al.'s parity with VAR and DSGE.
- **It cannot establish:**
  - *That a mechanism is the actual one.* Many models reproduce the same stylised facts.
  - *Behaviour outside validated ranges, or by unvalidated agents.* Parameters the record does not identify cannot be pinned down. Behaviour of LLM agents beyond what has been validated (homogeneous, hyper-rational in incentivised choices) is not evidence of human behaviour. RL agents show what an optimiser would do, not what people do.
  - *Results that depend on an accounting leak*, or on a production function chosen for convenience. Pichler et al. show the shock's size moving from a few points to 60% with that choice.
  - *Causal effects in the world without a design.* For those, simulation is a hypothesis generator and an evaluator of designs, which belong to peira.
- **Every result should be stated as the infrastructure states its others:** what was declared, what was estimated, what the record does not identify, the conservation verdict, the share of model-generated behaviour, and the verdicts of the circulation measures with their uncertainty across replications.

## Part VII. Economic Agents in Learned Spaces and Dynamical Systems: Latent Geometry, Densities, Learned Simulators and Digital Twins

*Remark.* Part VII's scope and conventions. Marks used throughout: **[V]** verified by reading the source itself (an abstract page or full text fetched during this research); **[S]** from a search-engine snippet or bibliographic record only; **[M]** from memory, not checked during this research. Where a DOI is marked [M] it should be checked before it is cited in a report. Research date: 2026-11-20.

Scope note: the brief has five topic areas ((1) latent spaces, (2) densities and mean fields, (3) learned dynamics, (4) digital twins, (5) combinations and other methods). They are distributed below across the five key questions; each question's section collects the sources that answer it. The last section, required by the brief, draws the consequences for the infrastructure.

### 1. What Embedding Economic Agents in a Latent Space or Manifold Reveals, and Its Pitfalls

#### Takeaway
A latent space turns a network of ties or flows into positions whose distances, curvature and one-dimensional orderings carry structure the raw graph does not show directly: transitivity and clustering (Hoff et al.), hierarchy and curvature (hyperbolic trade maps), relatedness and the paths of development (the product space, economic complexity as spectral clustering, the fitness–GDP plane), and substitution and complementarity (consumer embeddings). The same geometry is identified only up to a group of transformations (orthogonal, or indefinite orthogonal), its dimension is a modelling choice that can overfit, and orderings such as the ECI are approximate solutions of a relaxed cut problem whose link to income is empirical, not derived.

#### Cited Findings

**1.1 Latent space network models (statistics).**
- Hoff, Raftery and Handcock, "Latent Space Approaches to Social Network Analysis," *JASA* 97(460):1090–1098, 2002, DOI 10.1198/016214502388618906: the probability of a tie depends on the actors' positions in an unobserved "social space"; two variants, the distance model and the projection model; inference by maximum likelihood and Bayesian MCMC for positions and covariate effects [S] — [Search record / author PDF](https://handcock.github.io/publication/latent-space/hoffrafteryhandcock2002.pdf). (The author PDFs were scanned images and could not be read by the fetch tool.)
- The distance model's form is logit P(y_ij = 1) = α + β′x_ij − |z_i − z_j|; because the likelihood depends on the positions only through distances, it is invariant to rotation, reflection and translation of the configuration, and posterior positions are summarised after a Procrustes alignment to a reference configuration [M] — [Hoff et al. 2002](https://sites.stat.washington.edu/raftery/Research/PDF/hoff2002.pdf).
- Handcock, Raftery and Tantrum (2007) extended the model to clustering among actors as spherical Gaussian clusters in the latent space (the latent position cluster model) [S]; venue *JRSS A* 170(2) [M] — [Handcock et al. 2007 PDF](https://www.stat.cmu.edu/~brian/780/bibliography/03%20Latent%20Space%20Models/Handcock%20-%202007%20-%20Model-based%20clustering%20for%20social%20networks.pdf).
- Ward, Ahlquist and Rozenas, "Gravity's Rainbow: A dynamic latent space model for the world trade network," *Network Science* 1(1):95–118, 2013, DOI 10.1017/nws.2013.1: combine a gravity specification with latent space networks into a dynamic mixture model for real-valued directed graphs, 1990–2008; argue standard gravity models ignore network dependence (reciprocity, third-order dependence); the model "substantially outperforms standard accounts" in in- and out-of-sample predictive heuristics; replication data at Harvard Dataverse DOI 10.7910/DVN/8BECCS [S] — [Cambridge Core](https://www.cambridge.org/core/journals/network-science/article/gravitys-rainbow-a-dynamic-latent-space-model-for-the-world-trade-network/A452C408399F331A19B59B2D7D8FA031).

**1.2 Identifiability of latent positions (spectral and generalised).**
- Athreya et al., "Statistical inference on random dot product graphs: a survey," *JMLR* 18(226), 2018: spectral embeddings of adjacency and Laplacian matrices; the adjacency spectral embedding ASE(A, d) = Û Ŝ^{1/2}; latent positions are recoverable only up to an orthogonal transformation, since any orthogonal transformation preserves inner products and yields the same distribution of A [S] — [JMLR](https://jmlr.org/papers/v18/17-448.html).
- Rubin-Delanchy, Cape, Tang and Priebe, the generalised RDPG (arXiv 1709.05506; *JRSS B* 2022 [M]): with an indefinite signature (p, q), q > 0, positions are identified only up to indefinite orthogonal transformations, inter-point distances are then not invariant, and "distance-based inference procedures such as classical K-means are to be avoided"; a second non-identifiability, "artificial dimension blow-up" [S] — [arXiv 1709.05506](https://arxiv.org/pdf/1709.05506).

**1.3 Hyperbolic geometry and hierarchy.**
- García-Pérez, Boguñá, Allard and Serrano, "The hidden hyperbolic geometry of international trade: World Trade Atlas 1870–2013," *Scientific Reports* 6:33441, 2016 (DOI 10.1038/srep33441 [S]); arXiv 1512.02233: annual maps place countries in a "trade space" whose distance combines economic size and effective distance, from a gravity-type model that predicts significant trade channels; the space is hyperbolic, not flat; the departure from flatness has grown since the First World War, so trade has become more hierarchical, small economies moving away from all but the largest; preferential trade agreements do not align with the natural communities of the space and have not necessarily lowered internal barriers; three forces: globalisation, hierarchisation, localisation. The abstract reports no accuracy metrics [V] — [arXiv abstract](https://arxiv.org/abs/1512.02233).
- A hyperbolic embedding model for directed networks, applied to international trade, reports a core–periphery structure with about 35% of nodes in the centre [S] — [arXiv 1906.03597](https://arxiv.org/pdf/1906.03597).
- Hyperbolic embedding of interbank networks finds that high-degree banks typically act as hubs between lower-degree banks not directly linked [S] — [arXiv 2005.00399](https://arxiv.org/pdf/2005.00399).
- I found no peer-reviewed hyperbolic embedding of a firm-level production network or an input–output table; the closest is an unrefereed project embedding the NAICS industry taxonomy in the Lorentz model, chosen over the Poincaré ball for numerical stability near the boundary [S] — [NAICS hyperbolic embedder](https://lowmason.github.io/naics-embedder/overview).

**1.4 The product space and economic complexity as embeddings.**
- Hidalgo, Klinger, Barabási and Hausmann, "The Product Space Conditions the Development of Nations," *Science* 317(5837):482–487, 2007 (DOI 10.1126/science.1144581 [M]; arXiv 0708.2090): upscale products sit in a densely connected core, lower-income products in a sparse periphery; countries move to goods close to those they already specialise in, so countries in connected parts upgrade faster; reaching the core needs "empirically infrequent" jumps, which may explain non-convergence [S] — [arXiv 0708.2090](https://arxiv.org/abs/0708.2090).
- Mealy, Farmer and Teytelboym, "Interpreting economic complexity," *Science Advances* 5(1):eaau1705, 2019: the ECI is the eigenvector of M̃ = D⁻¹S (S = M U⁻¹ M′, a country-similarity matrix) for the second largest eigenvalue, equal to D^{−1/2} times the Fiedler vector of the normalised Laplacian, hence an approximate (relaxed) solution of the Normalized Cut problem, which is NP-hard exactly; the ECI is orthogonal to diversity as vectors (dot product zero) but orthogonality is not zero correlation (positive for countries, negative for UK local authorities, none for US states); the ECI is "the optimal one-dimensional ordering" minimising distances between similar nodes, a special case of diffusion-map distance; why such an ordering tracks income is "not immediately obvious" (empirical, not derived); for exports and both regional data sets the data "do not partition clearly into two components"; results depend on the binary RCA threshold [V] — [PMC full text](https://pmc.ncbi.nlm.nih.gov/articles/PMC6326748).
- Tacchella, Mazzilli and Pietronero, "A dynamical systems approach to gross domestic product forecasting," *Nature Physics* 14:861–865, 2018, DOI 10.1038/s41567-018-0204-y: growth as a two-dimensional dynamical system in the plane of GDP per capita and "fitness," computed only from product-level export data; forecast errors are predictable and uncorrelated with the IMF's, so the methods carry complementary information; press accounts say it beat the IMF's five-year forecasts in about a quarter of cases [S] — [Uniroma record](https://iris.uniroma1.it/handle/11573/1186189); [phys.org](https://phys.org/news/2018-07-physics-gdp.amp). The method forecasts by analogues in that plane ("selective predictability scheme"), with laminar regions where trajectories are predictable and chaotic ones where they are not [M].
- Brummitt, Gómez-Liévano, Hausmann and Bonds, "Machine-learned patterns suggest that diversification drives economic development," *J. R. Soc. Interface* 17(162):20190283, 2020: Principal Smooth-Dynamics Analysis (PriSDA) finds that diversity (weighted to sophisticated products) is what most distinguishes export baskets over 55 years; diversity precedes growth; the inferred dynamics predict diversified middle-income countries grow fastest and countries converge to intermediate income and specialisation [S] — [arXiv 1812.03534](https://arxiv.org/abs/1812.03534v1).

**1.5 Embeddings of firms, products, households and consumers.**
- Supply chains as a heterogeneous knowledge graph with learnable embeddings, link prediction by a relational GCN on a global automotive network ("the first such representation," per its authors) [S] — [arXiv 2107.10609](https://arxiv.org/abs/2107.10609).
- Firm "in" and "out" embeddings so that buying from and selling to a firm get different probabilities, for multi-scale reconstruction of large supply networks [S] — [arXiv 2412.16122](https://arxiv.org/pdf/2412.16122).
- Supplier–buyer–product transactions as a temporal graph with an inventory module that learns production functions and forecasts links; releases the open simulator SupplySim [S] — [arXiv 2407.18772](https://arxiv.org/html/2407.18772v1).
- Mungo et al., review of supply-network reconstruction: frames it as link prediction; a link's likelihood depends on what the two firms produce; methods from financial networks may not transfer to production networks [S] — [arXiv 2310.00446](https://arxiv.org/pdf/2310.00446).
- Ruiz, Athey and Blei, "SHOPPER: A probabilistic model of consumer choice with substitutes and complements," *Annals of Applied Statistics*, 2020, DOI 10.1214/19-AOAS1265: items have latent attribute vectors, choice probabilities are log-bilinear; framed both as a structural model of utility maximisation and as a generative model; "thinking ahead" lowers the probability of seasoning when its likely complement (taco shells) is expensive; accurate under price interventions and better than econometric choice models and ML baselines on a large grocery data set; identifies complements and substitutes [S] — [arXiv 1711.03560](https://arxiv.org/pdf/1711.03560.pdf).
- Begicheva and Zaytsev: embeddings of a large Russian bank's client transactions shift with economic events such as COVID-19 and outperform hand-crafted features for macro signals [S] — [arXiv 2110.12000](https://arxiv.org/pdf/2110.12000).
- Resheff and Shahar (Intuit, 2017): household demographics (adults, children) predicted from transaction categories and merchant names for about 680,000 users [S] — [arXiv 1712.07230](https://web3.arxiv.org/pdf/1712.07230).
- A transformer foundation model trained on payment-network transactions (TREASURE) reports embeddings of spending habits and merchant preferences and a 90% gain in downstream recommendation [S] — [arXiv 2511.19693](https://arxiv.org/html/2511.19693v1).

#### Inferences
- What a latent space adds is a *metric or ordering* in which questions become geometric: "how far is this country from the core" (product space), "how hierarchical has trade become" (curvature), "which items substitute" (inner products in SHOPPER). These are statements about invariants of the embedding (distances, inner products, curvature, orderings up to sign), not about coordinates. Any analysis that reads individual coordinates, or compares coordinates across separately fitted years without alignment, reads something that is not identified.
- The non-identifiability group is the key fact to carry into code: O(d) for distance and RDPG models, O(p, q) for generalised RDPG; translation as well for distance models. Procrustes alignment across time (as in dynamic latent space models) is a choice of gauge, and dynamics read from aligned positions depend on the alignment rule.
- Mealy et al. give a clean warning against over-reading one-dimensional indices: the ECI is a relaxed graph cut, its sign split is not clean in real data, and its relation to income is an empirical regularity. Tacchella et al. and Brummitt et al. show the productive use of such low-dimensional coordinates: as the state space of a learned dynamical system, evaluated by forecasts.
- Overfitting in latent space models enters through the dimension d and the flexibility of the link function; dimension selection by information criteria or held-out link prediction is the standard guard [M]. Ward et al.'s out-of-sample comparison is the right kind of evidence; the hyperbolic trade atlas abstract reports no held-out accuracy.

#### Gaps
- Hoff et al.'s exact treatment of invariance (Procrustes) and of dimension choice could not be read from the source (scanned PDF); the statements are [M].
- No peer-reviewed hyperbolic embedding of firm-level production networks or input–output tables was found; no study was found comparing node2vec-type embeddings of production networks against economic baselines on economic outcomes.
- No study was found that embeds *households* from consumption or payments and then reads circulation or inequality from the geometry; the household-level work found is commercial prediction (demographics, recommendation).

### 2. When an Economy of Many Agents Is Better Studied as a Dynamical System of a Density, and the Methods That Solve Such Models Now

#### Takeaway
A density description (Kolmogorov forward/Fokker–Planck equation coupled to a Hamilton–Jacobi–Bellman equation in mean-field games and continuous-time heterogeneous-agent models; Boltzmann and Fokker–Planck equations in kinetic exchange models) is preferable when agents are exchangeable given a low-dimensional individual state, numerous enough that idiosyncratic noise averages out, and interact only through aggregates or a mean field; it then yields analytic characterisations, uniqueness results and fast solvers. With aggregate shocks the density itself becomes a state, and the frontier (2021–2026) is deep learning: DeepHAM's learned "generalised moments," Maliar–Maliar–Winant's all-in-one expectation operator, symmetric architectures (Kahou et al.), and neural solutions of the master equation (Gu, Laurière, Merkel and Payne's EMINNs), all of which "tame" but do not remove the curse of dimensionality.

#### Cited Findings

**2.1 Mean-field games and the continuous-time heterogeneous-agent model.**
- Lasry and Lions, "Mean field games," *Japanese Journal of Mathematics* 2(1):229–260, 2007, DOI 10.1007/s11537-007-0657-8: surveys mean-field models by analogy with statistical mechanics; in three examples derives a mean-field problem of nonlinear differential equations; introduced independently by Huang, Malhamé and Caines [S] — [JJM abstract](https://www.ms.u-tokyo.ac.jp/~toshi/jjm/JJM_HP/contents/abstract/2-1/JJM0657.html).
- Achdou, Han, Lasry, Lions and Moll, "Income and Wealth Distribution in Macroeconomics: A Continuous-Time Approach," *Review of Economic Studies* 89(1):45–86, 2022 (NBER w23732, 2017; DOI 10.1093/restud/rdab002 [M]): recasts the Aiyagari–Bewley–Huggett model as a system of PDEs (HJB coupled to a Kolmogorov forward equation); analytic characterisation of the consumption and saving of the poor; a closed-form wealth distribution for two income types; a proof of a unique stationary equilibrium when the intertemporal elasticity of substitution is at least one; a "simple, efficient and portable" finite-difference algorithm for a wide class of heterogeneous-agent models; open access (CC BY) [S] — [NBER](https://www.nber.org/papers/w23732); codes at [benjaminmoll.com/HACT](https://benjaminmoll.com/HACT) [S].
- Carmona, "Applications of mean field games in financial engineering and economic theory" (arXiv 2012.05237; AMS PSAPM 2021): reviews MFG models of bitcoin mining, energy markets, macro growth, the bridge from macro to finance, and contract theory; argues general equilibria can be treated as Nash equilibria of MFGs [S] — [arXiv 2012.05237](https://www.arxiv.org/abs/2012.05237). Carmona and Laurière survey deep learning methods for MFGs with financial applications [S] — [arXiv 2107.04568](https://arxiv.org/abs/2107.04568v1).

**2.2 Kinetic and Fokker–Planck descriptions of wealth exchange.**
- Pareschi and Toscani: in non-conservative binary-exchange models a suitable asymptotic limit reduces the Boltzmann equation to a Fokker–Planck-type equation with an explicitly computable stationary state, giving power-law (Pareto) tails [S] — [arXiv 1009.2760](https://pdf.arxiv.org/pdf/1009.2760). The Fokker–Planck operator in this line was first obtained by Bouchaud and Mézard and later derived from a Boltzmann model by Cordier, Pareschi and Toscani [S] — [Düring and Toscani PDF](https://bduring.warwick.ac.uk/pdf/Economy_macro.pdf).
- Wealth coupled to a "knowledge" variable in binary trades: a nonlinear Boltzmann equation for the joint density and its Fokker–Planck limit [S] — [arXiv 1401.4550](https://arxiv.org/pdf/1401.4550.pdf).
- Düring and Pareschi: optimal control of kinetic wealth models (minimising the variance of the wealth density), related to taxation and redistribution [S] — [arXiv 1803.02171](https://ar5iv.labs.arxiv.org/html/1803.02171).
- Li, Boghosian and Li, "The Affine Wealth Model," *Physica A* 516:423–442, 2019, DOI 10.1016/j.physa.2018.10.042: a binary-transaction asset-exchange model invariant under scalings and shifts, allowing negative wealth (about 10.9% of US households in 2016); the steady state of its Fokker–Planck equation matches the US Survey of Consumer Finances over 27 years with average error below 0.16% [S] — [arXiv 1604.02370](https://arxiv.org/abs/1604.02370v2).
- Yakovenko and Rosser, "Colloquium: Statistical mechanics of money, wealth, and income," *Rev. Mod. Phys.* 81:1703, 2009, is the standard review of the exchange-model line begun by Drăgulescu and Yakovenko (2000) [M].

**2.3 Deep learning for heterogeneous-agent models with aggregate shocks.**
- Han, Yang and E, "DeepHAM: A Global Solution Method for Heterogeneous Agent Models with Aggregate Shocks" (arXiv 2112.14377; *Quantitative Economics* 17(2):297–341, 2026, DOI 10.3982/QE2190 [S]): the state distribution is represented by a set of optimal "generalised moments" (neural networks found by the algorithm, permutation-invariant in the agents); value and policy functions are neural networks; the objective is optimised over directly simulated paths; efficient, avoids the curse of dimensionality in their claims, gives an interpretable representation of the distribution, and solves the constrained-efficiency (planner) problem as easily as the competitive equilibrium [V] — [arXiv abstract](https://arxiv.org/abs/2112.14377). In the introduction one generalised moment reduces error by 54.2% against 37.5% for the Krusell–Smith method; the method assumes aggregate prices have explicit forms, excluding e.g. a New Keynesian Phillips curve; code at GitHub frankhan91/DeepHAM [S] — [arXiv PDF](https://arxiv.org/pdf/2112.14377v2.pdf).
- Maliar, Maliar and Winant, "Deep learning for solving dynamic economic models," *Journal of Monetary Economics* 122:76–101, 2021, DOI 10.1016/j.jmoneco.2021.07.004: recasts dynamic models as nonlinear regression; an "all-in-one" integration operator for high-dimensional expectations; neural networks for model reduction and multicollinearity; applied to Krusell–Smith; TensorFlow code [S] — [author PDF](https://web.stanford.edu/%7Emaliars/Files/JME2021.pdf).
- Kahou, Fernández-Villaverde, Perla and Sood, "Exploiting Symmetry in High-Dimensional Dynamic Programming" (NBER w28981, 2021): exploit permutation symmetry, a single Monte Carlo draw of idiosyncratic shocks, and architectures that use symmetry and concentration of measure; a multi-firm Lucas–Prescott investment model, checked against an LQG version, then solved nonlinearly [S] — [NBER](https://www.nber.org/papers/w28981).
- Gu, Laurière, Merkel and Payne, "Global Solutions to Master Equations for Continuous Time Heterogeneous Agent Macroeconomic Models" (arXiv 2406.13726, 2024): three finite approximations of the distribution (a finite number of agents, about 40 in Krusell–Smith; masses on a grid, about 200 points; a projection on about 5 basis functions built from eigenfunctions of the KFE operator); the value function is an "Economic Model Informed Neural Network" trained by adapting the Deep Galerkin Method and PINNs to minimise the squared master-equation error at sampled points; solves continuous-time Krusell–Smith, Aiyagari (matching finite differences), a heterogeneous-firm model and Bilal's (2023) spatial model. Stated limits: sampling the distribution is the hardest part and "deep learning does not remove the curse of dimensionality"; networks can converge to "cheat solutions" that zero derivatives; hard borrowing limits make training fragile (replaced by soft penalties); prices must be closed-form in (z, g); the distribution must enter only through prices [V] — [arXiv HTML](https://arxiv.org/html/2406.13726v1).
- Fernández-Villaverde, Nuño and Perla, "Taming the curse of dimensionality: quantitative economics with deep learning" (NBER w33117 / CEPR DP 19636 / Banco de España WP 2444, November 2024): deep learning as a route past the curse of dimensionality, "tame" not "break" since the curse returns in worst cases; applied to the stochastic growth model; ends with a survey of applications and "cautious optimism" [S] — [Banco de España](https://www.bde.es/wbe/en/publicaciones/analisis-economico-investigacion/documentos-trabajo/taming-the-curse-of-dimensionality-quantitative-economics-with-deep-learning.html).

**2.4 From agents to a density, learned from simulation.**
- Niemann, Klus and Schütte (PLOS ONE 2021; arXiv 2012.07718): Koopman-generator methods learn reduced models of agent-based systems from simulation data alone, as ODEs or SDEs in aggregated variables; agreement with analytical coarse-grained results holds only when the number of agents is sufficiently large [V] — [arXiv abstract](https://arxiv.org/abs/2012.07718).

#### Inferences
- A density model is the better object when (a) agents are exchangeable given a low-dimensional individual state (wealth, income state), (b) interactions pass through aggregates (prices, mean field) or through random binary encounters, and (c) N is large enough that the law of large numbers holds; Niemann et al.'s large-N condition is the empirical counterpart. It is the worse object when identities, networks of specific ties, or small numbers of large agents matter (supply chains, interbank networks), where agent-level or graph-level models are needed.
- Without aggregate shocks (stationary equilibria, transitions under perfect foresight) the HJB–KFE finite-difference approach of Achdou et al. is accurate and cheap; with aggregate shocks the density is an infinite-dimensional state and the choice is between perturbation (Reiter-style) and global deep methods. The deep methods' accuracy evidence is mostly internal (Euler or master-equation residuals, agreement with Krusell–Smith or finite-difference benchmarks), not out-of-sample fit to data.
- Kinetic exchange models give a second, rule-based route to distributions with a cheap stationary-state characterisation and a natural control formulation (Düring–Pareschi), but their agents do not optimise; the Affine Wealth Model's close fit to the SCF is a fit of a stationary distribution's shape, which several mechanisms could produce.

#### Gaps
- No head-to-head comparison was found of deep global methods (DeepHAM, EMINN, Maliar et al.) against each other on one benchmark with common error metrics; the published comparisons are each against classical methods.
- Opinion and expectation dynamics coupled to economic flows: beyond the kinetic wealth–knowledge model (arXiv 1401.4550) I found no source in this research; the heuristic-switching expectations literature (Brock–Hommes) is relevant [M] but was not checked.
- Achdou et al.'s DOI and the precise statement of their uniqueness theorem were not read from the article.

### 3. How Well Learned Simulators Reproduce Agent-Based or Real Economic Dynamics Out of Sample, and How Their Uncertainty Is Reported

#### Takeaway
Learned simulators generalise well in physics (graph network simulators roll out thousands of steps on systems ten times larger than trained on), but in economics the evidence is thinner and mostly synthetic: surrogates of ABMs reproduce *calibration targets* across parameter space (Lamperti et al.: F1 about 0.8, true positive rates 0.7–0.95, 500–3,750 times faster), GNN surrogates reproduce *individual behaviour* of canonical non-economic ABMs (Cozzi and Pangallo), and data-driven national ABMs beat AR(1), VAR and in some cases DSGE benchmarks in out-of-sample macro forecasts (Poledna et al. 2023; Wiese et al. 2024 for 38 OECD countries). Uncertainty is rarely reported as calibrated intervals: most papers give point metrics, figures with confidence bands over repeated runs, or posterior distributions over parameters from simulation-based inference; formal coverage of a surrogate's predictive intervals was not found.

#### Cited Findings

**3.1 Graph neural simulators (origins).**
- Battaglia, Pascanu, Lai, Rezende and Kavukcuoglu, "Interaction Networks for Learning about Objects, Relations and Physics," NIPS 2016 (arXiv 1612.00222): a network on graphs making object- and relation-centric predictions; simulates dozens of objects over thousands of steps, estimates energy, and generalises to different numbers and configurations of objects [V] — [arXiv](https://arxiv.org/abs/1612.00222).
- Sanchez-Gonzalez et al., "Learning to Simulate Complex Physics with Graph Networks" (arXiv 2002.09405; ICML 2020 [M]): the Graph Network-based Simulator learns from single-step predictions on systems of thousands of particles and generalises to new initial conditions, thousands of steps and systems with at least 10 times more particles; long-term accuracy depends mainly on the number of message-passing steps and on noise added to training inputs to limit error accumulation [V] — [arXiv](https://arxiv.org/abs/2002.09405).

**3.2 Surrogates and emulators of agent-based models.**
- Lamperti, Roventini and Sani, "Agent-based model calibration using machine learning surrogates," *JEDC* 90:366–389, 2018 (DOI 10.1016/j.jedc.2018.03.011 [M]): XGBoost surrogates trained by pool-based active learning (Sobol seed, new points from predicted positives, else by label entropy, C·log(budget) per round); calibration targets: a KS test of simulated returns against the S&P 500 (Brock–Hommes, 12 parameters) and sustained growth with fat tails (Islands growth model, 8 parameters); out-of-sample F1 about 0.80 and TPR about 0.70 at 500 and 0.95 at 2,500 labelled points; the surrogate is about 500 times (Brock–Hommes) and 3,750 times (Islands) faster; uncertainty only as 95% bands in figures over 100 repetitions, random seed fixed across parameter sets in the main experiments; the authors advise verifying positives in the true model [V] — [ar5iv](https://ar5iv.arxiv.org/html/1703.10639).
- Dyer, Cannon, Farmer and Schmon, "Calibrating Agent-based Models to Microdata with Graph Neural Networks" (arXiv 2206.07570; ICML 2022 AI4ABM spotlight): temporal (recurrent) GNNs with neural posterior estimation learn parameter posteriors directly from agent-level microstates rather than summary statistics [S] — [arXiv](https://arxiv.org/pdf/2206.07570v1.pdf).
- Cozzi and Pangallo, Graph Diffusion Networks (arXiv 2505.21426): a message-passing GNN for each agent's neighbourhood with a conditional diffusion model sampling the next state; a differentiable surrogate of individual behaviour, not of summary statistics; tested on Schelling segregation and a predator–prey ABM, no economic ABM; micro-level Earth Mover's Distance on held-out branches over 25 steps; macro sMAPE below 0.2 for Schelling and about 0.08 in the best predator–prey case when trained on 10 steps and forecasting 25; stochasticity from the diffusion model trained on about 500 sibling branches per step; limits: the interaction graph must be known, all-to-all interactions and multiple decision rounds per step are not covered, only two canonical ABMs validated [V] — [arXiv HTML](https://arxiv.org/html/2505.21426v2).
- Monti, Pangallo, De Francisci Morales and Bonchi, "On learning agent-based models from data," *Scientific Reports* 13:9268, 2023 (arXiv 2205.05052): turn an ABM into a probabilistic model with a tractable likelihood and fit agent-level latent variables by gradient-based EM; learned latent traces correlate with ground truth at Pearson 0.5–0.9 in synthetic tests; case study a housing-market ABM [S] — [arXiv](https://arxiv.org/abs/2205.05052v1).
- Antulov-Fantulin, "Towards agent-based-model informed neural networks" (arXiv 2512.05764, December 2025; EPJ Data Science January 2026 [S]): restricted GNNs with hierarchical decomposition to learn "structure-preserving dynamics" respecting mass conservation, locality and bounded rationality; recovers parameters of generalised Lotka–Volterra under interventions; beats GCN, GraphSAGE and Graph Transformer baselines out of sample on a graph SIR model; learns coupled GDP dynamics of the ten largest economies and demonstrates gradient-based counterfactual policy analysis, with no quantitative out-of-sample result for the macro case in the abstract [V] — [arXiv abstract](https://arxiv.org/abs/2512.05764).

**3.3 Operator and equation-discovery methods on ABM output and economic data.**
- Hogg et al. (PLOS ONE 2019): Koopman mode analysis of logistics ABMs finds growing, decaying or oscillating collective modes and detects bifurcations from the spectrum, with analysis justifying DMD in that setting [S] — [PMC record](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC6738619/).
- Koopman-based surrogate models for multi-objective optimisation of agent-based systems exist (arXiv 2306.17666), but only the reference list was seen [S] — [arXiv](https://arxiv.org/pdf/2306.17666).
- SINDy in economics: a 2025 *Physica A* paper finds SINDy-SR3 and ensemble SINDy improve identification under economic data constraints, with Kalman and Savitzky–Golay filtering for robustness, noting that prior work focused on microeconomic problems [S] — [IDEAS](https://ideas.repec.org/a/eee/phsmap/v675y2025ics0378437125005138.html); Yazdanpanah and Moaveni (*Computational Economics* 2025) combine transfer entropy with SINDy with control on macroeconomic variables [S] — [search record](https://arxiv.org/pdf/2511.08606v1). No benchmark of SINDy against VAR/ARMA on macro series was found.
- Hybrid universal differential equations: a known simplified macro structure plus a learned residual network initialised near zero, compared with a pure neural ODE and a PINN (arXiv 2607.09684, "SciML in the wild: when structural priors help and when they hurt") [S] — [arXiv](https://arxiv.org/pdf/2607.09684); a supply-chain UDE keeps conservation and order-up-to structure and learns only a small policy term (BULL-ODE) [S] — [arXiv 2509.18105](https://arxiv.org/abs/2509.18105v1).
- Reservoir computing: Ballarin et al., multi-frequency echo state networks for US output growth, *International Journal of Forecasting* 40(3):1206–1237, 2024: superior or comparable to MIDAS and dynamic factor models at much lower cost; later ensembles improve further [S] — [arXiv 2211.00363](https://arxiv.org/abs/2211.00363).

**3.4 Out-of-sample evidence for data-driven ABMs of real economies.**
- Poledna, Miess, Hommes and Rabitsch, "Economic forecasting with an agent-based model," *European Economic Review* 151:104306, 2023, DOI 10.1016/j.euroecorev.2022.104306 (CC BY): "the first agent-based model that can compete with benchmark VAR and DSGE models in out-of-sample forecasting of macro variables"; covers all ESA 2010 activities of Austria with millions of heterogeneous agents, from national and sector accounts, input–output tables, government statistics, census and business demography; applied to the medium-run effects of Austria's COVID-19 lockdowns [S] — [IIASA record](https://pure.iiasa.ac.at/18339). (The full text was behind bot protection; RMSE tables were not read.)
- Wiese, Kaszowska-Mojsa, Dyer, Moran, Pangallo, Lafond, Muellbauer, Calinescu and Farmer, "Forecasting Macroeconomic Dynamics using a Calibrated Data-Driven Agent-based Model" (arXiv 2409.18760, 2024): extends Poledna et al. with housing, richer credit, improved behavioural rules and synthetic household populations; calibrated for all 38 OECD countries by approximate Bayesian inference; outperforms the original model and an AR(1) "by a highly statistically significant margin" out of sample; a platform for building and evaluating alternative models [V] — [arXiv abstract](https://arxiv.org/abs/2409.18760).
- Pangallo and del Rio-Chanona, "Data-Driven Economic Agent-Based Models" (arXiv 2412.16591; *The Economy as an Evolving Complex System IV*, SFI Press): defines and grades data-driven ABMs by whether initial agent states come from micro-data and whether dynamics follow empirical time series; reviews calibration, initialisation and data assimilation [S] — [arXiv](https://arxiv.org/abs/2412.16591v1).

**3.5 Behaviour learned from data: inverse reinforcement learning and multi-agent RL.**
- Rawat and Rust, "Dynamic Discrete Choice and Inverse Reinforcement Learning: Inferring Preferences and Beliefs From Human Behavior" (arXiv 2608.24362, August 2026): soft Q-learning IRL is closely related to dynamic discrete choice with additive extreme-value shocks, the same softmax choice probabilities and smooth Bellman equations; DDC uses ML, CCP estimators and policy iteration, IRL maximum-entropy, adversarial and model-free TD methods that scale to high dimensions; shared problems: non-identification (many rewards rationalise the same behaviour) and the curse of dimensionality [V] — [arXiv](https://arxiv.org/abs/2608.24362).
- Kang, Yoganarasimhan and Jain frame DDC estimation as offline maximum-entropy IRL by empirical risk minimisation without a transition model [S] — [arXiv 2502.14131](https://www.arxiv.org/pdf/2502.14131); Sharma, Kitani and Groeger (2017) link maximum causal entropy IRL to DDC with a CCP-based algorithm [S] — [arXiv 1709.07597](https://arxiv.org/pdf/1709.07597v1.pdf).
- Zheng, Trott, Srinivasa, Parkes and Socher, "The AI Economist: Taxation policy design via two-level deep multiagent reinforcement learning," *Science Advances* 8(18), 2022, DOI 10.1126/sciadv.abk2607: agents and a planner co-adapt, stabilised by curriculum learning; recovers the optimal tax in one-step economies; improves the equality–productivity trade-off by 16% over baselines including the Saez framework in simulation; robust to learned tax-gaming [S] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC9067926).

**3.6 Other intelligent methods worth knowing.**
- Causal discovery for ABM validation: robust cross-validated VarLiNGAM and PCMCI improve reliability of recovered causal structure on noisy series [S] — [arXiv 2410.19412](https://arxiv.org/html/2410.19412v3); LPCMCI with a nonlinear (GPDC) test on US GDP, growth, inflation and unemployment 1970–2021, with PCMCI's assumptions (causal sufficiency, Markov, faithfulness, stationarity) often violated in macro data [S] — [arXiv 2509.07036](https://arxiv.org/html/2509.07036v1).
- Topological data analysis: Gidea and Katz, persistence landscapes of sliding-window point clouds of four US index returns; L^p norms grow strongly before the peaks of the 2000 and 2007–09 crashes, proposed as early warning complementary to standard statistics [S] — [arXiv 1703.04385](https://arxiv.org/pdf/1703.04385).
- Graph signal processing of payment flows: no application was found; the payment literature is network analysis (Fedwire, UK, global SWIFT MT103 flows) and temporal-graph representation learning for anomalies [S] — [search record](https://www.emeraldinsight.com/insight/content/doi/10.1108/SEF-10-2018-0310/full/html); [arXiv 2210.11168](https://arxiv.org/pdf/2210.11168).

#### Inferences
- The out-of-sample evidence comes in three different kinds, not to be pooled: (i) surrogate fidelity *to the simulator* across held-out parameters (Lamperti et al., Cozzi–Pangallo); (ii) forecast accuracy *against data* for aggregates (Poledna et al., Wiese et al., MFESN, Tacchella et al.); (iii) recovery of known truth in synthetic tests (Monti et al., Antulov-Fantulin's Lotka–Volterra). Only (ii) speaks to reality, and only for the variables and horizons forecast; none tests counterfactuals (the policy effects these models are built to evaluate), since those have no out-of-sample ground truth except natural experiments.
- The physics results suggest which design choices matter (message passing depth, training noise against error accumulation, relational inductive bias); economic graphs differ in having no conservation laws or symmetries to exploit except accounting identities (stock–flow consistency, conservation of money in transfers), which the hybrid "structure-preserving" approaches (UDEs, ABM-informed networks) exploit.
- Uncertainty reporting is the weakest link: the surrogate literature reports point metrics and repeated-run bands; simulation-based inference (Dyer et al.) reports parameter posteriors; diffusion-model surrogates report a predictive distribution but without checked calibration. Conformal or coverage-checked predictive intervals for economic surrogates were not found.
- IRL and DDC share a non-identification result: preferences learned from behaviour are a set. Any "learned agent" used to evaluate counterfactual allocations inherits that set, and should report decisions that hold across it.

#### Gaps
- Poledna et al.'s RMSE comparisons (variables, horizons, which benchmarks won where) were not read; only the abstract claim is cited.
- No paper was found applying graph network simulators in the Sanchez-Gonzalez sense to real economic transaction or production networks with out-of-sample rollouts.
- No Koopman/DMD study of a macroeconomic ABM or of real macro data was found; the found applications are logistics and canonical social ABMs.
- No comparison of SINDy, neural ODEs and reservoir computing on the same economic series was found.

### 4. Approaches That Simulate Agents' Relations, States and Behaviours Under Learned Dynamics and Then Analyse Circulation, Distribution or Stability, and Their Evidence of Validity

#### Takeaway
Few published systems do all of it at once. The pieces exist: low-dimensional latent states with learned dynamics for countries (fitness–GDP plane, PriSDA) evaluated by forecasts; data-driven national ABMs (Austria, 38 OECD countries) initialised from micro-data and validated on aggregate forecasts; deep-learned heterogeneous-agent models that read distributions and planner optima (DeepHAM, EMINN); kinetic models whose distributions fit survey data (Affine Wealth Model) and that can be controlled; multi-agent RL economies that read equality–productivity trade-offs (AI Economist); and graph-learned dynamics with counterfactuals (ABM-informed networks). Evidence of validity is strongest for aggregate forecasts and stationary distributions, weak for circulation on networks and for stability or fragility claims, and essentially absent for "digital twins" of whole economies or cities beyond aggregate forecasting.

#### Cited Findings
- Distribution and planner optimum from learned dynamics: DeepHAM solves the constrained-efficiency problem as easily as the competitive equilibrium, used to study optimal policy [V] — [arXiv](https://arxiv.org/abs/2112.14377); Gu et al. solve a dynamic spatial model (Bilal 2023) by the discrete-state master equation [V] — [arXiv HTML](https://arxiv.org/html/2406.13726v1).
- Distribution read from a kinetic exchange model and controlled toward lower variance (taxation and redistribution) [S] — [Düring and Pareschi](https://ar5iv.labs.arxiv.org/html/1803.02171); stationary distribution validated against 27 years of SCF data (average error below 0.16%) [S] — [Li, Boghosian and Li](https://arxiv.org/abs/1604.02370v2).
- Equality and productivity read from learned agents: AI Economist, 16% better trade-off than baselines in simulation [S] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC9067926).
- Latent state with learned dynamics, evaluated by forecasts: Tacchella et al. (fitness–GDP plane; errors uncorrelated with the IMF's) [S] — [Uniroma](https://iris.uniroma1.it/handle/11573/1186189); Brummitt et al. (diversification precedes growth; convergence to intermediate income and specialisation) [S] — [arXiv](https://arxiv.org/abs/1812.03534v1).
- Graph-learned macro dynamics with counterfactuals: coupled GDP dynamics of the ten largest economies with gradient-based counterfactual policy analysis, no quantitative held-out macro result in the abstract [V] — [arXiv](https://arxiv.org/abs/2512.05764).
- Whole-economy ABMs as twins: Austria (millions of agents, all ESA 2010 activities), competitive with VAR and DSGE out of sample [S] — [IIASA](https://pure.iiasa.ac.at/18339); 38 OECD countries, better than the original and AR(1) [V] — [arXiv](https://arxiv.org/abs/2409.18760); a data-driven ABM of the Italian economy evaluated against macroeconometric benchmarks (*Economic Modelling* 2025) [S] — [search record](https://u-pad.unimc.it/bitstream/11393/353890/1/Ec%20Mod%202025.pdf).
- A payment-system digital twin: León, Moreno and Soramäki built an ABM of Spain's retail payment ecosystem to test digital-euro adoption scenarios [S] — [De Gruyter](https://www.degruyterbrill.com/document/doi/10.1515/jbnst-2024-0002/html).
- A critique: agent-based emergence alone does not validate a model; empirical validation has been described (Fagiolo, Moneta and Windrum) as a central weakness of agent-based economics [S] — [arXiv 2606.19214](https://arxiv.org/pdf/2606.19214).
- Urban digital twins: Lei, Janssen, Stoter and Biljecki's systematic review and Delphi survey finds 14 technical and 9 non-technical challenges, social and legal issues often overlooked; no review tallying validation evidence for urban twins was found [S] — [TU Delft repository](https://repository.tudelft.nl/record/uuid:a07b18d6-e17b-4a4b-8358-048d225fb74c).
- Payment networks analysed as networks (Fedwire, UK, SWIFT), large-value connectedness more segmented than retail in Indonesia [S] — [Emerald](https://www.emeraldinsight.com/insight/content/doi/10.1108/SEF-10-2018-0310/full/html).

#### Inferences
- A complete pipeline "agents in a learned latent space, under learned dynamics, from which circulation, inequality or fragility are read" does not exist as one validated system in the literature found. It can be assembled from parts with different evidence: (a) an embedding (identified up to a group), (b) a learned law on the embedding or on a density (validated by held-out forecasts), (c) a reading (a distribution's Gini, a circulation by Hodge decomposition of flows, a stability indicator) whose validity depends on (a) and (b) being right in the region the reading visits.
- Circulation (cycles of value on a network) is a reading that no learned-simulator paper found here reports; it is a natural addition where flows are simulated on a graph, since the Hodge decomposition of a flow into gradient, local and harmonic parts is model-free.
- Stability and fragility read from learned dynamics (eigenvalues of a learned Jacobian, Koopman spectra, early-warning indicators) are least validated: TDA early warning (Gidea–Katz) is retrospective on two crashes; Koopman bifurcation detection is shown on logistics ABMs.
- "Digital twin of an economy" in the found literature means a data-initialised ABM forecasting aggregates, sometimes beating benchmarks; the claim that it is a valid counterfactual engine rests on the plausibility of behavioural rules, not on held-out evidence.

#### Gaps
- No study was found that learns dynamics in an embedding of firms or households and reads inequality or circulation from it with out-of-sample checks.
- No quantitative validation summary for urban digital twins was found.
- The Italian ABM's forecast results and the payment twin's validation were not read.

### 5. Open Code and Data to Build On

#### Takeaway
Code exists for the core solvers (Moll's HACT codes, DeepHAM, Maliar et al.'s TensorFlow, the graph network simulator), for supply-chain simulation (SupplySim), for data-driven ABMs (the platform of Wiese et al.), and for TDA (giotto-tda); data exist for trade (Ward et al.'s replication on Dataverse; product-level exports behind the Atlas of Economic Complexity), wealth (US SCF), and national accounts and input–output tables. Much of the general tooling (latentnet, PySINDy, PyDMD, Tigramite, torchdiffeq) is known from memory only.

#### Cited Findings
- HACT: Achdou et al.'s codes for continuous-time heterogeneous-agent models [S] — [benjaminmoll.com/HACT](https://benjaminmoll.com/HACT).
- DeepHAM replication code on GitHub (frankhan91/DeepHAM), journal package on Zenodo [S] — [Quantitative Economics](https://www.econometricsociety.org/doi/10.3982/QE2190).
- Maliar, Maliar and Winant provide TensorFlow code for several applications [S] — [EconPapers](https://econpapers.repec.org/RePEc:eee:moneco:v:122:y:2021:i:c:p:76-101).
- Ward, Ahlquist and Rozenas's replication data: Harvard Dataverse DOI 10.7910/DVN/8BECCS [S] — [Cambridge Core](https://www.cambridge.org/core/journals/network-science/article/gravitys-rainbow-a-dynamic-latent-space-model-for-the-world-trade-network/A452C408399F331A19B59B2D7D8FA031).
- SupplySim, an open supply-chain simulator with temporal transaction graphs [S] — [arXiv 2407.18772](https://arxiv.org/html/2407.18772v1).
- Wiese et al.'s platform for constructing, running and evaluating alternative macro ABMs (repository link not on the abstract page) [V] — [arXiv](https://arxiv.org/abs/2409.18760).
- Giotto-TDA tutorial implementing Gidea–Katz (practitioner write-up, not peer reviewed) [S] — [Towards Data Science](https://towardsdatascience.com/detecting-stock-market-crashes-with-topological-data-analysis-7d5dd98abe42).
- US Survey of Consumer Finances, 27 years used by the Affine Wealth Model [S] — [arXiv 1604.02370](https://arxiv.org/abs/1604.02370v2).
- Known from memory, not checked: R `latentnet` (Krivitsky and Handcock) for latent space models; the graph network simulator code in DeepMind's repositories; `PySINDy`; `PyDMD`; `Tigramite` (PCMCI, Runge et al. 2019 *Science Advances*); `torchdiffeq` and Julia's DiffEqFlux for neural ODEs and UDEs; Salesforce's `ai-economist` (Foundation) environment; the Atlas of Economic Complexity's HS/SITC export data; CEPII's BACI trade data; OECD ICIO input–output tables [M].

#### Inferences
- The most reusable assets for an infrastructure that owns its implementations are the *benchmarks and data*, not the code: Krusell–Smith and Aiyagari (with HACT's finite-difference solutions as reference values), Brock–Hommes and Islands as ABM calibration benchmarks, Schelling as a GNN-surrogate benchmark, the BACI or Atlas export matrix for embeddings and fitness, the SCF for wealth distributions, ICIO tables for production networks.

#### Gaps
- Code availability for the MFESN papers, the ABM-informed networks, the Graph Diffusion Networks and the Poledna et al. model was not confirmed.
- Licences of the listed code and data were not checked.

### 6. What This Means for the Infrastructure

#### Takeaway
Build first what the infrastructure's foundations already almost hold, and what can be checked against closed forms or reference solutions: (1) the HJB–Kolmogorov forward system and kinetic Fokker–Planck models of a density on kinesis's balance laws on graphs, held to HACT's finite-difference solutions and to closed-form stationary states; (2) latent space network models with their identifiability group made explicit (tyche, with morphe's graphs); (3) surrogate-assisted evaluation of agent-based models by active learning with calibrated uncertainty (heuresis's ask–tell and Bayesian optimisation over tyche's uncertainty), held to the true simulator; (4) learned laws on low-dimensional states and transfer/Koopman operators of simulated ensembles (kinesis's sparse discovery, linear operators and transfer operator on a partition). As evaluators of allocations whose order matters, these give three different things: a declared density law gives exact consequences of a sequence of interventions under its assumptions; a learned operator gives non-commuting compositions whose commutator measures order dependence, within the region it was trained on; a surrogate gives fast rankings with an uncertainty that must be checked against the simulator before any allocation is chosen on it.

#### Cited Findings
- The foundations named here hold, per the repository's own description: kinesis's `balance` (balance laws of a density on a graph of cells, run and learned from a few snapshots), the transfer operator on a partition, sparse discovery and its weak form, templates filled from records, linear operators and a forecaster with no law, candidates ranked on a part kept aside, `scales` and `transitions`; tyche's `bayes`, `uncertainty` (Latin hypercubes, Sobol' indices), `latent`, `networks`, `learning`, `decision` (POMDPs) and conformal intervals in `spatial`; heuresis's ask–tell protocol, Bayesian optimisation, surrogates and `evolution`; nomos's `agents` (agent-based models with stated schedules), `interaction` and `microsimulation`; physis's master equations, lumpability and Doi–Peliti operators; the economics domain's `circulation` (decomposition of flows on a network) and the markets domain's `procedure` (what the order of steps does apart from the outcome) — repository CLAUDE.md, §1 and §3.1 (in the working tree).
- Deep global methods "tame" and do not remove the curse of dimensionality; sampling the distribution is the hardest part; networks can find "cheat solutions" [V] — [Gu et al.](https://arxiv.org/html/2406.13726v1); [S] — [Fernández-Villaverde et al.](https://www.bde.es/wbe/en/publicaciones/analisis-economico-investigacion/documentos-trabajo/taming-the-curse-of-dimensionality-quantitative-economics-with-deep-learning.html).
- Surrogate positives should be verified in the true model; surrogate uncertainty in the literature is figure bands over repetitions [V] — [Lamperti et al.](https://ar5iv.arxiv.org/html/1703.10639).
- Koopman-reduced models of ABMs agree with analytical coarse-grainings only for many agents [V] — [Niemann et al.](https://arxiv.org/abs/2012.07718).
- Preferences learned from behaviour are not identified (many rewards rationalise one policy) [V] — [Rawat and Rust](https://arxiv.org/abs/2608.24362).
- Embedding positions are identified only up to O(d) or O(p, q); distance-based procedures fail under indefinite signatures [S] — [Athreya et al.](https://jmlr.org/papers/v18/17-448.html); [Rubin-Delanchy et al.](https://arxiv.org/pdf/1709.05506).
- Movement in the product space is local, and reaching the core needs rare long jumps [S] — [Hidalgo et al.](https://arxiv.org/abs/0708.2090).

#### Inferences

**6.1 What to build first, and on which foundation.**
1. *Densities as balance laws (kinesis `balance`, physis, calx).* The continuous-time heterogeneous-agent model is an HJB equation coupled to a Kolmogorov forward equation for the density g(a, z); Achdou et al.'s upwind finite-difference scheme makes the forward operator the adjoint (transpose) of the HJB's discretised generator, a conservative Markov generator on a grid. That is exactly a balance law of a density on a graph of cells with a drift given by the policy. Building it there gives: stationary distributions and transitions; held to HACT's reference solutions and to Achdou et al.'s closed form for two income types [M for the reference values]. Kinetic exchange models (Boltzmann collisions, their Fokker–Planck limits, the Affine Wealth Model) are a second family on the same machinery, held to their explicit stationary states. Both then serve as declared evaluators of redistribution and allocation policies with exact accounting (mass conserved, wealth conserved where the model says).
2. *Latent space network models (tyche `latent`/`networks`, morphe graphs).* Hoff's distance and projection models, the RDPG's adjacency spectral embedding, and a dynamic version with Procrustes gauge-fixing; the identifiability group declared as data, and every reported quantity checked for invariance under it (distances, inner products, curvature, sign-free orderings). The ECI as a relaxed Normalized Cut (Mealy et al.) is a two-line corollary of the spectral embedding and should be implemented as such, not as a separate "complexity" index.
3. *Surrogate-assisted evaluation (heuresis + tyche).* Lamperti et al.'s active-learning loop is an ask–tell search whose objective is a calibration criterion; heuresis already separates the asking from the evaluating. The improvement the literature lacks is calibrated uncertainty: conformal or Bayesian predictive intervals with checked coverage on held-out true-model runs, and an explicit rule that any allocation chosen on the surrogate is re-evaluated in the declared model or the ABM.
4. *Learned laws and operators on low-dimensional states (kinesis).* Sparse discovery on aggregates of ABM output or real series; the transfer operator on a partition (Ulam's method) or EDMD on ensembles of simulated trajectories; analogue forecasting in a two-dimensional state space (the fitness–GDP construction) as a law-free baseline. Each is ranked on a part kept aside, as kinesis already does for candidate laws.
5. *Later:* GNN surrogates of agent behaviour (Cozzi–Pangallo) once nomos's ABMs produce branching traces; simulation-based inference with GNN summary networks (Dyer et al.) on tyche's learning; IRL/DDC with the identified set reported, on tyche's decision module.

**6.2 How they serve as evaluators of allocations whose order matters.**
- *Why order matters in these models.* In a density model with a state-dependent policy, an allocation (a transfer, an investment, a capability given to some agents) changes the density, which changes prices or the mean field, which changes every later agent's policy; two allocations applied in different orders give different terminal densities whenever the induced maps on densities do not commute. In the product space, capability acquisition is path-dependent because only nearby products are reachable (Hidalgo et al.); the order of upgrades decides which later upgrades are cheap.
- *Declared density law as evaluator.* Composing the solved flow maps Φ_{a2} ∘ Φ_{a1} versus Φ_{a1} ∘ Φ_{a2} gives the exact effect of order under the declared model, with conservation checked; the difference of terminal readings (Gini, mass below a line, aggregate output) is the order effect. This can establish what the model implies; it cannot establish that the economy behaves so.
- *Learned linear operator as evaluator.* If simulated trajectories under each allocation are summarised by a transfer or Koopman operator K_a on an observable space, the order effect of a sequence is read from K_{a2}K_{a1} − K_{a1}K_{a2}, a commutator; a small commutator norm, on the region the data visited, means order is immaterial there. This can establish order dependence of the simulator within its training support; it cannot extrapolate outside it, and its reliability needs many agents (Niemann et al.).
- *Surrogate as evaluator in an evolutionary search over sequences.* A surrogate trained on (sequence, outcome) pairs from the simulator ranks candidate sequences cheaply; heuresis's search over permutations or schedules uses it as a pre-screen, with positives re-run in the simulator. It can establish a ranking within the surrogate's validated region with stated error; it cannot be the final judge.
- *Learned agents (IRL, MARL) as evaluator.* An allocation's effect on behaviour learned from data holds across the identified set of preferences only if it is robust over that set; report it as a dual verdict (holds under all rationalising preferences, under some, under none), which kybernesis's dual verdicts already support in form.

**6.3 What each method can and cannot establish.**
- *Latent space models:* can establish relatedness, hierarchy (curvature), clusters and low-dimensional orderings that predict held-out ties; cannot give meaning to individual coordinates, and dynamics read from aligned embeddings depend on the alignment rule.
- *Mean-field and kinetic densities:* can establish stationary distributions, uniqueness (under stated conditions), the exact consequences of interventions and their order under the model, and planner optima; cannot represent identities, specific network ties or small numbers of large agents; their fit to a distribution does not discriminate among mechanisms producing the same shape.
- *Deep solvers of heterogeneous-agent models:* can solve models with aggregate shocks globally with residual-based accuracy checks; cannot certify accuracy outside the sampled region of distributions, and may converge to spurious solutions.
- *Surrogates and GNN emulators:* can reproduce a simulator's input–output map or agents' behaviour within the training distribution, fast; cannot establish anything about reality beyond what the simulator does, and their reported uncertainty is not yet calibrated in the literature.
- *Equation discovery (SINDy, UDEs) and operators (Koopman, DMD, reservoirs):* can give parsimonious candidate laws, modes and forecasts ranked on held-out data; cannot identify causal mechanisms without interventions, and macro series are short and non-stationary.
- *Data-driven ABMs ("digital twins"):* can forecast aggregates out of sample, in published cases better than AR(1), VAR and sometimes DSGE; cannot validate counterfactual policy effects by forecasting alone.
- *Causal discovery, TDA, graph signal processing:* can screen structure (candidate causal graphs under stated assumptions, topological change before crashes, frequency content of flows on a graph); cannot establish mechanisms, and TDA's early-warning evidence is retrospective on few events.

#### Gaps
- The reference values for HACT's benchmark solutions (and for Achdou et al.'s two-type closed form) were not retrieved; they must be generated by running the authors' codes, as the repository's conventions require for fixtures.
- Whether kinesis's `balance` admits a drift that depends on a solved policy (an HJB coupled to the forward equation) rather than a fixed or learned drift was not checked against the code; the repository description says diffusion, crowding and drift, with growth.
- No literature was found that evaluates allocation *sequences* (order effects) with learned simulators; the commutator reading in 6.2 is a proposal, not an established method.

## Part VIII. Learning Classical Dynamical-System Models of Economies from Observed Data

*Remark.* Part VIII's scope and conventions. Scope: how laws of motion for aggregate, sectoral and distributional economic dynamics are learned from records, with their uncertainty and their validity out of sample. Agent-level models are left to Parts VI and VII. Written for a dynamical-systems foundation (kinesis) that already declares flows, maps, SDEs and systems with events; fits templates by least squares and Levenberg–Marquardt; discovers sparse laws (SINDy, weak SINDy); learns balance laws of densities on graphs; estimates transfer operators; and detects critical transitions.

Markers:
- **[V]**: verified by reading the source itself during this research. "[V abstract]" means only the abstract was read; "[V text]" means the full text was read.
- **[S]**: from a search snippet or a secondary summary only.
- **[M]**: from memory, not checked during this research. "[M; DOI checked]" means the reference exists as cited (title, journal, year, authors, volume and pages checked against Crossref's metadata), but the content claim was not checked.

Full texts read: Barnett and Serletis (1998 working paper of the 2000 JEDC article) and LeBaron (1994 working paper, its introduction and macroeconomics section). arXiv abstracts were read through the arXiv API. About 50 tool calls; researched 2026-11-20.

### 1. Methods That Learn an Economic Law of Motion (Not Just a Forecast) from Data, and the Evidence That They Recover Something Real Out of Sample

#### Takeaway
The econometric tradition learns laws of motion in three ways:
- **Linear reduced forms with identified shocks:** VAR/SVAR, local projections (LP), dynamic factor models.
- **Linear-Gaussian state-space likelihoods:** Kalman filtering, used for nowcasting and for estimating linearised DSGE models.
- **Explicit regime processes:** Markov switching, time-varying parameters.

The evidence that these recover something real out of sample is modest:
- Structural (DSGE) and statistical models forecast about equally badly in absolute terms since the Great Moderation.
- Nowcasting with factor models is the clearest operational success.
- Nonlinear machine-learning forecasts help mainly in episodes of stress.
- The strongest out-of-sample claim for an explicitly *dynamical* law learned from data is the two-variable fitness–GDP velocity field of Tacchella et al. (2018), and it is a nonparametric analogue method, not an equation.

Newer flexible learners (GP-VARs, echo-state networks, neural ODEs/UDEs) are benchmarked mostly as forecasters. A 2026 cross-country stress test found that physics-style structural priors (PINN, UDE) did *worse* than ARIMA and plain neural ODEs on sparse annual macro data.

#### Cited Findings

**1.1 VARs, structural VARs and local projections**
- **Sims (1980) and the VAR.** Sims proposed unrestricted vector autoregressions as an alternative to the "incredible" identifying restrictions of large macro-econometric models. This founded the reduced-form-plus-identified-shocks tradition [M; DOI checked] — [Sims 1980, *Econometrica* 48(1):1–48, doi:10.2307/1912017](https://doi.org/10.2307/1912017).
- **The Handbook surveys.** The standard modern surveys of identification and propagation are the Handbook of Macroeconomics vol. 2 chapters: Stock and Watson on dynamic factor models, FAVARs and SVARs (pp. 415–525), and Ramey on shocks and their propagation (pp. 71–162) [M; DOI checked] — [Stock & Watson 2016, doi:10.1016/bs.hesmac.2016.04.002](https://doi.org/10.1016/bs.hesmac.2016.04.002); [Ramey 2016, doi:10.1016/bs.hesmac.2016.03.003](https://doi.org/10.1016/bs.hesmac.2016.03.003).
- **Local projections.** Jordà (2005) estimates impulse responses by a separate regression at each horizon, with no law of motion imposed [M; DOI checked] — [Jordà 2005, *AER* 95(1), doi:10.1257/0002828053828518](https://doi.org/10.1257/0002828053828518).
- **In population, LPs and VARs agree.** With unrestricted lag lengths, local projections and VARs estimate the same impulse responses. They differ only in finite-sample bias and variance [M; DOI checked] — [Plagborg-Møller & Wolf 2021, *Econometrica* 89(2), doi:10.3982/ECTA17813](https://doi.org/10.3982/ECTA17813).
- **In samples, a bias–variance trade-off.** A simulation over thousands of data-generating processes calibrated to US data shows a clear trade-off. LPs have lower bias but much higher variance at intermediate and long horizons. Bias-corrected LP is preferred only if bias is all that matters. Otherwise Bayesian VARs are preferred at short and long horizons, and least-squares VARs at intermediate and long horizons [V abstract]. The DGPs come from an estimated dynamic factor model, so none is exactly a finite-order VAR [S] — [Li, Plagborg-Møller & Wolf, arXiv:2104.00655 (v4, Jan 2024)](https://arxiv.org/abs/2104.00655). Published in the *Journal of Econometrics* (2024) according to a search snippet [S]; Crossref returned only the NBER and SSRN versions (doi:10.3386/w30207).

**1.2 State space, the Kalman filter, dynamic factor models and nowcasting**
- **The Kalman filter** is the recursive linear-Gaussian filter. It gives the exact likelihood of a linear state-space model, and so the estimation engine for every linear latent-state model in this tradition [M; DOI checked] — [Kalman 1960, *J. Basic Eng.* 82:35–45, doi:10.1115/1.3662552](https://doi.org/10.1115/1.3662552).
- **Diffusion indexes.** A few principal-component factors from many series improve macro forecasts [M; DOI checked] — [Stock & Watson 2002, *JBES* 20:147–162, doi:10.1198/073500102317351921](https://doi.org/10.1198/073500102317351921).
- **Nowcasting** with a dynamic factor model in state-space form, updated as each data release arrives (the model's precision improves as information accumulates within the quarter) [M; DOI checked] — [Giannone, Reichlin & Small 2008, *JME*, doi:10.1016/j.jmoneco.2008.05.010](https://doi.org/10.1016/j.jmoneco.2008.05.010). The Handbook survey is [Bańbura, Giannone, Modugno & Reichlin 2013, *Handbook of Economic Forecasting* 2A:195–237, doi:10.1016/B978-0-444-53683-9.00004-9](https://doi.org/10.1016/b978-0-444-53683-9.00004-9) [M; DOI checked].
- **Reservoir computing as a nowcaster.** A Multi-Frequency Echo State Network matched or beat MIDAS and dynamic factor models in multistep forecasts of US GDP growth. It takes many series at far lower computational cost [V abstract] — [Ballarin et al. 2024, *IJF* 40(3):1206–1237, doi:10.1016/j.ijforecast.2023.10.009; arXiv:2211.00363](https://arxiv.org/abs/2211.00363).

**1.3 Regime switching and drifting parameters**
- **Markov switching.** Hamilton's model lets the mean growth of US real GNP switch between two states following a hidden Markov chain. The inferred recession probabilities line up with NBER dates [M; DOI checked] — [Hamilton 1989, *Econometrica* 57(2):357–384, doi:10.2307/1912559](https://doi.org/10.2307/1912559). The Handbook survey is [Hamilton 2016, "Macroeconomic Regimes and Regime Shifts", doi:10.1016/bs.hesmac.2016.03.004](https://doi.org/10.1016/bs.hesmac.2016.03.004) [M; DOI checked].
- **Nonlinearity found by fitting.** Fitting explicitly nonlinear specifications to macro data has been "generally more successful at finding strong evidence for nonlinearities" than diagnostic testing. LeBaron cites Hamilton (1989), Beaudry–Koop, Neftci, Potter and Teräsvirta–Anderson; this work supports asymmetry between expansions and contractions and between positive and negative shocks [V text] — [LeBaron 1994, "Chaos and Nonlinear Forecastability in Economics and Finance", working paper](https://econwpa.ub.uni-muenchen.de/econ-wp/fin/papers/9411/9411001.pdf).
- **Time-varying parameters.** Time-varying-parameter VARs with stochastic volatility let coefficients drift as random walks [M; DOI checked] — [Primiceri 2005, *REStud* 72:821–852, doi:10.1111/j.1467-937X.2005.00353.x](https://doi.org/10.1111/j.1467-937X.2005.00353.x); [Cogley & Sargent 2005, *RED* 8:262–302, doi:10.1016/j.red.2004.10.009](https://doi.org/10.1016/j.red.2004.10.009).
- **Switching variances, not rules.** For US monetary policy, Sims and Zha found that the best-fitting regime-switching model switches the shock variances rather than the policy rule [M; DOI checked] — [Sims & Zha 2006, *AER* 96(1):54–81, doi:10.1257/000282806776157678](https://doi.org/10.1257/000282806776157678).

**1.4 Estimating DSGE models by likelihood and Bayesian methods**
- **The Bayesian toolkit.** An and Schorfheide set out the standard Bayesian estimation and evaluation of linearised DSGE models: Kalman-filter likelihood, random-walk Metropolis–Hastings, marginal data densities, comparison with DSGE-VARs [M; DOI checked] — [An & Schorfheide 2007, *Econometric Reviews* 26:113–172, doi:10.1080/07474930701220071](https://doi.org/10.1080/07474930701220071).
- **The Handbook chapter on solution and estimation**: perturbation and projection solutions, likelihood by Kalman and particle filters, Bayesian computation [M; DOI checked] — [Fernández-Villaverde, Rubio-Ramírez & Schorfheide 2016, Handbook of Macroeconomics vol. 2, doi:10.1016/bs.hesmac.2016.03.006](https://doi.org/10.1016/bs.hesmac.2016.03.006).
- **Nonlinear DSGE likelihood** by the particle filter [M; DOI checked] — [Fernández-Villaverde & Rubio-Ramírez 2007, *REStud* 74:1059–1087, doi:10.1111/j.1467-937x.2007.00437.x](https://doi.org/10.1111/j.1467-937x.2007.00437.x).
- **Smets and Wouters.** A medium-scale New Keynesian model estimated by Bayesian methods, reported as competitive with Bayesian VARs in forecasting [M; DOI checked] — [Smets & Wouters 2007, *AER* 97(3):586–606, doi:10.1257/aer.97.3.586](https://doi.org/10.1257/aer.97.3.586).
- **The absolute record is poor.** Evaluated in absolute terms, the benchmark medium-scale DSGE forecasts inflation and GDP growth very poorly. Statistical and judgmental forecasts do equally poorly. The authors argue that forecasting ability during the Great Moderation is not a good metric for judging models, and that a "wrong" model could show the same pattern [S] — [Edge & Gürkaynak 2010, *Brookings Papers on Economic Activity* Fall 2010:209–259](https://www.brookings.edu/articles/how-useful-are-estimated-dsge-model-forecasts-for-central-bankers-with-comments-and-discussion).

**1.5 Data-driven laws with explicit out-of-sample tests**
- **The fitness–GDP velocity field.** Tacchella, Mazzilli and Pietronero model growth as a two-dimensional dynamical system in GDP per capita and "fitness", a complexity measure computed from product-level exports, and forecast by analogues. They report beating the IMF's five-year GDP forecasts by more than 25% on the available data, with errors uncorrelated with the IMF's [S] — [Tacchella, Mazzilli & Pietronero 2018, *Nature Physics* 14:861–865, doi:10.1038/s41567-018-0204-y](https://doi.org/10.1038/s41567-018-0204-y) (DOI checked). The World Bank/IFC working-paper version describes the "selective predictability scheme" as a two-dimensional kernel regression in fitness and GDP per capita. Critics noted that the IMF benchmark is opaque, and that the gain may partly reflect removing IMF biases [S] — [World Bank Open Knowledge](https://openknowledge.worldbank.org/server/api/core/bitstreams/b4a274b3-7d57-5477-aa88-fc76f4c4cf1f/content); [Cosmos coverage](https://cosmosmagazine.com/physics/physicists-barge-in-on-economists-predictions-ensue).
- **A follow-up on technological fitness** in the same forecasting frame exists [S] — [Angelini, Gabrielli, Tacchella, Zaccaria 2024, *Chaos, Solitons & Fractals* 184:115006, doi:10.1016/j.chaos.2024.115006](https://doi.org/10.1016/j.chaos.2024.115006).
- **Where machine learning helps.** Of four features of ML (nonlinearity, regularisation, cross-validation, loss function), "nonlinearity is the true game changer for macroeconomic prediction". Its gains are associated with high macroeconomic uncertainty, financial stress and housing-bubble bursts. The standard factor model remains the best regularisation, and L2 loss is preferred [V abstract] — [Goulet Coulombe, Leroux, Stevanovic & Surprenant 2022, *JAE* 37(5):920–964, doi:10.1002/jae.2910](https://doi.org/10.1002/jae.2910); [arXiv:2008.12477](https://arxiv.org/abs/2008.12477).
- **Random forests for inflation.** Random forests improve US inflation forecasts in a data-rich setting [M; DOI checked] — [Medeiros, Vasconcelos, Veiga & Zilberman 2021, *JBES* 39:98–119, doi:10.1080/07350015.2019.1637745](https://doi.org/10.1080/07350015.2019.1637745).
- **STAR and neural networks.** Smooth-transition autoregressions and neural networks were compared with linear AR models on G7 macro series; nonlinear models help for some series and horizons, mainly in combination [M; DOI checked] — [Teräsvirta, van Dijk & Medeiros 2005, *IJF* 21:755–774, doi:10.1016/j.ijforecast.2005.04.010](https://doi.org/10.1016/j.ijforecast.2005.04.010).
- **GP-VARs.** A Gaussian-process prior on the conditional-mean function of a VAR, with stochastic volatility, estimated by scalable MCMC. Applied to the asymmetric transmission of uncertainty shocks, and evaluated in a US forecasting exercise [S] — [Hauzenberger, Huber, Marcellino & Petz, *JBES* 43(1):27–43 (2025; online 2024), doi:10.1080/07350015.2024.2322089](https://doi.org/10.1080/07350015.2024.2322089) (DOI checked); [arXiv:2112.01995](https://arxiv.org/abs/2112.01995).
- **Neural ODEs, PINNs and UDEs on macro data.** A 2026 diagnostic study compared ARIMA, LSTM, neural ODE, PINN and UDE across 23 countries on sparse annual macro data, with several temporal splits and five seeds. "None of the evaluated models achieve consistently strong forecasting performance". Less-constrained models (ARIMA, neural ODE) consistently beat the structurally constrained PINN and UDE. Structural priors "can act as misregularizers" when they do not match the data-generating process. The failure modes named are prior misalignment, regime shifts, structural breaks and optimisation instability [V abstract] — [Anand et al., "SciML in the Wild", arXiv:2607.09684](https://arxiv.org/abs/2607.09684).
- **Neural ODEs elsewhere in economics.** A latent neural ODE fitted to poverty indicators in Odisha, 2007–2020, claims only that it "reproduces the observed data with high accuracy". No out-of-sample test is in the abstract [V abstract] — [Samota, Chakraverty & Sethi, arXiv:2604.00632](https://arxiv.org/abs/2604.00632). Agent-based-model-informed neural networks (restricted graph networks keeping mass conservation and locality) learn coupled GDP dynamics of the ten largest economies and are used for counterfactuals. Out-of-sample superiority is shown only for the synthetic SIR case [V abstract] — [Antulov-Fantulin, arXiv:2512.05764](https://arxiv.org/abs/2512.05764).
- **Universal differential equations** are defined as mechanistic differential equations with embedded universal approximators (the SciML formalism) [V abstract] — [Rackauckas et al., arXiv:2001.04385](https://arxiv.org/abs/2001.04385). Neural ODEs were introduced in [Chen et al., arXiv:1806.07366](https://arxiv.org/abs/1806.07366) [V abstract].

**1.6 What the tradition assumes about the dynamics**
- **VARs and LPs** assume linear, time-invariant conditional means and stationarity (or cointegration). Structural VARs further assume that the structural shocks can be recovered from current and past observables (invertibility). LPs relax the law of motion but not linearity in shocks [M].
- **Linearised DSGE models** assume rational expectations, a unique stable solution (determinacy), and dynamics near a steady state. The Kalman filter assumes Gaussian shocks [M] — see [FV–RR–S 2016](https://doi.org/10.1016/bs.hesmac.2016.03.006).
- **Regime models** make discrete change explicit, but assume a fixed small number of recurring regimes [M].

#### Inferences
- **Few methods yield a law with parameters or terms.** Only a minority give a law whose terms can be read and transplanted. Linear-Gaussian state-space models with structural restrictions (DSGE) do, as do regime-switching models with interpretable regimes and low-dimensional analogue fields (Tacchella et al.). Most "learned dynamics" papers in economics validate forecasts, not laws.
- **A law is credible when it beats the right benchmarks out of sample.** The benchmarks are a random walk, an AR or BVAR, a factor model and forecast combinations. It should also reproduce model-free objects (LP impulse responses) that it was not fitted to. No surveyed method routinely does both.
- **Structure can hurt.** The 2026 SciML result and the poor absolute DSGE record together say this: imposed structure helps only if it is right, and the record often cannot tell. A learned law should therefore always carry a less-constrained comparator.

#### Gaps
- I found no study that evaluated the *recovered structure* of a learned macro law out of sample (as opposed to point forecasts), for example by checking a learned SINDy or neural ODE law's implied impulse responses against LPs on later data.
- The published version of Li–Plagborg-Møller–Wolf, and the details of Edge–Gürkaynak and Tacchella et al., come from snippets only.

### 2. Equation Discovery (SINDy, Symbolic Regression, Koopman and DMD) Has Produced No Credible Economic Law, and Where It Has Failed

#### Takeaway
I found no peer-reviewed case in which SINDy, symbolic regression or Koopman/DMD discovered an economic law from real aggregate data that was then confirmed out of sample. The successes are recoveries of *known* models from *synthetic* data:
- the Goodwin cycle by SINDy;
- the Solow–Swan equation by OccamNet;
- EDMD on a simulated Samuelson–Hicks–Goodwin–Puu model.

Where real data are used, results range from descriptive to negative:
- DMD extracts cycles from stock valuations and makes trading signals.
- The one real-data SINDy/UDE attempt on US Goodwin variables (1948–2025) found no interaction term at any regularisation strength.

The failure modes are those the chaos debate already exposed: short samples, measurement noise, nonstationarity and structural breaks.

#### Cited Findings

**2.1 SINDy**
- **The method.** SINDy is sparse regression of time derivatives on a library of candidate terms [M; DOI checked] — [Brunton, Proctor & Kutz 2016, *PNAS* 113:3932, doi:10.1073/pnas.1517384113](https://doi.org/10.1073/pnas.1517384113).
- **Variants for noise and scarce data.** Weak (Galerkin) SINDy — [Messenger & Bortz 2021, *Multiscale Model. Simul.* 19:1474–1497, doi:10.1137/20M1343166](https://doi.org/10.1137/20M1343166) [M; DOI checked]. Ensemble-SINDy with bootstrapped inclusion probabilities, for the low-data, high-noise limit — [Fasel, Kutz, Brunton & Brunton 2022, *Proc. R. Soc. A* 478, doi:10.1098/rspa.2021.0904](https://doi.org/10.1098/rspa.2021.0904) [M; DOI checked].
- **Conformal prediction with Ensemble-SINDy** gives target coverage for forecasts, feature-importance-based model selection, and more robust coefficient intervals under non-Gaussian noise. It was demonstrated on predator–prey and chaotic systems, not economic data [V abstract] — [Fasel 2025, arXiv:2507.11739](https://arxiv.org/abs/2507.11739).
- **The one direct macro test.** SINDy and a SINDy-plus-UDE pipeline were applied to Goodwin's growth cycle (predator–prey between the employment rate and the labour share):
  - On synthetic nonstationary data, SINDy recovered the predator–prey structure.
  - On real US quarterly data, 1948–2025 (employment, capacity utilisation, labour share), neither method recovered the key interaction term at any regularisation strength. Plain SINDy returned dense polynomials, and the hybrid found near-zero neural corrections.

  [S; SSRN preprint, not peer reviewed; SSRN refused automated fetching] — [Khundadze, "Learning Macroeconomic Dynamics from Data: Applications of Universal Differential Equations and SINDy", SSRN 6491442](https://papers.ssrn.com/sol3/papers.cfm?abstract_id=6491442).
- **No other macro applications.** A search for other peer-reviewed SINDy applications to macroeconomic data found none [S] (same search).

**2.2 Symbolic regression**
- **OccamNet on social-science data.** A neuro-symbolic system extends OccamNet to find compact functions and differential equations in noisy longitudinal social-science data, and is shown to "discover interpretable models from real-world data in economics and sociology" [V abstract] — [Balla, Huang, Dugan, Dangovski & Soljačić, *Humanities and Social Sciences Communications* 12 (2025), doi:10.1057/s41599-025-04405-x; arXiv:2210.00563](https://arxiv.org/abs/2210.00563). The Solow–Swan equation is recovered from a *synthetic* dataset. The real-data part maps the accuracy–complexity trade-off of growth models across 18 OECD countries, as a test of how general Solow–Swan is [S] — [arXiv PDF](https://arxiv.org/pdf/2210.00563).
- **Genetic programming on experimental data.** Duffy and Engle-Warnick used genetic programming to infer the strategies subjects played in a repeated ultimatum game. This is behavioural, not aggregate dynamics [S] — [Springer chapter](https://link.springer.com/chapter/10.1007/978-3-7908-1784-3_4).
- **No accepted law.** No source found claims that symbolic regression has produced an accepted new economic law [S] — [search summary; AI-Descartes context, *Nat. Commun.* 2023](https://www.nature.com/articles/s41467-023-37236-y).

**2.3 Koopman operators and DMD**
- **Background.** DMD was introduced by Schmid; EDMD approximates the Koopman operator on a dictionary of observables; the modern review is Brunton et al. [M; DOIs checked] — [Schmid 2010, *JFM* 656:5–28, doi:10.1017/S0022112010001217](https://doi.org/10.1017/S0022112010001217); [Williams, Kevrekidis & Rowley 2015, *J. Nonlinear Sci.* 25:1307–1346, doi:10.1007/s00332-015-9258-5](https://doi.org/10.1007/s00332-015-9258-5); [Brunton, Budišić, Kaiser & Kutz 2022, *SIAM Review* 64:229–340, doi:10.1137/21M1401243](https://doi.org/10.1137/21M1401243).
- **DMD for trading strategies** [M; DOI checked] — [Mann & Kutz 2016, *Quantitative Finance* 16:1643–1655, doi:10.1080/14697688.2016.1170194](https://doi.org/10.1080/14697688.2016.1170194); [arXiv:1508.04487](https://arxiv.org/pdf/1508.04487).
- **Cycles in stock valuations.** Koopman-mode analysis of stock valuations finds cyclic behaviour that persisted through the Great Recession. It separates robust from non-robust features by requiring modes to persist across subgroups of the data [S] — [Hua et al. 2016, *Physica A* 448:172–180](https://www.sciencedirect.com/science/article/abs/pii/S0378437115010870).
- **EDMD for business cycles uses no real data.** Data are generated from a nonlinear model of the Samuelson–Hicks–Goodwin–Puu family [S] — [Leventides, Melas & Poulios 2022, *Data Science in Finance and Economics* 2(2):117–146, doi:10.3934/DSFE.2022006](https://doi.org/10.3934/DSFE.2022006) (DOI checked). A follow-up uses EDMD with Takens embedding to build a linear autoregressive approximation of a chaotic macro model [S] — [DSFE 2(4)](https://aimspress.com/aimspress-data/dsfe/2022/4/PDF/DSFE-02-04-021.pdf).
- **DMD reconstruction error as a regime detector.** It detects transient dynamics and regime changes, demonstrated on Kuramoto–Sivashinsky and atmospheric reanalysis (the North Atlantic Oscillation shift around 1970) [V abstract] — [Gottwald & Gugole 2019, *J. Stat. Phys.*, doi:10.1007/s10955-019-02392-3; arXiv:1904.09082](https://arxiv.org/abs/1904.09082). Caveat: if the transition happens on the same timescale as the equilibrium dynamics, the reconstruction error may not show it [S] (search summary of the same paper).

**2.4 Delay embedding and empirical dynamic modelling (EDM)**
- **Theory.** Delay-coordinate reconstruction rests on Takens's theorem, Embedology, and its generalisation to multivariate embeddings [M; DOIs checked] — [Sauer, Yorke & Casdagli 1991, *J. Stat. Phys.* 65:579–616, doi:10.1007/BF01053745](https://doi.org/10.1007/BF01053745); [Deyle & Sugihara 2011, *PLoS ONE* 6:e18295, doi:10.1371/journal.pone.0018295](https://doi.org/10.1371/journal.pone.0018295).
- **Convergent cross mapping** for causality in weakly coupled deterministic systems [M; DOI checked] — [Sugihara et al. 2012, *Science* 338:496, doi:10.1126/science.1227079](https://doi.org/10.1126/science.1227079).
- **Equation-free forecasting of a fishery** (salmon) by EDM [M; DOI checked] — [Ye et al. 2015, *PNAS* 112, doi:10.1073/pnas.1417063112](https://doi.org/10.1073/pnas.1417063112).
- **In finance.** Transfer entropy and CCM with Fourier-transform surrogates (separating linear from nonlinear causality) find significant *nonlinear* causality among German and US stock indices that correlation misses [V abstract] — [Ma, Prosperino, Haluszczynski & Räth, arXiv:2312.16185 (*Chaos* 2024)](https://arxiv.org/abs/2312.16185). Pairwise CCM on index returns finds nonlinear coupling rising in crises (GFC, COVID-19) [S] — [*J. Risk Financial Manag.* 2025](https://mdpi-res.com/d_attachment/jrfm/jrfm-19-00558/article_deploy/jrfm-19-00558.pdf).
- **CCM under noise.** Noise lowers cross-map skill, while the convergence rate is more robust [S] — [Mønster et al., arXiv:1603.01155](https://pdf.arxiv.org/pdf/1603.01155).

#### Inferences
- **Three reasons it fails on macro aggregates.** (a) A quarterly record of about 300 points over 75 years spans several policy regimes and measurement redefinitions. (b) The candidate laws (Goodwin, Solow) are low-dimensional caricatures of high-dimensional, shock-driven systems, so the "true" sparse law may not exist at the observed scale. (c) Derivative estimation amplifies measurement noise. The Khundadze result fits all three.
- **Real data works only for descriptive objects.** Equation discovery has worked on real economic data only for cycles, modes and couplings in high-frequency financial data, where samples are long. This mirrors the chaos literature's finding that financial data are "more promising" than macro data (§3).
- **What the foundation should claim.** A sparse law from economic data should be reported as one candidate among an ensemble, with inclusion probabilities, a linear comparator and an out-of-sample test. It should never be reported as "the" law.

#### Gaps
- The published version of Balla et al. was not read. Whether its OECD analysis tests out of sample is not established.
- The full text of Khundadze (SSRN) could not be fetched.
- No credible Koopman/DMD forecasting comparison against VARs or random walks on macro data was found.

### 3. The Verdict of the Literature on Low-Dimensional Chaos and Nonlinearity in Economic Series

#### Takeaway
The verdict has been stable since the mid-1990s:
- **Nonlinearity: strong evidence.** Business-cycle asymmetry, regime switching and volatility clustering are well documented.
- **Low-dimensional deterministic chaos: no credible evidence.** Claims for chaos (Divisia monetary aggregates; a few exchange rates) did not survive scrutiny or replication.
- **Nonlinear forecasting adds little out of sample**, except in stress episodes.

Barnett and Serletis add a deeper point: no existing test can locate chaos "within the structure of the economic system" as opposed to chaotic external shocks. They advise against taking a strong position either way.

#### Cited Findings
- **The BDS test** uses the correlation integral and has power against general dependence [M; DOI checked] — [Brock, Dechert, Scheinkman & LeBaron 1996, *Econometric Reviews* 15:197–235, doi:10.1080/07474939608800353](https://doi.org/10.1080/07474939608800353) (Crossref misspells the first author as "Broock").
- **BDS is not a test of chaos.** It tests whiteness (C(N,m,ε) = C(N,1,ε)^m) against general dependence, linear or nonlinear. It "does not provide a direct test for nonlinearity or for chaos", since the sampling distribution under those nulls is unknown. It can give indirect evidence of nonlinear dependence, which is "necessary but not sufficient for chaos" [V text] — [Barnett & Serletis 1998 WP (*JEDC* 24:703–724, 2000, doi:10.1016/S0165-1889(99)00023-8)](https://econwpa.ub.uni-muenchen.de/econ-wp/em/papers/9805/9805003.pdf) (DOI checked).
- **Nonlinearity yes, chaos no consensus.** "There is a broad consensus of support for the proposition that the (macroeconomic) data generating processes are characterized by a pattern of nonlinear dependence, but there is no consensus at all on whether there is chaos." The evidence they cite [V text] — same source:
  - Brock and Sayers (1988), Frank and Stengos (1988), and Frank, Gencay and Stengos (1988) find no chaos in US, Canadian and international macro series.
  - Barnett and Chen's (1988) claim of chaos in US Divisia monetary aggregates drew strong criticism from Ramsey, Sayers and Rothman (1990).
- **The macro null result:** [Brock & Sayers 1988, *JME* 22:71–90, doi:10.1016/0304-3932(88)90170-5](https://doi.org/10.1016/0304-3932(88)90170-5) [content as reported in Barnett–Serletis, V; DOI checked].
- **Why macro results disappoint.** Macro results have "not yet led to particularly encouraging results (mainly due to the small samples and high noise levels for most macroeconomic series)". Financial data, being longer and better measured, show "clear evidence of nonlinear dependence and some evidence of chaos". Two examples: Scheinkman and LeBaron's (1989) weekly CRSP returns; Serletis and Gogas (1997), chaos-consistent results in two of seven East European black-market exchange rates [V text] — same source.
- **High-frequency data.** Studies over short periods (FTSE 100 one- to sixty-minute returns; FTSE, S&P 500, DAX and Nikkei at 15 seconds to 5 minutes) reject independence in favour of nonlinear structure but find "no evidence of low-dimensional chaotic processes" [V text] — same source.
- **A blind competition of tests.** Barnett et al. ran a single-blind controlled competition: five generating processes (chaotic Feigenbaum recursion, GARCH, nonlinear MA, ARCH, ARMA) at sample sizes 380 and 2000; five tests (Hinich bispectrum, BDS, White, Kaplan, NEGM Lyapunov). It showed differing power and, importantly, different null hypotheses (four nulls among five tests). Differences between tests are therefore not just power [V text] — same source; original results in [Barnett et al., robustness to measurement error, inference method and sample size (*JEDC*)](https://pure.psu.edu/en/publications/robustness-of-nonlinearity-and-chaos-tests-to-measurement-error-i/) [S].
- **The conclusion of Barnett and Serletis** [V text] — same source:
  - Dynamic noise "makes it difficult and perhaps impossible to distinguish between (noisy) high-dimensional chaos and pure randomness". Estimates of dimension and Lyapunov exponents are sensitive to dynamic noise.
  - "There have been no published tests of chaos 'within the structure of the economic system', and there is very little chance that any such tests will be available in this field for a very long time." Existing tests cannot tell whether detected chaos comes from the economy or from chaotic external shocks (e.g. the weather).
  - "While there have been many published tests for chaotic nonlinear dynamics, little agreement exists among economists about the correct conclusions."
- **LeBaron's summary.** The search for chaos found "little or no evidence for chaos in any economic time series, but they have turned up a surprising amount of unexplained nonlinear structure". "The direct evidence for deterministic chaos in many economic series remains weak." Two nonlinear-forecasting studies of macro series (Granger et al. 1993; Jaditz and Sayers 1993) find that nonlinear forecasting "does not add much in terms of out of sample forecast performance", which "may suggest some problems in terms of stability/stationarity". Simulated-null (bootstrap) comparisons "helped keep researchers honest" [V text] — [LeBaron 1994](https://econwpa.ub.uni-muenchen.de/econ-wp/fin/papers/9411/9411001.pdf).
- **A later assessment** of chaotic time-series analysis in economics [M; DOI checked] — [Faggini 2014, *Chaos* 24(4), doi:10.1063/1.4903797](https://doi.org/10.1063/1.4903797).
- **The modern echo.** The gains of nonlinear ML forecasts are concentrated in high uncertainty and financial stress [V abstract] — [Goulet Coulombe et al. 2022](https://arxiv.org/abs/2008.12477).

#### Inferences
- **Three lessons for learned laws.**
  - Detecting nonlinearity is easy and says little about which law holds.
  - Distinguishing deterministic structure from dynamic noise is generally impossible at macro sample sizes.
  - Surrogate and simulated-null testing is the field's main safeguard.
- **"No chaos" is not "linear".** The dominant nonlinearity is *stochastic* and *state-dependent*: regimes and conditional heteroskedasticity. Learned economic laws should therefore be stochastic (SDEs, regime-switching maps), not deterministic flows with noise added afterwards.

#### Gaps
- I read the 1998 working paper, not the published 2000 JEDC article. The published conclusions are assumed to match.
- I found no post-2015 systematic re-test of low-dimensional chaos on modern long macro panels.

### 4. How Stochastic Economic Dynamics (Drift, Diffusion, Jumps, Distributions) Are Estimated from Data, and with What Identification Guarantees

#### Takeaway
The approaches by object:
- **Diffusions of single series.** Three tools:
  - closed-form likelihood expansions for discretely sampled diffusions (Aït-Sahalia), which converge to the true MLE;
  - nonparametric drift and diffusion estimators;
  - Kramers–Moyal conditional moments with finite-sampling corrections.
- **Jumps.** Tested and modelled with high-frequency asymptotics and self-exciting processes.
- **Distributions** (income, wealth, firm and city sizes). Estimated as random-growth Fokker–Planck (Kolmogorov forward) models through their stationary Pareto tails and, more demandingly, their *transition speeds*. Gabaix–Lasry–Lions–Moll showed the canonical model's transitions are about an order of magnitude too slow. The newest approach is a functional state-space model of the density jointly with aggregates (Chang–Chen–Schorfheide).

Identification is delicate in three ways:
- Diffusion coefficients are identified from high-frequency data, but drifts need a long span.
- Celebrated nonlinear short-rate drifts were artefacts of finite samples (Chapman–Pearson).
- Kramers–Moyal estimates at finite sampling interval are biased and need corrections that can themselves fake structure.

#### Cited Findings

**4.1 Drift and diffusion of SDEs**
- **Closed-form likelihood expansions.** A Hermite expansion gives an explicit sequence of closed-form functions converging to the true transition density of a discretely sampled diffusion. The resulting estimator converges to the true MLE and is consistent, asymptotically normal and efficient [S] — [Aït-Sahalia 2002, *Econometrica* 70(1):223–262, doi:10.1111/1468-0262.00274](https://doi.org/10.1111/1468-0262.00274) (DOI checked). Extended to multivariate diffusions [S] — [Aït-Sahalia 2008, *Ann. Statist.* 36(2)](https://projecteuclid.org/euclid.aos/1205420523).
- **Nonparametric short-rate models** reported a nonlinear drift (strong mean reversion only at extreme rates) [M; DOIs checked] — [Aït-Sahalia 1996, *RFS* 9:385–426, doi:10.1093/rfs/9.2.385](https://doi.org/10.1093/rfs/9.2.385); [Stanton 1997, *J. Finance* 52:1973–2002, doi:10.1111/j.1540-6261.1997.tb02748.x](https://doi.org/10.1111/j.1540-6261.1997.tb02748.x).
- **The artefact.** Applying those same estimators to simulated paths of a square-root diffusion, whose drift is linear, produced nonlinearities "of the type and magnitude" previously reported. Nonlinearity of the short-rate drift is therefore not a robust stylised fact [S] — [Chapman & Pearson 2000, *J. Finance* 55:355–388, doi:10.1111/0022-1082.00208](https://doi.org/10.1111/0022-1082.00208) (DOI checked).
- **Kramers–Moyal estimation** of drift and diffusion from conditional moments is reviewed in [Friedrich, Peinke, Sahimi & Tabar 2011, *Physics Reports* 506:87–162, doi:10.1016/j.physrep.2011.05.003](https://doi.org/10.1016/j.physrep.2011.05.003) [M; DOI checked]. An economic application: the foreign-exchange market, with a Fokker–Planck equation in *scale* for increments [M; DOI checked] — [Friedrich, Peinke & Renner 2000, *PRL* 84:5224, doi:10.1103/PhysRevLett.84.5224](https://doi.org/10.1103/PhysRevLett.84.5224).
- **Finite sampling distorts Kramers–Moyal estimates** [S] — [Ragwitz & Kantz 2001, *PRL* 87:254501, doi:10.1103/PhysRevLett.87.254501](https://doi.org/10.1103/PhysRevLett.87.254501) (DOI checked).
  - The first correction was first order in the sampling interval and assumed Gaussian finite-time conditionals [S].
  - Friedrich et al. commented that different "correction" terms "may fake large corrections" [S] — [comment, arXiv:physics/0203005](https://arxiv.org/pdf/physics/0203005).
  - Arbitrary-order corrections followed (Anteneodo and Riera 2009), and finite-time coefficients via the adjoint Fokker–Planck equation [S] — [Lade 2009, arXiv:0905.4324](https://ar5iv.arxiv.org/html/0905.4324); [arXiv:2104.12813](https://arxiv.org/pdf/2104.12813).
- **Identification of drift and diffusion.** Under high-frequency sampling on a fixed span, the diffusion coefficient is identified through quadratic variation. The drift is not consistently estimable without a long span [M].

**4.2 Jumps**
- **Testing for jumps** in a discretely observed process by high-frequency asymptotics [M; DOI checked] — [Aït-Sahalia & Jacod 2009, *Ann. Statist.* 37:184–222, doi:10.1214/07-AOS568](https://doi.org/10.1214/07-AOS568).
- **Contagion** modelled by mutually exciting (Hawkes) jumps across markets [M; DOI checked] — [Aït-Sahalia, Cacho-Diaz & Laeven 2015, *JFE* 117:585–606, doi:10.1016/j.jfineco.2015.03.002](https://doi.org/10.1016/j.jfineco.2015.03.002).

**4.3 Distributions as Fokker–Planck (Kolmogorov forward) systems**
- **Gibrat's law and Zipf's law.** Random growth (Gibrat) with a lower reflecting barrier yields Zipf's law for city sizes [M; DOI checked] — [Gabaix 1999, *QJE* 114:739–767, doi:10.1162/003355399556133](https://doi.org/10.1162/003355399556133). The survey of power laws in economics and finance is [Gabaix 2009, *Annu. Rev. Econ.* 1:255–294, doi:10.1146/annurev.economics.050708.142940](https://doi.org/10.1146/annurev.economics.050708.142940) [M; DOI checked].
- **The speed of transitions.** Standard random-growth theories generate transitions of top inequality that are too slow compared with the data; the NBER version says "an order of magnitude too slow". Two parsimonious deviations fit the data: "scale dependence" (e.g. changing skill prices) and "type dependence" (a group of high-growth types). Convergence speeds are derived by operator and PDE methods [S] — [Gabaix, Lasry, Lions & Moll 2016, *Econometrica* 84(6):2071–2111, doi:10.3982/ECTA13569](https://doi.org/10.3982/ECTA13569) (DOI checked); [NBER w21363](https://www.nber.org/papers/w21363).
- **Continuous-time heterogeneous-agent models** couple a Hamilton–Jacobi–Bellman equation with a Kolmogorov forward equation for the distribution [M; DOI checked] — [Achdou, Han, Lasry, Lions & Moll, *REStud* 89:45–86 (2022; online 2021), doi:10.1093/restud/rdab002](https://doi.org/10.1093/restud/rdab002).
- **Wealth distribution and mobility** are estimated jointly by simulated moments [M; DOI checked] — [Benhabib, Bisin & Luo 2019, *AER* 109:1623–1647, doi:10.1257/aer.20151684](https://doi.org/10.1257/aer.20151684).
- **Densities as time series.** A time series of cross-sectional densities is treated as a functional time series, with nonstationary (unit-root) components in the densities [M; DOI checked] — [Chang, Kim & Park 2016, *J. Econometrics* 192:152–167, doi:10.1016/j.jeconom.2015.06.025](https://doi.org/10.1016/j.jeconom.2015.06.025).
- **A functional state-space model** [S] — [Chang, Chen & Schorfheide 2024, *JPE* 132(12):4021–4067, doi:10.1086/731411](https://doi.org/10.1086/731411) (DOI checked):
  - Its transition is a functional VAR for aggregates and a cross-sectional density jointly.
  - Its measurement equation accounts for the sampling error of log densities estimated from repeated cross-sections.
  - Sieve approximations make it finite-dimensional.
  - Applied to technology shocks, GDP, employment and the earnings distribution, it finds modest spillovers between aggregate and distributional dynamics.
- **Distribution dynamics of cross-country incomes** by stochastic kernels (the "twin peaks") [M; DOI checked] — [Quah 1996, *Eur. Econ. Rev.* 40:1353–1375, doi:10.1016/0014-2921(95)00051-8](https://doi.org/10.1016/0014-2921(95)00051-8).

**4.4 Networks and flows among sectors and regions**
- **A fitness model of the World Trade Web.** GDP as fitness reproduces its topology [M; DOI checked] — [Garlaschelli & Loffredo 2004, *PRL* 93:188701, doi:10.1103/PhysRevLett.93.188701](https://doi.org/10.1103/PhysRevLett.93.188701).
- **World input–output networks over time** are mostly described, not modelled. Three studies [S] — [search results](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC4519177/):
  - Cerina et al. track WIOD 1995–2011 communities (a growing German-led European community, China's rise).
  - del Río-Chanona et al. track PageRank and strength over time — [PMC5270329](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5270329/).
  - Moosavi and Isacchini model the evolving network as a sequence of Markov chains with the Kemeny constant as a flow indicator — [PMC5655482](https://www.ncbi.nlm.nih.gov/pmc/articles/PMC5655482/).
- **Limits of the data.** WIOD's annual tables (1995–2011 in that release) limit dynamic estimation, and one dynamic-model paper uses a single year's network because input–output networks "evolve very slowly" [S] — same search.
- **Harris–Wilson spatial dynamics** (retail or urban structure as Lotka–Volterra-type flows among zones): the integrated methodology — [Wilson 2008, *J. R. Soc. Interface* 5:865–871, doi:10.1098/rsif.2007.1288](https://doi.org/10.1098/rsif.2007.1288) [M; DOI checked]. Made a stochastic model with Bayesian inference from data — [Ellam, Girolami, Pavliotis & Wilson 2018, *Proc. R. Soc. A* 474:20170700, doi:10.1098/rspa.2017.0700](https://doi.org/10.1098/rspa.2017.0700) [M; DOI checked]. Calibrated by neural networks — [Gaskin, Pavliotis & Girolami 2023, *PNAS* 120, doi:10.1073/pnas.2216415120](https://doi.org/10.1073/pnas.2216415120). Networks inferred from time series — [Gaskin, Pavliotis & Girolami 2024, *PNAS Nexus* 3, doi:10.1093/pnasnexus/pgae063](https://doi.org/10.1093/pnasnexus/pgae063) [M; DOIs checked].

#### Inferences
- **Guarantees hold only within the model class.** The guarantees that exist (consistency and efficiency of Aït-Sahalia's approximate MLE; convergence of finite-time-corrected Kramers–Moyal estimates) assume the model class is right and the process stationary and Markov. Chapman–Pearson shows that the decisive check is *simulation from the fitted null*: does the "discovered" nonlinearity reappear when the truth is linear?
- **Transition speed tests a distributional law.** For distributional laws, the speed of transition, not the stationary shape, is the informative test. Many laws share a Pareto tail; only some reproduce the speed of observed change (GLLM).
- **Fitted balance laws on economic graphs are scarce.** There is no standard, validated method for learning conservation laws of value among regions or sectors from a few snapshots. The closest are the Bayesian or neural calibration of Harris–Wilson SDEs and the functional VAR of densities. This is a gap the infrastructure's `balance` module could fill, with honest reporting.

#### Gaps
- Not verified by reading: the full GLLM text, Chapman–Pearson's estimator details, and the published Chang–Chen–Schorfheide text.
- No study was found that fits a Fokker–Planck model to *time series* of firm-size distributions and validates it out of sample.
- No validated dynamic model of input–output coefficient change (beyond RAS-type updating) was found in this search.

### 5. How a Learned Economic Model Should Report What the Record Cannot Identify, Structural Breaks and the Lucas Critique

#### Takeaway
Identification failures are widespread in estimated macro models: observational equivalence, partial and weak identification (Canova–Sala). Rank conditions exist for linearised models (Komunjer–Ng; Iskrev), and profile likelihood separates structural from practical non-identifiability (Raue et al.).

Structural instability is pervasive in macro relations (Stock–Watson 1996). Since the mid-1980s, inflation and output have been hard to forecast better than naive benchmarks (Stock–Watson 2007; Edge–Gürkaynak).

The empirical evidence on the Lucas critique is mixed:
- Little direct evidence (Ericsson–Irons).
- Reduced forms can look stable because the policy shifts in the record were small (Rudebusch).
- The critique matters for the early-1980s shift in US monetary policy (Lubik–Surico).

Forecast competitions add three lessons: combinations and hybrids beat pure machine learning (M4); out-of-sample comparisons need tests that handle nesting and instability (Diebold–Mariano, Clark–West, Giacomini–White); and conformal intervals need adaptations for dependence and drift.

#### Cited Findings

**5.1 Identifiability**
- **Komunjer and Ng: rank conditions.** Dynamic identification of DSGE parameters from first and second moments. Full rank of the Jacobian of the solution parameters with respect to the deep parameters is necessary but not sufficient. Rank and order conditions are given for stochastically singular and non-singular models, accommodating measurement error and a priori restrictions [S] — [Komunjer & Ng 2011, *Econometrica* 79(6):1995–2032, doi:10.3982/ECTA8916](https://doi.org/10.3982/ECTA8916) (DOI checked).
- **Iskrev: local identification** through the rank of the Jacobian of the model's moments [M; DOI checked] — [Iskrev 2010, *JME* 57:189–202, doi:10.1016/j.jmoneco.2009.12.007](https://doi.org/10.1016/j.jmoneco.2009.12.007).
- **Canova and Sala: identification problems are widespread.** "Observational equivalence, partial and weak identification problems are widespread". They "lead to biased estimates, unreliable t-statistics and may induce investigators to select false models". Diagnostics are proposed [S] — [Canova & Sala 2009, *JME* 56:431–449, doi:10.1016/j.jmoneco.2009.03.014](https://doi.org/10.1016/j.jmoneco.2009.03.014) (DOI checked).
- **Structural versus practical non-identifiability** of partially observed dynamical models, by the profile likelihood [M; DOI checked] — [Raue et al. 2009, *Bioinformatics* 25:1923–1929, doi:10.1093/bioinformatics/btp358](https://doi.org/10.1093/bioinformatics/btp358).

**5.2 The Lucas critique**
- **The critique.** Reduced-form parameters change when policy changes, because agents' decision rules depend on the policy regime [M; DOI checked] — [Lucas 1976, *Carnegie-Rochester Conf. Ser.* 1:19–46, doi:10.1016/S0167-2231(76)80003-6](https://doi.org/10.1016/S0167-2231(76)80003-6).
- **Ericsson and Irons.** Reviewing all articles in the Social Science Citation Index that cite Lucas (1976), they conclude that "virtually no evidence exists that empirically substantiates the Lucas critique", using tests of super exogeneity [S] — [Ericsson & Irons 1995, Fed IFDP 506](https://www.federalreserve.gov/econres/ifdp/the-lucas-critique-in-practice-theory-without-measurement.htm).
- **Rudebusch.** Estimated policy rules show US policy changed, yet VARs often do not reject parameter stability. This is reconciled by the modest size of historical policy shifts and the limited sensitivity of plausible forward-looking reduced forms to them [S] — [Rudebusch 2005, *JMCB* 37:245–272, doi:10.1353/mcb.2005.0024](https://doi.org/10.1353/mcb.2005.0024) (DOI checked).
- **Estrella and Fuhrer.** Some forward-looking models may be *less* stable than their backward-looking counterparts [S] — [Boston Fed WP 99-4](https://www.bostonfed.org/-/media/Documents/Workingpapers/PDF/wp99_4.pdf).
- **Lubik and Surico.** Empirical findings confirm the critique's relevance for the early-1980s shift in US monetary policy [S] — [Lubik & Surico 2010, *JAE* 25:177–194, doi:10.1002/jae.1129](https://doi.org/10.1002/jae.1129) (DOI checked); [Richmond Fed WP 06-05](https://www.richmondfed.org/-/media/RichmondFedOrg/publications/research/working_papers/2006/pdf/wp06-5.pdf).

**5.3 Structural breaks and instability**
- **Instability is pervasive.** Widespread instability was found in univariate and bivariate forecasting relations among US monthly macro series [M; DOI checked] — [Stock & Watson 1996, *JBES* 14:11–30, doi:10.1080/07350015.1996.10524626](https://doi.org/10.1080/07350015.1996.10524626).
- **Inflation.** US inflation became harder to forecast relative to naive models after the mid-1980s; the unobserved-components model with stochastic volatility fits this [M; DOI checked] — [Stock & Watson 2007, *JMCB* 39:3–33, doi:10.1111/j.1538-4616.2007.00014.x](https://doi.org/10.1111/j.1538-4616.2007.00014.x).
- **Testing and dating breaks.** Multiple structural changes are tested and dated in [Bai & Perron 1998, *Econometrica* 66:47–78, doi:10.2307/2998540](https://doi.org/10.2307/2998540). An introduction is [Hansen 2001, *JEP* 15(4):117–128, doi:10.1257/jep.15.4.117](https://doi.org/10.1257/jep.15.4.117). The survey of forecasting under instability is [Rossi 2013, *Handbook of Economic Forecasting* 2B:1203–1324, doi:10.1016/B978-0-444-62731-5.00021-X](https://doi.org/10.1016/b978-0-444-62731-5.00021-x) [M; DOIs checked].

**5.4 Out-of-sample comparisons and the record of forecasting**
- **The random walk.** It matched structural exchange-rate models at 1–12-month horizons, even with realised fundamentals [M; DOI checked] — [Meese & Rogoff 1983, *JIE* 14:3–24, doi:10.1016/0022-1996(83)90017-X](https://doi.org/10.1016/0022-1996(83)90017-X).
- **Tests of predictive accuracy** [M; DOIs checked]: equal accuracy — [Diebold & Mariano 1995, *JBES* 13:253–263, doi:10.1080/07350015.1995.10524599](https://doi.org/10.1080/07350015.1995.10524599); nested models — [Clark & West 2007, *J. Econometrics* 138:291–311, doi:10.1016/j.jeconom.2006.05.023](https://doi.org/10.1016/j.jeconom.2006.05.023); conditional predictive ability, robust to instability and estimation windows — [Giacomini & White 2006, *Econometrica* 74:1545–1578, doi:10.1111/j.1468-0262.2006.00718.x](https://doi.org/10.1111/j.1468-0262.2006.00718.x).
- **M4** (100,000 series, 61 methods) — [Makridakis, Spiliotis & Assimakopoulos 2020, *IJF* 36:54–74, doi:10.1016/j.ijforecast.2019.04.014](https://doi.org/10.1016/j.ijforecast.2019.04.014) (DOI checked):
  - Of the eight entries using pure ML or ML with time-series methods, the pure-ML ones were none more accurate than the combination benchmark, and only one beat Naïve2.
  - 12 of the 17 most accurate methods were combinations of mostly statistical methods.
  - The winning hybrid of statistics and ML was nearly 10% more accurate than the combination benchmark [S].
  - Critics questioned M4's realism [S] — [SAS commentary](https://blogs.sas.com/content/forecasting/2020/01/08/m4-forecasting-competition-results-and-commentary).
- **M5.** On hierarchical retail sales with explanatory variables, the leading entries were dominated by gradient-boosted trees [M; DOI checked] — [Makridakis, Spiliotis & Assimakopoulos 2022, *IJF* 38:1346–1364, doi:10.1016/j.ijforecast.2021.11.013](https://doi.org/10.1016/j.ijforecast.2021.11.013).

**5.5 Uncertainty quantification**
- **Bootstrap intervals for impulse responses** (bootstrap-after-bootstrap bias correction) [M; DOI checked] — [Kilian 1998, *REStat* 80:218–230, doi:10.1162/003465398557465](https://doi.org/10.1162/003465398557465).
- **Conformal inference for dependent data.** Block permutations keep exact validity under exchangeability and approximate validity for time series under weak conditions on the conformity score [V abstract] — [Chernozhukov, Wüthrich & Zhu 2018, COLT, arXiv:1802.06300](https://arxiv.org/abs/1802.06300).
- **Adaptive conformal inference.** Provable long-run coverage frequency "irrespective of the true data generating process" under arbitrary distribution shift [V abstract] — [Gibbs & Candès 2021, arXiv:2106.00170](https://arxiv.org/abs/2106.00170).
- **Conformal Growth-at-Risk.** Conformal calibration of extremal quantiles for US GDP improves calibration and robustness, especially in the tails [V abstract] — [Bogani, Fontana, Neri & Vantini 2024, arXiv:2411.00520](https://arxiv.org/abs/2411.00520).

#### Inferences
- **Two kinds of non-identification must be reported.**
  - *Structural*: the law's parameters (or terms) are not determined by the observables, whatever the sample. Rank conditions and observational equivalence show it.
  - *Practical*: a flat likelihood or posterior at this sample size, shown by profile likelihood, prior-to-posterior overlap or ensemble disagreement.

  For equation discovery, a third kind applies: *model-class non-identification*. Several sparse laws fit equally well. Ensemble inclusion probabilities and information criteria across libraries show it.
- **The Lucas critique becomes a support condition.** A law learned under a policy regime is a reduced form valid for the range of policy variation in the record. An evaluator should flag any proposed intervention outside that range: "out of support of the record". Low power of stability tests (Rudebusch) means "no break detected" must not be read as "invariant".
- **Breaks belong in the law.** They should be reported as part of it (dated breaks, regime probabilities, drifting coefficients), not as residual noise. Forecast evaluation should be per period (Giacomini–White), to show *when* a learned law helps.

#### Gaps
- I found no paper that systematically applies the identification diagnostics of the DSGE literature (rank, profile likelihood) to *machine-learned* economic laws (SINDy, neural ODE).
- The detailed results of Ericsson–Irons, Rudebusch and Lubik–Surico come from snippets.

### 6. What This Means for the Infrastructure

#### Takeaway
kinesis already has the core: templates with LM fitting, SINDy and weak SINDy, balance laws on graphs, transfer operators and transition detectors. What the econometric literature adds is less new estimators than three disciplines:
- **latent states with measurement error**, through state-space likelihoods and regime processes;
- **mandatory benchmarks and model-free checks**: random walk, AR/BVAR, factor models, combinations, and local-projection impulse responses;
- **explicit reporting of what the record does not identify**: rank and profile diagnostics, ensemble inclusion probabilities, finite-sample nulls, breaks, and the support of past policy variation.

A learned economic law can serve as an evaluator of allocations whose order matters. But order effects come precisely from the nonlinear, state-dependent terms that the record identifies worst. So the evaluator must say when a ranking of orders is driven by unidentified structure.

#### Recommendations (Inferences from the Findings Above)

**A. Methods to add to the foundation (where they belong)**

1. **A measurement layer for declared systems.** A declaration gains an observation equation, with measurement noise and missing or mixed-frequency observations. Its likelihood comes from tyche's filters:
   - Kalman for linear-Gaussian templates;
   - extended or unscented filters and particle filters for nonlinear ones (as in Fernández-Villaverde & Rubio-Ramírez 2007).

   This turns template fitting into latent-state estimation (output gaps, nowcasts in the Giannone–Reichlin–Small manner) without new estimators in kinesis. tyche already holds Gaussian conditioning, hidden Markov models and VARs; the composition belongs in kinesis's `fit`.
2. **Regime-switching laws.** A flow or map whose parameters follow a hidden Markov chain (Hamilton 1989), and drifting parameters (TVP with stochastic volatility, Primiceri 2005). These are "systems with events" whose events are unobserved. The detector of transitions and the regime model should share one representation, so that a detected break becomes a regime in the law.
3. **Baselines a learned law must beat, as declared objects:**
   - a random walk, AR and BVAR (Li–Plagborg-Møller–Wolf favour BVARs for precision);
   - a dynamic factor model;
   - an analogue or EDM forecaster (kinesis's "forecaster with no law"; Tacchella et al.'s analogue field is the strongest economic precedent);
   - an equal-weight combination (M4's lesson).

   Comparisons are run by Diebold–Mariano, Clark–West (nested) and Giacomini–White (conditional, period by period), with losses reported over time.
4. **Local projections as a falsification test.** Every learned law (template, SINDy, neural ODE) implies impulse responses. These are compared with LP or VAR responses estimated directly from the record, which in population estimate the same responses (Plagborg-Møller & Wolf 2021). A law whose responses fall outside the LP bands has learned forecasts, not dynamics.
5. **Stochastic laws estimated properly:**
   - Aït-Sahalia's closed-form likelihood expansion for discretely sampled diffusions;
   - Kramers–Moyal estimation with finite-sampling-interval corrections (with the warning that corrections can fake structure);
   - jump tests and self-exciting jumps.

   Every nonparametric drift or diffusion estimate gets a **Chapman–Pearson check**: refit the same estimator on paths simulated from the best *simple* (linear-drift) model, and report whether the apparent nonlinearity survives. This mirrors the transitions module's tests against a fitted null.
6. **Dynamics of distributions:**
   - **A functional state-space model**: a density represented by log-spline or sieve coefficients, measured with sampling error that depends on cross-section size, jointly with aggregates (Chang–Chen–Schorfheide 2024).
   - **Fokker–Planck laws** on a grid of income or wealth cells. This is the `balance` module applied to a state space of incomes rather than places.
   - **Transition speed as a reported diagnostic**: the spectral gap, compared with the speed of observed change (Gabaix–Lasry–Lions–Moll 2016).
7. **Balance laws for flows among regions and sectors**: Harris–Wilson-type spatial SDEs with Bayesian or neural calibration (Ellam et al. 2018; Gaskin et al. 2023). The urban-science domain's LUTI work already composes Wilson's models through nomos; the dynamic, stochastic, *learned* version is kinesis's. Input–output coefficient drift remains a research gap.
8. **Koopman/DMD extended**: Hankel DMD and EDMD beside the transfer operator, with bootstrap bands on eigenvalues. The DMD reconstruction error serves as a regime indicator (Gottwald–Gugole), wired into `transitions` with its stated blind spot (transitions on the same timescale as the dynamics).
9. **Conformal wrappers for forecasts**: block-permutation conformal for dependent data (Chernozhukov–Wüthrich–Zhu) and adaptive conformal under drift (Gibbs–Candès). These give interval validity without trusting the law's own likelihood.
10. **Equation discovery made honest for economics:**
    - Ensemble-SINDy with inclusion probabilities and conformal coefficient intervals (Fasel 2025).
    - Libraries that include regime indicators and stochastic terms.
    - A required synthetic-recovery test on data simulated *at the record's length and noise*. Khundadze's synthetic success and real failure show why it is needed.

**B. Learned economic laws as evaluators of allocations whose order matters**

- **Use the law as a stochastic transition map.** An allocation sequence (a₁, a₂, …) enters a learned law as a sequence of inputs or interventions; its value is a functional of the simulated path. Under a *linear time-invariant* law with additive effects, order matters only through timing and discounting, by superposition. Genuine order effects (complementarity, hysteresis, regime-triggering) come only from nonlinear or state-dependent terms, which §2–§3 show are the terms the economic record identifies worst. Hence:
  - **Evaluate with an ensemble of laws.** Use posterior draws, an E-SINDy bootstrap ensemble, the regime-switching posterior, *and the linear benchmark*. Report whether the *ranking* of orders is stable across the ensemble. This can be kybernesis's dual verdict: a standing (ranking robust, arguable, or undecided) beside a measure (the expected difference with its interval).
  - **Attribute every order effect** to the terms that produce it (which library terms, which regime transitions). Report the identification status of those terms (inclusion probability, profile-likelihood flatness). An ordering preference that rests on terms with low inclusion probability is "undecided by the record".
  - **Flag out-of-support allocations (Lucas).** If an allocation moves inputs or policy variables outside the range observed in the record, or into a regime never observed, the evaluator returns its value with the flag "out of support of the record" and widens or refuses the interval. This parallels tyche's withholding of estimates outside their area of applicability (ADR 0128). Absence of a detected break is not evidence of invariance (Rudebusch 2005).
  - **Cost and fidelity.** Learned laws are cheap surrogates relative to structural or agent-based evaluators, so they suit the inner loop of an evolutionary search. Candidate orders the surrogate ranks highly should be re-evaluated by a more structural evaluator, with the disagreement reported (multi-fidelity, as in Part V on pluggable evaluators).

**C. What each method should report about what the data cannot identify (a proposed minimum contract)**

| Method | Must report |
|---|---|
| Template fit (LS/LM, state-space likelihood) | Jacobian/Fisher rank and its smallest singular values (Iskrev; Komunjer–Ng); profile-likelihood intervals marking practically unidentified parameters (Raue et al.); for Bayesian fits, prior-versus-posterior overlap |
| VAR/SVAR/LP baselines | The identifying assumption (recursive, sign, external instrument) as declared data; bias–variance choice (LP vs BVAR); bootstrap or Bayesian bands |
| Regime-switching or TVP law | Regime probabilities over time; number of regimes chosen and alternatives; dated breaks (Bai–Perron) with intervals |
| SINDy / weak SINDy / symbolic regression | Inclusion probability of each term across an ensemble; rival sparse laws within a tolerance of the best fit; recovery on synthetic data at the record's length and noise; comparison with a linear law |
| Neural ODE / UDE | Forecast skill against less-constrained comparators (the 2026 SciML diagnostic); sensitivity to seeds and splits; no structural claim without an LP check |
| Diffusion / Kramers–Moyal | Sampling interval and the correction used; span-limited drift identification; the Chapman–Pearson simulated-null result |
| Distribution laws (Fokker–Planck, functional VAR) | Cross-section sampling error; transition speed versus observed; stationary versus transient fit |
| Koopman/DMD/EDM | Embedding dimension and lag choices; surrogate tests (Fourier or shuffled) separating linear from nonlinear structure; eigenvalue bootstrap bands |
| Any forecast | Rolling-origin out-of-sample losses against random walk, AR/BVAR, factor model and combination; Diebold–Mariano / Clark–West / Giacomini–White; conformal coverage achieved |
| Any law used for policy or allocation | Range of policy and input variation in the record (the support); "out of support" flags; "no break detected" stated with the test's power |

#### Gaps
- No surveyed source tests learned economic laws as *evaluators of sequences of allocations*. The order-effect argument above is inference, not a cited result.
- No standard exists for reporting model-class non-identification of equation discovery on economic data. The contract above is a proposal.
- The economics-specific record of neural ODEs, UDEs and reservoir computing is thin and recent (2024–2026, mostly preprints). It should be revisited before any of these is adopted as a default.

## Part IX. Quantum and Quantum-Like Dynamical Models of Economic Phenomena, How They Are Fitted, and Their Evidence

*Remark.* Part IX's scope and conventions. Markers: **[V]** verified by reading the source itself during this research (for arXiv items, "[V-abs]" means the abstract page was read, not the full paper); **[S]** from a search-result snippet only; **[M]** from memory, not checked in this session. Research done 2026-11-20. Several primary sources (PNAS, publisher PDFs) were blocked or unreadable; where so, the claim is marked [S] or [M] and says so.

Scope as assigned: (1) quantum-like models of decision and markets; (2) how they are fitted; (3) quantum computation for economic dynamics; (4) critiques and evidence, with order-dependent allocation in view. The first key question carries the catalogue of models; the fourth carries the formal treatment of non-commutativity. A final section, requested by the brief, says what this means for the infrastructure.

### 1. Quantum and Quantum-Like Economic Models That Make Predictions Classical Models Cannot, and Whether Those Predictions Survive Held-Out Data or New Experiments

#### Takeaway
Only one family makes a sharp, a priori, parameter-free prediction that has been tested at scale and largely held: the Lüders-projection model of question order, whose "QQ equality" held across about 70 national surveys (Wang et al. 2014). Even that prediction is not exclusive to quantum probability (classical noise and repeat-choice models can produce it), stronger reciprocity tests derived from the same quantum law fail for the non-degenerate models, and the disjunction effect that motivated interference models replicates poorly. Quantum game theory and most "quantum finance" make no prediction about economic data that a classical model cannot make; the former changes the game, the latter is largely classical stochastic mathematics written in quantum notation.

#### Cited Findings

**1a. Quantum cognition and decision (the strongest empirical line)**
- Programmatic statements: Pothos and Busemeyer, "Can quantum probability provide a new direction for cognitive modeling?", *Behavioral and Brain Sciences* 36(3):255–274 (2013), doi:10.1017/S0140525X12001525, published with open peer commentary (among them Bradley Love, who raised the concern that such accounts work largely at the computational level, and Gelman and colleagues, sympathetic but proposing another route) and a reply "Quantum principles in psychology: the debate, the evidence, and the future" (pp. 310–327) [S] — [Cambridge Core](https://www.cambridge.org/core/product/6F36FF614A8B06BDC0F2BB878E0C9105); [City Research Online](https://openaccess.city.ac.uk/id/eprint/2428/). Books: Busemeyer and Bruza, *Quantum Models of Cognition and Decision* (CUP 2012, doi:10.1017/CBO9780511997716) [M]; Haven and Khrennikov, *Quantum Social Science* (CUP 2013, doi:10.1017/CBO9781139003261) [M].
- **The QQ equality.** Wang, Solloway, Shiffrin and Busemeyer, "Context effects produced by question orders reveal quantum nature of human judgments", *PNAS* 111(26):9431–9436 (2014), doi:10.1073/pnas.1407756111, PMCID PMC4084470 [V] — [PMC full text](https://pmc.ncbi.nlm.nih.gov/articles/PMC4084470/):
  - The prediction: q = [p(AyBy) + p(AnBn)] − [p(ByAy) + p(BnAn)] = 0 (equivalently the off-diagonal form), derived from sequential projective (Lüders) measurement with the two questions asked "back to back without any information inserted in between" [V].
  - Tested on 70 national surveys with 651–3,006 respondents each (66 Pew surveys 2001–2011 that met the criteria, Gallup polls from Moore 2002, and Schuman, Presser and Ludwig 1981) and two laboratory experiments of more than 100 respondents each [V].
  - Results: across the 66 Pew surveys the distribution of q-test χ²(1) values did not deviate from the null (p = 0.4625) while the distribution of order-effect χ² values did (p = 0.0004): order effects are real and the equality holds; paired diagonal context effects correlate at r = −0.82 against a predicted slope of −1 (r = −0.73 without two extreme points) [V].
  - A failure: the Gallup Rose–Jackson pair violated the equality (q = 0.1514, χ²(1) = 28.57, p < 0.001); the authors attribute it to information inserted between the questions and excluded it [V]. They also concede the q test is informative only when order effects are well above zero [V].
  - What they compared: they did not fit classical Bayesian or Markov models; they argued that "no traditional psychology theories impose this precise kind of symmetry constraint" and showed in the Supporting Information that a repeat-the-first-choice model and an anchoring-adjustment model, constrained to satisfy QQ, may lose their ability to produce order effects [V].
- **Classical accounts of the same pattern.**
  - A class of classical "repeat-choice" models reproduces the mirrored (QQ-like) order effects and is offered as a benchmark against which quantum accounts should be compared: "Classic-probability accounts of mirrored (quantum-like) order effects in human judgments" [S, title and abstract snippet] — [AMiner listing](https://www.aminer.cn/pub/619b80a91c45e57ce9eaea10/classic-probability-accounts-of-mirrored-quantum-like-order-effects-in-human-judgments). Authors Kellen, Singmann and Batchelder, *Decision* 2018 [M; the listing did not show authors or venue].
  - A Bayesian update model, in which order effects arise from the respondent's prior belief that the two questions are correlated, accounts for order effects "with fewer parameters than its quantum counterparts"; by its symmetry assumptions it does not necessarily satisfy QQ — Moreira and de Barros, "Order effects in Bayesian updates", CogSci 2021, arXiv:2105.07354 [V-abs] — [arXiv](https://arxiv.org/abs/2105.07354). The same paper cites Costello and Watts (2018), for whom order effects follow from classical probability with noise [S, via that paper's text in search results].
- **Stronger tests derived from the same quantum law fail.** Boyer-Kassem, Duchêne and Guerci derive "Grand Reciprocity" equations from the quantum law of reciprocity; the non-degenerate versions of existing quantum-like models of question order "fail the GR test in most cases", so these models could survive only in degenerate versions, which they say is not easy — arXiv:1501.04901 (*Mathematical Social Sciences* 2016 [M for venue]) [V-abs] — [arXiv](https://arxiv.org/abs/1501.04901). The same group's experiment, run with methods of experimental economics, concludes that major quantum-like models "cannot account for the conjunction fallacy" — *Theory and Decision* 2016, arXiv:1606.03430 [V-abs] — [arXiv](https://arxiv.org/abs/1606.03430).
- **Interference and the sure-thing principle.** The disjunction effect (Tversky and Shafir 1992), a standard showcase for quantum interference in choice, replicates poorly:
  - Kühberger, Komunska and Perner (2001), *OBHDP* 85: no significant disjunction effect in the two-step gamble, within participants or with real payoffs [S] — [EconPapers](https://econpapers.repec.org/RePEc:eee:jobhdp:v:85:y:2001:i:2:p:250-264); Lambdin and Burdsal (2007) argued a within-participant effect may survive [S, via search summary].
  - Broekaert, Busemeyer and Pothos (2020), *Cognitive Psychology* 117:101262, doi:10.1016/j.cogpsych.2019.101262: online replication, N = 1,119; more risk-averse participants showed a reliable order effect and violation of the law of total probability plus a new "inflation effect", less risk-averse participants showed neither; a quantum-dynamics model "matched in form to the Markov model" described all major aspects while a standard utility model and a heuristic Markov model could not [S] — [City Research Online](https://openaccess.city.ac.uk/id/eprint/23325/); [author copy](https://jbusemey.pages.iu.edu/quantum/BroekaertBusemeyerPothos2020.pdf).
- **Contextuality, the stricter non-classical signature.**
  - Dzhafarov, Zhang and Kujala reviewed behavioural and social datasets "from polls of public opinion to visual illusions to conjoint choices to word combinations to psychophysical matching" and found that "none of these data provides any evidence for contextuality" under Contextuality-by-Default; they hypothesise that behavioural "contextual effects" are direct influences of context on response distributions — *Phil. Trans. R. Soc. A* 2015/2016, arXiv:1504.07422 [S] — [ar5iv](https://ar5iv.arxiv.org/html/1504.07422).
  - Against that, Cervantes and Dzhafarov, "Snow Queen is evil and beautiful", *Decision* 5:193–204 (2018), arXiv:1711.00418, found contextuality proper in a crowdsourced choice experiment despite violated marginal selectivity, extended by Basieva, Cervantes, Dzhafarov and Khrennikov, "True contextuality beats direct influences in human decision making" (arXiv:1807.05684) [S] — [arXiv 1711.00418](https://arxiv.org/abs/1711.00418); [ar5iv 1807.05684](https://ar5iv.arxiv.org/html/1807.05684).
- **Open-system (Lindblad) models of preference.** Kvam, Busemeyer and Pleskac, "Temporal oscillations in preference strength provide evidence for an open system model of constructed preference", *Scientific Reports* 11:8169 (2021), doi:10.1038/s41598-021-87659-0 [V] — [PMC](https://pmc.ncbi.nlm.nih.gov/articles/PMC8046775):
  - Prediction that distinguishes it: quantum-walk dynamics predict that mean preference strength oscillates over deliberation time, classical random walks a smooth approach; two experiments (gift cards; gambles) found systematic oscillation, and an early elicited choice changed the later pattern [V].
  - Model: dρ/dt = −i(1−α)[H, ρ] + α·L(ρ), H tridiagonal (drift μ_Q, diffusion σ_Q), L the Lindblad form of a Markov random walk (μ_M, σ_M), α the quantum–Markov weight, plus a decay λ [V].
- **Decision as relaxation of an open system.** Asano, Basieva, Khrennikov, Ohya and Tanaka, "Quantum-like dynamics of decision-making", *Physica A* 391(5):2083–2099 (2012), doi:10.1016/j.physa.2011.11.042: decision as stabilisation to a steady state of a quantum-like dynamics (lifting channels), the steady state giving decision probabilities [S] — [EconPapers](https://econpapers.repec.org/RePEc:eee:phsmap:v:391:y:2012:i:5:p:2083-2099). A 2026 review by Asano and Khrennikov places this line within GKSL (Lindblad) dynamics: superpositions of alternatives relaxing to steady states that signify commitment [S] — [arXiv 2604.18643](https://arxiv.org/abs/2604.18643v1).
- **A recent model-comparison paper in quantum cognition.** Huang, Busemeyer, Ebelt and Pothos, "Bridging the gap between subjective probability and probability judgments: the quantum sequential sampler", *Psychological Review* 132(4):916–955, doi:10.1037/rev0000489 (online 2024): quantum probability rules plus a sequential sampling process, compared against the Bayesian Sampler on a new experiment described as one of the largest datasets in probabilistic reasoning; reports a new systematic overestimation of probabilities [S] — [City Research Online](https://openaccess.city.ac.uk/id/eprint/32745/).

**1b. Quantum game theory**
- Eisert, Wilkens and Lewenstein, "Quantum games and quantum strategies", *PRL* 83:3077 (1999), doi:10.1103/PhysRevLett.83.3077: in an entangled quantisation of the prisoner's dilemma a new equilibrium escapes the dilemma [M]. Meyer, "Quantum strategies", *PRL* 82:1052 (1999), doi:10.1103/PhysRevLett.82.1052: a quantum player beats a classical one at penny flip [M].
- Benjamin and Hayden, Comment, *PRL* 87:069801 (2001), arXiv:quant-ph/0003036: when the strategy set is the whole of SU(2), the EWL equilibrium is not an equilibrium, and there is no Nash equilibrium in deterministic quantum strategies [S] — [arXiv](https://arxiv.org/abs/quant-ph/0003036).
- van Enk and Pike, "Classical rules in quantum games", *PRA* 66:024306 (2002), arXiv:quant-ph/0203133 (first titled "Quantum games are no fun (yet)"): the usual quantisation yields solutions that are neither genuinely quantum nor solutions of the original game, since changing the rules changes the game; a classical game with the same payoff structure can be written down. Later papers dispute (2017) and support (2018, arXiv:1802.00260) this [S] — [arXiv](https://arxiv.org/abs/quant-ph/0203133).
- Human play: Chen and Hogg, "How well do people play a quantum prisoner's dilemma?", *Quantum Information Processing* 5:43–67 (2006): with simulated quantum mechanics, untrained people nearly achieve the payoffs theory predicts but do not use the predicted mixed-strategy Nash equilibria [S] — [Unpaywall](https://unpaywall.org/10.1007%2FS11128-006-0012-7).

**1c. Quantum finance and quantum models of markets**
- Baaquie's field theory of forward interest rates (*Quantum Finance*, CUP 2004, doi:10.1017/CBO9780511617577 [M]). Baaquie and Srikant calibrated variants to Eurodollar futures; models based on "psychological factors" fitted best; Treasury tick data from GovPx could not yield forward rates — *PRE* 69:036129 (2004), arXiv:cond-mat/0208528 [S] — [arXiv](https://arxiv.org/abs/cond-mat/0208528). Their 2001 precursor reported HJM inconsistent with the data and the field theory model in significant agreement [S] — [arXiv cond-mat/0106317](https://arxiv.org/abs/cond-mat/0106317).
- Quantum harmonic oscillator for returns: Ahn, Choi, Dai, Sohn and Yang, *EPL* 120:38003 (2017): mean reversion as a restoring force, the stochastic equation transformed into a Schrödinger equation whose eigenfunctions imply a mixed χ distribution of returns; on the FTSE All-Share it fitted better than geometric Brownian motion and Heston [S] — [EPL](https://epljournal.edpsciences.org/10.1209/0295-5075/120/38003).
- Orrell's quantum economics: price change as the displacement of a quantum harmonic oscillator, offered to explain the square-root law of price impact and to predict "q-variance" and a "q-distribution" of price changes (a 2020 *Physica A* paper; "Q-Variance" with Paul Wilmott, 2025); Orrell frames the wavefunction as a modelling device, not a claim about physical reality, in reply to a physicist's critique that markets are not quantum [S, from the author's blog] — [Orrell blog QEF12](https://www.goodreads.com/author_blog_posts/22347835-qef12-a-quantum-oscillator-model-of-stock-markets); [QEF15](https://www.goodreads.com/author_blog_posts/25860056-qef15-a-quantum-oscillator-model-of-stock-markets-2-q-variance-and-th?tab=author). His book *Quantum Economics: The New Science of Money* (2018) [M].
- Schaden, "Quantum finance", *Physica A* 316 (2002); Haven's embedding of Black–Scholes in a quantum setting (*Physica A* 304, 2002) [M].
- Yukalov and Sornette's quantum decision theory (prospect interference and entanglement, arXiv:1102.2738) [S, title only] — [arXiv](https://arxiv.org/pdf/1102.2738); its "quarter law" fixing the average interference term at 1/4 as a parameter-free prediction, and tests on risky-choice data (Favre et al., *PLoS ONE* 2016) [M].

#### Inferences
- The only quantum-like prediction that is (a) a priori, (b) parameter-free, (c) tested on many independent datasets is the QQ equality. It survived as a pattern, but it is a consequence of a symmetry of the Lüders rule that some classical models share, so its survival is evidence for "a mechanism with that symmetry", not for quantum probability specifically. The GR failures show that the full quantum law of reciprocity, which a non-degenerate projective model also implies, does not hold.
- The Kvam et al. oscillation finding is a qualitatively distinctive prediction (coherent oscillation versus monotone approach) and is the most direct bridge to Lindblad dynamics; but the open-system model nests the Markov one, and the evidence is in-sample fit with an adjusted R² penalty (see §2), not held-out prediction.
- Contextuality in the Contextuality-by-Default sense is the only test that separates "non-classical" from "context-dependent". Its record is mostly negative for survey and choice data, with a few positive crowdsourced results. Order effects in polls, the showcase of quantum cognition, were among the datasets in which Dzhafarov et al. found no contextuality.
- Baaquie's path integrals and the QHO return models are, mathematically, Euclidean (imaginary-time) path integrals and Fokker–Planck operators recast as Schrödinger operators [M for the general mathematical point]: their "quantum" content is notation for classical stochastic processes (a Gaussian random field of forward rates; an Ornstein–Uhlenbeck-type mean reversion). Their empirical claims are in-sample fits against named classical benchmarks, which is a fair test of the stochastic model but no test of anything quantum.
- Quantum games are new games (with an entangling device as part of the rules); they say nothing about how people or markets behave in the original game, and the one human experiment found people did not play the predicted equilibria.

#### Gaps
- The full text and Supporting Information of Wang et al. 2014 (PNAS returned 403; the PMC text lacked the SI): the lab experiments' design and the exact classical models tested there were not read.
- The authorship and venue of the "classic-probability accounts" paper (Kellen, Singmann and Batchelder, *Decision* 2018) were not confirmed in this session [M].
- No held-out or preregistered replication of the QQ equality on new surveys after 2014 was found in this search; it may exist (e.g., in Wang and Busemeyer's later work), but none was read.
- The details of Orrell's empirical tests (q-variance against which classical baselines, on which data) could not be read; only the author's blog summaries were seen.
- Yukalov–Sornette's quarter law and its tests were not checked [M].

### 2. How Such Models Are Estimated from Data, and How They Compare with Classical Models of Equal Flexibility

#### Takeaway
Estimation ranges from parameter-free tests (QQ), through least squares on mean trajectories (Kvam et al. 2021) and hierarchical Bayesian estimation of nested quantum–Markov models, to calibration of quantum-finance models to market curves and maximum-likelihood or manifold-constrained learning of hidden quantum Markov models. The fair comparisons all exploit nesting (the Markov model is the quantum model with one parameter at zero, or HQMMs are a subclass of observable operator models); comparisons against classical models of equal flexibility, on held-out data, are rare. Quantum-system identification (Lindblad learning, Hamiltonian learning, process tomography) supplies mature estimation machinery that has barely been used on economic data.

#### Cited Findings

**2a. Quantum cognition**
- Parameter-free testing: the QQ equality is tested by a χ²(1) difference-in-proportions "q test" on each survey's two-way table, no parameters fitted [V] — [PMC4084470](https://pmc.ncbi.nlm.nih.gov/articles/PMC4084470/).
- Open-system preference model (Kvam et al. 2021) [V] — [PMC8046775](https://pmc.ncbi.nlm.nih.gov/articles/PMC8046775):
  - fitted by minimising squared error between observed and predicted mean preference strength (not likelihood, no BIC), with 1,000 random starts of MATLAB's fminsearch;
  - six core parameters (μ_Q, σ_Q, μ_M, σ_M, α, λ), a start-point variability fixed at half the scale width (the Methods are inconsistent on whether it was free);
  - compared with a pure Markov model (α = 1) and a pure quantum model (α = 0) by mean squared error and adjusted R²; the open system's MSE was "much lower than the other two in both experiments"; α estimated at 0.23 or 0.285 (Experiment 1) and 0.64 or 0.641 (Experiment 2; text and Table 3 disagree slightly);
  - samples: 118 recruited, 105 analysed (Experiment 1); 62 (Experiment 2);
  - data and code on OSF, https://osf.io/vknj6/.
- Nested hierarchical Bayesian estimation: Busemeyer, Wang and Trueblood (2012) fitted quantum and Markov models of dynamic inconsistency by hierarchical Bayesian estimation; the quantum model reduces to the Markov one when one parameter is zero, and the posterior of that parameter lay away from zero [S] — [Springer chapter](https://rd.springer.com/chapter/10.1007/978-3-642-35659-9_8). A conference paper by Busemeyer, Wang and Shiffrin (2012) titled as a Bayesian model comparison of quantum versus traditional models for violations of dynamic consistency is cited but was not found [S].
- Quantum versus belief-adjustment: Trueblood and Busemeyer compared the quantum inference model with Hogarth and Einhorn's belief-adjustment model on medical (Bergus et al. 1998) and jury order-effect data [S] — [Frontiers 2012 review describing it](https://www.frontiersin.org/articles/10.3389/fpsyg.2012.00138/full). Busemeyer, Pothos, Franco and Trueblood, *Psychological Review* 118(2):193–218 (2011), doi:10.1037/a0022542, is the broader account of probability judgment errors [S] — [City Research Online](https://openaccess.city.ac.uk/id/eprint/1977/1/QP4_1.pdf).
- The quantum sequential sampler was compared with the Bayesian Sampler on a large new dataset, with data and analysis code on GitHub [S] — [City Research Online](https://openaccess.city.ac.uk/id/eprint/32745/).
- A classical rival with fewer parameters (Moreira and de Barros 2021) [V-abs] — [arXiv](https://arxiv.org/abs/2105.07354).

**2b. Quantum finance**
- Calibration: Baaquie and Srikant calibrated several field-theory variants to Eurodollar futures and estimated the forward-rate volatility function model-independently from market data [S] — [arXiv cond-mat/0208528](https://arxiv.org/abs/cond-mat/0208528).
- Distribution fitting: Ahn et al. compared fitting errors and goodness-of-fit statistics of the QHO-implied return distribution against GBM and Heston on the FTSE All-Share [S] — [EPL](https://epljournal.edpsciences.org/10.1209/0295-5075/120/38003).

**2c. Quantum-like time-series models**
- Hidden quantum Markov models (HQMMs), parameterised by Kraus operators (Monras et al. 2010), learned by iteratively finding unitary transformations that increase likelihood; on data generated by HQMMs they matched an HMM's predictive performance with fewer hidden states — Srinivasan, Gordon and Boots, AISTATS 2018 (PMLR 84:1979–1987), arXiv:1710.09016 [S] — [PMLR](https://proceedings.mlr.press/v84/srinivasan18a.html).
- HQMMs are "a special subclass of the general class of observable operator models (OOMs)" that avoid negative probabilities by design; learned by retraction-based constrained gradient descent on the Stiefel manifold, faster and scaling to larger models — Adhikary, Srinivasan, Gordon and Boots, AISTATS 2020 (PMLR 108), arXiv:1912.02098 [V-abs] — [arXiv](https://arxiv.org/abs/1912.02098).

**2d. Quantum-system identification (the machinery available)**
- Lindblad learning from time series: a method that infers the parameters of Markovian open quantum systems from measurement data by direct simulation of the master equation preserving complete positivity, especially useful when measurements are far apart in time — arXiv:2307.03858 [V-abs] — [arXiv](https://arxiv.org/abs/2307.03858) (authors Wang and Li, *Quantum* 2024 [S]).
- Large-scale Lindblad learning: a linear system for the parameters built from observables and their time-derivatives, obtained by fitting damped sinusoids, applied to a gate layer on a 156-qubit processor — van den Berg et al., arXiv:2512.08165 (2025) [S] — [arXiv](https://arxiv.org/pdf/2512.08165).
- Maximum likelihood on Pauli measurements at transient times with a neural differential-equation term later removed to recover an interpretable Lindbladian — Heightman et al., arXiv:2603.07778 (2026) [S] — [arXiv](https://arxiv.org/abs/2603.07778).
- Hamiltonian learning with quantum resources (Wiebe, Granade, Ferrie and Cory, *PRL* 112:190501, 2014, doi:10.1103/PhysRevLett.112.190501) and process tomography as the quantum analogue of system identification [M].

#### Inferences
- **Counting flexibility.** A Lindblad generator on a d-level system has d² − 1 Hamiltonian parameters and (d² − 1)² Kossakowski parameters, d⁴ − d² real parameters in all (12 for d = 2), against d(d − 1) rates for a continuous-time Markov chain on d states (2 for d = 2) [derivation, not a citation]. A quantum-like model fitted to classical frequencies is therefore far more flexible than the Markov model it is usually compared with unless strongly constrained (as Kvam et al. did with tridiagonal H and L of a fixed form). Comparisons that penalise only the count of nominal parameters, or use adjusted R² on mean trajectories, understate this.
- **Equal flexibility means a classical model with the same class of expressible observation statistics.** For time series that is the OOM / quasi-realisation (of which HQMMs are a subclass), not the HMM. For order effects it is a repeat-choice or correlated-prior Bayesian model, not unconstrained Bayes. For oscillating preference it is a classical model with a second-order (oscillatory) component, e.g. a damped linear system or a Markov chain with complex eigenvalues; the Kvam et al. comparison against a pure first-order random walk does not rule that out.
- **Identifiability.** Observing only classical outcome frequencies (a choice, a price) identifies at best the classical observation process; the Hilbert-space dimension, the basis and phases are not identified without tomographically complete measurements, which choice experiments and markets do not supply. Lindblad learning in physics relies on preparing states and measuring a complete set of observables; economic data provide neither.
- **What a fair protocol looks like** (from the above): nest the models; fit by likelihood; compare by held-out log score or cross-validation and by Bayes factors with stated priors; test parameter-free predictions on new data; report when the fitted quantum model degenerates into the classical one (α → 1, commuting projectors).

#### Gaps
- No study was found that fits a quantum-like (Lindblad or HQMM) model and a classical model of equal expressive class to real economic or financial time series and compares them out of sample.
- Whether the Busemeyer–Wang–Shiffrin Bayesian model comparison was ever published, and with what Bayes factor, could not be established.
- No paper was found estimating Lindblad parameters from economic time series; the Lindblad-learning literature found is all on physical devices.
- Out-of-sample performance of Baaquie's calibrated models against modern classical term-structure models (e.g., multi-factor Gaussian affine models) was not found.

### 3. Evidence of Quantum Computational Advantage for Economic or Financial Dynamics on Real Data, as of 2026

#### Takeaway
No credible evidence was found. The provable speedups (quadratic, for Monte Carlo pricing and risk by amplitude estimation) need fault-tolerant machines with thousands of logical qubits and logical clock rates far beyond present hardware, and quadratic speedups are argued to be insufficient on early fault-tolerant devices. Claims on real data from current hardware (HSBC–IBM bond fills, 2025; quantum reservoir forecasting of volatility, 2026) are either explained by hardware noise with the noiseless quantum simulation showing no gain, or are proofs of concept without independent replication; systematic benchmarks find classical models beat off-the-shelf quantum classifiers.

#### Cited Findings
- **Provable speedups.** Rebentrost, Gupt and Bromley, "Quantum computational finance: Monte Carlo pricing of financial derivatives", *PRA* 98:022321 (2018), arXiv:1805.00109: distributions prepared in superposition, payoffs by circuits, prices by measurement; European and Asian options; classical numerical evidence that a quadratic speedup is achievable [S] — [arXiv](https://arxiv.org/abs/1805.00109). Woerner and Egger, "Quantum risk analysis", *npj Quantum Information* 5:15 (2019), doi:10.1038/s41534-019-0130-6 [M]; Montanaro, "Quantum speedup of Monte Carlo methods", *Proc. R. Soc. A* 471:20150301 (2015) [M].
- **Resource estimates.** Chakrabarti, Krishnakumar, Mazzola, Stamatopoulos, Woerner and Zeng, "A threshold for quantum advantage in derivative pricing", *Quantum* 5:463 (2021), arXiv:2012.03819: benchmark use cases need about 8k logical qubits and a T-depth of 54 million, executed in about a second to give advantage [V-abs] — [arXiv](https://arxiv.org/abs/2012.03819). Stamatopoulos and Zeng, "Derivative pricing using quantum signal processing", *Quantum* 8:1322 (2024), arXiv:2307.14310: T-gates reduced ~16×, logical qubits ~4×, logical clock rate ~5×; advantage still needs 4.7k logical qubits and 10⁹ T-gates at 45 MHz [S] — [arXiv](https://arxiv.org/abs/2307.14310v2).
- **Quadratic speedups are not enough on early fault tolerance.** Babbush, McClean, Gidney, Boixo and Neven, "Focus beyond quadratic speedups for error-corrected quantum advantage", *PRX Quantum* 2:010103 (2021), arXiv:2011.04149: advantage with reasonable fault-tolerant resources needs speedups beyond quadratic, dramatically better error correction, or both; quartic speedups look much better [S] — [arXiv](https://arxiv.org/abs/2011.04149v1).
- **Surveys.** Herman, Googin, Liu, Sun, Galda, Safro, Pistoia and Alexeev, "Quantum computing for finance", *Nature Reviews Physics* 5:450–465 (2023), doi:10.1038/s42254-023-00603-1 [S for the publication record] — [Nature](https://www.nature.com/articles/s42254-023-00603-1.pdf); its preprint covers stochastic modelling, optimisation and machine learning (derivative pricing, risk, portfolio optimisation, NLP, fraud detection) and their near-term feasibility [V-abs] — [arXiv 2201.02773](https://arxiv.org/abs/2201.02773). Bouland, van Dam, Joorati, Kerenidis and Prakash, "Prospects and challenges of quantum finance", arXiv:2011.06492: some algorithms are near-term heuristics, others proven speedups needing large machines, all measured against the best known classical algorithms [V-abs] — [arXiv](https://arxiv.org/abs/2011.06492).
- **The HSBC–IBM claim (2025).** Ciceri and 15 co-authors, "Enhanced fill probability estimates in institutional algorithmic bond trading using statistical learning algorithms with quantum computers", arXiv:2509.17715: production intraday European corporate-bond inquiry data transformed by an unspecified quantum algorithm on IBM Heron processors, compared with noiseless simulators; up to ~34% relative gain in out-of-sample scores for models given hardware-transformed data; the authors suggest "the inherent noise in current quantum hardware contributes to this effect" [V-abs] — [arXiv](https://arxiv.org/abs/2509.17715).
  - Aaronson: the advantage appears only on hardware and "disappears" under noiseless simulation, which is "all but an admission" that it is an artefact of the comparisons chosen, unrelated to quantum speedup; selection bias in unpublished runs; a real claim needs real hardware, a real quantum algorithm, a speedup over the best classical methods, on a problem that matters; he would not object to noise reported as an open puzzle [V] — [Shtetl-Optimized](https://scottaaronson.blog/?p=9170).
  - A data scientist (Morten Hagen) suggested "random resampling with shrinkage" might replicate the noise effect; the gains were tied to one dataset; HSBC did not trade real bonds with the machines [S] — [The DESK](https://www.fi-desk.com/quantum-computing-breakthrough-has-more-red-flags-than-a-peoples-liberation-army-parade/); [The Quantum Insider](https://thequantuminsider.com/2025/09/28/bond-trading-quantum-bond-trading-a-deeper-look-at-hsbc-and-ibms-bond-trading-study/); [American Banker](https://www.americanbanker.com/news/hsbc-claims-quantum-edge-in-bond-market).
- **Quantum reservoir computing for volatility (2026).** Li, Mukhopadhyay, Bayat and Habibnia, "Quantum reservoir computing for realized volatility forecasting", *Physical Review Research* 8:023028 (2026), arXiv:2505.13933: a fully connected transverse-field Ising reservoir with input and memory qubits, S&P 500 realised volatility, benchmarked against econometric and machine-learning models with model confidence sets; claimed to outperform "consistently across various metrics"; framed as a proof of concept; the abstract does not say whether the reservoir was simulated or run on hardware [V-abs] — [arXiv](https://arxiv.org/abs/2505.13933). A separate six-qubit QRC paper claims >86% up/down classification of stock trends (arXiv:2602.13094) [S] — [arXiv](https://arxiv.org/html/2602.13094v1).
- **Benchmarking QML.** Bowles, Ahmed and Schuld, "Better than classical? The subtle art of benchmarking quantum machine learning models", arXiv:2403.07059 (2024): 12 popular QML models on 6 binary classification tasks (160 datasets); out-of-the-box classical models outperform them; removing entanglement often caused no significant drop; about 40% of surveyed papers claimed a quantum model beat a classical one [S] — [arXiv](https://arxiv.org/abs/2403.07059v2).

#### Inferences
- A quantum reservoir with a handful of qubits is classically simulable; if the Li et al. reservoir was simulated, its result is evidence for a particular nonlinear random-feature model, not for quantum computation. Even run on hardware, it would establish advantage only against a classical reservoir or echo-state network of matched size and tuning, which the abstract does not say was included.
- The HSBC–IBM result, by its own control, is evidence that hardware noise acted as a data transformation that helped a downstream classifier on one dataset; it says nothing about quantum computational advantage. A classical noise-injection or resampling control was the missing experiment.
- For economic dynamics specifically (as opposed to pricing integrals), no quantum algorithm with a proven super-quadratic speedup on a relevant problem was found in this search.

#### Gaps
- Whether Li et al.'s reservoir was simulated or run on hardware, the sample period, and whether HAR and classical reservoirs of matched size were among the benchmarks were not read (only the abstract).
- No independent replication of the HSBC–IBM effect, nor a published rebuttal by the authors to the critiques, was found.
- Dequantisation results relevant to finance (Tang-style classical algorithms for low-rank problems) were not checked in this session.

### 4. What the Non-Commutativity of Quantum Observables Offers as a Formal Model of Order Effects in Sequential Decisions and Allocations, and Its Limits

#### Takeaway
Non-commuting projectors give order effects a model with structure: the order effect in a state is ⟨ψ|P_A P_B P_A|ψ⟩ − ⟨ψ|P_B P_A P_B|ψ⟩, nonzero only when the projectors fail to commute on the state, and the Lüders rule then imposes testable constraints (the QQ equality, the reciprocity and GR equations, bounded interference terms). But non-commutativity as such is not quantum: composing classical state updates or Markov kernels generally does not commute either, and order-dependent allocation (A then B ≠ B then A) is usually explained by a classical change of state (depletion, information, path dependence). The quantum model earns its place only where its specific constraints are tested and hold, and where the order effect comes from judgement being constructed by being elicited.

#### Cited Findings
- The QQ equality is derived exactly from sequential projective measurement under the condition that nothing intervenes between the two questions; inserted information breaks it (Rose–Jackson) [V] — [PMC4084470](https://pmc.ncbi.nlm.nih.gov/articles/PMC4084470/).
- Classical Bayesian updating treats the order of events as irrelevant, so order effects are hard for plain Bayes; the quantum inference model attributes them to incompatible questions, the first changing the state in which the second is evaluated [S] — [Frontiers 2012](https://www.frontiersin.org/articles/10.3389/fpsyg.2012.00138/full).
- Classical rivals that do produce order effects: belief adjustment (Hogarth and Einhorn) [S, ibid.]; repeat-choice models reproducing QQ-like patterns [S] — [AMiner](https://www.aminer.cn/pub/619b80a91c45e57ce9eaea10/classic-probability-accounts-of-mirrored-quantum-like-order-effects-in-human-judgments); correlated-prior Bayesian updating [V-abs] — [arXiv 2105.07354](https://arxiv.org/abs/2105.07354).
- The full quantum law of reciprocity (GR equations) fails for non-degenerate quantum-like models in most cases [V-abs] — [arXiv 1501.04901](https://arxiv.org/abs/1501.04901).
- Eliciting a choice early changed later preference dynamics, which the open-system model captures as the measurement collapsing the preference state, with ontic uncertainty about which decision will be observed separated from epistemic uncertainty about the current state [V] — [PMC8046775](https://pmc.ncbi.nlm.nih.gov/articles/PMC8046775).
- Behavioural "contextual effects" in many datasets are explained by direct influences of context on response distributions, with no contextuality proper [S] — [ar5iv 1504.07422](https://ar5iv.arxiv.org/html/1504.07422); exceptions found in designed choice experiments [S] — [arXiv 1711.00418](https://arxiv.org/abs/1711.00418).

#### Inferences
- **What the formalism contributes to allocation.** Suppose a decision-maker (a panel, a voter, a planner) is asked to allocate to A and to B in sequence, each allocation a yes/no or a level. A Lüders model represents the decision-maker's disposition as a unit vector, each allocation question as a projector (or a projection-valued measure over levels), and the act of allocating as collapse. It then predicts: (i) order effects exactly when the projectors do not commute on the state; (ii) the QQ equality for each pair of binary allocation questions asked in both orders, with no free parameter; (iii) the GR equations; (iv) for a "known versus unknown" first outcome, an interference term violating the law of total probability, bounded by 2√(p₁p₂) in magnitude. Each is falsifiable on counterbalanced data.
- **Its limits for allocation.**
  - When allocating A uses resources needed by B (a budget, a capacity, a queue), the order effect is a classical change of state; a resource-state model (a classical non-commuting composition of maps) is the right one, and a quantum model adds parameters without content.
  - When the first allocation conveys information (it reveals priorities, it anchors), the "nothing intervenes" condition of QQ is broken by construction, as in Rose–Jackson.
  - Order effects are a property of a mechanism's agenda (sequential picking, agenda-setting in voting) that classical social choice already analyses; the quantum model is a model of judgement, not of mechanisms.
  - A classical model with the same flexibility (repeat-choice, correlated priors, an OOM) can match the pattern; distinguishing them needs the parameter-free constraints, not goodness of fit.
- **Non-commutativity is necessary but not sufficient for "quantumness".** Two stochastic matrices generally do not commute; any sequential classical update with memory is order-dependent. The quantum-specific claims are the Lüders symmetry constraints and, more strictly, contextuality in the Contextuality-by-Default sense. A fair study of order effects in allocation would test all three tiers: order effect present; Lüders constraints hold; contextuality present beyond direct influences.

#### Gaps
- No empirical study of quantum-like models applied to resource allocation decisions (budgets, triage, priority-setting) was found in this search.
- The bound on interference terms and the derivation of the GR equations were stated from the formalism and from the abstract of Boyer-Kassem et al., not from a full reading of their paper.

### 5. Open Code and Data

#### Takeaway
Open material is scattered: the Kvam et al. open-system preference data and code (OSF), the quantum sequential sampler's data and code (GitHub, per the publisher record), the Pew survey files behind the QQ test (public through Pew), an open QML benchmarking package (PennyLane), and Qiskit Finance (community-maintained, last release seen 0.4.1). No maintained library of quantum-cognition models, HQMM learners or Lindblad fitting for economic data was found.

#### Cited Findings
- Kvam, Busemeyer and Pleskac (2021): data and code openly on OSF, https://osf.io/vknj6/ [V] — [PMC8046775](https://pmc.ncbi.nlm.nih.gov/articles/PMC8046775).
- Huang, Busemeyer, Ebelt and Pothos (quantum sequential sampler): data and analysis code on GitHub [S] — [City Research Online](https://openaccess.city.ac.uk/id/eprint/32745/); APA supplemental materials page [S] — [APA](https://supp.apa.org/psycarticles/supplemental/rev0000489/rev0000489_supp.html).
- QQ datasets: 66 Pew Research Center surveys (2001–2011) with question order manipulated, three Gallup polls supplied by David Moore, and Schuman, Presser and Ludwig (1981) [V] — [PMC4084470](https://pmc.ncbi.nlm.nih.gov/articles/PMC4084470/). Busemeyer keeps author copies of quantum-cognition papers at jbusemey.pages.iu.edu/quantum [V that the site serves them] — [example](https://jbusemey.pages.iu.edu/quantum/BroekaertBusemeyerPothos2020.pdf).
- Bowles, Ahmed and Schuld: an open-source benchmarking package built on PennyLane [S] — [arXiv](https://arxiv.org/abs/2403.07059v2); [PennyLane blog](https://pennylane.ai/blog/2024/03/benchmarking-near-term-quantum-algorithms-is-an-art/).
- Qiskit Finance: migrated to the qiskit-community GitHub organisation at 0.4.0 to emphasise community maintenance; 0.4.1 added Python 3.12 support; no deprecation notice found [S] — [GitHub releases](https://github.com/qiskit-community/qiskit-finance/releases); [release notes](https://qiskit.org/ecosystem/finance/release_notes.html).
- HQMM papers: author PDFs and appendix at the University of Washington [S] — [PDF](https://homes.cs.washington.edu/~bboots/files/learning_hqmms.pdf); [appendix](https://homes.cs.washington.edu/~bboots/files/learning_hqmms_appendix.pdf).

#### Inferences
- The Kvam et al. OSF archive is the one dataset found that directly supports re-fitting an open-system (Lindblad) decision model and its nested Markov rival, and so the natural first replication target for an infrastructure with a Lindblad simulator.
- The Pew files would let QQ be re-tested and the GR equations computed, provided the survey tables carry both orders.

#### Gaps
- Whether Wang et al. released a compiled table of the 70 surveys was not established (SI not read).
- No open code for HQMM learning was located (only papers).
- No open implementation of Baaquie's interest-rate field theory was found.

### 6. What This Means for the Infrastructure

#### Takeaway
Worth building, on kinesis's open quantum systems and tyche's inference: (1) the Lüders model of sequential judgement with its parameter-free tests (QQ, GR) and their classical rivals; (2) the open-system preference model of Kvam et al. as a kinesis declaration nested over its Markov limit; (3) hidden quantum Markov models as a time-series class compared against tyche's HMMs and against observable operator models of equal class. Build Baaquie-style "quantum finance" and QHO return models, if at all, as the classical stochastic models they are. Do not build quantum game theory as economics, and make no claims of quantum computational advantage.

#### Cited Findings
- Lindblad dynamics with a Hamiltonian–Markov mixing weight already models constructed preference: dρ/dt = −i(1−α)[H, ρ] + α·L(ρ), fitted to mean trajectories with data on OSF [V] — [PMC8046775](https://pmc.ncbi.nlm.nih.gov/articles/PMC8046775).
- The QQ test is a χ²(1) test on a two-way table under counterbalanced order [V] — [PMC4084470](https://pmc.ncbi.nlm.nih.gov/articles/PMC4084470/); the GR equations are the stronger test [V-abs] — [arXiv 1501.04901](https://arxiv.org/abs/1501.04901).
- HQMMs are a subclass of OOMs, learnable by Stiefel-manifold gradient descent [V-abs] — [arXiv 1912.02098](https://arxiv.org/abs/1912.02098).
- Lindblad parameters can be learned from time series by simulating the master equation with complete positivity preserved [V-abs] — [arXiv 2307.03858](https://arxiv.org/abs/2307.03858).
- Quantum computational advantage for finance needs thousands of logical qubits [V-abs] — [arXiv 2012.03819](https://arxiv.org/abs/2012.03819); noise-driven "advantages" vanish under noiseless simulation [V] — [Shtetl-Optimized](https://scottaaronson.blog/?p=9170).

#### Inferences
**6.1 Models worth building, and where.**
- *Sequential judgement under Lüders' rule* (a general model, no economic object named): states as unit vectors or density matrices, questions as projectors or POVMs, sequential probabilities and order effects computed exactly; the QQ statistic and the GR equations as tests in tyche's `testing`; the classical rivals (repeat-choice, belief adjustment, correlated-prior Bayes) in the same module so that every quantum fit has its rivals beside it. Its natural home is kinesis or apodeixis's readings only if neither already holds measurement sequences; otherwise a small module beside kinesis's open quantum systems.
- *Open-system decision dynamics*: the Kvam et al. model is directly a kinesis declaration (an open quantum system with a Hamiltonian part and a Lindblad part of fixed tridiagonal form), with α = 1 its Markov limit and α = 0 its unitary limit. kinesis's templates filled from records (least squares, Levenberg–Marquardt) already fit such a declaration; the first replication is the OSF data.
- *Hidden quantum Markov models* as a time-series class: Kraus-operator parameterisation on physis's Lindblad and channel machinery, learning by likelihood with a Stiefel retraction; compared with tyche's HMMs (fewer states) and with a general OOM (the class of equal expressiveness). A fit on economic regimes (the Economy Atlas's business-cycle HMM is the natural case) would show whether the quantum constraint helps out of sample.
- *Not worth building as quantum*: Baaquie's forward-rate field theory and QHO return models are Gaussian random fields and Fokker–Planck spectral models; physis's random fields and kinesis's stochastic differentials already hold their mathematics, and naming them "quantum" would mislead. Quantum games are new games whose entangling device is part of the rules; they belong, if anywhere, to nomos as game variants, with the van Enk–Pike caveat stated.

**6.2 Fitting and comparing fairly.**
- Nest wherever possible (Markov ⊂ open system; commuting projectors ⊂ Lüders; HMM ⊂ HQMM ⊂ OOM) and report the fitted distance from the classical limit with its interval.
- Count real parameters honestly (d⁴ − d² for a general Lindbladian against d(d − 1) for a Markov chain) and constrain the quantum model before fitting, not after.
- Fit by likelihood; compare by held-out log score (blocked by participant or by time), and by Bayes factors with stated priors; use tyche's online error control when many pairs of questions are tested.
- Prefer parameter-free predictions (QQ, GR, interference bounds) tested on data the model never saw, preregistered with peira's `protocol`.
- Always include a classical model of equal class, and report a quantum model that wins only in-sample as a fit, not as a finding.
- Keep "non-commutative", "Lüders-consistent" and "contextual" as three separate verdicts, the last decided by Contextuality-by-Default criteria.

**6.3 Order effects in allocation.**
- An allocation experiment (peira's designs) asks the same decision-makers to allocate to A and B in counterbalanced order, with no information between the two allocations. The Lüders model predicts the QQ equality for each pair; a resource-state classical model (budget depletion) predicts order effects that do not satisfy it in general. The test separates "judgement constructed by eliciting it" from "the state of resources changed".
- Where the allocation is made by a mechanism (a picking sequence, an agenda), order dependence is a theorem of the mechanism and needs no quantum model.
- A finding that allocation order effects satisfy QQ and GR would license a Lüders model of the decision-makers' judgement for forecasting what a different agenda would yield; a failure of QQ with order effects present would argue for the classical rivals.

**6.4 What such models can and cannot establish.**
- They can establish that a dataset obeys or violates the constraints of projective measurement, that an open-system model predicts held-out trajectories better than its nested Markov limit, and that a quantum-constrained time-series model is or is not more parsimonious than an HMM on some record.
- They cannot establish that minds, markets or economies are quantum physical systems; non-commutativity alone is not evidence of quantumness; in-sample fit of a more flexible model is not evidence at all; and classical outcome data do not identify Hilbert-space dimension, basis or phases.
- Quantum computation offers nothing to the infrastructure's economic dynamics on any horizon now foreseeable; heuresis's exact emulation of quantum search and an emulated amplitude-estimation pricer can serve learning, with the resource estimates stated beside them.

#### Gaps
- Whether kinesis's open-quantum-system declarations support POVMs and sequential measurement with collapse (needed for the Lüders model) was not checked against the code in this research.
- No prior study was found that applies a quantum-like model to allocation order effects, so the proposed experiment would be new; its effect sizes and sample sizes are unknown.

## Part X. What Was Built

The design (`development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`) is built in stages. Stages 1 to 3, the three foundation layers, were built on 2026-11-20 and reviewed by the user; Stages 4 and 5, the assessment and the composition, followed the same day (ADR 0137).

### X.1 Resources (Stage 3; kategoria's decision 0004)

kategoria's `resources`: resource types declared by components with quantity semirings (units, grains, divisible amounts, signed balances, qualities), kinds with disposal declared or refused, capacities and distinct items; conversions as the free processes; conservation laws and monotones verified on the generating processes; every verdict scoped (by construction, on generators, on a finite model decided by kategoria's own constructions, or by search with a witness replayed apart); allocations as morphisms; the order of grants decided on finite systems by their transition monoid and the commuting pairs; operators' equivariance under renaming decided as naturality. 39 checks, among them 2 H₂ + O₂ → 2 H₂O and a cow whose beef and leather cannot go to two recipients.

### X.2 Spaces (Stage 1; heuresis's decision 0008)

heuresis's `spaces`: one value tree for every genotype; a contract of a space with a declared unit; real and integer boxes, multisets, categories, ordered levels, the simplex in Aitchison's geometry, permutations under Cayley's, Hamming's or Kendall's metric, subsets of bounded size, compositions with caps, sequences of any space's values, the sphere and SO(3); the product (Moraglio's theorem), the choice and the constrained subspace; laws written once and run on any space; Raidl and Gottlieb's diagnostics; an adapter to heuresis's evolutionary loop. 83 checks: every law on 26 spaces; four faults planted in a broken space, each found; units and diameters against every pair of ten finite spaces; the product theorem on 3,000 children of 30 random products; uniform samplers by χ² over every value; an evolution over seven spaces at once finding a known optimum.

What the checks showed, beyond what Part III said: line recombination of random keys is geometric under Kendall's metric on the orders it decodes to (a proposition proved in heuresis's theory note, §14.3), yet random keys leave the order unchanged in nine small mutations of ten where a direct permutation never does (Part I's redundancy, measured).

### X.3 Orders (Stage 2; morphe's decision 0021, heuresis's decision 0009)

morphe's `traces` gained words over an alphabet with an independence, their lexicographic (Anisimov and Knuth) and Foata normal forms, and equivalence by the projection lemma written apart; held to the traces found by exchanges on every word up to length five over forty random alphabets (11 checks). heuresis's `sequential`: grant systems, commutation tested and declared, evaluation once per trace, decoders with their reach, crossovers by the feature they keep with a feature probe, words with fixed letter counts, Mallows's laws under three metrics, the adaptivity bracket (48 checks).

Found while building:
- **Commutation for allocation is not Mazurkiewicz's.** Counting two orders that are both infeasible as commuting called two claimants of the last unit of a stock independent. The definition kept is: either order, each grant skipped where it is not allowed, reaches the same state, and both orders grant both or neither. Under it, feasible words of one trace reach one state (heuresis's theory note, Proposition 15.1), which is what caching by trace needs.
- **Keys must be taken of the word granted.** A decoder that skips can make two orders of independent grants grant different words, so the trace is taken of what was granted, not of the genotype.
- **Starkweather's effect reproduced, and its size depends on the budget.** On a stock that runs out (weighted completion, 40 orders) operators that keep precedence reach a mean best of 15,194 against edge recombination's 16,281 over six runs of 100 generations; on 20 orders and 150 generations both reach nearly the same (4,181.0 against 4,181.5). On setups between consecutive items edge recombination wins clearly (5.90 against 8.30). The feature probe names the right feature in both.
- **The adaptivity gap** of 50 random stochastic knapsacks lies within Dean, Goemans and Vondrák's bound of 4, with the best sequence, the best adaptive policy (dynamic programming) and clairvoyance ordered on every instance.

### X.4 Assessment (Stage 4; heuresis's decision 0010)

heuresis's `assessment`, in the planned `noise`'s place: metrics with their role, uncertainty and standing (verified, computed, estimated, proposed); candidates ordered by tiers of evidence before Pareto; a metric never stronger than its evaluator; replicates by power; Hoeffding and empirical Bernstein races; common random numbers judged by the variance of differences; cascades by fidelity with their guarantee; a cache; the agreement of surrogates; Bradley–Terry with an order effect; an assessor for the loop (30 checks). An evolution judged by a computed loss, a noisy estimate and a model that wants the opposite found the computed optimum; where the variance is small the empirical Bernstein race spent 7,006 samples against Hoeffding's 70,906; cascades selected as exhaustive evaluation did at 6% of its cost.

### X.5 The Composition (Stage 5; ADR 0137)

The allocation domain's `evolution` composes the layers: resources with kategoria types and carriers, attributes on spaces, an order with precedence and effects, kategoria's verdict on every allocation as a verified constraint, keys by the trace of what was granted, the kept compared under principles, the record sealed (12 checks). A declared city case's optimum, enumerated over 153,600 discrete allocations, was found exactly; the multidimensional knapsack reproduced Raidl and Gottlieb's ordering (direct with repair 1.25% from the LP bound, permutations 5.14%, random keys 5.64%).

Found while building:
- **A deceptive basin.** Survival of the best settled in a basin of the city case on three seeds of seven, where the optimum asks several parts to change together; age layers (Hornby's ALPS) found it on all fourteen seeds tried, and are the composition's default. The survey's Part I had warned of premature convergence on decoders; here it appeared on a direct product.
- **Principles that cannot read every outcome.** A prioritarian principle with aversion needs outcomes above zero, which a duty's burden can break; the comparison is then refused with its reason, and recorded so.

### X.6 The Edges (Stage 6; ADR 0138)

The API's `science/evolve.hpp` declares problems, resource types and evaluators as JSON; `science.allocation` declares them in Python and asks a model to compare kept allocations in pairs, each pair shown both ways, a share asked again to audit it, of grade C; the visualization layer draws the field of the kept, an allocation on its recipients with the thread of grants, which grants commute and the types' laws as seals; the Urban Studio's *Allocation and justice* brings a tool, routes and a view on Melbourne's areas, with the project's people comparing kept allocations in pairs.

Found while building:
- **Claims refused whole leave most recipients with nothing.** On the declared Melbourne case, five of thirteen areas were granted; met in part, twelve or thirteen are. Partial grants are now a resource's declaration.
- **Kept allocations must differ for the people.** The eight kept differed only in a canopy's angle or below a grain; two are now kept apart only when someone receives something different.
- **A model under a role that forbids ranking declines to compare.** Asked 26 times under the general role it said "neither" every time; under a role of its own it told 2 of 26 pairs apart, judging the rest too alike, which the page says.

### X.7 The Economic Evaluators (Stage 7; ADR 0139)

The economics domain's `ledger` keeps an economy's accounts as §VI.6.1 asked: every agent's books on both sides, postings the only change, each refused with its fault named when the books or the flows disagree, when money changes otherwise than by its issuer's named act, or when a position would turn negative; whole units of a minor unit, so that conservation is exact over any run (200,000 random postings audit cleanly); an outside node. Agents act period by period through nomos's behaviours of three origins (rule, learned, proposed), each shown only what it declared, a run tallied by origin; payments that cannot be met wait in a queue until money arrives; grants carry an origin, and the money is traced from where it entered by rounds (§VI.6.4), its LM3 the three-round truncation. kinesis's `densities` moves a density by a generator whose drift and diffusion vary by cell (§VII.6.1, first item) and solves a value equation whose policy is its drift; `induction` holds a learned law to the naive forecasts, keeps the record's support, and reads the claim that one order is better across an ensemble (§VIII.6, A.3 and B). The allocation domain's `evaluators` puts all three behind its search.

Found while building:
- **A payment between two banks' customers moves money between issuers.** Conservation of money is of its total; each issuer's stock moves with its customers.
- **LM3 measured two ways agrees.** A grant traced through a steady economy built from accounts has LM3 1 + 0.8 + 0.8 · 0.6 = 2.28, equal to the walk read from the same payments to 10⁻⁸; random exchange reaches the exact law of a uniform configuration, the finite form of Drăgulescu and Yakovenko's exponential.
- **Order matters through state, as §VI.6.2 inferred.** In an economy of places, two orders of the same grants re-spend about 4% apart when households spend by whether they are short of money, and agree to 10⁻⁶ when they do not; under a law of holdings with a mean field the order changes the share below a line; under linear learned laws only timing remains.
- **A transfer to the few below a line overshoots any grid.** The declared rule fills the gap below the line first and shares the rest.

### X.8 The Research Tracks (Stage 8; ADR 0140)

- **Surrogates with calibrated intervals** (§VII.6.1, item 3). Split conformal intervals around a Gaussian process cover 90.2% of new runs at 90%; screened by them, a search runs in the true evaluator only the candidates that could be best. A stand-in for the economy of places covered 76% of held-out runs at 80% and, among sixty candidates, ran two in the economy and kept the best.
- **Policies against sequences** (Parts I and II, gaps). On knapsacks whose price rises with every grant, a policy learned once from thirty instances reached 95.5% of the best order of thirty it never saw; a sequence searched for each instance reached 93.5% with ten evaluations and 98.1% with thirty. The comparison the survey found nowhere has an answer for this family: the policy pays where instances are many and evaluations dear.
- **The order of two questions** (Part IX). The QQ equality was tested with its classical rivals nested: a symmetric anchoring keeps it exactly, so it singles out nothing by itself; an asymmetric Markov update breaks it; supporting it needs an equivalence test, which a hundred respondents per order cannot pass and two thousand can.
- **Latent positions with their group** (§VII.6.1, item 2). Distances between nodes' positions are not identified in a network that joins unlike nodes more than like ones; the test against the group says so before anything is reported.

### X.9 On Real Problems and Records (ADR 0141)

- **The benchmark files** (Part II). The OR-Library's multidimensional knapsacks, composed as allocations by a search that names no knapsack: five of `mknap1`'s seven optima found and the rest within 0.85%; within 11.2% of Chu and Beasley's best known on their 100-item problems, the price of generality that §II.6 anticipated.
- **A rule of participatory budgeting as a sequential mechanism** (Part III). Every order of the projects decoded by the budget gives a selection to which none can be added, and every such selection is an order's decoding; utilitarian greedy and the Method of Equal Shares are orders. On Warsaw's ballots of 2023 the city's count leaves 7.9% of 78,904 voters with nothing and Equal Shares 4.7%; orders the search kept, with the rules' orders proposed to its first generation, leave 3.8% while meeting more approvals than either. Proportionality in the sense of extended justified representation cannot be checked at this size and is not claimed.
- **Rules on the domain's own problems** (Parts I and II, gaps). On generated city cases where the worst order reaches 60% of the best, a linear rule over declared features of the composition's state, learned once, reaches 99.6% of held-out cases' best orders; searched orders reach 99.97% with a hundred evaluations.
- **Calibration, the accounts first** (§VI.6.3). Households' local share and firms' wages and supplies are read from the EU27's input–output table; the share of holdings spent is matched to the euro area's velocity of households' money. One moment identifies one parameter, and only given the money held idle: with as much idle in firms as in households no share reaches the velocity, and the calibration is refused.
- **The economic evaluators at the edges.** An economy of places, a law of holdings and laws learned from a record are declared as JSON at the API, and the Urban Studio allocates Melbourne's care fund in an economy of places: the allocation that re-spends most (A$2.13 m from A$2 m) reaches three areas, the one that reaches most re-spends 10% less and serves the people in need best; the two evaluators are kept apart.

### X.10 The Stand-in Inside the Search (ADR 0142)

The surrogate of §VII.6.1, item 3, now screens inside the loop: once warm, a candidate whose conformal interval lies above the best kept so far is not run, and the evaluator abstains, so that nothing is kept on the stand-in's word. On a fund for eight places judged by an economy whose households spend by whether they are short, the economy ran 420 and 337 times instead of 488 at 90% and 70% confidence, the best found was the same, and the online coverage, measured on the candidates let through, was 0.91 and 0.83. The theorem's band is not claimed, since a search's candidates drift.

### X.11 Not Yet Built

Screening on several metrics at once; the generalised assignment problems, which need a carrier by which each recipient draws from one of several pools; people's comparisons kept per person; the API's ask–tell over a saved state; real stocks and their revaluations in the ledger; a measurement layer and regime-switching laws; the tests of Clark and West and of Giacomini and White; Hoff's latent distance model; quantum-like models of allocation choices fitted to a counterbalanced experiment; a published heterogeneous-agent solution to hold kinesis's value equation to; calibration to a city's own records.
