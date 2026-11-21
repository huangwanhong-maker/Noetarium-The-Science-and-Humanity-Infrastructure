// Checks of policy/learning on an invented cohort of three participants whose every answer is worked by hand: the needs each
// activity reached and the need none did; seven actions read at a date, one in each state; the pace of completion by Kaplan and
// Meier's estimator with open and dropped actions censored; the highest result each participant reached at a sufficient grade,
// a result below it set aside; contribution claims substantiated, limited by a weak step, or held back by a rival not addressed;
// formats compared by the credit of substantiated claims shared equally, and their frontier; asks answered and open; and records
// that name what the programme lacks refused.

#include <policy/learning/learning.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace policy::learning;

namespace {

Programme cohort() {
  Programme p;
  p.name = "an invented cohort";
  p.ladder = {"reaction", "learning", "behaviour", "results"};
  p.grades = {"asserted", "corroborated", "verified"};
  p.sufficient = "corroborated";
  p.participants = {{"A", "city A", ""}, {"B", "city B", ""}, {"C", "city C", ""}};
  p.needs = {{"nA1", "A", "budgeting for maintenance", "finance", "a scorecard", 3}, {"nA2", "A", "flood warning", "resilience", "the city's words", 1},
             {"nB1", "B", "budgeting for maintenance", "finance", "a scorecard", 2}, {"nC1", "C", "street design", "streets", "the city's words", 2}};
  p.activities = {{"x1", "workshop", "maintenance budgeting", 10, 10, {}, {"nA1", "nB1"}},
                  {"x2", "study visit", "flood warning in practice", 30, 40, {"A", "B"}, {"nA2"}},
                  {"x3", "advisory", "a plan reviewed", 60, 5, {"C"}, {}}};
  auto act = [](std::string id, std::string who, double planned, double due, std::vector<Update> u) {
    Action a;
    a.id = id, a.participant = who, a.planned = planned, a.due = due, a.updates = u;
    return a;
  };
  p.actions = {act("a1", "A", 0, 50, {{40, "completed", "", "the city's report"}}),
               act("a2", "A", 10, 50, {{20, "started", "", ""}, {60, "completed", "", ""}}),
               act("a3", "B", 0, 120, {{30, "started", "", ""}}),
               act("a4", "B", 0, 80, {{20, "started", "", ""}}),
               act("a5", "C", 50, 150, {}),
               act("a6", "C", 20, 90, {{70, "dropped", "no budget", ""}}),
               act("a7", "C", 60, never, {})};
  p.results = {{"r1", "A", "behaviour", "maintenance budgeted by asset", "the budget of the year", "corroborated", 80, {"x1"}},
               {"r2", "A", "results", "roads in better condition", "the city's word", "asserted", 90, {}},
               {"r3", "B", "learning", "staff able to cost a warning system", "a test after the visit", "verified", 45, {"x2"}}};
  p.claims = {
      {"c1", "r1", {{"x1", "skills in asset budgeting", "verified", "tests before and after"}, {"skills in asset budgeting", "r1", "corroborated", "the budget's lines"}},
       {{"a national reform", "corroborated", "the reform came a year later"}}},
      {"c2", "r3", {{"x2", "r3", "asserted", "participants' word"}}, {}},
      {"c3", "r1", {{"x2", "r1", "verified", "the visit's notes"}}, {{"a consultant's advice", "", ""}}},
      {"c4", "r3", {{"x3", "r3", "verified", "the review's record"}}, {}},
      {"c5", "r3", {{"x1", "x2", "corroborated", "the visit asked for after the workshop"}, {"x2", "r3", "verified", "a test after the visit"}}, {}},
  };
  p.asks = {{"k1", "A", "a template", 30, 35, "sent"}, {"k2", "B", "an expert", 40, 52, "introduced"}, {"k3", "C", "a contact", 90, never, ""}};
  return p;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto p = cohort();
  {
    const auto c = coverage(p);
    s.require("needs reached: A's two and B's one by the workshop and the visit; C's need by none, named unmet",
              c.unmet.size() == 1 && c.unmet[0] == "nC1" && c.share_by_participant.at("A") == 1 && c.share_by_participant.at("B") == 1 && c.share_by_participant.at("C") == 0);
    s.require("... an activity reaches a need only when it includes its participant (the advisory, for C alone, addressed none of C's needs)", c.needs[3].activities.empty());
  }
  {
    const auto a = actions_at(p, 100);
    const State want[] = {State::completed_on_time, State::completed_late, State::in_progress, State::overdue, State::not_yet_due, State::dropped, State::not_started};
    bool all = a.actions.size() == 7;
    for (std::size_t k = 0; all && k < 7; k++) all = a.actions[k].state == want[k];
    s.require("seven actions read on day 100: on time, late, in progress, overdue, not yet due, dropped, not started", all);
    s.require("... each participant's share completed among the actions not dropped: A 1, B 0, C 0",
              a.completed_share.at("A") == 1 && a.completed_share.at("B") == 0 && a.completed_share.at("C") == 0);
    // Pace: completions at 40 and 50 days; censored at 40 (a7), 50 (a5 open, a6 dropped at 70 after planning at 20) and 100 (a3, a4).
    // At 40, seven at risk and one completes: 6/7; at 50, five at risk and one completes: 6/7 · 4/5 = 24/35.
    s.check("the pace by Kaplan and Meier: the share not yet completed after 50 days, 24/35, open and dropped actions censored",
            a.pace.survival.back() - 24.0 / 35, 1e-12);
    // At day 45 a2 is still open (completed on day 60), so the read is of what was known then.
    s.require("... and read on day 45, the action completed on day 60 is in progress, not completed", actions_at(p, 45).actions[1].state == State::in_progress);
  }
  {
    const auto L = ladder(p);
    s.require("the highest result each reached at the sufficient grade: A behaviour (its asserted result set aside), B learning, C none",
              L.participants[0].highest == 2 && L.participants[1].highest == 1 && L.participants[2].highest == -1 && L.set_aside == 1 && L.none == 1 &&
                  L.reached[1] == 1 && L.reached[2] == 1 && L.reached[3] == 0);
  }
  {
    const auto c1 = standing(p, p.claims[0]), c2 = standing(p, p.claims[1]), c3 = standing(p, p.claims[2]);
    s.require("a claim substantiated: every step corroborated or verified, its rival addressed; the weakest step limits it",
              c1.substantiated && c1.weakest == 1 && c1.limiting == "skills in asset budgeting -> r1");
    s.require("a claim resting on participants' word alone is not substantiated, and says which step limits it", !c2.substantiated && c2.weakest == 0 && c2.why.find("asserted") != std::string::npos);
    s.require("a claim whose steps are verified but whose rival is not addressed is not substantiated, the rival named",
              !c3.substantiated && c3.unaddressed.size() == 1 && c3.unaddressed[0] == "a consultant's advice");
  }
  {
    const auto f = formats(p);
    auto row = [&](const std::string& n) {
      for (const auto& r : f)
        if (r.format == n) return r;
      return FormatRow{};
    };
    const auto w = row("workshop"), v = row("study visit"), a = row("advisory");
    s.require("formats credited by substantiated claims shared equally: the workshop 1.5, the visit 0.5, the advisory 1",
              std::fabs(w.credit - 1.5) < 1e-12 && std::fabs(v.credit - 0.5) < 1e-12 && std::fabs(a.credit - 1) < 1e-12);
    s.require("... the frontier: the workshop and the advisory, the visit doing less for more", w.frontier && a.frontier && !v.frontier);
  }
  {
    const auto k = asks_at(p, 100);
    s.require("asks on day 100: three asked, two answered in a median of 8.5 days, C's open", k.asked == 3 && k.answered == 2 && k.median_days == 8.5 && k.open.size() == 1 && k.open[0] == "k3");
  }
  {
    auto refused = [](auto f) {
      try {
        f();
      } catch (const std::invalid_argument&) {
        return true;
      }
      return false;
    };
    auto q = p;
    q.results[0].level = "impact";
    s.require("a result on a level the ladder does not have is refused", refused([&] { ladder(q); }));
    q = p;
    q.claims[0].chain.back().to = "r3";
    s.require("a chain that does not end at the result it claims is refused", refused([&] { coverage(q); }));
    q = p;
    q.actions[0].participant = "D";
    s.require("an action of a participant the programme does not have is refused", refused([&] { actions_at(q, 100); }));
  }
  return s.done();
}
