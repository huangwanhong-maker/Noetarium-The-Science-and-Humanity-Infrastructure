// Checks of industrial_ecology::accounts: a physical supply–use table built to balance (two products, two
// industries and households) balances in every product, every activity and the economy; one entry
// changed shows at its product, its activity, and not in the economy's identity, which the other two
// imply; the economy-wide indicators by hand.

#include <industrial_ecology/accounts/accounts.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace industrial_ecology::accounts;
using calx::linalg::Rectangular;

int main() {
  calx::verification::Suite s;

  // Products: grain, flour. Activities: farming, milling, households. Natural inputs: biomass; residuals: emissions, waste.
  SupplyUse t;
  t.products = {"grain", "flour"};
  t.activities = {"farming", "milling", "households"};
  t.natural_inputs = {"biomass harvested"};
  t.residual_kinds = {"to air", "solid waste"};
  t.supply = Rectangular::from_rows({{100, 0, 0}, {0, 70, 0}});
  t.use = Rectangular::from_rows({{10, 80, 5}, {0, 0, 60}});
  t.imports = {5, 0};
  t.exports = {10, 8};
  t.accumulation = {0, 2};  // flour added to stocks
  t.natural = Rectangular::from_rows({{95, 0, 0}});
  t.residuals = Rectangular::from_rows({{3, 5, 50}, {2, 5, 15}});
  t.stock = {0, 0, 0};
  // Farming: 10 + 95 in, 100 + 5 out. Milling: 80 in, 70 + 10 out. Households: 65 in, 65 out.
  const auto b = balances(t);
  s.check("a balanced table: every product, every activity and the economy balance", b.worst, 1e-12);
  s.check("the economy: 95 + 5 natural and imported = 80 residuals + 18 exported + 2 accumulated", std::fabs(b.natural_total + b.imports_total - 100) + std::fabs(b.residual_total - 80), 1e-12);
  SupplyUse u = t;
  u.use(0, 1) = 85;  // milling uses 5 more grain than is supplied
  const auto c = balances(u);
  s.check("one entry changed: grain is short by 5", std::fabs(c.by_product[0] + 5), 1e-12);
  s.check("one entry changed: milling takes 5 more than it gives", std::fabs(c.by_activity[1] - 5), 1e-12);
  s.check("one entry changed: the economy's identity, implied by the other two, still holds", std::fabs(c.economy), 1e-12);

  EconomyWide e;
  e.materials = {"biomass", "metal ores", "non-metallic minerals", "fossil energy materials"};
  e.extraction = {120, 10, 400, 60};
  e.imports = {30, 50, 20, 90};
  e.exports = {40, 5, 10, 20};
  e.rme_imports = {45, 300, 30, 110};
  e.rme_exports = {50, 20, 15, 25};
  e.processed_output = 210;
  e.balancing_input = 80, e.balancing_output = 30;
  const auto I = indicators(e);
  s.check("DMI = DE + IMP = 590 + 190", std::fabs(I.DMI - 780), 1e-12);
  s.check("DMC = DMI − EXP = 705; PTB = IMP − EXP = 115", std::fabs(I.DMC - 705) + std::fabs(I.PTB - 115), 1e-12);
  s.check("NAS = DMI + BI − DPO − BO − EXP = 780 + 80 − 210 − 30 − 75 = 545", std::fabs(I.NAS - 545), 1e-12);
  s.check("RMC = DE + RME_IMP − RME_EXP = 590 + 485 − 110 = 965; metal ores alone 10 + 300 − 20", std::fabs(I.RMC - 965) + std::fabs(I.rmc[1] - 290), 1e-12);

  return s.done();
}
