// Checks of a body's decision rule as its instrument states it (rules.hpp).
//
// - A rule's declaration and a question's records, their faults each named.
// - Article 27(3) of the Charter: a non-procedural decision by nine affirmative votes including the permanent members' concurring
//   votes. An abstention by a permanent member read under the ICJ's reading (Namibia, 1971, §22) and under the words alone: the
//   authority's reading sets the other aside and the outcome is clear; with both readings of one kind it turns on the readings, and
//   the term is its pivot. A negative vote of a permanent member defeats it under every reading. The members decisive ex post.
// - Article 18(2) with rule 86 of the General Assembly's rules: a two-thirds majority of those present and voting, exactly at the
//   boundary; at least and more than told apart; abstentions counted under another reading.
// - Consensus as the WTO defines it (footnote 1 to Article IX:1): absence and a reservation do not block, an objection does; and the
//   fallback to a majority of the votes cast, on its own record.
// - A double majority with a blocking minority of at least four members, failing which the majority is deemed attained (the form of
//   Article 16(4) TEU), on invented weights.
// - Members not recorded: an outcome that turns on them is said to; one that does not is not; too many are refused.
// - The rule as a simple game: a weighted rule's power equal to nomos's weighted game; consensus gives every member a veto; the
//   Council's Shapley–Shubik index 421/2145 for a permanent member and 4/2145 for an elected one (Felsenthal and Machover 1998).
// - Monotonicity: on 400 random records under random quorums and majorities, a member who changes to yes never defeats a question
//   the record carried.
// - A recorded outcome the rule does not carry is found.
//
// Every member, weight and record here is invented; the rules' words are the instruments'.

#include <diplomacy/rules/rules.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace diplomacy::rules;

namespace {

std::vector<std::string> named(const std::string& prefix, int n) {
  std::vector<std::string> v;
  for (int k = 1; k <= n; k++) v.push_back(prefix + std::to_string(k));
  return v;
}

// Article 27(3): nine affirmative votes, including the concurring votes of the permanent members.
Rule council(bool authority = true) {
  Rule r;
  r.id = "council";
  r.title = "Article 27(3)";
  r.instrument = "Charter of the United Nations, Article 27(3)";
  r.text = "Decisions of the Security Council on all other matters shall be made by an affirmative vote of nine members including the concurring votes of the permanent members.";
  r.members = named("p", 5);
  for (const auto& e : named("e", 10)) r.members.push_back(e);
  Term concurring{"concurring vote", "whether a permanent member that abstains concurs", {}};
  concurring.readings.push_back({"an affirmative vote", {Act::yes}, tc::Source::analyst, 'B', "the words alone", "a concurring vote is a vote for", false});
  concurring.readings.push_back({"any vote but a negative one", {Act::yes, Act::abstain, Act::not_voting},
                                 authority ? tc::Source::authority : tc::Source::analyst, 'B',
                                 "International Court of Justice, Namibia Advisory Opinion (1971), §22",
                                 "a voluntary abstention by a permanent member does not bar the adoption of a resolution", true});
  r.terms = {concurring};
  Clause nine;
  nine.id = "nine";
  nine.text = "an affirmative vote of nine members";
  nine.count = 9;
  Clause permanent;
  permanent.kind = Clause::Kind::concurrence;
  permanent.id = "permanent";
  permanent.text = "including the concurring votes of the permanent members";
  permanent.members = named("p", 5);
  permanent.counted = {"concurring vote", {}};
  r.clause.kind = Clause::Kind::all_of;
  r.clause.parts = {nine, permanent};
  return r;
}

Question vote(const std::string& id, const std::map<std::string, Act>& acts) {
  Question q;
  q.id = id;
  q.records[""] = {Record::Procedure::vote, acts};
  return q;
}

std::map<std::string, Act> everyone(const Rule& r, Act a) {
  std::map<std::string, Act> m;
  for (const auto& x : r.members) m[x] = a;
  return m;
}

// Article 18(2), read through rule 86: a two-thirds majority of the members present and voting.
Rule assembly(bool strict = false) {
  Rule r;
  r.id = "assembly";
  r.title = "Article 18(2)";
  r.instrument = "Charter of the United Nations, Article 18(2); Rules of Procedure of the General Assembly, rule 86";
  r.members = named("m", 30);
  Term pv{"present and voting", "whether abstaining members are among those present and voting", {}};
  pv.readings.push_back({"members casting an affirmative or negative vote", present_and_voting(), tc::Source::authority, 'B',
                         "Rules of Procedure of the General Assembly, rule 86", "members which abstain from voting are considered as not voting", true});
  pv.readings.push_back({"members present, abstaining or not", {Act::yes, Act::no, Act::abstain}, tc::Source::analyst, 'B', "an analyst", "", false});
  r.terms = {pv};
  r.clause.id = "two-thirds";
  r.clause.text = "a two-thirds majority of the members present and voting";
  r.clause.base = {"present and voting", {}};
  r.clause.num = 2, r.clause.den = 3, r.clause.strict = strict;
  return r;
}

// Consensus as the WTO defines it, falling back to a majority of the votes cast (Marrakesh Agreement, Article IX:1).
Rule wto() {
  Rule r;
  r.id = "wto";
  r.title = "Article IX:1";
  r.members = named("m", 8);
  Term obj{"formal objection", "whether a statement made in joining blocks", {}};
  obj.readings.push_back({"a formal objection", {Act::objection}, tc::Source::authority, 'B', "Marrakesh Agreement, footnote 1 to Article IX:1", "", true});
  obj.readings.push_back({"an objection or a reservation", {Act::objection, Act::reservation}, tc::Source::analyst, 'C', "", "", false});
  r.terms = {obj};
  Clause consensus;
  consensus.kind = Clause::Kind::consensus;
  consensus.id = "consensus";
  consensus.stage = "consensus";
  consensus.counted = {"formal objection", {}};
  Clause majority;
  majority.id = "majority of the votes cast";
  majority.stage = "vote";
  majority.base = {{}, present_and_voting()};
  majority.num = 1, majority.den = 2, majority.strict = true;
  r.clause.kind = Clause::Kind::fallback;
  r.clause.parts = {consensus, majority};
  return r;
}

// A double majority with a blocking minority of at least four members, failing which it is deemed attained (Article 16(4) TEU's
// form), on invented members and populations.
Rule double_majority() {
  Rule r;
  r.id = "double";
  r.members = named("a", 8);
  r.weights["population"] = {{"a1", 30}, {"a2", 25}, {"a3", 20}, {"a4", 8}, {"a5", 6}, {"a6", 5}, {"a7", 4}, {"a8", 2}};
  Clause states;
  states.id = "55% of the members";
  states.num = 55, states.den = 100, states.strict = false;
  Clause people;
  people.id = "65% of the population";
  people.weights = "population";
  people.num = 65, people.den = 100, people.strict = false;
  Clause qualified;
  qualified.kind = Clause::Kind::all_of;
  qualified.parts = {states, people};
  Clause blocking;
  blocking.id = "a blocking minority of at least four members";
  blocking.counted = {{}, {Act::no, Act::abstain, Act::not_voting, Act::absent}};
  blocking.count = 4;
  blocking.against = true;
  r.clause.kind = Clause::Kind::any_of;
  r.clause.parts = {qualified, blocking};
  return r;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    s.require("the rules here are declared without fault", problems(council()).empty() && problems(assembly()).empty() && problems(wto()).empty() && problems(double_majority()).empty());
    Rule r = council();
    r.clause.parts[1].members.push_back("p9");
    r.clause.parts[0].counted.term = "affirmative";
    r.terms[0].readings[0].stated = true;
    Clause lonely;
    lonely.kind = Clause::Kind::fallback;
    lonely.parts = {Clause{}};
    r.clause.parts.push_back(lonely);
    const auto ps = problems(r);
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a rule's faults are named: a member not of the body, a term not declared, two readings stated, a fallback of one part",
              has("not a member") && has("not declared") && has("more than one reading") && has("two parts"), std::to_string(ps.size()) + " faults");
    Rule d = double_majority();
    d.weights["population"].erase("a8");
    s.require("a member with no weight is named", !problems(d).empty() && problems(d)[0].what.find("no weight") != std::string::npos);
    Question q;
    q.id = "q";
    q.records["consensus"] = {Record::Procedure::consensus, {{"m1", Act::no}}};
    q.records["elsewhere"] = {};
    const auto qs = problems(wto(), q);
    s.require("a question's faults are named: an act a consensus does not admit, a record the rule does not read", qs.size() == 2, std::to_string(qs.size()));
  }

  // ---- Article 27(3) ----
  {
    const Rule r = council();
    Outcome all = outcome(r, vote("all", everyone(r, Act::yes)));
    s.require("fifteen affirmative votes adopt a decision under Article 27(3)", all.answer == Answer::adopted, all.reason);
    auto acts = everyone(r, Act::no);
    for (const auto& m : {"p1", "p2", "p3", "p4", "e1", "e2", "e3", "e4", "e5"}) acts[m] = Act::yes;
    acts["p5"] = Act::abstain;
    const Outcome abst = outcome(r, vote("abstention", acts));
    s.require("with nine affirmative votes and a permanent member abstaining it is adopted, the ICJ's reading setting the words alone aside",
              abst.answer == Answer::adopted && !abst.construal.set_aside.empty(), abst.reason);
    const Outcome literal = outcome(council(false), vote("abstention", acts));
    s.require("with both readings of one kind it turns on the readings", literal.answer == Answer::turns_on_readings, literal.reason);
    s.require("and the concurring vote is its pivot, the words alone defeating it", literal.construal.pivots.size() == 1 && literal.construal.pivots[0].flips == std::vector<std::size_t>{0});
    std::size_t decisive = abst.decisive.size();
    bool p5 = false;
    for (const auto& d : abst.decisive)
      if (d.member == "p5") p5 = d.flips == std::vector<Act>{Act::no, Act::absent};
    s.require("every one of the nine affirmative votes is decisive, and the abstaining permanent member by voting no or staying away", decisive == 10 && p5,
              std::to_string(decisive) + " decisive");
    s.require("the tally says which clause holds, in words", abst.tally_complete && abst.tally.parts.size() == 2 && abst.tally.parts[0].words.find("9 members yes") == 0, abst.tally.parts[0].words);
    auto veto = everyone(r, Act::yes);
    veto["p2"] = Act::no;
    const Outcome v = outcome(r, vote("veto", veto));
    s.require("a negative vote of a permanent member defeats fourteen affirmative votes under every reading", v.answer == Answer::not_adopted && v.tally.parts[1].words.find("p2 voted no") != std::string::npos, v.tally.parts[1].words);
    auto eight = everyone(r, Act::abstain);
    for (const auto& m : {"p1", "p2", "p3", "p4", "p5", "e1", "e2", "e3"}) eight[m] = Act::yes;
    s.require("eight affirmative votes do not carry it", outcome(r, vote("eight", eight)).answer == Answer::not_adopted);
    Question q = vote("recorded", veto);
    q.recorded = "adopted";
    s.require("a question recorded as adopted that the rule does not carry is found", !outcome(r, q).discrepancy.empty());
  }

  // ---- Article 18(2) and rule 86 ----
  {
    std::map<std::string, Act> acts;
    for (int k = 1; k <= 30; k++) acts["m" + std::to_string(k)] = k <= 14 ? Act::yes : k <= 21 ? Act::no : k <= 27 ? Act::abstain : Act::absent;
    const Outcome at_least = outcome(assembly(false), vote("boundary", acts));
    s.require("fourteen of twenty-one present and voting is exactly two-thirds, and at least two-thirds carries it", at_least.answer == Answer::adopted, at_least.reason);
    s.require("more than two-thirds does not", outcome(assembly(true), vote("boundary", acts)).answer == Answer::not_adopted);
    s.require("counting abstentions would defeat it, a reading rule 86 sets aside", at_least.construal.set_aside.size() == 1 && at_least.construal.holds[1] == 0,
              at_least.construal.set_aside.empty() ? "" : at_least.construal.set_aside[0]);
  }

  // ---- Consensus and its fallback ----
  {
    const Rule r = wto();
    Question q;
    q.id = "consensus";
    q.records["consensus"] = {Record::Procedure::consensus, {{"m1", Act::yes}, {"m2", Act::yes}, {"m3", Act::absent}, {"m4", Act::yes}, {"m5", Act::reservation}, {"m6", Act::yes}, {"m7", Act::yes}, {"m8", Act::yes}}};
    const Outcome o = outcome(r, q);
    s.require("absence and a reservation do not block a consensus under the WTO's definition", o.answer == Answer::adopted && o.tally.parts.size() == 1, o.reason);
    q.records["consensus"].acts["m2"] = Act::objection;
    const Outcome blocked = outcome(r, q);
    s.require("an objection blocks it, and with nothing recorded of a vote it is not adopted", blocked.answer == Answer::not_adopted && blocked.tally.words.find("nothing is recorded") != std::string::npos, blocked.tally.words);
    q.records["vote"] = {Record::Procedure::vote, {{"m1", Act::yes}, {"m2", Act::no}, {"m3", Act::absent}, {"m4", Act::yes}, {"m5", Act::yes}, {"m6", Act::yes}, {"m7", Act::abstain}, {"m8", Act::no}}};
    const Outcome fell = outcome(r, q);
    s.require("then a majority of the votes cast decides it on its own record", fell.answer == Answer::adopted && fell.tally.parts.size() == 2, fell.tally.words);
  }

  // ---- A double majority with a blocking minority ----
  {
    const Rule r = double_majority();
    std::map<std::string, Act> acts{{"a1", Act::no}, {"a2", Act::no}, {"a3", Act::no}, {"a4", Act::yes}, {"a5", Act::yes}, {"a6", Act::yes}, {"a7", Act::yes}, {"a8", Act::yes}};
    const Outcome o = outcome(r, vote("three", acts));
    s.require("three members holding 75% of the population cannot block: the majority is deemed attained", o.answer == Answer::adopted && !o.tally.parts[0].met && o.tally.parts[1].met, o.tally.words);
    acts["a8"] = Act::abstain;
    s.require("a fourth member, however small, makes the minority blocking", outcome(r, vote("four", acts)).answer == Answer::not_adopted);
  }

  // ---- Members not recorded ----
  {
    const Rule r = council();
    auto acts = everyone(r, Act::no);
    for (const auto& m : {"p1", "p2", "p3", "p4", "e1", "e2", "e3", "e4", "e5"}) acts[m] = Act::yes;
    acts.erase("p5");
    const Outcome o = outcome(r, vote("silent", acts));
    s.require("an outcome that turns on a permanent member not recorded is said to", o.answer == Answer::turns_on_unrecorded && o.unrecorded == std::vector<std::string>{"p5"}, o.reason);
    auto most = everyone(r, Act::yes);
    most.erase("e10");
    s.require("one that no act of the member not recorded could change is not", outcome(r, vote("most", most)).answer == Answer::adopted);
    auto few = everyone(r, Act::yes);
    for (const auto& m : {"e1", "e2", "e3", "e4", "e5", "e6", "e7"}) few.erase(m);
    s.require("seven members not recorded are too many to read every way, and are refused", outcome(r, vote("few", few)).answer == Answer::undecided);
  }

  // ---- The rule as a simple game ----
  {
    Rule w;
    w.id = "weighted";
    w.members = {"a", "b", "c", "d"};
    w.weights["votes"] = {{"a", 4}, {"b", 3}, {"c", 2}, {"d", 1}};
    w.clause.weights = "votes";
    w.clause.count = 6;
    const auto mine = nomos::games::power(game(w, {}));
    const auto theirs = nomos::games::power(nomos::games::WeightedGame{{4, 3, 2, 1}, 6});
    double gap = 0;
    for (std::size_t k = 0; k < 4; k++) gap = std::max({gap, std::fabs(mine.penrose[k] - theirs.penrose[k]), std::fabs(mine.shapley_shubik[k] - theirs.shapley_shubik[k])});
    s.require("a weighted rule's game has the power of nomos's weighted game", gap < 1e-15, std::to_string(gap));
    Rule pure = wto();
    pure.clause = Clause(pure.clause.parts[0]);
    const auto p = nomos::games::properties(game(pure, meaning_of(pure, stated(pure))));
    s.require("consensus gives every member a veto", p.vetoes.size() == 8);
    const Rule c = council();
    const auto pc = nomos::games::power(game(c, meaning_of(c, stated(c))));
    s.require("the Council's Shapley–Shubik index is 421/2145 for a permanent member and 4/2145 for an elected one",
              std::fabs(pc.shapley_shubik[0] - 421.0 / 2145) < 1e-12 && std::fabs(pc.shapley_shubik[14] - 4.0 / 2145) < 1e-12,
              std::to_string(pc.shapley_shubik[0]) + ", " + std::to_string(pc.shapley_shubik[14]));
  }

  // ---- Monotonicity ----
  {
    calx::random::Stream rng(20261119, 134, 1, 0);
    int bad = 0, carried = 0;
    const std::vector<Act> choices{Act::yes, Act::no, Act::abstain, Act::absent};
    for (int k = 0; k < 400; k++) {
      Rule r;
      r.id = "random";
      r.members = named("m", 10);
      Clause quorum;
      quorum.kind = Clause::Kind::quorum;
      quorum.counted = {{}, present()};
      quorum.num = 1 + static_cast<long long>(rng.below(9)), quorum.den = 10, quorum.strict = false;
      Clause majority;
      majority.base = {{}, present_and_voting()};
      majority.num = 1 + static_cast<long long>(rng.below(4)), majority.den = 5, majority.strict = rng.below(2) == 0;
      r.clause.kind = Clause::Kind::all_of;
      r.clause.parts = {quorum, majority};
      std::map<std::string, Act> acts;
      for (const auto& m : r.members) acts[m] = choices[rng.below(4)];
      Acts_by_stage a{{"", acts}};
      if (!evaluate(r, {}, a).met) continue;
      carried++;
      for (const auto& m : r.members) {
        if (acts[m] == Act::yes) continue;
        auto b = a;
        b[""][m] = Act::yes;
        if (!evaluate(r, {}, b).met) bad++;
      }
    }
    s.require("on random records under random quorums and majorities, a member who changes to yes never defeats a question the record carried", bad == 0 && carried > 50,
              std::to_string(carried) + " carried");
  }
  return s.done();
}
