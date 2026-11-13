// Physical accounts of an economy: its supply and use of products in mass, what it takes from the
// environment and returns to it, and the economy-wide indicators of material use (theory note §6).
//
// A physical supply–use table (SEEA Central Framework 2012, §3.2–3.3, Tables 3.1–3.2) records, for
// products p and activities a (industries, households):
//
//   supply(p, a)             what a produces of p          use(p, a)                 what a uses of p
//   imports(p), exports(p)   with the rest of the world    accumulation(p)           products added to stocks (capital formation)
//   natural(i, a)            natural inputs a takes        residuals(i, a)           residuals a returns to the environment
//   stock(a)                 net additions to a's own stocks of materials (buildings, landfills)
//
// and three identities hold of a consistent table (SEEA-CF §3.71–3.79):
//
//   for each product:   Σ_a supply + imports = Σ_a use + exports + accumulation
//   for each activity:  Σ_p use + Σ_i natural = Σ_p supply + Σ_i residuals + stock
//   for the economy:    natural inputs + imports = residuals + exports + net additions to stock
//
// `balances` reports every departure. Economy-wide material flow accounts (Eurostat 2018, Economy-wide
// Material Flow Accounts Handbook, §2–3) summarise the boundary flows by material: domestic extraction
// DE, imports IMP, exports EXP; DMI = DE + IMP, DMC = DMI − EXP, PTB = IMP − EXP; with domestic processed
// output DPO and the balancing items (oxygen taken in combustion, water vapour given off) the net
// addition to stock NAS = DMI + BI − DPO − BO − EXP (Handbook eq. 3.1); with the raw material
// equivalents of trade RME_IMP, RME_EXP, the raw material consumption RMC = DE + RME_IMP − RME_EXP.

#pragma once

#include <calx/linalg/rectangular.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::accounts {

using calx::linalg::Rectangular;

struct SupplyUse {
  std::vector<std::string> products, activities, natural_inputs, residual_kinds;
  Rectangular supply, use;              // products × activities
  std::vector<double> imports, exports, accumulation;  // per product
  Rectangular natural, residuals;       // natural inputs × activities, residual kinds × activities
  std::vector<double> stock;            // per activity
};

struct Balances {
  std::vector<double> by_product, by_activity;  // supply − use, inputs − outputs
  double economy = 0;                           // natural inputs + imports − residuals − exports − additions to stock
  double worst = 0;                             // the largest |·| of all
  double natural_total = 0, residual_total = 0, imports_total = 0, exports_total = 0, additions = 0;
};

inline Balances balances(const SupplyUse& t) {
  const std::size_t P = t.products.size(), A = t.activities.size();
  if (t.supply.rows != P || t.supply.cols != A || t.use.rows != P || t.use.cols != A || t.imports.size() != P || t.exports.size() != P ||
      t.accumulation.size() != P || t.natural.cols != A || t.residuals.cols != A || t.stock.size() != A || t.natural.rows != t.natural_inputs.size() ||
      t.residuals.rows != t.residual_kinds.size())
    throw std::invalid_argument("industrial_ecology::accounts::balances: the tables' shapes do not agree with their labels");
  Balances b;
  b.by_product.assign(P, 0.0), b.by_activity.assign(A, 0.0);
  for (std::size_t p = 0; p < P; p++) {
    double v = t.imports[p] - t.exports[p] - t.accumulation[p];
    for (std::size_t a = 0; a < A; a++) v += t.supply(p, a) - t.use(p, a);
    b.by_product[p] = v;
    b.imports_total += t.imports[p], b.exports_total += t.exports[p], b.additions += t.accumulation[p];
  }
  for (std::size_t a = 0; a < A; a++) {
    double v = -t.stock[a];
    for (std::size_t p = 0; p < P; p++) v += t.use(p, a) - t.supply(p, a);
    for (std::size_t i = 0; i < t.natural.rows; i++) v += t.natural(i, a), b.natural_total += t.natural(i, a);
    for (std::size_t i = 0; i < t.residuals.rows; i++) v -= t.residuals(i, a), b.residual_total += t.residuals(i, a);
    b.by_activity[a] = v;
    b.additions += t.stock[a];
  }
  b.economy = b.natural_total + b.imports_total - b.residual_total - b.exports_total - b.additions;
  for (double v : b.by_product) b.worst = std::max(b.worst, std::fabs(v));
  for (double v : b.by_activity) b.worst = std::max(b.worst, std::fabs(v));
  b.worst = std::max(b.worst, std::fabs(b.economy));
  return b;
}

// ---- economy-wide material flow accounts ----

struct EconomyWide {
  std::vector<std::string> materials;                 // e.g. biomass, metal ores, non-metallic minerals, fossil energy materials
  std::vector<double> extraction, imports, exports;   // DE, IMP, EXP by material (tonnes)
  std::vector<double> rme_imports, rme_exports;       // raw material equivalents of trade, by material (optional)
  double processed_output = std::numeric_limits<double>::quiet_NaN();  // DPO, total (optional)
  double balancing_input = 0, balancing_output = 0;   // BI, BO
};

struct Indicators {
  std::vector<double> dmi, dmc, ptb, rmc, rtb;        // by material (rmc, rtb when RMEs are given)
  double DE = 0, IMP = 0, EXP = 0, DMI = 0, DMC = 0, PTB = 0;
  double RMC = std::numeric_limits<double>::quiet_NaN(), RTB = std::numeric_limits<double>::quiet_NaN();
  double NAS = std::numeric_limits<double>::quiet_NaN();  // when DPO is given
};

inline Indicators indicators(const EconomyWide& e) {
  const std::size_t K = e.materials.size();
  if (e.extraction.size() != K || e.imports.size() != K || e.exports.size() != K) throw std::invalid_argument("industrial_ecology::accounts::indicators: DE, IMP and EXP for every material");
  const bool rme = !e.rme_imports.empty();
  if (rme && (e.rme_imports.size() != K || e.rme_exports.size() != K)) throw std::invalid_argument("industrial_ecology::accounts::indicators: raw material equivalents for every material, or none");
  Indicators I;
  if (rme) I.RMC = 0, I.RTB = 0;
  for (std::size_t k = 0; k < K; k++) {
    I.dmi.push_back(e.extraction[k] + e.imports[k]);
    I.dmc.push_back(e.extraction[k] + e.imports[k] - e.exports[k]);
    I.ptb.push_back(e.imports[k] - e.exports[k]);
    I.DE += e.extraction[k], I.IMP += e.imports[k], I.EXP += e.exports[k];
    if (rme) {
      I.rmc.push_back(e.extraction[k] + e.rme_imports[k] - e.rme_exports[k]);
      I.rtb.push_back(e.rme_imports[k] - e.rme_exports[k]);
      I.RMC += I.rmc.back(), I.RTB += I.rtb.back();
    }
  }
  I.DMI = I.DE + I.IMP, I.DMC = I.DMI - I.EXP, I.PTB = I.IMP - I.EXP;
  if (!std::isnan(e.processed_output)) I.NAS = I.DMI + e.balancing_input - e.processed_output - e.balancing_output - I.EXP;
  return I;
}

}  // namespace industrial_ecology::accounts
