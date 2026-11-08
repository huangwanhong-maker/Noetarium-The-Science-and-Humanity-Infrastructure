// Fertility and reproduction (Preston, Heuveline and Guillot 2001, ch. 5; Keyfitz and Caswell 2005, ch. 6). The domain's theory note,
// §3.
//
// From age-specific fertility rates f_x (births per woman-year) in groups [x, x + n):
// - the total fertility rate TFR = Σ n f_x, the births a woman would have at these rates surviving to the end of childbearing;
// - the gross reproduction rate GRR = s TFR, its daughters, s the share of births that are girls (about 0.4886);
// - the net reproduction rate NRR = s Σ f_x L_x / l_0 with the women's life table: the daughters a newborn girl will bear at these
//   rates of birth and death, the factor by which a generation replaces itself;
// - the mean age at childbearing of the schedule, Σ (x + n/2) n f_x / TFR, and of the net maternity function, μ₁ = Σ (x + n/2) f_x L_x
//   / Σ f_x L_x, nearly the mean length of a generation.

#pragma once

#include <demography/lifetable/lifetable.hpp>

#include <cmath>
#include <cstddef>
#include <vector>

namespace demography::fertility {

struct Reproduction {
  double tfr = 0, grr = 0, nrr = 0;
  double mean_age = 0;       // of the schedule
  double mean_age_net = 0;   // of the net maternity function
};

// Rates f for the age groups starting at x (each of width n), the share s of births that are girls, and the women's life table
// (whose groups must include those of x, of the same widths).
inline Reproduction reproduction(const std::vector<double>& x, double n, const std::vector<double>& f, double girls, const lifetable::LifeTable& women) {
  lifetable::detail::require(x.size() == f.size() && !x.empty() && n > 0 && girls > 0 && girls < 1, "a rate for each group, n > 0, a share of girls in (0, 1)");
  Reproduction r;
  double net = 0, net_age = 0, age = 0;
  for (std::size_t i = 0; i < x.size(); i++) {
    std::size_t j = 0;
    while (j < women.size() && women.x[j] != x[i]) j++;
    lifetable::detail::require(j + 1 < women.size() && women.n[j] == n, "the women's table has each group, of the same width");
    r.tfr += n * f[i];
    age += (x[i] + n / 2) * n * f[i];
    net += f[i] * women.L[j] / women.l[0];
    net_age += (x[i] + n / 2) * f[i] * women.L[j];
  }
  r.grr = girls * r.tfr;
  r.nrr = girls * net;
  r.mean_age = r.tfr > 0 ? age / r.tfr : 0;
  r.mean_age_net = net > 0 ? net_age / (net * women.l[0]) : 0;
  return r;
}

}  // namespace demography::fertility
