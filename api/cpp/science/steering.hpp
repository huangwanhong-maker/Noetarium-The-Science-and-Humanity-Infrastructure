// Steering a system known through evidence, for any work (the design of 2026-11-19, development/ideas/2026-11-19-an-engagement-with-a-city.md):
// the foundation kybernesis composed with the API's JSON. An engagement keeps its trajectory (kybernesis's record): every block of the
// loop at every turn, made by a method, observed, entered by a person, amended or reviewed, with its files, sealed in archeion's journal,
// branched and revisited. The transfer of a practice from one system to another is assessed by a lens (kybernesis's transfer).
//
// Requests ({"op": …}), each answered by `answer`:
//
//   architecture  {script}                          the architecture read and verified: {admitted, checks, printed, faults}
//   methods       {}                                the engagement's predefined methods, block by block, with what each does
//   open          {script, rules, by, when}         a new trajectory: {trajectory}
//   act           {trajectory, act, …}              one act on it: observe, fill, amend, review, choose, attach, initial, branch, revise
//                                                   → {trajectory, outcome: {holds, undecided, reason, entry}}
//   turn          {trajectory, branch, turn, when, from?, carry?, by?, role?}  a turn run by the methods chosen (from: every turn from
//                                                   there, again; carry: the boundaries' values carried from the turn before first, by
//                                                   `by` in `role`) → {trajectory, progress: {made, waiting, stale, carried}}
//   view          {trajectory, branch?}             the trajectory as a page reads it: per branch and turn, each block's entry
//   verify        {trajectory}                      whether every digest and the journal hold
//   replay        {trajectory}                      every value a predefined method made, recomputed
//   diff          {trajectory, a: {branch, turn}, b: {branch, turn}}   what differs
//   resting       {trajectory, entry? | file? | agent?}               what rests on it
//   prov          {trajectory}                      the record as W3C PROV-JSON
//   transfer      {source, target, readings, actions?, tolerance?}    a practice carried by a lens, its support factors (below)
//
// A trajectory travels as JSON (noetarium.trajectory/1): the architecture's script, the rules of authority, the first values, the
// branches, the entries, the values by digest, the files by digest and the journal. It is verified whenever it is read again, and
// refused, with the first fault named, when it does not verify: a page cannot alter it unnoticed.
//
// Values are JSON texts. The engagement's methods fill the standard architecture's blocks on such values; a block may have several,
// the first its default, and which fills it from a turn on is chosen by an act of its own (kybernesis's record: choose), so that old
// evidence can be read again by another method on a branch (alternatives()):
//
//   Evidence        the observed items kept as traces, each with an id, and the readings they were computed under as words
//   Interpretation  the constitution's open terms with their admissible readings and sources, and every reading the data used
//   Estimation      for each quantity of each place ("quantity @ place", never mixed) under each combination of readings:
//                   what the evidence allows (the default): the intersection of the items' intervals when they agree (sound when each
//                     is), their hull with the conflict said when they do not;
//                   pooled by fixed effect, or by random effects (REML): each item's interval read as a 95% interval of an estimate,
//                     pooled by inverse variance (peira's synthesis), heterogeneity reported; items with no interval set aside, said;
//                   the latest item: the last item observed, the others set aside, said;
//                   in every method a quantity not measured this turn keeps the prior's estimate, marked carried
//   Interpretation  declared readings (the default): the constitution's terms and every reading the data used; or the constitution's
//                   readings only, the data's set aside
//   Evaluation      each declared operator under every admissible reading of its terms: holds where the estimate lies wholly on its
//                   side of the standard, fails wholly on the other, open between; standing clear, untenable or arguable over the
//                   readings, measure the estimate with its interval; the terms it turns on
//   PolicyMaking    the findings to be taken up (needs, problems and risks that are clear or arguable), the practices that answer
//                   them, the proposals, and what is open (the terms findings turn on, the conflicts among sources)
//   Policy          the advice as chosen in the policy version (a person's choice), and what is open
//   Decision        the advice as the intervention; what is open as the inquiry
//
// A transfer is declared by components: named conditions of a system (each true or false) and an outcome (success or not). Each
// system gives, for each action, the probability of success by rules over its conditions (the first rule whose conditions all match);
// conditions persist. The target is read in the source's terms by its components' names (the lens is the identity on what they share,
// and a source action is realised by the target action of the same name). Each reading gives the probability of each target condition
// (independent, unless stated otherwise), and each condition is a support factor.

#pragma once

#include "science/json.hpp"

#include <kybernesis/architecture/checks.hpp>
#include <kybernesis/architecture/language.hpp>
#include <kybernesis/record/record.hpp>
#include <kybernesis/transfer/transfer.hpp>
#include <peira/synthesis/pooling.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::steering {

namespace kr = kybernesis::record;
namespace kt = kybernesis::transfer;
namespace ka = kybernesis::architecture;
namespace kk = kybernesis::kernel;

inline constexpr const char* SCHEMA = "noetarium.trajectory/1";

// ---- values as JSON ---------------------------------------------------------------------------------------------------------

namespace detail {

inline std::string str(const Json& j, const char* key, const std::string& fallback = {}) {
  const Json* v = j.find(key);
  return v && v->is_string() ? v->as_string() : fallback;
}

inline double num(const Json& j, const char* key) {
  const Json* v = j.find(key);
  return v && v->is_number() ? v->as_number() : std::nan("");
}

inline Json parse_or(const std::string& text, Json fallback) {
  try {
    return Json::parse(text);
  } catch (...) {
    return fallback;
  }
}

inline Json verdict(const kk::Verdict& v) {
  return Json::object({{"holds", v.holds()}, {"undecided", v.undecided()}, {"claim", v.claim}, {"reason", v.reason}});
}

inline const char* json_kind(const std::string& type) {
  static const std::map<std::string, const char*> kinds{{"Traces", "array"},      {"Words", "array"},       {"Terms", "object"},     {"Operators", "array"},
                                                        {"Standards", "array"},   {"Rules", "array"},       {"Proposals", "array"},  {"Practice", "array"},
                                                        {"Readings", "object"},   {"Belief", "object"},     {"Evaluations", "array"}, {"PolicyVersion", "object"},
                                                        {"Action", "object"},     {"Inquiry", "object"}};
  const auto it = kinds.find(type);
  return it == kinds.end() ? "" : it->second;
}

}  // namespace detail

// Each type of the standard architecture carries a JSON text of a declared kind (an array or an object).
inline std::map<std::string, kr::Check> checks(const ka::Architecture& a) {
  std::map<std::string, kr::Check> out;
  for (const auto& t : a.types) {
    const std::string want = detail::json_kind(t.name);
    out[t.name] = [want, name = t.name](const std::string& text) -> kk::Verdict {
      const std::string claim = "a value of " + name;
      Json j;
      try {
        j = Json::parse(text);
      } catch (const std::exception& e) {
        return kk::Verdict::no(claim, std::string("it is not JSON: ") + e.what());
      }
      if (want == "array" && !j.is_array()) return kk::Verdict::no(claim, "it is a JSON array");
      if (want == "object" && !j.is_object()) return kk::Verdict::no(claim, "it is a JSON object");
      return kk::Verdict::yes(claim);
    };
  }
  return out;
}

// ---- the engagement's methods ------------------------------------------------------------------------------------------------

namespace detail {

// The signature of a set of readings: "term=reading" joined by "; ", in the terms' order; "" for none.
inline std::string signature(const std::map<std::string, std::string>& readings) {
  std::string s;
  for (const auto& [t, r] : readings) s += (s.empty() ? "" : "; ") + t + "=" + r;
  return s;
}

inline std::map<std::string, std::string> readings_of(const Json& item) {
  std::map<std::string, std::string> out;
  if (const Json* r = item.find("readings"); r && r->is_object())
    for (const auto& [k, v] : r->as_object())
      if (v.is_string()) out[k] = v.as_string();
  return out;
}

inline Json estimate_of(const std::vector<const Json*>& items) {
  double lo = -INFINITY, hi = INFINITY, hull_lo = INFINITY, hull_hi = -INFINITY, sum = 0;
  std::size_t values = 0, bounded = 0;
  Json ids = Json::array(), sources = Json::array(), levels = Json::array();
  for (const Json* it : items) {
    ids.push(str(*it, "id"));
    if (!str(*it, "source").empty()) sources.push(str(*it, "source"));
    if (!str(*it, "level").empty()) levels.push(str(*it, "level"));
    const double v = num(*it, "value"), l = num(*it, "low"), h = num(*it, "high");
    if (!std::isnan(v)) sum += v, values++;
    const double a = !std::isnan(l) ? l : v, b = !std::isnan(h) ? h : v;
    if (!std::isnan(a) && !std::isnan(b)) {
      bounded++;
      lo = std::max(lo, a), hi = std::min(hi, b);
      hull_lo = std::min(hull_lo, a), hull_hi = std::max(hull_hi, b);
    }
  }
  Json e = Json::object();
  e.set("items", ids);
  if (sources.size()) e.set("sources", sources);
  if (levels.size()) e.set("levels", levels);
  if (bounded == 0) {
    e.set("value", values ? Json(sum / static_cast<double>(values)) : Json());
    return e;
  }
  const bool agree = lo <= hi;
  const double low = agree ? lo : hull_lo, high = agree ? hi : hull_hi;
  double value = values ? sum / static_cast<double>(values) : (low + high) / 2;
  if (agree && !(value >= low && value <= high)) value = (low + high) / 2;
  e.set("value", value);
  e.set("low", low);
  e.set("high", high);
  e.set("conflict", !agree);
  if (!agree) e.set("remark", "the sources' intervals do not meet: their hull is kept, and what the evidence allows is not settled");
  return e;
}

// An estimate by a method: "intersection" (what the evidence allows), "fixed" or "random" (pooled by inverse variance, each
// interval read as a 95% interval, peira's synthesis), "latest" (the last item).
inline Json estimate_by(const std::vector<const Json*>& items, const std::string& mode) {
  if (mode == "latest") {
    Json e = estimate_of({items.back()});
    if (items.size() > 1) e.set("remark", "the latest of " + std::to_string(items.size()) + " items; the others set aside");
    return e;
  }
  if (mode != "fixed" && mode != "random") return estimate_of(items);
  const double z = 1.959963984540054;
  std::vector<double> y, v;
  Json used = Json::array(), set_aside = Json::array(), sources = Json::array();
  for (const Json* it : items) {
    const double val = num(*it, "value"), lo = num(*it, "low"), hi = num(*it, "high");
    const double centre = !std::isnan(val) ? val : (lo + hi) / 2;
    if (std::isnan(lo) || std::isnan(hi) || !(hi > lo) || std::isnan(centre)) {
      set_aside.push(str(*it, "id"));
      continue;
    }
    const double se = (hi - lo) / (2 * z);
    y.push_back(centre), v.push_back(se * se);
    used.push(str(*it, "id"));
    if (!str(*it, "source").empty()) sources.push(str(*it, "source"));
  }
  if (y.empty()) {
    Json e = estimate_of(items);
    e.set("remark", "no item has an interval to pool; what the evidence allows is given instead");
    return e;
  }
  peira::synthesis::PoolOptions o;
  o.model = mode == "random" && y.size() > 1 ? peira::synthesis::Model::random : peira::synthesis::Model::fixed;
  const auto pooled = peira::synthesis::pool(y, v, o);
  Json e = Json::object({{"items", used}, {"value", pooled.estimate}, {"low", pooled.lower}, {"high", pooled.upper}, {"conflict", false}});
  if (sources.size()) e.set("sources", sources);
  Json pd = Json::object({{"k", static_cast<double>(pooled.k)}, {"model", o.model == peira::synthesis::Model::random ? "random effects (REML)" : "fixed effect"},
                          {"tau2", pooled.tau2}, {"se", pooled.se}});
  if (y.size() > 1) pd.set("i2", pooled.heterogeneity.i2);
  e.set("pooled", pd);
  std::string remark;
  if (set_aside.size()) e.set("set_aside", set_aside), remark = std::to_string(set_aside.size()) + " items without an interval set aside";
  if (mode == "random" && y.size() == 1) remark += std::string(remark.empty() ? "" : "; ") + "one item: random effects cannot be estimated, the fixed effect is given";
  if (!remark.empty()) e.set("remark", remark);
  return e;
}

// Every combination of the readings of some terms, each with its source.
inline std::vector<std::map<std::string, std::string>> combinations(const Json& readings, const std::vector<std::string>& terms) {
  std::vector<std::map<std::string, std::string>> out{{}};
  for (const auto& t : terms) {
    const Json* rs = readings.find(t);
    std::vector<std::string> options;
    if (rs && rs->is_array())
      for (const auto& r : rs->as_array())
        if (!str(r, "reading").empty()) options.push_back(str(r, "reading"));
    if (options.empty()) continue;
    std::vector<std::map<std::string, std::string>> next;
    for (const auto& c : out)
      for (const auto& o : options) {
        auto d = c;
        d[t] = o;
        next.push_back(std::move(d));
      }
    out = std::move(next);
  }
  return out;
}

// The belief's entry for an operator's quantity: "quantity @ place" when the operator names its place, else the quantity itself, else
// the one place the quantity was measured at (several places and none named: no entry, so that places are never mixed).
inline const Json* entry_of(const Json& belief, const std::string& quantity, const std::string& place) {
  if (!place.empty()) return belief.find(quantity + " @ " + place);
  if (const Json* q = belief.find(quantity)) return q;
  const Json* found = nullptr;
  if (belief.is_object())
    for (const auto& [k, v] : belief.as_object())
      if (k.rfind(quantity + " @ ", 0) == 0) {
        if (found) return nullptr;
        found = &v;
      }
  return found;
}

inline const Json* estimate_under(const Json& belief, const std::string& quantity, const std::map<std::string, std::string>& readings, const std::string& place = {}) {
  const Json* q = entry_of(belief, quantity, place);
  if (!q || !q->is_object()) return nullptr;
  const Json* by = q->find("by_readings");
  if (!by || !by->is_object()) return nullptr;
  // the estimate computed under readings that agree with these on every term it names; the unconditioned one otherwise
  const Json* fallback = nullptr;
  for (const auto& [sig, est] : by->as_object()) {
    if (sig.empty()) {
      fallback = &est;
      continue;
    }
    bool agrees = true;
    for (const auto& part : [&] {
           std::vector<std::string> ps;
           std::string cur;
           for (std::size_t i = 0; i <= sig.size(); i++) {
             if (i == sig.size() || (sig[i] == ';' && i + 1 < sig.size() && sig[i + 1] == ' ')) {
               ps.push_back(cur), cur.clear();
               if (i < sig.size()) i++;
             } else {
               cur += sig[i];
             }
           }
           return ps;
         }()) {
      const auto eq = part.find('=');
      if (eq == std::string::npos) continue;
      const auto it = readings.find(part.substr(0, eq));
      if (it != readings.end() && it->second != part.substr(eq + 1)) agrees = false;
    }
    if (agrees) return &est;
  }
  return fallback;
}

inline double standard_under(const Json& op, const Json& standards, const std::map<std::string, std::string>& readings, std::string& held_by) {
  // the operator's own standard: a number, or {term, by_reading: {reading: number}}; or a standard of the constitution by its id
  const Json* s = op.find("standard");
  const Json* chosen = s;
  if (s && s->is_string())
    for (const auto& st : standards.as_array())
      if (str(st, "id") == s->as_string()) chosen = &st;
  if (!chosen) return std::nan("");
  if (chosen->is_number()) return chosen->as_number();
  if (!chosen->is_object()) return std::nan("");
  held_by = str(*chosen, "held_by");
  if (const Json* v = chosen->find("value"); v && v->is_number()) return v->as_number();
  const std::string term = str(*chosen, "term");
  const Json* by = chosen->find("by_reading");
  if (by && by->is_object()) {
    const auto it = readings.find(term);
    if (it != readings.end())
      if (const Json* v = by->find(it->second); v && v->is_number()) return v->as_number();
  }
  return std::nan("");
}

}  // namespace detail

inline kr::Method method(const std::string& name, const std::string& wraps, kk::Family family, kr::Step step) { return {name, wraps, family, std::move(step)}; }

// Estimation's step by a mode (estimate_by): for each quantity of each place under each combination of readings.
inline kr::Step estimation(const std::string& mode) {
  return [mode](const std::vector<std::string>& in) {
    const Json traces = Json::parse(in.at(0)), prior = detail::parse_or(in.at(2), Json::object());
    std::map<std::string, std::map<std::string, std::vector<const Json*>>> groups;
    std::map<std::string, Json> meta;
    for (const auto& it : traces.as_array()) {
      // a quantity of one place is kept apart from the same quantity of another: "quantity @ place"
      std::string q = detail::str(it, "quantity");
      if (q.empty()) continue;
      std::string where = detail::str(it, "unit_of");
      if (const Json* p = it.find("place"); where.empty() && p && p->is_object()) where = detail::str(*p, "name");
      if (!where.empty()) q += " @ " + where;
      groups[q][detail::signature(detail::readings_of(it))].push_back(&it);
      if (!meta.count(q)) {
        Json m = Json::object();
        for (const char* key : {"label", "unit", "scale"})
          if (!detail::str(it, key).empty()) m.set(key, detail::str(it, key));
        meta[q] = m;
      }
    }
    Json belief = Json::object();
    if (prior.is_object())
      for (const auto& [q, est] : prior.as_object())
        if (!groups.count(q)) {
          Json carried = est;
          carried.set("carried", true);
          belief.set(q, carried);
        }
    for (const auto& [q, by] : groups) {
      Json entry = meta[q];
      Json readings = Json::object();
      for (const auto& [sig, items] : by) readings.set(sig, detail::estimate_by(items, mode));
      entry.set("by_readings", readings);
      entry.set("carried", false);
      belief.set(q, entry);
    }
    return std::vector<std::string>{belief.dump()};
  };
}

// The predefined methods of an engagement, by block of the standard architecture: each block's default.
inline std::map<std::string, kr::Method> methods() {
  std::map<std::string, kr::Method> m;
  m["Evidence"] = method("traces kept", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    const Json items = Json::parse(in.at(0));
    Json traces = Json::array(), words = Json::array();
    std::size_t k = 0;
    for (const auto& it : items.as_array()) {
      Json t = it;
      if (detail::str(t, "id").empty()) t.set("id", "t" + std::to_string(k));
      for (const auto& [term, reading] : detail::readings_of(t))
        words.push(Json::object({{"term", term}, {"reading", reading}, {"source", "data"}, {"item", detail::str(t, "id")}}));
      traces.push(t);
      k++;
    }
    return std::vector<std::string>{traces.dump(), words.dump()};
  });
  m["Interpretation"] = method("declared readings", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    const Json words = Json::parse(in.at(0)), terms = Json::parse(in.at(1));
    Json out = terms;
    for (const auto& w : words.as_array()) {
      const std::string term = detail::str(w, "term"), reading = detail::str(w, "reading");
      if (term.empty() || reading.empty()) continue;
      Json list = out.find(term) && (*out.find(term)).is_array() ? *out.find(term) : Json::array();
      bool known = false;
      for (const auto& r : list.as_array()) known = known || detail::str(r, "reading") == reading;
      if (!known) list.push(Json::object({{"reading", reading}, {"source", "data"}}));
      out.set(term, list);
    }
    return std::vector<std::string>{out.dump()};
  });
  m["Estimation"] = method("what the evidence allows", "", kk::Family::statistical, estimation("intersection"));
  m["Evaluation"] = method("operators under every reading", "", kk::Family::formal, [](const std::vector<std::string>& in) {
    const Json belief = Json::parse(in.at(0)), readings = Json::parse(in.at(1)), operators = Json::parse(in.at(2)), standards = Json::parse(in.at(3));
    Json findings = Json::array();
    for (const auto& op : operators.as_array()) {
      std::vector<std::string> terms;
      if (const Json* ts = op.find("terms"); ts && ts->is_array())
        for (const auto& t : ts->as_array())
          if (t.is_string()) terms.push_back(t.as_string());
      if (const Json* s = op.find("standard"); s && s->is_object() && !detail::str(*s, "term").empty()) terms.push_back(detail::str(*s, "term"));
      std::sort(terms.begin(), terms.end());
      terms.erase(std::unique(terms.begin(), terms.end()), terms.end());
      const std::string quantity = detail::str(op, "quantity"), side = detail::str(op, "holds_when", ">=");
      std::size_t holds = 0, fails = 0, missing = 0;
      Json by = Json::array();
      std::map<std::string, std::set<std::string>> status_by_term;  // term -> reading -> statuses seen
      std::map<std::string, std::map<std::string, std::set<std::string>>> seen;
      double first_value = std::nan(""), first_low = std::nan(""), first_high = std::nan("");
      std::string held_by;
      const auto combos = detail::combinations(readings, terms);
      for (const auto& c : combos) {
        const Json* est = detail::estimate_under(belief, quantity, c, detail::str(op, "place"));
        std::string hb;
        const double threshold = detail::standard_under(op, standards, c, hb);
        if (!hb.empty()) held_by = hb;
        std::string status = "undetermined";
        Json row = Json::object();
        Json rj = Json::object();
        for (const auto& [t, r] : c) rj.set(t, r);
        row.set("readings", rj);
        row.set("standard", std::isnan(threshold) ? Json() : Json(threshold));
        if (est && !std::isnan(threshold)) {
          const double v = detail::num(*est, "value"), lo = std::isnan(detail::num(*est, "low")) ? v : detail::num(*est, "low"),
                       hi = std::isnan(detail::num(*est, "high")) ? v : detail::num(*est, "high");
          if (std::isnan(first_value)) first_value = v, first_low = lo, first_high = hi;
          const bool ge = side != "<=";
          if (ge ? lo >= threshold : hi <= threshold) status = "holds";
          else if (ge ? hi < threshold : lo > threshold) status = "fails";
          else status = "open";
          row.set("estimate", Json::object({{"value", v}, {"low", lo}, {"high", hi}}));
        }
        holds += status == "holds", fails += status == "fails", missing += status == "undetermined";
        row.set("status", status);
        for (const auto& [t, r] : c) seen[t][r].insert(status);
        by.push(row);
      }
      const std::size_t n = combos.size();
      const char* standing = missing ? "undecided" : holds == n ? "clear" : fails == n ? "untenable" : "arguable";
      // the terms it turns on: those whose readings, held apart, see different statuses
      Json turns = Json::array();
      for (const auto& [t, rs] : seen) {
        std::set<std::set<std::string>> distinct;
        for (const auto& [r, sts] : rs) distinct.insert(sts);
        if (distinct.size() > 1) turns.push(t);
      }
      Json f = Json::object({{"operator", detail::str(op, "id")}, {"label", detail::str(op, "label")}, {"kind", detail::str(op, "kind", "feature")},
                             {"quantity", quantity}, {"holds_when", side}, {"standing", standing}, {"by_reading", by}, {"turns_on", turns}});
      if (!detail::str(op, "place").empty()) f.set("place", detail::str(op, "place"));
      if (!held_by.empty()) f.set("held_by", held_by);
      if (!detail::str(op, "scale").empty()) f.set("scale", detail::str(op, "scale"));
      if (!std::isnan(first_value)) f.set("measure", Json::object({{"value", first_value}, {"low", first_low}, {"high", first_high}}));
      if (missing) f.set("remark", std::to_string(missing) + " of " + std::to_string(n) + " readings have no estimate or no standard");
      findings.push(f);
    }
    return std::vector<std::string>{findings.dump()};
  });
  m["PolicyMaking"] = method("practices answering findings", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    const Json findings = Json::parse(in.at(0)), proposals = Json::parse(in.at(2)), practice = Json::parse(in.at(3)), current = detail::parse_or(in.at(5), Json::object());
    Json taken = Json::array(), open = Json::array(), candidates = Json::array();
    std::set<std::string> ops;
    for (const auto& f : findings.as_array()) {
      const std::string kind = detail::str(f, "kind"), st = detail::str(f, "standing");
      if ((kind == "need" || kind == "problem" || kind == "risk") && (st == "clear" || st == "arguable")) {
        taken.push(Json::object({{"operator", detail::str(f, "operator")}, {"label", detail::str(f, "label")}, {"kind", kind}, {"standing", st}}));
        ops.insert(detail::str(f, "operator"));
      }
      if (const Json* t = f.find("turns_on"); t && t->is_array())
        for (const auto& term : t->as_array()) open.push(Json::object({{"term", term}, {"finding", detail::str(f, "operator")}}));
    }
    for (const auto& p : practice.as_array()) {
      bool answers = false;
      if (const Json* a = p.find("answers"); a && a->is_array())
        for (const auto& x : a->as_array()) answers = answers || (x.is_string() && ops.count(x.as_string()));
      if (answers) candidates.push(p);
    }
    const double version = std::isnan(detail::num(current, "version")) ? 0 : detail::num(current, "version");
    Json policy = Json::object({{"version", version + 1}, {"based_on", version}, {"taken_up", taken}, {"candidates", candidates}, {"proposals", proposals}, {"open", open}});
    if (current.find("chosen") && !detail::str(current, "chosen").empty()) policy.set("previously_chosen", detail::str(current, "chosen"));
    return std::vector<std::string>{policy.dump()};
  });
  m["Policy"] = method("advice as chosen", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    const Json policy = Json::parse(in.at(0));
    const std::string chosen = detail::str(policy, "chosen");
    Json action = Json::object({{"advice", chosen.empty() ? Json() : Json(chosen)}, {"status", chosen.empty() ? "none chosen" : "advised"}});
    if (const Json* o = policy.find("open")) action.set("open", *o);
    return std::vector<std::string>{action.dump(), policy.dump()};
  });
  m["Decision"] = method("intervene or inquire", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    const Json action = Json::parse(in.at(0));
    Json intervention = Json::object({{"advice", action.find("advice") ? *action.find("advice") : Json()}});
    Json inquiry = Json::object({{"ask", action.find("open") ? *action.find("open") : Json::array()}});
    return std::vector<std::string>{intervention.dump(), inquiry.dump()};
  });
  return m;
}

// Every block's alternative methods, its default first.
inline kr::Registry alternatives() {
  kr::Registry r = kr::registry_of(methods());
  r["Estimation"].push_back(method("pooled by fixed effect", "peira::synthesis::pool", kk::Family::statistical, estimation("fixed")));
  r["Estimation"].push_back(method("pooled by random effects", "peira::synthesis::pool", kk::Family::statistical, estimation("random")));
  r["Estimation"].push_back(method("the latest item", "", kk::Family::statistical, estimation("latest")));
  r["Interpretation"].push_back(method("the constitution's readings only", "", kk::Family::declared, [](const std::vector<std::string>& in) {
    return std::vector<std::string>{Json::parse(in.at(1)).dump()};
  }));
  return r;
}

inline Json methods_json() {
  static const std::map<std::string, std::string> others{
      {"pooled by fixed effect", "each item's interval read as a 95% interval of an estimate, pooled by inverse variance (peira's synthesis, the fixed effect); items without an interval set aside, said"},
      {"pooled by random effects", "as the fixed effect, with the variance between items estimated by REML (peira's synthesis), and the heterogeneity reported"},
      {"the latest item", "the last item observed for each quantity and reading; the others set aside, said"},
      {"the constitution's readings only", "the constitution's open terms and their readings; readings the data used are set aside"}};
  static const std::map<std::string, std::string> about{
      {"Evidence", "the observed items kept as traces, each with an id; the readings they were computed under, as words"},
      {"Interpretation", "the constitution's open terms with their readings and sources, and every reading the data used"},
      {"Estimation", "for each quantity under each combination of readings, what the evidence allows: the intersection of the items' intervals when they agree, their hull with the conflict said when they do not; a quantity not measured keeps the prior's estimate, marked carried"},
      {"Evaluation", "each declared operator under every admissible reading of its terms: holds, fails or open against its standard; standing over the readings; the terms it turns on"},
      {"PolicyMaking", "the needs, problems and risks to be taken up, the practices that answer them, the proposals, and what is open"},
      {"Policy", "the advice as chosen in the policy version, and what is open"},
      {"Decision", "the advice as the intervention, what is open as the inquiry"}};
  Json out = Json::array();
  for (const auto& [block, list] : alternatives())
    for (std::size_t k = 0; k < list.size(); k++) {
      const auto& m = list[k];
      const std::string does = k == 0 ? (about.count(block) ? about.at(block) : "") : (others.count(m.name) ? others.at(m.name) : "");
      out.push(Json::object({{"block", block}, {"method", m.name}, {"family", kk::name(m.family)}, {"does", does}, {"default", k == 0}}));
    }
  return out;
}

// ---- the trajectory as JSON ---------------------------------------------------------------------------------------------------

namespace detail {

inline Json strings(const std::vector<std::string>& xs) { return Json(xs); }

inline std::vector<std::string> strings_of(const Json* j) {
  std::vector<std::string> out;
  if (j && j->is_array())
    for (const auto& x : j->as_array()) out.push_back(x.is_string() ? x.as_string() : std::string());
  return out;
}

}  // namespace detail

inline Json entry_json(const kr::Entry& e) {
  Json fed = Json::array();
  for (long k : e.fed_by) fed.push(static_cast<double>(k));
  return Json::object({{"id", static_cast<double>(e.id)}, {"branch", e.branch}, {"turn", static_cast<double>(e.turn)}, {"block", e.block}, {"act", kr::name(e.act)},
                       {"by", e.by}, {"role", e.role}, {"family", kk::name(e.family)}, {"reason", e.reason}, {"fed_by", fed}, {"inputs", detail::strings(e.inputs)},
                       {"outputs", detail::strings(e.outputs)}, {"computed", detail::strings(e.computed)}, {"target", static_cast<double>(e.target)}, {"accepted", e.accepted},
                       {"files", detail::strings(e.files)}, {"architecture", e.architecture}, {"when", e.when}, {"counts", e.counts}, {"remarks", detail::strings(e.remarks)},
                       {"digest", e.digest}});
}

inline kr::Entry entry_from(const Json& j) {
  kr::Entry e;
  e.id = static_cast<std::size_t>(j["id"].as_number());
  e.branch = j["branch"].as_string(), e.turn = static_cast<std::size_t>(j["turn"].as_number()), e.block = j["block"].as_string();
  e.act = kr::act_of(j["act"].as_string()), e.by = j["by"].as_string(), e.role = j["role"].as_string(), e.family = kk::family_of(j["family"].as_string());
  e.reason = j["reason"].as_string();
  for (const auto& k : j["fed_by"].as_array()) e.fed_by.push_back(static_cast<long>(k.as_number()));
  e.inputs = detail::strings_of(j.find("inputs")), e.outputs = detail::strings_of(j.find("outputs")), e.computed = detail::strings_of(j.find("computed"));
  e.target = static_cast<long>(j["target"].as_number()), e.accepted = j["accepted"].as_bool(), e.files = detail::strings_of(j.find("files"));
  e.architecture = j["architecture"].as_string(), e.when = j["when"].as_string(), e.counts = j["counts"].as_bool(), e.remarks = detail::strings_of(j.find("remarks"));
  e.digest = j["digest"].as_string();
  return e;
}

inline Json rules_json(const kr::Rules& r) {
  Json out = Json::array();
  for (const auto& g : r.grants) out.push(Json::object({{"block", g.block}, {"act", kr::name(g.act)}, {"role", g.role}}));
  return out;
}

inline kr::Rules rules_from(const Json& j) {
  kr::Rules r;
  if (j.is_array())
    for (const auto& g : j.as_array()) r.grants.push_back({detail::str(g, "block", "*"), kr::act_of(detail::str(g, "act", "fill")), detail::str(g, "role", "*")});
  return r;
}

inline Json trajectory_json(const kr::Trajectory& t) {
  Json branches = Json::array(), entries = Json::array(), values = Json::object(), files = Json::object(), journal = Json::array(), initial = Json::object(),
       choices = Json::array();
  for (const auto& c : t.choices)
    choices.push(Json::object({{"branch", c.branch}, {"block", c.block}, {"method", c.method}, {"by", c.by}, {"role", c.role}, {"reason", c.reason},
                               {"when", c.when}, {"from", static_cast<double>(c.from)}, {"at", static_cast<double>(c.at)}}));
  for (const auto& b : t.branches)
    branches.push(Json::object({{"name", b.name}, {"parent", b.parent}, {"from", static_cast<double>(b.from)}, {"since", static_cast<double>(b.since)}, {"reason", b.reason},
                                {"by", b.by}, {"when", b.when}}));
  for (const auto& e : t.entries) entries.push(entry_json(e));
  for (const auto& [d, p] : t.values) values.set(d, p);
  for (const auto& [d, f] : t.files) files.set(d, Json::object({{"name", f.name}, {"kind", f.kind}, {"bytes", static_cast<double>(f.bytes)}}));
  for (const auto& j : t.journal.entries)
    journal.push(Json::object({{"when", j.when}, {"who", j.who}, {"what", j.what}, {"about", j.about}, {"note", j.note}, {"seal", j.seal}}));
  for (const auto& [w, d] : t.initial) initial.set(w, d);
  return Json::object({{"schema", SCHEMA}, {"architecture", ka::print(t.arch)}, {"arch_digest", t.arch_digest}, {"rules", rules_json(t.rules)}, {"initial", initial},
                       {"branches", branches}, {"entries", entries}, {"choices", choices}, {"values", values}, {"files", files}, {"journal", journal}});
}

// A trajectory read again, with the engagement's checks, and verified: refused with its first fault when it does not verify.
inline kr::Trajectory trajectory_from(const Json& j) {
  if (detail::str(j, "schema") != SCHEMA) throw std::invalid_argument(std::string("a trajectory is ") + SCHEMA);
  const auto reading = ka::read(j["architecture"].as_string());
  if (!reading.ok()) throw std::invalid_argument("the trajectory's architecture does not read: line " + std::to_string(reading.faults[0].line) + ": " + reading.faults[0].message);
  kr::Trajectory t;
  t.arch = reading.architecture;
  t.arch_digest = j["arch_digest"].as_string();
  t.rules = rules_from(j["rules"]);
  t.types = checks(t.arch);
  for (const auto& [w, d] : j["initial"].as_object()) t.initial[w] = d.as_string();
  for (const auto& b : j["branches"].as_array())
    t.branches.push_back({b["name"].as_string(), b["parent"].as_string(), static_cast<std::size_t>(b["from"].as_number()), static_cast<std::size_t>(b["since"].as_number()),
                          b["reason"].as_string(), b["by"].as_string(), b["when"].as_string()});
  for (const auto& e : j["entries"].as_array()) t.entries.push_back(entry_from(e));
  if (const Json* cs = j.find("choices"); cs && cs->is_array())
    for (const auto& c : cs->as_array())
      t.choices.push_back({c["branch"].as_string(), c["block"].as_string(), c["method"].as_string(), c["by"].as_string(), c["role"].as_string(), c["reason"].as_string(),
                           c["when"].as_string(), static_cast<std::size_t>(c["from"].as_number()), static_cast<std::size_t>(c["at"].as_number())});
  for (const auto& [d, p] : j["values"].as_object()) t.values[d] = p.as_string();
  for (const auto& [d, f] : j["files"].as_object()) t.files[d] = {f["name"].as_string(), d, f["kind"].as_string(), static_cast<std::uint64_t>(f["bytes"].as_number())};
  for (const auto& e : j["journal"].as_array())
    t.journal.entries.push_back({e["when"].as_string(), e["who"].as_string(), e["what"].as_string(), e["about"].as_string(), e["note"].as_string(), e["seal"].as_string()});
  if (kr::digest_of(ka::print(t.arch)) != t.arch_digest) throw std::invalid_argument("the trajectory's architecture is not the one it names");
  if (const auto v = kr::verify(t); v.fails()) throw std::invalid_argument("the trajectory does not verify: " + v.reason);
  return t;
}

// ---- reading a trajectory as a page reads it --------------------------------------------------------------------------------

inline Json view(const kr::Trajectory& t, const std::string& only = {}) {
  const kr::Registry alts = alternatives();
  Json branches = Json::array();
  for (const auto& b : t.branches) {
    if (!only.empty() && b.name != only) continue;
    const auto last = kr::last_turn(t, b.name);
    Json turns = Json::array();
    for (std::size_t turn = 0; last && turn <= *last; turn++) {
      Json blocks = Json::array();
      for (const auto& name : kr::order(t.arch)) {
        const ka::Block& blk = *t.arch.block(name);
        Json row = Json::object({{"block", name}, {"stage", blk.stage}, {"boundary", blk.boundary}, {"world", kr::referred(blk)}});
        if (const kr::Method* m = kr::pick(alts, name, kr::chosen(t, b.name, turn, name))) row.set("method", m->name);
        else if (!kr::chosen(t, b.name, turn, name).empty()) row.set("method", kr::chosen(t, b.name, turn, name));
        if (const kr::Entry* e = kr::current(t, b.name, turn, name)) {
          row.set("entry", static_cast<double>(e->id));
          row.set("act", kr::name(e->act));
          row.set("by", e->by);
          row.set("family", kk::name(e->family));
          row.set("held", !kr::holds_value(e));
          row.set("inherited", e->branch != b.name);
          if (const auto st = kr::stale(t, *e, b.name)) row.set("stale", *st);
          if (!e->remarks.empty()) row.set("remarks", detail::strings(e->remarks));
          Json outs = Json::object();
          if (kr::holds_value(e))
            for (std::size_t k = 0; k < blk.outputs.size() && k < e->outputs.size(); k++) outs.set(blk.outputs[k].name, detail::parse_or(kr::payload(t, e->outputs[k]), Json()));
          row.set("values", outs);
        }
        Json proposals = Json::array();
        for (const auto& e : t.entries)
          if (!e.counts && e.block == name && e.turn == turn && e.branch == b.name) {
            bool reviewed = false;
            for (const auto& r : t.entries) reviewed = reviewed || (r.act == kr::Act::review && r.target == static_cast<long>(e.id));
            if (!reviewed) proposals.push(Json::object({{"entry", static_cast<double>(e.id)}, {"by", e.by}, {"reason", e.reason}}));
          }
        if (proposals.size()) row.set("proposals", proposals);
        blocks.push(row);
      }
      turns.push(Json::object({{"turn", static_cast<double>(turn)}, {"blocks", blocks}}));
    }
    branches.push(Json::object({{"name", b.name}, {"parent", b.parent}, {"from", static_cast<double>(b.from)}, {"reason", b.reason}, {"turns", turns}}));
  }
  Json alt = Json::object(), choices = Json::array();
  for (const auto& [block, list] : alts) {
    Json names = Json::array();
    for (const auto& m : list) names.push(m.name);
    alt.set(block, names);
  }
  for (const auto& c : t.choices)
    choices.push(Json::object({{"branch", c.branch}, {"block", c.block}, {"method", c.method}, {"from", static_cast<double>(c.from)}, {"by", c.by}, {"reason", c.reason}, {"when", c.when}}));
  return Json::object({{"branches", branches}, {"entries", static_cast<double>(t.entries.size())}, {"head", t.journal.head()}, {"verified", detail::verdict(kr::verify(t))},
                       {"alternatives", alt}, {"choices", choices}});
}

// The record as W3C PROV-JSON (Huynh et al. 2013, PROV-JSON): entities, activities, agents and their relations.
inline Json prov(const kr::Trajectory& t) {
  const auto g = kr::lineage(t);
  Json entity = Json::object(), activity = Json::object(), agent = Json::object(), used = Json::object(), generated = Json::object(), associated = Json::object();
  for (const auto& e : g.entities) {
    Json attrs = Json::object();
    for (const auto& [c, d] : e.digests) attrs.set("noetarium:" + c, d);
    entity.set(e.id, attrs);
  }
  std::size_t u = 0, w = 0, a = 0;
  for (const auto& act : g.activities) {
    activity.set(act.id, Json::object({{"prov:type", act.kind}, {"prov:startTime", act.when}}));
    agent.set(act.agent, Json::object());
    for (const auto& x : act.used) used.set("_:u" + std::to_string(u++), Json::object({{"prov:activity", act.id}, {"prov:entity", x}}));
    for (const auto& x : act.generated) generated.set("_:g" + std::to_string(w++), Json::object({{"prov:entity", x}, {"prov:activity", act.id}}));
    associated.set("_:a" + std::to_string(a++), Json::object({{"prov:activity", act.id}, {"prov:agent", act.agent}}));
  }
  return Json::object({{"prefix", Json::object({{"noetarium", "https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure#"}})},
                       {"entity", entity}, {"activity", activity}, {"agent", agent}, {"used", used}, {"wasGeneratedBy", generated}, {"wasAssociatedWith", associated}});
}

// ---- transfer by components ----------------------------------------------------------------------------------------------------

namespace detail {

struct Declared {
  kt::System system;
  std::vector<std::string> components;
};

// States: every assignment of the components (first most significant) with the outcome (success, failure) last.
inline std::string state_name(const std::vector<std::string>& comps, std::size_t bits, bool success) {
  std::string s;
  for (std::size_t c = 0; c < comps.size(); c++) s += (s.empty() ? "" : ", ") + std::string(((bits >> (comps.size() - 1 - c)) & 1) ? "" : "not ") + comps[c];
  return s + (success ? " | success" : " | failure");
}

inline bool component(std::size_t bits, std::size_t c, std::size_t n) { return (bits >> (n - 1 - c)) & 1; }

inline Declared declare(const Json& j, const std::vector<std::string>& actions) {
  Declared d;
  d.system.name = str(j, "name", "a system");
  for (const auto& c : j["components"].as_array()) d.components.push_back(c.as_string());
  const std::size_t n = d.components.size();
  if (n == 0 || n > 6) throw std::invalid_argument(d.system.name + " declares between one and six components");
  const std::size_t S = (std::size_t{1} << n) * 2;
  for (std::size_t bits = 0; bits < (std::size_t{1} << n); bits++)
    for (int o = 1; o >= 0; o--) d.system.states.push_back(state_name(d.components, bits, o == 1));
  d.system.actions = actions;
  const Json& laws = j["laws"];
  for (std::size_t s = 0; s < S; s++) {
    const std::size_t bits = s / 2;
    std::vector<kk::Distribution> row;
    for (const auto& act : actions) {
      double p = 0;
      const Json* rules = laws.find(act);
      bool matched = false;
      if (rules && rules->is_array())
        for (const auto& rule : rules->as_array()) {
          bool all = true;
          if (const Json* when = rule.find("when"); when && when->is_object())
            for (const auto& [name, want] : when->as_object()) {
              const auto it = std::find(d.components.begin(), d.components.end(), name);
              if (it == d.components.end()) throw std::invalid_argument(d.system.name + "'s law for " + act + " names no component " + name);
              all = all && component(bits, static_cast<std::size_t>(it - d.components.begin()), n) == want.as_bool();
            }
          if (all) {
            p = rule["p"].as_number();
            matched = true;
            break;
          }
        }
      if (!matched) p = 0;
      if (!(p >= 0 && p <= 1)) throw std::invalid_argument(d.system.name + "'s probability of success under " + act + " lies in [0, 1]");
      kk::Distribution next(S, 0.0);
      next[bits * 2] = p, next[bits * 2 + 1] = 1 - p;
      row.push_back(next);
    }
    d.system.dynamics.push_back(row);
  }
  return d;
}

}  // namespace detail

inline Json transfer(const Json& r) {
  std::vector<std::string> actions;
  for (const auto& a : r["actions"].as_array()) actions.push_back(a.as_string());
  if (actions.empty()) throw std::invalid_argument("a transfer names the actions the systems share, the practice's among them");
  const auto src = detail::declare(r["source"], actions), tgt = detail::declare(r["target"], actions);
  const std::size_t nX = tgt.system.states.size(), nJ = src.system.states.size(), cX = tgt.components.size(), cJ = src.components.size();
  // the lens: each target state read as the source state agreeing on every component the source declares; a source component the
  // target does not declare is read by the probability the request gives it (a declared reading), else refused
  std::map<std::string, double> unshared;
  if (const Json* u = r.find("unshared"); u && u->is_object())
    for (const auto& [k, v] : u->as_object()) unshared[k] = v.as_number();
  kt::Lens lens;
  for (std::size_t s = 0; s < nX; s++) {
    const std::size_t bits = s / 2;
    const bool success = s % 2 == 0;
    kk::Distribution row(nJ, 0.0);
    // enumerate the source's assignments with their weights
    for (std::size_t jb = 0; jb < (std::size_t{1} << cJ); jb++) {
      double w = 1;
      for (std::size_t c = 0; c < cJ && w > 0; c++) {
        const bool want = detail::component(jb, c, cJ);
        const auto it = std::find(tgt.components.begin(), tgt.components.end(), src.components[c]);
        if (it != tgt.components.end()) {
          w *= detail::component(bits, static_cast<std::size_t>(it - tgt.components.begin()), cX) == want ? 1.0 : 0.0;
        } else {
          const auto p = unshared.find(src.components[c]);
          if (p == unshared.end()) throw std::invalid_argument("the target does not declare " + src.components[c] + ", and no reading of it is given (unshared)");
          w *= want ? p->second : 1 - p->second;
        }
      }
      if (w > 0) row[jb * 2 + (success ? 0 : 1)] += w;
    }
    lens.interpret.push_back(row);
  }
  for (std::size_t a = 0; a < actions.size(); a++) lens.realise.push_back(a);
  // readings: each gives the probability of each target component (independent), the outcome taken as failure now
  std::vector<kt::Reading> readings;
  for (const auto& rd : r["readings"].as_array()) {
    kk::Distribution b(nX, 0.0);
    for (std::size_t bits = 0; bits < (std::size_t{1} << cX); bits++) {
      double w = 1;
      for (std::size_t c = 0; c < cX; c++) {
        const Json* p = rd["probabilities"].find(tgt.components[c]);
        if (!p || !p->is_number()) throw std::invalid_argument("the reading " + detail::str(rd, "name") + " gives no probability of " + tgt.components[c]);
        w *= detail::component(bits, c, cX) ? p->as_number() : 1 - p->as_number();
      }
      b[bits * 2 + 1] = w;
    }
    readings.push_back({detail::str(rd, "name"), detail::str(rd, "source"), lens, b});
  }
  std::vector<kt::Factor> factors;
  for (std::size_t c = 0; c < cX; c++) {
    std::vector<char> holds(nX, 0);
    for (std::size_t s = 0; s < nX; s++) holds[s] = detail::component(s / 2, c, cX);
    std::string source = "declared";
    if (const Json* fs = r.find("factor_sources"); fs && fs->find(tgt.components[c])) source = (*fs)[tgt.components[c]].as_string();
    factors.push_back({tgt.components[c], source, holds});
  }
  std::vector<char> considered(actions.size(), 0);
  const std::string practice = detail::str(r, "practice", actions.front());
  for (std::size_t a = 0; a < actions.size(); a++) considered[a] = actions[a] == practice;
  const double tolerance = r.find("tolerance") && r["tolerance"].is_number() ? r["tolerance"].as_number() : 0.1;
  const kt::Assessment as = kt::assess(src.system, tgt.system, readings, factors, considered, tolerance);
  if (as.verdict.fails()) throw std::invalid_argument(as.verdict.reason);
  Json squares = Json::array();
  for (const auto& q : as.squares) {
    Json worst = Json::array();
    for (std::size_t k = 0; k < q.pairs.size() && k < 6; k++)
      worst.push(Json::object({{"state", tgt.system.states[q.pairs[k].state]}, {"action", actions[q.pairs[k].action]}, {"distance", q.pairs[k].distance}}));
    Json where = Json::array();
    for (std::size_t s = 0; s < nX; s += 2)
      if (q.commutes[s]) {
        std::string w = tgt.system.states[s];
        where.push(w.substr(0, w.find(" | ")));
      }
    Json beyond = Json::array();
    for (std::size_t k = 0; k < q.beyond.size() && k < 6; k++) beyond.push(q.beyond[k]);
    squares.push(Json::object({{"reading", q.reading}, {"relations", detail::verdict(q.relations)}, {"worst", q.worst}, {"worst_pairs", worst}, {"commutes_where", where},
                               {"beyond", beyond}}));
  }
  Json factors_json = Json::array();
  for (const auto& f : as.factors)
    factors_json.push(Json::object({{"name", f.name}, {"source", f.source}, {"standing", kk::name(f.standing)}, {"low", f.measure.lower}, {"high", f.measure.upper},
                                    {"sufficient", f.sufficient}, {"necessary", f.necessary}, {"words", f.words}}));
  Json by = Json::array();
  for (const auto& w : as.by_reading) by.push(w);
  return Json::object({{"practice", practice}, {"tolerance", tolerance}, {"standing", kk::name(as.carries.standing)}, {"low", as.carries.measure.lower},
                       {"high", as.carries.measure.upper}, {"by_reading", by}, {"factors", factors_json}, {"squares", squares}, {"verdict", detail::verdict(as.verdict)}});
}

// ---- acts and turns ----------------------------------------------------------------------------------------------------------

namespace detail {

inline std::vector<std::string> payloads(const Json& outputs) {
  std::vector<std::string> out;
  if (!outputs.is_array()) throw std::invalid_argument("an act's outputs are a list, one value a port");
  for (const auto& o : outputs.as_array()) out.push_back(o.is_string() ? o.as_string() : o.dump());
  return out;
}

inline Json outcome(const kr::Outcome& o) {
  Json j = verdict(o.verdict);
  j.set("entry", static_cast<double>(o.entry));
  return j;
}

inline Json progress(const kr::Progress& p) {
  Json made = Json::array(), waiting = Json::array(), stale = Json::array();
  for (auto id : p.made) made.push(static_cast<double>(id));
  for (const auto& [b, why] : p.waiting) waiting.push(Json::object({{"block", b}, {"why", why}}));
  for (const auto& [id, why] : p.stale) stale.push(Json::object({{"entry", static_cast<double>(id)}, {"why", why}}));
  return Json::object({{"made", made}, {"waiting", waiting}, {"stale", stale}});
}

}  // namespace detail

inline Json act(const Json& r) {
  kr::Trajectory t = trajectory_from(r["trajectory"]);
  const std::string what = detail::str(r, "act"), by = detail::str(r, "by"), role = detail::str(r, "role"), when = detail::str(r, "when"), reason = detail::str(r, "reason");
  const std::string branch = detail::str(r, "branch", "main");
  const std::size_t turn = r.find("turn") ? static_cast<std::size_t>(r["turn"].as_number()) : 0;
  std::vector<std::string> files = detail::strings_of(r.find("files"));
  Json outcome;
  if (what == "observe" || what == "fill") {
    const auto outs = detail::payloads(r["outputs"]);
    const std::string block = detail::str(r, "block");
    outcome = detail::outcome(what == "observe" ? kr::observe(t, branch, turn, block, by, role, outs, reason, files, when) : kr::fill(t, branch, turn, block, by, role, outs, reason, files, when));
  } else if (what == "amend") {
    outcome = detail::outcome(kr::amend(t, static_cast<std::size_t>(r["entry"].as_number()), by, role, detail::payloads(r["outputs"]), reason, files, when, detail::str(r, "branch")));
  } else if (what == "review") {
    outcome = detail::outcome(kr::review(t, static_cast<std::size_t>(r["entry"].as_number()), by, role, r["accept"].as_bool(), reason, when, detail::str(r, "branch")));
  } else if (what == "choose") {
    const kr::Registry alts = alternatives();
    outcome = detail::verdict(kr::choose(t, branch, turn, detail::str(r, "block"), detail::str(r, "method"), by, role, reason, when, &alts));
  } else if (what == "attach") {
    const Json& f = r["file"];
    outcome = detail::verdict(kr::attach(t, {detail::str(f, "name"), detail::str(f, "digest"), detail::str(f, "kind"), static_cast<std::uint64_t>(f["bytes"].as_number())}, by, when));
  } else if (what == "initial") {
    const Json& v = r["value"];
    outcome = detail::verdict(kr::initial(t, detail::str(r, "wire"), v.is_string() ? v.as_string() : v.dump(), by, when));
  } else if (what == "branch") {
    outcome = detail::verdict(kr::branch(t, detail::str(r, "from", "main"), turn, detail::str(r, "name"), reason, by, when));
  } else if (what == "revise") {
    const auto reading = ka::read(detail::str(r, "script"));
    if (!reading.ok()) throw std::invalid_argument("the revision does not read: line " + std::to_string(reading.faults[0].line) + ": " + reading.faults[0].message);
    const auto v = ka::verify(reading.architecture);
    if (!v.admitted()) throw std::invalid_argument("the revision is not admitted: " + v.verdict.reason);
    outcome = detail::verdict(kr::revise(t, reading.architecture, by, reason, when));
    t.types = checks(t.arch);
  } else {
    throw std::invalid_argument("no act named " + what + " (observe, fill, amend, review, choose, attach, initial, branch, revise)");
  }
  return Json::object({{"trajectory", trajectory_json(t)}, {"outcome", outcome}});
}

inline Json answer(const Json& r) {
  const std::string op = detail::str(r, "op");
  if (op == "architecture") {
    const auto reading = ka::read(detail::str(r, "script"));
    Json faults = Json::array();
    for (const auto& f : reading.faults) faults.push(Json::object({{"line", static_cast<double>(f.line)}, {"message", f.message}}));
    if (!reading.ok()) return Json::object({{"admitted", false}, {"faults", faults}});
    const auto v = ka::verify(reading.architecture);
    Json cs = Json::array();
    for (const auto& c : v.checks) cs.push(Json::object({{"name", c.name}, {"kind", c.kind}, {"verdict", detail::verdict(c.verdict)}}));
    return Json::object({{"admitted", v.admitted()}, {"reason", v.verdict.reason}, {"checks", cs}, {"printed", ka::print(reading.architecture)}, {"faults", faults}});
  }
  if (op == "methods") return Json::object({{"methods", methods_json()}});
  if (op == "open") {
    const auto reading = ka::read(detail::str(r, "script"));
    if (!reading.ok()) throw std::invalid_argument("the architecture does not read: line " + std::to_string(reading.faults[0].line) + ": " + reading.faults[0].message);
    const auto v = ka::verify(reading.architecture);
    if (!v.admitted()) throw std::invalid_argument("the architecture is not admitted: " + v.verdict.reason);
    const kr::Trajectory t = kr::open(reading.architecture, rules_from(r["rules"]), checks(reading.architecture), {}, detail::str(r, "by", "someone"), detail::str(r, "when"));
    return Json::object({{"trajectory", trajectory_json(t)}});
  }
  if (op == "act") return act(r);
  if (op == "turn") {
    kr::Trajectory t = trajectory_from(r["trajectory"]);
    const std::string branch = detail::str(r, "branch", "main"), when = detail::str(r, "when");
    const kr::Registry ms = alternatives();
    Json carried = Json::array();
    if (r.find("carry") && r["carry"].is_bool() && r["carry"].as_bool() && r.find("turn") && r["turn"].is_number())
      for (const auto& o : kr::carry(t, branch, static_cast<std::size_t>(r["turn"].as_number()), detail::str(r, "by", "someone"), detail::str(r, "role"), when))
        carried.push(detail::outcome(o));
    const kr::Progress p = r.find("from") && r["from"].is_number() ? kr::propagate(t, branch, static_cast<std::size_t>(r["from"].as_number()), ms, when)
                                                                    : kr::run_turn(t, branch, static_cast<std::size_t>(r["turn"].as_number()), ms, when);
    Json progress = detail::progress(p);
    progress.set("carried", carried);
    return Json::object({{"trajectory", trajectory_json(t)}, {"progress", progress}});
  }
  if (op == "view") return view(trajectory_from(r["trajectory"]), detail::str(r, "branch"));
  if (op == "verify") {
    // verification without refusal: the answer says what fails
    try {
      return detail::verdict(kr::verify(trajectory_from(r["trajectory"])));
    } catch (const std::exception& e) {
      return Json::object({{"holds", false}, {"undecided", false}, {"claim", "the record verifies"}, {"reason", e.what()}});
    }
  }
  if (op == "replay") {
    Json out = Json::array();
    for (const auto& rp : kr::replay(trajectory_from(r["trajectory"]), alternatives())) out.push(Json::object({{"entry", static_cast<double>(rp.entry)}, {"verdict", detail::verdict(rp.verdict)}}));
    return Json::object({{"replayed", out}});
  }
  if (op == "diff") {
    const kr::Trajectory t = trajectory_from(r["trajectory"]);
    Json out = Json::array();
    for (const auto& c : kr::diff(t, detail::str(r["a"], "branch", "main"), static_cast<std::size_t>(r["a"]["turn"].as_number()), detail::str(r["b"], "branch", "main"),
                                  static_cast<std::size_t>(r["b"]["turn"].as_number())))
      out.push(Json::object({{"block", c.block}, {"port", c.port}, {"before", c.before ? detail::parse_or(kr::payload(t, *c.before), Json()) : Json()},
                             {"after", c.after ? detail::parse_or(kr::payload(t, *c.after), Json()) : Json()}, {"before_entry", static_cast<double>(c.before_entry)},
                             {"after_entry", static_cast<double>(c.after_entry)}}));
    return Json::object({{"changes", out}});
  }
  if (op == "resting") {
    const kr::Trajectory t = trajectory_from(r["trajectory"]);
    std::set<std::size_t> s;
    if (r.find("entry")) s = kr::resting_on(t, {static_cast<std::size_t>(r["entry"].as_number())});
    else if (r.find("file")) s = kr::resting_on_file(t, detail::str(r, "file"));
    else s = kr::resting_on_agent(t, detail::str(r, "agent"));
    Json out = Json::array();
    for (auto id : s) out.push(Json::object({{"entry", static_cast<double>(id)}, {"block", t.entries[id].block}, {"turn", static_cast<double>(t.entries[id].turn)},
                                             {"branch", t.entries[id].branch}, {"act", kr::name(t.entries[id].act)}, {"by", t.entries[id].by}}));
    return Json::object({{"resting", out}});
  }
  if (op == "prov") return prov(trajectory_from(r["trajectory"]));
  if (op == "transfer") return transfer(r);
  throw std::invalid_argument("no operation named " + op);
}

}  // namespace science::steering
