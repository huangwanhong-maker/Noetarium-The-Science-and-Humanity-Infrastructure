# A Community's Greenhouse Gas Inventory and Its Mitigation

*Status: draft (ADR 0147). The theory note of the industrial-ecology domain's `emissions`.*

The module reads an inventory in the structure of the Global Protocol for Community-Scale Greenhouse Gas Emission Inventories (GPC; WRI, C40 and ICLEI 2014). It says what each total contains and how certain it is. It then credits measures against the inventory without counting what two of them save from one source twice. Every factor, potential, rate, cost and target is the caller's declaration.

## 1. Entries, Cells and Levels

**Definition 1 (entry).** An entry e has a sector σ(e) ∈ {stationary energy, transportation, waste, IPPU, AFOLU, other scope 3} and a scope s(e) ∈ {1, 2, 3}. It also has a gas g(e), and either an estimate or a notation key: NO not occurring, IE included elsewhere (naming the entry that holds it), NE not estimated, C confidential. An estimate is activity × factor (kg of the gas per unit) or a stated amount. With a declared potential GWP_g, its emissions are x_e = kg_e · GWP_g / 1000 tonnes CO2e.

Scope 1 is emitted within the boundary. Scope 2 comes from grid-supplied energy consumed within it, and so arises only in stationary energy and transportation. Scope 3 is emitted outside the boundary because of activities within it.

**Definition 2 (levels).** For a cell c = (σ, s), the levels are the sums of x_e over entries whose cell lies in:
- BASIC = {(I, 1), (I, 2), (II, 1), (II, 2), (III, 1), (III, 3)};
- BASIC+ = BASIC ∪ {(I, 3), (II, 3), (IV, 1), (V, 1)};
- territorial = every cell of scope 1.

Here I to V are the first five sectors. Three kinds of entry are treated apart:
- scope 1 emissions of generation supplied to the grid count in the territorial total and in neither BASIC level, since they reach consumers as scope 2;
- scope 1 emissions of waste from outside count in the territorial total and in neither BASIC level, since they are another community's scope 3;
- biogenic CO2 counts in no total and is reported apart.

**Definition 3 (completeness).** A level is complete when each of its cells has at least one entry and none of them is NE. An entry keyed NO, IE or C accounts for its cell.

*Remark.* Completeness is a property of what is reported, not of what is emitted. A town with no airport may key its aviation NO. A town that left out its airport has an NE in that cell and an incomplete BASIC+.

## 2. Uncertainty and Key Categories

The IPCC's Approach 1 (2006 Guidelines, vol. 1, ch. 3) combines relative half-widths of 95% intervals, assuming independence. A product of activity and factor has

  U_e = √(U_A² + U_F²)  (eq. 3.1),

and a sum has

  U = √(Σ_e (U_e x_e)²) / |Σ_e x_e|  (eq. 3.2).

*Caveat.* Eq. 3.1 is reliable for U below about 0.6. Entries that share a factor, such as every scope 2 entry under one grid factor, are correlated, and their combined uncertainty is then larger than eq. 3.2 gives. The module states this caveat beside each total it reports.

A category is (sector, scope, subsector, gas). In the IPCC's Approach 1 level assessment (vol. 1, ch. 4), categories are taken in descending order of |x| and are key until their cumulative share reaches 95%.

## 3. Measures

**Definition 4 (measure).** A measure k acts on a set of entries E_k (by sector, possibly every sector, and by scope, subsector and gas). It scales by (1 − r_k) either the activity or the factor of each entry in E_k, with r_k ∈ [0, 1]. For a set S of measures, entry e's emissions are x_e Π_{k ∈ S, e ∈ E_k} (1 − r_k). The abatement of S is

  v(S) = Σ_e x_e (1 − Π_{k ∈ S, e ∈ E_k} (1 − r_k)).

Activity and factor enter the same product, since x_e is bilinear in them.

**Proposition 1 (order of application).** v(S) does not depend on the order in which the measures of S are applied.

*Proof.* Each entry's emissions are multiplied by a product of factors (1 − r_k), and multiplication of reals commutes. ∎

**Proposition 2 (diminishing returns).** v is monotone and submodular: for S ⊆ T and k ∉ T, v(S ∪ {k}) − v(S) ≥ v(T ∪ {k}) − v(T) ≥ 0.

*Proof.* The marginal value of k at S is Σ_{e ∈ E_k} x_e r_k Π_{j ∈ S, e ∈ E_j} (1 − r_j). This is non-negative. It does not increase as S grows, since each added factor (1 − r_j) lies in [0, 1]. ∎

**Corollary (the curve's overlap).** Σ_k v({k}) ≥ v(N). The difference is the overlap, which an abatement cost curve that credits each measure alone counts twice (Kesicki and Ekins 2012). For two measures on a single entry of emissions x, the overlap is x r₁ r₂.

**Proposition 3 (what any order credits).** In every order of the measures, the marginal abatement credited to k lies in [v(N) − v(N ∖ {k}), v({k})], and so does its Shapley value φ_k. Moreover Σ_k φ_k = v(N).

*Proof.* In any order, k is added to some set S with ∅ ⊆ S ⊆ N ∖ {k}. By Proposition 2 its marginal is at most the marginal at ∅, which is v({k}), and at least the marginal at N ∖ {k}. The Shapley value is an average of such marginals, and it is efficient (Shapley 1953). ∎

So the curve's bar widths are not facts about the measures: they depend on an order, within the interval of Proposition 3. The module reports each measure alone, in the curve's order (by cost per tonne alone unless one is declared), and by its Shapley value, which nomos computes exactly over the 2ⁿ sets.

*Caveat.* A measure that moves demand from one entry to another (heat pumps take gas and add electricity; electric cars take fuel and add scope 2) is not a scaling of one entry. The module models it only if the caller enters the new demand as an entry of its own and a second measure that brings it in.

## 4. The Years Ahead

Activities grow at declared rates by sector, so that x_e(t) = x_e (1 + γ_{σ(e)})^{t − t₀}. Measure k reaches r_k(t) = r_k · clamp((t − start_k + 1)/ramp_k, 0, 1), and its cost in year t is cost_k times the same share. A target is a reduction ρ by a year: the level of the inventory's year times (1 − ρ). The gap is that year's emissions with the measures less the target. A budget is compared with the cumulative emissions over the pathway.

## 5. References

- WRI, C40 and ICLEI (2014). *Global Protocol for Community-Scale Greenhouse Gas Emission Inventories*, version 1.0.
- IPCC (2006). *2006 IPCC Guidelines for National Greenhouse Gas Inventories*, vol. 1, ch. 3 (uncertainties) and ch. 4 (key categories).
- IPCC (2013). *Climate Change 2013: The Physical Science Basis*, WG1, ch. 8, Table 8.7; IPCC (2021), WG1, ch. 7, Table 7.15.
- Kesicki, F., and Ekins, P. (2012). Marginal abatement cost curves: a call for caution. *Climate Policy* 12(2), 219–236.
- Shapley, L. S. (1953). A value for n-person games. In *Contributions to the Theory of Games II*, 307–317.
- The survey *Evidence, methods and actions for steering cities* (`research/literature/reports/`), §A.2 and §5.6.
