# 0148. A City's Municipal Solid Waste and SDG 11.6.1

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude. The survey *Evidence, methods and actions for steering cities* ranks "the Waste Wise Cities Tool's schema" next, at priority A (its Table 7.1), and names its design (§5.1) and the indicator's metadata (§A.5). The module's place in industrial ecology beside `flows` is provisional.

## 1. Context

What a city generates, collects and manages in controlled facilities is how SDG 11.6.1 is monitored. WaCT gives the steps and the survey design. The infrastructure held the pieces apart:
- peira's survey estimators;
- industrial ecology's material flow accounts;
- kybernesis's ordinal ladders.

It did not hold the account itself: the household survey's estimate with its interval, the ladder of control applied to facilities, residues counted once, and the indicator with its uncertainty.

## 2. Decision

1. **The industrial-ecology domain's `waste`** (theory note `domains/industrial-ecology/theory/municipal-solid-waste.md`):
   - generation from a stratified cluster survey by peira's estimators, with its ultimate-cluster interval, per person by stratum, and other sources declared;
   - a facility's level of control on a cumulative ladder of declared criteria, controlled from a declared level;
   - the account from generated to collected, delivered, recovered, disposed and managed in control, a disposal site weighed at its gate with residues counted once (Proposition 1), and the excess of residues over a gate reported as the imbalance;
   - SDG 11.6.1 with the survey's interval carried through a monotone map;
   - composition by weighted ratios.
2. **The API.** `science/waste.hpp` (`waste.account`), answered by the materials engine, and `science.waste` with WaCT's five level names, whose criteria are placeholders.
3. **The Urban Studio's plug-in *Materials and flows*** gains the tool *waste* and a template. Its invented city of 100,000 households has ninety households weighed in WaCT's design, four facilities and a ladder of criteria, its five files brought by the template.

## 3. Alternatives Considered

- **An interval for the facilities' amounts as well.** Weighbridge records have their own errors, but the survey's error dominates and WaCT treats facility records as measured. The module carries the survey's error alone and says so.
- **The ladder as a kybernesis scorecard.** A scorecard aggregates statements on levels. A ladder of control is the cumulative rule of Definition 1, simpler and exact, and stays in the module. If ladders multiply, they can move to kybernesis.
- **WaCT's own checklist as code.** Its criteria are a published instrument that the survey has not read in full. The level names are given, and the criteria are the caller's.

## 4. Consequences

- **Checks:**
  - the industrial-ecology domain's `waste`: 21;
  - the API's check of waste: 9;
  - the Urban Studio: one more.
- **What the checks showed:**
  - a survey of two strata of two areas reproduces its total and its ultimate-cluster variance by hand;
  - a dumpsite that compacts but is not fenced stands at the ladder's first level;
  - residues are counted once when the landfill is weighed at its gate, and residues declared beyond a gate are found;
  - on a small survey the indicator's interval can exceed one, and is reported as it is.
- **Not built:**
  - WaCT's forms read as data;
  - the collection-service ladder of households;
  - recovery by fraction against the composition;
  - repeated surveys compared;
  - waste in the GPC inventory's waste sector.
- **Provisional:** the module's place and name, and the example.
