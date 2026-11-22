# The Structure of Regional Economies, and How It Changes

*Status: draft (2026-11-23; [ADR 0166](../../../development/decisions/0166-the-structure-of-regional-economies.md)). Activity: use and learning.* The module `diversification` composes nomos's `structure` (its decision 0018), tyche's `bipartite_ordering` (its decision 0033) and Mann and Whitney's U (its decision 0036).

## 1. The Setting

A table holds the employment e_rp of regions r in industries p at two dates. Each region has a reference group (its country) whose employment is taken as the sum of its regions in the table; each industry has a group (its NACE section). A cell withheld by the statistical office at either date is left out at both, so that withholding does not appear as change, and the cells withheld are counted.

## 2. Shift-Share

**Definition 1 (Dunn 1960).** With the reference's growth G overall and G_p in industry p, the change Δe_rp of region r is split into the national share e_rp G, the industrial mix e_rp (G_p − G) and the competitive term Δe_rp − e_rp G_p.

**Proposition 1.** The three terms sum to Δe_rp; summed over the regions of a reference whose employment is the sum of its regions, the competitive terms vanish.

*Proof.* The first is the definition. For the second, Σ_r (Δe_rp − e_rp G_p) = ΔE_p − E_p G_p = 0 for each p, since G_p = ΔE_p / E_p. ∎

*Remark.* The competitive term is the region's departure from what its industries did nationally; it is the residual of an identity and not a measure of competitiveness in any causal sense. Esteban-Marquillas's split separates the part of it due to the region's specialisation (the allocation effect).

## 3. Variety

**Proposition 2 (Frenken, Van Oort and Verburg 2007).** With shares s_p of a region's employment and S_g = Σ_{p∈g} s_p, the entropy H = Σ s_p log(1/s_p) equals unrelated variety Σ_g S_g log(1/S_g) plus related variety Σ_g S_g H_g, H_g the entropy of the shares within g.

*Proof.* The chain rule of entropy for the pair (group, industry), the group a function of the industry. ∎

## 4. Relatedness and Its Test

A region holds an industry when its location quotient is at least a threshold (one by default). The proximity of two industries is the lesser of the two conditional probabilities that a region holding one holds the other; the density of region r around industry p is the proximity-weighted share of p's neighbours that r holds (Hidalgo et al. 2007).

**The test.** Among the pairs (r, p) with r not holding p at the first date, the density at the first date is set against whether r holds p at the second. The area under the curve of the densities of the pairs that entered against those that did not is Mann and Whitney's U over n₁n₂: one half when density tells nothing. It is an out-of-sample prediction of the kind reviewed by Hidalgo et al. (2018); it is not a causal estimate, since what makes a region enter an industry may also make the industries near it present.

## 5. Complexity

The complexity index of regions is the second eigenvector of the walk D⁻¹ M U⁻¹ Mᵀ on the incidence M, which Mealy, Farmer and Teytelboym (2019) show to be the relaxation of a normalised cut of the regions' similarity: it orders regions by which side of the strongest division of the industries they lie on. Where the incidence falls apart into blocks, the eigenvalue one repeats and the index would only name a block; it is then not given, and the reason is said.

*Caveat.* The index is signed by convention to rise with diversity. Among Europe's NUTS 2 regions the most diversified are those of manufacturing, and the conventional sign places coal mining, textiles and apparel at the top and capital regions of services at the bottom (ADR 0166). It is a position on the strongest division, and is reported so.

## 6. References

- Dunn, E. S. (1960), A statistical and analytical technique for regional analysis, *Papers and Proceedings of the Regional Science Association* 6, 97–112.
- Esteban-Marquillas, J. M. (1972), A reinterpretation of shift-share analysis, *Regional and Urban Economics* 2, 249–255.
- Frenken, K., Van Oort, F. and Verburg, T. (2007), Related variety, unrelated variety and regional economic growth, *Regional Studies* 41, 685–697.
- Hidalgo, C. A., Klinger, B., Barabási, A.-L. and Hausmann, R. (2007), The product space conditions the development of nations, *Science* 317, 482–487.
- Hidalgo, C. A. et al. (2018), The principle of relatedness, in *Unifying Themes in Complex Systems IX*, Springer, 451–457.
- Hidalgo, C. A. and Hausmann, R. (2009), The building blocks of economic complexity, *PNAS* 106, 10570–10575.
- Mealy, P., Farmer, J. D. and Teytelboym, A. (2019), Interpreting economic complexity, *Science Advances* 5, eaau1705.
