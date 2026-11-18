// Causal support for a statement (ADR 0125; the theory note §3.4): whether, within the group, more of the domain's provision goes
// with less of the gap's indicator, once the confounders a declared causal graph names are held fixed.
//
// The graph is the dataset's, over its columns; the provision of a domain is a column the dataset names. The adjustment set is the
// smallest back-door set among the observed columns (peira's graphs: Pearl's criterion, enumerated). The effect is the coefficient
// of provision in a weighted least-squares regression of the units' values (a value, or a share's successes over its total,
// weighted by the people each stands for) on provision and the adjustment set, a category entered by its levels, with standard
// errors robust to the clusters (tyche's regression).
//
// A need that provision would meet has an effect of the expected sign (less need where more is provided). The verdict:
//   holds:      the interval excludes zero on the expected side;
//   fails:      it excludes zero on the other side (more provision where there is more need: provision follows need, or the
//               graph is wrong);
//   undecided:  otherwise, or when the graph gives no adjustment set, or the group is too small.
//
// Caveat. The test is of the declared graph, not of the world; an effect estimated across units that differ in more than the graph
// says is confounded however small its interval.
#pragma once

#include <needs/evidence/parts.hpp>
#include <needs/language/statement.hpp>

#include <peira/graphs/causal.hpp>
#include <tyche/regression/linear.hpp>

#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace needs::evidence {

enum class Verdict { holds, fails, undecided };

inline const char* name(Verdict v) { return v == Verdict::holds ? "holds" : v == Verdict::fails ? "fails" : "undecided"; }

struct Causal {
  bool applicable = false;             // the dataset names a provision for the domain and a graph
  Verdict verdict = Verdict::undecided;
  std::string why;
  std::string provision, outcome;
  std::vector<std::string> adjustment;
  double effect = std::numeric_limits<double>::quiet_NaN(), se = effect, low = effect, high = effect, p = effect;
  std::size_t rows = 0, clusters = 0;
};

inline Causal causal(const Dataset& d, const language::Statement& s, const Parts& parts, Part part, double confidence = 0.95) {
  Causal out;
  auto it = d.provision.find(s.domain);
  if (it == d.provision.end() || d.graph.empty() || s.standard == language::Standard::expressed) {
    out.why = it == d.provision.end() ? "the data name no provision for " + s.domain : d.graph.empty() ? "the data declare no causal graph" : "an expressed need is its provision's shortfall already";
    return out;
  }
  out.applicable = true;
  const auto& ind = d.indicator(s.indicator);
  out.provision = it->second, out.outcome = ind.column;
  // The graph over names.
  std::map<std::string, std::size_t> id;
  auto node = [&](const std::string& n) {
    auto f = id.find(n);
    if (f == id.end()) f = id.emplace(n, id.size()).first;
    return f->second;
  };
  for (const auto& a : d.graph) node(a.from), node(a.to);
  if (!id.count(out.provision) || !id.count(out.outcome)) {
    out.why = "the graph does not hold both " + out.provision + " and " + out.outcome;
    return out;
  }
  peira::graphs::Dag g(id.size());
  for (const auto& [n, k] : id) g.names[k] = n;
  for (const auto& a : d.graph) g.arrow(id[a.from], id[a.to]);
  if (!peira::graphs::acyclic(g)) {
    out.why = "the declared graph has a cycle";
    return out;
  }
  std::vector<std::size_t> observed;
  for (const auto& [n, k] : id)
    if (d.table.find(n)) observed.push_back(k);
  std::vector<std::set<std::size_t>> sets;
  try {
    sets = peira::graphs::adjustment_sets(g, id[out.provision], id[out.outcome], observed);
  } catch (const std::exception& e) {
    out.why = e.what();
    return out;
  }
  if (sets.empty()) {
    out.why = "the graph gives no back-door set among the observed columns: the effect is not identified";
    return out;
  }
  for (std::size_t k : sets.front()) out.adjustment.push_back(g.names[k]);
  // The rows of the group in the part, with their values.
  const auto& t = d.table;
  const groups::Rows frame = groups::rows_of(t, s.frame);
  const groups::Rows rows = groups::rows_of(t, s.group, frame);
  const auto& yc = t.at(ind.column).numbers;
  const std::vector<double>* xc = ind.total.empty() ? nullptr : &t.at(ind.total).numbers;
  const auto& pc = t.at(out.provision).numbers;
  std::vector<std::size_t> use;
  for (std::size_t i = 0; i < t.rows; i++) {
    if (!rows[i] || parts.part_of_row[i] != static_cast<int>(part)) continue;
    const double x = xc ? (*xc)[i] : 1.0;
    if (!std::isfinite(yc[i]) || !std::isfinite(pc[i]) || !(x > 0) || !(t.w(i) > 0)) continue;
    bool ok = true;
    for (const auto& a : out.adjustment) {
      const auto& col = t.at(a);
      ok = ok && (col.kind == groups::Kind::number ? std::isfinite(col.numbers[i]) : col.codes[i] >= 0);
    }
    if (ok) use.push_back(i);
  }
  std::vector<std::vector<double>> columns{{}};
  std::vector<double> y, w;
  std::vector<std::int64_t> cl;
  std::set<int> clusters;
  for (std::size_t i : use) {
    const double x = xc ? (*xc)[i] : 1.0;
    y.push_back(ind.share ? yc[i] / x : yc[i]);
    w.push_back(t.w(i) * x);
    columns[0].push_back(pc[i]);
    cl.push_back(parts.cluster_of_row[i]);
    clusters.insert(parts.cluster_of_row[i]);
  }
  for (const auto& a : out.adjustment) {
    const auto& col = t.at(a);
    if (col.kind == groups::Kind::number) {
      std::vector<double> v;
      for (std::size_t i : use) v.push_back(col.numbers[i]);
      columns.push_back(v);
    } else {
      for (std::size_t l = 1; l < col.levels.size(); l++) {  // the first level is the base
        std::vector<double> v;
        for (std::size_t i : use) v.push_back(col.codes[i] == static_cast<int>(l) ? 1.0 : 0.0);
        columns.push_back(v);
      }
    }
  }
  out.rows = use.size(), out.clusters = clusters.size();
  if (use.size() < columns.size() + 3 || clusters.size() < 3) {
    out.why = "too few units of the group in the part to estimate the effect";
    return out;
  }
  try {
    tyche::regression::OlsOptions o;
    o.weights = w, o.covariance = tyche::regression::Covariance::cluster, o.clusters = cl, o.confidence = confidence, o.drop_collinear = true;
    const auto fit = tyche::regression::ols(tyche::regression::design_matrix(columns, true), y, o);
    out.effect = fit.beta[1], out.se = fit.se[1], out.low = fit.ci_lower[1], out.high = fit.ci_upper[1], out.p = fit.p[1];
  } catch (const std::exception& e) {
    out.why = e.what();
    return out;
  }
  if (!std::isfinite(out.effect)) {
    out.why = "provision does not vary within the group, given the adjustment";
    return out;
  }
  const bool expected_negative = ind.higher_is_worse;
  const bool below = out.high < 0, above = out.low > 0;
  if ((expected_negative && below) || (!expected_negative && above)) out.verdict = Verdict::holds, out.why = "more provision goes with less need, given the adjustment";
  else if ((expected_negative && above) || (!expected_negative && below)) out.verdict = Verdict::fails, out.why = "more provision goes with more need: provision may follow need, or the graph is wrong";
  else out.why = "the interval holds zero";
  return out;
}

}  // namespace needs::evidence
