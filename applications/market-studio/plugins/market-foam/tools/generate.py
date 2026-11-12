"""A market from rules: a record made by a system whose truth is known."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402


def create(api):
    class Generate(api.BoardAnalysis):
        id = "generate"
        title = "A market from rules"
        description = ("A record of movements made by rules, for trying what a record shows where none is to hand: subjects hold money and goods; each unit "
                       "of money buys at a hazard (the velocity); a purchase exchanges money for a good at its reference price, moved by noise and by "
                       "the two parties' bargaining; subjects also barter. The truth is known: the velocity, the prices, and who has the better of whom.")
        params = [api.param("subjects", "Subjects", 24, kind="integer", minimum=3, maximum=400),
                  api.param("goods", "Goods", 2, kind="integer", minimum=1, maximum=8),
                  api.param("horizon", "For how long", 20.0, minimum=0.5, maximum=2000),
                  api.param("velocity", "Velocity of money", 2.5, minimum=0, maximum=50, help="the hazard at which a unit of money buys"),
                  api.param("money", "Money each", 6.0, minimum=0, maximum=10000),
                  api.param("noise", "Noise of a price", 0.02, minimum=0, maximum=1, step=0.01),
                  api.param("bargaining", "Spread of bargaining", 0.1, minimum=0, maximum=1, step=0.01, help="how far the strongest and the weakest bargainer stand from the middle, in log price"),
                  api.param("barter", "Barter", 0.3, minimum=0, maximum=20, help="the hazard at which a subject barters one good for another"),
                  api.param("ring", "Partners are neighbours on a ring", False, kind="boolean"),
                  api.param("seed", "Seed", 20261113, kind="integer", minimum=0, maximum=2 ** 31)]

        def run(self, board, params, context):
            n, goods = int(params["subjects"]), int(params["goods"])
            # Bargaining strengths evenly spread, in an order of their own (not the order of the subjects on a ring).
            rng = np.random.default_rng(int(params["seed"]))
            beta = np.linspace(-1, 1, n) * float(params["bargaining"])
            rng.shuffle(beta)
            a = r.ask("generate", rules={"subjects": n, "goods": goods, "horizon": float(params["horizon"]), "velocity": float(params["velocity"]),
                                         "money_each": float(params["money"]), "noise": float(params["noise"]), "barter": float(params["barter"]),
                                         "ring": bool(params["ring"]), "seed": int(params["seed"]), "bargaining": [float(b) for b in beta],
                                         "price": [float(2 + 1.5 * g) for g in range(goods)]})
            rec = a["record"]
            width = len(str(n))
            subjects = [f"s{str(i + 1).zfill(width)}" for i in range(n)]
            kinds = ["money"] + [f"good {g + 1}" for g in range(goods)]
            events = len(set(rec["event"]))
            return [
                api.table("record", {"event": rec["event"], "time": np.round(rec["time"], 6), "from": [subjects[i] for i in rec["from"]], "to": [subjects[i] for i in rec["to"]],
                                     "kind": [kinds[k] for k in rec["kind"]], "quantity": np.round(rec["quantity"], 6)},
                          title="A record of movements, made by rules", roles={"key": "event", "amount": "quantity", "category": "kind", "name": "from"},
                          show={"kind": "chart", "chart": "histogram", "y": "time", "bins": 40}),
                api.table("truth", {"subject": subjects, "bargaining": np.round(a["bargaining"], 4)}, title="What the rules gave each subject: its bargaining strength",
                          roles={"name": "subject", "amount": "bargaining"}),
                api.table("opening", {"subject": [subjects[i] for i in range(n) for _ in kinds], "kind": kinds * n, "quantity": [q for row in rec["opening"] for q in row]},
                          title="Where each subject stood at the beginning", roles={"name": "subject", "category": "kind", "amount": "quantity"}),
                api.note(f"{events:,} events in {len(rec['event']):,} movements among {n} subjects over {float(params['horizon']):g}: purchases and barters, each an exchange. "
                         f"The truth of the rules: money buys at a velocity of {float(params['velocity']):g}; the goods' prices in money are "
                         f"{', '.join(f'{p:g}' for p in a['price'])}; money is neither made nor lost ({a['money']:g} in all). This record is synthetic."),
            ]
    return Generate()
