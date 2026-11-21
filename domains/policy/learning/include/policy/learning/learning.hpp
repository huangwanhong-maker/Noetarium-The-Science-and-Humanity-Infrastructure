// Knowledge and learning programmes: from the needs of a cohort to the actions its members take, and what can be claimed of the
// programme's part in them (ADR 0144).
//
// A programme brings a cohort of participants (cities, agencies, teams) through activities of declared formats (an assessment of
// needs, peer exchanges, workshops, a study visit, advisory support, follow-up) to action plans of their own. What it did is read
// on four records, each kept as declared and never summed into one score:
//
//  - needs and their coverage: every need named (from a diagnosis, a scorecard's weak statements, or a participant's own words),
//    the activities that addressed it, and the needs no activity reached;
//  - action plans at a date: each action completed on time or late, in progress, overdue, not yet due, or dropped, by its dated
//    updates; each participant's share completed; the time from planning to completion read by Kaplan and Meier's estimator
//    (peira's survival), open actions censored at the date, so that a cohort's pace is not judged by those who finished first;
//  - results on a declared ladder (Kirkpatrick's four levels, a capacity framework's outcomes, or another the programme states),
//    each a statement with its source and a grade of evidence on a declared scale; per participant the highest level evidenced at
//    or above a grade the programme declares sufficient. Levels are ordinal: they are counted, never averaged;
//  - contribution claims (Mayne 2001, "Addressing attribution through contribution analysis", Canadian Journal of Program
//    Evaluation 16(1), 1–24): a claimed result, the chain of steps from the programme's activities to it, each step with its
//    evidence grade, and the rival explanations considered, each addressed by evidence or not. A chain is no better shown than its
//    weakest step (the change module's Proposition 1, computed by `change::assess` on the chain as a theory of change), and a
//    claim is substantiated when its weakest step reaches the declared grade and every rival is addressed.
//
// Formats are compared by what they contributed: each substantiated claim's credit is shared equally among the distinct formats
// of the activities on its chain, summed per format and set against the format's cost; the formats that no other does better
// for less are its frontier. Credit shared equally is a declared rule, and the comparison claims no more than the claims do.
//
// Dates are day numbers on any one scale the caller keeps (the API reads ISO dates). Nothing here judges whether a programme was
// worth doing.
#pragma once

#include <policy/change/change.hpp>

#include <peira/survival/survival.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace policy::learning {

inline constexpr double never = std::numeric_limits<double>::quiet_NaN();

struct Participant {
  std::string id, name, group;
};

struct Need {
  std::string id, participant, text, theme, source;  // source: the diagnosis it came from
  int priority = 0;                                    // larger first; declared
};

struct Activity {
  std::string id, format, title;
  double date = never;
  double cost = 0;
  std::vector<std::string> participants;  // empty: the whole cohort
  std::vector<std::string> needs;         // the needs it addressed
};

struct Update {
  double date = never;
  std::string state;  // "started", "completed", "dropped", "revised"
  std::string note, source;
};

struct Action {
  std::string id, participant, need, text, owner;
  double planned = never, due = never;  // when it entered the plan; when it is due
  std::vector<Update> updates;          // in any order; read by date
};

struct Result {
  std::string id, participant, level, text, source, grade;
  double date = never;
  std::vector<std::string> activities;  // the programme's activities the statement names, if any
};

struct Step {
  std::string from, to;  // an activity's id, a result's id, or an intermediate state's words
  std::string grade;     // the grade of the evidence for this step; empty: none
  std::string evidence;  // what it rests on
};

struct Rival {
  std::string explanation;
  std::string grade;     // the grade of the evidence that addresses it; empty: not addressed
  std::string evidence;
};

struct Claim {
  std::string id, result;  // the result claimed
  std::vector<Step> chain; // from an activity to the result, in order
  std::vector<Rival> rivals;
};

struct Ask {
  std::string id, participant, text;
  double date = never, answered = never;
  std::string answer;
};

struct Programme {
  std::string name;
  std::vector<std::string> ladder;   // the levels of results, lowest first
  std::vector<std::string> grades;   // the grades of evidence, lowest first
  std::string sufficient;            // the least grade that counts a result or substantiates a step
  std::vector<Participant> participants;
  std::vector<Need> needs;
  std::vector<Activity> activities;
  std::vector<Action> actions;
  std::vector<Result> results;
  std::vector<Claim> claims;
  std::vector<Ask> asks;
};

// ---- checks of a programme ----------------------------------------------------------------------------------------------------

namespace detail {

template <class T>
const T* find(const std::vector<T>& v, const std::string& id) {
  for (const auto& x : v)
    if (x.id == id) return &x;
  return nullptr;
}

inline long rank(const std::vector<std::string>& scale, const std::string& g, const std::string& what) {
  if (g.empty()) return -1;
  const auto it = std::find(scale.begin(), scale.end(), g);
  if (it == scale.end()) throw std::invalid_argument("policy::learning: " + what + " '" + g + "' is not on the declared scale");
  return static_cast<long>(it - scale.begin());
}

}  // namespace detail

// Refuses a programme whose records name what it does not hold, or whose scales are not declared.
inline void validate(const Programme& p) {
  if (p.ladder.empty() || p.grades.empty()) throw std::invalid_argument("policy::learning: a programme declares its ladder of results and its grades of evidence");
  detail::rank(p.grades, p.sufficient, "the sufficient grade");
  std::set<std::string> ids;
  for (const auto& x : p.participants)
    if (!ids.insert(x.id).second) throw std::invalid_argument("policy::learning: two participants with the id " + x.id);
  auto participant = [&](const std::string& id, const std::string& who) {
    if (!detail::find(p.participants, id)) throw std::invalid_argument("policy::learning: " + who + " names a participant the programme does not have: " + id);
  };
  for (const auto& n : p.needs) participant(n.participant, "the need " + n.id);
  for (const auto& a : p.activities) {
    for (const auto& x : a.participants) participant(x, "the activity " + a.id);
    for (const auto& n : a.needs)
      if (!detail::find(p.needs, n)) throw std::invalid_argument("policy::learning: the activity " + a.id + " addresses a need the programme does not have: " + n);
    if (a.cost < 0) throw std::invalid_argument("policy::learning: the activity " + a.id + " has a negative cost");
  }
  for (const auto& a : p.actions) {
    participant(a.participant, "the action " + a.id);
    if (!a.need.empty() && !detail::find(p.needs, a.need)) throw std::invalid_argument("policy::learning: the action " + a.id + " answers a need the programme does not have: " + a.need);
    for (const auto& u : a.updates)
      if (u.state != "started" && u.state != "completed" && u.state != "dropped" && u.state != "revised")
        throw std::invalid_argument("policy::learning: an update of " + a.id + " is started, completed, dropped or revised, not '" + u.state + "'");
  }
  for (const auto& r : p.results) {
    participant(r.participant, "the result " + r.id);
    detail::rank(p.ladder, r.level, "the level of " + r.id);
    detail::rank(p.grades, r.grade, "the grade of " + r.id);
  }
  for (const auto& c : p.claims) {
    if (!detail::find(p.results, c.result)) throw std::invalid_argument("policy::learning: the claim " + c.id + " claims a result the programme does not have: " + c.result);
    if (c.chain.empty()) throw std::invalid_argument("policy::learning: the claim " + c.id + " has no chain of steps");
    if (!detail::find(p.activities, c.chain.front().from)) throw std::invalid_argument("policy::learning: the chain of " + c.id + " begins with no activity of the programme");
    if (c.chain.back().to != c.result) throw std::invalid_argument("policy::learning: the chain of " + c.id + " does not end at the result it claims");
    for (std::size_t k = 0; k + 1 < c.chain.size(); k++)
      if (c.chain[k].to != c.chain[k + 1].from) throw std::invalid_argument("policy::learning: the chain of " + c.id + " breaks after " + c.chain[k].to);
    for (const auto& s : c.chain) detail::rank(p.grades, s.grade, "the grade of a step of " + c.id);
    for (const auto& r : c.rivals) detail::rank(p.grades, r.grade, "the grade addressing a rival of " + c.id);
  }
  for (const auto& a : p.asks) participant(a.participant, "the ask " + a.id);
}

// ---- needs and their coverage ---------------------------------------------------------------------------------------------------

struct NeedCoverage {
  std::string need, participant;
  int priority = 0;
  std::vector<std::string> activities;  // that addressed it and included its participant
};

struct Coverage {
  std::vector<NeedCoverage> needs;
  std::vector<std::string> unmet;                  // needs no activity reached, highest priority first
  std::map<std::string, double> share_by_participant;  // the share of each participant's needs reached
};

inline Coverage coverage(const Programme& p) {
  validate(p);
  Coverage c;
  std::map<std::string, std::pair<int, int>> counts;
  for (const auto& n : p.needs) {
    NeedCoverage x{n.id, n.participant, n.priority, {}};
    for (const auto& a : p.activities) {
      const bool includes = a.participants.empty() || std::find(a.participants.begin(), a.participants.end(), n.participant) != a.participants.end();
      if (includes && std::find(a.needs.begin(), a.needs.end(), n.id) != a.needs.end()) x.activities.push_back(a.id);
    }
    auto& k = counts[n.participant];
    k.second++;
    if (!x.activities.empty()) k.first++;
    c.needs.push_back(x);
  }
  std::vector<const NeedCoverage*> un;
  for (const auto& x : c.needs)
    if (x.activities.empty()) un.push_back(&x);
  std::stable_sort(un.begin(), un.end(), [](const NeedCoverage* a, const NeedCoverage* b) { return a->priority > b->priority; });
  for (const auto* x : un) c.unmet.push_back(x->need);
  for (const auto& [who, k] : counts) c.share_by_participant[who] = k.second ? static_cast<double>(k.first) / k.second : 0;
  return c;
}

// ---- action plans at a date ------------------------------------------------------------------------------------------------------

enum class State { completed_on_time, completed_late, in_progress, overdue, not_yet_due, not_started, dropped };

inline const char* name(State s) {
  switch (s) {
    case State::completed_on_time: return "completed on time";
    case State::completed_late: return "completed late";
    case State::in_progress: return "in progress";
    case State::overdue: return "overdue";
    case State::not_yet_due: return "not yet due";
    case State::not_started: return "not started";
    case State::dropped: return "dropped";
  }
  return "?";
}

struct ActionState {
  std::string action, participant;
  State state = State::not_started;
  double completed = never;  // the date it was completed, if it was by the date read
  std::string why;
};

struct Actions {
  double date = never;
  std::vector<ActionState> actions;
  std::map<std::string, double> completed_share;  // per participant, among the actions not dropped
  peira::survival::KaplanMeier pace;              // days from planning to completion, open actions censored at the date
  std::size_t with_plan_date = 0;
};

inline ActionState state_at(const Action& a, double date) {
  ActionState s{a.id, a.participant, State::not_started, never, ""};
  std::vector<Update> u;
  for (const auto& x : a.updates)
    if (std::isfinite(x.date) && x.date <= date) u.push_back(x);
  std::stable_sort(u.begin(), u.end(), [](const Update& x, const Update& y) { return x.date < y.date; });
  bool started = false;
  for (const auto& x : u) {
    if (x.state == "dropped") {
      s.state = State::dropped, s.why = "dropped on day " + change::number(x.date) + (x.note.empty() ? "" : ": " + x.note);
      return s;
    }
    if (x.state == "completed") {
      s.completed = x.date;
      const bool late = std::isfinite(a.due) && x.date > a.due;
      s.state = late ? State::completed_late : State::completed_on_time;
      s.why = std::string(late ? "completed late" : "completed") + " on day " + change::number(x.date) + (x.source.empty() ? "" : ", by " + x.source);
      return s;
    }
    if (x.state == "started" || x.state == "revised") started = true;
  }
  const bool past_due = std::isfinite(a.due) && date > a.due;
  if (past_due) s.state = State::overdue, s.why = std::string(started ? "started" : "not started") + ", due on day " + change::number(a.due);
  else if (started) s.state = State::in_progress, s.why = "started";
  else s.state = std::isfinite(a.due) ? State::not_yet_due : State::not_started, s.why = std::isfinite(a.due) ? "due on day " + change::number(a.due) : "no update and no date due";
  return s;
}

inline Actions actions_at(const Programme& p, double date) {
  validate(p);
  if (!std::isfinite(date)) throw std::invalid_argument("policy::learning: a date to read the action plans at");
  Actions out;
  out.date = date;
  std::map<std::string, std::pair<int, int>> k;
  std::vector<double> times;
  std::vector<int> events;
  for (const auto& a : p.actions) {
    const auto s = state_at(a, date);
    out.actions.push_back(s);
    if (s.state != State::dropped) {
      auto& c = k[a.participant];
      c.second++;
      if (std::isfinite(s.completed)) c.first++;
    }
    // Pace: from planning to completion; open actions censored at the date; dropped ones censored when dropped.
    if (std::isfinite(a.planned) && a.planned <= date) {
      out.with_plan_date++;
      if (std::isfinite(s.completed)) times.push_back(std::max(0.0, s.completed - a.planned)), events.push_back(1);
      else if (s.state == State::dropped) {
        double when = date;  // the first time it was dropped
        for (const auto& x : a.updates)
          if (x.state == "dropped" && x.date <= date) when = std::min(when, x.date);
        times.push_back(std::max(0.0, when - a.planned)), events.push_back(0);
      } else times.push_back(std::max(0.0, date - a.planned)), events.push_back(0);
    }
  }
  for (const auto& [who, c] : k) out.completed_share[who] = c.second ? static_cast<double>(c.first) / c.second : 0;
  if (!times.empty()) out.pace = peira::survival::kaplan_meier(times, events);
  return out;
}

// ---- results on the ladder --------------------------------------------------------------------------------------------------------

struct ParticipantLadder {
  std::string participant;
  long highest = -1;                 // the highest level evidenced at or above the sufficient grade; −1: none
  std::vector<std::size_t> results;  // indices of the results that count
};

struct Ladder {
  std::vector<ParticipantLadder> participants;
  std::vector<std::size_t> reached;  // [level]: participants whose highest level is it
  std::size_t none = 0;              // participants with no result that counts
  std::size_t set_aside = 0;         // results below the sufficient grade
};

inline Ladder ladder(const Programme& p) {
  validate(p);
  Ladder L;
  L.reached.assign(p.ladder.size(), 0);
  const long floor = detail::rank(p.grades, p.sufficient, "the sufficient grade");
  for (const auto& who : p.participants) {
    ParticipantLadder x{who.id, -1, {}};
    for (std::size_t k = 0; k < p.results.size(); k++) {
      const auto& r = p.results[k];
      if (r.participant != who.id) continue;
      if (detail::rank(p.grades, r.grade, "a grade") < floor) continue;
      x.results.push_back(k);
      x.highest = std::max(x.highest, detail::rank(p.ladder, r.level, "a level"));
    }
    if (x.highest < 0) L.none++;
    else L.reached[static_cast<std::size_t>(x.highest)]++;
    L.participants.push_back(x);
  }
  for (const auto& r : p.results)
    if (detail::rank(p.grades, r.grade, "a grade") < floor) L.set_aside++;
  return L;
}

// ---- contribution claims ---------------------------------------------------------------------------------------------------------

struct ClaimStanding {
  std::string claim, result;
  long weakest = change::not_shown;  // the grade of the chain's weakest step, or not shown
  std::string limiting;              // the step that limits it
  std::vector<std::string> unaddressed;  // rivals not addressed at the sufficient grade
  bool substantiated = false;
  std::vector<std::string> formats;  // the distinct formats of the activities on its chain
  std::string why;
};

inline ClaimStanding standing(const Programme& p, const Claim& c) {
  // The chain as a theory of change whose links are shown at their evidence's grade (or not at all), read by change::assess.
  change::Theory t;
  t.name = c.id;
  t.levels = p.grades;
  std::vector<std::string> nodes;
  auto node = [&](const std::string& id) {
    if (std::find(nodes.begin(), nodes.end(), id) == nodes.end()) nodes.push_back(id), t.nodes.push_back({id, "state", id});
  };
  for (const auto& s : c.chain) {
    node(s.from), node(s.to);
    change::Link l;
    l.from = s.from, l.to = s.to, l.assumption = s.evidence;
    if (!s.grade.empty()) l.evidence = {true, 1.0, 1.0, 1.0, s.grade};
    t.links.push_back(l);
  }
  const auto a = change::assess(t, c.chain.front().from, c.chain.back().to, p.sufficient);
  ClaimStanding out;
  out.claim = c.id, out.result = c.result;
  out.weakest = a.strongest.weakest;
  if (!a.strongest.links.empty()) {
    const auto& l = t.links[a.strongest.links[a.strongest.limiting]];
    out.limiting = l.from + " -> " + l.to;
  }
  const long floor = detail::rank(p.grades, p.sufficient, "the sufficient grade");
  for (const auto& r : c.rivals)
    if (detail::rank(p.grades, r.grade, "a grade") < floor) out.unaddressed.push_back(r.explanation);
  out.substantiated = out.weakest >= floor && out.unaddressed.empty();
  std::set<std::string> fs;
  for (const auto& s : c.chain)
    if (const auto* act = detail::find(p.activities, s.from)) fs.insert(act->format);
  out.formats.assign(fs.begin(), fs.end());
  if (out.substantiated) out.why = "substantiated: every step at " + p.sufficient + " or above, every rival addressed";
  else if (out.weakest < floor) out.why = "not substantiated: the step " + out.limiting + (out.weakest == change::not_shown ? " has no evidence" : " is shown only at " + p.grades[static_cast<std::size_t>(out.weakest)]);
  else out.why = "not substantiated: " + std::to_string(out.unaddressed.size()) + " rival explanation(s) not addressed";
  return out;
}

// ---- formats compared -----------------------------------------------------------------------------------------------------------

struct FormatRow {
  std::string format;
  double cost = 0;
  std::size_t activities = 0;
  double credit = 0;             // substantiated claims' credit, shared equally among the formats on each chain
  double credit_per_cost = 0;    // NaN when the cost is zero
  bool frontier = false;         // no other format has at least its credit for at most its cost, and better in one
};

inline std::vector<FormatRow> formats(const Programme& p) {
  validate(p);
  std::map<std::string, FormatRow> m;
  for (const auto& a : p.activities) {
    auto& r = m[a.format];
    r.format = a.format, r.cost += a.cost, r.activities++;
  }
  for (const auto& c : p.claims) {
    const auto s = standing(p, c);
    if (!s.substantiated || s.formats.empty()) continue;
    for (const auto& f : s.formats) m[f].credit += 1.0 / static_cast<double>(s.formats.size());
  }
  std::vector<FormatRow> out;
  for (auto& [f, r] : m) r.credit_per_cost = r.cost > 0 ? r.credit / r.cost : never, out.push_back(r);
  for (auto& r : out) {
    bool beaten = false;
    for (const auto& o : out)
      beaten = beaten || (&o != &r && o.credit >= r.credit && o.cost <= r.cost && (o.credit > r.credit || o.cost < r.cost));
    r.frontier = !beaten;
  }
  return out;
}

// ---- asks ---------------------------------------------------------------------------------------------------------------------

struct Asks {
  std::size_t asked = 0, answered = 0;
  double median_days = never;  // to an answer, among those answered by the date
  std::vector<std::string> open;
};

inline Asks asks_at(const Programme& p, double date) {
  validate(p);
  Asks out;
  std::vector<double> days;
  for (const auto& a : p.asks) {
    if (!(std::isfinite(a.date) && a.date <= date)) continue;
    out.asked++;
    if (std::isfinite(a.answered) && a.answered <= date) out.answered++, days.push_back(a.answered - a.date);
    else out.open.push_back(a.id);
  }
  if (!days.empty()) {
    std::sort(days.begin(), days.end());
    const std::size_t n = days.size();
    out.median_days = n % 2 ? days[n / 2] : 0.5 * (days[n / 2 - 1] + days[n / 2]);
  }
  return out;
}

}  // namespace policy::learning
