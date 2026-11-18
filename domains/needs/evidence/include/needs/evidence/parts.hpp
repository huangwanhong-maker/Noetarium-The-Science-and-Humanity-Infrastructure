// The three parts of the data (ADR 0125; the theory note §5): discovery, which the search sees; validation, which it may consult
// only through a reusable holdout; confirmation, used once for each statement admitted and never fed back.
//
// The parts are made of whole clusters (the units the data say are drawn or resampled together: an area, a household), so that
// no cluster lends itself to two parts. A cluster's part is decided by a keyed hash of its name, not by its data, so that the split
// is independent of the outcomes and the same on every run with the same key (Cox 1975; Rinaldo, Wasserman and G'Sell 2019).
// A dataset may instead declare a column of parts, with the levels discovery, validation and confirmation.
//
// Rows without a cluster are clusters of one.
#pragma once

#include <needs/language/statement.hpp>

#include <array>
#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace needs::evidence {

using language::Dataset;
using language::Refusal;

enum class Part { discovery = 0, validation = 1, confirmation = 2 };

inline const char* name(Part p) {
  switch (p) {
    case Part::discovery: return "discovery";
    case Part::validation: return "validation";
    default: return "confirmation";
  }
}

struct Parts {
  std::vector<int> part_of_row;              // 0, 1, 2
  std::vector<int> cluster_of_row;           // index into clusters
  std::vector<std::string> clusters;         // names
  std::vector<int> part_of_cluster;
  std::array<std::vector<int>, 3> members;   // the clusters of each part, in order
  std::array<std::size_t, 3> rows{0, 0, 0};
  std::array<double, 3> fractions{0.5, 0.25, 0.25};
  std::uint64_t key = 0;
  bool declared = false;                     // read from the dataset's column

  // The position of a cluster within its part (for the per-part arrays of totals).
  std::vector<int> index_in_part;
};

namespace detail {
// FNV-1a over the name, then splitmix64 with the key: a uniform number in [0, 1) that the data cannot influence.
inline double keyed_uniform(const std::string& name, std::uint64_t key) {
  std::uint64_t h = 1469598103934665603ull;
  for (unsigned char c : name) h ^= c, h *= 1099511628211ull;
  std::uint64_t z = h ^ (key + 0x9e3779b97f4a7c15ull);
  z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ull;
  z = (z ^ (z >> 27)) * 0x94d049bb133111ebull;
  z ^= z >> 31;
  return static_cast<double>(z >> 11) * 0x1.0p-53;
}
}  // namespace detail

inline Parts split(const Dataset& d, std::array<double, 3> fractions = {0.5, 0.25, 0.25}, std::uint64_t key = 20261118) {
  const auto& t = d.table;
  double total = fractions[0] + fractions[1] + fractions[2];
  if (!(fractions[0] > 0) || fractions[1] < 0 || fractions[2] < 0 || !(total > 0)) throw Refusal("the parts' fractions are not a split: discovery takes a positive share");
  for (double& f : fractions) f /= total;
  Parts p;
  p.fractions = fractions, p.key = key;
  p.part_of_row.assign(t.rows, 0);
  p.cluster_of_row.assign(t.rows, -1);
  const groups::Column* cl = d.cluster.empty() ? nullptr : &t.at(d.cluster);
  if (cl && cl->kind != groups::Kind::category) throw Refusal("the clusters " + d.cluster + " are a column of categories");
  std::map<std::string, int> id;
  for (std::size_t i = 0; i < t.rows; i++) {
    std::string nm = cl && cl->codes[i] >= 0 ? cl->levels[static_cast<std::size_t>(cl->codes[i])] : "row " + std::to_string(i);
    auto it = id.find(nm);
    if (it == id.end()) it = id.emplace(nm, static_cast<int>(p.clusters.size())).first, p.clusters.push_back(nm);
    p.cluster_of_row[i] = it->second;
  }
  p.part_of_cluster.assign(p.clusters.size(), -1);
  const groups::Column* pc = d.part.empty() ? nullptr : &t.at(d.part);
  if (pc) {
    p.declared = true;
    for (std::size_t i = 0; i < t.rows; i++) {
      const std::string lv = pc->codes[i] >= 0 ? pc->levels[static_cast<std::size_t>(pc->codes[i])] : "";
      const int part = lv == "discovery" ? 0 : lv == "validation" ? 1 : lv == "confirmation" ? 2 : -1;
      if (part < 0) throw Refusal("row " + std::to_string(i + 1) + " is in no part (the levels are discovery, validation, confirmation)");
      int& c = p.part_of_cluster[static_cast<std::size_t>(p.cluster_of_row[i])];
      if (c >= 0 && c != part) throw Refusal("the cluster " + p.clusters[static_cast<std::size_t>(p.cluster_of_row[i])] + " lies in two parts");
      c = part;
    }
  } else {
    for (std::size_t c = 0; c < p.clusters.size(); c++) {
      const double u = detail::keyed_uniform(p.clusters[c], key);
      p.part_of_cluster[c] = u < fractions[0] ? 0 : u < fractions[0] + fractions[1] ? 1 : 2;
    }
  }
  p.index_in_part.assign(p.clusters.size(), -1);
  for (std::size_t c = 0; c < p.clusters.size(); c++) {
    const int part = p.part_of_cluster[c];
    p.index_in_part[c] = static_cast<int>(p.members[static_cast<std::size_t>(part)].size());
    p.members[static_cast<std::size_t>(part)].push_back(static_cast<int>(c));
  }
  for (std::size_t i = 0; i < t.rows; i++) {
    p.part_of_row[i] = p.part_of_cluster[static_cast<std::size_t>(p.cluster_of_row[i])];
    p.rows[static_cast<std::size_t>(p.part_of_row[i])]++;
  }
  return p;
}

}  // namespace needs::evidence
