# 0156. The Visitor Stock: How Many Visitors Are Present, and When

- **Status:** Accepted, provisionally; its figures amended by [ADR 0164](0164-platform-and-registered-nights-overlap.md), which removes the overlap of platform and registered nights
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue autonomously after the push of ADR 0152–0155. The survey *Evidence, methods and actions for steering cities* ranks "urban science or a tourism module: the visitor stock … with the reading of 'visitor' declared" first (its §4.2, asked by its chapter H), and lists seasonality and intensity measures with their standards among chapter H's new modules (its §H.5).

## 1. Context

A destination is judged by how many visitors are present beside its residents, when and from where, and no register holds that number. The infrastructure had the accommodation returns of no place and computed no stock, intensity or seasonality. Eurostat publishes openly, for every NUTS 2 and many NUTS 3 regions, the nights spent at tourist accommodation establishments by month and by residence of the guest, and since 2018 the nights spent in platform rentals by month for NUTS 2 regions.

What a visitor is stays open: the International Recommendations for Tourism Statistics leave the usual environment to each country, platform rentals are counted apart from registered establishments, and stays with friends and relatives, second homes and same-day visits are in no return.

## 2. Decision

1. **The data library's `visitor_stock.py`** reads, for a declared case of NUTS codes and a year, monthly nights in registered establishments (tour_occ_nin2m) and in platform rentals (tour_ce_omn12) by residence, annual arrivals (tour_occ_arn2), population and area. A place whose platform rentals are published only for its region carries the region's figures and both regions' registered nights; the product allocates nothing.
2. **Urban science's `visitors`** (theory note `domains/urban-science/theory/visitors.md`, 14 checks):
   - **the stock** month by month and by origin under a reading of "visitor" (sources counted, an unregistered rate, a same-day rate and hours), withheld months said;
   - **intensity and seasonality**: per thousand residents, nights per resident, per km², the peak over the mean, the Gini coefficient over the year's days on nomos's Gini (Proposition 2: $1 - p$ for visitors in a share $p$ of the days), foreign and platform shares, each judged against a declared band;
   - **a region's nights carried to a place** by a declared key, summing to the region's over places that partition it.
3. **The API's `science/visitors.hpp`** (`visitors.stock`) and `science.visitors`, with four declared readings and bands read from text (5 checks).
4. **A plug-in of the Urban Studio, *Visitors, how many and when*,** with an engine of its own, the tool *Visitors present, month by month* and a template on six European city-regions; the studio's data recipe gains this product and, overdue since ADR 0151, Melbourne's transit feed.

## 3. Alternatives Considered

- **Arrivals times a mean stay.** Monthly arrivals are published nationally, not by region; nights are, and nights over days is the stock without a model of stays.
- **Rates of unregistered stays and same-day visits from the literature.** They vary by place and survey and are not in the returns; they enter as declarations at the reading's rate, so that their effect is seen and attributed.
- **Platform rentals allocated by area or population.** Registered nights place them where visitors are known to stay; the key is declared and its weakness (rentals gathering in old centres, hotels on a coast) said.
- **The stock by place and hour now.** It needs counts or mobile aggregates joined under these totals; the monthly stock is the control total such an allocation needs, and comes first.

## 4. Consequences

- **What the six city-regions show** (2024, registered establishments and platform rentals):
  - the Metropolitan City of Venice has 146 visitors present per thousand residents on an average day and 369 in August, 53.6 nights a resident, and the strongest seasons (Gini 0.431, the peak 2.52 times the mean); its platform rentals are carried from Veneto by its registered nights;
  - Prague 47 per thousand, Grande Lisboa 36, Vienna 34 (its peak in December), Berlin 25 and the Brussels-Capital Region 21 (the flattest, Gini 0.055);
  - platform rentals are 37% of counted nights in Grande Lisboa and 10% in Berlin; foreign guests 87% of nights in Prague and 45% in Berlin;
  - with stays outside the returns and same-day visitors at the declared rates, Venice's average rises from 146 to 200 per thousand.
- **Not built:** the stock by place and hour; origins by country; unregistered stays and same-day visits estimated from surveys or mobile data; residents away; limits of acceptable change with triggered actions; visitors as a population for evacuation.
- **Provisional:** the readings' rates and the template's bands; the case's six places (Budapest left out so that a chart's six inks tell them apart); the plug-in's name and colour.
