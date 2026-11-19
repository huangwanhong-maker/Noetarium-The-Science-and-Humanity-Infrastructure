// Checks of review cycles (reviews.hpp).
//
// - A record's faults, each named.
// - A party reviewed in two cycles: each recommendation's state at a date, its implementation claimed by the party alone, evidenced
//   by another with its evidence named, or contested by a reporter who says otherwise; a party that reports progress over time read
//   at its latest report, and not taken to contest itself.
// - Follow-up: a recommendation repeated in the next cycle, declared so or matched by who recommends to whom on which theme.
// - The measures of a party and a cycle: counts by response and state, and the share supported of those answered.
// - The signs at a date: unanswered, unreported by the next cycle, claimed only by the party reviewed, contested, recommended again
//   though said to be done, and two parties that recommend each other only mildly.
//
// Every party and recommendation here is invented.

#include <diplomacy/reviews/reviews.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace diplomacy::reviews;

namespace {

Record record() {
  Record r;
  r.cycles = {{"c1", "2017-05-01", "2017-09-30"}, {"c2", "2022-05-01", "2022-09-30"}};
  r.recommendations = {{"r1", "c1", "south", "north", "open schools to every child", "schools", 5, ""},
                       {"r2", "c1", "east", "north", "bring water to the villages", "water", 4, ""},
                       {"r3", "c1", "west", "north", "free the press", "press", 3, ""},
                       {"r4", "c1", "south", "north", "speed the courts", "courts", 2, ""},
                       {"r5", "c1", "east", "north", "inspect the prisons", "prisons", 5, ""},
                       {"r6", "c1", "west", "north", "fund clinics", "health", 3, ""},
                       {"r7", "c2", "south", "north", "open schools to every child, again", "schools", 5, ""},
                       {"r12", "c2", "east", "north", "keep the water running", "water services", 0, "r2"},
                       {"r10", "c2", "east", "west", "consider a review", "general", 1, ""},
                       {"r11", "c2", "west", "east", "continue efforts", "general", 2, ""}};
  r.answers = {{"r1", "2017-09-20", "north", Response::supported}, {"r2", "2017-09-20", "north", Response::supported},
               {"r3", "2017-09-20", "north", Response::noted},     {"r4", "2017-09-20", "north", Response::supported},
               {"r5", "2017-09-20", "north", Response::supported}, {"r7", "2022-09-01", "north", Response::supported},
               {"r12", "2022-09-01", "north", Response::supported}, {"r10", "2022-09-01", "west", Response::supported},
               {"r11", "2022-09-01", "east", Response::supported}};
  r.progress = {{"r1", "2019-01-01", "north", Done::implemented, {}},
                {"r2", "2018-06-01", "north", Done::partly, {}},
                {"r2", "2019-01-01", "north", Done::implemented, {}},
                {"r2", "2020-03-01", "east", Done::implemented, {"a household survey of 2019"}},
                {"r4", "2019-01-01", "north", Done::implemented, {}},
                {"r4", "2020-03-01", "west", Done::not_implemented, {"a report of the bar"}}};
  return r;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    s.require("the record here is declared without fault", problems(record()).empty());
    Record r = record();
    r.recommendations.push_back({"x1", "c9", "east", "east", "", "", 0, ""});
    r.recommendations.push_back({"x2", "c1", "east", "north", "", "", 0, "r7"});
    r.answers.push_back({"none", "2020-01-01", "", Response::noted});
    r.progress.push_back({"r1", "someday", "north", Done::implemented, {}});
    const auto ps = problems(r);
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a record's faults are named: a cycle not recorded, a party recommending to itself, a repeat of no earlier cycle, an answer to nothing, a report on no date",
              has("c9 is not recorded") && has("recommends to itself") && has("of no earlier cycle") && has("no such recommendation") && has("no calendar date"), std::to_string(ps.size()) + " faults");
  }

  // ---- States ----
  {
    const Record r = record();
    const std::string at = "2023-01-01";
    s.require("an implementation claimed by the party reviewed alone is claimed", state(r, "r1", at).standing == Standing::claimed && state(r, "r1", at).words == "supported, implemented (claimed)",
              state(r, "r1", at).words);
    s.require("one another party reports with its evidence named is evidenced", state(r, "r2", at).standing == Standing::evidenced);
    s.require("a party reporting in part, then done, is read at its latest and does not contest itself", state(r, "r2", "2019-06-01").standing == Standing::claimed);
    s.require("a reporter who says it is not done contests it", state(r, "r4", at).standing == Standing::contested && state(r, "r4", at).words == "supported, not implemented (contested)",
              state(r, "r4", at).words);
    s.require("noted, supported and unreported, and unanswered are told apart",
              state(r, "r3", at).words == "noted" && state(r, "r5", at).words == "supported, its implementation not reported" && state(r, "r6", at).words == "unanswered");
    s.require("read at an earlier date, a recommendation is as it stood then", state(r, "r1", "2017-06-01").words == "unanswered");
  }

  // ---- Follow-up ----
  {
    const auto rp = repeats(record());
    bool matched = false, declared = false;
    for (const auto& x : rp) {
      matched = matched || (x.later == "r7" && x.earlier == "r1" && !x.declared);
      declared = declared || (x.later == "r12" && x.earlier == "r2" && x.declared);
    }
    s.require("a recommendation repeated in the next cycle is found, matched by who recommends to whom on which theme, or declared", matched && declared && rp.size() == 2,
              std::to_string(rp.size()) + " repeats");
  }

  // ---- Measures ----
  {
    const Measures m = measures(record(), "north", "c1", "2023-01-01");
    s.require("north's first cycle: six recommendations, four supported of the five answered", m.recommendations == 6 && m.by_response.at("supported") == 4 && m.by_response.at("noted") == 1 &&
                                                                                                     m.by_response.at("no response") == 1 && std::fabs(m.share_supported - 0.8) < 1e-15);
  }

  // ---- Signs ----
  {
    const auto sg = signs(record(), "2023-01-01", {90, 2});
    auto has = [&](const std::string& kind, const std::string& where) {
      for (const auto& x : sg)
        if (x.kind == kind && x.where == where) return true;
      return false;
    };
    s.require("the signs are found where they show: claimed by the party alone and recommended again, contested, unreported, unanswered, said done and repeated, mutually mild",
              has("claimed", "recommendation r1") && has("recommended again", "recommendation r1") && has("contested", "recommendation r4") && has("unreported", "recommendation r5") &&
                  has("unanswered", "recommendation r6") && has("recommended again", "recommendation r2") && has("mutually mild", "parties east and west") && sg.size() == 7,
              std::to_string(sg.size()) + " signs");
    s.require("before the next cycle opens, a supported recommendation not yet reported is no sign", [&] {
      for (const auto& x : signs(record(), "2021-01-01"))
        if (x.kind == "unreported") return false;
      return true;
    }());
  }
  return s.done();
}
