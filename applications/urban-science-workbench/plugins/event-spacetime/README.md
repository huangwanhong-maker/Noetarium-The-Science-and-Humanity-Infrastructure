# Event Spacetime

*A district's real events as a 2-complex, drawn above a city that can be seen through.* A plug-in of the [Urban Science Workbench](../../README.md). Status: **draft** (2026-11-04; its grammar 2026-11-26, [ADR 0168](../../../../development/decisions/0168-the-grammar-of-event-spacetime.md); New York's record, the patterns across worldlines and the histories grown from the grammar, [ADR 0171](../../../../development/decisions/0171-patterns-grown-histories-and-new-york.md)). Activity: research. [ADR 0067](../../../../development/decisions/0067-event-spacetime.md); the design and its formalism, [development/ideas/2026-11-04-event-based-gis-on-a-2-complex.md](../../../../development/ideas/2026-11-04-event-based-gis-on-a-2-complex.md), for the planned paper *Event-Based 3D GIS on a 2-Complex: A Spin-Foam Approach*.

## 1. Purpose

A geographic state is the realization of a history of events. The plug-in holds a district's record of real events as the complex its design defines (the design, §1–2):
- **Vertices are events.** The two ends of the record's window are vertices too.
- **Edges are couplings.** These are volumes: buildings, the space of a street between two junctions, the space of a junction. Each lives between two events it attends.
- **Faces are primitives.** These are interfaces between two volumes: party walls, frontages, the cross-sections where a street meets a junction. Each is a lens between two events that meet it, attached up one side's worldline and down the other's.

It draws that complex in spacetime above the district, with time rising, and the network of the moment at the clock's date. The city beneath can be made to fade, so that the complex shows through its towers.

## 2. The Record

Melbourne's Hoddle Grid, the middle of Landscape Dynamics' district (Elizabeth–Russell, Flinders–Lonsdale), 2002–2024. Every fact attaches to the cell it names by identifier. The data library compiles them (`data/products/city_events.py`, `city-events-melbourne.json`):

| Source | What it gives the record |
|---|---|
| CLUE's buildings, by `base_property_id` | each building's construction (a birth, when in the window), refurbishments (alterations) and its state at every census (an observation, a local tick) |
| The Development Activity Monitor | developments completed: a birth, or an alteration of a building that stood |
| The Victorian Heritage Register | registrations (an institution touching the building) and their amendments |
| Event permits, 2014–2018 | the streets' events, a state of the street spaces they name from their start to their end |

**New York.** The Financial District's old streets about Wall Street, Stone Street and Hanover Square (Landscape Dynamics' district), 2008–2024, from NYC Open Data (`data/products/city_events_new_york.py`, `city-events-new-york.json`):

| Source | What it gives the record |
|---|---|
| Building footprints | the buildings constructed in the window (births, at their construction year), joined by position |
| Certificates of occupancy (DOB's and DOB NOW's) | each job's first certificate: a new building's completion (a birth, or an alteration where the building had an earlier account of its making), a major alteration's completion |
| Individual landmark sites | the designations of the window (institutions) |
| Permitted events | each block of their location, a street between two cross streets resolved on the district's network; a full closure of the street meets the street spaces' cross-sections |
| Film permits (from 2023) | each block of the parking they hold; parking held is no closure |

In both records births end nothing: the buildings are those standing now (OpenStreetMap's in Melbourne, Valdrada's in New York), so a demolition before 2025 has no geometry to stand on. Melbourne's permits do not say whether a street was closed to vehicles, so they touch the street spaces and meet none of their interfaces. New York's full closures say so, and meet the cross-sections, whose permeability they change.

## 3. Layers and Analyses

**The district's events in spacetime.** Time rises 4, 10 or 20 m a year, from the ground or from above the tallest building. It shows:
- worldlines for the volumes, their vertices marked;
- sheets for the interfaces' lenses;
- beads for the events, coloured by kind, or, once the grammar is learned, by what each cost the grammar of the events before it;
- a plane at the clock's date with the network of what stood then, nodes and links.

Two ways to see one complex: refined (K̂, each event split into its volumes, joined by rungs), or as events (K, each event one vertex, the worldlines bending into it). Pick a bead, a worldline or a sheet for what it is; a bead names its rule in the grammar once it is learned.

**The record and its complex.** Seven tabs:
- **The record:** what the record holds and what it left out, with its reasons and its sources.
- **The complex:** the cells and homology of K and K̂.
- **The moment:** the network at the clock's date.
- **What it remembers:** the buildings' uses and the premises' trades, fitted as chains on their classifications: each level's rate, its memory and how long it is half kept, against one level alone; and the buildings' chains on the event complex, the record's probability by the contraction against the product of their transitions.
- **Rates:** each kind of event per year of exposure.
- **Its grammar:** the complex read as a foam whose words are the events' stars, with the patterns of its changes across the worldlines and histories grown from the grammar on request ([the theory note](../../../../domains/urban-science/theory/the-grammar-of-event-spacetime.md)), by the events' types or kinds. It shows what each event cost the grammar of the events before it, year by year, with and without its surroundings and the states carried into it. It compares the grammars as a partial order, and lists the rules with their κ, their shares of χ and their rates. It shows how χ is generated, how each new building is attached, and the volumes' lives compressed kind by kind.
- **The drawing:** how it is drawn, which cells, the city's opacity, and the buildings' worldlines coloured by their unseen changes of use.

## 4. The Engine

C++ on urban-science's `events` (the record, its complexes, the cut), its `landscape` (the district) and architecture's `space` (the massing), on morphe's worldline complexes (`engine/main.cpp`, `engine/ops.hpp`):
- **`record`.** Takes the district (its network and buildings, sent by the page) and the area's record. It returns the couplings, interfaces and events for drawing, the topology, and what was left out.
- **`cut`.** Returns the network of the moment at an instant.
- **`grow`.** Histories grown from the record's grammar (urban science's `grow`): the record without its censuses, each space carrying the number of regimes in force on it, its sites and rates in the eras of the area's sources (the page asks Melbourne's cut at 2014 and 2018, New York's at 2012 and 2023). Each history is measured as the record is (events by kind and year, χ, the depth of its trace, its prequential code, how far its street spaces' lives and its net compress), with a summary against the record and how far the table of what followed is from a function.
- **`grammar`.** The record's grammar on urban-science's `events/grammar.hpp` and syntaxis's `foams`, with the patterns of its changes (its process net compressed by digrams), read five ways: as recorded; with the buildings' uses, the regimes in force, and both, carried into each event; and without the censuses. Each reading gives its cells, rules, entropies, prequential judgement by year and in two eras about a date (2014 unless asked), lives and Foata steps; the grammars are compared by dominance.

The workbench's checks give the engine a district of three buildings and a street, with a record of their own, and hold every count to its value by hand, its grammar's among them.

## 5. What the District Remembers

The design's stages E3 and E4: physis's chains on a classified set and its contraction of a record (decision 0020), fitted to the censuses of 2002–2024.
- **The buildings' uses.** 404 properties, 8,455 transitions between censuses, 22 of CLUE's space uses in their families. A building half keeps its family (Retail, Unoccupied, Parking, …) for 7.3 ± 0.3 years and, within it, its exact use for 2.9 ± 0.3 years. One level alone is rejected, at twice the log-likelihood ratio 128 on one degree of freedom.
- **The premises' trades.** 213 premises that one establishment holds, 3,154 transitions, 68 ANZSIC classes. A premises that changes trade mostly draws a new one from the whole district's mix, at 0.093 ± 0.006 a year, so its division, subdivision and group are half kept for 7.4 ± 0.5 years. Within its group it changes class at a further 0.020 ± 0.007 a year, so its exact class is half kept for 6.1 ± 0.5 years. The division and the subdivision have no rate of their own, at the bound: a premises that changes hands keeps its group or loses everything. One level alone is rejected, at twice the log-likelihood ratio 17.9 on three degrees of freedom (p = 5 × 10⁻⁴).
- **On the complex.** The buildings' chains on the worldlines of K, contracted through every event they attend (8,272 cells), give log Z = −4191.535138, equal to the product of their transitions.

## 6. The Grammar of the Record

On the same district ([the experiment](../../../../research/experiments/2026-11-26-event-grammar/README.md)), 9,036 events, χ = −26,671:
- **What generates χ.** The censuses, alterations and registrations have κ = 0 and change nothing of the topology. The 555 permits, each holding on average 26 street spaces at its start and at its end, write χ, with the window's ends and the nine births.
- **What the grammar foresaw.** Before 2014 an event costs 0.2 to 0.4 bits. 2014–2017 bring 224 rules never seen, and from 2018 the grammar foresees nearly everything.
- **Which context tells.** The surroundings save 3.9 bits over the record and the uses 29. The regime in force saves 338 (7.0%), since a permit's start and its end consume the same spaces.
- **How a building is attached.** Every birth gives each neighbour exactly one interface, so its κ is (1 − m)/2.
- **What lives share.** The segments of each named street share its permits: the street, never declared by the record, appears as a rule of their lives.
- **Patterns across the volumes.** Each named street is one pattern whose every instance takes the same street spaces: a permit and its end on Swanston Street's 11 segments 49 times, on Bourke Street's 20 spaces 48 times.
- **Grown histories.** Forty histories grown from the grammar hold the record's χ, the depth of its trace, how far its lives and its net compress, and its events by kind. The record is more predictable than every one of them (1.82 bits an event, against 1.88 to 2.22).

New York's Financial District, 2,441 events, χ = −2,210:
- **Closures.** A closure meets every cross-section of the blocks it closes and leaves χ as it was: κ = 1 − χ(G) for the graph of those blocks and their junctions, held by every closure of the record.
- **Which context tells.** Whether a regime is in force halves the code, against Melbourne's 7%, since New York's permits mostly hold one or two spaces and their rules recur.
- **Patterns and grown histories.** Its patterns are runs on a few spaces, the farmers' market at Coenties Slip among them. Its trace is deeper than every grown history's (725 against 355 to 591).

## 7. What Comes Next

- the selection rule at splits of properties (only six in the record);
- the posterior of unrecorded events drawn as vertices;
- New York's Open Streets as regimes of the interfaces;
- the gauge models of relational states;
- patterns used as the places a growth draws from, and a place's memory beyond its last event.
