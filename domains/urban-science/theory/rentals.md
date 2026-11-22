# Short-Term Rentals and Housing Pressure

*Status: draft (ADR 0163). The module `urban::rentals`; its checks are `rentals/tests/check_rentals.cpp`.*

## 1. The Concept

Platform rentals add to a city's visitor stock (ADR 0156) and take homes from its residents. Neither platforms nor cities publish the nights a listing is booked; what is public is a snapshot of listings (Inside Airbnb scrapes them quarterly) with each listing's reviews, minimum stay, price, host and availability.

**The review-based model.** A listing with $R$ reviews in the last twelve months, a share $\rho$ of stays leaving a review, stays of $s$ nights and a minimum stay of $m$ nights is booked
$$n = \min\bigl(c \cdot 365,\; (R/\rho)\,\max(s, m)\bigr)$$
nights a year, the cap $c$ allowing for turnover and the owner's own use. This is the model Inside Airbnb uses after the San Francisco planning department (with $\rho = 0.5$, $s$ a city's average stay, $c = 0.7$). Each of $\rho$, $s$ and $c$ is a reading, and the nights are monotone in each: decreasing in $\rho$, non-decreasing in $s$ and $c$.

**The stay from the platforms' own statistics.** Eurostat publishes, for platforms that agreed to share data (Airbnb, Booking, Expedia Group and Tripadvisor), the stays and the nights rented out by region and month. Their ratio is the length of a stay, a measured value in place of a declared one.

**A check against the platforms' nights.** The estimated nights of one platform's listings must lie below the four platforms' nights rented out; the ratio is reported. The periods differ (the listings' last twelve months, Eurostat's year), and are said to.

## 2. Homes Withdrawn and Concentration

**Withdrawn.** An entire home is taken from residents, in a reading, when booked more than a declared number of nights a year (90 in several European cities' rules), or held by a host with at least a declared number of listings, the mark of a commercial operator. Private and shared rooms are not counted as homes withdrawn.

**Concentration.** Hosts' shares of listings give the Herfindahl–Hirschman index, the largest host's share and the ten largest hosts'; the share of listings held by hosts with more than one listing; and each area's share of the withdrawn homes.

**What the model cannot see.** Guests who do not review, reviews from long stays, listings booked off the platform, homes listed on several platforms (counted once by the platforms in Eurostat's statistics, here once per listing), and prices in the platform's local currency, not deflated.

## 3. What Is Not Here

- dwellings by area, to read withdrawn homes as a share of the housing stock;
- rents and the effect of listings on them (Garcia-López et al. 2020 for Barcelona, by instruments), which needs rents by area and time;
- other platforms' listings;
- listings over several snapshots, to follow entries and exits.

## References

- Inside Airbnb. Data assumptions and the San Francisco model. https://insideairbnb.com/data-assumptions/
- Eurostat. tour_ce_omn12, short-stay accommodation offered via online platforms.
- Garcia-López, M.-À., Jofre-Monseny, J., Martínez-Mazza, R., and Segú, M. (2020). Do short-term rental platforms affect housing markets? Evidence from Airbnb in Barcelona. *Journal of Urban Economics* 119, 103278.
