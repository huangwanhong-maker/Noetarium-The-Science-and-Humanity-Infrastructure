// Checks of federation, on the presets themselves:
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
  return s.done();
}
