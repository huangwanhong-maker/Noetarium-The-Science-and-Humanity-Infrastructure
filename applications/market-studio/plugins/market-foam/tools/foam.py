"""The record's spacetime: a record of movements as one complex of events, subjects and holdings."""

import sys
from pathlib import Path

import numpy as np

sys.path.insert(0, str(Path(__file__).parent))
import _record as r  # noqa: E402

INK, FAINT, EVENT = "#8f6a9a", "#b9a7bd", "#c96f62"
HUES = ["#b9a2e0", "#e9b872", "#9fc2b8", "#e0a3b6", "#8fb3dd", "#c9d38a", "#e48f78", "#d987c6"]


def drawing(d: dict, subjects: list, kinds: list, begin: float, end: float, title: str) -> str:
    """The first events of the record drawn as its complex, time rising: each subject a line, each event a joint across the
    subjects it touches, each holding a band beside its holder, as wide as the holding is large, cut where an event changes it."""
    n = len(subjects)
    left, top, col, height = 70.0, 46.0, max(26.0, min(64.0, 1100.0 / max(1, n))), 620.0
    width = left + col * n + 30
    span = max(end - begin, 1e-12)
    y = lambda t: top + height - (t - begin) / span * height  # noqa: E731
    # Each kind is drawn to its own scale: a band is as wide as the holding is large among the holdings of its kind.
    most = [max([abs(v) for v, k in zip(d["value"], d["kind"]) if k == kind] + [1e-12]) for kind in range(len(kinds))]
    band = (col - 10) / max(1, len(kinds))
    out = [f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 {width:.0f} {top + height + 64:.0f}" font-family="Source Serif 4, Georgia, serif" font-size="13">',
           f'<text x="{left}" y="22" font-size="17" font-style="italic" fill="{INK}">{title}</text>']
    for s in range(n):
        x = left + col * s
        out.append(f'<line x1="{x:.1f}" y1="{top}" x2="{x:.1f}" y2="{top + height}" stroke="{INK}" stroke-width="0.8"/>')
        if n <= 40 or s % 2 == 0:
            out.append(f'<text x="{x:.1f}" y="{top + height + 14}" text-anchor="middle" fill="{INK}">{subjects[s]}</text>')
    for s, k, b, e, v in zip(d["subject"], d["kind"], d["begin"], d["end"], d["value"]):
        w = band * 0.92 * abs(v) / most[k]
        if w < 0.3 or e <= b:
            continue
        x = left + col * s + 3 + band * k
        dash = ' stroke-dasharray="2 2"' if v < 0 else ""   # a position below nothing: a liability
        out.append(f'<rect x="{x:.1f}" y="{y(e):.1f}" width="{w:.1f}" height="{max(0.6, y(b) - y(e)):.1f}" fill="{HUES[k % len(HUES)]}" fill-opacity="{0.34 if v >= 0 else 0.12}" '
                   f'stroke="{INK}" stroke-opacity="0.45" stroke-width="0.5"{dash}/>')
    for e in d["events"]:
        at = e["subjects"]
        if not at:
            continue
        yy = y(e["time"])
        out.append(f'<line x1="{left + col * min(at):.1f}" y1="{yy:.1f}" x2="{left + col * max(at):.1f}" y2="{yy:.1f}" stroke="{EVENT}" stroke-width="0.9"/>')
        for s in at:
            out.append(f'<circle cx="{left + col * s:.1f}" cy="{yy:.1f}" r="2.3" fill="{EVENT}"/>')
    for i in range(5):
        t = begin + span * i / 4
        out.append(f'<text x="{left - 12}" y="{y(t) + 4:.1f}" text-anchor="end" fill="{INK}">{t:.3g}</text>')
        out.append(f'<line x1="{left - 8}" y1="{y(t):.1f}" x2="{left - 3}" y2="{y(t):.1f}" stroke="{FAINT}"/>')
    for k, name in enumerate(kinds):
        x = left + 150 * k
        out.append(f'<rect x="{x:.1f}" y="{top + height + 30}" width="14" height="9" fill="{HUES[k % len(HUES)]}" fill-opacity="0.5" stroke="{HUES[k % len(HUES)]}"/>'
                   f'<text x="{x + 20:.1f}" y="{top + height + 38}" fill="{INK}">{name}</text>')
    out.append(f'<text x="{left + 150 * len(kinds)}" y="{top + height + 38}" fill="{INK}" font-style="italic">time rises; a joint is an event; a band is a holding, as wide as it is large for its kind, cut where an event changes it</text></svg>')
    return "".join(out)


def create(api):
    class Foam(api.BoardAnalysis):
        id = "foam"
        title = "The record's spacetime"
        description = ("A record of movements as one complex: each event a point, each subject a line through the events that touch it, each holding a face between "
                       "the events that change it. From it: how many events each subject lives through (its own time), how much of the record is in causal order, "
                       "and whether the books balance at every event. Where holdings are booked decides the causal order: at a ledger every movement of a kind "
                       "follows the last; as bearer holdings, movements between other pairs are unrelated.")
        params = [*r.common(api), r.opening(api),
                  api.param("settlement", "Holdings are", "bearer", kind="choice", options=[{"value": "bearer", "label": "held by their holders (bearer)"}, {"value": "ledger", "label": "booked at a ledger for each kind"}]),
                  api.param("draw", "Draw the first events", 60, kind="integer", minimum=0, maximum=400, help="how many of the record's first events the figure shows; 0 for no figure")]

        def run(self, board, params, context):
            s = r.stated(board, params, booked=params.get("settlement") == "ledger")
            context.progress(0.3, "building the complex")
            a = r.ask("foam", record=s["record"])
            proper = np.array(a["proper_time"])
            order = np.argsort(-proper)
            measures = [("events", a["events"]), ("subjects", len(s["subjects"])), ("holdings that move", a["facts"]), ("vertices", a["vertices"]), ("edges", a["edges"]),
                        ("faces", a["faces"]), ("Euler characteristic", a["euler"])]
            if "density" in a:
                measures += [("share of pairs of events in causal order", round(a["density"], 4)), ("longest chain of events", a["longest"] + 1)]
            measures += [("greatest imbalance at an event", a["imbalance"]), ("boundary of a boundary", a["boundary_defect"])]
            results = [
                api.table("own_time", {"subject": [s["subjects"][i] for i in order], "events": proper[order].astype(int), "share_of_events": np.round(proper[order] / max(1, a["events"]), 4)},
                          title="Each subject's own time: the events it lives through", roles={"name": "subject", "amount": "events"},
                          show={"kind": "chart", "chart": "bar", "x": "subject", "y": "events", "how": "sum", "most": 30}),
                api.table("spacetime", {"measure": [m[0] for m in measures], "value": [float(m[1]) for m in measures]}, title="The record's complex, counted", roles={"name": "measure", "amount": "value"}),
            ]
            how = "booked at a ledger for each kind" if params.get("settlement") == "ledger" else "held as bearer holdings"
            causal = (f" With holdings {how}, {100 * a['density']:.1f}% of the pairs of events are in causal order, and the longest chain runs through {a['longest'] + 1:,} events."
                      if "density" in a else " The record is too long for its causal order to be held whole: take a slice of it.")
            results.append(api.note(
                f"{a['events']:,} events among {len(s['subjects'])} subjects make a complex of {a['vertices']:,} vertices, {a['edges']:,} edges and {a['faces']:,} faces. "
                f"The busiest subject lives through {int(proper.max()):,} events, the quietest through {int(proper.min()):,}: they age at different rates in one record."
                f"{causal} Every kind's total is kept at every event (greatest imbalance {a['imbalance']:.2g})."
                + (f" {s['dropped']:,} rows without a time, a quantity or two parties were left out." if s["dropped"] else "")))
            # The figure: the first events, with their faces.
            count = int(params.get("draw", 60))
            if count > 0:
                context.progress(0.7, "drawing")
                rec = s["record"]
                last = sorted(set(rec["event"]))[:count][-1]
                keep = [i for i, e in enumerate(rec["event"]) if e <= last]
                part = {**rec, **{k: [rec[k][i] for i in keep] for k in ("event", "time", "from", "to", "kind", "quantity")}}
                part["end"] = part["time"][-1] + 0.04 * max(part["time"][-1] - part["begin"], 1e-9)
                # Only the subjects these events touch, in the order they first appear, so that the drawing is as narrow as it can be.
                seen = list(dict.fromkeys(x for i in range(len(part["from"])) for x in (part["from"][i], part["to"][i])))
                renumber = {old: new for new, old in enumerate(seen)}
                part.update({"subjects": len(seen), "from": [renumber[x] for x in part["from"]], "to": [renumber[x] for x in part["to"]]})
                if "opening" in part:
                    part["opening"] = [part["opening"][old] for old in seen]
                d = r.ask("foam", record=part, faces=40000).get("drawing")
                if d:
                    results.append(api.figure(drawing(d, [s["subjects"][old] for old in seen], s["kinds"], part["begin"], part["end"],
                                                      f"The first {len(set(part['event']))} events of the record as its complex"), "The record's first events as a complex"))
            return results
    return Foam()
