// Care: the support people need to live when they cannot do everything for themselves, who gives it, and how the need changes as a
// population ages (ADR 0105).
//
// NEED. People needing care at age a are the population there times the prevalence of dependency (a severe limitation of usual
// activities, say), n_a π_a; the hours of care they need are a declared number of hours a week for each, by level. Projected
// populations give the need ahead, with the prevalence held or changed by declared scenarios: compressed (prevalence falling with each
// year of life gained) or expanded (Fries 1980; Gruenberg 1977; Manton 1982, the three hypotheses of morbidity).
//
// SUPPLY AND THE GAP. The hours of care given: informally, by carers in households (from time use: minutes a day caring for adults,
// times the population that gives them) and formally, by a care workforce (its people times their hours); the gap is need less supply,
// in hours a week, and the share of need met. The care diamond (Razavi 2007, UNRISD Programme Paper 3) reads the supply by who gives
// it: households and families, the state, the market, and the community; its shares are reported, and none is presumed.
//
// SUPPORT RATIOS. How many might give care for each who might need it: the old-age dependency ratio (65 and over per hundred aged 15–64)
// and the support ratio of the oldest old (aged 50–64 per person aged 85 and over), which demographers use for the number of potential
// carers of the very old (United Nations, World Population Ageing).
//
// WHAT MADE THE NEED CHANGE. The change in the number needing care between two years, Σ n_a π_a, splits exactly into the population's
// size, its age structure and the prevalence by the Shapley value of the three factors (nomos's), each of the 3! orders of replacing
// them averaged: the decomposition of Das Gupta (1993, Standardization and Decomposition of Rates, ch. 3) made order-free, which leaves
// no residual and gives each factor the same share in either direction.
#pragma once

#include <nomos/games/claims.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace social_provision::care {

// The people needing care in each age group, and in all.
struct Need {
  std::vector<double> by_age;
  double people = 0, hours_a_week = 0;
};

inline Need need(const std::vector<double>& population, const std::vector<double>& prevalence, double hours_a_week_each) {
  if (population.size() != prevalence.size() || population.empty()) throw std::invalid_argument("social_provision::care::need: one prevalence an age group");
  Need n;
  n.by_age.resize(population.size());
  for (std::size_t a = 0; a < population.size(); a++) {
    if (!(population[a] >= 0) || !(prevalence[a] >= 0 && prevalence[a] <= 1)) throw std::invalid_argument("social_provision::care::need: populations ≥ 0, prevalences in [0, 1]");
    n.by_age[a] = population[a] * prevalence[a];
    n.people += n.by_age[a];
  }
  n.hours_a_week = n.people * hours_a_week_each;
  return n;
}

// A prevalence moved by a scenario: each age's prevalence scaled by (1 − c)^years, c a declared annual change (positive: compression).
inline std::vector<double> scenario(const std::vector<double>& prevalence, double annual_change, double years) {
  std::vector<double> p(prevalence);
  const double f = std::pow(1 - annual_change, years);
  for (double& x : p) x = std::clamp(x * f, 0.0, 1.0);
  return p;
}

struct Supply {
  double informal = 0, formal = 0, total = 0;  // hours a week
};

// Informal hours from time use (minutes a day giving care, by group, times the population of the group) and formal hours declared.
inline Supply supply(const std::vector<double>& carers, const std::vector<double>& minutes_a_day, double workforce, double hours_a_week_each) {
  if (carers.size() != minutes_a_day.size()) throw std::invalid_argument("social_provision::care::supply: minutes for each group of carers");
  Supply s;
  for (std::size_t g = 0; g < carers.size(); g++) s.informal += carers[g] * minutes_a_day[g] * 7.0 / 60.0;
  s.formal = workforce * hours_a_week_each;
  s.total = s.informal + s.formal;
  return s;
}

struct Gap {
  double need = 0, supply = 0, gap = 0, met = 0;  // hours a week; the share of need met (at most one)
};

inline Gap gap(double need_hours, double supply_hours) {
  Gap g{need_hours, supply_hours, need_hours - supply_hours, 0};
  g.met = need_hours > 0 ? std::min(1.0, supply_hours / need_hours) : 1.0;
  return g;
}

// The care diamond: hours by who gives them, as shares.
struct Diamond {
  double household = 0, state = 0, market = 0, community = 0, total = 0;
  double share(double x) const { return total > 0 ? x / total : 0; }
};

inline Diamond diamond(double household, double state, double market, double community) {
  for (double x : {household, state, market, community})
    if (!(x >= 0)) throw std::invalid_argument("social_provision::care::diamond: hours are non-negative");
  return {household, state, market, community, household + state + market + community};
}

// Support ratios from a population by single years of age (or groups starting at `ages`).
struct Ratios {
  double old_age_dependency = 0;  // 65+ per 100 aged 15–64
  double oldest_old_support = 0;  // aged 50–64 per person aged 85+
};

inline Ratios ratios(const std::vector<double>& ages, const std::vector<double>& population) {
  if (ages.size() != population.size()) throw std::invalid_argument("social_provision::care::ratios: one population an age");
  double working = 0, old = 0, mid = 0, oldest = 0;
  for (std::size_t i = 0; i < ages.size(); i++) {
    const double a = ages[i], n = population[i];
    if (a >= 15 && a < 65) working += n;
    if (a >= 65) old += n;
    if (a >= 50 && a < 65) mid += n;
    if (a >= 85) oldest += n;
  }
  return {working > 0 ? 100 * old / working : NAN, oldest > 0 ? mid / oldest : NAN};
}

// The change in people needing care, Σ N s_a π_a with N the size, s the age structure (shares), π the prevalence, split among the three.
struct Change {
  double before = 0, after = 0, change = 0;
  double size = 0, structure = 0, prevalence = 0;  // Shapley shares, summing to the change
};

inline Change decompose(const std::vector<double>& pop_before, const std::vector<double>& prev_before, const std::vector<double>& pop_after,
                        const std::vector<double>& prev_after) {
  const std::size_t k = pop_before.size();
  if (prev_before.size() != k || pop_after.size() != k || prev_after.size() != k) throw std::invalid_argument("social_provision::care::decompose: four vectors of one length");
  double N0 = 0, N1 = 0;
  for (std::size_t a = 0; a < k; a++) N0 += pop_before[a], N1 += pop_after[a];
  if (!(N0 > 0 && N1 > 0)) throw std::invalid_argument("social_provision::care::decompose: populations with people in them");
  const auto value = [&](bool size, bool structure, bool prevalence) {
    const double N = size ? N1 : N0;
    double v = 0;
    for (std::size_t a = 0; a < k; a++) {
      const double s = structure ? pop_after[a] / N1 : pop_before[a] / N0, p = prevalence ? prev_after[a] : prev_before[a];
      v += N * s * p;
    }
    return v;
  };
  Change c;
  c.before = value(false, false, false);
  c.after = value(true, true, true);
  c.change = c.after - c.before;
  const auto phi = nomos::games::shapley(3, [&](std::uint32_t S) { return value(S & 1u, S & 2u, S & 4u) - c.before; });
  c.size = phi[0], c.structure = phi[1], c.prevalence = phi[2];
  return c;
}

}  // namespace social_provision::care
