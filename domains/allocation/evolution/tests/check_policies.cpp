// Checks of allocation/evolution/policies: a family of the domain's own problems, each a fund claimed by seven areas in proportion
// to their need and people, met in part when it runs short, the price of a job rising a fifth with each grant, and a worth that
// grows with the works each area's people get, with less for each more. A linear rule over the declared features, learned once on
// thirty cases, is followed through the composition's grant system on thirty it never saw; set beside each case's best order (all
// 5,040 enumerated) and the order searched for each case. No rule and no searched order is worth more than a case's best; the rule
// reaches most of the best and generalises; the searched orders, with a hundred evaluations each, reach nearly all of it.

#include <allocation/evolution/policies.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <numeric>
#include <string>

namespace ae = allocation::evolution;
namespace hq = heuresis::sequential;
namespace kr = kategoria::resources;

namespace {

ae::PolicyCase city(std::uint64_t seed) {
  calx::random::Stream r(seed, 0x63697479u, 0, 0);
  ae::Problem p;
  p.name = "a fund for seven areas (generated)";
  const std::size_t n = 7;
  double claims = 0;
  for (std::size_t i = 0; i < n; i++) {
    allocation::rounds::Entity e;
    e.id = "area " + std::to_string(i);
    const double people = std::round(r.uniform(200, 800)), need = r.uniform(0.5, 1.5);
    e.at = {{"people", people}, {"need", need}, {"claim", std::round(need * people / 10)}};
    claims += e.at["claim"];
    p.recipients.push_back(e);
  }
  kr::Type fund;
  fund.name = "fund";
  fund.components = {kr::Component{"fund", kr::Quantity::natural, 1, {kr::Kind{"fund", true, std::nullopt, false}}}};
  ae::Resource res{"fund", fund, ae::Carrier::fixed, {kr::Rational(static_cast<long long>(std::floor(0.4 * claims)))}, 0, {}, "claim"};
  res.partial = true;
  p.resources.push_back(res);
  p.scalars.assign(1 + n, 0.0);
  p.scalars[0] = 1;  // the price of a job
  p.effect = [](ae::State& x, std::size_t i) {
    if (x.held[0][i] > 0) {
      x.scalars[1 + i] = x.held[0][i] / x.scalars[0];  // the works the grant buys
      x.scalars[0] *= 1.2;
    }
  };
  ae::PolicyCase c;
  c.composition = std::make_shared<const ae::Composition>(p);
  c.requests = {std::vector<double>(n)};
  std::vector<double> people(n), need(n);
  for (std::size_t i = 0; i < n; i++) {
    c.requests[0][i] = p.recipients[i].at.at("claim");
    people[i] = p.recipients[i].at.at("people"), need[i] = p.recipients[i].at.at("need");
  }
  c.worth = [people, need, n](const ae::State& s) {
    double w = 0;
    for (std::size_t i = 0; i < n; i++) w += need[i] * people[i] * std::log1p(10 * s.scalars[1 + i] / people[i]);
    return w;
  };
  return c;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  std::vector<ae::PolicyCase> train, test;
  for (int k = 0; k < 30; k++) train.push_back(city(100 + k)), test.push_back(city(900 + k));
  ae::PolicyFeatures f;
  f.attributes = {"people", "need", "claim"};
  f.scalars = {0};  // the price
  const auto tr = ae::policy_instances(train, f), te = ae::policy_instances(test, f);
  const std::size_t d = ae::feature_count(train[0], f);
  s.require("the declared features: a constant, three attributes, the request over what remains, the share remaining, the price", d == 7, std::to_string(d));
  const auto L = hq::train_linear(tr, d, 600, 5);
  double best_mean = 0, rule_mean = 0, search_mean = 0, train_best = 0;
  bool below = true;
  for (std::size_t k = 0; k < te.size(); k++) {
    const double b = hq::best_order(te[k]), rule = hq::worth(te[k], hq::linear(L.weights)), q = hq::search_sequence(te[k], 100, 41 + k);
    below = below && rule <= b + 1e-9 && q <= b + 1e-9;
    best_mean += b / 30, rule_mean += rule / 30, search_mean += q / 30;
  }
  for (const auto& in : tr) train_best += hq::best_order(in) / 30;
  // How much the order matters here: the mean share of the best that each case's worst order reaches.
  double worst_mean = 0;
  for (const auto& in : te) {
    std::vector<std::size_t> w(in.system.grants);
    std::iota(w.begin(), w.end(), std::size_t{0});
    double lo = INFINITY;
    do lo = std::min(lo, in.worth(hq::serial(in.system, w, hq::Unmet::defer).state));
    while (std::next_permutation(w.begin(), w.end()));
    worst_mean += lo / hq::best_order(in) / 30;
  }
  std::printf("the worst order reaches %.1f%% of the best, on average\n", 100 * worst_mean);
  s.require("in this family the order matters: the worst order falls well short of the best (below 90% on average)", worst_mean < 0.9, std::to_string(worst_mean));
  std::printf("best %.2f; a rule learned once %.2f (%.1f%%, trained %.1f%%); orders searched per case, 100 evaluations each, %.2f (%.1f%%)\n", best_mean, rule_mean,
              100 * rule_mean / best_mean, 100 * L.train / train_best, search_mean, 100 * search_mean / best_mean);
  s.require("no rule and no searched order is worth more than a case's best order", below);
  s.require("a linear rule learned once reaches most of the best on cases it never saw (above 90%)", rule_mean > 0.9 * best_mean, std::to_string(rule_mean / best_mean));
  s.check("... and generalises: its share of the best on held-out cases against on its training ones", rule_mean / best_mean - L.train / train_best, 0.05);
  s.require("orders searched for each case, a hundred evaluations each, reach nearly all of it (above 95%)", search_mean > 0.95 * best_mean, std::to_string(search_mean / best_mean));
  return s.done();
}
