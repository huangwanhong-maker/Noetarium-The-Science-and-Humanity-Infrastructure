// Checks of the life table (lifetable.hpp).
//
// - A constant hazard m, with the a exact for it, gives e_x = 1/m at every age: the table reproduces the exponential law.
// - The table's identities on a Gompertz–Makeham schedule: the deaths sum to the radix, T_0 = Σ L, e_0 = T_0 / l_0, l decreasing.
// - Arriaga's contributions of the age groups sum to the difference of the two tables' e_0 exactly.
// - Rates read from peira's Lexis diagram of lives simulated with a constant hazard of 0.03 recover it within four standard errors.
// - A death rate standardised from equal rates in every group is that rate.

#include <demography/lifetable/lifetable.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace lt = demography::lifetable;

namespace {
// Five-year groups 0, 5, …, 85 and an open group at 90; Gompertz–Makeham rates with a raised first group.
lt::LifeTable gompertz(double level) {
  std::vector<double> x, m;
  for (int a = 0; a <= 90; a += 5) {
    x.push_back(a);
    m.push_back(level * (0.0004 + 0.00004 * std::exp(0.092 * (a + 2.5))) + (a == 0 ? 0.002 : 0.0));
  }
  return lt::life_table(x, m);
}
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    const double m = 0.02;
    std::vector<double> x, rates, a;
    for (int age = 0; age <= 100; age++) x.push_back(age), rates.push_back(m), a.push_back(lt::constant_hazard_a(m, 1));
    const auto t = lt::life_table(x, rates, a);
    double worst = 0;
    for (double e : t.e) worst = std::max(worst, std::fabs(e - 1 / m));
    s.check("a constant hazard 0.02, with the a exact for it: e_x = 50 at every age", worst, 1e-9);
    s.check("... and q = 1 − e^{−m}", t.q[0] - (1 - std::exp(-m)), 1e-15);
  }
  {
    const auto t = gompertz(1);
    double deaths = 0, L = 0;
    bool falling = true;
    for (std::size_t i = 0; i < t.size(); i++) {
      deaths += t.d[i], L += t.L[i];
      if (i > 0) falling = falling && t.l[i] < t.l[i - 1];
    }
    s.check("Gompertz–Makeham: the deaths sum to the radix (relative)", deaths / t.radix - 1, 1e-14);
    s.check("... T_0 = Σ L and e_0 = T_0 / l_0", std::fabs(t.T[0] - L) / L + std::fabs(t.e[0] - t.T[0] / t.l[0]), 1e-14);
    s.require("... l falls at every age, and e_0 is a human one (60–90 years)", falling && t.e[0] > 60 && t.e[0] < 90, std::to_string(t.e[0]));
  }
  {
    const auto one = gompertz(1.3), two = gompertz(0.8);
    const auto c = lt::arriaga(one, two);
    double sum = 0;
    for (double v : c) sum += v;
    s.check("Arriaga: the age groups' contributions sum to e_0² − e_0¹ exactly", sum - (two.e[0] - one.e[0]), 1e-10);
    s.require("... each positive, mortality lower at every age", [&] { for (double v : c) if (!(v > 0)) return false; return true; }(),
              std::to_string(two.e[0] - one.e[0]) + " years");
  }
  {
    calx::random::Stream st(20261108, 0x6d6f7274u, 0, 0);  // "mort"
    const double mu = 0.03;
    std::vector<peira::cohorts::Life> lives;
    for (int k = 0; k < 20000; k++) {
      const double birth = st.uniform(1930, 1990), death = birth - std::log(st.uniform()) / mu;
      const double entry = std::max(birth, 2010.0), exit = std::min(death, 2020.0);
      if (exit <= entry) continue;
      lives.push_back({birth, entry, exit, death <= 2020.0});
    }
    peira::cohorts::Grid g;
    for (int a = 20; a <= 90; a += 10) g.age.push_back(a);
    g.period = {2010, 2020};
    const auto L = peira::cohorts::lexis(lives, g);
    const auto m = lt::death_rates(L, 0);
    double worst = 0;
    for (std::size_t a = 0; a < m.size(); a++) {
      const double se = std::sqrt(L.events[a][0]) / L.person_time[a][0];
      worst = std::max(worst, std::fabs(m[a] - mu) / se);
    }
    s.check("rates from peira's Lexis diagram of lives with a constant hazard 0.03 recover it (largest standard score)", worst, 4);
    const auto sdr = lt::standardised_death_rate({3, 9, 30}, {100, 300, 1000}, {0.3, 0.5, 0.2});
    s.check("a death rate standardised from equal rates (0.03) is that rate", sdr.value - 0.03, 1e-15);
  }
  return s.done();
}
