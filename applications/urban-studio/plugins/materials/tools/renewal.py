"""What would renew the stock: the buildings as cohorts by age, retired by a declared lifetime law, replaced to keep a floor-area path, and the
materials that would enter and leave each year."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402


def create(api):
    class Renewal(api.BoardAnalysis):
        id = "renewal"
        title = "What would renew the stock"
        description = ("The buildings as cohorts by the year they were built, each retired by a declared lifetime law, and the floor area kept on a "
                       "path (as it stands, or growing by a share a year) by new buildings: how much floor area would be built and demolished each "
                       "year, and the materials that would enter and leave, with bounds. A dynamic stock model driven by the stock (Müller 2006); "
                       "where the path falls faster than buildings age out, the excess is retired early and said to be. The lifetime and the "
                       "intensity of new buildings are presumed, and said to be.")
        params = [api.param("table", "Buildings", "buildings", kind="text", role="table", help="one row a building"),
                  m.col(api, "floor_area", "Floor area (m²)", "floor_area_m2"),
                  m.col(api, "function", "Function (RS, RM or NR)", "function"),
                  m.col(api, "structure", "Structure (C, M, S or T)", "structure"),
                  m.col(api, "year", "Year built", "year"),
                  api.param("now", "From the end of the year", 2024, kind="number"),
                  api.param("years", "Years ahead", 26, kind="number", minimum=1, maximum=200),
                  api.param("growth", "Floor area grows by (% a year)", 0.0, kind="number")] + m.lifetime_params(api) + m.intensity_params(api)

        def run(self, board, params, context):
            t = board.table(params["table"])
            sch = m.scheme(params)
            area = m.nums(t, params["floor_area"])
            year = m.nums(t, params["year"])
            function = m.texts(t, params["function"], optional=True)
            structure = m.texts(t, params["structure"], optional=True)
            now = int(params["now"])
            keep = [i for i in range(t.rows) if area[i] is not None and area[i] >= 0]
            if not keep:
                raise ValueError("no building has a floor area")
            region = str(params["region"])
            a = m.materials.ask("buildings", scheme=sch, buildings={"floor_area": [area[i] for i in keep], "year": [year[i] for i in keep],
                                                                     "region": [region] * len(keep), "use": [function[i] for i in keep],
                                                                     "structure": [structure[i] for i in keep]},
                                renewal={"year": now, "years": int(params["years"]), "growth": float(params["growth"]) / 100, "lifetime": m.lifetime(params)})
            r = a["renewal"]
            years = [now + 1 + k for k in range(len(r["stock"]))]
            rows = {"year": years, "floor_area_m2": [round(v) for v in r["stock"]], "built_m2": [round(v) for v in r["inflow"]],
                    "demolished_m2": [round(v) for v in r["outflow"]], "retired_early_m2": [round(v) for v in r["retired_early"]],
                    "mean_age": [round(v, 1) for v in r["mean_age"]]}
            flows = {"year": [], "flow": [], "tonnes": [], "lower_t": [], "upper_t": []}
            mats = r["materials"]
            for k, y in enumerate(years):
                for flow in ("inflow", "outflow"):
                    c = sum(x[flow]["central"][k] for x in mats)
                    lo = sum(x[flow]["lower"][k] for x in mats)
                    hi = sum(x[flow]["upper"][k] for x in mats)
                    flows["year"].append(y), flows["flow"].append("built in" if flow == "inflow" else "demolished out")
                    flows["tonnes"].append(m.tonnes(c)), flows["lower_t"].append(m.tonnes(lo)), flows["upper_t"].append(m.tonnes(hi))
            by_material = {"material": [x["material"] for x in mats],
                           "in_t": [m.tonnes(sum(x["inflow"]["central"])) for x in mats], "out_t": [m.tonnes(sum(x["outflow"]["central"])) for x in mats],
                           "in_lower_t": [m.tonnes(sum(x["inflow"]["lower"])) for x in mats], "in_upper_t": [m.tonnes(sum(x["inflow"]["upper"])) for x in mats],
                           "out_lower_t": [m.tonnes(sum(x["outflow"]["lower"])) for x in mats], "out_upper_t": [m.tonnes(sum(x["outflow"]["upper"])) for x in mats]}
            built, gone = sum(r["inflow"]), sum(r["outflow"])
            out_c = sum(sum(x["outflow"]["central"]) for x in mats)
            out_lo = sum(sum(x["outflow"]["lower"]) for x in mats)
            out_hi = sum(sum(x["outflow"]["upper"]) for x in mats)
            in_c = sum(sum(x["inflow"]["central"]) for x in mats)
            early = sum(r["retired_early"])
            return [api.table("renewal", rows, title=f"The floor area and its renewal, {years[0]}–{years[-1]}",
                              roles={"time": "year", "amount": "built_m2"}, show={"kind": "chart", "chart": "line", "x": "year", "y": "demolished_m2", "how": "sum"}),
                    api.table("renewal_materials", flows, title="Materials built in and demolished out each year (tonnes, central)",
                              roles={"time": "year", "category": "flow", "amount": "tonnes"},
                              show={"kind": "chart", "chart": "line", "x": "year", "y": "tonnes", "split": "flow", "how": "sum"}),
                    api.table("renewal_by_material", by_material, title=f"Materials in and out, {years[0]}–{years[-1]}, by material (tonnes)",
                              roles={"name": "material", "amount": "out_t"}, show={"kind": "chart", "chart": "bar", "x": "material", "y": "out_t", "how": "sum"}),
                    api.note(f"Under {m.lifetime_words(params)}, keeping the floor area {'as it stands' if not float(params['growth']) else 'growing by ' + str(params['growth']) + '% a year'}, "
                             f"about {m.big(gone, ' m²')} would be demolished and {m.big(built, ' m²')} built from {years[0]} to {years[-1]}. "
                             f"The buildings retired would release about {m.big(out_c / 1000, ' tonnes')} of materials (between {m.big(out_lo / 1000)} and "
                             f"{m.big(out_hi / 1000)}); those built would take about {m.big(in_c / 1000, ' tonnes')}. The stock's mean age would go from "
                             f"{r['mean_age'][0]:.0f} to {r['mean_age'][-1]:.0f} years."
                             + (f" The path asks the stock to shrink faster than buildings age out: {m.big(early, ' m²')} retired early." if early > 0 else "")),
                    api.note(f"Presumed, not observed: the lifetime law; {r['presumed']}. The ages are the buildings' years of construction "
                             f"({m.big(r['left_out_area'], ' m²')} of floor area with no year, or none matched, left out). Refurbishment is not modelled: a building "
                             "lives until it is replaced.")]
    return Renewal()
