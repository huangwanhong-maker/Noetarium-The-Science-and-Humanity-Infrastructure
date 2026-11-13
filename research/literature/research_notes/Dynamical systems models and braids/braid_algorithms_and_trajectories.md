# The Computational Theory of Braid Groups and the Extraction of Braids from Trajectories

Status: research notes, compiled 2026-11-12. State of knowledge as of November 2026.

Reading conventions for these notes.

- A bullet under *Cited Findings* was read in the linked source during this session (through a search excerpt or a fetched page); where only an excerpt or an abstract was read, the bullet says so.
- A bullet marked **[from memory]** was not confirmed against a source in this session. It is believed correct but must be checked before it is cited or implemented.
- A bullet marked **[inference]** is this note's own reasoning.
- The verdicts on the brief's from-memory claims are collected at the head of each section's *Takeaway* and repeated in §10.

---

## 1. Algorithms in the Braid Group B_n

### Takeaway

Three independent exact solutions of the word problem are practical and cheap to implement: the left-greedy (Garside–Thurston–El-Rifai–Morton) normal form, O(ℓ² n log n); Dehornoy's handle reduction, whose complexity is still unproved but which is the fastest in practice; and Dynnikov's integer coordinates, quadratic time with only additions and maxima. The simplest to implement correctly are handle reduction and Dynnikov coordinates (a few dozen lines each); the greedy normal form is the one needed anyway for canonical forms, the lattice operations and the conjugacy problem, where sliding circuits (Gebhardt and González-Meneses) are the current standard and no polynomial bound is known.

### Cited Findings

**Presentation and basic structure**

- Artin's presentation: B_n = ⟨σ₁,…,σ_{n−1} | σᵢσⱼ = σⱼσᵢ for |i−j| > 1; σᵢσⱼσᵢ = σⱼσᵢσⱼ for |i−j| = 1⟩ (González-Meneses 2011, eq. (5); proved by combing in §3.1 and by the Fox–Neuwirth complex in §3.2). — [González-Meneses, *Basic results on braid groups*, arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Garside element Δ = σ₁(σ₂σ₁)⋯(σ_{n−1}⋯σ₁), the join of the generators, with σᵢΔ = Δσ_{n−i}; simple elements are the positive prefixes of Δ (permutation braids); centre Z(B_n) = ⟨Δ²⟩ for n > 2 (Theorem 4.3); B_n is torsion-free (four proofs: K(π,1), Nielsen realisation, left-orderability, Garside gcds). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Left normal form: every braid is uniquely Δ^p a₁⋯a_r with each aᵢ a proper simple element (1 ≺ a ≺ Δ) and (aᵢaᵢ₊₁) ∧ Δ = aᵢ (left-weightedness); Thurston's computation time is O(ℓ² n log n) for a word of length ℓ in B_n (§4.1). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Dehornoy's survey gives the same bound: greedy normal form O(ℓ²) for fixed n with precomputed tables, otherwise O(ℓ² n log n) (§2.4.1); the symmetric normal form is quadratic in ℓ for fixed n with an added factor n log n for variable n (§2.4.2). — [Dehornoy, *Efficient solutions to the braid isotopy problem*, arXiv:math/0703666](https://arxiv.org/abs/math/0703666)
- Bibliographic data for the normal-form sources as given in Dehornoy's reference list: Epstein, Cannon, Holt, Levy, Paterson and Thurston, *Word Processing in Groups*, Jones & Bartlett (1992); El-Rifai and Morton, "Algorithms for positive braids", Quart. J. Math. Oxford 45-2 (1994) 479–497; Adyan, "Fragments of the word Delta in a braid group", Mat. Zam. 36-1 (1984) 25–34; Thurston, "Finite state algorithms for the braid group", circulated notes (1988); Dehornoy, "A fast method for comparing braids", Adv. Math. 125 (1997) 200–235. — [arXiv:math/0703666](https://arxiv.org/abs/math/0703666)
- *Word Processing in Groups* (1992) has a second part of four chapters, one of them on braid groups; automatic groups have a normal form whose words form a regular language and to which a word is reduced in quadratic time. — [Wikipedia, *Word Processing in Groups*](https://www.wikipedia.org/wiki/Word_Processing_in_Groups); [Wikipedia, *Automatic group*](https://en.wikipedia.org/wiki/Automatic_group)
- Biautomaticity: Charney showed that braid groups (and spherical Artin–Tits groups) are biautomatic, which gives another solution of the conjugacy problem (§4.4). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)

**Handle reduction (Dehornoy)**

- A σᵢ-handle is a word σᵢ^e w₀ σᵢ₊₁^d w₁ ⋯ σᵢ₊₁^d w_m σᵢ^{−e} with e, d = ±1, m ≥ 0 and each w_k containing no σⱼ^{±1} with j ≤ i+1. Its reduct deletes the two outer letters and replaces each σᵢ₊₁^d by σᵢ₊₁^{−e} σᵢ^d σᵢ₊₁^e. Theorem 3.8 (from Dehornoy 1997): every sequence of handle reductions reaches a reduced word in finitely many steps, and a reduced word represents 1 iff it is empty (§3.2.1, Corollary 3.9). — [arXiv:math/0703666](https://arxiv.org/abs/math/0703666)
- The complexity of handle reduction "remains unknown but [it] is very efficient in practice"; the survey gives no proved bound. — [arXiv:math/0703666](https://arxiv.org/abs/math/0703666)
- Handle reduction "is believed to be of cubic complexity and to yield σ₁-consistent braids of linearly bounded length, but this is currently only a conjecture"; Wiest proposes another algorithm, conjectured quadratic, proved quadratic for 3 strands where it also finds a shortest representative. — [Wiest, *An algorithm for the word problem in braid groups*, arXiv:math/0211169](https://arxiv.org/abs/math/0211169)

**Dynnikov coordinates as a word-problem solver**

- A braid word on n strands is mapped to 2n integers, starting from (0,1,…,0,1); each letter σᵢ^{±1} updates only the two coordinate pairs at positions i and i+1, by eight explicit max-plus formulas (Definition 3.12). A word represents the identity iff its coordinates are (0,1,…,0,1) (Corollary 3.14). Stated cost: linear space and quadratic time; a pseudo-Anosov braid of length ℓ yields integers of O(ℓ) bits. The only operations are addition and maximum. No divide-and-conquer variant is known, and no statistical comparison with handle reduction existed when the survey was written (§3.3). — [arXiv:math/0703666](https://arxiv.org/abs/math/0703666)

**Birman–Ko–Lee (dual) presentation**

- Birman, Ko and Lee give a presentation on band generators a_{ts} (containing the Artin generators) with fundamental word δ = a_{n(n−1)} a_{(n−1)(n−2)} ⋯ a_{21} replacing Δ, and a solution of the word problem "which retains most of the desirable features of the Garside–Thurston solution"; they also give a conjugacy solution whose complexity improvement "is not clear at this writing". Published in Adv. Math. The bound O(|W|² n) for the word problem was seen in secondary summaries returned by the search, not read in the paper's own text in this session. — [Birman, Ko, Lee, arXiv:math/9712211](https://arxiv.org/abs/math/9712211)

**Conjugacy problem**

- Gebhardt and González-Meneses replace cycling and decycling by one operation, *cyclic sliding*, and define the set of sliding circuits SC(x): the conjugates of x that are fixed by some power of cyclic sliding. SC(x) ⊆ USS(x); it is a finite invariant of the conjugacy class; the worst-case complexity of the resulting algorithm is not better than the classical approach. — [Gebhardt and González-Meneses, *The cyclic sliding operation in Garside groups*, arXiv:0808.1430](https://arxiv.org/abs/0808.1430); algorithm in [arXiv:0809.0948](https://arxiv.org/abs/0809.0948)
- Two braids are conjugate iff SC(x) = SC(y), equivalently SC(x) ∩ SC(y) ≠ ∅; SC(x) is finite and its elements are connected by conjugations by simple elements; trying all n! simple elements is "not efficient at all"; no polynomial bound is given (§4.4). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Sets of sliding circuits can have size exponential in the number of strands; the properties of SC extend to elements of summit canonical length 1, which is not so for ultra summit sets. — [González-Meneses and Wiest, *Reducible braids and Garside theory*, arXiv:1008.0238](https://arxiv.org/abs/1008.0238) (as summarised in the search results, with [arXiv:1006.2258](https://arxiv.org/abs/1006.2258))

**Dehornoy's ordering**

- Let P be the set of nontrivial braids expressible by a word in which the generator of smallest index occurs only with positive exponents. Theorem 2.10: B_n = P ⊔ {1} ⊔ P⁻¹; this P defines Dehornoy's left-invariant ordering (§2.3). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Handle reduction ends in a σ-consistent word and so also decides the ordering (see Theorem 3.8 above). — [arXiv:math/0703666](https://arxiv.org/abs/math/0703666)

**Test vectors that exist**

- SageMath returns `left_normal_form()` and `right_normal_form()` as tuples of simple braids with the power of Δ in the first or last slot, and through libbraiding `super_summit_set`, `ultra_summit_set`, `sliding_circuits`, `conjugating_braid`, `is_conjugated`, `centralizer`, `gcd`, `lcm`, `thurston_type`, `rigidity`. — [SageMath reference, braid groups](https://doc.sagemath.org/html/en/reference/groups/sage/groups/braid.html)
- Bigelow's explicit kernel elements of the Burau representation (see §2) are nontrivial braids of lengths 120 (n = 5) and 44 (n = 6): every word-problem solver must report them nontrivial. — [Bigelow, arXiv:math/9904100](https://arxiv.org/abs/math/9904100)

### Inferences

- **[inference] Order of implementation.** (1) Dynnikov coordinates over morphe's unbounded integers: the shortest correct oracle for triviality, no tables, no permutations. (2) Handle reduction: gives triviality and the sign in Dehornoy's ordering; termination is a theorem, the bound is not. (3) Left-greedy normal form on permutations (simple braids as permutations; the left-weighted test by descent sets): needed for canonical forms, gcd/lcm, and conjugacy. The three share no code, so each checks the others, which suits a library whose claims are answered with evidence.
- **[inference]** The conjugacy problem should be offered with its cost stated: SC is finite and decides conjugacy, but its size is not polynomially bounded; for n ≤ 4 and short braids it is cheap.
- **[from memory]** The automaton recognising left normal forms has the simple braids (n! states) as states, with a transition a → b exactly when the pair (a, b) is left-weighted, i.e. the starting set S(b) is contained in the finishing set F(a) (El-Rifai and Morton 1994); in terms of permutations these are descent sets. The exact left/right descent convention depends on how a permutation is attached to a simple braid and must be fixed by tests against Sage.
- **[from memory]** Super summit sets are due to El-Rifai and Morton (1994); ultra summit sets to Gebhardt, "A new approach to the conjugacy problem in Garside groups", J. Algebra 292 (2005) 282–302. The journal data of the cyclic sliding papers (Math. Z. 265 (2010) 85–114; J. Symbolic Comput. 45 (2010) 629–656) and of Charney's theorem (Math. Ann. 292 (1992)) are from memory.
- **[from memory]** Periodic-braid identities useful as tests: with δ = σ₁⋯σ_{n−1} and ε = σ₁(σ₁⋯σ_{n−1}), δⁿ = Δ² = ε^{n−1}.

### Gaps

- The braid chapter of Epstein et al. was not read; its bound is reported here through González-Meneses and Dehornoy.
- The explicit max-plus formulas of Dehornoy's Definition 3.12 (2n coordinates) were not copied; the equivalent update rules in Thiffeault's 2n−4 coordinates are in §3.
- No published table of normal forms or summit sets was found; reference values must be generated (Sage with libbraiding, Magma, or CHEVIE).
- Whether some sources define Dehornoy's ordering with the generator of *largest* index (the "flipped" version) was not checked; I recall that Dehornoy's later books use both. The choice must be recorded.

---

## 2. Representations and Invariants Computable Exactly

### Takeaway

Burau is faithful for n ≤ 3 and unfaithful for n ≥ 5 (Bigelow gives explicit kernel words); for n = 4 the brief's "open" needs amending: a preprint of July 2026 by Bharathram, Birman and Brendle claims faithfulness, revised in September 2026 and not yet verified by the community. The Lawrence–Krammer–Bigelow representation, of dimension n(n−1)/2, is faithful for all n. Burau gives the Alexander polynomial of the closure, and the Temperley–Lieb representation the Jones polynomial; every source fixes its own convention for the matrices, so an implementation must state one and test against a named reference.

### Cited Findings

- Faithfulness of Burau: n = 2 faithful; n = 3 faithful (Magnus and Peluso 1969); n ≥ 5 not faithful (Bigelow 1999, combining computer search with the Long–Paton criterion; Long and Paton's paper is titled "The Burau representation is not faithful for n ≥ 6", 1993; the first unfaithful cases are Moody's). — [Wikipedia, *Burau representation*](https://en.wikipedia.org/wiki/Burau_representation)
- n = 4: described as "a longstanding open problem"; Vasudha Bharathram, Joan S. Birman and Tara E. Brendle posted arXiv:2607.05283, "The Burau representation of the braid group is faithful for n = 4", which per Wikipedia "has not yet been verified by the mathematical community". — [Wikipedia, *Burau representation*](https://en.wikipedia.org/wiki/Burau_representation)
- The arXiv record: v1 6 July 2026, v2 14 September 2026 ("28 pages, 32 figures. Minor corrections and improved exposition"), not withdrawn; the abstract uses ideas of Moody, Long, Long–Paton and Bigelow and adds that the Jones representation is therefore also faithful for n = 4. — [arXiv:2607.05283](https://arxiv.org/abs/2607.05283)
- Earlier partial evidence for n = 4: Bigelow's computer search found that any pair of arcs on the 4-punctured disc meeting his criterion must intersect at least 500 times; numerical evidence of Fullarton and Shadrach suggests faithfulness. — [arXiv:math/9904100](https://arxiv.org/abs/math/9904100); [arXiv:1610.04648](https://arxiv.org/abs/1610.04648)
- Bigelow's kernel elements (Geom. Topol. 3 (1999) 397–404). For n = 5: ψ₁ = σ₃⁻¹σ₂σ₁²σ₂σ₄³σ₃σ₂, ψ₂ = σ₄⁻¹σ₃σ₂σ₁⁻²σ₂σ₁²σ₂²σ₁σ₄⁵, and the kernel element [ψ₁⁻¹σ₄ψ₁, ψ₂⁻¹σ₄σ₃σ₂σ₁²σ₂σ₃σ₄ψ₂], of length 120. For n = 6: ψ₁ = σ₄σ₅⁻¹σ₂⁻¹σ₁, ψ₂ = σ₄⁻¹σ₅²σ₂σ₁⁻², kernel element [ψ₁⁻¹σ₃ψ₁, ψ₂⁻¹σ₃ψ₂], of length 44. Conventions there: σᵢ is a clockwise half-twist exchanging punctures i and i+1; braids act on the left; [a, b] = a⁻¹b⁻¹ab; the reduced Burau representation is the action on H₁ of the infinite cyclic cover of the punctured disc, a free ℤ[t, t⁻¹]-module of rank n−1. — [arXiv:math/9904100](https://arxiv.org/abs/math/9904100)
- Burau matrices: unreduced, σᵢ ↦ I_{i−1} ⊕ [[1−t, t],[1, 0]] ⊕ I_{n−i−1}; reduced, for n = 2, σ₁ ↦ (−t), and explicit (n−1)×(n−1) blocks for n ≥ 3 (the page's matrices were garbled in retrieval; the unreduced block is the standard form). — [Wikipedia, *Burau representation*](https://en.wikipedia.org/wiki/Burau_representation)
- Alexander polynomial from Burau: for a knot K that is the closure of f ∈ B_n, up to a unit of ℤ[t, t⁻¹], Δ_K(t) = (1 − t)/(1 − tⁿ) · det(I − f*), with f* the reduced Burau matrix. — [Wikipedia, *Burau representation*](https://en.wikipedia.org/wiki/Burau_representation)
- Lawrence–Krammer–Bigelow: the module is free of rank n(n−1)/2 and the representation B_n → Aut_R(H) is faithful for all n ≥ 2; Krammer first proved the case n = 4, and Bigelow all n (J. Amer. Math. Soc. 14 (2001) 471–486), with the matrices computed explicitly in his §4. Krammer's variable t corresponds to −t in Bigelow's paper. — [Bigelow, *Braid groups are linear*, arXiv:math/0005038](https://arxiv.org/abs/math/0005038); [*Introduction to representations of braid groups*, arXiv:1404.0724](https://arxiv.org/pdf/1404.0724)
- Artin's representation on the free group as González-Meneses writes it: ρ_{σᵢ}(xᵢ) = xᵢ₊₁, ρ_{σᵢ}(xᵢ₊₁) = xᵢ₊₁⁻¹ xᵢ xᵢ₊₁, other generators fixed; Theorem 1.3 characterises the image: each f(xᵢ) is a conjugate of some xⱼ and f(x₁⋯xₙ) = x₁⋯xₙ. — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- SageMath's corresponding conventions: `BraidGroup(n)` has generators s0,…,s(n−2), with integer i standing for s(i−1); the product is concatenation left to right; the free-group action is written on the right, `f * b`, and its examples give x·σ = x_i x_{i+1} x_i⁻¹ on the moved generator, which is not González-Meneses's formula. `burau_matrix(reduced=…)` has variants `'increasing'` (basis e₁ − eᵢ), `'simple'` (basis eᵢ − eᵢ₊₁, "matching Wikipedia") and `'unitary'` (Squier's form, returning M, M^adj, H with M·H·M^adj = H). Also `LKB_matrix`, `TL_matrix` (eigenvalues of the generators 1 and −A⁴), `alexander_polynomial` (from reduced Burau), `jones_polynomial` (unknot ↦ 1), `markov_trace`, `permutation`, `components_in_closure`. — [SageMath reference, braid groups](https://doc.sagemath.org/html/en/reference/groups/sage/groups/braid.html)
- braidlab computes `burau`, `lk` (Lawrence–Krammer), `alexpoly`, `writhe`, `perm`, `ispure`. — [braidlab guide, arXiv:1410.0849](https://arxiv.org/abs/1410.0849)

### Inferences

- **[inference]** "Burau is open for n = 4" is no longer the right sentence for a note written in November 2026. The accurate one is: faithfulness for n = 4 was the last open case; a proof was announced in July 2026 (arXiv:2607.05283, v2 September 2026) and awaits verification. Code and notes should not rely on it either way.
- **[inference]** Bigelow's two kernel words are the best exact test of the Burau and LKB implementations together: Burau must return the identity matrix, LKB must not, and each word-problem solver of §1 must report nontrivial. The words are stated in the clockwise convention with a left action; under the mirror convention (σᵢ ↔ σᵢ⁻¹) the mirrored words are again in the kernel, since the mirror map intertwines Burau at t and t⁻¹ up to conjugation, but word reversal and the commutator convention must be carried through consistently.
- **[from memory]** Exponent sum (writhe) e: B_n → ℤ is the abelianisation; the permutation is B_n → S_n with kernel the pure braid group P_n; for a pure braid the linking number of strands i and j is half the signed count of their crossings, equal to the winding number of z_j − z_i about 0. These pairwise numbers are the abelianisation of P_n (ℤ^{n(n−1)/2}) and so do not determine the braid.
- **[from memory]** Moody's 1991 paper proved unfaithfulness for n ≥ 9 (Bull. Amer. Math. Soc. 25); Krammer's "Braid groups are linear" is Ann. of Math. 155 (2002) 131–156. Wikipedia as fetched cites Moody (1993) without a value of n.

### Gaps

- The entrywise formula of the LKB matrices was not retrieved (Bigelow §4, or Krammer's paper); the t ↔ −t discrepancy between the two must be resolved against Sage's `LKB_matrix`.
- The exact Temperley–Lieb/Kauffman-bracket normalisation that turns the TL representation's trace into the Jones polynomial was not retrieved; Sage's `TL_matrix`, `markov_trace` and `jones_polynomial` are the reference.
- The proof in arXiv:2607.05283 was not read; no independent verification was found.

---

## 3. Braids as Mapping Classes

### Takeaway

B_n modulo its centre is the mapping class group of the n-punctured disc; a braid is periodic, reducible or pseudo-Anosov, and its topological entropy is the growth rate of loops under iteration. For computation, Dynnikov coordinates with their piecewise-linear update rules are the practical tool (exact integer arithmetic, linear cost per generator), the Bestvina–Handel train-track algorithm is exact but impractical for large braids, and the Burau spectral radius on the unit circle gives a lower bound that is sharp only in characterised cases.

### Cited Findings

- Nielsen–Thurston for braids (Theorem 6.1): every braid is exactly one of periodic, pseudo-Anosov, or non-periodic reducible, the type being read in B_n/⟨Δ²⟩; periodic means α^m = Δ^p for some m ≠ 0, equivalently α is conjugate to a power of δ = σ₁⋯σ_{n−1} or of ε = σ₁(σ₁⋯σ_{n−1}). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- The mapping class group of D_n is canonically B_n modulo its centre; B_n thereby acts on the space of Dynnikov coordinates, with separate rules for inverse generators; Hall and Yurttaş compute the entropy of each braid in an infinite family (two families of Hironaka and Kin) with Dynnikov coordinates on the boundary of Teichmüller space. — [Hall and Yurttaş, *On the topological entropy of families of braids*, arXiv:0808.3053](https://arxiv.org/abs/0808.3053)
- Dynnikov coordinates of an integer lamination on the n-punctured disc (Thiffeault's normalisation): from intersection numbers μ, ν with vertical arcs, aᵢ = (μ_{2i} − μ_{2i−1})/2 and bᵢ = (νᵢ − νᵢ₊₁)/2 for i = 1,…,n−2, a vector u = (a₁,…,a_{n−2}, b₁,…,b_{n−2}) ∈ ℤ^{2n−4}. — [Thiffeault, *Braids of entangled particle trajectories*, arXiv:0906.3647](https://arxiv.org/abs/0906.3647); same in [arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- Update rules as Thiffeault (2010) states them, for σᵢ the *clockwise* interchange (he notes that his cited source uses counterclockwise). Write f⁺ = max(f, 0), f⁻ = min(f, 0); unlisted components are unchanged.
  - For σᵢ, 2 ≤ i ≤ n−2, with c = a_{i−1} − aᵢ − bᵢ⁺ + b_{i−1}⁻:
    a′_{i−1} = a_{i−1} − b_{i−1}⁺ − (bᵢ⁺ + c)⁺; b′_{i−1} = bᵢ + c⁻; a′ᵢ = aᵢ − bᵢ⁻ − (b_{i−1}⁻ − c)⁻; b′ᵢ = b_{i−1} − c⁻.
  - For σ₁: a′₁ = −b₁ + (a₁ + b₁⁺)⁺; b′₁ = a₁ + b₁⁺.
  - For σ_{n−1}: a′_{n−2} = −b_{n−2} + (a_{n−2} + b_{n−2}⁻)⁻; b′_{n−2} = a_{n−2} + b_{n−2}⁻.
  - For σᵢ⁻¹, 2 ≤ i ≤ n−2, with d = a_{i−1} − aᵢ + bᵢ⁺ − b_{i−1}⁻:
    a′_{i−1} = a_{i−1} + b_{i−1}⁺ + (bᵢ⁺ − d)⁺; b′_{i−1} = bᵢ − d⁺; a′ᵢ = aᵢ + bᵢ⁻ + (b_{i−1}⁻ + d)⁻; b′ᵢ = b_{i−1} + d⁺.
  - For σ₁⁻¹: a′₁ = b₁ − (b₁⁺ − a₁)⁺; b′₁ = b₁⁺ − a₁.
  - For σ_{n−1}⁻¹: a′_{n−2} = b_{n−2} − (b_{n−2}⁻ − a_{n−2})⁻; b′_{n−2} = b_{n−2}⁻ − a_{n−2}.
  — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647) (the σᵢ rules for interior i also in [arXiv:1410.0849](https://arxiv.org/abs/1410.0849))
- The rules are piecewise linear: once the maxima and minima are resolved, what remains is a linear map. — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- Entropy of a braid: the exponential growth rate of the length L(u) of a loop, L being the minimum number of intersections with the horizontal line through the punctures (his eq. 5, with boundary terms b₀, b_{n−1} of eq. 6). Procedure for a periodic braid: apply the rules for m periods and take h = m⁻¹ log L(u), converging as 1/m. The flow's entropy bounds it: h_flow ≥ h_braid^{(n)}; it vanishes for n = 1, 2. Overflow is handled by periodic renormalisation, keeping the logarithm of the scale. — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- braidlab's `entropy` iterates the braid on a loop and monitors convergence (after Moussafir 2006), warning and returning zero when it fails (a finite-order or low-entropy braid); `'Method','train'` runs Bestvina–Handel through Toby Hall's Train and is called impractical for large braids; `complexity` is Dynnikov and Wiest's geometric complexity (2007), the log₂ of intersection counts with the real axis; the guide states no formal complexity. — [arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- Burau bound: for a homeomorphism of the punctured disc represented by a braid with Burau matrix B(t), the topological entropy is at least log of the spectral radius of B(η) over complex |η| = 1 (Fried; obtained independently by Kolev). Band and Boyland: for a pseudo-Anosov braid the estimate can be sharp at a root of unity η₀ only if η₀ = −1, and it is sharp at −1 iff the invariant foliations have odd-order singularities at all punctures and all interior singularities of even order; an analogous theorem holds for reducible braids. Algebr. Geom. Topol. 7 (2007), issue 3. — [Band and Boyland, *The Burau estimate for the entropy of a braid*, arXiv:math/0612716](https://arxiv.org/abs/math/0612716); restated in [arXiv:1801.04790](https://arxiv.org/pdf/1801.04790)
- Koberda gives a similar estimate from the Lawrence–Krammer–Bigelow representation. — [arXiv:1801.04790](https://arxiv.org/pdf/1801.04790)
- Thiffeault's monograph *Braids and Dynamics* (Springer 2022, Frontiers in Applied Dynamical Systems: Reviews and Tutorials 9, ISBN 978-3-031-04789-3, DOI 10.1007/978-3-031-04790-9) covers braids, the Thurston–Nielsen classification, entropy, train tracks, Dynnikov coordinates, braidlab, and braids from trajectory data. — [Springer](https://www.springer.com/book/9783031047893); [MAA review](https://maa.org/book-reviews/braids-and-dynamics/)
- Software that classifies: Sage's `thurston_type`, `is_reducible`, `is_periodic`, `is_pseudoanosov` (through libbraiding); flipper (Python, MIT) computes the Nielsen–Thurston type and dilatation of mapping classes of punctured surfaces from ideal triangulations. — [SageMath reference](https://doc.sagemath.org/html/en/reference/groups/sage/groups/braid.html); [flipper](https://github.com/MarkCBell/flipper)
- The Dynnikov approach is tied to the Artin presentation on the disc and does not directly apply to braids on other surfaces. — [arXiv:2112.07619](https://arxiv.org/pdf/2112.07619)

### Inferences

- **[inference]** The update rules use only +, −, max, min on integers, so they are exact over morphe's unbounded integers; the entropy estimate is then a limit of logarithms of exact integers, to be reported with its iteration count and its Burau lower bound as an interval, in the manner of the infrastructure's verdicts.
- **[inference]** The Burau bound needs the spectral radius of a complex matrix B(η) maximised over the unit circle; this is floating-point, not exact, and belongs to calx; at η = −1 the matrix is an integer matrix and the bound is an algebraic number computable exactly.
- **[from memory]** The standard first test: σ₁σ₂⁻¹ in B₃ is pseudo-Anosov with dilatation (3 + √5)/2, entropy ≈ 0.9624, and there the Burau estimate at η = −1 is sharp. Bestvina and Handel, "Train-tracks for surface homeomorphisms", Topology 34 (1995) 109–140. Dynnikov, "On a Yang–Baxter map and the Dehornoy ordering", Russian Math. Surveys 57 (2002) 592–594. Moussafir, "On computing the entropy of braids", Funct. Anal. Other Math. 1 (2006) 37–46. Hall and Yurttaş appeared in Topology Appl. 156 (2009) 1554–1564.

### Gaps

- The rules were copied through a summarising fetch; before implementing, check them character by character against arXiv:0906.3647 (and its appendix code) and against braidlab's source, then against the invariants: a braid relation applied both ways gives equal coordinates, σᵢσᵢ⁻¹ is the identity, and Δ² acts trivially on laminations.
- The relation between Dehornoy's 2n coordinates from (0,1,…,0,1) and Thiffeault's 2n−4 coordinates (extra punctures or boundary conventions) was not worked out.
- Complexity of deciding the Nielsen–Thurston type: I recall work of Calvez (2014) and of Margalit, Strenner and Yurttaş on fast classification, but found and read nothing in this session.
- Fried's and Kolev's original papers were not read; their statements are taken from Band and Boyland and from arXiv:1801.04790.

---

## 4. Braids from Trajectories

### Takeaway

The standard construction (Thiffeault 2005, 2010; braidlab) projects the particles on a line, records each exchange of order of two neighbours as a generator σᵢ^{±1} whose sign is read from the perpendicular coordinate at the crossing, and orders crossings by time, found by linear interpolation between samples. For closed (periodic) motions a change of projection direction conjugates the braid; for open trajectories it changes the braid by a bounded number of generators at the two ends, and the object is strictly an element of a groupoid of configurations, not of B_n. The clean treatment of endpoints, with canonical paths in a Fox–Neuwirth top cell, is in Guillemot and Lairez (2026), outside the fluid-dynamics literature.

### Cited Findings

**The construction**

- Positions are projected on a fixed line and particles labelled 1,…,n by increasing projection; a crossing is an exchange of order of two particles; if at the crossing time particle i is above particle i+1 (greater perpendicular coordinate) the generator is σᵢ, else σᵢ⁻¹; the output is the time-ordered sequence of generators with their crossing times. σᵢ is the clockwise interchange of strands i and i+1. — [Thiffeault 2010, arXiv:0906.3647](https://arxiv.org/abs/0906.3647) (Chaos 20, 017516)
- In the paper's `gencross` code the permutation is found by sorting positions at each sample; a change of permutation between consecutive samples marks a crossing interval; time and position of the crossing come from linear interpolation of each particle between the two samples (the root of the difference of projected positions), and the sign of the interpolated perpendicular difference sets the generator's sign. Pair crossings and the two triple crossings (ABC→CAB, ABC→BCA) are resolved; anything more complex, or a zero perpendicular difference, raises an error asking for finer data. — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- braidlab: `braid(XY)` projects on the X axis by default; `braid(XY, angle)` on a line at that angle. Two paths with the same projection raise "Paths of particles … have a coincident projection", judged with `BraidAbsTol` (default 1e-10), the remedy being another angle. Simultaneous crossings may give different generator sequences between runs of the parallel code, equal as braids. `databraid` keeps the crossing times (`tcross`) and is meant for data, for which it computes finite-time braiding exponents instead of entropy. — [braidlab guide, arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- braidlab's conventions: σᵢ exchanges the strings at positions i and i+1 with the left string passing over the right (§1.1), read on trajectories as the left particle passing above the right (§1.2); words are read left to right in time (bottom to top in the diagram); `a*b` is the word of a followed by that of b. — [arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- Budišić and Thiffeault: strands are indexed by their projection on an axis at angle α; σᵢ^{±1} is a crossing of strands i and i+1 "either clockwise (+1) or anticlockwise (−1)". — [*Finite-Time Braiding Exponents*, arXiv:1502.02162](https://arxiv.org/abs/1502.02162)

**Dependence on the projection direction**

- For periodic braids a change of projection line changes the braid only by conjugation, which leaves the entropy unchanged. For non-periodic trajectories "the choice of projection line changes the braid beyond simple conjugation", an error of a small, finite number of generators that does not affect the entropy asymptotically. Symmetric configurations (orbits on one line) cannot be resolved by some projection lines; displacing the line slightly cures it. — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- For non-periodic trajectories conjugation under a change of α is "not guaranteed"; the relative standard deviation of the FTBE over α decays inversely with the physical length of the braid, while that of the word length stays roughly constant; the authors fix α = 0. — [arXiv:1502.02162](https://arxiv.org/abs/1502.02162)
- A 2026 study of polar active matter reports that braid measures change with the projection angle through modified crossings at the start and end of the braid. — [arXiv:2609.04062](https://arxiv.org/pdf/2609.04062)

**Endpoints**

- For non-periodic data the method "does not define a braid in the traditional sense" where strands return to the initial configuration; only the order along the projection line matters. — [arXiv:1410.0849](https://arxiv.org/abs/1410.0849); [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- braidlab's `braid` warns when the data do not close up (`BRAIDLAB:braid:colorbraiding:notclosed`); `closure(XY)` appends segments from final to initial positions without creating new crossings in the X projection (the Y-projection braid may gain generators); `closure(XY,'MinDist')` instead matches endpoints to minimise total distance. — [arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- Repeated application of the braid of non-periodic trajectories is unjustified, since the physical braid does not represent a homeomorphism of a fixed punctured surface; hence the non-iterated FTBE. — [arXiv:1502.02162](https://arxiv.org/abs/1502.02162)
- Guillemot and Lairez give the canonical construction for an arbitrary path γ in the unordered configuration space: for a configuration c, order its points lexicographically (real parts, ties by imaginary parts) and let γ_c be the straight-line path from the base configuration {1,…,n} to c, which never collides because the lexicographic order is strict throughout. The braid of γ is the class of γ_{γ(0)} · γ · γ_{γ(1)}⁻¹ in π₁. The convention is compatible with concatenation: the braid of α·β is the product of the braids of α and β. — [Guillemot and Lairez, *Computing braids from approximate data*, arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §2.1
- Their exact algorithm (Algorithm 1) on a piecewise-linear path: compute the sorting permutation ord(t), its discontinuity points (or a finite superset: the times where two real parts coincide), and at each one write the change of permutation as a product of adjacent transpositions, each contributing σ^{±1} with the sign of the difference of imaginary parts at that time. With at most two points sharing a real part at any time the proof is immediate; the non-generic case is more technical and omitted there. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §2.2
- Their generator convention: σᵢ is the path exchanging i and i+1 "with the right point passing above the left point"; path concatenation α·β traverses α first (Hatcher's convention). — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §2.1
- Rodriguez and Wang (2017) compute braids of root motions by solving a polynomial system containing the times at which roots cross under projection on the real axis and determining the signs by homotopy continuation; per Guillemot and Lairez, further work is needed to certify it and to handle non-generic crossings. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073); [Rodriguez and Wang, arXiv:1711.07947](https://arxiv.org/abs/1711.07947)

**Measures and uses**

- Thiffeault 2005 ("Measuring topological chaos", Phys. Rev. Lett.) defines a braiding exponent proportional to the usual Lyapunov exponent of the flow. — [arXiv:nlin/0409041](https://arxiv.org/abs/nlin/0409041)
- Entropy from data (Thiffeault 2010): without averaging, apply the generators, plot log L(u) against the crossing times and fit a line; with averaging, cut time into subintervals, average log L over realisations, and fit; averaging log L, not L, gives the same entropy with smaller fluctuations. Convergence of h_braid^{(n)} to h_flow as n grows "appears fairly slow". Demonstrated on float data from the Labrador Sea. Two dimensions only. — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- FTBE: for a braid b of n trajectories over an interval of length T, FTBE(b) = (1/T) log(|b·ℓ_E| / |ℓ_E|), where ℓ_E is a loop generating the fundamental group of the n-punctured disc and |·| counts intersections with the horizontal axis; a non-iterated form adapted from Dynnikov and Wiest. — [arXiv:1502.02162](https://arxiv.org/abs/1502.02162)
- The FTBE approaches the topological entropy as the number of trajectories grows. — [arXiv:1910.12779](https://arxiv.org/pdf/1910.12779v1); a later paper links it to the Lyapunov exponent in the limit of long times and many particles — [arXiv:2412.05407](https://arxiv.org/pdf/2412.05407)
- Allshouse and Thiffeault, "Detecting coherent structures using braids" (Physica D, DOI 10.1016/j.physd.2011.10.002): trajectories as a braid in three dimensions with time the third coordinate; coherent regions are found by searching for loops around sets of trajectories whose growth under the braid is negligible; meant for a few trajectories (floats), where FTLE methods need dense velocity data. — [arXiv:1106.2231](https://arxiv.org/abs/1106.2231)
- Boyland, Aref and Stremler, "Topological fluid mechanics of stirring", J. Fluid Mech. 403 (2000) 277–304: stirring by finitely many stirrers in a disc of fluid, with the Thurston–Nielsen theory used to decide which protocols give pseudo-Anosov maps. — [author's copy](https://people.clas.ufl.edu/boyland/files/stir.pdf)
- Braid types and forcing: Boyland introduced the braid type of a periodic orbit of a surface homeomorphism; a braid type γ forces β if every disc homeomorphism with a periodic orbit of type γ has one of type β; this is a partial order (Boyland 1984, 1994), generalising Sharkovskii's; a finite-order braid type forces only itself. The survey's theme is isotopy-stable dynamics. — [Boyland, *Topological methods in surface dynamics*, Topology Appl. (1994)](https://people.clas.ufl.edu/boyland/files/topmethods.pdf); [de Carvalho and Hall, arXiv:1406.2896](https://arxiv.org/pdf/1406.2896)
- Moore, "Braids in classical dynamics", Phys. Rev. Lett. 70 (24) (1993) 3675–3679, DOI 10.1103/PhysRevLett.70.3675: classifies periodic orbits by braid type and found the figure-eight orbit of three equal masses numerically; Chenciner and Montgomery later proved its existence. (A Stony Brook page lists the title as "Braids in classical gravity"; the PRL title is "Braids in classical dynamics".) — [Wikipedia, *n-body choreography*](https://en.wikipedia.org/wiki/N-body_choreography)
- The figure-eight's braid word is (b₁b₂⁻¹)³, bᵢ the crossing of strand i over strand i+1. — [arXiv:2601.09843](https://arxiv.org/html/2601.09843v2)
- Montgomery, "The N-body problem, the braid group, and action-minimizing periodic solutions", Nonlinearity 11(2) (1998) 363–376: a collision-free reduced periodic orbit (periodic modulo rigid motion) of the planar N-body problem corresponds to a conjugacy class in the projective coloured (pure) braid group. — [index record](https://acnpsearch.tweb-dev.unibo.it/singlejournalindex/52554415); cited in [arXiv:1507.02883](https://arxiv.org/pdf/1507.02883)
- Other routes from trajectories: ensemble-based topological entropy (E-tec) for sparse trajectory data; Burau eigenanalysis of trajectory braids for material coherence (Yeung, Cohen-Steiner, Desbrun). — [arXiv:1809.01005](https://www.arxiv.org/pdf/1809.01005); [Yeung et al.](https://geometry.caltech.edu/pubs/YCSD19.pdf)

### Inferences

- **[inference] The two sign conventions in circulation are mirror images.** Thiffeault (2010), braidlab, Budišić–Thiffeault and Bigelow take σᵢ clockwise (the left point passes above, i.e. on the side of greater perpendicular coordinate). González-Meneses (§3.2: the i-th point passes *below* the (i+1)-st) and Guillemot–Lairez (the right point passes above the left) take σᵢ counterclockwise. A library must state one, and convert test vectors from the other by σᵢ ↔ σᵢ⁻¹.
- **[inference] The dependence on projection is exactly a change of canonical paths.** With Guillemot and Lairez's convention, the braid of an open path is the class of γ_{c₀}·γ·γ_{c₁}⁻¹. Another projection direction θ gives other canonical paths γ^θ_c, and the two braids differ by left and right multiplication by the braids of the loops γ_{c₀}·(γ^θ_{c₀})⁻¹ and γ^θ_{c₁}·γ_{c₁}⁻¹, each a positive-or-negative permutation braid determined by the endpoint configuration alone (of length at most n(n−1)/2 generators per end, since it sorts a configuration from one order to another). For a closed path the two end corrections are inverse and the result is conjugation. This reproduces, and makes exact, Thiffeault's "small, finite number of generators".
- **[inference]** An open trajectory therefore has a well-defined invariant: a morphism in the fundamental groupoid of the unordered configuration space, between its endpoint configurations. The word in B_n is a coordinate of that morphism in the trivialisation given by a projection direction (§6).
- **[inference]** Braidlab's `closure` options are choices, not canonical: closing without new crossings in the X projection is the same trivialisation; `'MinDist'` re-matches endpoints and changes the permutation.

### Gaps

- Thiffeault and Finn, "Topology, braids and mixing in fluids" (Phil. Trans. R. Soc. A 364 (2006) 3251–3266, **[from memory]**) was not retrieved.
- The journal data of Budišić–Thiffeault (Chaos 25, 087407 (2015)) and Allshouse–Thiffeault (Physica D 241 (2012) 95–105) are from memory; the arXiv numbers and the Physica D DOI are confirmed.
- Braid types of point-vortex motions specifically (as distinct from stirring rods and n-body choreographies) were not searched; Boyland–Stremler–Aref, "Topological fluid mechanics of point vortex motions" (Physica D 175 (2003)) is recalled **[from memory]**.
- Moore's and Montgomery's papers were read only through index records and citing papers.
- How braidlab's compiled `colorbraiding` code interpolates crossing times was not read in the source.

---

## 5. Soundness under Finite Sampling and Noise

### Takeaway

In the dynamics literature there is no theorem giving conditions (speed bound, sampling interval, separation) under which the braid of sampled trajectories is guaranteed correct: the papers state qualitative requirements and study the error empirically. The one rigorous treatment is from computer algebra: Guillemot and Lairez (January 2026) compute the exact braid from any certified enclosure of the paths presented as a separation predicate, with a correctness theorem. A speed bound turns samples into such enclosures, so a sound method for sampled trajectories can be built on their theorem; that step is this note's inference, not a published result.

### Cited Findings

**What the trajectory literature states**

- Thiffeault (2010) gives no quantitative sampling requirement. Crossings come from linear interpolation; too many simultaneous crossings between samples make the code fail, the remedy being finer data; "missing a few crossings is acceptable if the trajectories are long enough". — [arXiv:0906.3647](https://arxiv.org/abs/0906.3647)
- Budišić and Thiffeault name the two issues, the time step and the projection angle. Their stated conditions: strands must cross only between sampled times, and any two strands may cross at most once per time step; a crossing exactly at a sampling instant leaves the generator ambiguous (another α may resolve it); two crossings of one pair within a step are resolved by reducing τ (their Figure 2). Empirically, against a reference τ* = 10⁻⁴, FTBEs are "more resilient to large time steps than word length"; they then use τ = 10⁻². No error bound is given. — [arXiv:1502.02162](https://arxiv.org/abs/1502.02162)
- braidlab's `databraid` suppresses the not-closed warning because it is meant for noisy data; coincidence of projections is decided by an absolute tolerance (1e-10). — [arXiv:1410.0849](https://arxiv.org/abs/1410.0849)
- E-tec and related float studies report robustness tests in models and laboratory experiments for sparse, noisy trajectory data, not guarantees. — [arXiv:1809.01005](https://www.arxiv.org/pdf/1809.01005)

**The certified treatment (Guillemot and Lairez 2026)**

- Problem as they pose it: exact algorithms rely on the lexicographic order of points, "which is unstable under numerical uncertainty"; determining the Fox–Neuwirth cell of a configuration from a numerical approximation, even a certified one, may be impossible because equality cannot be decided. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), abstract and §3.2
- Definition 1 (path approximation): families of open subsets R_{ij}, I_{ij} of [0,1], for distinct i, j, with Re γᵢ(t) < Re γⱼ(t) on R_{ij}, Im γᵢ(t) < Im γⱼ(t) on I_{ij}, and R_{ij} ∪ R_{ji} ∪ I_{ij} ∪ I_{ji} = [0,1]. Definition 2 (computational form): a map sep(i, j, t) returning an ordered pair, an axis in {Re, Im} and a time t′ such that the two strands are strictly ordered along that axis on [t, t′], with t′ ≥ min(1, t + η) for some η > 0 depending only on γ. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §3.1
- An *arrangement* is a pair of strict partial orders (≺_Re, ≺_Im) on {1,…,n} in which every two elements are comparable in at least one; its cell is the set of ordered configurations satisfying the corresponding strict inequalities. Lemma 4: arrangement cells are nonempty, convex and open, cover the ordered configuration space, and are closed under nonempty intersection. Lemma 6: two paths with the same endpoints covered by one sequence of arrangement cells are homotopic (straight-line homotopy, by convexity). — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §3.2
- Algorithm 2 builds a covering sequence from sep, keeping a graph of Re/Im-labelled edges with lifetimes (Proposition 7: it terminates and the output covers γ). Algorithm 3 turns a sequence of arrangements into a braid through *permutation points* p(σ, τ) = (σ(k) + τ(k)·i)_k, one in each cell, and elementary braids computed by Lemma 10. Theorem 11 (correctness) and, for the merged on-the-fly Algorithm 5, Theorem 13: the output consists of arrangements containing γ(0) and γ(1), permutation points in them, and a braid β such that the path γ(0) → p₀ → (β) → p₁ → γ(1) is homotopic to γ. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §4–§5.3
- Cost remarks: initialising the arrangement by querying every pair is quadratic in n; if sep returns only Re-labelled edges at time 0, sorting by real part initialises it quasi-linearly. No overall complexity bound is stated in the parts read. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §5.3.4
- Their sep for certified root tracking: from interval enclosures of each strand over [t, t+δ] (boxes), return the axis and order if the two boxes are separated along Re or along Im, else halve δ; termination follows from disjointness at time t and continuity. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §5.4
- Prior certified work they cite: Marco-Buzunariz and Rodríguez (SIROCCO, ICMS 2016), who compute piecewise-linear paths inside certified tubes and read the braid from them; certified path trackers of van der Hoeven (2011), Beltrán and Leykin (2012, 2013), Kranich (2016), Xu, Burr and Yap (2018), Duff and Lee (2024), Guillemot and Lairez (2024, algpath). Their implementation is in Rust, at gitlab.inria.fr/aguillem/braids, a command-line tool for braid monodromy with algpath as backend; the paper is CC BY-SA 4.0. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073)

### Inferences

- **[inference] Negative finding, stated precisely.** I found no paper in the trajectory-braid literature (Thiffeault and co-authors, E-tec, coherent structures, active matter) that proves a sufficient condition on sampling for the computed braid to equal the true one, and none that propagates measurement uncertainty to a set or distribution of braids. What exists: the two qualitative conditions of Budišić–Thiffeault, empirical resilience curves, and the certified algorithm of Guillemot–Lairez for a different source of data (polynomial roots).
- **[inference] A sufficient condition under a speed bound.** Suppose every particle moves with speed at most v between samples t_k and t_{k+1} = t_k + Δt, and let r(t) = z_j(t) − z_i(t). Then |r(t) − r(t_k)| ≤ 2vΔt on the step, and the linear interpolant's relative vector lies on the segment from r(t_k) to r(t_{k+1}), inside the same closed disc D of radius 2vΔt about r(t_k). If 2vΔt < |r(t_k)| for every pair, D is convex and misses the origin, so the straight-line homotopy H(s, t) = (1−s)·γ(t) + s·γ_lin(t) keeps every relative vector in D, hence stays in the configuration space, and the true path is homotopic rel endpoints to the linear interpolant. This is the argument of Guillemot–Lairez's Lemma 6 with discs in place of arrangement cells. Under it, the braid read from the linear interpolant by the exact algorithm is the true braid of that step. Sharper forms follow from better enclosures (the ellipse with foci at the two samples; acceleration bounds).
- **[inference] The pairwise condition is not merely about winding numbers.** Pairwise winding numbers do not determine a braid (P_n is not abelian), so the argument must be, as above, a homotopy in the configuration space; the convexity of the common enclosure is what gives it.
- **[inference] Three-valued output.** Where the condition fails for some pair on some step, the honest answer is "undecided, at this step, for this pair", or the finite set of braids compatible with the enclosures; with measurement noise of radius ε the same holds with discs enlarged by ε. This fits the infrastructure's practice of evaluations that refuse with a reason.
- **[inference]** A separation predicate is directly implementable on enclosures by boxes or discs, so Algorithm 5 of Guillemot–Lairez applies to sampled trajectories once a speed bound supplies the enclosures; it also removes the dependence on a tolerance for "coincident projections", using the imaginary axis where the real one does not separate.

### Gaps

- No search was made of the robotics and multi-agent literature beyond one result (Mavrogiannis et al. use braids to abstract multi-agent trajectories; not read), nor of topological data analysis; a guarantee may exist there.
- The Rust implementation's licence was not checked (the paper's text is CC BY-SA 4.0; the code's terms are on its GitLab page).
- No probabilistic treatment (a distribution over braids from noisy tracks) was found.

---

## 6. Configuration Spaces

### Takeaway

Fox and Neuwirth (1962) stratify the unordered configuration space of n points of ℂ by which points share a real part and in what order; there is exactly one top-dimensional cell, the configurations with pairwise distinct real parts, and the codimension-1 and codimension-2 cells give Artin's generators and relations. That top cell is convex in sorted coordinates, hence contractible, so any path between generic configurations has a canonical braid word once a projection is fixed: this is the groupoid statement behind every trajectory method. The Fadell–Neuwirth fibration gives the strand-forgetting maps and the split sequence 1 → F_n → P_{n+1} → P_n → 1.

### Cited Findings

- Fox–Neuwirth cells as González-Meneses presents them (§3.2): work in ℂⁿ/Σ_n, of real dimension 2n; order complex numbers lexicographically (real part, then imaginary part); a symbol θ joins consecutive points by <, by "same real part, one below the other", or by =; each θ defines a cell C_θ of dimension 2n − (number of vertical coincidences) − 2·(number of equalities); the cells form a regular cell decomposition, and those with an equality form the subcomplex of the big diagonal, whose removal leaves the configuration space. — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- There is exactly one cell of dimension 2n, θ = (z₁ < ⋯ < z_n); the n−1 cells of dimension 2n−1 (one vertical coincidence between zᵢ and zᵢ₊₁) give the generators σᵢ; the cells of dimension 2n−2 give the relations: two separate coincidences give σᵢσⱼ = σⱼσᵢ, three points on a vertical line give σᵢσᵢ₊₁σᵢ = σᵢ₊₁σᵢσᵢ₊₁. The presentation is read from the dual complex: one vertex per top cell, one edge per codimension-1 cell, a maximal tree (here a point), a generator per remaining edge, a relation per codimension-2 cell. σᵢ is the motion in which the i-th point passes below the (i+1)-st. — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- The Fox–Neuwirth/Fuks stratification decomposes Conf_n(ℂ) into Euclidean cells and gives a CW structure on its one-point compactification; strata are indexed by a poset and satisfy the frontier condition. — [Ellenberg, Tran, Westerland, arXiv:1701.04541](https://arxiv.org/pdf/1701.04541); see also [*Configuration Spaces of Points: A User's Guide*, arXiv:2407.11092](https://arxiv.org/pdf/2407.11092)
- For the *ordered* configuration space of three points, the cells up to permutation are {Re z₁ < Re z₂ < Re z₃}, {Re z₁ < Re z₂ = Re z₃, Im z₂ < Im z₃}, {Re z₁ = Re z₂ < Re z₃, Im z₁ < Im z₂}, {Re z₁ = Re z₂ = Re z₃, Im z₁ < Im z₂ < Im z₃}. Fox and Neuwirth, "The braid groups", Math. Scand. 10 (1962) 119–126. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §3.2 and references
- Canonical paths: for any configuration, the straight-line path from the base configuration {1,…,n} to it, matching points in lexicographic order, never collides; defining the braid of a path through these canonical paths is compatible with concatenation. — [arXiv:2601.23073](https://arxiv.org/abs/2601.23073), §2.1
- Fadell–Neuwirth (Theorem 2.1 in González-Meneses): forgetting a point, M_{n+1} → M_n, is a locally trivial fibre bundle with fibre ℂ minus n points; hence the split exact sequence 1 → F_n → PB_{n+1} → PB_n → 1 (eq. 3; a section adds a point at |z₁| + ⋯ + |z_n| + 1), the sequence 1 → PB_n → B_n → Σ_n → 1 (eq. 2), and Theorem 2.2: the ordered and unordered configuration spaces are K(π,1). — [arXiv:1010.0321](https://arxiv.org/abs/1010.0321)
- Conf_k(ℝ²) is a K(Br_k, 1), and a groupoid model equivalent to the groupoid of braids appears in the operadic literature. — [Batanin, *Locally constant n-operads as higher braided operads*, arXiv:0804.4165](https://arxiv.org/pdf/0804.4165)

### Inferences

- **[inference] Contractibility of the generic cell, answering the brief's question.** The top cell of the unordered space is U = {configurations with pairwise distinct real parts}. Sorting by real part identifies U with {x₁ < ⋯ < x_n} × ℝⁿ (real parts in the open Weyl chamber, imaginary parts free), a convex open subset of ℝ^{2n}; so U is contractible. The sources read call the decomposition regular or Euclidean and do not use the word "contractible"; the convexity argument is elementary, and Guillemot–Lairez's non-colliding straight-line canonical paths are its explicit form (extended lexicographically to the lower cells).
- **[inference] Consequence.** The fundamental groupoid Π₁(UConf_n(ℂ)) restricted to objects in U is canonically equivalent to the one-object groupoid B_n: a path between two generic configurations has a well-defined braid, composition of paths is multiplication of braids, and the word is read from the sequence of codimension-1 walls crossed, with sign from the side of crossing. In the ordered space there are n! top cells and the corresponding statement gives the groupoid whose objects are orderings: the *coloured* braid groupoid, with vertex groups P_n.
- **[inference]** The projection direction is the choice of the real axis in the stratification; rotating it gives another contractible cell U_θ, and the transition between trivialisations is the endpoint correction described in §4. The exceptional directions for a configuration are the at most n(n−1)/2 directions perpendicular to a line through two of its points.
- **[inference]** Strand-forgetting maps are defined on pure braids (and on braids together with a choice of invariant subset of strands); on the groupoid of configurations they are functors, which is the right setting for braidlab's `subbraid` on open trajectories.

### Gaps

- Fox and Neuwirth's and Fadell and Neuwirth's original papers (Math. Scand. 10 (1962), 119–126 and 111–118; the latter's pages **[from memory]**) were not read; statements are from González-Meneses and from Guillemot–Lairez.
- Whether every lower Fox–Neuwirth cell is an open ball in the unordered space follows from the same sorted-coordinates argument but was not found stated in a source read.

---

## 7. Changing Numbers of Strands

### Takeaway

All the cited structures exist and the citations in the brief are correct in substance. They model different things: the inverse braid monoid (Easdown and Lavers 2004) models strands removed (partial braids on at most n positions); vines (Lavers 1997) model strands that merge and never split; braided injections (Schlichtkrull and Solberg) model strands inserted among others, with the braid groups as endomorphism monoids; singular braid monoids (Baez; Birman) model instantaneous double points, not persistent mergers; the braid category is only the disjoint union of the B_n, and tangle categories add creation and annihilation in pairs, which trajectories monotone in time do not have.

### Cited Findings

- **Inverse braid monoid.** D. Easdown and T. G. Lavers, "The inverse braid monoid", Adv. Math. 186 (2004) no. 2, 438–455. IB_n consists of partial braids, obtained by deleting one or several strings from braids of B_n, with a product that removes any arc not joining the upper and lower planes. — [Vershinin, *On the inverse braid monoid*, arXiv:0704.3002](https://arxiv.org/abs/0704.3002)
- Easdown–Lavers presentation (Vershinin's (1.7)): generators σᵢ, σᵢ⁻¹ (i = 1,…,n−1) and ε; relations σᵢσᵢ⁻¹ = σᵢ⁻¹σᵢ = 1; the braid relations; εσᵢ = σᵢε for i ≥ 2; εσ₁ε = σ₁εσ₁ε = εσ₁εσ₁; ε = ε² = εσ₁² = σ₁²ε. Replacing σᵢσᵢ⁻¹ = 1 by σᵢ² = 1 and deleting ε = εσ₁² = σ₁²ε gives Popova's presentation of the symmetric inverse monoid I_n; there is a canonical map τ_n: IB_n → I_n extending B_n → Σ_n. — [arXiv:0704.3002](https://arxiv.org/abs/0704.3002)
- Vershinin's presentation (1.10) with one idempotent per strand: generators σᵢ^{±1} and εᵢ (i = 1,…,n; εᵢ the trivial braid with the i-th string deleted); relations: braid relations; εⱼσᵢ = σᵢεⱼ for the indices not adjacent to the crossing; εᵢσᵢ = σᵢεᵢ₊₁; εᵢ₊₁σᵢ = σᵢεᵢ; εᵢ = εᵢ²; εᵢ₊₁σᵢ² = σᵢ²εᵢ₊₁ = εᵢ₊₁; εᵢεᵢ₊₁σᵢ = σᵢεᵢεᵢ₊₁ = εᵢεᵢ₊₁. Word problem: Theorem 2.3, a monomorphism of IB_n into a monoid of partial isomorphisms of a free group solves it; Theorem 2.6 claims Garside normal forms also do. — [arXiv:0704.3002](https://arxiv.org/abs/0704.3002)
- The factorisable braid monoid of Easdown, East and FitzGerald (Proc. Edinburgh Math. Soc.) is a different object from the inverse braid monoid. — [Cambridge Core](https://cambridge.org/core/services/aop-cambridge-core/content/view/CECE039049FF20129E8605F981CF8AEF/S0013091504001452a.pdf/factorizable_braid_monoid.pdf)
- **Vines.** T. G. Lavers, "The theory of vines", Comm. Algebra 25(4) (1997) 1257–1284, DOI 10.1080/00927879708825919 (topics: transformation monoids, presentations, braids, the word problem for vines). — [zbMATH/MaRDI record](https://portal.mardi4nfdi.de/wiki/Item:Q4338220)
- An (m, n)-vine is a set of m piecewise-linear arcs descending monotonically from the points (i, 0, 1) to points among (j, 0, 0), such that once two arcs meet they stay together: strands may merge into one path and never separate again. The category of vines 𝕍 has natural numbers as objects and vines modulo ambient isotopy as morphisms; it is "the free braided strict monoidal category containing a braided monoid" (the PROB for braided monoids), contains the braid category, and is braided, not symmetric. Each monoid V_n is not a group; B_n acts on it by pre- and post-composition. Reference besides Lavers: M. Weber, Theory Appl. Categ. 30 (2015) no. 50, 1713–1792, §6.3. — [nLab, *vine*](https://ncatlab.org/nlab/show/vine)
- Simple vines (no two strings cross) form a monoid isomorphic to the order-preserving transformations of {1,…,n}. — [Lavers, Australas. J. Combin. 10 (1994)](https://ajc-new.maths.uq.edu.au/pdf/10/ocr-ajc-v10-p147.pdf)
- Presentations of partial vine monoids are used in East's work on presentations of tensor categories. — [arXiv:2005.01953](https://arxiv.org/pdf/2005.01953)
- **Braided injections.** C. Schlichtkrull and M. Solberg, "Braided injections and double loop spaces", arXiv:1403.1101 (March 2014, revised December 2015). The category 𝔅 of braided injections generalises the geometric definition of the braid groups; its endomorphism monoids are the braid groups; it is a braided monoidal small category related to the category of finite sets and injections as the braid groups are to the symmetric groups. Its use there: rectifying E₂ spaces to commutative monoids in 𝔅-spaces, and iterated double deloopings. — [arXiv:1403.1101](https://arxiv.org/abs/1403.1101)
- **Singular braids.** The singular braid monoid SB_n, also called the Baez–Birman monoid, is given by generators σᵢ, σᵢ⁻¹, xᵢ (singular crossings) and relations (Vershinin's (3.7)), with references to Baez and to Birman; Vershinin also defines a partial singular braid monoid, neither factorisable nor inverse (Remark 3.2). — [arXiv:0704.3002](https://arxiv.org/abs/0704.3002)
- **Braid category.** The braid category is the disjoint union of the B_n as a strict monoidal groupoid with a braiding; Joyal and Street showed it is the free braided monoidal category on one object (Macquarie preprint 1985/86; "Braided tensor categories", Adv. Math. 102 (1993)). — [nLab, *braid category*](https://ncatlab.org/nlab/show/braid+category); [Baez and Lauda, *A prehistory of n-categorical physics*, arXiv:0908.2469](https://arxiv.org/pdf/0908.2469)

**What each models**

| Structure | Source | Appear | Disappear | Merge | Split | Presentation read |
|---|---|---|---|---|---|---|
| Inverse braid monoid IB_n | Easdown–Lavers 2004 | as the reverse of deletion, within n positions | yes (strings deleted) | no | no | yes (above) |
| Vines V_n, category 𝕍 | Lavers 1997 | no (every arc starts at the top) | no (merged instead) | yes, permanently | no (the opposite category gives splitting) | in Lavers; not read |
| Braided injections 𝔅 | Schlichtkrull–Solberg | yes (m strands injected among n) | no | no | no | not as a monoid presentation |
| Singular braid monoid SB_n | Baez; Birman | no | no | touch at an instant only | no | generators read; relations not copied |
| Braid category | Joyal–Street | no | no | no | no | free braided monoidal on one object |
| Tangle categories | see Gaps | in pairs | in pairs | no | no | not read |

Sources for the table: [arXiv:0704.3002](https://arxiv.org/abs/0704.3002); [nLab, *vine*](https://ncatlab.org/nlab/show/vine); [arXiv:1403.1101](https://arxiv.org/abs/1403.1101); [nLab, *braid category*](https://ncatlab.org/nlab/show/braid+category).

### Inferences

- **[inference]** For particles that enter and leave an observation window, the fitting model is the groupoid/category whose objects are finite configurations of varying cardinality: IB_n when a maximal index set is fixed and strands are only switched off, braided injections when strands are inserted, and their common generalisation, partial braids between sets of different sizes (the category of partial braids, of which IB_n are the endomorphism monoids). For coalescing particles (aggregation, vortex merger) vines are the model; for fission, the opposite category.
- **[inference]** None of the structures found models merging and splitting together with braiding in one presented category; that would be a braided category generated by a monoid and a comonoid object (a braided bialgebra-like PROB), and nothing was read on it.
- **[inference]** A singular crossing is a collision at one instant followed by separation, which for generic trajectories has codimension 2 and does not occur; it is the model for "the sampling cannot tell which way two particles passed", i.e. a formal sum σᵢ − σᵢ⁻¹ in Vassiliev's sense, and so may serve to represent an undecided crossing.
- **[from memory]** Citations: Baez, "Link invariants of finite type and perturbation theory", Lett. Math. Phys. 26 (1992) 43–51; Birman, "New points of view in knot theory", Bull. Amer. Math. Soc. 28 (1993) 253–287. Relations of SB_n: braid relations; xᵢxⱼ = xⱼxᵢ and xᵢσⱼ = σⱼxᵢ for |i−j| > 1; xᵢσᵢ = σᵢxᵢ; σᵢσᵢ₊₁xᵢ = xᵢ₊₁σᵢσᵢ₊₁; σᵢ₊₁σᵢxᵢ₊₁ = xᵢσᵢ₊₁σᵢ. SB_n embeds in a group (Fenn, Keyman, Rourke 1998). Schlichtkrull–Solberg appeared in Trans. Amer. Math. Soc. 368 (2016) 7305–7338, which is the "2016" of the brief.

### Gaps

- Lavers's 1997 paper was not read: its presentation of V_n and its solution of the word problem are known here only from the zbMATH topics and the nLab page.
- Tangle categories (Freyd and Yetter 1989; Turaev 1989; Shum 1994 for framed tangles as the free ribbon category) were searched once without a usable source; the statement that they are free braided categories with duals, presented by braidings, cups and caps, is **[from memory]**.
- The explicit definition of morphisms of 𝔅 (injective braids from m to n points) was read only at the level of the abstract.
- No source was found on a "partial braid category" under that name; East's arXiv:2005.01953 is the place to look.

---

## 8. Graph Braid Groups, Other Surfaces and Higher Dimensions

### Takeaway

For particles on a network the relevant groups are graph braid groups, computed from Abrams's discretised (cubical, nonpositively curved) configuration space by Farley and Sabalka's discrete Morse theory; presentations are computable and Python tools exist. In dimension three and above the brief's "point braids are trivial" should read: for a simply connected manifold of dimension at least 3 the pure braid group is trivial and the braid group is the symmetric group, so only the permutation survives; what replaces braids there are motion groups of extended objects, first the loop braid groups.

### Cited Findings

- Abrams's thesis introduced the discretised configuration space: for a graph Γ with enough vertices (after subdivision), the n-strand configuration space deformation retracts to a compact cubical complex D_nΓ (ordered) or UD_nΓ (unordered); these are cube complexes of nonpositive curvature, locally CAT(0). — [Farley and Sabalka, *Discrete Morse theory and graph braid groups*, arXiv:math/0410539](https://arxiv.org/abs/math/0410539) (Algebr. Geom. Topol. 5 (2005), issue 3); [Ko and Park, *Characteristics of graph braid groups*](https://typeset.io/pdf/characteristics-of-graph-braid-groups-k9ahkumywk.pdf)
- Farley and Sabalka apply Forman's discrete Morse theory to UD_nΓ for any n and any Γ, with a clear description of the critical cells, from which presentations follow. — [arXiv:math/0410539](https://arxiv.org/abs/math/0410539); [Farley and Sabalka, *Presentations of graph braid groups*, arXiv:0907.2730](https://arxiv.org/abs/0907.2730)
- Structure results: some graph braid groups, a planar-graph and a tree braid group among them, are not right-angled Artin groups (Abrams); every graph braid group embeds in a right-angled Artin group, hence is linear, bi-orderable and residually finite (Crisp and Wiest); the homology of tree braid groups is torsion-free (Farley); if Γ is non-planar, H₁(UC_nΓ) has 2-torsion, with the converse for n = 2 (Kim, Ko, Park), and the conjecture that H₁(B_nΓ) is torsion-free iff Γ is planar. — [Ko and Park](https://typeset.io/pdf/characteristics-of-graph-braid-groups-k9ahkumywk.pdf)
- Kurlin computes presentations step by step, starting from simple graphs and adding edges one at a time, with generators given as actual collision-free motions of robots on the graph. — [Kurlin, *Computing braid groups of graphs with applications to robot motion planning*, arXiv:0908.1067](https://arxiv.org/abs/0908.1067)
- Software: `graph-braid-presenter` by Daniel Berlyne and Tomasz Maciążek (Python) computes a presentation of a graph braid group, first trying to split it as a free product, on their `graph-braid-splitter` and `graph-morse` packages. — [repository mirror](https://git.histb.com/danberlyne/graph-braid-presenter)
- Dimension ≥ 3: for a simply connected manifold M of dimension at least 3, π₁ of the ordered configuration space is trivial (by the Fadell–Neuwirth fibrations) and the braid group π₁(C_k(M)) is isomorphic to Σ_k. — [arXiv:1311.5642](https://arxiv.org/pdf/1311.5642); [arXiv:1002.2291](https://arxiv.org/pdf/1002.2291)
- The sphere is different: the pure braid group of S² is nontrivial and the sphere braid group has its own presentation. — [arXiv:1002.2291](https://arxiv.org/pdf/1002.2291)
- Loop braid groups: LB_n is the motion group of n unlinked, unknotted circles in ℝ³ (or the 3-ball), and contains B_n; three-dimensional motion groups were defined by Dahm (1962) and developed by Goldsmith (1981, 1982); the same groups appear as groups of permutation-conjugacy automorphisms of free groups, as mapping classes, and (Damiani) in an extended version. — [Damiani, *A journey through loop braid groups*, arXiv:1605.02323](https://arxiv.org/abs/1605.02323); [Damiani, Martin, Rowell, arXiv:2008.04840](https://arxiv.org/pdf/2008.04840v2); [Wikipedia, *Loop braid group*](https://en.wikipedia.org/wiki/Loop_braid_group)

### Inferences

- **[inference]** The accurate form of the brief's claim: for points in ℝ^d, d ≥ 3, the unordered configuration space has fundamental group S_n; trajectories carry no braid, only their permutation (and, in a non-simply-connected M, one element of π₁(M) per strand: π₁(UConf_n(M)) ≅ π₁(M) ≀ S_n for dim M ≥ 3, **[from memory]**).
- **[inference]** For walkers on a street network, the graph braid group is the invariant of collision-free motions; with particles that may pass one another on an edge (streets with width), the right space is a thickening of the graph, a planar surface with holes, and the groups are surface braid groups of a multiply connected planar domain, i.e. braids of the particles together with the fixed obstacles (braidlab's `annbraid` is the one-obstacle case).
- **[from memory]** Ghrist, "Configuration spaces and braid groups on graphs in robotics" (2001), proved that configuration spaces of graphs are aspherical and introduced the robotics application; Bellingeri, "On presentations of surface braid groups", J. Algebra 274 (2004), gives presentations for closed and bounded surfaces; Goldsmith, "The theory of motion groups", Michigan Math. J. 28 (1981).

### Gaps

- No presentation of the sphere or torus braid groups was retrieved, and Bellingeri's paper was not found by the search made.
- The licence of the graph-braid packages was not checked.
- No source was read on braids of points in a planar domain with obstacles as used in motion planning (homotopy classes of multi-robot paths); it is adjacent to this brief.

---

## 9. Software

### Takeaway

Almost all mature braid software is GPL, which forbids linking or copying into an MIT library but not using it as an external oracle to generate reference values: braidlab (GPL-3.0), CBraid/Braiding/libbraiding (GPL), SageMath and SnapPy/Twister (GPL). flipper is MIT, braidpy MPL-2.0. The best sources of fixtures are SageMath with libbraiding (normal forms, summit sets, sliding circuits, Thurston type, Burau, LKB, TL, Alexander, Jones) and braidlab (Dynnikov coordinates, entropy, braids from trajectory data).

### Cited Findings

| Software | Language | What it computes | Licence | Source |
|---|---|---|---|---|
| braidlab | MATLAB with C++ MEX | classes `braid`, `loop`, `annbraid`, `databraid`; braids from trajectory data with a projection angle; Dynnikov coordinates (`loopcoords`), `entropy`, `complexity`, `train` (Bestvina–Handel through Toby Hall's Train), `burau`, `lk`, `alexpoly`, `writhe`, `perm`, `compact`, `istrivial`, `conjtest`, `subbraid`, `closure`, FTBE | GPL-3.0 ("released under the GNU General Public License v3") | [repository](https://github.com/jeanluct/braidlab); [guide, arXiv:1410.0849](https://arxiv.org/abs/1410.0849) |
| CBraid (Jae Choon Cha, final version 2001-12-07) and Braiding (Juan González-Meneses, v1.0 2004-10-04) | C++ | Garside normal forms; summit sets and conjugacy | GPL (version not stated on the pages read) | [jeanluct/cbraid](https://gh.hhj520.top/jeanluct/cbraid); [miguelmarco/libbraiding](https://github.com/miguelmarco/libbraiding) |
| libbraiding (Miguel Marco-Buzunariz; fork of Thiffeault's copy; sliding-circuit code by María Cumplido) | C++ shared library | the Braiding program's functions as a library, written for SageMath | GPL | [repository](https://github.com/miguelmarco/libbraiding) |
| SageMath `BraidGroup` | Python (libbraiding, GAP behind it) | normal forms; gcd, lcm; SSS, USS, sliding circuits; conjugating braid; centraliser; Thurston type; rigidity; Burau in four forms; LKB; Temperley–Lieb; Alexander, Jones, coloured Jones, Links–Gould; Markov trace; annular Khovanov homology; action on the free group; permutation | GPL (see Gaps for the version) | [reference manual](https://doc.sagemath.org/html/en/reference/groups/sage/groups/braid.html) |
| Magma `GrpBrd` | Magma | braid groups with normal forms, comparison of elements, conjugacy testing and search | proprietary (commercial system) | [Magma handbook](https://magma.maths.usyd.edu.au/magma/handbook/text/938) |
| flipper (Mark Bell) | Python 3.8–3.12 | mapping classes of punctured surfaces on ideal triangulations: action on laminations, Nielsen–Thurston type, dilatation, veering triangulation of the mapping torus | MIT | [repository](https://github.com/MarkCBell/flipper) |
| SnapPy; Twister (Bell, Hall, Schleimer) | Python/C | hyperbolic 3-manifolds; Twister builds surface bundles and ships inside SnapPy | GPL v2 or later (SnapPy); GPL (Twister) | [SnapPy](https://www.github.com/3-manifolds/SnapPy); [Bell's software page](https://markcbell.github.io/software.html) |
| Chevie.jl, module Garside (Jean Michel) | Julia | Garside monoids and groups in general, braid groups as a case; normal forms | not checked | [documentation](https://jmichel7.github.io/Chevie.jl/dev/garside) |
| Braids.jl (jwvictor) | Julia | basic operations, inverses, unreduced Burau, Dehornoy's reduction; alpha | not checked | [repository](https://www.github.com/jwvictor/Braids.jl) |
| BraidGroup.jl | Julia | Artin's B_∞: free simplification, Dehornoy reduction for equality, drawings | not checked | [JuliaHub](https://docs.juliahub.com/General/BraidGroup/stable/autodocs/) |
| braidpy (Baptiste Labat) | Python ≥ 3.10 | braid words, product, inverse, handle reduction, normal forms, permutation, linking numbers, Alexander polynomial, conjugacy, drawings; aimed at physical braiding; alpha, a one-person project built with AI coding tools | MPL-2.0 | [documentation](https://braidpy.readthedocs.io/en/latest/) |
| Guillemot–Lairez `braids` | Rust | certified braid monodromy from algpath's enclosures | not checked | [arXiv:2601.23073](https://arxiv.org/abs/2601.23073) |
| SIROCCO (Marco-Buzunariz, Rodríguez) | C | certified root continuation; braids of root motions through piecewise-linear paths in tubes; used by Sage | not checked | [arXiv:2601.23073](https://arxiv.org/abs/2601.23073) (reference list) |
| graph-braid-presenter (Berlyne, Maciążek) | Python | presentations and free splittings of graph braid groups | not checked | [mirror](https://git.histb.com/danberlyne/graph-braid-presenter) |

- braidlab also uses, per its README: GMP inside its MEX files, John D'Errico's Variable Precision Integer toolbox (bundled in the binary archive), Markus Buehren's `assignmentoptimal`, and a ThreadPool by Jakob Progsch and Václav Zeman. — [repository](https://github.com/jeanluct/braidlab)
- Sage marks the conjugacy, summit-set, sliding-circuit and Thurston-type methods as needing libbraiding; `left_normal_form` uses it when present and otherwise an `'artin'` method. — [reference manual](https://doc.sagemath.org/html/en/reference/groups/sage/groups/braid.html)

### Inferences

- **[inference] Licence consequences for an MIT library.** No GPL code (braidlab, CBraid, Braiding, libbraiding, Sage, SnapPy, Twister) may be copied or linked. Running them to produce fixtures is permitted: the output values are data. Following the project's practice, the fixtures should be written by `make_*fixtures.py` scripts that call Sage (in WSL), with source and retrieval date. MPL-2.0 (braidpy) is file-level copyleft and compatible with being a separate dependency, but it is an alpha project and a weak oracle.
- **[inference] Fixtures by oracle.** Normal forms, gcd/lcm, SSS/USS/SC sizes and elements, Thurston type: Sage with libbraiding. Burau (four bases), LKB, TL, Alexander, Jones: Sage. Dynnikov coordinates and entropy: braidlab needs MATLAB, so prefer closed forms (σ₁σ₂⁻¹; Hall–Yurttaş and Hironaka–Kin families with known dilatations as roots of stated polynomials) and flipper (MIT) for dilatations. Trajectory braids: synthetic trajectories of known braid (rigid rotation gives Δ² per turn; a prescribed word realised by smooth exchanges), not another program's output.
- **[inference]** Sage's conventions (0-based generator names, 1-based signed integers, right action on the free group, several reduced Burau bases) differ from González-Meneses's and Wikipedia's; every fixture must record the convention map.

### Gaps

- **GAP:** no dedicated GAP 4 package for braid groups was found; I recall that CHEVIE (GAP 3) holds Garside and braid monoids, of which Chevie.jl is the port, and that KBMAG computes automatic structures, but neither was confirmed in this session.
- **KnotTheory`** (Mathematica, Bar-Natan and the Knot Atlas): has braid representatives (`BR`) and invariants; its licence was not found.
- **SageMath's licence version** (GPL v2 or later, **[from memory]**) and the GPL versions of CBraid/libbraiding were not read from a licence file.
- **"pybraids":** nothing found under that name; braidpy is the Python package found. Other Python code (e.g. in SnapPy's `spherogram`, which has braid closures and link invariants, **[from memory]**) was not checked.
- Maintenance status: braidlab's repository shows 1,789 commits and a 2026 copyright notice but no releases; no last-commit dates were read for any repository.

---

## 10. Verdicts on the Brief's Claims from Memory

### Takeaway

Of the claims marked as from memory in the brief, all the citations exist as given; three statements need amending: Burau at n = 4 (a proof is now claimed), "point braids are trivial in dimension three" (the group is the symmetric group, and only for simply connected spaces is the pure part trivial), and the expectation that a prior soundness theory for sampled trajectory braids exists in the dynamics literature (it does not; the certified theory is Guillemot and Lairez's, for root paths).

### Cited Findings

| Claim in the brief | Verdict | Source |
|---|---|---|
| Burau unfaithful for n ≥ 5 | correct (Bigelow 1999 for n = 5; Long–Paton 1993 for n ≥ 6; Moody first) | [Wikipedia](https://en.wikipedia.org/wiki/Burau_representation); [arXiv:math/9904100](https://arxiv.org/abs/math/9904100) |
| Burau "open for n = 4" | out of date: faithfulness claimed by Bharathram, Birman and Brendle, arXiv:2607.05283 (July 2026, v2 September 2026); unverified | [arXiv:2607.05283](https://arxiv.org/abs/2607.05283) |
| Lawrence–Krammer–Bigelow faithful | correct, all n ≥ 2, dimension n(n−1)/2 | [arXiv:math/0005038](https://arxiv.org/abs/math/0005038) |
| Sliding circuits of Gebhardt and González-Meneses | correct; SC ⊆ USS; worst case not improved | [arXiv:0808.1430](https://arxiv.org/abs/0808.1430) |
| Dynnikov 2002; Moussafir 2006; Hall and Yurttaş 2009; braidlab | correct (Hall–Yurttaş is arXiv:0808.3053; the update rules in Thiffeault's form are in arXiv:0906.3647) | [arXiv:0808.3053](https://arxiv.org/abs/0808.3053); [arXiv:1410.0849](https://arxiv.org/abs/1410.0849) |
| Burau spectral-radius bound (Fried; Kolev; Band and Boyland) | correct; sharp at a root of unity only at −1, under a condition on singularities | [arXiv:math/0612716](https://arxiv.org/abs/math/0612716) |
| Thiffeault 2005 and 2010 | correct: "Measuring topological chaos" (PRL 2005); "Braids of entangled particle trajectories", Chaos 20, 017516 (2010) | [arXiv:nlin/0409041](https://arxiv.org/abs/nlin/0409041); [arXiv:0906.3647](https://arxiv.org/abs/0906.3647) |
| Budišić and Thiffeault 2015, finite-time braiding exponent | correct (arXiv:1502.02162) | [arXiv:1502.02162](https://arxiv.org/abs/1502.02162) |
| Allshouse and Thiffeault, coherent structures | correct (arXiv:1106.2231; Physica D, DOI 10.1016/j.physd.2011.10.002) | [arXiv:1106.2231](https://arxiv.org/abs/1106.2231) |
| Boyland–Aref–Stremler | correct: J. Fluid Mech. 403 (2000) 277–304 | [author's copy](https://people.clas.ufl.edu/boyland/files/stir.pdf) |
| Moore 1993; Montgomery | correct: Moore, PRL 70, 3675–3679, on n-body orbits (the figure-eight), title "Braids in classical dynamics"; Montgomery, Nonlinearity 11 (1998) 363–376. Point-vortex braid types are not Moore's subject | [Wikipedia](https://en.wikipedia.org/wiki/N-body_choreography); [index record](https://acnpsearch.tweb-dev.unibo.it/singlejournalindex/52554415) |
| Fox and Neuwirth 1962; contractible cells of generic configurations | correct, with the precision that in the unordered space there is one top cell (distinct real parts), convex in sorted coordinates; in the ordered space n! | [arXiv:1010.0321](https://arxiv.org/abs/1010.0321); [arXiv:2601.23073](https://arxiv.org/abs/2601.23073) |
| Inverse braid monoid, Easdown and Lavers (2004) | correct: Adv. Math. 186, 438–455 | [arXiv:0704.3002](https://arxiv.org/abs/0704.3002) |
| Lavers's theory of vines (1997) | correct: Comm. Algebra 25(4), 1257–1284; merging without splitting | [MaRDI](https://portal.mardi4nfdi.de/wiki/Item:Q4338220); [nLab](https://ncatlab.org/nlab/show/vine) |
| Braided injections (Schlichtkrull and Solberg 2016) | correct in substance: arXiv:1403.1101 (2014, revised 2015); the journal year 2016 was not confirmed in this session | [arXiv:1403.1101](https://arxiv.org/abs/1403.1101) |
| Singular braid monoids (Baez; Birman) | correct; "Baez–Birman monoid" | [arXiv:0704.3002](https://arxiv.org/abs/0704.3002) |
| Graph braid groups (Abrams; Farley and Sabalka; Ko and Park) | correct | [arXiv:math/0410539](https://arxiv.org/abs/math/0410539) |
| "Point braids are trivial in dimension three and above" | imprecise: the pure braid group of a simply connected manifold of dimension ≥ 3 is trivial and the braid group is Σ_k | [arXiv:1311.5642](https://arxiv.org/pdf/1311.5642) |
| braidlab is MATLAB | correct; GPL-3.0 | [repository](https://github.com/jeanluct/braidlab) |
| "`braids` in Julia", "pybraids" | Julia: Braids.jl, BraidGroup.jl and Chevie.jl exist; no "pybraids" was found, braidpy (MPL-2.0) was | [Braids.jl](https://www.github.com/jwvictor/Braids.jl); [braidpy](https://braidpy.readthedocs.io/en/latest/) |

### Inferences

- **[inference] Conventions an implementer must fix and record**, since the sources disagree: (1) the sign of σᵢ (clockwise: Thiffeault, braidlab, Bigelow; counterclockwise: González-Meneses §3.2, Guillemot–Lairez); (2) the reading order of words (time left to right in all sources read) and whether braids act on the left (Bigelow) or the right (Sage's free-group action); (3) the form of Artin's action on F_n (González-Meneses's xᵢ ↦ xᵢ₊₁, xᵢ₊₁ ↦ xᵢ₊₁⁻¹xᵢxᵢ₊₁ against Sage's); (4) the Burau matrices (unreduced block, and which reduced basis; t against t⁻¹); (5) the LKB variables (Krammer's t is Bigelow's −t); (6) the normalisation of Dynnikov coordinates (2n−4 in Thiffeault, 2n from (0,1,…,0,1) in Dehornoy) and whether σᵢ is clockwise in the update rules; (7) the projection axis and the lexicographic tie-break for configurations; (8) the direction of Dehornoy's ordering (smallest or largest index); (9) indexing of generators from 0 (Sage's names) or 1.

### Gaps

- The independent status of arXiv:2607.05283 will change; it should be rechecked before any statement about n = 4 is published.
- Items marked **[from memory]** throughout (journal volumes and pages for several papers, the SB_n relations, the tangle-category theorems, the periodic-braid identities, the entropy of σ₁σ₂⁻¹) need a pass against primary sources.
