// Checks of options appraised (options.hpp): the net present value's mean and spread in closed form and as nomos appraises the
// streams; the switching worth of a unit; two options compared, the value of perfect information and the probability of the better
// held to their closed forms within the draws' error; three options; refusals.

#include <policy/options/options.hpp>

#include <tyche/probability/distributions.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace policy;

int main() {
  calx::verification::Suite s;
  const auto schedule = nomos::appraisal::Schedule::constant(0.035);
  // A programme: 0.1 days more attendance a pupil a year (standard error 0.05), each day worth 1000, for 500 pupils; it costs
  // 20,000 at once and 5,000 a year for nine years.
  options::Option meals{"meals", 0.1, 0.05, 1000, 500, std::vector<double>(10, 5000.0), 1};
  meals.cost[0] = 20000;
  double A = 0;
  for (int t = 1; t < 10; t++) A += std::pow(1.035, -t);
  const double C = 20000 + 5000 * A, mean = A * 1000 * 500 * 0.1 - C, sd = A * 1000 * 500 * 0.05;
  const auto a = options::appraise(meals, schedule);
  s.check("the expected net present value is A v N μ − C", std::abs(a.mean - mean), 1e-8);
  s.check("its standard deviation |A v N| σ", std::abs(a.sd - sd), 1e-8);
  s.check("and nomos's appraisal of the streams at the mean agrees", std::abs(a.at_mean.npv - mean), 1e-8);
  auto at = meals;
  at.value = a.switching;
  s.check("at the switching worth of a unit, C/(A N μ), the programme breaks even", std::abs(options::appraise(at, schedule).mean), 1e-8);

  options::Option none{"status quo", 0, 0, 0, 0, std::vector<double>(10, 0.0), 1};
  const auto c = options::compare({none, meals}, schedule, 200000);
  const double closed = tyche::decision::evpi_normal(mean, sd);
  s.require("between the status quo and the programme, the programme of positive expected value is chosen", c.chosen == std::vector<std::size_t>{1} && mean > 0);
  s.require("the value of perfect information from 200,000 draws is the unit normal loss within four standard errors (" + options::number(c.evpi) + " against " + options::number(closed) + ")",
            std::abs(c.evpi - closed) < 4 * c.evpi_error && c.evpi_error > 0);
  const double p = tyche::probability::Normal{0, 1}.cdf(mean / sd);
  s.require("and the probability that the programme is the better is Φ(μ/σ) within four standard errors", std::abs(c.best[1] - p) < 4 * std::sqrt(p * (1 - p) / 200000));

  options::Option books{"books", 0.05, 0.01, 1000, 500, std::vector<double>(10, 2000.0), 1};
  const auto three = options::compare({none, meals, books}, schedule, 50000);
  double total = 0;
  for (double b : three.best) total += b;
  s.check("among three options the probabilities of being best sum to 1", std::abs(total - 1), 1e-9);
  s.require("and the option of greatest expected value is chosen", three.chosen.front() == (three.options[1].mean > three.options[2].mean ? 1u : 2u) && three.evpi >= 0);

  bool r1 = false, r2 = false;
  try { (void)options::appraise({"x", 0.1, 0.05, 1, 1, {100}, 1}, schedule); } catch (const std::invalid_argument&) { r1 = true; }
  try { (void)options::compare({meals}, schedule); } catch (const std::invalid_argument&) { r2 = true; }
  s.require("an option with no year of benefits within its horizon, and a comparison of one option, are refused", r1 && r2);
  return s.done();
}
