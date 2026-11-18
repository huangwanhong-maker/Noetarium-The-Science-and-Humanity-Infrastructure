// Theories of the case (ADR 0127): accounts of what happened, searched for by evolution, read for what the law makes of them, and
// tested on evidence they were not made from.
//
// A THEORY is an account in tekmerion's sense (scenarios/scenario.hpp): events of declared kinds with their hands, at places and
// instants, and a stance on every item of evidence. What the law asks of it is what it makes hold. Bindings say so: an event of a
// kind with an agent in a role makes a fact of the matter's vocabulary hold ("an ignition by Vane makes `accused_set_fire`"), and a
// believed item may make one hold too ("the receipt, believed, makes `accused_bought_fuel`"). The facts the bindings address are
// then set by the theory: proved where it makes them hold, not proved where it does not; the rest of the record stands as the
// matter declares it. Each issue is then asked under the theory, as any question of the matter is (norms.hpp, apodeixis): clear,
// arguable or untenable over the admissible readings of the open terms. A theory's facts are its own: what it makes hold is
// relational (it emerges from the account, persists while the account is held, and is what the law acts on); what happened is
// left open.
//
// OVER THE THEORIES FOUND. The search keeps, in each niche, the best coherent theories (tekmerion's search); here the niche is what a
// theory says of the central slot (by which kind, by whose hand) and each issue's answer under it. For each issue: the theories
// under which it follows and those under which it does not, the relative plausibility of each side (exp(−score), normalised over
// the theories kept), and the best theory of each side. The issue is clear over the theories found when it follows in all of them,
// untenable when in none, arguable otherwise; that is said of what was found and of nothing else.
//
// THE BURDEN. Whoever bears it must exclude the alternatives. Here an alternative is reasonable when it is a coherent theory, found,
// under which the issue does not follow, with a relative plausibility of at least a declared threshold. The threshold is the
// caller's (a matter's standard states it); none is any jurisdiction's, and "beyond reasonable doubt" is not a number in any law.
// An alternative not found is not thereby excluded: a search that keeps none is reported as having found none.
//
// THE TEST. Evidence may be held back from the search (in a matter, what came late, or what is kept aside on purpose), so that
// what was found can be tested on what it was not made from: a theory that predicts what it did not see is better grounded than
// one fitted to everything (the anchored narratives of Wagenaar, van Koppen and Crombag 1993 ask as much of a story). Each theory
// is tested once: the test is registered in a journal sealed by archeion's hash chain before the held-back items are read, and its
// outcome written after (admitted; or must doubt what it cannot admit; or no longer holds). A theory is never tested twice.
//
// COULD THEY HAVE BEEN THERE. Whether a person could have been at the scene at the time is read off their sightings under a bound
// on their speed (kinesis's prisms), and "at the scene" and "at the time" are open terms: within a door's few metres, or within the
// grounds; during the expert's window, or a narrower one. Each reading carries its source (authority, convention, analyst, data,
// proposal), and the finding is construed over them (tekmerion's construal): clear, arguable or untenable, with what it turns on.
// A sighting known within a span of time is taken at its middle, its disc widened by the distance the person could cover in half
// the span: an impossibility so found is certain, a possibility is under that widening.
#pragma once

#include <law/facts/facts.hpp>
#include <law/norms/norms.hpp>

#include <apodeixis/interpretation/interpretation.hpp>
#include <archeion/fixity/journal.hpp>
#include <tekmerion/construal/construal.hpp>
#include <tekmerion/scenarios/search.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace law::theories {

namespace ts = tekmerion::scenarios;
namespace tc = tekmerion::construal;
namespace ai = apodeixis::interpretation;
using apodeixis::kernel::Verdict;

// ---- What a theory makes hold -----------------------------------------------------------------------------------------------------

struct Makes {
  std::string kind, role = "actor";
  std::string agent;   // "" : whoever fills the role; "none": the role filled by no one
  std::string fact;
};

struct Grounds {
  std::string item, fact;   // the item, believed, makes the fact hold
};

struct Bindings {
  std::vector<Makes> makes;
  std::vector<Grounds> grounds;
  std::set<std::string> addressed() const {
    std::set<std::string> out;
    for (const auto& m : makes) out.insert(m.fact);
    for (const auto& g : grounds) out.insert(g.fact);
    return out;
  }
};

inline std::set<std::string> holding(const ts::Problem& p, const ts::Scenario& s, const Bindings& b) {
  std::set<std::string> out;
  for (const auto& m : b.makes)
    for (const auto& e : s.events) {
      if (e.kind != m.kind) continue;
      const auto it = e.roles.find(m.role);
      const std::string who = it == e.roles.end() ? "" : it->second;
      if (m.agent.empty() ? !who.empty() : m.agent == "none" ? who.empty() : who == m.agent) out.insert(m.fact);
    }
  for (const auto& g : b.grounds) {
    const long k = p.item(g.item);
    if (k >= 0 && static_cast<std::size_t>(k) < s.stances.size() && s.stances[static_cast<std::size_t>(k)].believed && !p.items[static_cast<std::size_t>(k)].held_back) out.insert(g.fact);
  }
  return out;
}

// The matter's record with the facts the bindings address set by the theory.
inline facts::Record record_under(const facts::Record& matter, const std::set<std::string>& addressed, const std::set<std::string>& holds) {
  facts::Record r = matter;
  std::set<std::string> seen;
  for (auto& f : r.facts) {
    if (!addressed.count(f.id)) continue;
    seen.insert(f.id);
    f.standing = holds.count(f.id) ? facts::Standing::proved : facts::Standing::not_proved;
  }
  for (const auto& id : addressed)
    if (!seen.count(id)) {
      facts::Fact f = facts::fact(id, id);
      f.standing = holds.count(id) ? facts::Standing::proved : facts::Standing::not_proved;
      r.facts.push_back(std::move(f));
    }
  return r;
}

struct Outcome {
  std::string issue;
  ai::Class cls = ai::Class::untenable;
};

// Each question under a theory, on the case the caller makes of a record (norms::subsume with its arguments and holdings).
inline std::vector<Outcome> outcomes(const facts::Record& under, const std::vector<std::string>& issues, const std::vector<std::string>& questions,
                                     const std::function<ai::Case(const facts::Record&)>& make_case) {
  std::vector<Outcome> out;
  const auto c = make_case(under);
  const auto ev = ai::evaluate(c);
  for (std::size_t k = 0; k < issues.size(); k++) {
    Outcome o{issues[k], ai::Class::untenable};
    for (const auto& a : ev.answers)
      if (a.query == questions[k]) o.cls = a.classification;
    out.push_back(o);
  }
  return out;
}

// An answer as a descriptor of a niche: 0 untenable, 1 arguable, 2 clear (3 with no admissible reading).
inline double code(ai::Class c) { return c == ai::Class::untenable ? 0 : c == ai::Class::arguable ? 1 : c == ai::Class::clear ? 2 : 3; }

// ---- Over the theories found ------------------------------------------------------------------------------------------------------

struct Theory {
  heuresis::search::Ticket ticket = 0;
  ts::Scenario scenario;
  ts::Assessment assessment;
  double plausibility = 0;            // relative, over the theories kept
  std::vector<Outcome> outcomes;
  std::set<std::string> holds;
  std::string central;                // what it says of the central slot, in words
  std::string note;                   // the archive's
};

struct Standing {
  std::string issue, question, burden;
  std::vector<std::size_t> follows, arguable, fails;  // indices of theories: clear, arguable, untenable under them
  double for_it = 0, open = 0, against = 0;           // relative plausibility of the theories where it is clear / arguable / untenable
  std::optional<std::size_t> best_for, best_open, best_against, best_alternative;  // the alternative: the best where it is not clear
  ai::Class over = ai::Class::no_admissible;          // over the theories found
  double threshold = 0;
  Verdict verdict;                                    // whether the alternatives are excluded under the threshold
};

inline std::string share(double x) {
  char b[32];
  std::snprintf(b, sizeof b, "%.3g", x);
  return b;
}

// The issues over the theories kept, `threshold` the least relative plausibility of a reasonable alternative. An alternative is a
// theory under which the issue is not clear: untenable, or arguable (turning on how an open term is read).
inline std::vector<Standing> standings(const std::vector<Theory>& theories, const std::vector<std::string>& issues, const std::vector<std::string>& questions,
                                       const std::vector<std::string>& burdens, double threshold) {
  std::vector<Standing> out;
  for (std::size_t k = 0; k < issues.size(); k++) {
    Standing st;
    st.issue = issues[k], st.question = questions[k], st.burden = k < burdens.size() ? burdens[k] : "", st.threshold = threshold;
    const auto better = [&](std::optional<std::size_t>& best, std::size_t t) {
      if (!best || theories[t].plausibility > theories[*best].plausibility) best = t;
    };
    for (std::size_t t = 0; t < theories.size(); t++) {
      const auto c = k < theories[t].outcomes.size() ? theories[t].outcomes[k].cls : ai::Class::untenable;
      if (c == ai::Class::clear) st.follows.push_back(t), st.for_it += theories[t].plausibility, better(st.best_for, t);
      else if (c == ai::Class::arguable) st.arguable.push_back(t), st.open += theories[t].plausibility, better(st.best_open, t), better(st.best_alternative, t);
      else st.fails.push_back(t), st.against += theories[t].plausibility, better(st.best_against, t), better(st.best_alternative, t);
    }
    const std::string claim = "the alternatives to " + issues[k] + " are excluded";
    if (theories.empty()) {
      st.verdict = Verdict::unknown(claim, "no coherent theory was found");
    } else {
      st.over = st.follows.size() == theories.size() ? ai::Class::clear : st.fails.size() == theories.size() ? ai::Class::untenable : ai::Class::arguable;
      if (st.follows.empty()) {
        st.verdict = Verdict::no(claim, st.arguable.empty() ? "it follows under none of the " + std::to_string(theories.size()) + " theories found"
                                                            : "it is clear under none of the theories found; under " + std::to_string(st.arguable.size()) + " it turns on how an open term is read");
      } else if (!st.best_alternative) {
        st.verdict = Verdict::yes(claim, "it follows under every theory found (" + std::to_string(theories.size()) + "); no alternative was found, which is not to say none exists");
      } else {
        const auto& alt = theories[*st.best_alternative];
        const std::string how = alt.outcomes.size() > k && alt.outcomes[k].cls == ai::Class::arguable ? "under which it turns on a reading" : "under which it does not follow";
        if (alt.plausibility >= threshold)
          st.verdict = Verdict::no(claim, "a coherent alternative was found " + how + " (" + alt.central + "), of relative plausibility " + share(alt.plausibility) + ", at least the threshold " + share(threshold))
                           .with("alternative", std::to_string(heuresis::search::serial_of(alt.ticket)));
        else
          st.verdict = Verdict::yes(claim, "the best alternative found (" + alt.central + ") has a relative plausibility of " + share(alt.plausibility) + ", below the threshold " + share(threshold));
      }
    }
    out.push_back(std::move(st));
  }
  return out;
}

// ---- The test on what was held back ----------------------------------------------------------------------------------------------

struct Confirmation {
  heuresis::search::Ticket ticket = 0;
  std::string key, text;
  std::string outcome;                 // admitted, must doubt, no longer holds
  std::vector<std::string> doubted;
  double cost = 0;
  std::size_t tried = 0;
  std::string registered, written;     // the seals of its two entries in the journal
  std::string reason;
};

struct Confirmer {
  archeion::fixity::Journal journal;
  std::vector<Confirmation> done;

  bool tested(const std::string& key) const {
    for (const auto& c : done)
      if (c.key == key) return true;
    return false;
  }

  // Tests a theory once on the held-back items: the test registered (sealed) before they are read, its outcome written after.
  std::optional<Confirmation> confirm(const ts::Problem& p, heuresis::search::Ticket ticket, const ts::Scenario& s, const std::string& when, const std::string& who,
                                     const ts::AssessOptions& o = {}, std::uint64_t key = 20261118) {
    const std::string k = ts::key(s);
    if (tested(k)) return std::nullopt;
    std::vector<std::string> items;
    for (const auto& i : p.items)
      if (i.held_back) items.push_back(i.id);
    if (items.empty()) return std::nullopt;
    Confirmation c;
    c.ticket = ticket, c.key = k, c.text = ts::write(p, s);
    std::string list;
    for (const auto& i : items) list += (list.empty() ? "" : ", ") + i;
    c.registered = journal.append({when, who, "registered a test", "theory #" + std::to_string(heuresis::search::serial_of(ticket)), "on " + list + " (not yet read)"}).seal;
    const auto t = ts::test(p, s, items, o, key);
    c.doubted = t.doubted, c.cost = t.cost, c.tried = t.tried;
    c.outcome = t.admitted ? "admitted" : t.coherent ? "must doubt" : "no longer holds";
    c.reason = t.reason;
    c.written = journal.append({when, who, "wrote a test's outcome", "theory #" + std::to_string(heuresis::search::serial_of(ticket)), c.outcome + ": " + t.reason}).seal;
    done.push_back(c);
    return c;
  }
};

// ---- Could they have been there ----------------------------------------------------------------------------------------------------

struct PlaceReading {
  std::string name;
  tc::Source source = tc::Source::analyst;
  std::string by;
  double radius = 0;   // about the scene's point
  bool stated = false; // the reading the finding is written under (its pivots are measured from it)
};

struct TimeReading {
  std::string name;
  tc::Source source = tc::Source::analyst;
  std::string by;
  double from = 0, until = 0;
  bool stated = false;
};

// A person's fixes from the evidence as it stands: every sighting not held back and not doubted, a span taken at its middle and
// its disc widened by the distance the person could cover in half of it.
inline std::vector<kinesis::prisms::Fix> fixes(const ts::Problem& p, const std::string& agent, const std::set<std::string>& doubted = {}) {
  const long a = p.agent(agent);
  if (a < 0) throw std::invalid_argument("law::theories: no agent " + agent);
  const double v = p.agents[static_cast<std::size_t>(a)].speed;
  std::vector<kinesis::prisms::Fix> out;
  for (const auto& i : p.items)
    if (i.says == ts::Says::sighting && i.agent == agent && !i.held_back && !doubted.count(i.id))
      out.push_back({i.id, i.x, i.y, (i.from + i.until) / 2, i.radius + v * (i.until - i.from) / 2});
  return out;
}

struct Presence {
  tc::Construal construal;
  std::vector<kinesis::prisms::Presence> each;   // under each interpretation, in apodeixis's order
};

inline Presence could_have_been_there(const ts::Problem& p, const std::string& agent, double x, double y, const std::vector<PlaceReading>& places, const std::vector<TimeReading>& times,
                                      const std::set<std::string>& doubted = {}) {
  if (places.empty() || times.empty()) throw std::invalid_argument("law::theories: at least one reading of the place and one of the time");
  const long a = p.agent(agent);
  if (a < 0) throw std::invalid_argument("law::theories: no agent " + agent);
  const double v = p.agents[static_cast<std::size_t>(a)].speed;
  const auto f = fixes(p, agent, doubted);
  const std::string who = p.agents[static_cast<std::size_t>(a)].name.empty() ? agent : p.agents[static_cast<std::size_t>(a)].name;
  tc::Finding finding;
  finding.claim = who + " could have been at the scene at the time";
  tc::Point place{"at the scene", "where the scene is taken to be", {}}, time{"at the time", "when the time is taken to be", {}};
  for (const auto& r : places) place.readings.push_back({r.name, r.source, r.source == tc::Source::proposal ? 'C' : 'A', r.by, "", r.stated});
  for (const auto& r : times) time.readings.push_back({r.name, r.source, r.source == tc::Source::proposal ? 'C' : 'A', r.by, "", r.stated});
  finding.points = {place, time};
  Presence out;
  std::map<std::pair<std::size_t, std::size_t>, kinesis::prisms::Presence> cache;
  finding.judge = [&](const ai::Interpretation& i) -> Verdict {
    const auto& pr = places[i[0]];
    const auto& tr = times[i[1]];
    const std::string claim = who + " could have been within " + share(pr.radius) + " of the scene during the time read as " + tr.name;
    if (f.empty()) return Verdict::yes(claim, "nothing places " + who + " anywhere: every place is open");
    auto it = cache.find({i[0], i[1]});
    if (it == cache.end()) it = cache.emplace(std::pair{i[0], i[1]}, kinesis::prisms::presence(f, v, {x, y, pr.radius, tr.from, tr.until})).first;
    const auto& id = it->second.identified;
    if (std::find(id.values.begin(), id.values.end(), "in") != id.values.end()) return Verdict::yes(claim, id.reason);
    if (id.complete) return Verdict::no(claim, id.reason);
    return Verdict::unknown(claim, id.reason);
  };
  out.construal = tc::construe(finding);
  const std::size_t n = ai::space(out.construal.points);
  for (std::size_t x2 = 0; x2 < n; x2++) {
    const auto i = ai::decode(out.construal.points, x2);
    out.each.push_back(cache.count({i[0], i[1]}) ? cache.at({i[0], i[1]}) : kinesis::prisms::Presence{});
  }
  return out;
}

}  // namespace law::theories
