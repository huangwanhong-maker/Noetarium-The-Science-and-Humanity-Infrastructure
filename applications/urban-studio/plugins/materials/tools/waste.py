"""A city's municipal solid waste from what households and others generate to what is managed in controlled facilities, and SDG
indicator 11.6.1 with its interval, after the seven steps of UN-Habitat's Waste Wise Cities Tool (ADR 0148)."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _materials as m  # noqa: E402
from science import waste as ws  # noqa: E402


def create(api):
    class Waste(api.BoardAnalysis):
        id = "waste"
        title = "A city's solid waste, from generation to controlled facilities (SDG 11.6.1)"
        description = ("Households weighed in a stratified cluster survey (strata by income, areas as clusters, each household's waste over the "
                       "days weighed), other sources declared, facilities with what they receive and recover and where a recovery facility sends "
                       "its residue, and a ladder of control whose criteria each facility meets or not. What households generate with its "
                       "interval, per person by stratum; each facility's level of control; the account from generated to collected to managed "
                       "in controlled facilities, residues counted once at the disposal site's gate; SDG 11.6.1 with the survey's interval "
                       "carried through; collection coverage, recovery and the composition. Every ladder, amount and design is declared.")
        params = [api.param("households", "Households weighed", "households", kind="text", role="table", help="stratum, area, persons, kg, days"),
                  api.param("strata", "Households by stratum", "strata", kind="text", role="table", help="stratum, households"),
                  api.param("facilities", "Facilities", "facilities", kind="text", role="table", help="id, name, kind, received, recovered, residue_to, met (criteria, ';')"),
                  api.param("ladder", "Ladder of control", "ladder", kind="text", role="table", help="level, name, criterion"),
                  api.param("sorted", "Sorted samples", "sorted", kind="text", role="table", help="stratum, area, then a column a fraction; a table not on the board is left out"),
                  api.param("controlled_from", "Controlled from the level", "basic control", kind="text"),
                  api.param("other", "Other sources (t a day: name=amount, …)", "markets=25, commerce=45, institutions=12", kind="text"),
                  api.param("not_delivered", "Collected and not delivered (t a day)", 8, minimum=0)]

        def run(self, board, params, context):
            h = board.table(params["households"])
            sample = [ws.household(int(st), int(ar), p, k, d) for st, ar, p, k, d in zip(m.nums(h, "stratum"), m.nums(h, "area"), m.nums(h, "persons"), m.nums(h, "kg"), m.nums(h, "days"))]
            st = board.table(params["strata"])
            strata = {int(s): H for s, H in zip(m.nums(st, "stratum"), m.nums(st, "households"))}
            names = dict(zip([int(x) for x in m.nums(st, "stratum")], m.texts(st, "name", optional=True)))
            lt = board.table(params["ladder"])
            levels, criteria = [], []
            for lv, name, crit in sorted(zip(m.nums(lt, "level"), m.texts(lt, "name"), m.texts(lt, "criterion", optional=True)), key=lambda r: r[0]):
                if not levels or levels[-1] != name:
                    levels.append(name), criteria.append([])
                if crit:
                    criteria[-1].append(crit)
            frm = str(params.get("controlled_from") or "").strip()
            if frm not in levels:
                raise LookupError(f"the ladder has no level '{frm}': {', '.join(levels)}")
            ladder = {"levels": levels, "criteria": criteria, "controlled_from": levels.index(frm)}
            ft = board.table(params["facilities"])
            facilities = [ws.facility(i, k.strip().lower(), rc or 0.0, rv or 0.0, to, [c.strip() for c in met.split(";") if c.strip()], name=n)
                          for i, n, k, rc, rv, to, met in zip(m.texts(ft, "id"), m.texts(ft, "name", optional=True), m.texts(ft, "kind"), m.nums(ft, "received"),
                                                              m.nums(ft, "recovered", optional=True), m.texts(ft, "residue_to", optional=True), m.texts(ft, "met", optional=True))]
            other = {}
            for part in str(params.get("other") or "").split(","):
                if "=" in part:
                    k, v = part.split("=", 1)
                    other[k.strip()] = float(v)
            sorted_samples = []
            sname = str(params.get("sorted") or "").strip()
            if sname and sname in board.tables:
                so = board.table(sname)
                fractions = [c for c in so.columns if c not in ("stratum", "area")]
                cols = {f: m.nums(so, f) for f in fractions}
                for i, (s_, a_) in enumerate(zip(m.nums(so, "stratum"), m.nums(so, "area"))):
                    sorted_samples.append(ws.sorted_sample(int(s_), int(a_), {f: cols[f][i] or 0.0 for f in fractions}))
            r = ws.account(ws.survey(strata, sample), ladder, facilities, other=other, not_delivered=float(params.get("not_delivered") or 0), sorted=sorted_samples)
            fr = r["facilities"]
            out = [api.table("waste_account", {
                "stage": ["generated by households", "generated by other sources", "generated", "collected", "delivered to facilities", "recovered",
                          "disposed", "managed in controlled facilities", "uncollected"],
                "t a day": [round(x, 1) for x in (r["households"]["value"], r["other"], r["generated"], r["collected"], r["delivered"], r["recovered"],
                                                  r["disposed"], r["managed_controlled"], r["uncollected"])]},
                title="From generation to controlled facilities", show={"kind": "chart", "chart": "bar", "x": "stage", "y": "t a day", "how": "sum"}),
                api.table("waste_facilities", {"facility": [f["id"] for f in fr], "level of control": [f["level_name"] for f in fr],
                                               "controlled": ["yes" if f["controlled"] else "no" for f in fr], "received": [round(f["received"], 1) for f in fr],
                                               "residues in": [round(f["residues_in"], 1) for f in fr], "recovered": [round(f["recovered"], 1) for f in fr],
                                               "managed in control": [round(f["managed_controlled"], 1) for f in fr]}, title="Facilities and their control")]
            if r.get("composition"):
                c = r["composition"]
                out.append(api.table("waste_composition", {"fraction": [x["fraction"] for x in c], "share": [round(x["value"], 4) for x in c],
                                                           "lower": [round(x["lower"], 4) for x in c], "upper": [round(x["upper"], 4) for x in c]},
                                     title="Composition of household waste"))
            pp = r["per_person_by_stratum"]
            text = (f"Households generate {r['households']['value']:,.1f} t a day (95% interval {r['households']['lower']:,.1f} to {r['households']['upper']:,.1f}), "
                    f"{r['per_person']['value']:.2f} kg a person a day: " + ", ".join(f"{names.get(int(s), s)} {v:.2f}" for s, v in pp.items()) + ". "
                    f"With {r['other']:,.0f} t from other sources, {r['generated']:,.0f} t a day are generated; {r['collected']:,.0f} are collected "
                    f"({100 * r['collection_coverage']:.0f}%) and {r['managed_controlled']:,.0f} are managed in controlled facilities. "
                    f"SDG 11.6.1: {100 * r['indicator']:.1f}% (interval {100 * r['indicator_lower']:.1f}% to {100 * r['indicator_upper']:.1f}%, from the survey alone). "
                    f"Recovery: {100 * r['recovery_rate']:.1f}% of what is generated. " + " ".join(x[0].upper() + x[1:] + "." for x in r["remarks"])
                    + " The amounts at facilities are taken as weighed; the interval carries only the household survey's error.")
            out.append(api.note(text))
            return out

    return Waste()
