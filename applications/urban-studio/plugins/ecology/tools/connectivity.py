"""How the canopy holds together as habitat: patches, the movement between them across the city, and what each patch does for it."""

import math
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).parent))
import _forest as f  # noqa: E402


def lattice(lat, lon, cover):
    """The regular grid a table of cells stands on (cells absent from it bare), from the spacing of their centres."""
    ys, xs = sorted(set(round(v, 7) for v in lat)), sorted(set(round(v, 7) for v in lon))
    dy = min((b - a for a, b in zip(ys, ys[1:]) if b - a > 1e-9), default=None)
    dx = min((b - a for a, b in zip(xs, xs[1:]) if b - a > 1e-9), default=None)
    if not dy or not dx:
        raise ValueError("the cells do not stand on a grid")
    y0, x0 = ys[0], xs[0]
    nx, ny = int(round((xs[-1] - x0) / dx)) + 1, int(round((ys[-1] - y0) / dy)) + 1
    grid = [0.0] * (nx * ny)
    for a, b, c in zip(lat, lon, cover):
        i, j = int(round((b - x0) / dx)), int(round((a - y0) / dy))
        grid[j * nx + i] = max(grid[j * nx + i], c)
    cell = dy * 110574.0
    return grid, nx, ny, x0, y0, dx, dy, cell


def clusters(grid, nx, ny, at_least):
    """Eight-connected clusters of cells with cover ≥ at_least."""
    label, out = [-1] * len(grid), []
    for s in range(len(grid)):
        if grid[s] < at_least or label[s] >= 0:
            continue
        stack, members = [s], []
        label[s] = len(out)
        while stack:
            c = stack.pop()
            members.append(c)
            i, j = c % nx, c // nx
            for di in (-1, 0, 1):
                for dj in (-1, 0, 1):
                    a, b = i + di, j + dj
                    if (di or dj) and 0 <= a < nx and 0 <= b < ny:
                        d = b * nx + a
                        if label[d] < 0 and grid[d] >= at_least:
                            label[d] = len(out)
                            stack.append(d)
        out.append(members)
    return out


def create(api):
    class Connectivity(api.BoardAnalysis):
        id = "connectivity"
        title = "How the canopy holds together"
        description = ("The canopy read as habitat: patches where it is dense, the city between them as a surface of resistance to movement (the "
                       "less canopy, the harder), the least-cost distance and circuit theory's effective resistance between patches, the current "
                       "that shows where movement is funnelled, and each patch's part in the probability of connectivity: the habitat within it, "
                       "the movement it starts or ends, and the movement between others it carries.")
        params = [api.param("table", "Canopy cells", "canopy", kind="text", role="table", help="cells of a regular grid with their canopy cover"),
                  api.param("lat", "Latitude", "lat", kind="text", role="column", of="table"),
                  api.param("lon", "Longitude", "lon", kind="text", role="column", of="table"),
                  api.param("cover", "Cover", "cover", kind="text", role="column", of="table"),
                  api.param("patch_cover", "Cover of a patch", 0.6, kind="number", minimum=0.1, maximum=1, step=0.05),
                  api.param("patches", "Largest patches taken", 8, kind="integer", minimum=2, maximum=16),
                  api.param("r_canopy", "Resistance under canopy", 1, kind="number", minimum=0.1, maximum=1000),
                  api.param("r_open", "Resistance in the open", 40, kind="number", minimum=0.1, maximum=10000,
                            help="between the two, resistance falls linearly with cover (presumed; a species' own surface would replace it)"),
                  api.param("median", "Median dispersal (m of open ground)", 400, kind="number", minimum=10, maximum=100000,
                            help="the least-cost distance, in metres at the open ground's resistance, at which a movement succeeds half the time")]

        def run(self, board, params, context):
            t = board.table(params["table"])
            lat, lon, cov = t.column(params["lat"]), t.column(params["lon"]), t.column(params["cover"])
            keep = [i for i in range(t.rows) if math.isfinite(lat[i]) and math.isfinite(lon[i]) and math.isfinite(cov[i])]
            grid, nx, ny, x0, y0, dx, dy, cell = lattice([lat[i] for i in keep], [lon[i] for i in keep], [cov[i] for i in keep])
            rc, ro = float(params["r_canopy"]), float(params["r_open"])
            resistance = [ro - (ro - rc) * min(1.0, max(0.0, c)) for c in grid]
            found = sorted(clusters(grid, nx, ny, float(params["patch_cover"])), key=len, reverse=True)[: int(params["patches"])]
            if len(found) < 2:
                raise ValueError("fewer than two patches at this cover: lower the cover of a patch")
            context.progress(0.3, f"{len(found)} patches on a grid of {nx} × {ny}; solving the circuits")
            c = f.ecology.ask("circuits", nx=nx, ny=ny, cell=cell, resistance=resistance, eight=True, patches=found, timeout=1200)
            area = [len(m) * cell * cell for m in found]
            # Least-cost distances in metres of open ground: the cost divided by the open ground's resistance.
            dist = [[(x / ro if x is not None else None) for x in row] for row in c["least_cost"]]
            landscape = nx * ny * cell * cell
            p = f.ecology.ask("patches", attribute=area, distances=dist, median=float(params["median"]), threshold=2 * float(params["median"]), landscape_area=landscape)
            centre = lambda m: (y0 + dy * sum(q // nx for q in m) / len(m), x0 + dx * sum(q % nx for q in m) / len(m))  # noqa: E731
            pts = [centre(m) for m in found]
            patches = {"patch": [f"P{k + 1}" for k in range(len(found))], "lat": [a for a, _ in pts], "lon": [b for _, b in pts],
                       "area_ha": [round(a / 1e4, 2) for a in area], "importance": [round(x, 2) for x in p["dPC"]], "within": [round(x, 2) for x in p["intra"]],
                       "flux": [round(x, 2) for x in p["flux"]], "connector": [round(x, 2) for x in p["connector"]]}
            current = {"lat": [], "lon": [], "current": []}
            top = max((v for v in c["current"] if v), default=0)
            for q, v in enumerate(c["current"]):
                if v and v > 0.005 * top:
                    current["lat"].append(round(y0 + dy * (q // nx), 6))
                    current["lon"].append(round(x0 + dx * (q % nx), 6))
                    current["current"].append(round(v, 4))
            pairs = {"from": [], "to": [], "effective_resistance": [], "least_cost_m": []}
            for i in range(len(found)):
                for j in range(i + 1, len(found)):
                    pairs["from"].append(f"P{i + 1}"), pairs["to"].append(f"P{j + 1}")
                    pairs["effective_resistance"].append(c["resistance"][i][j])
                    pairs["least_cost_m"].append(round(dist[i][j]) if dist[i][j] is not None else None)
            best = max(range(len(found)), key=lambda k: p["connector"][k])
            return [api.table("connectivity_current", current, title="The current between the patches, summed over every pair: where movement is funnelled",
                              roles={"lat": "lat", "lon": "lon", "amount": "current"}, show={"kind": "layer", "layer": "points", "colour": "current", "map": "orchid"}),
                    api.table("habitat_patches", patches, title="The patches and their part in the connectivity of the whole",
                              roles={"lat": "lat", "lon": "lon", "name": "patch", "amount": "importance"},
                              show={"kind": "layer", "layer": "points", "colour": "importance", "size": "area_ha", "map": "gold"}),
                    api.table("patch_pairs", pairs, title="Between the patches: effective resistance and least-cost distance"),
                    api.note(f"{len(found)} patches of canopy at {f.share(float(params['patch_cover']))} cover or more, {sum(area) / 1e4:,.1f} ha in all, "
                             f"amount to an equivalent connected area of {p['eca'] / 1e4:,.1f} ha (PC = {p['pc']:.2e}). The patch that most carries "
                             f"movement between others is {patches['patch'][best]} (connector {p['connector'][best]:.1f}% of the whole). "
                             f"Presumed, not observed: resistance falls linearly from {ro:g} in the open to {rc:g} under full canopy, and a movement "
                             f"succeeds half the time at {params['median']} m of open ground; a species' own surface and dispersal would replace both.")]
    return Connectivity()
