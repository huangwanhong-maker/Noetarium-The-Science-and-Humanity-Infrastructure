# Evidence-Based Policy as Argument and Decision

*Status: draft (2026-11-15).* The theory note of the policy domain ([ADR 0114](../../../development/decisions/0114-evidence-based-policy-and-interpretation-by-models.md)). It states what the modules compute and proves what they rely on. The foundations' notes hold the rest: tekmerion's `docs/theory.md` (grading, transfer, interpreters), peira's (synthesis, transport) and tyche's decision 0024 (the value of information).

## 1. The Structure of the Problem

A decision maker considers an intervention for a place. Three questions stand between the studies and the decision, and the domain keeps them apart:

1. *How is it meant to work?* A theory of change, whose links are causal claims.
2. *How far is each claim shown?* The evidence for it, pooled and graded.
3. *Will it work here, and with what effect?* An argument from there to here, and a transported effect.

The decision is then a choice under uncertainty about the effect here: adopt, reject, or learn more first.

*Rationale.* Methods that fold these into one number (a pooled effect taken as the effect here, a grade taken as a probability) hide where a conclusion may fail. Kept apart, each step can be contested where it is weak: a link without evidence, a support factor not known here, a decision that turns on the worth of a unit of the outcome.

## 2. Theories of Change

**Definition 1.** A *theory of change* is a finite acyclic directed graph (N, L) whose arcs ℓ = (u, v) are causal claims. Each carries an expected sign s_ℓ ∈ {+1, −1}, a set of support factors F_ℓ, and optionally evidence: an effect β_ℓ with an interval [a_ℓ, b_ℓ] and a grade g_ℓ in a totally ordered scale G = {g_0 < … < g_{m−1}}. The *standing* of a link is
ρ(ℓ) = index(g_ℓ) when its interval lies strictly on the expected side (a_ℓ > 0 if s_ℓ = +1, b_ℓ < 0 if s_ℓ = −1), and ρ(ℓ) = −1 otherwise (no evidence, an interval reaching zero, or one on the other side).

**Proposition 1 (the weakest link).** Let a path π from the intervention x to an outcome y be shown at grade g when every link of it is. Then π is shown at g exactly when min_{ℓ∈π} ρ(ℓ) ≥ index(g).

*Proof.* Immediate from the definition. ∎ *Remark.* The content of the proposition is the reading, not the algebra: the causal chain asserts every link, so the least shown link bounds how far the chain is shown, and stronger evidence of the other links does not raise it (Cartwright and Hardie 2012, ch. 3, on the "chain of causes").

**Proposition 2 (the strongest path).** Define B(x) = m and, in a topological order, B(v) = max_{(u,v)∈L} min(B(u), ρ(u, v)), with the maximum over an empty set −∞. Then B(y) = max_{π: x→y} min_{ℓ∈π} ρ(ℓ), and the predecessors achieving each maximum spell out a path attaining it.

*Proof.* By induction along the order: every path to v ends with some arc (u, v), and its weakest link is min(weakest of its prefix to u, ρ(u, v)); maximising over the prefixes gives B(u) by hypothesis, and over the last arc gives B(v). (This is the bottleneck, or widest-path, recurrence; min and max form a semiring on G ∪ {−1, −∞, m}, and the recurrence is that semiring's path sum.) ∎ The checks hold B against every path enumerated on 300 random theories.

**Proposition 3 (Wright's rule and its enclosure).** If every link on a path from x to y has a linear effect β_ℓ and the links are not confounded, the effect of x on y is T = Σ_π Π_{ℓ∈π} β_ℓ (Wright 1934), computed along the order by T(x) = 1, T(v) = Σ_{(u,v)} T(u) β_{(u,v)}. Replacing numbers by intervals in the same recurrence gives an interval containing every value of T as the β_ℓ range over their intervals; it is the exact range when no link lies on two paths.

*Proof.* The recurrence expands to the sum over paths by distributivity. Interval arithmetic encloses the range of any expression evaluated in it. When no link lies on two paths, T is a sum of monomials in disjoint sets of variables; the range of a monomial over a box is the interval product of its factors' ranges, and the range of a sum of functions of disjoint variables is the sum of their ranges, which is what the recurrence computes, since paths meet only at y. ∎ When a link is shared the enclosure can be strictly wider: with a ∈ [1, 2] followed by two paths of products 1 and −1, T = a · 0 = 0, while the enclosure is [−1, 1]. The checks show both cases.

**Definition 2.** A link is *on every path* from x to y when the number of paths through it, P(x → u) · P(v → y), equals P(x → y). A link on every path that is not shown is a *gap on every path*: no other route can make up for it.

## 3. The Evidence for a Claim

A body of evidence for one claim is pooled by peira (random effects by REML; the fixed effect of a single study), with the prediction interval and Egger's test when k ≥ 3, and graded by a declared scheme. tekmerion reads the synthesis's own factors off it by declared rules: thresholds inside the confidence or prediction interval, I² above declared values, Egger's p below declared levels with at least a declared number of studies, and a magnitude passed on the side of benefit. It holds each at its factor's most, and grades. The grade becomes the link's g_ℓ and the pooled interval its [a_ℓ, b_ℓ].

*Caveat.* A grade is an ordinal judgement of certainty under an institution's rules, not a probability, and the domain never multiplies grades. Where a probability is wanted, the pooled interval is the quantity to use, and the decision (§5) uses it.

## 4. From There to Here

The argument (tekmerion's `will_it_work_here`) is an ASPIC+ theory with one defeasible rule from three premises. The domain supplies the third premise from the theory: the support factors of the links on the path chosen (by default the strongest), each stated present, absent or not known at the place. All present: the premise holds. One absent: its negation. Otherwise it is missing, and every factor not known is named.

**Proposition 4 (transport through strata).** If the effect varies only through a covariate Z (Z is S-admissible in a selection diagram) and the studies give stratum effects τ_z with variances v_z, the effect here is τ* = Σ_z p*_z τ_z for the place's distribution p* of Z, with variance Σ_z p*_z² v_z when the strata are estimated independently (peira's `transport`; Pearl and Bareinboim 2011).

## 5. The Decision

**Proposition 5 (a net value normal in the effect).** Let an option reach N units a year with an effect μ_e ± σ_e per unit (normal), worth v per unit of outcome, with benefits in years t ∈ T_b and costs c_t, discounted by factors D(t). Its net present value is V = A v N e − C, with A = Σ_{t∈T_b} D(t) and C = Σ_t D(t) c_t. So V ~ N(A v N μ_e − C, (A v N σ_e)²), and V = 0 at the worth v* = C/(A N μ_e).

*Proof.* V is affine in e, and an affine image of a normal variable is normal. ∎

**Proposition 6 (adopt, reject, or study first).** With V ~ N(μ, σ²): deciding now is worth max(μ, 0); perfect information about e is worth EVPI = σ L(|μ|/σ), L(z) = φ(z) − z(1 − Φ(z)). A study of n units with sampling variance s² per unit changes the expected value of V by a normal amount of standard deviation σ̃_n = |A v N| σ_e² / √(σ_e² + s²/n). Studying first and then deciding is worth δ (max(μ, 0) + σ̃_n L(|μ|/σ̃_n)) − c(n), where δ ∈ (0, 1] discounts the decision for the study's time and c(n) is paid now.

*Proof.* The posterior mean of e after the study is normal with variance σ_e⁴/(σ_e² + s²/n) by conjugacy (the preposterior variance), so the posterior expected net value is normal with mean μ and standard deviation σ̃_n. Deciding after it is worth E max(posterior mean, 0) = max(μ, 0) + σ̃_n L(|μ|/σ̃_n) (the same identity as for EVPI: tyche decision 0024). Discounting by δ and subtracting the cost paid now gives the worth. ∎

The study worth doing maximises this worth over n = 1, …, n_max, and every n is examined. It is done when it is worth more than max(μ, 0). The checks hold the domain's EVSI against tyche's preposterior analysis on a table of 1201 effects and 1201 signal bins, which knows nothing of normal laws (relative agreement 2.5 × 10⁻⁵ for n = 50).

**Proposition 7 (robustness in the worth of a unit).** μ(v) = A N μ_e v − C is affine in v; over v ∈ [v_lo, v_hi] the choice between adopting and rejecting is the same at every worth exactly when μ(v_lo) and μ(v_hi) have the same sign. Γ-maximin over the interval adopts exactly when min(μ(v_lo), μ(v_hi)) > 0.

*Proof.* An affine function on an interval attains its extremes at the ends, and has one sign throughout exactly when it has that sign at both. ∎

*Remark (what is not modelled).* The four options of Claxton et al. (2012) include adopting with research and adopting only in research. Their worths depend on whether research remains possible once adoption is under way and on what an adoption costs to reverse; these are a programme's to state, and are planned.

## 6. References

Those of the README.
