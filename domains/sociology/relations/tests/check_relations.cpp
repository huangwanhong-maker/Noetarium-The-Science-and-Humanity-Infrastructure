// Checks of sociology/relations against networkx on Zachary's karate club (tests/fixtures/karate.txt, made by
// make_relations_fixtures.py): density, transitivity, average clustering and degree assortativity of the whole; assortativity by
// club; each member's Burt constraint and effective size, k-core, clustering (from networkx's triangles) and reach within two ties.
// By hand: the E–I index and the within-group shares counted from the edge list; the QAP test finding the clubs' ties within; the
// diversity of a star's contacts; reciprocity, isolates and components of a small directed network; closure drawn reproducibly.

#include <sociology/relations/relations.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace sociology::relations;

namespace {

struct Karate {
  std::size_t n = 0;
  std::vector<Tie> edges;
  std::vector<std::size_t> club;
  std::map<std::string, std::vector<double>> value;
};

Karate read_karate() {
  Karate k;
  std::ifstream in(std::string(SOCIOLOGY_FIXTURES) + "/karate.txt");
  std::string line;
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    std::istringstream is(line);
    std::string key;
    is >> key;
    if (key == "nodes") {
      is >> k.n;
      k.club.assign(k.n, 0);
    } else if (key == "edge") {
      std::size_t a, b;
      is >> a >> b;
      k.edges.push_back({a, b});
    } else if (key == "club") {
      std::size_t i, c;
      is >> i >> c;
      k.club[i] = c;
    } else {
      double v;
      while (is >> v) k.value[key].push_back(v);
    }
  }
  return k;
}

double worst(const std::vector<double>& a, const std::vector<double>& b) {
  double w = a.size() == b.size() ? 0 : 1e300;
  for (std::size_t i = 0; i < a.size() && i < b.size(); i++) w = std::max(w, std::fabs(a[i] - b[i]));
  return w;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Karate k = read_karate();
  s.require("karate: 34 members and 78 ties read", k.n == 34 && k.edges.size() == 78);
  const Network g = tyche::networks::network(k.n, k.edges, false);
  {
    const auto w = whole(g);
    s.check("whole: density as networkx", w.density - k.value.at("density")[0], 1e-14);
    s.check("whole: transitivity as networkx", w.transitivity - k.value.at("transitivity")[0], 1e-14);
    s.check("whole: average clustering as networkx", w.average_clustering - k.value.at("average_clustering")[0], 1e-14);
    s.check("whole: degree assortativity as networkx", w.degree_assortativity - k.value.at("degree_assortativity")[0], 1e-12);
    s.require("whole: one component, no isolates", w.components == 1 && w.largest == 34 && w.isolates == 0);
  }
  {
    const auto b = belonging(g, k.club);
    s.check("belonging: assortativity by club as networkx", b.assortativity - k.value.at("club_assortativity")[0], 1e-12);
    double within = 0, between = 0;
    std::vector<double> same(2, 0), all(2, 0);
    for (const auto& e : k.edges) {
      const bool in = k.club[e[0]] == k.club[e[1]];
      (in ? within : between) += 1;
      for (std::size_t end : {e[0], e[1]}) {
        all[k.club[end]] += 1;
        if (in) same[k.club[end]] += 1;
      }
    }
    s.check("belonging: E–I index counted from the edges", b.ei_index - (between - within) / (between + within), 1e-15);
    s.check("belonging: Mr. Hi's members' ties within, counted", b.within[0] - same[0] / all[0], 1e-15);
    s.require("belonging: the clubs keep their ties within beyond relabelling (QAP p < 0.01)", b.qap_r > 0 && b.qap_p < 0.01);
    s.require("belonging: more within than group sizes alone give", b.ei_index < b.ei_complete);
  }
  {
    const auto p = people(g, k.club);
    s.check("people: Burt's constraint as networkx", worst(p.constraint, k.value.at("constraint")), 1e-12);
    s.check("people: effective size as networkx", worst(p.effective_size, k.value.at("effective_size")), 1e-12);
    std::vector<double> core(p.core.begin(), p.core.end()), reach(p.reach.begin(), p.reach.end());
    s.check("people: k-cores as networkx", worst(core, k.value.at("core")), 0);
    s.check("people: reach within two ties as networkx", worst(reach, k.value.at("reach2")), 0);
    std::vector<double> local;
    for (std::size_t v = 0; v < k.n; v++) {
      const double d = static_cast<double>(p.contacts[v]);
      local.push_back(d < 2 ? 0 : 2 * k.value.at("triangles")[v] / (d * (d - 1)));
    }
    s.check("people: local clustering from networkx's triangles", worst(p.clustering, local), 1e-14);
  }
  {
    const std::vector<Tie> star{{0, 1}, {0, 2}, {0, 3}, {0, 4}};
    const Network h = tyche::networks::network(5, star, false);
    const std::vector<std::size_t> groups{0, 0, 1, 1, 2};
    const auto p = people(h, groups);
    s.check("people: Blau's index of a star's contacts, 1 − (1/16 + 1/4 + 1/16)", p.diversity[0] - 0.625, 1e-15);
    s.check("people: their effective number of groups, 1/0.375", p.effective_groups[0] - 1 / 0.375, 1e-12);
    s.require("people: a leaf reaches the other three through the centre", p.reach[1] == 4);
  }
  {
    const std::vector<Tie> arcs{{0, 1}, {1, 0}, {1, 2}};
    const Network d = tyche::networks::network(4, arcs, true);
    const auto w = whole(d);
    s.check("whole: one mutual pair of two, reciprocity 1/2", w.reciprocity - 0.5, 1e-15);
    s.require("whole: one isolate and two components", w.isolates == 1 && w.components == 2);
    s.require("whole: the triad census counts every triad", [&] {
      std::uint64_t t = 0;
      for (auto c : w.triads) t += c;
      return t == 4;
    }());
  }
  {
    const auto a = closure(g, 200), b = closure(g, 200);
    s.require("closure: reproducible from its seed", a.expected == b.expected && a.p_greater == b.p_greater);
    s.require("closure: the observed is the whole's transitivity", std::fabs(a.transitivity - k.value.at("transitivity")[0]) < 1e-14);
    s.require("closure: a valid p-value among 200 draws", a.p_greater > 0 && a.p_greater <= 1 && a.draws == 200);
  }
  return s.done();
}
