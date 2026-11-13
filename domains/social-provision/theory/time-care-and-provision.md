# Time, Care and Provision: What Is Computed, and Why It Holds

*Status: draft (2026-11-13). The social-provision domain's first note ([ADR 0105](../../../development/decisions/0105-social-provision.md)), with what it uses of demography's `health`, economics' `labour` and sociology's `time` ([ADR 0104](../../../development/decisions/0104-sociology-founded-time-and-relations.md)). Activities: research, learning.*

People's lives are sustained by more than markets. Goods and support reach people through households, care relationships, public services, commons and voluntary associations as well as through exchange. This note states what the domain computes of that provision and proves what holds. Its objects are households, the care people need and give, and, from the domains beneath it, the time a day holds, the years lived in health and the flows of a labour market.

## 1. Concepts

### 1.1 Provision, Not Only Exchange

**Definition 1.1.** *Provision* is the set of ways a good or a form of support reaches a person. It may come through the household (unpaid work, pooling), the market (purchase), the state (public services, transfers) or the community (commons, voluntary help). The *care diamond* (Razavi 2007) is the account of the hours of care by these four.

The domain keeps the four apart. It computes what a declaration of each gives, and presumes no division among them. Markets are the markets domain's; this domain holds what reaches people besides.

### 1.2 Counted, Estimated, Presumed

As in every work of the infrastructure, what is counted (minutes in a survey, people by age), what is estimated (a prevalence from a sample) and what is presumed (hours of care a dependent person needs, a wage at which unpaid work is valued, a time-poverty line) are said apart. Every presumption is a declared parameter.

## 2. Households

**Definition 2.1.** An *equivalence scale* $\eta$ maps a household's composition to its number of equivalent adults. Equivalised income $y/\eta$ is given to each member. The weights form is $\eta = a_1 + a_2(A - 1) + cK$ for $A \ge 1$ adults and $K$ children; the power form is $\eta = (A + K)^\theta$.

*Remark.* $\theta = 0$ treats needs as those of one person and $\theta = 1$ gives income per head; the OECD-modified weights $(1, 0.5, 0.3)$ and the square root $\theta = \tfrac12$ are declared schemes, neither the domain's.

**Definition 2.2.** The *value of unpaid work* by the input method is $V = \sum_k h_k w_k$, with $h_k$ the hours a year of activity $k$ ($m_k \cdot 365/60$ for $m_k$ minutes a day) and $w_k$ a wage (UNECE 2017). The three methods take, for $w_k$:

- **generalist replacement:** one housekeeper's wage for every activity;
- **specialist replacement:** each activity's market wage;
- **opportunity cost:** the person's own wage.

**Definition 2.3.** *Time poverty* (after Bardasi and Wodon 2010). A person whose committed time $c$ (paid and unpaid work) exceeds a line $z$ is time-poor. The rate is the weighted share above $z$; the gap is the mean of $c - z$ among the time-poor. By default $z = 1.5\,\mathrm{med}(c)$, a declared multiple.

## 3. Care

### 3.1 Need

**Definition 3.1.** With $n_a$ people at age $a$ and a prevalence $\pi_a$ of a state taken to need care, the people in need are $D = \sum_a n_a \pi_a$. Their hours a week are $hD$ for a declared $h$. A scenario scales each $\pi_a$ by $(1 - c)^t$: compression for $c > 0$, expansion for $c < 0$ (Fries 1980; Gruenberg 1977).

### 3.2 What Made the Need Change

Write $n_a = N s_a$, with $N$ the size and $s$ the age structure ($\sum_a s_a = 1$). Then $D(N, s, \pi) = N \sum_a s_a \pi_a$. Between two years, three factors change.

**Proposition 3.2.** Let $v(S) = D(\text{the factors in } S \text{ at their later values, the others earlier}) - D(\text{all earlier})$, a game on the players $\{N, s, \pi\}$. Its Shapley values $\varphi_N, \varphi_s, \varphi_\pi$:

1. sum to the change $D_1 - D_0$;
2. give $\varphi_s = \varphi_\pi = 0$ when only the size changes;
3. are negated when the two years are exchanged.

*Proof.*
- (1) is the efficiency of the Shapley value (Shapley 1953): $\sum_i \varphi_i = v(\{N, s, \pi\}) = D_1 - D_0$.
- (2) If only $N$ changes, $s$ and $\pi$ are null players, since adding either to any coalition changes nothing. A null player's value is zero.
- (3) Exchanging the years gives the game $v'(S) = D(\text{the factors in } S \text{ earlier, the others later}) - D_1 = v(\bar S) - v(\text{all})$, with $\bar S$ the complement. This is $-v^*(S)$, for $v^*(S) = v(\text{all}) - v(\bar S)$ the dual game. The Shapley value is self-dual, $\varphi(v^*) = \varphi(v)$ (each player's marginal contributions in $v^*$ are those in $v$ along the reversed order), so $\varphi(v') = -\varphi(v)$. ∎

This is Das Gupta's (1993) decomposition made order-free: no residual, and no order of replacing factors privileged. `check_social_provision_care` holds (1)–(3).

### 3.3 Supply, Gap and the Diamond

Informal supply is $\sum_g n_g m_g \cdot 7/60$ hours a week, from $n_g$ carers giving $m_g$ minutes a day (time use). Formal supply is the workforce times its hours. The gap is need less supply, and the share met is $\min(1, \text{supply}/\text{need})$. The diamond reports the supply's shares by giver.

### 3.4 Support Ratios

The old-age dependency ratio is $100 \cdot n_{65+}/n_{15\text{–}64}$. The support ratio of the oldest old is $n_{50\text{–}64}/n_{85+}$, the potential carers per very old person (United Nations, *World Population Ageing*).

## 4. Years Lived in Health (demography's `health`)

**Definition 4.1 (Sullivan 1971).** $HLE_x = l_x^{-1} \sum_{i \ge x} (1 - \pi_i) L_i$. Its sampling variance under prevalence sampling alone is $\sum_{i \ge x} (L_i / l_x)^2 \pi_i (1 - \pi_i)/n_i$ (Jagger et al. 2014).

**Proposition 4.2 (the life course and the trapezoid).** In an age-classified absorbing Markov chain with steps of $n$ years and deaths spread evenly within a step, the expected years in state $s$ from $(x, r)$ are $Y_s(x, r) = n(\sum_y N_{(y,s),(x,r)} - \tfrac12[s = r])$, with $N = (I - U)^{-1}$.

*Proof.* $N_{(y,s),(x,r)} = \sum_{t \ge 0} (U^t)_{(y,s),(x,r)}$ is the expected number of step-starts spent at $(y, s)$. Let $H_t$ be the probability of being in $s$ at the start of step $t$; then $\sum_y N_{(y,s),(x,r)} = \sum_t H_t$. The trapezoidal person-years in $s$ are $\sum_t n(H_t + H_{t+1})/2 = n(\sum_t H_t - H_0/2)$, since $H_t \to 0$. Here $H_0 = [s = r]$. ∎

**Corollary 4.3.** With one state, $Y = n(\sum_y l_y/l_x - \tfrac12) = e_x$ for the life table with $a = n/2$. In a stationary population, Sullivan's $HLE$ computed with the chain's own trapezoidal prevalence equals $Y_h$: both are $n\sum_t (H_t + H_{t+1})/2$. `check_demography_health` holds both exactly.

**Validation.** Sullivan's method applied to Eurostat's life tables and EU-SILC prevalence in ten-year bands reproduces Eurostat's published healthy life years at 65 to a median absolute difference of 0.10 years over 630 country-sex-years (2018–2024). At birth the result depends on the prevalence assumed below 16, where the survey asks no one: about $+0.4$ years with none assumed, $-0.25$ with half the first band's, and $-0.9$ with the first band's. The assumption is declared in every answer.

## 5. A Labour Market's Flows (economics' `labour`)

A row-stochastic $P$ over employment, unemployment and inactivity has a stationary $\pi = \pi P$ (calx's Perron vector of $P^\top$), unique when $\lambda = 1$ is simple. A stay in $i$ is geometric with mean $1/(1 - P_{ii})$. Deviations from $\pi$ decay at least as fast as $|\lambda_2|^t$, and the half-life is $\ln 2/(-\ln|\lambda_2|)$. The change of the steady unemployment rate between two periods is split among the six flows by the Shapley value of Proposition 3.2's form, each row's staying probability taking up the rest. It is exact and antisymmetric for the same reasons.

## 6. What Comes Next

- **Public services and their reach:** the urban-science domain's access to services, composed through the API.
- **Commons:** common-pool resources as games of provision (nomos's games), and Ostrom's principles as declared rules.
- **Care as a dynamic system:** dependency entered and left by age, with care given and received over a life course. This is the multistate chain of §4 joined to the time of carers, a composition of systems for the infrastructure's dynamics foundation, kinesis.
- **Non-market provision as accounts:** a satellite account of household production after the UNECE guide.

## 7. References

- Bardasi, E. and Wodon, Q. (2010), Working long hours and having no choice: time poverty in Guinea, *Feminist Economics* 16, 45–78.
- Das Gupta, P. (1993), *Standardization and Decomposition of Rates: A User's Manual*, U.S. Census Bureau.
- Fries, J. F. (1980), Aging, natural death, and the compression of morbidity, *New England Journal of Medicine* 303, 130–135.
- Gruenberg, E. M. (1977), The failures of success, *Milbank Memorial Fund Quarterly* 55, 3–24.
- Jagger, C. et al. (2014), *Health Expectancy Calculation by the Sullivan Method: A Practical Guide*, EHEMU Technical Report.
- Razavi, S. (2007), *The Political and Social Economy of Care in a Development Context*, UNRISD Programme Paper 3.
- Shapley, L. S. (1953), A value for n-person games, *Contributions to the Theory of Games II*, 307–317.
- Sullivan, D. F. (1971), A single index of mortality and morbidity, *HSMHA Health Reports* 86, 347–354.
- UNECE (2017), *Guide on Valuing Unpaid Household Service Work*, United Nations.
