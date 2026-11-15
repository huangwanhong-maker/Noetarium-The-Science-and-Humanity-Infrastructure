// Options appraised: what each intervention would be worth here, with the uncertainty of its effect, and what it would be worth to
// know which is best before choosing.
//
// An option is an intervention with the effect expected here (policy::transfer) per unit of the population it reaches each year,
// with its standard error; the worth of a unit of the outcome; the population; and its costs year by year. Its net present value is
// appraised by nomos (a schedule of discount rates declared; benefits from a stated year to the end of the cost stream), and since
// the benefits are linear in the effect, the net present value is normal when the effect is: mean A v N μ − C and standard
// deviation |A v N| σ, A the sum of the discount factors of the years with benefits, C the present value of the costs (the theory
// note, Proposition 4). The worth of a unit at which the option breaks even, v* = C/(A N μ), is its switching value.
//
// Among several options (the status quo among them, an option of no cost and no effect), the one of greatest expected net present
// value is chosen; the probability that each is in fact best, and the expected value of perfect information about which is, are
// estimated from draws of the effects, independent across options, by tyche's decision table over the draws (tyche::decision::evpi
// with the draws equally likely), with the estimate's standard error. With two options the value is known in closed form, the unit
// normal loss at the difference (tyche::decision::evpi_normal), and the checks hold the draws to it.

#pragma once

#include <apodeixis/kernel/verdict.hpp>

#include <calx/random/stream.hpp>
#include <nomos/appraisal/cba.hpp>
#include <tyche/decision/choice.hpp>
#include <tyche/decision/value.hpp>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace policy::options {

using apodeixis::kernel::Verdict;

struct Option {
  std::string name;
  double effect = 0, se = 0;   // per unit of the population per year, in the outcome's units, and its standard error
  double value = 0;            // the worth of one unit of the outcome
  double population = 0;       // the units it reaches each year
  std::vector<double> cost;    // by year, year 0 first; its length is the horizon
  std::size_t from = 1;        // the first year with benefits
};

struct Appraised {
  std::string name;
  double mean = 0, sd = 0;                    // the net present value's
  double factor = 0;                          // A v N: the net present value per unit of effect
  double costs = 0;                           // C
  double switching = std::numeric_limits<double>::quiet_NaN();  // the worth of a unit at which it breaks even
  nomos::appraisal::Appraisal at_mean;        // nomos's appraisal of the streams at the mean effect
};

// Four significant figures; amounts of ten thousand or more in whole units, as money is read.
inline std::string number(double x) {
  char b[48];
  if (std::abs(x) >= 1e4 && std::abs(x) < 1e15) std::snprintf(b, sizeof b, "%.0f", x);
  else std::snprintf(b, sizeof b, "%.4g", x);
  return b;
}

inline Appraised appraise(const Option& o, const nomos::appraisal::Schedule& schedule) {
  if (o.cost.empty()) throw std::invalid_argument("policy::options: the option " + o.name + " states its costs year by year");
  if (o.from >= o.cost.size()) throw std::invalid_argument("policy::options: the option " + o.name + " has no year with benefits within its horizon");
  if (!(o.se >= 0) || !std::isfinite(o.effect) || !std::isfinite(o.value) || !(o.population >= 0))
    throw std::invalid_argument("policy::options: the option " + o.name + " has a finite effect, a non-negative standard error and population, and a finite worth");
  const std::size_t T = o.cost.size();
  const auto d = schedule.factors(T);
  Appraised a;
  a.name = o.name;
  double A = 0;
  for (std::size_t t = o.from; t < T; t++) A += d[t];
  std::vector<double> benefit(T, 0.0);
  for (std::size_t t = o.from; t < T; t++) benefit[t] = o.effect * o.value * o.population;
  a.at_mean = nomos::appraisal::appraise(benefit, o.cost, schedule);
  a.factor = A * o.value * o.population;
  a.costs = a.at_mean.costs;
  a.mean = a.at_mean.npv;
  a.sd = std::abs(a.factor) * o.se;
  if (A * o.population * o.effect != 0) a.switching = a.costs / (A * o.population * o.effect);
  return a;
}

struct Comparison {
  std::vector<Appraised> options;
  std::vector<std::size_t> chosen;           // the options of greatest expected net present value
  std::vector<double> best;                  // the probability that each is best, from the draws
  double evpi = 0, evpi_error = 0;           // and the expected value of perfect information, with its standard error
  std::size_t draws = 0;
  Verdict verdict;
};

inline Comparison compare(const std::vector<Option>& options, const nomos::appraisal::Schedule& schedule, std::size_t draws = 20000, std::uint64_t seed = 20261115) {
  if (options.size() < 2) throw std::invalid_argument("policy::options: a comparison is of two options or more (the status quo among them)");
  if (draws < 2) throw std::invalid_argument("policy::options: at least two draws");
  Comparison c;
  c.draws = draws;
  for (const auto& o : options) c.options.push_back(appraise(o, schedule));
  // The choice on the expected values: a table of one state, each option an act.
  tyche::decision::Table by_mean;
  for (const auto& a : c.options) by_mean.u.push_back({a.mean}), by_mean.acts.push_back(a.name);
  c.chosen = tyche::decision::expected_utility(by_mean, std::vector<double>{1.0}).chosen;
  // Draws of the effects, an option's net present value linear in its own.
  calx::random::Stream rng(seed, 0x6f707473u /* "opts" */, 0, 0);
  tyche::decision::Table drawn;
  drawn.u.assign(c.options.size(), std::vector<double>(draws));
  for (std::size_t i = 0; i < c.options.size(); i++) drawn.acts.push_back(c.options[i].name);
  c.best.assign(c.options.size(), 0.0);
  for (std::size_t s = 0; s < draws; s++) {
    std::size_t top = 0;
    for (std::size_t i = 0; i < c.options.size(); i++) {
      drawn.u[i][s] = c.options[i].mean + c.options[i].sd * rng.normal();
      if (drawn.u[i][s] > drawn.u[top][s]) top = i;
    }
    c.best[top] += 1.0 / static_cast<double>(draws);
  }
  const std::vector<double> equal(draws, 1.0 / static_cast<double>(draws));
  c.evpi = tyche::decision::evpi(drawn, equal);
  // Its standard error: that of the mean regret of the option best on the draws' means.
  const auto e = tyche::decision::expectations(drawn, equal);
  std::size_t star = 0;
  for (std::size_t i = 1; i < e.size(); i++)
    if (e[i] > e[star]) star = i;
  double sum = 0, sq = 0;
  for (std::size_t s = 0; s < draws; s++) {
    double top = drawn.u[0][s];
    for (std::size_t i = 1; i < c.options.size(); i++) top = std::max(top, drawn.u[i][s]);
    const double r = top - drawn.u[star][s];
    sum += r, sq += r * r;
  }
  const double n = static_cast<double>(draws), mean = sum / n;
  c.evpi_error = std::sqrt(std::max(0.0, (sq / n - mean * mean) / (n - 1)));
  const auto& a = c.options[c.chosen.front()];
  c.verdict = Verdict::yes(a.name + " is the option of greatest expected net present value",
                           a.name + ": " + number(a.mean) + " (standard deviation " + number(a.sd) + "), best on " + number(100 * c.best[c.chosen.front()]) + "% of " + std::to_string(draws) +
                               " draws; knowing which is best would be worth " + number(c.evpi) + " ± " + number(c.evpi_error));
  if (c.chosen.size() > 1) c.verdict.with("tied", std::to_string(c.chosen.size()) + " options");
  return c;
}

}  // namespace policy::options
