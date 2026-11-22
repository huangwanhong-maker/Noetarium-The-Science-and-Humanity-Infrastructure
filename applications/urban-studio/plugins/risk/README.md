# Lifelines and Evacuation

*Status: draft (2026-11-18; [ADR 0129](../../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)).* The Urban Studio's ninth plug-in: a place's risks read through its roads, its people and its money, as a project whose steps are kept, rerun and placed on the map. The same composition answers the Urban Science Workbench's plug-in of the same name (the API's `science/risk.hpp`); the studio holds it as analyses with their parameters, tables and notes.

## 1. Concepts

- **Which roads matter.** Road links are the components of a system whose service is reaching care. A flood map closes or slows each by its deepest water (cars stop in 30 cm, Pregnolato et al. 2017); maps at return periods are the scenarios, each standing for its band and nothing counted below the first. A link's *risk* is what the floods cost through it (what protecting it would save), its *criticality* what losing it would add: person-minutes added to one trip to the nearest building mapped as care by everyone, expected over the year's largest flood.
- **Who stays within reach.** Coverage by stations within a time in each flood, who is covered twice, what each station alone covers, and where another would reach most over the floods, chosen greedily with a bound on the best.
- **Walking out.** The people within a hazard's zone walk to the designated refuges and the dry ground: who can get out within the warning walking freely; the most any routing could bring to safety by every minute, for everyone leaving at once and with the people's departures (a bound); and the crowd simulated, routed to the nearest refuge with room. The gaps say what departure costs and what routing and crowding cost.
- **What the floods cost the buildings, under every damage function.** The place's buildings, valued as its flood assessment valued them, meet each map at the depth it gives them; the JRC's global depth–damage curves for the regions declared to resemble the place are each a model applied to every building of a class at once, and the loss is their range, with the wider envelope beside it (ADR 0149).
- **A fragility learnt from an event.** A survey's counts by damage state and intensity fit each state's fragility, and update one held before the event; what the earlier one predicted is set against what was found (ADR 0149).
- **What the losses cost households.** Assets, income and transfers lost, reconstruction paid from consumption, savings and support capping the deepest part, valued by utility as national consumption, after *Unbreakable*'s two working papers; measures judged by the asset losses and by the well-being losses they avoid, which rank them differently (ADR 0150).
- **Who pays.** Modelled flood events met by layers of a contingency budget, a contingent credit and insurance, each costed a year and in the year of a rare flood, the cheapest tower, a parametric cover refused where no index is given.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Which roads matter most to reaching care* | The road links with any risk or criticality, placed at their middles and coloured by risk, sized by criticality; each flood's links closed and people cut off; a note with the expected loss, the link whose protection would save most and the most critical link |
| *Who stays within reach of a station* | Coverage by the buildings mapped as care (or fire, or police) within a time in each flood, covered twice, each station's reach and sole reach placed; the stations to add among buildings of public use |
| *Walking out of the hazard's reach* | The curves of departure, the two bounds and the simulated crowd; where people cannot walk out in time, placed; the designated refuges; the tall buildings chosen as refuges; and, as evidence for the project's ledger (ADR 0131), the share safe by the warning (between the simulated crowd and the bound) and the share who could reach safety walking freely, with the readings they were computed under |
| *What the floods would cost the buildings, under every damage function* | Each model's average annual loss, by class and at each map's return period; the losses gathered on cells, least and greatest over the models, placed; a note with the range, the envelope and the assessment's own figure beside it, said not to be like for like |
| *A fragility learnt from the damage an event left* | Each state's fragility held before, fitted (with standard errors) and updated (with whether its posterior sits at its grid's edge); the curves by intensity with the update's band; what the earlier fragility predicted against what the survey found |
| *What the losses cost households in well-being* | Each district's losses of assets and of well-being a year, their shares and resilience, and the persons pushed below a poverty line; each income group's shares; each measure's avoided losses of both kinds and its rank by each |
| *How the city might meet its flood losses* | Strategies costed a year and in the years of the 1-in-100 and 1-in-500-year floods; the cheapest tower; the refusal of a parametric cover |

## 3. Templates

- *Freetown: roads, floods and losses*: the tools on Freetown's centre and east within 4.5 km, its river floods at six return periods and its 1,943 modelled events; the 1-in-100-year flood's zone walked out of all at once and leaving as in 2011; its 153,435 buildings priced under nine models of the JRC's library.
- *A fragility learnt from an event's damage (invented)*: 500 houses in five states at five intensities, and a curve held before.
- *Households' well-being in a city's floods (invented)*: 100,000 people in three districts and five income groups, and six measures.
- *Miyako: walking out of the tsunami's reach*: the zone of the assumed tsunami walked out of at 1.0 m/s (as in 2011, and at once) and at 0.62 m/s, with tall buildings as refuges; the roads to care under the largest assumed flood.

## 4. Data

Freetown's maps, valued buildings and events (World Bank, JBA Risk Management, Freetown City Council; CC BY 4.0) by `data/products/freetown_flood.py`; the JRC's global flood depth–damage functions (Huizinga, de Moel and Szewczyk 2017) by `data/products/damage_functions.py`; Miyako's maps (「ハザードマップポータルサイト」を加工して作成) and designated evacuation sites (出典：国土地理院ウェブサイト) by `data/products/japan_hazards.py`; ways and buildings from Overture's tiles and people from GHS-POP, gathered by the API's `science.risk.place`.

## 5. Declarations

Speeds by class of road; walking at the speed a step states (1.0 m/s, the FDMA's planning value; 0.62 m/s, MLIT's mean of 2011; 0.5 m/s, for those who walk with difficulty); 1.3 people a second per metre of width, 5.4 per m² at a standstill; departures by the shares of the 2011 survey of the Cabinet Office, FDMA and JMA with the times declared; a tall building's refuge its floors from the third up, a person a square metre; the costs of funds, as the financing tool's parameters.

## 6. Engine

`engine/main.cpp`, a program around the API's `science/risk.hpp`, built by the work's `engine.py` into `data/cache/engines/urban-studio/risk/` and asked by `science.risk`.
