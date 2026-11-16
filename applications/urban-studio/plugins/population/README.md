# Population Dynamics

*Status: draft (2026-11-16; [ADR 0124](../../../../development/decisions/0124-population-dynamics.md)).* The Urban Studio's sixth plug-in: a place's people over half a century, cell by cell, read as a field that moves and grows, its laws learned as a fluid's, judged, and run ahead.

## 1. Concepts

- **A place's people is a field.** The field is GHS-POP's estimate of the residential population at every fifth year from 1975 to 2030 (2025 and 2030 are projections). It is carried onto a square grid of 1 km in the place's UTM zone, with nobody lost (`data/products/population_history.py`). Any place may be made from its latitude and longitude.
- **Its laws are a fluid's.** A law is a balance of what crosses each cell's faces and what is made within it:
  - growth, exponential, logistic or Gompertz's;
  - spread down the gradient (Fick's diffusion);
  - crowding, a diffusivity that grows with the density (the porous-medium equation);
  - drift toward the centre, or along the settled landscape.

  With growth and spread together it is Fisher's law, whose front of settlement advances at 2√(rD). The mathematics is kinesis's `balance`, on a graph of cells whose coast lets nothing through.
- **Learned from a dozen snapshots.** Each law is fitted on the years before those kept aside, by the integral form and then by runs fitted to the record (shooting). A spread found negative is ill-posed, so it is set aside and said to be.
- **Judged on the years kept aside**, against carrying the last field forward. Each law gets two skills:
  - *how many*: how well it forecast the total;
  - *where*: how well it placed the people once its total is set to the record's.

  The second is what a law of the field is about. The first depends on births, deaths and who moves in and out, which no law of the field knows.
- **Beside the laws:** a rate of its own for each cell, a law thresholded from every term, and the record's dynamic modes. The modes are the coherent structures fluid dynamics finds in a flow, here by the method of snapshots.
- **Run ahead.** A law learned on every year recorded runs on to 2050 with its currents of people. A new source of people may be placed by a click, and the spread scaled. A run is what the law does if nothing it does not hold changes, not a forecast of the place.

## 2. View

| View | Shows |
|---|---|
| *Population* | The field as the page: each km² painted by its people (orchid, pale for few and deep for many, on a logarithmic scale), the sea left as ground, a ribbon of years beneath to step or play through. *As recorded*: the record, or its change since the year before (lavender for loss, rust for gain), with everyone in the square over the years. *Laws learned*: the laws ranked by where they place the change, each with its two skills, its coefficients in words and numbers, and its currents as arrows; the best one's forecast, the record and its error for the years kept aside. *Modes*: the eigenvalues about the unit circle and each mode's map. *The years ahead*: a law chosen, its spread scaled, a new source placed by a click, the run to 2050 with its currents and its total against the JRC's projections. A km² opens where it stands, with its people over the years. |

## 3. Tools

| Tool | What it answers |
|---|---|
| *A place's people, 1975 to 2030* | The field's cells as a table placed on the map, a column a year, and the totals as a chart; a place made from its latitude and longitude when it is not yet |
| *How the whole place grew* | The total by exponential, logistic and Gompertz's growth: each fitted, judged on the years kept aside, and run on, a law that accelerates drawn up to where it runs away |
| *Laws of its people, learned and judged* | Every law with its skills, its parameters and what it says in words; the best forecast against the record for the last year kept aside, on the map; a law of one's own declared as its terms |
| *The record's modes* | The modes with their rates, periods and sizes, and their maps |
| *The years ahead under a law* | A law run on to a year, with a source placed and its spread scaled: a column a year on the map, the currents, the total against the projections |

## 4. Templates

*Melbourne's people, 1975 to 2050* and *Lagos's people, 1975 to 2050*: the place's field, and a plan of the five tools.

## 5. Server Side

`routes.py`, under `/api/projects/<project>/population/`:

- the places the data library has made, and a place's field;
- `growth`, `learn`, `modes` and `simulate` asked of the engine. These are computed and keep nothing, and are kept in memory while the field and the question are the same;
- the project's study (the place, the law, the sources placed), and a place made from its latitude and longitude. Both are for the work's own page or its people.

## 6. Dependencies

kinesis's `balance` and calx, through the API's `science/population.hpp`, built by the work's recipe into `data/cache/engines/urban-studio/population/`; the API's `science.population`; the visualization layer's `dynamics/fields.js` and `dynamics/charts.js`; the data library's `population_history.py`.
