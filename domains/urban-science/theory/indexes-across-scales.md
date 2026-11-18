# The Indexes of Cities Across Scales

*Status: draft (2026-11-18; [ADR 0128](../../../development/decisions/0128-indexes-and-cities-across-scales.md)). Activity: research, in service of use.* How the indexes of a city behave on a hierarchy of units, what a model across scales may and may not do with them, and what the first case shows. The survey it rests on is `research/literature/reports/Indexes of cities across scales.md`; the mathematics of hierarchies is kinesis's (its theory note, §9, and decision 0007), the estimators tyche's (decision 0028).

## 1. Units, Levels and Indexes

A city's units form a hierarchy $\mathcal H$: levels $L_0, \dots, L_m$ (blocks, areas, the city), each unit of a level below the top with one parent above, $\pi : L_\ell \to L_{\ell+1}$. A record gives quantities $x : L_0 \to \mathbb R$ on the finest level at some times. An index is a map from quantities to values, and an index *at a level* $\ell$ is that map applied to what the units of $\ell$ hold.

**Definition 1.1.** An index $I$ is *extensive* when $I_{\ell+1} = \Sigma I_\ell$; a *ratio of sums* when $I = a/b$ with $a, b$ extensive; *decomposable* when $I_{\ell+1}(v) = W(v) + B(v)$, a within term that is a weighted mean of the children's $I_\ell$ and a between term computable from the children's sufficient statistics; and *to be recomputed* otherwise.

**Proposition 1.2.** A ratio of sums at a coarse unit is the weighted mean of its children's ratios with the denominators as weights, and not their plain mean unless the denominators are equal (kinesis's theory note, Proposition 9.3). Gini's coefficient, the dissimilarity index, the HHI and a median are not functions of the children's values of the same index; they are recomputed from the units beneath. Theil's T and L, the generalised entropies, the segregation indices H and M, and Hill numbers decompose (Shorrocks 1980; Theil 1972; Jost 2007).

*Remark.* The rule is old and still broken: the mean of tract rates published as a city's rate. In Melbourne in 2024 the city's floor vacancy rate, unused floor over all floor, is 6.3%; the plain mean of its blocks' rates would say 5.7%.

## 2. Indexes a Record Does Not Hold

An index is **observed** when a record gives it at the units and times a model needs; **estimated** when a declared estimator gives it with an uncertainty it has shown; **declared** when a person states it, with a range; **modelled** when another model gives it. A model across scales lists what it needs and has not been given and refuses to run until each is supplied (kinesis decision 0007).

**A census's withheld cells.** CLUE leaves blank a cell that pertains to two or fewer businesses while keeping the row's total. A blank cell lies in $[0, R]$, with $R$ the row's total less what is stated: the census's own bound, whatever is assumed. Within it, a dasymetric carriage (tyche's) shares $R$ among the row's blank cells by prior weights (the block's floor of that use in the nearest year it was stated, else its area's mix), keeping every row's total exactly. In 2002–2024, 30.0% of all the floor space counted is withheld.

**An index where it was not observed.** An estimator is judged on blocks of units held out together, its intervals are conformal from those errors, and an estimate is withheld where the estimator does not beat the mean, where the unit is outside its area of applicability (Meyer and Pebesma 2021), or where its interval is wider than a declared share of it (statistical offices withhold an estimate whose relative standard error passes 50%). A withheld estimate is a quantity to declare.

## 3. Models Across Scales

A model's quantities are indexes on levels; its operators carry them between levels (sums and reductions up; a parent's value, shares and logits down); its laws advance states each at its pace. Two forms are offered for a city's floor space.

**Where growth goes.** The city's change in the floor of a use, $\Delta F_t$, is given. It is shared among areas by $s_a \propto (F_a + 1)^\alpha$ and within an area among blocks by $s_b \propto (f_b + 1)^\beta\,(\bar f_{N(b)} + 1)^\gamma\,e^{-\delta d_b}$, with $\bar f_{N(b)}$ the mean of the block's neighbours and $d_b$ its distance from the centre: $f_b \leftarrow f_b + s_b\, s_{a(b)}\, \Delta F_t$. With $\alpha = \beta = 1$, $\gamma = \delta = 0$ every unit keeps its share (proportional allocation), so the model nests the null of no change in where growth goes. By Proposition 9.3 of kinesis's note the blocks' change sums to the city's.

**The vacancy cycle** (after DiPasquale and Wheaton 1996). The city's occupied floor $D_t$ is given and shared among areas in proportion to $S_a^\gamma$; an area's gap $g_a = 1 - O_a/S_a$ (negative where demand exceeds its stock) and its vacancy $\max(g_a, 0)$; its rent relative to the cost of building, $\rho_a$, which no record here holds, follows $\dot\rho_a = -\lambda (g_a - v^*)\rho_a$; its stock $\dot S_a = \kappa \max(\rho_a - 1, 0) S_a$. Each block reads its area's vacancy.

## 4. What Melbourne's Record Shows

On CLUE's 603 blocks in 13 small areas, 2002–2024, fitted on 2002–2016 and judged on 2017–2024:

- **Growth comes in buildings.** The 5% of blocks that gained most took 70% of the homes added in 2016–2024 and 90% of the office floor. A model of shares spreads growth, and so places the change among blocks no better than chance (a correlation of 0.05 for homes, fitted or proportional) and errs by more than standing still. Among the areas it places part of it (0.35 for homes, 0.67 for offices, against 0.39 and 0.59 for proportional allocation). The next form is development as discrete events with a size, ranked by suitability under capacities (the constrained cellular automata and CLUE-S of the survey's Part I §3).
- **The vacancy cycle is not identified.** Thirteen areas' stock and vacancy over fourteen years do not determine its four rates (the normal matrix is singular at the optimum, with the stock's response at its bound). They are declared, with ranges, and the model is run over draws of every declaration: the screening ranks how demand among areas follows their stock, then how fast rents answer vacancy, as the declarations that move the city's vacancy most. The declared ranges given as defaults make the median vacancy overshoot the record; the declarations, not the record, carry that result.

Neither is a forecast of the city: the city's change is given, and what is declared is said to be.

## 5. References

DiPasquale and Wheaton (1996), *Urban Economics and Real Estate Markets*; Harris and Wilson (1978), Environment and Planning A 10, 371–388; Jost (2007), Ecology 88, 2427–2439; Meyer and Pebesma (2021), Methods in Ecology and Evolution 12, 1620–1633; Shorrocks (1980), Econometrica 48, 613–625; Theil (1972), *Statistical Decomposition Analysis*; White and Engelen (1993), Environment and Planning A 25, 1175–1199. The survey's parts give the rest, with their markers.
