// A participatory budget as a sequential mechanism (ADR 0141).
//
// Voters approve projects; projects cost money; a budget is spent. A rule of participatory budgeting is, read as a mechanism, an
// order of the projects decoded by the budget: walk the order, fund each project that still fits, skip the rest. Utilitarian
// greedy, the rule most cities use, is the order by approvals (or by approvals per unit of cost); any selection that leaves no
// project affordable is the decoding of the order that lists its projects first, in the order they were bought, so the Method of
// Equal Shares with its completion is one too, when its completion spends what can be spent. A selection that leaves money some
// further project could use is the outcome of no order: decoding by the budget is exhaustive.
//
// So the orders of the projects are a space of mechanisms' outcomes, and the composition searches it: each project a recipient
// with a fixed demand, its cost, on one budget; the order of grants the genotype. Two declared objectives are minimised, the share
// of voters left with no funded project they approved and the approvals not met (the negated sum of funded approvals), and the
// search keeps a front between them; the city's rule and Equal Shares are read on the same two measures, so that where they stand
// against what orders can reach is seen. Neither objective is the right one; they are declared, and their front is the answer.
#pragma once

#include <allocation/evolution/search.hpp>

#include <nomos/allocation/budgeting.hpp>

#include <algorithm>
#include <cmath>
#include <memory>
#include <numeric>
#include <string>
#include <vector>

namespace allocation::evolution {

namespace na = nomos::allocation;

struct BudgetReading {
  std::vector<std::size_t> funded;
  double spent = 0;
  double approvals = 0;                // Σ over voters of the funded projects they approved
  double nothing = 0;                  // the share of voters with no funded project they approved
  std::vector<double> nothing_by_group;  // the same within each group of voters, when groups are given
};

inline BudgetReading read_budget(const na::Election& e, std::vector<std::size_t> funded, const std::vector<std::size_t>* group = nullptr, std::size_t groups = 0) {
  BudgetReading r;
  std::vector<char> in(e.costs.size(), 0);
  for (std::size_t j : funded) {
    if (j >= e.costs.size()) throw Refusal("a funded project that is not one");
    in[j] = 1, r.spent += e.costs[j];
  }
  r.funded = std::move(funded);
  std::vector<double> count(groups, 0), none(groups, 0);
  std::size_t without = 0;
  for (std::size_t v = 0; v < e.approvals.size(); v++) {
    std::size_t met = 0;
    for (std::size_t j : e.approvals[v]) met += in[j];
    r.approvals += static_cast<double>(met);
    without += met == 0;
    if (group && groups) {
      const std::size_t g = (*group)[v];
      if (g < groups) count[g] += 1, none[g] += met == 0;
    }
  }
  r.nothing = e.approvals.empty() ? 0 : static_cast<double>(without) / static_cast<double>(e.approvals.size());
  for (std::size_t g = 0; g < groups; g++) r.nothing_by_group.push_back(count[g] > 0 ? none[g] / count[g] : 0);
  return r;
}

// Walks an order of the projects, funding each that still fits.
inline std::vector<std::size_t> decode_order(const na::Election& e, const std::vector<std::size_t>& order) {
  std::vector<std::size_t> funded;
  double left = e.budget;
  for (std::size_t j : order) {
    if (j >= e.costs.size()) throw Refusal("an order names a project that is not one");
    if (e.costs[j] <= left + 1e-9) funded.push_back(j), left -= e.costs[j];
  }
  return funded;
}

inline std::vector<double> votes(const na::Election& e) {
  std::vector<double> v(e.costs.size(), 0);
  for (const auto& a : e.approvals)
    for (std::size_t j : a) v[j] += 1;
  return v;
}

// The order of utilitarian greedy: by approvals, or by approvals per unit of cost; ties to the lesser index.
inline std::vector<std::size_t> order_greedy(const na::Election& e, bool per_cost = false) {
  const auto v = votes(e);
  std::vector<std::size_t> o(e.costs.size());
  std::iota(o.begin(), o.end(), std::size_t{0});
  std::stable_sort(o.begin(), o.end(), [&](std::size_t a, std::size_t b) {
    const double x = per_cost ? v[a] / e.costs[a] : v[a], y = per_cost ? v[b] / e.costs[b] : v[b];
    return x > y;
  });
  return o;
}

// The order that lists a selection's projects first, in the order bought, then the rest by approvals. Decoded, it reproduces the
// selection exactly when no further project fits (exhaustive); otherwise it funds more, and `exact` says so.
inline std::vector<std::size_t> order_of_selection(const na::Election& e, const std::vector<std::size_t>& bought, bool* exact = nullptr) {
  std::vector<char> in(e.costs.size(), 0);
  std::vector<std::size_t> o = bought;
  for (std::size_t j : bought) in[j] = 1;
  for (std::size_t j : order_greedy(e))
    if (!in[j]) o.push_back(j);
  if (exact) {
    auto d = decode_order(e, o), b = bought;
    std::sort(d.begin(), d.end()), std::sort(b.begin(), b.end());
    *exact = d == b;
  }
  return o;
}

// The composition: projects as recipients with fixed demands, their costs, on one budget.
inline Problem budget_problem(const na::Election& e, const std::vector<std::string>& names = {}) {
  na::detail::check_election(e);
  Problem p;
  p.name = "a participatory budget as a sequential mechanism";
  for (std::size_t j = 0; j < e.costs.size(); j++) {
    rounds::Entity x;
    x.id = j < names.size() ? names[j] : "project " + std::to_string(j);
    x.name = x.id;
    x.at["cost"] = std::round(e.costs[j]);
    p.recipients.push_back(x);
  }
  kr::Type money;
  money.name = "budget";
  money.components = {kr::Component{"budget", kr::Quantity::natural, 1, {kr::Kind{"budget", true, std::nullopt, false}}}};
  p.resources.push_back({"budget", money, Carrier::fixed, {kr::Rational(static_cast<long long>(std::floor(e.budget + 1e-9)))}, 0, {}, "cost"});
  return p;
}

struct BudgetSearch {
  Outcome outcome;
  BudgetReading greedy, greedy_per_cost, equal_shares;
  bool equal_shares_is_an_order = false;   // its selection (with completion) is the decoding of an order
  std::vector<BudgetReading> kept;         // what the search kept, best first by its tiers
  std::vector<BudgetReading> front;        // those of the kept that no other kept dominates on the two measures
};

// The orders of the projects searched under the two declared objectives, beside the rules read on them.
// With `propose_rules`, the rules' own orders (greedy, greedy per cost, Equal Shares' as bought) are proposed to the search's first
// generation, credited to their rules, so that what the search keeps is weighed against where the rules stand from the start.
inline BudgetSearch search_budget(const na::Election& e, Settings settings, const std::vector<std::size_t>* group = nullptr, std::size_t groups = 0,
                                  const std::vector<std::string>& names = {}, bool propose_rules = false) {
  auto c = std::make_shared<const Composition>(budget_problem(e, names));
  if (propose_rules) {
    settings.proposed.push_back({"utilitarian greedy", order_greedy(e)});
    settings.proposed.push_back({"greedy per cost", order_greedy(e, true)});
    settings.proposed.push_back({"equal shares", order_of_selection(e, na::equal_shares(e).funded)});
  }
  const auto supporters = na::detail::supporters(e);
  const std::size_t voters = e.approvals.size();
  auto measures = [supporters, voters](const Allocation& a) {
    std::vector<char> reached(voters, 0);
    double approvals = 0;
    for (std::size_t j : a.granted)
      for (std::size_t v : supporters[j]) approvals += 1, reached[v] = 1;
    double without = 0;
    for (char r : reached) without += r == 0;
    return std::make_pair(voters ? without / static_cast<double>(voters) : 0.0, approvals);
  };
  auto evaluate = evaluator(c, "voters and approvals", ha::Kind::closed_form, ha::Standing::computed, [measures](const Allocation& a, calx::random::Stream&) {
    const auto [nothing, approvals] = measures(a);
    ha::Assessment out;
    ha::Metric m1, m2;
    m1.name = "share of voters with nothing", m1.value = nothing, m1.uncertainty = 0;
    m2.name = "approvals not met", m2.value = -approvals, m2.uncertainty = 0;
    out.metrics = {m1, m2};
    return out;
  });
  BudgetSearch S;
  S.outcome = search(c, {evaluate}, settings);
  for (const auto& k : S.outcome.kept) S.kept.push_back(read_budget(e, k.allocation.granted, group, groups));
  for (const auto& a : S.kept) {
    bool beaten = false;
    for (const auto& b : S.kept)
      beaten = beaten || (b.nothing <= a.nothing && b.approvals >= a.approvals && (b.nothing < a.nothing || b.approvals > a.approvals));
    bool seen = false;
    for (const auto& f : S.front) seen = seen || (f.nothing == a.nothing && f.approvals == a.approvals);
    if (!beaten && !seen) S.front.push_back(a);
  }
  S.greedy = read_budget(e, decode_order(e, order_greedy(e)), group, groups);
  S.greedy_per_cost = read_budget(e, decode_order(e, order_greedy(e, true)), group, groups);
  const auto mes = na::equal_shares(e);  // its funded projects include those its completion added
  const std::vector<std::size_t> bought = mes.funded;
  bool exact = false;
  order_of_selection(e, bought, &exact);
  S.equal_shares_is_an_order = exact;
  S.equal_shares = read_budget(e, bought, group, groups);
  return S;
}

}  // namespace allocation::evolution
