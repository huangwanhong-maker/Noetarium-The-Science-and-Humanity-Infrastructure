# 0171. Patterns across Worldlines, Histories Grown from the Grammar, and New York's Record

- **Status:** Accepted, provisionally
- **Date:** 2026-11-26
- **Decided by:** the user's word to build the three things [ADR 0168](0168-the-grammar-of-event-spacetime.md) left unbuilt: "patterns spanning several streets found from the complex itself …; a generator that grows a district from its grammar; New York's record. do all of them, merci."

## 1. Context

ADR 0168 learned the grammar of a city's event spacetime and left three things unbuilt:
- its lives compressed each volume's history alone, so a pattern that binds several volumes showed only as runs that several lives share;
- its grammar could judge a record and could not grow one;
- only Melbourne's record had been compiled.

## 2. Decision

1. **syntaxis's `foams`** (its decisions 0005 to 0007; its theory note's §7.6 and §7.7):
   - `nets.hpp`: a foam read as a process net, its events as boxes with a port for each worldline they consume or leave, its segments as wires. The net is compressed by digrams after gRePair (Maneth and Peternek 2018): the commonest pair of boxes joined by wires, counted on occurrences that share no box, becomes a pattern, again until no pair recurs. Every pattern derives its events exactly.
   - `grow.hpp`: foams grown by the event operator. A site is generic (any participant of a kind) or concrete (the participants of an event the record holds), and each site is an automaton whose state is its last production. Its rates are per exposure, by last production and era, and a state an era never saw takes the window's rate. The rule is drawn from the context-sensitive grammar among the rules of the site's action that followed its last production in the record. The participants' states follow the record's table of what followed, and the growth says how far that table is from a function (`unsure`). Each history is simulated by Gillespie's direct method on a calx stream.
   - each star's *shape*, its rule with the event's kind left out.
2. **Urban science's `events`** (its theory note's §6 to §8):
   - `patterns`, `measure` and `grow` in `grammar.hpp`; the growth carries the number of regimes in force on each space;
   - `street_key` and `street_between` in `network.hpp`: a block named by its street and two cross streets, resolved on the district's network;
   - Proposition 8.2, a closure's curvature κ = 1 − χ(G) for the graph G of the blocks it closes and their junctions;
   - four checks more in `events`: a street between two others, the patterns of a record's changes, grown histories, and a street held by two regimes at once.
3. **The data library's `city_events_new_york.py`** (registry `nyc-district-events.json`): New York's Financial District from NYC Open Data, 2008–2024. Its sources are the building footprints, DOB's and DOB NOW's certificates of occupancy, the individual landmark sites, the permitted events of Manhattan's community district 1 (with their full closures) and the film permits.
4. **The workbench's *Event Spacetime***:
   - New York as an area;
   - the engine's `grammar` gives the patterns of the changes, and its new `grow` grows histories in each area's eras;
   - the tab *Its grammar* gains *Patterns across the worldlines* and *Histories grown from the grammar*;
   - the split and the eras by area (Melbourne 2014, with eras cut at 2014 and 2018; New York 2016, with eras cut at 2012 and 2023).
5. **The experiment** [2026-11-26-event-grammar](../../research/experiments/2026-11-26-event-grammar/README.md), extended: patterns and grown histories for Melbourne, and New York's record read the same way.

## 3. Alternatives Considered

- **Patterns from the lives alone** (ADR 0168). They see one worldline at a time. A run that several lives share shows as one rule in each life, never as one pattern binding the volumes.
- **Hyperedge replacement over the whole foam.** It is the general form, and its induction needs matching on typed complexes. A pattern of the net is a two-box hyperedge rule, nested, found by counting.
- **Growth from the grammar alone, without sites.** The grammar knows stars, not where they attach. A permit drawn anywhere would join streets that no permit joined. The sites keep the record's places.
- **A site reading its participants' shared states** (syntaxis's decision 0005). Another site's regime puts it under one, and it ends what it never began: Melbourne's 555 permits grew to about a thousand.
- **The states each site's own events left, by a table learned from the shared states** (syntaxis's decision 0006). It still gave 610 ends against 555 beginnings.
- **A rate of nought where an era never saw a state.** A permit grown across an era's start into an era that held none in force stayed in force to the end.
- **Whether a regime is in force carried in the growth.** Where permits overlap on one place an end leaves the place held, so the state carried out is no function of the state carried in, the rule and the place. Grown so, about a twentieth of Melbourne's grown permits were left unended. The number in force is such a function.
- **New York's blocks as the whole named street.** A permit names a block between two cross streets. The whole street would hold more spaces than the permit took.

## 4. Consequences

- **What Melbourne shows** (the changes, the censuses left out: 1,313 events):
  - each named street is one pattern whose every instance takes the same street spaces: a permit and its end on Swanston Street's 11 segments 49 times, on Bourke Street's 20 spaces 48 times, on Collins Street's 25 segments 24 times; the net falls from 29,961 to 22,697 by 94 patterns;
  - forty grown histories hold the record's χ (−25,813 on average, −27,885 to −23,936, against −26,671), the depth of its trace, how far its lives and its net compress, and its events by kind; every grown permit ends where it began, the table of what followed a function on the record (none of 28,648 states out other than its commonest);
  - the record is more predictable than every grown history (1.82 bits an event, against 1.88 to 2.22), and within each era the grown histories keep an even pace where the record peaks in 2014.
- **What New York shows** (168 buildings, 173 street spaces, 145 junction spaces; 2,441 events; χ = −2,210):
  - its permits hold 2.2 spaces on average, 646 of 1,059 only one, so most leave χ as it was; every one of its 102 closures and their reopenings holds Proposition 8.2, 178 of the 204 with κ = 0;
  - whether a regime is in force halves the code (3,328.9 bits to 1,653.2, against Melbourne's 7%): its permits' rules recur, and what is left open is whether an event begins a permit or ends one;
  - its patterns are runs on a few spaces: a permit and its end on one mall space 315 times, the farmers' market at Coenties Slip 109 times in runs up to 64 events, a permit on Old Slip, Coenties Slip and Gouverneur Lane 49 times on those four spaces;
  - forty grown histories hold its χ, its events by kind and how far its lives compress; its trace is deeper than every grown history's (725 against 355 to 591), and its net compresses better than all but one: its events follow one another on a few spaces in runs more regular than the grammar's draws.
- **Found while building:**
  - a place that read the states its streets share ended what it never began, and one that kept its own states by a table learned from them still ended twice (syntaxis's decisions 0005 to 0007);
  - a rate of nought for a state an era never saw left permits grown across an era's start in force to the end;
  - whether a regime is in force cannot be updated by a growth where permits overlap: 66 of the permits in Melbourne's data begin while another on the same named streets is in force, and grown so, about a twentieth of the permits were left unended. The growth carries the number in force, and its table of what followed is a function on both records (none of 28,648 states out, and none of 6,523, other than the commonest);
  - the page's texts that named Melbourne's split and sources are each area's own.
- **Pictures:** three more in the gallery's §56, from the page driven in headless Chrome (`applications/urban-science-workbench/tests/drive-event-grammar.mjs`, not in the suite).
- **Not built:**
  - patterns used as the growth's sites, and a site's memory beyond its last production;
  - breaks the eras do not cut, such as 2020;
  - New York's Open Streets as regimes of the interfaces, and its building permits as events;
  - demolitions: a birth ends nothing, since the buildings are those standing now;
  - a third city.
- **Provisional:** the eras and splits by area, New York's district, and the names of the tab's sections.
