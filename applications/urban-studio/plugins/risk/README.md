# Lifelines and Evacuation

*Status: draft (2026-11-18; [ADR 0129](../../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)).* The Urban Studio's ninth plug-in: a place's risks read through its roads, its people and its money, as a project whose steps are kept, rerun and placed on the map. The same composition answers the Urban Science Workbench's plug-in of the same name (the API's `science/risk.hpp`); the studio holds it as analyses with their parameters, tables and notes.

## 1. Concepts

- **Which roads matter.** Road links are the components of a system whose service is reaching care. A flood map closes or slows each by its deepest water (cars stop in 30 cm, Pregnolato et al. 2017); maps at return periods are the scenarios, each standing for its band and nothing counted below the first. A link's *risk* is what the floods cost through it (what protecting it would save), its *criticality* what losing it would add: person-minutes added to one trip to the nearest building mapped as care by everyone, expected over the year's largest flood.
- **Who stays within reach.** Coverage by stations within a time in each flood, who is covered twice, what each station alone covers, and where another would reach most over the floods, chosen greedily with a bound on the best.
- **Walking out.** The people within a hazard's zone walk to the designated refuges and the dry ground: who can get out within the warning walking freely; the most any routing could bring to safety by every minute, for everyone leaving at once and with the people's departures (a bound); and the crowd simulated, routed to the nearest refuge with room. The gaps say what departure costs and what routing and crowding cost.
- **Who pays.** Modelled flood events met by layers of a contingency budget, a contingent credit and insurance, each costed a year and in the year of a rare flood, the cheapest tower, a parametric cover refused where no index is given.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Which roads matter most to reaching care* | The road links with any risk or criticality, placed at their middles and coloured by risk, sized by criticality; each flood's links closed and people cut off; a note with the expected loss, the link whose protection would save most and the most critical link |
| *Who stays within reach of a station* | Coverage by the buildings mapped as care (or fire, or police) within a time in each flood, covered twice, each station's reach and sole reach placed; the stations to add among buildings of public use |
| *Walking out of the hazard's reach* | The curves of departure, the two bounds and the simulated crowd; where people cannot walk out in time, placed; the designated refuges; the tall buildings chosen as refuges |
| *How the city might meet its flood losses* | Strategies costed a year and in the years of the 1-in-100 and 1-in-500-year floods; the cheapest tower; the refusal of a parametric cover |

## 3. Templates

- *Freetown: roads, floods and losses*: the four tools on Freetown's centre and east within 4.5 km, its river floods at six return periods and its 1,943 modelled events; the 1-in-100-year flood's zone walked out of all at once and leaving as in 2011.
- *Miyako: walking out of the tsunami's reach*: the zone of the assumed tsunami walked out of at 1.0 m/s (as in 2011, and at once) and at 0.62 m/s, with tall buildings as refuges; the roads to care under the largest assumed flood.

## 4. Data

Freetown's maps and events (World Bank, JBA Risk Management, Freetown City Council; CC BY 4.0) by `data/products/freetown_flood.py`; Miyako's maps (「ハザードマップポータルサイト」を加工して作成) and designated evacuation sites (出典：国土地理院ウェブサイト) by `data/products/japan_hazards.py`; ways and buildings from Overture's tiles and people from GHS-POP, gathered by the API's `science.risk.place`.

## 5. Declarations

Speeds by class of road; walking at the speed a step states (1.0 m/s, the FDMA's planning value; 0.62 m/s, MLIT's mean of 2011; 0.5 m/s, for those who walk with difficulty); 1.3 people a second per metre of width, 5.4 per m² at a standstill; departures by the shares of the 2011 survey of the Cabinet Office, FDMA and JMA with the times declared; a tall building's refuge its floors from the third up, a person a square metre; the costs of funds, as the financing tool's parameters.

## 6. Engine

`engine/main.cpp`, a program around the API's `science/risk.hpp`, built by the work's `engine.py` into `data/cache/engines/urban-studio/risk/` and asked by `science.risk`.
