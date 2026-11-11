# Agora: A Market on the Map of a City

*Status: draft (2026-11-11). [ADR 0092](../../development/decisions/0092-the-markets-domain-and-agora.md). Its name and its colour (a coral rose with a plum minor) are provisional, checked by the user. Its formalism is the markets domain's theory note, [`domains/markets/theory/markets.md`](../../domains/markets/theory/markets.md).*

It serves **use** and **research**.

## 1. Purpose

A market for goods and services is buyers, offers and the kernel between them: the share of each buyer's spend that goes to each offer. Agora draws that on the map of a city and answers what market analysis asks of it: who sells, and how much of the spend buys; whose custom an outlet's is, and where it would go; what a new outlet would sell, from whom, and where to open; how large the market is.

Agora is a work of its own, on the markets domain. The 2D GIS and the board are its tools: the map is the visualization layer's plane of a town, and a finding is sent to a board (the one Tabula is the work about), where it is written about.

**The market here is presumed, not observed.** The data library holds no transactions. The kernel is Huff's model on minutes walked along the streets, and every number it rests on is an assumption stated at the left of the page, to be replaced with one's own. What the census gives is counted; what is assumed is said to be.

## 2. What One Can Do

The city is Melbourne's centre in the census year 2024: 410 blocks and 17,499 places of business in 205 kinds (ANZSIC classes), on the streets of the city model. Choose what is sold in the bar: cafés and restaurants (1,571) first, or any other kind.

- **The market.** Every outlet is a dot, sized by what it would sell. The blocks are coloured by the part of their spend that buys nothing, or by what they bring. Beside the map: what all who could buy bring a year, how much of it buys, the minutes it walks, the sellers (a seller is a trading name) with their shares and outlets, and the concentration among them (Herfindahl–Hirschman, the four largest, the number of equal sellers that would be as concentrated).
- **Whose custom.** Press an outlet. Its trade area is drawn on the blocks as a density, not a ring; the blocks outlined hold three fifths of its custom in the fewest buyers. Beside it: the minutes holding half and four fifths of its custom, and where its custom would go if it closed, rival by rival, with the part of its buyers each rival shares. Diversion follows who shares buyers with whom, not who is largest.
- **Where to open.** For a newcomer or for a seller of the market. Press a point: what an outlet there would sell, how much of it drawn from rivals, from those who bought nothing and from the seller's own outlets, and what the seller would gain. Or let three sites be chosen among the census's vacant spaces, or among the blocks, each the site of greatest gain.
- **How large.** The size as a tree: the census's dwellings and office floor space, times the assumptions, each with the least and the most it is believed to be. The bounds are exact. A tornado shows which assumption could change the size most. For a seller: the total market, what lies within reach of its outlets, and what it sells.
- **The board.** Each answer has a key that sends it to a board kept for the city: a note of what was found, a note of what was assumed, a table, and a chart or a map of it. The size is sent as assumptions on notes and a figure, so that the board reckons it again by itself.

## 3. What Is Assumed

| Assumption | Default | Where it enters |
|---|---|---|
| Residents a dwelling | 2 (1.8–2.3) | The census counts dwellings, not people |
| Workers per 100 m² of office | 5 (4–6.7) | The census counts floor space, not jobs |
| A resident's spend a week; a worker's | 30 (20–45); 25 (15–40) | Placeholders, in A$ |
| Weeks of a year | 48 (46–50) | |
| Nearer is better | as a power, β = 2, with a floor of a minute; or exponentially | Huff's model |
| Reach | 15 minutes on foot | Nothing beyond it is within reach |
| Not buying | pulls as an outlet 5 minutes away | The outside option |
| Attraction | every outlet alike | The census gives no size of an establishment |

- **Minutes** are walked on the city model's ways at 1.34 m/s (steps slower; motorways closed), from a block's centroid, with the walk within the block added: the mean distance of a square's points from its centre, for a square of the block's area.
- **Outlets beyond the map are not counted,** so blocks at its edge read as less served than they are.
- **Rivals do not respond** to a new outlet.

## 4. Structure

| Part | Role |
|---|---|
| [`domains/markets/`](../../domains/markets/) | The market, its size and its space: what is computed, with proofs and checks |
| [`domains/urban-science/access/`](../../domains/urban-science/) | The minutes walked on the streets |
| [`data/products/market_places.py`](../../data/products/market_places.py) | The places of a market in a city: blocks and establishments in the city model's frame; a city is a case |
| `engine/main.cpp`, `engine.py` | The engine: the composition of the two domains for this work, built with g++ into `data/cache/engines/agora/` and kept running |
| `serve.py` | The server: the page, the shared code, the engine's operations, the board kept under `data/cache/agora/boards/`, the recipe |
| `src/app.js`, `styles.css`, `index.html` | The page: the map, what is assumed, what is found, the board |
| [`visualization/js/map/plane.js`](../../visualization/js/map/plane.js) | The map of a town on its own plane |
| [`visualization/js/board/`](../../visualization/js/board/) | The board |

## 5. Running It

```
python data/acquisition/recipe.py applications/agora/recipe.json
python applications/agora/serve.py --open
```

It opens at http://127.0.0.1:9030/. The recipe builds the engine with g++ 11 or later (natively, or in WSL) and makes the data: the City of Melbourne's Census of Land Use and Employment and the city model of Melbourne's centre. `?view=outlet` begins with a question; `?theme=night` by night.

## 6. Dependencies

The markets domain (and through it economics, demography and nomos), the urban-science domain's `access`, the API's JSON, the visualization layer (`map/plane.js`, `board`, the styles' colour maps), the data library's CLUE datasets and city model. Sources: City of Melbourne, CLUE, CC BY 4.0; streets © OpenStreetMap contributors, ODbL.

## 7. Checks

- `node applications/agora/tests/run-tests.mjs`: the page, the recipe, the server (a board kept and read back; what is not a board and another page refused), and, where the engine is built and the data are in the cache, the market of Melbourne's cafés and restaurants: conservation, homogeneity in what buyers bring, a trade area's total, a core's share, a new outlet's draw, the sites chosen against outlets entered one by one, and the size reckoned twice, by the engine's tree and by the board's quantities.
- The markets domain's 98 checks are its own (`node domains/markets/tests/run-tests.mjs`).
- The page was driven in headless Chrome over the DevTools protocol; that drive is not part of the suite.

## 8. What Is Not Yet Here

Stated so that it is not looked for:

- **Observed or stated kernels.** Transactions, visits or a survey would replace the presumed kernel, and fit Huff's parameters (the domain's `demand`, planned).
- **A size of outlets.** The City publishes the seating of cafés as a dataset of its own, not yet in the data library.
- **Other cities.** A city is a case of the data product: its census of places, its blocks and a city model.
- **Competition and dynamics:** market definition on the diversion graph, rivals who respond, diffusion, cohorts and retention.
- **The street in three dimensions,** and the hours of the day.
- **Tabula's market modules on the domain.** They are still scripts of their own.
