# Tourism Accounts and Levies

*Status: draft (ADR 0157). The module `economics::tourism`; its checks are `tourism/tests/check_tourism.cpp`.*

## 1. What Visitors' Spending Becomes

**The tourism satellite account's question.** Tourism is not an industry in the national accounts: it is a demand, met by many industries. The Tourism Satellite Account (TSA: Recommended Methodological Framework 2008) measures it by the visitors' consumption of each product, and the direct value added of tourism as the value added of the output that visitors buy, industry by industry (the tourism ratio times the industry's value added).

**Demand.** Visitors' spending a night by product, at purchasers' prices, times nights by origin, is carried to basic prices by a declared share $\beta$ (the taxes on the final purchase and the trade margins, which a full TSA reallocates product by product). Each product's share of households' use supplied at home, $\sigma_i$ (from the input–output table's domestic and total use by households), splits demand $d$ into domestic demand $d^{D}_i = \sigma_i d_i$ and imports $(1-\sigma_i) d_i$.

**The chain.** On a symmetric table of $n$ products at basic prices with domestic flows $Z^D$ and outputs $x$, the domestic coefficients $A = Z^D \hat x^{-1}$ and the Leontief inverse $L = (I - A)^{-1}$ (economics' `production`) give the output that domestic demand calls for: $x^T = L d^D$. Each column $j$ of the table is completed by its value added $v_j$, net taxes on products $t_j$ and imported inputs $m_j$, so that
$$\sum_i a_{ij} + \frac{v_j + t_j + m_j}{x_j} = 1. \tag{1}$$

- Direct value added $V^{dir} = \sum_i (v_i / x_i)\, d^D_i$.
- Total value added $V = \sum_i (v_i / x_i)\, x^T_i$; indirect value added $V - V^{dir}$.
- Induced imported inputs $M = \sum_i (m_i/x_i)\, x^T_i$ and taxes $T = \sum_i (t_i/x_i)\, x^T_i$.

**Proposition 1.** $V + M + T = \mathbf 1^\top d^D$.

*Proof.* By (1), $\mathbf 1^\top A + (v + t + m)^\top \hat x^{-1} = \mathbf 1^\top$. Multiply on the right by $x^T$: $\mathbf 1^\top A x^T + (V + T + M) = \mathbf 1^\top x^T$, and $x^T - A x^T = d^D$. ∎

So nothing is created by the chain: a euro of domestic demand ends as domestic value added, imported inputs or product taxes, and leakage, $\bigl[\mathbf 1^\top (d - d^D) + M\bigr] / \mathbf 1^\top d$, is what goes abroad. The value-added multiplier $V / \mathbf 1^\top d^D$ is below one.

**Remarks.**

- A published table balances only to its rounding; the residual $\mathbf 1^\top d^D - V - M - T$ is reported as unattributed.
- A country's table is used for a city. A city imports more from the rest of its country than the country does from abroad, so its own leakage is larger and its multiplier smaller. Regionalising the coefficients (location quotients, Flegg's FLQ) is left for a later change and said to be.
- Products a country's table leaves empty (Czechia records retail with wholesale) take the spending of the products they record, as declared by the caller.

## 2. A Levy on Nights

**The market.** Nights $Q$ at a price $P_0$, with constant elasticities: demand $Q_d(p) = Q_0 (p/P_0)^{\varepsilon_d}$ in the visitor's price ($\varepsilon_d < 0$), supply $Q_s(p) = Q_0 (p/P_0)^{\varepsilon_s}$ in the operator's ($\varepsilon_s > 0$, infinite when the levy is passed on in full). A levy $t$ a night sets the visitor's price $p_c$ and the operator's $p_p = p_c - t$ where $Q_d(p_c) = Q_s(p_p)$.

- **Flat.** $\varepsilon_d \ln(p_c/P_0) = \varepsilon_s \ln((p_c - t)/P_0)$ has one root in $(t, P_0 + t]$, since the left side less the right decreases in $p_c$ and changes sign there; it is found by Brent's method (calx).
- **A rate $r$ on the operator's price.** $p_c = (1+r)\,p_p$ gives $p_p = P_0 (1+r)^{\varepsilon_d/(\varepsilon_s - \varepsilon_d)}$ in closed form.

**Proposition 2.**

1. The visitors' and the operators' burdens, $(p_c - P_0)Q$ and $(P_0 - p_p)Q$, sum to the revenue $tQ$.
2. For a small levy the visitor's share $(p_c - P_0)/t \to \varepsilon_s/(\varepsilon_s - \varepsilon_d)$.
3. The excess burden, the consumer and producer surpluses lost beyond the revenue, $\int_{P_0}^{p_c} Q_d + \int_{p_p}^{P_0} Q_s - tQ$, is not negative and, to second order, $\tfrac12 t\,\Delta Q$ (Harberger).

*Proof.* (1) is $p_c - p_p = t$. (2) Linearise the equilibrium: $\varepsilon_d\,\delta p_c / P_0 = \varepsilon_s (\delta p_c - t)/P_0$. (3) The integrals are the areas under the curves between the prices, each bounded below by the rectangle at the new quantity, which together make the revenue; the remainder is the triangle to second order. ∎

The surpluses have closed forms, $\int_a^b Q_0 (p/P_0)^{\varepsilon} dp = \frac{Q_0 P_0}{\varepsilon + 1}\bigl[(b/P_0)^{\varepsilon+1} - (a/P_0)^{\varepsilon+1}\bigr]$ (a logarithm at $\varepsilon = -1$).

**The levy that raises most.** Passed on in full, revenue $t Q_0 ((P_0+t)/P_0)^{\varepsilon_d}$ is greatest at $t^* = P_0/(|\varepsilon_d| - 1)$ when demand is elastic, and rises without bound when it is not; the module searches a declared range and says when the most lies at its end.

**Displacement and what is lost.** A declared share of the nights lost moves to stays the levy does not reach (stays with friends, unregistered rentals); the rest, at the value added a night that §1 gives, is the value added lost. A levy's revenue against the value added lost is a comparison of a transfer with a loss, and both are reported apart.

## 3. What Is Not Here

- a full TSA: products' trade margins and taxes reallocated, internal and inbound consumption apart, employment and the TSA's ten tables;
- regionalised coefficients for a city;
- the levy's incidence on residents (through prices of shared services) and its use (what the revenue pays for);
- levies by band and dual prices for residents and visitors, caps on nights or on visitors, and their combination with the visitor stock's seasons;
- elasticities estimated from records (their declaration here is a reading, as are the price and the displaced share).

## References

- United Nations, Eurostat, OECD and UNWTO (2010). *Tourism Satellite Account: Recommended Methodological Framework 2008.* Studies in Methods, Series F No. 80/Rev.1.
- Miller, R. E., and Blair, P. D. (2009). *Input–Output Analysis: Foundations and Extensions*, 2nd ed. Cambridge University Press. Ch. 2, 3.
- Flegg, A. T., Webber, C. D., and Elliott, M. V. (1995). On the appropriate use of location quotients in generating regional input–output tables. *Regional Studies* 29(6), 547–561.
- Harberger, A. C. (1964). The measurement of waste. *American Economic Review* 54(3), 58–76.
- Salanié, B. (2011). *The Economics of Taxation*, 2nd ed. MIT Press. Ch. 4 (tax incidence).
- Eurostat. naio_10_cp1750, symmetric input–output table at basic prices (product by product).
