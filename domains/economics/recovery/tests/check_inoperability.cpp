// Checks of economics::recovery, each worked by hand or against Leontief's model: the interdependency matrix of an unequal table; the
// static model equal to Leontief's output response over planned output; the dynamic model settling at the static one under a constant
// perturbation; a single sector recovering to 1% in its declared periods exactly; losses with and without the interdependencies, never
// less with them; the unbounded horizon's closed form, lost demand costing the same at any resilience; rankings by inoperability and by
// loss that differ; refusals.

#include <economics/recovery/inoperability.hpp>

#include <calx/verification/check.hpp>

#include <cmath>

using namespace economics::recovery;

namespace {
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
economics::InputOutputTable table(double y1, double y2) {
  economics::InputOutputTable T;
  T.sectors = {"a", "b"};
  T.z = calx::linalg::Matrix(2);
  T.z(0, 0) = 10, T.z(0, 1) = 20, T.z(1, 0) = 30, T.z(1, 1) = 0;
  T.final_demand = {y1, y2};
  const auto x = T.output();
  T.value_added = {x[0] - 40, x[1] - 20};
  return T;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto T = table(170, 70);  // outputs 200 and 100
  const auto I = interdependency(T);
  s.check("A* of a table with outputs 200 and 100: z_ij / x_i = [[0.05, 0.1], [0.3, 0]]",
          std::fabs(I.a(0, 0) - 0.05) + std::fabs(I.a(0, 1) - 0.1) + std::fabs(I.a(1, 0) - 0.3) + std::fabs(I.a(1, 1)), 1e-15);
  s.require("... its rows sum to 0.15 and 0.3, below one", std::fabs(I.largest_row - 0.3) < 1e-15);
  {
    const auto M = economics::leontief(T);
    const auto dx = economics::output_for(M, {10, 0});
    const auto q = static_inoperability(I, {10.0 / 200, 0});
    s.check("the static model equals Leontief's output response over planned output: q = Δx / x̂",
            std::fabs(q[0] - dx[0] / 200) + std::fabs(q[1] - dx[1] / 100), 1e-14);
    const std::vector<std::vector<double>> c(3000, {10.0 / 200, 0});
    const auto P = recover(I, {0, 0}, {0.5, 0.5}, c, 3000, 365);
    s.check("the dynamic model under a constant perturbation settles at the static one", std::fabs(P.q.back()[0] - q[0]) + std::fabs(P.q.back()[1] - q[1]), 1e-12);
  }
  {
    economics::InputOutputTable one;
    one.sectors = {"c"};
    one.z = calx::linalg::Matrix(1);
    one.final_demand = {365}, one.value_added = {365};
    const auto J = interdependency(one);
    const auto k = resilience(J, {10});
    const auto P = recover(J, {0.5}, k, {}, 10, 365);
    s.check("a sector alone, half inoperable, with a recovery of 10 periods to 1%: q(10) = 0.005 exactly", std::fabs(P.q[10][0] - 0.005), 1e-15);
    double sum = 0;
    for (int t = 0; t <= 10; t++) sum += 0.5 * std::pow(1 - k[0], t);
    s.check("... its loss a day's output times Σ_t q(t)", std::fabs(P.loss[0] - sum), 1e-12);
  }
  {
    const auto k = resilience(I, {30, 90});
    const auto P = recover(I, {0.2, 0}, k, {}, 400, 365);
    s.require("losses with the interdependencies exceed each sector's alone: b loses through a though it lost no capacity",
              P.total > P.total_alone && P.loss[1] > 0 && P.alone[1] == 0 && P.loss[0] >= P.alone[0]);
    s.require("... and both recover within the path", P.recovered[0] >= 0 && P.recovered[1] >= 0);
  }
  {
    // A large sector a little inoperable against a small one much inoperable: by inoperability b first, by loss a first.
    const auto r1 = ranking({0.01, 0.2}), r2 = ranking({0.01 * 1000, 0.2 * 10});
    s.require("ranked by inoperability the small sector first, by loss the large one", r1[0] == 1 && r2[0] == 0);
  }
  {
    // Proposition 4: lost demand for 20 periods and a first inoperability, followed until nothing is left; the path's sum against the
    // closed form, and the same lost demand at another resilience costing the same.
    std::vector<std::vector<double>> c(20, {0.02, 0.01});
    const std::vector<double> q0{0.1, 0}, k1{0.2, 0.3}, k2{0.6, 0.9};
    const auto P = recover(I, q0, k1, c, 4000, 365);
    const auto U = unbounded_loss(I, q0, k1, {0.4, 0.2}, 365);
    s.check("Proposition 4: the path's losses over a long horizon equal (I − A*)⁻¹ (C + K⁻¹ q0) times a period's output",
            std::fabs(P.loss[0] - U[0]) + std::fabs(P.loss[1] - U[1]), 1e-9);
    const auto D1 = recover(I, {0, 0}, k1, c, 4000, 365), D2 = recover(I, {0, 0}, k2, c, 4000, 365);
    s.check("... lost demand alone costs the same at either resilience", std::fabs(D1.total - D2.total), 1e-9);
  }
  s.require("an inoperability above one is refused", refused([&] { recover(I, {1.5, 0}, {0.5, 0.5}, {}, 10, 365); }));
  s.require("a resilience of nought is refused", refused([&] { recover(I, {0.1, 0}, {0, 0.5}, {}, 10, 365); }));
  return s.done();
}
