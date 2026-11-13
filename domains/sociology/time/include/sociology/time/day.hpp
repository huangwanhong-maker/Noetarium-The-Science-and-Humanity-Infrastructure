// sociology::time: a day as a composition of activities (ADR 0104).
//
// A day holds 1440 minutes. What people do with them is a composition: more of one activity is less of
// the others, so minutes are compared as ratios in Aitchison's geometry of the simplex (tyche's
// `compositions`), not as independent numbers. This header gives the day's sociological objects:
//
//   - a declared classification of activity codes into the parts of a day (HETUS's codes, a survey's
//     own, or Ås's four kinds of time), and the minutes it leaves unclassified;
//   - a day closed to 1440 minutes, with the zeros of a diary replaced at a declared resolution;
//   - a group's day: its geometric centre (and the arithmetic mean surveys publish), its spread;
//   - the contrast between two days: minutes, ratios, the clr difference and the Aitchison distance;
//   - who carries a part of the day: the shares of its minutes by group against their shares of people;
//   - the balance between two kinds of time (unpaid against paid work, say);
//   - places compared by their days: Aitchison distances and their principal coordinates.
//
// Sources: Aitchison, J. (1986), The Statistical Analysis of Compositional Data, Chapman and Hall;
// Gershuny, J. (2000), Changing Times: Work and Leisure in Postindustrial Society, Oxford University
// Press; Ås, D. (1978), Studies of time-use: problems and prospects, Acta Sociologica 21, 125–141;
// Dumuid, D. et al. (2018), Compositional data analysis for physical activity, sedentary time and sleep
// research, Statistical Methods in Medical Research 27, 3726–3738; Eurostat (2019), Harmonised European
// Time Use Surveys (HETUS) 2018 Guidelines, §5 (the diary's ten-minute slots).
//
// Accuracy: exact arithmetic on the given minutes. The centre is a statistic of the sample; its sampling
// error is not reported here (tyche's describe gives the variation). Zeros replaced at `resolution` are a
// declared presumption: a part a diary did not record is taken to hold 0.65 of a slot (Martín-Fernández et
// al. 2003), never left as an essential zero.

#pragma once

#include <tyche/compositions/balances.hpp>
#include <tyche/compositions/sample.hpp>
#include <tyche/compositions/simplex.hpp>
#include <tyche/compositions/zeros.hpp>
#include <tyche/multivariate/pcoa.hpp>

#include <calx/linalg/dense.hpp>
#include <calx/linalg/rectangular.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace sociology::time {

using calx::linalg::Matrix;
using calx::linalg::Rectangular;

inline constexpr double MINUTES_A_DAY = 1440;

inline void require(bool ok, const char* where, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("sociology::time::") + where + ": " + what);
}

// ---------------------------------------------------------------- a declared classification

// The parts of a day, each a set of activity codes. A code belongs to at most one part.
struct Classification {
  std::vector<std::string> parts;            // the parts' names, in order
  std::map<std::string, std::size_t> part;   // activity code -> its part
};

inline Classification classification(const std::vector<std::pair<std::string, std::vector<std::string>>>& parts) {
  require(parts.size() >= 2, "classification", "at least two parts");
  Classification c;
  for (std::size_t k = 0; k < parts.size(); k++) {
    require(!parts[k].first.empty(), "classification", "every part is named");
    c.parts.push_back(parts[k].first);
    for (const auto& code : parts[k].second) {
      require(!c.part.contains(code), "classification", "a code belongs to one part only");
      c.part[code] = k;
    }
  }
  return c;
}

struct Classified {
  std::vector<double> minutes;      // per part
  double unclassified = 0;          // minutes of codes no part names
  std::vector<std::string> left;    // those codes, in the order met
};

// The minutes of the given activities gathered into the classification's parts.
inline Classified classify(const Classification& c, std::span<const std::string> codes, std::span<const double> minutes) {
  require(codes.size() == minutes.size(), "classify", "one number of minutes per code");
  Classified r;
  r.minutes.assign(c.parts.size(), 0.0);
  for (std::size_t i = 0; i < codes.size(); i++) {
    require(std::isfinite(minutes[i]) && minutes[i] >= 0, "classify", "minutes are finite and not negative");
    const auto it = c.part.find(codes[i]);
    if (it != c.part.end()) {
      r.minutes[it->second] += minutes[i];
    } else {
      r.unclassified += minutes[i];
      r.left.push_back(codes[i]);
    }
  }
  return r;
}

// ---------------------------------------------------------------- a day closed to 1440 minutes

struct DayOptions {
  double total = MINUTES_A_DAY;   // the minutes a day closes to
  double resolution = 10;         // the diary's slot, in minutes (HETUS: ten); a zero holds 0.65 of it
  double ddof = 1;                // the divisor of variances, n − ddof
};

// A day closed to the total: unrecorded time (a day summing to less, or more) is spread in proportion to
// what was recorded, which is the closure. Zeros are replaced at the declared resolution.
inline std::vector<double> close_day(std::span<const double> minutes, const DayOptions& o = {}) {
  require(minutes.size() >= 2, "close_day", "at least two parts");
  double sum = 0;
  for (double m : minutes) {
    require(std::isfinite(m) && m >= 0, "close_day", "minutes are finite and not negative");
    sum += m;
  }
  require(sum > 0, "close_day", "some time is recorded");
  std::vector<double> scaled(minutes.begin(), minutes.end());
  for (double& m : scaled) m *= o.total / sum;
  if (!tyche::compositions::has_zeros(scaled)) return tyche::compositions::closure(scaled, o.total);
  tyche::compositions::ZeroOptions z;
  z.detection_limits.assign(scaled.size(), o.resolution);
  z.total = o.total;
  return tyche::compositions::replace_zeros(scaled, z);
}

// Every row of a table of days closed and its zeros replaced.
inline Rectangular close_days(const Rectangular& days, const DayOptions& o = {}) {
  Rectangular out(days.rows, days.cols);
  for (std::size_t i = 0; i < days.rows; i++) {
    std::vector<double> row(days.cols);
    for (std::size_t k = 0; k < days.cols; k++) row[k] = days(i, k);
    const auto c = close_day(row, o);
    for (std::size_t k = 0; k < days.cols; k++) out(i, k) = c[k];
  }
  return out;
}

// ---------------------------------------------------------------- a group's day

struct GroupDay {
  std::size_t people = 0, parts = 0;
  std::vector<double> centre;          // the geometric centre, closed to the total (Aitchison's)
  std::vector<double> mean;            // the arithmetic mean of the closed days (what surveys publish)
  double total_variance = 0;           // Aitchison's total variance, with divisor n − ddof
  double metric_sd = 0;                // its square root over the parts' dimension
  std::vector<double> clr_variance;    // per part: how much it varies relative to the day
  Matrix variation;                    // τ_ij = var ln(x_i/x_j)
};

// The day of a group of people (rows, minutes by part).
inline GroupDay group_day(const Rectangular& days, const DayOptions& o = {}) {
  require(days.rows >= 1 && days.cols >= 2, "group_day", "at least one day of two parts");
  const Rectangular X = close_days(days, o);
  GroupDay g;
  g.people = X.rows;
  g.parts = X.cols;
  g.centre = tyche::compositions::centre(X, o.total);
  g.mean.assign(X.cols, 0.0);
  for (std::size_t i = 0; i < X.rows; i++)
    for (std::size_t k = 0; k < X.cols; k++) g.mean[k] += X(i, k) / static_cast<double>(X.rows);
  if (X.rows > o.ddof) {
    const auto s = tyche::compositions::describe(X, o.ddof, o.total);
    g.total_variance = s.total_variance;
    g.metric_sd = s.metric_standard_deviation;
    g.clr_variance = s.clr_variances;
    g.variation = s.variation;
  } else {
    g.clr_variance.assign(X.cols, 0.0);
    g.variation = Matrix(X.cols);
  }
  return g;
}

// ---------------------------------------------------------------- two days contrasted

struct Contrast {
  std::vector<double> minutes;      // x_i − y_i, both closed to the total
  std::vector<double> ratio;        // x_i / y_i
  std::vector<double> clr;          // clr(x ⊖ y)_i = ln(x_i/y_i) − mean_j ln(x_j/y_j)
  std::vector<double> perturbation; // x ⊖ y, closed to 1: the neutral day is (1/D, …, 1/D)
  double distance = 0;              // Aitchison's distance between x and y
};

// How one day differs from another, in minutes and in the simplex's own geometry.
inline Contrast contrast(std::span<const double> x, std::span<const double> y, const DayOptions& o = {}) {
  require(x.size() == y.size(), "contrast", "two days of the same parts");
  const auto a = close_day(x, o), b = close_day(y, o);
  Contrast c;
  for (std::size_t k = 0; k < a.size(); k++) {
    c.minutes.push_back(a[k] - b[k]);
    c.ratio.push_back(a[k] / b[k]);
  }
  const auto za = tyche::compositions::clr(a), zb = tyche::compositions::clr(b);
  for (std::size_t k = 0; k < a.size(); k++) c.clr.push_back(za[k] - zb[k]);
  c.perturbation = tyche::compositions::difference(a, b, 1.0);
  c.distance = tyche::compositions::distance(a, b);
  return c;
}

// ---------------------------------------------------------------- who carries a part of the day

struct Carried {
  std::vector<double> minutes;            // each group's mean minutes a day in the part
  std::vector<double> people;             // each group's people (any unit: persons, thousands, weights)
  std::vector<double> share;              // the group's share of all the minutes given to the part, Σ = 1
  std::vector<double> people_share;       // the group's share of the people
  std::vector<double> over;               // share / people_share: above 1, more than its number
  double minutes_a_year_total = 0;        // Σ_g n_g m_g · 365 / 60, in hours a year (in the people's unit)
};

// Who carries a part: the minutes each group gives to it, weighed by how many they are.
inline Carried who_carries(std::span<const double> minutes, std::span<const double> people) {
  require(!minutes.empty() && minutes.size() == people.size(), "who_carries", "one number of minutes per group");
  Carried c;
  c.minutes.assign(minutes.begin(), minutes.end());
  c.people.assign(people.begin(), people.end());
  double all = 0, n = 0;
  for (std::size_t g = 0; g < minutes.size(); g++) {
    require(std::isfinite(minutes[g]) && minutes[g] >= 0, "who_carries", "minutes are finite and not negative");
    require(std::isfinite(people[g]) && people[g] > 0, "who_carries", "every group has people");
    all += minutes[g] * people[g];
    n += people[g];
  }
  require(all > 0, "who_carries", "someone gives time to the part");
  for (std::size_t g = 0; g < minutes.size(); g++) {
    c.share.push_back(minutes[g] * people[g] / all);
    c.people_share.push_back(people[g] / n);
    c.over.push_back(c.share.back() / c.people_share.back());
  }
  c.minutes_a_year_total = all * 365.0 / 60.0;
  return c;
}

// ---------------------------------------------------------------- the balance of two kinds of time

// The balance of the parts `plus` against the parts `minus`: √(rs/(r+s)) ln(g(x₊)/g(x₋)), an
// orthonormal coordinate (Egozcue and Pawlowsky-Glahn 2005). Zero: the two kinds of time are held in
// the same geometric proportion; each unit is a factor e^{√((r+s)/rs)} between them.
inline double balance(std::span<const double> day, std::span<const std::size_t> plus, std::span<const std::size_t> minus,
                      const DayOptions& o = {}) {
  return tyche::compositions::balance(close_day(day, o), plus, minus);
}

// ---------------------------------------------------------------- places compared by their days

struct Places {
  Matrix distance;                              // Aitchison's distances between the places' days
  tyche::multivariate::PCoA ordination;         // their principal coordinates (Euclidean: no negative eigenvalues)
  std::vector<double> centre;                   // the centre of the places' days, closed to the total
  double total_variance = 0;                    // Aitchison's total variance among them (divisor n − ddof)
  std::vector<double> clr_variance;             // which parts set the places apart
};

// Places (rows) compared by their days.
inline Places compare_places(const Rectangular& days, const DayOptions& o = {}) {
  require(days.rows >= 2 && days.cols >= 2, "compare_places", "at least two places' days of two parts");
  const Rectangular X = close_days(days, o);
  Places p;
  p.distance = Matrix(X.rows);
  std::vector<std::vector<double>> rows(X.rows, std::vector<double>(X.cols));
  for (std::size_t i = 0; i < X.rows; i++)
    for (std::size_t k = 0; k < X.cols; k++) rows[i][k] = X(i, k);
  for (std::size_t i = 0; i < X.rows; i++)
    for (std::size_t j = i + 1; j < X.rows; j++) {
      const double d = tyche::compositions::distance(rows[i], rows[j]);
      p.distance(i, j) = d;
      p.distance(j, i) = d;
    }
  p.ordination = tyche::multivariate::pcoa(p.distance);
  const auto s = tyche::compositions::describe(X, o.ddof, o.total);
  p.centre = s.centre;
  p.total_variance = s.total_variance;
  p.clr_variance = s.clr_variances;
  return p;
}

}  // namespace sociology::time
