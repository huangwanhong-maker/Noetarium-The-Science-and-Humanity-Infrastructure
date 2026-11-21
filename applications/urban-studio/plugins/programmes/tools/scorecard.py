"""A city's scorecard scored as its scale allows: each statement at an anchored level with the evidence it rests on, each part
aggregated by its declared rule (the weakest link, the median, a profile; never a mean of levels unless the scheme declares numbers
for them, and then marked), the scores without evidence named, the weaknesses taken as needs, and, where two assessors scored the
same city, their agreement by Krippendorff's ordinal α."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _programmes as P  # noqa: E402
from science import programmes as pg  # noqa: E402


def create(api):
    class Scorecard(api.BoardAnalysis):
        id = "scorecard"
        title = "Score a city's scorecard, as its scale allows"
        description = ("A scorecard's statements scored on anchored levels, each with the evidence it rests on, aggregated part by part by the "
                       "scheme's declared rules: the weakest link, the median, a profile of counts. A mean of levels is never taken unless the "
                       "scheme declares numbers for them, and then it is marked as assuming them equally spaced. Scores without evidence are "
                       "named; the statements at or below a declared level come out as needs, each with its evidence; and where two assessors "
                       "scored the same city, their agreement is measured. The example's cities, scores and evidence are invented.")
        params = [api.param("city", "City", "ardel", kind="text", help="ardel, brisa or corvo (the invented cohort)"),
                  api.param("weak_at", "Weak at or below", "1", kind="text", help="a level of the scheme")]

        def run(self, board, params, context):
            E = P.example()
            city = str(params.get("city") or "ardel").strip()
            if city not in E["cards"]:
                raise ValueError(f"no card for {city}: {', '.join(k for k in E['cards'] if '_' not in k)}")
            r = pg.score(E["scheme"], E["cards"][city], weak_at=str(params.get("weak_at") or E.get("weak_at", "1")))
            nodes = [n for n in r["nodes"] if n["node"] != r["root"]]
            out = [api.table("scorecard_parts", {"part": [n["title"] for n in nodes], "rule": [n["rule"] for n in nodes], "level": [n["code"] or "—" for n in nodes],
                                                 "decided by": [", ".join(n["decided_by"]) for n in nodes], "why": [n["why"] for n in nodes]},
                             title=f"{r['unit']}: the scorecard's parts, each by its rule")]
            root = next(n for n in r["nodes"] if n["node"] == r["root"])
            out.append(api.table("scorecard_profile", {"level": r["levels"], "parts": root["profile"]}, title="How many parts stand at each level (no single score)",
                                 show={"kind": "chart", "chart": "bar", "x": "level", "y": "parts", "how": "sum"}))
            w = r["weaknesses"]
            out.append(api.table("scorecard_weaknesses", {"statement": [x["statement"] for x in w], "level": [x["level"] for x in w], "text": [x["text"] for x in w],
                                                          "evidence": ["; ".join(e["text"] + (f" ({e['source']})" if e["source"] else "") for e in x["evidence"]) for x in w]},
                                 title="The weaknesses, as needs, with their evidence"))
            twins = [k for k in E["cards"] if k.startswith(city + "_")]
            agreed = ""
            if twins:
                a = pg.agreement(E["scheme"], [E["cards"][city]] + [E["cards"][k] for k in twins])
                agreed = f" Two assessors scored {r['unit']}: Krippendorff's ordinal α {a['alpha']:.2f}."
            out.append(api.note(f"{r['unit']} assessed on {r['date']}. {len(r['unjustified'])} score(s) without evidence"
                                + (f" ({', '.join(r['unjustified'])})" if r["unjustified"] else "") + f"; {len(w)} weakness(es) at {params.get('weak_at') or E.get('weak_at', '1')} or below."
                                + agreed + f" {E['said']}"))
            return out

    return Scorecard()
