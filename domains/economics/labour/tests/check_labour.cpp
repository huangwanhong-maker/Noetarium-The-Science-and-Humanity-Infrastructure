// Checks of economics/labour: the two-state steady state u* = s/(s + f) as the three-state chain with no inactivity; the stationary shares
// a fixed point of the chain; spells of 1/(1 − P_ii); the half-life against iterating the chain; the Shapley contributions summing to
// the change exactly, a single changed flow taking all of it, and the order of the flows not mattering; counts made probabilities.

#include <economics/labour/flows.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace economics::labour;

int main() {
  calx::verification::Suite s;
  {
    // No flow into inactivity: its steady share is zero, and the chain is the two-state one of employment and unemployment.
    const double sep = 0.02, find = 0.25;
    Transitions P{{{1 - sep, sep, 0}, {find, 1 - find, 0}, {0.1, 0.0, 0.9}}};
    const auto st = steady(P);
    s.check("two states: the unemployment rate s/(s + f)", st.unemployment_rate - sep / (sep + find), 1e-12);
    s.check("two states: a spell of unemployment lasts 1/f", st.spell[U] - 1 / find, 1e-12);
  }
  const Transitions P{{{0.955, 0.015, 0.030}, {0.25, 0.60, 0.15}, {0.04, 0.03, 0.93}}};
  const auto st = steady(P);
  {
    const auto n = next(st.shares, P);
    double worst = 0, sum = 0;
    for (std::size_t i = 0; i < 3; i++) worst = std::max(worst, std::fabs(n[i] - st.shares[i])), sum += st.shares[i];
    s.check("steady state: π P = π", worst, 1e-13);
    s.check("steady state: shares sum to one", sum - 1, 1e-13);
    s.check("steady state: participation = π_E + π_U", st.participation_rate - (st.shares[E] + st.shares[U]), 1e-15);
    // Half-life: iterate from all unemployed and find when the distance to π has halved since the deviation along λ₂ took over.
    std::array<double, 3> x{0.2, 0.5, 0.3};
    double d0 = 0;
    for (int t = 0; t < 200; t++) x = next(x, P);
    for (std::size_t i = 0; i < 3; i++) d0 += std::fabs(x[i] - st.shares[i]);
    const int h = static_cast<int>(std::round(st.half_life * 10));
    for (int t = 0; t < h; t++) x = next(x, P);
    double d1 = 0;
    for (std::size_t i = 0; i < 3; i++) d1 += std::fabs(x[i] - st.shares[i]);
    s.check("convergence: ten half-lives shrink the deviation about 2^10", std::log2(d0 / d1) - 10, 0.15 + 10 * std::fabs(st.half_life * 10 - h) / std::max(1.0, st.half_life * 10));
  }
  {
    const Transitions Q{{{0.950, 0.020, 0.030}, {0.20, 0.64, 0.16}, {0.04, 0.035, 0.925}}};
    const auto c = contributions(P, Q);
    double sum = 0;
    for (double x : c.by_flow) sum += x;
    s.check("contributions: the six flows' shares sum to the change exactly", sum - c.change, 1e-14);
    s.require("contributions: separations up and finding down both raise unemployment", c.by_flow[0] > 0 && c.by_flow[2] > 0);
    Transitions only = P;
    only[E][U] = 0.020, only[E][E] = 1 - 0.020 - only[E][I];
    const auto one = contributions(P, only);
    s.check("contributions: a single changed flow takes all of the change", one.by_flow[0] - one.change, 1e-14);
    double rest = 0;
    for (std::size_t k = 1; k < 6; k++) rest += std::fabs(one.by_flow[k]);
    s.check("contributions: and the others nothing", rest, 1e-14);
    const auto back = contributions(Q, P);
    double antisym = 0;
    for (std::size_t k = 0; k < 6; k++) antisym = std::max(antisym, std::fabs(back.by_flow[k] + c.by_flow[k]));
    s.check("contributions: reversing the periods reverses every share", antisym, 1e-13);
  }
  {
    const auto R = from_counts({{{900, 30, 70}, {50, 120, 30}, {20, 10, 470}}});
    s.check("counts: E→U = 30/1000", R[E][U] - 0.03, 1e-15);
    s.check("counts: rows sum to one", R[I][E] + R[I][U] + R[I][I] - 1, 1e-15);
    bool refused = false;
    try { (void)steady(Transitions{{{0.9, 0.2, 0.0}, {0.1, 0.9, 0.0}, {0, 0, 1}}}); } catch (const std::invalid_argument&) { refused = true; }
    s.require("a row that does not sum to one is refused", refused);
  }
  return s.done();
}
