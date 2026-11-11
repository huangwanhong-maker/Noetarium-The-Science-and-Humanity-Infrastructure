// Checks of markets in space (space.hpp).
//
// - Huff's model is the logit of α ln A − β ln t (or − β t): against the softmax computed apart.
// - Reilly's law is its two-offer case: at the breaking point a buyer splits evenly; the classic numbers (attractions 4 to 1, 30
//   apart) give 20 from the larger; the exponential form has its own breaking point.
// - As β grows with equal attractions, every buyer goes to the nearest outlet.
// - The reach of an outlet: its custom is its demand, the mean cost is the custom-weighted mean, the cost holding a share rises with
//   the share to the farthest buyer.
// - A new outlet: its demand is what it draws from each rival, from its own seller and from those who bought nothing, exactly; each
//   part is what the rival loses when the outlet is added to the rule; a cell with nothing within reach is drawn from not buying.
// - The seller's demand F is monotone and submodular in the set of sites; greedy gains at least 1 − 1/e of the best k sites' gain,
//   against every set of three among ten; its first site is the one of greatest gain on entering.

#include <markets/space/space.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace mm = markets::market;
namespace sp = markets::space;

namespace {

struct Town {
  mm::Buyers buyers;
  std::vector<double> bx, by, ox, oy, attraction;
  std::vector<std::size_t> seller;
  std::vector<sp::Reachable> pairs;
  std::vector<sp::Site> sites;
};

// Cells of buyers and outlets scattered on a square of side 10, the pairs within a reach of 4; sites with their own reach.
Town town(calx::random::Stream& rng, std::size_t n, std::size_t m, std::size_t sellers, std::size_t sites) {
  Town T;
  for (std::size_t b = 0; b < n; b++) {
    T.bx.push_back(rng.uniform(0, 10)), T.by.push_back(rng.uniform(0, 10));
    T.buyers.mass.push_back(rng.uniform(5, 60)), T.buyers.spend.push_back(rng.uniform(2, 12));
  }
  for (std::size_t o = 0; o < m; o++) {
    T.ox.push_back(rng.uniform(0, 10)), T.oy.push_back(rng.uniform(0, 10));
    T.attraction.push_back(rng.uniform(0.5, 4)), T.seller.push_back(o % sellers);
    for (std::size_t b = 0; b < n; b++) {
      const double d = std::hypot(T.bx[b] - T.ox[o], T.by[b] - T.oy[o]);
      if (d <= 4) T.pairs.push_back({b, o, d});
    }
  }
  for (std::size_t c = 0; c < sites; c++) {
    sp::Site site;
    site.attraction = rng.uniform(0.5, 4);
    const double x = rng.uniform(0, 10), y = rng.uniform(0, 10);
    for (std::size_t b = 0; b < n; b++) {
      const double d = std::hypot(T.bx[b] - x, T.by[b] - y);
      if (d <= 4) site.reach.push_back({b, d});
    }
    T.sites.push_back(site);
  }
  return T;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(2026, 11, 11, 3);

  // ---- Huff's model as a logit ----
  for (const auto decay : {sp::Decay::power, sp::Decay::exponential}) {
    const auto T = town(rng, 40, 8, 3, 0);
    sp::Huff h;
    h.alpha = 0.8, h.beta = decay == sp::Decay::power ? 1.7 : 0.6, h.decay = decay, h.floor = 0.05;
    std::vector<double> outside(40);
    for (double& a : outside) a = rng.uniform(0, 0.3);
    const auto K = mm::kernel(sp::rule(h, 40, T.attraction, T.pairs, outside));
    double worst = 0;
    for (std::size_t b = 0; b < 40; b++) {
      double total = outside[b];
      std::vector<double> e(8, 0.0);
      for (const auto& p : T.pairs)
        if (p.demand == b) {
          const double t = decay == sp::Decay::power ? std::max(p.cost, h.floor) : p.cost;
          e[p.supply] = std::exp(h.alpha * std::log(T.attraction[p.supply]) - h.beta * (decay == sp::Decay::power ? std::log(t) : t));
          total += e[p.supply];
        }
      for (std::size_t o = 0; o < 8; o++) worst = std::max(worst, std::fabs(K.shares.at(b, o) - (total > 0 ? e[o] / total : 0)));
    }
    s.check(decay == sp::Decay::power ? "Huff's model is the logit of α ln A − β ln t" : "... and in its exponential form of α ln A − β t", worst, 1e-13);
  }
  {
    sp::Huff h;
    h.floor = 2;
    s.check("below the floor nearer is no better: a weight is finite at the door", sp::weight(h, 3, 0) - sp::weight(h, 3, 2), 0);
    s.check("... and beyond it falls as the power", sp::weight(h, 3, 4) / sp::weight(h, 3, 2) - 0.25, 1e-15);
  }

  // ---- Reilly's law ----
  {
    sp::Huff h;  // α = 1, β = 2, the power form
    h.floor = 1e-9;
    const double x = sp::breaking_point(h, 4, 1, 30);
    s.check("Reilly's law: attractions 4 to 1, 30 apart, the breaking point 20 from the larger", x - 20, 1e-13);
    const std::vector<double> A{4, 1};
    const std::vector<sp::Reachable> at{{0, 0, x}, {0, 1, 30 - x}}, nearer{{0, 0, x - 1}, {0, 1, 31 - x}};
    s.check("... where a buyer splits evenly between the two", mm::kernel(sp::rule(h, 1, A, at)).shares.at(0, 0) - 0.5, 1e-13);
    s.require("... and nearer the larger, goes more to it", mm::kernel(sp::rule(h, 1, A, nearer)).shares.at(0, 0) > 0.5);
    sp::Huff g;
    g.alpha = 0.7, g.beta = 2.6, g.floor = 1e-9;
    const double y = sp::breaking_point(g, 2.5, 6, 12);
    const std::vector<double> B{2.5, 6};
    s.check("... for any α and β: d / (1 + (A₂/A₁)^{α/β})", mm::kernel(sp::rule(g, 1, B, std::vector<sp::Reachable>{{0, 0, y}, {0, 1, 12 - y}})).shares.at(0, 0) - 0.5, 1e-13);
    sp::Huff e;
    e.decay = sp::Decay::exponential, e.alpha = 1.2, e.beta = 0.3;
    const double z = sp::breaking_point(e, 5, 2, 10);
    s.check("the exponential form breaks at d/2 + α ln(A₁/A₂) / (2β)", mm::kernel(sp::rule(e, 1, std::vector<double>{5, 2}, std::vector<sp::Reachable>{{0, 0, z}, {0, 1, 10 - z}})).shares.at(0, 0) - 0.5, 1e-13);
  }

  // ---- the nearest, as β grows ----
  {
    // Costs apart by a factor of 1.3 at least, so that the limit is sharp.
    std::vector<sp::Reachable> pairs;
    for (std::size_t b = 0; b < 30; b++) {
      std::vector<double> costs;
      double c = rng.uniform(0.5, 2);
      for (std::size_t o = 0; o < 5; o++) costs.push_back(c), c *= rng.uniform(1.3, 2);
      for (std::size_t o = 0; o < 5; o++) pairs.push_back({b, (o + b) % 5, costs[o]});
    }
    sp::Huff h;
    h.beta = 150, h.floor = 1e-9;
    const std::vector<double> equal(5, 2.0);
    const auto K = mm::kernel(sp::rule(h, 30, equal, pairs)), N = sp::nearest(30, 5, pairs);
    double worst = 0;
    for (std::size_t b = 0; b < 30; b++)
      for (std::size_t o = 0; o < 5; o++) worst = std::max(worst, std::fabs(K.shares.at(b, o) - N.shares.at(b, o)));
    s.check("as β grows with equal attractions every buyer goes to the nearest outlet", worst, 1e-12);
    s.require("a cell with nothing within reach has no nearest outlet", sp::nearest(31, 5, pairs).outside(30) == 1);
  }

  // ---- the reach of an outlet ----
  {
    const auto T = town(rng, 60, 6, 2, 0);
    sp::Huff h;
    h.floor = 0.1;
    mm::Market M{T.buyers, T.seller, mm::kernel(sp::rule(h, 60, T.attraction, T.pairs))};
    const auto D = mm::demand(M);
    const auto r = sp::reach(M, T.pairs, 2);
    const auto t = mm::trade_area(M, 2);
    double mean = 0, farthest = 0;
    for (const auto& p : T.pairs)
      if (p.supply == 2) mean += t[p.demand] * p.cost, farthest = std::max(farthest, p.cost);
    s.check("the custom within an outlet's reach is its demand", r.custom / D.offers[2] - 1, 1e-13);
    s.check("the mean cost its custom bears is the custom-weighted mean", r.mean / (mean / D.offers[2]) - 1, 1e-13);
    s.require("the cost holding a share of the custom rises with the share, to the farthest buyer",
              r.within(0.25) <= r.within(0.5) && r.within(0.5) <= r.within(0.9) && r.within(1.0) == farthest && r.within(0.5) <= r.mean * 3);
    double held = 0;
    for (const auto& [cost, part] : r.by_cost)
      if (cost <= r.within(0.6)) held += part;
    s.require("... and holds at least that share", held >= 0.6 * r.custom - 1e-9);
  }

  // ---- a new outlet ----
  {
    const auto T = town(rng, 80, 7, 3, 6);
    sp::Huff h;
    h.alpha = 1.1, h.beta = 1.8, h.floor = 0.1;
    std::vector<double> outside(80);
    for (double& a : outside) a = rng.uniform(0, 0.2);
    const auto R = sp::rule(h, 80, T.attraction, T.pairs, outside);
    const auto before = mm::demand(mm::Market{T.buyers, T.seller, mm::kernel(R)});
    double adds = 0, loses = 0, sells = 0, own = 0, gains = 0;
    for (std::size_t c = 0; c < T.sites.size(); c++) {
      const std::size_t seller = c % 3;
      const auto e = sp::enter(T.buyers, T.seller, R, h, T.sites[c], seller);
      adds = std::max(adds, std::fabs(std::accumulate(e.from.begin(), e.from.end(), 0.0) + e.from_outside - e.demand) / e.demand);
      // The outlet added to the rule as an eighth offer: what it sells, and what each of the others then sells.
      auto attraction = T.attraction;
      attraction.push_back(T.sites[c].attraction);
      auto pairs = T.pairs;
      for (const auto& [b, cost] : T.sites[c].reach) pairs.push_back({b, 7, cost});
      auto sellers = T.seller;
      sellers.push_back(seller);
      const auto after = mm::demand(mm::Market{T.buyers, sellers, mm::kernel(sp::rule(h, 80, attraction, pairs, outside))});
      sells = std::max(sells, std::fabs(after.offers[7] - e.demand) / e.demand);
      for (std::size_t o = 0; o < 7; o++) loses = std::max(loses, std::fabs(before.offers[o] - after.offers[o] - e.from[o]) / e.demand);
      loses = std::max(loses, std::fabs(before.outside - after.outside - e.from_outside) / e.demand);
      double mine = 0;
      for (std::size_t o = 0; o < 7; o++)
        if (T.seller[o] == seller) mine += e.from[o];
      own = std::max(own, std::fabs(mine - e.cannibalised) / e.demand);
      const std::size_t one[1] = {c};
      gains = std::max(gains, std::fabs(sp::captured(T.buyers, T.seller, R, h, T.sites, one, seller) - sp::captured(T.buyers, T.seller, R, h, T.sites, {}, seller) - e.gain()) / e.demand);
    }
    s.check("a new outlet's demand is what it draws from each offer and from those who bought nothing, exactly", adds, 1e-13);
    s.check("... and is what it sells when it is added to the rule", sells, 1e-12);
    s.check("... each part what that offer, or not buying, then loses", loses, 1e-12);
    s.check("what it takes from its seller's own outlets is the cannibalisation", own, 1e-13);
    s.check("... and the seller gains the demand less that", gains, 1e-12);
  }
  {
    // A cell that had nothing within reach: an outlet there draws only from not buying.
    mm::Buyers buyers{{10.0, 20.0}, {3.0, 3.0}};
    sp::Huff h;
    const std::vector<double> A{1.0};
    const std::vector<sp::Reachable> pairs{{0, 0, 2.0}};
    const auto R = sp::rule(h, 2, A, pairs);
    sp::Site site;
    site.reach = {{1, 1.5}};
    const auto e = sp::enter(buyers, {0}, R, h, site, 0);
    s.check("an outlet where nothing was within reach sells to the whole of the cell, all of it from not buying", std::fabs(e.demand - 60) + std::fabs(e.from_outside - 60) + std::fabs(e.cannibalised), 1e-13);
  }

  // ---- where to open ----
  {
    const auto T = town(rng, 70, 6, 3, 10);
    sp::Huff h;
    h.beta = 1.5, h.floor = 0.1;
    std::vector<double> outside(70, 0.05);
    const auto R = sp::rule(h, 70, T.attraction, T.pairs, outside);
    const std::size_t seller = 1;
    const auto F = [&](const std::vector<std::size_t>& S) { return sp::captured(T.buyers, T.seller, R, h, T.sites, S, seller); };
    // Monotone and submodular: over sets S ⊂ U and a site c outside U.
    bool monotone = true, submodular = true;
    for (int trial = 0; trial < 200; trial++) {
      std::vector<std::size_t> S, U;
      const std::size_t c = rng.below(10);
      for (std::size_t k = 0; k < 10; k++) {
        if (k == c) continue;
        const double u = rng.uniform();
        if (u < 0.3) S.push_back(k), U.push_back(k);
        else if (u < 0.6) U.push_back(k);
      }
      auto Sc = S, Uc = U;
      Sc.push_back(c), Uc.push_back(c);
      // The demands are sums of some ten thousand; the differences are compared within their rounding.
      const double eps = 1e-12 * F(Uc);
      monotone = monotone && F(U) >= F(S) - eps && F(Sc) >= F(S) - eps;
      submodular = submodular && F(Sc) - F(S) >= F(Uc) - F(U) - eps;
    }
    s.require("the seller's demand does not fall when a site is added", monotone);
    s.require("... and a site adds less to a larger set of sites than to a smaller: the demand is submodular", submodular);
    // Greedy against every three of the ten.
    const auto G = sp::choose(T.buyers, T.seller, R, h, T.sites, 3, seller);
    double best = 0;
    for (std::size_t a = 0; a < 10; a++)
      for (std::size_t b = a + 1; b < 10; b++)
        for (std::size_t c = b + 1; c < 10; c++) best = std::max(best, F({a, b, c}));
    const double ratio = (G.after - G.before) / (best - G.before);
    s.check("the demand after the sites chosen is the demand with them on the market", G.after / F(G.sites) - 1, 1e-13);
    s.check("... the gains of the three adding to it", (G.before + G.gains[0] + G.gains[1] + G.gains[2]) / G.after - 1, 1e-13);
    s.require("choosing greedily gains at least 1 − 1/e of what the best three of ten gain", ratio >= 1 - std::exp(-1.0) && ratio <= 1 + 1e-12, "the ratio is " + std::to_string(ratio));
    s.require("... each site adding no more than the one before", G.gains[0] >= G.gains[1] - 1e-12 && G.gains[1] >= G.gains[2] - 1e-12);
    std::size_t first = 0;
    double most = -1;
    for (std::size_t c = 0; c < 10; c++) {
      const double g = sp::enter(T.buyers, T.seller, R, h, T.sites[c], seller).gain();
      if (g > most) most = g, first = c;
    }
    s.require("its first site is the one of greatest gain on entering", G.sites[0] == first && std::fabs(G.gains[0] - most) < 1e-9 * most);
    s.require("no sites chosen changes nothing", sp::choose(T.buyers, T.seller, R, h, T.sites, 0, seller).after == G.before);
  }

  return s.done();
}
