# Markets: Buyers, Offers and the Kernel Between Them

*Status: draft (2026-11-11). [ADR 0092](../../../development/decisions/0092-the-markets-domain-and-agora.md). The formalism of the domain's first three modules, `market`, `sizing` and `space`; each result names the check that holds it.*

It serves **learning** and **use**.

## 1. A Market

**Definition 1.1.** A *market* is $M = (B, \mu, q;\ O, o_0;\ K)$, where

- $(B, \mu)$ is a finite measure space of *buyers*: cells $b$ of mass $\mu_b \ge 0$ (people, households or firms);
- $q : B \to \mathbb{R}_{\ge 0}$ is the *spend* a buyer of a cell brings in a period, and $w_b = \mu_b q_b$ is what the cell brings;
- $O$ is a finite set of *offers*, each sold by a *seller* $\sigma(o)$, and $o_0 \notin O$ is the *outside option*, not buying;
- $K$ is a sub-stochastic kernel from $B$ to $O$: $K(b,o) \ge 0$ and $\sum_{o \in O} K(b,o) \le 1$. Its defect $K(b, o_0) = 1 - \sum_o K(b,o)$ is the share of the cell's spend that is not spent in this market.

A kernel says where it comes from: *observed* (transactions), *stated* (a survey) or *presumed* (a model with declared parameters). The three are kept apart, as Cosmopoiesis keeps observed, modelled and assimilated readings apart.

*Remark.* Financial markets (order books, the pricing of assets) are not this object. Nor is a market here a price mechanism: prices and their theory (revealed preference, Slutsky, general equilibrium) are the economics domain's planned `prices`.

**Definition 1.2.** The *demand* of an offer, and of the outside option, are
$$D_o = \sum_{b} w_b\, K(b,o), \qquad D_0 = \sum_b w_b\, K(b, o_0).$$
The *share* of an offer is $s_o = D_o / \sum_{o'} D_{o'}$, and the *penetration* of the market is $1 - D_0 / \sum_b w_b$. A seller's demand is the sum over its offers.

**Proposition 1.3 (conservation).** $\sum_{o \in O} D_o + D_0 = \sum_b w_b$.

*Proof.* Each row of $K$, with its defect, sums to one. $\square$ (`check_market`: "the demands of the offers and of not buying add to what the buyers bring".)

**Structure.** Concentration among sellers is nomos's: the Herfindahl–Hirschman index $H = \sum_\sigma s_\sigma^2 \in [1/n, 1]$, the share of the $k$ largest, and the *numbers equivalent* $1/H$, the number of equal sellers with that index (Adelman 1969).

## 2. Trade Areas

**Definition 2.1.** The *trade area* of an offer $o$ is the measure $t^o$ on $B$ with $t^o_b = w_b K(b,o)$. Its total is $D_o$.

*Remark.* A trade area is a density, not a polygon. The polygons drawn in practice (the "primary trade area holding 60% of custom") are reductions of it, and are computed and drawn as such.

**Definition 2.2.** For $p \in [0,1]$, a *core* of the trade area at the share $p$ is a function $x : B \to [0,1]$ (the part of each cell taken) with $\sum_b x_b t^o_b = p\,D_o$ and the least mass $\sum_b x_b \mu_b$.

**Proposition 2.3.** Order the cells with $t^o_b > 0$ by the custom per buyer, $t^o_b / \mu_b = q_b K(b,o)$, falling. Taking cells whole in that order, and the last in part so that the share is met, gives a core.

*Proof.* The problem is a linear programme: minimise $\sum x_b \mu_b$ subject to $\sum x_b t_b = pD_o$, $0 \le x \le 1$. It is the continuous knapsack problem, whose optimum takes items in the order of value per weight (Dantzig 1957): if $x$ takes a part of a cell $c$ while leaving a part of a cell $b$ with $t_b/\mu_b > t_c/\mu_c$, moving custom $\epsilon$ from $c$ to $b$ changes the mass by $\epsilon(\mu_b/t_b - \mu_c/t_c) < 0$. $\square$ (`check_market`: against every subset of nine cells, the last in part, over forty markets.)

**Definition 2.4.** The *overlap* of two offers that sell something is
$$\omega(o, o') = \sum_b \min\!\left(\frac{t^o_b}{D_o}, \frac{t^{o'}_b}{D_{o'}}\right) = 1 - d_{TV}\!\left(\frac{t^o}{D_o}, \frac{t^{o'}}{D_{o'}}\right).$$
It is symmetric, $1$ exactly when the two offers draw from the same buyers in the same proportions, and $0$ exactly when they have no buyers in common.

**Definition 2.5.** The *unserved spend* of a cell is $u_b = w_b K(b, o_0)$; $\sum_b u_b = D_0$. *White space* is where it is large.

## 3. Segments

**Definition 3.1.** A *segmentation* is a partition $\pi$ of $B$ into blocks $\beta$. The market *coarsened* to $\pi$ is $M/\pi$, with buyers the blocks,
$$\mu_\beta = \sum_{b \in \beta} \mu_b, \qquad q_\beta = \frac{W_\beta}{\mu_\beta}, \quad W_\beta = \sum_{b\in\beta} w_b, \qquad K_\beta(o) = \frac{1}{W_\beta} \sum_{b \in \beta} w_b K(b,o).$$

**Proposition 3.2.** $M/\pi$ has the demands of $M$, for every partition.

*Proof.* $\sum_\beta W_\beta K_\beta(o) = \sum_\beta \sum_{b\in\beta} w_b K(b,o) = D_o$. $\square$

So agreement of demands is no test of a segmentation. What a partition loses is the difference among the rows it puts together. Write $K_b \in \mathbb{R}^{O \cup \{o_0\}}$ for a cell's row with its defect, $\bar K = \sum_b w_b K_b / \sum_b w_b$ for the market's, and $W = \sum_b w_b$.

**Proposition 3.3 (Pythagoras).** With
$$V = \frac{1}{W}\sum_b w_b \lVert K_b - \bar K\rVert^2,\quad V_{\text{within}}(\pi) = \frac{1}{W}\sum_b w_b \lVert K_b - K_{\beta(b)}\rVert^2,\quad V_{\text{between}}(\pi) = \frac{1}{W}\sum_\beta W_\beta \lVert K_\beta - \bar K\rVert^2,$$
$V = V_{\text{within}}(\pi) + V_{\text{between}}(\pi)$. If $\pi'$ refines $\pi$ then $V_{\text{within}}(\pi') \le V_{\text{within}}(\pi)$. $V_{\text{within}}(\pi) = 0$ exactly when $K$ is constant on every block, over the cells that bring something.

*Proof.* $K_\beta$ is the $w$-weighted mean of the rows of its block, so the cross term of $\lVert (K_b - K_\beta) + (K_\beta - \bar K)\rVert^2$ vanishes on summing over the block. Applied within each block of $\pi$ to the blocks of $\pi'$, the same identity gives $V_{\text{within}}(\pi) = V_{\text{within}}(\pi') + {}$a sum of squares. $\square$ (`check_market`: "total = within + between", "refining a partition does not raise the dispersion within".)

**Corollary 3.4.** If $V_{\text{within}}(\pi) = 0$, then for every reweighting $w'$ of the buyers the demands $\sum_b w'_b K(b,o)$ are those of the coarsened market with the blocks' new totals. The segments are then *exact*: they answer every question that weighs their buyers anew (a campaign to one block, a later year's population).

*Remark.* $V_{\text{within}}$ is what $k$-means minimises over partitions into $k$ blocks. The search is general and belongs to tyche's `multivariate`, where it is planned.

*Remark.* Exactness of a coarse description is the lumpability of physis's decision 0018, read for a kernel in place of a chain.

## 4. What If: A Choice Rule

A table of shares is one kernel. Entry, exit and a change of attraction ask for more.

**Definition 4.1.** A *choice rule* assigns a kernel $K_S$ to every set $S \subseteq O$ of offers on the market. The *logit* rule has weights $a_{bo} > 0$ for the pairs within reach, $a_{b0} \ge 0$ for the outside option, and
$$K_S(b, o) = \frac{a_{bo}}{a_{b0} + \sum_{o' \in S} a_{bo'}} \quad (o \in S),$$
with $K_S(b,\cdot) = 0$ when the denominator is zero: a cell with nothing of $S$ within its reach buys nothing.

With $a_{bo} = e^{v_{bo}}$ this is the conditional logit of utilities $v$ (McFadden 1974), which nomos's `econometrics` estimates from choices.

**Proposition 4.2 (independence of irrelevant alternatives, cell by cell).** For $o, o' \in S$ within reach of $b$, $K_S(b,o)/K_S(b,o') = a_{bo}/a_{bo'}$, whatever $S$.

**Definition 4.3.** The *diversion* from $o$ to $o'$ is the part of $o$'s demand that $o'$ gains when $o$ leaves: $\delta_{o \to o'} = (D_{o'}^{S \setminus o} - D_{o'}^{S}) / D_o^S$, and likewise $\delta_{o \to o_0}$.

**Proposition 4.4.** Under the logit rule,
$$\delta_{o \to o'} = \frac{1}{D_o} \sum_b w_b\, \frac{K(b,o)\,K(b,o')}{1 - K(b,o)}, \qquad \sum_{o' \ne o} \delta_{o \to o'} + \delta_{o \to o_0} = 1,$$
a cell with $K(b,o) = 1$ giving all of its custom to the outside option.

*Proof.* In a cell with total weight $A$ and $a = a_{bo}$, removing $o$ changes the share of $o'$ from $a'/A$ to $a'/(A - a)$, a gain of $\frac{a'}{A}\cdot\frac{a/A}{1 - a/A}$. The gains of the others and of the outside option add to the share lost, $a/A$. $\square$ (`check_market`: "diversion by the removal itself is the logit's closed form, for every offer".)

*Remark.* Across cells of different tastes the market does not have the independence of Proposition 4.2: an offer's custom goes to the offers its own buyers like. With one cell, $\delta_{o\to o'} = D_{o'}/(W - D_o)$, in proportion to shares; with two cells of opposed tastes, two offers of equal demand gain very differently (checked). This is why diversion, not share, defines who competes with whom; the diversion matrix is a weighted directed graph on the offers, and market definition is a question about its nearly closed sets (the module `competition`, planned).

## 5. The Size of a Market

### 5.1 Trees of Quantities

**Definition 5.1.** A *size* is the value at the root of a finite acyclic graph whose leaves are quantities $x_\ell \in [\underline{x}_\ell, \overline{x}_\ell] \subset \mathbb{R}_{\ge 0}$, each a datum with its source or an assumption, and whose other nodes are sums and products of their children.

**Proposition 5.2.** Every node is nondecreasing in every leaf. So the least value of the size over the box of the leaves' ranges is its value at the lows, and the greatest its value at the highs; both are attained.

*Proof.* Sums and products of nonnegative nondecreasing functions are nondecreasing. $\square$ (`check_sizing`: against every corner.)

*Remark.* The bounds are exact, including when a leaf is used in several places, which interval arithmetic in general is not. A size that divides or subtracts is not such a tree; the board's quantities (ADR 0090) carry those by interval arithmetic, with its usual widening.

**Definition 5.3.** The *elasticity* of the size $Y$ to a leaf is $\varepsilon_\ell = \partial \ln Y / \partial \ln x_\ell$ at the mids.

**Proposition 5.4.** Through a product an elasticity is passed whole; through a sum, in proportion to the term's part. Hence $\varepsilon_\ell = 1$ for every leaf of a chain of products, and the elasticities of the leaves of a sum of leaves add to one. (Computed by adjoints over the graph; `check_sizing`: against finite differences on a graph whose nodes are used twice.)

**Definition 5.5.** The *swing* of a leaf is the interval of the size when that leaf runs over its range and the others stay at their mids. A tornado chart draws the swings, widest first.

### 5.2 TAM, SAM, SOM

A chain $\mu \ge \mu_{\text{served}} \ge \mu_{\text{won}}$ of measures on the buyers gives the total, serviceable and obtainable markets as integrals of $q$. Written as a base times shares in $[0,1]$, each stage is the product of a prefix, and $\text{TAM} \ge \text{SAM} \ge \text{SOM}$ at the lows, the mids and the highs. In the notation of §1, with a kernel, $\text{SOM}$ of a seller is its demand and $\text{SAM}$ the spend of the cells within its reach.

### 5.3 Two Estimates

An estimate from the top (a population and shares) and one from the bottom (outlets and what each sells) are two ranges for one quantity. They *agree* when the ranges meet, on their intersection. When they do not, the ratio of the nearer ends is the least factor by which something assumed is wrong.

### 5.4 Buyers of a Later Year

With a population by age $N$ and a rate of buying by age $\rho$, the buyers are $\rho \cdot N$. Under a Leslie matrix $L$ (demography's `projection`) they are $\rho \cdot L^t N$ after $t$ steps.

**Proposition 5.6.** In a stable population, $N = c\,w$ with $Lw = \lambda w$, the buyers grow by $\lambda$ a step, whatever $\rho$. $\square$ (`check_sizing`.)

### 5.5 Derived Demand

What industries buy of a product is a row of an input–output table (economics' `production`): $z_{ij}$, the deliveries of sector $i$ to sector $j$, beside the final demand $y_i$; $\sum_j z_{ij} + y_i = x_i$, the sector's output. Under another final demand $y'$, with $A$ the technical coefficients and $x' = (I - A)^{-1} y'$, the deliveries are $a_{ij} x'_j$: linear in $y'$, and the table's own under $y$ (checked).

## 6. Space

The domain has no map of its own. What separates a buyer from an outlet is a *cost* $t_{bo} \ge 0$ (minutes, metres, money) of the pairs within reach, computed by a work (on urban science's `access` for streets, on a great circle for regions) and given as a sparse list.

**Definition 6.1 (Huff 1963).** *Huff's model* is the logit rule with
$$a_{bo} = A_o^{\alpha}\, f(t_{bo}), \qquad f(t) = \max(t, t_0)^{-\beta} \ \text{ or } \ e^{-\beta t},$$
$A_o > 0$ the *attraction* of an outlet and $t_0 > 0$ a floor, so that a weight is finite at the door.

In utilities it is $\alpha \ln A_o - \beta \ln t$, or $\alpha \ln A_o - \beta t$ (`check_space`: against the softmax computed apart). Choice among places and choice among products are one family.

**Proposition 6.2 (Reilly 1931; Converse 1949).** Two centres a distance $d$ apart, a buyer on the line between them at $x$ from the first, the power form with $x, d - x \ge t_0$: the shares are equal at
$$x^\ast = \frac{d}{1 + (A_2/A_1)^{\alpha/\beta}},$$
which for $\alpha = 1$, $\beta = 2$ is Reilly's law of retail gravitation. For the exponential form, $x^\ast = d/2 + \alpha \ln(A_1/A_2)/(2\beta)$.

*Proof.* $A_1^\alpha x^{-\beta} = A_2^\alpha (d - x)^{-\beta}$, respectively $A_1^\alpha e^{-\beta x} = A_2^\alpha e^{-\beta(d-x)}$. $\square$

**Proposition 6.3.** With equal attractions and distinct costs, as $\beta \to \infty$ the kernel tends to the assignment of every cell to its nearest outlet (the cells of Thiessen). $\square$

**Definition 6.4.** The *reach* of an outlet reduces its trade area to numbers: the mean cost its custom bears, $\sum_b t^o_b t_{bo} / D_o$, and for each share $p$ the least cost within which $p$ of its custom lies.

## 7. A New Outlet

Let an outlet be added at a site, of weight $u_b \ge 0$ in each cell, and let $A_b = a_{b0} + \sum_o a_{bo}$ be the weights already there.

**Proposition 7.1.** The new outlet takes the share $u_b/(A_b + u_b)$ of each cell, and every offer and the outside option lose that share of their own custom there. So its demand
$$D_{\text{new}} = \sum_b w_b \frac{u_b}{A_b + u_b} = \sum_{o} \underbrace{\sum_b w_b K(b,o) \frac{u_b}{A_b+u_b}}_{\text{drawn from } o} + \underbrace{\sum_b w_b K(b,o_0) \frac{u_b}{A_b + u_b}}_{\text{from not buying}},$$
where a cell with $A_b = 0$ counts wholly as not buying.

*Proof.* Proposition 4.2. $\square$ (`check_space`: each part is what that offer loses when the outlet is added to the rule.)

The part drawn from the seller's own offers is *cannibalisation*; what the seller gains is the demand less that.

## 8. Where to Open

A seller $\sigma$ is to choose $k$ sites of a list $C$. With $\text{own}_b$ the weight of its offers in cell $b$ and $u_b(S) = \sum_{c \in S} u_{bc}$, its demand with outlets at $S \subseteq C$ is
$$F(S) = \sum_b w_b\, \frac{\text{own}_b + u_b(S)}{A_b + u_b(S)}.$$

**Proposition 8.1.** $F$ is monotone and submodular: $F(S) \le F(T)$ and $F(S \cup c) - F(S) \ge F(T \cup c) - F(T)$ for $S \subseteq T \subseteq C$, $c \notin T$.

*Proof.* In each cell, $g(u) = (\text{own} + u)/(A + u)$ is nondecreasing and concave on $u \ge 0$, since $\text{own} \le A$; and $u_b(\cdot)$ is a nonnegative modular set function. A concave nondecreasing function of a nonnegative modular function is monotone submodular, and a nonnegative combination of such is. $\square$ (`check_space`: over two hundred pairs of sets.)

**Corollary 8.2 (Nemhauser, Wolsey and Fisher 1978).** Choosing greedily, each time the site of greatest gain, gives $S_k$ with
$$F(S_k) - F(\emptyset) \ \ge\ \left(1 - \tfrac{1}{e}\right)\left(\max_{|S| = k} F(S) - F(\emptyset)\right).$$
(`check_space`: against every three of ten sites.)

*Remark.* The bound is for this objective, the seller's demand under a presumed kernel with rivals who do not respond. Rivals who respond make it a game (nomos's `games`; the module `competition`, planned).

## 9. What Is Not Yet Here

- **Estimation.** The parameters $\alpha$, $\beta$ and the outside weights are declared, not fitted: fitting needs observed or stated choices and is nomos's conditional logit (the module `demand`, planned).
- **Quantities as distributions.** Ranges here are intervals; a size as a distribution, with the probability of each stage, waits on tyche.
- **Competition, dynamics.** Market definition on the diversion graph, positioning, entry as a game; diffusion, cohorts, retention and lifetime value.
- **Segments found, not given.** $k$-means and mixtures, in tyche.

## References

- Adelman, M. A. (1969). Comment on the "H" concentration measure as a numbers-equivalent. *Review of Economics and Statistics* 51, 99–101.
- Converse, P. D. (1949). New laws of retail gravitation. *Journal of Marketing* 14, 379–384.
- Dantzig, G. B. (1957). Discrete-variable extremum problems. *Operations Research* 5, 266–288.
- Huff, D. L. (1963). A probabilistic analysis of shopping center trade areas. *Land Economics* 39, 81–90.
- Huff, D. L. (1964). Defining and estimating a trading area. *Journal of Marketing* 28, 34–38.
- McFadden, D. (1974). Conditional logit analysis of qualitative choice behavior. In P. Zarembka (ed.), *Frontiers in Econometrics*, 105–142. Academic Press.
- Nemhauser, G. L., Wolsey, L. A. and Fisher, M. L. (1978). An analysis of approximations for maximizing submodular set functions, I. *Mathematical Programming* 14, 265–294.
- Reilly, W. J. (1931). *The Law of Retail Gravitation*. Knickerbocker Press.
