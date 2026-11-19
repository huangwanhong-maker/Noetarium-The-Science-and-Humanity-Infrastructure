// An undertaking among many parties, as a typed dependency structure (ADR 0132).
//
// The user's monograph (Huang 2026, *Future Talent Strategy: The Possibility of the Human–AI Multilateral Phronimos*, §10.4.2) writes
// an undertaking as R = (V, {E_k}, D, Γ):
//
//   V     participants and systems: states, organisations, cities, firms, communities, persons, AI systems, datasets, review forums;
//   E_k   directed relations of declared types: evidential reliance, communication, access, authorisation, review, challenge,
//         membership, contribution;
//   D     decision points, each with its authorities, its evidence, those it affects and the venues where it may be contested;
//   Γ     constraints, each held by someone.
//
// Relations change: a tie holds from a date until another. And "missing observations must not be interpreted automatically as
// absent relationships": a tie is present when recorded, absent when recorded absent or when a declared design of observation covered
// both ends and found none, and otherwise *not observed*. A complete communication graph may stand beside an ambiguous authorisation
// graph, and the domain says which.
//
// A participant's relational position (the monograph's Definition 9.5) is read at a date: its ties of each type in and out, what it is
// authorised by, what it relies on, the decisions it holds, is affected by, or can contest. And the undertaking diagnoses itself (the
// monograph's Table 10.2), each finding naming where it fails:
//
//   evidence        a decision that names no evidence;
//   authorisation   a decision made by a participant neither constituted nor authorised (a recommendation recorded as a decision);
//   participation   a decision with no venue of challenge, or affecting a participant with no route to one;
//   AI dependency   a decision resting on AI systems alone, or counting one source twice (repeated outputs are not independent
//                   corroboration);
//   succession      a participant without whom the relations of authority fall apart (a cut vertex, morphe's blocks), a tie that is
//                   the only link (a bridge);
//   concentration   a participant on whom most evidential reliance falls;
//   unobserved      a type of relation of which no absence is known, every pair outside its recorded ties not observed.
//
// Dates are ISO 8601 calendar dates (YYYY-MM-DD), compared as text; an empty bound is open. Nothing here names a real body: what an
// undertaking holds is the caller's.
#pragma once

#include <morphe/combinatorics/blocks.hpp>
#include <morphe/combinatorics/graph.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::relations {

enum class Status { present, absent, not_observed };

inline const char* name(Status s) {
  switch (s) {
    case Status::present: return "present";
    case Status::absent: return "absent";
    default: return "not observed";
  }
}

struct RelationType {
  std::string name, meaning;
  bool authority = false;   // it carries authority: authorisation, delegation, review
  bool evidential = false;  // one relies on the other's claims
  bool challenge = false;   // one may contest the other's decisions
};

struct Participant {
  std::string id, name, kind;  // kind: a state, an organisation, a city, a firm, a community, a person, an AI system, a dataset, a forum…
  bool constituted = false;    // holds authority by its constitution, not by another's authorisation
};

struct Tie {
  std::string type, from, to;
  std::string since, until;  // the tie holds on dates d with since <= d < until; empty bounds are open
  bool absent = false;       // recorded absent: observed, and not there
  std::string source;
};

// A declared design of observation: for ties of a type, among the participants it covers, over a period. Within it, a pair with no
// tie recorded is absent; outside it, not observed.
struct Observation {
  std::string type, design, since, until;
  std::vector<std::string> covered;
};

struct Decision {
  std::string id, label, when, rule;
  std::string closure = "provisional";  // provisional (closure without epistemic finality) or final
  std::vector<std::string> authorities, evidence, affected, venues;
  std::string reason;
};

struct Constraint {
  std::string id, text, held_by;
};

struct Undertaking {
  std::string name;
  std::vector<RelationType> types;
  std::vector<Participant> participants;
  std::vector<Tie> ties;
  std::vector<Observation> observations;
  std::vector<Decision> decisions;
  std::vector<Constraint> constraints;

  const Participant* participant(const std::string& id) const {
    for (const auto& p : participants)
      if (p.id == id) return &p;
    return nullptr;
  }
  const RelationType* type(const std::string& n) const {
    for (const auto& t : types)
      if (t.name == n) return &t;
    return nullptr;
  }
};

struct Problem {
  std::string where, what;
};

inline bool holds_on(const std::string& since, const std::string& until, const std::string& at) {
  if (at.empty()) return true;
  return (since.empty() || since <= at) && (until.empty() || at < until);
}

// Every way an undertaking's declaration fails, each named.
inline std::vector<Problem> problems(const Undertaking& u) {
  std::vector<Problem> out;
  std::set<std::string> ids, types;
  for (const auto& t : u.types)
    if (!types.insert(t.name).second) out.push_back({"type " + t.name, "declared twice"});
  for (const auto& p : u.participants) {
    if (p.id.empty()) out.push_back({"a participant", "it has no id"});
    else if (!ids.insert(p.id).second) out.push_back({"participant " + p.id, "declared twice"});
  }
  auto known = [&](const std::string& id) { return ids.count(id) > 0; };
  for (const auto& t : u.ties) {
    const std::string where = "tie " + t.from + " " + t.type + " " + t.to;
    if (!types.count(t.type)) out.push_back({where, t.type + " is no declared type"});
    if (!known(t.from) || !known(t.to)) out.push_back({where, "it joins a participant not declared"});
    if (t.from == t.to) out.push_back({where, "a participant tied to itself"});
    if (!t.since.empty() && !t.until.empty() && !(t.since < t.until)) out.push_back({where, "it ends before it begins"});
  }
  for (const auto& o : u.observations) {
    if (!types.count(o.type)) out.push_back({"observation " + o.design, o.type + " is no declared type"});
    for (const auto& c : o.covered)
      if (!known(c)) out.push_back({"observation " + o.design, c + " is not declared"});
  }
  for (const auto& d : u.decisions)
    for (const auto* list : {&d.authorities, &d.evidence, &d.affected, &d.venues})
      for (const auto& x : *list)
        if (!known(x)) out.push_back({"decision " + d.id, x + " is not declared"});
  for (const auto& c : u.constraints)
    if (!c.held_by.empty() && !known(c.held_by)) out.push_back({"constraint " + c.id, c.held_by + " is not declared"});
  return out;
}

// Whether a tie of a type runs from a to b on a date: present when recorded and not recorded absent; absent when recorded absent or
// when a declared observation covering both found none; otherwise not observed.
inline Status status(const Undertaking& u, const std::string& type, const std::string& a, const std::string& b, const std::string& at = {}) {
  bool recorded_absent = false;
  for (const auto& t : u.ties)
    if (t.type == type && t.from == a && t.to == b && holds_on(t.since, t.until, at)) {
      if (!t.absent) return Status::present;
      recorded_absent = true;
    }
  if (recorded_absent) return Status::absent;
  for (const auto& o : u.observations)
    if (o.type == type && holds_on(o.since, o.until, at) && std::find(o.covered.begin(), o.covered.end(), a) != o.covered.end() &&
        std::find(o.covered.begin(), o.covered.end(), b) != o.covered.end())
      return Status::absent;
  return Status::not_observed;
}

struct Layer {
  std::string type;
  std::size_t present = 0, absent = 0, not_observed = 0;  // ordered pairs of distinct participants
  double observed() const {
    const std::size_t all = present + absent + not_observed;
    return all ? static_cast<double>(present + absent) / static_cast<double>(all) : 0.0;
  }
};

// The pairs of a type of relation on a date, by status.
inline Layer layer(const Undertaking& u, const std::string& type, const std::string& at = {}) {
  Layer l;
  l.type = type;
  for (const auto& a : u.participants)
    for (const auto& b : u.participants) {
      if (a.id == b.id) continue;
      switch (status(u, type, a.id, b.id, at)) {
        case Status::present: l.present++; break;
        case Status::absent: l.absent++; break;
        default: l.not_observed++;
      }
    }
  return l;
}

// A participant's relational position on a date (the monograph's Definition 9.5, the part a record can show).
struct Position {
  std::string participant;
  std::map<std::string, std::vector<std::string>> out, in;  // by type: whom it is tied to, who is tied to it
  std::vector<std::string> authorised_by, relies_on, relied_on_by, may_challenge;
  std::vector<std::string> decides, affected_by, venue_of;
  std::vector<std::string> constraints_held;
  bool constituted = false;
  bool formal_authority() const { return constituted || !authorised_by.empty(); }
};

inline Position position(const Undertaking& u, const std::string& id, const std::string& at = {}) {
  Position p;
  p.participant = id;
  if (const Participant* x = u.participant(id)) p.constituted = x->constituted;
  for (const auto& t : u.ties) {
    if (t.absent || !holds_on(t.since, t.until, at)) continue;
    const RelationType* ty = u.type(t.type);
    if (t.from == id) {
      p.out[t.type].push_back(t.to);
      if (ty && ty->evidential) p.relies_on.push_back(t.to);
      if (ty && ty->challenge) p.may_challenge.push_back(t.to);
    }
    if (t.to == id) {
      p.in[t.type].push_back(t.from);
      if (ty && ty->authority) p.authorised_by.push_back(t.from);
      if (ty && ty->evidential) p.relied_on_by.push_back(t.from);
    }
  }
  for (const auto& d : u.decisions) {
    if (!d.when.empty() && !at.empty() && d.when > at) continue;
    auto has = [&](const std::vector<std::string>& v) { return std::find(v.begin(), v.end(), id) != v.end(); };
    if (has(d.authorities)) p.decides.push_back(d.id);
    if (has(d.affected)) p.affected_by.push_back(d.id);
    if (has(d.venues)) p.venue_of.push_back(d.id);
  }
  for (const auto& c : u.constraints)
    if (c.held_by == id) p.constraints_held.push_back(c.id);
  return p;
}

// ---- The undertaking's own diagnostics ------------------------------------------------------------------------------------------

struct Diagnosis {
  std::string kind;   // evidence, authorisation, participation, AI dependency, succession, concentration, unobserved
  std::string where;  // the decision, participant, tie or type
  std::string what;
};

struct Thresholds {
  double concentration = 0.5;      // a participant receiving at least this share of evidential reliance
  std::string ai_kind = "AI system";
};

inline std::vector<Diagnosis> diagnose(const Undertaking& u, const std::string& at = {}, const Thresholds& th = {}) {
  std::vector<Diagnosis> out;
  auto named = [&](const std::string& id) {
    const Participant* p = u.participant(id);
    return p && !p->name.empty() ? p->name : id;
  };
  for (const auto& d : u.decisions) {
    if (!d.when.empty() && !at.empty() && d.when > at) continue;
    const std::string where = "decision " + d.id + (d.label.empty() ? "" : " (" + d.label + ")");
    if (d.evidence.empty()) out.push_back({"evidence", where, "it names no evidence"});
    for (const auto& a : d.authorities) {
      const Position p = position(u, a, d.when.empty() ? at : d.when);
      if (!p.formal_authority())
        out.push_back({"authorisation", where, named(a) + " decided it, holding no authority recorded: neither constituted nor authorised by anyone"});
    }
    if (d.authorities.empty()) out.push_back({"authorisation", where, "no one is recorded as deciding it"});
    if (d.venues.empty()) out.push_back({"participation", where, "it names no venue where it may be contested"});
    else
      for (const auto& a : d.affected) {
        bool route = std::find(d.venues.begin(), d.venues.end(), a) != d.venues.end();
        const Position p = position(u, a, d.when.empty() ? at : d.when);
        for (const auto& v : d.venues) route = route || std::find(p.may_challenge.begin(), p.may_challenge.end(), v) != p.may_challenge.end();
        if (!route) out.push_back({"participation", where, named(a) + " is affected and has no route of challenge to any of its venues"});
      }
    if (!d.evidence.empty()) {
      bool all_ai = true;
      std::set<std::string> seen;
      for (const auto& e : d.evidence) {
        const Participant* p = u.participant(e);
        all_ai = all_ai && p && p->kind == th.ai_kind;
        if (!seen.insert(e).second) out.push_back({"AI dependency", where, named(e) + " is counted more than once: repeated outputs of one source are not independent corroboration"});
      }
      if (all_ai) out.push_back({"AI dependency", where, "it rests on AI systems alone"});
    }
  }
  // Succession: the participants and ties that hold the relations of authority together.
  {
    std::map<std::string, std::size_t> index;
    std::vector<std::string> ids;
    for (const auto& p : u.participants) index[p.id] = ids.size(), ids.push_back(p.id);
    morphe::combinatorics::Multigraph G{ids.size(), {}};
    std::vector<const Tie*> carried;
    for (const auto& t : u.ties) {
      const RelationType* ty = u.type(t.type);
      if (t.absent || !ty || !ty->authority || !holds_on(t.since, t.until, at) || !index.count(t.from) || !index.count(t.to)) continue;
      G.edges.push_back({index[t.from], index[t.to]});
      carried.push_back(&t);
    }
    if (!G.edges.empty()) {
      const auto b = morphe::combinatorics::blocks(G);
      for (std::size_t v = 0; v < ids.size(); v++)
        if (b.cut[v]) out.push_back({"succession", "participant " + ids[v], "without " + named(ids[v]) + " the relations of authority fall apart: the undertaking cannot continue without it"});
      for (std::size_t e = 0; e < carried.size(); e++)
        if (b.bridge[e])
          out.push_back({"succession", "tie " + carried[e]->from + " " + carried[e]->type + " " + carried[e]->to,
                         "the only link of authority between " + named(carried[e]->from) + "'s side and " + named(carried[e]->to) + "'s"});
    }
  }
  // Concentration of evidential reliance.
  {
    std::map<std::string, std::size_t> on;
    std::size_t all = 0;
    for (const auto& t : u.ties) {
      const RelationType* ty = u.type(t.type);
      if (t.absent || !ty || !ty->evidential || !holds_on(t.since, t.until, at)) continue;
      on[t.to]++, all++;
    }
    for (const auto& [id, k] : on)
      if (all >= 2 && static_cast<double>(k) >= th.concentration * static_cast<double>(all))
        out.push_back({"concentration", "participant " + id, named(id) + " receives " + std::to_string(k) + " of " + std::to_string(all) + " ties of evidential reliance"});
  }
  // Types of which no absence is known.
  for (const auto& t : u.types) {
    bool any = false;
    for (const auto& o : u.observations) any = any || (o.type == t.name && holds_on(o.since, o.until, at));
    for (const auto& x : u.ties) any = any || (x.type == t.name && x.absent);
    if (!any) out.push_back({"unobserved", "type " + t.name, "no design of observation covers it: every pair outside its recorded ties is not observed, not absent"});
  }
  return out;
}

}  // namespace diplomacy::relations
