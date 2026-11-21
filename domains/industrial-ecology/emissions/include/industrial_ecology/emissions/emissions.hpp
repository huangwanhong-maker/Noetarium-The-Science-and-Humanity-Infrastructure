// A community's greenhouse gas inventory by sector and scope, and its mitigation (ADR 0147).
//
// The inventory follows the structure of the Global Protocol for Community-Scale Greenhouse Gas Emission Inventories (GPC; WRI,
// C40 and ICLEI 2014): six sectors (stationary energy, transportation, waste, industrial processes and product use, agriculture,
// forestry and other land use, other scope 3), three scopes by the community's boundary (1, emitted within it; 2, from grid-supplied
// energy consumed within it; 3, emitted outside it as a result of activities within it), and notation keys where a figure is not
// given (NO not occurring, IE included elsewhere, NE not estimated, C confidential). Two reporting levels are sums over cells
// (sector, scope):
//   BASIC   stationary energy 1 and 2, transportation 1 and 2, waste 1 and 3;
//   BASIC+  BASIC with stationary energy 3 (transmission and distribution losses), transportation 3 (transboundary journeys),
//           industrial processes 1, and agriculture, forestry and land use 1;
// and the territorial total is every scope 1 emission. Scope 1 emissions from energy generated within the boundary and supplied to
// the grid, and from waste generated outside and treated within, are in the territorial total and kept out of BASIC and BASIC+, since
// they are other communities' scope 2 or scope 3. Biogenic CO2 is reported apart and in no total.
//
// An entry's emissions are activity × factor (kg of a gas per unit of activity) or a stated amount, converted to CO2 equivalents by a
// declared set of global warming potentials. Uncertainty follows the IPCC's Approach 1 (2006 Guidelines, vol. 1, ch. 3, eqs 3.1 and
// 3.2): an entry's relative half-width U = √(U_A² + U_F²), and a total's √(Σ (U_i x_i)²) / |Σ x_i|, independence assumed (reliable for
// U below about 0.6). Key categories by the IPCC's Approach 1 level assessment (vol. 1, ch. 4): categories in descending order of
// magnitude until their sum reaches 95% of the total.
//
// Mitigation. A measure scales by (1 − r) what remains of an activity or of a factor in the entries it acts on. Measures commute, so a
// set's joint abatement does not depend on the order in which they are applied; but a measure's own abatement does, since two
// measures on one entry overlap (a fall in activity leaves less for a cleaner factor to save). Abatement cost curves that credit
// each measure with its abatement alone count the overlap twice (Kesicki and Ekins 2012, Climate Policy 12: 219–236). The module
// reports each measure alone, its marginal abatement in a declared order (the curve's), its Shapley value over every order (nomos),
// and the joint abatement, so that the overlap is seen.
//
// Pathway. Activities grow at declared rates by sector, measures ramp to full effect over declared years, and the years ahead are
// compared with a declared target (a reduction from the base year by a target year) and a declared cumulative budget.
//
// Every factor, potential, rate, cost and target is the caller's declaration; nothing here is any city's inventory or commitment.
#pragma once

#include <nomos/games/claims.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace industrial_ecology::emissions {

inline constexpr double none = std::numeric_limits<double>::quiet_NaN();

enum class Sector { stationary_energy, transportation, waste, ippu, afolu, other };
enum class Key { estimated, NO, IE, NE, C };
enum class Level { basic, basic_plus, territorial };

inline const char* name(Sector s) {
  switch (s) {
    case Sector::stationary_energy: return "stationary energy";
    case Sector::transportation: return "transportation";
    case Sector::waste: return "waste";
    case Sector::ippu: return "industrial processes and product use";
    case Sector::afolu: return "agriculture, forestry and other land use";
    default: return "other scope 3";
  }
}
inline const char* code(Key k) {
  switch (k) {
    case Key::estimated: return "";
    case Key::NO: return "NO";
    case Key::IE: return "IE";
    case Key::NE: return "NE";
    default: return "C";
  }
}

// Global warming potentials over 100 years, kg CO2e per kg of gas: a declared set with its source.
struct Potentials {
  std::string name, source;
  std::map<std::string, double> gwp;
};
inline Potentials ar5() {
  return {"AR5 GWP100", "IPCC AR5 WG1 (2013), Table 8.7, without climate–carbon feedbacks", {{"CO2", 1}, {"CH4", 28}, {"N2O", 265}, {"SF6", 23500}, {"HFC-134a", 1300}, {"CO2e", 1}}};
}
inline Potentials ar6() {
  // AR6 distinguishes fossil methane from non-fossil, so a caller names which; there is no plain "CH4" in it.
  return {"AR6 GWP100", "IPCC AR6 WG1 (2021), Table 7.15 and 7.SM.7", {{"CO2", 1}, {"CH4 fossil", 29.8}, {"CH4 non-fossil", 27.0}, {"N2O", 273}, {"SF6", 24300}, {"HFC-134a", 1530}, {"CO2e", 1}}};
}

struct Entry {
  std::string id;               // the caller's code (GPC's I.1.1 or any other)
  Sector sector = Sector::stationary_energy;
  int scope = 1;
  std::string subsector;        // a label; with the gas it names a category
  std::string gas = "CO2";
  double activity = none;       // in `unit`
  std::string unit;
  double factor = none;         // kg of the gas per unit of activity
  double amount = none;         // kg of the gas, when stated instead of activity × factor
  double u_activity = 0, u_factor = 0;  // relative half-widths of 95% intervals
  Key key = Key::estimated;
  std::string included_in;      // for IE: the entry that holds it
  bool grid_supply = false;     // generation within the boundary supplied to the grid
  bool imported_waste = false;  // waste generated outside, treated within
  bool biogenic = false;        // biogenic CO2, reported apart
};

struct Inventory {
  std::string name, boundary;
  int year = 0;
  double population = none, gdp = none;
  Potentials potentials;
  std::vector<Entry> entries;
};

// kg of the gas an estimated entry states or computes.
inline double gas_kg(const Entry& e) { return std::isfinite(e.amount) ? e.amount : e.activity * e.factor; }

inline void validate(const Inventory& I) {
  std::set<std::string> ids;
  for (const auto& e : I.entries) {
    if (e.id.empty() || !ids.insert(e.id).second) throw std::invalid_argument("emissions: every entry has an id of its own (\"" + e.id + "\")");
    if (e.scope < 1 || e.scope > 3) throw std::invalid_argument("emissions: entry " + e.id + ": the scope is 1, 2 or 3");
    if (e.scope == 2 && e.sector != Sector::stationary_energy && e.sector != Sector::transportation)
      throw std::invalid_argument("emissions: entry " + e.id + ": scope 2 is grid-supplied energy, in stationary energy or transportation only");
    if (e.grid_supply && !(e.sector == Sector::stationary_energy && e.scope == 1)) throw std::invalid_argument("emissions: entry " + e.id + ": generation supplied to the grid is stationary energy, scope 1");
    if (e.imported_waste && !(e.sector == Sector::waste && e.scope == 1)) throw std::invalid_argument("emissions: entry " + e.id + ": waste from outside treated within is waste, scope 1");
    if (e.biogenic && e.gas != "CO2") throw std::invalid_argument("emissions: entry " + e.id + ": only CO2 is reported as biogenic");
    const bool given = std::isfinite(e.amount) || (std::isfinite(e.activity) && std::isfinite(e.factor));
    if (e.key == Key::estimated) {
      if (!given) throw std::invalid_argument("emissions: entry " + e.id + ": an estimate states an amount, or an activity and a factor");
      if (!I.potentials.gwp.count(e.gas)) throw std::invalid_argument("emissions: entry " + e.id + ": the gas " + e.gas + " has no potential in " + I.potentials.name);
      if (e.u_activity < 0 || e.u_factor < 0) throw std::invalid_argument("emissions: entry " + e.id + ": uncertainties are not negative");
    } else if (given) {
      throw std::invalid_argument("emissions: entry " + e.id + ": a notation key (" + code(e.key) + ") stands instead of a figure, not beside one");
    }
  }
  for (const auto& e : I.entries)
    if (e.key == Key::IE && !ids.count(e.included_in)) throw std::invalid_argument("emissions: entry " + e.id + ": included elsewhere, but in no entry named \"" + e.included_in + "\"");
}

// Tonnes CO2e of an estimated entry (zero for a notation key).
inline double tonnes(const Inventory& I, const Entry& e) {
  if (e.key != Key::estimated) return 0;
  return gas_kg(e) * I.potentials.gwp.at(e.gas) / 1000.0;
}

inline bool in_level(const Entry& e, Level L) {
  if (e.biogenic) return false;
  if (L == Level::territorial) return e.scope == 1;
  if (e.grid_supply || e.imported_waste) return false;
  const auto s = e.sector;
  const bool basic = ((s == Sector::stationary_energy || s == Sector::transportation) && (e.scope == 1 || e.scope == 2)) ||
                     (s == Sector::waste && (e.scope == 1 || e.scope == 3));
  if (L == Level::basic) return basic;
  return basic || ((s == Sector::stationary_energy || s == Sector::transportation) && e.scope == 3) || ((s == Sector::ippu || s == Sector::afolu) && e.scope == 1);
}

// The cells a level asks to be accounted for.
inline std::vector<std::pair<Sector, int>> cells(Level L) {
  std::vector<std::pair<Sector, int>> c = {{Sector::stationary_energy, 1}, {Sector::stationary_energy, 2}, {Sector::transportation, 1}, {Sector::transportation, 2}, {Sector::waste, 1}, {Sector::waste, 3}};
  if (L == Level::basic_plus) c.insert(c.end(), {{Sector::stationary_energy, 3}, {Sector::transportation, 3}, {Sector::ippu, 1}, {Sector::afolu, 1}});
  return c;
}

struct Cell {
  Sector sector;
  int scope;
  double tonnes = 0;
  int estimated = 0, keyed = 0;
  bool missing = false, not_estimated = false;
};

struct Totals {
  double basic = 0, basic_plus = 0, territorial = 0, biogenic = 0;
  double u_basic = 0, u_basic_plus = 0, u_territorial = 0;   // relative half-widths, IPCC Approach 1
  std::vector<Cell> cells;                                    // every (sector, scope) with an entry, and every cell a level asks for
  std::map<std::string, double> by_gas;                       // tonnes CO2e within BASIC+
  bool basic_complete = false, basic_plus_complete = false;
  std::vector<std::string> gaps_basic, gaps_basic_plus;       // the cells that leave a level incomplete, and why
  double per_capita = none, per_gdp = none;                   // BASIC+ over population and GDP
};

inline double entry_u(const Entry& e) {
  if (std::isfinite(e.amount)) return e.u_activity;          // a stated amount carries its own uncertainty
  return std::sqrt(e.u_activity * e.u_activity + e.u_factor * e.u_factor);
}

inline Totals totals(const Inventory& I) {
  validate(I);
  Totals T;
  double var[3] = {0, 0, 0};
  std::map<std::pair<int, int>, Cell> cs;
  for (const auto& e : I.entries) {
    auto& c = cs[{static_cast<int>(e.sector), e.scope}];
    c.sector = e.sector, c.scope = e.scope;
    if (e.key == Key::estimated) c.estimated++;
    else c.keyed++;
    if (e.key == Key::NE) c.not_estimated = true;
    const double t = tonnes(I, e);
    if (e.biogenic) {
      T.biogenic += t;
      continue;
    }
    c.tonnes += t;
    const double v = (entry_u(e) * t) * (entry_u(e) * t);
    if (in_level(e, Level::basic)) T.basic += t, var[0] += v;
    if (in_level(e, Level::basic_plus)) T.basic_plus += t, var[1] += v, T.by_gas[e.gas] += t;
    if (in_level(e, Level::territorial)) T.territorial += t, var[2] += v;
  }
  auto rel = [](double v, double x) { return x != 0 ? std::sqrt(v) / std::fabs(x) : 0.0; };
  T.u_basic = rel(var[0], T.basic), T.u_basic_plus = rel(var[1], T.basic_plus), T.u_territorial = rel(var[2], T.territorial);
  auto check = [&](Level L, std::vector<std::string>& gaps) {
    for (auto [s, sc] : cells(L)) {
      auto it = cs.find({static_cast<int>(s), sc});
      const std::string label = std::string(name(s)) + ", scope " + std::to_string(sc);
      if (it == cs.end()) {
        Cell c;
        c.sector = s, c.scope = sc, c.missing = true;
        cs[{static_cast<int>(s), sc}] = c;
        gaps.push_back(label + ": no entry");
      } else if (it->second.not_estimated) {
        gaps.push_back(label + ": not estimated (NE)");
      }
    }
    return gaps.empty();
  };
  T.basic_complete = check(Level::basic, T.gaps_basic);
  T.basic_plus_complete = check(Level::basic_plus, T.gaps_basic_plus);
  for (auto& [k, c] : cs) T.cells.push_back(c);
  if (std::isfinite(I.population) && I.population > 0) T.per_capita = T.basic_plus / I.population;
  if (std::isfinite(I.gdp) && I.gdp > 0) T.per_gdp = T.basic_plus / I.gdp;
  return T;
}

struct Category {
  std::string sector;
  int scope = 1;
  std::string subsector, gas;
  double tonnes = 0, share = 0, cumulative = 0;
  bool key = false;
};

// Key categories by the IPCC's Approach 1 level assessment over a level: categories (sector, scope, subsector, gas) in descending order of
// |tonnes|, key until their cumulative share reaches the threshold (95% by default).
inline std::vector<Category> key_categories(const Inventory& I, Level L = Level::basic_plus, double threshold = 0.95) {
  validate(I);
  std::map<std::tuple<int, int, std::string, std::string>, double> by;
  for (const auto& e : I.entries)
    if (in_level(e, L) && e.key == Key::estimated) by[{static_cast<int>(e.sector), e.scope, e.subsector, e.gas}] += tonnes(I, e);
  std::vector<Category> out;
  double total = 0;
  for (const auto& [k, t] : by) {
    out.push_back({name(static_cast<Sector>(std::get<0>(k))), std::get<1>(k), std::get<2>(k), std::get<3>(k), t});
    total += std::fabs(t);
  }
  std::stable_sort(out.begin(), out.end(), [](const Category& a, const Category& b) { return std::fabs(a.tonnes) > std::fabs(b.tonnes); });
  double cum = 0;
  bool reached = false;
  for (auto& c : out) {
    c.share = total > 0 ? std::fabs(c.tonnes) / total : 0;
    c.key = !reached;
    cum += c.share, c.cumulative = cum;
    if (cum >= threshold - 1e-12) reached = true;
  }
  return out;
}

// ---- mitigation -------------------------------------------------------------------------------------------------------------------

enum class On { activity, factor };

struct Measure {
  std::string id, name;
  Sector sector = Sector::stationary_energy;
  bool any_sector = false;      // a measure on every sector (a cleaner grid lowers every scope 2 entry)
  int scope = 0;                // 0: any scope
  std::string subsector, gas;   // empty: any
  On on = On::activity;
  double reduction = 0;         // the share of what remains that it removes, in [0, 1]
  double cost = 0;              // a year's cost once in full effect (negative: a saving)
  int start = 0;                // the first year it has effect
  int ramp = 1;                 // years to full effect
};

inline bool acts_on(const Measure& m, const Entry& e) {
  return e.key == Key::estimated && (m.any_sector || m.sector == e.sector) && (m.scope == 0 || m.scope == e.scope) && (m.subsector.empty() || m.subsector == e.subsector) && (m.gas.empty() || m.gas == e.gas);
}

inline void validate(const std::vector<Measure>& ms) {
  std::set<std::string> ids;
  for (const auto& m : ms) {
    if (!ids.insert(m.id).second) throw std::invalid_argument("emissions: every measure has an id of its own (\"" + m.id + "\")");
    if (!(m.reduction >= 0 && m.reduction <= 1)) throw std::invalid_argument("emissions: measure " + m.id + ": a reduction between 0 and 1");
    if (m.ramp < 1) throw std::invalid_argument("emissions: measure " + m.id + ": a ramp of a year or more");
  }
}

// Tonnes CO2e within a level with the measures of `set` (a mask over `ms`) applied, each at the share `effect[k]` of its reduction.
inline double with_measures(const Inventory& I, const std::vector<Measure>& ms, std::uint32_t set, Level L, const std::vector<double>& effect = {}, double activity_scale = 1.0,
                            const std::map<Sector, double>& growth = {}) {
  double total = 0;
  for (const auto& e : I.entries) {
    if (!in_level(e, L) || e.key != Key::estimated) continue;
    double a = 1, f = 1;
    for (std::size_t k = 0; k < ms.size(); k++) {
      if (!(set >> k & 1u) || !acts_on(ms[k], e)) continue;
      const double r = ms[k].reduction * (effect.empty() ? 1.0 : effect[k]);
      (ms[k].on == On::activity ? a : f) *= 1 - r;
    }
    const auto g = growth.find(e.sector);
    total += tonnes(I, e) * a * f * (g == growth.end() ? activity_scale : g->second);
  }
  return total;
}

struct Abatement {
  std::string id, name;
  double alone = 0;              // tonnes abated by the measure alone
  double in_order = 0;           // its marginal abatement in the declared order
  double shapley = 0;            // its Shapley value over every order
  double cost = 0;
  double cost_per_tonne_alone = none, cost_per_tonne_in_order = none, cost_per_tonne_shapley = none;
  int position = 0;              // its place in the order
};

struct Curve {
  double baseline = 0, joint = 0, sum_alone = 0, overlap = 0;  // overlap = Σ alone − joint
  std::vector<Abatement> measures;                              // in the order of the curve
};

// The abatement cost curve. The order is by cost per tonne abated alone (the usual curve) unless `order` lists every measure's id.
inline Curve curve(const Inventory& I, const std::vector<Measure>& ms, Level L = Level::basic_plus, std::vector<std::string> order = {}) {
  validate(I);
  validate(ms);
  const std::size_t n = ms.size();
  if (n == 0 || n > 20) throw std::invalid_argument("emissions: between one and twenty measures");
  Curve C;
  C.baseline = with_measures(I, ms, 0, L);
  const std::uint32_t all = (std::uint32_t{1} << n) - 1;
  C.joint = C.baseline - with_measures(I, ms, all, L);
  std::vector<Abatement> A(n);
  for (std::size_t k = 0; k < n; k++) {
    A[k].id = ms[k].id, A[k].name = ms[k].name, A[k].cost = ms[k].cost;
    A[k].alone = C.baseline - with_measures(I, ms, std::uint32_t{1} << k, L);
    C.sum_alone += A[k].alone;
    if (A[k].alone > 0) A[k].cost_per_tonne_alone = ms[k].cost / A[k].alone;
  }
  C.overlap = C.sum_alone - C.joint;
  const auto phi = nomos::games::shapley(n, [&](std::uint32_t S) { return C.baseline - with_measures(I, ms, S, L); });
  for (std::size_t k = 0; k < n; k++) {
    A[k].shapley = phi[k];
    if (phi[k] > 0) A[k].cost_per_tonne_shapley = ms[k].cost / phi[k];
  }
  std::vector<std::size_t> idx(n);
  for (std::size_t k = 0; k < n; k++) idx[k] = k;
  if (order.empty()) {
    std::stable_sort(idx.begin(), idx.end(), [&](std::size_t a, std::size_t b) {
      const double x = std::isfinite(A[a].cost_per_tonne_alone) ? A[a].cost_per_tonne_alone : std::numeric_limits<double>::infinity();
      const double y = std::isfinite(A[b].cost_per_tonne_alone) ? A[b].cost_per_tonne_alone : std::numeric_limits<double>::infinity();
      return x < y;
    });
  } else {
    if (order.size() != n) throw std::invalid_argument("emissions: an order lists every measure once");
    for (std::size_t p = 0; p < n; p++) {
      auto it = std::find_if(ms.begin(), ms.end(), [&](const Measure& m) { return m.id == order[p]; });
      if (it == ms.end()) throw std::invalid_argument("emissions: no measure \"" + order[p] + "\" to order");
      idx[p] = static_cast<std::size_t>(it - ms.begin());
    }
    if (std::set<std::size_t>(idx.begin(), idx.end()).size() != n) throw std::invalid_argument("emissions: an order lists every measure once");
  }
  std::uint32_t S = 0;
  double before = C.baseline;
  for (std::size_t p = 0; p < n; p++) {
    const std::size_t k = idx[p];
    S |= std::uint32_t{1} << k;
    const double after = with_measures(I, ms, S, L);
    A[k].in_order = before - after, A[k].position = static_cast<int>(p) + 1;
    if (A[k].in_order > 0) A[k].cost_per_tonne_in_order = ms[k].cost / A[k].in_order;
    before = after;
    C.measures.push_back(A[k]);
  }
  return C;
}

// ---- pathway ----------------------------------------------------------------------------------------------------------------------

struct Target {
  int year = 0;
  double reduction = none;   // from the base year's emissions within the level
  double budget = none;      // cumulative tonnes over the years of the pathway
};

struct Year {
  int year = 0;
  double baseline = 0, with_measures = 0, cost = 0;
};

struct Pathway {
  std::vector<Year> years;
  double cumulative_baseline = 0, cumulative_with = 0;
  double target_level = none, gap = none;   // the target's emissions, and the pathway's emissions above them in its year
  bool meets_target = false, within_budget = true;
};

// Years from the inventory's year + 1 to `last`: activities grow by sector at declared rates; each measure's effect rises linearly
// from its start over its ramp; the cost of a year is each measure's cost at its share of full effect.
inline Pathway pathway(const Inventory& I, const std::vector<Measure>& ms, const std::map<Sector, double>& growth, int last, const Target& target = {}, Level L = Level::basic_plus) {
  validate(I);
  validate(ms);
  if (last <= I.year) throw std::invalid_argument("emissions: a pathway runs past the inventory's year");
  if (ms.size() > 32) throw std::invalid_argument("emissions: at most 32 measures in a pathway");
  Pathway P;
  const double base = with_measures(I, ms, 0, L);
  const std::uint32_t all = ms.size() == 32 ? 0xffffffffu : ((std::uint32_t{1} << ms.size()) - 1);
  for (int y = I.year + 1; y <= last; y++) {
    std::map<Sector, double> scale;
    for (const auto& [s, g] : growth) scale[s] = std::pow(1 + g, y - I.year);
    std::vector<double> effect(ms.size());
    double cost = 0;
    for (std::size_t k = 0; k < ms.size(); k++) {
      effect[k] = std::clamp((y - ms[k].start + 1) / static_cast<double>(ms[k].ramp), 0.0, 1.0);
      cost += ms[k].cost * effect[k];
    }
    Year Y{y, with_measures(I, ms, 0, L, {}, 1.0, scale), with_measures(I, ms, all, L, effect, 1.0, scale), cost};
    P.cumulative_baseline += Y.baseline, P.cumulative_with += Y.with_measures;
    P.years.push_back(Y);
  }
  if (target.year > 0 && std::isfinite(target.reduction)) {
    P.target_level = base * (1 - target.reduction);
    auto it = std::find_if(P.years.begin(), P.years.end(), [&](const Year& Y) { return Y.year == target.year; });
    if (it == P.years.end()) throw std::invalid_argument("emissions: the target's year lies outside the pathway");
    P.gap = it->with_measures - P.target_level;
    P.meets_target = P.gap <= 1e-9 * std::max(1.0, std::fabs(P.target_level));
  }
  if (std::isfinite(target.budget)) P.within_budget = P.cumulative_with <= target.budget;
  return P;
}

}  // namespace industrial_ecology::emissions
