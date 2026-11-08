// The engine of the plug-in "Geometric syntax" (ADR 0083): syntaxis's grammars of geometry and topology on what the map shows,
// through urban-science's and architecture's syntax.
//
// It answers requests, one JSON object a line ({"op": …, …}), with one JSON line each ({"ok": true, "result": …} or
// {"ok": false, "error": …}): with --serve until its input ends, as the workbench's server keeps it; otherwise the one request given.
//
//   learn     {ways?: [[x0, y0, x1, y1, …], …] | example?: "sierpinski" | "bridged" | "grid", depth?, tolerance?, maxOrder?, snap?, bend?}
//                         the street graph (or the example), the grammar learned from it, every step of learning, the derivation tree,
//                         and its replay; kept as the engine's grammar for parse and sample
//   parse     {ways? | example?, grammar?}
//                         the syntax analysis of a graph by the grammar given (or the one kept): every step, the parse forest, its cover
//   sample    {kind: "pcfg" | "pcsg", seed?, merge?, count?}
//                         graphs drawn from the probabilistic (or context-sensitive) grammar made from the kept one, with statistics
//   words     {ways? | rings?, degrees?, maxArea?, seed?, grow?}
//                         a 2-complex (the blocks of the streets, or footprints) as words: each triangle's letter, the gluing grammar,
//                         its entropies, and a complex grown from it
//   replace   {weights?: [stop, pachner, midpoint, sierpinski], depth?, seed?}
//                         a triangle rewritten by the replacement grammar, and the syntax analysis of its leaves
//   configuration {ways | example?, cost?: "angular" | "metric" | "topological", radius? (m; 0 for n), turn?, points?: [[x, y], …],
//                  counters?: [{id, x, y}, …]}
//                         the street network's segment map and each segment's integration and choice (urban-science's configuration,
//                         ADR 0084), and the segment nearest each point with its measures; for counters of the City of Melbourne
//                         (the data library's street-counts), each one's mean daily count over the live record's full days, against
//                         its street's measures (Spearman's rho, and R^2 of the log count on each measure)
//   status    {}

#include <architecture/syntax/syntax.hpp>
#include <urban/syntax/configuration.hpp>
#include <urban/syntax/syntax.hpp>

#include <syntaxis/derivation/derivation.hpp>
#include <syntaxis/induction/induction.hpp>
#include <syntaxis/parsing/parsing.hpp>
#include <syntaxis/simplicial/simplicial.hpp>
#include <syntaxis/stochastic/stochastic.hpp>

#include <calx/random/stream.hpp>
#include <science/json.hpp>

#include <chrono>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <map>
#include <algorithm>
#include <optional>
#include <string>
#include <thread>
#include <vector>

namespace {

using science::Json;
namespace sx = syntaxis;
using sx::graphs::Graph;
using sx::shape::Point;
using sx::shape::Similarity;

double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
std::string text_or(const Json& j, const char* key, const std::string& fallback) { return j.has(key) && j[key].is_string() ? j[key].text() : fallback; }
Json pt(Point p) { return Json::array({Json(std::round(p.real() * 1000) / 1000), Json(std::round(p.imag() * 1000) / 1000)}); }
Json sim(const Similarity& s) { return Json::array({Json(s.a.real()), Json(s.a.imag()), Json(s.b.real()), Json(s.b.imag())}); }
Similarity sim_of(const Json& j) { return {Point{j[0].as_number(), j[1].as_number()}, Point{j[2].as_number(), j[3].as_number()}}; }

Json graph_json(const Graph& g) {
  Json nodes = Json::array(), edges = Json::array();
  for (const auto& n : g.nodes) {
    Json x = pt(n.at);
    x.push(Json(static_cast<double>(n.symbol)));
    nodes.push(std::move(x));
  }
  for (const auto& [u, v] : g.edges()) edges.push(Json::array({Json(static_cast<double>(u)), Json(static_cast<double>(v))}));
  return Json::object({{"nodes", nodes}, {"edges", edges}});
}

// The graph a request names: its ways (the street graph), or an example; carried by `transform` ([a.re, a.im, b.re, b.im]) if given.
Graph graph_named(const Json& r);
Graph graph_of(const Json& r) {
  Graph g = graph_named(r);
  if (r.has("transform")) g = sx::graphs::transformed(g, sim_of(r["transform"]));
  return g;
}
Graph graph_named(const Json& r) {
  if (r.has("ways")) {
    std::vector<std::vector<double>> ways;
    for (std::size_t k = 0; k < r["ways"].size(); k++) {
      std::vector<double> xy;
      for (std::size_t i = 0; i < r["ways"][k].size(); i++) xy.push_back(r["ways"][k][i].as_number());
      if (xy.size() >= 4) ways.push_back(std::move(xy));
    }
    return urban::syntax::street_graph(ways, number_or(r, "snap", 1.5), number_or(r, "bend", 20));
  }
  const std::string ex = text_or(r, "example", "sierpinski");
  const int depth = static_cast<int>(number_or(r, "depth", 4));
  if (ex == "bridged") return sx::graphs::bridged_triangles(depth);
  if (ex == "grid") return sx::graphs::grid(static_cast<std::size_t>(number_or(r, "rows", 8)), static_cast<std::size_t>(number_or(r, "columns", 8)));
  return sx::graphs::sierpinski(depth);
}

// ---- grammars as JSON, both ways -----------------------------------------------------------------------------------------

Json grammar_json(const sx::induction::Grammar& G) {
  Json rules = Json::array();
  for (const auto& r : G.rules) {
    Json nodes = Json::array(), edges = Json::array();
    for (const auto& n : r.nodes) nodes.push(Json::object({{"at", Json::array({Json(n.at.real()), Json(n.at.imag())})}, {"symbol", Json(static_cast<double>(n.symbol))}, {"frame", sim(n.frame)}}));
    for (const auto& [a, b] : r.edges) edges.push(Json::array({Json(static_cast<double>(a)), Json(static_cast<double>(b))}));
    rules.push(Json::object({{"symbol", Json(static_cast<double>(r.symbol))}, {"key", Json(r.key)}, {"uses", Json(static_cast<double>(r.uses))}, {"level", Json(static_cast<double>(r.level))},
                             {"nodes", nodes}, {"edges", edges}}));
  }
  Json an = Json::array(), ae = Json::array();
  for (const auto& n : G.axiom.nodes) an.push(Json::object({{"at", Json::array({Json(n.at.real()), Json(n.at.imag())})}, {"symbol", Json(static_cast<double>(n.symbol))}, {"frame", sim(n.frame)}}));
  for (const auto& [u, v] : G.axiom.edges()) ae.push(Json::array({Json(static_cast<double>(u)), Json(static_cast<double>(v))}));
  const Json axiom = Json::object({{"nodes", an}, {"edges", ae}});
  return Json::object({{"schema", Json("noetarium.grammar/1")}, {"kind", Json("geometric graph grammar, node replacement, straight-line")},
                       {"tolerance", Json(G.tolerance)}, {"rules", rules}, {"axiom", axiom}});
}

sx::induction::Grammar grammar_of(const Json& j) {
  sx::induction::Grammar G;
  G.tolerance = j["tolerance"].as_number();
  for (std::size_t k = 0; k < j["rules"].size(); k++) {
    const Json& r = j["rules"][k];
    sx::induction::Rule rule;
    rule.symbol = static_cast<int>(r["symbol"].as_number());
    rule.key = r["key"].text();
    rule.uses = static_cast<std::size_t>(number_or(r, "uses", 0));
    rule.level = static_cast<std::size_t>(number_or(r, "level", 0));
    for (std::size_t i = 0; i < r["nodes"].size(); i++) {
      const Json& n = r["nodes"][i];
      rule.nodes.push_back({Point{n["at"][0].as_number(), n["at"][1].as_number()}, static_cast<int>(n["symbol"].as_number()), sim_of(n["frame"])});
    }
    for (std::size_t i = 0; i < r["edges"].size(); i++) rule.edges.push_back({static_cast<std::size_t>(r["edges"][i][0].as_number()), static_cast<std::size_t>(r["edges"][i][1].as_number())});
    G.rules.push_back(std::move(rule));
  }
  const Json& ax = j["axiom"];
  for (std::size_t i = 0; i < ax["nodes"].size(); i++) {
    const Json& n = ax["nodes"][i];
    G.axiom.add(Point{n["at"][0].as_number(), n["at"][1].as_number()}, static_cast<int>(n["symbol"].as_number()), sim_of(n["frame"]));
  }
  for (std::size_t i = 0; i < ax["edges"].size(); i++) G.axiom.connect(static_cast<std::size_t>(ax["edges"][i][0].as_number()), static_cast<std::size_t>(ax["edges"][i][1].as_number()));
  return G;
}

// The leaves (input nodes) under each instance, for drawing what a step replaced.
std::vector<std::vector<std::size_t>> leaves_of(const std::vector<sx::induction::Instance>& I) {
  std::vector<std::vector<std::size_t>> out(I.size());
  for (std::size_t i = 0; i < I.size(); i++) {
    if (I[i].children.empty()) { if (I[i].terminal != sx::induction::none) out[i] = {I[i].terminal}; continue; }
    for (std::size_t c : I[i].children) out[i].insert(out[i].end(), out[c].begin(), out[c].end());
  }
  return out;
}

Json tree_json(const std::vector<sx::induction::Instance>& I, const std::vector<std::size_t>& roots) {
  Json nodes = Json::array();
  for (const auto& in : I) {
    Json ch = Json::array();
    for (std::size_t c : in.children) ch.push(Json(static_cast<double>(c)));
    nodes.push(Json::object({{"symbol", Json(static_cast<double>(in.symbol))}, {"at", pt(in.at)}, {"level", Json(static_cast<double>(in.level))},
                             {"terminal", in.terminal == sx::induction::none ? Json() : Json(static_cast<double>(in.terminal))}, {"children", ch}}));
  }
  Json r = Json::array();
  for (std::size_t v : roots) r.push(Json(static_cast<double>(v)));
  return Json::object({{"schema", Json("noetarium.derivation/1")}, {"instances", nodes}, {"roots", r}});
}

Json steps_json(const std::vector<sx::induction::Step>& steps, const std::vector<sx::induction::Instance>& I) {
  const auto leaves = leaves_of(I);
  Json out = Json::array();
  for (const auto& s : steps) {
    Json occ = Json::array(), made = Json::array();
    for (std::size_t m : s.made) {
      Json l = Json::array();
      for (std::size_t v : leaves[m]) l.push(Json(static_cast<double>(v)));
      occ.push(l);
      made.push(pt(I[m].at));
    }
    out.push(Json::object({{"symbol", Json(static_cast<double>(s.symbol))}, {"order", Json(static_cast<double>(s.order))}, {"occurrences", occ}, {"made", made},
                           {"nodes", Json(static_cast<double>(s.before.size()))}}));
  }
  return out;
}

std::optional<sx::induction::Learned>& kept() { static std::optional<sx::induction::Learned> L; return L; }
Graph& kept_input() { static Graph g; return g; }

std::string& data_dir() { static std::string d; return d; }

// A counter's mean daily count, both directions, over the 24-hour windows of the live record (from its t0, a local midnight) with
// every hour counted; NaN with fewer than 60 such days. Read once from the data library's street-counts/<id>.json and kept.
double mean_daily(int id) {
  static std::map<int, double> kept_means;
  if (const auto it = kept_means.find(id); it != kept_means.end()) return it->second;
  double mean = std::numeric_limits<double>::quiet_NaN();
  std::ifstream in(data_dir() + "/street-counts/" + std::to_string(id) + ".json", std::ios::binary);
  if (in) {
    std::ostringstream text;
    text << in.rdbuf();
    const Json j = Json::parse(text.str());
    if (j.has("directions")) {
      const Json& d1 = j["directions"]["d1"];
      const Json& d2 = j["directions"]["d2"];
      double total = 0;
      std::size_t days = 0;
      for (std::size_t day = 0; (day + 1) * 24 <= std::min(d1.size(), d2.size()); day++) {
        double sum = 0;
        bool full = true;
        for (std::size_t h = day * 24; h < (day + 1) * 24 && full; h++) {
          if (!d1[h].is_number() || !d2[h].is_number()) full = false;
          else sum += d1[h].as_number() + d2[h].as_number();
        }
        if (full) total += sum, days++;
      }
      if (days >= 60) mean = total / static_cast<double>(days);
    }
  }
  kept_means[id] = mean;
  return mean;
}

// Spearman's rho, ties at their mean rank.
double spearman_rho(const std::vector<double>& x, const std::vector<double>& y) {
  const auto ranks = [](const std::vector<double>& v) {
    std::vector<std::size_t> o(v.size());
    for (std::size_t i = 0; i < o.size(); i++) o[i] = i;
    std::sort(o.begin(), o.end(), [&](std::size_t a, std::size_t b) { return v[a] < v[b]; });
    std::vector<double> r(v.size());
    for (std::size_t i = 0; i < o.size();) {
      std::size_t j = i;
      while (j + 1 < o.size() && v[o[j + 1]] == v[o[i]]) j++;
      for (std::size_t k = i; k <= j; k++) r[o[k]] = (static_cast<double>(i) + static_cast<double>(j)) / 2;
      i = j + 1;
    }
    return r;
  };
  const auto rx = ranks(x), ry = ranks(y);
  const double n = static_cast<double>(x.size());
  double mx = 0, my = 0;
  for (std::size_t i = 0; i < x.size(); i++) mx += rx[i] / n, my += ry[i] / n;
  double sxy = 0, sxx = 0, syy = 0;
  for (std::size_t i = 0; i < x.size(); i++) sxy += (rx[i] - mx) * (ry[i] - my), sxx += (rx[i] - mx) * (rx[i] - mx), syy += (ry[i] - my) * (ry[i] - my);
  return sxx > 0 && syy > 0 ? sxy / std::sqrt(sxx * syy) : 0;
}

// R^2 of the least-squares line of y on x.
double line_r2(const std::vector<double>& x, const std::vector<double>& y) {
  const double n = static_cast<double>(x.size());
  double mx = 0, my = 0;
  for (std::size_t i = 0; i < x.size(); i++) mx += x[i] / n, my += y[i] / n;
  double sxy = 0, sxx = 0, syy = 0;
  for (std::size_t i = 0; i < x.size(); i++) sxy += (x[i] - mx) * (y[i] - my), sxx += (x[i] - mx) * (x[i] - mx), syy += (y[i] - my) * (y[i] - my);
  return sxx > 0 && syy > 0 ? sxy * sxy / (sxx * syy) : 0;
}

std::size_t threads() { return std::clamp<std::size_t>(std::thread::hardware_concurrency(), 1, 8); }

Json numbers(const std::vector<double>& v, double scale = 1000) {
  Json a = Json::array();
  for (double x : v) a.push(Json(std::round(x * scale) / scale));
  return a;
}

// A distribution's summary: its mean and its 10th, 50th and 90th percentiles.
Json summary(std::vector<double> v) {
  if (v.empty()) return Json::object({{"n", Json(0.0)}});
  double mean = 0;
  for (double x : v) mean += x;
  mean /= static_cast<double>(v.size());
  std::sort(v.begin(), v.end());
  const auto q = [&](double f) { return v[static_cast<std::size_t>(std::floor(f * static_cast<double>(v.size() - 1)))]; };
  return Json::object({{"n", Json(static_cast<double>(v.size()))}, {"mean", Json(mean)}, {"p10", Json(q(0.1))}, {"p50", Json(q(0.5))}, {"p90", Json(q(0.9))}});
}

// The two-sample Kolmogorov–Smirnov distance: the largest gap between two empirical distribution functions.
double ks_distance(std::vector<double> a, std::vector<double> b) {
  if (a.empty() || b.empty()) return 1;
  std::sort(a.begin(), a.end()), std::sort(b.begin(), b.end());
  std::size_t i = 0, j = 0;
  double d = 0;
  while (i < a.size() && j < b.size()) {
    const double x = std::min(a[i], b[j]);
    while (i < a.size() && a[i] <= x) i++;
    while (j < b.size() && b[j] <= x) j++;
    d = std::max(d, std::fabs(static_cast<double>(i) / a.size() - static_cast<double>(j) / b.size()));
  }
  return d;
}

urban::syntax::Cost cost_of(const std::string& c) {
  return c == "metric" ? urban::syntax::Cost::metric : c == "topological" ? urban::syntax::Cost::topological : urban::syntax::Cost::angular;
}

Json op_learn(const Json& r) {
  const Graph g = graph_of(r);
  if (g.size() < 3) throw std::runtime_error("too few junctions here to learn from: pick a place with streets");
  if (g.size() > 6000) throw std::runtime_error("too many junctions (" + std::to_string(g.size()) + "): pick a smaller radius");
  sx::induction::Options o;
  o.tolerance = number_or(r, "tolerance", 0.08);
  o.max_order = static_cast<std::size_t>(number_or(r, "maxOrder", 5));
  const auto t0 = std::chrono::steady_clock::now();
  sx::induction::Learned L = sx::induction::learn(g, o);
  const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  const auto rep = sx::derivation::replay(L, g);
  // The configuration of the streets each rule joins: angular NACH at radius n of the input's segments its instances swallowed.
  const auto map = urban::syntax::segment_map(g);
  const auto conf = urban::syntax::configuration(map, urban::syntax::Cost::angular, std::numeric_limits<double>::infinity(), threads());
  std::vector<double> rule_nach(L.grammar.rules.size(), 0), rule_count(L.grammar.rules.size(), 0);
  for (const auto& in : L.instances)
    if (in.symbol > 0)
      for (const std::size_t e : in.swallowed)
        if (e < conf.nach.size()) rule_nach[static_cast<std::size_t>(in.symbol - 1)] += conf.nach[e], rule_count[static_cast<std::size_t>(in.symbol - 1)] += 1;
  Json rules_conf = Json::array();
  for (std::size_t k = 0; k < rule_nach.size(); k++) rules_conf.push(rule_count[k] > 0 ? Json(rule_nach[k] / rule_count[k]) : Json());
  std::size_t description = L.grammar.axiom.size() + L.grammar.axiom.edge_count();
  for (const auto& rule : L.grammar.rules) description += rule.nodes.size() + rule.edges.size();
  Json result = Json::object({{"graph", graph_json(g)}, {"grammar", grammar_json(L.grammar)}, {"steps", steps_json(L.steps, L.instances)},
                              {"tree", tree_json(L.instances, L.roots)}, {"seconds", Json(seconds)},
                              {"description", Json(static_cast<double>(description))}, {"size", Json(static_cast<double>(g.size() + g.edge_count()))},
                              {"replay", Json::object({{"edges", Json(rep.graph.edges() == g.edges())}, {"worst", Json(rep.worst)}})},
                              {"configuration", Json::object({{"ruleNach", rules_conf}, {"nach", summary(conf.nach)}, {"integration", summary(conf.integration)}})}});
  kept_input() = g;
  kept() = std::move(L);
  return result;
}

Json op_parse(const Json& r) {
  sx::induction::Grammar G;
  if (r.has("grammar")) G = grammar_of(r["grammar"]);
  else if (kept()) G = kept()->grammar;
  else throw std::runtime_error("no grammar: learn one first, or give one");
  const Graph g = graph_of(r);
  if (g.size() > 6000) throw std::runtime_error("too many junctions (" + std::to_string(g.size()) + "): pick a smaller radius");
  const auto P = sx::parsing::parse(G, g);
  Json uses = Json::array();
  for (auto u : P.uses) uses.push(Json(static_cast<double>(u)));
  return Json::object({{"graph", graph_json(g)}, {"steps", steps_json(P.steps, P.instances)}, {"tree", tree_json(P.instances, P.roots)}, {"uses", uses},
                       {"covered", Json(P.covered)}, {"derived", Json(P.derived)}, {"remaining", Json(static_cast<double>(P.remaining.size()))}});
}

Json stats_json(const sx::stochastic::Statistics& s) {
  Json d = Json::array();
  for (double v : s.degree) d.push(Json(v));
  return Json::object({{"nodes", Json(static_cast<double>(s.nodes))}, {"edges", Json(static_cast<double>(s.edges))}, {"crossings", Json(static_cast<double>(s.crossings))},
                       {"degree", d}, {"meanLength", Json(s.mean_length)}, {"cvLength", Json(s.cv_length)}});
}

Json op_sample(const Json& r) {
  if (!kept()) throw std::runtime_error("no grammar: learn one first");
  const auto m = sx::stochastic::model(*kept(), number_or(r, "merge", 2.0 * kept()->grammar.tolerance));
  sx::stochastic::SampleOptions o;
  o.kind = text_or(r, "kind", "pcfg") == "pcsg" ? sx::stochastic::Kind::pcsg : sx::stochastic::Kind::pcfg;
  o.max_depth = static_cast<std::size_t>(number_or(r, "depth", 10));
  calx::random::Stream st(static_cast<std::uint64_t>(number_or(r, "seed", 1)), 0x5a17u, 0, 0);
  const auto smp = sx::stochastic::sample(m, kept()->grammar.axiom, st, o);
  const auto a = sx::stochastic::statistics(kept_input(), kept_input().size() < 4000);
  const auto b = sx::stochastic::statistics(smp.graph, smp.graph.size() < 4000);
  Json cats = Json::array();
  for (const auto& c : m.categories) {
    Json rules = Json::array(), p = Json::array();
    for (int x : c.rules) rules.push(Json(static_cast<double>(x)));
    for (double v : c.probability) p.push(Json(v));
    cats.push(Json::object({{"rules", rules}, {"probability", p}}));
  }
  // The sample's configuration against the streets': angular, at radius n, integration and choice compared as distributions.
  const auto ca = urban::syntax::configuration(urban::syntax::segment_map(kept_input()), urban::syntax::Cost::angular, std::numeric_limits<double>::infinity(), threads());
  const auto cb = urban::syntax::configuration(urban::syntax::segment_map(smp.graph), urban::syntax::Cost::angular, std::numeric_limits<double>::infinity(), threads());
  Json conf = Json::object({{"input", Json::object({{"nach", summary(ca.nach)}, {"integration", summary(ca.integration)}})},
                            {"sample", Json::object({{"nach", summary(cb.nach)}, {"integration", summary(cb.integration)}})},
                            {"ksNach", Json(ks_distance(ca.nach, cb.nach))}, {"ksIntegration", Json(ks_distance(ca.integration, cb.integration))}});
  return Json::object({{"graph", graph_json(smp.graph)}, {"rewrites", Json(static_cast<double>(smp.rewrites))}, {"refused", Json(static_cast<double>(smp.refused))},
                       {"truncated", Json(smp.truncated)}, {"categories", cats}, {"input", stats_json(a)}, {"sample", stats_json(b)},
                       {"degreeDistance", Json(sx::stochastic::degree_distance(a, b))}, {"configuration", conf}});
}

Json complex_json(const sx::simplicial::Complex& c, double degrees) {
  Json pts = Json::array(), tris = Json::array(), letters = Json::array();
  for (const auto& p : c.points) pts.push(pt(p));
  for (const auto& t : c.triangles) {
    tris.push(Json::array({Json(static_cast<double>(t[0])), Json(static_cast<double>(t[1])), Json(static_cast<double>(t[2]))}));
    letters.push(Json(sx::simplicial::name(sx::simplicial::letter(c, t, degrees), degrees)));
  }
  return Json::object({{"points", pts}, {"triangles", tris}, {"letters", letters}});
}

Json op_words(const Json& r) {
  const double degrees = number_or(r, "degrees", 15);
  sx::simplicial::Complex c;
  if (r.has("rings")) {
    std::vector<std::vector<Point>> rings;
    for (std::size_t k = 0; k < r["rings"].size(); k++) {
      std::vector<Point> ring;
      for (std::size_t i = 0; i + 1 < r["rings"][k].size(); i += 2) ring.push_back({r["rings"][k][i].as_number(), r["rings"][k][i + 1].as_number()});
      if (ring.size() >= 3) rings.push_back(std::move(ring));
    }
    c = architecture::syntax::footprints(rings, number_or(r, "merge", 0.05));
  } else {
    c = urban::syntax::blocks(graph_of(r), number_or(r, "maxArea", 2e5));
  }
  const auto G = sx::simplicial::learn_gluing(c, degrees);
  Json letters = Json::array();
  for (const auto& [l, k] : G.letters) letters.push(Json::object({{"letter", Json(sx::simplicial::name(l, degrees))}, {"count", Json(k)}}));
  calx::random::Stream st(static_cast<std::uint64_t>(number_or(r, "seed", 1)), 0x6d0eu, 0, 0);
  // Conditioned on its size: the grammar alone is subcritical, and its patches small (syntaxis::simplicial::mean_offspring).
  const std::size_t target = static_cast<std::size_t>(number_or(r, "grow", 300));
  const auto grown = sx::simplicial::grow(G, st, target, std::min<std::size_t>(target / 2, c.triangles.size() / 2), 400);
  return Json::object({{"complex", complex_json(c, degrees)}, {"letters", letters}, {"entropy", Json(G.entropy)}, {"conditionalEntropy", Json(G.conditional_entropy)},
                       {"gluedEntropy", Json(G.glued_entropy)}, {"meanOffspring", Json(sx::simplicial::mean_offspring(G))}, {"skipped", Json(static_cast<double>(c.skipped))},
                       {"grown", complex_json(grown.complex, degrees)}, {"refused", Json(static_cast<double>(grown.refused))},
                       {"draws", Json(static_cast<double>(grown.draws))}, {"reached", Json(grown.reached)}, {"asked", Json(static_cast<double>(std::min<std::size_t>(target / 2, c.triangles.size() / 2)))}});
}

Json op_replace(const Json& r) {
  std::array<double, 4> w{1, 1, 1, 1};
  if (r.has("weights")) for (std::size_t k = 0; k < 4 && k < r["weights"].size(); k++) w[k] = r["weights"][k].as_number();
  calx::random::Stream st(static_cast<std::uint64_t>(number_or(r, "seed", 1)), 0x7e9au, 0, 0);
  const std::array<Point, 3> start{Point{0, 0}, Point{1, 0}, std::polar(1.0, 3.141592653589793 / 3)};
  const auto R = sx::simplicial::replace(start, w, static_cast<std::size_t>(number_or(r, "depth", 4)), st);
  const auto P = sx::simplicial::parse_replacement(R.leaves);
  const auto tri = [](const std::array<Point, 3>& t) { return Json::array({pt(t[0]), pt(t[1]), pt(t[2])}); };
  Json tree = Json::array(), leaves = Json::array(), parsed = Json::array(), steps = Json::array();
  for (const auto& n : R.tree) {
    Json ch = Json::array();
    for (std::size_t c : n.children) ch.push(Json(static_cast<double>(c)));
    tree.push(Json::object({{"corners", tri(n.corners)}, {"move", Json(static_cast<double>(n.move))}, {"children", ch}, {"depth", Json(static_cast<double>(n.depth))}}));
  }
  for (const auto& t : R.leaves) leaves.push(tri(t));
  for (const auto& n : P.nodes) {
    Json ch = Json::array();
    for (std::size_t c : n.children) ch.push(Json(static_cast<double>(c)));
    parsed.push(Json::object({{"corners", tri(n.corners)}, {"move", Json(static_cast<double>(n.move))}, {"children", ch}}));
  }
  for (const auto& s : P.steps) {
    Json made = Json::array();
    for (std::size_t m : s) made.push(Json(static_cast<double>(m)));
    steps.push(made);
  }
  const auto moves = [](const std::array<std::size_t, 3>& m) { return Json::array({Json(static_cast<double>(m[0])), Json(static_cast<double>(m[1])), Json(static_cast<double>(m[2]))}); };
  return Json::object({{"tree", tree}, {"leaves", leaves}, {"moves", moves(R.moves)}, {"parse", Json::object({{"nodes", parsed}, {"steps", steps}, {"moves", moves(P.moves)}, {"derived", Json(P.derived)}})}});
}

Json op_configuration(const Json& r) {
  const Graph g = graph_of(r);
  if (g.edge_count() < 2) throw std::runtime_error("too few streets here: pick a place with streets");
  if (g.edge_count() > 40000) throw std::runtime_error("too many segments (" + std::to_string(g.edge_count()) + "): read a smaller area");
  const auto cost = cost_of(text_or(r, "cost", "angular"));
  const double radius = number_or(r, "radius", 0) > 0 ? number_or(r, "radius", 0) : std::numeric_limits<double>::infinity();
  const auto t0 = std::chrono::steady_clock::now();
  const auto map = urban::syntax::segment_map(g);
  const auto c = urban::syntax::configuration(map, cost, radius, threads(), number_or(r, "turn", 30));
  const double seconds = std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
  Json segments = Json::array();
  for (std::size_t i = 0; i < map.size(); i++)
    segments.push(Json::array({Json(std::round(map.a[i].real() * 10) / 10), Json(std::round(map.a[i].imag() * 10) / 10), Json(std::round(map.b[i].real() * 10) / 10),
                               Json(std::round(map.b[i].imag() * 10) / 10)}));
  Json at = Json::array();
  if (r.has("points"))
    for (std::size_t k = 0; k < r["points"].size(); k++) {
      const Point p{r["points"][k][0].as_number(), r["points"][k][1].as_number()};
      const auto n = urban::syntax::nearest_segment(map, p);
      if (n.segment >= map.size()) { at.push(Json()); continue; }
      at.push(Json::object({{"segment", Json(static_cast<double>(n.segment))}, {"distance", Json(n.distance)}, {"nach", Json(c.nach[n.segment])},
                            {"integration", Json(c.integration[n.segment])}, {"choice", Json(c.choice[n.segment])}, {"meanDepth", Json(c.mean_depth[n.segment])},
                            {"nodeCount", Json(c.node_count[n.segment])}}));
    }
  // The City's counters within the streets read (and 30 m of a segment), against their streets' measures.
  Json counters = Json::array(), against = Json();
  if (r.has("counters")) {
    std::vector<double> lc, xn, xi;
    for (std::size_t k = 0; k < r["counters"].size(); k++) {
      const Json& q = r["counters"][k];
      const Point p{q["x"].as_number(), q["y"].as_number()};
      const auto n = urban::syntax::nearest_segment(map, p);
      if (n.segment >= map.size() || n.distance > 30) continue;
      const int id = static_cast<int>(q["id"].as_number());
      const double m = mean_daily(id);
      if (!(m > 0)) continue;
      counters.push(Json::object({{"id", Json(static_cast<double>(id))}, {"x", Json(p.real())}, {"y", Json(p.imag())}, {"meanDaily", Json(std::round(m))},
                                  {"segment", Json(static_cast<double>(n.segment))}, {"nach", Json(c.nach[n.segment])}, {"integration", Json(c.integration[n.segment])}}));
      lc.push_back(std::log(m)), xn.push_back(c.nach[n.segment]), xi.push_back(c.integration[n.segment]);
    }
    if (lc.size() >= 5)
      against = Json::object({{"n", Json(static_cast<double>(lc.size()))}, {"rhoNach", Json(spearman_rho(xn, lc))}, {"r2Nach", Json(line_r2(xn, lc))},
                              {"rhoIntegration", Json(spearman_rho(xi, lc))}, {"r2Integration", Json(line_r2(xi, lc))}});
  }
  return Json::object({{"segments", segments}, {"length", numbers(map.length, 10)}, {"nach", numbers(c.nach)}, {"integration", numbers(c.integration)},
                       {"counters", counters}, {"against", against},
                       {"choice", numbers(c.choice, 10)}, {"meanDepth", numbers(c.mean_depth)}, {"nodeCount", numbers(c.node_count, 1)}, {"at", at},
                       {"junctions", Json(static_cast<double>(g.size()))}, {"seconds", Json(seconds)}, {"threads", Json(static_cast<double>(threads()))},
                       {"summary", Json::object({{"nach", summary(c.nach)}, {"integration", summary(c.integration)}})}});
}

Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "learn") return op_learn(r);
  if (op == "parse") return op_parse(r);
  if (op == "sample") return op_sample(r);
  if (op == "words") return op_words(r);
  if (op == "replace") return op_replace(r);
  if (op == "configuration") return op_configuration(r);
  if (op == "status") return Json::object({{"engine", Json("syntax")}, {"kept", Json(static_cast<bool>(kept()))}});
  throw std::runtime_error("no operation " + op);
}

std::string respond(const std::string& line) {
  try {
    return Json::object({{"ok", Json(true)}, {"result", answer(Json::parse(line))}}).dump();
  } catch (const std::exception& e) {
    return Json::object({{"ok", Json(false)}, {"error", Json(std::string(e.what()))}}).dump();
  }
}

}  // namespace

int main(int argc, char** argv) {
  bool serve = false;
  std::string one;
  for (int i = 1; i < argc; i++) {
    const std::string a = argv[i];
    if (a == "--serve") serve = true;
    else if (a == "--data" && i + 1 < argc) data_dir() = argv[++i];
    else one = a;
  }
  if (!serve) {
    std::cout << respond(one.empty() ? "{\"op\":\"status\"}" : one) << std::endl;
    return 0;
  }
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    std::cout << respond(line) << std::endl;
  }
  return 0;
}
