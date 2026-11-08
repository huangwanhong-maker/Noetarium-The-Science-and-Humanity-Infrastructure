// Checks of projection (projection.hpp).
//
// - The power method's λ solves the discrete characteristic equation, and its structure is the eigenvector: M w = λ w.
// - Strong ergodicity: a population all in one group, projected, takes the stable structure and grows by λ.
// - λ > 1 exactly when R₀ > 1, over fifty schedules of random level; at R₀ = 1, λ = 1.
// - The Leslie matrix's r and Lotka's continuous r agree to the order of n²; R₀ is the life table's NRR exactly.

#include <demography/fertility/fertility.hpp>
#include <demography/projection/projection.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace dp = demography::projection;
namespace lt = demography::lifetable;

int main() {
  calx::verification::Suite s;
  std::vector<double> ages, m;
  for (int a = 0; a <= 90; a += 5) ages.push_back(a), m.push_back(0.0004 + 0.00004 * std::exp(0.092 * (a + 2.5)) + (a == 0 ? 0.002 : 0.0));
  const auto women = lt::life_table(ages, m);
  const double girls = 0.4886;
  const std::size_t k = 11;  // groups 0–54
  std::vector<double> f(k, 0.0);
  const std::vector<double> schedule{0.018, 0.098, 0.158, 0.138, 0.072, 0.018, 0.0023};  // ages 15–49, TFR 2.5
  for (std::size_t i = 0; i < schedule.size(); i++) f[3 + i] = schedule[i];
  const auto M = dp::leslie(women, k, f, girls);
  const auto st = dp::stable(M);
  s.check("λ solves the characteristic equation Σ F_i Π s_j λ^{−(i+1)} = 1", dp::characteristic(M, st.lambda) - 1, 1e-10);
  const auto Mw = dp::project(M, st.structure);
  double eig = 0;
  for (std::size_t i = 0; i < k; i++) eig = std::max(eig, std::fabs(Mw[i] - st.lambda * st.structure[i]));
  s.check("... and the stable structure is its eigenvector, M w = λ w", eig, 1e-12);
  {
    std::vector<double> N(k, 0.0);
    N[5] = 1000;
    double growth = 0;
    for (int t = 0; t < 600; t++) {
      const auto next = dp::project(M, N);
      double a = 0, b = 0;
      for (std::size_t i = 0; i < k; i++) a += N[i], b += next[i];
      growth = b / a;
      N = next;
    }
    double total = 0, worst = 0;
    for (double v : N) total += v;
    for (std::size_t i = 0; i < k; i++) worst = std::max(worst, std::fabs(N[i] / total - st.structure[i]));
    s.check("ergodicity: a population all in one group takes the stable structure", worst, 1e-9);
    s.check("... and grows by λ each step", growth - st.lambda, 1e-10);
  }
  {
    calx::random::Stream rnd(20261108, 0x6c736c65u, 0, 0);  // "lsle"
    int agree = 0;
    for (int r = 0; r < 50; r++) {
      const double scale = rnd.uniform(0.3, 2.5);
      std::vector<double> g = f;
      for (auto& v : g) v *= scale;
      const auto S = dp::stable(dp::leslie(women, k, g, girls));
      agree += (S.lambda > 1) == (S.r0 > 1);
    }
    s.require("λ > 1 exactly when R₀ > 1, over fifty schedules of random level", agree == 50, std::to_string(agree));
    const double c = 1 / st.r0;
    std::vector<double> g = f;
    for (auto& v : g) v *= c;
    s.check("... and with fertility scaled to R₀ = 1, λ = 1", dp::stable(dp::leslie(women, k, g, girls)).lambda - 1, 1e-10);
  }
  {
    const std::vector<double> x{15, 20, 25, 30, 35, 40, 45};
    const double lotka = dp::lotka_r(women, x, 5, schedule, girls);
    s.check("Lotka's r and the Leslie matrix's agree to the order of n² (per year)", lotka - st.r, 1e-4);
    s.require("... both positive with this schedule (NRR above one)", lotka > 0 && st.r > 0, std::to_string(lotka) + " and " + std::to_string(st.r));
    const auto rep = demography::fertility::reproduction(x, 5, schedule, girls, women);
    s.check("R₀ of the Leslie matrix is the life table's NRR exactly (the schedule zero at the matrix's ends)", st.r0 - rep.nrr, 1e-14);
  }
  return s.done();
}
