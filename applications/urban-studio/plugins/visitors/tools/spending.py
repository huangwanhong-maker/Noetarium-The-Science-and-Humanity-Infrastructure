"""What visitors' spending becomes in an economy: a place's nights by origin times declared spending, carried through the country's
input–output table to direct and total value added, imported inputs, product taxes and leakage (ADR 0157)."""

from science import visitors

# Where a country's table leaves a product empty, the product that records it (retail with wholesale; imputed rents with real estate).
FALLBACK = {"G47": "G46", "L68A": "L68B", "R93": "R90-92"}

NAMES = {"I": "lodging and food", "G47": "retail", "G46": "wholesale", "H49": "land transport", "H52": "warehousing", "R90-92": "culture", "R93": "recreation",
         "N79": "travel agencies", "L68B": "real estate", "A01": "agriculture", "D": "energy", "K64": "finance", "C10-12": "food industry", "F": "construction",
         "M69_70": "law, management", "N77": "rental, leasing", "J61": "telecoms", "C19": "fuels", "E36": "water", "B": "mining"}


def short(code: str, label: str) -> str:
    """A product's name short enough for a chart's axis: a declared one, else the first words of its label."""
    if code in NAMES:
        return NAMES[code]
    words = label.split(",")[0].split()
    out = ""
    for w in words:
        if len(out) + len(w) + 1 > 18:
            break
        out = (out + " " + w).strip()
    return out.lower() or code


def create(api):
    class Spending(api.BoardAnalysis):
        id = "spending"
        title = "What visitors' spending becomes (value added, imports, leakage)"
        description = ("A place's visitor nights by origin (registered establishments and platform rentals, from the visitor stock) times a "
                       "declared spending a night, split over products by a declared basket and carried to basic prices by a declared share, "
                       "then through the country's input–output table: direct value added (what visitors buy, the tourism satellite account's "
                       "measure), indirect value added along the supply chains, imported inputs, taxes on products, and leakage. Every euro of "
                       "domestic demand ends as value added, imported inputs or taxes (Proposition 1). The table is national, so a city's own "
                       "leakage, larger than its country's, is not seen.")
        params = [api.param("case", "Case of the data library", "european-cities", kind="text"),
                  api.param("year", "Year of the nights", 2024, minimum=2020, maximum=2030, step=1),
                  api.param("place", "Place", "prague", kind="text", help="a place of the case: prague, brussels, …"),
                  api.param("geo", "Country of the input–output table", "CZ", kind="text"),
                  api.param("table_year", "Year of the table", 2022, minimum=2010, maximum=2030, step=1),
                  api.param("foreign", "A foreign visitor's spending a night (EUR, declared)", 130, minimum=0, maximum=2000, step=5),
                  api.param("domestic", "A domestic visitor's spending a night (EUR, declared)", 80, minimum=0, maximum=2000, step=5),
                  api.param("basket", "Basket (product = share)", "I=0.55; G47=0.20; H49=0.08; R90-92=0.07; R93=0.05; N79=0.05", kind="text"),
                  api.param("basic_share", "Basic over purchasers' prices (declared)", 0.85, minimum=0.5, maximum=1, step=0.01)]

        def run(self, board, params, context):
            g = lambda k, d: float(params.get(k) if params.get(k) not in (None, "") else d)  # noqa: E731
            P = visitors.product(str(params.get("case") or "european-cities"), int(g("year", 2024)))
            pid = str(params.get("place") or "prague").strip().lower()
            place = next((p for p in P["places"] if p["id"] == pid), None)
            if place is None:
                raise LookupError(f"no place {pid} in the case: {', '.join(p['id'] for p in P['places'])}")
            nights = {o: sum(v or 0 for v in place["registered"][o]) + (sum(v or 0 for v in place["platform"][o]) if not place.get("platform_region") else 0) for o in ("DOM", "FOR")}
            if place.get("platform_region"):
                a = visitors.ask("visitors.stock", places=[visitors.place(place, P["year"])], readings=visitors.readings()[1:2])
                carried = a["places"][0]["allocated"]["nights"]
                for o in ("DOM", "FOR"):
                    nights[o] += sum(v or 0 for v in carried[o])
            T = visitors.table(str(params.get("geo") or "CZ"), int(g("table_year", 2022)))
            B = visitors.basket(params.get("basket") or "")
            held = {c for c, row, f in zip(T["products"], T["domestic"], T["final_domestic"]) if sum(row) + f > 0}
            moved = []
            for k in list(B):
                if k not in held:
                    to = FALLBACK.get(k)
                    if not to or to not in held:
                        raise LookupError(f"{T['geo']}'s table of {T['year']} holds nothing of {k}, and no product records it")
                    B[to] = B.get(to, 0) + B.pop(k)
                    moved.append(f"{k} to {to}")
            spend = {"DOM": g("domestic", 80), "FOR": g("foreign", 130)}
            r = visitors.ask("tourism.accounts", table=T, purchases=visitors.purchases(spend, B, g("basic_share", 0.85)), nights=nights, units=1e-6)
            m = lambda v: round(v / 1e6, 1)  # noqa: E731
            parts = ["direct value added", "chain value added", "imported purchases", "imported inputs", "product taxes"]
            vals = [r["direct_gva"], r["indirect_gva"], r["demand_imported"], r["induced_imports"], r["induced_taxes"]]
            out = [api.table("tourism_where", {"part": parts, "million_eur": [m(v) for v in vals]},
                             title=f"Where {place['name']}'s visitors' {m(r['demand']):,.0f} million euro of spending at basic prices goes",
                             show={"kind": "chart", "chart": "bar", "x": "part", "y": "million_eur", "how": "sum"})]
            labels = dict(zip(T["products"], T["labels"]))
            prods = sorted(r["products"], key=lambda x: -x["gva_induced"])[:12]
            out.append(api.table("tourism_products", {
                "product": [short(x["product"], labels.get(x["product"], x["product"])) for x in prods], "code": [x["product"] for x in prods],
                "value_added_induced_meur": [m(x["gva_induced"]) for x in prods], "direct_value_added_meur": [m(x["direct_gva"]) for x in prods],
                "tourism_ratio": [round(x["ratio"], 5) for x in prods]},
                title="The products whose value added visitors' spending induces most, and each product's tourism ratio",
                show={"kind": "chart", "chart": "bar", "x": "product", "y": "value_added_induced_meur", "how": "sum"}))
            out.append(api.table("tourism_measures", {
                "measure": ["nights", "spending (purchasers' prices, M EUR)", "demand (basic prices, M EUR)", "domestic demand (M EUR)", "direct value added (M EUR)",
                            "total value added (M EUR)", "value added per unit of domestic demand", "leakage", "direct value added in the country's", "value added per night (EUR)",
                            "unattributed by the table (M EUR)"],
                "value": [round(r["nights"]), m(r["spending"]), m(r["demand"]), m(r["demand_domestic"]), m(r["direct_gva"]), m(r["total_gva"]), round(r["gva_multiplier"], 4),
                          round(r["leakage"], 4), round(r["direct_share_of_gva"], 5), round(r["value_added_per_night"], 2), round(r["unattributed"] / 1e6, 3)]},
                title="The accounts in figures"))
            out.append(api.note(
                f"{place['name']}'s {r['nights'] / 1e6:.1f} million visitor nights in {P['year']}, at the declared {spend['FOR']:g} euro a foreign night and {spend['DOM']:g} a domestic one, "
                f"spend {m(r['spending']):,.0f} million euro, {m(r['demand']):,.0f} million at basic prices. Through {T['geo']}'s table of {T['year']}, it becomes {m(r['direct_gva']):,.0f} million of "
                f"value added directly and {m(r['total_gva']):,.0f} million along the supply chains ({r['gva_multiplier']:.2f} per euro of domestic demand), "
                f"{r['direct_share_of_gva']:.2%} of the country's value added directly; {r['leakage']:.1%} leaks abroad as imports. "
                f"The spending and its basket are declarations, and the national table hides a city's own leakage."
                + (f" The table records nothing of {', '.join(m_.split(' to ')[0] for m_ in moved)}; their spending is carried to the products that record them ({'; '.join(moved)})." if moved else "")
                + f" {T['credit']}; nights from the visitor stock."))
            return out

    return Spending()
