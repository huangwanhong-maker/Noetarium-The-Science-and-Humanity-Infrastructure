// Individuals named at the ranks of a classification, and what the classification tells of a collection.
//
// An individual (a tree in a street, a bird in a count) carries its names at three ranks, species ⊂ genus ⊂ family; a name
// may be unknown at a rank (a tree recorded as "Eucalyptus sp." has a genus and a family but no species). A tally at a rank
// counts the individuals of each name; those unknown at that rank are counted apart and are not a class.
//
// Taxonomic distinctness measures how far apart in the classification the members of a collection are, not how many
// kinds it holds (Warwick and Clarke 1995, Mar. Ecol. Prog. Ser. 129: 301–305; Clarke and Warwick 1998, J. Appl. Ecol.
// 35: 523–531). With L levels above the species (here genus, family and a common root, L = 3), the weight ω_ij of a pair
// of species is the number of steps to their lowest common rank, scaled so that the greatest is 100:
//
//   ω = 100/3 in one genus,  200/3 in one family,  100 in different families.
//
//   average distinctness   Δ⁺ = Σ_{i<j} ω_ij / (s(s − 1)/2)            over the s species present (Clarke and Warwick 1998, eq. 3)
//   its variation          Λ⁺ = Σ_{i<j} (ω_ij − Δ⁺)² / (s(s − 1)/2)    (Clarke and Warwick 2001, Mar. Ecol. Prog. Ser. 216: 265–278, eq. 2)
//   distinctness           Δ* = Σ_{i<j} ω_ij x_i x_j / Σ_{i<j} x_i x_j  for abundances x (Warwick and Clarke 1995, eq. 3)
//
// Δ⁺ of a collection drawn at random from a master list of S species has the master list's Δ⁺ as its expectation, whatever
// the number s drawn, since every pair is equally likely to be drawn (Clarke and Warwick 1998, §2); the spread about it
// narrows as s grows, which draws a funnel against which a collection is judged. The funnel here is by simulation, drawing
// s species without replacement, reproducible by its seed, and states its number of draws.
#pragma once

#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::taxonomy {

enum class Rank { species = 0, genus = 1, family = 2 };

inline const char* rank_name(Rank r) {
  switch (r) {
    case Rank::species: return "species";
    case Rank::genus: return "genus";
    case Rank::family: return "family";
  }
  return "?";
}

struct Taxon {
  std::string species, genus, family;  // empty where unknown
  const std::string& at(Rank r) const { return r == Rank::species ? species : r == Rank::genus ? genus : family; }
};

struct Tally {
  Rank rank = Rank::species;
  std::vector<std::string> names;  // in decreasing count (ties by name)
  std::vector<double> counts;
  double known = 0;                // Σ counts
  double unknown = 0;              // individuals with no name at this rank
  double share(std::size_t k) const { return known > 0 ? counts[k] / known : 0.0; }
};

// The tally of the individuals at a rank; `weights` (one per individual, or empty for ones) counts an individual several times.
inline Tally tally(const std::vector<Taxon>& individuals, Rank r, const std::vector<double>& weights = {}) {
  if (!weights.empty() && weights.size() != individuals.size()) throw std::invalid_argument("ecology::taxonomy::tally: one weight per individual");
  std::map<std::string, double> by;
  Tally t;
  t.rank = r;
  for (std::size_t i = 0; i < individuals.size(); i++) {
    const double w = weights.empty() ? 1.0 : weights[i];
    const std::string& name = individuals[i].at(r);
    if (name.empty()) t.unknown += w;
    else by[name] += w;
  }
  std::vector<std::pair<std::string, double>> rows(by.begin(), by.end());
  std::stable_sort(rows.begin(), rows.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
  for (const auto& [name, c] : rows) {
    t.names.push_back(name);
    t.counts.push_back(c);
    t.known += c;
  }
  return t;
}

// The species present, each with its genus and family as the individuals name them (the first naming wins; a species
// given two genera is refused, since the classification must be a tree).
struct SpeciesList {
  std::vector<std::string> species, genus, family;
  std::vector<double> abundance;
};

inline SpeciesList species_list(const std::vector<Taxon>& individuals) {
  std::map<std::string, std::size_t> index;
  SpeciesList L;
  for (const auto& t : individuals) {
    if (t.species.empty()) continue;
    auto it = index.find(t.species);
    if (it == index.end()) {
      index[t.species] = L.species.size();
      L.species.push_back(t.species);
      L.genus.push_back(t.genus);
      L.family.push_back(t.family);
      L.abundance.push_back(1);
    } else {
      const std::size_t k = it->second;
      if (!t.genus.empty() && !L.genus[k].empty() && t.genus != L.genus[k])
        throw std::invalid_argument("ecology::taxonomy::species_list: the species " + t.species + " is given two genera");
      if (L.genus[k].empty()) L.genus[k] = t.genus;
      if (L.family[k].empty()) L.family[k] = t.family;
      L.abundance[k] += 1;
    }
  }
  return L;
}

// ω between species i and j of a list: 100/3 in one genus, 200/3 in one family, 100 otherwise. A rank unknown for either
// species counts as different there (the cautious reading: it does not make them closer).
inline double path_weight(const SpeciesList& L, std::size_t i, std::size_t j) {
  if (i == j) return 0;
  if (!L.genus[i].empty() && L.genus[i] == L.genus[j]) return 100.0 / 3;
  if (!L.family[i].empty() && L.family[i] == L.family[j]) return 200.0 / 3;
  return 100;
}

struct Distinctness {
  std::size_t species = 0;
  double average = 0;    // Δ⁺
  double variation = 0;  // Λ⁺
  double weighted = 0;   // Δ*, with the list's abundances
};

inline Distinctness distinctness(const SpeciesList& L) {
  Distinctness d;
  const std::size_t s = L.species.size();
  d.species = s;
  if (s < 2) return d;
  const double pairs = static_cast<double>(s) * static_cast<double>(s - 1) / 2;
  double sum = 0, wsum = 0, xsum = 0;
  for (std::size_t i = 0; i < s; i++)
    for (std::size_t j = i + 1; j < s; j++) {
      const double w = path_weight(L, i, j);
      sum += w;
      wsum += w * L.abundance[i] * L.abundance[j];
      xsum += L.abundance[i] * L.abundance[j];
    }
  d.average = sum / pairs;
  double var = 0;
  for (std::size_t i = 0; i < s; i++)
    for (std::size_t j = i + 1; j < s; j++) var += std::pow(path_weight(L, i, j) - d.average, 2);
  d.variation = var / pairs;
  d.weighted = xsum > 0 ? wsum / xsum : 0;
  return d;
}

// The funnel: Δ⁺ of s species drawn at random (without replacement) from a master list, at each s asked, as the mean and the
// 2.5% and 97.5% quantiles over `draws` draws.
struct FunnelPoint {
  std::size_t species = 0;
  double mean = 0, lower = 0, upper = 0;
};

inline std::vector<FunnelPoint> funnel(const SpeciesList& master, const std::vector<std::size_t>& sizes, std::size_t draws = 1000, std::uint64_t seed = 1) {
  const std::size_t S = master.species.size();
  calx::random::Stream rng(seed, 0, 0, 0);
  std::vector<FunnelPoint> out;
  for (std::size_t s : sizes) {
    if (s < 2 || s > S) throw std::invalid_argument("ecology::taxonomy::funnel: sizes between 2 and the master list's length");
    std::vector<double> values;
    values.reserve(draws);
    std::vector<std::size_t> idx(S);
    for (std::size_t k = 0; k < draws; k++) {
      for (std::size_t i = 0; i < S; i++) idx[i] = i;
      for (std::size_t i = 0; i < s; i++) std::swap(idx[i], idx[i + rng.below(S - i)]);  // a partial Fisher–Yates shuffle
      double sum = 0;
      for (std::size_t a = 0; a < s; a++)
        for (std::size_t b = a + 1; b < s; b++) sum += path_weight(master, idx[a], idx[b]);
      values.push_back(sum / (static_cast<double>(s) * static_cast<double>(s - 1) / 2));
    }
    std::sort(values.begin(), values.end());
    FunnelPoint p;
    p.species = s;
    double m = 0;
    for (double v : values) m += v;
    p.mean = m / static_cast<double>(draws);
    const auto q = [&](double u) { return values[std::min(values.size() - 1, static_cast<std::size_t>(u * static_cast<double>(values.size())))]; };
    p.lower = q(0.025);
    p.upper = q(0.975);
    out.push_back(p);
  }
  return out;
}

}  // namespace ecology::taxonomy
