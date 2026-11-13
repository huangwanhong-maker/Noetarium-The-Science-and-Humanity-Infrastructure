// Checks of historical objects and their record (objects/record.hpp).
//
// - The kinds: the core's twenty beneath one root; a school a tradition and a collective; a kind added beneath
//   one that is there, and refused beneath one that is not or twice.
// - Typing: a relation fits by a signature of its type and the order of kinds; the reason when it does not.
// - Standing: the rule worked case by case (a model's word alone; one reference unconfirmed; one confirmed;
//   two confirmed; a documentary one; a dispute), and the three layers.
// - A small record read whole: its rows, a dependence written short gathered into its object's emergence, an
//   emergence written whole with what marks it, dates in the historians' convention, when an object appears.
// - Every fault found where it is, the rest of the record kept: an unknown kind, an ill-typed relation, a
//   grade that does not suit its source, a quotation missing, a circle of specialisation, a circle of
//   dependence, an anachronism, a place with no position, a premise in the role of a problem that is none.
// - The domain's records (records/*.jsonl) read with no fault.

#include <history/objects/record.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <filesystem>
#include <string>

using namespace history::objects;

namespace {

const char* SMALL = R"({"row": "record", "id": "small", "format": "noetarium.histos/1", "title": "A small record"}
{"row": "kind", "id": "Tuning", "name": "tuning", "under": ["Theory"], "plugin": "music"}
{"row": "source", "id": "model", "kind": "model", "cite": "A model, from memory"}
{"row": "source", "id": "book", "kind": "reference", "cite": "A book, 1999"}
{"row": "source", "id": "other", "kind": "reference", "cite": "Another book, 2005"}
{"row": "source", "id": "title", "kind": "primary", "cite": "The title page itself", "object": "work"}
{"row": "object", "id": "town", "kind": "Place", "name": "A town", "lat": 50.0, "lon": 8.0}
{"row": "object", "id": "ann", "kind": "Person", "name": "Ann", "date": "1500/1570", "said": [{"by": "book", "grade": "A", "confirmed": "a reader, 2026"}, {"by": "other", "grade": "A", "confirmed": "a reader, 2026"}]}
{"row": "object", "id": "old", "kind": "Tuning", "name": "The old tuning", "date": "-0500~"}
{"row": "object", "id": "trouble", "kind": "Problem", "name": "The old tuning does not close"}
{"row": "object", "id": "tool", "kind": "Technology", "name": "A tool", "date": "1400~"}
{"row": "object", "id": "new", "kind": "Tuning", "name": "The new tuning", "date": "1560"}
{"row": "object", "id": "work", "kind": "Work", "name": "A treatise", "date": "1561"}
{"row": "object", "id": "printed", "kind": "Event", "name": "The treatise is printed", "date": "1561-03"}
{"row": "object", "id": "tempering", "kind": "Tuning", "name": "Tempering"}
{"row": "relation", "id": "r1", "type": "made_by", "from": "work", "to": "ann", "said": [{"by": "title", "grade": "B", "quote": "by Ann", "documentary": true, "confirmed": "a reader, 2026"}]}
{"row": "relation", "id": "r2", "type": "sets_out", "from": "work", "to": "new", "said": [{"by": "book", "grade": "B", "quote": "sets out the new tuning"}]}
{"row": "relation", "id": "r3", "type": "specialises", "from": "new", "to": "tempering", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r4", "type": "at", "from": "work", "to": "town"}
{"row": "relation", "id": "r5", "type": "enabled_by", "from": "trouble", "to": "old", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r6", "type": "draws_on", "from": "work", "to": "new", "said": [{"by": "book", "grade": "A", "confirmed": "x"}, {"by": "other", "grade": "A", "disputes": true}]}
{"row": "emergence", "id": "e1", "product": "new", "date": "1560", "marked_by": ["printed"], "premises": [{"what": "trouble", "role": "problem"}, {"what": "old", "role": "against"}, {"what": "tool", "role": "condition", "said": [{"by": "book", "grade": "A", "confirmed": "x"}, {"by": "other", "grade": "A", "confirmed": "y"}]}], "said": [{"by": "model", "grade": "C"}]}
)";

bool faulted(const Record& r, const std::string& row, const std::string& part) {
  return std::any_of(r.faults.begin(), r.faults.end(), [&](const Fault& f) { return f.row.find(row) != std::string::npos && f.what.find(part) != std::string::npos; });
}

Record with(const std::string& more) { return read_text(std::string(SMALL) + more); }

std::string first_fault(const Record& r) { return r.faults.empty() ? "" : r.faults.front().row + ": " + r.faults.front().what; }

}  // namespace

int main() {
  calx::verification::Suite s;

  {
    const Kinds k = Kinds::core();
    bool all = k.all().size() == 20;
    for (const auto& kind : k.all()) all = all && k.is_a(kind.id, "Object");
    s.require("the core has twenty kinds, each a kind of the historical object", all);
    s.require("a school is a tradition and a collective, and so an agent", k.is_a("School", "Tradition") && k.is_a("School", "Collective") && k.is_a("School", "Agent"));
    s.require("a person is an agent and no work; a kind is a kind of itself", k.is_a("Person", "Agent") && !k.is_a("Person", "Work") && k.is_a("Work", "Work") && !k.is_a("Agent", "Person"));
    const auto up = k.above("Institution");
    s.require("the kinds above one, nearest first", up.size() == 3 && up[0] == "Collective" && up[1] == "Agent" && up[2] == "Object");
    Kinds more = k;
    more.add({"Tuning", "tuning", "music", {"Theory"}});
    s.require("a discipline's kind lies beneath the core's", more.is_a("Tuning", "Idea") && more.all().size() == 21);
    bool refused = false, twice = false, orphan = false;
    try { more.add({"Mode", "mode", "music", {"Scale"}}); } catch (const std::invalid_argument&) { refused = true; }
    try { more.add({"Tuning", "tuning", "music", {"Theory"}}); } catch (const std::invalid_argument&) { twice = true; }
    try { more.add({"Loose", "loose", "", {}}); } catch (const std::invalid_argument&) { orphan = true; }
    s.require("a kind beneath one that is not there, a kind twice and a kind beneath nothing are refused", refused && twice && orphan);
  }
  {
    const Kinds k = Kinds::core();
    const auto types = core_types();
    const auto type = [&](const char* id) { return *std::find_if(types.begin(), types.end(), [&](const RelationType& t) { return t.id == id; }); };
    s.require("a work is made by a person, since a person is an agent", !misfit(k, type("made_by"), "Work", "Person"));
    s.require("a person is made by no one: the reason names the kinds", misfit(k, type("made_by"), "Person", "Person").value_or("").find("not from Person to Person") != std::string::npos);
    s.require("anything answers a problem and only a problem", !misfit(k, type("answers"), "Theory", "Problem") && misfit(k, type("answers"), "Theory", "Concept"));
    s.require("a school specialises a tradition; a work specialises no person", !misfit(k, type("specialises"), "School", "Tradition") && misfit(k, type("specialises"), "Work", "Person"));
    s.require("an event exemplifies no person", static_cast<bool>(misfit(k, type("exemplifies"), "Event", "Person")));
  }
  {
    const Assertion model{"model", 'C', false, false, "", "", ""};
    const Assertion open{"book", 'A', false, false, "", "", ""};
    const Assertion one{"book", 'A', false, false, "", "", "a reader"};
    const Assertion two{"other", 'B', false, false, "the passage", "p. 3", "a reader"};
    const Assertion own{"title", 'B', false, true, "by Ann", "", "a reader"};
    const Assertion denies{"other", 'A', true, false, "", "", ""};
    s.require("a model's word alone is proposed", standing_of({model}) == Standing::proposed && standing_of({}) == Standing::proposed);
    s.require("a reference not yet confirmed, or one confirmed, is attested", standing_of({open, model}) == Standing::attested && standing_of({one}) == Standing::attested);
    s.require("the same source twice is one source", standing_of({one, one}) == Standing::attested);
    s.require("two confirmed sources establish", standing_of({one, two}) == Standing::established);
    s.require("a confirmed documentary assertion establishes alone", standing_of({own}) == Standing::established);
    s.require("a source's denial makes it disputed, whatever else is said", standing_of({one, two, denies}) == Standing::disputed);
    s.require("a policy of one source", standing_of({one}, Policy{1}) == Standing::established);
    s.require("the layers: established alone; with the attested; with the proposed; the disputed in none",
              in_layer(Standing::established, Layer::established) && !in_layer(Standing::attested, Layer::established) && in_layer(Standing::attested, Layer::attested) &&
                  !in_layer(Standing::proposed, Layer::attested) && in_layer(Standing::proposed, Layer::proposed) && !in_layer(Standing::disputed, Layer::proposed));
  }
  {
    const Record r = read_text(SMALL);
    s.require("a small record reads with no fault", r.faults.empty(), first_fault(r));
    s.require("its sources, objects and relations", r.sources.size() == 4 && r.objects.size() == 9 && r.relations.size() == 6 && r.kinds.all().size() == 21);
    s.require("an emergence written whole, and one gathered for each object with a dependence written short",
              r.emergences.size() == 3 && r.emergences[0].written && r.emergences[0].premises.size() == 3 && !r.emergences[1].written);
    const auto& gathered = r.emergences[1];
    s.require("a dependence written short is a premise in its type's role", r.objects[gathered.product].id == "trouble" && gathered.premises.size() == 1 && gathered.premises[0].role == "condition" && gathered.premises[0].relation == 4);
    s.require("what marks an emergence is an event", r.emergences[0].marked_by.size() == 1 && r.objects[r.emergences[0].marked_by[0]].id == "printed");
    const auto ann = r.objects[static_cast<std::size_t>(r.find("ann"))];
    s.require("a life is an interval, and an object appears at its beginning", ann.when && ann.when->first == history::jdn_julian(1500, 1, 1) && ann.when->begin_last == history::jdn_julian(1500, 12, 31) && ann.when->last == history::jdn_julian(1570, 12, 31));
    s.require("a year BC is astronomical and Julian", r.objects[static_cast<std::size_t>(r.find("old"))].when->first == history::jdn_julian(-500, 1, 1));
    s.require("standing by the record's rule: two sources, a documentary one, an unconfirmed one, a model's, a dispute",
              r.standing(ann.said) == Standing::established && r.standing(r.relations[0].said) == Standing::established && r.standing(r.relations[1].said) == Standing::attested &&
                  r.standing(r.relations[2].said) == Standing::proposed && r.standing(r.relations[5].said) == Standing::disputed);
    s.require("a source that is also an object is tied to it", r.sources[3].object == "work");
  }
  {
    s.require("an unknown kind", faulted(with(R"({"row": "object", "id": "x", "kind": "Mood", "name": "X"})"), "object x", "is no kind"));
    s.require("an object twice", faulted(with(R"({"row": "object", "id": "ann", "kind": "Person", "name": "Ann again"})"), "object ann", "there already"));
    s.require("an ill-typed relation, with the kinds it holds between", faulted(with(R"({"row": "relation", "id": "x", "type": "made_by", "from": "ann", "to": "work"})"), "relation x", "not from Person to Work"));
    s.require("a relation to what the record does not hold", faulted(with(R"({"row": "relation", "id": "x", "type": "at", "from": "ann", "to": "nowhere"})"), "relation x", "the second is no object"));
    s.require("a model's word given grade A", faulted(with(R"({"row": "relation", "id": "x", "type": "at", "from": "ann", "to": "town", "said": [{"by": "model", "grade": "A"}]})"), "relation x", "a model's word is of grade C"));
    s.require("grade C from a book", faulted(with(R"({"row": "relation", "id": "x", "type": "at", "from": "ann", "to": "town", "said": [{"by": "book", "grade": "C"}]})"), "relation x", "is no model"));
    s.require("grade B without the passage", faulted(with(R"({"row": "relation", "id": "x", "type": "at", "from": "ann", "to": "town", "said": [{"by": "book", "grade": "B"}]})"), "relation x", "quotes the passage"));
    s.require("an assertion by no source of the record", faulted(with(R"({"row": "relation", "id": "x", "type": "at", "from": "ann", "to": "town", "said": [{"by": "nobody", "grade": "A"}]})"), "relation x", "no source"));
    const Record circle = with(R"({"row": "relation", "id": "x", "type": "specialises", "from": "tempering", "to": "new"})");
    s.require("a circle of specialisation, named", faulted(circle, "specialises", "in a circle") && faulted(circle, "specialises", "new"));
    s.require("a circle of dependence, named", faulted(with(R"({"row": "relation", "id": "x", "type": "draws_on", "from": "old", "to": "trouble"})"), "dependence", "in a circle"));
    s.require("an anachronism: a premise that begins after what it is a premise of", faulted(with(R"({"row": "relation", "id": "x", "type": "draws_on", "from": "old", "to": "work"})"), "emergence of:old", "an anachronism"));
    s.require("a place with no position", faulted(with(R"({"row": "object", "id": "x", "kind": "Place", "name": "Somewhere"})"), "object x", "a latitude and a longitude"));
    s.require("in the role of the problem, what is no problem", faulted(with(R"({"row": "emergence", "id": "x", "product": "work", "premises": [{"what": "old", "role": "problem"}]})"), "emergence x", "is no problem"));
    s.require("a date that is not read", faulted(with(R"({"row": "object", "id": "x", "kind": "Event", "name": "X", "date": "the spring"})"), "object x", "its date is not read"));
    s.require("a line that is not JSON", faulted(with("{not json\n"), "", "not JSON"));
    const Record kept = with(R"({"row": "object", "id": "x", "kind": "Mood", "name": "X"})");
    s.require("the record stands past a fault", kept.faults.size() == 1 && kept.objects.size() == 9);
  }
#ifdef HISTORY_RECORDS
  {
    std::size_t files = 0;
    for (const auto& entry : std::filesystem::directory_iterator(HISTORY_RECORDS)) {
      if (entry.path().extension() != ".jsonl") continue;
      files++;
      const Record r = read_file(entry.path().string());
      const std::string f = r.faults.empty() ? "" : "line " + std::to_string(r.faults.front().line) + ", " + first_fault(r);
      s.require("the record " + entry.path().stem().string() + " reads with no fault: " + std::to_string(r.objects.size()) + " objects, " + std::to_string(r.relations.size()) + " relations", r.faults.empty(), f);
    }
    s.require("the domain holds a record of objects", files >= 1);
  }
#endif

  return s.done();
}
