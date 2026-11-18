"""Evidence from a table: the rows of any table of the project (a survey, a scorecard, a diagnostic, a city's own statistics) answered
as evidence, so that the engagement can gather them with what the other tools computed. A column names each item's quantity; others
give its value, interval, level, unit, scale, place and period; one column may hold the readings it was computed under, written
"term=reading; term=reading"."""


def create(api):
    class EvidenceFromTable(api.BoardAnalysis):
        id = "evidence_table"
        title = "Evidence from a table"
        description = ("Each row of a table becomes an item of evidence: its quantity (an index's id in the urban-science catalogue, or a name), "
                       "its value or interval or level, and where and when it holds. Rows without a quantity, or without a value, interval or level, "
                       "are left out and counted. Readings are written term=reading; term=reading. Nothing is computed: what the table says is "
                       "carried, with the table named as its source.")
        params = [api.param("table", "Table", "", kind="text", role="table"),
                  api.param("quantity", "Column of the quantity", "quantity", kind="text", role="column", of="table"),
                  api.param("value", "Column of the value", "value", kind="text", role="column", of="table"),
                  api.param("low", "Column of the low end (blank: none)", "", kind="text"),
                  api.param("high", "Column of the high end (blank: none)", "", kind="text"),
                  api.param("level", "Column of a level, for ordinal findings (blank: none)", "", kind="text"),
                  api.param("unit", "Unit (or a column's name)", "", kind="text"),
                  api.param("scale", "Scale", "ratio", kind="text", help="nominal, ordinal, interval, ratio or absolute"),
                  api.param("place", "Column of the place (blank: none)", "", kind="text"),
                  api.param("readings", "Column of the readings (blank: none)", "", kind="text")]

        def run(self, board, params, context):
            name = params.get("table")
            if not name:
                raise ValueError("choose a table of the project")
            t = board.table(name)
            rows = t.rows
            data = {c: t.raw(c) for c in t.columns}
            col = lambda key: (params.get(key) or "").strip()

            def cell(c, i):
                return data[c][i] if c and c in data and i < len(data[c]) else None

            def number(x):
                try:
                    return None if x is None or x == "" else float(x)
                except (TypeError, ValueError):
                    return None

            items, left = [], 0
            for i in range(rows):
                q = cell(col("quantity"), i)
                item = {"quantity": str(q).strip()} if q not in (None, "") else None
                if item is None:
                    left += 1
                    continue
                for key in ("value", "low", "high"):
                    v = number(cell(col(key), i))
                    if v is not None:
                        item[key] = v
                lv = cell(col("level"), i)
                if lv not in (None, ""):
                    item["level"] = str(lv)
                if ("low" in item) != ("high" in item):
                    item.pop("low", None), item.pop("high", None)
                if not any(k in item for k in ("value", "low", "level")):
                    left += 1
                    continue
                unit = col("unit")
                if unit:
                    item["unit"] = str(cell(unit, i)) if unit in data else unit
                if col("scale"):
                    item["scale"] = col("scale")
                pl = cell(col("place"), i)
                if pl not in (None, ""):
                    item["place"] = {"name": str(pl)}
                rd = cell(col("readings"), i)
                if rd not in (None, ""):
                    pairs = [p.split("=", 1) for p in str(rd).split(";") if "=" in p]
                    item["readings"] = {a.strip(): b.strip() for a, b in pairs if a.strip()}
                item["source"] = f"the table {name}"
                items.append(item)
            if not items:
                raise ValueError(f"no row of {name} gives a quantity with a value, an interval or a level")
            out = [api.evidence(items, f"evidence from {name}"),
                   {"kind": "note", "text": f"{len(items)} items of evidence from {name}" + (f"; {left} rows left out (no quantity, or nothing measured)." if left else ".")}]
            return out

    return EvidenceFromTable()
