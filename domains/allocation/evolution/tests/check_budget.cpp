// Checks of allocation/evolution/budget: on an invented budget of ten projects whose every subset can be listed, utilitarian greedy
// is the decoding of the order by approvals (and per cost, of the order by approvals per unit of cost), as nomos's rule computes it;
// the Method of Equal Shares with its completion is the decoding of an order; every order decodes to a selection to which no
// project can be added; and the front of what the search over orders keeps lies on the true front of the two declared
// objectives, its two ends among it; and with the rules' orders proposed to a short search, each rule stands on or behind its front.

#include <allocation/evolution/budget.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>

namespace ae = allocation::evolution;
namespace na = nomos::allocation;

namespace {
std::vector<std::size_t> sorted(std::vector<std::size_t> v) {
  std::sort(v.begin(), v.end());
  return v;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  // Ten projects; sixty voters who mostly approve the first five, twenty who mostly approve the last five.
  na::Election e;
  calx::random::Stream r(41, 1, 0, 0);
  double total = 0;
  for (int j = 0; j < 10; j++) e.costs.push_back(static_cast<double>(1 + r.below(10))), total += e.costs.back();
  e.budget = std::floor(0.4 * total);
  for (int v = 0; v < 80; v++) {
    std::vector<std::size_t> a;
    const bool many = v < 60;
    for (std::size_t j = 0; j < 10; j++)
      if (r.uniform() < ((j < 5) == many ? 0.6 : 0.08)) a.push_back(j);
    e.approvals.push_back(a);
  }
  s.require("utilitarian greedy is the decoding of the order by approvals",
            sorted(ae::decode_order(e, ae::order_greedy(e))) == sorted(na::utilitarian_greedy(e).funded));
  s.require("... and per unit of cost, of the order by approvals per unit of cost",
            sorted(ae::decode_order(e, ae::order_greedy(e, true))) == sorted(na::utilitarian_greedy(e, true).funded));
  {
    bool exact = false;
    ae::order_of_selection(e, na::equal_shares(e).funded, &exact);
    s.require("the Method of Equal Shares with its completion is the decoding of an order", exact);
  }
  // Every subset: feasible and maximal ones, with the two measures.
  struct Point {
    double nothing, approvals;
  };
  std::vector<Point> maximal;
  for (std::uint32_t mask = 0; mask < 1024; mask++) {
    double spent = 0;
    std::vector<std::size_t> f;
    for (std::size_t j = 0; j < 10; j++)
      if (mask >> j & 1u) spent += e.costs[j], f.push_back(j);
    if (spent > e.budget) continue;
    bool can_add = false;
    for (std::size_t j = 0; j < 10; j++)
      if (!(mask >> j & 1u) && spent + e.costs[j] <= e.budget) can_add = true;
    if (can_add) continue;
    const auto rd = ae::read_budget(e, f);
    maximal.push_back({rd.nothing, rd.approvals});
  }
  auto dominated = [&](const Point& p) {
    for (const auto& q : maximal)
      if (q.nothing <= p.nothing && q.approvals >= p.approvals && (q.nothing < p.nothing || q.approvals > p.approvals)) return true;
    return false;
  };
  {
    calx::random::Stream o(7, 2, 0, 0);
    bool all_maximal = true;
    for (int t = 0; t < 200; t++) {
      std::vector<std::size_t> w(10);
      std::iota(w.begin(), w.end(), std::size_t{0});
      for (std::size_t k = 10; k > 1; k--) std::swap(w[k - 1], w[o.below(k)]);
      const auto f = ae::decode_order(e, w);
      double spent = 0;
      for (std::size_t j : f) spent += e.costs[j];
      for (std::size_t j = 0; j < 10; j++)
        if (std::find(f.begin(), f.end(), j) == f.end() && spent + e.costs[j] <= e.budget) all_maximal = false;
    }
    s.require("every order decodes to a selection to which no project can be added (200 orders)", all_maximal);
  }
  ae::Settings st;
  st.strategy.mu = 30, st.strategy.lambda = 30, st.strategy.generations = 80;
  st.keep = 12, st.keep_apart = 0, st.seed = 3;
  const auto S = ae::search_budget(e, st);
  bool on_front = !S.front.empty();
  double least_nothing = 1, most_approvals = 0, true_least = 1, true_most = 0;
  for (const auto& k : S.front) {
    on_front = on_front && !dominated({k.nothing, k.approvals});
    least_nothing = std::min(least_nothing, k.nothing), most_approvals = std::max(most_approvals, k.approvals);
  }
  for (const auto& p : maximal) true_least = std::min(true_least, p.nothing), true_most = std::max(true_most, p.approvals);
  std::size_t true_front = 0;
  for (const auto& p : maximal) true_front += !dominated(p);
  s.require("the front of what the search kept lies on the true front of the two objectives", on_front,
            std::to_string(S.front.size()) + " on the kept front, of " + std::to_string(true_front) + " maximal selections on the true front");
  s.require("... among them its two ends: the fewest voters with nothing and the most approvals met",
            std::fabs(least_nothing - true_least) < 1e-12 && most_approvals == true_most,
            std::to_string(least_nothing) + " / " + std::to_string(true_least) + ", " + std::to_string(most_approvals) + " / " + std::to_string(true_most));
  s.require("the rules are read on the same two measures", S.greedy.approvals > 0 && S.equal_shares.approvals > 0 && S.equal_shares_is_an_order);
  {
    // The rules' orders proposed to a search of two generations: each rule then stands on or behind its kept front.
    ae::Settings few;
    few.strategy.mu = 6, few.strategy.lambda = 6, few.strategy.generations = 2;
    few.keep = 60, few.keep_apart = 0, few.seed = 5;
    const auto T = ae::search_budget(e, few, nullptr, 0, {}, true);
    auto behind = [&](const ae::BudgetReading& r) {
      for (const auto& f : T.front)
        if (f.nothing <= r.nothing + 1e-12 && f.approvals >= r.approvals) return true;
      return false;
    };
    s.require("with the rules' orders proposed, each rule stands on or behind the kept front (greedy, per cost, equal shares)",
              behind(T.greedy) && behind(T.greedy_per_cost) && behind(T.equal_shares));
  }
  return s.done();
}
