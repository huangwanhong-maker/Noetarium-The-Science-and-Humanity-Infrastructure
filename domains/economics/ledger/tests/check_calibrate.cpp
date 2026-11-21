// Checks of economics/ledger/calibrate: habits taken from accounts exactly, shares that cannot be refused; the velocity of
// households' money rising with the share of holdings they spend; a share recovered from the velocity it makes; a velocity beyond
// reach refused with the range that can be reached; and the economy of places calibrated to the EU27's accounts of 2022 and the
// euro area's velocity of households' money that year, with what the records leave unidentified said.
//
// The record's values (data/products/economy_habits.py, run 2026-11-21 on Eurostat's naio_10_cp1750 for EU27_2020 in 2022, its
// nama_10_gdp for EA20 and the ECB's M1): households' local share 0.936239, wages 0.232759, domestic supplies 0.461391, imported
// supplies 0.077657 of output; households' consumption 7,116,870.7 million euros over a mean M1 of 11,556,559 million, 0.051319 a
// month.

#include <economics/ledger/calibrate.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdio>
#include <string>

using namespace economics::ledger;

int main() {
  calx::verification::Suite s;
  // Firms hold little money idle (500 each, declared): with 4,000 each, idle in firms that pay out only what they receive, no share
  // of holdings spent turns the money over as fast as the euro area's households do (at most 0.042 a month), and the calibration
  // is refused; how much money lies idle is not in the records either.
  const std::vector<Place> places = {{"north", 4, 2, 2000, 500}, {"south", 4, 2, 2000, 500}, {"east", 4, 2, 2000, 500}};
  AccountShares eu{0.936239, 0.232759, 0.461391, 0.077657, "Eurostat naio_10_cp1750, EU27_2020, 2022"};
  {
    const Habits h = habits_from_accounts(eu);
    s.require("habits from accounts: households' local share, firms' wages and domestic supplies, exactly as the accounts give them",
              h.local == eu.local && h.short_local == eu.local && h.wages == eu.wages && h.supplies == eu.supplies && h.source.find("calibrated") == 0);
    bool refused = false;
    try {
      habits_from_accounts({0.9, 0.7, 0.5, 0.0, ""});
    } catch (const Refusal&) {
      refused = true;
    }
    s.require("wages and domestic supplies beyond output refused", refused);
  }
  Habits base = habits_from_accounts(eu);
  base.short_below = 0;
  {
    double prev = 0;
    bool rising = true;
    for (double sp : {0.05, 0.1, 0.2, 0.4, 0.8}) {
      Habits x = base;
      x.spend = x.short_spend = sp;
      const double v = consumption_velocity(places, x);
      rising = rising && v > prev;
      prev = v;
    }
    s.require("the velocity of households' money rises with the share of holdings they spend", rising);
  }
  {
    Habits truth = base;
    truth.spend = truth.short_spend = 0.3;
    const double v = consumption_velocity(places, truth);
    const auto c = calibrate_spending(places, base, v);
    s.check("a share of 0.3 recovered from the velocity it makes", c.habits.spend - 0.3, 0.01);
    bool refused = false;
    try {
      calibrate_spending(places, base, 50.0);
    } catch (const Refusal& e) {
      refused = std::string(e.what()).find("beyond what the share of holdings spent can reach") != std::string::npos;
    }
    s.require("a velocity beyond reach refused, with the range that can be reached", refused);
  }
  {
    const auto c = calibrate_spending(places, base, 0.051319);
    s.check("calibrated to the euro area's velocity of households' money (0.0513 a month): the velocity matched", c.achieved / c.target - 1, 1e-3);
    std::printf("the share of holdings a household spends a month, calibrated: %.4f (%zu runs); the velocities reachable [%.4f, %.4f]\n", c.habits.spend, c.runs, c.lowest,
                c.highest);
    s.require("... and what the records leave unidentified is said: the threshold of being short, how short households spend, exports, the money idle",
              c.unidentified.size() == 4 && c.identified.size() == 4);
  }
  return s.done();
}
