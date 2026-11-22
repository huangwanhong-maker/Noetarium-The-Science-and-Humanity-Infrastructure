// Short-term rentals and housing pressure (theory note `theory/rentals.md`; ADR 0163): the nights a city's listings are booked, read
// from the reviews guests leave under declared readings, the homes they take from residents, and how concentrated their supply is.
//
// No platform publishes its bookings. The review-based model (Inside Airbnb's, after the San Francisco planning department's) reads a
// listing's nights booked in a year as its reviews over the share of stays that leave one, times the length of a stay (at least the
// listing's minimum nights), capped at a declared share of the year: n = min(c · 365, (R / ρ) · max(s, m)). The review rate ρ, the
// stay s and the cap c are readings, and each gives its own nights; a stay's length may be taken from the platforms' own statistics
// (Eurostat's nights rented out over stays). An entire home counts as withdrawn from residents when booked more nights than a declared
// threshold, or held by a host with at least a declared number of listings; the thresholds are readings too.
//
// Concentration: hosts' shares of listings (the Herfindahl–Hirschman index and the share of the largest), and each area's share of the
// city's listings and of its withdrawn homes. Nothing here names a city.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace urban::rentals {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::rentals: " + what);
}
}  // namespace detail

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

struct Listing {
  std::string area, type, host;     // type: "entire", "private", "shared", "hotel"
  double reviews = 0;               // reviews in the last twelve months
  double minimum_nights = 1;
  double price = unknown;           // a night
  double host_listings = 1;         // the host's listings in the city
};

struct Reading {
  std::string name;
  double review_rate = 0.5;         // the share of stays that leave a review
  double stay = 3;                  // nights a stay, before the listing's minimum
  bool minimum_binds = true;        // a stay lasts at least the listing's minimum nights
  double cap = 0.7;                 // the most of a year a listing can be booked
  double withdrawn_nights = 90;     // an entire home booked more nights than this is withdrawn
  double withdrawn_host = 0;        // or held by a host with at least this many listings (0: not a test)
  std::string source;
};

struct Area {
  double listings = 0, entire = 0, nights = 0, withdrawn = 0, revenue = 0;
};

struct Result {
  std::string reading;
  std::vector<double> nights;       // each listing's nights booked a year
  double listings = 0, entire = 0, nights_total = 0, withdrawn = 0;
  double revenue = 0, priced_nights = 0;   // over listings with a price, and the nights they hold
  double hhi = 0, largest_host = 0, top10 = 0;   // hosts' shares of listings
  double multi_host_share = 0;      // of listings held by hosts with more than one
  std::map<std::string, Area> areas;
};

inline double booked(const Listing& l, const Reading& r) {
  const double stay = r.minimum_binds && std::isfinite(l.minimum_nights) ? std::max(r.stay, l.minimum_nights) : r.stay;
  return std::min(r.cap * 365, l.reviews / r.review_rate * stay);
}

inline Result assess(const std::vector<Listing>& ls, const Reading& r) {
  detail::require(!ls.empty(), "a city has listings");
  detail::require(r.review_rate > 0 && r.review_rate <= 1, "a review rate lies in (0, 1]");
  detail::require(r.stay > 0 && r.cap > 0 && r.cap <= 1 && r.withdrawn_nights >= 0 && r.withdrawn_host >= 0, "a stay, a cap in (0, 1] and thresholds that are not negative");
  Result out;
  out.reading = r.name;
  std::map<std::string, double> hosts;
  for (const auto& l : ls) {
    detail::require(l.reviews >= 0, "reviews are not negative");
    const double n = booked(l, r);
    out.nights.push_back(n);
    const bool entire = l.type == "entire";
    const bool withdrawn = entire && (n > r.withdrawn_nights || (r.withdrawn_host > 0 && l.host_listings >= r.withdrawn_host));
    out.listings += 1, out.entire += entire, out.nights_total += n, out.withdrawn += withdrawn;
    if (std::isfinite(l.price) && l.price > 0) out.revenue += n * l.price, out.priced_nights += n;
    Area& a = out.areas[l.area];
    a.listings += 1, a.entire += entire, a.nights += n, a.withdrawn += withdrawn;
    if (std::isfinite(l.price) && l.price > 0) a.revenue += n * l.price;
    hosts[l.host] += 1;
    if (l.host_listings > 1) out.multi_host_share += 1;
  }
  out.multi_host_share /= out.listings;
  std::vector<double> shares;
  for (const auto& [h, k] : hosts) shares.push_back(k / out.listings);
  std::sort(shares.rbegin(), shares.rend());
  for (std::size_t i = 0; i < shares.size(); i++) {
    out.hhi += shares[i] * shares[i];
    if (i < 10) out.top10 += shares[i];
  }
  out.largest_host = shares.empty() ? 0 : shares[0];
  return out;
}

}  // namespace urban::rentals
