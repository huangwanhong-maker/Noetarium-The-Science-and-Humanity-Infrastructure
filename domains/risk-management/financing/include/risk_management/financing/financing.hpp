// Financing a loss: instruments as layers of it, what each costs, strategies compared, a tower chosen, and parametric cover with its
// basis risk (theory/risk-and-its-management.md §3).
//
// THE PROBLEM. A loss must be paid by someone. A government, a city or a utility can meet the loss of an event from a budget line
// held for contingencies, a reserve fund, a contingent credit drawn when the event comes, insurance or a catastrophe bond paid before,
// or, failing all of these, by reallocating what was meant for other ends or borrowing at the worst moment. Risk layering gives each
// source a layer of the event's loss: the frequent and small to what is cheap to hold, the rare and severe to what is cheap to
// transfer (Mahul and Ghesquiere 2010, Policy Research Working Paper 5429, World Bank; Clarke, Mahul, Poulter and Teh 2017, The Geneva
// Papers on Risk and Insurance 42, 565, whose framework compares strategies by the cost of the funds they provide).
//
// AN INSTRUMENT pays the layer [a, a + l] of each event's loss (tyche's layers; per occurrence) and costs, each a declaration:
//
//     a year:          fixed + rate · l      (a reserve's opportunity cost, a credit's commitment fee, a bond's spread on its principal)
//     a year:          multiple · E_i        (a premium as a multiple of the layer's expected loss E_i a year: insurance, a bond's risk)
//     at each event:   unit · (what it pays) (each unit it pays costs this: 1 from a budget or a fund, a credit's repayment in present
//                                             value, 0 for what a premium already bought)
//
// so that its expected annual cost is fixed + rate·l + (multiple + unit)·E_i. What no layer pays is UNFUNDED, and costs by declared
// tiers of the amount (a little is reallocated cheaply; much needs cuts or costly borrowing): the convexity that makes transfer worth
// its price at the top. A STRATEGY is a set of instruments whose layers do not overlap, with its unfunded tiers; its expected annual
// cost is the sum of its instruments' and E[unfunded cost], and its cost in the year of the T-year event is what the premiums and fees
// cost every year plus what the T-year loss draws from each source.
//
// A TOWER is chosen from instruments in a declared order, each taking the layer between two boundaries chosen from candidate losses
// (the losses at given return periods, say), by trying every choice: the least expected cost, among those whose unfunded part of the
// T-year loss is within a cap. The candidates are few, so the search is exact over them, and says how many it tried.
//
// PARAMETRIC COVER pays on an index, not the loss: fitted to an indemnity layer by tyche's fit_payout, with its basis risk and its
// premium.
//
// Caveat. Losses here are the record's events (an occurrence cover); covers with an annual aggregate, reinstatements, the timing of
// payouts and the dependence of costs on the market's cycle are not modelled. Every cost is a declaration, and the answer turns on
// them; `compare` says how much each strategy saves against leaving everything unfunded.

#pragma once

#include <tyche/descriptive/exceedance.hpp>
#include <tyche/descriptive/layers.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace risk_management::financing {

namespace td = tyche::descriptive;
inline constexpr double unbounded = std::numeric_limits<double>::infinity();

enum class Kind { budget, reserve, contingent_credit, insurance, catastrophe_bond, other };

inline const char* name_of(Kind k) {
  switch (k) {
    case Kind::budget: return "budget";
    case Kind::reserve: return "reserve";
    case Kind::contingent_credit: return "contingent credit";
    case Kind::insurance: return "insurance";
    case Kind::catastrophe_bond: return "catastrophe bond";
    default: return "other";
  }
}

struct Instrument {
  std::string name;
  Kind kind = Kind::other;
  double attachment = 0, limit = unbounded;  // the layer of each event's loss it pays
  double fixed = 0;                          // a year
  double rate = 0;                           // a year, per unit of limit (needs a finite limit)
  double multiple = 0;                       // a year, times the layer's expected loss
  double unit = 1;                           // each unit paid at an event
};

// What a unit of unfunded loss costs, by tiers of the amount an event leaves unfunded: up_to ascending, the last +inf.
struct Tier {
  double up_to = unbounded;
  double unit = 1;
};

struct Strategy {
  std::string name;
  std::vector<Instrument> instruments;
  std::vector<Tier> unfunded{{unbounded, 1.5}};
};

struct InstrumentCost {
  std::string name;
  Kind kind = Kind::other;
  double expected_paid = 0;   // E_i, a year
  double attach_rate = 0;     // events a year that reach it
  double exhaust_rate = 0;    // events a year that use all of it
  double annual = 0;          // fixed + rate·l + multiple·E_i: paid every year
  double at_events = 0;       // unit · E_i: expected a year
  double expected = 0;        // annual + at_events
};

struct StrategyCost {
  std::string name;
  std::vector<InstrumentCost> instruments;
  double expected_loss = 0;       // the record's expected loss a year
  double expected_unfunded = 0;   // of it, what no layer pays, a year
  double unfunded_cost = 0;       // its expected cost a year, by the tiers
  double expected = 0;            // the strategy's expected cost a year
  double funded_share = 0;        // 1 − expected_unfunded / expected_loss
  // At each return period asked: the event's loss, what each instrument pays, what is unfunded, and the year's cost.
  std::vector<double> return_periods, loss_at, unfunded_at, cost_at;
  std::vector<std::vector<double>> paid_at;  // [period][instrument]
};

namespace detail {

inline double tiered(const std::vector<Tier>& tiers, double amount) {
  double cost = 0, below = 0;
  for (const Tier& t : tiers) {
    const double part = std::min(amount, t.up_to) - below;
    if (part <= 0) break;
    cost += part * t.unit;
    below = t.up_to;
  }
  return cost;
}

inline void validate(const Strategy& s, const char* who) {
  for (const Instrument& i : s.instruments) {
    if (!(i.attachment >= 0 && std::isfinite(i.attachment)) || !(i.limit > 0))
      throw std::invalid_argument(std::string(who) + ": instrument '" + i.name + "' attaches at a ≥ 0 with a limit > 0");
    if (!(i.fixed >= 0 && i.rate >= 0 && i.multiple >= 0 && i.unit >= 0) || !std::isfinite(i.fixed + i.rate + i.multiple + i.unit))
      throw std::invalid_argument(std::string(who) + ": instrument '" + i.name + "' has finite costs ≥ 0");
    if (i.rate > 0 && !std::isfinite(i.limit)) throw std::invalid_argument(std::string(who) + ": instrument '" + i.name + "' is charged on a limit it does not have");
  }
  std::vector<std::pair<double, double>> spans;
  for (const Instrument& i : s.instruments) spans.push_back({i.attachment, i.attachment + i.limit});
  std::sort(spans.begin(), spans.end());
  for (std::size_t k = 1; k < spans.size(); k++)
    if (spans[k].first < spans[k - 1].second) throw std::invalid_argument(std::string(who) + ": strategy '" + s.name + "' has layers that overlap");
  if (s.unfunded.empty() || std::isfinite(s.unfunded.back().up_to)) throw std::invalid_argument(std::string(who) + ": the unfunded tiers end without bound");
  for (std::size_t k = 0; k < s.unfunded.size(); k++) {
    if (!(s.unfunded[k].unit >= 0 && std::isfinite(s.unfunded[k].unit))) throw std::invalid_argument(std::string(who) + ": a tier's cost is finite and ≥ 0");
    if (k > 0 && !(s.unfunded[k].up_to > s.unfunded[k - 1].up_to)) throw std::invalid_argument(std::string(who) + ": tiers ascend");
  }
}

}  // namespace detail

// The record: each event's loss over `years` years (a catalogue's event-loss table); return periods at which to read the year's cost.
inline StrategyCost cost(const Strategy& s, const std::vector<double>& losses, double years, const std::vector<double>& return_periods = {}) {
  const char* who = "risk_management::financing::cost";
  detail::validate(s, who);
  StrategyCost out;
  out.name = s.name;
  out.expected_loss = td::average_annual_loss(losses, years);
  double annual_fixed = 0;
  for (const Instrument& i : s.instruments) {
    const auto L = td::layer_statistics(losses, years, {i.attachment, i.limit}, td::Record::per_event);
    InstrumentCost c;
    c.name = i.name;
    c.kind = i.kind;
    c.expected_paid = L.expected;
    c.attach_rate = L.attachment_rate;
    c.exhaust_rate = L.exhaustion_rate;
    c.annual = i.fixed + (i.rate > 0 ? i.rate * i.limit : 0) + i.multiple * L.expected;
    c.at_events = i.unit * L.expected;
    c.expected = c.annual + c.at_events;
    annual_fixed += c.annual;
    out.expected += c.expected;
    out.instruments.push_back(c);
  }
  const auto paid_by = [&](double loss) {
    std::vector<double> paid;
    for (const Instrument& i : s.instruments) paid.push_back(td::layer_loss(loss, i.attachment, i.limit));
    return paid;
  };
  for (double v : losses) {
    double funded = 0;
    for (double p : paid_by(v)) funded += p;
    const double u = std::max(0.0, v - funded);
    out.expected_unfunded += u / years;
    out.unfunded_cost += detail::tiered(s.unfunded, u) / years;
  }
  out.expected += out.unfunded_cost;
  out.funded_share = out.expected_loss > 0 ? 1 - out.expected_unfunded / out.expected_loss : 1;
  if (!return_periods.empty()) {
    std::vector<double> sorted(losses);
    const auto at = td::occurrence_curve(sorted, years, return_periods);
    for (std::size_t k = 0; k < return_periods.size(); k++) {
      const auto paid = paid_by(at[k]);
      double funded = 0, cost_now = annual_fixed;
      for (std::size_t j = 0; j < paid.size(); j++) {
        funded += paid[j];
        cost_now += s.instruments[j].unit * paid[j];
      }
      const double u = std::max(0.0, at[k] - funded);
      cost_now += detail::tiered(s.unfunded, u);
      out.return_periods.push_back(return_periods[k]);
      out.loss_at.push_back(at[k]);
      out.paid_at.push_back(paid);
      out.unfunded_at.push_back(u);
      out.cost_at.push_back(cost_now);
    }
  }
  return out;
}

struct Comparison {
  StrategyCost baseline;               // nothing arranged: every loss unfunded, by the first strategy's tiers
  std::vector<StrategyCost> strategies;
  std::vector<double> saving;          // baseline.expected − strategy's expected, a year
  std::size_t cheapest = 0;
};

inline Comparison compare(const std::vector<Strategy>& strategies, const std::vector<double>& losses, double years, const std::vector<double>& return_periods = {}) {
  if (strategies.empty()) throw std::invalid_argument("risk_management::financing::compare: at least one strategy");
  Comparison out;
  out.baseline = cost(Strategy{"nothing arranged", {}, strategies.front().unfunded}, losses, years, return_periods);
  double least = unbounded;
  for (std::size_t k = 0; k < strategies.size(); k++) {
    out.strategies.push_back(cost(strategies[k], losses, years, return_periods));
    out.saving.push_back(out.baseline.expected - out.strategies.back().expected);
    if (out.strategies.back().expected < least) least = out.strategies.back().expected, out.cheapest = k;
  }
  return out;
}

struct TowerOptions {
  double cap_period = 0;            // > 0: the return period at which the unfunded part is capped
  double cap = unbounded;           // the most the event of that period may leave unfunded
  bool top_unbounded = false;       // the last instrument may take everything above its attachment
};

struct Tower {
  Strategy strategy;                // the instruments with their layers
  StrategyCost cost;
  std::size_t tried = 0, feasible = 0;
};

// Instruments in their order from the bottom, each taking [b_{i−1}, b_i) with b_0 = 0 and b_1 < … < b_k chosen from the candidates
// (and +inf for the top if allowed); an instrument may be left out by a layer of no width, so each subset in order is tried.
inline Tower best_tower(const std::vector<Instrument>& order, std::vector<double> candidates, const std::vector<Tier>& unfunded,
                        const std::vector<double>& losses, double years, TowerOptions o = {}) {
  const char* who = "risk_management::financing::best_tower";
  if (order.empty()) throw std::invalid_argument(std::string(who) + ": at least one instrument");
  std::sort(candidates.begin(), candidates.end());
  candidates.erase(std::unique(candidates.begin(), candidates.end()), candidates.end());
  for (double c : candidates)
    if (!(c > 0 && std::isfinite(c))) throw std::invalid_argument(std::string(who) + ": candidate boundaries are finite and > 0");
  std::vector<double> levels{0};
  for (double c : candidates) levels.push_back(c);
  if (o.top_unbounded) levels.push_back(unbounded);
  const std::size_t k = order.size(), m = levels.size();
  Tower best;
  double least = unbounded;
  const std::vector<double> periods = o.cap_period > 0 ? std::vector<double>{o.cap_period} : std::vector<double>{};
  // Enumerate non-decreasing sequences idx[1..k] over 0..m−1.
  std::vector<std::size_t> seq(k, 0);
  for (;;) {
    Strategy s{"tower", {}, unfunded};
    bool ok = true;
    std::size_t prev = 0;
    for (std::size_t i = 0; i < k && ok; i++) {
      const std::size_t top = seq[i];
      if (top < prev) ok = false;
      else if (top > prev) {
        Instrument ins = order[i];
        ins.attachment = levels[prev];
        ins.limit = levels[top] - levels[prev];
        if (!std::isfinite(ins.limit) && ins.rate > 0) ok = false;  // a rate on an unbounded limit
        s.instruments.push_back(ins);
      }
      prev = std::max(prev, top);
    }
    if (ok && !s.instruments.empty()) {
      best.tried++;
      const auto c = cost(s, losses, years, periods);
      const bool within = periods.empty() || c.unfunded_at.front() <= o.cap;
      if (within) {
        best.feasible++;
        if (c.expected < least) {
          least = c.expected;
          best.strategy = s;
          best.cost = c;
        }
      }
    }
    // next sequence
    std::size_t i = k;
    while (i > 0 && seq[i - 1] == m - 1) i--;
    if (i == 0) break;
    seq[i - 1]++;
    for (std::size_t j = i; j < k; j++) seq[j] = seq[i - 1];
  }
  if (best.feasible == 0) throw std::invalid_argument(std::string(who) + ": no tower leaves the capped event's unfunded part within the cap");
  return best;
}

struct Parametric {
  td::Payout payout;
  td::BasisRisk risk;
  double expected_paid = 0;   // a year
  double premium = 0;         // multiple × expected_paid
  double covered = 0;         // what the indemnity layer would pay, a year
};

// A payout on an index fitted to the indemnity layer [a, a + l] of each event's loss; weights are each event's rate (1/years each).
inline Parametric parametric(const std::vector<double>& index, const std::vector<double>& losses, double years, double attachment, double limit, double multiple,
                             std::size_t grid = 64) {
  const char* who = "risk_management::financing::parametric";
  if (index.size() != losses.size() || losses.size() < 2) throw std::invalid_argument(std::string(who) + ": an index for each of at least two events");
  if (!(years > 0) || !(limit > 0 && std::isfinite(limit)) || !(attachment >= 0) || !(multiple >= 0)) throw std::invalid_argument(std::string(who) + ": years, a finite limit, an attachment and a multiple");
  std::vector<double> covered(losses.size()), weight(losses.size(), 1 / years);
  for (std::size_t i = 0; i < losses.size(); i++) covered[i] = td::layer_loss(losses[i], attachment, limit);
  const auto F = td::fit_payout(index, covered, limit, weight, grid);
  Parametric out;
  out.payout = F.payout;
  out.risk = F.risk;
  out.expected_paid = F.risk.paid;
  out.premium = multiple * F.risk.paid;
  out.covered = F.risk.covered;
  return out;
}

}  // namespace risk_management::financing
