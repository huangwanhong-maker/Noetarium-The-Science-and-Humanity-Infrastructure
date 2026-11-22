# Public Transport and Who Can Reach It

*Status: draft (2026-11-22; [ADR 0151](../../../../development/decisions/0151-public-transport-and-sdg-11-2-1.md)).* A plug-in of the Urban Studio: who lives within reach of public transport, read under every declared reading of SDG 11.2.1's open terms, and every stop's service.

## 1. Concepts

- **Reach.** SDG 11.2.1 counts people within 500 m along the streets of a stop of a low-capacity system (a bus) or 1 km of a high-capacity one (rail, metro, ferry). UN-Habitat's metadata names no class for a tram, and treats frequency only as a secondary indicator, with an average wait at the peak of 20 minutes. Each reading of these open terms gives its own share. The plug-in reports them all and leaves the choice of reading to its user.
- **Service.** A stop's departures on a declared weekday, read at the morning peak:
  - departures an hour;
  - the longest wait;
  - the mean wait of a passenger arriving at random, taken beyond the window where the day goes on and said to be infinite where it does not.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Who lives within reach of public transport (SDG 11.2.1)* | The share within reach under each reading (trams of low or high capacity; every stop or only frequent ones; along the streets or in a straight line), with the stops counting for each class; every stop's service at the peak, placed; where people live and how far they walk, placed |

## 3. Templates

- *Melbourne: who lives within reach of public transport*: the centre within 2.5 km of the Town Hall, the feed read for a Wednesday.

## 4. Data

- **Stops and departures:** the data library's `transit_service.py`, from the Department of Transport and Planning's GTFS Schedule (CC BY 4.0). It is read for a declared weekday, its feeds of trains, trams and buses taken from the archive by range requests.
- **Ways and people:** the API's `science.risk.place` gathers Overture's ways and GHS-POP's people, shared among buildings by floor space.

## 5. Declarations

- **Readings.** The tram's class, the test of frequency, the distances and the measure of distance.
- **Inputs.** The weekday and the peak.
- **Walkers.** Motorways and trunk roads are closed to walkers. People reach the network at their nearest joinable point.

## 6. Engine

`engine/main.cpp`, around the API's `science/transit.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/transit/`. It is asked one request a process through `science.transit`.
