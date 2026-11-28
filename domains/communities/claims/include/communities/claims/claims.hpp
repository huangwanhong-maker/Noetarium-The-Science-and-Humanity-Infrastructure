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

// ---- Every item's route (thesmos's decision 0011; the design's Phase 8) ---------------------------------------------------------------
//
// Since thesmos's decision 0011 every directive has a route where a constitution declares its routes: an application, a question, a
// petition, or a type of item a community declares opens an item answered by a body or an office within a term, never by a party to it.
// The same five complements are read for any item: its author a known party; a forum (the body or office its route names); evidence
// (its acts cite what the chronicle holds); authority (the forum can answer); and capacity (some holder of the forum who is not a party to
// the item). An item whose forum cannot be composed is referred, to its route's review body or beyond the community.

struct ItemReading {
  std::string item, type, status, author, bearer;
  std::string body, office;                       // the forum its route names
  bool route_empty = false;                       // the route is declared empty, reaching no body
  Route route;                                    // its five complements
  std::vector<std::string> may_answer, recused;   // holders of the forum who may answer it, and those recused as parties to it
  long long opened = 0, due = -1;
  bool overdue = false;
  std::string closed_by, finding, referral;
};

inline ItemReading read_item(const State& s, const std::string& id, long long now) {
  ItemReading r;
  r.item = id;
  const auto it = s.items.find(id);
  if (it == s.items.end()) {
    r.status = "no such item";
    return r;
  }
  const auto& i = it->second;
  r.type = i.type, r.status = i.status, r.author = i.author, r.bearer = i.bearer, r.opened = i.opened, r.due = i.due, r.closed_by = i.closed_by, r.finding = i.finding;
  r.overdue = thesmos::chronicle::overdue(i, now);
  const auto w = thesmos::chronicle::answerer_of(s, i);
  r.body = w.body, r.office = w.office;
  if (i.type != "application") r.route_empty = thesmos::chronicle::route_of(s.rules, i.type).empty;
  r.route.party = s.parties.has(i.author);
  r.route.forum = (!w.body.empty() && s.rules.bodies.count(w.body)) || (!w.office.empty() && s.rules.positions.count(w.office));
  r.route.evidence = true;
  if (i.place < s.acts.size())
    for (const auto& seal : detail::list(s.acts[i.place].field("cites"))) {
      bool found = false;
      for (const auto& e : s.journal.entries) found = found || e.seal == seal;
      r.route.evidence = r.route.evidence && found;
    }
  // Who holds the forum, and of them who may answer: thesmos's own reading of an item's answerers (its route, recusal of its parties).
  std::set<std::string> holders;
  for (const auto& [key, since] : s.positions) {
    const bool on = (!w.body.empty() && s.member(key.first, w.body)) || (!w.office.empty() && key.second == w.office);
    if (on) holders.insert(key.first);
  }
  r.route.authority = r.route.forum && !r.route_empty;
  for (const auto& h : holders) {
    if (i.status != "open") break;
    (thesmos::chronicle::may_answer(s, i, h, h) ? r.may_answer : r.recused).push_back(h);
  }
  r.route.capacity = i.status != "open" || !r.may_answer.empty();
  if (i.status == "open" && !r.route.capacity) {
    const std::string review = i.type == "application" ? std::string() : thesmos::chronicle::route_of(s.rules, i.type).review;
    r.referral = review.empty() ? "beyond the community (a league's panel or an outside forum)" : review;
  }
  return r;
}

// The open items no party may answer, each with where it is referred: what the community's routes leave without capacity.
inline std::vector<ItemReading> unanswerable(const State& s, long long now) {
  std::vector<ItemReading> out;
  for (const auto& [id, i] : s.items)
    if (i.status == "open") {
      const auto r = read_item(s, id, now);
      if (!r.route.capacity) out.push_back(r);
    }
  return out;
}

}  // namespace communities::claims
