# Tabula: A Board for Thought

*Status: draft (2026-11-11). [ADR 0090](../../development/decisions/0090-a-board-for-thought.md); its name and its colours are the user's. The design is [the idea of 2026-11-11](../../development/ideas/2026-11-11-markets-and-a-board-for-ideas.md).*

It serves **research** and **use**.

## 1. Purpose

Tools compute; people judge. A market analysis, like any inquiry, is half judgement: what is assumed, what was heard, what is doubted, what was decided. Tabula is the ground on which that half is set out and tied to the other: notes of several kinds, joined by links that say how they stand to each other, beside the data, the maps and the models that bear on them.

The board is of no subject. A subject comes to it as a **plug-in**: its kinds of note, its templates, its cards and its modules. **Market analysis is the first plug-in.** Four more stand beside it: research, the city in three dimensions, agents and history.

## 2. What One Can Do

**Think.**

- **Write.** Double-click the ground for a note. Notes are of kinds, drawn apart and named: an idea, a fact (with its source), an assumption, a question, an insight, a decision, a risk; a plug-in adds its own (a competitor, a segment, what someone said; a hypothesis, a method).
- **Draw.** The pen (`P`) draws with a mouse or a stylus, a stylus by its pressure; fingers still move the board while it is in the hand. Seven inks, three widths and a marker; the rubber (`E`) takes strokes away. A stroke is an item like any other: it is selected, moved, undone, and belongs to the region it is drawn in.
- **Gather.** Regions name parts of the board; what lies in a region belongs to it and moves with it. Text, shapes and images (dropped, pasted or chosen) lie beside the notes.
- **Join.** Drag the dot at an item's side to another item. A link says *supports*, *contradicts*, *leads to*, *part of*, *then*, or nothing. Tab on a note adds another joined to it, as a mind map grows.
- **Plan and chronicle.** A plan is written as lines (`Interviews : after Desk research, 3w, 20%`) and drawn as a Gantt chart with the chain that cannot slip; a chronicle is events on a line of time, from days to millennia.
- **Arrange.** A selection is tidied into a grid, set out as a tree by its links, set in the order of time, aligned or spread.
- **Layers.** Named layers above the ground, back to front. New items are laid on the layer in the hand; a layer is hidden, held or made faint with all that lies on it, moved forward or back, and removed, leaving what lay on it on the ground.
- **Find.** The panel lists the board's contents, layer by layer, as its regions hold them. A row selects its item and brings it into view; an item can be hidden, or held where it is.

**Reckon.**

- **Assumptions that compute.** A line of a note such as `share = 3% [2%, 5%]` makes a quantity with its range. A figure's formula uses the names (`TAM = buyers * need * spend`), carries the ranges through by interval arithmetic, and names the assumptions it rests on.
- **Tables.** Rows and columns pasted from a sheet, or a CSV file dropped or chosen, become a table with a name. Whatever made a table, a chart, a map or a module draws it by that name.
- **Charts and maps.** A chart draws a table as bars, lines or points. A map draws a table's places: points by `lat` and `lon`, economies by `iso3`, over the countries or, for a town, over a layer of the data library. Areas in dispute are hatched, in no colour of the data, and unnamed.
- **Modules.** A module is a script in Python that is given what stands on the board (its tables, its quantities with their ranges, its notes) and answers tables, notes and figures. It gets data where the data library keeps them, and may use whatever its Python has: numpy always, scikit-learn or torch where they are installed. A press on a table it answered lays that table beside it, as a map where its rows have places, else as a chart.

**Keep.** A board is kept a moment after every change, and opened again by its address. It is given away as a file (`noetarium.board/1`, JSON) or as a picture (PNG). Day and night.

## 3. A Plug-in Says What It Needs

A module declares its parameters, each with its kind and what it names; the panel builds the inputs from the declaration, as an inspector is built from a component's fields. Nothing about a module is written into the page.

```python
def create(api):
    class Segments(api.BoardAnalysis):
        id = "segments"; title = "Segments, by k-means"; description = "…"
        params = [api.param("table", "Table", "economies", kind="text", role="table"),                   # the panel offers the board's tables
                  api.param("columns", "Alike in", "gdp_per_capita", kind="text", role="columns", of="table"),
                  api.param("k", "Segments", 4, kind="integer", minimum=2, maximum=12),
                  api.param("log", "Compare in logarithms", True, kind="boolean"),
                  api.param("file", "A file", "", kind="text", role="file")]                             # a path, data:<dataset>/<file>, or a file chosen
        def run(self, board, params, context):
            t = board.table(params["table"])
            share = board.quantity("adoption")          # an assumption written on a note: .lo, .mid, .hi
            ...
            return [api.table("segments", {...}), api.note("…"), api.figure(matplotlib_figure)]
    return Segments()
```

| A parameter | In the panel |
|---|---|
| `kind="number"`, `"integer"` | a number within its bounds |
| `kind="boolean"` | a tick |
| `kind="choice"` | a list of its options |
| `kind="text"` | words |
| `role="table"` | the board's tables, by name |
| `role="column"`, `"columns"` (with `of`) | the columns of the table another parameter names |
| `role="quantity"` | the quantities the notes define |
| `role="file"` | a path on this computer, `data:<dataset>/<file>` of the data library, or a file chosen and kept |

The contract is the infrastructure's, not this page's: [`api/python/science/board.py`](../../api/python/science/board.py), on the host that runs the plug-ins of the Economy Atlas and the Urban Science Workbench. A few lines written on a card itself need only `def run(board, api)`.

A card is declared in the same way in JavaScript (`form(ctx)` answers its fields): the board's own chart and map are cards, and a plug-in may bring more.

## 4. The Market Analysis Plug-in

[`plugins/market-analysis/`](plugins/market-analysis/). Three kinds of note (competitor, segment, said), twelve templates and four modules.

| Module | What it does | Data |
|---|---|---|
| Economies in a year | Every economy with its people, its income per person and its share of people in towns | the data library's table of economies (World Bank WDI, CC BY 4.0) |
| What the market could be worth, by economy | Buyers and their spending in each economy, with the range, from two assumptions read from the board's notes | the table above, and the board |
| Segments, by k-means | Gathers the rows of any table into k kinds that are alike in the columns chosen | any table |
| Places of business in Melbourne | The establishments of one kind in a census year, each with its place | City of Melbourne, CLUE (CC BY 4.0) |

| Template | The question |
|---|---|
| Market size | How large the market is, from assumptions with their ranges |
| Where the buyers are | Which economies, by what the market could be worth in each: three modules, two maps and a chart |
| Where the offers are | The places of business of one kind in Melbourne, on the city's blocks |
| The offers, in the round | The same standing on the city in three dimensions (the card of `gis-3d`) |
| What to test first · SWOT · Five forces · PESTEL · Positioning map · A segment · A buyer's journey · Going to market | The questions of §3 of the design |

*Caveat.* The numbers in the templates are placeholders to be written over. None is a fact about any market. Income per person is GDP per person, which is income and not spending; the module says so.

*Remark.* These modules are scripts of a plug-in, in Python. When the markets domain is written (trade areas, choice, diffusion; the design's §2 to §4), its computations will be C++ in the domain and reach the board through modules such as these.

*Remark.* The Business Model Canvas and the Value Proposition Canvas are left out until their licences are checked against this repository's.

A module of no subject stands in [`plugins/general/modules/`](plugins/general/modules/): *A figure from a file* reads a CSV named by a parameter, gives it to the board as a table and draws it with matplotlib.

### 4.1 The Other Plug-ins

| Plug-in | Brings | Stands on |
|---|---|---|
| [`research`](plugins/research/) | Two kinds of note (hypothesis, method); hypotheses and evidence, a reading map, a study planned | the board alone |
| [`gis-3d`](plugins/gis-3d/) | The card *City in 3D*: a table's places standing on a city of the data library's city model (Melbourne's centre, Lower Manhattan), each a column on the roof it lies under, as tall and coloured as columns of the table say | the visualization layer's 3D GIS and three.js, both fetched when a card is first drawn |
| [`agents`](plugins/agents/) | The card *Run*, which plays any table whose rows have a place and a step at which each comes to be; the module *How fast it would spread*, Bass's diffusion as agents on a lattice, from two assumptions on notes | numpy |
| [`history`](plugins/history/) | The card *Events*, a table of events on a line of time, each kind in a colour of its own; the module *The events of a history*, which reads one of the history domain's six canons | the history domain's records |

*Caveat.* The agents' model is written with numpy at the plug-in's level and held to its mean field: where everyone looks to everyone, the mean of its runs follows Bass's recursion $F_{t+1} = F_t + (1 - F_t)(p + q F_t)$. The general home of agent-based models is nomos's `agents`.

*Caveat.* The canons are seeds drafted by a language model and not yet checked against their sources; the module says so with every answer. A canon writes each date in its source's calendar, and the card's line of time is the proleptic Gregorian calendar's: they differ by ten to thirteen days, which a line of years does not show.

## 5. Structure

| Part | Role |
|---|---|
| [`visualization/js/board/`](../../visualization/js/board/) | The board itself, shared by every work: its model, operations, arrangements, schedules, quantities, tables, templates, cards, its drawing and the hand's work, its rail and panel, its picture |
| [`api/python/science/board.py`](../../api/python/science/board.py) | The contract of a board's modules, and their host |
| `src/app.js`, `src/store.js` | This page: the plug-ins and their modules loaded, one board made with all of them, boards kept and opened; what the server serves the board's cards |
| `plugins/<name>/plugin.js` | A subject: kinds of note, templates, cards |
| `plugins/<name>/modules/*.py` | Its modules |
| `plugins/<name>/plugin.css` | The style of its cards, in the board's variables |
| `serve.py` | The server: the page, the shared code, the store of boards, images and files under `data/cache/tabula/` (never committed), the modules described and run |

## 6. Running It

```
python applications/tabula/serve.py --open
```

It opens at http://127.0.0.1:9010/. It needs Python with numpy and a browser. `--python` names another Python for the modules (one with scikit-learn or torch). The market's modules need their datasets in the data library's cache; a module that lacks one says which to fetch. `?template=market-analysis/where` begins a board with a template; `?theme=night` by night; `?dry` keeps nothing.

**Who may run a module.** A module runs with the authority of whoever started the server. Only the server's own page, at this computer's own address, may ask for a run; another page open in the same browser is refused, as it is refused keeping anything. A board opened from a file runs nothing by itself: its modules run when their keys are pressed.

## 7. Dependencies

The visualization layer (`board`; within it the toolbox's charts, the 2D GIS's projections and the styles' colour maps; `gis` for the city in three dimensions), the API's `science.board`, the data library's cache, and the history domain's canons, read as files. No domain's code yet.

## 8. Checks

- `node applications/tabula/tests/run-tests.mjs`: 43 checks. The plug-ins (every template a sound board, schedules read, quantities valued, every module a template lays one the server has); the colours of its charts apart in every vision; the server (boards kept, listed, opened, put aside without being destroyed; images and files of data kept by content; a run refused to what is not its own page; a script run on a board's tables and quantities; a failing script told with its reason); the market's modules on the data library's own data (the worth of an economy against its closed form, its range that of the assumptions, k-means the same on the same seed); the agents against their mean field; a canon's occasions in the order of time, years before 1 among them.
- The board's 73 checks are the visualization layer's (`visualization/tests/board-checks.mjs`), and the contract's 37 the API's (`api/tests/check_board.py`).
- The page was driven in headless Chrome over the DevTools protocol, 70 assertions in three drives: the hand (writing, Tab, moving and undoing, regions, the panel, joining, resizing, the marquee, a template, keeping and reopening, the picture), and the data (three modules run from their keys, the economies coloured on the map with areas in dispute hatched, the chart, an assumption doubled and the module's answer doubling, the panel built from a module's declaration, 1,677 places of Melbourne on its blocks, the pen and the rubber, a sheet read as a table, the contents), and the plug-ins and layers (a run played by its clock beside Bass's curve, forty events of a canon on a line of time, 969 places raised on Melbourne's centre in three dimensions and the scene given back when the card is removed, items laid on a layer, the layer hidden, made faint, removed and brought back). Those drives are not yet part of the suite.

## 9. What Is Not Yet Here

Stated so that it is not looked for:

- **Modules in C++ and JavaScript.** The host runs C++ plug-ins for the other works; the board's contract is written for Python only.
- **The markets domain** (trade areas, choice, diffusion, quantities as distributions).
- **A stylus was not tried.** The pen was driven with a mouse; pressure, the stylus's own rubber and fingers moving the board under the pen are written and unchecked. Copy and paste, and files dropped, were not driven either.
- **Several people on one board;** a native SVG picture; dragging a plan's bars; units that are checked (they are labels); a map's own picture when the board is drawn as a PNG was not looked at.
- **The drives of the page in the suite.**
