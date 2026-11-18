"""A need, area by area: the share of a statement's group in each area against the rest of the area's people, placed on the map."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _needs as n  # noqa: E402


def create(api):
    class Areas(api.BoardAnalysis):
        id = "areas"
        title = "A need, area by area"
        description = ("For one statement of need, each area's people in its group, the group's value of the indicator there and the rest's, and the part of "
                       "the data the area lies in (discovery, validation, confirmation), placed at the area's point; the map draws where the need is.")
        params = n.case_params(api) + [api.param("statement", "Statement", "need care for age >= 75 by comparative need_assist higher", kind="text")]

        def run(self, board, params, context):
            d, case = n.data_of(params)
            m = n.nd.ask("map", data=d, statement=str(params.get("statement") or ""))
            where = d.get("where", {})
            frame = d.get("place", {}).get("frame", {})
            import importlib.util
            spec = importlib.util.spec_from_file_location("needs_tables", Path(__file__).resolve().parents[5] / "data" / "products" / "needs_tables.py")
            nt = importlib.util.module_from_spec(spec)
            spec.loader.exec_module(nt)
            rows = []
            for p in m["places"]:
                if p["place"] not in where:
                    continue
                lat, lon = nt.lat_lon(frame, *where[p["place"]])
                rows.append({"area": p["place"], "lat": round(lat, 6), "lon": round(lon, 6), "group_people": p["group_people"], "group_value": n.num(p.get("group_value")),
                             "rest_value": n.num(p.get("rest_value")), "rest_people": p["rest_people"], "part": p["part"]})
            cols = n.columns(rows)
            return [api.table("need_by_area", cols, title=m["statement"], roles={"lat": "lat", "lon": "lon", "name": "area", "amount": "group_people"},
                              show={"kind": "map", "layer": "points", "amount": "group_people"}),
                    api.note(f"{m['statement']}: {len(rows)} areas. An area's own values are those of few people, and say where the need lies, not that it "
                             "holds there; the search judges the whole group.")]
    return Areas()
