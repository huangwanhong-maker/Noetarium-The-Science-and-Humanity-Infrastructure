// Institutions, their memberships, the complexes they form, and the reciprocity they carry (ADR 0134).
//
// Multilateralism is an institutional form: it coordinates relations among three or more parties on generalised principles of
// conduct, with indivisibility and diffuse reciprocity (Ruggie 1992). Institutions differ in design (membership, scope,
// centralisation, control, flexibility; Koremenos, Lipson and Snidal 2001), and on one issue several stand side by side: a regime
// complex of institutions partially overlapping and non-hierarchical (Raustiala and Victor 2004), nested, overlapping or parallel
// (Alter and Meunier 2009), among which a party belonging to several can choose its forum. A record of memberships makes these
// readings:
//
//   context     at a date, the relation "party p is a member of institution i": a formal context (morphe's concepts). Its concepts
//               are the maximal groups of parties together with every institution they all share, ordered in a lattice: a club of
//               a few parties sharing many institutions is a concept low in it. Its canonical basis of implications says, in the
//               fewest statements, every regularity of membership: "every member of A is a member of B" is nesting, and "every
//               member of A and of B is a member of C" a joint condition (Ganter and Wille 1999; Guigues and Duquenne 1986);
//   complex     the institutions covering an issue, each pair read by membership as nested (one's members within the other's),
//               overlapping (sharing members, neither within the other) or apart; and the parties that can choose their forum;
//   design      each institution's declared design on Koremenos, Lipson and Snidal's dimensions, kept as declared;
//   reciprocity among parties exchanging within an institution, over windows of a declared length: the pairwise imbalance (each
//               pair's flows against each other, 0 when every pair balances: specific reciprocity) and the group imbalance (each
//               party's giving against its receiving from all, 0 when every party balances: diffuse reciprocity; Keohane 1986).
//               A cycle of equal gifts has pairwise imbalance 1 and group imbalance 0: diffuse reciprocity with no specific
//               reciprocity at all, and the group's imbalance never exceeds the pairs' (the theory note, Proposition 10.4). The
//               window over which the imbalances fall is the horizon of the reciprocity Ruggie asks of multilateralism.
//
// Dates are ISO 8601 calendar dates. Nothing here names a real institution; what a record holds is the caller's.
//
// References. Ruggie, J. G. (1992), Multilateralism: the anatomy of an institution, International Organization 46, 561–598.
// Keohane, R. O. (1986), Reciprocity in international relations, International Organization 40, 1–27. Koremenos, B., Lipson, C.
// and Snidal, D. (2001), The rational design of international institutions, International Organization 55, 761–799. Raustiala,
// K. and Victor, D. G. (2004), The regime complex for plant genetic resources, International Organization 58, 277–309. Alter, K.
// J. and Meunier, S. (2009), The politics of international regime complexity, Perspectives on Politics 7, 13–24. Ganter, B. and
// Wille, R. (1999), Formal Concept Analysis, Springer.
#pragma once

#include <archeion/retention/dates.hpp>
#include <morphe/combinatorics/concepts.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::institutions {

namespace mc = morphe::combinatorics;
namespace ar = archeion::retention;

struct Institution {
  std::string id, name, kind;
  std::vector<std::string> issues;
  std::map<std::string, std::string> design;  // membership, scope, centralisation, control, flexibility: as declared
};

struct Membership {
  std::string party, institution, since, until;  // a member on dates d with since <= d < until; empty bounds are open
};

struct Landscape {
  std::vector<std::string> parties;
  std::vector<Institution> institutions;
  std::vector<Membership> memberships;
};

struct Problem {
  std::string where, what;
};

inline const std::vector<std::string>& dimensions() {
  static const std::vector<std::string> d{"membership", "scope", "centralisation", "control", "flexibility"};
  return d;
}

inline std::vector<Problem> problems(const Landscape& l) {
  std::vector<Problem> out;
  std::set<std::string> parties(l.parties.begin(), l.parties.end()), ids;
  if (parties.size() != l.parties.size()) out.push_back({"the parties", "a party is listed twice"});
  for (const auto& i : l.institutions) {
    if (!ids.insert(i.id).second) out.push_back({"institution " + i.id, "declared twice"});
    for (const auto& [d, _] : i.design)
      if (std::find(dimensions().begin(), dimensions().end(), d) == dimensions().end())
        out.push_back({"institution " + i.id, d + " is not a dimension of design (membership, scope, centralisation, control, flexibility)"});
  }
  for (const auto& m : l.memberships) {
    const std::string where = "membership of " + m.party + " in " + m.institution;
    if (!parties.count(m.party)) out.push_back({where, m.party + " is not a party"});
    if (!ids.count(m.institution)) out.push_back({where, m.institution + " is not an institution"});
    for (const auto* d : {&m.since, &m.until})
      if (!d->empty() && !ar::is_date(*d)) out.push_back({where, *d + " is not a calendar date"});
    if (!m.since.empty() && !m.until.empty() && !(m.since < m.until)) out.push_back({where, "it ends before it begins"});
  }
  return out;
}

inline bool member(const Landscape& l, const std::string& party, const std::string& institution, const std::string& at) {
  for (const auto& m : l.memberships)
    if (m.party == party && m.institution == institution && (at.empty() || ((m.since.empty() || m.since <= at) && (m.until.empty() || at < m.until)))) return true;
  return false;
}

// ---- The context and its lattice -------------------------------------------------------------------------------------------

inline mc::Context context(const Landscape& l, const std::string& at = {}) {
  mc::Context c;
  c.objects = l.parties.size(), c.attributes = l.institutions.size();
  c.incidence.assign(c.objects, std::vector<char>(c.attributes, 0));
  for (std::size_t p = 0; p < c.objects; p++)
    for (std::size_t i = 0; i < c.attributes; i++) c.incidence[p][i] = member(l, l.parties[p], l.institutions[i].id, at);
  return c;
}

struct Group {
  std::vector<std::string> parties, institutions;  // a concept: these parties, and every institution they all share
};

struct Lattice {
  std::vector<Group> groups;
  std::vector<std::pair<std::size_t, std::size_t>> covers;  // (lower, upper)
  std::size_t top = 0, bottom = 0;
};

struct Regularity {
  std::vector<std::string> premise, conclusion;  // every member of all the premise is a member of every conclusion
};

namespace detail {

inline void require(const Landscape& l) {
  if (const auto ps = problems(l); !ps.empty()) throw std::invalid_argument("diplomacy::institutions: " + ps[0].where + ": " + ps[0].what);
}

}  // namespace detail

inline Lattice lattice(const Landscape& l, const std::string& at = {}) {
  detail::require(l);
  const auto L = mc::lattice(context(l, at));
  Lattice out;
  for (const auto& c : L.concepts) {
    Group g;
    for (std::size_t p : c.extent) g.parties.push_back(l.parties[p]);
    for (std::size_t i : c.intent) g.institutions.push_back(l.institutions[i].id);
    out.groups.push_back(std::move(g));
  }
  for (const auto& [a, b] : L.covers) out.covers.push_back({a, b});
  out.top = L.top, out.bottom = L.bottom;
  return out;
}

// The canonical basis of the regularities of membership at a date, each conclusion without its premise.
inline std::vector<Regularity> regularities(const Landscape& l, const std::string& at = {}) {
  detail::require(l);
  std::vector<Regularity> out;
  for (const auto& imp : mc::stem_base(context(l, at))) {
    Regularity r;
    for (std::size_t i : imp.premise) r.premise.push_back(l.institutions[i].id);
    for (std::size_t i : imp.conclusion)
      if (std::find(imp.premise.begin(), imp.premise.end(), i) == imp.premise.end()) r.conclusion.push_back(l.institutions[i].id);
    out.push_back(std::move(r));
  }
  return out;
}

// ---- A regime complex ---------------------------------------------------------------------------------------------------------

enum class Relation { nested, overlapping, apart, same };

inline const char* name(Relation r) {
  switch (r) {
    case Relation::nested: return "nested";
    case Relation::overlapping: return "overlapping";
    case Relation::same: return "the same members";
    default: return "apart";
  }
}

struct Pair {
  std::string a, b;
  Relation relation = Relation::apart;
  std::string inner;  // when nested, the one whose members are within the other's
  std::size_t shared = 0;
};

struct Complex {
  std::string issue;
  std::vector<std::string> institutions;
  std::vector<Pair> pairs;
  std::vector<std::pair<std::string, std::vector<std::string>>> forums;  // parties in two or more, with the institutions
};

inline Complex complex(const Landscape& l, const std::string& issue, const std::string& at = {}) {
  detail::require(l);
  Complex c;
  c.issue = issue;
  std::vector<const Institution*> on;
  for (const auto& i : l.institutions)
    if (std::find(i.issues.begin(), i.issues.end(), issue) != i.issues.end()) on.push_back(&i), c.institutions.push_back(i.id);
  auto members = [&](const Institution* i) {
    std::set<std::string> s;
    for (const auto& p : l.parties)
      if (member(l, p, i->id, at)) s.insert(p);
    return s;
  };
  std::vector<std::set<std::string>> m;
  for (const auto* i : on) m.push_back(members(i));
  for (std::size_t x = 0; x < on.size(); x++)
    for (std::size_t y = x + 1; y < on.size(); y++) {
      Pair p{on[x]->id, on[y]->id, Relation::apart, "", 0};
      for (const auto& q : m[x]) p.shared += m[y].count(q);
      const bool x_in_y = p.shared == m[x].size(), y_in_x = p.shared == m[y].size();
      if (p.shared == 0) p.relation = Relation::apart;
      else if (x_in_y && y_in_x) p.relation = Relation::same;
      else if (x_in_y) p.relation = Relation::nested, p.inner = on[x]->id;
      else if (y_in_x) p.relation = Relation::nested, p.inner = on[y]->id;
      else p.relation = Relation::overlapping;
      c.pairs.push_back(p);
    }
  for (const auto& p : l.parties) {
    std::vector<std::string> in;
    for (std::size_t x = 0; x < on.size(); x++)
      if (m[x].count(p)) in.push_back(on[x]->id);
    if (in.size() >= 2) c.forums.push_back({p, in});
  }
  return c;
}

// ---- Reciprocity -------------------------------------------------------------------------------------------------------------

struct Transfer {
  std::string from, to, date;
  double value = 0;  // on a ratio scale, in a unit the caller states
};

struct Window {
  std::string from, until;     // the window [from, until)
  double total = 0;
  double pairwise = 0, group = 0;  // imbalances in [0, 1] (0 when no flow)
};

struct Reciprocity {
  long days = 0;
  std::vector<Window> windows;
  double pairwise = 0, group = 0;  // over the windows, weighted by their flow
};

// Imbalances over consecutive windows of a number of days, from the first transfer's date.
inline Reciprocity reciprocity(const std::vector<Transfer>& transfers, long days) {
  if (days <= 0) throw std::invalid_argument("diplomacy::institutions: a window of at least one day");
  Reciprocity r;
  r.days = days;
  if (transfers.empty()) return r;
  std::string first = transfers[0].date, last = transfers[0].date;
  for (const auto& t : transfers) {
    if (!ar::is_date(t.date)) throw std::invalid_argument("diplomacy::institutions: a transfer on " + t.date + ", no calendar date");
    if (!(t.value >= 0) || !std::isfinite(t.value)) throw std::invalid_argument("diplomacy::institutions: a transfer's value is finite and not negative");
    first = std::min(first, t.date), last = std::max(last, t.date);
  }
  double wp = 0, wg = 0, tot = 0;
  for (std::string a = first; a <= last; a = ar::add_days(a, days)) {
    Window w;
    w.from = a, w.until = ar::add_days(a, days);
    std::map<std::pair<std::string, std::string>, double> flow;
    std::map<std::string, double> out, in;
    for (const auto& t : transfers)
      if (t.date >= w.from && t.date < w.until) flow[{t.from, t.to}] += t.value, out[t.from] += t.value, in[t.to] += t.value, w.total += t.value;
    if (w.total > 0) {
      std::set<std::pair<std::string, std::string>> seen;
      double pair = 0;
      for (const auto& [ab, v] : flow) {
        const std::pair<std::string, std::string> key = ab.first < ab.second ? ab : std::make_pair(ab.second, ab.first);
        if (!seen.insert(key).second) continue;
        const double there = flow.count(key) ? flow.at(key) : 0, back = flow.count({key.second, key.first}) ? flow.at({key.second, key.first}) : 0;
        pair += std::fabs(there - back);
      }
      std::set<std::string> parties;
      for (const auto& [p, _] : out) parties.insert(p);
      for (const auto& [p, _] : in) parties.insert(p);
      double group = 0;
      for (const auto& p : parties) group += std::fabs((out.count(p) ? out.at(p) : 0) - (in.count(p) ? in.at(p) : 0));
      w.pairwise = pair / w.total;
      w.group = group / (2 * w.total);
      wp += w.pairwise * w.total, wg += w.group * w.total, tot += w.total;
    }
    r.windows.push_back(w);
  }
  if (tot > 0) r.pairwise = wp / tot, r.group = wg / tot;
  return r;
}

}  // namespace diplomacy::institutions
