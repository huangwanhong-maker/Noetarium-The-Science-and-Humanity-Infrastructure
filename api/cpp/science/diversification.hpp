// The structure of regional economies, as one composition any work may call (ADR 0166): the economics domain's `diversification` on a
// table of regions' employment by industry at two dates. Nothing in it names a region.
//
//   diversification.assess {table: {regions, references, industries, groups, before: [[…]], after: [[…]]},
//                           threshold?, base?, opportunities?}
//                          each region's shift-share against its reference, variety at both dates (entropy, unrelated, related), diversity,
//                          complexity, entries, exits, withheld cells and opportunities; each industry's complexity at the second date;
//                          the areas under the curve of density for entries and exits
//
// A withheld cell is null.

#pragma once

#include <economics/diversification/structure.hpp>

#include <science/json.hpp>

#include <cmath>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::diversification {

namespace ed = economics::diversification;

inline double num(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline std::vector<std::string> texts(const Json& a) {
  std::vector<std::string> out;
  for (const auto& x : a.as_array()) out.push_back(x.text(""));
  return out;
}
inline std::vector<std::vector<double>> rows(const Json& a) {
  std::vector<std::vector<double>> out;
  for (const auto& row : a.as_array()) {
    out.emplace_back();
    for (const auto& v : row.as_array()) out.back().push_back(v.is_number() ? v.as_number() : ed::unknown);
  }
  return out;
}

inline Json variety(const nomos::measurement::Variety& v) {
  return Json::object({{"entropy", v.entropy}, {"unrelated", v.unrelated}, {"related", v.related}});
}
inline Json ranks(const tyche::testing::RankSum& r) {
  return Json::object({{"auc", r.auc}, {"statistic", r.statistic}, {"p_value", r.p_value}, {"n1", static_cast<double>(r.n1)}, {"n2", static_cast<double>(r.n2)}});
}

inline Json assess(const Json& r) {
  const Json& t = r["table"];
  ed::Table T;
  T.regions = texts(t["regions"]), T.references = texts(t["references"]), T.industries = texts(t["industries"]), T.groups = texts(t["groups"]);
  T.before = rows(t["before"]), T.after = rows(t["after"]);
  ed::Options o;
  o.threshold = num(r["threshold"], 1), o.base = num(r["base"], 2), o.opportunities = static_cast<std::size_t>(num(r["opportunities"], 3));
  const auto A = ed::assess(T, o);
  Json regions = Json::array();
  for (const auto& G : A.regions) {
    Json opp = Json::array();
    for (const auto& [k, v] : G.opportunities) opp.push(Json::object({{"industry", k}, {"density", v}}));
    regions.push(Json::object({{"region", G.name}, {"reference", G.reference}, {"before", G.total_before}, {"after", G.total_after},
                               {"withheld_before", G.withheld_before}, {"withheld_after", G.withheld_after}, {"national", G.national}, {"mix", G.mix},
                               {"competitive", G.competitive}, {"competitive_effect", G.competitive_effect}, {"allocation", G.allocation},
                               {"variety_before", variety(G.variety_before)}, {"variety_after", variety(G.variety_after)},
                               {"diversity_before", G.diversity_before}, {"diversity_after", G.diversity_after}, {"complexity_before", G.complexity_before},
                               {"complexity_after", G.complexity_after}, {"entries", G.entries}, {"exits", G.exits}, {"opportunities", opp}}));
  }
  Json industries = Json::array();
  for (std::size_t p = 0; p < T.industries.size(); p++)
    industries.push(Json::object({{"industry", T.industries[p]}, {"complexity", p < A.industry_complexity.size() ? A.industry_complexity[p] : ed::unknown}}));
  return Json::object({{"regions", regions}, {"industries", industries}, {"complexity_before", A.complexity_before}, {"complexity_after", A.complexity_after},
                       {"complexity_note", A.complexity_note}, {"entry", ranks(A.entry)}, {"exit", ranks(A.exit)},
                       {"candidates", static_cast<double>(A.candidates)}, {"entries", static_cast<double>(A.entries)}, {"holdings", static_cast<double>(A.holdings)},
                       {"exits", static_cast<double>(A.exits)}});
}

inline bool handles(const std::string& op) { return op.rfind("diversification.", 0) == 0; }

inline Json run(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "diversification.assess") return assess(r);
  throw std::invalid_argument("diversification: no operation " + op);
}

}  // namespace science::diversification
