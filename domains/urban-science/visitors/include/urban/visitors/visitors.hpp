// Visitors: the stock of visitors present in a place, month by month and by origin, from accommodation returns and declared readings
// of "visitor", with the intensity and seasonality a destination is judged by (theory/visitors.md; ADR 0156).
//
// A night spent is a visitor present for a day, so a month's nights over its days is the mean number of overnight visitors present
// (the stock). Over a period whose stays begin and end within it, nights are arrivals times the mean stay, Little's law L = λW in
// the form the accommodation returns give. What counts as a visitor is a reading (the International Recommendations for Tourism
// Statistics leave the "usual environment" to each country): registered establishments only, or platform rentals too; stays with
// friends and relatives and in second homes, which no return holds, at a declared rate per registered night; same-day visitors at a
// declared rate per overnight night and for declared hours. Each reading gives its own stock, and none is the stock until one is chosen.
//
// Measures: the mean and peak stock, per thousand residents and per km² (intensity and density), the peak month over the mean and the
// Gini coefficient of the stock over the year's days (seasonality, on nomos's Gini), the foreign share of nights, the platform share;
// each judged, where a band is declared, as below, within or above it. Nothing here names a city.

#pragma once

#include <nomos/measurement/inequality.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace urban::visitors {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::visitors: " + what);
}
}  // namespace detail

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();
inline bool known(double x) { return std::isfinite(x); }

// Days in each month of a year (Gregorian).
inline std::array<int, 12> days_in(int year) {
  const bool leap = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
  return {31, leap ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
}

// Nights by month (twelve values, NaN where withheld) for each source and origin: source → origin → nights.
using Nights = std::map<std::string, std::map<std::string, std::array<double, 12>>>;

struct Reading {
  std::string name;
  std::set<std::string> sources{"registered"};  // which accommodation counts: "registered", "platform"
  double unregistered = 0;    // nights with friends and relatives or in second homes, per registered night
  double same_day = 0;        // same-day visits per overnight night
  double same_day_hours = 6;  // hours a same-day visitor is present
  std::string source;
};

struct Band {
  std::string measure;   // "per_thousand_mean", "per_thousand_peak", "per_km2_mean", "nights_per_resident", "peak_to_mean", "gini", "foreign_share"
  double lower = -std::numeric_limits<double>::infinity(), upper = std::numeric_limits<double>::infinity();
  std::string holder;    // whose standard
};

struct Month {
  int days = 0;
  bool complete = true;                 // every counted source's every origin known
  double overnight = 0, same_day = 0;   // mean visitors present
  std::map<std::string, double> by_origin;  // mean overnight visitors present, by origin (unregistered stays shared as registered ones)
  double stock() const { return overnight + same_day; }
};

struct Result {
  std::string reading;
  std::array<Month, 12> months;
  bool complete = true;
  double nights = 0;                    // overnight nights counted in the year (unregistered included)
  double mean = 0, peak = 0;            // mean and peak monthly stock
  int peak_month = 0;                   // 1..12
  double peak_to_mean = 0, gini = 0;
  double foreign_share = unknown, platform_share = unknown;
  double per_thousand_mean = unknown, per_thousand_peak = unknown, per_km2_mean = unknown, nights_per_resident = unknown;
  std::map<std::string, std::string> verdicts;  // measure → "below", "within" or "above"
};

// The stock under a reading. `population` and `area_km2` may be NaN; measures that need them are then NaN.
inline Result stock(const Nights& n, int year, const Reading& r, double population = unknown, double area_km2 = unknown, const std::vector<Band>& bands = {}) {
  detail::require(!r.sources.empty(), "a reading counts at least one source");
  detail::require(r.unregistered >= 0 && r.same_day >= 0 && r.same_day_hours >= 0 && r.same_day_hours <= 24, "a reading's rates are not negative and a same-day visit lasts at most a day");
  for (const auto& s : r.sources) detail::require(n.count(s), "no nights of the source " + s);
  const auto days = days_in(year);
  Result out;
  out.reading = r.name;
  double reg = 0, plat = 0, foreign = 0, all_origin = 0;
  for (int m = 0; m < 12; m++) {
    Month& M = out.months[m];
    M.days = days[m];
    double overnight_nights = 0;
    for (const auto& s : r.sources)
      for (const auto& [origin, v] : n.at(s)) {
        if (!known(v[m])) {
          M.complete = false;
          continue;
        }
        detail::require(v[m] >= 0, "nights are not negative");
        const double add = v[m] * (s == "registered" ? 1 + r.unregistered : 1);
        overnight_nights += add;
        M.by_origin[origin] += add / M.days;
        (s == "registered" ? reg : plat) += v[m];
        if (origin == "FOR") foreign += add;
        all_origin += add;
      }
    M.overnight = overnight_nights / M.days;
    M.same_day = r.same_day * overnight_nights * (r.same_day_hours / 24) / M.days;
    out.complete = out.complete && M.complete;
    out.nights += overnight_nights;
  }
  std::vector<double> s(12), w(12);
  double total_days = 0, visitor_days = 0;
  for (int m = 0; m < 12; m++) {
    s[m] = out.months[m].stock(), w[m] = out.months[m].days;
    total_days += w[m], visitor_days += s[m] * w[m];
    if (s[m] > out.peak) out.peak = s[m], out.peak_month = m + 1;
  }
  out.mean = visitor_days / total_days;
  out.peak_to_mean = out.mean > 0 ? out.peak / out.mean : unknown;
  out.gini = out.mean > 0 ? nomos::measurement::gini(s, w) : unknown;
  if (all_origin > 0 && n.at(*r.sources.begin()).count("FOR")) out.foreign_share = foreign / all_origin;
  if (r.sources.count("platform") && reg + plat > 0) out.platform_share = plat / (reg + plat);
  if (known(population) && population > 0) {
    out.per_thousand_mean = 1000 * out.mean / population, out.per_thousand_peak = 1000 * out.peak / population;
    out.nights_per_resident = out.nights / population;
  }
  if (known(area_km2) && area_km2 > 0) out.per_km2_mean = out.mean / area_km2;
  const std::map<std::string, double> values{{"per_thousand_mean", out.per_thousand_mean}, {"per_thousand_peak", out.per_thousand_peak}, {"per_km2_mean", out.per_km2_mean},
                                             {"nights_per_resident", out.nights_per_resident}, {"peak_to_mean", out.peak_to_mean}, {"gini", out.gini}, {"foreign_share", out.foreign_share}};
  for (const auto& b : bands) {
    detail::require(values.count(b.measure), "no measure " + b.measure);
    detail::require(b.lower <= b.upper, "a band's lower end is not above its upper");
    const double v = values.at(b.measure);
    if (known(v)) out.verdicts[b.measure] = v < b.lower ? "below" : v > b.upper ? "above" : "within";
  }
  return out;
}

// A region's nights carried to a place within it by a key, month by month: nights × key_place / key_region (each origin by its own key
// when keyed by origin). The shares of all the places of a region that partition it sum to the region's nights (control totals).
inline std::map<std::string, std::array<double, 12>> allocate(const std::map<std::string, std::array<double, 12>>& region,
                                                              const std::map<std::string, std::array<double, 12>>& key_place,
                                                              const std::map<std::string, std::array<double, 12>>& key_region) {
  std::map<std::string, std::array<double, 12>> out;
  for (const auto& [origin, v] : region) {
    detail::require(key_place.count(origin) && key_region.count(origin), "a key for every origin");
    auto& o = out[origin];
    for (int m = 0; m < 12; m++) {
      const double kp = key_place.at(origin)[m], kr = key_region.at(origin)[m];
      if (!known(v[m]) || !known(kp) || !known(kr) || !(kr > 0)) {
        o[m] = unknown;
        continue;
      }
      detail::require(kp >= 0 && kp <= kr * (1 + 1e-12), "a place's key lies between nothing and its region's");
      o[m] = v[m] * kp / kr;
    }
  }
  return out;
}

// The mean stay (nights per arrival), and the mean stock it implies at a rate of arrivals per day: L = λW.
struct Stay {
  double nights = 0, arrivals = 0;
  double mean_stay() const { return arrivals > 0 ? nights / arrivals : unknown; }
};

}  // namespace urban::visitors
