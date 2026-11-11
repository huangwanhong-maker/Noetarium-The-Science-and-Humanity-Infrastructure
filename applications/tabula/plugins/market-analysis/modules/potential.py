"""The size of a market in every economy, from two assumptions written on the board."""

import numpy as np


def create(api):
    class Potential(api.BoardAnalysis):
        id = "potential"
        title = "What the market could be worth, by economy"
        description = ("For each economy: its buyers (its people, or its people in towns, times the share of them who would buy) and what they would spend "
                       "(the buyers times income per person times the share of income spent on the offer). The two shares are assumptions read from the "
                       "board's notes with their ranges, and the answer carries the range. Income per person is GDP per person, which is income and not spending.")
        params = [api.param("table", "Economies", "economies", kind="text", role="table"),
                  api.param("adoption", "Share who would buy", "adoption", kind="text", role="quantity"),
                  api.param("wallet", "Share of income spent on it", "wallet", kind="text", role="quantity"),
                  api.param("towns", "Only people in towns", True, kind="boolean"),
                  api.param("name", "Name its table", "potential", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            adoption, wallet = board.quantity(params["adoption"]), board.quantity(params["wallet"])
            people = t.column("urban_population") if params["towns"] and t.has("urban_population") else t.column("population")
            income = t.column("gdp_per_capita")
            keep = np.flatnonzero(np.isfinite(people) & np.isfinite(income))
            people, income = people[keep], income[keep]
            # Both shares are positive, so the bounds of the product are the products of the bounds.
            value = {k: people * getattr(adoption, k) * income * getattr(wallet, k) for k in ("lo", "mid", "hi")}
            order = np.argsort(-value["mid"])
            total = value["mid"].sum()
            take = lambda values: [values[keep[i]] for i in order]
            columns = {
                "iso3": take(t.text("iso3")), "name": take(t.text("name")), "region": take(t.text("region")) if t.has("region") else [""] * order.size,
                "buyers": (people * adoption.mid)[order], "value": value["mid"][order], "value_low": value["lo"][order], "value_high": value["hi"][order],
                "share": (100.0 * value["mid"] / total)[order] if total > 0 else np.zeros(order.size),
                "rank": np.arange(1, order.size + 1),
            }
            labels = {"buyers": "people who would buy", "value": "what they would spend, US$ a year", "value_low": "at the low ends of both assumptions",
                      "value_high": "at their high ends", "share": "% of all economies' value"}
            first = ", ".join(columns["name"][:5])
            who = "people in towns" if params["towns"] and t.has("urban_population") else "people"
            return [api.table(params["name"] or "potential", columns, title="What the market could be worth, by economy", source=t.source, labels=labels),
                    api.note(f"Of {who}, {adoption.mid:.2%} would buy ({adoption.lo:.2%} to {adoption.hi:.2%}) and spend {wallet.mid:.2%} of income "
                             f"({wallet.lo:.2%} to {wallet.hi:.2%}): US$ {total / 1e9:,.1f} billion a year over {order.size} economies "
                             f"(US$ {value['lo'].sum() / 1e9:,.1f} to {value['hi'].sum() / 1e9:,.1f} billion). The first five: {first}.")]

    return Potential()
