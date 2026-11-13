// The dependence of historical objects: what an object's appearing rested on, and what rests on it.
//
// A record's emergences make a relation among its objects: p comes before x when p is a premise of an
// emergence of x. The relation is acyclic (the record's reading refuses a circle), so its closure is a partial
// order, and everything here is read from that order (morphe's acyclic.hpp) at the layer a reader takes
// (established, attested or proposed): a premise outside the layer is not there.
//
//   cone        the past of an object: its premises, each in its role, then theirs, by steps; with what it
//               inherits, the premises of the objects it specialises or exemplifies, marked as inherited and
//               never as its own. The future the same forward.
//   lineage     the objects an object specialises or exemplifies, nearest first.
//   frontier    at a date, the problems posed and what had answered each by then: the open ones are the
//               questions of that moment. A problem with no date is posed once the premises of one of its
//               emergences have all appeared, or else when its first answer does.
//   support     what rests on an object: what could not have appeared, on the record's own premises, without
//               it. A property of the record and not a counterfactual of history: an emergence's premises
//               are taken as jointly needed, several emergences of one object as alternatives.
//   comparisons the objects that answer a problem an object answers, and whether each is independent of it
//               (no path of dependence either way): the comparison cases.
//   inquiry     the four questions asked of every object, answered from the record, with what the record is
//               silent on said to be so.
//   gather      the relation among chosen classifying objects that the dependences of what stands under each
//               induce, with their numbers: derived, and kept apart from what is asserted of them directly.
//
// Costs are those of morphe's functions: linear in the record but for the reachability used by comparisons.

#pragma once

#include <history/objects/record.hpp>

#include <morphe/combinatorics/acyclic.hpp>

#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace history::dependence {

using objects::Layer;
using objects::Record;
using objects::Standing;
namespace mc = morphe::combinatorics;

// One premise at a layer: `what` is a premise, in a role, of the appearing of `product`.
struct Link {
  std::size_t what = 0, product = 0, emergence = 0;
  std::string role, note;
  Standing standing = Standing::proposed;
  long relation = -1;
};

// The dependence of a record at a layer.
struct Web {
  std::size_t n = 0;
  Layer layer = Layer::established;
  std::vector<Link> links;
  std::vector<mc::Arc> arcs;                 // (premise, product), one for each link
  std::vector<mc::Rule> rules;               // one for each emergence with a premise in the layer
  std::vector<std::size_t> givens;           // the objects with no premise in the layer
};

inline Web web(const Record& R, Layer layer) {
  Web W;
  W.n = R.objects.size();
  W.layer = layer;
  std::vector<char> has(W.n, 0);
  for (std::size_t k = 0; k < R.emergences.size(); k++) {
    const auto& e = R.emergences[k];
    mc::Rule rule{{}, e.product};
    for (const auto& p : e.premises) {
      // A premise written in an emergence with no word of its own stands as the emergence does.
      const Standing s = R.standing(p.said.empty() ? e.said : p.said);
      if (!objects::in_layer(s, layer)) continue;
      W.links.push_back({p.what, e.product, k, p.role, p.note, s, p.relation});
      W.arcs.push_back({p.what, e.product});
      rule.premises.push_back(p.what);
    }
    if (!rule.premises.empty()) { W.rules.push_back(std::move(rule)); has[e.product] = 1; }
  }
  for (std::size_t v = 0; v < W.n; v++)
    if (!has[v]) W.givens.push_back(v);
  return W;
}

// The objects an object specialises or exemplifies, by the layer, nearest first.
struct Ancestor {
  std::size_t object = 0, steps = 0;
  std::string by;  // the type of the first step from the object: specialises or exemplifies
};

inline std::vector<Ancestor> lineage(const Record& R, std::size_t x, Layer layer) {
  std::vector<Ancestor> out;
  std::set<std::size_t> seen{x};
  std::vector<Ancestor> level{{x, 0, ""}};
  while (!level.empty()) {
    std::vector<Ancestor> next;
    for (const auto& at : level)
      for (const auto& r : R.relations) {
        if (r.from != at.object || (r.type != "specialises" && r.type != "exemplifies")) continue;
        // Beyond the first step only specialisation carries on: what exemplifies a class stands under what the class specialises.
        if (at.steps > 0 && r.type != "specialises") continue;
        if (!objects::in_layer(R.standing(r.said), layer) || !seen.insert(r.to).second) continue;
        next.push_back({r.to, at.steps + 1, at.steps == 0 ? r.type : at.by});
      }
    out.insert(out.end(), next.begin(), next.end());
    level = std::move(next);
  }
  return out;
}

// The objects that stand under one: those that specialise or exemplify it, at any number of steps.
inline std::vector<std::size_t> under(const Record& R, std::size_t x, Layer layer) {
  std::vector<std::size_t> out;
  for (std::size_t v = 0; v < R.objects.size(); v++) {
    if (v == x) continue;
    for (const auto& a : lineage(R, v, layer))
      if (a.object == x) { out.push_back(v); break; }
  }
  return out;
}

struct ConeEntry {
  std::size_t object = 0, steps = 0;
  std::string role;          // of a premise one step away; "" further off
  Standing standing = Standing::established;  // of the link one step away
  long inherited = -1;       // the ancestor through which it comes, or −1 when it is the object's own
};

// The cone of x to a depth (0: all of it): past (what it rests on) or future (what rests on it).
inline std::vector<ConeEntry> cone(const Record& R, const Web& W, std::size_t x, bool past, std::size_t depth = 0, bool inherit = true) {
  std::vector<ConeEntry> out;
  std::set<std::size_t> seen{x};
  const auto from = [&](std::size_t seed, long inherited) {
    for (const auto& r : mc::reach(W.n, W.arcs, seed, !past, depth)) {
      if (!seen.insert(r.element).second) continue;
      ConeEntry e{r.element, r.steps, "", Standing::established, inherited};
      if (r.steps == 1)
        for (const auto& l : W.links)
          if ((past ? l.what : l.product) == r.element && (past ? l.product : l.what) == seed) { e.role = l.role; e.standing = l.standing; break; }
      out.push_back(e);
    }
  };
  from(x, -1);
  if (past && inherit)
    for (const auto& a : lineage(R, x, W.layer)) {
      seen.insert(a.object);
      from(a.object, static_cast<long>(a.object));
    }
  return out;
}

// At a day, each problem that has been posed with what had answered it by then, and what answered it after.
struct Question {
  std::size_t problem = 0;
  std::vector<std::size_t> answered, later;  // objects that answer it, appeared by the day and after it
  bool open() const { return answered.empty(); }
};

inline std::vector<Question> frontier(const Record& R, const Web& W, long day) {
  std::vector<Question> out;
  for (std::size_t p = 0; p < W.n; p++) {
    if (!R.is_a(p, "Problem")) continue;
    auto posed = R.appears(p);
    if (!posed) {
      // A problem with no date of its own is posed once the premises of one of its emergences have all appeared.
      for (const auto& e : R.emergences) {
        if (e.product != p || e.premises.empty()) continue;
        long latest = 0;
        bool dated = true;
        for (const auto& pr : e.premises) {
          const auto a = R.appears(pr.what);
          dated = dated && static_cast<bool>(a);
          if (a) latest = std::max(latest, a->first);
        }
        if (dated && (!posed || latest < posed->first)) posed = std::pair{latest, latest};
      }
    }
    if (posed && posed->first > day) continue;
    Question q{p, {}, {}};
    bool known = static_cast<bool>(posed);
    for (const auto& l : W.links) {
      if (l.what != p || l.role != "problem") continue;
      const auto appeared = R.appears(l.product);
      if (!appeared) continue;
      (appeared->first <= day ? q.answered : q.later).push_back(l.product);
      known = true;
    }
    // A problem with no date is posed, as far as the record says, when its first answer appears.
    if (!posed && q.answered.empty()) continue;
    if (known) out.push_back(std::move(q));
  }
  return out;
}

// What rests on c, on the record's own premises.
inline std::vector<std::size_t> support(const Web& W, std::size_t c) { return mc::rests_on(W.n, W.rules, W.givens, c); }

struct Comparison {
  std::size_t object = 0;
  std::vector<std::size_t> problems;  // shared
  bool independent = true;            // no path of dependence either way
};

inline std::vector<Comparison> comparisons(const Record& R, const Web& W, std::size_t x) {
  std::set<std::size_t> mine;
  const auto problems_of = [&](std::size_t v) {
    std::set<std::size_t> out;
    for (const auto& l : W.links)
      if (l.product == v && l.role == "problem") out.insert(l.what);
    for (const auto& a : lineage(R, v, W.layer))
      for (const auto& l : W.links)
        if (l.product == a.object && l.role == "problem") out.insert(l.what);
    return out;
  };
  mine = problems_of(x);
  std::vector<Comparison> out;
  if (mine.empty()) return out;
  std::set<std::size_t> before, after, kin;
  for (const auto& r : mc::down(W.n, W.arcs, x)) before.insert(r.element);
  for (const auto& r : mc::up(W.n, W.arcs, x)) after.insert(r.element);
  for (const auto& a : lineage(R, x, W.layer)) kin.insert(a.object);
  for (std::size_t y = 0; y < W.n; y++) {
    if (y == x || kin.count(y)) continue;
    bool below = false;
    for (const auto& a : lineage(R, y, W.layer)) below = below || a.object == x;
    if (below) continue;
    Comparison c{y, {}, !before.count(y) && !after.count(y)};
    for (std::size_t p : problems_of(y))
      if (mine.count(p)) c.problems.push_back(p);
    if (!c.problems.empty()) out.push_back(std::move(c));
  }
  return out;
}

// The four questions asked of every object, answered from the record at a layer.
struct Answer {
  std::size_t object = 0;
  std::string role, note;
  Standing standing = Standing::established;
  long inherited = -1;  // the ancestor whose premise it is, or −1
  long of = -1;         // for "arose": the problem whose premise it is
};
struct Inquiry {
  std::vector<Answer> problems;    // 1. what it answers
  std::vector<Answer> arose;       // 2. what those problems rested on: why they arose then and there
  std::vector<Answer> answerable;  // 3. what made it possible: its conditions and its sources
  std::vector<Answer> against;     //    and what it argues against
  std::vector<Answer> for_whom;    // 4. who made it, held it, belonged to it
  std::vector<Answer> after;       //    and what drew on it
  std::vector<std::string> silent; // the questions the record does not answer at this layer
};

inline Inquiry inquiry(const Record& R, const Web& W, std::size_t x) {
  Inquiry Q;
  std::vector<std::pair<std::size_t, long>> seeds{{x, -1}};
  for (const auto& a : lineage(R, x, W.layer)) seeds.push_back({a.object, static_cast<long>(a.object)});
  std::set<std::size_t> had;
  for (const auto& [seed, inherited] : seeds)
    for (const auto& l : W.links) {
      if (l.product != seed || !had.insert(l.what * 8 + (l.role == "problem" ? 0 : l.role == "against" ? 1 : 2)).second) continue;
      Answer a{l.what, l.role, l.note, l.standing, inherited, -1};
      (l.role == "problem" ? Q.problems : l.role == "against" ? Q.against : Q.answerable).push_back(std::move(a));
    }
  for (const auto& p : Q.problems)
    for (const auto& l : W.links)
      if (l.product == p.object) Q.arose.push_back({l.what, l.role, l.note, l.standing, -1, static_cast<long>(p.object)});
  for (const auto& r : R.relations) {
    if (!objects::in_layer(R.standing(r.said), W.layer)) continue;
    if (r.from == x && (r.type == "made_by" || r.type == "held_by" || r.type == "member_of")) Q.for_whom.push_back({r.to, r.type, r.note, R.standing(r.said), -1, -1});
  }
  for (const auto& l : W.links)
    if (l.what == x) Q.after.push_back({l.product, l.role, l.note, l.standing, -1, -1});
  if (Q.problems.empty()) Q.silent.push_back("what problem it answers");
  if (!Q.problems.empty() && Q.arose.empty()) Q.silent.push_back("why that problem arose then and there");
  if (Q.answerable.empty()) Q.silent.push_back("what made it possible");
  if (Q.for_whom.empty()) Q.silent.push_back("who made it, and for whom");
  if (Q.after.empty()) Q.silent.push_back("what it made possible after");
  return Q;
}

// The dependences among chosen classifying objects that those of what stands under each induce.
struct Gathering {
  std::vector<std::size_t> classes;             // as given
  std::vector<mc::Gathered> derived;            // between classes (indices into `classes`), with their numbers
  std::vector<std::size_t> inside;              // by class, the dependences within it
  std::vector<Link> asserted;                   // links of the web directly between two of the classes
  std::size_t unplaced = 0;                     // dependences with an end under none of the classes
};

inline Gathering gather(const Record& R, const Web& W, const std::vector<std::size_t>& classes) {
  Gathering G;
  G.classes = classes;
  const std::size_t none = classes.size();
  std::map<std::size_t, std::size_t> index;
  for (std::size_t i = 0; i < classes.size(); i++) index[classes[i]] = i;
  // An object is placed under the nearest chosen class above it; a chosen class under itself.
  std::vector<std::size_t> cls(W.n, none);
  for (std::size_t v = 0; v < W.n; v++) {
    if (index.count(v)) { cls[v] = index[v]; continue; }
    for (const auto& a : lineage(R, v, W.layer))
      if (index.count(a.object)) { cls[v] = index[a.object]; break; }
  }
  std::vector<mc::Arc> placed;
  for (const auto& l : W.links) {
    if (index.count(l.what) && index.count(l.product)) { G.asserted.push_back(l); continue; }
    if (cls[l.what] == none || cls[l.product] == none) { G.unplaced++; continue; }
    placed.push_back({l.what, l.product});
  }
  const auto q = mc::quotient(W.n, placed, cls, none + 1);
  G.derived = q.arcs;
  G.inside.assign(q.inside.begin(), q.inside.begin() + static_cast<long>(none));
  return G;
}

}  // namespace history::dependence
