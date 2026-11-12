"""The canopy people live under: each area's measured canopy, weighted by who lives there."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Canopy(api.BoardAnalysis):
        id = "canopy"
        title = "The canopy people live under"
        description = ("Each area's share of ground under canopy, weighted by what lives in it (dwellings, people): the mean canopy over a "
                       "home, and the share of homes in areas below each threshold. Whether a city's shade reaches those who live in it is "
                       "a question of distribution, which a city's total cover hides.")
        params = [api.param("table", "Areas", "blocks", kind="text", role="table", help="one row an area, with its canopy cover"),
                  api.param("cover", "Canopy cover", "cover", kind="text", role="column", of="table", help="the share of the area under canopy, 0 to 1"),
                  api.param("weight", "Who lives there", "dwellings", kind="text", role="column", of="table", help="dwellings, people or any count of residents"),
                  api.param("name", "Name", "block_id", kind="text", role="column", of="table"),
                  api.param("thresholds", "Thresholds", "0.1, 0.2, 0.3", kind="text", help="covers below which an area counts as bare")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            cover, weight = t.column(params["cover"]), t.column(params["weight"])
            th = [float(x) for x in str(params["thresholds"]).replace(";", ",").split(",") if x.strip()]
            keep = [i for i in range(t.rows) if math.isfinite(cover[i]) and math.isfinite(weight[i])]
            a = f.ecology.ask("canopy", cover=[float(cover[i]) for i in keep], weight=[float(weight[i]) for i in keep], thresholds=th)
            plain_mean = sum(float(cover[i]) for i in keep) / max(1, len(keep))
            names = f._text(t, params["name"]) if params.get("name") and t.has(params["name"]) else [str(i) for i in range(t.rows)]
            rows = {"threshold": th, "share_of_homes_below": [round(x, 4) for x in a["share_below"]]}
            bare = sorted((i for i in keep if weight[i] > 0), key=lambda i: (cover[i], -weight[i]))[:12]
            words = "; ".join(f"{names[i]} ({f.share(cover[i])} under {weight[i]:,.0f})" for i in bare[:6])
            return [api.table("canopy_exposure", rows, title="The share of homes in areas below each canopy cover",
                              roles={"amount": "share_of_homes_below"}, show={"kind": "chart", "chart": "bar", "x": "threshold", "y": "share_of_homes_below", "how": "sum"}),
                    api.note(f"Over {f.count(a['weight'])} {params['weight']} in {len(keep)} areas, the mean canopy over a home is {f.share(a['mean'])}, "
                             f"against {f.share(plain_mean)} for the mean area: {'homes stand where shade is scarcer' if a['mean'] < plain_mean else 'homes stand where shade is more common'}. "
                             + ", ".join(f"{f.share(s)} of homes lie in areas under {f.share(x)}" for x, s in zip(th, a["share_below"]))
                             + (f". The barest areas with homes: {words}." if words else "."))]
    return Canopy()
