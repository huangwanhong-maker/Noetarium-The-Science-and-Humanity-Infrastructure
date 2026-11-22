// Checks of risk_management::wellbeing, each against a closed form or a computation apart: the calibration's flat tax and capital, and
// income recovered from them; P2's Eq. 16, the income lost equal to Π L; the well-being loss of a log-utility household equal to the
// series Σ xⁿ/(n(ρ + nλ)), and at η = 2 to (1/c) Σ xⁿ/(ρ + nλ); a small loss whose equivalent is P1's Γ times the asset loss, so that
// resilience is (ρ + λ)/(Π + λ); a buffer whose spent area is what it held, and P2's Eq. 27 in its reading; the optimal rate where P2's
// Eq. 23 vanishes; the step weights over return periods; support financed exactly by its tax, lowering the equivalent loss; the poor's
// share of well-being losses above their share of asset losses; reconstruction slowed to the floor; a hard minimum reported; persons
// pushed below a poverty line; refusals.

#include <risk_management/wellbeing/wellbeing.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace risk_management::wellbeing;

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
// One household of consumption c, no transfers, all affected, private assets, losing x c at the start: v = x Π/(Π + λ).
Household single(double c, double x, const Readings& R) {
  Household h;
  h.id = "h";
  h.group = "all";
  h.income = c;
  h.vulnerability = x * R.productivity / (R.productivity + R.lambda());
  return h;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  Readings R;
  R.productivity = 0.3;

  // ---- calibration and Eq. 16 ----
  {
    std::vector<Household> H = {{"A", "poor", 2, 1000, 100}, {"B", "rich", 3, 3000, 150}};
    const auto C = calibrate(H, R);
    const double d = 650.0 / 11000;
    s.check("the flat tax financing transfers δ = (2 · 100 + 3 · 150)/(2 · 1000 + 3 · 3000) = 650/11,000", std::fabs(C.delta_sp - d), 1e-15);
    s.check("effective capital k = (i − i_sp)/((1 − δ)Π): 900/((1 − δ) 0.3) for A", std::fabs(C.capital[0] - 900 / ((1 - d) * 0.3)), 1e-9);
    double back = 0;
    for (std::size_t h = 0; h < H.size(); h++) back += H[h].weight * ((1 - C.delta_sp) * R.productivity * C.capital[h] + H[h].transfers);
    s.check("... income recovered from capital and transfers, Σ ω[(1 − δ)Π k + i_sp] = Σ ω i = 11,000", std::fabs(back - 11000), 1e-9);
    const auto o = outcome(H, C, R, {50, {0.6, 0.2}});
    s.check("P2's Eq. 16: the income lost at the start, own income of the affected and transfers of all, is Π L", std::fabs(o.income_loss - R.productivity * o.asset_loss) / o.asset_loss, 1e-14);
  }

  // ---- closed forms ----
  {
    Readings L = R;
    L.eta = 1;
    L.horizon = 60;
    const double c = 1000, x = 0.5;
    const auto H = std::vector<Household>{single(c, x, L)};
    const auto C = calibrate(H, L);
    const auto o = outcome(H, C, L, {100, {1.0}});
    double series = 0;
    for (int n = 1; n < 200; n++) series += std::pow(x, n) / (n * (L.rho + n * L.lambda()));
    s.check("log utility, half of consumption lost at the start: ΔW = Σ xⁿ/(n(ρ + nλ))", std::fabs(o.wellbeing - series) / series, 1e-9);
    Readings Q = L;
    Q.eta = 2;
    const auto o2 = outcome(H, calibrate(H, Q), Q, {100, {1.0}});
    double s2 = 0;
    for (int n = 1; n < 200; n++) s2 += std::pow(x, n) / (Q.rho + n * Q.lambda());
    s2 /= c;
    s.check("η = 2: ΔW = (1/c) Σ xⁿ/(ρ + nλ)", std::fabs(o2.wellbeing - s2) / s2, 1e-9);
    Readings small = R;
    small.horizon = 60;
    const auto Hs = std::vector<Household>{single(c, 1e-6, small)};
    const auto os = outcome(Hs, calibrate(Hs, small), small, {100, {1.0}});
    const double Gamma = (small.productivity + small.lambda()) / (small.rho + small.lambda());
    s.check("a small loss to a household at the mean: ΔC_eq = Γ ΔK, P1's Γ = (Π + λ)/(ρ + λ)", std::fabs(os.equivalent / (Gamma * os.asset_loss) - 1), 1e-5);
    s.check("... so that resilience is (ρ + λ)/(Π + λ)", std::fabs(os.resilience - 1 / Gamma) * Gamma, 1e-5);
  }

  // ---- buffers (P2 Eqs 25–27) ----
  {
    Path p;
    p.a = 100;
    p.alpha = 1;
    const double spent = cap(p, 30);
    const double area = 100 * (1 - std::exp(-p.t_hat)) - p.gamma * p.t_hat;
    s.check("a buffer of 30 spent on the deepest part: the area above γ is 30, and the loss meets γ at t̂", std::fabs(area - 30) + std::fabs(100 * std::exp(-p.t_hat) - p.gamma) + std::fabs(spent - 30), 1e-9);
    const double beta = std::exp(-p.t_hat);
    s.check("... P2's Eq. 27 read with γ the loss's level: a(1 − β) + γ ln β − λ B = 0", std::fabs(100 * (1 - beta) + p.gamma * std::log(beta) - 30), 1e-9);
    Path q;
    q.a = 100;
    q.alpha = 2;
    const double all = cap(q, 80);
    s.check("a buffer above the whole loss (100/2 = 50) leaves nothing, and only 50 is spent", std::fabs(all - 50) + recovery_loss(q, 1000, R), 1e-12);
    Path h;
    h.a = 100;
    h.alpha = 1;
    h.ceiling = 50;
    // Held at 50 until ln 2, then 100 e^{−t}: 50 ln 2 + 50 in all.
    s.check("a loss held below a ceiling of 50: its whole is 50 ln 2 + 50, and a buffer never spends against what the ceiling removed",
            std::fabs(h.total() - (50 * std::log(2.0) + 50)) + std::fabs(cap(h, 200) - (50 * std::log(2.0) + 50)), 1e-9);
  }

  // ---- the optimal rate (P2 Eqs 22–23) ----
  {
    Readings O = R;
    O.productivity = 0.33;
    const double v = 0.3, l = optimal_lambda(v, O);
    // Eq. 23: ∫_0^H [Π − (Π + λ) v e^{−λt}]^{−η} v e^{−λt}(t(Π + λ) − 1) e^{−ρt} dt = 0, by Simpson's rule.
    const int n = 20000;
    double I = 0, scale = 0;
    for (int k = 0; k <= n; k++) {
      const double t = O.horizon * k / n, w = (k == 0 || k == n) ? 1 : (k % 2 ? 4 : 2);
      const double c = O.productivity - (O.productivity + l) * v * std::exp(-l * t);
      const double g = std::pow(c, -O.eta) * v * std::exp(-l * t) * std::exp(-O.rho * t);
      I += w * g * (t * (O.productivity + l) - 1);
      scale += w * g;
    }
    s.check("the optimal rate makes P2's Eq. 23 vanish (relative to its scale)", std::fabs(I / scale), 1e-6);
    s.require("... and is a maximum, below the bound (Π + λ)v < Π", own_wellbeing(l, v, O) > own_wellbeing(l - 0.02, v, O) && own_wellbeing(l, v, O) > own_wellbeing(l + 0.02, v, O) &&
                                                                        (O.productivity + l) * v < O.productivity);
  }

  // ---- over return periods, support, distribution ----
  std::vector<Household> H = {{"p1", "poorest fifth", 20, 600, 60, 0, 1, 0.7}, {"p2", "poorest fifth", 20, 800, 80, 0, 1, 0.7},
                              {"m", "middle", 30, 2000, 100, 300, 1, 0.4}, {"r", "richest fifth", 30, 6000, 150, 2000, 1, 0.14}};
  const std::vector<Event> E = {{100, {0.5, 0.4, 0.3, 0.2}}, {10, {0.2, 0.15, 0.1, 0.05}}};
  {
    const auto A = annual(H, R, E);
    s.check("the step weights: 1/10 − 1/100 = 0.09 for the frequent event, 1/100 for the rare", std::fabs(A.weights[0] - 0.09) + std::fabs(A.weights[1] - 0.01), 1e-15);
    s.check("... the expected asset loss 0.09 L₁₀ + 0.01 L₁₀₀, resilience the ratio of the expectations",
            std::fabs(A.asset_loss - (0.09 * A.events[0].asset_loss + 0.01 * A.events[1].asset_loss)) + std::fabs(A.resilience - A.asset_loss / A.equivalent), 1e-9);
    const auto& poor = A.by_group.at("poorest fifth");
    double total_eq = 0;
    for (const auto& [_, g] : A.by_group) total_eq += g.equivalent;
    s.require("the poorest fifth's share of well-being losses exceeds its share of asset losses", poor.equivalent / total_eq > poor.asset_loss / A.asset_loss);
    s.check("... and the groups' equivalent losses sum to the whole", std::fabs(total_eq - A.equivalent) / A.equivalent, 1e-12);
    Support S{0.8, "poorest fifth", 0.25, 0.05};
    const auto B = annual(H, R, E, S);
    const auto& o = B.events[1];
    double helped = 0, income = 0;
    for (std::size_t h = 0; h < H.size(); h++) {
      helped += H[h].weight * (E[0].exposed[h] * 0.75 + (1 - E[0].exposed[h]) * 0.05);
      income += H[h].weight * H[h].income;
    }
    s.check("support: 80% of the poorest fifth's mean loss paid to each person helped, its cost the tax on all income",
            std::fabs(o.support_paid - o.support_payout * helped) + std::fabs(o.support_tax_rate * income - o.support_paid), 1e-9);
    s.require("... and the expected equivalent loss falls, the asset loss unchanged", B.equivalent < A.equivalent && std::fabs(B.asset_loss - A.asset_loss) < 1e-9);
  }
  {
    // The floor: reconstruction slows so that consumption starts at the floor exactly.
    Readings F = R;
    F.floor = 700;
    std::vector<Household> one = {{"h", "g", 1, 1000, 0, 0, 1, 0.15}};
    Options at_floor;
    at_floor.poverty_line = 700 + 1e-6;
    const auto C = calibrate(one, F);
    const auto o = outcome(one, C, F, {50, {1.0}}, {}, at_floor);
    const double own = F.productivity * C.capital[0] * 0.15;  // δ = 0: 150 of income lost
    const double lf = (1000 - 700 - own) / (C.capital[0] * 0.15);  // 0.3 a year, slower than λ = ln 20/3
    s.require("a loss that would take consumption below the floor slows reconstruction to λ_h = (c − floor − Δi)/Δk = 0.3, and consumption starts at the floor",
              o.slowed == 1 && std::fabs(lf - 0.3) < 1e-12 && lf < F.lambda() && std::fabs(o.pushed_below - 1) < 1e-12);
    std::vector<Household> two = {{"h", "g", 1, 1000, 0, 0, 1, 0.99}};
    Readings G = R;
    G.floor = 900;
    const auto og = outcome(two, calibrate(two, G), G, {50, {1.0}});
    s.require("income loss alone below the floor: rebuilt at the slowest pace, and held at the hard minimum where it falls below it", og.slowed == 1);
    Readings Z = R;
    std::vector<Household> three = {{"h", "g", 1, 1000, 0, 0, 1, 1.0}};
    s.require("no floor and everything lost: consumption held at the hard minimum, reported", outcome(three, calibrate(three, Z), Z, {50, {1.0}}).capped >= 1);
  }
  {
    Options O;
    O.poverty_line = 950;
    std::vector<Household> one = {{"h", "g", 4, 1000, 0, 0, 1, 0.05}};
    const auto C = calibrate(one, R);
    const auto o = outcome(one, C, R, {50, {0.5}}, {}, O);
    const double loss0 = (R.productivity + R.lambda()) * 0.05 * C.capital[0];
    s.require("half of four persons affected, losing " + std::to_string(static_cast<int>(loss0)) + " of 1,000 at the start: two pushed below a line of 950",
              loss0 > 50 && std::fabs(o.pushed_below - 2) < 1e-12);
  }
  {
    s.require("transfers above income are refused", refused([] { calibrate({{"h", "g", 1, 100, 150}}, Readings{}); }));
    s.require("an exposed share above one is refused", refused([&] { outcome(H, calibrate(H, R), R, {10, {1.5, 0, 0, 0}}); }));
    s.require("two events at one return period are refused", refused([&] { annual(H, R, {{10, {0, 0, 0, 0}}, {10, {0, 0, 0, 0}}}); }));
    Readings bad = R;
    bad.slowest_rebuild_years = 1;
    s.require("a slowest pace faster than N is refused", refused([&] { calibrate(H, bad); }));
  }
  return s.done();
}
