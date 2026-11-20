// Checks of allocation/evaluators: in an economy of places, two orders of the same grants re-spend differently when households
// spend by whether they are short of money and alike when they do not, the accounts balance under both, the per-recipient
// re-spending sums to the objective, and the reading is computed from written rules; a search over a fund's units driven by
// the economy keeps lawful allocations whose accounts balance; under a law of holdings with a mean field the two orders end
// differently, the overall mean rises by exactly what was granted per unit, and nothing is lost; under laws learned from a
// record the allocation's worth is their mean, a grant beyond what the record varied violates the support, and two
// allocations are ranked across the laws with a standing that turns undecided outside the support.

#include <allocation/evaluators/evaluators.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

namespace ae = allocation::evolution;
namespace av = allocation::evaluators;
namespace el = economics::ledger;
namespace ha = heuresis::assessment;
namespace kr = kategoria::resources;

namespace {

ae::Allocation granted(std::vector<double> amounts, std::vector<std::size_t> order) {
  ae::Allocation a;
  a.amount = {std::move(amounts)};
  a.granted = std::move(order);
  return a;
}

kr::Type natural(const std::string& name) {
  kr::Type T;
  T.name = name;
  T.components = {kr::Component{name, kr::Quantity::natural, 1, {kr::Kind{name, true, std::nullopt, false}}}};
  return T;
}

allocation::rounds::Entity area(std::string id) {
  allocation::rounds::Entity e;
  e.id = id, e.name = "place " + id;
  e.at = {{"population", 4}};
  return e;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  // ---- an economy of places -------------------------------------------------------------------------------------------
  av::EconomySpec spec;
  spec.places = {{"north", 4, 2, 200000, 400000}, {"south", 4, 2, 20000, 400000}};
  spec.habits.short_below = 60000;
  spec.unit = 1000;
  spec.spacing = 3;
  spec.periods = 120;
  {
    const auto ns = av::read_economy(spec, granted({600, 600}, {0, 1})), sn = av::read_economy(spec, granted({600, 600}, {1, 0}));
    const double a = ns.assessment.value("local re-spending lost"), b = sn.assessment.value("local re-spending lost");
    s.require("an economy of places: the accounts balance under both orders",
              ns.assessment.value("accounts that do not balance") == 0 && sn.assessment.value("accounts that do not balance") == 0);
    s.require("households that spend by whether they are short: the order of the same grants changes what is re-spent", std::fabs(a - b) > 1e-3 * std::fabs(a),
              std::to_string(a) + " against " + std::to_string(b));
    double sum = 0;
    for (double u : ns.assessment.find("local re-spending lost")->units) sum += u;
    s.check("the re-spending of each place's grant sums to the objective", sum + a, 1e-9);
    s.require("written rules alone: the reading is computed", ns.assessment.find("local re-spending lost")->standing == ha::Standing::computed);
    av::EconomySpec lin = spec;
    lin.habits.short_spend = lin.habits.spend, lin.habits.short_local = lin.habits.local;
    const double c = av::read_economy(lin, granted({600, 600}, {0, 1})).assessment.value("local re-spending lost"),
                 d = av::read_economy(lin, granted({600, 600}, {1, 0})).assessment.value("local re-spending lost");
    s.check("habits that do not depend on the state: the order leaves the re-spending as it was (relative)", (c - d) / c, 1e-4);
  }
  {
    // A fund of 9 units of 1,000 for three places, in an order, searched with the economy as the evaluator.
    ae::Problem p;
    p.name = "a fund for three places (declared, illustrative)";
    p.recipients = {area("north"), area("south"), area("east")};
    p.resources = {{"fund", natural("fund"), ae::Carrier::units, {kr::Rational(9)}, 0, {}, {}, false}};
    auto c = std::make_shared<const ae::Composition>(p);
    av::EconomySpec three = spec;
    three.places.push_back({"east", 4, 2, 40000, 400000});
    three.periods = 60;
    ae::Settings st;
    st.strategy.mu = 16, st.strategy.lambda = 16, st.strategy.generations = 25;
    st.seed = 5;
    st.keep = 3;
    const auto out = ae::search(c, {av::economy_evaluator(c, three)}, st);
    bool lawful = !out.kept.empty(), balanced = true;
    for (const auto& k : out.kept) {
      lawful = lawful && k.lawfulness.lawful;
      balanced = balanced && k.assessment.value("accounts that do not balance") == 0;
    }
    s.require("a search driven by the economy keeps lawful allocations", lawful, std::to_string(out.kept.size()) + " kept");
    s.require("... each with accounts that balance", balanced);
    double given = 0;
    for (double v : out.kept.front().allocation.amount[0]) given += v;
    s.require("... the best granting the fund, since money re-spent is worth more than money kept back", given == 9, std::to_string(given));
  }
  // ---- a stand-in for the economy, its coverage checked -------------------------------------------------------------
  {
    av::EconomySpec three = spec;
    three.places.push_back({"east", 4, 2, 40000, 400000});
    three.periods = 60;
    const auto truth = [&](const ae::Allocation& a) { return av::read_economy(three, a).assessment.value("local re-spending lost"); };
    // Nine units of 1,000 among three places, in a random order: the amounts by nine draws, the order by a shuffle.
    const auto draw = [](std::size_t k) {
      calx::random::Stream r(4000 + k, 7, 0, 0);
      std::vector<double> amount(3, 0);
      for (int u = 0; u < 9; u++) amount[r.below(3)] += 1;
      std::vector<std::size_t> order{0, 1, 2};
      for (std::size_t j = 3; j > 1; j--) std::swap(order[j - 1], order[r.below(j)]);
      return granted(amount, order);
    };
    const auto S = av::stand_in(draw, truth, 0, 9, 60, 150, 300, 0.2);
    // Promised 80%; the coverage given one calibration set varies with a deviation of about √(0.16/151), and the held-out
    // share adds √(0.16/300): three of their combined deviations either side of the band [0.80, 0.807].
    const double sd = std::sqrt(0.16 / 151 + 0.16 / 300);
    s.require("a stand-in for the economy covers held-out runs as often as promised (80%), within three deviations of sampling",
              S.held_out_coverage > 0.8 - 3 * sd && S.held_out_coverage < 0.807 + 3 * sd, std::to_string(S.held_out_coverage));
    std::vector<ae::Allocation> candidates;
    for (std::size_t k = 0; k < 60; k++) candidates.push_back(draw(10000 + k));
    const auto screened = av::screen_allocations(S, candidates, truth, 0, 9);
    double least = truth(candidates[0]);
    for (std::size_t k = 1; k < candidates.size(); k++) least = std::min(least, truth(candidates[k]));
    s.require("screening by the stand-in keeps the allocation the economy finds best among the candidates", std::fabs(screened.best_value - least) < 1e-9,
              std::to_string(screened.assessed.size()) + " of 60 run in the economy");
    s.require("... running fewer than all of them in the economy", screened.assessed.size() < candidates.size());
  }
  // ---- a law of holdings with a mean field ----------------------------------------------------------------------------
  {
    av::DensitySpec d;
    d.population = {100, 300};
    d.mean = {2.0, 0.6};
    d.grid = {0, 60, 601};
    d.unit = 1;
    d.line = 0.5;
    d.between = 10, d.after = 30;
    const auto a = av::read_density(d, granted({40, 40}, {0, 1})), b = av::read_density(d, granted({40, 40}, {1, 0}));
    s.require("under a law of holdings with a mean field, two orders of the same grants end differently", std::fabs(a.below - b.below) > 1e-5,
              std::to_string(a.below) + " against " + std::to_string(b.below));
    s.check("grants carry next to no unit past the grid's end (only the top node's minute mass)", a.beyond + b.beyond, 1e-4);
    // The mean field keeps the overall mean; reflection at the grid's far end loses a little of a tail that falls as w⁻⁵.
    s.check("the overall mean rises by what was granted per unit (the mean field keeps it)", a.mean - (a.start + 80.0 / 400), 3e-4);
    s.check("... the start being the declared means as the grid holds them", a.start - (100 * 2.0 + 300 * 0.6) / 400, 0.15);
    double mass = 0;
    for (const auto& g : a.densities)
      for (double v : g) mass += v;
    s.check("... and nothing is lost: every density keeps its mass", mass - 2, 1e-10);
  }
  // ---- laws learned from a record -------------------------------------------------------------------------------------
  {
    av::LawsSpec L;
    auto law = [](double a) {
      return kinesis::induction::Member{"a = " + std::to_string(a), 1, [a](const std::vector<double>& x, const std::vector<double>& u) {
                                          return std::vector<double>{a * x[0] + u[0], a * x[1] + u[1]};
                                        }};
    };
    L.laws = {law(0.8), law(0.9)};
    L.start = {0, 0};
    L.between = 1, L.after = 3;
    L.inputs = kinesis::induction::support_of({{0, 0}, {500, 500}}, {"grant to the first", "grant to the second"});
    const auto first = granted({100, 300}, {0, 1}), later = granted({100, 300}, {1, 0});
    const auto r = av::read_laws(L, first);
    // Under a: 100 at step 0 and 300 at step 1, then three steps: worth 100 a⁴ + 300 a³.
    const double expect = (100 * std::pow(0.8, 4) + 300 * std::pow(0.8, 3) + 100 * std::pow(0.9, 4) + 300 * std::pow(0.9, 3)) / 2;
    s.check("the worth under the learned laws is their mean, exactly", r.value("worth lost under the learned laws") + expect, 1e-9);
    s.require("... and stands as an estimate", r.find("worth lost under the learned laws")->standing == ha::Standing::estimated);
    const auto rk = av::rank(L, first, later);
    s.require("giving the larger grant later is worth more under every law with a < 1: the claim holds", rk.standing == "holds");
    const auto bold = av::read_laws(L, granted({100, 600}, {0, 1}));
    s.require("a grant beyond what the record varied violates the support, verified", bold.value("steps beyond the record's support") > 0 &&
                                                                                         bold.find("steps beyond the record's support")->standing == ha::Standing::verified);
    s.require("... and a ranking that involves it is undecided", av::rank(L, granted({100, 600}, {0, 1}), later).standing == "undecided");
  }
  return s.done();
}
