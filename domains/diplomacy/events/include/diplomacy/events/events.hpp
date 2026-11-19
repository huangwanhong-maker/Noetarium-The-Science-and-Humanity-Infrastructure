// Events among parties as the reports they rest on, each origin counted once (ADR 0134).
//
// Machine-coded event data count reports: one statement carried by fifty outlets becomes fifty events unless what they share is
// found (the survey's §A.6.2; Hammond and Weidmann 2014). An event here keeps the reports it rests on, each with its source, the
// origin it relays (a wire, an agency's statement, a ministry's release; itself when it relays none), its date and its passage.
// What follows:
//
//   support     the reports, sources and origins an event rests on, the reports that deny it, and, when channels are declared
//               for sources and origins, the force of the reports for the event's having occurred: the reports that relay one
//               origin form one group, never more forceful than the origin however many they are, and groups of independent
//               origins corroborate (tekmerion's dependence, its decision 0007). What counting every report as independent would
//               give is said beside it;
//   coincident  records that may describe one event (the same category, a shared actor and target, dates within a declared
//               window), each pair with what they share. They are reported and never merged: two records are manifestations,
//               and that they coincide does not make them one (the user's Generative Relational framework, as in archeion);
//   series      a pair of parties' events by period and by class, each event counted once, beside the count the reports would give;
//   signs       an event no report attests; one attested by a single origin however many outlets carry it; one whose reports
//               disagree; one reported only by a party to it; a report with no passage.
//
// The vocabulary is declared. PLOVER's sixteen categories (Halterman, Schrodt, Beger, Bagozzi and Scarborough 2023; the PLOVER
// specification, CC BY-SA 4.0) are the default, classed here into cooperation and conflict, verbal and material, after the
// CAMEO quad classes they descend from (Gerner, Schrodt, Yilmaz and Abu-Jabr 2002). Caveat: that classing is this module's
// reading, declared and replaceable; PLOVER itself records a mode and a context in place of CAMEO's fine codes.
//
// Dates are ISO 8601 calendar dates. Nothing here names a real event; what a record holds is the caller's.
#pragma once

#include <archeion/retention/dates.hpp>
#include <tekmerion/evidence/dependence.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::events {

namespace te = tekmerion::evidence;

struct Category {
  std::string name, meaning;
  bool cooperation = true;  // cooperation or conflict
  bool material = false;    // material or verbal
};

struct Vocabulary {
  std::string name, source;
  std::vector<Category> categories;
  const Category* category(const std::string& n) const {
    for (const auto& c : categories)
      if (c.name == n) return &c;
    return nullptr;
  }
};

// PLOVER's sixteen categories, classed after CAMEO's quad classes (a declared reading).
inline Vocabulary plover() {
  Vocabulary v;
  v.name = "PLOVER";
  v.source = "Halterman et al. (2023); the PLOVER specification (CC BY-SA 4.0); classes after CAMEO's quad classes, this module's reading";
  auto c = [&](const char* n, bool coop, bool material, const char* meaning) { v.categories.push_back({n, meaning, coop, material}); };
  c("AGREE", true, false, "agree to, or sign, an agreement or a statement of intent");
  c("CONSULT", true, false, "meet, visit, negotiate or mediate");
  c("SUPPORT", true, false, "praise, endorse or defend another");
  c("CONCEDE", true, true, "yield: ease a sanction, release, accept a demand");
  c("RETREAT", true, true, "withdraw, demobilise, surrender");
  c("COOPERATE", true, true, "cooperate in practice: share information, act jointly");
  c("AID", true, true, "give aid: economic, military, humanitarian");
  c("DEMAND", false, false, "demand or request with force");
  c("DISAPPROVE", false, false, "criticise, accuse, complain");
  c("REJECT", false, false, "reject a proposal, refuse to meet");
  c("THREATEN", false, false, "threaten a sanction or force");
  c("PROTEST", false, true, "demonstrate, strike, boycott");
  c("MOBILIZE", false, true, "mobilise forces or a posture of force");
  c("SANCTION", false, true, "sanction, reduce relations, expel");
  c("COERCE", false, true, "coerce: seize, arrest, repress");
  c("ASSAULT", false, true, "attack with force");
  return v;
}

inline std::string class_of(const Category& c) {
  return std::string(c.material ? "material " : "verbal ") + (c.cooperation ? "cooperation" : "conflict");
}

struct Report {
  std::string id, source, origin;  // the outlet or archive; the origin it relays (empty: itself)
  std::string published, passage;
  bool says = true;                // whether it says the event occurred (false: it denies it)
};

struct Event {
  std::string id, category, mode, context;
  std::vector<std::string> actors, targets;
  std::string date, until;  // when it is reported to have occurred: a date, or a span
  std::string place;
  std::vector<Report> reports;
  std::string origin_of(const Report& r) const { return r.origin.empty() ? "itself: " + r.id : r.origin; }
};

struct Problem {
  std::string where, what;
};

inline std::vector<Problem> problems(const std::vector<Event>& events, const Vocabulary& v = plover(), const std::vector<std::string>& parties = {}) {
  std::vector<Problem> out;
  std::set<std::string> ids;
  const std::set<std::string> known(parties.begin(), parties.end());
  for (const auto& e : events) {
    const std::string where = "event " + e.id;
    if (!ids.insert(e.id).second) out.push_back({where, "declared twice"});
    if (!v.category(e.category)) out.push_back({where, e.category + " is no category of " + v.name});
    if (e.actors.empty()) out.push_back({where, "it names no actor"});
    for (const auto* list : {&e.actors, &e.targets})
      for (const auto& p : *list)
        if (!known.empty() && !known.count(p)) out.push_back({where, p + " is not a party of the undertaking"});
    if (!archeion::retention::is_date(e.date)) out.push_back({where, "its date is not a calendar date"});
    if (!e.until.empty() && (!archeion::retention::is_date(e.until) || e.until < e.date)) out.push_back({where, "its span ends before it begins, or not on a date"});
    std::set<std::string> rids;
    for (const auto& r : e.reports) {
      if (!rids.insert(r.id).second) out.push_back({where, "the report " + r.id + " is declared twice"});
      if (r.source.empty()) out.push_back({where, "the report " + r.id + " names no source"});
      if (!r.published.empty() && !archeion::retention::is_date(r.published)) out.push_back({where, "the report " + r.id + " is not dated by a calendar date"});
    }
  }
  return out;
}

// ---- Support ------------------------------------------------------------------------------------------------------------------

// Declared channels: how a source relays its origin, and how an origin registers an event (P(report | event), P(report | none)).
struct Channels {
  std::map<std::string, te::Channel> sources, origins;
  te::Channel relay{0.95, 0.05};  // a source not declared
  te::Channel origin{0.9, 0.1};   // an origin named and not declared (a report that relays none is its own origin, and perfect, so
                                  // that it is a report through its source's channel alone)
};

struct Support {
  std::size_t reports = 0, sources = 0, origins = 0, denials = 0;
  std::vector<std::string> origin_names;
  bool weighed = false;
  double ratio = 1;          // P(reports | it occurred) / P(reports | it did not), origins counted once
  double as_independent = 1; // the same, every report counted as independent
  double overstatement = 1;  // as_independent / ratio
};

inline Support support(const Event& e, const Channels* ch = nullptr) {
  Support s;
  s.reports = e.reports.size();
  std::set<std::string> sources;
  std::map<std::string, std::vector<const Report*>> by_origin;
  for (const auto& r : e.reports) {
    sources.insert(r.source);
    by_origin[e.origin_of(r)].push_back(&r);
    s.denials += !r.says;
  }
  s.sources = sources.size(), s.origins = by_origin.size();
  for (const auto& [o, _] : by_origin) s.origin_names.push_back(o);
  if (ch && !e.reports.empty()) {
    std::vector<te::Group> groups;
    for (const auto& [o, rs] : by_origin) {
      te::Group g;
      auto oc = ch->origins.find(o);
      if (oc != ch->origins.end()) g.origin = oc->second;
      else if (o.rfind("itself: ", 0) == 0) g.origin = {1, 0};
      else g.origin = ch->origin;
      for (const Report* r : rs) {
        auto sc = ch->sources.find(r->source);
        g.relays.push_back({sc == ch->sources.end() ? ch->relay : sc->second, r->says});
      }
      groups.push_back(std::move(g));
    }
    s.weighed = true;
    s.ratio = te::dependent_ratio(1, 0, groups);
    s.as_independent = te::as_if_independent(1, 0, groups);
    s.overstatement = te::overstatement(1, 0, groups);
  }
  return s;
}

// ---- Coincident records -------------------------------------------------------------------------------------------------------

struct Criterion {
  long window = 3;           // days between their dates at most
  bool same_category = true;
  bool shared_target = true; // a target in common as well as an actor (when both name targets)
};

struct Coincidence {
  std::string a, b, what;
};

inline std::vector<Coincidence> coincident(const std::vector<Event>& events, const Criterion& c = {}) {
  std::vector<Coincidence> out;
  auto shared = [](const std::vector<std::string>& x, const std::vector<std::string>& y) {
    std::vector<std::string> s;
    for (const auto& a : x)
      if (std::find(y.begin(), y.end(), a) != y.end()) s.push_back(a);
    return s;
  };
  for (std::size_t i = 0; i < events.size(); i++)
    for (std::size_t j = i + 1; j < events.size(); j++) {
      const Event &x = events[i], &y = events[j];
      if (c.same_category && x.category != y.category) continue;
      const long days = std::labs(archeion::retention::days_between(x.date, y.date));
      if (days > c.window) continue;
      const auto actors = shared(x.actors, y.actors);
      if (actors.empty()) continue;
      const auto targets = shared(x.targets, y.targets);
      if (c.shared_target && !x.targets.empty() && !y.targets.empty() && targets.empty()) continue;
      std::string what = x.category + ", " + std::to_string(days) + " days apart, sharing " + actors[0];
      if (!targets.empty()) what += " towards " + targets[0];
      out.push_back({x.id, y.id, what + ": possibly one event, kept as two records"});
    }
  return out;
}

// ---- A pair's series -------------------------------------------------------------------------------------------------------

struct Period {
  std::string period;
  std::map<std::string, std::size_t> events;   // by class, each event once
  std::map<std::string, std::size_t> reports;  // by class, as the reports would count them
  long net = 0;                                // cooperation less conflict, in events
};

// The events between two parties (either acting towards the other), by month ("YYYY-MM") or year ("YYYY").
inline std::vector<Period> series(const std::vector<Event>& events, const std::string& a, const std::string& b, bool by_year = false, const Vocabulary& v = plover()) {
  std::map<std::string, Period> m;
  auto in = [](const std::vector<std::string>& l, const std::string& x) { return std::find(l.begin(), l.end(), x) != l.end(); };
  for (const auto& e : events) {
    const bool between = (in(e.actors, a) && in(e.targets, b)) || (in(e.actors, b) && in(e.targets, a));
    if (!between) continue;
    const Category* c = v.category(e.category);
    if (!c) continue;
    const std::string key = e.date.substr(0, by_year ? 4 : 7);
    Period& p = m[key];
    p.period = key;
    p.events[class_of(*c)]++;
    p.reports[class_of(*c)] += e.reports.size();
    p.net += c->cooperation ? 1 : -1;
  }
  std::vector<Period> out;
  for (auto& [_, p] : m) out.push_back(std::move(p));
  return out;
}

// ---- Signs -------------------------------------------------------------------------------------------------------------------

struct Sign {
  std::string kind, where, what;
};

inline std::vector<Sign> signs(const std::vector<Event>& events) {
  std::vector<Sign> out;
  for (const auto& e : events) {
    const std::string where = "event " + e.id;
    if (e.reports.empty()) {
      out.push_back({"unattested", where, "no report attests it"});
      continue;
    }
    const Support s = support(e);
    if (s.origins == 1 && s.reports > 1)
      out.push_back({"one origin", where, std::to_string(s.reports) + " reports from " + std::to_string(s.sources) + " sources relay one origin, " + s.origin_names[0] + ": it is attested once"});
    if (s.denials > 0 && s.denials < s.reports) out.push_back({"disputed", where, std::to_string(s.denials) + " of its " + std::to_string(s.reports) + " reports deny it"});
    else if (s.denials == s.reports) out.push_back({"denied", where, "every report denies it"});
    bool own = true;
    for (const auto& r : e.reports) {
      const bool party = std::find(e.actors.begin(), e.actors.end(), r.source) != e.actors.end() ||
                         std::find(e.targets.begin(), e.targets.end(), r.source) != e.targets.end();
      own = own && party;
    }
    if (own) out.push_back({"self-reported", where, "it is reported only by a party to it"});
    for (const auto& r : e.reports)
      if (r.passage.empty()) out.push_back({"no passage", where, "the report " + r.id + " gives no passage"});
  }
  return out;
}

}  // namespace diplomacy::events
