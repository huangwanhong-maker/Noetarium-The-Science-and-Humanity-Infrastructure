# Undertakings and Partnerships

*Status: draft (2026-11-19).* The theory of the diplomacy domain's first two modules, `relations` and `partnerships` ([ADR 0132](../../../development/decisions/0132-diplomacy-and-multilateral-phronimos.md)). It states what they compute, and proves the four results on which their answers rest. The concepts are those of the user's monograph (Huang 2026); the survey behind them is [Multilateral practice, diplomacy and partnerships](../../../research/literature/reports/Multilateral%20practice,%20diplomacy%20and%20partnerships.md).

## 1. An Undertaking as a Typed Dependency Structure

**Definition 1.1 (undertaking).** An undertaking is a tuple U = (V, T, E, O, D, Γ):

- V is a finite set of participants, each with a kind (a state, an organisation, a city, a community, a person, an AI system, a dataset, a forum…) and a flag saying whether it holds authority by its own constitution;
- T is a finite set of relation types. Each type may carry authority, evidential reliance or challenge, or none of them;
- E is a finite set of ties e = (k, a, b, [s, u), ε): a type k ∈ T, an ordered pair a ≠ b of participants, a half-open interval of dates on which it holds (either end open), and a flag ε saying it was recorded absent;
- O is a finite set of designs of observation o = (k, C, [s, u)): a type, the participants C ⊆ V it covers, and a period;
- D is a finite set of decisions, each with a date, its authorities, its evidence, those it affects and its venues of challenge, all subsets of V;
- Γ is a finite set of constraints, each held by a participant.

This is the monograph's R = (V, {E_k}, D, Γ) (its §10.4.2) with time and observation made explicit. E_k is the set of ties of type k.

**Definition 1.2 (status).** For a type k, an ordered pair (a, b) and a date t, the status σ_k(a, b; t) is:

- *present* if some tie (k, a, b, I, false) has t ∈ I;
- else *absent* if some tie (k, a, b, I, true) has t ∈ I, or some design (k, C, J) has t ∈ J and a, b ∈ C;
- else *not observed*.

*Remark 1.3.* The definition encodes the monograph's rule that "missing observations must not be interpreted automatically as absent relationships". Absence needs a record or a design. A design is a claim about a procedure of observation (a register kept complete, a survey of every pair), and it is the caller's to declare.

**Proposition 1.4 (absence is never inferred from silence).** Let U′ be U with a tie or a design removed. Then every pair that is absent in U′ is absent in U, and every pair present in U′ is present in U. Removing every design and every tie recorded absent leaves no pair absent.

*Proof.* The conditions for *present* and for *absent* in Definition 1.2 are existential over E and O, so they are monotone under adding ties and designs. A recorded presence takes priority over absence, and adding a tie recorded absent cannot remove a present one. With no design and no tie recorded absent, the second clause is never met. ∎

**Definition 1.5 (relational position).** The position of a participant p at t is the record's part of the monograph's Definition 9.5: the ties in force at t of every type, in and out of p; the participants that authorise p (sources of ties of a type carrying authority into p); those p relies on and is relied on by; the participants whose decisions p may contest; and the decisions made by t that p holds, is affected by, or is a venue of. A participant holds *formal authority* when it is constituted or authorised by someone.

## 2. Diagnostics of an Undertaking

The monograph's Table 10.2 names failures of a coordinating function. The module computes those a record can show, each as a statement about U at a date t:

| Diagnostic | Raised when |
|---|---|
| evidence | a decision names no evidence |
| authorisation | a decision's authority holds no formal authority at the decision's date (a recommendation recorded as a decision), or no one is recorded as deciding it |
| participation | a decision names no venue; or someone it affects is neither a venue nor tied by a challenge to any of its venues |
| AI dependency | every item of a decision's evidence is an AI system; or one item is counted twice ("repeated outputs are not independent corroboration") |
| succession | a participant is a cut vertex of the graph of ties carrying authority in force at t; a tie is a bridge of it |
| concentration | a participant receives at least a declared share of the ties of evidential reliance |
| unobserved | a type has no design in force and no tie recorded absent, so all its unrecorded pairs are not observed |

**Definition 2.1 (graph of authority).** G_A(t) is the multigraph on V with one undirected edge {a, b} for each tie (k, a, b, I, false) with k carrying authority and t ∈ I.

A participant p is a *cut vertex* of G_A(t) when G_A(t) − p has more components among V ∖ {p} than G_A(t) has among V, excluding the case where p is isolated. A tie is a *bridge* when removing its edge leaves more components. Both are found in O(|V| + |E|) by Hopcroft and Tarjan's lowpoints (1973), in morphe's `combinatorics/blocks.hpp` (morphe's decision 0020).

*Remark 2.2.* The succession diagnostic answers "can the undertaking continue without p?" in one exact sense. Without p, some participants who were joined by chains of authority are no longer joined. G_A reads authority as undirected. Whether a direction-sensitive reading (the dominators of a directed graph from a constituted root) fits a given undertaking better is left open (README §7).

*Caveat 2.3.* Each diagnostic is a property of the record under its declared types and thresholds. It is evidence about the undertaking, not a verdict on it. The checks hold succession to removal itself on 300 random undertakings, with ties ended and recorded absent among them.

## 3. Values and Their Scales

A partnership's ledger holds values v_{p,d} of party p on dimension d. Each value carries the interval [v⁻, v⁺] its evidence allows, and possibly a standard error. Each dimension d has a scale (Stevens 1946) and a direction. Its scale is a group 𝒢_d of admissible transformations: all bijections (nominal), strictly increasing maps (ordinal), x ↦ αx + β with α > 0 (interval), and x ↦ αx with α > 0 (ratio).

**Definition 3.1 (order on a dimension).** For values x, y on an ordered dimension where higher is better, x is *better* than y when x⁻ > y⁺. It is *worse* when x⁺ < y⁻, *equal* when both are points and coincide, and *overlapping* otherwise. On a nominal dimension x and y are equal or different. A value with no observation is *missing*, never lower.

**Definition 3.2 (dominance).** For vectors a, b over the dimensions, a *dominates* b when on every dimension a is better than or equal to b, and better on at least one. The two *cross* when a is better on some dimension and worse on another.

**Proposition 3.3 (dominance is a strict partial order).** On any set of vectors, dominance is irreflexive and transitive. Every finite non-empty set of alternatives therefore has an undominated member.

*Proof.* Irreflexive: a is better than a on no dimension, since x⁻ > x⁺ is impossible. Transitive: suppose a dominates b and b dominates c. Fix a dimension; the pair's orders lie in {better, equal}. If both are equal, all three are the same point, so a equals c. If a is better than b and b equals c, then a⁻ > b⁺ = c⁺. If a equals b and b is better than c, then a⁻ = b⁻ > c⁺. If both are better, a⁻ > b⁺ ≥ b⁻ > c⁺. So a is better than or equal to c on every dimension, and better wherever either step was better, which happens at least once. On a nominal dimension, equal and equal give equal. A strict partial order on a finite non-empty set has a maximal element. ∎

**Proposition 3.4 (crossing leaves the ranking to the rule).** Let a and b cross: a is better on d and worse on e, both ordered dimensions with finite bounds. Then for each of the two orders there is a weighted sum with positive weights that ranks the alternatives in that order, whatever values in their intervals they take. This is the monograph's Proposition 12.1, in the module's terms.

*Proof.* Write Δ_f for the difference a_f − b_f on dimension f, signed so that positive means better for a. Then Δ_d ≥ a⁻_d − b⁺_d =: δ > 0 and Δ_e ≤ −(b⁻_e − a⁺_e) =: −η < 0. Let B bound |Δ_f| on every dimension. Take weight M on d, 1 on e and ε on each other dimension. The weighted difference is at least Mδ − B − εB(n − 2), which is positive once M > B(1 + ε(n − 2))/δ. Exchanging the roles of d and e gives a rule that puts b first. ∎

*Consequence.* When two alternatives cross, no ranking of them is in the data. The module says so and ranks only by a rule someone declares.

**Proposition 3.5 (which weighted sums are meaningful).** Let a rule weigh dimensions d with weights w_d, each read as a worth in a common unit per unit of d, so that a change of d's unit by α_d changes w_d to w_d/α_d. Compare two alternatives by S(a) = Σ_d w_d a_d against S(b). Then:

1. if every weighted dimension is ratio or interval, the comparison S(a) > S(b) is invariant under every admissible transformation applied to each dimension (to both alternatives alike, with the weights carried as their units require);
2. if some weighted dimension is ordinal, with at least two levels in use whose values differ between the alternatives, there are values and an admissible transformation of it that reverse the comparison. Its truth is then not meaningful.

*Proof.* (1) Under x ↦ α_d x + β_d with weights w_d/α_d, S(a) − S(b) = Σ_d (w_d/α_d)(α_d a_d + β_d − α_d b_d − β_d) = Σ_d w_d (a_d − b_d), unchanged. The ratio case is β_d = 0. (2) Take two dimensions with equal weights, A = (1, 4) and B = (3, 3) on the ordinal one and equal elsewhere: S(A) = 5 < 6 = S(B). Under the strictly increasing map x ↦ x³, S(A) = 65 > 54 = S(B). ∎

*Remark 3.6.* (1) makes comparisons meaningful, not the sum's level: on an interval scale the level of S shifts with β. The module returns the sum with its interval, carried through by the signs of the weights; a reader compares sums by the same rule. The checks hold the refusal of (2) to kybernesis's test of meaningfulness, which finds the same transformation.

## 4. Benefit Party by Party

Each party p declares criteria of its own benefit: pairs (d, θ) read "v_{p,d} reaches θ" in d's direction. Each criterion carries the reading of the open term it construes and its source (a resolution, a strategy, an analyst).

**Definition 4.1 (standing).** Let B_p be the box of values the evidence allows for p's criteria, the product of their intervals. p's benefit is:

- *undecided* if a criterion's value is not observed;
- *untenable* if some criterion is unmet at every point of B_p;
- *clear* if every criterion is met at every point of B_p;
- *arguable* otherwise.

These are the labels of tekmerion's construal and apodeixis's derivability under interpretation, with the points of B_p as the readings.

**Proposition 4.2 (standing is stable under sharper evidence).** If the evidence narrows, B′_p ⊆ B_p, a clear standing stays clear and an untenable one stays untenable. Only an arguable standing can change, to any of the three.

*Proof.* "Met at every point of B_p" is universal over B_p, so it holds over any subset. An untenable criterion has its whole interval on the failing side of θ, and so does any sub-interval. ∎

**Proposition 4.3 (Fréchet's bounds).** Let A_1, …, A_k be events (the criteria met) with P(A_i) ∈ [l_i, u_i]. Then, for every joint law,

max(0, Σ_i l_i − (k − 1)) ≤ P(A_1 ∩ … ∩ A_k) ≤ min_i u_i,

and when l_i = u_i = p_i both bounds are attained by some joint law.

*Proof.* The upper bound holds since the intersection lies in each A_i. For the lower, P(∩A_i) = 1 − P(∪A_iᶜ) ≥ 1 − Σ_i (1 − p_i) = Σ_i p_i − (k − 1), by Boole's inequality, and is minimised at p_i = l_i. To attain the bounds, place each A_i on the unit interval with Lebesgue measure. A_i = [0, p_i] for all i (comonotone) gives min p_i. When Σ(1 − p_i) ≤ 1, laying the complements A_iᶜ end to end without overlap gives Σ p_i − (k − 1); otherwise they can be laid so the intersection is empty. (Fréchet 1935; Rüschendorf 1991.) ∎

The module takes p_i from a value's estimate and error, by the normal law (tyche), when the evidence does not settle the criterion. It takes 1 or 0 when the interval settles it, and [0, 1] when neither an error nor a settling interval is given. It reports the conjunction's interval. The checks hold the bounds to three joint laws of two criteria with margins 0.9 and 0.8, which reach both ends (0.7 and 0.8) with independence (0.72) between.

**Mutual benefit** is judged by a declared standard (kybernesis's: a least standing, a probability, or both), party by party. The partnership's verdict holds only when every party's holds, and it fails at the first party whose benefit fails, named. It is never summed.

## 5. Commitments in Time

**Definition 5.1.** A commitment c has a holder, a goal, an indicator (name, unit, baseline β, target τ, direction), a date due δ, and a list of amendments. Each amendment has a date and may set a new target, a new date or a new holder. *As amended at t* means with every amendment dated on or before t applied in order. Let r_1, …, r_m be the reports of c dated on or before t, in order. The state at t is:

- *honoured* if some report reaches the target as amended and the first such is on or before the date as amended;
- *honoured late* if it is after that date;
- *unreported* if m = 0;
- *overdue* if m > 0, none reaches, and t is past the date;
- *in progress* otherwise.

Progress is (r_m − β)/(τ − β).

*Remark 5.2.* Amendments never reach back. A reading at a past date gives the same answer whatever was amended later, so the record can be appended to and sealed, as archeion's journal seals a docket. An unreported commitment is not overdue: the absence of a report is the absence of an observation (Remark 1.3).

**Definition 5.3 (the SMART screen).** A scheme is a list of criteria, each a test of a commitment answered holds, fails or undecided, with its reason. The screen's verdict holds when all hold, fails when any fails, and is undecided otherwise. The default scheme is UN DESA's for the SDG Actions Platform: Specific, Measurable, Achievable, Resource-based (a secured resource base), Time-bound. *Achievable* is undecided until someone is recorded as having judged it, with a reason: the record cannot make that judgment.

## 6. Signs of Failure in a Partnership

The survey (§B.4.5) finds seven modes of failure detectable from a structured record. Each is computed as a sign by a declared threshold:

| Mode | Sign |
|---|---|
| paper partnership | nothing contributed or reported for a declared number of days since the last activity (or since registration) |
| drift | a declared share of contributions serves no declared goal (directly, or through its commitment) |
| resource fragility | one party gives a declared share of a kind of contribution in a unit (its CR₁, with the Herfindahl–Hirschman index from nomos); or a commitment has no secured resource |
| capture | one party set a declared share of the goals while some member set none |
| collaborative inertia | a commitment's reports have not moved toward its target for a declared number of days |
| accountability gap | a commitment past its date, not honoured, with nothing reported since; or one with no indicator and target, so that no one can tell |
| succession loss | the holder of an open commitment is no longer a member, and no amendment transferred it |

*Caveat 6.1.* These are signs, not findings. Whether they predict a partnership's failure is an empirical question. Pattberg and Widerberg's nine conditions (2016) are naturally set-theoretic, and a comparison of cases (zetesis's QCA) can test them, with the outcome defined by a declared scheme.

## 7. References

- Fréchet, M. (1935), Généralisations du théorème des probabilités totales, *Fundamenta Mathematicae* 25, 379–387.
- Hopcroft, J. and Tarjan, R. (1973), Algorithm 447: efficient algorithms for graph manipulation, *Communications of the ACM* 16(6), 372–378.
- Huang, W. (2026), *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos*, Knowledge Commons, DOI 10.17613/s9xqh-xm397 (v6.6): Definitions 9.5 and 10.4, §10.4.2, Table 10.2, Proposition 12.1, Appendix D.
- Krantz, D. H., Luce, R. D., Suppes, P. and Tversky, A. (1971), *Foundations of Measurement*, vol. I, Academic Press.
- Pattberg, P. and Widerberg, O. (2016), Transnational multistakeholder partnerships for sustainable development: conditions for success, *Ambio* 45, 42–51.
- Rüschendorf, L. (1991), Bounds for distributions with multivariate marginals, in *Stochastic Orders and Decision under Risk*, IMS Lecture Notes 19.
- Stevens, S. S. (1946), On the theory of scales of measurement, *Science* 103, 677–680.
- Suppes, P. and Zinnes, J. L. (1963), Basic measurement theory, in *Handbook of Mathematical Psychology*, vol. I, Wiley.
- UN DESA, SDG Actions Platform, About: https://sdgs.un.org/partnerships/about.
