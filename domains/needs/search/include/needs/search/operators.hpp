// Operators of variation on statements (ADR 0125; the theory note §4), and the distance between statements.
//
// Mutations, each of which may refuse (nothing to change) and then gives nothing:
//   add        a condition from the pool on an attribute the statement does not use
//   drop       a condition (a group keeps at least one)
//   shift      a number's cut moved to the next cut, up or down
//   flip       a condition turned to its complement (≤ ↔ >, = ↔ ≠)
//   widen      a category's condition taking one more level (= l becomes in {l, m})
//   narrow     one level fewer (in {l, m} becomes = l)
//   swap       a condition replaced by one on another attribute
//   indicator  another indicator, in its direction and its usual domain
//   standard   comparative ↔ normative, the level one an authority declares for the indicator or, failing that, the frame's value
//   frame      a condition added to the frame, or the frame dropped
// Crossovers:
//   join       the conjunction of the two groups (on distinct attributes)
//   meet       the conditions the two share
//   one-point  the first half of one's conditions and the second half of the other's
//   uniform    each condition of either kept by a fair coin
//   graft      one's group with the other's indicator and standard
//
// The distance between two statements is the weighted Jaccard distance of their groups' rows on the discovery part, plus a half
// for another indicator and a quarter for another standard, over 1.75: so that novelty and niching tell statements apart by whom
// they concern, then by what they say of them.
#pragma once

#include <needs/search/space.hpp>

#include <heuresis/evolution/evolution.hpp>

#include <calx/random/stream.hpp>

#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

namespace needs::search {

using Member = heuresis::evolution::Member<Statement>;
using Context = heuresis::evolution::Context<Statement>;

struct Variation {
  const Dataset* d = nullptr;
  const Parts* parts = nullptr;
  const Space* sp = nullptr;
  mutable std::unordered_map<std::string, groups::Rows> rows_memo;  // a group's discovery rows, by its key

  bool fits(const Statement& s) const { return !s.group.conditions.empty() && size(s) <= sp->max_conditions && distinct_attributes(groups::join(s.group, s.frame)); }

  std::optional<Statement> ok(Statement s) const {
    if (!fits(s)) return std::nullopt;
    return s;
  }

  std::vector<std::size_t> pool_not_using(const Statement& s) const {
    std::set<std::string> used;
    for (const auto& c : s.group.conditions) used.insert(c.attribute);
    for (const auto& c : s.frame.conditions) used.insert(c.attribute);
    std::vector<std::size_t> out;
    for (std::size_t k = 0; k < sp->conditions.size(); k++)
      if (!used.count(sp->conditions[k].attribute)) out.push_back(k);
    return out;
  }

  std::optional<Statement> add(const Statement& s, calx::random::Stream& r) const {
    const auto pool = pool_not_using(s);
    if (pool.empty() || size(s) >= sp->max_conditions) return std::nullopt;
    Statement t = s;
    t.group.conditions.push_back(sp->conditions[pool[r.below(pool.size())]]);
    return ok(t);
  }
  std::optional<Statement> drop(const Statement& s, calx::random::Stream& r) const {
    if (s.group.conditions.size() < 2) return std::nullopt;
    Statement t = s;
    t.group.conditions.erase(t.group.conditions.begin() + static_cast<std::ptrdiff_t>(r.below(t.group.conditions.size())));
    return ok(t);
  }
  std::optional<Statement> shift(const Statement& s, calx::random::Stream& r) const {
    std::vector<std::size_t> numeric;
    for (std::size_t k = 0; k < s.group.conditions.size(); k++)
      if (sp->cuts.count(s.group.conditions[k].attribute)) numeric.push_back(k);
    if (numeric.empty()) return std::nullopt;
    Statement t = s;
    auto& c = t.group.conditions[numeric[r.below(numeric.size())]];
    const auto& cuts = sp->cuts.at(c.attribute);
    std::size_t at = 0;
    for (std::size_t k = 0; k < cuts.size(); k++)
      if (std::fabs(cuts[k] - c.number) < std::fabs(cuts[at] - c.number)) at = k;
    const bool up = r.uniform() < 0.5;
    if (up && at + 1 < cuts.size()) c.number = cuts[at + 1];
    else if (!up && at > 0) c.number = cuts[at - 1];
    else if (at + 1 < cuts.size()) c.number = cuts[at + 1];
    else if (at > 0) c.number = cuts[at - 1];
    else return std::nullopt;
    c.literal.clear();
    return ok(t);
  }
  std::optional<Statement> flip(const Statement& s, calx::random::Stream& r) const {
    Statement t = s;
    auto& c = t.group.conditions[r.below(t.group.conditions.size())];
    if (c.op == groups::Op::in) return std::nullopt;
    switch (c.op) {
      case groups::Op::le: c.op = groups::Op::gt; break;
      case groups::Op::gt: c.op = groups::Op::le; break;
      case groups::Op::lt: c.op = groups::Op::ge; break;
      case groups::Op::ge: c.op = groups::Op::lt; break;
      case groups::Op::eq: c.op = groups::Op::ne; break;
      default: c.op = groups::Op::eq; break;
    }
    return ok(t);
  }
  std::optional<Statement> widen(const Statement& s, calx::random::Stream& r) const {
    std::vector<std::size_t> cat;
    for (std::size_t k = 0; k < s.group.conditions.size(); k++) {
      const auto& c = s.group.conditions[k];
      if (sp->levels.count(c.attribute) && (c.op == groups::Op::eq || c.op == groups::Op::in)) cat.push_back(k);
    }
    if (cat.empty()) return std::nullopt;
    Statement t = s;
    auto& c = t.group.conditions[cat[r.below(cat.size())]];
    std::vector<std::string> others;
    for (const auto& l : sp->levels.at(c.attribute))
      if (std::find(c.levels.begin(), c.levels.end(), l) == c.levels.end()) others.push_back(l);
    if (others.empty() || c.levels.size() + 1 >= sp->levels.at(c.attribute).size()) return std::nullopt;
    c.levels.push_back(others[r.below(others.size())]);
    std::sort(c.levels.begin(), c.levels.end());
    c.op = groups::Op::in;
    return ok(t);
  }
  std::optional<Statement> narrow(const Statement& s, calx::random::Stream& r) const {
    std::vector<std::size_t> sets;
    for (std::size_t k = 0; k < s.group.conditions.size(); k++)
      if (s.group.conditions[k].op == groups::Op::in && s.group.conditions[k].levels.size() >= 2) sets.push_back(k);
    if (sets.empty()) return std::nullopt;
    Statement t = s;
    auto& c = t.group.conditions[sets[r.below(sets.size())]];
    c.levels.erase(c.levels.begin() + static_cast<std::ptrdiff_t>(r.below(c.levels.size())));
    if (c.levels.size() == 1) c.op = groups::Op::eq;
    return ok(t);
  }
  std::optional<Statement> swap(const Statement& s, calx::random::Stream& r) const {
    Statement t = s;
    const std::size_t k = r.below(t.group.conditions.size());
    const std::string gone = t.group.conditions[k].attribute;
    t.group.conditions.erase(t.group.conditions.begin() + static_cast<std::ptrdiff_t>(k));
    auto pool = pool_not_using(t);
    std::erase_if(pool, [&](std::size_t q) { return sp->conditions[q].attribute == gone; });
    if (pool.empty()) return std::nullopt;
    t.group.conditions.insert(t.group.conditions.begin() + static_cast<std::ptrdiff_t>(k), sp->conditions[pool[r.below(pool.size())]]);
    return ok(t);
  }
  std::optional<Statement> indicator(const Statement& s, calx::random::Stream& r) const {
    if (sp->indicators.size() < 2 || s.standard == Standard::expressed) return std::nullopt;
    Statement t = s;
    std::string ind;
    do ind = sp->indicators[r.below(sp->indicators.size())];
    while (ind == s.indicator);
    t.indicator = ind, t.domain = sp->domain(ind), t.worse_when_higher = d->indicator(ind).higher_is_worse;
    if (t.standard != Standard::comparative) t.level = level_for(t, r);
    if (!std::isfinite(t.level) && t.standard != Standard::comparative) t.standard = Standard::comparative;
    return ok(t);
  }
  double level_for(const Statement& s, calx::random::Stream& r) const {
    const auto& ind = d->indicator(s.indicator);
    if (!ind.levels.empty()) return ind.levels[r.below(ind.levels.size())];
    const double v = evidence::frame_value(*d, s.indicator, s.frame, *parts, Part::discovery);
    if (!std::isfinite(v)) return v;
    // The frame's value, to two significant figures, so that a level reads as a person would state it.
    if (v == 0) return 0;
    const int e = static_cast<int>(std::floor(std::log10(std::fabs(v)))) - 1;
    return e >= 0 ? std::round(v / std::pow(10.0, e)) * std::pow(10.0, e) : std::round(v * std::pow(10.0, -e)) / std::pow(10.0, -e);
  }
  std::optional<Statement> standard(const Statement& s, calx::random::Stream& r) const {
    if (s.standard == Standard::expressed) return std::nullopt;
    Statement t = s;
    if (s.standard == Standard::comparative) {
      t.standard = Standard::normative, t.level = level_for(t, r), t.reference.reset();
      if (!std::isfinite(t.level)) return std::nullopt;
      if (d->indicator(t.indicator).share && !(t.level > 0 && t.level < 1)) return std::nullopt;
    } else {
      t.standard = Standard::comparative, t.level = std::numeric_limits<double>::quiet_NaN();
    }
    return ok(t);
  }
  std::optional<Statement> frame(const Statement& s, calx::random::Stream& r) const {
    Statement t = s;
    if (!t.frame.conditions.empty() && r.uniform() < 0.5) {
      t.frame.conditions.clear();
      return ok(t);
    }
    const auto pool = pool_not_using(s);
    if (pool.empty() || size(s) >= sp->max_conditions) return std::nullopt;
    t.frame.conditions.push_back(sp->conditions[pool[r.below(pool.size())]]);
    return ok(t);
  }

  std::optional<Statement> join(const Statement& a, const Statement& b) const {
    Statement t = a;
    t.group = groups::join(a.group, b.group);
    return ok(t);
  }
  std::optional<Statement> meet(const Statement& a, const Statement& b) const {
    Statement t = a;
    t.group = groups::meet(a.group, b.group);
    if (t.group.conditions.empty() || t.group.key() == a.group.key()) return std::nullopt;
    return ok(t);
  }
  std::optional<Statement> one_point(const Statement& a, const Statement& b, calx::random::Stream& r) const {
    Statement t = a;
    const std::size_t ka = 1 + r.below(a.group.conditions.size()), kb = r.below(b.group.conditions.size() + 1);
    Group g;
    for (std::size_t k = 0; k < ka; k++) g.conditions.push_back(a.group.conditions[k]);
    Group tail;
    for (std::size_t k = kb; k < b.group.conditions.size(); k++) tail.conditions.push_back(b.group.conditions[k]);
    t.group = groups::join(g, tail);
    return ok(t);
  }
  std::optional<Statement> uniform(const Statement& a, const Statement& b, calx::random::Stream& r) const {
    Statement t = a;
    Group g;
    for (const auto* src : {&a.group, &b.group})
      for (const auto& c : src->conditions)
        if (r.uniform() < 0.5) g = groups::join(g, Group{{c}});
    if (g.conditions.empty()) g.conditions.push_back(a.group.conditions[0]);
    t.group = g;
    return ok(t);
  }
  std::optional<Statement> graft(const Statement& a, const Statement& b) const {
    if (a.indicator == b.indicator && a.standard == b.standard) return std::nullopt;
    Statement t = b;
    t.group = a.group, t.frame = a.frame, t.reference.reset();
    return ok(t);
  }

  const groups::Rows& rows(const Statement& s) const {
    const std::string k = s.group.key() + " | " + s.frame.key();
    auto it = rows_memo.find(k);
    if (it != rows_memo.end()) return it->second;
    if (rows_memo.size() > 20000) rows_memo.clear();
    groups::Rows r = groups::rows_of(d->table, s.group, groups::rows_of(d->table, s.frame));
    for (std::size_t i = 0; i < r.size(); i++) r[i] = r[i] && parts->part_of_row[i] == 0;
    return rows_memo.emplace(k, std::move(r)).first->second;
  }
  // Two indicators are one measure read two ways when a declared point of interpretation holds both as readings (ADR 0126): then
  // statements that differ only in them are one need, and the distance does not count the difference.
  bool readings_of_one(const std::string& x, const std::string& y) const {
    for (const auto& p : d->points) {
      if (p.kind != language::PointKind::indicator) continue;
      bool hx = false, hy = false;
      for (const auto& r : p.readings) hx = hx || r.text == x, hy = hy || r.text == y;
      if (hx && hy) return true;
    }
    return false;
  }
  double distance(const Statement& a, const Statement& b) const {
    double dist = groups::jaccard_distance(d->table, rows(a), rows(b));
    if (a.indicator != b.indicator && !readings_of_one(a.indicator, b.indicator)) dist += 0.5;
    if (a.standard != b.standard) dist += 0.25;
    return dist / 1.75;
  }
};

inline const std::vector<std::string>& mutation_names() {
  static const std::vector<std::string> n{"add", "drop", "shift", "flip", "widen", "narrow", "swap", "indicator", "standard", "frame"};
  return n;
}
inline const std::vector<std::string>& crossover_names() {
  static const std::vector<std::string> n{"join", "meet", "one-point", "uniform", "graft"};
  return n;
}

}  // namespace needs::search
