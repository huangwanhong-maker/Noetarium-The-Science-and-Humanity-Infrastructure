// Derivability under interpretation, weighed by interpreters, for any work's engine and for scripts (ADR 0112; ADR 0114): one
// composition of apodeixis's `interpretation` and tekmerion's interpreters, answering requests in JSON. Nothing is read from a
// file, and nothing is kept between requests.
//
//   points  {case}                      the case in apodeixis's written form, read: its open points with their readings, its
//                                       queries, or every line that could not be read
//   weigh   {case, query, interpreters, question?}
//                                       apodeixis's answer for the query (clear, arguable or untenable over the admissible
//                                       interpretations; D(φ)'s label; the readings fixed and set aside by the arguments) and
//                                       each interpreter's reading weighed over the admissible interpretations, with the panel's
//                                       bounds and who gives each
//
// An interpreter is {"name", "method", …}, by its method:
//
//   law         {"law": {point: {reading: p}}}                               a law at each point
//   credal      {"intervals": {point: {reading: [lower, upper]}}}            each reading's probability within an interval
//   precedents  {"alpha"?: 1}                                                the readings adopted by the question's history
//   decision    {"belief": {purpose: p}, "utility": {point: {reading: {purpose: u}}}}
//   ambiguity   {"beliefs": [{purpose: p}, …], "utility": as above, "rule": "gamma_maximin" | "e_admissible" | "maximal"}
//   proposal    {"by", "proposed": {point: reading | null}, "rationale"?, "grade"?: "C"}
//                                       what a person or a model proposed; a model's answer at the API's edge
//                                       (api/python/science/interpret.py) arrives here, and stands as proposed
//   sequence    {"purposes", "signals", "utility": as above, "responses": [{"when": {point: reading}, "law": {purpose: {signal: p}}}],
//                "inquiries"?: [{"name", "cost", "law": {purpose: {signal: p}}}], "drift"?: 0, "discount"?: 0.9,
//                "prior"?: {purpose: p}, "epsilon"?: 0.001}
//                                       a POMDP over the sequence of cases; each interpretation's response law is that of the last
//                                       entry whose "when" it completes
//
// A question is {"name"?, "features"?: {name: x}, "history"?: [{"adopted": {point: reading} | "inquiry": name, "response"}],
// "text"?}. Readings, points and purposes are named as the case and the interpreter name them; a name that is not declared is
// refused with the reason.

#pragma once

#include "json.hpp"

#include <tekmerion/interpretation/panel.hpp>
#include <tekmerion/interpretation/sequential.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::interpretation {

namespace ap = ::apodeixis::interpretation;
namespace ti = ::tekmerion::interpretation;

inline Json strings(const std::vector<std::string>& xs) {
  Json out = Json::array();
  for (const auto& x : xs) out.push(x);
  return out;
}

inline Json number(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline Json verdict(const apodeixis::kernel::Verdict& v) {
  Json evidence = Json::array();
  for (const auto& e : v.evidence) evidence.push(Json::object({{"role", e.role}, {"value", e.value}}));
  return Json::object({{"status", v.holds() ? "holds" : v.fails() ? "fails" : "undecided"}, {"claim", v.claim}, {"reason", v.reason}, {"evidence", evidence}});
}

// ---- Names to indices -----------------------------------------------------------------------------------------------------------

inline std::size_t point_of(const std::vector<ap::OpenPoint>& points, const std::string& name) {
  for (std::size_t k = 0; k < points.size(); k++)
    if (points[k].name == name) return k;
  throw std::invalid_argument("the case has no open point " + name);
}

inline std::size_t reading_of(const ap::OpenPoint& p, const std::string& name) {
  for (std::size_t r = 0; r < p.readings.size(); r++)
    if (p.readings[r] == name) return r;
  throw std::invalid_argument("the open point " + p.name + " has no reading " + name);
}

// {point: reading}, every point named: an interpretation.
inline ap::Interpretation interpretation_of(const std::vector<ap::OpenPoint>& points, const Json& j) {
  if (!j.is_object()) throw std::invalid_argument("an interpretation is an object {point: reading}");
  ap::Interpretation i(points.size(), 0);
  std::vector<char> seen(points.size(), 0);
  for (const auto& [name, value] : j.as_object()) {
    const std::size_t k = point_of(points, name);
    i[k] = reading_of(points[k], value.text()), seen[k] = 1;
  }
  for (std::size_t k = 0; k < points.size(); k++)
    if (!seen[k]) throw std::invalid_argument("an interpretation names a reading of " + points[k].name);
  return i;
}

// {point: reading | null}: a partial interpretation.
inline ap::Partial partial_of(const std::vector<ap::OpenPoint>& points, const Json& j) {
  ap::Partial p(points.size(), -1);
  if (j.is_null()) return p;
  if (!j.is_object()) throw std::invalid_argument("a partial interpretation is an object {point: reading or null}");
  for (const auto& [name, value] : j.as_object()) {
    const std::size_t k = point_of(points, name);
    if (!value.is_null()) p[k] = static_cast<long>(reading_of(points[k], value.text()));
  }
  return p;
}

inline Json partial_json(const std::vector<ap::OpenPoint>& points, const ap::Partial& p) {
  Json out = Json::object();
  for (std::size_t k = 0; k < points.size(); k++) out.set(points[k].name, p[k] >= 0 ? Json(points[k].readings[static_cast<std::size_t>(p[k])]) : Json());
  return out;
}

inline Json interpretation_json(const std::vector<ap::OpenPoint>& points, const ap::Interpretation& i) {
  Json out = Json::object();
  for (std::size_t k = 0; k < points.size(); k++) out.set(points[k].name, points[k].readings[i[k]]);
  return out;
}

// A law over the named items, in their order; every item named.
inline std::vector<double> law_of(const std::vector<std::string>& items, const Json& j, const std::string& what) {
  if (!j.is_object()) throw std::invalid_argument(what + " is an object {name: probability}");
  std::vector<double> p(items.size(), NAN);
  for (const auto& [name, value] : j.as_object()) {
    std::size_t i = items.size();
    for (std::size_t t = 0; t < items.size(); t++)
      if (items[t] == name) i = t;
    if (i == items.size()) throw std::invalid_argument(what + " names " + name + ", which is not declared");
    p[i] = value.as_number();
  }
  for (std::size_t i = 0; i < items.size(); i++)
    if (std::isnan(p[i])) throw std::invalid_argument(what + " gives no probability of " + items[i]);
  return p;
}

inline std::vector<std::string> names(const Json& j, const std::string& what) {
  if (!j.is_array() || j.size() == 0) throw std::invalid_argument(what + " are a list of names");
  std::vector<std::string> out;
  for (std::size_t i = 0; i < j.size(); i++) out.push_back(j[i].text());
  return out;
}

// utility[k][r][s] from {point: {reading: {purpose: u}}}.
inline std::vector<std::vector<std::vector<double>>> utility_of(const std::vector<ap::OpenPoint>& points, const std::vector<std::string>& purposes, const Json& j) {
  std::vector<std::vector<std::vector<double>>> u(points.size());
  for (std::size_t k = 0; k < points.size(); k++) {
    const Json& at = j[points[k].name];
    if (!at.is_object()) throw std::invalid_argument("utilities for every reading of " + points[k].name);
    for (const auto& reading : points[k].readings) {
      const Json& row = at[reading];
      if (!row.is_object()) throw std::invalid_argument("utilities for [" + points[k].name + "=" + reading + "] under each purpose");
      std::vector<double> v;
      for (const auto& s : purposes) {
        const double x = row[s].as_number();
        if (!std::isfinite(x)) throw std::invalid_argument("a utility for [" + points[k].name + "=" + reading + "] under " + s);
        v.push_back(x);
      }
      u[k].push_back(v);
    }
  }
  return u;
}

inline std::vector<std::string> purposes_of_belief(const Json& belief) {
  if (!belief.is_object() || belief.size() == 0) throw std::invalid_argument("a belief is an object {purpose: probability}");
  std::vector<std::string> out;
  for (const auto& [name, value] : belief.as_object()) out.push_back(name);
  return out;
}

// ---- Interpreters from JSON ---------------------------------------------------------------------------------------------------

inline ti::Sequence sequence_of(const std::vector<ap::OpenPoint>& points, const Json& j) {
  ti::Sequence q;
  q.purposes = names(j["purposes"], "the purposes");
  q.signals = names(j["signals"], "the signals");
  q.utility = ti::additive_table(points, utility_of(points, q.purposes, j["utility"])).u;
  const std::size_t n = ap::space(points);
  std::vector<std::vector<std::vector<double>>> response(n);
  std::vector<char> set(n, 0);
  const Json& rs = j["responses"];
  if (!rs.is_array() || rs.size() == 0) throw std::invalid_argument("a sequence states the responses to its readings");
  for (std::size_t e = 0; e < rs.size(); e++) {
    const ap::Partial when = partial_of(points, rs[e]["when"]);
    std::vector<std::vector<double>> law;
    for (const auto& s : q.purposes) law.push_back(law_of(q.signals, rs[e]["law"][s], "the response under " + s));
    for (std::size_t x = 0; x < n; x++)
      if (ap::completes(ap::decode(points, x), when)) response[x] = law, set[x] = 1;
  }
  for (std::size_t x = 0; x < n; x++)
    if (!set[x]) throw std::invalid_argument("no response is stated for " + ap::print(points, ap::decode(points, x)));
  q.response = std::move(response);
  const Json& inq = j["inquiries"];
  for (std::size_t i = 0; i < inq.size(); i++) {
    ti::Inquiry in;
    in.name = inq[i]["name"].text();
    in.cost = inq[i]["cost"].as_number();
    for (const auto& s : q.purposes) in.law.push_back(law_of(q.signals, inq[i]["law"][s], "the inquiry " + in.name + " under " + s));
    q.inquiries.push_back(std::move(in));
  }
  if (j.has("drift")) q.drift = j["drift"].as_number();
  if (j.has("discount")) q.discount = j["discount"].as_number();
  if (j.has("prior")) q.prior = law_of(q.purposes, j["prior"], "the prior");
  return q;
}

inline ti::Interpreter interpreter_of(const std::vector<ap::OpenPoint>& points, const Json& j) {
  const std::string name = j["name"].text(j["method"].text("an interpreter"));
  const std::string method = j["method"].text();
  if (method == "law") {
    ti::Law law;
    for (const auto& p : points) law.push_back(law_of(p.readings, j["law"][p.name], "the law of " + p.name));
    return ti::of_law(name, law);
  }
  if (method == "credal") {
    ti::Credal c;
    for (const auto& p : points) {
      std::vector<ti::Interval> box;
      for (const auto& r : p.readings) {
        const Json& iv = j["intervals"][p.name][r];
        if (!iv.is_array() || iv.size() != 2) throw std::invalid_argument("an interval [lower, upper] for [" + p.name + "=" + r + "]");
        box.push_back({iv[0].as_number(), iv[1].as_number()});
      }
      c.push_back(box);
    }
    return ti::of_credal(name, c);
  }
  if (method == "precedents") return ti::of_precedents(name, j.has("alpha") ? j["alpha"].as_number() : 1.0);
  if (method == "decision") {
    const auto purposes = purposes_of_belief(j["belief"]);
    return ti::of_decision(name, law_of(purposes, j["belief"], "the belief"), utility_of(points, purposes, j["utility"]));
  }
  if (method == "ambiguity") {
    const Json& bs = j["beliefs"];
    if (!bs.is_array() || bs.size() == 0) throw std::invalid_argument("ambiguity states the beliefs of its credal set");
    const auto purposes = purposes_of_belief(bs[0]);
    tyche::decision::Credal c;
    for (std::size_t i = 0; i < bs.size(); i++) c.points.push_back(law_of(purposes, bs[i], "a belief of the set"));
    const std::string rule = j["rule"].text("e_admissible");
    const ti::Rule r = rule == "gamma_maximin" ? ti::Rule::gamma_maximin : rule == "maximal" ? ti::Rule::maximal : rule == "e_admissible" ? ti::Rule::e_admissible
                                                                                                                    : throw std::invalid_argument("a rule is gamma_maximin, e_admissible or maximal");
    return ti::of_ambiguity(name, c, utility_of(points, purposes, j["utility"]), r);
  }
  if (method == "proposal") {
    const std::string grade = j["grade"].text("C");
    if (grade.size() != 1) throw std::invalid_argument("a grade is A, B or C");
    return ti::of_proposal(name, partial_of(points, j["proposed"]), j["by"].text(name), j["rationale"].text(), grade[0]);
  }
  if (method == "sequence") return ti::of_sequence(name, sequence_of(points, j), j.has("epsilon") ? j["epsilon"].as_number() : 1e-3);
  throw std::invalid_argument("no method of interpretation named " + method + " (law, credal, precedents, decision, ambiguity, proposal, sequence)");
}

inline ti::Question question_of(const std::vector<ap::OpenPoint>& points, const Json& j) {
  ti::Question q;
  q.points = points;
  q.name = j["name"].text();
  q.text = j["text"].text();
  if (j["features"].is_object())
    for (const auto& [name, value] : j["features"].as_object()) q.features[name] = value.as_number();
  const Json& h = j["history"];
  for (std::size_t t = 0; t < h.size(); t++) {
    ti::Earlier e;
    if (h[t].has("adopted")) e.adopted = interpretation_of(points, h[t]["adopted"]);
    else e.inquiry = h[t]["inquiry"].text();
    e.response = h[t]["response"].text();
    q.history.push_back(std::move(e));
  }
  return q;
}

// ---- Answers --------------------------------------------------------------------------------------------------------------------

inline Json points_json(const ap::Case& c) {
  Json pts = Json::array();
  for (const auto& p : c.points) pts.push(Json::object({{"name", p.name}, {"readings", strings(p.readings)}}));
  return pts;
}

inline Json op_points(const Json& r) {
  const auto parsed = ap::read_case(r["case"].text());
  if (!parsed.ok) return Json::object({{"ok", false}, {"errors", strings(parsed.errors)}});
  return Json::object({{"ok", true}, {"points", points_json(parsed.value)}, {"queries", strings(parsed.value.queries)}, {"interpretations", static_cast<double>(ap::space(parsed.value.points))}});
}

inline Json op_weigh(const Json& r) {
  const ap::Case c = ap::read(r["case"].text());
  const std::string query = r["query"].text(c.queries.empty() ? std::string() : c.queries.front());
  if (query.empty()) throw std::invalid_argument("a query: the sentence whose derivability is weighed");
  std::vector<ti::Interpreter> interpreters;
  const Json& in = r["interpreters"];
  for (std::size_t i = 0; i < in.size(); i++) {
    // An interpreter stated wrongly is one that cannot read the case: the panel reports it with the reason, and goes on.
    try {
      interpreters.push_back(interpreter_of(c.points, in[i]));
    } catch (const std::exception& e) {
      const std::string why = e.what();
      interpreters.push_back({in[i]["name"].text(in[i]["method"].text("an interpreter")), in[i]["method"].text(), [why](const ti::Question&) -> ti::Reading { throw std::invalid_argument(why); }});
    }
  }
  const auto weighed = ti::weigh_case(c, query, interpreters, question_of(c.points, r["question"]));
  const auto& a = weighed.evaluation.answers.front();
  const auto& adm = weighed.evaluation.admissibility;
  Json label = Json::array();
  for (const auto& e : a.label) label.push(partial_json(c.points, e));
  Json answers = Json::array();
  for (const auto& w : weighed.panel.answers) {
    Json row = Json::object({{"interpreter", w.interpreter}, {"kind", ti::word(w.kind)}, {"basis", w.basis}, {"probability", number(w.probability)},
                             {"lower", number(w.lower)}, {"upper", number(w.upper)}, {"follows", w.follows}, {"admissible", w.admissible}, {"verdict", verdict(w.verdict)}});
    if (w.chosen) row.set("chosen", interpretation_json(c.points, *w.chosen));
    if (w.proposed) row.set("proposed", partial_json(c.points, *w.proposed));
    answers.push(row);
  }
  Json failed = Json::array();
  for (const auto& [who, why] : weighed.panel.failed) failed.push(Json::object({{"interpreter", who}, {"why", why}}));
  const auto& p = weighed.panel;
  return Json::object({
      {"points", points_json(c)},
      {"query", query},
      {"derivation", Json::object({{"classification", std::string(ap::word(a.classification))}, {"verdict", verdict(a.verdict)}, {"label", label},
                                   {"admissible", static_cast<double>(a.admissible)}, {"admissible_derivable", static_cast<double>(a.admissible_derivable)},
                                   {"fixed", strings(adm.fixed)}, {"set_aside", strings(adm.set_aside)}, {"witness", a.witness}})},
      {"panel", Json::object({{"answers", answers}, {"failed", failed}, {"lower", number(p.lower)}, {"upper", number(p.upper)}, {"lowest", strings(p.lowest)},
                              {"highest", strings(p.highest)}, {"weighing", static_cast<double>(p.weighing)}, {"apart", static_cast<double>(p.apart)}, {"verdict", verdict(p.verdict)}})},
  });
}

inline Json answer(const Json& request) {
  const std::string op = request["op"].text();
  if (op == "points") return op_points(request);
  if (op == "weigh") return op_weigh(request);
  throw std::invalid_argument("science::interpretation: no operation named " + op + " (points, weigh)");
}

}  // namespace science::interpretation
