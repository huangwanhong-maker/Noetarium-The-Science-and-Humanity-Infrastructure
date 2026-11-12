// A market as a system that generates its own histories (theory note, Part I).
//
// In the fragment where events change quantities only, a state is a vector x over places (a subject's position in a kind) and
// an event of type a moves it by a fixed vector ν_a: a transfer q(e_j − e_i), an exchange at the ratio r, q(e_ik − e_jk) +
// rq(e_jl − e_il), an issuance q(e_holder − e_issuer). With a propensity λ_a(x) for each type the system is a Markov jump
// process, and everything below follows from the matrix ν and the propensities.
//
//   invariants     the linear quantities c·x kept by every history: a basis of ker νᵀ (Proposition O.1). A kind moved only
//                  by transfers and exchanges keeps its total; a claim issued from nothing keeps the total zero
//   drift          b(ξ) = Σ ν_a λ_a(ξ): the classical regime ξ̇ = b(ξ)
//   diffusion      σσᵀ = Σ λ_a ν_a ν_aᵀ: the Gaussian correction
//   hamiltonian    H(ξ, θ) = Σ λ_a(ξ)(e^{θ·ν_a} − 1): the path regime. An invariant c is kept in all three: c·b = 0,
//                  cᵀσσᵀc = 0, and H(ξ, θ + sc) = H(ξ, θ) for every s (Proposition D.1)
//   simulate       a history by the next-event method, with each type's clock θ_a = ∫ λ_a dt beside its count: a type is a
//                  unit-rate Poisson process in its own clock (Definition 4.1; Ethier and Kurtz 1986, Chapter 6)
//   resistance     the distance a tracer's rates of passage induce on the subjects: the effective resistance of the network
//                  of rates. For the tracer's walk on n nodes the expected time from i to j and back is n R_ij (Definition
//                  4.2 and Proposition D.4; Klein and Randić 1993)
//
// And a market from rules, for trying what a record shows where no record is to hand:
//
//   generate       subjects holding money and goods. Each unit of money buys at the hazard `velocity` (so money faces generate
//                  payments in proportion to their labels); a purchase is an exchange of `payment` of money for a good at
//                  its reference price, moved by noise and by the two parties' bargaining; subjects also barter one good for
//                  another. The result is a record of movements whose truth is known: the velocity, the reference prices,
//                  and who has the better of whom
//
// Sources: W. Huang (2026), Unified Design Draft v0.9, §3.3 to §3.5 and §4.1 to §4.4.

#pragma once

#include <markets/foam/record.hpp>

#include <calx/linalg/dense.hpp>
#include <calx/linalg/eigen.hpp>
#include <calx/random/stream.hpp>
#include <physis/discretisation/kirchhoff.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <stdexcept>
#include <utility>
#include <vector>

namespace markets::system {

struct Grammar {
  std::size_t places = 0;
  struct Type {
    std::vector<std::pair<std::size_t, double>> change;  // ν_a: (place, amount)
  };
  std::vector<Type> types;

  std::size_t transfer(std::size_t from, std::size_t to, double q = 1) {
    types.push_back({{{from, -q}, {to, q}}});
    return types.size() - 1;
  }
  // i gives q of k and receives rq of l from j: the four places are i's and j's positions in k and in l.
  std::size_t exchange(std::size_t ik, std::size_t jk, std::size_t il, std::size_t jl, double q, double r) {
    types.push_back({{{ik, -q}, {jk, q}, {jl, -r * q}, {il, r * q}}});
    return types.size() - 1;
  }
  // A claim of q appears at the holder and −q at its issuer.
  std::size_t issuance(std::size_t holder, std::size_t issuer, double q = 1) {
    types.push_back({{{holder, q}, {issuer, -q}}});
    return types.size() - 1;
  }
};

inline void validate(const Grammar& G) {
  for (const auto& t : G.types)
    for (const auto& [p, q] : t.change)
      if (p >= G.places || !std::isfinite(q)) throw std::invalid_argument("markets::system::Grammar: an event changes places of the grammar by finite amounts");
}

// c·ν_a for each event type.
inline std::vector<double> increments(const Grammar& G, const std::vector<double>& c) {
  if (c.size() != G.places) throw std::invalid_argument("markets::system::increments: a coefficient for each place");
  std::vector<double> out(G.types.size(), 0.0);
  for (std::size_t a = 0; a < G.types.size(); a++)
    for (const auto& [p, q] : G.types[a].change) out[a] += c[p] * q;
  return out;
}

// An orthonormal basis of ker νᵀ: the eigenvectors of ν νᵀ whose eigenvalue is zero (below `tolerance` times the greatest).
inline std::vector<std::vector<double>> invariants(const Grammar& G, double tolerance = 1e-10) {
  validate(G);
  calx::linalg::Matrix M(G.places);
  for (const auto& t : G.types)
    for (const auto& [p, q] : t.change)
      for (const auto& [p2, q2] : t.change) M(p, p2) += q * q2;
  const auto E = calx::linalg::symmetric_eigen(M);
  const double scale = std::max(1.0, E.values.empty() ? 1.0 : std::fabs(E.values.back()));
  std::vector<std::vector<double>> basis;
  for (std::size_t k = 0; k < E.values.size(); k++) {
    if (std::fabs(E.values[k]) > tolerance * scale) continue;
    std::vector<double> c(G.places);
    for (std::size_t i = 0; i < G.places; i++) c[i] = E.vectors(i, k);
    basis.push_back(std::move(c));
  }
  return basis;
}

inline std::vector<double> drift(const Grammar& G, const std::vector<double>& rates) {
  if (rates.size() != G.types.size()) throw std::invalid_argument("markets::system::drift: a rate for each event type");
  std::vector<double> b(G.places, 0.0);
  for (std::size_t a = 0; a < G.types.size(); a++)
    for (const auto& [p, q] : G.types[a].change) b[p] += rates[a] * q;
  return b;
}

inline calx::linalg::Matrix diffusion(const Grammar& G, const std::vector<double>& rates) {
  if (rates.size() != G.types.size()) throw std::invalid_argument("markets::system::diffusion: a rate for each event type");
  calx::linalg::Matrix S(G.places);
  for (std::size_t a = 0; a < G.types.size(); a++)
    for (const auto& [p, q] : G.types[a].change)
      for (const auto& [p2, q2] : G.types[a].change) S(p, p2) += rates[a] * q * q2;
  return S;
}

inline double hamiltonian(const Grammar& G, const std::vector<double>& rates, const std::vector<double>& theta) {
  if (rates.size() != G.types.size() || theta.size() != G.places) throw std::invalid_argument("markets::system::hamiltonian: a rate for each event type and a momentum for each place");
  double h = 0;
  for (std::size_t a = 0; a < G.types.size(); a++) {
    double tv = 0;
    for (const auto& [p, q] : G.types[a].change) tv += theta[p] * q;
    h += rates[a] * std::expm1(tv);
  }
  return h;
}

struct Run {
  std::vector<double> times;         // of the events
  std::vector<std::size_t> types;    // their types
  std::vector<double> state;         // at the end
  std::vector<double> clock;         // θ_a = ∫ λ_a dt, for each type
  std::vector<std::size_t> count;    // the events of each type
};

// A history from x₀ over [0, horizon]: `propensities(x)` gives λ_a(x) for every type.
inline Run simulate(const Grammar& G, const std::function<std::vector<double>(const std::vector<double>&)>& propensities, std::vector<double> x, double horizon,
                    calx::random::Stream& rng, std::size_t most_events = 10000000) {
  validate(G);
  if (x.size() != G.places) throw std::invalid_argument("markets::system::simulate: a quantity for each place");
  Run R;
  R.clock.assign(G.types.size(), 0.0);
  R.count.assign(G.types.size(), 0);
  double t = 0;
  while (R.times.size() < most_events) {
    const auto rate = propensities(x);
    double total = 0;
    for (double r : rate) {
      if (!(r >= 0)) throw std::invalid_argument("markets::system::simulate: propensities are not negative");
      total += r;
    }
    const double wait = total > 0 ? rng.exponential() / total : horizon - t + 1;
    const double step = std::min(wait, horizon - t);
    for (std::size_t a = 0; a < rate.size(); a++) R.clock[a] += rate[a] * step;
    if (t + wait > horizon) break;
    t += wait;
    double u = rng.uniform() * total;
    std::size_t a = 0;
    while (a + 1 < rate.size() && u >= rate[a]) u -= rate[a++];
    for (const auto& [p, q] : G.types[a].change) x[p] += q;
    R.times.push_back(t);
    R.types.push_back(a);
    R.count[a]++;
  }
  R.state = std::move(x);
  return R;
}

// The effective resistance between two nodes of the network of symmetric rates w (w(i, j) the rate of passage between i and j).
inline double resistance(const calx::linalg::Matrix& w, std::size_t i, std::size_t j) {
  physis::discretisation::ConductanceNetwork net;
  net.vertices = w.n;
  for (std::size_t a = 0; a < w.n; a++)
    for (std::size_t b = a + 1; b < w.n; b++) {
      if (std::fabs(w(a, b) - w(b, a)) > 1e-12 * (1 + std::fabs(w(a, b)))) throw std::invalid_argument("markets::system::resistance: rates of passage are symmetric");
      if (w(a, b) > 0) net.add(a, b, w(a, b));
    }
  if (i == j) return 0;
  return 1.0 / physis::discretisation::effective_conductance(net, i, j);
}

// ---- a market from rules --------------------------------------------------------------------------------------------------

struct Rules {
  std::size_t subjects = 30, goods = 2;  // the kinds are money (0) and the goods (1 to goods)
  double horizon = 50;
  double velocity = 2.5;                 // the hazard at which a unit of money buys
  double payment = 1;                    // the money of one purchase
  std::vector<double> price;             // the reference price of each good in money; empty for 1, 2, 3, …
  double noise = 0.02;                   // the standard deviation of a trade's log price about its reference
  std::vector<double> bargaining;        // β for each subject: a trade's log ratio moves by β_giver − β_taker; empty for none
  double barter = 0.2;                   // the hazard at which a subject barters one good for another
  double money_each = 20, goods_each = 200;  // the opening positions
  bool ring = false;                     // partners are the two neighbours on a ring; else any other subject
  std::uint64_t seed = 20261113;
};

struct Generated {
  foam::Record record;
  std::vector<double> price;       // the reference prices used, by good
  std::vector<double> bargaining;  // β, by subject
  double money = 0;                // the money in the record, constant
};

inline Generated generate(const Rules& P) {
  const std::size_t n = P.subjects, K = P.goods + 1;
  if (n < 3 || P.goods < 1 || !(P.horizon > 0) || !(P.velocity >= 0) || !(P.payment > 0) || !(P.barter >= 0) || !(P.money_each >= 0))
    throw std::invalid_argument("markets::system::generate: three subjects or more, a good, a horizon, and rates that are not negative");
  Generated G;
  G.price = P.price;
  if (G.price.empty())
    for (std::size_t g = 0; g < P.goods; g++) G.price.push_back(static_cast<double>(g + 1));
  G.bargaining = P.bargaining.empty() ? std::vector<double>(n, 0.0) : P.bargaining;
  if (G.price.size() != P.goods || G.bargaining.size() != n) throw std::invalid_argument("markets::system::generate: a price for each good and a bargaining strength for each subject");
  auto& R = G.record;
  R.subjects = n;
  R.kinds = K;
  R.begin = 0;
  R.end = P.horizon;
  R.opening.assign(n, std::vector<double>(K, P.goods_each));
  std::vector<double> m(n, P.money_each);
  for (std::size_t s = 0; s < n; s++) R.opening[s][0] = P.money_each;
  G.money = P.money_each * static_cast<double>(n);
  calx::random::Stream rng(P.seed, 31, 0, 0);
  const auto partner = [&](std::size_t i) {
    if (P.ring) return rng.uniform() < 0.5 ? (i + 1) % n : (i + n - 1) % n;
    const std::size_t j = static_cast<std::size_t>(rng.below(n - 1));
    return j >= i ? j + 1 : j;
  };
  double t = 0;
  std::size_t event = 0;
  const double buying = P.velocity * G.money / P.payment, bartering = P.barter * static_cast<double>(n) * (P.goods > 1 ? 1.0 : 0.0);
  if (!(buying + bartering > 0)) return G;
  for (;;) {
    t += rng.exponential() / (buying + bartering);
    if (t > P.horizon) break;
    if (rng.uniform() * (buying + bartering) < buying) {
      // A purchase: the buyer is drawn in proportion to its money, so that each unit of money buys at the same hazard.
      double u = rng.uniform() * G.money;
      std::size_t i = 0;
      while (i + 1 < n && u >= m[i]) u -= m[i++];
      const std::size_t j = partner(i), g = 1 + static_cast<std::size_t>(rng.below(P.goods));
      const double pay = std::min(P.payment, m[i]);
      if (!(pay > 0)) continue;
      // The buyer gives money and takes the good: ln(good per money) = −ln P_g + β_buyer − β_seller + noise.
      const double a = -std::log(G.price[g - 1]) + G.bargaining[i] - G.bargaining[j] + P.noise * rng.normal();
      R.movements.push_back({event, t, i, j, 0, pay});
      R.movements.push_back({event, t, j, i, g, pay * std::exp(a)});
      m[i] -= pay;
      m[j] += pay;
    } else {
      const std::size_t i = static_cast<std::size_t>(rng.below(n)), j = partner(i);
      const std::size_t g = 1 + static_cast<std::size_t>(rng.below(P.goods));
      std::size_t g2 = 1 + static_cast<std::size_t>(rng.below(P.goods - 1));
      if (g2 >= g) g2++;
      const double a = std::log(G.price[g - 1] / G.price[g2 - 1]) + G.bargaining[i] - G.bargaining[j] + P.noise * rng.normal();
      R.movements.push_back({event, t, i, j, g, 1.0});
      R.movements.push_back({event, t, j, i, g2, std::exp(a)});
    }
    event++;
  }
  return G;
}

}  // namespace markets::system
