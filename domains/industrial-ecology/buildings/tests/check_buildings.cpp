// Checks of industrial_ecology::buildings: a scheme's rows matched by specificity; a building whose
// structure is unknown given the envelope of the structures its use admits; a year unknown, matched in
// any period; a building no row matches reported and not counted; stocks by group summing to the whole;
// cohorts by age, whose intensities, carried into a dynamic stock model as its initial stock, give back
// the buildings' own material stock.

#include <industrial_ecology/buildings/buildings.hpp>
#include <industrial_ecology/stocks/stocks.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace industrial_ecology;

int main() {
  calx::verification::Suite s;
  buildings::Scheme sch;
  sch.name = "a test scheme";
  sch.materials = {"concrete", "steel", "timber"};
  // kg per m²: lower, central, upper
  sch.rows = {
      {"", "residential", "concrete", INT_MIN, 1979, {700, 30, 5}, {900, 45, 10}, {1200, 70, 20}, "test"},
      {"", "residential", "concrete", 1980, INT_MAX, {800, 40, 5}, {1000, 60, 10}, {1300, 90, 20}, "test"},
      {"", "residential", "timber", INT_MIN, INT_MAX, {100, 5, 60}, {150, 10, 90}, {250, 20, 130}, "test"},
      {"", "office", "", INT_MIN, INT_MAX, {900, 60, 2}, {1100, 80, 5}, {1500, 120, 10}, "test"},
  };
  using buildings::Building;
  const std::vector<Building> bs{
      {"a", "AU", "residential", "concrete", 100, 1990},
      {"b", "AU", "residential", "", 200, 1960},
      {"c", "AU", "office", "steel", 1000, buildings::unknown_year},
      {"d", "AU", "retail", "", 300, 2000},
  };
  const auto Ia = buildings::intensity(sch, bs[0]);
  s.require("a known building takes its one row (concrete, built 1990: the period from 1980)", Ia.matched && Ia.rows.size() == 1 && Ia.rows[0] == 1);
  const auto Ib = buildings::intensity(sch, bs[1]);
  s.require("structure unknown: the envelope of the residential concrete (to 1979) and timber rows", Ib.matched && Ib.rows.size() == 2);
  s.check("structure unknown: lower the least, upper the greatest, central the mean",
          std::fabs(Ib.lower[0] - 100) + std::fabs(Ib.upper[0] - 1200) + std::fabs(Ib.central[0] - 525) + std::fabs(Ib.central[2] - 50), 1e-12);
  const auto Ic = buildings::intensity(sch, bs[2]);
  s.require("an office of any structure and an unknown year matches the office row", Ic.matched && Ic.rows.size() == 1 && Ic.rows[0] == 3);
  s.require("a retail building no row matches is not matched", !buildings::intensity(sch, bs[3]).matched);

  const auto st = buildings::stock(sch, bs, {"housing", "housing", "work", "work"});
  s.require("stock: the unmatched building is reported, with its area", st.unmatched.size() == 1 && st.unmatched[0] == "d" && st.unmatched_area == 300);
  s.check("stock: concrete, central: 100·1000 + 200·525 + 1000·1100", std::fabs(st.total.central[0] - (100000 + 105000 + 1100000)), 1e-9);
  s.check("stock: the groups sum to the whole", std::fabs(st.by_group.at("housing").central[1] + st.by_group.at("work").central[1] - st.total.central[1]) +
                                                   std::fabs(st.by_group.at("housing").floor_area + st.by_group.at("work").floor_area - st.total.floor_area), 1e-9);
  s.require("stock: bounds in order for every material", st.total.lower[0] <= st.total.central[0] && st.total.central[2] <= st.total.upper[2]);

  // Cohorts at the end of 2020, carried into a dynamic stock model as its initial stock.
  const std::vector<Building> dated{bs[0], bs[1]};
  const auto co = buildings::cohorts(sch, dated, 2020, 80);
  s.check("cohorts: floor area by age (30 and 60 years)", std::fabs(co.floor_area[30] - 100) + std::fabs(co.floor_area[60] - 200), 1e-12);
  stocks::Initial init{co.floor_area, stocks::weibull(3, 70)};
  const auto M = stocks::inflow_driven(std::vector<double>(5, 0.0), {stocks::weibull(3, 70)}, &init);
  stocks::Intensity in;
  in.materials = sch.materials;
  in.lower = {{0, 0, 0}}, in.central = {{0, 0, 0}}, in.upper = {{0, 0, 0}};
  in.initial_lower = co.lower, in.initial_central = co.central, in.initial_upper = co.upper;
  const auto mat = stocks::materials(M, in);
  // At t = 0 the stock is what survived year 0; its mass plus what left in year 0 is the buildings' own.
  const auto own = buildings::stock(sch, dated);
  double worst = 0;
  for (std::size_t k = 0; k < 3; k++) worst = std::max(worst, std::fabs(mat[k].stock.central[0] + mat[k].outflow.central[0] - own.total.central[k]) / own.total.central[k]);
  s.check("cohorts into a dynamic model: stock + outflow of the first year is the buildings' own material stock", worst, 1e-13);

  return s.done();
}
