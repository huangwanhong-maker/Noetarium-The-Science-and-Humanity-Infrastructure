// Selection among firms: the decompositions of aggregate productivity growth of the literature, each composed on tyche's census record
// and its Price decomposition (ADR 0165; theory note theory/selection-among-firms.md).
//
// A panel holds firms at two dates, each with an identity, a weight (employment, output, value added: the weight that makes the
// aggregate) and a productivity (in logs or in levels, as the caller states), and optionally its industry. The aggregate is the
// weighted mean Φ = Σ s_i φ_i. Firms present at both dates survive; firms present only after enter; firms present only before exit. The
// change ΔΦ is decomposed exactly by each method, every one an identity whatever the data:
//
//   bhc    Baily, Hulten and Campbell (1992): within Σ_S s_i Δφ_i, between Σ_S Δs_i φ'_i, entry Σ_E s'_i φ'_i, exit −Σ_X s_i φ_i
//          (Melitz and Polanec 2015, eq. 3); not invariant to a constant added to every productivity, so that its entry and exit move
//          with the level of productivity;
//   gr     Griliches and Regev (1995): within Σ_S s̄_i Δφ_i, between Σ_S Δs_i (φ̄_i − Φ̄), entry Σ_E s'_i (φ'_i − Φ̄), exit
//          −Σ_X s_i (φ_i − Φ̄), with bars the averages over the two dates (tyche's average form);
//   fhk    Foster, Haltiwanger and Krizan (2001): within Σ_S s_i Δφ_i, between Σ_S Δs_i (φ_i − Φ), cross Σ_S Δs_i Δφ_i, entry
//          Σ_E s'_i (φ'_i − Φ), exit −Σ_X s_i (φ_i − Φ) (tyche's incumbent form);
//   dopd   Melitz and Polanec (2015), the dynamic Olley–Pakes decomposition: ΔΦ = (Φ'_S − Φ_S) + s'_E (Φ'_E − Φ'_S) + s_X (Φ_S − Φ_X),
//          the survivors' change split by Olley and Pakes (1996) into the change of their unweighted mean and of their covariance
//          Σ_S (s_i/s_S − 1/n_S)(φ_i − φ̄_S) (tyche's survivors' form, with Olley and Pakes's split);
//   price  Price's equation over the survivors, selection between industries and within them, and transmission (learning), with entry
//          and exit as in dopd (tyche's survivors' form with industries as its level).
//
// The methods differ in what they call entry: where every firm's productivity grows by γ, dopd credits entry and exit with nothing, gr
// with s_E γ/2 and s_X γ/2, fhk with s_E γ and nothing (Melitz and Polanec 2015, §2). Decompositions over successive intervals do not add
// to the decomposition over the whole span (Holm 2014, §4), and selection read from revenue productivity confounds efficiency with
// prices (Foster, Haltiwanger and Syverson 2008). Every term is an accounting, never a cause. Nothing here names an industry or a
// country; the number type is a template, double or morphe's exact rationals.

#pragma once

#include <tyche/census/price.hpp>

#include <cmath>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace economics::selection {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("economics::selection: " + what);
}
}  // namespace detail

template <class T = double>
struct Firm {
  std::string id;
  T weight{};        // employment, output or value added at that date
  T productivity{};  // in the scale the caller states (logs or levels)
  std::string industry;
};

template <class T = double>
struct Panel {
  std::vector<Firm<T>> before, after;
};

enum class Method { bhc, gr, fhk, dopd, price };

inline const char* name(Method m) {
  switch (m) {
    case Method::bhc: return "Baily, Hulten and Campbell";
    case Method::gr: return "Griliches and Regev";
    case Method::fhk: return "Foster, Haltiwanger and Krizan";
    case Method::dopd: return "Melitz and Polanec (dynamic Olley–Pakes)";
    case Method::price: return "Price's equation over the survivors";
  }
  return "";
}

template <class T = double>
struct Result {
  Method method = Method::dopd;
  T before{}, after{}, change{};                   // Φ, Φ', ΔΦ
  std::vector<std::pair<std::string, T>> terms;    // named terms summing to ΔΦ
  T entering_share{}, exiting_share{};             // s'_E, s_X
  std::size_t survivors = 0, entrants = 0, exiters = 0;
  T term(const std::string& n) const {
    for (const auto& [k, v] : terms)
      if (k == n) return v;
    throw std::invalid_argument("economics::selection: no term " + n);
  }
};

// The panel as tyche's census record, firms keyed by identity; industries as the one declared level of the census before.
template <class T>
tyche::census::Record<T> record(const Panel<T>& p) {
  detail::require(!p.before.empty() && !p.after.empty(), "a panel has firms at both dates");
  tyche::census::Census<T> b, a;
  std::vector<std::string> ib, ia;
  bool industries = false;
  for (const auto& f : p.before) industries = industries || !f.industry.empty();
  for (const auto& f : p.before) {
    b.weight.push_back(f.weight), b.trait.push_back(f.productivity), ib.push_back(f.id);
    if (industries) b.levels.push_back({f.industry});
  }
  for (const auto& f : p.after) a.weight.push_back(f.weight), a.trait.push_back(f.productivity), ia.push_back(f.id);
  return tyche::census::by_identity<T>(ib, b, ia, a, "entry", "exit", "survives");
}

// Olley and Pakes's static decomposition of one cross-section: Φ = φ̄ + Σ (s_i − 1/n)(φ_i − φ̄).
template <class T = double>
struct OlleyPakes {
  T aggregate{}, unweighted_mean{}, covariance{};
};

template <class T>
OlleyPakes<T> olley_pakes(const std::vector<T>& weight, const std::vector<T>& productivity) {
  detail::require(weight.size() == productivity.size() && !weight.empty(), "a weight and a productivity for every firm");
  T W{};
  for (const auto& w : weight) detail::require(!(w < T(0)), "weights are not negative"), W += w;
  detail::require(T(0) < W, "a positive total weight");
  OlleyPakes<T> o;
  const T n(static_cast<long long>(weight.size()));
  for (std::size_t i = 0; i < weight.size(); i++) o.unweighted_mean += productivity[i], o.aggregate += weight[i] / W * productivity[i];
  o.unweighted_mean = o.unweighted_mean / n;
  for (std::size_t i = 0; i < weight.size(); i++) o.covariance += (weight[i] / W - T(1) / n) * (productivity[i] - o.unweighted_mean);
  return o;
}

template <class T>
Result<T> decompose(const Panel<T>& p, Method m) {
  using tyche::census::Form;
  const auto r = record(p);
  Result<T> R;
  R.method = m;
  std::set<std::string> ids_after;
  for (const auto& f : p.after) ids_after.insert(f.id);
  std::set<std::string> ids_before;
  for (const auto& f : p.before) ids_before.insert(f.id);
  for (const auto& f : p.before) (ids_after.count(f.id) ? R.survivors : R.exiters)++;
  for (const auto& f : p.after) R.entrants += ids_before.count(f.id) ? 0 : 1;
  auto finish = [&](const tyche::census::Decomposition<T>& D) {
    R.before = D.before_mean, R.after = D.after_mean, R.change = D.total;
    R.entering_share = D.entering_share, R.exiting_share = D.exiting_share;
  };
  switch (m) {
    case Method::bhc: {
      const auto D = tyche::census::decompose(r, Form::declared, T(0));
      finish(D);
      R.terms = {{"within", D.within}, {"between", D.selection + D.cross}, {"entry", D.entry}, {"exit", D.exit}};
      break;
    }
    case Method::gr: {
      const auto D = tyche::census::decompose(r, Form::average);
      finish(D);
      R.terms = {{"within", D.within}, {"between", D.selection}, {"entry", D.entry}, {"exit", D.exit}};
      break;
    }
    case Method::fhk: {
      const auto D = tyche::census::decompose(r, Form::incumbent);
      finish(D);
      R.terms = {{"within", D.within}, {"between", D.selection}, {"cross", D.cross}, {"entry", D.entry}, {"exit", D.exit}};
      break;
    }
    case Method::dopd: {
      const auto D = tyche::census::decompose(r, Form::survivors);
      finish(D);
      std::vector<T> w0, z0, w1, z1;
      std::map<std::string, std::size_t> at;
      for (std::size_t j = 0; j < p.after.size(); j++) at[p.after[j].id] = j;
      for (const auto& f : p.before)
        if (at.count(f.id)) {
          const auto& g = p.after[at[f.id]];
          w0.push_back(f.weight), z0.push_back(f.productivity), w1.push_back(g.weight), z1.push_back(g.productivity);
        }
      const auto o0 = olley_pakes(w0, z0), o1 = olley_pakes(w1, z1);
      R.terms = {{"survivors' unweighted mean", o1.unweighted_mean - o0.unweighted_mean},
                 {"survivors' covariance", o1.covariance - o0.covariance},
                 {"entry", D.entry},
                 {"exit", D.exit}};
      break;
    }
    case Method::price: {
      const auto D = tyche::census::decompose(r, Form::survivors);
      finish(D);
      if (D.selection_by_level.size() == 2) {
        R.terms = {{"selection between industries", D.selection_by_level[0]}, {"selection within industries", D.selection_by_level[1]}};
      } else {
        R.terms = {{"selection", D.selection}};
      }
      R.terms.push_back({"learning", D.transmission});
      R.terms.push_back({"entry", D.entry});
      R.terms.push_back({"exit", D.exit});
      break;
    }
  }
  return R;
}

// A panel in levels carried to logs: each productivity replaced by its natural logarithm (positive productivities only).
inline Panel<double> logs(const Panel<double>& p) {
  Panel<double> q = p;
  for (auto* side : {&q.before, &q.after})
    for (auto& f : *side) {
      detail::require(f.productivity > 0, "a productivity in levels is positive: " + f.id);
      f.productivity = std::log(f.productivity);
    }
  return q;
}

}  // namespace economics::selection
