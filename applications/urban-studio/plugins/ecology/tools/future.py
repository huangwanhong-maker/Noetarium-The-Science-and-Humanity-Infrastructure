"""The forest's future: the trees standing and the carbon they hold, year by year, as they grow, are removed and are planted."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def create(api):
    class Future(api.BoardAnalysis):
        id = "future"
        title = "The forest's future"
        description = ("The trees standing and the carbon they hold, year by year: each grows by its genus's increment, is removed at the end of the "
                       "remaining life an arborist stated (or by a declared hazard where none is stated), and a planting adds trees of a mix "
                       "each year. The expectation and its spread are exact for these presumptions; so is the planting that holds today's carbon.")
        params = f.column_params(api) + f.life_params(api) + f.allometry_params(api) + [
            api.param("years", "Years", 30, kind="integer", minimum=1, maximum=100),
            api.param("hazard", "Removal without a stated life", 0.03, kind="number", minimum=0, maximum=0.5, step=0.005,
                      help="the annual probability of removal of a tree whose remaining life is not stated, and after an open class (presumed)"),
            api.param("planting", "Trees planted a year", 0, kind="number", minimum=0, maximum=1e6),
            api.param("mix", "Planted mix", "", kind="text",
                      help="lines  Species; Genus; Family = share ; empty: the forest's own mix of its most common species"),
            api.param("planted_dbh", "Diameter at planting (cm)", 5, kind="number", minimum=1, maximum=30),
            api.param("planted_hazard", "Removal of the planted", 0.04, kind="number", minimum=0, maximum=0.5, step=0.005,
                      help="the annual probability that a planted tree is removed (young trees die more often; presumed)"),
            api.param("increment", "Increment where none is told (cm a year)", 0.8, kind="number", minimum=0, maximum=5,
                      help="for a forest whose inventory gives no ages, every genus grows by this (presumed)")]

        def run(self, board, params, context):
            trees, areas, t, info = f.read_trees(board, params, life=True)
            alloc = f.allometry(params)
            mix = f.mix_from_text(params["mix"]) or f.present_mix(trees)
            planting = {"per_year": float(params["planting"]), "mix": mix, "dbh": float(params["planted_dbh"]), "hazard": float(params["planted_hazard"])}
            context.progress(0.2, "the future, year by year")
            a = f.ecology.ask("future", trees=trees, scheme=alloc, years=int(params["years"]), removal={"hazard": float(params["hazard"])},
                              planting=planting, increment=float(params["increment"]), hold=True)
            years = a["t"]
            sd = a["carbon_sd_t"]
            cols = {"year": years, "standing": [round(x, 1) for x in a["standing"]], "standing_low": [round(x - 1.96 * s, 1) for x, s in zip(a["standing"], a["standing_sd"])],
                    "standing_high": [round(x + 1.96 * s, 1) for x, s in zip(a["standing"], a["standing_sd"])],
                    "carbon_t": [round(x, 1) for x in a["carbon_t"]], "carbon_low_t": [round(x - 1.96 * s, 1) for x, s in zip(a["carbon_t"], sd)],
                    "carbon_high_t": [round(x + 1.96 * s, 1) for x, s in zip(a["carbon_t"], sd)], "planted_standing": [round(x, 1) for x in a["planted_standing"]],
                    "removed": [round(x, 1) for x in a["removed"]], "largest_species": a["largest"]["species"],
                    "largest_species_share": [round(x, 4) for x in a["largest_share"]["species"]],
                    "largest_genus_share": [round(x, 4) for x in a["largest_share"]["genus"]],
                    "largest_family_share": [round(x, 4) for x in a["largest_share"]["family"]]}
            first, last = a["carbon_t"][0], a["carbon_t"][-1]
            hold = a.get("hold_per_year")
            hold_words = ("no planting is needed to hold today's carbon: the forest gains it as it grows" if hold is not None and hold <= 0
                          else f"holding today's carbon at the horizon needs about {hold:,.0f} trees a year of this mix" if hold is not None else "")
            mixed = ", ".join(f"{m['species'] or m['genus']}" for m in mix[:4]) + (" …" if len(mix) > 4 else "")
            return [
                api.table("future", cols, title="The forest year by year: standing trees and carbon, with the spread of the removals",
                          roles={"time": "year", "amount": "carbon_t"}, show={"kind": "chart", "chart": "line", "x": "year", "y": "carbon_t", "how": "sum"}),
                api.note(f"Over {params['years']} years the trees with a diameter go from {a['standing'][0]:,.0f} to {a['standing'][-1]:,.0f} standing "
                         f"(± {1.96 * a['standing_sd'][-1]:,.0f}), and their carbon from {first:,.0f} t to {last:,.0f} t (± {1.96 * sd[-1]:,.0f}), "
                         f"with {params['planting']:,.0f} trees planted a year ({mixed}); {hold_words}. "
                         f"In the last year the most common species would be {a['largest']['species'][-1]} at {f.share(a['largest_share']['species'][-1])}."),
                api.note(f"Presumed, not observed: increments of the genera as one inventory tells them (D/age where planting years are known; "
                         f"{params['increment']} cm a year where they are not); removal at the end of each stated life, uniform within its class, and by "
                         f"{params['hazard']} a year without one; planted trees removed at {params['planted_hazard']} a year; the allometry is "
                         f"{alloc['name']}. The spread is the removals' chance alone, given these." + f.aside_words(info)),
            ]
    return Future()
