// A market, and what is computed from it (the domain's theory note, §1–§4).
//
// A MARKET is M = (B, μ, q; O, o₀; K). Buyers are a finite measure space: cells b of mass μ_b ≥ 0 (people, households, firms), each
// buyer of a cell bringing a spend q_b ≥ 0 in a period. Offers o ∈ O are what is sold, each by a seller; o₀ is the outside option,
// not buying. The kernel K gives, for each cell, the share of its spend that goes to each offer: K(b, o) ≥ 0, Σ_o K(b, o) ≤ 1, and
// what is left, K(b, o₀) = 1 − Σ_o K(b, o), is not spent in this market. A kernel says where it comes from: observed (transactions),
// stated (a survey) or presumed (a model with declared parameters), and a work draws the three apart.
//
// WHAT IS COMPUTED is a functional of M. With w_b = μ_b q_b, what a cell brings:
//   demand          D_o = Σ_b w_b K(b, o), and D₀ for the outside option; Σ_o D_o + D₀ = Σ_b w_b (conservation, checked)
//   share           s_o = D_o / Σ_{o'} D_{o'}; penetration 1 − D₀ / Σ_b w_b
//   structure       a seller's demand is the sum over its offers; concentration is nomos's (HHI, CR_k, the numbers equivalent)
//   trade area      of o: the measure t_b = w_b K(b, o) on the buyers, of total D_o. It is a density, not a polygon: the polygons of
//                   practice are reductions of it. Its core at a share p is a set of least mass holding p of the custom: the cells in
//                   the order of K(b, o) q_b, the custom per buyer, the last taken in part (a fractional knapsack; §2, checked
//                   against every subset)
//   overlap         of o and o': Σ_b min(t_b / D_o, t'_b / D_{o'}) = 1 − the total variation distance of the two custom
//                   distributions; 1 when they draw from the same buyers in the same proportions, 0 when from none in common
//   unserved        u_b = w_b K(b, o₀), the spend of a cell that buys nothing; white space is where it is large
//
// SEGMENTS. A segmentation is a partition of the buyers. The market coarsened to its blocks has a block's mass, its mean spend, and
// the spend-weighted mean of its cells' rows; it has the same demands as the market, whatever the partition (§3). What a partition
// loses is the difference among the rows it puts together: with K̄ the row of the whole and K_β that of a block, the spend-weighted
// squared distances obey  total = within + between  (Pythagoras), within never rises when a partition is refined, and it is zero
// exactly when the kernel is constant on every block: then the segments are exact, for every question that weighs their buyers anew.
//
// WHAT IF. A table of shares is one kernel. Entry, exit and a change of attraction ask for a choice rule: a kernel for every set of
// offers. The rule here is the logit in its multiplicative form: each pair within reach has a weight a_bo > 0 (e^{utility}), the
// outside option a weight a_b0 ≥ 0, and K_S(b, o) = a_bo / (a_b0 + Σ_{o' ∈ S} a_bo') for o ∈ S. A cell with no offer of S within its
// reach spends nothing: that, too, is the outside option. The rule has the independence of irrelevant alternatives, cell by cell:
// removing o hands its custom to the others in proportion to their shares, so the diversion from o to o' is
//   δ_{o→o'} = D_o⁻¹ Σ_b w_b K(b, o) K(b, o') / (1 − K(b, o)),
// and Σ_{o'} δ_{o→o'} + δ_{o→o₀} = 1 (§4; checked against the removal itself). Across cells of different tastes the market as a whole
// does not have that independence: diversion follows who shares buyers with whom.

#pragma once

#include <nomos/measurement/concentration.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <numeric>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace markets::market {

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("markets::market: ") + what);
}
}  // namespace detail

enum class Provenance { observed, stated, presumed };

// The buyers: cells of mass μ, each buyer of a cell bringing the spend q.
struct Buyers {
  std::vector<double> mass, spend;
  std::size_t size() const { return mass.size(); }
  double brings(std::size_t b) const { return mass[b] * spend[b]; }  // w_b
  double total() const {
    double t = 0;
    for (std::size_t b = 0; b < size(); b++) t += brings(b);
    return t;
  }
};

// Rows of pairs (an offer, a number), one row a cell of buyers: a kernel's shares, a choice rule's weights.
struct Rows {
  std::size_t offers = 0;
  std::vector<std::size_t> start{0}, offer;
  std::vector<double> value;
  std::size_t buyers() const { return start.size() - 1; }
  void add(std::span<const std::size_t> o, std::span<const double> v) {
    detail::require(o.size() == v.size(), "a value for each offer of a row");
    offer.insert(offer.end(), o.begin(), o.end());
    value.insert(value.end(), v.begin(), v.end());
    start.push_back(offer.size());
  }
  double sum(std::size_t b) const {
    double t = 0;
    for (std::size_t k = start[b]; k < start[b + 1]; k++) t += value[k];
    return t;
  }
  double at(std::size_t b, std::size_t o) const {
    double t = 0;
    for (std::size_t k = start[b]; k < start[b + 1]; k++)
      if (offer[k] == o) t += value[k];
    return t;
  }
};

struct Kernel {
  Rows shares;
  Provenance provenance = Provenance::presumed;
  double outside(std::size_t b) const { return std::max(0.0, 1.0 - shares.sum(b)); }
};

struct Market {
  Buyers buyers;
  std::vector<std::size_t> seller;  // the seller of each offer
  Kernel kernel;
  std::size_t offers() const { return seller.size(); }
  std::size_t sellers() const { return seller.empty() ? 0 : *std::max_element(seller.begin(), seller.end()) + 1; }
};

// What is wrong with a market is refused, with the reason.
inline void validate(const Market& M, double tolerance = 1e-9) {
  const auto& K = M.kernel.shares;
  detail::require(M.buyers.mass.size() == M.buyers.spend.size(), "a mass and a spend for each cell of buyers");
  detail::require(K.buyers() == M.buyers.size(), "a row of the kernel for each cell of buyers");
  detail::require(K.offers == M.offers(), "the kernel's offers are the market's");
  for (std::size_t b = 0; b < M.buyers.size(); b++) {
    detail::require(M.buyers.mass[b] >= 0 && std::isfinite(M.buyers.mass[b]), "nonnegative, finite masses");
    detail::require(M.buyers.spend[b] >= 0 && std::isfinite(M.buyers.spend[b]), "nonnegative, finite spends");
    detail::require(K.sum(b) <= 1 + tolerance, "the shares of a cell sum to at most one");
  }
  for (std::size_t k = 0; k < K.offer.size(); k++) {
    detail::require(K.offer[k] < K.offers, "a share names an offer of the market");
    detail::require(K.value[k] >= 0 && std::isfinite(K.value[k]), "nonnegative, finite shares");
  }
}

// ---- demand, shares, structure ------------------------------------------------------------------------------------------------

struct Demand {
  std::vector<double> offers;
  double outside = 0, total = 0;  // D₀, and Σ_b w_b
  double bought() const { return total - outside; }
  double penetration() const { return total > 0 ? 1 - outside / total : 0; }
};

inline Demand demand(const Market& M) {
  const auto& K = M.kernel.shares;
  Demand D;
  D.offers.assign(M.offers(), 0.0);
  for (std::size_t b = 0; b < M.buyers.size(); b++) {
    const double w = M.buyers.brings(b);
    D.total += w;
    D.outside += w * M.kernel.outside(b);
    for (std::size_t k = K.start[b]; k < K.start[b + 1]; k++) D.offers[K.offer[k]] += w * K.value[k];
  }
  return D;
}

// s_o = D_o / Σ D: the shares of what is bought.
inline std::vector<double> shares(const Demand& D) { return nomos::measurement::shares(D.offers); }

inline std::vector<double> by_seller(const Market& M, std::span<const double> per_offer) {
  detail::require(per_offer.size() == M.offers(), "an amount for each offer");
  std::vector<double> out(M.sellers(), 0.0);
  for (std::size_t o = 0; o < M.offers(); o++) out[M.seller[o]] += per_offer[o];
  return out;
}

struct Structure {
  double hhi = 0, cr4 = 0, equivalent = 0;  // Σ s², the four largest shares, and 1 / HHI: as many equal sellers
  std::size_t sellers = 0;                  // those that sell something
};

inline Structure structure(std::span<const double> seller_demand) {
  Structure S;
  S.hhi = nomos::measurement::herfindahl_hirschman(seller_demand);
  S.cr4 = nomos::measurement::concentration_ratio(seller_demand, std::min<std::size_t>(4, seller_demand.size()));
  S.equivalent = 1 / S.hhi;
  for (double d : seller_demand) S.sellers += d > 0;
  return S;
}

// ---- trade areas ----------------------------------------------------------------------------------------------------------------

// t_b = w_b K(b, o): where the custom of o comes from. Its total is D_o.
inline std::vector<double> trade_area(const Market& M, std::size_t o) {
  detail::require(o < M.offers(), "an offer of the market");
  std::vector<double> t(M.buyers.size(), 0.0);
  for (std::size_t b = 0; b < t.size(); b++) t[b] = M.buyers.brings(b) * M.kernel.shares.at(b, o);
  return t;
}

// The core of a trade area at a share p: cells of least total mass holding p of the custom, the last taken in part.
struct Core {
  std::vector<std::size_t> cells;  // in the order taken
  std::vector<double> part;        // the fraction of each cell taken: 1, but for the last
  double mass = 0, custom = 0;     // the buyers within, and their custom
};

inline Core core(const Market& M, std::size_t o, double p) {
  detail::require(p >= 0 && p <= 1, "a share between 0 and 1");
  const auto t = trade_area(M, o);
  const double want = p * std::accumulate(t.begin(), t.end(), 0.0);
  std::vector<std::size_t> order;
  for (std::size_t b = 0; b < t.size(); b++)
    if (t[b] > 0) order.push_back(b);
  // The custom per buyer, K(b, o) q_b, falling; ties by the cell's index, so that the core is one.
  const auto density = [&](std::size_t b) { return t[b] / M.buyers.mass[b]; };
  std::sort(order.begin(), order.end(), [&](std::size_t a, std::size_t c) { return density(a) > density(c) || (density(a) == density(c) && a < c); });
  Core C;
  for (std::size_t b : order) {
    if (C.custom >= want) break;
    const double take = std::min(1.0, (want - C.custom) / t[b]);
    C.cells.push_back(b), C.part.push_back(take);
    C.custom += take * t[b], C.mass += take * M.buyers.mass[b];
  }
  return C;
}

// 1 − the total variation distance of the custom of two offers, as distributions over the buyers.
inline double overlap(const Market& M, std::size_t o, std::size_t other) {
  const auto t = trade_area(M, o), u = trade_area(M, other);
  const double a = std::accumulate(t.begin(), t.end(), 0.0), c = std::accumulate(u.begin(), u.end(), 0.0);
  detail::require(a > 0 && c > 0, "two offers that sell something");
  double s = 0;
  for (std::size_t b = 0; b < t.size(); b++) s += std::min(t[b] / a, u[b] / c);
  return s;
}

// u_b = w_b K(b, o₀): the spend of each cell that buys nothing.
inline std::vector<double> unserved(const Market& M) {
  std::vector<double> u(M.buyers.size(), 0.0);
  for (std::size_t b = 0; b < u.size(); b++) u[b] = M.buyers.brings(b) * M.kernel.outside(b);
  return u;
}

// ---- segments ---------------------------------------------------------------------------------------------------------------------

// The market coarsened to the blocks of a partition (block[b] < blocks): a block's mass, its mean spend, the spend-weighted mean
// of its cells' rows. Its demands are the market's.
inline Market coarsen(const Market& M, std::span<const std::size_t> block, std::size_t blocks) {
  detail::require(block.size() == M.buyers.size(), "a block for each cell of buyers");
  const auto& K = M.kernel.shares;
  std::vector<double> mass(blocks, 0.0), brings(blocks, 0.0);
  std::vector<std::vector<double>> row(blocks, std::vector<double>(M.offers(), 0.0));
  for (std::size_t b = 0; b < block.size(); b++) {
    detail::require(block[b] < blocks, "a block of the partition");
    const double w = M.buyers.brings(b);
    mass[block[b]] += M.buyers.mass[b], brings[block[b]] += w;
    for (std::size_t k = K.start[b]; k < K.start[b + 1]; k++) row[block[b]][K.offer[k]] += w * K.value[k];
  }
  Market C;
  C.seller = M.seller;
  C.kernel.provenance = M.kernel.provenance;
  C.kernel.shares.offers = M.offers();
  for (std::size_t g = 0; g < blocks; g++) {
    C.buyers.mass.push_back(mass[g]);
    C.buyers.spend.push_back(mass[g] > 0 ? brings[g] / mass[g] : 0.0);
    std::vector<std::size_t> o;
    std::vector<double> v;
    for (std::size_t j = 0; j < M.offers(); j++)
      if (row[g][j] > 0) o.push_back(j), v.push_back(row[g][j] / brings[g]);
    C.kernel.shares.add(o, v);
  }
  return C;
}

// How much the rows of the kernel differ, weighted by what each cell brings: Σ_b w_b ‖K_b − K̄‖² / Σ_b w_b in all, of which the part
// within the blocks of a partition and the part between them. Rows are taken with the outside option, as distributions.
struct Dispersion {
  double total = 0, within = 0, between = 0;
  double explained() const { return total > 0 ? between / total : 1; }
};

inline Dispersion dispersion(const Market& M, std::span<const std::size_t> block, std::size_t blocks) {
  detail::require(block.size() == M.buyers.size(), "a block for each cell of buyers");
  const auto& K = M.kernel.shares;
  const std::size_t n = M.offers() + 1;  // the last entry is the outside option
  std::vector<double> weight(blocks, 0.0), mean(n, 0.0);
  std::vector<std::vector<double>> centre(blocks, std::vector<double>(n, 0.0));
  double all = 0;
  const auto row = [&](std::size_t b) {
    std::vector<double> r(n, 0.0);
    for (std::size_t k = K.start[b]; k < K.start[b + 1]; k++) r[K.offer[k]] += K.value[k];
    r[n - 1] = M.kernel.outside(b);
    return r;
  };
  for (std::size_t b = 0; b < block.size(); b++) {
    detail::require(block[b] < blocks, "a block of the partition");
    const double w = M.buyers.brings(b);
    const auto r = row(b);
    all += w, weight[block[b]] += w;
    for (std::size_t j = 0; j < n; j++) centre[block[b]][j] += w * r[j], mean[j] += w * r[j];
  }
  Dispersion D;
  if (!(all > 0)) return D;
  for (double& v : mean) v /= all;
  for (std::size_t g = 0; g < blocks; g++)
    if (weight[g] > 0)
      for (double& v : centre[g]) v /= weight[g];
  const auto squared = [&](const std::vector<double>& a, const std::vector<double>& c) {
    double s = 0;
    for (std::size_t j = 0; j < n; j++) s += (a[j] - c[j]) * (a[j] - c[j]);
    return s;
  };
  for (std::size_t b = 0; b < block.size(); b++) {
    const double w = M.buyers.brings(b);
    const auto r = row(b);
    D.total += w * squared(r, mean), D.within += w * squared(r, centre[block[b]]);
  }
  for (std::size_t g = 0; g < blocks; g++) D.between += weight[g] * squared(centre[g], mean);
  D.total /= all, D.within /= all, D.between /= all;
  return D;
}

// ---- a choice rule: the logit, in weights -----------------------------------------------------------------------------------------

// a_bo > 0 for the pairs within reach, a_b0 ≥ 0 for the outside option of each cell.
struct Logit {
  Rows weights;
  std::vector<double> outside;
};

inline void validate(const Logit& R) {
  detail::require(R.outside.size() == R.weights.buyers(), "an outside weight for each cell of buyers");
  for (double a : R.outside) detail::require(a >= 0 && std::isfinite(a), "nonnegative, finite outside weights");
  for (std::size_t k = 0; k < R.weights.offer.size(); k++) {
    detail::require(R.weights.offer[k] < R.weights.offers, "a weight names an offer");
    detail::require(R.weights.value[k] > 0 && std::isfinite(R.weights.value[k]), "positive, finite weights");
  }
}

// The kernel of the rule when the offers of `active` are on the market (all of them when it is empty).
inline Kernel kernel(const Logit& R, const std::vector<bool>& active = {}) {
  detail::require(active.empty() || active.size() == R.weights.offers, "an answer for each offer, or none");
  const auto& W = R.weights;
  Kernel K;
  K.provenance = Provenance::presumed;
  K.shares.offers = W.offers;
  for (std::size_t b = 0; b < W.buyers(); b++) {
    double total = R.outside[b];
    for (std::size_t k = W.start[b]; k < W.start[b + 1]; k++)
      if (active.empty() || active[W.offer[k]]) total += W.value[k];
    std::vector<std::size_t> o;
    std::vector<double> v;
    if (total > 0)
      for (std::size_t k = W.start[b]; k < W.start[b + 1]; k++)
        if (active.empty() || active[W.offer[k]]) o.push_back(W.offer[k]), v.push_back(W.value[k] / total);
    K.shares.add(o, v);
  }
  return K;
}

// Where the custom of an offer goes when it leaves: to each other offer and to the outside option, as parts of what it sold.
struct Diversion {
  std::vector<double> to;
  double outside = 0, lost = 0;  // δ to not buying; D_o, what the offer sold
};

// By the removal itself: the demands with o on the market, and without it.
inline Diversion diversion(const Buyers& buyers, const std::vector<std::size_t>& seller, const Logit& R, std::size_t o, std::vector<bool> active = {}) {
  detail::require(o < R.weights.offers, "an offer of the rule");
  if (active.empty()) active.assign(R.weights.offers, true);
  detail::require(active[o], "an offer that is on the market");
  Market with{buyers, seller, kernel(R, active)};
  active[o] = false;
  Market without{buyers, seller, kernel(R, active)};
  const auto before = demand(with), after = demand(without);
  Diversion d;
  d.lost = before.offers[o];
  detail::require(d.lost > 0, "an offer that sells something");
  d.to.assign(R.weights.offers, 0.0);
  for (std::size_t j = 0; j < d.to.size(); j++)
    if (j != o) d.to[j] = (after.offers[j] - before.offers[j]) / d.lost;
  d.outside = (after.outside - before.outside) / d.lost;
  return d;
}

// By the closed form of the logit: δ_{o→o'} = D_o⁻¹ Σ_b w_b K(b, o) K(b, o') / (1 − K(b, o)).
inline Diversion diversion_closed(const Market& M, std::size_t o) {
  detail::require(o < M.offers(), "an offer of the market");
  const auto& K = M.kernel.shares;
  Diversion d;
  d.to.assign(M.offers(), 0.0);
  for (std::size_t b = 0; b < M.buyers.size(); b++) {
    const double w = M.buyers.brings(b), mine = K.at(b, o);
    if (!(w > 0) || !(mine > 0)) continue;
    d.lost += w * mine;
    // A cell that buys only o buys nothing when it leaves.
    if (mine >= 1) { d.outside += w * mine; continue; }
    for (std::size_t k = K.start[b]; k < K.start[b + 1]; k++)
      if (K.offer[k] != o) d.to[K.offer[k]] += w * mine * K.value[k] / (1 - mine);
    d.outside += w * mine * M.kernel.outside(b) / (1 - mine);
  }
  detail::require(d.lost > 0, "an offer that sells something");
  for (double& v : d.to) v /= d.lost;
  d.outside /= d.lost;
  return d;
}

}  // namespace markets::market
