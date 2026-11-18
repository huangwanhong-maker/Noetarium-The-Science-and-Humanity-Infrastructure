# Risk and Its Management

*Status: draft (2026-11-18). Activity: learning and use. The theory of the risk-management domain's `criticality` and `financing`; the domains of urban risk management and disaster risk stand on it ([ADR 0129](../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)).*

## 1. Risk as Lost Service

**Definition 1.1 (system, state, loss).** A *system* has components $1, \dots, n$. A *state* is $x \in [0,1]^n$, $x_c$ the share of its capacity component $c$ keeps; $\mathbf 1$ is the intact system. A *loss* is a function $L : [0,1]^n \to [0, \infty)$, the service given up against the intact system, stated by whoever knows the service (people beyond reach of a hospital, person-minutes added to their journeys, demand unmet).

**Definition 1.2 (hazard, expected loss).** A *hazard* is a finite family of scenarios $(x_s, w_s)$, a state and a weight $w_s \ge 0$. Its expected loss is $\mathbb E L = \sum_s w_s L(x_s)$.

The weights are rates a year, probabilities, or the weights of a quadrature over a curve of return periods (§2.3); the definitions need only that they are non-negative and that every quantity below is linear in them.

*Remark.* Risk is often written as hazard × exposure × vulnerability. Here the three are folded into $x_s$ (what the hazard does to each component, given where it stands and how it fails) and $L$ (what that costs the service). The decomposition belongs to the domain that knows the hazard (disaster risk), not to this one.

## 2. Criticality and the Importance of Components

### 2.1 Forcing a Component

**Definition 2.1.** For a component $c$ and a state $x$, write $x^{c\downarrow}$ for $x$ with $x_c = 0$ and $x^{c\uparrow}$ for $x$ with $x_c = 1$. Then

$$F_c = \sum_s w_s L(x_s^{c\downarrow}), \qquad H_c = \sum_s w_s L(x_s^{c\uparrow}),$$

the expected loss with $c$ failed in every scenario and with $c$ never disturbed.

**Definition 2.2 (importance measures).**

| Measure | Definition | Reading |
|---|---|---|
| criticality | $F_c - \mathbb E L$ | what losing $c$ would add |
| risk | $\mathbb E L - H_c$ | what $c$'s disturbance costs now: what protecting it would save |
| Birnbaum | $F_c - H_c$ | the whole swing of $c$ |
| RAW | $F_c / \mathbb E L$ | risk achievement worth |
| RRW | $\mathbb E L / H_c$ | risk reduction worth |
| Fussell–Vesely | $(\mathbb E L - H_c) / \mathbb E L$ | the share of the risk owed to $c$ |

**Proposition 2.3.** Let components fail independently, $c$ with probability $q_c$, every combination of failures a scenario with its probability, and let $L$ take values in $\{0, 1\}$ (the system fails or not). Then Birnbaum's measure is Birnbaum's importance $\partial \mathbb E L / \partial q_c$; for a series system of two it is $1 - q_2$ for the first, for a parallel system $q_2$.

*Proof.* $\mathbb E L$ is affine in $q_c$: conditioning on $c$, $\mathbb E L = q_c F_c + (1 - q_c) H_c$, since given $c$'s state the others' probabilities are unchanged. So $\partial \mathbb E L / \partial q_c = F_c - H_c$. In series, $F_1 = 1$ and $H_1 = q_2$; in parallel, $F_1 = q_2$ and $H_1 = 0$. ∎

Checked in `check_risk_management_criticality`.

### 2.2 What Need Not Be Asked

**Proposition 2.4.** If $x_{s,c} = 0$ then $L(x_s^{c\downarrow}) = L(x_s)$; if $x_{s,c} = 1$ then $L(x_s^{c\uparrow}) = L(x_s)$.

So a scenario that disturbs few components asks for few hardened losses. A system that knows which components cannot change its loss at a state (a road off every shortest path, urban risk management's `lifelines` §2) answers all forced failures of that state at once.

### 2.3 A Loss over a Curve of Return Periods

**Proposition 2.5.** Let scenarios be the undisturbed system and the states of the $T_1 < \dots < T_m$-year events, and let the loss be integrated over the annual exceedance probability $p = 1/T$ by trapezoids between the maps, held at the last value to $p = 0$, and taken as nothing for $p > p_1$. With $p_k = 1/T_k$, the weights

$$w_0 = 1 - p_1, \quad w_1 = \tfrac12 (p_1 - p_2), \quad w_k = \tfrac12 (p_{k-1} - p_{k+1}) \ (1 < k < m), \quad w_m = \tfrac12 (p_{m-1} - p_m) + p_m$$

reproduce that integral of any loss, and sum to 1. Counting instead the first map's state falling linearly to nothing at $p = 1$ (tyche's `curve_area`) moves $\tfrac12(1 - p_1)$ from $w_0$ to $w_1$.

*Proof.* Each trapezoid $[p_{k+1}, p_k]$ gives half its width to each end; the tail $[0, p_m]$ gives its width to the last; the maps' weights sum to $p_1$, and the undisturbed state takes the rest. ∎

*Remark.* $w_k$ is the probability that the year's largest event falls in the band the $k$-th map stands for, so $\mathbb E L$ is the expectation of the loss over the year's largest event; with a loss measured per trip it is the expected time added to one trip during that event, not a total over the year, since how long a flood stands is not in the maps. The default is a lower bound where events more frequent than $T_1$ would disturb the system; the linear convention, with a first map at 20 years, counts nearly half the 20-year flood's state over every year, which overstates frequent events whose maps are not given.

Since $F_c$ and $H_c$ are linear in the weights, criticality and risk integrated over the curve are the same quadrature of each scenario's criticality and risk. The undisturbed state's weight matters to $F_c$: a link lost in a dry year still costs its criticality.

*Caveat.* Between the mapped return periods the states are interpolated in the loss, not in the hazard; a component that fails between $T_k$ and $T_{k+1}$ is counted by the trapezoid.

## 3. Financing a Loss

### 3.1 Layers

A layer $[a, a + l]$ takes $\min(\max(L - a, 0), l)$ of each event's loss (tyche's `layers`, its decision 0029). A tower $0 = a_0 < a_1 < \dots$ splits every loss exactly. From a catalogue of events over $Y$ years, a layer's expected payment a year is $E_i = \sum_e \min(\max(L_e - a, 0), l) / Y$.

### 3.2 The Cost of an Instrument and of What Is Unfunded

**Definition 3.1.** An instrument on $[a, a + l]$ costs a year $f + r l + m E_i$ (a fee, a rate on its limit, a premium as a multiple of its expected payment) and at each event $u$ for each unit it pays. Its expected annual cost is $f + r l + (m + u) E_i$.

**Definition 3.2.** What no instrument pays of an event's loss, $U_e$, costs $\phi(U_e)$, with $\phi$ piecewise linear and non-decreasing in its slopes: declared tiers $0 = t_0 < t_1 < \dots$, the slope on $[t_{j-1}, t_j)$ the cost of a unit raised after an event of that size.

**Definition 3.3.** A strategy is a set of instruments with disjoint layers and the function $\phi$; its expected annual cost is $\sum_i (f_i + r_i l_i + (m_i + u_i) E_i) + \sum_e \phi(U_e)/Y$.

*Rationale.* A budget or a reserve pays at $u = 1$ (the reserve's holding cost is its rate $r$); a contingent credit at $u$ equal to the present value of repaying a unit; insurance and a bond at $u = 0$, having paid $m E_i$ before. When $\phi$'s slopes rise with the amount, as raising funds after a large event costs more, transfer at a multiple $m > 1$ can be cheaper in expectation than leaving the top unfunded, and the layering that minimises cost holds the bottom and transfers the top (Mahul and Ghesquiere 2010; Clarke et al. 2017).

### 3.3 Choosing a Tower

Instruments are given in order from the bottom; the layer of the $i$-th is $[b_{i-1}, b_i)$ with $b_0 = 0$ and $b_1 \le \dots \le b_k$ from finite candidates (an equal pair leaves an instrument out). `best_tower` lists every non-decreasing sequence, $\binom{M + k - 1}{k}$ of them for $M$ levels, and returns the cheapest whose unfunded part of a stated event lies within a cap. The search is exact over the candidates and says how many it tried and how many met the cap.

### 3.4 Basis Risk

A parametric instrument pays $P(I)$ on an index. Against the indemnity layer $C$ it is meant to meet, its basis risk is the distribution of $P(I) - C$ over events: its absolute and squared means, the shortfall $\mathbb E\max(C - P, 0)$, the excess, and the rates at which a loss meets no payout and a payout no loss (tyche's `basis_risk`). The linear payout between a trigger and an exhaustion is fitted to least squared error on quantiles of the index.

*Caveat.* An event-loss table without its events' years supports occurrence covers only. The index must come with the events; a catalogue that gives only losses cannot be used to price a parametric cover, and the work says so rather than invent one.
