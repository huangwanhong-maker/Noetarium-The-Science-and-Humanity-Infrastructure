// Checks of fertility (fertility.hpp).
//
// - TFR is n Σ f, GRR its share of girls, and the mean age of the schedule its weighted mean.
// - When no one dies before the end of childbearing (L = n l_0), the net reproduction rate is the gross one.
// - With mortality, NRR is s Σ f L / l_0 and falls below GRR; the mean age of net maternity lies within the childbearing ages.

#include <demography/fertility/fertility.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace df = demography::fertility;
namespace lt = demography::lifetable;

int main() {
  calx::verification::Suite s;
  const std::vector<double> x{15, 20, 25, 30, 35, 40, 45};
  const std::vector<double> f{0.012, 0.065, 0.105, 0.092, 0.048, 0.012, 0.0015};
  const double girls = 0.4886;
  std::vector<double> ages, tiny, real;
  for (int a = 0; a <= 90; a += 5) {
    ages.push_back(a);
    tiny.push_back(a < 90 ? 1e-12 : 0.5);
    real.push_back(0.0004 + 0.00004 * std::exp(0.092 * (a + 2.5)) + (a == 0 ? 0.002 : 0.0));
  }
  const auto immortal = lt::life_table(ages, tiny), women = lt::life_table(ages, real);
  const auto r0 = df::reproduction(x, 5, f, girls, immortal);
  double tfr = 0, age = 0;
  for (std::size_t i = 0; i < f.size(); i++) tfr += 5 * f[i], age += (x[i] + 2.5) * 5 * f[i];
  s.check("TFR = n Σ f", r0.tfr - tfr, 1e-15);
  s.check("GRR = s TFR, and the schedule's mean age its weighted mean", std::fabs(r0.grr - girls * tfr) + std::fabs(r0.mean_age - age / tfr), 1e-12);
  s.check("with no deaths before the end of childbearing, NRR = GRR", r0.nrr - r0.grr, 1e-9);
  const auto r = df::reproduction(x, 5, f, girls, women);
  s.require("with mortality, NRR < GRR, and the mean age of net maternity lies within the childbearing ages",
            r.nrr < r.grr && r.mean_age_net > 15 && r.mean_age_net < 50, "NRR " + std::to_string(r.nrr) + ", GRR " + std::to_string(r.grr) + ", μ₁ " + std::to_string(r.mean_age_net));
  return s.done();
}
