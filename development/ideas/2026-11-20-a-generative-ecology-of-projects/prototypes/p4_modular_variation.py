"""Prototype 4. Variation part by part, against variation of the whole.

A numerical check of the proposition on modular variation (the general framework, Section 6): when a type is a
tuple of parts and each part varies at a rate of its own, a population follows a best type that changes in one
part at a time at the price of that part's alternatives, and the best rate for each part is the rate at which
that part of the environment changes. No model is asked.

    python p4_modular_variation.py       writes results/p4.json

Types are triples of parts with four alternatives each (64 types). The environment names one best alternative
for each part; part j changes with probability q_j in a turn, the parts at very different rates. A type is
multiplied by 1.6 for each part that matches and by 0.7 for each that does not.
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

import numpy as np

OUT = Path(__file__).resolve().parent / "results"
OUT.mkdir(exist_ok=True)
J, K, GOOD, BAD, T = 3, 4, 1.6, 0.7, 120_000
Q = (0.02, 0.002, 0.0002)


def environment(seed: int):
    rng = random.Random(seed)
    state, states, changes = [0] * J, [], [0] * J
    for _ in range(T):
        for j in range(J):
            if rng.random() < Q[j]:
                state[j] = rng.choice([v for v in range(K) if v != state[j]])
                changes[j] += 1
        states.append(tuple(state))
    return states, changes


def growth(states, modular, rates) -> float:
    """The mean logarithmic growth of the whole population per turn."""
    n = np.full((K,) * J, 1.0 / K ** J)
    total = 0.0
    factors = [np.full(K, BAD) for _ in range(J)]
    for state in states:
        w = np.ones((K,) * J)
        for j in range(J):
            f = factors[j].copy()
            f[state[j]] = GOOD
            shape = [1] * J
            shape[j] = K
            w = w * f.reshape(shape)
        n = n * w
        if modular:
            for j in range(J):
                mu = rates[j]
                n = (1 - mu) * n + mu / (K - 1) * (n.sum(axis=j, keepdims=True) - n)
        else:
            mu = rates
            n = (1 - mu) * n + mu / (K ** J - 1) * (n.sum() - n)
        size = n.sum()
        total += math.log(size)
        n = n / size
    return total / len(states)


def best_single_rate(states, modular: bool):
    """Golden-section search on the logarithm of one rate."""
    lo, hi, phi = math.log(1e-6), math.log(0.5), (math.sqrt(5) - 1) / 2
    f = lambda x: growth(states, modular, (math.exp(x),) * J if modular else math.exp(x))  # noqa: E731
    a, b = hi - phi * (hi - lo), lo + phi * (hi - lo)
    fa, fb = f(a), f(b)
    for _ in range(16):
        if fa < fb:
            lo, a, fa = a, b, fb
            b = lo + phi * (hi - lo)
            fb = f(b)
        else:
            hi, b, fb = b, a, fa
            a = hi - phi * (hi - lo)
            fa = f(a)
    x = (lo + hi) / 2
    return math.exp(x), f(x)


def main() -> None:
    states, changes = environment(7)
    matched = J * math.log(GOOD)  # the growth of a type that matches in every part, per turn
    turns_with_change = sum(1 for a, b in zip(states, states[1:]) if a != b)

    def cost(alternatives, s, mu):  # the bracket of the tracking bound, per turn
        return (math.log(alternatives) + s * math.log((alternatives - 1) / mu) + (T - 1 - s) * math.log(1 / (1 - mu))) / T

    rows = {}
    own = tuple(max(c, 1) / (T - 1) for c in changes)
    rows["part by part, each at its own rate S_j/(T-1)"] = {
        "rates": own, "growth": growth(states, True, own),
        "bound": matched - sum(cost(K, s, mu) for s, mu in zip(changes, own))}
    mu, g = best_single_rate(states, True)
    rows["part by part, one rate for all parts (best)"] = {
        "rates": (mu,) * J, "growth": g, "bound": matched - sum(cost(K, s, mu) for s in changes)}
    mu, g = best_single_rate(states, False)
    rows["the whole type at one rate (best)"] = {
        "rates": mu, "growth": g, "bound": matched - cost(K ** J, turns_with_change, mu)}
    rows["no variation"] = {"rates": 0.0, "growth": growth(states, False, 0.0), "bound": None}
    record = {"parts": J, "alternatives_per_part": K, "turns": T, "rates_of_change": Q, "changes_by_part": changes,
              "growth_of_the_matched_type": matched, "rows": rows}
    (OUT / "p4.json").write_text(json.dumps(record, indent=1), encoding="utf-8")
    print("changes by part", changes, "| matched type grows", round(matched, 4))
    for name, r in rows.items():
        print(f"{name:52s} growth {r['growth']:.4f}  bound {r['bound'] if r['bound'] is None else round(r['bound'], 4)}  rates {r['rates']}")


if __name__ == "__main__":
    main()
