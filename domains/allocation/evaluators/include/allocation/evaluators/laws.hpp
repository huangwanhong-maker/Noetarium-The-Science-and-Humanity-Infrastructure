// An allocation evaluated by an ensemble of laws learned from a record, within what the record supports (ADR 0139).
//
// Each law of the ensemble (kinesis's induction: bootstrap refits, rival forms, the linear benchmark) maps a state of one
// value per recipient and an input of one value per recipient to the next state. The allocation's grants enter as inputs in
// its order, `between` steps apart, and the laws run on for `after` steps; its worth under a law is the sum of the final
// state. The reading:
//   objective    the weighted mean worth over the laws, negated; per recipient its mean final value; the laws' spread as a
//                report;
//   constraint   the steps at which an input or a state left the record's support, verified (the box of the record's ranges):
//                an allocation that drives a law where the record never went is not evaluated by it (Lucas 1976; the survey of
//                evolutionary allocation, Part VIII §6.B).
// Learned laws give estimates, and that is their standing. Two kept allocations are compared across the laws by `rank`:
// the claim that the first is better holds, fails, is arguable or undecided, with its difference by law.
#pragma once

#include <allocation/evolution/problem.hpp>
#include <allocation/evolution/search.hpp>

#include <kinesis/induction/evaluators.hpp>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace allocation::evaluators {

namespace ae = allocation::evolution;
namespace ki = kinesis::induction;
namespace ha = heuresis::assessment;

struct LawsSpec {
  std::size_t resource = 0;
  double unit = 1;                     // input per unit of the resource's amount
  std::vector<ki::Member> laws;
  std::vector<double> start;           // the state at the start, one value per recipient
  std::size_t between = 1, after = 4;
  std::optional<ki::Support> states, inputs;  // the record's support
  double margin = 0;                   // allowed beyond each range, as a share of it
};

// The inputs an allocation makes: one step for each grant in its order, `between` steps apart, then `after` steps of none.
inline std::vector<std::vector<double>> inputs_of(const LawsSpec& s, const ae::Allocation& a) {
  const std::size_t n = s.start.size();
  if (s.resource >= a.amount.size() || a.amount[s.resource].size() != n) throw ae::Refusal("the laws' state is not one value per recipient of the allocation");
  std::vector<char> seen(n, 0);
  std::vector<std::size_t> order;
  for (const std::size_t i : a.granted)
    if (i < n && !seen[i]) seen[i] = 1, order.push_back(i);
  for (std::size_t i = 0; i < n; i++)
    if (!seen[i] && a.amount[s.resource][i] > 0) order.push_back(i);
  std::vector<std::vector<double>> u;
  for (std::size_t k = 0; k < order.size(); k++) {
    std::vector<double> step(n, 0.0);
    step[order[k]] = a.amount[s.resource][order[k]] * s.unit;
    u.push_back(step);
    for (std::size_t j = 1; j < s.between && k + 1 < order.size(); j++) u.push_back(std::vector<double>(n, 0.0));
  }
  for (std::size_t j = 0; j < s.after; j++) u.push_back(std::vector<double>(n, 0.0));
  return u;
}

inline ha::Assessment read_laws(const LawsSpec& s, const ae::Allocation& a) {
  if (s.laws.empty()) throw ae::Refusal("an ensemble of laws needs at least one law");
  const auto u = inputs_of(s, a);
  const std::size_t n = s.start.size();
  std::vector<double> units(n, 0.0);
  double W = 0, mean = 0, lo = std::numeric_limits<double>::infinity(), hi = -std::numeric_limits<double>::infinity();
  std::size_t flags = 0;
  for (const auto& m : s.laws) {
    const auto r = ki::run(m, s.start, u, s.states ? &*s.states : nullptr, s.inputs ? &*s.inputs : nullptr, s.margin);
    flags = std::max(flags, r.flags.size());
    double worth = 0;
    for (std::size_t i = 0; i < n; i++) worth += r.states.back()[i], units[i] += m.weight * r.states.back()[i];
    W += m.weight, mean += m.weight * worth, lo = std::min(lo, worth), hi = std::max(hi, worth);
  }
  for (double& v : units) v /= W;
  ha::Assessment as;
  ha::Metric support;
  support.name = "steps beyond the record's support", support.role = ha::Role::constraint, support.value = static_cast<double>(flags), support.uncertainty = 0,
  support.standing = ha::Standing::verified;
  as.metrics.push_back(support);
  ha::Metric obj;
  obj.name = "worth lost under the learned laws", obj.role = ha::Role::objective, obj.value = -mean / W, obj.units = units, obj.standing = ha::Standing::estimated;
  as.metrics.push_back(obj);
  ha::Metric spread;
  spread.name = "spread of worth across the laws", spread.role = ha::Role::report, spread.value = hi - lo, spread.uncertainty = 0, spread.standing = ha::Standing::estimated;
  as.metrics.push_back(spread);
  return as;
}

inline ha::Evaluator laws_evaluator(std::shared_ptr<const ae::Composition> c, LawsSpec spec, std::string name = "laws learned from a record") {
  if (spec.start.size() != c->recipients()) throw ae::Refusal("a starting value for each recipient");
  return ae::evaluator(c, std::move(name), ha::Kind::surrogate, ha::Standing::estimated,
                       [spec](const ae::Allocation& a, calx::random::Stream&) { return read_laws(spec, a); }, false, 1, "1");
}

// Two allocations compared across the laws: the claim that the first is worth more, its standing and its difference by law.
inline ki::Ranking rank(const LawsSpec& s, const ae::Allocation& first, const ae::Allocation& second) {
  const auto worth = [](const ki::Run& r) {
    double w = 0;
    for (double v : r.states.back()) w += v;
    return w;
  };
  return ki::rank(s.laws, s.start, inputs_of(s, first), inputs_of(s, second), worth, s.states ? &*s.states : nullptr, s.inputs ? &*s.inputs : nullptr, 1e-12, s.margin);
}

}  // namespace allocation::evaluators
