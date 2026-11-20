# Part A. Biology, Evolvability and Transmission: Annotated Survey

Date of search: 20-21 November 2026. Status: draft, for citation after the caveats in section 1 are read.

## 1. Scope and method

The survey covers the primary literature behind five design ideas: (1) a compact generative "genome" per unit of a project, from which the body is redeveloped and checked; (2) parts of the genome that control evolution (rates of variation, recombination partners, conserved parts); (3) modular variation as a cheaper way to follow a changing environment; (4) a compact bottleneck each generation that purges damage and favours compressible structure, at the risk of regression toward the copier's prior; (5) two coexisting modes of propagation, copying the body and redevelopment from the genome.

Sources used: the Crossref REST API (bibliographic record of each DOI: title, authors, year, journal, volume, pages and, where deposited, abstract); the arXiv API; Open Library (catalogue records for books); web search for the iterated-learning literature on language models and for the Griffiths and Kalish DOI. Publisher pages were not opened one by one.

Meaning of the marks, and a limit that must be stated plainly:

- [V]: a record was opened and authors, title, year and venue match what is written here. For most items the record is bibliographic. Where the record carried an abstract, the field "Abstract read" says yes and the summary rests on it. Where it says "no", the summary of the content comes from the author's background knowledge of the work and was **not re-read in this search**; those summaries should be checked against the paper before being quoted.
- [P]: some field was confirmed only through a secondary source or is incomplete (stated per item).
- [U]: not opened. None is used as a finding.

Not found or not opened: Weismann's original texts (no catalogue record retrieved; the germ-soma doctrine is cited through Buss and Grosberg and Strathmann); Simon 1962 (JSTOR page not opened, marked [P]); Hornby and Pollack's other papers and the grammatical-evolution founding paper by Ryan, Collins and O'Neill (1998), not searched. No 2023-2026 study of LLM populations improving a repository by evolution was sought here; that belongs to another part.

The versions of some dates: Wagner's "paradox resolved" appeared online in 2007 and in volume 275 in 2008; Payne and Wagner appeared online in 2018 and in volume 20 in 2019. Both years are shown where the record gives them.

## 2. Annotated works

### 2.1 Facilitated variation and conserved core processes

**1. [V]** Kirschner, M., Gerhart, J. (1998). Evolvability. *Proc. Natl. Acad. Sci. USA* 95(15): 8420-8427. doi:10.1073/pnas.95.15.8420. Abstract read: yes.
Defines evolvability as the capacity to generate heritable phenotypic variation. Argues that metazoan diversity arose with conserved core processes, and that regulatory changes in when, where and under what conditions those processes are used drove diversification; weak linkage, compartmentation, redundancy and exploratory processes let non-lethal variation accumulate. Suggests evolvability may be selected only indirectly, and more directly in lineages with repeated radiations.
Bearing: supports ideas 2 and 3 as an organising picture (a conserved core, with change concentrated in regulatory links). The limit is the authors' own: selection for evolvability is stated as indirect and conditional, not as a general mechanism.

**2. [V]** Gerhart, J., Kirschner, M. (2007). The theory of facilitated variation. *Proc. Natl. Acad. Sci. USA* 104(suppl 1): 8582-8589. doi:10.1073/pnas.0701035104. Abstract read: partly (first sentences).
Proposes that most anatomical and physiological traits since the Cambrian result from regulatory changes in the use of a large set of conserved core components, so that random genetic change yields viable and often useful phenotypic variants. The core processes are held constant while their regulation varies.
Bearing: supports ideas 2 and 3 (conserved core, cheap regulatory variation). It is a theory of animals; its transfer to texts and code is an analogy and must be labelled so.

**3. [V]** Parter, M., Kashtan, N., Alon, U. (2008). Facilitated variation: how evolution learns from past environments to generalize to new environments. *PLoS Comput. Biol.* 4(11): e1000206. doi:10.1371/journal.pcbi.1000206. Abstract read: no (record has none).
A computational model, from the background knowledge of the paper, in which evolution under changing goals built from shared modules finds solutions to new goals faster, because the population has learned the structure of the problem family.
Bearing: the main formal support for idea 3. Its limit is that the speed-up depends on the new goals sharing structure with the old ones.

### 2.2 Evolvability, and whether it evolves

**4. [V]** Wagner, G. P., Altenberg, L. (1996). Perspective: complex adaptations and the evolution of evolvability. *Evolution* 50(3): 967-976. doi:10.1111/j.1558-5646.1996.tb02339.x. Abstract read: no.
From background knowledge: argues that the genotype-phenotype map shapes the distribution of variation, that modularity (a map in which parts vary independently) is a precondition for complex adaptation, and that such structure can be selected in the long run through its effect on the fitness of descendants.
Bearing: supports idea 2 conceptually. It is an argument, not a measurement, and it is the source of the objection that selection on evolvability needs long time horizons.

**5. [V]** Pigliucci, M. (2008). Is evolvability evolvable? *Nat. Rev. Genet.* 9: 75-82. doi:10.1038/nrg2278. Abstract read: no.
From background knowledge: reviews the concept, separating the question whether the capacity to evolve is itself a target of selection from whether it is a by-product, and concludes the evidence is partial and the theoretical problem of second-order selection is open.
Bearing: a limit for ideas 2 and 3. A designer should not claim that evolvability evolves as established.

**6. [V]** Sniegowski, P. D., Murphy, H. A. (2006). Evolvability. *Curr. Biol.* 16(19): R831-R834. doi:10.1016/j.cub.2006.08.080. Abstract read: no (record found by title search).
From background knowledge: a short critical primer that distinguishes the several meanings of evolvability, and points out that the empirical evidence that selection acts on it is thin outside mutators.
Bearing: a limit for idea 2 and for the word "evolvability" in the design document: it should be defined operationally.

**7. [V]** Payne, J. L., Wagner, A. (2019; online 2018). The causes of evolvability and their evolution. *Nat. Rev. Genet.* 20: 24-38. doi:10.1038/s41576-018-0069-z. Abstract read: no.
From background knowledge: a review organising the causes of evolvability (robustness, a genotype-phenotype map with neutral networks, modularity, regulatory structure) and the evidence on whether those causes are themselves selected.
Bearing: supports ideas 2 and 3 and gives the most recent consolidated statement; its own conclusions on direct selection are guarded.

### 2.3 Origins and role of modularity

**8. [V]** Kashtan, N., Alon, U. (2005). Spontaneous evolution of modularity and network motifs. *Proc. Natl. Acad. Sci. USA* 102(39): 13773-13778. doi:10.1073/pnas.0503610102. Abstract read: partly.
Reports simulations in which modularity and recurring network motifs evolve spontaneously when the goal changes over time in a way that keeps common subgoals ("modularly varying goals"), and not under a fixed goal.
Bearing: supports idea 3, with the condition that the environment varies modularly. Limit: the result is for small simulated circuits.

**9. [V]** Kashtan, N., Noor, E., Alon, U. (2007). Varying environments can speed up evolution. *Proc. Natl. Acad. Sci. USA* 104(34): 13711-13716. doi:10.1073/pnas.0611630104. Abstract read: partly.
Shows in simulation that evolution toward a goal can be faster when the goal varies in a structured way over time than when it is fixed, a generic way to speed up evolution.
Bearing: supports idea 3 directly; the speed-up is for structured variation only, which the designer must check holds for the project at hand.

**10. [V]** Clune, J., Mouret, J.-B., Lipson, H. (2013). The evolutionary origins of modularity. *Proc. R. Soc. B* 280(1755): 20122863. doi:10.1098/rspb.2012.2863. Abstract read: partly.
Argues that modularity is a key driver of evolvability, and reports that selection for performance alone does not produce it; adding a cost for network connections yields modular networks that also evolve faster.
Bearing: a limit and a design hint for idea 3. Modularity does not appear from performance pressure alone; a cost on coupling (here, a cost on cross-unit dependencies) may be needed.

**11. [P]** Simon, H. A. (1962). The architecture of complexity. *Proc. Am. Philos. Soc.* 106(6): 467-482. JSTOR 985254. Not opened; the bibliographic data are from memory and a Crossref title search did not return the item. Treat as unconfirmed.
From background knowledge: complex systems that evolve are hierarchic and nearly decomposable, and the watchmaker parable shows that stable subassemblies shorten the search.
Bearing: supports idea 3 as the classical argument; no empirical test of it in the present setting.

### 2.4 Robustness, neutral networks, degeneracy, canalisation

**12. [V]** Wagner, A. (2008; online 2007). Robustness and evolvability: a paradox resolved. *Proc. R. Soc. B* 275(1630): 91-100. doi:10.1098/rspb.2007.1137. Abstract read: partly.
Robustness and evolvability seem opposed; the paper argues, on genotype networks, that a system robust to mutation can still reach many different phenotypes because neutral change moves it across the network to places with different neighbours.
Bearing: supports idea 1 and 3 where checks tolerate redevelopment variation; limits: the claim concerns genotype spaces with large neutral networks, which a generative text may not have.

**13. [V]** Wagner, A. (2005). Robustness, evolvability, and neutrality. *FEBS Lett.* 579(8): 1772-1778. doi:10.1016/j.febslet.2005.01.063. Abstract read: no (record found by title search; metadata only).
From background knowledge: reviews neutral networks and the link between robustness and the ability to explore.
Bearing: as item 12.

**14. [P]** Schuster, P., Fontana, W., Stadler, P. F., Hofacker, I. L. (1994). From sequences to shapes and back: a case study in RNA secondary structures. *Proc. R. Soc. Lond. B* 255: 279-284. doi:10.1098/rspb.1994.0040. Authors not returned by Crossref; taken from memory. Abstract read: no.
From background knowledge: in the RNA sequence-to-structure map, many sequences fold to the same shape and shapes are reachable from one another by few mutations, which established neutral networks.
Bearing: the empirical root of the neutral-network argument (ideas 1 and 3); an RNA folding map is not a map from text to behaviour.

**15. [V]** Edelman, G. M., Gally, J. A. (2001). Degeneracy and complexity in biological systems. *Proc. Natl. Acad. Sci. USA* 98(24): 13763-13768. doi:10.1073/pnas.231499798. Abstract read: yes.
Defines degeneracy as the ability of structurally different elements to perform the same function, and argues it is ubiquitous (genetic code, immune system, neural systems) and a feature of complexity and evolution.
Bearing: supports the design choice of several different bodies satisfying one genome (idea 1, 5). It distinguishes degeneracy from redundancy, which a designer should too.

**16. [V]** Whitacre, J. M. (2010). Degeneracy: a link between evolvability, robustness and complexity in biological systems. *Theor. Biol. Med. Model.* 7: 6. doi:10.1186/1742-4682-7-6. Abstract read: no.
From background knowledge: a review proposing that degeneracy links robustness to evolvability, since distinct components that are redundant under one condition differ under others.
Bearing: ideas 1 and 3; a review with modelling, not a test.

**17. [V]** Waddington, C. H. (1942). Canalization of development and the inheritance of acquired characters. *Nature* 150: 563-565. doi:10.1038/150563a0. Abstract read: no.
The origin of canalisation: development is buffered so that a standard phenotype appears despite genetic and environmental perturbation.
Bearing: gives the term for the check that holds the body to the genome (idea 1); also the idea behind capacitance (item 38).

### 2.5 Genotype-phenotype maps, development and generative encodings

**18. [V]** Carroll, S. B. (2008). Evo-devo and an expanding evolutionary synthesis: a genetic theory of morphological evolution. *Cell* 134(1): 25-36. doi:10.1016/j.cell.2008.06.030. Abstract read: no.
From background knowledge: argues that morphological evolution proceeds largely by changes in cis-regulatory elements of conserved toolkit genes, which allows change in one part without damage to others.
Bearing: biological support for ideas 2 and 3 (variation concentrated in regulatory text, part by part). It is a position in a debate; coding-sequence evolution also matters.

**19. [V]** Stanley, K. O., Miikkulainen, R. (2003). A taxonomy for artificial embryogeny. *Artif. Life* 9(2): 93-130. doi:10.1162/106454603322221487. Abstract read: partly.
Notes that evolving phenotypes of biological complexity needs an indirect mapping from genotype, and sets out a taxonomy (cell fate, targeting, heterochrony, canalisation, complexification) for such development in evolutionary computation.
Bearing: supports idea 1 as a design space, not as evidence that it works for text.

**20. [V]** Stanley, K. O. (2007). Compositional pattern producing networks: a novel abstraction of development. *Genet. Program. Evolvable Mach.* 8(2): 131-162. doi:10.1007/s10710-007-9028-8. Abstract read: no.
From background knowledge: the CPPN abstracts development as a function composition that yields patterns with symmetry and repetition, without simulating cell growth.
Bearing: idea 1, as an example of a compact generator with regularities.

**21. [V]** Stanley, K. O., D'Ambrosio, D. B., Gauci, J. (2009). A hypercube-based encoding for evolving large-scale neural networks. *Artif. Life* 15(2): 185-212. doi:10.1162/artl.2009.15.2.15202. Abstract read: partly.
HyperNEAT encodes connectivity patterns by a CPPN over the geometry of the substrate, which lets large networks be evolved from a compact description.
Bearing: evidence for idea 1 where the body has geometric regularity; the limit is that regularity must exist in the body.

**22. [V]** Hornby, G. S., Pollack, J. B. (2002). Creating high-level components with a generative representation for body-brain evolution. *Artif. Life* 8(3): 223-246. doi:10.1162/106454602320991837. Abstract read: partly.
Defines generative representations by their reuse of genotype elements when the genotype is translated to a phenotype, and shows they scale and form higher-level components in body-brain evolution.
Bearing: supports idea 1 and the compactness claim; the benefit shown is for reuse in regular bodies.

**23. [V]** O'Neill, M., Ryan, C. (2001). Grammatical evolution. *IEEE Trans. Evol. Comput.* 5(4): 349-358. doi:10.1109/4235.942529. Abstract read: no.
From background knowledge: a variable-length binary or integer genome is mapped through a grammar to programs, separating search space from solution space.
Bearing: idea 1; a standard example of a genome-to-body mapping that yields valid bodies by construction.

### 2.6 Germ and soma, bottlenecks, individuality

**24. [V]** Buss, L. W. (1987). *The Evolution of Individuality*. Princeton University Press. Open Library record /works/OL4987903W (title, author, year match; publisher not confirmed from the record).
From background knowledge: an account of the conflict between cell lineages and the evolution of the early separation of germ and soma as a way to control it.
Bearing: motivates a protected lineage of genomes distinct from the bodies (idea 4). The doctrine's own premises vary across taxa (plants have no early germline); do not generalise.

**25. [V]** Grosberg, R. K., Strathmann, R. R. (2007). The evolution of multicellularity: a minor major transition? *Annu. Rev. Ecol. Evol. Syst.* 38: 621-654. doi:10.1146/annurev.ecolsys.36.102403.114735. Abstract read: yes.
Reviews the repeated origins of multicellularity and the requirements (adhesion, communication, programmed death); it discusses single-cell stages and the control of conflict among cells.
Bearing: supports idea 4 as one of several solutions to conflict among units; limits: it is a minor transition, many multicellular lineages do not have a strict germline.

**26. [V]** Michod, R. E. (2007). Evolution of individuality during the transition from unicellular to multicellular life. *Proc. Natl. Acad. Sci. USA* 104(suppl 1): 8613-8618. doi:10.1073/pnas.0701489104. Abstract read: partly.
Describes stages, each advantageous in itself, from unicellular to multicellular individuals, including the evolution of germ-soma specialisation and the suppression of conflict.
Bearing: idea 4; supports the claim that a restricted germ stage reduces conflict, with models for specific conditions.

**27. [V]** Kirkwood, T. B. L. (1977). Evolution of ageing. *Nature* 270: 301-304. doi:10.1038/270301a0. Abstract read: no.
From background knowledge: the disposable soma theory, in which an organism spends limited resources on maintenance of the soma less than on the germline.
Bearing: gives a rationale for spending effort on the genome (germline) and tolerating decay of the body (idea 1).

**28. [V]** Bergstrom, C. T., Pritchard, J. (1998). Germline bottlenecks and the evolutionary maintenance of mitochondrial genomes. *Genetics* 149(4): 2135-2146. doi:10.1093/genetics/149.4.2135. Abstract read: yes.
Mitochondria are uniparentally inherited, appear non-recombining and have high mutation rates, and so may suffer Muller's ratchet; the paper models how a germline bottleneck affects this.
Bearing: the closest direct model for idea 4; the effect depends on parameters, and bottlenecks have costs as well as benefits (in the paper's framework, variance among offspring is increased).

**29. [V]** Muller, H. J. (1964). The relation of recombination to mutational advance. *Mutat. Res.* 1(1): 2-9. doi:10.1016/0027-5107(64)90047-8. Abstract read: no.
The origin of the ratchet: in asexual lineages the class with fewest deleterious mutations is lost irreversibly.
Bearing: the danger of idea 5 if clonal copying is the only mode, and the reason to have redevelopment or recombination.

**30. [V]** Birky, C. W. (2001). The inheritance of genes in mitochondria and chloroplasts: laws, mechanisms, and models. *Annu. Rev. Genet.* 35: 125-148. doi:10.1146/annurev.genet.35.102401.090231. Abstract read: no (record found by title search).
From background knowledge: reviews uniparental inheritance and the segregation of organelle genomes.
Bearing: biology of the single-parent mode for idea 4 and 5; a biological case in which the sole-copier model brings both purging by segregation and accumulation by absence of recombination.

### 2.7 Control of evolution by the genome

**31. [V]** Sniegowski, P. D., Gerrish, P. J., Lenski, R. E. (1997). Evolution of high mutation rates in experimental populations of *E. coli*. *Nature* 387: 703-705. doi:10.1038/42701. Abstract read: no.
From background knowledge: mutator alleles arose and spread by hitchhiking with beneficial mutations in long-term populations.
Bearing: supports idea 2: a heritable rate controller can win by association with what it produces. Limit: mutators carry a load, and later work finds they are often lost.

**32. [V]** Taddei, F., Radman, M., Maynard-Smith, J., Toupance, B., Gouyon, P. H., Godelle, B. (1997). Role of mutator alleles in adaptive evolution. *Nature* 387: 700-702. doi:10.1038/42696. Abstract read: no.
From background knowledge: mutators are favoured by hitchhiking in adapting asexual populations, and the benefit is strongest in new environments.
Bearing: idea 2; conditions: asexual, strong selection.

**33. [V]** Moxon, E. R., Rainey, P. B., Nowak, M. A., Lenski, R. E. (1994). Adaptive evolution of highly mutable loci in pathogenic bacteria. *Curr. Biol.* 4(1): 24-33. doi:10.1016/S0960-9822(00)00005-1. Abstract read: no.
From background knowledge: contingency loci with high mutation rates (e.g. simple repeats) are selected where the environment fluctuates, giving local control of variation.
Bearing: supports idea 2 at the level of single parts: locus-specific rates can be selected in pathogens.

**34. [V]** Galhardo, R. S., Hastings, P. J., Rosenberg, S. M. (2007). Mutation as a stress response and the regulation of evolvability. *Crit. Rev. Biochem. Mol. Biol.* 42(5): 399-435. doi:10.1080/10409230701648502. Abstract read: no.
From background knowledge: reviews stress-induced mutagenesis in microbes and the argument that mutation rate is regulated by environmental stress.
Bearing: idea 2; the adaptive interpretation is contested (some see by-products).

**35. [V]** Monroe, J. G., Srikant, T., Carbonell-Bejerano, P., Becker, C., Lensink, M., Exposito-Alonso, M., et al. (2022). Mutation bias reflects natural selection in *Arabidopsis thaliana*. *Nature* 602: 101-105. doi:10.1038/s41586-021-04269-6. Abstract read: partly.
Reports lower mutation rates in genomic regions where mutations are more likely to be deleterious, tested on large surveys of mutations, and reads this as a challenge to the assumption that mutation is random with respect to its consequences.
Bearing: would support idea 2 (rate heterogeneity aligned with importance). Contested, see item 36.

**36. [V]** Liu, H., Zhang, J. (2022). Is the mutation rate lower in genomic regions of stronger selective constraints? *Mol. Biol. Evol.* 39(8): msac169. doi:10.1093/molbev/msac169. Abstract read: yes.
Re-examines the claim in item 35 on other large mutation datasets, and finds that the finding does not generalise as claimed (the abstract opens with the Arabidopsis result and reports a test of its generality).
Bearing: a limit on idea 2. A designer should not cite adaptive mutation-rate heterogeneity as an established fact. (A later reply by the original authors was not sought.)

**37. [V]** Lynch, M., Ackerman, M. S., Gout, J.-F., Long, H., Sung, W., Thomas, W. K., Foster, P. L. (2016). Genetic drift, selection and the evolution of the mutation rate. *Nat. Rev. Genet.* 17: 704-714. doi:10.1038/nrg.2016.104. Abstract read: no (record shows six of seven authors in the first extraction; the full list was not re-checked).
From background knowledge: the drift-barrier hypothesis says that selection lowers mutation rates until the gain is smaller than drift can resolve, so the rate has a floor set by population size.
Bearing: limit for idea 2: pressure on rate is toward lower rates and is weak in small populations; a small agent population will not tune rates finely.

**38. [V]** Rutherford, S. L., Lindquist, S. (1998). Hsp90 as a capacitor for morphological evolution. *Nature* 396: 336-342. doi:10.1038/24550. Abstract read: no.
From background knowledge: Hsp90 buffers hidden variation in *Drosophila*, which appears when the chaperone is impaired, so that stress releases stored variation.
Bearing: supports idea 2 (a buffering element that releases variation under stress). Later work disputes how general and adaptive this is; not surveyed here.

**39. [V]** Otto, S. P., Lenormand, T. (2002). Resolving the paradox of sex and recombination. *Nat. Rev. Genet.* 3: 252-261. doi:10.1038/nrg761. Abstract read: no.
From background knowledge: reviews hypotheses for the maintenance of recombination, among them selection against deleterious mutations in finite populations and the breaking of negative linkage; benefits depend on population size and the strength of selection.
Bearing: the cost-benefit basis for choosing partners and controlling recombination (idea 2); no hypothesis is universal.

**40. [V]** Charlesworth, D., Charlesworth, B., Marais, G. (2005). Steps in the evolution of heteromorphic sex chromosomes. *Heredity* 95: 118-128. doi:10.1038/sj.hdy.6800697. Abstract read: no.
From background knowledge: suppression of recombination between sex chromosomes spreads in steps and leads to degeneration of the non-recombining region.
Bearing: limit for idea 2: suppressing recombination to keep co-adapted parts together has the cost of degeneration.

**41. [V]** Takayama, S., Isogai, A. (2005). Self-incompatibility in plants. *Annu. Rev. Plant Biol.* 56: 467-489. doi:10.1146/annurev.arplant.56.032604.144249. Abstract read: no.
From background knowledge: a single multi-allelic locus lets a plant recognise and reject its own pollen, so that mating partners are controlled by a genome locus.
Bearing: an example of a genome part that decides who may recombine with whom (idea 2).

**42. [V]** Burt, A., Trivers, R. (2006). *Genes in Conflict: The Biology of Selfish Genetic Elements*. Harvard University Press (Belknap). Open Library /works/OL5849376W (author and title match; year 2006 from the record's first listing; publisher not confirmed).
From background knowledge: surveys elements that spread at the expense of the host (transposons, meiotic drivers, B chromosomes) and the host's defences.
Bearing: a warning for ideas 2 and 5: any mechanism that propagates copies and lets parts control their own spread invites selfish variants; genome defence (checks) is needed.

### 2.8 Clonal and sexual propagation in plants

**43. [V]** Vallejo-Marín, M., Dorken, M. E., Barrett, S. C. H. (2010). The ecological and evolutionary consequences of clonality for plant mating. *Annu. Rev. Ecol. Evol. Syst.* 41: 193-213. doi:10.1146/annurev.ecolsys.110308.120258. Abstract read: yes (partly).
Many flowering plants use both sexual and clonal reproduction; clonal growth produces ramets of one genet. The review treats how clonality changes mating, including geitonogamy and reduced seed production.
Bearing: supports idea 5, with limits: clonal spread can lower sexual fitness and seed set, so the two modes interact rather than simply add.

### 2.9 Iterated transmission and bottlenecks

**44. [V]** Kirby, S., Cornish, H., Smith, K. (2008). Cumulative cultural evolution in the laboratory: an experimental approach to the origins of structure in human language. *Proc. Natl. Acad. Sci. USA* 105(31): 10681-10686. doi:10.1073/pnas.0707835105. Abstract read: yes (partly).
In chains of learners, each trained on the previous one's output, an initially random artificial language becomes structured and easier to learn.
Bearing: supports idea 4: transmission through a learner favours compressible, generalisable structure. The structure that appears is the learners' preference.

**45. [V]** Griffiths, T. L., Kalish, M. L. (2007). Language evolution by iterated learning with Bayesian agents. *Cogn. Sci.* 31(3): 441-480. doi:10.1080/15326900701326576. Abstract read: yes (Crossref record and web search summary).
With learners that sample from their posterior, iterated learning converges to the prior; with learners that choose the most probable hypothesis, the outcome depends on both prior and data.
Bearing: states the limit in idea 4: a bottleneck passes content toward the copier's prior. Applied to a language model that redevelops a unit from its genome, the prior is the model's.

**46. [V]** Perez, J., Kovač, G., Léger, C., Colas, C., Molinaro, G., Derex, M., Oudeyer, P.-Y., Moulin-Frier, C. (2024). When LLMs play the telephone game: cumulative changes and attractors in iterated cultural transmissions. arXiv:2407.04503 (v1 titled as above; the ICLR 2025 version is titled "...Cultural attractors as conceptual tools to evaluate LLMs in multi-turn settings"). Abstract read: yes (arXiv record; first author given as Jérémy Perez; full author list beyond four names was taken from the web summary, hence the details of the middle names are not checked).
Chains of LLMs rewrite the text they receive; the authors find cumulative biases and attractors, whose strength depends on the starting text, the instructions, the model and its size, and which are stronger under open-ended instructions.
Bearing: direct evidence for the limit in idea 4. Constrained tasks reduce drift, which supports redevelopment from a precise genome with checks.

**47. [V]** Acerbi, A., Stubbersfield, J. M. (2023). Large language models show human-like content biases in transmission chain experiments. *Proc. Natl. Acad. Sci. USA* 120(44): e2313790120. doi:10.1073/pnas.2313790120. Abstract read: yes (partly).
Using transmission-chain designs from cultural evolution, shows that a model's outputs favour some content types over others, as humans do.
Bearing: content biases of the copier (idea 4) are measurable and not random.

**48. [V]** Shumailov, I., Shumaylov, Z., Zhao, Y., Papernot, N., Anderson, R., Gal, Y. (2024). AI models collapse when trained on recursively generated data. *Nature* 631: 755-759. doi:10.1038/s41586-024-07566-y. Abstract read: no (record shown, abstract failed to print).
From background knowledge: models trained recursively on their own output lose the tails of the original distribution (model collapse).
Bearing: a danger in idea 4 if each generation's text replaces the previous one without any anchor to outside data.

**49. [V]** Gerstgrasser, M., Schaeffer, R., Dey, A., Rafailov, R., et al. (2024). Is model collapse inevitable? Breaking the curse of recursion by accumulating real and synthetic data. arXiv:2404.01413. Abstract read: yes (arXiv record; further authors not listed here).
Earlier collapse results assumed that new data replace old; when synthetic data accumulate beside the real data, collapse is avoided in the settings studied.
Bearing: a counterweight to item 48: keeping the older material (the lineage, or the body beside the genome) avoids collapse in those settings; a support for idea 5 (two modes, an accumulating record).

### 2.10 Succession, disturbance, islands and metapopulations

**50. [V]** Connell, J. H., Slatyer, R. O. (1977). Mechanisms of succession in natural communities and their role in community stability and organization. *Am. Nat.* 111(982): 1119-1144. doi:10.1086/283241. Abstract read: no.
From background knowledge: distinguishes facilitation, tolerance and inhibition as mechanisms by which early colonisers affect later species.
Bearing: bears on founding a population on bare material: early agents may prepare the ground, tolerate or block later ones; only one of the three is facilitation.

**51. [V]** Connell, J. H. (1978). Diversity in tropical rain forests and coral reefs. *Science* 199(4335): 1302-1310. doi:10.1126/science.199.4335.1302. Abstract read: yes.
Proposes the intermediate disturbance hypothesis: high diversity is a non-equilibrium state that, undisturbed, declines toward low diversity, and it is maintained by disturbances at intermediate frequency or intensity.
Bearing: supports a policy of moderate, not zero or maximal, turnover of variants. The hypothesis has since been criticised in the ecological literature; that criticism was not surveyed here.

**52. [V]** MacArthur, R. H., Wilson, E. O. (1967). *The Theory of Island Biogeography*. Princeton University Press. Open Library /works/OL5605777W (title, authors, year of the first record match; publisher not confirmed).
From background knowledge: species number on an island is a balance of immigration and extinction, depending on area and distance.
Bearing: a model for several linked populations of different sizes and distances; transfer to projects is an analogy.

**53. [V]** Levins, R. (1969). Some demographic and genetic consequences of environmental heterogeneity for biological control. *Bull. Entomol. Soc. Am.* 15(3): 237-240. doi:10.1093/besa/15.3.237. Abstract read: no.
From background knowledge: introduces the metapopulation as a population of populations in patches that go extinct and are recolonised.
Bearing: a frame for several linked populations (patch loss and recolonisation).

**54. [V]** Hanski, I. (1998). Metapopulation dynamics. *Nature* 396: 41-49. doi:10.1038/23876. Abstract read: no.
From background knowledge: reviews empirical and theoretical metapopulation work, including the importance of patch size and connectivity.
Bearing: as item 53; the survival of the whole depends on connectivity.

### 2.11 Compact description

**55. [V]** Rissanen, J. (1978). Modeling by shortest data description. *Automatica* 14(5): 465-471. doi:10.1016/0005-1098(78)90005-5. Abstract read: no.
The minimum description length principle: prefer the model that gives the shortest total code for the model and the data.
Bearing: a formal frame for "compact genome": it gives a criterion (genome length plus residual), but says nothing about whether a model can redevelop the body.

**56. [V]** Li, M., Vitányi, P. (1993; later editions 2008, 2019). *An Introduction to Kolmogorov Complexity and Its Applications*. Springer. Open Library /works/OL1332298W (title, authors, 1993 match; publisher not confirmed).
From background knowledge: Kolmogorov complexity is the length of the shortest program that produces an object; it is uncomputable.
Bearing: a limit: compactness has no computable measure; in practice only bounds from a particular compressor or generator can be reported.

### 2.12 Error threshold

**57. [V]** Eigen, M. (1971). Selforganization of matter and the evolution of biological macromolecules. *Naturwissenschaften* 58: 465-523. doi:10.1007/BF00623322. Abstract read: no.
From background knowledge: the error threshold: above a copying-error rate set by genome length and selective advantage, information in the master sequence is lost.
Bearing: sets a limit for idea 1 and 4: the allowed variation rate falls as the genome lengthens; a long genome needs a lower rate, or error correction.

**58. [V]** Bull, J. J., Meyers, L. A., Lachmann, M. (2005). Quasispecies made simple. *PLoS Comput. Biol.* 1(6): e61. doi:10.1371/journal.pcbi.0010061. Abstract read: no.
From background knowledge: restates quasispecies theory, and discusses that the error threshold is sensitive to the fitness landscape and may not apply to the landscapes of real viruses.
Bearing: the threshold is a landscape-dependent bound, not a universal constant (idea 1 and 4).

## 3. Synthesis by design idea

### 3.1 Idea 1. A compact genome from which the body is redeveloped and checked

Supported. Generative and indirect encodings in evolutionary computation (items 19 to 23) show that compact descriptions with reuse and regularity can generate large, valid bodies, and that they can be evolved. Degeneracy (15, 16) supports allowing several bodies for one genome. Canalisation (17) names the check that holds the body to the genome.

Limits. Benefits shown are for bodies with regularity or geometry (HyperNEAT, generative representations); no cited work tests a text genome developed by a language model. The error threshold (57, 58) says that the tolerable variation falls as the genome grows, and that it depends on the landscape. Compactness has no computable measure (56); only a particular compressor gives a bound.

Not to claim: that the genome is the project's "DNA" in any strict sense; that a bounded text determines the body (the model's prior and its sampling add content); that compact means better.

### 3.2 Idea 2. Parts of the genome that control evolution

Partly supported. Mutator alleles hitchhike (31, 32), contingency loci show locus-specific rates (33), stress-induced mutagenesis is described (34), capacitance (38), self-incompatibility loci (41) and recombination modifiers (39, 40) show controls over rate and partners. Regulatory evolution is a main route of morphological change (18).

Limits and critiques. Whether evolvability itself is selected is open (4, 5, 6, 7); the argument needs long horizons. Mutators are linked to a load, and the drift-barrier model (37) says that rates are pushed down, and weakly so in small populations. Rate heterogeneity aligned with importance (35) is contested (36). Suppressing recombination carries degeneration (40). Selfish elements exploit any such control (42). Capacitance is debated.

Not to claim: that evolvability evolves as a settled fact; that a genome can reliably tune its own mutation rate per part; that Monroe et al. (2022) show adaptive mutation bias.

### 3.3 Idea 3. Modular variation follows a changing environment more cheaply

Supported under conditions. Modularly varying goals produce modularity and speed up evolution in simulation (8, 9, 3); facilitated variation (1, 2) and the regulatory-evolution view (18) give a biological frame; Simon (11) gives the classical argument.

Limits. The speed-up needs environments that vary modularly (8, 9) and new goals that share structure with old ones (3); modularity does not arise from performance pressure alone and needs a cost on connections (10). Results are for small simulated networks. Whether a software project's changes are modular in that sense is an empirical question.

Not to claim: that modular variation is cheaper in general; that the design in itself creates modularity.

### 3.4 Idea 4. A compact bottleneck purges damage, favours compressible structure, may regress to the prior

Supported on both sides. Purging: germline bottlenecks in mitochondria (28), germ-soma separation (24, 25, 26) and Muller's ratchet (29) give the case for a restricted stage as a defence against accumulation. Structure: iterated learning produces compressible structure (44). Regression: iterated learning with Bayesian learners converges to the prior (45); LLM chains show attractors and content biases (46, 47); recursive training loses tails (48), though accumulation of data avoids collapse in studied settings (49).

Limits. Bottleneck benefits depend on parameters and the bottleneck has costs (28); many multicellular lineages have no strict germline (25). The prior of a language model is large and not under the designer's control.

Not to claim: that a bottleneck is always beneficial; that compressibility is the same as quality; that the redeveloped body equals the earlier body in content. A designer should measure drift and add checks and outside anchors.

### 3.5 Idea 5. Copying the body and redevelopment from the genome coexist

Supported by plants (43) and by the ratchet argument (29): clonal copying keeps proven bodies cheaply, and a sexual or seed route renews them. Selfish elements (42) and sole-copier effects (30) are cautions.

Limits. In plants the two modes interact, and clonality can reduce seed output (43). Clonal lineages accumulate deleterious change (29). Recombination has benefits that depend on conditions (39).

Not to claim: that two modes are independent, or that the combination escapes both costs.

### 3.6 Population and ecological frame (no design idea numbered)

Succession (50) distinguishes facilitation from inhibition, so the first agents on bare material do not necessarily prepare the way for later ones. Intermediate disturbance (51) is contested. Island biogeography and metapopulations (52, 53, 54) are analogies for several linked populations; their parameters (area, distance, patch connectivity) have no direct counterparts in a project.

## 4. References with marks

[V] Kirschner and Gerhart 1998, doi:10.1073/pnas.95.15.8420
[V] Gerhart and Kirschner 2007, doi:10.1073/pnas.0701035104
[V] Parter, Kashtan, Alon 2008, doi:10.1371/journal.pcbi.1000206
[V] Wagner and Altenberg 1996, doi:10.1111/j.1558-5646.1996.tb02339.x
[V] Pigliucci 2008, doi:10.1038/nrg2278
[V] Sniegowski and Murphy 2006, doi:10.1016/j.cub.2006.08.080
[V] Payne and Wagner 2019, doi:10.1038/s41576-018-0069-z
[V] Kashtan and Alon 2005, doi:10.1073/pnas.0503610102
[V] Kashtan, Noor, Alon 2007, doi:10.1073/pnas.0611630104
[V] Clune, Mouret, Lipson 2013, doi:10.1098/rspb.2012.2863
[P] Simon 1962, JSTOR 985254 (not opened)
[V] Wagner 2008, doi:10.1098/rspb.2007.1137
[V] Wagner 2005, doi:10.1016/j.febslet.2005.01.063
[P] Schuster et al. 1994, doi:10.1098/rspb.1994.0040 (authors from memory)
[V] Edelman and Gally 2001, doi:10.1073/pnas.231499798
[V] Whitacre 2010, doi:10.1186/1742-4682-7-6
[V] Waddington 1942, doi:10.1038/150563a0
[V] Carroll 2008, doi:10.1016/j.cell.2008.06.030
[V] Stanley and Miikkulainen 2003, doi:10.1162/106454603322221487
[V] Stanley 2007, doi:10.1007/s10710-007-9028-8
[V] Stanley, D'Ambrosio, Gauci 2009, doi:10.1162/artl.2009.15.2.15202
[V] Hornby and Pollack 2002, doi:10.1162/106454602320991837
[V] O'Neill and Ryan 2001, doi:10.1109/4235.942529
[V] Buss 1987, Open Library OL4987903W
[V] Grosberg and Strathmann 2007, doi:10.1146/annurev.ecolsys.36.102403.114735
[V] Michod 2007, doi:10.1073/pnas.0701489104
[V] Kirkwood 1977, doi:10.1038/270301a0
[V] Bergstrom and Pritchard 1998, doi:10.1093/genetics/149.4.2135
[V] Muller 1964, doi:10.1016/0027-5107(64)90047-8
[V] Birky 2001, doi:10.1146/annurev.genet.35.102401.090231
[V] Sniegowski, Gerrish, Lenski 1997, doi:10.1038/42701
[V] Taddei et al. 1997, doi:10.1038/42696
[V] Moxon et al. 1994, doi:10.1016/S0960-9822(00)00005-1
[V] Galhardo, Hastings, Rosenberg 2007, doi:10.1080/10409230701648502
[V] Monroe et al. 2022, doi:10.1038/s41586-021-04269-6
[V] Liu and Zhang 2022, doi:10.1093/molbev/msac169
[V] Lynch et al. 2016, doi:10.1038/nrg.2016.104
[V] Rutherford and Lindquist 1998, doi:10.1038/24550
[V] Otto and Lenormand 2002, doi:10.1038/nrg761
[V] Charlesworth, Charlesworth, Marais 2005, doi:10.1038/sj.hdy.6800697
[V] Takayama and Isogai 2005, doi:10.1146/annurev.arplant.56.032604.144249
[V] Burt and Trivers 2006, Open Library OL5849376W
[V] Vallejo-Marín, Dorken, Barrett 2010, doi:10.1146/annurev.ecolsys.110308.120258
[V] Kirby, Cornish, Smith 2008, doi:10.1073/pnas.0707835105
[V] Griffiths and Kalish 2007, doi:10.1080/15326900701326576
[V] Perez et al. 2024, arXiv:2407.04503
[V] Acerbi and Stubbersfield 2023, doi:10.1073/pnas.2313790120
[V] Shumailov et al. 2024, doi:10.1038/s41586-024-07566-y
[V] Gerstgrasser et al. 2024, arXiv:2404.01413
[V] Connell and Slatyer 1977, doi:10.1086/283241
[V] Connell 1978, doi:10.1126/science.199.4335.1302
[V] MacArthur and Wilson 1967, Open Library OL5605777W
[V] Levins 1969, doi:10.1093/besa/15.3.237
[V] Hanski 1998, doi:10.1038/23876
[V] Rissanen 1978, doi:10.1016/0005-1098(78)90005-5
[V] Li and Vitányi, Open Library OL1332298W
[V] Eigen 1971, doi:10.1007/BF00623322
[V] Bull, Meyers, Lachmann 2005, doi:10.1371/journal.pcbi.0010061

Count: 56 listed in section 2 plus items 57 and 58 = 58 works; 56 marked [V] (bibliographic record opened; abstract read for about 22 of them), 2 marked [P]; none [U]. Weismann's texts were not found or opened.
