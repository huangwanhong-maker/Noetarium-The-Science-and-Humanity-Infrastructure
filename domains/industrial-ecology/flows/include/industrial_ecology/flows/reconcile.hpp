// Data reconciliation: measured flows made to balance, unknown flows found from the balances, and a test
// of whether the data can balance at all (theory note §3.2).
//
// Each variable is measured (a value x̃ with standard deviation σ > 0), known exactly (σ = 0), or unknown
// (no value). The balances A x = 0 split as A_M x_M + A_U x_U = b, b = −A_E x_E. Following Crowe (1983,
// AIChE J. 29: 881–888) the unknowns are eliminated by a matrix P whose rows span the null space of A_Uᵀ
// (the last columns of Q in A_U Π = Q R), leaving the redundant balances G x_M = c, G = P A_M, c = P b.
// Their independent rows (by QR with pivoting of Gᵀ) give the weighted least-squares estimate
//
//   x̂_M = x̃ − V Gᵀ W⁻¹ r,   W = G V Gᵀ,   r = G x̃ − c,   Cov(x̂_M) = V − V Gᵀ W⁻¹ G V,
//
// the estimate STAN computes (Cencic and Rechberger 2008, J. Environ. Eng. Manag. 18: 3–7; Narasimhan
// and Jordache 2000, Data Reconciliation and Gross Error Detection, ch. 3). Under normal errors and
// balances that hold, χ² = rᵀ W⁻¹ r follows χ² with rank G degrees of freedom: a small p-value says the
// measurements and the balances disagree beyond their stated errors. Each measurement's normalised
// adjustment (x̂ − x̃)/√(σ² − σ̂²) is the test of a gross error at it (Narasimhan and Jordache, §7.3).
// The unknowns are then x̂_U = A_U⁺ (b − A_M x̂_M); an unknown is observable when it is fixed by the
// balances (orthogonal to the null space of A_U), and only then given a value and an error.
//
// Accuracy: by orthogonal factorisations throughout, so as accurate as the conditioning of W allows.

#pragma once

#include <industrial_ecology/flows/system.hpp>

#include <calx/linalg/qr.hpp>
#include <tyche/probability/distributions.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <stdexcept>
#include <vector>

namespace industrial_ecology::flows {

struct Observation {
  double value = std::numeric_limits<double>::quiet_NaN();  // NaN: unknown
  double sd = 0;                                            // > 0 measured, 0 exact
};
inline Observation measured(double value, double sd) { return {value, sd}; }
inline Observation exact(double value) { return {value, 0}; }
inline Observation unknown() { return {}; }

struct Reconciled {
  std::vector<double> value, sd;     // per variable; NaN where an unknown is not observable
  std::vector<int> kind;             // 0 measured, 1 exact, 2 unknown
  std::vector<bool> observable;      // measured and exact variables are; an unknown when fixed by the balances
  std::vector<double> adjustment;    // measured: x̂ − x̃ (0 otherwise)
  std::vector<double> normalised;    // measured: (x̂ − x̃)/√(σ² − σ̂²); NaN where the variance is not reduced
  double chi2 = 0, p = 1;
  std::size_t redundancy = 0;        // the degrees of freedom: independent balances left after the unknowns
  double balance = 0;                // max |A x̂| over the processes, with unobservable unknowns at the basic solution
};

inline Reconciled reconcile(const System& s, const std::vector<Observation>& obs) {
  using calx::linalg::Matrix;
  using calx::linalg::Rectangular;
  const std::size_t nv = s.variables.size(), np = s.processes.size();
  if (obs.size() != nv) throw std::invalid_argument("industrial_ecology::flows::reconcile: one observation per variable");
  const Rectangular A = s.incidence();
  std::vector<std::size_t> M, E, U;
  Reconciled R;
  R.kind.resize(nv);
  for (std::size_t j = 0; j < nv; j++) {
    if (std::isnan(obs[j].value)) U.push_back(j), R.kind[j] = 2;
    else if (obs[j].sd > 0 && std::isfinite(obs[j].sd)) M.push_back(j), R.kind[j] = 0;
    else if (obs[j].sd == 0) E.push_back(j), R.kind[j] = 1;
    else throw std::invalid_argument("industrial_ecology::flows::reconcile: a standard deviation ≥ 0: " + s.variables[j].name);
  }
  const auto columns = [&](const std::vector<std::size_t>& idx) {
    Rectangular B(np, idx.size());
    for (std::size_t i = 0; i < np; i++)
      for (std::size_t k = 0; k < idx.size(); k++) B(i, k) = A(i, idx[k]);
    return B;
  };
  const Rectangular AM = columns(M), AU = columns(U), AE = columns(E);
  std::vector<double> b(np, 0.0);
  for (std::size_t i = 0; i < np; i++)
    for (std::size_t k = 0; k < E.size(); k++) b[i] -= AE(i, k) * obs[E[k]].value;
  // P: rows spanning the null space of A_Uᵀ.
  std::vector<std::vector<double>> P;
  if (U.empty()) {
    for (std::size_t i = 0; i < np; i++) {
      std::vector<double> e(np, 0.0);
      e[i] = 1;
      P.push_back(e);
    }
  } else {
    const auto f = calx::linalg::qr(AU);
    for (std::size_t j = f.rank; j < np; j++) {
      std::vector<double> e(np, 0.0);
      e[j] = 1;
      P.push_back(calx::linalg::apply_q(f, e));
    }
  }
  // G = P A_M, c = P b; keep independent rows.
  const std::size_t m = M.size();
  std::vector<std::vector<double>> G;
  std::vector<double> c;
  for (const auto& row : P) {
    std::vector<double> g(m, 0.0);
    double ci = 0;
    for (std::size_t i = 0; i < np; i++) {
      if (row[i] == 0) continue;
      for (std::size_t k = 0; k < m; k++) g[k] += row[i] * AM(i, k);
      ci += row[i] * b[i];
    }
    G.push_back(g), c.push_back(ci);
  }
  std::vector<std::size_t> keep;
  if (!G.empty() && m > 0) {
    // Scale each measured column by its σ, so that the rank decision is made in units of the errors.
    Rectangular Gt(m, G.size());
    for (std::size_t r = 0; r < G.size(); r++)
      for (std::size_t k = 0; k < m; k++) Gt(k, r) = G[r][k] * obs[M[k]].sd;
    const auto f = calx::linalg::qr(Gt, true, 1e-10);
    for (std::size_t q = 0; q < f.rank; q++) keep.push_back(f.permutation[q]);
  }
  R.value.assign(nv, std::numeric_limits<double>::quiet_NaN());
  R.sd.assign(nv, std::numeric_limits<double>::quiet_NaN());
  R.observable.assign(nv, true);
  R.adjustment.assign(nv, 0.0);
  R.normalised.assign(nv, std::numeric_limits<double>::quiet_NaN());
  for (std::size_t j : E) R.value[j] = obs[j].value, R.sd[j] = 0;
  std::vector<double> xm(m), V(m);
  for (std::size_t k = 0; k < m; k++) xm[k] = obs[M[k]].value, V[k] = obs[M[k]].sd * obs[M[k]].sd;
  Matrix Vhat(m);  // covariance of x̂_M
  for (std::size_t k = 0; k < m; k++) Vhat(k, k) = V[k];
  R.redundancy = keep.size();
  if (!keep.empty()) {
    const std::size_t q = keep.size();
    Matrix W(q);
    std::vector<double> r(q, 0.0);
    for (std::size_t a = 0; a < q; a++) {
      const auto& ga = G[keep[a]];
      r[a] = -c[keep[a]];
      for (std::size_t k = 0; k < m; k++) r[a] += ga[k] * xm[k];
      for (std::size_t bq = 0; bq < q; bq++) {
        const auto& gb = G[keep[bq]];
        double w = 0;
        for (std::size_t k = 0; k < m; k++) w += ga[k] * V[k] * gb[k];
        W(a, bq) = w;
      }
    }
    const Matrix Winv = calx::linalg::solve(W, Matrix::identity(q));
    std::vector<double> lambda(q, 0.0);
    for (std::size_t a = 0; a < q; a++)
      for (std::size_t bq = 0; bq < q; bq++) lambda[a] += Winv(a, bq) * r[bq];
    for (std::size_t a = 0; a < q; a++) R.chi2 += r[a] * lambda[a];
    R.p = tyche::probability::ChiSquared{static_cast<double>(q)}.survival(std::max(R.chi2, 0.0));
    // H = V Gᵀ (m × q); x̂ = x̃ − H λ; V̂ = V − H W⁻¹ Hᵀ.
    Rectangular H(m, q);
    for (std::size_t k = 0; k < m; k++)
      for (std::size_t a = 0; a < q; a++) H(k, a) = V[k] * G[keep[a]][k];
    for (std::size_t k = 0; k < m; k++) {
      double d = 0;
      for (std::size_t a = 0; a < q; a++) d += H(k, a) * lambda[a];
      xm[k] -= d;
    }
    for (std::size_t k = 0; k < m; k++)
      for (std::size_t l = 0; l < m; l++) {
        double t = 0;
        for (std::size_t a = 0; a < q; a++)
          for (std::size_t bq = 0; bq < q; bq++) t += H(k, a) * Winv(a, bq) * H(l, bq);
        Vhat(k, l) -= t;
      }
  }
  for (std::size_t k = 0; k < m; k++) {
    const std::size_t j = M[k];
    R.value[j] = xm[k];
    R.sd[j] = std::sqrt(std::max(Vhat(k, k), 0.0));
    R.adjustment[j] = xm[k] - obs[j].value;
    const double reduced = V[k] - Vhat(k, k);
    if (reduced > 1e-14 * V[k]) R.normalised[j] = R.adjustment[j] / std::sqrt(reduced);
  }
  if (!U.empty()) {
    // rhs = b − A_M x̂_M, in the range of A_U after reconciliation; x̂_U by least squares.
    std::vector<double> rhs = b;
    for (std::size_t i = 0; i < np; i++)
      for (std::size_t k = 0; k < m; k++) rhs[i] -= AM(i, k) * xm[k];
    const auto f = calx::linalg::qr(AU);
    const auto xu = calx::linalg::least_squares(f, rhs);
    // Observability: e_i orthogonal to the null space of A_U, from the QR of A_Uᵀ.
    const std::size_t u = U.size();
    const auto g = calx::linalg::qr(AU.transpose());
    std::vector<std::vector<double>> null;
    for (std::size_t j = g.rank; j < u; j++) {
      std::vector<double> e(u, 0.0);
      e[j] = 1;
      null.push_back(calx::linalg::apply_q(g, e));
    }
    // K = A_U⁺ (a generalised inverse) column by column; Cov(x̂_U) = K A_M V̂ A_Mᵀ Kᵀ.
    Rectangular K(u, np);
    for (std::size_t i = 0; i < np; i++) {
      std::vector<double> e(np, 0.0);
      e[i] = 1;
      const auto col = calx::linalg::least_squares(f, e);
      for (std::size_t k = 0; k < u; k++) K(k, i) = col[k];
    }
    Rectangular KAM(u, m);
    for (std::size_t k = 0; k < u; k++)
      for (std::size_t l = 0; l < m; l++)
        for (std::size_t i = 0; i < np; i++) KAM(k, l) += K(k, i) * AM(i, l);
    for (std::size_t k = 0; k < u; k++) {
      bool obs_k = true;
      for (const auto& v : null) obs_k = obs_k && std::fabs(v[k]) < 1e-9;
      const std::size_t j = U[k];
      R.observable[j] = obs_k;
      R.value[j] = xu[k];  // the basic solution; meaningful only where observable
      if (obs_k) {
        double var = 0;
        for (std::size_t a = 0; a < m; a++)
          for (std::size_t bq = 0; bq < m; bq++) var += KAM(k, a) * Vhat(a, bq) * KAM(k, bq);
        R.sd[j] = std::sqrt(std::max(var, 0.0));
      }
    }
  }
  const auto imb = s.imbalance(R.value);
  for (double v : imb) R.balance = std::max(R.balance, std::fabs(v));
  for (std::size_t j : U)
    if (!R.observable[j]) R.value[j] = std::numeric_limits<double>::quiet_NaN();
  return R;
}

}  // namespace industrial_ecology::flows
