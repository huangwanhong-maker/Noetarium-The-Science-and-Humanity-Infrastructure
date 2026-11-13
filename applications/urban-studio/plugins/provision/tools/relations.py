"""Ties among people: how closed a network is, whether groups keep their ties among themselves, who bridges, and who has few to turn to."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class Relations(api.BoardAnalysis):
        id = "relations"
        title = "Ties among people"
        description = ("A table of ties (who names whom: friendship, help, kin, neighbours) read as a network. It says how closed it is (friends of "
                       "friends who are friends, against networks with the same numbers of ties per person), whether groups keep their ties among "
                       "themselves (homophily, the E–I index and a permutation test), who bridges between circles (Burt's constraint and effective "
                       "size), who is reached within two ties, and how varied each person's contacts are.")
        params = [api.param("table", "Ties", "ties", kind="text", role="table", help="one row a tie"),
                  s.col(api, "source", "From", "from"),
                  s.col(api, "target", "To", "to"),
                  api.param("directed", "Ties have a direction", "no", kind="choice", options=["no", "yes"],
                            help="yes: a names b is not b names a (reciprocity and triads are read)"),
                  api.param("people", "People", "", kind="text", role="table", help="a table of the people with a group column (optional)"),
                  api.param("person", "Person column", "id", kind="text"),
                  api.param("group", "Group column", "group", kind="text", help="gender, origin, class, age band: what belonging is read by"),
                  api.param("permutations", "Permutations", 999, kind="number", minimum=99, maximum=9999),
                  api.param("draws", "Networks drawn", 500, kind="number", minimum=50, maximum=5000)]

        def run(self, board, params, context):
            t = board.table(params["table"])
            a_col, b_col = s.texts(t, params["source"]), s.texts(t, params["target"])
            ids, order = {}, []

            def ident(v):
                if v not in ids:
                    ids[v] = len(order)
                    order.append(v)
                return ids[v]
            group_of = {}
            if params.get("people"):
                pt = board.table(params["people"])
                for pid, g in zip(s.texts(pt, params["person"]), s.texts(pt, params["group"])):
                    ident(pid)
                    group_of[pid] = g
            ties = [[ident(a), ident(b)] for a, b in zip(a_col, b_col) if a and b and a != b]
            if not ties:
                raise ValueError("the table holds no tie between two different people")
            req = {"people": len(order), "ties": ties, "directed": params["directed"] == "yes",
                   "permutations": int(params["permutations"]), "draws": int(params["draws"])}
            names = []
            if group_of:
                names = sorted({g for g in group_of.values() if g})
                code = {g: k for k, g in enumerate(names)}
                missing = [p for p in order if not group_of.get(p)]
                if missing:
                    raise ValueError(f"{len(missing)} people have no group, among them '{missing[0]}'")
                req["groups"] = [code[group_of[p]] for p in order]
            a = s.society.ask("relations", **req)
            w, pe = a["whole"], a["people"]
            rows = {"person": order, "contacts": pe["contacts"], "reach_in_two": pe["reach"], "clustering": [round(x, 4) for x in pe["clustering"]],
                    "constraint": [None if x is None else round(x, 4) for x in pe["constraint"]],
                    "effective_size": [None if x is None else round(x, 3) for x in pe["effective_size"]], "core": pe["core"]}
            if group_of:
                rows["group"] = [group_of[p] for p in order]
                rows["contact_diversity"] = [None if x is None else round(x, 4) for x in pe["diversity"]]
            show = {"kind": "chart", "chart": "scatter", "x": "contacts", "y": "constraint", **({"split": "group"} if group_of else {})}
            out = [api.table("people_ties", rows, title="Each person's ties", roles={"name": "person", "amount": "contacts"}, show=show)]
            whole = (f"{w['people']:.0f} people and {w['ties']:.0f} ties: each has {w['mean_contacts']:.1f} contacts on average, the density is "
                     f"{w['density']:.3f}, {w['isolates']:.0f} are isolated and the largest connected part holds {w['largest']:.0f}.")
            if w.get("reciprocity") is not None:
                whole += f" Of the pairs tied, {s.share(w['reciprocity'])} name each other."
            c = a.get("closure")
            if c:
                whole += (f" Friends of friends are tied {s.share(c['transitivity'])} of the time, against {s.share(c['expected'])} in networks with the "
                          f"same numbers of contacts (p = {c['p_greater']:.3f}, {c['draws']:.0f} drawn).")
            out.append(api.note(whole))
            b = a.get("belonging")
            if b:
                parts = [f"{g} {s.share(x)} of its ties within (its people {s.share(sh)})" for g, x, sh in zip(names, b["within"], b["share"]) if x is not None]
                out.append(api.table("belonging", {"group": names, "members": b["members"], "share_of_people": b["share"], "ties_within": b["within"],
                                                   "inbreeding": b["inbreeding"], "ei": b["ei"]},
                                     title="Belonging: how each group keeps its ties", roles={"name": "group", "amount": "inbreeding"},
                                     show={"kind": "chart", "chart": "bar", "x": "group", "y": "inbreeding", "how": "sum"}))
                out.append(api.note(f"Groups keep their ties among themselves: {'; '.join(parts)}. The E–I index is {b['ei_index']:+.2f} where the groups' sizes "
                                    f"alone would give {b['ei_complete']:+.2f} (−1 all within, +1 all between); the association of ties with a shared group is "
                                    f"{b['qap_r']:.2f}, p = {b['qap_p']:.3f} by {b['permutations']:.0f} relabellings of the people."))
            k = min(range(len(order)), key=lambda i: pe["constraint"][i] if pe["constraint"][i] is not None else 9)
            few = sum(1 for x in pe["contacts"] if x <= 1)
            out.append(api.note(f"The least constrained, the one who bridges most between circles, is {order[k]} (constraint {pe['constraint'][k]:.2f}, "
                                f"effective size {pe['effective_size'][k]:.1f}). {few} people have one contact or none: those with fewest to turn to."))
            out.append(api.note("The ties are as the table names them, which is what a survey's name generator or a record caught, not every tie that exists. "
                                "The tests compare the network with others drawn under a stated null: relabelled people for belonging, the same numbers of "
                                "contacts for closure."))
            return out
    return Relations()
