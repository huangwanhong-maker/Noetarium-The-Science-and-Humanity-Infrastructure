// Allocation under scarcity, as one composition any work may call (ADR 0136): the allocation domain with nomos's rules and welfare
// orders, archeion's journal and the economics domain's circulation, read and answered as JSON. The Urban Studio's plug-in
// "Allocation and justice" answers its page with it, and a study may run it without the page. Nothing in it names a place.
//
//   run         {round}
//               what each entity receives, stage by stage, with its reasons, who was excluded and why, who was reached first
//   judge       {round, reading, principles, variation, flips}
//               the round against its alternatives (other rules, other orders) under the declared principles: the undominated
//               options, the best under each principle, the pivots, where a parameter flips a comparison, what is at stake
//   procedure   {round, acts}
//               the acts replayed into a sealed journal (declare, consult, interest, oversee, allocate, appeal, review, commit,
//               draw), each appeal heard for everyone, and the rules of procedural justice checked
//   claims      {estate, claims, rules, theta}
//               a divisible sum among claims by each rationing rule named
//   budget      {budget, costs, approvals, names, rules, satisfaction}
//               projects funded by votes under each rule (utilitarian greedy, equal shares, Phragmén), the voters left with nothing,
//               the satisfaction's distribution, and EJR where the voters are few enough to enumerate
//   reserves    {individuals, categories: [{name, capacity, priority}], precedence}
//               a reserve system processed in a precedence, its three axioms, and who is served under every order
//   incidence   {capitalisation, propagation, lm3, network, doughnut, targeting}
//               who finally gains, each part answered or refused apart
//
// A round is {name, resource, unit, amount, entities: [{id, name, group, at: {attribute: number}}], stages: [{kind, principle,
// source: {who, when, why}, share, attribute, bound, at_least, per, rule, theta, claim, indicators: [{name, edges, rising, weight}],
// bases, rates, lambda}], sources: {attribute: where its values came from}}; the stages' kinds are allocation::rounds's. A principle
// is {name, kind: total | prioritarian | leximin | shortfall | opportunity, parameter, alpha, types}.

#pragma once

#include <allocation/incidence/incidence.hpp>
#include <allocation/procedure/procedure.hpp>
#include <allocation/rounds/rounds.hpp>
#include <allocation/verdicts/verdicts.hpp>
#include <nomos/allocation/budgeting.hpp>
#include <nomos/allocation/rationing.hpp>
#include <nomos/allocation/reserves.hpp>
#include <nomos/measurement/inequality.hpp>

#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

#include <science/json.hpp>

namespace science::allocation {

namespace ar = ::allocation::rounds;
namespace av = ::allocation::verdicts;
namespace ap = ::allocation::procedure;
namespace ai = ::allocation::incidence;
namespace na = nomos::allocation;
namespace nw = nomos::welfare;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline double r6(double x) { return std::isfinite(x) ? std::round(x * 1e6) / 1e6 : x; }
inline Json numbers(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(std::isfinite(x) ? Json(r6(x)) : Json());
  return a;
}
inline Json texts(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(x);
  return a;
}
inline std::vector<double> doubles(const Json& j) {
  std::vector<double> out;
  if (j.is_array())
    for (const auto& x : j.as_array()) out.push_back(x.as_number());
  return out;
}
inline std::vector<std::string> strings(const Json& j) {
  std::vector<std::string> out;
  if (j.is_array())
    for (const auto& x : j.as_array()) out.push_back(x.is_string() ? x.as_string() : x.dump());
  return out;
}

// ---- A round, read and written --------------------------------------------------------------------------------------------------

inline ar::Round round_of(const Json& j) {
  if (!j.is_object()) throw std::invalid_argument("allocation: a round is an object");
  ar::Round r;
  r.name = j["name"].text();
  r.resource = j["resource"].text();
  r.unit = j["unit"].text();
  r.amount = num(j["amount"], 0);
  if (!j["entities"].is_array() || j["entities"].size() == 0) throw std::invalid_argument("allocation: a round needs entities");
  for (const auto& e : j["entities"].as_array()) {
    ar::Entity x;
    x.id = e["id"].is_string() ? e["id"].as_string() : e["id"].dump();
    x.name = e["name"].text(x.id);
    x.group = e["group"].text();
    if (e["at"].is_object())
      for (const auto& [k, v] : e["at"].as_object())
        if (v.is_number()) x.at[k] = v.as_number();
    r.entities.push_back(std::move(x));
  }
  if (j["stages"].is_array())
    for (const auto& s : j["stages"].as_array()) {
      ar::Stage st;
      st.kind = s["kind"].text("proportional");
      st.principle = s["principle"].text();
      st.source = {s["source"]["who"].text(), s["source"]["when"].text(), s["source"]["why"].text()};
      st.share = num(s["share"], 1);
      st.attribute = s["attribute"].text();
      st.bound = num(s["bound"], 0);
      st.at_least = !s["at_least"].is_bool() || s["at_least"].as_bool();
      st.per = s["per"].text("population");
      st.rule = s["rule"].text("proportional");
      st.theta = num(s["theta"], 0.5);
      st.claim = s["claim"].text();
      if (s["indicators"].is_array())
        for (const auto& i : s["indicators"].as_array())
          st.indicators.push_back({i["name"].text(), {}, doubles(i["edges"]), !i["rising"].is_bool() || i["rising"].as_bool(), num(i["weight"], 1)});
      st.bases = strings(s["bases"]);
      st.rates = doubles(s["rates"]);
      st.lambda = num(s["lambda"], 1);
      r.stages.push_back(std::move(st));
    }
  if (j["sources"].is_object())
    for (const auto& [k, v] : j["sources"].as_object()) r.sources[k] = v.text();
  return r;
}

inline Json result_json(const ar::Round& r, const ar::Result& res) {
  Json entities = Json::array();
  for (std::size_t i = 0; i < r.entities.size(); i++) {
    Json by = Json::array();
    for (const auto& st : res.by_stage) by.push(r6(st[i]));
    entities.push(Json::object({{"id", r.entities[i].id}, {"name", r.entities[i].name}, {"amount", r6(res.amount[i])}, {"by_stage", by},
                                {"excluded", res.excluded[i].empty() ? Json() : Json(res.excluded[i])}, {"reasons", texts(res.reasons[i])}}));
  }
  Json order = Json::array();
  for (const auto& x : ar::order_reached(res))
    order.push(Json::object({{"entity", r.entities[x.entity].id}, {"stage", x.stage < r.stages.size() ? Json(static_cast<double>(x.stage)) : Json()}, {"first", r6(x.first)}}));
  Json stages = Json::array();
  for (std::size_t k = 0; k < r.stages.size(); k++) {
    double given = 0;
    for (double x : res.by_stage[k]) given += x;
    stages.push(Json::object({{"kind", r.stages[k].kind}, {"principle", r.stages[k].principle}, {"given", r6(given)}, {"remaining", r6(res.remaining_after[k])}}));
  }
  return Json::object({{"entities", entities}, {"stages", stages}, {"order", order}, {"left", r6(res.left)}, {"notes", texts(res.notes)},
                       {"reads", texts(ar::attributes_read(r))}, {"digest", ap::digest(r)}, {"result_digest", ap::digest(res)}});
}

inline Json run_op(const Json& q) {
  const auto r = round_of(q["round"]);
  return result_json(r, ar::run(r));
}

// ---- Judgement ------------------------------------------------------------------------------------------------------------------

inline nw::Principle principle_of(const Json& p) {
  nw::Principle x;
  x.name = p["name"].text();
  const std::string k = p["kind"].text("total");
  if (k == "total") x.kind = nw::Kind::total;
  else if (k == "prioritarian") x.kind = nw::Kind::prioritarian;
  else if (k == "leximin") x.kind = nw::Kind::leximin;
  else if (k == "shortfall") x.kind = nw::Kind::shortfall;
  else if (k == "opportunity") x.kind = nw::Kind::opportunity;
  else throw std::invalid_argument("allocation: a principle is total, prioritarian, leximin, shortfall or opportunity, not " + k);
  if (x.name.empty()) x.name = k;
  x.parameter = num(p["parameter"], 0);
  x.alpha = num(p["alpha"], 1);
  for (const auto& t : doubles(p["types"])) x.types.push_back(static_cast<std::size_t>(t));
  return x;
}

inline Json judge_op(const Json& q) {
  const auto r = round_of(q["round"]);
  av::Reading reading{q["reading"]["level"].text(), q["reading"].has("per") ? q["reading"]["per"].text() : std::string("population")};
  std::vector<nw::Principle> principles;
  if (q["principles"].is_array())
    for (const auto& p : q["principles"].as_array()) {
      auto x = principle_of(p);
      // Opportunity by group: types from the entities' groups when none are given.
      if (x.kind == nw::Kind::opportunity && x.types.empty()) {
        std::vector<std::string> groups;
        for (const auto& e : r.entities) {
          if (!reading.per.empty() && !(e.get(reading.per) > 0)) continue;  // left out of the outcomes, as verdicts reads them
          auto it = std::find(groups.begin(), groups.end(), e.group);
          if (it == groups.end()) groups.push_back(e.group), x.types.push_back(groups.size() - 1);
          else x.types.push_back(static_cast<std::size_t>(it - groups.begin()));
        }
      }
      principles.push_back(x);
    }
  av::Variation v;
  if (q["variation"].is_object()) {
    if (q["variation"]["rules"].is_array()) v.rules = strings(q["variation"]["rules"]);
    if (q["variation"]["orders"].is_bool()) v.orders = q["variation"]["orders"].as_bool();
  }
  std::vector<av::FlipRange> ranges;
  if (q["flips"].is_array())
    for (const auto& f : q["flips"].as_array())
      ranges.push_back({static_cast<std::size_t>(num(f["principle"], 0)), num(f["lo"], 0), num(f["hi"], 5), static_cast<int>(num(f["steps"], 200))});
  const auto J = av::judge(r, reading, principles, v, ranges);
  Json options = Json::array();
  for (std::size_t k = 0; k < J.options.size(); k++) {
    const auto& o = J.options[k];
    options.push(Json::object({{"names", texts(o.names)}, {"amounts", numbers(o.result.amount)}, {"values", numbers(J.comparison.values[k])}, {"outcome", numbers(o.outcome.value)},
                               {"undominated", std::find(J.comparison.maximal.begin(), J.comparison.maximal.end(), k) != J.comparison.maximal.end()}}));
  }
  Json best = Json::array();
  for (const auto& b : J.comparison.best_under) {
    Json a = Json::array();
    for (auto i : b) a.push(static_cast<double>(i));
    best.push(a);
  }
  Json pivots = Json::array();
  for (const auto& [m, text] : J.pivots) pivots.push(Json::object({{"option", static_cast<double>(m)}, {"pivot", text}}));
  Json flips = Json::array();
  for (const auto& f : J.flips) flips.push(Json::object({{"principle", static_cast<double>(f.principle)}, {"against", static_cast<double>(f.against)}, {"at", numbers(f.at)}}));
  Json stakes = Json::array();
  for (std::size_t i = 0; i < J.stakes.size(); i++)
    stakes.push(Json::object({{"entity", r.entities[i].id}, {"least", r6(J.stakes[i].least)}, {"most", r6(J.stakes[i].most)}, {"declared", r6(J.stakes[i].declared)}, {"best", static_cast<double>(J.stakes[i].best)}}));
  // The order among the options: every pair one dominates the other in, under the declared principles together.
  Json order = Json::array();
  for (const auto& pw : J.comparison.pairs) {
    if (pw.relation == nw::Relation::dominates) order.push(Json::array({static_cast<double>(pw.a), static_cast<double>(pw.b)}));
    else if (pw.relation == nw::Relation::dominated) order.push(Json::array({static_cast<double>(pw.b), static_cast<double>(pw.a)}));
  }
  Json maximal = Json::array(), dominating = Json::array();
  for (auto m : J.comparison.maximal) maximal.push(static_cast<double>(m));
  for (auto d : J.dominating) dominating.push(static_cast<double>(d));
  return Json::object({{"principles", texts(J.comparison.principles)}, {"options", options}, {"undominated", maximal}, {"best_under", best},
                       {"declared_undominated", J.declared_undominated}, {"dominating", dominating}, {"dominance", order}, {"pivots", pivots}, {"flips", flips}, {"stakes", stakes}, {"said", texts(J.said)}});
}

// ---- Procedure ------------------------------------------------------------------------------------------------------------------

inline ap::Appeal appeal_of(const Json& a) {
  ap::Appeal x;
  x.when = a["when"].text();
  x.who = a["who"].text();
  x.entity = a["entity"].text();
  x.kind = a["kind"].text("datum");
  x.stage = static_cast<std::size_t>(num(a["stage"], 0));
  x.key = a["key"].text();
  x.value = num(a["value"], 0);
  x.text = a["text"].text();
  x.reason = a["reason"].text();
  return x;
}

inline Json hearing_json(const ar::Round& r, const ap::Hearing& h) {
  Json changed = Json::array();
  for (auto i : h.changed)
    changed.push(Json::object({{"entity", r.entities[i].id}, {"before", r6(h.before.amount[i])}, {"after", r6(h.after.amount[i])}}));
  return Json::object({{"changed", changed}, {"said", h.said}, {"revised_digest", ap::digest(h.revised)}});
}

inline Json procedure_op(const Json& q) {
  ar::Round current = round_of(q["round"]);
  ap::Record rec;
  Json hearings = Json::array(), draws = Json::array();
  if (q["acts"].is_array())
    for (const auto& a : q["acts"].as_array()) {
      const std::string act = a["act"].text(), when = a["when"].text(), who = a["who"].text();
      if (act == "declare") ap::declare(rec, current, when, who, a["note"].text());
      else if (act == "consult") ap::consult(rec, a["group"].text(), when, who, a["note"].text());
      else if (act == "interest") ap::interest(rec, who, a["entity"].text(), when, a["note"].text());
      else if (act == "oversee") ap::oversee(rec, who, when, a["note"].text());
      else if (act == "allocate") ap::allocate(rec, current, ar::run(current), when, who);
      else if (act == "appeal") {
        auto x = appeal_of(a);
        const std::size_t k = ap::appeal(rec, x);
        hearings.push(Json::object({{"appeal", static_cast<double>(k)}, {"hearing", hearing_json(current, ap::hear(current, x))}}));
      } else if (act == "review") {
        const std::size_t k = static_cast<std::size_t>(num(a["appeal"], 0));
        if (k >= rec.appeals.size()) throw std::invalid_argument("allocation: a review answers an appeal made before it");
        const bool upheld = a["upheld"].is_bool() && a["upheld"].as_bool();
        ar::Round revised = upheld ? ap::corrected(current, rec.appeals[k]) : current;
        ap::review(rec, {k, when, who, upheld, a["reason"].text()}, revised);
        if (upheld) current = revised;
      } else if (act == "commit") ap::commit(rec, a["seed"].text(), when, who);
      else if (act == "draw") {
        const auto d = ap::draw(rec, a["seed"].text(), strings(a["among"]), static_cast<std::size_t>(num(a["k"], 1)), when, who, doubles(a["weights"]));
        draws.push(Json::object({{"drawn", texts(d.drawn)}, {"commitment", d.commitment}}));
      } else throw std::invalid_argument("allocation: an act is declare, consult, interest, oversee, allocate, appeal, review, commit or draw, not \"" + act + "\"");
    }
  Json rules = Json::array();
  for (const auto& x : ap::check(rec, current))
    rules.push(Json::object({{"name", x.name}, {"framework", x.framework}, {"status", std::string(apodeixis::kernel::word(x.verdict.status))}, {"claim", x.verdict.claim}, {"reason", x.verdict.reason}}));
  Json journal = Json::array();
  for (const auto& e : rec.journal.entries)
    journal.push(Json::object({{"when", e.when}, {"who", e.who}, {"what", e.what}, {"about", e.about}, {"note", e.note}, {"seal", e.seal}}));
  return Json::object({{"rules", rules}, {"journal", journal}, {"head", rec.journal.head()}, {"hearings", hearings}, {"draws", draws},
                       {"round", ap::digest(current)}, {"result", result_json(current, ar::run(current))}});
}

// ---- Claims, budgets and reserves -----------------------------------------------------------------------------------------------

inline Json claims_op(const Json& q) {
  na::Claims P{num(q["estate"], 0), doubles(q["claims"])};
  std::vector<std::string> rules = q["rules"].is_array() ? strings(q["rules"]) : std::vector<std::string>{"proportional", "equal_awards", "equal_losses", "talmud", "piniles", "adjusted_proportional"};
  Json out = Json::object();
  for (const auto& name : rules) out.set(name, numbers(ar::rule_of(name, num(q["theta"], 0.5))(P)));
  return out;
}

inline Json budget_op(const Json& q) {
  na::Election e;
  e.budget = num(q["budget"], 0);
  e.costs = doubles(q["costs"]);
  for (const auto& v : q["approvals"].as_array()) {
    std::vector<std::size_t> a;
    for (const auto& x : v.as_array()) a.push_back(static_cast<std::size_t>(x.as_number()));
    e.approvals.push_back(std::move(a));
  }
  const std::vector<std::string> rules = q["rules"].is_array() ? strings(q["rules"]) : std::vector<std::string>{"greedy", "equal_shares", "phragmen"};
  const auto sat = q["satisfaction"].text("cost") == "cardinality" ? na::Satisfaction::cardinality : na::Satisfaction::cost;
  const std::size_t n = e.approvals.size();
  // Voters' groups (a neighbourhood, say), -1 for none, and their names.
  std::vector<long> group(n, -1);
  const auto gv = doubles(q["groups"]);
  if (!gv.empty() && gv.size() != n) throw std::invalid_argument("allocation: a group for every voter, or none");
  for (std::size_t i = 0; i < gv.size(); i++) group[i] = static_cast<long>(gv[i]);
  const auto group_names = strings(q["group_names"]);
  long G = 0;
  for (long g : group) G = std::max(G, g + 1);
  Json out = Json::object(), list = Json::array();
  auto judge_selection = [&](const std::string& name, const na::Selection& s) {
    std::vector<bool> funded(e.costs.size(), false);
    for (auto j : s.funded) funded[j] = true;
    std::vector<double> count(n, 0), money(n, 0);
    double nothing = 0;
    std::vector<double> g_voters(G, 0), g_nothing(G, 0), g_count(G, 0);
    for (std::size_t i = 0; i < n; i++) {
      for (auto j : e.approvals[i])
        if (funded[j]) count[i] += 1, money[i] += e.costs[j];
      if (count[i] == 0) nothing += 1;
      if (group[i] >= 0) g_voters[group[i]] += 1, g_nothing[group[i]] += count[i] == 0, g_count[group[i]] += count[i];
    }
    const double mean = n ? nomos::measurement::mean(count) : 0.0;
    Json item = Json::object({{"rule", name}, {"funded", Json(std::vector<double>(s.funded.begin(), s.funded.end()))}, {"spent", r6(s.spent)},
                              {"completed", Json(std::vector<double>(s.completed.begin(), s.completed.end()))}, {"said", s.said},
                              {"voters_with_nothing", nothing}, {"share_with_nothing", n ? r6(nothing / n) : 0.0},
                              {"mean_funded", r6(mean)}, {"gini_funded", mean > 0 ? Json(r6(nomos::measurement::gini(count))) : Json()},
                              {"mean_cost_funded", n ? r6(nomos::measurement::mean(money)) : 0.0}});
    if (G > 0) {
      Json groups = Json::array();
      for (long g = 0; g < G; g++)
        groups.push(Json::object({{"group", g < static_cast<long>(group_names.size()) ? Json(group_names[g]) : Json(static_cast<double>(g))}, {"voters", g_voters[g]},
                                  {"share_with_nothing", g_voters[g] > 0 ? Json(r6(g_nothing[g] / g_voters[g])) : Json()},
                                  {"mean_funded", g_voters[g] > 0 ? Json(r6(g_count[g] / g_voters[g])) : Json()}}));
      item.set("groups", groups);
    }
    if (n <= 16) {
      const auto ejr = na::check_ejr(e, s.funded);
      item.set("ejr", Json::object({{"satisfied", ejr.satisfied}, {"group", Json(std::vector<double>(ejr.group.begin(), ejr.group.end()))}, {"shortfall", static_cast<double>(ejr.shortfall)}}));
    } else {
      item.set("ejr", Json::object({{"checked", false}, {"why", "EJR is checked by enumerating groups of voters, possible to sixteen; here there are " + std::to_string(n)}}));
    }
    list.push(item);
  };
  for (const auto& name : rules) {
    na::Selection s;
    if (name == "greedy") s = na::utilitarian_greedy(e);
    else if (name == "greedy_per_cost") s = na::utilitarian_greedy(e, true);
    else if (name == "equal_shares") s = na::equal_shares(e, sat, !q["complete"].is_bool() || q["complete"].as_bool());
    else if (name == "phragmen") s = na::phragmen(e);
    else throw std::invalid_argument("allocation: a budgeting rule is greedy, greedy_per_cost, equal_shares or phragmen");
    judge_selection(name, s);
  }
  // What was decided in fact, judged alike.
  if (q["recorded"].is_array()) {
    na::Selection s;
    for (double j : doubles(q["recorded"])) {
      const auto k = static_cast<std::size_t>(j);
      if (k >= e.costs.size()) throw std::invalid_argument("allocation: a recorded project is not among the projects");
      s.funded.push_back(k), s.spent += e.costs[k];
    }
    s.said = "the selection recorded";
    judge_selection("recorded", s);
  }
  out.set("rules", list);
  out.set("voters", static_cast<double>(n));
  out.set("projects", static_cast<double>(e.costs.size()));
  return out;
}

inline Json reserves_op(const Json& q) {
  const std::size_t n = static_cast<std::size_t>(num(q["individuals"], 0));
  std::vector<na::Category> cats;
  for (const auto& c : q["categories"].as_array()) {
    na::Category x;
    x.name = c["name"].text();
    x.capacity = static_cast<std::size_t>(num(c["capacity"], 0));
    for (double i : doubles(c["priority"])) x.priority.push_back(static_cast<std::size_t>(i));
    cats.push_back(std::move(x));
  }
  std::vector<std::size_t> prec;
  for (double i : doubles(q["precedence"])) prec.push_back(static_cast<std::size_t>(i));
  if (prec.empty())
    for (std::size_t c = 0; c < cats.size(); c++) prec.push_back(c);
  const auto m = na::sequential_reserve(n, cats, prec);
  const auto ax = na::check_axioms(n, cats, m.category_of);
  Json holders = Json::array();
  for (const auto& h : m.holders) holders.push(Json(std::vector<double>(h.begin(), h.end())));
  Json orders = Json::array();
  if (cats.size() <= 8)
    for (const auto& o : na::every_order(n, cats))
      orders.push(Json::object({{"precedence", Json(std::vector<double>(o.precedence.begin(), o.precedence.end()))}, {"served", Json(std::vector<double>(o.served.begin(), o.served.end()))}}));
  return Json::object({{"holders", holders}, {"served_order", Json(std::vector<double>(m.served_order.begin(), m.served_order.end()))},
                       {"axioms", Json::object({{"eligible", ax.eligible}, {"non_wasteful", ax.non_wasteful}, {"respects_priorities", ax.respects_priorities}, {"violations", texts(ax.violations)}})},
                       {"orders", orders}});
}

// ---- Incidence ------------------------------------------------------------------------------------------------------------------

inline ai::Interval interval_of(const Json& j, double fallback) {
  if (j.is_array() && j.size() == 2) return {j[0].as_number(), j[1].as_number()};
  if (j.is_number()) return {j.as_number(), j.as_number()};
  return {fallback, fallback};
}

inline Json incidence_op(const Json& q) {
  Json out = Json::object();
  auto part = [&](const char* key, auto f) {
    if (!q.has(key)) return;
    try {
      out.set(key, f(q[key]));
    } catch (const std::exception& e) {
      out.set(key, Json::object({{"refused", e.what()}}));
    }
  };
  part("capitalisation", [](const Json& c) {
    ai::Capitalisation k{num(c["value_per_unit"], 1), interval_of(c["migration"], 1), interval_of(c["supply"], 1)};
    const auto S = ai::capitalise(num(c["amount"], 0), num(c["renters_share"], 0), num(c["landlords_local"], 0), k);
    auto iv = [](const ai::Interval& i) { return Json::array({r6(i.lo), r6(i.hi)}); };
    return Json::object({{"share", iv(S.share)}, {"renters", iv(S.renters)}, {"owner_occupiers", r6(S.owner_occupiers)}, {"landlords_local", iv(S.landlords_local)},
                         {"landlords_away", iv(S.landlords_away)}, {"said", S.said}});
  });
  part("propagation", [](const Json& p) {
    const auto P = ai::propagate(num(p["amount"], 0), num(p["spent"], 0), num(p["kept"], 0), static_cast<std::size_t>(num(p["rounds"], 3)));
    return Json::object({{"rounds", numbers(P.rounds)}, {"total", r6(P.total)}, {"multiplier", r6(P.multiplier)}, {"lm3", r6(P.lm3)}, {"leaked", r6(P.leaked)}});
  });
  part("lm3", [](const Json& l) { return Json(r6(ai::lm3(l[0].as_number(), l[1].as_number(), l[2].as_number()))); });
  part("network", [](const Json& n) {
    economics::TransferNetwork N;
    N.positions = strings(n["positions"]);
    for (const auto& l : n["legs"].as_array())
      N.legs.push_back({static_cast<std::size_t>(l[0].as_number()), static_cast<std::size_t>(l[1].as_number()), 1.0, l[2].as_number(), l.size() > 3 ? l[3].text() : std::string()});
    std::vector<std::size_t> g;
    for (double x : doubles(n["generators"])) g.push_back(static_cast<std::size_t>(x));
    const auto G = ai::returns(N, g);
    Json cap = Json::array(), kinds = Json::array();
    for (const auto& [p, v] : G.captured) cap.push(Json::object({{"position", N.positions[p]}, {"net", r6(v)}}));
    for (const auto& [k, v] : G.circulating_by_kind) kinds.push(Json::object({{"kind", k}, {"circulating", r6(v)}}));
    return Json::object({{"sent", numbers(G.sent)}, {"returned", numbers(G.returned)}, {"ratio", numbers(G.ratio)}, {"extraction", numbers(G.extraction)},
                         {"pooled_ratio", r6(G.pooled_ratio)}, {"captured", cap}, {"circulating", r6(G.circulating)}, {"by_kind", kinds}, {"said", G.said}});
  });
  part("doughnut", [](const Json& d) {
    const auto D = ai::doughnut(doubles(d["per_head"]), doubles(d["people"]), num(d["floor"], 0), num(d["ceiling"], 1e300));
    return Json::object({{"shortfall", numbers(D.shortfall)}, {"overshoot", numbers(D.overshoot)}, {"shortfall_total", r6(D.shortfall_total)}, {"overshoot_total", r6(D.overshoot_total)},
                         {"people_below", r6(D.people_below)}, {"people_above", r6(D.people_above)}});
  });
  part("targeting", [](const Json& t) {
    const auto T = ai::targeting(doubles(t["amount"]), doubles(t["population"]), doubles(t["meant"]));
    return Json::object({{"reaching", r6(T.reaching)}, {"meant", r6(T.meant)}, {"missed", r6(T.missed)}, {"per_person_meant", r6(T.per_person_meant)}, {"said", T.said}});
  });
  return out;
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "run") return run_op(q);
  if (op == "judge") return judge_op(q);
  if (op == "procedure") return procedure_op(q);
  if (op == "claims") return claims_op(q);
  if (op == "budget") return budget_op(q);
  if (op == "reserves") return reserves_op(q);
  if (op == "incidence") return incidence_op(q);
  throw std::invalid_argument("allocation: no operation \"" + op + "\"");
}

}  // namespace science::allocation
