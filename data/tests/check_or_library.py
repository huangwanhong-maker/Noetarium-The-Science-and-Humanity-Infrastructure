"""Checks of the OR-Library's reader and products (data/formats/orlib.py, data/products/or_library.py), offline on text written here,
and on the cache where the products have been built.

- Offline: two knapsacks and one assignment in the OR-Library's formats read back exactly; truncated and overlong text refused;
  mkcbres's first table read and its second (the LP relaxations) not; gapinfo's tables of optima read by file.
- Cache: mknap1's seven problems with their optima; mknapcb1's thirty problems of 100 variables and 5 constraints, each with the best
  value Chu and Beasley found; gap1's five problems with the optima gapinfo gives (336, 327, 339, 341, 326), and gap12's ten agents
  and sixty jobs.

    python data/tests/check_or_library.py
"""

from __future__ import annotations

import json

from checks import PRODUCTS, assert_, skip

import orlib

KNAP = "2\n4 2 11\n 6 5 4 3\n 2 3 1 4\n 3 1 2 2\n 5 4\n4 1 0\n 1 2 3 4\n 1 1 1 1\n 2\n"
GAP = "1\n2 3\n 5 1 4\n 2 6 3\n 3 2 2\n 1 3 2\n 4 4\n"
RES = """Problem Name        Best Feasible Solution Value

5.100-00                  24381
5.100-01                  24274

Problem Name        LP relaxation

5.100-00                  24585.9
"""
INFO = """<pre> gap1           gap2
 c515-1  336    c520-1  434
 c515-2  327    c520-2  436
</pre>"""


def main() -> None:
    k = orlib.read_knapsacks(KNAP)
    assert_("knapsacks read: two problems, sizes, profits, weights, capacities, an unknown optimum as none",
            len(k) == 2 and k[0]["n"] == 4 and k[0]["optimum"] == 11 and k[0]["weights"][1][0] == 3 and k[0]["capacities"] == [5, 4] and k[1]["optimum"] is None)
    g = orlib.read_assignments(GAP)
    assert_("an assignment read: agents, jobs, costs, resources, capacities", len(g) == 1 and g[0]["m"] == 2 and g[0]["n"] == 3 and g[0]["resources"][1][1] == 3)
    refused = 0
    for bad in ("1\n4 2 11\n 6 5 4 3\n", KNAP + " 9"):
        try:
            orlib.read_knapsacks(bad)
        except ValueError:
            refused += 1
    assert_("truncated and overlong text refused", refused == 2)
    best = orlib.read_best_values(RES)
    assert_("mkcbres: the best feasible values read, the LP relaxations not", best == {"5.100-00": 24381.0, "5.100-01": 24274.0})
    opt = orlib.read_gap_optima(INFO)
    assert_("gapinfo: the optima read by file", opt == {"gap1": [336.0, 327.0], "gap2": [434.0, 436.0]})
    folder = PRODUCTS / "or-library"
    if not (folder / "mknap1.json").is_file():
        skip("the OR-Library's products are not in the cache (python data/products/or_library.py)")
        return
    m1 = json.loads((folder / "mknap1.json").read_text(encoding="utf-8"))
    assert_("mknap1: seven problems, each with its optimum", len(m1["problems"]) == 7 and all(p["optimum"] for p in m1["problems"]))
    if (folder / "mknapcb1.json").is_file():
        cb = json.loads((folder / "mknapcb1.json").read_text(encoding="utf-8"))
        ps = cb["problems"]
        assert_("mknapcb1: thirty problems of 100 variables and 5 constraints, each with Chu and Beasley's best value",
                len(ps) == 30 and all(p["n"] == 100 and p["m"] == 5 and p["best_known"] for p in ps) and ps[0]["best_known"] == 24381)
    if (folder / "gap1.json").is_file():
        g1 = json.loads((folder / "gap1.json").read_text(encoding="utf-8"))
        assert_("gap1: five problems with gapinfo's optima 336, 327, 339, 341, 326", [p["optimum"] for p in g1["problems"]] == [336, 327, 339, 341, 326])
    if (folder / "gap12.json").is_file():
        g12 = json.loads((folder / "gap12.json").read_text(encoding="utf-8"))
        assert_("gap12: ten agents and sixty jobs in each problem", all(p["m"] == 10 and p["n"] == 60 for p in g12["problems"]))


if __name__ == "__main__":
    main()
