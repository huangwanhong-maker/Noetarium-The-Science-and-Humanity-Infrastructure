# Lifelines and Evacuation

*Status: draft (2026-11-18; [ADR 0129](../../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Lens: Risk.* A place's risks read through its roads and its people, on the domains of risk management, urban risk management and disaster risk. The Urban Studio holds the same analyses as a project's steps (its plug-in of the same name).

## 1. Concepts

- **Which roads matter.** A city's road links are the components of a system whose service is reaching care; a flood map closes or slows each link by its deepest water (cars stop in 30 cm, Pregnolato et al. 2017); the maps at return periods are the scenarios, each standing for its band and nothing counted below the first. Each link's *risk* is what the floods cost through it (what protecting it would save), its *criticality* what losing it would add, in person-minutes added to one trip to the nearest building mapped as care by everyone, a person cut off counted at two hours, expected over the year's largest flood. How long a flood stands is not in the maps, so this is no total over a year.
- **Walking out.** The people within a hazard's zone walk to the designated refuges and the dry ground: who can get out within the warning walking freely; the most any routing could bring to safety by every minute (a bound, for everyone leaving at once and for the people's departures); and the crowd as it moves, routed to the nearest refuge with room. The gap between the bounds is what late departure costs, the gap from the second bound to the crowd what routing and crowding cost.
- **Refuges.** Among the tall buildings in view, which three would house most of those who cannot get out in time, chosen greedily with a bound on the best three.
- **Financing.** A city's modelled flood losses met by layers of a contingency budget, a contingent credit and insurance, against leaving everything to be found after the flood; all costs declared and illustrative.

## 2. Layers and Tools

| Layer or tool | Where | Shows |
|---|---|---|
| *which roads matter most to reaching care* | anywhere | Road links coloured and widened by risk or criticality, drawn on the ground; the buildings of care; the scenarios with what each closes and cuts off; where a place's flood maps are held (Freetown, Miyako), the floods close the roads, elsewhere a dry year alone |
| *walking out of the hazard's reach* | Miyako, Freetown | The zone on the ground; the crowd's density on the ways, played and following the clock from the moment of the warning; the designated refuges; those who cannot walk out in time; the curves of departure, the two bounds and the simulated crowd; the tall buildings chosen as refuges |
| *How the city might meet its flood losses* | Freetown | Three strategies costed a year and in the year of the 1-in-100-year flood, the cheapest tower, and a parametric cover refused for want of an index |

## 3. Data

- Freetown: the Freetown Flood Risk Assessment's river-flood maps at 20 to 1,500 years and its 1,943 modelled events (World Bank, JBA Risk Management, Freetown City Council; CC BY 4.0), by `data/products/freetown_flood.py`.
- Miyako, Iwate: the assumed tsunami (津波浸水想定) and the largest assumed river flood (洪水浸水想定区域, 想定最大規模) from the Hazard Map Portal's tiles (「ハザードマップポータルサイト」を加工して作成), and the designated emergency evacuation sites with the hazards each serves (出典：国土地理院ウェブサイト), by `data/products/japan_hazards.py`. A depth class is taken at its middle; a site's capacity is not published, so none is assumed.
- Ways and buildings: Overture Maps and OpenStreetMap, from the world's tiles. People: GHS-POP R2023A, shared among buildings by floor space, on cells of 50 m.

## 4. Declarations

Speeds by class of road; widths by class where a way gives none; walking at 1.0 m/s (the Fire and Disaster Management Agency's planning value), 0.62 m/s (MLIT's mean of 2011) or 0.5 m/s (for those who walk with difficulty); 1.3 people a second per metre of width and 5.4 per m² at a standstill; departures at once or by the shares of the 2011 survey of the Cabinet Office, FDMA and JMA (57% at once, 31% after doing something, 11% as the tsunami neared), with the times when each group left declared; a tall building's refuge its floors from the third up, a person a square metre; the costs of funds. Each is said on the page where it acts. What the page shows is an illustration under these declarations, not an official evacuation plan.

## 5. Engine

`engine/main.cpp`, a program around the API's `science/risk.hpp` (criticality, response, evacuation, financing), built by the work's `engine.py` into `data/cache/engines/urban/lifelines/`; maps are read from the data library's products.
