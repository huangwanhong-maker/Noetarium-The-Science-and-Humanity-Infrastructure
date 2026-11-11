// Checks of a market and what is computed from it (market.hpp).
//
// - Conservation: the demands of the offers and of the outside option add to what the buyers bring; a trade area's total is its
//   offer's demand; the unserved spends add to the outside option's.
// - Structure through nomos: n equal sellers have HHI 1/n and are n equivalent sellers; one seller, 1.
// - The core of a trade area holds the share asked for, and no set of cells (the last in part) of less mass does: against every subset.
// - Overlap is 1 − the total variation distance; 1 with itself, 0 for offers with no buyers in common, symmetric.
// - A coarsened market has the market's demands, whatever the partition; total = within + between; refining a partition never raises
//   the dispersion within, which is zero for the partition into cells and for a kernel constant on the blocks.
// - The logit: a cell with nothing within reach buys nothing; within a cell the ratio of two shares does not change when a third
//   offer leaves (IIA); diversion by the removal itself is the closed form, and its parts add to one; with buyers of one taste it is
//   in proportion to shares, and with buyers of different tastes it is not.
// - What is not a market is refused.

#include <markets/market/market.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

namespace mm = markets::market;

namespace {

// A market of n cells and m offers of `sellers` sellers: each cell gives shares to some of the offers and keeps a part unspent.
mm::Market random_market(calx::random::Stream& rng, std::size_t n, std::size_t m, std::size_t sellers) {
  mm::Market M;
  for (std::size_t o = 0; o < m; o++) M.seller.push_back(o % sellers);
  M.kernel.shares.offers = m;
  for (std::size_t b = 0; b < n; b++) {
    M.buyers.mass.push_back(rng.uniform(1, 50));
    M.buyers.spend.push_back(rng.uniform(5, 40));
    std::vector<std::size_t> o;
    std::vector<double> v;
    double total = rng.uniform(0.0, 0.6);  // the outside option's part, before scaling
    for (std::size_t j = 0; j < m; j++)
      if (rng.uniform() < 0.6) o.push_back(j), v.push_back(rng.uniform(0.05, 1)), total += v.back();
    for (double& x : v) x /= total;
    M.kernel.shares.add(o, v);
  }
  return M;
}

mm::Logit random_rule(calx::random::Stream& rng, std::size_t n, std::size_t m, bool outside) {
  mm::Logit R;
  R.weights.offers = m;
  for (std::size_t b = 0; b < n; b++) {
    std::vector<std::size_t> o;
    std::vector<double> v;
    for (std::size_t j = 0; j < m; j++)
      if (rng.uniform() < 0.7) o.push_back(j), v.push_back(rng.uniform(0.1, 3));
    R.weights.add(o, v);
    R.outside.push_back(outside ? rng.uniform(0.0, 1.5) : 0.0);
  }
  return R;
}

bool refuses(const mm::Market& M) {
  try {
    mm::validate(M);
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(2026, 11, 11, 1);

  // ---- demand, shares, structure ----
  {
    const auto M = random_market(rng, 40, 7, 3);
    mm::validate(M);
    const auto D = mm::demand(M);
    s.check("the demands of the offers and of not buying add to what the buyers bring", (std::accumulate(D.offers.begin(), D.offers.end(), 0.0) + D.outside) / D.total - 1, 1e-13);
    s.check("... which is Σ μ q", D.total / M.buyers.total() - 1, 1e-13);
    const auto sh = mm::shares(D);
    s.check("the shares of what is bought add to one", std::accumulate(sh.begin(), sh.end(), 0.0) - 1, 1e-13);
    s.check("penetration is the part of the spend that buys", D.penetration() - D.bought() / D.total, 1e-13);
    const auto by = mm::by_seller(M, D.offers);
    s.check("the sellers' demands add to the offers'", std::accumulate(by.begin(), by.end(), 0.0) / D.bought() - 1, 1e-13);
    double worst = 0, unserved = 0;
    for (std::size_t o = 0; o < M.offers(); o++) {
      const auto t = mm::trade_area(M, o);
      worst = std::max(worst, std::fabs(std::accumulate(t.begin(), t.end(), 0.0) - D.offers[o]) / D.total);
    }
    for (double u : mm::unserved(M)) unserved += u;
    s.check("a trade area's total is its offer's demand", worst, 1e-13);
    s.check("the unserved spends add to the outside option's demand", unserved / D.outside - 1, 1e-13);
  }
  {
    const std::vector<double> equal(8, 3.5), one{0, 12, 0};
    const auto E = mm::structure(equal), O = mm::structure(one);
    s.check("eight equal sellers: HHI 1/8", E.hhi - 0.125, 1e-15);
    s.check("... as many equivalent sellers as there are", E.equivalent - 8, 1e-12);
    s.check("... the four largest hold a half", E.cr4 - 0.5, 1e-15);
    s.require("one seller: HHI 1, one equivalent seller, one that sells", O.hhi == 1 && O.equivalent == 1 && O.sellers == 1);
  }

  // ---- the core of a trade area ----
  {
    double worst_share = 0, worst_mass = 0;
    for (int trial = 0; trial < 40; trial++) {
      const auto M = random_market(rng, 9, 3, 2);
      const double p = rng.uniform(0.1, 0.95);
      const auto t = mm::trade_area(M, 0);
      const double D = std::accumulate(t.begin(), t.end(), 0.0), want = p * D;
      const auto C = mm::core(M, 0, p);
      worst_share = std::max(worst_share, std::fabs(C.custom - want) / D);
      // Every set of cells taken whole, and one more taken in part to make up the share: the least mass among them.
      double least = INFINITY;
      const std::size_t n = M.buyers.size();
      for (std::size_t set = 0; set < (std::size_t{1} << n); set++) {
        double custom = 0, mass = 0;
        for (std::size_t b = 0; b < n; b++)
          if (set >> b & 1) custom += t[b], mass += M.buyers.mass[b];
        if (custom > want + 1e-12) continue;
        if (custom >= want - 1e-12) { least = std::min(least, mass); continue; }
        for (std::size_t b = 0; b < n; b++)
          if (!(set >> b & 1) && t[b] > 0 && (want - custom) <= t[b]) least = std::min(least, mass + (want - custom) / t[b] * M.buyers.mass[b]);
      }
      worst_mass = std::max(worst_mass, std::fabs(C.mass - least) / least);
    }
    s.check("the core of a trade area holds the share of the custom asked for", worst_share, 1e-13);
    s.check("... in the least mass of buyers: against every subset, the last cell in part, over forty markets", worst_mass, 1e-12);
    const auto M = random_market(rng, 12, 3, 2);
    s.require("the whole of the custom is every cell that buys, a part of it fewer; none of it, none",
              mm::core(M, 1, 1.0).cells.size() >= mm::core(M, 1, 0.5).cells.size() && mm::core(M, 1, 0.0).cells.empty());
  }

  // ---- overlap ----
  {
    const auto M = random_market(rng, 30, 5, 2);
    const auto t = mm::trade_area(M, 1), u = mm::trade_area(M, 3);
    const double a = std::accumulate(t.begin(), t.end(), 0.0), c = std::accumulate(u.begin(), u.end(), 0.0);
    double tv = 0;
    for (std::size_t b = 0; b < t.size(); b++) tv += 0.5 * std::fabs(t[b] / a - u[b] / c);
    s.check("overlap is 1 − the total variation distance of the two customs", mm::overlap(M, 1, 3) - (1 - tv), 1e-13);
    s.check("... symmetric", mm::overlap(M, 1, 3) - mm::overlap(M, 3, 1), 1e-15);
    s.check("... 1 of an offer with itself", mm::overlap(M, 2, 2) - 1, 1e-13);
    mm::Market apart;
    apart.buyers = {{10, 20}, {1, 1}};
    apart.seller = {0, 1};
    apart.kernel.shares.offers = 2;
    apart.kernel.shares.add(std::vector<std::size_t>{0}, std::vector<double>{0.7});
    apart.kernel.shares.add(std::vector<std::size_t>{1}, std::vector<double>{0.4});
    s.check("... 0 for offers with no buyers in common", mm::overlap(apart, 0, 1), 0);
  }

  // ---- segments ----
  {
    const auto M = random_market(rng, 60, 6, 3);
    const auto D = mm::demand(M);
    std::vector<std::size_t> coarse(60), fine(60), cells(60);
    for (std::size_t b = 0; b < 60; b++) coarse[b] = rng.below(4), fine[b] = coarse[b] * 3 + rng.below(3), cells[b] = b;
    const auto C = mm::coarsen(M, coarse, 4);
    mm::validate(C);
    const auto DC = mm::demand(C);
    double worst = std::fabs(DC.outside - D.outside) / D.total;
    for (std::size_t o = 0; o < 6; o++) worst = std::max(worst, std::fabs(DC.offers[o] - D.offers[o]) / D.total);
    s.check("a market coarsened to the blocks of a partition has the market's demands", worst, 1e-13);
    s.check("... and its buyers' mass and spend", std::fabs(DC.total / D.total - 1) + std::fabs(std::accumulate(C.buyers.mass.begin(), C.buyers.mass.end(), 0.0) / std::accumulate(M.buyers.mass.begin(), M.buyers.mass.end(), 0.0) - 1), 1e-13);
    const std::vector<std::size_t> whole(60, 0);
    const auto W = mm::coarsen(M, whole, 1);
    const auto sh = mm::shares(D);
    double row = 0;
    for (std::size_t o = 0; o < 6; o++) row = std::max(row, std::fabs(W.kernel.shares.at(0, o) - sh[o] * D.penetration()));
    s.check("coarsened to one block, the row is the market's shares times its penetration", row, 1e-13);
    const auto dc = mm::dispersion(M, coarse, 4), df = mm::dispersion(M, fine, 12), di = mm::dispersion(M, cells, 60), dw = mm::dispersion(M, whole, 1);
    s.check("the dispersion of the rows: total = within + between (Pythagoras)", (dc.within + dc.between) / dc.total - 1, 1e-12);
    s.check("... for a finer partition too, with the same total", std::fabs((df.within + df.between) / df.total - 1) + std::fabs(df.total / dc.total - 1), 1e-12);
    s.require("refining a partition does not raise the dispersion within", df.within <= dc.within + 1e-15 && dc.within <= dw.within + 1e-15, std::to_string(df.within) + " <= " + std::to_string(dc.within));
    s.check("the partition into cells loses nothing", di.within, 1e-15);
    s.check("the partition into one block explains nothing", dw.between, 1e-15);
    // A kernel constant on blocks: the segments are exact.
    mm::Market K = M;
    K.kernel.shares = mm::Rows{};
    K.kernel.shares.offers = 6;
    for (std::size_t b = 0; b < 60; b++) {
      const auto& src = C.kernel.shares;
      const std::size_t g = coarse[b];
      K.kernel.shares.add(std::span(src.offer).subspan(src.start[g], src.start[g + 1] - src.start[g]), std::span(src.value).subspan(src.start[g], src.start[g + 1] - src.start[g]));
    }
    const auto dk = mm::dispersion(K, coarse, 4);
    s.check("a kernel constant on the blocks has no dispersion within them", dk.within, 1e-15);
    s.check("... and the blocks explain all of it", dk.explained() - 1, 1e-12);
  }

  // ---- the logit ----
  {
    const auto R = random_rule(rng, 50, 6, true);
    mm::validate(R);
    mm::Market M;
    for (std::size_t b = 0; b < 50; b++) M.buyers.mass.push_back(rng.uniform(1, 9)), M.buyers.spend.push_back(rng.uniform(1, 9));
    M.seller = {0, 0, 1, 1, 2, 2};
    M.kernel = mm::kernel(R);
    mm::validate(M);
    double worst = 0;
    for (std::size_t b = 0; b < 50; b++) {
      const double w = R.weights.sum(b);
      worst = std::max(worst, std::fabs(M.kernel.shares.sum(b) - w / (w + R.outside[b])));
    }
    s.check("the logit's row adds to the weights within reach over those and the outside option's", worst, 1e-14);
    // IIA within a cell: the ratio of two shares does not change when a third offer leaves.
    std::vector<bool> active(6, true);
    active[4] = false;
    const auto without = mm::kernel(R, active);
    double ratio = 0;
    for (std::size_t b = 0; b < 50; b++) {
      const double a = M.kernel.shares.at(b, 0), c = M.kernel.shares.at(b, 1);
      if (a > 0 && c > 0) ratio = std::max(ratio, std::fabs(without.shares.at(b, 0) / without.shares.at(b, 1) - a / c));
    }
    s.check("within a cell the ratio of two shares does not change when a third offer leaves", ratio, 1e-12);
    s.require("an offer that has left has no share", mm::demand(mm::Market{M.buyers, M.seller, without}).offers[4] == 0);
    // Diversion: by the removal, and by the closed form.
    double differ = 0, adds = 0;
    for (std::size_t o = 0; o < 6; o++) {
      const auto removed = mm::diversion(M.buyers, M.seller, R, o), closed = mm::diversion_closed(M, o);
      double total = closed.outside;
      differ = std::max(differ, std::fabs(removed.outside - closed.outside));
      for (std::size_t j = 0; j < 6; j++) differ = std::max(differ, std::fabs(removed.to[j] - closed.to[j])), total += closed.to[j];
      adds = std::max(adds, std::fabs(total - 1));
    }
    s.check("diversion by the removal itself is the logit's closed form, for every offer", differ, 1e-12);
    s.check("... and its parts, to the other offers and to not buying, add to one", adds, 1e-12);
  }
  {
    // A cell with nothing within reach, and no outside weight: it buys nothing.
    mm::Logit R;
    R.weights.offers = 2;
    R.weights.add(std::vector<std::size_t>{0, 1}, std::vector<double>{2.0, 1.0});
    R.weights.add(std::vector<std::size_t>{}, std::vector<double>{});
    R.weights.add(std::vector<std::size_t>{1}, std::vector<double>{4.0});
    R.outside = {1.0, 0.0, 0.0};
    const auto K = mm::kernel(R);
    s.require("a cell with nothing within reach buys nothing; one with a single offer and no outside option buys only it",
              K.outside(1) == 1 && K.shares.at(2, 1) == 1 && std::fabs(K.shares.at(0, 0) - 0.5) < 1e-15);
    // When that single offer leaves, its cell buys nothing: all of it is diverted to not buying.
    mm::Buyers buyers{{0.0, 5.0, 10.0}, {1.0, 1.0, 1.0}};
    const auto d = mm::diversion(buyers, {0, 1}, R, 1);
    s.check("the custom of a cell's only offer goes to not buying when it leaves", d.outside - 1, 1e-15);
  }
  {
    // Buyers of one taste: diversion in proportion to shares. Buyers of two tastes: not.
    mm::Logit one, two;
    one.weights.offers = two.weights.offers = 3;
    one.weights.add(std::vector<std::size_t>{0, 1, 2}, std::vector<double>{3.0, 2.0, 1.0});
    one.outside = {2.0};
    two.weights.add(std::vector<std::size_t>{0, 1, 2}, std::vector<double>{6.0, 5.0, 0.1});
    two.weights.add(std::vector<std::size_t>{0, 1, 2}, std::vector<double>{0.1, 5.0, 6.0});
    two.outside = {1.0, 1.0};
    const std::vector<std::size_t> sellers{0, 1, 2};
    const mm::Buyers b1{{10.0}, {1.0}}, b2{{10.0, 10.0}, {1.0, 1.0}};
    const auto D1 = mm::demand(mm::Market{b1, sellers, mm::kernel(one)});
    const auto d1 = mm::diversion(b1, sellers, one, 0);
    s.check("buyers of one taste: diversion from an offer to another is the other's share of the rest of the spend", d1.to[1] - D1.offers[1] / (D1.total - D1.offers[0]), 1e-13);
    const auto D2 = mm::demand(mm::Market{b2, sellers, mm::kernel(two)});
    const auto d2 = mm::diversion(b2, sellers, two, 0);
    // Offers 0 and 2 sell the same by symmetry, but the buyers of 0 hardly buy 2: when 0 leaves, 2 gains far less than its share
    // of the rest of the spend, which is what a market of one taste would give it.
    const double by_share = D2.offers[2] / (D2.total - D2.offers[0]);
    s.check("buyers of two tastes: two offers sell the same ...", D2.offers[0] / D2.offers[2] - 1, 1e-13);
    s.require("... yet when one leaves the other gains far less than its share of the rest: custom follows who shares buyers with whom", d2.to[2] < 0.2 * by_share && d2.to[1] > 10 * d2.to[2],
              std::to_string(d2.to[2]) + " against " + std::to_string(by_share) + " by share");
  }

  // ---- refusals ----
  {
    auto good = random_market(rng, 5, 3, 2);
    auto over = good, negative = good, short_rows = good, stray = good;
    over.kernel.shares.value[0] = 1.5;
    negative.buyers.mass[2] = -1;
    short_rows.buyers.mass.push_back(1), short_rows.buyers.spend.push_back(1);
    stray.kernel.shares.offer[0] = 9;
    s.require("shares above one, a negative mass, a cell without a row and a share of no offer are refused, a market is not",
              refuses(over) && refuses(negative) && refuses(short_rows) && refuses(stray) && !refuses(good));
  }

  return s.done();
}
