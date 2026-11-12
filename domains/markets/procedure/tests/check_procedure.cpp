// Checks of markets/procedure against the source's examples and closed forms.
//
// The statements held are C.9 to C.15 of the source (W. Huang 2026, Cases v0.9b, §4.3 and §4.4): order sensitivity as a
// commutator, priority steps and their mixture, the sequential, mixed and concurrent procedures (on matrix exponentials),
// and procedures with one law of outcomes. The concurrent limit is held in its amended form (theory note, amendment A2):
// it is neutral between the orders but is not one rule, since it depends on the rates at which the steps run.

#include <markets/procedure/procedure.hpp>

#include <calx/linalg/expm.hpp>
#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace markets::procedure;
using calx::linalg::Matrix;

namespace {

Matrix scaled(const Matrix& a, double c) {
  Matrix m = a;
  for (double& x : m.a) x *= c;
  return m;
}
Matrix plus(const Matrix& a, const Matrix& b, double cb = 1) {
  Matrix m = a;
  for (std::size_t i = 0; i < m.a.size(); i++) m.a[i] += cb * b.a[i];
  return m;
}
double norm(const Matrix& m) {
  double s = 0;
  for (double x : m.a) s += x * x;
  return std::sqrt(s);
}
double worst(const std::vector<double>& a, const std::vector<double>& b) {
  double w = 0;
  for (std::size_t i = 0; i < a.size(); i++) w = std::max(w, std::fabs(a[i] - b[i]));
  return w;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(20261112, 15, 0, 0);

  // ---- order ----
  {
    // An estate of 10 and claims of 8 and 6. The states are what is left of the estate, 0 to 10; step A pays the first claimant
    // min(8, left), step B the second min(6, left). f is what is left.
    const std::size_t n = 11;
    Matrix A(n), B(n);
    std::vector<double> left(n);
    for (std::size_t x = 0; x < n; x++) {
      A(x, x - std::min<std::size_t>(8, x)) = 1;
      B(x, x - std::min<std::size_t>(6, x)) = 1;
      left[x] = static_cast<double>(x);
    }
    s.check("two steps that each pay a claimant leave the same of the estate in either order: no sensitivity on what is left", order_sensitivity(A, B, left, 10), 0);
    // What the first claimant has been paid is not a function of what is left; track it in the state: (left, paid to the first).
    const std::size_t m = 11 * 11;
    Matrix A2(m), B2(m);
    std::vector<double> paid(m);
    for (std::size_t l = 0; l < 11; l++)
      for (std::size_t p = 0; p < 11; p++) {
        const std::size_t x = l * 11 + p, pay = std::min<std::size_t>(8 - std::min<std::size_t>(8, p), l);
        A2(x, (l - pay) * 11 + p + pay) = 1;
        B2(x, (l - std::min<std::size_t>(6, l)) * 11 + p) = 1;
        paid[x] = static_cast<double>(p);
      }
    s.check("on what the first claimant receives, the two orders differ by 4: paid first it takes 8, paid second 4", order_sensitivity(A2, B2, paid, 10 * 11) - 4, 0);
    s.check("a step commutes with itself", order_sensitivity(A2, A2, paid, 10 * 11), 0);

    const auto D = divide({10, {8, 6}});
    s.check("the order alone moves the first claimant between 4 and 8, the second between 2 and 6", worst(D.least, {4, 2}) + worst(D.greatest, {8, 6}), 0);
    s.check("the orders mixed by lot give (6, 4); the claimants paid together, (5.714, 4.286) at rates in proportion to their claims and (5, 5) at equal rates",
            worst(D.random_arrival, {6, 4}) + worst(D.proportional, {10 * 8.0 / 14, 10 * 6.0 / 14}) + worst(D.equal_awards, {5, 5}), 1e-14);
    const auto T = divide({10, {7, 5, 4}});
    s.check("three claimants of 7, 5 and 4 on 10: mixed by lot, the Shapley value (4.5, 3, 2.5)", worst(T.random_arrival, {4.5, 3, 2.5}), 1e-14);
    s.require("the two order-neutral procedures are different rules", worst(D.random_arrival, D.proportional) > 0.2 && worst(D.proportional, D.equal_awards) > 0.5);
  }

  // ---- sequential, mixed and concurrent ----
  {
    // Two steps as generators of jump processes on five states.
    const std::size_t n = 5;
    Matrix A(n), B(n);
    for (Matrix* M : {&A, &B})
      for (std::size_t i = 0; i < n; i++) {
        double out = 0;
        for (std::size_t j = 0; j < n; j++)
          if (i != j) out += ((*M)(i, j) = rng.uniform(0, 1));
        (*M)(i, i) = -out;
      }
    const Matrix comm = plus(A * B, B * A, -1);
    const auto sequential = [&](double t) { return norm(plus(plus(calx::linalg::expm(scaled(A, t)) * calx::linalg::expm(scaled(B, t)), calx::linalg::expm(scaled(plus(A, B), t)), -1), scaled(comm, t * t / 2), -1)); };
    const auto mixed = [&](double t) {
      const Matrix ea = calx::linalg::expm(scaled(A, t)), eb = calx::linalg::expm(scaled(B, t));
      return norm(plus(scaled(plus(ea * eb, eb * ea), 0.5), calx::linalg::expm(scaled(plus(A, B), t)), -1));
    };
    const auto interleaved = [&](std::size_t k) {
      const Matrix step = calx::linalg::expm(scaled(A, 1.0 / static_cast<double>(k))) * calx::linalg::expm(scaled(B, 1.0 / static_cast<double>(k)));
      Matrix P = Matrix::identity(n);
      for (std::size_t i = 0; i < k; i++) P = P * step;
      return norm(plus(P, calx::linalg::expm(plus(A, B)), -1));
    };
    s.check("A then B differs from A and B together by half the commutator times t², the rest of third order (halving t divides it by eight)", sequential(0.01) / sequential(0.005) - 8, 0.5);
    s.check("the order drawn by lot agrees with A and B together to second order: what is left is of third order", mixed(0.01) / mixed(0.005) - 8, 0.5);
    s.require("the commutator does not vanish, and what is left of the mixture is smaller than the sequential bias", norm(comm) > 0.1 && mixed(0.04) < 0.2 * norm(scaled(comm, 0.04 * 0.04 / 2)));
    s.check("fine interleaving tends to A and B together, at the rate 1/n (doubling n halves the gap)", interleaved(64) / interleaved(128) - 2, 0.05);
  }

  // ---- procedures with one law of outcomes ----
  {
    // Two parties, each heard or not; then a fair lottery awards. Histories (heard 1, heard 2, award): eight.
    Procedure P, Q;
    std::vector<std::size_t> mirror(8);
    for (std::size_t h = 0; h < 8; h++) {
      const bool a = h & 1, b = h & 2;
      P.weight.push_back((a ? 0.9 : 0.1) * (b ? 0.9 : 0.1) * 0.5);
      Q.weight.push_back((a ? 0.9 : 0.1) * (b ? 0.1 : 0.9) * 0.5);
      P.outcome.push_back((h >> 2) & 1);
      P.admissible.push_back(a && b);
      mirror[h] = (b ? 1 : 0) | (a ? 2 : 0) | ((h & 4) ^ 4);  // the parties change places, and with them the award
    }
    Q.outcome = P.outcome, Q.admissible = P.admissible;
    s.check("two procedures that both award by a fair lottery have one law of outcomes", worst(outcome_law(P), outcome_law(Q)), 1e-16);
    const auto d = compare(P, Q);
    s.check("their whole discrepancy, 1.7578 nats, is hidden from a record of outcomes", std::fabs(d.visible) + std::fabs(d.hidden - 1.7578) + std::fabs(d.total - d.hidden), 5e-5);
    s.check("the histories in which both parties are heard weigh 0.81 in the one and 0.09 in the other", std::fabs(admissible_weight(P) - 0.81) + std::fabs(admissible_weight(Q) - 0.09), 1e-15);
    s.check("the first treats the parties alike; the second differs from its mirror image by 3.5156 nats", std::fabs(asymmetry(P, mirror)) + std::fabs(asymmetry(Q, mirror) - 3.5156), 5e-5);
    // When the award depends on the hearings: right with probability 0.9, 0.7 and 0.5 after two, one and no hearings.
    Procedure P2, Q2;
    for (std::size_t h = 0; h < 8; h++) {
      const bool a = h & 1, b = h & 2, right = h & 4;
      const double accuracy = a && b ? 0.9 : a || b ? 0.7 : 0.5, award = right ? accuracy : 1 - accuracy;
      P2.weight.push_back((a ? 0.9 : 0.1) * (b ? 0.9 : 0.1) * award);
      Q2.weight.push_back((a ? 0.9 : 0.1) * (b ? 0.1 : 0.9) * award);
      P2.outcome.push_back(right ? 1 : 0);
    }
    Q2.outcome = P2.outcome;
    s.check("when the award depends on the hearings the two are right with probability 0.86 and 0.70", std::fabs(reliability(P2, {0, 1}) - 0.86) + std::fabs(reliability(Q2, {0, 1}) - 0.70), 1e-15);
    s.require("and their difference is now partly visible in the outcomes", compare(P2, Q2).visible > 0.01);
  }

  // ---- the hearing condition on a history ----
  {
    using namespace markets::foam;
    // Junctions: two parties (0, 1) and a court (2). Facts: each party's case before the court. Types: 0 a hearing, 1 a judgment.
    History H;
    H.junctions = 3, H.begin = 0, H.end = 5;
    H.facts.push_back({0, 2, 0, none, 0.0, true});
    H.facts.push_back({1, 2, 0, none, 0.0, true});
    H.events.push_back({1, 0, {{0, Touch::modify, 1.0}}, {}});                               // the first party is heard
    H.events.push_back({2, 1, {{0, Touch::modify, 2.0}, {1, Touch::modify, 2.0}}, {}});     // judgment on both
    H.events.push_back({3, 0, {{1, Touch::modify, 3.0}}, {}});                               // the second is heard afterwards
    const auto F = foam(H);
    s.require("a party heard by the court before its judgment meets the hearing condition", heard(H, F, 0, 2, 0, 1));
    s.require("a party heard only after the judgment does not", !heard(H, F, 1, 2, 0, 1));
    s.require("a hearing before another junction than the one that decides does not count", !heard(H, F, 0, 1, 0, 1));
  }
  return s.done();
}
