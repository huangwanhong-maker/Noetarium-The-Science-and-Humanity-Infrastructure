# 0155. Homes and Who Lacks Them: SDG 11.1.1 under Its Readings

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue ("continuer, merci"). The survey *Evidence, methods and actions for steering cities* ranks "urban science: households in slums and inadequate housing (SDG 11.1.1), the union rule over declared readings, with population weights" first (its §4.2, asked by its chapter B).

## 1. Context

Urban science reads who can reach public transport (ADR 0151) and open space (ADR 0154) under the readings of their indicators. SDG 11.1.1, the share of urban people in slums, informal settlements or inadequate housing, was missing. UN-Habitat's metadata (last updated 2021-12-20) defines a slum household by the union of five deprivations and inadequate housing by a cost above 30% of income, both counted as people, from household surveys with their designs.

Each deprivation has readings: which sources and toilets are improved (the expert group's lists, or the JMP's ladder), how many people to a room (the expert group considered two to four), which parts of a dwelling must be durable and whether the site counts, what makes tenure secure, and whether tenure is counted at all, since many surveys never asked it. Answers are missing in every survey.

## 2. Decision

1. **Urban science's `shelter`** (theory note `domains/urban-science/theory/shelter.md`, 19 checks):
   - **a household's status** under a reading, component by component: deprived, not deprived, unknown or uncounted, a missing answer decided where the other answers decide it;
   - **the union rule**, with Proposition 1: a wider reading lowers neither bound;
   - **shares of people** by peira's ratio estimator with the ultimate-cluster variance, a district as a domain of the whole survey, and bounds over missing answers (partial identification);
   - **what else**: each component's share, people deprived of one alone, people by number of deprivations, inadequate housing and either.
2. **The API's `science/shelter.hpp`** (`shelter.assess`) and `science.shelter`, with six declared readings and the codes they call improved or durable (4 checks).
3. **A plug-in of the Urban Studio, *Homes and who lacks them*,** with an engine of its own, the tool *Homes and who lacks them (SDG 11.1.1)* and a template on an invented city's survey.

## 3. Alternatives Considered

- **One reading, the metadata's five.** Tenure is often not asked, and the threshold of crowding and the treatment of shared toilets each move the share by several points. Every declared reading is reported.
- **Imputing missing answers.** Imputation needs a model of why answers are missing. Bounds hold whatever the reason; complete cases are offered as the value between, and said to assume answers missing at random. peira's multiple imputation is there for a reading that wants it.
- **Districts as surveys of their own.** A district's households are a domain of the survey, not a survey: its variance is taken from the whole design, with the other households' contributions set to zero.
- **A module of social provision.** The survey places the indicator in urban science beside 11.2.1 and 11.7.1; the household's dwelling and site are urban objects. The module stands on peira alone, and social provision may use it through the API.

## 4. Consequences

- **What the invented city shows** (1,200 households in sixty areas of four districts, 110,000 households in all, every number invented):
  - between 44.2% (four components) and 64.2% (two to a room) of people live in slum or informal households; under the metadata's five, 55.9% (52.8% to 59.1%), bounded by missing answers between 49.9% and 60.7%;
  - insecure tenure is what is lacked most (32.5% of people), and alone puts 8.9% in the count;
  - the new estates 13.1% (8.3% to 17.9%) and the riverside 98.2%; missing answers widen the new estates' bounds to 10.4% and 30.9%, since where deprivation is rare a missing answer weighs most;
  - 28.8% of people live in housing that costs more than 30% of their income, and 70.4% in one or the other.
- **Not built:** the urban population's delineation; accessibility and cultural adequacy; homelessness; small areas by Fay and Herriot; area-level indicators from imagery; a reader of DHS or MICS recodes.
- **Provisional:** the readings and their codes; the plug-in's name and colour.
