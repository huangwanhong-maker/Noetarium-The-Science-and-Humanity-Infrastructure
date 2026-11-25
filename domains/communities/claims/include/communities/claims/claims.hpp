// Claims: whether a claim has a route, whether it was answered in its term, whether its finding is complete, and whether it was
// reviewed by another body (the design record of 2026-11-25, Sections 7.4 and 8.3; invariant G8).
//
// A claim has a route when five complements hold together: a party who can claim; a forum (the body the constitution names for
// claims); evidence (every act the claim cites stands in the chronicle); authority (some position of the forum is recognised to
// answer); and capacity (a member of the forum who is not a party to the claim, since the body recuses its members who are). A claim
// whose forum cannot be composed is referred: to the body the constitution names for review, or beyond the community. A finding is
// complete when it states the proposition, the standard, the burden and the consequence; a review is made by a member of the review
// body who took no part in the finding.

#pragma once

#include <thesmos/chronicle/chronicle.hpp>

#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace communities::claims {

using thesmos::chronicle::State;

struct Route {
  bool party = false, forum = false, evidence = false, authority = false, capacity = false;
  std::vector<std::string> missing() const {
    std::vector<std::string> m;
    if (!party) m.push_back("party");
    if (!forum) m.push_back("forum");
    if (!evidence) m.push_back("evidence");
    if (!authority) m.push_back("authority");
    if (!capacity) m.push_back("capacity");
    return m;
  }
  bool stands() const { return missing().empty(); }
};

struct Reading {
  std::string claim, status, forum, referral;
  Route route;
  std::vector<std::string> may_answer, recused;  // members of the forum who may answer it, and those recused
  long long opened = 0, term = 0;                // the turn of the claim, and the turns its answer is due within
  bool overdue = false;
  std::vector<std::string> finding_lacks;        // what the finding of its answer does not state
  bool reviewed = false;                         // reviewed by a member of the review body who took no part in the finding
};

namespace detail {
// Turns in a written term: "one round", "two rounds", ..., or a number of rounds.
inline long long turns(const std::string& written) {
  static const std::map<std::string, long long> words = {{"one", 1}, {"two", 2}, {"three", 3}, {"four", 4}, {"five", 5}, {"six", 6}};
  std::istringstream in(written);
  std::string first;
  in >> first;
  if (words.count(first)) return words.at(first);
  try {
    return std::stoll(first);
  } catch (...) {
    return 0;
  }
}
inline std::vector<std::string> list(const std::string& s) {
  std::vector<std::string> out;
  std::stringstream in(s);
  for (std::string item; std::getline(in, item, ',');) {
    while (!item.empty() && item.front() == ' ') item.erase(item.begin());
    if (!item.empty()) out.push_back(item);
  }
  return out;
}
}  // namespace detail

// Reads a claim of a community at turn `now`. `term_written` is the term in which its constitution says a claim is answered (the
// "within" of the rule that makes its forum answer); the written form keeps it with the choice rule.
inline Reading read(const State& s, const std::string& id, long long now, const std::string& term_written = "one round") {
  Reading r;
  r.claim = id;
  const auto c = s.claims.find(id);
  if (c == s.claims.end()) {
    r.status = "no such claim";
    return r;
  }
  r.status = c->second.status;
  r.forum = s.rules.claims.route;
  r.route.party = s.parties.has(c->second.claimant);
  const auto body = s.rules.bodies.find(r.forum);
  r.route.forum = body != s.rules.bodies.end();
  const thesmos::acts::Act* made = nullptr;
  const thesmos::acts::Act* answered = nullptr;
  for (const auto& a : s.acts) {
    if (a.kind == "claim" && a.field("id") == id) made = &a;
    if (a.kind == "answer" && a.field("id") == id) answered = &a;
  }
  r.opened = made ? made->turn : 0;
  r.route.evidence = true;
  if (made)
    for (const auto& seal : detail::list(made->field("cites"))) {
      bool found = false;
      for (const auto& e : s.journal.entries) found = found || e.seal == seal;
      r.route.evidence = r.route.evidence && found;
    }
  if (r.route.forum) {
    for (const auto& position : body->second.of) r.route.authority = r.route.authority || thesmos::constitutions::permits(s.rules, position, "answer");
    std::set<std::string> members;
    for (const auto& [key, since] : s.positions)
      for (const auto& position : body->second.of)
        if (key.second == position) members.insert(key.first);
    const std::string& against = c->second.against;
    for (const auto& m : members) {
      const bool party = m == c->second.claimant || m == against ||
                         (s.parties.has(m) && s.parties.has(against) && thesmos::kernel::origin(s.parties, m) == thesmos::kernel::origin(s.parties, against));
      (party ? r.recused : r.may_answer).push_back(m);
    }
    r.route.capacity = !r.may_answer.empty();
  }
  if (!r.route.capacity) r.referral = s.rules.claims.review.empty() ? "beyond the community (a league's panel or an outside forum)" : s.rules.claims.review;
  r.term = detail::turns(term_written);
  r.overdue = r.status != "answered" && now > r.opened + r.term;
  if (answered) {
    for (const char* part : {"finding", "standard", "burden", "remedy"})
      if (answered->field(part).empty()) r.finding_lacks.push_back(part);
    // A review: an assessment of the claim by a member of the review body other than whoever answered it.
    const auto review = s.rules.bodies.find(s.rules.claims.review);
    if (review != s.rules.bodies.end())
      for (const auto& x : s.assessments) {
        if (x.object != id || x.by == answered->author) continue;
        for (const auto& position : review->second.of) r.reviewed = r.reviewed || s.holds(x.by, position);
      }
  }
  return r;
}

}  // namespace communities::claims
