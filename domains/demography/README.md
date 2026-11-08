# Demography

*Status: draft (2026-11-08).* The domain of populations by age: their deaths, births and growth. It is the first domain on the foundation peira ([ADR 0085](../../development/decisions/0085-demography-on-peira.md)), whose designs and cohorts it uses. It is a field at height 0, standing on the foundations alone, C++ from the start.

## 1. Purpose

A population is a stock renewed by births and drained by deaths and migration, structured by age. Demography holds the objects that describe it:
- the life table, a population's mortality as a cohort would live it;
- the rates of reproduction;
- the projection of a population by age, and the stable population it tends to.

What is general belongs to peira's `cohorts` and is used here, not written again:
- the person-time and events of the Lexis diagram;
- rates with exact intervals;
- standardisation.

Its theory note, [theory/demography.md](theory/demography.md), states and proves what the code relies on.

## 2. Modules

| Module | Holds | Status |
|---|---|---|
| `lifetable` | The period life table from death rates (a_x by default, or exact for a constant hazard); rates read from peira's Lexis diagram; death rates standardised through peira; Arriaga's decomposition of a difference in life expectancy | draft: 9 checks |
| `fertility` | The total, gross and net reproduction rates; the mean ages at childbearing | draft: 4 checks |
| `projection` | The Leslie matrix (Keyfitz's first row), projection, the stable population (λ, r, its structure, R₀), Lotka's intrinsic rate | draft: 9 checks |
| `mortality` | Laws of mortality fitted (Gompertz–Makeham), Lee and Carter's model, mortality over time | planned |
| `migration` | Migration in cohort-component projections, its schedules (Rogers and Castro) | planned |
| `decomposition` | Kitagawa's decomposition of rates, and others beside Arriaga's | planned |
| `households` | Households and their formation | planned |

## 3. Checks

`node domains/demography/tests/run-tests.mjs` builds and runs 22 checks against closed forms and exact identities:
- a constant hazard reproduced exactly (e_x = 1/m);
- the life table's identities;
- Arriaga's contributions summing to the difference in e_0;
- rates from peira's Lexis diagram of simulated lives;
- NRR = GRR without deaths;
- the characteristic equation, the stable eigenvector and ergodicity;
- λ > 1 exactly when R₀ > 1, and R₀ = NRR exactly;
- Lotka's r against the matrix's.

## 4. Dependencies

peira (`cohorts`), tyche (`probability`, through peira), calx (`solvers`, `verification`); morphe and physis as the siblings' order requires. No domain.

## 5. References

Graunt (1662); Halley (1693); Lotka (1939); Leslie (1945); Keyfitz (1964); Lopez (1961); Chiang (1984); Arriaga (1984); Breslow and Day (1987); Preston, Heuveline and Guillot (2001), *Demography: Measuring and Modeling Population Processes*; Keyfitz and Caswell (2005), *Applied Mathematical Demography*.

## 6. Open Questions

- Which real tables come first: the Australian Bureau of Statistics' life tables for Melbourne's state, or the UN's World Population Prospects for every country? The Earth Atlas and the Urban Science Workbench already hold populations on grids.
- Epidemiology, planned on demography, will need incidence and prevalence on the same Lexis diagram.
