// Checks of disruption (disruption.hpp).
//
// - Pregnolato et al.'s curve: the whole speed in no water, v(100 mm)/v(0) = 40.6548/86.9448 at 0.1 m, nothing from 0.3 m.
// - The frame carries a point to longitude and latitude and back to within a millimetre, and WGS 84's radii of curvature at 45° are
//   6,367,381.8 m and 6,388,838.3 m.
// - A grid with a band of water 0.6 m deep across a lattice's middle row of streets: the links crossing or along the band read 0.6 m,
//   the others nothing; cars keep no speed there, people on foot none either, and links outside keep all of theirs.
// - Scenarios of three return periods are weighted to sum to one, the undisturbed first.
// - Refusals: a curve of one point, shares outside [0, 1].

#include <disaster_risk/disruption/disruption.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace dd = disaster_risk::disruption;
namespace ll = urban_risk_management::lifelines;

namespace {
bool throws(auto&& f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    const auto c = dd::pregnolato_2017();
    s.check("Pregnolato et al.: all the speed in no water", c(0) - 1, 1e-15);
    s.check("v(100 mm) / v(0) at 0.1 m", c(0.1) - 40.6548 / 86.9448, 1e-12);
    s.require("nothing from 0.3 m", c(0.3) == 0 && c(1.0) == 0 && c(0.29) > 0);
  }
  const dd::Frame f{8.45, -13.2};
  {
    const double lat = 8.461, lon = -13.187;
    const double x = f.east(lon), y = f.north(lat);
    s.check("the frame carries a point there and back", std::hypot(f.lat(y) - lat, f.lon(x) - lon) * 110600, 1e-3);
    const dd::Frame f45{45, 0};
    s.check("WGS 84's meridian radius at 45° is 6,367,381.8 m (m)", f45.meridian() - 6367381.8, 0.5);
    s.check("and its prime vertical radius 6,388,838.3 m (m)", f45.prime() - 6388838.3, 0.5);
  }
  {
    // A 5 × 5 lattice 100 m apart about the frame's centre, and water 0.6 m deep in a band 20 m wide across y = 200.
    std::vector<ll::Road> roads;
    for (int i = 0; i < 5; i++) {
      ll::Road h{{}, 10, 8, ""}, v{{}, 10, 8, ""};
      for (int j = 0; j < 5; j++) h.xy.push_back(j * 100.0), h.xy.push_back(i * 100.0), v.xy.push_back(i * 100.0), v.xy.push_back(j * 100.0);
      roads.push_back(h), roads.push_back(v);
    }
    ll::GraphOptions o;
    o.stitch = 0;
    const auto g = ll::graph_of(roads, o);
    urban::exposure::Grid grid;
    grid.south = f.lat(-50), grid.north = f.lat(450), grid.west = f.lon(-50), grid.east = f.lon(450);
    grid.width = 500, grid.height = 500, grid.unit = 0.05;
    grid.values.assign(grid.width * grid.height, 0);
    for (std::size_t j = 0; j < grid.height; j++) {
      const double y = -50 + (j + 0.5);
      if (std::fabs(y - 200) <= 10)
        for (std::size_t i = 0; i < grid.width; i++) grid.values[j * grid.width + i] = 12;  // 0.6 m
    }
    const auto D = dd::depths(g, grid, f, 2);
    bool right = true;
    for (std::size_t l = 0; l < g.links.size(); l++) {
      const auto& L = g.links[l];
      double lo = 1e9, hi = -1e9;
      for (std::size_t p : L.points) lo = std::min(lo, g.y[p]), hi = std::max(hi, g.y[p]);
      const bool wet = lo <= 210 && hi >= 190;
      right = right && (wet ? std::fabs(D.deepest[l] - 0.6) < 1e-12 : D.deepest[l] == 0) && D.mapped[l];
    }
    s.require("the links crossing or along the band read its 0.6 m, the others nothing", right);
    const auto car = dd::state(D, dd::pregnolato_2017()), foot = dd::state(D, dd::walking_declared());
    bool shares = true;
    for (std::size_t l = 0; l < g.links.size(); l++) shares = shares && (D.deepest[l] > 0 ? car[l] == 0 && foot[l] == 0 : car[l] == 1 && foot[l] == 1);
    s.require("cars and people on foot keep nothing there and everything elsewhere", shares);
    const auto sc = dd::scenarios({10, 100, 1000}, {car, car, car});
    double sum = 0;
    for (const auto& z : sc) sum += z.weight;
    s.require("scenarios of three return periods weighted to sum to one, the undisturbed first",
              sc.size() == 4 && sc[0].state.empty() && std::fabs(sum - 1) < 1e-15 && sc[2].name == "100-year");
  }
  {
    s.require("refusals: a curve of one point, shares outside [0, 1]",
              throws([] { dd::Curve{"x", {0}, {1}, ""}.check(); }) && throws([] { dd::Curve{"x", {0, 1}, {1, 2}, ""}.check(); }));
  }
  return s.done();
}
