# 0168. The Grammar of Event Spacetime

- **Status:** Accepted, provisionally; amended by [ADR 0171](0171-patterns-grown-histories-and-new-york.md), which builds what its §4 left unbuilt
- **Date:** 2026-11-26
- **Decided by:** the user's question: "I wonder whether we already have 2-complex generative grammar. If we have, please learn such grammar, and utilize learned grammar to analyze urban spacetime events' structure's generation process."

## 1. Context

Two kinds of 2-complex already existed in the infrastructure, and neither had a learned grammar of events.
- **syntaxis's `simplicial`** (ADR 0083) holds grammars whose words are triangles in the plane, with letters by shape: a gluing grammar and a replacement grammar with its analyser.
- **A city's record of events** ([ADR 0067](0067-event-spacetime.md)) makes a 2-complex of another kind. It is morphe's worldline complex of the record's encounters: abstract, typed, with holes, and generated one event at a time. The design of event spacetime defines each event as a rule schema, whose event operator attaches the event's vertex with its link (its Definition 4.8); those rules had yet to be learned from a record.

The prototypes of the user's draft papers on grammars of records were in Python, outside the libraries. So the answer to the question was: in part. There was a grammar of 2-complexes of triangles, and an event operator defined and still to be learned.

## 2. Decision

1. **morphe's `repair`** (its decision 0022): straight-line grammars of words by Re-Pair, each run of letters counted once, every word recovered exactly.
2. **syntaxis's `foams`** (its decision 0004; its theory note's §7), grammars whose words are foams:
   - the star of each vertex, with roles and corners, four canonical keys (star, rule, match, context), and its curvature κ, with χ = Σκ;
   - the Foata steps of the record's trace;
   - the stochastic grammar at three levels by a hierarchical Dirichlet backoff, judged on held-out events and prequentially, and compared as a partial order by dominance;
   - rates per exposure;
   - the participants' lives compressed by Re-Pair.
3. **Urban science's `events/grammar.hpp`** (theory note [the-grammar-of-event-spacetime.md](../../domains/urban-science/theory/the-grammar-of-event-spacetime.md)):
   - the event complex as a foam, by the events' types or kinds;
   - the couplings' states carried into each event from the record before it (a building's use as its last census saw it, a space under a regime begun and not yet ended), never set by the event itself;
   - the reading: rules with their κ and their shares of χ; the prequential judgement by year and in two eras; entropies; the lives by kind of coupling; rates;
   - grammars with each kind of context compared by dominance;
   - four checks in `events`, by hand.
4. **The workbench's *Event Spacetime*:**
   - the engine's `grammar`, reading the record as recorded, with each kind of state, and without its censuses;
   - the tab *Its grammar* of *The record and its complex*;
   - the layer colours each event by what it cost the grammar when it came, and names its rule when picked;
   - six checks on the workbench's district of three buildings.
5. **The experiment** [2026-11-26-event-grammar](../../research/experiments/2026-11-26-event-grammar/README.md): the grammar of a district of Melbourne's Hoddle Grid, 2002–2024.

## 3. Alternatives Considered

- **Extending `simplicial`.** Its letters are triangles by angle. A foam's letters are combinatorial types of stars, and its cells are lenses bounded by two worldlines. One structure for both would hide the difference.
- **A split by date as the test.** Melbourne's permits begin in 2014. A grammar learned before 2014 has never seen a street event, so a test after 2014 measures the source's arrival. The prequential judgement prices each new rule once and then learns it.
- **States given at the vertex of the event that sets them.** A census's sight of a use at its own vertex, or a regime's start at its own, would put the event's outcome into its context and void the test. Each state is given as carried into the event.
- **The surroundings as the only context.** Over Melbourne's record they save 3.9 bits of 4,823. The state that tells, whether a regime is in force, is an attribute the productions set themselves, and the grammar read with it is a grammar of attributed symbols.
- **Hyperedge replacement over the whole foam.** It is the general form, and its induction needs matching on typed complexes. The star grammar is exact and local, and Re-Pair over lives gives its hierarchy along worldlines; nonterminals across worldlines stay planned.

## 4. Consequences

- **What Melbourne shows** (440 buildings, 399 street spaces, 356 junction spaces; 9,036 events; χ = −26,671):
  - the censuses, alterations and registrations (88% of the events) have κ = 0 and change nothing of the topology; the 555 permits, each holding on average 26 street spaces at its start and at its end, write χ (−27,726), with the window's ends (+1,080.5) and the nine births (−25.5);
  - before 2014 an event costs 0.2 to 0.4 bits and no context helps; 2014–2017 bring 224 rules never seen, and from 2018 the grammar foresees nearly everything (0.05 to 0.19 bits an event);
  - the surroundings save 3.9 bits, the uses 29, and the regime in force 338 (7.0%), since a permit's start and its end consume the same spaces; all five grammars are undominated;
  - every birth gives each standing volume it touches exactly one interface, so its κ is (1 − m)/2; each of the nine is a rule of its own;
  - the segments of each named street share its permits (Swanston Street's 11 segments a run of 288 events): the street, which the record never declares, appears as a rule of their lives.
- **Pictures:** four in the gallery's §56, from the page driven in headless Chrome (`applications/urban-science-workbench/tests/drive-event-grammar.mjs`, not in the suite).
- **Not built:** nonterminals across worldlines found by compressing the foam itself; a generator that grows a district's foam from the grammar by the event operator; exposure for matches of several participants; New York's record read the same way; attachment learned from more births than one district holds.
- **Provisional:** the tab's name, the states' names, and 2014 as the split.
