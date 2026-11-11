// Markets, for any work's engine (ADR 0093): one composition of the markets domain, the urban-science domain's access and peira's
// survival, answering requests in JSON. The market is given in the request, by whoever holds the data: nothing here names a place,
// a census or a file's columns. A work's tool reads its sources, states the market, and asks.
//
//   A MARKET is stated as
//     {"buyers": {"mass": [...], "spend": [...]},                    cells of buyers: how many each stands for, what one brings
//      "offers": {"seller": [...], "attraction"?: [...]},            each offer's seller (an index); its attraction (1 when not given)
//      "pairs": {"b": [...], "o": [...], "cost": [...]},             the pairs within reach: a cell, an offer, the cost between them
//      "huff": {"alpha"?, "beta"?, "decay"?: "power" | "exponential", "floor"?},
//      "outside"?: c}                                                not buying pulls as an offer of attraction 1 at the cost c; 0: no pull
//   and the kernel is Huff's model on it: presumed, and the answer says so.
//
//   costs      {"model": "<file of the data directory>" | "ways": [[speed, x0, y0, x1, y1, ...], ...], "from": [[x, y], ...],
//               "to": [[x, y], ...], "limit": seconds, "walk"?: m/s}
//                                the pairs of `from` and `to` within the limit on the ways, with their seconds (urban science's access);
//                                a city model's ways are read once and kept
//   market     {market}          each offer's demand, each seller's, the structure, what each cell brings and what of it buys nothing
//   offer      {market, "offer": o, "share"?}
//                                one offer: its trade area by cell, its core at a share, its reach, where its custom would go
//   enter      {market, "site": {"attraction"?, "b": [...], "cost": [...]}, "seller": s}
//                                a new offer: what it would sell, drawn from whom, the cannibalisation, its trade area
//   sites      {market, "sites": [{"attraction"?, "b": [...], "cost": [...]}, ...], "k", "seller": s}
//                                k sites chosen greedily for a seller (a seller that has no offer is a newcomer)
//   size       {"nodes": [{"kind": "leaf", "name", "low", "mid", "high", "source"?, "assumed"?} | {"kind": "sum" | "product", "name", "children": [i, ...]}]}
//                                a size as a tree: every node's bounds, the leaves' elasticities, the swings
//   structure  {"amounts": [...]} | {"sets": [[...], ...]}           concentration among holders (nomos), of one set or of several
//   survival   {"time": [...], "event": [...], "group"?: [...]}      Kaplan and Meier's curves, by group, with the log-rank test (peira)
//
// Units are the caller's: costs in what it measures, spends in a currency a period. Answers are whole vectors, in the caller's order.

#pragma once

#include <markets/market/market.hpp>
#include <markets/sizing/sizing.hpp>
#include <markets/space/space.hpp>

#include <peira/survival/survival.hpp>
#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>

#include "json.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace science::markets {

namespace mm = ::markets::market;
namespace ms = ::markets::sizing;
namespace sp = ::markets::space;
namespace ua = ::urban::access;

inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline std::string text_or(const Json& j, const char* key, const std::string& fallback) { return j.has(key) && j[key].is_string() ? j[key].text() : fallback; }
inline void require(bool ok, const char* what) {
  if (!ok) throw std::invalid_argument(std::string("markets: ") + what);
}
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  v.reserve(a.size());
  for (std::size_t k = 0; k < a.size(); k++) v.push_back(a[k].as_number());
  return v;
}
inline std::vector<std::size_t> indices(const Json& a) {
  std::vector<std::size_t> v;
  v.reserve(a.size());
  for (std::size_t k = 0; k < a.size(); k++) {
    const double x = a[k].as_number();
    require(x >= 0 && x == std::floor(x), "an index is a whole number, not negative");
    v.push_back(static_cast<std::size_t>(x));
  }
  return v;
}
// Numbers as they are written: NaN and the infinities are null.
inline Json written(const std::vector<double>& v) {
  Json out = Json::array();
  for (double x : v) out.push(std::isfinite(x) ? Json(x) : Json());
  return out;
}
inline Json written(const std::vector<std::size_t>& v) {
  Json out = Json::array();
  for (std::size_t x : v) out.push(x);
  return out;
}

// ---- the market a request states ---------------------------------------------------------------------------------------------------

struct Stated {
  mm::Market M;
  mm::Logit rule;
  sp::Huff huff;
  std::vector<sp::Reachable> pairs;
  std::vector<double> attraction, outside;
  mm::Demand D;
};

inline sp::Huff huff_of(const Json& m) {
  sp::Huff h;
  if (!m.has("huff")) return h;
  const Json& j = m["huff"];
  h.alpha = number_or(j, "alpha", 1), h.beta = number_or(j, "beta", 2), h.floor = number_or(j, "floor", 1);
  h.decay = text_or(j, "decay", "power") == "exponential" ? sp::Decay::exponential : sp::Decay::power;
  require(h.beta >= 0 && h.floor > 0 && std::isfinite(h.alpha), "Huff's α is finite, its β not negative, its floor positive");
  return h;
}

inline Stated stated(const Json& r) {
  require(r.has("market"), "a request states its market");
  const Json& m = r["market"];
  require(m.has("buyers") && m.has("offers") && m.has("pairs"), "a market has buyers, offers and the pairs within reach");
  Stated X;
  X.M.buyers.mass = numbers(m["buyers"]["mass"]), X.M.buyers.spend = numbers(m["buyers"]["spend"]);
  require(X.M.buyers.mass.size() == X.M.buyers.spend.size(), "a mass and a spend for each cell of buyers");
  X.M.seller = indices(m["offers"]["seller"]);
  require(!X.M.seller.empty(), "a market has at least one offer");
  X.attraction = m["offers"].has("attraction") ? numbers(m["offers"]["attraction"]) : std::vector<double>(X.M.seller.size(), 1.0);
  require(X.attraction.size() == X.M.seller.size(), "an attraction for each offer");
  const auto b = indices(m["pairs"]["b"]), o = indices(m["pairs"]["o"]);
  const auto cost = numbers(m["pairs"]["cost"]);
  require(b.size() == o.size() && b.size() == cost.size(), "a cell, an offer and a cost for each pair");
  for (std::size_t k = 0; k < b.size(); k++) X.pairs.push_back({b[k], o[k], cost[k]});
  X.huff = huff_of(m);
  const double away = number_or(m, "outside", 0);
  require(away >= 0, "the pull of not buying is a cost, not negative");
  X.outside.assign(X.M.buyers.size(), away > 0 ? sp::weight(X.huff, 1.0, away) : 0.0);
  X.rule = sp::rule(X.huff, X.M.buyers.size(), X.attraction, X.pairs, X.outside);
  X.M.kernel = mm::kernel(X.rule);
  mm::validate(X.M);
  X.D = mm::demand(X.M);
  return X;
}

// ---- costs on the ways -----------------------------------------------------------------------------------------------------------------

inline std::string slurp(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("the data library has no " + path);
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

// The ways one may not walk; steps are slower.
inline double speed_of(const std::string& kind, double walk) {
  static const char* closed[] = {"motorway", "motorway_link", "construction", "proposed", "raceway", "busway", "bus_guideway", "rail"};
  for (const char* k : closed)
    if (kind == k) return 0;
  return kind == "steps" ? 0.45 * walk : walk;
}

// The network of a city model of the data library (its roads, in its own frame), read once and kept.
inline const ua::Network& network_of_model(const std::string& data, const std::string& file) {
  static std::map<std::string, std::unique_ptr<ua::Network>> kept;
  auto found = kept.find(file);
  if (found != kept.end()) return *found->second;
  for (const char c : file) require(std::isalnum(static_cast<unsigned char>(c)) || c == '-' || c == '_' || c == '.', "a model is a file of the data directory, by its name");
  require(file.find("..") == std::string::npos, "a model is a file of the data directory, by its name");
  const Json model = Json::parse(slurp(data + "/" + file));
  std::vector<ua::Way> ways;
  const Json& roads = model["roads"];
  for (std::size_t k = 0; k < roads.size(); k++) {
    ua::Way way;
    way.speed = speed_of(roads[k]["kind"].text(), 1.34);
    const Json& line = roads[k]["line"];
    for (std::size_t i = 0; i < line.size(); i++) way.xy.push_back(line[i][0].as_number()), way.xy.push_back(line[i][1].as_number());
    if (way.xy.size() >= 4) ways.push_back(std::move(way));
  }
  auto n = std::make_unique<ua::Network>(ua::network_of(ways, 1.5));
  ua::stitch_ends(*n, 8, 1.34);
  return *(kept[file] = std::move(n));
}

inline std::vector<std::pair<double, double>> points(const Json& a) {
  std::vector<std::pair<double, double>> p;
  for (std::size_t k = 0; k < a.size(); k++) p.push_back({a[k][0].as_number(), a[k][1].as_number()});
  return p;
}

inline Json op_costs(const Json& r, const std::string& data) {
  const auto from = points(r["from"]), to = points(r["to"]);
  const double limit = number_or(r, "limit", 900), walk = number_or(r, "walk", 1.34);
  require(limit > 0 && walk > 0, "a limit and a speed, positive");
  std::vector<sp::Reachable> pairs;
  if (r.has("model")) pairs = ua::pairs_within(network_of_model(data, r["model"].text()), from, to, limit, walk);
  else {
    require(r.has("ways"), "costs are walked on a model's ways or on the ways given");
    std::vector<ua::Way> ways;
    for (std::size_t k = 0; k < r["ways"].size(); k++) {
      const Json& w = r["ways"][k];
      ua::Way way;
      way.speed = w[0].as_number();
      for (std::size_t i = 1; i < w.size(); i++) way.xy.push_back(w[i].as_number());
      ways.push_back(std::move(way));
    }
    ua::Network n = ua::network_of(ways, number_or(r, "snap", 1.5));
    ua::stitch_ends(n, number_or(r, "stitch", 8), walk);
    pairs = ua::pairs_within(n, from, to, limit, walk);
  }
  Json b = Json::array(), o = Json::array(), s = Json::array();
  for (const auto& p : pairs) b.push(p.demand), o.push(p.supply), s.push(p.cost);
  return Json::object({{"b", b}, {"o", o}, {"seconds", s}});
}

// ---- the market, an offer, a new offer, sites ---------------------------------------------------------------------------------------------

inline Json op_market(const Json& r) {
  const Stated X = stated(r);
  const auto by = mm::by_seller(X.M, X.D.offers);
  const auto S = mm::structure(by);
  std::vector<double> brings(X.M.buyers.size());
  for (std::size_t b = 0; b < brings.size(); b++) brings[b] = X.M.buyers.brings(b);
  double cost = 0;  // the mean cost borne by the spend that buys
  for (const auto& p : X.pairs) cost += X.M.buyers.brings(p.demand) * X.M.kernel.shares.at(p.demand, p.supply) * p.cost;
  return Json::object({{"provenance", "presumed"}, {"total", X.D.total}, {"bought", X.D.bought()}, {"penetration", X.D.penetration()},
                       {"mean_cost", X.D.bought() > 0 ? cost / X.D.bought() : 0.0}, {"demand", written(X.D.offers)}, {"by_seller", written(by)},
                       {"structure", Json::object({{"hhi", S.hhi}, {"cr4", S.cr4}, {"equivalent", S.equivalent}, {"selling", S.sellers}})},
                       {"brings", written(brings)}, {"unserved", written(mm::unserved(X.M))}});
}

inline Json op_offer(const Json& r) {
  const Stated X = stated(r);
  const double asked = number_or(r, "offer", -1);
  require(asked >= 0 && asked < static_cast<double>(X.M.offers()), "an offer of the market, by its index");
  const std::size_t o = static_cast<std::size_t>(asked);
  require(X.D.offers[o] > 0, "this offer sells nothing: no buyers are within its reach");
  const double share = std::clamp(number_or(r, "share", 0.6), 0.05, 1.0);
  const auto core = mm::core(X.M, o, share);
  const auto reach = sp::reach(X.M, X.pairs, o);
  const auto d = mm::diversion_closed(X.M, o);
  // The overlap with the offers its custom would go to most.
  std::vector<std::size_t> rivals(d.to.size());
  std::iota(rivals.begin(), rivals.end(), std::size_t{0});
  std::sort(rivals.begin(), rivals.end(), [&](std::size_t a, std::size_t b) { return d.to[a] > d.to[b]; });
  Json overlap = Json::array();
  double own = 0;
  for (std::size_t j = 0; j < d.to.size(); j++)
    if (j != o && X.M.seller[j] == X.M.seller[o]) own += d.to[j];
  for (std::size_t k = 0; k < rivals.size() && k < 20 && d.to[rivals[k]] > 0; k++) overlap.push(Json::array({Json(rivals[k]), Json(mm::overlap(X.M, o, rivals[k]))}));
  std::vector<double> part(core.part);
  return Json::object({{"offer", o}, {"demand", X.D.offers[o]}, {"share", X.D.offers[o] / X.D.bought()}, {"trade", written(mm::trade_area(X.M, o))},
                       {"core", Json::object({{"share", share}, {"cells", written(core.cells)}, {"part", written(part)}, {"buyers", core.mass}, {"custom", core.custom}})},
                       {"reach", Json::object({{"mean", reach.mean}, {"half", reach.within(0.5)}, {"most", reach.within(0.8)}})},
                       {"diversion", Json::object({{"to", written(d.to)}, {"outside", d.outside}, {"own", own}, {"overlap", overlap}})}});
}

inline sp::Site site_of(const Json& j) {
  sp::Site site;
  site.attraction = number_or(j, "attraction", 1);
  const auto b = indices(j["b"]);
  const auto cost = numbers(j["cost"]);
  require(b.size() == cost.size(), "a cost for each cell a site reaches");
  for (std::size_t k = 0; k < b.size(); k++) site.reach.push_back({b[k], cost[k]});
  return site;
}

// A seller by its index; one beyond those that have offers is a newcomer.
inline std::size_t seller_of(const Stated& X, const Json& r) {
  const double s = number_or(r, "seller", -1);
  return s < 0 ? X.M.sellers() : static_cast<std::size_t>(s);
}

inline Json op_enter(const Json& r) {
  const Stated X = stated(r);
  require(r.has("site"), "a new offer stands at a site");
  const auto site = site_of(r["site"]);
  require(!site.reach.empty(), "no buyers are within reach of this site");
  const auto e = sp::enter(X.M.buyers, X.M.seller, X.rule, X.huff, site, seller_of(X, r));
  std::vector<double> trade(X.M.buyers.size(), 0.0);
  for (const auto& [b, cost] : site.reach) {
    const double u = sp::weight(X.huff, site.attraction, cost), total = X.outside[b] + X.rule.weights.sum(b);
    trade[b] += X.M.buyers.brings(b) * u / (total + u);
  }
  return Json::object({{"demand", e.demand}, {"from", written(e.from)}, {"from_outside", e.from_outside}, {"cannibalised", e.cannibalised}, {"gain", e.gain()},
                       {"from_rivals", e.demand - e.from_outside - e.cannibalised}, {"trade", written(trade)}});
}

inline Json op_sites(const Json& r) {
  const Stated X = stated(r);
  require(r.has("sites") && r["sites"].size() > 0, "there are sites to choose among");
  std::vector<sp::Site> sites;
  for (std::size_t k = 0; k < r["sites"].size(); k++) sites.push_back(site_of(r["sites"][k]));
  const std::size_t k = static_cast<std::size_t>(std::clamp(number_or(r, "k", 3), 1.0, static_cast<double>(sites.size())));
  const auto chosen = sp::choose(X.M.buyers, X.M.seller, X.rule, X.huff, sites, k, seller_of(X, r));
  return Json::object({{"chosen", written(chosen.sites)}, {"gains", written(chosen.gains)}, {"before", chosen.before}, {"after", chosen.after}, {"bound", 1 - std::exp(-1.0)}});
}

// ---- a size, a structure, survival ----------------------------------------------------------------------------------------------------------

inline Json op_size(const Json& r) {
  require(r.has("nodes") && r["nodes"].size() > 0, "a size is a tree of nodes");
  ms::Tree T;
  for (std::size_t i = 0; i < r["nodes"].size(); i++) {
    const Json& n = r["nodes"][i];
    const std::string kind = text_or(n, "kind", "leaf"), name = text_or(n, "name", "node " + std::to_string(i));
    if (kind == "leaf") {
      const double mid = n["mid"].as_number();
      T.leaf(name, {number_or(n, "low", mid), mid, number_or(n, "high", mid)}, text_or(n, "source", ""), !(n.has("assumed") && n["assumed"].is_bool() && !n["assumed"].as_bool()));
    } else {
      const auto children = indices(n["children"]);
      require(kind == "sum" || kind == "product", "a node is a leaf, a sum or a product");
      if (kind == "sum") T.sum(name, children);
      else T.product(name, children);
    }
  }
  const auto R = ms::evaluate(T);
  const auto eps = ms::elasticities(T);
  Json nodes = Json::array(), swings = Json::array();
  for (std::size_t i = 0; i < T.nodes.size(); i++) nodes.push(Json::object({{"name", T.nodes[i].name}, {"low", R[i].low}, {"mid", R[i].mid}, {"high", R[i].high}, {"elasticity", eps[i]}}));
  for (const auto& w : ms::swings(T)) swings.push(Json::object({{"leaf", w.leaf}, {"name", T.nodes[w.leaf].name}, {"low", w.low}, {"high", w.high}}));
  return Json::object({{"nodes", nodes}, {"swings", swings}});
}

inline Json structure_of(const std::vector<double>& amounts) {
  const auto S = mm::structure(amounts);
  return Json::object({{"hhi", S.hhi}, {"cr4", S.cr4}, {"equivalent", S.equivalent}, {"selling", S.sellers}});
}

// One set of amounts, or several ("sets": a period each, say) answered together.
inline Json op_structure(const Json& r) {
  if (!r.has("sets")) return structure_of(numbers(r["amounts"]));
  Json all = Json::array();
  for (std::size_t k = 0; k < r["sets"].size(); k++) all.push(structure_of(numbers(r["sets"][k])));
  return Json::object({{"sets", all}});
}

inline Json op_survival(const Json& r) {
  const auto time = numbers(r["time"]);
  std::vector<int> event, group;
  for (std::size_t k = 0; k < r["event"].size(); k++) event.push_back(r["event"][k].as_number() != 0);
  require(time.size() == event.size() && !time.empty(), "a time and whether it ended, for each");
  if (r.has("group"))
    for (std::size_t k = 0; k < r["group"].size(); k++) group.push_back(static_cast<int>(r["group"][k].as_number()));
  require(group.empty() || group.size() == time.size(), "a group for each, or none");
  std::map<int, std::vector<std::size_t>> members;
  for (std::size_t k = 0; k < time.size(); k++) members[group.empty() ? 0 : group[k]].push_back(k);
  Json curves = Json::array();
  for (const auto& [g, who] : members) {
    std::vector<double> t;
    std::vector<int> e;
    for (std::size_t k : who) t.push_back(time[k]), e.push_back(event[k]);
    const auto km = peira::survival::kaplan_meier(t, e);
    curves.push(Json::object({{"group", g}, {"n", who.size()}, {"time", written(km.time)}, {"at_risk", written(km.at_risk)}, {"events", written(km.events)},
                              {"survival", written(km.survival)}, {"lower", written(km.lower)}, {"upper", written(km.upper)}, {"median", std::isfinite(km.median) ? Json(km.median) : Json()}}));
  }
  Json answer = Json::object({{"curves", curves}});
  if (members.size() > 1) {
    const auto lr = peira::survival::log_rank(time, event, group);
    answer.set("log_rank", Json::object({{"statistic", lr.statistic}, {"df", lr.df}, {"p", lr.p_value}}));
  }
  return answer;
}

// One request answered. `data` is the directory of the data library's products, of which only a named city model is read.
inline Json answer(const Json& r, const std::string& data = ".") {
  const std::string op = r["op"].text();
  if (op == "costs") return op_costs(r, data);
  if (op == "market") return op_market(r);
  if (op == "offer") return op_offer(r);
  if (op == "enter") return op_enter(r);
  if (op == "sites") return op_sites(r);
  if (op == "size") return op_size(r);
  if (op == "structure") return op_structure(r);
  if (op == "survival") return op_survival(r);
  if (op == "status") return Json::object({{"ok", true}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

}  // namespace science::markets
