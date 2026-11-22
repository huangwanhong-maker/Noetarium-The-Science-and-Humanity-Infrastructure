// Checks of industrial_ecology::waste's treatment, each against the IPCC's own numbers or worked by hand: the Annex's Table 3A1.1 (100
// units a year at k = 0.1 decompose 9.5 then 18.1); nothing decays in the year of deposition at the default delay, and a share does
// without one; recovery subtracted before oxidation; inert components emit nothing; composting and digestion by Table 4.1; open burning's
// fossil and biogenic CO2, CH4 and N2O; the 2019 Refinement's DOCf and oxidation; the inventory's entries by origin, scope and the BASIC
// rule, methane as non-fossil under AR6; refusals.

#include <industrial_ecology/waste/treatment.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace industrial_ecology::waste;
namespace em = industrial_ecology::emissions;

namespace {
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
  {
    // Table 3A1.1 (p. 3.35): 100 units of DDOCm a year, k = 0.1 (garden waste, temperate wet), the default delay.
    Defaults d = ipcc2006();
    d.components["garden"] = {0.4, 1.0, 0.49, 0.0, 1.0, "garden"};
    Site st{"annex", "managed anaerobic", false, {{"garden", 1.0}}, {}, {}};
    for (int y = 0; y <= 2; y++) st.deposited[y] = 100;
    const auto r = first_order_decay(st, d, Climate::temperate_wet, 2);
    const double unit = d.F * 16.0 / 12.0;
    s.check("Table 3A1.1: nothing decomposes in the year of deposition at the default delay (M = 13)", r[0].generated, 1e-15);
    s.check("... 100(1 − e^{−0.1}) = 9.5 decomposes in year 1, 190.5 (1 − e^{−0.1}) = 18.1 in year 2",
            std::fabs(r[1].generated / unit - 100 * (1 - std::exp(-0.1))) + std::fabs(r[2].generated / unit - (100 + 100 * std::exp(-0.1)) * (1 - std::exp(-0.1))), 1e-10);
    s.check("... as the table rounds them, 9.5 and 18.1", std::fabs(std::round(r[1].generated / unit * 10) / 10 - 9.5) + std::fabs(std::round(r[2].generated / unit * 10) / 10 - 18.1), 1e-12);
    Defaults now = d;
    now.M = 7;  // no delay
    const auto n = first_order_decay(st, now, Climate::temperate_wet, 0);
    s.check("with no delay (M = 7) a deposit loses 1 − e^{−k/2} in its own year", std::fabs(n[0].generated / unit - 100 * (1 - std::exp(-0.05))), 1e-10);
  }
  {
    // A managed covered site: (generated − recovered)(1 − 0.1); plastics and glass emit nothing.
    const Defaults d = ipcc2006();
    Site st{"covered", "managed anaerobic", true, {{"food", 0.5}, {"plastics", 0.3}, {"glass", 0.2}}, {{2020, 1000}}, {{2021, 2.0}}};
    const auto r = first_order_decay(st, d, Climate::tropical_wet, 2021);
    // food: 1000 · 0.5 · 0.15 · 0.5 · 1.0 = 37.5 t DDOCm; year 2021 decomposes 37.5 (1 − e^{−0.4}); CH4 = that · 0.5 · 16/12.
    const double g = 37.5 * (1 - std::exp(-0.4)) * 0.5 * 16.0 / 12.0;
    s.check("food alone generates 37.5 (1 − e^{−0.4}) · 0.5 · 16/12 t CH4; plastics and glass nothing",
            std::fabs(r[1].generated - g) + r[1].by_component.count("plastics") + r[1].by_component.count("glass"), 1e-12);
    s.check("... recovery of 2 t subtracted before oxidation of 10%: (g − 2) · 0.9", std::fabs(r[1].emitted - (g - 2) * 0.9), 1e-12);
    Site wrong = st;
    wrong.type = "a pit";
    s.require("a site of a type the edition does not name is refused", refused([&] { first_order_decay(wrong, d, Climate::tropical_wet, 2021); }));
    Site over = st;
    over.recovered[2021] = 100;
    s.require("recovery above what a site generates is refused", refused([&] { first_order_decay(over, d, Climate::tropical_wet, 2021); }));
    Site half = st;
    half.composition = {{"food", 0.5}};
    s.require("a composition whose shares do not sum to one is refused", refused([&] { first_order_decay(half, d, Climate::tropical_wet, 2021); }));
  }
  {
    const Defaults d = ipcc2006();
    const auto c = composting(1000, d);
    s.check("composting 1,000 t wet: 4 t CH4 and 0.24 t N2O (Table 4.1)", std::fabs(c.ch4 - 4) + std::fabs(c.n2o - 0.24), 1e-12);
    const auto a = digestion(1000, d);
    s.check("digestion: 0.8 t CH4, N2O negligible", std::fabs(a.ch4 - 0.8) + a.n2o, 1e-12);
    const auto p = open_burning(1, {{"plastics", 1.0}}, d);
    s.check("a tonne of plastics burnt in the open: 1 · 0.75 · 1 · 0.58 · 44/12 t fossil CO2, no biogenic",
            std::fabs(p.co2_fossil - 0.75 * 0.58 * 44.0 / 12.0) + p.co2_biogenic, 1e-12);
    s.check("... 6.5 kg CH4 and 0.15 kg N2O (a tonne of dry matter)", std::fabs(p.ch4 - 0.0065) + std::fabs(p.n2o - 0.00015), 1e-15);
    const auto f = open_burning(1, {{"food", 1.0}}, d);
    s.check("a tonne of food burnt: biogenic CO2 0.4 · 0.38 · 0.58 · 44/12, N2O on its 0.4 t of dry matter",
            std::fabs(f.co2_biogenic - 0.4 * 0.38 * 0.58 * 44.0 / 12.0) + f.co2_fossil + std::fabs(f.n2o - 0.4 * 0.00015), 1e-12);
    const auto p19 = open_burning(1, {{"plastics", 1.0}}, ipcc2019());
    s.check("the 2019 Refinement burns 71% of the carbon, not 58%", std::fabs(p19.co2_fossil / p.co2_fossil - 0.71 / 0.58), 1e-12);
  }
  {
    // 2019's DOCf for food, 0.7 against 0.5: the methane in proportion.
    Site st{"s", "managed anaerobic", false, {{"food", 1.0}}, {{2020, 100}}, {}};
    const auto a = first_order_decay(st, ipcc2006(), Climate::temperate_dry, 2021), b = first_order_decay(st, ipcc2019(), Climate::temperate_dry, 2021);
    s.check("the 2019 Refinement's DOCf of 0.7 for food raises its methane by 0.7/0.5", std::fabs(b[1].generated / a[1].generated - 1.4), 1e-12);
  }
  {
    // Into the inventory: the city's landfill inside (III.1.1, scope 1), its compost outside (III.2.2, scope 3), a neighbour's waste at
    // the landfill (III.1.3, scope 1, outside BASIC).
    const auto P = em::ar6();
    const std::vector<Treated> ts = {{"landfill", "disposal", Origin::city_inside, {100, 0, 0, 0}},
                                     {"compost", "composting", Origin::city_outside, {4, 0.24, 0, 0}},
                                     {"landfill", "disposal", Origin::imported, {20, 0, 0, 0}},
                                     {"burning", "open burning", Origin::city_inside, {1, 0.1, 50, 30}}};
    const auto E = inventory_entries(ts, P);
    s.require("entries by GPC's codes: III.1.1 scope 1, III.2.2 scope 3, III.1.3 brought in, methane non-fossil under AR6",
              E[0].id.rfind("III.1.1", 0) == 0 && E[0].scope == 1 && E[0].gas == "CH4 non-fossil" && E[1].id.rfind("III.2.2", 0) == 0 && E[1].scope == 3 &&
                  E[3].id.rfind("III.1.3", 0) == 0 && E[3].imported_waste);
    em::Inventory I;
    I.name = "waste";
    I.year = 2024;
    I.potentials = P;
    I.entries = E;
    const auto T = em::totals(I);
    const double basic = (100 * 27.0 + 4 * 27.0 + 0.24 * 273 + 1 * 27.0 + 0.1 * 273 + 50);
    s.check("BASIC counts the city's waste inside and out, not the neighbour's: 27 · (100 + 4 + 1) + 273 · 0.34 + 50 t CO2e", std::fabs(T.basic - basic), 1e-9);
    s.check("... the territorial total counts what is treated inside, the neighbour's included, not what went out", std::fabs(T.territorial - (basic - 4 * 27.0 - 0.24 * 273 + 20 * 27.0)), 1e-9);
    s.check("... and the 30 t of biogenic CO2 apart", std::fabs(T.biogenic - 30), 1e-9);
    s.require("a treatment of an unknown kind is refused", refused([&] { inventory_entries({{"x", "pyrolysis", Origin::city_inside, {1, 0, 0, 0}}}, P); }));
  }
  return s.done();
}
