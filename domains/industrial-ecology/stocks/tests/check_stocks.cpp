// Checks of industrial_ecology::stocks: dynamic stock models against closed forms (a fixed lifetime with
// a constant inflow, the geometric stock of an exponential law, the echo of a stock kept by a fixed
// lifetime, the memoryless decay of an initial stock), the stock-driven model as the inverse of the
// inflow-driven, the correction of negative inflows, a lifetime law estimated by peira's Kaplan–Meier,
// and the materials a stock holds, with exact bounds; against numpy and scipy under ODYM's convention
// (tests/fixtures/stocks_normal.txt, made by make_stocks_fixtures.py).

#include <industrial_ecology/stocks/stocks.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace industrial_ecology::stocks;
using calx::verification::relative;

int main() {
  calx::verification::Suite s;

  // ---- a fixed lifetime with a constant inflow ----
  {
    const auto M = inflow_driven(std::vector<double>(30, 5.0), {fixed(10)});
    double worst = 0, worst_out = 0;
    for (std::size_t t = 0; t < 30; t++) {
      worst = std::max(worst, std::fabs(M.stock[t] - 5.0 * std::min<double>(static_cast<double>(t) + 1, 10)));
      worst_out = std::max(worst_out, std::fabs(M.outflow[t] - (t >= 10 ? 5.0 : 0.0)));
    }
    s.check("fixed lifetime 10, inflow 5: S(t) = 5 min(t + 1, 10)", worst, 1e-13);
    s.check("fixed lifetime 10, inflow 5: nothing leaves before year 10, then 5 a year", worst_out, 1e-13);
    s.check("fixed lifetime: the mean age of the steady stock is (L − 1)/2 = 4.5", std::fabs(M.mean_age[29] - 4.5), 1e-13);
    s.check("fixed lifetime: mass balance S(t) − S(t − 1) = I(t) − O(t)", M.balance, 1e-12);
  }
  // ---- an exponential law: the geometric stock ----
  {
    const double q = std::exp(-1.0 / 8);
    const auto M = inflow_driven(std::vector<double>(60, 2.0), {exponential(8)});
    double worst = 0;
    for (std::size_t t = 0; t < 60; t++) worst = std::max(worst, relative(M.stock[t], 2 * (1 - std::pow(q, static_cast<double>(t) + 1)) / (1 - q)));
    s.check("exponential law, mean 8: S(t) = I (1 − q^{t+1})/(1 − q), q = e^{−1/8}", worst, 1e-14);
  }
  // ---- a stock kept constant by a fixed lifetime: the echo ----
  {
    const auto M = stock_driven(std::vector<double>(30, 100.0), {fixed(7)});
    double worst = 0;
    for (std::size_t t = 0; t < 30; t++) worst = std::max(worst, std::fabs(M.inflow[t] - (t % 7 == 0 ? 100.0 : 0.0)));
    s.check("stock-driven, S = 100, fixed lifetime 7: the inflow returns every 7 years, I(t) = 100 when 7 | t", worst, 1e-12);
  }
  // ---- the stock-driven model inverts the inflow-driven ----
  {
    std::vector<double> inflow(50);
    for (std::size_t t = 0; t < 50; t++) inflow[t] = 20 + 10 * std::sin(0.3 * static_cast<double>(t)) + 0.4 * static_cast<double>(t);
    const std::vector<Lifetime> laws{weibull(2.5, 30)};
    const auto A = inflow_driven(inflow, laws);
    const auto B = stock_driven(A.stock, laws);
    double worst = 0, outflows = 0;
    for (std::size_t t = 0; t < 50; t++) worst = std::max(worst, relative(B.inflow[t], inflow[t])), outflows = std::max(outflows, std::fabs(B.outflow[t] - A.outflow[t]));
    s.check("stock-driven inverts inflow-driven: the inflows recovered (Weibull 2.5, 30)", worst, 1e-11);
    s.check("stock-driven inverts inflow-driven: the outflows agree", outflows, 1e-10);
  }
  // ---- a stock that falls faster than its cohorts leave ----
  {
    std::vector<double> stock(40);
    for (std::size_t t = 0; t < 40; t++) stock[t] = t < 20 ? 10.0 * static_cast<double>(t + 1) : 200.0 - 9.0 * static_cast<double>(t - 19);
    const std::vector<Lifetime> laws{weibull(3, 40)};
    const auto kept = stock_driven(stock, laws, nullptr, false);
    const auto fixedup = stock_driven(stock, laws);
    const bool negative = std::any_of(kept.inflow.begin(), kept.inflow.end(), [](double v) { return v < 0; });
    double follows = 0, retired = 0;
    bool nonnegative = true;
    for (std::size_t t = 0; t < 40; t++) {
      follows = std::max(follows, std::fabs(fixedup.stock[t] - stock[t]));
      retired += fixedup.retired_early[t];
      nonnegative = nonnegative && fixedup.inflow[t] >= 0;
      for (double o : fixedup.outflow_by_cohort[t]) nonnegative = nonnegative && o >= -1e-12;
    }
    s.require("a falling stock asks for negative inflows when they are kept", negative);
    s.require("corrected: no negative inflow and no negative outflow of any cohort", nonnegative);
    s.check("corrected: the stock follows its path exactly", follows, 1e-12);
    s.require("corrected: what was retired early is reported", retired > 0);
    s.check("corrected: mass is conserved", fixedup.balance, 1e-12);
  }
  // ---- an initial stock ----
  {
    Initial init{{10, 10, 10, 10, 10}, fixed(5)};  // ages 0..4 at the end of year −1
    const auto M = inflow_driven(std::vector<double>(6, 0.0), {fixed(5)}, &init);
    double worst = 0;
    for (std::size_t t = 0; t < 6; t++) worst = std::max(worst, std::fabs(M.stock[t] - std::max(0.0, 40.0 - 10.0 * static_cast<double>(t))));
    s.check("an initial stock of ages 0–4 under a fixed lifetime of 5 leaves one age a year: 40, 30, …, 0", worst, 1e-13);
    s.check("an initial stock: S(−1) = 50 and the balance from it", std::fabs(M.initial_total - 50) + M.balance, 1e-13);
    const double q = std::exp(-1.0 / 12);
    Initial memoryless{{3, 7, 1, 9}, exponential(12)};
    const auto E = inflow_driven(std::vector<double>(10, 0.0), {exponential(12)}, &memoryless);
    double decay = 0;
    for (std::size_t t = 0; t < 10; t++) decay = std::max(decay, relative(E.stock[t], 20 * std::pow(q, static_cast<double>(t) + 1)));
    s.check("an exponential law forgets age: the initial stock decays as 20 q^{t+1} whatever its ages", decay, 1e-14);
  }
  // ---- lifetime laws ----
  {
    const auto w = weibull(2.2, 35).survival(80);
    const tyche::probability::Weibull W{2.2, 35};
    double worst = 0;
    for (std::size_t a = 0; a < 80; a++) worst = std::max(worst, std::fabs(w[a] - W.survival(static_cast<double>(a))));
    s.check("weibull: the survival at whole ages is tyche's", worst, 1e-16);
    s.check("normal(30, 10), not truncated: S(0) = Φ(3) < 1", relative(normal(30, 10).survival(1)[0], tyche::probability::Normal{0, 1}.cdf(3)), 1e-15);
    const std::vector<double> lives{1, 2, 3, 4};
    const std::vector<int> died{1, 1, 1, 1};
    const auto km = peira::survival::kaplan_meier(lives, died);
    const auto L = from_kaplan_meier(km);
    const auto sf = L.survival(6);
    s.check("from Kaplan–Meier: four complete lives give S = 1, 0.75, 0.5, 0.25, 0, 0", std::fabs(sf[0] - 1) + std::fabs(sf[1] - 0.75) + std::fabs(sf[2] - 0.5) + std::fabs(sf[3] - 0.25) + sf[4] + sf[5], 1e-15);
    s.check("from Kaplan–Meier: the mean of the step function is the mean life, 2.5", std::fabs(L.mean - 2.5), 1e-15);
  }
  // ---- against numpy and scipy, ODYM's convention ----
  {
    std::ifstream in(std::string(INDUSTRIAL_ECOLOGY_FIXTURES) + "/stocks_normal.txt");
    std::vector<double> inflow, stock, outflow;
    std::string line;
    while (std::getline(in, line)) {
      if (line.empty() || line[0] == '#') continue;
      std::istringstream ls(line);
      double t, i, st, o;
      ls >> t >> i >> st >> o;
      inflow.push_back(i), stock.push_back(st), outflow.push_back(o);
    }
    s.require("fixture: forty years read", inflow.size() == 40);
    const auto M = inflow_driven(inflow, {normal(25, 8)});
    double ws = 0, wo = 0;
    for (std::size_t t = 0; t < inflow.size(); t++) ws = std::max(ws, relative(M.stock[t], stock[t])), wo = std::max(wo, std::fabs(M.outflow[t] - outflow[t]) / (1 + outflow[t]));
    s.check("normal(25, 8): the stock agrees with numpy and scipy", ws, 1e-13);
    s.check("normal(25, 8): the outflow agrees with numpy and scipy", wo, 1e-12);
  }
  // ---- the materials a stock holds ----
  {
    Initial init{{50, 40, 30}, weibull(2, 20)};
    std::vector<double> inflow(25);
    for (std::size_t t = 0; t < 25; t++) inflow[t] = 10 + static_cast<double>(t % 4);
    const auto M = inflow_driven(inflow, {weibull(2, 20)}, &init);
    Intensity in;
    in.materials = {"concrete", "steel"};
    in.lower = {{800, 40}}, in.central = {{1000, 55}}, in.upper = {{1300, 80}};
    in.initial_lower = {{900, 30}, {900, 30}, {700, 20}}, in.initial_central = {{1100, 45}, {1100, 45}, {950, 35}}, in.initial_upper = {{1400, 70}, {1400, 70}, {1200, 60}};
    const auto mat = materials(M, in);
    double order = 0, balance = 0, exact = 0;
    for (const auto& m : mat)
      for (std::size_t t = 0; t < 25; t++) {
        for (const Bounded* b : {&m.stock, &m.inflow, &m.outflow}) order = std::max(order, std::max(b->lower[t] - b->central[t], b->central[t] - b->upper[t]));
        const double prev_central = t == 0 ? (m.material == "concrete" ? 50 * 1100 + 40 * 1100 + 30 * 950 : 50 * 45 + 40 * 45 + 30 * 35) : m.stock.central[t - 1];
        balance = std::max(balance, std::fabs(m.stock.central[t] - prev_central - (m.inflow.central[t] - m.outflow.central[t])));
      }
    // With one intensity everywhere the material stock is the stock times it.
    Intensity flat;
    flat.materials = {"steel"};
    flat.lower = flat.central = flat.upper = {{55}};
    flat.initial_lower = flat.initial_central = flat.initial_upper = {{55}};
    const auto f = materials(M, flat);
    for (std::size_t t = 0; t < 25; t++) exact = std::max(exact, relative(f[0].stock.central[t], 55 * M.stock[t]));
    s.check("materials: lower ≤ central ≤ upper in every period, for the stock and both flows", std::max(order, 0.0), 0.0);
    s.check("materials: each material balances, including what the initial stock's ages hold", balance, 1e-9);
    s.check("materials: one intensity everywhere gives the stock times it", exact, 1e-14);
  }

  return s.done();
}
