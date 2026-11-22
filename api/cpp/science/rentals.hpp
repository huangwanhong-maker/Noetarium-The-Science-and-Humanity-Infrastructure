// Short-term rentals and housing pressure, as one composition any work may call (ADR 0163): urban science's `rentals` on a city's
// listings under declared readings of the review-based model. Nothing in it names a city.
//
//   rentals.assess {listings: [[area, type, reviews_ltm, minimum_nights, price, host, host_listings, …]],
//                   readings: [{name, review_rate?, stay?, minimum_binds?, cap?, withdrawn_nights?, withdrawn_host?, source?}]}
//                  each reading's nights booked, entire homes withdrawn from residents, revenue over priced listings, hosts' concentration,
//                  and each area's listings, entire homes, nights, withdrawn homes and revenue
//
// A listing is a row in the data library's order (platform_rentals.py's `fields`); a missing number is null.

#pragma once

#include <urban/rentals/rentals.hpp>

#include <science/json.hpp>

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::rentals {

namespace ur = urban::rentals;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline Json assess(const Json& r) {
  std::vector<ur::Listing> ls;
  for (const auto& x : r["listings"].as_array()) {
    ur::Listing l;
    l.area = x[0].text(""), l.type = x[1].text(""), l.reviews = num(x[2], 0), l.minimum_nights = num(x[3], 1), l.price = num(x[4], ur::unknown);
    l.host = x[5].is_number() ? std::to_string(static_cast<long long>(x[5].as_number())) : x[5].text(""), l.host_listings = num(x[6], 1);
    ls.push_back(l);
  }
  Json out = Json::array();
  for (const auto& x : r["readings"].as_array()) {
    ur::Reading R;
    R.name = x["name"].text("reading"), R.review_rate = num(x["review_rate"], 0.5), R.stay = num(x["stay"], 3), R.cap = num(x["cap"], 0.7);
    R.minimum_binds = !(x["minimum_binds"].is_bool() && !x["minimum_binds"].as_bool());
    R.withdrawn_nights = num(x["withdrawn_nights"], 90), R.withdrawn_host = num(x["withdrawn_host"], 0), R.source = x["source"].text("the request");
    const auto a = ur::assess(ls, R);
    Json areas = Json::array();
    for (const auto& [k, v] : a.areas)
      areas.push(Json::object({{"area", k}, {"listings", v.listings}, {"entire", v.entire}, {"nights", v.nights}, {"withdrawn", v.withdrawn}, {"revenue", v.revenue}}));
    out.push(Json::object({{"reading", a.reading}, {"source", R.source}, {"listings", a.listings}, {"entire", a.entire}, {"nights", a.nights_total},
                           {"withdrawn", a.withdrawn}, {"revenue", a.revenue}, {"priced_nights", a.priced_nights}, {"hhi", a.hhi}, {"largest_host", a.largest_host},
                           {"top10", a.top10}, {"multi_host_share", a.multi_host_share}, {"areas", areas}}));
  }
  return Json::object({{"readings", out}});
}

inline bool handles(const std::string& op) { return op.rfind("rentals.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "rentals.assess") return assess(r);
  throw std::invalid_argument("rentals: no operation " + op);
}

}  // namespace science::rentals
