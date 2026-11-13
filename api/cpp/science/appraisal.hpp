// Appraisal and futures, for any work's engine (ADR 0108): one composition of nomos's `appraisal` and its agreement of coders,
// answering requests in JSON. Alternatives, streams, panels, fields and codes are stated in the request; nothing here names a place.
//
//   mcda        {"alternatives": [...], "criteria": [...], "score": [[...]], "benefit": [true|false...], "weight": [...],
//                "limits"?: [{"criterion": i, "bound": v, "at_least": bool}], "method"?: "weighted_sum" | "topsis" | "promethee",
//                "preferences"?: [{"kind": "usual"|"u"|"v"|"linear"|"gaussian", "q", "p", "s"}], "draws"?: 10000}
//                 alternatives screened by non-compensatory limits, then ranked; each weight's interval of stability; SMAA's
//                 rank acceptabilities
//   ahp         {"items": [...], "judgements": [[...]]}   weights from pairwise judgements, λ_max, CI, CR
//   cba         {"benefit": [...], "cost": [...], "rate"?: r, "bands"?: [[from_year, rate], ...], "groups"?: {"income": [...],
//                "npv": [...], "eta": η, "mean_income": ȳ}}     present values, NPV, ratio, IRR, payback, switching values, weights
//   delphi      {"rounds": [[...], ...], "consensus"?: 0.25, "stability"?: 0.05, "trim"?: 0.1}
//   morphology  {"parameters": [...], "values": [[...]], "excluded": [[p, v, q, w], ...], "keep"?: 1000}
//   agreement   {"codes": [[code by unit] per coder] (−1 missing), "categories": K, "metric"?: "nominal"|"ordinal"|"interval"|"ratio"}
//                 Krippendorff's α; with two coders also Cohen's κ (unweighted, linear, quadratic); Fleiss's κ when every unit has
//                 every coder

#pragma once

#include <nomos/appraisal/appraisal.hpp>
#include <nomos/measurement/agreement.hpp>

#include "json.hpp"

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::appraisal {

namespace na = ::nomos::appraisal;
namespace nm = ::nomos::measurement;

inline double num(const Json& j) { return j.is_number() ? j.as_number() : NAN; }
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(num(a[i]));
  return v;
}
inline std::vector<std::string> texts(const Json& a) {
  std::vector<std::string> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(a[i].text());
  return v;
}
inline Json written(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(std::isfinite(x) ? Json(x) : Json());
  return a;
}
inline Json words(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(Json(x));
  return a;
}

inline Json op_mcda(const Json& r) {
  na::Matrix m;
  m.alternatives = texts(r["alternatives"]), m.criteria = texts(r["criteria"]);
  for (std::size_t i = 0; i < r["score"].size(); i++) m.score.push_back(numbers(r["score"][i]));
  for (std::size_t j = 0; j < r["benefit"].size(); j++) m.benefit.push_back(r["benefit"][j].as_bool());
  m.weight = numbers(r["weight"]);
  std::vector<na::Limit> limits;
  for (std::size_t i = 0; i < r["limits"].size(); i++) {
    const Json& l = r["limits"][i];
    limits.push_back({static_cast<std::size_t>(num(l["criterion"])), num(l["bound"]), !l.has("at_least") || l["at_least"].as_bool()});
  }
  const auto sc = na::screen(m, limits);
  Json out = Json::object({{"set_aside", words(sc.set_aside)}, {"why", words(sc.why)}, {"kept", words(sc.kept.alternatives)}});
  if (sc.kept.alternatives.empty()) return out;
  const std::string method = r["method"].text("weighted_sum");
  na::Ranking rk;
  if (method == "topsis") rk = na::topsis(sc.kept);
  else if (method == "promethee") {
    std::vector<na::Preference> prefs;
    for (std::size_t j = 0; j < m.criteria.size(); j++) {
      const Json& p = r["preferences"][j];
      na::Preference pr;
      const std::string k = p["kind"].text("usual");
      pr.kind = k == "u" ? na::Preference::u_shape : k == "v" ? na::Preference::v_shape : k == "linear" ? na::Preference::linear : k == "gaussian" ? na::Preference::gaussian : na::Preference::usual;
      pr.q = number_or(p, "q", 0), pr.p = number_or(p, "p", 1), pr.s = number_or(p, "s", 1);
      prefs.push_back(pr);
    }
    const auto f = na::promethee(sc.kept, prefs);
    rk = f.ranking;
    out.set("positive", written(f.positive)), out.set("negative", written(f.negative));
  } else rk = na::weighted_sum(sc.kept);
  std::vector<double> rank;
  for (auto v : rk.rank) rank.push_back(static_cast<double>(v));
  out.set("method", Json(method)), out.set("value", written(rk.value)), out.set("rank", written(rank));
  if (sc.kept.alternatives.size() >= 2) {
    const auto st = na::weight_stability(sc.kept);
    out.set("stability", Json::object({{"best", Json(sc.kept.alternatives[st.best])}, {"lower", written(st.lower)}, {"upper", written(st.upper)}}));
    const auto acc = na::acceptability(sc.kept, static_cast<std::size_t>(number_or(r, "draws", 10000)));
    Json a = Json::array();
    for (const auto& row : acc) a.push(written(row));
    out.set("acceptability", a);
  }
  return out;
}

inline Json op_ahp(const Json& r) {
  std::vector<std::vector<double>> P;
  for (std::size_t i = 0; i < r["judgements"].size(); i++) P.push_back(numbers(r["judgements"][i]));
  const auto a = na::ahp(P);
  return Json::object({{"items", r["items"]}, {"weight", written(a.weight)}, {"lambda", Json(a.lambda)}, {"ci", Json(a.ci)}, {"cr", Json(a.cr)}, {"consistent", Json(a.consistent)}});
}

inline Json op_cba(const Json& r) {
  na::Schedule s = na::Schedule::constant(number_or(r, "rate", 0.035));
  if (r["bands"].size()) {
    s.from.clear(), s.rate.clear();
    for (std::size_t i = 0; i < r["bands"].size(); i++) s.from.push_back(static_cast<std::size_t>(num(r["bands"][i][0]))), s.rate.push_back(num(r["bands"][i][1]));
  }
  const auto b = numbers(r["benefit"]), c = numbers(r["cost"]);
  const auto a = na::appraise(b, c, s);
  const auto d = s.factors(b.size());
  std::vector<double> cum;
  double acc = 0;
  for (std::size_t t = 0; t < b.size(); t++) acc += (b[t] - c[t]) * d[t], cum.push_back(acc);
  Json out = Json::object({{"benefits", Json(a.benefits)}, {"costs", Json(a.costs)}, {"npv", Json(a.npv)}, {"ratio", std::isfinite(a.ratio) ? Json(a.ratio) : Json()},
                           {"irr", std::isfinite(a.irr) ? Json(a.irr) : Json()}, {"payback", std::isfinite(a.payback) ? Json(a.payback) : Json()},
                           {"annual", std::isfinite(a.annual) ? Json(a.annual) : Json()}, {"benefit_switch", std::isfinite(a.benefit_switch) ? Json(a.benefit_switch) : Json()},
                           {"cost_switch", std::isfinite(a.cost_switch) ? Json(a.cost_switch) : Json()}, {"factors", written(d)}, {"cumulative", written(cum)}});
  if (r.has("groups")) {
    const Json& g = r["groups"];
    const auto w = na::distributional(numbers(g["income"]), numbers(g["npv"]), number_or(g, "eta", 1.3), num(g["mean_income"]));
    out.set("weights", written(w.weight)), out.set("weighted_npv", Json(w.weighted_npv));
  }
  return out;
}

inline Json op_delphi(const Json& r) {
  std::vector<std::vector<double>> rounds;
  for (std::size_t i = 0; i < r["rounds"].size(); i++) rounds.push_back(numbers(r["rounds"][i]));
  na::DelphiOptions o;
  o.consensus = number_or(r, "consensus", o.consensus), o.stability = number_or(r, "stability", o.stability), o.trim = number_or(r, "trim", o.trim);
  const auto d = na::delphi(rounds, o);
  Json out = Json::array();
  for (const auto& x : d)
    out.push(Json::object({{"median", Json(x.median)}, {"q1", Json(x.q1)}, {"q3", Json(x.q3)}, {"iqr", Json(x.iqr)}, {"trimmed_mean", Json(x.trimmed_mean)},
                           {"relative_iqr", std::isfinite(x.relative_iqr) ? Json(x.relative_iqr) : Json()}, {"panel", Json(static_cast<double>(x.panel))},
                           {"consensus", Json(x.consensus)}, {"stable", Json(x.stable)}}));
  return Json::object({{"rounds", out}});
}

inline Json op_morphology(const Json& r) {
  na::Field f;
  f.parameters = texts(r["parameters"]);
  for (std::size_t i = 0; i < r["values"].size(); i++) f.values.push_back(texts(r["values"][i]));
  for (std::size_t i = 0; i < r["excluded"].size(); i++) {
    const auto e = numbers(r["excluded"][i]);
    if (e.size() != 4) throw std::invalid_argument("morphology: an exclusion is [parameter, value, parameter, value]");
    f.excluded.push_back({{static_cast<std::size_t>(e[0]), static_cast<std::size_t>(e[1])}, {static_cast<std::size_t>(e[2]), static_cast<std::size_t>(e[3])}});
  }
  const auto m = na::morphological(f, static_cast<std::size_t>(number_or(r, "keep", 1000)));
  Json conf = Json::array();
  for (const auto& c : m.consistent) {
    Json row = Json::array();
    for (std::size_t p = 0; p < c.size(); p++) row.push(Json(f.values[p][c[p]]));
    conf.push(row);
  }
  return Json::object({{"size", Json(static_cast<double>(m.size))}, {"count", Json(static_cast<double>(m.count))}, {"configurations", conf}});
}

inline Json op_agreement(const Json& r) {
  std::vector<std::vector<int>> codes;
  for (std::size_t i = 0; i < r["codes"].size(); i++) {
    std::vector<int> row;
    for (double v : numbers(r["codes"][i])) row.push_back(std::isfinite(v) ? static_cast<int>(v) : -1);
    codes.push_back(row);
  }
  const std::size_t K = static_cast<std::size_t>(num(r["categories"]));
  const std::string metric = r["metric"].text("nominal");
  const nm::Metric mt = metric == "ordinal" ? nm::Metric::ordinal : metric == "interval" ? nm::Metric::interval : metric == "ratio" ? nm::Metric::ratio : nm::Metric::nominal;
  Json out = Json::object({{"alpha", Json(nm::krippendorff(codes, K, mt))}, {"metric", Json(metric)}});
  if (codes.size() == 2) {
    out.set("kappa", Json(nm::cohen(codes[0], codes[1], K)));
    out.set("kappa_linear", Json(nm::cohen(codes[0], codes[1], K, nm::Disagreement::linear)));
    out.set("kappa_quadratic", Json(nm::cohen(codes[0], codes[1], K, nm::Disagreement::quadratic)));
  }
  bool complete = true;
  for (const auto& c : codes)
    for (int v : c) complete = complete && v >= 0;
  if (complete && codes.size() >= 2) {
    std::vector<std::vector<double>> counts(codes[0].size(), std::vector<double>(K, 0.0));
    for (const auto& c : codes)
      for (std::size_t u = 0; u < c.size(); u++) counts[u][static_cast<std::size_t>(c[u])] += 1;
    out.set("fleiss", Json(nm::fleiss(counts)));
  }
  return out;
}

inline bool handles(const std::string& op) { return op == "mcda" || op == "ahp" || op == "cba" || op == "delphi" || op == "morphology" || op == "agreement"; }

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "mcda") return op_mcda(r);
  if (op == "ahp") return op_ahp(r);
  if (op == "cba") return op_cba(r);
  if (op == "delphi") return op_delphi(r);
  if (op == "morphology") return op_morphology(r);
  if (op == "agreement") return op_agreement(r);
  throw std::invalid_argument("appraisal: no operation " + op + " (mcda, ahp, cba, delphi, morphology, agreement)");
}

}  // namespace science::appraisal
