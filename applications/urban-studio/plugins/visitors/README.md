# Visitors: How Many, What They Spend, What a Levy Does

*Status: draft (2026-11-22; [ADR 0156](../../../../development/decisions/0156-the-visitor-stock.md), [ADR 0157](../../../../development/decisions/0157-tourism-accounts-and-levies.md)).* A plug-in of the Urban Studio: how many visitors are present in a place on an average day of each month, and from where, under every declared reading of "visitor", with intensity and seasonality against declared bands; what their spending becomes in the economy; and what a levy on their nights raises, costs and falls on.

## 1. Concepts

- **The stock.** A night spent is a visitor present for a day, so a month's nights over its days is the mean number of overnight visitors present. Over a period, nights are arrivals times the mean stay (Little's law in the returns' form).
- **Readings of "visitor".** Registered establishments only; platform rentals too; stays with friends and relatives or in second homes, which no return holds, at a declared rate per registered night; same-day visitors at a declared rate per overnight night, present for declared hours. Each gives its own stock.
- **Platform and registered nights overlap** ([ADR 0164](../../../../development/decisions/0164-platform-and-registered-nights-overlap.md)). Eurostat advises against adding them, since a rental booked through a platform may also be a registered establishment, by an amount no one measures. The platforms cover short-stay accommodation alone, so the overlap is at most the registered nights in holiday and other short-stay accommodation (NACE I552), month by month at the year's share. Every tool counts platform nights less a declared share of that largest overlap, all of it by default (a lower bound, nothing counted twice); the stock is also read with none removed (an upper bound).
- **Intensity and seasonality.** Visitors present per thousand residents (mean and peak month), nights per resident, visitors per km²; the peak month over the mean, the Gini coefficient of the stock over the year's days; the foreign and platform shares. Each is judged against a band only where someone declares one.
- **What spending becomes.** Spending a night at declared amounts, over a declared basket of products, at basic prices by a declared share, through the country's input–output table: direct value added (the tourism satellite account's measure), value added along the supply chains, imported inputs and taxes, which together are the domestic demand, and leakage abroad. The table is national, so a city's own leakage is not seen.
- **A levy.** A levy a night at the equilibrium of declared elasticities of demand and supply: the visitor's and operator's prices, their burdens (which sum to the revenue), the nights lost and those displaced, the excess burden, the value added lost, and the levy that raises most within a cap.
- **When visitors stay away.** A declared fall in visitors' spending for declared days, and lodging capacity closed, run through the country's interdependencies over the recovery by the dynamic inoperability input–output model: output and value added lost with the interdependencies and alone, the loss over an unbounded horizon (lost demand costs its Leontief total whatever the pace of recovery), and sectors ranked by inoperability and by loss, which differ.
- **The region's own table.** The spending and the shock may be read through the region's table instead of the country's, built by Flegg's location quotient from the region's employment by industry: the region's purchases from the rest of its country become a fourth leak, and its multiplier falls.
- **Two regions.** Or through a table of the region and the rest of its country, whose totals are the country's: value added and losses counted where they fall, in the city and elsewhere, with the feedback between them.
- **Jobs.** Wherever the region's employment is held, the jobs supported by the spending, directly and in all, and the job-years a shock costs, at the country's persons employed per unit of output in each group of industries.
- **Carried from a region.** Platform rentals are published by NUTS 2 region; a place within one takes them in proportion to its registered nights, a declared key whose sum over the region's places is the region's.

## 2. Tools

| Tool | What it answers |
|---|---|
| *What visitors' spending becomes (value added, imports, leakage)* | Where the spending goes (direct value added, along the chains, imports, taxes); the products it induces most with their tourism ratios; the accounts in figures |
| *A levy on visitors' nights (revenue, who bears it, what is lost)* | Revenue, the burdens of visitors and operators and the value added lost over a range of levies; the figures at the declared levy; the levy that raises most |
| *Designing a levy (bands, a cap, dual prices)* | A flat levy and a rate compared by class of accommodation, each class's levy over its price; a cap on nights as the levy that holds it and its scarcity rent; dual prices at a declared attraction |
| *When visitors stay away (losses over the recovery)* | Output and value added lost by sector over the days followed, with the interdependencies and alone, with each sector's ranks; the inoperability of the six sectors losing most, day by day |
| *Short-term rentals and housing pressure* | Nights booked and homes withdrawn under four readings, against the platforms' own nights rented out; homes withdrawn by area; hosts' concentration |
| *Visitors present, month by month (the visitor stock)* | Each place's visitors per thousand residents month by month; intensity, seasonality, origins and the registered mean stay, with verdicts against declared bands; the stock under each reading; visitors present by origin |

## 3. Templates

- *Visitors in six European city-regions, 2024*: Vienna, Berlin, Prague, the Brussels-Capital Region, Grande Lisboa and the Metropolitan City of Venice. The rates of unregistered stays and same-day visits, and the two bands, are declarations to show the tool. Then Prague's visitors' spending through Czechia's table of 2022, and a levy on its paid nights, every amount, basket, price, elasticity and share declared.

## 4. Data

The data library's `visitor_stock.py` reads Eurostat's monthly nights at tourist accommodation establishments (tour_occ_nin2m) and in platform rentals (tour_ce_omn12) by residence of the guest, annual arrivals (tour_occ_arn2), population (demo_r_pjanaggr3) and area (reg_area3), reused with the source acknowledged, into `data/cache/products/visitors/`. A case is a list of NUTS codes; months Eurostat withholds stay empty and are said to be.

## 5. Engine

`engine/main.cpp`, around the API's `science/visitors.hpp`, `science/tourism.hpp` and `science/recovery.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/visitors/`. It is asked one request a process through `science.visitors`.
