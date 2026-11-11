# 0093. Market Studio: A Project-Based Platform for the Analysis of Markets

- **Status:** Accepted, provisionally (2026-11-11). Supersedes §2.3 of [ADR 0092](0092-the-markets-domain-and-agora.md), the work *Agora*; the markets domain of that record stands.
- **Date:** 2026-11-11
- **Decided by:**
  - **The question.** The user, of Agora: "We don't love it just like a beautiful demo, but any apply to various space, and analysis various spatial-temporal things in market, and visualize them, right? I think it requires allowing user highly flexible usage. [...] if data provide space-temporal data in specific format, we can visualize them, analyze them, etc. Also allow user write template script, do task-specific visualization, analysis".
  - **The decision.** The user: "we need largely revise, make it a general platform for market analysis. And also rename 'agora'. [...] I think we may better make it a 'market studio'? Project-based. [...] A project can specify one or more spatial, and temporal ranges to analysis, i.e. [project <1 - *> spatial-temporal range/or spatial-temporal slice]. For each slice, we can do some special analysis and visualization. And I think data format should also not fit, but depends on specific analysis script?"
  - **The manner.** The user: "Consider how Unity3D, Visual Studio, Matlab, QGIS etc's general project develop software design. I think if want 'general', project-based and extensible by script/plug-in, but in UI easy to use, is a more proper design." And: "the board need remain. I think each project one board is OK. Or more boards [...] The current visualization and analysis is good. Migrate to our new design. I think some feature now must become (pre-defined) plug-in/script/extension [...] provide comprehensively pre-defined features also a good choice. Like Excel, user have various chart visualization requirement, but it pre-defines many kinds of charts; if user need more complex visualization, it may need write code itself."
  - **The design.** Claude, in the conversation and here.

## 1. Context

ADR 0092 founded the markets domain and drew it by Agora: one city, one census, four questions, a kernel with fixed assumptions. The domain is general: it names no place, and a market is given to it as data. The work was not. Its engine read one product of the data library, its buyers were built from one census's columns, its questions were the page's own code. Another city, another kind of data, a question through time or an analysis of one's own each needed the work rewritten.

The user asked whether it could be applied to any space and to the many things in a market that vary in space and time, with the person's own data and scripts. It could not, and the user decided to revise it largely.

What general project software has in common was the guide. A GIS, a game engine's editor, an engineering environment and a numerical workbench share one shape: a project that refers to data and does not swallow them; an explorer of what the project holds; an inspector whose forms are built from declarations; a toolbox of predefined operations beside scripts of one's own under the same contract; a record of what was run; views that are layers and charts; a console.

## 2. Decision

### 2.1 The Work Is Renamed and Rebuilt

`applications/agora` becomes `applications/market-studio`, *Market Studio*, on the same port (9030) and in the same colours (coral rose with a plum minor, provisionally). The page of ADR 0092 is replaced. Everything it computed remains, as predefined tools.

### 2.2 The Project

A project is a folder (`data/cache/market-studio/projects/<name>/`) and a document of the schema `noetarium.workspace/1`:

- **Sources.** The person's files, left as they are: delimited text, a table or a list of rows in JSON, GeoJSON. A source names its file, binds its columns to roles and may carry a filter, applied as the file is read.
- **Roles.** Latitude, longitude, cell, time, until, identity, name, amount, kind. The roles are what the page needs, to draw a table and move it with a clock. What an analysis needs of a table it declares in its own parameters. This answers the user's point: the format is the script's to ask, and the platform's only to describe.
- **Slices.** A region, an interval of time and a period: the user's [project 1–* slice]. A tool run on a slice is given the rows of its tables within it. The rule is one, stated twice and checked to agree: in the page (`rowsIn`) and in the server (`rows_in`).
- **Quantities.** Data and assumptions by name, each with its range.
- **Steps.** A tool that was run: which, on which slice, with which parameters, when, and the tables, notes and figures it answered. A step is how its tables are made again. A step is stale when a table it was given, its slice or a quantity changed after it ran, and so is every step on its answers.
- **Views.** Layers of the map and charts, each drawing a table by its name.
- **Boards.** One or several, the visualization layer's board.
- **A plan.** Steps a template proposes, run in order.

Tables are named, and the names are the project's wiring: a tool is given tables by name. One name belongs to one table; a table answered under a name that is taken is named after its slice.

### 2.3 Tools Are Modules of the Board's Contract

A tool is a script of `api/python/science/board.py`, the contract of ADR 0090: declared parameters; tables, notes and figures answered. A tool written for the Studio runs on a board of Tabula, and the other way round. The contract is extended, additively:

- a table may carry `roles`, `places` (the outlines of its cells) and `show` (the view it suggests);
- a module is given `board.view`: the slice its tables were cut to.

The predefined tools are files of the work (`tools/<group>/*.py`), described to the page by their declarations. A project's own scripts stand beside them under `project/`. The page builds each form from the declaration: a parameter that names a table offers the project's tables, a column that table's columns, a quantity the project's quantities.

Fourteen are predefined, in three groups:

- **The market:** buyers from dwellings and floor space; the market; whose custom; where to open; how large. These are what Agora computed.
- **Through time:** openings and closings; how long they last; concentration over time; change between two times.
- **Tables:** count or add up by a column; places gathered into squares; places counted in areas; a new column from a formula; what a table holds.

### 2.4 The Markets Domain for Any Work

Agora's engine composed the domain for one city. The composition moves down to the API, after the toolbox's place (ADR 0080) and the design loop's (ADR 0089):

- `api/cpp/science/markets.hpp`: a market stated in the request as plain data (cells of buyers, offers and their sellers, the pairs within reach with their costs, Huff's parameters), and nine operations on the markets domain, urban science's `access` and peira's survival. It names no place, census or column.
- `api/python/science/markets.py`: the same for scripts. A script states its market and asks; it computes nothing of a market again.

The work's engine is a program of sixty lines around the header. It compiles in about two minutes.

### 2.5 The Page

The project at the left: slices, data, quantities, tools, results, views, boards. In the middle what is looked at: a map with a clock, a table, a chart, a board. At the right what is selected, with its form. Beneath, what happened.

- **The map** is the visualization layer's plane about the project's frame. A layer whose table has a time moves with the clock; the clock's steps are the slice's periods.
- **Charts** are predefined, as a spreadsheet's are: bars, stacked bars, lines, points, a histogram, shares, each gathering by count, sum, mean, least or greatest. What they do not cover is a tool's to compute, or a figure a script draws itself.
- **The page knows no market.** Its checks assert that it names none of the domain's terms.

### 2.6 In the Visualization Layer

`visualization/js/workspace`: the project's model and the views, pure and of no subject, for any work that wants to be project-based. It is the second appearance of the pattern (the Studio of ADR 0081 holds projects of frames and systems), and it is kept apart from that one: a canvas of live frames and a workspace of slices and steps are different objects.

### 2.7 The Example

The template *Melbourne's cafés, 2002–2024*: the City of Melbourne's census, every café and restaurant in each of 23 years (32,943 rows of its 68 MB file, kept by a filter), the blocks' dwellings and floor space year by year, vacant spaces, the streets. Five slices, seven quantities, a plan of nine steps.

What its plan answers, on the template's assumptions (the market's figures rest on a presumed kernel and on assumed spends; the record's are counts):

- **Two districts in 2024.** The Hoddle Grid: 1,036 outlets of 975 sellers, 98.8% of what its buyers bring finds an outlet, the four largest sellers hold 2.3%. Docklands: 148 outlets of 145 sellers, 90.1%, the four largest 7.3%.
- **Through time.** The census holds 908 cafés and restaurants in 2002, 1,802 in 2019 and 1,677 in 2024. Read as lives by trading name and property, they are 5,902, of which 4,226 were last seen before the record ends.
- **How long they last.** In every cohort of beginnings from 2003 to 2022, half are gone after five years. The cohorts do not last alike (log-rank p of 4 × 10⁻¹⁷).
- **A property of the record.** In the early cohorts the closings fall at even durations: 96% remain after one year and 69% after two. This suggests that part of the record is revised every second year. It is the record's, and is said beside the finding.
- **Concentration** by outlets stays low: the four largest sellers hold 1.5% to 2.8% of the outlets over the 23 years.
- **How large.** What all who could buy bring is A$629 M a year at the middle of every quantity, between 327 M and 1.26 B at their ends.

### 2.8 Checks

- **The workspace:** 41 checks in the visualization layer's suite (398 in all): instants and periods against the calendar, the rows within a slice and at an instant, roles, soundness, staleness, the plane, and the views against brute force.
- **Market Studio:** 49 checks. Its server runs on a store of its own: a file kept and read, the rows within a slice the same by the server's rule and by the page's, tools described, runs refused to other pages, seven tools run with answers checked by hand, names held apart, a formula that is not arithmetic refused, a script kept and run with its figure read back, a board kept. Where the engine is built and the data are in the cache, the template's plan runs on Melbourne, the other seven tools among it: the market conserves what buyers bring, the size is the product of its quantities, survival never rises, the sites' gains do not rise.
- **The contract's other users** pass unchanged: Tabula 108, the API 296.
- **The page** was driven in headless Chrome (22 assertions, not in the suite): the template begun, the clock moved, a slice chosen, a source's roles, a tool's form, the plan run, the map, charts, the board, a script written and run.
- **Found while checking.** The board's map card wrote the word "null" in its legend where a scale was not logarithmic; it is mended.

## 3. Alternatives Considered

- **Generalising Agora in place:** more cities as more products, more questions as more views. Each would still be the work's own code, and the person's own data would still not enter.
- **Making it a plug-in of Tabula.** The board is where findings are written about; a project with data, slices and a record of runs is a different centre. The user's word was that the board is a tool and this another application. The two share the contract of tools, which is what makes a tool reusable between them.
- **Making it a mode of the Urban Science Workbench's Studio (ADR 0081).** That Studio renders projects of frames and live systems for figures; its scripts are systems in a schedule, tied to the workbench's 3D world. A market project is slices, steps and tables. The pure parts that could be shared were looked at and left where they are.
- **A fixed schema for market data** (outlets, buyers, transactions in named columns). The user rejected it: the format is the analysis script's to ask.
- **A graph of nodes** for the steps. A list with staleness says the same for the projects foreseen, and is easier to use.
- **A new contract for tools.** The board's was there, with its host; extending it kept every board module a tool.

## 4. Consequences

- **Easier:** another place, another kind of outlet, another census are a project, not a work. A question the predefined tools do not answer is a script in the project. A new predefined tool is a file.
- **The cost.** The first Agora answered in a second from a process kept alive; a tool here is a process of its own that reads its tables as JSON and asks the engine by the request. The market of a district takes four to nine seconds, and the template's plan of nine steps about a minute.
- **Not built:** the kernel fitted to observed or stated choices (the domain's `demand`); regions that are not boxes; the 3D GIS and the globe as views; tools in C++ and JavaScript; several people on one project.
- **Agora's name** is retired with its page. Its gallery pictures remain as the record of ADR 0092.
