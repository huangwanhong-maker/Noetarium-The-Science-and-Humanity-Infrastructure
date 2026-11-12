---
title: A Design Studio for Buildings, and Then for Streets
status: promoted (ADR 0095: the studio and the plan, stage 1 of §5; ADR 0096: the interior of stage 3 begun, and the building walked through; ADR 0097: the shell of stage 2, without a day's heat and the façade's grammar; ADR 0098: the first step to the street of §6); stages 4 and 5 remain planned
date: 2026-11-12
related: [architecture (ADR 0062, ADR 0063, ADR 0089), the board and its agent (ADR 0090, ADR 0091), Market Studio (ADR 0093), the whole Earth in the workbench (ADR 0078), heuresis (ADR 0073), syntaxis (ADR 0083), the survey research/literature/reports/Design of a single building.md]
---

# A Design Studio for Buildings, and Then for Streets

*The record of a discussion with the user on 2026-11-11 and 2026-11-12, at their opening: "Architecture and Street Spatial Design. We will from a single architecture (both outdoor appearance and indoor appearance and equipment design), then move to street architectures/spatial design." The first stage was built the same day and is recorded as [ADR 0095](../decisions/0095-the-design-studio.md); this note keeps the reasoning, the turns the user gave it, and what is planned beyond that record.*

## 1. The Question

The user asked what existing easy-to-use design software still leaves open, and what our innovation would be. A survey was made to answer with checked facts ([*Design of a single building*](../../research/literature/reports/Design%20of%20a%20single%20building.md)): the software, the literature, and the open data and real rules of a first case.

## 2. What the Tools Leave Open

- **A design is a drawing, not an object one can reason about.** The room planners compute nothing of what they draw; the professional tools compute on massing at the scale of a site.
- **Scales are disconnected.** Furniture, room, building, lot and street live in different tools, though they meet at shared interfaces: a door, a façade, a frontage.
- **Rules are not reported.** No vendor publishes which provisions it checks, or answers rule by rule.
- **Equipment is absent or decorative,** and no result carries its uncertainty.

## 3. The Three Turns

The design first proposed was a chain of refinements from a brief to a building, generated, on a real lot in Melbourne. The user turned it three times.

1. **One building first.** "I think consider single architecture first is better. For street one, I think we can utilize GIS, and may be need solve some optimization problems." And of the street: "geography, and economics etc reality is one we need pursue."
2. **Any street.** Of a first site in one city: "it is wierd. I think it should be a general software, and can choose any street." A place is a case of data: a scheme of rules, whatever its records add. The same condition was set for ADR 0087.
3. **A studio, not a generator.** "I don't think we should always autogenerate design. But in fact a design studio, can even allow users to design. Of course, can like market AI agent, automatically use tools and plug-in provided to design. but it just assistant". It is the turn the user gave the board (ADR 0090): tools are tools.

The user also remarked that the infrastructure's meaning emerges from relations: the search, the design loop, the heat on Hodge Laplacians and the networks were written for other subjects, and a building needs each of them.

## 4. What Is Ours, and What Is Not

The survey corrected the first claims.

| Claimed at first | After the survey |
|---|---|
| plans enumerated, not sampled | old: Mitchell, Steadman and Liggett enumerated small rectangular plans in 1976. Ours could be the real envelope, several storeys, and its place as an assistant beside a hand-made design |
| nobody computes a design | true of the consumer tools; Autodesk Forma computes sun, wind, noise and carbon on massing |
| an agent that designs | arriving in several tools; ours would act on operations that can be undone, with their consequences computed |
| grammars of buildings | written by hand since 1972; one façade grammar was learned and parsed in 2013. A symmetry group acting on a learned grammar was not found: the user's group grammars have an opening |

What remains ours, more narrowly and more firmly: one house computed inside and out as a person draws it, on any street, with its rules reported one by one and what is presumed said to be; and, later, grammars that carry symmetry.

## 5. The Building: Five Stages

Each stage is a complete slice: a module of the domain with its checks, and hand tools in the studio with their measures. Assistants may arrive with their stage, but no stage depends on them.

1. **The studio and the plan** (ADR 0095, written). A building drawn by hand on any lot; rooms as the faces of the walls; areas, widths, light, air, access; siting rules and a code of habitation as declared schemes; a brief; quantities, cost as a range, the fabric's heat loss.
2. **The shell.** Walls and roof with real constructions; a roof's form; daylight from the sky; heat through a day with the real windows, after ISO 52016-1's hourly method, held to the BESTEST cases, EnergyPlus and Radiance; the façade as a grammar on the envelope's faces, with its symmetry and what the plan's windows break of it; materials, drawn from the model so that a picture is never inconsistent with the design. The rules that need the neighbours come here: overlooking, overshadowing, daylight to their windows.
3. **The interior** (begun, [ADR 0096](../decisions/0096-what-stands-in-the-rooms.md)). Fixtures and furniture as parametric objects with use-zones and service connections, placed by hand and checked against published clearances. No open library of such fixtures exists; ours is our own: 36 kinds, each with the clear floor it needs and a model built to its size, found within its room, usable and reached. At the user's further request the building is walked through at eye height. The connections, and clearances stated by a code, remain.
4. **Equipment.** Water, drainage, power, heating and air as networks through walls and shafts, routed and sized as the plan changes, on physis's Kirchhoff networks. The literature routes services on a plan taken as given; here the plan is the person's, and the networks follow it live.
5. **The assistants.** Alternatives for a storey: rectangulations enumerated (held to 1, 2, 6, 24, 116, 642, 3938, … for 1 to 7 rooms), kept where they answer the brief, dimensioned, with the theorem that says when dimensioning cannot fail (a layout takes every assignment of areas exactly when it is one-sided; Eppstein et al. 2012). Search over the whole with heuresis, the person choosing from a front. An agent given the studio's operations and the plug-ins the person allows, as Tabula's (ADR 0091). An assistant never undoes what the hand fixed (after Tanagra, 2011).

## 6. The Street

After the building, at the user's direction: GIS, optimisation, and reality.

- **Composition.** A street is buildings joined at their frontages and party walls; the district is already one complex (ADR 0063). A building drawn in the studio would stand in the Urban Science Workbench's city, and a block's buildings be designed together.
- **Reality.** The street is judged by what the domains already hold: walkers (`movement`), configuration (`syntax`), access to services, what people pay for a home (`demand`), custom for a shop (`markets`), who will live there (demography), sun and heat (architecture, ADR 0089).
- **What is missing.** Construction cost from quantities at sourced rates; feasibility (revenue less cost, the residual value of the land), without which an optimiser proposes what nobody would build; parcels where a cadastre is open; a place's rules as written.
- **A caveat from our own studies.** Configuration did not explain Melbourne's walkers (ADR 0084), and the records could not identify what a new building did (ADR 0087). Pursuing reality means reporting such limits.

## 7. Open Questions

- The work's name and colour: the user's.
- Whether equipment stays in architecture as `services` or becomes a domain of building services, as civil engineering will be for structures.
- Where a jurisdiction's scheme belongs when several works use it: now a file of the studio; perhaps the data library's.
- Which open plans could teach a grammar: Swiss Dwellings (45,176 apartments, CC BY 4.0) is usable; no open collection of house plans was found.
