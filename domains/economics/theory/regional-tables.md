# A Region's Own Table: Location Quotients

*Status: draft (ADR 0159). The module `economics::regional`; its checks are `regional/tests/check_regional.cpp`.*

## 1. The Concept

A city's economy buys much of what its industries use from the rest of its country, and a country's table counts those purchases as domestic. Multipliers read on a national table are therefore too large for a city, and its leakage too small. Regional tables built from surveys are rare; the standard non-survey method scales the national coefficients by location quotients (Miller and Blair 2009, ch. 8).

**Location quotients.** With $E_i^r$ and $E_i$ the region's and the country's employment in industry $i$, and $E^r$, $E$ their totals:

- the simple quotient $SLQ_i = (E_i^r/E^r) / (E_i/E)$: how far the region holds its share of the country's industry $i$;
- the cross-industry quotient $CILQ_{ij} = SLQ_i / SLQ_j$: a supplier against a buyer;
- Flegg's quotient $FLQ_{ij} = CILQ_{ij}\,\lambda^*$, with $FLQ_{ii} = SLQ_i\,\lambda^*$ and $\lambda^* = [\log_2(1 + E^r/E)]^{\delta}$, which shrinks the quotients further the smaller the region (Flegg, Webber and Elliott 1995); $\delta \in [0, 1)$ is declared, 0.3 in much of the literature;
- the augmented $AFLQ_{ij} = FLQ_{ij} \log_2(1 + SLQ_j)$ where the buyer is specialised, $SLQ_j > 1$ (Flegg and Webber 2000).

**The region's coefficients.** $a^r_{ij} = a_{ij} \min(1, LQ_{ij})$. The region's output is its share of national employment in each industry times the national output, productivity taken as the country's.

## 2. What the Region Buys Elsewhere

What the scaled coefficients leave, $\sum_i (a_{ij} - a^r_{ij})\,x^r_j$, is column $j$'s purchases from the rest of the country. Imports from abroad, taxes on products and value added keep their national coefficients.

**Proposition 1.** Every column of the region's table, with its purchases from the rest of the country, sums as the country's: $\sum_i a^r_{ij} + \rho_j + (m_j + t_j + v_j)/x_j = \sum_i a_{ij} + (m_j + t_j + v_j)/x_j$, where $\rho_j = \sum_i (a_{ij} - a^r_{ij})$.

*Proof.* By construction of $\rho_j$. ∎

So the tourism accounts' identity holds for the region with a fourth leak: value added, imported inputs, taxes and inputs from the rest of the country sum to the region's domestic demand (the tourism note's Proposition 1, with $\rho$ beside $m$).

**Proposition 2.** The region's output multipliers do not exceed the country's.

*Proof.* $0 \le A^r \le A$ elementwise, so $(I - A^r)^{-1} = \sum_k (A^r)^k \le \sum_k A^k = (I - A)^{-1}$. ∎

**Households.** The share of households' use of product $i$ supplied in the region is taken as the national domestic share times $\min(1, SLQ_i)$, the rest of the domestic share from the rest of the country: a declared rule, the final-demand analogue of the simple quotient.

**What the method cannot see.** Location quotients assume the region's industries use the national recipe and buy locally whatever the region holds in proportion; cross-hauling (a region buying and selling the same product) is invisible, so leakage is underestimated where products are differentiated. A row whose local use exceeds the region's output gives a negative final use; such products are named.

## 3. What Is Not Here

- survey-based or hybrid regional tables, and interregional tables with trade between regions;
- employment by product rather than by group (Eurostat's regional employment has ten groups; each product takes its group's share);
- regional productivity and prices;
- the rest of the country's own losses from a region's shock, which an interregional table would carry back.

## References

- Flegg, A. T., Webber, C. D., and Elliott, M. V. (1995). On the appropriate use of location quotients in generating regional input–output tables. *Regional Studies* 29(6), 547–561.
- Flegg, A. T., and Webber, C. D. (2000). Regional size, regional specialization and the FLQ formula. *Regional Studies* 34(6), 563–569.
- Miller, R. E., and Blair, P. D. (2009). *Input–Output Analysis*, 2nd ed. Cambridge University Press. Ch. 8.
- Eurostat. nama_10r_3empers, employment by NUTS region and NACE group.
