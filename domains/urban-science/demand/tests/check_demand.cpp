// Checks of demand (hedonic.hpp, stated.hpp).
//
// - The rule: on prices drawn from a lag model it chooses the lag, from an error model the error, without spatial dependence least
//   squares; and the chosen model recovers the coefficient of a characteristic within four standard errors.
// - The survey's promise: the standard error of the willingness to pay for a lane that the design's information gives for 200
//   respondents is within 15% of the spread of 120 replications of 200 simulated respondents answering by the priors; the plan's count
//   of respondents scales as 1/h².

#include <urban/demand/hedonic.hpp>
#include <urban/demand/stated.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace ud = urban::demand;
namespace ts = tyche::spatial;

int main() {
  calx::verification::Suite s;
  {
    // 600 places at random in 3 km; ln P = 12 + 0.8 floor + spatial structure.
    calx::random::Stream st(20261109, 0x6865646fu, 0, 0);  // "hedo"
    const std::size_t n = 600;
    std::vector<double> x(n), y(n), floor(n);
    for (std::size_t i = 0; i < n; i++) x[i] = st.uniform(0, 3000), y[i] = st.uniform(0, 3000), floor[i] = st.normal();
    const auto W = ts::k_nearest_weights(x, y, 8);
    calx::linalg::Rectangular X(n, 2);
    for (std::size_t i = 0; i < n; i++) X(i, 0) = 1, X(i, 1) = floor[i];
    const auto solveA = [&](double rho, const std::vector<double>& v) {
      calx::linalg::Matrix A = calx::linalg::Matrix::identity(n), B(n);
      for (std::size_t i = 0; i < n; i++)
        for (std::size_t q = W.row_start[i]; q < W.row_start[i + 1]; q++) A(i, W.column[q]) -= rho * W.weight[q];
      for (std::size_t i = 0; i < n; i++) B(i, 0) = v[i];
      const auto Y = calx::linalg::solve(A, B);
      std::vector<double> out(n);
      for (std::size_t i = 0; i < n; i++) out[i] = Y(i, 0);
      return out;
    };
    std::vector<double> e(n), xb(n);
    for (std::size_t i = 0; i < n; i++) e[i] = 0.3 * st.normal(), xb[i] = 12 + 0.8 * floor[i];
    // Lag: y = (I − 0.6W)⁻¹ (Xβ + ε).
    std::vector<double> v(n);
    for (std::size_t i = 0; i < n; i++) v[i] = xb[i] + e[i];
    const auto ylag = solveA(0.6, v);
    // Error: y = Xβ + (I − 0.7W)⁻¹ ε.
    const auto u = solveA(0.7, e);
    std::vector<double> yerr(n), yols(n);
    for (std::size_t i = 0; i < n; i++) yerr[i] = xb[i] + u[i], yols[i] = xb[i] + e[i];
    const auto fl = ud::hedonic(X, ylag, x, y), fe = ud::hedonic(X, yerr, x, y), fo = ud::hedonic(X, yols, x, y);
    s.require("the rule: prices from a lag model choose the lag", fl.chosen == ud::SpatialModel::lag);
    s.require("... from an error model the error", fe.chosen == ud::SpatialModel::error);
    s.require("... and without spatial dependence least squares (Moran's I of the residuals " + std::to_string(fo.moran) + ")", fo.chosen == ud::SpatialModel::least_squares);
    s.check("... each chosen model recovers the floor's 0.8 within four standard errors (largest |z|)",
            std::max({std::fabs(fl.beta[1] - 0.8) / fl.se[1], std::fabs(fe.beta[1] - 0.8) / fe.se[1], std::fabs(fo.beta[1] - 0.8) / fo.se[1]}), 4);
  }
  {
    // A lane (binary), trees (three levels), minutes more (numeric 0, 5, 10), a levy (numeric 0, 10, 25 a month); triples, 12 sets.
    const std::vector<peira::design::ChoiceAttribute> at{{"lane", 2, {}}, {"trees", 3, {}}, {"minutes", 3, {0, 5, 10}}, {"levy", 3, {0, 10, 25}}};
    const std::vector<double> prior{0.5, 0.3, 0.1, -0.06, -0.05};
    calx::random::Stream st(20261109, 0x73747464u, 0, 0);  // "sttd"
    const auto p = ud::plan_survey(at, 12, 3, prior, 4, 2.0, st, 8);
    const auto w = ud::simulate(p, 0, 200, 120, st);
    double mean = 0, var = 0;
    for (double v : w) mean += v / static_cast<double>(w.size());
    for (double v : w) var += (v - mean) * (v - mean) / static_cast<double>(w.size() - 1);
    const double promised = p.wtp_se_one[0] / std::sqrt(200.0);
    s.check("the survey's promise: the lane's willingness to pay for 200 respondents, the design's standard error against 120 simulations (relative)",
            std::fabs(std::sqrt(var) - promised) / promised, 0.15);
    const auto p2 = ud::plan_survey(at, 12, 3, prior, 4, 1.0, st, 8);
    s.check("... and the respondents a half-width needs scale as 1/h²: twice as precise, about four times as many",
            std::fabs(p2.respondents[0] / p.respondents[0] * p.wtp_se_one[0] * p.wtp_se_one[0] / (p2.wtp_se_one[0] * p2.wtp_se_one[0]) - 4) / 4, 0.02);
  }
  return s.done();
}
