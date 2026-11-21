// Scorecards as one composition any work may call (ADR 0144): kybernesis's declared schemes scored on ordinal levels, read and
// answered as JSON. Nothing in it is any publisher's scheme; a scheme is the requester's declaration.
//
//   scorecard.score      {scheme, card, weak_at}
//                        each node's result with the parts that decided it and why, the scores without evidence, the statements
//                        not scored, and (with weak_at, a level) the weaknesses at or below it with their evidence
//   scorecard.compare    {scheme, before, after}
//                        statement by statement and node by node, by order, each node's comparison judged meaningful or not on
//                        the ordinal scale of the levels
//   scorecard.agreement  {scheme, cards}
//                        Krippendorff's ordinal α among assessors' cards of the same unit
//
// A scheme is {name, source, levels: [{code, meaning}], values: [number per level, only for interval_mean], statements: [{id, text,
// guidance}], nodes: [{id, title, children, rule: weakest_link | conversion | median | threshold | profile | interval_mean, plus,
// table: {"A B": "B+"}, thresholds: [{level, at_least, of_level}], exclude_unrated}], root}. A card is {unit, date, assessor,
// scores: [{statement, level (a code, NA or NR), evidence: [{text, source}], assessor, note}]}.
#pragma once

#include <kybernesis/scorecards/scorecards.hpp>

#include <science/json.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace science::scorecards {

namespace ks = kybernesis::scorecards;

// A JSON array's elements, or none when the value is absent or not an array.
inline const Json::Array& list(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}

inline ks::Scheme scheme_of(const Json& j) {
  ks::Scheme s;
  s.name = j["name"].text("a scorecard"), s.source = j["source"].text();
  for (const auto& l : list(j["levels"])) s.levels.push_back({l["code"].text(), l["meaning"].text()});
  for (const auto& v : list(j["values"])) s.values.push_back(v.as_number());
  for (const auto& x : list(j["statements"])) s.statements.push_back({x["id"].text(), x["text"].text(), x["guidance"].text()});
  for (const auto& n : list(j["nodes"])) {
    ks::Node node;
    node.id = n["id"].text(), node.title = n["title"].text(node.id);
    for (const auto& c : list(n["children"])) node.children.push_back(c.text());
    node.rule = ks::rule_of(n["rule"].text("weakest_link"));
    node.plus = n.has("plus") && n["plus"].as_bool();
    node.exclude_unrated = n.has("exclude_unrated") && n["exclude_unrated"].as_bool();
    if (n["table"].is_object())
      for (const auto& [k, v] : n["table"].as_object()) node.table[k] = v.text();
    for (const auto& t : list(n["thresholds"])) node.thresholds.push_back({t["level"].text(), static_cast<std::size_t>(t["at_least"].as_number()), t["of_level"].text()});
    s.nodes.push_back(node);
  }
  s.root = j["root"].text(s.nodes.empty() ? "" : s.nodes.back().id);
  ks::validate(s);
  return s;
}

inline ks::Card card_of(const Json& j) {
  ks::Card c;
  c.unit = j["unit"].text(), c.date = j["date"].text(), c.assessor = j["assessor"].text();
  for (const auto& x : list(j["scores"])) {
    ks::Score s;
    s.statement = x["statement"].text(), s.level = x["level"].text(), s.assessor = x["assessor"].text(c.assessor), s.note = x["note"].text();
    for (const auto& e : list(x["evidence"])) s.evidence.push_back({e["text"].text(), e["source"].text()});
    c.scores.push_back(s);
  }
  return c;
}

inline Json strings(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(x);
  return a;
}

inline Json evidence_json(const std::vector<ks::Evidence>& v) {
  Json a = Json::array();
  for (const auto& e : v) a.push(Json::object({{"text", e.text}, {"source", e.source}}));
  return a;
}

inline Json score_op(const Json& q) {
  const auto s = scheme_of(q["scheme"]);
  const auto c = card_of(q["card"]);
  const auto r = ks::score(s, c);
  Json nodes = Json::array();
  for (const auto& n : s.nodes) {
    const auto& x = r.nodes.at(n.id);
    Json profile = Json::array();
    for (auto k : x.profile) profile.push(static_cast<double>(k));
    nodes.push(Json::object({{"node", n.id}, {"title", n.title}, {"rule", std::string(ks::name(n.rule))}, {"rated", x.rated}, {"code", x.code(s)},
                             {"value", std::isfinite(x.value) ? Json(x.value) : Json()}, {"assumes_interval", x.assumes_interval}, {"profile", profile},
                             {"decided_by", strings(x.decided_by)}, {"used", static_cast<double>(x.used)}, {"left_out", static_cast<double>(x.left_out)},
                             {"why", x.why}, {"meaningful_on_ordinal", ks::meaningful_on_ordinal(n.rule)}}));
  }
  Json weak = Json::array();
  if (q.has("weak_at"))
    for (const auto& w : ks::weaknesses(s, c, q["weak_at"].text()))
      weak.push(Json::object({{"statement", w.statement}, {"text", w.text}, {"level", w.level}, {"evidence", evidence_json(w.evidence)}}));
  Json levels = Json::array();
  for (const auto& l : s.levels) levels.push(l.code);
  return Json::object({{"scheme", s.name}, {"unit", c.unit}, {"date", c.date}, {"levels", levels}, {"nodes", nodes}, {"root", s.root},
                       {"unjustified", strings(r.unjustified)}, {"unscored", strings(r.unscored)}, {"weaknesses", weak}});
}

inline Json compare_op(const Json& q) {
  const auto s = scheme_of(q["scheme"]);
  Json out = Json::array();
  for (const auto& ch : ks::compare(s, card_of(q["before"]), card_of(q["after"])))
    out.push(Json::object({{"id", ch.id}, {"before", ch.before}, {"after", ch.after}, {"direction", static_cast<double>(ch.direction)}, {"comparable", ch.comparable},
                           {"meaningful", ch.meaningful.claim.empty() ? Json() : Json(ch.meaningful.holds())}, {"why", ch.meaningful.reason}}));
  return Json::object({{"scheme", s.name}, {"changes", out}});
}

inline Json agreement_op(const Json& q) {
  const auto s = scheme_of(q["scheme"]);
  std::vector<ks::Card> cards;
  for (const auto& c : list(q["cards"])) cards.push_back(card_of(c));
  return Json::object({{"scheme", s.name}, {"assessors", static_cast<double>(cards.size())}, {"alpha", ks::agreement(s, cards)}, {"metric", "ordinal"}});
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "scorecard.score") return score_op(q);
  if (op == "scorecard.compare") return compare_op(q);
  if (op == "scorecard.agreement") return agreement_op(q);
  throw std::invalid_argument("scorecards: no operation \"" + op + "\"");
}

}  // namespace science::scorecards
