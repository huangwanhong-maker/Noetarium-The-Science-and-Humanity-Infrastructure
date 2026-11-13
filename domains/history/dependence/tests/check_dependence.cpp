// Checks of the dependence of historical objects (dependence.hpp), on a record small enough to follow by hand.
//
// The record: an old tuning made on a string meets a problem (it does not close); two answers appear far
// apart, each with its own condition, and one of them is a kind of tempering; a treatise draws on the first.
// A second problem has no answer yet.
//
// - The web at each layer: the premises that are in it, and the objects with none.
// - Cones: the past by steps with roles on the first step; a bound on the depth; what is inherited from what an
//   object specialises, marked so; the future.
// - Lineage: nearest first; what exemplifies a class stands under what the class specialises.
// - The frontier: a problem open before its first answer, answered after; a problem with no answer open.
// - Support: everything rests on the string; nothing rests on one of two alternative emergences alone.
// - Comparisons: two answers to one problem with no path between them are independent; a treatise that draws
//   on one of them is not.
// - The inquiry: each of the four questions answered, and the silences said.
// - Gathering: dependences between what stands under two classes, counted, apart from what is asserted of them.
// - On the domain's records: the web is acyclic at every layer, cones hold nothing twice, and every problem
//   that has an answer is on the frontier once it is answered.

#include <history/dependence/dependence.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <filesystem>
#include <set>
#include <string>

using namespace history;
using namespace history::dependence;
using objects::Layer;
using objects::Record;

namespace {

const char* SMALL = R"({"row": "record", "id": "small", "format": "noetarium.histos/1", "title": "A small record"}
{"row": "source", "id": "model", "kind": "model", "cite": "A model"}
{"row": "source", "id": "a", "kind": "reference", "cite": "A"}
{"row": "source", "id": "b", "kind": "reference", "cite": "B"}
{"row": "object", "id": "string", "kind": "Technology", "name": "A string", "date": "-0600~"}
{"row": "object", "id": "old", "kind": "Theory", "name": "The old tuning", "date": "-0500~"}
{"row": "object", "id": "closing", "kind": "Problem", "name": "It does not close", "date": "-0400~"}
{"row": "object", "id": "keys", "kind": "Problem", "name": "Every key", "date": "1500~"}
{"row": "object", "id": "tempering", "kind": "Theory", "name": "Tempering"}
{"row": "object", "id": "roots", "kind": "Technology", "name": "Roots", "date": "1300~"}
{"row": "object", "id": "east", "kind": "Theory", "name": "The eastern answer", "date": "1584"}
{"row": "object", "id": "west", "kind": "Theory", "name": "The western answer", "date": "1585"}
{"row": "object", "id": "treatise", "kind": "Work", "name": "A treatise", "date": "1636"}
{"row": "object", "id": "ann", "kind": "Person", "name": "Ann", "date": "1540/1610"}
{"row": "object", "id": "school", "kind": "Theory", "name": "A school of tempering"}
{"row": "object", "id": "piece", "kind": "Work", "name": "A piece", "date": "1700"}
{"row": "relation", "id": "r1", "type": "enabled_by", "from": "old", "to": "string", "said": [{"by": "a", "grade": "A", "confirmed": "x"}, {"by": "b", "grade": "A", "confirmed": "y"}]}
{"row": "relation", "id": "r2", "type": "enabled_by", "from": "closing", "to": "old", "said": [{"by": "a", "grade": "A", "confirmed": "x"}]}
{"row": "relation", "id": "r3", "type": "answers", "from": "east", "to": "closing", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r4", "type": "enabled_by", "from": "east", "to": "roots", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r5", "type": "answers", "from": "west", "to": "closing", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r6", "type": "draws_on", "from": "treatise", "to": "west", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r7", "type": "specialises", "from": "west", "to": "tempering", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r8", "type": "answers", "from": "tempering", "to": "keys", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r9", "type": "made_by", "from": "east", "to": "ann", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r10", "type": "exemplifies", "from": "piece", "to": "school", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r11", "type": "specialises", "from": "school", "to": "tempering", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "r12", "type": "draws_on", "from": "piece", "to": "treatise", "said": [{"by": "model", "grade": "C"}]}
)";

std::set<std::string> ids(const Record& R, const std::vector<ConeEntry>& cone) {
  std::set<std::string> out;
  for (const auto& e : cone) out.insert(R.objects[e.object].id);
  return out;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Record R = objects::read_text(SMALL);
  s.require("the record reads with no fault", R.faults.empty(), R.faults.empty() ? "" : R.faults.front().row + ": " + R.faults.front().what);
  const auto at = [&](const char* id) { return static_cast<std::size_t>(R.find(id)); };
  const Web all = web(R, Layer::proposed), attested = web(R, Layer::attested), established = web(R, Layer::established);

  s.require("the web by layer: eight premises proposed, two attested, one established", all.links.size() == 8 && attested.links.size() == 2 && established.links.size() == 1);
  s.require("an object with no premise in the layer is given", std::count(all.givens.begin(), all.givens.end(), at("string")) == 1 && std::count(all.givens.begin(), all.givens.end(), at("old")) == 0 &&
                                                                 std::count(established.givens.begin(), established.givens.end(), at("closing")) == 1);
  {
    const auto past = cone(R, all, at("east"), true, 0, false);
    s.require("the past of the eastern answer: its problem, its condition, the old tuning, the string", ids(R, past) == std::set<std::string>({"closing", "roots", "old", "string"}));
    bool roles = true;
    for (const auto& e : past) {
      const std::string id = R.objects[e.object].id;
      roles = roles && (id == "closing" ? e.role == "problem" && e.steps == 1 : id == "roots" ? e.role == "condition" && e.steps == 1 : id == "old" ? e.role.empty() && e.steps == 2 : e.steps == 3);
    }
    s.require("with a role one step away and the number of steps beyond", roles);
    s.require("a bound on the depth", ids(R, cone(R, all, at("east"), true, 1, false)) == std::set<std::string>({"closing", "roots"}));
    const auto west = cone(R, all, at("west"), true, 1, true);
    bool inherited = false;
    for (const auto& e : west) inherited = inherited || (R.objects[e.object].id == "keys" && e.inherited == static_cast<long>(at("tempering")) && e.role == "problem");
    s.require("the western answer inherits tempering's problem, marked as inherited", inherited && ids(R, west) == std::set<std::string>({"closing", "keys"}));
    s.require("the future of the old tuning", ids(R, cone(R, all, at("old"), false)) == std::set<std::string>({"closing", "east", "west", "treatise", "piece"}));
    s.require("at the attested layer the past of the eastern answer is empty", cone(R, attested, at("east"), true).empty());
  }
  {
    const auto line = lineage(R, at("piece"), Layer::proposed);
    s.require("what exemplifies a class stands under what the class specialises, nearest first",
              line.size() == 2 && R.objects[line[0].object].id == "school" && line[0].by == "exemplifies" && R.objects[line[1].object].id == "tempering" && line[1].steps == 2);
    const auto below = under(R, at("tempering"), Layer::proposed);
    s.require("what stands under tempering", std::set<std::size_t>(below.begin(), below.end()) == std::set<std::size_t>({at("west"), at("school"), at("piece")}));
    s.require("at the established layer nothing is above anything", lineage(R, at("piece"), Layer::established).empty());
  }
  {
    const auto f1500 = frontier(R, all, jdn_julian(1550, 1, 1)), f1600 = frontier(R, all, jdn_gregorian(1600, 1, 1)), early = frontier(R, all, jdn_julian(-450, 1, 1));
    const auto find = [&](const std::vector<Question>& f, const char* id) { return std::find_if(f.begin(), f.end(), [&](const Question& q) { return q.problem == at(id); }); };
    s.require("before it is posed a problem is not on the frontier", early.empty());
    s.require("in 1550 the closing is open, with two answers still to come", find(f1500, "closing") != f1500.end() && find(f1500, "closing")->open() && find(f1500, "closing")->later.size() == 2);
    s.require("in 1600 it has its two answers", find(f1600, "closing")->answered.size() == 2 && !find(f1600, "closing")->open());
    const Record undated = objects::read_text(std::string(SMALL) + R"({"row": "object", "id": "late", "kind": "Problem", "name": "A problem with no date"}
{"row": "relation", "id": "x1", "type": "enabled_by", "from": "late", "to": "roots", "said": [{"by": "model", "grade": "C"}]}
{"row": "relation", "id": "x2", "type": "enabled_by", "from": "late", "to": "east", "said": [{"by": "model", "grade": "C"}]}
)");
    const Web wu = web(undated, Layer::proposed);
    const auto has = [&](long day) { const auto f = frontier(undated, wu, day); return std::any_of(f.begin(), f.end(), [&](const Question& q) { return undated.objects[q.problem].id == "late"; }); };
    s.require("a problem with no date is posed once all its premises have appeared, and not before", !has(jdn_julian(1500, 1, 1)) && has(jdn_gregorian(1585, 1, 1)));
    s.require("a problem whose only answer has no date stays open", find(f1600, "keys") != f1600.end() && find(f1600, "keys")->open());
  }
  {
    const auto rests = support(all, at("string"));
    s.require("everything that follows rests on the string", std::set<std::size_t>(rests.begin(), rests.end()) == std::set<std::size_t>({at("old"), at("closing"), at("east"), at("west"), at("treatise"), at("piece")}));
    s.require("the treatise and the piece rest on the western answer, the eastern does not", support(all, at("west")) == std::vector<std::size_t>({at("treatise"), at("piece")}));
    const Record two = objects::read_text(std::string(SMALL) + R"({"row": "object", "id": "pipe", "kind": "Technology", "name": "A pipe", "date": "-0600~"}
{"row": "emergence", "id": "again", "product": "closing", "premises": [{"what": "pipe", "role": "condition"}], "said": [{"by": "model", "grade": "C"}]}
)");
    const Web w2 = web(two, Layer::proposed);
    const auto on_old = support(w2, static_cast<std::size_t>(two.find("old")));
    s.require("with a second way for the problem to arise, it no longer rests on the old tuning", on_old.empty(), std::to_string(on_old.size()));
  }
  {
    const auto c = comparisons(R, all, at("east"));
    s.require("the western answer is a comparison case of the eastern, and independent of it", c.size() == 1 && c[0].object == at("west") && c[0].independent && c[0].problems == std::vector<std::size_t>({at("closing")}));
    const Record tied = objects::read_text(std::string(SMALL) + R"({"row": "relation", "id": "x", "type": "draws_on", "from": "west", "to": "east", "said": [{"by": "model", "grade": "C"}]})" + "\n");
    const auto d = comparisons(tied, web(tied, Layer::proposed), static_cast<std::size_t>(tied.find("east")));
    s.require("once one draws on the other they are compared and not independent", d.size() == 1 && !d[0].independent);
    s.require("what answers no problem has no comparison", comparisons(R, all, at("treatise")).empty());
  }
  {
    const Inquiry q = inquiry(R, all, at("east"));
    s.require("what it answers", q.problems.size() == 1 && q.problems[0].object == at("closing"));
    s.require("why the problem arose: what the problem rests on", q.arose.size() == 1 && q.arose[0].object == at("old") && q.arose[0].of == static_cast<long>(at("closing")));
    s.require("what made it possible, for whom, and nothing after", q.answerable.size() == 1 && q.answerable[0].object == at("roots") && q.for_whom.size() == 1 && q.for_whom[0].object == at("ann") && q.after.empty());
    s.require("the silence is said", q.silent == std::vector<std::string>({"what it made possible after"}));
    const Inquiry w = inquiry(R, all, at("west"));
    bool own = false, inherited = false;
    for (const auto& p : w.problems) { own = own || (p.object == at("closing") && p.inherited < 0); inherited = inherited || (p.object == at("keys") && p.inherited == static_cast<long>(at("tempering"))); }
    s.require("the western answer's problems: its own, and tempering's as inherited", own && inherited && w.after.size() == 1);
    s.require("at the established layer the record is silent on all but one question of the old tuning", inquiry(R, established, at("old")).silent.size() == 3 && inquiry(R, established, at("east")).silent.size() == 4);
  }
  {
    const Gathering g = gather(R, all, {at("tempering"), at("closing")});
    // Under tempering: west, school, piece. piece → nothing chosen… the links: west→treatise (treatise unplaced), treatise→piece (unplaced),
    // closing→west (closing is a class itself, west under tempering): one derived arc closing → tempering.
    bool one = g.derived.size() == 1 && g.derived[0].from == 1 && g.derived[0].to == 0 && g.derived[0].arcs == 1;
    s.require("one dependence gathered from the problem to what stands under tempering", one);
    s.require("what has an end under no class is counted apart", g.unplaced == 7 && g.asserted.empty());
    const Gathering direct = gather(R, all, {at("tempering"), at("keys")});
    s.require("a premise directly between two classes is asserted, not derived", direct.asserted.size() == 1 && direct.derived.empty());
  }
#ifdef HISTORY_RECORDS
  for (const auto& entry : std::filesystem::directory_iterator(HISTORY_RECORDS)) {
    if (entry.path().extension() != ".jsonl") continue;
    const Record T = objects::read_file(entry.path().string());
    const std::string name = entry.path().stem().string();
    bool acyclic = true, once = true, answered = true;
    std::size_t links = 0, open_now = 0;
    for (Layer layer : {Layer::established, Layer::attested, Layer::proposed}) {
      const Web W = web(T, layer);
      links = W.links.size();
      acyclic = acyclic && morphe::combinatorics::topological_order(W.n, W.arcs).acyclic;
      for (std::size_t x = 0; x < W.n; x++) {
        const auto c = cone(T, W, x, true);
        std::set<std::size_t> seen;
        for (const auto& e : c) once = once && seen.insert(e.object).second && e.object != x;
      }
      const auto f = frontier(T, W, jdn_gregorian(2000, 1, 1));
      open_now = 0;
      for (const auto& q : f) {
        open_now += q.open();
        answered = answered && q.later.empty();
      }
    }
    s.require(name + ": the dependence is acyclic at every layer (" + std::to_string(links) + " premises with the proposed)", acyclic);
    s.require(name + ": a cone holds nothing twice, and not its own object", once);
    s.require(name + ": by 2000 no answer is still to come (" + std::to_string(open_now) + " problems open)", answered);
  }
#endif

  return s.done();
}
