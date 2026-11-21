"""The OR-Library's instances (Beasley 1990, J. Oper. Res. Soc. 41, 1069–1072), read from their text.

- ``read_knapsacks(text)``: the multidimensional knapsack (mknap1, mknapcb1 … 9): the number of problems, then for each: n, m and the
  optimal value (0 when unknown), the n profits, the m rows of n weights, the m capacities.
- ``read_assignments(text)``: the generalised assignment problem (gap1 … gap12): the number of problems, then for each: m agents and
  n jobs, the m rows of n costs, the m rows of n resources, the m capacities.
- ``read_best_values(text)``: mkcbres's first table, the best feasible values Chu and Beasley found, by the problems' names
  ("5.100-00": m constraints, n variables, problem number).
- ``read_gap_optima(html)``: the optima gapinfo publishes for gap1 to gap12 (as maximisation problems), by file and problem.

The same reading is heuresis's in C++ (benchmarks/orlib.hpp); this one serves the data library's products.
"""

from __future__ import annotations

import re


class Numbers:
    def __init__(self, text: str):
        self.tokens = text.split()
        self.i = 0

    def next(self, what: str) -> float:
        if self.i >= len(self.tokens):
            raise ValueError(f"the OR-Library text ends before {what}")
        v = float(self.tokens[self.i])
        self.i += 1
        return v

    def count(self, what: str) -> int:
        v = self.next(what)
        if v < 0 or v != int(v):
            raise ValueError(f"{what} is not a count")
        return int(v)

    def done(self) -> bool:
        return self.i >= len(self.tokens)


def read_knapsacks(text: str) -> list[dict]:
    r = Numbers(text)
    out = []
    for _ in range(r.count("the number of problems")):
        n, m = r.count("the number of variables"), r.count("the number of constraints")
        optimum = r.next("the optimal value")
        if n == 0 or m == 0:
            raise ValueError("a knapsack needs variables and constraints")
        profits = [r.next("the profits") for _ in range(n)]
        weights = [[r.next("the weights") for _ in range(n)] for _ in range(m)]
        capacities = [r.next("the capacities") for _ in range(m)]
        out.append({"n": n, "m": m, "optimum": optimum or None, "profits": profits, "weights": weights, "capacities": capacities})
    if not r.done():
        raise ValueError("numbers left after the last knapsack")
    return out


def read_assignments(text: str) -> list[dict]:
    r = Numbers(text)
    out = []
    for _ in range(r.count("the number of problems")):
        m, n = r.count("the number of agents"), r.count("the number of jobs")
        if n == 0 or m == 0:
            raise ValueError("an assignment needs agents and jobs")
        costs = [[r.next("the costs") for _ in range(n)] for _ in range(m)]
        resources = [[r.next("the resources") for _ in range(n)] for _ in range(m)]
        capacities = [r.next("the capacities") for _ in range(m)]
        out.append({"m": m, "n": n, "costs": costs, "resources": resources, "capacities": capacities})
    if not r.done():
        raise ValueError("numbers left after the last assignment")
    return out


def read_best_values(text: str) -> dict[str, float]:
    """The first table of mkcbres: lines "m.n-kk   value" before the table of LP relaxations."""
    out: dict[str, float] = {}
    for line in text.splitlines():
        if "LP" in line and out:
            break
        m = re.match(r"\s*(\d+\.\d+-\d+)\s+(\d+(?:\.\d+)?)\s*$", line)
        if m and m.group(1) not in out:
            out[m.group(1)] = float(m.group(2))
    return out


def read_gap_optima(html: str) -> dict[str, list[float]]:
    """gapinfo's tables of optima: under each header row "gap1 gap2 …", rows of "c515-1 336 c520-1 434 …"."""
    text = re.sub(r"<[^>]*>", " ", html)
    out: dict[str, list[float]] = {}
    files: list[str] = []
    for line in text.splitlines():
        heads = re.findall(r"\bgap(\d+)\b", line)
        if heads and len(heads) == len(line.split()):
            files = [f"gap{h}" for h in heads]
            for f in files:
                out.setdefault(f, [])
            continue
        pairs = re.findall(r"\b([a-z]\d+-\d+)\s+(\d+)\b", line)
        if files and pairs and len(pairs) == len(files):
            for f, (_, v) in zip(files, pairs):
                out[f].append(float(v))
    return out
