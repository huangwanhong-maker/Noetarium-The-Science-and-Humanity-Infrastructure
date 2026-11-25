// Presets: constitutions of different internal orders as data, each admitted by thesmos, and what a reader compares between them.
//
// A preset is a constitution in the written form noetarium.constitution/1 (data/*.json): an open commons to which anyone answerable
// may lend an agent, a guild entered by assessed competence, a cooperative with mutual credit, a polity with juries by lot, a
// league whose members are communities, and the first league with its charter. The ground admits each by form and leaves its
// substance to the community (the design record of 2026-11-25, Section 3.3).

#pragma once

#include <thesmos/constitutions/constitutions.hpp>

#include <set>
#include <string>
#include <vector>

namespace communities::presets {

using thesmos::constitutions::Constitution;

struct Profile {
  std::size_t positions = 0, decisions = 0;
  std::set<std::string> counted_by, open_positions, operative_positions, community_positions, conversions;
  std::vector<std::string> circulation;
  std::string route_for_claims;
};

inline Profile profile(const Constitution& c) {
  Profile p;
  p.positions = c.positions.size();
  p.decisions = c.aggregation.size();
  for (const auto& [name, d] : c.aggregation) p.counted_by.insert(d.count_by);
  for (const auto& [name, b] : c.boundary)
    if (b.entry.mode == "open") p.open_positions.insert(name);
  for (const auto& [name, held] : c.positions) {
    for (const auto& k : held) {
      if (k == "operative") p.operative_positions.insert(name);
      if (k == "community") p.community_positions.insert(name);
    }
  }
  for (const auto& [name, k] : c.conversions) p.conversions.insert(name);
  p.circulation = c.circulation;
  p.route_for_claims = c.claims.route;
  return p;
}

// Whether a position is held by communities alone: the mark of a league, a community of communities.
inline bool of_communities(const Constitution& c, const std::string& position) {
  const auto it = c.positions.find(position);
  return it != c.positions.end() && it->second == std::vector<std::string>{"community"};
}

}  // namespace communities::presets
