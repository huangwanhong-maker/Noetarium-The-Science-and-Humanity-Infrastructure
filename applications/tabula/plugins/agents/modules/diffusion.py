"""How fast something spreads among buyers who look to one another: Bass's diffusion, as agents on a lattice."""

import numpy as np


def bass(p: float, q: float, steps: int) -> np.ndarray:
    """The share who have adopted at each step when everyone looks to everyone: F(t+1) = F(t) + (1 − F(t)) (p + q F(t)), F(0) = 0. It is the mean
    field of the agents below, exact in expectation as their number grows (Bass 1969, in discrete time)."""
    f = np.zeros(steps + 1)
    for t in range(steps):
        f[t + 1] = f[t] + (1.0 - f[t]) * min(1.0, p + q * f[t])
    return f


def simulate(p: float, q: float, side: int, steps: int, runs: int, everyone: bool, rng) -> tuple:
    """Agents on a side × side lattice that closes on itself. At each step one who has not adopted does so with probability p + q s, s the share
    of those it looks to who have: its eight neighbours, or everyone. Answers the step at which each agent of the first run adopted (−1: never
    within the steps) and the share adopted at each step in every run."""
    adopted = np.zeros((runs, side, side), dtype=bool)
    when = np.full((side, side), -1, dtype=int)
    share = np.zeros((runs, steps + 1))
    for t in range(1, steps + 1):
        if everyone:
            looked = np.broadcast_to(adopted.mean(axis=(1, 2))[:, None, None], adopted.shape)
        else:
            looked = sum(np.roll(np.roll(adopted, dx, axis=1), dy, axis=2) for dx in (-1, 0, 1) for dy in (-1, 0, 1) if dx or dy) / 8.0
        new = ~adopted & (rng.random(adopted.shape) < p + q * looked)
        when[new[0]] = t
        adopted |= new
        share[:, t] = adopted.mean(axis=(1, 2))
    return when, share


def create(api):
    class Diffusion(api.BoardAnalysis):
        id = "diffusion"
        title = "How fast it would spread"
        description = ("Buyers on a lattice, each adopting at a step with probability p + q s: p of its own accord, q by looking to others, s the share of "
                       "those it looks to who have adopted (its eight neighbours, or everyone). p and q are read from the board's notes with their ranges. "
                       "Several runs give the spread of chance; Bass's curve, the case where everyone looks to everyone, is drawn beside them, with the "
                       "curves at the low and high ends of the two assumptions.")
        params = [api.param("p", "Adopting of one's own accord", "innovation", kind="text", role="quantity"),
                  api.param("q", "Adopting by looking to others", "imitation", kind="text", role="quantity"),
                  api.param("reach", "Each looks to", "neighbours", kind="choice", options=["neighbours", "everyone"]),
                  api.param("side", "Buyers along a side", 40, kind="integer", minimum=5, maximum=200),
                  api.param("steps", "Steps", 30, kind="integer", minimum=2, maximum=400),
                  api.param("runs", "Runs", 20, kind="integer", minimum=1, maximum=200),
                  api.param("seed", "Seed", 1, kind="integer", minimum=0),
                  api.param("name", "Name its tables", "spread", kind="text")]

        def run(self, board, params, context):
            p, q = board.quantity(params["p"]), board.quantity(params["q"])
            for name, v in (("p", p), ("q", q)):
                if not 0 <= v.lo <= v.hi <= 1:
                    raise ValueError(f"{name} is a probability at a step: {v.name} must lie within 0 and 1, and is {v.lo:g} to {v.hi:g}")
            side, steps, runs = params["side"], params["steps"], params["runs"]
            when, share = simulate(p.mid, q.mid, side, steps, runs, params["reach"] == "everyone", np.random.default_rng(params["seed"]))
            context.progress(0.8, "the runs are done")
            ys, xs = np.mgrid[0:side, 0:side]
            agents = api.table(f"{params['name']}_buyers", {"x": xs.ravel(), "y": ys.ravel(), "adopted_at": [None if w < 0 else int(w) for w in when.ravel()]},
                               title="Each buyer of the first run, and the step at which it adopted", labels={"adopted_at": "the step of its adoption; empty: not within the steps"})
            field = bass(p.mid, q.mid, steps)
            curve = api.table(params["name"], {
                "step": np.arange(steps + 1), "share": share[0], "mean": share.mean(axis=0), "low": share.min(axis=0), "high": share.max(axis=0),
                "bass": field, "bass_low": bass(p.lo, q.lo, steps), "bass_high": bass(p.hi, q.hi, steps),
            }, title="The share who have adopted, step by step", labels={
                "share": "the first run", "mean": "the mean of the runs", "low": "the least of the runs", "high": "the most of the runs",
                "bass": "everyone looking to everyone (Bass)", "bass_low": "Bass at the low ends of p and q", "bass_high": "Bass at their high ends"})
            mean = share.mean(axis=0)
            half = lambda f: next((int(t) for t in range(len(f)) if f[t] >= 0.5), None)
            told = lambda t: f"by step {t}" if t is not None else f"not within {steps} steps"
            who = "everyone" if params["reach"] == "everyone" else "its eight neighbours"
            return [curve, agents, api.note(
                f"{side * side:,} buyers, each looking to {who}; p = {p.mid:g} ({p.lo:g} to {p.hi:g}), q = {q.mid:g} ({q.lo:g} to {q.hi:g}). "
                f"Half have adopted {told(half(mean))} in the mean of {runs} runs; {told(half(field))} if everyone looked to everyone. "
                f"At the end {mean[-1]:.0%} have adopted ({share[:, -1].min():.0%} to {share[:, -1].max():.0%} across the runs).")]

    return Diffusion()
