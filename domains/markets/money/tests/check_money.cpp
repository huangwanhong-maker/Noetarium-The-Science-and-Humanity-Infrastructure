// Checks of markets/money against closed forms.
//
// The statements held are C.1 to C.4 of the source (W. Huang 2026, Cases v0.9b, §3), on its own example where it gives one:
// a ring of twelve sites, each spending half with each neighbour, and an injection of one per cent of a site's balance.
// Proposition C.4 is also held where the source does not go (theory note, amendment A4): shares of spending that are not
// symmetric, for which the receipts are (Wᵀm) and total consumption is still total output.

#include <markets/money/money.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace markets::money;
using calx::linalg::Matrix;

namespace {

Economy ring(std::size_t n, double forward = 0.5) {
  Economy E;
  E.W = Matrix(n);
  for (std::size_t i = 0; i < n; i++) E.W(i, (i + 1) % n) = forward, E.W(i, (i + n - 1) % n) = 1 - forward;
  return E;
}
double worst(const std::vector<double>& a, const std::vector<double>& b) {
  double w = 0;
  for (std::size_t i = 0; i < a.size(); i++) w = std::max(w, std::fabs(a[i] - b[i]));
  return w;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(20261112, 13, 0, 0);

  // ---- the loop of a loan ----
  {
    const auto flat = fisher(0.0, 0.03, 0.05, 0.02, 1.0);
    s.check("Fisher: the loop is flat when the nominal rate is the own rate and inflation, 0.05 = 0.02 + 0.03", flat.holonomy, 1e-17);
    const auto surprise = fisher(0.0, 0.07, 0.05, 0.02, 1.0);
    s.check("Fisher: inflation four points above what the claim was written for leaves the holonomy −0.04", surprise.holonomy + 0.04, 1e-16);
    s.check("Fisher: and multiplies the creditor's real position by 0.9608", surprise.real_factor - 0.9608, 5e-5);
    const auto redenominated = fisher(0.0 + 4.6, 0.07 + 4.6, 0.05, 0.02, 1.0);
    s.check("Fisher: a change of the money unit at both exchanges leaves the holonomy", redenominated.holonomy - surprise.holonomy, 1e-15);
    s.check("velocity: 250 of payments over a money area of 100 is 2.5", velocity(250, 100) - 2.5, 0);
  }

  // ---- an injection on the source's ring ----
  {
    const auto E = ring(12);
    const double balance = 1.0, added = 0.01;  // y = v = 1, so p = 1
    const auto R = injection(E, 12 * balance, 0, added, 200.0, 20000);
    s.check("ring of twelve: total consumption is total output at every instant", R.total_consumed_defect, 1e-12);
    double factor = 0;
    for (std::size_t j = 0; j < 12; j++) factor = std::max(factor, std::fabs(R.price.back()[j] / R.price_before[j] - (1 + added / 12)));
    s.check("ring of twelve: every price ends at 1 + ΔM/M = 1.000833 times what it was", factor, 1e-10);
    const auto first = first_order_gains(E, 0, added, 1.0), closed = symmetric_gains(E, 0, added, 1.0);
    s.check("ring of twelve: the first-order gains are the source's (ΔM/p)(I + W)(e₀ − 1/n)", worst(first, closed), 1e-15);
    s.check("ring of twelve: the site of the injection gains 0.00833, its neighbours 0.00333 each, every other site loses 0.00167",
            std::fabs(closed[0] - 0.01 * (1 - 1.0 / 6)) + std::fabs(closed[1] - 0.01 * (0.5 - 1.0 / 6)) + std::fabs(closed[6] + 0.01 / 6), 1e-16);
    s.check("ring of twelve: followed in full, the site gains 0.00831", R.gain[0] - 0.00831, 1e-5);
    s.check("ring of twelve: and the neighbours and the distant sites what the first order says, within a part in a hundred", worst(R.gain, closed) / closed[0], 1e-2);
    double total = 0;
    for (double g : R.gain) total += g;
    s.check("ring of twelve: the gains add up to zero", total, 1e-9);
    s.require("ring of twelve: the neighbours' prices rise at once, and beyond their own end (they overshoot)", R.half_rise[1] == 0 && R.price[0][1] > R.price_after[1]);
    bool later = true;
    for (std::size_t d = 2; d < 6; d++) later = later && R.half_rise[d + 1] > R.half_rise[d];
    s.require("ring of twelve: farther sites' prices rise later", later);
    s.check("ring of twelve: the prices at distances two to six have made half their rise at 0.21, 1.60, 4.81, 8.07 and 9.12",
            std::fabs(R.half_rise[2] - 0.21) + std::fabs(R.half_rise[3] - 1.60) + std::fabs(R.half_rise[4] - 4.81) + std::fabs(R.half_rise[5] - 8.07) + std::fabs(R.half_rise[6] - 9.12), 0.03);
    const auto large = injection(E, 12 * balance, 0, 0.5, 200.0, 20000);
    const auto linear = symmetric_gains(E, 0, 0.5, 1.0);
    bool signs = true;
    for (std::size_t i = 0; i < 12; i++) signs = signs && (large.gain[i] > 0) == (linear[i] > 0);
    s.check("an injection of half a balance: the site gains 0.372 where the first order says 0.417", std::fabs(large.gain[0] - 0.372) + std::fabs(linear[0] - 0.41667), 1e-3);
    s.require("an injection of half a balance: who gains and who loses is unchanged", signs);
  }

  // ---- shares of spending that are not symmetric ----
  {
    // A ring on which 0.7 of spending goes forward and 0.3 back: the columns still sum to one, so balances rest equal.
    const auto E = ring(9, 0.7);
    s.require("a one-sided ring is balanced: its sites rest at equal balances and one price", balanced(E) && worst(rest(E, 9.0), std::vector<double>(9, 1.0)) < 1e-12);
    const auto R = injection(E, 9.0, 0, 1e-4, 300.0, 30000);
    s.check("a one-sided ring: total consumption is total output at every instant, with no symmetry", R.total_consumed_defect, 1e-12);
    const auto first = first_order_gains(E, 0, 1e-4, 1.0);
    s.check("a one-sided ring: the first-order gains are those followed in full, for a small injection", worst(R.gain, first) / first[0], 2e-3);
    s.require("a one-sided ring: the site that receives more of the injected site's spending gains more", first[1] > first[8] && first[1] > 0);
    // Written with (Wm) for the receipts, as the source has it, consumption is not conserved here.
    std::vector<double> m(9, 1.0);
    m[0] += 0.3;
    double wrong = 0;
    for (std::size_t i = 0; i < 9; i++) {
      double c = 0;
      for (std::size_t j = 0; j < 9; j++) {
        double wm = 0;
        for (std::size_t k = 0; k < 9; k++) wm += E.W(j, k) * m[k];
        if (E.W(i, j) > 0) c += E.W(i, j) / wm;
      }
      wrong += m[i] * c;
    }
    double right = 0;
    for (double c : consumption(E, m)) right += c;
    s.check("with receipts (Wᵀm) consumption adds up to output", right - 9, 1e-13);
    s.require("with receipts written (Wm) it does not, unless W is symmetric", std::fabs(wrong - 9) > 1e-3);

    // Random shares whose columns do not sum to one: the sites rest at unequal balances and prices, in proportion to the
    // stationary law of the shares; consumption is still conserved, and prices all end 1 + ΔM/M times what they were.
    Economy G;
    G.W = Matrix(6);
    for (std::size_t i = 0; i < 6; i++) {
      double row = 0;
      for (std::size_t j = 0; j < 6; j++) row += (G.W(i, j) = i == j ? 0.0 : rng.uniform(0.1, 1));
      for (std::size_t j = 0; j < 6; j++) G.W(i, j) /= row;
    }
    const auto m0 = rest(G, 6.0);
    const auto p0 = prices(G, m0);
    s.require("unbalanced shares: the sites rest at unequal balances and prices", !balanced(G) && *std::max_element(p0.begin(), p0.end()) > 1.05 * *std::min_element(p0.begin(), p0.end()));
    std::vector<double> back = receipts_share(G, m0);
    s.check("unbalanced shares: at rest each site receives what it spends", worst(back, m0), 1e-12);
    const auto Q = injection(G, 6.0, 2, 0.06, 200.0, 4000);
    double factor = 0;
    for (std::size_t j = 0; j < 6; j++) factor = std::max(factor, std::fabs(Q.price.back()[j] / Q.price_before[j] - 1.01));
    s.check("unbalanced shares: consumption is conserved and every price ends 1.01 times what it was", Q.total_consumed_defect + factor, 1e-9);
    bool threw = false;
    try { (void)first_order_gains(G, 0, 0.01, 1.0); } catch (const std::invalid_argument&) { threw = true; }
    s.require("the first-order formula is refused where balances do not rest equal", threw);
  }
  return s.done();
}
