# The Grammar of Event Spacetime in Melbourne and New York

*Status: draft (2026-11-26; [ADR 0168](../../../development/decisions/0168-the-grammar-of-event-spacetime.md), [ADR 0171](../../../development/decisions/0171-patterns-grown-histories-and-new-york.md)).* A record of **research**: the grammar learned from the event complex of a district of Melbourne's Hoddle Grid, 2002–2024, and of New York's Financial District, 2008–2024. It covers which events generate the complex, how much an event's surroundings and the states carried into it tell about what it does, how a new building is attached, what the volumes' lives share, the patterns that bind several volumes, and what histories grown from the grammar hold of the record. The constructions are syntaxis's `foams` and urban science's `events/grammar.hpp` ([the theory note](../../../domains/urban-science/theory/the-grammar-of-event-spacetime.md)).

## 1. The Question and the Record

The question is how the structure of the district's events in spacetime is generated: which productions build its complex, how far each is foreseen from what came before, and whether what an event does depends on its surroundings.

The record is the one the Urban Science Workbench's *Event Spacetime* builds ([ADR 0067](../../../development/decisions/0067-event-spacetime.md)), from the data library's city model and `city-events-melbourne.json`:
- **Volumes:** 440 buildings, 399 street spaces (streets, malls and laneways) and 356 junction spaces.
- **Interfaces:** 426 party walls, 1,075 frontages and 798 cross-sections.
- **Events:** 9,036 in all.
  - 7,723 censuses of buildings (CLUE);
  - 177 alterations: 127 refurbishments, 45 developments completed and 5 heritage amendments;
  - 17 heritage registrations;
  - 555 street events from the permits of 2014–2018, each also ended (555 ends);
  - 9 births: 6 constructions and 3 developments completed.

Its event complex $K$ has 9,038 vertices, 38,008 segments and 2,299 lenses: χ = −26,671, with Betti numbers 1, 27,730 and 1,058. New York's record is read the same way in §10.

## 2. The Method

`run.mjs` builds the district as the page does, asks the plug-in's engine for the record and then for its grammar, by the events' types and by their kinds, and writes `results.json`. The grammar reads:
- **Stars.** Each event's star is keyed four ways (star, rule, match, context). All 9,036 events have exact keys. A star with no corner between two participants, which is every star but the nine births', is keyed by the sorted list of its participants' labels; the births' stars, which open interfaces between their building and its neighbours, by canonical forms whose search was exact.
- **Five grammars.** The context-free grammar (a rule given what the event consumes), and the context-sensitive grammar with four kinds of context:
  - the surroundings alone (the interfaces open beside what it consumes);
  - with the buildings' uses as their last census saw them;
  - with the spaces under a regime begun and not yet ended;
  - with both.

  Each state is carried into an event from the record before it, never set by the event itself.
- **The prequential judgement.** Each event is priced by the grammar of every event before it, refitted at the start of each year (22 refits), summed by year and per event in two eras about 2014.
- **Lives.** Each volume's events in time, the censuses left out, compressed by Re-Pair kind by kind.
- **Patterns across the volumes.** The changes, the censuses left out, read as a process net and compressed by digrams (the theory note's §6).
- **Grown histories.** Forty histories grown from the grammar of the changes in the eras of the record's sources, each measured as the record is (the theory note's §7).

## 3. What Generates χ

χ = Σκ over the stars, each rule's share its count times its star's κ (syntaxis's Proposition 7.1); the shares add to χ exactly.

| Events | Count | Share of χ |
|---|---|---|
| the window's two ends | 2 | +1,080.5 |
| street events, at their starts | 555 | −13,863 |
| street events, at their ends | 555 | −13,863 |
| births | 9 | −25.5 |
| censuses, alterations and registrations | 7,917 | 0 |
| **all** | 9,038 | **−26,671** |

- **The censuses, alterations and registrations.** Each touches one building, which passes through, and meets no interface: κ = 0. These are 88% of the events, and they change nothing of the complex's topology. The record without its censuses has the same χ, which the reading checks.
- **The permits write χ.** Each holds on average 26.0 street spaces together (median 20, at most 199) and meets none of their interfaces, since Melbourne's permits do not say whether a street was closed. So each start and each end pinches its spaces' worldlines (κ = 1 − n). The 1,110 vertices of the permits give −27,726, all of χ apart from the window's ends and the births.
- **The births** attach new buildings (§5).

## 4. What the Grammar Foresaw

Bits per event, each event priced by the grammar of the events before it:

| Grammar | Before 2014 (4,145 events) | 2014 on (4,891) | Total bits | Pairs counted |
|---|---|---|---|---|
| a rule given what it consumes (context-free) | 0.269 | 0.758 | 4,823.4 | 236 |
| and its surroundings | 0.271 | 0.756 | 4,819.6 | 383 |
| and the buildings' uses | 0.268 | 0.753 | 4,794.1 | 644 |
| and the regimes in force | 0.271 | **0.687** | **4,485.1** | 421 |
| and both | 0.268 | 0.701 | 4,536.9 | 682 |

- **Before 2014.** The record is censuses, a few alterations, registrations and births, at 0.2 to 0.4 bits an event, and no context helps. An event's surroundings, the interfaces open beside what it consumes, save 3.9 bits over the whole record.
- **The permits arrive.** 2014 brings 105 rules never seen (2.71 bits an event), and 2015–2017 another 119. From 2018 the grammar foresees nearly everything: 0.05 to 0.19 bits an event. 227 rules first appear in or after 2014, carried by 1,113 events.
- **The regime in force is the context that tells.** It saves 338 bits (7.0%): 340 in 2015–2018 and 4 in 2014, with a net 6 lost over the other years. In 2014 most rules are new, and a new rule costs the same at every level. With both states, 2015–2017 cost 0.78, 0.83 and 0.68 bits an event, against 0.95, 0.93 and 0.83 for the context-free grammar. A permit's start and its end consume the same street spaces, so what is consumed cannot tell them apart; whether a regime is in force does. The uses save 29 bits.
- **The concentrations agree.** Chosen by leave-one-out on every event, α₂ is 141 for the surroundings alone: the grammar backs off to what is consumed and nearly ignores the context. With the regimes in force it is 0.71: the context is trusted.
- **Over every event.** H(R) = 1.437 bits; H(R | consumed) = 0.278; given the context, 0.267 with the surroundings alone, 0.244 with the uses, 0.187 with the regimes in force and 0.163 with both.
- **No order among the five.** All five grammars are undominated. The context-free one counts the fewest pairs, the uses do best before 2014, and the regimes in force do best after it. None is no better than another on all three measures, so the partial order ranks none below another.

## 5. How a New Building Is Attached

Each of the nine births is a rule of its own, seen once. Every one gives each standing volume it touches exactly one interface, a party wall or a frontage, so its κ is (1 − m)/2 for m neighbours (the theory note's Proposition 3.2):

| κ | Neighbours (m) | Interfaces opened |
|---|---|---|
| −1 | a building, a laneway, a street (3) | 2 frontages, a party wall |
| −1.5 | a building, 3 laneways (4) | 3 frontages, a party wall |
| −1.5 | 2 laneways, 2 streets (4) | 4 frontages |
| −2 | 2 laneways, 3 streets (5) | 5 frontages |
| −2.5 | 3 buildings, 2 laneways, a street (6) | 3 frontages, 3 party walls |
| −3.5 | 5 buildings, 2 laneways, a street (8) | 3 frontages, 5 party walls |
| −3.5 | a building, 5 laneways, 2 streets (8) | 7 frontages, a party wall |
| −4 | 4 buildings, 2 laneways, 3 streets (9) | 5 frontages, 4 party walls |
| −6 | 4 buildings, 2 laneways, a mall, 6 streets (13) | 9 frontages, 4 party walls |

Nine births are too few to learn attachment from: each is new when it comes.

## 6. The Lives of the Volumes

Each volume's changes in time, the censuses left out, compressed by Re-Pair within its kind:
- **Buildings.** 149 of the 440 have changes: 222 events, 176 symbols, 5 rules.
  - The commonest is two alterations in a row: 41 uses, by 39 buildings, among them the Flinders Gate Car Park, the T&G Building and the Galleria.
  - Three alterations in a row: 9 buildings.
  - A registration followed by alterations: David Jones and one other.
  - An alteration followed by a registration: Mecca and Mitchell House.
  - A registration followed by one alteration: Melbourne's GPO and one other.
- **Street spaces of streets.** 231 lives: 27,603 events, 1,829 symbols, 793 rules. The segments of each named street share its permits, and every life that spends one of these runs bears the street's name:
  - 11 segments of Swanston Street share a run of 288 events;
  - 18 of Bourke Street, a run of 278;
  - 25 of Collins Street, a run of 134;
  - 29 of Little Bourke Street, a run of 108;
  - 33 of Elizabeth Street, a run of 104;
  - 35 of Little Collins Street, a run of 96.

  The street as a unit, which the record never declares, appears as a rule of its segments' lives.
- **Malls.** 25 lives: 855 events, 525 symbols, 98 rules. The commonest is a permit on one mall space alone and its end: 89 uses over six spaces, among them Fulham Place, McKillop Street, Hardware Lane and Somerset Place.
- **Laneways.** 46 lives: 419 events, 238 symbols, 81 rules. Hosier Lane's four segments share runs of 40 events.
- **Junction spaces.** No changes: the permits name streets.

## 7. Patterns across the Volumes

The changes, the censuses left out, read as a process net and compressed by digrams (the theory note's §6): 1,313 events, 28,648 wires and 893 open ends. Compression finds 94 patterns. Their description falls from 29,961 to 22,697 (a ratio of 1.32), and 8,262 of the wires lie inside patterns that bind two volumes or more.

| Pattern | Volumes | Instances | Most often |
|---|---|---|---|
| a permit and its end on one mall space | 1 | 89 (6 spaces) | Bourke Street Mall (75) |
| a permit and its end on Swanston Street's 11 segments | 11 | 49 | the same 11, every time |
| a permit and its end on Bourke Street's 20 spaces | 20 | 48 | the same 20, every time |
| two alterations of a building in a row | 1 | 41 (39 buildings) | Collins Arcade (2) |
| a permit and its end on Bourke Street with its mall | 21 | 24 | the same 21, every time |
| a permit and its end on Collins Street's 25 segments | 25 | 24 | the same 25, every time |
| a permit and its end on Little Bourke Street's 29 segments | 29 | 16 (2 sets) | the same 29 (14) |
| a permit and its end on Little Collins Street's 35 segments | 35 | 15 (3 sets) | the same 35 (12) |
| a permit on the mall alone, then one on Bourke Street with its mall | 21 | 8 | the same 21, every time |

- **Each named street is one pattern.** The lives found Swanston Street as a run that each of its 11 segments' words repeats (§6). The net finds it as one pattern whose every instance binds the same 11 volumes. The street, which the record never declares, is the set of volumes a pattern's instances take together.
- **Patterns nest.** A permit and its end on a street, twice in a row, is a pattern of patterns (14 instances on Bourke Street, 12 on Swanston Street). The longest among the commonest, eight events, is on Bourke Street Mall.
- **A pattern can join two units.** A permit on the mall alone, followed by one on Bourke Street with its mall, binds 21 volumes by the one worldline they share.
- **With the censuses.** Read as recorded, the net's patterns are runs of censuses on single buildings (3,683 pairs, up to 24 in a row), the lives' structure again: 45,202 falls to 25,922.

## 8. Histories Grown from the Grammar

Forty histories are grown from the grammar of the changes, the censuses left out (the theory note's §7). They grow at 136 places, 4 of them any volume of a kind, in three eras cut at 2014 and 2018, each street space carrying the number of regimes in force on it. Each is measured as the record is:

| Measure | The record | Grown, on average | Grown, 5% to 95% | Grown at or below the record |
|---|---|---|---|---|
| events | 1,313 | 1,230.7 | 1,118 to 1,349 | 83% |
| χ | −26,671 | −25,813 | −27,885 to −23,936 | 20% |
| depth of the trace | 400 | 365.0 | 335 to 405 | 93% |
| prequential bits per event | 1.818 | 2.058 | 1.884 to 2.221 | **0%** |
| the street spaces' lives, compressed | 15.09 | 15.33 | 14.78 to 15.92 | 35% |
| the net, compressed | 1.320 | 1.348 | 1.298 to 1.396 | 13% |

- **What the grammar holds.** The grown histories hold the record's χ, the depth of its trace, how far its lives and its net compress, and its events by kind: 176.3 alterations against 177, 7.6 births against 9, 14.5 registrations against 17, 518.8 permits begun and 513.6 ended against 555 of each. Every permit ends at the place it began. The table of what followed is a function on the record: of the 28,648 states its volumes carried out of their events, none differs from what the state carried in, the event and the place give most often.
- **What it does not hold.** The record is more predictable than every grown history: its prequential code is 1.82 bits an event, the grown histories' 1.88 to 2.22. Drawing each event's rule afresh in its context leaves the grown histories less ordered than the record they came from. Within each era the grown histories keep an even pace: about 12 events a year before 2014, 240 to 300 in 2014–2017 and 6 to 12 after, where the record has 353 in 2014 and 21 in 2018. A place whose one permit the record shows has its rate from that permit's wait, so a grown history leaves some places without their permit (518.8 begun against 555).
- **Three designs that failed.** A place that read the states its street spaces share saw other permits' regimes and ended what it never began: 555 permits grew to about a thousand. A place keeping the states its own events left, by a table learned from those shared states, ended twice where its streets were often held by others (610 ends against 555 beginnings). A place remembering its last event, with each space carrying only whether a regime is in force, left about a twentieth of its permits unended: 66 of the permits in Melbourne's data begin while another on the same named streets is in force, and after one of two such permits ends the table of what followed calls the streets free. The number in force mends it.

## 9. By Kinds

Read with each event's kind (22 kinds of permit, among them filming, promotions, public events and parades), the grammar has 343 rules, 329 of them the permits' starts and ends. What an event consumes leaves more open: H(R | consumed) = 0.390 bits, and 0.239 given every state. After 2014 the context-free grammar pays 1.138 bits an event and the grammar with both states 1.110. The kind of a permit is less foreseen than its coming and going, and the regime in force tells less of it.

## 10. New York's Financial District

The record is built from NYC Open Data (`data/products/city_events_new_york.py`): the Financial District's old streets about Wall Street, Stone Street and Hanover Square, 2008–2024.
- **Volumes:** 168 buildings, 173 street spaces (streets, malls and plazas) and 145 junction spaces.
- **Interfaces:** 127 party walls, 422 frontages and 346 cross-sections.
- **Events:** 2,441 in all.
  - 1,161 permits from the permitted events of Manhattan's community district 1 and the film permits, each a block or several resolved on the district's network, 102 of them full closures; 1,159 of them ended in the window (102 reopenings among the ends);
  - 108 alterations: 106 major alterations completed, and two accounts of a building's making kept as alterations, the building having an earlier one;
  - 11 births: 4 constructions and 7 developments completed;
  - 2 landmark designations.
- **Left out:** 579 permits none of whose blocks lies in the district (1,365 blocks in all whose street does not run between its two cross streets inside it), and 66 certificates or footprints naming no building of the district.

Its event complex has 2,443 vertices, 7,264 segments and 2,611 lenses: χ = −2,210, with Betti numbers 1, 2,596 and 385. Its Foata steps are 725 deep, where Melbourne's changes are 400 deep over a window six years longer: most of New York's events follow one another on a few spaces.

| Events | Count | Share of χ |
|---|---|---|
| the window's two ends | 2 | +395.5 |
| permits, at their starts | 1,059 | −1,282 |
| permits, at their ends | 1,057 | −1,276 |
| closures, at their starts | 102 | −16 |
| closures, at their ends | 102 | −16 |
| births | 11 | −15.5 |
| alterations and designations | 110 | 0 |
| **all** | 2,443 | **−2,210** |

- **The permits write χ, as in Melbourne.** A permit holding n spaces without closing them has κ = 1 − n. New York's permits hold 2.2 spaces on average (median 1, at most 32); 646 of the 1,059 hold one space, and those leave χ as it was.
- **A closure leaves χ as it was.** It meets every cross-section of the blocks it closes, so κ = 1 − χ(G) for the graph G of those blocks and their junctions (the theory note's Proposition 8.2). Every closure rule holds it. 178 of the 204 closures and reopenings have κ = 0, their blocks a graph of Euler characteristic one (one run of blocks, or one tree of runs); the other 26 have κ from −1 to −3.

**The grammar.** Bits per event, each event priced by the grammar of the events before it, refitted each year (13 refits):

| Grammar | Before 2016 (589 events) | 2016 on (1,852) | Total bits |
|---|---|---|---|
| a rule given what it consumes (context-free) | 1.603 | 1.288 | 3,328.9 |
| and its surroundings | 1.605 | 1.287 | 3,329.1 |
| and the regimes in force | **0.973** | **0.583** | **1,653.2** |

- **The regime in force halves the code.** It saves 1,675.7 bits (50.3%), against Melbourne's 7%. H(R | consumed) = 0.965 bits, and 0.115 given the regimes in force. New York's permits mostly hold one or two spaces, so their rules recur and what is left open is whether an event begins or ends a permit, which the regime in force tells. Melbourne's permits hold 26 spaces on average and their rules are often new, and a new rule costs the same at every level.
- **The surroundings tell nothing**, and New York's record has no censuses, so there are no uses to carry. All five grammars are undominated.
- **The sources arrive.** Before 2012 the record holds two events. The context-free grammar pays most per event in 2013 (2.53 bits), when the permits' rules are still new, and in 2023 (1.48 bits), when the film permits arrive; 95 rules first appear in or after 2016. 2020 holds 39 events against 190 the year before.

**Patterns across the volumes.** The net (2,441 events, 6,523 wires) falls from 8,964 to 4,015 (a ratio of 2.23) by 139 patterns, and 2,633 wires lie inside patterns of several volumes. A permit and its end on one mall space (315 instances, 206 on Broad Street's) and on one street space (310, 130 on Wall Street) are the commonest. Runs of them nest: read by kinds, the farmers' market at Coenties Slip is a permit and its end 109 times, in runs up to 64 events long. A permit and its end on two street spaces recurs 131 times, 81 of them on Exchange Place and Wall Street, and one on Old Slip, Coenties Slip and Gouverneur Lane 49 times, always those four spaces.

**Histories grown from the grammar.** Forty histories are grown at 172 places, 3 of them any volume of a kind, in three eras cut at 2012 and 2023:

| Measure | The record | Grown, on average | Grown, 5% to 95% | Grown at or below the record |
|---|---|---|---|---|
| events | 2,441 | 2,200.0 | 1,766 to 2,724 | 78% |
| χ | −2,210 | −2,423 | −2,929 to −2,056 | 70% |
| depth of the trace | 725 | 439.0 | 355 to 591 | **100%** |
| prequential bits per event | 0.682 | 0.912 | 0.732 to 1.095 | 3% |
| the street spaces' lives, compressed | 3.06 | 2.91 | 2.71 to 3.08 | 88% |
| the net, compressed | 2.233 | 2.059 | 1.951 to 2.173 | 98% |

- **What the grammar holds.** χ, how far the street spaces' lives compress, and the events by kind: 103.0 alterations against 108, 10.1 births against 11, 1,058.9 permits begun against 1,161. The table of what followed is a function on the record: none of its 6,523 states out differs from its commonest.
- **Where the record lies outside its grown histories.** Its trace is deeper than every grown history's (725 against 355 to 591). Its net compresses better, and its code is shorter, than all but one. New York's events follow one another on a few spaces in runs more regular than the grammar's draws: the farmers' market at Coenties Slip holds its space in runs of up to 32 permits with nothing between, where a grown place fires at random times at its rate.
- **Its years.** The grown histories rise through the second era as its places begin, from 67 events in 2012 to 198 in 2022. The record rises at once (221 in 2014) and falls in 2020 (39 events), a year no era marks. In 2023 and 2024 the grown histories hold 388 and 340 events, against the record's 270 and 428.
- **Permits left open.** On average 32 grown permits are still in force at the window's end, against the record's 2.

## 11. What the Record Cannot Show

- **The kinds of event are the sources'.** In Melbourne the censuses are CLUE's, the alterations the monitor's and the register's, the street events the permits'. A context that tells nothing says so of this record.
- **Melbourne's permits are a source of 2014–2018.** The grammar's novelty in 2014–2017 is that source arriving. The prequential judgement prices it once, and so no later year is mistaken for a failure of the grammar.
- **The regime in force is partly the record's construction.** The record makes each permit two events, a start and an end. That the end follows the start is a law of how the record is built before it is one of the city; what the state adds beyond it is the overlap of permits on one space.
- **The grown histories are the grammar's.** They hold what the grammar holds: each place's last event, its rate in its era, the rule in its context, the number of regimes in force. A memory longer than one event, a pace more regular than chance, and a year no era marks, such as 2020, are not in them.
- **New York's record is its sources'.** The permits of Manhattan's community district 1 name blocks, and 579 of them lie wholly outside the district. The certificates and footprints name buildings by position.
- **No cause.** The grammar describes how the record's complex is generated, and says nothing about why any event occurred.

## 12. How to Run

```
python applications/urban-science-workbench/engine.py build event-spacetime
node research/experiments/2026-11-26-event-grammar/run.mjs
node research/experiments/2026-11-26-event-grammar/run.mjs --summary
python data/products/city_events_new_york.py
node research/experiments/2026-11-26-event-grammar/run.mjs --area new-york
```

A run takes a few minutes and writes `results.json` (`results-new-york.json` for New York); `--summary` prints its summary again, and `--split`, `--breaks` and `--histories` change the split, the eras and the number of grown histories. New York's record is built from NYC Open Data, fetched once into the cache. The same reading is the workbench's *Event Spacetime* tab *Its grammar*, in either area.
