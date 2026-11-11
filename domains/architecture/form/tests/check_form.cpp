// Checks of a building's form from its parameters (form.hpp).
//
// - Measures of boxes known by hand: a cube of 10 m standing on the ground has compactness 5 (walls and roof 500 m² over 1000^(2/3)).
// - An upper part on a share c of its envelope covers c of its area (scaled by √c about the envelope's centroid).
// - Storeys are clipped to the height limit; and over every choice of storeys and coverage, the massing made complies with the rules
//   (rules.hpp), the plot ratio kept by taking storeys off.

#include <architecture/form/form.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace af = architecture::form;
namespace ar = architecture::rules;

namespace {
ar::Ring box(double x0, double y0, double x1, double y1) { return {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}; }
double area(const ar::Ring& r) { return std::fabs(physis::computational_geometry::ring_area(r)); }
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    ar::Lot lot{box(0, 0, 10, 10), {1, 0, 0, 0}};
    ar::Rules free;
    free.storey = 2.5;
    const auto m = af::massing(lot, free, 4, 0, 0);
    const auto q = af::measures(m, lot, free.storey);
    s.check("a cube of 10 m: floor 4 × 100", q.floor_area - 400, 1e-9);
    s.check("... volume 1000", q.volume - 1000, 1e-9);
    s.check("... walls and roof 500", q.surface - 500, 1e-9);
    s.check("... compactness 5", q.compactness - 5, 1e-12);
    s.check("... site coverage 1", q.site_coverage - 1, 1e-12);
  }
  ar::Lot lot{box(0, 0, 30, 40), {1, 0, 0, 0}};
  ar::Rules r;
  r.street_setback = 3, r.side_setback = 2, r.upper_setback = 5, r.street_wall = 12, r.max_height = 60, r.plot_ratio = 6, r.storey = 3;
  {
    const auto m = af::massing(lot, r, 4, 6, 0.25);
    s.check("an upper part on a quarter of its envelope covers a quarter of 26 × 30", area(m.upper) - 780 * 0.25, 1e-4);
    const auto c = af::centroid(m.upper), ce = af::centroid(ar::envelope(lot, r).upper);
    s.check("... moved in evenly, so about the rectangle's centroid", std::hypot(c[0] - ce[0], c[1] - ce[1]), 1e-6);
    // (26 − 2d)(30 − 2d) = 195: 4d² − 112d + 585 = 0.
    const double d = (112 - std::sqrt(112.0 * 112 - 16 * 585)) / 8;
    const double w = 26 - 2 * d, l = 30 - 2 * d;
    s.check("... and stands 6 storeys above a podium of 4: 30 m", m.upper_height - 30, 1e-9);
    const auto q = af::measures(m, lot, r.storey);
    // Walls: the podium 2 × (26 + 35) × 12, the tower 2 (w + l) × 18; roofs 910 (the podium's, the tower's included).
    s.check("... its surface: walls 1,464 + 36 (w + l) and roofs 910", q.surface - (1464 + 36 * (w + l) + 910), 1e-4);
    const auto P = af::prisms(m, 0, 1);
    s.require("... and its prisms for the Sun are the podium to 12 m and the tower to 30 m", P.size() == 2 && P[0].height == 12 && P[1].height == 30 && P[1].variable == 1);
  }
  {
    // On an L, a part scaled about the centroid would leave the envelope; moved in evenly, every share stays within it.
    ar::Lot ell{{{0, 0}, {40, 0}, {40, 12}, {12, 12}, {12, 40}, {0, 40}}, {1, 0, 0, 0, 0, 1}};
    ar::Rules q = r;
    q.plot_ratio = 1e9;
    bool inside = true;
    for (double c : {0.25, 0.5, 0.75}) {
      const auto m = af::massing(ell, q, 4, 6, c);
      const auto v = ar::complies(m, ell, q);
      inside = inside && v.ok && std::fabs(area(m.upper) - c * area(ar::envelope(ell, q).upper)) < 1e-3 * area(m.upper);
    }
    s.require("on an L-shaped lot each share of the upper envelope is covered and lies within it", inside);
  }
  {
    ar::Rules low = r;
    low.max_height = 20, low.plot_ratio = 1e9;
    const auto m = af::massing(lot, low, 4, 5, 1);
    s.check("storeys clipped to a height limit of 20 m at 3 m a storey: 4 + 2", m.upper_height - 18, 1e-9);
  }
  {
    int ok = 0, all = 0, kept = 0;
    for (int p = 0; p <= 8; p++)
      for (int u = 0; u <= 20; u += 2)
        for (double c : {0.25, 0.5, 0.75, 1.0}) {
          const auto m = af::massing(lot, r, p, u, c);
          const auto v = ar::complies(m, lot, r);
          all++;
          ok += v.ok || (p > 4 && v.reasons.size() == 1 && v.reasons[0] == "the podium above the street wall on the street line");
          kept += v.floor_area <= 7200 + 1e-6;
        }
    s.require("over " + std::to_string(all) + " choices every massing keeps the plot ratio: " + std::to_string(kept), kept == all);
    s.require("... and complies, but for podiums the caller raised above the street wall: " + std::to_string(ok), ok == all);
  }
  return s.done();
}
