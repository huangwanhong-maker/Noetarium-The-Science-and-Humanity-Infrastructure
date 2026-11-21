# 0141. Evolutionary Allocation on Real Problems and Records

- **Status:** Accepted, provisionally
- **Date:** 2026-11-21
- **Decided by:** the user: "record not haven't build things. and build: the economic evaluators in the API and the Urban Studio; rules learned on the allocation domain's own problems; calibrating an economy's habits to real records; Warsaw's budget as a sequential mechanism; reading the OR-Library's benchmark files." What ADRs 0137 to 0140 left unbuilt is recorded in the design (`development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`, *What Is Not Built*), the first five built here and the rest left for later.

## 1. Context

ADRs 0137 to 0140 built evolutionary allocation in the domain and its foundations, with a closed form at the API and the Urban Studio, and checked it on cases made for the purpose. Five things stood between that and use. The economic evaluators were reachable only from C++. Rules had been learned on knapsacks, not on the domain's own compositions. An economy's habits were declared, not calibrated to any record. Warsaw's budget had been counted by rules, never searched as the sequential mechanism the survey said a rule of participatory budgeting is. The composition had never been held to the benchmark files that allocation methods are judged on.

## 2. Decision

1. **The OR-Library read** (heuresis's decision 0012; the data library's `formats/orlib.py` and `products/or_library.py`). The multidimensional knapsacks (`mknap1`, `mknapcb1`–`9`, with `mkcbres.txt`'s best values) and the generalised assignment problems (`gap1`–`gap12`, with `gapinfo`'s optima) are read in C++ and in Python. A text that ends early or runs on is refused. The OR-Library states no licence, so the files stay in the cache. The allocation domain composes each knapsack as recipients with fixed demands on capacities, and holds the composition to the published values.
2. **A participatory budget as a sequential mechanism** (the allocation domain's `evolution/budget.hpp`). Each project is a recipient with a fixed demand, its cost, on one budget; the order of grants is the genotype, decoded by walking it and funding each project that still fits. Decoding by the budget is exhaustive: every order gives a selection no project can be added to, and every such selection is some order's decoding. Utilitarian greedy is the order by approvals, and the Method of Equal Shares with its completion is the order its projects were bought in. Two declared objectives are minimised, the share of voters with nothing and the approvals not met, and the front of the kept is reported beside the rules read on the same two measures. The composition now takes **orders proposed from outside** (`Composition::with_order`, `Settings::proposed`), each credited to its source through heuresis's proposals, so the rules' own orders enter the first generation.
3. **Rules learned on the domain's own problems** (`evolution/policies.hpp`). A case is a composition with its requests fixed and the worth of where its grants end. A rule is a score over declared features of granting a recipient at a state: attributes over their mean, a request over what remains, the share remaining, and quantities of the problem's own such as a price. It is followed through the composition's grant system by heuresis's policies (its decision 0011).
4. **An economy's habits calibrated to records** (the economics domain's `ledger/calibrate.hpp`; the data library's `products/economy_habits.py`). The accounts come first, then behaviour. From Eurostat's input–output table, households' local share of their consumption and a firm's wages and domestic supplies over its output are read, not fitted. The share of its holding a household spends in a period is then matched by bisection on simulated runs to the velocity of households' money: their consumption over the money households and firms hold, the ECB's M1 for the euro area. A velocity beyond what any share reaches is refused, with the range that can be reached. What the records leave unidentified is said: the threshold of being short of money, how short households spend, what outside buys, and the money held idle, given which alone the velocity fixes the share spent. The accounts are the EU27's and the money the euro area's, since no input–output table is published for the euro area.
5. **The economic evaluators at the API and the Urban Studio.** In `science/evolve.hpp` an evaluator may be of kind `economy`, `density` or `laws`. A `laws` evaluator learns a pooled linear law from a record by least squares, with refits on its steps drawn again as the ensemble. Three operations join the API: `evolve.learn`, `evolve.calibrate` and `evolve.budget`, the last proposing the rules' orders by default. `science.allocation` declares them (`place`, `habits`, `economy`, `density`, `laws`, `learn`, `habits_record`, `calibrate`, `budget_orders`). The Urban Studio's *Allocation and justice* gains:
   - a tool that calibrates an economy's habits;
   - a tool that searches a budget as orders of its projects;
   - the case *Melbourne: a care fund in an economy of places*: each area claims A$2,000 for every person needing assistance, more than the fund holds, so the order decides who is served, and each allocation is judged by the money re-spent in each area and by a declared value to the people in need, kept apart;
   - two templates (`melbourne-economy`, `warsaw-orders`).

## 3. Alternatives Considered

- **Fitting every habit to the velocity.** One moment identifies one parameter. The accounts fix what they measure, and the rest is declared and said to be.
- **Equal Shares judged by its proportionality axioms.** Extended justified representation cannot be checked over 78,904 voters. The front is read on the two declared measures alone, and a selection beyond Equal Shares on them is not claimed to be more proportional.
- **The OR-Library's files committed as fixtures.** No licence is stated; hand-written instances carry the format in the checks.
- **Ranking the economy's kept allocations by one score.** The money re-spent and the value to people in need are different questions; the kept are compared under declared principles, and the two evaluators stand apart.

## 4. Consequences

- **Checks:**
  - heuresis: OR-Library 9 (780);
  - the economics domain: calibration 7 (48 in `ledger`);
  - the allocation domain: the OR-Library 3, budgets 8, rules 6 (138);
  - the API's allocation checks: 17 new (65);
  - the data library: the OR-Library 9;
  - the Urban Studio: three new (150); the page driven in headless Chrome (4 steps, not in the suite), two pictures in the gallery's §50.2.
- **What the checks and the records showed:**
  - **The OR-Library.** On `mknap1` the composition, which names no knapsack, finds five of seven optima and comes within 0.85% of the rest. On Chu and Beasley's 100-item problems it comes within 11.2% of the best known: the price of generality, against the specialised genetic algorithm that set those values.
  - **Rules.** On generated city cases where order matters (the worst order reaches 59.7% of the best), a linear rule learned once reaches 99.6% of held-out cases' best orders (99.8% in training). Orders searched per case reach 99.97% with a hundred evaluations each.
  - **Calibration.** On three declared places the EU27's accounts and the euro area's velocity of 2022 (0.0513 a month) are matched at a share spent of 0.084 a month. With firms holding as much idle money as households, no share reaches that velocity, and the calibration is refused: the money held idle is not in the records.
  - **Warsaw 2023** (78,904 voters, 138 projects, PLN 30.3 m; 200 generations in 62 s). The city's count, utilitarian greedy, which reproduces its selection, leaves 7.9% of voters with nothing and meets 290,845 approvals. Greedy per cost leaves 5.8% (392,518); Equal Shares 4.7% (349,558), itself an order's decoding. Orders the search kept reach 3.8% with nothing (355,392 approvals), and 4.4% with 389,106. On these two measures the city's count and Equal Shares lie behind the front, and greedy per cost is its end of the most approvals. Whether such selections keep Equal Shares' proportionality cannot be checked at this size.
  - **Melbourne in an economy of places** (13 areas, 149 households of the model, the habits calibrated to a share spent of 0.070 a month; 40 generations, 776 allocations in 213 s). Order matters: Carlton's and Kensington's grants do not commute. In the allocation that re-spends most, the A$2 m granted is re-spent A$2.13 m in the grants' own areas, summed over the rounds after it entered (LM3 1.94; 70% of the money leaves its area, mostly as firms' purchases from other areas and what the accounts send outside the economy), and meets three areas in full and a fourth in part, leaving ten re-spending less than A$100,000. The allocation that reaches most areas (nine) leaves four short, re-spends 10% less (A$1.92 m), and has the highest value to the people in need (1,170 against 1,107). Neither is the answer; four of the eight kept are undominated under the two principles. The habits are a continent's, declared for a city, and the page says so.
  - **The standing of a check an evaluator makes exactly** (the accounts' audit, the record's support) is capped at the evaluator's, by heuresis's rule that no metric is stronger than what computed it (its decision 0010). The API says so.
- **Left for later**, as the design records:
  - the stand-in inside the evolutionary loop;
  - people's comparisons kept per person;
  - the API's ask–tell over a saved state;
  - real stocks and revaluations in the ledger;
  - a measurement layer and regime-switching laws;
  - Clark and West's and Giacomini and White's tests;
  - Hoff's latent distance model;
  - quantum-like models fitted to an experiment;
  - a published heterogeneous-agent solution to hold kinesis's value equation to;
  - the generalised assignment problems composed as allocations;
  - calibration to a city's own records, which the data library does not hold.
