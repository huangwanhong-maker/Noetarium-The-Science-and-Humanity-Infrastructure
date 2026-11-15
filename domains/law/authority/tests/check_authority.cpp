// Checks of interpretive authority: courts of an invented order read "vehicle" in a park bylaw; which holdings bind the magistrates
// and which persuade, a court departing from itself, a holding after the question's date, a court reading outside its competence,
// the parties' canons against binding and persuasive holdings, and the faults of holdings that read what the case does not declare.

#include <law/authority/authority.hpp>
#include <law/norms/norms.hpp>

#include <calx/verification/check.hpp>

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
  n.terms = {{"vehicle", {"broad", "narrow"}}};
  n.counts = {{"bicycle_counts", "bicycle", "vehicle", "broad", "vehicle"}};
  return n;
}

ai::Case bicycle_case() {
  facts::Record r;
  r.facts = {stands("bicycle", facts::Standing::found), stands("in_park", facts::Standing::admitted)};
  return norms::subsume(r, park(), {"prohibited"});
}

const std::vector<authority::Court> courts = {
    {"high", "the High Court of Avalon", "Avalon", 1, {"bylaw"}},
    {"supreme", "the Supreme Court of Avalon", "Avalon", 2, {"bylaw"}},
    {"erewhon", "the Court of Erewhon", "Erewhon", 2, {"bylaw"}},
    {"union", "the Court of the Union", "Union", 3, {"charter"}},
};
const authority::Forum magistrates{"magistrates", "the magistrates of Avalon", {"Avalon", "Union"}};

authority::Holding held(const std::string& id, const std::string& court, const std::string& date, const std::string& reading, const std::string& instrument = "bylaw") {
  return {id, court, "Case " + id, "AV:" + id, date, instrument, "vehicle", reading, "a vehicle is read " + reading};
}

ai::Class answer(ai::Case c, const authority::Authority& a) {
  authority::add_to(c, a);
  return ai::evaluate(c).answers.at(0).classification;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto points = bicycle_case().points;

  {
    const auto a = authority::weigh(points, courts, {held("h1", "high", "2010-03-01", "narrow"), held("s1", "supreme", "2015-06-01", "broad")}, magistrates);
    s.require("both holdings of Avalon's courts bind its magistrates, each with its reason",
              a.assessed.size() == 2 && a.assessed[0].force == authority::Force::binding && a.assessed[1].force == authority::Force::binding && a.faults.empty()
                  && a.assessed[0].reason.find("the High Court of Avalon binds the magistrates of Avalon (Avalon)") != std::string::npos);
    s.require("the High Court's kind is ranked below the Supreme Court's", a.canon_order.size() == 1 && a.canon_order[0].first == a.assessed[0].kind && a.canon_order[0].second == a.assessed[1].kind);
    s.require("so the higher court's broad reading is fixed, and the bicycle is clearly prohibited", answer(bicycle_case(), a) == ai::Class::clear);
  }
  {
    const auto a = authority::weigh(points, courts, {held("h1", "high", "2010-03-01", "narrow"), held("s1", "supreme", "2015-06-01", "broad"), held("s2", "supreme", "2018-02-01", "narrow")}, magistrates);
    s.require("the Supreme Court departs from itself in 2018: the later holding prevails, and the ban on the bicycle is untenable", answer(bicycle_case(), a) == ai::Class::untenable);
    authority::Options o;
    o.later_prevails = false;
    const auto b = authority::weigh(points, courts, {held("s1", "supreme", "2015-06-01", "broad"), held("s2", "supreme", "2018-02-01", "narrow")}, magistrates, {}, o);
    s.require("unless a later holding is not taken to prevail: then the court's two readings stand against each other and the question is arguable", answer(bicycle_case(), b) == ai::Class::arguable);
  }
  {
    authority::Options o;
    o.date = "2016-01-01";
    const auto a = authority::weigh(points, courts, {held("s1", "supreme", "2015-06-01", "broad"), held("s2", "supreme", "2018-02-01", "narrow")}, magistrates, {}, o);
    s.require("asked as of 2016, the holding of 2018 is left out with the reason, and the broad reading of 2015 is fixed",
              a.assessed[1].force == authority::Force::left_out && a.assessed[1].reason.find("after the date of the question") != std::string::npos && answer(bicycle_case(), a) == ai::Class::clear);
  }
  {
    const auto a = authority::weigh(points, courts, {held("e1", "erewhon", "2019-01-01", "narrow"), held("s1", "supreme", "2015-06-01", "broad")}, magistrates);
    s.require("a court of another order persuades and does not bind, and is ranked below the binding holding",
              a.assessed[0].force == authority::Force::persuasive && a.assessed[0].reason.find("the Court of Erewhon (Erewhon) does not bind the magistrates of Avalon") != std::string::npos && answer(bicycle_case(), a) == ai::Class::clear);
    const auto b = authority::weigh(points, courts, {held("u1", "union", "2020-01-01", "narrow")}, magistrates);
    s.require("a court of an order that binds the forum, reading an instrument outside its competence, persuades only",
              b.assessed[0].force == authority::Force::persuasive && b.assessed[0].reason.find("does not interpret bylaw with authority") != std::string::npos);
  }
  {
    // The council argues from the bylaw's purpose that only motor vehicles are meant; a foreign court read it broadly.
    ai::Case c = bicycle_case();
    c.arguments.push_back({"purpose", 0, 1, "purpose", "the bylaw keeps the park quiet; a bicycle is quiet", 0});
    const std::vector<authority::Holding> foreign = {held("e1", "erewhon", "2019-01-01", "broad")};
    const auto unranked = authority::weigh(points, courts, foreign, magistrates, {"purpose"});
    s.require("a canon and a persuasive holding unranked stand against each other: arguable", answer(c, unranked) == ai::Class::arguable);
    authority::Options above, below;
    above.canons = authority::Canons::above, below.canons = authority::Canons::below;
    s.require("with the canons above persuasive holdings, the purpose fixes the narrow reading: untenable", answer(c, authority::weigh(points, courts, foreign, magistrates, {"purpose"}, above)) == ai::Class::untenable);
    s.require("with them below, the foreign court's broad reading is fixed: clear", answer(c, authority::weigh(points, courts, foreign, magistrates, {"purpose"}, below)) == ai::Class::clear);
    const auto binding = authority::weigh(points, courts, {held("s1", "supreme", "2015-06-01", "broad")}, magistrates, {"purpose"}, above);
    s.require("against a binding holding the canon does not succeed, whatever its place against persuasive ones", answer(c, binding) == ai::Class::clear);
  }
  {
    auto bad_court = held("x1", "nowhere", "2015-01-01", "broad");
    auto bad_reading = held("x2", "supreme", "2015-01-01", "motorised");
    auto other_term = held("x3", "supreme", "2015-01-01", "whole");
    other_term.term = "park";
    const auto a = authority::weigh(points, courts, {bad_court, bad_reading, other_term}, magistrates);
    s.require("a holding naming a court not declared, or a reading the term does not admit, is a fault; one reading a term the case has not is only left out",
              a.faults.size() == 2 && a.arguments.empty() && a.assessed[2].force == authority::Force::left_out && a.assessed[2].reason.find("no open term of the case") != std::string::npos);
  }
  return s.done();
}
