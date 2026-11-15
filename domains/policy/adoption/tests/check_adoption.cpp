// Checks of the decision to adopt, reject or study first (adoption.hpp): clear cases each way; an uncertain programme whose study is
// worth doing, the size found the best of every size and its EVSI below EVPI; the domain's EVSI held to a preposterior analysis on a
// fine table by tyche's own evsi, which knows nothing of normal laws; waiting made costly; the switching worth of a unit and the
// robustness of the choice over bounds on it; refusals.

#include <policy/adoption/adoption.hpp>

#include <tyche/probability/distributions.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace policy;

namespace {

options::Option programme(double effect) {
  // 1000 pupils, each day of attendance worth 1000; 10,000 at once, benefits for five years.
  options::Option o{"meals", effect, 0.03, 1000, 1000, std::vector<double>(6, 0.0), 1};
  o.cost[0] = 10000;
  return o;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto schedule = nomos::appraisal::Schedule::constant(0.035);
  const adoption::Study study{0.25, 2000, 5, 2000, 1};

  // ---- Clear cases ---------------------------------------------------------------------------------------------------------------
  {
    const auto yes = adoption::decide(programme(0.2), schedule, study);
    s.require("a programme whose net value is six standard deviations above zero is adopted, and learning more is worth almost nothing",
              yes.chosen == std::vector<std::string>{"adopt"} && yes.evpi < 1e-3 * yes.adopt, yes.verdict.reason);
    const auto no = adoption::decide(programme(-0.05), schedule, study);
    s.require("one whose net value is well below zero is rejected", no.chosen == std::vector<std::string>{"reject"}, no.verdict.reason);
  }

  // ---- An uncertain programme, and the study worth doing -------------------------------------------------------------------------
  {
    const auto o = programme(0.02);
    const auto d = adoption::decide(o, schedule, study);
    s.require("an uncertain programme (mean 0.59 standard deviations above zero) is best studied first, at a size found among every size up to 2000",
              d.chosen == std::vector<std::string>{"study first"} && d.n > 1 && d.n < 2000, d.verdict.reason);
    const auto at = [&](std::size_t n) { return adoption::study_worth(d.option, o.se, study, n).first; };
    s.require("the size chosen is worth at least its neighbours", at(d.n) >= at(d.n - 1) && at(d.n) >= at(d.n + 1));
    s.require("and its EVSI is below EVPI, toward which EVSI rises with the size", d.evsi < d.evpi && adoption::study_worth(d.option, o.se, study, 1000000).second > d.evsi);

    // The same EVSI by tyche's preposterior analysis of a table: the effect on a fine grid under its normal prior, the study's mean
    // observed in bins, each bin's chance under each effect the normal law's mass over it.
    const tyche::probability::Normal z{0, 1};
    const std::size_t S = 1201, Y = 1201, n = 50;
    const double mu = o.effect, sigma = o.se, noise = std::sqrt(study.unit_variance / static_cast<double>(n));
    tyche::decision::Table t;
    t.u.assign(2, std::vector<double>(S, 0.0));
    std::vector<double> prior(S), grid(S);
    double total = 0;
    for (std::size_t i = 0; i < S; i++) {
      grid[i] = mu + sigma * (-7.0 + 14.0 * static_cast<double>(i) / static_cast<double>(S - 1));
      total += prior[i] = z.pdf((grid[i] - mu) / sigma);
      t.u[1][i] = d.option.factor * grid[i] - d.option.costs;  // adopt; reject is 0
    }
    for (auto& p : prior) p /= total;
    tyche::decision::Test test;
    const double spread = 7 * std::sqrt(sigma * sigma + noise * noise);
    std::vector<double> edges;
    for (std::size_t k = 1; k < Y; k++) edges.push_back(mu - spread + 2 * spread * static_cast<double>(k) / static_cast<double>(Y));
    for (std::size_t i = 0; i < S; i++) {
      std::vector<double> row(Y);
      double below = 0;
      for (std::size_t k = 0; k + 1 < Y; k++) {
        const double c = z.cdf((edges[k] - grid[i]) / noise);
        row[k] = c - below, below = c;
      }
      row[Y - 1] = 1 - below;
      test.likelihood.push_back(row);
    }
    const double table_evsi = tyche::decision::evsi(t, prior, test).evsi;
    const double domain_evsi = adoption::study_worth(d.option, o.se, study, n).second;
    s.check("the EVSI of a study of 50 by the normal formula and by a preposterior analysis on a table of 1201 effects and 1201 signals agree (relative)",
            std::abs(table_evsi - domain_evsi) / domain_evsi, 2e-3);

    adoption::Study slow = study;
    slow.delay = 0.6;
    const auto waited = adoption::decide(o, schedule, slow);
    s.require("when waiting for the study costs four tenths of the decision's worth, adopting now is better", waited.chosen == std::vector<std::string>{"adopt"}, waited.verdict.reason);
  }

  // ---- The worth of a unit -------------------------------------------------------------------------------------------------------
  {
    const auto o = programme(0.02);
    const auto d = adoption::decide(o, schedule, std::nullopt, std::pair{50.0, 1500.0});
    auto at = o;
    at.value = d.option.switching;
    s.check("the switching worth of a unit, C/(A N μ), makes the net value zero", std::abs(options::appraise(at, schedule).mean), 1e-8);
    s.require("with the worth between 50 and 1500, the choice turns on it (it switches near 111), and Γ-maximin rejects", !d.robust && d.maximin == "reject" && d.verdict.reason.find("turns on the worth") != std::string::npos,
              d.verdict.reason);
    const auto firm = adoption::decide(o, schedule, std::nullopt, std::pair{200.0, 1500.0});
    s.require("between 200 and 1500 it is adopted at every worth", firm.robust && firm.maximin == "adopt");
  }

  bool r1 = false, r2 = false, r3 = false;
  adoption::Study bad = study;
  bad.delay = 1.5;
  try { (void)adoption::decide(programme(0.02), schedule, bad); } catch (const std::invalid_argument&) { r1 = true; }
  bad = study, bad.unit_variance = 0;
  try { (void)adoption::decide(programme(0.02), schedule, bad); } catch (const std::invalid_argument&) { r2 = true; }
  try { (void)adoption::decide(programme(0.02), schedule, std::nullopt, std::pair{1200.0, 1500.0}); } catch (const std::invalid_argument&) { r3 = true; }
  s.require("a factor for waiting above 1, a study of no sampling variance, and bounds on the worth that exclude the worth stated are refused", r1 && r2 && r3);
  return s.done();
}
