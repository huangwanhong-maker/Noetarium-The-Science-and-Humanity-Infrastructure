# The algebra and category theory of resources, allocations and processes

Markers: **[V]** verified by reading the source itself during this research (for arXiv items marked [V abstract], only the abstract page was read; for items marked [V text], the full text was read and the definition or theorem number is given); **[S]** from a search snippet or a secondary summary only; **[M]** from memory, not checked during this research. Researched 2026-11-20. Full texts read: Coecke–Fritz–Spekkens (2016), Fritz (2017), Jung et al., "Iris from the ground up" (2018), Moraglio's thesis (2007) and Radcliffe (1994). The last two came from copies another researcher had saved in the shared scratchpad.

## 1. The most suitable formal notion of a "resource type" for allocation

### Takeaway
The best-supported notion has two levels. A resource *theory* is a symmetric monoidal category (SMC), whose objects are resources and whose morphisms are the free conversions. Its decategorification, which records only whether a conversion exists, is an **ordered (preordered) commutative monoid** (OCM). Divisibility is extra structure on top of this: a semimodule action of ℚ≥0 or ℝ≥0, or a closure of the monoid under fractions. Copyable ("quality-like") and consumable ("quantity-like") resources differ only in their monoid law: idempotent `max`/`∨` against `+`. Distinct, labelled items are best described by a combinatorial species, or a partial commutative monoid, on top of the OCM. In practice a resource type should be a **product of typed components**: each kind gets an OCM over a quantity semiring, with declared properties (divisible, cancellative, disposable, splittable, idempotent) and an optional validity predicate that makes combination partial.

### Cited Findings

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

**1.2 Ordered commutative monoids and their regularizations (Fritz 2017)**
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
- Theorem 8.24 [V text]: the maximal regularized rate satisfies R^reg_max(x→y) = inf_f f(x)/f(y), over functionals with f(y)≠0, assuming x,y ≥ 0. This gives a new formula for the Shannon capacity of a graph. Section 9 relates embedding an OCM into ℝ, "an essentially unique way of assigning a value or price", to von Neumann–Morgenstern utility and to Lieb–Yngvason. — [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
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

### Inferences
- The OCM is the **right minimal core**. It is the exact decategorification of the SMC picture (CFS Theorem 4.2) and contains every law that "how amounts combine" and "how they are ordered" need. Everything else (divisibility, distinctness, partiality, copyability) can be declared as extra structure or properties on top of it.
- **Divisibility has two distinct meanings**, and a framework should keep them apart:
  - *physical divisibility* is a semimodule action of ℚ≥0 or ℝ≥0 on the carrier (a cone), with Riesz decomposition for splitting;
  - *asymptotic divisibility* is Fritz's many-copy regularization (formal fractions) used for rates.

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

### Gaps
- I did not read Gonda–Spekkens beyond the abstract, so the exact form of their "framework where composition does not depend on the type of resource" is unverified.
- I did not find a source that defines a single formal object which unifies divisible, indivisible, labelled and copyable resources. The product construction proposed below is my synthesis, not a published definition.
- I did not open Bergeron–Labelle–Leroux (1998), Flajolet–Sedgewick (2009) or Yorgey's thesis, so the species facts above are [M].
- I did not find whether existence of catalysis is decidable in finitely presented OCMs.

## 2. Allocations and conversions as morphisms, and their laws

### Takeaway
A conversion is a morphism in a resource SMC. Free conversions are an all-object sub-SMC (CFS's "partitioned process theory"), and convertibility a≽b is the existence of such a morphism. An allocation of a pool among recipients is a morphism `pool → r₁ ⊗ … ⊗ rₙ`. When ⊗ is the monoid +, this is a decomposition pool ≃ Σ rᵢ, or pool ≽ Σ rᵢ with disposal. Petri nets give the concrete, finitely presentable case: the free commutative monoid of markings, with transitions as generating conversions. Separation logic and Iris give the case where combination is *partial* (distinct or exclusive items) and conversions must be *frame-preserving*. The laws are:
- associativity, commutativity and unit of combination;
- monotonicity (x≥y ⇒ x+z ≥ y+z);
- conservation, as invariant homomorphisms;
- splittability (Riesz decomposition or refinement);
- for partial resources, validity closed under sub-bundles and frame preservation.

### Cited Findings

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

### Inferences
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

### Gaps
- I found no paper that formalises *fair division* or *allocation mechanisms* specifically as morphisms in a resource SMC. The bridge between resource theories and the fair-division literature (Bei et al., Bogomolnaia et al.) appears to be open; I did not search exhaustively for "categorical fair division".
- I did not read Meseguer–Montanari, O'Hearn–Pym or Calcagno–O'Hearn–Yang in full; their details are [S].

## 3. Categorical structures for order-dependent (sequential, stateful) allocation

### Takeaway
Sequential allocation is composition (◦) where simultaneous allocation is ⊗. Its standard algebraic carriers are:
- the free monoid Σ* of allocation steps acting on a state set (deterministic policies);
- Kleisli categories of a monad, giving powerset nondeterminism or distribution-monad stochasticity (Markov categories);
- lenses or optics, for parametrised processes with a backward pass (learning, optimisers, games' coutility);
- operads of wiring diagrams and doctrines of systems theory, for composing open stateful systems;
- the selection monad (X→R)→X, for search and backward induction.

Order-dependence is the failure of steps to commute. It can be expressed by asking whether the action factors through a partially commutative ("trace") monoid.

### Cited Findings
- Fong, Spivak and Tuyéras, "Backprop as functor" (LICS 2019) [V abstract]. They define a category whose morphisms are update rules (learners) and show that gradient descent with backpropagation "defines a monoidal functor from a category of parametrised functions to this category of update rules". — [arXiv:1711.10455](https://arxiv.org/abs/1711.10455)
- Cruttwell, Gavranović, Ghani, Wilson and Zanasi, "Categorical foundations of gradient-based learning" [V abstract]. Their semantics is built from lenses, parametrised maps and reverse derivative categories, and covers ADAM, AdaGrad, Nesterov momentum, MSE and softmax cross-entropy. It extends beyond smooth settings to Boolean circuits and has a Python implementation. The arXiv page lists no journal; it appeared at ESOP 2022 [M]. — [arXiv:2103.01931](https://arxiv.org/abs/2103.01931)
- Gavranović, Lessard, Dudzik, von Glehn, Araújo and Veličković, "Categorical deep learning is an algebraic theory of all architectures" (ICML 2024, position paper) [V abstract]. They propose "the universal algebra of monads valued in a 2-category of parametric maps" as one theory for both constraints and implementations of architectures. — [arXiv:2402.15332](https://arxiv.org/abs/2402.15332)
- Spivak, "The operad of wiring diagrams" [V abstract]: wiring diagrams nest, and so are the morphisms of an operad. The paper uses them to structure database queries and circuits, and extends them to plug-and-play devices and recursion. — [arXiv:1305.0297](https://arxiv.org/abs/1305.0297)
- Myers, categorical systems theory [S]. The book is a work in progress on the author's site. A "doctrine" is a 2-functor from a theory category into monoidal double categories of systems. Systems compose in three ways: by setting parameters (lenses), by sharing variables (spans) and by exposing interfaces. See "Double categories of open dynamical systems" (2020) and Libkind and Myers, "Towards a double operadic theory of systems" (2025). — [Myers JMM 2023 talk](https://www.davidjaz.com/Talks/DJM-JMM-2023.pdf); [arXiv:2005.05956](https://arxiv.org/abs/2005.05956); [arXiv:2505.18329](https://arxiv.org/abs/2505.18329)
- Fritz, "A synthetic approach to Markov kernels, conditional independence and theorems on sufficient statistics", *Adv. Math.* 370 (2020) 107239 [V abstract]. Markov categories give a synthetic treatment of conditioning, disintegration, conditional independence and sufficient statistics, uniform across discrete and measure-theoretic probability, "following work of Golubtsov as well as Cho and Jacobs". — [arXiv:1908.07021](https://arxiv.org/abs/1908.07021); Cho and Jacobs [M]: [arXiv:1709.00322](https://arxiv.org/abs/1709.00322)
- Escardó and Oliva, "Selection functions, bar recursion and backward induction", *MSCS* 20 (2010) 127–168 [S]. J_R X = (X→R)→X is the selection monad, a strong monad. Products of selection functions compute backward induction, and finite bar recursion is a morphism available in any strong monad. Later work extends it to a selection monad transformer for backtracking search. — [DOI 10.1017/S0960129509990351](https://doi.org/10.1017/S0960129509990351); [author's PDF](https://martinescardo.github.io/papers/selection-escardo-oliva.pdf)
- In open games [V abstract], sequential composition and ⊗ ("simultaneous moves") are the two compositions, so the order of play is the order of composition. — [arXiv:1603.04641](https://arxiv.org/abs/1603.04641)
- Baez–Master's operational semantics is "a category of processes a net can carry out" [V abstract]: the free (commutative) monoidal category of firing sequences, whose composition is sequencing. — [arXiv:1808.05415](https://arxiv.org/abs/1808.05415)

### Inferences
- **Deterministic sequential allocation**: a policy is an action of the free monoid Σ* of steps (assign item i to agent j; convert by rule t) on the state set S of partial allocations. The result is order-independent exactly when the action factors through the free commutative monoid ℕ^Σ. When only some steps commute (steps touching disjoint recipients or items), it factors through the free partially commutative monoid. That is the trace monoid of Cartier–Foata and Mazurkiewicz [M], and its independence relation is exactly the "frame" disjointness of separation logic. A finite fragment can be checked: the action commutes for each independent pair of generators at each reachable state.
- **Stochastic sequential allocation** (evolutionary operators, randomised mechanisms) lives in the Kleisli category of the distribution monad, which is a Markov category (Fritz 2020). Finite instances are column-stochastic matrices over ℝ≥0, which is a semiring model a matrix-based library can decide laws in.
- **Allocation with feedback** (stateful mechanisms, learning which allocation is good) has the shape of a lens or parametrised map (Fong–Spivak–Tuyéras; Cruttwell et al.). A forward pass allocates; a backward pass returns a utility or loss, which is the coutility of open games.
- **Search over allocations** is a selection function (X→R)→X in Escardó–Oliva's sense. Composing selection functions for sub-problems gives backward induction. An evolutionary algorithm is a stochastic selection function, i.e. a Kleisli arrow (X→R)→D(X). This is my inference; I found no paper stating it (§5).
- Traced monoidal categories (Joyal, Street and Verity 1996) [M] would express loops and feedback of resources, such as a catalyst returned or a stock carried to the next turn. I found nothing on their use for allocation.

### Gaps
- I found no source that treats sequential allocation (picking sequences, serial dictatorship) categorically.
- Myers's book status and content are [S]; I did not read the book.
- I did not check whether Cruttwell et al. or open games give lawful conditions (lens laws) for optimisers that a library could decide on finite instances.

## 4. Laws that can be decided mechanically on finite fragments or presentations, and what a finite verdict establishes

### Takeaway
Four kinds of verdict must be kept apart:
1. **Proved by construction or universal property.** The carrier is built from verified primitives by closure operations that preserve the laws (varieties, ordered varieties, free constructions), or a homomorphism law reduces to finitely many checks on generators.
2. **Decided on a finite model.** Every tuple of a finite carrier is checked. This establishes the law *for that model only*.
3. **Decided for a finite presentation.** Examples are convertibility in a finitely presented OCM or Petri net (decidable, but EXPSPACE-complete for commutative-semigroup equality and Ackermann-complete for Petri-net reachability) and equality by Knuth–Bendix when completion terminates.
4. **Sampled.** Property-based tests on samples are evidence, never proof.

Refutations (a counterexample, or a monotone M with M(a) < M(b)) are definitive in all four cases.

### Cited Findings
- **The project's category library** [V text, this repository's CLAUDE.md, §3.1, the kategoria row]. kategoria answers every claim with a verdict: holds, fails or undecided, with the place and the reason. It decides claims on finite categories, functors (found by exhaustive search), limits and colimits, set-valued functors and Kan extensions. Presentations are completed by Knuth–Bendix on paths, "with derivations a small checker verifies". Symmetric monoidal theories have string diagrams "equal exactly when isomorphic" and "models in matrices". — `c:\Users\Micro\Desktop\The Science\CLAUDE.md` §3.1
- **Word problem for commutative semigroups** [S]. Mayr and Meyer, "The complexity of the word problems for commutative semigroups and polynomial ideals", *Adv. Math.* 46(3) (1982) 305–329: any decision procedure needs space exponential in the instance size, and a simple procedure achieves the bound. Finitely presented commutative semigroups are equivalent to reversible Petri nets or vector addition systems. — [MIT DSpace](https://dspace.mit.edu/handle/1721.1/149010); [Koppenhagen and Mayr 1996](https://mediatum.ub.tum.de/doc/1094422/1094422.pdf)
- **Petri-net reachability** [S] is decidable and Ackermann-complete:
  - the lower bound was found independently in 2021 by Czerwiński and Orlikowski ("Reachability in vector addition systems is Ackermann-complete", FOCS) and by Leroux ("The reachability problem for Petri nets is not primitive recursive");
  - the matching upper bound is Leroux and Schmitz (2019).

  — [arXiv:2104.13866](https://arxiv.org/abs/2104.13866); [arXiv:2104.12695](https://arxiv.org/abs/2104.12695); [Lasota, arXiv:2105.08551](https://arxiv.org/abs/2105.08551)
- **Ordered analogue of Birkhoff's HSP theorem** [S]. Bloom, "Varieties of ordered algebras", *JCSS* 13(2) (1976) 200–212: a class of ordered algebras is axiomatisable by term inequations iff it is closed under subalgebras, products and suitable homomorphic images. The paper also gives a complete inequational logic. A modern statement is in arXiv:1812.02016. — [DOI 10.1016/S0022-0000(76)80030-X](https://doi.org/10.1016/S0022-0000(76)80030-X); [arXiv:1812.02016](https://arxiv.org/abs/1812.02016)
- **Monotones as certificates of impossibility** [V text]: if M(a) < M(b) then a⋡b, but the converse fails in general. A complete family always exists, but it is indexed by R itself (CFS Proposition 5.2). In the Archimedean ℚ-vector-space regularization, functionals are complete (Fritz Theorem 6.15). — [arXiv:1409.5531](https://arxiv.org/abs/1409.5531); [arXiv:1504.03661](https://arxiv.org/abs/1504.03661)
- **Laws proved generically, then instantiated** [V text]. Iris proves frame-preserving updates once for constructions (sum, exclusive, finite maps, pointwise in products), and every RA built from them inherits them. This is the "by construction" route in a proof assistant. — [Iris JFP](https://cs.au.dk/~birke/papers/iris-journal.pdf)
- Classical references, not checked [M]:
  - Birkhoff (1935): equational classes are exactly those closed under H, S and P;
  - Knuth and Bendix (1970), "Simple word problems in universal algebras": when completion terminates with a convergent system, the word problem is decided by normal forms, and it may fail to terminate;
  - Claessen and Hughes, "QuickCheck", ICFP 2000, DOI 10.1145/351240.351266: random testing of stated properties, which finds counterexamples but proves nothing;
  - for commutative presentations, Gröbner bases of binomial ideals play the part of Knuth–Bendix;
  - Petri-net place invariants (vectors y with yᵀC = 0, where C is the incidence matrix) are linear-algebra certificates of conservation.

### Inferences
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

### Gaps
- I did not verify the precise theorem statements of Mayr–Meyer, Bloom or Knuth–Bendix (all [S] or [M]).
- I found no literature on the decidability or complexity of computing Fritz's regularized rates, or of deciding catalysis, for finitely presented OCMs.
- I did not find a published tool that verifies user-declared resource-type laws other than Iris, in Coq, which is proof-based, not decision-based.

## 5. Prior work using category theory to specify or verify evolutionary algorithms, search spaces or genetic operators

### Takeaway
I found **no primary source that specifies evolutionary algorithms or genetic operators categorically**: no functorial or monadic account of crossover or mutation, and no categorical verification of EA laws. Several targeted searches returned nothing. The closest prior work is *algebraic*:
- Radcliffe's forma analysis: equivalence relations generate representations and operators;
- Rowe, Vose and Wright: permutation-group symmetries and invariant operators;
- Moraglio's geometric framework: operators defined by the axioms of a metric, with offspring in metric segments;
- Mitavskiy: comparing EAs via re-encodings.

Adjacent categorical work exists: categorical deep learning (monads in parametric maps), lenses for optimisers, and the selection monad for search. This is a genuine gap that a framework could fill.

### Cited Findings
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

### Inferences
- The algebraic EA literature already supplies **laws** a categorical specification could state and decide:
  - Rowe–Vose–Wright give *equivariance*: an operator commutes with a symmetry group's action, a naturality square checkable on finite search spaces;
  - Moraglio gives *geometricity*: Im[CX(p₁,p₂)] ⊆ [p₁;p₂]_d, decidable on finite metric spaces;
  - Radcliffe gives *respect and assortment* of formae, conditions on equivalence relations;
  - a further natural law is that an operator is a Kleisli arrow of the distribution monad (a Markov kernel), whose finite instances are stochastic matrices.
- A typed account fits the user's resource framework. Each resource type determines a search space, its allocation species. Operators are then natural transformations between species, labelled-structure maps that commute with relabelling, so that equivariance under renaming of items or agents is naturality. Mutation is a Markov kernel F → D(F); crossover is F × F → D(F). This is my proposal, not found in the literature.

### Gaps
- Coverage of GECCO, FOGA and ACT proceedings was by web search only; a systematic search of the ACT proceedings and dblp might find workshop papers I missed.
- I did not read Mitavskiy 2003, Rowe–Vose–Wright 2002 or Kozyrev 2023 in full.

## What this means for a universal framework

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
