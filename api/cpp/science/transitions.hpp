// Critical transitions for any work's engine (ADR 0135): kinesis's detectors read on a record of any state space, the test
// models with known answers, and the climate domain's models with folds in nature. A request holds the whole of what it asks;
// nothing is read from a file or kept between requests.
//
// A space:   {units, parts, chart: identity | composition | histogram | covariance, kind: intensive | extensive, with_total,
//             bin_edges, probabilities, reference, unit_names, part_names, edges: [[a, b, weight?]],
//             hierarchy: {levels: [{name, units: [names], parent: [index or name in the level above], edges}]}}
//             (one unit of one part, read as it is, when absent)
// A record:  {times: […], states: [[…], …]} (a native state a time, units × parts unit by unit), or the Dynamics Lab's
//             {names, t, x} (a row for each time), or {series: […], step} for one quantity
// Options:   {window, bandwidth, null_draws, alternative_draws, remaining, robustness: [phase | iaaft | block], surrogate_draws,
//             grid, windows, bandwidths, grid_draws, alpha, seed, rates: [ar1 | increment | red | drift], likelihood,
//             replicates, drift: linear | fold, wells, spatial, hierarchy, max_dimension, power}
//
// Operations (`op`):
//   assess      {space, record, options}   every indicator the space admits, each with what it sees and is blind to, its
//                                          null, its change, significance and operating characteristics, robustness and
//                                          grid, a verdict with its reasons; the dominant eigenvalue's path, where the
//                                          leading mode shows, wells, the likelihood test; what none of them can see
//   likelihood  {times, values, drift, replicates, alpha, seed}   a drifting restoring rate against a constant one
//   wells       {values, step}             the landscape's wells, switches and Kramers's time
//   model       {model, length, step, from, to, sigma, seed, …}   a record with a known answer: fold, transcritical,
//                                          pitchfork, hopf, flip, stationary, rate_tipping, double_well, reddening, network
//   climate     {model: energy_balance | overturning, form, from, to, length, step, sigma, seed}   a record of the climate
//                                          domain's model driven from one forcing to another, with its folds and its true
//                                          restoring rate along the way
//
// The methods are kinesis's and the climate domain's; the engine only reads, calls and writes.
#pragma once

#include "science/json.hpp"

#include <climate/energy_balance/energy_balance.hpp>
#include <climate/overturning/overturning.hpp>
#include <kinesis/transitions/transitions.hpp>

#include <calx/random/stream.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::transitions {

namespace kt = kinesis::transitions;

namespace detail {

inline double number(const Json& j, const char* key, double fallback) { return j[key].is_number() ? j[key].as_number() : fallback; }
inline std::size_t count(const Json& j, const char* key, std::size_t fallback) {
  return j[key].is_number() ? static_cast<std::size_t>(std::max(0.0, j[key].as_number())) : fallback;
}
inline bool flag(const Json& j, const char* key, bool fallback) { return j[key].is_bool() ? j[key].as_bool() : fallback; }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> out;
  if (!a.is_array()) return out;
  for (std::size_t i = 0; i < a.size(); i++) out.push_back(a[i].is_number() ? a[i].as_number() : std::numeric_limits<double>::quiet_NaN());
  return out;
}
inline std::vector<std::string> texts(const Json& a) {
  std::vector<std::string> out;
  if (a.is_array())
    for (std::size_t i = 0; i < a.size(); i++) out.push_back(a[i].text());
  return out;
}
inline Json strings(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& s : v) a.push(s);
  return a;
}
inline Json complexes(const std::vector<std::complex<double>>& v) {
  Json a = Json::array();
  for (const auto& z : v) a.push(Json::array({z.real(), z.imag()}));
  return a;
}

inline kt::Chart chart_of(const std::string& s) {
  if (s == "identity" || s.empty()) return kt::Chart::identity;
  if (s == "composition") return kt::Chart::composition;
  if (s == "histogram") return kt::Chart::histogram;
  if (s == "covariance") return kt::Chart::covariance;
  throw std::invalid_argument("a chart is identity, composition, histogram or covariance, not '" + s + "'");
}

inline kt::Null null_of(const std::string& s) {
  if (s == "phase") return kt::Null::phase;
  if (s == "iaaft") return kt::Null::iaaft;
  if (s == "block") return kt::Null::block;
  if (s == "red") return kt::Null::red;
  throw std::invalid_argument("a further null is phase, iaaft or block, not '" + s + "'");
}

inline kt::RateEstimator rate_of(const std::string& s) {
  if (s == "ar1") return kt::RateEstimator::ar1;
  if (s == "increment") return kt::RateEstimator::increment;
  if (s == "red") return kt::RateEstimator::red;
  if (s == "drift") return kt::RateEstimator::drift;
  throw std::invalid_argument("a rate estimator is ar1, increment, red or drift, not '" + s + "'");
}

inline std::vector<kinesis::scales::Level::Edge> edges_of(const Json& a) {
  std::vector<kinesis::scales::Level::Edge> out;
  if (!a.is_array()) return out;
  for (std::size_t i = 0; i < a.size(); i++) {
    const Json& e = a[i];
    if (!e.is_array() || e.size() < 2) throw std::invalid_argument("an edge is [a, b] or [a, b, weight]");
    out.push_back({static_cast<std::size_t>(e[0].as_number()), static_cast<std::size_t>(e[1].as_number()), e.size() > 2 ? e[2].as_number() : 1.0});
  }
  return out;
}

inline kt::StateSpace space_of(const Json& j) {
  kt::StateSpace s;
  if (!j.is_object()) return s;
  s.units = count(j, "units", 1);
  s.parts = count(j, "parts", 1);
  s.chart = chart_of(j["chart"].text("identity"));
  s.kind = j["kind"].text("intensive") == "extensive" ? kt::Kind::extensive : kt::Kind::intensive;
  s.with_total = flag(j, "with_total", false);
  s.bin_edges = numbers(j["bin_edges"]);
  s.probabilities = numbers(j["probabilities"]);
  s.reference = count(j, "reference", 0);
  s.unit_names = texts(j["unit_names"]);
  s.part_names = texts(j["part_names"]);
  s.edges = edges_of(j["edges"]);
  if (j["hierarchy"].is_object()) {
    const Json& levels = j["hierarchy"]["levels"];
    std::vector<kinesis::scales::Level> ls;
    for (std::size_t l = 0; l < levels.size(); l++) {
      kinesis::scales::Level lv;
      lv.name = levels[l]["name"].text("level " + std::to_string(l + 1));
      lv.units = texts(levels[l]["units"]);
      lv.edges = edges_of(levels[l]["edges"]);
      ls.push_back(lv);
    }
    // Parents by index or by name in the level above.
    for (std::size_t l = 0; l + 1 < ls.size(); l++) {
      const Json& parent = levels[l]["parent"];
      for (std::size_t u = 0; u < parent.size(); u++) {
        if (parent[u].is_number()) ls[l].parent.push_back(static_cast<std::size_t>(parent[u].as_number()));
        else {
          const auto& names = ls[l + 1].units;
          const auto it = std::find(names.begin(), names.end(), parent[u].text());
          if (it == names.end()) throw std::invalid_argument("the parent '" + parent[u].text() + "' is not a unit of " + ls[l + 1].name);
          ls[l].parent.push_back(static_cast<std::size_t>(it - names.begin()));
        }
      }
    }
    s.hierarchy = kinesis::scales::Hierarchy(ls);
    if (s.unit_names.empty()) s.unit_names = ls.front().units;
  }
  return s;
}

inline kt::Record record_of(const Json& j) {
  kt::Record r;
  if (j["series"].is_array()) {
    const auto x = numbers(j["series"]);
    const double dt = number(j, "step", 1);
    for (std::size_t i = 0; i < x.size(); i++) r.times.push_back(static_cast<double>(i) * dt), r.states.push_back({x[i]});
    return r;
  }
  if (j["x"].is_array()) {
    r.times = numbers(j["t"]);
    for (std::size_t i = 0; i < j["x"].size(); i++) r.states.push_back(numbers(j["x"][i]));
    if (r.times.empty())
      for (std::size_t i = 0; i < r.states.size(); i++) r.times.push_back(static_cast<double>(i));
    return r;
  }
  r.times = numbers(j["times"]);
  for (std::size_t i = 0; i < j["states"].size(); i++) {
    const Json& s = j["states"][i];
    if (s.is_number()) r.states.push_back({s.as_number()});
    else r.states.push_back(numbers(s));
  }
  if (r.times.empty())
    for (std::size_t i = 0; i < r.states.size(); i++) r.times.push_back(static_cast<double>(i));
  for (const auto& s : r.states)
    for (double v : s)
      if (!std::isfinite(v)) throw std::invalid_argument("a record's states must all be numbers; fill or drop a missing value first, and say how");
  return r;
}

inline kt::Options options_of(const Json& o) {
  kt::Options out;
  if (!o.is_object()) return out;
  auto& t = out.trend;
  t.window = number(o, "window", t.window);
  t.bandwidth = number(o, "bandwidth", t.bandwidth);
  t.null_draws = count(o, "null_draws", t.null_draws);
  t.alternative_draws = count(o, "alternative_draws", t.alternative_draws);
  t.remaining = number(o, "remaining", t.remaining);
  if (o["robustness"].is_array()) {
    t.robustness.clear();
    for (const auto& n : texts(o["robustness"])) t.robustness.push_back(null_of(n));
  }
  t.surrogate_draws = count(o, "surrogate_draws", t.surrogate_draws);
  t.grid = flag(o, "grid", t.grid);
  if (o["windows"].is_array()) t.windows = numbers(o["windows"]);
  if (o["bandwidths"].is_array()) t.bandwidths = numbers(o["bandwidths"]);
  t.grid_draws = count(o, "grid_draws", t.grid_draws);
  t.alpha = number(o, "alpha", t.alpha);
  t.seed = static_cast<std::uint64_t>(number(o, "seed", 1));
  t.max_windows = count(o, "max_windows", t.max_windows);
  if (o["rates"].is_array()) {
    out.rates.clear();
    for (const auto& r : texts(o["rates"])) out.rates.push_back(rate_of(r));
  }
  out.likelihood = flag(o, "likelihood", out.likelihood);
  out.likelihood_options.replicates = count(o, "replicates", out.likelihood_options.replicates);
  out.likelihood_options.drift = o["drift"].text("linear") == "fold" ? kt::Drift::fold : kt::Drift::linear;
  out.likelihood_options.seed = t.seed;
  out.wells = flag(o, "wells", out.wells);
  out.spatial = flag(o, "spatial", out.spatial);
  out.hierarchy = flag(o, "hierarchy", out.hierarchy);
  out.max_dimension = count(o, "max_dimension", out.max_dimension);
  out.power_required = number(o, "power", out.power_required);
  out.channel_null_draws = count(o, "channel_null_draws", out.channel_null_draws);
  out.channel_alternative_draws = count(o, "channel_alternative_draws", out.channel_alternative_draws);
  return out;
}

inline Json write(const kt::Wells& w) {
  return Json::object({{"wells", static_cast<double>(w.wells)}, {"degree", static_cast<double>(w.degree)}, {"minima", Json(w.minima)}, {"maxima", Json(w.maxima)},
                       {"grid", Json(w.grid)}, {"landscape", Json(w.landscape)}, {"fitted", Json(w.fitted)}, {"switches", static_cast<double>(w.switches)},
                       {"bimodality", w.bimodality}, {"escape_time", std::isfinite(w.escape_time) ? Json(w.escape_time) : Json()}, {"sentence", w.sentence}});
}

inline Json write(const kt::LikelihoodTest& l) {
  auto fit = [](const kt::OuFit& f) {
    return Json::object({{"mean", f.theta}, {"rate", f.kappa0}, {"noise", f.sigma}, {"slope", f.slope}, {"log_likelihood", f.log_likelihood}});
  };
  return Json::object({{"drift", std::string(kt::name(l.drift))}, {"null", fit(l.null)}, {"alternative", fit(l.alternative)}, {"deviance", l.deviance}, {"p", l.p},
                       {"false_alarm", l.false_alarm}, {"detection", l.detection}, {"auc", l.auc}, {"null_deviances", Json(l.null_deviances)},
                       {"alternative_deviances", Json(l.alternative_deviances)}, {"tipping_reported", l.tipping_reported},
                       {"tipping", std::isfinite(l.tipping) ? Json(l.tipping) : Json()}, {"tipping_lower", l.tipping_reported ? Json(l.tipping_lower) : Json()},
                       {"tipping_upper", l.tipping_reported ? Json(l.tipping_upper) : Json()}, {"sentence", l.tipping_sentence}, {"assumptions", strings(l.assumptions)}});
}

inline Json write(const kt::Assessment& a) {
  Json findings = Json::array();
  for (const auto& f : a.findings) {
    const auto& t = f.trend;
    Json robust = Json::array(), grid = Json::array();
    for (const auto& [n, p] : t.robustness) robust.push(Json::object({{"null", n}, {"p", p}}));
    for (const auto& c : t.grid)
      grid.push(Json::object({{"window", c.window}, {"bandwidth", c.bandwidth}, {"tau", c.tau}, {"change", c.change}, {"p", c.p}, {"p_reverse", c.p_reverse}}));
    std::vector<double> series;
    for (double v : t.series) series.push_back(v);
    Json s = Json::array();
    for (double v : series) s.push(std::isfinite(v) ? Json(v) : Json());
    findings.push(Json::object({{"channel", f.channel}, {"indicator", f.indicator}, {"estimates", f.estimates}, {"direction", std::string(kt::name(f.direction))},
                                {"sees", strings(f.sees)}, {"blind", strings(f.blind)}, {"assumes", strings(f.assumes)}, {"null", f.null},
                                {"at", Json(t.at)}, {"series", s}, {"tau", t.tau}, {"change", t.change}, {"positive", t.positive}, {"p", t.p},
                                {"false_alarm", t.false_alarm}, {"detection", t.detection}, {"auc", t.auc}, {"robustness", robust}, {"grid", grid},
                                {"grid_significant", t.grid_significant}, {"grid_flips", t.grid_flips}, {"verdict", std::string(kt::name(f.verdict))},
                                {"reasons", strings(f.reasons)}}));
  }
  Json channels = Json::array();
  for (const auto& c : a.channels) channels.push(Json::object({{"name", c.name}, {"joint_p", c.joint_p}, {"coordinates", static_cast<double>(c.coordinates)}}));
  Json out = Json::object({{"space", a.space}, {"states", static_cast<double>(a.states)}, {"coordinates", static_cast<double>(a.coordinates)}, {"step", a.step},
                           {"window", static_cast<double>(a.window)}, {"overall", std::string(kt::name(a.overall))}, {"summary", strings(a.summary)},
                           {"cannot_see", strings(a.cannot_see)}, {"refused", strings(a.refused)}, {"findings", findings}, {"channels", channels}});
  if (!a.kind.path.empty()) {
    const char* approach = a.kind.approach == kt::Approach::stationary ? "stationary"
                           : a.kind.approach == kt::Approach::flip      ? "flip"
                           : a.kind.approach == kt::Approach::oscillatory ? "oscillatory"
                                                                          : "undetermined";
    out.set("kind", Json::object({{"approach", std::string(approach)}, {"described", std::string(kt::name(a.kind.approach))}, {"sentence", a.kind.sentence},
                                  {"at", Json(a.kind.at)}, {"path", complexes(a.kind.path)}, {"tau_modulus", a.kind.tau_modulus}, {"change_modulus", a.kind.change_modulus},
                                  {"frequency", a.kind.frequency}, {"shape", a.kind.shape}}));
  }
  if (a.where) out.set("where", Json::object({{"units", strings(a.where->units)}, {"loading", Json(a.where->loading)}, {"excitation", Json(a.where->excitation)}, {"sentence", a.where->sentence}}));
  if (a.wells) out.set("wells", write(*a.wells));
  if (a.likelihood) out.set("likelihood", write(*a.likelihood));
  if (a.noise)
    out.set("noise", Json::object({{"red", a.noise->red_noise}, {"rate", a.noise->kappa}, {"noise_rate", a.noise->noise_rate},
                                   {"fit_red", a.noise->fit_red}, {"fit_white", a.noise->fit_white}, {"assumptions", strings(a.noise->assumptions)}}));
  return out;
}

inline Json record_json(const std::vector<double>& times, const std::vector<std::vector<double>>& states) {
  Json s = Json::array();
  for (const auto& v : states) s.push(Json(v));
  return Json::object({{"times", Json(times)}, {"states", s}});
}

inline Json scalar_json(const std::vector<double>& times, const std::vector<double>& x) {
  Json s = Json::array();
  for (double v : x) s.push(Json::array({v}));
  return Json::object({{"times", Json(times)}, {"states", s}});
}

}  // namespace detail

// ---- operations -------------------------------------------------------------------------------------------------

inline Json assess(const Json& r) {
  const kt::StateSpace space = detail::space_of(r["space"]);
  const kt::Record record = detail::record_of(r["record"]);
  return detail::write(kt::assess(space, record, detail::options_of(r["options"])));
}

inline Json likelihood(const Json& r) {
  kt::LikelihoodOptions o;
  o.drift = r["drift"].text("linear") == "fold" ? kt::Drift::fold : kt::Drift::linear;
  o.replicates = detail::count(r, "replicates", 100);
  o.alpha = detail::number(r, "alpha", 0.05);
  o.seed = static_cast<std::uint64_t>(detail::number(r, "seed", 1));
  return detail::write(kt::likelihood_test(detail::numbers(r["times"]), detail::numbers(r["values"]), o));
}

inline Json wells(const Json& r) { return detail::write(kt::wells(detail::numbers(r["values"]), detail::number(r, "step", 1))); }

inline Json model(const Json& r) {
  const std::string m = r["model"].text("fold");
  const std::size_t T = detail::count(r, "length", 600);
  const double dt = detail::number(r, "step", 0.2), sigma = detail::number(r, "sigma", 0.05);
  calx::random::Stream rng(static_cast<std::uint64_t>(detail::number(r, "seed", 1)), 0x6d6f646cu, 0, 0);  // 'modl'
  const auto times = kt::models::times(T, dt);
  Json truth = Json::object({{"model", m}});
  if (m == "fold" || m == "transcritical" || m == "pitchfork") {
    const double a = detail::number(r, "from", m == "fold" ? 1.0 : -1.0), b = detail::number(r, "to", m == "fold" ? 0.02 : -0.02);
    const auto x = m == "fold" ? kt::models::fold(T, dt, a, b, sigma, rng) : m == "transcritical" ? kt::models::transcritical(T, dt, a, b, sigma, rng) : kt::models::pitchfork(T, dt, a, b, sigma, rng);
    truth.set("rate_from", m == "fold" ? 2 * std::sqrt(a) : std::abs(a));
    truth.set("rate_to", m == "fold" ? 2 * std::sqrt(std::max(0.0, b)) : std::abs(b));
    truth.set("expected", "a stationary loss of stability: slowing down, the dominant eigenvalue going to +1");
    return Json::object({{"record", detail::scalar_json(times, x)}, {"truth", truth}});
  }
  if (m == "hopf") {
    const auto z = kt::models::hopf(T, dt, detail::number(r, "from", -1.0), detail::number(r, "to", -0.05), detail::number(r, "omega", 2.0), sigma, rng);
    truth.set("expected", "an oscillatory loss of stability: the dominant eigenvalue going to the circle off the real axis");
    return Json::object({{"record", detail::record_json(times, z)}, {"truth", truth}, {"space", Json::object({{"parts", 2.0}, {"part_names", Json::array({"x", "y"})}})}});
  }
  if (m == "flip") {
    const auto x = kt::models::flip(T, detail::number(r, "from", 0.6), detail::number(r, "to", 0.03), sigma, rng);
    truth.set("expected", "a period-doubling: the lag-1 autocorrelation falling to −1, the dominant eigenvalue going to −1");
    return Json::object({{"record", detail::scalar_json(kt::models::times(T, 1.0), x)}, {"truth", truth}});
  }
  if (m == "stationary") {
    const double k = detail::number(r, "rate", 0.5);
    const auto x = kt::models::ou(times, [k](double) { return k; }, sigma, rng);
    truth.set("expected", "nothing: false alarms at the stated level");
    return Json::object({{"record", detail::scalar_json(times, x)}, {"truth", truth}});
  }
  if (m == "rate_tipping") {
    const double rr = detail::number(r, "rate", 0.6);
    const auto x = kt::models::rate_tipping(T, dt, rr, sigma, rng);
    truth.set("expected", rr < 1 ? "nothing slows: the stable state's rate 2√(1 − r) is constant" : "it tips with nothing slowing: rate-induced tipping");
    return Json::object({{"record", detail::scalar_json(times, x)}, {"truth", truth}});
  }
  if (m == "double_well") {
    const auto x = kt::models::double_well(T, dt, sigma, rng);
    truth.set("expected", "two wells and switching between them, at fixed parameters: no slowing down");
    return Json::object({{"record", detail::scalar_json(times, x)}, {"truth", truth}});
  }
  if (m == "reddening") {
    const auto x = kt::models::reddening(T, dt, detail::number(r, "rate", 0.5), detail::number(r, "from", 8.0), detail::number(r, "to", 1.0), sigma, rng);
    truth.set("expected", "variance and autocorrelation rising with the restoring rate unchanged: a change of noise, not slowing down");
    return Json::object({{"record", detail::scalar_json(times, x)}, {"truth", truth}});
  }
  if (m == "network") {
    const std::size_t n = detail::count(r, "units", 8);
    std::vector<std::pair<std::size_t, std::size_t>> ring;
    Json edges = Json::array();
    for (std::size_t i = 0; i < n; i++) ring.push_back({i, (i + 1) % n}), edges.push(Json::array({static_cast<double>(i), static_cast<double>((i + 1) % n)}));
    const double g0 = detail::number(r, "from", 1.0), g1 = detail::number(r, "to", 0.05), coupling = detail::number(r, "coupling", 0.3);
    const bool one = r["where"].is_number();
    const std::size_t u0 = one ? static_cast<std::size_t>(r["where"].as_number()) : 0;
    const double duration = static_cast<double>(T - 1) * dt;
    const auto z = kt::models::network_ou(T, dt, n, ring, coupling, [&](double t) {
      std::vector<double> g(n, g0);
      const double v = g0 + (g1 - g0) * t / duration;
      if (one) g[u0] = v;
      else std::fill(g.begin(), g.end(), v);
      return g;
    }, sigma, rng);
    truth.set("expected", one ? "the leading mode concentrating at the unit whose rate falls" : "a slowing uniform over the units, which shows in their mean and not their spread");
    std::vector<std::string> names;
    for (std::size_t i = 0; i < n; i++) names.push_back("unit " + std::to_string(i + 1));
    return Json::object({{"record", detail::record_json(times, z)}, {"truth", truth},
                         {"space", Json::object({{"units", static_cast<double>(n)}, {"edges", edges}, {"unit_names", detail::strings(names)}})}});
  }
  throw std::invalid_argument("a test model is fold, transcritical, pitchfork, hopf, flip, stationary, rate_tipping, double_well, reddening or network, not '" + m + "'");
}

inline Json climate(const Json& r) {
  const std::string m = r["model"].text("overturning");
  const std::size_t T = detail::count(r, "length", 800);
  const double sigma = detail::number(r, "sigma", 0.02);
  calx::random::Stream rng(static_cast<std::uint64_t>(detail::number(r, "seed", 1)), 0x636c696du, 0, 0);  // 'clim'
  if (m == "overturning") {
    const auto c = climate::overturning::cessi();
    const auto f = climate::overturning::folds(c);
    const double dt = detail::number(r, "step", 0.05), p0 = detail::number(r, "from", 1.0), p1 = detail::number(r, "to", 1.46);
    const double duration = static_cast<double>(T - 1) * dt;
    const auto eq0 = climate::overturning::equilibria(c, p0);
    const double y0 = r["branch"].text("lower") == "upper" ? eq0.back().y : eq0.front().y;
    const auto x = kt::models::sde(T, dt, y0, [&](double y, double t) { return climate::overturning::tendency(c, p0 + (p1 - p0) * t / duration, y); }, sigma, rng);
    std::vector<double> rates;
    for (std::size_t i = 0; i < T; i++) {
      const double p = p0 + (p1 - p0) * static_cast<double>(i) / static_cast<double>(T - 1);
      const auto e = climate::overturning::equilibria(c, p);
      const bool upper = r["branch"].text("lower") == "upper";
      const auto& eq = upper ? e.back() : e.front();
      rates.push_back(eq.rate);
    }
    return Json::object({{"record", detail::scalar_json(kt::models::times(T, dt), x)},
                         {"folds", Json::object({{"p_lower", f.p_lower}, {"p_upper", f.p_upper}, {"y_upper", f.y_upper}, {"y_lower", f.y_lower}})},
                         {"true_rate", Json(rates)}, {"unit_of_time", "the diffusive time τ_d (≈ 219 years, Cessi's estimate)"},
                         {"forcing", Json::array({p0, p1})}});
  }
  if (m == "energy_balance") {
    const auto form = r["form"].text("smooth") == "ramp" ? climate::energy_balance::Form::ramp : climate::energy_balance::Form::smooth;
    const auto b = climate::energy_balance::common(form);
    const auto f = climate::energy_balance::folds(b);
    const double dt = detail::number(r, "step", 1.0), q0 = detail::number(r, "from", f.warm_lost + 6), q1 = detail::number(r, "to", f.warm_lost + 0.5);
    const double duration = static_cast<double>(T - 1) * dt;
    const double T0 = climate::energy_balance::equilibria(b, q0).back().T;
    const auto x = kt::models::sde(T, dt, T0, [&](double temp, double t) { return climate::energy_balance::tendency(b, q0 + (q1 - q0) * t / duration, temp); }, sigma, rng);
    std::vector<double> rates;
    for (std::size_t i = 0; i < T; i++) {
      const double q = q0 + (q1 - q0) * static_cast<double>(i) / static_cast<double>(T - 1);
      const auto e = climate::energy_balance::equilibria(b, q);
      rates.push_back(e.empty() ? std::numeric_limits<double>::quiet_NaN() : e.back().rate);
    }
    return Json::object({{"record", detail::scalar_json(kt::models::times(T, dt), x)},
                         {"folds", Json::object({{"warm_lost", f.warm_lost}, {"cold_lost", f.cold_lost}, {"T_warm", f.T_warm}, {"T_cold", f.T_cold}})},
                         {"true_rate", Json(rates)}, {"unit_of_time", "years"}, {"forcing", Json::array({q0, q1})},
                         {"form", form == climate::energy_balance::Form::ramp ? "ramp" : "smooth"}});
  }
  throw std::invalid_argument("a climate model is energy_balance or overturning, not '" + m + "'");
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text("assess");
  if (op == "assess") return assess(r);
  if (op == "likelihood") return likelihood(r);
  if (op == "wells") return wells(r);
  if (op == "model") return model(r);
  if (op == "climate") return climate(r);
  throw std::invalid_argument("an operation of critical transitions is assess, likelihood, wells, model or climate, not '" + op + "'");
}

}  // namespace science::transitions
