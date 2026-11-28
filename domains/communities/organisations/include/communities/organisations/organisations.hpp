// Organisations: the accounts of a treaty organisation read from its record (the design *Communities That Agents Of Any Provider Find,
// Join, Work In And Leave*, Definition 54 and Sections 5.16, 7.9.8 and 8.13).
//
// A treaty organisation is a community founded by compact (the preset treaty-organisation). What this module reads, it reads from the
// organisation's chronicle and nothing else:
//
// - **Calls for contributions** are acts of the declared kind "call for contributions", read as requests whose matter is an assessed
//   contribution, each naming the member, the amount in the core budget's unit, the period and the turn it is due.
// - **Payments** are deposits into the budgets: into the core budget, credited to the member that answers for them (the bearer, so that
//   an envoy who pays under its community's mandate pays for its community); into an earmarked holding, voluntary, with its purpose.
//   A core payment is credited first to the call it names, then to the member's calls in the order they were made, and what is left
//   is held as a credit for the member's next calls, after the United Nations' Financial Regulations, which credit a member's payment
//   to its contributions in the order in which they were assessed.
// - **Arrears** at a turn are what remains owed on calls whose due turn has passed: an overdue request (the design's Section 7.9.8).
// - **The scale.** An adopted budget is a proposal under the budget's decision that passes under the rule in force (its change names
//   the budget and the period); the scale is a declared set of weights (by default the budget decision's own). Each member's quota is
//   B·w/W exactly; a member's call follows the scale when it lies within one unit of its quota, and the calls follow it when they also
//   sum to the budget (the quota property of apportionment; Balinski and Young, *Fair Representation*, 1982, ch. 2).
// - **A member in arrears beyond a declared number of periods**: arrears equal to or more than the contributions called from it for the
//   preceding k full periods (the Charter of the United Nations, Article 19, reads k = 2 for a vote in the General Assembly). A declared
//   mechanism, reported and never applied: a sanction is the assembly's decision, recorded like any other.
// - **Core against earmarked**: what each member paid into each, and the share earmarked.
// - **Staff by origin**: the holders of the staff's positions by their origin under control (thesmos's Definition 3), each origin's
//   share exactly and the Herfindahl–Hirschman index of the shares (nomos's measurement).
// - **Withdrawal**: a member that leaves its position, the turn its notice takes effect (declared), and what it owed when it left and owes
//   now: contributions called before its withdrawal stay owed (the first league's Article 10; the design's Definition 54).

#pragma once

#include <thesmos/chronicle/chronicle.hpp>

#include <nomos/measurement/concentration.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <algorithm>
#include <map>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace communities::organisations {

using morphe::arithmetic::Rational;
using thesmos::chronicle::State;

// What an organisation declares for its accounts, as its constitution names them.
struct Declared {
  std::string core = "core budget";
  std::vector<std::string> earmarked = {"earmarked"};
  std::string member = "member";
  std::vector<std::string> staff = {"staff", "secretary-general"};
  std::string budget = "adopt budget";  // the decision that adopts a budget, whose weights are the scale unless one is given
  long long notice = 4;                 // the turns between a member's leaving and its effect
};

struct Call {
  std::string id, member, by;
  long long amount = 0, period = 0, due = 0, turn = 0;
  std::size_t place = 0;  // its place in the chronicle
};
struct Payment {
  std::string member, author, holding, call, purpose;
  long long amount = 0, turn = 0;
  std::size_t place = 0;
};

namespace detail {
inline long long whole(const std::string& v) {
  if (v.empty() || v.size() > 18 || v.find_first_not_of("0123456789") != std::string::npos) return -1;
  return std::stoll(v);
}
inline bool listed(const std::vector<std::string>& v, const std::string& x) { return std::find(v.begin(), v.end(), x) != v.end(); }
}  // namespace detail

// The calls for contributions in the record, in order: requests whose matter is an assessed contribution (as the declared kind fixes it).
inline std::vector<Call> calls(const State& s) {
  std::vector<Call> out;
  for (std::size_t k = 0; k < s.readings.size(); k++) {
    const auto& a = s.readings[k];
    if (a.kind != "request" || a.field("matter") != "assessed contribution") continue;
    out.push_back({a.field("id"), a.field("member"), a.author, detail::whole(a.field("amount")), detail::whole(a.field("period")), detail::whole(a.field("due")), a.turn, k});
  }
  return out;
}

// The payments into the core and earmarked budgets, in order: each admitted deposit there, with the party that answers for it (from the
// access the fold recorded) and the call it names or the purpose it states (from the act).
inline std::vector<Payment> payments(const State& s, const Declared& d) {
  std::vector<Payment> out;
  auto budget = [&](const std::string& h) { return h == d.core || detail::listed(d.earmarked, h); };
  std::vector<const thesmos::chronicle::Access*> deposits;  // the deposits recorded, in order
  for (const auto& x : s.accesses)
    if (x.aim == "deposit" && budget(x.holding)) deposits.push_back(&x);
  std::size_t next = 0;
  for (std::size_t k = 0; k < s.readings.size() && next < deposits.size(); k++) {
    const auto& a = s.readings[k];
    if (a.kind != "deposit" || !budget(a.field("holding"))) continue;
    const auto* x = deposits[next++];
    out.push_back({x->bearer, x->author, x->holding, s.acts[k].field("contribution"), x->purpose, x->amount, x->turn, k});
  }
  return out;
}

// A member's account at a turn: what was called from it, paid into the core budget, still owed, overdue among that (its arrears), its
// credit, and the calls overdue.
struct Account {
  long long called = 0, paid = 0, owed = 0, arrears = 0, credit = 0;
  std::vector<std::string> overdue;
};

// Every member's account at a turn, the calls and core payments up to it taken in the order of the record.
inline std::map<std::string, Account> accounts(const State& s, const Declared& d, long long turn) {
  struct Open {
    std::string id;
    long long owed = 0, due = 0;
  };
  std::map<std::string, std::vector<Open>> open;
  std::map<std::string, Account> out;
  const auto cs = calls(s);
  const auto ps = payments(s, d);
  std::size_t i = 0, j = 0;
  auto pay = [&](const std::string& m, long long amount, const std::string& named) {
    auto& list = open[m];
    for (auto& o : list)  // the call it names first
      if (amount > 0 && o.id == named && o.owed > 0) {
        const long long x = std::min(amount, o.owed);
        o.owed -= x, amount -= x;
      }
    for (auto& o : list)  // then the member's calls in the order they were made
      if (amount > 0 && o.owed > 0) {
        const long long x = std::min(amount, o.owed);
        o.owed -= x, amount -= x;
      }
    out[m].credit += amount;  // what is left waits for the next call
  };
  while (i < cs.size() || j < ps.size()) {
    const bool call_first = j >= ps.size() || (i < cs.size() && cs[i].place < ps[j].place);
    if (call_first) {
      const Call& c = cs[i++];
      if (c.turn > turn) continue;
      out[c.member].called += c.amount;
      open[c.member].push_back({c.id, c.amount, c.due});
      if (const long long credit = out[c.member].credit; credit > 0) {
        out[c.member].credit = 0;
        pay(c.member, credit, c.id);
      }
    } else {
      const Payment& p = ps[j++];
      if (p.turn > turn || p.holding != d.core) continue;
      out[p.member].paid += p.amount;
      pay(p.member, p.amount, p.call);
    }
  }
  for (auto& [m, list] : open)
    for (const auto& o : list) {
      out[m].owed += o.owed;
      if (o.owed > 0 && turn > o.due) out[m].arrears += o.owed, out[m].overdue.push_back(o.id);
    }
  return out;
}

// ---- The scale ----------------------------------------------------------------------------------------------------------------------

// The budget adopted for a period: the amount the last proposal under the budget's decision names (its change "budget" and "period") that
// passes under the decision's rule, unit of count and weights in force, or nothing.
inline std::optional<long long> adopted_budget(const State& s, const Declared& d, long long period) {
  const auto dec = s.rules.aggregation.find(d.budget);
  if (dec == s.rules.aggregation.end()) return std::nullopt;
  const auto unit = thesmos::procedures::unit_named(dec->second.count_by);
  const auto rule = thesmos::procedures::read_rule(dec->second.rule);
  if (!unit || !rule) return std::nullopt;
  std::optional<long long> out;
  long long opened = -1;
  for (const auto& [id, p] : s.proposals) {
    if (p.decision != d.budget) continue;
    const auto b = p.change.find("budget"), q = p.change.find("period");
    if (b == p.change.end() || q == p.change.end() || detail::whole(q->second) != period || detail::whole(b->second) < 0) continue;
    if (!thesmos::procedures::decide(s.parties, p.ballots(), *unit, *rule, dec->second.weights).passed) continue;
    if (p.opened >= opened) opened = p.opened, out = detail::whole(b->second);
  }
  return out;
}

struct Quota {
  std::string member;
  Rational quota;        // B·w/W
  long long called = 0;  // what the period's calls ask of it
  bool within = false;   // |called − quota| < 1
};
struct Apportionment {
  long long budget = 0, called = 0;
  std::vector<Quota> quotas;
  bool sums = false;          // the calls sum to the budget
  bool follows = false;       // every call within one unit of its quota, and the calls summing to the budget
};

// The period's calls against the scale: each member's quota of the budget, exactly, and whether its call lies within one unit of it.
inline Apportionment apportionment(const State& s, long long period, long long budget, const thesmos::procedures::Weights& scale) {
  Apportionment a;
  a.budget = budget;
  Rational total(0);
  for (const auto& [m, w] : scale) total += w;
  std::map<std::string, long long> called;
  for (const auto& c : calls(s))
    if (c.period == period) called[c.member] += c.amount, a.called += c.amount;
  a.follows = true;
  for (const auto& [m, w] : scale) {
    Quota q;
    q.member = m, q.quota = total == Rational(0) ? Rational(0) : Rational(budget) * w / total, q.called = called[m];
    q.within = Rational(q.called) - q.quota < Rational(1) && q.quota - Rational(q.called) < Rational(1);
    a.follows = a.follows && q.within;
    a.quotas.push_back(q);
  }
  a.sums = a.called == budget;
  a.follows = a.follows && a.sums;
  return a;
}

// The scale a decision declares: its weights.
inline thesmos::procedures::Weights scale_of(const State& s, const std::string& decision) {
  const auto d = s.rules.aggregation.find(decision);
  return d == s.rules.aggregation.end() ? thesmos::procedures::Weights{} : d->second.weights;
}

// The members whose arrears at a turn equal or exceed what was called from them for the k full periods before the current one (a declared
// mechanism after the Charter of the United Nations, Article 19, where k = 2). Reported, never applied.
inline std::vector<std::string> in_arrears_beyond(const State& s, const Declared& d, long long turn, long long current_period, long long k) {
  std::map<std::string, long long> preceding;
  for (const auto& c : calls(s))
    if (c.period >= current_period - k && c.period < current_period && c.turn <= turn) preceding[c.member] += c.amount;
  std::vector<std::string> out;
  for (const auto& [m, acc] : accounts(s, d, turn))
    if (preceding[m] > 0 && acc.arrears >= preceding[m]) out.push_back(m);
  return out;
}

// ---- Core against earmarked ----------------------------------------------------------------------------------------------------------

struct Funding {
  long long core = 0, earmarked = 0;
  Rational earmarked_share;                                    // earmarked / (core + earmarked), 0 when nothing was paid
  std::map<std::string, std::pair<long long, long long>> by;  // member -> (core, earmarked)
  std::map<std::string, long long> by_purpose;                // what was earmarked for each purpose
};

inline Funding funding(const State& s, const Declared& d) {
  Funding f;
  for (const auto& p : payments(s, d)) {
    if (p.holding == d.core) f.core += p.amount, f.by[p.member].first += p.amount;
    else f.earmarked += p.amount, f.by[p.member].second += p.amount, f.by_purpose[p.purpose.empty() ? std::string("unstated") : p.purpose] += p.amount;
  }
  f.earmarked_share = f.core + f.earmarked == 0 ? Rational(0) : Rational(f.earmarked) / Rational(f.core + f.earmarked);
  return f;
}

// ---- Staff by origin -----------------------------------------------------------------------------------------------------------------

struct Staffing {
  std::map<std::string, std::vector<std::string>> by_origin;  // origin under control -> the holders of the staff's positions
  std::map<std::string, Rational> shares;                     // each origin's share of the holders
  double herfindahl = 0;                                      // Σ share², nomos's index; 0 when no one holds a staff position
  std::string largest;                                        // the origin with the largest share (the first by name among ties)
  std::vector<std::string> from_members;                      // the origins that hold the member's position
};

inline Staffing staffing(const State& s, const Declared& d) {
  Staffing st;
  std::set<std::string> holders;
  for (const auto& [key, since] : s.positions)
    if (detail::listed(d.staff, key.second)) holders.insert(key.first);
  for (const auto& h : holders) st.by_origin[s.parties.has(h) ? thesmos::kernel::origin(s.parties, h) : h].push_back(h);
  std::vector<double> counts;
  for (const auto& [o, hs] : st.by_origin) {
    st.shares[o] = Rational(static_cast<long long>(hs.size())) / Rational(static_cast<long long>(holders.size()));
    counts.push_back(static_cast<double>(hs.size()));
    if (st.largest.empty() || hs.size() > st.by_origin[st.largest].size()) st.largest = o;
    if (s.holds(o, d.member)) st.from_members.push_back(o);
  }
  if (!counts.empty()) st.herfindahl = nomos::measurement::herfindahl_hirschman(counts);
  return st;
}

// ---- Withdrawal ------------------------------------------------------------------------------------------------------------------------

struct Withdrawal {
  std::string member;
  long long left = 0, effective = 0;    // the turn of its leaving, and the turn its notice takes effect
  long long owed_at_leaving = 0, owed_now = 0;
};

// The members that left their position (by an act of their own naming it, or every position when contributions were called from them),
// with what they owed then and owe now.
inline std::vector<Withdrawal> withdrawals(const State& s, const Declared& d, long long turn) {
  std::vector<Withdrawal> out;
  std::set<std::string> called;
  for (const auto& c : calls(s)) called.insert(c.member);
  for (const auto& a : s.readings)
    if (a.kind == "leave" && (a.field("position") == d.member || (a.field("position") == "all" && called.count(a.author))) && a.turn <= turn) {
      Withdrawal w;
      w.member = a.author, w.left = a.turn, w.effective = a.turn + d.notice;
      const auto then = accounts(s, d, a.turn), now = accounts(s, d, turn);
      if (const auto x = then.find(a.author); x != then.end()) w.owed_at_leaving = x->second.owed;
      if (const auto x = now.find(a.author); x != now.end()) w.owed_now = x->second.owed;
      out.push_back(w);
    }
  return out;
}

}  // namespace communities::organisations
