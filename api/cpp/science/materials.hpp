// Materials and flows, for any work's engine (ADR 0106): one composition of the industrial-ecology domain, answering requests in JSON.
// What a stock, a system of flows, a product system or an economy is, is given in the request by whoever holds the data; nothing here
// names a city, a database or a file's columns.
//
//   stock       {"mode": "inflow" | "stock", "values": [...], "lifetime": L, "lifetimes"?: [L, ...] (one per period), "initial"?: {"by_age":
//                [...], "lifetime": L}, "correct"?: true, "intensity"?: {"materials": [...], "lower": [[...]], "central": [[...]], "upper":
//                [[...]], "initial_lower"?…}, "cohorts"?: false}
//                 a dynamic stock model: inflow, stock, outflow, net addition, what was retired early, the mean age, the balance, and the
//                 materials held and moved with their bounds
//   buildings   {"scheme": S, "buildings": {"id", "region", "use", "structure", "floor_area", "year"} (columns), "group"?: [...],
//                "per_building"?: false, "renewal"?: {"year": y, "years": n, "lifetime": L, "growth"?: g, "path"?: [...], "max_age"?: 150,
//                "new"?: {"lower": [...], "central": [...], "upper": [...]}}}
//                 the material stock of buildings by group with bounds, the buildings no row matched, and the flows that would renew the
//                 stock over the years asked: cohorts by age at `year` as the initial stock, a floor-area path (kept, grown by g a year,
//                 or given), new buildings at the intensity stated or else at that of the last ten years' buildings, said to be
//   reconcile   {"processes": [...], "flows": [{"name", "from": process | null, "to": process | null, "stock"?: false, "value"?: v, "sd"?: s}]}
//                 measured flows (sd > 0), exact (sd = 0) and unknown (no value) reconciled to balance, with the χ² test, each measurement's
//                 normalised adjustment, which unknowns the balances determine, and the links of a Sankey diagram
//   transfer    {"processes": [...], "shares": [[...]], "accumulation": [...], "imports": [...]}   steady flows by transfer coefficients
//   lca         {"products", "processes", "interventions", "A": [[...]], "B": [[...]], "method": {"categories", "units", "C": [[...]]},
//                "demand": [...], "partition"?: [{"process", "factors": {product: f}}], "substitute"?: [{"process", "product", "by"}],
//                "monte_carlo"?: {"gsd_A", "gsd_B", "draws", "seed"}, "paths"?: {"category"?: 0, "cutoff"?: 1e-3}}
//                 the scaling, inventory, impacts and each process's contribution; the largest elasticities; a Monte Carlo spread; the
//                 structural paths of one category
//   footprint   {"regions", "sectors", "Z": [[...]], "Y": [[...]] (one column per region), "stressors", "units"?, "F": [[...]], "F_Y"?: [[...]],
//                "characterise"?: {"categories", "units", "C": [[...]]}, "region"?: r, "paths"?: {"stressor"?: 0, "cutoff"?: 1e-3},
//                "categories"?: [...], "direct"?: category}
//                 production- and consumption-based accounts by region, what trade embodies, each stressor's origin–destination matrix,
//                 the multipliers, a region's footprint by product, and its leading supply chains; for one region whose Y has one
//                 column per category of final demand (households, government, capital, exports), the footprint of each category and
//                 each category's by product, F_Y given to the category named `direct` (by default the first)
//   accounts    {"supply_use"?: T, "economy_wide"?: E}      a physical supply–use table's balances; economy-wide material flow indicators
//
// A lifetime law L is {"law": "fixed", "years"} | {"law": "exponential", "mean"} | {"law": "normal", "mean", "sd"} | {"law": "weibull",
// "shape", "scale"} or {"law": "weibull", "shape", "mean"} | {"law": "lognormal", "mean", "sd"} | {"law": "gamma", "shape", "scale"} |
// {"law": "table", "ages", "survival"}. A scheme S is {"name", "materials", "rows": [{"region"?, "use"?, "structure"?, "from"?, "to"?,
// "lower", "central", "upper", "source"?}]}. Units are the request's: whatever a flow, an area or an intensity is measured in comes back.

#pragma once

#include <industrial_ecology/accounts/accounts.hpp>
#include <industrial_ecology/buildings/buildings.hpp>
#include <industrial_ecology/eeio/eeio.hpp>
#include <industrial_ecology/flows/flows.hpp>
#include <industrial_ecology/lca/lca.hpp>
#include <industrial_ecology/stocks/stocks.hpp>

#include "json.hpp"

#include <climits>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::materials {

namespace ie = ::industrial_ecology;
using calx::linalg::Matrix;
using calx::linalg::Rectangular;

// ---------------------------------------------------------------- reading and writing

inline double num(const Json& j) { return j.is_number() ? j.as_number() : NAN; }
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(num(a[i]));
  return v;
}
inline std::vector<std::string> texts(const Json& a) {
  std::vector<std::string> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(a[i].is_string() ? a[i].as_string() : std::string());
  return v;
}
inline Json written(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(std::isfinite(x) ? Json(x) : Json());
  return a;
}
inline Json written(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(Json(x));
  return a;
}
inline Rectangular rows_of(const Json& a, std::size_t cols, const char* what) {
  if (!a.is_array()) throw std::invalid_argument(std::string(what) + ": a table of rows");
  Rectangular X(a.size(), cols);
  for (std::size_t i = 0; i < a.size(); i++) {
    const auto v = numbers(a[i]);
    if (v.size() != cols) throw std::invalid_argument(std::string(what) + ": every row has one number per column");
    for (std::size_t k = 0; k < cols; k++) {
      if (!std::isfinite(v[k])) throw std::invalid_argument(std::string(what) + ": a missing number");
      X(i, k) = v[k];
    }
  }
  return X;
}
inline std::vector<std::vector<double>> nested(const Json& a) {
  std::vector<std::vector<double>> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(numbers(a[i]));
  return v;
}
inline Json table_json(const Rectangular& t) {
  Json a = Json::array();
  for (std::size_t i = 0; i < t.rows; i++) a.push(written(t.row(i)));
  return a;
}
inline Json table_json(const Matrix& t) {
  Json a = Json::array();
  for (std::size_t i = 0; i < t.n; i++) {
    std::vector<double> row(t.n);
    for (std::size_t j = 0; j < t.n; j++) row[j] = t(i, j);
    a.push(written(row));
  }
  return a;
}
inline std::size_t index_of(const std::vector<std::string>& names, const std::string& name, const char* what) {
  for (std::size_t i = 0; i < names.size(); i++)
    if (names[i] == name) return i;
  throw std::invalid_argument(std::string(what) + ": no such name, " + name);
}

// ---------------------------------------------------------------- stocks

inline ie::stocks::Lifetime lifetime_of(const Json& L) {
  const std::string law = L["law"].text();
  if (law == "fixed") return ie::stocks::fixed(num(L["years"]));
  if (law == "exponential") return ie::stocks::exponential(num(L["mean"]));
  if (law == "normal") return ie::stocks::normal(num(L["mean"]), num(L["sd"]));
  if (law == "weibull") {
    if (L.has("mean")) {
      const auto w = tyche::probability::Weibull::with_mean(num(L["mean"]), num(L["shape"]));
      return ie::stocks::weibull(w.shape, w.scale);
    }
    return ie::stocks::weibull(num(L["shape"]), num(L["scale"]));
  }
  if (law == "lognormal") return ie::stocks::lognormal(num(L["mean"]), num(L["sd"]));
  if (law == "gamma") return ie::stocks::gamma(num(L["shape"]), num(L["scale"]));
  if (law == "table") return ie::stocks::table(numbers(L["ages"]), numbers(L["survival"]));
  throw std::invalid_argument("a lifetime law is fixed, exponential, normal, weibull, lognormal, gamma or table");
}

inline Json bounded_json(const ie::stocks::Bounded& b) {
  return Json::object({{"lower", written(b.lower)}, {"central", written(b.central)}, {"upper", written(b.upper)}});
}

inline Json model_json(const ie::stocks::Model& M, bool cohorts) {
  Json out = Json::object({{"inflow", written(M.inflow)},
                           {"stock", written(M.stock)},
                           {"outflow", written(M.outflow)},
                           {"net_addition", written(M.net_addition)},
                           {"retired_early", written(M.retired_early)},
                           {"mean_age", written(M.mean_age)},
                           {"initial_stock", written(M.initial_stock)},
                           {"initial_total", Json(M.initial_total)},
                           {"balance", Json(M.balance)}});
  if (cohorts) {
    Json c = Json::array();
    for (const auto& row : M.by_cohort) c.push(written(row));
    out.set("by_cohort", c);
  }
  return out;
}

inline Json materials_json(const std::vector<ie::stocks::MaterialFlows>& mats) {
  Json a = Json::array();
  for (const auto& m : mats)
    a.push(Json::object({{"material", Json(m.material)}, {"stock", bounded_json(m.stock)}, {"inflow", bounded_json(m.inflow)}, {"outflow", bounded_json(m.outflow)}}));
  return a;
}

inline Json op_stock(const Json& r) {
  const auto values = numbers(r["values"]);
  if (values.empty()) throw std::invalid_argument("stock: values for at least one period");
  std::vector<ie::stocks::Lifetime> laws;
  if (r.has("lifetimes")) for (std::size_t i = 0; i < r["lifetimes"].size(); i++) laws.push_back(lifetime_of(r["lifetimes"][i]));
  else laws.push_back(lifetime_of(r["lifetime"]));
  ie::stocks::Initial init;
  const bool has_initial = r.has("initial");
  if (has_initial) init = {numbers(r["initial"]["by_age"]), lifetime_of(r["initial"]["lifetime"])};
  const std::string mode = r["mode"].text("inflow");
  ie::stocks::Model M;
  if (mode == "inflow") M = ie::stocks::inflow_driven(values, laws, has_initial ? &init : nullptr);
  else if (mode == "stock") M = ie::stocks::stock_driven(values, laws, has_initial ? &init : nullptr, !r.has("correct") || r["correct"].as_bool());
  else throw std::invalid_argument("stock: the mode is \"inflow\" or \"stock\"");
  Json out = model_json(M, r.has("cohorts") && r["cohorts"].as_bool());
  out.set("laws", Json(laws.size() == 1 ? laws[0].law : std::to_string(laws.size()) + " laws, one per period"));
  if (r.has("intensity")) {
    const Json& I = r["intensity"];
    ie::stocks::Intensity in;
    in.materials = texts(I["materials"]);
    in.lower = nested(I["lower"]), in.central = nested(I["central"]), in.upper = nested(I["upper"]);
    in.initial_lower = nested(I["initial_lower"]), in.initial_central = nested(I["initial_central"]), in.initial_upper = nested(I["initial_upper"]);
    out.set("materials", materials_json(ie::stocks::materials(M, in)));
  }
  return out;
}

// ---------------------------------------------------------------- buildings

inline ie::buildings::Scheme scheme_of(const Json& S) {
  ie::buildings::Scheme s;
  s.name = S["name"].text("a scheme");
  s.materials = texts(S["materials"]);
  for (std::size_t i = 0; i < S["rows"].size(); i++) {
    const Json& row = S["rows"][i];
    ie::buildings::Row r;
    r.region = row["region"].text(), r.use = row["use"].text(), r.structure = row["structure"].text();
    r.from = row.has("from") && row["from"].is_number() ? static_cast<int>(row["from"].as_number()) : INT_MIN;
    r.to = row.has("to") && row["to"].is_number() ? static_cast<int>(row["to"].as_number()) : INT_MAX;
    r.lower = numbers(row["lower"]), r.central = numbers(row["central"]), r.upper = numbers(row["upper"]);
    r.source = row["source"].text();
    s.rows.push_back(r);
  }
  s.validate();
  return s;
}

inline std::vector<ie::buildings::Building> buildings_of(const Json& B) {
  const auto area = numbers(B["floor_area"]);
  const std::size_t n = area.size();
  auto col = [&](const char* key) {
    auto v = texts(B[key]);
    if (v.empty()) v.assign(n, std::string());
    if (v.size() != n) throw std::invalid_argument(std::string("buildings: the column ") + key + " has one entry per building");
    return v;
  };
  const auto id = col("id"), region = col("region"), use = col("use"), structure = col("structure");
  auto year = numbers(B["year"]);
  if (year.empty()) year.assign(n, NAN);
  if (year.size() != n) throw std::invalid_argument("buildings: one year per building, or none");
  std::vector<ie::buildings::Building> bs(n);
  for (std::size_t i = 0; i < n; i++)
    bs[i] = {id[i].empty() ? std::to_string(i) : id[i], region[i], use[i], structure[i], area[i],
             std::isfinite(year[i]) ? static_cast<int>(year[i]) : ie::buildings::unknown_year};
  return bs;
}

inline Json totals_json(const ie::buildings::Totals& t) {
  return Json::object({{"lower", written(t.lower)}, {"central", written(t.central)}, {"upper", written(t.upper)},
                       {"floor_area", Json(t.floor_area)}, {"buildings", Json(static_cast<double>(t.buildings))}});
}

inline Json op_buildings(const Json& r) {
  const auto s = scheme_of(r["scheme"]);
  const auto bs = buildings_of(r["buildings"]);
  const auto group = texts(r["group"]);
  const auto st = ie::buildings::stock(s, bs, group);
  Json groups = Json::object();
  for (const auto& [name, t] : st.by_group) groups.set(name, totals_json(t));
  Json out = Json::object({{"materials", written(s.materials)}, {"total", totals_json(st.total)}, {"by_group", groups},
                           {"unmatched", written(st.unmatched)}, {"unmatched_area", Json(st.unmatched_area)}, {"scheme", Json(s.name)}});
  if (r.has("per_building") && r["per_building"].as_bool()) {
    Json lo = Json::array(), mid = Json::array(), hi = Json::array();
    for (const auto& b : bs) {
      const auto I = ie::buildings::intensity(s, b);
      std::vector<double> l(s.materials.size(), NAN), c = l, u = l;
      if (I.matched)
        for (std::size_t k = 0; k < l.size(); k++) l[k] = b.floor_area * I.lower[k], c[k] = b.floor_area * I.central[k], u[k] = b.floor_area * I.upper[k];
      lo.push(written(l)), mid.push(written(c)), hi.push(written(u));
    }
    out.set("per_building", Json::object({{"lower", lo}, {"central", mid}, {"upper", hi}}));
  }
  if (r.has("renewal")) {
    const Json& R = r["renewal"];
    const int year = static_cast<int>(num(R["year"]));
    const std::size_t years = static_cast<std::size_t>(number_or(R, "years", 25));
    const std::size_t max_age = static_cast<std::size_t>(number_or(R, "max_age", 150));
    const auto co = ie::buildings::cohorts(s, bs, year, max_age);
    double area = 0;
    for (double a : co.floor_area) area += a;
    std::vector<double> path;
    if (R.has("path")) path = numbers(R["path"]);
    else {
      const double g = number_or(R, "growth", 0);
      for (std::size_t t = 0; t < years; t++) path.push_back(area * std::pow(1 + g, static_cast<double>(t + 1)));
    }
    const auto law = lifetime_of(R["lifetime"]);
    ie::stocks::Initial init{co.floor_area, law};
    const auto M = ie::stocks::stock_driven(path, {law}, &init);
    ie::stocks::Intensity in;
    in.materials = s.materials;
    in.initial_lower = co.lower, in.initial_central = co.central, in.initial_upper = co.upper;
    std::string presumed;
    if (R.has("new")) {
      in.lower = {numbers(R["new"]["lower"])}, in.central = {numbers(R["new"]["central"])}, in.upper = {numbers(R["new"]["upper"])};
      presumed = "new buildings at the intensity stated";
    } else {
      // The floor-area-weighted intensity of the buildings of the last ten years, said to be presumed.
      const std::size_t K = s.materials.size();
      std::vector<double> l(K, 0), c(K, 0), u(K, 0);
      double w = 0;
      for (std::size_t a = 0; a < std::min<std::size_t>(10, co.floor_area.size()); a++) {
        w += co.floor_area[a];
        for (std::size_t k = 0; k < K; k++) l[k] += co.floor_area[a] * co.lower[a][k], c[k] += co.floor_area[a] * co.central[a][k], u[k] += co.floor_area[a] * co.upper[a][k];
      }
      if (!(w > 0)) throw std::invalid_argument("buildings: no building of the last ten years to presume new buildings' intensity from; state renewal.new");
      for (std::size_t k = 0; k < K; k++) l[k] /= w, c[k] /= w, u[k] /= w;
      in.lower = {l}, in.central = {c}, in.upper = {u};
      presumed = "new buildings presumed at the floor-area-weighted intensity of those built in the last ten years";
    }
    Json ren = model_json(M, false);
    ren.set("materials", materials_json(ie::stocks::materials(M, in)));
    ren.set("presumed", Json(presumed));
    ren.set("lifetime", Json(law.law));
    ren.set("left_out_area", Json(co.left_out_area));
    ren.set("floor_area_by_age", written(co.floor_area));
    out.set("renewal", ren);
  }
  return out;
}

// ---------------------------------------------------------------- flows

inline Json op_reconcile(const Json& r) {
  ie::flows::System sys;
  sys.processes = texts(r["processes"]);
  std::vector<ie::flows::Observation> obs;
  auto end = [&](const Json& j) { return j.is_string() ? static_cast<int>(index_of(sys.processes, j.as_string(), "reconcile")) : ie::flows::outside; };
  for (std::size_t i = 0; i < r["flows"].size(); i++) {
    const Json& f = r["flows"][i];
    ie::flows::Variable v{f["name"].text("flow " + std::to_string(i)), end(f["from"]), end(f["to"]), f.has("stock") && f["stock"].as_bool()};
    sys.variables.push_back(v);
    if (!f.has("value") || !f["value"].is_number()) obs.push_back(ie::flows::unknown());
    else obs.push_back({f["value"].as_number(), number_or(f, "sd", 0)});
  }
  const auto R = ie::flows::reconcile(sys, obs);
  Json observable = Json::array(), kind = Json::array(), links = Json::array();
  for (std::size_t j = 0; j < R.value.size(); j++) {
    observable.push(Json(static_cast<bool>(R.observable[j])));
    kind.push(Json(R.kind[j] == 0 ? "measured" : R.kind[j] == 1 ? "exact" : "unknown"));
    const auto& v = sys.variables[j];
    const std::string from = v.from == ie::flows::outside ? "(outside)" : sys.processes[static_cast<std::size_t>(v.from)];
    const std::string to = v.stock ? sys.processes[static_cast<std::size_t>(v.from)] + " (stock)" : v.to == ie::flows::outside ? "(outside)" : sys.processes[static_cast<std::size_t>(v.to)];
    links.push(Json::object({{"name", Json(v.name)}, {"source", Json(from)}, {"target", Json(to)}, {"value", std::isfinite(R.value[j]) ? Json(R.value[j]) : Json()}}));
  }
  return Json::object({{"value", written(R.value)}, {"sd", written(R.sd)}, {"kind", kind}, {"observable", observable},
                       {"adjustment", written(R.adjustment)}, {"normalised", written(R.normalised)}, {"chi2", Json(R.chi2)}, {"p", Json(R.p)},
                       {"redundancy", Json(static_cast<double>(R.redundancy))}, {"balance", Json(R.balance)}, {"links", links}});
}

inline Json op_transfer(const Json& r) {
  ie::flows::TransferModel m;
  m.processes = texts(r["processes"]);
  const std::size_t n = m.processes.size();
  const auto S = rows_of(r["shares"], n, "transfer: shares");
  if (S.rows != n) throw std::invalid_argument("transfer: one row of shares per process");
  m.shares = Matrix(n);
  m.shares.a = S.a;
  m.accumulation = numbers(r["accumulation"]);
  if (m.accumulation.empty()) m.accumulation.assign(n, 0.0);
  const auto T = ie::flows::transfer(m, numbers(r["imports"]));
  return Json::object({{"throughput", written(T.throughput)}, {"to_outside", written(T.to_outside)}, {"stock_change", written(T.stock_change)},
                       {"flows", table_json(T.flows)}, {"imports", Json(T.imports)}, {"exports", Json(T.exports)}, {"accumulated", Json(T.accumulated)},
                       {"balance", Json(T.balance)}});
}

// ---------------------------------------------------------------- life cycles

inline Json op_lca(const Json& r) {
  ie::lca::ProductSystem ps;
  ps.products = texts(r["products"]), ps.processes = texts(r["processes"]), ps.interventions = texts(r["interventions"]);
  ps.A = rows_of(r["A"], ps.processes.size(), "lca: A");
  ps.B = rows_of(r["B"], ps.processes.size(), "lca: B");
  ps.validate();
  for (std::size_t i = 0; i < r["partition"].size(); i++) {
    const Json& p = r["partition"][i];
    std::vector<double> f(ps.products.size(), 0.0);
    for (const auto& [name, v] : p["factors"].as_object()) f[index_of(ps.products, name, "lca: partition")] = num(v);
    ps = ie::lca::partition(ps, index_of(ps.processes, p["process"].text(), "lca: partition"), f);
  }
  for (std::size_t i = 0; i < r["substitute"].size(); i++) {
    const Json& p = r["substitute"][i];
    ps = ie::lca::substitute(ps, index_of(ps.processes, p["process"].text(), "lca: substitute"), index_of(ps.products, p["product"].text(), "lca: substitute"),
                             index_of(ps.processes, p["by"].text(), "lca: substitute"));
  }
  ie::lca::Method m;
  m.categories = texts(r["method"]["categories"]), m.units = texts(r["method"]["units"]);
  m.C = rows_of(r["method"]["C"], ps.interventions.size(), "lca: C");
  const auto demand = numbers(r["demand"]);
  const auto R = ie::lca::assess(ps, m, demand);
  Json elas = Json::array();
  for (std::size_t k = 0; k < m.categories.size(); k++) {
    Json top = Json::array();
    if (R.impacts[k] != 0) {
      const auto E = ie::lca::elasticities(ps, m, R, k);
      for (std::size_t i = 0; i < std::min<std::size_t>(15, E.size()); i++) {
        const auto& e = E[i];
        top.push(Json::object({{"coefficient", Json(std::string(e.matrix == 'A' ? "A" : "B") + "[" + (e.matrix == 'A' ? ps.products[e.row] : ps.interventions[e.row]) +
                                                    ", " + ps.processes[e.col] + "]")},
                               {"elasticity", Json(e.elasticity)}}));
      }
    }
    elas.push(top);
  }
  Json out = Json::object({{"processes", written(ps.processes)}, {"scaling", written(R.scaling)}, {"inventory", written(R.inventory)}, {"impacts", written(R.impacts)},
                           {"categories", written(m.categories)}, {"units", written(m.units)}, {"contribution", table_json(R.contribution)}, {"elasticities", elas}});
  if (r.has("monte_carlo")) {
    const Json& mc = r["monte_carlo"];
    const auto S = ie::lca::monte_carlo(ps, m, demand, number_or(mc, "gsd_A", 1.1), number_or(mc, "gsd_B", 1.2), static_cast<std::size_t>(number_or(mc, "draws", 1000)),
                                        static_cast<std::uint64_t>(number_or(mc, "seed", 1)));
    out.set("spread", Json::object({{"median", written(S.median)}, {"lower", written(S.lower)}, {"upper", written(S.upper)}, {"mean", written(S.mean)}, {"draws", Json(static_cast<double>(S.draws))}}));
  }
  if (r.has("paths")) {
    const std::size_t k = static_cast<std::size_t>(number_or(r["paths"], "category", 0));
    const auto W = ie::lca::paths(ps, m, demand, k, number_or(r["paths"], "cutoff", 1e-3));
    Json walks = Json::array();
    for (const auto& w : W.walks) {
      std::vector<std::string> names;
      for (std::size_t j : w.nodes) names.push_back(ps.processes[j]);
      walks.push(Json::object({{"path", written(names)}, {"value", Json(w.value)}}));
    }
    out.set("paths", Json::object({{"category", Json(m.categories.at(k))}, {"walks", walks}, {"total", Json(W.total)}, {"covered", Json(W.covered)}, {"complete", Json(W.complete)}}));
  }
  return out;
}

// ---------------------------------------------------------------- footprints

inline Json op_footprint(const Json& r) {
  ie::eeio::Mrio io;
  io.regions = texts(r["regions"]), io.sectors = texts(r["sectors"]);
  if (io.regions.empty()) io.regions = {"economy"};
  const std::size_t N = io.size(), R = io.R();
  const auto Z = rows_of(r["Z"], N, "footprint: Z");
  if (Z.rows != N) throw std::invalid_argument("footprint: Z has one row per sector of every region");
  io.Z = Matrix(N);
  io.Z.a = Z.a;
  // One region with its final demand in categories (households, government, capital, exports): Y has one column per category.
  const auto categories = texts(r["categories"]);
  Rectangular Yc;
  if (!categories.empty()) {
    if (R != 1) throw std::invalid_argument("footprint: categories of final demand are for one region; a multi-regional Y has one column per region");
    Yc = rows_of(r["Y"], categories.size(), "footprint: Y");
    io.Y = Rectangular(N, 1);
    for (std::size_t i = 0; i < N; i++)
      for (std::size_t c = 0; c < categories.size(); c++) io.Y(i, 0) += Yc(i, c);
  } else {
    io.Y = rows_of(r["Y"], R, "footprint: Y");
  }
  io.ext.stressors = texts(r["stressors"]), io.ext.units = texts(r["units"]);
  io.ext.F = rows_of(r["F"], N, "footprint: F");
  if (r.has("F_Y")) io.ext.F_y = rows_of(r["F_Y"], R, "footprint: F_Y");
  if (r.has("characterise")) {
    const Json& c = r["characterise"];
    io.ext = ie::eeio::characterise(io.ext, rows_of(c["C"], io.ext.stressors.size(), "footprint: C"), texts(c["categories"]), texts(c["units"]));
  }
  const auto acc = ie::eeio::accounts(io);
  Json origin = Json::array();
  for (const auto& P : acc.origin) origin.push(table_json(P));
  const std::size_t region = static_cast<std::size_t>(number_or(r, "region", 0));
  Json out = Json::object({{"stressors", written(io.ext.stressors)}, {"units", written(io.ext.units)}, {"output", written(acc.output)},
                           {"production", table_json(acc.production)}, {"consumption", table_json(acc.consumption)},
                           {"exports_embodied", table_json(acc.exports_embodied)}, {"imports_embodied", table_json(acc.imports_embodied)},
                           {"origin", origin}, {"multipliers", table_json(acc.M)}, {"intensities", table_json(acc.S)},
                           {"by_product", table_json(ie::eeio::by_product(io, acc, region))}, {"region", Json(io.regions.at(region))},
                           {"productive", Json(acc.leontief.productive)}, {"balance", Json(acc.balance)}});
  if (!categories.empty()) {
    // By category: Σ_j M_kj Y_jc, with F_Y given to the category named "direct" (by default the first), and each category by product.
    const std::size_t K = io.ext.stressors.size(), C = categories.size(), n = io.n();
    const std::string direct = r["direct"].text(categories[0]);
    Rectangular byc(K, C);
    Json byp = Json::array();
    for (std::size_t c = 0; c < C; c++) {
      Rectangular p(K, n);
      for (std::size_t k = 0; k < K; k++)
        for (std::size_t j = 0; j < N; j++) {
          const double v = acc.M(k, j) * Yc(j, c);
          byc(k, c) += v;
          p(k, j % n) += v;
        }
      if (categories[c] == direct && io.ext.F_y.cols)
        for (std::size_t k = 0; k < K; k++) byc(k, c) += io.ext.F_y(k, 0);
      byp.push(table_json(p));
    }
    out.set("categories", written(categories));
    out.set("by_category", table_json(byc));
    out.set("by_category_product", byp);
    out.set("direct", Json(direct));
  }
  if (r.has("paths")) {
    const std::size_t k = static_cast<std::size_t>(number_or(r["paths"], "stressor", 0));
    const auto W = ie::eeio::paths(io, acc, k, region, number_or(r["paths"], "cutoff", 1e-3));
    Json walks = Json::array();
    for (const auto& w : W.walks) {
      std::vector<std::string> names;
      for (std::size_t j : w.nodes) names.push_back(io.label(j));
      walks.push(Json::object({{"path", written(names)}, {"value", Json(w.value)}}));
    }
    out.set("paths", Json::object({{"stressor", Json(io.ext.stressors.at(k))}, {"walks", walks}, {"total", Json(W.total)}, {"covered", Json(W.covered)}, {"complete", Json(W.complete)}}));
  }
  return out;
}

// ---------------------------------------------------------------- accounts

inline Json op_accounts(const Json& r) {
  Json out = Json::object();
  if (r.has("supply_use")) {
    const Json& t = r["supply_use"];
    ie::accounts::SupplyUse su;
    su.products = texts(t["products"]), su.activities = texts(t["activities"]);
    su.natural_inputs = texts(t["natural_inputs"]), su.residual_kinds = texts(t["residual_kinds"]);
    const std::size_t A = su.activities.size();
    su.supply = rows_of(t["supply"], A, "accounts: supply"), su.use = rows_of(t["use"], A, "accounts: use");
    su.imports = numbers(t["imports"]), su.exports = numbers(t["exports"]), su.accumulation = numbers(t["accumulation"]);
    su.natural = rows_of(t["natural"], A, "accounts: natural"), su.residuals = rows_of(t["residuals"], A, "accounts: residuals");
    su.stock = numbers(t["stock"]);
    if (su.stock.empty()) su.stock.assign(A, 0.0);
    const auto b = ie::accounts::balances(su);
    out.set("supply_use", Json::object({{"by_product", written(b.by_product)}, {"by_activity", written(b.by_activity)}, {"economy", Json(b.economy)},
                                        {"worst", Json(b.worst)}, {"natural", Json(b.natural_total)}, {"residuals", Json(b.residual_total)},
                                        {"imports", Json(b.imports_total)}, {"exports", Json(b.exports_total)}, {"additions", Json(b.additions)}}));
  }
  if (r.has("economy_wide")) {
    const Json& e = r["economy_wide"];
    ie::accounts::EconomyWide ew;
    ew.materials = texts(e["materials"]);
    ew.extraction = numbers(e["extraction"]), ew.imports = numbers(e["imports"]), ew.exports = numbers(e["exports"]);
    ew.rme_imports = numbers(e["rme_imports"]), ew.rme_exports = numbers(e["rme_exports"]);
    ew.processed_output = number_or(e, "processed_output", NAN);
    ew.balancing_input = number_or(e, "balancing_input", 0), ew.balancing_output = number_or(e, "balancing_output", 0);
    const auto I = ie::accounts::indicators(ew);
    out.set("economy_wide", Json::object({{"DE", Json(I.DE)}, {"IMP", Json(I.IMP)}, {"EXP", Json(I.EXP)}, {"DMI", Json(I.DMI)}, {"DMC", Json(I.DMC)},
                                          {"PTB", Json(I.PTB)}, {"RMC", std::isfinite(I.RMC) ? Json(I.RMC) : Json()}, {"RTB", std::isfinite(I.RTB) ? Json(I.RTB) : Json()},
                                          {"NAS", std::isfinite(I.NAS) ? Json(I.NAS) : Json()}, {"dmi", written(I.dmi)}, {"dmc", written(I.dmc)},
                                          {"ptb", written(I.ptb)}, {"rmc", written(I.rmc)}}));
  }
  if (!out.size()) throw std::invalid_argument("accounts: a supply_use table, economy_wide flows, or both");
  return out;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "stock") return op_stock(r);
  if (op == "buildings") return op_buildings(r);
  if (op == "reconcile") return op_reconcile(r);
  if (op == "transfer") return op_transfer(r);
  if (op == "lca") return op_lca(r);
  if (op == "footprint") return op_footprint(r);
  if (op == "accounts") return op_accounts(r);
  throw std::invalid_argument("materials: no operation " + op + " (stock, buildings, reconcile, transfer, lca, footprint, accounts)");
}

}  // namespace science::materials
