# urban-risk-management

*Status: draft (2026-11-18; [ADR 0129](../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Bridge, height 2, over risk management and urban science; C++ from the start.* The risks of a city's systems: which of its roads matter most to reaching a hospital, who stays within reach of a fire station, how many separate ways lead out of a district. Disaster risk stands on it with the states a flood or a tsunami puts a city in.

## 1. Purpose

- **A city's networks are systems whose service is reach.** People need to reach a hospital, a fire engine needs to reach them, everyone needs to reach a shelter. The domain reads a city's roads as the links of such a system, joins people and services to them, and states the loss of a state of the links as the person-minutes it adds, a person cut off counted at a declared cap.
- **Criticality without a search per link.** Which links matter most is risk management's criticality: each link closed in every scenario. A link off every shortest path changes nothing, and closing one on the forest of shortest paths changes only what lies beneath it, so the whole ranking costs the sum of the subtrees.
- **Redundancy and coverage.** How many links must fail to cut a district off from every service (link-disjoint paths and the least cut), who is covered once and twice by stations within a time, and where another station would cover most over a hazard's scenarios.
- **Any city.** The roads are any polylines with speeds and widths (a city's tiles, a survey, a plan); thresholds, caps and speeds are declarations.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `lifelines` | Roads joined as urban science's `access` joins them (snapping, stitching loose ends to points and segments) and contracted to links between junctions, each with its length, free time, least width and road; people and services joined across a gap; times to the nearest service under a state of the links (each keeping a share of its speed, 0 closed); the loss in person-minutes with the people beyond a threshold and those cut off; every link's closure at a state answered at once (Proposition 2.2); criticality and risk through risk management; redundancy by link-disjoint paths and the least cut | draft |
| `response` | Coverage by stations within a time at a state of the roads; backup coverage; what each station reaches and alone reaches; where to add k stations over a hazard's scenarios (maximal covering, greedily with calx's bound) | draft |
| `water` | Water networks under a hazard: civil engineering's criticality of pipes composed with the states a hazard puts them in | planned |
| `services` | Hospitals, schools and shelters as systems of their own: capacity, staff, the demand an event brings | planned |

## 3. Dependencies

| Source | What urban risk management uses from it |
|---|---|
| risk-management | `criticality`: importance measures over scenarios |
| urban-science | `access`: a network of ways, snapping and stitching, the parts a place may join |
| morphe | `combinatorics`: Dijkstra's shortest paths with offsets (its decision 0011), the greatest flow and its least cut (its decision 0019) |
| calx | `optimisation`: greedy submodular maximisation with a bound (its decision 0019); verification |
| nomos, physis, tyche | through urban science and risk management |

## 4. Used By

- **disaster-risk**: roads closed or slowed by a flood's depth, the scenarios of its return periods, evacuation over the walking network.
- **The API's `science/risk.hpp`**, for the plug-ins *Lifelines and evacuation* of the Urban Studio (a project's steps) and of the Urban Science Workbench (on the map).

## 5. Open Questions

- **Places on links.** A place joins the nearest point of the network, not the nearest point of a segment; on a closed link it reaches nothing, though a person could walk to its open end.
- **Turns and junctions.** Junction delays, turn bans and one-way streets are not in the links.
- **Trips other than to the nearest service.** Origin–destination demand (the journeys to work urban science's `luti` holds) as a loss of a state is open.

## 6. References

- Hallegatte, S., Rentschler, J. and Rozenberg, J. (2019), *Lifelines: The Resilient Infrastructure Opportunity*, World Bank.
- Church, R. and ReVelle, C. (1974), The maximal covering location problem, *Papers of the Regional Science Association* 32, 101–118.
- Hogan, K. and ReVelle, C. (1986), Concepts and applications of backup coverage, *Management Science* 32, 1434–1444.
- Menger, K. (1927), Zur allgemeinen Kurventheorie, *Fundamenta Mathematicae* 10, 96–115.
