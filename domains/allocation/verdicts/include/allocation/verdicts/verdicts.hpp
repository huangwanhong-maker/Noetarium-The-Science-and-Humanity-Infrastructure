// A round judged against its alternatives under declared principles, never by one score.
//
// The round a person declared is one of many that could have been declared: each stage could have used another rationing rule,
// and the stages could have come in another order. This module enumerates those alternatives (an archive of rules, in the
// survey's words a repertoire for deliberation, Part IV §4), reads each one's result as an outcome per head, and compares them
// under every declared principle at once with nomos's welfare orders:
//
//   - the options no other option beats under every principle (Sen's maximal set), and whether the declared round is among them;
//   - for the declared round against each other undominated option, which principles favour which (the pivot);
//   - where a principle's parameter (inequality aversion, a threshold) flips such a comparison, so that choosing the parameter is
//     seen as the choice it is;
//   - for each entity, what is at stake: the least and the most it receives among the undominated options.
//
// Principles are not added together (Fleurbaey and Tungodden's two tyrannies; the survey, §1.2). Options with the same result
// are one option with several names. The verdict says what follows from what was declared; it does not say which option is just.
#pragma once

#include <allocation/rounds/rounds.hpp>
#include <nomos/welfare/comparison.hpp>
#include <nomos/welfare/orders.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace allocation::verdicts {

using rounds::Refusal;
using rounds::Result;
using rounds::Round;
using nomos::welfare::Outcome;
using nomos::welfare::Principle;

// How a result is read as an outcome for each entity: its level (an attribute, or nothing) plus what it received per head, each
// entity weighed by its people. An entity with nobody counted cannot be read per head and is left out of every option's outcome alike
// (`left_out` says how many).
struct Reading {
  std::string level;                 // the attribute the amount adds to (spending per head, income per head); "" for none
  std::string per = "population";    // the attribute counting people; "" to count each entity once
};

inline Outcome outcome(const Round& r, const Result& res, const Reading& reading, const std::string& name) {
  Outcome o;
  o.name = name;
  for (std::size_t i = 0; i < r.entities.size(); i++) {
    const auto& e = r.entities[i];
    const double people = reading.per.empty() ? 1.0 : e.get(reading.per);
    if (!std::isfinite(people)) throw Refusal("reading a result per head needs '" + reading.per + "' for " + (e.name.empty() ? e.id : e.name));
    if (!(people > 0)) continue;
    const double level = reading.level.empty() ? 0.0 : e.get(reading.level);
    if (!std::isfinite(level)) throw Refusal("reading a result needs the level '" + reading.level + "' of " + (e.name.empty() ? e.id : e.name));
    o.value.push_back(level + res.amount[i] / people);
    o.weight.push_back(people);
  }
  if (o.value.empty()) throw Refusal("no entity has a positive '" + reading.per + "' to read the result per head");
  return o;
}

inline std::size_t left_out(const Round& r, const Reading& reading) {
  if (reading.per.empty()) return 0;
  std::size_t k = 0;
  for (const auto& e : r.entities) k += !(e.get(reading.per) > 0);
  return k;
}

struct Option {
  std::vector<std::string> names;    // the first is how it was made; the others made the same result
  Round round;
  Result result;
  Outcome outcome;
};

struct Variation {
  // The rationing rules a stage that takes one (floor, priority, claims) may use instead of its own.
  std::vector<std::string> rules = {"proportional", "equal_awards", "equal_losses", "talmud", "piniles", "adjusted_proportional"};
  bool orders = true;                // also every order of the stages after eligibility (at most 6 stages)
  std::size_t product_limit = 512;   // rules varied in every combination up to this many; beyond it, one stage at a time
};

namespace detail {

inline bool rationed(const rounds::Stage& s) { return s.kind == "floor" || s.kind == "priority" || s.kind == "claims"; }

inline std::string describe_rules(const Round& r) {
  std::string d;
  for (std::size_t k = 0; k < r.stages.size(); k++)
    if (rationed(r.stages[k])) d += (d.empty() ? "" : ", ") + std::string("stage ") + std::to_string(k + 1) + " by " + r.stages[k].rule;
  return d;
}

inline std::string describe_order(const Round& r) {
  std::string d;
  for (const auto& s : r.stages) d += (d.empty() ? "" : " → ") + s.kind;
  return d;
}

inline bool same(const Result& a, const Result& b, double tol) {
  for (std::size_t i = 0; i < a.amount.size(); i++)
    if (std::abs(a.amount[i] - b.amount[i]) > tol) return false;
  return true;
}

}  // namespace detail

// The declared round first, then its alternatives, each with its result; options whose results agree are merged.
inline std::vector<Option> alternatives(const Round& declared, const Reading& reading, const Variation& v = {}) {
  std::vector<Round> made;
  std::vector<std::string> how;
  made.push_back(declared);
  how.push_back("as declared");

  std::vector<std::size_t> varied;
  for (std::size_t k = 0; k < declared.stages.size(); k++)
    if (detail::rationed(declared.stages[k])) varied.push_back(k);
  std::size_t product = 1;
  for (std::size_t j = 0; j < varied.size() && product <= v.product_limit; j++) product *= v.rules.size();
  if (!varied.empty() && !v.rules.empty()) {
    if (product <= v.product_limit) {
      std::vector<std::size_t> pick(varied.size(), 0);
      while (true) {
        Round r = declared;
        for (std::size_t j = 0; j < varied.size(); j++) r.stages[varied[j]].rule = v.rules[pick[j]];
        made.push_back(r);
        how.push_back("rules: " + detail::describe_rules(r));
        std::size_t j = 0;
        while (j < pick.size() && ++pick[j] == v.rules.size()) pick[j++] = 0;
        if (j == pick.size()) break;
      }
    } else {
      for (std::size_t k : varied)
        for (const auto& rule : v.rules) {
          Round r = declared;
          r.stages[k].rule = rule;
          made.push_back(r);
          how.push_back("rules: stage " + std::to_string(k + 1) + " by " + rule + ", the others as declared");
        }
    }
  }
  if (v.orders) {
    std::size_t first = 0;
    while (first < declared.stages.size() && declared.stages[first].kind == "eligibility") first++;
    const std::size_t m = declared.stages.size() - first;
    if (m > 6) throw Refusal("every order of more than six stages is not enumerated");
    std::vector<std::size_t> perm(m);
    for (std::size_t j = 0; j < m; j++) perm[j] = first + j;
    while (std::next_permutation(perm.begin(), perm.end())) {
      Round r = declared;
      for (std::size_t j = 0; j < m; j++) r.stages[first + j] = declared.stages[perm[j]];
      made.push_back(r);
      how.push_back("order: " + detail::describe_order(r));
    }
  }

  std::vector<Option> out;
  const double tol = 1e-9 * std::max(1.0, declared.amount);
  for (std::size_t j = 0; j < made.size(); j++) {
    Result res = rounds::run(made[j]);
    bool merged = false;
    for (auto& o : out)
      if (detail::same(o.result, res, tol)) {
        o.names.push_back(how[j]);
        merged = true;
        break;
      }
    if (merged) continue;
    Option o;
    o.names.push_back(how[j]);
    o.round = made[j];
    o.result = std::move(res);
    o.outcome = outcome(made[j], o.result, reading, out.empty() ? std::string("the declared round") : "option " + std::to_string(out.size()));
    out.push_back(std::move(o));
  }
  return out;
}

struct Flip {
  std::size_t principle = 0, against = 0;   // the principle whose parameter is varied; the option compared with the declared
  std::vector<double> at;                   // the parameters at which the comparison changes
};

struct Stake {
  double least = 0, most = 0, declared = 0;  // among the undominated options, and under the declared round
  std::size_t best = 0;                      // the undominated option that gives it most
};

struct Judgement {
  std::vector<Option> options;               // [0] is the declared round
  nomos::welfare::Comparison comparison;
  bool declared_undominated = true;
  std::vector<std::size_t> dominating;       // the options better than the declared under every principle
  std::vector<std::pair<std::size_t, std::string>> pivots;  // (option, its pivot against the declared) for each other undominated option
  std::vector<Flip> flips;
  std::vector<Stake> stakes;                 // per entity
  std::vector<std::string> said;
};

struct FlipRange {
  std::size_t principle = 0;
  double lo = 0, hi = 0;
  int steps = 200;
};

inline Judgement judge(const Round& declared, const Reading& reading, const std::vector<Principle>& principles, const Variation& v = {},
                       const std::vector<FlipRange>& ranges = {}) {
  if (principles.empty()) throw Refusal("a judgement needs at least one declared principle");
  Judgement J;
  J.options = alternatives(declared, reading, v);
  std::vector<Outcome> outs;
  for (const auto& o : J.options) outs.push_back(o.outcome);
  J.comparison = nomos::welfare::compare_all(outs, principles);
  const auto& M = J.comparison.maximal;
  J.declared_undominated = std::find(M.begin(), M.end(), 0) != M.end();
  for (const auto& p : J.comparison.pairs) {
    const bool zero_a = p.a == 0, zero_b = p.b == 0;
    if (!zero_a && !zero_b) continue;
    const std::size_t other = zero_a ? p.b : p.a;
    const bool other_dominates = zero_a ? p.relation == nomos::welfare::Relation::dominated : p.relation == nomos::welfare::Relation::dominates;
    if (other_dominates) J.dominating.push_back(other);
  }
  for (std::size_t m : M)
    if (m != 0) J.pivots.push_back({m, nomos::welfare::pivot(J.comparison, 0, m)});
  for (const auto& fr : ranges) {
    if (fr.principle >= principles.size()) throw Refusal("a range of flips names a principle not declared");
    for (std::size_t m : M) {
      if (m == 0) continue;
      Flip f{fr.principle, m, nomos::welfare::flips(principles[fr.principle], J.options[0].outcome, J.options[m].outcome, fr.lo, fr.hi, fr.steps)};
      if (!f.at.empty()) J.flips.push_back(std::move(f));
    }
  }
  const std::size_t n = declared.entities.size();
  J.stakes.assign(n, {});
  for (std::size_t i = 0; i < n; i++) {
    Stake s;
    s.declared = J.options[0].result.amount[i];
    s.least = s.most = J.options[M.front()].result.amount[i];
    s.best = M.front();
    for (std::size_t m : M) {
      const double a = J.options[m].result.amount[i];
      if (a < s.least) s.least = a;
      if (a > s.most) s.most = a, s.best = m;
    }
    J.stakes[i] = s;
  }
  J.said.push_back(std::to_string(J.options.size()) + " distinct options from the declared round's alternatives; " + std::to_string(M.size()) + " undominated under the " +
                   std::to_string(principles.size()) + " principles declared");
  if (const auto k = left_out(declared, reading)) J.said.push_back(std::to_string(k) + " entit" + (k == 1 ? "y" : "ies") + " with no '" + reading.per + "' left out of the outcomes read per head");
  if (J.declared_undominated) J.said.push_back("the declared round is undominated: no alternative is better under every principle");
  else J.said.push_back("the declared round is dominated: " + std::to_string(J.dominating.size()) + " alternative(s) are better under every principle declared");
  return J;
}

}  // namespace allocation::verdicts
