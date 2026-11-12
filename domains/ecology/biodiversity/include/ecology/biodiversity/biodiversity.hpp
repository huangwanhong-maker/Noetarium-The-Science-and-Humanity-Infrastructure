// The diversity of living things in an assemblage and among its areas, at the ranks of their classification, on tyche's classes.
//
// An assemblage is the individuals counted (the trees of a city, the birds of a count), each named at species, genus and family
// (taxonomy.hpp). At a rank it is a sample of counts of names, and what tyche's classes estimate of any classes is estimated of
// it: the Hill numbers ^qD of orders 0, 1 and 2 (richness, the exponential of Shannon's entropy, the inverse of Simpson's
// concentration: the effective numbers of kinds; Hill 1973, Ecology 54: 427–432), as observed and as estimated for the whole
// from which the sample came (Chao1; Chao, Wang and Jost 2013; Chao et al. 2014), and the sample's coverage, the share of the
// individuals of the whole that belong to the kinds the sample holds (Chao and Jost 2012, Ecology 93: 2533–2547).
//
// Areas are compared at one coverage, not at one size: two areas sampled to the same completeness, each rarefied or extrapolated
// to the size at which its coverage reaches the common target (Chao and Jost 2012). Comparing raw richness favours the areas
// counted most. The partition of the whole's diversity among its areas, γ = α × β with equal weights (Jost 2007, Ecology 88:
// 2427–2439), says how many distinct communities the areas amount to (β between 1 and the number of areas), and the overlaps
// how much of their kinds they share (Chao, Chiu and Jost 2014). With equal weights the partition compares compositions alone;
// weighting by size would mix how many trees an area holds into how different it is (tyche's partition.hpp says why).
#pragma once

#include <ecology/taxonomy/taxonomy.hpp>

#include <tyche/classes/asymptotic.hpp>
#include <tyche/classes/counts.hpp>
#include <tyche/classes/partition.hpp>
#include <tyche/classes/rarefaction.hpp>
#include <tyche/classes/richness.hpp>

#include <calx/linalg/rectangular.hpp>

#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace ecology::biodiversity {

using taxonomy::Rank;
using taxonomy::Taxon;

struct Diversity {
  Rank rank = Rank::species;
  double individuals = 0;      // named at the rank
  double unnamed = 0;          // not named at the rank (not counted)
  double observed[3] = {0, 0, 0};
  double estimated[3] = {0, 0, 0};
  double coverage = 0;         // Ĉ_n
  tyche::classes::RichnessEstimate richness;  // Chao1 with its interval
};

// The counts of each name at a rank (the order is the tally's: most common first).
inline std::vector<double> counts_at(const std::vector<Taxon>& individuals, Rank r) {
  return taxonomy::tally(individuals, r).counts;
}

inline Diversity diversity(const std::vector<Taxon>& individuals, Rank r, double level = 0.95) {
  const auto t = taxonomy::tally(individuals, r);
  Diversity d;
  d.rank = r;
  d.individuals = t.known;
  d.unnamed = t.unknown;
  if (t.counts.empty()) return d;
  const auto s = tyche::classes::abundances(t.counts);
  const auto a = tyche::classes::asymptotic_diversity(s, level);
  for (int q = 0; q < 3; q++) d.observed[q] = a.observed[q], d.estimated[q] = a.estimate[q];
  d.coverage = a.coverage;
  d.richness = a.richness;
  return d;
}

// Diversity of orders 0, 1, 2 of each sample at a common coverage C (each rarefied or extrapolated to the size at which its
// coverage reaches C). A sample whose coverage cannot reach C, even extrapolated to twice its size, gives NaN and says so by a
// size of +∞ (estimates beyond 2n are unreliable; Chao et al. 2014, §4).
struct AtCoverage {
  double size = NAN;
  double value[3] = {NAN, NAN, NAN};
  bool reached = false;
};

inline AtCoverage at_coverage(const std::vector<double>& counts, double C) {
  AtCoverage out;
  if (counts.empty()) return out;
  const auto s = tyche::classes::abundances(counts);
  const double m = tyche::classes::size_at_coverage(s, C);
  out.size = m;
  if (!std::isfinite(m) || m > 2.0 * static_cast<double>(s.n) * (1 + 1e-9) + 1e-6 || m < 1) return out;  // within rounding of 2n
  out.reached = true;
  for (int q = 0; q < 3; q++) out.value[q] = tyche::classes::estimate_at(s, q, m);
  return out;
}

// The coverage every sample can reach, extrapolated to at most twice its size: the least of their coverages at 2n (Chao et al.
// 2014's suggestion for a common standard), capped at the target asked.
inline double common_coverage(const std::vector<std::vector<double>>& samples, double cap = 1.0) {
  double c = cap;
  for (const auto& counts : samples) {
    if (counts.empty()) continue;
    const auto s = tyche::classes::abundances(counts);
    c = std::min(c, tyche::classes::coverage_at(s, 2.0 * static_cast<double>(s.n)) - 1e-12);
  }
  return c;
}

// γ, α, β and the overlaps among areas at a rank and an order, with equal weights.
inline tyche::classes::Partition partition(const std::vector<Taxon>& individuals, const std::vector<int>& area, std::size_t areas, Rank r, double q) {
  if (area.size() != individuals.size()) throw std::invalid_argument("ecology::biodiversity::partition: one area per individual");
  std::map<std::string, std::size_t> names;
  for (const auto& t : individuals)
    if (!t.at(r).empty()) names.emplace(t.at(r), names.size());
  std::vector<double> held(areas, 0.0);
  calx::linalg::Rectangular z(areas, names.size());
  for (std::size_t i = 0; i < individuals.size(); i++) {
    const std::string& n = individuals[i].at(r);
    if (n.empty() || area[i] < 0 || static_cast<std::size_t>(area[i]) >= areas) continue;
    z(static_cast<std::size_t>(area[i]), names.at(n)) += 1;
    held[static_cast<std::size_t>(area[i])] += 1;
  }
  // Areas with nothing named at the rank are left out of the partition.
  std::vector<std::size_t> keep;
  for (std::size_t a = 0; a < areas; a++)
    if (held[a] > 0) keep.push_back(a);
  if (keep.empty()) throw std::invalid_argument("ecology::biodiversity::partition: no area holds a named individual");
  calx::linalg::Rectangular w(keep.size(), names.size());
  for (std::size_t k = 0; k < keep.size(); k++)
    for (std::size_t j = 0; j < names.size(); j++) w(k, j) = z(keep[k], j);
  return tyche::classes::partition(w, q, tyche::classes::Weighting::equal);
}

}  // namespace ecology::biodiversity
