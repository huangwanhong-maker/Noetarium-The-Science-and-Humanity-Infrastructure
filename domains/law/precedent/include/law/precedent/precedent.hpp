// Precedent: how decided cases constrain the deciding of a new one, by the reason model.
//
// A case is a set of factors, each favouring one of two sides; it was decided for a side, on a reason, a set of the factors present
// that favour that side. Horty's reason model (Horty 2011, Rules and reasons in the theory of precedent, Legal Theory 17; Horty
// and Bench-Capon 2012, A factor-based definition of precedential constraint, Artificial Intelligence and Law 20) reads a decision
// as a preference: the reason R for side s outweighs the factors X^s̄ the case had for the other side, and so outweighs any weaker
// set (U ⊆ X^s̄) and is outweighed by nothing it is contained in. Over the case base Γ, U <_Γ V when some case decided for V's
// side has R ⊆ V and U ⊆ X^s̄. Γ is *inconsistent* when some U <_Γ V and V <_Γ U. A later court is *constrained* to decide so
// that the case base stays consistent.
//
// Exactly (the theory note, Propositions 1 and 2):
//   - Γ is inconsistent iff there are cases c, for s, and d, for s̄, with R_c ⊆ X_d^s and R_d ⊆ X_c^s̄;
//   - deciding a new situation X for s on reason R ⊆ X^s keeps a consistent Γ consistent iff no case d for s̄ has R ⊆ X_d^s and
//     R_d ⊆ X^s̄;
//   - a decision for s̄ is possible on some reason iff it is possible on the reason X^s̄; so s is *forced* iff some case d for s
//     has R_d ⊆ X^s and X^s̄ ⊆ X_d^s̄ (the a fortiori constraint, which the reason model thus contains).
// When nothing forces the decision, the precedents for a side whose reasons are present are *distinguished* by the factors for
// the other side the new situation has and they did not.
//
// The *result model* takes every factor for the winning side as the reason (R = X^s); a precedent with no reason stated is read
// so. The written form, a line each:
//
//   sides plaintiff defendant
//   factor security_measures plaintiff: the owner took measures to keep it secret
//   case mason for plaintiff: security_measures, agreed_not_to_disclose, disclosed_in_negotiations
//   reason mason: security_measures

#pragma once

#include <apodeixis/kernel/verdict.hpp>

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace law::precedent {

using apodeixis::kernel::Verdict;
using Factors = std::set<std::string>;

struct Precedent {
  std::string name;
  Factors facts;
  std::string outcome;  // a side
  Factors reason;       // factors present that favour the outcome; empty: every such factor (the result model)
};

struct Base {
  std::array<std::string, 2> sides;
  std::map<std::string, std::string> side_of;  // factor → the side it favours
  std::map<std::string, std::string> text;     // factor → what it says, where given
  std::vector<Precedent> cases;
};

// ---- The parts of a situation ----------------------------------------------------------------------------------------------------

inline std::string other(const Base& b, const std::string& side) {
  if (side == b.sides[0]) return b.sides[1];
  if (side == b.sides[1]) return b.sides[0];
  throw std::invalid_argument("law::precedent: " + side + " is not one of the sides " + b.sides[0] + " and " + b.sides[1]);
}

// X^s: the factors of the situation that favour s.
inline Factors favouring(const Base& b, const Factors& x, const std::string& side) {
  Factors out;
  for (const auto& f : x) {
    const auto it = b.side_of.find(f);
    if (it == b.side_of.end()) throw std::invalid_argument("law::precedent: the factor " + f + " is not declared");
    if (it->second == side) out.insert(f);
  }
  return out;
}

inline bool within(const Factors& a, const Factors& b) { return std::includes(b.begin(), b.end(), a.begin(), a.end()); }

inline std::string words(const Factors& f) {
  if (f.empty()) return "{}";
  std::string s = "{";
  for (const auto& x : f) s += (s.size() > 1 ? ", " : "") + x;
  return s + "}";
}

// The reason a precedent was decided on: as stated, or every factor for its outcome.
inline Factors reason_of(const Base& b, const Precedent& c) {
  const Factors all = favouring(b, c.facts, c.outcome);
  if (c.reason.empty()) return all;
  if (!within(c.reason, all)) throw std::invalid_argument("law::precedent: the reason of " + c.name + " holds factors that are not present or do not favour " + c.outcome);
  return c.reason;
}

// ---- Consistency ---------------------------------------------------------------------------------------------------------------

// Whether the case base is consistent; when not, the first pair of cases that make it so, named.
inline Verdict consistent(const Base& b) {
  const std::string claim = "the case base of " + apodeixis::kernel::count(b.cases.size(), "case") + " is consistent";
  for (std::size_t i = 0; i < b.cases.size(); i++)
    for (std::size_t j = 0; j < b.cases.size(); j++) {
      const auto& c = b.cases[i];
      const auto& d = b.cases[j];
      if (c.outcome == d.outcome || c.outcome != b.sides[0]) continue;  // each pair once, c for the first side
      const auto rc = reason_of(b, c), rd = reason_of(b, d);
      if (within(rc, favouring(b, d.facts, c.outcome)) && within(rd, favouring(b, c.facts, d.outcome)))
        return Verdict::no(claim, c.name + " holds that " + words(rc) + " outweighs " + words(favouring(b, c.facts, d.outcome)) + ", which contains " + d.name + "'s reason " + words(rd) + "; and " + d.name +
                                      " holds that " + words(rd) + " outweighs " + words(favouring(b, d.facts, c.outcome)) + ", which contains " + words(rc))
            .with("conflict", c.name + " and " + d.name);
    }
  return Verdict::yes(claim, "no two cases decided for different sides each held the other's reason outweighed");
}

// ---- The constraint on a new situation --------------------------------------------------------------------------------------------

// Whether deciding X for `side` on `reason` (empty: every factor for the side) keeps the case base consistent; when not, the
// precedents that forbid it.
inline Verdict permitted(const Base& b, const Factors& x, const std::string& side, Factors reason = {}) {
  const std::string opp = other(b, side);
  const Factors mine = favouring(b, x, side), theirs = favouring(b, x, opp);
  if (reason.empty()) reason = mine;
  if (!within(reason, mine)) throw std::invalid_argument("law::precedent::permitted: a reason holds only factors present that favour " + side);
  const std::string claim = "deciding for " + side + " on " + words(reason) + " is consistent with the precedents";
  std::vector<std::string> against;
  for (const auto& d : b.cases) {
    if (d.outcome != opp) continue;
    if (within(reason, favouring(b, d.facts, side)) && within(reason_of(b, d), theirs)) against.push_back(d.name);
  }
  if (against.empty()) return Verdict::yes(claim, "no precedent for " + opp + " held a reason present here to outweigh a set containing " + words(reason));
  std::string names;
  for (std::size_t i = 0; i < against.size(); i++) names += (i ? ", " : "") + against[i];
  auto v = Verdict::no(claim, names + (against.size() == 1 ? " holds" : " hold") + " that a reason for " + opp + " present here outweighs factors for " + side + " that contain " + words(reason));
  for (const auto& a : against) v.with("forbidden by", a);
  return v;
}

struct Distinguished {
  std::string precedent;
  Factors by;  // the factors for the other side present here and not in the precedent
};

struct Constraint {
  std::string forced;                        // the side the situation must be decided for; empty when either is open
  std::vector<std::string> forcing;          // the precedents that force it
  std::vector<Distinguished> distinguished;  // precedents whose reasons are present, set apart by new factors against them
  Verdict verdict;
};

inline Constraint constraint(const Base& b, const Factors& x) {
  Constraint out;
  for (const auto& s : b.sides) {
    const std::string opp = other(b, s);
    const Factors mine = favouring(b, x, s), theirs = favouring(b, x, opp);
    for (const auto& d : b.cases) {
      if (d.outcome != s) continue;
      const auto rd = reason_of(b, d);
      if (!within(rd, mine)) continue;
      const Factors dt = favouring(b, d.facts, opp);
      if (within(theirs, dt)) {
        out.forced = s;
        out.forcing.push_back(d.name);
      } else {
        Distinguished g{d.name, {}};
        std::set_difference(theirs.begin(), theirs.end(), dt.begin(), dt.end(), std::inserter(g.by, g.by.begin()));
        out.distinguished.push_back(g);
      }
    }
  }
  bool both = false;
  for (const auto& name : out.forcing)
    for (const auto& d : b.cases)
      if (d.name == name && d.outcome != out.forced) both = true;
  const std::string claim = "the precedents force a decision for the situation " + words(x);
  if (both) {
    out.verdict = Verdict::no(claim, "precedents for both sides apply a fortiori: the case base is inconsistent");
  } else if (!out.forced.empty()) {
    std::string names;
    for (std::size_t i = 0; i < out.forcing.size(); i++) names += (i ? ", " : "") + out.forcing[i];
    out.verdict = Verdict::yes(claim, "it must be decided for " + out.forced + ": " + names + " had its reason and at least every factor against it that this situation has");
    out.verdict.with("forced", out.forced);
  } else {
    std::string why = "neither side is forced";
    for (const auto& g : out.distinguished) why += "; " + g.precedent + " is distinguished by " + words(g.by);
    out.verdict = Verdict::unknown(claim, why);
  }
  for (const auto& g : out.distinguished) out.verdict.with("distinguished", g.precedent + " by " + words(g.by));
  return out;
}

// ---- The written form ------------------------------------------------------------------------------------------------------------

struct Parsed {
  bool ok = false;
  Base base;
  std::vector<std::string> errors;
};

namespace detail {

inline std::string trim(std::string_view s) {
  std::size_t a = 0, z = s.size();
  while (a < z && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r')) a++;
  while (z > a && (s[z - 1] == ' ' || s[z - 1] == '\t' || s[z - 1] == '\r')) z--;
  return std::string(s.substr(a, z - a));
}

inline std::vector<std::string> split(const std::string& s, char by) {
  std::vector<std::string> out;
  std::size_t from = 0;
  while (from <= s.size()) {
    std::size_t to = s.find(by, from);
    if (to == std::string::npos) to = s.size();
    const auto part = trim(std::string_view(s).substr(from, to - from));
    if (!part.empty()) out.push_back(part);
    from = to + 1;
    if (to == s.size()) break;
  }
  return out;
}

}  // namespace detail

inline Parsed read(std::string_view text) {
  Parsed out;
  Base& b = out.base;
  std::size_t number = 0, from = 0;
  std::map<std::string, std::size_t> case_of;
  while (from <= text.size()) {
    std::size_t to = text.find('\n', from);
    if (to == std::string_view::npos) to = text.size();
    const std::string line = detail::trim(text.substr(from, to - from));
    from = to + 1;
    number++;
    const auto err = [&](const std::string& why) { out.errors.push_back("line " + std::to_string(number) + ": " + why); };
    if (!line.empty() && line[0] != '#') {
      const std::size_t colon = line.find(':');
      const auto head = detail::split(line.substr(0, colon == std::string::npos ? line.size() : colon), ' ');
      const std::string tail = colon == std::string::npos ? "" : detail::trim(std::string_view(line).substr(colon + 1));
      if (head.empty()) err("a line begins with a word");
      else if (head[0] == "sides" && head.size() == 3 && colon == std::string::npos) {
        if (head[1] == head[2]) err("the two sides have different names");
        b.sides = {head[1], head[2]};
      } else if (head[0] == "factor" && head.size() == 3) {
        if (head[2] != b.sides[0] && head[2] != b.sides[1]) err("the factor " + head[1] + " favours " + head[2] + ", which is not a side declared before it");
        else if (b.side_of.count(head[1])) err("the factor " + head[1] + " is declared twice");
        else b.side_of[head[1]] = head[2], b.text[head[1]] = tail;
      } else if (head[0] == "case" && head.size() == 4 && head[2] == "for" && colon != std::string::npos) {
        if (case_of.count(head[1])) err("the case " + head[1] + " is declared twice");
        else if (head[3] != b.sides[0] && head[3] != b.sides[1]) err("the case " + head[1] + " is decided for " + head[3] + ", which is not a side");
        else {
          Precedent p{head[1], {}, head[3], {}};
          for (const auto& f : detail::split(tail, ',')) {
            if (!b.side_of.count(f)) err("the factor " + f + " of " + head[1] + " is not declared");
            p.facts.insert(f);
          }
          case_of[head[1]] = b.cases.size();
          b.cases.push_back(p);
        }
      } else if (head[0] == "reason" && head.size() == 2 && colon != std::string::npos) {
        const auto it = case_of.find(head[1]);
        if (it == case_of.end()) err("no case " + head[1] + " is declared before its reason");
        else {
          auto& p = b.cases[it->second];
          for (const auto& f : detail::split(tail, ',')) {
            const auto s = b.side_of.find(f);
            if (!p.facts.count(f)) err("the reason of " + p.name + " names " + f + ", which the case does not have");
            else if (s != b.side_of.end() && s->second != p.outcome) err("the reason of " + p.name + " names " + f + ", which favours the other side");
            else p.reason.insert(f);
          }
        }
      } else {
        err("a line is \"sides a b\", \"factor name side: text\", \"case name for side: factors\" or \"reason name: factors\"");
      }
    }
    if (to == text.size()) break;
  }
  if (b.sides[0].empty()) out.errors.push_back("the two sides are declared");
  out.ok = out.errors.empty();
  return out;
}

inline Base base(std::string_view text) {
  auto p = read(text);
  if (!p.ok) {
    std::string all;
    for (const auto& e : p.errors) all += (all.empty() ? "" : "; ") + e;
    throw std::invalid_argument("law::precedent::base: " + all);
  }
  return p.base;
}

}  // namespace law::precedent
