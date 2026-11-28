// Federation: how communities stand to one another. Recognition is directed and comes in levels; a compact binds the communities
// that ratified it, each by its own constitution; a notary witnesses the heads of members' chronicles and finds a history rewritten;
// a league is a community whose members are communities, run on thesmos's fold like any other (the design record of 2026-11-25,
// Sections 4.5 and 8).
//
// Since the design's Phase 8 (ADR 0173) recognitions too are read from each community's own chronicle, where thesmos's act `recognise`
// sets them (decision 0009), generally or for a matter; and a compact that names no parties, open to any community as a treaty
// organisation's founding treaty is, has for parties every community whose chronicle holds a ratification of it.
//
// Since thesmos's decision 0013 a compact's standing is read from each party's own chronicle by thesmos's compacts: each party's
// ratification (with the terms whose digest it ratified, its reservations and its withdrawal) kept apart, the compact entering into force
// when its rule of entry holds, binding each party until its withdrawal takes effect, and an article binding two parties unless either
// reserved it. A compact ratified by its name alone binds when every party named has ratified, as before.

#pragma once

#include <thesmos/compacts/compacts.hpp>
#include <thesmos/crossings/crossings.hpp>

#include <algorithm>
#include <map>
#include <string>
#include <string_view>
#include <tuple>
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

// A level as thesmos's acts write it ("none", "exists", "evidence", "undertakings", "decisions"); none for any other word.
inline Level level_named(const std::string& w) {
  if (w == "exists") return Level::exists;
  if (w == "evidence") return Level::evidence;
  if (w == "undertakings") return Level::undertakings;
  if (w == "decisions") return Level::decisions;
  return Level::none;
}

using Chronicles = std::map<std::string, const thesmos::chronicle::State*>;

// Recognitions as made, the latest of each direction in force, generally or for one matter. Recognition is directed and stops at the
// party recognised: each direction and each further party takes an act of its own. Since the design's Phase 8 they are read from each
// community's own chronicle (read), where the act `recognise` of thesmos's decision 0009 sets them, so that what a community recognises
// is what its record says; a level for a matter, where set, takes the place of the general one for that matter, as the fold reads it.
class Recognitions {
 public:
  void recognise(const std::string& by, const std::string& of, Level level) { level_[{by, of}] = level; }
  void recognise_for(const std::string& by, const std::string& of, const std::string& matter, Level level) { matter_[{by, of, matter}] = level; }
  void withdraw(const std::string& by, const std::string& of) { level_[{by, of}] = Level::none; }
  Level level(const std::string& by, const std::string& of, const std::string& matter = {}) const {
    if (!matter.empty())
      if (const auto it = matter_.find({by, of, matter}); it != matter_.end()) return it->second;
    const auto it = level_.find({by, of});
    return it == level_.end() ? Level::none : it->second;
  }
  bool recognises(const std::string& by, const std::string& of, Level at_least, const std::string& matter = {}) const {
    return static_cast<int>(level(by, of, matter)) >= static_cast<int>(at_least);
  }
  // Whether each of two communities recognises the other at the level, at least.
  bool mutual(const std::string& a, const std::string& b, Level at_least, const std::string& matter = {}) const {
    return recognises(a, b, at_least, matter) && recognises(b, a, at_least, matter);
  }
  // The recognitions each community's chronicle holds, keyed by the community it is the chronicle of.
  static Recognitions read(const Chronicles& chronicles) {
    Recognitions r;
    for (const auto& [by, st] : chronicles) {
      if (!st) continue;
      for (const auto& [of, level] : st->recognitions) r.recognise(by, of, level_named(level));
      for (const auto& [key, level] : st->recognised_for) r.recognise_for(by, key.first, key.second, level_named(level));
    }
    return r;
  }

 private:
  std::map<std::pair<std::string, std::string>, Level> level_;
  std::map<std::tuple<std::string, std::string, std::string>, Level> matter_;
};

// ---- Compacts ---------------------------------------------------------------------------------------------------------------------

struct Compact {
  std::string id;
  std::vector<std::string> parties;  // the communities it would bind; none named: any community that ratifies it, found from the records
};

struct Standing {
  std::vector<std::string> ratified, pending;     // whose ratification stands at the turn, and those named with none standing
  std::vector<std::string> bound, withdrawing, withdrawn;
  long long entered = -1;                          // the turn it entered into force; -1: not yet
  bool force = false;
  bool in_force() const { return force; }
};

namespace detail {
using federation::Chronicles;
// The parties of a compact: those it names, or, for a compact that names none (open to any community), every community whose chronicle
// holds a ratification of it, in the order of their names.
inline std::vector<std::string> parties_of(const Compact& c, const Chronicles& chronicles) {
  if (!c.parties.empty()) return c.parties;
  std::vector<std::string> out;
  for (const auto& [party, st] : chronicles)
    if (st)
      for (const auto& r : st->rules.compacts)
        if (r.id == c.id && std::find(out.begin(), out.end(), party) == out.end()) out.push_back(party);
  return out;
}
// The latest turn the chronicles show.
inline long long now(const Chronicles& chronicles) {
  long long t = 0;
  for (const auto& [party, st] : chronicles)
    if (st && !st->acts.empty()) t = std::max(t, st->acts.back().turn);
  return t;
}
// The compact's terms, as the first party that ratified them carries them; else terms of its name and parties alone (entry by all, no
// notice).
inline thesmos::compacts::Terms terms_of(const Compact& c, const Chronicles& chronicles) {
  for (const auto& party : parties_of(c, chronicles))
    if (const auto it = chronicles.find(party); it != chronicles.end() && it->second)
      for (const auto& r : it->second->rules.compacts)
        if (r.id == c.id && r.has_terms) return r.terms;
  thesmos::compacts::Terms t;
  t.format = "noetarium.compact/1", t.id = c.id, t.parties = c.parties, t.articles = {{"the compact", false}};
  return t;
}
// Each party's ratifications of the compact, read from its own chronicle.
inline thesmos::compacts::Record record_of(const Compact& c, const Chronicles& chronicles) {
  thesmos::compacts::Record rec;
  for (const auto& party : parties_of(c, chronicles))
    if (const auto it = chronicles.find(party); it != chronicles.end() && it->second)
      for (const auto& r : it->second->rules.compacts)
        if (r.id == c.id) rec[party].push_back(r);
  return rec;
}
}  // namespace detail

// The compact's standing at a turn (by default the latest the chronicles show), read from each party's own chronicle: a compact is
// ratified there when an amendment of the community, decided by its own rule, adopted it, and withdrawn by another.
inline Standing standing(const Compact& c, const Chronicles& chronicles, long long turn = -1) {
  if (turn < 0) turn = detail::now(chronicles);
  const auto t = detail::terms_of(c, chronicles);
  const auto x = thesmos::compacts::standing(t, detail::record_of(c, chronicles), turn);
  Standing s;
  s.ratified = x.ratified, s.bound = x.bound, s.withdrawing = x.withdrawing, s.withdrawn = x.withdrawn, s.entered = x.entered, s.force = x.in_force;
  for (const auto& p : detail::parties_of(c, chronicles))
    if (std::find(s.ratified.begin(), s.ratified.end(), p) == s.ratified.end() && std::find(s.withdrawn.begin(), s.withdrawn.end(), p) == s.withdrawn.end())
      s.pending.push_back(p);
  return s;
}

// Whether an article of the compact binds two of its parties to each other at a turn, read from their chronicles.
inline bool binds(const Compact& c, const Chronicles& chronicles, long long turn, const std::string& article, const std::string& a, const std::string& b) {
  return thesmos::compacts::binds(detail::terms_of(c, chronicles), detail::record_of(c, chronicles), turn, article, a, b);
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
