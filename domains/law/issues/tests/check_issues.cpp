// Checks of the plan of proof: the park bylaw with a counts-as link for a bicycle and a car and an exception for an emergency. The
// facts the question turns on read off the norms; the ways to establish the ban and to resist it, worked by hand; how a binding
// holding and a fact decided against change them; and the plan refused above its limit.

#include <law/authority/authority.hpp>
#include <law/issues/issues.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <set>
#include <string>

using namespace law;
namespace ai = apodeixis::interpretation;

namespace {

facts::Fact stands(const std::string& id, facts::Standing st) {
  facts::Fact f = facts::fact(id, id);
  f.kind = "Social";
  f.standing = st;
  f.basis = "the record", f.emerged_from = "the events in the park", f.since = "2026-05-03", f.constrains = {"prohibited"};
  return f;
}

norms::Norms park() {
  norms::Norms n;
  n.rules = {{"ban", "", {"vehicle", "in_park"}, "prohibited"}};
  n.exceptions = {{"emergency_exception", "ban", {"emergency"}}};
  n.terms = {{"vehicle", {"broad", "narrow"}}};
  n.counts = {{"bicycle_counts", "bicycle", "vehicle", "broad", "vehicle"}, {"car_broad", "car", "vehicle", "broad", "vehicle"}, {"car_narrow", "car", "vehicle", "narrow", "vehicle"}};
  return n;
}

// A way written as one line, its parts sorted: "+a +b -c [t=r]".
std::string line(const issues::Way& w) {
  std::vector<std::string> parts;
  for (const auto& x : w.prove) parts.push_back("+" + x);
  for (const auto& x : w.disprove) parts.push_back("-" + x);
  for (const auto& x : w.readings) parts.push_back(x);
  std::sort(parts.begin(), parts.end());
  std::string s;
  for (const auto& p : parts) s += (s.empty() ? "" : " ") + p;
  return s;
}

std::set<std::string> lines(const std::vector<issues::Way>& ws) {
  std::set<std::string> out;
  for (const auto& w : ws) out.insert(line(w));
  return out;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const issues::Issue issue{"ban", "prohibited", "the council", "balance"};

  s.require("the ban turns on four facts no rule gives: a bicycle, a car, an emergency, being in the park",
            issues::relevant(park(), "prohibited") == std::vector<std::string>{"bicycle", "car", "emergency", "in_park"});

  facts::Record r;
  r.facts = {stands("in_park", facts::Standing::admitted), stands("bicycle", facts::Standing::alleged)};
  const auto base = norms::subsume(r, park(), {"prohibited"});
  {
    const auto p = issues::plan(base, r, park(), issue);
    s.require("as the record stands, only what is alleged is in play: the bicycle is open; the car and the emergency, which no one mentions, are absent",
              p.given == std::vector<std::string>{"in_park"} && p.open == std::vector<std::string>{"bicycle"} && p.absent == std::vector<std::string>{"car", "emergency"} && p.decided.empty());
    s.require("then one way to establish the ban (the bicycle, read broadly) and two to resist it (no bicycle; the narrow reading)",
              lines(p.establish) == std::set<std::string>{"+bicycle [vehicle=broad]"} && lines(p.resist) == std::set<std::string>{"-bicycle", "[vehicle=narrow]"}
                  && p.must_prove == std::vector<std::string>{"bicycle"}, p.verdict.reason);
  }
  {
    const auto p = issues::plan(base, r, park(), issue, issues::plan_limit, true);
    s.require("with absent facts opened too: being in the park is given; the bicycle, the car and the emergency are open",
              p.given == std::vector<std::string>{"in_park"} && p.open == std::vector<std::string>{"bicycle", "car", "emergency"} && p.absent.empty() && p.decided.empty());
    s.require("on the record as it stands the ban is untenable: nothing that is a vehicle is established", p.now == ai::Class::untenable);
    s.require("two ways to establish it: a car with no emergency; a bicycle with no emergency, read broadly",
              lines(p.establish) == std::set<std::string>{"+car -emergency", "+bicycle -emergency [vehicle=broad]"}, p.verdict.reason);
    s.require("three ways to resist it: an emergency; neither a bicycle nor a car; no car, with vehicle read narrowly",
              lines(p.resist) == std::set<std::string>{"+emergency", "-bicycle -car", "-car [vehicle=narrow]"});
    s.require("no fact must be made out in every way, but the emergency must be shown not to hold in every way",
              p.must_prove.empty() && p.must_disprove == std::vector<std::string>{"emergency"} && p.verdict.holds());
  }
  {
    // A binding holding fixes the broad reading: the narrow one is no longer admissible.
    ai::Case c = base;
    const authority::Forum forum{"magistrates", "the magistrates", {"Avalon"}};
    const std::vector<authority::Court> courts = {{"supreme", "the Supreme Court", "Avalon", 2, {"bylaw"}}};
    authority::add_to(c, authority::weigh(c.points, courts, {{"s1", "supreme", "Case S1", "AV:S1", "2015-06-01", "bylaw", "vehicle", "broad", ""}}, forum));
    const auto p = issues::plan(c, r, park(), issue, issues::plan_limit, true);
    s.require("with the broad reading binding, a bicycle suffices with no reading to argue, and resisting by the narrow reading is gone",
              lines(p.establish) == std::set<std::string>{"+car -emergency", "+bicycle -emergency"} && lines(p.resist) == std::set<std::string>{"+emergency", "-bicycle -car"});
  }
  {
    facts::Record d = r;
    d.facts.push_back(stands("emergency", facts::Standing::rebutted));
    const auto p = issues::plan(norms::subsume(d, park(), {"prohibited"}), d, park(), issue, issues::plan_limit, true);
    s.require("the emergency rebutted is decided against, and drops out of every way",
              p.decided == std::vector<std::string>{"emergency"} && lines(p.establish) == std::set<std::string>{"+car", "+bicycle [vehicle=broad]"}
                  && lines(p.resist) == std::set<std::string>{"-bicycle -car", "-car [vehicle=narrow]"} && p.must_disprove.empty());
  }
  {
    const auto p = issues::plan(base, r, park(), issue, 8, true);
    s.require("above its limit the plan is refused with the number of combinations, not sampled",
              !p.verdict.holds() && !p.verdict.fails() && p.verdict.reason.find("16 combinations") != std::string::npos && p.establish.empty());
  }
  return s.done();
}
