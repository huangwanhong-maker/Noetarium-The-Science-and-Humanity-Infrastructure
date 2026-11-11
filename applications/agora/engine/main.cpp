// Agora's engine: a market in a city, composed for this work from the markets domain (buyers, offers and the kernel between them),
// the urban-science domain's access (minutes walked on the streets) and the data library's places of a market (market_places.py).
//
// It answers requests, one JSON object a line ({"op": …, …}), with one JSON line each ({"ok": true, "result": …} or
// {"ok": false, "error": …}): with --serve until its input ends, as the server keeps it; otherwise the one request given as an
// argument or on its input. --data names the directory of the data library's products.
//
// Every request names a city and a market, {"city": "melbourne", "market": {…}}: what is sold (an ANZSIC class of the census), who
// could buy (the residents of the blocks' dwellings and the workers of their offices, by two declared assumptions), what each
// brings, and Huff's model with its parameters. The kernel is PRESUMED: no transaction is held, and every parameter is declared.
//
//   market   {}                     the market: each offer's demand, the sellers and the structure, each block's spend and what of it is
//                                   unserved, the penetration
//   offer    {offer}                one offer: its trade area by block, its core at a share, its reach in minutes, where its custom
//                                   would go if it left (diversion), its overlap with those rivals
//   enter    {at: [x, y], seller}   a new outlet at a point: what it would sell, drawn from whom, the cannibalisation
//   sites    {k, seller, from}      where to open: k sites chosen greedily among the census's vacant spaces ("vacant") or the blocks'
//                                   centroids ("blocks"), for a seller of the market or a newcomer (seller −1)
//   size     {seller?, ranges}      the size of the market as a tree of data and assumptions with their ranges: bounds, elasticities,
//                                   swings; and for a seller what it could serve and what it sells
//   status   {}                     the engine answers
//
// A block's buyers stand throughout it, not at its centroid. The minutes from a block to a point are those walked on the streets from
// the centroid, and the walk within the block besides: the mean distance of a square's points from its centre, 0.3826 of its side,
// for a square of the block's area. Without it an outlet beside the centroid of a large block would be at no distance from all of
// its buyers. Outlets beyond the city model's box are not counted, so blocks at its edge read as less served than they are.
//
// Lengths are metres in the city model's frame (east and north of its origin), costs minutes, spends a currency a year.

#include <markets/market/market.hpp>
#include <markets/sizing/sizing.hpp>
#include <markets/space/space.hpp>

#include <urban/access/access.hpp>
#include <urban/access/catchment.hpp>

#include <science/json.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <memory>
#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace {

using science::Json;
namespace mm = markets::market;
namespace ms = markets::sizing;
namespace sp = markets::space;
namespace ua = urban::access;

std::string& data_dir() { static std::string d = "."; return d; }
double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
std::string text_or(const Json& j, const char* key, const std::string& fallback) { return j.has(key) && j[key].is_string() ? j[key].text() : fallback; }

std::string slurp(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::runtime_error("the data library has no " + path + ": run the recipe");
  std::ostringstream s;
  s << in.rdbuf();
  return s.str();
}

// ---- a city: its blocks, its places of business, its streets ------------------------------------------------------------------

struct City {
  int year = 0;
  std::vector<std::string> block_id;
  std::vector<std::pair<double, double>> block_at;
  std::vector<double> dwellings, office;  // a cell the census withheld counts as none, and is counted as withheld
  std::vector<double> within;             // minutes walked within each block, from where its buyers stand to its centroid
  std::size_t withheld_dwellings = 0, withheld_office = 0;
  std::vector<std::string> place_name, place_code;
  std::vector<std::pair<double, double>> place_at;
  ua::Network network;
};

// The ways one may not walk; steps are slower.
double speed_of(const std::string& kind, double walk) {
  static const char* closed[] = {"motorway", "motorway_link", "construction", "proposed", "raceway", "busway", "bus_guideway", "rail"};
  for (const char* k : closed)
    if (kind == k) return 0;
  return kind == "steps" ? 0.45 * walk : walk;
}

const City& city(const std::string& id) {
  static std::map<std::string, std::unique_ptr<City>> loaded;
  auto found = loaded.find(id);
  if (found != loaded.end()) return *found->second;
  for (const char c : id)
    if (!std::isalnum(static_cast<unsigned char>(c)) && c != '-') throw std::invalid_argument("a city is named by letters, digits and -");
  const Json product = Json::parse(slurp(data_dir() + "/market-" + id + ".json"));
  auto C = std::make_unique<City>();
  C->year = static_cast<int>(product["year"].as_number());
  const Json& blocks = product["blocks"];
  for (std::size_t k = 0; k < blocks.size(); k++) {
    const Json& b = blocks[k];
    C->block_id.push_back(b["id"].text());
    C->block_at.push_back({b["x"].as_number(), b["y"].as_number()});
    C->withheld_dwellings += !b["dwellings"].is_number(), C->withheld_office += !b["office_m2"].is_number();
    C->dwellings.push_back(number_or(b, "dwellings", 0)), C->office.push_back(number_or(b, "office_m2", 0));
    C->within.push_back(0.3826 * std::sqrt(std::max(0.0, number_or(b, "area_m2", 0))) / 1.34 / 60);
  }
  const Json& places = product["places"];
  for (std::size_t k = 0; k < places["name"].size(); k++) {
    C->place_name.push_back(places["name"][k].text()), C->place_code.push_back(places["code"][k].text());
    C->place_at.push_back({places["x"][k].as_number(), places["y"][k].as_number()});
  }
  const Json model = Json::parse(slurp(data_dir() + "/" + product["model"].text()));
  std::vector<ua::Way> ways;
  const Json& roads = model["roads"];
  for (std::size_t k = 0; k < roads.size(); k++) {
    ua::Way way;
    way.speed = speed_of(roads[k]["kind"].text(), 1.34);
    const Json& line = roads[k]["line"];
    for (std::size_t i = 0; i < line.size(); i++) way.xy.push_back(line[i][0].as_number()), way.xy.push_back(line[i][1].as_number());
    if (way.xy.size() >= 4) ways.push_back(std::move(way));
  }
  C->network = ua::network_of(ways, 1.5);
  ua::stitch_ends(C->network, 8, 1.34);
  return *(loaded[id] = std::move(C));
}

// ---- a market, as a request states it ---------------------------------------------------------------------------------------------

struct Stated {
  std::string kind = "4511";
  double residents_per_dwelling = 2.0, workers_per_100m2 = 5.0;  // assumptions: the census counts dwellings and floor, not people
  double spend_resident = 30, spend_worker = 25, weeks = 48;     // a week, in the market's currency; the weeks of a year that count
  sp::Huff huff;                                                 // costs in minutes
  double limit = 15;                                             // minutes: nothing beyond is within reach
  double outside = 5;                                            // not buying pulls as an outlet this many minutes away; 0: no such pull
};

Stated stated(const Json& m) {
  Stated s;
  s.kind = text_or(m, "kind", s.kind);
  s.residents_per_dwelling = number_or(m, "residents_per_dwelling", s.residents_per_dwelling);
  s.workers_per_100m2 = number_or(m, "workers_per_100m2", s.workers_per_100m2);
  s.spend_resident = number_or(m, "spend_resident", s.spend_resident), s.spend_worker = number_or(m, "spend_worker", s.spend_worker);
  s.weeks = number_or(m, "weeks", s.weeks);
  s.huff.alpha = number_or(m, "alpha", 1), s.huff.beta = number_or(m, "beta", 2), s.huff.floor = number_or(m, "floor", 1);
  s.huff.decay = text_or(m, "decay", "power") == "exponential" ? sp::Decay::exponential : sp::Decay::power;
  s.limit = number_or(m, "limit", s.limit), s.outside = number_or(m, "outside", s.outside);
  const auto positive = [](double v) { return v > 0 && std::isfinite(v); };
  if (!positive(s.residents_per_dwelling) || !positive(s.workers_per_100m2) || !positive(s.weeks) || s.spend_resident < 0 || s.spend_worker < 0 || !positive(s.limit) || s.limit > 60
      || s.outside < 0 || !(s.huff.beta >= 0) || !positive(s.huff.floor))
    throw std::invalid_argument("the market's assumptions are positive numbers, its reach at most an hour");
  return s;
}

std::string seller_key(const std::string& name) {
  std::string out;
  bool space = false;
  for (const char c : name) {
    if (std::isspace(static_cast<unsigned char>(c))) { space = !out.empty(); continue; }
    if (space) out += ' ', space = false;
    out += static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  }
  return out;
}

// The market computed: cell 2i is the residents of block i, cell 2i + 1 its workers.
struct Computed {
  std::string key;
  const City* C = nullptr;
  Stated s;
  std::vector<std::size_t> place;        // of each offer
  std::vector<std::string> seller_name;  // of each seller
  std::vector<sp::Reachable> pairs;      // cells × offers within reach, minutes
  std::vector<double> outside;
  mm::Logit rule;
  mm::Market M;
  mm::Demand D;
};

// The cells within reach of points, with their minutes: residents and workers of a block share its place.
std::vector<sp::Reachable> cells_within(const City& C, const std::vector<std::pair<double, double>>& points, double minutes) {
  std::vector<sp::Reachable> out;
  for (const auto& p : ua::pairs_within(C.network, C.block_at, points, minutes * 60, 1.34)) {
    const double cost = p.cost / 60 + C.within[p.demand];
    out.push_back({2 * p.demand, p.supply, cost});
    out.push_back({2 * p.demand + 1, p.supply, cost});
  }
  return out;
}

const Computed& computed(const Json& r) {
  static Computed last;
  const std::string id = text_or(r, "city", "melbourne");
  const Json& m = r.has("market") ? r["market"] : Json::object();
  const std::string key = id + "|" + m.dump();
  if (last.key == key) return last;
  Computed X;
  X.C = &city(id);
  X.s = stated(m);
  const City& C = *X.C;
  std::map<std::string, std::size_t> sellers;
  std::vector<std::pair<double, double>> at;
  for (std::size_t k = 0; k < C.place_code.size(); k++) {
    if (C.place_code[k] != X.s.kind) continue;
    // A place without a name is a seller of its own.
    const std::string name = seller_key(C.place_name[k]), who = name.empty() ? "(unnamed " + std::to_string(k) + ")" : name;
    const auto [it, fresh] = sellers.try_emplace(who, sellers.size());
    if (fresh) X.seller_name.push_back(name.empty() ? "(unnamed)" : C.place_name[k]);
    X.place.push_back(k), X.M.seller.push_back(it->second), at.push_back(C.place_at[k]);
  }
  if (at.empty()) throw std::invalid_argument("the census holds no place of the kind " + X.s.kind + " here");
  for (std::size_t b = 0; b < C.block_id.size(); b++) {
    X.M.buyers.mass.push_back(C.dwellings[b] * X.s.residents_per_dwelling), X.M.buyers.spend.push_back(X.s.spend_resident * X.s.weeks);
    X.M.buyers.mass.push_back(C.office[b] / 100 * X.s.workers_per_100m2), X.M.buyers.spend.push_back(X.s.spend_worker * X.s.weeks);
  }
  X.pairs = cells_within(C, at, X.s.limit);
  const std::vector<double> attraction(at.size(), 1.0);  // the census gives no size of an establishment
  X.outside.assign(X.M.buyers.size(), X.s.outside > 0 ? sp::weight(X.s.huff, 1.0, X.s.outside) : 0.0);
  X.rule = sp::rule(X.s.huff, X.M.buyers.size(), attraction, X.pairs, X.outside);
  X.M.kernel = mm::kernel(X.rule);
  mm::validate(X.M);
  X.D = mm::demand(X.M);
  X.key = key;
  return last = std::move(X);
}

Json round_all(const std::vector<double>& v, double unit = 1) {
  Json out = Json::array();
  for (double x : v) out.push(std::round(x / unit) * unit);
  return out;
}

// ---- the operations -----------------------------------------------------------------------------------------------------------------

Json op_market(const Json& r) {
  const Computed& X = computed(r);
  const City& C = *X.C;
  const auto by = mm::by_seller(X.M, X.D.offers);
  const auto S = mm::structure(by);
  std::vector<double> outlets(by.size(), 0.0), brings, unserved, residents, workers;
  for (std::size_t s : X.M.seller) outlets[s] += 1;
  const auto u = mm::unserved(X.M);
  for (std::size_t b = 0; b < C.block_id.size(); b++) {
    brings.push_back(X.M.buyers.brings(2 * b) + X.M.buyers.brings(2 * b + 1)), unserved.push_back(u[2 * b] + u[2 * b + 1]);
    residents.push_back(X.M.buyers.mass[2 * b]), workers.push_back(X.M.buyers.mass[2 * b + 1]);
  }
  double minutes = 0;  // the mean minutes walked by the spend that buys
  for (const auto& p : X.pairs) minutes += X.M.buyers.brings(p.demand) * X.M.kernel.shares.at(p.demand, p.supply) * p.cost;
  Json names = Json::array();
  for (const auto& n : X.seller_name) names.push(n);
  Json place = Json::array(), seller = Json::array();
  for (std::size_t o = 0; o < X.place.size(); o++) place.push(X.place[o]), seller.push(X.M.seller[o]);
  return Json::object({
      {"year", C.year}, {"kind", X.s.kind}, {"provenance", "presumed"}, {"offers", X.place.size()}, {"sellers", by.size()}, {"pairs", X.pairs.size()},
      {"total", X.D.total}, {"bought", X.D.bought()}, {"penetration", X.D.penetration()}, {"minutes", X.D.bought() > 0 ? minutes / X.D.bought() : 0.0},
      {"buyers", Json::object({{"residents", std::accumulate(residents.begin(), residents.end(), 0.0)}, {"workers", std::accumulate(workers.begin(), workers.end(), 0.0)},
                               {"withheld_dwellings", C.withheld_dwellings}, {"withheld_office", C.withheld_office}})},
      {"structure", Json::object({{"hhi", S.hhi}, {"cr4", S.cr4}, {"equivalent", S.equivalent}, {"selling", S.sellers}})},
      {"offer", Json::object({{"place", place}, {"seller", seller}, {"demand", round_all(X.D.offers)}})},
      {"seller", Json::object({{"name", names}, {"outlets", Json(outlets)}, {"demand", round_all(by)}})},
      {"block", Json::object({{"brings", round_all(brings)}, {"unserved", round_all(unserved)}, {"residents", round_all(residents)}, {"workers", round_all(workers)}})},
  });
}

std::size_t offer_of(const Computed& X, const Json& r) {
  const double o = number_or(r, "offer", -1);
  if (!(o >= 0 && o < static_cast<double>(X.place.size()))) throw std::invalid_argument("an offer of the market, by its index");
  return static_cast<std::size_t>(o);
}

Json op_offer(const Json& r) {
  const Computed& X = computed(r);
  const City& C = *X.C;
  const std::size_t o = offer_of(X, r);
  if (!(X.D.offers[o] > 0)) throw std::invalid_argument("this outlet sells nothing: no buyers are within its reach");
  const double share = std::clamp(number_or(r, "share", 0.6), 0.05, 1.0);
  const auto t = mm::trade_area(X.M, o);
  std::vector<double> by_block(C.block_id.size(), 0.0);
  for (std::size_t b = 0; b < by_block.size(); b++) by_block[b] = t[2 * b] + t[2 * b + 1];
  const auto core = mm::core(X.M, o, share);
  std::vector<double> in_core(C.block_id.size(), 0.0);  // the part of each block's custom that lies in the core
  for (std::size_t k = 0; k < core.cells.size(); k++) in_core[core.cells[k] / 2] += core.part[k] * t[core.cells[k]];
  Json core_blocks = Json::array();
  for (std::size_t b = 0; b < in_core.size(); b++)
    if (in_core[b] > 0) core_blocks.push(Json::array({Json(b), Json(by_block[b] > 0 ? in_core[b] / by_block[b] : 0.0)}));
  const auto reach = sp::reach(X.M, X.pairs, o);
  const auto d = mm::diversion_closed(X.M, o);
  std::vector<std::size_t> rivals(d.to.size());
  std::iota(rivals.begin(), rivals.end(), std::size_t{0});
  std::sort(rivals.begin(), rivals.end(), [&](std::size_t a, std::size_t b) { return d.to[a] > d.to[b]; });
  Json to = Json::array();
  double own = 0;  // to the same seller's other outlets
  for (std::size_t j = 0; j < d.to.size(); j++)
    if (j != o && X.M.seller[j] == X.M.seller[o]) own += d.to[j];
  for (std::size_t k = 0; k < rivals.size() && k < 12 && d.to[rivals[k]] > 0; k++)
    to.push(Json::object({{"offer", rivals[k]}, {"part", d.to[rivals[k]]}, {"overlap", mm::overlap(X.M, o, rivals[k])}}));
  return Json::object({
      {"offer", o}, {"place", X.place[o]}, {"seller", X.M.seller[o]}, {"demand", X.D.offers[o]}, {"share", X.D.offers[o] / X.D.bought()},
      {"trade", round_all(by_block)},
      {"core", Json::object({{"share", share}, {"blocks", core_blocks}, {"buyers", core.mass}, {"custom", core.custom}})},
      {"reach", Json::object({{"mean", reach.mean}, {"half", reach.within(0.5)}, {"most", reach.within(0.8)}})},
      {"diversion", Json::object({{"to", to}, {"outside", d.outside}, {"own", own}})},
  });
}

// The seller a request names: one of the market's, or a newcomer (−1), who is one more.
std::size_t seller_of(const Computed& X, const Json& r) {
  const double s = number_or(r, "seller", -1);
  if (s < 0) return X.seller_name.size();
  if (!(s < static_cast<double>(X.seller_name.size()))) throw std::invalid_argument("a seller of the market, by its index, or −1 for a newcomer");
  return static_cast<std::size_t>(s);
}

sp::Site site_at(const Computed& X, std::pair<double, double> at, double attraction) {
  sp::Site site;
  site.attraction = attraction;
  for (const auto& p : cells_within(*X.C, {at}, X.s.limit)) site.reach.push_back({p.demand, p.cost});
  return site;
}

Json op_enter(const Json& r) {
  const Computed& X = computed(r);
  if (!r.has("at") || r["at"].size() != 2) throw std::invalid_argument("a new outlet stands at a point [x, y]");
  const std::size_t seller = seller_of(X, r);
  const auto site = site_at(X, {r["at"][0].as_number(), r["at"][1].as_number()}, 1.0);
  if (site.reach.empty()) throw std::invalid_argument("no buyers are within reach of this point");
  const auto e = sp::enter(X.M.buyers, X.M.seller, X.rule, X.s.huff, site, seller);
  std::vector<std::size_t> rivals(e.from.size());
  std::iota(rivals.begin(), rivals.end(), std::size_t{0});
  std::sort(rivals.begin(), rivals.end(), [&](std::size_t a, std::size_t b) { return e.from[a] > e.from[b]; });
  Json from = Json::array();
  for (std::size_t k = 0; k < rivals.size() && k < 12 && e.from[rivals[k]] > 0; k++) from.push(Json::object({{"offer", rivals[k]}, {"amount", e.from[rivals[k]]}}));
  std::vector<double> by_block(X.C->block_id.size(), 0.0);  // its trade area, were it to open
  for (const auto& [b, cost] : site.reach) {
    const double u = sp::weight(X.s.huff, 1.0, cost), total = X.outside[b] + X.rule.weights.sum(b);
    by_block[b / 2] += X.M.buyers.brings(b) * u / (total + u);
  }
  return Json::object({{"demand", e.demand}, {"from", from}, {"from_outside", e.from_outside}, {"cannibalised", e.cannibalised}, {"gain", e.gain()},
                       {"from_rivals", e.demand - e.from_outside - e.cannibalised}, {"trade", round_all(by_block)}, {"share", e.demand / (X.D.bought() + e.from_outside)}});
}

Json op_sites(const Json& r) {
  const Computed& X = computed(r);
  const City& C = *X.C;
  const std::size_t seller = seller_of(X, r), k = static_cast<std::size_t>(std::clamp(number_or(r, "k", 3), 1.0, 12.0));
  // The candidates: the census's vacant spaces, one for each 25 m of the plane, or the blocks' centroids.
  std::vector<std::pair<double, double>> at;
  if (text_or(r, "from", "vacant") == "blocks") at = C.block_at;
  else {
    std::map<std::pair<long, long>, bool> seen;
    for (std::size_t p = 0; p < C.place_code.size(); p++)
      if (C.place_code[p] == "0000" && seen.try_emplace(std::pair<long, long>{std::lround(C.place_at[p].first / 25), std::lround(C.place_at[p].second / 25)}, true).second)
        at.push_back(C.place_at[p]);
  }
  if (at.empty()) throw std::invalid_argument("there are no such places here");
  std::vector<sp::Site> sites(at.size());
  for (const auto& p : cells_within(C, at, X.s.limit)) sites[p.supply].reach.push_back({p.demand, p.cost});
  const auto chosen = sp::choose(X.M.buyers, X.M.seller, X.rule, X.s.huff, sites, std::min(k, sites.size()), seller);
  Json out = Json::array();
  for (std::size_t i = 0; i < chosen.sites.size(); i++) out.push(Json::object({{"x", at[chosen.sites[i]].first}, {"y", at[chosen.sites[i]].second}, {"gain", chosen.gains[i]}}));
  return Json::object({{"candidates", at.size()}, {"chosen", out}, {"before", chosen.before}, {"after", chosen.after}, {"bound", 1 - std::exp(-1.0)}});
}

ms::Range range_of(const Json& ranges, const char* name, double mid) {
  if (ranges.has(name) && ranges[name].size() == 2) return {ranges[name][0].as_number(), mid, ranges[name][1].as_number()};
  return ms::exactly(mid);
}

Json op_size(const Json& r) {
  const Computed& X = computed(r);
  const City& C = *X.C;
  const Json& ranges = r.has("ranges") ? r["ranges"] : Json::object();
  const std::string census = "CLUE " + std::to_string(C.year);
  ms::Tree T;
  const auto dwellings = T.leaf("dwellings", ms::exactly(std::accumulate(C.dwellings.begin(), C.dwellings.end(), 0.0)), census, false);
  const auto per_dwelling = T.leaf("residents_per_dwelling", range_of(ranges, "residents_per_dwelling", X.s.residents_per_dwelling));
  const auto spend_r = T.leaf("spend_resident", range_of(ranges, "spend_resident", X.s.spend_resident));
  const auto office = T.leaf("office_100m2", ms::exactly(std::accumulate(C.office.begin(), C.office.end(), 0.0) / 100), census, false);
  const auto per_floor = T.leaf("workers_per_100m2", range_of(ranges, "workers_per_100m2", X.s.workers_per_100m2));
  const auto spend_w = T.leaf("spend_worker", range_of(ranges, "spend_worker", X.s.spend_worker));
  const auto weeks = T.leaf("weeks", range_of(ranges, "weeks", X.s.weeks));
  const auto residents = T.product("residents' spend", {dwellings, per_dwelling, spend_r, weeks});
  const auto workers = T.product("workers' spend", {office, per_floor, spend_w, weeks});
  T.sum("all who could buy", {residents, workers});
  const auto R = ms::evaluate(T);
  const auto eps = ms::elasticities(T);
  Json nodes = Json::array(), swings = Json::array();
  for (std::size_t i = 0; i < T.nodes.size(); i++) {
    const auto& n = T.nodes[i];
    nodes.push(Json::object({{"name", n.name}, {"kind", n.kind == ms::Node::Kind::leaf ? "leaf" : n.kind == ms::Node::Kind::sum ? "sum" : "product"}, {"low", R[i].low}, {"mid", R[i].mid},
                             {"high", R[i].high}, {"source", n.source}, {"assumed", n.kind == ms::Node::Kind::leaf && n.assumed}, {"elasticity", eps[i]}}));
  }
  for (const auto& w : ms::swings(T))
    if (w.width() > 0) swings.push(Json::object({{"leaf", T.nodes[w.leaf].name}, {"low", w.low}, {"high", w.high}}));
  Json answer = Json::object({{"nodes", nodes}, {"swings", swings}, {"all", Json::object({{"low", R[T.root()].low}, {"mid", R[T.root()].mid}, {"high", R[T.root()].high}})},
                              {"bought", X.D.bought()}});
  // A seller: what lies within reach of its outlets, and what it sells.
  if (number_or(r, "seller", -1) >= 0) {
    const std::size_t s = seller_of(X, r);
    std::vector<char> reached(X.M.buyers.size(), 0);
    for (const auto& p : X.pairs)
      if (X.M.seller[p.supply] == s) reached[p.demand] = 1;
    double served = 0;
    for (std::size_t b = 0; b < reached.size(); b++)
      if (reached[b]) served += X.M.buyers.brings(b);
    answer.set("seller", Json::object({{"index", s}, {"served", served}, {"sells", mm::by_seller(X.M, X.D.offers)[s]}}));
  }
  return answer;
}

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "market") return op_market(r);
  if (op == "offer") return op_offer(r);
  if (op == "enter") return op_enter(r);
  if (op == "sites") return op_sites(r);
  if (op == "size") return op_size(r);
  if (op == "status") return Json::object({{"ok", true}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

std::string respond(const std::string& line) {
  Json reply = Json::object();
  try {
    const Json r = Json::parse(line);
    if (r.has("id")) reply.set("id", r["id"]);
    reply.set("ok", true);
    reply.set("result", answer(r));
  } catch (const std::exception& e) {
    reply = Json::object({{"ok", false}, {"error", e.what()}});
  }
  return reply.dump();
}

}  // namespace

int main(int argc, char** argv) {
  std::ios::sync_with_stdio(false);
  bool serve = false;
  std::string request;
  for (int i = 1; i < argc; i++) {
    if (std::strcmp(argv[i], "--serve") == 0) serve = true;
    else if (std::strcmp(argv[i], "--data") == 0 && i + 1 < argc) data_dir() = argv[++i];
    else request = argv[i];
  }
  if (serve) {
    std::string line;
    while (std::getline(std::cin, line)) {
      if (line.empty()) continue;
      std::cout << respond(line) << "\n" << std::flush;
    }
    return 0;
  }
  if (request.empty()) std::getline(std::cin, request);
  std::cout << respond(request) << "\n";
  return 0;
}
