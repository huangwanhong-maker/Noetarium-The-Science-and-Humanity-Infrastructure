// A local or regional government's finances as one composition any work may call (ADR 0145): the economics domain's `fiscal`,
// read and answered as JSON. Every threshold, rate, floor and ceiling in a request is the requester's declaration.
//
//   fiscal.assess  {accounts, ratios, thresholds, projection, capacity}
//                  each year's totals by category and its ratios judged against the scheme; the years ahead (totals grown at
//                  declared rates, the surplus, the debt service of the loans, the coverage, the stock of debt); and the largest
//                  new annuity keeping the coverage at its floor, bounded by a ceiling on the stock where one is declared
//   fiscal.debt    {d0, r, g, pb, record: {r, g, pb}, block, draws, threshold, quantiles, seed}
//                  the path of debt as a share of its base, the balance that stabilises it now, and the fan of paths drawn in blocks
//                  of the record's joint deviations, with the share above the threshold each year
//   fiscal.effort  {governments: [names], bases: [[...]], revenues: [[...]], base_names}
//                  the representative rates, each government's capacity and effort
//
// accounts {unit, currency, chart: [{code, name, category}], years: [{year, amounts: {code: amount}}]}; ratios [{name, numerator:
// [{category, sign}], denominator: [...]}]; thresholds [{ratio, sound: at_least | at_most, warning, intervention}]; projection
// {base_year, years, growth: {category: rate}, surplus: [terms], loans: [{name, principal, rate, first_year, tenor, bullet}]};
// capacity {rate, tenor, first_year, floor, revenue: [terms], ceiling}.
#pragma once

#include <economics/fiscal/fiscal.hpp>

#include <science/json.hpp>
#include <science/scorecards.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::fiscal {

namespace ef = economics::fiscal;
using science::scorecards::list;

inline double num(const Json& j, double fallback = ef::none) { return j.is_number() ? j.as_number() : fallback; }
inline Json maybe(double x) { return std::isfinite(x) ? Json(x) : Json(); }

inline std::vector<double> numbers(const Json& j) {
  std::vector<double> v;
  for (const auto& x : list(j)) v.push_back(x.as_number());
  return v;
}

inline std::vector<ef::Term> terms_of(const Json& j) {
  std::vector<ef::Term> t;
  for (const auto& x : list(j)) t.push_back({x["category"].text(), num(x["sign"], 1)});
  return t;
}

inline ef::Accounts accounts_of(const Json& j) {
  ef::Accounts a;
  a.unit = j["unit"].text(), a.currency = j["currency"].text();
  for (const auto& l : list(j["chart"])) a.chart.push_back({l["code"].text(), l["name"].text(), l["category"].text()});
  for (const auto& y : list(j["years"])) {
    ef::Year year;
    year.year = static_cast<int>(y["year"].as_number());
    if (y["amounts"].is_object())
      for (const auto& [k, v] : y["amounts"].as_object()) year.amounts[k] = v.as_number();
    a.years.push_back(year);
  }
  if (a.years.empty()) throw std::invalid_argument("fiscal: accounts of at least one year");
  return a;
}

inline Json totals_json(const ef::Totals& t) {
  Json o = Json::object();
  for (const auto& [k, v] : t) o.set(k, v);
  return o;
}

inline Json assess_op(const Json& q) {
  const auto a = accounts_of(q["accounts"]);
  std::vector<ef::Ratio> rs;
  for (const auto& r : list(q["ratios"])) rs.push_back({r["name"].text(), terms_of(r["numerator"]), terms_of(r["denominator"])});
  std::vector<ef::Threshold> scheme;
  for (const auto& t : list(q["thresholds"]))
    scheme.push_back({t["ratio"].text(), t["sound"].text("at_least") == "at_most" ? ef::Sound::at_most : ef::Sound::at_least, num(t["warning"]), num(t["intervention"])});
  Json years = Json::array();
  for (const auto& y : a.years) years.push(Json::object({{"year", static_cast<double>(y.year)}, {"totals", totals_json(ef::totals(a, y))}}));
  Json ratios = Json::array();
  for (const auto& v : ef::ratios(a, rs, scheme))
    ratios.push(Json::object({{"ratio", v.ratio}, {"year", static_cast<double>(v.year)}, {"value", maybe(v.value)}, {"status", std::string(ef::name(v.status))}, {"why", v.why}}));
  Json out = Json::object({{"unit", a.unit}, {"currency", a.currency}, {"years", years}, {"ratios", ratios}});
  if (q.has("projection") && !q["projection"].is_null()) {
    const Json& p = q["projection"];
    const int base_year = static_cast<int>(num(p["base_year"], a.years.back().year));
    const ef::Year* base = nullptr;
    for (const auto& y : a.years)
      if (y.year == base_year) base = &y;
    if (!base) throw std::invalid_argument("fiscal: the projection's base year is not among the accounts' years");
    std::map<std::string, double> growth;
    if (p["growth"].is_object())
      for (const auto& [k, v] : p["growth"].as_object()) growth[k] = v.as_number();
    std::vector<ef::Loan> loans;
    for (const auto& l : list(p["loans"]))
      loans.push_back({l["name"].text(), num(l["principal"], 0), num(l["rate"], 0), static_cast<int>(num(l["first_year"], base_year + 1)), static_cast<int>(num(l["tenor"], 1)),
                       l.has("bullet") && l["bullet"].as_bool()});
    const int years_ahead = static_cast<int>(num(p["years"], 5));
    if (years_ahead < 1 || years_ahead > 60) throw std::invalid_argument("fiscal: between one and sixty years ahead");
    const auto pr = ef::project(ef::totals(a, *base), base_year, years_ahead, growth, terms_of(p["surplus"]), loans);
    Json rows = Json::array();
    for (std::size_t k = 0; k < pr.years.size(); k++)
      rows.push(Json::object({{"year", static_cast<double>(pr.years[k])}, {"surplus", pr.surplus[k]}, {"debt_service", pr.debt_service[k]}, {"coverage", maybe(pr.coverage[k])},
                              {"outstanding", pr.outstanding[k]}, {"totals", totals_json(pr.totals[k])}}));
    out.set("projection", rows);
    if (q.has("capacity") && !q["capacity"].is_null()) {
      const Json& c = q["capacity"];
      const auto cap = ef::debt_capacity(pr, num(c["rate"], 0.05), static_cast<int>(num(c["tenor"], 10)), static_cast<int>(num(c["first_year"], base_year + 1)), num(c["floor"], 1.2),
                                         terms_of(c["revenue"]), num(c["ceiling"]));
      out.set("capacity", Json::object({{"largest", cap.largest}, {"binding_year", static_cast<double>(cap.binding_year)}, {"binding", cap.binding},
                                         {"payment_per_unit", cap.payment_per_unit}}));
    }
  }
  return out;
}

inline Json debt_op(const Json& q) {
  const auto r = numbers(q["r"]), g = numbers(q["g"]), pb = numbers(q["pb"]);
  const double d0 = num(q["d0"]);
  if (!std::isfinite(d0)) throw std::invalid_argument("fiscal: the debt to start from, d0");
  const auto path = ef::debt_path(d0, r, g, pb);
  Json p = Json::array();
  for (double x : path) p.push(x);
  Json out = Json::object({{"path", p}, {"stabilising_balance", r.empty() ? Json() : Json(ef::stabilising_balance(d0, r[0], g[0]))}});
  if (q.has("record") && !q["record"].is_null()) {
    std::vector<double> qs = numbers(q["quantiles"]);
    if (qs.empty()) qs = {0.1, 0.5, 0.9};
    const auto draws = static_cast<std::size_t>(num(q["draws"], 2000));
    if (draws > 100000) throw std::invalid_argument("fiscal: at most 100,000 paths");
    const auto f = ef::fan(d0, r, g, pb, numbers(q["record"]["r"]), numbers(q["record"]["g"]), numbers(q["record"]["pb"]), static_cast<std::size_t>(num(q["block"], 2)), draws,
                           num(q["threshold"], INFINITY), qs, static_cast<std::uint64_t>(num(q["seed"], 1)));
    Json at = Json::array(), above = Json::array(), qj = Json::array();
    for (const auto& row : f.paths_at) {
      Json r_ = Json::array();
      for (double x : row) r_.push(x);
      at.push(r_);
    }
    for (double x : f.above) above.push(x);
    for (double x : qs) qj.push(x);
    out.set("fan", Json::object({{"quantiles", qj}, {"paths_at", at}, {"above", above}, {"threshold", maybe(num(q["threshold"]))}}));
  }
  return out;
}

inline Json effort_op(const Json& q) {
  std::vector<std::vector<double>> bases, revenues;
  for (const auto& row : list(q["bases"])) bases.push_back(numbers(row));
  for (const auto& row : list(q["revenues"])) revenues.push_back(numbers(row));
  const auto e = ef::representative_tax_system(bases, revenues);
  Json rates = Json::array(), cap = Json::array(), eff = Json::array();
  for (double x : e.rates) rates.push(x);
  for (double x : e.capacity) cap.push(x);
  for (double x : e.effort) eff.push(maybe(x));
  return Json::object({{"rates", rates}, {"capacity", cap}, {"effort", eff}, {"governments", q["governments"]}, {"base_names", q["base_names"]}});
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "fiscal.assess") return assess_op(q);
  if (op == "fiscal.debt") return debt_op(q);
  if (op == "fiscal.effort") return effort_op(q);
  throw std::invalid_argument("fiscal: no operation \"" + op + "\"");
}

}  // namespace science::fiscal
