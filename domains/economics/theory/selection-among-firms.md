# Selection Among Firms: Decompositions of Productivity Growth as Instances of Price's Equation

*Status: draft (2026-11-23; [ADR 0165](../../../development/decisions/0165-the-economics-of-evolution-and-selection-among-firms.md)). Activity: use and learning.* The module `selection` holds the decompositions; their general accounting is tyche's `census` (its decision 0035).

## 1. The Setting

A panel holds firms at two dates t and t′. Firm i has a weight x_i (employment, output, value added) and a productivity φ_i, in logs or in levels as stated. With shares s_i = x_i / Σ x, the aggregate is Φ = Σ_i s_i φ_i. Firms present at both dates form the survivors S, those only after the entrants E, those only before the exiters X; s_S, s′_E and s_X are the shares of these sets at the date where they exist.

**Definition 1 (a decomposition).** A decomposition of ΔΦ = Φ′ − Φ is a list of terms, each a function of the panel, whose sum is ΔΦ for every panel.

## 2. The Methods

With Δs_i = s′_i − s_i, Δφ_i = φ′_i − φ_i, and bars averages over the two dates:

| Method | Terms |
|---|---|
| Baily, Hulten and Campbell (1992) | within Σ_S s_i Δφ_i; between Σ_S Δs_i φ′_i; entry Σ_E s′_i φ′_i; exit −Σ_X s_i φ_i |
| Griliches and Regev (1995) | within Σ_S s̄_i Δφ_i; between Σ_S Δs_i (φ̄_i − Φ̄); entry Σ_E s′_i (φ′_i − Φ̄); exit −Σ_X s_i (φ_i − Φ̄) |
| Foster, Haltiwanger and Krizan (2001) | within Σ_S s_i Δφ_i; between Σ_S Δs_i (φ_i − Φ); cross Σ_S Δs_i Δφ_i; entry Σ_E s′_i (φ′_i − Φ); exit −Σ_X s_i (φ_i − Φ) |
| Melitz and Polanec (2015), dynamic Olley–Pakes | survivors (Φ′_S − Φ_S) = Δφ̄_S + Δcov_S; entry s′_E (Φ′_E − Φ′_S); exit s_X (Φ_S − Φ_X) |
| Price's equation over the survivors | selection Σ_S (σ′_i − σ_i)(φ_i − Φ_S), between industries and within them; learning Σ_S σ′_i Δφ_i; entry and exit as above |

Here Φ_S = Σ_S σ_i φ_i with σ_i = s_i / s_S, and Olley and Pakes's split of a cross-section is Φ = φ̄ + Σ_i (s_i − 1/n)(φ_i − φ̄), φ̄ the unweighted mean.

**Proposition 1.** Each method is a decomposition.

*Proof.* For the three with a reference R (R = 0, Φ̄ or Φ), the terms sum to Σ_S (s′_i φ′_i − s_i φ_i) + Σ_E s′_i φ′_i − Σ_X s_i φ_i − R (Σ_S Δs_i + s′_E − s_X); the first three sums are Φ′ − Φ, and the bracket is (s′_S + s′_E) − (s_S + s_X) = 1 − 1 = 0. (For Griliches and Regev the survivors' terms sum to Σ_S (s′_i φ′_i − s_i φ_i) by expanding s̄_i Δφ_i + Δs_i φ̄_i.) For the dynamic Olley–Pakes decomposition, Φ′ = s′_S Φ′_S + s′_E Φ′_E and Φ = s_S Φ_S + s_X Φ_X with s′_S = 1 − s′_E, s_S = 1 − s_X; substituting gives the three terms. Olley and Pakes's split holds since Σ_i (s_i − 1/n) φ̄ = 0. Price's split of the survivors' change is tyche's identity (its `census`, Proposition in the module's header). ∎

**Proposition 2 (Melitz and Polanec 2015, §2).** If every firm's productivity is φ at t and φ + γ at t′, entry is credited with 0 by the dynamic Olley–Pakes decomposition, s′_E γ/2 by Griliches and Regev's and s′_E γ by Foster, Haltiwanger and Krizan's; exit with 0, s_X γ/2 and 0.

*Proof.* Φ′_E = Φ′_S = φ + γ and Φ_S = Φ_X = φ give the first; Φ̄ = φ + γ/2 the second; R = Φ = φ the third. ∎

**Proposition 3.** Adding a constant c to every productivity leaves the terms of Griliches and Regev's, Foster, Haltiwanger and Krizan's, the dynamic Olley–Pakes and Price's decompositions unchanged, and moves Baily, Hulten and Campbell's entry by c s′_E, exit by −c s_X and between by c (s′_S − s_S).

*Proof.* In the first four every productivity enters as a difference from a mean or between dates. In the fifth, R = 0 does not move with c. ∎

*Remark.* Proposition 3 is why the literature since Foster, Haltiwanger and Krizan judges entrants and exiters against a reference: with R = 0 the credit of entry depends on the units of productivity. Proposition 2 is why the reference itself carries meaning, and why Melitz and Polanec judge entrants against survivors.

## 3. What the Terms Do Not Say

- **Causes.** Selection is a causal claim only when what makes the covariance is supplied (Hodgson and Knudsen 2006). Selection read from revenue productivity confounds efficiency with prices (Foster, Haltiwanger and Syverson 2008).
- **Additivity over intervals.** The selection terms of successive intervals do not sum to the selection term of the whole span (Holm 2014, §4); tyche's checks show it on composed records.
- **Logs and levels.** Decompositions in logs and in levels differ; the caller states which.

## 4. References

- Baily, M. N., Hulten, C. and Campbell, D. (1992), Productivity dynamics in manufacturing plants, *Brookings Papers on Economic Activity: Microeconomics*, 187–267.
- Foster, L., Haltiwanger, J. and Krizan, C. J. (2001), Aggregate productivity growth: lessons from microeconomic evidence, in Hulten, Dean and Harper (eds), *New Developments in Productivity Analysis*, Chicago, 303–372.
- Foster, L., Haltiwanger, J. and Syverson, C. (2008), Reallocation, firm turnover, and efficiency: selection on productivity or profitability?, *American Economic Review* 98, 394–425.
- Griliches, Z. and Regev, H. (1995), Firm productivity in Israeli industry 1979–1988, *Journal of Econometrics* 65, 175–203.
- Hodgson, G. M. and Knudsen, T. (2006), The nature and units of social selection, *Journal of Evolutionary Economics* 16, 477–489.
- Holm, J. R. (2014), The significance of selection and learning for productivity growth, *Journal of Evolutionary Economics* 24, 1013–1040.
- Melitz, M. J. and Polanec, S. (2015), Dynamic Olley–Pakes productivity decomposition with entry and exit, *RAND Journal of Economics* 46, 362–375.
- Olley, G. S. and Pakes, A. (1996), The dynamics of productivity in the telecommunications equipment industry, *Econometrica* 64, 1263–1297.
