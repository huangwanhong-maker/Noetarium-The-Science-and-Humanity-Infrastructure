"""Years lived in health: health expectancy by Sullivan's method, country by country, held against what a statistical office publishes."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class HealthyLife(api.BoardAnalysis):
        id = "healthy_life"
        title = "Years lived in health"
        description = ("Health expectancy by Sullivan's method: of the years a life table expects, those lived without a limitation of usual "
                       "activities, given the prevalence of the limitation by age. Computed for each place and sex from a life table and a "
                       "survey's prevalence, and held against the healthy life years a statistical office publishes where a table of them is given.")
        params = [api.param("table", "Life tables", "life_tables", kind="text", role="table", help="one row an age: survivors l and person-years L"),
                  api.param("place", "Place", "country", kind="text", role="column", of="table"),
                  api.param("sex", "Sex", "sex", kind="text", role="column", of="table"),
                  api.param("year", "Year", 2023, kind="integer", minimum=1990, maximum=2100),
                  api.param("prevalence", "Prevalence", "limitation", kind="text", role="table", help="one row an age band of a place, sex and year"),
                  api.param("measure", "Limitation", "some_or_severe", kind="choice", options=["some_or_severe", "severe"],
                            help="some or severe gives healthy life years; severe gives years free of severe limitation"),
                  api.param("below", "Below the first band", "none", kind="choice", options=["none", "half", "first"],
                            help="the prevalence assumed where the survey asks no one (children): none, half the first band's, or the first band's"),
                  api.param("published", "Published values", "healthy_life", kind="text", role="table", help="a table of published healthy life years to hold the answers against (optional)"),
                  api.param("sexes", "Sexes", "F, M", kind="text", help="the sexes to compute, by their codes")]

        def run(self, board, params, context):
            lt, pv = board.table(params["table"]), board.table(params["prevalence"])
            year = str(int(params["year"]))
            place, sex = s.texts(lt, params["place"]), s.texts(lt, params["sex"])
            years, ages, l, L = s.texts(lt, "year"), s.nums(lt, "age"), s.nums(lt, "l"), s.nums(lt, "L")
            tables = s.grouped(list(zip(place, sex, years)), ages, l, L)
            pp, ps, py, pf, pm = s.texts(pv, "country"), s.texts(pv, "sex"), s.texts(pv, "year"), s.nums(pv, "from"), s.nums(pv, params["measure"])
            prev = s.grouped(list(zip(pp, ps, py)), pf, pm)
            published = {}
            if params.get("published"):
                try:
                    hl = board.table(params["published"])
                    for g, sx, y, h0, h65 in zip(s.texts(hl, "country"), s.texts(hl, "sex"), s.texts(hl, "year"), s.nums(hl, "hly0"), s.nums(hl, "hly65")):
                        published[(g, sx, y)] = (h0, h65)
                except (KeyError, LookupError):
                    published = {}
            wanted = [x.strip() for x in str(params["sexes"]).split(",") if x.strip()]
            rows = {k: [] for k in ("place", "sex", "le0", "hle0", "le65", "hle65", "share0", "published_hly0", "published_hly65", "difference65")}
            done = 0
            for (g, sx, y), tab in sorted(tables.items()):
                if y != year or sx not in wanted or (g, sx, y) not in prev:
                    continue
                tab = sorted(t for t in tab if t[0] is not None and t[1] is not None and t[2] is not None)
                bands = sorted((a, p) for a, p in prev[(g, sx, y)] if a is not None and p is not None)
                if len(tab) < 20 or len(bands) < 4:
                    continue
                a = s.society.ask("sullivan", ages=[t[0] for t in tab], l=[t[1] for t in tab], L=[t[2] for t in tab],
                                  bands=[b[0] for b in bands], prevalence=[b[1] for b in bands], unit="percent", below=params["below"])
                ix65 = a["ages"].index(65.0) if 65.0 in a["ages"] else None
                pub = published.get((g, sx, y), (None, None))
                rows["place"].append(g), rows["sex"].append(sx)
                rows["le0"].append(round(a["le"][0], 2)), rows["hle0"].append(round(a["hle"][0], 2))
                rows["le65"].append(round(a["le"][ix65], 2) if ix65 is not None else None)
                rows["hle65"].append(round(a["hle"][ix65], 2) if ix65 is not None else None)
                rows["share0"].append(round(a["share"][0], 4))
                rows["published_hly0"].append(pub[0]), rows["published_hly65"].append(pub[1])
                rows["difference65"].append(round(a["hle"][ix65] - pub[1], 2) if ix65 is not None and pub[1] is not None else None)
                done += 1
                context.progress(min(0.95, done / 70), f"{g} {sx}")
            if not done:
                raise ValueError(f"no place has both a life table and a prevalence for {year}")
            diffs = [abs(d) for d in rows["difference65"] if d is not None]
            diffs.sort()
            med = diffs[len(diffs) // 2] if diffs else None
            best = max(range(done), key=lambda k: rows["hle0"][k])
            worst = min(range(done), key=lambda k: rows["hle0"][k])
            return [api.table("healthy_life_years", rows, title=f"Years lived in health, {year}, by Sullivan's method",
                              roles={"name": "place", "category": "sex", "amount": "hle0"}, show={"kind": "chart", "chart": "bar", "x": "place", "y": "hle0", "split": "sex", "how": "mean"}),
                    api.note(f"{done} places and sexes in {year}. Healthy years at birth range from {rows['hle0'][worst]:.1f} ({rows['place'][worst]}, {rows['sex'][worst]}) "
                             f"to {rows['hle0'][best]:.1f} ({rows['place'][best]}, {rows['sex'][best]}) of lives of {rows['le0'][worst]:.1f} and {rows['le0'][best]:.1f} years."
                             + (f" Held against the published healthy life years at 65, the method agrees to a median of {med:.2f} years" if med is not None else "")
                             + ("; the prevalence comes in bands of ten years, the published values from finer ones." if med is not None else "")),
                    api.note(f"Presumed, not observed: below the survey's first band the prevalence is taken as {params['below']}; at birth the answer moves "
                             f"by about a year between the assumptions, at 65 not at all. Sullivan's method describes the health of the present population, "
                             f"not the life course of a cohort.")]
    return HealthyLife()
