// Life cycle assessment by the matrix method (theory note §4; Heijungs and Suh 2002, The Computational
// Structure of Life Cycle Assessment, ch. 2–5).
//
// A product system has products and processes. Its technology matrix A (products × processes) holds
// each process's outputs (positive) and inputs (negative) per unit of operation; its intervention matrix
// B (elementary flows × processes) the flows each exchanges with the environment, emissions and
// extractions, per unit. A method's characterisation matrix C (categories × elementary flows) weighs
// them. For a functional unit, a demand f on the products, the system is scaled so that its net
// output is f:
//
//   A s = f,   g = B s,   h = C g,                                    (Heijungs and Suh, eqs. 2.18–2.24)
//
// the scaling s, the inventory g and the impacts h. A must be square and non-singular: one process for
// each product. A process with several products is made single by `partition` (its inputs and
// interventions divided among its products by declared factors, physical or economic: ISO 14044 §4.3.4)
// or by `substitute` (the co-product credited with the process it displaces: system expansion).
// Neither is chosen by the domain.
//
//   assess(system, method, f)      s, g, h; each process's contribution C B diag(s)
//   elasticities(…)                ∂h_k/∂a_ij = −λ_ki s_j and ∂h_k/∂b_lj = C_kl s_j with λ = C B A⁻¹, as
//                                  elasticities (a_ij / h_k) ∂h_k/∂a_ij: exact first-order sensitivities
//                                  (Heijungs 2010, Int. J. Life Cycle Assess. 15: 511–520; eq. 6–8)
//   monte_carlo(…)                 every non-zero coefficient drawn lognormally about its value with a
//                                  geometric standard deviation, as ecoinvent's uncertainty is stated
//                                  (Weidema et al. 2013, ecoinvent data quality guideline, §10.2); the
//                                  impacts' median and interval
//   paths(…)                       structural path analysis: with each product made on the diagonal,
//                                  A = (I − Z) D, so h = (C B D⁻¹)(I − Z)⁻¹ f, a Neumann series whose
//                                  leading walks calx finds (calx decision 0017)

#pragma once

#include <calx/linalg/dense.hpp>
#include <calx/linalg/nonnegative.hpp>
#include <calx/linalg/rectangular.hpp>
#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::lca {

using calx::linalg::Matrix;
using calx::linalg::Rectangular;

struct ProductSystem {
  std::vector<std::string> products, processes, interventions;
  Rectangular A;  // products × processes
  Rectangular B;  // interventions × processes

  void validate() const {
    if (A.rows != products.size() || A.cols != processes.size() || B.rows != interventions.size() || B.cols != processes.size())
      throw std::invalid_argument("industrial_ecology::lca: A is products × processes and B interventions × processes");
  }
  bool square() const { return products.size() == processes.size(); }
};

struct Method {
  std::vector<std::string> categories, units;
  Rectangular C;  // categories × interventions
};

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("industrial_ecology::lca: " + what);
}
inline Matrix square_of(const Rectangular& A) {
  Matrix M(A.rows);
  M.a = A.a;
  return M;
}
}  // namespace detail

// ---- multifunctional processes ----

// Process j's outputs divided among its products by factors (one per product, summing to one over the
// products j yields): j becomes one process per output, each with that output alone and the factor's
// share of j's inputs and interventions.
inline ProductSystem partition(const ProductSystem& ps, std::size_t j, const std::vector<double>& factors) {
  ps.validate();
  detail::require(j < ps.processes.size() && factors.size() == ps.products.size(), "partition: a process, and one factor per product");
  std::vector<std::size_t> outputs;
  double total = 0;
  for (std::size_t p = 0; p < ps.products.size(); p++)
    if (ps.A(p, j) > 0) outputs.push_back(p), total += factors[p];
  detail::require(outputs.size() >= 2, "partition: the process has at least two products");
  detail::require(std::fabs(total - 1) < 1e-9, "partition: the factors of its products sum to one");
  ProductSystem r;
  r.products = ps.products, r.interventions = ps.interventions;
  const std::size_t n = ps.processes.size(), cols = n - 1 + outputs.size();
  r.A = Rectangular(ps.products.size(), cols), r.B = Rectangular(ps.interventions.size(), cols);
  std::size_t c = 0;
  for (std::size_t q = 0; q < n; q++) {
    if (q != j) {
      r.processes.push_back(ps.processes[q]);
      for (std::size_t p = 0; p < ps.products.size(); p++) r.A(p, c) = ps.A(p, q);
      for (std::size_t l = 0; l < ps.interventions.size(); l++) r.B(l, c) = ps.B(l, q);
      c++;
      continue;
    }
    for (std::size_t o : outputs) {
      r.processes.push_back(ps.processes[j] + " (" + ps.products[o] + ")");
      const double f = factors[o];
      for (std::size_t p = 0; p < ps.products.size(); p++) r.A(p, c) = ps.A(p, j) > 0 ? (p == o ? ps.A(p, j) : 0.0) : f * ps.A(p, j);
      for (std::size_t l = 0; l < ps.interventions.size(); l++) r.B(l, c) = f * ps.B(l, j);
      c++;
    }
  }
  return r;
}

// System expansion: process j's co-product p displaces process r's output of p; j is credited with r's
// column scaled to the amount displaced, and its output of p disappears.
inline ProductSystem substitute(const ProductSystem& ps, std::size_t j, std::size_t p, std::size_t r) {
  ps.validate();
  detail::require(j < ps.processes.size() && r < ps.processes.size() && p < ps.products.size() && j != r, "substitute: two processes and a product");
  detail::require(ps.A(p, j) > 0 && ps.A(p, r) > 0, "substitute: both processes yield the product");
  ProductSystem out = ps;
  const double k = ps.A(p, j) / ps.A(p, r);
  for (std::size_t i = 0; i < ps.products.size(); i++) out.A(i, j) -= k * ps.A(i, r);
  for (std::size_t l = 0; l < ps.interventions.size(); l++) out.B(l, j) -= k * ps.B(l, r);
  out.A(p, j) = 0;
  out.processes[j] = ps.processes[j] + " (crediting " + ps.processes[r] + ")";
  return out;
}

// ---- the inventory and the impacts ----

struct Result {
  std::vector<double> scaling, inventory, impacts;
  Rectangular inventory_by_process;  // B diag(s)
  Rectangular contribution;          // C B diag(s), categories × processes
  Matrix inverse;                    // A⁻¹
};

inline Result assess(const ProductSystem& ps, const Method& m, const std::vector<double>& demand) {
  ps.validate();
  detail::require(ps.square(), "assess: one process per product (partition or substitute first)");
  detail::require(demand.size() == ps.products.size(), "assess: a demand on each product");
  detail::require(m.C.cols == ps.interventions.size() && m.C.rows == m.categories.size(), "assess: C is categories × interventions");
  Result R;
  const std::size_t n = ps.products.size();
  R.inverse = calx::linalg::solve(detail::square_of(ps.A), Matrix::identity(n));
  R.scaling.assign(n, 0.0);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t k = 0; k < n; k++) R.scaling[i] += R.inverse(i, k) * demand[k];
  R.inventory = calx::linalg::multiply(ps.B, R.scaling);
  R.impacts = calx::linalg::multiply(m.C, R.inventory);
  R.inventory_by_process = Rectangular(ps.interventions.size(), n);
  for (std::size_t l = 0; l < ps.interventions.size(); l++)
    for (std::size_t j = 0; j < n; j++) R.inventory_by_process(l, j) = ps.B(l, j) * R.scaling[j];
  R.contribution = m.C * R.inventory_by_process;
  return R;
}

// ---- sensitivities ----

struct Elasticity {
  char matrix = 'A';  // 'A' a technology coefficient, 'B' an intervention coefficient
  std::size_t row = 0, col = 0;
  double derivative = 0, elasticity = 0;
};

// For category k: every non-zero coefficient's derivative ∂h_k and elasticity, largest |elasticity| first.
inline std::vector<Elasticity> elasticities(const ProductSystem& ps, const Method& m, const Result& R, std::size_t k) {
  const std::size_t n = ps.products.size();
  detail::require(k < m.categories.size(), "elasticities: a category");
  const double h = R.impacts[k];
  detail::require(h != 0, "elasticities: a category with a non-zero impact");
  std::vector<double> cb(n, 0.0);  // (C B)_k,j
  for (std::size_t j = 0; j < n; j++)
    for (std::size_t l = 0; l < ps.interventions.size(); l++) cb[j] += m.C(k, l) * ps.B(l, j);
  std::vector<double> lambda(n, 0.0);  // λ_k = (C B)_k A⁻¹
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) lambda[i] += cb[j] * R.inverse(j, i);
  std::vector<Elasticity> out;
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++)
      if (ps.A(i, j) != 0) {
        const double d = -lambda[i] * R.scaling[j];
        out.push_back({'A', i, j, d, ps.A(i, j) * d / h});
      }
  for (std::size_t l = 0; l < ps.interventions.size(); l++)
    for (std::size_t j = 0; j < n; j++)
      if (ps.B(l, j) != 0) {
        const double d = m.C(k, l) * R.scaling[j];
        out.push_back({'B', l, j, d, ps.B(l, j) * d / h});
      }
  std::sort(out.begin(), out.end(), [](const Elasticity& a, const Elasticity& b) { return std::fabs(a.elasticity) > std::fabs(b.elasticity); });
  return out;
}

// ---- uncertainty ----

struct Spread {
  std::vector<double> median, lower, upper, mean;  // per category; the interval at the stated coverage
  std::size_t draws = 0;
};

inline Spread monte_carlo(const ProductSystem& ps, const Method& m, const std::vector<double>& demand, double gsd_A, double gsd_B,
                          std::size_t draws = 1000, std::uint64_t seed = 1, double coverage = 0.95) {
  detail::require(gsd_A >= 1 && gsd_B >= 1 && draws >= 10 && coverage > 0 && coverage < 1, "monte_carlo: geometric standard deviations ≥ 1, at least ten draws, a coverage in (0, 1)");
  calx::random::Stream rng(seed, 0, 0, 0);
  const double sA = std::log(gsd_A), sB = std::log(gsd_B);
  const std::size_t K = m.categories.size();
  std::vector<std::vector<double>> h(K);
  ProductSystem d = ps;
  for (std::size_t t = 0; t < draws; t++) {
    for (std::size_t i = 0; i < ps.A.a.size(); i++) d.A.a[i] = ps.A.a[i] == 0 ? 0.0 : ps.A.a[i] * std::exp(sA * rng.normal());
    for (std::size_t i = 0; i < ps.B.a.size(); i++) d.B.a[i] = ps.B.a[i] == 0 ? 0.0 : ps.B.a[i] * std::exp(sB * rng.normal());
    const auto R = assess(d, m, demand);
    for (std::size_t k = 0; k < K; k++) h[k].push_back(R.impacts[k]);
  }
  Spread S;
  S.draws = draws;
  auto quantile = [](std::vector<double> v, double p) {
    std::sort(v.begin(), v.end());
    const double pos = p * static_cast<double>(v.size() - 1);
    const std::size_t i = static_cast<std::size_t>(pos);
    const double f = pos - static_cast<double>(i);
    return i + 1 < v.size() ? v[i] * (1 - f) + v[i + 1] * f : v.back();  // Hyndman and Fan's type 7
  };
  for (std::size_t k = 0; k < K; k++) {
    double mean = 0;
    for (double v : h[k]) mean += v;
    S.mean.push_back(mean / static_cast<double>(draws));
    S.median.push_back(quantile(h[k], 0.5));
    S.lower.push_back(quantile(h[k], (1 - coverage) / 2));
    S.upper.push_back(quantile(h[k], (1 + coverage) / 2));
  }
  return S;
}

// ---- structural paths ----

// For category k: the leading walks of h_k through the supply chains, product i made by process i.
inline calx::linalg::Walks paths(const ProductSystem& ps, const Method& m, const std::vector<double>& demand, std::size_t k, double cutoff = 1e-3,
                                 std::size_t depth = 10, std::size_t keep = 30) {
  ps.validate();
  detail::require(ps.square() && k < m.categories.size(), "paths: a square system and a category");
  const std::size_t n = ps.products.size();
  Matrix Z(n);
  std::vector<double> e(n, 0.0);
  for (std::size_t j = 0; j < n; j++) {
    detail::require(ps.A(j, j) > 0, "paths: each product made by its own process, on the diagonal of A");
    for (std::size_t i = 0; i < n; i++)
      if (i != j) Z(i, j) = -ps.A(i, j) / ps.A(j, j);
    for (std::size_t l = 0; l < ps.interventions.size(); l++) e[j] += m.C(k, l) * ps.B(l, j) / ps.A(j, j);
  }
  return calx::linalg::leading_walks(Z, e, demand, cutoff, depth, keep);
}

}  // namespace industrial_ecology::lca
