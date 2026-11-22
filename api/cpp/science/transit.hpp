// Public transport and who can reach it, as one composition any work may call (ADR 0151): urban science's `transit` on a place's
// walking ways, people and stops with their departures. Nothing in it names a city.
//
//   transit.access   {ways: [[x0, y0, x1, y1, …], …], places: [[x, y, people], …], stops: [{id, name, x, y, departures: {mode: [s, …]}}],
//                    peak?: [a, b] (s of the service day, default 7:00–9:00), readings?: [{trams_high?, frequent_minutes?, along_network?}
//                    | {name, capacity: {mode: "low" | "high"}, low_metres?, high_metres?, frequent_minutes?, along_network?}], spacing?}
//                    each reading's SDG 11.2.1 (the share within reach, the people, the stops counting for each class); every stop's
//                    service at the peak (departures an hour, mean and longest wait) by mode and together; and each place's
//                    distances and whether it is served under the first reading
//
// Coordinates are metres of a local frame. With no readings given, four: trams of low or high capacity, every stop or only stops whose
// mean wait at the peak is at most 20 minutes, all along the network.

#pragma once

#include <urban/transit/transit.hpp>

#include <science/json.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::transit {

namespace ut = urban::transit;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline ut::Reading reading_of(const Json& r) {
  const double minutes = num(r["frequent_minutes"], 0);
  const double wait = minutes > 0 ? minutes * 60 : ut::never;
  const bool network = !(r["along_network"].is_bool() && !r["along_network"].as_bool());
  if (!r["capacity"].is_object()) return ut::metadata_reading(r["trams_high"].is_bool() && r["trams_high"].as_bool(), wait, network);
  ut::Reading R;
  R.name = r["name"].text("declared");
  for (const auto& [mode, c] : r["capacity"].as_object()) {
    const std::string k = c.text();
    if (k != "low" && k != "high") throw std::invalid_argument("transit: a mode's capacity is low or high (" + mode + ")");
    R.capacity[mode] = k == "high" ? ut::Capacity::high : ut::Capacity::low;
  }
  R.low_metres = num(r["low_metres"], 500);
  R.high_metres = num(r["high_metres"], 1000);
  R.longest_mean_wait = wait;
  R.along_network = network;
  R.source = r["source"].text("the request");
  return R;
}

inline Json access(const Json& r) {
  std::vector<std::vector<double>> lines;
  for (const auto& w : r["ways"].as_array()) {
    std::vector<double> xy;
    for (const auto& v : w.as_array()) xy.push_back(v.as_number());
    if (xy.size() >= 4) lines.push_back(std::move(xy));
  }
  std::vector<ut::Place> places;
  for (const auto& p : r["places"].as_array()) places.push_back({p[0].as_number(), p[1].as_number(), p[2].as_number()});
  std::vector<ut::Stop> stops;
  for (const auto& s : r["stops"].as_array()) {
    ut::Stop st{s["id"].text(""), s["name"].text(""), s["x"].as_number(), s["y"].as_number(), {}};
    for (const auto& [mode, times] : s["departures"].as_object())
      for (const auto& t : times.as_array()) st.departures[mode].push_back(t.as_number());
    stops.push_back(std::move(st));
  }
  const double a = r["peak"].is_array() ? r["peak"][0].as_number() : 7 * 3600.0, b = r["peak"].is_array() ? r["peak"][1].as_number() : 9 * 3600.0;
  std::vector<ut::Reading> readings;
  if (r["readings"].is_array())
    for (const auto& x : r["readings"].as_array()) readings.push_back(reading_of(x));
  if (readings.empty())
    for (const bool trams : {false, true})
      for (const double wait : {ut::never, 20.0 * 60}) readings.push_back(ut::metadata_reading(trams, wait));
  const auto walk = ut::walking(lines, num(r["spacing"], 20));
  Json results = Json::array();
  std::vector<ut::Result> all;
  for (const auto& R : readings) {
    all.push_back(ut::access_share(walk, places, stops, R, a, b));
    const auto& x = all.back();
    results.push(Json::object({{"reading", x.reading}, {"share", x.share}, {"within", x.within}, {"people", x.people},
                               {"low_stops", static_cast<double>(x.low_stops)}, {"high_stops", static_cast<double>(x.high_stops)},
                               {"low_metres", R.low_metres}, {"high_metres", R.high_metres}, {"source", R.source}}));
  }
  Json service = Json::array();
  for (const auto& s : stops) {
    std::vector<double> merged;
    Json modes = Json::object();
    for (const auto& [mode, times] : s.departures) {
      merged.insert(merged.end(), times.begin(), times.end());
      const auto v = ut::service(times, a, b);
      modes.set(mode, Json::object({{"per_hour", v.per_hour}, {"mean_wait", finite(v.mean_wait)}}));
    }
    const auto v = ut::service(merged, a, b);
    service.push(Json::object({{"id", s.id}, {"name", s.name}, {"x", s.x}, {"y", s.y}, {"day_departures", static_cast<double>(merged.size())},
                               {"per_hour", v.per_hour}, {"mean_wait", finite(v.mean_wait)}, {"longest_wait", finite(v.longest_wait)}, {"modes", modes}}));
  }
  Json first = Json::array();
  if (!all.empty())
    for (std::size_t i = 0; i < places.size(); i++)
      first.push(Json::array({Json(places[i].x), Json(places[i].y), Json(places[i].people), finite(all[0].to_low[i]), finite(all[0].to_high[i]), Json(static_cast<bool>(all[0].served[i]))}));
  return Json::object({{"readings", results}, {"stops", service}, {"places", first}, {"peak", Json::array({Json(a), Json(b)})}});
}

inline bool handles(const std::string& op) { return op.rfind("transit.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "transit.access") return access(r);
  throw std::invalid_argument("transit: no operation " + op);
}

}  // namespace science::transit
