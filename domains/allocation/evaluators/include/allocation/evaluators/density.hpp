// An allocation evaluated under a declared law of the holdings of each recipient's units, a density, coupled by a mean
// field (ADR 0139).
//
// Each recipient is a population of units (households, say), whose holdings form a density on a grid moved by kinesis's
// generator: Bouchaud and Mézard's law, drift J(m̄(t) − w) toward the mean holding of all the recipients' units together and
// diffusion σ²w² (2000, Physica A 282, 536–545), so that what one recipient is given raises the common mean, which pulls
// every other recipient's units. A recipient's grant first fills the gap of its units holding less than a line, each in
// proportion to its own gap (so that, short of filling it, every such unit closes the same share of its distance to the
// line), and what remains after the gap is filled is shared by all its units equally; each is a transfer of the density
// that keeps mass and adds exactly the grant, reported should any holding be carried past the grid's end. Grants arrive
// in the allocation's order, `between` steps apart, and the law
// runs on for `after` steps. Since a transfer to those below a line depends on the density at the moment it is made, and
// the mean field on everything given before, two orders of the same grants end differently: the exact effect of order
// under the declared law (kinesis's densities, generator.hpp), which says what the law implies, not that anything
// behaves so.
//
// The reading: the share of all units holding less than the line at the end (minimised), and per recipient the share of
// its units at or above it (for the comparison under principles); the mean holding as a report.
#pragma once

#include <allocation/evolution/problem.hpp>
#include <allocation/evolution/search.hpp>

#include <kinesis/densities/generator.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace allocation::evaluators {

namespace ae = allocation::evolution;
namespace kd = kinesis::densities;
namespace ha = heuresis::assessment;

struct DensitySpec {
  std::size_t resource = 0;
  double unit = 1;                     // holding granted per unit of the resource's amount
  std::vector<double> population;      // units of each recipient
  std::vector<double> mean;            // each recipient's mean holding at the start (its density Bouchaud and Mézard's stationary law, scaled)
  kd::Grid grid{0, 12, 241};           // holdings
  double pull = 1, spread = 1.0 / 3;   // J and σ²
  double line = 0.5;                   // grants go to those below it; the reading counts them
  double dt = 0.05;
  std::size_t between = 10, after = 40;
};

namespace detail {
// The stationary law of drift J(m − w) and diffusion σ²w²: the inverse gamma of shape 1 + J/σ² and scale m J/σ², at the nodes.
inline std::vector<double> start(const DensitySpec& s, double m) {
  const double a = 1 + s.pull / s.spread, b = m * s.pull / s.spread;
  std::vector<double> g(s.grid.cells, 0);
  double sum = 0;
  for (std::size_t i = 0; i < s.grid.cells; i++) {
    const double w = s.grid.at(i);
    g[i] = w > 0 ? std::exp(a * std::log(b) - std::lgamma(a) - (a + 1) * std::log(w) - b / w) : 0;
    sum += g[i];
  }
  for (double& v : g) v /= sum;
  return g;
}
}  // namespace detail

struct DensityReading {
  ha::Assessment assessment;
  std::vector<std::vector<double>> densities;  // each recipient's at the end
  double below = 0, mean = 0;
  double start = 0;   // the mean holding at the start, as the grid holds it
  double beyond = 0;  // units carried past the grid's end by a grant (its holding then cut to the end)
};

inline DensityReading read_density(const DensitySpec& s, const ae::Allocation& a) {
  const std::size_t n = s.population.size();
  if (n == 0 || s.mean.size() != n) throw ae::Refusal("a population and a mean holding for each recipient");
  if (s.resource >= a.amount.size() || a.amount[s.resource].size() != n) throw ae::Refusal("the densities are not the allocation's recipients");
  double pop = 0;
  for (double p : s.population) {
    if (!(p > 0)) throw ae::Refusal("every recipient has units");
    pop += p;
  }
  std::vector<std::vector<double>> g(n);
  for (std::size_t i = 0; i < n; i++) g[i] = detail::start(s, s.mean[i]);
  auto law = [&](double mbar) {
    std::vector<std::vector<double>> mu(1, std::vector<double>(s.grid.cells)), D(1, std::vector<double>(s.grid.cells));
    for (std::size_t i = 0; i < s.grid.cells; i++) {
      const double w = s.grid.at(i);
      mu[0][i] = s.pull * (mbar - w), D[0][i] = s.spread * w * w;
    }
    return kd::generator(s.grid, mu, D);
  };
  const auto G = law(1);  // the grid's readings (they do not depend on the law)
  auto common_mean = [&]() {
    double m = 0;
    for (std::size_t i = 0; i < n; i++) m += s.population[i] * kd::mean(G, g[i]);
    return m / pop;
  };
  auto flow = [&](std::size_t steps) {
    for (std::size_t k = 0; k < steps; k++) {
      const auto F = kd::flow(law(common_mean()), s.dt);  // the mean field held over a step
      for (auto& d : g) d = F.step(d);
    }
  };
  const double start_mean = common_mean();
  double beyond_ = 0;
  std::vector<char> seen(n, 0);
  std::vector<std::size_t> order;
  for (const std::size_t i : a.granted)
    if (i < n && !seen[i]) seen[i] = 1, order.push_back(i);
  for (std::size_t i = 0; i < n; i++)
    if (!seen[i]) order.push_back(i);
  for (std::size_t k = 0; k < order.size(); k++) {
    const std::size_t i = order[k];
    const double amount = a.amount[s.resource][i] * s.unit;
    if (amount > 0) {
      // The gap below the line, in holding: population × Σ_{w < line} (line − w) g(w).
      double gap = 0;
      for (std::size_t c = 0; c < s.grid.cells && s.grid.at(c) < s.line; c++) gap += (s.line - s.grid.at(c)) * g[i][c];
      gap *= s.population[i];
      const double line = s.line;
      if (gap > 0) {
        const double f = std::min(1.0, amount / gap);
        const auto m = kd::carry(G, g[i], [f, line](double w, std::size_t) { return w < line ? w + f * (line - w) : w; });
        g[i] = m.density, beyond_ += m.beyond * s.population[i];
      }
      const double rest = std::max(0.0, amount - gap) / s.population[i];
      if (rest > 0) {
        const auto m = kd::carry(G, g[i], [rest](double w, std::size_t) { return w + rest; });
        g[i] = m.density, beyond_ += m.beyond * s.population[i];
      }
    }
    flow(k + 1 < order.size() ? s.between : s.after);
  }
  DensityReading R;
  R.densities = g;
  R.start = start_mean;
  R.beyond = beyond_;
  std::vector<double> above(n);
  for (std::size_t i = 0; i < n; i++) {
    const double b = kd::below(G, g[i], s.line);
    above[i] = 1 - b;
    R.below += s.population[i] * b / pop;
    R.mean += s.population[i] * kd::mean(G, g[i]) / pop;
  }
  ha::Metric obj;
  obj.name = "share holding less than the line", obj.role = ha::Role::objective, obj.value = R.below, obj.uncertainty = 0, obj.units = above, obj.standing = ha::Standing::computed;
  R.assessment.metrics.push_back(obj);
  ha::Metric m;
  m.name = "mean holding", m.role = ha::Role::report, m.value = R.mean, m.uncertainty = 0, m.standing = ha::Standing::computed;
  R.assessment.metrics.push_back(m);
  ha::Metric cut = m;
  cut.name = "units carried past the grid's end", cut.value = R.beyond;
  R.assessment.metrics.push_back(cut);
  return R;
}

inline ha::Evaluator density_evaluator(std::shared_ptr<const ae::Composition> c, DensitySpec spec, std::string name = "a law of holdings with a mean field") {
  if (spec.population.size() != c->recipients()) throw ae::Refusal("a population for each recipient");
  return ae::evaluator(c, std::move(name), ha::Kind::simulation, ha::Standing::computed,
                       [spec](const ae::Allocation& a, calx::random::Stream&) { return read_density(spec, a).assessment; }, false, 3, "1");
}

}  // namespace allocation::evaluators
