# civil-engineering

*Status: draft (2026-11-13; [ADR 0107](../../development/decisions/0107-civil-engineering-founded-water-networks.md)). Field, height 0, on the foundations alone; C++ from the start.* Structures, hydraulics and transport as engineered systems: how the works that provide water, carry traffic and stand under load behave, and how well they serve. Founded with its first module, water distribution networks, at the user's request to implement the handbook *Material and Ecological Conditions of Life*, whose §6.1 names EPANET and WNTR. Its theory note is [water-networks.md](theory/water-networks.md).

## 1. Purpose

- **Provision is not presence.** A network may hold water and still leave a junction without pressure. The domain computes what each junction receives, at what pressure, and how old the water is, and what is lost when a pipe fails.
- **One method for every network of conduits.** The hydraulics are physis's networks of conduits (its decision 0029), Todini and Pilati's method as EPANET uses it. What names water is here: Hazen–Williams' and Darcy–Weisbach's laws, pumps' curves, demands and their patterns.
- **Held to EPANET.** EPANET's input files are read, and EPANET's own solution of its example network is reproduced to a millimetre of head.
- **Presumptions declared.** The required and minimum pressures of pressure-driven demand, the age of the water a tank discharges, and where the simulation over time departs from EPANET's are each said.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `water` | EPANET's input files read and converted to SI; heads, pressures and flows at an instant (Hazen–Williams with EPANET's coefficient, Darcy–Weisbach with Swamee and Jain's friction factor, minor losses, pumps' curves fitted as EPANET fits them, check valves, closed links); demand-driven and pressure-driven demands (Wagner et al. 1988); isolated junctions found; a period simulated with patterns, tanks and simple controls; the age of the water in a steady state; Todini's resilience index; the criticality of each link | draft |
| `structures` | Frames and trusses by the stiffness method; loads and their combinations | planned |
| `transport` | Traffic on networks: assignment, signal timing, capacity | planned (urban science's `luti` holds equilibrium assignment) |
| `drainage` | Stormwater and sewers: SWMM's kinematic and dynamic waves on networks of conduits and channels | planned |

## 3. Dependencies

| Source | What civil engineering uses from it |
|---|---|
| physis | `discretisation`: networks of conduits with monotone laws (decision 0029), and Kirchhoff networks |
| calx | `linalg` (through physis), verification |
| morphe | declared; not yet used |

## 4. Used By

- **The API's `science/water.hpp`** composes it for any work's engine, and the Urban Studio's plug-in *Materials and flows* runs it on any EPANET file.
- **energy-systems** and **social provision**, planned to stand on it: the networks that deliver water and power as services.

## 5. Open Questions

- **EPANET's timing.** The simulation over a period moves at the hydraulic step; EPANET also stops at the instant a tank fills or empties or a control's condition is met. A tank or control that changes within a step changes later here.
- **Quality over time.** Age is found in a steady state; chlorine decay and transport over time (EPANET's Lagrangian scheme) are open.
- **Valves.** Pressure-reducing, pressure-sustaining and flow-control valves are read by EPANET and not yet here.
- **Pressure-driven demand's law.** Wagner's law is steepened beyond its ends, so delivery stays within 10⁻⁴ of it; WNTR smooths it differently.

## 6. References

- Rossman, L. A. et al. (2020), *EPANET 2.2 User Manual*, EPA/600/R-20/133.
- Todini, E. and Pilati, S. (1988), A gradient algorithm for the analysis of pipe networks, in *Computer Applications in Water Supply* 1, 1–20.
- Todini, E. (2000), Looped water distribution networks design using a resilience index based heuristic approach, *Urban Water* 2, 115–122.
- Wagner, J. M., Shamir, U. and Marks, D. H. (1988), Water distribution reliability: simulation methods, *J. Water Resour. Plan. Manage.* 114, 276–294.
- Klise, K. A. et al. (2017), A software framework for assessing the resilience of drinking water systems to disasters, *Environ. Model. Softw.* 95, 420–431.
- Swamee, P. K. and Jain, A. K. (1976), Explicit equations for pipe-flow problems, *J. Hydraul. Div.* 102, 657–664.
