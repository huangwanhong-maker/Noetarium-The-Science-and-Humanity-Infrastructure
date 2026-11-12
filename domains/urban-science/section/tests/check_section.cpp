// Checks of a street's section along its length (section.hpp), worked by hand.
//
// A street 100 m long running east along y = 0. On its left (the north) a building on [0, 100] × [10, 30], 20 m high: a face 10 m
// from the centreline all along. On its right (the south) one on [0, 60] × [−22, −8], 8 m high: a face 8 m away for the first 60 m,
// and open ground beyond. Stations every 10 m: ten, at 5, 15, … 95; six of them have both faces.
//
// - Where both stand the width is 18 m, the enclosure (20 + 8) / 2 / 18, the sky (cos atan(20 / 10) + cos atan(8 / 8)) / 2; where
//   the south is open the sky is (cos atan 2 + 1) / 2 and no width is measured.
// - The left side: its line at 10 m, built and aligned all along, 20 m high. The right: at 8 m, built and aligned for 0.6.
// - Strips, from the left: a footpath 3, a verge 1.5, two lanes of 3.5, parking 2.5, a footpath 3: 17 m, which fits in 18; for
//   people 6 / 17, green 1.5 / 17, vehicles 9.5 / 17. The first lies from −8.5 to −5.5: the footpaths' middles are 7 m out on
//   either side.
// - A scheme asking footpaths of 2 m at least, lanes of 3.0 to 3.3 m, 40% for people and an enclosure of 0.5 to 1: the lanes fail
//   twice and the share for people fails; the rest holds.
// - The Sun due south for an hour at 30° and an hour at 25° (environment's check_solar has the arithmetic): the north footpath has
//   one hour beside the south building and two beyond it, 1.4 on average; the south footpath none beside it and two beyond, 0.8.
//   Asked for one hour, all of the north side has it and 0.4 of the south.
// - To keep an hour where there is one, the south building may rise to 15 tan 30° = 8.660 m; the north one shades neither footpath.
// - Twenty-eight metres of strips do not fit, and the street says so.

#include <urban/section/section.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>

namespace us = urban::section;
namespace ae = architecture::environment;
namespace ar = architecture::rules;

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180;
  const std::vector<us::Point> way = {{0, 0}, {40, 0}, {100, 0}};
  const std::vector<us::Building> buildings = {{{{0, 10}, {100, 10}, {100, 30}, {0, 30}}, 20}, {{{0, -22}, {60, -22}, {60, -8}, {0, -8}}, 8}};
  const std::vector<us::Strip> strips = {{"footpath", 3}, {"verge", 1.5}, {"lane", 3.5}, {"lane", 3.5}, {"parking", 2.5}, {"footpath", 3}};
  const std::vector<ae::Sun> suns = {{30 * deg, 180 * deg, 1.0}, {25 * deg, 180 * deg, 1.0}};
  us::Scheme scheme;
  scheme.width["footpath"] = ar::Limit::at_least(2.0, "a");
  scheme.width["lane"] = ar::Limit::between(3.0, 3.3, "b");
  scheme.people = ar::Limit::at_least(0.4), scheme.enclosure = ar::Limit::between(0.5, 1.0);

  const us::Reading R = us::read(way, buildings, strips, suns, scheme, 1.0, 10.0);
  s.require("a street of 100 m has ten stations 10 m apart, the first at 5 m", R.stations.size() == 10 && std::fabs(R.stations[0].at - 5) < 1e-12 && std::fabs(R.length - 100) < 1e-12);
  const auto& both = R.stations[2];
  const auto& open = R.stations[8];
  s.check("its left is the north: a face 10 m away, 20 m high", std::fabs(both.left[1] - 1) + std::fabs(both.face[0].distance - 10) + std::fabs(both.face[0].height - 20), 1e-12, "");
  s.check("where both sides stand the width is 18 m", both.width - 18, 1e-12, "m");
  s.check("... the enclosure (20 + 8) / 2 / 18", both.enclosure - 14.0 / 18, 1e-12, "");
  s.check("... the sky (cos atan 2 + cos 45°) / 2", both.sky - (std::cos(std::atan(2.0)) + std::cos(45 * deg)) / 2, 1e-12, "");
  s.require("where the south is open no width is measured", std::isinf(open.width) && std::isinf(open.face[1].distance) && open.face[1].building == us::none);
  s.check("... and the sky is (cos atan 2 + 1) / 2", open.sky - (std::cos(std::atan(2.0)) + 1) / 2, 1e-12, "");
  s.check("the street's least and median widths are 18 m", std::fabs(R.width_least - 18) + std::fabs(R.width_median - 18), 1e-12, "m");
  s.check("its enclosure is that of the stations with two faces", R.enclosure - 14.0 / 18, 1e-12, "");
  s.check("its sky the mean of all ten", R.sky - (6 * (std::cos(std::atan(2.0)) + std::cos(45 * deg)) / 2 + 4 * (std::cos(std::atan(2.0)) + 1) / 2) / 10, 1e-12, "");
  s.check("the left side: a line at 10 m, built and aligned all along, 20 m high", std::fabs(R.side[0].line - 10) + std::fabs(R.side[0].built - 1) + std::fabs(R.side[0].aligned - 1) + std::fabs(R.side[0].height - 20), 1e-12, "");
  s.check("the right side: a line at 8 m, built and aligned for 0.6, 8 m high", std::fabs(R.side[1].line - 8) + std::fabs(R.side[1].built - 0.6) + std::fabs(R.side[1].aligned - 0.6) + std::fabs(R.side[1].height - 8), 1e-12, "");

  s.check("the strips are 17 m across, the first from −8.5 to −5.5", std::fabs(R.total - 17) + std::fabs(R.strips[0].from + 8.5) + std::fabs(R.strips[0].to + 5.5), 1e-12, "m");
  s.require("... which fits between the faces", R.fits);
  s.check("... 6 / 17 for people, 1.5 / 17 green, 9.5 / 17 for vehicles", std::fabs(R.share[0] - 6.0 / 17) + std::fabs(R.share[2] - 1.5 / 17) + std::fabs(R.share[4] - 9.5 / 17) + R.share[1] + R.share[3], 1e-12, "");
  s.check("the footpaths' points stand 7 m out on either side", std::fabs(both.foot[0][1] - 7) + std::fabs(both.foot[1][1] + 7) + std::fabs(both.foot[0][0] - 25), 1e-12, "m");
  std::size_t failed = 0, lanes = 0, found = R.findings.size();
  for (const auto& f : R.findings) {
    failed += f.ok ? 0 : 1;
    lanes += (f.rule == "width.lane" && !f.ok) ? 1 : 0;
  }
  s.require("the scheme finds seven things: four widths, the fit, the share for people, the enclosure", found == 7);
  s.require("... of which the two lanes and the share for people fail", failed == 3 && lanes == 2);

  s.require("the Sun was given", R.lit);
  s.check("the north footpath has one hour beside the south building and two beyond: 1.4 on average", std::fabs(both.sun[0] - 1) + std::fabs(open.sun[0] - 2) + std::fabs(R.side[0].sun - 1.4), 1e-12, "h");
  s.check("the south footpath none beside it and two beyond: 0.8", both.sun[1] + std::fabs(open.sun[1] - 2) + std::fabs(R.side[1].sun - 0.8), 1e-12, "h");
  s.check("asked for one hour, all of the north side has it and 0.4 of the south", std::fabs(R.side[0].sunny - 1) + std::fabs(R.side[1].sunny - 0.4), 1e-12, "");
  s.require("two buildings stand on the street", R.rises.size() == 2 && R.rises[0].building == 0 && R.rises[1].building == 1);
  s.check("the south one, 8 m high, may rise to 15 tan 30° and keep the north footpath's hour", R.rises[1].limit - 15 * std::tan(30 * deg), 1e-5, "m");
  s.check("... as it stands it takes an hour from six points of the north footpath and two from six of the south: 18 over 20 points", R.rises[1].takes - 18.0 / 20, 1e-12, "h");
  s.require("... the north one shades neither footpath", std::isinf(R.rises[0].limit) && R.rises[0].reached == 0);

  {
    const us::Reading plain = us::read(way, buildings);
    s.require("with no strips and no Sun the street is still measured", plain.strips.empty() && !plain.lit && plain.findings.empty() && plain.rises.size() == 2 && plain.stations.size() == 20);
    s.check("... its footpath points one metre out from the faces", std::fabs(plain.stations[0].foot[0][1] - 9) + std::fabs(plain.stations[0].foot[1][1] + 7), 1e-12, "m");
    std::vector<us::Strip> wide = strips;
    wide.push_back({"tram", 11});
    const us::Reading W = us::read(way, buildings, wide);
    s.require("twenty-eight metres of strips do not fit in eighteen, and the street says so", !W.fits && !W.findings.empty() && !W.findings.back().ok && W.findings.back().rule == "fits");
    // A building given storey by storey: two prisms on one footprint, 3 m and 6 m. Its face is 6 m high.
    std::vector<us::Building> stacked = buildings;
    stacked.push_back({{{0, -22}, {60, -22}, {60, -8}, {0, -8}}, 11});
    const us::Reading T = us::read(way, stacked);
    s.require("of two prisms met at one distance, a building's storeys, the higher is the face", T.stations[0].face[1].height == 11 && T.stations[0].face[1].building == 2);
    bool refused = false;
    try { us::read(way, buildings, {{"moat", 3}}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("a strip of no known kind is refused", refused);
  }
  return s.done();
}
