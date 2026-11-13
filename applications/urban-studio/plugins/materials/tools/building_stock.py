"""What a city's buildings hold: each building's mass of eight materials by a declared scheme of intensities, with bounds, by group and in all."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class BuildingStock(api.BoardAnalysis):
        id = "building_stock"
        title = "What the buildings hold"
        description = ("Each building's mass of concrete, brick, wood, steel, glass, plastics, aluminium and copper: its floor area times a material "
                       "intensity of a declared scheme (RASMI by default) for its region, function and structure. Where a building's function or "
                       "structure is not recorded, its bounds widen over every value the scheme admits, rather than guessing one. Floor areas are "
                       "the table's; intensities are presumed, and said to be; buildings no row matches are counted, not filled.")
        params = [api.param("table", "Buildings", "buildings", kind="text", role="table", help="one row a building"),
                  m.col(api, "floor_area", "Floor area (m²)", "floor_area_m2"),
                  m.col(api, "function", "Function (RS, RM or NR)", "function", help="empty where not known"),
                  m.col(api, "structure", "Structure (C, M, S or T)", "structure", help="empty where not known"),
                  m.col(api, "group", "Group", "block", help="a block, a use, a decade: what the stock is summed by"),
                  m.col(api, "lat", "Latitude", "latitude"),
                  m.col(api, "lon", "Longitude", "longitude"),
                  api.param("each", "Each building", "on the map", kind="choice", options=["on the map", "summed only"],
                            help="summed only when another step already draws the same buildings")] + m.intensity_params(api)

        def run(self, board, params, context):
            t = board.table(params["table"])
            sch = m.scheme(params)
            area = m.nums(t, params["floor_area"])
            function = m.texts(t, params["function"], optional=True)
            structure = m.texts(t, params["structure"], optional=True)
            group = m.texts(t, params["group"], optional=True)
            lat, lon = m.nums(t, params["lat"], optional=True), m.nums(t, params["lon"], optional=True)
            keep = [i for i in range(t.rows) if area[i] is not None and area[i] >= 0]
            if not keep:
                raise ValueError("no building has a floor area")
            context.progress(0.3, f"{len(keep):,} buildings")
            region = str(params["region"])
            a = m.materials.ask("buildings", scheme=sch, per_building=True, group=[group[i] or "(none)" for i in keep],
                                buildings={"id": [str(i) for i in keep], "region": [region] * len(keep), "use": [function[i] for i in keep],
                                           "structure": [structure[i] for i in keep], "floor_area": [area[i] for i in keep]})
            mats = a["materials"]
            K = len(mats)
            per = a["per_building"]
            rows = {"group": [], "lat": [], "lon": [], "floor_area_m2": [], "function": [], "total_t": [], "total_lower_t": [], "total_upper_t": []}
            for k in mats:
                rows[f"{k}_t"] = []
            for n, i in enumerate(keep):
                c, lo, hi = per["central"][n], per["lower"][n], per["upper"][n]
                if c[0] is None:
                    continue  # no row matched; counted below
                rows["group"].append(group[i]), rows["lat"].append(lat[i]), rows["lon"].append(lon[i])
                rows["floor_area_m2"].append(area[i]), rows["function"].append(function[i] or "unknown")
                rows["total_t"].append(m.tonnes(sum(c))), rows["total_lower_t"].append(m.tonnes(sum(lo))), rows["total_upper_t"].append(m.tonnes(sum(hi)))
                for k, name in enumerate(mats):
                    rows[f"{name}_t"].append(m.tonnes(c[k]))
            by_group = {"group": [], "buildings": [], "floor_area_m2": [], "total_t": [], "total_lower_t": [], "total_upper_t": [], "t_per_m2": []}
            for name, g in sorted(a["by_group"].items(), key=lambda kv: -sum(kv[1]["central"])):
                by_group["group"].append(name), by_group["buildings"].append(g["buildings"]), by_group["floor_area_m2"].append(round(g["floor_area"]))
                by_group["total_t"].append(m.tonnes(sum(g["central"]))), by_group["total_lower_t"].append(m.tonnes(sum(g["lower"])))
                by_group["total_upper_t"].append(m.tonnes(sum(g["upper"])))
                by_group["t_per_m2"].append(round(sum(g["central"]) / g["floor_area"] / 1000, 3) if g["floor_area"] else None)
            tot = a["total"]
            total = {"material": mats, "lower_t": [m.tonnes(v) for v in tot["lower"]], "central_t": [m.tonnes(v) for v in tot["central"]],
                     "upper_t": [m.tonnes(v) for v in tot["upper"]], "kg_per_m2": [round(v / tot["floor_area"], 1) if tot["floor_area"] else None for v in tot["central"]]}
            central, lower, upper = sum(tot["central"]), sum(tot["lower"]), sum(tot["upper"])
            heaviest = max(range(K), key=lambda k: tot["central"][k])
            unknown = sum(1 for i in keep if not function[i])
            unmatched = len(a["unmatched"])
            each = [api.table("building_materials", rows, title="What each building holds (tonnes; central, with the bounds of its total)",
                              roles={"lat": "lat", "lon": "lon", "name": "group", "amount": "total_t", "category": "function"},
                              show={"kind": "layer", "layer": "points", "colour": "total_t", "size": "total_t", "map": "gold"})]
            out = (each if params.get("each", "on the map") == "on the map" else []) + [
                   api.table("materials_by_group", by_group, title=f"The material stock by {params['group'] or 'group'} (tonnes)",
                             roles={"name": "group", "amount": "total_t"}, show={"kind": "chart", "chart": "bar", "x": "group", "y": "total_t", "how": "sum"}),
                   api.table("materials_total", total, title="The material stock by material (tonnes)",
                             roles={"name": "material", "amount": "central_t"}, show={"kind": "chart", "chart": "bar", "x": "material", "y": "central_t", "how": "sum"}),
                   api.note(f"{tot['buildings']:,} buildings of {m.big(tot['floor_area'], ' m²')} hold about {m.big(central / 1000, ' tonnes')} of these eight "
                            f"materials, between {m.big(lower / 1000)} and {m.big(upper / 1000)}: {central / tot['floor_area']:,.0f} kg per m², "
                            f"{mats[heaviest]} the most ({100 * tot['central'][heaviest] / central:.0f}%)."),
                   api.note(f"Presumed, not observed: the intensities are {sch['name']}; the bounds are the scheme's range for each building's region, "
                            f"function and structure, and widen over every structure, since none is recorded"
                            + (f", and over every function for the {unknown:,} buildings whose use is not known" if unknown else "")
                            + (f". {unmatched:,} building{'' if unmatched == 1 else 's'} of {m.big(a['unmatched_area'], ' m²')} that no row of the scheme "
                               f"matches {'is' if unmatched == 1 else 'are'} counted and left out"
                               if unmatched else "; every building matched a row of the scheme")
                            + ". Floor areas are the table's.")]
            return out
    return BuildingStock()
