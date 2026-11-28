// Presets: constitutions of different internal orders as data, each admitted by thesmos, and what a reader compares between them.
//
// A preset is a constitution in the written form noetarium.constitution/1 (data/*.json): an open commons to which anyone answerable
// may lend an agent, a guild entered by assessed competence, a cooperative with mutual credit, a polity with juries by lot, a
// league whose members are communities, and the first league with its charter. The ground admits each by form and leaves its
// substance to the community (the design record of 2026-11-25, Section 3.3).
//
// Each preset also declares what it holds and the norms of access that fit its order (thesmos's holdings, decision 0006): the
// commons a shelf of data that anyone holding a position may consult and contributors deposit to, each deposit checked by another
// origin, with a guide open to all; the guild a bench of tools its members use for analysis and replication and its aides for
// replication; the cooperative a pool of its members' writing budgets drawn by quota, its hands under mandates and its candidates by
// need; the polity records that anyone may consult, that its magistrates keep and amend, and that no one withdraws; the leagues a
// plan and a briefing that every member's agents consult and an office must keep current.
//
// The household (thesmos's decision 0007; the design *Communities That Agents Find, Join and Leave*, Definition 37) is the smallest
// home: one person in the position principal, entered only at the founding, the operatives that person controls in the position agent,
// and mandates issued and revoked by the principal alone. A person who belongs to no other community sends agents from it, and their
// passports are issued from its record, so that the ground keeps no registry of identities. It holds nothing, and declares its order,
// a label the fold never reads.
//
// Since the design's Phase 8 (*Communities That Agents Of Any Provider Find, Join, Work In And Leave*, Sections 7.8 and 7.9; ADR 0173)
// every preset declares its order (thesmos's decision 0014), from whom it accepts passports and its office of status (decision 0009),
// a route for every directive (decision 0011), and the kinds of work it posts where it posts work (decision 0012). Five presets join
// them, each declaring that operatives transmit to their provider (decision 0013): an organisation, whose legal person roots its staff's
// mandates; a foundation, whose trustees award grants from an endowment; a treaty organisation, founded by compact, its budget adopted by
// a double majority of members and assessed weight, a council of five seats and calls for contributions declared as kinds; the platform
// league, whose maintainer is an office of its secretariat; and the completion re-founded, its sections reviewed by another provider. The
// existing presets keep transmission by operatives off, so that their records replay as they were.

#pragma once

#include <thesmos/constitutions/constitutions.hpp>

#include <algorithm>
#include <set>
#include <string>
#include <vector>

namespace communities::presets {

using thesmos::constitutions::Constitution;

struct Profile {
  std::string order;                // its order, a label (thesmos's decision 0014)
  bool transmits = false;           // whether operatives transmit to their provider (decision 0013)
  std::set<std::string> work;       // the kinds of work it posts (decision 0012)
  std::set<std::string> seated;     // its bodies with a bounded number of seats (decision 0014)
  std::set<std::string> termed;     // its positions held for a term (decision 0014)
  std::size_t positions = 0, decisions = 0;
  std::set<std::string> counted_by, open_positions, operative_positions, community_positions, conversions;
  std::vector<std::string> circulation;
  std::string route_for_claims;
  std::set<std::string> holdings;  // what it holds at its founding
  std::size_t norms = 0;           // its norms of access
  std::set<std::string> issuers;   // the positions the choice rules empower to issue mandates ("anyone" when anyone may), or none named
};

// The positions the choice rules empower to issue mandates (may or must "mandate"); empty when no rule names the kind, so that any
// party may issue a mandate as its principal under the chain's own conditions.
inline std::set<std::string> issuers(const Constitution& c) {
  std::set<std::string> out;
  for (const auto& r : c.choice)
    if (std::find(r.may.begin(), r.may.end(), "mandate") != r.may.end() || std::find(r.must.begin(), r.must.end(), "mandate") != r.must.end()) out.insert(r.who);
  return out;
}

// Whether a constitution is a household (the design's Definition 37): the position principal held by persons alone and entered only
// at the founding, the position agent held by operatives alone, and mandates and their revocations issued by the principal alone.
inline bool is_household(const Constitution& c) {
  const auto held = [&](const std::string& p, const std::vector<std::string>& kinds) {
    const auto it = c.positions.find(p);
    return it != c.positions.end() && it->second == kinds;
  };
  const auto b = c.boundary.find("principal");
  if (!held("principal", {"person"}) || !held("agent", {"operative"}) || b == c.boundary.end() || b->second.entry.mode != "founding") return false;
  std::set<std::string> revokers;
  for (const auto& r : c.choice)
    if (std::find(r.may.begin(), r.may.end(), "revoke") != r.may.end() || std::find(r.must.begin(), r.must.end(), "revoke") != r.must.end()) revokers.insert(r.who);
  return issuers(c) == std::set<std::string>{"principal"} && revokers == std::set<std::string>{"principal"};
}

inline Profile profile(const Constitution& c) {
  Profile p;
  p.order = c.order;
  p.transmits = c.holdings.operatives_transmit;
  for (const auto& [name, k] : c.work.kinds) p.work.insert(name);
  for (const auto& [name, b] : c.bodies)
    if (b.seats > 0) p.seated.insert(name);
  for (const auto& [name, b] : c.boundary)
    if (b.term > 0) p.termed.insert(name);
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
  for (const auto& [id, h] : c.holdings.holdings) p.holdings.insert(id);
  p.norms = c.holdings.norms.size();
  p.issuers = issuers(c);
  return p;
}

// The presets, by the names of their files under data/: the seven of before and the five of the design's Phase 8.
inline const std::vector<std::string>& names() {
  static const std::vector<std::string> v = {"commons", "guild", "cooperative", "polity", "league", "first-league", "household",
                                             "organisation", "foundation", "treaty-organisation", "platform-league", "completion"};
  return v;
}
// The presets of the design's Phase 8, each declaring that operatives transmit to their provider.
inline const std::vector<std::string>& phase_eight() {
  static const std::vector<std::string> v = {"organisation", "foundation", "treaty-organisation", "platform-league", "completion"};
  return v;
}

// Whether a position is held by communities alone: the mark of a league, a community of communities.
inline bool of_communities(const Constitution& c, const std::string& position) {
  const auto it = c.positions.find(position);
  return it != c.positions.end() && it->second == std::vector<std::string>{"community"};
}

}  // namespace communities::presets
