# Disaster Risk: Disruption and Evacuation

*Status: draft (2026-11-18). Activity: use and research. The theory of the disaster-risk domain's `disruption` and `evacuation` ([ADR 0129](../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)), on urban risk management's lifelines and risk management's criticality.*

## 1. A Hazard as States of a System

A hazard map gives an intensity $h(x)$ (a depth) at each point of a region. For a link $e$ of the roads, its intensity is $h_e = \max_{x \in e} h(x)$, read every few metres; a depth–disruption curve $\psi$ gives the share of speed kept, $\varphi_e = \psi(h_e)$. Maps at return periods $T_1 < \dots < T_m$ give states $\varphi^{(k)}$; with the undisturbed state and risk management's weights (its theory, Proposition 2.5) they are the scenarios of urban risk management's criticality.

*Remark.* Taking the deepest point is the worst case along a link: a road is as passable as its least passable stretch. A link long enough to be partly flooded and partly not is closed whole.

**The curves.** For cars, Pregnolato et al. (2017, eq. 4) fitted $v(w) = 0.0009 w^2 - 0.5529 w + 86.9448$ km/h for depth $w$ in millimetres, and their Table 2 takes a car to stop at 300 mm; since the quadratic's least value is about 2 km/h near 307 mm, the curve here is $v(w)/v(0)$ below 300 mm and 0 from it. For people on foot no curve of comparable standing was found; the domain's `walking_declared` (1 at no depth, 0 from 0.5 m) is a declaration.

## 2. Evacuation on Foot

### 2.1 The Network and Its Sinks

The walking network is the graph of open links, short links merged so that a cell of the crowd is not shorter than the scheme can cross in a step. Each edge $e$ has a length $\ell_e$, a usable width $b_e$ and a share $\varphi_e$ of walking speed $v$. People $P_i$ start at nodes after walking a gap; sinks are shelters with capacities $C_k$ and, if declared, safe ground with none.

### 2.2 Reach with Capacities

Let $R_k$ be the places that reach sink $k$ by walking freely within $W$. The most housed within $W$ is the value of the flow from places (capacities $P_i$) through the arcs $i \to k$ for $i \in R_k$ to sinks (capacities $C_k$).

**Proposition 2.1.** For a set $S$ of open sinks, $g(S) = $ the most housed within $W$ is monotone and submodular in $S$.

*Proof.* $g$ is the value of a flow to the sinks of $S$ in a bipartite network with capacities. By the max-flow min-cut theorem, with $\Gamma(T)$ the places that reach some sink of $T$,
$$g(S) = \min_{T \subseteq S} \Bigl( \sum_{k \in S \setminus T} C_k + \sum_{i \in \Gamma(T)} P_i \Bigr),$$
a cut keeping the sinks of $T$ on the sink's side and cutting the places that reach them. This is the convolution of the modular function $C$ with the weighted coverage $T \mapsto P(\Gamma(T))$, which is monotone and submodular; such a convolution is the rank function of a polymatroid, the one induced through the bipartite graph (Fujishige 2005, §2.3 and §3.1), and so monotone and submodular. ∎

`check_disaster_risk_evacuation` verifies the inequality on every nested pair of sets and element of random instances. So calx's greedy choice of $k$ new shelters reaches $1 - 1/e$ of the best, with a bound from the instance.

### 2.3 The Bound

Divide time into steps of $\Delta$. Let each edge take $\tau_e = \lfloor \ell_e / (v \varphi_e \Delta) \rfloor$ steps and pass at most $u_e = b_e q_{\max} \Delta$ people a step ($q_{\max}$ the capacity per metre of width, per second); people released in step $j$ (their departure in $(j\Delta, (j+1)\Delta]$, plus their walk to the network rounded down) enter at step $j$; anyone may wait at a node; each sink absorbs up to its capacity.

**Proposition 2.2.** The greatest flow over time in this network by step $k$ bounds from above the number who can be safe by time $k\Delta$, under any routing in which no one walks faster than $v\varphi_e$, no edge passes more than $b_e q_{\max}$ a second, and no one leaves earlier than their departure.

*Proof.* Any such continuous evacuation, sampled at the steps, gives a feasible flow of the discrete network: each traversal of an edge takes at least $\ell_e / (v\varphi_e) \ge \tau_e \Delta$, so arriving $\tau_e$ steps after entering is no later than the truth, and what arrives early waits at the node; departures are moved to the start of their step; capacities per step are the rates times the step. The discrete greatest flow is at least the value of any feasible flow. ∎

The bound with everyone released at once and the bound with the people's departures differ by what departure costs; each is computed on one time-expanded network (morphe's `flow_over_time`), whose value at every step is the greatest for that step.

### 2.4 The Simulation

The crowd is a density $\rho$ per metre of each edge obeying $\partial_t \rho + \partial_x q(\rho) = 0$ with the triangular flux $q(\rho) = \min(v\varphi\rho,\ w(\rho_j b - \rho))$, $q_{\max} b$ at $\rho_c = q_{\max} b/(v\varphi)$, so that $w = q_{\max} b / (\rho_j b - \rho_c)$ (physis's kinematic waves; Daganzo's cell transmission model). At every node, all that arrives turns into the edge of the tree of shortest walks towards the nearest sink with room; a sink admits until full, and the tree is rebuilt when one fills. People enter through one cell at their node as their departures release them, and leave through one at their sink. Godunov's scheme conserves people to rounding, which the domain checks.

*Caveat.* The scheme is first order: a crowd's front diffuses ahead at the scheme's speed, so the simulated curve may exceed the bound by a little, and its rear is smeared, so the time when all are safe runs later than the people's own spread. Clearance is therefore reported at a declared share (99%), beside the time when all but a millionth are safe.

### 2.5 What the Comparison Says

With $B_0(t)$ the bound for everyone leaving at once, $B(t)$ the bound with the people's departures and $S(t)$ the simulation, $B_0 - B$ is what departure costs and $B - S$ what routing to the nearest shelter and crowding cost. A plan that moves departures earlier (warnings, drills) acts on the first; one that opens shelters, widens streets or guides people to other routes acts on the second.
