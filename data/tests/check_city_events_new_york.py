"""Checks of New York's record of a district's events (data/products/city_events_new_york.py), offline: street names keyed as
the engine keys them (urban science's events/network.hpp, street_key, on the same examples as its check); the blocks a permit's
location names; the sources' dates and the window. Where the product has been built, its schema and its counts. Nothing is
fetched.
"""

from __future__ import annotations

import datetime as dt
import json

from checks import PRODUCTS, assert_, skip

import city_events_new_york as ny  # noqa: E402

# ---- street names, keyed as the engine keys them ---------------------------------------------------------------------------
pairs = [("WEST   43 STREET", "WEST 43 STREET"), ("West 43rd Street", "WEST 43 STREET"), ("S William St.", "SOUTH WILLIAM STREET"),
         ("MAIN ST", "MAIN STREET"), ("first avenue", "FIRST AVENUE"), ("SECOND AVE", "SECOND AVENUE"), ("Hanover Sq", "HANOVER SQUARE")]
wrong = [(a, ny.street_key(a), b) for a, b in pairs if ny.street_key(a) != b]
assert_("street_key: names as records write them keyed as urban science's street_key keys them (its check's three among them)", not wrong, str(wrong))
assert_("street_key: a lone direction is a name, not an abbreviation (\"W\" stays \"W\")", ny.street_key("W") == "W", ny.street_key("W"))

# ---- the blocks a location names ---------------------------------------------------------------------------------------------
B = ny.blocks("WALL STREET between BROAD STREET and WILLIAM STREET, NEW STREET between  EXCHANGE PLACE and BEAVER STREET")
assert_("blocks: two blocks of a location, each [street, from, to], spaces collapsed",
        B == [["WALL STREET", "BROAD STREET", "WILLIAM STREET"], ["NEW STREET", "EXCHANGE PLACE", "BEAVER STREET"]], str(B))
assert_("blocks: a location that names no block (a plaza alone) gives none, and case does not matter",
        ny.blocks("Stone Street Plaza") == [] and ny.blocks("Pearl St BETWEEN Broad St AND Whitehall St") == [["Pearl St", "Broad St", "Whitehall St"]])

# ---- dates and the window -----------------------------------------------------------------------------------------------------
dates = [("02/15/22 11:08:46 AM", dt.date(2022, 2, 15)), ("6/21/2011", dt.date(2011, 6, 21)), ("2014-05-03T00:00:00.000", dt.date(2014, 5, 3)),
         ("not a date", None), ("", None)]
wrong = [(a, ny.dobnow_date(a), b) for a, b in dates if ny.dobnow_date(a) != b]
assert_("dates: DOB NOW's stamps, a date alone and ISO read; anything else refused", not wrong, str(wrong))
assert_("window: 2008-01-01 is in it, 2025-01-01 and 2007-12-31 are not",
        ny.in_window(dt.date(2008, 1, 1)) and not ny.in_window(dt.date(2025, 1, 1)) and not ny.in_window(dt.date(2007, 12, 31)) and not ny.in_window(None))

# ---- the product, where it has been built -----------------------------------------------------------------------------------
path = PRODUCTS / "city-events-new-york.json"
if not path.exists():
    skip("the product: city-events-new-york.json not built (python data/products/city_events_new_york.py)")
else:
    P = json.loads(path.read_text(encoding="utf-8"))
    cols = P["events"]["columns"]
    rows = [dict(zip(cols, r)) for r in P["events"]["rows"]]
    by_type = {}
    for r in rows:
        by_type[r["type"]] = by_type.get(r["type"], 0) + 1
    assert_("the product: its events counted by type as its report counts them", by_type == P["report"]["events_by_type"] and len(rows) == P["report"]["counts"]["events"],
            f"{by_type} against {P['report']['events_by_type']}")
    regimes = [r for r in rows if r["type"] == "regime"]
    bad = [r["id"] for r in regimes if not (r["match"].get("blocks") or r["match"].get("streets") or r["match"].get("street"))]
    assert_("the product: every regime names its blocks or streets", not bad, str(bad[:5]))
    closures = sum(1 for r in regimes if (r.get("detail") or {}).get("closure") is True)
    assert_("the product: its closures as its report counts them", closures == P["report"]["regimes_closing_the_street"], f"{closures}")
    wrong = [r["id"] for r in rows if not ny.in_window(dt.date.fromisoformat(r["from"][:10]))]
    assert_("the product: every event begins in the window", not wrong, str(wrong[:5]))
