# markets

*Status: draft (2026-11-11). [ADR 0092](../../development/decisions/0092-the-markets-domain-and-agora.md). A bridge at height 1 over economics and demography, C++ from the start. Its formalism, with proofs, is the theory note [`theory/markets.md`](theory/markets.md).*

It serves **use** and **research**.

## 1. Concepts

A market for goods and services is $M = (B, \mu, q;\ O, o_0;\ K)$: buyers as a finite measure space with the spend each brings, offers with an outside option (not buying), and a kernel $K$ from buyers to offers, $K(b,o)$ the share of a buyer's spend that goes to an offer. What market analysis computes is a functional of $M$: demand, share and penetration, the structure of the sellers, an offer's trade area, the overlap of two, the spend no one serves, segments, the size of the market.

- **A kernel says where it comes from:** observed (transactions), stated (a survey) or presumed (a model with declared parameters). A work draws the three apart.
- **A trade area is a density, not a polygon.** The polygons of practice are reductions of it, and are computed as such.
- **What if needs a choice rule:** a kernel for every set of offers. The rule here is the logit; Huff's model is that logit on costs in space, and Reilly's law its two-offer case.
- **Diversion, not share, says who competes with whom:** an offer's custom goes to the offers its own buyers like.
- **The domain has no map of its own.** The cost between a buyer and an outlet (minutes, metres, money) is given by a work, which computes it on urban science's `access` or on a great circle. So the domain depends on neither urban science nor geography.

*Remark.* Financial markets are not this object. Prices as a mechanism (revealed preference, Slutsky, general equilibrium) are the economics domain's planned `prices`.

## 2. Scope

| Module | Holds | Status |
|---|---|---|
| `market` | A market and its validity; demand, shares, penetration; structure through nomos (HHI, CR₄, the numbers equivalent); trade areas, their cores of least mass and their overlap; unserved spend; a market coarsened to a segmentation, and what a segmentation loses (total = within + between); the logit as a choice rule, and diversion by removal and in closed form ([`market/`](market/include/markets/market/)) | draft |
| `sizing` | A size as a tree of sums and products of data and assumptions with ranges: exact bounds, elasticities, swings; TAM, SAM, SOM as a chain; two estimates reconciled; buyers of a later year on demography's projection; derived demand on economics' input–output table ([`sizing/`](sizing/include/markets/sizing/)) | draft |
| `space` | Huff's model in its power and exponential forms; Reilly's breaking point; the nearest outlet; the reach of an outlet; a new outlet and whom it draws from, with cannibalisation; where to open, chosen greedily, with the bound of submodularity ([`space/`](space/include/markets/space/)) | draft |
| `demand` | Choice from stated and revealed data: part-worths, willingness to pay, elasticities, a simulator of shares; the parameters of Huff's model fitted (on nomos's `econometrics`, peira's `design` and `surveys`) | planned |
| `competition` | The diversion graph and its nearly closed sets; market definition (the hypothetical monopolist, Elzinga and Hogarty's flows); positioning maps; entry as a game (on nomos's `games`, morphe's graphs) | planned |
| `dynamics` | Diffusion, the life cycle, cohorts, retention and churn, lifetime value (on peira's `survival` and `cohorts`, tyche's `processes`) | planned |

## 3. Conventions

- **Spend** is a currency a period, both stated by the work; the domain carries neither.
- **Cost** is nonnegative, in the unit the work measures (Market Studio's tools: minutes on foot).
- **A share** is a part of a buyer's spend, in $[0,1]$; a row of the kernel sums to at most one, and what is left is the outside option's.
- **Ranges** are low ≤ mid ≤ high, nonnegative. A size's bounds are exact for sums and products; a size that divides or subtracts is not such a tree.
- **Sellers and offers** are indices; names are a work's.

## 4. Dependencies

- **Domains:** economics (`production`: the input–output table and the Leontief model, for derived demand) and demography (`projection`: the Leslie matrix, for buyers of a later year).
- **Foundations:** nomos (`measurement`: concentration; the sparse pairs within reach, `Reachable`), calx (verification, random streams in the checks), and morphe, physis, tyche and peira as the domains beneath require.
- Nothing here names a place: the census, the streets and the minutes between them are a work's: Market Studio's tools read them from a person's tables.

## 5. Code and Checks

Header-only C++20, each module a target `markets::<module>`.

```
node domains/markets/tests/run-tests.mjs
```

| Check | Count | Against |
|---|---|---|
| `check_markets_market` | 39 | conservation; the core against every subset of nine cells over forty markets; overlap against the total variation distance; Pythagoras and refinement; diversion by removal against the closed form |
| `check_markets_sizing` | 30 | every corner of the leaves' ranges; finite differences; the worked chain 8.64 M, 4.32 M, 345.6 k; a stable population's λ; the table's own deliveries |
| `check_markets_space` | 29 | the softmax computed apart; Reilly's classic numbers; the nearest outlet as β grows; a new outlet added to the rule; every three of ten sites |

## 6. Open Questions

1. **Attraction.** The census gives no size of an establishment, so the example of Market Studio takes every outlet to attract alike (its tools take a column of attractions where there is one). A size (seats, floor space) would enter as $A_o$; the City publishes seating for cafés as a separate dataset, not yet in the data library.
2. **Fitting.** $\alpha$, $\beta$ and the pull of not buying are declared. Fitting them needs observed or stated choices (`demand`).
3. **Quantities as distributions** in place of ranges, on tyche.
4. **Segments found, not given:** $k$-means minimises the dispersion within (theory note, §3) and is planned in tyche's `multivariate`.
5. **Rivals who respond:** where to open is a game once they do.

## 7. References

Huff (1963, 1964); Reilly (1931); Converse (1949); McFadden (1974); Nemhauser, Wolsey and Fisher (1978); Dantzig (1957); Adelman (1969). In full in the theory note.
