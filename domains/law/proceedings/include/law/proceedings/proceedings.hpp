// Proceedings: a docket kept as a sealed journal, its steps held to declared rules of procedure, and the standing of facts moved by
// them.
//
// A proceeding is a sequence of steps (a claim filed, served, defended, heard, a fact admitted or found, judgment given, an
// appeal), each taken by a party on a date and concerning some facts of the record. The docket is kept in archeion's journal: every
// step an entry sealed to the one before, so that a step changed, removed or put out of order afterwards is found at its place,
// and a docket whose last seal was kept elsewhere cannot lose its last steps unseen.
//
// The rules of procedure are declared, as no jurisdiction's rules are the library's (theory/facts-and-their-registers.md): for
// each kind of step, the kinds that must come before it, the roles that may take it, whether it may be taken once only, a period
// in days from an earlier kind within which it must be taken, and the standing it gives the facts it concerns (law::facts, whose
// can_become says which changes of standing there are). review() holds a docket to its rules step by step and says every fault
// with its place; state() says, at a date, what may be taken next, what is due and by when, and what is overdue; apply() moves
// the record's facts through the steps, refusing a change of standing the facts do not allow.
//
// Dates are the first ten characters of an entry's instant (YYYY-MM-DD), counted in days by archeion's dates.

#pragma once

#include <apodeixis/kernel/verdict.hpp>
#include <archeion/fixity/journal.hpp>
#include <archeion/retention/dates.hpp>
#include <law/facts/facts.hpp>

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace law::proceedings {

using apodeixis::kernel::Verdict;
namespace fx = archeion::fixity;
namespace dt = archeion::retention;

struct Kind {
  std::string name;
  std::vector<std::string> after;               // the kinds that must each have been taken before
  std::vector<std::string> by;                  // the roles that may take it; empty: any party
  bool once = false;
  std::string from;                             // the kind from whose last step its period runs
  long within = -1;                             // the period in days; −1: none
  std::optional<law::facts::Standing> moves;    // the standing it gives the facts it concerns
};

struct Rules {
  std::string name;
  std::vector<Kind> kinds;

  const Kind* kind(const std::string& n) const {
    for (const auto& k : kinds)
      if (k.name == n) return &k;
    return nullptr;
  }
};

struct Party {
  std::string name, role;
};

struct Step {
  std::string when;                // a date, or an instant beginning with one
  std::string who;                 // a party
  std::string kind;
  std::vector<std::string> facts;  // the facts of the record it concerns
  std::string note;
};

struct Docket {
  std::string proceeding;
  std::vector<Party> parties;
  fx::Journal journal;
};

namespace detail {

inline std::string join(const std::vector<std::string>& v, const char* by = ",") {
  std::string s;
  for (std::size_t i = 0; i < v.size(); i++) s += (i ? by : "") + v[i];
  return s;
}

inline std::vector<std::string> split(const std::string& s) {
  std::vector<std::string> out;
  std::size_t from = 0;
  while (from < s.size()) {
    std::size_t to = s.find(',', from);
    if (to == std::string::npos) to = s.size();
    if (to > from) out.push_back(s.substr(from, to - from));
    from = to + 1;
  }
  return out;
}

inline std::string date_of(const std::string& when) { return when.substr(0, std::min<std::size_t>(10, when.size())); }

}  // namespace detail

// Takes a step: an entry sealed in the docket's journal. The step's form is checked here (a date, a party); whether the rules
// allow it is review()'s question, so that a docket records what was done, rightly or not.
inline const fx::Entry& take(Docket& d, const Step& s) {
  if (!dt::is_date(detail::date_of(s.when))) throw std::invalid_argument("law::proceedings::take: a step's instant begins with a date YYYY-MM-DD, not " + s.when);
  bool party = false;
  for (const auto& p : d.parties) party = party || p.name == s.who;
  if (!party) throw std::invalid_argument("law::proceedings::take: " + s.who + " is not a party to " + d.proceeding);
  for (const auto& f : s.facts)
    if (f.find(',') != std::string::npos) throw std::invalid_argument("law::proceedings::take: a fact's id holds no comma");
  return d.journal.append({s.when, s.who, s.kind, detail::join(s.facts), s.note, ""});
}

struct Review {
  std::vector<std::string> faults;  // each with the step's place, from 1
  Verdict verdict;
};

// The docket held to its rules, and, when `head` is given, to the last seal kept.
inline Review review(const Docket& d, const Rules& rules, const law::facts::Record& record, const std::string& head = {}) {
  Review r;
  const auto sealed = fx::verify(d.journal, head);
  if (!sealed.holds()) r.faults.push_back("the docket: " + sealed.reason);
  std::map<std::string, std::string> last;  // kind → the date of its last step
  std::map<std::string, std::size_t> count;
  std::map<std::string, law::facts::Standing> standing;
  for (const auto& f : record.facts) standing[f.id] = f.standing;
  for (std::size_t i = 0; i < d.journal.entries.size(); i++) {
    const auto& e = d.journal.entries[i];
    const std::string at = "step " + std::to_string(i + 1) + " (" + e.what + ", " + detail::date_of(e.when) + ")";
    const Kind* k = rules.kind(e.what);
    if (!k) {
      r.faults.push_back(at + ": no kind of step " + e.what + " is declared by " + rules.name);
      continue;
    }
    std::string role;
    for (const auto& p : d.parties)
      if (p.name == e.who) role = p.role;
    if (!k->by.empty() && std::find(k->by.begin(), k->by.end(), role) == k->by.end())
      r.faults.push_back(at + ": taken by " + e.who + (role.empty() ? ", who is not a party" : ", the " + role + ",") + " where the rules allow only " + detail::join(k->by, " or "));
    for (const auto& a : k->after)
      if (!count.count(a)) r.faults.push_back(at + ": taken before any " + a + ", which must come first");
    if (k->once && count[e.what] > 0) r.faults.push_back(at + ": a " + e.what + " may be taken once, and was taken before");
    if (k->within >= 0) {
      if (!last.count(k->from)) r.faults.push_back(at + ": its period runs from a " + k->from + ", and none was taken");
      else {
        const long days = dt::days_between(last[k->from], detail::date_of(e.when));
        if (days > k->within)
          r.faults.push_back(at + ": taken " + std::to_string(days) + " days after the " + k->from + " of " + last[k->from] + ", where the rules allow " + std::to_string(k->within));
      }
    }
    if (k->moves)
      for (const auto& f : detail::split(e.about)) {
        if (!standing.count(f)) {
          r.faults.push_back(at + ": it concerns " + f + ", which is no fact of the record");
          continue;
        }
        if (!law::facts::can_become(standing[f], *k->moves))
          r.faults.push_back(at + ": " + f + " is " + std::string(law::facts::word(standing[f])) + ", and does not become " + std::string(law::facts::word(*k->moves)));
        else standing[f] = *k->moves;
      }
    last[e.what] = detail::date_of(e.when);
    count[e.what]++;
  }
  const std::string claim = "the docket of " + d.proceeding + " keeps " + rules.name;
  if (r.faults.empty()) r.verdict = Verdict::yes(claim, "its " + apodeixis::kernel::count(d.journal.entries.size(), "step") + " seal, and each was taken in order, on time, by a party allowed to take it", d.journal.entries.size());
  else {
    r.verdict = Verdict::no(claim, std::to_string(r.faults.size()) + (r.faults.size() == 1 ? " fault: " : " faults, the first: ") + r.faults[0], d.journal.entries.size());
    for (const auto& f : r.faults) r.verdict.with("fault", f);
  }
  return r;
}

struct Due {
  std::string kind, by;  // by: the last day
  bool overdue = false;
};

struct State {
  std::vector<std::string> taken;  // the kinds taken, in the order first taken
  std::vector<std::string> open;   // the kinds that may be taken next
  std::vector<Due> due;
  Verdict verdict;
};

// What may be taken next at a date, what is due, and what is overdue.
inline State state(const Docket& d, const Rules& rules, const std::string& today) {
  if (!dt::is_date(today)) throw std::invalid_argument("law::proceedings::state: " + today + " is not a date YYYY-MM-DD");
  State s;
  std::map<std::string, std::string> last;
  for (const auto& e : d.journal.entries) {
    if (!last.count(e.what)) s.taken.push_back(e.what);
    last[e.what] = detail::date_of(e.when);
  }
  for (const auto& k : rules.kinds) {
    bool ready = !(k.once && last.count(k.name));
    for (const auto& a : k.after) ready = ready && last.count(a);
    if (!ready) continue;
    s.open.push_back(k.name);
    if (k.within >= 0 && last.count(k.from) && !(last.count(k.name) && last[k.name] >= last[k.from])) {
      const std::string by = dt::add_days(last[k.from], k.within);
      s.due.push_back({k.name, by, today > by});
    }
  }
  std::string why = s.open.empty() ? "nothing may be taken next" : "may be taken next: " + detail::join(s.open, ", ");
  for (const auto& u : s.due) why += "; " + u.kind + (u.overdue ? " was due by " : " is due by ") + u.by;
  s.verdict = Verdict::yes("the state of " + d.proceeding + " on " + today, why);
  for (const auto& u : s.due)
    if (u.overdue) s.verdict.with("overdue", u.kind + " (by " + u.by + ")");
  return s;
}

// The record after the docket's steps: each fact a step concerns given the standing its kind gives, with the step as its basis.
// Refuses a change the fact does not allow, as review() reports it.
inline law::facts::Record apply(const Docket& d, const Rules& rules, law::facts::Record record) {
  for (std::size_t i = 0; i < d.journal.entries.size(); i++) {
    const auto& e = d.journal.entries[i];
    const Kind* k = rules.kind(e.what);
    if (!k || !k->moves) continue;
    for (const auto& id : detail::split(e.about)) {
      auto it = std::find_if(record.facts.begin(), record.facts.end(), [&](const law::facts::Fact& f) { return f.id == id; });
      if (it == record.facts.end()) throw std::invalid_argument("law::proceedings::apply: step " + std::to_string(i + 1) + " concerns " + id + ", which is no fact of the record");
      law::facts::advance(*it, *k->moves, e.what + " of " + detail::date_of(e.when) + " by " + e.who + (e.note.empty() ? "" : ": " + e.note));
      if (*k->moves == law::facts::Standing::found || *k->moves == law::facts::Standing::final_) {
        // A finding is a relational fact: it emerged from the proceeding, persists from the step, and constrains what follows.
        if (it->emerged_from.empty()) it->emerged_from = d.proceeding + ", " + e.what + " of " + detail::date_of(e.when);
        if (it->since.empty()) it->since = detail::date_of(e.when);
      }
    }
  }
  return record;
}

}  // namespace law::proceedings
