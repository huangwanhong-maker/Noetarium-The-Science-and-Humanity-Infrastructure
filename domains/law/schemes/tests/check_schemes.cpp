// Checks of schemes: an invented park bylaw stated clause by clause, whole; then each kind of fault planted one at a time and found
// where it is: a provision cited that is not there, a quotation not in its clause, a fact asked for and never declared, a
// consequence declared as a fact, a reading a term does not admit, a holding whose passage is not in its judgment, a provision whose
// parent is missing, an id used twice, a rule cited to nothing.

#include <law/schemes/schemes.hpp>

#include <calx/verification/check.hpp>

#include <string>

using namespace law;

namespace {

schemes::Scheme bylaw() {
  schemes::Scheme s;
  s.id = "park", s.title = "The Park Bylaw, as an example", s.as_of = "2026-01-01";
  schemes::Instrument i;
  i.id = "bylaw", i.title = "Park Bylaw 2010", i.citation = "PB 2010", i.kind = "bylaw", i.as_of = "2010-01-01";
  i.provisions = {
      {"bylaw/s1", "Section 1", "", ""},
      {"bylaw/s1/1", "Section 1(1)", "No vehicle may be taken into the park.", "bylaw/s1"},
      {"bylaw/s1/2", "Section 1(2)", "Subsection (1) does not apply\n   in an emergency.", "bylaw/s1"},
  };
  s.instruments = {i};
  s.norms.rules = {{"ban", "", {"vehicle", "in_park"}, "prohibited"}};
  s.norms.exceptions = {{"emergency_exception", "ban", {"emergency"}}};
  s.norms.terms = {{"vehicle", {"broad", "narrow"}}};
  s.norms.counts = {{"bicycle_counts", "bicycle", "vehicle", "broad", "vehicle"}, {"car_counts", "car", "vehicle", "narrow", "vehicle"}};
  s.vocabulary = {{"bicycle", "a bicycle was ridden in", schemes::Use::fact}, {"car", "a car was driven in", schemes::Use::fact}, {"in_park", "within the park", schemes::Use::fact},
                  {"emergency", "there was an emergency", schemes::Use::fact}, {"vehicle", "a vehicle in the bylaw's sense", schemes::Use::consequence},
                  {"prohibited", "taking it in is prohibited", schemes::Use::consequence}};
  s.citations = {{"ban", {"bylaw/s1/1"}, "No vehicle may be taken"},
                 {"emergency_exception", {"bylaw/s1/2"}, "does not apply in an emergency"},  // the clause's line break is only whitespace
                 {"bicycle_counts", {"bylaw/s1/1"}, ""},
                 {"car_counts", {"bylaw/s1/1"}, ""}};
  s.courts = {{"supreme", "the Supreme Court", "Avalon", 2, {"bylaw"}}};
  s.holdings = {{"s1", "supreme", "Rider v Council", "AV:S1", "2015-06-01", "bylaw", "vehicle", "broad", "a bicycle is a vehicle within section 1"}};
  return s;
}

const std::map<std::string, std::string> judgments = {{"s1", "We hold that a bicycle is a vehicle within section 1, whatever its quiet."}};

bool found(const std::vector<schemes::Fault>& faults, const std::string& where, const std::string& what) {
  for (const auto& f : faults)
    if (f.where == where && f.what.find(what) != std::string::npos) return true;
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  {
    const auto faults = schemes::check(bylaw(), judgments);
    std::string said;
    for (const auto& f : faults) said += f.where + ": " + f.what + "; ";
    s.require("the bylaw stated clause by clause is whole: every rule cited, every quotation in its clause, every fact declared", faults.empty(), said);
    const auto sc = bylaw();
    s.require("a rule's provisions and their instrument are looked up", schemes::cited(sc, "ban") == std::vector<std::string>{"bylaw/s1/1"} && schemes::instrument_of(sc, "bylaw/s1/2")->id == "bylaw"
                                                                             && schemes::provision(sc, "bylaw/s1/1")->label == "Section 1(1)");
  }
  {
    auto sc = bylaw();
    sc.citations[0].provisions = {"bylaw/s9"};
    s.require("a provision cited that is not there", found(schemes::check(sc), "citation/ban", "bylaw/s9, which is no provision"));
  }
  {
    auto sc = bylaw();
    sc.citations[0].quote = "No vehicle shall enter";
    s.require("a quotation not found in the words of its clause", found(schemes::check(sc), "citation/ban", "quotation is not found"));
  }
  {
    auto sc = bylaw();
    sc.norms.rules[0].elements.push_back("tricycle");
    s.require("a fact asked for and never declared", found(schemes::check(sc), "rule/ban", "tricycle, which is neither declared nor given by a rule"));
  }
  {
    auto sc = bylaw();
    sc.vocabulary[5].use = schemes::Use::fact;
    s.require("a consequence declared as a fact", found(schemes::check(sc), "rule/ban", "declared as a fact, not a consequence"));
  }
  {
    auto sc = bylaw();
    sc.norms.counts[1].reading = "motorised";
    s.require("a counts-as link naming a reading its term does not admit", found(schemes::check(sc), "counts/car_counts", "the reading motorised"));
  }
  {
    auto sc = bylaw();
    sc.holdings[0].passage = "a bicycle is never a vehicle";
    s.require("a holding whose passage is not in the text of its judgment", found(schemes::check(sc, judgments), "holding/s1", "not found in the text of the judgment"));
  }
  {
    auto sc = bylaw();
    sc.instruments[0].provisions[1].parent = "bylaw/s0";
    s.require("a provision whose parent is missing", found(schemes::check(sc), "bylaw/s1/1", "its parent bylaw/s0"));
  }
  {
    auto sc = bylaw();
    sc.instruments[0].provisions[2].id = "bylaw/s1/1";
    s.require("a provision's id used twice", found(schemes::check(sc), "bylaw/bylaw/s1/1", "used twice"));
  }
  {
    auto sc = bylaw();
    sc.citations.pop_back();
    s.require("a counts-as link cited to no provision", found(schemes::check(sc), "car_counts", "cited to no provision"));
  }
  return s.done();
}
