// Households: the unit within which money, time and care are pooled, and what their members' unpaid work is worth (ADR 0105).
//
// EQUIVALENCE SCALES. A household's income does not keep its members as well as the same income keeps one person: rooms, heating and
// meals are shared. An equivalence scale is the number of single adults a household counts as; equivalised income is income over it,
// given to each member (OECD 2013, Framework for Statistics on the Distribution of Household Income, Consumption and Wealth, ch. 8;
// Atkinson, Rainwater and Smeeding 1995). Two forms are declared, neither the domain's:
//
//   weights     first adult a₁, each further adult a₂, each child c (children below an age): the OECD-modified scale is 1, 0.5, 0.3 with
//               children under 14 (Hagenaars, de Vos and Zaidi 1994), the original OECD scale 1, 0.7, 0.5;
//   power       size^θ: θ = 0.5 is the square-root scale the OECD uses in its comparisons; θ = 0 treats a household's needs as one
//               person's (full economies of scale), θ = 1 is income per head (none).
//
// THE VALUE OF UNPAID WORK. Minutes of household work, cooking, cleaning, caring for children and adults, valued by the input method
// (UNECE 2017, Guide on Valuing Unpaid Household Service Work, ch. 4): hours times a wage, which a scheme declares:
//
//   replacement cost, generalist     every activity at a housekeeper's wage
//   replacement cost, specialist     each activity at the wage of the market worker who does it (a cook, a cleaner, a childcare worker)
//   opportunity cost                 at the wage the person would earn in the time
//
// a year's value being minutes a day × 365/60 × the wage, for each person. The three answer different questions and are reported apart.
//
// FULL INCOME (Becker 1965, Econ. J. 75: 493–517): money income plus the value of the time a household has, the budget of goods and of
// hours together; here the money income and the value of its unpaid work, each said.
//
// TIME POVERTY. Who lacks time for rest and for themselves after paid and unpaid work: a person is time-poor whose committed time (paid
// work, its travel, unpaid work and care) exceeds a declared line, by default 1.5 × the median committed time of the population, after
// Bardasi and Wodon (2010, Feminist Economics 16: 45–78); the line is declared, never presumed universal.
#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace social_provision::households {

struct Scale {
  std::string name = "OECD-modified";
  enum class Form { weights, power } form = Form::weights;
  double first_adult = 1, other_adult = 0.5, child = 0.3, child_below = 14;  // weights
  double theta = 0.5;                                                        // power
};

inline Scale oecd_modified() { return {}; }
inline Scale oecd_original() { return {"OECD (original)", Scale::Form::weights, 1, 0.7, 0.5, 14, 0.5}; }
inline Scale square_root() { return {"square root", Scale::Form::power, 1, 0.5, 0.3, 14, 0.5}; }

// The number of equivalent adults of a household whose members have these ages.
inline double equivalent_adults(const Scale& s, const std::vector<double>& ages) {
  if (ages.empty()) throw std::invalid_argument("social_provision::households: a household has at least one member");
  if (s.form == Scale::Form::power) return std::pow(static_cast<double>(ages.size()), s.theta);
  std::size_t adults = 0, children = 0;
  for (double a : ages) (a >= s.child_below ? adults : children) += 1;
  if (adults == 0) {  // a household of children only: the eldest counts as the first adult
    adults = 1, children -= 1;
  }
  return s.first_adult + s.other_adult * static_cast<double>(adults - 1) + s.child * static_cast<double>(children);
}

inline double equivalised(double income, const Scale& s, const std::vector<double>& ages) { return income / equivalent_adults(s, ages); }

// Unpaid work: minutes a day by activity, and wages by activity (specialist), one wage for all (generalist), or the person's own.
struct Valuation {
  double hours_a_year = 0;
  double generalist = 0, specialist = 0, opportunity = 0;  // a year's value by each method (in the wages' currency)
  std::map<std::string, double> by_activity;              // the specialist value of each activity
};

inline Valuation value_unpaid(const std::map<std::string, double>& minutes_a_day, const std::map<std::string, double>& specialist_wage,
                              double generalist_wage, double own_wage) {
  Valuation v;
  for (const auto& [activity, minutes] : minutes_a_day) {
    if (!(minutes >= 0)) throw std::invalid_argument("social_provision::households::value_unpaid: minutes are non-negative");
    const double hours = minutes * 365.0 / 60.0;
    v.hours_a_year += hours;
    v.generalist += hours * generalist_wage;
    v.opportunity += hours * own_wage;
    auto w = specialist_wage.find(activity);
    const double wage = w != specialist_wage.end() ? w->second : generalist_wage;  // an activity with no specialist wage at the generalist's
    v.by_activity[activity] = hours * wage;
    v.specialist += hours * wage;
  }
  return v;
}

struct FullIncome {
  double money = 0, unpaid = 0, full = 0, unpaid_share = 0;
};

inline FullIncome full_income(double money, double unpaid_value) {
  FullIncome f{money, unpaid_value, money + unpaid_value, 0};
  f.unpaid_share = f.full > 0 ? unpaid_value / f.full : 0;
  return f;
}

// Time poverty: committed minutes a day of each person, with weights; the line declared (a multiple of the weighted median, or minutes).
struct TimePoverty {
  double line = 0, median = 0;
  double rate = 0;   // the weighted share above the line
  double gap = 0;    // the mean excess over the line among the time-poor, minutes a day
};

inline double weighted_median(std::vector<std::pair<double, double>> vw) {
  std::sort(vw.begin(), vw.end());
  double total = 0;
  for (auto& p : vw) total += p.second;
  double acc = 0;
  for (auto& p : vw) {
    acc += p.second;
    if (acc >= total / 2) return p.first;
  }
  return vw.empty() ? NAN : vw.back().first;
}

inline TimePoverty time_poverty(const std::vector<double>& committed, const std::vector<double>& weight = {}, double multiple = 1.5, double line_minutes = NAN) {
  if (committed.empty() || (!weight.empty() && weight.size() != committed.size())) throw std::invalid_argument("social_provision::households::time_poverty: one weight a person, or none");
  std::vector<std::pair<double, double>> vw;
  for (std::size_t i = 0; i < committed.size(); i++) vw.emplace_back(committed[i], weight.empty() ? 1.0 : weight[i]);
  TimePoverty t;
  t.median = weighted_median(vw);
  t.line = std::isfinite(line_minutes) ? line_minutes : multiple * t.median;
  double total = 0, poor = 0, excess = 0;
  for (auto [c, w] : vw) {
    total += w;
    if (c > t.line) poor += w, excess += w * (c - t.line);
  }
  t.rate = total > 0 ? poor / total : 0;
  t.gap = poor > 0 ? excess / poor : 0;
  return t;
}

}  // namespace social_provision::households
