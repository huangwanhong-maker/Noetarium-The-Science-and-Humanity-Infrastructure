// Checks of registries: a listing written and read back, its faults named; a listing made from a community's record, its profile's digest
// that of the profile document; verified current against the community's own profile, a trusted notary's witness and a proof of
// consistency; stale once the community amends what it publishes; unverified without a trusted witness; faulty when the community's record
// was rewritten or the profile is another's; claimed when the community says it is listed; a registry in its keeper's chronicle (the league
// preset): a member's agent lists its community, pending until another origin checks it, an outsider refused, a listing delisted by the
// custodian with its reason and the norm that permitted it, one delisted with no reason found as a fault; and a search across two registries
// that gathers each community once, the newest listing first, leaves out what is not current, and reports where the registries disagree.

#include <communities/registries/registries.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::acts::Act;
using thesmos::chronicle::State;
using thesmos::kernel::Kind;
using thesmos::kernel::Parties;
namespace ch = thesmos::chronicle;
namespace rg = communities::registries;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
thesmos::constitutions::Constitution preset(const std::string& name, const std::string& listed_in = {}) {
  auto doc = symploke::jsonld::parse(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/" + name + ".json"));
  if (!listed_in.empty()) doc.get("external")->set("listed in", symploke::jsonld::parse("[\"" + listed_in + "\"]"));
  return thesmos::constitutions::read(symploke::jsonld::dump(doc));
}
// A community on a preset founded by two persons, by default its steward and its reviewer.
State founded(const thesmos::constitutions::Constitution& c, const std::string& p, const std::string& first = "steward", const std::string& second = "reviewer") {
  Parties ps;
  for (const char* x : {"steward", "reviewer"}) ps.add({p + "/" + x, Kind::person});
  State s(ps, c);
  Act f = act(p + "/steward", "found", 0);
  f.holders = {{p + "/steward", first}, {p + "/reviewer", second}};
  ch::step(s, f);
  return s;
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // The notary: a league whose secretary attests heads.
  Parties lp;
  for (const char* x : {"convener", "sec"}) lp.add({x, Kind::person});
  for (const char* c : {"atelier", "outsider"}) lp.add({c, Kind::community});
  lp.add({"atelier/agent", Kind::operative, "atelier", "anthropic"});
  lp.add({"outsider/agent", Kind::operative, "outsider", "anthropic"});
  State league(lp, preset("league"));
  {
    Act f = act("convener", "found", 0);
    f.holders = {{"atelier", "member"}, {"sec", "secretary"}};
    ch::step(league, f);
  }

  State atelier = founded(preset("commons", "the league/registry"), "atelier");
  ch::step(atelier, act("atelier/steward", "say", 1));
  const auto witnessed = thesmos::crossings::witness(league, "the league", "sec", 1, thesmos::crossings::head("atelier", atelier));
  const rg::Listing listing = rg::listing_of("atelier", atelier, "http://127.0.0.1:9230/", "the league", "2026-11-28T01:00:00Z");

  // ---- 1. The written form ----
  {
    const std::string text = rg::write(listing);
    const auto back = rg::read(text);
    const auto bad = rg::read(R"({"format": "noetarium.listing/1", "profile": {"sha256": "abc"}, "head": {"size": 3}})");
    s.require("a listing is written in its canonical form and read back to the same text; one that names no community, a profile by no digest and a head with no root "
              "is not read, each fault named",
              back.read && rg::write(back.listing) == text && rg::digest(back.listing) == archeion::fixity::sha256_hex(text) && !bad.read && bad.faults.size() == 3,
              back.faults.empty() ? "" : back.faults.front());
  }

  // ---- 2. A listing from the record ----
  s.require("a listing made from a community's record: its order and purpose, the digest of its profile document, its head, the positions an outsider may enter "
            "(contributor and visitor, open) and its open work",
            listing.order == "commons" && listing.purpose == atelier.rules.purpose && listing.profile_sha256 == archeion::fixity::sha256_hex(rg::profile_document("atelier", atelier)) &&
                listing.head.size == 2 && listing.open_positions == std::vector<std::string>{"contributor", "visitor"} && listing.open_work == 0 && witnessed.admitted,
            witnessed.reason);

  // ---- 3. Verified current, against the community's own profile and a trusted notary ----
  ch::step(atelier, act("atelier/steward", "say", 2));
  const rg::Evidence fresh{rg::profile_document("atelier", atelier), thesmos::crossings::head("atelier", atelier), thesmos::crossings::consistency(atelier, listing.head.size),
                           {witnessed.witness}, {witnessed.witness.at}, "the league/registry"};
  const auto current = rg::verify(listing, fresh);
  s.require("verified current: the profile fetched has the digest listed, a notary the reader trusts witnessed the listing's head, the listing's head is the beginning of "
            "the community's present head, and the community says it is listed in this registry",
            current.status == "current" && current.witnessed && current.profile_current && current.consistent && current.claimed && current.reasons.empty(),
            current.reasons.empty() ? "" : current.reasons.front());

  // ---- 4. Stale, unverified, faulty, unclaimed ----
  {
    State amended = atelier;
    bool ok = ch::step(amended, act("atelier/steward", "propose", 3, {{"id", "open review"}, {"decision", "amend operational"}, {"change.open", "reviewer"}})).admitted;
    ok = ok && ch::step(amended, act("atelier/steward", "vote", 3, {{"id", "open review"}, {"value", "yes"}})).admitted &&
         ch::step(amended, act("atelier/reviewer", "vote", 3, {{"id", "open review"}, {"value", "yes"}})).admitted &&
         ch::step(amended, act("atelier/steward", "amend", 4, {{"id", "open review"}})).admitted;
    const rg::Evidence later{rg::profile_document("atelier", amended), thesmos::crossings::head("atelier", amended), thesmos::crossings::consistency(amended, listing.head.size),
                             {witnessed.witness}, {witnessed.witness.at}, "the league/registry"};
    const auto stale = rg::verify(listing, later);
    s.require("stale once the community amends what it publishes (its reviewers' seat opened): the record still begins with the head listed, and the profile's digest differs",
              ok && stale.status == "stale" && stale.consistent && stale.witnessed && !stale.profile_current, stale.reasons.empty() ? "" : stale.reasons.front());
    rg::Evidence untrusted = fresh;
    untrusted.notaries.clear();
    rg::Evidence unwitnessed = fresh;
    unwitnessed.witnesses.clear();
    s.require("unverified when the reader trusts no notary that witnessed the head, or holds no witness", rg::verify(listing, untrusted).status == "unverified" &&
                                                                                                             rg::verify(listing, unwitnessed).status == "unverified");
    State rewritten = founded(preset("commons", "the league/registry"), "atelier");
    ch::step(rewritten, act("atelier/steward", "say", 1, {{"matter", "another story"}}));
    ch::step(rewritten, act("atelier/steward", "say", 2));
    const rg::Evidence forged{rg::profile_document("atelier", rewritten), thesmos::crossings::head("atelier", rewritten), thesmos::crossings::consistency(rewritten, listing.head.size),
                              {witnessed.witness}, {witnessed.witness.at}, "the league/registry"};
    rg::Evidence elsewhere = fresh;
    elsewhere.profile = rg::profile_document("another", atelier);
    const auto rewrote = rg::verify(listing, forged), other = rg::verify(listing, elsewhere);
    rg::Evidence unclaimed = fresh;
    unclaimed.registry = "another registry";
    const auto quiet = rg::verify(listing, unclaimed);
    s.require("faulty when the community's present record does not begin with the head listed (it was rewritten), or the profile fetched is another community's; "
              "current but unclaimed when the community does not say it is listed in the registry read",
              rewrote.status == "faulty" && has(rewrote.reasons.front(), "rewritten") && other.status == "faulty" && quiet.status == "current" && !quiet.claimed &&
                  has(quiet.reasons.back(), "another registry"),
              rewrote.reasons.empty() ? "" : rewrote.reasons.front());
  }

  // ---- 5. A registry in its keeper's chronicle ----
  std::string kept;
  {
    std::string why;
    auto ok = [&](const Act& a) {
      const auto o = ch::step(league, a);
      if (!o.admitted && why.empty()) why = a.author + " " + a.kind + ": " + o.reason;
      return o.admitted;
    };
    Act m = act("atelier", "mandate", 2);
    thesmos::mandates::Mandate g;
    g.id = "m-atelier", g.principal = "atelier", g.holder = "atelier/agent", g.kinds = {"join", "deposit", "withdraw"}, g.from = 0, g.to = 100;
    m.grant = g;
    Act n = act("outsider", "mandate", 2);
    g.id = "m-outsider", g.principal = "outsider", g.holder = "outsider/agent";
    n.grant = g;
    kept = rg::write(listing);
    const std::string d = rg::digest(listing);
    bool all = ok(m) && ok(n) &&
               ok(act("atelier/agent", "deposit", 3, {{"holding", "atelier listing"}, {"within", "registry"}, {"kind", "listing"}, {"digest", d}, {"source", "the listing of atelier"}}, "m-atelier"));
    const auto pending = rg::entries(league, "registry");
    all = all && ok(act("sec", "attest", 3, {{"holding", "atelier listing"}}));
    const auto listed = rg::entries(league, "registry");
    const auto refused = ch::step(league, act("outsider/agent", "deposit", 3, {{"holding", "outsider listing"}, {"within", "registry"}, {"kind", "listing"}, {"digest", d}}, "m-outsider"));
    s.require("in the league's registry a member's agent lists its community, pending until a party of another origin checks it, then listed with its digest and who "
              "answers for it; the listing's text is the one held; an outsider's agent is refused by the norms",
              all && pending.size() == 1 && pending[0].status == "pending" && listed[0].status == "listed" && listed[0].checked_by == "sec" &&
                  listed[0].depositor == "atelier" && listed[0].deposited_by == "atelier/agent" && rg::holds(listed[0], kept) && !rg::holds(listed[0], kept + " ") && !refused.admitted,
              why + " / " + refused.reason);
    why.clear();
    all = ok(act("sec", "withdraw", 4, {{"holding", "atelier listing"}, {"reason", "the community left the league"}}));
    all = all && ok(act("atelier/agent", "deposit", 5, {{"holding", "atelier listing 2"}, {"within", "registry"}, {"kind", "listing"}, {"digest", d}}, "m-atelier")) &&
          ok(act("sec", "attest", 5, {{"holding", "atelier listing 2"}})) && ok(act("sec", "withdraw", 6, {{"holding", "atelier listing 2"}})) &&
          ok(act("atelier/agent", "deposit", 7, {{"holding", "atelier listing 3"}, {"within", "registry"}, {"kind", "listing"}, {"digest", d}}, "m-atelier")) &&
          ok(act("atelier/agent", "withdraw", 7, {{"holding", "atelier listing 3"}}, "m-atelier"));
    const auto es = rg::entries(league, "registry");
    const auto fs = rg::faults(es);
    std::map<std::string, rg::Entry> by;
    for (const auto& e : es) by[e.holding] = e;
    s.require("delisting with reasons: the secretary withdraws a listing, its reason and the norm that permitted it recorded; a second withdrawn by the secretary with no "
              "reason is found as a fault; a third withdrawn by its depositor needs none",
              all && by["atelier listing"].status == "delisted" && by["atelier listing"].reason == "the community left the league" && by["atelier listing"].delisted_by == "sec" &&
                  by["atelier listing"].norm == "a listing withdrawn by the secretary with reasons" && fs.size() == 1 && has(fs[0], "atelier listing 2") &&
                  by["atelier listing 3"].status == "delisted" && by["atelier listing 3"].norm == "a listing withdrawn by its depositor",
              why);
  }

  // ---- 6. A search across two registries ----
  {
    State guild = founded(preset("guild", "registry A"), "smiths");
    State coop = founded(preset("cooperative", "registry B"), "bakers", "board", "member");
    auto checked = [&](const std::string& name, const State& st, const std::string& registry) {
      State notary = league;
      const auto w = thesmos::crossings::witness(notary, "the league", "sec", 9, thesmos::crossings::head(name, st));
      const rg::Listing l = rg::listing_of(name, st, "http://node/", "the league", "2026-11-28T02:00:00Z");
      return std::pair<rg::Listing, rg::Witness>{l, w.witness};
    };
    s.require("the three communities to be listed are founded", atelier.journal.entries.size() == 3 && guild.journal.entries.size() == 1 && coop.journal.entries.size() == 1);
    const auto [a1, w1] = checked("atelier", atelier, "registry A");
    State later = atelier;
    ch::step(later, act("atelier/steward", "say", 3));
    const auto [a2, w2] = checked("atelier", later, "registry B");
    const auto [g1, w3] = checked("smiths", guild, "registry A");
    const auto [c1, w4] = checked("bakers", coop, "registry B");
    State forged = founded(preset("commons", "the league/registry"), "atelier");
    ch::step(forged, act("atelier/steward", "say", 1, {{"matter", "another story"}}));
    ch::step(forged, act("atelier/steward", "say", 2));
    const auto [f1, w5] = checked("atelier", forged, "registry B");
    // What a reader holds: the community's profile now (changed, for the bakers, since they were listed), its present head with a proof of
    // consistency from the head listed, the witness, and the notary's head it trusts.
    auto evidence = [&](const std::string& name, const State& now, const rg::Listing& l, const rg::Witness& w, const std::string& registry, bool changed = false) {
      State shown = now;
      if (changed) shown.rules.purpose += " Amended.";
      return rg::Evidence{rg::profile_document(name, shown), thesmos::crossings::head(name, now), thesmos::crossings::consistency(now, l.head.size), {w}, {w.at}, registry};
    };
    const rg::Registry A{"the league", "registry A",
                         {{a1, rg::verify(a1, evidence("atelier", later, a1, w1, "registry A"))}, {g1, rg::verify(g1, evidence("smiths", guild, g1, w3, "registry A"))}}};
    const rg::Registry B{"a commons of listings", "registry B",
                         {{a2, rg::verify(a2, evidence("atelier", later, a2, w2, "registry B"))}, {c1, rg::verify(c1, evidence("bakers", coop, c1, w4, "registry B", true))},
                          {f1, rg::verify(f1, evidence("atelier", later, f1, w5, "registry B"))}}};
    const auto commons = rg::search({A, B}, {"commons", {"Commons"}, {}, true});
    const auto everything = rg::search({A, B}, {{}, {}, {}, false});
    const auto current = rg::search({A, B}, {{}, {}, {}, true});
    const auto contributors = rg::search({A, B}, {{}, {}, "contributor", true});
    std::vector<std::string> all_names, current_names;
    for (const auto& f : everything) all_names.push_back(f.community);
    for (const auto& f : current) current_names.push_back(f.community);
    s.require("a search across two registries: the commons listed in both is found once, under both registries, its newest listing the longer head; the forged listing "
              "of its record, a head of the same length with another root, is faulty, kept out of the newest and reported as a disagreement",
              commons.size() == 1 && commons[0].community == "atelier" && commons[0].registries == std::vector<std::string>{"the league/registry A", "a commons of listings/registry B"} &&
                  commons[0].newest.head.size == a2.head.size && a2.head.size == a1.head.size + 1 && commons[0].status == "current" &&
                  commons[0].disagreements == std::vector<std::string>{"two listings name heads of " + std::to_string(a1.head.size) + " entries with different roots"},
              commons.empty() ? "nothing found" : (commons[0].disagreements.empty() ? "no disagreement" : commons[0].disagreements.front()));
    s.require("the bakers' listing, whose profile has changed since, is stale and left out unless every listing is asked for; a search for an open position finds the "
              "communities that list it open (the commons' contributors), and each community once",
              all_names == std::vector<std::string>{"atelier", "bakers", "smiths"} && current_names == std::vector<std::string>{"atelier", "smiths"} &&
                  contributors.size() == 1 && contributors[0].community == "atelier");
  }
  return s.done();
}
