# Urban Studio

*A studio for the analysis of cities and the life in them, project by project: one's data as they are, slices of space and time, tools brought by plug-ins or written as one's own scripts, what they answered drawn as maps and charts, opened in the Urban Science Workbench in three dimensions, and written about on boards.* Status: **draft** (2026-11-13; [ADR 0102](../../development/decisions/0102-the-urban-studio.md)). Activities: research, use, learning. Port 9090.

**Try it online,** with nothing to install: **http://173.212.213.207:9204/?workbench=http://173.212.213.207:9203/** (our online server, with the Urban Science Workbench beside it at http://173.212.213.207:9203/). It is open to read there: five example projects (Melbourne's urban forest, time and care in Europe, the material stock of Melbourne's centre, what the European Union's final demand emits, EPANET's Net3) with their results, maps, charts and boards; `?project=<name>` opens one. Tools are run, and projects kept, on one's own computer (§4).

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

The Studio knows no subject. A plug-in brings one: its tools, its templates, its engine and its declared schemes. The first plug-in is *Ecology and life* (§3), the second *Time, care and provision*. A plug-in may also bring a server side and views of its own, as the fifth, *Evidence in space and time*, and the sixth, *Population dynamics*, do. Later ones (energy and water, the material stock of buildings, health, culture) join the same way, and nothing else of the work changes. A tool is a module of the board's contract (`api/python/science/board.py`), so the same tool runs on a board of Tabula and in Market Studio's projects.

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
| [Materials and flows](plugins/materials/README.md) | Seven tools on the industrial-ecology domain and civil engineering's water ([ADR 0106](../../development/decisions/0106-industrial-ecology-founded.md), [ADR 0107](../../development/decisions/0107-civil-engineering-founded-water-networks.md)): what the buildings hold; what would renew the stock; a stock over time; flows that balance; a life cycle; what final demand causes; a water network; a community's greenhouse gas inventory; measures against it ([ADR 0147](../../development/decisions/0147-community-emissions-and-their-mitigation.md)); a city's solid waste and SDG 11.6.1 ([ADR 0148](../../development/decisions/0148-municipal-solid-waste-and-sdg-11-6-1.md)). Five templates (the material stock of Melbourne's centre; what the EU's final demand emits; EPANET's Net3; an invented town's emissions; an invented city's waste) and its engine | draft |
| [Appraisal and futures](plugins/appraisal/README.md) | Four tools on nomos's appraisal ([ADR 0108](../../development/decisions/0108-appraisal-futures-and-the-agreement-of-coders.md)): alternatives compared, limits first; costs and benefits; Delphi rounds and morphological fields; how far coders agree. Its engine | draft |
| [Population dynamics](plugins/population/README.md) | A place's people over half a century, cell by cell, as a field that moves and grows, on kinesis's balance laws through the API's `science/population.hpp` ([ADR 0124](../../development/decisions/0124-population-dynamics.md)). Its view, *Population*: the field as the page with a ribbon of years; the laws learned as a fluid's (growth, spread, drift, crowding) and judged on the years kept aside, with their currents; the record's modes; the years ahead under a law, a new source placed by a click. Five tools: a place's people, 1975 to 2030; how the whole place grew; laws of its people, learned and judged; the record's modes; the years ahead under a law. Two templates (Melbourne; Lagos), its engine and its server side | draft |
| [Needs](plugins/needs/README.md) | Needs explored and validated by evolution, on the needs domain through the API's `science/needs.hpp` ([ADR 0125](../../development/decisions/0125-the-evolutionary-exploration-and-validation-of-needs.md)). Its view, *Needs*: the field of statements as the page, a need opened where it stands with its tests in words and the argument of its standing, the loop of the search with a strategy chosen at every stage, the areas by the part of the data each lies in or by where a need lies, the ledger of confirmations against the online rule's levels, the generations, statements of one's own and a language model's proposals; since [ADR 0126](../../development/decisions/0126-needs-read-under-their-terms-and-a-model-in-every-part.md) each statement read under the readings of its open terms with its rivals, a language model in every part of the search, and the lineage as PROV. Three tools: test statements of need; search for needs by evolution; a need, area by area. One template (the City of Melbourne at the 2021 Census), its engine and its server side | draft |
| [Cities across scales](plugins/scales/README.md) | A city's indexes at every scale at once, and models of them, on kinesis's systems across scales, tyche's change of support and estimates, and nomos's measures through the API's `science/scales.hpp` ([ADR 0128](../../development/decisions/0128-indexes-and-cities-across-scales.md)). Its view, *Across scales*: the blocks toned by an index read at the block, its area or the city, with a ribbon of years; models drawn as their levels, operators and laws, what the records do not hold ringed to be declared; the urban-science domain's catalogue of 1,536 indexes opened in place. Five tools: indexes across scales; what the census withheld; an index where it was not observed; where growth goes, across scales; the vacancy cycle and what must be declared. One template (Melbourne's CLUE blocks, 2002–2024), its engine and its server side | draft |
| [Lifelines and evacuation](plugins/risk/README.md) | A place's risks through its roads, its people and its money, on risk management, urban risk management and disaster risk through the API's `science/risk.hpp` ([ADR 0129](../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Four tools: which roads matter most to reaching care under the place's floods (risk and criticality of every link, placed); who stays within reach of a station in each flood and where to add one; walking out of a hazard's reach (the bounds by flows over time and the simulated crowd, the late placed, tall buildings chosen as refuges); how the city might meet its flood losses (layers of a budget, a credit and insurance, the cheapest tower). Two templates (Freetown's roads, floods and losses; Miyako walking out of the assumed tsunami) and its engine | draft |
| [Public transport and who can reach it](plugins/transit/README.md) | Who lives within reach of public transport, SDG 11.2.1 under every declared reading of its open terms, and every stop's service at the peak, on urban science's `transit` through the API's `science/transit.hpp` ([ADR 0151](../../development/decisions/0151-public-transport-and-sdg-11-2-1.md)); a template on Melbourne's centre with the state's GTFS feed |
| [From evidence to action](plugins/engagement/README.md) | An engagement with a city kept as kybernesis's record, through the API's `science/steering.hpp` ([ADR 0131](../../development/decisions/0131-an-engagement-with-a-city.md)): the evidence the project's tools answer gathered into the loop, readings with their sources, what the evidence allows for each quantity of each place, declared operators judged under every reading against standards with their holders, the needs taken up meeting the practices that answer them, the advice and what is open; every block filled by a method or by a person (fill, amend, review, files), a role not empowered entering a proposal; every act sealed, the trajectory replayed, branched and revisited; a practice carried by a lens, its support factors checked. A tool (*Evidence from a table*), an example and a template (Freetown learning from Miyako), its engine, a server side and views | draft |
| [Evidence in space and time](plugins/evidence/README.md) | An archive of evidence about places and times, on archeion, tekmerion, tyche and calx through the API's `science/spacetime_evidence.hpp` ([ADR 0123](../../development/decisions/0123-evidence-in-space-and-time.md)). Two views: *Evidence*, the place as the page, claims toned by the weight of their evidence and items at their places, each opening where it stands, and *What to do*; *Holdings*, each file with its digest and chain of custody. Three tools: claims weighed and decided; a kind of report's channel, counted; how the conclusions fared. One template (rats in the East Village and the Lower East Side, 2024, on New York City's records), its engine and its server side | draft |
| [Programmes and public finance](plugins/programmes/README.md) | What a city can do and afford, and what a programme of learning did for it, on kybernesis's scorecards, the policy domain's `learning` and the economics domain's `fiscal` and `capture` through the API's `science/scorecards.hpp`, `science/learning.hpp`, `science/fiscal.hpp` and `science/capture.hpp` ([ADR 0144](../../development/decisions/0144-knowledge-programmes-and-scorecards.md), [ADR 0145](../../development/decisions/0145-the-finances-of-local-governments.md), [ADR 0146](../../development/decisions/0146-land-value-capture-and-readjustment.md)). Five tools: a city's scorecard scored as its scale allows, its weaknesses as needs; a learning programme read from needs to results; a city's finances and how much more it can borrow; a European country's local government debt and its fan ahead (Eurostat); what a project could take back from the land it raises, instrument by instrument, and a land readjustment with its replotting. One template (an invented cohort of three cities) and its engine | draft |

A plug-in is a directory `plugins/<name>/`:

- `plugin.json`: its title, what it is about, its version and colour, the domains it stands on, its theory note;
- `tools/*.py`: a group of tools under its name;
- `templates/*.json`: projects to begin from, with a plan;
- `engine/main.cpp`: a C++ engine on one of the API's compositions, built by `engine.py` through `science.engines` into `data/cache/engines/urban-studio/<name>/`;
- `schemes/*.json`: declared files its tools offer as choices;
- `routes.py`: a server side of its own, routes under `/api/projects/<project>/<name>/`, changes taken only from the work's own page (ADR 0123);
- `views/*.js` and `views/*.css`: views of its own beside the map, the table, the chart and the board, loaded by the studio page (ADR 0123).

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
- seven Eurostat datasets (time use, healthy life years, limitation in usual activities, life tables, population, its projection, labour market transitions; CC BY 4.0) and the product made from them by `data/products/time_and_care.py`;
- New York City's rodent inspections and 311's rodent complaints of 2024 (NYC Open Data), with the place's ways and footprints from Overture Maps (ODbL), written as an archive of evidence by `data/products/urban_evidence.py`;
- GHS-POP R2023A's 1 km tiles of every fifth year from 1975 to 2030 for Melbourne and Lagos (the European Commission's JRC, CC BY 4.0), carried onto a 1 km UTM grid by `data/products/population_history.py`;
- the ABS 2021 Census DataPack's tables of need for assistance, personal income and unpaid care by SA1, age and sex (G18, G17A–C, G25; CC BY 4.0), with the census of land use's floor space, written for the City of Melbourne by `data/products/needs_tables.py`;
- the census of land use's blocks, floor space by use with what it withheld, establishments and dwellings, every census year 2002–2024 (City of Melbourne, CC BY 4.0), written as a city across scales by `data/products/city_scales.py`.

A project on one's own data needs only the engines. The page builds them too, from the project's panel. Projects are kept under `data/cache/urban-studio/projects/`, never committed. Only 127.0.0.1 is bound unless `--host` says otherwise; on a public address, keeping and running need `--token`.

## 5. Layout

```
urban-studio/
├── index.html · styles.css · src/app.js    the page: the visualization layer's studio, started with this work's name
├── serve.py                                the API's host for project-based works, configured
├── plugins/risk/                           tools/ · templates/ · engine/ · plugin.json · README.md
├── plugins/transit/                        tools/ · templates/ · engine/ · plugin.json · README.md
├── plugins/engagement/                     tools/ · templates/ · examples/ · engine/ · views/ · routes.py · plugin.json · README.md
├── engine.py · recipe.json                 the plug-ins' engines; the examples' data
├── tools/tables/                           the work's own tools
├── plugins/ecology/                        tools/ · templates/ · engine/ · schemes/ · plugin.json · README.md
├── plugins/evidence/                       tools/ · templates/ · engine/ · routes.py · views/ · plugin.json · README.md
├── plugins/population/                     tools/ · templates/ · engine/ · routes.py · views/ · plugin.json · README.md
├── plugins/needs/                          tools/ · templates/ · engine/ · routes.py · views/ · plugin.json · README.md
├── plugins/scales/                         tools/ · templates/ · engine/ · routes.py · views/ · plugin.json · README.md
├── plugins/programmes/                     tools/ · templates/ · examples/ · engine/ · plugin.json · README.md
└── tests/run-tests.mjs · cdp.mjs           the checks; a driver of headless Chrome
    drive-evidence.mjs · drive-population.mjs · drive-needs.mjs · drive-scales.mjs   the evidence, population, needs and scales plug-ins driven in headless Chrome (not in the suite)
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
  - every tool of the appraisal plug-in on tables worked by hand (alternatives screened and ranked, a stream's present value, a panel's rounds, a field's configurations, two coders' κ against statsmodels);
  - the evidence plug-in's server side: a file taken in with its SHA-256, handed over, served as taken in; an archive kept over a change made meanwhile refused, one naming an item twice refused, a change from another page refused; and, where its engine is built and its example is in the cache, the example laid out and appraised, the resident's channel counted lot by lot, the claims weighed as a placed table, and the predictions held against the quarter after;
  - the population plug-in's server side: a study kept from the work's own page and refused from another, a place that is not a name refused; and, where its engine is built and a place is in the cache, its field served, every law learned and judged on the two years kept aside with persistence at no skill and uniform growth placing nothing, no law left with a negative spread, each law's currents, the record's modes, a run that only moves people gaining exactly what its new source adds, and its laws and years ahead as tables placed on the map.
  - the needs plug-in's server side: a dataset that is not a name refused, a search not started by another site's page; and, where its engine is built and the Melbourne dataset is in the cache, statements read with one refused, a statement tested, a search run in steps and followed to its end with every admission confirmed once and sealed, continued from its kept state, a need area by area, and its three tools.

The page was driven in headless Chrome: the Melbourne template's plan was run from the page in 127 s and its seventeen views drawn. A record was opened in the Workbench (not in the suite). Pictures are in the gallery's §33. The evidence plug-in was driven by `tests/drive-evidence.mjs` (17 steps, not in the suite): the example laid out, a claim and an item opened, the decision declared, a file altered on disk and found, a claim placed by a click, the tools run, by day and by night. Its pictures are in the gallery's §43.

## 7. Dependencies

- **The API:** `science.studio_host`, `science.workspace`, `science.engines`, `science.board`, `science.ecology`, `science.society`, `science.materials`, `science.water`, `science.appraisal`, `science.spacetime_evidence`, `science.population`, `science.needs`, `science.risk`, `science.steering`, `science.project_archive`; `science/ecology.hpp`, `science/society.hpp`, `science/materials.hpp`, `science/water.hpp`, `science/appraisal.hpp`, `science/spacetime_evidence.hpp`, `science/population.hpp`, `science/needs.hpp`, `science/risk.hpp` and `science/steering.hpp` for the plug-ins' engines.
- **The visualization layer:** `workspace` (its model, views and studio page), `board`, `map/plane.js`, `toolbox`, `evidence/located.js`, `dynamics/fields.js` and `dynamics/charts.js`, `styles`.
- **The data library:** `urban_forest.py`, `time_and_care.py`, `materials_and_flows.py`, `urban_evidence.py`, `population_history.py`, `world_city.within` and the datasets of the recipe.
- **The domains and foundations:** through the API only; ecology, sociology, social provision, demography, economics, industrial ecology, civil engineering and geodesy, and through them calx, morphe, physis, tyche, peira and nomos; archeion and tekmerion for the evidence plug-in; kinesis for the population plug-in.

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
