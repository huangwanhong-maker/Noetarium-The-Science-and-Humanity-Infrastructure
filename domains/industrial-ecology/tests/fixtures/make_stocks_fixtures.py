"""Reference values for the dynamic stock model, computed independently with numpy and scipy.

ODYM's convention (Pauliuk and Heeren 2020): a cohort entering in year c holds I(c) sf(t - c) at the
end of year t, sf the survival function of the lifetime law at whole ages 0, 1, 2, ...; here a normal
law of mean 25 and standard deviation 8, not truncated (scipy.stats.norm.sf), and an inflow
I(t) = 100 + 40 sin(t / 5) + 2 t over forty years. Written to stocks_normal.txt as "t I S O".

Run with the repository's .venv:  .venv/Scripts/python domains/industrial-ecology/tests/fixtures/make_stocks_fixtures.py
(numpy 2 and scipy 1.14 or later; generated 2026-11-13).
"""
from pathlib import Path

import numpy as np
from scipy.stats import norm

T = 40
t = np.arange(T)
inflow = 100 + 40 * np.sin(t / 5) + 2 * t
sf = np.zeros((T, T))  # sf[t, c]
for c in range(T):
    sf[c:, c] = norm.sf(np.arange(0, T - c), loc=25, scale=8)
s_c = sf * inflow[np.newaxis, :]
stock = s_c.sum(axis=1)
o_c = np.zeros((T, T))
o_c[1:, :] = -np.diff(s_c, axis=0)
o_c[np.diag_indices(T)] = inflow - np.diag(s_c)
outflow = o_c.sum(axis=1)
out = Path(__file__).with_name("stocks_normal.txt")
with out.open("w", encoding="utf-8", newline="\n") as f:
    f.write("# t inflow stock outflow; normal(25, 8) lifetimes, ODYM's convention; numpy/scipy, 2026-11-13\n")
    for k in range(T):
        f.write(f"{k} {inflow[k]:.17g} {stock[k]:.17g} {outflow[k]:.17g}\n")
print("wrote", out)
