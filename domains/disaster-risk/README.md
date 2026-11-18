# disaster-risk

*Status: draft (2026-11-18; [ADR 0129](../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Field, height 3, on urban risk management; C++ from the start.* What a natural hazard does to a city's systems and how its people are brought to safety: risk management → urban risk management → disaster risk, at the user's word. The hazard's intensity comes as maps (a flood's or a tsunami's depth at return periods); the domain reads them on the roads, weighs the scenarios, and moves people on foot towards shelters and safe ground.

## 1. Purpose

- **A hazard is states of a system.** A flood's depth along a road is the share of its speed the road keeps (for cars by Pregnolato et al.'s measured curve; on foot by a declaration); a map at a return period is a state of every link; the maps of a curve, weighted by trapezoids in the annual exceedance probability, are the scenarios over which urban risk management ranks links.
- **Evacuation, bounded and simulated.** Who can walk to safety within the warning time, with and without the shelters' capacities; the most that any routing could bring to safety by every instant (a flow over time, an upper bound); and the crowd as it moves, leaving when it does and walking to the nearest shelter with room (kinematic waves). The gaps between them are what departure and what crowding and routing cost.
- **Where to act.** Which shelters, among candidates, would house most within the warning time, chosen greedily with a bound that says how far from the best the choice can be.
- **Any place, any hazard mapped.** Freetown's flood maps and Japan's tsunami maps are cases of data; speeds, capacities, departures and warning times are declarations.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `disruption` | Depth–disruption curves (cars: Pregnolato et al. 2017, eq. 4, stopped from 0.3 m; on foot: declared, none from 0.5 m; any table); a local frame to longitude and latitude; the greatest depth along each link from a map (urban science's exposure grid); the state of the links; the scenarios of maps at return periods with the weights that integrate over the curve | draft |
| `evacuation` | The walking network (closed links left out, short ones merged); places and shelters joined to it; safe ground; reach within a warning time and the most housed within it under capacities; the bound by flows over time with departures; the crowd simulated on physis's kinematic waves, routed to the nearest shelter with room and rerouted as shelters fill, with its peak densities, crowded person-seconds, arrivals and frames; shelters chosen greedily with a bound | draft |
| `warning` | Early warning: the time of a slope's failure from its rate of displacement (Fukuzono 1985; Voight 1988), signs of a coming transition in a gauge's record on kinesis's criticality, each said to be research | planned |
| `vehicles` | Evacuation and response by car: assignment with congestion on the roads a hazard leaves open, contraflow | planned |

## 3. Dependencies

| Source | What disaster risk uses from it |
|---|---|
| urban-risk-management | `lifelines`: the contracted road network, attachments, times under a state |
| risk-management | `criticality`: scenarios, the weights of a curve (through urban risk management) |
| urban-science | `exposure`: a hazard's grid on longitude and latitude (through urban risk management) |
| morphe | `combinatorics`: Dijkstra's shortest paths, the greatest flow, flows over time (its decision 0019) |
| physis | `continuum`: kinematic waves on networks of links, Godunov's scheme (its decision 0016) |
| calx | `optimisation`: greedy submodular maximisation with a bound (its decision 0019) |

## 4. Used By

- **The API's `science/risk.hpp`**, for the plug-ins *Lifelines and evacuation* of the Urban Studio and the Urban Science Workbench: Freetown's roads under its floods, and Miyako's people walking from the tsunami's reach.

## 5. Open Questions

- **The hazard in time.** Maps are of the greatest depth; the arrival of water, and roads that close as it rises, are not yet in an evacuation.
- **People.** Everyone walks at one speed; the old and those who walk with difficulty, groups that wait for one another, and the choice of a shelter other than the nearest with room are open.
- **Vertical evacuation.** A shelter's height and the time to climb it (0.21 m/s on stairs in Japan's guidance) are not modelled.
- **The bound against the scheme.** The simulation is first order; a little of a crowd's front runs ahead of the free speed, so its curve may exceed the bound by a small share, which the checks measure.

## 6. References

- Pregnolato, M., Ford, A., Wilkinson, S. M. and Dawson, R. J. (2017), The impact of flooding on road transport: a depth-disruption function, *Transportation Research Part D* 55, 67–81.
- Ford, L. R. and Fulkerson, D. R. (1958), Constructing maximal dynamic flows from static flows, *Operations Research* 6, 419–433.
- Lighthill, M. J. and Whitham, G. B. (1955), On kinematic waves II, *Proc. R. Soc. A* 229, 317–345; Richards, P. I. (1956), Shock waves on the highway, *Operations Research* 4, 42–51.
- Daganzo, C. F. (1994), The cell transmission model, *Transportation Research Part B* 28, 269–287.
- Weidmann, U. (1993), *Transporttechnik der Fussgänger*, IVT, ETH Zürich.
- Fire and Disaster Management Agency (2002), manual for tsunami evacuation planning (walking at 1.0 m/s, 0.5 m/s for those who walk with difficulty), as quoted by MLIT's Chubu Regional Bureau.
- MLIT City Bureau (2013), guide to tsunami evacuation planning, 3rd ed. (mean walking speed observed in 2011: 2.24 km/h).
- Cabinet Office, FDMA and JMA (2011), interview survey of evacuation in the Great East Japan Earthquake (N = 870).
- Fujishige, S. (2005), *Submodular Functions and Optimization*, 2nd ed., Elsevier.
