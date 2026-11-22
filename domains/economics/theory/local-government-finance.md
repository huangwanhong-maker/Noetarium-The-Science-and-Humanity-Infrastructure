# The Finances of a Local Government

*Status: draft (ADR 0145). The theory note of the economics domain's `fiscal`.*

## 1. Accounts, Categories and Ratios

**Definition 1 (chart).** A chart is a map χ from the lines of a government's accounts to declared categories. A year's totals are T_y(k) = Σ_{ℓ : χ(ℓ) = k} a_y(ℓ). A line outside the domain of χ is refused.

**Definition 2 (ratio).** A ratio is a pair of signed sums of categories, ρ = (Σ_k α_k T(k)) / (Σ_k β_k T(k)). It is undefined when its denominator is zero.

**Definition 3 (scheme of thresholds).** A scheme states, for a ratio, the side that is sound (at least, or at most) and the levels of warning and of intervention. A year's value is in intervention when it lies beyond the intervention level away from the sound side, in warning when beyond the warning level, and sound otherwise.

*Caveat.* "Debt service coverage" names three different quantities in the sources (the survey *Learning finance scorecards and value capture*, Part II):

- a cash-flow coverage in the IMF's usage;
- a liquidity coverage at S&P;
- a synthetic annuity coverage at Fitch.

The domain therefore states each ratio by its terms and never by a name alone. No threshold in the code is any jurisdiction's.

## 2. The Years Ahead

Each category grows at a declared rate: T_{y₀+k}(c) = T_{y₀}(c)(1 + γ_c)^k. The operating surplus is a declared signed sum OS_t.

An annuity loan of principal P at rate r over n years pays a = P r / (1 − (1 + r)^{−n}) a year, its interest on the balance outstanding and the rest as principal. A bullet loan pays interest only, and the principal in its last year. The debt service DS_t is the sum over loans of their interest and principal due in year t, and the coverage is OS_t / DS_t.

## 3. Debt Capacity

Let a new annuity of principal B at rate r over n years begin repayment in year s, and let a = r / (1 − (1 + r)^{−n}) be the payment per unit borrowed, so that the new loan asks aB in each year t ∈ [s, s + n).

**Proposition 1.** Let θ > 0 be a floor of coverage, and suppose the projection covers every repayment year. Define

  B* = min_{t ∈ [s, s+n)} (OS_t / θ − DS_t) / a.

If B* > 0, then for every B ≤ B* the coverage OS_t / (DS_t + aB) is at least θ in every repayment year, and at B = B* it equals θ in a year attaining the minimum. If B* ≤ 0, no positive loan keeps the floor in every repayment year.

*Proof.* In each repayment year t, OS_t / (DS_t + aB) ≥ θ if and only if B ≤ (OS_t / θ − DS_t) / a. This step needs OS_t > 0: if OS_t ≤ 0 the right-hand side is negative, B* < 0, and no positive B qualifies, which is the second case. The condition holds in every repayment year if and only if B is at most the minimum of the right-hand sides, B*, with equality in the year that attains it. ∎

A declared ceiling κ on the stock of debt relative to a year's revenue bounds B further. Let D⁰_s be the existing stock after year s, and let λ_s be the share of a unit loan outstanding after its first repayment year. The bound is B ≤ (κ Rev_s − D⁰_s) / λ_s. The capacity is the lesser of the two bounds, and the domain reports which one binds and in which year. The survey's Part II states the same solver independently, with any statutory caps taken as further constraints.

## 4. Debt Dynamics

Take debt d as a share of a base (GDP or revenue) that grows at g, at an effective rate r on last year's debt, with a primary balance pb as a share of the same base. Then

  d_{t+1} = d_t (1 + r_{t+1}) / (1 + g_{t+1}) − pb_{t+1}.

The primary balance that keeps d constant is pb* = d (r − g) / (1 + g). On a record, the stock-flow adjustment is the residual sfa_t = d_t − [d_{t−1}(1 + r_t)/(1 + g_t) − pb_t].

**The fan.** Take the record's joint deviations (r_t − r̄, g_t − ḡ, pb_t − p̄b). Blocks of b consecutive years are drawn with replacement (Künsch's moving-block bootstrap) and added to a baseline. The years ahead are then given as quantiles of the paths (Hyndman and Fan's type 7), with the share of paths above a declared threshold.

*Caveat.* The fan keeps the record's joint variation and short serial dependence within blocks. It is neither a forecast nor a structural model, and a baseline of recent averages is a declaration.

## 5. Capacity and Effort

Under the representative tax system, a base k has representative rate t̄_k = Σ_i R_{ik} / Σ_i B_{ik}. Government i's capacity is C_i = Σ_k t̄_k B_{ik}, and its effort is E_i = Σ_k R_{ik} / C_i.

**Proposition 2.** Σ_i C_i = Σ_i Σ_k R_{ik}.

*Proof.* Σ_i C_i = Σ_k t̄_k Σ_i B_{ik} = Σ_k Σ_i R_{ik}. ∎

## 6. A Project's Financing Plan

*Added by ADR 0152; the header `fiscal/project.hpp`, its checks `fiscal/tests/check_project.cpp`.*

**The plan.** A project's capital cost $c_t$ is met in year $t$ from the funds at hand. These are:

- the year's capital receipts $q_t$ (land value captured, the sale of reserve land);
- its grants $g_t$;
- the reserve $r_{t-1}$ carried from the year before at a declared rate $i$.

So the funds at hand are $F_t = q_t + g_t + (1+i)\,r_{t-1}$. The cost paid from them is $\min(c_t, F_t)$, and the rest, $b_t = \max(0, c_t - F_t)$, is borrowed as an annuity drawn in $t$ and repaid over $n$ years from $t+1$. The new loans' service in year $t$ is $D'_t$.

**Pledging.** If receipts are pledged, what is left, $\ell_t = \max(0, F_t - c_t)$, pays $p_t = \min(\ell_t, D'_t)$ of that service, and the reserve keeps $r_t = \ell_t - p_t$. Otherwise $p_t = 0$ and $r_t = \ell_t$.

**Coverage.** With $OS_t$ the operating surplus and $D_t$ the existing debt service, the coverage is
$$\frac{OS_t}{D_t + D'_t - p_t}.$$
Capital receipts never enter $OS_t$, since public accounts keep capital apart from the operating budget that services debt.

**Proposition 1.** Scale every $c_t$ by $s \ge 0$, holding receipts and grants. As $s$ rises:

- $b_t$, $D'_t$ and the net service $D_t + D'_t - p_t$ do not fall;
- $r_t$ does not rise.

Hence the least coverage over the years does not rise.

*Proof.* By induction on $t$. Suppose $r_{t-1}$ does not rise with $s$.

1. **Funds.** Then $F_t$ does not rise.
2. **Borrowing.** $b_t = \max(0, s c_t - F_t)$ does not fall, so neither does $D'_t$, a sum of payments on loans none of which fall.
3. **Leftover.** $\ell_t = \max(0, F_t - s c_t)$ does not rise.
4. **Net service.** It is $D_t + \max(0, D'_t - \ell_t)$, which does not fall.
5. **Reserve.** $r_t = \max(0, \ell_t - D'_t)$ does not rise, which closes the induction. ∎

**The largest scale.** By Proposition 1, the scales whose least coverage stays at or above a floor $\theta$ form an interval $[0, s^*]$, and $s^*$ is found by bisection. Two cases are said:

- the floor fails even at $s = 0$;
- the floor holds at the largest scale tried.

Comparing $s^*$ with the receipts and without them says how much more of a project a city can afford because of what it captures. On the example's invented tram line, with the leftover receipts pledged, that is 2.17 times the line against 1.85 without. Unpledged, the receipts help little, since most arrive after the works.

## 7. References

- Farvacque-Vitkovic, C. and Kopanyi, M., eds. (2014), *Municipal Finances: A Handbook for Local Governments*, World Bank.
- Advisory Commission on Intergovernmental Relations (1962), *Measures of State and Local Fiscal Capacity and Tax Effort*.
- Künsch, H. R. (1989), The jackknife and the bootstrap for general stationary observations, *Annals of Statistics* 17, 1217–1241.
- Hyndman, R. J. and Fan, Y. (1996), Sample quantiles in statistical packages, *The American Statistician* 50, 361–365.
- The survey *Learning finance scorecards and value capture* (`research/literature/reports/`), Part II.
