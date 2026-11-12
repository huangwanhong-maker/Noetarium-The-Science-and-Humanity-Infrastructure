// The forest's future: the trees standing, growing and removed year by year, and those planted, with the expectation and
// the spread of what they hold computed exactly; the planting that holds a stock; and the stage model a forest's rates imply.
//
// Each tree i of the inventory grows by its genus's increment Δ_i a year (inventory.hpp), so that D_i(t) = D_i + Δ_i t, and
// is removed at a time L_i that is independent of the others:
//
//   L_i uniform on [a_i, b_i]   where an arborist stated its remaining useful life as that interval (a point when a = b),
//   L_i = a_i + geometric(h)     where the class is open ("more than a_i years"): certain to a_i, then the hazard,
//   L_i geometric of hazard h    otherwise (a presumed annual probability of removal, stated by whoever runs it).
//
// A tree planted in year τ starts at a stated diameter and is removed by the hazard of the planted, h_p. Every standing
// tree is a Bernoulli variable with P(standing at t) = p_i(t):
//
//   p_i(t) = clamp((b_i − t)/(b_i − a_i), 0, 1)  (= 1 for t < a, 0 for t ≥ b when a = b),
//   p_i(t) = 1 for t < a_i, (1 − h)^{t − a_i} after,     p_i(t) = (1 − h)^t,
//
// so the number standing N(t) = Σ_i 1{L_i > t} and the carbon C(t) = Σ_i c_i(t) 1{L_i > t} have the exact moments
//
//   E N(t) = Σ_i p_i(t),  Var N(t) = Σ_i p_i(1 − p_i),   E C(t) = Σ_i c_i(t) p_i(t),  Var C(t) = Σ_i c_i(t)² p_i(1 − p_i),
//
// with c_i(t) = C(D_i(t)) by the scheme's allometry (allometry.hpp). The band reported is E ± 1.96 sd: the removals'
// chance alone, given the presumptions. No Monte Carlo is needed, so the answer is the same at every run and immediate for
// a city's trees over decades. Planting enters linearly: P trees a year add P Σ_τ c_p(t − τ)(1 − h_p)^{t−τ}, so the
// planting that brings the expected stock at a horizon back to today's is a quotient, solved exactly.
//
// The stage view (Usher 1966, J. Appl. Ecol. 3: 355–367; 1969, Biometrics 25: 309–315): trees in diameter classes;
// a class's annual survival σ_j = 1 − 1/L̄_j, the constant hazard with the mean expected remaining life L̄_j of its trees (a
// stated life's first year alone would say nearly every tree survives, which is true of a year and false of a life), its
// growth γ_j = mean Δ / class width (the fraction of a
// class, spread evenly across it, that crosses its upper bound in a year; capped at one), no recruitment but the planting
// into the first class. This is a population model (populations/stages.hpp) whose equilibrium (I − A)⁻¹ b is the forest
// a constant planting sustains, and whose life expectancies are a planted tree's expected years standing.
//
// Caveat. Every rate here is presumed: the increments from one inventory, the remaining useful life as one assessment,
// hazards as declared. The exactness is of the consequences of those presumptions, not of the future.
#pragma once

#include <ecology/forest/allometry.hpp>
#include <ecology/forest/inventory.hpp>
#include <ecology/populations/stages.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::forest {

struct Planting {
  double per_year = 0;                           // trees planted each year (a mean; fractions are expectations)
  std::vector<std::pair<Taxon, double>> mix;     // taxa and their shares (normalised)
  double dbh = 5;                                // cm at planting
  double hazard = 0.03;                          // annual probability of removal of a planted tree (presumed)
};

struct Removal {
  double hazard = 0.03;   // annual probability of removal of a tree whose remaining life is not stated (presumed)
  bool use_stated_life = true;
};

struct FutureYear {
  int t = 0;                                        // years from now
  double standing = 0, standing_sd = 0;             // E N(t), sd
  double carbon = 0, carbon_sd = 0;                 // E C(t), sd (kg)
  double planted_standing = 0, planted_carbon = 0;  // of which, from planting
  double removed = 0;                               // expected removals in the year ending at t
  double largest_share[3] = {0, 0, 0};              // the largest expected share at species, genus, family
  std::string largest[3];
};

// The expected remaining life of a tree, years: the middle of a stated interval; its lower end plus 1/h for an open class; 1/h
// where none is stated.
inline double expected_life(const Tree& tr, const Removal& rm) {
  const double inv = rm.hazard > 0 ? 1 / rm.hazard : INFINITY;
  if (rm.use_stated_life && std::isfinite(tr.life_lower)) {
    const double a = std::max(0.0, tr.life_lower);
    if (!std::isfinite(tr.life_upper)) return a + inv;
    return (a + std::max(a, tr.life_upper)) / 2;
  }
  return inv;
}

inline double standing_probability(const Tree& tr, const Removal& rm, double t) {
  if (t <= 0) return 1;
  if (rm.use_stated_life && std::isfinite(tr.life_lower)) {
    const double a = std::max(0.0, tr.life_lower);
    if (!std::isfinite(tr.life_upper)) return t < a ? 1.0 : std::pow(1 - rm.hazard, t - a);  // an open class: "more than a years"
    const double b = std::max(a, tr.life_upper);
    if (b == a) return t < a ? 1.0 : 0.0;
    return std::clamp((b - t) / (b - a), 0.0, 1.0);
  }
  return std::pow(1 - rm.hazard, t);
}

namespace detail {

inline double increment_of(const Scheme& s, const Increments& inc, const Taxon& t) {
  const auto a = assign(s, t);
  const double g = s.groups[a.group].increment;
  if (std::isfinite(g)) return g;
  const double x = inc.of(t);
  return std::isfinite(x) ? x : 0.0;
}

inline double carbon_at(const Scheme& s, const Taxon& t, double d) {
  const double c = estimate(s, t, std::max(d, 0.1)).carbon;
  return std::isfinite(c) ? c : 0.0;
}

inline void normalise(std::vector<std::pair<Taxon, double>>& mix) {
  double total = 0;
  for (auto& [t, w] : mix) total += w;
  if (!(total > 0)) throw std::invalid_argument("ecology::forest: a planting mix with positive shares");
  for (auto& [t, w] : mix) w /= total;
}

}  // namespace detail

// The forest's future, year by year to `years`, for the trees of `area` (all when area < 0).
inline std::vector<FutureYear> future(const std::vector<Tree>& trees, const Scheme& scheme, const Increments& inc, Planting plant,
                                      const Removal& rm, int years, int area = -1) {
  if (years < 0) throw std::invalid_argument("ecology::forest::future: years ≥ 0");
  if (plant.per_year > 0) detail::normalise(plant.mix);
  std::vector<FutureYear> out(static_cast<std::size_t>(years) + 1);
  std::vector<std::map<std::string, double>> names(3 * out.size());  // expected counts by name, rank by rank
  const auto add_names = [&](const Taxon& t, int y, double p) {
    for (int r = 0; r < 3; r++) {
      const std::string& n = t.at(static_cast<Rank>(r));
      if (!n.empty()) names[static_cast<std::size_t>(3 * y + r)][n] += p;
    }
  };
  for (const auto& tr : trees) {
    if (area >= 0 && tr.area != area) continue;
    if (!(tr.dbh > 0)) continue;
    const double d = detail::increment_of(scheme, inc, tr.taxon);
    for (int y = 0; y <= years; y++) {
      const double p = standing_probability(tr, rm, y), c = detail::carbon_at(scheme, tr.taxon, tr.dbh + d * y);
      auto& f = out[static_cast<std::size_t>(y)];
      f.standing += p;
      f.standing_sd += p * (1 - p);
      f.carbon += c * p;
      f.carbon_sd += c * c * p * (1 - p);
      if (y > 0) f.removed += standing_probability(tr, rm, y - 1) - p;
      add_names(tr.taxon, y, p);
    }
  }
  if (plant.per_year > 0) {
    for (const auto& [taxon, share] : plant.mix) {
      const double d = detail::increment_of(scheme, inc, taxon), n = plant.per_year * share;
      // A cohort planted in year τ = 1..years, of age a = y − τ at year y; each tree standing with (1 − h_p)^a.
      for (int y = 1; y <= years; y++) {
        auto& f = out[static_cast<std::size_t>(y)];
        for (int tau = 1; tau <= y; tau++) {
          const int age = y - tau;
          const double p = std::pow(1 - plant.hazard, age), c = detail::carbon_at(scheme, taxon, plant.dbh + d * age);
          f.standing += n * p;
          f.standing_sd += n * p * (1 - p);
          f.carbon += n * c * p;
          f.carbon_sd += n * c * c * p * (1 - p);
          f.planted_standing += n * p;
          f.planted_carbon += n * c * p;
          if (age > 0) f.removed += n * (std::pow(1 - plant.hazard, age - 1) - p);
          add_names(taxon, y, n * p);
        }
      }
    }
  }
  for (std::size_t y = 0; y < out.size(); y++) {
    auto& f = out[y];
    f.t = static_cast<int>(y);
    f.standing_sd = std::sqrt(f.standing_sd);
    f.carbon_sd = std::sqrt(f.carbon_sd);
    for (int r = 0; r < 3; r++) {
      const auto& m = names[3 * y + static_cast<std::size_t>(r)];
      double total = 0, best = 0;
      std::string arg;
      for (const auto& [n, c] : m) {
        total += c;
        if (c > best) best = c, arg = n;
      }
      f.largest_share[r] = total > 0 ? best / total : 0;
      f.largest[r] = arg;
    }
  }
  return out;
}

// The trees a year to plant (with the plan's mix, diameter and hazard) so that the expected carbon at `years` equals today's.
// Negative when the forest gains without planting (then no planting is needed to hold it).
inline double planting_to_hold_carbon(const std::vector<Tree>& trees, const Scheme& scheme, const Increments& inc, Planting plant,
                                      const Removal& rm, int years, int area = -1) {
  if (years < 1) throw std::invalid_argument("ecology::forest::planting_to_hold_carbon: a horizon of a year or more");
  plant.per_year = 0;
  const auto without = future(trees, scheme, inc, plant, rm, years, area);
  plant.per_year = 1;
  const auto unit = future(trees, scheme, inc, plant, rm, years, area);
  const double per_tree = unit.back().planted_carbon;
  if (!(per_tree > 0)) throw std::invalid_argument("ecology::forest::planting_to_hold_carbon: the planted trees hold no carbon at the horizon");
  return (without.front().carbon - without.back().carbon) / per_tree;
}

// The stage model the forest's rates imply, in diameter classes [b_0, b_1), …, [b_{k−1}, ∞): σ_j the mean probability of
// 1 − 1/(mean expected remaining life), γ_j the mean increment over the class width (capped at one), no recruitment.
inline populations::StageModel stage_model(const std::vector<Tree>& trees, const Scheme& scheme, const Increments& inc,
                                           const Removal& rm, const std::vector<double>& bounds, int area = -1) {
  const std::size_t k = bounds.size();
  if (k < 1) throw std::invalid_argument("ecology::forest::stage_model: at least one class");
  std::vector<double> n(k, 0), surv(k, 0), grow(k, 0);
  for (const auto& tr : trees) {
    if ((area >= 0 && tr.area != area) || !(tr.dbh >= bounds[0])) continue;
    std::size_t j = 0;
    while (j + 1 < k && tr.dbh >= bounds[j + 1]) j++;
    n[j] += 1;
    surv[j] += expected_life(tr, rm);
    grow[j] += detail::increment_of(scheme, inc, tr.taxon);
  }
  populations::StageModel m;
  for (std::size_t j = 0; j < k; j++) {
    m.stages.push_back(j + 1 < k ? std::to_string(static_cast<int>(bounds[j])) + "–" + std::to_string(static_cast<int>(bounds[j + 1])) + " cm"
                                 : "≥ " + std::to_string(static_cast<int>(bounds[j])) + " cm");
    const double life = n[j] > 0 ? surv[j] / n[j] : (rm.hazard > 0 ? 1 / rm.hazard : INFINITY);
    m.survival.push_back(std::isfinite(life) && life > 1 ? 1 - 1 / life : (std::isfinite(life) ? 0.0 : 1.0));
    const double width = j + 1 < k ? bounds[j + 1] - bounds[j] : 0;
    m.growth.push_back(j + 1 < k && n[j] > 0 && width > 0 ? std::min(1.0, grow[j] / n[j] / width) : 0.0);
    m.fecundity.push_back(0);
  }
  return m;
}

}  // namespace ecology::forest
