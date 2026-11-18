# Lifelines and Evacuation in Freetown and Miyako

*Status: draft (2026-11-18). Activity: research. [ADR 0129](../../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md).* The domains of risk management, urban risk management and disaster risk on two places: Freetown's roads under its river floods and its losses financed, and Miyako's people walking out of the assumed tsunami's reach. `run.py` makes `results.json` through the Urban Science Workbench's plug-in engine (the API's `science/risk.hpp`) on the data the page holds. Every result is an illustration under the declarations below, not an official evacuation plan or an assessment of either city's preparedness.

## 1. Data and Declarations

- **Freetown** (within 4.5 km of the centre): 9,942 ways and 221,188 buildings from Overture's tiles; 1.22 million people from GHS-POP R2023A shared among buildings by floor space; the 35 buildings mapped as care; the Freetown Flood Risk Assessment's river-flood maps at 20, 50, 100, 200, 500 and 1,500 years and its 1,943 modelled events over 10,000 years (World Bank, JBA Risk Management, Freetown City Council; CC BY 4.0).
- **Miyako, Iwate** (within 3.2 km of the bay-side centre): 3,775 ways, 15,978 buildings, 28,632 people; the assumed tsunami and the largest assumed river flood from the Hazard Map Portal's tiles (「ハザードマップポータルサイト」を加工して作成); the 52 designated emergency evacuation sites for tsunami (出典：国土地理院ウェブサイト), no capacity being published.
- **Declared:** speeds by class of road; cars stop in 30 cm of water (Pregnolato et al. 2017); a person cut off from care counted at two hours; each flood map standing for its band of return periods and nothing counted below the first; walking at 1.0, 0.62 or 0.5 m/s; 1.3 people a second per metre of width, 5.4 per m² at a standstill; departures by the shares of the 2011 survey of the Cabinet Office, FDMA and JMA (57% at once, 31% after doing something, 11% as the tsunami neared) with the times when each group left declared (from 2 to 10 minutes, to 25, to 40, the last 1% by 45), taken for Freetown too, where no survey exists; a warning of 30 minutes; a tall building's refuge its floors from the third up, a person a square metre; the costs of funds.

## 2. Freetown's Roads under Its Floods

- **The floods cut people off from care.** The 1-in-20-year river flood closes 213 of the 10,406 road links and slows 73; 163,179 people (13% of those within 4.5 km) can no longer reach any of the 35 buildings mapped as care, and the people's mean time to care, capped at two hours, rises from 4.9 to 20.4 minutes. At 1,500 years 644 links close and 298,075 people are cut off. People whose own street floods count as cut off.
- **Expected over the year's largest flood,** the floods add 1.03 million person-minutes to a trip to care by everyone (each map standing for its band of return periods, nothing below 20 years): a lower bound, since floods more frequent than 1 in 20 years are not mapped.
- **Few links carry the risk, as alternatives.** 451 links carry some risk. Protecting the single link of greatest risk, 423 m long, would remove 15% of the expected loss (its Fussell–Vesely importance); each of the next four would remove 14–15%. Such shares are not additive: these links are alternative ways out of the same flooded districts, and protecting any one of them reconnects them.
- **Single points of failure lie where no flood reaches.** The links of greatest criticality carry no risk. Losing the first, 136 m long, would multiply the expected loss of access by 2.5 (its RAW), because the people beyond it have no other way to care.

## 3. Freetown's People and Its River Flood

| Walking, leaving | Within 30 min, walking freely | Bound by 30 min | Simulated by 30 min |
|---|---|---|---|
| 1.0 m/s, at once | 49,725 (100%) | 49,725 (100%; everyone by 12 min) | 49,725 (99% by 10.5 min) |
| 1.0 m/s, as in 2011 | 49,725 (100%) | 45,806 (92%) | 44,090 (89%) |
| 0.5 m/s, as in 2011 | 49,575 (99.7%) | 44,972 (90%) | 42,163 (85%) |

- **Getting out is short.** 49,725 people live within the 1-in-100-year river flood's reach in the area. Leaving at once, the bound brings everyone out by 12 minutes; the simulated crowd has 99% out by 10.5 minutes and the last of them by 22. The river flood is narrow, and dry ground is a few hundred metres away.
- **So departure is everything.** Leaving as Miyako's people did in 2011, at most 92% can be out by 30 minutes: almost exactly the share who have left by then, since the walk itself takes minutes. The crowd reaches 89%. At 0.5 m/s the bound is 90% and the crowd 85%, though walking freely 99.7% could reach dry ground in time. Where safety is near, what a warning achieves is decided by how soon people leave, more even than in Miyako, where the walk is longer.
- **Refuges.** Only six buildings within the area are mapped at 12 m or more, and none would bring anyone to safety within 30 minutes at 0.5 m/s who could not already reach dry ground.

## 4. Miyako's People and the Assumed Tsunami

| Walking, leaving | Within 30 min, walking freely | Bound by 30 min | Simulated by 30 min |
|---|---|---|---|
| 1.0 m/s, at once | 18,391 (100%) | 18,391 (100%; everyone by 21 min) | 18,391 (99% by 21.5 min) |
| 1.0 m/s, as in 2011 | 18,391 (100%) | 15,693 (85%) | 14,650 (80%) |
| 0.62 m/s, as in 2011 | 17,813 (97%) | 13,956 (76%) | 11,869 (65%) |
| 0.5 m/s, as in 2011 | 15,566 (85%) | 12,653 (69%) | 10,840 (59%) |

- **Of the zone's 18,391 people,** everyone can walk to a refuge or to ground outside the zone within 30 minutes at 1.0 m/s, and leaving at once everyone could be out by 21 minutes; the simulated crowd matches it (99% by 21.5 minutes). Streets and refuges are not the bottleneck.
- **Departure is.** Leaving as people did in 2011, at most 85% can be safe by 30 minutes, however they are routed. The 15% shortfall is the people who leave late; it depends on the declared times as much as on the survey's shares. Routing to the nearest refuge and crowding cost a further 6% (the simulation's 80%).
- **At the speeds observed in 2011,** 0.62 m/s on average, the bound falls to 76% and the crowd reaches 65%. At 0.5 m/s, the planning speed for those who walk with difficulty, 15% of the zone cannot get out in time even walking freely.
- **Refuges.** Of the 15 buildings mapped at 12 m or more within the area, none would bring anyone to safety within 30 minutes at 0.5 m/s who could not already reach a refuge or dry ground. Building heights are often missing from the map, so the candidates are few.

The simulation never ran ahead of the bound at any reading, in any run.

## 5. Miyako's Roads under the Largest Assumed Flood

Under the largest assumed river flood (想定最大規模), 1,089 of 3,931 road links close and 20,673 of 28,632 people (72%) are cut off from the six buildings mapped as care. It is a single map of an extreme rainfall, not a return period, so no expectation is taken.

## 6. Freetown's Losses Financed

The 1,943 events cost $809k a year on average. The costs of funds are this study's declarations: what is left unfunded costs 1.2 a dollar up to $2 m and 2.0 beyond; a contingency budget's holding 3% of it a year; a contingent credit 0.5% of its limit a year and 1.1 a dollar drawn; insurance $50k a year and 1.8 times its expected payment.

| Strategy | Expected cost a year | Saved a year | Funded | The 1-in-100-year flood: unfunded | its year's cost |
|---|---|---|---|---|---|
| nothing arranged | $1.49 m | — | 0% | $13.5 m | $25.3 m |
| a contingency budget of $1 m | $1.45 m | $0.04 m | 10% | $12.5 m | $24.3 m |
| and a contingent credit to $10 m | $1.22 m | $0.27 m | 60% | $3.5 m | $16.3 m |
| and insurance from $10 m to $60 m | $1.24 m | $0.25 m | 94% | $0 | $11.5 m |

- **The trade-off of layering.** Insurance above the credit costs a little more on average ($17k a year) and cuts the cost of the 1-in-100-year flood's year from $16.3 m to $11.5 m, and of the 1-in-500-year flood's from $110 m to $12 m.
- **The cheapest tower that leaves nothing of the 1-in-100-year flood unfunded** is, on these costs, a contingent credit alone to $30.9 m, at $1.18 m a year (100 of 119 towers tried met the cap). Holding money at 3% a year is dearer than borrowing it when needed at these terms; a higher fee or rate would change the answer. That is a finding about the declared costs, not about Freetown.
- **A parametric cover** cannot be priced: the catalogue gives its events' losses and no index, and none is invented.

## 7. Limits

- Ways, buildings and the buildings mapped as care are what Overture and OpenStreetMap hold; a hospital mapped as a plain building is missed.
- People are GHS-POP's, placed by floor space; nobody's position at the hour of a flood or a tsunami is known.
- The hazard is its greatest depth, not its arrival in time; the bound and the simulation take the water as standing from the start (Freetown) or not yet arrived (Miyako).
- Everyone walks at one speed; cars, groups and stairs are not modelled.
