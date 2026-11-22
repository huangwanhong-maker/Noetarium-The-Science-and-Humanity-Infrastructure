# Homes and Who Lacks Them

*Status: draft (2026-11-22; [ADR 0155](../../../../development/decisions/0155-shelter-and-sdg-11-1-1.md)).* A plug-in of the Urban Studio: who lives in slum or informal households and in inadequate housing, read from a household survey under every declared reading of SDG 11.1.1's open terms.

## 1. Concepts

- **Slum or informal households.** A household lacking one or more of five things: an improved water source, improved sanitation, sufficient living area, a durable dwelling and secure tenure (UN-Habitat's metadata, after the expert group of 2002). The share is of people.
- **Inadequate housing.** A household whose housing costs more than 30% of its income.
- **Readings.** Each term is open, and each reading gives its own share:
  - tenure left out, as four components were long reported, or secure with a document and no fear of eviction;
  - the JMP's ladder, where a shared toilet is not improved and delivered water is;
  - two people to a room instead of three;
  - walls and roof without the floor;
  - a hazardous site counted against durability.
- **Missing answers** bound each share between counting them as not deprived and as deprived. The value reported between leaves them out, or takes either bound, as declared.
- **Intervals** come from the survey's weights, strata and clusters (peira's ratio estimator with the ultimate-cluster variance), and a district's share is estimated as a domain of the whole survey.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Homes and who lacks them (SDG 11.1.1)* | The share of people in slum or informal households under six readings, with intervals and bounds, inadequate housing and either; under the metadata's five, what households lack (each component, and it alone), the share by district, and people by the number of things lacked |

## 3. Templates

- *A city's homes and who lacks them (invented)*: 1,200 households in sixty enumeration areas of four districts (an old town, a riverside settlement on a flood plain, new estates, a hillside), some answers missing. Every household and number is invented to show the tool.

## 4. Data

A survey is a table of the project, one household a row, with the columns `weight`, `stratum`, `cluster`, `district`, `persons`, `rooms`, `water`, `sanitation`, `shared_toilet`, `walls`, `roof`, `floor`, `hazardous_site`, `documented`, `fears_eviction`, `housing_cost` and `income`. An empty cell is a missing answer. The codes the readings call improved or durable are declared in `science.shelter` (`WATER_EGM`, `WATER_JMP`, `SANITATION`, `DURABLE`); a survey with other codes declares its own readings.

## 5. Engine

`engine/main.cpp`, around the API's `science/shelter.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/shelter/`. It is asked one request a process through `science.shelter`.
