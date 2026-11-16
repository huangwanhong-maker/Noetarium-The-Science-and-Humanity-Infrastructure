// Population dynamics, for any work's engine (ADR 0124): a place's population recorded cell by cell at a few times,
// learned as balance laws of a field on its cells (kinesis's balance laws), judged on the times kept aside, and run ahead.
// A request holds the whole field; nothing is read from a file or kept between requests.
//
// The field, noetarium.population-field/1 (data/products/population_history.py writes it):
//   {grid: {rows, cols, dx /* metres */}, land: [[0|1 …] …] /* rows from the north */, epochs: [years],
//    projected: [years that are projections, not records], values: {"<year>": [[people per cell | null …] …]}}
// Cells are the land cells in row-major order; a density is people per km², time is in years, distance in km.
//
// Covariates, for drift and sources: `centre`, minus the distance in km from the grid's middle (a drift up it is a
// movement toward the centre, at a speed in km a year equal to its coefficient); `settled`, ln(1 + u) at the first time
// (a drift up it is toward where people were). A request may add its own as {"name": [a value per land cell]}.
//
//   growth    the whole place's people over time, as one cell: exponential, logistic and Gompertz's growth, each fitted
//             on the times before those kept aside (`held_out`, 2 by default), judged on them, fitted again on every time,
//             and run to `to` (2050), year by year
//   learn     laws of the field: a rate of its own for each cell, growth everywhere at one rate, logistic growth, growth
//             and spread (Fisher's law), spread and drift toward the centre, crowding and drift, spread and a drift along the
//             settled landscape, a law thresholded from the whole library, and the record's modes; each fitted on the times before those kept aside (by the integral
//             form, then shooting), forecast to them, and judged against carrying the last field forward; the forecasts
//             of the best, and every model's currents at the last time fitted. `models` names a few; `law` declares one.
//   modes     the record's dynamic modes (kinesis's modes): eigenvalues, rates a year, how much each carries, their maps
//   simulate  a law (terms and coefficients, or a model by name fitted on every time recorded) run from `from` (the last
//             year recorded) to `to` every `every` years (5), with `sources` [{row, col, people_per_year}] added, its
//             fields, totals and currents; and the difference from the record's own projections where there are some
//
// The methods are kinesis's; the engine only reads, calls and writes.
#pragma once

#include "json.hpp"

#include <kinesis/balance/balance.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::population {

namespace kf = kinesis::balance;

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }
inline double round1(double x) { return std::round(x * 10) / 10; }

struct Place {
  kf::Grid grid;
  std::vector<double> epochs;                  // every epoch in the field
  std::vector<char> projected;                 // per epoch
  std::vector<std::vector<double>> fields;     // per epoch, per land cell
  std::map<std::string, std::vector<double>> covariates;
  std::vector<std::string> covariate_names;    // in the order the laws index them
};

inline Place read_field(const Json& f, const Json& extra = Json()) {
  if (!f.is_object()) throw std::invalid_argument("the request has no field");
  const Json& g = f["grid"];
  const std::size_t rows = static_cast<std::size_t>(g["rows"].as_number()), cols = static_cast<std::size_t>(g["cols"].as_number());
  const double dx = g["dx"].is_number() ? g["dx"].as_number() / 1000.0 : 1.0;
  std::vector<char> mask(rows * cols, 1);
  if (f["land"].is_array()) {
    if (f["land"].size() != rows) throw std::invalid_argument("the field's land has " + std::to_string(f["land"].size()) + " rows, not " + std::to_string(rows));
    for (std::size_t i = 0; i < rows; i++)
      for (std::size_t j = 0; j < cols; j++) mask[i * cols + j] = f["land"][i][j].is_number() && f["land"][i][j].as_number() != 0;
  }
  Place p;
  p.grid = kf::grid(rows, cols, mask, dx);
  std::vector<double> proj;
  for (std::size_t k = 0; k < f["projected"].size(); k++) proj.push_back(f["projected"][k].as_number());
  for (std::size_t k = 0; k < f["epochs"].size(); k++) {
    const double year = f["epochs"][k].as_number();
    char key[32];
    std::snprintf(key, sizeof key, "%g", year);
    const Json& v = f["values"][std::string_view(key)];
    if (!v.is_array()) throw std::invalid_argument(std::string("the field has no values for ") + key);
    std::vector<double> u(p.grid.graph.size(), 0.0);
    for (std::size_t c = 0; c < u.size(); c++) {
      const Json& x = v[p.grid.row[c]][p.grid.col[c]];
      u[c] = x.is_number() ? std::max(0.0, x.as_number() / (dx * dx)) : 0.0;  // people per cell to people per km²
    }
    p.epochs.push_back(year);
    p.projected.push_back(std::find(proj.begin(), proj.end(), year) != proj.end());
    p.fields.push_back(std::move(u));
  }
  if (p.epochs.size() < 3) throw std::invalid_argument("the field needs at least three times");
  const double mi = (static_cast<double>(rows) - 1) / 2, mj = (static_cast<double>(cols) - 1) / 2;
  std::vector<double> centre(p.grid.graph.size()), settled(p.grid.graph.size());
  for (std::size_t c = 0; c < centre.size(); c++) {
    centre[c] = -dx * std::hypot(static_cast<double>(p.grid.row[c]) - mi, static_cast<double>(p.grid.col[c]) - mj);
    settled[c] = std::log1p(p.fields[0][c]);
  }
  p.covariates["centre"] = centre, p.covariates["settled"] = settled;
  if (extra.is_object())
    for (const auto& [name, vals] : extra.as_object()) {
      std::vector<double> v;
      for (std::size_t c = 0; c < vals.size(); c++) v.push_back(vals[c].as_number());
      if (v.size() != p.grid.graph.size()) throw std::invalid_argument("the covariate " + name + " has " + std::to_string(v.size()) + " values for " + std::to_string(p.grid.graph.size()) + " land cells");
      p.covariates[name] = v;
    }
  for (const auto& [name, v] : p.covariates) p.covariate_names.push_back(name);
  return p;
}

inline kf::Covariates covariates_of(const Place& p) {
  kf::Covariates c;
  for (const auto& n : p.covariate_names) c.push_back(p.covariates.at(n));
  return c;
}

// "linear", "drift:centre", "source:settled".
inline kf::Term term_of(const Place& p, const std::string& s) {
  const auto colon = s.find(':');
  kf::Term t{kf::kind_of(s.substr(0, colon)), 0};
  if (t.kind == kf::Kind::drift || t.kind == kf::Kind::source) {
    const std::string cov = colon == std::string::npos ? "centre" : s.substr(colon + 1);
    const auto it = std::find(p.covariate_names.begin(), p.covariate_names.end(), cov);
    if (it == p.covariate_names.end()) throw std::invalid_argument("there is no covariate " + cov);
    t.covariate = static_cast<std::size_t>(it - p.covariate_names.begin());
  } else if (colon != std::string::npos) {
    throw std::invalid_argument("the term " + s.substr(0, colon) + " takes no covariate");
  }
  return t;
}

inline std::string term_text(const Place& p, const kf::Term& t) {
  std::string s = kf::name(t.kind);
  if (t.kind == kf::Kind::drift || t.kind == kf::Kind::source) s += ":" + p.covariate_names[t.covariate];
  return s;
}

// The record: the epochs recorded (not projected) up to `until`.
inline kf::Record record_of(const Place& p, double until) {
  kf::Record r;
  for (std::size_t k = 0; k < p.epochs.size(); k++)
    if (!p.projected[k] && p.epochs[k] <= until) r.t.push_back(p.epochs[k]), r.u.push_back(p.fields[k]);
  if (r.t.size() < 3) throw std::invalid_argument("fewer than three recorded times up to " + std::to_string(static_cast<int>(until)));
  return r;
}

inline double until_of(const Json& r, const Place& p) {
  if (r["until"].is_number()) return r["until"].as_number();
  double u = p.epochs.front();
  for (std::size_t k = 0; k < p.epochs.size(); k++)
    if (!p.projected[k]) u = std::max(u, p.epochs[k]);
  return u;
}

inline Json field_json(const std::vector<double>& u) {
  Json a = Json::array();
  for (double v : u) a.push(round1(v));
  return a;
}

inline Json currents_json(const kf::Currents& c) {
  Json e = Json::array(), n = Json::array();
  for (std::size_t i = 0; i < c.east.size(); i++) e.push(round1(c.east[i])), n.push(round1(c.north[i]));
  return Json::object({{"east", e}, {"north", n}, {"unit", "people a year"}});
}

// What the coefficients say, in the quantities a reader knows.
inline Json reading(const Place& p, const kf::Law& law, const std::vector<double>& se) {
  Json out = Json::object();
  double a = 0, b = 0, g = 0, D = 0;
  bool hasa = false, hasb = false, hasg = false, hasD = false;
  for (std::size_t k = 0; k < law.terms.size(); k++) {
    const double th = law.coefficients[k];
    if (th == 0) continue;
    const kf::Term& t = law.terms[k];
    switch (t.kind) {
      case kf::Kind::linear: a = th, hasa = true; break;
      case kf::Kind::quadratic: b = th, hasb = true; break;
      case kf::Kind::gompertz: g = th, hasg = true; break;
      case kf::Kind::diffusion: D = th, hasD = true; out.set("diffusivity_km2_a_year", num(th)); out.set("diffusivity_se", num(se[k])); break;
      case kf::Kind::crowding: out.set("crowding_km2_a_year_per_density", num(th)); out.set("crowding_se", num(se[k])); break;
      case kf::Kind::drift:
        if (p.covariate_names[t.covariate] == "centre") out.set("speed_toward_centre_km_a_year", num(th)), out.set("speed_toward_centre_se", num(se[k]));
        else out.set("drift_up_" + p.covariate_names[t.covariate], num(th)), out.set("drift_up_" + p.covariate_names[t.covariate] + "_se", num(se[k]));
        break;
      case kf::Kind::constant: out.set("made_everywhere_per_km2_a_year", num(th)); break;
      default: out.set("made_in_proportion_to_" + p.covariate_names[t.covariate], num(th)); break;
    }
  }
  if (hasa && hasg) {
    out.set("growth_rate_a_year", num(-g));
    out.set(g < 0 ? "ceiling_per_km2" : "floor_per_km2", num(std::exp(-a / g)));  // g > 0: growth that runs away from e^{-a/g}
    out.set("form", "Gompertz");
    if (g > 0) out.set("accelerating", true);
  } else if (hasa && hasb) {
    out.set("growth_rate_a_year", num(a));
    out.set("ceiling_per_km2", b < 0 ? num(-a / b) : Json(nullptr));
    out.set("form", "logistic");
    if (b > 0) out.set("accelerating", true);
  } else if (hasa) {
    out.set("growth_rate_a_year", num(a));
    out.set("form", "exponential");
  }
  if (hasD && hasa && a > 0 && D > 0) out.set("front_speed_km_a_year", num(2 * std::sqrt(a * D)));  // Fisher's 2√(rD)
  return out;
}

struct Model {
  std::string name, title;
  std::vector<std::string> terms;  // empty: a baseline
  double threshold = 0;
};

inline std::vector<Model> models_default() {
  return {
      {"persistence", "Each place as it was", {}, 0},
      {"own-rates", "Each place at a rate of its own", {}, 0},
      {"growth", "Growth everywhere at one rate", {"linear"}, 0},
      {"logistic", "Logistic growth everywhere", {"linear", "quadratic"}, 0},
      {"fisher", "Growth and spread (Fisher's law)", {"linear", "quadratic", "diffusion"}, 0},
      {"drift", "Growth, spread and drift toward the centre", {"linear", "quadratic", "diffusion", "drift:centre"}, 0},
      {"crowding", "Growth, crowding and drift toward the centre", {"linear", "quadratic", "crowding", "drift:centre"}, 0},
      {"settled", "Growth, spread and a drift along the settled landscape", {"linear", "quadratic", "diffusion", "drift:settled"}, 0},
      {"sparse", "A law found among every term", {"constant", "linear", "quadratic", "gompertz", "diffusion", "crowding", "drift:centre", "drift:settled"}, 0.05},
      {"modes", "The record's modes, carried on", {}, 0},
  };
}

struct Learned {
  kf::Estimate integral, shot;
  bool ok = true;
  std::string error;
};

inline Learned learn_law(const Place& p, const std::vector<std::string>& names, double threshold, const kf::Record& rec, const kf::LearnOptions& base) {
  Learned l;
  std::vector<kf::Term> terms;
  for (const auto& n : names) terms.push_back(term_of(p, n));
  const kf::Covariates cov = covariates_of(p);
  kf::LearnOptions o = base;
  o.threshold = threshold;
  l.integral = kf::integral(p.grid.graph, terms, rec, cov, o);
  try {
    l.shot = kf::shooting(p.grid.graph, l.integral, rec, cov, o);
  } catch (const std::exception& e) {
    l.shot = l.integral;
    l.error = std::string("shooting failed (") + e.what() + "); the integral form's law is kept";
  }
  return l;
}

inline Json law_json(const Place& p, const kf::Law& law, const std::vector<double>& se) {
  Json terms = Json::array();
  for (std::size_t k = 0; k < law.terms.size(); k++)
    terms.push(Json::object({{"term", term_text(p, law.terms[k])}, {"symbol", kf::symbol(law.terms[k], p.covariate_names)}, {"coefficient", num(law.coefficients[k])},
                             {"se", num(k < se.size() ? se[k] : 0.0)}}));
  return terms;
}

inline kf::LearnOptions options_of(const Json& r) {
  kf::LearnOptions o;
  if (r["weights"].is_string()) o.weights = r["weights"].text();
  if (r["most"].is_number()) o.most = static_cast<std::size_t>(r["most"].as_number());
  return o;
}

inline Json learn(const Json& r) {
  const Place p = read_field(r["field"], r["covariates"]);
  const kf::Record all = record_of(p, until_of(r, p));
  const std::size_t held = r["held_out"].is_number() ? static_cast<std::size_t>(r["held_out"].as_number()) : 2;
  if (held < 1 || held + 3 > all.t.size()) throw std::invalid_argument("keeping " + std::to_string(held) + " of " + std::to_string(all.t.size()) + " times aside leaves fewer than three to learn from");
  const std::size_t from = all.t.size() - held - 1;
  const kf::Record train = kf::first(all, from + 1);
  const kf::Covariates cov = covariates_of(p);
  const kf::LearnOptions base = options_of(r);
  std::vector<Model> models = models_default();
  if (r["models"].is_array()) {
    std::vector<Model> chosen;
    for (std::size_t k = 0; k < r["models"].size(); k++)
      for (const Model& m : models)
        if (m.name == r["models"][k].text()) chosen.push_back(m);
    models = chosen;
  }
  if (r["law"].is_array()) {
    Model m{"declared", r["title"].text("The law declared"), {}, r["threshold"].is_number() ? r["threshold"].as_number() : 0};
    for (std::size_t k = 0; k < r["law"].size(); k++) m.terms.push_back(r["law"][k].text());
    models.push_back(m);
  }
  Json list = Json::array();
  struct Best { double skill = -std::numeric_limits<double>::infinity(); std::string name; std::vector<std::vector<double>> forecast; };
  Best best;
  Json flows = Json::object();
  for (const Model& m : models) {
    Json one = Json::object({{"name", m.name}, {"title", m.title}});
    try {
      std::vector<std::vector<double>> predicted;
      double clipped = 0;
      std::size_t k = 0;
      if (m.name == "persistence") {
        for (std::size_t q = from + 1; q < all.t.size(); q++) predicted.push_back(all.u[from]);
      } else if (m.name == "own-rates") {
        const kf::OwnRates o = kf::own_rates(train);
        for (std::size_t q = from + 1; q < all.t.size(); q++) predicted.push_back(o.at(all.t[q]));
        k = p.grid.graph.size();
      } else if (m.name == "modes") {
        const kf::Modes md = kf::modes(train);
        for (std::size_t q = from + 1; q < all.t.size(); q++) {
          std::vector<double> u = md.at(all.t[q]);
          for (double& v : u) v = std::max(v, 0.0);
          predicted.push_back(u);
        }
        k = md.rank * 2;
        one.set("rank", static_cast<double>(md.rank));
        one.set("residual", num(md.residual));
      } else {
        const Learned l = learn_law(p, m.terms, m.threshold, train, base);
        predicted = kf::forecast(p.grid.graph, l.shot.law, all, from, cov, &clipped);
        k = l.shot.terms;
        one.set("law", law_json(p, l.shot.law, l.shot.standard_errors));
        one.set("reading", reading(p, l.shot.law, l.shot.standard_errors));
        one.set("integral", Json::object({{"law", law_json(p, l.integral.law, l.integral.standard_errors)}, {"explained", num(l.integral.explained)}}));
        one.set("fit", Json::object({{"explained", num(l.shot.explained)}, {"residual", num(l.shot.residual)}, {"iterations", static_cast<double>(l.shot.iterations)},
                                     {"converged", l.shot.converged}, {"method", l.shot.method}}));
        if (!l.error.empty()) one.set("note", l.error);
        if (!l.shot.dropped.empty()) {
          Json d = Json::array();
          for (const auto& x : l.shot.dropped) d.push(x);
          one.set("dropped", d);
        }
        flows.set(m.name, currents_json(kf::currents(p.grid, kf::flux(p.grid.graph, l.shot.law, all.u[from], cov))));
      }
      const kf::Score s = kf::score(p.grid.graph, predicted, all, from);
      one.set("parameters", static_cast<double>(k));
      one.set("held_out", Json::object({{"skill", num(s.skill)}, {"pattern_skill", num(s.pattern_skill)}, {"rmse", num(s.rmse)}, {"persistence_rmse", num(s.persistence_rmse)},
                                        {"total_error", num(s.total_error)}, {"change_correlation", num(s.change_correlation)}}));
      Json totals = Json::array();
      for (const auto& u : predicted) totals.push(num(kf::content(p.grid.graph, u)));
      one.set("forecast_totals", totals);
      if (clipped > 0) one.set("clipped", num(clipped));
      if (s.skill > best.skill && m.name != "persistence") best = {s.skill, m.name, predicted};
    } catch (const std::exception& e) {
      one.set("error", e.what());
    }
    list.push(one);
  }
  Json times = Json::array(), kept = Json::array(), recorded = Json::array();
  for (std::size_t q = 0; q <= from; q++) times.push(all.t[q]);
  for (std::size_t q = from + 1; q < all.t.size(); q++) kept.push(all.t[q]);
  for (std::size_t q = 0; q < all.t.size(); q++) recorded.push(num(kf::content(p.grid.graph, all.u[q])));
  Json out = Json::object({{"cells", static_cast<double>(p.grid.graph.size())}, {"fitted_on", times}, {"held_out", kept}, {"recorded_totals", recorded},
                           {"models", list}, {"currents_at", all.t[from]}, {"currents", flows}});
  if (!best.name.empty()) {
    Json frames = Json::array();
    for (const auto& u : best.forecast) frames.push(field_json(u));
    out.set("best", Json::object({{"name", best.name}, {"skill", num(best.skill)}, {"frames", frames}}));
  }
  return out;
}

inline Json modes_op(const Json& r) {
  const Place p = read_field(r["field"], r["covariates"]);
  const kf::Record all = record_of(p, until_of(r, p));
  const std::size_t rank = r["rank"].is_number() ? static_cast<std::size_t>(r["rank"].as_number()) : 0;
  const std::size_t show = r["show"].is_number() ? static_cast<std::size_t>(r["show"].as_number()) : 6;
  const kf::Modes m = kf::modes(all, rank);
  Json list = Json::array();
  for (std::size_t k = 0; k < m.modes.size(); k++) {
    const kf::Mode& md = m.modes[k];
    Json one = Json::object({{"eigenvalue", Json::array({num(md.eigenvalue.real()), num(md.eigenvalue.imag())})},
                             {"rate_a_year", num(md.rate.real())}, {"turn_a_year", num(md.rate.imag())},
                             {"period_years", md.rate.imag() != 0 ? num(2 * M_PI / std::abs(md.rate.imag())) : Json(nullptr)},
                             {"size", num(md.size)}});
    if (k < show) {
      // The mode's contribution at the first time, Re(b φ), as a map.
      Json shape = Json::array();
      for (const auto& v : md.shape) shape.push(round1((md.amplitude * v).real()));
      one.set("map", shape);
    }
    list.push(one);
  }
  Json sv = Json::array();
  for (double v : m.singular_values) sv.push(num(v));
  return Json::object({{"cells", static_cast<double>(p.grid.graph.size())}, {"step", m.step}, {"from", m.t0}, {"rank", static_cast<double>(m.rank)},
                       {"singular_values", sv}, {"residual", num(m.residual)}, {"modes", list}});
}

inline Json simulate_op(const Json& r) {
  const Place p = read_field(r["field"], r["covariates"]);
  const kf::Record all = record_of(p, until_of(r, p));
  const kf::Covariates cov = covariates_of(p);
  kf::Law law;
  std::vector<double> se;
  Json fitted;
  if (r["coefficients"].is_array()) {
    for (std::size_t k = 0; k < r["law"].size(); k++) law.terms.push_back(term_of(p, r["law"][k].text())), law.coefficients.push_back(r["coefficients"][k].as_number());
    se.assign(law.terms.size(), 0.0);
  } else {
    std::vector<std::string> names;
    double threshold = 0;
    if (r["law"].is_array()) {
      for (std::size_t k = 0; k < r["law"].size(); k++) names.push_back(r["law"][k].text());
      threshold = r["threshold"].is_number() ? r["threshold"].as_number() : 0;
    } else {
      const std::string want = r["model"].text("drift");
      for (const Model& m : models_default())
        if (m.name == want) names = m.terms, threshold = m.threshold;
      if (names.empty()) throw std::invalid_argument("the model " + want + " is not a law that runs (persistence, own-rates and modes are not)");
    }
    const Learned l = learn_law(p, names, threshold, all, options_of(r));
    law = l.shot.law, se = l.shot.standard_errors;
    fitted = Json::object({{"explained", num(l.shot.explained)}, {"converged", l.shot.converged}, {"on", Json::array({all.t.front(), all.t.back()})}});
  }
  // Scale the law's coefficients where asked: {"diffusion": 2} doubles the spread.
  if (r["scale"].is_object())
    for (const auto& [name, f] : r["scale"].as_object())
      for (std::size_t k = 0; k < law.terms.size(); k++)
        if (term_text(p, law.terms[k]) == name || kf::name(law.terms[k].kind) == name) law.coefficients[k] *= f.as_number();
  const double from = r["from"].is_number() ? r["from"].as_number() : all.t.back();
  const double to = r["to"].is_number() ? r["to"].as_number() : 2050;
  const double every = r["every"].is_number() ? r["every"].as_number() : 5;
  std::size_t k0 = all.t.size();
  for (std::size_t k = 0; k < all.t.size(); k++)
    if (std::abs(all.t[k] - from) < 1e-9) k0 = k;
  if (k0 == all.t.size()) throw std::invalid_argument("the run starts at a recorded year; " + std::to_string(static_cast<int>(from)) + " is not one");
  std::vector<double> extra(p.grid.graph.size(), 0.0);
  for (std::size_t s = 0; s < r["sources"].size(); s++) {
    const Json& q = r["sources"][s];
    const std::size_t row = static_cast<std::size_t>(q["row"].as_number()), col = static_cast<std::size_t>(q["col"].as_number());
    if (row >= p.grid.rows || col >= p.grid.cols || p.grid.vertex[row * p.grid.cols + col] < 0) throw std::invalid_argument("a source is not on a land cell");
    const long v = p.grid.vertex[row * p.grid.cols + col];
    extra[static_cast<std::size_t>(v)] += q["people_per_year"].as_number() / p.grid.graph.volume[static_cast<std::size_t>(v)];
  }
  std::vector<double> times;
  for (double t = from + every; t <= to + 1e-9; t += every) times.push_back(t);
  const kf::Run run = kf::simulate(p.grid.graph, law, all.u[k0], from, times, cov, extra);
  Json frames = Json::array(), totals = Json::array(), flows = Json::array(), years = Json::array();
  frames.push(field_json(all.u[k0])), totals.push(num(kf::content(p.grid.graph, all.u[k0]))), years.push(from);
  flows.push(currents_json(kf::currents(p.grid, kf::flux(p.grid.graph, law, all.u[k0], cov))));
  for (std::size_t k = 0; k < times.size(); k++) {
    years.push(times[k]);
    frames.push(field_json(run.u[k]));
    totals.push(num(kf::content(p.grid.graph, run.u[k])));
    flows.push(currents_json(kf::currents(p.grid, kf::flux(p.grid.graph, law, run.u[k], cov))));
  }
  // Against the field's own projections, where a year of the run is one.
  Json against = Json::array();
  for (std::size_t k = 0; k < times.size(); k++)
    for (std::size_t e = 0; e < p.epochs.size(); e++)
      if (p.projected[e] && std::abs(p.epochs[e] - times[k]) < 1e-9) {
        double sse = 0, n = 0;
        for (std::size_t c = 0; c < p.fields[e].size(); c++) sse += (run.u[k][c] - p.fields[e][c]) * (run.u[k][c] - p.fields[e][c]), n++;
        against.push(Json::object({{"year", times[k]}, {"projected_total", num(kf::content(p.grid.graph, p.fields[e]))}, {"run_total", num(kf::content(p.grid.graph, run.u[k]))},
                                   {"rmse", num(std::sqrt(sse / n))}}));
      }
  Json out = Json::object({{"cells", static_cast<double>(p.grid.graph.size())}, {"years", years}, {"frames", frames}, {"totals", totals}, {"currents", flows},
                           {"law", law_json(p, law, se)}, {"reading", reading(p, law, se)}, {"clipped", num(run.clipped)}, {"steps", static_cast<double>(run.steps)},
                           {"against_projections", against}});
  if (fitted.is_object()) out.set("fitted", fitted);
  return out;
}

// The whole place as one cell of area one: its people over time, by exponential, logistic and Gompertz's growth.
inline Json growth(const Json& r) {
  std::vector<double> years, people;
  std::vector<char> projected;
  if (r["field"].is_object()) {
    const Place p = read_field(r["field"]);
    for (std::size_t k = 0; k < p.epochs.size(); k++) years.push_back(p.epochs[k]), people.push_back(kf::content(p.grid.graph, p.fields[k])), projected.push_back(p.projected[k]);
  } else {
    for (std::size_t k = 0; k < r["years"].size(); k++) {
      years.push_back(r["years"][k].as_number()), people.push_back(r["people"][k].as_number());
      projected.push_back(r["projected"].is_array() && r["projected"][k].is_bool() && r["projected"][k].as_bool());
    }
  }
  const double scale = 1e6;  // millions, so that the coefficients are of moderate size
  kf::Record all;
  for (std::size_t k = 0; k < years.size(); k++)
    if (!projected[k] && (!r["until"].is_number() || years[k] <= r["until"].as_number())) all.t.push_back(years[k]), all.u.push_back({people[k] / scale});
  const std::size_t held = r["held_out"].is_number() ? static_cast<std::size_t>(r["held_out"].as_number()) : 2;
  if (held + 3 > all.t.size()) throw std::invalid_argument("too few times to keep " + std::to_string(held) + " aside");
  const std::size_t from = all.t.size() - held - 1;
  const kf::Record train = kf::first(all, from + 1);
  const kf::Graph one{{1.0}, {}};
  const double to = r["to"].is_number() ? r["to"].as_number() : 2050;
  kf::LearnOptions o;
  o.weights = "none";
  struct Form { std::string name, title; std::vector<kf::Term> terms; };
  const std::vector<Form> forms{{"exponential", "Exponential growth", {{kf::Kind::linear}}},
                                {"logistic", "Logistic growth (Verhulst)", {{kf::Kind::linear}, {kf::Kind::quadratic}}},
                                {"gompertz", "Gompertz's growth", {{kf::Kind::linear}, {kf::Kind::gompertz}}}};
  Json list = Json::array();
  for (const Form& f : forms) {
    Json m = Json::object({{"name", f.name}, {"title", f.title}});
    try {
      const kf::Estimate tr = kf::shooting(one, kf::integral(one, f.terms, train, {}, o), train, {}, o);
      const auto predicted = kf::forecast(one, tr.law, all, from);
      Json kept = Json::array();
      double sse = 0;
      for (std::size_t q = 0; q < predicted.size(); q++) {
        const double e = (predicted[q][0] - all.u[from + 1 + q][0]) / all.u[from + 1 + q][0];
        kept.push(num(predicted[q][0] * scale)), sse += e * e;
      }
      m.set("held_out", Json::object({{"forecast", kept}, {"relative_rmse", num(std::sqrt(sse / static_cast<double>(predicted.size())))}}));
      const kf::Estimate full = kf::shooting(one, kf::integral(one, f.terms, all, {}, o), all, {}, o);
      std::vector<double> ys;
      for (double t = all.t.front() + 1; t <= to + 1e-9; t += 1) ys.push_back(t);
      // Year by year, so that a law that runs to infinity is drawn up to the year before it does, and said to.
      Json curve = Json::array(), at = Json::array();
      curve.push(num(all.u.front()[0] * scale)), at.push(all.t.front());
      std::vector<double> u = all.u.front();
      double t = all.t.front();
      for (double y : ys) {
        try {
          const kf::Run step = kf::simulate(one, full.law, u, t, {y});
          if (!(step.u[0][0] < 1e6)) throw kf::Refusal("past a million million people");  // a trillion: no longer a population
          u = step.u[0], t = y;
          curve.push(num(u[0] * scale)), at.push(y);
        } catch (const kf::Refusal&) {
          m.set("runs_away", y);
          break;
        }
      }
      Json rd = Json::object();
      const double a = full.law.coefficients[0], b = full.law.coefficients.size() > 1 ? full.law.coefficients[1] : 0;
      rd.set("rate_a_year", num(f.name == "gompertz" ? -b : a));
      if (f.name == "logistic") {
        rd.set("ceiling", b < 0 ? num(-a / b * scale) : Json(nullptr));
        if (b > 0) rd.set("accelerating", true);
      }
      if (f.name == "gompertz") {
        rd.set(b < 0 ? "ceiling" : "floor", b != 0 ? num(std::exp(-a / b) * scale) : Json(nullptr));
        if (b > 0) rd.set("accelerating", true);
      }
      Json se = Json::array();
      for (double v : full.standard_errors) se.push(num(v));
      Json co = Json::array();
      for (double v : full.law.coefficients) co.push(num(v));
      m.set("coefficients", co), m.set("se", se), m.set("reading", rd), m.set("explained", num(full.explained));
      m.set("curve", Json::object({{"years", at}, {"people", curve}}));
    } catch (const std::exception& e) {
      m.set("error", e.what());
    }
    list.push(m);
  }
  Json ry = Json::array(), rp = Json::array(), rj = Json::array();
  for (std::size_t k = 0; k < years.size(); k++) ry.push(years[k]), rp.push(num(people[k])), rj.push(static_cast<bool>(projected[k]));
  Json fitted = Json::array(), kept = Json::array();
  for (std::size_t q = 0; q <= from; q++) fitted.push(all.t[q]);
  for (std::size_t q = from + 1; q < all.t.size(); q++) kept.push(all.t[q]);
  return Json::object({{"years", ry}, {"people", rp}, {"projected", rj}, {"fitted_on", fitted}, {"held_out", kept}, {"models", list}});
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "growth") return growth(r);
  if (op == "learn") return learn(r);
  if (op == "modes") return modes_op(r);
  if (op == "simulate") return simulate_op(r);
  throw std::invalid_argument("science::population: no operation is called " + op + " (there is: growth, learn, modes, simulate)");
}

}  // namespace science::population
