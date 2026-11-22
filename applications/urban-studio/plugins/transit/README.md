# Public Transport, Open Space and Who Can Reach Them

*Status: draft (2026-11-22; [ADR 0151](../../../../development/decisions/0151-public-transport-and-sdg-11-2-1.md), [ADR 0154](../../../../development/decisions/0154-open-space-and-sdg-11-7-1.md)).* A plug-in of the Urban Studio: who lives within reach of public transport and of open space for public use, each read under every declared reading of its indicator's open terms (SDG 11.2.1, SDG 11.7.1), and every stop's service.

## 1. Concepts

- **Reach.** SDG 11.2.1 counts people within 500 m along the streets of a stop of a low-capacity system (a bus) or 1 km of a high-capacity one (rail, metro, ferry). UN-Habitat's metadata names no class for a tram, and treats frequency only as a secondary indicator, with an average wait at the peak of 20 minutes. Each reading of these open terms gives its own share. The plug-in reports them all and leaves the choice of reading to its user.
- **Service.** A stop's departures on a declared weekday, read at the morning peak:
  - departures an hour;
  - the longest wait;
  - the mean wait of a passenger arriving at random, taken beyond the window where the day goes on and said to be infinite where it does not.
- **Open space.** SDG 11.7.1 is the share of a city's built-up area that is open space for public use, free of charge, and the share of people within 400 m of it along the streets. Which land counts is a reading: parks and plazas; pedestrian areas and sports grounds as well, often open to the public; every green and open land, gardens and cemeteries among them. WHO Europe's rule of thumb asks a park of at least 0.5 ha within 300 m in a straight line. The open land is Overture's land use and land cover, read as kinds by a declared table (`science.transit.SPACE_KINDS`); their union is counted once where layers overlap.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Open space and who can reach it (SDG 11.7.1)* | Under each reading: the share of people within reach, the share of the square that is open space, the spaces counted; each kind of land alone; where people live and how far they walk to a park or a plaza, placed; the open land by the kind it was read as, drawn |
| *Who lives within reach of public transport (SDG 11.2.1)* | The share within reach under each reading (trams of low or high capacity; every stop or only frequent ones; along the streets or in a straight line), with the stops counting for each class; every stop's service at the peak, placed; where people live and how far they walk, placed |

## 3. Templates

- *Melbourne: who lives within reach of public transport and of open space*: the centre within 2.5 km of the Town Hall, the feed read for a Wednesday, and the open land in the square of 5 km about it.

## 4. Data

- **Stops and departures:** the data library's `transit_service.py`, from the Department of Transport and Planning's GTFS Schedule (CC BY 4.0). It is read for a declared weekday, its feeds of trains, trams and buses taken from the archive by range requests.
- **Ways and people:** the API's `science.risk.place` gathers Overture's ways and GHS-POP's people, shared among buildings by floor space.
- **Open land:** the data library's `world_city.open_spaces`, Overture's base theme (land use and land cover) at its finest level, each polygon with its layer, subtype and class; a polygon cut by a tile's edge comes in its parts.

## 5. Declarations

- **Readings.** The tram's class, the test of frequency, the distances and the measure of distance; the kinds of land that count as open space, the least area, and the analysis area (a square about the centre, ways and land gathered half a kilometre beyond it).
- **Inputs.** The weekday and the peak.
- **Walkers.** Motorways and trunk roads are closed to walkers. People reach the network at their nearest joinable point.

## 6. Engine

`engine/main.cpp`, around the API's `science/transit.hpp` and `science/openspace.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/transit/`. It is asked one request a process through `science.transit`.
