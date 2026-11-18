# Cities Across Scales

*Status: draft (2026-11-18; [ADR 0128](../../../../development/decisions/0128-indexes-and-cities-across-scales.md)).* The Urban Studio's eighth plug-in: a city's indexes at every scale at once (blocks within areas within the city), and models of them, fitted where the records allow and declared where they do not.

## 1. Concepts

- **The indexes of cities.** The urban-science domain's catalogue (`domains/urban-science/indicators/`) names 1,536 indexes, quantitative and qualitative, drawn from a keyword atlas of urban science and urban economics; each says how it is carried from one scale to the next (summed, a weighted mean, a ratio of sums, recomputed from the units beneath, or meaningful at one scale only), how it is observed and how it may be estimated. It was drafted by a language model and is unchecked.
- **Every index at every level, by the model's own operators.** The records' quantities (floor space by use, establishments, dwellings, land) are summed up the levels and every index is computed on each level's sums, so that a ratio is the ratio of sums at every level; an index among the units beneath (the dissimilarity of homes and offices, the inequality of floor area ratio) is recomputed from them. The mathematics is kinesis's systems across scales.
- **What the census withheld.** A cell pertaining to two or fewer businesses is blank while the row's total is true. Each blank cell lies between nothing and the row's withheld floor, the census's own bound; within it the floor is shared by tyche's dasymetric carriage on prior weights, keeping every row's total. In Melbourne, 2002–2024, 30% of the floor space counted is withheld.
- **An index where it was not observed** is estimated from other columns, judged on blocks of rows held out together, given conformal intervals, and withheld with its reason where it does not beat the mean, lies outside its area of applicability, or is wider than the use allows (by default half the estimate). A withheld estimate is a quantity to declare.
- **Models across scales** are quantities on levels, operators between levels and laws each at its pace. *Where growth goes* shares the city's change of a use among areas and blocks by logits, nesting proportional allocation; *the vacancy cycle* (after DiPasquale and Wheaton) moves stock by rents that no record holds, declared. Each is fitted on the years before those held aside, with what the record does not identify said, judged at every level against standing still, and run over draws of what is declared, the declarations ranked by what they move.

## 2. View

| View | Shows |
|---|---|
| *Across scales* | *Indexes*: the city's blocks drawn as they lie, toned by an index read at the block, at its area or for the whole city (the areas named when read at the area; a block with no value hatched), with a ribbon of census years; the indexes the records give, the chosen one's place in the catalogue and how it is carried up, the city's value, and the city with its areas over the years. *Models*: a model drawn as its levels, the coarsest at the top, each quantity by its role (a state, a driver, a fixed value, a derived quantity), the operators between levels as arrows (up for a sum, dashed down for a parent's value), its laws beneath, what the records do not hold ringed and listed to be declared, and its unknowns with their bounds. *The catalogue*: its counts by class, by how indexes are carried up, by estimability and role, searched, each index opened in place with its definition, source, estimation and law. |

## 3. Tools

| Tool | What it answers |
|---|---|
| *Indexes across scales* | Every index for the blocks (placed on the map), for the areas with the indexes among their blocks, and for the city over the years; the ratio of sums set beside the mean of the blocks' ratios |
| *What the census withheld* | A use's floor space by block, stated, estimated and at most (placed on the map); by area, with each area's share of floor withheld |
| *An index where it was not observed* | A column of a table estimated in its empty rows from other columns, or withheld with the reason; the estimator's skill on blocks held out, its interval, and how much each covariate weighs |
| *Where growth goes, across scales* | The fitted unknowns with their standard errors; the change of the years held aside placed at the areas and the blocks, fitted against proportional allocation and standing still; each block's change, recorded and modelled (placed on the map); how lumpy growth was |
| *The vacancy cycle, and what must be declared* | What the record identifies of the four rates and what is declared instead; the city's vacancy recorded and under draws of every declaration; the declarations ranked by what they move |

## 4. Template

*Melbourne across scales, 2002 to 2024*: the City of Melbourne's 603 CLUE blocks in 13 small areas, and a plan of the five tools (the indexes in 2024, homes withheld in 2024, where homes and offices went, and the vacancy cycle).

## 5. Server Side

`routes.py`, under `/api/projects/<project>/scales/`: the cities the data library has made and the project's study; the catalogue in brief with its counts; every index at every level and year (`indexes`); a model of the plug-in read by the engine (`describe`). What is computed keeps nothing and is kept in memory while the city's product is the same; the study is kept from the work's own page.

## 6. What Melbourne Shows

Fitted on 2002–2016 and judged on 2017–2024: growth comes in buildings (the 5% of blocks that gained most took 70% of the homes and 90% of the office floor added in 2016–2024), so a model of shares places the change among blocks no better than chance and errs by more than standing still, though it places part of it among areas; the vacancy cycle's rates are not identified by thirteen areas over fourteen years and are declared, and with the default declared ranges its median overshoots the record. The city's change is given, not forecast.

## 7. Dependencies

kinesis's `scales`, tyche's `spatial` (change of support, prediction) and nomos's measures, through the API's `science/scales.hpp`, built by the work's recipe into `data/cache/engines/urban-studio/scales/`; the API's `science.scales`; the visualization layer's `dynamics/scales.js`, `dynamics/fields.js` and `dynamics/charts.js`; the data library's `city_scales.py`; the urban-science domain's catalogue.
