// Checks of an undertaking as a typed dependency structure (relations.hpp).
//
// - Its declaration's faults, each named.
// - A tie's status: present when recorded, absent when recorded absent or when a declared design of observation covered both ends,
//   and otherwise not observed; never absent for want of a record. The pairs of a type counted by status.
// - Ties in time: a tie that has ended is not present after it, and a position read at an earlier date is the earlier one.
// - A participant's relational position: what authorises it, what it relies on, the decisions it holds, is affected by, can contest.
// - The undertaking's diagnostics (the monograph's Table 10.2): each kind raised on a case built to raise it, and none on a case
//   built to raise none.
// - Succession held to removal: on 300 random undertakings with ties of every kind, some ended and some recorded absent, the
//   participants named as holding the relations of authority together are exactly those whose removal leaves more components of
//   the ties of authority in force, and the ties named as the only link exactly those whose removal does.
//
// Every participant and decision here is invented.

#include <diplomacy/relations/relations.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <numeric>
#include <set>
#include <string>
#include <vector>

using namespace diplomacy::relations;

namespace {

std::vector<RelationType> kinds() {
  return {{"authorises", "one gives the other authority to act for the undertaking", true, false, false},
          {"relies", "one relies on the other's claims", false, true, false},
          {"challenges", "one may contest the other's decisions", false, false, true},
          {"informs", "one tells the other", false, false, false}};
}

// An invented programme: a fund (constituted) authorises a secretariat, which authorises two cities' offices; a community is
// affected; a model and a dataset supply evidence; a review panel hears challenges.
Undertaking programme() {
  Undertaking u;
  u.name = "an invented programme of water services in two cities";
  u.types = kinds();
  u.participants = {{"fund", "the fund", "organisation", true},
                    {"secretariat", "the secretariat", "organisation", false},
                    {"north", "the northern city's office", "city", false},
                    {"south", "the southern city's office", "city", false},
                    {"residents", "the residents' association", "community", false},
                    {"model", "the forecasting model", "AI system", false},
                    {"survey", "the household survey", "dataset", false},
                    {"panel", "the review panel", "forum", true}};
  u.ties = {{"authorises", "fund", "secretariat", "2025-01-01", "", false, "the founding agreement"},
            {"authorises", "secretariat", "north", "2025-03-01", "", false, "a delegation"},
            {"authorises", "secretariat", "south", "2025-03-01", "", false, "a delegation"},
            {"relies", "secretariat", "model", "2025-06-01", "", false, "the work plan"},
            {"relies", "north", "model", "2025-06-01", "", false, "the work plan"},
            {"relies", "south", "model", "2025-06-01", "", false, "the work plan"},
            {"relies", "south", "survey", "2025-06-01", "", false, "the work plan"},
            {"challenges", "north", "panel", "2025-01-01", "", false, "the rules of review"},
            {"informs", "secretariat", "residents", "2025-01-01", "2025-09-01", false, "the outreach plan"},
            {"informs", "north", "residents", "", "", true, "the northern office's minutes: it reports no contact"}};
  u.observations = {{"informs", "the outreach log", "2025-01-01", "2026-01-01", {"secretariat", "north", "south", "residents"}}};
  u.decisions = {{"d1", "tariffs set", "2025-07-01", "the secretariat's own rule", "provisional", {"secretariat"}, {"model", "survey"}, {"north", "south", "residents"}, {"panel"}, "the survey shows ability to pay"},
                 {"d2", "pipes replaced first in the south", "2025-08-01", "", "provisional", {"model"}, {"model", "model"}, {"south"}, {}, ""},
                 {"d3", "a meeting moved", "2025-09-01", "", "final", {"north"}, {}, {"north"}, {"panel"}, ""}};
  u.constraints = {{"c1", "no tariff above 3% of a household's income", "fund"}};
  return u;
}

bool has(const std::vector<Diagnosis>& ds, const std::string& kind, const std::string& where_part, const std::string& what_part = "") {
  for (const auto& d : ds)
    if (d.kind == kind && d.where.find(where_part) != std::string::npos && d.what.find(what_part) != std::string::npos) return true;
  return false;
}

std::size_t count(const std::vector<Diagnosis>& ds, const std::string& kind) {
  return static_cast<std::size_t>(std::count_if(ds.begin(), ds.end(), [&](const Diagnosis& d) { return d.kind == kind; }));
}

// Components among the given vertices of a graph on edges (a, b), by union–find.
std::size_t components(const std::vector<std::size_t>& vertices, const std::vector<std::pair<std::size_t, std::size_t>>& edges, std::size_t n) {
  std::vector<std::size_t> up(n);
  std::iota(up.begin(), up.end(), 0);
  std::function<std::size_t(std::size_t)> find = [&](std::size_t x) { return up[x] == x ? x : up[x] = find(up[x]); };
  for (const auto& [a, b] : edges) up[find(a)] = find(b);
  std::set<std::size_t> roots;
  for (auto v : vertices) roots.insert(find(v));
  return roots.size();
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Undertaking u = programme();

  // ---- The declaration ------------------------------------------------------------------------------------------------------------
  s.require("the invented programme is declared without fault", problems(u).empty(), problems(u).empty() ? "" : problems(u)[0].what);
  {
    Undertaking bad = u;
    bad.participants.push_back({"fund", "a second fund", "organisation", false});
    bad.ties.push_back({"funds", "fund", "north", "", "", false, ""});
    bad.ties.push_back({"informs", "north", "nobody", "", "", false, ""});
    bad.ties.push_back({"relies", "south", "south", "", "", false, ""});
    bad.ties.push_back({"relies", "north", "survey", "2026-01-01", "2025-01-01", false, ""});
    bad.decisions.push_back({"d9", "", "", "", "provisional", {"ghost"}, {}, {}, {}, ""});
    const auto ps = problems(bad);
    s.require("its faults are found, each named: a participant declared twice, a type not declared, a tie to no one, a tie to itself, a tie that ends before it begins, a decision by no one declared",
              ps.size() == 6, std::to_string(ps.size()) + (ps.empty() ? "" : ": " + ps[0].where + ", " + ps[0].what));
  }

  // ---- Status: present, absent, not observed ----------------------------------------------------------------------------------------
  s.require("a recorded tie is present", status(u, "authorises", "fund", "secretariat", "2025-06-01") == Status::present);
  s.require("a tie recorded absent is absent, whatever the design", status(u, "informs", "north", "residents", "2025-06-01") == Status::absent);
  s.require("within the outreach log's design a pair with no tie recorded is absent", status(u, "informs", "south", "secretariat", "2025-06-01") == Status::absent);
  s.require("outside the design's participants it is not observed, never absent", status(u, "informs", "fund", "north", "2025-06-01") == Status::not_observed);
  s.require("after the design's period it is not observed", status(u, "informs", "south", "secretariat", "2026-03-01") == Status::not_observed);
  s.require("a type with no design: every unrecorded pair is not observed", status(u, "relies", "fund", "survey", "2025-06-01") == Status::not_observed);
  {
    const Layer l = layer(u, "informs", "2025-06-01");
    // 8 participants, 56 ordered pairs; the design covers 4 participants (12 ordered pairs), of which 1 is present
    // (secretariat informs residents) and 11 absent (north–residents recorded absent among them); 44 not observed.
    s.require("the pairs of informing on 1 June 2025: 1 present, 11 absent, 44 not observed",
              l.present == 1 && l.absent == 11 && l.not_observed == 44, std::to_string(l.present) + " " + std::to_string(l.absent) + " " + std::to_string(l.not_observed));
    s.check("so 12 of its 56 ordered pairs are observed", std::abs(l.observed() - 12.0 / 56.0), 1e-15);
    const Layer l2 = layer(u, "informs", "2025-10-01");
    s.require("after the outreach tie ends (1 September 2025) the secretariat no longer informs the residents: absent within the design",
              status(u, "informs", "secretariat", "residents", "2025-10-01") == Status::absent && l2.present == 0 && l2.absent == 12);
  }

  // ---- Position ------------------------------------------------------------------------------------------------------------------
  {
    const Position p = position(u, "secretariat", "2025-07-15");
    s.require("the secretariat is authorised by the fund, holds no authority of its own constitution, and authorises two offices",
              p.authorised_by == std::vector<std::string>{"fund"} && !p.constituted && p.out.at("authorises").size() == 2 && p.formal_authority());
    s.require("it relies on the model, and decides d1", p.relies_on == std::vector<std::string>{"model"} && p.decides == std::vector<std::string>{"d1"});
    const Position early = position(u, "secretariat", "2025-02-01");
    s.require("read on 1 February 2025 it has authorised no one yet and relies on nothing", !early.out.count("authorises") && early.relies_on.empty() && early.decides.empty());
    const Position r = position(u, "residents", "2025-07-15");
    s.require("the residents are affected by d1 and can contest nothing: no tie of challenge", r.affected_by == std::vector<std::string>{"d1"} && r.may_challenge.empty());
    const Position m = position(u, "model", "2025-07-15");
    s.require("the model is relied on by three participants", m.relied_on_by.size() == 3);
  }

  // ---- Diagnostics -----------------------------------------------------------------------------------------------------------------
  {
    const auto ds = diagnose(u, "2025-10-01");
    s.require("evidence: d3 names none", has(ds, "evidence", "d3", "no evidence") && count(ds, "evidence") == 1);
    s.require("authorisation: the model decided d2 holding no authority (a recommendation recorded as a decision)", has(ds, "authorisation", "d2", "forecasting model"));
    s.require("the authorised secretariat and the delegated northern office raise no fault of authorisation", count(ds, "authorisation") == 1);
    s.require("participation: d2 names no venue of challenge", has(ds, "participation", "d2", "no venue"));
    s.require("participation: the southern office and the residents are affected by d1 with no route to the panel; the northern office has one",
              has(ds, "participation", "d1", "southern city") && has(ds, "participation", "d1", "residents") && !has(ds, "participation", "d1", "northern"));
    s.require("AI dependency: d2 rests on the model alone, and counts it twice", has(ds, "AI dependency", "d2", "AI systems alone") && has(ds, "AI dependency", "d2", "more than once"));
    s.require("d1 rests on a model and a survey: no fault of AI dependency", !has(ds, "AI dependency", "d1"));
    s.require("succession: the secretariat holds authority together, and each delegation is the only link",
              has(ds, "succession", "participant secretariat") && count(ds, "succession") == 4 && has(ds, "succession", "tie fund authorises secretariat"));
    s.require("concentration: the model receives 3 of 4 ties of reliance", has(ds, "concentration", "participant model", "3 of 4"));
    s.require("unobserved: authorising, relying and challenging have no design; informing has one",
              has(ds, "unobserved", "type authorises") && has(ds, "unobserved", "type relies") && has(ds, "unobserved", "type challenges") && !has(ds, "unobserved", "type informs"));
    const auto before = diagnose(u, "2025-06-15");
    s.require("read before the decisions are made, they raise nothing", count(before, "evidence") == 0 && count(before, "authorisation") == 0 && count(before, "participation") == 0);
  }
  {
    // A sound undertaking: two constituted parties authorise each other and a joint office through a triangle of authority, every
    // decision rests on two sources, names a venue, and everyone affected can reach it; every type is observed.
    Undertaking v;
    v.types = kinds();
    v.participants = {{"a", "A", "state", true}, {"b", "B", "state", true}, {"o", "the joint office", "organisation", false},
                      {"x", "a study", "dataset", false}, {"y", "a second study", "dataset", false}, {"f", "the forum", "forum", true}};
    v.ties = {{"authorises", "a", "o"}, {"authorises", "b", "o"}, {"authorises", "a", "b"}, {"relies", "o", "x"}, {"relies", "o", "y"},
              {"challenges", "a", "f"}, {"challenges", "b", "f"}, {"informs", "o", "a"}};
    std::vector<std::string> all = {"a", "b", "o", "x", "y", "f"};
    for (const auto& t : v.types) v.observations.push_back({t.name, "a full register", "", "", all});
    v.decisions = {{"d", "a joint plan", "2025-05-01", "consensus", "provisional", {"o"}, {"x", "y"}, {"a", "b"}, {"f"}, ""}};
    const auto ds = diagnose(v, "2025-06-01", {0.75, "AI system"});
    s.require("a sound undertaking raises no diagnostic", ds.empty(), ds.empty() ? "" : ds[0].kind + ": " + ds[0].what);
  }

  // ---- Succession held to removal on random undertakings -------------------------------------------------------------------------
  {
    calx::random::Stream rng(20261119, 132, 0, 0);
    const std::vector<std::string> dates = {"2025-01-01", "2025-04-01", "2025-07-01", "2025-10-01"};
    std::size_t bad = 0, cases = 0, cuts = 0, bridges = 0;
    std::string why;
    for (int k = 0; k < 300; k++) {
      Undertaking r;
      r.types = kinds();
      const std::size_t n = 2 + rng.below(9);
      for (std::size_t i = 0; i < n; i++) r.participants.push_back({"p" + std::to_string(i), "", "organisation", rng.below(3) == 0});
      const std::size_t m = rng.below(2 * n + 2);
      for (std::size_t e = 0; e < m; e++) {
        const std::size_t a = rng.below(n), b = rng.below(n);
        if (a == b) continue;
        Tie t{r.types[rng.below(4)].name, "p" + std::to_string(a), "p" + std::to_string(b)};
        if (rng.below(4) == 0) t.since = dates[rng.below(4)];
        if (rng.below(4) == 0) t.until = "2025-12-01";
        t.absent = rng.below(6) == 0;
        if (!t.since.empty() && !t.until.empty() && !(t.since < t.until)) t.until.clear();
        r.ties.push_back(t);
      }
      const std::string at = "2025-08-01";
      const auto ds = diagnose(r, at);
      // the ties of authority in force at the date, by brute force
      std::vector<std::pair<std::size_t, std::size_t>> E;
      std::vector<std::size_t> E_tie;
      for (std::size_t e = 0; e < r.ties.size(); e++) {
        const Tie& t = r.ties[e];
        if (t.type != "authorises" || t.absent || !holds_on(t.since, t.until, at)) continue;
        E.push_back({std::stoul(t.from.substr(1)), std::stoul(t.to.substr(1))});
        E_tie.push_back(e);
      }
      std::vector<std::size_t> V(n);
      std::iota(V.begin(), V.end(), 0);
      const std::size_t base = components(V, E, n);
      for (std::size_t v = 0; v < n; v++) {
        bool touched = false;
        std::vector<std::pair<std::size_t, std::size_t>> rest;
        for (const auto& [a, b] : E) {
          if (a == v || b == v) touched = true;
          else rest.push_back({a, b});
        }
        std::vector<std::size_t> others;
        for (auto w : V)
          if (w != v) others.push_back(w);
        // removing a participant alone in its component removes that component; any other is a cut participant when the others
        // then fall into more components than the whole had
        const bool truth = touched && components(others, rest, n) > base;
        bool named = false;  // by exact name, so that "participant p1" is not taken for "participant p10"
        for (const auto& d : ds)
          if (d.kind == "succession" && d.where == "participant p" + std::to_string(v)) named = true;
        if (named != truth) bad++, why = "participant p" + std::to_string(v) + " in case " + std::to_string(k);
        cuts += truth;
      }
      for (std::size_t e = 0; e < E.size(); e++) {
        std::vector<std::pair<std::size_t, std::size_t>> rest;
        for (std::size_t f = 0; f < E.size(); f++)
          if (f != e) rest.push_back(E[f]);
        const bool truth = components(V, rest, n) > base;
        const Tie& t = r.ties[E_tie[e]];
        bool named = false;
        for (const auto& d : ds)
          if (d.kind == "succession" && d.where == "tie " + t.from + " authorises " + t.to) named = true;
        if (named != truth) bad++, why = "tie " + t.from + " " + t.to + " in case " + std::to_string(k);
        bridges += truth;
      }
      cases++;
    }
    s.require("on 300 random undertakings the participants and ties named in succession are exactly those whose removal leaves more components of authority",
              bad == 0, std::to_string(cases) + " cases, " + std::to_string(cuts) + " cut participants, " + std::to_string(bridges) + " only links" + (bad ? "; " + why : ""));
  }
  return s.done();
}
