# 0163. Short-Term Rentals and Housing Pressure

- **Status:** Accepted, provisionally
- **Date:** 2026-11-22
- **Decided by:** the user's request of ADR 0144, which left the priorities to Claude, and their word to continue with economics first. The survey *Evidence, methods and actions for steering cities* names "readers for platform data (Inside Airbnb within its licence) and housing-pressure indicators" among chapter H's new modules (its §H.5), and the visitor stock (ADR 0156) counts platform rentals without the homes they take.

## 1. Context

Platform rentals are a quarter of Prague's paid nights (ADR 0156). Their effect on housing is a question every touristic city asks: how many homes are let to visitors most of the year, where, and by whom. Inside Airbnb publishes quarterly snapshots of listings under CC BY 4.0; Eurostat publishes the platforms' own stays and nights rented out by region.

## 2. Decision

1. **The data library's `platform_rentals.py`** reads a city's snapshot of listings (area, room type, reviews in the last twelve months, minimum nights, price, host and the host's listings, availability, place), and `visitor_stock.py` adds the platforms' nights rented out.
2. **Urban science's `rentals`** (theory note `domains/urban-science/theory/rentals.md`, 10 checks): each listing's nights booked by the review-based model under declared readings (review rate, stay, cap); entire homes withdrawn by a declared night threshold and, as a reading, by host size; hosts' concentration; each area's listings, nights and withdrawn homes.
3. **The API's `science/rentals.hpp`** (`rentals.assess`), answered by the visitors plug-in's engine (1 check).
4. **The visitors plug-in's tool** *Short-term rentals and housing pressure*, with a stay's length from Eurostat's nights rented out over stays, and the estimate held against those nights; a step on Prague.

## 3. Alternatives Considered

- **Availability as occupancy.** A listing's blocked calendar days mix bookings and the host's own blocks; reviews are the trace guests leave, and the model's readings make its assumptions visible.
- **A declared stay.** Eurostat's platform statistics give the stay for every region they cover; a declared value remains for others.
- **Withdrawn homes as a share of dwellings.** It needs dwellings by area, which the data library does not hold for Prague; counts and each area's share are given, and the share of the stock is named as missing.

## 4. Consequences

- **What Prague shows** (11,804 listings on 20 September 2026, 9,865 entire homes; a stay of 3.15 nights from Eurostat's 2024 nights rented out over stays; half of stays reviewed; a cap of 70%):
  - 1.24 million nights booked a year, 63% of the four platforms' 1.96 million nights rented out in 2024;
  - 4,882 entire homes booked more than 90 nights a year, between 4,075 and 5,856 as the review rate goes from 0.7 to 0.3, and 8,312 when the homes of hosts with three or more listings count too;
  - Praha 1 holds 41% of the withdrawn homes; hosts with more than one listing hold 81% of listings, the ten largest 7.9%.
- **Not built:** dwellings by area; rents and the effect of listings on them; other platforms; entries and exits over snapshots.
- **Provisional:** the readings and thresholds; the snapshot's date.
