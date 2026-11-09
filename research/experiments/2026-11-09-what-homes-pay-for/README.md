# What Brooklyn's Home Prices Pay For

*Status: draft (2026-11-09). Research.* The first study of urban science's demand on tyche's spatial regression ([ADR 0088](../../../development/decisions/0088-what-people-pay-for.md); `domains/urban-science/demand`).

## 1. The Question and Why

A home's price is the sum of what it is and what is near it at implicit prices (Rosen 1974). The question is what the subway, a park, a protected lane and the centre add to a price in Brooklyn.

Prices near each other share what the characteristics leave out, so least squares misstates both the prices and their uncertainty. Spatial models absorb the dependence: an outcome that spills into its neighbours' (the lag model), or a shared omission (the error model). The Lagrange multiplier tests choose between them.

## 2. The Method

- **The record** (`data/products/home_prices.py`, case `brooklyn-homes`):
  - **The homes.** The NYC Department of Finance's sales of 2025 in Brooklyn: one-, two- and three-family homes above $100,000, with a floor area, a year built, and a price at least a tenth of the median. That leaves 5,473 homes, median $1.19 million.
  - **The amenities**, by straight distances:
    - to the nearest subway station (MTA);
    - to the edge of the nearest park of 1 hectare or more (NYC Parks), zero inside one;
    - whether a protected lane installed before the sale lies within 300 m (NYC DOT, ADR 0087's record);
    - to Grand Central Terminal.
- **The model.** ln price on:
  - the home's floor and land areas (logarithms), its age and age², its class;
  - the four amenities, the distances in logarithms.
- **Space.** Each home's 8 nearest others, row-standardised. Least squares and its LM tests, Anselin's rule, the lag and error models by maximum likelihood, and the lag model's impacts (tyche's spatial regression, held to spreg).
- **The samples.** The dense models take about 80 s for 2,000 homes. Three seeded samples of 2,000 are drawn at k = 8, and the first again at k = 4 and k = 12 (`run.py`).
- **The survey.** A design of 12 sets of three alternatives for a street, at priors, with the respondents each willingness to pay needs (`run.py`, the engine's `survey`):
  - the attributes: a protected lane, trees, minutes more on the way, a levy;
  - the priors: β = 0.5 for the lane; 0.3 and 0.1 for trees; −0.06 a minute; −0.05 a dollar.

## 3. What Was Found

1. **Prices carry strong spatial dependence, and the tests choose the lag model.**
   - Moran's I of the least-squares residuals is 0.37.
   - LM lag is 1,348 and LM error 1,181; robust, 201 and 34.
   - AIC is 3,079 for least squares, 2,364 for the lag model, 2,411 for the error model.
   - Every run chooses the lag, with ρ from 0.52 (k = 4) to 0.70 (k = 12).
2. **What least squares credits to amenities, the spatial models halve or more.** First sample, 95% intervals:

   | Change in a price | Least squares | Lag, direct | Lag, with spillover | Error |
   |---|---|---|---|---|
   | The subway, 10% farther | −0.9% (−1.2, −0.6) | −0.1% (−0.4, 0.1) | −0.4% | −0.1% (−0.6, 0.4) |
   | A park, 10% farther | −0.4% (−0.7, −0.1) | −0.1% (−0.4, 0.1) | −0.3% | −0.2% (−0.6, 0.3) |
   | A protected lane within 300 m | +21% (14, 29) | +7% (2, 13) | +21% | +5% (−2, 13) |
   | The centre, 10% farther | −3.8% (−4.7, −2.8) | −0.9% (−1.7, −0.1) | −2.5% | −5.2% (−7.1, −3.2) |

   - **Protected lanes.** Across the five runs, least squares gives +21% to +25%. The lag model's direct effect is +7% to +11%; the error model's is +4% to +11%, with some intervals reaching zero.
   - **The subway.** Its effect falls from about −0.9% to between −0.1% and −0.3%.
3. **What the gap says.**
   - **Where the amenities stand.** Lanes were built, and stations long ago placed, in neighbourhoods whose homes cost more for other reasons. Least squares credits the amenity with part of the neighbourhood, and the spatial models give part back to the neighbourhood.
   - **The lag model's total.** With spillover the lane's effect is +21%, near least squares'. The lag model reads the association as prices moving prices, not as a smaller premium.
   - **What neither model does.** It does not separate an amenity from an unmeasured advantage that comes with it. The lane's direct +7% is a bound on its value to buyers, not a measure of it.
4. **The survey.** At these priors, a lane is worth $10 a month and many trees $6.
   - **Respondents needed.** For a 95% interval of ±$5 a month, a lane's worth needs 7 respondents answering all 12 sets, trees' 13, a minute's 1. Twice the precision needs four times as many.
   - **The promise holds.** The design's promise is held within 15% of simulations (urban science's checks).

## 4. What It Means, and What It Does Not

- **A hedonic premium is not a willingness to pay** unless buyers sort freely and nothing left out moves with the amenity. The spatial models answer dependence, not that. The study bounds the lane's premium rather than measuring it.
- **The stated-choice survey is the complement.** It asks people directly what a street with a lane is worth, free of where lanes happen to be. The plug-in designs it and counts its respondents; its answers, once collected, are estimated by nomos's conditional logit.
- **Not done.**
  - Network distances.
  - Years beyond 2025, and repeat sales, which difference out a home's fixed advantages.
  - Instruments for where amenities were placed.
  - Sparse models of all 5,473 homes at once.

## 5. Running It

```
python data/products/home_prices.py brooklyn-homes
python applications/urban-science-workbench/engine.py build demand
python research/experiments/2026-11-09-what-homes-pay-for/run.py
python research/experiments/2026-11-09-what-homes-pay-for/analyse.py
```

`results/` holds the runs and `summary.json`. The figures are the gallery's §24.
