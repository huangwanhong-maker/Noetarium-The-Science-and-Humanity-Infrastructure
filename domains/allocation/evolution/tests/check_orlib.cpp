// Checks of the composition on the OR-Library's multidimensional knapsacks (ADR 0141), read from the data library's cache by
// heuresis's reader: each of mknap1's seven problems composed as items with fixed demands on capacities, searched as orders decoded
// first fit, and held to the optimum the file gives; three of Chu and Beasley's problems of 100 items and 5 constraints set beside
// the best values their genetic algorithm with LP-guided repair found, the generic route about a tenth below them. Skipped where the cache does not hold the files (python data/products/or_library.py).

#include <allocation/evolution/search.hpp>

#include <heuresis/benchmarks/orlib.hpp>

#include <calx/verification/check.hpp>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <string>

namespace ae = allocation::evolution;
namespace ha = heuresis::assessment;
namespace hb = heuresis::benchmarks;
namespace kr = kategoria::resources;

namespace {

std::string slurp(const std::string& rel) {
  const char* env = std::getenv("SCIENCE_CACHE");
  std::ifstream in((env ? std::string(env) : std::string(ALLOCATION_CACHE)) + "/" + rel);
  if (!in) return {};
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

kr::Type natural(const std::string& name) {
  kr::Type T;
  T.name = name;
  T.components = {kr::Component{name, kr::Quantity::natural, 1, {kr::Kind{name, true, std::nullopt, false}}}};
  return T;
}

// The best profit the composition finds for a knapsack, with orders of the items decoded first fit.
double composed_best(const hb::Knapsack& k, std::uint64_t seed, std::size_t generations) {
  ae::Problem p;
  p.name = "an OR-Library knapsack";
  for (std::size_t j = 0; j < k.n; j++) {
    allocation::rounds::Entity e;
    e.id = "item " + std::to_string(j);
    for (std::size_t i = 0; i < k.m; i++) e.at["w" + std::to_string(i)] = k.weights[i][j];
    p.recipients.push_back(e);
  }
  for (std::size_t i = 0; i < k.m; i++)
    p.resources.push_back({"capacity " + std::to_string(i), natural("capacity " + std::to_string(i)), ae::Carrier::fixed,
                           {kr::Rational(static_cast<long long>(std::floor(k.capacities[i])))}, 0, {}, "w" + std::to_string(i)});
  auto c = std::make_shared<const ae::Composition>(p);
  ae::Settings st;
  st.strategy.mu = 40, st.strategy.lambda = 40, st.strategy.generations = generations;
  st.seed = seed, st.keep = 1;
  const auto profits = k.profits;
  auto total = ae::evaluator(c, "profit", ha::Kind::closed_form, ha::Standing::computed, [profits](const ae::Allocation& a, calx::random::Stream&) {
    ha::Assessment out;
    ha::Metric m;
    double v = 0;
    for (auto j : a.granted) v += profits[j];
    m.name = "negative profit", m.value = -v, m.uncertainty = 0;
    out.metrics.push_back(m);
    return out;
  });
  const auto r = ae::search(c, {total}, st);
  return -r.kept[0].assessment.value("negative profit");
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string mknap1 = slurp("or-library/mknap1.txt");
  if (mknap1.empty()) {
    s.skip("mknap1 is not in the data library's cache (python data/products/or_library.py)");
  } else {
    const auto K = hb::read_knapsacks(mknap1);
    std::size_t reached = 0;
    double worst = 0;
    std::string line;
    for (std::size_t k = 0; k < K.size(); k++) {
      double best = 0;
      for (std::uint64_t seed : {1, 2, 3}) best = std::max(best, composed_best(K[k], seed, 150));
      const double gap = (K[k].optimum - best) / K[k].optimum;
      reached += gap < 1e-9;
      worst = std::max(worst, gap);
      char b[80];
      std::snprintf(b, sizeof b, "%s%zu items: %.1f of %.1f", line.empty() ? "" : "; ", K[k].n, best, K[k].optimum);
      line += b;
    }
    std::printf("mknap1 through the composition (best of three seeds): %s\n", line.c_str());
    s.require("mknap1's seven problems through the composition: none above its optimum, and the optimum reached on most", worst >= 0 && reached >= 5,
              std::to_string(reached) + " of 7 reached");
    s.check("... the largest gap to an optimum (relative)", worst, 0.02);
  }
  const std::string cb = slurp("or-library/mknapcb1.txt"), res = slurp("or-library/mkcbres.txt");
  if (cb.empty() || res.empty()) {
    s.skip("mknapcb1 and mkcbres are not in the data library's cache");
  } else {
    const auto K = hb::read_knapsacks(cb);
    // Chu and Beasley's best values for 5.100-00, -01 and -02, as mkcbres gives them.
    const double best_known[3] = {24381, 24274, 23551};
    std::string line;
    double worst = 0;
    for (std::size_t k = 0; k < 3; k++) {
      const double v = composed_best(K[k], 1, 150);
      worst = std::max(worst, (best_known[k] - v) / best_known[k]);
      char b[80];
      std::snprintf(b, sizeof b, "%s5.100-%02zu: %.0f of %.0f", line.empty() ? "" : "; ", k, v, best_known[k]);
      line += b;
    }
    std::printf("Chu and Beasley's problems through the composition: %s\n", line.c_str());
    // The generic route (orders decoded first fit) against a genetic algorithm with LP-guided repair: the price of generality that
    // ADR 0137 measured on generated instances, here on the published ones, about a tenth at 6,000 evaluations.
    s.check("three of Chu and Beasley's problems of 100 items: the generic composition within an eighth of their best values", worst, 0.125);
  }
  return s.done();
}
