"""The Archive Studio's tools held to answers known by construction, on small tables invented here: calibration's anchors at 0.05,
0.5 and 0.95; QCA recovering the two paths a table was made with; the concepts of a context of three cases; Mill's method of
difference finding the one condition that differs; two explanations weighed to the posterior Bayes's rule gives; coders who agree
completely (α = 1) and coders who split one code; Cronbach's α against its formula; the k of a table of people; a mixed model
whose groups differ only in their intercepts. Prints one JSON line a check; run-tests.mjs reads them.

    python applications/archive-studio/tests/tools_check.py
"""

from __future__ import annotations

import importlib.util
import json
import math
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[3]
sys.path.insert(0, str(ROOT / "api" / "python"))
from science import board as api  # noqa: E402

TOOLS = ROOT / "applications" / "archive-studio" / "tools"


def say(name, ok, detail=""):
    print(json.dumps({"name": name, "ok": bool(ok), "detail": "" if ok else str(detail)[:400]}), flush=True)


def tool(group, name):
    spec = importlib.util.spec_from_file_location(f"t_{name}", TOOLS / group / f"{name}.py")
    m = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(m)
    return m.create(api)


def T(columns):
    return {"columns": [{"name": k, "kind": "number" if all(isinstance(v, (int, float)) or v is None for v in vals) else "text"} for k, vals in columns.items()], "data": columns}


def table(out, name):
    return next(o for o in out if o.get("kind") == "table" and o["name"] == name)


def run(group, name, tables, params):
    try:
        return tool(group, name).run(api.Board({"tables": tables}), params, None)
    except Exception as e:  # noqa: BLE001 (a tool that fails fails its check)
        say(f"tools: {name} runs", False, f"{type(e).__name__}: {e}")
        return None


out = run("cases", "calibrate", {"t": T({"raw": [10.0, 50.0, 90.0]})}, {"table": "t", "raw": "raw", "out": 10, "crossover": 50, "full": 90, "name": "m"})
if out:
    m = table(out, "t_calibrated")["data"]["m"]
    say("tools: calibration puts the anchors at 0.0474, 0.5 and 0.9526 (log-odds −3, 0, +3)", abs(m[0] - 1 / (1 + math.exp(3))) < 1e-3 and m[1] == 0.5 and abs(m[2] - 1 / (1 + math.exp(-3))) < 1e-3, m)

# A crisp table made with Y = A*B + C*D, every configuration once.
rows = [(a, b, c, d) for a in (0, 1) for b in (0, 1) for c in (0, 1) for d in (0, 1)]
cases = T({"case": [f"k{i}" for i in range(16)], "A": [r[0] for r in rows], "B": [r[1] for r in rows], "C": [r[2] for r in rows], "D": [r[3] for r in rows],
           "Y": [1 if (r[0] and r[1]) or (r[2] and r[3]) else 0 for r in rows]})
out = run("cases", "qca", {"c": cases}, {"table": "c", "case": "case", "conditions": "A, B, C, D", "outcome": "Y", "consistency": 0.9, "frequency": 1, "expect": "", "negate": "no"})
if out:
    models = sorted(set(table(out, "solutions")["data"]["model"]))
    say("tools: QCA recovers the paths a table was made with, A*B + C*D, as its only minimal model", models == ["M1: A*B + C*D"], models)

ctx = T({"case": ["x", "y", "z"], "p": [1, 1, 0], "q": [1, 0, 0], "r": [0, 0, 1]})
out = run("cases", "concepts", {"c": ctx}, {"table": "c", "case": "case", "attributes": "p, q, r"})
if out:
    cs = table(out, "concepts")["data"]
    imp = table(out, "implications")["data"]
    # Concepts: ({x,y,z},∅), ({x,y},{p}), ({x},{p,q}), ({z},{r}), (∅,{p,q,r}); stem base: q → p, and p r → q.
    say("tools: three cases and three attributes hold five concepts, q → p among the implications", len(cs["concept"]) == 5 and any(i == "q" and t == "p" for i, t in zip(imp["if"], imp["then"])), (cs, imp))

mill = T({"case": ["with", "without"], "a": [1, 1], "b": [1, 0], "Y": [1, 0]})
out = run("cases", "mill", {"m": mill}, {"table": "m", "case": "case", "conditions": "a, b", "outcome": "Y"})
if out:
    d = table(out, "mill")["data"]
    k = d["method"].index(next(m for m in d["method"] if "difference" in m))
    say("tools: Mill's method of difference leaves standing the one condition that differs", d["conditions_standing"][k] == "b", d)

out = run("cases", "tracing", {"h": T({"name": ["H1", "H2"], "prior": [0.5, 0.5]}), "k": T({"clue": ["e"], "found": ["yes"], "H1": [0.8], "H2": [0.2]})},
          {"hypotheses": "h", "name": "name", "prior": "prior", "clues": "k", "clue": "clue", "found": "found"})
if out:
    post = table(out, "posterior")["data"]["H1"][-1]
    say("tools: one clue four times likelier under H1 takes it from 0.5 to 0.8, as Bayes's rule gives", abs(post - 0.8) < 1e-12, post)

agree = T({"document": ["d"] * 4, "begin": [0, 0, 6, 6], "end": [5, 5, 11, 11], "quote": ["alpha", "alpha", "bravo", "bravo"], "code": ["x", "x", "y", "y"], "coder": ["a", "b", "a", "b"]})
docs = T({"document": ["d"], "case": ["d"], "text": ["alpha bravo"]})
out = run("texts", "coding", {"s": agree, "d": docs}, {"segments": "s", "documents": "d"})
if out:
    a = table(out, "agreement")["data"]
    say("tools: two coders who agree on every passage agree completely (α = 1 for each code)", all(abs(v - 1) < 1e-12 for v in a["alpha"]), a)

items = [[1, 2, 2], [3, 3, 4], [2, 2, 3], [4, 5, 4], [1, 1, 2], [5, 4, 5]]
out = run("measures", "reliability", {"t": T({"a": [r[0] for r in items], "b": [r[1] for r in items], "c": [r[2] for r in items]})}, {"table": "t", "items": "a, b, c"})
if out:
    k = 3
    var = lambda xs: sum((x - sum(xs) / len(xs)) ** 2 for x in xs) / (len(xs) - 1)
    alpha = k / (k - 1) * (1 - sum(var([r[j] for r in items]) for j in range(k)) / var([sum(r) for r in items]))
    note = next(o["text"] for o in out if o.get("kind") == "note")
    say("tools: Cronbach's α as its formula gives it", f"{alpha:.3f}" in note, (alpha, note))

people = T({"age": ["30", "30", "40", "40", "40", "50"], "place": ["N", "N", "S", "S", "S", "N"]})
out = run("measures", "anonymity", {"p": people}, {"table": "p", "quasi": "age, place", "k": 2, "sensitive": ""})
if out:
    risk = table(out, "at_risk")["data"]["row"]
    say("tools: the table is 1-anonymous, its one unique row (the sixth) at risk", risk == [6], risk)

g = ["a"] * 5 + ["b"] * 5 + ["c"] * 5
x = [1, 2, 3, 4, 5] * 3
shift = {"a": -2.0, "b": 0.0, "c": 2.0}
noise = [0.1, -0.1, 0.05, -0.05, 0.0] * 3
y = [1 + 0.5 * xi + shift[gi] + e for xi, gi, e in zip(x, g, noise)]
out = run("measures", "mixed", {"t": T({"y": y, "x": x, "g": g})}, {"table": "t", "y": "y", "x": "x", "group": "g", "slopes": "", "method": "REML"})
if out:
    fe = table(out, "fixed_effects")["data"]
    slope = fe["estimate"][fe["term"].index("x")]
    say("tools: a mixed model whose groups differ in their intercepts finds the common slope, 0.5", abs(slope - 0.5) < 0.02, fe)
