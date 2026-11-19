# Undertakings, Partnerships and Their Affairs

*Status: draft (2026-11-19).* The theory of the diplomacy domain: its first two modules, `relations` and `partnerships` ([ADR 0132](../../../development/decisions/0132-diplomacy-and-multilateral-phronimos.md)), and the six that hold the affairs by which an undertaking changes, `rules`, `negotiation`, `events`, `mandates`, `reviews` and `institutions` ([ADR 0134](../../../development/decisions/0134-the-affairs-of-an-undertaking.md)). It states what they compute, and proves the results on which their answers rest. The concepts are those of the user's monograph (Huang 2026); the survey behind them is [Multilateral practice, diplomacy and partnerships](../../../research/literature/reports/Multilateral%20practice,%20diplomacy%20and%20partnerships.md).

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

## 7. Decision Rules

**Definition 7.1 (a rule).** Let M be a body's members and 𝒜 = {yes, no, abstain, not voting, absent, objection, reservation} the acts a member may take at a question. A *record* is a partial map ρ: M ⇀ 𝒜 at each stage of the question (a consensus, then a vote). A rule is a tree of clauses over M:

- a *threshold* with a counted set C ⊆ 𝒜, a base set B ⊆ 𝒜 and weights w: M → ℕ (each 1 unless declared) is met when w(ρ⁻¹(C) ∩ ρ⁻¹(B)) · q > p · w(ρ⁻¹(B)) (or ≥, "at least p/q"), or, given a number k, when w(ρ⁻¹(C)) ≥ k; a *blocking* threshold is met when the unblocked one is not;
- a *concurrence* over members S with a concurring set C is met when ρ(s) ∈ C for every s ∈ S;
- a *consensus* with a blocking set C is met when no member's act lies in C;
- a *quorum* is a threshold on the members present;
- *all of*, *any of* and a *fallback* compose clauses (a fallback is met when its first part is, else when its second is, on its own stage's record).

An *open term* τ names a role (a counted, base, concurring or blocking set) and admits readings R_τ, each a subset of 𝒜 with its source. An interpretation ι chooses one reading of each term, and O(ρ, ι) ∈ {0, 1} is the rule's outcome on a complete record. The comparisons are exact in integers.

**Proposition 7.2 (monotone rules).** Order 𝒜 so that yes is above every other act. Suppose every threshold counts C = {yes} with yes ∈ B and is not blocking, every concurrence's set contains yes, every consensus's blocking set excludes yes, and every quorum's set contains yes. Then for every interpretation, O(·, ι) is monotone: changing one member's act to yes never turns an adopted question into one not adopted.

*Proof.* Each kind of leaf is monotone. A threshold with a number: the counted weight does not fall. A threshold with a fraction, with c and b the counted and base weights: if the member's act was in B, c rises by w and b is unchanged; if not, both rise by w, and (c + w)/(b + w) ≥ c/b exactly when c ≤ b, which holds since C ∩ B ⊆ B. A concurrence, a consensus and a quorum are met by yes whenever they were met before. Conjunction and disjunction preserve monotonicity, and a fallback is the disjunction of its parts on records that change together. ∎

The checks hold this on 400 random records under random quorums and majorities.

**Definition 7.3 (an outcome under readings).** Suppose a record leaves the members U unrecorded, each with the acts its procedure admits (a vote's yes, no, abstain, absent; a consensus's joining, objection, reservation, absence). Under ι the outcome is *adopted* when O = 1 on every completion, *not adopted* when O = 0 on every completion, and *turns on the members not recorded* otherwise. The question's answer reads this over the admissible interpretations of tekmerion's construal, in which an authority's reading of a term sets aside an analyst's. It is adopted or not adopted when every admissible interpretation agrees, turns on the readings when they differ, and turns on the members not recorded when any admissible interpretation does.

*Remark 7.4.* Whether a permanent member's abstention is a concurring vote under Article 27(3) of the Charter is such a term. Its readings are {yes}, the words alone, and {yes, abstain, not voting}, the Court's (Namibia, 1971, §22). With the second read as an authority, the first is set aside, and nine affirmative votes with an abstaining permanent member adopt the decision clearly. With both read as an analyst's, the outcome turns on the term, and the term is its pivot. The simple game a rule makes when every member votes yes or no (nomos's games) has a priori power that belongs to the rule alone. For Article 27(3) it is 421/2145 for a permanent member and 4/2145 for an elected one, in Shapley and Shubik's measure (Felsenthal and Machover 1998).

## 8. Negotiation at Two Levels

**Definition 8.1 (a negotiation).** Issues I, each with finitely many options; a package π chooses an option of each issue (of a draft: of those still in brackets). A *valuation* gives each option o of each issue i an interval V_i(o) = [v⁻, v⁺], the worth of no agreement on i an interval N_i, and an outside option N₀. Values add (separability, a declared assumption), so a package of all the issues is worth V(π) = Σ_i V_i(π_i) and the reservation is N = N₀ + Σ_i N_i, interval sums. The valuation *accepts* π clearly when V(π)⁻ ≥ N⁺, untenably when V(π)⁺ < N⁻, and arguably otherwise. A party has a negotiator's valuation, ratifiers each with their own, and a ratification rule (Definition 7.1) over them. Its ratifiers accepting clearly vote yes, those rejecting clearly vote no, and the rest may vote either way. Ratification is clear when the rule adopts under every way the undecided go, untenable under none, and arguable otherwise. The party accepts when its negotiator and its ratification do (a conjunction of standings), and an agreement on π is the conjunction over the parties.

**Proposition 8.2 (the conjunctions are exact).** Let a *world* choose each actor's values within its intervals. Then π is clearly agreed exactly when it is agreed in every world, untenably exactly when in none, and arguably exactly when in some and not in others.

*Proof.* Each actor's acceptance depends on its own values alone, and a world chooses them independently across actors. A conjunction of conditions on disjoint coordinates holds in every world exactly when each holds in every world. It holds in some world exactly when each holds in some world, since the favourable choices can be made together. The same holds for ratification: its outcome over the undecided ratifiers ranges over exactly the coalitions their worlds produce. ∎

**Proposition 8.3 (a tighter rule never enlarges a win-set).** If every coalition that wins under a ratification rule R′ wins under R, then for every package π, the standing of π's ratification under R′ is no stronger than under R (clear > arguable > untenable).

*Proof.* Let S₀ be the ratifiers accepting clearly and U the undecided. Ratification under a rule is clear when every S₀ ∪ T, T ⊆ U, wins, and untenable when none does. Winning under R′ implies winning under R, so clear under R′ implies clear under R, and untenable under R implies untenable under R′. ∎

The checks hold this on 100 random negotiations, unanimity of three against a majority.

**Proposition 8.4 (linkage at one rate).** Let two parties' valuations be points, exactly opposed at one rate k > 0 on every issue: V_B,i(o) = c_i − k V_A,i(o) for every option. Let each issue's surplus s_i = c_i − k N_A,i − N_B,i be negative, and the outside options be zero. Then no package of the linked negotiation is agreeable.

*Proof.* If π were agreeable, Σ_i V_A,i(π_i) ≥ Σ_i N_A,i and Σ_i (c_i − k V_A,i(π_i)) ≥ Σ_i N_B,i. Multiplying the first by k and adding gives 0 ≥ Σ_i (k N_A,i + N_B,i − c_i) = −Σ_i s_i > 0, a contradiction. ∎

So linkage creates a zone only when the parties trade the issues at different rates (Tollison and Willett 1979; Sebenius 1983). The checks show such a zone created on two issues traded at rates 10 : 2 and 2 : 10, and none on 200 random negotiations at one rate.

*Remark 8.5.* Value claimed is read by Nash's, Kalai and Smorodinsky's and the egalitarian solutions (nomos's bargaining) on the negotiators' values at their midpoints, over the clear packages. That is a declared basis, never a measure of what was gained. A party whose ratification excludes the packages favourable to the other claims more there (Schelling 1960; Putnam 1988). On one issue split in tenths, the Nash point moves from five tenths to seven when the first party's parliament will ratify no less than seven.

## 9. Events, Mandates and Reviews

**Definition 9.1 (an event's support).** An event rests on reports, each with a source and an origin (itself when it relays none). Reports sharing an origin form a group that relays it. Channels may be declared: each origin's P(it registers | event) and P(it registers | none), and each source's P(it reports | origin registered) and P(it reports | not). Then the reports' force for the event's having occurred is tekmerion's dependent ratio. A group is never more forceful than its origin however many reports it holds, and groups of independent origins multiply (tekmerion, docs/theory.md §2.1). Counted as independent, the same reports would grow without bound. Take one wire carried by fifty outlets, with an origin of 0.8 and 0.05 and relays of 0.9 and 0.1: it is worth just under 16, where the count as independent gives about 10³⁶.

*Remark 9.2.* Two records that coincide (one category, a shared actor and target, dates within a window) are reported as possibly one event and kept as two. A record is a manifestation, and coincidence does not identify what is manifested (the user's Generative Relational framework, as in archeion).

**Definition 9.3 (a mandate's lifecycle).** A mandate is in force on [begins, ends). A later mandate *supersedes*, *consolidates* or *terminates* it from the later one's beginning. After its end it is *renewed* when a later mandate renews it, and *expired* otherwise. The relations from earlier to later mandates must be acyclic. A mandate's lineage is its cone in their order (morphe's acyclic relations), and what its latest member added to its roots is *creep*. A review falls due a declared number of days after the last review recorded (or after the beginning) or by a declared date, and is overdue when the date is past.

**Definition 9.4 (an implementation's standing).** Take each reporter's latest report on a recommendation. The claim that it is implemented is *contested* when some of these say it is and some say it is not. It is *evidenced* when a reporter other than the party reviewed says it is and names its evidence, and *claimed* when it is said to be done otherwise. The party reviewed is a party to its own report. A reporter who reports progress over time is read at its latest report and never contests itself.

## 10. Institutions and Reciprocity

**Definition 10.1 (memberships as a context).** At a date, the relation "p is a member of i" between parties and institutions is a formal context. Its concepts are the pairs (P, J) with J the institutions every party of P belongs to and P the parties belonging to every institution of J. Its canonical basis of implications J₁ → J₂ (Guigues and Duquenne 1986) is the fewest statements from which every regularity "every member of all of J₁ belongs to all of J₂" follows (morphe's concepts). On an issue, two institutions are *nested* when one's members lie within the other's, *overlapping* when they share members and neither contains the other, and *apart* otherwise.

**Definition 10.2 (imbalances).** In a window, let f(a, b) be the flow from a to b, T = Σ f the total, and out(a), in(a) a party's giving and receiving. The *pairwise imbalance* is Π = Σ_{{a,b}} |f(a, b) − f(b, a)| / T, and the *group imbalance* is Γ = Σ_a |out(a) − in(a)| / (2T). Π = 0 when every pair balances (specific reciprocity), Γ = 0 when every party balances against the group (diffuse reciprocity; Keohane 1986).

*Remark 10.3.* Both lie in [0, 1]: Π ≤ Σ_{{a,b}} (f(a, b) + f(b, a)) / T = 1, and Γ ≤ Σ_a (out(a) + in(a)) / (2T) = 1.

**Proposition 10.4 (specific reciprocity implies diffuse).** Γ ≤ Π in every window.

*Proof.* out(a) − in(a) = Σ_b (f(a, b) − f(b, a)), so |out(a) − in(a)| ≤ Σ_b |f(a, b) − f(b, a)|. Summing over a counts each pair twice: Σ_a |out(a) − in(a)| ≤ 2 Σ_{{a,b}} |f(a, b) − f(b, a)|, and dividing by 2T gives Γ ≤ Π. ∎

*Remark 10.5.* The converse fails. A cycle of equal gifts a → b → c → a has Π = 1 and Γ = 0: reciprocity that is diffuse and not specific at all, which is the kind Ruggie (1992) finds proper to multilateralism. The length of window over which Γ falls is its horizon. The checks hold Proposition 10.4 on 200 random sets of transfers at three windows.

## 11. References

- Alter, K. J. and Meunier, S. (2009), The politics of international regime complexity, *Perspectives on Politics* 7, 13–24.
- Felsenthal, D. S. and Machover, M. (1998), *The Measurement of Voting Power*, Edward Elgar.
- Fréchet, M. (1935), Généralisations du théorème des probabilités totales, *Fundamenta Mathematicae* 25, 379–387.
- Ganter, B. and Wille, R. (1999), *Formal Concept Analysis: Mathematical Foundations*, Springer.
- Guigues, J.-L. and Duquenne, V. (1986), Familles minimales d'implications informatives résultant d'un tableau de données binaires, *Mathématiques et sciences humaines* 95, 5–18.
- Hopcroft, J. and Tarjan, R. (1973), Algorithm 447: efficient algorithms for graph manipulation, *Communications of the ACM* 16(6), 372–378.
- Huang, W. (2026), *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos*, Knowledge Commons, DOI 10.17613/s9xqh-xm397 (v6.6): Definitions 9.5 and 10.4, §10.4.2, Table 10.2, Proposition 12.1, Appendix D.
- International Court of Justice (1971), Legal Consequences for States of the Continued Presence of South Africa in Namibia (South West Africa) notwithstanding Security Council Resolution 276 (1970), Advisory Opinion of 21 June 1971, *ICJ Reports* 1971, 16, §22.
- Keohane, R. O. (1986), Reciprocity in international relations, *International Organization* 40, 1–27.
- Krantz, D. H., Luce, R. D., Suppes, P. and Tversky, A. (1971), *Foundations of Measurement*, vol. I, Academic Press.
- Pattberg, P. and Widerberg, O. (2016), Transnational multistakeholder partnerships for sustainable development: conditions for success, *Ambio* 45, 42–51.
- Putnam, R. D. (1988), Diplomacy and domestic politics: the logic of two-level games, *International Organization* 42, 427–460.
- Ruggie, J. G. (1992), Multilateralism: the anatomy of an institution, *International Organization* 46, 561–598.
- Rüschendorf, L. (1991), Bounds for distributions with multivariate marginals, in *Stochastic Orders and Decision under Risk*, IMS Lecture Notes 19.
- Schelling, T. C. (1960), *The Strategy of Conflict*, Harvard University Press.
- Sebenius, J. K. (1983), Negotiation arithmetic: adding and subtracting issues and parties, *International Organization* 37, 281–316.
- Stevens, S. S. (1946), On the theory of scales of measurement, *Science* 103, 677–680.
- Suppes, P. and Zinnes, J. L. (1963), Basic measurement theory, in *Handbook of Mathematical Psychology*, vol. I, Wiley.
- Tollison, R. D. and Willett, T. D. (1979), An economic theory of mutually advantageous issue linkages in international negotiations, *International Organization* 33, 425–449.
- UN DESA, SDG Actions Platform, About: https://sdgs.un.org/partnerships/about.
