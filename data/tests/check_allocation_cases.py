"""Checks of the reader and the cases of allocation (data/formats/pabulib.py, data/products/allocation_cases.py), offline on a file
written here, and on the cache where the cases have been built.

- Pabulib: a file of three sections with a comment in META, a project with two coordinates' average given, a voter naming a project
  not listed (counted, and left out of the approval set), a repeated approval read once; costs and the budget as numbers.
- England, where it is in the cache: 296 districts and 33,755 LSOAs; the population summed from the denominators; Birmingham's share
  of people in income-deprived households 41.7% and Blackpool's of working-age adults in employment deprivation 26.0%, as the
  statistical release's Table 7 gives them; the four kinds of authority read from the codes.
- Warsaw's budget, where it is in the cache: every approval set within the projects, the budget the file's, the city's selection
  within it.

    python data/tests/check_allocation_cases.py
"""

from __future__ import annotations

import json
import tempfile
from pathlib import Path

from checks import PRODUCTS, assert_, skip

import pabulib

SAMPLE = """META
key;value
description;An invented budget
unit;Nowhere
instance;2026
budget;100
vote_type;approval
comment;#1: averaged coordinates
PROJECTS
project_id;cost;votes;name;category;selected;latitude;longitude
p1;60;3;A park;greenery;1;52.1;21.0
p2;50;2;A path;transport;0;;
p3;40;1;A bench;public space;1;;
VOTES
voter_id;vote;age;sex;neighborhood
1;p1,p2;30;F;North
2;p1,p1,p3;41;M;South
3;p2,p9;19;F;North
"""

with tempfile.TemporaryDirectory() as d:
    path = Path(d) / "nowhere.pb"
    path.write_text(SAMPLE, encoding="utf-8")
    E = pabulib.election(pabulib.read(path))
    assert_("pabulib: three projects, three voters, the budget and costs as numbers", len(E["costs"]) == 3 and len(E["approvals"]) == 3 and E["budget"] == 100 and E["costs"] == [60, 50, 40], E)
    assert_("pabulib: a repeated approval read once, and a project not listed left out and counted", E["approvals"][1] == [0, 2] and E["approvals"][2] == [1] and E["unknown_votes"] == 1, E["approvals"])
    assert_("pabulib: the city's selection, coordinates where given, and the voters' neighbourhoods kept",
            E["selected"] == [True, False, True] and E["lat"][0] == 52.1 and E["lat"][1] is None and E["voters"][0]["neighborhood"] == "North", E["selected"])

eng = PRODUCTS / "allocation" / "england-iod2025.json"
if not eng.is_file():
    skip("england: the case is not built (python data/products/allocation_cases.py england)")
else:
    D = json.loads(eng.read_text(encoding="utf-8"))
    by = {e["name"]: e for e in D["entities"]}
    assert_("england: 296 districts and 33,755 LSOAs", len(D["entities"]) == 296 and len(D["lsoas"]["population"]) == 33755, (len(D["entities"]), len(D["lsoas"]["population"])))
    assert_("england: the population summed from the LSOAs' denominators", sum(e["at"]["population"] for e in D["entities"]) == sum(D["lsoas"]["population"]) or
            abs(sum(e["at"]["population"] for e in D["entities"]) - sum(D["lsoas"]["population"])) < 300)
    assert_("england: Birmingham's income-deprived share 41.7%, as the release's Table 7", round(100 * by["Birmingham"]["at"]["income_rate"], 1) == 41.7, by["Birmingham"]["at"])
    assert_("england: Blackpool's employment-deprived share 26.0%, as the release's Table 7", round(100 * by["Blackpool"]["at"]["employment_rate"], 1) == 26.0, by["Blackpool"]["at"])
    kinds = {e["group"] for e in D["entities"]}
    assert_("england: the four kinds of authority read from the codes", kinds == {"London borough", "metropolitan district", "unitary authority", "shire district"}, kinds)

war = PRODUCTS / "allocation" / "pabulib-Poland_Warszawa_2023.json"
if not war.is_file():
    skip("warsaw: the case is not built (python data/products/allocation_cases.py pabulib)")
else:
    W = json.loads(war.read_text(encoding="utf-8"))
    m = len(W["costs"])
    assert_("warsaw: every approval set within the projects", all(0 <= j < m for a in W["approvals"] for j in a))
    selected = sum(p["cost"] for p in W["projects"] if p["selected"])
    assert_("warsaw: the city's selection within the budget", selected <= W["budget"], (selected, W["budget"]))
