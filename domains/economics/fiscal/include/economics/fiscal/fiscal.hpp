// The finances of a local or regional government: its accounts read through a declared chart, its ratios against declared
// thresholds, its years ahead with the loans it carries, the largest loan it can service, the dynamics of its debt, and its
// fiscal capacity and effort (ADR 0145).
//
// Accounts. A chart maps each line of a government's accounts to a declared category (own-source revenue, transfers received,
// operating expenditure, interest, principal repaid, capital expenditure, or whatever a codebook names); the amounts of a year are
// summed by category, and a line the chart does not map is refused, never guessed. Ratios are declared as signed sums of
// categories over signed sums (an operating surplus over operating revenue; debt service over own-source revenue), and a scheme
// of thresholds states, for each ratio, the side that is sound and the levels of warning and of intervention: nothing in the code
// is any jurisdiction's limit.
//
// The years ahead. Each category grows at a declared rate; loans are annuities (equal payments of P r / (1 − (1 + r)^−n) over n
// years) or bullets, their interest and principal scheduled year by year; the operating surplus is a declared signed sum. The
// debt service coverage ratio of a year is the operating surplus over the debt service due.
//
// Debt capacity. The largest new annuity a government can take, at rate r over n years from a year s, keeping the coverage at
// or above a floor θ in every year it is repaid: with OS_t the projected surplus, DS_t the existing service and a = r/(1 − (1 +
// r)^−n) the payment per unit borrowed, it is min_{t ∈ [s, s+n)} (OS_t/θ − DS_t)/a, and none when that is negative (the coverage
// already fails); a declared ceiling on the stock of debt, as a multiple of a year's revenue, bounds it further. Proposition:
// the coverage then holds in every repayment year, and at the year that attains the minimum it holds with equality.
//
// Debt dynamics. For debt d as a share of a base (GDP, or revenue) growing at g, at an effective rate r on last year's debt, and
// a primary balance pb as a share of the same base: d_{t+1} = d_t (1 + r)/(1 + g) − pb_{t+1}; the primary balance that holds the
// share where it is, d (r − g)/(1 + g). The years ahead under uncertainty: blocks of the record's joint deviations of (r, g, pb)
// from their means, drawn with replacement (the moving-block bootstrap), added to a baseline; the fan of quantiles and the share
// of paths above a declared threshold.
//
// Capacity and effort. The representative tax system (Advisory Commission on Intergovernmental Relations 1962; the method of
// equalisation in several federations): each base's representative rate is the whole's revenue from it over the whole's base;
// a government's capacity is its bases at those rates, its effort what it raised over its capacity.
//
// Pure computation on declared numbers; every threshold, rate, floor and ceiling is the caller's.
#pragma once

#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::fiscal {

inline constexpr double none = std::numeric_limits<double>::quiet_NaN();

// ---- accounts and ratios ---------------------------------------------------------------------------------------------------------

struct Line {
  std::string code, name, category;
};

struct Year {
  int year = 0;
  std::map<std::string, double> amounts;  // by line code
};

struct Accounts {
  std::string unit, currency;
  std::vector<Line> chart;
  std::vector<Year> years;
};

using Totals = std::map<std::string, double>;  // by category

inline Totals totals(const Accounts& a, const Year& y) {
  std::map<std::string, const Line*> by;
  for (const auto& l : a.chart) {
    if (l.category.empty()) throw std::invalid_argument("economics::fiscal: the line " + l.code + " has no category");
    if (!by.emplace(l.code, &l).second) throw std::invalid_argument("economics::fiscal: the chart maps the line " + l.code + " twice");
  }
  Totals t;
  for (const auto& l : a.chart) t[l.category] += 0;
  for (const auto& [code, v] : y.amounts) {
    const auto it = by.find(code);
    if (it == by.end()) throw std::invalid_argument("economics::fiscal: the line " + code + " of " + std::to_string(y.year) + " is not in the chart");
    if (!std::isfinite(v)) throw std::invalid_argument("economics::fiscal: the line " + code + " of " + std::to_string(y.year) + " is not a number");
    t[it->second->category] += v;
  }
  return t;
}

struct Term {
  std::string category;
  double sign = 1;
};

inline double sum(const Totals& t, const std::vector<Term>& terms) {
  double s = 0;
  for (const auto& x : terms) {
    const auto it = t.find(x.category);
    if (it == t.end()) throw std::invalid_argument("economics::fiscal: no category " + x.category);
    s += x.sign * it->second;
  }
  return s;
}

struct Ratio {
  std::string name;
  std::vector<Term> numerator, denominator;
};

enum class Sound { at_least, at_most };

struct Threshold {
  std::string ratio;
  Sound sound = Sound::at_least;   // the side that is sound
  double warning = none, intervention = none;  // crossing them, away from the sound side
};

enum class Status { sound, warning, intervention, undefined };

inline const char* name(Status s) {
  switch (s) {
    case Status::sound: return "sound";
    case Status::warning: return "warning";
    case Status::intervention: return "intervention";
    case Status::undefined: return "undefined";
  }
  return "?";
}

struct RatioValue {
  std::string ratio;
  int year = 0;
  double value = none;
  Status status = Status::undefined;
  std::string why;
};

inline Status judge(const Threshold& t, double v) {
  if (!std::isfinite(v)) return Status::undefined;
  auto worse = [&](double level) { return std::isfinite(level) && (t.sound == Sound::at_least ? v < level : v > level); };
  if (worse(t.intervention)) return Status::intervention;
  if (worse(t.warning)) return Status::warning;
  return Status::sound;
}

inline std::vector<RatioValue> ratios(const Accounts& a, const std::vector<Ratio>& rs, const std::vector<Threshold>& scheme = {}) {
  std::vector<RatioValue> out;
  for (const auto& y : a.years) {
    const auto t = totals(a, y);
    for (const auto& r : rs) {
      RatioValue v;
      v.ratio = r.name, v.year = y.year;
      const double den = sum(t, r.denominator), num = sum(t, r.numerator);
      v.value = den != 0 ? num / den : none;
      if (den == 0) v.why = "its denominator is zero";
      for (const auto& th : scheme)
        if (th.ratio == r.name) {
          v.status = judge(th, v.value);
          v.why = std::string(name(v.status)) + (v.status == Status::sound || v.status == Status::undefined ? "" : std::string(" (") + (th.sound == Sound::at_least ? "below " : "above ") +
                                                    std::to_string(v.status == Status::intervention ? th.intervention : th.warning) + ")");
        }
      out.push_back(v);
    }
  }
  return out;
}

// ---- the years ahead -------------------------------------------------------------------------------------------------------------

struct Loan {
  std::string name;
  double principal = 0, rate = 0;
  int first_year = 0, tenor = 1;   // repaid over tenor years from first_year
  bool bullet = false;             // interest only, the principal at the end
};

struct Service {
  double interest = 0, principal = 0, outstanding_after = 0;
};

// What a loan asks in a year (zero outside its life).
inline Service service(const Loan& l, int year) {
  if (l.tenor < 1 || l.principal < 0) throw std::invalid_argument("economics::fiscal: a loan has a tenor of a year or more and a principal that is not negative");
  Service s;
  const int k = year - l.first_year;  // 0 for the first repayment year
  if (k < 0) {
    s.outstanding_after = l.principal;
    return s;
  }
  if (k >= l.tenor) return s;
  if (l.bullet) {
    s.interest = l.principal * l.rate;
    s.principal = k == l.tenor - 1 ? l.principal : 0;
    s.outstanding_after = k == l.tenor - 1 ? 0 : l.principal;
    return s;
  }
  const double r = l.rate, n = l.tenor;
  const double payment = r > 0 ? l.principal * r / (1 - std::pow(1 + r, -n)) : l.principal / n;
  double out = l.principal;
  for (int j = 0; j <= k; j++) {
    const double i = out * r, p = payment - i;
    if (j == k) s.interest = i, s.principal = p, s.outstanding_after = out - p;
    out -= p;
  }
  if (std::fabs(s.outstanding_after) < 1e-9 * std::max(1.0, l.principal)) s.outstanding_after = 0;
  return s;
}

struct Projection {
  std::vector<int> years;
  std::vector<Totals> totals;              // each category grown
  std::vector<double> surplus, debt_service, coverage, outstanding;
};

// Categories grown from a base year's totals at declared rates (a category not named stays as it was); the surplus a declared
// signed sum; the loans' service and stock.
inline Projection project(const Totals& base, int base_year, int years, const std::map<std::string, double>& growth, const std::vector<Term>& surplus,
                          const std::vector<Loan>& loans) {
  if (years < 1) throw std::invalid_argument("economics::fiscal: at least a year ahead");
  for (const auto& [c, g] : growth)
    if (!base.count(c)) throw std::invalid_argument("economics::fiscal: a growth rate for a category the totals lack: " + c);
  Projection p;
  for (int k = 1; k <= years; k++) {
    const int y = base_year + k;
    Totals t;
    for (const auto& [c, v] : base) {
      const auto it = growth.find(c);
      t[c] = v * std::pow(1 + (it == growth.end() ? 0.0 : it->second), k);
    }
    double ds = 0, stock = 0;
    for (const auto& l : loans) {
      const auto s = service(l, y);
      ds += s.interest + s.principal, stock += s.outstanding_after;
    }
    const double os = sum(t, surplus);
    p.years.push_back(y), p.totals.push_back(t), p.surplus.push_back(os), p.debt_service.push_back(ds), p.outstanding.push_back(stock);
    p.coverage.push_back(ds > 0 ? os / ds : none);
  }
  return p;
}

struct Capacity {
  double largest = 0;            // the largest new annuity
  int binding_year = 0;          // the year whose coverage binds it (or 0)
  std::string binding;           // "coverage", "stock ceiling" or "coverage already below the floor"
  double payment_per_unit = 0;   // a = r/(1 − (1 + r)^−n)
};

// The largest new annuity at rate r over n years from year s keeping the coverage at or above θ in every repayment year inside
// the projection, and, if a ceiling k is declared, the stock of debt after the first year at most k times that year's `revenue`.
inline Capacity debt_capacity(const Projection& p, double rate, int tenor, int first_year, double floor, const std::vector<Term>& revenue = {}, double ceiling = none) {
  if (!(floor > 0) || tenor < 1) throw std::invalid_argument("economics::fiscal: a positive floor of coverage and a tenor of a year or more");
  Capacity c;
  c.payment_per_unit = rate > 0 ? rate / (1 - std::pow(1 + rate, -static_cast<double>(tenor))) : 1.0 / tenor;
  double best = std::numeric_limits<double>::infinity();
  bool any = false;
  for (std::size_t k = 0; k < p.years.size(); k++) {
    const int y = p.years[k];
    if (y < first_year || y >= first_year + tenor) continue;
    any = true;
    const double room = (p.surplus[k] / floor - p.debt_service[k]) / c.payment_per_unit;
    if (room < best) best = room, c.binding_year = y;
  }
  if (!any) throw std::invalid_argument("economics::fiscal: the new loan's repayment years lie outside the projection");
  c.binding = "coverage";
  if (best <= 0) {
    c.largest = 0, c.binding = "coverage already below the floor";
    return c;
  }
  c.largest = best;
  if (std::isfinite(ceiling) && !revenue.empty()) {
    for (std::size_t k = 0; k < p.years.size(); k++) {
      if (p.years[k] != first_year) continue;
      // After the first repayment year the new loan's stock is B (1 − share repaid); bounded by k·revenue − existing stock.
      const Loan unit{"unit", 1.0, rate, first_year, tenor, false};
      const double left = service(unit, first_year).outstanding_after;
      const double room = (ceiling * sum(p.totals[k], revenue) - p.outstanding[k]) / std::max(left, 1e-12);
      if (room < c.largest) c.largest = std::max(0.0, room), c.binding = "stock ceiling", c.binding_year = first_year;
    }
  }
  return c;
}

// ---- debt dynamics ---------------------------------------------------------------------------------------------------------------

inline double stabilising_balance(double d, double r, double g) { return d * (r - g) / (1 + g); }

// d_{t+1} = d_t (1 + r_t)/(1 + g_t) − pb_t, for each year of the paths given.
inline std::vector<double> debt_path(double d0, const std::vector<double>& r, const std::vector<double>& g, const std::vector<double>& pb) {
  if (r.size() != g.size() || r.size() != pb.size()) throw std::invalid_argument("economics::fiscal: a rate, a growth and a balance for every year");
  std::vector<double> d{d0};
  for (std::size_t t = 0; t < r.size(); t++) {
    if (!(g[t] > -1)) throw std::invalid_argument("economics::fiscal: growth above −100%");
    d.push_back(d.back() * (1 + r[t]) / (1 + g[t]) - pb[t]);
  }
  return d;
}

struct Fan {
  std::vector<double> quantiles;               // the levels asked, e.g. 0.1, 0.5, 0.9
  std::vector<std::vector<double>> paths_at;   // [year][quantile]
  std::vector<double> above;                   // [year]: the share of paths above the threshold
  std::vector<double> baseline;
};

// The years ahead under the record's joint uncertainty: deviations of (r, g, pb) from their means in the record, drawn in blocks
// of `block` consecutive years with replacement, added to the baseline paths.
inline Fan fan(double d0, const std::vector<double>& r, const std::vector<double>& g, const std::vector<double>& pb, const std::vector<double>& record_r,
               const std::vector<double>& record_g, const std::vector<double>& record_pb, std::size_t block, std::size_t draws, double threshold,
               std::vector<double> quantiles, std::uint64_t seed) {
  const std::size_t T = record_r.size(), H = r.size();
  if (record_g.size() != T || record_pb.size() != T || T < 2) throw std::invalid_argument("economics::fiscal: a record of at least two years of rate, growth and balance");
  if (block < 1 || block > T || draws < 10) throw std::invalid_argument("economics::fiscal: blocks of at least a year and no longer than the record, at least ten draws");
  auto mean = [](const std::vector<double>& v) {
    double m = 0;
    for (double x : v) m += x / static_cast<double>(v.size());
    return m;
  };
  const double mr = mean(record_r), mg = mean(record_g), mp = mean(record_pb);
  Fan f;
  f.quantiles = quantiles;
  f.baseline = debt_path(d0, r, g, pb);
  std::vector<std::vector<double>> at(H + 1);
  calx::random::Stream s(seed, 0x66616e00u, 0, 0);
  for (std::size_t k = 0; k < draws; k++) {
    std::vector<double> rr(H), gg(H), pp(H);
    std::size_t t = 0;
    while (t < H) {
      const std::size_t start = static_cast<std::size_t>(s.below(T - block + 1));
      for (std::size_t j = 0; j < block && t < H; j++, t++)
        rr[t] = r[t] + record_r[start + j] - mr, gg[t] = g[t] + record_g[start + j] - mg, pp[t] = pb[t] + record_pb[start + j] - mp;
    }
    const auto d = debt_path(d0, rr, gg, pp);
    for (std::size_t y = 0; y <= H; y++) at[y].push_back(d[y]);
  }
  for (std::size_t y = 0; y <= H; y++) {
    auto v = at[y];
    std::sort(v.begin(), v.end());
    std::vector<double> q;
    for (double level : quantiles) {
      const double h = level * static_cast<double>(v.size() - 1);
      const std::size_t lo = static_cast<std::size_t>(std::floor(h));
      const std::size_t hi = std::min(lo + 1, v.size() - 1);
      q.push_back(v[lo] + (h - static_cast<double>(lo)) * (v[hi] - v[lo]));  // Hyndman and Fan's type 7
    }
    f.paths_at.push_back(q);
    double over = 0;
    for (double x : v) over += x > threshold;
    f.above.push_back(over / static_cast<double>(v.size()));
  }
  return f;
}

// ---- capacity and effort ----------------------------------------------------------------------------------------------------------

struct Effort {
  std::vector<double> rates;     // the representative rate of each base
  std::vector<double> capacity;  // of each government
  std::vector<double> effort;    // what it raised over its capacity
};

// bases[i][k] and revenues[i][k]: government i's base k and what it raised from it.
inline Effort representative_tax_system(const std::vector<std::vector<double>>& bases, const std::vector<std::vector<double>>& revenues) {
  const std::size_t n = bases.size();
  if (n == 0 || revenues.size() != n) throw std::invalid_argument("economics::fiscal: bases and revenues for every government");
  const std::size_t K = bases[0].size();
  for (std::size_t i = 0; i < n; i++)
    if (bases[i].size() != K || revenues[i].size() != K) throw std::invalid_argument("economics::fiscal: every government with every base");
  Effort e;
  e.rates.assign(K, 0);
  for (std::size_t k = 0; k < K; k++) {
    double B = 0, R = 0;
    for (std::size_t i = 0; i < n; i++) B += bases[i][k], R += revenues[i][k];
    e.rates[k] = B > 0 ? R / B : 0;
  }
  for (std::size_t i = 0; i < n; i++) {
    double cap = 0, raised = 0;
    for (std::size_t k = 0; k < K; k++) cap += e.rates[k] * bases[i][k], raised += revenues[i][k];
    e.capacity.push_back(cap);
    e.effort.push_back(cap > 0 ? raised / cap : none);
  }
  return e;
}

}  // namespace economics::fiscal
