"""Economies in a year: where the buyers and their incomes are, from the data library."""

import numpy as np


def create(api):
    class Economies(api.BoardAnalysis):
        id = "economies"
        title = "Economies in a year"
        description = ("Every economy with its people, its income per person and the share of its people in towns, in one year: the World Bank's World "
                       "Development Indicators, through the data library's table of economies. Regions and income groups, which are sums of economies, are left out.")
        params = [api.param("year", "Year", 2023, kind="integer", minimum=2000, maximum=2024),
                  api.param("name", "Name its table", "economies", kind="text")]

        def run(self, board, params, context):
            t = api.library("economies")
            year, people, income = t.column("year"), t.column("population"), t.column("gdp_per_capita")
            region = t.text("region")
            keep = (year == params["year"]) & np.isfinite(people) & np.isfinite(income) & np.array([r not in ("", "Aggregates") for r in region])
            rows = np.flatnonzero(keep)
            if not rows.size:
                return [api.note(f"The data library has no economy with both people and income in {params['year']}.")]
            take = lambda values: [values[i] for i in rows]
            urban = t.column("urban_share")[rows]
            columns = {
                "iso3": take(t.text("iso3")), "name": take(t.text("name")), "region": take(region), "income_group": take(t.text("income")),
                "population": people[rows], "gdp_per_capita": income[rows], "urban_share": urban, "urban_population": people[rows] * urban / 100.0,
                "gdp_growth": t.column("gdp_growth")[rows],
            }
            labels = {"population": "people", "gdp_per_capita": "GDP per person, current US$", "urban_share": "people in towns, % of all",
                      "urban_population": "people in towns", "gdp_growth": "growth of GDP, % a year", "income_group": "World Bank income group"}
            return [api.table(params["name"] or "economies", columns, title=f"Economies in {params['year']}", source=t.source, labels=labels),
                    api.note(f"{rows.size} economies in {params['year']}, {people[rows].sum() / 1e9:.2f} billion people among them.")]

    return Economies()
