// Checks of organisations, on the treaty organisation preset run by thesmos's fold: a budget adopted by a double majority of members and
// weight (refused with two members, adopted with a third); calls for contributions read from the record, one by a member's envoy refused;
// the calls of a period against the scale, following it and not; payments credited to the call they name, then in the order of the calls,
// what is left held as a credit for the next call; arrears at a turn; the members in arrears beyond the preceding two periods (the
// Charter's Article 19 as a declared mechanism); core against earmarked; staff by origin with the Herfindahl–Hirschman index; and a
// member's withdrawal, its contributions owed staying owed and a payment after it credited; and on random sequences of calls and payments the
// identity of Proposition 5 of the theory note. Every number is worked by hand in the comments.

#include <communities/organisations/organisations.hpp>

#include <calx/random/stream.hpp>
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
namespace og = communities::organisations;

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
const std::vector<std::string> kMembers = {"commons", "guild", "cooperative", "polity"};
}  // namespace

int main() {
  calx::verification::Suite s;
  Parties ps;
  for (const char* p : {"convener", "sg", "donor"}) ps.add({p, Kind::person});
  for (const auto& m : kMembers) {
    ps.add({m, Kind::community});
    ps.add({m + "/envoy", Kind::operative, m, "anthropic"});
  }
  ps.add({"polity/agent-1", Kind::operative, "polity", "openai"});
  ps.add({"polity/agent-2", Kind::operative, "polity", "openai"});
  ps.add({"commons/aide", Kind::operative, "commons", "anthropic"});
  State st(ps, thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/treaty-organisation.json")));
  std::string why;
  auto ok = [&](const Act& a) {
    const auto o = ch::step(st, a);
    if (!o.admitted && why.empty()) why = a.author + " " + a.kind + ": " + o.reason;
    return o.admitted;
  };
  auto envoy = [](const std::string& m, std::string kind, long long turn, std::map<std::string, std::string> fields = {}) {
    return act(m + "/envoy", std::move(kind), turn, std::move(fields), "m-" + m);
  };
  bool all;
  {
    Act f = act("convener", "found", 0);
    f.holders = {{"commons", "member"}, {"guild", "member"}, {"cooperative", "member"}, {"polity", "member"}, {"sg", "secretary-general"}};
    all = ok(f);
    for (const auto& m : kMembers) {
      Act g = act(m, "mandate", 1);
      thesmos::mandates::Mandate x;
      x.id = "m-" + m, x.principal = m, x.holder = m + "/envoy", x.from = 0, x.to = 100;
      x.kinds = {"join", "propose", "vote", "pay contribution", "deposit", "call for contributions"};
      g.grant = x;
      all = all && ok(g) && ok(envoy(m, "join", 1, {{"position", "envoy"}, {"sponsor", m}}));
    }
  }

  // ---- 1. A budget adopted by a double majority of members (1/2) and weight (2/3), the weights 1, 2, 2, 3 (W = 8) ----
  og::Declared d;
  all = all && ok(envoy("polity", "propose", 1, {{"id", "b1"}, {"decision", "adopt budget"}, {"change.budget", "100"}, {"change.period", "1"}})) &&
        ok(envoy("polity", "vote", 1, {{"id", "b1"}, {"value", "yes"}})) && ok(envoy("guild", "vote", 1, {{"id", "b1"}, {"value", "yes"}})) &&
        ok(envoy("commons", "vote", 1, {{"id", "b1"}, {"value", "no"}})) && ok(envoy("cooperative", "vote", 1, {{"id", "b1"}, {"value", "no"}}));
  const auto two = og::adopted_budget(st, d, 1);  // members 2/4 = 1/2, weight (3 + 2)/8 = 5/8 < 2/3: fails
  all = all && ok(envoy("cooperative", "vote", 1, {{"id", "b1"}, {"value", "yes"}}));
  const auto three = og::adopted_budget(st, d, 1);  // the cooperative turns to yes: members 3/4, weight 7/8: passes
  s.require("a budget of 100 for the first period, the commons and the cooperative against, is not adopted with the polity and the guild for it (members 1/2, "
            "weight 5/8 under 2/3), and is once the cooperative turns to it (3/4 and 7/8), the scale being the decision's weights",
            all && !two && three && *three == 100 && og::scale_of(st, "adopt budget").size() == 4, why);

  // ---- 2. Calls for contributions under the scale: quotas 100·(1, 2, 2, 3)/8 = 12.5, 25, 25, 37.5; calls 13, 25, 25, 37, due at turn 5 ----
  const std::map<std::string, long long> first = {{"commons", 13}, {"guild", 25}, {"cooperative", 25}, {"polity", 37}};
  for (const auto& [m, n] : first)
    all = all && ok(act("sg", "call for contributions", 2, {{"id", "c-" + m + "-1"}, {"member", m}, {"amount", std::to_string(n)}, {"period", "1"}, {"due", "5"}}));
  const auto by_envoy = ch::step(st, envoy("polity", "call for contributions", 2, {{"id", "c-x"}, {"member", "guild"}, {"amount", "9"}, {"period", "1"}, {"due", "5"}}));
  const auto cs = og::calls(st);
  s.require("the secretary-general's four calls are read from the record with their members, amounts, periods and due turns; a member's envoy may not call for "
            "contributions",
            all && cs.size() == 4 && cs[0].member == "commons" && cs[0].amount == 13 && cs[3].member == "polity" && cs[3].amount == 37 && cs[3].due == 5 && cs[3].period == 1 &&
                cs[3].by == "sg" && !by_envoy.admitted,
            why + " / " + by_envoy.reason);
  const auto one = og::apportionment(st, 1, *three, og::scale_of(st, "adopt budget"));
  bool exact = true;
  for (const auto& q : one.quotas)
    exact = exact && ((q.member == "commons" && q.quota == Rational(25, 2)) || (q.member == "guild" && q.quota == Rational(25)) ||
                      (q.member == "cooperative" && q.quota == Rational(25)) || (q.member == "polity" && q.quota == Rational(75, 2)));
  s.require("the first period's calls follow the scale: each within one unit of its exact quota (12.5, 25, 25, 37.5), the calls summing to the budget",
            one.follows && one.sums && exact && one.quotas.size() == 4);

  // ---- 3. Payments credited ----
  // commons pays 10 naming its call: owes 3. polity pays 50: its call of 37 met, 13 held as a credit. cooperative pays 20 at turn 8.
  all = ok(envoy("commons", "pay contribution", 3, {{"amount", "10"}, {"contribution", "c-commons-1"}})) && ok(envoy("polity", "pay contribution", 4, {{"amount", "50"}}));
  const auto at6 = og::accounts(st, d, 6);  // after the calls' due turn 5
  s.require("at turn 6: the commons owes 3 of its 13, overdue; the guild and the cooperative owe their 25, overdue; the polity owes nothing and holds a credit of 13",
            all && at6.at("commons").owed == 3 && at6.at("commons").arrears == 3 && at6.at("commons").overdue == std::vector<std::string>{"c-commons-1"} &&
                at6.at("guild").arrears == 25 && at6.at("cooperative").arrears == 25 && at6.at("polity").owed == 0 && at6.at("polity").credit == 13 &&
                at6.at("polity").paid == 50,
            why);
  // A second period: a budget of 80 (quotas 10, 20, 20, 30); calls 12, 20, 20, 28, due at turn 10; the polity's credit of 13 meets 13 of its 28.
  all = ok(envoy("polity", "propose", 6, {{"id", "b2"}, {"decision", "adopt budget"}, {"change.budget", "80"}, {"change.period", "2"}}));
  for (const char* m : {"polity", "guild", "cooperative"}) all = all && ok(envoy(m, "vote", 6, {{"id", "b2"}, {"value", "yes"}}));
  const std::map<std::string, long long> second = {{"commons", 12}, {"guild", 20}, {"cooperative", 20}, {"polity", 28}};
  for (const auto& [m, n] : second)
    all = all && ok(act("sg", "call for contributions", 7, {{"id", "c-" + m + "-2"}, {"member", m}, {"amount", std::to_string(n)}, {"period", "2"}, {"due", "10"}}));
  all = all && ok(envoy("cooperative", "pay contribution", 8, {{"amount", "20"}}));  // its first call: 25 − 20 leaves 5
  const auto two_periods = og::apportionment(st, 2, *og::adopted_budget(st, d, 2), og::scale_of(st, "adopt budget"));
  bool commons_out = false;
  for (const auto& q : two_periods.quotas) commons_out = commons_out || (q.member == "commons" && !q.within && q.quota == Rational(10));
  s.require("the second period's calls sum to its budget of 80 but do not follow the scale: the commons is called 12 against a quota of 10", all && two_periods.sums && !two_periods.follows && commons_out,
            why);
  const auto at11 = og::accounts(st, d, 11);
  // commons: 3 + 12 = 15 overdue; guild: 25 + 20 = 45; cooperative: 5 + 20 = 25; polity: 28 − 13 = 15.
  s.require("at turn 11 the arrears are those worked by hand: the commons 15, the guild 45, the cooperative 25, and the polity 15 after its credit met 13 of its second call",
            at11.at("commons").arrears == 15 && at11.at("guild").arrears == 45 && at11.at("cooperative").arrears == 25 && at11.at("polity").arrears == 15 &&
                at11.at("polity").credit == 0 && at11.at("polity").called == 65);
  // Called for periods 1 and 2: commons 25, guild 45, cooperative 45, polity 65; only the guild's arrears (45) reach that.
  s.require("in the third period, with k = 2 preceding periods (the Charter's Article 19 as a declared mechanism), only the guild is in arrears of all that was called "
            "from it: 45 of 45; the commons owes 15 of 25, the cooperative 25 of 45, the polity 15 of 65",
            og::in_arrears_beyond(st, d, 11, 3, 2) == std::vector<std::string>{"guild"});

  // ---- 4. Core against earmarked ----
  all = ok(envoy("commons", "deposit", 9, {{"holding", "earmarked"}, {"amount", "10"}, {"purpose", "a programme on water"}})) &&
        ok(act("donor", "deposit", 9, {{"holding", "earmarked"}, {"amount", "5"}, {"purpose", "a programme on water"}}));
  const auto fu = og::funding(st, d);
  // core: 10 + 50 + 20 = 80; earmarked 15; share 15/95 = 3/19.
  s.require("core against earmarked: 80 paid into the core budget, 15 earmarked for a programme on water by a member and a donor outside it, the earmarked share 3/19",
            all && fu.core == 80 && fu.earmarked == 15 && fu.earmarked_share == Rational(3, 19) && fu.by_purpose.at("a programme on water") == 15 &&
                fu.by.at("commons") == std::pair<long long, long long>{10, 10} && fu.by.at("donor").second == 5 && st.holdings.at("core budget").quantity == 80,
            why);

  // ---- 5. Staff by origin ----
  for (const char* h : {"polity/agent-1", "polity/agent-2", "commons/aide"}) all = all && ok(act("sg", "admit", 9, {{"position", "staff"}, {"whom", h}}));
  const auto sf = og::staffing(st, d);
  // holders: sg (origin sg), two of the polity, one of the commons: shares 1/4, 1/2, 1/4; Σ share² = 1/16 + 1/4 + 1/16 = 3/8.
  s.require("staff by origin: the secretary-general, two agents of the polity and one of the commons; shares 1/4, 1/2 and 1/4, the Herfindahl–Hirschman index 3/8, "
            "the polity the largest, two of the origins members",
            all && sf.shares.at("polity") == Rational(1, 2) && sf.shares.at("commons") == Rational(1, 4) && sf.shares.at("sg") == Rational(1, 4) &&
                std::abs(sf.herfindahl - 0.375) < 1e-12 && sf.largest == "polity" && sf.from_members == std::vector<std::string>{"commons", "polity"},
            why);

  // ---- 6. Proposition 5 of the theory note: owed less credit is called less paid, and one of the two is nought ----
  {
    calx::random::Stream r(20261127, 50, 0, 0);
    bool identity = true;
    long long steps = 0, credits = 0;
    for (int k = 0; k < 60 && identity; k++) {
      State t(ps, st.rules);
      Act f = act("convener", "found", 0);
      f.holders = {{"commons", "member"}, {"guild", "member"}, {"cooperative", "member"}, {"polity", "member"}, {"sg", "secretary-general"}};
      ch::step(t, f);
      for (const auto& m : kMembers) {
        Act g = act(m, "mandate", 0);
        thesmos::mandates::Mandate x;
        x.id = "m-" + m, x.principal = m, x.holder = m + "/envoy", x.from = 0, x.to = 1000, x.kinds = {"join", "pay contribution"};
        g.grant = x;
        ch::step(t, g);
        ch::step(t, envoy(m, "join", 0, {{"position", "envoy"}, {"sponsor", m}}));
      }
      long long turn = 1;
      for (int i = 0; i < 25 && identity; i++) {
        turn += static_cast<long long>(r.below(2));
        const std::string& m = kMembers[r.below(kMembers.size())];
        const long long amount = 1 + static_cast<long long>(r.below(20));
        if (r.below(2) == 0)
          ch::step(t, act("sg", "call for contributions", turn, {{"id", "r" + std::to_string(i)}, {"member", m}, {"amount", std::to_string(amount)}, {"period", "1"},
                                                                {"due", std::to_string(turn + 2)}}));
        else
          ch::step(t, envoy(m, "pay contribution", turn, {{"amount", std::to_string(amount)}}));
        for (const auto& [who, acc] : og::accounts(t, d, turn)) {
          identity = identity && acc.owed - acc.credit == acc.called - acc.paid && std::min(acc.owed, acc.credit) == 0 && acc.arrears <= acc.owed;
          credits += acc.credit > 0;
        }
        steps++;
      }
    }
    s.require("on 60 random sequences of calls and payments run through the fold (" + std::to_string(steps) + " steps, " + std::to_string(credits) +
                  " accounts in credit), every member owes its calls less its payments, less any credit, and never both owes and holds a credit (Proposition 5)",
              identity && credits > 0);
  }

  // ---- 7. Withdrawal ----
  all = ok(act("guild", "leave", 12, {{"position", "member"}})) && ok(envoy("guild", "pay contribution", 13, {{"amount", "20"}}));
  const auto ws = og::withdrawals(st, d, 14);
  s.require("the guild leaves at turn 12 with notice of four turns, owing its 45; its contributions stay owed, and a payment of 20 after its leaving is credited, leaving 25",
            all && ws.size() == 1 && ws[0].member == "guild" && ws[0].left == 12 && ws[0].effective == 16 && ws[0].owed_at_leaving == 45 && ws[0].owed_now == 25 &&
                !st.holds("guild", "member"),
            why);
  return s.done();
}
