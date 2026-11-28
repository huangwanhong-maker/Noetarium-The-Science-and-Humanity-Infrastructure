// Checks of federation, on the presets themselves (and, since thesmos's decision 0013, a compact of three with its terms read from the
// parties' own chronicles: entry by two ratifications, a reservation excluding an article both ways, one party's withdrawal kept apart):
//   - a league run on the first league's constitution: member communities send envoys under their mandates; an accession that
//     five envoys of one member favour fails, counted by community, until a second member agrees, and the new member is admitted;
//   - a compact ratified by two commons, each by its own rule of constitutional amendment and at its own tempo, in force only when
//     both have ratified;
//   - a crossing act between the two commons, with its receipt; two notaries witnessing a commons' heads and both finding a history
//     rewritten; recognition directed and not passed on.

#include <communities/federation/federation.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::acts::Act;
using thesmos::chronicle::State;
using thesmos::kernel::Kind;
using thesmos::kernel::Parties;
using thesmos::mandates::Mandate;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.turn = turn;
  a.fields = std::move(fields);
  a.mandate = std::move(mandate);
  return a;
}
bool ok(State& s, const Act& a, std::string& why) {
  const auto o = thesmos::chronicle::step(s, a);
  if (!o.admitted && why.empty()) why = a.author + " " + a.kind + ": " + o.reason;
  return o.admitted;
}
State commons(const std::string& p, const thesmos::constitutions::Constitution& c) {
  Parties ps;
  for (const char* x : {"steward", "reviewer"}) ps.add({p + x, Kind::person});
  State s(ps, c);
  Act found = act(p + "steward", "found", 0);
  found.holders = {{p + "steward", "steward"}, {p + "reviewer", "reviewer"}};
  thesmos::chronicle::step(s, found);
  return s;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string data = std::string(COMMUNITIES_DATA) + "/presets/data/";

  // ---- A league of communities ----
  {
    Parties ps;
    ps.add({"convener", Kind::person});
    for (const char* c : {"commons", "coop", "guild", "polity"}) ps.add({c, Kind::community});
    for (int i = 1; i <= 5; i++) ps.add({"commons/agent-" + std::to_string(i), Kind::operative, "commons"});
    ps.add({"coop/ana", Kind::person});
    ps.add({"guild/envoy", Kind::operative, "guild"});
    ps.add({"registry", Kind::organisation});
    State league(ps, thesmos::constitutions::read(slurp(data + "first-league.json")));
    std::string why;
    Act found = act("convener", "found", 0);
    found.holders = {{"commons", "member"}, {"coop", "member"}, {"guild", "member"}, {"registry", "notary"}};
    bool all = ok(league, found, why);
    const std::set<std::string> envoy_kinds = {"join", "propose", "vote", "report"};
    std::vector<std::pair<std::string, std::string>> envoys;  // (envoy, community)
    for (int i = 1; i <= 5; i++) envoys.push_back({"commons/agent-" + std::to_string(i), "commons"});
    envoys.push_back({"coop/ana", "coop"});
    envoys.push_back({"guild/envoy", "guild"});
    for (const auto& [envoy, community] : envoys) {
      Act m = act(community, "mandate", 1);
      m.grant = Mandate{"m-" + envoy, community, envoy, envoy_kinds, {}, {}, 1, 30};
      all = all && ok(league, m, why);
      all = all && ok(league, act(envoy, "join", 1, {{"position", "envoy"}, {"sponsor", community}}, "m-" + envoy), why);
    }
    s.require("seven envoys of three member communities join the council under their communities' mandates, one a person and six operatives", all, why);
    all = ok(league, act("commons/agent-1", "propose", 2, {{"id", "accession of the polity"}, {"decision", "accession"}, {"change.admit", "polity"}, {"change.position", "member"}}, "m-commons/agent-1"), why);
    for (const auto& [envoy, community] : envoys) all = all && ok(league, act(envoy, "vote", 2, {{"id", "accession of the polity"}, {"value", community == "commons" ? "yes" : "no"}}, "m-" + envoy), why);
    s.require("the accession is proposed and voted: five envoys of the commons in favour, the cooperative's and the guild's against", all, why);
    const auto& p = league.proposals.at("accession of the polity");
    const auto by_operative = thesmos::procedures::count(league.parties, p.ballots(), thesmos::procedures::Unit::operative);
    const auto by_community = thesmos::procedures::count(league.parties, p.ballots(), thesmos::procedures::Unit::community);
    const auto early = thesmos::chronicle::step(league, act("convener", "admit", 3, {{"position", "member"}, {"whom", "polity"}, {"proposal", "accession of the polity"}}));
    s.require("counted by operative the accession would pass (5/7 against two thirds); counted by community it fails (1/3), and the admission is refused",
              by_operative.share() == thesmos::procedures::Rational(5, 7) && by_community.share() == thesmos::procedures::Rational(1, 3) && !early.admitted &&
                  early.reason.find("has not passed") != std::string::npos,
              early.reason);
    why.clear();
    all = ok(league, act("coop/ana", "vote", 3, {{"id", "accession of the polity"}, {"value", "yes"}}, "m-coop/ana"), why);
    all = all && ok(league, act("convener", "admit", 3, {{"position", "member"}, {"whom", "polity"}, {"proposal", "accession of the polity"}}), why);
    s.require("once a second member agrees (2/3 by community), the polity is admitted: a person who votes as the cooperative's envoy votes for the cooperative",
              all && league.holds("polity", "member"), why);
    const auto notary_votes = thesmos::chronicle::step(league, act("registry", "vote", 3, {{"id", "accession of the polity"}, {"value", "yes"}}));
    s.require("the league's notary, which the charter forbids to vote, cannot vote", !notary_votes.admitted, notary_votes.reason);
  }

  // ---- A compact between two commons ----
  const auto commons_rules = thesmos::constitutions::read(slurp(data + "commons.json"));
  State x = commons("x/", commons_rules), y = commons("y/", commons_rules);
  {
    const federation::Compact compact{"clearing compact", {"x", "y"}};
    std::string why;
    bool all = ok(x, act("x/steward", "propose", 1, {{"id", "ratify"}, {"level", "constitutional"}, {"change.compact", "clearing compact"}}), why);
    all = all && ok(x, act("x/steward", "vote", 1, {{"id", "ratify"}, {"value", "yes"}}), why) && ok(x, act("x/reviewer", "vote", 1, {{"id", "ratify"}, {"value", "yes"}}), why);
    const auto hasty = thesmos::chronicle::step(x, act("x/steward", "amend", 2, {{"id", "ratify"}}));
    s.require("a ratification decided at the constitutional tempo waits its turns: an amendment one turn after the proposal is refused", all && !hasty.admitted && hasty.reason.find("tempo") != std::string::npos,
              why + hasty.reason);
    all = ok(x, act("x/steward", "amend", 4, {{"id", "ratify"}}), why);
    const auto half = federation::standing(compact, {{"x", &x}, {"y", &y}});
    s.require("the first commons ratifies by its own rule; the compact is not in force while the other has not", all && half.ratified == std::vector<std::string>{"x"} && !half.in_force(), why);
    all = ok(y, act("y/steward", "propose", 5, {{"id", "ratify"}, {"level", "constitutional"}, {"change.compact", "clearing compact"}}), why);
    all = all && ok(y, act("y/steward", "vote", 5, {{"id", "ratify"}, {"value", "yes"}}), why) && ok(y, act("y/reviewer", "vote", 5, {{"id", "ratify"}, {"value", "yes"}}), why);
    all = all && ok(y, act("y/steward", "amend", 8, {{"id", "ratify"}}), why);
    s.require("once both have ratified, each by its own procedure, the compact is in force", all && federation::standing(compact, {{"x", &x}, {"y", &y}}).in_force(), why);
  }

  // ---- A compact of three with its terms, a reservation and a withdrawal (thesmos's decision 0013) ----
  {
    const std::string terms = R"({"format": "noetarium.compact/1", "id": "review compact", "parties": ["p", "q", "r"],
      "articles": ["mutual review", {"id": "shared record", "closed to reservation": true}, "common bench"], "entry into force": 2, "withdrawal": {"notice": 2}})";
    auto named = [&](const std::string& name) {  // a commons whose founding names its community
      Parties ps;
      for (const char* x : {"steward", "reviewer"}) ps.add({name + "/" + x, Kind::person});
      State st(ps, commons_rules);
      Act found = act(name + "/steward", "found", 0, {{"community", name}});
      found.holders = {{name + "/steward", "steward"}, {name + "/reviewer", "reviewer"}};
      thesmos::chronicle::step(st, found);
      return st;
    };
    State p = named("p"), q = named("q"), r = named("r");
    std::string why;
    auto ratify = [&](State& st, const std::string& pre, long long turn, std::map<std::string, std::string> change) {
      std::map<std::string, std::string> f{{"id", "c" + std::to_string(turn)}, {"level", "constitutional"}};
      for (auto& [k, v] : change) f["change." + k] = v;
      bool done = ok(st, act(pre + "steward", "propose", turn, f), why);
      done = done && ok(st, act(pre + "steward", "vote", turn, {{"id", f["id"]}, {"value", "yes"}}), why) && ok(st, act(pre + "reviewer", "vote", turn, {{"id", f["id"]}, {"value", "yes"}}), why);
      return done && ok(st, act(pre + "steward", "amend", turn + 3, {{"id", f["id"]}}), why);
    };
    const federation::Compact compact{"review compact", {"p", "q", "r"}};
    bool all = ratify(p, "p/", 1, {{"compact", "review compact"}, {"terms", terms}});
    const auto one = federation::standing(compact, {{"p", &p}, {"q", &q}, {"r", &r}});
    all = all && ratify(q, "q/", 2, {{"compact", "review compact"}, {"terms", terms}, {"reservations", "common bench"}});
    const auto two = federation::standing(compact, {{"p", &p}, {"q", &q}, {"r", &r}});
    all = all && ratify(r, "r/", 3, {{"compact", "review compact"}, {"terms", terms}});
    const bool reserved = !federation::binds(compact, {{"p", &p}, {"q", &q}, {"r", &r}}, 6, "common bench", "p", "q") &&
                          federation::binds(compact, {{"p", &p}, {"q", &q}, {"r", &r}}, 6, "common bench", "p", "r");
    all = all && ratify(p, "p/", 7, {{"withdraw compact", "review compact"}});
    const auto during = federation::standing(compact, {{"p", &p}, {"q", &q}, {"r", &r}}, 11), after = federation::standing(compact, {{"p", &p}, {"q", &q}, {"r", &r}}, 12);
    s.require("a compact of three entering into force at two ratifications, read from the communities' own chronicles: not in force with one; in force from the second at "
              "turn 5; the second party's reservation of the common bench excluding it between it and the others; the first party's withdrawal at turn 10 binding it "
              "through the notice and not from turn 12, the other two still bound",
              all && !one.in_force() && two.in_force() && two.entered == 5 && reserved && during.in_force() && during.bound.size() == 3 && after.in_force() &&
                  after.bound == std::vector<std::string>{"q", "r"} && after.withdrawn == std::vector<std::string>{"p"},
              why);
  }

  // ---- A crossing, two notaries, recognition ----
  {
    const Act notice = act("x/steward", "request", 9, {{"to community", "y"}, {"matter", "the first clearing round"}});
    thesmos::chronicle::step(x, notice);
    const auto got = thesmos::crossings::receive(y, "y", "y/steward", 9, "x", x.journal.entries.back().seal, notice);
    s.require("a request from one commons to the other is entered on both sides, and the receipt verifies", got.admitted && thesmos::crossings::verify(got.receipt, x).holds(), got.reason);

    federation::Notary first("the first notary"), second("the second notary");
    const auto h1 = thesmos::crossings::head("x", x);
    first.witness(h1, {});
    second.witness(h1, {});
    thesmos::chronicle::step(x, act("x/steward", "request", 10, {{"matter", "the second round"}}));
    const auto h2 = thesmos::crossings::head("x", x);
    const auto proof = thesmos::crossings::consistency(x, h1.size);
    s.require("both notaries witness the commons' later head with its proof of consistency", first.witness(h2, proof).holds() && second.witness(h2, proof).holds());
    State rewritten(x.parties, commons_rules);
    for (std::size_t k = 0; k < x.acts.size(); k++) {
      Act a = x.acts[k];
      if (k == 1) a.fields["change.compact"] = "another compact";
      thesmos::chronicle::step(rewritten, a);
    }
    thesmos::chronicle::step(rewritten, act("x/steward", "request", 11, {{"matter", "the third round"}}));
    const auto h3 = thesmos::crossings::head("x", rewritten);
    const auto bad = thesmos::crossings::consistency(rewritten, h2.size);
    s.require("a commons that rewrote what it had shown cannot have its new head witnessed by either notary", first.witness(h3, bad).fails() && second.witness(h3, bad).fails());

    federation::Recognitions r;
    r.recognise("x", "y", federation::Level::evidence);
    r.recognise("y", "z", federation::Level::decisions);
    s.require("recognition is directed and stops at the party recognised",
              r.recognises("x", "y", federation::Level::evidence) && !r.recognises("x", "y", federation::Level::undertakings) && !r.recognises("y", "x", federation::Level::exists) &&
                  !r.recognises("x", "z", federation::Level::exists));
    r.withdraw("x", "y");
    s.require("a recognition withdrawn ends", !r.recognises("x", "y", federation::Level::exists));
  }

  // ---- Recognitions read from the communities' own chronicles, and a compact open to any community (the design's Phase 8) ----
  {
    auto named = [&](const std::string& name) {
      Parties ps;
      for (const char* x : {"steward", "reviewer"}) ps.add({name + "/" + x, Kind::person});
      State st(ps, commons_rules);
      Act found = act(name + "/steward", "found", 0, {{"community", name}});
      found.holders = {{name + "/steward", "steward"}, {name + "/reviewer", "reviewer"}};
      thesmos::chronicle::step(st, found);
      return st;
    };
    State a = named("a"), b = named("b"), c = named("c"), d = named("d");
    std::string why;
    bool all = ok(a, act("a/steward", "recognise", 1, {{"of", "b"}, {"level", "evidence"}}), why) &&
               ok(a, act("a/steward", "recognise", 1, {{"of", "c"}, {"level", "decisions"}, {"matter", "clearing"}}), why) &&
               ok(b, act("b/steward", "recognise", 1, {{"of", "a"}, {"level", "exists"}}), why);
    const auto r = federation::Recognitions::read({{"a", &a}, {"b", &b}, {"c", &c}});
    s.require("recognitions read from each community's own chronicle: a recognises b's attestations as evidence and c's decisions for the matter of clearing alone, "
              "b recognises that a exists, c recognises no one; recognition is mutual at the level of existence and not of evidence",
              all && r.level("a", "b") == federation::Level::evidence && r.recognises("a", "c", federation::Level::decisions, "clearing") &&
                  !r.recognises("a", "c", federation::Level::exists) && r.level("b", "a") == federation::Level::exists && r.level("c", "a") == federation::Level::none &&
                  r.mutual("a", "b", federation::Level::exists) && !r.mutual("a", "b", federation::Level::evidence),
              why);
    // A founding treaty that names no parties: any community may ratify it; in force at two ratifications; withdrawal after four turns.
    const std::string terms = R"({"format": "noetarium.compact/1", "id": "the founding treaty", "parties": [],
      "articles": ["article 1", {"id": "article 2", "closed to reservation": true}], "entry into force": 2, "withdrawal": {"notice": 4}})";
    auto ratify = [&](State& st, const std::string& pre, long long turn, std::map<std::string, std::string> change) {
      std::map<std::string, std::string> f{{"id", "t" + std::to_string(turn)}, {"level", "constitutional"}};
      for (auto& [k, v] : change) f["change." + k] = v;
      bool done = ok(st, act(pre + "steward", "propose", turn, f), why);
      done = done && ok(st, act(pre + "steward", "vote", turn, {{"id", f["id"]}, {"value", "yes"}}), why) && ok(st, act(pre + "reviewer", "vote", turn, {{"id", f["id"]}, {"value", "yes"}}), why);
      return done && ok(st, act(pre + "steward", "amend", turn + 3, {{"id", f["id"]}}), why);
    };
    all = ratify(a, "a/", 2, {{"compact", "the founding treaty"}, {"terms", terms}}) && ratify(b, "b/", 3, {{"compact", "the founding treaty"}, {"terms", terms}}) &&
          ratify(c, "c/", 4, {{"compact", "the founding treaty"}, {"terms", terms}}) && ratify(a, "a/", 8, {{"withdraw compact", "the founding treaty"}});
    const federation::Compact open{"the founding treaty", {}};
    const federation::Chronicles all4{{"a", &a}, {"b", &b}, {"c", &c}, {"d", &d}};
    const auto at5 = federation::standing(open, all4, 5), at6 = federation::standing(open, all4, 6), at14 = federation::standing(open, all4, 14),
               at15 = federation::standing(open, all4, 15);
    s.require("a compact open to any community has for parties those whose chronicles ratify it, here a, b and c and never d: at turn 5 only a's ratification stands; "
              "it enters into force at turn 6 with b's; a's withdrawal, adopted at turn 11, binds a through its notice of four turns and not from turn 15; an article "
              "binds two parties that reserved nothing",
              all && at5.ratified == std::vector<std::string>{"a"} && at5.pending == std::vector<std::string>{"b", "c"} && !at5.in_force() && at6.in_force() && at6.entered == 6 &&
                  at14.bound == std::vector<std::string>{"a", "b", "c"} && at15.bound == std::vector<std::string>{"b", "c"} && at15.withdrawn == std::vector<std::string>{"a"} &&
                  federation::binds(open, all4, 10, "article 1", "a", "c") && !federation::binds(open, all4, 10, "article 1", "a", "d"),
              why);
  }
  return s.done();
}
