// Checks of allocation/verdicts: a round judged against its alternatives under declared principles.
//
// Claims of 10 and 100 on 60: equal awards gives (10, 50), equal losses (0, 60), proportional (5.45, 54.55), the Talmud rule and
// adjusted proportional (5, 55), Piniles (7.5, 52.5). Under leximin and the headcount below 54.8:
// - the Talmud rule and adjusted proportional are one option with two names;
// - equal losses is dominated (by the Talmud rule: one below 54.8 each, a better least); proportional and Piniles by equal
//   awards (two below each, a worse least);
// - the undominated are equal awards (best under leximin) and the Talmud rule (best under the headcount), and the comparison of
//   the two turns on which principle is given precedence, which the pivot says;
// - what is at stake for the small claimant: from 5 to 10 among the undominated.
// Equal claims make every rule agree: one option. Totals never tell options apart when the whole sum is given. Where a round
// leaves part of its sum, one order of its stages can dominate the other; an unequal outcome with more and an equal one with
// less exchange places once as aversion rises.

#include <allocation/verdicts/verdicts.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace allocation;
using nomos::welfare::Kind;
using nomos::welfare::Principle;

static rounds::Entity entity(const std::string& id, std::map<std::string, double> at) {
  rounds::Entity e;
  e.id = e.name = id;
  e.at = std::move(at);
  return e;
}

static bool named(const verdicts::Option& o, const std::string& part) {
  for (const auto& n : o.names)
    if (n.find(part) != std::string::npos) return true;
  return false;
}

static std::size_t option_named(const verdicts::Judgement& J, const std::string& part) {
  for (std::size_t k = 0; k < J.options.size(); k++)
    if (named(J.options[k], part)) return k;
  return J.options.size();
}

int main() {
  calx::verification::Suite s;

  rounds::Round r;
  r.amount = 60;
  r.entities = {entity("small", {{"claim", 10}, {"population", 1}}), entity("large", {{"claim", 100}, {"population", 1}})};
  rounds::Stage st;
  st.kind = "claims";
  st.principle = "claims";
  st.attribute = "claim";
  st.rule = "equal_losses";
  r.stages = {st};

  const std::vector<Principle> principles{{"leximin", Kind::leximin, 0, 1, {}}, {"headcount below 54.8", Kind::shortfall, 54.8, 0, {}}, {"total", Kind::total, 0, 1, {}}};
  verdicts::Variation v;
  v.orders = false;
  const auto J = verdicts::judge(r, {"", "population"}, principles, v);

  const std::size_t talmud = option_named(J, "by talmud"), ap = option_named(J, "by adjusted_proportional"), cea = option_named(J, "by equal_awards");
  s.require("the Talmud rule and adjusted proportional give one result, kept as one option with both names", talmud < J.options.size() && talmud == ap);
  s.require("five distinct options from six rules and the declared round", J.options.size() == 5);
  s.require("the declared round (equal losses: 0 and 60) is dominated", !J.declared_undominated && std::find(J.dominating.begin(), J.dominating.end(), talmud) != J.dominating.end());
  const auto& M = J.comparison.maximal;
  s.require("the undominated are equal awards and the Talmud rule", M.size() == 2 && std::find(M.begin(), M.end(), cea) != M.end() && std::find(M.begin(), M.end(), talmud) != M.end());
  s.require("equal awards is best under leximin, the Talmud rule under the headcount",
            J.comparison.best_under[0] == std::vector<std::size_t>{cea} && std::find(J.comparison.best_under[1].begin(), J.comparison.best_under[1].end(), talmud) != J.comparison.best_under[1].end());
  {
    const std::string p = nomos::welfare::pivot(J.comparison, cea, talmud);
    s.require("and their comparison turns on which principle is given precedence", p.find("leximin") != std::string::npos && p.find("headcount") != std::string::npos && p.find("turns on") != std::string::npos);
  }
  bool totals_equal = true;
  for (std::size_t k = 1; k < J.options.size(); k++) totals_equal = totals_equal && std::abs(J.comparison.values[k][2] - J.comparison.values[0][2]) < 1e-9;
  s.require("the total never tells apart options that give the whole sum", totals_equal);
  s.require("for the small claimant, from 5 to 10 is at stake among the undominated, equal awards giving most",
            std::abs(J.stakes[0].least - 5) < 1e-9 && std::abs(J.stakes[0].most - 10) < 1e-9 && J.stakes[0].best == cea && std::abs(J.stakes[0].declared) < 1e-9);

  {
    auto q = r;
    q.stages[0].rule = "talmud";
    const auto K = verdicts::judge(q, {"", "population"}, principles, v);
    s.require("declared by the Talmud rule, the round is undominated, with a pivot against equal awards", K.declared_undominated && K.pivots.size() == 1 && K.pivots[0].second.find("turns on") != std::string::npos);
  }
  {
    auto q = r;
    q.entities[0].at["claim"] = 50;
    q.entities[1].at["claim"] = 50;
    const auto K = verdicts::judge(q, {"", "population"}, principles, v);
    s.require("with equal claims every rule agrees: one option, seven names", K.options.size() == 1 && K.options[0].names.size() == 7);
  }

  {
    // A round that gives part of its sum: claims met first (CEA), then a third of what is left by population; or the other order.
    rounds::Round q;
    q.amount = 100;
    q.entities = {entity("a", {{"claim", 30}, {"population", 1}, {"level", 5}}), entity("b", {{"claim", 0}, {"population", 1}, {"level", 40}})};
    rounds::Stage c;
    c.kind = "claims";
    c.principle = "need";
    c.attribute = "claim";
    c.rule = "equal_awards";
    rounds::Stage p;
    p.kind = "proportional";
    p.principle = "equality";
    p.attribute = "population";
    p.share = 0.5;
    q.stages = {c, p};
    verdicts::Variation w;
    w.rules = {};
    w.orders = true;
    const std::vector<Principle> pr{{"prioritarian", Kind::prioritarian, 1, 1, {}}, {"total", Kind::total, 0, 1, {}}};
    const auto K = verdicts::judge(q, {"level", "population"}, pr, w, {{0, 0, 20, 400}});
    // Claims first: a gets 30, then half of the 70 left, 17.5 each: (5 + 47.5, 40 + 17.5) = (52.5, 57.5), 35 left.
    // Proportional first: 25 each, then a's claim less 25 is 5: (5 + 30, 40 + 25) = (35, 65), 45 left.
    s.require("the two orders give (52.5, 57.5) and (35, 65)", K.options.size() == 2 && std::abs(K.options[0].outcome.value[0] - 52.5) < 1e-9 &&
                                                               std::abs(K.options[0].outcome.value[1] - 57.5) < 1e-9 && std::abs(K.options[1].outcome.value[0] - 35) < 1e-9);
    s.require("the first dominates the second under both principles", K.declared_undominated && K.comparison.maximal.size() == 1);
    // A flip needs options that disagree: compare a high-total unequal and a low-total equal outcome directly.
    nomos::welfare::Outcome hi{"unequal", {10, 100}, {}}, lo{"equal", {30, 30}, {}};
    const auto at = nomos::welfare::flips(pr[0], hi, lo, 0, 20, 400);
    bool sides = at.size() == 1;
    if (sides) {
      Principle below = pr[0], above = pr[0];
      below.parameter = at[0] * 0.9, above.parameter = at[0] * 1.1;
      sides = nomos::welfare::compare(below, hi, lo) > 0 && nomos::welfare::compare(above, hi, lo) < 0;
    }
    s.require("an unequal option with more and an equal one with less exchange places once as aversion rises, checked on either side", sides);
  }

  {
    auto q = r;
    q.entities.push_back(entity("empty", {{"claim", 5}, {"population", 0}}));
    const auto K = verdicts::judge(q, {"", "population"}, principles, v);
    s.require("an entity with nobody counted is left out of the outcomes read per head, and said to be",
              K.options[0].outcome.value.size() == 2 && std::any_of(K.said.begin(), K.said.end(), [](const std::string& x) { return x.find("left out") != std::string::npos; }));
  }
  {
    bool refused = false;
    try {
      verdicts::judge(r, {"", "population"}, {}, v);
    } catch (const rounds::Refusal&) {
      refused = true;
    }
    s.require("a judgement with no principle declared is refused", refused);
  }

  return s.done();
}
