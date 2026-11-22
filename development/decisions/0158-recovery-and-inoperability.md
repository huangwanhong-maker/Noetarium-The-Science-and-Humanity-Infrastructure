# 0158. Recovery and Inoperability: Indirect Losses through an Economy's Interdependencies

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. The survey *Evidence, methods and actions for steering cities* lists "economics: adaptive regional and inoperability input–output" for indirect losses over the recovery (its §4.2, asked by its chapter F), and the day's news named the gap between direct losses and what they become.

## 1. Context

The risk modules give direct losses (buildings by `vulnerability`, households by `wellbeing`), and the visitor stock gives the visitors a shock removes. What those become through the economy's interdependencies, and how long they last, was not computed. ADR 0157 brought a country's input–output table and the domestic Leontief model into the tourism accounts.

The inoperability input–output model (Santos and Haimes 2004; Lian and Haimes 2006) is the linear core of the adaptive regional model (Hallegatte 2008): exact from Leontief's identity in its static form, with a declared pace of recovery in its dynamic one.

## 2. Decision

1. **The economics domain's `recovery`** (theory note `domains/economics/theory/recovery-and-inoperability.md`, 13 checks):
   - the interdependency matrix $A^* = \hat X^{-1} A \hat X$ and the static model $q = (I - A^*)^{-1} c^*$ (Proposition 1, Leontief's response over planned output);
   - the dynamic model with each sector's resilience from a declared recovery time, in the discrete form that recovers exactly; its stability (Proposition 2);
   - losses with the interdependencies and alone, never less with them (Proposition 3), sectors ranked by inoperability and by loss;
   - the loss over an unbounded horizon, $(I - A^*)^{-1}(C + K^{-1} q(0))$ (Proposition 4): lost demand costs its Leontief total whatever the resilience, which sets only when.
2. **The API's `science/recovery.hpp`** (`recovery.path`), reading the table as the tourism composition does, answered by the visitors plug-in's engine (1 check).
3. **The visitors plug-in** gains the tool *When visitors stay away (losses over the recovery)*, with a step on Prague.
4. **`science.engines`** passes the include directories to g++ in a response file, since the libraries' modules have grown past what a Windows command line holds.

## 3. Alternatives Considered

- **Hallegatte's ARIO in full.** Inventories, orders, adapting overproduction, rationing and reconstruction demand need parameters no open table gives, and its nonlinearity hides the identities the linear model keeps. The inoperability model is built first, and ARIO is named as what would follow.
- **Recovery times as rates.** A declared time to recover to a share is what practitioners state; the rate follows from it in the form that makes the declaration exact for a sector alone.
- **The loss over the path followed only.** Proposition 4 gives the whole, so the share a finite horizon holds is reported and a short horizon cannot pass for a small loss.

## 4. Consequences

- **What Prague shows** (Czechia's table of 2022; the visitors' spending of ADR 0157's step; a declared fall of half of it for 90 days, each sector alone recovering to 1% in 60 days):
  - 544 million euro of output lost over a year and 225 million of value added; each sector alone would lose 309 million, exactly the lost demand (Proposition 4), so the interdependencies add 235 million;
  - real estate is fourth by output lost and 23rd by inoperability, though no visitor's spending reaches it directly;
  - with 30% of lodging and food capacity closed at first as well, 731 million of output and 303 million of value added.
- **Found while building:** the compile line of an engine on many modules exceeded Windows' limit; the includes now go through a response file, for every work's engines.
- **Not built:** ARIO's inventories, rationing, adapting capacity and reconstruction; regional tables; capacity lost read from the vulnerability module's damage by sector; households' lost income joined to the well-being module.
- **Provisional:** the declared fall, closure and recovery times of the template.
