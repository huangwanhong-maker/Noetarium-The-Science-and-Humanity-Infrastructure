# Land Use and Transport on New York's Journeys to Work, 2002–2023

*Status: draft (2026-11-12). Research. [ADR 0094](../../../development/decisions/0094-land-use-and-transport-interacting.md); the model's theory in [land-use-and-transport.md](../../../domains/urban-science/theory/land-use-and-transport.md).*

## 1. The Questions

The land-use and transport model of ADR 0094 rests on three claims. This study tests each against twenty-two years of New York City's journeys to work:

1. **Journeys fall with distance.** The doubly constrained model with an exponential decay describes where people work, given where they live and where the jobs are.
2. **The decay carries over.** A decay fitted in one year forecasts a later year's flows from that year's margins.
3. **Access feeds back to where people live.** Places that gain access to jobs gain residents, as dwellings follow rents.

## 2. The Record

- **The flows.** LODES 8 (US Census Bureau, public domain): New York State's main files of jobs by the census blocks of home and workplace, in ten years from 2002 to 2023. They are summed to the 2,327 census tracts of the five boroughs, keeping the 2.4 to 3.3 million jobs a year whose worker both lives and works in the city (`data/products/commuting.py`, case `nyc-lodes`). A worker with two jobs counts twice, and the block counts are perturbed for confidentiality, which summing to tracts largely cancels.
- **The costs.** The great-circle kilometres between the tracts' points, each point weighted by its blocks' jobs and workers. A tract's own cost is half the distance to its nearest neighbour. The subway is not in the costs: what distance cannot explain includes it.

## 3. Method

`study.cpp`, on nomos's `interaction`, tyche's regression and spatial models, and urban science's `luti`:
- **Decay.** Each year's table is calibrated by Hyman's method, which is the Poisson maximum likelihood estimate. It is scored by the common part of commuters (CPC): the share of the flows the model places where they are. The independence model (β = 0) gives the scale.
- **Forecast.** An earlier year's flows are carried to a later year's margins in four ways: the earlier year's β (gravity); the earlier table balanced to the later margins by Furness, the cells it lacks given by that gravity forecast (the pivot-point form); the later year's own β, which a forecast cannot know; and the earlier table unchanged.
- **Access and residence.** Each tract's change of accessibility to jobs from 2010 to 2019 is the logsum of the jobs at 2010's β, in kilometres. It is set against the change of the log of its resident workers, the boroughs' means removed, for the 2,215 tracts with 200 resident workers or more. The change of access is smooth in space (Moran's I 0.995) and what is left is clustered (0.20, p = 0.001), so the slope's standard error comes from the spatial error model (eight nearest neighbours).
- **The composed model.** `luti` is revealed in 2010, with the resident workers as households and every rent at the rent at which building pays. The jobs of each year are interpolated between the record's years, and the model is run to 2019. One number, the speed of building, is set by bisection so that the region's resident workers in 2019 are the record's. Where they went is the model's, and it is set against the record's.

## 4. What It Shows

### 4.1 The Decay Has Weakened, and Distance Explains Little

β fell from 0.146 per km in 2002 to 0.131 in 2019 and 0.127 in 2021–2023. The mean straight journey stayed between 9.3 and 9.5 km. The common part is 0.56–0.60, against 0.50–0.54 for a model that ignores distance. Most of where New Yorkers work is not explained by how far it is.

### 4.2 The Decay Carries Over; a Year's Own Pattern Lasts a Few Years

| From → to | 2010's β (gravity) | Carried table | The later year's own β | Unchanged |
|---|---|---|---|---|
| 2010 → 2019 | **0.598** | 0.595 | 0.597 | 0.515 |
| 2019 → 2023 | 0.583 | **0.602** | 0.583 | 0.575 |
| 2002 → 2019 | **0.598** | 0.571 | 0.597 | 0.468 |

An earlier year's β forecasts as well as the later year's own. Over nine and seventeen years the gravity forecast beats carrying the old table forward. Across the four years of the pandemic the old table, carried, does better: the pairs of places it holds outlast that short interval, and fade over a decade.

### 4.3 Where Access Rose, Resident Workers Came Less

Within a borough, a tract whose accessibility to jobs rose a kilometre more than another's gained 17.1% fewer resident workers (± 3.5%, the spatial error model, λ = 0.51; least squares, −18.1 ± 2.7). Adding access to the boroughs raises R² from 0.039 to 0.076.

Access rose most, relative to the median tract, in eastern Queens, the Bronx and southern Brooklyn, where jobs grew fastest in proportion. Resident workers grew most in Harlem, the South Bronx and central Brooklyn.

The sign is the opposite of what the model's supply response would give. The record cannot say why: rents are not in it, nor the rezonings, nor who moved.

### 4.4 The Composed Model Does Not Find Where They Went

The model reproduces the total (+23.9%) at a speed of building of 0.057 a year. Where the growth went correlates −0.04 with the record across the 2,215 tracts, and its R² is −0.09. Uniform growth does better.

The mechanism the model holds, in which access raises rents and rents bring building, did not describe New York's decade of change. That is consistent with §4.3.

## 5. What It Does Not Show

- **Not that the mechanism is wrong elsewhere.** New York's supply is set more by its rezonings and conversions than by rents a year earlier. A city whose building does follow rents may differ, and the model takes another place's record as a case.
- **Not the subway.** Costs are straight distances. The pairs that distance misplaces include every line of the subway; a network of transit times is the next cost to try.
- **Not households.** LODES counts jobs held by residents, not households, and a worker with two jobs counts twice. The model's households are those workers.

## 6. Reproducing

```
python data/products/commuting.py nyc-lodes                               # about 420 MB of LODES's files, kept in data/cache/raw/lodes
python research/experiments/2026-11-12-land-use-and-transport/run.py      # builds study.cpp in WSL, about 5 minutes
python research/experiments/2026-11-12-land-use-and-transport/analyse.py  # the figures, to assets/gallery/
```

`results/summary.json` holds every number above, and `results/tracts.csv` each tract's workers, jobs, accessibility and the model's 2019.

## 7. Figures

In the gallery's section on land use and transport:
- `luti-04-decay`: β and the mean journey by year;
- `luti-05-forecast`: the four forecasts;
- `luti-06-access`: the change of access and of resident workers on the map;
- `luti-07-model`: the composed model against the record.
