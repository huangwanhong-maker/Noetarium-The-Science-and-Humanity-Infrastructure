"""Care ahead: the people who will need care as a population ages, the hours they will need, and what made the need change."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class CareAhead(api.BoardAnalysis):
        id = "care_ahead"
        title = "Care ahead"
        description = ("People needing care now and in a year ahead, place by place: a population by age times the prevalence of a severe "
                       "limitation of usual activities (as the measure of need it is declared to be), with the hours a week each needs; support "
                       "ratios; and the change in need split exactly among the population's size, its age structure and the prevalence, which "
                       "a scenario of compression or expansion may move.")
        params = [api.param("population", "Population now", "population", kind="text", role="table", help="persons by place, year and single age"),
                  api.param("now", "Year now", 2024, kind="integer", minimum=1990, maximum=2100),
                  api.param("projections", "Projections", "projections", kind="text", role="table", help="projected persons by place, year and single age"),
                  api.param("later", "Year ahead", 2050, kind="integer", minimum=2025, maximum=2100),
                  api.param("prevalence", "Prevalence", "limitation", kind="text", role="table"),
                  api.param("year", "Prevalence of the year", 2023, kind="integer", minimum=1990, maximum=2100),
                  api.param("measure", "Need", "severe", kind="choice", options=["severe", "some_or_severe"], help="the limitation taken as needing care"),
                  api.param("hours", "Hours a week each", 20, kind="number", minimum=0, maximum=168, help="the care one person in need is presumed to need"),
                  api.param("compression", "Annual change of prevalence", 0.0, kind="number", minimum=-0.05, maximum=0.05, step=0.001,
                            help="positive: prevalence falls each year (compression of morbidity); negative: it rises (expansion)"),
                  api.param("places", "Places", "", kind="text", help="codes of the places to compute; empty: all"),
                  api.param("groups", "Groups of places", "EU27_2020, EU28, EA19, EA20, EA21", kind="text",
                            help="codes that stand for groups of the other places: computed and reported apart, never added to their members")]

        def run(self, board, params, context):
            pop, proj, pv = board.table(params["population"]), board.table(params["projections"]), board.table(params["prevalence"])
            now, later, py = str(int(params["now"])), str(int(params["later"])), str(int(params["year"]))
            def by_place(t, year):
                out = {}
                for g, y, a, n in zip(s.texts(t, "country"), s.texts(t, "year"), s.nums(t, "age"), s.nums(t, "persons")):
                    if y == year and a is not None and n is not None:
                        out.setdefault(g, {})[int(a)] = n
                return out
            P0, P1 = by_place(pop, now), by_place(proj, later)
            prev = {}
            for g, sx, y, f, m in zip(s.texts(pv, "country"), s.texts(pv, "sex"), s.texts(pv, "year"), s.nums(pv, "from"), s.nums(pv, params["measure"])):
                if sx == "T" and y == py and f is not None and m is not None:
                    prev.setdefault(g, []).append((f, m))
            wanted = [x.strip() for x in str(params["places"]).split(",") if x.strip()]
            rows = {k: [] for k in ("place", "need_now", "need_later", "hours_now", "hours_later", "change", "from_size", "from_ageing", "from_prevalence",
                                     "dependency_now", "dependency_later", "support_now", "support_later")}
            for g in sorted(set(P0) & set(P1) & set(prev)):
                if wanted and g not in wanted:
                    continue
                ages = sorted(set(P0[g]) & set(P1[g]))
                bands = sorted(prev[g])
                if len(ages) < 80 or len(bands) < 4:
                    continue
                a = s.society.ask("care", ages=ages, now=[P0[g][x] for x in ages], later=[P1[g][x] for x in ages], bands=[b[0] for b in bands],
                                  prevalence=[b[1] for b in bands], unit="percent", hours=float(params["hours"]), compression=float(params["compression"]),
                                  years=float(int(later) - int(now)))
                rows["place"].append(g)
                rows["need_now"].append(round(a["now"]["people"])), rows["need_later"].append(round(a["later"]["people"]))
                rows["hours_now"].append(round(a["now"]["hours_a_week"])), rows["hours_later"].append(round(a["later"]["hours_a_week"]))
                c = a["change"]
                rows["change"].append(round(c["total"])), rows["from_size"].append(round(c["size"])), rows["from_ageing"].append(round(c["structure"]))
                rows["from_prevalence"].append(round(c["prevalence"]))
                rows["dependency_now"].append(round(a["now"]["old_age_dependency"], 1)), rows["dependency_later"].append(round(a["later"]["old_age_dependency"], 1))
                rows["support_now"].append(round(a["now"]["oldest_old_support"], 2)), rows["support_later"].append(round(a["later"]["oldest_old_support"], 2))
            if not rows["place"]:
                raise ValueError("no place has a population now, a projection and a prevalence for the years asked")
            # A group (the EU, the euro area) is its members again: it is computed on its own figures and reported apart, never added in.
            groups = {x.strip() for x in str(params.get("groups") or "").split(",") if x.strip()}
            member = [k for k, g in enumerate(rows["place"]) if g not in groups]
            grouped = [k for k, g in enumerate(rows["place"]) if g in groups]
            if not member:
                raise ValueError("every place computed is a group of places: name the members, or clear the groups")
            pick = lambda ks: {c: [v[k] for k in ks] for c, v in rows.items()}  # noqa: E731
            places, together = pick(member), pick(grouped)
            n0, n1 = sum(places["need_now"]), sum(places["need_later"])
            ag, sz, pr = sum(places["from_ageing"]), sum(places["from_size"]), sum(places["from_prevalence"])
            top = max(range(len(places["place"])), key=lambda k: places["need_later"][k] / max(1, places["need_now"][k]))
            factors = {"place": [], "factor": [], "people": []}
            for k, g in enumerate(places["place"]):
                for name, col in (("size", "from_size"), ("ageing", "from_ageing"), ("prevalence", "from_prevalence")):
                    factors["place"].append(g), factors["factor"].append(name), factors["people"].append(places[col][k])
            out = [api.table("care_ahead", places, title=f"People needing care, {now} and {later}, place by place", roles={"name": "place", "amount": "change"}),
                   api.table("care_change", factors, title=f"What made the need for care change, {now} to {later}: size, ageing, prevalence",
                             roles={"name": "place", "category": "factor", "amount": "people"},
                             show={"kind": "chart", "chart": "stacked", "x": "place", "y": "people", "split": "factor", "how": "sum"})]
            if together["place"]:
                out.append(api.table("care_ahead_groups", together, title=f"People needing care, {now} and {later}, in groups of places (not added to their members)",
                                     roles={"name": "place", "amount": "change"}))
            text = (f"Across {len(places['place'])} places the people needing care go from {n0:,.0f} in {now} to {n1:,.0f} in {later}, a change of {n1 - n0:+,.0f}: "
                    f"ageing accounts for {ag:+,.0f}, the populations' size for {sz:+,.0f} and the prevalence for {pr:+,.0f}. The steepest rise is in "
                    f"{places['place'][top]} (×{places['need_later'][top] / max(1, places['need_now'][top]):.2f}). Each place's change is split exactly among its size, "
                    f"its age structure and its prevalence.")
            if together["place"]:
                k = max(range(len(together["place"])), key=lambda j: together["need_now"][j])  # the widest group, the EU where it is given
                text +=(f" On its own figures, {together['place'][k]} goes from {together['need_now'][k]:,.0f} to {together['need_later'][k]:,.0f}; groups of places "
                         f"are reported apart and never added to their members.")
            return out + [
                    api.note(text),
                    api.note(f"Presumed, not observed: need is the {params['measure'].replace('_', ' ')} limitation of usual activities in {py}, applied by age bands "
                             f"(from 16; none below), {params['hours']} hours a week each, and a prevalence changing by {params['compression']:+.3f} a year. The "
                             f"projection is the statistical office's baseline.")]
    return CareAhead()
