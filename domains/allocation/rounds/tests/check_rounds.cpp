// Checks of allocation/rounds: an allocation as declared stages applied in turn to what remains.
//
// - A claims stage by the Talmud rule on the Talmud's estate of 200 gives (50, 75, 75).
// - A floor in plenty closes every shortfall exactly and leaves the rest to the next stage; in scarcity it divides what there is
//   among the shortfalls by its declared rule, and the next stage gets nothing.
// - Eligibility excludes, with the reason; priority classes are served in order; a claims stage counts what earlier stages gave;
//   banded points agree with nomos's; equalisation divides its pool exactly and holds no grant below zero; a stage takes only its
//   share of what remains.
// - Every sum is conserved and every amount is at least zero on random rounds of random stages.
// - Who was reached first: those the floor reached, most first, then the others, the excluded last.
// - What is not given is refused by name.

#include <allocation/rounds/rounds.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace allocation::rounds;

static bool close(double a, double b, double tol = 1e-9) { return std::abs(a - b) <= tol * std::max(1.0, std::abs(b)); }

static Entity entity(const std::string& id, std::map<std::string, double> at, const std::string& group = "") {
  Entity e;
  e.id = id;
  e.name = id;
  e.group = group;
  e.at = std::move(at);
  return e;
}

static Stage stage(const std::string& kind, const std::string& principle) {
  Stage s;
  s.kind = kind;
  s.principle = principle;
  s.source = {"the council", "2026-11-20", "declared for the checks"};
  return s;
}

int main() {
  calx::verification::Suite s;

  {
    Round r;
    r.amount = 200;
    r.unit = "shekels";
    r.entities = {entity("a", {{"claim", 100}}), entity("b", {{"claim", 200}}), entity("c", {{"claim", 300}})};
    Stage st = stage("claims", "the Talmud's division");
    st.attribute = "claim";
    st.rule = "talmud";
    r.stages = {st};
    const auto res = run(r);
    s.require("a claims stage by the Talmud rule on the estate of 200 gives (50, 75, 75)", close(res.amount[0], 50) && close(res.amount[1], 75) && close(res.amount[2], 75) && close(res.left, 0));
    r.stages[0].rule = "tal";
    r.stages[0].theta = 0.5;
    const auto t = run(r);
    s.require("and so does TAL at θ = ½", close(t.amount[0], 50) && close(t.amount[1], 75) && close(t.amount[2], 75));
  }

  // A floor, then the rest by population.
  auto floor_round = [](double amount) {
    Round r;
    r.amount = amount;
    r.unit = "£";
    r.entities = {entity("north", {{"level", 60}, {"population", 10}}, "towns"), entity("south", {{"level", 90}, {"population", 20}}, "towns"),
                  entity("east", {{"level", 120}, {"population", 30}}, "cities")};
    Stage f = stage("floor", "sufficiency");
    f.attribute = "level";
    f.bound = 100;
    f.rule = "proportional";
    Stage p = stage("proportional", "equality per head");
    p.attribute = "population";
    r.stages = {f, p};
    return r;
  };
  {
    // Shortfalls: north (100 − 60) × 10 = 400, south 10 × 20 = 200, east none: 600 in all.
    const auto res = run(floor_round(1200));
    s.require("a floor in plenty closes every shortfall: 400 and 200", close(res.by_stage[0][0], 400) && close(res.by_stage[0][1], 200) && close(res.by_stage[0][2], 0));
    s.require("and the rest (600) goes by population, 100, 200 and 300", close(res.by_stage[1][0], 100) && close(res.by_stage[1][1], 200) && close(res.by_stage[1][2], 300) && close(res.left, 0));
    const auto scarce = run(floor_round(300));
    s.require("a floor in scarcity divides what there is among the shortfalls by its rule: 200 and 100", close(scarce.by_stage[0][0], 200) && close(scarce.by_stage[0][1], 100));
    s.require("and the next stage gets nothing", close(scarce.by_stage[1][0] + scarce.by_stage[1][1] + scarce.by_stage[1][2], 0) && close(scarce.left, 0));
    auto r = floor_round(300);
    r.stages[0].rule = "equal_awards";
    const auto ea = run(r);
    s.require("by equal awards instead: 150 and 150 (both shortfalls exceed it)", close(ea.by_stage[0][0], 150) && close(ea.by_stage[0][1], 150));
    const auto order = order_reached(res);
    s.require("who first: north and south (the floor, the larger first), then east", order[0].entity == 0 && order[1].entity == 1 && order[2].entity == 2 && order[0].stage == 0 && order[2].stage == 1);
    bool reasons = res.reasons[0].size() == 2 && res.reasons[0][0].find("sufficiency") != std::string::npos && res.reasons[0][0].find("the council") != std::string::npos;
    s.require("each amount is given with its reasons: the stage, the principle and who declared it", reasons);
  }

  {
    auto r = floor_round(1200);
    Stage e = stage("eligibility", "residence");
    e.attribute = "population";
    e.bound = 15;
    r.stages.insert(r.stages.begin(), e);
    const auto res = run(r);
    s.require("eligibility excludes north (10 below 15) and says why", !res.excluded[0].empty() && res.excluded[0].find("population") != std::string::npos && close(res.amount[0], 0));
    s.require("the others share the whole sum", close(res.amount[1] + res.amount[2], 1200));
    const auto order = order_reached(res);
    s.require("the excluded come last in who was reached", order.back().entity == 0);
  }

  {
    Round r;
    r.amount = 100;
    r.entities = {entity("a", {{"urgency", 2}, {"claim", 50}}), entity("b", {{"urgency", 2}, {"claim", 30}}), entity("c", {{"urgency", 1}, {"claim", 100}})};
    Stage p = stage("priority", "urgency");
    p.attribute = "urgency";
    p.claim = "claim";
    p.rule = "proportional";
    r.stages = {p};
    const auto res = run(r);
    s.require("priority classes: the more urgent met in full (50, 30), the less urgent given what is left (20)", close(res.amount[0], 50) && close(res.amount[1], 30) && close(res.amount[2], 20));
    r.amount = 40;
    const auto scarce = run(r);
    s.require("when the first class cannot be met it is divided by the rule (25, 15) and the second gets nothing", close(scarce.amount[0], 25) && close(scarce.amount[1], 15) && close(scarce.amount[2], 0));
  }

  {
    auto r = floor_round(1200);
    Stage c = stage("claims", "claims less what was given");
    c.attribute = "claim";
    c.rule = "proportional";
    r.entities[0].at["claim"] = 500;
    r.entities[1].at["claim"] = 200;
    r.entities[2].at["claim"] = 0;
    r.stages = {r.stages[0], c};
    const auto res = run(r);
    s.require("a claims stage counts what earlier stages gave: north 400 then 100 more, south 200 and nothing more", close(res.amount[0], 500) && close(res.amount[1], 200) && close(res.left, 500));
  }

  {
    Round r;
    r.amount = 1000;
    r.entities = {entity("a", {{"need", 0.8}, {"people", 100}}), entity("b", {{"need", 0.3}, {"people", 400}}), entity("c", {{"need", 0.5}, {"people", 900}})};
    Stage p = stage("points", "need and size, banded");
    nomos::allocation::Indicator need{"need", {}, {0.25, 0.5, 0.75}, true, 2};
    nomos::allocation::Indicator people{"people", {}, {200, 500}, true, 1};
    p.indicators = {need, people};
    r.stages = {p};
    const auto res = run(r);
    nomos::allocation::Indicator n2 = need, p2 = people;
    n2.values = {0.8, 0.3, 0.5};
    p2.values = {100, 400, 900};
    const auto direct = nomos::allocation::banded_points(1000, {n2, p2});
    s.require("banded points agree with nomos's", close(res.amount[0], direct.award[0]) && close(res.amount[1], direct.award[1]) && close(res.amount[2], direct.award[2]));
  }

  {
    Round r;
    r.amount = 3000;
    r.entities = {entity("rich", {{"population", 100}, {"need", 900}, {"base", 50000}}), entity("middle", {{"population", 100}, {"need", 1000}, {"base", 30000}}),
                  entity("poor", {{"population", 100}, {"need", 1200}, {"base", 10000}})};
    Stage e = stage("equalisation", "needs against capacities");
    e.attribute = "need";
    e.bases = {"base"};
    e.rates = {0.02};
    r.stages = {e};
    const auto res = run(r);
    // Per head: 10 + (e − ē) + t (b̄ − b): rich 10 − 133.3 − 400 < 0, held at zero.
    s.require("equalisation divides its pool exactly", close(res.amount[0] + res.amount[1] + res.amount[2], 3000));
    s.require("and holds the grant that would fall below zero at zero, the poorest most", res.amount[0] >= 0 && res.amount[0] < 1e-9 && res.amount[2] > res.amount[1]);
  }

  {
    auto r = floor_round(1200);
    r.stages[1].share = 0.5;
    const auto res = run(r);
    s.require("a stage takes only its share of what remains: half of 600, 300 left", close(res.left, 300) && close(res.remaining_after[0], 600) && close(res.remaining_after[1], 300));
  }

  {
    auto r = floor_round(1200);
    r.entities[1].at.erase("level");
    bool refused = false;
    try {
      run(r);
    } catch (const Refusal& e) {
      refused = std::string(e.what()).find("level") != std::string::npos && std::string(e.what()).find("south") != std::string::npos;
    }
    s.require("an attribute not given is refused, naming it and whose it is", refused);
    auto q = floor_round(1200);
    q.stages[0].rule = "first come";
    bool rule = false;
    try {
      run(q);
    } catch (const Refusal&) {
      rule = true;
    }
    s.require("so is a rationing rule that is not known", rule);
    const auto read = attributes_read(floor_round(1));
    s.require("the attributes a round reads: level, population", read.size() == 2 && read[0] == "level" && read[1] == "population");
  }

  {
    calx::random::Stream rng(20261120, 136, 2, 0);
    bool conserved = true, nonnegative = true;
    const std::vector<std::string> rules{"proportional", "equal_awards", "equal_losses", "talmud", "piniles", "adjusted_proportional", "tal"};
    for (int trial = 0; trial < 200; trial++) {
      Round r;
      r.amount = 10 + 1000 * rng.uniform();
      const int n = 2 + static_cast<int>(rng.uniform() * 6);
      for (int i = 0; i < n; i++)
        r.entities.push_back(entity("e" + std::to_string(i), {{"level", 200 * rng.uniform()}, {"population", 1 + 50 * rng.uniform()}, {"claim", 400 * rng.uniform()},
                                                              {"urgency", std::floor(3 * rng.uniform())}, {"need", 1000 + 500 * rng.uniform()}, {"base", 20000 * rng.uniform()}}));
      const int K = 1 + static_cast<int>(rng.uniform() * 4);
      for (int k = 0; k < K; k++) {
        const int kind = static_cast<int>(rng.uniform() * 5);
        Stage st = stage(kind == 0 ? "floor" : kind == 1 ? "priority" : kind == 2 ? "claims" : kind == 3 ? "proportional" : "equalisation", "random");
        st.share = rng.uniform() < 0.3 ? rng.uniform() : 1.0;
        st.rule = rules[static_cast<std::size_t>(rng.uniform() * rules.size()) % rules.size()];
        st.theta = rng.uniform();
        if (st.kind == "floor") st.attribute = "level", st.bound = 150;
        if (st.kind == "priority") st.attribute = "urgency", st.claim = "claim";
        if (st.kind == "claims") st.attribute = "claim";
        if (st.kind == "proportional") st.attribute = "population";
        if (st.kind == "equalisation") st.attribute = "need", st.bases = {"base"}, st.rates = {0.01};
        r.stages.push_back(st);
      }
      const auto res = run(r);
      double sum = res.left;
      for (double a : res.amount) {
        sum += a;
        nonnegative = nonnegative && a >= -1e-9 * r.amount;
      }
      conserved = conserved && close(sum, r.amount, 1e-7) && res.left >= -1e-9 * r.amount;
    }
    s.require("on 200 random rounds of random stages, what is given and what is left sum to the amount", conserved);
    s.require("and nothing given is below zero", nonnegative);
  }

  return s.done();
}
