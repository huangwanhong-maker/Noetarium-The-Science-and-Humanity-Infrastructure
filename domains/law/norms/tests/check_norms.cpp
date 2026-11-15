// Checks of the law domain's norms: a bylaw applied to a record of facts as a case for apodeixis; an open term and what counts
// under each reading; an exception; facts by their standing; two rules whose sources are ranked.

#include <law/norms/norms.hpp>

#include <calx/verification/check.hpp>

#include <string>

using namespace law;
namespace ai = apodeixis::interpretation;

namespace {

facts::Fact stands(const std::string& id, facts::Standing st, const std::string& basis = {}) {
  facts::Fact f = facts::fact(id, id);
  f.kind = "Social";
  f.standing = st;
  f.basis = basis;
  f.emerged_from = "the events in the park", f.since = "2026-05-03", f.constrains = {"prohibited"};
  return f;
}

norms::Norms park() {
  norms::Norms n;
  n.sources = {{"bylaw", "the Park Bylaw", sources::Kind::regulation, "2010-01-01", "", 2, {}, ""}, {"act", "the Cycling Act", sources::Kind::statute, "2005-01-01", "", 1, {}, ""}};
  n.rules = {{"ban", "bylaw", {"vehicle", "in_park"}, "prohibited"}};
  n.exceptions = {{"emergency_exception", "ban", {"emergency"}}};
  n.terms = {{"vehicle", {"broad", "narrow"}}};
  n.counts = {{"bicycle_counts", "bicycle", "vehicle", "broad", "vehicle"}, {"car_counts_broad", "car", "vehicle", "broad", "vehicle"}, {"car_counts_narrow", "car", "vehicle", "narrow", "vehicle"}};
  return n;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  {
    facts::Record r;
    r.facts = {stands("bicycle", facts::Standing::found, "the judgment"), stands("in_park", facts::Standing::admitted)};
    const auto c = norms::subsume(r, park(), {"prohibited"});
    const auto ev = ai::evaluate(c);
    const auto& a = ev.answers.at(0);
    s.require("a bicycle in the park: whether it is prohibited turns on the reading of \"vehicle\": arguable, its label [vehicle=broad]",
              a.classification == ai::Class::arguable && a.label.size() == 1 && ai::print(c.points, a.label[0]) == "[vehicle=broad]", a.verdict.reason);
    s.require("a fact found and a fact admitted enter the case as axioms", c.theory.axioms.size() == 2 && c.theory.premises.empty());
  }
  {
    facts::Record r;
    r.facts = {stands("car", facts::Standing::proved), stands("in_park", facts::Standing::admitted)};
    const auto c = norms::subsume(r, park(), {"prohibited"});
    const auto ev = ai::evaluate(c);
    s.require("a car in the park is prohibited under either reading: clear, whatever the readings", ev.answers[0].classification == ai::Class::clear && ai::print(c.points, ev.answers[0].label[0]) == "(whatever the readings)");
    s.require("a fact that is proved enters as an ordinary premise", c.theory.premises == std::vector<std::string>{"car"});
  }
  {
    facts::Record r;
    r.facts = {stands("car", facts::Standing::proved), stands("in_park", facts::Standing::admitted), stands("emergency", facts::Standing::proved)};
    const auto ev = ai::evaluate(norms::subsume(r, park(), {"prohibited"}));
    s.require("with an emergency proved, the exception takes the rule away: the prohibition is untenable", ev.answers[0].classification == ai::Class::untenable, ev.answers[0].verdict.reason);
  }
  {
    facts::Record r;
    r.facts = {stands("car", facts::Standing::alleged), stands("in_park", facts::Standing::admitted)};
    const auto c = norms::subsume(r, park(), {"prohibited"});
    s.require("a fact only alleged is absent from the case, and nothing follows from it", c.theory.premises.empty() && ai::evaluate(c).answers[0].classification == ai::Class::untenable);
    facts::Record q;
    q.facts = {stands("car", facts::Standing::not_proved), stands("in_park", facts::Standing::rebutted, "x")};
    s.require("nor do a fact not proved and a presumption rebutted enter it", norms::subsume(q, park(), {"prohibited"}).theory.axioms.empty());
  }
  {
    // A statute lets bicycles into every public place; the bylaw bans vehicles from the park. The statute is the higher source.
    auto n = park();
    n.rules.push_back({"freedom", "act", {"bicycle"}, "¬prohibited"});
    facts::Record r;
    r.facts = {stands("bicycle", facts::Standing::found, "the judgment"), stands("in_park", facts::Standing::admitted)};
    const auto c = norms::subsume(r, n, {"prohibited", "¬prohibited"});
    const auto ev = ai::evaluate(c);
    s.require("two rules of ranked sources: the statute's is preferred to the bylaw's", c.theory.rule_order == std::vector<std::pair<std::string, std::string>>{{"ban", "freedom"}});
    s.require("so the bicycle is not prohibited under either reading, and the prohibition is untenable", ev.answers[1].classification == ai::Class::clear && ev.answers[0].classification == ai::Class::untenable);
    // With the maxims ordered so that the later source prevails, the bylaw's ban wins where it applies.
    n.maxims = {sources::Maxim::posterior, sources::Maxim::superior, sources::Maxim::specialis};
    const auto ev2 = ai::evaluate(norms::subsume(r, n, {"prohibited"}));
    s.require("ordered so that the later source prevails, the ban wins under the broad reading: the answer follows the declared order of maxims", ev2.answers[0].classification == ai::Class::arguable);
  }

  return s.done();
}
