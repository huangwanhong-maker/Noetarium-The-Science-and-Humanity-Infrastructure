// Planning a stated-choice survey about a street: what to show, and how many people must answer (ADR 0088).
//
// THE SURVEY. People are shown choice sets of alternatives for a street, each described by attributes (a protected lane, trees, the
// traffic's speed, minutes more on the way, a levy) and asked which they prefer. Their choices identify how much each attribute is
// worth to them against the levy: a willingness to pay (nomos's conditional logit). What to show is a design (peira's coordinate
// exchange at priors for the coefficients); how many must answer follows from the design's information.
//
// HOW MANY. Each respondent answers every set of the design, so N respondents give the information N M(β), M the design's (peira), and
// the asymptotic covariance of β̂ is M⁻¹/N (McFadden 1974). The willingness to pay for attribute k, −β_k/β_p, has the delta-method
// standard error se₁/√N with se₁ from M⁻¹, so its 95% interval has half-width h when N = (1.96 se₁ / h)². This is the design's
// promise at the priors; `simulate` holds it to what N simulated respondents answering by the priors give (the check: within 15%).
// Where the priors are wrong the promise is too; a Bayesian design (peira) hedges the design, not the count.

#pragma once

#include <nomos/econometrics/conditional_logit.hpp>
#include <peira/design/choice.hpp>

#include <calx/linalg/dense.hpp>
#include <calx/random/stream.hpp>

#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace urban::demand {

struct SurveyPlan {
  peira::design::ChoiceDesign design;
  std::vector<double> prior;                    // the coefficients the design was made at
  calx::linalg::Matrix information;             // M(β): one respondent's
  std::vector<double> se_one;                   // √diag M⁻¹: one respondent's standard errors
  std::size_t price = 0;                        // the coded column of the levy
  std::vector<double> wtp, wtp_se_one;          // per coded column: −β_k/β_p at the priors and its standard error for one respondent
  std::vector<double> respondents;              // per coded column: N for a 95% half-width of `half_width` (NaN for the price)
  double half_width = 0;
};

// A design of `sets` sets of `alternatives`, at the priors, and the respondents its willingness to pay needs.
inline SurveyPlan plan_survey(const std::vector<peira::design::ChoiceAttribute>& attributes, std::size_t sets, std::size_t alternatives, const std::vector<double>& prior,
                              std::size_t price, double half_width, calx::random::Stream& stream, std::size_t starts = 10) {
  if (!(half_width > 0)) throw std::invalid_argument("demand: a half-width, positive");
  SurveyPlan p;
  p.design = peira::design::coordinate_exchange(attributes, sets, alternatives, prior, stream, starts);
  p.prior = prior, p.price = price, p.half_width = half_width;
  p.information = peira::design::choice_information(p.design, prior);
  const std::size_t K = prior.size();
  if (price >= K) throw std::invalid_argument("demand: the levy among the coded columns");
  const auto V = calx::linalg::solve(p.information, calx::linalg::Matrix::identity(K));
  for (std::size_t c = 0; c < K; c++) p.se_one.push_back(std::sqrt(V(c, c)));
  for (std::size_t c = 0; c < K; c++) {
    if (c == price) {
      p.wtp.push_back(NAN), p.wtp_se_one.push_back(NAN), p.respondents.push_back(NAN);
      continue;
    }
    const auto w = nomos::econometrics::willingness_to_pay(prior, V, c, price);
    p.wtp.push_back(w.value), p.wtp_se_one.push_back(w.se);
    p.respondents.push_back(std::ceil(std::pow(1.959963984540054 * w.se / half_width, 2)));
  }
  return p;
}

// N simulated respondents answering the plan's design by its priors, `replications` times: each replication's estimated willingness
// to pay for coded column k.
inline std::vector<double> simulate(const SurveyPlan& p, std::size_t k, std::size_t N, std::size_t replications, calx::random::Stream& stream) {
  const auto& d = p.design;
  const std::size_t J = d.alternatives, A = d.attributes.size(), K = p.prior.size();
  std::vector<double> out;
  std::vector<double> row(K), u(J);
  for (std::size_t r = 0; r < replications; r++) {
    nomos::econometrics::ChoiceData data;
    data.X = calx::linalg::Rectangular(N * d.sets * J, K);
    std::size_t at = 0;
    for (std::size_t n = 0; n < N; n++)
      for (std::size_t s = 0; s < d.sets; s++) {
        data.start.push_back(at);
        double top = -1e300;
        for (std::size_t j = 0; j < J; j++) {
          peira::design::detail_choice::code(d.attributes, &d.level[(s * J + j) * A], row.data());
          double v = 0;
          for (std::size_t c = 0; c < K; c++) data.X(at + j, c) = row[c], v += row[c] * p.prior[c];
          // Gumbel errors: the conditional logit's.
          u[j] = v - std::log(-std::log(stream.uniform()));
          top = std::max(top, u[j]);
        }
        std::size_t pick = 0;
        for (std::size_t j = 0; j < J; j++)
          if (u[j] == top) pick = j;
        data.chosen.push_back(at + pick);
        at += J;
      }
    data.start.push_back(at);
    const auto f = nomos::econometrics::conditional_logit(data);
    out.push_back(-f.beta[k] / f.beta[p.price]);
  }
  return out;
}

}  // namespace urban::demand
