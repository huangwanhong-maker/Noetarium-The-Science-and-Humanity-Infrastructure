// The material stock of buildings: what a city's buildings weigh, of what they are made, and the
// cohorts in which they were built (theory note §7).
//
// A building's mass of material k is its floor area times a material intensity, kg per m² of gross
// floor area, which depends on the building's region, use, structure and period of construction
// (Heeren and Fishman 2019, Sci. Data 6: 23; Fishman et al. 2024, the RASMI intensities, Sci. Data 11:
// 418). Intensities are declared in a scheme of rows, each with its bounds and its source:
//
//   a row matches a building when each of its fields (region, use, structure) is empty or equal to the
//   building's, or the building's is unknown, and the building's year lies in the row's period (any
//   period, when the year is unknown). The rows that match are grouped by their values of what is
//   unknown of the building (its structure, say, or the period when its year is unknown); within each
//   group the most specific rows are taken (each field stated that the building also knows counts one,
//   a bounded period one when its year is known). The building's intensity is the envelope of the rows
//   taken: the least lower bound, the greatest upper bound, and the mean of the central values.
//
// So a building's mass is never more precise than what is known of it: an unknown attribute widens its
// bounds over every value the scheme admits for it, rather than being guessed. A building no row
// matches is reported, never filled. The bounds of the masses are exact for the sums (non-negative
// areas times bounded intensities).

#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace industrial_ecology::buildings {

inline constexpr int unknown_year = INT_MIN;

struct Row {
  std::string region, use, structure;           // "" matches any
  int from = INT_MIN, to = INT_MAX;             // the period of construction, inclusive
  std::vector<double> lower, central, upper;    // kg per m² of floor area, per material
  std::string source;
};

struct Scheme {
  std::string name;
  std::vector<std::string> materials;
  std::vector<Row> rows;

  void validate() const {
    const std::size_t K = materials.size();
    for (const auto& r : rows) {
      if (r.lower.size() != K || r.central.size() != K || r.upper.size() != K) throw std::invalid_argument("industrial_ecology::buildings: every row has one intensity per material");
      for (std::size_t k = 0; k < K; k++)
        if (!(0 <= r.lower[k] && r.lower[k] <= r.central[k] && r.central[k] <= r.upper[k])) throw std::invalid_argument("industrial_ecology::buildings: 0 ≤ lower ≤ central ≤ upper in every row");
      if (r.from > r.to) throw std::invalid_argument("industrial_ecology::buildings: a period whose start is not after its end");
    }
  }
};

struct Building {
  std::string id, region, use, structure;  // "" where unknown
  double floor_area = 0;                   // m² of gross floor area
  int year = unknown_year;                 // of construction
};

struct Intensity {
  bool matched = false;
  std::vector<std::size_t> rows;          // the rows taken
  std::vector<double> lower, central, upper;
};

inline Intensity intensity(const Scheme& s, const Building& b) {
  auto field = [](const std::string& row, const std::string& known) { return row.empty() || known.empty() || row == known; };
  // Group by the row's values of what the building does not know; keep the most specific in each group.
  std::map<std::string, std::pair<int, std::vector<std::size_t>>> groups;
  for (std::size_t i = 0; i < s.rows.size(); i++) {
    const Row& r = s.rows[i];
    if (!field(r.region, b.region) || !field(r.use, b.use) || !field(r.structure, b.structure)) continue;
    if (b.year != unknown_year && (b.year < r.from || b.year > r.to)) continue;
    std::string key;
    if (b.region.empty()) key += r.region + '\x1f';
    if (b.use.empty()) key += r.use + '\x1f';
    if (b.structure.empty()) key += r.structure + '\x1f';
    if (b.year == unknown_year) key += std::to_string(r.from) + ':' + std::to_string(r.to);
    const int spec = (!r.region.empty() && !b.region.empty()) + (!r.use.empty() && !b.use.empty()) + (!r.structure.empty() && !b.structure.empty()) +
                     (b.year != unknown_year && (r.from != INT_MIN || r.to != INT_MAX));
    auto& g = groups.try_emplace(key, -1, std::vector<std::size_t>{}).first->second;
    if (spec > g.first) g.first = spec, g.second.clear();
    if (spec == g.first) g.second.push_back(i);
  }
  std::vector<std::size_t> taken;
  for (const auto& [key, g] : groups) taken.insert(taken.end(), g.second.begin(), g.second.end());
  std::sort(taken.begin(), taken.end());
  Intensity I;
  if (taken.empty()) return I;
  const std::size_t K = s.materials.size();
  I.matched = true, I.rows = taken;
  I.lower.assign(K, INFINITY), I.upper.assign(K, 0.0), I.central.assign(K, 0.0);
  for (std::size_t i : taken)
    for (std::size_t k = 0; k < K; k++) {
      I.lower[k] = std::min(I.lower[k], s.rows[i].lower[k]);
      I.upper[k] = std::max(I.upper[k], s.rows[i].upper[k]);
      I.central[k] += s.rows[i].central[k] / static_cast<double>(taken.size());
    }
  return I;
}

struct Totals {
  std::vector<double> lower, central, upper;  // kg per material
  double floor_area = 0;
  std::size_t buildings = 0;
};

struct Stock {
  std::vector<std::string> materials;
  std::map<std::string, Totals> by_group;     // by the group each building is given (a block, a use, a decade)
  Totals total;
  std::vector<std::string> unmatched;         // ids of buildings no row matched
  double unmatched_area = 0;
};

// The material stock of a set of buildings, grouped by `group[i]` (empty: one group "all").
inline Stock stock(const Scheme& s, const std::vector<Building>& bs, const std::vector<std::string>& group = {}) {
  s.validate();
  if (!group.empty() && group.size() != bs.size()) throw std::invalid_argument("industrial_ecology::buildings::stock: one group per building");
  const std::size_t K = s.materials.size();
  Stock st;
  st.materials = s.materials;
  auto zero = [&](Totals& t) {
    if (t.central.empty()) t.lower.assign(K, 0.0), t.central.assign(K, 0.0), t.upper.assign(K, 0.0);
  };
  zero(st.total);
  for (std::size_t i = 0; i < bs.size(); i++) {
    const Building& b = bs[i];
    if (!(b.floor_area >= 0) || !std::isfinite(b.floor_area)) throw std::invalid_argument("industrial_ecology::buildings::stock: a floor area ≥ 0: " + b.id);
    const Intensity I = intensity(s, b);
    if (!I.matched) {
      st.unmatched.push_back(b.id);
      st.unmatched_area += b.floor_area;
      continue;
    }
    Totals& g = st.by_group[group.empty() ? std::string("all") : group[i]];
    zero(g);
    for (Totals* t : {&g, &st.total}) {
      for (std::size_t k = 0; k < K; k++) {
        t->lower[k] += b.floor_area * I.lower[k];
        t->central[k] += b.floor_area * I.central[k];
        t->upper[k] += b.floor_area * I.upper[k];
      }
      t->floor_area += b.floor_area;
      t->buildings++;
    }
  }
  return st;
}

// The buildings as cohorts by age at the end of `year`: floor area by age a (built in year − a), and the
// floor-area-weighted intensity of each age, the initial stock of a dynamic stock model. Buildings of
// unknown year, or built after `year`, are left out and counted.
struct Cohorts {
  std::vector<double> floor_area;                          // by age
  std::vector<std::vector<double>> lower, central, upper;  // [age][material], kg per m²; zero where no floor area
  double left_out_area = 0;
};

inline Cohorts cohorts(const Scheme& s, const std::vector<Building>& bs, int year, std::size_t max_age) {
  s.validate();
  const std::size_t K = s.materials.size();
  Cohorts c;
  c.floor_area.assign(max_age + 1, 0.0);
  c.lower.assign(max_age + 1, std::vector<double>(K, 0.0)), c.central = c.lower, c.upper = c.lower;
  for (const auto& b : bs) {
    const Intensity I = intensity(s, b);
    if (b.year == unknown_year || b.year > year || !I.matched) {
      c.left_out_area += b.floor_area;
      continue;
    }
    const std::size_t a = std::min<std::size_t>(static_cast<std::size_t>(year - b.year), max_age);
    c.floor_area[a] += b.floor_area;
    for (std::size_t k = 0; k < K; k++) {
      c.lower[a][k] += b.floor_area * I.lower[k];
      c.central[a][k] += b.floor_area * I.central[k];
      c.upper[a][k] += b.floor_area * I.upper[k];
    }
  }
  for (std::size_t a = 0; a <= max_age; a++)
    if (c.floor_area[a] > 0)
      for (std::size_t k = 0; k < K; k++) c.lower[a][k] /= c.floor_area[a], c.central[a][k] /= c.floor_area[a], c.upper[a][k] /= c.floor_area[a];
  return c;
}

}  // namespace industrial_ecology::buildings
