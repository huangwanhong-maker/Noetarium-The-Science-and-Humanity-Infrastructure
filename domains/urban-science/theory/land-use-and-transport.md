# Land Use and Transport Interacting

*Status: draft (2026-11-12). The theory of the module `luti` ([ADR 0094](../../../development/decisions/0094-land-use-and-transport-interacting.md)). Learning and use.*

Where people live depends on how easily they reach their work. How easily they reach it depends on where everyone else lives and works, and on the roads they share. Buildings follow the rents that this sets, slowly. Land-use and transport models compose these three speeds (Wegener 2004). This note states the composition held in the code, proves what the code relies on, and says what it leaves out.

## 1. The Region and Its Year

**Definition 1.1 (region).** A region is:
- a finite set of zones Z;
- for each zone i ∈ Z, a stock of dwellings S_i ≥ 0, a capacity K_i ∈ [0, ∞] (the most dwellings its rules allow), an own value δ⁰_i ∈ ℝ and basic employment E^B_i ≥ 0;
- a market of households (urban::housing): classes c with weights w_c and rent coefficients α_c, M households in all, a natural vacancy v and a reference rent r̄;
- a transport system giving costs c_ij ≥ 0 between zones: either a fixed table, or a network with arc times t_a(x_a) nondecreasing in flow and fixed added costs;
- parameters β > 0 (the journey to work's rate), λ ≥ 0 (the value of a unit of accessibility), w > 0 (workers per household), b ≥ 0 (service jobs per household) and β_S > 0;
- a development rule (r_c, κ, d): the rent at which building pays, the speed of building, and depreciation.

**Definition 1.2 (a year).** Given last year's rents r(t−1), stocks S(t−1) and accessibility A(t−1), the year t is computed in order:

1. **Development.** S_i(t) = (1 − d) S_i(t−1) + min(1, κ ln⁺(r_i(t−1)/r_c)) · ρ_i, where the room ρ_i = (K_i − (1 − d) S_i(t−1))⁺ if K_i < ∞ and (1 − d) S_i(t−1) otherwise.
2. **Location.** The market's segments are the zones with S_i(t) > 0, each with value δ_i = δ⁰_i + λ A_i(t−1). The rents r(t) clear (1 − v) S(t), and H(t) are the households located.
3. **Jobs.** E^S_k = b Σ_i H_i P^S_ik, with P^S_i the production-constrained shares toward W_k = E^B_k + H_k at rate β_S. Then E = E^B + E^S.
4. **Travel.** O_i = w H_i, and D_j = E_j Σ O / Σ E. The table T and costs c are either Evans's combined equilibrium on the network (§4) or the doubly constrained table at the fixed costs (§2).
5. **Accessibility.** A_i(t) = (1/β) ln Σ_j E_j e^{−β c_ij}.

*Rationale.* The order follows Wegener's speeds of change: construction is slowest and travel fastest. Location reads last year's accessibility, and development reads last year's rents. These lags make the region a discrete dynamical system x(t) = Φ(x(t−1)), with x = (S, r, A).

## 2. Distribution

**Theorem 2.1 (Wilson 1967).** Let O, D ≥ 0 with Σ O = Σ D, and let the set F of tables T ≥ 0 with row sums O, column sums D and Σ T_ij c_ij = C have a member with every entry positive where c is finite. Then the maximiser of H(T) = −Σ T_ij (ln T_ij − 1) over F is unique and has the form T_ij = a_i b_j e^{−β c_ij}, β being the multiplier of the cost constraint.

*Proof.* H is strictly concave and F is convex and compact, so the maximiser exists and is unique. At an interior maximiser the Lagrangian H − Σ μ_i (Σ_j T_ij − O_i) − Σ ν_j (Σ_i T_ij − D_j) − β (Σ T c − C) is stationary, giving −ln T_ij = μ_i + ν_j + β c_ij. Hence T_ij = e^{−μ_i} e^{−ν_j} e^{−β c_ij}. It is interior because ∂H/∂T = −ln T diverges at zero. ∎

The code meets the margins by Furness's balancing. That is the cyclic I-projection onto the two affine sets of tables with the given row sums and with the given column sums. It converges to the I-projection of the seed e^{−βc} onto their intersection (Csiszár 1975), which is the table of Theorem 2.1 for the β whose cost is C.

**Proposition 2.2 (the mean cost falls with β).** Let C(β) = Σ T(β) c, with T(β) the doubly constrained table. Then C is nonincreasing in β.

*Proof.* T(β) maximises the strictly concave function H(T) − β Σ T c over the tables with the two margins. Its value V(β) is convex in β, as a pointwise maximum of functions affine in β, and V′(β) = −C(β) by the envelope theorem. A convex function has a nondecreasing derivative, so C is nonincreasing. ∎

Hyman's calibration therefore has at most one solution, and the bracket in the code finds it.

**Theorem 2.3 (calibration is maximum likelihood; Flowerdew and Aitkin 1982).** Let an observed table N be modelled as independent Poisson counts with means μ_ij = exp(α_i + γ_j − β c_ij). The likelihood equations are Σ_j μ_ij = Σ_j N_ij for every i, Σ_i μ_ij = Σ_i N_ij for every j, and Σ μ_ij c_ij = Σ N_ij c_ij. The doubly constrained table at the margins of N, with β chosen so that its total cost is N's, satisfies them.

*Proof.* The Poisson log-likelihood is Σ (N ln μ − μ). Its derivative in α_i is Σ_j (N_ij − μ_ij), in γ_j it is Σ_i (N_ij − μ_ij), and in β it is −Σ (N_ij − μ_ij) c_ij. Setting each to zero gives the three equations, and they are exactly the constraints of Theorem 2.1 with O, D and C taken from N. The likelihood is concave in (α, γ, β), so this solution is its maximum. ∎

The checks hold Hyman's β to statsmodels' Poisson GLM and to nomos's PPML to 10⁻¹⁰.

## 3. Accessibility

**Proposition 3.1 (Williams 1977).** Let A_i = (1/β) ln Σ_j W_j e^{−β c_ij} and P_ij = W_j e^{−β c_ij} / Σ_k W_k e^{−β c_ik}. Then ∂A_i/∂c_ij = −P_ij and ∂A_i/∂ln W_j = P_ij / β. Moreover A_i is β⁻¹ times the expected greatest utility of a traveller from i choosing among the destinations with utilities ln W_j − β c_ij plus independent Gumbel errors, up to Euler's constant.

*Proof.* The two derivatives are direct. The expectation of the greatest of independent Gumbel variables with locations u_j is ln Σ e^{u_j} + γ_E (McFadden 1978). ∎

A change of accessibility ΔA is therefore a change of the traveller's welfare in units of cost: the rule of a half, made exact.

## 4. Assignment and the Combined Equilibrium

**Theorem 4.1 (Beckmann, McGuire and Winsten 1956).** Let every arc time t_a be continuous and nondecreasing. Then the arc flows at which every path in use takes the least time of its pair (Wardrop's first principle) are exactly the minimisers of Z(x) = Σ_a ∫₀^{x_a} t_a over the flows that an assignment of the demands to paths makes.

The proof is the Karush–Kuhn–Tucker conditions of Z in path flows: a path's derivative is its time, and the multiplier of its pair's demand is the least time. The equilibrium exists, and its arc flows are unique when every t_a is strictly increasing.

**Theorem 4.2 (Evans 1976).** Let G(T, x) = Z(x) + (1/β) Σ T_ij (ln T_ij − 1) + Σ T_ij τ_ij over the tables with margins O and D and the flows they make, τ the added costs. Its minimisers are the pairs in which T is the doubly constrained table at the least times c(x) + τ and x is Wardrop's equilibrium for T.

*Proof.* G is convex: Z because each t_a is nondecreasing, the entropy term strictly. Its stationarity in T_ij, with multipliers μ_i and ν_j for the margins, gives (1/β) ln T_ij + τ_ij + κ_ij(x) = μ_i + ν_j. Here κ_ij(x) is the least path time, since at fixed T the minimum over paths of Z is Wardrop's. This is T_ij = e^{β μ_i} e^{β ν_j} e^{−β(κ_ij + τ_ij)}. Stationarity in the path flows is Wardrop's condition (Theorem 4.1). ∎

**Proposition 4.3 (the bound in the code).** At (T, x), let t = t(x), let c be the least times plus τ, let T* be the doubly constrained table at c, and let y be T* loaded on the shortest paths. Then
L = Z(x) + Σ c T* − Σ t x + (1/β) Σ T* (ln T* − 1) ≤ min G.

*Proof.* Z is convex, so Z(x′) ≥ Z(x) + Σ t (x′ − x) for all x′. Then G(T′, x′) ≥ Z(x) − Σ t x + Σ t x′ + Σ T′ τ + (1/β) Σ T′ (ln T′ − 1) =: G_x(T′, x′). Minimising G_x over the flows that T′ makes gives Σ t x′ = Σ T′ κ(x), so min G_x = min over T′ of Z(x) − Σ t x + Σ T′ c + (1/β) Σ T′ (ln T′ − 1). By Theorem 2.1 the inner minimiser is T*, and its value is L. ∎

The gap (G − L)/Σ t x is zero exactly at the equilibrium. The code stops below a declared gap, 10⁻⁴ in the region's years.

## 5. Location and Capitalisation

The market's clearing rents exist and are unique when M exceeds the occupied stock (the logit's demands are gross substitutes; [housing.md](housing.md)).

**Theorem 5.1 (capitalisation).** Let the market have one class, with rent coefficient α, and let the stock be held (κ = 0, d = 0). Two years whose last accessibilities differ by ΔA have log rents that differ by exactly λ ΔA_i / α in every zone, and the same households.

*Proof.* With one class, clearing says that each zone's occupied stock O_i = (1 − v) S_i equals M e^{V_i} / (1 + Σ_k e^{V_k}), where V_i = δ⁰_i + λ A_i − α ln(r_i / r̄). Dividing by the outside option's share, ln(O_i / (M − Σ_k O_k)) = δ⁰_i + λ A_i − α ln(r_i / r̄). The left side does not depend on A, so α Δ ln r_i = λ ΔA_i, and the households are the occupied stock in both years. ∎

With several classes the rents are no longer explicit, but they still rise in the zones whose access rose, by gross substitutes. The checks hold Theorem 5.1 to 10⁻¹³.

**Corollary 5.2 (Braess's paradox in land).** Let a region have one zone of dwellings, at S, whose workers all work at E, and let the stock be held. A road whose addition raises the equilibrium time from S to E by Δc lowers the rent at S, two years later, by the factor e^{−λ Δc / α}.

*Proof.* With one destination, A_S = (1/β) ln E_E − c_SE, so ΔA_S = −Δc. Apply Theorem 5.1. ∎

In Braess's network of 4,000 drivers Δc = 15 minutes, and the checks find the rent's fall to 10⁻¹⁵.

## 6. Revealed Base Years and Stationary States

**Proposition 6.1 (a revealed base year is a fixed point).** Let households H and rents r be observed, set S_i = H_i / (1 − v), compute T, c and A from H, and set δ⁰_i = δ_i − λ A_i, where δ is Berry's inversion of the market at (r, H). If no stock is built (κ ln⁺(r/r_c) = 0 in every zone) and nothing changes, the next year has the same households, rents and accessibility.

*Proof.* The next year's market values are δ⁰ + λ A = δ, at which r clears H by construction of the inversion. The clearing rents are unique, so they are r, and the households are H. Steps 3 to 5 depend only on H, so A repeats. ∎

**Proposition 6.2 (stationary states).** Let d = 0. At a fixed point of the year map, every zone has either S_i = K_i or r_i ≤ r_c.

*Proof.* At a fixed point the stock does not change. If r_i > r_c and S_i < K_i, step 1 adds min(1, κ ln(r_i/r_c)) (K_i − S_i) > 0 dwellings. ∎

Whether the year map converges, and how fast, depends on κ, the market's elasticities and the network. The code iterates it (`stationary`) and does not claim convergence in general. The checks find it on their regions.

## 7. What the Model Leaves Out

- **One type of dwelling per zone,** and one mode of travel. Housing's types and classes are kept, but the market's segments are the zones.
- **Jobs filled in proportion by the region's workers;** firms do not choose where to locate (basic employment is the scenario's).
- **Households do not age or form.** M changes only by the scenario; demography's projections are the way in.
- **Rents enter development only through r_c.** There is no land market, no developer's expectation, and no lag of construction beyond a year.
- **The years are deterministic.** The agents of `housing` and `movement` are the stochastic counterpart, and would replace steps 2 and 4 in a microsimulation.

## 8. What New York's Record Says

The study `research/experiments/2026-11-12-land-use-and-transport` tests the parts against LODES's journeys to work between New York City's 2,327 tracts, 2002–2023, with straight distances as costs:
- **§2's model describes the journeys to work only in part.** It places 0.56–0.60 of them where they are (0.50–0.54 without distance). Its decay carries over: 2010's β forecasts 2019 as well as 2019's own.
- **§5's mechanism is contradicted there.** Within a borough, a kilometre more access to jobs went with 17.1% fewer resident workers (± 3.5, the spatial error model). The composed model, its speed of building fitted to the total, places the growth no better than uniform growth (correlation −0.04).

So the theorems hold of the model, and the model did not hold of New York's decade.

## 9. References

- Beckmann, M., McGuire, C. B. and Winsten, C. B. (1956), *Studies in the Economics of Transportation*, Yale University Press.
- Csiszár, I. (1975), I-divergence geometry of probability distributions and minimization problems, *Ann. Probab.* 3, 146–158.
- DiPasquale, D. and Wheaton, W. C. (1996), *Urban Economics and Real Estate Markets*, Prentice Hall.
- Evans, S. P. (1976), Derivation and analysis of some models for combining trip distribution and assignment, *Transp. Res.* 10, 37–57.
- Flowerdew, R. and Aitkin, M. (1982), A method of fitting the gravity model based on the Poisson distribution, *J. Reg. Sci.* 22, 191–202.
- Lowry, I. S. (1964), *A Model of Metropolis*, RM-4035-RC, RAND Corporation.
- McFadden, D. (1978), Modelling the choice of residential location, in Karlqvist et al. (eds), *Spatial Interaction Theory and Planning Models*, North-Holland, 75–96.
- Wegener, M. (2004), Overview of land-use transport models, in Hensher et al. (eds), *Handbook of Transport Geography and Spatial Systems*, Elsevier, 127–146.
- Williams, H. C. W. L. (1977), On the formation of travel demand models and economic evaluation measures of user benefit, *Environ. Plan. A* 9, 285–344.
- Wilson, A. G. (1967), A statistical theory of spatial distribution models, *Transp. Res.* 1, 253–269.
