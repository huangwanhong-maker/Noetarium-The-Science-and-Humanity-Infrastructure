// A forest as an inventory of trees: its composition by rank against declared limits, its size structure, the diameter
// increments one inventory can tell, and its stock of carbon.
//
// A tree here is one record of an inventory: its taxon, its diameter at breast height, its place in a local frame (metres),
// the area it stands in (a block, a precinct, a park; given by whoever composes the inventory), its age where the planting
// year is known, and the remaining useful life an arborist assessed, where stated, as an interval of years.
//
// Composition limits. Santamour (1990, "Trees for urban planting: diversity, uniformity, and common sense", Proc. 7th Conf.
// Metropolitan Tree Improvement Alliance 7: 57–65) proposed that no species exceed 10% of a city's trees, no genus 20% and
// no family 30%, so that one pest or disease cannot take a large part of them (as Dutch elm disease took the elms of North
// American streets). Other limits have been proposed since; a limit here is a declared scheme, found rank by rank, and
// none is the domain's.
//
// Increments from one inventory. Where a tree's planting year is known, D/age is its mean annual increment since planting
// (including the diameter it was planted at). The median of D/age over the trees of a genus, within an age range, is a
// presumption of that genus's increment, not a measured growth rate: growth slows with size and differs with site, and a
// repeated inventory would measure what one inventory can only suggest (Pretzsch 2009, Forest Dynamics, Growth and Yield,
// ch. 9, on growth from single surveys).
#pragma once

#include <ecology/forest/allometry.hpp>
#include <ecology/taxonomy/taxonomy.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace ecology::forest {

struct Tree {
  Taxon taxon;
  double dbh = NAN;  // cm
  double x = 0, y = 0;
  int area = -1;
  double age = NAN;                          // years since planting
  double life_lower = NAN, life_upper = NAN;  // remaining useful life, years
};

struct Composition {
  taxonomy::Tally species, genus, family;
  const taxonomy::Tally& at(Rank r) const { return r == Rank::species ? species : r == Rank::genus ? genus : family; }
};

// The composition of the trees standing in `area` (every tree when area < 0).
inline Composition composition(const std::vector<Tree>& trees, int area = -1) {
  std::vector<Taxon> taxa;
  for (const auto& t : trees)
    if (area < 0 || t.area == area) taxa.push_back(t.taxon);
  return {taxonomy::tally(taxa, Rank::species), taxonomy::tally(taxa, Rank::genus), taxonomy::tally(taxa, Rank::family)};
}

struct Limits {
  std::string name = "Santamour (1990): 10% of a species, 20% of a genus, 30% of a family";
  double species = 0.10, genus = 0.20, family = 0.30;
  double at(Rank r) const { return r == Rank::species ? species : r == Rank::genus ? genus : family; }
};

struct Finding {
  Rank rank = Rank::species;
  double limit = 0;
  std::string largest;    // the most common name at the rank
  double share = 0;       // its share of the trees named at the rank
  bool holds = true;
  std::vector<std::pair<std::string, double>> over;  // every name above the limit, with its share
  double unnamed = 0;     // trees not named at this rank (not counted in the shares)
};

inline std::vector<Finding> findings(const Composition& c, const Limits& limits) {
  std::vector<Finding> out;
  for (Rank r : {Rank::species, Rank::genus, Rank::family}) {
    const auto& t = c.at(r);
    Finding f;
    f.rank = r;
    f.limit = limits.at(r);
    f.unnamed = t.unknown;
    if (!t.names.empty()) {
      f.largest = t.names[0];
      f.share = t.share(0);
      for (std::size_t k = 0; k < t.names.size(); k++)
        if (t.share(k) > f.limit) f.over.emplace_back(t.names[k], t.share(k));
    }
    f.holds = f.over.empty();
    out.push_back(f);
  }
  return out;
}

// Counts of trees in diameter classes [b_0, b_1), [b_1, b_2), …, [b_{k−1}, ∞).
inline std::vector<double> size_structure(const std::vector<Tree>& trees, const std::vector<double>& bounds, int area = -1) {
  std::vector<double> n(bounds.size(), 0.0);
  for (const auto& t : trees) {
    if (!(t.dbh > 0) || (area >= 0 && t.area != area)) continue;
    std::size_t k = 0;
    while (k + 1 < bounds.size() && t.dbh >= bounds[k + 1]) k++;
    if (t.dbh >= bounds[0]) n[k] += 1;
  }
  return n;
}

struct Increments {
  std::map<std::string, double> by_genus;  // cm per year, genera with at least `least` aged trees
  std::map<std::string, std::size_t> counts;
  double overall = NAN;
  std::size_t used = 0;
  double of(const Taxon& t) const {
    if (auto it = by_genus.find(t.genus); it != by_genus.end()) return it->second;
    return overall;
  }
};

inline double median(std::vector<double> v) {
  if (v.empty()) return NAN;
  std::sort(v.begin(), v.end());
  const std::size_t n = v.size();
  return n % 2 ? v[n / 2] : (v[n / 2 - 1] + v[n / 2]) / 2;
}

inline Increments increments_from_ages(const std::vector<Tree>& trees, double age_min = 3, double age_max = 80, std::size_t least = 10) {
  std::map<std::string, std::vector<double>> by;
  std::vector<double> all;
  for (const auto& t : trees) {
    if (!(t.dbh > 0) || !(t.age >= age_min && t.age <= age_max)) continue;
    const double r = t.dbh / t.age;
    all.push_back(r);
    if (!t.taxon.genus.empty()) by[t.taxon.genus].push_back(r);
  }
  Increments inc;
  inc.used = all.size();
  inc.overall = median(all);
  for (auto& [g, v] : by)
    if (v.size() >= least) {
      inc.counts[g] = v.size();
      inc.by_genus[g] = median(v);
    }
  return inc;
}

struct Stock {
  double trees = 0, estimated = 0;   // trees with a diameter; of them, estimated
  double biomass = 0, carbon = 0, co2e = 0;  // kg
  double sequestration = 0;          // kg of carbon per year, where an increment is known
  double matched[4] = {0, 0, 0, 0};  // trees matched at species, genus, family, fallback
  double outside_range = 0;          // trees outside their equations' range of D
  double crown_area = 0;             // m², Σ of crown areas where the scheme states crowns (overlaps not removed)
};

inline Stock stock(const std::vector<Tree>& trees, const Scheme& scheme, const Increments* increments = nullptr, int area = -1) {
  Stock s;
  for (const auto& t : trees) {
    if (area >= 0 && t.area != area) continue;
    if (!(t.dbh > 0)) continue;
    s.trees += 1;
    const auto e = estimate(scheme, t.taxon, t.dbh);
    s.matched[static_cast<int>(e.assignment.match)] += 1;
    if (e.outside_range) s.outside_range += 1;
    if (std::isfinite(e.crown_area)) s.crown_area += e.crown_area;
    if (!std::isfinite(e.carbon)) continue;
    s.estimated += 1;
    s.biomass += e.biomass;
    s.carbon += e.carbon;
    s.co2e += e.co2e;
    if (increments) {
      double inc = scheme.groups[e.assignment.group].increment;
      if (!std::isfinite(inc)) inc = increments->of(t.taxon);
      if (std::isfinite(inc)) s.sequestration += sequestration(scheme, t.taxon, t.dbh, inc);
    }
  }
  return s;
}

}  // namespace ecology::forest
