# 0146. Land Value Capture and Land Readjustment

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude. The survey *Learning finance scorecards and value capture* (Part IV and §6.4) gives the instruments' identities. ADR 0145 left them unbuilt. Their placement as a module of economics beside `fiscal`, not a domain of municipal finance, is provisional and the user's to revisit (CLAUDE.md §6.5).

## 1. Context

A project a city builds raises the value of the land around it, and how much of that rise the city can take back decides what it can afford to build. The instruments differ in who pays and when, and in whether they take from the rise or from new floor area:

- a betterment levy;
- development rights sold by a formula or at auction;
- tax increment;
- a developer obligation;
- land readjustment.

The literature calls three different ratios "the capture rate": cost recovered, uplift taken, and a statutory rate (the survey, Part IV §6). Tax increment takes revenue that other governments would have had (Dye and Merriman 2006). The infrastructure had no module for any of this.

## 2. Decision

1. **The economics domain's `capture`** (theory note `domains/economics/theory/land-value-capture.md`). It holds:
   - uplift as declared values with and without the project at one date, the counterfactual the caller's;
   - the instruments as streams of payments with their payers: the levy L = min(C(1 + a), λU) apportioned by uplift, area or frontage; rights by the formula (ΔA/FAR₀) p k or at auction, with authorised = sold + unsold; tax increment with the but-for share, what it diverts by jurisdiction; an obligation with its incidence;
   - cost recovery ρ, capture of the uplift κ and the statutory rate σ kept apart;
   - the parcels charged twice over named;
   - land readjustment by its area identity, contribution ratio, value kept (Proposition 3: exactly when the land returned is worth at least the land pooled), cap and feasibility;
   - replotting by calx's rectangular assignment with entitlements and settlements summing to zero (Proposition 4).
2. **The API.**
   - `science/capture.hpp` (`capture.compare`, `capture.readjust`), where a sale of rights may name the parcel it is granted on;
   - in `science.fiscal`: `parcel`, `capture_compare`, `readjust`.
3. **The Urban Studio's plug-in *Programmes and public finance*** gains the tool *capture* and a step of its template. Its example is an invented tram line through Ardel's eastern districts: eight parcels, four instruments, and a readjustment of five owners onto six plots, one of them the reserve.

## 3. Alternatives Considered

- **One "capture rate".** It would conflate what a project gets back with what share of the rise is taken. An obligation recovers cost while taking nothing from the rise, and a levy can take most of a small rise while recovering little.
- **Uplift estimated inside the module.** The estimate is a design question of its own (hedonic models, comparisons of reached and unreached places; peira's quasi-experiments and the urban-science domain's `demand` hold the methods). The module takes the counterfactual as declared and says so.
- **A jurisdiction's statute as code** (a levy's legal share, a readjustment's statutory cap). These are data a caller declares, as with ADR 0145's thresholds.
- **Replotting by value or by consent.** Nearness is the one criterion stated exactly enough to compute. The others are left to people, and the module says it minimises nearness only.

## 4. Consequences

- **Checks:**
  - economics's `capture`: 24, among them replotting equal to enumeration;
  - the API's check of programmes: 11 more;
  - the Urban Studio: one more, and the plug-in's five tools.
- **What the checks showed:**
  - a levy apportioned by uplift never charges a parcel more than its uplift (Proposition 1), and one apportioned by area can;
  - a levy and a sale of rights on one parcel together take more than its uplift, and the parcel is named;
  - a fee on new floor area recovers cost while taking nothing from the uplift.
- **Not built:**
  - uplift estimated from records;
  - auctions of rights with a schedule of issues and their secondary market;
  - incidence estimated rather than declared;
  - readjustment with owners' preferences, holdouts and a vote;
  - captured revenue carried into the years ahead of `fiscal` as capital revenue;
  - split-rate property taxes.
- **Provisional:** the module's place and name, and the example's figures.
