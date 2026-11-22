# 0153. What a City's Waste Emits, Carried into Its Inventory

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's word to continue designing and implementing while they slept. Their news piece on the day's work named the link as missing: "the waste account is not yet connected to the emissions inventory". ADR 0148's "Not built" listed it too ("waste in the GPC inventory's waste sector").

## 1. Context

Two modules of the industrial-ecology domain stood apart:

- **`waste` (ADR 0148):** what a city generates, collects and manages in control;
- **`emissions` (ADR 0147):** an inventory whose waste sector was entered by hand as amounts.

The IPCC's methods join them. For disposal sites, first-order decay is the method the GPC recommends; biological treatment and open burning have factors of their own. The defaults had to come from the Guidelines, not from memory. An agent read the 2006 Guidelines' Volume 5, the 2019 Refinement and the GPC's Chapter 8, citing every table and page, and found where the GPC's own equations depart from the IPCC's.

## 2. Decision

1. **The industrial-ecology domain's `waste` gains `treatment.hpp`** (theory note `domains/industrial-ecology/theory/municipal-solid-waste.md` §5, 21 checks).
   - **Defaults:** the 2006 Guidelines' and the 2019 Refinement's, as declared sets with their sources (Table 2.4; Tables 3.0–3.3; Table 4.1; Tables 5.2 and 5.6).
   - **Disposal:** first-order decay by component with the reaction's delay (Eqs 3.1–3.6 and 3A1.12–15), recovery subtracted before oxidation.
   - **Biological treatment and open burning:** composting and digestion by their factors; open burning's fossil and biogenic CO₂, CH₄ and N₂O.
   - **The inventory:** each treatment's gases written as the inventory's entries by the GPC's codes and scopes, waste brought in kept outside BASIC.
   - **Dependencies:** `waste` now uses `emissions`.
2. **The API's `waste.emissions`.** It answers each site's methane year by year, each treatment's gases, the entries and their totals.
3. **The Urban Studio's *Materials and flows*** gains the tool *waste emissions*, and the waste template a file and a step.
   - **The account:** it reads the waste account's facilities with their treatment.
   - **Each disposal site:** its history is declared from the year it opened, at the account's amount grown back at a declared rate.
   - **Uncollected waste:** a declared share is burnt in the open.
   - **The output:** the entries come out in the inventory tool's own columns, so that they can be added to a city's inventory.

## 3. Alternatives Considered

- **The methane commitment method.** The GPC accepts it, and says it "will consistently overstate" for most cities. First-order decay is what the GPC recommends, and what the IPCC requires.
- **The GPC's own equations.** They take DOCf 0.6 where the IPCC takes 0.5, start decay in the year of disposal, and misplace a factor of 10³ in one line. The IPCC's are followed, and the note says where the GPC differs.
- **A default fraction of waste burnt.** Neither the 2006 Guidelines nor the Refinement gives one, so it is declared.
- **The bridge in the API, outside the domain.** The GPC's codes and scopes for waste are the domain's knowledge, so the bridge lives in the domain, and `waste` declares `emissions`.

## 4. Consequences

- **Checks:**
  - the industrial-ecology domain's treatment: 21 (the domain 174);
  - the API's check of waste: 2 more (11);
  - the Urban Studio: one more.
- **What the checks showed:**
  - the Annex's Table 3A1.1 reproduced (9.5 and 18.1);
  - nothing decays in the year of deposition at the default delay;
  - recovery before oxidation;
  - composting by Table 4.1, and open burning's fossil and biogenic CO₂;
  - the 2019 Refinement's DOCf raising food's methane by 0.7/0.5;
  - BASIC counting the city's waste inside and out, not a neighbour's.
- **What the invented city shows:**
  - its waste emits 63,145 t CO₂e within BASIC by the 2006 defaults: the landfill 1,639 t CH₄ and the dumpsite 558 t, as their deposits since 2008 and 1996 decay, composting 26 t CH₄, and burning 30% of the uncollected waste 1,275 t of fossil CO₂;
  - by the 2019 Refinement, 81,149 t: the edition alone moves the waste sector by 29%.
- **Not built:**
  - wastewater (III.4);
  - incineration with its technologies;
  - the IPCC Waste Model's assignment of nappies and rubber;
  - sites shared between cities by their historical shares;
  - histories from a site's capacity or waste in place;
  - an uncertainty for the decay's parameters.
- **Provisional:** the header's and the tool's names, the template's declarations.
