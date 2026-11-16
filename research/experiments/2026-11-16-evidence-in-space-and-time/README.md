# Evidence in Space and Time: Rats in the East Village and the Lower East Side, 2024

*Status: draft (2026-11-16; [ADR 0123](../../../development/decisions/0123-evidence-in-space-and-time.md)). Activity: research.* The example of the Urban Studio's plug-in *Evidence in space and time*, worked through and held against what came after.

## 1. Question

Two public records of New York City report rats at places and times: the health department's initial inspections of lots, and residents' complaints to 311. Taken as evidence about where rats are active, they raise three questions:

- what a complaint is worth, counted against what inspectors found;
- what the reports of one quarter establish about each place, when each report is counted only as far as it may be about the place, and repeated reports count once;
- whether what the archive concludes holds up against the inspections of the quarter after.

## 2. Records and Assumptions

- **Records** (NYC Open Data, fetched by `data/products/urban_evidence.py`, case `nyc-rodents`): the Department of Health and Mental Hygiene's Rodent Inspection (p937-wjvj) and 311's Service Requests of the type Rodent (erm2-nwe9), in a box over the East Village and the Lower East Side (40.7185–40.7335° N, 73.9925–73.9745° W), January to September 2024. Both files are holdings with their SHA-256 and chains of custody.
- **The archive** holds April to June: 1,461 initial inspections and 105 complaints as items at their places (good to 15 m) and days; complaints from one address share an origin. 399 claims that rats are active within 40 m of a point, on a lattice of 80 m. Each claim's items report events of their own (lots inspected, sightings), not one event.
- **Counted,** from January to March: the resident's channel, lot by lot (§3.1); the prior of each claim, the share of areas looked at that had rats somewhere (50%); and P(E | H), the share of an area's inspected lots found with rats when the area had some (26%).
- **Declared, not counted** (the example's assumptions, said so in the archive):
  - an inspector's finding: rats found where they are 90% of the time, where they are not 2%;
  - the acts: treating an area costs one unit; leaving rats alone for a quarter costs six; a survey of an area finds activity that is there four times in five, reports it where there is none once in twenty, and costs a quarter of a treatment; forty surveys can be paid for.

None of it is advice to anyone.

## 3. Results

### 3.1 A Resident's Channel

Of the 213 lots inspectors found with rats in the first quarter, 26 had a complaint (12.2%; Wilson's 95% interval 8.5% to 17.3%). Of the 1,217 they passed, 22 had one (1.8%; 1.2% to 2.7%). A complaint is about 6.8 times as likely where rats are found as where they are not, and its absence 0.89 times as likely.

*Caveat.* The lots counted are those inspectors chose, many because of a complaint. The channel is the one the archive needs (a complaint against an inspected lot), not a resident's accuracy in general.

### 3.2 What the Quarter Establishes

Under the declared rule (established at 0.9 or above, refuted at 0.05 or below):

- 80 claims are established, 3 refuted and 189 open; 127 have no item bearing on them and stand at their prior.
- **Repetition counted once.** The largest overstatement, the ratio counting a claim's items as independent against the one held, is 15.8: one household's repeated complaints, which as independent reports would have given 3.7 × 10⁴ in place of 2.4 × 10³.
- **Location dilutes.** An item's force is diluted by the share of its disc and span that falls in a claim's. A passed lot wholly about a claim divides its odds by 1.3; one 31 m from its centre, about it at 84%, by 1.2.

### 3.3 What to Do

On those posteriors, 386 claims are treated and 13 left. Thirty-seven surveys, costing 9.25 of the 10 units, are worth buying first: they are expected to improve what is done by 7.46 units, and no other set within the budget gains more (calx's knapsack, with its bound as certificate).

### 3.4 A File Altered

The complaints' file was altered on disk. At the next use its digest no longer matched the one of its last handover, and the 105 complaints resting on it no longer counted. Weighed again without them: 69 established, 3 refuted, 194 open, 133 with nothing bearing on them; 40 surveys are now worth buying, expected to improve what is done by 8.67.

### 3.5 Held Against the Quarter After

For each claim, the archive predicts what a later inspection within it finds, through the inspector's declared channel. Of the 399 claims, 227 had inspections from July to September (`outcome.csv`, kept out of the archive).

| Score (lower is better) | Brier | log loss |
|---|---|---|
| The archive | 0.234 | 0.666 |
| The prior alone | 0.249 | 0.704 |
| Repeated reports as independent | 0.234 | 0.666 |

| Predicted | Found with rats | Mean prediction |
|---|---|---|
| 0.0 to 0.3 | 35% | 19% |
| 0.3 to 0.5 | 42% | 40% |
| 0.5 to 0.7 | 59% | 58% |
| 0.7 to 1.0 | 76% | 85% |

The archive predicts better than the prior. It is well calibrated in the middle and overconfident at both ends.

## 4. Discussion

- **The ends.** Overconfidence at both ends is what neighbouring claims taken as independent of one another should produce: their states are shared over a few blocks, and each claim is weighed alone. A model of the place beneath the claims (ADR 0123's first item not built) is what would address it.
- **Repetition did not move the scores here.** Counting repeated reports as independent gives the same scores to three decimals. Only a few claims carry a household's repeated complaints, and those are established either way. The correction matters claim by claim (a factor of 15.8 on one), not on average in this record.
- **What is presumed.** The quarter after differs from the archive's in season and in where inspectors went. A prediction that misses may be wrong, or the place may have changed; the tool says both.

## 5. Reproduction

```
python data/products/urban_evidence.py
python data/acquisition/recipe.py applications/urban-studio/recipe.json
python applications/urban-studio/serve.py --open
```

Begin a project from the template *Rats in the East Village and the Lower East Side*, lay out its archive from the *Evidence* view, and run the plan: *A kind of report's channel, counted* on `calibration`, *Claims weighed and decided*, and *How the conclusions fared* on `outcome`. The figures above are those of a run on 2026-11-16, fetched that day. The pictures are in the gallery's §43.
