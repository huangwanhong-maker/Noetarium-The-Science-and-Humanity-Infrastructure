# Urban Studio

*A studio for the analysis of cities and the life in them, project by project: one's data as they are, slices of space and time, tools brought by plug-ins or written as one's own scripts, what they answered drawn as maps and charts, opened in the Urban Science Workbench in three dimensions, and written about on boards.* Status: **draft** (2026-11-13; [ADR 0102](../../development/decisions/0102-the-urban-studio.md)). Activities: research, use, learning. Port 9090.

## 1. Concepts

### 1.1 A Work of Studies

The Urban Science Workbench is where a city is seen: the Earth to a doorstep, in three dimensions, with the clock. The Urban Studio is where a city is studied. A question becomes a project, with:

- its data bound by their columns' roles;
- its slices of space and time;
- its quantities with their ranges;
- the tools run, each answer kept;
- a register of the methods used;
- boards on which it is written about.

What a study places on the Earth goes to the Workbench to be seen there (§1.4).

### 1.2 Subjects Arrive as Plug-ins

The Studio knows no subject. A plug-in brings one: its tools, its templates, its engine and its declared schemes. The first plug-in is *Ecology and life* (§3), the second *Time, care and provision*. Later ones (energy and water, the material stock of buildings, health, culture) join the same way, and nothing else of the work changes. A tool is a module of the board's contract (`api/python/science/board.py`), so the same tool runs on a board of Tabula and in Market Studio's projects.

### 1.3 Programmable

A project holds scripts of its own: Python modules that declare their parameters and answer tables, notes and figures. They may use numpy, the data library and the domains through the API (`science.engines`, `science.ecology`). A script written in the Studio runs on a board, and the other way round.

### 1.4 Joined to the Workbench by a Document

The two works share no code; what both need lies below them. A placed table of a project is written as a record in space and time (`visualization/js/gis/spacetime.js`'s events) under `data/cache/urban-studio/exports/`. The Workbench shows it from `?record=<path under /data/>` and flies to it. The key *In the Workbench*, beside a table, does both.

### 1.5 Presumed, Not Observed

Every answer separates what was counted from what was estimated and from what was presumed. The schemes a tool offers are files that name their sources, and every presumption is a parameter shown in the step's form.

## 2. The Page

At the left, the project:

- its slices, data and quantities;
- the tools by group;
- what they answered;
- its views and its boards.

In the middle, what is looked at: a map that moves with a clock, a table, a chart, a board. At the right, what is selected, with a form built from what it declares; for the project, its plug-ins and the methods it used. Beneath, what happened.

The page is the visualization layer's studio (`visualization/js/workspace/studio.js`), started with the Studio's name. Its look is heather and moss, provisionally, until the user chooses:

- heather (#9B72A6) only on what is chosen or in the hand;
- moss (#6E9171) for what lives;
- paper with a breath of green by day, a dusk of aubergine by night;
- hairlines, serif type, generous air.

## 3. Plug-ins

| Plug-in | What it brings | Status |
|---|---|---|
| [Ecology and life](plugins/ecology/README.md) | Ten tools on the ecology domain ([ADR 0101](../../development/decisions/0101-ecology-founded-the-urban-forest.md)): what a forest is made of against declared limits; diversity, and what a fuller count would find; how far apart its kinds are; carbon tree by tree; the forest's future under removals and planting; the forest by size as a stage model; any population by stages; occupancy, though not always seen; the canopy people live under; how the canopy holds together as habitat. Two templates (Melbourne's urban forest; New York's street trees, 2015), its engine, two declared schemes | draft |
| [Time, care and provision](plugins/provision/README.md) | Eight tools on the sociology and social-provision domains, with demography's health and economics' labour ([ADR 0104](../../development/decisions/0104-sociology-founded-time-and-relations.md), [ADR 0105](../../development/decisions/0105-social-provision.md)): how a day is divided; unpaid work and who carries it; who has too little time; time moved between activities; years lived in health; care ahead; a labour market's flows; ties among people. One template (Time and care in Europe, on seven Eurostat datasets) and its engine | draft |
| [Materials and flows](plugins/materials/README.md) | Seven tools on the industrial-ecology domain and civil engineering's water ([ADR 0106](../../development/decisions/0106-industrial-ecology-founded.md), [ADR 0107](../../development/decisions/0107-civil-engineering-founded-water-networks.md)): what the buildings hold; what would renew the stock; a stock over time; flows that balance; a life cycle; what final demand causes; a water network. Three templates (the material stock of Melbourne's centre; what the EU's final demand emits; EPANET's Net3) and its engine | draft |
| [Appraisal and futures](plugins/appraisal/README.md) | Four tools on nomos's appraisal ([ADR 0108](../../development/decisions/0108-appraisal-futures-and-the-agreement-of-coders.md)): alternatives compared, limits first; costs and benefits; Delphi rounds and morphological fields; how far coders agree. Its engine | draft |

A plug-in is a directory `plugins/<name>/`:

- `plugin.json`: its title, what it is about, its version and colour, the domains it stands on, its theory note;
- `tools/*.py`: a group of tools under its name;
- `templates/*.json`: projects to begin from, with a plan;
- `engine/main.cpp`: a C++ engine on one of the API's compositions, built by `engine.py` through `science.engines` into `data/cache/engines/urban-studio/<name>/`;
- `schemes/*.json`: declared files its tools offer as choices.

The work's own tools are in `tools/tables/`: describe, gather, compute and density.

## 4. Running It

```
python data/acquisition/recipe.py applications/urban-studio/recipe.json    # the engines, and the examples' data
python applications/urban-studio/serve.py --open                            # http://127.0.0.1:9090/
```

The recipe compiles each plug-in's engine with g++ (natively, or in WSL) and fetches the examples' data:

- Melbourne's urban forest, its tree canopies of 2021, its census blocks and dwellings (City of Melbourne, CC BY 4.0);
- New York City's 2015 Street Tree Census (NYC Open Data);
- the products made from them by `data/products/urban_forest.py`;
- seven Eurostat datasets (time use, healthy life years, limitation in usual activities, life tables, population, its projection, labour market transitions; CC BY 4.0) and the product made from them by `data/products/time_and_care.py`.

A project on one's own data needs only the engines. The page builds them too, from the project's panel. Projects are kept under `data/cache/urban-studio/projects/`, never committed. Only 127.0.0.1 is bound unless `--host` says otherwise; on a public address, keeping and running need `--token`.

## 5. Layout

```
urban-studio/
├── index.html · styles.css · src/app.js    the page: the visualization layer's studio, started with this work's name
├── serve.py                                the API's host for project-based works, configured
├── engine.py · recipe.json                 the plug-ins' engines; the examples' data
├── tools/tables/                           the work's own tools
├── plugins/ecology/                        tools/ · templates/ · engine/ · schemes/ · plugin.json · README.md
└── tests/run-tests.mjs · cdp.mjs           the checks; a driver of headless Chrome
```

## 6. Checks

`node applications/urban-studio/tests/run-tests.mjs` checks:

- **the page**: every element the shared studio asks for, and the files it loads;
- **the recipe, the plug-ins, their schemes and templates**: every step of every plan is a tool that is there;
- **the server, on a store of its own**:
  - every tool described;
  - a small forest kept and read, and every tool of the ecology plug-in run on it, held to counts by hand;
  - occupancy recovered from simulated visits, and the current of a corridor;
  - the record written for the Workbench;
  - a run refused to another page;
  - where the products are in the cache, Melbourne's template's first step;
  - every new tool of the provision plug-in on small tables answered by hand (a day's Aitchison distance and balance, unpaid work's share and value, two circles of three people, a sample with a known law of the day, committed minutes against a declared line), and, where its product is in the cache, the first steps of Time and care in Europe;
  - every tool of the materials plug-in on small tables answered by hand (a stock under a fixed lifetime, one process's flows reconciled, Heijungs and Suh's life cycle, a two-industry economy's footprint, a building weighed by the envelope of RASMI's structures), Net1 from the data library, and, where the products are in the cache, the first steps of its templates;
  - every tool of the appraisal plug-in on tables worked by hand (alternatives screened and ranked, a stream's present value, a panel's rounds, a field's configurations, two coders' κ against statsmodels).

The page was driven in headless Chrome: the Melbourne template's plan was run from the page in 127 s and its seventeen views drawn. A record was opened in the Workbench (not in the suite). Pictures are in the gallery's §33.

## 7. Dependencies

- **The API:** `science.studio_host`, `science.workspace`, `science.engines`, `science.board`, `science.ecology`, `science.society`, `science.materials`, `science.water`, `science.appraisal`; `science/ecology.hpp`, `science/society.hpp`, `science/materials.hpp`, `science/water.hpp` and `science/appraisal.hpp` for the plug-ins' engines.
- **The visualization layer:** `workspace` (its model, views and studio page), `board`, `map/plane.js`, `toolbox`, `styles`.
- **The data library:** `urban_forest.py`, `time_and_care.py`, `materials_and_flows.py` and the datasets of the recipe.
- **The domains:** through the API only; ecology, sociology, social provision, demography, economics, industrial ecology and civil engineering, and through them calx, morphe, physis, tyche, peira and nomos.

## 8. Limits and Open Questions

- **Its map is a plane about the project's frame.** The Workbench is its view in three dimensions. A 3D view inside the Studio would be the visualization layer's 3D GIS as a view of the workspace.
- **Market Studio keeps its own server and page.** It can move onto the shared host and studio, with its checks as the guard.
- **The handbook's methods not yet tools:**
  - material and energy accounts (MFA, LCA, EEIO);
  - specialist simulators of water and energy;
  - sensitivity analysis of a whole plan;
  - assessment and foresight.

  Qualitative evidence (interviews, field notes, protocols) has its place on a project's boards today.
- **Provisional:** the name, the colour and the port.
