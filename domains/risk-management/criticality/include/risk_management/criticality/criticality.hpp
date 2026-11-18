// Criticality: which components of a system matter most to the service it gives, under the states a hazard puts it in
// (theory/risk-and-its-management.md §2).
//
// A SYSTEM has components 0..n−1, each in a state x_c ∈ [0, 1], the share of its capacity it keeps (1 intact, 0 failed). Its loss
// L(x) ≥ 0 is the service lost against the intact system (people beyond reach of a hospital, person-minutes added, demand unmet),
// a function the caller states. A HAZARD is a set of scenarios s, each a state x_s with a weight w_s: an annual rate, a probability,
// or a weight of integration over a curve of exceedance (Σ w_s = 1 when the scenarios with the undisturbed state among them span the
// year). The system's expected loss is
//
//     E L = Σ_s w_s L(x_s).
//
// Two questions are asked of each component c, by forcing it in every scenario:
//
//     failed:    E[L | c failed]   = Σ_s w_s L(x_s with x_c = 0)      what the system would lose without c;
//     hardened:  E[L | c hardened] = Σ_s w_s L(x_s with x_c = 1)      what it would lose were c never disturbed.
//
// From them the importance measures of probabilistic risk assessment (Birnbaum 1969, in Multivariate Analysis II, 581; Vesely et al.
// 1983, NUREG/CR-3385; Cheok, Parry and Sherry 1998, Reliability Engineering and System Safety 60, 213), read for a loss:
//
//     criticality  = E[L | c failed] − E L              what losing c would add (the consequence of its failure);
//     risk         = E L − E[L | c hardened]            what c's disturbance costs now (what protecting c would save a year);
//     Birnbaum     = E[L | c failed] − E[L | c hardened];
//     RAW          = E[L | c failed] / E L,   RRW = E L / E[L | c hardened],   Fussell–Vesely = risk / E L.
//
// Studies of the criticality of transport networks rank links by the first and choose what to protect by the second (Hallegatte,
// Rentschler and Rozenberg 2019, Lifelines: The Resilient Infrastructure Opportunity, World Bank). Each is linear in the weights, so a
// loss integrated over a curve of return periods is integrated alike.
//
// What is asked of L. Forcing c to failure where it is already failed, or to the intact state where it is intact, changes nothing,
// and is not asked. A system that can answer every "failed" of one state at once (sharing work, or knowing which components cannot
// matter there) gives `failed_all`; otherwise L is asked once a component and a scenario. "Hardened" is asked only for the components a
// scenario disturbs.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace risk_management::criticality {

using State = std::vector<double>;  // per component, the share of its capacity it keeps

struct Scenario {
  std::string name;
  double weight = 0;  // a rate, a probability or a weight of integration, ≥ 0
  State state;        // empty: the undisturbed system
};

using Loss = std::function<double(const State&)>;
// The loss at the state with each component in turn forced to 0, given the loss at the state itself.
using FailedAll = std::function<std::vector<double>(const State&, double)>;

struct Importance {
  std::size_t component = 0;
  double failed = 0, hardened = 0;     // E[L | c failed], E[L | c hardened]
  double criticality = 0, risk = 0, birnbaum = 0;
  double raw = 0, rrw = 0, fussell_vesely = 0;  // RAW and RRW are +inf where their denominators vanish and the numerator does not
  std::vector<double> by_scenario;     // L(x_s with x_c = 0) − L(x_s), the criticality within each scenario
};

struct Assessment {
  double expected = 0;                  // E L
  double weight = 0;                    // Σ w_s
  std::vector<double> scenario_loss;    // L(x_s)
  std::vector<Importance> components;   // by component
  std::size_t evaluations = 0;          // how often L was asked (failed_all counts once a scenario)

  // Components by a measure, greatest first.
  std::vector<std::size_t> ranked(double Importance::*measure) const {
    std::vector<std::size_t> order(components.size());
    for (std::size_t i = 0; i < order.size(); i++) order[i] = i;
    std::stable_sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b) { return components[a].*measure > components[b].*measure; });
    return order;
  }
};

inline Assessment assess(std::size_t n, const std::vector<Scenario>& scenarios, const Loss& loss, const FailedAll& failed_all = {}) {
  const char* who = "risk_management::criticality::assess";
  if (!loss) throw std::invalid_argument(std::string(who) + ": a loss");
  if (scenarios.empty()) throw std::invalid_argument(std::string(who) + ": at least one scenario");
  std::vector<State> states;
  for (const Scenario& s : scenarios) {
    if (!(s.weight >= 0 && std::isfinite(s.weight))) throw std::invalid_argument(std::string(who) + ": weights are finite and ≥ 0");
    State x = s.state.empty() ? State(n, 1.0) : s.state;
    if (x.size() != n) throw std::invalid_argument(std::string(who) + ": scenario '" + s.name + "' gives a state for each component");
    for (double v : x)
      if (!(v >= 0 && v <= 1)) throw std::invalid_argument(std::string(who) + ": a component keeps a share of its capacity in [0, 1]");
    states.push_back(std::move(x));
  }
  Assessment out;
  out.components.resize(n);
  for (std::size_t c = 0; c < n; c++) {
    out.components[c].component = c;
    out.components[c].by_scenario.assign(scenarios.size(), 0);
  }
  for (std::size_t k = 0; k < scenarios.size(); k++) {
    const double w = scenarios[k].weight;
    State x = states[k];
    const double Lk = loss(x);
    out.evaluations++;
    if (!(Lk >= 0 && std::isfinite(Lk))) throw std::invalid_argument(std::string(who) + ": a loss is finite and ≥ 0");
    out.scenario_loss.push_back(Lk);
    out.expected += w * Lk;
    out.weight += w;
    std::vector<double> F;
    if (failed_all) {
      F = failed_all(x, Lk);
      out.evaluations++;
      if (F.size() != n) throw std::invalid_argument(std::string(who) + ": failed_all gives a loss for each component");
    } else {
      F.assign(n, Lk);
      for (std::size_t c = 0; c < n; c++) {
        if (x[c] == 0) continue;
        const double keep = x[c];
        x[c] = 0;
        F[c] = loss(x);
        out.evaluations++;
        x[c] = keep;
      }
    }
    for (std::size_t c = 0; c < n; c++) {
      Importance& I = out.components[c];
      I.failed += w * F[c];
      I.by_scenario[k] = F[c] - Lk;
      double H = Lk;
      if (x[c] < 1) {
        const double keep = x[c];
        x[c] = 1;
        H = loss(x);
        out.evaluations++;
        x[c] = keep;
      }
      I.hardened += w * H;
    }
  }
  const double inf = std::numeric_limits<double>::infinity();
  const auto ratio = [inf](double a, double b) { return b > 0 ? a / b : (a > 0 ? inf : 1.0); };
  for (Importance& I : out.components) {
    I.criticality = I.failed - out.expected;
    I.risk = out.expected - I.hardened;
    I.birnbaum = I.failed - I.hardened;
    I.raw = ratio(I.failed, out.expected);
    I.rrw = ratio(out.expected, I.hardened);
    I.fussell_vesely = out.expected > 0 ? I.risk / out.expected : 0;
  }
  return out;
}

// The weights that integrate a loss over a curve of return periods T_1 < … < T_m (> 1) by trapezoids in the annual exceedance
// probability p = 1/T between the maps, the last held to p = 0 (tyche's curve_area): the first weight is the undisturbed state's,
// then one for each T; they sum to 1. Read so, a scenario's weight is the probability that the year's largest event falls in its band,
// and E L the expectation of the loss over the year's largest event. What happens below the first map is not mapped: by default
// nothing (the events more frequent than T_1 disturb nothing, a lower bound where they would), or, with `linear_below`, the loss
// falling linearly from the first map to nothing at p = 1 (curve_area's convention), which counts half the first map's state over
// every year more frequent than it.
inline std::vector<double> curve_weights(const std::vector<double>& return_periods, bool linear_below = false) {
  const char* who = "risk_management::criticality::curve_weights";
  if (return_periods.empty()) throw std::invalid_argument(std::string(who) + ": at least one return period");
  for (std::size_t k = 0; k < return_periods.size(); k++) {
    if (!(return_periods[k] > 1) || !std::isfinite(return_periods[k])) throw std::invalid_argument(std::string(who) + ": return periods exceed one year");
    if (k > 0 && !(return_periods[k] > return_periods[k - 1])) throw std::invalid_argument(std::string(who) + ": return periods ascend");
  }
  const std::size_t m = return_periods.size();
  std::vector<double> p(m + 1);
  p[0] = 1;
  for (std::size_t k = 0; k < m; k++) p[k + 1] = 1 / return_periods[k];
  std::vector<double> w(m + 1, 0);
  for (std::size_t k = linear_below ? 0 : 1; k < m; k++) {  // the trapezoid between point k and point k + 1
    w[k] += (p[k] - p[k + 1]) / 2;
    w[k + 1] += (p[k] - p[k + 1]) / 2;
  }
  w[m] += p[m];
  if (!linear_below) w[0] = 1 - p[1];
  return w;
}

}  // namespace risk_management::criticality
