// Valuation: the ratios at which kinds are exchanged, as a connection, and what is left of them when units are forgotten
// (theory note, Part I §3 and Part III §2).
//
// An exchange sets a ratio: a = ln(units of l obtained for a unit of k). The ratios of a record are a 1-cochain on the graph
// of kinds, one edge for each exchange. Its sum round a cycle of exchanges is the cycle's holonomy; e^h multiplies a holding
// carried round. A price is a potential of the cochain, π with a = π_k − π_l on every edge: it exists exactly when every
// holonomy vanishes, is fixed up to one constant, and changes with the units of the kinds while the holonomies do not.
//
//   reference        the least-squares potential π* of a record's ratios (weights w on the exchanges), by physis's Hodge
//                    decomposition on the graph of kinds; the residual ρ = a − (π*_k − π*_l) of each exchange; and for each
//                    subject its advantage, Σ ρ over the exchanges in which it gave k, less Σ ρ over those in which it gave l.
//                    Residuals and advantages do not change with the units; the advantages add up to zero; the holonomy
//                    of a cycle of exchanges is the sum of its residuals (Propositions O.3 and C.7)
//   holonomy         of a closed path of kinds, each step taken at a given ratio
//   lenses           through time: two exchanges of the same two kinds at t₀ and t₁ and the two holdings between them make the
//                    smallest loop with a duration. With a carry ρ_k per unit of time on holding kind k its holonomy is
//                        h = (a₀ − a₁) + (ρ_l − ρ_k)(t₁ − t₀):
//                    a geometric part, fixed by the ratios whatever the durations, and a dynamical part in proportion to
//                    the duration (Proposition C.6; h = 0 is the cost of carry, of which covered interest parity is a case)
//   dissipate        ratios moved by arbitrage on the triangles of an exchange complex, ȧ = −γ CᵀC a (C the holonomies of
//                    the triangles): the divergence of a is kept, the triangles' holonomies die away, and a tends to its
//                    part without them (Proposition D.5; the theory note amends its last clause)
//   CycleMap         a cycle acting on bundles of several kinds by an invertible matrix Φ: after n rounds the bundle is Φⁿx.
//                    Its scalar defect ln|det Φ| can vanish while one kind compounds and another is extinguished: what
//                    repeats is read from the eigenvalues (Proposition C.8)
//
// Sources: W. Huang (2026), Unified Design Draft v0.9, §3.6 and §4.6; Market Foam v0.9a, §7.3; Cases v0.9b, §4.2.
// Jiang, Lim, Yao and Ye (2011), Math. Program. B 127: 203–244 (the decomposition of a pairwise comparison).

#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/eigenvalues.hpp>
#include <calx/linalg/expm.hpp>
#include <physis/discretisation/hodge.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace markets::valuation {

// An exchange as a ratio: party `giver` gave kind k and received kind l at a = ln(units of l for a unit of k) from `taker`.
struct Trade {
  std::size_t giver = 0, taker = 0, k = 0, l = 0;
  double a = 0, time = 0, weight = 1;
};

struct Reference {
  std::vector<double> potential;   // π*, one for each kind, summing to zero on each connected part of the graph of kinds
  std::vector<double> residual;    // ρ, one for each trade
  std::vector<double> advantage;   // one for each subject
  double fitted = 0, unfitted = 0; // Σ w (π*_k − π*_l)² and Σ w ρ²
};

inline morphe::topology::CWComplex graph_of(const std::vector<Trade>& trades, std::size_t kinds) {
  morphe::topology::CWComplex X;
  X.vertices = kinds;
  for (const auto& t : trades) {
    if (t.k >= kinds || t.l >= kinds || t.k == t.l) throw std::invalid_argument("markets::valuation: a trade is between two kinds of the record");
    X.edges.push_back({t.k, t.l});
  }
  return X;
}

inline Reference reference(const std::vector<Trade>& trades, std::size_t kinds, std::size_t subjects) {
  Reference R;
  R.potential.assign(kinds, 0.0);
  R.advantage.assign(subjects, 0.0);
  if (trades.empty()) return R;
  const auto X = graph_of(trades, kinds);
  physis::discretisation::Cochain a(trades.size());
  physis::discretisation::Weights W;
  W.edge.resize(trades.size());
  for (std::size_t v = 0; v < trades.size(); v++) {
    if (!(trades[v].weight > 0)) throw std::invalid_argument("markets::valuation::reference: weights are positive");
    a[v] = trades[v].a;
    W.edge[v] = trades[v].weight;
  }
  const auto H = physis::discretisation::hodge_decomposition(X, a, W);
  // The exact part is d₀α, α at the head less α at the tail of each edge k → l: so π = −α.
  for (std::size_t k = 0; k < kinds; k++) R.potential[k] = -H.potential[k];
  R.residual.resize(trades.size());
  for (std::size_t v = 0; v < trades.size(); v++) {
    const auto& t = trades[v];
    const double fit = R.potential[t.k] - R.potential[t.l];
    R.residual[v] = t.a - fit;
    R.fitted += t.weight * fit * fit;
    R.unfitted += t.weight * R.residual[v] * R.residual[v];
    if (t.giver >= subjects || t.taker >= subjects) throw std::invalid_argument("markets::valuation::reference: a trade is between subjects of the record");
    R.advantage[t.giver] += R.residual[v];
    R.advantage[t.taker] -= R.residual[v];
  }
  return R;
}

// The holonomy of a closed path: step i goes from kind path[i] to path[i + 1] (the last back to the first) at ratios[i].
inline double holonomy(const std::vector<double>& ratios) {
  double h = 0;
  for (double a : ratios) h += a;
  return h;
}

// A loop through time: the same two kinds exchanged at two times, and held between.
struct Lens {
  std::size_t k = 0, l = 0, first = 0, second = 0;  // the kinds (k < l) and the two trades
  double begin = 0, end = 0;
  double geometric = 0, dynamical = 0;
  double holonomy() const { return geometric + dynamical; }
};

// One lens for each two consecutive exchanges of the same two kinds; `carry[k]` is the rate ρ_k of holding kind k (empty: none).
inline std::vector<Lens> lenses(const std::vector<Trade>& trades, const std::vector<double>& carry = {}) {
  std::map<std::pair<std::size_t, std::size_t>, std::vector<std::size_t>> by_pair;
  for (std::size_t v = 0; v < trades.size(); v++) by_pair[{std::min(trades[v].k, trades[v].l), std::max(trades[v].k, trades[v].l)}].push_back(v);
  std::vector<Lens> out;
  for (auto& [pair, list] : by_pair) {
    std::stable_sort(list.begin(), list.end(), [&](std::size_t x, std::size_t y) { return trades[x].time < trades[y].time; });
    const auto oriented = [&](std::size_t v) { return trades[v].k == pair.first ? trades[v].a : -trades[v].a; };  // ln(units of l per unit of k), k < l
    for (std::size_t i = 0; i + 1 < list.size(); i++) {
      Lens L;
      L.k = pair.first, L.l = pair.second, L.first = list[i], L.second = list[i + 1];
      L.begin = trades[L.first].time, L.end = trades[L.second].time;
      L.geometric = oriented(L.first) - oriented(L.second);
      const double rk = carry.empty() ? 0.0 : carry.at(L.k), rl = carry.empty() ? 0.0 : carry.at(L.l);
      L.dynamical = (rl - rk) * (L.end - L.begin);
      out.push_back(L);
    }
  }
  return out;
}

// The ratios after a time t of arbitrage on the triangles of X at the rate γ: e^{−γ t CᵀC} a.
inline physis::discretisation::Cochain dissipate(const morphe::topology::CWComplex& X, const physis::discretisation::Cochain& a, double gamma, double t) {
  const std::size_t E = X.edges.size();
  if (a.size() != E) throw std::invalid_argument("markets::valuation::dissipate: a ratio for each edge");
  calx::linalg::Matrix M(E);  // −γ t CᵀC
  for (const auto& face : X.faces)
    for (const auto& s : face.boundary)
      for (const auto& u : face.boundary) M(s.edge, u.edge) -= gamma * t * s.sign * u.sign;
  const auto P = calx::linalg::expm(M);
  physis::discretisation::Cochain out(E, 0.0);
  for (std::size_t i = 0; i < E; i++)
    for (std::size_t j = 0; j < E; j++) out[i] += P(i, j) * a[j];
  return out;
}

// Where arbitrage on the triangles leaves the ratios: their part with no holonomy round any triangle.
inline physis::discretisation::Cochain settled(const morphe::topology::CWComplex& X, const physis::discretisation::Cochain& a) {
  const auto H = physis::discretisation::hodge_decomposition(X, a);
  physis::discretisation::Cochain out(a.size());
  for (std::size_t e = 0; e < a.size(); e++) out[e] = H.exact[e] + H.harmonic[e];
  return out;
}

struct CycleMap {
  calx::linalg::Matrix phi;
  // ln|det Φ|: the sum of the logarithms of the moduli of its eigenvalues.
  double scalar_defect() const { return std::log(std::fabs(calx::linalg::determinant(phi))); }
  std::vector<std::complex<double>> spectrum() const { return calx::linalg::eigenvalues(phi); }
  std::vector<double> after(std::vector<double> x, std::size_t rounds) const {
    if (x.size() != phi.n) throw std::invalid_argument("markets::valuation::CycleMap: a bundle of as many kinds as the map takes");
    for (std::size_t r = 0; r < rounds; r++) {
      std::vector<double> y(phi.n, 0.0);
      for (std::size_t i = 0; i < phi.n; i++)
        for (std::size_t j = 0; j < phi.n; j++) y[i] += phi(i, j) * x[j];
      x = std::move(y);
    }
    return x;
  }
};

}  // namespace markets::valuation
