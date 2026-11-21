# Land Value Capture and Land Readjustment

*Status: draft (ADR 0146). The theory note of the economics domain's `capture`.*

A public project (a line of transit, a road, drainage) raises the value of the land it serves. Land value capture is the family of instruments by which a government takes part of that rise back to pay for the project; land readjustment is the instrument by which owners pool their land, give part of it for streets and for sale, and receive smaller plots worth more. The module computes what each instrument would take, from whom and when, and keeps apart three ratios that the literature calls by one name. It decides nothing. Every rate, share, cap and value is the caller's declaration, and none is any jurisdiction's law.

## 1. Uplift

**Definition 1 (uplift).** Let a project touch parcels i = 1, …, n. Each parcel has a value V_i⁻ without the project and V_i⁺ with it, at one declared valuation date. Its uplift is ΔV_i = V_i⁺ − V_i⁻, and the capturable uplift is U = Σ_i ΔV_i⁺, where x⁺ = max(0, x).

*Caveat.* The counterfactual V_i⁻ is the caller's. It may come from a hedonic model or from a comparison of places the project reached with places it did not. A difference of values before and after the project is not a counterfactual, since it carries the market's trend with it (the survey *Learning finance scorecards and value capture*, Part IV §1). A parcel the project lowers in value contributes nothing to U and is charged nothing on the uplift basis.

## 2. The Instruments

Each instrument is a stream of payments (t, x, payer, parcel, from-uplift), where t is in years after the valuation date. Streams are compared on present values at one declared rate r, PV = Σ x / (1 + r)^t.

**Definition 2 (levy).** A betterment levy of cost C, mark-up a and share λ ∈ [0, 1] raises L = min(C(1 + a), λU). It is apportioned by a declared score w_i (the uplift ΔV_i⁺, the area, or the frontage): parcel i owes L w_i / Σ_j w_j, paid in equal yearly instalments.

**Definition 3 (development rights).**
- *By formula:* added floor area ΔA sold at C = (ΔA / FAR₀) p k. Here FAR₀ is the basic floor-area ratio, p the price of land per unit of area, and k a declared factor (the "virtual land" method, after São Paulo's *outorga onerosa*; Smolka 2013).
- *At auction:* N certificates at a minimum price m are sold to the bids at or above m, highest first, while they last. Each sells at the clearing price: m if some certificate remains unsold, otherwise the larger of m and the lowest accepted bid. The stock identity N = sold + unsold is kept.

**Definition 4 (tax increment).** Let a district have assessed values AV_t over its term, a base B, a share s of the increment it keeps, and overlapping jurisdictions j with rates τ_j. The district collects R_t = s Σ_j τ_j (AV_t − B)⁺. Let β ∈ [0, 1] be the share of the increment that would not have come without the project (the "but-for" share). Then the part τ_j s (1 − β)(AV_t − B)⁺ of jurisdiction j's revenue would have come anyway, and the district diverts it.

**Definition 5 (developer obligation).** A charge c per unit permitted raises cQ on Q units. Its incidence is split by declared shares between land (lower prices for land) and buyers (higher prices for what is built), the shares summing to one. It is a fee on new floor area, not on uplift, so it is marked as not taken from the uplift.

## 3. Three Ratios Kept Apart

**Definition 6.** For an instrument with public revenue PV and with PV_U of it taken from the uplift, and for a project of cost PV_C:
- *cost recovery* ρ = PV / PV_C;
- *capture of the uplift* κ = PV_U / U;
- *the statutory rate* σ, as a law or scheme declares it.

The three are different quantities: what the project gets back, what share of the rise is taken, and what a rule says should be taken. The survey found each called "the capture rate" (Part IV §6). An obligation has ρ > 0 and κ = 0. A levy on a small rise can take most of the uplift and still recover little of the cost.

**Remark.** The κ of several instruments share the denominator U, so they add. When Σ κ > 1, the instruments together take more than the rise there is, and someone pays from value they held before the project.

**Definition 7 (charged twice over).** A parcel is charged twice over when the payments that name it and are taken from the uplift exceed its uplift in present value, Σ PV > ΔV_i⁺. The module names every such parcel. In the API, a sale of rights may name the parcel its added floor is granted on, so that a levy and a sale of rights on one parcel are seen together.

**Proposition 1.** A levy apportioned by uplift, with λ ≤ 1 and r ≥ 0, never charges a parcel more than its uplift.

*Proof.* Parcel i owes L ΔV_i⁺ / U in nominal terms, and L ≤ λU ≤ U, so it owes at most ΔV_i⁺. Discounting at r ≥ 0 only lowers the present value. ∎

Apportioned by area or by frontage, a levy can charge a parcel that gained little more than it gained. That is why the module checks every parcel rather than trusting the basis.

**Proposition 2.** At one rate and a constant β, the present value the district diverts is (1 − β) times the present value it collects.

*Proof.* Term by term, each diverted payment is (1 − β) times the payment the district collects in the same year from the same jurisdiction, and present value is linear. ∎

So a district that keeps its whole increment (s = 1) with β = 0.6 collects from the uplift only 0.6 of what it raises. The other 0.4 was the other governments' revenue. The module counts that 0.4 as not taken from the uplift, and reports it by jurisdiction.

## 4. Land Readjustment

**Definition 8.** Let M be the public land before, A the private land pooled, N the public land after, E the land returned to owners and R the reserve land sold to pay for the works. The areas balance when M + A = N + E + R. The contribution ratio is d = (P + R)/A with P = N − M, split into a public share P/A and a reserve share R/A. Let unit values be a before and e after, with y = e/a. The owners' land keeps its value when (1 − d) y ≥ 1.

**Proposition 3.** When the areas balance, 1 − d = E/A, so the owners' land keeps its value exactly when eE ≥ aA: the land returned is worth at least the land pooled.

*Proof.* d = (N − M + R)/A, and N + R = M + A − E by the identity, so d = (A − E)/A. Then (1 − d)y = (E/A)(e/a), which is at least 1 exactly when eE ≥ aA. ∎

The project is feasible when the reserve land's sale R e (1 − c), with c a declared selling cost, together with declared subsidies, meets the declared costs in present value. A declared cap on d is checked. Reserve-land sales are capital revenue: they raise no operating surplus, and so do not raise the debt capacity of the economics domain's `fiscal` (its Proposition 1). The survey's synthesis (§5.7) keeps the two apart for the same reason.

**Definition 9 (replotting and settlement).** Owners i hold parcels at points x_i with values V_i before; plots j lie at points z_j with values V′_j = area_j · unit value_j, and there are at least as many plots as owners. Replotting assigns each owner a plot by an injection π minimising Σ_i ‖x_i − z_{π(i)}‖, so that owners stay as near their old land as the plots allow. Owner i's entitlement is V*_i = V_i · Σ_k V′_{π(k)} / Σ_k V_k, their share of the value returned. Their settlement is s_i = V′_{π(i)} − V*_i: positive, the owner pays it; negative, the owner is paid.

**Proposition 4.** Σ_i s_i = 0.

*Proof.* Σ_i V*_i = (Σ_i V_i)(Σ_k V′_{π(k)}) / Σ_k V_k = Σ_k V′_{π(k)}. ∎

The injection is found by calx's Hungarian method for rectangular assignments, and the checks hold it equal to enumeration. Nearness is one declared criterion among several used in practice, alongside frontage, use and the owners' consent. The module minimises nearness only and says so.

## 5. What Is Left to the Caller

- the counterfactual values, and how they were estimated;
- the incidence shares of an obligation;
- the but-for share of an increment;
- the cap, the selling cost and the costs of a readjustment;
- any statutory rate.

Land markets that do not clear at the declared values, holdouts, and the legal power to bind a minority are not modelled. They decide whether a readjustment happens at all (Hong and Needham 2007, as summarised in the survey's Part IV).

## 6. References

- The survey *Learning finance scorecards and value capture* (`research/literature/reports/`), Part IV and §6.4, with its verification markers.
- Smolka, M. O. (2013). *Implementing Value Capture in Latin America: Policies and Tools for Urban Development.* Lincoln Institute of Land Policy.
- Germán, L., and Bernstein, A. E. (2018). *Land Value Capture: Tools to Finance Our Urban Future.* Lincoln Institute of Land Policy, policy brief.
- Suzuki, H., Murakami, J., Hong, Y.-H., and Tamayose, B. (2015). *Financing Transit-Oriented Development with Land Values.* World Bank.
- Dye, R. F., and Merriman, D. F. (2006). Tax increment financing. *Land Lines* (January). Lincoln Institute of Land Policy.
- OECD and Lincoln Institute of Land Policy (2022). *Global Compendium of Land Value Capture Policies.* OECD Publishing.
- Hong, Y.-H., and Needham, B. (eds.) (2007). *Analyzing Land Readjustment: Economics, Law, and Collective Action.* Lincoln Institute of Land Policy (not read; cited through the survey).
