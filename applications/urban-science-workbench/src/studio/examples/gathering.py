"""Where the crowd gathers: a system in Python. Each tick it counts the shared crowd (written by the crowd's script, which the schedule
therefore runs first) on a grid of cells, stands a column where many are, and writes the largest count as the shared resource "densest".
It also keeps that count tick by tick, and draws it as a line in the canvas's frame "densest"."""

import math


def create(api):
    class Gathering(api.System):
        title = "Where the crowd gathers"
        phase = "tick"
        reads = ["crowd"]
        writes = ["densest"]
        resources = {"densest": {"type": "number", "initial": 0}}
        params = [api.param("cell_km", "Cell (km)", 20, minimum=1, maximum=500)]

        def __init__(self):
            self.cells, self.size = {}, 0.2
            self.ticks, self.counts = [], []

        def tick(self, ctx, io):
            crowd = io.get("crowd")
            self.cells = {}
            if not crowd:
                return
            self.size = float(ctx.params.get("cell_km", 20)) / 111.0
            for lon, lat in zip(crowd["lon"], crowd["lat"]):
                key = (math.floor(lon / self.size), math.floor(lat / self.size))
                self.cells[key] = self.cells.get(key, 0) + 1
            io.set("densest", max(self.cells.values()))
            self.ticks.append(ctx.tick)
            self.counts.append(max(self.cells.values()))
            del self.ticks[:-300], self.counts[:-300]

        def draw(self, ctx, io):
            if not self.cells:
                return []
            keys = list(self.cells)
            return [api.columns("Where it gathers", lon=[(i + 0.5) * self.size for i, _ in keys], lat=[(j + 0.5) * self.size for _, j in keys],
                                value=[self.cells[k] for k in keys], radius=self.size * 111e3 * 0.4, top=7000, map="gold"),
                    api.chart("densest", "line", x=self.ticks, y=self.counts, title="The densest cell, tick by tick", xlabel="tick",
                              ylabel="walkers in the densest cell")]

        def save(self):
            return {"ticks": self.ticks, "counts": self.counts}

        def load(self, state):
            self.ticks, self.counts = state["ticks"], state["counts"]

    return Gathering()
