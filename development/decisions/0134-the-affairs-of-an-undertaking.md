# 0134. The Affairs of an Undertaking

- **Status:** Accepted, provisionally
- **Date:** 2026-11-19
- **Amends:** [ADR 0132](0132-diplomacy-and-multilateral-phronimos.md), whose planned modules of the diplomacy domain this builds
- **Decided by:** the user. Asked what remained and where Multilateral Phronimos could improve, they chose "high generativity affairs" first: the domain's planned modules. Claude read the phrase as the affairs by which an undertaking changes (decisions taken by rules, negotiations, events, mandates, reviews) together with the institutions it sits among, and built them in that order.

## 1. Context

ADR 0132 built an undertaking as a typed dependency structure (`relations`) and a partnership's record (`partnerships`). It planned six modules more: `rules`, `institutions`, `events`, `negotiation`, `mandates` and `reviews`. The survey behind it ([Multilateral practice, diplomacy and partnerships](../../research/literature/reports/Multilateral%20practice,%20diplomacy%20and%20partnerships.md), §A.3–A.6 and §A.11) says what each should hold.

- **Decision rules** are simple games, and their open terms decide outcomes: consensus as "no formal objection among those present", "present and voting", "concurring votes".
- **Negotiation** is two-level (Putnam 1988): every party's win-set is what its constituency would ratify.
- **Machine-coded events** count reports, not events.
- **Mandates** accumulate beyond review (A/80/318).
- **Peer review** is a loop of recommendation, response, implementation and evidence.
- **Institutions** form regime complexes, and multilateralism asks diffuse reciprocity (Ruggie 1992).

These are the generative parts of an undertaking. Each is a record of acts in time, and what follows from it (an outcome, a narrower zone, a lineage, a repetition) is read from the record, never assumed.

## 2. Decision

1. **Six modules of the diplomacy domain**, C++ from the start, each held to answers known by construction, in closed form or by brute force (102 checks; the domain 183):
   - **`rules`** (28 checks):
     - a body's rule as its instrument states it: thresholds as exact fractions or numbers of members or weights, blocking minorities, concurrences, consensus, quorums, fallbacks;
     - its open terms declared with readings, each the set of acts it covers and its source, read by tekmerion's construal, so that an authority's reading sets aside an analyst's;
     - a question's outcome on a record of acts: adopted, not adopted, turning on the readings, or turning on the members not recorded, who are never taken to have abstained;
     - the tally in words, the members decisive ex post, a recorded outcome the rule does not carry, and the simple game the rule makes, whose power is nomos's.
   - **`negotiation`** (17 checks):
     - issues with options in brackets, packages, values and reservations as intervals, ratifiers at home deciding by a `rules` rule;
     - each package clear, arguable or untenable for every party and for agreement, the conjunctions exact (the theory note, Proposition 8.2);
     - the drafts in order: what each settles and reopens, and when the zone empties;
     - linkage, which at one rate creates nothing (Proposition 8.4), and value claimed by nomos's bargaining on a declared basis;
     - a tighter ratification rule never enlarging a win-set (Proposition 8.3).
   - **`events`** (12 checks):
     - events in a declared vocabulary (PLOVER's by default, classed after CAMEO, a declared reading) as the reports they rest on;
     - reports relaying one origin counted once by tekmerion's dependence, with what counting them apart would give;
     - coincident records reported and never merged;
     - a pair's series, each event once, and the signs of thin evidence.
   - **`mandates`** (20 checks):
     - lifecycles: a mandate's state at a date, and renewals, amendments, supersessions, consolidations and terminations as an acyclic relation (morphe), with a mandate's lineage in it;
     - reviews due and overdue, overlaps, each addressee's load against what is resourced, and creep;
     - authority and addressees read against an undertaking.
   - **`reviews`** (12 checks):
     - recommendations, responses and implementation across cycles;
     - a claim of implementation by the party reviewed alone kept apart from one evidenced by another, and from one contested;
     - repeats, measures by response and state, and the signs of a loop not closing, mutual mildness among them.
   - **`institutions`** (13 checks):
     - memberships at a date as a formal context: the lattice of groups and the canonical basis of regularities (morphe's concepts);
     - regime complexes on an issue, nested, overlapping or apart, and the parties that can choose a forum;
     - design as declared;
     - reciprocity over windows, the group's imbalance never above the pairs' (Proposition 10.4).
2. **The theory note** gains four sections (§7–§10), each result proved: decision rules and their monotonicity, negotiation at two levels, events, mandates and reviews, institutions and reciprocity.
3. **The API's `science/affairs.hpp`** composes the six modules and answers through `science::multilateral::answer`: `outcome`, `rule_game`, `negotiation`, `events`, `mandates`, `reviews` and `institutions`. **`science.multilateral`** gains `DECLARED`, three rules as their instruments state them: Article 27(3) with the Court's reading of "concurring votes", Article 18(2) with rule 86, and the WTO's consensus with its fallback.
4. **The visualization layer's `multilateral/affairs.js`** draws each affair where nothing is told by colour alone:
   - a question as a hemicycle of seats with the decisive ringed, its clauses, and its readings with what each gives;
   - the talks as a field of packages closed on by each draft's frame;
   - a pair's events with the reports' ghost where they count more;
   - mandates in time by lineage;
   - review cycles with repeats threaded;
   - the lattice of memberships, and reciprocity.
5. **Multilateral Phronimos** draws each affair where its subject is:
   - questions, mandates and reviews on governance;
   - the talks opening the negotiation;
   - a pair's events beneath the round;
   - a fifth view, Institutions.

   The record (`noetarium.multilateral/1`) gains `declared`, `questions`, `talks`, `events` and `channels`, `mandates`, `reviews` and `institutions`. The negotiation view's own bargaining problem keeps its key, `negotiation`. Every affair is read apart, and one the engine refuses is said so without hiding the others. The invented Arda Basin Partnership gains an invented record of each.

## 3. Alternatives Considered

- **Rules as nomos's simple games only.** Rejected: a game knows coalitions, not acts. That a permanent member abstained, that a member was absent, that a statement joined a consensus with a reservation, and how a phrase is read, all decide outcomes before any coalition is formed.
- **Unrecorded members read as absent.** Rejected: absence is an act, and not recording is the absence of an observation (ADR 0132's first distinction). The outcome is read under every act the procedure admits, and said to turn on them when it does.
- **One value per party per package.** Rejected: positions are seldom known as numbers. Intervals keep what the evidence allows, and the conjunctions over independent actors are exact.
- **Merging coincident event records.** Rejected: two records are two manifestations, and their coincidence is reported for a person to judge.
- **A score of a partnership's reviews or a mandate's performance.** Rejected, as ADR 0132 rejected a composite index: counts and shares by state, each said.

## 4. Consequences

- **The domain** declares tekmerion. Its down-set is unchanged, since tekmerion was already beneath it through kybernesis. It passes alone with its down-set.
- **Checks:**
  - the API's multilateral checks rise to 33;
  - the visualization layer's to 874, 19 of them the affairs' drawings;
  - the work's suite to 41;
  - the drive to 27 steps, not in the suite.

  Six pictures are in the gallery's §49.1.
- **What the invented basin shows:**
  - the first tranche turns on whether the Bank concurred by abstaining, and the body recorded it adopted all the same;
  - of the dry-season talks' 25 packages one can be agreed, and only arguably, turning on the Delta parliament's reading of 40%; a draft empties the zone and the final text finds it again;
  - twelve outlets relaying one wire are one origin, overstated 2.9 million times if counted apart;
  - the monitoring renewed has crept to the sediment without resources, and a flood warning was asked by the fishers, who hold no authority recorded;
  - the releases claimed published by the Upland Republic alone are recommended again;
  - what the parties gave balances for the group over two years but seldom pair by pair.
- **Not built:**
  - readings of a rule's terms that depend on one another;
  - values of combinations of issues (the modules assume separability, and say so);
  - channels estimated from corrections and retractions, and chains of copying among sources;
  - hyperedges for statements on behalf of a group;
  - dominators for succession;
  - the data library's readers for UPR Info, PLOVER, the UN Treaty Collection and COW memberships;
  - the record sealed in archeion's journal;
  - transfer between undertakings.
- **Provisional:** the record's new keys, the drawings' names, and the reading of "high generativity affairs".
