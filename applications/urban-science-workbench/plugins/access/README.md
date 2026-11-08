# Reach and use

*How far one can go from any place, who can reach what they need, and how a city's walkers use its places.* Status: **draft**. Activity: research. [ADR 0078](../../../../development/decisions/0078-the-whole-earth-in-the-urban-workbench.md), [ADR 0086](../../../../development/decisions/0086-access-to-services-and-unmet-need.md).

## 1. Layers

- **How far in so many minutes** (anywhere). Pick a place on the map: the minutes to every way about it run down the streets as a wave, on foot or by road, with the length of way reached within each time. On the ways of the tiles in view, so that it ends where they end.
- **Access to services, and who is left without** (anywhere). Pick a place: its people on GHS-POP's cells of 30 seconds of arc (about 1 km), the buildings mapped as care, schools or shops with their floor space (footprint × storeys) as what they offer, and the ways at a mode's speeds. Each cell is coloured by its access by the two-step floating catchment with a Gaussian decay: floor space shared among all the people who can reach it, so that a crowded clinic counts for less. The cells are draped on the ground as GHS-POP's own grid, so that they follow the hills, and the cells with nothing within the limit are hatched. The layer shares the ground with the other rasters (one at a time). The card gives the share of people within reach, the people with nothing, those below half the mean, the person-weighted Gini coefficient of access, and the Rural Access Index where GHS-SMOD calls the cells rural.
- **Where the walkers go, against their habit** (Melbourne). The Urban Space Usage Index on H3 cells: each cell's share of the day's walkers at the counters, and its departure from the same weekday over the twelve weeks before. It follows the clock's date. A date in a gap of the record takes the nearest before it.

## 2. Engine

`engine/main.cpp`, on urban-science's `access` and `usage`, nomos's accessibility, morphe's shortest paths and physis's H3: `reach`, `services`, `usage`. The server's `/world/population?lat=&lon=&km=` reads GHS-POP's and GHS-SMOD's cells about a point (`data/acquisition/services.py`).

## 3. Limits

- Speeds are declared in `layers/reach.js`, and are assumptions: walking at 1.34 m/s (Weidmann 1993), a road's free flow by its class. No signals, gradients, one-way streets or traffic: a time is a lower bound on the journey.
- Ways that end within 8 m of another point are joined, since a tile cuts a way at its edge. A dead end that stops that near another way is joined too.
- A service is what Overture and OpenStreetMap say a building is. A hospital mapped as a plain building is missed, and its floor space is not its capacity: the measures are of what is mapped, and say so.
- A cell of people joins the network at the point nearest its centre, a building at the point nearest its centroid, with the metres between walked. Only points of a connected part holding at least 1% of the network's points may be joined: a way closed to the mode (a footway to a car) leaves stubs stitched at their ends, islands from which nothing is reached. People are spread evenly within a cell for none of the measures: a cell is one place.
- Only the cells within the world's tiles whose ways have come are measured. The people of the others are counted as left out, never as without.
- The Rural Access Index takes motorways to tertiary roads as all-season, as no surface or season is mapped for most roads, and measures the distance from a cell's centre in a straight line.
- The counters are a sample of the centre's footpaths and not of the city, and the shares are of those reporting.
