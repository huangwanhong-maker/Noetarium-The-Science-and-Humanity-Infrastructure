"""Host services: code a community's holdings declare, run by the node for a party under the community's norms (the design of
2026-11-27, Section 9.4).

A tool is a pure function in a declared language, ``restricted python 1`` first (``Restricted``: ``def tool(data, params)`` in a
subset of Python, run in a separate interpreter with a time limit, at most two at once, the bound WSL bears after the first attempt's
semaphore). A run is three acts of the requesting party put to the community's rules in one step (``use`` of the tool, ``use`` of the
capacity it draws, charged along the party's mandate, and the ``deposit`` of the result as a new holding within its container, with the
result's digest), each constituted by the host's episode of the run (operation ``run tool``, its duration, the digests of its inputs
and output). The deposit is checked against the party's briefing before anything is stepped, so that a run whose result could not be
kept is not run; the host runs the code only when both uses are admitted.

Moved here from ``bench.py``, which keeps the book's shelf and its tools and reads the restricted language from here.
"""

from __future__ import annotations

import ast
import hashlib
import json
import math
import subprocess
import sys
import threading
import time

LANGUAGE = "restricted python 1"
RUNS = threading.BoundedSemaphore(2)  # at most two runs at once


def digest_of(value) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, ensure_ascii=False, separators=(",", ":")).encode("utf-8")).hexdigest()


class Restricted:
    """A tool in ``restricted python 1``: ``def tool(data, params)`` in a subset of Python (arithmetic, comparisons, loops,
    comprehensions, lists, dicts, strings; ``math`` and ``statistics``; no other imports, no names or attributes beginning with an
    underscore, no I/O), answering {"facts": {name: number or text}, "summary": text}. It runs in a separate interpreter (``-I``) with a
    time limit."""

    ALLOWED_NODES = {"Module", "FunctionDef", "arguments", "arg", "Return", "Assign", "AugAssign", "For", "While", "If", "Break", "Continue",
                     "Expr", "Pass", "BinOp", "UnaryOp", "BoolOp", "Compare", "Call", "Name", "Constant", "List", "Tuple", "Dict", "Set",
                     "ListComp", "DictComp", "SetComp", "GeneratorExp", "comprehension", "Subscript", "Slice", "Attribute", "IfExp", "keyword",
                     "Load", "Store", "Del", "Add", "Sub", "Mult", "Div", "FloorDiv", "Mod", "Pow", "USub", "UAdd", "Not", "And", "Or",
                     "Eq", "NotEq", "Lt", "LtE", "Gt", "GtE", "In", "NotIn", "Is", "IsNot", "JoinedStr", "FormattedValue", "Import", "alias", "Lambda"}
    ATTRIBUTES = ({"append", "extend", "insert", "pop", "remove", "index", "count", "sort", "reverse", "copy", "clear", "get", "items", "keys",
                   "values", "setdefault", "update", "lower", "upper", "strip", "split", "join", "startswith", "endswith", "replace", "title",
                   "add", "discard", "union", "intersection", "difference", "isdigit"}
                  | {n for n in dir(math) if not n.startswith("_")}
                  | {"mean", "fmean", "median", "median_low", "median_high", "mode", "pstdev", "stdev", "pvariance", "variance", "quantiles",
                     "geometric_mean", "harmonic_mean", "correlation", "covariance", "linear_regression"})
    NAMES = {"len", "range", "min", "max", "sum", "abs", "round", "sorted", "enumerate", "zip", "float", "int", "str", "bool", "list", "dict",
             "set", "tuple", "any", "all", "reversed", "isinstance", "math", "statistics", "map", "filter", "None", "True", "False"}
    MODULES = {"math", "statistics"}

    @classmethod
    def faults(cls, code: str) -> list[str]:
        try:
            tree = ast.parse(code)
        except SyntaxError as e:
            return [f"not Python: {e.msg} at line {e.lineno}"]
        out = []
        defs = [n for n in tree.body if isinstance(n, ast.FunctionDef)]
        if not any(d.name == "tool" and len(d.args.args) == 2 for d in defs):
            out.append("it defines no function tool(data, params)")
        bound = {a.arg for n in ast.walk(tree) if isinstance(n, ast.arguments) for a in n.args} | {d.name for d in defs}
        for n in ast.walk(tree):
            kind = type(n).__name__
            if kind not in cls.ALLOWED_NODES:
                out.append(f"{kind} is not allowed (line {getattr(n, 'lineno', '?')})")
            if isinstance(n, ast.Import) and any(a.name not in cls.MODULES or a.asname for a in n.names):
                out.append("only math and statistics may be imported, under their own names")
            if isinstance(n, ast.Attribute) and n.attr not in cls.ATTRIBUTES:  # an allowlist: a generator's frame and the like are unreachable
                out.append(f"the attribute {n.attr} is not allowed")
            if isinstance(n, ast.BinOp) and isinstance(n.op, ast.Pow) and isinstance(n.right, ast.Constant) and isinstance(n.right.value, (int, float)) and n.right.value > 64:
                out.append("a power above 64 is not allowed")
            if isinstance(n, ast.Name) and n.id.startswith("_"):
                out.append(f"the name {n.id} is not allowed")
            if isinstance(n, ast.Name) and isinstance(n.ctx, ast.Store):
                bound.add(n.id)
        for n in ast.walk(tree):
            if isinstance(n, ast.Name) and isinstance(n.ctx, ast.Load) and n.id not in cls.NAMES and n.id not in bound:
                out.append(f"the name {n.id} is not known")
        return sorted(set(out))[:8]

    RUNNER = r"""
import json, math, statistics, sys
spec = json.loads(sys.stdin.read())
safe = {k: __builtins__[k] if isinstance(__builtins__, dict) else getattr(__builtins__, k) for k in
        ("len", "range", "min", "max", "sum", "abs", "round", "sorted", "enumerate", "zip", "float", "int", "str", "bool", "list", "dict",
         "set", "tuple", "any", "all", "reversed", "isinstance", "map", "filter")}
def _import(name, *a, **k):
    if name in ("math", "statistics"):
        return {"math": math, "statistics": statistics}[name]
    raise ImportError(name)
safe["__import__"] = _import
env = {"__builtins__": safe, "math": math, "statistics": statistics}
exec(compile(spec["code"], "<tool>", "exec"), env)
data = spec["data"]
if spec.get("shelf"):
    data = {c: {u: {int(y): v for y, v in ys.items()} for u, ys in units.items()} for c, units in data.items()}
    data["regions"], data["names"] = spec["regions"], spec["names"]
out = env["tool"](data, spec["params"])
print(json.dumps(out, default=str))
"""

    @classmethod
    def execute(cls, code: str, data, params: dict, timeout: float = 30.0, **extra) -> dict:
        """Runs admissible code on data in its own interpreter, two runs at most at once: its answer {facts, summary}, or {refused}."""
        why = cls.faults(code)
        if why:
            return {"refused": "the tool's code is not admissible: " + "; ".join(why)}
        spec = {"code": code, "params": params, "data": data, **extra}
        with RUNS:
            try:
                done = subprocess.run([sys.executable, "-I", "-c", cls.RUNNER], input=json.dumps(spec), capture_output=True, text=True, timeout=timeout,
                                      encoding="utf-8")
            except subprocess.TimeoutExpired:
                return {"refused": f"the tool ran longer than {timeout:.0f} s"}
        if done.returncode != 0:
            return {"refused": "the tool failed: " + (done.stderr.strip().splitlines() or ["no reason"])[-1][:300]}
        try:
            out = json.loads(done.stdout)
        except ValueError:
            return {"refused": "the tool's answer is not JSON"}
        if not isinstance(out, dict) or not isinstance(out.get("facts"), dict):
            return {"refused": "the tool answered no facts"}
        return out


# ---- the host's runs for the gateway ------------------------------------------------------------------------------------------------

def declare(node, community: str, name: str, code: str, holding: str = "bench", draws: str = "bench runs", reads: list[str] | None = None) -> dict:
    """Declares a tool of a community's holding (its code kept by its digest beside the record, its language, the capacity a run
    draws and the holdings whose content it reads). Refused when the code is not admissible in the language."""
    why = Restricted.faults(code)
    if why:
        raise ValueError("the tool's code is not admissible: " + "; ".join(why))
    h = node.hosted(community)
    d = hashlib.sha256(code.encode("utf-8")).hexdigest()
    h.tools[name] = {"name": name, "holding": holding, "language": LANGUAGE, "digest": d, "code": code, "draws": draws, "reads": list(reads or [])}
    node.save(community)
    return {k: v for k, v in h.tools[name].items() if k != "code"}


def run(node, community: str, party: str, mandate: str, name: str, params: dict, into: str, item: str = "", episode_fields: dict | None = None) -> dict:
    """A run for a party: the deposit checked against its briefing; the two uses stepped, each constituted by the request's episode;
    the code run on the content of the holdings it reads; the result kept as a payload and deposited as a new holding within ``into``.
    Answers {admitted, outcomes, result?, digest?, refused?, norm?}."""
    from .behaviour import episode
    h = node.hosted(community)
    tool = h.tools.get(name)
    if tool is None:
        return {"admitted": False, "missing": True, "refused": f"no tool {name} is declared in {community}"}
    brief = node.session(community).briefing(party, mandate=mandate)
    allowed = [e for e in brief.get("entries", []) if e.get("aim") == "deposit" and (e.get("holding") == into or e.get("within") == into)]
    if not allowed or not allowed[0]["decision"]["permitted"]:
        reason = allowed[0]["decision"]["reason"] if allowed else f"no norm lets {party} deposit within {into}"
        return {"admitted": False, "refused": "the result could not be kept: " + reason, "norm": (allowed[0]["decision"].get("norm") if allowed else "")}
    began = node.now()
    base = dict(episode_fields or {})
    uses = [node.act(party, "use", {"holding": tool["holding"], "purpose": f"run {name}"}, mandate=mandate),
            node.act(party, "use", {"holding": tool["draws"], "amount": 1}, mandate=mandate)]
    for u in uses:
        u["conduct"] = episode(party, "run tool", community=community, node=node.name, begun=began, holdings=[tool["holding"], tool["draws"]],
                               items=[item] if item else [], inputs=[tool["digest"]], tool=name, **base)
    stepped = node.step(community, uses)
    if not all(o.get("admitted") for o in stepped["outcomes"]):
        bad = next(o for o in stepped["outcomes"] if not o.get("admitted"))
        return {"admitted": False, "outcomes": stepped["outcomes"], "refused": bad["reason"]}
    data = {r: h.content.get(r) for r in tool["reads"]}
    t0 = time.time()
    result = Restricted.execute(tool["code"], data, params)
    ms = int((time.time() - t0) * 1000)
    if "refused" in result:
        node.observe(community, episode(party, "run tool", community=community, node=node.name, begun=began, ended=node.now(), inputs=[tool["digest"]],
                                        tool=name, status="failed", reason=result["refused"][:200], ms=ms, **base))
        return {"admitted": False, "outcomes": stepped["outcomes"], "refused": result["refused"]}
    kept = {"tool": name, "language": LANGUAGE, "code": tool["digest"], "params": params, "inputs": {r: digest_of(v) for r, v in data.items()}, "result": result}
    d = h.payloads.put(kept, "constitutive episode", party, node.now())
    deposit = node.act(party, "deposit", {"holding": f"{name} {d[:12]}", "within": into, "digest": d, "kind": "result", "source": f"the tool {name}"}, mandate=mandate)
    deposit["conduct"] = episode(party, "run tool", community=community, node=node.name, begun=began, ended=node.now(), inputs=[tool["digest"], *kept["inputs"].values()],
                                 outputs=[d], holdings=[into], items=[item] if item else [], resources={"bench runs": 1}, tool=name, ms=ms, **base)
    out = node.step(community, [deposit])
    o = out["outcomes"][0]
    return {"admitted": bool(o.get("admitted")), "outcomes": stepped["outcomes"] + out["outcomes"], "result": result, "digest": d,
            **({} if o.get("admitted") else {"refused": o["reason"]})}
