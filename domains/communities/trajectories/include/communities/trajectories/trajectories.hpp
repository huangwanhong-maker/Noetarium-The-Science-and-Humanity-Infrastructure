// Trajectories: what a society reads from its archive of acts and conduct (the design *Communities That Agents Of Any Provider Find,
// Join, Work In And Leave*, Section 4: Definitions 29, 32 and 33, and the measures of power of Section 4.6).
//
// Three readings, each reproducible from the record by anyone who holds it:
//
// - **Facts about conduct recognised** (Definition 29; thesmos's Definitions 30 and 35). An observation is an act of evidence (a report,
//   an attestation or an assessment) that names the fact, its subject, what it says and its grade, and cites traces by digest. A grade A
//   or B stands only when every trace it cites is in the record (a seal of the chronicle, a holding's digest, or an episode of the journal
//   the reader holds); otherwise it is read as C, an account from memory. The standing is thesmos's rule of recognition under the
//   community's declared standard. A recognised fact states the three criteria of the Generative Relational framework: what it emerged
//   from (the observations and the standard), since when it persists (the turn from which it has stood established), and what it
//   constrains (the acts that rest on it, by their field "rests on"). Reports of one origin count once: the corroboration is the number of
//   distinct origins among the observations that meet the standard and agree, and their weight is tekmerion's force of reports that
//   share an origin (its decision 0007), each origin a group that registered the fact through the channel its best grade declares.
// - **Contribution** (Definition 32). A party's contribution to an artefact is the part of the graph of provenance beneath it (archeion's
//   PROV: the activities that generated it or anything in its lineage) attributed to the party or to agents acting for it (an operative's
//   origin under control), by role (the activity's kind: writing, reviewing, checking, integrating, maintaining), so that receiving work is
//   counted; under declared weights of the roles, each party's share of the weighted activities, exactly; and what a faulty input taints.
// - **Measures of power** (Section 4.6), each a declared mechanism named with its parameters and identified by the digest of its
//   declaration, none adopted by default, each answered with the head of the record it read (Definition 33): tenure by office (from a
//   replay of the record, every interval a position was held), the concentration of acts of a force by origin, the authorship of
//   amendments, rulings by origin, petitions by origin, voting power under a decision's rule (the decision's own rule, unit and weights
//   read by thesmos, its a priori power by nomos's games), and offices held by one origin beyond a declared bound. A measure is itself a
//   reading: several stand side by side, and what a community does with one goes by a declared route, never by the measure alone.
// - **Value readings** (Definition 33, Proposition 15; the companion paper's Proposition 1 and its condition H7). A declared mechanism of
//   value formation read from the chronicle and the behaviour journal through their join (thesmos's Definition 31): the share of
//   decisions of a kind that went one way (a rule of succession, exact), a norm as practised (thesmos's facets of a statement's norm),
//   and conduct by category (the journal's episodes in the categories of the taxonomy in force, by their outcome). An act whose field
//   "under reading" names a reading is left out of every mechanism, with the episode that constituted it (H7), so that a reading is never
//   fed by the decisions taken under it. The reading names its mechanism by digest, both heads, the join's verdict, its result and grade
//   B, and its own digest: two readers holding records that verify against the same heads obtain the same digest.

#pragma once

#include <communities/trajectories/episodes.hpp>

#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/chronicle/conduct.hpp>
#include <thesmos/crossings/crossings.hpp>
#include <thesmos/norms/norms.hpp>
#include <thesmos/statements/statements.hpp>

#include <archeion/fixity/sha256.hpp>
#include <archeion/provenance/provenance.hpp>

#include <nomos/games/power.hpp>
#include <nomos/measurement/concentration.hpp>

#include <tekmerion/evidence/dependence.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace communities::trajectories {

using morphe::arithmetic::Rational;
using thesmos::acts::Act;
using thesmos::chronicle::State;

namespace detail {
inline std::string origin(const State& s, const std::string& p) { return s.parties.has(p) ? thesmos::kernel::origin(s.parties, p) : p; }
inline std::vector<std::string> list(const std::string& s) {
  std::vector<std::string> out;
  std::stringstream in(s);
  for (std::string item; std::getline(in, item, ',');) {
    while (!item.empty() && item.front() == ' ') item.erase(item.begin());
    while (!item.empty() && item.back() == ' ') item.pop_back();
    if (!item.empty()) out.push_back(item);
  }
  return out;
}
// The party that answers for an act: the root principal of its mandate, or its author.
inline std::string bearer(const State& s, const Act& a) {
  if (a.mandate.empty()) return a.author;
  std::string at = a.mandate;
  for (std::size_t steps = 0; steps <= s.mandates.size(); steps++) {
    const auto it = s.mandates.find(at);
    if (it == s.mandates.end()) return a.author;
    if (it->second.parent.empty()) return it->second.principal;
    at = it->second.parent;
  }
  return a.author;
}
}  // namespace detail

// ---- Facts about conduct recognised -----------------------------------------------------------------------------------------------

struct Observation {
  std::string author, origin, kind, fact, subject, value, grade, traced;  // traced: the grade as its traces bear it out
  std::vector<std::string> cites;
  long long turn = 0;
  std::size_t place = 0;
};

// Whether a digest is a trace the record holds: a seal of the chronicle, a holding's digest or one of its earlier versions, or one of the
// traces the reader holds beside it (the seals of the behaviour journal's episodes).
inline bool traced(const State& s, const std::string& digest, const std::set<std::string>& traces) {
  if (traces.count(digest)) return true;
  for (const auto& e : s.journal.entries)
    if (e.seal == digest) return true;
  for (const auto& [id, h] : s.holdings) {
    if (h.provenance.digest == digest) return true;
    if (std::find(h.versions.begin(), h.versions.end(), digest) != h.versions.end()) return true;
  }
  return false;
}

// The observations of a fact about a subject in the record: its acts of evidence that name the fact and the subject, in order.
inline std::vector<Observation> observations(const State& s, const std::string& fact, const std::string& subject, const std::set<std::string>& traces = {}) {
  std::vector<Observation> out;
  for (std::size_t k = 0; k < s.readings.size(); k++) {
    const Act& a = s.readings[k];
    if ((a.kind != "report" && a.kind != "attest" && a.kind != "assess") || a.field("fact") != fact || a.field("subject") != subject) continue;
    Observation o;
    o.author = a.author, o.origin = detail::origin(s, a.author), o.kind = a.kind, o.fact = fact, o.subject = subject, o.value = a.field("value");
    o.grade = a.field("grade").empty() ? std::string("C") : a.field("grade");
    o.cites = detail::list(a.field("cites"));
    bool all = !o.cites.empty();
    for (const auto& c : o.cites) all = all && traced(s, c, traces);
    o.traced = (o.grade == "A" || o.grade == "B") && !all ? std::string("C") : o.grade;
    o.turn = a.turn, o.place = k;
    out.push_back(std::move(o));
  }
  return out;
}

// The standard of recognition in force for a fact (the constitution's taxonomy, the base's among them), if one is declared.
inline std::optional<thesmos::behaviour::Standard> standard_of(const State& s, const std::string& fact) {
  for (const auto& x : s.rules.behaviour.recognition)
    if (x.fact == fact) return x;
  return std::nullopt;
}

struct Fact {
  std::string id, fact, subject, standing;
  std::vector<std::string> values;                   // what the observations meeting the standard say (one when established)
  thesmos::behaviour::Standard standard;
  std::vector<Observation> from;                     // what it emerged from: the observations, read under the standard
  long long since = -1;                              // the turn from which it has stood established, -1 when it does not
  std::vector<std::string> constrains;               // what it constrains: the acts that rest on it, as "kind by author at turn"
  std::size_t reports = 0, origins = 0;              // observations meeting the standard and agreeing, and their distinct origins
};

inline std::string fact_id(const std::string& fact, const std::string& subject) { return fact + " of " + subject; }

// A fact about conduct as the record recognises it under a standard.
inline Fact recognise(const State& s, const std::string& fact, const std::string& subject, const thesmos::behaviour::Standard& standard,
                      const std::set<std::string>& traces = {}) {
  Fact f;
  f.id = fact_id(fact, subject), f.fact = fact, f.subject = subject, f.standard = standard;
  f.from = observations(s, fact, subject, traces);
  const std::string subject_origin = detail::origin(s, subject);
  auto as_thesmos = [&](std::size_t n) {
    std::vector<thesmos::behaviour::Observation> v;
    for (std::size_t k = 0; k < n; k++) v.push_back({f.from[k].author, f.from[k].origin, f.from[k].subject, f.from[k].traced, f.from[k].value});
    return v;
  };
  f.standing = thesmos::behaviour::standing(as_thesmos(f.from.size()), standard, subject_origin);
  // Since when: the last turn at which the standing became established and stayed so through every later observation.
  for (std::size_t n = 1; n <= f.from.size(); n++) {
    const bool established = thesmos::behaviour::standing(as_thesmos(n), standard, subject_origin) == "established";
    if (!established) f.since = -1;
    else if (f.since < 0) f.since = f.from[n - 1].turn;
  }
  if (f.standing != "established") f.since = -1;
  std::map<std::string, std::set<std::string>> agreeing;  // value -> origins that meet the standard
  std::map<std::string, std::size_t> count;
  for (const auto& o : f.from) {
    const bool meets = thesmos::behaviour::grade_rank(o.traced) >= thesmos::behaviour::grade_rank(standard.grade) && (!standard.another_origin || o.origin != subject_origin);
    if (!meets) continue;
    agreeing[o.value].insert(o.origin), count[o.value]++;
  }
  for (const auto& [v, os] : agreeing) f.values.push_back(v);
  if (f.values.size() == 1) f.reports = count[f.values.front()], f.origins = agreeing[f.values.front()].size();
  for (const Act& a : s.acts)
    if (a.field("rests on") == f.id) f.constrains.push_back(a.kind + " by " + a.author + " at turn " + std::to_string(a.turn));
  return f;
}

// The weight of observations for a value: each origin a group (tekmerion's reports that share an origin) that registered the fact through
// the channel its best grade declares, its observations relays through a declared channel; the likelihood ratio of what they say for the
// value against its absence, the ratio were each observation an independent report, and the factor by which that overstates.
struct Weight {
  double ratio = 1, as_if_independent = 1, overstatement = 1;
  std::size_t origins = 0, reports = 0;
};
inline Weight weigh(const std::vector<Observation>& obs, const std::string& value, const std::map<std::string, tekmerion::evidence::Channel>& by_grade,
                    const tekmerion::evidence::Channel& relay = {}) {
  std::map<std::string, std::vector<const Observation*>> by_origin;
  for (const auto& o : obs)
    if (by_grade.count(o.traced)) by_origin[o.origin].push_back(&o);
  std::vector<tekmerion::evidence::Group> groups;
  Weight w;
  for (const auto& [origin, os] : by_origin) {
    std::string best = "C";
    for (const Observation* o : os)
      if (thesmos::behaviour::grade_rank(o->traced) > thesmos::behaviour::grade_rank(best)) best = o->traced;
    tekmerion::evidence::Group g;
    g.origin = by_grade.at(best);
    for (const Observation* o : os) g.relays.push_back({relay, o->value == value});
    w.reports += os.size();
    groups.push_back(std::move(g));
  }
  w.origins = groups.size();
  if (groups.empty()) return w;
  w.ratio = tekmerion::evidence::dependent_ratio(1, 0, groups);
  w.as_if_independent = tekmerion::evidence::as_if_independent(1, 0, groups);
  w.overstatement = tekmerion::evidence::overstatement(1, 0, groups);
  return w;
}

// ---- Contribution over the graph of provenance ----------------------------------------------------------------------------------------

struct Contribution {
  std::string party;                                        // the party that answers for the agents' activities
  std::map<std::string, std::vector<std::string>> by_role;  // role (the activity's kind) -> the activities
  std::set<std::string> generated;                          // what its activities beneath the artefact generated
  Rational share;                                           // its share of the weighted activities, under declared weights of roles
};

// The activities beneath an artefact: those that generated it or an entity of its lineage.
inline std::vector<const archeion::provenance::Activity*> beneath(const archeion::provenance::Graph& g, const std::string& artefact) {
  std::set<std::string> below = {artefact};
  for (const auto& e : archeion::provenance::lineage(g, artefact)) below.insert(e);
  std::vector<const archeion::provenance::Activity*> out;
  for (const auto& a : g.activities) {
    bool here = false;
    for (const auto& x : a.generated) here = here || below.count(x);
    if (here) out.push_back(&a);
  }
  return out;
}

// Each party's contribution to an artefact, in the order of their names. A role with no declared weight weighs one; with no weights
// given every activity weighs one.
inline std::vector<Contribution> contribution(const archeion::provenance::Graph& g, const std::string& artefact, const thesmos::kernel::Parties& ps,
                                              const std::map<std::string, Rational>& weights = {}) {
  std::map<std::string, Contribution> by;
  Rational total(0);
  std::map<std::string, Rational> weighed;
  for (const auto* a : beneath(g, artefact)) {
    const std::string party = ps.has(a->agent) ? thesmos::kernel::origin(ps, a->agent) : a->agent;
    Contribution& c = by[party];
    c.party = party;
    c.by_role[a->kind].push_back(a->id);
    for (const auto& x : a->generated) c.generated.insert(x);
    const auto w = weights.find(a->kind);
    const Rational x = w == weights.end() ? Rational(1) : w->second;
    weighed[party] += x, total += x;
  }
  std::vector<Contribution> out;
  for (auto& [party, c] : by) {
    c.share = total == Rational(0) ? Rational(0) : weighed[party] / total;
    out.push_back(std::move(c));
  }
  return out;
}

// What faulty inputs taint of each party's contribution to an artefact: the entities its activities beneath the artefact generated that
// are, or were derived from, a faulty one (archeion's taint).
inline std::map<std::string, std::set<std::string>> tainted(const archeion::provenance::Graph& g, const std::string& artefact, const thesmos::kernel::Parties& ps,
                                                            const std::set<std::string>& faulty) {
  const auto bad = archeion::provenance::tainted(g, faulty);
  std::map<std::string, std::set<std::string>> out;
  for (const auto& c : contribution(g, artefact, ps))
    for (const auto& x : c.generated)
      if (bad.count(x)) out[c.party].insert(x);
  return out;
}

// ---- Measures of power: declared mechanisms ----------------------------------------------------------------------------------------

// A record as a reader holds it: the parties declared beside it, the constitution at the founding, and the admitted acts in order.
struct Record {
  thesmos::kernel::Parties parties;
  thesmos::constitutions::Constitution founding;
  std::vector<Act> acts;
};

// Every interval during which a party held a position, [from, to), read by replaying the record act by act: an interval begins at the turn
// of entry and ends at the turn of the act that left it ended (by leaving, an office, or a term, settled at that act); one still held ends
// at the last act's turn, and is marked open.
struct Interval {
  std::string party, position;
  long long from = 0, to = 0;
  bool open = false;
};
inline std::vector<Interval> tenure(const Record& r) {
  State s(r.parties, r.founding);
  std::vector<Interval> out;
  std::map<std::pair<std::string, std::string>, long long> held;
  long long last = 0;
  for (const Act& a : r.acts) {
    if (!thesmos::chronicle::step(s, a).admitted) continue;
    last = a.turn;
    for (auto it = held.begin(); it != held.end();) {
      const auto now = s.positions.find(it->first);
      if (now == s.positions.end() || now->second != it->second) {
        out.push_back({it->first.first, it->first.second, it->second, a.turn, false});
        it = held.erase(it);
      } else
        ++it;
    }
    for (const auto& [key, since] : s.positions)
      if (!held.count(key)) held[key] = since;
  }
  for (const auto& [key, since] : held) out.push_back({key.first, key.second, since, last, true});
  return out;
}

struct Mechanism {
  std::string name;
  std::map<std::string, std::string> parameters;
};
// The mechanisms this module declares, by name.
inline const std::vector<std::string>& mechanisms() {
  static const std::vector<std::string> v = {"tenure by office", "acts of force by origin", "authorship of amendments", "rulings by origin",
                                             "petitions by origin", "voting power", "long offices"};
  return v;
}
// The digest of a mechanism's declaration: its name and parameters in a canonical text, so that a reading cites what it applied.
inline std::string digest(const Mechanism& m) {
  std::string t = "noetarium.mechanism/1\x1f" + m.name;
  for (const auto& [k, v] : m.parameters) t += "\x1f" + k + "=" + v;
  return archeion::fixity::sha256_hex(t);
}

// A reading of power: the mechanism and its digest, the head of the record it read, the counts by key (an origin, or a member), their exact
// shares, the Herfindahl–Hirschman index of the counts (nomos), and for voting power each member's Banzhaf and Shapley–Shubik index.
struct Reading {
  Mechanism mechanism;
  std::string mechanism_digest, head;
  std::size_t size = 0;
  std::map<std::string, long long> counts;
  std::map<std::string, Rational> shares;
  double herfindahl = 0;
  std::map<std::string, double> banzhaf, shapley_shubik;
  std::vector<std::string> notes, refused;  // what the reading found to note, and why it could not be read
};

namespace detail {
inline std::string param(const Mechanism& m, const std::string& k, const std::string& otherwise = {}) {
  const auto it = m.parameters.find(k);
  return it == m.parameters.end() ? otherwise : it->second;
}
inline void finish(Reading& r) {
  long long total = 0;
  std::vector<double> xs;
  for (const auto& [k, n] : r.counts) total += n, xs.push_back(static_cast<double>(n));
  for (const auto& [k, n] : r.counts) r.shares[k] = total == 0 ? Rational(0) : Rational(n) / Rational(total);
  if (total > 0) r.herfindahl = nomos::measurement::herfindahl_hirschman(xs);
}
inline bool is_amendment(const thesmos::constitutions::Constitution& c, const std::string& decision) {
  for (const auto& [level, d] : c.amendment.by_level)
    if (d == decision) return true;
  return false;
}
}  // namespace detail

// Applies a declared mechanism to a record. Parameters: "position" for tenure and long offices, "bound" (turns) for long offices, "force"
// for acts of force (declarative unless named), "decision" for voting power.
inline Reading measure(const Mechanism& m, const Record& rec) {
  Reading r;
  r.mechanism = m, r.mechanism_digest = digest(m);
  State s(rec.parties, rec.founding);
  for (const Act& a : rec.acts) thesmos::chronicle::step(s, a);
  r.head = archeion::fixity::hex(thesmos::crossings::head(s.community, s).root), r.size = s.journal.entries.size();
  if (m.name == "tenure by office" || m.name == "long offices") {
    const std::string position = detail::param(m, "position");
    std::map<std::string, std::vector<std::pair<long long, long long>>> by_origin;  // origin -> intervals
    for (const auto& i : tenure(rec))
      if (i.position == position) by_origin[detail::origin(s, i.party)].push_back({i.from, i.to});
    if (m.name == "tenure by office")
      for (const auto& [o, iv] : by_origin)
        for (const auto& [a, b] : iv) r.counts[o] += b - a;
    else {
      const long long bound = detail::param(m, "bound").empty() ? -1 : std::stoll(detail::param(m, "bound"));
      if (bound < 0) r.refused.push_back("long offices read a bound in turns");
      // The longest continuous holding of the office by one origin: its intervals merged where they overlap or meet.
      for (auto& [o, iv] : by_origin) {
        std::sort(iv.begin(), iv.end());
        long long best = 0, from = iv.front().first, to = iv.front().second;
        for (const auto& [a, b] : iv) {
          if (a <= to) to = std::max(to, b);
          else best = std::max(best, to - from), from = a, to = b;
        }
        best = std::max(best, to - from);
        r.counts[o] = best;
        if (bound >= 0 && best > bound) r.notes.push_back(o + " has held " + position + " for " + std::to_string(best) + " turns together, beyond " + std::to_string(bound));
      }
      return r;  // no shares: each count is a length, not a part of a whole
    }
  } else if (m.name == "acts of force by origin") {
    const std::string force = detail::param(m, "force", "declarative");
    for (std::size_t k = 0; k < s.readings.size(); k++) {
      const auto f = thesmos::acts::ground().find(s.readings[k].kind);
      if (f == thesmos::acts::ground().end() || std::string(thesmos::acts::word(f->second)) != force) continue;
      r.counts[detail::origin(s, detail::bearer(s, s.acts[k]))]++;
    }
  } else if (m.name == "authorship of amendments") {
    for (const auto& [id, p] : s.proposals)
      if (p.status == "adopted" && detail::is_amendment(rec.founding, p.decision)) r.counts[detail::origin(s, p.author)]++;
  } else if (m.name == "rulings by origin") {
    for (const auto& [id, x] : s.rulings) r.counts[detail::origin(s, x.by)]++;
  } else if (m.name == "petitions by origin") {
    std::map<std::string, long long> met;
    for (const auto& [id, i] : s.items)
      if (i.type == "petition") {
        r.counts[detail::origin(s, i.bearer.empty() ? i.author : i.bearer)]++;
        if (i.status == "adopted" || i.status == "met" || i.status == "carried") met[detail::origin(s, i.bearer.empty() ? i.author : i.bearer)]++;
      }
    for (const auto& [o, n] : met) r.notes.push_back(o + ": " + std::to_string(n) + " met, adopted or carried");
  } else if (m.name == "voting power") {
    const auto d = s.rules.aggregation.find(detail::param(m, "decision"));
    if (d == s.rules.aggregation.end()) {
      r.refused.push_back("voting power reads a decision the constitution declares");
      return r;
    }
    const auto unit = thesmos::procedures::unit_named(d->second.count_by);
    const auto rule = thesmos::procedures::read_rule(d->second.rule);
    std::vector<std::string> members;
    for (const auto& [o, w] : d->second.weights)
      if (Rational(0) < w) members.push_back(o);
    if (members.empty())
      for (const auto& [key, since] : s.positions)
        if (s.member(key.first, d->second.body) && std::find(members.begin(), members.end(), detail::origin(s, key.first)) == members.end()) members.push_back(detail::origin(s, key.first));
    if (!unit || !rule || members.empty() || members.size() > 20) {
      r.refused.push_back("voting power reads a decision with a known rule and unit, and between one and twenty members");
      return r;
    }
    // The simple game of the decision: a coalition wins when its members voting for and the others against pass under the decision's
    // own rule, unit of count and weights (thesmos), every member casting a ballot.
    thesmos::kernel::Parties voters;
    for (const auto& o : members) voters.add({o, s.parties.has(o) ? s.parties.at(o).kind : thesmos::kernel::Kind::community});
    const nomos::games::SimpleGame game{members.size(), [&](std::uint32_t mask) {
                                          std::vector<thesmos::procedures::Ballot> ballots;
                                          for (std::size_t i = 0; i < members.size(); i++) ballots.push_back({members[i], members[i], (mask >> i & 1u) != 0});
                                          return thesmos::procedures::decide(voters, ballots, *unit, *rule, d->second.weights).passed;
                                        }};
    const auto p = nomos::games::power(game);
    for (std::size_t i = 0; i < members.size(); i++) r.banzhaf[members[i]] = p.banzhaf[i], r.shapley_shubik[members[i]] = p.shapley_shubik[i];
    return r;
  } else {
    r.refused.push_back("the mechanism " + m.name + " is none this module declares");
    return r;
  }
  detail::finish(r);
  return r;
}

// ---- Value readings: declared mechanisms over both records ----------------------------------------------------------------------

// A value reading (Definition 33): its mechanism and the mechanism's digest; the chronicle's head (community, entries, root) and the
// journal's (entries, last seal) it read, and the join's verdict ("joined", "no journal", or where the join breaks); the places (from
// 1) of the acts left out as decided under a reading and the episodes left out with them (H7); its result, each value an exact text;
// grade B; and its digest, a function of all of these.
struct ValueReading {
  Mechanism mechanism;
  std::string mechanism_digest, community, root, journal_seal, join;
  std::size_t size = 0, journal_size = 0, episodes_left_out = 0;
  std::vector<std::size_t> excluded;
  std::map<std::string, std::string> result;
  std::string grade = "B", digest;
  std::vector<std::string> refused;
};

inline const std::vector<std::string>& value_mechanisms() {
  static const std::vector<std::string> v = {"share of decisions", "practised norm", "conduct by category"};
  return v;
}

// Applies a mechanism of value formation to a community's state and, where it reads one, its behaviour journal. Parameters: "share of
// decisions" {act (answer), field (outcome), kinds "a, b", alpha (1)}, the share (n_a + α)/(n_a + n_b + 2α) of the acts of that kind
// whose field says a, exactly; "practised norm" {statement}, the facets of the statement's norm over the acts; "conduct by category", the
// journal's episodes counted in each decidable category of the taxonomy in force, and by outcome (constituted, refused, none).
inline ValueReading read_values(const Mechanism& m, const State& s, const thesmos::chronicle::Conduct* c = nullptr) {
  ValueReading r;
  r.mechanism = m, r.mechanism_digest = digest(m), r.community = s.community;
  const auto h = thesmos::crossings::head(s.community, s);
  r.size = h.size, r.root = archeion::fixity::hex(h.root);
  // H7: the acts decided under a reading, and the episodes that constituted them, are left out.
  std::set<std::string> marked_seals;
  std::vector<std::size_t> kept;
  for (std::size_t k = 0; k < s.acts.size(); k++)
    if (!s.acts[k].field("under reading").empty()) r.excluded.push_back(k + 1), marked_seals.insert(s.journal.entries[k].seal);
    else kept.push_back(k);
  if (c) {
    r.journal_size = c->journal.entries.size(), r.journal_seal = c->journal.head();
    const auto v = thesmos::chronicle::joined(s, *c);
    r.join = v.holds() ? "joined" : v.reason;
  } else
    r.join = "no journal";
  if (m.name == "share of decisions") {
    const std::string kind = detail::param(m, "act", "answer"), field = detail::param(m, "field", "outcome");
    const auto kinds = detail::list(detail::param(m, "kinds"));
    Rational alpha(1);
    bool read = kinds.size() == 2;
    try {
      if (!detail::param(m, "alpha").empty()) alpha = Rational::parse(detail::param(m, "alpha"));
    } catch (const std::exception&) {
      read = false;
    }
    if (!read || !(Rational(0) < alpha)) r.refused.push_back("a share of decisions reads two kinds (kinds \"a, b\") and a positive alpha");
    else {
      long long na = 0, nb = 0;
      for (std::size_t k : kept) {
        const Act& a = s.readings[k];
        if (a.kind != kind && s.acts[k].kind != kind) continue;
        na += a.field(field) == kinds[0], nb += a.field(field) == kinds[1];
      }
      r.result["n " + kinds[0]] = std::to_string(na), r.result["n " + kinds[1]] = std::to_string(nb);
      r.result["share of " + kinds[0]] = ((Rational(na) + alpha) / (Rational(na + nb) + alpha + alpha)).str();
    }
  } else if (m.name == "practised norm") {
    const auto st = thesmos::statements::read(detail::param(m, "statement"));
    if (!st.read) r.refused.push_back("the statement does not read: " + st.error);
    else {
      std::vector<Act> acts;
      for (std::size_t k : kept) acts.push_back(s.acts[k]);
      const auto holds = [&](const std::string& party, const std::string& position) { return s.holds(party, position); };
      const auto f = thesmos::norms::facets(thesmos::statements::norm(st.statement, holds, acts), acts);
      std::string marks;
      for (const auto& x : f.marks) marks += (marks.empty() ? "" : "; ") + x;
      r.result = {{"episodes", std::to_string(f.episodes)}, {"regularity", f.regularity ? f.regularity->str() : std::string("none")}, {"invoked", std::to_string(f.invoked)},
                  {"adopted", f.adopted ? "yes" : "no"}, {"marks", marks}};
    }
  } else if (m.name == "conduct by category") {
    if (!c) r.refused.push_back("conduct by category reads the behaviour journal");
    else {
      std::map<std::string, long long> n = {{"constituted", 0}, {"refused", 0}, {"none", 0}, {"unread", 0}};
      for (const auto& k : s.rules.behaviour.conduct)
        if (!k.analytic) n["category " + k.name] = 0;
      const auto& e = c->journal.entries;
      for (std::size_t k = 0; k < e.size(); k++) {
        if (e[k].what != "episode") continue;
        std::string outcome;
        if (k + 1 < e.size() && e[k + 1].what == "outcome" && e[k + 1].about.rfind("episode:" + e[k].seal + "\x1e", 0) == 0)
          outcome = e[k + 1].about.substr(("episode:" + e[k].seal + "\x1e").size());
        if (outcome.rfind("seal:", 0) == 0 && marked_seals.count(outcome.substr(5))) {
          r.episodes_left_out++;
          continue;
        }
        const auto episode = episode_of(e[k].about);
        if (!episode) {
          n["unread"]++;
          continue;
        }
        n[outcome.rfind("seal:", 0) == 0 ? "constituted" : outcome.rfind("refused:", 0) == 0 ? "refused" : "none"]++;
        for (const auto& name : thesmos::behaviour::categorise(s.rules.behaviour, *episode)) n["category " + name]++;
      }
      for (const auto& [k, x] : n) r.result[k] = std::to_string(x);
    }
  } else
    r.refused.push_back("the mechanism " + m.name + " is no mechanism of value this module declares");
  std::string t = "noetarium.value-reading/1\x1f" + r.mechanism_digest + "\x1f" + r.community + "\x1f" + std::to_string(r.size) + "\x1f" + r.root + "\x1f" +
                  std::to_string(r.journal_size) + "\x1f" + r.journal_seal + "\x1f" + r.join;
  for (const auto& [k, v] : r.result) t += "\x1f" + k + "=" + v;
  for (const auto& x : r.refused) t += "\x1frefused=" + x;
  r.digest = archeion::fixity::sha256_hex(t);
  return r;
}

}  // namespace communities::trajectories
