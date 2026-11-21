"""Checks of the product of local government finance (data/products/local_government_finance.py), on the cache where it has been
built: the local government sector of at least twenty European countries with debt, GDP and the derived series; the primary
balance (B9 + D41PAY) / GDP, the effective interest rate interest over last year's debt, and nominal growth recomputed exactly
from the amounts; the stock-flow adjustment equal to what the identity of debt dynamics leaves; aggregates marked.

    python data/tests/check_local_government_finance.py
"""

from __future__ import annotations

import json

from checks import PRODUCTS, assert_, skip

path = PRODUCTS / "local-government-finance" / "eurostat-s1313.json"
if not path.is_file():
    skip("local government finance: the product is not in the cache (python data/products/local_government_finance.py)")
else:
    d = json.loads(path.read_text(encoding="utf-8"))
    countries = [c for c in d["countries"] if not c["aggregate"]]
    assert_("local government finance: at least twenty countries, each with ten years of debt over GDP",
            len(countries) >= 20 and all(sum(v is not None for v in c["debt_share"]) >= 10 for c in countries), len(countries))
    assert_("local government finance: aggregates are marked (EU27_2020 among them)", any(c["geo"] == "EU27_2020" and c["aggregate"] for c in d["countries"]))
    worst_pb = worst_r = worst_g = worst_sfa = 0.0
    for c in countries:
        n = len(c["years"])
        for k in range(n):
            y, b9, it, gd = c["gdp_meur"][k], c["net_lending_meur"][k], c["interest_meur"][k], c["debt_meur"][k]
            if None not in (y, b9, it) and c["primary_balance_share"][k] is not None:
                worst_pb = max(worst_pb, abs(c["primary_balance_share"][k] - (b9 + it) / y))
            if k and c["interest_rate"][k] is not None:
                worst_r = max(worst_r, abs(c["interest_rate"][k] - it / c["debt_meur"][k - 1]))
            if k and c["growth"][k] is not None:
                worst_g = max(worst_g, abs(c["growth"][k] - (y / c["gdp_meur"][k - 1] - 1)))
            if k and c["stock_flow_adjustment"][k] is not None:
                dk, d1, r, g, pb = c["debt_share"][k], c["debt_share"][k - 1], c["interest_rate"][k], c["growth"][k], c["primary_balance_share"][k]
                worst_sfa = max(worst_sfa, abs(c["stock_flow_adjustment"][k] - (dk - (d1 * (1 + r) / (1 + g) - pb))))
    assert_("local government finance: the primary balance is (B9 + D41PAY) / GDP", worst_pb < 1e-12, worst_pb)
    assert_("local government finance: the effective rate is interest over last year's debt", worst_r < 1e-12, worst_r)
    assert_("local government finance: nominal growth from GDP", worst_g < 1e-12, worst_g)
    assert_("local government finance: the stock-flow adjustment is what the identity of debt dynamics leaves", worst_sfa < 1e-12, worst_sfa)
    de = next((c for c in countries if c["geo"] == "DE"), None)
    assert_("local government finance: Germany's local debt from 2000, under a tenth of GDP",
            de is not None and de["debt_share"][de["years"].index(2000)] is not None and all(v is None or 0 < v < 0.1 for v in de["debt_share"]))
