// Checks of ecology/biodiversity: an assemblage's observed Hill numbers against their definitions at each rank; the unnamed
// counted apart; Chao1 at least the observed richness; at one's own coverage a sample gives back its observed diversity; a
// common coverage no sample exceeds at twice its size; the partition among areas: identical areas give β = 1, areas with no
// species in common and equal weights give β = N, and β lies between them otherwise.

#include <ecology/biodiversity/biodiversity.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace ecology::biodiversity;

int main() {
  calx::verification::Suite s;

  // Ten trees: 4 + 3 + 2 of three species in two genera, one known only to its genus.
  std::vector<Taxon> trees;
  for (int i = 0; i < 4; i++) trees.push_back({"Ulmus procera", "Ulmus", "Ulmaceae"});
  for (int i = 0; i < 3; i++) trees.push_back({"Ulmus minor", "Ulmus", "Ulmaceae"});
  for (int i = 0; i < 2; i++) trees.push_back({"Platanus orientalis", "Platanus", "Platanaceae"});
  trees.push_back({"", "Platanus", "Platanaceae"});
  {
    const auto d = diversity(trees, Rank::species);
    s.require("species: nine named, one unnamed", d.individuals == 9 && d.unnamed == 1);
    s.check("species: ⁰D = 3", d.observed[0] - 3, 1e-12);
    double h = 0, c = 0;
    for (double x : {4.0, 3.0, 2.0}) h -= x / 9 * std::log(x / 9), c += (x / 9) * (x / 9);
    s.check("species: ¹D = exp H", d.observed[1] - std::exp(h), 1e-12);
    s.check("species: ²D = 1/Σp²", d.observed[2] - 1 / c, 1e-12);
    s.require("species: Chao1 is at least the observed richness", d.richness.estimate >= 3 - 1e-12);
    const auto g = diversity(trees, Rank::genus);
    s.check("genus: two genera, 7 and 3 trees: ⁰D = 2", g.observed[0] - 2, 1e-12);
    s.check("genus: ²D = 1/(0.7² + 0.3²)", g.observed[2] - 1 / (0.49 + 0.09), 1e-12);
  }
  {
    // A sample at its own coverage gives back its observed diversity (rarefaction to its own size).
    const std::vector<double> counts{12, 7, 5, 3, 2, 1, 1, 1};
    const auto sAb = tyche::classes::abundances(counts);
    const double cn = tyche::classes::coverage_at(sAb, static_cast<double>(sAb.n));
    const auto at = at_coverage(counts, cn);
    s.require("coverage: the sample's own coverage is reached at its own size", at.reached && std::fabs(at.size - sAb.n) < 1e-9);
    s.check("coverage: there, ⁰D is the observed richness", at.value[0] - 8, 1e-9);
    const std::vector<std::vector<double>> samples{counts, {30, 2, 1}, {5, 5, 4, 4, 3, 3, 1}};
    const double C = common_coverage(samples, 0.99);
    bool all = true;
    for (const auto& c : samples) all = all && at_coverage(c, C).reached;
    s.require("common coverage: every sample reaches it within twice its size", all && C <= 0.99);
  }
  {
    // Partition: three areas.
    std::vector<Taxon> t;
    std::vector<int> area;
    const auto add = [&](const char* sp, const char* ge, int a, int n) { for (int i = 0; i < n; i++) t.push_back({sp, ge, "F"}), area.push_back(a); };
    // Identical areas.
    add("A", "G", 0, 3), add("B", "G", 0, 2), add("A", "G", 1, 3), add("B", "G", 1, 2);
    for (double q : {0.0, 1.0, 2.0}) s.check("partition: identical areas give β = 1", partition(t, area, 2, Rank::species, q).beta - 1, 1e-12);
    // Disjoint areas with equal numbers: β = N.
    t.clear(), area.clear();
    add("A", "G", 0, 4), add("B", "H", 1, 4), add("C", "K", 2, 4);
    for (double q : {0.0, 1.0, 2.0}) s.check("partition: disjoint areas give β = N = 3", partition(t, area, 3, Rank::species, q).beta - 3, 1e-12);
    // Overlapping: between 1 and N, and an area holding nothing named is left out.
    t.clear(), area.clear();
    add("A", "G", 0, 5), add("B", "G", 0, 1), add("A", "G", 1, 2), add("C", "H", 1, 4);
    t.push_back({"", "", ""}), area.push_back(2);
    const auto p = partition(t, area, 3, Rank::species, 1.0);
    s.require("partition: overlapping areas give 1 < β < 2, the empty area left out", p.beta > 1 && p.beta < 2 && p.units == 2);
    s.check("partition: γ = α β", p.gamma - p.alpha * p.beta, 1e-12);
  }
  return s.done();
}
