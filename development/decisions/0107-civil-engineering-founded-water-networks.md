# 0107. Civil Engineering Founded: Water Networks

- **Status:** Accepted, provisionally (2026-11-13).
- **Date:** 2026-11-13
- **Decided by:**
  - **The request.** The user asked to implement the handbook's methods, and said that new domains could be founded where needed if they were told: "implement them, as possible, if some new domain(s), foundation(s) is/are needed, just tell me".
  - **The decision.** Claude told the user that the handbook's water networks (§6.1: EPANET, WNTR) need civil engineering. README §4 already plans that domain, for structures, hydraulics and transport. Claude founded it with this module.
  - **The design.** Claude, here.

## 1. Context

The handbook's §6.1 names models of water, soil and agriculture, and warns that "a river-basin nutrient model does not replace a municipal drinking-water network model". Of these, a distribution network is the most general: every city has one, and its service (pressure, age, what is lost when a pipe fails) is what the handbook calls the difference between a resource's presence and its accessibility.

The infrastructure had Kirchhoff's networks with linear laws (physis 0022) and no nonlinear hydraulics.

## 2. Decision

- **physis first** ([physis decision 0029](https://github.com/huangwanhong-maker/physis/blob/main/development/decisions/0029-networks-of-conduits.md)): networks of conduits with monotone laws, by Todini and Pilati's global gradient algorithm. It names no fluid.
- **civil-engineering** is a field at height 0 on the foundations alone, C++ from the start. Its theory note is `domains/civil-engineering/theory/water-networks.md`. Its first module, `water`:
  - EPANET's input files read, converted to SI;
  - heads, pressures and flows at an instant, with:
    - Hazen–Williams' law (with EPANET's coefficient) and Darcy–Weisbach's (with Swamee and Jain's friction factor);
    - minor losses;
    - pumps' curves fitted as EPANET fits them;
    - check valves;
  - demand-driven and pressure-driven demands;
  - isolated junctions found;
  - a period simulated with patterns, tanks and simple controls;
  - the age of the water;
  - Todini's resilience index;
  - the criticality of each link.
- `structures`, `transport` and `drainage`: planned.

## 3. Alternatives Considered

- **Wrapping EPANET's toolkit.** EPANET is public domain and could sit behind the domain's interface. Rejected for now: the method is physis's, and the domain's own solver can be held to EPANET, as it is here, to a millimetre on Net1. EPANET's toolkit remains a candidate backend for very large networks.
- **Water in urban science.** Rejected: the network is an engineered system that urban science, social provision and energy systems all use. README §4 already places hydraulics in civil engineering.
- **A domain of water resources** (hydrology, groundwater, crops). README §4 puts hydrology in geography. River basins (SWAT+), groundwater (MODFLOW) and crops (AquaCrop) belong there, or to a bridge over geography and civil engineering, when a case needs them.

## 4. Consequences

- **Checks.** physis's conduits (8 checks) and the domain's water module (25).
- **Shown.**
  - EPANET's example network is reproduced to a millimetre of head.
  - A branch's loss costs its junction's demand exactly, while a loop's pipe can be lost with everything still delivered.
- **Not built:**
  - EPANET's timing within a step;
  - valves other than check valves;
  - chlorine over time;
  - structures, transport and drainage.
- **Provisional:** the steepened ends of Wagner's law; the age of the water a tank discharges, taken as zero.
