// Mandates and their lifecycles (ADR 0134).
//
// A mandate is a task a body gives: a resolution's operative paragraph asks an entity to do something, for a period, perhaps with
// resources and a clause of review. Mandates accumulate: the UN Secretary-General's Mandate Implementation Review (A/80/318, 2025)
// counted more than 40,000 given to UN entities, found their delivery fragmented and a widening gap between mandates and resources,
// and left each mandate's merit to the member states (the survey's §A.3.3). Principal–agent theory reads a secretariat as an agent
// whose slack its principals limit by screening, monitoring and sanction (Hawkins, Lake, Nielson and Tierney 2006). A record of
// mandates makes the review's questions queries:
//
//   lifecycle   a mandate's state at a date: not yet in force, in force, expired, renewed (continued by another), superseded,
//               consolidated into another, or terminated; its relations to earlier mandates (it renews, amends, supersedes,
//               consolidates or terminates them) form a directed relation that must be acyclic (morphe's acyclic relations), and a
//               mandate's lineage is its cone in it;
//   review      when a mandate's review falls due (every so many days, or by a date), the reviews recorded, and whether a review is
//               overdue or has never been made;
//   overlap     mandates in force at once on a common addressee and a common declared theme: possible duplication, for the members
//               to judge;
//   resources   mandates in force without resources stated, and the gap, addressee by addressee, between the mandates it carries and
//               those resourced;
//   creep       what a lineage added: the themes and addressees of its latest member that its first did not have;
//   signs       a review overdue, a mandate never reviewed, one open-ended with no clause of review, one unresourced, overlaps,
//               creep; and, read against an undertaking (diplomacy::relations), a mandate given by a participant that held no
//               authority recorded when it gave it, or addressed to one that is not a participant.
//
// Dates are ISO 8601 calendar dates, counted by archeion's day numbers. Nothing here is any body's mandate until a caller states it.
//
// References. United Nations (2025), Mandate Implementation Review: Report of the Secretary-General, A/80/318. Hawkins, D. G.,
// Lake, D. A., Nielson, D. L. and Tierney, M. J., eds. (2006), Delegation and Agency in International Organizations, Cambridge
// University Press.
#pragma once

#include <archeion/retention/dates.hpp>
#include <diplomacy/relations/relations.hpp>
#include <morphe/combinatorics/acyclic.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::mandates {

namespace ar = archeion::retention;

enum class Link { renews, amends, supersedes, consolidates, terminates };

inline const char* name(Link l) {
  switch (l) {
    case Link::renews: return "renews";
    case Link::amends: return "amends";
    case Link::supersedes: return "supersedes";
    case Link::consolidates: return "consolidates";
    default: return "terminates";
  }
}

inline Link link_of(const std::string& s) {
  for (Link l : {Link::renews, Link::amends, Link::supersedes, Link::consolidates, Link::terminates})
    if (s == name(l)) return l;
  throw std::invalid_argument("diplomacy::mandates: no link is called " + s + " (renews, amends, supersedes, consolidates, terminates)");
}

struct Predecessor {
  Link link = Link::renews;
  std::string mandate;
};

struct Mandate {
  std::string id, title;
  std::string instrument, paragraph;  // where it is given: a resolution, and its operative paragraph
  std::string issued_by, issued;      // the body that gave it, and when
  std::vector<std::string> addressees, themes;
  std::string begins, ends;           // in force on dates d with begins <= d < ends; an empty end is open
  long review_every = 0;              // days between reviews (0: no periodic review)
  std::string review_by;              // a review by a date (empty: none)
  bool resourced = false;             // resources stated and provided
  std::string resources;              // what is said of them ("within existing resources", an amount)
  std::vector<Predecessor> earlier;   // the mandates it renews, amends, supersedes, consolidates or terminates
};

struct Review {
  std::string mandate, date, by, finding;  // finding: continued, amended, to be ended, …
};

struct Registry {
  std::vector<Mandate> mandates;
  std::vector<Review> reviews;
  const Mandate* mandate(const std::string& id) const {
    for (const auto& m : mandates)
      if (m.id == id) return &m;
    return nullptr;
  }
};

struct Problem {
  std::string where, what;
};

namespace detail {

inline std::map<std::string, std::size_t> index(const Registry& r) {
  std::map<std::string, std::size_t> ix;
  for (std::size_t k = 0; k < r.mandates.size(); k++) ix[r.mandates[k].id] = k;
  return ix;
}

// Arcs earlier → later.
inline std::vector<morphe::combinatorics::Arc> arcs(const Registry& r) {
  const auto ix = index(r);
  std::vector<morphe::combinatorics::Arc> a;
  for (std::size_t k = 0; k < r.mandates.size(); k++)
    for (const auto& p : r.mandates[k].earlier)
      if (auto it = ix.find(p.mandate); it != ix.end()) a.push_back({it->second, k});
  return a;
}

inline bool date_or_empty(const std::string& d) { return d.empty() || ar::is_date(d); }

}  // namespace detail

inline std::vector<Problem> problems(const Registry& r) {
  std::vector<Problem> out;
  std::set<std::string> ids;
  for (const auto& m : r.mandates) {
    const std::string where = "mandate " + m.id;
    if (!ids.insert(m.id).second) out.push_back({where, "declared twice"});
    if (!ar::is_date(m.begins)) out.push_back({where, "it begins on no calendar date"});
    for (const auto* d : {&m.ends, &m.issued, &m.review_by})
      if (!detail::date_or_empty(*d)) out.push_back({where, *d + " is not a calendar date"});
    if (!m.ends.empty() && ar::is_date(m.begins) && ar::is_date(m.ends) && !(m.begins < m.ends)) out.push_back({where, "it ends before it begins"});
    if (m.review_every < 0) out.push_back({where, "its period of review is negative"});
    if (m.addressees.empty()) out.push_back({where, "it is addressed to no one"});
  }
  for (const auto& m : r.mandates)
    for (const auto& p : m.earlier) {
      if (!ids.count(p.mandate)) out.push_back({"mandate " + m.id, "it " + std::string(name(p.link)) + " " + p.mandate + ", which is not recorded"});
      if (p.mandate == m.id) out.push_back({"mandate " + m.id, "it " + std::string(name(p.link)) + " itself"});
    }
  for (const auto& v : r.reviews) {
    if (!ids.count(v.mandate)) out.push_back({"review of " + v.mandate, "no such mandate is recorded"});
    if (!ar::is_date(v.date)) out.push_back({"review of " + v.mandate, "it is dated by no calendar date"});
  }
  const auto order = morphe::combinatorics::topological_order(r.mandates.size(), detail::arcs(r));
  if (!order.acyclic) {
    std::string c;
    for (std::size_t k : order.cycle) c += (c.empty() ? "" : " → ") + r.mandates[k].id;
    out.push_back({"the mandates", "their relations run in a circle: " + c});
  }
  return out;
}

// ---- Lifecycle ---------------------------------------------------------------------------------------------------------------

enum class State { pending, in_force, expired, renewed, superseded, consolidated, terminated };

inline const char* name(State s) {
  switch (s) {
    case State::pending: return "not yet in force";
    case State::in_force: return "in force";
    case State::expired: return "expired";
    case State::renewed: return "renewed";
    case State::superseded: return "superseded";
    case State::consolidated: return "consolidated";
    default: return "terminated";
  }
}

struct Status {
  std::string mandate;
  State state = State::pending;
  std::string by;  // the mandate that renewed, superseded, consolidated or terminated it
  std::string why;
};

inline Status status(const Registry& r, const std::string& id, const std::string& at) {
  const Mandate* m = r.mandate(id);
  if (!m) throw std::invalid_argument("diplomacy::mandates: no mandate " + id);
  Status s{id, State::pending, "", ""};
  if (at < m->begins) return s.why = "it begins on " + m->begins, s;
  // A later mandate in force by then that ends, replaces or merges it.
  for (const auto& later : r.mandates)
    for (const auto& p : later.earlier) {
      if (p.mandate != id || at < later.begins) continue;
      if (p.link == Link::supersedes) return {id, State::superseded, later.id, "superseded by " + later.id + " from " + later.begins};
      if (p.link == Link::consolidates) return {id, State::consolidated, later.id, "consolidated into " + later.id + " from " + later.begins};
      if (p.link == Link::terminates) return {id, State::terminated, later.id, "terminated by " + later.id + " from " + later.begins};
    }
  if (m->ends.empty() || at < m->ends) return s.state = State::in_force, s.why = m->ends.empty() ? "in force, open-ended" : "in force until " + m->ends, s;
  for (const auto& later : r.mandates)
    for (const auto& p : later.earlier)
      if (p.mandate == id && p.link == Link::renews && later.begins <= at) return {id, State::renewed, later.id, "renewed by " + later.id + " from " + later.begins};
  return s.state = State::expired, s.why = "it ended on " + m->ends, s;
}

inline std::vector<std::string> in_force(const Registry& r, const std::string& at) {
  std::vector<std::string> v;
  for (const auto& m : r.mandates)
    if (status(r, m.id, at).state == State::in_force) v.push_back(m.id);
  return v;
}

struct Lineage {
  std::string mandate;
  std::vector<std::string> before, after;         // what it continues, and what continues it, nearest first
  std::vector<std::string> roots;                 // the first mandates of its lineage (with nothing before them)
  std::vector<std::string> themes_added, addressees_added;  // what it has that its roots had none of
};

inline Lineage lineage(const Registry& r, const std::string& id) {
  if (const auto ps = problems(r); !ps.empty()) throw std::invalid_argument("diplomacy::mandates: " + ps[0].where + ": " + ps[0].what);
  const auto ix = detail::index(r);
  auto it = ix.find(id);
  if (it == ix.end()) throw std::invalid_argument("diplomacy::mandates: no mandate " + id);
  const auto a = detail::arcs(r);
  Lineage l;
  l.mandate = id;
  for (const auto& x : morphe::combinatorics::down(r.mandates.size(), a, it->second)) l.before.push_back(r.mandates[x.element].id);
  for (const auto& x : morphe::combinatorics::up(r.mandates.size(), a, it->second)) l.after.push_back(r.mandates[x.element].id);
  std::set<std::string> root_themes, root_addressees;
  std::vector<std::size_t> candidates{it->second};
  for (const auto& b : l.before) candidates.push_back(ix.at(b));
  for (std::size_t k : candidates)
    if (r.mandates[k].earlier.empty()) {
      l.roots.push_back(r.mandates[k].id);
      root_themes.insert(r.mandates[k].themes.begin(), r.mandates[k].themes.end());
      root_addressees.insert(r.mandates[k].addressees.begin(), r.mandates[k].addressees.end());
    }
  if (!l.before.empty()) {
    for (const auto& t : r.mandates[it->second].themes)
      if (!root_themes.count(t)) l.themes_added.push_back(t);
    for (const auto& x : r.mandates[it->second].addressees)
      if (!root_addressees.count(x)) l.addressees_added.push_back(x);
  }
  return l;
}

// ---- Review ------------------------------------------------------------------------------------------------------------------

struct Due {
  std::string mandate;
  std::string last;      // the last review recorded on or before the date (empty: none)
  std::string due;       // when the next falls due (empty: no clause of review)
  bool overdue = false, never = false;
};

inline Due due(const Registry& r, const std::string& id, const std::string& at) {
  const Mandate* m = r.mandate(id);
  if (!m) throw std::invalid_argument("diplomacy::mandates: no mandate " + id);
  Due d{id, "", "", false, true};
  for (const auto& v : r.reviews)
    if (v.mandate == id && v.date <= at && v.date > d.last) d.last = v.date;
  d.never = d.last.empty();
  if (m->review_every > 0) d.due = ar::add_days(d.last.empty() ? m->begins : d.last, m->review_every);
  if (!m->review_by.empty() && (d.last.empty() || d.last < m->review_by) && (d.due.empty() || m->review_by < d.due)) d.due = m->review_by;
  d.overdue = !d.due.empty() && d.due <= at;
  return d;
}

// ---- Overlap and resources ---------------------------------------------------------------------------------------------------

struct Overlap {
  std::string a, b;
  std::vector<std::string> addressees, themes;
};

inline std::vector<Overlap> overlaps(const Registry& r, const std::string& at) {
  std::vector<Overlap> out;
  const auto live = in_force(r, at);
  for (std::size_t i = 0; i < live.size(); i++)
    for (std::size_t j = i + 1; j < live.size(); j++) {
      const Mandate &x = *r.mandate(live[i]), &y = *r.mandate(live[j]);
      Overlap o{x.id, y.id, {}, {}};
      for (const auto& a : x.addressees)
        if (std::find(y.addressees.begin(), y.addressees.end(), a) != y.addressees.end()) o.addressees.push_back(a);
      for (const auto& t : x.themes)
        if (std::find(y.themes.begin(), y.themes.end(), t) != y.themes.end()) o.themes.push_back(t);
      if (!o.addressees.empty() && !o.themes.empty()) out.push_back(std::move(o));
    }
  return out;
}

struct Load {
  std::string addressee;
  std::size_t mandates = 0, resourced = 0;
};

// Addressee by addressee, the mandates in force it carries and those of them resourced.
inline std::vector<Load> loads(const Registry& r, const std::string& at) {
  std::map<std::string, Load> m;
  for (const auto& id : in_force(r, at)) {
    const Mandate& x = *r.mandate(id);
    for (const auto& a : x.addressees) {
      Load& l = m[a];
      l.addressee = a;
      l.mandates++;
      l.resourced += x.resourced;
    }
  }
  std::vector<Load> out;
  for (auto& [_, l] : m) out.push_back(l);
  return out;
}

// ---- Signs -------------------------------------------------------------------------------------------------------------------

struct Sign {
  std::string kind, where, what;
};

// The signs at a date; with an undertaking, also whether each mandate in force was given with authority and to participants.
inline std::vector<Sign> signs(const Registry& r, const std::string& at, const relations::Undertaking* u = nullptr) {
  if (const auto ps = problems(r); !ps.empty()) throw std::invalid_argument("diplomacy::mandates: " + ps[0].where + ": " + ps[0].what);
  std::vector<Sign> out;
  for (const auto& id : in_force(r, at)) {
    const Mandate& m = *r.mandate(id);
    const std::string where = "mandate " + id;
    const Due d = due(r, id, at);
    if (d.overdue) out.push_back({"review overdue", where, "its review fell due on " + d.due + (d.last.empty() ? ", and none is recorded" : "; the last was on " + d.last)});
    else if (d.never && ar::days_between(m.begins, at) > 365) out.push_back({"never reviewed", where, "in force since " + m.begins + " and never reviewed"});
    if (m.ends.empty() && m.review_every == 0 && m.review_by.empty()) out.push_back({"open-ended", where, "it has no end and no clause of review"});
    if (!m.resourced) out.push_back({"unresourced", where, m.resources.empty() ? "no resources are stated" : "its resources: " + m.resources});
    const Lineage l = lineage(r, id);
    if (!l.themes_added.empty()) {
      std::string t;
      for (const auto& x : l.themes_added) t += (t.empty() ? "" : ", ") + x;
      out.push_back({"creep", where, "its lineage from " + l.roots[0] + " has added " + t});
    }
    if (u) {
      if (!m.issued_by.empty()) {
        if (!u->participant(m.issued_by)) out.push_back({"authority", where, m.issued_by + ", who gave it, is not a participant"});
        else if (!relations::position(*u, m.issued_by, m.issued.empty() ? m.begins : m.issued).formal_authority())
          out.push_back({"authority", where, m.issued_by + " gave it holding no authority recorded: neither constituted nor authorised"});
      }
      for (const auto& a : m.addressees)
        if (!u->participant(a)) out.push_back({"addressee", where, a + " is addressed and is not a participant"});
    }
  }
  for (const auto& o : overlaps(r, at)) out.push_back({"overlap", "mandates " + o.a + " and " + o.b, "both in force on " + o.addressees[0] + " about " + o.themes[0]});
  return out;
}

}  // namespace diplomacy::mandates
