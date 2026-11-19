# climate

*Status: draft. Bridge, height 4.* The climate system: the coupled ocean and atmosphere, its energy balance, its zones, its variability, and its astronomical forcing; since ADR 0135, its first models with folds in nature, on which kinesis's detectors of critical transitions are held.

## 1. Scope

| Module | Scope | Status |
|---|---|---|
| `zones` | Climatic zones and their classification: Köppen–Geiger after Peel et al. (2007) and Beck et al. (2018), with dry months after Gaussen | draft (JavaScript: `climate.js`) |
| `energy_balance` | The planet's energy balance in one temperature with the ice–albedo feedback (Budyko, Sellers; North, Cahalan and Coakley 1981): a ramp albedo, whose folds are non-smooth and give no slowing before them, and a smooth one, whose folds do; equilibria, their restoring rates, the folds in closed form or by extremum (ADR 0135) | draft (C++) |
| `overturning` | The overturning of a basin in two boxes, Stommel's with Cessi's exchange law, in Berglund and Gentz's dimensionless form: the slow–fast pair and its reduction, the folds y± = 2/3 ± √(η² − 3)/(3η), the bistable interval of freshwater forcing, equilibria as a cubic's roots (ADR 0135) | draft (C++) |
| `coupled` | The coupled ocean–atmosphere system | planned |
| `variability` | Variability such as ENSO and the monsoons | planned |
| `orbital_forcing` | Insolation and Milankovitch cycles | planned |
| later | The cryosphere and the land surface | planned |

## 2. Dependencies

| Domain | Used for |
|---|---|
| meteorology | The atmosphere |
| oceanography | The ocean |
| geography | The land surface |
| astronomy | Orbital elements, insolation |
| air-sea | Coupling between ocean and atmosphere |

Foundations: physis; calx; kinesis (with morphe and tyche beneath it), whose detectors of critical transitions read the models' records in the checks.

## 3. Code

`js/index.js` exports `climate.js`. The C++ is header-only, a target `climate::<module>` for each module (`CMakeLists.txt`). The checks are run by the command below:

- **the classification** (JavaScript): fifteen constructed climates, each meeting one rule; invariance of the class under the group (S₆ × S₆) ≀ Z/2 acting on the months; every class reachable;
- **the energy balance** (18): the ramp's folds and equilibria at their closed forms; its constant restoring rate B/C on a branch; the smooth form's folds where the rate vanishes; the smooth balance driven towards its fold read as slowing by kinesis, and the ramp's not;
- **the overturning** (13): the folds, the interval [0.964, 1.480] at η² = 7.5 and Ginzburg and Landau's condition; the cubic's equilibria; the slow–fast pair following its reduction; the forcing rising towards the fold read as slowing.

```
node domains/climate/tests/run-tests.mjs
```

The comparison of our classification with the published maps of Beck et al. reads the data library's products, and is the Earth Atlas's.

## 4. Notes

- [The Köppen–Geiger classification of climates, and the climate diagram](theory/koppen-geiger-classification.md)

## 5. Used By

The Earth Atlas.
