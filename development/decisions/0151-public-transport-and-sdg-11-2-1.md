# 0151. Public Transport and Who Can Reach It: SDG 11.2.1 under Its Readings

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue autonomously. The survey *Evidence, methods and actions for steering cities* ranks "urban science: frequency-aware access to public transport (SDG 11.2.1), network service areas with frequency, on a GTFS product" first (its §4.2, asked by its chapter B), with a GTFS reader first among the data library's needs (its §4.3). Its §B.3.4 names the indicator's open terms.

## 1. Context

Urban science's `access` computes who can reach what along a place's ways (ADR 0086), and the data library reads places and people anywhere. Public transport was missing: the infrastructure read no timetable and no stop's service, and computed no SDG 11.2.1. The indicator has open terms. A tram is in neither of the metadata's lists; frequency is only a secondary indicator; distances run along the network, though a straight line is often used.

## 2. Decision

1. **The data library's `formats/gtfs.py`** reads GTFS Schedule by the standard library:
   - the services on a date, from the calendar and its exceptions;
   - stops, routes and their modes by the basic and extended route types;
   - each stop's departures in a window, streamed.

   **Its `products/transit_service.py`** reads a place's feed for a declared weekday into stops with their departures by mode. The first case is Melbourne, from the Department of Transport and Planning's GTFS Schedule (CC BY 4.0): trains, trams and buses read from the archive by range requests.
2. **Urban science's `transit`** (theory note `domains/urban-science/theory/transit.md`, 12 checks):
   - **service at a stop:** departures an hour, the longest wait, and the exact mean wait of a random arrival (Proposition 1), taken beyond the window where the day goes on;
   - **SDG 11.2.1 under a reading:** classes of mode, a test of frequency, the distances, and the network or a straight line, the latter never reaching fewer (Proposition 2);
   - **the network:** the walking network densified, so that places join it near where they are.
3. **The API's `science/transit.hpp`** (`transit.access`) and `science.transit`: stops in a place's frame, its walking ways and people from `science.risk.place`.
4. **A plug-in of the Urban Studio, *Public transport and who can reach it*,** with an engine of its own, the tool *access* and a template on Melbourne's centre.

## 3. Alternatives Considered

- **One reading, the metadata's.** The metadata leaves the tram and frequency open, and each choice moves the share in a city of trams by six points. Every declared reading is reported.
- **Frequency as departures an hour.** The mean wait of a random arrival is what the secondary indicator speaks of. It also reflects bunched timetables, which a rate hides.
- **Hosting the tool in an existing plug-in.** None of the Urban Studio's plug-ins is about access to services, and the risk plug-in's engine, which holds urban science's network, is about hazards. A plug-in of its own leaves room for SDG 11.7.1, open public space within 400 m, which the survey ranks next.
- **The GTFS feed parsed in C++.** Parsing a file format is the data library's, and the domain takes departures, not files (CLAUDE.md §3.3).

## 4. Consequences

- **Checks:**
  - urban science's `transit`: 12 (urban science 480);
  - the API's check of transit: 5;
  - the Urban Studio: one more.
- **What the checks showed:**
  - Proposition 1 against the mean over 200,000 arrivals to 10⁻⁴;
  - a corner that puts a place out of reach along the streets and within it in a straight line;
  - a bus every hour dropped by the test of 20 minutes;
  - a tram 900 m away counting only when read as of high capacity.
- **What Melbourne shows** (the feed's Wednesday, 2 December 2026; 271,453 people within 2.5 km of the Town Hall; 1,343 stops):
  - 92.0% live within reach with trams of low capacity, 91.5% if only frequent stops count, and 98.6% with trams of high capacity;
  - in a straight line, 100%;
  - the median stop's mean wait at the morning peak is 4.7 minutes, so the frequency test removes little in a centre.
- **Not built:**
  - people by sex, age and disability;
  - the Degree of Urbanisation's delineation;
  - informal transport;
  - affordability;
  - other cities' feeds as cases;
  - waits weighted by passengers;
  - SDG 11.7.1.
- **Provisional:** the module's, the plug-in's and the tool's names, the plug-in's colour, the declared weekday and peak.
