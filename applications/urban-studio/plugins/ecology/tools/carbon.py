"""Carbon tree by tree, placed: what each tree holds by a declared allometry, for the map."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Carbon(api.BoardAnalysis):
        id = "carbon"
        title = "Carbon, tree by tree"
        description = ("Each tree's above-ground and root biomass, carbon and CO₂ by a declared allometry, with the rank of the scheme that matched it "
                       "and whether its diameter lies outside the source's, as a table with each tree's place, to draw on the map or open in the "
                       "Workbench. Presumed, not observed.")
        params = f.column_params(api) + [
            api.param("lat", "Latitude", "lat", kind="text", role="column", of="table"),
            api.param("lon", "Longitude", "lon", kind="text", role="column", of="table")] + f.allometry_params(api)

        def run(self, board, params, context):
            t = board.table(params["table"])
            alloc = f.allometry(params)
            largest = float(params.get("largest") or 0)
            lat, lon = t.column(params["lat"]), t.column(params["lon"])
            species, genus, family = (f._text(t, params[k]) or [""] * t.rows for k in ("species", "genus", "family"))
            dbh = t.column(params["dbh"])
            keep = [i for i in range(t.rows) if math.isfinite(dbh[i]) and dbh[i] > 0 and (largest <= 0 or dbh[i] <= largest)
                    and math.isfinite(lat[i]) and math.isfinite(lon[i])]
            aside = sum(1 for i in range(t.rows) if math.isfinite(dbh[i]) and largest > 0 and dbh[i] > largest)
            if not keep:
                raise ValueError("no tree has a diameter and a place in this slice")
            # One request: the trees with a diameter, each its own area, so that the engine's stock by area is the tree's own estimate.
            context.progress(0.3, f"{len(keep):,} trees")
            groups = {}
            for i in keep:
                key = (species[i], genus[i], family[i], round(float(dbh[i]), 1))
                groups.setdefault(key, []).append(i)
            keys = list(groups)
            a = f.ecology.ask("forest", trees=f.ecology.trees([k[0] for k in keys], [k[1] for k in keys], [k[2] for k in keys], [k[3] for k in keys],
                                                               area=list(range(len(keys)))),
                              areas=[str(k) for k in range(len(keys))], scheme=alloc, limits={}, top=1)
            carbon = {int(x["area"]): x["stock"]["carbon_t"] * 1000 for x in a["areas"]}
            match = {int(x["area"]): next((r for r in ("species", "genus", "family", "fallback") if x["stock"]["matched"][r] > 0), "") for x in a["areas"]}
            outside = {int(x["area"]): x["stock"]["outside_range"] > 0 for x in a["areas"]}
            cols = {"lat": [], "lon": [], "species": [], "genus": [], "dbh_cm": [], "carbon_kg": [], "co2e_kg": [], "matched": [], "outside_range": []}
            for k, key in enumerate(keys):
                c = carbon.get(k)
                for i in groups[key]:
                    cols["lat"].append(float(lat[i]))
                    cols["lon"].append(float(lon[i]))
                    cols["species"].append(species[i] or genus[i])
                    cols["genus"].append(genus[i])
                    cols["dbh_cm"].append(float(dbh[i]))
                    cols["carbon_kg"].append(round(c, 1) if c is not None else None)
                    cols["co2e_kg"].append(round(c * 44 / 12, 1) if c is not None else None)
                    cols["matched"].append(match.get(k, ""))
                    cols["outside_range"].append("outside" if outside.get(k) else "")
            total = sum(x for x in cols["carbon_kg"] if x)
            return [api.table("tree_carbon", cols, title="The carbon each tree holds (presumed)",
                              roles={"lat": "lat", "lon": "lon", "name": "species", "amount": "carbon_kg", "category": "matched"},
                              show={"kind": "layer", "layer": "points", "colour": "carbon_kg", "size": "carbon_kg", "map": "sea"}),
                    api.note(f"{len(keep):,} trees placed, holding {total / 1000:,.0f} t of carbon by {alloc['name']}. Presumed, not observed: each tree's "
                             f"estimate stands on its rank's equation; `matched` says which, `outside_range` where its diameter exceeds the source's."
                             + f.aside_words({"set_aside": aside, "largest": largest}))]
    return Carbon()
