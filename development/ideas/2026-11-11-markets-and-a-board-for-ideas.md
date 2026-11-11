---
title: Markets and Their Analysis, a Proposed Domain, a Work, and a Board for Ideas
status: promoted (ADR 0090: the board, its modules' contract, Tabula and its plug-ins; ADR 0092: the markets domain of §2 to §5, its first three modules, and its work, Agora); the modules demand, competition and dynamics of §3 remain planned
date: 2026-11-11
related: [economics (ADR 0044, ADR 0056), demography (ADR 0085), nomos (ADR 0056, ADR 0086, ADR 0088), peira (ADR 0082), tyche, heuresis (ADR 0073), the visualization layer's map and gis (ADR 0069, ADR 0078), data in space and time (ADR 0079), the toolbox (ADR 0080), the Studio (ADR 0081)]
---

# Markets and Their Analysis: A Proposed Domain, a Work, and a Board for Ideas

*An initial design, at the user's request (2026-11-11) for "a new domain: market analysis and visualization", independent of the Urban Science Workbench, reusing the 2D and 3D GIS where they serve, with "a reusable sticky/whiteboard module", since organising ideas is part of market analysis. §9 lists what the user is asked to decide.*

*Revised the same day at the user's word (§10, §11): the board comes first and is the work; market analysis is a plug-in of it. Where §3, §7 and §8 say otherwise, §10 holds. The user judged it and it is recorded as [ADR 0090](../decisions/0090-a-board-for-thought.md) (§12). The markets domain of §2 to §4 was founded the same day as [ADR 0092](../decisions/0092-the-markets-domain-and-agora.md), independent of economics at the user's word, with a work of its own, Agora (not the "Market Workbench" of §7); its formalism is now the domain's theory note, [`domains/markets/theory/markets.md`](../../domains/markets/theory/markets.md).*

## 1. The Reading of the Request

- **Markets for goods and services.** "Market analysis" is read as the study of who buys, what is offered, by whom, where, at what price, and how that changes. Financial markets (order books, the pricing of assets) are left out at first; economics' `spacetime` already touches them, and they would be a later module or a domain of their own.
- **Three things are proposed:**
  - a domain, `markets`, holding the objects of a market and what is computed from them;
  - a work, provisionally the *Market Workbench*, composing the domain with the edges;
  - a shared module, the *board*, in the visualization layer, for any work.
- **Most methods are already held.** Concentration, accessibility, logits, stated-choice designs, surveys, survival, cohorts, spatial regression and search are in the foundations (§4). The domain's work is the objects, not the methods.

## 2. A Market as a Kernel from Buyers to Offers

**Definition.** A *market* is $M = (B, \mu, q;\ O, o_0;\ K)$, where

- $(B, \mu)$ is a measure space of *buyers* (people, households or firms), with attributes $x : B \to X$, among them a place, and $q : B \to \mathbb{R}_{\ge 0}$ the quantity or spend a buyer brings;
- $O$ is a finite set of *offers*, each a product of a seller at an outlet or through a channel, with attributes $z_o$, among them a price $p_o$ and, where it has one, a place; $o_0$ is the *outside option*, not buying;
- $K$ is a Markov kernel from $B$ to $O \cup \{o_0\}$: $K(b, o)$ is the share of buyer $b$'s spend that goes to $o$.

**What is computed is a functional of $M$.**

| Quantity | Definition |
|---|---|
| Demand | $D_o = \int_B q(b)\,K(b,o)\,d\mu(b)$ |
| Share, penetration | $s_o = D_o / \sum_{o' \in O} D_{o'}$; $\ 1 - \int q K(\cdot, o_0)\,d\mu \big/ \int q\,d\mu$ |
| Structure | a seller's share is the sum over its offers; concentration is nomos's (HHI, $CR_k$) |
| Trade area of $o$ | the measure $q\,K(\cdot, o)\,\mu$ on $B$, carried to the map by the buyers' places |
| Overlap, cannibalisation | the pairing of two trade areas; of two offers of one seller |
| White space | buyers with large $q$ and $K(b, o_0)$ near 1 |
| Size | the integrals of $q$ against a chain $\mu \ge \mu_{\text{served}} \ge \mu_{\text{obtained}}$: TAM, SAM and SOM |
| Segment | a block of a partition of $B$ on which $K$ is constant within a tolerance |

*Remark.* A trade area is a density, not a polygon. The polygons drawn in practice (the contour holding 60% of an outlet's custom) are reductions of it, and should be drawn as such.

*Remark.* A segmentation is exact when the market coarsened to its blocks gives the same demands. This is the lumpability of physis's decision 0018, read for a kernel in place of a chain.

**What is and what if.** A table of shares gives one kernel, $K_O$. Entry, exit and a change of price ask for more: a *choice rule*, a kernel $K_S$ for every set of offers $S$ and every assignment of their attributes. Under a choice rule:

- the *diversion* from $o$ to $o'$ is the part of $o$'s lost demand that $o'$ gains; for a logit within each buyer, on removal, $\delta_{o \to o'} = D_o^{-1} \int q\, K(b,o)\,K(b,o') / (1 - K(b,o))\,d\mu$;
- the diversion matrix is a weighted directed graph on the offers; its nearly closed sets are submarkets, and the hypothetical monopolist test of market definition is a question about them;
- elasticities, willingness to pay and the share of a new offer follow.

**Where the kernel comes from.** Three sources, kept apart and drawn apart, as Cosmopoiesis keeps observed, modelled and assimilated readings:

- *observed*: transactions, visits, loyalty records;
- *stated*: surveys and conjoint studies (peira's designs, nomos's logits);
- *presumed*: a model with declared parameters.

*Remark.* Huff's model, $K(b,o) \propto A_o^{\alpha}\, t_{bo}^{-\beta}$, is the conditional logit with utility $\alpha \ln A_o - \beta \ln t_{bo}$. Choice among places and choice among products are one family, which nomos's `econometrics` already estimates (ADR 0088).

**Time.** A record of transactions is events in space and time, the records of ADR 0079. A *funnel* (aware, considering, buying, buying again) is a chain of kernels, an absorbing Markov chain; lifetime value is its expected discounted reward. Adoption diffuses (Bass); buyers enter in cohorts and leave by a hazard (peira's `cohorts` and `survival`).

## 3. The Domain and Its Modules

| Module | Holds | Stands on |
|---|---|---|
| `market` | buyers, offers, sellers, the kernel and its provenance; demand, shares, penetration, structure; segments as partitions and mixtures | nomos `measurement`; tyche `multivariate` |
| `sizing` | a tree of quantities, each leaf a datum with its source or an assumption with its interval; top-down and bottom-up estimates and their reconciliation; TAM, SAM, SOM | tyche `probability` |
| `space` | trade areas as densities; Huff and Reilly; catchments from an impedance given; white space; cannibalisation; location–allocation (p-median, maximal covering) | nomos accessibility and logit; heuresis; physis (H3, Voronoi cells) |
| `demand` | choice from stated and revealed data: part-worths, willingness to pay, elasticities, demand curves, a simulator of shares | peira `design`, `surveys`; nomos `econometrics` |
| `competition` | diversion and the substitution graph; market definition (the hypothetical monopolist, Elzinga and Hogarty's flows); positioning maps; Hotelling's line, entry | nomos `games`; morphe graphs; tyche `multivariate` |
| `dynamics` | diffusion, the life cycle, cohorts, retention and churn, lifetime value, forecasts | peira `survival`, `cohorts`; tyche `time_series`, `processes` |

- **Pure, and without a map of its own.** The domain takes the impedance between buyers and outlets (minutes, metres, a cost) as given. A work computes it, on the urban-science domain's `access` for streets or on a great circle for regions. The domain therefore does not depend on urban science or geography.
- **Placement.** A bridge at height 1 over economics (prices, trade, the value chain of `production`) and demography (the buyers of a future year). Economics' README plans "markets and money"; the markets would go here, and money stay there.
- **C++ from the start,** header-only, checked against closed forms, brute force and reference libraries (statsmodels, PySAL's `spopt` for location–allocation, lifetimes for repeat purchases), with fixtures generated by script.
- **First slice:** `market`, `sizing` and `space`.

## 4. What the Foundations Hold, and What They Would Gain

**Held, and to be used, never rewritten:**

| Need | Where |
|---|---|
| Concentration, specialisation, location quotients, composite indicators | nomos `measurement` |
| Gravity access, the two-step floating catchment, unmet need | nomos `measurement` (ADR 0086) |
| Conditional, nested and mixed logit; willingness to pay | nomos `econometrics` (ADR 0088) |
| Stated-choice designs; surveys, raking; experiments (A/B tests) and their power | peira `design`, `surveys`, `randomisation` |
| Retention and churn; cohorts on the Lexis diagram | peira `survival`, `cohorts` |
| What a change did (a campaign, an opening), with spillovers | peira `quasi`, `interference` |
| Regression, panels, ARIMA, Moran's I, spatial lag and error models, hidden Markov models | tyche |
| Shortest paths and centralities; H3; search under many objectives | morphe; physis; heuresis |
| Equilibria of games; matching | nomos `games`, `matching` |

**To gain, each when a module first needs it:**

- **tyche `multivariate`:** k-means, Gaussian mixtures and principal components (planned there; heuresis also waits for k-means), and multidimensional scaling or correspondence analysis for positioning maps. **tyche `descriptive`:** kernel densities (planned).
- **physis `computational_geometry`:** Voronoi cells in the plane (planned).
- **nomos:** Bass's diffusion; the aggregate logit with Berry's inversion (now in urban science's `housing`; a second use is the rule's occasion to move it down); the latent-class logit; models of repeat purchase (BG/NBD).
- **heuresis or morphe:** location–allocation as a class of problems, with exact answers for small cases.

## 5. What a GIS Gives a Market

By question, not by technique:

1. **Where the buyers are.** $\mu$ on the map: cells of people (GHS-POP, H3), with what a census gives of them.
2. **Where the offers are.** Places of business (Overture's places, a city's own registers, the user's outlets).
3. **What separates them.** Minutes on the real network, not circles.
4. **Whose custom is whose.** Trade areas as densities; where two overlap, they compete; where a seller's own overlap, it cannibalises.
5. **Where no one serves.** White space, hatched as ADR 0086 hatches unmet need.
6. **Where to open next.** Location–allocation, the candidates and the front of a search on the map.
7. **When.** Footfall by the hour; a record's events on the line of time.

- **The 2D map is the default.** Most questions of a market are regional and read best flat (`visualization/js/map`, with its naming policy). The 3D GIS (`visualization/js/gis`) is for the street: footfall past a frontage, what stands beside an outlet, floors above one another, and time rising above the map.
- **Maps that are not of the Earth.** A positioning map places offers in a space of attributes or perceptions. It is drawn with the same marks, scales and legends of the scene model.

## 6. The Board

### 6.1 Purpose

Half of a market analysis is judgement: hypotheses, assumptions, what an interview said, what a competitor did. These need arranging, and tying to the evidence. A board holds them beside the maps and the numbers, and is the same for any work that wants one.

### 6.2 Content Apart from Placement

**Definition.** A *board* is $(V, R, \beta;\ \lambda)$:

- $V$: items, each of a kind (note, text, image, region, card) with its content;
- $R$: typed relations among items (supports, contradicts, causes, part of, follows, or a plain arrow);
- $\beta$: bindings from items to things outside the board: a place, an instant, a dataset, an object of the analysis (a segment, a seller), a computed figure, a source;
- $\lambda$: a placement, giving each item a position, a size and a style.

$(V, R, \beta)$ is what is thought; $\lambda$ is how it lies. An *arrangement* is a function from content to placement:

| Arrangement | From |
|---|---|
| free | the hand |
| clusters (affinity) | regions |
| tree (an issue tree) | *part of* |
| matrix (2 × 2) | two scored attributes |
| line of time | instants bound |
| map | places bound |

A template (SWOT, the five forces, a business model canvas, PESTEL, a journey, a value chain) is a set of named regions: data, not code.

### 6.3 More than Slips of Paper

- **Kinds with a status.** A note is a fact (with its source), an assumption (with a value and an interval, if it has one), a question, an insight, a decision or a risk, and is drawn by its kind.
- **Live cards.** A map, a chart, a table or a tool's answer placed on the board, still live. The Studio's frames are this pattern.
- **Assumptions that compute.** An assumption with a quantity is a parameter that `sizing` reads. Editing the note changes the estimate, and every estimate names the assumptions it rests on.
- **Notes in space and time.** A note bound to a place is pinned on the map; the notes bound to places and instants are a record of events (ADR 0079).

### 6.4 Placement in the Repository

`visualization/js/board/`: the model, its operations, the arrangements, a renderer and the templates. The canvas of placed items has now appeared twice (the Studio's canvas of frames, and this), so the abstraction is due. The Studio is not moved onto it now; it may be later.

### 6.5 Engineering

- **Schema.** `noetarium.board/1`, JSON, after `noetarium.grammar/1`.
- **State as a fold of operations.** Undo, redo and history follow; operations are designed to commute where they can, so that boards can later be merged.
- **Renderer.** DOM and SVG under one transform for pan and zoom; pointer events for mouse, pen and touch. Text is edited in place.
- **Checks.** The model, operations and arrangements are pure and run under Node: each operation with its inverse, the arrangements deterministic, the schema read and written back.
- **Storage.** By the work's server, under `data/cache/<work>/boards/`; the user's boards are never committed. Exports: JSON, SVG, PNG.
- **Not at first:** several people on one board at once, freehand ink, a language model on the board.

### 6.6 Appearance

Vellum slips with hairlines, serif type, the kinds in the work's palette. Not office-yellow squares.

## 7. The Work

- **Name and port.** Provisionally the *Market Workbench*, on port 9010. Its colour is the user's to choose within the warm family.
- **Views.** Board · Map · Market (structure, segments, positioning) · Sizing · Demand · Dynamics · Toolbox (the shared one of ADR 0080).
- **Reuse.** `visualization/js/map`, `gis`, `toolbox`, the records and marks of ADR 0079, the scripts' contract in three languages.
- **Data.** The library's (people, places of business, footfall, the economies' indicators), and the user's own: outlets, sales, customers, as CSV. A user's customers stay on their computer.
- **An engine** in C++ on the domain and the foundations, built by the recipe and kept alive by the server, as the other works'.

**A first case on data already held:** cafés and restaurants in Melbourne's centre. CLUE gives the businesses and their floor space, the counters the walkers by hour, the censuses the residents and workers, the streets the minutes. It can be sized, its trade areas drawn, its white space found.

*Caveat.* Without sales or visits, the kernel of that case is presumed. The case can show the machinery, not validate it. A case with the user's own records, or with open visit data, would make the kernel observed.

## 8. Stages

1. **The board,** and the work's shell with Board and Map: notes, regions, relations, templates, notes pinned to places.
2. **`market`, `sizing`, `space`,** and the first case: assumptions that compute, trade areas, white space.
3. **`demand` and `competition`:** a survey designed and read, a share simulator, the substitution graph.
4. **`dynamics`:** diffusion, cohorts, lifetime value.

The foundations gain what §4 lists as each stage reaches it.

## 9. What the User Is Asked to Decide

1. **Scope.** Markets for goods and services first, financial markets later (proposed); or both.
2. **The domain.** Its name (`markets`) and place (a bridge on economics and demography, height 1), taking "markets" from economics' plan.
3. **The work.** Its name and colour.
4. **The first case.** Melbourne's cafés on held data (proposed, as a demonstration), or a market the user is analysing, with their own data.
5. **The board.** One person at a time first; in `visualization/js/board/`; the Studio left as it is.

## 10. The Design Revised: The Board First, Subjects as Plug-ins

### 10.1 The User's Word

"Tools just tools, human analysis still very important. This is why I said a reusable whiteboard, sticky etc module so important. Please make them delicate, and they may also utilize in other domains. So I think market analysis as a template/plug-in for this whiteboard (can add text, images, various stickies, brainstorm, even Gantt diagrams) is better. Of course, advanced tech, like scripts and GIS, visualization, data figure generation, ABM, historical events visualization, are also important."

### 10.2 What Changes

- **The work is the board.** Not a market workbench with a board among its views, but a board (provisionally *Tabula*, `applications/tabula`, port 9010) on which any subject is worked.
- **A subject is a plug-in:** its kinds of note, its templates, its cards. Market analysis is the first; a small research plug-in stands beside it so that the board is seen not to be the market's.
- **Advanced techniques come to the board as cards:** live views placed among the notes. A map, a chart, a script's drawing, a model's run and a history's events are each a card a plug-in declares.
- **The markets domain keeps its design (§2–§4) and waits.** It is written when a card first needs its computations. Until then the plug-in computes with the board's own arithmetic: quantities with ranges, and plans.

### 10.3 What Is Built

- **`visualization/js/board/`:** the model `noetarium.board/1`; operations with inverses and a history; geometry; arrangements; plans and chronicles written as lines; quantities by interval arithmetic; templates; the drawing, the hand's work, a rail, a panel and the picture. 53 checks in the visualization layer's suite.
- **`applications/tabula/`:** the page, a server that keeps boards and images under `data/cache/tabula/`, and two plug-ins, market analysis (nine templates, three kinds of note) and research (three, two). 18 checks, and 27 assertions by driving the page in headless Chrome.

Three things §6 proposed are built differently, and one is added:

- **Membership is kept, and follows the place.** An item's region is stored on the item (`in`), so that content does not refer to placement; the view sets it from where the item is dropped.
- **Arrangement by a matrix of scores is not built.** A two by two is four regions placed by hand.
- **Notes bound to places are not built.** They wait for the map card. Notes can say *when* and *from what source*.
- **Added: a syntax for plans and chronicles,** a line an entry, read by a small grammar, with the float of each entry and the chain that cannot slip.

### 10.4 The Stages, Revised

1. **The board** (built): notes, regions, links, text, shapes, images, plans, chronicles, figures, templates, arrangements, keeping, the picture.
2. **Cards:** a chart from a table; the 2D map with the notes that have places; a script's drawing (the contract of ADR 0079 in three languages); then the 3D GIS, a model's run, a history's events.
3. **The markets domain,** from `sizing` (quantities as distributions) and `space` (trade areas), each reaching the board as a card.
4. **Several people on one board.**

### 10.5 What the User Is Asked to Decide Now

1. **The name.** *Tabula* is provisional.
2. **The colours.** Peony and orchid on cream paper, plum by night, are provisional.
3. **The record.** An ADR (0090), CLAUDE.md, the README and the release notes are to be written once the name stands and the board has been judged.
4. **Which card first.**
5. **The canvases of Strategyzer.** The Business Model Canvas is CC BY-SA 3.0 and the Value Proposition Canvas is not openly licensed; both are left out of the plug-in until the user says how to treat them.

## 11. The Second Revision: A Gentler Board, the Pen, and Plug-ins That Get Data and Run Models

### 11.1 The User's Word

"The whiteboard UI can be more gentle, delicate, beautiful. Even allow painting, utilize touch pen. And I think most importantly, allow plug-ins, specifically market analysis modules for our new domain, can get, visualize GIS data, even run machine learning models." And, while it was being built: "The plug-in can be like Unity style? It provides what parameters are needed, then the UI provides specific input, then the plug-in can do some: a figure's plug-in can make a CSV file path parameter, after set up it visualizes the data, and places it somewhere on our board. And we may even need to implement a layers panel then?"

### 11.2 What Was Decided in the Building

- **A plug-in says what it needs, and the board asks for it.** A module declares its parameters, each with a kind and, where it names something, a role: a table of the board, a column or columns of it, a quantity a note defines, a file. The panel is built from the declaration. This is the infrastructure's own pattern (the toolbox's declared forms, ADR 0056; the plug-ins of ADR 0029 and ADR 0079), which the user's comparison with Unity's inspector names well.
- **One kind of datum between everything: the table,** in the data library's schema. A sheet pasted, a file dropped, a module's answer and a file a module read are all tables with names; a chart, a map and a module draw a table by its name, whoever made it. GIS data are tables whose rows have places (`lat` and `lon`, or `iso3`).
- **The contract is the API's,** `science.board`, on the host that runs the other works' plug-ins, so that a module is of the infrastructure and not of one page. `networks.param` gained the kind `text`.
- **A module's answer is placed with one press:** a table it answered is laid beside it as a map where its rows have places, else as a chart. A figure it drew itself (matplotlib) is shown on its card.
- **The pen is of the board, not a plug-in.** A stroke is an item: its points with their pressures, simplified within a tolerance, drawn as an outline. It was not too difficult to integrate, so no separate canvas was needed.
- **The assumptions on the notes reach the models.** A module reads a quantity with its range from the board; the market's module of worth carries the range through exactly, since both its assumptions are positive.

### 11.3 What Is Built

- **The look:** a warm ground with a wash of the accent, slips of paper with a rule in the colour of their kind, hairlines, vellum sheets that let the ground show through, a rail of small keys named on the pointer, a bar whose keys are words, a dusty orchid that marks only what is chosen. The exported picture is still (an item caught as it arrived had been drawn empty; found by looking at the picture).
- **The pen and the rubber;** tables; cards (chart, map, module); the contents of the board in the panel, with hiding and holding.
- **`api/python/science/board.py`** and its 37 checks.
- **Four modules of the market plug-in on real data:** 204 economies of 2023 from the data library; what a market could be worth in each from two assumptions (the first five: the United States, China, Japan, Germany, the United Kingdom, at the placeholders' values); the economies in four segments by k-means (numpy's, scikit-learn not being installed); 1,677 cafés and restaurants of the City of Melbourne in 2024 on its 603 blocks. One general module, a figure from a file.
- **Found while checking:** the delimiter of a sheet was sniffed line by line, which a line break within quotes defeats; a range written after a negative number took the sign for the whole value; text read from an element came back in capitals where the style sets capitals.

### 11.4 On Layers

The contents list is a first form: the board's items as its regions hold them, each selected and brought into view from its row, hidden or held. What a layers panel proper would add, and is not built: named layers independent of regions, an order and an opacity for each, and moving items between them by dragging. Regions already do much of what layers do on a board: they gather items, which move with them. Hiding a region with all it holds would be the natural next step. Whether layers are wanted beside regions is the user's to say.

### 11.5 The Stages, As They Now Stand

1. The board (built), with the pen, tables, cards and modules (built).
2. More cards: the 3D GIS, a model's run in time (an agent-based model), a history's events.
3. The markets domain in C++ (`sizing` with distributions, `space` with trade areas), reaching the board through modules.
4. Modules in C++ and JavaScript on the same contract; several people on one board.

## 12. The User's Judgement, and What Followed

"1, 2 all checked": the name *Tabula* and its colours stand, and layers proper are wanted. Of the 3D GIS as a card, an agent-based model's run and historical events: "I think we should implement, but as plug-ins."

- **Layers** are built in the board: named, back to front above the ground, each hidden, held or made faint with all that lies on it; new items are laid on the layer in the hand.
- **Three plug-ins** are built: `gis-3d` (a table's places on a city of the city model, on the visualization layer's 3D GIS), `agents` (a run played with a clock; Bass's diffusion as agents, held to its mean field) and `history` (a canon's events on a line of time).
- **A card may leave something to be released,** which a scene in three dimensions and a clock need.
- The whole is recorded as [ADR 0090](../decisions/0090-a-board-for-thought.md), with its debts.
