# Well-Being Losses: Consumption over a Recovery, Valued by Utility

*Status: draft (ADR 0150). The module `risk_management::wellbeing`; its checks are `wellbeing/tests/check_wellbeing.cpp`.*

## 1. The Concept: Losses Measured in What People Can Consume

An asset loss counts a dollar alike for everyone. A household that loses its house loses the income the house earned, rents or rebuilds from what it would have consumed, and draws on whatever savings and help it has. A dollar of loss weighs more where consumption is low. *Unbreakable* (Hallegatte, Vogt-Schilb, Bangalore and Rozenberg 2017) measures losses in well-being for this reason.

**Definition.** Socioeconomic resilience is the ratio of asset losses to well-being losses, the latter expressed as the fall in national consumption that would cost as much well-being (Hallegatte, Bangalore and Vogt-Schilb 2016, "P1", p. 2).

**The point for action.** Asset losses and well-being losses rank the same places, and the same measures, differently. A flood in a wealthy district has large asset losses and small well-being losses; one in an informal settlement has the reverse. The module computes both and leaves the choice of measure to the caller. This is the survey's advice in *Evidence, methods and actions for steering cities*: parallel evaluation spaces, never one index.

**Sources.** The equations are written from P1 and from Walsh and Hallegatte (2019, "P2", the working paper of their 2020 article in *Economics of Disasters and Climate Change*), both CC BY 3.0 IGO. The models' published code is GPL-3 or unlicensed and was not used.

## 2. The Household Model

### 2.1 Income and Effective Capital

Households $h$ stand for $\omega_h$ persons each. Each person has income $i_h$, taken equal to consumption $c_h$, of which $i^{sp}_h$ comes from transfers.

**Financing.** Transfers are paid by a flat tax at the rate (P2 Eq. 3)
$$\delta_{sp} = \frac{\sum_h \omega_h i^{sp}_h}{\sum_h \omega_h i_h}.$$

**Effective capital.** Income not from transfers is earned by capital at its average productivity $\Pi$ (P2 Eqs 4–6):
$$k_h = \frac{i_h - i^{sp}_h}{(1 - \delta_{sp})\Pi}, \qquad K = \sum_h \omega_h k_h.$$

**Lemma 1.** $\sum_h \omega_h i_h = \Pi K$.

*Proof.* Sum $i_h = (1-\delta_{sp})\Pi k_h + i^{sp}_h$ over $h$ with weights $\omega_h$, and use $\sum_h \omega_h i^{sp}_h = \delta_{sp}\sum_h \omega_h i_h$. ∎

### 2.2 Losses

**Asset loss.** In an event a share $f_h$ of household $h$ is affected and loses $\Delta k_h = v_h k_h$. The vulnerability $v_h$ is reduced by a share $\pi$ for households warned in time. The nation loses $L = \sum_h \omega_h f_h \Delta k_h$.

**Income loss.** At the start of recovery:

- the affected lose $(1-\delta_{sp})\Pi\Delta k_h$ of their own income (P2 Eq. 12);
- everyone loses $(L/K)\, i^{sp}_h$ of transfers, which fall with the nation's assets (P2 Eq. 14).

**Proposition 1 (P2 Eq. 16).** The income lost at the start is
$$\sum_h \omega_h\big[f_h(1-\delta_{sp})\Pi\Delta k_h + (L/K)\, i^{sp}_h\big] = \Pi L.$$

*Proof.*

1. The first term sums to $(1-\delta_{sp})\Pi L$.
2. The second term is $(L/K)\,\delta_{sp}\sum_h \omega_h i_h$.
3. By Lemma 1 the second term equals $\delta_{sp}\Pi L$.
4. The two terms add to $\Pi L$. ∎

### 2.3 Reconstruction

**Rate.** The rate is $\lambda = \ln 20 / N$, with 95% rebuilt in $N$ years (P1 p. 5; P2 Eq. 18). The private share $s_h$ of an affected household's loss is rebuilt from its consumption at rate $\lambda_h$, which costs $\lambda_h s_h \Delta k_h e^{-\lambda_h t}$ (P2 Eq. 20). The consumption loss is
$$\Delta c_h(t) = \big[(1-\delta_{sp})\Pi + \lambda_h s_h\big]\Delta k_h\, e^{-\lambda_h t} + (L/K)\, i^{sp}_h\, e^{-\lambda t}.$$

**Public assets.** The share $1-s$ is rebuilt by the government and paid later by a flat tax $i_h L_{pub}/K$ (P2 Eq. 24).

**Each household's own best pace.** A household may choose $\lambda_h$ to maximise its own well-being over the horizon, omitting transfers and taxes (P2 Eq. 22):
$$W(\lambda) = \int_0^H u\big(\Pi - (\Pi+\lambda)v e^{-\lambda t}\big)\, e^{-\rho t}\, dt.$$
Its stationary condition is P2's Eq. 23:
$$\int_0^H u'\big(\Pi - (\Pi+\lambda)v e^{-\lambda t}\big)\, v e^{-\lambda t}\big(t(\Pi+\lambda) - 1\big)\, e^{-\rho t}\, dt = 0,$$
since $\partial_\lambda[\Pi - (\Pi+\lambda)v e^{-\lambda t}] = v e^{-\lambda t}(t(\Pi+\lambda) - 1)$. Consumption at the start stays positive only while $(\Pi+\lambda)v < \Pi$. The module maximises by Brent's method within that bound, and no slower than a declared slowest pace. The check confirms that Eq. 23 vanishes at the rate returned.

**A floor.** Where consumption would fall below a declared floor, reconstruction slows so that consumption starts at the floor (P2 pp. 53–55). Where income loss alone breaches the floor, the household rebuilds at the slowest pace.

**A hard minimum.** Consumption is never modelled below a share of the household's own (5% by default). Where it would fall below, the loss beyond is reported, since the well-being loss is then understated. No buffer is spent against that part.

### 2.4 Savings and Support

**The cap.** Savings and support received are spent on the deepest part of the loss (P2 Eqs 25–27). The loss is capped at a level $\gamma$ until $\hat t$, when the buffer $B$ is exhausted:
$$\int_0^{\hat t}\big(\Delta c(t) - \gamma\big)\, dt = B, \qquad \Delta c(\hat t) = \gamma.$$

**Proposition 2.** For a single exponential $\Delta c(t) = a e^{-\lambda t}$ with $B < a/\lambda$, write $\beta = e^{-\lambda\hat t}$. Then $\gamma = a\beta$ and
$$a(1-\beta) + \gamma\ln\beta - \lambda B = 0.$$
This is P2's Eq. 27 read with $\gamma$ the level of the loss. The root in $\beta$ is unique.

*Proof.*

1. **The identity.** $\int_0^{\hat t} a e^{-\lambda t}\,dt = (a/\lambda)(1-\beta)$ and $\gamma\hat t = -(a\beta/\lambda)\ln\beta$. Multiply the defining equation by $\lambda$.
2. **Uniqueness.** The left side, as a function of $\beta$, has derivative $a\ln\beta < 0$ on $(0, 1)$. It equals $-\lambda B \le 0$ at $\beta = 1$ and $a - \lambda B > 0$ as $\beta \to 0$. ∎

*Remark.* P2's Eq. 26 prints $\gamma$ as a level of consumption, which contradicts Eqs 25 and 27; the reading above follows the latter.

**Order and rebuilding.** Support is spent before savings. Savings spent are rebuilt later, and buffers do not speed reconstruction (P2 pp. 57–58).

**Support.** Each person helped receives a payout: a declared share $\theta$ of the mean asset loss of a reference group's affected members. Among the affected a declared share is excluded; among the unaffected a declared share is included in error (P1 Eq. 16 gives the errors' form). Support is financed by a flat tax on income paid later, so that its cost equals what it pays.

### 2.5 Well-Being and Its Equivalent

**The household's loss.** With $u(c) = c^{1-\eta}/(1-\eta)$ ($\log c$ at $\eta = 1$),
$$\Delta W_h = \int_0^H \big[u(c_h) - u(c_h - \Delta c_h(t))\big] e^{-\rho t}\, dt + c_h^{-\eta}\,\big(\text{later taxes} + \text{savings rebuilt}\big).$$
The later term is undiscounted, as P2's Eq. 31 prints it.

**The equivalent.** The equivalent consumption loss is $\Delta C_{eq} = \Delta W / \bar c^{-\eta}$, where $\bar c$ is national mean consumption (P2 Eq. 34). Resilience is $L/\Delta C_{eq}$.

**Proposition 3 (P1's Γ).** Take a single household at the mean, with no transfers, all affected, every loss private, and an infinite horizon. As the loss tends to zero,
$$\Delta C_{eq}/L \to \Gamma = \frac{\Pi + \lambda}{\rho + \lambda},$$
so that resilience tends to $1/\Gamma$.

*Proof.*

1. To first order, $\Delta W = c^{-\eta}\int_0^\infty (\Pi+\lambda)\Delta k\, e^{-(\lambda+\rho)t}\,dt = c^{-\eta}\,\Gamma\Delta k$.
2. With $c = \bar c$, $\Delta C_{eq} = \Gamma\Delta k = \Gamma L$. ∎

**Closed forms for the checks.** Let $a = xc$ with $x < 1$. Expanding the logarithm, and $1/(1 - xe^{-\lambda t})$, gives:

- at $\eta = 1$: $\int_0^\infty e^{-\rho t}\big[-\ln(1 - x e^{-\lambda t})\big]dt = \sum_{n\ge1} \dfrac{x^n}{n(\rho + n\lambda)}$;
- at $\eta = 2$: $\int_0^\infty e^{-\rho t}\left[\dfrac{1}{c - a e^{-\lambda t}} - \dfrac1c\right]dt = \dfrac1c\sum_{n\ge1}\dfrac{x^n}{\rho + n\lambda}$.

The module integrates by adaptive Gauss–Kronrod, in pieces cut where the path has kinks, and the checks hold both series to $10^{-9}$.

### 2.6 Over Return Periods

Events are maps of exposure at return periods $r_1 < \dots < r_m$. Expectations take the step weights $p_j = 1/r_j - 1/r_{j+1}$ and $p_m = 1/r_m$ (P1 p. 13, its code's rule): each event stands for the frequencies down to the next. Resilience is the ratio of the expectations.

The disaster-risk domain's trapezoids (`vulnerability`) are a different reading of the same curve. Neither reading is the curve itself.

## 3. Measures

The API compares measures by changing the declarations and computing everything again. The measures are:

- faster reconstruction;
- stronger homes for a group;
- early warning;
- savings;
- protection that removes a share of a group's exposure;
- support scaled up.

Each measure is reported with the asset losses and the well-being losses it avoids, and with its rank by each.

## 4. Readings Declared

The following are declarations, the caller's to change:

- the elasticity $\eta$ (1.5 in both papers);
- the discount rate $\rho$ (0.06 in the papers' code; the texts state none);
- the productivity of capital $\Pi$ (a country's own: output over capital in national accounts);
- $N$, the horizon (10 years in P2), the floor and the hard minimum;
- the step rule over return periods;
- support spent before savings;
- the undiscounted later term.

The country model of P1 (Eqs 8–11) values losses as a change in the present value of wealth, $w(c/\rho - \Delta\tilde C)$. It does not give the same numbers as the household model (P2 fn. 6), and it is not built.

## 5. What Is Not Here

- vulnerabilities drawn at random within their classes (P2 Table 7);
- exposure calibrated to modelled asset losses;
- the country model;
- support's targeting from scorecards of delivery (P1's $q_s$, $q_f$);
- a dynamic labour market;
- recovery observed in panel surveys.

## References

- Hallegatte, S., Bangalore, M. and Vogt-Schilb, A. (2016). Socioeconomic resilience: multi-hazard estimates in 117 countries. World Bank Policy Research Working Paper 7886. CC BY 3.0 IGO.
- Hallegatte, S., Vogt-Schilb, A., Bangalore, M. and Rozenberg, J. (2017). *Unbreakable: Building the Resilience of the Poor in the Face of Natural Disasters.* World Bank. CC BY 3.0 IGO.
- Walsh, B. and Hallegatte, S. (2019). Measuring natural risks in the Philippines: socioeconomic resilience and wellbeing losses. World Bank Policy Research Working Paper 8723. CC BY 3.0 IGO. Published as *Economics of Disasters and Climate Change* 4, 249–293 (2020).
- Markhvida, M., Walsh, B., Hallegatte, S. and Baker, J. (2020). Quantification of disaster impacts through household well-being losses. *Nature Sustainability* 3, 538–547.
