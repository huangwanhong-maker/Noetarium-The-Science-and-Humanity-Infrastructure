# 0147. A Community's Greenhouse Gas Inventory and Its Mitigation

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude. The survey *Evidence, methods and actions for steering cities* ranks "industrial ecology: a GPC inventory" next, at priority A (its Table 7.1), and asks that abatement cost curves carry the cautions of Kesicki and Ekins (§5.6). The module's place in industrial ecology follows that table and is provisional.

## 1. Context

A city's climate work begins with its inventory and goes on to its measures. The industrial-ecology domain held what final demand causes along supply chains (`eeio`, a consumption-based footprint). It did not hold a community's inventory as the GPC structures it: sectors, scopes by the boundary, notation keys, the BASIC and BASIC+ levels, what is kept out of them and why. Nor did it hold the measures credited against the inventory.

The usual abatement cost curve credits each measure with what it saves alone. Where two measures act on one source, that counts their overlap twice (Kesicki and Ekins 2012).

## 2. Decision

1. **The industrial-ecology domain's `emissions`** (theory note `domains/industrial-ecology/theory/community-emissions.md`):
   - entries as activity × factor or a stated amount, or a notation key in place of a figure (NO, IE naming where, NE, C);
   - global warming potentials as declared sets: AR5, and AR6 with fossil and non-fossil methane told apart, so a caller must say which;
   - BASIC, BASIC+ and the territorial total as sums over cells; generation supplied to the grid and waste from outside kept out of BASIC; biogenic CO2 apart;
   - completeness with the cells that leave a level incomplete;
   - uncertainty by the IPCC's Approach 1, and key categories by its level assessment;
   - measures as scalings of what remains of an activity or a factor, on one sector or every sector;
   - the curve: each measure alone, in an order, and by its Shapley value (nomos), with the joint abatement and the overlap;
   - a pathway with growth by sector, measures ramping in, a target and a budget.

   Submodularity is proved (Proposition 2), with the interval within which any order credits a measure (Proposition 3), and checked.
2. **The API.** `science/emissions.hpp` (`emissions.inventory`, `emissions.curve`, `emissions.pathway`), answered by the materials engine, and `science.emissions`.
3. **The Urban Studio's plug-in *Materials and flows*** gains the tools *inventory* and *mitigation* and a template: an invented town of 320,000 with an inventory of 26 entries and seven measures, its two files brought by the template.

## 3. Alternatives Considered

- **Measures credited alone, as the usual curve does.** That overstates what a set of measures abates. The module reports the alone figures beside the joint abatement and the Shapley values, so the overstatement is seen.
- **One order as the truth.** Any order is one of n!, and by Proposition 3 the credit lies in an interval that the order does not choose. The Shapley value is the average over every order.
- **Measures that move demand between entries** (heat pumps, electric cars) as a kind of their own. That needs a link from one entry to another and a rule for what the moved demand emits. It is left out and said to be: a caller enters the new demand as an entry.
- **A domain of climate mitigation, or the climate-risk domain.** That would be founding a domain, which is the user's decision. The survey placed the inventory in industrial ecology.

## 4. Consequences

- **Checks:**
  - the industrial-ecology domain's `emissions`: 28;
  - the API's check of emissions: 14;
  - the Urban Studio: two.
- **What the checks showed:**
  - two measures on one source overlap by x r₁ r₂, and the curve's order moves which of them is credited with it;
  - every Shapley value lies between its measure's marginal when added last and its abatement alone;
  - plain "CH4" under AR6 is refused;
  - on the invented town, seven measures abate 0.85 Mt where crediting each alone claims 0.87, and a 40% target for 2030 is missed once activities grow.
- **Not built:**
  - the GPC's reporting tables and the Common Reporting Framework as data;
  - emission factors from a library;
  - inventories downscaled from EDGAR;
  - measures that move demand between entries;
  - a measure's cost as a schedule in time, and discounting;
  - consumption-based and territorial accounts joined to `eeio`;
  - correlated uncertainty (Approach 2 by Monte Carlo on tyche's `uncertainty`).
- **Provisional:** the module's place and name, and the example.
