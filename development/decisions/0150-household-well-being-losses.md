# 0150. Household Well-Being Losses after Unbreakable

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue autonomously. The survey *Evidence, methods and actions for steering cities* ranks "household well-being losses, after *Unbreakable*, on nomos's `microsimulation`, with the readings of utility, discounting and reconstruction declared" first (its §4.2, asked by its chapters C, F and G). It advises holding asset, service and well-being losses as parallel spaces, since they rank places and measures differently (its §C.2.2 and §F.7.2). The module's place in risk management is provisional.

## 1. Context

Ranked by avoided asset losses, investments in resilience favour wealthy districts. Ranked by well-being losses, they favour informal settlements. The infrastructure could count asset losses (tyche's exceedance, risk management's financing, and now the disaster-risk domain's `vulnerability`, ADR 0149), but not what they cost the people who bear them.

The model is *Unbreakable*'s:

- Hallegatte, Bangalore and Vogt-Schilb (2016), Policy Research Working Paper 7886 (P1);
- Walsh and Hallegatte (2019), Policy Research Working Paper 8723, published in *Economics of Disasters and Climate Change* in 2020 (P2).

Both papers are CC BY 3.0 IGO. The published code is GPL-3 or unlicensed, so the module is written from the papers' equations alone. A reading of the papers and the code (by an agent, every equation with its page) found their discrepancies, and the theory note settles each.

## 2. Decision

1. **Risk management's `wellbeing`** (theory note `domains/risk-management/theory/wellbeing.md`, 28 checks):
   - **Income and capital:** effective capital and the flat tax of transfers (P2 Eqs 3–6).
   - **Losses:** assets lost, and income and transfers lost, their sum Π L (Proposition 1, P2 Eq. 16).
   - **Reconstruction:** paid from consumption at λ = ln 20/N, or at each household's own best pace by P2's Eqs 22–23, slowed to a declared floor; consumption held at a hard minimum with the excess reported.
   - **Buffers:** savings and support capping the deepest part of the loss (P2 Eqs 25 and 27, Eq. 26 read as they require, Proposition 2).
   - **Support:** a payout set by a reference group's losses, with errors of exclusion and inclusion, financed by a later tax.
   - **Well-being:** the loss over the horizon; the equivalent consumption loss at national mean consumption; socioeconomic resilience, which tends to P1's 1/Γ for a small loss (Proposition 3).
   - **Return periods:** expectations by the step weights of P1's code.
2. **The API's `science/wellbeing.hpp`** (`wellbeing.assess`), answered by the Urban Studio's risk engine. A measure (faster, stronger, warning, savings, protect, support) changes the declarations, and the whole is computed again. Each measure is ranked by the asset losses and by the well-being losses it avoids.
3. **The Urban Studio's plug-in *Lifelines and evacuation*** gains the tool *well-being* and a template: an invented city of 100,000 people in three districts and five income groups, with six measures.

## 3. Alternatives Considered

- **P1's country model,** which values a loss as a change in the present value of wealth. Its results are not comparable with the household model's (P2 fn. 6). The household model is the one a city's survey can feed, so the country model is left out and said to be.
- **Hosting the module on nomos's `microsimulation`,** as the survey suggests. That module holds taxes and benefits applied to a weighted sample. The well-being model shares only the weights, and would have bent the module to its use. It stands in risk management on calx's quadrature and minimiser.
- **A common pace of reconstruction as the template's default.** On the invented city, rebuilding in three years from consumption drives seven groups of poor households to the hard minimum, where the loss is understated. Each household's own best pace, P2's choice, drives none there and is the template's default. The module's default stays the common pace, the simpler reading.
- **Spending buffers against the raw loss.** The first version did, and a measure adding savings then appeared to do harm: savings were spent against the part of a loss the hard minimum had already removed, and rebuilt at full cost. The buffer now caps the loss held below the hard minimum, with a check.

## 4. Consequences

- **Checks:**
  - risk management's `wellbeing`: 28 (the domain 55);
  - the API's check of well-being: 5;
  - the Urban Studio: one more.
- **What the checks showed:**
  - the log-utility and η = 2 series to 10⁻⁹;
  - P1's Γ for a small loss to 10⁻⁵;
  - P2's Eq. 23 vanishing at the rate found;
  - Eq. 27 holding on the buffer's cap;
  - support costing exactly what it pays;
  - the poorest fifth's share of well-being losses above its share of asset losses.
- **What the invented city shows** (every household and parameter invented):
  - a year's floods cost $1.45 m of assets and $17.4 m of well-being as national consumption, a resilience of 8%, low because its riverside households are declared fragile and poor;
  - the informal riverside bears 46% of the asset losses and 91% of the well-being losses, and the poorest fifth 22% and 67%;
  - an embankment for the waterfront saves the most assets and ranks fifth of six by well-being, where strengthening the riverside's homes ranks first;
  - when households choose their own pace, rebuilding the nation faster does almost nothing for them.
- **Not built:**
  - vulnerabilities drawn within their classes;
  - exposure calibrated to modelled asset losses;
  - the country model;
  - support's targeting from scorecards of delivery;
  - recovery observed in panel surveys;
  - a link from `vulnerability`'s losses of buildings to households' asset losses, which needs households placed among the buildings.
- **Provisional:** the module's name and place, the readings' defaults, the template.
