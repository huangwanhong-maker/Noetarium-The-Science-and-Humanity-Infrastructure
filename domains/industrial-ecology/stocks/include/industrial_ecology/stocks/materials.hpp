// The materials a stock holds and moves: each cohort's units carry a mass of each material per unit,
// known within bounds (theory note §2.4).
//
// For material k with intensity m_k(c) per unit of cohort c, and m_k(a) per unit of the initial stock's
// age a, the material stock and flows are
//
//   M_k(t) = Σ_c s(t, c) m_k(c) + Σ_a s₀(t, a) m_k(a),   and likewise for the inflow and the outflow.
//
// Every s(t, c), I(c), o(t, c) and s₀(t, a) of a model whose inflows are non-negative is non-negative,
// so the material sums are increasing in each intensity: the bounds of the intensities carry to the
// bounds of the stock and flows exactly, term by term, and the central values to the central. A model
// with a negative inflow (stock_driven told to keep it) is refused, since the bounds would not hold.

#pragma once

#include <industrial_ecology/stocks/dynamic.hpp>

#include <cstddef>
#include <string>
#include <vector>

namespace industrial_ecology::stocks {

// Mass per unit, by material, with bounds: rows [cohort][material] (one row for every cohort, or a
// single row for all), and for an initial stock rows [age][material] (one per age, or a single row).
struct Intensity {
  std::vector<std::string> materials;
  std::vector<std::vector<double>> lower, central, upper;
  std::vector<std::vector<double>> initial_lower, initial_central, initial_upper;
};

struct Bounded {
  std::vector<double> lower, central, upper;
};
struct MaterialFlows {
  std::string material;
  Bounded stock, inflow, outflow;
};

namespace detail {
inline void check_rows(const std::vector<std::vector<double>>& lo, const std::vector<std::vector<double>>& mid, const std::vector<std::vector<double>>& hi,
                       std::size_t K, std::size_t count, const char* what) {
  require(lo.size() == mid.size() && mid.size() == hi.size() && (mid.size() == 1 || mid.size() == count), what);
  for (std::size_t c = 0; c < mid.size(); c++) {
    require(lo[c].size() == K && mid[c].size() == K && hi[c].size() == K, "one intensity per material");
    for (std::size_t k = 0; k < K; k++) require(0 <= lo[c][k] && lo[c][k] <= mid[c][k] && mid[c][k] <= hi[c][k], "0 ≤ lower ≤ central ≤ upper");
  }
}
}  // namespace detail

inline std::vector<MaterialFlows> materials(const Model& M, const Intensity& in) {
  const std::size_t T = M.inflow.size(), K = in.materials.size(), A = M.initial_start.size();
  for (double v : M.inflow) detail::require(v >= 0, "material bounds need a model with no negative inflow");
  detail::check_rows(in.lower, in.central, in.upper, K, T, "intensities for one cohort, or for every cohort");
  const bool has_initial = M.initial_total > 0;
  if (has_initial) detail::check_rows(in.initial_lower, in.initial_central, in.initial_upper, K, A, "intensities for the initial stock: one row, or one per age");
  auto row = [](const std::vector<std::vector<double>>& m, std::size_t c) -> const std::vector<double>& { return m.size() == 1 ? m[0] : m[c]; };
  const std::vector<std::vector<double>>* cohort[3] = {&in.lower, &in.central, &in.upper};
  const std::vector<std::vector<double>>* initial[3] = {&in.initial_lower, &in.initial_central, &in.initial_upper};
  std::vector<MaterialFlows> out(K);
  for (std::size_t k = 0; k < K; k++) {
    out[k].material = in.materials[k];
    Bounded* parts[3] = {&out[k].stock, &out[k].inflow, &out[k].outflow};
    for (Bounded* b : parts) b->lower.assign(T, 0.0), b->central.assign(T, 0.0), b->upper.assign(T, 0.0);
    for (std::size_t t = 0; t < T; t++)
      for (int j = 0; j < 3; j++) {
        double s = 0, i = M.inflow[t] * row(*cohort[j], t)[k], o = 0;
        for (std::size_t c = 0; c <= t; c++) {
          s += M.by_cohort[t][c] * row(*cohort[j], c)[k];
          o += M.outflow_by_cohort[t][c] * row(*cohort[j], c)[k];
        }
        if (has_initial)
          for (std::size_t a = 0; a < A; a++) {
            const double mk = row(*initial[j], a)[k];
            const double now = M.initial_by_age[t][a], before = t == 0 ? M.initial_start[a] : M.initial_by_age[t - 1][a];
            s += now * mk;
            o += (before - now) * mk;
          }
        const double v[3] = {s, i, o};
        for (int q = 0; q < 3; q++) (j == 0 ? parts[q]->lower : j == 1 ? parts[q]->central : parts[q]->upper)[t] = v[q];
      }
  }
  return out;
}

}  // namespace industrial_ecology::stocks
