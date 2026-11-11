"""How large: a size computed from the project's quantities, each with its range."""

import re

from science import markets

NAME = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")


def parse(text: str) -> list:
    """Lines `name = a * b + c * d` as definitions: sums of products of names and numbers."""
    out = []
    for raw in str(text).splitlines():
        line = raw.strip()
        if not line or line.startswith("#"):
            continue
        name, eq, expression = line.partition("=")
        if not eq or not NAME.match(name.strip()):
            raise ValueError(f"a line of the formula is  name = a * b + c * d : '{line}'")
        terms = [[f.strip() for f in term.split("*")] for term in expression.split("+")]
        if any(not f for term in terms for f in term):
            raise ValueError(f"a size is sums and products only (a size that subtracts or divides has no exact bounds here): '{line}'")
        out.append((name.strip(), terms))
    if not out:
        raise ValueError("the formula is empty")
    return out


def create(api):
    class Size(api.BoardAnalysis):
        id = "size"
        title = "How large"
        description = ("A size as sums and products of the project's quantities, each a datum or an assumption with its range. The bounds are exact "
                       "(the size rises with every quantity); the elasticities say what one per cent of each is worth; the swings, which could change it most.")
        params = [api.param("formula", "Formula", "residents = dwellings * residents_per_dwelling * spend_resident * weeks\nworkers = office_100m2 * workers_per_100m2 * spend_worker * weeks\nall = residents + workers",
                            kind="text", help="lines  name = a * b + c * d  of the project's quantities and of names defined above; the last line is the size")]

        def run(self, board, params, context):
            nodes, index = [], {}

            def leaf(name: str) -> int:
                if name in index:
                    return index[name]
                try:
                    value = float(name)
                    nodes.append({"kind": "leaf", "name": name, "low": value, "mid": value, "high": value, "assumed": False})
                except ValueError:
                    q = board.quantity(name)
                    nodes.append({"kind": "leaf", "name": name, "low": q.lo, "mid": q.mid, "high": q.hi, "assumed": q.hi > q.lo})
                index[name] = len(nodes) - 1
                return index[name]

            for name, terms in parse(params["formula"]):
                products = []
                for term in terms:
                    children = [leaf(f) for f in term]
                    if len(children) == 1:
                        products.append(children[0])
                    else:
                        nodes.append({"kind": "product", "name": " × ".join(term), "children": children})
                        products.append(len(nodes) - 1)
                if len(products) == 1 and nodes[products[0]]["kind"] != "leaf":
                    nodes[products[0]]["name"] = name
                    index[name] = products[0]
                else:
                    nodes.append({"kind": "sum", "name": name, "children": products})
                    index[name] = len(nodes) - 1
            a = markets.ask("size", nodes=nodes)
            n = a["nodes"]
            last = n[-1]
            leaves = [k for k, node in enumerate(nodes) if node["kind"] == "leaf"]
            return [
                api.table("size", {"quantity": [x["name"] for x in n], "is": [nodes[k]["kind"] if nodes[k]["kind"] != "leaf" else "assumed" if nodes[k]["assumed"] else "given" for k in range(len(n))],
                                   "low": [x["low"] for x in n], "mid": [x["mid"] for x in n], "high": [x["high"] for x in n],
                                   "elasticity": [round(x["elasticity"], 4) if k in leaves else None for k, x in enumerate(n)]},
                          title="The size and what it rests on", roles={"name": "quantity", "amount": "mid"}),
                api.table("swings", {"quantity": [w["name"] for w in a["swings"]], "at_its_least": [w["low"] for w in a["swings"]], "at_its_most": [w["high"] for w in a["swings"]],
                                     "swing": [w["high"] - w["low"] for w in a["swings"]]},
                          title="The size with one quantity at its ends, the others at their middles", roles={"name": "quantity", "amount": "swing"},
                          show={"kind": "chart", "chart": "bar", "x": "quantity", "y": "swing", "how": "sum"}),
                api.note(f"{last['name']}: {last['mid']:,.0f} at the middle of every quantity, {last['low']:,.0f} to {last['high']:,.0f} at their ends. The bounds are exact."),
            ]
    return Size()
