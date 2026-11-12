// Checks of the sun and the sky at a pane (panes.hpp), worked by hand.
//
// A pane 1.8 m wide from z = 0.3 to 2.4 in a face that looks west (normal (−1, 0)), the middle of its foot at (0, 0, 0.3). Its nine
// points stand at heights 0.65, 1.35 and 2.05 and at −0.6, 0 and +0.6 along the face.
//
// - In the open, the Sun due west at 30°: all nine points are lit; the hours are the instant's; the energy is I(30°) cos 30°.
// - The Sun behind the face, or below the horizon: nothing.
// - A prism 10 m away (its near face at x = −10.05, 10 m from the points, which stand 0.05 m out) of height 7.5: the ray from a
//   point at height z clears it when z + 10 tan 30° > 7.5, that is z > 1.7265: only the top row, a third of the pane; the middle is
//   not lit. Of height 6: every point clears it.
// - An overhang 0.6 m out with its underside at 2.7: at 30° the ray from the top row reaches 2.7 after a run of (2.7 − 2.05) / tan 30°
//   = 1.126 m > 0.6, and passes outside; at 60° after 0.375 m < 0.6, so the top row is under it, and the rows at 1.35 (0.779 m) and
//   0.65 are not: two thirds.
// - The Sun at 60° from the south-west (azimuth 225°): the run square to the face is the run times cos 45°: the top row
//   0.375 × 0.7071 = 0.265 < 0.6 (under), the middle row 0.779 × 0.7071 = 0.551 < 0.6 (under), the foot row 1.184 × 0.7071 = 0.837 (lit).
// - The sky: the prism of 7.5 hides it to atan((7.5 − 1.35) / 10); the overhang beyond atan((2.7 − 1.35) / 0.6); the angle between.
// - The average daylight factor: T = 0.68, 3 m² of glass seeing 65°, 100 m² of surfaces, R = 0.5: 0.68 × 3 × 65 / (100 × 0.75).

#include <architecture/environment/panes.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>

namespace ae = architecture::environment;

int main() {
  calx::verification::Suite s;
  const double deg = std::numbers::pi / 180;
  ae::Pane pane;
  pane.x = 0, pane.y = 0, pane.z0 = 0.3, pane.z1 = 2.4, pane.width = 1.8, pane.nx = -1, pane.ny = 0;
  const ae::Sun west{30 * deg, 270 * deg, 1.0};
  const auto prism = [](double height) { return ae::Prism{{{-12, -20}, {-10.05, -20}, {-10.05, 20}, {-12, 20}}, height, -1}; };

  {
    const auto R = ae::panes({pane}, {west}, {});
    s.check("in the open, the Sun due west at 30°: an hour of sun on the whole pane", R[0].hours - 1, 1e-12, "h");
    s.check("... and on its middle", R[0].full_hours - 1, 1e-12, "h");
    s.check("... its energy I(30°) cos 30° for the hour", R[0].energy - ae::clear_sky(30 * deg) * std::cos(30 * deg), 1e-9, "Wh/m²");
    s.check("... the whole sky before it", R[0].sky - std::numbers::pi / 2, 1e-12, "rad");
    const auto B = ae::panes({pane}, {{30 * deg, 90 * deg, 1.0}, {-5 * deg, 270 * deg, 1.0}}, {});
    s.require("the Sun behind the face, or below the horizon, gives nothing", B[0].hours == 0 && B[0].energy == 0);
  }
  {
    const auto R = ae::panes({pane}, {west}, {prism(7.5)});
    s.check("a prism of 7.5 m at 10 m leaves the top row: a third of the pane", R[0].hours - 1.0 / 3, 1e-12, "h");
    s.check("... the middle is not lit", R[0].full_hours, 1e-12, "h");
    s.check("... and hides the sky to atan(6.15 / 10)", R[0].low - std::atan(0.615), 1e-9, "rad");
    s.check("a prism of 6 m at 10 m shades nothing", ae::panes({pane}, {west}, {prism(6)})[0].hours - 1, 1e-12, "h");
  }
  {
    ae::Pane under = pane;
    under.overhang = 0.6, under.overhang_z = 2.7;
    s.check("under an overhang of 0.6 m at 2.7, the Sun at 30° passes outside it", ae::panes({under}, {west}, {})[0].hours - 1, 1e-12, "h");
    s.check("... at 60° the top row is under it: two thirds", ae::panes({under}, {{60 * deg, 270 * deg, 1.0}}, {})[0].hours - 2.0 / 3, 1e-12, "h");
    const auto R = ae::panes({under}, {{60 * deg, 225 * deg, 1.0}}, {prism(7.5)});
    s.check("... at 60° from the south-west only the foot row is lit: a third", R[0].hours - 1.0 / 3, 1e-12, "h");
    s.check("... its energy I(60°) cos 60° cos 45° over a third", R[0].energy - ae::clear_sky(60 * deg) * std::cos(60 * deg) * std::cos(45 * deg) / 3, 1e-9, "Wh/m²");
    s.check("the overhang hides the sky beyond atan(1.35 / 0.6)", R[0].high - std::atan(1.35 / 0.6), 1e-12, "rad");
    s.check("... and the angle of visible sky is what lies between", R[0].sky - (std::atan(1.35 / 0.6) - std::atan(0.615)), 1e-9, "rad");
  }
  s.check("the average daylight factor of 3 m² of glass seeing 65° in a room of 100 m² of surfaces", ae::daylight_factor(3 * 65, 100, 0.68, 0.5) - 0.68 * 3 * 65 / 75, 1e-12, "%");
  s.require("... is nothing in a room without surfaces", ae::daylight_factor(3 * 65, 0) == 0);
  return s.done();
}
