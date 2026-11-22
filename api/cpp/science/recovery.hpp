// Indirect losses over a recovery, as one composition any work may call (ADR 0158): the economics domain's `recovery` (the static and
// dynamic inoperability input–output models) on a country's symmetric input–output table, read as the tourism composition reads it.
//
//   recovery.path {table: {products, domestic, final_domestic, gva, taxes, imported, households_domestic_share}, units?,
//                  first?: {product: inoperability}, demand?: [{from, to, lost: {product: final demand lost a year}}],
//                  recovery_periods: {product: periods} | number, remaining?, periods, periods_a_year, track?}
//                 each sector's output and value added lost over the path, with the interdependencies and alone, its mean inoperability
//                 and the period it recovers; the totals and the indirect losses; the static inoperability of the first period's lost
//                 demand; the output lost over an unbounded horizon (Proposition 4), with and without the interdependencies; the paths
//                 of the `track` sectors losing most (6 by default)
//
// A sector's lost final demand is a rate a year in the spending's money (the table's over `units`, as outputs are read); its
// inoperability c*_i is that over its planned output. The first inoperabilities are capacity lost when the path begins.

#pragma once

#include <economics/recovery/inoperability.hpp>

#include <science/json.hpp>
#include <science/tourism.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::recovery {

namespace er = economics::recovery;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline Json path(const Json& r) {
  const double units = num(r["units"], 1.0);
  const auto A = science::tourism::accounts_of(r["table"], units);
  const auto I = er::interdependency(A.domestic);
  const std::size_t n = I.sectors.size();
  std::map<std::string, std::size_t> at;
  for (std::size_t i = 0; i < n; i++) at[I.sectors[i]] = i;
  const auto index = [&](const std::string& p) {
    const auto f = at.find(p);
    if (f == at.end()) throw std::invalid_argument("recovery: no product " + p + " with output in the table");
    return f->second;
  };
  std::vector<double> q0(n, 0.0);
  if (r["first"].is_object())
    for (const auto& [p, v] : r["first"].as_object()) q0[index(p)] = v.as_number();
  const int periods = static_cast<int>(num(r["periods"], 365));
  const double ppy = num(r["periods_a_year"], 365);
  std::vector<std::vector<double>> c(static_cast<std::size_t>(periods), std::vector<double>(n, 0.0));
  if (r["demand"].is_array())
    for (const auto& d : r["demand"].as_array()) {
      const int from = static_cast<int>(num(d["from"], 0)), to = static_cast<int>(num(d["to"], periods));
      if (from < 0 || to < from) throw std::invalid_argument("recovery: a perturbation runs from a period to a later one");
      for (const auto& [p, v] : d["lost"].as_object()) {
        const std::size_t i = index(p);
        for (int t = from; t < std::min(to, periods); t++) c[t][i] += v.as_number() / I.output[i];
      }
    }
  std::vector<double> T(n, num(r["recovery_periods"], 90));
  if (r["recovery_periods"].is_object()) {
    std::fill(T.begin(), T.end(), num(r["recovery_periods"]["default"], 90));
    for (const auto& [p, v] : r["recovery_periods"].as_object())
      if (p != "default") T[index(p)] = v.as_number();
  }
  const auto k = er::resilience(I, T, num(r["remaining"], 0.01));
  const auto P = er::recover(I, q0, k, c, periods, ppy);
  const auto q_static = er::static_inoperability(I, c.empty() ? std::vector<double>(n, 0.0) : c[0]);
  std::vector<double> c_sum(n, 0.0);
  for (const auto& ct : c)
    for (std::size_t i = 0; i < n; i++) c_sum[i] += ct[i];
  const auto U = er::unbounded_loss(I, q0, k, c_sum, ppy);
  double unbounded = 0, unbounded_alone = 0;
  for (std::size_t i = 0; i < n; i++) unbounded += U[i], unbounded_alone += (c_sum[i] + q0[i] / k[i]) * I.output[i] / ppy;
  std::vector<double> va(n);
  double va_total = 0, va_alone = 0;
  Json sectors = Json::array();
  for (std::size_t i = 0; i < n; i++) {
    const double ratio = A.gva[i] / I.output[i];
    va[i] = P.loss[i] * ratio;
    va_total += va[i], va_alone += P.alone[i] * ratio;
  }
  const auto by_loss = er::ranking(P.loss), by_q = er::ranking(P.mean);
  std::vector<std::size_t> rank_q(n);
  for (std::size_t k2 = 0; k2 < n; k2++) rank_q[by_q[k2]] = k2 + 1;
  for (std::size_t k2 = 0; k2 < n; k2++) {
    const std::size_t i = by_loss[k2];
    if (!(P.loss[i] > 0)) continue;
    sectors.push(Json::object({{"product", I.sectors[i]}, {"loss", P.loss[i]}, {"alone", P.alone[i]}, {"value_added_lost", va[i]}, {"mean_inoperability", P.mean[i]},
                               {"first", q0[i]}, {"static", q_static[i]}, {"recovered", static_cast<double>(P.recovered[i])}, {"rank_by_loss", static_cast<double>(k2 + 1)},
                               {"rank_by_inoperability", static_cast<double>(rank_q[i])}, {"resilience", k[i]}}));
  }
  const std::size_t track = static_cast<std::size_t>(num(r["track"], 6));
  Json paths = Json::array();
  for (std::size_t k2 = 0; k2 < std::min(track, n); k2++) {
    const std::size_t i = by_loss[k2];
    if (!(P.loss[i] > 0)) continue;
    Json q = Json::array();
    for (const auto& qt : P.q) q.push(Json(qt[i]));
    paths.push(Json::object({{"product", I.sectors[i]}, {"q", q}}));
  }
  return Json::object({{"total", P.total}, {"alone", P.total_alone}, {"indirect", P.total - P.total_alone}, {"unbounded", unbounded}, {"unbounded_alone", unbounded_alone}, {"value_added_lost", va_total},
                       {"value_added_lost_alone", va_alone}, {"largest_row", I.largest_row}, {"sectors", sectors}, {"paths", paths}});
}

inline bool handles(const std::string& op) { return op.rfind("recovery.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "recovery.path") return path(r);
  throw std::invalid_argument("recovery: no operation " + op);
}

}  // namespace science::recovery
