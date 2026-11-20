// The procedure of an allocation, kept as a record that cannot be altered unnoticed, and judged by the checkable rules of
// procedural justice.
//
// Procedure confers legitimacy, not correctness (Rawls's imperfect procedural justice; the survey, §1.6). What can be checked is a
// record: whether the round was published before it was applied, whether its result follows from it, whether each stage gave its
// reason, where each datum came from, who declared an interest, who was consulted, whether every appeal was heard and an upheld
// one revised the round for everyone similarly placed, whether a lottery's seed was committed before the draw. The rules are
// Leventhal's six (1980: consistency, bias suppression, accuracy, correctability, representativeness, ethicality) and Daniels and
// Sabin's accountability for reasonableness (1997, 2008: publicity, relevance, revision and appeals, enforcement). Relevance and
// ethicality need a person's judgement, and are reported undecided with that reason.
//
// The record is archeion's hash-chained journal: each act an entry sealed to the one before, rounds and results named by the
// SHA-256 of their canonical text, so that a round published and a round applied can be compared by their digests.
//
// An appeal attacks a datum (an entity's attribute), a parameter of a stage, or a stage's applicability. Hearing it recomputes the
// round for everyone, not only the appellant: the failures the survey reads (Robodebt's unappealed tribunal losses) came from
// appeals that settled one case and left the rule standing (§1.6, Part V §3).
//
// A lottery among those the procedure cannot tell apart is drawn from a seed committed (its digest entered in the record) before
// the draw, each candidate's key the SHA-256 of the seed and its id, so that anyone can repeat the draw; a weighted lottery takes
// u^(1/w) for each key u in (0, 1) (Efraimidis and Spirakis 2006), the weights answering unequal claims (Saunders 2009).
#pragma once

#include <allocation/rounds/rounds.hpp>
#include <apodeixis/kernel/verdict.hpp>
#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace allocation::procedure {

using apodeixis::kernel::Verdict;
using archeion::fixity::Entry;
using archeion::fixity::Journal;
using rounds::Refusal;
using rounds::Result;
using rounds::Round;

namespace detail {

inline std::string exact(double v) {
  char b[40];
  std::snprintf(b, sizeof b, "%.17g", v);
  return b;
}

inline std::string field(const std::string& s) {
  // A field of canonical text: its length then its bytes, so that no content can be mistaken for structure.
  return std::to_string(s.size()) + ":" + s + ";";
}

}  // namespace detail

// The canonical text of a round: every field in a fixed order, numbers exactly, entities in the order given, attributes and
// sources in key order. Two rounds have the same digest exactly when they declare the same thing.
inline std::string canonical(const Round& r) {
  using detail::exact;
  using detail::field;
  std::string s = "round;" + field(r.name) + field(r.resource) + field(r.unit) + field(exact(r.amount));
  for (const auto& e : r.entities) {
    s += "entity;" + field(e.id) + field(e.name) + field(e.group);
    for (const auto& [k, v] : e.at) s += field(k) + field(exact(v));
  }
  for (const auto& st : r.stages) {
    s += "stage;" + field(st.kind) + field(st.principle) + field(st.source.who) + field(st.source.when) + field(st.source.why) + field(exact(st.share)) +
         field(st.attribute) + field(exact(st.bound)) + field(st.at_least ? "at least" : "at most") + field(st.per) + field(st.rule) + field(exact(st.theta)) +
         field(st.claim) + field(exact(st.lambda));
    for (const auto& ind : st.indicators) {
      s += "indicator;" + field(ind.name) + field(ind.rising ? "rising" : "falling") + field(exact(ind.weight));
      for (double x : ind.edges) s += field(exact(x));
    }
    for (std::size_t k = 0; k < st.bases.size(); k++) s += "base;" + field(st.bases[k]) + field(k < st.rates.size() ? exact(st.rates[k]) : "");
  }
  for (const auto& [k, v] : r.sources) s += "source;" + field(k) + field(v);
  return s;
}

inline std::string canonical(const Result& res) {
  std::string s = "result;";
  for (double a : res.amount) s += detail::field(detail::exact(a));
  for (const auto& x : res.excluded) s += detail::field(x);
  return s;
}

inline std::string digest(const Round& r) { return archeion::fixity::sha256_hex(canonical(r)); }
inline std::string digest(const Result& r) { return archeion::fixity::sha256_hex(canonical(r)); }

// ---- The record ---------------------------------------------------------------------------------------------------------------

struct Appeal {
  std::string when, who;          // who appeals, on behalf of which entity
  std::string entity;             // the entity's id ("" when it attacks the round as a whole)
  std::string kind = "datum";     // datum | parameter | stage
  std::size_t stage = 0;          // parameter, stage: the stage attacked (from 0)
  std::string key;                // datum: the attribute; parameter: bound, share, theta, lambda or rule
  double value = 0;               // the value claimed
  std::string text;               // parameter rule: the rule claimed
  std::string reason;
};

struct Review {
  std::size_t appeal = 0;
  std::string when, who;
  bool upheld = false;
  std::string reason;
};

struct Draw {
  std::string when, who;
  std::string commitment;          // the digest of the seed, entered before
  std::string seed;                // revealed at the draw
  std::vector<std::string> among;  // the candidates, by id
  std::vector<double> weights;     // empty: equal chances
  std::vector<std::string> drawn;  // in the order drawn
};

struct Record {
  Journal journal;
  std::vector<std::string> declared;                        // the digests of the rounds declared, in order
  std::vector<std::pair<std::string, std::string>> allocated;  // (round digest, result digest), in order
  std::map<std::string, std::set<std::string>> interests;   // who → the entities they declared an interest in
  std::set<std::string> consulted;                          // groups consulted
  std::vector<std::string> commitments;                     // digests of seeds committed
  std::vector<Appeal> appeals;
  std::vector<Review> reviews;
  std::vector<Draw> draws;
  std::vector<std::string> overseers;                       // bodies that may enforce the rules
};

inline void declare(Record& rec, const Round& r, const std::string& when, const std::string& who, const std::string& note = {}) {
  const std::string d = digest(r);
  rec.journal.append({when, who, "declare", d, note.empty() ? "the round " + r.name + " published" : note, {}});
  rec.declared.push_back(d);
}

inline void consult(Record& rec, const std::string& group, const std::string& when, const std::string& who, const std::string& note) {
  rec.journal.append({when, who, "consult", group, note, {}});
  rec.consulted.insert(group);
}

inline void interest(Record& rec, const std::string& who, const std::string& entity, const std::string& when, const std::string& note = {}) {
  rec.journal.append({when, who, "interest", entity, note, {}});
  rec.interests[who].insert(entity);
}

inline void oversee(Record& rec, const std::string& body, const std::string& when, const std::string& note) {
  rec.journal.append({when, body, "oversee", "", note, {}});
  rec.overseers.push_back(body);
}

inline void allocate(Record& rec, const Round& r, const Result& res, const std::string& when, const std::string& who) {
  const std::string d = digest(r), e = digest(res);
  rec.journal.append({when, who, "allocate", d, e, {}});
  rec.allocated.push_back({d, e});
}

inline std::size_t appeal(Record& rec, const Appeal& a) {
  if (a.kind != "datum" && a.kind != "parameter" && a.kind != "stage") throw Refusal("an appeal attacks a datum, a parameter or a stage");
  rec.journal.append({a.when, a.who, "appeal", a.entity, a.kind + " " + (a.kind == "datum" ? a.key : "stage " + std::to_string(a.stage + 1) + (a.key.empty() ? "" : " " + a.key)) + ": " + a.reason, {}});
  rec.appeals.push_back(a);
  return rec.appeals.size() - 1;
}

// The round as the appeal would have it.
inline Round corrected(const Round& r, const Appeal& a) {
  Round c = r;
  if (a.kind == "datum") {
    auto it = std::find_if(c.entities.begin(), c.entities.end(), [&](const rounds::Entity& e) { return e.id == a.entity; });
    if (it == c.entities.end()) throw Refusal("the appeal names an entity not in the round: " + a.entity);
    it->at[a.key] = a.value;
  } else {
    if (a.stage >= c.stages.size()) throw Refusal("the appeal names a stage the round does not have");
    if (a.kind == "stage") c.stages.erase(c.stages.begin() + static_cast<long>(a.stage));
    else {
      auto& s = c.stages[a.stage];
      if (a.key == "bound") s.bound = a.value;
      else if (a.key == "share") s.share = a.value;
      else if (a.key == "theta") s.theta = a.value;
      else if (a.key == "lambda") s.lambda = a.value;
      else if (a.key == "rule") s.rule = a.text;
      else throw Refusal("an appeal's parameter is bound, share, theta, lambda or rule");
    }
  }
  return c;
}

struct Hearing {
  Round revised;
  Result before, after;
  std::vector<std::size_t> changed;   // every entity whose amount the correction changes, the appellant and those similarly placed
  std::string said;
};

inline Hearing hear(const Round& r, const Appeal& a) {
  Hearing h;
  h.revised = corrected(r, a);
  h.before = rounds::run(r);
  h.after = rounds::run(h.revised);
  const double tol = 1e-9 * std::max(1.0, r.amount);
  for (std::size_t i = 0; i < h.before.amount.size(); i++)
    if (std::abs(h.before.amount[i] - h.after.amount[i]) > tol || h.before.excluded[i] != h.after.excluded[i]) h.changed.push_back(i);
  h.said = "the correction changes what " + apodeixis::kernel::count(h.changed.size(), "entity", "entities") + " receive";
  return h;
}

// A review of an appeal; an upheld one declares the corrected round, which applies to everyone.
inline void review(Record& rec, const Review& v, const Round& revised = {}) {
  if (v.appeal >= rec.appeals.size()) throw Refusal("a review answers an appeal in the record");
  rec.journal.append({v.when, v.who, "review", std::to_string(v.appeal + 1), std::string(v.upheld ? "upheld: " : "dismissed: ") + v.reason, {}});
  rec.reviews.push_back(v);
  if (v.upheld) declare(rec, revised, v.when, v.who, "the round revised on appeal " + std::to_string(v.appeal + 1));
}

// ---- Lotteries ----------------------------------------------------------------------------------------------------------------

inline void commit(Record& rec, const std::string& seed, const std::string& when, const std::string& who) {
  const std::string d = archeion::fixity::sha256_hex(seed);
  rec.journal.append({when, who, "commit", d, "a lottery's seed committed", {}});
  rec.commitments.push_back(d);
}

// The candidates' order under a seed: by u^(1/w), largest first, u from the first 52 bits of SHA-256(seed ‖ 0x1F ‖ id).
inline std::vector<std::string> lottery_order(const std::string& seed, const std::vector<std::string>& among, const std::vector<double>& weights = {}) {
  if (!weights.empty() && weights.size() != among.size()) throw Refusal("a weight for every candidate or none");
  std::vector<std::pair<double, std::string>> keyed;
  for (std::size_t i = 0; i < among.size(); i++) {
    const std::string h = archeion::fixity::sha256_hex(seed + '\x1f' + among[i]);
    const unsigned long long bits = std::stoull(h.substr(0, 13), nullptr, 16);  // 52 bits
    const double u = (static_cast<double>(bits) + 0.5) / 4503599627370496.0;     // in (0, 1)
    const double w = weights.empty() ? 1.0 : weights[i];
    if (!(w > 0)) throw Refusal("a lottery's weights are positive");
    keyed.push_back({std::log(u) / w, among[i]});
  }
  std::sort(keyed.begin(), keyed.end(), [](const auto& a, const auto& b) { return a.first != b.first ? a.first > b.first : a.second < b.second; });
  std::vector<std::string> out;
  for (const auto& k : keyed) out.push_back(k.second);
  return out;
}

inline Draw draw(Record& rec, const std::string& seed, const std::vector<std::string>& among, std::size_t k, const std::string& when, const std::string& who,
                 const std::vector<double>& weights = {}) {
  Draw d{when, who, archeion::fixity::sha256_hex(seed), seed, among, weights, {}};
  auto order = lottery_order(seed, among, weights);
  order.resize(std::min(k, order.size()));
  d.drawn = order;
  std::string note = "seed " + seed + "; drawn:";
  for (const auto& x : d.drawn) note += " " + x;
  rec.journal.append({when, who, "draw", d.commitment, note, {}});
  rec.draws.push_back(d);
  return d;
}

// ---- The rules of procedural justice ------------------------------------------------------------------------------------------

struct Rule {
  std::string name;        // consistency, publicity, …
  std::string framework;   // Leventhal, A4R or both
  Verdict verdict;
};

inline std::vector<Rule> check(const Record& rec, const Round& applied) {
  using apodeixis::kernel::Status;
  std::vector<Rule> out;
  const std::string d = digest(applied);
  const Result res = rounds::run(applied);
  const std::string e = digest(res);

  {  // The journal itself.
    out.push_back({"record sealed", "both", archeion::fixity::verify(rec.journal)});
  }
  {  // Consistency: the result recorded is what the declared round gives, applied alike to every entity.
    std::string claim = "the result recorded follows from the round applied";
    auto it = std::find_if(rec.allocated.rbegin(), rec.allocated.rend(), [&](const auto& p) { return p.first == d; });
    if (it == rec.allocated.rend()) out.push_back({"consistency", "Leventhal", Verdict::no(claim, "no allocation of this round is recorded")});
    else if (it->second != e) out.push_back({"consistency", "Leventhal", Verdict::no(claim, "the result recorded is not what the round gives: recomputing it gives another digest").with("recorded", it->second).with("recomputed", e)});
    else out.push_back({"consistency", "Leventhal", Verdict::yes(claim, "recomputing the round gives the result recorded, digest " + e.substr(0, 12))});
  }
  {  // Publicity: the round applied was published before it was applied.
    std::string claim = "the round was published before it was applied";
    std::size_t declared_at = 0, allocated_at = 0;
    for (std::size_t k = 0; k < rec.journal.entries.size(); k++) {
      const auto& x = rec.journal.entries[k];
      if (x.what == "declare" && x.about == d && !declared_at) declared_at = k + 1;
      if (x.what == "allocate" && x.about == d) allocated_at = k + 1;
    }
    if (!declared_at) out.push_back({"publicity", "A4R", Verdict::no(claim, "the round applied was never published: no declaration has its digest")});
    else if (allocated_at && allocated_at < declared_at) out.push_back({"publicity", "A4R", Verdict::no(claim, "the round was applied before it was published")});
    else out.push_back({"publicity", "A4R", Verdict::yes(claim, "published as entry " + std::to_string(declared_at) + (allocated_at ? ", applied as entry " + std::to_string(allocated_at) : ""))});
  }
  {  // Reasons: every stage names its principle and gives its reason (relevance itself is a person's judgement).
    std::string missing;
    for (std::size_t k = 0; k < applied.stages.size(); k++)
      if (applied.stages[k].principle.empty() || applied.stages[k].source.why.empty() || applied.stages[k].source.who.empty())
        missing += (missing.empty() ? "" : ", ") + std::to_string(k + 1);
    const std::string claim = "every stage names its principle, who declared it and why";
    out.push_back({"reasons given", "A4R", missing.empty() ? Verdict::yes(claim, "each of the " + std::to_string(applied.stages.size()) + " stages does")
                                                           : Verdict::no(claim, "stage(s) " + missing + " do not")});
    out.push_back({"relevance", "A4R", Verdict::unknown("the reasons rest on what fair-minded people can accept as relevant", "a person's judgement: the record holds the reasons, not their relevance")});
  }
  {  // Accuracy: every attribute the stages read says where its values came from.
    std::string missing;
    for (const auto& a : rounds::attributes_read(applied))
      if (!applied.sources.count(a) || applied.sources.at(a).empty()) missing += (missing.empty() ? "" : ", ") + a;
    const std::string claim = "every datum the round reads has a stated source";
    out.push_back({"accuracy", "Leventhal", missing.empty() ? Verdict::yes(claim, "each attribute read is sourced") : Verdict::no(claim, "no source for: " + missing)});
  }
  {  // Bias suppression: no stage was declared by someone with a declared interest in an entity that stage gave to.
    const std::string claim = "no one declared a stage that gave to an entity they declared an interest in";
    if (rec.interests.empty()) out.push_back({"bias suppression", "Leventhal", Verdict::unknown(claim, "no interests were declared, so none can be checked")});
    else {
      std::string found;
      for (std::size_t k = 0; k < applied.stages.size(); k++) {
        const auto it = rec.interests.find(applied.stages[k].source.who);
        if (it == rec.interests.end()) continue;
        for (std::size_t i = 0; i < applied.entities.size(); i++)
          if (res.by_stage[k][i] > 0 && it->second.count(applied.entities[i].id))
            found += (found.empty() ? "" : "; ") + applied.stages[k].source.who + " declared stage " + std::to_string(k + 1) + ", which gave to " + applied.entities[i].id;
      }
      out.push_back({"bias suppression", "Leventhal", found.empty() ? Verdict::yes(claim, "checked against " + std::to_string(rec.interests.size()) + " declarations of interest") : Verdict::no(claim, found)});
    }
  }
  {  // Representativeness: every group of entities was consulted.
    std::set<std::string> groups;
    for (const auto& en : applied.entities)
      if (!en.group.empty()) groups.insert(en.group);
    const std::string claim = "every group the round allocates to was consulted";
    if (groups.empty()) out.push_back({"representativeness", "Leventhal", Verdict::unknown(claim, "the entities name no groups")});
    else {
      std::string missing;
      for (const auto& g : groups)
        if (!rec.consulted.count(g)) missing += (missing.empty() ? "" : ", ") + g;
      out.push_back({"representativeness", "Leventhal", missing.empty() ? Verdict::yes(claim, "all " + std::to_string(groups.size()) + " groups were") : Verdict::no(claim, "not consulted: " + missing)});
    }
  }
  {  // Correctability, revision and appeals: every appeal heard; an upheld one followed by a revised round that was then applied.
    const std::string claim = "every appeal was heard, and an upheld one revised the round for everyone";
    std::string unheard, unrevised;
    for (std::size_t a = 0; a < rec.appeals.size(); a++) {
      auto it = std::find_if(rec.reviews.begin(), rec.reviews.end(), [&](const Review& v) { return v.appeal == a; });
      if (it == rec.reviews.end()) unheard += (unheard.empty() ? "" : ", ") + std::to_string(a + 1);
    }
    // An upheld review declares a revision; the last revision is what must be applied.
    std::string last_revision;
    for (const auto& x : rec.journal.entries)
      if (x.what == "declare" && x.note.rfind("the round revised on appeal", 0) == 0) last_revision = x.about;
    if (!last_revision.empty() && last_revision != d) unrevised = "the round applied is not the last one revised on appeal";
    Verdict v = !unheard.empty() ? Verdict::no(claim, "appeal(s) " + unheard + " not heard")
                : !unrevised.empty() ? Verdict::no(claim, unrevised)
                : rec.appeals.empty() ? Verdict::unknown(claim, "no appeal was made, so the path of appeal was not tried")
                                      : Verdict::yes(claim, apodeixis::kernel::count(rec.appeals.size(), "appeal") + " heard" + (last_revision.empty() ? "" : ", the revised round applied"));
    out.push_back({"correctability", "both", v});
  }
  {  // Lotteries: each draw after a commitment to its seed, and repeatable from it.
    const std::string claim = "every lottery was drawn from a seed committed before it, and repeats";
    if (rec.draws.empty()) out.push_back({"lottery", "both", Verdict::unknown(claim, "no lottery was drawn")});
    else {
      std::string fault;
      for (std::size_t k = 0; k < rec.draws.size(); k++) {
        const auto& dr = rec.draws[k];
        std::size_t committed_at = 0, drawn_at = 0;
        for (std::size_t j = 0; j < rec.journal.entries.size(); j++) {
          const auto& x = rec.journal.entries[j];
          if (x.what == "commit" && x.about == dr.commitment && !committed_at) committed_at = j + 1;
          if (x.what == "draw" && x.about == dr.commitment) drawn_at = j + 1;
        }
        auto again = lottery_order(dr.seed, dr.among, dr.weights);
        again.resize(dr.drawn.size());
        if (archeion::fixity::sha256_hex(dr.seed) != dr.commitment) fault += "draw " + std::to_string(k + 1) + "'s seed does not match its commitment; ";
        else if (!committed_at || committed_at > drawn_at) fault += "draw " + std::to_string(k + 1) + " was not committed before it was drawn; ";
        else if (again != dr.drawn) fault += "draw " + std::to_string(k + 1) + " does not repeat from its seed; ";
      }
      out.push_back({"lottery", "both", fault.empty() ? Verdict::yes(claim, apodeixis::kernel::count(rec.draws.size(), "draw") + " checked") : Verdict::no(claim, fault)});
    }
  }
  out.push_back({"ethicality", "Leventhal", Verdict::unknown("the procedure accords with the moral values of those it allocates to", "a person's judgement, and theirs")});
  {
    const std::string claim = "a body can enforce the procedure's rules";
    out.push_back({"enforcement", "A4R", rec.overseers.empty() ? Verdict::unknown(claim, "no body of oversight is recorded") : Verdict::yes(claim, "recorded: " + rec.overseers.front())});
  }
  return out;
}

}  // namespace allocation::procedure
