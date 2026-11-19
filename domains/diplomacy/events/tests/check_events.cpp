// Checks of events as the reports they rest on (events.hpp).
//
// - A record's faults, each named: a category not in the vocabulary, no actor, an actor not of the undertaking, a date that is none,
//   a report with no source.
// - One wire carried by fifty outlets: one origin, one event in the series where the reports would count fifty.
// - Force: with the wire's own channel declared, the reports' force rises with the number of outlets toward the wire's and never
//   passes it, while counted as independent it grows without bound (tekmerion's theorem, its decision 0007); a report that relays
//   no one is its source's channel alone; two independent origins corroborate as the product of their forces.
// - Coincident records: the same category, actors and targets two days apart are reported as possibly one event and kept as two;
//   ten days apart, or of another category, they are not.
// - The series of a pair by month and by class, its net cooperation, and a vocabulary of the caller's in place of PLOVER's.
// - The signs: unattested, one origin, disputed, denied, self-reported, a report with no passage.
//
// Every party, report and event here is invented.

#include <diplomacy/events/events.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace diplomacy::events;

namespace {

Event event(const std::string& id, const std::string& category, const std::string& actor, const std::string& target, const std::string& date) {
  Event e;
  e.id = id, e.category = category, e.date = date;
  e.actors = {actor}, e.targets = {target};
  return e;
}

Report report(const std::string& id, const std::string& source, const std::string& origin, bool says = true) {
  return {id, source, origin, "2026-02-02", "the words of " + id, says};
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    Event bad = event("e1", "WAVE", "north", "south", "2026-13-01");
    bad.actors.clear();
    bad.reports = {report("r1", "", "")};
    Event stranger = event("e2", "CONSULT", "east", "south", "2026-02-01");
    const auto ps = problems({bad, stranger}, plover(), {"north", "south"});
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a record's faults are named: a category not in PLOVER, no actor, a date that is none, a report with no source, an actor not of the undertaking",
              has("no category") && has("no actor") && has("not a calendar date") && has("names no source") && has("east is not a party"), std::to_string(ps.size()) + " faults");
  }

  // ---- One wire, fifty outlets ----
  {
    Event e = event("summit", "CONSULT", "north", "south", "2026-02-01");
    for (int k = 1; k <= 50; k++) e.reports.push_back(report("r" + std::to_string(k), "outlet " + std::to_string(k), "the agency's wire"));
    const Support sp = support(e);
    s.require("fifty outlets carrying one wire rest on one origin", sp.reports == 50 && sp.sources == 50 && sp.origins == 1);
    const auto ser = series({e}, "north", "south");
    s.require("and count once in the series, where the reports would count fifty", ser.size() == 1 && ser[0].events.at("verbal cooperation") == 1 && ser[0].reports.at("verbal cooperation") == 50);
  }

  // ---- Force ----
  {
    Channels ch;
    ch.origins["the agency's wire"] = {0.8, 0.05};
    ch.relay = {0.9, 0.1};
    std::vector<double> held, counted;
    for (int n : {1, 5, 50}) {
      Event e = event("summit", "CONSULT", "north", "south", "2026-02-01");
      for (int k = 1; k <= n; k++) e.reports.push_back(report("r" + std::to_string(k), "outlet " + std::to_string(k), "the agency's wire"));
      const Support sp = support(e, &ch);
      held.push_back(sp.ratio), counted.push_back(sp.as_independent);
    }
    const double wire = 0.8 / 0.05;
    s.require("the reports' force rises with the outlets toward the wire's and never passes it", held[0] < held[1] && held[1] < held[2] && held[2] <= wire + 1e-9 && wire - held[2] < 0.01,
              std::to_string(held[0]) + ", " + std::to_string(held[1]) + ", " + std::to_string(held[2]) + " against " + std::to_string(wire));
    char b[64];
    std::snprintf(b, sizeof b, "%.3g counted for fifty outlets", counted[2]);
    s.require("counted as independent it grows without bound", counted[2] > 1e30 && counted[2] / held[2] > 1e28, b);
    Event alone = event("statement", "DISAPPROVE", "north", "south", "2026-02-01");
    alone.reports = {report("r1", "the observer", "")};
    ch.sources["the observer"] = {0.7, 0.2};
    s.require("a report that relays no one is its source's channel alone", std::fabs(support(alone, &ch).ratio - 0.7 / 0.2) < 1e-12);
    Event two = event("visit", "CONSULT", "north", "south", "2026-02-01");
    two.reports = {report("r1", "outlet 1", "the agency's wire"), report("r2", "the observer", "")};
    const double product = (0.8 * 0.9 + 0.2 * 0.1) / (0.05 * 0.9 + 0.95 * 0.1) * (0.7 / 0.2);
    s.require("two independent origins corroborate as the product of their forces", std::fabs(support(two, &ch).ratio - product) < 1e-9 * product,
              std::to_string(support(two, &ch).ratio) + " against " + std::to_string(product));
  }

  // ---- Coincident records ----
  {
    const std::vector<Event> es{event("a", "SANCTION", "north", "south", "2026-03-01"), event("b", "SANCTION", "north", "south", "2026-03-03"),
                                event("c", "SANCTION", "north", "south", "2026-03-11"), event("d", "THREATEN", "north", "south", "2026-03-02")};
    const auto co = coincident(es);
    s.require("two records of one category, actors and target two days apart are possibly one event, and kept as two",
              co.size() == 1 && co[0].a == "a" && co[0].b == "b" && co[0].what.find("kept as two") != std::string::npos && es.size() == 4,
              co.empty() ? "" : co[0].what);
  }

  // ---- The series of a pair ----
  {
    std::vector<Event> es{event("1", "CONSULT", "north", "south", "2026-01-05"), event("2", "AGREE", "south", "north", "2026-01-20"),
                          event("3", "SANCTION", "north", "south", "2026-02-02"), event("4", "DISAPPROVE", "south", "north", "2026-02-10"),
                          event("5", "AID", "north", "east", "2026-02-11")};
    for (auto& e : es) e.reports = {report("r", "outlet", "")};
    const auto m = series(es, "north", "south");
    s.require("by month: two cooperative events in January, two of conflict in February, the event towards a third party left out",
              m.size() == 2 && m[0].net == 2 && m[1].net == -2 && m[1].events.at("material conflict") == 1 && m[1].events.at("verbal conflict") == 1);
    s.require("by year the net cooperation sums", series(es, "north", "south", true).size() == 1 && series(es, "north", "south", true)[0].net == 0);
    Vocabulary mine{"two words", "the caller", {{"MEET", "", true, false}, {"QUARREL", "", false, false}}};
    std::vector<Event> own{event("m", "MEET", "north", "south", "2026-01-01"), event("q", "QUARREL", "south", "north", "2026-01-02")};
    s.require("a vocabulary of the caller's replaces PLOVER's", problems(own, mine).empty() && !problems(own).empty() && series(own, "north", "south", false, mine)[0].net == 0);
  }

  // ---- Signs ----
  {
    Event unattested = event("u", "CONSULT", "north", "south", "2026-01-01");
    Event wire = event("w", "CONSULT", "north", "south", "2026-01-02");
    wire.reports = {report("r1", "outlet 1", "the wire"), report("r2", "outlet 2", "the wire")};
    Event disputed = event("d", "ASSAULT", "north", "south", "2026-01-03");
    disputed.reports = {report("r1", "outlet 1", ""), report("r2", "outlet 2", "", false)};
    Event denied = event("n", "ASSAULT", "north", "south", "2026-01-04");
    denied.reports = {report("r1", "outlet 1", "", false)};
    Event own = event("o", "AID", "north", "south", "2026-01-05");
    own.reports = {report("r1", "north", "")};
    own.reports[0].passage.clear();
    const auto sg = signs({unattested, wire, disputed, denied, own});
    auto kind = [&](const std::string& k, const std::string& where) {
      for (const auto& x : sg)
        if (x.kind == k && x.where == "event " + where) return true;
      return false;
    };
    s.require("the signs are found where they show: unattested, one origin, disputed, denied, self-reported, no passage",
              kind("unattested", "u") && kind("one origin", "w") && kind("disputed", "d") && kind("denied", "n") && kind("self-reported", "o") && kind("no passage", "o") && sg.size() == 6,
              std::to_string(sg.size()) + " signs");
  }
  return s.done();
}
