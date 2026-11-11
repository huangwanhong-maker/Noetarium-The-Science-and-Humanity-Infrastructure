"""A figure from a file: a file of rows and columns named in the panel, read, drawn and given to the board as a table."""


def create(api):
    class FigureFromFile(api.BoardAnalysis):
        id = "figure-from-file"
        title = "A figure from a file"
        description = ("Reads a file of rows and columns (CSV, with commas, semicolons or tabs), gives it to the board as a table, and draws one column "
                       "against another. Name the file in the panel: choose one of this computer, write its path, or data:<dataset>/<file> of the data library.")
        params = [api.param("file", "File", "", kind="text", role="file"),
                  api.param("x", "Across (a column's name)", "", kind="text"),
                  api.param("y", "Up (a column's name)", "", kind="text"),
                  api.param("form", "Drawn as", "line", kind="choice", options=["line", "points", "bars"]),
                  api.param("name", "Name its table", "from_file", kind="text")]

        def run(self, board, params, context):
            t = api.read_delimited(api.file(params["file"]), params["name"] or "from_file")
            results = [api.table(t.name, {c: t.raw(c) for c in t.columns}, title=t.title, source=str(params["file"]))]
            numeric = t.numeric()
            x = params["x"] or (t.columns[0] if t.columns else "")
            y = params["y"] or next((c for c in numeric if c != x), "")
            if not y:
                return results + [api.note(f"{t.rows} rows of {', '.join(t.columns)}. No column of numbers to draw.")]
            try:
                import matplotlib
                matplotlib.use("Agg")
                import matplotlib.pyplot as plt
            except ImportError:
                return results + [api.note(f"{t.rows} rows. matplotlib is not installed here: lay the table on the board as a chart instead.")]
            fig, ax = plt.subplots(figsize=(6.4, 3.6))
            ys = t.column(y)
            if t.kinds.get(x) == "number":
                xs = t.column(x)
                if params["form"] == "line":
                    order = xs.argsort()
                    ax.plot(xs[order], ys[order], color="#9678bf", linewidth=1.6)
                elif params["form"] == "bars":
                    ax.bar(xs, ys, color="#c4688f", width=(xs.max() - xs.min()) / max(len(xs), 1) * 0.8 or 0.8)
                else:
                    ax.scatter(xs, ys, s=14, color="#9678bf", alpha=0.75, linewidths=0)
            else:
                names = t.text(x)[:40]
                ax.bar(range(len(names)), ys[:40], color="#c4688f")
                ax.set_xticks(range(len(names)), names, rotation=60, ha="right", fontsize=7)
            ax.set_xlabel(x)
            ax.set_ylabel(y)
            for side in ("top", "right"):
                ax.spines[side].set_visible(False)
            ax.grid(True, linewidth=0.4, alpha=0.5)
            results.append(api.figure(fig, f"{y} against {x}"))
            plt.close(fig)
            return results + [api.note(f"{t.rows} rows of {', '.join(t.columns)}.")]

    return FigureFromFile()
