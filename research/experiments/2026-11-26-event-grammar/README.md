# The Grammar of Melbourne's Event Spacetime

*Status: draft (2026-11-26; [ADR 0168](../../../development/decisions/0168-the-grammar-of-event-spacetime.md)).* A record of **research**: the grammar learned from the event complex of a district of Melbourne's Hoddle Grid, 2002–2024. It covers which events generate the complex, how much an event's surroundings and the states carried into it tell about what it does, how a new building is attached, and what the volumes' lives share. The constructions are syntaxis's `foams` and urban science's `events/grammar.hpp` ([the theory note](../../../domains/urban-science/theory/the-grammar-of-event-spacetime.md)).

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

Its event complex $K$ has 9,038 vertices, 38,008 segments and 2,299 lenses: χ = −26,671, with Betti numbers 1, 27,730 and 1,058.

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

## 7. By Kinds

Read with each event's kind (22 kinds of permit, among them filming, promotions, public events and parades), the grammar has 343 rules, 329 of them the permits' starts and ends. What an event consumes leaves more open: H(R | consumed) = 0.390 bits, and 0.239 given every state. After 2014 the context-free grammar pays 1.138 bits an event and the grammar with both states 1.110. The kind of a permit is less foreseen than its coming and going, and the regime in force tells less of it.

## 8. What the Record Cannot Show

- **The kinds of event are the sources'.** The censuses are CLUE's, the alterations the monitor's and the register's, the street events the permits'. A context that tells nothing says so of this record.
- **The permits are a source of 2014–2018.** The grammar's novelty in 2014–2017 is that source arriving. The prequential judgement prices it once, and so no later year is mistaken for a failure of the grammar.
- **The regime in force is partly the record's construction.** The record makes each permit two events, a start and an end. That the end follows the start is a law of how the record is built before it is one of the city; what the state adds beyond it is the overlap of permits on one space.
- **No cause.** The grammar describes how the record's complex is generated, and says nothing about why any event occurred.

## 9. How to Run

```
python applications/urban-science-workbench/engine.py build event-spacetime
node research/experiments/2026-11-26-event-grammar/run.mjs
node research/experiments/2026-11-26-event-grammar/run.mjs --summary
```

The first run takes about two minutes and writes `results.json`; the second prints its summary again. The same reading is the workbench's *Event Spacetime* tab *Its grammar*.
