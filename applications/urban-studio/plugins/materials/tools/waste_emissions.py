"""What a city's waste emits where it is treated, by the IPCC's methods, carried into the community inventory's waste sector: the waste
account's facilities with their treatment, each disposal site's history of deposits decaying by first-order decay, composting by its
factors, and the uncollected waste burnt in the open as declared; the entries in the inventory tool's own columns (ADR 0153)."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402
import _waste as W  # noqa: E402
from science import waste as ws  # noqa: E402


def create(api):
    class WasteEmissions(api.BoardAnalysis):
        id = "waste_emissions"
        title = "What the city's waste emits, carried into its inventory"
        description = ("The waste account's facilities, each with its treatment: a disposal site's methane by the IPCC's first-order decay over "
                       "its history of deposits (declared from the year it opened, at the account's amount grown back at a declared rate), "
                       "composting by its factors, and a declared share of the uncollected waste burnt in the open. The composition is the "
                       "survey's, its fractions mapped onto the IPCC's components as declared. The entries come out in the inventory tool's own "
                       "columns, by the GPC's codes: the city's waste treated inside in scope 1, outside in scope 3. The defaults are the 2006 "
                       "Guidelines' or the 2019 Refinement's, each cited; every amount, history and share here is invented.")
        params = [api.param("households", "Households weighed", "households", kind="text", role="table"),
                  api.param("strata", "Households by stratum", "strata", kind="text", role="table"),
                  api.param("facilities", "Facilities", "facilities", kind="text", role="table"),
                  api.param("ladder", "Ladder of control", "ladder", kind="text", role="table"),
                  api.param("sorted", "Sorted samples", "sorted", kind="text", role="table"),
                  api.param("sites", "Treatment at each facility", "sites", kind="text", role="table",
                            help="facility, treatment (sorting, composting, digestion, disposal), site_type, opened, oxidising_cover (yes/no), outside (yes/no)"),
                  api.param("controlled_from", "Controlled from the level", "basic control", kind="text"),
                  api.param("other", "Other sources (t a day: name=amount, …)", "markets=25, commerce=45, institutions=12", kind="text"),
                  api.param("not_delivered", "Collected and not delivered (t a day)", 8, minimum=0),
                  api.param("components", "Fractions as the IPCC's components", "organic=food, paper=paper, plastic=plastics, glass=glass, metal=metal, other=other", kind="text"),
                  api.param("year", "The inventory's year", 2024, minimum=1990, step=1),
                  api.param("growth", "Growth of waste a year, back to a site's opening", 0.03, minimum=0, maximum=0.2, step=0.005),
                  api.param("burnt", "Share of uncollected waste burnt in the open", 0.3, minimum=0, maximum=1, step=0.05),
                  api.param("climate", "Climate zone", "tropical wet", kind="choice", options=["temperate dry", "temperate wet", "tropical dry", "tropical wet"]),
                  api.param("edition", "IPCC defaults", "2006", kind="choice", options=["2006", "2019"])]

        def run(self, board, params, context):
            r, R = W.account(board, params)
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            year = int(g("year", 2024))
            if not r.get("composition"):
                raise ValueError("the composition needs the sorted samples")
            mapping = {}
            for part in str(params.get("components") or "").split(","):
                if "=" in part:
                    k, v = part.split("=", 1)
                    mapping[k.strip()] = v.strip()
            comp = {}
            for x in r["composition"]:
                if x["fraction"] not in mapping:
                    raise ValueError(f"the fraction {x['fraction']} is not mapped onto a component")
                comp[mapping[x["fraction"]]] = comp.get(mapping[x["fraction"]], 0) + x["value"]
            total = sum(comp.values())
            comp = {k: v / total for k, v in comp.items()}
            st = board.table(params["sites"])
            treat = {f: (t.strip().lower(), ty.strip(), op, cv.strip().lower() == "yes", out.strip().lower() == "yes")
                     for f, t, ty, op, cv, out in zip(m.texts(st, "facility"), m.texts(st, "treatment"), m.texts(st, "site_type", optional=True),
                                                      m.nums(st, "opened", optional=True), m.texts(st, "oxidising_cover", optional=True), m.texts(st, "outside", optional=True))}
            growth = g("growth", 0.03)
            sites, composting, digestion, burning, rows = [], [], [], [], []
            for f in r["facilities"]:
                t = treat.get(f["id"])
                if t is None:
                    raise ValueError(f"the facility {f['id']} has no treatment declared")
                kind, typ, opened, cover, outside = t
                per_year = f["received"] * 365
                origin = "outside" if outside else "inside"
                if kind == "disposal":
                    if opened is None or opened > year:
                        raise ValueError(f"the disposal site {f['id']} needs the year it opened, no later than {year}")
                    dep = {str(y): per_year / (1 + growth) ** (year - y) for y in range(int(opened), year + 1)}
                    sites.append({"id": f["id"], "type": typ, "oxidising_cover": cover, "composition": comp, "deposited": dep, "origin": origin})
                elif kind == "composting":
                    composting.append({"id": f["id"], "tonnes": per_year, "origin": origin})
                elif kind == "digestion":
                    digestion.append({"id": f["id"], "tonnes": per_year, "origin": origin})
                rows.append((f["id"], kind, per_year))
            burnt = r["uncollected"] * 365 * g("burnt", 0.3)
            if burnt > 0:
                burning.append({"id": "uncollected, burnt in the open", "tonnes": burnt, "composition": comp, "origin": "inside"})
            a = ws.ask("waste.emissions", edition=str(params.get("edition") or "2006"), climate=str(params.get("climate") or "tropical wet"), year=year,
                                 potentials="AR6", sites=sites, composting=composting, digestion=digestion, burning=burning)
            E = a["entries"]
            out = [api.table("waste_emissions_entries", {
                "code": [e["id"].split(" ", 1)[0] for e in E], "source": [e["id"].split(" ", 1)[1] for e in E], "scope": [int(e["scope"]) for e in E],
                "gas": [e["gas"] for e in E], "tonnes": [round(e["kg"] / 1000, 2) for e in E], "t CO2e": [round(e["t_co2e"], 1) for e in E],
                "biogenic": ["yes" if e["biogenic"] else "" for e in E]},
                title=f"What the waste emits in {year}, by the GPC's codes ({a['edition']}; {a['potentials']})")]
            series = []
            for s in a["sites"]:
                series += [(s["id"], int(x["year"]), x["emitted"]) for x in s["series"]]
            out.append(api.table("waste_site_methane", {"year": [y for _, y, _ in series], "site": [n for n, _, _ in series], "t CH4": [round(v, 1) for _, _, v in series]},
                                 title="Methane emitted by each disposal site, year by year, as its deposits decay",
                                 show={"kind": "chart", "chart": "line", "x": "year", "y": "t CH4", "how": "mean", "split": "site"}))
            out.append(api.table("waste_for_inventory", {
                "id": [e["id"] for e in E], "sector": ["waste"] * len(E), "scope": [int(e["scope"]) for e in E],
                "subsector": [e["id"].split(" ", 1)[1].split(" ")[0] for e in E], "gas": [e["gas"] for e in E], "activity": [None] * len(E), "unit": [""] * len(E),
                "factor": [None] * len(E), "amount": [round(e["kg"]) for e in E], "u_activity": [0.3] * len(E), "u_factor": [None] * len(E), "key": [""] * len(E),
                "included_in": [""] * len(E), "flag": ["biogenic" if e["biogenic"] else ("imported_waste" if e["imported_waste"] else "") for e in E]},
                title="The waste sector in the inventory tool's own columns, to add to a city's inventory (an uncertainty of 30% declared)"))
            sh = {s["id"]: s["emitted"] for s in a["sites"]}
            text = (f"In {year} the city's waste emits {a['basic']:,.0f} t CO2e within BASIC by the {a['edition']} defaults and AR6's potentials"
                    + (f", with {a['biogenic']:,.0f} t of biogenic CO2 reported apart" if a["biogenic"] > 0 else "") + ". "
                    + "; ".join(f"{k} {v:,.0f} t CH4 as its deposits since it opened decay" for k, v in sh.items()) + ". "
                    + (f"Composting {sum(c['tonnes'] for c in composting):,.0f} t a year adds {sum(t['gases']['ch4'] for t in a['treatments'] if t['what'] == 'composting'):,.1f} t CH4. " if composting else "")
                    + (f"Burning {burnt:,.0f} t of the uncollected waste in the open, as declared, emits {sum(t['gases']['co2_fossil'] for t in a['treatments'] if t['what'] == 'open burning'):,.0f} t of fossil CO2. " if burning else "")
                    + "A site's history is declared, not measured; the GPC's own equations depart from the IPCC's in three places, and the IPCC's are followed.")
            out.append(api.note(text))
            return out

    return WasteEmissions()
