// Diversification: the structure of regional economies and how it changed between two dates, the second module of the economics of
// evolution (ADR 0166; theory note theory/regional-structure.md).
//
// A table holds regions' employment by industry at two dates, each region with its reference group (its country) and each industry with
// its group (its section). For each region it gives, on nomos's `structure`:
//
//   - shift-share against its reference (the reference's employment the sum of its regions in the table): national share, industrial
//     mix, competitive, and Esteban-Marquillas's split where defined;
//   - variety at each date: entropy, unrelated variety (among sections) and related variety (within them), in bits (Frenken, Van Oort
//     and Verburg 2007);
//   - diversity (the industries it holds with RCA ≥ the threshold), its complexity index at each date over all regions of the table,
//     the industries it took up and those it gave up, and the industries it does not hold that lie nearest those it does (its
//     opportunities, by relatedness density at the second date).
//
// And over the table: whether relatedness density at the first date predicts the industries regions take up by the second, as the area
// under the curve of the densities of entries against those of the candidates that did not enter (Mann and Whitney's U, tyche), and
// likewise for exits (lower density, more exit), the out-of-sample test of the principle of relatedness (Hidalgo et al. 2018).
//
// A cell withheld (NaN) at either date is left out at both, so that withholding does not appear as change, and the count of withheld
// cells is reported for each region: a declared reading, which understates a region's employment where it withholds much. The measures predict and describe; none identifies why a region diversified. Nothing
// here names a region, an industry or a country.

#pragma once

#include <nomos/measurement/structure.hpp>
#include <tyche/testing/ranks.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace economics::diversification {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("economics::diversification: " + what);
}
}  // namespace detail

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

struct Table {
  std::vector<std::string> regions, references;   // per region: its name and its reference group
  std::vector<std::string> industries, groups;    // per industry: its code and its group
  std::vector<std::vector<double>> before, after;  // regions × industries; NaN where withheld
};

struct Options {
  double threshold = 1;              // RCA at or above which a region holds an industry
  double base = 2;                   // of the logarithms of variety
  std::size_t opportunities = 3;     // per region
};

struct Region {
  std::string name, reference;
  double total_before = 0, total_after = 0;
  int withheld_before = 0, withheld_after = 0;
  double national = 0, mix = 0, competitive = 0, competitive_effect = unknown, allocation = unknown;
  nomos::measurement::Variety variety_before, variety_after;
  int diversity_before = 0, diversity_after = 0, entries = 0, exits = 0;
  double complexity_before = unknown, complexity_after = unknown;
  std::vector<std::pair<std::string, double>> opportunities;  // industry, density at the second date
};

struct Assessment {
  std::vector<Region> regions;
  std::vector<double> industry_complexity;  // at the second date, per industry (NaN where the index is not defined)
  bool complexity_before = false, complexity_after = false;
  std::string complexity_note;
  tyche::testing::RankSum entry, exit;     // density at the first date: entries against candidates not entering; stayers against exits
  std::size_t candidates = 0, entries = 0, holdings = 0, exits = 0;
};

// A date's employment with every cell withheld at either date left out at both, so that withholding does not appear as change.
inline calx::linalg::Rectangular matrix(const std::vector<std::vector<double>>& t, const std::vector<std::vector<double>>& other, std::size_t cols) {
  calx::linalg::Rectangular x(t.size(), cols);
  for (std::size_t r = 0; r < t.size(); r++)
    for (std::size_t c = 0; c < cols; c++) x(r, c) = std::isnan(t[r][c]) || std::isnan(other[r][c]) ? 0.0 : t[r][c];
  return x;
}

inline Assessment assess(const Table& T, const Options& o = {}) {
  const std::size_t R = T.regions.size(), P = T.industries.size();
  detail::require(R >= 3 && P >= 3, "at least three regions and three industries");
  detail::require(T.references.size() == R && T.groups.size() == P && T.before.size() == R && T.after.size() == R, "a reference for every region and a group for every industry");
  for (std::size_t r = 0; r < R; r++) detail::require(T.before[r].size() == P && T.after[r].size() == P, "employment for every industry");
  const auto x0 = matrix(T.before, T.after, P), x1 = matrix(T.after, T.before, P);
  const auto I0 = nomos::measurement::incidence(x0, o.threshold), I1 = nomos::measurement::incidence(x1, o.threshold);
  const auto phi0 = nomos::measurement::proximity(I0), phi1 = nomos::measurement::proximity(I1);
  const auto w0 = nomos::measurement::density(I0, phi0), w1 = nomos::measurement::density(I1, phi1);

  Assessment A;
  // Complexity on the regions and industries that hold anything.
  auto complexity = [&](const nomos::measurement::Incidence& I, std::vector<double>& region_index, std::vector<double>* industry_index) {
    std::vector<std::size_t> rows, cols;
    for (std::size_t r = 0; r < R; r++)
      if (I.diversity[r] > 0) rows.push_back(r);
    for (std::size_t p = 0; p < P; p++)
      if (I.ubiquity[p] > 0) cols.push_back(p);
    calx::linalg::Rectangular M(rows.size(), cols.size());
    for (std::size_t a = 0; a < rows.size(); a++)
      for (std::size_t b = 0; b < cols.size(); b++) M(a, b) = I.M(rows[a], cols[b]);
    region_index.assign(R, unknown);
    if (industry_index) industry_index->assign(P, unknown);
    try {
      const auto C = nomos::measurement::complexity(nomos::measurement::from_binary(M));
      for (std::size_t a = 0; a < rows.size(); a++) region_index[rows[a]] = C.regions[a];
      if (industry_index)
        for (std::size_t b = 0; b < cols.size(); b++) (*industry_index)[cols[b]] = C.activities[b];
      return true;
    } catch (const std::invalid_argument& e) {
      A.complexity_note = e.what();
      return false;
    }
  };
  std::vector<double> c0, c1;
  A.complexity_before = complexity(I0, c0, nullptr);
  A.complexity_after = complexity(I1, c1, &A.industry_complexity);

  // References: the sum of their regions.
  std::map<std::string, std::vector<double>> ref0, ref1;
  for (std::size_t r = 0; r < R; r++) {
    auto& a = ref0[T.references[r]];
    auto& b = ref1[T.references[r]];
    a.resize(P, 0.0), b.resize(P, 0.0);
    for (std::size_t p = 0; p < P; p++) a[p] += x0(r, p), b[p] += x1(r, p);
  }

  std::vector<double> dens_entry, dens_stay_out, dens_stay, dens_exit;
  for (std::size_t r = 0; r < R; r++) {
    Region G;
    G.name = T.regions[r], G.reference = T.references[r];
    std::vector<double> e0(P), e1(P);
    for (std::size_t p = 0; p < P; p++) {
      e0[p] = x0(r, p), e1[p] = x1(r, p), G.total_before += e0[p], G.total_after += e1[p];
      G.withheld_before += std::isnan(T.before[r][p]), G.withheld_after += std::isnan(T.after[r][p]);
    }
    const auto S = nomos::measurement::shift_share(e0, e1, ref0[G.reference], ref1[G.reference]);
    G.national = S.national_total, G.mix = S.mix_total, G.competitive = S.competitive_total;
    if (S.homothetic) G.competitive_effect = S.competitive_effect_total, G.allocation = S.allocation_total;
    if (G.total_before > 0) G.variety_before = nomos::measurement::variety(e0, T.groups, o.base);
    if (G.total_after > 0) G.variety_after = nomos::measurement::variety(e1, T.groups, o.base);
    G.diversity_before = static_cast<int>(I0.diversity[r]), G.diversity_after = static_cast<int>(I1.diversity[r]);
    G.complexity_before = c0[r], G.complexity_after = c1[r];
    std::vector<std::pair<std::string, double>> opp;
    for (std::size_t p = 0; p < P; p++) {
      const bool held0 = I0.M(r, p) > 0, held1 = I1.M(r, p) > 0;
      if (!held0) {
        A.candidates++;
        if (held1) G.entries++, A.entries++;
        if (!std::isnan(w0(r, p))) (held1 ? dens_entry : dens_stay_out).push_back(w0(r, p));
      } else {
        A.holdings++;
        if (!held1) G.exits++, A.exits++;
        if (!std::isnan(w0(r, p))) (held1 ? dens_stay : dens_exit).push_back(w0(r, p));
      }
      if (!held1 && !std::isnan(w1(r, p))) opp.push_back({T.industries[p], w1(r, p)});
    }
    std::sort(opp.begin(), opp.end(), [](const auto& a, const auto& b) { return a.second > b.second || (a.second == b.second && a.first < b.first); });
    if (opp.size() > o.opportunities) opp.resize(o.opportunities);
    G.opportunities = opp;
    A.regions.push_back(G);
  }
  if (!dens_entry.empty() && !dens_stay_out.empty()) A.entry = tyche::testing::mann_whitney(dens_entry, dens_stay_out);
  if (!dens_stay.empty() && !dens_exit.empty()) A.exit = tyche::testing::mann_whitney(dens_stay, dens_exit);
  return A;
}

}  // namespace economics::diversification
