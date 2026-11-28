// Progress among units toward declared targets, as one composition any work may call: nomos's trend to target (decision 0019), the
// spread among units with its decomposition between and within groups (nomos), convergence (tyche's least squares), breaks in a trend
// (tyche's change points) and the links between two series across units (tyche's rank correlations). Nothing in it names a goal, a
// country or an indicator: units, groups and years are the caller's.
//
// Every operation takes rows [{unit, year, value}] (a null or missing value is not observed); optional groups {unit: group} and weights
// {unit: weight}.
//
//   progress.trend {rows, desired: "increase"|"decrease", target?, target_year? (2030), base_year? (2015),
//                   scheme?: {ratio_floors, rate_floors, names}}
//       each unit's trend (baseline and latest year and value, rate, expected value, ratio, met, level, trend, basis) or why it is
//       not assessed; the count of units in each class, and their weighted share where weights are given
//   progress.spread {rows, groups?, weights?, years?}
//       for each year with at least three units: n, the (weighted) mean, Gini, Theil's T, the coefficient of variation, and with groups
//       Theil's T within and between groups and each group's mean and share
//   progress.convergence {rows, from, to, log?: false}
//       units observed in both years: the annual change regressed on the initial level (β with HC1 errors, its p-value, R², n) and the
//       coefficient of variation at both years (σ-convergence)
//   progress.breaks {rows, unit?}
//       a unit's series (the only one, or the one named): its annual changes segmented by PELT under BIC, with the years each segment
//       spans and its mean change
//   progress.links {a, b, from, to}
//       two series across the units observed in both at both years: Spearman's ρ and Kendall's τ_b of their changes, and of their
//       levels in the later year, with n

#pragma once

#include <nomos/measurement/decomposition.hpp>
#include <nomos/measurement/inequality.hpp>
#include <nomos/measurement/progress.hpp>
#include <tyche/descriptive/rank.hpp>
#include <tyche/regression/linear.hpp>
#include <tyche/time_series/changepoint.hpp>

#include <science/json.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::progress {

namespace nm = nomos::measurement;

inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }

// unit -> year -> value
using Table = std::map<std::string, std::map<int, double>>;

inline Table table_of(const Json& rows) {
  if (!rows.is_array()) throw std::invalid_argument("progress: rows are a list of {unit, year, value}");
  Table t;
  for (const auto& r : rows.as_array()) {
    if (!r["value"].is_number() || !r["year"].is_number()) continue;
    const double v = r["value"].as_number();
    if (std::isfinite(v)) t[r["unit"].text()][static_cast<int>(r["year"].as_number())] = v;
  }
  return t;
}

inline std::map<std::string, double> numbers(const Json& j) {
  std::map<std::string, double> out;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object())
      if (v.is_number()) out[k] = v.as_number();
  return out;
}

inline std::map<std::string, std::string> texts(const Json& j) {
  std::map<std::string, std::string> out;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object())
      if (v.is_string()) out[k] = v.as_string();
  return out;
}

inline nm::ProgressScheme scheme_of(const Json& r) {
  nm::ProgressScheme s;
  if (r["base_year"].is_number()) s.base_year = r["base_year"].as_number();
  const Json& j = r["scheme"];
  if (j.is_object()) {
    auto list = [](const Json& a, std::vector<double>& out) {
      if (!a.is_array()) return;
      out.clear();
      for (const auto& x : a.as_array()) out.push_back(x.as_number());
    };
    list(j["ratio_floors"], s.ratio_floors);
    list(j["rate_floors"], s.rate_floors);
    if (j["names"].is_array()) {
      s.names.clear();
      for (const auto& x : j["names"].as_array()) s.names.push_back(x.text());
    }
  }
  return s;
}

inline Json trend(const Json& r) {
  const Table t = table_of(r["rows"]);
  const std::string d = r["desired"].text("increase");
  if (d != "increase" && d != "decrease") throw std::invalid_argument("progress.trend: desired is increase or decrease");
  const nm::Desired desired = d == "increase" ? nm::Desired::increase : nm::Desired::decrease;
  const nm::ProgressScheme s = scheme_of(r);
  const bool has_target = r["target"].is_number();
  const double target = has_target ? r["target"].as_number() : 0, target_year = r["target_year"].is_number() ? r["target_year"].as_number() : 2030;
  const auto weights = numbers(r["weights"]);
  std::vector<double> count(s.names.size(), 0), weight(s.names.size(), 0);
  double weighed = 0;
  Json units = Json::array();
  for (const auto& [unit, ys] : t) {
    std::vector<double> years, values;
    for (const auto& [y, v] : ys) years.push_back(y), values.push_back(v);
    try {
      const auto p = nm::trend_of_series(years, values, desired, s, has_target, target, target_year);
      count[p.level] += 1;
      if (weights.count(unit)) weight[p.level] += weights.at(unit), weighed += weights.at(unit);
      units.push(Json::object({{"unit", unit}, {"baseline_year", p.baseline_year}, {"baseline", p.baseline}, {"latest_year", p.latest_year},
                               {"latest", p.latest}, {"rate", finite(p.rate)}, {"expected", finite(p.expected)}, {"ratio", finite(p.ratio)},
                               {"met", p.met}, {"level", static_cast<double>(p.level)}, {"trend", p.trend}, {"basis", p.basis}}));
    } catch (const std::invalid_argument& e) {
      units.push(Json::object({{"unit", unit}, {"not assessed", std::string(e.what())}}));
    }
  }
  Json classes = Json::array();
  for (std::size_t k = 0; k < s.names.size(); k++)
    classes.push(Json::object({{"trend", s.names[k]}, {"units", count[k]}, {"weighted share", weighed > 0 ? Json(weight[k] / weighed) : Json()}}));
  return Json::object({{"units", units}, {"classes", classes}, {"target", has_target ? Json(target) : Json()}, {"target_year", target_year},
                       {"base_year", s.base_year}, {"desired", d}});
}

inline Json spread(const Json& r) {
  const Table t = table_of(r["rows"]);
  const auto groups = texts(r["groups"]);
  const auto weights = numbers(r["weights"]);
  std::set<int> years;
  if (r["years"].is_array())
    for (const auto& y : r["years"].as_array()) years.insert(static_cast<int>(y.as_number()));
  else
    for (const auto& [u, ys] : t)
      for (const auto& [y, v] : ys) years.insert(y);
  Json out = Json::array();
  for (const int y : years) {
    std::vector<double> x, w;
    std::vector<std::string> g;
    for (const auto& [u, ys] : t) {
      const auto it = ys.find(y);
      if (it == ys.end() || it->second < 0) continue;
      if (!weights.empty() && !weights.count(u)) continue;  // with weights given, a unit without one is left out, never weighed as one person
      x.push_back(it->second);
      w.push_back(weights.empty() ? 1.0 : weights.at(u));
      g.push_back(groups.count(u) ? groups.at(u) : "");
    }
    if (x.size() < 3) continue;
    Json row = Json::object({{"year", static_cast<double>(y)}, {"n", static_cast<double>(x.size())}, {"mean", nm::mean(x, w)}});
    const bool positive = std::all_of(x.begin(), x.end(), [](double v) { return v > 0; });
    double m = nm::mean(x, w);
    row.set("gini", m > 0 ? finite(nm::gini(x, w)) : Json());
    row.set("cv", m > 0 ? finite(nm::coefficient_of_variation(x, w)) : Json());
    row.set("theil_t", positive ? finite(nm::theil_t(x, w)) : Json());
    if (!groups.empty() && positive) {
      std::map<std::string, std::size_t> label;
      for (const auto& s : g) label.emplace(s, 0);
      std::size_t k = 0;
      for (auto& [s, i] : label) i = k++;
      std::vector<std::size_t> gi;
      for (const auto& s : g) gi.push_back(label.at(s));
      const auto dec = nm::decompose_generalised_entropy(x, gi, 1.0, w);
      Json parts = Json::array();
      for (const auto& [s, i] : label)
        parts.push(Json::object({{"group", s}, {"mean", finite(dec.mean[i])}, {"population share", dec.population_share[i]}, {"theil_t", finite(dec.index[i])}}));
      row.set("within", dec.within);
      row.set("between", dec.between);
      row.set("between share", dec.total > 0 ? Json(dec.between / dec.total) : Json());
      row.set("groups", parts);
    }
    out.push(row);
  }
  return Json::object({{"years", out}});
}

inline Json convergence(const Json& r) {
  const Table t = table_of(r["rows"]);
  if (!r["from"].is_number() || !r["to"].is_number()) throw std::invalid_argument("progress.convergence: from and to are years");
  const int from = static_cast<int>(r["from"].as_number()), to = static_cast<int>(r["to"].as_number());
  if (to <= from) throw std::invalid_argument("progress.convergence: to is after from");
  const bool logs = r["log"].is_bool() && r["log"].as_bool();
  std::vector<double> x0, g, x1;
  for (const auto& [u, ys] : t) {
    const auto a = ys.find(from), b = ys.find(to);
    if (a == ys.end() || b == ys.end()) continue;
    if (logs && (a->second <= 0 || b->second <= 0)) continue;
    x0.push_back(logs ? std::log(a->second) : a->second);
    x1.push_back(b->second);
    g.push_back(((logs ? std::log(b->second) : b->second) - x0.back()) / (to - from));
  }
  if (x0.size() < 4) throw std::invalid_argument("progress.convergence: fewer than four units are observed in both years");
  tyche::regression::Rectangular X(x0.size(), 2);
  for (std::size_t i = 0; i < x0.size(); i++) X(i, 0) = 1, X(i, 1) = x0[i];
  tyche::regression::OlsOptions o;
  o.covariance = tyche::regression::Covariance::hc1;
  const auto f = tyche::regression::ols(X, g, o);
  std::vector<double> a0(x0.size());
  for (std::size_t i = 0; i < x0.size(); i++) a0[i] = logs ? std::exp(x0[i]) : x0[i];
  const double m0 = nm::mean(a0), m1 = nm::mean(x1);
  return Json::object({{"n", static_cast<double>(x0.size())}, {"from", static_cast<double>(from)}, {"to", static_cast<double>(to)}, {"log", logs},
                       {"beta", finite(f.beta[1])}, {"se", finite(f.se[1])}, {"p", finite(f.p[1])}, {"intercept", finite(f.beta[0])},
                       {"r_squared", finite(f.r_squared)},
                       {"cv_from", m0 > 0 ? finite(nm::coefficient_of_variation(a0)) : Json()},
                       {"cv_to", m1 > 0 ? finite(nm::coefficient_of_variation(x1)) : Json()}});
}

inline Json breaks(const Json& r) {
  const Table t = table_of(r["rows"]);
  if (t.empty()) throw std::invalid_argument("progress.breaks: no observed series");
  const std::string unit = r["unit"].text(t.begin()->first);
  if (!t.count(unit)) throw std::invalid_argument("progress.breaks: no series for " + unit);
  const auto& ys = t.at(unit);
  std::vector<int> years;
  std::vector<double> change;
  int prev = 0;
  double pv = 0;
  bool first = true;
  for (const auto& [y, v] : ys) {
    if (!first && y == prev + 1) years.push_back(y), change.push_back(v - pv);
    first = false, prev = y, pv = v;
  }
  if (change.size() < 6) throw std::invalid_argument("progress.breaks: fewer than six annual changes in consecutive years");
  const auto seg = tyche::time_series::pelt(change);
  Json segs = Json::array();
  for (const auto& sgm : seg.segments)
    segs.push(Json::object({{"from", static_cast<double>(years[sgm.begin])}, {"to", static_cast<double>(years[sgm.end - 1])},
                            {"mean change", sgm.mean}, {"years", static_cast<double>(sgm.size())}}));
  Json at = Json::array();
  for (const auto c : seg.changepoints) at.push(static_cast<double>(years[c]));
  return Json::object({{"unit", unit}, {"breaks", at}, {"segments", segs}, {"changes", static_cast<double>(change.size())}});
}

inline Json links(const Json& r) {
  const Table a = table_of(r["a"]), b = table_of(r["b"]);
  if (!r["from"].is_number() || !r["to"].is_number()) throw std::invalid_argument("progress.links: from and to are years");
  const int from = static_cast<int>(r["from"].as_number()), to = static_cast<int>(r["to"].as_number());
  std::vector<double> da, db, la, lb;
  for (const auto& [u, ya] : a) {
    const auto ib = b.find(u);
    if (ib == b.end()) continue;
    const auto& yb = ib->second;
    if (ya.count(from) && ya.count(to) && yb.count(from) && yb.count(to)) {
      da.push_back(ya.at(to) - ya.at(from)), db.push_back(yb.at(to) - yb.at(from));
      la.push_back(ya.at(to)), lb.push_back(yb.at(to));
    }
  }
  if (da.size() < 5) throw std::invalid_argument("progress.links: fewer than five units are observed in both series at both years");
  return Json::object({{"n", static_cast<double>(da.size())}, {"from", static_cast<double>(from)}, {"to", static_cast<double>(to)},
                       {"spearman_change", finite(tyche::descriptive::spearman(da, db))}, {"kendall_change", finite(tyche::descriptive::kendall_tau_b(da, db))},
                       {"spearman_level", finite(tyche::descriptive::spearman(la, lb))}, {"kendall_level", finite(tyche::descriptive::kendall_tau_b(la, lb))}});
}

inline bool handles(const std::string& op) { return op.rfind("progress.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "progress.trend") return trend(r);
  if (op == "progress.spread") return spread(r);
  if (op == "progress.convergence") return convergence(r);
  if (op == "progress.breaks") return breaks(r);
  if (op == "progress.links") return links(r);
  throw std::invalid_argument("progress: no operation " + op);
}

}  // namespace science::progress
