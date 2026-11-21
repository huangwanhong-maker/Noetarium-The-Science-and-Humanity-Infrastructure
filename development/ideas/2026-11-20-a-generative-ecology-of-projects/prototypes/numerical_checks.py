"""Numerical checks of Proposition 3, of the model of reseeding (Section 4.4) and of the caveat of Proposition 2 of the
general framework.

No model is called. Four things are checked.

1. The identity of Proposition 3, part 1: G(b) = G(p) - D(p || b), on worlds, foci and odds drawn at random.
2. The bound of Proposition 3, part 2: a population of K founders with different foci, each keeping its focus, grows on
   every sequence of states at least as the best founder less ln K / T; and on average at least max G(b_k) - ln K / T.
3. The model of Section 4.4: the share of a unit's particular rules present after g generations of reseeding without an
   anchor, against the closed form; and the probability that a rule is lost in development when k bodies are developed
   independently and one that passes its check is kept, against lambda to the power k. The independence is assumed.
4. The caveat of Proposition 2: when every part of the best type changes at once, variation part by part loses more
   growth than variation of the whole, and the brackets of the two bounds differ by
   ((J - 1)(T - 1) H(mu) - S ln((K - 1) / prod (K_j - 1))) / T at the common rate mu = S / (T - 1), H the binary entropy
   in nats. The model is that of the fourth prototype, on another environment.

    python numerical_checks.py        writes results/numerical-checks.json
"""
from __future__ import annotations

import json
import math
import random
from pathlib import Path

OUT = Path(__file__).resolve().parent / "results"


def simplex(rng: random.Random, n: int, floor: float = 0.02) -> list[float]:
    """A distribution on n states with every weight at least `floor`."""
    raw = [rng.random() + 1e-9 for _ in range(n)]
    total = sum(raw)
    return [floor + (1 - n * floor) * x / total for x in raw]


def growth(p, b, o) -> float:
    return sum(pe * math.log(be * oe) for pe, be, oe in zip(p, b, o))


def relative_entropy(p, b) -> float:
    return sum(pe * math.log(pe / be) for pe, be in zip(p, b) if pe > 0)


def identity(rng: random.Random, trials: int = 2000) -> dict:
    worst = 0.0
    for _ in range(trials):
        n = rng.randrange(2, 9)
        p, b = simplex(rng, n), simplex(rng, n)
        o = [rng.uniform(0.5, 6.0) for _ in range(n)]
        worst = max(worst, abs(growth(p, b, o) - (growth(p, p, o) - relative_entropy(p, b))))
    return {"trials": trials, "greatest_difference": worst}


def founders(rng: random.Random, runs: int = 400, turns: int = 400) -> dict:
    below_pathwise, margins, means, bounds = 0, [], [], []
    for _ in range(runs):
        n, k = rng.randrange(2, 7), rng.randrange(2, 9)
        p = simplex(rng, n)
        o = [rng.uniform(0.5, 6.0) for _ in range(n)]
        foci = [simplex(rng, n) for _ in range(k)]
        logs = [0.0] * k                                      # ln of each founder's wealth, from equal shares 1 / K
        for _ in range(turns):
            e = rng.choices(range(n), weights=p)[0]
            for j in range(k):
                logs[j] += math.log(foci[j][e] * o[e])
        top = max(logs)
        whole = top + math.log(sum(math.exp(x - top) for x in logs)) - math.log(k)        # ln (N_T / N_0)
        best = max(logs)
        margin = whole / turns - (best / turns - math.log(k) / turns)
        margins.append(margin)
        below_pathwise += margin < -1e-12
        means.append(whole / turns)
        bounds.append(max(growth(p, b, o) for b in foci) - math.log(k) / turns)
    return {"runs": runs, "turns": turns, "runs_below_the_bound_on_their_own_sequence": below_pathwise, "least_margin": min(margins),
            "mean_growth_minus_mean_of_the_bound_in_expectation": sum(means) / runs - sum(bounds) / runs}


def reseeding(rng: random.Random, rules: int = 200000) -> dict:
    rows = []
    for lam, rho in ((0.05, 0.0), (0.05, 0.01), (0.2, 0.05), (0.1, 0.3)):
        present = [True] * rules
        worst = 0.0
        for g in range(1, 31):
            present = [(rng.random() >= lam) if here else (rng.random() < rho) for here in present]
            limit = rho / (lam + rho)
            closed = limit + (1 - limit) * (1 - lam - rho) ** g
            worst = max(worst, abs(sum(present) / rules - closed))
        rows.append({"loss": lam, "return": rho, "generations": 30, "limit": rho / (lam + rho), "share_at_the_end": sum(present) / rules,
                     "greatest_difference_from_the_closed_form": worst})
    anchored = []
    for lam, k in ((0.3, 1), (0.3, 3), (0.5, 3), (0.5, 5)):
        trials = 400000
        lost = sum(1 for _ in range(trials) if all(rng.random() < lam for _ in range(k)))
        anchored.append({"loss": lam, "developments": k, "trials": trials, "lost": lost / trials, "loss_to_the_power": lam ** k})
    return {"rules": rules, "without_an_anchor": rows, "with_a_check": anchored}


def coordinated(turns: int = 30_000) -> list[dict]:
    """Every part changes at once, at two rates. Growth lost against the matched type under each scheme of variation."""
    import p4_modular_variation as p4
    J, K = p4.J, p4.K
    rows = []
    for rate in (0.002, 0.02):
        rng = random.Random(11)
        state, states, changes = [0] * J, [], 0
        for _ in range(turns):
            if rng.random() < rate:
                state = [rng.choice([v for v in range(K) if v != s]) for s in state]
                changes += 1
            states.append(tuple(state))
        matched = J * math.log(p4.GOOD)
        star = changes / (turns - 1)
        by_part = p4.growth(states, True, (star,) * J)
        whole_rate, whole = p4.best_single_rate(states, False)

        def bracket(alternatives: int) -> float:
            return (math.log(alternatives) + changes * math.log((alternatives - 1) / star) + (turns - 1 - changes) * math.log(1 / (1 - star))) / turns

        entropy = changes * math.log(1 / star) + (turns - 1 - changes) * math.log(1 / (1 - star))
        formula = ((J - 1) * entropy - changes * math.log((K ** J - 1) / (K - 1) ** J)) / turns
        rows.append({"rate_of_change": rate, "turns": turns, "changes": changes, "growth_of_the_matched_type": matched,
                     "lost_part_by_part_at_the_rate_of_change": matched - by_part, "lost_by_the_whole_type_at_its_best_rate": matched - whole,
                     "best_rate_of_the_whole_type": whole_rate, "difference_of_the_brackets": J * bracket(K) - bracket(K ** J),
                     "difference_by_the_formula": formula})
    return rows


if __name__ == "__main__":
    rng = random.Random(20261121)
    result = {"proposition_3_part_1": identity(rng), "proposition_3_part_2": founders(rng), "section_4_4": reseeding(rng),
              "proposition_2_coordinated_changes": coordinated()}
    result["all_hold"] = (
        result["proposition_3_part_1"]["greatest_difference"] < 1e-9
        and result["proposition_3_part_2"]["runs_below_the_bound_on_their_own_sequence"] == 0
        and result["proposition_3_part_2"]["mean_growth_minus_mean_of_the_bound_in_expectation"] > -0.01
        and all(r["greatest_difference_from_the_closed_form"] < 0.005 for r in result["section_4_4"]["without_an_anchor"])
        and all(abs(r["lost"] - r["loss_to_the_power"]) < 0.004 for r in result["section_4_4"]["with_a_check"])
        and all(r["lost_part_by_part_at_the_rate_of_change"] > r["lost_by_the_whole_type_at_its_best_rate"] for r in result["proposition_2_coordinated_changes"])
        and all(abs(r["difference_of_the_brackets"] - r["difference_by_the_formula"]) < 1e-9 for r in result["proposition_2_coordinated_changes"])
    )
    OUT.mkdir(exist_ok=True)
    (OUT / "numerical-checks.json").write_text(json.dumps(result, indent=1), encoding="utf-8")
    print(json.dumps(result, indent=1))
