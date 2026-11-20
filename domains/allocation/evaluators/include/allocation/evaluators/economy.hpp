// An allocation of money evaluated in an economy of places whose every payment is posted (ADR 0139).
//
// Each recipient of the allocation is a place of the economics domain's regional economy (households and firms that trade
// with one another and with outside, households spending by whether they are short of money). The money each recipient was
// granted reaches its households in the allocation's order, one grant every `spacing` periods, so that money given first is
// spent before the next grant arrives; the economy runs for `periods`, and the readings follow each place's grant from where
// it entered (the ledger's tracer):
//   objective    the money re-spent in its own place after it entered, summed over the places (R2 + R3 + … of every grant),
//                negated, since objectives are minimised; per recipient, its own grant's re-spending, for the comparison under
//                principles;
//   reports      the grants' LM3 weighted by their amounts; the share of granted money that left its place; payments that had
//                to wait for money;
//   constraint   the accounts' audit, verified: a candidate whose run does not balance is infeasible, not good (the survey of
//                evolutionary allocation, Part VI §6.2: a winner by a leak is no winner).
// The behaviours are written rules, so the reading is computed from the declarations: what the economy as declared implies,
// a ranking of allocations under one model, never that a place behaves so.
#pragma once

#include <allocation/evolution/problem.hpp>
#include <allocation/evolution/search.hpp>

#include <economics/ledger/regions.hpp>

#include <cmath>
#include <memory>
#include <string>
#include <vector>

namespace allocation::evaluators {

namespace ae = allocation::evolution;
namespace el = economics::ledger;
namespace ha = heuresis::assessment;

struct EconomySpec {
  std::size_t resource = 0;               // the resource whose amounts are money
  double unit = 1;                        // whole units of money per unit of the resource's amount
  std::vector<el::Place> places;          // one for each recipient, in the problem's order
  el::Habits habits;
  std::vector<std::vector<double>> trade; // between the places' firms (empty: equal)
  double spacing = 1;                     // periods between successive grants in the allocation's order
  std::size_t periods = 60;
  std::size_t rounds = 8;                 // the rounds the tracer tells apart
};

struct EconomyReading {
  ha::Assessment assessment;
  std::vector<el::Trace> traces;  // each place's grant followed
  el::Outcome outcome;
};

inline EconomyReading read_economy(const EconomySpec& spec, const ae::Allocation& a) {
  const std::size_t n = spec.places.size();
  if (n == 0) throw ae::Refusal("an economy of places needs a place for each recipient");
  if (spec.resource >= a.amount.size() || a.amount[spec.resource].size() != n) throw ae::Refusal("the economy's places are not the allocation's recipients");
  if (!(spec.unit > 0) || !(spec.spacing >= 0)) throw ae::Refusal("a positive unit of money and a spacing of grants that is not negative");
  el::Economy E(spec.rounds);
  const auto P = el::regional(E, spec.places, spec.habits, spec.trade);
  std::vector<std::pair<std::size_t, el::Amount>> grants;
  std::vector<double> when;
  std::vector<char> seen(n, 0);
  auto add = [&](std::size_t i) {
    if (i >= n || seen[i]) return;
    seen[i] = 1;
    const el::Amount m = static_cast<el::Amount>(std::floor(a.amount[spec.resource][i] * spec.unit + 1e-9));
    if (m <= 0) return;
    when.push_back(static_cast<double>(grants.size()) * spec.spacing);
    grants.push_back({i, m});
  };
  for (const std::size_t i : a.granted) add(i);
  for (std::size_t i = 0; i < n; i++) add(i);  // a recipient holding money it was not granted in the word (none, by the composition)
  el::grant_places(E, P, grants, when);
  EconomyReading R;
  R.outcome = E.run(spec.periods);
  R.traces = E.tracer.traces();
  ha::Assessment& as = R.assessment;
  ha::Metric balance;
  balance.name = "accounts that do not balance", balance.role = ha::Role::constraint, balance.value = R.outcome.audit.holds ? 0 : 1, balance.uncertainty = 0,
  balance.standing = ha::Standing::verified;
  as.metrics.push_back(balance);
  if (!R.outcome.audit.holds) {
    as.status = ha::Status::refused;
    as.reason = "the accounts do not balance: " + R.outcome.audit.failure;
    return R;
  }
  const ha::Standing standing = std::string(R.outcome.tally.standing()) == "computed" ? ha::Standing::computed
                                : std::string(R.outcome.tally.standing()) == "estimated" ? ha::Standing::estimated : ha::Standing::proposed;
  std::vector<double> respent(n, 0);
  double lm3 = 0, granted = 0, leaked = 0;
  for (const auto& t : R.traces) {
    std::size_t i = n;
    for (std::size_t k = 0; k < n; k++)
      if (P.names[k] == t.origin) i = k;
    if (i == n) continue;
    double r = 0;
    for (std::size_t k = 1; k < t.reached.size(); k++) r += t.reached[k];
    respent[i] = r / spec.unit;
    lm3 += t.lm3() * t.injected, granted += t.injected, leaked += t.leaked_total();
  }
  ha::Metric obj;
  obj.name = "local re-spending lost";
  obj.role = ha::Role::objective;
  for (double v : respent) obj.value -= v;
  obj.uncertainty = 0, obj.units = respent, obj.standing = standing;
  as.metrics.push_back(obj);
  auto report = [&](std::string name, double v) {
    ha::Metric m;
    m.name = std::move(name), m.role = ha::Role::report, m.value = v, m.uncertainty = 0, m.standing = standing;
    as.metrics.push_back(m);
  };
  report("LM3 of the grants", granted > 0 ? lm3 / granted : 0);
  report("share of granted money that left its place", granted > 0 ? leaked / granted : 0);
  report("payments that waited for money", static_cast<double>(R.outcome.queued));
  report("share of decisions by model-generated behaviour", R.outcome.tally.model_share());
  return R;
}

inline ha::Evaluator economy_evaluator(std::shared_ptr<const ae::Composition> c, EconomySpec spec, std::string name = "an economy of places") {
  if (spec.places.size() != c->recipients()) throw ae::Refusal("the economy has " + std::to_string(spec.places.size()) + " places for " + std::to_string(c->recipients()) + " recipients");
  return ae::evaluator(c, std::move(name), ha::Kind::agent_model, ha::Standing::computed,
                       [spec](const ae::Allocation& a, calx::random::Stream&) { return read_economy(spec, a).assessment; }, false, 5, "1");
}

}  // namespace allocation::evaluators
