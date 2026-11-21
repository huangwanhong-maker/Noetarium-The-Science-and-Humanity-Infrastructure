// Knowledge and learning programmes as one composition any work may call (ADR 0144): the policy domain's `learning`, with
// kybernesis's scorecards to name needs, read and answered as JSON. Dates are ISO dates (YYYY-MM-DD), read by archeion's calendar.
//
//   learning.assess  {programme, date, needs_from: [{participant, scheme, card, weak_at}]}
//                    the needs and their coverage; the action plans at the date, each participant's share completed and the pace of
//                    completion by Kaplan and Meier; the results on the ladder; each contribution claim's standing; the formats
//                    compared by substantiated contribution per cost; the asks. With needs_from, a scorecard's statements at or
//                    below `weak_at` join the participant's needs, each with its evidence.
//
// A programme is {name, ladder: [level, lowest first], grades: [grade, lowest first], sufficient, participants: [{id, name, group}],
// needs: [{id, participant, text, theme, source, priority}], activities: [{id, format, title, date, cost, participants, needs}],
// actions: [{id, participant, need, text, owner, planned, due, updates: [{date, state, note, source}]}], results: [{id, participant,
// level, text, source, grade, date, activities}], claims: [{id, result, chain: [{from, to, grade, evidence}], rivals: [{explanation,
// grade, evidence}]}], asks: [{id, participant, text, date, answered, answer}]}.
#pragma once

#include <policy/learning/learning.hpp>

#include <archeion/retention/dates.hpp>

#include <science/json.hpp>
#include <science/scorecards.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace science::learning {

namespace pl = ::policy::learning;
using science::scorecards::list;
using science::scorecards::strings;

inline double day(const Json& j) {
  if (j.is_null() || (j.is_string() && j.as_string().empty())) return pl::never;
  if (j.is_number()) return j.as_number();
  return static_cast<double>(archeion::retention::day_number(j.as_string()));
}

inline std::string date_of(double d) { return std::isfinite(d) ? archeion::retention::date_of(static_cast<long>(d)) : std::string(); }

inline std::vector<std::string> words(const Json& j) {
  std::vector<std::string> v;
  for (const auto& x : list(j)) v.push_back(x.text());
  return v;
}

inline pl::Programme programme_of(const Json& j) {
  pl::Programme p;
  p.name = j["name"].text("a programme");
  p.ladder = words(j["ladder"]), p.grades = words(j["grades"]);
  p.sufficient = j["sufficient"].text(p.grades.empty() ? "" : p.grades.back());
  for (const auto& x : list(j["participants"])) p.participants.push_back({x["id"].text(), x["name"].text(x["id"].text()), x["group"].text()});
  for (const auto& x : list(j["needs"]))
    p.needs.push_back({x["id"].text(), x["participant"].text(), x["text"].text(), x["theme"].text(), x["source"].text(), static_cast<int>(x["priority"].is_number() ? x["priority"].as_number() : 0)});
  for (const auto& x : list(j["activities"]))
    p.activities.push_back({x["id"].text(), x["format"].text(), x["title"].text(), day(x["date"]), x["cost"].is_number() ? x["cost"].as_number() : 0, words(x["participants"]), words(x["needs"])});
  for (const auto& x : list(j["actions"])) {
    pl::Action a;
    a.id = x["id"].text(), a.participant = x["participant"].text(), a.need = x["need"].text(), a.text = x["text"].text(), a.owner = x["owner"].text();
    a.planned = day(x["planned"]), a.due = day(x["due"]);
    for (const auto& u : list(x["updates"])) a.updates.push_back({day(u["date"]), u["state"].text(), u["note"].text(), u["source"].text()});
    p.actions.push_back(a);
  }
  for (const auto& x : list(j["results"]))
    p.results.push_back({x["id"].text(), x["participant"].text(), x["level"].text(), x["text"].text(), x["source"].text(), x["grade"].text(), day(x["date"]), words(x["activities"])});
  for (const auto& x : list(j["claims"])) {
    pl::Claim c;
    c.id = x["id"].text(), c.result = x["result"].text();
    for (const auto& s : list(x["chain"])) c.chain.push_back({s["from"].text(), s["to"].text(), s["grade"].text(), s["evidence"].text()});
    for (const auto& r : list(x["rivals"])) c.rivals.push_back({r["explanation"].text(), r["grade"].text(), r["evidence"].text()});
    p.claims.push_back(c);
  }
  for (const auto& x : list(j["asks"])) p.asks.push_back({x["id"].text(), x["participant"].text(), x["text"].text(), day(x["date"]), day(x["answered"]), x["answer"].text()});
  return p;
}

inline Json assess_op(const Json& q) {
  auto p = programme_of(q["programme"]);
  // A scorecard's weaknesses as needs, each with its evidence, ranked by how far below the cut it lies.
  Json from_cards = Json::array();
  for (const auto& f : list(q["needs_from"])) {
    const auto s = science::scorecards::scheme_of(f["scheme"]);
    const auto c = science::scorecards::card_of(f["card"]);
    const std::string cut = f["weak_at"].text();
    long cut_index = -1;
    for (std::size_t k = 0; k < s.levels.size(); k++)
      if (s.levels[k].code == cut) cut_index = static_cast<long>(k);
    for (const auto& w : kybernesis::scorecards::weaknesses(s, c, cut)) {
      long at = 0;
      for (std::size_t k = 0; k < s.levels.size(); k++)
        if (s.levels[k].code == w.level) at = static_cast<long>(k);
      std::string evidence;
      for (const auto& e : w.evidence) evidence += (evidence.empty() ? "" : "; ") + e.text + (e.source.empty() ? "" : " (" + e.source + ")");
      pl::Need need;
      need.id = f["participant"].text() + ":" + w.statement;
      need.participant = f["participant"].text();
      need.text = w.text, need.theme = s.name;
      need.source = "the scorecard " + s.name + ", " + w.statement + " at " + w.level + (evidence.empty() ? std::string() : ": " + evidence);
      need.priority = static_cast<int>(cut_index - at + 1);
      p.needs.push_back(need);
      from_cards.push(need.id);
    }
  }
  const double date = day(q["date"]);
  const auto cov = pl::coverage(p);
  const auto act = pl::actions_at(p, date);
  const auto lad = pl::ladder(p);
  const auto fm = pl::formats(p);
  const auto asks = pl::asks_at(p, date);
  Json needs = Json::array();
  for (const auto& n : cov.needs) {
    const pl::Need* need = nullptr;
    for (const auto& x : p.needs)
      if (x.id == n.need) need = &x;
    needs.push(Json::object({{"need", n.need}, {"participant", n.participant}, {"priority", static_cast<double>(n.priority)}, {"text", need ? need->text : ""},
                             {"source", need ? need->source : ""}, {"activities", strings(n.activities)}}));
  }
  Json share = Json::object();
  for (const auto& [who, v] : cov.share_by_participant) share.set(who, v);
  Json actions = Json::array();
  for (const auto& a : act.actions)
    actions.push(Json::object({{"action", a.action}, {"participant", a.participant}, {"state", std::string(pl::name(a.state))}, {"completed", date_of(a.completed)}, {"why", a.why}}));
  Json completed = Json::object();
  for (const auto& [who, v] : act.completed_share) completed.set(who, v);
  Json pace = Json::object({{"days", Json::array()}, {"not_yet", Json::array()}, {"at_risk", Json::array()}});
  {
    Json t = Json::array(), sv = Json::array(), ar = Json::array();
    for (std::size_t k = 0; k < act.pace.time.size(); k++) t.push(act.pace.time[k]), sv.push(act.pace.survival[k]), ar.push(act.pace.at_risk[k]);
    pace = Json::object({{"days", t}, {"not_yet", sv}, {"at_risk", ar}, {"median_days", std::isfinite(act.pace.median) ? Json(act.pace.median) : Json()},
                         {"actions_with_a_plan_date", static_cast<double>(act.with_plan_date)}});
  }
  Json ladder = Json::array();
  for (const auto& x : lad.participants)
    ladder.push(Json::object({{"participant", x.participant}, {"highest", x.highest >= 0 ? Json(p.ladder[static_cast<std::size_t>(x.highest)]) : Json()},
                              {"results", static_cast<double>(x.results.size())}}));
  Json reached = Json::array();
  for (auto k : lad.reached) reached.push(static_cast<double>(k));
  Json claims = Json::array();
  for (const auto& c : p.claims) {
    const auto st = pl::standing(p, c);
    claims.push(Json::object({{"claim", st.claim}, {"result", st.result}, {"weakest", st.weakest >= 0 ? Json(p.grades[static_cast<std::size_t>(st.weakest)]) : Json()},
                              {"limiting", st.limiting}, {"unaddressed", strings(st.unaddressed)}, {"substantiated", st.substantiated}, {"formats", strings(st.formats)},
                              {"why", st.why}}));
  }
  Json formats = Json::array();
  for (const auto& f : fm)
    formats.push(Json::object({{"format", f.format}, {"cost", f.cost}, {"activities", static_cast<double>(f.activities)}, {"credit", f.credit},
                               {"credit_per_cost", std::isfinite(f.credit_per_cost) ? Json(f.credit_per_cost) : Json()}, {"frontier", f.frontier}}));
  return Json::object({{"programme", p.name}, {"date", date_of(date)}, {"needs", needs}, {"unmet", strings(cov.unmet)}, {"coverage", share},
                       {"needs_from_scorecards", from_cards}, {"actions", actions}, {"completed_share", completed}, {"pace", pace},
                       {"ladder", ladder}, {"ladder_levels", strings(p.ladder)}, {"reached", reached}, {"no_result", static_cast<double>(lad.none)},
                       {"set_aside", static_cast<double>(lad.set_aside)}, {"claims", claims}, {"formats", formats},
                       {"asks", Json::object({{"asked", static_cast<double>(asks.asked)}, {"answered", static_cast<double>(asks.answered)},
                                              {"median_days", std::isfinite(asks.median_days) ? Json(asks.median_days) : Json()}, {"open", strings(asks.open)}})}});
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "learning.assess") return assess_op(q);
  throw std::invalid_argument("learning: no operation \"" + op + "\"");
}

}  // namespace science::learning
