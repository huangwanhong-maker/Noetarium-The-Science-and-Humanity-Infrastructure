# risk-management

*Status: draft (2026-11-18; [ADR 0129](../../development/decisions/0129-risk-management-urban-risk-and-disaster-risk.md)). Field, height 0, on the foundations alone; C++ from the start.* Risk as what a system may fail to give, and how its loss is met: the most general of the infrastructure's domains of risk, beneath urban risk management and disaster risk, and, planned, climate risk.

## 1. Purpose

- **Risk is lost service, weighed by its states.** A system (a road network, a water network, a hospital's catchment, a budget) gives a service; a hazard puts its components in states; the loss is the service given up, and its expectation over the hazard's states is the risk. The domain asks of each component what losing it would add and what protecting it would save, the importance measures of probabilistic risk assessment read for a loss.
- **A loss is paid by someone.** What is not arranged before an event is paid after it, at a cost. The domain gives each source of funds a layer of the loss, says what each costs on its declared terms, compares strategies and chooses a tower of them, and measures how far a payout on an index is from the loss.
- **Nothing in it names a city, a hazard or a currency.** Those come from the domains above it and from the works: urban risk management states the services of a city's networks, disaster risk the states a flood or a tsunami puts them in.
- **Every cost and threshold is a declaration.** No premium multiple, interest rate or cost of unfunded loss in it is any institution's; a work states them, and the answers say how much turns on them.

## 2. Scope

| Module | Scope | Status |
|---|---|---|
| `criticality` | A system of components with states in [0, 1] and a loss stated by its caller; scenarios with weights (rates, probabilities, or the weights that integrate over a curve of return periods); each component forced to failure and to the intact state in every scenario: criticality, risk, Birnbaum's importance, RAW, RRW, Fussell–Vesely, and the criticality within each scenario; a system that answers every failure of one state at once | draft |
| `financing` | Instruments as layers of each event's loss (a budget, a reserve, a contingent credit, insurance, a catastrophe bond) with their declared costs a year and at each event; what is unfunded costed by tiers of the amount; strategies costed in expectation and in the year of the T-year event, compared with leaving everything unfunded; the cheapest tower from candidate boundaries, with a cap on the unfunded part of a given event; parametric cover fitted to an indemnity layer, with its basis risk and premium | draft |
| `wellbeing` | Households' losses of assets, income and transfers in an event, reconstruction paid from consumption at a common rate or each household's best (P2 Eqs 22–23), slowed to a floor, held at a hard minimum; savings and support capping the deepest part (P2 Eqs 25–27), support financed by a later tax; the well-being loss over the horizon, the equivalent consumption loss at national mean consumption and socioeconomic resilience, by group and household, over return periods by the step weights; after *Unbreakable*'s two working papers (ADR 0150; theory note `theory/wellbeing.md`) | draft |
| `register` | Risks as a register: owners, likelihoods and consequences on declared scales, treatments and their residual risk | planned |

## 3. Dependencies

| Source | What risk management uses from it |
|---|---|
| tyche | `descriptive`: the average annual loss and exceedance curves (its decision 0012), layers of a loss and basis risk (its decision 0029) |
| calx | verification |
| morphe, physis | declared as tyche's order requires; not used directly |

## 4. Used By

- **urban-risk-management**, which states the services of a city's networks as losses and asks which links matter most.
- **disaster-risk**, through urban risk management: the scenarios of a hazard's return periods, and the financing of a city's catalogue of losses.
- **climate-risk**, planned.
- **The API's `science/risk.hpp`** and `science.risk`, which compose the three domains for any work: the Urban Studio's and the Urban Science Workbench's plug-ins *Lifelines and evacuation*.

## 5. Open Questions

- **Dependent failures.** Components are forced one at a time; the criticality of pairs and the minimal cut sets of a loss are not computed.
- **Aggregate covers.** Financing is per occurrence, as an event-loss table without years allows; reserves depleted over a year, reinstatements and the timing of payouts need the years.
- **Costs that move.** The costs of instruments are constants; their dependence on the market's cycle and on the size of the cover is a declaration of a work.

## 6. References

- Birnbaum, Z. W. (1969), On the importance of different components in a multicomponent system, in *Multivariate Analysis II*, ed. P. R. Krishnaiah, Academic Press, 581–592.
- Vesely, W. E., Davis, T. C., Denning, R. S. and Saltos, N. (1983), *Measures of Risk Importance and Their Applications*, NUREG/CR-3385.
- Cheok, M. C., Parry, G. W. and Sherry, R. R. (1998), Use of importance measures in risk-informed regulatory applications, *Reliability Engineering and System Safety* 60, 213–226.
- Hallegatte, S., Rentschler, J. and Rozenberg, J. (2019), *Lifelines: The Resilient Infrastructure Opportunity*, World Bank.
- Mahul, O. and Ghesquiere, F. (2010), *Financial Protection of the State against Natural Disasters: A Primer*, Policy Research Working Paper 5429, World Bank.
- Clarke, D. J., Mahul, O., Poulter, R. and Teh, T.-L. (2017), Evaluating sovereign disaster risk finance strategies: a framework, *The Geneva Papers on Risk and Insurance* 42, 565–584.
- Klugman, S. A., Panjer, H. H. and Willmot, G. E. (2012), *Loss Models: From Data to Decisions*, 4th ed., Wiley.
