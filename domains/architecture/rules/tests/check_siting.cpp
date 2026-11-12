// Checks of a dwelling's siting on its lot (siting.hpp), on a lot of 15 × 30 m facing a street on its south edge, worked by hand.
//
// - The setback that grows with height: 1 m to 3.6 m, 1.99 m at 6.9 m, 4.09 m at 9 m (Victoria's one-dwelling standard as a form).
// - The length a boundary may carry: 10 + 25% of the rest; the whole of a boundary shorter than 10 m.
// - The garden share by the lot's size.
// - A house of 12 × 14 m, walls 0.25 thick, 6 m back from the street to its face and 1.375 m from the sides: every rule met; one
//   storey more at 6 m high fails the side setback of 1.72 m, and only that.
// - What is covered: the floor to the centrelines plus half the thickness along the outer walls.
// - A wall within 0.2 m of a side boundary stands on it: 16 m of wall on a boundary that may carry 15 fails by its length, and is
//   left out of the setback.
// - A scheme that states nothing finds only whether the building is within the lot.

#include <architecture/rules/siting.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace ar = architecture::rules;

namespace {
ar::Level box(double x0, double y0, double x1, double y1, double top, double t = 0.25) {
  ar::Level L;
  L.walls = {{{x0, y0}, {x1, y0}}, {{x1, y0}, {x1, y1}}, {{x1, y1}, {x0, y1}}, {{x0, y1}, {x0, y0}}};
  L.thickness = t, L.top = top, L.floor = (x1 - x0) * (y1 - y0);
  return L;
}
const ar::Finding* find(const ar::Siting& S, const std::string& rule, const std::string& subject = "") {
  for (const auto& f : S.findings)
    if (f.rule == rule && f.subject.find(subject) != std::string::npos) return &f;
  return nullptr;
}
bool all_ok(const ar::Siting& S) {
  return std::all_of(S.findings.begin(), S.findings.end(), [](const ar::Finding& f) { return f.ok; });
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const ar::Lot lot{{{0, 0}, {15, 0}, {15, 30}, {0, 30}}, {1, 0, 0, 0}};

  ar::Scheme v;
  v.name = "a scheme in the form of Victoria's clause 54";
  v.height = ar::Limit::at_most(9), v.storeys = ar::Limit::at_most(2);
  v.street_setback = ar::Limit::at_least(6), v.side_setback = ar::Limit::at_least(1, "54.02-3");
  v.side_steps = {{3.6, 6.9, 0.3}, {6.9, std::numeric_limits<double>::infinity(), 1.0}};
  v.coverage = ar::Limit::at_most(0.6), v.open_share = ar::Limit::at_least(0.2), v.open_area = ar::Limit::at_least(25);
  v.boundary_base = 10, v.boundary_share = 0.25, v.boundary_height = ar::Limit::at_most(3.6);
  v.garden = {{400, 0.25}, {500, 0.30}, {650, 0.35}};

  s.check("to 3.6 m a wall stands 1 m from a side boundary", ar::side_setback(v, 3.0) - 1, 1e-15, "m");
  s.check("at 6.9 m, 1 + 0.3 × 3.3 = 1.99 m", ar::side_setback(v, 6.9) - 1.99, 1e-12, "m");
  s.check("at 9 m, 1.99 + 2.1 = 4.09 m", ar::side_setback(v, 9) - 4.09, 1e-12, "m");
  s.check("a boundary of 30 m may carry 10 + 25% of 20 = 15 m of wall", ar::boundary_allowance(v, 30) - 15, 1e-12, "m");
  s.check("... and one of 8 m, all of it", ar::boundary_allowance(v, 8) - 8, 1e-15, "m");
  s.require("garden by the lot's size: none at 300 m², 25% above 400, 30% above 500, 35% above 650",
            ar::garden_share(v, 300) == 0 && ar::garden_share(v, 450) == 0.25 && ar::garden_share(v, 500) == 0.25 && ar::garden_share(v, 501) == 0.30 && ar::garden_share(v, 700) == 0.35);

  // The house: its face 6 m from the street.
  const auto ground = box(1.5, 6.125, 13.5, 20.125, 3.0);
  {
    const auto S = ar::check(lot, {ground}, v);
    s.require("a house of 12 × 14 m, 6 m from the street and 1.375 m from the sides, meets every rule", all_ok(S) && S.findings.size() >= 7);
    s.check("its street setback is 6 m to the wall's face", find(S, "street-setback")->actual - 6, 1e-12, "m");
    s.check("its side setback 1.5 − 0.125 = 1.375 m, against 1 m", std::fabs(find(S, "side-setback")->actual - 1.375) + std::fabs(find(S, "side-setback")->least - 1), 1e-12, "m");
    s.check("it covers 168 m² of floor and 52 × 0.125 of wall: 174.5 m²", S.covered - 174.5, 1e-12, "m²");
    s.check("... 38.8% of the lot's 450 m²", find(S, "coverage")->actual - 174.5 / 450, 1e-15);
    s.check("the open space asked is the greater of 20% of the lot and 25 m²: 90 m²", find(S, "open-space")->least - 90, 1e-12, "m²");
    s.check("... and 275.5 m² is left", find(S, "open-space")->actual - 275.5, 1e-12, "m²");
    s.check("a lot of 450 m² keeps a quarter as garden", find(S, "garden")->least - 0.25, 1e-15);
    s.require("no wall stands on a boundary", std::all_of(S.on_boundary.begin(), S.on_boundary.end(), [](double x) { return x == 0; }) && !find(S, "boundary-length"));
  }
  {
    const auto S = ar::check(lot, {box(1.5, 6, 13.5, 20, 3.0)}, v);
    s.require("12.5 cm nearer the street it fails the street setback, and only that", !find(S, "street-setback")->ok && std::count_if(S.findings.begin(), S.findings.end(), [](const ar::Finding& f) { return !f.ok; }) == 1);
    s.check("... at 5.875 m", find(S, "street-setback")->actual - 5.875, 1e-12, "m");
  }
  {
    auto upper = ground;
    upper.top = 6.0;
    const auto S = ar::check(lot, {ground, upper}, v);
    const auto* f = find(S, "side-setback", "storey 2");
    s.require("a second storey over it, 6 m high, fails the side setback and nothing else", f && !f->ok && std::count_if(S.findings.begin(), S.findings.end(), [](const ar::Finding& x) { return !x.ok; }) == 1);
    s.check("... which at 6 m is 1 + 0.3 × 2.4 = 1.72 m", f->least - 1.72, 1e-12, "m");
    s.check("the building's height is 6 m", S.height - 6, 1e-15, "m");
  }
  {
    auto upper = box(3.5, 8, 11.5, 18, 6.0);
    const auto S = ar::check(lot, {ground, upper}, v);
    s.require("set in to 3.375 m from the sides, the second storey passes", all_ok(S));
    s.check("... its setback 3.5 − 0.125", find(S, "side-setback", "storey 2")->actual - 3.375, 1e-12, "m");
  }

  // A wall on the west boundary.
  {
    const auto S = ar::check(lot, {box(0.125, 8, 10, 24, 3.0)}, v);
    const auto* length = find(S, "boundary-length", "edge 4");
    const auto* height = find(S, "boundary-height", "edge 4");
    s.require("a wall whose face is on the west boundary stands on it", length && height);
    s.check("... 16 m of it, on a boundary that may carry 15", std::fabs(length->actual - 16) + std::fabs(length->most - 15), 1e-12, "m");
    s.require("... failing by its length, not by its height of 3 m", !length->ok && height->ok);
    s.check("... and left out of the setback, which is the east side's 15 − 10 − 0.125", find(S, "side-setback")->actual - 4.875, 1e-12, "m");
    s.check("the lot's edge 4 carries 16 m", S.on_boundary[3] - 16, 1e-12, "m");
  }

  // Beyond the lot, and a scheme that states nothing.
  {
    const auto S = ar::check(lot, {box(-1, 8, 10, 24, 3.0)}, ar::Scheme{});
    s.require("a scheme that states nothing finds only whether the building is within the lot", S.findings.size() == 1 && S.findings[0].rule == "lot");
    s.require("... and a house 1 m over the west boundary is not", !S.findings[0].ok && S.findings[0].actual == 4);
    s.require("... while the house within it is", ar::check(lot, {ground}, ar::Scheme{}).findings[0].ok);
  }
  return s.done();
}
