// A region's own input–output table from its country's, by location quotients (theory note `theory/regional-tables.md`; ADR 0159).
//
// A region buys some of what its industries use from itself, the rest from the rest of its country or abroad. With no survey of its
// purchases, the national coefficients are scaled down by a location quotient: a_ij^r = a_ij · min(1, LQ_ij). The simple quotient
// SLQ_i = (E_i^r / E^r) / (E_i / E) says how far the region holds its share of the country's industry i; the cross-industry quotient
// CILQ_ij = SLQ_i / SLQ_j compares supplier and buyer; Flegg's FLQ_ij = CILQ_ij · λ* with λ* = [log2(1 + E^r / E)]^δ (SLQ_i · λ* on
// the diagonal) shrinks them further for a small region; the augmented AFLQ multiplies by log2(1 + SLQ_j) where the buyer is
// specialised (SLQ_j > 1). Flegg, Webber and Elliott (1995), Flegg and Webber (2000).
//
// The region's output is its share of the country's employment in each industry times the country's output (productivity taken as the
// country's). What the scaled coefficients leave of the national ones is bought from the rest of the country; imports from abroad, taxes
// and value added keep their national coefficients. Each column then sums as the country's does, so that the accounts' identity holds
// for the region with purchases from the rest of the country as a fourth leak. Final use by households is supplied in the region at the
// national domestic share times min(1, SLQ_i), the rest of the domestic share from the rest of the country (a declared rule).

#pragma once

#include <economics/production/input_output.hpp>

#include <calx/linalg/dense.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::regional {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("economics::regional: " + what);
}
}  // namespace detail

enum class Quotient { slq, cilq, flq, aflq };

struct Region {
  std::vector<double> share;   // by product: the region's share of the country's employment in the product's industry, E_i^r / E_i
  double size = 0;             // the region's share of the country's employment, E^r / E
};

struct Regionalised {
  InputOutputTable local;              // flows within the region; final demand the rest of each row; value added the rest of each column
  std::vector<double> output;          // x^r
  std::vector<double> rest;            // each column's purchases from the rest of the country
  std::vector<double> slq;             // SLQ by product
  double lambda = 1;                   // λ* (1 for SLQ and CILQ)
  calx::linalg::Matrix quotient;       // min(1, LQ_ij), the share of each national coefficient kept in the region
};

inline Regionalised regionalise(const InputOutputTable& T, const Region& R, Quotient q = Quotient::flq, double delta = 0.3) {
  const std::size_t n = T.n();
  detail::require(R.share.size() == n, "one employment share per product");
  detail::require(R.size > 0 && R.size < 1, "a region holds part of its country's employment");
  detail::require(delta >= 0 && delta < 1, "δ lies in [0, 1)");
  const auto x = T.output();
  Regionalised out;
  out.output.resize(n), out.slq.resize(n), out.rest.assign(n, 0.0);
  for (std::size_t i = 0; i < n; i++) {
    detail::require(R.share[i] >= 0 && R.share[i] <= 1, "an employment share lies in [0, 1]");
    out.output[i] = x[i] * R.share[i];
    out.slq[i] = R.share[i] / R.size;
  }
  out.lambda = (q == Quotient::flq || q == Quotient::aflq) ? std::pow(std::log2(1 + R.size), delta) : 1.0;
  out.quotient = calx::linalg::Matrix(n);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) {
      double lq = 1;
      switch (q) {
        case Quotient::slq: lq = out.slq[i]; break;
        case Quotient::cilq: lq = i == j ? out.slq[i] : (out.slq[j] > 0 ? out.slq[i] / out.slq[j] : 1.0); break;
        case Quotient::flq:
        case Quotient::aflq:
          lq = (i == j ? out.slq[i] : (out.slq[j] > 0 ? out.slq[i] / out.slq[j] : 1.0)) * out.lambda;
          if (q == Quotient::aflq && out.slq[j] > 1) lq *= std::log2(1 + out.slq[j]);
          break;
      }
      out.quotient(i, j) = std::clamp(lq, 0.0, 1.0);
    }
  InputOutputTable& L = out.local;
  L.sectors = T.sectors, L.z = calx::linalg::Matrix(n), L.final_demand.assign(n, 0.0), L.value_added.assign(n, 0.0);
  for (std::size_t j = 0; j < n; j++) {
    if (!(x[j] > 0)) continue;
    double kept = 0, all = 0;
    for (std::size_t i = 0; i < n; i++) {
      const double a = T.z(i, j) / x[j];
      L.z(i, j) = a * out.quotient(i, j) * out.output[j];
      kept += L.z(i, j), all += a * out.output[j];
    }
    out.rest[j] = all - kept;
    L.value_added[j] = out.output[j] - kept;
  }
  for (std::size_t i = 0; i < n; i++) {
    double used = 0;
    for (std::size_t j = 0; j < n; j++) used += L.z(i, j);
    L.final_demand[i] = out.output[i] - used;
  }
  return out;
}

// Households' use supplied in the region: the national domestic share times min(1, SLQ_i).
inline std::vector<double> local_share(const std::vector<double>& national_share, const std::vector<double>& slq) {
  detail::require(national_share.size() == slq.size(), "one share per product");
  std::vector<double> s(slq.size());
  for (std::size_t i = 0; i < slq.size(); i++) s[i] = national_share[i] * std::min(1.0, slq[i]);
  return s;
}

}  // namespace economics::regional
