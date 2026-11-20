// Checks of allocation/procedure: the round's sealed record and the checkable rules of procedural justice.
//
// - A round's digest is that of what it declares: the same round, the same digest; one attribute changed, another.
// - A clean record (published, every group consulted, every datum sourced, a body of oversight, then applied): consistency,
//   publicity, reasons, accuracy, representativeness and enforcement hold; the record seals; relevance and ethicality are left to
//   a person; correctability and the lottery are undecided, since neither was tried.
// - Each fault is found: applied before published; a result that does not follow from the round; a datum without a source; a
//   stage declared by someone with an interest in an entity it gave to; a group not consulted; an appeal not heard; an upheld
//   appeal whose revision was not applied; an entry altered after it was sealed.
// - An appeal on a datum recomputes the round for everyone: a population corrected changes what every entity receives.
// - A lottery drawn from a committed seed repeats from it; one not committed first, or altered after, fails; equal chances give
//   each of four candidates a quarter of the first places over 4000 seeds, weights 1, 1, 2 give a quarter, a quarter and a half.

#include <allocation/procedure/procedure.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace allocation;
using allocation::procedure::Rule;

static rounds::Entity entity(const std::string& id, std::map<std::string, double> at, const std::string& group) {
  rounds::Entity e;
  e.id = e.name = id;
  e.group = group;
  e.at = std::move(at);
  return e;
}

static rounds::Round example() {
  rounds::Round r;
  r.name = "small grants";
  r.resource = "grants";
  r.unit = "£";
  r.amount = 1200;
  r.entities = {entity("north", {{"level", 60}, {"population", 10}}, "towns"), entity("south", {{"level", 90}, {"population", 20}}, "towns"),
                entity("east", {{"level", 120}, {"population", 30}}, "cities")};
  rounds::Stage f;
  f.kind = "floor";
  f.principle = "sufficiency";
  f.source = {"the panel", "2026-11-01", "no place below 100 a head"};
  f.attribute = "level";
  f.bound = 100;
  rounds::Stage p;
  p.kind = "proportional";
  p.principle = "equality per head";
  p.source = {"the council", "2026-11-01", "the rest alike for every resident"};
  p.attribute = "population";
  r.stages = {f, p};
  r.sources = {{"level", "the spending returns of 2025"}, {"population", "the census of 2021"}};
  return r;
}

// By value: the rules are often those of a check just made, which do not outlive the expression.
static Rule rule(const std::vector<Rule>& rules, const std::string& name) {
  for (const auto& r : rules)
    if (r.name == name) return r;
  return {"", "", apodeixis::kernel::Verdict::unknown("", "not checked")};
}

static procedure::Record clean(const rounds::Round& r) {
  procedure::Record rec;
  procedure::declare(rec, r, "2026-11-02T09:00:00Z", "the council");
  procedure::consult(rec, "towns", "2026-11-03T09:00:00Z", "the council", "a meeting with the towns");
  procedure::consult(rec, "cities", "2026-11-03T10:00:00Z", "the council", "a meeting with the cities");
  procedure::oversee(rec, "the ombudsman", "2026-11-03T11:00:00Z", "may hear complaints of procedure");
  procedure::allocate(rec, r, rounds::run(r), "2026-11-10T09:00:00Z", "the council");
  return rec;
}

int main() {
  calx::verification::Suite s;
  const auto r = example();

  {
    auto q = r;
    s.require("the same round has the same digest", procedure::digest(r) == procedure::digest(q) && procedure::digest(r).size() == 64);
    q.entities[0].at["level"] = 61;
    s.require("one attribute changed, another digest", procedure::digest(r) != procedure::digest(q));
  }

  {
    const auto rec = clean(r);
    const auto rules = procedure::check(rec, r);
    bool held = true;
    for (const char* n : {"record sealed", "consistency", "publicity", "reasons given", "accuracy", "representativeness", "enforcement"}) held = held && rule(rules, n).verdict.holds();
    s.require("a clean record: consistency, publicity, reasons, accuracy, representativeness and enforcement hold, the record seals", held);
    s.require("relevance and ethicality are left to a person", rule(rules, "relevance").verdict.undecided() && rule(rules, "ethicality").verdict.undecided());
    s.require("correctability and the lottery are undecided, neither tried", rule(rules, "correctability").verdict.undecided() && rule(rules, "lottery").verdict.undecided());
    s.require("bias suppression is undecided when no interest was declared", rule(rules, "bias suppression").verdict.undecided());
  }

  {
    procedure::Record rec;
    procedure::allocate(rec, r, rounds::run(r), "2026-11-01T09:00:00Z", "the council");
    procedure::declare(rec, r, "2026-11-02T09:00:00Z", "the council");
    s.require("applied before it was published: publicity fails", rule(procedure::check(rec, r), "publicity").verdict.fails());
  }
  {
    procedure::Record rec;
    procedure::declare(rec, r, "2026-11-02T09:00:00Z", "the council");
    auto altered = rounds::run(r);
    altered.amount[0] += 1, altered.amount[2] -= 1;
    procedure::allocate(rec, r, altered, "2026-11-10T09:00:00Z", "the council");
    const auto c = rule(procedure::check(rec, r), "consistency").verdict;
    s.require("a result that does not follow from the round: consistency fails, with both digests", c.fails() && !c.piece("recorded").empty() && !c.piece("recomputed").empty());
  }
  {
    auto q = r;
    q.sources.erase("population");
    const auto rec = clean(q);
    const auto a = rule(procedure::check(rec, q), "accuracy").verdict;
    s.require("a datum with no source: accuracy fails, naming it", a.fails() && a.reason.find("population") != std::string::npos);
  }
  {
    auto rec = clean(r);
    procedure::interest(rec, "the panel", "north", "2026-11-11T09:00:00Z", "a member lives there");
    const auto b = rule(procedure::check(rec, r), "bias suppression").verdict;
    s.require("the panel declared the floor and an interest in north, which the floor gave to: bias suppression fails", b.fails() && b.reason.find("north") != std::string::npos);
    auto rec2 = clean(r);
    procedure::interest(rec2, "the panel", "east", "2026-11-11T09:00:00Z", "a member lives there");
    s.require("an interest in east, to which the floor gave nothing, does not", rule(procedure::check(rec2, r), "bias suppression").verdict.holds());
  }
  {
    procedure::Record rec;
    procedure::declare(rec, r, "2026-11-02T09:00:00Z", "the council");
    procedure::consult(rec, "towns", "2026-11-03T09:00:00Z", "the council", "a meeting");
    procedure::allocate(rec, r, rounds::run(r), "2026-11-10T09:00:00Z", "the council");
    const auto v = rule(procedure::check(rec, r), "representativeness").verdict;
    s.require("the cities not consulted: representativeness fails, naming them", v.fails() && v.reason.find("cities") != std::string::npos);
  }

  {
    auto rec = clean(r);
    procedure::Appeal a;
    a.when = "2026-11-11T09:00:00Z";
    a.who = "north's clerk";
    a.entity = "north";
    a.kind = "datum";
    a.key = "population";
    a.value = 12;
    a.reason = "the census missed a new estate";
    const std::size_t k = procedure::appeal(rec, a);
    s.require("an appeal not yet heard: correctability fails", rule(procedure::check(rec, r), "correctability").verdict.fails());
    const auto h = procedure::hear(r, a);
    s.require("hearing it recomputes the round for everyone: all three amounts change", h.changed.size() == 3);
    s.require("north's floor rises from 400 to 480", std::abs(h.after.by_stage[0][0] - 480) < 1e-9 && std::abs(h.before.by_stage[0][0] - 400) < 1e-9);
    procedure::review(rec, {k, "2026-11-12T09:00:00Z", "the panel", true, "the estate is occupied"}, h.revised);
    s.require("upheld, the revision declared, the old round still applied: correctability fails", rule(procedure::check(rec, r), "correctability").verdict.fails());
    procedure::allocate(rec, h.revised, h.after, "2026-11-13T09:00:00Z", "the council");
    const auto rules = procedure::check(rec, h.revised);
    s.require("the revised round applied: correctability holds, and so do consistency and publicity",
              rule(rules, "correctability").verdict.holds() && rule(rules, "consistency").verdict.holds() && rule(rules, "publicity").verdict.holds());
    s.require("and the record, with its appeal, review and revision, still seals", rule(rules, "record sealed").verdict.holds());
    auto tampered = rec;
    tampered.journal.entries[1].note = "a meeting with no one";
    s.require("an entry altered after it was sealed: the record does not seal", rule(procedure::check(tampered, h.revised), "record sealed").verdict.fails());
  }
  {
    procedure::Appeal a;
    a.kind = "parameter";
    a.stage = 0;
    a.key = "bound";
    a.value = 110;
    const auto h = procedure::hear(r, a);
    s.require("an appeal on a parameter: a floor of 110 gives north 500 and south 400", std::abs(h.after.by_stage[0][0] - 500) < 1e-9 && std::abs(h.after.by_stage[0][1] - 400) < 1e-9);
    procedure::Appeal b;
    b.kind = "stage";
    b.stage = 0;
    const auto g = procedure::hear(r, b);
    s.require("an appeal against a stage's applicability: without the floor, everything by population", std::abs(g.after.amount[0] - 200) < 1e-9 && std::abs(g.after.amount[2] - 600) < 1e-9);
  }

  {
    auto rec = clean(r);
    const std::vector<std::string> among{"north", "south", "east"};
    procedure::commit(rec, "a seed drawn in public", "2026-11-14T09:00:00Z", "the clerk");
    const auto d = procedure::draw(rec, "a seed drawn in public", among, 2, "2026-11-15T09:00:00Z", "the clerk");
    auto again = procedure::lottery_order("a seed drawn in public", among);
    again.resize(2);
    s.require("a draw repeats from its seed", d.drawn == again);
    s.require("a draw after its commitment: the lottery holds", rule(procedure::check(rec, r), "lottery").verdict.holds());
    auto uncommitted = clean(r);
    procedure::draw(uncommitted, "another seed", among, 1, "2026-11-15T09:00:00Z", "the clerk");
    s.require("a draw whose seed was never committed: the lottery fails", rule(procedure::check(uncommitted, r), "lottery").verdict.fails());
    auto altered = rec;
    std::reverse(altered.draws[0].drawn.begin(), altered.draws[0].drawn.end());
    s.require("a draw altered after it was made does not repeat: the lottery fails", rule(procedure::check(altered, r), "lottery").verdict.fails());
  }
  {
    const std::vector<std::string> four{"a", "b", "c", "d"}, three{"a", "b", "c"};
    std::vector<double> first(4, 0), weighted(3, 0);
    const int N = 4000;
    for (int k = 0; k < N; k++) {
      const auto o = procedure::lottery_order("seed " + std::to_string(k), four);
      first[static_cast<std::size_t>(o[0][0] - 'a')] += 1.0 / N;
      const auto w = procedure::lottery_order("seed " + std::to_string(k), three, {1, 1, 2});
      weighted[static_cast<std::size_t>(w[0][0] - 'a')] += 1.0 / N;
    }
    bool equal = true;
    for (double f : first) equal = equal && std::abs(f - 0.25) < 0.03;
    s.require("equal chances: each of four is first a quarter of the time over 4000 seeds", equal);
    s.require("weights 1, 1, 2: first a quarter, a quarter and half of the time", std::abs(weighted[0] - 0.25) < 0.03 && std::abs(weighted[1] - 0.25) < 0.03 && std::abs(weighted[2] - 0.5) < 0.03);
  }

  return s.done();
}
