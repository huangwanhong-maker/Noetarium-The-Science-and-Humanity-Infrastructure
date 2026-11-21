// Land value capture and land readjustment: what a public project adds to the value of land, and how much of it each instrument
// takes back, from whom, and when (ADR 0146).
//
// Uplift. Each parcel has a value with the project and without it at a declared valuation date, the counterfactual stated by
// the caller (from a hedonic model or a comparison of treated and untreated places; a before-and-after difference is not one,
// since it carries the market's trend). The uplift ΔV_i = V_i(with) − V_i(without) is what may be captured.
//
// Instruments, each a stream of payments with its payer, compared on present values at one rate (the survey *Learning finance
// scorecards and value capture*, Part IV, and its design section, whose identities are held here):
//   levy                 L = min(cost (1 + a), λ Σ ΔV_i), apportioned by a declared score of each parcel (its uplift, area or frontage),
//                        paid by owners in declared instalments;
//   development rights   additional floor area sold, by the formula C = (ΔA / FAR_basic) · p_land · k (the "virtual land" method) or
//                        at auction: certificates at a minimum price sold to the bids at or above it, the stock identity authorised =
//                        sold + unsold kept;
//   tax increment        R_t = s Σ_j τ_j max(0, AV_t − B) over the district's term; of the increment I_t = AV_t − B, the share 1 − β
//                        would have come without the project (β the "but-for" share), and taking it diverts τ_j (1 − β) I_t from each
//                        overlapping jurisdiction j;
//   developer obligation a rate per unit of what is permitted, its incidence split by declared shares between land (lower prices) and
//                        buyers (higher prices);
//   land readjustment    below.
// Three ratios that the literature calls "capture" are kept apart: cost recovery ρ = PV revenue / PV cost; capture of the
// counterfactual uplift κ = PV taken from the uplift / PV ΣΔV_i; and the statutory rate σ the caller declares. A parcel whose uplift
// is taken more than once over is named (no double charge of one increase).
//
// Land readjustment. Owners pool land; the project returns smaller plots worth more. With public land before M, private land before
// A, public land after N, land returned to owners E and reserve land R sold to pay for the works, the area identity is M + A = N +
// E + R; the contribution ratio d = (P + R)/A with P = N − M; with unit values a before and e after, y = e/a, and the owners' land keeps
// its value when (1 − d) y ≥ 1. Each owner's entitlement is V*_i = V_i · ΣV′ / ΣV (their share of the value returned), the settlement
// s_i = V′_i − V*_i for the plot V′_i they receive, Σ s_i = 0; a declared cap on the contribution ratio is checked; the project is
// feasible when the reserve land's sale, net of selling costs, with subsidies, meets the cost in present value. Replotting assigns
// each owner a plot by calx's assignment, nearest first (the least total distance from old plots to new), and settles the rest in
// money.
//
// Every rate, share, cap and value is the caller's declaration; nothing here is any jurisdiction's law.
#pragma once

#include <calx/linalg/rectangular.hpp>
#include <calx/optimisation/assignment.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::capture {

inline constexpr double none = std::numeric_limits<double>::quiet_NaN();

struct Parcel {
  std::string id;
  double area = 0;
  double without = 0, with = 0;   // values at the valuation date, without and with the project
  double frontage = 0;            // for apportionment by frontage, if used
  double x = 0, y = 0;            // where it lies, for replotting
  double uplift() const { return with - without; }
};

struct Payment {
  int year = 0;                   // years after the valuation date (0 at it)
  double amount = 0;
  std::string payer;              // "owners", "developers", "buyers", "jurisdiction:<name>", "outside owners"
  std::string parcel;             // the parcel it falls on, if one
  bool from_uplift = true;        // whether it is taken from the uplift (a cost-based fee on new floor area may not be)
};

inline double present_value(const std::vector<Payment>& ps, double rate) {
  double v = 0;
  for (const auto& p : ps) v += p.amount / std::pow(1 + rate, p.year);
  return v;
}

// ---- the instruments ---------------------------------------------------------------------------------------------------------

enum class Basis { uplift, area, frontage };

// A levy of min(cost (1 + a), λ ΣΔV) apportioned by the basis, in `instalments` equal yearly payments from year `first`.
inline std::vector<Payment> levy(const std::vector<Parcel>& ps, double cost, double markup, double share, Basis basis, int first = 0, int instalments = 1) {
  if (cost < 0 || markup < 0 || share < 0 || share > 1 || instalments < 1) throw std::invalid_argument("economics::capture: a levy's cost, mark-up and share are declared, the share at most 1");
  double uplift = 0, total_score = 0;
  for (const auto& p : ps) uplift += std::max(0.0, p.uplift());
  const double L = std::min(cost * (1 + markup), share * uplift);
  auto score = [&](const Parcel& p) { return basis == Basis::uplift ? std::max(0.0, p.uplift()) : basis == Basis::area ? p.area : p.frontage; };
  for (const auto& p : ps) total_score += score(p);
  std::vector<Payment> out;
  if (total_score <= 0 || L <= 0) return out;
  for (const auto& p : ps)
    for (int k = 0; k < instalments; k++) out.push_back({first + k, L * score(p) / total_score / instalments, "owners", p.id, true});
  return out;
}

// Additional floor area sold by the formula (ΔA / FAR_basic) p_land k, in year `year`.
inline Payment rights_by_formula(double added_floor, double basic_far, double land_price, double factor, int year = 0) {
  if (!(basic_far > 0) || added_floor < 0) throw std::invalid_argument("economics::capture: a positive basic floor-area ratio and added floor area");
  return {year, added_floor / basic_far * land_price * factor, "developers", "", true};
}

struct Auction {
  std::size_t authorised = 0, sold = 0, unsold = 0;
  double price = 0, revenue = 0;
};

// Certificates at a minimum price, sold to the highest bids at or above it while they last, each at the clearing price (the lowest
// accepted bid, or the minimum when every certificate is taken by bids above it): authorised = sold + unsold.
inline Auction rights_by_auction(std::size_t authorised, double minimum, std::vector<double> bids) {
  std::sort(bids.begin(), bids.end(), std::greater<double>());
  Auction a;
  a.authorised = authorised;
  std::vector<double> accepted;
  for (double b : bids)
    if (b >= minimum && accepted.size() < authorised) accepted.push_back(b);
  a.sold = accepted.size(), a.unsold = authorised - a.sold;
  a.price = accepted.empty() ? 0 : (a.sold < authorised ? minimum : std::max(minimum, accepted.back()));
  a.revenue = a.price * static_cast<double>(a.sold);
  return a;
}

struct Increment {
  std::vector<Payment> district;     // what the district collects
  std::vector<Payment> diverted;     // what would have come without the project, taken from each overlapping jurisdiction
  double pv_district = 0, pv_diverted = 0;
};

// Tax increment financing: values AV_t for t = 1 … T, base B, the district's share s of the increment, the rates of the overlapping
// jurisdictions, and the but-for share β of the increment that is the project's.
inline Increment tax_increment(const std::vector<double>& assessed, double base, double share, const std::map<std::string, double>& rates, double but_for, double rate) {
  if (share < 0 || share > 1 || but_for < 0 || but_for > 1) throw std::invalid_argument("economics::capture: shares between 0 and 1");
  Increment out;
  for (std::size_t t = 0; t < assessed.size(); t++) {
    const double I = std::max(0.0, assessed[t] - base);
    for (const auto& [j, tau] : rates) {
      const int year = static_cast<int>(t) + 1;
      out.district.push_back({year, share * tau * I, "jurisdiction:" + j, "", true});
      out.diverted.push_back({year, share * tau * (1 - but_for) * I, "jurisdiction:" + j, "", false});
    }
  }
  out.pv_district = present_value(out.district, rate), out.pv_diverted = present_value(out.diverted, rate);
  return out;
}

struct Obligation {
  std::vector<Payment> payments;
  double on_land = 0, on_buyers = 0;  // the incidence of the total, by declared shares
};

inline Obligation developer_obligation(double units, double rate_per_unit, double to_land, double to_buyers, int year = 0) {
  if (to_land < 0 || to_buyers < 0 || std::fabs(to_land + to_buyers - 1) > 1e-9) throw std::invalid_argument("economics::capture: the incidence on land and on buyers sums to one");
  Obligation o;
  const double total = units * rate_per_unit;
  o.payments.push_back({year, total, "developers", "", false});
  o.on_land = to_land * total, o.on_buyers = to_buyers * total;
  return o;
}

// ---- comparisons ----------------------------------------------------------------------------------------------------------------

struct Comparison {
  std::string instrument;
  double pv = 0;               // public revenue
  double cost_recovery = none; // ρ = PV revenue / PV cost
  double capture = none;       // κ = PV taken from the uplift / PV of the uplift
  double statutory = none;     // σ, as declared
  std::map<std::string, double> by_payer;
};

inline Comparison compare(const std::string& name, const std::vector<Payment>& ps, double rate, double pv_cost, double pv_uplift, double statutory = none) {
  Comparison c;
  c.instrument = name, c.pv = present_value(ps, rate), c.statutory = statutory;
  double taken = 0;
  for (const auto& p : ps) {
    const double v = p.amount / std::pow(1 + rate, p.year);
    c.by_payer[p.payer] += v;
    if (p.from_uplift) taken += v;
  }
  if (pv_cost > 0) c.cost_recovery = c.pv / pv_cost;
  if (pv_uplift > 0) c.capture = taken / pv_uplift;
  return c;
}

struct DoubleCharge {
  std::string parcel;
  double uplift = 0, taken = 0;
};

// The parcels from whose uplift the instruments together take more than the uplift itself (present values at the valuation date).
inline std::vector<DoubleCharge> double_charges(const std::vector<Parcel>& ps, const std::vector<std::vector<Payment>>& instruments, double rate) {
  std::map<std::string, double> taken;
  for (const auto& ins : instruments)
    for (const auto& p : ins)
      if (p.from_uplift && !p.parcel.empty()) taken[p.parcel] += p.amount / std::pow(1 + rate, p.year);
  std::vector<DoubleCharge> out;
  for (const auto& p : ps)
    if (taken[p.id] > std::max(0.0, p.uplift()) * (1 + 1e-12) + 1e-9) out.push_back({p.id, p.uplift(), taken[p.id]});
  return out;
}

// ---- land readjustment ------------------------------------------------------------------------------------------------------------

struct Areas {
  double public_before = 0, private_before = 0, public_after = 0, returned = 0, reserve = 0;
};

struct Readjustment {
  bool areas_balance = false;
  double imbalance = 0;            // M + A − (N + E + R)
  double contribution = none;      // d = (P + R)/A
  double public_share = none, reserve_share = none;
  double uplift_ratio = none;      // y = e/a
  double kept = none;              // (1 − d) y: at least 1 when owners keep their value
  bool value_kept = false;
  bool within_cap = true;
  double reserve_revenue = 0;      // R e (1 − selling cost)
  double pv_balance = 0;           // PV of reserve sales and subsidies less costs
  bool feasible = false;
};

inline Readjustment readjust(const Areas& m, double value_before, double value_after, double cap, double selling_cost, const std::vector<Payment>& subsidies,
                             const std::vector<Payment>& costs, int sale_year, double rate) {
  if (!(m.private_before > 0) || !(value_before > 0)) throw std::invalid_argument("economics::capture: private land before and its unit value are positive");
  Readjustment r;
  r.imbalance = m.public_before + m.private_before - (m.public_after + m.returned + m.reserve);
  r.areas_balance = std::fabs(r.imbalance) <= 1e-9 * (m.public_before + m.private_before);
  const double P = m.public_after - m.public_before;
  r.public_share = P / m.private_before, r.reserve_share = m.reserve / m.private_before;
  r.contribution = (P + m.reserve) / m.private_before;
  r.uplift_ratio = value_after / value_before;
  r.kept = (1 - r.contribution) * r.uplift_ratio;
  r.value_kept = r.kept >= 1 - 1e-12;
  r.within_cap = !std::isfinite(cap) || r.contribution <= cap + 1e-12;
  r.reserve_revenue = m.reserve * value_after * (1 - selling_cost);
  std::vector<Payment> flows = subsidies;
  flows.push_back({sale_year, r.reserve_revenue, "buyers of reserve land", "", true});
  for (const auto& c : costs) flows.push_back({c.year, -c.amount, c.payer, c.parcel, false});
  r.pv_balance = present_value(flows, rate);
  r.feasible = r.pv_balance >= 0;
  return r;
}

struct Plot {
  std::string id;
  double area = 0, unit_value = 0;
  double x = 0, y = 0;
  double value() const { return area * unit_value; }
};

struct Replotting {
  std::vector<std::size_t> plot_of;   // for each owner (parcel), the plot assigned
  std::vector<double> entitlement;    // V*_i = V_i ΣV′ / ΣV
  std::vector<double> received;       // V′_i
  std::vector<double> settlement;     // s_i = V′_i − V*_i: positive, the owner pays it; negative, the owner is paid
  double distance = 0;                // Σ of the distances from old parcels to new plots
};

// Each owner's parcel assigned a plot, the least total distance from old parcels to new plots (calx's assignment; as many plots
// as owners or more), entitlements in proportion to the values before, and settlements in money.
inline Replotting replot(const std::vector<Parcel>& owners, const std::vector<Plot>& plots) {
  const std::size_t n = owners.size(), m = plots.size();
  if (n == 0 || m < n) throw std::invalid_argument("economics::capture: at least as many plots as owners");
  calx::linalg::Rectangular cost(n, m);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < m; j++) cost(i, j) = std::hypot(owners[i].x - plots[j].x, owners[i].y - plots[j].y);
  const auto a = calx::optimisation::assignment(cost);
  Replotting r;
  r.plot_of = a.column, r.distance = a.cost;
  double before = 0, returned = 0;
  for (const auto& o : owners) before += o.without;
  for (std::size_t i = 0; i < n; i++) returned += plots[a.column[i]].value();
  if (!(before > 0)) throw std::invalid_argument("economics::capture: owners' values before are positive");
  for (std::size_t i = 0; i < n; i++) {
    const double e = owners[i].without * returned / before, v = plots[a.column[i]].value();
    r.entitlement.push_back(e), r.received.push_back(v), r.settlement.push_back(v - e);
  }
  return r;
}

}  // namespace economics::capture
