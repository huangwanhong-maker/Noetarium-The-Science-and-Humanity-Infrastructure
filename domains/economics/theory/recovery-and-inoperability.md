# Recovery and Inoperability: Indirect Losses through an Economy's Interdependencies

*Status: draft (ADR 0158). The module `economics::recovery`; its checks are `recovery/tests/check_inoperability.cpp`.*

## 1. The Concept

A disaster or a shock destroys capacity in some sectors (a flooded factory, a closed hotel) or removes demand for their output (visitors who do not come). The losses do not stop there: the sectors that buy from them and sell to them lose too, and the losses last until each recovers. The direct losses are the risk modules' (disaster risk's `vulnerability`, the visitor stock); the indirect losses through the interdependencies of production are this module's.

**Inoperability.** For a sector $i$ with planned output $\hat x_i$ and actual output $\tilde x_i$, the inoperability is $q_i = (\hat x_i - \tilde x_i)/\hat x_i$, the share not produced: 0 when the sector works as planned, 1 when it stops.

**Proposition 1 (the static model).** Let $x = A x + c$ hold for both the planned and the perturbed economy, with technical coefficients $a_{ij} = z_{ij}/\hat x_j$. Then $q = A^* q + c^*$, with $A^* = \hat X^{-1} A \hat X$ and $c^*_i = (\hat c_i - \tilde c_i)/\hat x_i$, so $q = (I - A^*)^{-1} c^*$.

*Proof.* Subtract: $\hat x - \tilde x = A(\hat x - \tilde x) + (\hat c - \tilde c)$. Multiply on the left by $\hat X^{-1}$ and write $\hat x - \tilde x = \hat X q$. ∎

The entry $a^*_{ij} = z_{ij}/\hat x_i$ is the share of $i$'s output that $j$ uses (Santos and Haimes 2004). $A^*$ is similar to $A$, so it has the same eigenvalues and $(I - A^*)^{-1}$ exists exactly when Leontief's inverse does.

## 2. The Dynamic Model

Lian and Haimes (2006) let each sector move toward the inoperability its suppliers' and buyers' state implies, at its own pace:
$$q(t+1) = q(t) + K\bigl[A^* q(t) + c^*(t) - q(t)\bigr], \qquad K = \operatorname{diag}(k_i).$$

**Resilience from a recovery time.** If sector $i$ alone (no other sector inoperable) is to recover to a share $r$ of its first inoperability in $T_i$ periods, then $q_i(T_i) = (1 - k_i(1 - a^*_{ii}))^{T_i} q_i(0) = r\,q_i(0)$, so
$$k_i = \frac{1 - r^{1/T_i}}{1 - a^*_{ii}},$$
the discrete form of Lian and Haimes's $k_i = \ln(q_i(0)/q_i(T_i)) / \bigl(T_i (1 - a^*_{ii})\bigr)$, which it approaches as $T_i$ grows.

**Proposition 2 (stability).** If every sector's final demand is positive, every row of $A^*$ sums to below one, and for $k_i \in (0, 1]$ the step $I - K(I - A^*)$ has $\infty$-norm below one. So the path converges, to $q = 0$ when $c^* = 0$ and to the static $q$ of Proposition 1 under a constant $c^*$.

*Proof.* Row $i$ of $A^*$ sums to $\sum_j z_{ij}/\hat x_i = 1 - \hat c_i/\hat x_i < 1$. Row $i$ of $I - K + K A^*$ has non-negative entries summing to $1 - k_i + k_i \sum_j a^*_{ij} < 1$. A contraction has a unique fixed point, which solves $q = A^* q + c^*$. ∎

**Proposition 3 (interdependency never lessens losses).** With non-negative first inoperabilities and perturbations, the path with $A^*$ dominates, sector by sector and period by period, the path with $A^* = 0$ and the same $K$.

*Proof.* By induction: if $q(t) \ge \bar q(t) \ge 0$, then $q(t+1) - \bar q(t+1) = (I - K)(q(t) - \bar q(t)) + K A^* q(t) \ge 0$. ∎

**Proposition 4 (the whole loss).** Over an unbounded horizon, with $q(t) \to 0$, the inoperabilities sum to
$$S = \sum_{t \ge 0} q(t) = (I - A^*)^{-1}\bigl(C + K^{-1} q(0)\bigr), \qquad C = \sum_{t \ge 0} c^*(t).$$

*Proof.* Sum the step over $t \ge 0$: $S - q(0) = S + K(A^* S + C - S)$, so $(I - A^*) S = C + K^{-1} q(0)$. ∎

So lost demand costs its Leontief total, $\hat X (I - A^*)^{-1} C$, whatever the resilience, which sets only when the loss falls; lost capacity costs more the slower each sector recovers, through $K^{-1}$. A path followed for a finite horizon holds part of $S$, and the share is reported.

## 3. What a Path Costs

With $\hat x$ a year's output and $p$ periods a year, a sector loses $\sum_t q_i(t)\,\hat x_i / p$ of output over the path. The loss without the interdependencies (each sector alone) is the direct loss; the difference is the indirect loss. Ranking sectors by mean inoperability and by output lost gives different orders (Lian and Haimes's observation): a small sector may be much inoperable and lose little, a large one a little inoperable and lose much.

**Readings.** The first inoperabilities (from capital destroyed over capital, or from closures), the lost demand by period, the recovery times and the share counted as recovered are declarations. Losses are of output; value added lost is output lost times each sector's value added over its output.

## 4. What Is Not Here

- Hallegatte's adaptive regional input–output model (ARIO): inventories, orders, overproduction capacity that adapts, rationing among clients, reconstruction demand and prices; the inoperability model is its linear core without them;
- reconstruction as demand on construction and manufacturing, and its financing;
- regional tables and interregional trade;
- the inoperability of households (lost income), which the well-being module of risk management treats.

## References

- Santos, J. R., and Haimes, Y. Y. (2004). Modeling the demand reduction input–output (I–O) inoperability due to terrorism of interconnected infrastructures. *Risk Analysis* 24(6), 1437–1451.
- Lian, C., and Haimes, Y. Y. (2006). Managing the risk of terrorism to interdependent infrastructure systems through the dynamic inoperability input–output model. *Systems Engineering* 9(3), 241–258.
- Haimes, Y. Y., Horowitz, B. M., Lambert, J. H., Santos, J. R., Lian, C., and Crowther, K. G. (2005). Inoperability input–output model for interdependent infrastructure sectors. I: Theory and methodology. *Journal of Infrastructure Systems* 11(2), 67–79.
- Hallegatte, S. (2008). An adaptive regional input–output model and its application to the assessment of the economic cost of Katrina. *Risk Analysis* 28(3), 779–799.
- Miller, R. E., and Blair, P. D. (2009). *Input–Output Analysis*, 2nd ed. Cambridge University Press. Ch. 2.
