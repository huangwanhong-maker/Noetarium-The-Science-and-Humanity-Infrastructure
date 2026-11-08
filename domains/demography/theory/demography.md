# The Theory of the Demography Domain: Life Tables, Reproduction and the Stable Population

*Status: draft (2026-11-08). The results the domain's code relies on, with their hypotheses and proofs or citations. Each section names its module. Its general methods (the Lexis diagram, rates, standardisation) are peira's, in its theory note.*

## 1. The Life Table

*Module `lifetable`.* A population's mortality is read as a cohort would live it.

**Definition 1.1.** Let ages be cut into groups $[x, x + n_x)$, the last open. Let $m_x$ be the death rate in each (deaths per person-year), and $a_x$ the mean years lived in the group by those who die in it. From a radix $l_0$:
$$q_x = \frac{n_x m_x}{1 + (n_x - a_x) m_x}, \qquad d_x = l_x q_x, \qquad l_{x + n} = l_x - d_x, \qquad L_x = n_x l_{x+n} + a_x d_x,$$
with $T_x = \sum_{y \ge x} L_y$ and $e_x = T_x / l_x$. In the open group, $q = 1$ and $L = l / m$.

**Proposition 1.2 (the conversion of rates).** If $m_x = d_x / L_x$ and $L_x = n_x l_{x+n} + a_x d_x$, then $q_x$ is as given.

*Proof.* $L_x = n_x (l_x - d_x) + a_x d_x$, so $m_x = d_x / (n_x l_x - (n_x - a_x) d_x)$. Divide numerator and denominator by $l_x$ and solve for $q_x = d_x / l_x$. ∎

**Proposition 1.3 (the exponential law reproduced).** If the hazard is a constant $\mu$ within a group and $a = 1/\mu - n e^{-\mu n}/(1 - e^{-\mu n})$, then $q = 1 - e^{-\mu n}$. A table of constant $\mu$ at every age, with this $a$ and an open last group, has $e_x = 1/\mu$ at every age.

*Proof.* Under a constant hazard, $l_{x+n} = l_x e^{-\mu n}$ and $L_x = \int_0^n l_x e^{-\mu t} dt = l_x (1 - e^{-\mu n})/\mu$, so $d_x / L_x = \mu = m_x$. Then $a = (L_x - n l_{x+n}) / d_x$ is the stated value. Each group's $L$ is that of the exponential law, and the open group's $l/\mu$ is its tail, so $T_x = l_x / \mu$. ∎ *Checked:* $e_x = 50$ for $\mu = 0.02$ at every age, to $3 \times 10^{-14}$.

**Rates from a record.** $m_x$ is read from peira's Lexis diagram as deaths over person-years in a period: the maximum-likelihood estimate of a hazard constant within each cell (Breslow and Day 1987, ch. 2). *Checked:* simulated lives with a hazard of 0.03 give it back within $2.1$ standard errors in every age group.

## 2. Decomposing a Difference in Life Expectancy

**Proposition 2.1 (Arriaga 1984).** For two tables 1 and 2 of the same groups, with $l_0^1$ the first's radix, let
$$\Delta_x = \frac{l^1_x}{l^1_0}\Big(\frac{L^2_x}{l^2_x} - \frac{L^1_x}{l^1_x}\Big) + \frac{T^2_{x+n}}{l^1_0}\Big(\frac{l^1_x}{l^2_x} - \frac{l^1_{x+n}}{l^2_{x+n}}\Big)$$
for a closed group, and $\Delta_\omega = \frac{l^1_\omega}{l^1_0}\big(\frac{T^2_\omega}{l^2_\omega} - \frac{T^1_\omega}{l^1_\omega}\big)$ for the open one. Then $\sum_x \Delta_x = e^2_0 - e^1_0$.

*Proof.* $\sum_x L^1_x / l^1_0 = e^1_0$. The remaining terms are $\sum_x \frac{l^1_x}{l^2_x}(L^2_x + T^2_{x+n}) / l^1_0 - \sum_x \frac{l^1_{x+n}}{l^2_{x+n}} T^2_{x+n} / l^1_0$. Since $L^2_x + T^2_{x+n} = T^2_x$, this telescopes to $\frac{l^1_0}{l^2_0} T^2_0 / l^1_0 = e^2_0$. ∎ *Checked:* to $8 \times 10^{-15}$.

## 3. Reproduction

*Module `fertility`.* Let $f_x$ be the age-specific fertility rates (births per woman-year) in groups of width $n$, and $s$ the share of births that are girls. Then:
- $\mathrm{TFR} = n \sum f_x$;
- $\mathrm{GRR} = s\,\mathrm{TFR}$;
- $\mathrm{NRR} = s \sum f_x L_x / l_0$, the daughters a newborn girl will bear at these rates of birth and death.

When no woman dies before the end of childbearing, $L_x = n l_0$ and $\mathrm{NRR} = \mathrm{GRR}$ (checked).

## 4. Projection and the Stable Population

*Module `projection`.*

**Definition 4.1 (Leslie 1945; Keyfitz 1964).** For $k$ closed groups of width $n$, the Leslie matrix has survival ratios $s_i = L_{i+1}/L_i$ below the diagonal and first row
$$F_i = s \frac{L_0}{2 l_0} (f_i + s_i f_{i+1}).$$

**Proposition 4.2 ($R_0$ is the NRR).** If $f$ vanishes at the first and last of the $k$ groups, then $R_0 = \sum_i F_i \prod_{j<i} s_j = \mathrm{NRR}$.

*Proof.* $\prod_{j<i} s_j = L_i / L_0$, so $R_0 = \frac{s}{2 l_0} \sum_i (f_i L_i + f_{i+1} L_{i+1})$. Each $f_i L_i$ appears twice, and the boundary terms vanish. ∎ *Checked:* exactly.

**Theorem 4.3 (Perron and Frobenius; the strong ergodic theorem).** If $M$ is non-negative and primitive (the fertile ages have greatest common divisor one), it has a simple dominant eigenvalue $\lambda > 0$ with a positive eigenvector $w$. For every $N(0) \ge 0$, $N(0) \ne 0$, $N(t)/\|N(t)\| \to w/\|w\|$ and $\|N(t+1)\|/\|N(t)\| \to \lambda$ (Lopez 1961; Keyfitz and Caswell 2005, §4.5). Moreover $\lambda$ is the unique positive root of $\sum_i F_i \prod_{j<i} s_j \lambda^{-(i+1)} = 1$. The left side decreases in $\lambda$ and equals $R_0$ at $\lambda = 1$, so $\lambda > 1 \iff R_0 > 1$.

*Checked:*
- $\lambda$ solves the characteristic equation to $6 \times 10^{-14}$, and $M w = \lambda w$;
- a population all in one group reaches the stable structure;
- the sign theorem holds for fifty schedules;
- $\lambda = 1$ at $R_0 = 1$.

**Lotka's equation.** The continuous intrinsic rate solves $\int e^{-ra} p(a) m(a)\, da = 1$, here discretised as $\sum e^{-r(x + n/2)} s f_x L_x / l_0 = 1$. It and $\ln \lambda / n$ discretise the same renewal equation and agree to $O(n^2)$. *Checked:* 0.006093 against 0.006097 per year.

## 5. What Comes Next

- **Mortality models:** Gompertz–Makeham fitted, and Lee and Carter's (1992) model of mortality over time.
- **Migration** in cohort-component projections.
- **Decomposition** of rates by Kitagawa's method.
- **Age–period–cohort analysis.** Its non-identification is shown exactly in peira's `cohorts`.
- **Real tables:** the Australian Bureau of Statistics' and the UN's World Population Prospects through the data library, held to their published life expectancies.
