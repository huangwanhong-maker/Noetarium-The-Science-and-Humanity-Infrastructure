# The Urban Forest: What Is Computed, and Why It Holds

*Status: draft (2026-11-13). The ecology domain's first note ([ADR 0101](../../../development/decisions/0101-ecology-founded-the-urban-forest.md)). Activities: research, learning.*

The domain's first object is a forest of individual trees in a city: counted, named, measured once, assessed by arborists, planted by people. This note states what the domain computes of it and proves what holds. It is the reference of the modules `taxonomy`, `biodiversity`, `populations`, `forest` and `connectivity`, and of the Urban Studio's ecology plug-in, whose tools say their results plainly and leave the propositions here.

## 1. Concepts

### 1.1 A Forest as an Inventory

**Definition 1.1.** An *inventory* is a finite set $I$ of trees. Each tree $i$ carries a taxon $\tau_i = (s_i, g_i, f_i)$ (species, genus, family, any of them possibly unknown), a diameter at breast height $D_i > 0$ in cm (possibly unknown), an area $a_i$ (possibly none), an age $A_i$ (possibly unknown) and a stated remaining life, an interval $[\ell_i, u_i]$ with $u_i = \infty$ for an open class (possibly none).

The classification is a tree: a species lies in one genus, a genus in one family. A record that gives one species two genera is refused (`taxonomy::species_list`).

*Remark.* Nothing in an inventory is a rate. Growth, removal and planting enter as presumptions, each declared by whoever runs the analysis, and the answers say which.

### 1.2 Presumed, Not Observed

Three kinds of statement are kept apart in every answer:

- **counted**: tallies, shares and Hill numbers of the record;
- **estimated**: what a fuller count would find, with an interval from the sampling model;
- **presumed**: carbon from allometry, the future from declared rates.

The tools phrase the third kind with the words *Presumed, not observed*, and a board receives it as an assumption, not as a finding.

## 2. Composition and Its Limits

**Definition 2.1.** For a rank $r$, the *tally* $T_r$ counts the trees named at $r$ by name; the unnamed at $r$ are counted apart and are no class. The *share* of a name is its count over the trees named at $r$.

**Definition 2.2.** A *scheme of limits* is $(\lambda_s, \lambda_g, \lambda_f)$. The limit *holds* at rank $r$ when no share exceeds $\lambda_r$. Santamour (1990) proposed $(0.10, 0.20, 0.30)$.

*Remark.* A share equal to its limit holds. Excluding the unnamed from the denominator makes the share the composition of what is known; including them would dilute it by ignorance. The count of the unnamed is reported beside every finding.

**Example (Melbourne, the City's record of 2025-09-22).** Of 82,064 trees, *Eucalyptus camaldulensis* is 10.3% of the 81,102 named at the species, *Eucalyptus* 21.6% and Myrtaceae 36.0% of the trees named at those ranks: every rank fails Santamour's limits, by the smallest margin at the species.

## 3. Diversity, Counted and Estimated

The diversity of a law $p$ over classes is the family of Hill numbers ${}^qD(p) = (\sum_i p_i^q)^{1/(1-q)}$ (Hill 1973), computed with its estimators by tyche's `classes` (its decision 0017). Two points are the domain's.

**Proposition 3.1 (Comparison at a common coverage).** Let samples $X^{(1)}, \dots, X^{(m)}$ be compared by ${}^qD$ at a common sample coverage $C$, each rarefied or extrapolated to the size $n_k(C)$ at which its expected coverage reaches $C$. Then the comparison does not favour a sample for its size alone.

*Proof sketch.* Coverage is the probability mass of the classes a sample holds (Good 1953; Chao and Jost 2012). At equal coverage two samples have left out equal masses of their assemblages, so a difference of ${}^qD$ is not a difference of completeness. At equal size a larger, more even assemblage has left out more, and comparing at size confounds the two (Chao and Jost 2012, §2). ∎

The common coverage is set by the areas with at least a stated number of trees (50 by default), as the least of their coverages at twice their size, so that no area is extrapolated beyond $2n$, where the estimators are unreliable (Chao et al. 2014). Smaller areas are shown at their own coverage only.

**Proposition 3.2 (Partition with equal weights).** With equal weights Jost's $\beta = {}^qD_\gamma / {}^qD_\alpha$ lies in $[1, N]$, equals 1 exactly when the areas have one composition, and equals $N$ when they share no class.

*Proof.* Jost (2007), Theorems 1–2, for equal weights and every $q \ge 0$; checked in `check_ecology_biodiversity` on identical and disjoint areas. ∎

*Caveat.* Weighting by size mixes how many trees an area holds into how different it is: two areas with one composition at different sizes then have $\beta > 1$ (tyche's `partition.hpp`). The domain weights equally.

## 4. Taxonomic Distinctness

**Definition 4.1.** With three levels above the species, the weight of a pair of species is $\omega = 100/3$ in one genus, $200/3$ in one family and $100$ otherwise; a rank unknown for either counts as different. $\Delta^+$ is the mean of $\omega$ over the pairs of species present (Clarke and Warwick 1998).

**Proposition 4.2.** If $s$ species are drawn at random without replacement from a list of $S$, $E[\Delta^+] = \Delta^+_S$, the list's own.

*Proof.* Each of the $\binom{S}{2}$ pairs is drawn with the same probability $\binom{S-2}{s-2}/\binom{S}{s}$, so the expected sum of $\omega$ over the drawn pairs is $\binom{s}{2}/\binom{S}{2}$ times the list's, and dividing by $\binom{s}{2}$ gives $\Delta^+_S$. ∎

The funnel of 2.5% and 97.5% quantiles is by simulation, reproducible by its seed; Proposition 4.2 is checked exactly, by listing all $\binom{8}{3} = 56$ subsets of a list. An area below its funnel holds species more closely related than chance: one pest of a genus would take more of it.

## 5. Carbon by Declared Allometry

**Definition 5.1.** A *scheme of allometry* assigns each taxon to a group by its species, else its genus, else its family, else a fallback, and gives each group $c\,e^{\beta_0} D^{\beta_1}$ for above-ground dry biomass (Jenkins et al. 2003, Table 4), a ratio $e^{\beta_0 + \beta_1/D}$ for coarse roots (their Table 6), and a stated adjustment $c$ (0.8 for open-grown trees; Nowak 1994). Carbon is a stated fraction of dry biomass; CO₂ is 44/12 of carbon.

Every estimate reports the rank at which it matched and whether $D$ lies beyond its source's range. Beyond the largest diameter the source measured, the scheme declares whether the power law is carried on or the tree is estimated at that largest diameter. Diameters beyond a stated plausibility bound (250 cm by default) are taken for errors of the record, set aside and counted.

*Caveat.* Jenkins et al. fitted North American forest trees. In Melbourne the equations matched 14,131 trees at the genus, 453 at the family and 22,588 by the fallback; 5,625 lay beyond their source's diameters. The 19,969 t of carbon reported for the 37,172 trees with a diameter is a presumption of that scheme. It is not a measurement, and it says nothing of the 44,892 trees the record leaves unmeasured.

## 6. The Forest's Future

### 6.1 Exact Moments

**Assumptions.** Tree $i$ grows by a constant increment $\Delta_i$, so that $D_i(t) = D_i + \Delta_i t$, and is removed at a time $L_i$, the $L_i$ independent:

- $L_i \sim U[\ell_i, u_i]$ for a stated interval;
- $L_i = \ell_i + G$, with $G$ geometric of hazard $h$, for an open class;
- $L_i \sim$ geometric of hazard $h$ when no life is stated.

**Proposition 6.1.** Let $p_i(t) = P(L_i > t)$, $c_i(t) = C(D_i(t))$, $N(t) = \sum_i 1\{L_i > t\}$ and $C(t) = \sum_i c_i(t) 1\{L_i > t\}$. Then

$$E N(t) = \sum_i p_i(t), \quad \operatorname{Var} N(t) = \sum_i p_i(t)(1 - p_i(t)), \quad E C(t) = \sum_i c_i(t) p_i(t), \quad \operatorname{Var} C(t) = \sum_i c_i(t)^2 p_i(t)(1 - p_i(t)),$$

with $p_i(t) = \operatorname{clamp}((u_i - t)/(u_i - \ell_i), 0, 1)$ for a stated interval, $1$ for $t < \ell_i$ and $(1-h)^{t - \ell_i}$ after for an open class, and $(1 - h)^t$ for none.

*Proof.* Each indicator $1\{L_i > t\}$ is Bernoulli with mean $p_i(t)$, and the $c_i(t)$ are deterministic. Expectation is linear, and the variance of a sum of independent terms is the sum of their variances, $c^2 p(1-p)$ for a scaled Bernoulli. The forms of $p_i$ are the survival functions of the three laws. ∎

The band reported is $E \pm 1.96\,\mathrm{sd}$. It is the removals' chance alone, given the presumptions; it holds no uncertainty of the rates themselves. No Monte Carlo is needed: the answer is the same at every run, and immediate for a city's trees over decades. `check_ecology_forest` holds $E C(7)$ against 40,000 simulated removals (0.7 standard errors) and $\mathrm{sd}\,C(7)$ to 2%.

### 6.2 Planting, and the Planting That Holds

$P$ trees a year of a mix $(w_k)$, planted at diameter $d_0$ and removed with hazard $h_p$, add to $E C(t)$ the amount $P \sum_k w_k \sum_{\tau=1}^{t} c_k(d_0 + \Delta_k(t-\tau))(1-h_p)^{t-\tau}$, which is linear in $P$.

**Corollary 6.2.** The planting that brings $E C(H)$ back to $E C(0)$ is $P^* = (E C_0(0) - E C_0(H)) / E C_1^{\mathrm{planted}}(H)$, with $C_0$ the forest unplanted and $C_1^{\mathrm{planted}}$ what one tree a year adds. It is negative exactly when the forest gains carbon unplanted. ∎

### 6.3 The Stage View

**Definition 6.3.** In diameter classes $[b_j, b_{j+1})$, a class's survival is $\sigma_j = 1 - 1/\bar L_j$, the constant hazard with the mean expected remaining life $\bar L_j$ of its trees. Its growth is $\gamma_j = \min(1, \bar\Delta_j / (b_{j+1} - b_j))$ (Usher 1966, 1969), and it has no recruitment. The projection matrix $A = T$ is that of `populations/stages.hpp`.

*Rationale.* A stated life's first year alone would give $\sigma_j \approx 1$ (nearly every tree survives a year), which is true of a year and false of a life. The constant hazard with the same mean life is the one-parameter law that keeps the expected life of a class's trees.

**Proposition 6.4.** For a population with no recruitment, $\lambda = \rho(T) = \max_j \sigma_j(1 - \gamma_j)$, the survival of the longest-lived class that keeps its members.

*Proof.* $T$ is lower bidiagonal, so its eigenvalues are its diagonal entries $\sigma_j(1 - \gamma_j)$. ∎

So λ says little of a planted forest. What matters is what a planting sustains.

**Proposition 6.5 (What a constant input sustains, and its sensitivities).** If $\rho(A) < 1$ and $b \ge 0$, then $n(t+1) = A n(t) + b$ converges from every start to $n^* = N b$, $N = (I - A)^{-1}$. The total $\mathbf 1^\top n^*$ has, for any rate $x$ on which $A$ depends,

$$\frac{\partial (\mathbf 1^\top n^*)}{\partial x} = (N^\top \mathbf 1)^\top \frac{\partial A}{\partial x}\, n^* .$$

*Proof.* The error $e(t) = n(t) - n^*$ satisfies $e(t+1) = A e(t)$, so $e(t) = A^t e(0) \to 0$ when $\rho(A) < 1$, since $A^t \to 0$ then (Horn and Johnson 2013, Theorem 5.6.12). Differentiating $(I - A) N = I$ gives $\partial N/\partial x = N (\partial A/\partial x) N$, and $\mathbf 1^\top (\partial N/\partial x) b = (\mathbf 1^\top N)(\partial A/\partial x)(N b)$. ∎

The vector $N^\top \mathbf 1$ holds the expected steps a member of each stage has left (calx's `expected_time_to_leave`; Kemeny and Snell 1960, ch. III). A rate's sensitivity is the members it acts on times the life it adds or takes. `check_ecology_populations` holds the formula against central differences to $10^{-5}$ of the total.

**Example.** For Melbourne's forest in classes of 0–15, 15–30, 30–45, 45–60, 60–90 and ≥ 90 cm, with 3,000 trees planted a year, about 105,000 trees are sustained, and the total answers most to the survival of the youngest class: one per cent more of it keeps 7.1% more trees standing.

## 7. Connectivity of Habitat

### 7.1 Patches

Patches $i$ carry attributes $a_i$. The probability of connectivity $PC = \sum_{i,j} a_i a_j p^*_{ij} / A_L^2$ uses $p^*$, the greatest product of direct probabilities $e^{-k d}$ along any path (Saura and Pascual-Hortal 2007). It is a shortest path in the weights $k d$ (morphe's Dijkstra).

**Proposition 7.1.** The importance $dPC_m$ splits as $\mathrm{intra}_m + \mathrm{flux}_m + \mathrm{connector}_m$, and $\mathrm{connector}_m \ge 0$.

*Proof.* Removing $m$ removes the terms $a_m^2$ (intra) and $2\sum_{j \ne m} a_m a_j p^*_{mj}$ (flux), and can only lower $p^*_{ij}$ for $i, j \ne m$, since paths through $m$ are lost and none are gained. The remainder, the loss of the pairs $i, j \ne m$, is non-negative (Saura and Rubio 2010). ∎

`check_ecology_connectivity` holds the connector of a stepping stone equal to $100 \cdot 2 a_0 a_2 (p^*_{02} - p_{02})/\mathrm{num}$ exactly.

### 7.2 Circuits

On a grid of resistances, the effective resistance between two patches (each joined as one node) is the voltage across them when a unit current flows (McRae 2006). It is solved by physis's Kirchhoff networks, to a relative residual of $10^{-10}$ (physis's decision 0027).

**Proposition 7.2.** The effective resistance is at most the series resistance of any one path between the patches, in particular of the least-cost path, and it obeys the triangle inequality.

*Proof.* Rayleigh's monotonicity law: removing edges cannot lower an effective resistance, and a single path's resistance is its series sum (Doyle and Snell 1984, §1.4). Effective resistance is a metric on the nodes (Klein and Randić 1993). ∎

Both are checked on a random landscape, and the strip's closed forms, $(L-1)/w$ with four neighbours and $(L-1)/(w + 2(w-1)/\sqrt2)$ with eight, hold to $10^{-9}$.

*Caveat.* The resistance surface (linear in canopy cover between declared ends) and the median dispersal are presumptions standing for no species in particular. A species' own surface and its dispersal would replace them; the tool states both.

## 8. What Comes Next

- **The life around the trees:** birds and bats from repeated surveys (peira's occupancy is in the plug-in; a city's own survey data are wanted), and species distributions as point processes (tyche's spatial statistics).
- **Repeated inventories:** growth and mortality measured from two records of a city, replacing the increments one inventory suggests.
- **Allometry of a southern city's trees:** the eucalypts, the planes and the elms of Melbourne by equations fitted to them, as a second declared scheme.
- **The material stock of buildings:** a city's mass by its buildings' footprints, heights and material intensities, and its flows by lifetime laws ([ADR 0101](../../../development/decisions/0101-ecology-founded-the-urban-forest.md), §5).

## 9. References

- Chao, A. and Jost, L. (2012), Coverage-based rarefaction and extrapolation, *Ecology* 93, 2533–2547.
- Chao, A., Gotelli, N. J., Hsieh, T. C. et al. (2014), Rarefaction and extrapolation with Hill numbers, *Ecological Monographs* 84, 45–67.
- Clarke, K. R. and Warwick, R. M. (1998), A taxonomic distinctness index and its statistical properties, *Journal of Applied Ecology* 35, 523–531.
- Doyle, P. G. and Snell, J. L. (1984), *Random Walks and Electric Networks*, Mathematical Association of America.
- Good, I. J. (1953), The population frequencies of species and the estimation of population parameters, *Biometrika* 40, 237–264.
- Hill, M. O. (1973), Diversity and evenness: a unifying notation and its consequences, *Ecology* 54, 427–432.
- Horn, R. A. and Johnson, C. R. (2013), *Matrix Analysis*, 2nd ed., Cambridge University Press.
- Jenkins, J. C., Chojnacky, D. C., Heath, L. S. and Birdsey, R. A. (2003), National-scale biomass estimators for United States tree species, *Forest Science* 49, 12–35.
- Jost, L. (2007), Partitioning diversity into independent alpha and beta components, *Ecology* 88, 2427–2439.
- Kemeny, J. G. and Snell, J. L. (1960), *Finite Markov Chains*, Van Nostrand.
- Klein, D. J. and Randić, M. (1993), Resistance distance, *Journal of Mathematical Chemistry* 12, 81–95.
- McRae, B. H. (2006), Isolation by resistance, *Evolution* 60, 1551–1561.
- Nowak, D. J. (1994), Atmospheric carbon dioxide reduction by Chicago's urban forest, in *Chicago's Urban Forest Ecosystem*, USDA Forest Service Gen. Tech. Rep. NE-186, 83–94.
- Santamour, F. S. Jr. (1990), Trees for urban planting: diversity, uniformity, and common sense, *Proceedings of the 7th Conference of the Metropolitan Tree Improvement Alliance* 7, 57–65.
- Saura, S. and Pascual-Hortal, L. (2007), A new habitat availability index to integrate connectivity in landscape conservation planning, *Landscape and Urban Planning* 83, 91–103.
- Saura, S. and Rubio, L. (2010), A common currency for the different ways in which patches and links can contribute to habitat availability and connectivity in the landscape, *Ecography* 33, 523–537.
- Usher, M. B. (1966), A matrix approach to the management of renewable resources, with special reference to selection forests, *Journal of Applied Ecology* 3, 355–367.
- Usher, M. B. (1969), A matrix model for forest management, *Biometrics* 25, 309–315.
