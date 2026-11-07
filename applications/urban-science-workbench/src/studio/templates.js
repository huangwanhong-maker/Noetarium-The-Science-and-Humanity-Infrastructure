// A new script in each language: the smallest system that runs, reads a shared resource, writes one, and draws where the camera looks.
// The project's examples (examples/) are fuller, and share one resource across the three languages.

export const TEMPLATES = {
  js: (title) => `// ${title}: a system in JavaScript. It runs each tick, in the order the schedule gives from what it reads and writes.
export default (api) => {
  let angle = 0;
  return {
    title: ${JSON.stringify(title)},
    phase: 'tick',
    reads: ['speed'],          // shared resources it reads this tick
    writes: [],                // shared resources it writes
    previous: [],              // shared resources it reads as they were at the end of the last tick
    resources: { speed: { type: 'number', initial: 0.5 } },
    params: [api.param('count', 'Points', 36, { min: 3, max: 2000 }), api.param('radius_km', 'Radius (km)', 120, { min: 1, max: 5000 })],
    tick(ctx, io) { angle += (io.get('speed') ?? 0.5) * ctx.dt; },
    draw(ctx) {
      const v = ctx.view || { lon: 0, lat: 0 }, r = ctx.params.radius_km / 111, n = ctx.params.count;
      const lon = [], lat = [], value = [];
      for (let k = 0; k < n; k++) {
        const a = angle + (2 * Math.PI * k) / n;
        lat.push(v.lat + r * Math.sin(a));
        lon.push(v.lon + (r * Math.cos(a)) / Math.cos((v.lat * Math.PI) / 180));
        value.push(k / n);
      }
      return [api.points(${JSON.stringify(title)}, { lon, lat, value, size: 6, map: 'orchid' })];
    },
    save: () => ({ angle }),
    load(state) { angle = state.angle; },
  };
};
`,
  python: (title) => `"""${title}: a system in Python, run by the Studio's Python kernel each tick, in the schedule's order."""

import math


def create(api):
    class System(api.System):
        title = ${JSON.stringify(title)}
        phase = "tick"
        reads = ["speed"]
        writes = []
        previous = []
        resources = {"speed": {"type": "number", "initial": 0.5}}
        params = [api.param("count", "Points", 24, minimum=3, maximum=2000), api.param("radius_km", "Radius (km)", 200, minimum=1, maximum=5000)]

        def __init__(self):
            self.angle = 0.0

        def tick(self, ctx, io):
            self.angle -= (io.get("speed") or 0.5) * ctx.dt

        def draw(self, ctx, io):
            v = ctx.view or {"lon": 0, "lat": 0}
            r, n = ctx.params["radius_km"] / 111.0, int(ctx.params["count"])
            lon = [v["lon"] + r * math.cos(self.angle + 2 * math.pi * k / n) / math.cos(math.radians(v["lat"])) for k in range(n)]
            lat = [v["lat"] + r * math.sin(self.angle + 2 * math.pi * k / n) for k in range(n)]
            return [api.points(${JSON.stringify(title)}, lon=lon, lat=lat, value=[k / n for k in range(n)], size=6, map="gold")]

        def save(self):
            return {"angle": self.angle}

        def load(self, state):
            self.angle = state["angle"]

    return System()
`,
  cpp: (title) => `// ${title}: a system in C++, compiled alone as a shared library and swapped in while the project runs.
#include "studio_plugin.hpp"

#include <cmath>
#include <vector>

namespace st = science::studio;
using science::Json;

struct System : st::System {
  double angle = 0;
  std::vector<double> lon, lat;

  Json describe() const override {
    return st::declaration(${JSON.stringify(title)}, "tick", {"speed"}, {}, {}, Json::array(),
                           Json::object({{"speed", Json::object({{"type", Json("number")}, {"initial", Json(0.5)}})}}));
  }
  void tick(const st::Context& c, st::Io& io) override {
    angle += io.number("speed", 0.5) * c.dt;
    const double vlon = c.view.is_object() ? c.view["lon"].as_number() : 0, vlat = c.view.is_object() ? c.view["lat"].as_number() : 0;
    lon.clear(); lat.clear();
    for (int k = 0; k < 48; k++) {
      const double a = 2 * angle + 2 * M_PI * k / 48.0, r = (1.2 + 0.4 * std::sin(3 * a)) * 150 / 111.0;
      lon.push_back(vlon + r * std::cos(a) / std::cos(vlat * M_PI / 180));
      lat.push_back(vlat + r * std::sin(a));
    }
  }
  Json draw(const st::Context&, st::Io&) override { return Json::array({st::points(${JSON.stringify(title)}, lon, lat, {}, 5, "plum")}); }
  Json save() const override { return Json::object({{"angle", Json(angle)}}); }
  void load(const Json& s) override { if (s.is_object() && s.has("angle")) angle = s["angle"].as_number(); }
};

SCIENCE_STUDIO_SYSTEM(System)
`,
};
