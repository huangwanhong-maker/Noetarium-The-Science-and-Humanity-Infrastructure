"""Homes and who lacks them: SDG 11.1.1 from a household survey, the people in slum or informal households and in inadequate housing
under every declared reading of its open terms, with the survey's intervals and bounds over missing answers (ADR 0155)."""

from science import shelter

SHORT = ["four", "five", "JMP's ladder", "two a room", "walls, roof", "hazard counts"]


def create(api):
    class Shelter(api.BoardAnalysis):
        id = "shelter"
        title = "Homes and who lacks them (SDG 11.1.1)"
        description = ("The share of people living in slum or informal households, which lack one or more of improved water, improved sanitation, "
                       "sufficient living area, a durable dwelling and secure tenure, and in inadequate housing, whose cost exceeds a share of "
                       "income (30% in UN-Habitat's metadata), from a household survey with its weights, strata and clusters. Each open term is "
                       "read every way declared: tenure left out or secure with a document and no fear of eviction; the JMP's ladder, where a "
                       "shared toilet is not improved; two people to a room instead of three; walls and roof without the floor; a hazardous site "
                       "counted. Missing answers bound each share between counting them as not deprived and as deprived. The survey's columns: "
                       "weight, stratum, cluster, district, persons, rooms, water, sanitation, shared_toilet, walls, roof, floor, hazardous_site, "
                       "documented, fears_eviction, housing_cost, income.")
        params = [api.param("households", "Households surveyed", "households", kind="text", role="table", help="a table of the survey, one household a row"),
                  api.param("afford", "Inadequate when housing costs more than (share of income)", 0.30, minimum=0.05, maximum=0.9, step=0.05),
                  api.param("unknowns", "A missing answer is", "left out", kind="choice", options=["left out", "not deprived", "deprived"]),
                  api.param("confidence", "Confidence of the intervals", 0.95, minimum=0.5, maximum=0.999, step=0.01)]

        def run(self, board, params, context):
            t = board.table(str(params.get("households") or "households"))
            rows = [{c: t.raw(c)[i] for c in t.columns} for i in range(t.rows)]
            afford = float(params.get("afford") or 0.30)
            unknowns = str(params.get("unknowns") or "left out")
            R = shelter.readings(afford, unknowns)
            a = shelter.ask("shelter.assess", households=shelter.households(rows), readings=R, confidence=float(params.get("confidence") or 0.95))
            A = a["readings"]
            pct = lambda v: None if v is None else round(v, 4)  # noqa: E731
            out = [api.table("shelter_readings", {
                "reading": SHORT[:len(A)], "in full": [x["reading"] for x in A], "slum_share": [pct(x["slum"]["value"]) for x in A],
                "slum_ci_lower": [pct(x["slum"]["ci_lower"]) for x in A], "slum_ci_upper": [pct(x["slum"]["ci_upper"]) for x in A],
                "slum_if_unknowns_not_deprived": [pct(x["slum"]["lower"]) for x in A], "slum_if_unknowns_deprived": [pct(x["slum"]["upper"]) for x in A],
                "inadequate_share": [pct(x["inadequate"]["value"]) for x in A], "either_share": [pct(x["either"]["value"]) for x in A],
                "source": [x["source"] for x in A]},
                title="People in slum or informal households under each reading of SDG 11.1.1's open terms",
                show={"kind": "chart", "chart": "bar", "x": "reading", "y": "slum_share", "how": "sum"})]
            m = A[1]
            comp = [c["component"] for c in m["components"]]
            out.append(api.table("shelter_components", {
                "component": comp + comp, "measure": ["deprived"] * 5 + ["deprived of it alone"] * 5,
                "share_of_people": [pct(c["value"]) for c in m["components"]] + [pct(v) for v in m["only"]]},
                title=f"What households lack, under the reading '{m['reading']}': each component, and it alone",
                show={"kind": "chart", "chart": "bar", "x": "component", "y": "share_of_people", "split": "measure", "how": "sum"}))
            groups = sorted(m["slum_by_group"])
            out.append(api.table("shelter_districts", {
                "district": groups, "slum_share": [pct(m["slum_by_group"][g]["value"]) for g in groups],
                "ci_lower": [pct(m["slum_by_group"][g]["ci_lower"]) for g in groups], "ci_upper": [pct(m["slum_by_group"][g]["ci_upper"]) for g in groups],
                "if_unknowns_not_deprived": [pct(m["slum_by_group"][g]["lower"]) for g in groups], "if_unknowns_deprived": [pct(m["slum_by_group"][g]["upper"]) for g in groups]},
                title=f"People in slum or informal households by district, under '{m['reading']}'",
                show={"kind": "chart", "chart": "bar", "x": "district", "y": "slum_share", "how": "sum"}))
            out.append(api.table("shelter_deprivations", {
                "deprivations": [str(k) for k in range(6)], "share_of_people": [pct(v) for v in m["deprivations"]]},
                title=f"People by the number of things their household lacks, under '{m['reading']}' (households whose every answer is known)",
                show={"kind": "chart", "chart": "bar", "x": "deprivations", "y": "share_of_people", "how": "sum"}))
            lo, hi = min(A, key=lambda x: x["slum"]["value"]), max(A, key=lambda x: x["slum"]["value"])
            top = max(range(5), key=lambda c: m["components"][c]["value"] or 0)
            out.append(api.note(
                f"Of {round(m['people']):,} people in {round(m['households']):,} households, between {lo['slum']['value']:.1%} ({lo['reading']}) and "
                f"{hi['slum']['value']:.1%} ({hi['reading']}) live in slum or informal households. Under the metadata's five components, "
                f"{m['slum']['value']:.1%} ({m['slum']['ci_lower']:.1%} to {m['slum']['ci_upper']:.1%}); counting every missing answer as not deprived "
                f"and as deprived, between {m['slum']['lower']:.1%} and {m['slum']['upper']:.1%}. What is lacked most is {comp[top]} ({m['components'][top]['value']:.1%} "
                f"of people). {m['inadequate']['value']:.1%} live in housing that costs more than {afford:.0%} of their income, and {m['either']['value']:.1%} "
                f"in one or the other. None of these is the indicator's value until a reading is chosen."))
            return out

    return Shelter()
