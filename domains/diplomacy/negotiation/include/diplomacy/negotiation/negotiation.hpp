// A negotiation among parties over issues, at the table and at home, draft by draft (ADR 0134).
//
// A multilateral text is negotiated as issues whose options stand in brackets until they are settled. A package is one option of
// each issue. What each party can accept is a matter of two levels (Putnam 1988): its negotiator's judgement at the table, and
// ratification at home, where the actors who must ratify (chambers, ministries, a cabinet, a legislature's parties) decide by a
// rule of their own. A party's win-set is the packages its constituency would ratify; an agreement needs a package in every
// party's win-set, and the zone of possible agreement is their intersection (Fisher and Ury 1981; Raiffa 1982).
//
//   values      each party and each ratifier values each option of each issue, in its own units, as an interval: what the
//               evidence allows of its stated or estimated position, never one number pretended. Values add across issues
//               (separability, a declared assumption; Raiffa 1982, ch. 10). What leaving an issue unagreed is worth to it (none),
//               and what the whole is worth elsewhere (an outside option), make its reservation: its value of no agreement;
//   accepts     a valuation accepts a package clearly when it is worth at least the reservation however the intervals fall,
//               untenably when it is worth less however they fall, arguably otherwise;
//   ratified    the ratifiers that accept clearly vote yes, those that reject clearly vote no, and those undecided may go either
//               way: a package is ratified clearly when the ratification rule (diplomacy::rules) adopts it however the undecided go,
//               untenably when it adopts it no way, arguably otherwise. A party accepts a package when its negotiator does (if a
//               valuation is declared at the table) and its ratifiers do (if any are declared);
//   zone        the packages of a draft (the product of the options still in brackets) each read for every party, an agreement
//               clear when every party's acceptance is, untenable when any party's is, arguable otherwise. Since every actor's
//               values are its own, a world can make any set of them favourable at once, so that these conjunctions are exact;
//   trajectory  the drafts in order, each with its zone, what it settled and reopened, and the moment the zone emptied or lost its
//               clear agreements;
//   linkage     whether joining the issues creates a zone where none of them alone has one (Sebenius 1983; Tollison and Willett
//               1979). It can only when the parties trade the issues at different rates: with valuations exactly opposed at one
//               rate on every issue, a package's total surplus is the sum of the issues' surpluses, and no package is agreeable
//               when every issue's surplus is negative (the theory note, Proposition 8.4);
//   claimed     between two parties, how the agreeable packages might be divided: Nash's, Kalai and Smorodinsky's and the egalitarian
//               solutions (nomos's games) on the negotiators' values taken at the midpoints of their intervals, a declared basis.
//               A party whose win-set is smaller may claim more (Schelling 1960; Putnam 1988): a constraint at home can strengthen
//               a hand at the table.
//
// A change of a ratification rule moves the zone (`difference`): a rule whose winning coalitions are fewer never enlarges a win-set.
//
// Nothing here is any party's position until a caller states it; the checks state invented parties on invented issues.
//
// References. Putnam, R. D. (1988), Diplomacy and domestic politics: the logic of two-level games, International Organization 42,
// 427–460. Raiffa, H. (1982), The Art and Science of Negotiation, Harvard University Press. Fisher, R. and Ury, W. (1981), Getting
// to Yes, Houghton Mifflin. Sebenius, J. K. (1983), Negotiation arithmetic: adding and subtracting issues and parties,
// International Organization 37, 281–316. Tollison, R. D. and Willett, T. D. (1979), An economic theory of mutually advantageous
// issue linkages in international negotiations, International Organization 33, 425–449. Schelling, T. C. (1960), The Strategy of
// Conflict, Harvard University Press, ch. 2. Zartman, I. W. and Berman, M. R. (1982), The Practical Negotiator, Yale University Press.
#pragma once

#include <diplomacy/rules/rules.hpp>
#include <kybernesis/kernel/verdicts.hpp>
#include <nomos/games/bargaining.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace diplomacy::negotiation {

using kybernesis::kernel::Standing;

struct Interval {
  double lo = 0, hi = 0;
  double mid() const { return (lo + hi) / 2; }
};

inline Interval operator+(Interval a, Interval b) { return {a.lo + b.lo, a.hi + b.hi}; }

struct Option {
  std::string id, text;
};

struct Issue {
  std::string id, text;
  std::vector<Option> options;
};

struct Valuation {
  std::map<std::string, std::map<std::string, Interval>> values;  // issue → option → what it is worth
  std::map<std::string, Interval> none;                          // issue → the worth of no agreement on it (0 when not stated)
  Interval outside{0, 0};                                        // what the whole is worth elsewhere
  bool declared() const { return !values.empty(); }
};

struct Ratifier {
  std::string id, name;
  Valuation valuation;
};

struct Party {
  std::string id, name;
  Valuation valuation;            // the negotiator's at the table; undeclared when only ratification is modelled
  std::vector<Ratifier> ratifiers;
  rules::Rule ratification;       // a rule over the ratifiers' ids
};

// A draft: the options of each issue still in brackets. An issue not listed keeps every option; one option left is settled.
struct Draft {
  std::string id, when, text;
  std::map<std::string, std::vector<std::string>> open;
};

struct Negotiation {
  std::string name;
  std::vector<Issue> issues;
  std::vector<Party> parties;
  std::vector<Draft> drafts;
  const Party* party(const std::string& id) const {
    for (const auto& p : parties)
      if (p.id == id) return &p;
    return nullptr;
  }
};

struct Problem {
  std::string where, what;
};

inline constexpr std::size_t most_ratifiers = 16;

namespace detail {

inline void valuation_problems(const Negotiation& n, const Valuation& v, const std::string& where, std::vector<Problem>& out) {
  if (!v.declared()) return;
  for (const auto& is : n.issues) {
    auto it = v.values.find(is.id);
    for (const auto& o : is.options) {
      if (it == v.values.end() || !it->second.count(o.id)) {
        out.push_back({where, "it values no option " + o.id + " of " + is.id});
        continue;
      }
      const Interval x = it->second.at(o.id);
      if (!(x.lo <= x.hi)) out.push_back({where, "its value of " + is.id + "=" + o.id + " is an interval whose lower end is above its upper"});
    }
    if (auto none = v.none.find(is.id); none != v.none.end() && !(none->second.lo <= none->second.hi))
      out.push_back({where, "its worth of no agreement on " + is.id + " is an interval whose lower end is above its upper"});
  }
  for (const auto& [issue, _] : v.values) {
    bool known = false;
    for (const auto& is : n.issues) known = known || is.id == issue;
    if (!known) out.push_back({where, "it values " + issue + ", which is no issue"});
  }
  if (!(v.outside.lo <= v.outside.hi)) out.push_back({where, "its outside option is an interval whose lower end is above its upper"});
}

}  // namespace detail

inline std::vector<Problem> problems(const Negotiation& n) {
  std::vector<Problem> out;
  std::set<std::string> issues, parties;
  for (const auto& is : n.issues) {
    if (!issues.insert(is.id).second) out.push_back({"issue " + is.id, "declared twice"});
    if (is.options.empty()) out.push_back({"issue " + is.id, "it has no option"});
    std::set<std::string> opts;
    for (const auto& o : is.options)
      if (!opts.insert(o.id).second) out.push_back({"issue " + is.id, "the option " + o.id + " is declared twice"});
  }
  for (const auto& p : n.parties) {
    const std::string where = "party " + p.id;
    if (!parties.insert(p.id).second) out.push_back({where, "declared twice"});
    if (!p.valuation.declared() && p.ratifiers.empty()) out.push_back({where, "it declares neither a valuation at the table nor ratifiers at home"});
    detail::valuation_problems(n, p.valuation, where, out);
    if (!p.ratifiers.empty()) {
      if (p.ratifiers.size() > most_ratifiers) out.push_back({where, "more than " + std::to_string(most_ratifiers) + " ratifiers"});
      std::set<std::string> ids;
      for (const auto& r : p.ratifiers) {
        if (!ids.insert(r.id).second) out.push_back({where, "the ratifier " + r.id + " is declared twice"});
        if (!r.valuation.declared()) out.push_back({where, "the ratifier " + r.id + " declares no valuation"});
        detail::valuation_problems(n, r.valuation, where + ", ratifier " + r.id, out);
      }
      const std::set<std::string> members(p.ratification.members.begin(), p.ratification.members.end());
      if (members != ids) out.push_back({where, "its ratification rule's members are not its ratifiers"});
      for (const auto& q : rules::problems(p.ratification)) out.push_back({where + ", ratification " + q.where, q.what});
    }
  }
  for (const auto& d : n.drafts)
    for (const auto& [issue, opts] : d.open) {
      const Issue* is = nullptr;
      for (const auto& x : n.issues)
        if (x.id == issue) is = &x;
      if (!is) {
        out.push_back({"draft " + d.id, issue + " is no issue"});
        continue;
      }
      if (opts.empty()) out.push_back({"draft " + d.id, "it leaves " + issue + " no option"});
      for (const auto& o : opts) {
        bool known = false;
        for (const auto& x : is->options) known = known || x.id == o;
        if (!known) out.push_back({"draft " + d.id, o + " is no option of " + issue});
      }
    }
  return out;
}

// ---- Acceptance --------------------------------------------------------------------------------------------------------------

// A package: an option's index for each issue of a selection of issues (all of them, in order, unless said otherwise).
using Package = std::vector<std::size_t>;

inline std::string words(const Negotiation& n, const Package& p, const std::vector<std::size_t>& issues = {}) {
  std::string s;
  for (std::size_t k = 0; k < p.size(); k++) {
    const Issue& is = n.issues[issues.empty() ? k : issues[k]];
    s += (k ? "; " : "") + is.id + ": " + (is.options[p[k]].text.empty() ? is.options[p[k]].id : is.options[p[k]].text);
  }
  return s;
}

inline Standing both(Standing a, Standing b) {
  if (a == Standing::untenable || b == Standing::untenable) return Standing::untenable;
  if (a == Standing::clear && b == Standing::clear) return Standing::clear;
  return Standing::arguable;
}

namespace detail {

inline Interval none_of(const Valuation& v, const std::string& issue) {
  auto it = v.none.find(issue);
  return it == v.none.end() ? Interval{0, 0} : it->second;
}

}  // namespace detail

// What a package of the given issues is worth to a valuation, and its reservation over them (with the outside option when the
// package covers the whole negotiation).
inline Interval value(const Negotiation& n, const Valuation& v, const Package& p, const std::vector<std::size_t>& issues) {
  Interval s;
  for (std::size_t k = 0; k < p.size(); k++) {
    const Issue& is = n.issues[issues[k]];
    s = s + v.values.at(is.id).at(is.options[p[k]].id);
  }
  return s;
}

inline Interval reservation(const Negotiation& n, const Valuation& v, const std::vector<std::size_t>& issues) {
  Interval s = issues.size() == n.issues.size() ? v.outside : Interval{0, 0};
  for (std::size_t i : issues) s = s + detail::none_of(v, n.issues[i].id);
  return s;
}

inline Standing accepts(const Negotiation& n, const Valuation& v, const Package& p, const std::vector<std::size_t>& issues) {
  const Interval x = value(n, v, p, issues), r = reservation(n, v, issues);
  if (x.lo >= r.hi) return Standing::clear;
  if (x.hi < r.lo) return Standing::untenable;
  return Standing::arguable;
}

struct Answer {
  std::string party;
  Standing standing = Standing::undecided;
  std::string why;
};

// The coalitions of a party's ratifiers that ratify, under its rule read at its stated readings: a table by mask of the rule's
// members.
inline std::vector<char> ratifying(const Party& party) {
  const auto meaning = rules::meaning_of(party.ratification, rules::stated(party.ratification));
  const auto game = rules::game(party.ratification, meaning);
  std::vector<char> t(std::size_t{1} << party.ratification.members.size());
  for (std::uint32_t S = 0; S < t.size(); S++) t[S] = game.wins(S);
  return t;
}

// A party's acceptance of a package: its negotiator's, and its ratifiers' under its rule (`table` from ratifying, or computed).
inline Answer answer(const Negotiation& n, const Party& party, const Package& p, const std::vector<std::size_t>& issues, const std::vector<char>* table = nullptr) {
  Answer a{party.id, Standing::clear, ""};
  if (party.valuation.declared()) {
    a.standing = accepts(n, party.valuation, p, issues);
    a.why = std::string("its negotiator: ") + kybernesis::kernel::name(a.standing);
  }
  if (party.ratifiers.empty()) return a;
  std::vector<char> own;
  if (!table) own = ratifying(party), table = &own;
  std::uint32_t sure = 0;
  std::vector<std::size_t> maybe;
  std::vector<std::string> yes, no, open;
  for (std::size_t k = 0; k < party.ratification.members.size(); k++) {
    const std::string& id = party.ratification.members[k];
    const Ratifier* r = nullptr;
    for (const auto& x : party.ratifiers)
      if (x.id == id) r = &x;
    const Standing s = accepts(n, r->valuation, p, issues);
    const std::string who = r->name.empty() ? r->id : r->name;
    if (s == Standing::clear) sure |= std::uint32_t{1} << k, yes.push_back(who);
    else if (s == Standing::arguable) maybe.push_back(k), open.push_back(who);
    else no.push_back(who);
  }
  std::size_t wins = 0;
  const std::size_t ways = std::size_t{1} << maybe.size();
  for (std::size_t m = 0; m < ways; m++) {
    std::uint32_t S = sure;
    for (std::size_t j = 0; j < maybe.size(); j++)
      if (m >> j & 1u) S |= std::uint32_t{1} << maybe[j];
    wins += (*table)[S] != 0;
  }
  const Standing r = wins == ways ? Standing::clear : wins == 0 ? Standing::untenable : Standing::arguable;
  auto list = [](const std::vector<std::string>& v) {
    std::string s;
    for (std::size_t k = 0; k < v.size(); k++) s += (k ? ", " : "") + v[k];
    return s.empty() ? std::string("none") : s;
  };
  const std::string ratification = std::string("its ratification: ") + kybernesis::kernel::name(r) + " (accept: " + list(yes) + "; reject: " + list(no) +
                                   (open.empty() ? "" : "; undecided: " + list(open)) + ")";
  a.standing = party.valuation.declared() ? both(a.standing, r) : r;
  a.why = a.why.empty() ? ratification : a.why + "; " + ratification;
  return a;
}

// ---- The zone of possible agreement -------------------------------------------------------------------------------------------

struct Reading {
  Package choice;
  Standing agreement = Standing::undecided;
  std::vector<Answer> parties;
  std::vector<Interval> values;  // each party's negotiator's value (zero width and zero when undeclared)
};

struct Zone {
  std::string draft;
  std::size_t packages = 0, clear = 0, arguable = 0;
  std::vector<Reading> agreeable;           // the packages clear or arguable, in lexicographic order of their options
  std::vector<std::size_t> undominated;     // of `agreeable`: those no other agreeable package is surely better than for every party
  bool dominance_read = false;              // false when too many agreeable packages to compare
  std::vector<std::string> settled;         // issues with one option left
  std::vector<std::string> blocking;        // parties that accept no package of the draft
};

struct Options {
  std::size_t packages = 200'000;  // a draft's packages read at most
  std::size_t compared = 5'000;    // agreeable packages compared for dominance at most
};

// The options of each issue a draft leaves open (every option when draft is null).
inline std::vector<std::vector<std::size_t>> open_options(const Negotiation& n, const Draft* d) {
  std::vector<std::vector<std::size_t>> out;
  for (const auto& is : n.issues) {
    std::vector<std::size_t> v;
    const std::vector<std::string>* listed = nullptr;
    if (d)
      if (auto it = d->open.find(is.id); it != d->open.end()) listed = &it->second;
    for (std::size_t k = 0; k < is.options.size(); k++)
      if (!listed || std::find(listed->begin(), listed->end(), is.options[k].id) != listed->end()) v.push_back(k);
    out.push_back(v);
  }
  return out;
}

namespace detail {

inline bool surely_better(const Interval& a, const Interval& b) { return a.lo > b.hi; }
inline bool no_worse(const Interval& a, const Interval& b) { return a.lo >= b.hi; }

inline Zone zone_over(const Negotiation& n, const std::vector<std::vector<std::size_t>>& open, const std::vector<std::size_t>& issues, const std::string& label,
                      const Options& o) {
  Zone z;
  z.draft = label;
  std::size_t count = 1;
  for (std::size_t i : issues) {
    count *= open[i].size();
    if (count > o.packages) throw std::invalid_argument("diplomacy::negotiation: more than " + std::to_string(o.packages) + " packages in " + label);
  }
  z.packages = count;
  for (std::size_t i : issues)
    if (open[i].size() == 1) z.settled.push_back(n.issues[i].id);
  std::vector<char> accepts_any(n.parties.size(), 0);
  std::vector<std::vector<char>> tables(n.parties.size());
  for (std::size_t j = 0; j < n.parties.size(); j++)
    if (!n.parties[j].ratifiers.empty()) tables[j] = ratifying(n.parties[j]);
  Package p(issues.size(), 0);
  std::vector<std::size_t> at(issues.size(), 0);
  for (std::size_t c = 0; c < count; c++) {
    for (std::size_t k = 0; k < issues.size(); k++) p[k] = open[issues[k]][at[k]];
    Reading r;
    r.choice = p;
    r.agreement = Standing::clear;
    for (std::size_t j = 0; j < n.parties.size(); j++) {
      const auto& party = n.parties[j];
      Answer a = answer(n, party, p, issues, party.ratifiers.empty() ? nullptr : &tables[j]);
      if (a.standing != Standing::untenable) accepts_any[j] = 1;
      r.agreement = both(r.agreement, a.standing);
      r.parties.push_back(std::move(a));
      r.values.push_back(party.valuation.declared() ? value(n, party.valuation, p, issues) : Interval{0, 0});
    }
    if (r.agreement != Standing::untenable) {
      (r.agreement == Standing::clear ? z.clear : z.arguable)++;
      z.agreeable.push_back(std::move(r));
    }
    for (std::size_t k = issues.size(); k-- > 0;) {
      if (++at[k] < open[issues[k]].size()) break;
      at[k] = 0;
    }
  }
  for (std::size_t j = 0; j < n.parties.size(); j++)
    if (!accepts_any[j]) z.blocking.push_back(n.parties[j].id);
  if (z.agreeable.size() <= o.compared) {
    z.dominance_read = true;
    for (std::size_t a = 0; a < z.agreeable.size(); a++) {
      bool dominated = false;
      for (std::size_t b = 0; b < z.agreeable.size() && !dominated; b++) {
        if (a == b) continue;
        bool all = true, one = false, any_valued = false;
        for (std::size_t j = 0; j < n.parties.size(); j++) {
          if (!n.parties[j].valuation.declared()) continue;
          any_valued = true;
          all = all && no_worse(z.agreeable[b].values[j], z.agreeable[a].values[j]);
          one = one || surely_better(z.agreeable[b].values[j], z.agreeable[a].values[j]);
        }
        dominated = any_valued && all && one;
      }
      if (!dominated) z.undominated.push_back(a);
    }
  }
  return z;
}

inline std::vector<std::size_t> every_issue(const Negotiation& n) {
  std::vector<std::size_t> v(n.issues.size());
  for (std::size_t k = 0; k < v.size(); k++) v[k] = k;
  return v;
}

inline void require(const Negotiation& n) {
  if (const auto ps = problems(n); !ps.empty()) throw std::invalid_argument("diplomacy::negotiation: " + ps[0].where + ": " + ps[0].what);
}

}  // namespace detail

// The zone of a draft (by index), or of every option when the index is past the drafts.
inline Zone zone(const Negotiation& n, std::size_t draft = static_cast<std::size_t>(-1), const Options& o = {}) {
  detail::require(n);
  const Draft* d = draft < n.drafts.size() ? &n.drafts[draft] : nullptr;
  return detail::zone_over(n, open_options(n, d), detail::every_issue(n), d ? d->id : "every option", o);
}

// The packages whose agreement differs between two zones of one negotiation (a package missing from a zone is untenable there).
struct Moved {
  Package choice;
  Standing before = Standing::untenable, after = Standing::untenable;
};

inline std::vector<Moved> difference(const Zone& a, const Zone& b) {
  std::map<Package, std::pair<Standing, Standing>> m;
  for (const auto& r : a.agreeable) m[r.choice] = {r.agreement, Standing::untenable};
  for (const auto& r : b.agreeable) {
    auto it = m.find(r.choice);
    if (it == m.end()) m[r.choice] = {Standing::untenable, r.agreement};
    else it->second.second = r.agreement;
  }
  std::vector<Moved> out;
  for (const auto& [choice, s] : m)
    if (s.first != s.second) out.push_back({choice, s.first, s.second});
  return out;
}

// ---- The drafts in order --------------------------------------------------------------------------------------------------------

struct Step {
  std::string draft, when;
  Zone zone;
  std::vector<std::string> settled_now, reopened, notes;
};

inline std::vector<Step> trajectory(const Negotiation& n, const Options& o = {}) {
  detail::require(n);
  std::vector<Step> out;
  std::vector<std::vector<std::size_t>> before = open_options(n, nullptr);
  const Zone* last = nullptr;
  Zone first = zone(n, static_cast<std::size_t>(-1), o);
  last = &first;
  for (std::size_t d = 0; d < n.drafts.size(); d++) {
    Step s;
    s.draft = n.drafts[d].id, s.when = n.drafts[d].when;
    const auto now = open_options(n, &n.drafts[d]);
    s.zone = detail::zone_over(n, now, detail::every_issue(n), n.drafts[d].id, o);
    for (std::size_t i = 0; i < n.issues.size(); i++) {
      if (now[i].size() == 1 && before[i].size() > 1) {
        s.settled_now.push_back(n.issues[i].id);
        bool held = false;
        for (const auto& r : last->agreeable) held = held || r.choice[i] == now[i][0];
        if (!last->agreeable.empty() && !held)
          s.notes.push_back("it settles " + n.issues[i].id + " on " + n.issues[i].options[now[i][0]].id + ", which no agreeable package of the text before held");
      }
      for (std::size_t k : now[i])
        if (std::find(before[i].begin(), before[i].end(), k) == before[i].end()) s.reopened.push_back(n.issues[i].id + "=" + n.issues[i].options[k].id);
    }
    if (!last->agreeable.empty() && s.zone.agreeable.empty()) s.notes.push_back("the zone emptied: no package of this draft can be agreed");
    else if (last->clear > 0 && s.zone.clear == 0 && !s.zone.agreeable.empty())
      s.notes.push_back("no clear agreement is left: every package of this draft turns on how undecided values fall");
    for (const auto& p : s.zone.blocking) s.notes.push_back(p + " accepts no package of this draft");
    out.push_back(std::move(s));
    before = now;
    last = &out.back().zone;
  }
  return out;
}

// ---- Linkage ----------------------------------------------------------------------------------------------------------------

struct Alone {
  std::string issue;
  std::size_t agreeable = 0, clear = 0;
};

struct Linkage {
  std::vector<Alone> alone;           // each issue negotiated alone, against its own worth of no agreement
  std::size_t linked = 0, linked_clear = 0;
  bool created = false;               // the linked negotiation has a zone though no issue alone has one
};

inline Linkage linkage(const Negotiation& n, const Options& o = {}) {
  detail::require(n);
  Linkage l;
  const auto open = open_options(n, nullptr);
  bool any_alone = false;
  for (std::size_t i = 0; i < n.issues.size(); i++) {
    const Zone z = detail::zone_over(n, open, {i}, n.issues[i].id, o);
    l.alone.push_back({n.issues[i].id, z.agreeable.size(), z.clear});
    any_alone = any_alone || !z.agreeable.empty();
  }
  const Zone all = zone(n, static_cast<std::size_t>(-1), o);
  l.linked = all.agreeable.size(), l.linked_clear = all.clear;
  l.created = l.linked > 0 && !any_alone;
  return l;
}

// ---- Value claimed -------------------------------------------------------------------------------------------------------------

struct Claimed {
  std::string a, b;
  std::string basis;
  nomos::games::BargainingSolution nash, kalai_smorodinsky, egalitarian;
  // Of the zone's agreeable packages read, the one nearest each solution's point.
  std::size_t at_nash = 0, at_kalai_smorodinsky = 0, at_egalitarian = 0;
};

inline Claimed claimed(const Negotiation& n, const Zone& z, const std::string& a, const std::string& b, bool clear_only = true) {
  std::size_t ia = n.parties.size(), ib = n.parties.size();
  for (std::size_t j = 0; j < n.parties.size(); j++) {
    if (n.parties[j].id == a) ia = j;
    if (n.parties[j].id == b) ib = j;
  }
  if (ia == n.parties.size() || ib == n.parties.size() || ia == ib) throw std::invalid_argument("diplomacy::negotiation: claimed between two parties of the negotiation");
  if (!n.parties[ia].valuation.declared() || !n.parties[ib].valuation.declared())
    throw std::invalid_argument("diplomacy::negotiation: value claimed needs both negotiators' valuations");
  Claimed c;
  c.a = a, c.b = b;
  c.basis = std::string("the negotiators' values at the midpoints of their intervals, over the ") + (clear_only ? "clear" : "agreeable") +
            " packages, against the midpoints of their reservations";
  nomos::games::Bargaining B;
  std::vector<std::size_t> which;
  for (std::size_t k = 0; k < z.agreeable.size(); k++) {
    if (clear_only && z.agreeable[k].agreement != Standing::clear) continue;
    B.outcomes.push_back({z.agreeable[k].values[ia].mid(), z.agreeable[k].values[ib].mid()});
    which.push_back(k);
  }
  if (which.empty()) throw std::invalid_argument("diplomacy::negotiation: no package to divide");
  const auto every = detail::every_issue(n);
  B.disagreement = {reservation(n, n.parties[ia].valuation, every).mid(), reservation(n, n.parties[ib].valuation, every).mid()};
  B.outcomes.push_back(B.disagreement);
  c.nash = nomos::games::nash(B), c.kalai_smorodinsky = nomos::games::kalai_smorodinsky(B), c.egalitarian = nomos::games::egalitarian(B);
  auto nearest = [&](const nomos::games::Pair& p) {
    std::size_t best = which[0];
    double d = INFINITY;
    for (std::size_t k = 0; k + 1 < B.outcomes.size(); k++) {
      const double e = std::hypot(B.outcomes[k][0] - p[0], B.outcomes[k][1] - p[1]);
      if (e < d) d = e, best = which[k];
    }
    return best;
  };
  c.at_nash = nearest(c.nash.point), c.at_kalai_smorodinsky = nearest(c.kalai_smorodinsky.point), c.at_egalitarian = nearest(c.egalitarian.point);
  return c;
}

}  // namespace diplomacy::negotiation
