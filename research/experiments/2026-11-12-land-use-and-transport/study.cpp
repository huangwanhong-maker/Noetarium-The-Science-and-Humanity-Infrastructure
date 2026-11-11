// Land use and transport on New York City's journeys to work, 2002–2023 (ADR 0094): the record is LODES 8's flows between the
// five boroughs' census tracts (data/products/commuting.py, case nyc-lodes). The costs are the great-circle kilometres between
// the tracts' points; a tract's own cost is half the distance to its nearest neighbour.
//
//   1. Distance decay, year by year: the doubly constrained model calibrated to each year's table (Hyman's β, the Poisson
//      maximum likelihood estimate), its common part of commuters, and the independence model's (β = 0) for scale.
//   2. Forecasting flows: 2019's table from 2010's β and 2019's margins, against 2010's table carried to 2019's margins by
//      Furness (the cells it lacks, a tract's first jobs or workers among them, given that gravity forecast, so that every
//      margin can be met: the pivot-point form), against 2019's own calibration and 2010's table unchanged; the same from 2019
//      to 2023 and from 2002 to 2019.
//   3. Access and residence: each tract's change of accessibility to jobs, 2010 to 2019 (the logsum at 2010's β), against the
//      change of the log of its resident workers, with the boroughs' own means removed; Moran's I of what is left. The change of
//      access is smooth in space and what is left is clustered, so that least squares' standard errors count the tracts as
//      independent when they are not: the spatial error model (tyche's, eight nearest neighbours) gives the slope's honest one.
//   4. The composed model (urban::luti), revealed in 2010 (the resident workers as households, rents all at the rent at which
//      building pays), the jobs of each year between the record's years interpolated, run to 2019. One number, the speed of
//      building, is set so that the region's resident workers in 2019 are the record's; where they went is the model's.
//
// Usage: study <product directory> <results directory>

#include <urban/luti/model.hpp>

#include <nomos/interaction/gravity.hpp>
#include <tyche/regression/linear.hpp>
#include <tyche/spatial/autocorrelation.hpp>
#include <tyche/spatial/regression.hpp>

#include <chrono>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

namespace ni = nomos::interaction;
namespace ul = urban::luti;

namespace {
struct Tract {
  std::string geoid, borough;
  double lat = 0, lon = 0;
};

std::vector<std::string> split(const std::string& line) {
  std::vector<std::string> out;
  std::string cell;
  bool quoted = false;
  for (char c : line) {
    if (c == '"') quoted = !quoted;
    else if (c == ',' && !quoted) out.push_back(cell), cell.clear();
    else if (c != '\r') cell += c;
  }
  out.push_back(cell);
  return out;
}

std::vector<Tract> read_tracts(const std::string& path) {
  std::ifstream f(path);
  if (!f) throw std::runtime_error("cannot read " + path);
  std::string line;
  std::getline(f, line);
  std::vector<Tract> out;
  while (std::getline(f, line)) {
    const auto c = split(line);
    out.push_back({c[1], c[3], std::stod(c[4]), std::stod(c[5])});
  }
  return out;
}

ni::Table read_flows(const std::string& path, std::size_t n) {
  std::ifstream f(path);
  if (!f) throw std::runtime_error("cannot read " + path);
  std::string line;
  std::getline(f, line);
  ni::Table t(n, n);
  while (std::getline(f, line)) {
    const auto c = split(line);
    t(std::stoul(c[0]), std::stoul(c[1])) += std::stod(c[2]);
  }
  return t;
}

double km(const Tract& a, const Tract& b) {
  constexpr double R = 6371.0088, rad = M_PI / 180;   // the mean radius of the Earth (IUGG)
  const double p1 = a.lat * rad, p2 = b.lat * rad, dp = p2 - p1, dl = (b.lon - a.lon) * rad;
  const double h = std::sin(dp / 2) * std::sin(dp / 2) + std::cos(p1) * std::cos(p2) * std::sin(dl / 2) * std::sin(dl / 2);
  return 2 * R * std::asin(std::sqrt(h));
}

double seconds_since(std::chrono::steady_clock::time_point t) { return std::chrono::duration<double>(std::chrono::steady_clock::now() - t).count(); }

std::string num(double x) {
  if (!std::isfinite(x)) return "null";
  char b[32];
  std::snprintf(b, sizeof b, "%.6g", x);
  return b;
}
}  // namespace

int main(int argc, char** argv) {
  if (argc < 3) { std::cerr << "usage: study <product> <results>\n"; return 2; }
  const std::string product = argv[1], results = argv[2];
  const auto started = std::chrono::steady_clock::now();
  const auto tracts = read_tracts(product + "/tracts.csv");
  const std::size_t n = tracts.size();
  ni::Table cost(n, n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) cost(i, j) = i == j ? 0.0 : km(tracts[i], tracts[j]);
  for (std::size_t i = 0; i < n; i++) {
    double nearest = 1e300;
    for (std::size_t j = 0; j < n; j++)
      if (j != i) nearest = std::min(nearest, cost(i, j));
    cost(i, i) = nearest / 2;
  }
  std::cerr << n << " tracts; costs in " << seconds_since(started) << " s\n";

  const std::vector<int> years{2002, 2005, 2008, 2010, 2012, 2015, 2017, 2019, 2021, 2023};
  std::map<int, ni::Table> flows;
  std::map<int, double> beta;
  std::ostringstream js;
  js << "{\n \"tracts\": " << n << ",\n \"decay\": [";
  bool first = true;
  for (const int y : years) {
    flows[y] = read_flows(product + "/flows-" + std::to_string(y) + ".csv", n);
    const auto& N = flows[y];
    const auto t0 = std::chrono::steady_clock::now();
    const auto cal = ni::calibrate(N, cost, 1e-8);
    beta[y] = cal.beta;
    const auto O = N.row_sums(), D = N.col_sums();
    const auto independent = ni::doubly_constrained(O, D, cost, 0.0).flows;
    js << (first ? "" : ",") << "\n  {\"year\": " << y << ", \"jobs\": " << num(N.total()) << ", \"beta\": " << num(cal.beta) << ", \"meanKm\": " << num(ni::mean_cost(N, cost))
       << ", \"cpc\": " << num(ni::common_part(N, cal.fit.flows)) << ", \"srmse\": " << num(ni::srmse(N, cal.fit.flows)) << ", \"cpcIndependent\": " << num(ni::common_part(N, independent))
       << ", \"converged\": " << (cal.converged ? "true" : "false") << "}";
    first = false;
    std::cerr << y << ": " << N.total() << " jobs, beta " << cal.beta << " per km, CPC " << ni::common_part(N, cal.fit.flows) << " (" << seconds_since(t0) << " s)\n";
  }
  js << "\n ],\n \"forecast\": [";
  first = true;
  for (const auto& [from, to] : std::vector<std::pair<int, int>>{{2010, 2019}, {2019, 2023}, {2002, 2019}}) {
    const auto& A = flows[from];
    const auto& B = flows[to];
    const auto O = B.row_sums(), D = B.col_sums();
    const auto gravity = ni::doubly_constrained(O, D, cost, beta[from]).flows;
    const auto own = ni::doubly_constrained(O, D, cost, beta[to]).flows;
    ni::Table seed = A;
    for (std::size_t k = 0; k < seed.v.size(); k++)
      if (!(seed.v[k] > 0)) seed.v[k] = gravity.v[k];
    const auto carried = ni::balance(seed, O, D, 1e-6, 5000);   // to a millionth of each margin: ample for a common part given to three decimals
    js << (first ? "" : ",") << "\n  {\"from\": " << from << ", \"to\": " << to << ", \"cpcGravity\": " << num(ni::common_part(B, gravity)) << ", \"cpcCarried\": " << num(ni::common_part(B, carried.flows))
       << ", \"cpcOwn\": " << num(ni::common_part(B, own)) << ", \"cpcUnchanged\": " << num(ni::common_part(B, A)) << ", \"carriedConverged\": " << (carried.converged ? "true" : "false") << ", \"carriedResidual\": " << num(carried.residual) << ", \"carriedSweeps\": " << carried.iterations << "}";
    first = false;
    std::cerr << "forecast " << from << " to " << to << ": CPC " << ni::common_part(B, gravity) << " by gravity, " << ni::common_part(B, carried.flows) << " carried (" << carried.iterations << " sweeps), "
              << seconds_since(started) << " s\n";
  }
  js << "\n ],";

  // Access and residence, 2010 to 2019.
  const int y0 = 2010, y1 = 2019;
  const auto H0 = flows[y0].row_sums(), H1 = flows[y1].row_sums(), E0 = flows[y0].col_sums(), E1 = flows[y1].col_sums();
  const auto A0 = ni::logsum(E0, cost, beta[y0]), A1 = ni::logsum(E1, cost, beta[y0]);
  std::vector<std::size_t> kept;
  for (std::size_t i = 0; i < n; i++)
    if (H0[i] >= 200 && H1[i] > 0 && std::isfinite(A0[i]) && std::isfinite(A1[i])) kept.push_back(i);
  std::vector<double> dA, dlnH, x, y;
  std::map<std::string, std::vector<double>> dummies;
  std::vector<std::string> boroughs{"Kings", "New York", "Queens", "Richmond"};   // the Bronx the reference
  for (const auto& b : boroughs) dummies[b] = {};
  for (const std::size_t i : kept) {
    dA.push_back(A1[i] - A0[i]);
    dlnH.push_back(std::log(H1[i] / H0[i]));
    for (const auto& b : boroughs) dummies[b].push_back(tracts[i].borough == b ? 1.0 : 0.0);
    const double lat0 = 40.7128 * M_PI / 180;
    x.push_back(6371.0088 * tracts[i].lon * M_PI / 180 * std::cos(lat0));
    y.push_back(6371.0088 * tracts[i].lat * M_PI / 180);
  }
  std::vector<std::vector<double>> cols{dA};
  for (const auto& b : boroughs) cols.push_back(dummies[b]);
  tyche::regression::OlsOptions oo;
  oo.covariance = tyche::regression::Covariance::hc1;
  const auto fit = tyche::regression::ols(tyche::regression::design_matrix(cols), dlnH, oo);
  std::vector<std::vector<double>> only_boroughs;
  for (const auto& b : boroughs) only_boroughs.push_back(dummies[b]);
  const auto base_fit = tyche::regression::ols(tyche::regression::design_matrix(only_boroughs), dlnH, oo);
  const auto W = tyche::spatial::k_nearest_weights(x, y, 8);
  const auto moran = tyche::spatial::morans_i(fit.residuals, W, 999, 20261112);
  const auto moran_dA = tyche::spatial::morans_i(dA, W, 999, 20261112);
  const auto sem = tyche::spatial::spatial_error(tyche::regression::design_matrix(cols), dlnH, W);
  std::cerr << "access, spatial error model: slope " << sem.beta[1] << " ± " << sem.se[1] << ", λ " << sem.parameter << " (" << seconds_since(started) << " s)\n";
  js << "\n \"access\": {\"tracts\": " << kept.size() << ", \"slope\": " << num(fit.beta[1]) << ", \"se\": " << num(fit.se[1]) << ", \"t\": " << num(fit.t[1]) << ", \"p\": " << num(fit.p[1])
     << ", \"r2\": " << num(fit.r_squared) << ", \"r2Boroughs\": " << num(base_fit.r_squared) << ", \"moranResiduals\": " << num(moran.I) << ", \"moranResidualsP\": " << num(moran.p_permutation)
     << ", \"moranAccess\": " << num(moran_dA.I) << ", \"semSlope\": " << num(sem.beta[1]) << ", \"semSe\": " << num(sem.se[1]) << ", \"semP\": " << num(sem.p[1])
     << ", \"semLambda\": " << num(sem.parameter) << ", \"beta\": " << num(beta[y0]) << "},";
  std::cerr << "access: slope " << fit.beta[1] << " ± " << fit.se[1] << " per km of access, R² " << fit.r_squared << " against " << base_fit.r_squared << " by boroughs alone\n";

  // The composed model, revealed in 2010 and run to 2019.
  ul::Model m;
  m.market.classes = {{1.0, 1.0, {}}};
  m.market.alpha = 1;
  m.market.mean_income = 1;
  m.market.income_power = 1;
  m.market.vacancy = 0.05;
  m.market.reference_rent = 1;
  double total0 = 0, total1 = 0;
  for (std::size_t i = 0; i < n; i++) total0 += H0[i], total1 += H1[i];
  m.market.households = 2 * total1;
  m.transport.costs = cost;
  m.beta = beta[y0];
  m.lambda = 1;
  m.workers_per_household = 1;
  m.zones.resize(n);
  for (std::size_t i = 0; i < n; i++) m.zones[i].basic = E0[i];
  // Zones with no resident workers in 2010 take one, so that every zone is a segment of the market.
  std::vector<double> households0 = H0;
  for (double& h : households0) h = std::max(h, 1.0);
  const auto revealed = ul::reveal(m, households0);
  m.development = {1.0, 0.0, 0.0};
  // The jobs of each year, linear between the record's years.
  const std::vector<int> record{2010, 2012, 2015, 2017, 2019};
  std::vector<ul::Event> events;
  for (int t = 1; t <= y1 - y0; t++) {
    const int year = y0 + t;
    std::size_t k = 0;
    while (record[k + 1] < year) k++;
    const double f = double(year - record[k]) / (record[k + 1] - record[k]);
    const auto Ea = flows[record[k]].col_sums(), Eb = flows[record[k + 1]].col_sums();
    for (std::size_t i = 0; i < n; i++) {
      ul::Event e;
      e.kind = ul::Event::basic, e.year = t, e.index = i, e.value = (1 - f) * Ea[i] + f * Eb[i];
      events.push_back(e);
    }
  }
  const auto run = [&](double speed) {
    ul::Model r = m;
    r.development.speed = speed;
    ul::Year y = revealed;
    for (int t = 1; t <= y1 - y0; t++) {
      ul::apply(r, events, t);
      y = ul::step(r, y);
    }
    return y;
  };
  // The speed at which the region's resident workers in 2019 are the record's (the total rises with the speed).
  double lo = 0, hi = 1;
  const auto located = [](const ul::Year& y) {
    double s = 0;
    for (const double h : y.households) s += h;
    return s;
  };
  while (located(run(hi)) < total1 && hi < 1e4) hi *= 2;
  ul::Year best;
  for (int k = 0; k < 40; k++) {
    const double mid = 0.5 * (lo + hi);
    best = run(mid);
    std::cerr << "speed " << mid << ": " << located(best) << " resident workers against " << total1 << " (" << seconds_since(started) << " s)\n";
    (located(best) < total1 ? lo : hi) = mid;
    if ((hi - lo) / hi < 1e-4) break;
  }
  const double speed = 0.5 * (lo + hi);
  best = run(speed);
  std::vector<double> pred, obs, weight;
  for (const std::size_t i : kept) pred.push_back(std::log(best.households[i] / households0[i])), obs.push_back(std::log(H1[i] / H0[i]));
  double mo = 0, mp = 0;
  for (std::size_t k = 0; k < obs.size(); k++) mo += obs[k] / obs.size(), mp += pred[k] / pred.size();
  double sxy = 0, sxx = 0, syy = 0, sse = 0, uniform = 0;
  const double g = std::log(total1 / total0);
  for (std::size_t k = 0; k < obs.size(); k++) {
    sxy += (pred[k] - mp) * (obs[k] - mo), sxx += (pred[k] - mp) * (pred[k] - mp), syy += (obs[k] - mo) * (obs[k] - mo);
    sse += (obs[k] - pred[k]) * (obs[k] - pred[k]), uniform += (obs[k] - g) * (obs[k] - g);
  }
  js << "\n \"model\": {\"speed\": " << num(speed) << ", \"correlation\": " << num(sxy / std::sqrt(sxx * syy)) << ", \"r2\": " << num(1 - sse / syy) << ", \"r2Uniform\": " << num(1 - uniform / syy)
     << ", \"growthObserved\": " << num(total1 / total0 - 1) << ", \"tracts\": " << obs.size() << "},\n";
  js << " \"seconds\": " << num(seconds_since(started)) << "\n}\n";
  std::ofstream(results + "/summary.json") << js.str();

  std::ofstream tf(results + "/tracts.csv");
  tf << "index,geoid,borough,lat,lon,workers2010,workers2019,jobs2010,jobs2019,access2010,access2019,model2019,stock2019\n";
  for (std::size_t i = 0; i < n; i++)
    tf << i << "," << tracts[i].geoid << "," << tracts[i].borough << "," << tracts[i].lat << "," << tracts[i].lon << "," << H0[i] << "," << H1[i] << "," << E0[i] << "," << E1[i] << "," << num(A0[i]) << ","
       << num(A1[i]) << "," << num(best.households[i]) << "," << num(best.stock[i]) << "\n";
  std::cerr << "model: speed " << speed << ", correlation " << sxy / std::sqrt(sxx * syy) << "; " << seconds_since(started) << " s in all\n";
  return 0;
}
