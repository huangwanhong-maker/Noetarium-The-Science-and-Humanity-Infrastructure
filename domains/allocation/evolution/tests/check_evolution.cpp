// Checks of the allocation domain's evolution: heterogeneous allocation composed from kategoria's resource types, heuresis's
// spaces, orders of grants and assessment, where what is to be found is known.
//
// - A resource type that fails its laws is refused when a problem is composed, with the failing law.
// - A declared city case (money claimed in units, crews in units, maintenance duties that may not be thrown away, a site to one
//   area or none, a canopy's direction on the circle; the order of grants mattering through a price that rises with each job and a
//   precedence): the optimum found by enumerating all 153,600 discrete allocations is found by the search; every allocation kept is
//   lawful by kategoria's verdict, and the unlawful ones the search met (duties held back) were refused by that verdict as a
//   verified constraint; the canopy faces south; the kept compared under three principles; order found to matter, with a witness;
//   the record sealed and verified, and an altered entry found.
// - Keys collapse orders of grants that commute: four recipients of two pools, 24 orders in 4 traces of 6 orders, as morphe counts.
// - The multidimensional knapsack (instances generated as Chu and Beasley's): the direct encoding with LP-guided repair ahead of
//   permutations and random keys decoded first-fit through the composition's grant system, the ordering Raidl and Gottlieb found;
//   the price of generality reported.

#include <allocation/evolution/search.hpp>

#include <calx/optimisation/linear_programming.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <numeric>
#include <set>
#include <string>
#include <vector>

namespace ae = allocation::evolution;
namespace hs = heuresis::spaces;
namespace ha = heuresis::assessment;
namespace kr = kategoria::resources;
using kr::Rational;

namespace {

kr::Type single(const std::string& name, kr::Quantity q, bool disposable = true) {
  kr::Type T;
  T.name = name;
  T.components = {kr::Component{name, q, 1, {kr::Kind{name, disposable, std::nullopt, false}}}};
  return T;
}

allocation::rounds::Entity area(std::string id, double population, double need) {
  allocation::rounds::Entity e;
  e.id = id, e.name = "area " + id;
  e.at = {{"population", population}, {"need", need}};
  return e;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- a type that fails its laws is refused ----
  {
    kr::Type bad = single("tokens", kr::Quantity::natural);
    bad.conversions = {{"mint", {Rational(0)}, {Rational(1)}}};
    bad.laws = {{"tokens are conserved", {Rational(1)}, true}};
    ae::Problem p;
    p.name = "refused";
    p.recipients = {area("a", 1, 1)};
    p.resources = {{"tokens", bad, ae::Carrier::units, {Rational(3)}, 0, {}, {}}};
    bool refused = false;
    std::string why;
    try {
      ae::Composition c(p);
    } catch (const ae::Refusal& e) {
      refused = true, why = e.what();
    }
    s.require("a resource type whose declared conservation a conversion breaks is refused, with the law", refused && why.find("conserved") != std::string::npos, why);
  }

  // ---- a declared city case ----
  ae::Problem city;
  city.name = "three areas (declared, illustrative)";
  city.recipients = {area("a", 300, 1.0), area("b", 500, 1.4), area("c", 200, 0.8)};
  kr::Type site;
  site.name = "sites";
  site.components = {kr::Component{"sites", kr::Quantity::natural, 1, {kr::Kind{"river site", true, std::nullopt, true}}}};
  city.resources = {
      {"money", single("money", kr::Quantity::natural), ae::Carrier::unit_claims, {Rational(5)}, 0, {3, 3, 3}, {}},
      {"crews", single("crews", kr::Quantity::natural), ae::Carrier::units, {Rational(2)}, 0, {}, {}},
      {"duties", single("maintenance duties", kr::Quantity::natural, false), ae::Carrier::units, {Rational(2)}, 0, {}, {}},
      {"site", site, ae::Carrier::items, {Rational(1)}, 0, {}, {}}};
  city.attributes = {{"canopy direction", std::make_shared<hs::Sphere>(2)}};
  city.precedence = {{0, 1, "the drainage of area a before area b's works"}};
  city.scalars = {1, 0, 0, 0};  // the price of a job, and the works each area got
  city.effect = [](ae::State& x, std::size_t i) {
    if (x.held[0][i] > 0) {
      x.scalars[1 + i] = x.held[0][i] / x.scalars[0];
      x.scalars[0] *= 1.25;  // each job raises the price of the next
    }
  };
  auto c = std::make_shared<const ae::Composition>(city);
  auto benefit = [c](const ae::Allocation& a, std::vector<double>* units) {
    double total = 0;
    for (std::size_t i = 0; i < 3; i++) {
      double b = 0;
      if (a.state.granted[i]) {
        const double need = c->problem().recipients[i].get("need");
        b = need * std::log(1 + a.state.scalars[1 + i]) * (1 + 0.5 * a.amount[1][i]) - 0.3 * a.amount[2][i] * a.amount[2][i] / need;
        if (a.owner[3][0] == static_cast<std::int64_t>(i)) b += 1.2 * std::max(0.0, -a.attribute[0][i].reals[1]);
      }
      if (units) units->push_back(b);
      total += b;
    }
    return total;
  };
  {
    // Enumeration of every discrete allocation, the canopies facing south.
    double best = -1e9;
    std::size_t count = 0, lawful_count = 0;
    hs::Composition two(4, 2);
    const auto crews = two.elements(100), duties = two.elements(100);
    std::vector<std::size_t> ord = {0, 1, 2};
    std::vector<hs::Value> south(3, hs::point_value({0, -1}));
    do {
      for (int m0 = 0; m0 <= 3; m0++)
        for (int m1 = 0; m1 <= 3; m1++)
          for (int m2 = 0; m2 <= 3; m2++)
            for (const auto& cr : crews)
              for (const auto& du : duties)
                for (int st = 0; st < 4; st++) {
                  hs::Value g = hs::record_value({hs::integer_value({m0, m1, m2}), cr, du, hs::record_value({hs::category_value(st)}), hs::record_value(south),
                                                  hs::permutation_value({static_cast<std::int64_t>(ord[0]), static_cast<std::int64_t>(ord[1]), static_cast<std::int64_t>(ord[2])})});
                  const auto a = c->decode(g);
                  count++;
                  if (!c->lawful(a).lawful) continue;
                  lawful_count++;
                  best = std::max(best, benefit(a, nullptr));
                }
    } while (std::next_permutation(ord.begin(), ord.end()));

    ae::Settings settings;
    settings.strategy.mu = 60, settings.strategy.lambda = 60, settings.strategy.generations = 600;  // age layers, by default
    settings.operators.scales = {0.01, 0.1, 1};
    settings.comparing.metric = "loss of benefit";
    auto principle = [](std::string name, nomos::welfare::Kind kind, double parameter = 0) {
      nomos::welfare::Principle p;
      p.name = std::move(name), p.kind = kind, p.parameter = parameter;
      return p;
    };
    // Benefits can fall below zero (duties are a burden), so the principles are those that read any outcome.
    nomos::welfare::Principle shortfall = principle("shortfall below 1", nomos::welfare::Kind::shortfall, 1);
    settings.comparing.principles = {principle("total", nomos::welfare::Kind::total), principle("leximin", nomos::welfare::Kind::leximin), shortfall};
    settings.seed = 11;
    auto value = ae::evaluator(c, "benefit", ha::Kind::closed_form, ha::Standing::computed, [&](const ae::Allocation& a, calx::random::Stream&) {
      ha::Assessment out;
      ha::Metric m;
      std::vector<double> units;
      m.name = "loss of benefit", m.role = ha::Role::objective, m.value = -benefit(a, &units), m.uncertainty = 0;
      m.units = units;
      out.metrics.push_back(m);
      return out;
    });
    const auto found = ae::search(c, {value}, settings);
    const double got = found.kept.empty() ? -1e9 : -found.kept[0].assessment.value("loss of benefit");
    s.check("the city case: the search finds the optimum of all " + std::to_string(count) + " discrete allocations (" + std::to_string(lawful_count) + " lawful), less the search's",
            std::max(0.0, best - got), 1e-3);
    bool all_lawful = !found.kept.empty();
    for (const auto& k : found.kept) all_lawful = all_lawful && k.lawfulness.lawful;
    s.require("every allocation kept is lawful by kategoria's verdict", all_lawful);
    s.require("unlawful allocations met by the search (duties held back or unassigned) were refused by that verdict (" + std::to_string(found.unlawful_seen) + ")", found.unlawful_seen > 0);
    if (found.kept.empty()) return s.done();
    const auto& top = found.kept[0].allocation;
    double south_error = 0;
    if (top.owner[3][0] >= 0) south_error = std::acos(std::clamp(-top.attribute[0][static_cast<std::size_t>(top.owner[3][0])].reals[1], -1.0, 1.0));
    s.check("the canopy on the site faces south (radians off)", south_error, 0.05);
    s.require("the kept are compared under three principles, never one score", found.comparison.has_value() && !found.comparison->maximal.empty() && found.comparison->principles.size() == 3);
    s.require("order is found to matter, with a witness", found.order.order_may_matter() && !found.order.witnesses.empty(), found.order.witnesses.empty() ? "" : found.order.witnesses[0]);
    const auto verdict = archeion::fixity::verify(found.record);
    s.require("the search's record is sealed and verifies (" + std::to_string(found.record.entries.size()) + " entries)", verdict.holds());
    auto altered = found.record;
    altered.entries[altered.entries.size() / 2].note += " (altered)";
    s.require("and an altered entry is found", !archeion::fixity::verify(altered).holds());
    s.require("the loop reused remembered evaluations by key (" + std::to_string(found.reused) + ")", found.reused > 0 && found.evaluated_allocations <= found.evaluated_genotypes);
  }

  // ---- keys collapse orders of grants that commute ----
  {
    ae::Problem two;
    two.name = "two pools";
    for (int i = 0; i < 4; i++) {
      auto e = area(std::string(1, char('a' + i)), 1, 1);
      e.at["from first"] = i < 2 ? 1 : 0;
      e.at["from second"] = i < 2 ? 0 : 1;
      two.recipients.push_back(e);
    }
    two.resources = {{"first", single("first", kr::Quantity::natural), ae::Carrier::fixed, {Rational(10)}, 0, {}, "from first"},
                     {"second", single("second", kr::Quantity::natural), ae::Carrier::fixed, {Rational(10)}, 0, {}, "from second"}};
    ae::Composition cc(two);
    std::set<std::string> keys;
    std::vector<std::int64_t> perm = {0, 1, 2, 3};
    do keys.insert(cc.key(hs::record_value({hs::permutation_value(perm)})));
    while (std::next_permutation(perm.begin(), perm.end()));
    const auto a = cc.decode(hs::record_value({hs::permutation_value({0, 1, 2, 3})}));
    const auto lin = morphe::formal::traces::linearisations(morphe::formal::traces::causal_order(a.granted, cc.independence(a))).str();
    s.require("24 orders of four grants on two pools have 4 keys, one per trace (" + std::to_string(keys.size()) + "; morphe counts " + lin + " orders in each)", keys.size() == 4 && lin == "6");
  }

  // ---- the multidimensional knapsack: three routes ----
  {
    const std::size_t n = 50, m = 5;
    std::vector<double> gaps_di, gaps_pe, gaps_rk;
    for (std::uint32_t inst = 0; inst < 3; inst++) {
      calx::random::Stream g(1998, 50 + inst, 0, 0);
      std::vector<std::vector<double>> w(m, std::vector<double>(n));
      std::vector<double> cap(m, 0), profit(n, 0);
      for (std::size_t i = 0; i < m; i++)
        for (std::size_t j = 0; j < n; j++) w[i][j] = static_cast<double>(g.below(1001)), cap[i] += w[i][j];
      for (auto& v : cap) v = std::floor(0.5 * v);
      for (std::size_t j = 0; j < n; j++) {
        for (std::size_t i = 0; i < m; i++) profit[j] += w[i][j] / static_cast<double>(m);
        profit[j] += 500 * g.uniform();
      }
      // The LP relaxation: its bound, and its duals for the direct encoding's repair.
      calx::optimisation::LinearProgram lp;
      lp.c.resize(n);
      for (std::size_t j = 0; j < n; j++) lp.c[j] = -profit[j];
      lp.a_ub = calx::linalg::Rectangular(m, n);
      for (std::size_t i = 0; i < m; i++)
        for (std::size_t j = 0; j < n; j++) lp.a_ub(i, j) = w[i][j];
      lp.b_ub = cap;
      lp.lower.assign(n, 0), lp.upper.assign(n, 1);
      const auto sol = calx::optimisation::simplex(lp);
      const double bound = -sol.value;
      std::vector<double> utility(n);
      for (std::size_t j = 0; j < n; j++) {
        double d = 1e-12;
        for (std::size_t i = 0; i < m; i++) d += -sol.duals_ub[i] * w[i][j];
        utility[j] = profit[j] / d;
      }
      std::vector<std::size_t> by_utility(n);
      std::iota(by_utility.begin(), by_utility.end(), 0);
      std::sort(by_utility.begin(), by_utility.end(), [&](std::size_t a, std::size_t b) { return utility[a] > utility[b]; });
      auto fits = [&](const std::vector<char>& x) {
        for (std::size_t i = 0; i < m; i++) {
          double used = 0;
          for (std::size_t j = 0; j < n; j++) used += x[j] ? w[i][j] : 0;
          if (used > cap[i]) return false;
        }
        return true;
      };
      // The direct encoding: a subset repaired by the LP's utilities, dropping the least useful, then adding the most useful that fit.
      auto repair = [&](const hs::Value& v) {
        std::vector<char> x(n, 0);
        for (auto j : v.ints) x[static_cast<std::size_t>(j)] = 1;
        for (std::size_t k = n; k-- > 0 && !fits(x);)
          if (x[by_utility[k]]) x[by_utility[k]] = 0;
        for (auto j : by_utility)
          if (!x[j]) {
            x[j] = 1;
            if (!fits(x)) x[j] = 0;
          }
        return x;
      };
      heuresis::evolution::Strategy strategy;
      strategy.mu = 40, strategy.lambda = 40, strategy.generations = 125, strategy.from_archive = 0;
      auto direct = [&](std::uint64_t seed) {
        auto sub = std::make_shared<hs::Subset>(n);
        auto ops = hs::operators(sub);
        ops.key = [&](const hs::Value& v) {  // phenotypic duplicates removed
          const auto x = repair(v);
          return std::string(x.begin(), x.end());
        };
        heuresis::evolution::Evolution<hs::Value> e(ops, strategy, {}, seed);
        heuresis::evolution::evolve(e, [&](const hs::Value& v) {
          const auto x = repair(v);
          double p = 0;
          for (std::size_t j = 0; j < n; j++) p += x[j] ? profit[j] : 0;
          return heuresis::search::scalar(-p);
        });
        return (bound + e.state().best->evaluation.objectives[0]) / bound;
      };
      // Permutations and random keys decoded first-fit through the composition: each item a recipient with fixed demands.
      auto composed = [&](ae::OrderBy by) {
        ae::Problem p;
        p.name = "a multidimensional knapsack";
        p.order_by = by;
        for (std::size_t j = 0; j < n; j++) {
          allocation::rounds::Entity e;
          e.id = "item " + std::to_string(j);
          for (std::size_t i = 0; i < m; i++) e.at["w" + std::to_string(i)] = w[i][j];
          e.at["profit"] = profit[j];
          p.recipients.push_back(e);
        }
        for (std::size_t i = 0; i < m; i++)
          p.resources.push_back({"capacity " + std::to_string(i), single("capacity " + std::to_string(i), kr::Quantity::natural), ae::Carrier::fixed,
                                 {Rational(static_cast<long long>(cap[i]))}, 0, {}, "w" + std::to_string(i)});
        return std::make_shared<const ae::Composition>(p);
      };
      auto via = [&](ae::OrderBy by) {
        auto cmp = composed(by);
        return [cmp, &strategy, &profit, bound](std::uint64_t seed) {
          ae::Settings st;
          st.strategy = strategy, st.seed = seed, st.keep = 1;
          auto total = ae::evaluator(cmp, "profit", ha::Kind::closed_form, ha::Standing::computed, [&profit](const ae::Allocation& a, calx::random::Stream&) {
            ha::Assessment out;
            ha::Metric mm;
            double p = 0;
            for (auto j : a.granted) p += profit[j];
            mm.name = "negative profit", mm.value = -p, mm.uncertainty = 0;
            out.metrics.push_back(mm);
            return out;
          });
          const auto r = ae::search(cmp, {total}, st);
          return (bound + r.kept[0].assessment.value("negative profit")) / bound;
        };
      };
      const auto routes = ae::compare_routes({{"direct with repair", direct}, {"permutations, first fit", via(ae::OrderBy::permutation)}, {"random keys, first fit", via(ae::OrderBy::keys)}}, {1, 2, 3});
      gaps_di.push_back(routes.mean[0]), gaps_pe.push_back(routes.mean[1]), gaps_rk.push_back(routes.mean[2]);
    }
    auto mean = [](const std::vector<double>& v) { return std::accumulate(v.begin(), v.end(), 0.0) / static_cast<double>(v.size()); };
    char buf[200];
    std::snprintf(buf, sizeof buf, "mean gaps to the LP bound: direct %.3f%%, permutations %.3f%%, random keys %.3f%%; the price of generality %.3f points",
                  100 * mean(gaps_di), 100 * mean(gaps_pe), 100 * mean(gaps_rk), 100 * (std::min(mean(gaps_pe), mean(gaps_rk)) - mean(gaps_di)));
    s.require("the knapsack: the direct encoding with LP-guided repair ahead of permutations and random keys (Raidl and Gottlieb's ordering)",
              mean(gaps_di) < mean(gaps_pe) && mean(gaps_di) < mean(gaps_rk), buf);
  }

  return s.done();
}
