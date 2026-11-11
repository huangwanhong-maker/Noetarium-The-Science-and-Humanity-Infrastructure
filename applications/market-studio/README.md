# Market Studio

**Status:** draft. **Activity:** use. **Port:** 9030. **Decision:** [ADR 0093](../../development/decisions/0093-market-studio.md), which supersedes the page of [ADR 0092](../../development/decisions/0092-the-markets-domain-and-agora.md) (*Agora*).

A studio for the analysis of markets in space and time, for any place and any data. It is project software in the manner of a GIS or an engineering environment: a project holds one's data as they are, slices of space and time, tools with declared parameters, what the tools answered, views of it, and boards on which it is written about. Nothing in the page knows a market. The tools do, and they are scripts: fourteen are predefined, and a project may hold its own.

## 1. Concepts

### 1.1 The Project

A project is a folder under `data/cache/market-studio/projects/<name>/`, never committed:

| Part | Holds |
|---|---|
| `project.json` | the project (`noetarium.workspace/1`): sources, slices, quantities, steps, views, boards, and a plan |
| `data/` | files the person added, as they are |
| `scripts/` | tools the person wrote |
| `results/` | what each step answered, by the step's id |
| `boards/` | its boards (`noetarium.board/1`) |

### 1.2 Data Left as They Are

A source is a file and a statement of what its columns are. No format is asked of the file beyond being a table: delimited text, a table or a list of rows in JSON, or GeoJSON (points, areas, lines); a city model of the data library is read as its streets.

- **Roles.** A column is bound to a role: latitude, longitude, cell (an area whose outline the table carries), time, until, identity, name, amount, kind. The page guesses them from the names and the person confirms.
- **A filter.** A source may keep only the rows where conditions hold (`industry = 4511`). It is applied as the file is read, so a large file of which a part is wanted is never held whole.
- **Names.** A table has a name, and tools are given tables by name. One name belongs to one table.

What a tool needs of a table it says in its own parameters. The roles are what the page needs, to draw a table on the map and to move it with the clock.

### 1.3 Slices of Space and Time

A slice is a region (a box in degrees), an interval of time and a period. A tool run on a slice is given the rows of its tables within it: by their latitude and longitude, or by the middle of their area; and by their period (a row of the year 2018 stands for that calendar year). A project has as many slices as its questions need, and the same tool run on two slices answers two tables.

### 1.4 Quantities

A datum or an assumption by name, with the range it is believed to lie in. Tools read them (the people a dwelling holds, what a buyer brings), and a size computed from them carries exact bounds.

### 1.5 Tools, Steps and the Plan

A tool is a module of the board's contract (`api/python/science/board.py`): it declares its parameters and answers tables, notes and figures. The panel at the right is built from the declaration: a parameter that names a table offers the project's tables, one that names a column offers that table's columns, one that names a quantity offers the project's quantities.

A tool that was run is a step: which tool, on which slice, with which parameters, when, and what it answered. A step whose table, slice or quantity changed afterwards is marked stale, with every step that stands on its answer, and can be run again. A template may bring a plan, steps to be run in order.

### 1.6 Views and Boards

- **Layers of the map:** points (alike, or sized and coloured by a column), areas coloured by a column, lines. A layer whose table has a time moves with the clock.
- **Charts:** bars, stacked bars, lines, points, a histogram, shares. A chart may gather the rows first: by a column and a second, taking the count, the sum, the mean, the least or the greatest of another.
- **Figures.** A script may answer a figure of its own (SVG, PNG or a matplotlib figure) when the predefined charts do not serve.
- **Boards.** A project has one board or several (the visualization layer's board, as in Tabula). A table, a view or a whole result is sent to the board in view, where it becomes a table with the card that draws it, and its notes become notes.

## 2. The Predefined Tools

| Group | Tool | Answers |
|---|---|---|
| The market | Buyers from dwellings and floor space | who could buy, from a census that counts dwellings and floor space, by the project's quantities |
| | The market | each outlet's sales and each seller's, concentration, and what of each buyer's spend buys nothing |
| | Whose custom | one outlet's trade area and its core, and where its custom would go if it left |
| | Where to open | sites chosen among candidates, with the bound of submodularity, and what an outlet at each would draw and from whom |
| | How large | a size as sums and products of quantities: exact bounds, elasticities, swings |
| Through time | Openings and closings | a record of who was there, period by period, read as lives |
| | How long they last | Kaplan and Meier's survival by cohort, with the log-rank test |
| | Concentration over time | the index of concentration and the four largest, period by period |
| | Change between two times | what each area, kind or seller held at the beginning and at the end |
| Tables | Count or add up, by a column | counts, sums, means by one or two columns |
| | Where they are thick | places gathered into the squares of a grid |
| | Count places in areas | the places of one table in each area of another, and for each hectare |
| | A new column | a table with one more column, from a formula of its columns and the project's quantities |
| | What a table holds | each column in brief |

The market's tools compute nothing of a market themselves. They read the person's tables, state the market in plain data and ask the engine: the markets domain's C++, with urban science's `access` for minutes walked on streets and peira's survival, composed in `api/cpp/science/markets.hpp` and reached from Python through `science.markets`. The kernel is presumed (Huff's model), and every answer says so.

## 3. A Tool of One's Own

`Tools +` writes a script into the project and opens it. Its contract is the board's, so a tool written here runs on a board of Tabula and the other way round:

```python
def create(api):
    class Mine(api.BoardAnalysis):
        id, title, description = "mine", "My tool", "What it answers."
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("by", "By", "", kind="text", role="column", of="table")]

        def run(self, board, params, context):
            t = board.table(params["table"])        # its rows are those of the slice; t.roles, t.places
            ...
            return [api.table("mine", {...}, roles={...}, places=..., show={"kind": "chart", "chart": "bar", "x": ..., "y": ...}),
                    api.note("..."), api.figure(a_matplotlib_figure)]
    return Mine()
```

A script may use numpy, the data library, and the markets domain through `science.markets`. It runs as a process of its own with the authority of whoever started the server: only this work's own page, at this computer's own address, may ask for a run or keep a script.

## 4. The Example

The template *Melbourne's cafés, 2002–2024* is a project on open data: the City of Melbourne's Census of Land Use and Employment (CC BY 4.0), every café and restaurant in every year, the blocks' dwellings and floor space, and OpenStreetMap's streets (ODbL). It brings five slices (the city over the years and in 2024; the Hoddle Grid, Docklands and Carlton in 2024), seven quantities and a plan of nine steps.

## 5. Running It

```
python applications/market-studio/serve.py --open
```

The recipe (`recipe.json`, run from the page or by `python data/acquisition/recipe.py applications/market-studio/recipe.json`) builds the engine with g++ 11 or later, natively or in WSL, into `data/cache/engines/market-studio/`, and fetches the example's data.

## 6. Layout

| Path | Holds |
|---|---|
| `index.html`, `styles.css`, `src/app.js` | the page: explorer, map with a clock, table, chart, board, inspector, console |
| `serve.py` | the server: projects, tables within slices, tools described and run, scripts, boards |
| `sources.py` | files read as tables, filters, the rows within a slice |
| `tools/<group>/*.py` | the predefined tools, each group with its `group.json` |
| `templates/*.json` | projects to begin from |
| `engine/main.cpp`, `engine.py` | the engine and its build |
| `tests/run-tests.mjs` | its checks |

In the other layers: `visualization/js/workspace` (the project's model and the views, pure, of no subject), `api/cpp/science/markets.hpp` and `api/python/science/markets.py` (the markets domain for any work and any script), and the board's contract extended with a table's roles, places and suggested view.

## 7. Dependencies

The markets domain and, for minutes walked, urban science's `access`; peira (`survival`), nomos, physis, morphe, calx beneath them; the API (`science.board`, `science.markets`, the JSON of `api/cpp`); the visualization layer (`workspace`, `board`, `map/plane.js`, `toolbox/charts.js`, the styles); the data library for the example.

## 8. Limits and Open Questions

- **The kernel is presumed.** No tool yet fits it to observed or stated choices: the domain's `demand` is planned.
- **The map is a plane about the project's frame.** It serves a city or a region; a project spanning continents stretches east and west. The 3D GIS of cities and the globe are not yet views of the Studio.
- **Regions are boxes.** A slice by a drawn outline, or by the areas of a table, is not built.
- **Tools are written in Python.** The space-time contract's C++ and JavaScript scripts (ADR 0079) are not hosted here.
- **A step's tables are kept whole as JSON.** A result of more than two million cells is refused.
- **One person, one computer.**
