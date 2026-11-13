"""How a day is divided: each place's women's and men's days as compositions of 1440 minutes, compared in the simplex's own geometry."""

import math
import statistics
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402

PARTS = ("personal care and sleep, paid work, study, household and family care, voluntary work and help, social life, culture and rest, "
         "sport, hobbies and media, travel")
SEXES = {"F": "women", "M": "men"}


def split_parts(text):
    """Part names separated by commas; a name that itself holds a comma ("social life, culture and rest") is kept whole when the table has it."""
    return [p.strip() for p in str(text).split(",") if p.strip()]


def create(api):
    class TheDay(api.BoardAnalysis):
        id = "the_day"
        title = "How a day is divided"
        description = ("Each place's day, for women and for men, as a composition of 1440 minutes: more of one activity is less of the others, so "
                       "days are compared by their ratios (Aitchison's geometry) rather than minute by minute. Places are set out by how their days "
                       "differ, the parts that set them apart are named, the balance of unpaid against paid work is read for each, and the distance "
                       "between women's and men's days is measured place by place.")
        params = [api.param("table", "Days", "days", kind="text", role="table", help="one row a place, wave, sex and age group, with minutes by part of the day"),
                  api.param("parts", "Parts of the day", PARTS, kind="text", role="columns", of="table",
                            help="the columns of minutes, together making the day"),
                  s.col(api, "place", "Place", "name"),
                  s.col(api, "sex", "Sex", "sex", help="F and M"),
                  s.col(api, "wave_column", "Wave column", "wave"),
                  api.param("wave", "Wave", "2010", kind="text"),
                  s.col(api, "age_column", "Age column", "age"),
                  api.param("age", "Age group", "TOTAL", kind="text"),
                  api.param("unpaid", "Unpaid work", "household and family care", kind="text", role="column", of="table"),
                  api.param("paid", "Paid work", "paid work", kind="text", role="column", of="table")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            names = self.parts(t, params["parts"])
            if len(names) < 2:
                raise ValueError("a day needs at least two parts")
            for n in (params["unpaid"], params["paid"]):
                if n not in names:
                    raise ValueError(f"the part '{n}' is not among the parts of the day")
            cols = [s.nums(t, n) for n in names]
            days = {}
            for i, (pl, sx, w, ag) in enumerate(zip(s.texts(t, params["place"]), s.texts(t, params["sex"]), s.texts(t, params["wave_column"]), s.texts(t, params["age_column"]))):
                if w != str(params["wave"]) or ag != str(params["age"]) or sx not in SEXES:
                    continue
                row = [c[i] for c in cols]
                if any(v is None for v in row):
                    continue
                days[(pl, sx)] = row
            places = sorted({pl for pl, _ in days if (pl, "F") in days and (pl, "M") in days})
            if len(places) < 2:
                raise ValueError(f"fewer than two places have women's and men's days in the wave {params['wave']}, age {params['age']}")
            groups, index = [], {}
            for pl in places:
                for sx in ("F", "M"):
                    index[(pl, sx)] = len(groups)
                    groups.append({"name": f"{pl}, {SEXES[sx]}", "days": [days[(pl, sx)]]})
            u, p = names.index(params["unpaid"]), names.index(params["paid"])
            a = s.society.ask("day", parts=names, groups=groups, balance={"plus": [u], "minus": [p]},
                              contrast=[[index[(pl, "F")], index[(pl, "M")]] for pl in places])
            coords = a["compared"]["coordinates"]
            explained = a["compared"]["explained"]
            rows = {"day": [], "place": [], "sex": [], "coordinate_1": [], "coordinate_2": [], "unpaid_against_paid": []}
            for n in names:
                rows[n] = []
            for pl in places:
                for sx in ("F", "M"):
                    g = index[(pl, sx)]
                    rows["day"].append(groups[g]["name"]), rows["place"].append(pl), rows["sex"].append(SEXES[sx])
                    c = coords[g] + [None, None]
                    rows["coordinate_1"].append(round(c[0], 4) if c[0] is not None else None)
                    rows["coordinate_2"].append(round(c[1], 4) if c[1] is not None else None)
                    rows["unpaid_against_paid"].append(round(a["groups"][g]["balance"], 4))
                    for k, n in enumerate(names):
                        rows[n].append(round(a["groups"][g]["centre"][k], 1))
            between = {"place": [], "distance": [], "most_apart": [], "women_unpaid": [], "men_unpaid": [], "women_paid": [], "men_paid": []}
            for pl, c in zip(places, a["contrasts"]):
                k = max(range(len(names)), key=lambda j: abs(c["clr"][j]))
                between["place"].append(pl)
                between["distance"].append(round(c["distance"], 4))
                between["most_apart"].append(names[k])
                f, m = days[(pl, "F")], days[(pl, "M")]
                between["women_unpaid"].append(f[u]), between["men_unpaid"].append(m[u])
                between["women_paid"].append(f[p]), between["men_paid"].append(m[p])
            spread = a["compared"]["clr_variance"]
            order = sorted(range(len(names)), key=lambda j: -spread[j])
            axis = self.axis(rows, names)
            far = max(range(len(places)), key=lambda i: between["distance"][i])
            near = min(range(len(places)), key=lambda i: between["distance"][i])
            apart = statistics.mode(between["most_apart"])
            count = between["most_apart"].count(apart)
            women_side = statistics.fmean(v for v, x in zip(rows["coordinate_1"], rows["sex"]) if x == "women")
            men_side = statistics.fmean(v for v, x in zip(rows["coordinate_1"], rows["sex"]) if x == "men")
            split = sum(1 for pl in places if (rows["coordinate_1"][index[(pl, "F")]] - rows["coordinate_1"][index[(pl, "M")]]) * (women_side - men_side) > 0)
            fb = [b for b, x in zip(rows["unpaid_against_paid"], rows["sex"]) if x == "women"]
            mb = [b for b, x in zip(rows["unpaid_against_paid"], rows["sex"]) if x == "men"]
            return [api.table("the_day", rows, title=f"Days of women and men, {params['wave']}, set out by how they differ",
                              roles={"name": "day"}, show={"kind": "chart", "chart": "scatter", "x": "coordinate_1", "y": "coordinate_2", "split": "sex"}),
                    api.table("between_sexes", between, title="How far apart women's and men's days are, place by place (Aitchison's distance)",
                              roles={"name": "place", "amount": "distance"}, show={"kind": "chart", "chart": "bar", "x": "place", "y": "distance", "how": "sum"}),
                    api.note(f"Across {len(places)} places and both sexes, the parts of the day that differ most, as ratios, are {names[order[0]]} and "
                             f"{names[order[1]]}; {names[order[-1]]} differs least. The first axis of the figure holds {s.share(explained[0])} of the "
                             f"differences{axis}; women's and men's days of the same place lie on opposite sides of it in {split} of {len(places)} places."),
                    api.note(f"Women's and men's days are furthest apart in {places[far]} (a distance of {between['distance'][far]:.2f}) and nearest in "
                             f"{places[near]} ({between['distance'][near]:.2f}). The part that separates them most is {apart}, in {count} of {len(places)} "
                             f"places. Unpaid against paid work, women's balance is above zero in {sum(1 for b in fb if b > 0)} places and men's in "
                             f"{sum(1 for b in mb if b > 0)}: zero is the two held in the same proportion."),
                    api.note("Counted, then compared: the days are the survey's means over everyone, those who did none of an activity included, so a "
                             "place's day is a composition of averages, not the centre of people's diaries, which are not open. Minutes the diaries did "
                             "not assign are spread over the parts in proportion. Distances and the axes are Aitchison's, so a part's change counts by "
                             "its ratio: an hour moved between short parts weighs more than an hour moved between long ones.")]

        @staticmethod
        def parts(t, text):
            """The parts named, read against the table's columns so that a name holding a comma stays whole."""
            pieces, out, k = split_parts(text), [], 0
            while k < len(pieces):
                for j in range(len(pieces), k, -1):
                    name = ", ".join(pieces[k:j])
                    if t.has(name):
                        out.append(name)
                        k = j
                        break
                else:
                    raise LookupError(f"the table '{t.name}' has no column '{pieces[k]}'")
            return out

        @staticmethod
        def axis(rows, names):
            """Which part's ratio the first axis follows most closely, in words (empty when none stands out)."""
            x = rows["coordinate_1"]
            best, name = 0.0, None
            for n in names:
                v = [math.log(max(m, 0.5)) for m in rows[n]]
                if len(set(v)) < 2:
                    continue
                r = statistics.correlation(x, v)
                if abs(r) > abs(best):
                    best, name = r, n
            return f", and follows {name} most closely (r = {best:+.2f} with its logarithm)" if name and abs(best) >= 0.5 else ""
    return TheDay()
