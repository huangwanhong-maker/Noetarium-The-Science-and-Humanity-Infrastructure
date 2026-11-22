// Tourism accounts and levies (theory note `theory/tourism-accounts-and-levies.md`; ADR 0157): what visitors' spending becomes in an
// economy, and what a levy on their nights raises and who bears it.
//
// Accounts. Visitors' spending by product, at purchasers' prices, is carried to basic prices by a declared share and split by each
// product's domestic share of households' use into domestic demand and imports. Domestic demand d runs through the domestic input–output
// table's Leontief inverse (economics::leontief): output x = L d. Its value added, imported inputs and net taxes on products are each
// column's coefficients times x. Direct value added is the value added of the domestic products visitors buy, the tourism satellite
// account's direct measure (tourism ratio times value added). Proposition 1: total value added, induced imported inputs and induced
// product taxes sum to domestic demand exactly, since every column's coefficients sum to one; so leakage is the imports of the final
// purchases plus the induced imported inputs, and nothing is lost or created by the chain. For a region (ADR 0159) its purchases from the
// rest of the country are a fourth leak, and the four sum to its domestic demand.
//
// Levies. Nights in a market of declared price and constant elasticities of demand (in the visitor's price) and supply (in the
// operator's) meet at an equilibrium where the visitor's price exceeds the operator's by the levy: per night (flat, solved by Brent's
// method on calx) or a rate on the operator's price (in closed form). Proposition 2: the visitor's share of the burden tends, for a small
// levy, to ε_s / (ε_s − ε_d); the burdens of visitors and operators sum to the revenue; the excess burden (surpluses lost beyond the
// revenue) is not negative and, for a small levy, is half the levy times the nights lost (Harberger). A declared share of the nights lost
// may move to stays the levy does not reach. Nothing here names a place or a rate.

#pragma once

#include <economics/production/input_output.hpp>

#include <calx/optimisation/minimise.hpp>
#include <calx/solvers/roots.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::tourism {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("economics::tourism: " + what);
}
}  // namespace detail

constexpr double infinite = std::numeric_limits<double>::infinity();

// A symmetric input–output table at basic prices, product by product, split into domestic and imported flows.
struct Accounts {
  InputOutputTable domestic;             // z: domestic intermediate flows; final_demand: domestic final use; value_added: the rest of each column
  std::vector<double> gva, taxes, imported;  // each column's gross value added, net taxes on products and imported intermediate inputs
  std::vector<double> domestic_share;    // of households' use of each product, the part supplied at home (1 where not known)
  std::vector<double> rest;              // for a region (ADR 0159): each column's purchases from the rest of the country; empty for a country
  std::vector<double> rest_share;        // for a region: of households' use, the part supplied from the rest of the country; empty for a country
  // The largest gap, relative to output, between a column's output and its domestic inputs, imported inputs, taxes and value added.
  double imbalance() const {
    const auto x = domestic.output();
    double worst = 0;
    for (std::size_t j = 0; j < domestic.n(); j++) {
      double col = gva.at(j) + taxes.at(j) + imported.at(j) + (rest.empty() ? 0.0 : rest.at(j));
      for (std::size_t i = 0; i < domestic.n(); i++) col += domestic.z(i, j);
      if (x[j] > 0) worst = std::max(worst, std::fabs(col - x[j]) / x[j]);
    }
    return worst;
  }
};

// What visitors buy of a product, per night of a visitor of each origin, at purchasers' prices, and the share of it at basic prices.
struct Purchase {
  std::string product;
  std::map<std::string, double> per_night;  // origin → spending a night
  double basic_share = 1;                   // basic over purchasers' prices (taxes on the final purchase and margins declared)
};

// Tourism demand by product at basic prices: Σ over origins of nights × spending a night × the basic share.
inline std::vector<double> demand(const Accounts& A, const std::vector<Purchase>& purchases, const std::map<std::string, double>& nights) {
  std::vector<double> d(A.domestic.n(), 0.0);
  for (const auto& p : purchases) {
    std::size_t k = A.domestic.n();
    for (std::size_t i = 0; i < A.domestic.n(); i++)
      if (A.domestic.sectors[i] == p.product) k = i;
    detail::require(k < A.domestic.n(), "no product " + p.product + " in the table");
    detail::require(p.basic_share > 0 && p.basic_share <= 1, "a basic share lies in (0, 1]");
    for (const auto& [origin, spend] : p.per_night) {
      detail::require(spend >= 0, "spending is not negative");
      const auto n = nights.find(origin);
      detail::require(n != nights.end() && n->second >= 0, "nights for the origin " + origin);
      d[k] += n->second * spend * p.basic_share;
    }
  }
  return d;
}

struct Effects {
  double demand = 0, demand_domestic = 0, demand_imported = 0;  // at basic prices
  double demand_rest = 0;                  // for a region: final purchases from the rest of the country
  std::vector<double> ratio;               // tourism ratio: domestic tourism demand over output, by product
  std::vector<double> total_output;        // x = L d, by product
  double direct_gva = 0, total_gva = 0, indirect_gva = 0;
  double induced_imports = 0, induced_taxes = 0;
  double induced_rest = 0;                 // for a region: inputs induced from the rest of the country
  double unattributed = 0;                 // domestic demand less the three: zero for a balanced table, its imbalance otherwise
  double gva_multiplier = 0;               // total value added per unit of domestic demand
  double leakage = 0;                      // (imports of the final purchases + induced imported inputs) over demand
  double leakage_country = 0;              // for a region: (final purchases and induced inputs from the rest of the country) over demand
  double direct_share_of_gva = 0;          // direct value added over the economy's value added
};

inline Effects effects(const Accounts& A, const std::vector<double>& d) {
  const std::size_t n = A.domestic.n();
  detail::require(d.size() == n && A.gva.size() == n && A.taxes.size() == n && A.imported.size() == n && A.domestic_share.size() == n, "one entry per product");
  detail::require((A.rest.empty() || A.rest.size() == n) && (A.rest_share.empty() || A.rest_share.size() == n), "a region's purchases from the rest of the country, one per product");
  const auto M = leontief(A.domestic);
  Effects out;
  std::vector<double> dd(n);
  double economy_gva = 0;
  for (std::size_t i = 0; i < n; i++) {
    detail::require(d[i] >= 0 && A.domestic_share[i] >= 0 && A.domestic_share[i] <= 1, "demand is not negative and a domestic share lies in [0, 1]");
    const double rs = A.rest_share.empty() ? 0.0 : A.rest_share[i];
    detail::require(rs >= 0 && A.domestic_share[i] + rs <= 1 + 1e-12, "the local and the rest-of-country shares sum to at most one");
    dd[i] = d[i] * A.domestic_share[i];
    out.demand += d[i], out.demand_domestic += dd[i], out.demand_rest += d[i] * rs;
    economy_gva += A.gva[i];
  }
  out.demand_imported = out.demand - out.demand_domestic - out.demand_rest;
  out.total_output = output_for(M, dd);
  out.ratio.resize(n);
  for (std::size_t i = 0; i < n; i++) {
    const double x = M.output[i];
    out.ratio[i] = dd[i] / x;
    out.direct_gva += A.gva[i] / x * dd[i];
    out.total_gva += A.gva[i] / x * out.total_output[i];
    out.induced_imports += A.imported[i] / x * out.total_output[i];
    out.induced_taxes += A.taxes[i] / x * out.total_output[i];
    if (!A.rest.empty()) out.induced_rest += A.rest[i] / x * out.total_output[i];
  }
  out.indirect_gva = out.total_gva - out.direct_gva;
  out.unattributed = out.demand_domestic - out.total_gva - out.induced_imports - out.induced_taxes - out.induced_rest;
  out.gva_multiplier = out.demand_domestic > 0 ? out.total_gva / out.demand_domestic : 0;
  out.leakage = out.demand > 0 ? (out.demand_imported + out.induced_imports) / out.demand : 0;
  out.leakage_country = out.demand > 0 ? (out.demand_rest + out.induced_rest) / out.demand : 0;
  out.direct_share_of_gva = economy_gva > 0 ? out.direct_gva / economy_gva : 0;
  return out;
}

// ---- levies -------------------------------------------------------------------------------------------------------------------

struct Market {
  double nights = 0, price = 0;           // before the levy: nights a year, the price of a night
  double demand_elasticity = -1;          // of nights in the visitor's price, < 0
  double supply_elasticity = infinite;    // of nights in the operator's price, > 0 (infinite: the levy passed on in full)
  double displaced = 0;                   // the share of nights lost that move to stays the levy does not reach
  void check() const {
    detail::require(nights > 0 && price > 0, "a market has nights and a price");
    detail::require(demand_elasticity < 0 && supply_elasticity > 0, "demand falls and supply rises with price");
    detail::require(displaced >= 0 && displaced <= 1, "a displaced share lies in [0, 1]");
  }
};

struct Levy {
  double levy = 0;                        // per night, at the equilibrium
  double consumer_price = 0, producer_price = 0, pass_through = 0;  // pass-through: the visitor's share of the levy
  double nights_after = 0, nights_lost = 0, displaced_nights = 0;
  double revenue = 0, visitors_burden = 0, operators_burden = 0;
  double consumer_surplus_loss = 0, producer_surplus_loss = 0, excess_burden = 0;
};

namespace detail {
// ∫_a^b Q0 (p/P0)^ε dp.
inline double area(double Q0, double P0, double e, double a, double b) {
  if (std::fabs(e + 1) < 1e-12) return Q0 * P0 * std::log(b / a);
  return Q0 * P0 / (e + 1) * (std::pow(b / P0, e + 1) - std::pow(a / P0, e + 1));
}
inline Levy settle(const Market& m, double pc, double pp) {
  Levy L;
  L.consumer_price = pc, L.producer_price = pp, L.levy = pc - pp;
  L.pass_through = L.levy > 0 ? (pc - m.price) / L.levy : (std::isinf(m.supply_elasticity) ? 1 : m.supply_elasticity / (m.supply_elasticity - m.demand_elasticity));
  L.nights_after = m.nights * std::pow(pc / m.price, m.demand_elasticity);
  L.nights_lost = m.nights - L.nights_after;
  L.displaced_nights = m.displaced * L.nights_lost;
  L.revenue = L.levy * L.nights_after;
  L.visitors_burden = (pc - m.price) * L.nights_after;
  L.operators_burden = (m.price - pp) * L.nights_after;
  L.consumer_surplus_loss = area(m.nights, m.price, m.demand_elasticity, m.price, pc);
  L.producer_surplus_loss = std::isinf(m.supply_elasticity) ? 0 : area(m.nights, m.price, m.supply_elasticity, pp, m.price);
  L.excess_burden = L.consumer_surplus_loss + L.producer_surplus_loss - L.revenue;
  return L;
}
}  // namespace detail

// A levy of `t` a night: the visitor's price p solves Q0 (p/P0)^ε_d = Q0 ((p − t)/P0)^ε_s, by Brent's method on calx.
inline Levy flat(const Market& m, double t) {
  m.check();
  detail::require(t >= 0, "a levy is not negative");
  if (t == 0) return detail::settle(m, m.price, m.price);
  if (std::isinf(m.supply_elasticity)) return detail::settle(m, m.price + t, m.price);
  const auto f = [&](double p) { return m.demand_elasticity * std::log(p / m.price) - m.supply_elasticity * std::log((p - t) / m.price); };
  const double lo = t + 1e-12 * (m.price + t), hi = m.price + t;
  const auto r = calx::solvers::brent(f, lo, hi);
  detail::require(r.converged, "the equilibrium was not found");
  return detail::settle(m, r.x, r.x - t);
}

// A rate `r` on the operator's price: p_c = (1 + r) p_p, and p_p = P0 (1 + r)^{ε_d / (ε_s − ε_d)} in closed form.
inline Levy ad_valorem(const Market& m, double r) {
  m.check();
  detail::require(r >= 0, "a rate is not negative");
  if (std::isinf(m.supply_elasticity)) return detail::settle(m, m.price * (1 + r), m.price);
  const double pp = m.price * std::pow(1 + r, m.demand_elasticity / (m.supply_elasticity - m.demand_elasticity));
  return detail::settle(m, pp * (1 + r), pp);
}

// The flat levy in [0, cap] that raises the most, by Brent's maximiser on calx; `at_cap` when the most lies at the cap.
struct Peak {
  double levy = 0, revenue = 0;
  bool at_cap = false;
};
inline Peak revenue_maximising(const Market& m, double cap) {
  m.check();
  detail::require(cap > 0, "a cap is positive");
  const auto best = calx::optimisation::brent_maximise([&](double t) { return flat(m, t).revenue; }, 0.0, cap, 1e-9 * cap);
  Peak p{best.x, flat(m, best.x).revenue, false};
  const double at = flat(m, cap).revenue;
  if (at >= p.revenue - 1e-9 * std::max(1.0, at)) p = {cap, at, true};
  return p;
}

}  // namespace economics::tourism
