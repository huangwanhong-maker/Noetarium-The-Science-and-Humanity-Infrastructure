// Rules (ADR 0125; the theory note §3.6): needs that declared norms entitle, derived by forward chaining.
//
// A rule is a statement an authority holds, written in the language (`need care for assistance = needed by ...`): its group is
// who is entitled, its domain to what. Implications between domains (whoever needs care needs transport to it) are declared
// beside them. A unit is entitled to the domains of the rules whose groups hold of it, closed under the implications (morphe's
// forward chaining). A statement is then read against the rules:
//   holds:      most of the group's people (half or more) are entitled to its domain;
//   fails:      rules reach the domain, and none of the group is entitled: the gap is not one the norms recognise;
//   undecided:  no rule reaches the domain, or some of the group is entitled.
// Each entitlement keeps its derivation: the rule, and the implications it went through.
//
// Remark. A normative gap need not be an entitlement, nor an entitlement a gap; the test says whether the data's statement and the
// declared norms agree, which is what a reader of a needs assessment asks first.
#pragma once

#include <needs/evidence/causal.hpp>
#include <needs/evidence/gap.hpp>
#include <needs/evidence/parts.hpp>
#include <needs/language/reader.hpp>

#include <morphe/combinatorics/acyclic.hpp>

#include <map>
#include <string>
#include <vector>

namespace needs::evidence {

struct Rulebook {
  std::vector<language::Statement> rules;
  std::vector<std::string> sources;
  std::vector<std::string> domains;                      // every domain the rules or implications name
  std::vector<std::vector<char>> reaches;                // [domain of a rule][domain]: the closure under the implications
  std::vector<std::vector<std::size_t>> route;           // [from][to]: the domain before `to` on a shortest chain, for derivations
  std::vector<std::string> refused;                      // rules that did not read, with why
  std::size_t index(const std::string& d) const {
    for (std::size_t k = 0; k < domains.size(); k++)
      if (domains[k] == d) return k;
    return domains.size();
  }
};

inline Rulebook rulebook(const Dataset& d) {
  Rulebook b;
  auto add = [&](const std::string& dom) {
    if (b.index(dom) == b.domains.size()) b.domains.push_back(dom);
  };
  for (const auto& r : d.rules) {
    try {
      auto s = language::read_one(r.text);
      language::bind(d, s);
      for (const auto* g : {&s.group, &s.frame}) groups::check(d.table, *g);
      b.rules.push_back(s), b.sources.push_back(r.source), add(s.domain);
    } catch (const std::exception& e) {
      b.refused.push_back(r.text + ": " + e.what());
    }
  }
  for (const auto& [a, c] : d.implies) add(a), add(c);
  const std::size_t n = b.domains.size();
  std::vector<morphe::combinatorics::Rule> chain;
  for (const auto& [a, c] : d.implies) chain.push_back({{b.index(a)}, b.index(c)});
  b.reaches.assign(n, std::vector<char>(n, 0));
  b.route.assign(n, std::vector<std::size_t>(n, n));
  for (std::size_t k = 0; k < n; k++) {
    b.reaches[k] = morphe::combinatorics::closure(n, chain, {k});
    // A shortest chain from k, for the derivation shown (breadth first over the implications).
    std::vector<std::size_t> frontier{k};
    std::vector<char> seen(n, 0);
    seen[k] = 1;
    while (!frontier.empty()) {
      std::vector<std::size_t> next;
      for (std::size_t u : frontier)
        for (const auto& r : chain)
          if (r.premises[0] == u && !seen[r.conclusion]) seen[r.conclusion] = 1, b.route[k][r.conclusion] = u, next.push_back(r.conclusion);
      frontier = next;
    }
  }
  return b;
}

struct Entitlement {
  bool applicable = false;               // some rule reaches the domain
  Verdict verdict = Verdict::undecided;
  std::string why;
  double share = nan;                    // of the group's people, entitled to the domain
  std::vector<std::string> derivations;  // "rule (source) ⇒ care ⇒ transport"
};

inline Entitlement entitlement(const Rulebook& b, const Dataset& d, const language::Statement& s, const Parts& parts, Part part) {
  Entitlement out;
  const std::size_t target = b.index(s.domain);
  if (target == b.domains.size()) {
    out.why = "no rule reaches " + s.domain;
    return out;
  }
  std::vector<std::size_t> reaching;  // rules whose domain reaches the target
  for (std::size_t k = 0; k < b.rules.size(); k++)
    if (b.reaches[b.index(b.rules[k].domain)][target]) reaching.push_back(k);
  if (reaching.empty()) {
    out.why = "no rule reaches " + s.domain;
    return out;
  }
  out.applicable = true;
  const auto& t = d.table;
  const groups::Rows frame = groups::rows_of(t, s.frame);
  const groups::Rows g = groups::rows_of(t, s.group, frame);
  std::vector<groups::Rows> held;
  for (std::size_t k : reaching) held.push_back(groups::rows_of(t, b.rules[k].group, groups::rows_of(t, b.rules[k].frame)));
  // The people a row stands for, read through the statement's indicator.
  std::vector<double> people(t.rows, 0.0);
  if (s.standard != language::Standard::expressed) {
    const auto& ind = d.indicator(s.indicator);
    const std::vector<double>* x = ind.total.empty() ? nullptr : &t.at(ind.total).numbers;
    for (std::size_t i = 0; i < t.rows; i++) people[i] = t.w(i) * (x ? ((*x)[i] > 0 ? (*x)[i] : 0.0) : 1.0);
  } else {
    for (std::size_t i = 0; i < t.rows; i++) people[i] = t.w(i);
  }
  double all = 0, entitled = 0;
  std::vector<char> used(reaching.size(), 0);
  for (std::size_t i = 0; i < t.rows; i++) {
    if (!g[i] || parts.part_of_row[i] != static_cast<int>(part)) continue;
    all += people[i];
    bool e = false;
    for (std::size_t k = 0; k < reaching.size(); k++)
      if (held[k][i]) e = true, used[k] = 1;
    if (e) entitled += people[i];
  }
  if (!(all > 0)) {
    out.applicable = false;
    out.why = "the group has nobody in this part";
    return out;
  }
  out.share = entitled / all;
  for (std::size_t k = 0; k < reaching.size(); k++) {
    if (!used[k]) continue;
    const auto& r = b.rules[reaching[k]];
    std::string chain = s.domain;
    for (std::size_t at = target, from = b.index(r.domain); at != from;) {
      at = b.route[from][at];
      if (at == b.domains.size()) break;
      chain = b.domains[at] + " ⇒ " + chain;
    }
    out.derivations.push_back("for " + r.group.write() + (b.sources[reaching[k]].empty() ? "" : " (" + b.sources[reaching[k]] + ")") + " ⇒ " + chain);
  }
  if (out.share >= 0.5) out.verdict = Verdict::holds, out.why = "most of the group is entitled to " + s.domain + " by the declared rules";
  else if (entitled == 0) out.verdict = Verdict::fails, out.why = "rules reach " + s.domain + ", and none of the group is entitled";
  else out.why = "some of the group is entitled to " + s.domain;
  return out;
}

}  // namespace needs::evidence
