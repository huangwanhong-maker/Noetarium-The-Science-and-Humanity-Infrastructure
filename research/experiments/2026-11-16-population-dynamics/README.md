# Population Dynamics: Melbourne and Lagos as Fields, 1975 to 2020

*Status: draft (2026-11-16; [ADR 0124](../../../development/decisions/0124-population-dynamics.md)). Activity: research.* The examples of the Urban Studio's plug-in *Population dynamics*, worked through: a city's people read as a field whose laws are learned as a fluid's, and held against the years kept aside.

## 1. Question

Does a city's population move and grow like a fluid? The question is asked of its record, cell by cell, at every fifth year:

- how well laws of spread (diffusion, crowding) and drift (toward the centre, along the settled landscape), with growth, describe the change of the field;
- whether they place the change better than growing every cell alike, or than the record's own modes;
- what laws of growth say of the whole.

## 2. Records and Methods

- **The record:** GHS-POP R2023A (the European Commission's JRC, CC BY 4.0), the residential population at every fifth year from 1975 to 2030. The 1 km Mollweide tiles are carried onto a 1 km UTM grid with nobody lost (`data/products/population_history.py`).
  - Melbourne: 80 km square about the City, 5,636 land cells.
  - Lagos: 70 km square about Lagos Island, 3,047 land cells; the sea and the lagoons are not cells.

  Each epoch is the JRC's disaggregation of the censuses then available into the built-up surface. 2025 and 2030 are projections, never fitted on.
- **The laws** are kinesis's balance laws on the grid (its decision 0005). Each is fitted on 1975–2010 by the integral form and then shooting, the cells weighed by 1/√(u + ū) as counts are. A diffusion found negative is set aside.
- **The judgement.** Each law is run from 2010 to 2015 and 2020 and compared with the record, against carrying 2010 forward:
  - *how many* is the skill of the forecast as it is;
  - *where* is the skill once each forecast's total is set to the record's.

  Growing every cell alike has *where* = 0 by construction.
- **Baselines:** a rate of its own for each cell, and the record's dynamic modes.

## 3. Results

### 3.1 No Diffusion at 1 km

In both cities and in every law that holds it, the diffusivity is found negative and set aside, or comes out too small to move anyone. Fisher's law in Lagos gives D = 0.006 km² a year, a front at 28 m a year. A spread that moved people a kilometre a year (D ≈ 1 km² a year) would flatten Melbourne's centre (14,000 people per km² beside 3,000) within a year. A city's density is pinned by its buildings, and its record holds no Fickian spread.

### 3.2 Melbourne

| Law (fitted 1975–2010) | Where | How many | Total in 2020 |
|---|---|---|---|
| The record's modes, carried on | +0.87 | +0.90 | −0.0% |
| A rate of its own for each cell (5,636 numbers) | +0.74 | +0.77 | −5.1% |
| A law found among every term | +0.02 | +0.13 | −15.1% |
| Growth, spread and drift toward the centre | +0.01 | +0.08 | −16.3% |
| Growth, spread and a drift along the settled landscape | +0.01 | +0.08 | −16.2% |
| Logistic growth everywhere | +0.00 | +0.08 | −16.3% |
| Growth everywhere at one rate | 0 | +0.12 | −15.7% |

- **The total.** The laws learned on 1975–2010 grow at 0.7–1.3% a year, and Melbourne then grew at about 2.4%: they miss 16% of the people in 2020.
- **The pattern.** They place it no better than growing everywhere alike. The drift toward the centre is 2 m a year, inward.
- **The modes.** The record's modes, a linear extrapolation in the span of what was seen, catch both the acceleration and the pattern. The first mode fades by 0.3% a year (most of the place, nearly fixed); the next ones fade fast or turn.

### 3.3 Lagos

| Law (fitted 1975–2010) | Where | How many | Total in 2020 |
|---|---|---|---|
| Growth, spread and a drift along the settled landscape | +0.09 | −0.72 | +18.2% |
| Growth everywhere at one rate | 0 | −0.66 | +17.1% |
| Logistic growth everywhere | −0.09 | −0.23 | +15.7% |
| Growth and spread (Fisher's law) | −0.50 | −1.78 | +21.3% |
| The record's modes, carried on | −4.04 | −1.82 | −19.9% |
| A rate of its own for each cell | −7.60 | −4.39 | +33.8% |

- **The total.** Lagos grew 3.4% a year to 2010 and 1.8% after. Every law overshoots, and carrying 2010 forward is better as a count.
- **The pattern.** The law with a drift *down* the log of 1975's density (χ = −0.0019 km² a year), people moving outward from where they had been, places the change better than uniform growth. It is the only law that does.
- **The baselines.** The cells' own rates and the record's modes, which did well in Melbourne, fail: in a city growing from nothing at its edge, a cell's rate of 1975–2010 is no guide to its next decade.

### 3.4 The Whole

| | Melbourne | Lagos |
|---|---|---|
| Exponential, off on 2015 and 2020 | 10.7% | 17.6% |
| Logistic | 5.1% (accelerating: no ceiling, runs away before 2046) | 3.6% (ceiling 15.5 million) |
| Gompertz's | 6.0% (accelerating) | **0.6%** (ceiling 20.5 million) |

Gompertz's law fits Lagos's slowing growth closely. For Melbourne, whose growth accelerated, every law of the whole either runs away or misses: its growth after 2005 was not in its record before.

## 4. Discussion

- **What a fluid law holds and a city does not.** A fluid's laws are local and the same everywhere: spread and drift with coefficients that do not depend on the place. A city's people are placed by what was built and zoned, which the field does not hold. Neither record admits a local spread. A drift along the settled landscape is the one fluid law that helps, a little, in the growing city.
- **Two questions, kept apart.** How many people there will be depends on births, deaths and migration, which no law of the field knows. Where they will be is what a law of the field is about. The pattern's skill keeps the two apart; on the totals alone, Lagos's laws would look worse than they are and Melbourne's better.
- **GHS-POP's own making.** Its epochs are disaggregated into built-up surface, so its field moves partly as the built-up layer moves. What is learned is a law of that estimate.
- **What would help:** covariates that the field does not hold (roads, access, land use, zoning) to drift up; coefficients that vary in space; zones with their own faces; finer or yearly records.

## 5. Reproduction

```
python data/products/population_history.py
python data/acquisition/recipe.py applications/urban-studio/recipe.json
python applications/urban-studio/serve.py --open
```

Begin a project from *Melbourne's people, 1975 to 2050* or *Lagos's people, 1975 to 2050*, open the *Population* view, and run the plan. The figures above are those of 2026-11-16. The pictures are in the gallery's §44.
