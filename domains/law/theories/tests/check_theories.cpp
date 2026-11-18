// Checks of law/theories: theories of the case read for what the law makes of them, weighed over the theories found, tested on what
// was held back, and whether a person could have been at the scene, under the readings of its open terms.
//
// An invented ordinance: whoever sets fire to property of another commits damage. The matter admits that the shed was another's.
// The evidence is tekmerion's shed (its checks): a fire begun at the door between 300 s and 1500 s; ana (a bicycle) placed by a mast,
// an inn's camera and a landline at home; ben (on foot) at his cottage and later paying at the inn; a man in a grey coat seen by a
// jogger; a log of no fault; a till record putting ana at the inn at 860 s, held back. Bindings: an ignition by ana makes
// accused_set_fire.
//
// - Under ana's theory damage is clear; under the grey coat's it is untenable.
// - Over the three hands found equally plausible, damage is arguable: the best alternative, at a relative plausibility of 1/3, is
//   above any threshold of 0.05, and the alternatives are not excluded.
// - With a witness of credibility 0.9 who saw ana light it, every alternative must doubt the witness: each is then 1/9 as plausible
//   as ana's theory, 1/11 of the whole; below a threshold of 0.1 they are excluded, above 0.05 they are not.
// - The test on the held-back till record: ana's theory must doubt it, the grey coat's admits it; each is registered in the journal
//   before it is written, the journal verifies, and no theory is tested twice.
// - Could each have been at the door: ana, clearly; ben, arguably (not at the door before 600 s, though in the grounds), with the
//   point of the time the pivot; a person who stayed 3 km away, untenably (an alibi under every reading).

#include <law/theories/theories.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace law::theories;
namespace ts = tekmerion::scenarios;
namespace tc = tekmerion::construal;
namespace lf = law::facts;

static ts::Problem shed() {
  ts::Problem p;
  p.agents = {{"ana", "Ana", 6, true}, {"ben", "Ben", 1.6, true}, {"grey", "the man in the grey coat", 2, false}, {"cal", "Cal", 1.4, true}};
  p.kinds = {{"ignite", "a flame applied", {"actor"}}, {"fault", "a fault", {}}};
  p.scheme = {{"origin", "how the fire began", {"ignite", "fault"}}};
  p.rooms = {{"origin", 0, 0, 5, 300, 1500}};
  const auto item = [](std::string id, ts::Says says, std::string agent, double x, double y, double r, double from, double until, double c) {
    ts::Item i;
    i.id = std::move(id), i.says = says, i.agent = std::move(agent), i.x = x, i.y = y, i.radius = r, i.from = from, i.until = until, i.credibility = c;
    return i;
  };
  auto report = item("report", ts::Says::event, "", 0, 0, 5, 300, 1500, 0.85);
  report.kind = "ignite";
  auto call = item("call", ts::Says::event, "", 0, 0, 10, 0, 1680, 0.99);
  call.kind = "origin";
  auto log = item("log", ts::Says::absence, "", 0, 0, 10, 0, 1800, 0.7);
  log.kind = "fault";
  auto till = item("till", ts::Says::sighting, "ana", -560, 210, 3, 860, 860, 0.97);
  till.held_back = true;
  p.items = {report, call,
             item("mast", ts::Says::sighting, "ana", -300, 500, 900, 60, 60, 0.95),
             item("inn", ts::Says::sighting, "ana", -560, 210, 3, 780, 780, 0.6),
             item("landline", ts::Says::sighting, "ana", 1650, 940, 15, 1260, 1260, 0.7),
             item("jogger", ts::Says::sighting, "grey", -120, -260, 20, 900, 1500, 0.6),
             log,
             item("cottage", ts::Says::sighting, "ben", -900, -400, 10, 0, 0, 0.5),
             item("pub", ts::Says::sighting, "ben", -560, 210, 5, 1320, 1320, 0.95),
             item("far-early", ts::Says::sighting, "cal", 3000, 0, 5, 0, 0, 0.9),
             item("far-late", ts::Says::sighting, "cal", 3000, 0, 5, 1800, 1800, 0.9),
             till};
  return p;
}

static ts::Scenario account(const ts::Problem& p, const std::string& text) {
  const auto r = ts::read(p, text);
  if (!r.ok) throw std::runtime_error(r.errors.front());
  return r.scenario;
}

int main() {
  calx::verification::Suite s;
  auto p = shed();
  law::norms::Norms n;
  n.rules = {{"damage-rule", "", {"accused_set_fire", "property_of_another"}, "damage"}};
  lf::Record matter;
  lf::Fact other = lf::fact("property_of_another", "the shed belonged to another");
  other.standing = lf::Standing::admitted;
  matter.facts = {other};
  const Bindings b{{{"ignite", "actor", "ana", "accused_set_fire"}}, {}};
  const std::vector<std::string> issues{"damage"}, questions{"damage"}, burdens{"the prosecution"};
  const auto make_case = [&](const lf::Record& r) { return law::norms::subsume(r, n, questions); };

  const auto theory = [&](const std::string& text, const ts::Problem& q) {
    Theory t;
    t.scenario = account(q, text);
    t.assessment = ts::assess(q, t.scenario);
    t.holds = holding(q, t.scenario, b);
    t.outcomes = outcomes(record_under(matter, b.addressed(), t.holds), issues, questions, make_case);
    return t;
  };
  auto ana = theory("event ignite actor=ana at 0 0 when 900\n", p);
  auto grey = theory("event ignite actor=grey at 0 0 when 1000\nbelieve jogger when 1250\n", p);
  auto ben = theory("event ignite actor=ben at 0 0 when 800\n", p);
  s.require("the three theories are coherent", ana.assessment.coherent && grey.assessment.coherent && ben.assessment.coherent, ben.assessment.reason);
  s.require("under ana's theory accused_set_fire holds and damage is clear", ana.holds.count("accused_set_fire") && ana.outcomes[0].cls == ai::Class::clear);
  s.require("under the grey coat's it is untenable: accused_set_fire is not proved", !grey.holds.count("accused_set_fire") && grey.outcomes[0].cls == ai::Class::untenable);

  {
    std::vector<Theory> found{ana, grey, ben};
    const auto w = ts::relative_plausibility({ana.assessment.score, grey.assessment.score, ben.assessment.score});
    for (std::size_t k = 0; k < found.size(); k++) found[k].plausibility = w[k], found[k].central = k == 0 ? "ignite by Ana" : k == 1 ? "ignite by the man in the grey coat" : "ignite by Ben";
    const auto st = standings(found, issues, questions, burdens, 0.05);
    s.require("over three equally plausible hands, damage is arguable", st[0].over == ai::Class::arguable && st[0].follows.size() == 1 && st[0].fails.size() == 2);
    s.check("... the theories where it follows weigh 1/3", st[0].for_it - 1.0 / 3, 1e-12);
    s.require("... and the alternatives are not excluded: one of relative plausibility 1/3 was found", st[0].verdict.fails(), st[0].verdict.reason);
  }
  {
    auto q = p;
    ts::Item saw;
    saw.id = "witness", saw.says = ts::Says::event, saw.kind = "ignite", saw.agent = "ana", saw.role = "actor", saw.x = 0, saw.y = 0, saw.radius = 5, saw.from = 300, saw.until = 1500, saw.credibility = 0.9;
    q.items.insert(q.items.end() - 1, saw);
    auto a2 = theory("event ignite actor=ana at 0 0 when 900\n", q);
    auto g2 = theory("event ignite actor=grey at 0 0 when 1000\nbelieve jogger when 1250\ndoubt witness\n", q);
    auto b2 = theory("event ignite actor=ben at 0 0 when 800\ndoubt witness\n", q);
    s.require("with a witness who saw ana light it, the alternatives that doubt the witness stay coherent", a2.assessment.coherent && g2.assessment.coherent && b2.assessment.coherent);
    std::vector<Theory> found{a2, g2, b2};
    const auto w = ts::relative_plausibility({a2.assessment.score, g2.assessment.score, b2.assessment.score});
    for (std::size_t k = 0; k < found.size(); k++) found[k].plausibility = w[k], found[k].central = k == 0 ? "ignite by Ana" : "an alternative";
    s.check("... each alternative is then 1/9 as plausible as ana's theory", w[1] / w[0] - 1.0 / 9, 1e-9);
    s.check("... 1/11 of the whole", w[1] - 1.0 / 11, 1e-9);
    const auto lax = standings(found, issues, questions, burdens, 0.05), strict = standings(found, issues, questions, burdens, 0.1);
    s.require("... above a threshold of 0.05 the alternatives are not excluded", lax[0].verdict.fails(), lax[0].verdict.reason);
    s.require("... below a threshold of 0.1 they are", strict[0].verdict.holds(), strict[0].verdict.reason);
  }

  {
    Confirmer c;
    const auto ca = c.confirm(p, 1, ana.scenario, "2026-11-18T10:00:00Z", "the search");
    const auto cg = c.confirm(p, 2, grey.scenario, "2026-11-18T10:00:01Z", "the search");
    s.require("the test on the held-back till record: ana's theory must doubt it", ca && ca->outcome == "must doubt" && ca->doubted == std::vector<std::string>{"till"}, ca ? ca->reason : "");
    s.require("... the grey coat's admits it", cg && cg->outcome == "admitted", cg ? cg->reason : "");
    s.require("... each registered before it was written, four entries in order", c.journal.entries.size() == 4 && c.journal.entries[0].what == "registered a test" && c.journal.entries[1].what == "wrote a test's outcome" && c.journal.entries[0].note.find("not yet read") != std::string::npos);
    s.require("... the journal verifies", archeion::fixity::verify(c.journal).holds());
    s.require("... and no theory is tested twice", !c.confirm(p, 1, ana.scenario, "2026-11-18T10:00:02Z", "the search") && c.journal.entries.size() == 4);
  }

  {
    const std::vector<PlaceReading> places{{"at the door", tc::Source::analyst, "the expert's point of origin", 5, true}, {"in the grounds", tc::Source::analyst, "the boundary of the grounds", 60}};
    const std::vector<TimeReading> times{{"the expert's window", tc::Source::analyst, "the fire report", 300, 1500, true}, {"before the bell", tc::Source::analyst, "the caretaker's account", 300, 600}};
    const auto pa = could_have_been_there(p, "ana", 0, 0, places, times);
    s.require("ana could have been at the scene: clear under every reading", pa.construal.record == ai::Class::clear, pa.construal.verdict.reason);
    const auto pb = could_have_been_there(p, "ben", 0, 0, places, times);
    s.require("ben: arguable, three of four readings", pb.construal.record == ai::Class::arguable && std::fabs(pb.construal.share - 0.75) < 1e-12, pb.construal.verdict.reason);
    bool time_pivots = false;
    for (const auto& pv : pb.construal.pivots) time_pivots = time_pivots || (pb.construal.points[pv.point].name == "at the time" && !pv.flips.empty());
    s.require("... the point of the time is a pivot", time_pivots);
    s.require("... not at the door before 600 s, though in the grounds", !pb.construal.holds[ai::encode(pb.construal.points, {0, 1})] && pb.construal.holds[ai::encode(pb.construal.points, {1, 1})]);
    const auto pc = could_have_been_there(p, "cal", 0, 0, places, times);
    s.require("a person who stayed 3 km away: untenable under every reading, an alibi", pc.construal.record == ai::Class::untenable, pc.construal.verdict.reason);
    bool certified = true;
    for (const auto& e : pc.each) certified = certified && e.identified.certified() && e.identified.value() == "out";
    s.require("... each reading's answer certified out", certified && pc.each.size() == 4);
  }
  return s.done();
}
