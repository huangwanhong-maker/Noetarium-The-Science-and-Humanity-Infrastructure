# The Indexes of Cities

Status: **draft** (2026-11-18; [ADR 0128](../../../development/decisions/0128-indexes-and-cities-across-scales.md)). Data of the urban-science domain: what a city's indexes are, how each is carried from one scale to the next, how it is observed and how it may be estimated where it is not. Activity: use, research.

## 1. Concepts

An **index** is a quantity or an assessment that a study of a city reads off a record: a vacancy rate, a Gini coefficient, a floor area ratio, a net present value, a perceived safety, a stakeholder's salience. It is quantitative (a count, an amount, a flow, a rate, a ratio, a density, a price, a duration, a statistic of a distribution, a coefficient, a composite, a network or spatial statistic, a physical quantity, a valuation) or qualitative (ordinal, categorical, binary, fuzzy, latent, coded), and qualitative indexes are as much indexes as the others: each says its measurement model (its items and scale, its rubric, its coding, its calibration).

How an index is **carried up** a hierarchy of units is part of what it is: summed (extensive), averaged with a declared weight (intensive), a ratio of sums, decomposable into within and between, recomputed from the units beneath (Gini, the dissimilarity, a median), a maximum or minimum, the median of an ordinal scale, or meaningful at one scale only. A study across scales carries the sufficient statistics (counts, sums, distributions) and computes each index from them at every level; it never averages an index that must be recomputed ([the theory note](../theory/indexes-across-scales.md)).

## 2. The Catalogue

`catalogue.tsv` holds one row per index, 1,536 of them (1,366 quantitative, 170 qualitative), drawn from a keyword atlas of urban economics, urban science, market analysis, political economy and resilient infrastructure that the user gave (4,591 keywords in 30 sections). Its columns:

| Column | What it holds |
|---|---|
| `id`, `name` | the index's identifier (snake_case, British spelling) and its name |
| `class`, `kind` | quantitative or qualitative, and its kind |
| `unit` | SI, currency per period, 1 for dimensionless; for a qualitative index, its scale |
| `aggregation` | how it is carried up: `sum`, `wmean:<weight>`, `ratio-of-sums:<numerator>/<denominator>`, `decomposable`, `recompute`, `max`, `min`, `median`, `none` |
| `scales`, `tempo` | the scales at which it is meaningful, and the time scale on which it changes |
| `definition`, `source` | a definition (a formula where there is one) and its standard reference |
| `observed_by`, `estimable_from`, `estimability` | the records that observe it, how it may be estimated where it is not, and how well (high, medium, low, none: a quantity to declare) |
| `role`, `law` | its role in a dynamical model (state, flow, parameter, driver, output, observation) and, for a state or flow, the standard law that advances it |
| `computed_by` | where the infrastructure already computes it (73 indexes) |
| `sections`, `keywords`, `places`, `notes` | where the atlas names it, by which keywords, places it was named for (Ginza), and remarks |

`keywords.tsv` accounts for every keyword of the atlas, in its order: the index or indexes it names or is operationalised by (2,277 keywords), or why it names none (2,314: a method, 641; a concept with no standard measure, 975; a policy instrument, 347; a document or instrument, 230; an actor or organisation, 99; a place, 22).

*Caveat.* The catalogue was drafted by classifying the atlas with a language model, without browsing; its definitions, formulas and sources come from the model's knowledge and are **unchecked**, as a statement of grade C is in the infrastructure's records. It is a working map of what to compute, not a reference to cite. An index enters the infrastructure's code with its source checked and a check of its value; `computed_by` names those that have.

*Caveat.* Near-duplicates were merged where they were the same index under two names (28 pairs); variants that differ in meaning were kept apart (an office and a retail vacancy rate). Some generic ids stand for several subjects (`risk_rating`), named in their notes.

## 3. Uses

The Urban Studio's plug-in *Cities across scales* reads the catalogue: it computes the indexes a city's records give at every level of its hierarchy (by kinesis's systems across scales), shows the catalogue with each index opened in place, and names each computed index by its id here.

## 4. Checks

The domain's suite reads both tables: every row has its columns, every class, kind, aggregation, estimability and role is one of the vocabulary's, every id is one row, every keyword's index is in the catalogue, and the catalogue's counts are those stated above.
