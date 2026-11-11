"""Places of business in the City of Melbourne, by what they do: where the offers are."""

import csv


def create(api):
    class Places(api.BoardAnalysis):
        id = "places"
        title = "Places of business in Melbourne"
        description = ("The establishments of one kind in the City of Melbourne in one year of its Census of Land Use and Employment (CLUE), each with its "
                       "name, its address and its place: for a map of where the offers are. The kind is matched within the ANZSIC description, whatever its "
                       "case: 'cafes' finds Cafes and Restaurants.")
        params = [api.param("industry", "Kind of business", "Cafes and Restaurants", kind="text"),
                  api.param("year", "Year (0: the latest)", 0, kind="integer", minimum=0, maximum=2100),
                  api.param("area", "Small area (empty: all)", "", kind="text"),
                  api.param("name", "Name its table", "places", kind="text")]

        def run(self, board, params, context):
            path = api.cache("melbourne-clue-establishments", "all", "establishments.csv")
            want, area = params["industry"].strip().lower(), params["area"].strip().lower()
            if not want:
                raise ValueError("no kind of business is named")
            found, latest, kinds = {}, 0, {}
            with open(path, encoding="utf-8-sig", newline="") as f:
                for row in csv.DictReader(f, delimiter=";"):
                    try:
                        year = int(row["census_year"])
                    except (KeyError, ValueError):
                        continue
                    latest = max(latest, year)
                    kind = row.get("industry_anzsic4_description") or ""
                    if want not in kind.lower() or (area and area not in (row.get("clue_small_area") or "").lower()):
                        continue
                    try:
                        lon, lat = float(row["longitude"]), float(row["latitude"])
                    except (KeyError, ValueError):
                        continue
                    found.setdefault(year, []).append((row.get("trading_name") or "", row.get("business_address") or "", row.get("clue_small_area") or "", kind, lon, lat))
                    kinds[kind] = kinds.get(kind, 0) + 1
            year = params["year"] or latest
            rows = found.get(year, [])
            if not rows:
                years = ", ".join(str(y) for y in sorted(found)) or "none"
                return [api.note(f"No establishment whose kind holds '{params['industry']}' in {year}. Years with any: {years}.")]
            columns = {"name": [r[0] for r in rows], "address": [r[1] for r in rows], "area": [r[2] for r in rows], "industry": [r[3] for r in rows],
                       "lon": [r[4] for r in rows], "lat": [r[5] for r in rows]}
            matched = sorted({r[3] for r in rows})
            return [api.table(params["name"] or "places", columns, title=f"{', '.join(matched)} in the City of Melbourne, {year}",
                              source="City of Melbourne, Census of Land Use and Employment (CLUE), CC BY 4.0"),
                    api.note(f"{len(rows):,} establishments in {year}" + (f" in {params['area']}" if area else "") + f": {', '.join(matched)}.")]

    return Places()
