// The engine of the plug-in "What people pay for" (ADR 0088): a hedonic model of a case's home prices with the spatial dependence they
// carry (urban science's demand on tyche's spatial regression), and the plan of a stated-choice survey about a street (peira's designs,
// nomos's conditional logit). Nothing in it names a city: a case is a file of homes sold (data/products/home_prices.py).
//
// It answers requests, one JSON object a line, with one JSON line each: with --serve until its input ends; --data names the directory of
// the data library's products, of which it reads home-prices/.
//
//   cases   {}                                the cases built (home-prices/index.json)
//   hedonic {case, neighbours?, sample?, seed?}
//                         ln price on floor, land, age, age², class, the subway, parks, a protected lane and the centre: least squares,
//                         the LM tests, the rule's model, both spatial models, the lag model's impacts, and each home's residual
//   survey  {attributes: [{name, levels | values}], prior: [...], price, sets?, alternatives?, half_width?, seed?}
//                         a design at the priors (coordinate exchange) and the respondents each willingness to pay needs
//   status  {}

#include <urban/demand/hedonic.hpp>
#include <urban/demand/stated.hpp>

#include <science/json.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

namespace {

using science::Json;

std::string& data_dir() { static std::string d = "."; return d; }
double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
Json finite(double v) { return std::isfinite(v) ? Json(v) : Json(); }

std::string slurp(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("the data library has no " + path + ": python data/products/home_prices.py <case>");
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

const char* NAMES[] = {"intercept", "floor area (log)", "land area (log)", "age", "age² / 100", "two-family", "three-family", "distance to the subway (log)",
                       "distance to a park (log)", "a protected lane within 300 m", "distance to the centre (log)"};

Json vec(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(finite(x));
  return a;
}

Json op_hedonic(const Json& r) {
  const std::string name = r["case"].text();
  for (const char c : name)
    if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '-')) throw std::invalid_argument("a case's name is letters, digits and hyphens");
  const Json rec = Json::parse(slurp(data_dir() + "/home-prices/" + name + ".json"));
  const Json& C = rec["columns"];
  const auto col = [&](const char* key) {
    for (std::size_t i = 0; i < C.size(); i++)
      if (C[i].text() == key) return i;
    throw std::invalid_argument(std::string("the case has no column ") + key);
  };
  const std::size_t ix = col("x"), iy = col("y"), ip = col("log_price"), ifl = col("log_floor"), ila = col("log_land"), iag = col("age"), icl = col("class"),
                    isu = col("log_subway"), ipa = col("log_park"), iln = col("lane_300m"), ice = col("log_centre"), ilat = col("lat"), ilon = col("lon");
  const Json& R = rec["records"];
  std::vector<std::size_t> rows(R.size());
  std::iota(rows.begin(), rows.end(), std::size_t{0});
  const std::size_t sample = static_cast<std::size_t>(number_or(r, "sample", 2000));
  if (sample > 0 && sample < rows.size()) {
    calx::random::Stream st(static_cast<std::uint64_t>(number_or(r, "seed", 20261109)), 0x686f6d65u, 0, 0);  // "home"
    for (std::size_t i = 0; i < sample; i++) std::swap(rows[i], rows[i + st.below(rows.size() - i)]);
    rows.resize(sample);
  }
  const std::size_t n = rows.size(), k = 11;
  calx::linalg::Rectangular X(n, k);
  std::vector<double> y(n), px(n), py(n);
  for (std::size_t q = 0; q < n; q++) {
    const Json& h = R[rows[q]];
    const double age = h[iag].as_number(), cls = h[icl].as_number();
    const double v[] = {1, h[ifl].as_number(), h[ila].as_number(), age, age * age / 100, cls == 2 ? 1.0 : 0.0, cls == 3 ? 1.0 : 0.0, h[isu].as_number(), h[ipa].as_number(),
                        h[iln].as_number(), h[ice].as_number()};
    for (std::size_t c = 0; c < k; c++) X(q, c) = v[c];
    y[q] = h[ip].as_number(), px[q] = h[ix].as_number(), py[q] = h[iy].as_number();
  }
  urban::demand::HedonicOptions o;
  o.neighbours = static_cast<std::size_t>(number_or(r, "neighbours", 8));
  const auto f = urban::demand::hedonic(X, y, px, py, o);
  Json names = Json::array();
  for (const char* nm : NAMES) names.push(Json(nm));
  const auto model = [&](const std::vector<double>& b, const std::vector<double>& se) { return Json::object({{"beta", vec(b)}, {"se", vec(se)}}); };
  Json homes = Json::array();
  for (std::size_t q = 0; q < n; q++) {
    const Json& h = R[rows[q]];
    homes.push(Json::array({h[ilat], h[ilon], Json(std::round(f.residuals[q] * 1e4) / 1e4), Json(std::round(y[q] * 1e4) / 1e4)}));
  }
  const char* chosen = f.chosen == urban::demand::SpatialModel::lag ? "lag" : f.chosen == urban::demand::SpatialModel::error ? "error" : "least squares";
  return Json::object({
      {"case", Json(name)}, {"city", rec["city"]}, {"question", rec["question"]}, {"year", rec["year"]}, {"median_price", rec["median_price"]}, {"homes", Json(static_cast<double>(n))},
      {"of", Json(static_cast<double>(R.size()))}, {"neighbours", Json(static_cast<double>(o.neighbours))}, {"names", names}, {"chosen", Json(chosen)},
      {"ols", Json::object({{"beta", vec(f.ols.beta)}, {"se", vec(f.ols.se)}, {"r2", Json(f.ols.r_squared)}, {"aic", Json(f.ols.aic)}})},
      {"lag", Json::object({{"beta", vec(f.lag.beta)}, {"se", vec(f.lag.se)}, {"rho", Json(f.lag.parameter)}, {"rho_se", Json(f.lag.parameter_se)}, {"aic", Json(f.lag.aic)}})},
      {"error", Json::object({{"beta", vec(f.error.beta)}, {"se", vec(f.error.se)}, {"lambda", Json(f.error.parameter)}, {"lambda_se", Json(f.error.parameter_se)}, {"aic", Json(f.error.aic)}})},
      {"chosen_fit", model(f.beta, f.se)},
      {"impacts", Json::object({{"direct", vec(f.impacts.direct)}, {"indirect", vec(f.impacts.indirect)}, {"total", vec(f.impacts.total)}})},
      {"moran", Json::object({{"I", Json(f.moran)}, {"p", Json(f.moran_p)}})},
      {"tests", Json::object({{"lm_lag", Json(f.tests.lm_lag)}, {"p_lag", Json(f.tests.p_lag)}, {"lm_error", Json(f.tests.lm_error)}, {"p_error", Json(f.tests.p_error)},
                              {"robust_lag", Json(f.tests.robust_lag)}, {"p_robust_lag", Json(f.tests.p_robust_lag)}, {"robust_error", Json(f.tests.robust_error)},
                              {"p_robust_error", Json(f.tests.p_robust_error)}})},
      {"homes_drawn", homes}, {"source", rec["source"]}});
}

Json op_survey(const Json& r) {
  std::vector<peira::design::ChoiceAttribute> at;
  const Json& A = r["attributes"];
  for (std::size_t i = 0; i < A.size(); i++) {
    peira::design::ChoiceAttribute a;
    a.name = A[i]["name"].text();
    if (A[i].has("values"))
      for (std::size_t j = 0; j < A[i]["values"].size(); j++) a.values.push_back(A[i]["values"][j].as_number());
    else a.levels = static_cast<std::size_t>(A[i]["levels"].as_number());
    at.push_back(a);
  }
  std::vector<double> prior;
  for (std::size_t i = 0; i < r["prior"].size(); i++) prior.push_back(r["prior"][i].as_number());
  calx::random::Stream st(static_cast<std::uint64_t>(number_or(r, "seed", 20261109)), 0x73757276u, 0, 0);  // "surv"
  const auto p = urban::demand::plan_survey(at, static_cast<std::size_t>(number_or(r, "sets", 12)), static_cast<std::size_t>(number_or(r, "alternatives", 3)), prior,
                                            static_cast<std::size_t>(r["price"].as_number()), number_or(r, "half_width", 5), st, 8);
  Json cards = Json::array();
  for (std::size_t s = 0; s < p.design.sets; s++) {
    Json set = Json::array();
    for (std::size_t j = 0; j < p.design.alternatives; j++) {
      Json alt = Json::array();
      for (std::size_t a = 0; a < at.size(); a++) alt.push(Json(static_cast<double>(p.design.at(s, j, a))));
      set.push(alt);
    }
    cards.push(set);
  }
  return Json::object({{"cards", cards}, {"d_error", Json(p.design.d_error)}, {"wtp", vec(p.wtp)}, {"wtp_se_one", vec(p.wtp_se_one)}, {"respondents", vec(p.respondents)},
                       {"half_width", Json(p.half_width)}});
}

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "cases") return Json::parse(slurp(data_dir() + "/home-prices/index.json"));
  if (op == "hedonic") return op_hedonic(r);
  if (op == "survey") return op_survey(r);
  if (op == "status") return Json::object({{"ok", true}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

std::string respond(const std::string& line) {
  Json reply = Json::object();
  try {
    const Json r = Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", answer(r));
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", false}, {"error", e.what()}});
  }
  return reply.dump();
}

}  // namespace

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  bool serve = false;
  std::string request;
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--serve") == 0) serve = true;
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) data_dir() = argv[++i];
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) continue;
      std::cout << respond(line) << "\n" << std::flush;
    }
    return 0;
  }
  if (request.empty()) std::getline(std::cin, request);
  std::cout << respond(request) << "\n";
  return 0;
}
