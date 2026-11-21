// An economy's habits calibrated to records: the accounts first, then behaviour (ADR 0141; the survey of evolutionary allocation,
// Part VI §6.3).
//
// The accounts fix what they measure. From an input–output table: a household's local share of what it buys is the domestic part
// of households' final consumption; a firm's wages, domestic supplies and imported supplies are compensation of employees,
// domestic and imported intermediate use, each over output; what remains of output (taxes on production, operating surplus,
// consumption of fixed capital) has no place in an economy of households and firms, and leaves it with the imports. These are
// read, not fitted.
//
// Behaviour the accounts do not measure is then estimated from a record of circulation, one parameter for one moment: the share of
// its holding a household spends in a period, from the velocity of households' money, their consumption per period over the money
// households and firms hold, as the economy declared runs it after a settling time. The velocity rises with the share, so the share
// that matches a measured velocity is found by bisection on simulated runs; a target beyond what any share reaches is refused with
// the range that can be reached.
//
// What the records do not identify is said: the threshold below which a household is short of money, how much a short household
// spends and buys locally (declared equal to the accounts' local share and to the share fitted unless a caller states otherwise),
// and the share of its holding outside spends at the places' firms; and the money held idle, the opening balances, given which alone
// the velocity fixes the share spent (with much money idle in firms that pay out only what they receive, no share reaches a fast
// velocity, and the calibration is refused). A calibration to one economy's accounts carries that
// economy's habits: national accounts measure what is domestic to a nation, and a city's places are far less local than that.
#pragma once

#include <economics/ledger/regions.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace economics::ledger {

struct AccountShares {
  double local = 0;     // households' domestic consumption over their consumption
  double wages = 0;     // compensation of employees over output
  double supplies = 0;  // domestic intermediate use over output
  double imported = 0;  // imported intermediate use over output
  std::string source;
};

// The habits the accounts fix, on a declared base; a short household's local share set to the accounts' unless stated.
inline Habits habits_from_accounts(const AccountShares& a, Habits base = {}, bool short_local_too = true) {
  if (!(a.local >= 0 && a.local <= 1 && a.wages >= 0 && a.supplies >= 0 && a.wages + a.supplies <= 1))
    throw Refusal("accounts' shares between 0 and 1, wages and domestic supplies together at most output");
  base.local = a.local;
  if (short_local_too) base.short_local = a.local;
  base.wages = a.wages;
  base.supplies = a.supplies;
  base.source = "calibrated: " + (a.source.empty() ? std::string("accounts") : a.source);
  return base;
}

// Households' consumption per period over the money households and firms hold, after `settle` periods, over `periods` more.
inline double consumption_velocity(const std::vector<Place>& places, const Habits& h, std::size_t settle = 20, std::size_t periods = 40) {
  Economy E;
  const auto P = regional(E, places, h);
  E.run(settle);
  const std::size_t from = E.recorder.payments.size(), start = E.holdings.size();
  E.run(periods);
  std::vector<char> household(E.L.agents().size(), 0), holder(E.L.agents().size(), 0);
  for (std::size_t r = 0; r < P.names.size(); r++) {
    for (std::size_t a : P.households[r]) household[a] = holder[a] = 1;
    for (std::size_t a : P.firms[r]) holder[a] = 1;
  }
  double spent = 0;
  for (std::size_t k = from; k < E.recorder.payments.size(); k++) {
    const auto& p = E.recorder.payments[k];
    if (!p.created && !p.destroyed && household[p.from]) spent += p.amount;
  }
  double held = 0;
  for (std::size_t t = start; t < E.holdings.size(); t++)
    for (std::size_t a = 0; a < E.holdings[t].size(); a++)
      if (holder[a]) held += E.holdings[t][a];
  held /= static_cast<double>(E.holdings.size() - start);
  return held > 0 ? spent / static_cast<double>(periods) / held : 0;
}

struct Calibration {
  Habits habits;
  double target = 0, achieved = 0;
  double lowest = 0, highest = 0;  // the velocities the share reaches at its ends
  std::size_t runs = 0;
  std::vector<std::string> identified, unidentified;
};

// The share of its holding a household spends in a period, matched to a velocity per period by bisection; with `short_too`, a
// short household's share follows it (the records do not tell them apart).
inline Calibration calibrate_spending(const std::vector<Place>& places, Habits h, double target, bool short_too = true, double tolerance = 1e-4,
                                      std::size_t most = 40) {
  if (!(target > 0)) throw Refusal("a velocity to match must be positive");
  Calibration c;
  c.target = target;
  auto at = [&](double s) {
    Habits x = h;
    x.spend = s;
    if (short_too) x.short_spend = s;
    c.runs++;
    return consumption_velocity(places, x);
  };
  double lo = 1e-3, hi = 1.0;
  c.lowest = at(lo), c.highest = at(hi);
  if (target < c.lowest || target > c.highest)
    throw Refusal("a velocity of " + std::to_string(target) + " a period is beyond what the share of holdings spent can reach here, [" + std::to_string(c.lowest) + ", " +
                  std::to_string(c.highest) + "]");
  double mid = 0.5, v = 0;
  for (std::size_t k = 0; k < most; k++) {
    mid = 0.5 * (lo + hi);
    v = at(mid);
    if (std::fabs(v - target) <= tolerance * target) break;
    (v < target ? lo : hi) = mid;
  }
  c.habits = h;
  c.habits.spend = mid;
  if (short_too) c.habits.short_spend = mid;
  c.achieved = v;
  c.identified = {"local (households' domestic consumption, the accounts)", "wages (compensation of employees over output, the accounts)",
                  "supplies (domestic intermediate use over output, the accounts)", "spend (the share of holdings spent, by the velocity of households' money)"};
  c.unidentified = {"short_below (the threshold of being short of money)", "short_spend and short_local (declared equal to spend and local)",
                    "exports (the share outside spends at the places' firms)",
                    "the money held idle (opening balances): the velocity fixes the share spent only given them"};
  return c;
}

}  // namespace economics::ledger
