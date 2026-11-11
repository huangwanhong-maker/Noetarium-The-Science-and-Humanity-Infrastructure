// Checks of the rules that bound a building's form (rules.hpp), on lots whose envelopes are known by hand.
//
// - A 30 × 40 lot facing a street on its south edge, set back 3 from the street and 2 elsewhere, stepping back 5 more above a street
//   wall of 12 m: its ground envelope is 26 × 35, its upper one 26 × 30.
// - A massing complies or fails with each reason named: outside the setbacks, above the street wall on the street line, the upper
//   part not set back, above the height limit, beyond the plot ratio.
// - A passage 4 m wide across the lot leaves two parts of 13 × 40, their edges along it facing it as a street.

#include <architecture/rules/rules.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace ar = architecture::rules;

namespace {
ar::Ring box(double x0, double y0, double x1, double y1) { return {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}; }
double area(const ar::Ring& r) { return std::fabs(physis::computational_geometry::ring_area(r)); }
bool says(const ar::Compliance& c, const std::string& why) { return std::find(c.reasons.begin(), c.reasons.end(), why) != c.reasons.end(); }
}  // namespace

int main() {
  calx::verification::Suite s;
  ar::Lot lot{box(0, 0, 30, 40), {1, 0, 0, 0}};
  ar::Rules r;
  r.street_setback = 3, r.side_setback = 2, r.upper_setback = 5, r.street_wall = 12, r.max_height = 60, r.plot_ratio = 6, r.storey = 3;
  const auto e = ar::envelope(lot, r);
  s.check("the ground envelope of a 30 × 40 lot, 3 from its street and 2 from its other edges, is 26 × 35", area(e.ground) - 910, 1e-9);
  s.check("... and above a street wall stepping back 5 more, 26 × 30", area(e.upper) - 780, 1e-9);
  s.check("... and its plot ratio of 6 allows 7,200 m² of floor", e.max_floor_area - 7200, 1e-9);
  {
    ar::Massing m{e.ground, 12, box(2 + 6.5, 8 + 7.5, 28 - 6.5, 38 - 7.5), 30};  // a podium of 4 storeys, a tower of 6 on a quarter of the upper envelope
    const auto c = ar::complies(m, lot, r);
    s.require("a podium of 4 storeys on its envelope and a tower of 6 set back complies: " + std::to_string(c.floor_area) + " m²", c.ok && c.reasons.empty());
    s.check("... its floor 910 × 4 + 195 × 6", c.floor_area - (910 * 4 + 195 * 6), 1e-9);
  }
  {
    ar::Massing m{e.ground, 12, e.upper, 60};
    const auto c = ar::complies(m, lot, r);
    s.require("a tower on the whole upper envelope to 60 m exceeds the plot ratio, and only that", !c.ok && c.reasons.size() == 1 && says(c, "beyond the plot ratio"));
  }
  {
    ar::Massing m{e.ground, 15, {}, 0};
    s.require("a podium of 15 m on the street line rises above the street wall", says(ar::complies(m, lot, r), "the podium above the street wall on the street line"));
    ar::Massing up{e.ground, 12, e.ground, 18};
    s.require("an upper part on the street line is not set back", says(ar::complies(up, lot, r), "the upper part not set back from the street"));
    ar::Massing tall{e.ground, 12, box(12, 20, 18, 26), 63};
    s.require("a tower to 63 m is above the height limit of 60", says(ar::complies(tall, lot, r), "above the height limit"));
    ar::Massing out{lot.ring, 6, {}, 0};
    s.require("a podium on the whole lot is outside the setbacks", says(ar::complies(out, lot, r), "the podium outside the setbacks"));
  }
  {
    // A passage 4 m wide along x = 15.
    const auto parts = ar::split(lot, {15, -10}, {15, 50}, 4);
    bool shape = parts.size() == 2;
    double total = 0;
    for (const auto& p : parts) total += p.area(), shape = shape && std::fabs(p.area() - 520) < 1e-9;
    s.require("a passage 4 m wide across the lot leaves two parts of 13 × 40", shape && std::fabs(total - 1040) < 1e-9);
    // Each part faces the street on its south edge and the passage on one side; its envelope is set back 3 from both.
    bool fronts = true;
    for (const auto& p : parts) {
      int faced = 0;
      for (char f : p.street) faced += f;
      fronts = fronts && faced == 2 && std::fabs(area(ar::envelope(p, r).ground) - (13 - 3 - 2) * (40 - 3 - 2)) < 1e-9;
    }
    s.require("... each part faces two streets, the old and the passage, and is set back 3 from both", fronts);
  }
  {
    // An L-shaped lot, set back 1 from every edge: its envelope keeps both arms (physis's inset).
    ar::Lot ell{{{0, 0}, {20, 0}, {20, 8}, {8, 8}, {8, 20}, {0, 20}}, {1, 0, 0, 0, 0, 1}};
    ar::Rules q;
    q.street_setback = 1, q.side_setback = 1;
    s.check("an L-shaped lot set back 1 all round keeps both arms: 18 × 6 + 6 × 12", area(ar::envelope(ell, q).ground) - (18 * 6 + 6 * 12), 1e-9);
  }
  return s.done();
}
