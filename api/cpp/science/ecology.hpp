// Ecology, for any work's engine (ADR 0101): one composition of the ecology domain, answering requests in JSON. The forest, the
// population or the landscape is given in the request by whoever holds the data; nothing here names a city, a record or a file's
// columns. A work's tool reads its sources, states what it asks, and asks.
//
//   TREES are stated as columns of equal length:
//     {"species": [...], "genus": [...], "family": [...], "dbh": [...], "age"?: [...], "life_lower"?: [...], "life_upper"?: [...],
//      "area"?: [...]}                     names ("" unknown), diameters in cm, ages and lives in years (null unknown), and each
//                                          tree's area as an index into "areas" (−1 or null for none)
//   a SCHEME of allometry as
//     {"name", "source", "carbon_fraction"?, "open_grown"?, "groups": [{"name", "aboveground"?: {"b0", "b1"}, "roots"?: {"b0", "b1"},
//      "height"?: {"b0", "b1"}, "crown"?: {"b0", "b1"}, "d_min"?, "d_max"?, "increment"?}], "species"?: {name: group},
//      "genera"?: {name: group}, "families"?: {name: group}, "fallback"?: group}
//   and LIMITS of composition as {"name"?, "species", "genus", "family"} (shares).
//
//   forest        {trees, "areas"?, scheme, limits, "bounds"?, "top"?}
//                   the composition overall and by area (the largest names at each rank), the limits found rank by rank, the
//                   size classes, the increments one inventory tells, the stock of carbon and how the scheme matched
//   future        {trees, scheme, "planting"?: {"per_year", "mix": [{"species", "genus", "family", "share"}], "dbh"?, "hazard"?},
//                  "removal"?: {"hazard"?, "use_stated_life"?}, "years", "area"?, "increment"?, "hold"?}
//                   standing trees and carbon year by year with their exact spread, and the planting that holds today's carbon
//   stages        {trees, scheme, "removal"?, "bounds", "planting"?: per year, "area"?, "increment"?}
//                   the stage model a forest implies, its analysis, and the forest a constant planting sustains
//   population    {"stages", "survival", "growth", "fecundity", "n0"?, "years"?, "input"?}
//                   a stage-classified population: λ, structure, values, elasticities, life expectancy, projection, equilibrium
//   distinctness  {trees, "areas"?, "draws"?, "seed"?}
//                   taxonomic distinctness overall and by area, against the funnel of the whole forest's species
//   patches       {"attribute", "distances", "median", "threshold", "landscape_area"}
//                   PC, ECA and IIC, and each patch's importance split into its parts
//   circuits      {"nx", "ny", "cell", "resistance", "eight"?, "patches": [[cell, ...], ...]}
//                   effective resistances, least-cost distances and the cumulative current
//   canopy        {"cover", "weight", "thresholds"}    the canopy the weight of each cell lives under
//   diversity     {trees, "areas"?, "rank"?, "coverage"?, "curve"?: {"sizes"?, "replicates"?, "seed"?}}
//                   Hill numbers of orders 0, 1, 2 observed and estimated, overall and by area; each area at a common coverage;
//                   the partition among areas; the overall rarefaction and extrapolation curve with its bootstrap band
//   occupancy     {"y": [[0 | 1 | null, ...], ...], "site"?: {name: [...]}, "visit"?: {name: [[...], ...]}, "names"?: [...]}
//                   imperfect detection (peira): ψ and p with their covariates, the posterior of occupancy at each site
//
// Units: diameters in cm, carbon in kg (answers give tonnes where they say so), distances and cells in metres.

#pragma once

#include <ecology/biodiversity/biodiversity.hpp>
#include <ecology/connectivity/circuits.hpp>
#include <ecology/connectivity/patches.hpp>
#include <ecology/forest/allometry.hpp>
#include <ecology/forest/canopy.hpp>
#include <ecology/forest/future.hpp>
#include <ecology/forest/inventory.hpp>
#include <ecology/populations/stages.hpp>
#include <ecology/taxonomy/taxonomy.hpp>

#include <peira/detection/occupancy.hpp>

#include "json.hpp"

#include <algorithm>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::ecology {

namespace ef = ::ecology::forest;
namespace ep = ::ecology::populations;
namespace et = ::ecology::taxonomy;
namespace ec = ::ecology::connectivity;
namespace eb = ::ecology::biodiversity;

// ---- reading -----------------------------------------------------------------------------------------------------------

inline double num(const Json& j) { return j.is_number() ? j.as_number() : NAN; }
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(num(a[i]));
  return v;
}
inline std::vector<std::string> texts(const Json& a) {
  std::vector<std::string> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(a[i].text());
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

inline std::vector<ef::Tree> trees_of(const Json& t) {
  const auto sp = texts(t["species"]), ge = texts(t["genus"]), fa = texts(t["family"]);
  const auto dbh = numbers(t["dbh"]), age = numbers(t["age"]), lo = numbers(t["life_lower"]), hi = numbers(t["life_upper"]), area = numbers(t["area"]);
  const std::size_t n = dbh.size();
  if (sp.size() != n || ge.size() != n || fa.size() != n) throw std::invalid_argument("trees: species, genus, family and dbh of one length");
  std::vector<ef::Tree> out(n);
  for (std::size_t i = 0; i < n; i++) {
    out[i].taxon = {sp[i], ge[i], fa[i]};
    out[i].dbh = dbh[i];
    if (age.size() == n) out[i].age = age[i];
    if (lo.size() == n) out[i].life_lower = lo[i];
    if (hi.size() == n) out[i].life_upper = hi[i];
    if (area.size() == n && std::isfinite(area[i])) out[i].area = static_cast<int>(area[i]);
  }
  return out;
}

inline ef::Power power_of(const Json& j) { return {number_or(j, "b0", 0), number_or(j, "b1", 0), number_or(j, "scale", 1)}; }

inline ef::Scheme scheme_of(const Json& j) {
  ef::Scheme s;
  s.name = j["name"].text("scheme");
  s.source = j["source"].text();
  s.carbon_fraction = number_or(j, "carbon_fraction", 0.5);
  s.open_grown = number_or(j, "open_grown", 1.0);
  s.cap_beyond_range = j["beyond_range"].text("extrapolate") == "cap";
  std::map<std::string, std::size_t> index;
  const Json& groups = j["groups"];
  for (std::size_t k = 0; k < groups.size(); k++) {
    const Json& g = groups[k];
    ef::Group G;
    G.name = g["name"].text();
    if (g.has("aboveground")) G.aboveground = power_of(g["aboveground"]);
    if (g.has("roots")) G.roots = ef::Ratio{number_or(g["roots"], "b0", 0), number_or(g["roots"], "b1", 0)};
    if (g.has("height")) G.height = power_of(g["height"]);
    if (g.has("crown")) G.crown = power_of(g["crown"]);
    G.d_min = number_or(g, "d_min", 0);
    G.d_max = number_or(g, "d_max", INFINITY);
    G.increment = number_or(g, "increment", NAN);
    index[G.name] = s.add(G);
  }
  const auto bind = [&](const char* key, std::map<std::string, std::size_t>& to) {
    if (!j.has(key)) return;
    for (const auto& [name, group] : j[key].as_object()) {
      auto it = index.find(group.text());
      if (it == index.end()) throw std::invalid_argument("scheme: " + name + " names no group \"" + group.text() + "\"");
      to[name] = it->second;
    }
  };
  bind("species", s.by_species);
  bind("genera", s.by_genus);
  bind("families", s.by_family);
  if (j.has("fallback")) {
    auto it = index.find(j["fallback"].text());
    if (it == index.end()) throw std::invalid_argument("scheme: the fallback names no group");
    s.fallback = it->second;
  }
  return s;
}

inline ef::Limits limits_of(const Json& j) {
  ef::Limits l;
  if (!j.is_object()) return l;
  l.name = j["name"].text(l.name);
  l.species = number_or(j, "species", l.species);
  l.genus = number_or(j, "genus", l.genus);
  l.family = number_or(j, "family", l.family);
  return l;
}

inline ef::Increments increments_of(const std::vector<ef::Tree>& trees, const Json& r) {
  auto inc = ef::increments_from_ages(trees);
  if (r.has("increment") && r["increment"].is_number()) {
    // A stated increment for every genus the inventory cannot tell.
    if (!std::isfinite(inc.overall)) inc.overall = r["increment"].as_number();
  }
  return inc;
}

// ---- answers -------------------------------------------------------------------------------------------------------------

inline Json tally_json(const et::Tally& t, std::size_t top) {
  Json names = Json::array(), counts = Json::array();
  for (std::size_t k = 0; k < std::min(top, t.names.size()); k++) names.push(Json(t.names[k])), counts.push(Json(t.counts[k]));
  return Json::object({{"names", names}, {"counts", counts}, {"kinds", Json(static_cast<double>(t.names.size()))}, {"known", Json(t.known)}, {"unknown", Json(t.unknown)}});
}

inline Json findings_json(const std::vector<ef::Finding>& fs) {
  Json out = Json::array();
  for (const auto& f : fs) {
    Json over = Json::array();
    for (const auto& [n, s] : f.over) over.push(Json::object({{"name", Json(n)}, {"share", Json(s)}}));
    out.push(Json::object({{"rank", Json(std::string(et::rank_name(f.rank)))}, {"limit", Json(f.limit)}, {"largest", Json(f.largest)}, {"share", Json(f.share)},
                           {"holds", Json(f.holds)}, {"over", over}, {"unnamed", Json(f.unnamed)}}));
  }
  return out;
}

inline Json stock_json(const ef::Stock& s) {
  return Json::object({{"trees", Json(s.trees)}, {"estimated", Json(s.estimated)}, {"carbon_t", Json(s.carbon / 1000)}, {"co2e_t", Json(s.co2e / 1000)},
                       {"biomass_t", Json(s.biomass / 1000)}, {"sequestration_t", Json(s.sequestration / 1000)},
                       {"matched", Json::object({{"species", Json(s.matched[0])}, {"genus", Json(s.matched[1])}, {"family", Json(s.matched[2])}, {"fallback", Json(s.matched[3])}})},
                       {"outside_range", Json(s.outside_range)}, {"crown_area_m2", Json(s.crown_area)}});
}

inline Json op_forest(const Json& r) {
  const auto trees = trees_of(r["trees"]);
  const auto scheme = scheme_of(r["scheme"]);
  const auto limits = limits_of(r["limits"]);
  const auto areas = texts(r["areas"]);
  std::vector<double> bounds = r.has("bounds") ? numbers(r["bounds"]) : std::vector<double>{0, 15, 30, 45, 60, 90};
  const std::size_t top = static_cast<std::size_t>(number_or(r, "top", 12));
  const auto inc = increments_of(trees, r);
  const auto overall = ef::composition(trees);
  Json incs = Json::object();
  for (const auto& [g, v] : inc.by_genus) incs.set(g, Json::object({{"cm_per_year", Json(v)}, {"trees", Json(static_cast<double>(inc.counts.at(g)))}}));
  Json res = Json::object();
  res.set("overall", Json::object({{"species", tally_json(overall.species, top)}, {"genus", tally_json(overall.genus, top)}, {"family", tally_json(overall.family, top)},
                                   {"findings", findings_json(ef::findings(overall, limits))}, {"size", written(ef::size_structure(trees, bounds))},
                                   {"stock", stock_json(ef::stock(trees, scheme, &inc))}}));
  res.set("bounds", written(bounds));
  res.set("limits", Json::object({{"name", Json(limits.name)}, {"species", Json(limits.species)}, {"genus", Json(limits.genus)}, {"family", Json(limits.family)}}));
  res.set("increments", Json::object({{"by_genus", incs}, {"overall", std::isfinite(inc.overall) ? Json(inc.overall) : Json()}, {"used", Json(static_cast<double>(inc.used))}}));
  res.set("scheme", Json::object({{"name", Json(scheme.name)}, {"source", Json(scheme.source)}}));
  Json by = Json::array();
  for (std::size_t a = 0; a < areas.size(); a++) {
    const auto c = ef::composition(trees, static_cast<int>(a));
    if (c.genus.known + c.genus.unknown == 0) continue;
    const auto f = ef::findings(c, limits);
    by.push(Json::object({{"area", Json(areas[a])}, {"index", Json(static_cast<double>(a))}, {"trees", Json(c.genus.known + c.genus.unknown)},
                          {"species", tally_json(c.species, 3)}, {"genus", tally_json(c.genus, 3)}, {"family", tally_json(c.family, 3)},
                          {"findings", findings_json(f)}, {"stock", stock_json(ef::stock(trees, scheme, &inc, static_cast<int>(a)))},
                          {"size", written(ef::size_structure(trees, bounds, static_cast<int>(a)))}}));
  }
  res.set("areas", by);
  return res;
}

inline ef::Planting planting_of(const Json& j) {
  ef::Planting p;
  if (!j.is_object()) return p;
  p.per_year = number_or(j, "per_year", 0);
  p.dbh = number_or(j, "dbh", p.dbh);
  p.hazard = number_or(j, "hazard", p.hazard);
  const Json& mix = j["mix"];
  for (std::size_t k = 0; k < mix.size(); k++) p.mix.push_back({{mix[k]["species"].text(), mix[k]["genus"].text(), mix[k]["family"].text()}, number_or(mix[k], "share", 1)});
  return p;
}

inline ef::Removal removal_of(const Json& j) {
  ef::Removal r;
  if (!j.is_object()) return r;
  r.hazard = number_or(j, "hazard", r.hazard);
  if (j.has("use_stated_life") && j["use_stated_life"].is_bool()) r.use_stated_life = j["use_stated_life"].as_bool();
  return r;
}

inline Json op_future(const Json& r) {
  const auto trees = trees_of(r["trees"]);
  const auto scheme = scheme_of(r["scheme"]);
  const auto inc = increments_of(trees, r);
  const auto plant = planting_of(r["planting"]);
  const auto rm = removal_of(r["removal"]);
  const int years = static_cast<int>(number_or(r, "years", 30));
  const int area = static_cast<int>(number_or(r, "area", -1));
  const auto f = ef::future(trees, scheme, inc, plant, rm, years, area);
  std::vector<double> t, n, nsd, c, csd, pn, pc, rem, ls, lg, lf;
  std::vector<std::string> nsp, ngen, nfam;
  for (const auto& y : f) {
    t.push_back(y.t), n.push_back(y.standing), nsd.push_back(y.standing_sd), c.push_back(y.carbon / 1000), csd.push_back(y.carbon_sd / 1000);
    pn.push_back(y.planted_standing), pc.push_back(y.planted_carbon / 1000), rem.push_back(y.removed);
    ls.push_back(y.largest_share[0]), lg.push_back(y.largest_share[1]), lf.push_back(y.largest_share[2]);
    nsp.push_back(y.largest[0]), ngen.push_back(y.largest[1]), nfam.push_back(y.largest[2]);
  }
  Json res = Json::object({{"t", written(t)}, {"standing", written(n)}, {"standing_sd", written(nsd)}, {"carbon_t", written(c)}, {"carbon_sd_t", written(csd)},
                           {"planted_standing", written(pn)}, {"planted_carbon_t", written(pc)}, {"removed", written(rem)},
                           {"largest_share", Json::object({{"species", written(ls)}, {"genus", written(lg)}, {"family", written(lf)}})},
                           {"largest", Json::object({{"species", written(nsp)}, {"genus", written(ngen)}, {"family", written(nfam)}})}});
  if (r.has("hold") && r["hold"].is_bool() && r["hold"].as_bool() && !plant.mix.empty())
    res.set("hold_per_year", Json(ef::planting_to_hold_carbon(trees, scheme, inc, plant, rm, years, area)));
  return res;
}

inline Json analysis_json(const ep::StageModel& m, const ep::Analysis& a) {
  Json E = Json::array();
  if (a.elasticity.n)
    for (std::size_t i = 0; i < a.elasticity.n; i++) {
      std::vector<double> row;
      for (std::size_t j = 0; j < a.elasticity.n; j++) row.push_back(a.elasticity(i, j));
      E.push(written(row));
    }
  return Json::object({{"stages", written(m.stages)}, {"survival", written(m.survival)}, {"growth", written(m.growth)}, {"fecundity", written(m.fecundity)},
                       {"lambda", Json(a.lambda)}, {"damping", std::isfinite(a.damping) ? Json(a.damping) : Json()}, {"r0", std::isfinite(a.r0) ? Json(a.r0) : Json()},
                       {"stable", written(a.stable)}, {"reproductive", written(a.reproductive)}, {"elasticity", E}, {"life_expectancy", written(a.life_expectancy)},
                       {"rates", Json::object({{"survival", written(a.rates.survival_elasticity)}, {"growth", written(a.rates.growth_elasticity)}, {"fecundity", written(a.rates.fecundity_elasticity)}})}});
}

inline Json op_stages(const Json& r) {
  const auto trees = trees_of(r["trees"]);
  const auto scheme = scheme_of(r["scheme"]);
  const auto inc = increments_of(trees, r);
  const auto rm = removal_of(r["removal"]);
  const auto bounds = r.has("bounds") ? numbers(r["bounds"]) : std::vector<double>{0, 15, 30, 45, 60, 90};
  const int area = static_cast<int>(number_or(r, "area", -1));
  const auto m = ef::stage_model(trees, scheme, inc, rm, bounds, area);
  const auto a = ep::analyse(m);
  Json res = analysis_json(m, a);
  res.set("present", written(ef::size_structure(trees, bounds, area)));
  const double planting = number_or(r, "planting", 0);
  if (planting > 0 && a.lambda < 1) {
    std::vector<double> b(m.size(), 0.0);
    b[0] = planting;
    const auto s = ep::sustained(m, b);
    res.set("equilibrium", written(s.n));
    res.set("sustained", Json::object({{"total", Json(s.total)}, {"survival_elasticity", written(s.survival_elasticity)}, {"growth_elasticity", written(s.growth_elasticity)}}));
  }
  return res;
}

inline Json op_population(const Json& r) {
  ep::StageModel m{texts(r["stages"]), numbers(r["survival"]), numbers(r["growth"]), numbers(r["fecundity"])};
  if (m.stages.size() != m.survival.size()) {
    m.stages.clear();
    for (std::size_t j = 0; j < m.survival.size(); j++) m.stages.push_back("stage " + std::to_string(j + 1));
  }
  const auto a = ep::analyse(m);
  Json res = analysis_json(m, a);
  const auto A = ep::matrix(m);
  if (r.has("n0")) {
    std::vector<std::vector<double>> input;
    if (r.has("input")) input.push_back(numbers(r["input"]));
    const auto path = ep::project(A, numbers(r["n0"]), static_cast<std::size_t>(number_or(r, "years", 50)), input);
    Json p = Json::array();
    for (const auto& row : path) p.push(written(row));
    res.set("projection", p);
    if (!input.empty() && a.lambda < 1) res.set("equilibrium", written(ep::equilibrium(A, input[0])));
  }
  return res;
}

inline Json op_distinctness(const Json& r) {
  const auto trees = trees_of(r["trees"]);
  const auto areas = texts(r["areas"]);
  std::vector<et::Taxon> all;
  for (const auto& t : trees) all.push_back(t.taxon);
  const auto master = et::species_list(all);
  const auto d = et::distinctness(master);
  Json by = Json::array();
  std::vector<std::size_t> sizes;
  std::vector<std::pair<std::size_t, et::Distinctness>> found;
  for (std::size_t a = 0; a < areas.size(); a++) {
    std::vector<et::Taxon> here;
    for (const auto& t : trees)
      if (t.area == static_cast<int>(a)) here.push_back(t.taxon);
    const auto L = et::species_list(here);
    if (L.species.size() < 2) continue;
    const auto x = et::distinctness(L);
    found.push_back({a, x});
    if (x.species <= master.species.size()) sizes.push_back(x.species);
  }
  std::sort(sizes.begin(), sizes.end());
  sizes.erase(std::unique(sizes.begin(), sizes.end()), sizes.end());
  const auto fun = sizes.empty() ? std::vector<et::FunnelPoint>{} : et::funnel(master, sizes, static_cast<std::size_t>(number_or(r, "draws", 500)), static_cast<std::uint64_t>(number_or(r, "seed", 1)));
  std::map<std::size_t, et::FunnelPoint> band;
  for (const auto& p : fun) band[p.species] = p;
  for (const auto& [a, x] : found) {
    const auto& p = band[x.species];
    by.push(Json::object({{"area", Json(areas[a])}, {"species", Json(static_cast<double>(x.species))}, {"average", Json(x.average)}, {"variation", Json(x.variation)},
                          {"weighted", Json(x.weighted)}, {"expected", Json(p.mean)}, {"lower", Json(p.lower)}, {"upper", Json(p.upper)},
                          {"below", Json(x.average < p.lower)}}));
  }
  return Json::object({{"species", Json(static_cast<double>(d.species))}, {"average", Json(d.average)}, {"variation", Json(d.variation)}, {"weighted", Json(d.weighted)}, {"areas", by}});
}

inline Json op_patches(const Json& r) {
  const auto a = numbers(r["attribute"]);
  ec::Distances D;
  for (std::size_t i = 0; i < r["distances"].size(); i++) D.push_back(numbers(r["distances"][i]));
  for (auto& row : D)
    for (double& x : row)
      if (!std::isfinite(x)) x = INFINITY;
  const auto c = ec::connectivity(a, D, number_or(r, "median", 500), number_or(r, "threshold", 500), number_or(r, "landscape_area", 1));
  std::vector<double> tot, in, fl, co;
  for (const auto& p : c.importance) tot.push_back(p.total), in.push_back(p.intra), fl.push_back(p.flux), co.push_back(p.connector);
  return Json::object({{"pc", Json(c.pc)}, {"eca", Json(c.eca)}, {"iic", Json(c.iic)}, {"dPC", written(tot)}, {"intra", written(in)}, {"flux", written(fl)},
                       {"connector", written(co)}, {"dIIC", written(c.d_iic)}});
}

inline Json op_circuits(const Json& r) {
  ec::ResistanceGrid g;
  g.nx = static_cast<std::size_t>(number_or(r, "nx", 0));
  g.ny = static_cast<std::size_t>(number_or(r, "ny", 0));
  g.cell = number_or(r, "cell", 1);
  g.resistance = numbers(r["resistance"]);
  for (double& x : g.resistance)
    if (!std::isfinite(x)) x = INFINITY;
  if (r.has("eight") && r["eight"].is_bool()) g.eight = r["eight"].as_bool();
  std::vector<std::vector<std::size_t>> patches;
  for (std::size_t p = 0; p < r["patches"].size(); p++) {
    std::vector<std::size_t> cells;
    for (double x : numbers(r["patches"][p])) cells.push_back(static_cast<std::size_t>(x));
    patches.push_back(cells);
  }
  const auto c = ec::circuits(g, patches);
  Json R = Json::array(), L = Json::array();
  for (const auto& row : c.resistance) R.push(written(row));
  for (const auto& row : c.least_cost) L.push(written(row));
  return Json::object({{"resistance", R}, {"least_cost", L}, {"current", written(c.current)}});
}

inline Json op_canopy(const Json& r) {
  const auto e = ef::canopy_exposure(numbers(r["cover"]), numbers(r["weight"]), numbers(r["thresholds"]));
  return Json::object({{"weight", Json(e.weight)}, {"mean", Json(e.mean)}, {"thresholds", written(e.thresholds)}, {"share_below", written(e.share_below)}});
}

inline et::Rank rank_of(const std::string& s) {
  if (s == "genus") return et::Rank::genus;
  if (s == "family") return et::Rank::family;
  return et::Rank::species;
}

inline Json op_diversity(const Json& r) {
  const auto trees = trees_of(r["trees"]);
  const auto areas = texts(r["areas"]);
  const auto rank = rank_of(r["rank"].text("species"));
  std::vector<et::Taxon> all;
  std::vector<int> area;
  for (const auto& t : trees) all.push_back(t.taxon), area.push_back(t.area);
  const auto d = eb::diversity(all, rank);
  const auto three = [](const double* x) { return Json::array({Json(x[0]), Json(x[1]), Json(x[2])}); };
  Json res = Json::object({{"rank", Json(std::string(et::rank_name(rank)))}, {"individuals", Json(d.individuals)}, {"unnamed", Json(d.unnamed)},
                           {"observed", three(d.observed)}, {"estimated", three(d.estimated)}, {"coverage", Json(d.coverage)},
                           {"richness", Json::object({{"estimate", Json(d.richness.estimate)}, {"lower", Json(d.richness.lower)}, {"upper", Json(d.richness.upper)},
                                                      {"se", Json(d.richness.se)}})}});
  // Each area: its counts, its observed and estimated diversity, and its diversity at the common coverage.
  std::vector<std::vector<double>> samples(areas.size());
  std::vector<std::vector<et::Taxon>> members(areas.size());
  for (std::size_t i = 0; i < trees.size(); i++)
    if (trees[i].area >= 0 && static_cast<std::size_t>(trees[i].area) < areas.size()) members[static_cast<std::size_t>(trees[i].area)].push_back(trees[i].taxon);
  for (std::size_t a = 0; a < areas.size(); a++) samples[a] = eb::counts_at(members[a], rank);
  // The common coverage is set by the areas with enough individuals to say anything (`least`, 50 by default); smaller ones are
  // shown at their own coverage only.
  const double least = number_or(r, "least", 50);
  std::vector<std::vector<double>> usable;
  for (const auto& s : samples) {
    double n = 0;
    for (double x : s) n += x;
    if (s.size() >= 2 && n >= least) usable.push_back(s);
  }
  const double C = usable.empty() ? NAN : eb::common_coverage(usable, number_or(r, "coverage", 0.99));
  Json by = Json::array();
  for (std::size_t a = 0; a < areas.size(); a++) {
    if (samples[a].empty()) continue;
    const auto x = eb::diversity(members[a], rank);
    Json row = Json::object({{"area", Json(areas[a])}, {"individuals", Json(x.individuals)}, {"observed", three(x.observed)}, {"estimated", three(x.estimated)},
                             {"coverage", Json(x.coverage)}});
    if (std::isfinite(C) && samples[a].size() >= 2 && x.individuals >= least) {
      const auto at = eb::at_coverage(samples[a], C);
      row.set("at_coverage", at.reached ? three(at.value) : Json());
      row.set("size_at_coverage", at.reached ? Json(at.size) : Json());
    }
    by.push(row);
  }
  res.set("common_coverage", std::isfinite(C) ? Json(C) : Json());
  res.set("areas", by);
  // The partition among the areas, orders 0, 1, 2.
  if (areas.size() >= 2) {
    Json parts = Json::array();
    for (double q : {0.0, 1.0, 2.0}) {
      try {
        const auto p = eb::partition(all, area, areas.size(), rank, q);
        parts.push(Json::object({{"q", Json(q)}, {"gamma", Json(p.gamma)}, {"alpha", Json(p.alpha)}, {"beta", Json(p.beta)}, {"units", Json(static_cast<double>(p.units))},
                                 {"sorensen", std::isfinite(p.overlap.sorensen) ? Json(p.overlap.sorensen) : Json()},
                                 {"jaccard", std::isfinite(p.overlap.jaccard) ? Json(p.overlap.jaccard) : Json()}}));
      } catch (const std::exception&) {
      }
    }
    res.set("partition", parts);
  }
  // The overall curve of rarefaction and extrapolation, with its bootstrap band.
  if (r.has("curve") && d.individuals > 1) {
    tyche::classes::CurveOptions o;
    const Json& c = r["curve"];
    if (c.has("sizes")) o.sizes = numbers(c["sizes"]);
    o.replicates = static_cast<std::size_t>(number_or(c, "replicates", 30));
    o.seed = static_cast<std::uint64_t>(number_or(c, "seed", 20261112));
    o.knots = static_cast<std::size_t>(number_or(c, "knots", 30));
    const auto pts = tyche::classes::curve(eb::counts_at(all, rank), o);
    std::vector<double> m, q, est, lo, hi, cov;
    std::vector<std::string> how;
    for (const auto& pt : pts) {
      m.push_back(pt.size), q.push_back(pt.order), est.push_back(pt.estimate), lo.push_back(pt.lower), hi.push_back(pt.upper), cov.push_back(pt.coverage);
      how.push_back(pt.method == tyche::classes::Method::interpolated ? "interpolated" : pt.method == tyche::classes::Method::observed ? "observed" : "extrapolated");
    }
    res.set("curve", Json::object({{"size", written(m)}, {"order", written(q)}, {"estimate", written(est)}, {"lower", written(lo)}, {"upper", written(hi)},
                                   {"coverage", written(cov)}, {"method", written(how)}}));
  }
  return res;
}

inline Json op_occupancy(const Json& r) {
  namespace pd = ::peira::detection;
  const Json& y = r["y"];
  const std::size_t S = y.size(), T = S ? y[0].size() : 0;
  if (S == 0 || T == 0) throw std::invalid_argument("occupancy: a table of sites by visits");
  pd::SiteVisits v(S, T);
  for (std::size_t i = 0; i < S; i++) {
    if (y[i].size() != T) throw std::invalid_argument("occupancy: every site has the same visits (null where a visit was not made)");
    for (std::size_t j = 0; j < T; j++) v(i, j) = y[i][j].is_number() ? (y[i][j].as_number() > 0 ? 1 : 0) : -1;
  }
  std::vector<std::string> xn{"(intercept)"}, wn{"(intercept)"};
  std::vector<std::vector<double>> xs, ws;
  if (r.has("site"))
    for (const auto& [name, col] : r["site"].as_object()) xn.push_back(name), xs.push_back(numbers(col));
  if (r.has("visit"))
    for (const auto& [name, rows] : r["visit"].as_object()) {
      std::vector<double> flat;
      for (std::size_t i = 0; i < rows.size(); i++)
        for (double x : numbers(rows[i])) flat.push_back(x);
      wn.push_back(name), ws.push_back(flat);
    }
  calx::linalg::Rectangular X(S, xn.size()), W(S * T, wn.size());
  for (std::size_t i = 0; i < S; i++) {
    X(i, 0) = 1;
    for (std::size_t k = 0; k < xs.size(); k++) X(i, k + 1) = std::isfinite(xs[k].at(i)) ? xs[k][i] : 0.0;
  }
  for (std::size_t i = 0; i < S * T; i++) {
    W(i, 0) = 1;
    for (std::size_t k = 0; k < ws.size(); k++) W(i, k + 1) = std::isfinite(ws[k].at(i)) ? ws[k][i] : 0.0;
  }
  const auto o = pd::occupancy(v, X, W);
  return Json::object({{"psi_names", written(xn)}, {"psi", written(o.psi_coefficients)}, {"psi_se", written(o.psi_se)},
                       {"p_names", written(wn)}, {"p", written(o.p_coefficients)}, {"p_se", written(o.p_se)},
                       {"log_likelihood", Json(o.log_likelihood)}, {"aic", Json(o.aic)}, {"converged", Json(o.converged)},
                       {"site_psi", written(o.psi)}, {"occupied", written(o.occupied)}, {"naive", Json(o.naive)}, {"finite_sample", Json(o.finite_sample)},
                       {"mean_psi", Json(o.mean_psi)}, {"sites_surveyed", Json(static_cast<double>(o.sites_surveyed))}, {"sites_detected", Json(static_cast<double>(o.sites_detected))}});
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "forest") return op_forest(r);
  if (op == "future") return op_future(r);
  if (op == "stages") return op_stages(r);
  if (op == "population") return op_population(r);
  if (op == "distinctness") return op_distinctness(r);
  if (op == "diversity") return op_diversity(r);
  if (op == "patches") return op_patches(r);
  if (op == "circuits") return op_circuits(r);
  if (op == "canopy") return op_canopy(r);
  if (op == "occupancy") return op_occupancy(r);
  if (op == "status") return Json::object({{"ok", Json(true)}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

}  // namespace science::ecology
