"""Unpaid work and who carries it: the minutes of household and family care, by sex, and what they are worth at a declared wage."""

import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _society as s  # noqa: E402


def create(api):
    class UnpaidWork(api.BoardAnalysis):
        id = "unpaid_work"
        title = "Unpaid work, and who carries it"
        description = ("The minutes a day women and men give to household and family care, place by place, the share of it women carry, and its "
                       "value for a year at a declared wage by the replacement cost of a generalist (UNECE's guide). The minutes are counted; "
                       "the value is presumed, and says at what wage.")
        params = [api.param("table", "Days", "days", kind="text", role="table", help="one row a place, wave, sex and age group, with minutes by part of the day"),
                  s.col(api, "place", "Place", "name"),
                  api.param("care", "Unpaid work", "household and family care", kind="text", role="column", of="table"),
                  api.param("paid", "Paid work", "paid work", kind="text", role="column", of="table"),
                  api.param("wave", "Wave", "2010", kind="text"),
                  api.param("age", "Age group", "TOTAL", kind="text"),
                  api.param("wage", "Wage an hour", 12.0, kind="number", minimum=0, help="the replacement cost of an hour of household work (declared)"),
                  api.param("currency", "Currency", "EUR", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            cells = {}
            for g, w, sx, ag, c, p in zip(s.texts(t, params.get("place") or "country"), s.texts(t, "wave"), s.texts(t, "sex"), s.texts(t, "age"), s.nums(t, params["care"]), s.nums(t, params["paid"])):
                if w == params["wave"] and ag == params["age"] and c is not None:
                    cells.setdefault(g, {})[sx] = (c, p)
            rows = {k: [] for k in ("place", "women_unpaid", "men_unpaid", "women_share", "women_paid", "men_paid", "women_total_work", "men_total_work", "value_woman_a_year", "value_man_a_year")}
            for g in sorted(cells):
                d = cells[g]
                if "F" not in d or "M" not in d:
                    continue
                vf = s.society.ask("unpaid", minutes={"unpaid work": d["F"][0]}, generalist=float(params["wage"]), own=float(params["wage"]))
                vm = s.society.ask("unpaid", minutes={"unpaid work": d["M"][0]}, generalist=float(params["wage"]), own=float(params["wage"]))
                rows["place"].append(g)
                rows["women_unpaid"].append(d["F"][0]), rows["men_unpaid"].append(d["M"][0])
                rows["women_share"].append(round(d["F"][0] / (d["F"][0] + d["M"][0]), 4))
                rows["women_paid"].append(d["F"][1]), rows["men_paid"].append(d["M"][1])
                rows["women_total_work"].append(d["F"][0] + (d["F"][1] or 0)), rows["men_total_work"].append(d["M"][0] + (d["M"][1] or 0))
                rows["value_woman_a_year"].append(round(vf["generalist"])), rows["value_man_a_year"].append(round(vm["generalist"]))
            if not rows["place"]:
                raise ValueError(f"no place has women's and men's days in the wave {params['wave']}")
            k = max(range(len(rows["place"])), key=lambda i: rows["women_share"][i])
            j = min(range(len(rows["place"])), key=lambda i: rows["women_share"][i])
            more = sum(1 for f, m in zip(rows["women_total_work"], rows["men_total_work"]) if f > m)
            return [api.table("unpaid_work", rows, title=f"Unpaid work by sex, {params['wave']}, and its value at {params['wage']} {params['currency']} an hour",
                              roles={"name": "place", "amount": "women_share"}, show={"kind": "chart", "chart": "bar", "x": "place", "y": "women_share", "how": "sum"}),
                    api.note(f"In every one of {len(rows['place'])} places women carry most of the household and family care: from {s.share(rows['women_share'][j])} "
                             f"({rows['place'][j]}) to {s.share(rows['women_share'][k])} ({rows['place'][k]}). Counting paid and unpaid work together, women work "
                             f"longer days than men in {more} of them. At {params['wage']} {params['currency']} an hour, a woman's unpaid work in {rows['place'][k]} "
                             f"would cost {rows['value_woman_a_year'][k]:,.0f} {params['currency']} a year to replace."),
                    api.note("Presumed, not observed: the value is the replacement cost of a generalist at the declared wage; a specialist's wages or a person's "
                             "own would give other values, which answer other questions. The minutes are the survey's means over everyone, those who do none included.")]
    return UnpaidWork()
