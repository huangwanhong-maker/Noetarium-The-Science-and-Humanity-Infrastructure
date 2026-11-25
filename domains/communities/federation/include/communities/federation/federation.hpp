// Federation: how communities stand to one another. Recognition is directed and comes in levels; a compact binds the communities
// that ratified it, each by its own constitution; a notary witnesses the heads of members' chronicles and finds a history rewritten;
// a league is a community whose members are communities, run on thesmos's fold like any other (the design record of 2026-11-25,
// Sections 4.5 and 8).

#pragma once

#include <thesmos/crossings/crossings.hpp>

#include <map>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace communities::federation {

// ---- Recognition ----------------------------------------------------------------------------------------------------------------

// The levels of recognition, each including those before it: that the other exists; that its attestations count as evidence; that
// its envoys' undertakings bind it; that its decisions are honoured.
enum class Level { none = 0, exists = 1, evidence = 2, undertakings = 3, decisions = 4 };

inline std::string_view word(Level l) {
  switch (l) {
    case Level::none: return "none";
    case Level::exists: return "exists";
    case Level::evidence: return "its attestations count as evidence";
    case Level::undertakings: return "its envoys' undertakings bind it";
    case Level::decisions: return "its decisions are honoured";
  }
  return "?";
}

// Recognitions as made, the latest of each direction in force. Recognition is directed and stops at the party recognised: each
// direction and each further party takes an act of its own.
class Recognitions {
 public:
  void recognise(const std::string& by, const std::string& of, Level level) { level_[{by, of}] = level; }
  void withdraw(const std::string& by, const std::string& of) { level_[{by, of}] = Level::none; }
  Level level(const std::string& by, const std::string& of) const {
    const auto it = level_.find({by, of});
    return it == level_.end() ? Level::none : it->second;
  }
  bool recognises(const std::string& by, const std::string& of, Level at_least) const { return static_cast<int>(level(by, of)) >= static_cast<int>(at_least); }

 private:
  std::map<std::pair<std::string, std::string>, Level> level_;
};

// ---- Compacts ---------------------------------------------------------------------------------------------------------------------

struct Compact {
  std::string id;
  std::vector<std::string> parties;  // the communities it would bind
};

struct Standing {
  std::vector<std::string> ratified, pending;
  bool in_force() const { return pending.empty() && !ratified.empty(); }
};

// Which parties have ratified a compact, read from each party's own chronicle: a compact is ratified there when an amendment of the
// community, decided by its own rule, adopted it.
inline Standing standing(const Compact& c, const std::map<std::string, const thesmos::chronicle::State*>& chronicles) {
  Standing s;
  for (const auto& party : c.parties) {
    const auto it = chronicles.find(party);
    const bool yes = it != chronicles.end() && std::find(it->second->rules.compacts.begin(), it->second->rules.compacts.end(), c.id) != it->second->rules.compacts.end();
    (yes ? s.ratified : s.pending).push_back(party);
  }
  return s;
}

// ---- Notaries -----------------------------------------------------------------------------------------------------------------------

// A notary keeps the last head it witnessed of each community; a new head is witnessed only with a proof that the last is its
// beginning, so that a community that rewrote what it had shown is found.
class Notary {
 public:
  explicit Notary(std::string name) : name_(std::move(name)) {}
  apodeixis::kernel::Verdict witness(const thesmos::crossings::Head& now, const std::vector<archeion::fixity::Digest>& proof) {
    const auto it = last_.find(now.community);
    if (it == last_.end()) {
      last_[now.community] = now;
      return apodeixis::kernel::Verdict::yes(name_ + " witnesses the first head of " + now.community);
    }
    auto v = thesmos::crossings::consistent(it->second, now, proof);
    if (v.holds()) it->second = now;
    return v;
  }
  const thesmos::crossings::Head* last(const std::string& community) const {
    const auto it = last_.find(community);
    return it == last_.end() ? nullptr : &it->second;
  }

 private:
  std::string name_;
  std::map<std::string, thesmos::crossings::Head> last_;
};

}  // namespace communities::federation
