# 0092. The Markets Domain, and Agora

- **Status:** Accepted, provisionally (2026-11-11). The work of §2.3, *Agora*, is superseded by [ADR 0093](0093-market-studio.md), *Market Studio*; the domain stands.
- **Date:** 2026-11-11
- **Decided by:**
  - **The request.** The user: "Now, I think we need to formally develop the market domain. I think it is worth an independent domain (or inside the economics one?), and an application. Of course, the whiteboard is a tool, but this is another application."
  - **The answer.** On the proposal of an independent bridge over economics and demography, with a work of its own named *Agora*: "Yes! And of course, both GIS and whiteboard etc. can be its tools." On the name, the colour, the port and the renaming of economics' planned module: "checked".
  - **The design.** Claude: [the idea of 2026-11-11](../ideas/2026-11-11-markets-and-a-board-for-ideas.md), §2 to §5, which [ADR 0090](0090-a-board-for-thought.md) deferred.

## 1. Context

ADR 0090 built the board first, at the user's turn: tools are tools, and analysis is a person's. Market analysis came to the board as a plug-in whose modules are scripts, and the record kept a debt: the computations of a market belong to a domain, in C++, checked.

The question was where. The economics domain holds value carried between positions: transfer networks, circulation, production, growth, trade, regimes. It stands at height 0 and depends on no domain. A market for analysis is another object, and it needs demography: the buyers of a later year are a projected population.

## 2. Decision

### 2.1 An Independent Domain, a Bridge

`domains/markets`, a bridge at height 1 over economics and demography, C++ from the start.

- **The object.** A market is $M = (B, \mu, q;\ O, o_0;\ K)$: buyers as a finite measure space with the spend each brings, offers with an outside option, and a kernel from buyers to offers. What market analysis computes is a functional of $M$. The theory note, `domains/markets/theory/markets.md`, states it with proofs.
- **A kernel says where it comes from:** observed, stated or presumed.
- **No map of its own.** The cost between a buyer and an outlet is given by a work. The domain depends on neither urban science nor geography.
- **Economics' planned module `markets`** (revealed preference, Slutsky, general equilibrium) is renamed `prices`, so that the two do not collide: prices as a mechanism stay with economics.
- **Financial markets** are not this object, and are left for later.

### 2.2 Its First Three Modules

- **`market`.** Demand, shares and penetration; structure through nomos; trade areas as densities, with the core of least mass holding a share of the custom (a continuous knapsack) and the overlap of two (one less the total variation distance); the spend that buys nothing; a market coarsened to a segmentation, which always has the market's demands, and what the segmentation loses (total = within + between, the part within never rising under refinement); the logit as a choice rule, and diversion by the removal itself and in closed form.
- **`sizing`.** A size as a tree of sums and products of data and assumptions with ranges. Every node rises with every leaf, so the bounds are exact and attained. Elasticities by adjoints, swings for a tornado, TAM ≥ SAM ≥ SOM as a chain, two estimates reconciled. Buyers of a later year on demography's Leslie matrix; derived demand on economics' input–output table. These two are what makes the domain a bridge.
- **`space`.** Huff's model as that logit on costs, in the power form of the original and the exponential; Reilly's law as its two-offer case; the nearest outlet as its limit; the reach of an outlet; a new outlet, and whom it draws from; where to open. The seller's demand is monotone and submodular in the set of sites, so choosing greedily gains at least $1 - 1/e$ of the best.
- **Planned:** `demand` (choice from stated and revealed data, the parameters fitted), `competition` (the diversion graph, market definition, entry as a game), `dynamics` (diffusion, cohorts, retention).

The methods that name no object stay in the foundations and are used: concentration and the pairs within reach in nomos, the logit's estimation there too, later k-means in tyche.

### 2.3 The Work: Agora

`applications/agora`, on port 9030. A work of its own, map-first; the board and the 2D GIS are its tools.

- **Four questions on the map:** the market (who sells, how much of the spend buys), whose custom (one outlet's trade area, its reach, where its custom would go), where to open (a new outlet's draw; sites chosen among the census's vacant spaces), how large (the tree with its ranges, a tornado, a seller's total, serviceable and obtained market).
- **Its engine** composes the markets domain with the urban-science domain's `access`: minutes walked on the city model's streets. It is C++, built into `data/cache/engines/agora/` and kept running, as the other works' engines are.
- **The board is embedded.** Each answer is sent to a board kept for the city: what was found, what was assumed, a table, and a chart or a map. The size is sent as quantities on notes and a figure, so that the board reckons it again.
- **Its name and colour** are provisional, checked by the user: *Agora*; a coral rose (#D9867A) with a plum minor, on cream paper by day and on plum by night. Its data keep the visualization layer's validated maps (sea for what buys nothing, gold for what is brought, orchid for one outlet's custom).

### 2.4 The Kernel Is Presumed, and Says So

The data library holds no transactions. The first city is Melbourne's centre in the census year 2024 (CLUE, CC BY 4.0): 410 blocks and 17,499 places of business in 205 ANZSIC classes.

- **The census counts dwellings and floor space, not people.** Residents per dwelling and workers per 100 m² of office are assumptions with ranges, as is what each brings.
- **The census gives no size of an establishment.** Every outlet is taken to attract alike.
- **Sellers are trading names.**
- **The census's vacant spaces are where an outlet could open.**
- Every assumption is stated at the left of the page and travels with a finding to the board.

### 2.5 In the Other Layers

- **The data library:** `data/products/market_places.py`, the places of a market in a city, in the city model's frame; a city is a case.
- **The visualization layer:** `map/plane.js`, a map of a town on its own plane. The 2D GIS of ADR 0069 draws the sphere; a district of a few kilometres is a few pixels there. A local plane had appeared once already, inside the board's map card, so the abstraction was due.

### 2.6 What Was Found

On Melbourne's centre in 2024, with the assumptions at their defaults (a resident brings 30 a week, a worker 25; both are placeholders):

- **Cafés and restaurants are a market of many small sellers.** 1,571 outlets of 1,477 trading names. The index of concentration is 0.0009, that of 1,113 equal sellers; the four largest hold 1.7%. The largest by presumed sales is a chain of eight outlets, with half a per cent.
- **Almost all of the spend finds an outlet.** 98% of it buys, walking 7.8 minutes on average. What buys nothing lies at the edges of the map, where outlets beyond it are not counted.
- **Supermarkets and grocers are another market.** 94 outlets of 77 names; 83% of the spend buys; the four largest hold 16%, the index is that of 51 equal sellers.
- **Three of the census's 1,395 vacant places** (one for each 25 m) would sell a newcomer 3.7 M a year together, the first 1.33 M; no vacant space tried one by one does better than the first.
- **The size:** 542 M a year at the middle of every assumption, 280 M to 1.09 B at their ends; what a resident and a worker spend could change it most. The board, given the same notes, finds the same three numbers by its own arithmetic.
- **A block's buyers do not stand at its centroid.** With them there, the two outlets beside the centroid of a block of large offices led the list of sellers, each with half its custom within a minute. The walk within a block is now added (§3).
- **The engine took sixteen minutes to compile in WSL** just after a whole run of the checks, of which twelve were spent before the compiler started; four minutes otherwise.

None of these is a measurement of sales: the kernel is presumed.

### 2.7 Checks

- **The markets domain: 98,** against closed forms and brute force: the core of a trade area against every subset of nine cells over forty markets; diversion by removal against the closed form; the bounds of a size against every corner; the worked chain 8.64 M, 4.32 M, 345.6 k; a stable population's λ; Reilly's classic numbers; a new outlet added to the rule; greedy sites against every three of ten.
- **Agora: 33,** of which 24 ask the engine about Melbourne: conservation, homogeneity in what buyers bring, a core's share, a new outlet's draw, the sites chosen against outlets entered one by one, and the size reckoned by the engine's tree and by the board's quantities.
- **The visualization layer: 5 more,** for the plane's fitting and zoom.
- **The page was driven in headless Chrome,** 17 assertions: not part of the suite.

## 3. Alternatives Considered

- **A module of economics.** Rejected: the object differs, and economics would have had to depend on demography, or the market's buyers go without their projection.
- **Market analysis as Tabula's plug-in alone.** It stays there, as templates. Its computations were scripts; a domain holds them with proofs and checks, and a work of its own draws them where they are: on the map.
- **Distances as the crow flies.** Rejected: the access module walks the streets, and a river or a railway is what separates buyers from an outlet.
- **A block's buyers at its centroid.** Built first, and corrected: an outlet beside the centroid of a large block was at no distance from all of its buyers, and led every list. The walk within the block is added.
- **An attraction made up for each outlet.** Rejected: nothing in the census supports one. Alike is the assumption that says least.
- **The composition in the API** (as `science/design.hpp`, ADR 0089). Deferred until a second caller exists; Tabula's modules will be it.

## 4. Consequences

- **The order of the domains gains `markets` at height 1.** It is the first domain to stand on economics.
- **Debts, stated:**
  - **The kernel is presumed.** Its parameters are declared, not fitted; no number Agora shows of Melbourne's cafés is a measurement of their sales.
  - **Outlets beyond the city model's box are not counted,** so its edges read as less served than they are.
  - **Tabula's market modules are still scripts of their own.**
  - **Segments are given, not found;** quantities are ranges, not distributions.
  - **The drive of the page is not in the suite.**
- **Not built:** other cities; the seating of cafés as attraction; the street in three dimensions; the hours of the day.
