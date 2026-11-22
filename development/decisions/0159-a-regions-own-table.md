# 0159. A Region's Own Table: Location Quotients for a City's Accounts and Recovery

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. ADR 0157 and ADR 0158 each named regionalised coefficients as not built: on a national table a city's own leakage is not seen, and its multipliers are too large.

## 1. Context

The tourism accounts (ADR 0157) and the recovery paths (ADR 0158) read a country's input–output table for a city. A city buys much of what its industries use from the rest of its country, which a national table counts as domestic. No open regional tables exist for most European cities, but Eurostat publishes employment by NUTS region in ten industry groups, which is what the standard non-survey methods, location quotients, need.

## 2. Decision

1. **The economics domain's `regional`** (theory note `domains/economics/theory/regional-tables.md`, 12 checks): a region's table from its country's by SLQ, CILQ, Flegg's FLQ (δ declared) or the augmented AFLQ; its output at the country's productivity; each column's purchases from the rest of the country, so that every column sums as the country's (Proposition 1); regional multipliers never above the national (Proposition 2); households' local shares by min(1, SLQ).
2. **The tourism accounts gain a fourth leak:** `economics::tourism::Accounts` takes a region's purchases from the rest of the country and households' share of them, and the identity holds with them; national tables are unchanged.
3. **The data library's `regional_employment.py`** reads nama_10r_3empers for a region and its country into ten groups that partition activities, mapped to the 65 products.
4. **The API's `science/regional.hpp`** (`regional.table`) answers a region's table in the form the tourism and recovery compositions read (1 check).
5. **The visitors plug-in's tools** *What visitors' spending becomes* and *When visitors stay away* take the country's table or the region's (FLQ); the template gains a step reading Prague's spending through Prague's own table.

## 3. Alternatives Considered

- **The simple quotient alone.** It ignores the region's size and overstates local purchases for a small region; FLQ is the method the literature favours, with SLQ, CILQ and AFLQ kept as readings.
- **Gravity-based interregional tables.** They need flows between regions that no open source gives for cities; location quotients need only employment.
- **Regional productivity.** Output per worker differs between Prague and the rest of Czechia, but regional output by product is not published; the country's productivity is used and said to be.

## 4. Consequences

- **What Prague shows** (Czechia's table of 2022; Prague's employment of 2022, 17.7% of the country's; FLQ with δ 0.3, λ* = 0.65):
  - Prague's visitors' spending becomes the same 1,063 million euro of value added directly, since Prague holds the products visitors buy, but 407 million along the chains in Prague against 766 million in Czechia; 0.59 of value added per euro of domestic demand against 0.73; 17.1% leaks abroad and 22.1% to the rest of the country;
  - half the visitors' spending lost for 90 days costs Prague 423 million euro of output (181 million of value added) against Czechia's 544 million: the rest falls on the rest of the country, which an interregional table would follow.
- **Not built:** interregional tables; employment by product; regional productivity; the rest of the country's losses from a region's shock.
- **Provisional:** δ, the households' rule, and the default of the country's table in the tools.
