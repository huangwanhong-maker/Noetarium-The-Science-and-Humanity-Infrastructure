// Checks of urban::openspace, each worked by hand: a square's area with a hole, and what it contains; the union of two overlapping
// squares on the raster; reach along a street to a park, a place inside it counted, the distance's bound; a least area that drops a small
// space; the straight line round a corner; a kind not counted; refusals.

#include <urban/openspace/openspace.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace urban::openspace;
using urban::transit::Place;

namespace {
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
Space square(const std::string& kind, double x, double y, double side) { return {kind, {x, y, x + side, y, x + side, y + side, x, y + side}, {}}; }
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    Space p = square("park", 0, 0, 100);
    p.holes = {{40, 40, 60, 40, 60, 60, 40, 60}};
    s.check("a square of 100 m with a hole of 20 m: 10,000 − 400 m²", std::fabs(p.area() - 9600), 1e-9);
    s.require("... it contains a point in its ring, not one in its hole or outside", p.contains(10, 10) && !p.contains(50, 50) && !p.contains(150, 50));
    const Space a = square("park", 0, 0, 100), b = square("park", 50, 0, 100);
    s.check("two squares overlapping by half: their union 15,000 m² on a raster of 1 m", std::fabs(union_area({&a, &b}, 1.0) - 15000), 1e-9);
    s.check("... and the square with its hole, 9,600 m²", std::fabs(union_area({&p}, 1.0) - 9600), 1e-9);
    const Box half{0, 0, 50, 100};
    s.check("... within a box of the left half, the union counts 50 · 100 m² only", std::fabs(union_area({&a, &b}, 1.0, &half) - 5000), 1e-9);
  }
  {
    // A street of 1.2 km along y = 0; a park 100 m square across it from x = 500 to 600; four places of ten people.
    const auto walk = urban::transit::walking({{0, 0, 1200, 0}});
    const std::vector<Space> spaces = {{"park", {500, -50, 600, -50, 600, 50, 500, 50}, {}}, square("park", 1150, 10, 20)};
    const std::vector<Place> places = {{0, 0, 10}, {160, 0, 10}, {550, 0, 10}, {1000, 0, 10}};
    Reading r{"parks within 400 m", {"park"}, 0, 400, true, ""};
    const auto R = reach(walk, places, spaces, r, 1e6);
    s.check("500 m from the park's edge out, 340 m in, inside it in, and the place at 1,000 m 160 m from the small park, in: 3/4",
            std::fabs(R.share - 0.75) + std::fabs(R.distance[0] - 500) + std::fabs(R.distance[1] - 340) + R.distance[2], 1e-9);
    s.check("... the counted spaces' union is 10,000 + 400 m², a share of 10,400/10⁶", std::fabs(R.area - 10400) + std::fabs(R.area_share - 0.0104), 1e-6);
    Reading big = r;
    big.least_area = 5000;
    const auto B = reach(walk, places, spaces, big, 1e6);
    s.require("a least area of 0.5 ha drops the small park: the place at 1,000 m is 400 m from the large one's edge, still in, at the bound", B.spaces == 1 && B.served[3] && std::fabs(B.distance[3] - 400) < 1e-9);
    Reading other = r;
    other.kinds = {"plaza"};
    s.require("a kind not counted: no space, no one within reach", reach(walk, places, spaces, other, 1e6).within == 0);
  }
  {
    // Round a corner: a place 300 m up one street, a park beside the other street 250 m along it.
    const auto walk = urban::transit::walking({{0, 0, 0, 1000}, {0, 0, 1000, 0}});
    const std::vector<Space> spaces = {square("park", 250, 5, 50)};
    const std::vector<Place> places = {{0, 300, 1}};
    Reading net{"along", {"park"}, 0, 400, true, ""}, line{"line", {"park"}, 0, 400, false, ""};
    const auto N = reach(walk, places, spaces, net, 1e6), L = reach(walk, places, spaces, line, 1e6);
    s.require("round a corner: about 555 m along the streets, about 350 m in a straight line, so within 400 m only in a straight line",
              N.share == 0 && L.share == 1 && N.distance[0] > 500 && L.distance[0] < 400);
  }
  s.require("a space of two points is refused", refused([] { Space{"park", {0, 0, 1, 1}, {}}.check(); }));
  s.require("an analysis area of nothing is refused", refused([] { reach(urban::transit::walking({{0, 0, 1, 0}}), {}, {}, Reading{"r", {"park"}, 0, 400, true, ""}, 0); }));
  return s.done();
}
