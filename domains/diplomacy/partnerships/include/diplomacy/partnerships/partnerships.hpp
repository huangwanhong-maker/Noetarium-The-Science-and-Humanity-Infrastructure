// A partnership: its members, goals, commitments, contributions, progress and the values each party finds in it (ADR 0132).
//
// What a registry of partnerships keeps today is declared commitments with unstructured progress (the UN's SDG Actions Platform,
// the survey's §B.4.1). A structured record lets what practice asks be read off it:
//
//   screen      a commitment screened by declared criteria, each answered with its reason. The UN DESA platform's SMART criteria
//               are the default scheme: Specific (concrete deliverables tied to a goal, a defined role for each partner),
//               Measurable (an indicator with a unit and a target), Achievable, Resource-based ("a secured resource base, rather
//               than merely project proposals") and Time-bound. Whether a target is achievable is a judgment the record cannot
//               make: it is undecided until someone is recorded as having judged it, with a reason;
//   state       whether a commitment is honoured, honoured late, in progress, overdue or unreported at a date, its target as
//               amended then. No report is not a failure: "absence of observation is not absence of relation" (the monograph,
//               §10.4.2), and an unreported commitment is said to be unreported;
//   ledger      the values a party finds in the partnership (TPI's mission value, organisational gain and indirect gains, net of
//               what it costs), each on a dimension with its scale and unit, with what the evidence allows of it and the
//               reading under which it was taken. Values are never summed across dimensions or parties unless a rule is declared,
//               and a declared rule is refused where its scale forbids it: a weighted sum of ordinal levels means nothing;
//   compare     two vectors of values compared dimension by dimension as their scales permit. When they cross (one better
//               here, worse there) no ranking follows without a declared rule (the monograph's Proposition 12.1);
//   benefit     whether each party's own criterion of benefit is met, read as a dual verdict: its standing (clear when met under
//               every value the evidence allows, untenable when unmet under every one, arguable otherwise, undecided when a value
//               is not observed) and its measure (the probability that every criterion is met, bounded by Fréchet's inequalities,
//               which assume nothing of how the criteria depend on one another). Mutual benefit is the conjunction, reported
//               party by party (the survey's §A.4.5), never one number;
//   diagnose    the modes of failure the survey's §B.4.5 finds detectable from a structured record, each a sign named with
//               where it shows: a paper partnership, drift, resource fragility, capture, collaborative inertia, accountability
//               gaps and succession loss.
//
// Dates are ISO 8601 calendar dates, counted by archeion's day numbers. Nothing here is any institution's scheme until a caller
// declares it; the SMART scheme is given as UN DESA states it, and a caller may declare another.
#pragma once

#include <apodeixis/kernel/verdict.hpp>
#include <archeion/retention/dates.hpp>
#include <kybernesis/kernel/scales.hpp>
#include <kybernesis/kernel/verdicts.hpp>
#include <nomos/measurement/concentration.hpp>
#include <tyche/probability/distributions.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::partnerships {

using apodeixis::kernel::Verdict;
using kybernesis::kernel::Scale;
using kybernesis::kernel::Standing;
using kybernesis::kernel::Measure;

inline constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// ---- The record ------------------------------------------------------------------------------------------------------------

struct Member {
  std::string party, role, since, until;  // a member from since until until (open when empty)
};

struct Goal {
  std::string id, text;
  std::vector<std::string> set_by;  // who set it
};

struct Indicator {
  std::string name, unit;
  double baseline = unknown, target = unknown;
  bool higher_is_better = true;
};

struct Resource {
  std::string party, kind, unit;
  double amount = 0;
  bool secured = false;  // secured, not merely proposed
};

// A change to a commitment, kept with its reason; the commitment as first made is never overwritten.
struct Amendment {
  std::string when, by, reason;
  double target = unknown;   // a new target, or unknown to keep it
  std::string due;           // a new date, or empty to keep it
  std::string transferred_to;  // a member who takes the commitment over, or empty
};

struct Commitment {
  std::string id, by, text, goal, made, due;
  std::map<std::string, std::string> roles;  // each partner's role in it
  Indicator indicator;
  std::vector<Resource> resources;
  std::vector<Amendment> amendments;
  // Whether the target is achievable is a judgment: by whom, with what reason. Empty when no one has judged it.
  std::string achievable_by, achievable_reason;
  bool achievable = false;
};

struct Report {
  std::string commitment, when, by, source;
  double value = unknown;
};

struct Contribution {
  std::string party, commitment, goal, kind, unit, when, source;
  double amount = 0;
};

struct Dimension {
  std::string name, kind, unit;  // kind: mission, organisational, indirect, cost, or a caller's own
  Scale scale = Scale::ratio;
  bool higher_is_better = true;
  std::vector<std::string> levels;  // an ordinal dimension's levels, lowest first
};

// A value a party finds on a dimension: what the evidence allows of it ([lower, upper], equal to value when exact), with a
// standard error when it is an estimate, the reading under which it was taken and its source.
struct Value {
  std::string party, dimension, when, reading, source;
  double value = unknown, lower = unknown, upper = unknown, error = unknown;
  char grade = 'A';
  double lo() const { return std::isfinite(lower) ? lower : value; }
  double hi() const { return std::isfinite(upper) ? upper : value; }
};

// A party's own criterion of benefit: the value it asks on a dimension, under its own reading of the term.
struct Interest {
  std::string party, dimension, reading, source;
  double threshold = unknown;
};

struct Partnership {
  std::string name, registered;
  std::vector<Member> members;
  std::vector<Goal> goals;
  std::vector<Commitment> commitments;
  std::vector<Report> reports;
  std::vector<Contribution> contributions;
  std::vector<Dimension> dimensions;
  std::vector<Value> values;
  std::vector<Interest> interests;

  const Dimension* dimension(const std::string& n) const {
    for (const auto& d : dimensions)
      if (d.name == n) return &d;
    return nullptr;
  }
  const Commitment* commitment(const std::string& id) const {
    for (const auto& c : commitments)
      if (c.id == id) return &c;
    return nullptr;
  }
  bool member(const std::string& party, const std::string& at = {}) const {
    for (const auto& m : members)
      if (m.party == party && (at.empty() || ((m.since.empty() || m.since <= at) && (m.until.empty() || at < m.until)))) return true;
    return false;
  }
};

struct Problem {
  std::string where, what;
};

inline bool date_or_empty(const std::string& d) { return d.empty() || archeion::retention::is_date(d); }

// Every way a partnership's record fails as a declaration, each named.
inline std::vector<Problem> problems(const Partnership& p) {
  std::vector<Problem> out;
  std::set<std::string> parties, goals, commitments, dims;
  if (!date_or_empty(p.registered)) out.push_back({"the partnership", p.registered + " is not a date"});
  for (const auto& m : p.members) {
    parties.insert(m.party);
    if (!date_or_empty(m.since) || !date_or_empty(m.until)) out.push_back({"member " + m.party, "a date is not YYYY-MM-DD"});
  }
  for (const auto& g : p.goals) {
    if (!goals.insert(g.id).second) out.push_back({"goal " + g.id, "declared twice"});
    for (const auto& s : g.set_by)
      if (!parties.count(s)) out.push_back({"goal " + g.id, s + " set it and is no member"});
  }
  for (const auto& d : p.dimensions) {
    if (!dims.insert(d.name).second) out.push_back({"dimension " + d.name, "declared twice"});
    if (d.scale == Scale::ordinal && d.levels.empty()) out.push_back({"dimension " + d.name, "an ordinal dimension names its levels"});
  }
  for (const auto& c : p.commitments) {
    const std::string where = "commitment " + c.id;
    if (!commitments.insert(c.id).second) out.push_back({where, "declared twice"});
    if (!parties.count(c.by)) out.push_back({where, c.by + " made it and is no member"});
    if (!c.goal.empty() && !goals.count(c.goal)) out.push_back({where, "it serves " + c.goal + ", which is no declared goal"});
    if (!date_or_empty(c.made) || !date_or_empty(c.due)) out.push_back({where, "a date is not YYYY-MM-DD"});
    for (const auto& [party, role] : c.roles)
      if (!parties.count(party)) out.push_back({where, party + " has a role in it and is no member"});
    for (const auto& a : c.amendments)
      if (!a.transferred_to.empty() && !parties.count(a.transferred_to)) out.push_back({where, "it is transferred to " + a.transferred_to + ", who is no member"});
  }
  for (const auto& r : p.reports) {
    if (!commitments.count(r.commitment)) out.push_back({"a report of " + r.when, r.commitment + " is no commitment"});
    if (!date_or_empty(r.when) || r.when.empty()) out.push_back({"a report of " + r.commitment, "it has no date"});
  }
  for (const auto& c : p.contributions) {
    if (!parties.count(c.party)) out.push_back({"a contribution of " + c.when, c.party + " is no member"});
    if (!c.commitment.empty() && !commitments.count(c.commitment)) out.push_back({"a contribution of " + c.party, c.commitment + " is no commitment"});
    if (!(c.amount >= 0)) out.push_back({"a contribution of " + c.party, "its amount is negative"});
  }
  for (const auto& v : p.values) {
    const std::string where = "the value of " + v.party + " on " + v.dimension;
    if (!dims.count(v.dimension)) out.push_back({where, v.dimension + " is no declared dimension"});
    if (!parties.count(v.party)) out.push_back({where, v.party + " is no member"});
    if (!(v.lo() <= v.hi())) out.push_back({where, "its lower bound exceeds its upper"});
  }
  for (const auto& i : p.interests) {
    if (!dims.count(i.dimension)) out.push_back({"the interest of " + i.party, i.dimension + " is no declared dimension"});
    if (!parties.count(i.party)) out.push_back({"the interest of " + i.party, i.party + " is no member"});
    if (!std::isfinite(i.threshold)) out.push_back({"the interest of " + i.party, "it states no threshold"});
  }
  return out;
}

// ---- The screen of a commitment -----------------------------------------------------------------------------------------------

struct Criterion {
  std::string name, meaning;
  std::function<Verdict(const Commitment&)> test;
};

struct Screen {
  std::string commitment;
  std::vector<kybernesis::kernel::Check> checks;
  Verdict verdict;
};

// UN DESA's SMART criteria for the SDG Actions Platform, as the platform states them.
inline std::vector<Criterion> smart() {
  return {
      {"specific", "concrete deliverables tied to goals and targets, and a defined role for each partner",
       [](const Commitment& c) {
         const std::string claim = c.id + " is specific";
         if (c.text.empty()) return Verdict::no(claim, "it states no deliverable");
         if (c.goal.empty()) return Verdict::no(claim, "it is tied to no goal");
         for (const auto& [party, role] : c.roles)
           if (role.empty()) return Verdict::no(claim, party + " has no defined role");
         return Verdict::yes(claim, "a deliverable tied to " + c.goal + (c.roles.empty() ? "" : ", each partner's role defined"));
       }},
      {"measurable", "an indicator with a unit and a target",
       [](const Commitment& c) {
         const std::string claim = c.id + " is measurable";
         if (c.indicator.name.empty()) return Verdict::no(claim, "it names no indicator");
         if (!std::isfinite(c.indicator.target)) return Verdict::no(claim, "its indicator has no target");
         return Verdict::yes(claim, c.indicator.name + " to reach " + kybernesis::kernel::number(c.indicator.target) + (c.indicator.unit.empty() ? "" : " " + c.indicator.unit));
       }},
      {"achievable", "judged achievable by someone, with a reason: the record cannot make the judgment",
       [](const Commitment& c) {
         const std::string claim = c.id + " is achievable";
         if (c.achievable_by.empty()) return Verdict::unknown(claim, "no one is recorded as having judged it");
         return c.achievable ? Verdict::yes(claim, c.achievable_by + " judged it so: " + c.achievable_reason)
                             : Verdict::no(claim, c.achievable_by + " judged it not: " + c.achievable_reason);
       }},
      {"resource-based", "a secured resource base, rather than merely project proposals",
       [](const Commitment& c) {
         const std::string claim = c.id + " is resource-based";
         if (c.resources.empty()) return Verdict::no(claim, "it names no resources");
         for (const auto& r : c.resources)
           if (r.secured) return Verdict::yes(claim, r.party + "'s " + r.kind + " is secured");
         return Verdict::no(claim, "its resources are proposed, none secured");
       }},
      {"time-bound", "a date by which the deliverable is due, after the commitment is made",
       [](const Commitment& c) {
         const std::string claim = c.id + " is time-bound";
         if (c.due.empty()) return Verdict::no(claim, "it has no date due");
         if (!c.made.empty() && !(c.made < c.due)) return Verdict::no(claim, "it falls due before it is made");
         return Verdict::yes(claim, "due " + c.due);
       }},
  };
}

inline Screen screen(const Commitment& c, const std::vector<Criterion>& scheme = smart()) {
  Screen s;
  s.commitment = c.id;
  for (const auto& k : scheme) s.checks.push_back({k.name, k.test(c), "criterion"});
  s.verdict = kybernesis::kernel::all_of(s.checks, c.id + " meets the scheme");
  return s;
}

// ---- The state of a commitment ----------------------------------------------------------------------------------------------------

enum class State { honoured, honoured_late, in_progress, overdue, unreported };

inline const char* name(State s) {
  switch (s) {
    case State::honoured: return "honoured";
    case State::honoured_late: return "honoured late";
    case State::in_progress: return "in progress";
    case State::overdue: return "overdue";
    default: return "unreported";
  }
}

struct Standing_of {
  std::string commitment, by, due;
  double target = unknown, latest = unknown, progress = unknown;  // progress: the share of the way from baseline to target
  std::string reached_on, last_report;
  State state = State::unreported;
  std::size_t reports = 0;
};

// The commitment's target and date and holder as amended on or before a date.
inline void as_amended(const Commitment& c, const std::string& at, double& target, std::string& due, std::string& by) {
  target = c.indicator.target, due = c.due, by = c.by;
  for (const auto& a : c.amendments) {
    if (!at.empty() && a.when > at) continue;
    if (std::isfinite(a.target)) target = a.target;
    if (!a.due.empty()) due = a.due;
    if (!a.transferred_to.empty()) by = a.transferred_to;
  }
}

inline bool reaches(double value, double target, bool higher_is_better) {
  return std::isfinite(value) && std::isfinite(target) && (higher_is_better ? value >= target : value <= target);
}

inline Standing_of state(const Partnership& p, const Commitment& c, const std::string& at) {
  Standing_of s;
  s.commitment = c.id;
  as_amended(c, at, s.target, s.due, s.by);
  std::vector<const Report*> rs;
  for (const auto& r : p.reports)
    if (r.commitment == c.id && (at.empty() || r.when <= at)) rs.push_back(&r);
  std::sort(rs.begin(), rs.end(), [](const Report* a, const Report* b) { return a->when < b->when; });
  s.reports = rs.size();
  for (const Report* r : rs)
    if (s.reached_on.empty() && reaches(r->value, s.target, c.indicator.higher_is_better)) s.reached_on = r->when;
  if (!rs.empty()) {
    s.latest = rs.back()->value, s.last_report = rs.back()->when;
    const double b = c.indicator.baseline;
    if (std::isfinite(b) && std::isfinite(s.target) && s.target != b && std::isfinite(s.latest)) s.progress = (s.latest - b) / (s.target - b);
  }
  const bool past_due = !s.due.empty() && !at.empty() && at > s.due;
  if (!s.reached_on.empty()) s.state = (s.due.empty() || s.reached_on <= s.due) ? State::honoured : State::honoured_late;
  else if (rs.empty()) s.state = State::unreported;
  else s.state = past_due ? State::overdue : State::in_progress;
  return s;
}

// ---- The ledger of values ----------------------------------------------------------------------------------------------------

// A party's latest value on each dimension, on or before a date.
inline std::map<std::string, Value> values_of(const Partnership& p, const std::string& party, const std::string& at = {}) {
  std::map<std::string, Value> out;
  for (const auto& v : p.values) {
    if (v.party != party || (!at.empty() && !v.when.empty() && v.when > at)) continue;
    auto it = out.find(v.dimension);
    if (it == out.end() || it->second.when <= v.when) out[v.dimension] = v;
  }
  return out;
}

enum class Order { better, worse, equal, different, overlapping, missing };

inline const char* name(Order o) {
  switch (o) {
    case Order::better: return "better";
    case Order::worse: return "worse";
    case Order::equal: return "equal";
    case Order::different: return "different";
    case Order::overlapping: return "overlapping";
    default: return "missing";
  }
}

// One dimension compared as its scale permits: a nominal dimension tells only equal or different; the others order the values,
// a value certainly better only when what the evidence allows of it lies wholly above the other's.
inline Order order(const Dimension& d, const Value* a, const Value* b) {
  if (!a || !b || !std::isfinite(a->lo()) || !std::isfinite(b->lo())) return Order::missing;
  if (d.scale == Scale::nominal) return a->value == b->value ? Order::equal : Order::different;
  const double alo = a->lo(), ahi = a->hi(), blo = b->lo(), bhi = b->hi();
  if (alo == ahi && blo == bhi && alo == blo) return Order::equal;
  const bool up = d.higher_is_better;
  if (alo > bhi) return up ? Order::better : Order::worse;
  if (ahi < blo) return up ? Order::worse : Order::better;
  return Order::overlapping;
}

enum class Relation { dominates, dominated, equal, crossing, incomparable, undetermined };

inline const char* name(Relation r) {
  switch (r) {
    case Relation::dominates: return "dominates";
    case Relation::dominated: return "is dominated by";
    case Relation::equal: return "equals";
    case Relation::crossing: return "crosses";
    case Relation::incomparable: return "is incomparable with";
    default: return "is undetermined against";
  }
}

struct Comparison {
  Relation relation = Relation::undetermined;
  std::map<std::string, Order> by;  // each dimension's order
  std::string why;
};

// Two vectors of values compared on the given dimensions. They cross when one is better on some dimension and worse on another:
// then no ranking follows without a declared rule (the monograph's Proposition 12.1).
inline Comparison compare(const std::vector<Dimension>& dims, const std::map<std::string, Value>& a, const std::map<std::string, Value>& b) {
  Comparison c;
  std::vector<std::string> better, worse, different, open;
  for (const auto& d : dims) {
    const auto ia = a.find(d.name), ib = b.find(d.name);
    const Order o = order(d, ia == a.end() ? nullptr : &ia->second, ib == b.end() ? nullptr : &ib->second);
    c.by[d.name] = o;
    if (o == Order::better) better.push_back(d.name);
    if (o == Order::worse) worse.push_back(d.name);
    if (o == Order::different) different.push_back(d.name);
    if (o == Order::overlapping || o == Order::missing) open.push_back(d.name);
  }
  auto list = [](const std::vector<std::string>& v) {
    std::string s;
    for (std::size_t i = 0; i < v.size(); i++) s += (i ? ", " : "") + v[i];
    return s;
  };
  if (!better.empty() && !worse.empty())
    c.relation = Relation::crossing, c.why = "better on " + list(better) + ", worse on " + list(worse) + ": no ranking without a declared rule";
  else if (!different.empty())
    c.relation = Relation::incomparable, c.why = "different on " + list(different) + ", which is nominal: neither is better";
  else if (!open.empty())
    c.relation = Relation::undetermined, c.why = "what the evidence allows overlaps, or is not observed, on " + list(open);
  else if (!better.empty())
    c.relation = Relation::dominates, c.why = "better on " + list(better) + ", no worse on any";
  else if (!worse.empty())
    c.relation = Relation::dominated, c.why = "worse on " + list(worse) + ", no better on any";
  else
    c.relation = Relation::equal, c.why = "equal on every dimension";
  return c;
}

// The alternatives that nothing dominates (by name).
inline std::vector<std::string> undominated(const std::vector<Dimension>& dims, const std::map<std::string, std::map<std::string, Value>>& alternatives) {
  std::vector<std::string> out;
  for (const auto& [n, a] : alternatives) {
    bool beaten = false;
    for (const auto& [m, b] : alternatives)
      if (m != n && compare(dims, b, a).relation == Relation::dominates) beaten = true;
    if (!beaten) out.push_back(n);
  }
  return out;
}

// A declared rule of aggregation: each dimension's worth in a common unit, by whom, and why.
struct Rule {
  std::string name, unit, declared_by, reason;
  std::map<std::string, double> weights;
};

struct Aggregate {
  bool refused = false;
  double value = unknown, lower = unknown, upper = unknown;
  std::string unit, why;
};

// A weighted sum by a declared rule, refused when a dimension it weighs is nominal or ordinal (its levels have no distances to
// weigh) or not observed. What the evidence allows of each value is carried through as an interval.
inline Aggregate aggregate(const std::vector<Dimension>& dims, const std::map<std::string, Value>& v, const Rule& rule) {
  Aggregate a;
  a.unit = rule.unit;
  if (rule.declared_by.empty()) return a.refused = true, a.why = "the rule " + rule.name + " is declared by no one", a;
  double x = 0, lo = 0, hi = 0;
  for (const auto& [dim, w] : rule.weights) {
    const Dimension* d = nullptr;
    for (const auto& e : dims)
      if (e.name == dim) d = &e;
    if (!d) return a.refused = true, a.why = dim + " is no declared dimension", a;
    if (d->scale == Scale::nominal || d->scale == Scale::ordinal)
      return a.refused = true, a.why = dim + " is " + kybernesis::kernel::name(d->scale) + ": a weighted sum of its levels means nothing", a;
    const auto it = v.find(dim);
    if (it == v.end() || !std::isfinite(it->second.lo())) return a.refused = true, a.why = dim + " is not observed: it is not zero", a;
    const double s = d->higher_is_better ? w : -w;
    x += s * it->second.value;
    lo += s >= 0 ? s * it->second.lo() : s * it->second.hi();
    hi += s >= 0 ? s * it->second.hi() : s * it->second.lo();
  }
  a.value = x, a.lower = lo, a.upper = hi, a.why = "by " + rule.name + ", declared by " + rule.declared_by + ": " + rule.reason;
  return a;
}

// ---- Benefit, party by party --------------------------------------------------------------------------------------------------------

struct Benefit {
  std::string party;
  std::vector<std::string> met, unmet, open, unobserved;  // each interest's dimension, by its standing
};

// Whether a party's criteria of benefit are met, as a dual verdict. The standing: undecided when a value it asks is not observed;
// untenable when some criterion is unmet under every value the evidence allows; clear when every one is met under every value;
// arguable otherwise. The measure: each criterion's probability of being met (from its estimate and error, or 0 or 1 when the
// evidence decides it, or [0, 1] when it does not), joined by Fréchet's bounds on a conjunction, max(0, Σ lᵢ − (k − 1)) ≤ P ≤
// min uᵢ, which hold however the criteria depend on one another (Fréchet 1935).
inline kybernesis::kernel::Finding<Benefit> benefit(const Partnership& p, const std::string& party, const std::string& at = {}) {
  kybernesis::kernel::Finding<Benefit> f;
  f.value.party = party;
  f.provenance.method = "diplomacy::partnerships::benefit";
  f.provenance.family = kybernesis::kernel::Family::declared;
  const auto vs = values_of(p, party, at);
  double sum_lo = 0, min_hi = 1;
  std::size_t k = 0;
  bool statistical = false;
  for (const auto& i : p.interests) {
    if (i.party != party) continue;
    k++;
    const Dimension* d = p.dimension(i.dimension);
    const auto it = vs.find(i.dimension);
    if (!d || it == vs.end() || !std::isfinite(it->second.lo())) {
      f.value.unobserved.push_back(i.dimension), min_hi = std::min(min_hi, 1.0);
      continue;
    }
    const Value& v = it->second;
    f.provenance.rests_on.push_back(v.source.empty() ? party + " on " + i.dimension : v.source);
    const bool up = d->higher_is_better;
    const bool surely = up ? v.lo() >= i.threshold : v.hi() <= i.threshold;
    const bool never = up ? v.hi() < i.threshold : v.lo() > i.threshold;
    (surely ? f.value.met : never ? f.value.unmet : f.value.open).push_back(i.dimension);
    double plo = surely ? 1 : 0, phi = never ? 0 : 1;
    if (!surely && !never && std::isfinite(v.error) && v.error > 0) {
      const double q = tyche::probability::Normal{v.value, v.error}.cdf(i.threshold);
      plo = phi = up ? 1 - q : q;
      statistical = true;
    }
    sum_lo += plo, min_hi = std::min(min_hi, phi);
  }
  if (k == 0) {
    f.standing = Standing::undecided;
    return f;
  }
  if (statistical) f.provenance.family = kybernesis::kernel::Family::statistical;
  if (!f.value.unobserved.empty()) f.standing = Standing::undecided;
  else if (!f.value.unmet.empty()) f.standing = Standing::untenable;
  else if (f.value.open.empty()) f.standing = Standing::clear;
  else f.standing = Standing::arguable;
  const double lo = std::max(0.0, sum_lo - static_cast<double>(k - 1)), hi = std::max(lo, min_hi);
  f.measure = lo == hi ? Measure::probability(lo) : Measure::interval(lo, hi);
  return f;
}

struct Mutual {
  std::vector<kybernesis::kernel::Finding<Benefit>> parties;
  std::vector<kybernesis::kernel::Check> checks;
  Verdict verdict;
};

// Mutual benefit by a declared standard: each party's benefit judged, and the partnership's only when every party's holds.
inline Mutual mutual(const Partnership& p, const kybernesis::kernel::Standard& standard, const std::string& at = {}) {
  Mutual m;
  std::set<std::string> seen;
  for (const auto& mem : p.members) {
    if (!seen.insert(mem.party).second || (!at.empty() && !p.member(mem.party, at))) continue;
    auto f = benefit(p, mem.party, at);
    const bool any = !f.value.met.empty() || !f.value.unmet.empty() || !f.value.open.empty() || !f.value.unobserved.empty();
    Verdict v = any ? kybernesis::kernel::judge(standard, f, mem.party + " benefits")
                    : Verdict::unknown(mem.party + " benefits", "it has declared no criterion of its benefit");
    m.checks.push_back({mem.party, v, "benefit"});
    m.parties.push_back(std::move(f));
  }
  m.verdict = kybernesis::kernel::all_of(m.checks, "the partnership benefits every party");
  return m;
}

// ---- Modes of failure ------------------------------------------------------------------------------------------------------------

struct Sign {
  std::string mode;   // paper partnership, drift, resource fragility, capture, collaborative inertia, accountability gap, succession loss
  std::string where;
  std::string what;
};

struct Thresholds {
  long inactive_days = 365;    // a partnership with no contribution and no report for this long is on paper
  double drift = 0.5;          // the share of contributions serving no declared goal
  double fragility = 0.6;      // the share of a kind and unit of contribution from one party
  double capture = 0.75;       // the share of goals set by one party, while others set none
  long inertia_days = 365;     // reports that do not move for this long
};

inline std::vector<Sign> diagnose(const Partnership& p, const std::string& at, const Thresholds& th = {}) {
  using archeion::retention::days_between;
  std::vector<Sign> out;
  // A paper partnership: registered, and nothing contributed or reported for a long while.
  {
    std::string last = p.registered;
    for (const auto& c : p.contributions)
      if (c.when <= at && c.when > last) last = c.when;
    for (const auto& r : p.reports)
      if (r.when <= at && r.when > last) last = r.when;
    if (!last.empty() && days_between(last, at) >= th.inactive_days)
      out.push_back({"paper partnership", "the partnership",
                     "nothing has been contributed or reported since " + last + ", " + std::to_string(days_between(last, at)) + " days"});
  }
  // Drift: contributions to no declared goal.
  {
    std::set<std::string> goals;
    for (const auto& g : p.goals) goals.insert(g.id);
    std::size_t all = 0, off = 0;
    for (const auto& c : p.contributions) {
      if (c.when > at) continue;
      all++;
      std::string goal = c.goal;
      if (goal.empty() && !c.commitment.empty())
        if (const Commitment* k = p.commitment(c.commitment)) goal = k->goal;
      if (goal.empty() || !goals.count(goal)) off++;
    }
    if (all > 0 && static_cast<double>(off) >= th.drift * static_cast<double>(all))
      out.push_back({"drift", "the contributions", std::to_string(off) + " of " + std::to_string(all) + " serve no declared goal"});
  }
  // Resource fragility: a kind of contribution in a unit resting on one party; a commitment with no secured resource.
  {
    std::map<std::pair<std::string, std::string>, std::map<std::string, double>> by;
    for (const auto& c : p.contributions)
      if (c.when <= at && c.amount > 0) by[{c.kind, c.unit}][c.party] += c.amount;
    for (const auto& [ku, parties] : by) {
      std::vector<double> x;
      std::string top;
      double most = -1;
      for (const auto& [party, amount] : parties) {
        x.push_back(amount);
        if (amount > most) most = amount, top = party;
      }
      const double cr1 = nomos::measurement::concentration_ratio(x, 1), h = nomos::measurement::herfindahl_hirschman(x);
      if (cr1 >= th.fragility && p.members.size() > 1)
        out.push_back({"resource fragility", ku.first + (ku.second.empty() ? "" : " in " + ku.second),
                       top + " gives " + kybernesis::kernel::number(100 * cr1) + "% of it (Herfindahl–Hirschman " + kybernesis::kernel::number(h) + ")"});
    }
    for (const auto& c : p.commitments) {
      if (!c.made.empty() && c.made > at) continue;
      bool secured = false;
      for (const auto& r : c.resources) secured = secured || r.secured;
      if (!secured) out.push_back({"resource fragility", "commitment " + c.id, "no resource of it is secured"});
    }
  }
  // Capture: the objectives set by one party while other members set none.
  if (!p.goals.empty()) {
    std::map<std::string, std::size_t> set;
    for (const auto& g : p.goals)
      for (const auto& s : g.set_by) set[s]++;
    std::vector<std::string> silent;
    std::set<std::string> seen;
    for (const auto& m : p.members)
      if (p.member(m.party, at) && seen.insert(m.party).second && !set.count(m.party)) silent.push_back(m.party);
    for (const auto& [party, k] : set)
      if (static_cast<double>(k) >= th.capture * static_cast<double>(p.goals.size()) && !silent.empty()) {
        std::string s;
        for (std::size_t i = 0; i < silent.size(); i++) s += (i ? ", " : "") + silent[i];
        out.push_back({"capture", "the goals", party + " set " + std::to_string(k) + " of " + std::to_string(p.goals.size()) + "; " + s + " set none"});
      }
  }
  for (const auto& c : p.commitments) {
    const Standing_of s = state(p, c, at);
    // Collaborative inertia: reports that have not moved toward the target for a long while.
    std::vector<const Report*> rs;
    for (const auto& r : p.reports)
      if (r.commitment == c.id && r.when <= at && std::isfinite(r.value)) rs.push_back(&r);
    std::sort(rs.begin(), rs.end(), [](const Report* a, const Report* b) { return a->when < b->when; });
    if (rs.size() >= 2 && s.state != State::honoured && s.state != State::honoured_late) {
      const Report* last = rs.back();
      const Report* since = last;
      for (auto it = rs.rbegin(); it != rs.rend(); ++it) {
        const bool moved = c.indicator.higher_is_better ? last->value > (*it)->value : last->value < (*it)->value;
        if (moved) break;
        since = *it;
      }
      if (since != last && days_between(since->when, last->when) >= th.inertia_days)
        out.push_back({"collaborative inertia", "commitment " + c.id,
                       "reported at " + kybernesis::kernel::number(last->value) + " or worse since " + since->when + ", without moving toward its target"});
    }
    // Accountability gaps: overdue with no report since the date it fell due; a commitment with nothing to measure it by.
    if (!s.due.empty() && at > s.due && s.state != State::honoured && s.state != State::honoured_late && (s.last_report.empty() || s.last_report <= s.due))
      out.push_back({"accountability gap", "commitment " + c.id, "due " + s.due + ", not honoured, and nothing reported since"});
    if (c.indicator.name.empty() || !std::isfinite(c.indicator.target))
      out.push_back({"accountability gap", "commitment " + c.id, "it has no indicator with a target, so no one can tell whether it is honoured"});
    // Succession loss: the holder has left, the commitment is open, and no one has taken it over.
    if (!p.member(s.by, at) && s.state != State::honoured && s.state != State::honoured_late)
      out.push_back({"succession loss", "commitment " + c.id, s.by + " holds it and is no longer a member; no one has taken it over"});
  }
  return out;
}

}  // namespace diplomacy::partnerships
