// Checks of industrial_ecology::emissions on an invented community, each answer worked by hand: potentials of two assessments; an
// entry as activity × factor; BASIC, BASIC+ and the territorial total, generation supplied to the grid and waste from outside kept
// out of the first two and biogenic CO2 out of all three; refusals of what the structure forbids; completeness with its gaps named;
// the IPCC's Approach 1 for uncertainty and key categories; and mitigation: two measures on one entry overlapping, the overlap
// counted once by the joint abatement and shared by the Shapley value, the curve's marginal abatements depending on its order, the
// joint result not depending on the order of application, and a pathway against a target and a budget.

#include <industrial_ecology/emissions/emissions.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace industrial_ecology::emissions;

namespace {

Entry est(std::string id, Sector s, int scope, std::string sub, std::string gas, double activity, double factor, double ua = 0, double uf = 0) {
  Entry e;
  e.id = std::move(id), e.sector = s, e.scope = scope, e.subsector = std::move(sub), e.gas = std::move(gas), e.activity = activity, e.factor = factor;
  e.u_activity = ua, e.u_factor = uf;
  return e;
}
Entry keyed(std::string id, Sector s, int scope, Key k, std::string in = "") {
  Entry e;
  e.id = std::move(id), e.sector = s, e.scope = scope, e.key = k, e.included_in = std::move(in);
  return e;
}
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  s.require("potentials: a tonne of methane is 28 t CO2e under AR5, 29.8 (fossil) or 27.0 (non-fossil) under AR6; nitrous oxide 265 and 273",
            ar5().gwp.at("CH4") == 28 && ar6().gwp.at("CH4 fossil") == 29.8 && ar6().gwp.at("CH4 non-fossil") == 27.0 && ar5().gwp.at("N2O") == 265 && ar6().gwp.at("N2O") == 273 &&
                !ar6().gwp.count("CH4"));

  Inventory I;
  I.name = "an invented town", I.year = 2025, I.population = 50000, I.gdp = 2.0e9, I.potentials = ar5();
  // Buildings burn gas (1,000 t CO2) and use grid power (100,000 MWh at 0.4 t/MWh = 40,000 t); a plant within the town supplies the
  // grid (5,000 t, territorial only); cars burn 20,000 t, trains use power (5,000 MWh = 2,000 t), flights out are 3,000 t of scope 3;
  // a landfill emits 100 t of methane (2,800 t CO2e), the town's waste landfilled elsewhere 50 t (1,400 t), a neighbour's waste
  // treated here 20 t (560 t, territorial only); a cement kiln 4,000 t; land use 10 t of N2O (2,650 t); wood burnt 700 t biogenic CO2.
  I.entries = {
      est("I.1.1", Sector::stationary_energy, 1, "residential", "CO2", 1.0e6, 1.0, 0.05, 0.03),
      est("I.1.2", Sector::stationary_energy, 2, "residential", "CO2", 1.0e5, 400, 0.02, 0.10),
      est("I.4.4", Sector::stationary_energy, 1, "generation", "CO2", 5.0e6, 1.0),
      est("II.1.1", Sector::transportation, 1, "on-road", "CO2", 2.0e7, 1.0, 0.10, 0.05),
      est("II.2.2", Sector::transportation, 2, "railways", "CO2", 5000, 400),
      est("II.4.3", Sector::transportation, 3, "aviation", "CO2", 3.0e6, 1.0),
      est("III.1.1", Sector::waste, 1, "landfill", "CH4", 1.0e5, 1.0, 0.0, 0.30),
      est("III.1.2", Sector::waste, 3, "landfill", "CH4", 5.0e4, 1.0),
      est("III.1.3", Sector::waste, 1, "landfill", "CH4", 2.0e4, 1.0),
      est("IV.1", Sector::ippu, 1, "cement", "CO2", 4.0e6, 1.0),
      est("V.3", Sector::afolu, 1, "soils", "N2O", 1.0e4, 1.0),
      est("I.1.1b", Sector::stationary_energy, 1, "residential", "CO2", 7.0e5, 1.0),
      keyed("II.3.1", Sector::transportation, 1, Key::IE, "II.1.1"),
      keyed("I.3.3", Sector::stationary_energy, 3, Key::NO),
  };
  I.entries[2].grid_supply = true;
  I.entries[8].imported_waste = true;
  I.entries[11].biogenic = true;
  const auto T = totals(I);
  // BASIC: 1,000 + 40,000 + 20,000 + 2,000 + 2,800 + 1,400 = 67,200.
  s.check("BASIC: stationary energy 1 and 2, transportation 1 and 2, waste 1 and 3 = 67,200 t", T.basic - 67200, 1e-9);
  s.check("BASIC+: with flights (3,000), cement (4,000) and soils (2,650) = 76,850 t", T.basic_plus - 76850, 1e-9);
  s.check("the territorial total: every scope 1, the grid's generation and the neighbour's waste with it = 1,000 + 5,000 + 20,000 + 2,800 + 560 + 4,000 + 2,650",
          T.territorial - 36010, 1e-9);
  s.check("biogenic CO2 reported apart (700 t) and in no total", T.biogenic - 700, 1e-9);
  s.check("BASIC+ per resident: 76,850 / 50,000 = 1.537 t", T.per_capita - 1.537, 1e-12);
  s.require("BASIC is complete (NO and IE count as accounted for); BASIC+ too", T.basic_complete && T.basic_plus_complete);

  {
    // Approach 1 within BASIC: 1,000 ± √(0.05² + 0.03²), 40,000 ± √(0.02² + 0.1²), 20,000 ± √(0.1² + 0.05²), 2,800 ± 0.3.
    const double v = std::pow(1000 * std::sqrt(0.0034), 2) + std::pow(40000 * std::sqrt(0.0104), 2) + std::pow(20000 * std::sqrt(0.0125), 2) + std::pow(2800 * 0.3, 2);
    s.check("uncertainty by the IPCC's Approach 1: √(Σ (U_i x_i)²) / Σ x_i within BASIC", T.u_basic - std::sqrt(v) / 67200, 1e-12);
  }
  {
    auto J = I;
    J.entries.erase(J.entries.begin() + 7);  // the town's waste landfilled elsewhere
    J.entries[3].key = Key::NE, J.entries[3].activity = none, J.entries[3].factor = none;
    const auto U = totals(J);
    s.require("a level is incomplete where a cell has no entry (waste, scope 3) or one not estimated (transportation, scope 1), each named",
              !U.basic_complete && U.gaps_basic.size() == 2 && U.gaps_basic[0].find("transportation, scope 1") != std::string::npos &&
                  U.gaps_basic[1].find("waste, scope 3") != std::string::npos);
  }
  {
    auto bad = I;
    bad.entries[6].scope = 2;
    s.require("scope 2 in waste is refused", refused([&] { totals(bad); }));
    auto ie = I;
    ie.entries[12].included_in = "nowhere";
    s.require("an entry included elsewhere, in no entry that exists, is refused", refused([&] { totals(ie); }));
    auto both = I;
    both.entries[13].amount = 5;
    s.require("a notation key beside a figure is refused", refused([&] { totals(both); }));
    auto gas = I;
    gas.potentials = ar6();
    s.require("plain \"CH4\" under AR6 is refused: fossil or not is to be said", refused([&] { totals(gas); }));
  }
  {
    const auto K = key_categories(I, Level::basic);
    // 40,000 (59.5%), 20,000 (29.8%), 2,800 (4.2%) reach 93.5%; then 2,000 (3.0%) reaches 96.4%: four key categories of six, home gas (scope 1) apart from home power (scope 2).
    int key = 0;
    for (const auto& c : K) key += c.key;
    s.require("key categories by Approach 1: grid power, cars, the landfill and the railways reach 95%; the remaining waste and gas are not key",
              K.size() == 6 && key == 4 && K[0].subsector == "residential" && K[0].tonnes == 40000 && K[3].subsector == "railways" && !K[4].key);
  }

  // Mitigation on a one-entry town: 400 t of grid power, an efficiency measure removing 20% of the activity, a cleaner grid halving
  // the factor. Alone 80 and 200; together 400 − 400 · 0.8 · 0.5 = 240; the overlap 40.
  Inventory M;
  M.year = 2025, M.potentials = ar5();
  M.entries = {est("p", Sector::stationary_energy, 2, "residential", "CO2", 1000, 400)};
  std::vector<Measure> ms(2);
  ms[0].id = "efficiency", ms[0].sector = Sector::stationary_energy, ms[0].on = On::activity, ms[0].reduction = 0.2, ms[0].cost = 4000;
  ms[1].id = "grid", ms[1].sector = Sector::stationary_energy, ms[1].on = On::factor, ms[1].reduction = 0.5, ms[1].cost = 6000;
  {
    const auto C = curve(M, ms);
    s.check("the joint abatement 240 t, the overlap 40 t beneath the sum alone", std::fabs(C.joint - 240) + std::fabs(C.overlap - 40), 1e-9);
    // Cost per tonne alone: 50 and 30, so the grid comes first: its marginal 200, then efficiency's 40.
    s.require("the curve orders the grid first (30 a tonne alone), efficiency second (50), and in that order efficiency abates 40 t, not 80",
              C.measures[0].id == "grid" && std::fabs(C.measures[0].in_order - 200) < 1e-9 && std::fabs(C.measures[1].in_order - 40) < 1e-9 &&
                  std::fabs(C.measures[1].cost_per_tonne_in_order - 100) < 1e-9);
    s.check("the Shapley value shares the overlap: efficiency 80 − 20 = 60, the grid 200 − 20 = 180", std::fabs(C.measures[1].shapley - 60) + std::fabs(C.measures[0].shapley - 180), 1e-9);
    const auto D = curve(M, ms, Level::basic_plus, {"efficiency", "grid"});
    s.require("declared in the other order, efficiency is credited 80 and the grid 160; the joint 240 is the same",
              std::fabs(D.measures[0].in_order - 80) < 1e-9 && std::fabs(D.measures[1].in_order - 160) < 1e-9 && std::fabs(D.joint - C.joint) < 1e-9);
  }
  {
    auto N = M;
    N.entries.push_back(est("q", Sector::transportation, 1, "on-road", "CO2", 100, 1000));
    auto two = ms;
    two[1].sector = Sector::transportation;  // now on another entry
    const auto C = curve(N, two);
    s.require("measures on different entries do not overlap: Shapley equals each alone", std::fabs(C.overlap) < 1e-9 && std::fabs(C.measures[0].shapley - C.measures[0].alone) < 1e-9);
  }
  {
    // Activities grow 2% a year; efficiency from 2026 at full effect, the grid from 2027 over two years.
    auto ps = ms;
    ps[0].start = 2026, ps[1].start = 2027, ps[1].ramp = 2;
    Target t;
    t.year = 2028, t.reduction = 0.6, t.budget = 1000;
    const auto P = pathway(M, ps, {{Sector::stationary_energy, 0.02}}, 2028, t);
    const double g1 = 1.02, g2 = 1.02 * 1.02, g3 = g2 * 1.02;
    s.check("2026: grown 2%, efficiency alone at full effect: 400 · 1.02 · 0.8", P.years[0].with_measures - 400 * g1 * 0.8, 1e-9);
    s.check("2027: the grid at half its effect: 400 · 1.02² · 0.8 · 0.75", P.years[1].with_measures - 400 * g2 * 0.8 * 0.75, 1e-9);
    s.check("2028: both at full effect: 400 · 1.02³ · 0.4, against a target of 400 · 0.4 = 160", std::fabs(P.years[2].with_measures - 400 * g3 * 0.4) + std::fabs(P.target_level - 160), 1e-9);
    s.require("the target missed by 400 · 1.02³ · 0.4 − 160 = 9.8 t, growth eating what the measures saved; the cumulative 746 t within a budget of 1,000",
              !P.meets_target && std::fabs(P.gap - (400 * g3 * 0.4 - 160)) < 1e-9 && P.within_budget);
    s.check("the cost of 2027: efficiency in full and half the grid's", P.years[1].cost - (4000 + 3000), 1e-9);
  }
  {
    // A cleaner grid on every sector's scope 2: the town's homes (40,000 t) and railways (2,000 t), not its cars.
    Measure g;
    g.id = "grid", g.any_sector = true, g.scope = 2, g.on = On::factor, g.reduction = 0.5;
    const auto C = curve(I, {g}, Level::basic);
    s.check("a measure on every sector's scope 2 halves homes' power and the railways': 21,000 t", C.joint - 21000, 1e-9);
  }
  {
    // Proposition 3: on the town, four measures that overlap; each one's Shapley value lies between its marginal when added last
    // (read from a curve declared with it last) and its abatement alone, and the values sum to the joint abatement.
    std::vector<Measure> four(4);
    four[0].id = "homes", four[0].sector = Sector::stationary_energy, four[0].subsector = "residential", four[0].on = On::activity, four[0].reduction = 0.2;
    four[1].id = "grid", four[1].any_sector = true, four[1].scope = 2, four[1].on = On::factor, four[1].reduction = 0.5;
    four[2].id = "gas", four[2].sector = Sector::stationary_energy, four[2].scope = 1, four[2].on = On::factor, four[2].reduction = 0.3;
    four[3].id = "cars", four[3].sector = Sector::transportation, four[3].on = On::activity, four[3].reduction = 0.1;
    const auto C = curve(I, four, Level::basic_plus);
    bool within = true;
    double sum = 0;
    for (const auto& a : C.measures) {
      std::vector<std::string> order;
      for (const auto& m : four)
        if (m.id != a.id) order.push_back(m.id);
      order.push_back(a.id);
      const auto last = curve(I, four, Level::basic_plus, order);
      const double marginal_last = last.measures.back().in_order;
      within = within && marginal_last <= a.shapley + 1e-9 && a.shapley <= a.alone + 1e-9;
      sum += a.shapley;
    }
    s.require("every measure's Shapley value lies between its marginal last and its abatement alone (Proposition 3)", within);
    s.check("... and the Shapley values sum to the joint abatement", sum - C.joint, 1e-9);
  }
  {
    Measure m;
    m.id = "x", m.reduction = 1.5;
    s.require("a reduction above one is refused", refused([&] { curve(M, {m}); }));
  }
  return s.done();
}
