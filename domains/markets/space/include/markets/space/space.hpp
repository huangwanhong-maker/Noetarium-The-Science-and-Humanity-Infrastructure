// Markets in space (the domain's theory note, §6–§8).
//
// The domain has no map of its own. What separates a buyer from an outlet is a cost (minutes, metres, money) of the pairs within
// reach, which a work computes (on urban science's `access` for streets, on a great circle for regions) and gives as a sparse list:
// nomos's `Reachable {demand, supply, cost}`, a cell of buyers, an offer and the cost between them.
//
// HUFF'S MODEL (Huff 1963, Land Economics 39, 81–90; 1964, J. Marketing 28, 34–38) is the logit of market.hpp with the weights
//   a_bo = A_o^α f(t_bo),   f(t) = max(t, t₀)^{−β}  (the power of the original)  or  e^{−β t},
// A_o the attraction of an outlet (its floor space, its seats), t₀ a floor below which nearer is no better, so that a weight is
// finite at the door. In utilities it is α ln A_o − β ln t, or α ln A_o − β t: the conditional logit, which nomos's `econometrics`
// estimates from choices. A cell with nothing within reach buys nothing.
//
//   Reilly's law      two centres a distance d apart, the buyers on the line between them choosing by the power form: the point of
//                     equal shares lies d / (1 + (A₂ / A₁)^{α/β}) from the first (Reilly 1931 and Converse 1949, with α = 1, β = 2);
//                     for the exponential form, d/2 + α ln(A₁ / A₂) / (2β). It is the two-offer case of Huff's model (§6, checked)
//   the nearest       as β → ∞ with equal attractions every buyer goes to the nearest outlet: the cells of Thiessen (§6, checked)
//
// THE REACH OF AN OUTLET reduces its trade area to numbers: the mean cost its custom bears, and the cost within which a share p of its
// custom lies (a quantile weighted by custom).
//
// A NEW OUTLET (§7). At a site with attraction A and costs to the cells within its reach, with u_b its weight for cell b and W_b the
// weights already there (the outside option's among them), it takes the share u_b / (W_b + u_b) of each cell and every other offer
// loses that share of its own custom there, the outside option too. So its demand is drawn  from each rival, from its seller's own
// outlets (cannibalisation) and from those who bought nothing, and the parts add up to it exactly (checked). What the seller gains
// is the demand less what it took from itself.
//
// WHERE TO OPEN (§8). To choose k sites of a list for a seller, so that the seller's demand is greatest. With own_b the seller's
// weight in a cell, the demand is F(S) = Σ_b w_b (own_b + u_b(S)) / (W_b + u_b(S)), u_b(S) the sum of the weights of the sites of S:
// a concave, increasing function of a sum, in every cell. So F is monotone and submodular, and choosing greedily, the site of
// greatest gain each time, gains at least 1 − 1/e of what the best k sites gain (Nemhauser, Wolsey and Fisher 1978, Math. Programming
// 14, 265–294). The best set is found by trying all for small cases (checked: the bound holds, and greedy is usually the best).

#pragma once

#include <markets/market/market.hpp>

#include <nomos/measurement/accessibility.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace markets::space {

using nomos::measurement::Reachable;

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("markets::space: ") + what);
}
}  // namespace detail

enum class Decay { power, exponential };

struct Huff {
  double alpha = 1;  // the exponent of attraction
  double beta = 2;   // of the decay: t^{−β}, or e^{−β t}
  Decay decay = Decay::power;
  double floor = 1;  // the power form: a cost below it counts as it
};

// a = A^α f(t).
inline double weight(const Huff& h, double attraction, double cost) {
  detail::require(attraction > 0 && std::isfinite(attraction), "a positive, finite attraction");
  detail::require(cost >= 0 && std::isfinite(cost), "a nonnegative, finite cost");
  detail::require(h.beta >= 0 && (h.decay == Decay::exponential || h.floor > 0), "a decay that does not rise, and a positive floor");
  const double f = h.decay == Decay::power ? std::pow(std::max(cost, h.floor), -h.beta) : std::exp(-h.beta * cost);
  return std::pow(attraction, h.alpha) * f;
}

// The choice rule of Huff's model: the pairs within reach, in rows by cell. `outside` is the weight of not buying, a cell each (none
// when empty).
inline market::Logit rule(const Huff& h, std::size_t buyers, std::span<const double> attraction, std::span<const Reachable> pairs, std::span<const double> outside = {}) {
  detail::require(outside.empty() || outside.size() == buyers, "an outside weight for each cell, or none");
  std::vector<std::vector<std::pair<std::size_t, double>>> rows(buyers);
  for (const Reachable& p : pairs) {
    detail::require(p.demand < buyers && p.supply < attraction.size(), "a pair names a cell of buyers and an offer");
    rows[p.demand].push_back({p.supply, weight(h, attraction[p.supply], p.cost)});
  }
  market::Logit R;
  R.weights.offers = attraction.size();
  R.outside.assign(buyers, 0.0);
  for (std::size_t b = 0; b < buyers; b++) {
    if (!outside.empty()) R.outside[b] = outside[b];
    std::vector<std::size_t> o;
    std::vector<double> v;
    for (const auto& [offer, a] : rows[b]) o.push_back(offer), v.push_back(a);
    R.weights.add(o, v);
  }
  market::validate(R);
  return R;
}

// The point of equal shares between two centres a distance d apart, from the first.
inline double breaking_point(const Huff& h, double a1, double a2, double d) {
  detail::require(a1 > 0 && a2 > 0 && d > 0 && h.beta > 0, "two attractions, a distance and a decay, all positive");
  if (h.decay == Decay::power) return d / (1 + std::pow(a2 / a1, h.alpha / h.beta));
  return std::clamp(d / 2 + h.alpha * std::log(a1 / a2) / (2 * h.beta), 0.0, d);
}

// Every cell to the offer of least cost within its reach (the least index among equals): the limit of Huff's model as β → ∞.
inline market::Kernel nearest(std::size_t buyers, std::size_t offers, std::span<const Reachable> pairs) {
  std::vector<std::size_t> best(buyers, offers);
  std::vector<double> cost(buyers, std::numeric_limits<double>::infinity());
  for (const Reachable& p : pairs) {
    detail::require(p.demand < buyers && p.supply < offers, "a pair names a cell of buyers and an offer");
    if (p.cost < cost[p.demand] || (p.cost == cost[p.demand] && p.supply < best[p.demand])) cost[p.demand] = p.cost, best[p.demand] = p.supply;
  }
  market::Kernel K;
  K.shares.offers = offers;
  for (std::size_t b = 0; b < buyers; b++) {
    if (best[b] == offers) K.shares.add({}, {});
    else {
      const std::size_t o[1] = {best[b]};
      const double one[1] = {1.0};
      K.shares.add(o, one);
    }
  }
  return K;
}

// ---- the reach of an outlet ----------------------------------------------------------------------------------------------------

struct Reach {
  double custom = 0, mean = 0;                           // D_o, and the mean cost its custom bears
  std::vector<std::pair<double, double>> by_cost;        // (cost, custom) of the pairs that give it custom, by rising cost
  // The cost within which the share p of the custom lies.
  double within(double p) const {
    detail::require(p >= 0 && p <= 1 && custom > 0, "a share between 0 and 1 of an outlet that sells something");
    double seen = 0;
    for (const auto& [cost, part] : by_cost) {
      seen += part;
      if (seen >= p * custom - 1e-12 * custom) return cost;
    }
    return by_cost.back().first;
  }
};

inline Reach reach(const market::Market& M, std::span<const Reachable> pairs, std::size_t o) {
  detail::require(o < M.offers(), "an offer of the market");
  Reach r;
  for (const Reachable& p : pairs) {
    if (p.supply != o) continue;
    const double part = M.buyers.brings(p.demand) * M.kernel.shares.at(p.demand, o);
    if (part > 0) r.by_cost.push_back({p.cost, part}), r.custom += part, r.mean += part * p.cost;
  }
  if (r.custom > 0) r.mean /= r.custom;
  std::sort(r.by_cost.begin(), r.by_cost.end());
  return r;
}

// ---- a new outlet ------------------------------------------------------------------------------------------------------------------

// A place where an outlet could stand: its attraction, and the cells within its reach with their costs.
struct Site {
  double attraction = 1;
  std::vector<std::pair<std::size_t, double>> reach;  // (a cell of buyers, the cost)
};

struct Entry {
  double demand = 0;         // what the new outlet would sell
  std::vector<double> from;  // drawn from each offer on the market
  double from_outside = 0;   // from those who bought nothing
  double cannibalised = 0;   // from the seller's own offers
  double gain() const { return demand - cannibalised; }
};

// What an outlet at the site would sell, and from whom, were it the seller's: by the logit's closed form.
inline Entry enter(const market::Buyers& buyers, const std::vector<std::size_t>& seller_of, const market::Logit& R, const Huff& h, const Site& site, std::size_t seller) {
  const auto& W = R.weights;
  detail::require(W.buyers() == buyers.size() && seller_of.size() == W.offers, "the rule's cells and offers are the market's");
  Entry e;
  e.from.assign(W.offers, 0.0);
  for (const auto& [b, cost] : site.reach) {
    detail::require(b < buyers.size(), "a site reaches cells of the market");
    const double u = weight(h, site.attraction, cost), w = buyers.brings(b);
    const double total = R.outside[b] + W.sum(b), share = u / (total + u);
    e.demand += w * share;
    if (!(total > 0)) { e.from_outside += w * share; continue; }  // a cell that had nothing within reach
    for (std::size_t k = W.start[b]; k < W.start[b + 1]; k++) e.from[W.offer[k]] += w * share * W.value[k] / total;
    e.from_outside += w * share * R.outside[b] / total;
  }
  for (std::size_t o = 0; o < W.offers; o++)
    if (seller_of[o] == seller) e.cannibalised += e.from[o];
  return e;
}

// ---- where to open -------------------------------------------------------------------------------------------------------------------

// The seller's demand with outlets at the sites `chosen` added to the market: F(S).
inline double captured(const market::Buyers& buyers, const std::vector<std::size_t>& seller_of, const market::Logit& R, const Huff& h, std::span<const Site> sites,
                       std::span<const std::size_t> chosen, std::size_t seller) {
  const auto& W = R.weights;
  std::vector<double> total(buyers.size(), 0.0), own(buyers.size(), 0.0);
  for (std::size_t b = 0; b < buyers.size(); b++) {
    total[b] = R.outside[b];
    for (std::size_t k = W.start[b]; k < W.start[b + 1]; k++) {
      total[b] += W.value[k];
      if (seller_of[W.offer[k]] == seller) own[b] += W.value[k];
    }
  }
  for (std::size_t c : chosen) {
    detail::require(c < sites.size(), "a site of the list");
    for (const auto& [b, cost] : sites[c].reach) {
      const double u = weight(h, sites[c].attraction, cost);
      total[b] += u, own[b] += u;
    }
  }
  double F = 0;
  for (std::size_t b = 0; b < buyers.size(); b++)
    if (total[b] > 0) F += buyers.brings(b) * own[b] / total[b];
  return F;
}

struct Chosen {
  std::vector<std::size_t> sites;  // in the order chosen
  std::vector<double> gains;       // what each added to the seller's demand when it was chosen
  double before = 0, after = 0;    // the seller's demand without them, and with them
};

// k sites chosen greedily: each time the site that adds most to the seller's demand, the least index among equals.
inline Chosen choose(const market::Buyers& buyers, const std::vector<std::size_t>& seller_of, const market::Logit& R, const Huff& h, std::span<const Site> sites, std::size_t k,
                     std::size_t seller) {
  detail::require(k <= sites.size(), "no more sites than the list holds");
  const auto& W = R.weights;
  // The weights in each cell, all and the seller's own, as the sites chosen so far leave them; a site's gain is a sum over its reach.
  std::vector<double> total(buyers.size(), 0.0), own(buyers.size(), 0.0);
  for (std::size_t b = 0; b < buyers.size(); b++) {
    total[b] = R.outside[b];
    for (std::size_t i = W.start[b]; i < W.start[b + 1]; i++) {
      total[b] += W.value[i];
      if (seller_of[W.offer[i]] == seller) own[b] += W.value[i];
    }
  }
  const auto share = [&](std::size_t b) { return total[b] > 0 ? own[b] / total[b] : 0.0; };
  // A site's weights by cell (a cell named twice counts with both).
  std::vector<std::vector<std::pair<std::size_t, double>>> adds(sites.size());
  for (std::size_t c = 0; c < sites.size(); c++)
    for (const auto& [b, cost] : sites[c].reach) {
      detail::require(b < buyers.size(), "a site reaches cells of the market");
      const double u = weight(h, sites[c].attraction, cost);
      const auto at = std::find_if(adds[c].begin(), adds[c].end(), [&](const auto& p) { return p.first == b; });
      if (at == adds[c].end()) adds[c].push_back({b, u});
      else at->second += u;
    }
  Chosen C;
  for (std::size_t b = 0; b < buyers.size(); b++) C.before += buyers.brings(b) * share(b);
  C.after = C.before;
  std::vector<bool> taken(sites.size(), false);
  for (std::size_t round = 0; round < k; round++) {
    std::size_t best = sites.size();
    double most = -1;
    for (std::size_t c = 0; c < sites.size(); c++) {
      if (taken[c]) continue;
      double gain = 0;
      for (const auto& [b, u] : adds[c]) gain += buyers.brings(b) * ((own[b] + u) / (total[b] + u) - share(b));
      if (gain > most) most = gain, best = c;
    }
    taken[best] = true;
    for (const auto& [b, u] : adds[best]) total[b] += u, own[b] += u;
    C.sites.push_back(best), C.gains.push_back(most), C.after += most;
  }
  return C;
}

}  // namespace markets::space
