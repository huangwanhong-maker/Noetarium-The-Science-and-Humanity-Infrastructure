# Material and Energy Accounts

*Status: draft (2026-11-13). The theory note of the industrial-ecology domain ([ADR 0106](../../../development/decisions/0106-industrial-ecology-founded.md)).*

## 1. Concepts

A society keeps itself by taking materials and energy from its environment, holding some of them in use as stocks (buildings, roads, machines, goods), and returning them as emissions and waste. Fischer-Kowalski (1998) called this its *social metabolism*; industrial ecology studies it with one principle: mass is conserved. Every account of this domain is a balance, and its results are statements of what the balance implies once some of its terms are measured and others declared.

The domain distinguishes four objects.

- **A stock** is what is in use at an instant, by cohort (the period it entered).
- **A flow** is what passes in a period between processes, or across the system's boundary.
- **A product system** is a set of processes linked by the products they exchange, scaled to deliver a functional unit (life cycle assessment).
- **An economy** is a set of sectors linked by monetary deliveries, extended by the physical pressures each sector exerts (input–output analysis).

The handbook that asked for this domain warns against collapsing distinctions (its §1.1). Three are kept here: resource presence is not access, environmental accounting is not ecological integrity, and a model's output is not a measurement. The second is why ecosystems' condition stays with the ecology domain.

## 2. Dynamic Stock Models

### 2.1 Lifetimes

**Definition 2.1.** A *lifetime law* is a survival function $S:[0,\infty)\to[0,1]$, non-increasing, the share of a cohort still in use at age $a$. A dynamic stock model reads it at whole ages $a = 0, 1, 2, \dots$; a law may differ by cohort, $S_c$.

The laws offered are those of the literature on goods in use: fixed, exponential, normal (not truncated, so that $S(0) = \Phi(\mu/\sigma) < 1$, as in ODYM), Weibull, lognormal, gamma, a life table, and the Kaplan–Meier estimate from observed lives with censoring (peira). None is a default; a law is declared.

### 2.2 The Inflow-Driven Model

**Definition 2.2.** Given inflows $I(c)$, $c = 0,\dots,T-1$, the stock of cohort $c$ at the end of period $t \ge c$ is $s(t,c) = I(c)\,S_c(t-c)$; its outflow is $o(c,c) = I(c) - s(c,c)$ and $o(t,c) = s(t-1,c) - s(t,c)$ for $t > c$. An initial stock $s_0(a)$ by age at the end of period $-1$ survives by the conditional law: $s_0(t,a) = s_0(a)\,S(t+1+a)/S(a)$.

**Proposition 2.3 (balance).** With $S(t) = \sum_{c\le t} s(t,c) + \sum_a s_0(t,a)$ and $O(t)$ the sum of the outflows, $S(t) - S(t-1) = I(t) - O(t)$ for every $t$.

*Proof.* For a cohort $c < t$, $s(t,c) - s(t-1,c) = -o(t,c)$; for $c = t$, $s(t,t) = I(t) - o(t,t)$; for the initial ages, $s_0(t,a) - s_0(t-1,a) = -o_0(t,a)$ by definition. Summing gives the identity. $\square$

**Proposition 2.4 (closed forms).** With a constant inflow $I$:

- under a fixed lifetime $L$, $S(t) = I\min(t+1, L)$, and the steady stock has mean age $(L-1)/2$;
- under the exponential law with mean $\tau$, $S(a) = q^a$ with $q = e^{-1/\tau}$, and $S(t) = I(1 - q^{t+1})/(1-q)$.

An exponential law forgets age: $S(t+1+a)/S(a) = q^{t+1}$, so an initial stock decays as $S(-1)\,q^{t+1}$ whatever its ages.

### 2.3 The Stock-Driven Model

**Proposition 2.5 (Müller 2006).** Given a path $S(t)$ and laws with $S_t(0) > 0$, there is exactly one inflow sequence whose inflow-driven stock is $S(t)$:
$$I(t) = \frac{S(t) - \sum_{c<t} I(c)\,S_c(t-c) - \sum_a s_0(t,a)}{S_t(0)}.$$

*Proof.* The stock at $t$ depends on $I(0),\dots,I(t)$, and on $I(t)$ with coefficient $S_t(0) \ne 0$. The system is therefore lower triangular with a non-zero diagonal, and forward substitution solves it uniquely. $\square$

The solution may be negative, when the stock falls faster than its cohorts leave. The domain then offers, and does by default, an *early retirement*:

- the inflow is set to zero;
- every existing cohort is scaled by $r = S(t)/R(t)$, where $R(t)$ is what would remain;
- each cohort then survives by its law from the scaled level.

The stock follows its path exactly. The excess $R(t) - S(t)$ is reported as retired early and is part of the outflow, so Proposition 2.3 still holds.

### 2.4 Materials

**Proposition 2.6.** Let $m_k(c)\in[\underline m_k(c), \overline m_k(c)]$ be cohort $c$'s mass of material $k$ per unit. If every $I(c) \ge 0$, the material stock $M_k(t) = \sum_c s(t,c)\,m_k(c)$ and its flows lie in the intervals obtained by putting the lower and the upper intensities in every term, and these bounds are attained.

*Proof.* Every $s(t,c)$ and $o(t,c)$ is non-negative (survival is non-increasing, and early retirement scales by $r \in [0,1]$). $M_k$ is therefore a non-negative combination of the intensities, monotone in each. The bounds are attained at the corners. $\square$

## 3. Material Flow Analysis

### 3.1 Balances and Transfer Coefficients

**Definition 3.1.** A system has processes $p$ and variables $j$: flows between processes, flows across the boundary, and stock changes. Its incidence $A$ has $A_{pj} = +1$ when $j$ enters $p$ and $-1$ when $j$ leaves $p$ or adds to $p$'s stock. The balances are $A x = 0$.

**Proposition 3.2 (transfer coefficients).** Let each process send fixed shares $K_{pq} \ge 0$ of its throughput to other processes and a share $a_p$ to its stock, with $\sum_q K_{pq} + a_p \le 1$. Suppose every process has a path of positive shares to one with $\sum_q K_{pq} + a_p < 1$. Then $T = u + K^{\mathsf T} T$ has the unique solution $T = (I - K^{\mathsf T})^{-1} u \ge 0$ for imports $u \ge 0$, and the system's imports equal its exports plus its accumulation.

*Proof.* $K$ is substochastic by rows and, under the hypothesis, every state is transient, so $\rho(K) < 1$ (Seneta 2006, Theorem 4.3). Hence $(I - K^{\mathsf T})^{-1} = \sum_n (K^{\mathsf T})^n \ge 0$. Summing the balances over processes, the internal flows cancel. $\square$

### 3.2 Reconciliation

**Definition 3.3.** Each variable is measured ($\tilde x_j$, $\sigma_j > 0$), exact, or unknown. Split $A = [A_M\ A_E\ A_U]$, and write $b = -A_E x_E$.

Let the rows of $P$ span $\ker A_U^{\mathsf T}$, and let $G = PA_M$, $c = Pb$. The balances that do not involve the unknowns are $G x_M = c$; their number of independent rows is the *redundancy* $\rho$.

**Proposition 3.4.** Under independent normal errors and balances that hold, the estimate
$$\hat x_M = \tilde x - VG^{\mathsf T}W^{-1}(G\tilde x - c), \qquad W = GVG^{\mathsf T},$$
is the minimum-variance unbiased estimate among the linear estimates satisfying $G\hat x_M = c$. Its covariance is $V - VG^{\mathsf T}W^{-1}GV$, and $\chi^2 = r^{\mathsf T}W^{-1}r \sim \chi^2_\rho$.

*Proof.* This is generalised least squares under linear constraints (Narasimhan and Jordache 2000, §3.2–3.4). The statistic is a quadratic form in the normal residual $r = G(\tilde x - x) \sim N(0, W)$. $\square$

**Proposition 3.5 (observability).** An unknown $x_{U,i}$ is determined by the balances if and only if $e_i \perp \ker A_U$.

*Proof.* The unknowns satisfy $A_U x_U = b - A_M \hat x_M$, which lies in the range of $A_U$ after reconciliation. The solution set is one particular solution plus $\ker A_U$, so its $i$-th coordinate is fixed exactly when every vector of $\ker A_U$ has a zero $i$-th coordinate. $\square$

The domain's checks hold the estimate to its closed form by hand, the mean of $\chi^2$ to $\rho$ over 3,000 simulated surveys, and the size of the test to 5%.

## 4. Life Cycle Assessment

**Definition 4.1 (Heijungs and Suh 2002, §2.3).** A product system is $(A, B)$: $A$ the products × processes technology matrix (outputs positive, inputs negative), $B$ the elementary flows × processes intervention matrix. For a demand $f$ and a characterisation $C$:
$$s = A^{-1}f, \quad g = Bs, \quad h = Cg.$$

A process with several products leaves $A$ rectangular. It is made square by *partition*, which divides its inputs and interventions by declared factors, or by *substitution*, which subtracts from it the process its co-product displaces, scaled to the amount displaced. ISO 14044 ranks substitution before partition. The domain implements both and chooses neither.

**Proposition 4.2 (elasticities; Heijungs 2010).** With $\lambda = CBA^{-1}$:
$$\frac{\partial h_k}{\partial a_{ij}} = -\lambda_{ki}\,s_j, \qquad \frac{\partial h_k}{\partial b_{lj}} = C_{kl}\,s_j.$$
For every process $j$, the elasticities of $h_k$ to the coefficients of column $j$ of $A$ and $B$ sum to zero.

*Proof.* Differentiate $AA^{-1} = I$, which gives $\partial A^{-1} = -A^{-1}(\partial A)A^{-1}$. For the sum, scale column $j$ of $A$ and $B$ by $\kappa$. Then $s_j$ scales by $1/\kappa$ and $h$ is unchanged. Differentiating at $\kappa = 1$ gives Euler's identity for this homogeneity of degree zero. $\square$

**Proposition 4.3 (structural paths).** Suppose product $i$ is made by process $i$ with $a_{ii} > 0$ and the other entries of $A$ are $\le 0$. Then $A = (I - Z)D$ with $D = \operatorname{diag}(a_{ii})$ and $Z_{ij} = -a_{ij}/a_{jj} \ge 0$, and
$$h_k = (CBD^{-1})_k\,(I-Z)^{-1}f = \sum_n (CBD^{-1})_k\,Z^n f,$$
a sum over walks in the graph of $Z$. calx's `leading_walks` finds every walk above a cut, with no omission, by the bound $v\,m_j$ on everything that extends a walk ending at $j$ (calx decision 0017). When the bound alone ends the search, what is left is less than the number of branches cut times the cut.

## 5. Environmentally Extended Input–Output Analysis

**Definition 5.1.** Take a table of $R$ regions × $n$ sectors with deliveries $Z$, final demand $Y$ (one column per region), and extension $(F, F_Y)$. From economics' Leontief model, $L = (I - A)^{-1}$. Define $S = F\,\mathrm{diag}(x)^{-1}$, $x^{(r)} = L\,Y_r$, and
$$P_k(\rho, r) = \sum_{j\in\rho} S_{kj}\,x^{(r)}_j + [\rho = r]\,F_Y(k, r).$$

**Proposition 5.2.** With production $= \sum_r P_k(\rho, r)$, consumption $= \sum_\rho P_k(\rho, r)$, exports embodied the off-diagonal row sums and imports embodied the off-diagonal column sums:

- production(r) − consumption(r) = exports(r) − imports(r) for every region;
- the world's production equals its consumption, $\sum_j F_{kj} + \sum_r F_Y(k, r)$.

*Proof.* The diagonal term $P_k(r,r)$ appears in both the row sum and the column sum of $r$, so the difference of the margins is the difference of the off-diagonal sums. For the world, $\sum_r x^{(r)} = LY\mathbf 1 = x$, so $\sum_{\rho,r} P_k(\rho, r) = \sum_j S_{kj}x_j + \sum_r F_Y(k, r) = \sum_j F_{kj} + \sum_r F_Y(k,r)$. $\square$

These are pymrio's regional accounts $D_{\mathrm{cba}}$, $D_{\mathrm{pba}}$, $D_{\mathrm{imp}}$ and $D_{\mathrm{exp}}$, which the domain's checks reproduce on pymrio's own test system to $10^{-10}$. With domestic extraction as the extension, consumption is the raw material consumption (Wiedmann et al. 2015).

## 6. Physical Supply–Use Tables

**Proposition 6.1.** If every product balances and every activity balances (SEEA-CF §3.71–3.79), the economy balances: natural inputs + imports = residuals + exports + additions to stock.

*Proof.* Sum the product balances and the activity balances. Supply and use appear once with each sign and cancel. $\square$

The converse fails, so the domain reports all three. A single wrong entry in the use table shows at its product and at its activity, and not at the economy.

## 7. The Material Stock of Buildings

A building's mass is its floor area times a material intensity in kg/m², which depends on its region, use, structure and period of construction (Heeren and Fishman 2019; Fishman et al. 2024). The matching rule is stated in `buildings.hpp`.

- The rows that match are grouped by their values of what is unknown of the building.
- Within each group, the most specific rows are taken.
- The intensity is the envelope of what is taken: the least lower bound, the greatest upper bound, and the mean of the central values.

**Proposition 7.1.** Let a building's attribute be unknown. Its interval contains the interval it would receive for each value of the attribute that the scheme admits (the most specific rows of each group).

*Proof.* The envelope is taken over the union of the groups, so every group's interval is contained in it. $\square$

This is the sense in which the domain never guesses: an unknown attribute widens the bounds over every value the scheme admits for it. Proposition 2.6 then carries the bounds of a city's buildings, by cohort, into the flows of its renewal.

## 8. References

- Baccini, P. and Brunner, P. H. (2012), *Metabolism of the Anthroposphere*, 2nd ed., MIT Press.
- Brunner, P. H. and Rechberger, H. (2016), *Handbook of Material Flow Analysis*, 2nd ed., CRC Press.
- Cencic, O. and Rechberger, H. (2008), Material flow analysis with software STAN, *J. Environ. Eng. Manag.* 18, 3–7.
- Crowe, C. M., Campos, Y. A. G. and Hrymak, A. (1983), Reconciliation of process flow rates by matrix projection, *AIChE J.* 29, 881–888.
- Defourny, J. and Thorbecke, E. (1984), Structural path analysis and multiplier decomposition, *Econ. J.* 94, 111–136.
- Eurostat (2018), *Economy-wide Material Flow Accounts Handbook*.
- Fischer-Kowalski, M. (1998), Society's metabolism, *J. Ind. Ecol.* 2, 61–78.
- Fishman, T. et al. (2024), RASMI, *Sci. Data* 11, 418.
- Heeren, N. and Fishman, T. (2019), A database seed for a community-driven material intensity research platform, *Sci. Data* 6, 23.
- Heijungs, R. (2010), Sensitivity coefficients for matrix-based LCA, *Int. J. LCA* 15, 511–520.
- Heijungs, R. and Suh, S. (2002), *The Computational Structure of Life Cycle Assessment*, Kluwer.
- Kitzes, J. (2013), An introduction to environmentally-extended input-output analysis, *Resources* 2, 489–503.
- Lenzen, M. (2002), A guide for compiling inventories in hybrid LCA, *J. Cleaner Prod.* 10, 545–572.
- Miller, R. E. and Blair, P. D. (2009), *Input–Output Analysis*, 2nd ed., Cambridge University Press.
- Müller, D. B. (2006), Stock dynamics for forecasting material flows, *Ecol. Econ.* 59, 142–156.
- Narasimhan, S. and Jordache, C. (2000), *Data Reconciliation and Gross Error Detection*, Gulf.
- Pauliuk, S. and Heeren, N. (2020), ODYM, *J. Ind. Ecol.* 24, 446–458.
- Peters, G. P. (2008), From production-based to consumption-based national emission inventories, *Ecol. Econ.* 65, 13–23.
- Peters, G. P. and Hertwich, E. G. (2006), Structural analysis of international trade, *Econ. Syst. Res.* 18, 155–181.
- Seneta, E. (2006), *Non-negative Matrices and Markov Chains*, Springer.
- Stadler, K. (2021), Pymrio, *J. Open Res. Softw.* 9, 8.
- United Nations et al. (2014), *System of Environmental-Economic Accounting 2012: Central Framework*.
- Wiedmann, T. O. et al. (2015), The material footprint of nations, *PNAS* 112, 6271–6276.
