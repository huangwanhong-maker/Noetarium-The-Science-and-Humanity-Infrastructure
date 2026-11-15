"""Concepts: what a table of cases and attributes holds, as a lattice, and the implications among the attributes."""

import html
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parents[1]))
import _common as c  # noqa: E402


def hasse(concepts, covers, objects, attributes, members) -> str:
    """The lattice drawn by layers of intent, each attribute at the greatest concept holding it, each case at the least."""
    n = len(concepts)
    layer = [len(x["intent"]) for x in concepts]
    layers = sorted(set(layer))
    rows = {k: [i for i in range(n) if layer[i] == k] for k in layers}
    W, gap = 760, 96
    H = gap * (len(layers) + 1)
    pos = {}
    for r, k in enumerate(layers):
        for j, i in enumerate(rows[k]):
            pos[i] = (W * (j + 1) / (len(rows[k]) + 1), gap * (r + 0.75))
    at_attr, at_obj = {}, {}
    for a, name in enumerate(attributes):
        having = sorted(o for o in range(len(objects)) if members[o][a] > 0.5)
        for i, x in enumerate(concepts):
            if sorted(objects.index(e) for e in x["extent"]) == having and name in x["intent"]:
                at_attr.setdefault(i, []).append(name)
                break
    for o, name in enumerate(objects):
        own = sorted(attributes[a] for a in range(len(attributes)) if members[o][a] > 0.5)
        for i, x in enumerate(concepts):
            if sorted(x["intent"]) == own:
                at_obj.setdefault(i, []).append(name)
                break
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {W} {H}" font-family="Source Serif 4, serif" font-size="12">']
    for lo, hi in covers:
        (x1, y1), (x2, y2) = pos[int(lo)], pos[int(hi)]
        out.append(f'<line x1="{x1:.1f}" y1="{y1:.1f}" x2="{x2:.1f}" y2="{y2:.1f}" stroke="#7D5A9E" stroke-opacity="0.55" stroke-width="1"/>')
    for i in range(n):
        x, y = pos[i]
        out.append(f'<circle cx="{x:.1f}" cy="{y:.1f}" r="6" fill="#F9F6F0" stroke="#7D5A9E" stroke-width="1.4"/>')
        if i in at_attr:
            out.append(f'<text x="{x:.1f}" y="{y - 11:.1f}" text-anchor="middle" fill="#7D5A9E" font-weight="600">{html.escape(", ".join(at_attr[i]))}</text>')
        if i in at_obj:
            out.append(f'<text x="{x:.1f}" y="{y + 21:.1f}" text-anchor="middle" fill="#B9607E" font-style="italic">{html.escape(", ".join(at_obj[i]))}</text>')
    out.append("</svg>")
    return "".join(out)


def create(api):
    class Concepts(api.BoardAnalysis):
        id = "concepts"
        title = "Concepts of cases and attributes"
        description = ("The formal concepts a table of cases and attributes holds (Ganter and Wille 1999): each a set of cases and the attributes they all share, "
                       "closed both ways, ordered in a lattice drawn with each attribute where it first appears and each case where it last does; and the "
                       "stem base of implications among the attributes, the fewest from which every implication that holds follows. A case has an attribute "
                       "when its value exceeds 0.5. The concepts are morphe's, found by NextClosure.")
        params = [api.param("table", "Cases", "", kind="text", role="table"),
                  c.col(api, "case", "Case"),
                  c.cols(api, "attributes", "Attributes", help="0 or 1, or memberships")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            attrs = c.words(params["attributes"])
            rows, dropped = c.complete(t, attrs, [params["case"]])
            names = c.text(t, params["case"])
            objects = [names[i] for i in rows]
            members = [[c.numbers(t, a)[i] for a in attrs] for i in rows]
            r = c.inquiry.ask("concepts", objects=objects, attributes=attrs, members=members)
            cs = {"concept": [], "cases": [], "attributes": [], "n_cases": []}
            for k, x in enumerate(r["concepts"]):
                cs["concept"].append(k + 1), cs["cases"].append(", ".join(x["extent"]) or "∅"), cs["attributes"].append(", ".join(x["intent"]) or "∅"), cs["n_cases"].append(len(x["extent"]))
            imp = {"if": [", ".join(i["premise"]) or "∅" for i in r["implications"]], "then": [", ".join(i["conclusion"]) for i in r["implications"]]}
            return [api.table("concepts", cs, title="Formal concepts"),
                    api.table("implications", imp, title="Implications among the attributes (stem base)"),
                    api.figure(hasse(r["concepts"], r["covers"], objects, attrs, members), title="The concept lattice"),
                    api.note(f"{len(objects)} cases and {len(attrs)} attributes hold {len(r['concepts'])} concepts and {len(r['implications'])} implications in their stem base." + c.left_out(dropped))]
    return Concepts()
