// A region's own input–output table, as one composition any work may call (ADR 0159): the economics domain's `regional` on a
// country's symmetric table and the region's shares of national employment by product. The answer is a table in the form the tourism
// and recovery compositions read, so that their accounts and paths are the region's.
//
//   regional.table {table: {products, domestic, final_domestic, gva, taxes, imported, households_domestic_share}, units?,
//                   shares: {product: the region's share of national employment in its industry}, size: the region's share of all
//                   employment, quotient?: "slq" | "cilq" | "flq" | "aflq", delta?}
//                  the region's table: flows within it, its final use as the rest of each row, value added, taxes and imports at the
//                  national coefficients, its purchases from the rest of the country (`rest`), households' local and rest-of-country
//                  shares; with each product's SLQ, λ*, the output multipliers of the country and the region, and the products whose
//                  local use exceeds their output (a negative final use, which location quotients can give)

#pragma once

#include <economics/regional/regional.hpp>

#include <science/json.hpp>
#include <science/tourism.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::regional {

namespace eg = economics::regional;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline Json table(const Json& r) {
  const double units = num(r["units"], 1.0);
  const auto A = science::tourism::accounts_of(r["table"], units);  // products of no output left out
  const std::size_t n = A.domestic.n();
  eg::Region R;
  R.size = num(r["size"], 0);
  R.share.resize(n);
  for (std::size_t i = 0; i < n; i++) {
    const auto& v = r["shares"][A.domestic.sectors[i]];
    if (!v.is_number()) throw std::invalid_argument("regional: no employment share for " + A.domestic.sectors[i]);
    R.share[i] = v.as_number();
  }
  const std::string qs = r["quotient"].text("flq");
  const eg::Quotient q = qs == "slq" ? eg::Quotient::slq : qs == "cilq" ? eg::Quotient::cilq : qs == "aflq" ? eg::Quotient::aflq : eg::Quotient::flq;
  if (qs != "slq" && qs != "cilq" && qs != "flq" && qs != "aflq") throw std::invalid_argument("regional: a quotient is slq, cilq, flq or aflq, not " + qs);
  const auto G = eg::regionalise(A.domestic, R, q, num(r["delta"], 0.3));
  const auto x = A.domestic.output();
  const auto local = eg::local_share(A.domestic_share, G.slq);
  Json products = Json::array(), domestic = Json::array(), final = Json::array(), gva = Json::array(), taxes = Json::array(), imported = Json::array(),
       rest = Json::array(), share = Json::array(), rest_share = Json::array(), slq = Json::array(), output = Json::array();
  Json negative = Json::array();
  for (std::size_t i = 0; i < n; i++) {
    products.push(Json(A.domestic.sectors[i]));
    Json row = Json::array();
    for (std::size_t j = 0; j < n; j++) row.push(Json(G.local.z(i, j) * units));
    domestic.push(row);
    final.push(Json(G.local.final_demand[i] * units));
    const double f = x[i] > 0 ? G.output[i] / x[i] : 0;
    gva.push(Json(A.gva[i] * f * units)), taxes.push(Json(A.taxes[i] * f * units)), imported.push(Json(A.imported[i] * f * units));
    rest.push(Json(G.rest[i] * units));
    share.push(Json(local[i])), rest_share.push(Json(A.domestic_share[i] - local[i]));
    slq.push(Json(G.slq[i])), output.push(Json(G.output[i] * units));
    if (G.local.final_demand[i] < 0) negative.push(Json(A.domestic.sectors[i]));
  }
  const auto Mn = economics::leontief(A.domestic);
  double mn = 0, mr = 0, xn = 0, xr = 0;
  bool positive = true;
  for (std::size_t i = 0; i < n; i++) positive = positive && G.output[i] > 0;
  if (positive) {
    const auto Mr = economics::leontief(G.local);
    for (std::size_t i = 0; i < n; i++) mn += Mn.multipliers[i] * x[i], mr += Mr.multipliers[i] * G.output[i], xn += x[i], xr += G.output[i];
  }
  return Json::object({{"products", products}, {"domestic", domestic}, {"final_domestic", final}, {"gva", gva}, {"taxes", taxes}, {"imported", imported},
                       {"rest", rest}, {"households_domestic_share", share}, {"households_rest_share", rest_share}, {"slq", slq}, {"output", output},
                       {"lambda", G.lambda}, {"quotient", qs}, {"negative_final", negative},
                       {"multiplier_country", xn > 0 ? Json(mn / xn) : Json()}, {"multiplier_region", xr > 0 ? Json(mr / xr) : Json()}});
}

// regional.two {the same request}: the region and the rest of its country as one table of 2n products ("r:" and "s:" before each code),
// each region's coefficients by its own location quotient and the rest bought from the other (ADR 0160); with `two_region`, which the
// tourism composition reads to split a purchase made in the region among the region, the rest of the country and abroad.
inline Json two(const Json& r) {
  const double units = num(r["units"], 1.0);
  const auto A = science::tourism::accounts_of(r["table"], units);
  const std::size_t n = A.domestic.n();
  eg::Region R;
  R.size = num(r["size"], 0);
  R.share.resize(n);
  for (std::size_t i = 0; i < n; i++) {
    const auto& v = r["shares"][A.domestic.sectors[i]];
    if (!v.is_number()) throw std::invalid_argument("regional: no employment share for " + A.domestic.sectors[i]);
    R.share[i] = v.as_number();
  }
  const std::string qs = r["quotient"].text("flq");
  if (qs != "slq" && qs != "cilq" && qs != "flq" && qs != "aflq") throw std::invalid_argument("regional: a quotient is slq, cilq, flq or aflq, not " + qs);
  const eg::Quotient q = qs == "slq" ? eg::Quotient::slq : qs == "cilq" ? eg::Quotient::cilq : qs == "aflq" ? eg::Quotient::aflq : eg::Quotient::flq;
  const auto W = eg::two_region(A.domestic, R, q, num(r["delta"], 0.3));
  const auto x = A.domestic.output();
  const auto local = eg::local_share(A.domestic_share, W.slq_region);
  Json products = Json::array(), domestic = Json::array(), final = Json::array(), gva = Json::array(), taxes = Json::array(), imported = Json::array(),
       ones = Json::array(), output = Json::array(), negative = Json::array();
  for (std::size_t i = 0; i < 2 * n; i++) {
    const std::size_t b = i % n;
    products.push(Json(W.table.sectors[i]));
    Json row = Json::array();
    for (std::size_t j = 0; j < 2 * n; j++) row.push(Json(W.table.z(i, j) * units));
    domestic.push(row);
    final.push(Json(W.table.final_demand[i] * units));
    const double f = x[b] > 0 ? W.output[i] / x[b] : 0;
    gva.push(Json(A.gva[b] * f * units)), taxes.push(Json(A.taxes[b] * f * units)), imported.push(Json(A.imported[b] * f * units));
    ones.push(Json(1.0)), output.push(Json(W.output[i] * units));
  }
  for (const auto i : W.negative_final) negative.push(Json(W.table.sectors[i]));
  Json base = Json::array(), national = Json::array(), loc = Json::array(), slq_r = Json::array(), slq_s = Json::array();
  for (std::size_t i = 0; i < n; i++) {
    base.push(Json(A.domestic.sectors[i])), national.push(Json(A.domestic_share[i])), loc.push(Json(local[i]));
    slq_r.push(Json(W.slq_region[i])), slq_s.push(Json(W.slq_rest[i]));
  }
  return Json::object({{"products", products}, {"domestic", domestic}, {"final_domestic", final}, {"gva", gva}, {"taxes", taxes}, {"imported", imported},
                       {"households_domestic_share", ones}, {"output", output}, {"negative_final", negative}, {"quotient", qs},
                       {"lambda_region", W.lambda_region}, {"lambda_rest", W.lambda_rest}, {"slq_region", slq_r}, {"slq_rest", slq_s},
                       {"two_region", Json::object({{"base", base}, {"national_share", national}, {"local_share", loc}})}});
}

inline bool handles(const std::string& op) { return op.rfind("regional.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "regional.table") return table(r);
  if (op == "regional.two") return two(r);
  throw std::invalid_argument("regional: no operation " + op);
}

}  // namespace science::regional
