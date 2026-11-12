// Checks of markets/system against closed forms, identities and simulation.
//
// The statements held are the source's (W. Huang 2026): O.1 and D.1 to D.4 of the Unified Design Draft v0.9 (invariants as a
// kernel, their inheritance in every regime, the speed-blind stationary regime, kurtosis under a mismatched clock, the
// commute-time identity), S.10 to S.13 of Market Foam v0.9a (the sum over histories, the clock-free law, proper time and the
// event clock, finite propagation), and C.1 and C.7 of Cases v0.9b on a market generated from rules whose truth is known.

#include <markets/system/system.hpp>
#include <markets/valuation/valuation.hpp>

#include <calx/linalg/expm.hpp>
#include <calx/linalg/rectangular.hpp>
#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

using namespace markets::system;
using calx::linalg::Matrix;

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(20261112, 14, 0, 0);

  // ---- invariants ----
  {
    // Three subjects and three kinds: money, a good, and a claim issued by subject 0. Every transfer between two subjects, every
    // exchange of two kinds between two subjects at a ratio of its own, and the issuance of the claim to each of the others.
    const std::size_t S = 3, K = 3;
    const auto place = [&](std::size_t sub, std::size_t k) { return sub * K + k; };
    Grammar G;
    G.places = S * K;
    for (std::size_t i = 0; i < S; i++)
      for (std::size_t j = 0; j < S; j++) {
        if (i == j) continue;
        for (std::size_t k = 0; k < K; k++) {
          G.transfer(place(i, k), place(j, k));
          for (std::size_t l = k + 1; l < K; l++) G.exchange(place(i, k), place(j, k), place(i, l), place(j, l), 1.0, 0.5 + rng.uniform());
        }
      }
    G.issuance(place(1, 2), place(0, 2));
    G.issuance(place(2, 2), place(0, 2));
    const auto basis = invariants(G);
    s.require("three subjects, three kinds: the quantities every history keeps are three, one for each kind", basis.size() == 3);
    double worst = 0;
    for (std::size_t k = 0; k < K; k++) {
      std::vector<double> total(G.places, 0.0);
      for (std::size_t sub = 0; sub < S; sub++) total[place(sub, k)] = 1;
      for (double d : increments(G, total)) worst = std::max(worst, std::fabs(d));
    }
    s.check("each kind's total is one of them", worst, 1e-15);
    // In the classical, the Gaussian and the path regime.
    std::vector<double> rates(G.types.size()), theta(G.places);
    for (double& r : rates) r = rng.uniform(0.1, 2);
    for (double& t : theta) t = rng.uniform(-0.5, 0.5);
    const auto b = drift(G, rates);
    const auto D = diffusion(G, rates);
    double classical = 0, gaussian = 0, path = 0;
    for (const auto& c : basis) {
      double cb = 0, cdc = 0;
      for (std::size_t i = 0; i < G.places; i++) {
        cb += c[i] * b[i];
        for (std::size_t j = 0; j < G.places; j++) cdc += c[i] * D(i, j) * c[j];
      }
      auto shifted = theta;
      for (std::size_t i = 0; i < G.places; i++) shifted[i] += 1.7 * c[i];
      classical = std::max(classical, std::fabs(cb));
      gaussian = std::max(gaussian, std::fabs(cdc));
      path = std::max(path, std::fabs(hamiltonian(G, rates, shifted) - hamiltonian(G, rates, theta)));
    }
    s.check("an invariant is kept by the drift, has no variance, and is a symmetry of the path Hamiltonian", classical + gaussian + path, 1e-12);
    // Along a history of 5,000 events from no claims: the claim's total stays zero, the issuer's position its liability.
    std::vector<double> x(G.places, 100.0);
    for (std::size_t sub = 0; sub < S; sub++) x[place(sub, 2)] = 0;
    const auto run = simulate(G, [&](const std::vector<double>&) { return rates; }, x, 1e9, rng, 5000);
    const double claims = run.state[place(0, 2)] + run.state[place(1, 2)] + run.state[place(2, 2)];
    s.check("along 5,000 events the claim's positions add up to zero: every claim held is its issuer's liability", claims, 1e-9);
    s.require("and the issuer's own position is what the others hold, with its sign changed, and not nothing", run.times.size() == 5000 && std::fabs(run.state[place(0, 2)]) > 0.5);
  }

  // ---- the stationary regime does not see the speed ----
  {
    // N subjects each in state 0 or 1, moving up at c p and down at c (1 − p). Places: the shares in 0 and in 1.
    Grammar G;
    G.places = 2;
    G.transfer(0, 1);
    G.transfer(1, 0);
    const double p = 0.4;
    double stationary = 0, holding = 0;
    for (const double c : {1.0, 4.0})
      for (const double xi : {0.1, 0.35, 0.6, 0.9}) {
        const std::vector<double> rates{c * p * (1 - xi), c * (1 - p) * xi};
        const double dV = std::log(xi * (1 - p) / ((1 - xi) * p));  // V′(ξ) for V the relative entropy of ξ from p
        stationary = std::max(stationary, std::fabs(hamiltonian(G, rates, {0, dV})));
        // The cost of holding the share at ξ: −min_θ H, attained at e^θ = √(B/A).
        const double A = rates[0], B = rates[1];
        holding = std::max(holding, std::fabs(-hamiltonian(G, rates, {0, 0.5 * std::log(B / A)}) - c * std::pow(std::sqrt(p * (1 - xi)) - std::sqrt((1 - p) * xi), 2)));
      }
    s.check("two states: the relative entropy solves H(ξ, V′) = 0 whatever the speed c", stationary, 1e-15);
    s.check("two states: the cost of holding a share is c(√(p(1 − ξ)) − √((1 − p)ξ))², in proportion to the speed", holding, 1e-15);
  }

  // ---- clocks ----
  {
    // Increments that are Gaussian in the event clock, counted over windows of calendar time: the kurtosis is 3(1 + Var N/(E N)²).
    const int windows = 400000;
    double m2 = 0, m4 = 0;
    for (int w = 0; w < windows; w++) {
      double t = 0;
      int count = 0;
      while ((t += rng.exponential() / 20.0) <= 1) count++;
      const double x = std::sqrt(static_cast<double>(count)) * rng.normal();
      m2 += x * x, m4 += x * x * x * x;
    }
    m2 /= windows, m4 /= windows;
    s.check("a Poisson clock of mean 20: the kurtosis of the windowed sum is 3(1 + 1/20) = 3.15", m4 / (m2 * m2) - 3.15, 0.05);

    // A type's count and its clock have one mean (the clock is the compensator): transfers at a hazard in proportion to the holding.
    Grammar G;
    G.places = 3;
    G.transfer(0, 1), G.transfer(1, 2), G.transfer(2, 0);
    double sum = 0, squares = 0, mean = 0;
    const int runs = 4000;
    for (int r = 0; r < runs; r++) {
      const auto run = simulate(G, [](const std::vector<double>& x) { return std::vector<double>{0.3 * x[0], 0.2 * x[1], 0.1 * x[2]}; }, {10, 6, 4}, 4.0, rng);
      const double d = static_cast<double>(run.count[0]) - run.clock[0];
      sum += d, squares += d * d, mean += static_cast<double>(run.count[0]);
    }
    const double se = std::sqrt((squares / runs - (sum / runs) * (sum / runs)) / runs);
    s.require("proper time and the event clock: over 4,000 histories the count less the clock is zero within four standard errors", std::fabs(sum / runs) < 4 * se && mean / runs > 5);
  }

  // ---- distance ----
  {
    const std::size_t n = 7;
    Matrix w(n);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = i + 1; j < n; j++)
        if (j == i + 1 || rng.uniform() < 0.5) w(i, j) = w(j, i) = rng.uniform(0.2, 2);
    // The expected time for the tracer to go from i to j: h_j(j) = 0, Σ_k w_ik (h_j(i) − h_j(k)) = 1 elsewhere.
    const auto hitting = [&](std::size_t target) {
      Matrix A(n), b(n);
      for (std::size_t i = 0; i < n; i++) {
        if (i == target) { A(i, i) = 1; continue; }
        b(i, 0) = 1;
        for (std::size_t k = 0; k < n; k++)
          if (k != i) A(i, i) += w(i, k), A(i, k) -= w(i, k);
      }
      const auto X = calx::linalg::solve(A, b);
      std::vector<double> h(n);
      for (std::size_t i = 0; i < n; i++) h[i] = X(i, 0);
      return h;
    };
    double commute = 0, triangle = 0;
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) {
        if (i == j) continue;
        commute = std::max(commute, std::fabs(hitting(j)[i] + hitting(i)[j] - static_cast<double>(n) * resistance(w, i, j)));
        for (std::size_t k = 0; k < n; k++) triangle = std::max(triangle, resistance(w, i, j) - resistance(w, i, k) - resistance(w, k, j));
      }
    s.check("distance: the expected time there and back is n times the effective resistance, on seven nodes", commute, 1e-10);
    s.require("distance: the triangle inequality holds", triangle < 1e-12);
    Matrix faster = w;
    faster(0, 1) = faster(1, 0) = w(0, 1) + 1;
    double longer = 0;
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) longer = std::max(longer, resistance(faster, i, j) - resistance(w, i, j));
    s.require("distance: raising one rate lengthens no distance", longer < 1e-12);
  }

  // ---- the sum over histories ----
  {
    const std::size_t n = 4;
    Matrix R(n);  // the rates between states
    std::vector<double> total(n, 0.0);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++)
        if (i != j) total[i] += (R(i, j) = rng.uniform(0.1, 1.5));
    const double sl = 0.7;
    // Σ_n [(s + D)⁻¹ R]ⁿ (s + D)⁻¹, summed term by term: every history from x to y, weighted cell by cell.
    Matrix step(n), sum(n), power = Matrix::identity(n);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) step(i, j) = R(i, j) / (sl + total[i]);
    for (int k = 0; k < 400; k++) {
      for (std::size_t i = 0; i < n; i++)
        for (std::size_t j = 0; j < n; j++) sum(i, j) += power(i, j) / (sl + total[j]);
      power = power * step;
    }
    Matrix sQ(n);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) sQ(i, j) = (i == j ? sl + total[i] : -R(i, j));
    const auto resolvent = calx::linalg::inverse(sQ);
    double worst = 0;
    for (std::size_t i = 0; i < n * n; i++) worst = std::max(worst, std::fabs(sum.a[i] - resolvent.a[i]));
    s.check("the sum over all histories between two states, weighted cell by cell, is the resolvent (s − Q)⁻¹", worst, 1e-13);
    // The law of the sequence of events, durations summed out, does not change when the rates at each state are rescaled.
    const std::vector<std::size_t> path{0, 2, 1, 3, 0, 1};
    const auto law = [&](const std::vector<double>& g) {
      double pr = 1;
      for (std::size_t k = 0; k + 1 < path.size(); k++) pr *= g[path[k]] * R(path[k], path[k + 1]) / (g[path[k]] * total[path[k]]);
      return pr;
    };
    s.check("the law of the sequence of events is the same under any rescaling of time state by state", law({1, 1, 1, 1}) / law({0.2, 3, 7, 0.5}) - 1, 1e-15);
  }

  // ---- a market from rules ----
  {
    Rules P;
    P.subjects = 8, P.goods = 2, P.horizon = 100, P.velocity = 2.5, P.money_each = 10, P.noise = 0.02, P.barter = 0.4;
    P.price = {2.0, 5.0};
    for (std::size_t i = 0; i < P.subjects; i++) P.bargaining.push_back(0.05 * (static_cast<double>(i) - 3.5));
    const auto G = generate(P);
    const auto& R = G.record;
    const auto M = markets::foam::stock(R, 0);
    s.check("generated: money is neither made nor lost", *std::max_element(M.level.begin(), M.level.end()) - *std::min_element(M.level.begin(), M.level.end()), 1e-9);
    s.check("generated: the payments over the area of the money faces give the velocity, 2.5", M.moved / M.integral - 2.5, 0.07);
    // Every purchase and every barter is an exchange; the reference valuation finds the prices, and who has the better of whom.
    const auto X = markets::foam::exchanges(R);
    std::vector<markets::valuation::Trade> trades;
    for (const auto& e : X) trades.push_back({e.a, e.b, e.ka, e.kb, e.log_ratio(), e.time, 1.0});
    const auto V = markets::valuation::reference(trades, R.kinds, R.subjects);
    s.check("generated: the reference valuation finds the goods' prices in money, 2 and 5", std::fabs(std::exp(V.potential[1] - V.potential[0]) - 2) / 2 + std::fabs(std::exp(V.potential[2] - V.potential[0]) - 5) / 5, 0.01);
    double sa = 0, sb = 0, saa = 0, sbb = 0, sab = 0;
    const double n = static_cast<double>(P.subjects);
    for (std::size_t i = 0; i < P.subjects; i++) {
      const double a = V.advantage[i], b = P.bargaining[i];
      sa += a, sb += b, saa += a * a, sbb += b * b, sab += a * b;
    }
    const double correlation = (sab - sa * sb / n) / std::sqrt((saa - sa * sa / n) * (sbb - sb * sb / n));
    s.require("generated: the subjects' advantages follow their hidden bargaining strength (correlation above 0.99)", correlation > 0.99);
    // The record as a foam: a ring of thirty subjects, partners the neighbours. Influence spreads one neighbour an event.
    Rules Q;
    Q.subjects = 30, Q.goods = 1, Q.horizon = 3, Q.ring = true, Q.barter = 0, Q.money_each = 5;
    const auto ring = generate(Q).record;
    const auto H = markets::foam::history(ring).history;
    const auto F = markets::foam::foam(H);
    s.check("generated: the boundary of a boundary vanishes on the record's complex", markets::foam::boundary_defect(F), 0);
    const auto around = [&](std::size_t a, std::size_t b) { const std::size_t d = a > b ? a - b : b - a; return std::min(d, 30 - d); };
    const auto subjects_of = [&](std::size_t v) {
      std::vector<std::size_t> out;
      for (std::size_t x : F.touched[v])
        if (x < 30) out.push_back(x);
      return out;
    };
    bool bounded = true, attained = false;
    for (std::size_t u = 0; u < F.events && u < 40; u++) {
      const auto steps = markets::foam::steps_from(F, u);
      for (std::size_t v = 0; v < F.events; v++) {
        if (steps[v] == markets::foam::none || v == u) continue;
        for (std::size_t a : subjects_of(u))
          for (std::size_t b : subjects_of(v)) {
            bounded = bounded && around(a, b) <= steps[v] + 1;
            attained = attained || around(a, b) == steps[v] + 1;
          }
      }
    }
    s.require("on a ring where each event joins two neighbours, an event m steps later lies within m + 1 of the first, and the bound is met", bounded && attained);
  }
  return s.done();
}
