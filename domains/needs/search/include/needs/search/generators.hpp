// Generators of statements (ADR 0125; the theory note §4): the ways new statements enter a search, each without a language model.
//
//   enumeration    the language breadth first: every group of one condition, then of two, … with every indicator
//   depth-first    the language depth first: a group, then its refinements, before its siblings
//   random         groups of one to the largest size, conditions drawn from the pool
//   beam           subgroup discovery by beam search (Klösgen 1996; Wrobel 1997): a group's quality is its gap from the frame
//                  times its share of the frame's people to a power a (a = 1/2 and 1 by turns), kept to the best of a width
//                  at each depth
//   tree           the leaves of a regression tree of the indicator over the attributes (Breiman et al. 1984), the worst first
//   rules          the groups of the declared normative rules, with the indicators of their domains
//   simulation     subgroup discovery on the share of demand that the seeking model left unmet, place by place
//
// All read the discovery part only. They are deterministic given their cursors and the key: enumeration and depth-first walk the
// language from where they stopped, and a search saves their cursors with its state. A language model's proposals enter by the
// evolution's seam for proposals, not here (the API's science.needs).
#pragma once

#include <needs/evidence/rules.hpp>
#include <needs/evidence/simulation.hpp>
#include <needs/search/space.hpp>

#include <calx/random/stream.hpp>

#include <algorithm>
#include <memory>
#include <set>

namespace needs::search {

// An indicator's rows on the discovery part, as the sums a quality needs: y and x per row (weighted), and the frame's totals.
struct Lens {
  std::vector<std::size_t> rows;   // discovery rows with a value
  std::vector<double> y, x;        // per row of `rows`
  double sy = 0, sx = 0, sign = 1;
};

inline Lens lens(const Dataset& d, const Parts& parts, const Statement& s) {
  Lens L;
  const auto cols = evidence::detail::columns(d, s);
  L.sign = s.worse_when_higher ? 1 : -1;
  const groups::Rows frame = groups::rows_of(d.table, s.frame);
  for (std::size_t i = 0; i < d.table.rows; i++) {
    if (parts.part_of_row[i] != 0 || !frame[i]) continue;
    double y, x, v;
    if (!evidence::detail::row(cols, i, y, x, v)) continue;
    const double w = d.table.w(i);
    L.rows.push_back(i), L.y.push_back(w * y), L.x.push_back(w * x), L.sy += w * y, L.sx += w * x;
  }
  return L;
}

// A lens on a target given per row (y, x already weighted): for the simulation's unmet demand.
inline Lens lens_of(const Parts& parts, const std::vector<double>& y, const std::vector<double>& x) {
  Lens L;
  for (std::size_t i = 0; i < y.size(); i++) {
    if (parts.part_of_row[i] != 0 || !(x[i] > 0)) continue;
    L.rows.push_back(i), L.y.push_back(y[i]), L.x.push_back(x[i]), L.sy += y[i], L.sx += x[i];
  }
  return L;
}

struct Scored {
  Group group;
  double quality = 0, coverage = 0, value = 0;
};

// Beam search over conjunctions of the pool's conditions, on distinct attributes.
inline std::vector<Scored> beam(const Space& sp, const Lens& L, double a, std::size_t width, std::size_t depth) {
  struct Node {
    std::vector<std::size_t> conds;
    std::vector<std::size_t> idx;  // positions in L.rows
  };
  std::vector<Scored> found;
  std::set<std::string> seen;
  if (!(L.sx > 0)) return found;
  const double frame_value = L.sy / L.sx;
  std::vector<Node> current{{{}, {}}};
  current[0].idx.resize(L.rows.size());
  for (std::size_t k = 0; k < L.rows.size(); k++) current[0].idx[k] = k;
  for (std::size_t level = 0; level < depth; level++) {
    std::vector<std::pair<double, Node>> next;
    std::set<std::vector<std::size_t>> reached;  // a conjunction is a set: refined from two members of the beam, it is kept once
    for (const auto& node : current) {
      std::set<std::size_t> used;
      for (std::size_t c : node.conds) used.insert(sp.attribute_of[c]);
      for (std::size_t c = 0; c < sp.conditions.size(); c++) {
        if (used.count(sp.attribute_of[c])) continue;
        auto conds = node.conds;
        conds.push_back(c);
        std::sort(conds.begin(), conds.end());
        if (!reached.insert(conds).second) continue;
        double y = 0, x = 0;
        Node child;
        for (std::size_t k : node.idx)
          if (sp.rows[c][L.rows[k]]) y += L.y[k], x += L.x[k], child.idx.push_back(k);
        if (!(x >= sp.min_coverage) || !(x > 0)) continue;
        const double q = std::pow(x / L.sx, a) * L.sign * (y / x - frame_value);
        child.conds = std::move(conds);
        next.push_back({q, std::move(child)});
      }
    }
    std::sort(next.begin(), next.end(), [](const auto& u, const auto& v) { return u.first > v.first; });
    current.clear();
    for (auto& [q, node] : next) {
      Group g;
      for (std::size_t c : node.conds) g.conditions.push_back(sp.conditions[c]);
      if (!seen.insert(g.key()).second) continue;
      double x = 0, y = 0;
      for (std::size_t k : node.idx) x += L.x[k], y += L.y[k];
      found.push_back({g, q, x, y / x});
      if (current.size() < width) current.push_back(std::move(node));
      if (current.size() >= width && found.size() > width * (level + 2)) break;
    }
    if (current.empty()) break;
  }
  std::sort(found.begin(), found.end(), [](const Scored& u, const Scored& v) { return u.quality > v.quality; });
  return found;
}

inline Condition negation(const Condition& c) {
  Condition n = c;
  switch (c.op) {
    case groups::Op::le: n.op = groups::Op::gt; break;
    case groups::Op::gt: n.op = groups::Op::le; break;
    case groups::Op::lt: n.op = groups::Op::ge; break;
    case groups::Op::ge: n.op = groups::Op::lt; break;
    case groups::Op::eq: n.op = groups::Op::ne; break;
    case groups::Op::ne: n.op = groups::Op::eq; break;
    default: break;
  }
  return n;
}

// A regression tree of the indicator's per-row value (weighted by the people each row stands for), its leaves the worst first.
inline std::vector<Scored> tree(const Space& sp, const Lens& L, std::size_t depth) {
  std::vector<Scored> leaves;
  if (!(L.sx > 0)) return leaves;
  const double overall = L.sy / L.sx;
  struct Job {
    std::vector<std::size_t> idx;
    Group path;
    std::set<std::size_t> used;
    std::size_t depth;
  };
  std::vector<Job> stack;
  Job root;
  root.idx.resize(L.rows.size());
  for (std::size_t k = 0; k < L.rows.size(); k++) root.idx[k] = k;
  root.depth = 0;
  stack.push_back(std::move(root));
  while (!stack.empty()) {
    Job j = std::move(stack.back());
    stack.pop_back();
    double ty = 0, tx = 0;
    for (std::size_t k : j.idx) ty += L.y[k], tx += L.x[k];
    double best = 0;
    std::size_t bc = sp.conditions.size();
    if (j.depth < depth) {
      for (std::size_t c = 0; c < sp.conditions.size(); c++) {
        if (j.used.count(sp.attribute_of[c]) || sp.conditions[c].op == groups::Op::gt) continue;  // ≤ and > split alike
        double ly = 0, lx = 0;
        for (std::size_t k : j.idx)
          if (sp.rows[c][L.rows[k]]) ly += L.y[k], lx += L.x[k];
        const double rx = tx - lx, ry = ty - ly;
        if (lx < sp.min_coverage || rx < sp.min_coverage) continue;
        const double gain = lx * rx / tx * std::pow(ly / lx - ry / rx, 2);
        if (gain > best) best = gain, bc = c;
      }
    }
    if (bc == sp.conditions.size()) {
      if (!j.path.conditions.empty()) leaves.push_back({j.path, L.sign * (ty / tx - overall), tx, ty / tx});
      continue;
    }
    Job left, right;
    for (std::size_t k : j.idx) (sp.rows[bc][L.rows[k]] ? left : right).idx.push_back(k);
    left.path = j.path, right.path = j.path;
    left.path.conditions.push_back(sp.conditions[bc]);
    right.path.conditions.push_back(negation(sp.conditions[bc]));
    left.used = right.used = j.used;
    left.used.insert(sp.attribute_of[bc]), right.used.insert(sp.attribute_of[bc]);
    left.depth = right.depth = j.depth + 1;
    stack.push_back(std::move(left)), stack.push_back(std::move(right));
  }
  std::sort(leaves.begin(), leaves.end(), [](const Scored& u, const Scored& v) { return u.quality > v.quality; });
  return leaves;
}

// The generators' shared state: the space, the caches of the heuristic generators, and the cursors of the enumerations.
struct Generators {
  const Dataset* d = nullptr;
  const Parts* parts = nullptr;
  const Space* sp = nullptr;
  const evidence::Rulebook* rules = nullptr;
  std::map<std::string, evidence::Field> fields;   // indicator → what the seeking model left, where it applies
  std::uint64_t key = 1;
  std::size_t width = 24;

  // Cursors, saved with a search's state.
  std::vector<std::size_t> bfs{1};                 // [size, indicator, conditions…]
  std::vector<std::size_t> dfs;                    // [indicator, conditions…]
  std::map<std::string, std::size_t> handed;       // per cached list, how many were handed out

  std::map<std::string, std::vector<Statement>> lists;  // beam, tree, rules, simulation: computed once

  Statement statement(const Group& g, const std::string& indicator) const { return plain(*sp, *d, g, indicator); }

  // The next combination of pool indices on distinct attributes, of the same size, after `c` (lexicographic); false at the end.
  bool next_same_size(std::vector<std::size_t>& c) const {
    const std::size_t n = sp->conditions.size(), k = c.size();
    for (;;) {
      std::size_t i = k;
      while (i > 0 && c[i - 1] == n - k + i - 1) i--;
      if (i == 0) return false;
      c[i - 1]++;
      for (std::size_t j = i; j < k; j++) c[j] = c[j - 1] + 1;
      if (valid(c)) return true;
    }
  }
  bool valid(const std::vector<std::size_t>& c) const {
    std::set<std::size_t> a;
    for (std::size_t x : c)
      if (!a.insert(sp->attribute_of[x]).second) return false;
    return true;
  }
  Group group_of(const std::vector<std::size_t>& c) const {
    Group g;
    for (std::size_t x : c) g.conditions.push_back(sp->conditions[x]);
    return g;
  }

  std::vector<Statement> enumeration(std::size_t n) {
    std::vector<Statement> out;
    const std::size_t I = sp->indicators.size(), N = sp->conditions.size();
    if (bfs.size() < 2) bfs = {1, 0, 0};
    while (out.size() < n) {
      std::size_t k = bfs[0];
      if (k > sp->max_conditions || k > N) break;
      std::vector<std::size_t> c(bfs.begin() + 2, bfs.end());
      if (c.size() != k) {
        c.resize(k);
        for (std::size_t j = 0; j < k; j++) c[j] = j;
        if (!valid(c) && !next_same_size(c)) { bfs = {k + 1, 0}; continue; }
        bfs.resize(2), bfs.insert(bfs.end(), c.begin(), c.end());
      }
      out.push_back(statement(group_of(c), sp->indicators[bfs[1]]));
      if (++bfs[1] < I) continue;
      bfs[1] = 0;
      if (next_same_size(c)) bfs.resize(2), bfs.insert(bfs.end(), c.begin(), c.end());
      else bfs = {k + 1, 0};
    }
    return out;
  }

  std::vector<Statement> depth_first(std::size_t n) {
    std::vector<Statement> out;
    const std::size_t I = sp->indicators.size(), N = sp->conditions.size();
    if (dfs.empty()) dfs = {0, 0};
    while (out.size() < n && dfs.size() > 1) {
      std::vector<std::size_t> c(dfs.begin() + 1, dfs.end());
      out.push_back(statement(group_of(c), sp->indicators[dfs[0]]));
      if (++dfs[0] < I) continue;
      dfs[0] = 0;
      // The next node of the subset tree in preorder: go deeper, else to the next sibling, else back up.
      bool moved = false;
      if (c.size() < sp->max_conditions) {
        for (std::size_t x = c.back() + 1; x < N && !moved; x++) {
          auto d2 = c;
          d2.push_back(x);
          if (valid(d2)) c = d2, moved = true;
        }
      }
      while (!moved && !c.empty()) {
        std::size_t last = c.back();
        c.pop_back();
        for (std::size_t x = last + 1; x < N && !moved; x++) {
          auto d2 = c;
          d2.push_back(x);
          if (valid(d2)) c = d2, moved = true;
        }
      }
      dfs.resize(1);
      if (moved) dfs.insert(dfs.end(), c.begin(), c.end());
    }
    return out;
  }

  std::vector<Statement> random(std::size_t n, calx::random::Stream& s) {
    std::vector<Statement> out;
    for (std::size_t t = 0; out.size() < n && t < 20 * n; t++) {
      const std::size_t k = 1 + s.below(sp->max_conditions);
      std::vector<std::size_t> c;
      for (std::size_t tries = 0; c.size() < k && tries < 50; tries++) {
        const std::size_t x = s.below(sp->conditions.size());
        auto c2 = c;
        c2.push_back(x);
        if (valid(c2)) c = c2;
      }
      std::sort(c.begin(), c.end());
      out.push_back(statement(group_of(c), sp->indicators[s.below(sp->indicators.size())]));
    }
    return out;
  }

  const std::vector<Statement>& list(const std::string& which) {
    auto it = lists.find(which);
    if (it != lists.end()) return it->second;
    std::vector<Statement>& out = lists[which];
    if (which == "beam") {
      // Round robin over indicators and the two powers, best first within each.
      std::vector<std::vector<Statement>> per;
      for (const auto& ind : sp->indicators)
        for (double a : {0.5, 1.0}) {
          const Statement base = statement({}, ind);
          std::vector<Statement> v;
          for (const auto& sc : beam(*sp, lens(*d, *parts, base), a, width, sp->max_conditions)) v.push_back(statement(sc.group, ind));
          per.push_back(std::move(v));
        }
      interleave(per, out);
    } else if (which == "tree") {
      std::vector<std::vector<Statement>> per;
      for (const auto& ind : sp->indicators) {
        std::vector<Statement> v;
        for (const auto& sc : tree(*sp, lens(*d, *parts, statement({}, ind)), sp->max_conditions)) v.push_back(statement(sc.group, ind));
        per.push_back(std::move(v));
      }
      interleave(per, out);
    } else if (which == "rules" && rules) {
      for (const auto& r : rules->rules) {
        std::vector<std::string> of;  // the indicators of the rule's domain, or of a domain it reaches; all, when none is
        for (const auto& ind : sp->indicators) {
          const std::size_t to = rules->index(sp->domain(ind));
          if (sp->domain(ind) == r.domain || (to < rules->domains.size() && rules->reaches[rules->index(r.domain)][to])) of.push_back(ind);
        }
        if (of.empty()) of = sp->indicators;
        for (const auto& ind : of) {
          Statement s = statement(r.group, ind);
          s.frame = r.frame;
          if (!s.group.conditions.empty()) out.push_back(s);
        }
      }
    } else if (which == "simulation") {
      std::vector<std::vector<Statement>> per;
      for (const auto& [ind, f] : fields) {
        if (!f.applicable) continue;
        const auto rd = evidence::detail::demand_of_rows(*d, ind);
        std::vector<double> y(d->table.rows, 0.0), x(d->table.rows, 0.0);
        for (std::size_t i = 0; i < d->table.rows; i++) {
          if (f.place_of_row[i] < 0) continue;
          double u = 0;
          for (const auto& rep : f.unmet) u += rep[static_cast<std::size_t>(f.place_of_row[i])] / static_cast<double>(f.unmet.size());
          y[i] = rd[i] * u, x[i] = rd[i];
        }
        std::vector<Statement> v;
        for (const auto& sc : beam(*sp, lens_of(*parts, y, x), 0.5, width, sp->max_conditions)) v.push_back(statement(sc.group, ind));
        per.push_back(std::move(v));
      }
      interleave(per, out);
    }
    return out;
  }

  static void interleave(std::vector<std::vector<Statement>>& per, std::vector<Statement>& out) {
    std::set<std::string> seen;
    for (std::size_t r = 0;; r++) {
      bool any = false;
      for (auto& v : per)
        if (r < v.size()) {
          any = true;
          if (!v[r].group.conditions.empty() && seen.insert(v[r].key()).second) out.push_back(v[r]);
        }
      if (!any) break;
    }
  }

  std::vector<Statement> from_list(const std::string& which, std::size_t n) {
    const auto& v = list(which);
    std::vector<Statement> out;
    std::size_t& h = handed[which];
    while (out.size() < n && !v.empty()) {
      out.push_back(v[h % v.size()]);
      h++;
      if (h >= 4 * v.size() + n) break;
    }
    return out;
  }

  std::vector<Statement> make(const std::string& which, std::size_t n, calx::random::Stream& s) {
    if (which == "enumeration") return enumeration(n);
    if (which == "depth-first") return depth_first(n);
    if (which == "random") return random(n, s);
    if (which == "beam" || which == "tree" || which == "rules" || which == "simulation") return from_list(which, n);
    throw Refusal("no generator '" + which + "' (there are enumeration, depth-first, random, beam, tree, rules, simulation)");
  }
};

inline const std::vector<std::string>& generator_names() {
  static const std::vector<std::string> n{"enumeration", "depth-first", "random", "beam", "tree", "rules", "simulation"};
  return n;
}

}  // namespace needs::search
