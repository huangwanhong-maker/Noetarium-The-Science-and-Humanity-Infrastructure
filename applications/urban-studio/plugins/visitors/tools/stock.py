"""Visitors, how many and when: the stock of visitors present in each place month by month and by origin, under declared readings of
"visitor", with intensity and seasonality against declared bands (ADR 0156)."""

from science import visitors

MONTHS = ["Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"]


def create(api):
    class Stock(api.BoardAnalysis):
        id = "stock"
        title = "Visitors present, month by month (the visitor stock)"
        description = ("How many visitors are present on an average day of each month, from the nights they spend: a night is a visitor present "
                       "for a day. What counts as a visitor is read every way declared: registered establishments only (hotels, holiday "
                       "accommodation, camping); platform rentals too (Eurostat's short-stay rentals booked through four platforms); stays with "
                       "friends and relatives or in second homes, which no return holds, at a declared rate per registered night; and same-day "
                       "visitors at a declared rate, present for declared hours. Each reading gives its own stock. Intensity (visitors per "
                       "thousand residents, nights per resident, per km²) and seasonality (the peak month over the mean, the Gini coefficient of "
                       "the stock over the year's days) are judged against bands where declared. A place whose platform rentals are published "
                       "only for its region takes them in proportion to its registered nights, a declared key.")
        params = [api.param("case", "Case of the data library", "european-cities", kind="text", help="a case of visitor_stock.py: european-cities"),
                  api.param("year", "Year", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("unregistered", "Unregistered nights per registered night (declared)", 0.25, minimum=0, maximum=3, step=0.05),
                  api.param("same_day", "Same-day visits per overnight night (declared)", 0.5, minimum=0, maximum=10, step=0.1),
                  api.param("same_day_hours", "Hours a same-day visitor is present (declared)", 6, minimum=0, maximum=24, step=0.5),
                  api.param("bands", "Bands (declared standards)", "", kind="text", help="e.g. nights_per_resident<=20; peak_to_mean<=1.5")]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
            R = visitors.readings(g("unregistered", 0.25), g("same_day", 0.5), g("same_day_hours", 6))
            B = visitors.bands(params.get("bands") or "")
            a = visitors.ask("visitors.stock", places=[visitors.place(p, P["year"]) for p in P["places"]], readings=R, bands=B)
            A = a["places"]
            main = 1  # with platform rentals: what the returns themselves hold
            people = {p["id"]: p.get("population") for p in P["places"]}
            short = {p["id"]: p.get("short") or p["name"] for p in P["places"]}
            rows = {"place": [], "month": [], "visitors_per_thousand_residents": [], "visitors_present": []}
            for p in A:
                r = p["readings"][main]
                for m, M in enumerate(r["months"]):
                    rows["place"].append(short[p["id"]]), rows["month"].append(m + 1)
                    rows["visitors_present"].append(round(M["stock"]))
                    rows["visitors_per_thousand_residents"].append(round(1000 * M["stock"] / people[p["id"]], 2) if people.get(p["id"]) else None)
            out = [api.table("visitors_months", rows, title=f"Visitors present on an average day, per thousand residents, {P['year']} ({R[main]['name']})",
                             show={"kind": "chart", "chart": "line", "x": "month", "y": "visitors_per_thousand_residents", "split": "place", "how": "sum"})]
            pct = lambda v, k=4: None if v is None else round(v, k)  # noqa: E731
            r1 = [p["readings"][main] for p in A]
            cols = {"place": [p["name"] for p in A], "mean_present": [round(r["mean"]) for r in r1], "peak_present": [round(r["peak"]) for r in r1],
                    "peak_month": [MONTHS[int(r["peak_month"]) - 1] for r in r1], "per_thousand_mean": [pct(r["per_thousand_mean"], 1) for r in r1],
                    "per_thousand_peak": [pct(r["per_thousand_peak"], 1) for r in r1], "nights_per_resident": [pct(r["nights_per_resident"], 2) for r in r1],
                    "per_km2_mean": [pct(r["per_km2_mean"], 1) for r in r1], "peak_to_mean": [pct(r["peak_to_mean"], 3) for r in r1], "gini": [pct(r["gini"]) for r in r1],
                    "foreign_share": [pct(r["foreign_share"]) for r in r1], "platform_share": [pct(r["platform_share"]) for r in r1],
                    "mean_stay_registered": [pct(p["mean_stay"], 2) for p in A], "complete": ["yes" if r["complete"] else "no" for r in r1]}
            for b in B:
                cols[f"verdict_{b['measure']}"] = [r["verdicts"].get(b["measure"]) for r in r1]
            out.append(api.table("visitors_measures", cols, title=f"Intensity and seasonality, {P['year']} ({R[main]['name']})",
                                 show={"kind": "chart", "chart": "bar", "x": "place", "y": "nights_per_resident", "how": "sum"}))
            words_r = ["registered", "+ platforms", "+ unregistered", "+ same-day"]
            lr = {"place": [], "reading": [], "mean_present_per_thousand": [], "in full": []}
            for p in A:
                for k, r in enumerate(p["readings"]):
                    lr["place"].append(short[p["id"]]), lr["reading"].append(words_r[k]), lr["in full"].append(r["reading"])
                    lr["mean_present_per_thousand"].append(pct(r["per_thousand_mean"], 1))
            out.append(api.table("visitors_readings", lr, title="Visitors present per thousand residents under each reading of \"visitor\"",
                                 show={"kind": "chart", "chart": "bar", "x": "place", "y": "mean_present_per_thousand", "split": "reading", "how": "sum"}))
            lo = {"place": [], "origin": [], "mean_present": []}
            for p, r in zip(A, r1):
                days = [M["days"] for M in r["months"]]
                for origin, word in (("DOM", "domestic"), ("FOR", "foreign")):
                    v = sum(M["by_origin"].get(origin, 0) * d for M, d in zip(r["months"], days)) / sum(days)
                    lo["place"].append(short[p["id"]]), lo["origin"].append(word), lo["mean_present"].append(round(v))
            out.append(api.table("visitors_origin", lo, title=f"Visitors present on an average day by origin ({R[main]['name']})",
                                 show={"kind": "chart", "chart": "bar", "x": "place", "y": "mean_present", "split": "origin", "how": "sum"}))
            top = max(zip(A, r1), key=lambda x: x[1]["per_thousand_peak"] or 0)
            sea = max(zip(A, r1), key=lambda x: x[1]["gini"] or 0)
            flat = min(zip(A, r1), key=lambda x: x[1]["gini"] if x[1]["gini"] is not None else 9)
            words = [f"Counting registered establishments and platform rentals, {top[0]['name']} has the most visitors present for its residents: "
                     f"{top[1]['per_thousand_peak']:.0f} per thousand in its peak month ({MONTHS[int(top[1]['peak_month']) - 1]}), {top[1]['nights_per_resident']:.1f} nights a resident in the year.",
                     f"Seasonality is strongest in {sea[0]['name']} (Gini {sea[1]['gini']:.3f}, the peak {sea[1]['peak_to_mean']:.2f} times the mean) and weakest in {flat[0]['name']} (Gini {flat[1]['gini']:.3f})."]
            alloc = [p["name"] for p in A if p.get("allocated")]
            if alloc:
                words.append(f"{', '.join(alloc)} took its region's platform rentals in proportion to its registered nights, a declaration.")
            inc = [p["name"] for p, r in zip(A, r1) if not r["complete"]]
            if inc:
                words.append(f"Months withheld by Eurostat in: {', '.join(inc)}; the year is incomplete there.")
            words.append(f"Unregistered stays and same-day visitors enter only at the declared rates ({g('unregistered', 0.25):g} a registered night; "
                         f"{g('same_day', 0.5):g} a night for {g('same_day_hours', 6):g} hours), which no statistic here supports; none of the readings is the stock until one is chosen. {P['credit']}.")
            out.append(api.note(" ".join(words)))
            return out

    return Stock()
