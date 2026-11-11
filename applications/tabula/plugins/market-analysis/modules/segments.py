"""Segments: the rows of a table gathered into kinds that are alike, by k-means."""

import numpy as np


def kmeans(x, k, seed, restarts=10):
    """Lloyd's algorithm from k-means++ beginnings, the best of several: labels, centres and the sum of squares within the segments."""
    rng = np.random.default_rng(seed)
    best = None
    for _ in range(restarts):
        centres = [x[rng.integers(len(x))]]
        for _ in range(1, k):
            d = np.min(((x[:, None, :] - np.array(centres)[None, :, :]) ** 2).sum(axis=2), axis=1)
            centres.append(x[rng.choice(len(x), p=d / d.sum())] if d.sum() > 0 else x[rng.integers(len(x))])
        centres = np.array(centres)
        labels = np.zeros(len(x), dtype=int)
        for _ in range(300):
            new = np.argmin(((x[:, None, :] - centres[None, :, :]) ** 2).sum(axis=2), axis=1)
            for j in range(k):
                if np.any(new == j):
                    centres[j] = x[new == j].mean(axis=0)
            if np.array_equal(new, labels):
                break
            labels = new
        within = float(((x - centres[labels]) ** 2).sum())
        if best is None or within < best[2]:
            best = (labels.copy(), centres.copy(), within)
    return best


def create(api):
    class Segments(api.BoardAnalysis):
        id = "segments"
        title = "Segments, by k-means"
        description = ("Gathers the rows of a table into k segments that are alike in the columns chosen. Each column is set on one scale (its mean taken "
                       "away, divided by its spread), in logarithms if asked and if all its values are positive. Rows that lack one of the columns are left "
                       "out. scikit-learn's KMeans where it is installed, else the same algorithm written here with numpy.")
        params = [api.param("table", "Table", "economies", kind="text", role="table"),
                  api.param("columns", "Alike in", "gdp_per_capita, urban_share", kind="text", role="columns", of="table"),
                  api.param("k", "Segments", 4, kind="integer", minimum=2, maximum=12),
                  api.param("log", "Compare in logarithms", True, kind="boolean"),
                  api.param("seed", "Seed", 1, kind="integer", minimum=0),
                  api.param("name", "Name its table", "segments", kind="text")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            chosen = [c.strip() for c in str(params["columns"]).split(",") if c.strip()]
            if not chosen:
                raise ValueError("no columns are chosen to compare the rows by")
            raw = np.column_stack([t.column(c) for c in chosen])
            rows = np.flatnonzero(np.all(np.isfinite(raw), axis=1))
            k = params["k"]
            if rows.size < k:
                raise ValueError(f"only {rows.size} rows have all of {', '.join(chosen)}: too few for {k} segments")
            x = raw[rows].copy()
            logged = [params["log"] and bool(np.all(x[:, j] > 0)) for j in range(x.shape[1])]
            for j, yes in enumerate(logged):
                if yes:
                    x[:, j] = np.log10(x[:, j])
            spread = x.std(axis=0)
            z = (x - x.mean(axis=0)) / np.where(spread > 0, spread, 1.0)
            context.progress(0.2, "gathering")
            try:
                from sklearn.cluster import KMeans
                model = KMeans(n_clusters=k, n_init=10, random_state=params["seed"]).fit(z)
                labels, within, how = model.labels_, float(model.inertia_), "scikit-learn's KMeans"
            except ImportError:
                labels, _, within = kmeans(z, k, params["seed"])
                how = "k-means written with numpy (scikit-learn is not installed)"
            # Segments are numbered by the first column, from the least, so that a run names them alike.
            order = np.argsort([np.median(raw[rows][labels == j, 0]) if np.any(labels == j) else np.inf for j in range(k)])
            rank = np.empty(k, dtype=int)
            rank[order] = np.arange(k)
            segment = rank[labels]
            total = float((z ** 2).sum())
            names = [f"S{j + 1}" for j in segment]
            columns = {c: [t.raw(c)[i] for i in rows] for c in t.columns}
            columns["segment"] = names
            profile = {"segment": [f"S{j + 1}" for j in range(k)], "rows": [int((segment == j).sum()) for j in range(k)]}
            for c, column in zip(chosen, raw[rows].T):
                profile[f"median_{c}"] = [float(np.median(column[segment == j])) if np.any(segment == j) else None for j in range(k)]
            name = params["name"] or "segments"
            scale = ", ".join(f"{c}{' (log)' if yes else ''}" for c, yes in zip(chosen, logged))
            return [api.table(name, columns, title=f"{t.title or t.name}, in {k} segments", source=t.source),
                    api.table(f"{name}_profile", profile, title="The segments: how many rows, and the median of each column"),
                    api.note(f"{rows.size} rows in {k} segments by {scale}; {t.rows - rows.size} left out for a missing value. The segments account for "
                             f"{100 * (1 - within / total) if total > 0 else 100:.0f}% of the spread of the rows. {how}, seed {params['seed']}.")]

    return Segments()
