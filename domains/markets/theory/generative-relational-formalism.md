# The Generative–Relational Formalism in the Markets Domain

*Status: draft (2026-11-12). [ADR 0099](../../../development/decisions/0099-the-market-foam.md). What the domain's modules `system`, `foam`, `valuation`, `money` and `procedure` hold of the user's formalism, what the checks establish, and what the drafts should revise.*

It serves **learning** and **research**.

**Sources.** W. Huang (2026), *A Unified Generative–Relational Formalism for Market Analysis*, Working Formalism v0.9 (cited as **U**); *Market Foam: A Labelled Two-Complex Model of Market Spacetime With Heterogeneous Facts*, v0.9a (**F**); *Inflation and Justice in the Market Foam: Two Case Studies*, v0.9b (**C**). Statements are cited by their labels there: O, D, K, F, P for **U**; S for **F**; C for **C**. The drafts' verification scripts are not in this repository, so every number below was computed here from the drafts' text.

## 1. Scope

The drafts design one object in five layers. Their proofs use the fragment in which events change quantities only, and so does the code. Table 1 says where each statement lives and which check holds it. The check programs are `domains/markets/<module>/tests/check_<module>.cpp`; two general methods went to the foundations first.

**Table 1.** The drafts' statements, the code that holds them, and their checks.

| Statement | Held by | Check |
|---|---|---|
| U O.1, invariants as a kernel | `system`: `invariants`, `increments` | `check_system`: three subjects, three kinds |
| U D.1, invariants in every regime | `system`: `drift`, `diffusion`, `hamiltonian` | the same |
| U D.2, the stationary regime does not see the speed | `system`: `hamiltonian` | two states, $c = 1$ and $c = 4$ |
| U D.3, kurtosis under a mismatched clock | (closed form) | a Poisson clock of mean 20: 3.15 |
| U D.4, commute time and effective resistance | `system`: `resistance`, on physis's Kirchhoff networks | seven nodes |
| U O.2, O.3, price potentials and their covariance | `valuation`: `reference`, on physis's Hodge decomposition | `check_valuation` |
| U D.5, arbitrage on triangles | `valuation`: `dissipate`, `settled` | the same; clause (iv) as amended (§5, A1) |
| U K.1, K.2, visible and hidden discrepancy | tyche `probability/information.hpp` (its decision 0016) | tyche's `check_information` |
| F S.2 to S.5, the history complex | `foam`: `foam`, `slice`, `order` | `check_foam` |
| F S.6, S.7, balance laws; balance as an invariant subspace | `foam`: `imbalance`, `at_junction` | the same |
| F S.8, S.9, the path weight and the area law | `foam`: `area_weight`, `hazard_integral` | the same |
| F S.10, S.11, the sum over histories; the clock-free law | (identities) | `check_system` |
| F S.12, S.13, proper time; finite propagation | `foam`: `order`, `steps_from`; `system`: `simulate` | `check_system` |
| F S.14, Equation (7.1), the valuation connection | `valuation`: `lenses` | `check_valuation` |
| F S.15, S.16, numéraire-free valuations; duality | (identities) | the same |
| C C.1, the equation of exchange | `foam`: `stock`; `money`: `velocity` | `check_system`, on a generated record |
| C C.2, C.3, redenomination and Fisher's relation | `money`: `fisher`; `valuation`: `lenses` | `check_money`, `check_valuation` |
| C C.4, an injection of money | `money`: `injection`, `first_order_gains` | `check_money`; generalised (§5, A4) |
| C C.6 to C.8, C.12, reciprocity | `valuation`: `reference`, `lenses`, `CycleMap` | `check_valuation` |
| C C.9 to C.11, order and its mixtures | `procedure`: `order_sensitivity`, `divide`, on nomos's `games/claims.hpp` (its decision 0011) | `check_procedure`; C.11 as amended (§5, A2) |
| C C.13 to C.15, Equation (4.3), procedure as a law on histories | `procedure`: `compare`, `reliability`, `asymmetry`, `heard` | `check_procedure` |

## 2. A Market as a System

**Definition 2.1.** A *grammar* in the stoichiometric fragment is a set of places $P$ (a subject's position in a kind) and event types $a$ with vectors $\nu_a \in \mathbb{R}^P$: a transfer $q(e_j - e_i)$; an exchange at the ratio $r$, $q(e_{ik} - e_{jk}) + rq(e_{jl} - e_{il})$ up to the sign convention of who gives; an issuance $q(e_{c\kappa} - e_{d\kappa})$ with $d$ the issuer. With propensities $\lambda_a(x) \ge 0$ it generates a Markov jump process.

Positions are signed. A liability is a position below zero, and a claim issued from nothing keeps $\sum_s x_{s\kappa} = 0$.

**Proposition 2.2 (U O.1, D.1).** $c \cdot x$ is constant along every history exactly when $c \in \ker \nu^{\top}$. For such $c$: $c \cdot b = 0$ for the drift $b = \sum_a \nu_a \lambda_a$; $c^{\top}\sigma\sigma^{\top}c = 0$ for $\sigma\sigma^{\top} = \sum_a \lambda_a \nu_a\nu_a^{\top}$; and $H(\xi, \theta + sc) = H(\xi, \theta)$ for $H = \sum_a \lambda_a (e^{\theta\cdot\nu_a} - 1)$.

*Computation.* `invariants` returns the eigenvectors of $\nu\nu^{\top}$ with eigenvalue zero. With all transfers and exchanges among three subjects and three kinds, and one claim issued, the space has dimension three. $\square$

**Distance (U Definition 4.2, D.4).** For symmetric rates $w$ of a tracer on $n$ nodes, the effective resistance $R_{ij}$ is a distance and the expected time from $i$ to $j$ and back is $nR_{ij}$. `resistance` is the reciprocal of physis's effective conductance. Raising one rate lengthens no distance (Rayleigh).

**A market from rules.** `generate` makes a record whose truth is known: each unit of money buys at the hazard $V$; a purchase exchanges money for a good at its reference price, moved by noise and by $\beta_{\text{giver}} - \beta_{\text{taker}}$; subjects also barter. It is the test bed of §4, and of the plug-in of Market Studio.

## 3. The Market Foam

### 3.1 The History Complex

**Definition 3.1 (F 4.1, 4.2).** A *history* is junctions, facts (each with two ends among the junctions, a type and a value), and events in time, each touching some facts: creating, removing, modifying or reading them. An event touches a junction when it touches a fact ending there. The *history complex* has

- as vertices the events, and for each junction a point at the beginning and at the end of the history;
- as edges the segments of each junction's worldline between consecutive events that touch it, and the links of the first and of the last state;
- as faces, for each fact, each interval between consecutive events that touch it, attached along the two ends' worldlines over the interval, pinched at an event and closed by a link at the history's ends.

A fact given one end is completed by a private junction that lives as long as the fact.

**Proposition 3.2 (F S.2, S.3).** The complex is a two-dimensional cell complex with $\partial_1\partial_2 = 0$, and at every time between events the faces containing it, with their labels, are the state. (`check_foam`: on 160 events, as bearer holdings and booked at ledgers.)

**Proposition 3.3 (the interior is morphe's worldline complex).** Let $E$ be the record of encounters whose participants are the junctions, whose events are the history's, and whose pairs are the facts, each meeting at the events that touch it. Then the faces of the history complex that begin and end at events are the lenses of morphe's worldline complex $W(E)$, and its segments between events are the segments of $W(E)$.

*Proof.* A lens of $W(E)$ is attached along $a[p,q]\cdot b[p,q]^{-1}$ for consecutive meetings $p, q$ of a pair $(a, b)$, which is the attaching word of Definition 3.1 for a face from $p$ to $q$. Segments are defined alike in both. $\square$

So the economics domain's spacetime of transactions, the history domain's complex and this one are one construction. What Definition 3.1 adds is the two ends: with them the boundary of the complex is the first and the last state, as graphs.

**Presentation.** Where a kind's positions are booked is a choice (F §3). `history` reads a record of movements either way: a kind held as bearer holdings makes each position a fact with one end; a kind booked at a ledger makes each position a fact between its holder and the ledger.

### 3.2 Order, Time, Balance and Weight

**Definition 3.4.** An event *directly precedes* the next event on a junction's worldline. The *worldline order* is the closure. A junction's *proper time* between two of its events is the number of segments between them.

**Proposition 3.5 (F S.5).** The order of elements (an event precedes the next that touches the same fact) is within the worldline order. They coincide when each junction holds one fact that every event on it changes. With three kinds held as bearer holdings the containment is strict. (`check_foam`.)

**Proposition 3.6 (settlement decides the causal structure; F §7.2, S.13).** With one kind booked at one ledger, every two movements are causally related: the worldline order is total. As bearer holdings, movements between disjoint pairs are unrelated. If every event touches junctions within a diameter $D$, an event reached in $m$ steps lies within $(m+1)D$ of the first; on a ring of neighbours the bound is attained. (`check_foam`, `check_system`.)

**Proposition 3.7 (F S.6).** In the history of a record of movements, at every event each kind's labels after are its labels before; on a ledger's worldline the labels add up to the kind's total at every time (a Gauss law). Both hold to rounding.

**Proposition 3.8 (F S.9).** If each fact $f$ generates events at the hazard $h_f$ times its label, the exponent of the path weight is $\sum_f h_f\, j_f\, |f|$ over the faces: the integral over time of the total hazard, arranged by face. (`area_weight` against `hazard_integral`.)

**Proposition 3.9 (F S.10, S.11).** For a finite chain with rates $R$ and totals $\Lambda$, $\sum_n [(s + \Lambda)^{-1}R]^n (s+\Lambda)^{-1} = (s - Q)^{-1}$; and the law of the sequence of events, $\prod_i R(x_i, x_{i+1})/\Lambda(x_i)$, does not change when the rates at each state are multiplied by a positive factor. (`check_system`.)

## 4. Money, Reciprocity and Procedure

### 4.1 Money

**Velocity (C C.1).** $V = \sum_v m_v / \int_0^T M\,dt$: the payments over the area of the money faces. On a generated record with $V = 2.5$ the estimate is within $0.003$.

**Fisher's loop (C C.3).** $h = (i - \pi - \rho)\tau$. An inflation four points above what a claim was written for leaves $h = -0.04$ and a real factor of $0.9608$.

**An injection (C C.4, generalised).** Let $W$ be row-stochastic, $W_{ij}$ the share of site $i$'s spending that goes to $j$; each site produces $y$, holds $m_i$ and spends at the rate $v$. Receipts clear each market:

$$p_j = \frac{v\,(W^{\top}m)_j}{y}, \qquad c_i = y\, m_i \sum_j \frac{W_{ij}}{(W^{\top}m)_j}, \qquad \dot m = v\,(W^{\top} - I)\,m. \tag{4.1}$$

**Proposition 4.1.** (i) $\sum_i c_i = ny$ at every time, for every row-stochastic $W$. (ii) At rest $m$ is in proportion to the stationary law of $W$; balances are equal, with one price, exactly when the columns of $W$ also sum to one. After an injection $\Delta M$ every price ends $1 + \Delta M/M$ times what it was. (iii) When the columns of $W$ sum to one, to first order in $\Delta M$ at site $0$,

$$G = \frac{\Delta M}{p}\,\bigl(I - WW^{\top}\bigr)\bigl(I - W^{\top}\bigr)^{+}\Bigl(e_0 - \tfrac1n\mathbf 1\Bigr), \tag{4.2}$$

which for symmetric $W$ is $\frac{\Delta M}{p}(I + W)(e_0 - \frac1n\mathbf 1)$, the source's Equation (3.4).

*Proof.* (i) $\sum_i c_i = y\sum_j \bigl(\sum_i m_i W_{ij}\bigr)/(W^{\top}m)_j = ny$. (ii) $W^{\top}m = m$ exactly for $m$ in proportion to the stationary law; it is constant exactly when $\mathbf 1^{\top}W = \mathbf 1^{\top}$. The limit holds the new total in the same proportions. (iii) With $m = \bar m(1 + \varepsilon)$ and column sums one, $(W^{\top}m)_j = \bar m(1 + (W^{\top}\varepsilon)_j)$, so $c - y\mathbf 1 = y(I - WW^{\top})\varepsilon$ to first order, and $\dot\varepsilon = v(W^{\top} - I)\varepsilon$. Integrating on the complement of the constants gives $(I - WW^{\top})[v(I - W^{\top})]^{+}$ applied to $\varepsilon(0) - \bar\varepsilon\mathbf 1$; and $y/(v\bar m) = 1/p$. $\square$

On the source's ring of twelve the code reproduces its numbers: the site gains $0.00831$ against $0.00833$ to first order, its neighbours $0.00333$, distant sites lose $0.00167$; the prices at distances two to six have made half their rise at $0.21$, $1.60$, $4.81$, $8.07$ and $9.12$; and an injection of half a balance gains the site $0.372$ against $0.417$.

*Remark.* By (4.2) a partner of the injected site gains only if it receives more than $2/n$ of that site's spending. In a market where each subject spends evenly with all the others ($W_{i0} \approx 1/(n-1) < 2/n$) the injected subject alone gains. The first-receiver advantage reaches as far as spending is concentrated.

### 4.2 Reciprocity

**Reference valuation (U O.3, C C.7).** For trades $v$ with ratios $a_v = \ln(\text{units of } l \text{ per unit of } k)$ and weights $w_v$, $\pi^{\star}$ minimises $\sum_v w_v (a_v - \pi_k + \pi_l)^2$; the residual is $\rho_v = a_v - (\pi^{\star}_k - \pi^{\star}_l)$; the advantage of a subject is $\sum \rho_v$ over the trades in which it gave $k$, less $\sum\rho_v$ over those in which it gave $l$. Residuals and advantages are unchanged by a change of units; the advantages add up to zero; the holonomy of a cycle of trades is the sum of its residuals. On a generated record the advantages follow the hidden $\beta$ at a correlation above $0.99$.

**Loops through time (F Equation 7.1, C C.6).** Two exchanges of kinds $k, l$ at $t_0 < t_1$ and the holdings between, with carries $\rho_k, \rho_l$:

$$h = \underbrace{a_0 - a_1}_{\text{geometric}} + \underbrace{(\rho_l - \rho_k)(t_1 - t_0)}_{\text{dynamical}}.$$

A static change of units moves neither part. A redenomination of $l$ at the rate $\pi$ that every rate follows moves $\pi\tau$ from the first to the second and leaves $h$.

**Cycle maps (C C.8, C.12).** $\Phi = \begin{pmatrix}1.8 & 0.6\\ 0 & 1/1.8\end{pmatrix}$ has determinant one and takes $(1,1)$ to $(529.19, 0.0028)$ in ten rounds.

### 4.3 Procedure

**Order (C C.9, C.10).** For steps as stochastic matrices, `order_sensitivity` is $([T_A, T_B]f)(x)$. Priority steps on an estate of 10 with claims 8 and 6 give $(8,2)$ and $(4,6)$; on what the first claimant receives the commutator is 4. Their mixture by lot is the Shapley value of $w(S) = \max(0, E - \sum_{j\notin S} c_j)$: here $(6,4)$, and $(4.5, 3, 2.5)$ for claims 7, 5, 4.

**Procedures as laws on histories (C C.13 to C.15).** With `compare` on tyche's chain rule, two procedures with one law of outcomes hide their whole discrepancy from a record of outcomes: on the source's example $1.7578$ nats, with admissible weights $0.81$ and $0.09$, an asymmetry of $3.5156$ nats in the second, and reliabilities $0.86$ and $0.70$.

## 5. Amendments

Five places where the drafts should change. Each is held by a check.

**A1. U Proposition D.5, clause (iv).** *As stated:* a cycle outside the span of triangles keeps its holonomy under $\dot a = -\gamma C^{\top}Ca$. *Counterexample.* A square $0\text{–}1\text{–}2\text{–}3$ with a triangle $0\text{–}1\text{–}4$ on one side. The square is not a sum of triangles. The flow lowers each edge of the triangle by a third of the triangle's holonomy $h_\triangle$, and the edge $01$ belongs to the square: the square's holonomy moves by $-h_\triangle/3$. The proof's claim that a cochain supported on such a cycle lies in $\ker C$ fails whenever the cycle shares an edge with a triangle. *Corrected.* $a(t) \to P_{\ker C}\,a(0)$, the exact part plus the harmonic part of $a(0)$. The holonomy of a cycle $z$ tends to $\langle z, P_{\ker C}a(0)\rangle$, which is its initial holonomy exactly when $z \in \ker C$ (for one, when it shares no edge with a triangle). What persists is the harmonic part, not each cycle's own number. (`check_valuation`: "a square that shares an edge with a triangle does not keep its holonomy".)

**A2. C Proposition C.11 and the reading after it.** *As stated:* the concurrent procedure, the limit of fine interleaving, is one of two order-neutral procedures; in the claims problem interleaved payments in proportion to claims converge to the proportional rule. *Two points.* (a) The limit depends on the rates at which the steps run: at rates in proportion to the claims it is the proportional rule, at equal rates the constrained equal awards, a different rule. Fine interleaving removes the order and leaves the rates, so "the concurrent procedure" names a family indexed by rates, and choosing among them is a choice of rule. (b) The steps of the claims problem are truncations, not bounded linear generators, so Proposition C.11 does not apply to them as stated; the convergence there is elementary (each round pays $h r_i$ until a claim or the estate is exhausted). (`check_claims` in nomos: "paid at equal rates, they tend to the constrained equal awards, another rule".)

**A3. F Proposition S.1 and Table 3.** The proof's construction makes a kind a fact kind when it is the source of one or two structural maps into the remaining kinds. In the minimal schema of **U** a claim *position* is a `Pos`, whose only map into a remaining kind is `holder`; its issuer is reached through `kind: Pos → Res` and the inclusion `inc: Clm → Res`, which is not a structural map out of `Pos`. So the construction presents a claim as a fact with one end, while Table 3 presents it between holder and issuer. *Corrected.* Either the schema gains a kind of claim positions with a map to `Clm` (then `issuer ∘` it gives the second end), or Table 3 is said to be a presentation chosen by design and not the one the proof constructs. The code takes the second reading: `history` is told which kinds are booked.

**A4. C Proposition C.4.** *As stated:* $p_j = v(Wm)_j/y$, and total consumption is total output "by symmetry of $W$". *Corrected.* The receipts of site $j$ are $v(W^{\top}m)_j$. With that, (i) needs no symmetry, (ii) the rest state is the stationary law of $W$ and not uniform in general, and (iii) has the form (4.2). Written with $(Wm)$ on a ring that sends $0.7$ forward and $0.3$ back, consumption does not add up to output. (`check_money`: "with receipts written (Wm) it does not, unless W is symmetric".)

**A5. F §7.2 and check F4.** The draft reports a share of causally related pairs of $0.957$ under ledger settlement for 120 transfers among 40 subjects. With one kind at one ledger every event touches the ledger, the worldline order is total, and the share is exactly one. The reported figure needs its setup stated (several kinds, each with a ledger, would give less than one).

## 6. The Drafts' Numbers

Reproduced from the text alone: the holonomy $0.00978$ of a cycle with product $1.00983$; the rate $0.7746$ of (U 5.2); the kurtosis $3.15$; every number of C.3, C.4, C.6, C.8, C.10 and C.15 quoted in §4; the five closed labellings of F check F9.

Not comparable without the scripts: the value $492.574519$ of F9 (it depends on the weights $w$, which the draft does not give; the identity is held for weights of our own); J5's gap of $0.008$ at steps of $1/256$ and J6's coefficients (they depend on the step rule and the matrices); F4's $0.404$ and F6's means (random histories).

## 7. What Is Not Yet Here

- **The categorical layer of U §3.1 to §3.4:** schemas as categories, states as functors, data migration along schema functors, and events as double-pushout rules on attributed states. morphe holds graph rewriting by double pushouts; the two are not yet joined. The drafts' own proofs do not use it.
- **Observers as schema views (U §5.1), the observation sheaf (H2), the reflexive extension (U §6.1), consent factorisation (U §7.2), action classes and reification (U §7.4).** Of the epistemic layer only the discrepancy accounting is computed.
- **The classical cases of U §9** other than those above: decentralised exchange, the zero-intelligence auction, quality uncertainty, belief selection (F.2) and the Price equation, the cobweb, the deposit run, the medium of exchange (P.2), stable matching. Several have their general part in the foundations already (nomos's matching and agents).
- **Rule change, kind creation and reification as constructions on the complex** (F §10, item 3); facts with more than two ends.
- **The hypotheses** H1 (continuum), H3 (non-abelian labels), H4 (generating expansion).
- **Field data.** Every check is on a synthetic model, as in the drafts.

## References

- Aumann, R. J., and Maschler, M. (1985). Game theoretic analysis of a bankruptcy problem from the Talmud. *Journal of Economic Theory* 36: 195–213.
- Fisher, I. (1911). *The Purchasing Power of Money*. Macmillan. Fisher, I. (1930). *The Theory of Interest*. Macmillan.
- Jiang, X., Lim, L.-H., Yao, Y., and Ye, Y. (2011). Statistical ranking and combinatorial Hodge theory. *Mathematical Programming B* 127: 203–244.
- Klein, D. J., and Randić, M. (1993). Resistance distance. *Journal of Mathematical Chemistry* 12: 81–95.
- O'Neill, B. (1982). A problem of rights arbitration from the Talmud. *Mathematical Social Sciences* 2: 345–371.
- Shapley, L. S. (1953). A value for n-person games. *Contributions to the Theory of Games II*: 307–317.
