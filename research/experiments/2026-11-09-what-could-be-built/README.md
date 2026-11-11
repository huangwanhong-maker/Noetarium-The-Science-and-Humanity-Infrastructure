# What Could Be Built on Three Blocks of Melbourne

*Status: draft (2026-11-09). Research.* The first study of the generative design loop ([ADR 0089](../../../development/decisions/0089-what-could-be-built.md)): the API's composition (`api/cpp/science/design.hpp`) on the architecture domain's `rules`, `form`, `frontage` and `environment`, urban science's configuration and access, and heuresis's NSGA-III, run on the data library's city model without the workbench's page.

This is a study of methods on real places, not a planning proposal. The rules are declared assumptions, none of them Melbourne's planning scheme; the lots are the buildings' footprints as built, since parcels are not in the open data; and nothing here says what should be built.

## 1. The Question and Why

A block can be built in many ways, and every way is better in something. More floor casts more shadow on the street; towers set back from the street let in sun but expose more wall to the cold; a passage through the block joins streets but takes ground from its lots. The question is what the rules of a street wall and towers, of mid-rise and of towers apart allow on real blocks, and what each trade costs, measured by six models from two domains at once.

1. Under each rule set, how much of the floor standing now can a block hold?
2. How much winter sun can the streets gain, and at what floor?
3. Is any design warmer per m² than what stands?
4. What does a passage through the block do to the streets' configuration and to what lies within a walk?

## 2. Method

- **The blocks.** Three blocks of the data library's city model of Melbourne (`city-model-melbourne.json`, OpenStreetMap), each the face of the streets about a point:
  - Collins Street to Flinders Lane, Swanston to Russell (15 lots, 19,848 m²);
  - Bourke Street to Little Bourke, Elizabeth to Swanston (17 lots);
  - a block of Carlton (31 lots of row houses).
- **The requests** are built as the plug-in's page builds them (`run.py`, after `plugins/architecture/layers/design.js`):
  - the streets of the classes that bound blocks within 700 m; the ways walked within 1,100 m;
  - the buildings within 600 m, at their mapped heights (9.6 m where none is mapped).
  - The page also takes the world's tiles' buildings beside a city model; here the city model alone stands.
- **The Sun and the day.** The Sun at each hour from 10:00 to 14:00 solar time on 21 June 2026, from the astronomy domain (`sun.mjs`); the air at 8 °C by day and 4 °C by night, declared.
- **The rules** (the same three as the page's):

  | Rules | Height | Plot ratio | Street wall | Above it | Setbacks | Street line walled |
  |---|---|---|---|---|---|---|
  | a street wall and towers | 100 m | 12 | 24 m | set back 10 m | none | 60% |
  | mid-rise | 32 m | 4.5 | 16 m | set back 5 m | none | 80% |
  | towers apart | 150 m | 8 | 9.6 m | set back 4 m more | 6 m from street and neighbours | none |

- **A design** gives each lot its podium's storeys, its upper part's storeys and the share of the upper envelope that part covers (a quarter to all of it, moved in evenly). It may open one passage, 6 m wide, across the block's middle or along it. Every massing complies with the rules by construction, and each is checked against them.
- **The measures.**
  - floor;
  - winter sun on the streets, in m²·h of open ground about the block;
  - winter sun on the neighbours' faces;
  - the heat a winter day needs per m² of floor (a balance of the envelope, `balance.hpp`);
  - the mean integration (angular, radius 800 m) of the streets within 300 m;
  - the floor within a five-minute walk of each lot's door, weighted by the lot's floor.
- **The constraints.** Each design keeps half (or a tenth) of the floor standing now, and walls the share of the street line its rules ask.
- **The search.** NSGA-III on the six measures, each relative to the block as built, 128 designs for 60 generations (7,680 evaluated) on eight threads. Every rule set and floor kept is run with seed 1; Collins Street under a street wall and towers is also run with seeds 2 and 3.

## 3. Code Version

| Library | Commit |
|---|---|
| Noetarium (root) | the commit of ADR 0089, on `887e4b2` |
| physis | the commit of its decision 0025, on `e967d58` |
| heuresis | `d267ccd` |
| calx | `90aa710` |
| morphe | `c8b4fc7` |

The study's program (`study.cpp`) is compiled by `run.py` with g++ 11.5 in WSL. Node 22 ran `sun.mjs`, and the repository's Python 3.12 ran `run.py` and `analyse.py`.

```
python research/experiments/2026-11-09-what-could-be-built/run.py
.venv/Scripts/python.exe research/experiments/2026-11-09-what-could-be-built/analyse.py
```

## 4. Data and Provenance

| Data | Source | Licence | Retrieved |
|---|---|---|---|
| buildings and ways | the data library's city model of Melbourne (`city-model-melbourne.json`, built 2026-10-31) from OpenStreetMap | ODbL 1.0, © OpenStreetMap contributors | 2026-10-31 |
| the Sun | the astronomy domain's low-precision theory (Astronomical Almanac, §C5), 0.01° | — | — |

`results/requests/` holds the requests and answers, which are extracts of OpenStreetMap, so they are not committed (`.gitignore`). `results/runs.json` and `results/summary.json` hold only the designs' measures.

## 5. What Was Found

1. **In the Hoddle Grid the rules hold less floor than stands.**
   - **Collins Street.** Under a street wall and towers, the most floor any design holds is 96%, 97% and 98% of the 181,000 m² standing now, over three seeds.
   - **Bourke Street.** The most is 78% of 199,000 m².
   - **Mid-rise.** Its plot ratio of 4.5 allows less than half the floor standing on either block, so nothing keeps half. Keeping a tenth, it holds at most 41% and 36%.
   - **Why.** The blocks' buildings fill their footprints to the street and to each other. Towers set back 10 m above a street wall cannot hold what towers on the lot line do.
2. **Winter sun on the streets can rise by half, at a cost in floor.**
   - **Collins Street.** The most winter sun on the streets is 38%, 48% and 54% above today's over the three seeds. Among designs keeping four fifths of the floor, the best gains 10–16%.
   - **Towers apart.** Keeping a tenth of the floor, they bring 3.6 and 4.1 times today's winter sun, at a fifth to two fifths of the floor.
3. **No design is warmer per m² than what stands.**
   - Over every run and every design, none needs less heat per m² on a winter day than the block as built.
   - **Collins Street.** The best needs 7–20% more than its 97 Wh/m², over the three seeds.
   - **Why.** The buildings standing share their walls and fill their lots, so little of their envelope meets the air. A set-back tower exposes four walls to the air above its podium.
4. **A passage moves configuration little and access slightly.**
   - **Configuration.** A passage through one block changes the mean integration of the streets within 300 m by at most 0.2% at radius 800 m.
   - **Access.** The floor within a five-minute walk changes by up to a tenth either way.
   - **The front.** Most designs open the passage across the block (76–86 of 127–128 on Collins Street): it costs little floor and adds frontage and sun.
5. **Carlton is the other way round.**
   - **More floor.** A street wall and towers can double the floor of its row houses, at a plot ratio of 2.7 now.
   - **More sun.** Or they can bring 16% more winter sun to its streets.
   - **Towers apart.** They find nothing: their 6 m setbacks swallow lots of row houses.
6. **The search has not converged.** The extremes move by up to 10% between seeds (the most sun 1.38–1.54 times today's). The fronts are good designs, not the best possible ones.

## 6. What It Means, and What It Does Not

- **The rules decide more than the search.** In the Hoddle Grid the floor lost to setbacks is the main trade, and the search mostly chooses where to give it up.
- **Heat per m² favours the city as built.** This measure leaves out storage, ventilation, internal gains and daylight, which favour the slender forms the sun favours. It compares envelopes, and it does not predict bills.
- **The lots are footprints.** On real parcels the envelopes would be larger, and the floor held by the rules closer to what stands.
- **Not done.**
  - Parcels from cadastres where they are open.
  - Uses on each floor, so that access counts homes and jobs, not floor.
  - Shading by the sky's diffuse light, and roofs shaded by their neighbours.
  - The heat of the massing with its storage (`massing_heat.hpp`), for the designs of the front.
  - A converged search: more generations, or a seeded population.

## 7. Figures

`analyse.py` writes `assets/gallery/design-05-front.jpg` (the front of Collins Street's block) and `design-06-blocks.jpg` (three blocks under three rules); the gallery's §25 shows them with the plug-in's pictures.
