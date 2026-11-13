// Checks of demography/health: Sullivan's health expectancy against its sum by hand; no prevalence giving the life expectancy and full
// prevalence none; prevalence carried from bands to groups; the sampling variance by hand; a life course of one state reproducing the
// geometric expectation n(1/(1 − p) − ½); a dependent start under constant persistence; and Sullivan equal to the chain in a stationary
// population, the prevalence read from the chain's own person-years.

#include <demography/health/health.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace demography::health;

namespace {

// A table whose survivors and person-years are given, n wide groups, the last closed (everyone dies within it).
LifeTable table_of(const std::vector<double>& l, double n) {
  LifeTable t;
  const std::size_t k = l.size();
  t.radix = l[0];
  for (std::size_t i = 0; i < k; i++) {
    t.x.push_back(n * static_cast<double>(i));
    t.n.push_back(n);
    t.l.push_back(l[i]);
    const double next = i + 1 < k ? l[i + 1] : 0.0;
    t.L.push_back(n * (l[i] + next) / 2);
  }
  t.T.assign(k, 0), t.e.assign(k, 0);
  double T = 0;
  for (std::size_t i = k; i-- > 0;) T += t.L[i], t.T[i] = T, t.e[i] = l[i] > 0 ? T / l[i] : 0;
  return t;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Sullivan ---------------------------------------------------------------------------------------------------
  {
    const auto t = table_of({100000, 98000, 90000, 60000}, 20);
    const std::vector<double> pi{0.02, 0.10, 0.30, 0.60};
    const auto r = sullivan(t, pi);
    double h = 0;
    for (std::size_t i = 0; i < 4; i++) h += (1 - pi[i]) * t.L[i];
    s.check("Sullivan: HLE₀ = Σ (1 − π) L / l₀", r.hle[0] - h / t.l[0], 1e-12);
    s.check("Sullivan: DLE = LE − HLE", r.dle[1] - (r.le[1] - r.hle[1]), 1e-12);
    s.check("Sullivan: no prevalence gives the life expectancy", sullivan(t, {0, 0, 0, 0}).hle[0] - t.e[0], 1e-12);
    s.check("Sullivan: full prevalence gives no healthy years", sullivan(t, {1, 1, 1, 1}).hle[0], 1e-12);
    const auto v = sullivan(t, pi, {500, 400, 300, 200});
    double var = 0;
    for (std::size_t i = 2; i < 4; i++) var += t.L[i] * t.L[i] * pi[i] * (1 - pi[i]) / std::vector<double>{500, 400, 300, 200}[i];
    s.check("Sullivan: the sampling standard error at 40 by hand", v.se[2] - std::sqrt(var) / t.l[2], 1e-12);
    const auto p = prevalence_at(t, {20, 50}, {0.1, 0.4});
    s.require("prevalence: groups at 0, 20, 40, 60 take none, 0.1, 0.1, 0.4", p[0] == 0 && p[1] == 0.1 && p[2] == 0.1 && p[3] == 0.4);
    bool refused = false;
    try { (void)sullivan(t, {0.1, 1.2, 0, 0}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("Sullivan: a prevalence above one is refused", refused);
  }

  // ---- a life course of transitions ------------------------------------------------------------------------------------
  {
    // One state in effect: kept healthy with probability p at every age, the last age open.
    const double p = 0.9, n = 5;
    std::vector<Step> one(12, Step{p, 0, 0, 0});
    const auto lc = life_course(one, n);
    s.check("life course: one state gives n (1/(1 − p) − ½)", lc.healthy[0] - n * (1 / (1 - p) - 0.5), 1e-9);
    s.check("life course: and no dependent years", lc.dependent[0], 1e-12);
    // A dependent start that stays dependent with q and never recovers.
    const double q = 0.8;
    std::vector<Step> dep(10, Step{0.95, 0.02, 0.0, q});
    const auto ld = life_course(dep, 1);
    s.check("life course: dependent persisting with q gives 1/(1 − q) − ½", ld.dependent_from_dependent[0] - (1 / (1 - q) - 0.5), 1e-9);
    s.check("life course: no healthy years after a dependent start without recovery", ld.healthy_from_dependent[0], 1e-12);
  }

  // ---- Sullivan and the chain agree in a stationary population -----------------------------------------------------------
  {
    const double n = 5;
    std::vector<Step> st;
    for (int y = 0; y < 20; y++) {
      const double age = n * y, die = std::min(0.6, 0.002 * std::exp(0.075 * age));
      const double inc = std::min(0.5, 0.004 * std::exp(0.06 * age)), rec = 0.15;
      Step k;
      k.hh = (1 - die) * (1 - inc), k.hd = (1 - die) * inc;
      const double dmort = std::min(0.95, 1.5 * die);
      k.dh = (1 - dmort) * rec, k.dd = (1 - dmort) * (1 - rec);
      st.push_back(k);
    }
    st.back() = Step{0, 0, 0, 0};  // the last step closes: all die within it, so the chain and a closed table agree
    const auto lc = life_course(st, n);
    std::vector<double> l, pi;
    for (std::size_t y = 0; y < st.size(); y++) l.push_back(1e5 * (lc.alive_healthy[y] + lc.alive_dependent[y]));
    const auto t = table_of(l, n);
    for (std::size_t y = 0; y < st.size(); y++) {
      const double D = n * (lc.alive_dependent[y] + lc.alive_dependent[y + 1]) / 2, A = t.L[y] / 1e5;
      pi.push_back(A > 0 ? D / A : 0);
    }
    const auto r = sullivan(t, pi);
    s.check("stationary: Sullivan's HLE₀ equals the chain's healthy years", r.hle[0] - lc.healthy[0], 1e-9);
    s.check("stationary: and LE − HLE its dependent years", r.dle[0] - lc.dependent[0], 1e-9);
  }
  return s.done();
}
