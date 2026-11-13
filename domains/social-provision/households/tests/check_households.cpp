// Checks of social_provision/households: the OECD-modified, original and square-root scales on households counted by hand; a household
// of children only; equivalised income; the value of unpaid work by the three methods by hand, an activity without a specialist wage at
// the generalist's; full income; time poverty against a line of 1.5 medians and a declared line, its rate and gap by hand.

#include <social_provision/households/households.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace social_provision::households;

int main() {
  calx::verification::Suite s;
  {
    const std::vector<double> family{41, 39, 12, 8};  // two adults, two children under 14
    s.check("OECD-modified: 1 + 0.5 + 0.3 + 0.3 = 2.1", equivalent_adults(oecd_modified(), family) - 2.1, 1e-15);
    s.check("OECD original: 1 + 0.7 + 0.5 + 0.5 = 2.7", equivalent_adults(oecd_original(), family) - 2.7, 1e-15);
    s.check("square root: √4 = 2", equivalent_adults(square_root(), family) - 2, 1e-15);
    s.check("a single adult counts as one", equivalent_adults(oecd_modified(), {30}) - 1, 1e-15);
    s.check("a teenager of 14 counts as an adult", equivalent_adults(oecd_modified(), {40, 14}) - 1.5, 1e-15);
    s.check("children only: the eldest as the first adult", equivalent_adults(oecd_modified(), {13, 9}) - 1.3, 1e-15);
    s.check("equivalised income: 42,000 over 2.1", equivalised(42000, oecd_modified(), family) - 20000, 1e-9);
    bool refused = false;
    try { (void)equivalent_adults(oecd_modified(), {}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("an empty household is refused", refused);
  }
  {
    // 60 minutes of cooking and 30 of childcare a day; cooks paid 20, childcare 15, a housekeeper 12, the person's own wage 25.
    const auto v = value_unpaid({{"cooking", 60}, {"childcare", 30}, {"gardening", 12}}, {{"cooking", 20}, {"childcare", 15}}, 12, 25);
    const double h = 365.0 / 60.0;
    s.check("unpaid work: hours a year", v.hours_a_year - 102 * h, 1e-9);
    s.check("unpaid work: generalist replacement cost", v.generalist - 102 * h * 12, 1e-9);
    s.check("unpaid work: specialist cost, gardening at the generalist's wage", v.specialist - h * (60 * 20 + 30 * 15 + 12 * 12), 1e-9);
    s.check("unpaid work: opportunity cost", v.opportunity - 102 * h * 25, 1e-9);
    const auto f = full_income(30000, v.specialist);
    s.check("full income: money plus unpaid", f.full - (30000 + v.specialist), 1e-9);
  }
  {
    // Committed minutes of five people, equally weighted: median 480, line 720.
    const auto t = time_poverty({300, 420, 480, 600, 800});
    s.check("time poverty: the median", t.median - 480, 1e-12);
    s.check("time poverty: the line 1.5 × median", t.line - 720, 1e-12);
    s.check("time poverty: one in five above it", t.rate - 0.2, 1e-15);
    s.check("time poverty: the gap among the poor, 80 minutes", t.gap - 80, 1e-12);
    const auto d = time_poverty({300, 420, 480, 600, 800}, {}, 1.5, 500);
    s.check("time poverty: a declared line of 500 minutes", d.rate - 0.4, 1e-15);
  }
  return s.done();
}
