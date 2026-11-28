// Checks of the presets: six constitutions of different internal orders, written in one schema, each read and admitted by
// thesmos; they differ in their units of count; only the leagues give a position to communities alone; the first league's
// charter is entrenched and amended only by unanimity of its member communities. Each holds what fits its order, and its norms of
// access decide as its order intends (thesmos's holdings): the commons' shelf, the guild's bench, the cooperative's pool by quota,
// mandate and need, the polity's records, the leagues' plan and briefing. The household, a seventh, is admitted and is the only
// household: one person issues and revokes the mandates of the agents they send (thesmos's decision 0007).

#include <communities/presets/presets.hpp>

#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/profile.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::constitutions::Constitution;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}

namespace h = thesmos::holdings;

// A declared holding as its norms see it, or the new holding a deposit within it would make.
h::Target target(const Constitution& c, const std::string& id, bool fresh = false) {
  const h::Holding& x = c.holdings.holdings.at(id);
  h::Target t;
  t.id = fresh ? std::string() : id, t.kind = x.kind, t.custodian = x.custodian, t.office = x.office, t.metered = x.metered(), t.unit = x.unit;
  if (fresh) t.fresh = true, t.within = {id};
  return t;
}
h::Situation party(std::string id, std::set<std::string> positions, thesmos::kernel::Kind kind = thesmos::kernel::Kind::person) {
  h::Situation s;
  s.party = s.bearer = s.origin = std::move(id);
  s.kind = kind;
  s.positions = s.origin_positions = std::move(positions);
  return s;
}
// An operative of `origin`, under its mandate: the positions it holds itself, and those the party that answers for it holds.
h::Situation agent(std::string origin, std::set<std::string> own, std::set<std::string> origins, std::set<std::string> kinds = {}, std::set<std::string> matters = {}) {
  h::Situation s;
  s.party = origin + "/agent", s.bearer = s.origin = origin, s.kind = thesmos::kernel::Kind::operative;
  s.positions = std::move(own), s.bearer_positions = origins, s.origin_positions = std::move(origins);
  s.mandated = true, s.mandate_kinds = std::move(kinds), s.mandate_matters = std::move(matters);
  return s;
}
bool may(const Constitution& c, const h::Target& t, h::Aim a, const h::Situation& s, long long amount = 0, std::string purpose = {}) {
  return h::decide(c.holdings, t, {a, amount, std::move(purpose)}, s).permitted;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const std::vector<std::string> names = {"commons", "guild", "cooperative", "polity", "league", "first-league"};
  std::map<std::string, Constitution> presets;
  for (const auto& name : names) {
    const std::string text = slurp(std::string(COMMUNITIES_DATA) + "/presets/data/" + name + ".json");
    s.require("the preset " + name + " is read", !text.empty());
    presets[name] = thesmos::constitutions::read(text);
    const auto faults = thesmos::constitutions::admission(presets[name]);
    s.require("the preset " + name + " is admitted by form", faults.empty(), faults.empty() ? "" : std::to_string(faults.front().condition) + ": " + faults.front().reason);
  }
  std::set<std::set<std::string>> units;
  for (const auto& [name, c] : presets) units.insert(presets::profile(c).counted_by);
  s.require("the presets differ in their units of count (" + std::to_string(units.size()) + " different sets)", units.size() >= 3);
  std::vector<std::string> leagues;
  for (const auto& [name, c] : presets)
    if (presets::of_communities(c, "member")) leagues.push_back(name);
  s.require("of the first six, only the leagues give their membership to communities alone", leagues == std::vector<std::string>{"first-league", "league"});
  const auto commons = presets::profile(presets["commons"]);
  s.require("in the commons anyone answerable may contribute, operatives among them", commons.open_positions.count("contributor") && commons.operative_positions.count("contributor"));
  const Constitution& first = presets["first-league"];
  const auto& charter = first.aggregation.at(first.amendment.by_level.at("constitutional"));
  s.require("the first league's charter is entrenched and amended only by unanimity, counted by community",
            std::find(first.amendment.entrenched.begin(), first.amendment.entrenched.end(), "the charter") != first.amendment.entrenched.end() &&
                charter.rule == "unanimity" && charter.count_by == "community");

  // ---- What each holds, and the norms of access to it ---------------------------------------------------------------------------
  using h::Aim;
  bool held = true;
  for (const auto& [name, c] : presets) held = held && !presets::profile(c).holdings.empty() && presets::profile(c).norms > 0;
  s.require("every preset holds something and declares norms of access to it", held);
  const Constitution& cm = presets["commons"];
  s.require("the commons: anyone holding a position may consult its shelf and a contributor deposit to it, each deposit checked by another origin; a party "
            "holding none is refused, and the guide is open to all",
            may(cm, target(cm, "shelf"), Aim::consult, party("v", {"visitor"})) && may(cm, target(cm, "shelf", true), Aim::deposit, party("k", {"contributor"})) &&
                !may(cm, target(cm, "shelf", true), Aim::deposit, party("v", {"visitor"})) && cm.holdings.holdings.at("shelf").review == "another origin" &&
                !may(cm, target(cm, "shelf"), Aim::consult, party("x", {})) && may(cm, target(cm, "guide"), Aim::consult, party("x", {})));
  const Constitution& gd = presets["guild"];
  s.require("the guild: a member uses its bench for analysis or replication, an aide for replication alone, an applicant consults it and uses it not",
            may(gd, target(gd, "bench"), Aim::use, party("m", {"member"}), 0, "analysis") && may(gd, target(gd, "bench"), Aim::use, party("m", {"member"}), 0, "replication") &&
                may(gd, target(gd, "bench"), Aim::use, agent("m", {"aide"}, {"member"}), 0, "replication") &&
                !may(gd, target(gd, "bench"), Aim::use, agent("m", {"aide"}, {"member"}), 0, "analysis") && may(gd, target(gd, "bench"), Aim::consult, party("a", {"applicant"})) &&
                !may(gd, target(gd, "bench"), Aim::use, party("a", {"applicant"}), 0, "analysis"));
  const Constitution& co = presets["cooperative"];
  h::Situation member = party("m", {"member"}), hand = agent("m", {"hand"}, {"member"}, {"use"}, {"writing"}), candidate = party("c", {"candidate"});
  member.used["the pool drawn"] = 1500;
  const bool by_need_before = may(co, target(co, "writing pool"), Aim::use, candidate, 100);
  candidate.needs.insert("pressing need");
  s.require("the cooperative: members pool their writing budgets and draw 2,000 words a round; a member's hand draws within 1,000 under a mandate on writing; a "
            "candidate draws only when its need is attested",
            may(co, target(co, "writing pool"), Aim::deposit, member, 5000) && may(co, target(co, "writing pool"), Aim::use, member, 500) &&
                !may(co, target(co, "writing pool"), Aim::use, member, 501) && may(co, target(co, "writing pool"), Aim::use, hand, 1000) &&
                !may(co, target(co, "writing pool"), Aim::use, hand, 1001) && !by_need_before && may(co, target(co, "writing pool"), Aim::use, candidate, 100));
  const Constitution& po = presets["polity"];
  const auto never = h::decide(po.holdings, target(po, "records"), {Aim::withdraw, 0, {}}, party("g", {"magistrate"}));
  s.require("the polity: anyone consults its records, its magistrates amend them and a citizen may not, and no one withdraws them, the prohibition cited",
            may(po, target(po, "records"), Aim::consult, party("x", {})) && may(po, target(po, "records"), Aim::modify, party("g", {"magistrate"})) &&
                !may(po, target(po, "records"), Aim::modify, party("z", {"citizen"})) && !never.permitted && never.norm == "the records never withdrawn",
            never.reason);
  bool briefed = true;
  for (const auto& [name, office] : std::vector<std::pair<std::string, std::string>>{{"league", "secretary"}, {"first-league", "notary"}}) {
    const Constitution& lg = presets[name];
    const h::Situation envoy_agent = agent("a-community", {}, {"member"});
    const auto duties = h::duties(lg.holdings, {target(lg, "briefing")}, party("o", {office}));
    briefed = briefed && may(lg, target(lg, "plan"), Aim::consult, envoy_agent) && may(lg, target(lg, "briefing"), Aim::consult, envoy_agent) &&
              !may(lg, target(lg, "plan"), Aim::consult, party("x", {})) && duties.size() == 1 && lg.holdings.norms[duties[0]].id == "the briefing kept current";
  }
  s.require("the leagues: every member's agents consult the plan and the briefing, an outsider does not, and an office must keep the briefing current", briefed);
  using V = h::Visibility::Level;
  auto sent = [](const Constitution& c, const h::Target& t, const h::Situation& x) { return h::decide(c.holdings, t, {Aim::consult, 0, {}, "a model provider"}, x).permitted; };
  const Constitution& lg = presets["league"];
  s.require("what each order shows and sends outside: the commons' shelf and the polity's records are public and may be read through a model provider; the guild's "
            "bench is its members' and never leaves the guild; a league's briefing goes to the provider of a member's agent; the cooperative shows its custody of "
            "means to its board alone",
            cm.holdings.holdings.at("shelf").visibility.level == V::everyone && sent(cm, target(cm, "shelf"), party("k", {"contributor"})) &&
                po.holdings.holdings.at("records").visibility.level == V::everyone && sent(po, target(po, "records"), party("x", {})) &&
                gd.holdings.holdings.at("bench").visibility.level == V::members && !sent(gd, target(gd, "bench"), party("m", {"member"})) &&
                may(gd, target(gd, "bench"), Aim::consult, party("m", {"member"})) && sent(lg, target(lg, "briefing"), agent("a-community", {}, {"member"})) &&
                thesmos::constitutions::shown(co, "custody").offices == std::set<std::string>{"board"} &&
                thesmos::constitutions::shown(co, "custody").level == V::offices);

  // ---- The household: the smallest home, from which a person's passports are issued (thesmos's decision 0007) --------------------------
  {
    const std::string text = slurp(std::string(COMMUNITIES_DATA) + "/presets/data/household.json");
    const Constitution hh = thesmos::constitutions::read(text);
    const auto faults = thesmos::constitutions::admission(hh);
    s.require("the household is read and admitted by form", !text.empty() && faults.empty(), faults.empty() ? "" : std::to_string(faults.front().condition) + ": " + faults.front().reason);
    bool another = false;
    for (const auto& [name, c] : presets) another = another || presets::is_household(c);
    s.require("it is a household: one person as principal from its founding, the operatives that person controls as agents, mandates issued and revoked by the "
              "principal alone; no other preset is one",
              presets::is_household(hh) && !another && presets::profile(hh).issuers == std::set<std::string>{"principal"});
    const auto p = presets::profile(hh);
    s.require("it holds nothing, counts by person, enters agents by the principal's assignment, and keeps its route for claims, open to outsiders, with the person",
              p.holdings.empty() && p.counted_by == std::set<std::string>{"person"} && hh.boundary.at("agent").entry.decided_by == "assign agent" &&
                  hh.aggregation.at("assign agent").rule == "any one" && hh.claims.route == "principal" && hh.claims.outsiders);
    Constitution unrecognised = hh;
    for (auto& rule : unrecognised.choice) rule.may.erase(std::remove(rule.may.begin(), rule.may.end(), "issue passports"), rule.may.end());
    const auto over = thesmos::constitutions::admission(unrecognised);
    s.require("its power to issue passports is recognised by the principal's rules, and held without them it is overreach (condition 6)",
              !over.empty() && over.front().condition == 6 && over.front().reason.find("issue passports") != std::string::npos);
  }

  // ---- The presets of Phase 8, and what every preset now declares (thesmos's decisions 0009 to 0014) -------------------------------
  {
    std::map<std::string, Constitution> all;
    bool admitted = true;
    std::string why;
    for (const auto& name : presets::names()) {
      all[name] = thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/" + name + ".json"));
      const auto faults = thesmos::constitutions::admission(all[name]);
      if (!faults.empty() && why.empty()) why = name + ": " + std::to_string(faults.front().condition) + ": " + faults.front().reason;
      admitted = admitted && faults.empty();
    }
    s.require("all twelve presets are read and admitted by form, the five new ones among them", admitted && all.size() == 12, why);
    bool declared = true, transmission = true;
    std::set<std::string> orders;
    for (const auto& [name, c] : all) {
      declared = declared && !c.order.empty() && c.passports.declared && c.routes.declared;
      orders.insert(c.order);
      const bool new_one = std::find(presets::phase_eight().begin(), presets::phase_eight().end(), name) != presets::phase_eight().end();
      transmission = transmission && presets::profile(c).transmits == new_one;
    }
    s.require("every preset declares its order, from whom it accepts passports with its office of status, and a route for every directive; the orders are "
              "eight (commons, guild, cooperative, polity, league, household, organisation, foundation, treaty organisation)",
              declared && orders == std::set<std::string>{"commons", "cooperative", "foundation", "guild", "household", "league", "organisation", "polity", "treaty organisation"});
    s.require("the five new presets declare that operatives transmit to their provider, and the seven of before do not, so that their records replay as they were",
              transmission);
    const auto treaty = presets::profile(all["treaty-organisation"]), league = presets::profile(all["platform-league"]);
    const auto& budget = all["treaty-organisation"].aggregation.at("adopt budget");
    s.require("the treaty organisation: a council of five seats, a councillor's term and the secretary-general's, its budget adopted by a double majority of members "
              "(1/2) and weight (2/3) over declared weights, calls for contributions and their payments declared as kinds, and its founding treaty a compact with terms "
              "that names no parties, open to any community",
              all["treaty-organisation"].bodies.at("council").seats == 5 && treaty.termed == std::set<std::string>{"councillor", "secretary-general"} &&
                  budget.count_by == "weight" && budget.rule == "double majority of members at least 1/2 and weight at least 2/3" && budget.weights.size() == 4 &&
                  all["treaty-organisation"].behaviour.declared_kind("call for contributions") && all["treaty-organisation"].behaviour.declared_kind("pay contribution") &&
                  all["treaty-organisation"].compacts.size() == 1 && all["treaty-organisation"].compacts[0].has_terms && all["treaty-organisation"].compacts[0].terms.parties.empty(),
              budget.rule);
    s.require("the platform league: a committee of seven seats elected for a term, a maintainer held for a term and posted work of implementation reviewed by another "
              "provider; the completion: sections, reviews, an overview after every section and an audit as kinds of work, its assembly counted by provider",
              league.seated == std::set<std::string>{"committee"} && league.termed.count("technical") && league.termed.count("maintainer") && league.work == std::set<std::string>{"implementation"} &&
                  all["platform-league"].work.kinds.at("implementation").review_by == "another provider" &&
                  presets::profile(all["completion"]).work == std::set<std::string>{"audit", "overview", "review", "section"} &&
                  all["completion"].aggregation.at("amend operational").count_by == "provider" && all["completion"].work.kinds.at("overview").after_kinds == std::vector<std::string>{"section"});

    // A sample party's briefing in each new preset gives the verdicts its norms state.
    namespace ch = thesmos::chronicle;
    using thesmos::acts::Act;
    using thesmos::kernel::Kind;
    auto act = [](std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
      Act a;
      a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
      return a;
    };
    auto mandate = [&](const std::string& principal, const std::string& holder, std::set<std::string> kinds, std::set<std::string> matters = {}) {
      Act a = act(principal, "mandate", 0);
      thesmos::mandates::Mandate m;
      m.id = "m-" + holder, m.principal = principal, m.holder = holder, m.kinds = std::move(kinds), m.matters = std::move(matters), m.from = 0, m.to = 100;
      a.grant = m;
      return a;
    };
    // 1 permitted, 0 refused, -1 not listed: the briefing's entry for a holding (or the new holding a deposit within it would make) and an aim.
    auto verdict = [](const ch::Briefing& b, const std::string& holding, h::Aim aim, bool fresh = false) {
      for (const auto& e : b.entries) {
        const auto& t = b.targets[e.target];
        const bool here = fresh ? (t.fresh && !t.within.empty() && t.within.front() == holding) : (!t.fresh && t.id == holding);
        if (here && e.aim == aim) return e.decision.permitted ? 1 : 0;
      }
      return -1;
    };
    std::string steps;
    auto run = [&](ch::State& st, const Act& a) {
      const auto o = ch::step(st, a);
      if (!o.admitted && steps.empty()) steps = a.author + " " + a.kind + ": " + o.reason;
      return o.admitted;
    };
    {  // the completion: writers by application while sections are open; sections read only through the providers the principals named
      thesmos::kernel::Parties ps;
      ps.add({"author", Kind::person});
      ps.add({"steward", Kind::operative, "author", "openai"});
      ps.add({"codex", Kind::operative, "author", "openai"});
      ps.add({"gem", Kind::operative, "author", "google"});
      ch::State st(ps, all["completion"]);
      Act f = act("author", "found", 0);
      f.holders = {{"author", "principal"}, {"steward", "steward"}};
      bool ok = run(st, f) && run(st, mandate("author", "steward", {"request", "accept", "answer", "report", "say"})) &&
                run(st, mandate("author", "codex", {"apply", "commit", "consult", "deposit", "deliver", "say"})) && run(st, mandate("author", "gem", {"join", "consult", "say"})) &&
                run(st, act("steward", "request", 1, {{"id", "s-c28-3"}, {"work", "section"}, {"spec", "plan"}}, "m-steward")) &&
                run(st, act("codex", "apply", 1, {{"id", "ap-1"}, {"position", "writer"}}, "m-codex")) && run(st, act("gem", "join", 1, {{"position", "visitor"}}, "m-gem"));
      const auto codex = ch::brief(st, "codex", "m-codex"), gem = ch::brief(st, "gem", "m-gem");
      s.require("the completion: a writer of OpenAI admitted on application while a section is open; its briefing permits it to read the sections (its reading reaching "
                "openai, which the norm names) and to deposit within them, and refuses it the adopted sections' modification; a visitor of Google may read the plan "
                "and is refused the sections, its reading reaching google, which the norm does not name",
                ok && st.holds("codex", "writer") && st.items.at("ap-1").status == "admitted" && codex.reaches == "openai" && verdict(codex, "sections", h::Aim::consult) == 1 &&
                    verdict(codex, "sections", h::Aim::deposit, true) == 1 && verdict(codex, "adopted", h::Aim::modify) == 0 && gem.reaches == "google" &&
                    verdict(gem, "plan", h::Aim::consult) == 1 && verdict(gem, "sections", h::Aim::consult) == 0,
                steps);
    }
    {  // the treaty organisation: a member's envoy pays into the core budget, reads the scale and registers a treaty; the council's seats
      thesmos::kernel::Parties ps;
      for (const char* p : {"convener", "ana", "sg"}) ps.add({p, Kind::person});  // ana, a person, the commons' envoy under its mandate
      for (const char* c : {"commons", "polity"}) ps.add({c, Kind::community});
      ch::State st(ps, all["treaty-organisation"]);
      Act f = act("convener", "found", 0);
      f.holders = {{"commons", "member"}, {"polity", "member"}, {"sg", "secretary-general"}};
      steps.clear();
      bool ok = run(st, f) && run(st, mandate("commons", "ana", {"join", "pay contribution", "deposit", "consult", "vote", "propose"})) &&
                run(st, act("ana", "join", 1, {{"position", "envoy"}, {"sponsor", "commons"}}, "m-ana")) &&
                run(st, act("sg", "call for contributions", 1, {{"id", "a1"}, {"member", "commons"}, {"amount", "10"}, {"period", "1"}, {"due", "4"}})) &&
                run(st, act("ana", "pay contribution", 2, {{"amount", "6"}, {"contribution", "a1"}}, "m-ana"));
      const auto unnamed = ch::step(st, act("sg", "call for contributions", 2, {{"id", "a2"}, {"member", "polity"}, {"period", "1"}, {"due", "4"}}));
      const auto ana = ch::brief(st, "ana", "m-ana");
      s.require("the treaty organisation: a call for contributions and a payment, each a declared kind, admitted and read as a request and a deposit into the core "
                "budget, which then holds 6 service hours; a call that names no amount refused; the envoy's briefing permits it to pay in, to read the scale and to "
                "register a treaty (the commons, which answers for her acts, holding the member's position), and lists no use of the budget for her",
                ok && st.readings[3].kind == "request" && st.readings[3].field("matter") == "assessed contribution" && st.readings[4].kind == "deposit" &&
                    st.holdings.at("core budget").quantity == 6 && !unnamed.admitted && verdict(ana, "core budget", h::Aim::deposit) == 1 &&
                    verdict(ana, "scale", h::Aim::consult) == 1 && verdict(ana, "treaty register", h::Aim::deposit, true) == 1 && verdict(ana, "core budget", h::Aim::use) != 1,
                steps + " / " + unnamed.reason);
    }
    {  // the platform league: the public profile shows its order and its public holdings; the registrar's duty to keep the docket current
      thesmos::kernel::Parties ps;
      ps.add({"convener", Kind::person});
      ps.add({"reg", Kind::person});
      ps.add({"completion", Kind::community});
      ch::State st(ps, all["platform-league"]);
      Act f = act("convener", "found", 0);
      f.holders = {{"completion", "member"}, {"reg", "registrar"}};
      steps.clear();
      const bool ok = run(st, f);
      const auto pub = ch::profile(st);
      const auto reg = ch::brief(st, "reg");
      bool docket = false;
      for (const auto& n : reg.duties) docket = docket || n.id == "the docket kept current";
      std::set<std::string> shown;
      for (const auto& x : pub.holdings) shown.insert(x.id);
      s.require("the platform league: its public profile gives its order, the registries it is listed in, and its public holdings (specification, code, conformance, "
                "releases, docket, registry); the registrar's briefing lays on it the duty to keep the docket current",
                ok && pub.order == "treaty organisation" && pub.listed_in == std::vector<std::string>{"platform-league/registry"} &&
                    shown == std::set<std::string>{"code", "conformance", "docket", "registry", "releases", "specification"} && docket,
                steps);
    }
    {  // the organisation and the foundation: staff of one provider under the legal person's mandate; an applicant's report, an officer's draw
      thesmos::kernel::Parties ps;
      ps.add({"firm", Kind::organisation});
      ps.add({"chair", Kind::person});
      ps.add({"admin", Kind::person});
      ps.add({"aide", Kind::operative, "firm", "anthropic"});
      ch::State st(ps, all["organisation"]);
      Act f = act("firm", "found", 0);
      f.holders = {{"firm", "legal person"}, {"chair", "board"}, {"admin", "administration"}};
      steps.clear();
      bool ok = run(st, f) && run(st, mandate("firm", "aide", {"consult", "use", "deposit", "commit", "deliver"}, {"the organisation's work"})) &&
                run(st, act("admin", "admit", 1, {{"position", "staff"}, {"whom", "aide"}}));
      const auto aide = ch::brief(st, "aide", "m-aide");
      bool budget_quota = false;
      for (const auto& e : aide.entries)
        if (aide.targets[e.target].id == "budget" && e.aim == h::Aim::use) budget_quota = e.decision.permitted && e.remaining == 20;
      thesmos::kernel::Parties qs;
      for (const char* p : {"t1", "t2", "off", "app", "ext"}) qs.add({p, Kind::person});
      ch::State fd(qs, all["foundation"]);
      Act g = act("t1", "found", 0);
      g.holders = {{"t1", "trustee"}, {"t2", "trustee"}};
      ok = ok && run(fd, g) && run(fd, act("t1", "admit", 1, {{"position", "officer"}, {"whom", "off"}})) &&
           run(fd, act("off", "request", 1, {{"id", "g1"}, {"work", "grant"}, {"matter", "a field study"}})) && run(fd, act("app", "apply", 1, {{"id", "ap1"}, {"position", "applicant"}}));
      const auto app = ch::brief(fd, "app"), off = ch::brief(fd, "off", {}, "a grant awarded"), ext = ch::brief(fd, "ext");
      const std::string flags = std::string(aide.reaches == "anthropic" ? "" : "reaches ") + (verdict(aide, "deliverables", h::Aim::consult) == 1 ? "" : "deliverables ") +
                                (budget_quota ? "" : "quota ") + (fd.holds("app", "applicant") ? "" : "applicant ") + (verdict(app, "grant reports", h::Aim::deposit, true) == 1 ? "" : "reports ") +
                                (verdict(off, "endowment", h::Aim::use) == 1 ? "" : "endowment ") + (verdict(ext, "deed", h::Aim::consult) == 1 ? "" : "deed ");
      s.require("the organisation: staff of Anthropic admitted by the administration under the legal person's mandate read its deliverables (reaching anthropic) and "
                "draw on its budget within a quota of 20; the foundation: an applicant admitted while a grant is open may deposit its report, an officer may draw on "
                "the endowment for a grant awarded, and anyone may read the deed",
                ok && aide.reaches == "anthropic" && verdict(aide, "deliverables", h::Aim::consult) == 1 && budget_quota && fd.holds("app", "applicant") &&
                    verdict(app, "grant reports", h::Aim::deposit, true) == 1 && verdict(off, "endowment", h::Aim::use) == 1 && verdict(ext, "deed", h::Aim::consult) == 1,
                steps + flags);
    }
  }
  return s.done();
}
