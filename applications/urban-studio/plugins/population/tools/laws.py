"""Laws of its people, learned and judged: a place's population field learned as balance laws of a fluid on its cells, each judged on
the years kept aside."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _population as p  # noqa: E402


def create(api):
    class Laws(api.BoardAnalysis):
        id = "laws"
        title = "Laws of its people, learned and judged"
        description = ("The field learned as laws of a fluid on its cells, as kinesis fits them: growth everywhere at one rate, logistic growth, growth and "
                       "spread (Fisher's law), spread and drift toward the centre, crowding and drift, a law thresholded from every term; beside them a rate "
                       "of its own for each cell and the record's dynamic modes. Each is fitted on the years before those kept aside (the integral form, then "
                       "runs fitted to the record) and judged on them against carrying the last field forward: its skill, and its skill at placing the "
                       "change once the total is set right. A law of one's own is declared as its terms, e.g. 'linear, quadratic, diffusion, drift:centre'.")
        params = p.case_params(api) + [
            api.param("held_out", "Years kept aside (epochs)", 2, minimum=1, maximum=4, step=1),
            api.param("weights", "Weights", "poisson", kind="choice", options=["poisson", "none"], help="poisson: each cell's error grows with its count"),
            api.param("law", "A law of one's own (its terms)", "", kind="text", help="constant, linear, quadratic, gompertz, diffusion, crowding, drift:centre, drift:settled, source:settled")]

        def run(self, board, params, context):
            f, case = p.field_of(params)
            request = {"held_out": int(params.get("held_out") or 2), "weights": params.get("weights") or "poisson"}
            law = [t.strip() for t in str(params.get("law") or "").replace(";", ",").split(",") if t.strip()]
            if law:
                request["law"] = law
            L = p.pd.ask("learn", field=f, **request)
            ms = L["models"]
            cols = {"model": [m["title"] for m in ms], "skill": [p.num((m.get("held_out") or {}).get("skill")) for m in ms],
                    "pattern_skill": [p.num((m.get("held_out") or {}).get("pattern_skill")) for m in ms],
                    "change_correlation": [p.num((m.get("held_out") or {}).get("change_correlation")) for m in ms],
                    "total_error": [p.num((m.get("held_out") or {}).get("total_error")) for m in ms],
                    "parameters": [m.get("parameters") for m in ms],
                    "law": [p.reading_words(m["reading"]) if m.get("reading") else (m.get("error") or "") for m in ms]}
            out = [api.table("laws", cols, title=f"{f['title'].split(':')[0]}: laws judged on {', '.join(f'{y:.0f}' for y in L['held_out'])}",
                             show={"kind": "chart", "chart": "bar", "x": "model", "y": "pattern_skill", "how": "sum"})]
            best = L.get("best")
            if best:
                cs = p.cells(f, case)
                last = L["held_out"][-1]
                rec = f["values"][str(int(last))]
                fc = best["frames"][-1]
                out.append(api.table("forecast", {"cell": [f"r{c['row']:03d}c{c['col']:03d}" for c in cs], "lat": [c.get("lat") for c in cs], "lon": [c.get("lon") for c in cs],
                                                  "recorded": [rec[c["row"]][c["col"]] for c in cs], "forecast": fc,
                                                  "error": [b - (a or 0) for a, b in zip([rec[c["row"]][c["col"]] for c in cs], fc)]},
                                     title=f"The best law's forecast for {last:.0f} against the record", roles={"lat": "lat", "lon": "lon", "name": "cell", "amount": "error"},
                                     show={"kind": "layer", "layer": "points", "colour": "error", "map": "lavender-rust"}))
            ranked = sorted([m for m in ms if m.get("held_out")], key=lambda m: -m["held_out"]["pattern_skill"])
            words = [f"Fitted on {L['fitted_on'][0]:.0f}–{L['fitted_on'][-1]:.0f} and judged on {', '.join(f'{y:.0f}' for y in L['held_out'])}."]
            if ranked:
                b = ranked[0]
                words.append(f"Placing the change best: {b['title']} (pattern skill {b['held_out']['pattern_skill']:+.2f}, skill {b['held_out']['skill']:+.2f}, total off by {p.pct(b['held_out']['total_error'])}).")
            for m in ms:
                if m.get("reading") and m["name"] in ("fisher", "drift"):
                    words.append(f"{m['title']}: {p.reading_words(m['reading'])}.")
                for d in m.get("dropped", []):
                    words.append(f"{m['title']}: {d}.")
            out.append(api.note(" ".join(words)))
            out.append(api.note("Skill is against carrying the last field forward: above zero, better. The pattern's skill sets each forecast's total to the "
                                "record's first, so that it judges where the people went apart from how many there were, which no law of the field can know. "
                                "Standard errors treat neighbouring cells as independent, and so understate."))
            return out
    return Laws()
