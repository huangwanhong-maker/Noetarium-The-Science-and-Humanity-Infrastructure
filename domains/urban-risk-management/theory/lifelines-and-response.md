# Lifelines and Response

*Status: draft (2026-11-18). Activity: use and learning. The theory of the urban-risk-management domain's `lifelines` and `response` ([ADR 0129](../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)), on risk management's theory of criticality.*

## 1. The Network and Its Service

**Definition 1.1.** A road network is a graph $G = (V, E)$ whose vertices are junctions (points of degree other than two) and whose edges are links (chains of road between junctions), each link $e$ with a free time $\tau_e > 0$. A state $\varphi \in [0,1]^E$ gives each link the share of its speed it keeps; its time is $\tau_e / \varphi_e$, infinite when $\varphi_e = 0$.

**Definition 1.2.** Services join the network at junctions or along links, each with an offset; the time to the nearest service is $d_\varphi(v) = \min_s (o_s + \mathrm{dist}_\varphi(s, v))$, a shortest path from all services at once. A place $p$ with people $w_p$ joins across a gap; its time $t_p(\varphi)$ is the gap plus the least of its link's ends' times and offsets.

**Definition 1.3 (loss).** With a cap $c$ (the time a person cut off is counted as),

$$L(\varphi) = \sum_p w_p \bigl(\min(t_p(\varphi), c) - \min(t_p(\mathbf 1), c)\bigr) / 60,$$

person-minutes against the intact network. $L \ge 0$, since removing speed never shortens a path, and $L(\mathbf 1) = 0$.

## 2. What a Link's Closure Changes

Let $F_\varphi$ be the forest of shortest paths: each junction reached by a link from a parent junction, or seeded by a service.

**Lemma 2.1.** If link $e$ is not in $F_\varphi$ and carries no service or place, closing it changes no junction's time.

*Proof.* Closing $e$ removes paths; no time can fall. Each junction's path in $F_\varphi$ avoids $e$ and remains, so no time rises. ∎

**Proposition 2.2.** Let $e \in F_\varphi$ and let $S$ be the junctions beneath $e$ in the forest (those whose path to their service passes $e$). After closing $e$, the times outside $S$ are unchanged, and the times in $S$ are the shortest paths within $S$ from the seeds $d_\varphi(z) + \tau_{zv}/\varphi_{zv}$ for $v \in S$, $z \notin S$, and from services joining $S$ directly.

*Proof.* A junction outside $S$ keeps its forest path, which avoids $e$. A shortest path to $v \in S$ after closing $e$ leaves the services, possibly crosses junctions outside $S$ (whose times are unchanged and final), and enters $S$ for the last time through a link $zv'$ with $z \notin S$; its part from there lies in $S$. ∎

So the loss with every link closed at a state costs, besides one search, a search over each link's subtree: $\sum_e |S_e|$ junctions, which is the sum of the depths of the junctions in the forest, far below $|E|$ searches of the whole network. `check_urban_risk_management_lifelines` holds it equal to fresh searches on random lattices and states.

## 3. Criticality and Risk of Links

With scenarios $(\varphi_s, w_s)$ (risk management's §1; with maps at return periods, $w_s$ the probability that the year's largest flood falls in the band of map $s$, so that a loss per trip is expected over that flood and is no total over the year), the criticality of a link is $\sum_s w_s [L(\varphi_s^{e\downarrow}) - L(\varphi_s)]$ and its risk $\sum_s w_s [L(\varphi_s) - L(\varphi_s^{e\uparrow})]$. The first ranks what matters if it fails, in dry years and in floods alike; the second what a hazard costs through that link now, and what protecting it would save.

**Proposition 3.1 (redundancy).** The least number of links whose closure cuts an area $A$ from every service equals the greatest number of link-disjoint paths from $A$ to the services' junctions (Menger 1927), the value of a unit-capacity flow, and the links of a least cut realise it.

## 4. Coverage

A station covers a place within a threshold $\theta$ when $t_p \le \theta$ along the roads at their state. For a set $S$ of new sites added to existing stations $E$, over scenarios,

$$f(S) = \sum_s w_s \sum_p w_p \,[\exists k \in E \cup S : t_{p,k}(\varphi_s) \le \theta].$$

**Proposition 4.1.** $f$ is monotone and submodular.

*Proof.* For each scenario and place, the indicator that some station of $E \cup S$ covers it is a coverage function of $S$; coverage functions are monotone and submodular, and non-negative sums keep both. ∎

So the greedy choice reaches at least $1 - 1/e$ of the best $k$ sites (Nemhauser, Wolsey and Fisher 1978), with the bound calx computes from the instance.

*Caveat.* Coverage within a threshold is not the time to arrive under congestion, nor the chance a station is free when called (the queueing of Larson's hypercube model); both are open.
