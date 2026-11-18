# Needs in the City of Melbourne, Searched by Evolution

*Status: draft (2026-11-18; [ADR 0125](../../../development/decisions/0125-the-evolutionary-exploration-and-validation-of-needs.md)).* A record of **research**: what the needs domain's search finds in the City of Melbourne at the 2021 Census, and how far each finding is shown.

## 1. The Question and the Data

The question is which groups of the City of Melbourne's people have needs in care, income and carer support, and how strongly the data show each.

The data are `data/products/needs_tables.py`'s case `melbourne-assistance`: the 2021 Census by SA1, sex and age band. That is 353 areas, 5,837 rows and 149,049 people. Three indicators are read, each a share of those who stated it:
- **need for assistance** (G18): help with self-care, mobility or communication, for a profound or severe disability;
- **low income** (G17): a personal income under $500 a week, for those aged 15 and over;
- **unpaid care given** (G25): for those aged 15 and over.

Each area's tenure, social housing, flats, household income and moves are attributes groups may be stated in. The suppliers of care are CLUE's blocks with care floor space, one person served per 40 m² (declared). The causal graph and the rules are declared, illustrative and no institution's.

## 2. The Method

The search ran with its defaults, by `run.py`:
- a population of 60 statements and 60 children a generation, with (μ+λ) survival and tournament parents;
- objectives: the gap's z, people concerned, and fewest conditions;
- the three parts of whole areas by a keyed hash: 174 areas for discovery, 73 for validation, 95 for confirmation (some areas hold no row);
- admission: z ≥ 3, an effect of at least 0.2 of the indicator's spread across areas, stability 0.8, each condition's own z ≥ 2, distinct by δ = 0.25, at least five areas, and the holdout's agreement;
- confirmation: e-LOND at α = 0.05, valid under any dependence.

## 3. What Was Found

Ten generations took 1.8 s:
- 537 statements were judged, 55 admitted and 37 kept (18 retired, most as superseded);
- 28 were confirmed on the areas kept aside;
- the holdout was asked 118 times and told the validation value 9 times, of a budget of 100;
- the journal of confirmations ends in `ee26afd5…ac52`.

The kept statements are in `kept.csv`.

- **Income.** Young people (15 to 24) mostly have personal incomes under $500 a week. The share is 56% against 17% across the city outside Southbank, and 70% in Carlton and Parkville in areas of low household income. These are students around the universities. Confirmed.
- **Carer support.** Older people in settled areas, where at most two in five moved in a year, give unpaid care at 15% against 4%. Women aged over 35 there give it at 17%. Confirmed.
- **Care.** People over 25 in areas mostly renting and settled need assistance at 17% against 1.3%, and those over 45 in settled areas at 11.6% against 0.4%; both are confirmed. Those over 65 in areas of lower household income need it at 53% against 1.0%. That statement is admitted but not confirmed by e-LOND, which is conservative. These are probably homes for the aged, whose residents the census counts where the home is: the search found them without being told.
  - The simulation of seeking holds for every care statement: the groups live farther from the blocks with care floor space, and the model leaves more of their demand unmet.
  - Provision's effect within the group, adjusted for household income and flats, is shown for one statement only and is not shown for the rest.

## 4. Strategies of Survival Compared

Ten generations each, the same data and options:

| Survival | Kept | Admitted | Confirmed |
|---|---|---|---|
| (μ+λ) | 37 | 55 | 28 |
| generational | 45 | 57 | 31 |
| aging | 48 | 68 | 35 |
| deterministic crowding | 38 | 55 | 28 |
| restricted tournament | 40 | 62 | 31 |
| clearing | 42 | 59 | 26 |
| NSGA-II | 44 | 64 | 31 |
| ALPS | 41 | 52 | 24 |
| MAP-Elites niches | 52 | 73 | 35 |

Every strategy finds needs in all three domains. Niches and aging admit and confirm the most here; one run each is no comparison of algorithms. Confirmations count statements, not distinct needs.

## 5. Caveats

- A census is a full count, read as a sample of the process behind it, with its areas as clusters. A need of this census is a need of the population as its questions measure it.
- A gap is not a cause. Causal support tests the declared graph, not the world.
- The 2021 Census was taken during a lockdown, when many international students were abroad. The centre's people that night were not an ordinary year's.
- The suppliers' capacity, the catchment and the graph are declared.
- The productivity screen at z = 2 lets through some refinements that carry no gap of their own; the archive's statements about young people on low incomes are variants of one need.
