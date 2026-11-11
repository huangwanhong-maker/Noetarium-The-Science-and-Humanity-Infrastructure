// The size of a market (the domain's theory note, §5).
//
// A SIZE IS A TREE OF QUANTITIES. Its leaves are data, each with its source, or assumptions, each with the range it is believed to lie
// in: low ≤ mid ≤ high, all nonnegative. Its other nodes are sums and products of their children (the buyers of several regions; buyers
// × the share with the need × what one spends). A node may be used by more than one other, so the "tree" is an acyclic graph, built
// in order: a node's children come before it.
//
//   bounds         every node is nondecreasing in every leaf, so its least and greatest values are taken at the leaves' lows and at
//                  their highs: the bounds are exact, attained, and need no interval arithmetic (§5.1; checked against every corner)
//   elasticity     ε_ℓ = ∂ ln Y / ∂ ln x_ℓ at the mids: the per cent the size moves for one per cent of a leaf. Through a product it is
//                  passed whole, through a sum in proportion to the term's part; so it is 1 for every leaf of a chain of products, and
//                  the elasticities of the leaves of a sum of single leaves add to 1 (by adjoints over the graph; checked against
//                  finite differences)
//   swing          the size with one leaf at its low and at its high, the others at their mids: what a tornado chart draws, the
//                  leaves in the order of what they could change
//
// TAM, SAM, SOM. A chain  all who could buy ≥ those who can be served ≥ those who can be won  is a base times shares in [0, 1], and
// each stage is the product of a prefix: TAM ≥ SAM ≥ SOM at the lows, the mids and the highs (§5.2).
//
// TWO ESTIMATES OF ONE SIZE (from the top: a population and shares; from the bottom: outlets and what each sells) agree when their
// ranges meet; when they do not, the gap is the ratio of the nearer ends, and something assumed is wrong (§5.3).
//
// BUYERS OF A LATER YEAR. With a population by age N and a rate of buying by age ρ, the buyers are Σ_x ρ_x N_x. Projected by a Leslie
// matrix (demography's `projection`), they follow N(t); in a stable population they grow by λ a step exactly (§5.4, checked).
//
// DERIVED DEMAND. What industries buy of a product is a row of an input–output table (economics' `production`): z_ij, the deliveries of
// sector i to sector j, with the final demand y_i beside it; under another final demand y' it is a_ij x'_j with x' = L y' (§5.5).

#pragma once

#include <demography/projection/projection.hpp>
#include <economics/production/input_output.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace markets::sizing {

namespace detail {
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("markets::sizing: ") + what);
}
}  // namespace detail

struct Range {
  double low = 0, mid = 0, high = 0;
};

inline Range exactly(double v) { return {v, v, v}; }

struct Node {
  enum class Kind { leaf, sum, product };
  Kind kind = Kind::leaf;
  std::string name, source;  // a leaf says where it comes from
  bool assumed = false;      // a leaf that is an assumption, not a datum
  Range value;               // a leaf's
  std::vector<std::size_t> children;
};

struct Tree {
  std::vector<Node> nodes;

  std::size_t leaf(std::string name, Range value, std::string source = "", bool assumed = true) {
    detail::require(0 <= value.low && value.low <= value.mid && value.mid <= value.high && std::isfinite(value.high), "0 ≤ low ≤ mid ≤ high, finite");
    nodes.push_back({Node::Kind::leaf, std::move(name), std::move(source), assumed, value, {}});
    return nodes.size() - 1;
  }
  std::size_t sum(std::string name, std::vector<std::size_t> children) { return inner(Node::Kind::sum, std::move(name), std::move(children)); }
  std::size_t product(std::string name, std::vector<std::size_t> children) { return inner(Node::Kind::product, std::move(name), std::move(children)); }
  std::size_t root() const {
    detail::require(!nodes.empty(), "a tree with a node");
    return nodes.size() - 1;
  }

 private:
  std::size_t inner(Node::Kind kind, std::string name, std::vector<std::size_t> children) {
    detail::require(!children.empty(), "a sum or a product of something");
    for (std::size_t c : children) detail::require(c < nodes.size(), "children are made before the node that uses them");
    nodes.push_back({kind, std::move(name), "", false, {}, std::move(children)});
    return nodes.size() - 1;
  }
};

// The value of every node when each leaf takes the value `at` reads from its range.
template <typename At>
std::vector<double> values(const Tree& T, At at) {
  std::vector<double> v(T.nodes.size(), 0.0);
  for (std::size_t i = 0; i < T.nodes.size(); i++) {
    const Node& n = T.nodes[i];
    if (n.kind == Node::Kind::leaf) v[i] = at(i, n.value);
    else if (n.kind == Node::Kind::sum) for (std::size_t c : n.children) v[i] += v[c];
    else {
      v[i] = 1;
      for (std::size_t c : n.children) v[i] *= v[c];
    }
  }
  return v;
}

// Every node's range: exact, the lows and the highs attained together.
inline std::vector<Range> evaluate(const Tree& T) {
  const auto low = values(T, [](std::size_t, const Range& r) { return r.low; });
  const auto mid = values(T, [](std::size_t, const Range& r) { return r.mid; });
  const auto high = values(T, [](std::size_t, const Range& r) { return r.high; });
  std::vector<Range> out(T.nodes.size());
  for (std::size_t i = 0; i < out.size(); i++) out[i] = {low[i], mid[i], high[i]};
  return out;
}

// ε_ℓ = (x_ℓ / Y) ∂Y/∂x_ℓ at the mids, for every node (zero for those that are not leaves); Y is the root.
inline std::vector<double> elasticities(const Tree& T) {
  const auto v = values(T, [](std::size_t, const Range& r) { return r.mid; });
  std::vector<double> adjoint(T.nodes.size(), 0.0), out(T.nodes.size(), 0.0);
  const std::size_t root = T.root();
  detail::require(v[root] > 0, "a size that is not zero");
  adjoint[root] = 1;
  for (std::size_t i = T.nodes.size(); i-- > 0;) {
    const Node& n = T.nodes[i];
    if (n.kind == Node::Kind::sum) for (std::size_t c : n.children) adjoint[c] += adjoint[i];
    else if (n.kind == Node::Kind::product)
      for (std::size_t k = 0; k < n.children.size(); k++) {
        double others = 1;  // the product of the other factors, without dividing: a factor may be zero
        for (std::size_t j = 0; j < n.children.size(); j++)
          if (j != k) others *= v[n.children[j]];
        adjoint[n.children[k]] += adjoint[i] * others;
      }
    else out[i] = adjoint[i] * v[i] / v[root];
  }
  return out;
}

// The size with one leaf at its low and at its high, the others at their mids.
struct Swing {
  std::size_t leaf = 0;
  double low = 0, high = 0;
  double width() const { return high - low; }
};

// For every leaf, in the order of what each could change, the widest first.
inline std::vector<Swing> swings(const Tree& T) {
  std::vector<Swing> out;
  const std::size_t root = T.root();
  for (std::size_t l = 0; l < T.nodes.size(); l++) {
    if (T.nodes[l].kind != Node::Kind::leaf) continue;
    const auto at = [&](bool high) { return values(T, [&](std::size_t i, const Range& r) { return i == l ? (high ? r.high : r.low) : r.mid; })[root]; };
    out.push_back({l, at(false), at(true)});
  }
  std::stable_sort(out.begin(), out.end(), [](const Swing& a, const Swing& b) { return a.width() > b.width(); });
  return out;
}

// ---- a chain: TAM, SAM, SOM ----------------------------------------------------------------------------------------------------

// A base and shares in [0, 1], each stage the stage before times its share: stages[0] is the base (all who could buy).
inline std::vector<Range> chain(Range base, const std::vector<Range>& shares) {
  detail::require(0 <= base.low && base.low <= base.mid && base.mid <= base.high, "a base 0 ≤ low ≤ mid ≤ high");
  std::vector<Range> out{base};
  for (const Range& s : shares) {
    detail::require(0 <= s.low && s.low <= s.mid && s.mid <= s.high && s.high <= 1, "shares within 0 and 1, low ≤ mid ≤ high");
    const Range& last = out.back();
    out.push_back({last.low * s.low, last.mid * s.mid, last.high * s.high});
  }
  return out;
}

// ---- two estimates of one size ---------------------------------------------------------------------------------------------------

struct Reconciled {
  bool agree = false;
  double low = 0, high = 0;  // what both allow, when they agree
  double gap = 1;            // else the ratio of the nearer ends, > 1: by how much the two differ at least
};

inline Reconciled reconcile(const Range& a, const Range& b) {
  Reconciled r;
  r.low = std::max(a.low, b.low), r.high = std::min(a.high, b.high);
  r.agree = r.low <= r.high;
  if (!r.agree) r.gap = r.high > 0 ? r.low / r.high : INFINITY;
  return r;
}

// ---- buyers of a later year --------------------------------------------------------------------------------------------------------

// Σ_x ρ_x N_x.
inline double buyers(const std::vector<double>& population, const std::vector<double>& rate) {
  detail::require(population.size() == rate.size(), "a rate of buying for each age group");
  double total = 0;
  for (std::size_t x = 0; x < rate.size(); x++) {
    detail::require(rate[x] >= 0 && population[x] >= 0, "nonnegative rates and populations");
    total += rate[x] * population[x];
  }
  return total;
}

// The buyers now and after each of `steps` projections by the Leslie matrix (each a step of M.n years).
inline std::vector<double> buyers_ahead(const demography::projection::Leslie& M, std::vector<double> population, const std::vector<double>& rate, int steps) {
  detail::require(steps >= 0, "a number of steps");
  std::vector<double> out{buyers(population, rate)};
  for (int t = 0; t < steps; t++) {
    population = demography::projection::project(M, population);
    out.push_back(buyers(population, rate));
  }
  return out;
}

// ---- derived demand ----------------------------------------------------------------------------------------------------------------

// What is bought of sector i's product: by each sector, and by final demand.
struct Derived {
  std::vector<double> by_sector;
  double final_demand = 0;
  double intermediate() const {
    double t = 0;
    for (double v : by_sector) t += v;
    return t;
  }
  double total() const { return intermediate() + final_demand; }
};

// As the table records it: z_ij and y_i. Its total is the sector's output.
inline Derived derived_demand(const economics::InputOutputTable& T, std::size_t i) {
  detail::require(i < T.n(), "a sector of the table");
  Derived d;
  for (std::size_t j = 0; j < T.n(); j++) d.by_sector.push_back(T.z(i, j));
  d.final_demand = T.final_demand[i];
  return d;
}

// Under another final demand y: a_ij x_j with x = L y.
inline Derived derived_demand(const economics::LeontiefModel& M, std::size_t i, const std::vector<double>& y) {
  detail::require(i < M.L.n && y.size() == M.L.n, "a sector of the model, and a final demand for each");
  const auto x = economics::output_for(M, y);
  Derived d;
  for (std::size_t j = 0; j < M.L.n; j++) d.by_sector.push_back(M.A(i, j) * x[j]);
  d.final_demand = y[i];
  return d;
}

}  // namespace markets::sizing
