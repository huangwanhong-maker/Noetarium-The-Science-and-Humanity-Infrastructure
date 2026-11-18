// Systems across scales, for any work's engine (ADR 0128): a hierarchy of units, quantities on its levels, the operators that
// carry them between levels and the laws that advance them (kinesis's scales); estimates where a quantity was not observed, and
// the places where none should be given (tyche's spatial prediction and change of support). A request holds the whole document;
// nothing is read from a file or kept between requests.
//
// The document, noetarium.scales/1:
//   hierarchy {levels: [{name, units: [names], parent: [index or name in the level above], measures: {name: [per unit]},
//              edges: [[a, b, conductance?]], costs: {name: [[n × n]] or [n·n] or {points: [[x, y]], speed, detour}}]}
//                                                                                          the finest level first; costs from
//                                                                                          points are detour · distance / speed
//   model     {step, quantities: [{name, level, kind, role, weight, unit, lower, upper, thresholds}],
//              derived: [{name, level, op, of, weight, reducer, costs, beta, kind, unit}],
//              laws: [{target, form, expression, step, damping}],
//              parameters: [{name, value, lower, upper, fit, unit}]}
//   inputs    {values: {name: [a value per unit]}, series: {name: {times, at: [[a value per unit] per time]}},
//              parameters: {name: value}}
//             where a value is a number, null (none), or {value, source, sigma, lower, upper}
//   run       {t0, t1, report: [times], record: [names]}
//
// Operations:
//   describe   the model read: its levels, quantities, derived quantities in their order, laws with their equations in LaTeX,
//              what the inputs lack (missing) and which inputs state an uncertainty (factors)
//   run        what the model does: every value kept at the times asked for, by quantity, time and unit
//   ensemble   the model over draws of its uncertain inputs (`draws`, `seed`, `outputs`): quantiles at each time and unit
//   screen     which uncertain input moves `output` (summed over its units, or at `unit`) most, by Morris's elementary effects
//   fit        the unknowns filled from `observations` [{quantity, unit, time, value, sigma}], those after `held_after` held aside
//              and the fit judged on them against persistence; with the run under the fitted values when `run` is asked
//   aggregate  whether a linear law `matrix` aggregates perfectly under `aggregation` (kinesis's perfect aggregation)
//   estimate   a quantity where it was not observed: training rows {covariates, value, block} and target rows {covariates},
//              with the fitness for use declared (`min_skill`, `max_relative_halfwidth`, `level`, `ridge`, `log`): estimates,
//              intervals, the index of dissimilarity and the area of applicability, and each estimate withheld with its reason
//   carry      a quantity carried from source zones to target zones: `method` areal (extensive or intensive), dasymetric,
//              classes (densities of ancillary classes, then dasymetric), or pycnophylactic on a grid of zones
//
// Reducers the model may name (op reduce): gini, theil, atkinson (ε = 1/2), cv, hhi, hhi_normalised, entropy, entropy_normalised
// (entropy over the logarithm of the number of parts: a land-use mix in [0, 1]), diversity (exp of the entropy), dissimilarity
// (½ Σ |a_i/A − w_i/W| of the values against the weights), median (weighted), largest_share. They are nomos's measures, the
// median tyche's.
//
// The methods are kinesis's, tyche's and nomos's; the engine only reads, calls and writes.
#pragma once

#include "json.hpp"

#include <kinesis/scales/aggregation.hpp>
#include <kinesis/scales/ensemble.hpp>
#include <kinesis/scales/fit.hpp>
#include <kinesis/scales/model.hpp>
#include <kinesis/scales/run.hpp>
#include <kinesis/terms/term.hpp>
#include <nomos/measurement/concentration.hpp>
#include <nomos/measurement/inequality.hpp>
#include <nomos/measurement/specialisation.hpp>
#include <tyche/descriptive/summary.hpp>
#include <tyche/spatial/prediction.hpp>
#include <tyche/spatial/support.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::scales {

namespace ks = kinesis::scales;

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }

inline Json nums(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(num(x));
  return a;
}

inline double number_or(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

// ---- reading -------------------------------------------------------------------------------------------------

inline ks::Hierarchy read_hierarchy(const Json& h) {
  if (!h["levels"].is_array() || h["levels"].size() == 0) throw std::invalid_argument("the hierarchy has no levels");
  std::vector<ks::Level> levels;
  for (const Json& lj : h["levels"].as_array()) {
    ks::Level l;
    l.name = lj["name"].text();
    for (const Json& u : lj["units"].as_array()) l.units.push_back(u.is_string() ? u.as_string() : Json(u).dump());
    levels.push_back(l);
  }
  for (std::size_t li = 0; li < levels.size(); li++) {
    const Json& lj = h["levels"][li];
    ks::Level& l = levels[li];
    if (lj["parent"].is_array()) {
      if (li + 1 >= levels.size()) throw std::invalid_argument("the top level " + l.name + " cannot have parents");
      std::map<std::string, std::size_t> above;
      for (std::size_t u = 0; u < levels[li + 1].units.size(); u++) above[levels[li + 1].units[u]] = u;
      for (const Json& p : lj["parent"].as_array()) {
        if (p.is_number()) {
          l.parent.push_back(static_cast<std::size_t>(p.as_number()));
        } else {
          const auto it = above.find(p.text());
          if (it == above.end()) throw std::invalid_argument("a unit of " + l.name + " names a parent " + p.text() + " that " + levels[li + 1].name + " does not have");
          l.parent.push_back(it->second);
        }
      }
    }
    if (lj["measures"].is_object())
      for (const auto& [name, v] : lj["measures"].as_object()) {
        std::vector<double> m;
        for (const Json& x : v.as_array()) m.push_back(x.as_number());
        l.measures[name] = m;
      }
    if (lj["edges"].is_array())
      for (const Json& e : lj["edges"].as_array())
        l.edges.push_back({static_cast<std::size_t>(e[0].as_number()), static_cast<std::size_t>(e[1].as_number()), number_or(e[2], 1.0)});
    if (lj["costs"].is_object())
      for (const auto& [name, c] : lj["costs"].as_object()) {
        if (c.is_object()) {
          // Costs from positions on a plane: the straight distance, times a detour, over a speed (minutes at metres a minute).
          const Json& pts = c["points"];
          const double speed = number_or(c["speed"], 1.0), detour = number_or(c["detour"], 1.0);
          const std::size_t n = pts.size();
          std::vector<double> m(n * n, 0.0);
          for (std::size_t i = 0; i < n; i++)
            for (std::size_t j = 0; j < n; j++)
              m[i * n + j] = detour * std::hypot(pts[i][0].as_number() - pts[j][0].as_number(), pts[i][1].as_number() - pts[j][1].as_number()) / speed;
          l.costs[name] = m;
          continue;
        }
        std::vector<double> m;
        for (const Json& row : c.as_array()) {
          if (row.is_array())
            for (const Json& x : row.as_array()) m.push_back(x.as_number());
          else
            m.push_back(row.as_number());
        }
        l.costs[name] = m;
      }
  }
  return ks::Hierarchy(levels);
}

// A measure that is not defined for one unit (a share of nothing, a dissimilarity of an empty group) gives that unit no value
// (NaN, written null), and the run goes on: it is not a refusal of the model.
inline ks::Reducer undefined_as_none(ks::Reducer f) {
  return [f](std::span<const double> x, std::span<const double> w) {
    try {
      return f(x, w);
    } catch (const std::invalid_argument&) {
      return ks::missing_value;
    }
  };
}

inline std::map<std::string, ks::Reducer> reducers() {
  using V = std::span<const double>;
  std::map<std::string, ks::Reducer> r;
  r["gini"] = [](V x, V w) { return nomos::measurement::gini(x, w); };
  r["theil"] = [](V x, V w) { return nomos::measurement::theil_t(x, w); };
  r["atkinson"] = [](V x, V w) { return nomos::measurement::atkinson(x, 0.5, w); };
  r["cv"] = [](V x, V w) { return nomos::measurement::coefficient_of_variation(x, w); };
  r["hhi"] = [](V x, V) { return nomos::measurement::herfindahl_hirschman(x); };
  r["hhi_normalised"] = [](V x, V) { return nomos::measurement::herfindahl_hirschman_normalised(x); };
  r["entropy"] = [](V x, V) { return nomos::measurement::entropy_index(x); };
  r["entropy_normalised"] = [](V x, V) {
    std::size_t parts = 0;
    for (double v : x) parts += v > 0;
    return parts > 1 ? nomos::measurement::entropy_index(x) / std::log(static_cast<double>(x.size())) : 0.0;
  };
  r["diversity"] = [](V x, V) { return std::exp(nomos::measurement::entropy_index(x)); };
  r["dissimilarity"] = [](V x, V w) { return nomos::measurement::dissimilarity(x, w); };
  r["median"] = [](V x, V w) { return tyche::descriptive::weighted_quantile(x, w, 0.5); };
  r["largest_share"] = [](V x, V) {
    double total = 0, top = 0;
    for (double v : x) total += v, top = std::max(top, v);
    return total > 0 ? top / total : 0.0;
  };
  for (auto& [name, f] : r) f = undefined_as_none(f);
  return r;
}

inline double bound(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline ks::Model read_model(const Json& m) {
  ks::Model out;
  out.step = number_or(m["step"], 1.0);
  for (const Json& q : m["quantities"].is_array() ? m["quantities"].as_array() : Json::Array{}) {
    ks::Quantity x;
    x.name = q["name"].text();
    x.level = q["level"].text();
    x.kind = ks::kind_of(q["kind"].text());
    x.role = ks::role_of(q["role"].text("state"));
    x.weight = q["weight"].text();
    x.unit = q["unit"].text();
    x.lower = bound(q["lower"], -ks::infinity);
    x.upper = bound(q["upper"], ks::infinity);
    if (q["thresholds"].is_array())
      for (const Json& t : q["thresholds"].as_array()) x.thresholds.push_back(t.as_number());
    out.quantities.push_back(x);
  }
  for (const Json& d : m["derived"].is_array() ? m["derived"].as_array() : Json::Array{}) {
    ks::Derived x;
    x.name = d["name"].text();
    x.level = d["level"].text();
    x.op = ks::op_of(d["op"].text("value"));
    x.of = d["of"].text();
    x.weight = d["weight"].text();
    x.reducer = d["reducer"].text();
    x.costs = d["costs"].text();
    x.beta = d["beta"].is_number() ? Json(d["beta"]).dump() : d["beta"].text();
    x.kind = ks::kind_of(d["kind"].text());
    x.unit = d["unit"].text();
    out.derived.push_back(x);
  }
  for (const Json& l : m["laws"].is_array() ? m["laws"].as_array() : Json::Array{}) {
    ks::Law x;
    x.target = l["target"].text();
    x.form = ks::form_of(l["form"].text("flow"));
    x.expression = l["expression"].text();
    x.step = number_or(l["step"], 0.0);
    x.damping = number_or(l["damping"], 1.0);
    out.laws.push_back(x);
  }
  for (const Json& p : m["parameters"].is_array() ? m["parameters"].as_array() : Json::Array{}) {
    ks::Parameter x;
    x.name = p["name"].text();
    x.value = p["value"].is_number() ? p["value"].as_number() : ks::missing_value;
    x.lower = bound(p["lower"], -ks::infinity);
    x.upper = bound(p["upper"], ks::infinity);
    x.fit = p["fit"].is_bool() && p["fit"].as_bool();
    x.unit = p["unit"].text();
    out.parameters.push_back(x);
  }
  out.reducers = reducers();
  return out;
}

inline ks::Value read_value(const Json& v) {
  ks::Value out;
  if (v.is_number()) {
    out.value = v.as_number();
  } else if (v.is_object()) {
    out.value = v["value"].is_number() ? v["value"].as_number() : ks::missing_value;
    out.source = ks::source_of(v["source"].text());
    out.sigma = number_or(v["sigma"], 0.0);
    out.lower = v["lower"].is_number() ? v["lower"].as_number() : ks::missing_value;
    out.upper = v["upper"].is_number() ? v["upper"].as_number() : ks::missing_value;
    if (!v["source"].is_string() && std::isfinite(out.lower)) out.source = ks::Source::declared;
  }
  return out;
}

inline ks::Inputs read_inputs(const Json& in) {
  ks::Inputs out;
  if (in["values"].is_object())
    for (const auto& [name, vs] : in["values"].as_object())
      for (const Json& v : vs.as_array()) out.values[name].push_back(read_value(v));
  if (in["series"].is_object())
    for (const auto& [name, s] : in["series"].as_object()) {
      ks::Series series;
      for (const Json& t : s["times"].as_array()) series.times.push_back(t.as_number());
      for (const Json& row : s["at"].as_array()) {
        std::vector<ks::Value> r;
        for (const Json& v : row.as_array()) r.push_back(read_value(v));
        series.at.push_back(r);
      }
      out.series[name] = series;
    }
  if (in["parameters"].is_object())
    for (const auto& [name, v] : in["parameters"].as_object()) out.parameters[name] = read_value(v);
  return out;
}

inline ks::Options read_options(const Json& r) {
  ks::Options o;
  o.t0 = number_or(r["t0"], 0.0);
  o.t1 = number_or(r["t1"], o.t0 + 1);
  if (r["report"].is_array())
    for (const Json& t : r["report"].as_array()) o.report.push_back(t.as_number());
  if (r["record"].is_array())
    for (const Json& n : r["record"].as_array()) o.record.push_back(n.text());
  o.tolerance = number_or(r["tolerance"], o.tolerance);
  return o;
}

struct Read {
  ks::Compiled compiled;
  ks::Inputs inputs;
  ks::Options options;
};

inline Read read(const Json& r) {
  const Json& doc = r["document"].is_object() ? r["document"] : r;
  Read out{ks::compile(read_hierarchy(doc["hierarchy"]), read_model(doc["model"])), read_inputs(doc["inputs"]), read_options(doc["run"])};
  if (r["run"].is_object() && r["document"].is_object()) out.options = read_options(r["run"]);
  return out;
}

// ---- writing ---------------------------------------------------------------------------------------------------

inline Json write_missing(const std::vector<ks::Missing>& ms) {
  Json a = Json::array();
  for (const auto& m : ms) {
    Json ex = Json::array();
    for (const auto& e : m.examples) ex.push(e);
    a.push(Json::object({{"quantity", m.quantity}, {"level", m.level}, {"what", m.what}, {"units", static_cast<double>(m.units)},
                         {"of", static_cast<double>(m.of)}, {"examples", ex}}));
  }
  return a;
}

inline Json write_factors(const std::vector<ks::Factor>& fs) {
  Json a = Json::array();
  for (const auto& f : fs)
    a.push(Json::object({{"what", ks::name(f.what)}, {"name", f.name}, {"source", ks::name(f.source)}, {"law", f.law}}));
  return a;
}

inline Json write_run(const ks::Run& r) {
  Json values = Json::object();
  for (const auto& [name, v] : r.values) {
    Json t = Json::array();
    for (const auto& row : v) t.push(nums(row));
    values.set(name, t);
  }
  Json level = Json::object();
  for (const auto& [n, l] : r.level) level.set(n, l);
  Json notes = Json::array();
  for (const auto& n : r.notes) notes.push(n);
  return Json::object({{"times", nums(r.times)}, {"values", values}, {"level", level}, {"notes", notes},
                       {"clipped", static_cast<double>(r.counts.clipped)}, {"unconverged", static_cast<double>(r.counts.unconverged)}});
}

inline std::string latex_of(const std::string& text) {
  try {
    return kinesis::terms::latex(kinesis::language::read_term(text));
  } catch (...) {
    return text;
  }
}

// ---- operations ------------------------------------------------------------------------------------------------

inline Json describe(const Json& r) {
  const Read x = read(r);
  const ks::Compiled& c = x.compiled;
  Json levels = Json::array();
  for (std::size_t l = 0; l < c.hierarchy.levels(); l++) {
    Json measures = Json::array(), costs = Json::array();
    for (const auto& [n, m] : c.hierarchy.level(l).measures) measures.push(n);
    for (const auto& [n, m] : c.hierarchy.level(l).costs) costs.push(n);
    levels.push(Json::object({{"name", c.hierarchy.level(l).name}, {"units", static_cast<double>(c.hierarchy.size(l))},
                              {"edges", static_cast<double>(c.hierarchy.level(l).edges.size())}, {"measures", measures}, {"costs", costs}}));
  }
  Json quantities = Json::array();
  for (const auto& q : c.model.quantities) {
    const ks::Law* law = c.law_of(q.name);
    Json j = Json::object({{"name", q.name}, {"level", q.level}, {"kind", ks::name(q.kind)}, {"role", ks::name(q.role)}, {"unit", q.unit}});
    if (law) {
      j.set("form", ks::name(law->form));
      j.set("expression", law->expression);
      j.set("latex", latex_of(law->expression));
      j.set("step", law->step > 0 ? law->step : c.model.step);
    }
    quantities.push(j);
  }
  Json derived = Json::array();
  for (const auto& cd : c.derived) {
    const ks::Derived& d = c.model.derived[cd.index];
    derived.push(Json::object({{"name", d.name}, {"level", d.level}, {"op", ks::name(d.op)}, {"of", d.of}, {"latex", latex_of(d.of)},
                               {"weight", d.weight}, {"reducer", d.reducer}, {"kind", ks::name(d.kind)}, {"unit", d.unit},
                               {"reads", cd.of.level == cd.level ? "its level" : (cd.of.level < cd.level ? "the level beneath" : "the level above")}}));
  }
  Json parameters = Json::array();
  for (const auto& p : c.model.parameters) {
    const auto it = x.inputs.parameters.find(p.name);
    const double v = it != x.inputs.parameters.end() ? ks::centre(it->second) : p.value;
    parameters.push(Json::object({{"name", p.name}, {"value", num(v)}, {"lower", num(p.lower)}, {"upper", num(p.upper)}, {"fit", p.fit},
                                  {"unit", p.unit}, {"source", it != x.inputs.parameters.end() ? ks::name(it->second.source) : "the model's default"}}));
  }
  return Json::object({{"levels", levels}, {"quantities", quantities}, {"derived", derived}, {"parameters", parameters},
                       {"missing", write_missing(ks::missing(c, x.inputs, x.options.t0, x.options.t1))},
                       {"factors", write_factors(ks::factors(x.inputs))}});
}

inline Json run_op(const Json& r) {
  const Read x = read(r);
  return write_run(ks::simulate(x.compiled, x.inputs, x.options));
}

inline Json ensemble_op(const Json& r) {
  const Read x = read(r);
  ks::EnsembleOptions e;
  e.draws = static_cast<std::size_t>(number_or(r["draws"], 64));
  e.seed = static_cast<std::uint64_t>(number_or(r["seed"], 1));
  e.low = number_or(r["low"], 0.05);
  e.high = number_or(r["high"], 0.95);
  if (r["outputs"].is_array())
    for (const Json& n : r["outputs"].as_array()) e.outputs.push_back(n.text());
  const ks::Ensemble en = ks::ensemble(x.compiled, x.inputs, x.options, e);
  Json bands = Json::object();
  for (const auto& [n, b] : en.bands) {
    const auto rows = [](const std::vector<std::vector<double>>& m) {
      Json a = Json::array();
      for (const auto& row : m) a.push(nums(row));
      return a;
    };
    bands.set(n, Json::object({{"low", rows(b.low)}, {"middle", rows(b.middle)}, {"high", rows(b.high)}}));
  }
  Json notes = Json::array();
  for (const auto& n : en.notes) notes.push(n);
  return Json::object({{"times", nums(en.times)}, {"bands", bands}, {"factors", write_factors(en.factors)},
                       {"draws", static_cast<double>(en.draws)}, {"refused", static_cast<double>(en.refused)}, {"notes", notes}});
}

inline Json screen_op(const Json& r) {
  const Read x = read(r);
  const ks::Screening s = ks::screen(x.compiled, x.inputs, x.options, r["output"].text(), r["unit"].text(),
                                     static_cast<std::size_t>(number_or(r["trajectories"], 10)), static_cast<std::uint64_t>(number_or(r["seed"], 1)));
  Json rows = Json::array();
  for (std::size_t k = 0; k < s.factors.size(); k++)
    rows.push(Json::object({{"what", ks::name(s.factors[k].what)}, {"name", s.factors[k].name}, {"source", ks::name(s.factors[k].source)},
                            {"mu_star", num(s.mu_star[k])}, {"mu", num(s.mu[k])}, {"sigma", num(s.sigma[k])}}));
  return Json::object({{"outcome", s.outcome}, {"factors", rows}, {"evaluations", static_cast<double>(s.evaluations)}});
}

inline Json fit_op(const Json& r) {
  const Read x = read(r);
  std::vector<ks::Observation> obs;
  for (const Json& o : r["observations"].as_array())
    obs.push_back({o["quantity"].text(), o["unit"].is_string() ? o["unit"].as_string() : Json(o["unit"]).dump(), o["time"].as_number(),
                   o["value"].as_number(), number_or(o["sigma"], 1.0)});
  ks::FitOptions fo;
  fo.held_after = number_or(r["held_after"], ks::infinity);
  fo.most = static_cast<std::size_t>(number_or(r["most"], 40));
  const ks::Fitted f = ks::fit(x.compiled, x.inputs, x.options, obs, fo);
  Json unknowns = Json::array();
  for (std::size_t k = 0; k < f.names.size(); k++)
    unknowns.push(Json::object({{"name", f.names[k]}, {"value", num(f.values[k])}, {"se", num(f.standard_errors[k])}}));
  Json corr = Json::array();
  for (const auto& row : f.correlation) corr.push(nums(row));
  Json ni = Json::array();
  for (const auto& n : f.not_identified) ni.push(n);
  Json held = Json::array();
  for (char h : f.held) held.push(static_cast<bool>(h));
  Json out = Json::object({{"unknowns", unknowns}, {"correlation", corr}, {"condition", num(f.condition)}, {"not_identified", ni},
                           {"predicted", nums(f.predicted)}, {"held", held}, {"rms", num(f.rms)}, {"held_rms", num(f.held_rms)},
                           {"persistence_rms", num(f.persistence_rms)}, {"skill", num(f.skill)}, {"fitted", static_cast<double>(f.fitted)},
                           {"held_count", static_cast<double>(f.held_count)}, {"iterations", static_cast<double>(f.iterations)},
                           {"converged", f.converged}, {"method", f.method}});
  if (r["with_run"].is_bool() && r["with_run"].as_bool()) out.set("run", write_run(ks::simulate(x.compiled, f.inputs, x.options)));
  return out;
}

inline Json aggregate_op(const Json& r) {
  const Json& M = r["matrix"];
  const Json& A = r["aggregation"];
  calx::linalg::Rectangular m(M.size(), M.size()), a(A.size(), M.size());
  for (std::size_t i = 0; i < M.size(); i++)
    for (std::size_t j = 0; j < M.size(); j++) m(i, j) = M[i][j].as_number();
  for (std::size_t i = 0; i < A.size(); i++)
    for (std::size_t j = 0; j < M.size(); j++) a(i, j) = A[i][j].as_number();
  const ks::Aggregation g = ks::perfect_aggregation(m, a, number_or(r["tolerance"], 1e-10));
  Json coarse = Json::array();
  for (std::size_t i = 0; i < g.coarse.rows; i++) {
    std::vector<double> row;
    for (std::size_t j = 0; j < g.coarse.cols; j++) row.push_back(g.coarse(i, j));
    coarse.push(nums(row));
  }
  return Json::object({{"coarse", coarse}, {"residual", num(g.residual)}, {"perfect", g.perfect}});
}

inline calx::linalg::Rectangular rows_of(const Json& rows, const char* key, std::size_t& p) {
  const std::size_t n = rows.size();
  if (n == 0) throw std::invalid_argument(std::string("no rows of ") + key);
  p = rows[0][key].size();
  calx::linalg::Rectangular X(n, p);
  for (std::size_t i = 0; i < n; i++) {
    if (rows[i][key].size() != p) throw std::invalid_argument(std::string("every row carries the same ") + key);
    for (std::size_t j = 0; j < p; j++) X(i, j) = rows[i][key][j].as_number();
  }
  return X;
}

inline Json estimate_op(const Json& r) {
  const Json& train = r["training"];
  const Json& targets = r["targets"];
  std::size_t p = 0, pt = 0;
  const auto X = rows_of(train, "covariates", p);
  const auto Xt = rows_of(targets, "covariates", pt);
  std::vector<double> y;
  std::vector<std::int64_t> block;
  std::map<std::string, std::int64_t> blocks;
  for (const Json& t : train.as_array()) {
    y.push_back(t["value"].as_number());
    const std::string b = t["block"].is_string() ? t["block"].as_string() : Json(t["block"]).dump();
    block.push_back(blocks.emplace(b, static_cast<std::int64_t>(blocks.size())).first->second);
  }
  tyche::spatial::PredictOptions o;
  o.ridge = number_or(r["ridge"], 0.0);
  o.level = number_or(r["level"], 0.9);
  o.min_skill = number_or(r["min_skill"], 0.0);
  o.max_relative_halfwidth = number_or(r["max_relative_halfwidth"], ks::infinity);
  o.log = r["log"].is_bool() && r["log"].as_bool();
  const auto pr = tyche::spatial::predict(X, y, block, Xt, o);
  Json reasons = Json::array(), withheld = Json::array(), applicable = Json::array();
  for (std::size_t t = 0; t < Xt.rows; t++) {
    reasons.push(pr.reason[t]);
    withheld.push(static_cast<bool>(pr.withheld[t]));
    applicable.push(static_cast<bool>(pr.applicable[t]));
  }
  return Json::object({{"estimate", nums(pr.estimate)}, {"lower", nums(pr.lower)}, {"upper", nums(pr.upper)},
                       {"dissimilarity", nums(pr.dissimilarity)}, {"applicable", applicable}, {"withheld", withheld}, {"reason", reasons},
                       {"threshold", num(pr.threshold)}, {"halfwidth", num(pr.halfwidth)}, {"cv_rmse", num(pr.cv_rmse)},
                       {"cv_skill", num(pr.cv_skill)}, {"cv_predicted", nums(pr.cv_predicted)},
                       {"training_dissimilarity", nums(pr.training_dissimilarity)}, {"coefficients", nums(pr.coefficients)},
                       {"importance", nums(pr.importance)}, {"blocks", static_cast<double>(pr.blocks)}, {"method", pr.method}});
}

inline Json carry_op(const Json& r) {
  const std::string method = r["method"].text("areal");
  if (method == "pycnophylactic") {
    const Json& g = r["grid"];
    tyche::spatial::ZoneGrid zg{static_cast<std::size_t>(g["rows"].as_number()), static_cast<std::size_t>(g["cols"].as_number()), {}};
    for (const Json& z : g["zone"].as_array()) zg.zone.push_back(z.is_number() ? static_cast<int>(z.as_number()) : -1);
    std::vector<double> totals;
    for (const Json& t : r["totals"].as_array()) totals.push_back(t.as_number());
    const auto s = tyche::spatial::pycnophylactic(zg, totals, static_cast<std::size_t>(number_or(r["most"], 20000)), number_or(r["tolerance"], 1e-7));
    return Json::object({{"values", nums(s.values)}, {"sweeps", static_cast<double>(s.sweeps)}, {"converged", s.converged},
                         {"method", "pycnophylactic interpolation (Tobler 1979)"}});
  }
  std::vector<tyche::spatial::Part> parts;
  for (const Json& p : r["parts"].as_array())
    parts.push_back({static_cast<std::size_t>(p["source"].as_number()), static_cast<std::size_t>(p["target"].as_number()),
                     number_or(p["measure"], 0.0), number_or(p["ancillary"], 0.0)});
  std::vector<double> y, M;
  for (const Json& v : r["values"].as_array()) y.push_back(v.as_number());
  if (r["source_measure"].is_array())
    for (const Json& v : r["source_measure"].as_array()) M.push_back(v.as_number());
  const std::size_t targets = static_cast<std::size_t>(r["targets"].as_number());
  tyche::spatial::Carried c;
  Json extra = Json::object();
  if (method == "areal") {
    c = r["intensive"].is_bool() && r["intensive"].as_bool() ? tyche::spatial::areal_intensive(targets, parts, y)
                                                             : tyche::spatial::areal_extensive(targets, parts, y, M);
  } else if (method == "dasymetric") {
    c = tyche::spatial::dasymetric(targets, parts, y);
  } else if (method == "classes") {
    // parts carry `classes`: the measure of each ancillary class in the part
    const std::size_t K = r["parts"][0]["classes"].size();
    calx::linalg::Rectangular A(y.size(), K);
    for (const Json& p : r["parts"].as_array())
      for (std::size_t k = 0; k < K; k++) A(static_cast<std::size_t>(p["source"].as_number()), k) += p["classes"][k].as_number();
    const auto cd = tyche::spatial::class_densities(A, y);
    std::vector<tyche::spatial::Part> weighted = parts;
    for (std::size_t i = 0; i < weighted.size(); i++) {
      double a = 0;
      for (std::size_t k = 0; k < K; k++) a += cd.density[k] * r["parts"][i]["classes"][k].as_number();
      weighted[i].ancillary = a;
    }
    c = tyche::spatial::dasymetric(targets, weighted, y);
    extra.set("densities", nums(cd.density));
    extra.set("explained", num(cd.explained));
  } else {
    throw std::invalid_argument("no way of carrying is called " + method + " (areal, dasymetric, classes, pycnophylactic)");
  }
  Json out = Json::object({{"values", nums(c.values)}, {"given", nums(c.given)}, {"fell_back", static_cast<double>(c.fell_back)}, {"method", c.method}});
  for (const auto& [k, v] : extra.as_object()) out.set(k, v);
  return out;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "describe") return describe(r);
  if (op == "run") return run_op(r);
  if (op == "ensemble") return ensemble_op(r);
  if (op == "screen") return screen_op(r);
  if (op == "fit") return fit_op(r);
  if (op == "aggregate") return aggregate_op(r);
  if (op == "estimate") return estimate_op(r);
  if (op == "carry") return carry_op(r);
  throw std::invalid_argument("science::scales: no operation is called " + op + " (there is: describe, run, ensemble, screen, fit, aggregate, estimate, carry)");
}

}  // namespace science::scales
