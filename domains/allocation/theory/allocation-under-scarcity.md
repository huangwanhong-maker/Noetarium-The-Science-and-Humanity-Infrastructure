# Allocation under Scarcity: Rounds, Verdicts, Procedure and Incidence

*Status: draft (2026-11-20; [ADR 0136](../../../development/decisions/0136-allocation-under-scarcity.md)). Activity: use and research.* The theory of the allocation domain's four modules, with the results its code relies on and what it does not claim. The literature is collected in the infrastructure's survey *Allocative justice under scarcity* (`research/literature/reports/`), cited here as "the survey".

## 1. The Question and Its Form

When a resource falls short of what is wanted, someone decides who is served first, what each receives and how much, and by what procedure. The survey's first finding (§1.1) is that practice decides this by composing declared rules, not by maximising one objective, and that the principles invoked are orders over whole allocations that conflict (§1.2). The domain takes that form literally:

- a **round** is a sequence of stages, each a rule with a named principle;
- a **verdict** compares a round with the rounds it could have been, under several principles at once, and reports a partial order, not a score;
- a **procedure** is a sealed record against which checkable rules of procedural justice are decided;
- **incidence** follows the money past its first recipients.

*Rationale.* Fleurbaey and Tungodden (2010) show that a complete ordering of allocations satisfying weak Pareto and a minimal concern for the worst off falls into one of two tyrannies: of aggregation (small gains to many outweigh a large loss to one) or of non-aggregation (any gain to the worst off outweighs everything). A tool that summed principles would have chosen a tyranny for its user. The domain reports where principles agree and where the choice turns on precedence, and leaves the precedence to people.

## 2. Rounds

### 2.1 Definitions

**Definition 2.1 (round).** A round is a tuple $R = (A, E, a, (s_1, \dots, s_K))$: an amount $A \ge 0$; a finite set $E$ of entities; attributes $a : E \times \mathcal{K} \rightharpoonup \mathbb{R}$, a partial map from entities and attribute names to numbers; and stages $s_k$, each with a kind, a share $\theta_k \in [0, 1]$, its parameters, a named principle and a source (who declared it, when and why).

**Definition 2.2 (run).** Let $r_0 = A$, $x^{(0)} = 0 \in \mathbb{R}^E$ and $M_0 = E$, the entities still taking part. Stage $k$ is given the pool $P_k = \theta_k r_{k-1}$ and returns $g^{(k)} \in \mathbb{R}^E_{\ge 0}$, zero outside $M_{k-1}$, and $M_k \subseteq M_{k-1}$; then $x^{(k)} = x^{(k-1)} + g^{(k)}$ and $r_k = r_{k-1} - \sum_i g^{(k)}_i$. The result is $x = x^{(K)}$, the amounts by stage $g^{(k)}$, and what is left, $r_K$.

The seven kinds, for $i \in M_{k-1}$ (`allocation::rounds`):

| Kind | $g^{(k)}_i$ | Source |
|---|---|---|
| eligibility | $0$; $M_k = \{ i : a_i(\alpha) \ge b \}$ (or $\le b$), each other entity excluded with its reason | a constraint, not a weight |
| floor | the gaps $d_i = \max(0, \tau - a_i(\ell)) \, a_i(p)$, met in full when $\sum d \le P_k$, else $\rho(P_k, d)$ for the declared rationing rule $\rho$ | sufficiency first; the survey §1.5 |
| priority | classes by $a(\alpha)$, highest first, the residual claims $c_i = \max(0, a_i(c) - x^{(k-1)}_i)$ met class by class, the class where the pool runs out divided by $\rho$ | Moulin (2000), priority classes |
| claims | $\rho(\min(P_k, \sum c), c)$ for the residual claims $c$ | O'Neill (1982); Thomson (2003) |
| proportional | $P_k \, a_i(w) / \sum_j a_j(w)$ | |
| points | $P_k \, \pi_i / \sum_j \pi_j$, $\pi_i = \sum_q w_q \, \mathrm{grade}_q(a_i(q))$ on declared bands | Porto Alegre (Santos 1998) |
| equalisation | $P_k \, p_i / \sum_j p_j + \lambda p_i [(e_i - \bar e) + \sum_m t_m(\bar b_m - b_{im})]$, a grant below zero held at zero and the difference spread over the others by people | horizontal fiscal equalisation |

where $\rho$ is one of nomos's rationing rules (proportional, constrained equal awards and losses, the Talmud rule, TAL($\theta$), Piniles, adjusted proportional, random arrival).

### 2.2 Results

**Proposition 2.3 (conservation).** For every round, $\sum_i x_i + r_K = A$, and every stage of every kind gives at most its pool: $\sum_i g^{(k)}_i \le P_k$.

*Proof.* By Definition 2.2, $r_K = A - \sum_k \sum_i g^{(k)}_i$ and $\sum_i x_i = \sum_k \sum_i g^{(k)}_i$. For the bound: eligibility gives nothing; floor, priority and claims give either the claims in full when they sum to at most $P_k$, or $\rho(P_k, \cdot)$, which sums to $P_k$ by the feasibility of rationing rules; proportional and points give exactly $P_k$; equalisation's formula part sums to zero (Definition: $\sum_i p_i(e_i - \bar e) = 0$ and $\sum_i p_i(\bar b_m - b_{im}) = 0$ for population-weighted means), so its total is $P_k$, and holding a grant at zero while subtracting the same amount from the others preserves the total. $\square$

**Proposition 2.4 (non-negativity).** Every amount of the first six kinds is at least zero; equalisation's amounts are at least zero after its redistribution terminates.

*Proof.* Rationing rules are bounded below by zero, pools are at least zero, and points and proportional weights are at least zero (refused otherwise). For equalisation, each pass sets the negative grants to zero and lowers the positive ones in proportion; the code makes at most $|M|$ passes. *Caveat.* That $|M|$ passes suffice is checked on 200 random rounds, not proved: a pass can push a small positive grant below zero, which the next pass catches.

**Definition 2.5 (who first).** The order reached sorts the entities taking part by the first stage that gave each something, and within a stage by what it gave, largest first; entities never reached follow, the excluded last. It answers the user's question "to which entity first, then which" for the round as declared; in a priority stage on a continuous attribute every entity is its own class, and the order is the priority order itself.

## 3. Verdicts

### 3.1 The Options

Let $V$ be the set of variations of a round: for each of its $m$ rationed stages a rule from a declared list $L$, and a permutation of the stages after the leading eligibility stages. So $V \cong L^m \times S_{K - e}$, enumerated in full when $|L|^m \le 512$ (else one stage at a time) and for $K - e \le 6$. The run gives a map $\Phi : V \to \mathbb{R}^E$. The **options** are the image of $\Phi$, each named by every variation that reaches it: the quotient of $V$ by equality of results.

**Definition 3.1 (reading).** A result $x$ is read as an outcome $o_i = \ell_i + x_i / p_i$ with weight $p_i$, for a declared level $\ell$ (or zero) and a declared count of people $p$; entities with $p_i = 0$ cannot be read per head and are left out of every option alike, which the verdict says.

### 3.2 The Order

Each principle $\pi$ (nomos's `welfare`: total, prioritarian($\varepsilon$), leximin, shortfall($\tau, \alpha$), opportunity) is a complete preorder $\succeq_\pi$ on outcomes.

**Definition 3.2 (dominance).** $o \succ o'$ when $o \succeq_\pi o'$ for every declared $\pi$ and $o \succ_\pi o'$ for at least one.

**Proposition 3.3.** $\succ$ is a strict partial order on the options, and on a finite set of options its maximal set (Sen's undominated options) is non-empty.

*Proof.* Irreflexive: $o \succ_\pi o$ holds for no $\pi$. Transitive: if $o \succ o'$ and $o' \succ o''$, then $o \succeq_\pi o''$ for every $\pi$ by transitivity of each preorder, and for the $\pi$ with $o \succ_\pi o'$, $o \succ_\pi o''$. A finite strict partial order has maximal elements. $\square$

The verdict reports: the maximal set; each option's best principles; the **dominance pairs**, whose covering relation is drawn as a Hasse diagram, an option at the length of the longest chain above it; for the declared round $d$ and each other maximal $m$, the **pivot**, the sign vector $\sigma(d, m) \in \{-1, 0, 1\}^{\Pi}$ said in words; and, for a principle with a parameter $t$ (aversion, threshold), the **flips**, the values of $t$ in a declared interval at which $\mathrm{sign}(f_t(d) - f_t(m))$ changes, found on a grid and refined by bisection.

*Remark.* Generalised Lorenz dominance implies a higher prioritarian value for every $\varepsilon \ge 0$ (Shorrocks 1983); it is the part of the order no choice of aversion can reverse, and nomos's checks hold it on random pairs.

*Remark.* For fixed totals (every option gives the whole sum), the total principle is constant on the options and never tells them apart. The domain's checks hold this, and the England case shows it: what remains is distribution.

### 3.3 What Is at Stake

For each entity $i$, the stake is $[\min_{m \in \mathcal{M}} x^{(m)}_i, \max_{m \in \mathcal{M}} x^{(m)}_i]$ over the maximal set $\mathcal{M}$, with the declared amount beside it. It says, for each party, how much turns on choices the principles do not settle.

## 4. Procedure

### 4.1 The Record

A round and a result are written as canonical texts in which every field is prefixed by its length, so that the text is an injective function of the declaration; their SHA-256 digests name them. The record is archeion's journal: entries $(t, \text{who}, \text{what}, \text{about}, \text{note})$ sealed by $s_k = H(s_{k-1} \,\|\, 0 \,\|\, e_k)$ (Haber and Stornetta 1991). Altering, removing, inserting or reordering an entry changes every later seal, so a record whose last seal is known elsewhere cannot be altered unnoticed except by a collision of SHA-256.

The acts are: declare (a round's digest published), consult (a group), interest (a person's interest in an entity declared), oversee (a body that may enforce), allocate (a round's digest and its result's), appeal, review (an upheld review declares the corrected round), commit (a seed's digest) and draw.

### 4.2 Appeals Heard for Everyone

An appeal attacks a datum $a_i(\kappa)$, a parameter of a stage, or a stage's applicability. Its hearing computes the corrected round $R'$ and reports $\Delta = \{ i : x_i(R') \ne x_i(R) \text{ or exclusion changed} \}$, every entity the correction moves. An upheld appeal declares $R'$; the record is correctable only when the round last applied is the last one revised. *Rationale.* The survey reads the failures of Robodebt and the Dutch childcare benefits as appeals that settled individual cases while the rule stood (§1.6, Part V §3); a correction here is a correction of the rule.

### 4.3 Lotteries

**Proposition 4.1 (Efraimidis and Spirakis 2006).** Let $u_1, \dots, u_n$ be independent and uniform on $(0, 1)$ and $w_i > 0$. The index maximising $\log(u_i) / w_i$ is $i$ with probability $w_i / \sum_j w_j$.

*Proof.* $-\log u_i$ is exponential with rate 1, so $-\log(u_i)/w_i$ is exponential with rate $w_i$; the maximum of $\log(u_i)/w_i$ is the minimum of these, and the minimum of independent exponentials is the $i$-th with probability $w_i / \sum_j w_j$. $\square$

The domain takes $u_i$ from the first 52 bits of $H(\text{seed} \,\|\, \texttt{0x1F} \,\|\, \text{id}_i)$, so that anyone holding the seed repeats the draw; the seed's digest is entered before the draw. Its checks find a quarter of first places for each of four candidates over 4,000 seeds, and a quarter, a quarter and a half for weights 1, 1, 2. *Caveat.* The commitment stops the seed from being changed after the candidates are known; it does not stop a committer who knows the candidates from trying many seeds before committing. A seed from a public source of randomness drawn after the list is fixed would.

### 4.4 The Rules of Procedural Justice

| Rule | Framework | Decided as |
|---|---|---|
| record sealed | both | archeion's `verify` |
| consistency | Leventhal | the result recorded for the round applied equals the result recomputed |
| publicity | A4R | the round applied was declared, and before it was applied |
| reasons given | A4R | every stage names its principle, who declared it and why |
| relevance | A4R | undecided: a person's judgement |
| accuracy | Leventhal | every attribute a stage reads has a stated source |
| bias suppression | Leventhal | no stage was declared by someone with a declared interest in an entity it gave to; undecided when no interest was declared |
| representativeness | Leventhal | every group of entities was consulted; undecided when entities name no groups |
| correctability | both | every appeal heard, and the last revision applied; undecided when no appeal was made |
| lottery | both | each draw after its commitment, its seed matching, and repeatable |
| ethicality | Leventhal | undecided: a person's judgement, and theirs |
| enforcement | A4R | a body of oversight recorded; undecided otherwise |

*Caveat.* A record that passes every decided rule is legitimately decided in the sense these rules can check; it is not thereby just (Rawls's imperfect procedural justice; the survey §1.6).

## 5. Incidence

### 5.1 Capitalisation

In spatial equilibrium a share $\kappa = \varepsilon_m / (\varepsilon_m + \varepsilon_s)$ of the value residents place on what a place-directed amount buys is capitalised into rents, for the elasticities $\varepsilon_m$ of residents' numbers to the place's attractiveness and $\varepsilon_s$ of housing to rents (the survey, Part III §3, after Roback 1982 and Kline and Moretti 2014).

**Proposition 5.1.** For $\varepsilon_m \in [m_0, m_1]$ and $\varepsilon_s \in [s_0, s_1]$, $\kappa \in [m_0/(m_0 + s_1), \, m_1/(m_1 + s_0)]$, and both bounds are attained.

*Proof.* $\partial\kappa/\partial\varepsilon_m = \varepsilon_s/(\varepsilon_m + \varepsilon_s)^2 \ge 0$ and $\partial\kappa/\partial\varepsilon_s = -\varepsilon_m/(\varepsilon_m + \varepsilon_s)^2 \le 0$, so $\kappa$ is least at $(m_0, s_1)$ and greatest at $(m_1, s_0)$, both in the box. $\square$

With value $v g$ and renters' share $s$: renters keep $(1 - \kappa) v g s$; owner-occupiers $v g (1 - s)$ (they pay the rise as occupiers and receive it as owners); landlords receive $\kappa v g s$, a declared part of it in the place. The parts sum to $v g$ for every $\kappa$, which the checks hold at both ends of the interval.

### 5.2 Re-spending

An injection $g$ whose recipients spend a share $c$ of each round's income, a share $r$ of it in the place, gives rounds $R_k = g (cr)^{k-1}$, a total $g / (1 - cr)$ for $cr < 1$, and spending outside the place $c(1 - r)$ times the total. LM3 $= (R_1 + R_2 + R_3)/R_1$ is the New Economics Foundation's measure of local retention from three rounds of a survey (Sacks 2002); the survey's example 100, 80, 40 gives 2.2. *Caveat.* Neither is a counterfactual impact: fixed shares, idle capacity, no prices.

### 5.3 Return to Generators

On a network of value flows $F$ (the economics domain's transfer network) with generators $G$, generator $g$'s return ratio is $\rho_g = (\text{value flowing to } g)/(\text{value } g \text{ sends})$ and its extraction share $\varepsilon_g = 1 - \min(\rho_g, 1)$; the positions with a net inflow that are not generators are those that capture. The economics domain's Hodge decomposition $F = c\,d_0\alpha + c\,\delta_1\gamma + c\,h$ is orthogonal in the energy $\sum F^2/c$, so the circulating share $(\lVert c\,\delta_1\gamma\rVert^2 + \lVert c\,h\rVert^2)/\lVert F\rVert^2$ is one minus the driven share; a pure cycle circulates wholly and a one-way flow not at all.

*Remark.* Reading $\rho_g$, $\varepsilon_g$ and the circulating share as measures of Eglash's generative justice is the survey's proposal (Part IV §6), not Eglash's: "unalienated" has no operational definition, and a cycle can be extractive (debt service returns to a lender). The readings are reported with the labels of control and kind they need, never alone.

### 5.4 Floor and Ceiling, and Targeting

A doughnut (Raworth 2017) counts each entity's shortfall below a declared floor and overshoot above a declared ceiling apart, never netted. Targeting sets a place-based amount $A_j$ against the people meant in each place, $m_j$ of $p_j$: the share reaching them if each place spreads its amount over its people is $\sum_j A_j m_j/p_j \,/\, \sum_j A_j$, and the people meant where nothing goes are $\sum_{j : A_j = 0} m_j$.

## 6. What the Domain Does Not Claim

- That a round, an undominated option or a legitimately recorded procedure is just. It says what follows from what was declared.
- That any amount, threshold, weight, rule or response in its examples is any authority's.
- That a language model's proposal is evidence: at the API's edge a model may propose stages, restate an objection as an appeal and draft reasons, each of grade C, and never chooses, weighs or speaks for anyone.

## 7. References

- Daniels, N. and Sabin, J. (1997). Limits to health care. *Philosophy & Public Affairs* 26(4); (2008) *Setting Limits Fairly*, 2nd ed., Oxford.
- Efraimidis, P. S. and Spirakis, P. G. (2006). Weighted random sampling with a reservoir. *Information Processing Letters* 97(5).
- Eglash, R. (2016). An introduction to generative justice. *Teknokultura* 13(2).
- Elster, J. (1992). *Local Justice*. Russell Sage.
- Fleurbaey, M. and Tungodden, B. (2010). The tyranny of non-aggregation versus the tyranny of aggregation in social choices. *Social Choice and Welfare* 35.
- Haber, S. and Stornetta, W. S. (1991). How to time-stamp a digital document. *Journal of Cryptology* 3.
- Kline, P. and Moretti, E. (2014). People, places, and public policy. *Annual Review of Economics* 6.
- Leventhal, G. S. (1980). What should be done with equity theory? In *Social Exchange*, Plenum.
- Moulin, H. (2000). Priority rules and other asymmetric rationing methods. *Econometrica* 68(3).
- O'Neill, B. (1982). A problem of rights arbitration from the Talmud. *Mathematical Social Sciences* 2.
- Ostrom, E. (1990). *Governing the Commons*. Cambridge.
- Raworth, K. (2017). *Doughnut Economics*. Random House.
- Roback, J. (1982). Wages, rents, and the quality of life. *Journal of Political Economy* 90(6).
- Sacks, J. (2002). *The Money Trail*. New Economics Foundation.
- Santos, B. de S. (1998). Participatory budgeting in Porto Alegre. *Politics & Society* 26(4).
- Shorrocks, A. F. (1983). Ranking income distributions. *Economica* 50.
- Thomson, W. (2003). Axiomatic and game-theoretic analysis of bankruptcy and taxation problems. *Mathematical Social Sciences* 45.
