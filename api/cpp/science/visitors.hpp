// The visitor stock, as one composition any work may call (ADR 0156): urban science's `visitors` on accommodation returns by month and
// origin, under declared readings of "visitor", with intensity and seasonality judged against declared bands. Nothing in it names a city.
//
//   visitors.stock {places: [{id, name?, year, nights: {source: {origin: [12 months, null where withheld]}}, population?, area_km2?,
//                   arrivals?: {origin: annual arrivals of the registered source},
//                   allocate?: {source, key_place: {origin: [12]}, key_region: {origin: [12]}}}],
//                  readings: [{name, sources: ["registered", "platform"], unregistered?, same_day?, same_day_hours?, source?}],
//                  bands?: [{measure, lower?, upper?, holder?}]}
//                 each place under each reading: the months' stock (overnight and same-day, by origin), the mean and peak, per thousand
//                 residents and per km², nights per resident, the peak over the mean and the Gini of the stock over the year's days, the
//                 foreign and platform shares, the verdicts against the bands; the registered mean stay where arrivals are given
//
// A source's nights published only for a region around the place are carried to it by `allocate` before any reading: the region's
// nights times the place's key over the region's, month by month and origin by origin.

#pragma once

#include <urban/visitors/visitors.hpp>

#include <science/json.hpp>

#include <array>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::visitors {

namespace uv = urban::visitors;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }
inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline std::map<std::string, std::array<double, 12>> by_origin(const Json& j) {
  std::map<std::string, std::array<double, 12>> out;
  for (const auto& [origin, months] : j.as_object()) {
    const auto& a = months.as_array();
    if (a.size() != 12) throw std::invalid_argument("visitors: twelve months for " + origin);
    std::array<double, 12> v{};
    for (int m = 0; m < 12; m++) v[m] = a[m].is_number() ? a[m].as_number() : uv::unknown;
    out[origin] = v;
  }
  return out;
}

inline Json months_json(const std::array<double, 12>& a) {
  Json j = Json::array();
  for (const double v : a) j.push(finite(v));
  return j;
}

inline Json stock(const Json& r) {
  std::vector<uv::Reading> readings;
  for (const auto& x : r["readings"].as_array()) {
    uv::Reading R;
    R.name = x["name"].text("reading");
    R.sources.clear();
    for (const auto& s : x["sources"].as_array()) R.sources.insert(s.text());
    R.unregistered = num(x["unregistered"], 0), R.same_day = num(x["same_day"], 0), R.same_day_hours = num(x["same_day_hours"], 6);
    R.source = x["source"].text("the request");
    readings.push_back(R);
  }
  std::vector<uv::Band> bands;
  if (r["bands"].is_array())
    for (const auto& b : r["bands"].as_array())
      bands.push_back({b["measure"].text(), num(b["lower"], -std::numeric_limits<double>::infinity()), num(b["upper"], std::numeric_limits<double>::infinity()), b["holder"].text("declared")});
  Json places = Json::array();
  for (const auto& p : r["places"].as_array()) {
    uv::Nights n;
    for (const auto& [source, origins] : p["nights"].as_object()) n[source] = by_origin(origins);
    Json allocated = Json();
    if (p["allocate"].is_object()) {
      const auto& a = p["allocate"];
      const std::string src = a["source"].text();
      if (!n.count(src)) throw std::invalid_argument("visitors: no nights of the source " + src + " to allocate");
      n[src] = uv::allocate(n[src], by_origin(a["key_place"]), by_origin(a["key_region"]));
      Json carried = Json::object();
      for (const auto& [origin, v] : n[src]) carried.set(origin, months_json(v));
      allocated = Json::object({{"source", src}, {"nights", carried}});
    }
    const int year = static_cast<int>(num(p["year"], 2024));
    const double population = num(p["population"], uv::unknown), area = num(p["area_km2"], uv::unknown);
    Json results = Json::array();
    for (const auto& R : readings) {
      const auto a = uv::stock(n, year, R, population, area, bands);
      Json months = Json::array();
      for (const auto& M : a.months) {
        Json origins = Json::object();
        for (const auto& [o, v] : M.by_origin) origins.set(o, v);
        months.push(Json::object({{"days", static_cast<double>(M.days)}, {"complete", M.complete}, {"overnight", M.overnight}, {"same_day", M.same_day},
                                  {"stock", M.stock()}, {"by_origin", origins}}));
      }
      Json verdicts = Json::object();
      for (const auto& [m, v] : a.verdicts) verdicts.set(m, v);
      results.push(Json::object({{"reading", a.reading}, {"source", R.source}, {"complete", a.complete}, {"months", months}, {"nights", a.nights}, {"mean", a.mean},
                                 {"peak", a.peak}, {"peak_month", static_cast<double>(a.peak_month)}, {"peak_to_mean", finite(a.peak_to_mean)}, {"gini", finite(a.gini)},
                                 {"foreign_share", finite(a.foreign_share)}, {"platform_share", finite(a.platform_share)}, {"per_thousand_mean", finite(a.per_thousand_mean)},
                                 {"per_thousand_peak", finite(a.per_thousand_peak)}, {"per_km2_mean", finite(a.per_km2_mean)},
                                 {"nights_per_resident", finite(a.nights_per_resident)}, {"verdicts", verdicts}}));
    }
    Json stay = Json();
    if (p["arrivals"].is_object() && n.count("registered")) {
      uv::Stay st;
      bool ok = true;
      for (const auto& [origin, v] : n.at("registered"))
        for (const double x : v) ok = ok && uv::known(x), st.nights += uv::known(x) ? x : 0;
      for (const auto& [origin, a] : p["arrivals"].as_object()) ok = ok && a.is_number(), st.arrivals += a.is_number() ? a.as_number() : 0;
      if (ok && st.arrivals > 0) stay = Json(st.mean_stay());
    }
    places.push(Json::object({{"id", p["id"].text("")}, {"name", p["name"].text(p["id"].text(""))}, {"readings", results}, {"mean_stay", stay}, {"allocated", allocated}}));
  }
  return Json::object({{"places", places}});
}

inline bool handles(const std::string& op) { return op.rfind("visitors.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "visitors.stock") return stock(r);
  throw std::invalid_argument("visitors: no operation " + op);
}

}  // namespace science::visitors
