// The crowd's spread: a system in C++. Each tick it reads the shared crowd, finds its centre and the root-mean-square distance of its
// walkers from it, writes that spread (km) as the shared resource "spread_km", draws a ring of that radius about the centre on the
// world, and a histogram of the walkers' distances in the canvas's frame "distances".
// Compiled alone as a shared library, it is swapped in while the project runs whenever it is saved again.

#include "studio_plugin.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace st = science::studio;
using science::Json;

struct Spread : st::System {
  std::vector<double> ring_lon, ring_lat, distances;

  Json describe() const override {
    return st::declaration("The crowd's spread", "tick", {"crowd"}, {"spread_km"}, {}, Json::array(),
                           Json::object({{"spread_km", Json::object({{"type", Json("number")}, {"initial", Json(0.0)}})}}));
  }

  void tick(const st::Context&, st::Io& io) override {
    ring_lon.clear();
    ring_lat.clear();
    distances.clear();
    const Json* crowd = io.get("crowd");
    if (!crowd || !crowd->is_object()) return;
    const auto& lon = (*crowd)["lon"].as_array();
    const auto& lat = (*crowd)["lat"].as_array();
    const std::size_t n = std::min(lon.size(), lat.size());
    if (n == 0) return;
    double clon = 0, clat = 0;
    for (std::size_t i = 0; i < n; i++) { clon += lon[i].as_number(); clat += lat[i].as_number(); }
    clon /= static_cast<double>(n);
    clat /= static_cast<double>(n);
    const double kx = 111.32 * std::cos(clat * M_PI / 180.0), ky = 110.57;
    double s2 = 0;
    for (std::size_t i = 0; i < n; i++) {
      const double dx = (lon[i].as_number() - clon) * kx, dy = (lat[i].as_number() - clat) * ky;
      s2 += dx * dx + dy * dy;
      distances.push_back(std::sqrt(dx * dx + dy * dy));
    }
    const double spread = std::sqrt(s2 / static_cast<double>(n));
    io.set("spread_km", Json(spread));
    for (int k = 0; k <= 96; k++) {
      const double a = 2 * M_PI * k / 96.0;
      ring_lon.push_back(clon + spread * std::cos(a) / kx);
      ring_lat.push_back(clat + spread * std::sin(a) / ky);
    }
  }

  Json draw(const st::Context&, st::Io&) override {
    if (ring_lon.empty()) return Json::array();
    // The distances in twelve bins, from the centre to the farthest walker.
    const double top = *std::max_element(distances.begin(), distances.end()) + 1e-9;
    std::vector<double> centre(12), count(12, 0.0);
    for (int b = 0; b < 12; b++) centre[b] = (b + 0.5) * top / 12;
    for (double d : distances) count[std::min(11, static_cast<int>(d / top * 12))] += 1;
    return Json::array({st::points("The crowd's spread", ring_lon, ring_lat, {}, 3, "orchid"),
                        st::chart("distances", "bars", centre, count, "How far the walkers stand from their centre", "km from the centre", "walkers")});
  }
};

SCIENCE_STUDIO_SYSTEM(Spread)
