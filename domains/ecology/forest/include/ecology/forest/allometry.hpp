// Allometry: what a tree's diameter at breast height says of its mass, its carbon, its height and its crown, by equations
// that a scheme declares.
//
// No equation is the domain's own. A scheme names its source, groups taxa (by species, genus or family, the most specific
// match winning, then a fallback group), and gives each group the equations its source states, each of the power form
//
//   y = c · exp(β₀ + β₁ ln D) = c e^{β₀} D^{β₁},      D the diameter at breast height in cm,
//
// which is how the national-scale estimators of Jenkins et al. (2003, For. Sci. 49: 12–35, Table 4) state above-ground dry
// biomass in kg, and how most urban equations state height and crown diameter in m; and the ratio form
//
//   r = exp(β₀ + β₁ / D)
//
// of Jenkins et al.'s component ratios (Table 6: coarse roots, foliage, stem wood and bark as fractions of above-ground
// biomass). c is a stated adjustment: for trees grown in the open, Nowak (1994, in Chicago's Urban Forest Ecosystem,
// USDA Gen. Tech. Rep. NE-186, pp. 83–94) multiplied forest-grown biomass by 0.8. Carbon is a stated fraction of dry
// biomass (0.5 in Jenkins et al. and most urban studies; 0.47 by default in IPCC 2006, vol. 4, Table 4.3), CO₂ its
// 44/12 multiple.
//
// Each estimate says which rank of the scheme matched (species, genus, family or the fallback) and whether D lay outside the
// range of the source's data, so that what is presumed is said to be. Beyond the largest diameter the source measured, a scheme
// declares whether its equations are extrapolated (the power law carried on) or capped (the tree estimated as the largest the
// source measured, a lower bound for a sound tree); neither is known to be right, and the count beyond is reported either way. The equations' own error is large: Jenkins et al.
// report residuals between −35% and +36% of the prediction (10th and 90th percentiles, Table 5) for single trees; sums
// over many trees are better than any one.
#pragma once

#include <ecology/taxonomy/taxonomy.hpp>

#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <numbers>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::forest {

using taxonomy::Rank;
using taxonomy::Taxon;

struct Power {
  double b0 = 0, b1 = 0, scale = 1;  // y = scale · exp(b0 + b1 ln D)
  double at(double d) const { return scale * std::exp(b0 + b1 * std::log(d)); }
};

struct Ratio {
  double b0 = 0, b1 = 0;  // r = exp(b0 + b1 / D)
  double at(double d) const { return std::exp(b0 + b1 / d); }
};

struct Group {
  std::string name;
  std::optional<Power> aboveground;  // kg dry mass
  std::optional<Ratio> roots;        // coarse roots / above-ground
  std::optional<Power> height;       // m
  std::optional<Power> crown;        // crown diameter, m
  double d_min = 0, d_max = std::numeric_limits<double>::infinity();  // the source's range of D, cm
  double increment = std::numeric_limits<double>::quiet_NaN();        // mean diameter increment, cm per year, where stated
};

struct Scheme {
  std::string name, source;
  double carbon_fraction = 0.5;
  double open_grown = 1.0;  // c applied to above-ground biomass (0.8 after Nowak 1994 for trees grown in the open)
  bool cap_beyond_range = false;  // a diameter beyond its group's d_max is estimated at d_max (true) or extrapolated (false)
  std::vector<Group> groups;
  std::map<std::string, std::size_t> by_species, by_genus, by_family;
  std::size_t fallback = std::numeric_limits<std::size_t>::max();  // none: a taxon matched nowhere is refused

  std::size_t add(Group g) {
    groups.push_back(std::move(g));
    return groups.size() - 1;
  }
};

enum class Match { species, genus, family, fallback };

inline const char* match_name(Match m) {
  switch (m) {
    case Match::species: return "species";
    case Match::genus: return "genus";
    case Match::family: return "family";
    case Match::fallback: return "fallback";
  }
  return "?";
}

struct Assignment {
  std::size_t group = 0;
  Match match = Match::fallback;
};

inline Assignment assign(const Scheme& s, const Taxon& t) {
  if (!t.species.empty())
    if (auto it = s.by_species.find(t.species); it != s.by_species.end()) return {it->second, Match::species};
  if (!t.genus.empty())
    if (auto it = s.by_genus.find(t.genus); it != s.by_genus.end()) return {it->second, Match::genus};
  if (!t.family.empty())
    if (auto it = s.by_family.find(t.family); it != s.by_family.end()) return {it->second, Match::family};
  if (s.fallback < s.groups.size()) return {s.fallback, Match::fallback};
  throw std::invalid_argument("ecology::forest::assign: the scheme " + s.name + " has no group for " + (t.species.empty() ? t.genus : t.species) + " and no fallback");
}

struct Estimate {
  double aboveground = NAN, roots = NAN, biomass = NAN;  // kg dry
  double carbon = NAN, co2e = NAN;                      // kg
  double height = NAN, crown_diameter = NAN, crown_area = NAN;  // m, m, m²
  bool outside_range = false;
  Assignment assignment;
};

inline Estimate estimate(const Scheme& s, const Taxon& t, double dbh_cm) {
  if (!(dbh_cm > 0)) throw std::invalid_argument("ecology::forest::estimate: a positive diameter");
  Estimate e;
  e.assignment = assign(s, t);
  const Group& g = s.groups[e.assignment.group];
  e.outside_range = dbh_cm < g.d_min || dbh_cm > g.d_max;
  if (s.cap_beyond_range && dbh_cm > g.d_max) dbh_cm = g.d_max;
  if (g.aboveground) {
    e.aboveground = s.open_grown * g.aboveground->at(dbh_cm);
    e.roots = g.roots ? e.aboveground * g.roots->at(dbh_cm) : 0.0;
    e.biomass = e.aboveground + e.roots;
    e.carbon = s.carbon_fraction * e.biomass;
    e.co2e = e.carbon * 44.0 / 12.0;
  }
  if (g.height) e.height = g.height->at(dbh_cm);
  if (g.crown) {
    e.crown_diameter = g.crown->at(dbh_cm);
    e.crown_area = std::numbers::pi * e.crown_diameter * e.crown_diameter / 4;
  }
  return e;
}

// The carbon a tree adds in a year as its diameter grows by Δ (kg per year): C(D + Δ) − C(D).
inline double sequestration(const Scheme& s, const Taxon& t, double dbh_cm, double increment_cm) {
  if (!(increment_cm >= 0)) return NAN;
  return estimate(s, t, dbh_cm + increment_cm).carbon - estimate(s, t, dbh_cm).carbon;
}

}  // namespace ecology::forest
