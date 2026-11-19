// Review cycles: recommendations, responses, implementation and its evidence (ADR 0134).
//
// Peer review is the systematic assessment of one party's performance by others, whose force comes from recommendations,
// dialogue, public scrutiny and comparison, not from sanction (Pagani 2002). The Universal Periodic Review reviews every member of
// the United Nations in cycles; recommendations made by reviewing states are recorded as supported or noted by the state reviewed,
// and their implementation is reported at the next cycle (Human Rights Council resolution 5/1, 2007; the survey's §A.3.4). Treaty
// bodies, the OECD's peer reviews and partnerships' reviews follow the same loop. A record of it makes these readings:
//
//   state       a recommendation's state at a date: unanswered, noted, rejected, supported and unreported, or supported with its
//               implementation reported (implemented, in part, not implemented);
//   evidence    an implementation said to be complete stands as evidenced when a source other than the party reviewed reports it
//               with evidence named, as claimed when only the party reviewed says so, as contested when the reports disagree. A
//               claim is not a finding: the party reviewed is a party to its own report;
//   follow-up   a recommendation of a later cycle that repeats an earlier one, declared so or matched by who recommended it to whom
//               on which theme: repetition is the reviewers' reading that the earlier one is not done;
//   measures    for a party reviewed and a cycle, the recommendations by response and by state, each a count and a share, never
//               combined into a score;
//   signs       a recommendation unanswered after its cycle closed, one supported and unreported by the next cycle, an
//               implementation claimed only by the party reviewed, one claimed done and recommended again, one contested; and, when
//               the record grades recommendations by how specific an action they ask for (as UPR Info's five levels do), pairs of
//               parties that recommend each other only mildly (cf. Terman and Voeten 2018, who found that partners give softer
//               recommendations).
//
// Dates are ISO 8601 calendar dates. Nothing here is any review's record until a caller states it.
//
// References. Pagani, F. (2002), Peer Review as a Tool for Co-operation and Change, OECD, SG/LEG(2002)1. Human Rights Council,
// resolution 5/1, Institution-building of the United Nations Human Rights Council, 18 June 2007. Terman, R. and Voeten, E. (2018),
// The relational politics of shame: evidence from the universal periodic review, Review of International Organizations 13, 1–23.
#pragma once

#include <archeion/retention/dates.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::reviews {

namespace ar = archeion::retention;

enum class Response { supported, noted, rejected, none };

inline const char* name(Response r) {
  switch (r) {
    case Response::supported: return "supported";
    case Response::noted: return "noted";
    case Response::rejected: return "rejected";
    default: return "no response";
  }
}

inline Response response_of(const std::string& s) {
  for (Response r : {Response::supported, Response::noted, Response::rejected, Response::none})
    if (s == name(r)) return r;
  throw std::invalid_argument("diplomacy::reviews: no response is called " + s);
}

enum class Done { implemented, partly, not_implemented };

inline const char* name(Done d) { return d == Done::implemented ? "implemented" : d == Done::partly ? "in part" : "not implemented"; }

inline Done done_of(const std::string& s) {
  for (Done d : {Done::implemented, Done::partly, Done::not_implemented})
    if (s == name(d)) return d;
  throw std::invalid_argument("diplomacy::reviews: no state of implementation is called " + s);
}

struct Cycle {
  std::string id, opened, closed;
};

struct Recommendation {
  std::string id, cycle, from, to, text, theme;
  int level = 0;         // how specific an action it asks for, on the record's declared scale (0: not graded)
  std::string repeats;   // a recommendation of an earlier cycle it repeats, when declared
};

struct Answer {
  std::string recommendation, date, by;
  Response response = Response::none;
};

struct Progress {
  std::string recommendation, date, by;
  Done done = Done::not_implemented;
  std::vector<std::string> evidence;
};

struct Record {
  std::vector<Cycle> cycles;
  std::vector<Recommendation> recommendations;
  std::vector<Answer> answers;
  std::vector<Progress> progress;
  const Recommendation* recommendation(const std::string& id) const {
    for (const auto& r : recommendations)
      if (r.id == id) return &r;
    return nullptr;
  }
  const Cycle* cycle(const std::string& id) const {
    for (const auto& c : cycles)
      if (c.id == id) return &c;
    return nullptr;
  }
};

struct Problem {
  std::string where, what;
};

inline std::vector<Problem> problems(const Record& r) {
  std::vector<Problem> out;
  std::set<std::string> cycles, recs;
  for (const auto& c : r.cycles) {
    if (!cycles.insert(c.id).second) out.push_back({"cycle " + c.id, "declared twice"});
    if (!ar::is_date(c.opened) || (!c.closed.empty() && (!ar::is_date(c.closed) || c.closed < c.opened)))
      out.push_back({"cycle " + c.id, "it opens on no calendar date, or closes before it opens"});
  }
  for (const auto& x : r.recommendations) {
    const std::string where = "recommendation " + x.id;
    if (!recs.insert(x.id).second) out.push_back({where, "declared twice"});
    if (!cycles.count(x.cycle)) out.push_back({where, "its cycle " + x.cycle + " is not recorded"});
    if (x.from.empty() || x.to.empty()) out.push_back({where, "it names no one recommending or no one reviewed"});
    if (x.from == x.to && !x.from.empty()) out.push_back({where, "a party recommends to itself"});
    if (x.level < 0) out.push_back({where, "its level is negative"});
  }
  for (const auto& x : r.recommendations)
    if (!x.repeats.empty()) {
      const Recommendation* e = r.recommendation(x.repeats);
      if (!e) out.push_back({"recommendation " + x.id, "it repeats " + x.repeats + ", which is not recorded"});
      else if (const Cycle *a = r.cycle(e->cycle), *b = r.cycle(x.cycle); a && b && !(a->opened < b->opened))
        out.push_back({"recommendation " + x.id, "it repeats " + x.repeats + ", which is of no earlier cycle"});
    }
  for (const auto& a : r.answers) {
    if (!recs.count(a.recommendation)) out.push_back({"answer to " + a.recommendation, "no such recommendation"});
    if (!ar::is_date(a.date)) out.push_back({"answer to " + a.recommendation, "it is dated by no calendar date"});
  }
  for (const auto& p : r.progress) {
    if (!recs.count(p.recommendation)) out.push_back({"progress on " + p.recommendation, "no such recommendation"});
    if (!ar::is_date(p.date)) out.push_back({"progress on " + p.recommendation, "it is dated by no calendar date"});
  }
  return out;
}

// ---- A recommendation's state ------------------------------------------------------------------------------------------------

enum class Standing { none, claimed, evidenced, contested };

inline const char* name(Standing s) {
  switch (s) {
    case Standing::claimed: return "claimed";
    case Standing::evidenced: return "evidenced";
    case Standing::contested: return "contested";
    default: return "not reported";
  }
}

struct State {
  std::string recommendation;
  Response response = Response::none;
  bool reported = false;
  Done done = Done::not_implemented;  // the latest report's, when reported
  Standing standing = Standing::none; // of the claim that it is implemented
  std::string words;
};

inline State state(const Record& r, const std::string& id, const std::string& at) {
  const Recommendation* x = r.recommendation(id);
  if (!x) throw std::invalid_argument("diplomacy::reviews: no recommendation " + id);
  State s;
  s.recommendation = id;
  std::string last;
  for (const auto& a : r.answers)
    if (a.recommendation == id && a.date <= at && a.date >= last) s.response = a.response, last = a.date;
  // Each reporter's latest report: a reporter who changes its account is read at its latest; reporters who disagree contest.
  std::string latest;
  std::map<std::string, const Progress*> by;
  for (const auto& p : r.progress) {
    if (p.recommendation != id || p.date > at) continue;
    s.reported = true;
    if (p.date >= latest) s.done = p.done, latest = p.date;
    auto& q = by[p.by];
    if (!q || p.date >= q->date) q = &p;
  }
  bool done = false, not_done = false, evidenced = false;
  for (const auto& [who, p] : by) {
    if (p->done != Done::implemented) {
      not_done = true;
      continue;
    }
    done = true;
    if (who != x->to && !p->evidence.empty()) evidenced = true;
  }
  if (done && not_done) s.standing = Standing::contested;
  else if (evidenced) s.standing = Standing::evidenced;
  else if (done) s.standing = Standing::claimed;
  if (s.response == Response::none) s.words = "unanswered";
  else if (s.response != Response::supported) s.words = name(s.response);
  else if (!s.reported) s.words = "supported, its implementation not reported";
  else s.words = std::string("supported, ") + name(s.done) + (s.done == Done::implemented || s.standing == Standing::contested ? std::string(" (") + name(s.standing) + ")" : "");
  return s;
}

// ---- Follow-up -------------------------------------------------------------------------------------------------------------------

struct Repeat {
  std::string later, earlier;
  bool declared = false;
};

// The recommendations of later cycles that repeat earlier ones: declared, or matched by who recommends to whom on which theme.
inline std::vector<Repeat> repeats(const Record& r) {
  std::vector<Repeat> out;
  for (const auto& x : r.recommendations) {
    if (!x.repeats.empty()) {
      out.push_back({x.id, x.repeats, true});
      continue;
    }
    const Cycle* cx = r.cycle(x.cycle);
    if (!cx || x.theme.empty()) continue;
    const Recommendation* best = nullptr;
    std::string best_open;
    for (const auto& y : r.recommendations) {
      const Cycle* cy = r.cycle(y.cycle);
      if (!cy || !(cy->opened < cx->opened) || y.from != x.from || y.to != x.to || y.theme != x.theme) continue;
      if (!best || cy->opened > best_open) best = &y, best_open = cy->opened;
    }
    if (best) out.push_back({x.id, best->id, false});
  }
  return out;
}

// ---- Measures -------------------------------------------------------------------------------------------------------------------

struct Measures {
  std::string reviewed, cycle;
  std::size_t recommendations = 0;
  std::map<std::string, std::size_t> by_response, by_state;
  double share_supported = 0;  // of those answered
};

inline Measures measures(const Record& r, const std::string& reviewed, const std::string& cycle, const std::string& at) {
  Measures m;
  m.reviewed = reviewed, m.cycle = cycle;
  std::size_t answered = 0, supported = 0;
  for (const auto& x : r.recommendations) {
    if (x.to != reviewed || x.cycle != cycle) continue;
    m.recommendations++;
    const State s = state(r, x.id, at);
    m.by_response[name(s.response)]++;
    m.by_state[s.words]++;
    if (s.response != Response::none) answered++, supported += s.response == Response::supported;
  }
  m.share_supported = answered ? static_cast<double>(supported) / static_cast<double>(answered) : 0.0;
  return m;
}

// ---- Signs -----------------------------------------------------------------------------------------------------------------------

struct Sign {
  std::string kind, where, what;
};

struct Thresholds {
  long answer_within = 90;  // days after a cycle closes by which a recommendation should be answered
  int mild = 0;             // levels at or below which a recommendation is mild (0: not read)
};

inline std::vector<Sign> signs(const Record& r, const std::string& at, const Thresholds& th = {}) {
  if (const auto ps = problems(r); !ps.empty()) throw std::invalid_argument("diplomacy::reviews: " + ps[0].where + ": " + ps[0].what);
  std::vector<Sign> out;
  std::map<std::string, std::string> next_opens;  // cycle → when the next cycle opens
  for (const auto& c : r.cycles)
    for (const auto& d : r.cycles)
      if (d.opened > c.opened && (next_opens[c.id].empty() || d.opened < next_opens[c.id])) next_opens[c.id] = d.opened;
  const auto reps = repeats(r);
  for (const auto& x : r.recommendations) {
    const std::string where = "recommendation " + x.id;
    const Cycle* c = r.cycle(x.cycle);
    const State s = state(r, x.id, at);
    if (s.response == Response::none && c && !c->closed.empty() && ar::add_days(c->closed, th.answer_within) <= at)
      out.push_back({"unanswered", where, "its cycle closed on " + c->closed + " and it is unanswered"});
    if (s.response == Response::supported && !s.reported && !next_opens[x.cycle].empty() && next_opens[x.cycle] <= at)
      out.push_back({"unreported", where, "supported, and nothing is reported of it by the next cycle (" + next_opens[x.cycle] + ")"});
    if (s.standing == Standing::claimed) {
      bool only_self = true;
      for (const auto& p : r.progress)
        if (p.recommendation == x.id && p.date <= at && p.done == Done::implemented && p.by != x.to) only_self = false;
      if (only_self) out.push_back({"claimed", where, "its implementation is claimed only by " + x.to + ", the party reviewed"});
    }
    if (s.standing == Standing::contested) out.push_back({"contested", where, "the reports of its implementation disagree"});
    for (const auto& rp : reps)
      if (rp.earlier == x.id) {
        const Recommendation* later = r.recommendation(rp.later);
        const Cycle* lc = later ? r.cycle(later->cycle) : nullptr;
        if (lc && lc->opened <= at && state(r, x.id, lc->opened).done == Done::implemented && state(r, x.id, lc->opened).reported)
          out.push_back({"recommended again", where, "said to be implemented, and recommended again as " + rp.later});
      }
  }
  if (th.mild > 0) {
    std::map<std::pair<std::string, std::string>, std::pair<int, int>> pair;  // (a, b) → (most specific level a gave b, count)
    for (const auto& x : r.recommendations) {
      if (x.level <= 0) continue;
      auto& p = pair[{x.from, x.to}];
      p.first = std::max(p.first, x.level), p.second++;
    }
    for (const auto& [ab, p] : pair) {
      if (!(ab.first < ab.second)) continue;
      auto back = pair.find({ab.second, ab.first});
      if (back == pair.end()) continue;
      if (p.first <= th.mild && back->second.first <= th.mild)
        out.push_back({"mutually mild", "parties " + ab.first + " and " + ab.second,
                       "they recommend each other only at levels up to " + std::to_string(th.mild) + " (" + std::to_string(p.second) + " and " + std::to_string(back->second.second) + " recommendations)"});
    }
  }
  return out;
}

}  // namespace diplomacy::reviews
