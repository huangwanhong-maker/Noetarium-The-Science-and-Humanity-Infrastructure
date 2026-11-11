// Checks of the frontage (frontage.hpp) on a lot known by hand.
//
// - A 20 × 20 lot facing a street on its south edge, set back 2 from its sides: a podium on its envelope stands on 16 m of the 20 m
//   street line, a share of 0.8; the same with the street line set back 3; none for a podium set into the lot or of no height.
// - The active length is the wall's length times the share declared active; a lot given clockwise gives the same.

#include <architecture/frontage/frontage.hpp>
#include <architecture/form/form.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

namespace ar = architecture::rules;
namespace af = architecture::form;
namespace fr = architecture::frontage;

namespace {
ar::Ring box(double x0, double y0, double x1, double y1) { return {{x0, y0}, {x1, y0}, {x1, y1}, {x0, y1}}; }
}  // namespace

int main() {
  calx::verification::Suite s;
  ar::Lot lot{box(0, 0, 20, 20), {1, 0, 0, 0}};
  ar::Rules r;
  r.side_setback = 2, r.storey = 3;
  {
    const auto m = af::massing(lot, r, 3, 0, 0);
    const auto f = fr::frontage(m, lot, r, 0.5);
    s.check("a podium 2 m in from each side stands on 16 of the street's 20 m", f.share - 0.8, 1e-12);
    s.check("... its wall 9 m high", f.wall_height - 9, 1e-12);
    s.check("... and half of it active, 8 m", f.active_length - 8, 1e-12);
  }
  {
    ar::Rules back = r;
    back.street_setback = 3;
    const auto f = fr::frontage(af::massing(lot, back, 3, 0, 0), lot, back);
    s.check("with the street line set back 3 the podium on it still stands on 16 m", f.share - 0.8, 1e-12);
  }
  {
    ar::Massing set{af::scaled(ar::envelope(lot, r).ground, 0.7), 9, {}, 0};
    s.check("a podium set into the lot stands on none of it", fr::frontage(set, lot, r).share, 1e-15);
    ar::Massing none{ar::envelope(lot, r).ground, 0, {}, 0};
    s.check("... nor does one of no height", fr::frontage(none, lot, r).share, 1e-15);
  }
  {
    ar::Lot cw{{{0, 0}, {0, 20}, {20, 20}, {20, 0}}, {0, 0, 0, 1}};  // the same lot clockwise: its street edge is now edge 3
    const auto f = fr::frontage(af::massing(cw, r, 3, 0, 0), cw, r);
    s.check("the same lot given clockwise gives the same share", f.share - 0.8, 1e-12);
  }
  return s.done();
}
