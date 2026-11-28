// Checks of trajectories, every value worked by hand in the comments:
//   - a fact about conduct recognised under the base's standard of authorship (grade B, attested by another origin): attested by the
//     subject's own report, established by another origin's attestation, its three criteria stated (what it emerged from, since when, what
//     rests on it), a grade the traces do not bear out read as C, and disputed once another origin of grade A says otherwise; one origin's
//     two attestations counted once;
//   - the weight of observations by tekmerion's reports that share an origin: three reports of one origin are worth its channel's ratio, 9,
//     where counted apart they would be 729;
//   - contribution on a constructed graph of provenance: each party's activities beneath a section by role, its exact share under declared
//     weights of the roles, an activity off the section's lineage left out, and what a faulty review taints;
//   - measures of power on a constructed chronicle of the organisation preset: tenure by office and its concentration by origin, the longest
//     holding of an office against a bound, acts of force by origin, the authorship of amendments, rulings by origin, petitions by origin,
//     each with its mechanism's digest and the head it read; and voting power under a decision's own rule (Banzhaf and Shapley–Shubik).
//   - value readings on a constructed chronicle and journal: a share of decisions (n_a + 1)/(n_a + n_b + 2) with an act decided under a
//     reading left out (H7), conduct by category by outcome, a second reader obtaining the same digest and an altered entry named.

#include <communities/trajectories/trajectories.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using morphe::arithmetic::Rational;
using thesmos::acts::Act;
using thesmos::chronicle::State;
using thesmos::kernel::Kind;
using thesmos::kernel::Parties;
namespace ch = thesmos::chronicle;
namespace tr = communities::trajectories;

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
bool near(double a, double b) { return std::abs(a - b) <= 1e-9 * std::max(1.0, std::abs(b)); }
thesmos::constitutions::Constitution preset(const std::string& name) {
  return thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/" + name + ".json"));
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // The organisation: the firm (its legal person), two board members, an administration of persons, and outsiders.
  Parties ps;
  ps.add({"firm", Kind::organisation});
  for (const char* p : {"chair", "chair2", "admin", "ada", "bo", "x", "y"}) ps.add({p, Kind::person});
  const auto org = preset("organisation");
  tr::Record rec{ps, org, {}};
  State st(ps, org);
  std::string why;
  auto run = [&](const Act& a) {
    const auto o = ch::step(st, a);
    if (o.admitted) rec.acts.push_back(a);
    else if (why.empty()) why = a.author + " " + a.kind + ": " + o.reason;
    return o.admitted;
  };
  bool all;
  {
    Act f = act("firm", "found", 0);
    f.holders = {{"firm", "legal person"}, {"chair", "board"}, {"admin", "administration"}};
    all = run(f);                                                                                    // declarative: firm
  }
  all = all && run(act("chair", "admit", 2, {{"position", "administration"}, {"whom", "ada"}}));      // declarative: chair
  all = all && run(act("admin", "admit", 3, {{"position", "staff"}, {"whom", "ada"}}));               // declarative: admin
  all = all && run(act("chair", "assign", 4, {{"position", "administration"}, {"whom", "admin"}}));   // declarative: chair; admin's office ends
  all = all && run(act("ada", "deposit", 5, {{"holding", "x1"}, {"within", "deliverables"}, {"kind", "document"},
                                                {"digest", archeion::fixity::sha256_hex("a section ada wrote")}}));  // declarative: ada
  const std::string deposit_seal = st.journal.entries.back().seal;
  all = all && run(act("chair", "admit", 6, {{"position", "administration"}, {"whom", "bo"}}));       // declarative: chair

  // ---- 1. A fact about conduct recognised: the authorship of x1 by ada ----
  all = all && run(act("ada", "report", 6, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x1"}, {"grade", "B"}, {"cites", deposit_seal}}));
  const auto standard = tr::standard_of(st, "authorship");
  const auto self_only = tr::recognise(st, "authorship", "ada", *standard);
  all = all && run(act("bo", "attest", 7, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x1"}, {"grade", "B"}, {"cites", deposit_seal}})) &&
        run(act("chair", "report", 7, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x1"}, {"grade", "C"}})) &&
        run(act("bo", "attest", 8, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x1"}, {"grade", "B"}, {"cites", deposit_seal}})) &&
        run(act("chair", "report", 8, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x2"}, {"grade", "B"}, {"cites", std::string(64, 'a')}})) &&
        run(act("chair", "report", 8, {{"matter", "the section's place in the plan"}, {"rests on", "authorship of ada"}}));  // no observation: it rests on the fact
  const auto established = tr::recognise(st, "authorship", "ada", *standard);
  s.require("the base's standard of authorship is in force (grade B, attested by another origin); with ada's own report alone the fact is attested",
            all && standard && standard->grade == "B" && standard->another_origin && self_only.standing == "attested", why);
  s.require("established by bo's attestation of grade B, citing the deposit's seal: it emerged from five observations under the standard, persists since turn 7, and "
            "constrains the chair's report that rests on it; bo's two attestations are one origin; the chair's grade B citing a trace the record does not hold is read "
            "as C and does not dispute it",
            established.standing == "established" && established.values == std::vector<std::string>{"x1"} && established.from.size() == 5 && established.since == 7 &&
                established.reports == 2 && established.origins == 1 && established.from[4].traced == "C" && established.from[4].grade == "B" &&
                established.constrains == std::vector<std::string>{"report by chair at turn 8"},
            established.standing + ", " + std::to_string(established.from.size()) + " observations, since " + std::to_string(established.since) + ", " +
                std::to_string(established.reports) + " reports of " + std::to_string(established.origins) + " origins, " + std::to_string(established.constrains.size()) +
                " acts resting on it" + (established.from.size() > 4 ? ", the fifth traced " + established.from[4].traced : std::string()));
  all = run(act("x", "say", 9));
  const std::string say_seal = st.journal.entries.back().seal;
  all = all && run(act("chair", "report", 9, {{"fact", "authorship"}, {"subject", "ada"}, {"value", "x2"}, {"grade", "A"}, {"cites", say_seal}}));
  const auto disputed = tr::recognise(st, "authorship", "ada", *standard);
  s.require("disputed once another origin (the chair) with a grade A record says x2: the standing no longer established, and so no turn since which it stands",
            all && disputed.standing == "disputed" && disputed.since == -1 && disputed.values == std::vector<std::string>{"x1", "x2"}, why);

  // ---- 2. The weight of observations that share an origin ----
  {
    std::vector<tr::Observation> obs;
    for (int k = 0; k < 3; k++) obs.push_back({"bo/" + std::to_string(k), "bo", "attest", "authorship", "ada", "x1", "B", "B", {}, 7, 0});
    const std::map<std::string, tekmerion::evidence::Channel> grades = {{"A", {0.99, 0.01}}, {"B", {0.9, 0.1}}};
    const auto one = tr::weigh(obs, "x1", grades);
    obs.push_back({"cy", "cy", "attest", "authorship", "ada", "x1", "B", "B", {}, 7, 0});
    const auto two = tr::weigh(obs, "x1", grades);
    // One origin through (0.9, 0.1), relays perfect: P(reports | E) = 0.9, P(reports | ¬E) = 0.1, ratio 9; apart, 9³ = 729, overstated 81.
    // A second origin: 9 · 9 = 81, apart 9⁴ = 6561.
    s.require("three reports of one origin through a channel (0.9, 0.1) are worth its ratio, 9, where counted apart they would be 729 (overstated 81 times); a second "
              "origin makes 81",
              near(one.ratio, 9) && near(one.as_if_independent, 729) && near(one.overstatement, 81) && one.origins == 1 && one.reports == 3 && near(two.ratio, 81) &&
                  near(two.as_if_independent, 6561) && two.origins == 2);
  }

  // ---- 3. Contribution over the graph of provenance ----
  {
    namespace pv = archeion::provenance;
    Parties gp;
    for (const char* p : {"ada", "bo", "cy", "dee"}) gp.add({p, Kind::person});
    gp.add({"ada/bot", Kind::operative, "ada", "anthropic"});
    pv::Graph g;
    for (const char* e : {"data", "draft", "review", "section", "elsewhere"}) g.entities.push_back({e, {}});
    g.activities = {{"collect", "maintaining", "cy", "2026-11-28T00:00:00Z", {}, {"data"}, {}},
                    {"write", "writing", "ada/bot", "2026-11-28T01:00:00Z", {"data"}, {"draft"}, {}},
                    {"read", "reviewing", "bo", "2026-11-28T02:00:00Z", {"draft"}, {"review"}, {}},
                    {"integrate", "integrating", "ada", "2026-11-28T03:00:00Z", {"draft", "review"}, {"section"}, {}},
                    {"other", "writing", "dee", "2026-11-28T04:00:00Z", {}, {"elsewhere"}, {}}};
    const auto cs = tr::contribution(g, "section", gp, {{"writing", Rational(3)}, {"reviewing", Rational(1)}, {"integrating", Rational(1)}, {"maintaining", Rational(1)}});
    std::map<std::string, tr::Contribution> by;
    for (const auto& c : cs) by[c.party] = c;
    // Beneath the section: collect (cy, 1), write (ada through her bot, 3), read (bo, 1), integrate (ada, 1); total 6: ada 4/6, bo 1/6, cy 1/6.
    s.require("the contribution to a section: ada's writing through her agent and her integrating, bo's reviewing, cy's maintaining of the data; under weights 3, 1, 1, 1 "
              "their shares are 2/3, 1/6 and 1/6; dee's writing elsewhere is no part of it",
              cs.size() == 3 && by["ada"].share == Rational(2, 3) && by["bo"].share == Rational(1, 6) && by["cy"].share == Rational(1, 6) &&
                  by["ada"].by_role.at("writing") == std::vector<std::string>{"write"} && by["ada"].by_role.at("integrating") == std::vector<std::string>{"integrate"} &&
                  by["bo"].by_role.at("reviewing") == std::vector<std::string>{"read"} && !by.count("dee"));
    const auto bad = tr::tainted(g, "section", gp, {"review"});
    s.require("a review found faulty taints bo's review and ada's section, and neither cy's data nor ada's draft",
              bad.size() == 2 && bad.at("bo") == std::set<std::string>{"review"} && bad.at("ada") == std::set<std::string>{"section"});
  }

  // ---- 4. Measures of power on the organisation's chronicle ----
  // Two questions ruled, a petition, an amendment by each board member, and the last act at turn 14.
  all = run(act("x", "join", 9, {{"position", "visitor"}})) &&                                                        // declarative: x
        run(act("x", "question", 9, {{"id", "q1"}, {"about", "norm"}, {"subject", "deliverables"}})) &&                 // directive
        run(act("ada", "answer", 9, {{"id", "q1"}, {"outcome", "ruled"}, {"reading", "members read them"}})) &&          // declarative: ada
        run(act("y", "question", 10, {{"id", "q2"}, {"about", "rule"}, {"subject", "staff"}})) &&                        // directive
        run(act("bo", "answer", 10, {{"id", "q2"}, {"outcome", "ruled"}, {"reading", "staff are assigned"}})) &&         // declarative: bo
        run(act("x", "petition", 10, {{"id", "p1"}, {"kind", "tool"}})) &&                                              // directive
        run(act("firm", "admit", 10, {{"position", "board"}, {"whom", "chair2"}})) &&                                    // declarative: firm
        run(act("chair", "propose", 11, {{"id", "a1"}, {"decision", "amend operational"}, {"change.receiving", "14"}})) &&
        run(act("chair", "vote", 11, {{"id", "a1"}, {"value", "yes"}})) &&                                              // declarative: chair
        run(act("chair2", "vote", 11, {{"id", "a1"}, {"value", "yes"}})) &&                                             // declarative: chair2
        run(act("chair", "amend", 12, {{"id", "a1"}})) &&                                                               // declarative: chair
        run(act("chair2", "propose", 12, {{"id", "a2"}, {"decision", "amend operational"}, {"change.receiving", "15"}})) &&
        run(act("chair", "vote", 12, {{"id", "a2"}, {"value", "yes"}})) &&                                              // declarative: chair
        run(act("chair2", "vote", 12, {{"id", "a2"}, {"value", "yes"}})) &&                                             // declarative: chair2
        run(act("chair2", "amend", 13, {{"id", "a2"}})) &&                                                              // declarative: chair2
        run(act("chair", "say", 14));                                                                                   // expressive
  s.require("the organisation's chronicle runs as constructed", all, why);
  // The administration: admin [0, 4), ada [2, 14), bo [6, 14): 4, 12 and 8 turns of 24; shares 1/6, 1/2, 1/3; Σ share² = 1/36 + 1/4 + 1/9 = 7/18.
  const auto ten = tr::measure({"tenure by office", {{"position", "administration"}}}, rec);
  s.require("tenure by office: the administration held 4 turns by admin, 12 by ada and 8 by bo, shares 1/6, 1/2 and 1/3, the Herfindahl–Hirschman index 7/18; "
            "the reading cites its mechanism's digest and the head it read",
            ten.counts.at("admin") == 4 && ten.counts.at("ada") == 12 && ten.counts.at("bo") == 8 && ten.shares.at("ada") == Rational(1, 2) && ten.shares.at("bo") == Rational(1, 3) &&
                near(ten.herfindahl, 7.0 / 18) && ten.mechanism_digest == tr::digest({"tenure by office", {{"position", "administration"}}}) &&
                ten.head == archeion::fixity::hex(thesmos::crossings::head(st.community, st).root) && ten.size == st.journal.entries.size() && ten.refused.empty());
  const auto intervals = tr::tenure(rec);
  bool admin_closed = false, ada_open = false;
  for (const auto& i : intervals) {
    admin_closed = admin_closed || (i.party == "admin" && i.position == "administration" && i.from == 0 && i.to == 4 && !i.open);
    ada_open = ada_open || (i.party == "ada" && i.position == "administration" && i.from == 2 && i.to == 14 && i.open);
  }
  s.require("the intervals read by replaying the record: admin's office closed at turn 4 by the board's act, ada's open at the last act's turn", admin_closed && ada_open);
  const auto longest = tr::measure({"long offices", {{"position", "administration"}, {"bound", "10"}}}, rec);
  s.require("long offices with a bound of 10 turns: ada's 12 turns are noted, bo's 8 and admin's 4 are not",
            longest.notes == std::vector<std::string>{"ada has held administration for 12 turns together, beyond 10"} && longest.counts.at("bo") == 8);
  // Declarative acts admitted: the chair 6 (two admissions, the end of an office, two votes, an amendment), chair2 3 (two votes, an
  // amendment), the firm 2 (the founding, chair2's admission), ada 2 (the deposit, a ruling), admin 1 (ada's staff), bo 1 (a ruling), x 1
  // (joining as a visitor): 16 in all.
  const auto force = tr::measure({"acts of force by origin", {}}, rec);
  s.require("acts of force by origin: of 16 declarative acts the chair made 6, chair2 3, the firm 2, ada 2, and admin, bo and x one each",
            force.counts.at("chair") == 6 && force.counts.at("chair2") == 3 && force.counts.at("firm") == 2 && force.counts.at("ada") == 2 && force.counts.at("bo") == 1 &&
                force.counts.at("admin") == 1 && force.counts.at("x") == 1 && force.counts.size() == 7 && force.shares.at("chair") == Rational(3, 8),
            std::to_string(force.counts.size()));
  const auto amendments = tr::measure({"authorship of amendments", {}}, rec), rulings = tr::measure({"rulings by origin", {}}, rec),
             petitions = tr::measure({"petitions by origin", {}}, rec);
  s.require("the authorship of amendments one each by the chair and chair2 (shares 1/2); rulings one each by ada and bo; one petition, by x, still open",
            amendments.counts == std::map<std::string, long long>{{"chair", 1}, {"chair2", 1}} && amendments.shares.at("chair") == Rational(1, 2) &&
                rulings.counts == std::map<std::string, long long>{{"ada", 1}, {"bo", 1}} && petitions.counts == std::map<std::string, long long>{{"x", 1}} && petitions.notes.empty());
  const auto unknown = tr::measure({"charisma", {}}, rec);
  s.require("a mechanism this module does not declare is refused, nothing measured", !unknown.refused.empty() && unknown.counts.empty());

  // ---- 5. Voting power under a decision's own rule ----
  {
    auto doc = symploke::jsonld::parse(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/treaty-organisation.json"));
    doc.get("aggregation")->set("weighted test", symploke::jsonld::parse(R"({"body": "assembly", "rule": "at least 2/3", "count by": "weight", "weights": {"a": 2, "b": 1, "c": 1}, "tempo": "deliberative"})"));
    const auto c = thesmos::constitutions::read(symploke::jsonld::dump(doc));
    Parties vp;
    vp.add({"convener", Kind::person});
    for (const char* m : {"a", "b", "c", "commons", "guild", "cooperative", "polity"}) vp.add({m, Kind::community});
    tr::Record v{vp, c, {}};
    const auto weighted = tr::measure({"voting power", {{"decision", "weighted test"}}}, v);
    // W = 4, quota 8/3: winning {a, b}, {a, c}, {a, b, c}; swings a 3, b 1, c 1: Banzhaf 3/5, 1/5, 1/5; Shapley–Shubik 4/6, 1/6, 1/6.
    s.require("voting power under at least 2/3 of weights 2, 1, 1: Banzhaf 3/5, 1/5, 1/5 and Shapley–Shubik 2/3, 1/6, 1/6",
              near(weighted.banzhaf.at("a"), 0.6) && near(weighted.banzhaf.at("b"), 0.2) && near(weighted.shapley_shubik.at("a"), 2.0 / 3) && near(weighted.shapley_shubik.at("c"), 1.0 / 6),
              weighted.refused.empty() ? "" : weighted.refused.front());
    const auto budget = tr::measure({"voting power", {{"decision", "adopt budget"}}}, v);
    // The double majority of members 1/2 and weight 2/3 over weights 1, 2, 2, 3 (commons, guild, cooperative, polity), counted apart.
    const std::vector<std::string> ms = {"commons", "cooperative", "guild", "polity"};
    const std::vector<int> ws = {1, 2, 2, 3};
    std::map<std::string, int> swings;
    int total_swings = 0;
    auto wins = [&](unsigned mask) {
      int members = 0, weight = 0;
      for (int i = 0; i < 4; i++)
        if (mask >> i & 1u) members++, weight += ws[i];
      return 2 * members >= 4 && 3 * weight >= 2 * 8;
    };
    for (unsigned mask = 0; mask < 16; mask++)
      for (int i = 0; i < 4; i++)
        if (!(mask >> i & 1u) && !wins(mask) && wins(mask | 1u << i)) swings[ms[i]]++, total_swings++;
    bool agree = true;
    for (const auto& m : ms) agree = agree && near(budget.banzhaf.at(m), static_cast<double>(swings[m]) / total_swings);
    s.require("voting power under the treaty organisation's double majority equals the Banzhaf index counted apart over the sixteen coalitions", agree && total_swings > 0,
              budget.refused.empty() ? "" : budget.refused.front());
  }

  // ---- 5. Value readings over both records (Definition 33, H7, Proposition 15) ----
  {
    State v(ps, org);
    ch::Conduct journal;
    auto episode = [](const std::string& party, const std::string& operation, long long turn) {
      thesmos::behaviour::Episode e;
      e.party = party, e.operation = operation, e.kind = "request", e.turn = turn, e.features["role"] = "test";
      return e;
    };
    Act f = act("firm", "found", 0);
    f.holders = {{"firm", "legal person"}, {"chair", "board"}, {"admin", "administration"}};
    bool made = ch::constitute(v, journal, episode("firm", "found", 0), f).admitted;                                                          // place 1
    made = made && ch::constitute(v, journal, episode("chair", "admit", 2), act("chair", "admit", 2, {{"position", "administration"}, {"whom", "ada"}})).admitted;  // 2
    made = made && ch::constitute(v, journal, episode("admin", "admit", 3), act("admin", "admit", 3, {{"position", "staff"}, {"whom", "ada"}})).admitted;          // 3
    ch::observe(journal, episode("ada", "reading", 3), "");                                                                                   // no act
    made = made && !ch::constitute(v, journal, episode("y", "admit", 4), act("y", "admit", 4, {{"position", "administration"}, {"whom", "y"}})).admitted;  // refused
    made = made && ch::constitute(v, journal, episode("chair", "admit", 5), act("chair", "admit", 5, {{"position", "administration"}, {"whom", "bo"}, {"under reading", "r1"}})).admitted;  // 4, marked
    const tr::Mechanism share{"share of decisions", {{"act", "admit"}, {"field", "position"}, {"kinds", "administration, staff"}}};
    const auto r = tr::read_values(share, v, &journal);
    // Unmarked admissions: administration 1 (ada), staff 1 (ada): (1 + 1)/(1 + 1 + 2) = 1/2; with bo's marked admission counted it would be (2 + 1)/(3 + 2) = 3/5.
    s.require("a share of decisions reads (n_a + 1)/(n_a + n_b + 2) over the acts not decided under a reading: 1/2, bo's admission under r1 left out (H7) at place 4",
              made && r.refused.empty() && r.result.at("share of administration") == "1/2" && r.result.at("n administration") == "1" && r.excluded == std::vector<std::size_t>{4} &&
                  r.join == "joined" && r.grade == "B" && r.size == 4 && r.journal_size == 12,
              made ? (r.refused.empty() ? r.result.at("share of administration") + ", join " + r.join + ", journal " + std::to_string(r.journal_size) : r.refused.front()) : "an act was not as expected");
    const auto c = tr::read_values({"conduct by category", {}}, v, &journal);
    // Five episodes: found, two admissions constituted; one reading with no act; y's refused; bo's admission left out with its act.
    s.require("conduct by category counts the journal's episodes by outcome: 3 constituted, 1 refused, 1 with no act, 1 left out with the act it constituted",
              c.refused.empty() && c.result.at("constituted") == "3" && c.result.at("refused") == "1" && c.result.at("none") == "1" && c.result.at("unread") == "0" && c.episodes_left_out == 1,
              c.refused.empty() ? c.result.at("constituted") + " constituted, " + c.result.at("unread") + " unread" : c.refused.front());
    const auto again = tr::read_values(share, State(v), &journal);
    ch::Conduct altered = journal;
    const auto at = altered.journal.entries[2].about.find("admit");
    if (at != std::string::npos) altered.journal.entries[2].about.replace(at, 5, "admix");
    const auto changed = tr::read_values(share, v, &altered);
    s.require("a second reader of the same records obtains the same digest; one whose journal has entry 3 altered obtains another, and the journal's verification "
              "names entry 3 (Proposition 15)",
              again.digest == r.digest && r.digest.size() == 64 && changed.digest != r.digest && changed.join.find("entry 3 does not seal") != std::string::npos, changed.join);
    s.require("an episode's canonical text reads back as the episode, and a text altered in its lengths reads as nothing",
              tr::episode_of(journal.journal.entries[0].about) == episode("firm", "found", 0) && !tr::episode_of("party:3:firm;"));
  }
  return s.done();
}
