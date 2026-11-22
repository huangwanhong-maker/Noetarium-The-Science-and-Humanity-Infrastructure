# Visitors, How Many and When

*Status: draft (2026-11-22; [ADR 0156](../../../../development/decisions/0156-the-visitor-stock.md)).* A plug-in of the Urban Studio: how many visitors are present in a place on an average day of each month, and from where, under every declared reading of "visitor", with intensity and seasonality against declared bands.

## 1. Concepts

- **The stock.** A night spent is a visitor present for a day, so a month's nights over its days is the mean number of overnight visitors present. Over a period, nights are arrivals times the mean stay (Little's law in the returns' form).
- **Readings of "visitor".** Registered establishments only; platform rentals too; stays with friends and relatives or in second homes, which no return holds, at a declared rate per registered night; same-day visitors at a declared rate per overnight night, present for declared hours. Each gives its own stock.
- **Intensity and seasonality.** Visitors present per thousand residents (mean and peak month), nights per resident, visitors per km²; the peak month over the mean, the Gini coefficient of the stock over the year's days; the foreign and platform shares. Each is judged against a band only where someone declares one.
- **Carried from a region.** Platform rentals are published by NUTS 2 region; a place within one takes them in proportion to its registered nights, a declared key whose sum over the region's places is the region's.

## 2. Tools

| Tool | What it answers |
|---|---|
| *Visitors present, month by month (the visitor stock)* | Each place's visitors per thousand residents month by month; intensity, seasonality, origins and the registered mean stay, with verdicts against declared bands; the stock under each reading; visitors present by origin |

## 3. Templates

- *Visitors in six European city-regions, 2024*: Vienna, Berlin, Prague, the Brussels-Capital Region, Grande Lisboa and the Metropolitan City of Venice. The rates of unregistered stays and same-day visits, and the two bands, are declarations to show the tool.

## 4. Data

The data library's `visitor_stock.py` reads Eurostat's monthly nights at tourist accommodation establishments (tour_occ_nin2m) and in platform rentals (tour_ce_omn12) by residence of the guest, annual arrivals (tour_occ_arn2), population (demo_r_pjanaggr3) and area (reg_area3), reused with the source acknowledged, into `data/cache/products/visitors/`. A case is a list of NUTS codes; months Eurostat withholds stay empty and are said to be.

## 5. Engine

`engine/main.cpp`, around the API's `science/visitors.hpp`, is built by the Urban Studio's `engine.py` into `data/cache/engines/urban-studio/visitors/`. It is asked one request a process through `science.visitors`.
