# Visitors: the Stock of Visitors Present, Its Intensity and Its Seasons

*Status: draft (ADR 0156). The module `urban::visitors`; its checks are `visitors/tests/check_visitors.cpp`.*

## 1. The Concept

A destination is judged by how many visitors are present beside its residents, when, and from where. No register holds that number: it is a stock, estimated from flows. The accommodation returns that statistical offices publish give the flows nearest to it, the nights spent by guests in a month, by the guests' country of residence.

**Definition.** A night spent is a visitor present for a day. For a month $m$ of $d_m$ days with $N_m$ nights spent, the mean stock of overnight visitors is $S_m = N_m / d_m$.

**Proposition 1 (Little's law in the returns' form).** Over a period of $D$ days whose stays all begin and end within it, with $A$ arrivals of mean stay $W$ nights, $N = A W$, so the mean stock is $S = N/D = \lambda W$ with $\lambda = A/D$ the arrivals per day.

*Proof.* Each stay contributes its nights to $N$ and its days present to the stock's integral, the same number. ∎

Stays that cross the period's ends are counted in the returns by the nights that fall within it; the identity then holds for nights, and the mean stay $N/A$ is a ratio of the period's returns, not the mean length of stays begun in it.

**Open terms.** The International Recommendations for Tourism Statistics (2008) define a visitor as a traveller outside their usual environment for less than a year, and leave the usual environment to each country. What counts is a reading:

- which accommodation counts: registered establishments (hotels, holiday and other short-stay accommodation, camping grounds), or platform rentals as well, which Eurostat publishes from four platforms' data since 2018 and as a series by NUTS 2 region;
- stays with friends and relatives and in second homes, which no return holds, at a declared rate per registered night;
- same-day visitors, who spend no night, at a declared rate per overnight night and for declared hours present.

Each reading gives its own stock: $S_m = \bigl[N^{\text{reg}}_m (1 + u) + N^{\text{plat}}_m\bigr] (1 + s\,h/24) / d_m$ with $u$ the unregistered rate, $s$ the same-day rate and $h$ the hours, the platform term present only when platforms count. None is the stock until a reading is chosen. A reading that adds a source or raises a rate adds to every month's stock.

## 2. Intensity and Seasonality

**Intensity.** The mean and the peak monthly stock per thousand residents, nights per resident, and the mean stock per km² (density).

**Seasonality.** The peak month's stock over the year's mean, between 1 and $365/31$ when every visitor comes in one month of 31 days; and the Gini coefficient of the stock over the year's days, each month's stock weighted by its days (nomos's Gini).

**Proposition 2.** If all visitors come in months holding a share $p$ of the year's days, at a constant stock, the Gini coefficient is $1 - p$.

*Proof.* The days form two groups, a share $p$ at stock $x$ and $1-p$ at nothing; $G = \sum w_i w_j |x_i - x_j| / (2\mu) = 2 p (1-p) x / (2 p x) = 1 - p$. ∎

So a year whose visitors all come in July has $G = 334/365$; a year of equal stock has $G = 0$.

**Bands.** A measure may be judged against a band someone declares (a city's threshold of nights per resident, a limit on the peak's ratio to the mean), as below, within or above it, with whose standard it is. No band in the infrastructure is any city's until a caller states it.

## 3. A Region's Nights Carried to a Place

Platform rentals are published by NUTS 2 region, and a place may be a NUTS 3 region within one (the Metropolitan City of Venice within Veneto). They are carried to it by a declared key, month by month and origin by origin: $N^{\text{plat}}_{p,m} = N^{\text{plat}}_{R,m}\, k_{p,m} / k_{R,m}$, with the key its registered nights. For places that partition the region the carried nights sum to the region's, the region's total kept as a control total. The key says that platform rentals lie where registered nights do, which may be wrong where rentals gather in old centres and hotels on the coast.

## 4. What Is Not Here

- the stock by place within a city and by hour, which needs counts, mobile aggregates or card spending joined under these totals (kinesis's `scales`, peira's small areas);
- origins by country (Eurostat's regional returns split residents from the rest only), and the concentration of origins;
- stays with friends and relatives and same-day visitors estimated from surveys, rather than declared;
- residents away, so that a de facto population could be formed;
- carrying capacity and limits of acceptable change as declared indicators with triggered actions (kybernesis).

## References

- United Nations (2010). *International Recommendations for Tourism Statistics 2008.* Studies in Methods, Series M No. 83/Rev.1. New York.
- Eurostat. Tourism statistics: tour_occ_nin2m, tour_ce_omn12, tour_occ_arn2; population demo_r_pjanaggr3; area reg_area3.
- Little, J. D. C. (1961). A proof for the queuing formula: L = λW. *Operations Research* 9(3), 383–387.
- Lerman, R. I., and Yitzhaki, S. (1989). Improving the accuracy of estimates of Gini coefficients. *Journal of Econometrics* 42(1), 43–47.
