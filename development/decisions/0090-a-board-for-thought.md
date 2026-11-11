# 0090. A Board for Thought, and Subjects as Its Plug-ins

- **Status:** Accepted, provisionally (2026-11-11).
- **Date:** 2026-11-11
- **Decided by:**
  - **The request.** The user: "We now implement a new domain: market analysis and visualization. Independent to the urban workbench, but of course 3D and 2D GIS module can be reused if necessary. But before implementation, we need design. What market analysis basic objects, how GIS and other methods may help it. And whether we can make a reusable sticky/whiteboard module for it? I think sometimes organizing ideas is also important in market analysis."
  - **The turn.** The user, on the first design: "Tools just tools, human analysis still very important. This is why I said a reusable whiteboard, sticky etc module so important. Please make them delicate, and they may also utilize in other domains. So I think market analysis as a template/plug-in for this whiteboard (can add text, images, various stickies, brainstorm, even Gantt diagrams) is better. Of course, advanced tech, like scripts and GIS, visualization, data figure generation, ABM, historical events visualization, are also important."
  - **The second turn.** "The whiteboard UI can be more gentle, delicate, beautiful. Even allow painting, utilize touch pen. And I think most importantly, allow plug-ins, specifically market analysis modules for our new domain, can get, visualize GIS data, even run machine learning models."
  - **The manner of plug-ins.** "The plug-in can be like Unity style? It can provide what parameters are needed, then in the UI can provide specific input, then the plug-in can do some."
  - **The last word.** On the name and colours, and on layers: "1, 2 all checked." On the 3D GIS, an agent-based model's run and historical events: "I think we should implement, but as plug-ins. After finish, please commit and push."
  - **The design.** Claude: [the idea of 2026-11-11](../ideas/2026-11-11-markets-and-a-board-for-ideas.md), with its two revisions.

## 1. Context

The infrastructure's works compute and draw. None held what is thought about the results: what is assumed, what was heard, what is doubted, what was decided, and how these bear on one another. The Studio (ADR 0081) lays frames on a canvas for a figure; it is not for thinking.

The user asked for market analysis, and saw that its centre is judgement, which tools serve. So the first thing to build was the ground judgement is set out on, for any subject, with market analysis coming to it as one subject among others.

A canvas of placed items had now appeared twice (the Studio's frames, and this), so the abstraction was due (§2.2 of CLAUDE.md).

## 2. Decision

### 2.1 The Board, in the Visualization Layer

`visualization/js/board/`, exported as the namespace `board`, is a board for thought that any work embeds.

- **Content apart from placement.** A board is $(V, R, \beta;\ \lambda)$: items $V$ of kinds; relations $R$ (links that say *supports*, *contradicts*, *leads to*, *part of*, *then*, and membership in a region); bindings $\beta$ to what lies outside it (a source, a date, data); and a placement $\lambda$ (a rectangle for each item, an order, layers). Content never refers to placement, so an arrangement is a function from the one to the other: a grid, a region's items tidied, a tree by the links, the order of time.
- **Every change an operation with its inverse.** A board is the fold of its operations (`add`, `remove`, `set`, `place`, `order`, `title`, `layers`). Undo and redo are exact, a refusal changes nothing, and operations on different items commute, which merging will later need.
- **The schema** `noetarium.board/1`, plain JSON.
- **Kinds.** Notes of kinds (an idea, a fact with its source, an assumption, a question, an insight, a decision, a risk; a plug-in adds its own), text, images, shapes, regions, links, strokes of a pen, plans and chronicles, figures, tables, cards.
- **The pen is of the board.** A stroke is an item: its points with their pressures, kept in fewer points within a tolerance (Ramer, Douglas and Peucker), drawn as an outline as wide as the pen pressed. A mouse or a stylus draws; fingers still move the board.
- **Plans and chronicles are written as lines** (`Interviews : after Desk research, 3w, 20%`; `480 BC : Salamis`), read by a small grammar, resolved to days of the proleptic Gregorian calendar, with the float of each entry and the chain that cannot slip.
- **Assumptions that compute.** A line of a note such as `share = 3% [2%, 5%]` defines a quantity with its range. All definitions on a board are one system, evaluated by interval arithmetic; a figure names the values written down that it rests on.
- **Layers.** Named layers above the ground, back to front, each hidden, held or made faint with all that lies on it. New items are laid on the layer in the hand. The panel also lists the board's contents as its regions hold them.
- **Its look.** Paper, not plastic: slips of tinted paper above a warm ground, hairlines, vellum sheets, serif type. Every colour is a variable.

### 2.2 Tables, Cards and What a Card Declares

- **One kind of datum between everything: the table,** in the data library's schema (named columns of numbers or of text). A sheet pasted, a file dropped and a module's answer are all tables with names. GIS data are tables whose rows have places: `lat` and `lon`, or `iso3`.
- **A card is a live view among the notes** that draws a table by its name, whoever made it. The board has three: a **chart** (the toolbox's charts, ADR 0080), a **map** (the 2D GIS's projections, ADR 0069: points on a town's own plane over a layer of the data library, economies on Equal Earth, areas in dispute hatched and unnamed), and a **module**.
- **A card says what it needs.** It declares its parameters, each with a kind (a number, a tick, a choice, words) and, where it names something, a role: a table of the board, a column or columns of it, a quantity a note defines, a file. The panel is built from the declaration. This is the infrastructure's pattern of declared forms (ADR 0029, 0056, 0079), which the user's comparison with an inspector names.
- **A card may leave something to be released** (a scene, a clock) when it is drawn again or taken away.

### 2.3 The Contract of a Board's Modules, in the API

`api/python/science/board.py`. A module is a script that is given what stands on a board (its tables, its quantities with their ranges, its notes) and answers tables, notes and figures. It declares its parameters as above. It gets data where the data library keeps them (`cache`, `library`, `file`) and may use whatever its Python has.

- `board.Host` is `networks.Host` (ADR 0046), on a board: each run a process behind the file contract, stopped after its time. `networks.param` gained the kind `text`.
- A few lines written on a card need only `def run(board, api)`.
- The contract is written for Python only so far.

### 2.4 The Work: Tabula

`applications/tabula`, on port 9010. *Tabula* is the name the user confirmed, with its colours: a dusty orchid (#C586B6) and a soft violet (#A58FD0) on cream paper by day, on plum by night.

- **Its server** keeps boards, images and files of data under `data/cache/tabula/` (never committed), describes the modules of the plug-ins and runs them.
- **Who may run a module.** A module runs with the authority of whoever started the server. Only the server's own page, at this computer's own address, may ask for a run (ADR 0079's rule); a page of another origin keeps nothing either. A board opened from a file runs nothing by itself.
- **A plug-in** is a directory: `plugin.js` (kinds of note, templates, cards), `modules/*.py`, and a style sheet if it has cards. Nothing about a plug-in is written into the page.

### 2.5 The Plug-ins

| Plug-in | Brings | On |
|---|---|---|
| `market-analysis` | Three kinds of note, twelve templates (the size of a market from assumptions, where the buyers are, where the offers are and the same in the round, what to test first, SWOT, five forces, PESTEL, a positioning map, a segment, a buyer's journey, going to market), four modules | the data library's table of economies (World Bank WDI); the City of Melbourne's CLUE |
| `research` | Two kinds of note, three templates (hypotheses and evidence, a reading map, a study planned) | the board alone |
| `gis-3d` | A card: a table's places standing on a city of the data library's city model, each a column on the roof it lies under | `visualization/js/gis` (ADR 0060), three.js |
| `agents` | A card that plays a run with a clock; a module, Bass's diffusion as agents on a lattice; a template | numpy |
| `history` | A card that sets a table of events on a line of time, each kind in a colour of its own; a module that reads a canon; a template | the history domain's canons (ADR 0041) |
| `general` | A module: a figure from a file named by a parameter | matplotlib |

**The market's modules:**

- *Economies in a year:* every economy with its people, its income per person and its people in towns.
- *What the market could be worth, by economy:* buyers and their spending from two assumptions read from the board's notes. Both are positive, so the range of the answer is exactly the range of the assumptions.
- *Segments, by k-means:* scikit-learn's where it is installed, else the same algorithm with numpy.
- *Places of business in Melbourne:* the establishments of one kind in a census year.

### 2.6 What Was Found

- **On real data.** 204 economies of 2023; the economies in four segments accounting for 78% of their spread in income and urban share; 1,677 cafés and restaurants of the City of Melbourne in 2024, 969 of them in the centre, standing on its 15,038 buildings.
- **The agents are held to their mean field.** Where everyone looks to everyone, the mean of ten runs of ten thousand buyers lies within 0.0015 of Bass's recursion $F_{t+1} = F_t + (1 - F_t)(p + q F_t)$.
- **Found while checking.**
  - A range written after a negative number took the sign for the whole value.
  - The delimiter of a sheet was sniffed line by line, which a line break within quotes defeats.
  - Text read from an element came back in capitals where the style sets capitals.
  - The exported picture drew every item empty, each caught as it arrived; it was found by looking at the picture.
  - The first layer of a board, undone, left an empty list where there had been none.

### 2.7 Checks

- The board: 73 checks in the visualization layer's suite (`visualization/tests/board-checks.mjs`). Among them: every operation undone exactly; interval arithmetic enclosing 8,000 sampled values; the calendar against JavaScript's own from 1583 to 2400; float against a case worked by hand; the contrast of ink on every tone by day and by night.
- The contract: 37 checks in the API's (`api/tests/check_board.py`).
- Tabula: 43 checks (`applications/tabula/tests/run-tests.mjs`), the market's modules against closed forms on the data library's own data, the agents against their mean field.
- The page was driven in headless Chrome over the DevTools protocol, 70 assertions in three drives. They are not part of the suite.

## 3. Alternatives Considered

- **A market workbench with a board among its views** (the first design). Rejected by the user: the board is the work, and subjects come to it.
- **The pen as a canvas plug-in.** The user offered it in case integration proved difficult. It did not: a stroke is an item like any other, so it is selected, moved, undone and laid on layers with the rest.
- **The 3D GIS, a run and events as cards of the board itself.** Rejected by the user: they are plug-ins. The board keeps only what every subject needs (a chart, a map, a module).
- **A contract of the page's own for modules.** Rejected: the infrastructure has one host for plug-ins that run as processes (ADR 0046, 0079), and a module should be of the infrastructure.
- **Datasets by reference in a board** (a table that names a file of the data library). Not built: a module gets the data and answers a table, so that a board holds what it shows.
- **Moving the Studio's canvas onto the board.** Deferred: the Studio is a figure's layout with a schedule of scripts, and nothing asks for the move.
- **Founding the markets domain now.** Deferred with the user's turn. Its design stands in the idea (§2 to §4): a market as a kernel from buyers to offers.

## 4. Consequences

- **A work can now hold thought beside its results,** and any work can embed the board.
- **Debts, stated:**
  - **The agents' model is numpy in a plug-in.** The general home of agent-based models is nomos's `agents` (ADR 0071), in C++. The module is held to its mean field, and should call nomos once an engine serves the board.
  - **The market's modules are scripts.** The markets domain's computations (trade areas, choice, diffusion, quantities as distributions) are to be C++ in the domain, reaching the board through modules.
  - **The events card does not convert calendars.** A canon writes each date in its source's calendar; the card's line is proleptic Gregorian. The two differ by ten to thirteen days, which a line of years does not show. The history domain converts exactly.
  - **The canons are seeds** drafted by a language model and unchecked (ADR 0041); the module says so with every answer.
  - **The contract is Python's only.** The host runs C++ plug-ins for the other works.
  - **A stylus was not tried.** The pen was driven with a mouse; pressure, the stylus's own rubber and fingers moving the board under the pen are written and unchecked. Copy and paste, and files dropped, were not driven either.
  - **The drives of the page are not in the suite.**
- **Left out for their licences:** the Business Model Canvas (CC BY-SA 3.0) and the Value Proposition Canvas, until they are checked against this repository's.
- **Not built:** several people on one board; a native SVG picture; dragging a plan's bars; units that are checked.
