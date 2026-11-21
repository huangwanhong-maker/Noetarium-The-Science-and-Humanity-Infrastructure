# 0144. Knowledge Programmes and Scorecards

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user, before sleeping. They asked that the infrastructure be carried on where it would serve work on cities' development (decision making, multilateral relations and governance, urban science, economics, climate, architecture, street design), with the priorities and the order left to Claude: "Consider and implement such you think has high edge benefits, properly priorize implementation orders." They then asked for related literature to be surveyed where useful. The priorities followed the survey of steering cities (*Evidence, methods and actions for steering cities*, §4, "What the Infrastructure Lacks"), whose first priorities include "ordinal evaluation spaces and declared-scheme scorecards" (§4.1) and "knowledge programmes" (§4.2). A survey in four parts was made alongside (`research/literature/reports/Learning finance scorecards and value capture.md`).

## 1. Context

Programmes in which cities learn from one another, and from practice elsewhere, are a large part of how cities' development is supported. They identify needs, often with a scorecard; they bring peers together, take participants to see practice, advise, follow up; and they try to say what came of it. The infrastructure held theories of change, evidence and its grading (the policy domain), and the record of a steering with transfers between cities (kybernesis, ADR 0131). It did not hold a scorecard scored as its ordinal scale allows, nor a programme's needs, actions, results and contribution claims.

## 2. Decision

1. **kybernesis's `scorecards`** (its decision 0006). Schemes of statements scored on ordinal levels, each score with its evidence and assessor, aggregated by declared rules meaningful on that scale:
   - the weakest link with an optional "+";
   - a conversion table declared as data;
   - the lower median;
   - non-compensatory thresholds;
   - profiles of counts.

   An interval mean is computed only where the scheme declares numbers for its levels, and is marked. Comparisons go by order, and the kernel judges whether a mean's change is meaningful. Assessors' agreement is Krippendorff's ordinal α. Weaknesses come out with their evidence.
2. **The policy domain's `learning`**. A programme's participants, needs, activities, action plans, results, contribution claims and asks, read at a date:
   - the coverage of needs;
   - each action's state, and the pace of completion by Kaplan and Meier with open and dropped actions censored (peira);
   - the highest result each participant reached on a declared ladder at a sufficient grade of evidence;
   - each claim no better shown than its weakest step (the change module's Proposition 1), and substantiated only when every rival explanation is addressed (Mayne);
   - formats compared by substantiated contribution per cost, with their frontier.

   The theory note is `domains/policy/theory/knowledge-programmes.md`. The README's planned `programmes` (several interventions under a budget) is another thing and stays planned.
3. **The API.**
   - `science/scorecards.hpp`: `scorecard.score`, `scorecard.compare`, `scorecard.agreement`.
   - `science/learning.hpp`: `learning.assess`. A scorecard's weaknesses may join a participant's needs, and ISO dates are read by archeion's calendar.
   - `science.programmes` declares and asks both.
4. **The Urban Studio's plug-in *Programmes and public finance***: the tools *scorecard* and *programme* (with *finance* and *local_debt*, ADR 0145), an engine, and a template on an invented cohort of three cities.

## 3. Alternatives Considered

- **A codebook of capacity outcomes in the code.** The survey found three published typologies that overlap and differ: the CDRF of 2009, the WBI primer of 2011, and the *Art of Knowledge Exchange*. A ladder is the programme's declaration, and a mapping between ladders a declaration of its own.
- **Published scorecards as code.** A scheme's statements, levels and tables are its publisher's and change between editions, so they are data. PEFA's M2 table, which the survey transcribed and found to assume equally spaced levels, is such data.
- **Credit shared by a fitted model of contribution.** Equal shares among the formats on a substantiated chain is a declared rule that claims no more than the claims do. The survey found no head-to-head evidence on formats.

## 4. Consequences

- **Checks:**
  - kybernesis's scorecards: 24 (186);
  - the policy domain's learning: 16;
  - the API's check of programmes (shared with ADR 0145);
  - the Urban Studio: the plug-in's checks.
- **What the checks showed:**
  - a mean of levels rising from 1.5 to 2 is reversed by x ↦ x³ and found not meaningful, where the median's rise is;
  - on the invented cohort, the scorecards' weaknesses become nine needs, one of them reached by no activity;
  - two of three contribution claims are substantiated; the third is held back by a rival not addressed;
  - the peer exchange and the cheaper diagnostic workshop are the formats' frontier.
- **Not built:**
  - substantiation by independent parties recorded per claim;
  - follow-up waves with their non-response;
  - a programme's knowledge products drafted from its record with every number bound to it;
  - mappings between codebooks of outcomes;
  - published scorecards held as data with their editions.
- **Provisional:** the module's name `learning`; the plug-in's name and colour.
