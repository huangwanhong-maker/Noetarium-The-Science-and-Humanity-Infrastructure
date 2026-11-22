// Transit: public transport's stops, the service each gives, and who lives within reach of it, SDG 11.2.1 read under declared readings
// (theory/transit.md; ADR 0151).
//
// SERVICE AT A STOP. A stop's departures in a day, by mode, are read in a window [a, b): their number, their rate an hour, the longest
// wait and the mean wait of a passenger arriving at a uniformly random instant of the window for the next departure, taken beyond b
// when the day has one (Proposition 1: with departures t_1 < … < t_n, the next after b included, the mean wait is
// (1/(b − a)) Σ_k [ (t_{k+1} − max(t_k, a))² − max(0, t_{k+1} − b)² ] / 2, which is h/2 for a regular headway h). A passenger arriving
// after the day's last departure waits for none: the mean wait is then infinite, and said to be.
//
// SDG 11.2.1. The share of a population within 500 m along the street network of a stop of a low-capacity system (bus, BRT) and/or 1 km
// of a stop of a high-capacity system (rail, metro, ferry) (UN-Habitat's metadata of 2025-04-23). Its open terms are a reading's:
// which modes are of which capacity (a tram is named by neither list), whether a stop counts only where its service is frequent (the
// metadata's secondary indicator recommends an average wait at the peak of at most 20 minutes), the distances, and whether distances run
// along the network or in a straight line. Each reading gives its own share; none is the indicator's value until a reading is chosen.
//
// DISTANCES. Along the network, the walking ways are urban science's access network at a speed of 1 m/s, so that its times are metres,
// and each place's distance to the nearest stop of a class is one search from all those stops at once, a place and a stop each joined to
// the network at its nearest joinable point with the metres between (access's join). In a straight line, the least Euclidean distance.
// Coordinates are metres of a local frame.

#pragma once

#include <urban/access/catchment.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace urban::transit {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::transit: " + what);
}
}  // namespace detail

inline constexpr double never = std::numeric_limits<double>::infinity();

struct Stop {
  std::string id, name;
  double x = 0, y = 0;
  std::map<std::string, std::vector<double>> departures;  // mode → seconds of the service day
};

struct Service {
  double departures = 0;     // within the window
  double per_hour = 0;
  double mean_wait = never;  // s, of an arrival uniform on the window
  double longest_wait = never;
  bool continues = false;    // a departure follows the window, so that every arrival is served
};

// Proposition 1.
inline Service service(std::vector<double> t, double a, double b) {
  detail::require(b > a, "a window ends after it begins");
  std::sort(t.begin(), t.end());
  Service s;
  for (const double x : t) s.departures += x >= a && x < b;
  s.per_hour = s.departures / ((b - a) / 3600);
  const auto first = std::lower_bound(t.begin(), t.end(), a);
  if (first == t.end()) return s;
  const auto after = std::lower_bound(t.begin(), t.end(), b);  // the first departure at or after b
  s.continues = after != t.end();
  if (!s.continues) return s;
  double integral = 0, longest = 0, from = a;
  for (auto it = first; it <= after; ++it) {
    const double next = *it, gap = next - from;
    integral += 0.5 * (gap * gap - std::max(0.0, next - b) * std::max(0.0, next - b));
    longest = std::max(longest, gap);  // an arrival just after `from` waits the whole gap
    from = next;
    if (next >= b) break;
  }
  s.mean_wait = integral / (b - a);
  s.longest_wait = longest;
  return s;
}

enum class Capacity { none, low, high };

struct Reading {
  std::string name;
  std::map<std::string, Capacity> capacity;  // mode → its class; a mode not named does not count
  double low_metres = 500, high_metres = 1000;
  double longest_mean_wait = never;          // s: a stop counts for a class only where its mean wait at the peak is at most this
  bool along_network = true;
  std::string source;
};

// The metadata's classes: bus, BRT, trolleybus and coach of low capacity; rail, metro, ferry and monorail of high. A tram is named by
// neither and is declared here, low or high.
inline Reading metadata_reading(bool trams_high, double longest_mean_wait = never, bool along_network = true) {
  Reading r;
  r.name = std::string("trams ") + (trams_high ? "high" : "low") + (std::isfinite(longest_mean_wait) ? ", frequent stops" : ", every stop") +
           (along_network ? "" : ", straight line");
  r.capacity = {{"bus", Capacity::low}, {"trolleybus", Capacity::low}, {"coach", Capacity::low}, {"rail", Capacity::high}, {"metro", Capacity::high},
                {"ferry", Capacity::high}, {"monorail", Capacity::high}, {"tram", trams_high ? Capacity::high : Capacity::low}};
  r.longest_mean_wait = longest_mean_wait;
  r.along_network = along_network;
  r.source = "UN-Habitat's metadata of SDG 11.2.1 (2025-04-23); the tram's class and the test of frequency declared";
  return r;
}

// The walking network at 1 m/s, its ways densified so that no two points of a way lie more than `spacing` metres apart: access joins a
// place at the nearest point, which on a long straight segment would otherwise be far from the place.
inline access::Network walking(const std::vector<std::vector<double>>& lines, double spacing = 20) {
  detail::require(spacing > 0, "a spacing is positive");
  std::vector<access::Way> ways;
  for (const auto& xy : lines) {
    detail::require(xy.size() >= 4 && xy.size() % 2 == 0, "a way has at least two points");
    access::Way w;
    w.speed = 1.0;
    w.xy = {xy[0], xy[1]};
    for (std::size_t k = 2; k < xy.size(); k += 2) {
      const double x0 = xy[k - 2], y0 = xy[k - 1], x1 = xy[k], y1 = xy[k + 1];
      const int parts = std::max(1, static_cast<int>(std::ceil(std::hypot(x1 - x0, y1 - y0) / spacing)));
      for (int j = 1; j <= parts; j++) {
        w.xy.push_back(x0 + (x1 - x0) * j / parts);
        w.xy.push_back(y0 + (y1 - y0) * j / parts);
      }
    }
    ways.push_back(std::move(w));
  }
  return access::network_of(ways);
}

struct Place {
  double x = 0, y = 0, people = 0;
};

struct Result {
  std::string reading;
  double people = 0, within = 0, share = 0;
  std::size_t low_stops = 0, high_stops = 0;      // stops counting for each class
  std::vector<double> to_low, to_high;            // metres from each place to the nearest stop of each class
  std::vector<char> served;
};

// Whether a stop counts for a class under a reading: some mode of that class serves it, and, where frequency is asked, the departures of
// all its modes of that class together leave a mean wait at the peak no longer than the reading allows.
inline bool counts(const Stop& s, Capacity c, const Reading& r, double peak_a, double peak_b) {
  std::vector<double> merged;
  bool any = false;
  for (const auto& [mode, times] : s.departures) {
    const auto it = r.capacity.find(mode);
    if (it == r.capacity.end() || it->second != c || times.empty()) continue;
    any = true;
    merged.insert(merged.end(), times.begin(), times.end());
  }
  if (!any) return false;
  if (!std::isfinite(r.longest_mean_wait)) return true;
  return service(merged, peak_a, peak_b).mean_wait <= r.longest_mean_wait;
}

// `walk`: the access network of the walking ways at 1 m/s (times in metres); unused for a reading in a straight line.
inline Result access_share(const access::Network& walk, const std::vector<Place>& places, const std::vector<Stop>& stops, const Reading& r, double peak_a, double peak_b) {
  detail::require(r.low_metres > 0 && r.high_metres > 0, "a reading's distances are positive");
  Result out;
  out.reading = r.name;
  std::vector<std::pair<double, double>> low, high, demand;
  for (const auto& s : stops) {
    if (counts(s, Capacity::low, r, peak_a, peak_b)) low.push_back({s.x, s.y});
    if (counts(s, Capacity::high, r, peak_a, peak_b)) high.push_back({s.x, s.y});
  }
  out.low_stops = low.size();
  out.high_stops = high.size();
  for (const auto& p : places) {
    detail::require(p.people >= 0, "a place's people are at least zero");
    demand.push_back({p.x, p.y});
  }
  const auto nearest = [&](const std::vector<std::pair<double, double>>& supply) {
    std::vector<double> d(demand.size(), never);
    if (supply.empty() || demand.empty()) return d;
    if (r.along_network) return access::nearest_supply(walk, demand, supply, 1.0);
    for (std::size_t i = 0; i < demand.size(); i++)
      for (const auto& [x, y] : supply) d[i] = std::min(d[i], std::hypot(demand[i].first - x, demand[i].second - y));
    return d;
  };
  out.to_low = nearest(low);
  out.to_high = nearest(high);
  for (std::size_t i = 0; i < places.size(); i++) {
    const bool ok = out.to_low[i] <= r.low_metres || out.to_high[i] <= r.high_metres;
    out.served.push_back(ok);
    out.people += places[i].people;
    if (ok) out.within += places[i].people;
  }
  out.share = out.people > 0 ? out.within / out.people : 0;
  return out;
}

}  // namespace urban::transit
