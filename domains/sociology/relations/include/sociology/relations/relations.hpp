// sociology::relations: ties among people and what they hold (ADR 0104).
//
// Relational life (friendship, kinship, help, neighbouring) is read as a network of people with their
// attributes. The statistics are tyche's (`networks`, `classes`); this header gives the sociological
// objects they answer:
//
//   - the whole: its density, closure (transitivity and clustering), reciprocity, triads, components,
//     isolates and who is tied to whom by degree;
//   - belonging: how groups (by gender, origin, class, age) keep their ties among themselves: the share
//     of ties within, Coleman's inbreeding homophily, Krackhardt and Stern's E–I index against the index
//     the complete network would have, Newman's assortativity, and a QAP test of ties against shared group;
//   - each person's ties: contacts, clustering, Burt's constraint and effective size, the k-core they sit
//     in, the people reached within two ties, and the diversity of their contacts' groups (Blau's index and
//     the effective number of groups, Hill's number of order two);
//   - closure beyond chance: transitivity against networks with the same degrees (a CUG test).
//
// Sources: Wasserman, S. and Faust, K. (1994), Social Network Analysis, Cambridge University Press;
// Coleman, J. S. (1958), Relational analysis, Human Organization 17, 28–36; Krackhardt, D. and Stern, R. N.
// (1988), Informal networks and organizational crises, Social Psychology Quarterly 51, 123–140; Burt, R. S.
// (1992), Structural Holes, Harvard University Press; Blau, P. M. (1977), Inequality and Heterogeneity, Free
// Press; McPherson, M., Smith-Lovin, L. and Cook, J. M. (2001), Birds of a feather, Annual Review of
// Sociology 27, 415–444; Krackhardt, D. (1988), Predicting with networks: nonparametric multiple regression
// analysis of dyadic data, Social Networks 10, 359–381.
//
// Accuracy: exact counts and ratios on the network given. Tests are by permutation (QAP) and by drawing
// networks with the observed degrees, with p-values (1 + #)/(1 + B) that are exact for the null drawn.

#pragma once

#include <tyche/classes/diversity.hpp>
#include <tyche/networks/brokerage.hpp>
#include <tyche/networks/descriptive.hpp>
#include <tyche/networks/mixing.hpp>
#include <tyche/networks/network.hpp>
#include <tyche/networks/null_models.hpp>
#include <tyche/networks/qap.hpp>

#include <calx/linalg/dense.hpp>

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <stdexcept>
#include <string>
#include <vector>

namespace sociology::relations {

using tyche::networks::Network;
using tyche::networks::Tie;

inline void require(bool ok, const char* where, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("sociology::relations::") + where + ": " + what);
}

// ---------------------------------------------------------------- the whole

struct Whole {
  std::size_t people = 0, ties = 0;
  bool directed = false;
  double density = 0;
  double mean_contacts = 0;             // mean distinct contacts in the underlying graph
  double transitivity = 0;              // 3 × triangles / connected triples (underlying graph)
  double average_clustering = 0;        // the mean of the local coefficients (0 for people with fewer than two contacts)
  double reciprocity = std::numeric_limits<double>::quiet_NaN();  // M/(M + A), directed only
  double degree_assortativity = std::numeric_limits<double>::quiet_NaN();
  std::size_t components = 0, largest = 0, isolates = 0;
  tyche::networks::TriadCensus triads{};  // directed only (zeros otherwise)
};

inline Whole whole(const Network& g) {
  require(g.nodes >= 2, "whole", "at least two people");
  Whole w;
  w.people = g.nodes;
  w.ties = g.size();
  w.directed = g.directed;
  w.density = tyche::networks::density(g);
  const Network u = g.directed ? tyche::networks::underlying(g) : g;
  const auto d = tyche::networks::degrees(u);
  double contacts = 0;
  for (std::size_t v = 0; v < u.nodes; v++) {
    contacts += static_cast<double>(d.neighbours[v]);
    if (d.neighbours[v] == 0) w.isolates++;
  }
  w.mean_contacts = contacts / static_cast<double>(u.nodes);
  w.transitivity = tyche::networks::transitivity(u);
  w.average_clustering = tyche::networks::average_clustering(u);
  const auto comp = tyche::networks::weak_components(g);
  w.components = comp.count();
  w.largest = comp.largest();
  if (g.size() > 0) {
    if (g.directed) {
      w.reciprocity = tyche::networks::reciprocity(g).dyadic;
      w.triads = tyche::networks::triad_census(g);
    }
    try {
      w.degree_assortativity = tyche::networks::degree_assortativity(g);
    } catch (const std::exception&) {
      // Every tie joins people of one degree: the coefficient is undefined.
    }
  }
  return w;
}

// ---------------------------------------------------------------- belonging

struct BelongingOptions {
  std::size_t permutations = 999;
  std::uint64_t seed = 20261113;
};

struct Belonging {
  std::vector<std::size_t> members;   // per group
  std::vector<double> share;          // its share of the people
  std::vector<double> within;         // the share of its members' ties that stay in the group
  std::vector<double> inbreeding;     // Coleman's (within − share)/(1 − share)
  std::vector<double> ei;             // Krackhardt and Stern's E–I index of the group's ties
  double ei_index = 0;                // over every tie: −1 all within, +1 all between
  double ei_complete = 0;             // the index if everyone were tied to everyone: the baseline of group sizes
  double assortativity = 0;           // Newman's, by group
  double qap_r = 0;                   // the correlation of ties with shared group, over the pairs
  double qap_p = 1;                   // its permutation p-value, ties more within groups than relabelling gives
  std::size_t permutations = 0;
};

inline Belonging belonging(const Network& g, std::span<const std::size_t> group, const BelongingOptions& o = {}) {
  require(group.size() == g.nodes, "belonging", "one group per person");
  require(g.size() > 0, "belonging", "at least one tie");
  Belonging b;
  const auto h = tyche::networks::inbreeding_homophily(g, group);
  b.members = h.members;
  b.share = h.share;
  b.within = h.same;
  b.inbreeding = h.inbreeding;
  const auto e = tyche::networks::ei_index(g, group);
  b.ei = e.group;
  b.ei_index = e.index;
  b.ei_complete = e.complete;
  b.assortativity = tyche::networks::categorical_assortativity(g, group);
  calx::linalg::Matrix A = tyche::networks::adjacency(g), S(g.nodes);
  for (std::size_t i = 0; i < g.nodes; i++)
    for (std::size_t j = 0; j < g.nodes; j++) {
      S(i, j) = i != j && group[i] == group[j] ? 1.0 : 0.0;
      if (A(i, j) != 0) A(i, j) = 1;
    }
  tyche::networks::QapOptions q;
  q.permutations = o.permutations;
  q.seed = o.seed;
  q.symmetric = !g.directed;
  const auto r = tyche::networks::qap_correlation(A, S, q);
  b.qap_r = r.r;
  b.qap_p = r.p_greater;
  b.permutations = r.permutations;
  return b;
}

// ---------------------------------------------------------------- each person's ties

struct People {
  std::vector<std::size_t> contacts;      // distinct contacts
  std::vector<double> clustering;         // local clustering (underlying graph)
  std::vector<double> constraint;         // Burt's constraint: high, tied into one closed circle
  std::vector<double> effective_size;     // Burt's effective size: contacts less their redundancy
  std::vector<std::size_t> core;          // the deepest k-core the person sits in
  std::vector<std::size_t> reach;         // others reached within two ties
  std::vector<double> diversity;          // Blau's 1 − Σ p_g² over the contacts' groups (NaN without contacts or groups)
  std::vector<double> effective_groups;   // Hill's number of order two of the same law, 1/Σ p_g²
};

inline People people(const Network& g, std::span<const std::size_t> group = {}) {
  require(group.empty() || group.size() == g.nodes, "people", "one group per person, or none");
  const Network u = g.directed ? tyche::networks::underlying(g) : g;
  const auto nb = tyche::networks::neighbourhoods(u);
  People p;
  const auto br = tyche::networks::brokerage(g);
  p.constraint = br.constraint;
  p.effective_size = br.effective_size;
  p.clustering = tyche::networks::local_clustering(u);
  p.core = tyche::networks::core_numbers(u);
  std::size_t groups = 0;
  for (std::size_t c : group) groups = std::max(groups, c + 1);
  const double nan = std::numeric_limits<double>::quiet_NaN();
  std::vector<char> seen(g.nodes, 0);
  for (std::size_t v = 0; v < g.nodes; v++) {
    p.contacts.push_back(nb[v].size());
    std::vector<std::size_t> marked;
    for (std::size_t a : nb[v]) {
      if (!seen[a]) seen[a] = 1, marked.push_back(a);
      for (std::size_t b : nb[a])
        if (b != v && !seen[b]) seen[b] = 1, marked.push_back(b);
    }
    p.reach.push_back(marked.size());
    for (std::size_t a : marked) seen[a] = 0;
    if (group.empty() || nb[v].empty()) {
      p.diversity.push_back(nan);
      p.effective_groups.push_back(nan);
      continue;
    }
    std::vector<double> law(groups, 0.0);
    for (std::size_t a : nb[v]) law[group[a]] += 1.0 / static_cast<double>(nb[v].size());
    double sq = 0;
    for (double q : law) sq += q * q;
    p.diversity.push_back(1 - sq);
    std::vector<double> present;
    for (double q : law)
      if (q > 0) present.push_back(q);
    p.effective_groups.push_back(tyche::classes::hill(present, 2));
  }
  return p;
}

// ---------------------------------------------------------------- closure beyond chance

struct Closure {
  double transitivity = 0;            // observed (underlying graph)
  double expected = 0, sd = 0;        // among networks with the same degrees
  double p_greater = 1;               // (1 + #{t* ≥ t})/(1 + B)
  std::size_t draws = 0;
};

// Whether friends of friends are friends more often than the people's numbers of contacts alone imply.
inline Closure closure(const Network& g, std::size_t draws = 500, std::uint64_t seed = 20261113) {
  const Network u = g.directed ? tyche::networks::underlying(g) : g;
  require(u.size() >= 2, "closure", "at least two ties");
  tyche::networks::CugOptions o;
  o.conditioning = tyche::networks::Conditioning::degrees;
  o.draws = draws;
  o.seed = seed;
  const auto t = tyche::networks::cug_test(u, [](const Network& h) { return tyche::networks::transitivity(h); }, o);
  Closure c;
  c.transitivity = t.observed;
  c.expected = t.mean;
  c.sd = t.sd;
  c.p_greater = t.p_greater;
  c.draws = t.draws;
  return c;
}

}  // namespace sociology::relations
