// The evolutionary exploration and validation of needs, for any work's engine (ADR 0125): the needs domain composed with what it
// stands on, read from and written to JSON. A request holds the whole dataset (and, to go on with a search, its saved state); nothing
// is read from a file or kept between requests.
//
// The dataset, noetarium.needs-data/1 (data/products/needs_tables.py writes it):
//   {columns: [{name, kind: number|category, values: [...]}], weight: column name | null, attributes: [names],
//    indicators: [{name, share, column, total, higher_is_worse, domain, label, unit, levels}], domains: [words],
//    cluster, part, unit_place, where: {place: [x, y]}, suppliers: [{id, x, y, capacity, domain}], reach,
//    graph: [[from, to]], provision: {domain: column}, rules: [{text, source}], implies: [[domain, domain]],
//    points: [{name, about, kind: indicator|reference|level, indicators, readings: [{text, source, grade, by, rationale}]}],
//    order: [[weaker, stronger]], graph_source, graphs: [{name, source, grade, by, rationale, arrows}],
//    rivals: [{name, about, source, grade, by, strata: [group] | attribute, cuts}]}           (interpretation, ADR 0126)
//
//   interpret  statements read under every reading of their open terms (tekmerion's construal), their causal support under each
//              graph, and their rivals, on discovery
//   evolve also takes proposals [{text, source, parents: [tickets]}] and, given requests {budget, roles}, answers what to ask a
//   model for next (requests); the evidence of test and of the archive holds interpretation, causal_readings and rivals.
//
//   describe   what the data hold: rows, clusters and parts; each indicator's value over everyone on discovery and its spread;
//              the attributes and the pool of conditions; the seeking model's fields; the rules read and refused
//   read       a text of statements, line by line: each read, bound and checked against the data, or refused with the reason
//   test       statements put to every test (on discovery; with a search's state, the confirmation each had): the gap, stability,
//              causal support, simulation, rules, standing with its arguments
//   map        a statement's people and gap place by place: for drawing
//   generate   what a generator makes, without a search (enumeration, depth-first, random, beam, tree, rules, simulation)
//   evolve     a search run for `generations` more, from `state` when given, with `proposals` [{text, source}] added: the state to
//              keep, and the report (the population, the archive with each statement's evidence, the retired with their reasons,
//              the admissions and confirmations, the journal, each generation's statistics, the lineage, the holdout, the rule)
//   options    every strategy and objective there is, by stage, for a page to offer
#pragma once

#include "json.hpp"

#include <needs/search/search.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::needs {

namespace nl = ::needs::language;
namespace ne = ::needs::evidence;
namespace ns = ::needs::search;
namespace hev = heuresis::evolution;

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }

// The elements of an array; none for an absent or null value, so that a request may leave out what it does not need.
inline const Json::Array& items(const Json& j) {
  static const Json::Array none;
  return j.is_array() ? j.as_array() : none;
}

// ---- the dataset ----

inline nl::Dataset read_data(const Json& j) {
  if (!j.is_object()) throw std::invalid_argument("the request holds no dataset");
  nl::Dataset d;
  for (const auto& c : items(j["columns"])) {
    const std::string name = c["name"].as_string(), kind = c["kind"].text("number");
    const auto& vals = items(c["values"]);
    if (kind == "category") {
      std::vector<std::string> v;
      v.reserve(vals.size());
      for (const auto& x : vals) v.push_back(x.is_null() ? std::string() : x.is_string() ? x.as_string() : Json::number_text(x.as_number()));
      d.table.add(::needs::groups::categories(name, v));
    } else {
      std::vector<double> v;
      v.reserve(vals.size());
      for (const auto& x : vals) v.push_back(x.is_number() ? x.as_number() : std::numeric_limits<double>::quiet_NaN());
      d.table.add(::needs::groups::numbers(name, v));
    }
  }
  if (j["weight"].is_string()) d.table.set_weight(d.table.at(j["weight"].as_string()).numbers);
  for (const auto& a : items(j["attributes"])) d.attributes.push_back(a.as_string());
  for (const auto& i : items(j["indicators"])) {
    nl::Indicator ind;
    ind.name = i["name"].as_string(), ind.share = i["share"].is_bool() && i["share"].as_bool();
    ind.column = i["column"].as_string(), ind.total = i["total"].text();
    ind.higher_is_worse = !i["higher_is_worse"].is_bool() || i["higher_is_worse"].as_bool();
    ind.label = i["label"].text(), ind.unit = i["unit"].text(), ind.domain = i["domain"].text();
    for (const auto& l : items(i["levels"])) ind.levels.push_back(l.as_number());
    d.indicators[ind.name] = ind;
  }
  if (d.indicators.empty()) throw std::invalid_argument("the dataset declares no indicator");
  for (const auto& w : items(j["domains"])) d.domains.push_back(w.as_string());
  // Domains are words of the language of statements.
  auto word = [](const std::string& w) {
    if (w.empty() || !(std::isalpha(static_cast<unsigned char>(w[0])) || w[0] == '_')) return false;
    for (char c : w)
      if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '.' || c == '-')) return false;
    return true;
  };
  for (const auto& w : d.domains)
    if (!word(w)) throw std::invalid_argument("the domain '" + w + "' is not a word of the language (letters, digits, _ . -; no space)");
  for (const auto& [k, ind] : d.indicators)
    if (!ind.domain.empty() && !word(ind.domain)) throw std::invalid_argument("the indicator " + k + "'s domain '" + ind.domain + "' is not a word of the language");
  d.cluster = j["cluster"].text(), d.part = j["part"].text(), d.place = j["unit_place"].text();
  if (j["where"].is_object())
    for (const auto& [k, v] : j["where"].as_object()) d.where[k] = {v[0].as_number(), v[1].as_number()};
  for (const auto& s : items(j["suppliers"]))
    d.suppliers.push_back({s["id"].text(), s["x"].as_number(), s["y"].as_number(), s["capacity"].as_number(), s["domain"].text()});
  if (j["reach"].is_number()) d.reach = j["reach"].as_number();
  for (const auto& a : items(j["graph"])) d.graph.push_back({a[0].as_string(), a[1].as_string()});
  if (j["provision"].is_object())
    for (const auto& [k, v] : j["provision"].as_object()) d.provision[k] = v.as_string();
  for (const auto& r : items(j["rules"])) d.rules.push_back({r["text"].as_string(), r["source"].text()});
  for (const auto& a : items(j["implies"])) d.implies.push_back({a[0].as_string(), a[1].as_string()});
  // Interpretation (ADR 0126): points with their readings, an order of sources, alternative graphs, rivals.
  auto grade = [](const Json& g, const std::string& source) {
    const std::string t = g.text(source == "proposal" ? "C" : "A");
    if (t.size() != 1 || (t[0] != 'A' && t[0] != 'B' && t[0] != 'C')) throw std::invalid_argument("a grade is A, B or C, not '" + t + "'");
    return t[0];
  };
  for (const auto& p : items(j["points"])) {
    nl::Point q;
    q.name = p["name"].as_string(), q.about = p["about"].text();
    const std::string kind = p["kind"].text("reference");
    q.kind = kind == "indicator" ? nl::PointKind::indicator : kind == "level" ? nl::PointKind::level : kind == "reference" ? nl::PointKind::reference
                                                                                                                          : throw std::invalid_argument("a point is of kind indicator, reference or level, not '" + kind + "'");
    for (const auto& i : items(p["indicators"])) q.indicators.push_back(i.as_string());
    for (const auto& r : items(p["readings"])) {
      const std::string source = r["source"].text("analyst");
      q.readings.push_back({r["text"].is_number() ? Json::number_text(r["text"].as_number()) : r["text"].as_string(), source, grade(r["grade"], source), r["by"].text(), r["rationale"].text()});
    }
    d.points.push_back(std::move(q));
  }
  if (j["order"].is_array()) {
    d.order_stated = true;
    for (const auto& a : items(j["order"])) d.order.push_back({a[0].as_string(), a[1].as_string()});
  }
  if (j["graph_source"].is_string()) d.graph_source = j["graph_source"].as_string();
  for (const auto& g : items(j["graphs"])) {
    nl::Graph x;
    x.name = g["name"].as_string(), x.source = g["source"].text("analyst"), x.grade = grade(g["grade"], x.source), x.by = g["by"].text(), x.rationale = g["rationale"].text();
    for (const auto& a : items(g["arrows"])) x.arrows.push_back({a[0].as_string(), a[1].as_string()});
    d.graphs.push_back(std::move(x));
  }
  for (const auto& r : items(j["rivals"])) {
    nl::Rival x;
    x.name = r["name"].as_string(), x.about = r["about"].text(), x.source = r["source"].text("analyst"), x.grade = grade(r["grade"], x.source), x.by = r["by"].text();
    if (r["attribute"].is_string()) {
      std::vector<double> cuts;
      for (const auto& c : items(r["cuts"])) cuts.push_back(c.as_number());
      x.strata = ne::strata_of(d, r["attribute"].as_string(), cuts);
      for (const auto& g : x.strata) x.labels.push_back(g.write());
    } else {
      for (const auto& t : items(r["strata"])) {
        auto g = nl::read_group(t.as_string());
        ::needs::groups::bind(d.table, g);
        ::needs::groups::check(d.table, g);
        x.strata.push_back(g), x.labels.push_back(t.as_string());
      }
    }
    d.rivals.push_back(std::move(x));
  }
  ne::check_declarations(d);
  return d;
}

// ---- options ----

template <class E>
inline E pick(const Json& j, const char* key, E fallback) {
  return j[key].is_string() ? hev::of<E>(j[key].as_string()) : fallback;
}
inline double number_or(const Json& j, const char* key, double fallback) { return j[key].is_number() ? j[key].as_number() : fallback; }
inline std::size_t count_or(const Json& j, const char* key, std::size_t fallback) {
  return j[key].is_number() ? static_cast<std::size_t>(std::max(0.0, j[key].as_number())) : fallback;
}

inline ns::Options read_options(const Json& j) {
  ns::Options o;
  const Json& s = j["strategy"];
  auto& st = o.strategy;
  st.mu = count_or(s, "mu", st.mu), st.lambda = count_or(s, "lambda", st.lambda);
  st.scoring = pick(s, "scoring", st.scoring), st.parents = pick(s, "parents", st.parents), st.schedule = pick(s, "schedule", st.schedule);
  st.credit = pick(s, "credit", st.credit), st.survival = pick(s, "survival", st.survival);
  st.tournament = count_or(s, "tournament", st.tournament), st.ranking_pressure = number_or(s, "ranking_pressure", st.ranking_pressure);
  st.temperature = number_or(s, "temperature", st.temperature), st.truncation = number_or(s, "truncation", st.truncation);
  st.from_archive = number_or(s, "from_archive", st.from_archive), st.crossover_rate = number_or(s, "crossover_rate", st.crossover_rate);
  st.generator_rate = number_or(s, "generator_rate", st.generator_rate), st.p_min = number_or(s, "p_min", st.p_min);
  st.adaptation = number_or(s, "adaptation", st.adaptation), st.exploration = number_or(s, "exploration", st.exploration);
  st.elites = count_or(s, "elites", st.elites), st.window = count_or(s, "window", st.window), st.radius = number_or(s, "radius", st.radius);
  st.capacity = count_or(s, "capacity", st.capacity), st.layers = count_or(s, "layers", st.layers), st.age_gap = count_or(s, "age_gap", st.age_gap);
  if (s["novelty"].is_bool()) st.novelty = s["novelty"].as_bool();
  st.novelty_k = count_or(s, "novelty_k", st.novelty_k);
  st.archive_capacity = count_or(s, "archive_capacity", st.archive_capacity), st.retire_after = count_or(s, "retire_after", st.retire_after);
  st.retire_age = count_or(s, "retire_age", st.retire_age);
  st.generations = count_or(s, "generations", st.generations), st.stagnation = count_or(s, "stagnation", st.stagnation);
  if (s["restart_on_stagnation"].is_bool()) st.restart_on_stagnation = s["restart_on_stagnation"].as_bool();
  for (const auto& w : items(s["weights"])) st.weights.push_back(w.as_number());
  if (st.survival == hev::Survival::niches && st.bins.empty()) {
    // Niches over the descriptors: domain, kind of standard, log₁₀ people, conditions.
    st.bins = {4, 4, 6, 4}, st.lower = {0, 0, 1, 1}, st.upper = {4, 4, 6, 5};
  }
  if (st.survival == hev::Survival::nsga2) st.scoring = hev::Scoring::pareto;
  if (j["objectives"].is_array()) {
    o.objectives.clear();
    for (const auto& x : items(j["objectives"])) o.objectives.push_back(x.as_string());
  }
  if (j["operators"].is_object())
    for (const auto& [k, v] : j["operators"].as_object()) o.operators[k] = v.as_number();
  if (j["inference"].is_string()) o.inference = ne::inference_of(j["inference"].as_string());
  if (j["fractions"].is_array()) o.fractions = {j["fractions"][0].as_number(), j["fractions"][1].as_number(), j["fractions"][2].as_number()};
  if (j["key"].is_number()) o.key = static_cast<std::uint64_t>(j["key"].as_number());
  o.z_admit = number_or(j, "z_admit", o.z_admit), o.min_effect = number_or(j, "min_effect", o.min_effect);
  o.stability_min = number_or(j, "stability_min", o.stability_min), o.stability_threshold = number_or(j, "stability_threshold", o.stability_threshold);
  o.stability_resamples = count_or(j, "stability_resamples", o.stability_resamples);
  o.z_productive = number_or(j, "z_productive", o.z_productive), o.min_distance = number_or(j, "min_distance", o.min_distance);
  o.min_clusters = count_or(j, "min_clusters", o.min_clusters);
  const Json& h = j["holdout"];
  o.holdout.threshold = number_or(h, "threshold", o.holdout.threshold), o.holdout.sigma = number_or(h, "sigma", o.holdout.sigma);
  o.holdout.budget = count_or(h, "budget", o.holdout.budget);
  if (j["rule"].is_string()) o.rule = tyche::testing::online_rule(j["rule"].as_string());
  o.alpha = number_or(j, "alpha", o.alpha);
  o.strata = count_or(j, "strata", o.strata);
  if (j["started"].is_string()) o.started = j["started"].as_string();
  if (j["data_version"].is_string()) o.data_version = j["data_version"].as_string();
  if (j["simulate"].is_bool()) o.simulate = j["simulate"].as_bool();
  if (j["interpret"].is_bool()) o.interpret = j["interpret"].as_bool();
  if (j["refuse_untenable"].is_bool()) o.refuse_untenable = j["refuse_untenable"].as_bool();
  if (j["admit_clear_only"].is_bool()) o.admit_clear_only = j["admit_clear_only"].as_bool();
  if (j["refuse_explained"].is_bool()) o.refuse_explained = j["refuse_explained"].as_bool();
  const Json& k = j["seeking"];
  o.seeking.replications = count_or(k, "replications", o.seeking.replications), o.seeking.units = count_or(k, "units", o.seeking.units);
  o.seeking.scale = number_or(k, "scale", o.seeking.scale);
  const Json& sp = j["space"];
  o.space.quantiles = count_or(sp, "quantiles", o.space.quantiles), o.space.max_conditions = count_or(sp, "max_conditions", o.space.max_conditions);
  o.space.min_coverage = number_or(sp, "min_coverage", o.space.min_coverage), o.space.max_levels = count_or(sp, "max_levels", o.space.max_levels);
  return o;
}

// ---- statements and evaluations ----

inline nl::Statement statement_of(const nl::Dataset& d, const std::string& text) {
  auto s = nl::read_one(text);
  nl::bind(d, s);
  nl::check(d, s);
  return s;
}

inline Json evaluation_json(const heuresis::search::Evaluation& e) {
  Json j = Json::object({{"objectives", e.objectives}, {"constraints", e.constraints}, {"descriptors", e.descriptors}, {"cost", e.cost}});
  if (e.status == heuresis::search::Status::failed) j.set("failed", true), j.set("reason", e.reason);
  return j;
}
inline heuresis::search::Evaluation evaluation_of(const Json& j) {
  heuresis::search::Evaluation e;
  for (const auto& x : items(j["objectives"])) e.objectives.push_back(x.as_number());
  for (const auto& x : items(j["constraints"])) e.constraints.push_back(x.as_number());
  for (const auto& x : items(j["descriptors"])) e.descriptors.push_back(x.as_number());
  e.cost = number_or(j, "cost", 1);
  if (j["failed"].is_bool() && j["failed"].as_bool()) e.status = heuresis::search::Status::failed, e.reason = j["reason"].text();
  return e;
}

inline std::vector<double> numbers(const Json& j) {
  std::vector<double> v;
  for (const auto& x : items(j)) v.push_back(x.as_number());
  return v;
}
template <class T>
inline std::vector<T> counts(const Json& j) {
  std::vector<T> v;
  for (const auto& x : items(j)) v.push_back(static_cast<T>(x.as_number()));
  return v;
}

inline Json member_json(const hev::Member<nl::Statement>& m) {
  return Json::object({{"ticket", static_cast<double>(m.ticket)}, {"text", m.genotype.write()}, {"evaluation", evaluation_json(m.evaluation)},
                       {"score", m.score}, {"operation", m.operation}, {"parents", std::vector<double>(m.parents.begin(), m.parents.end())},
                       {"born", m.born}, {"age", m.age}, {"layer", m.layer}, {"last_useful", m.last_useful}, {"novelty", num(m.novelty)},
                       {"restarted", m.restarted}});
}
inline hev::Member<nl::Statement> member_of(const nl::Dataset& d, const Json& j) {
  hev::Member<nl::Statement> m;
  m.ticket = static_cast<heuresis::search::Ticket>(j["ticket"].as_number());
  m.genotype = nl::read_one(j["text"].as_string());
  nl::bind(d, m.genotype);
  m.evaluation = evaluation_of(j["evaluation"]);
  m.score = numbers(j["score"]);
  m.operation = j["operation"].text();
  for (const auto& p : items(j["parents"])) m.parents.push_back(static_cast<heuresis::search::Ticket>(p.as_number()));
  m.born = count_or(j, "born", 0), m.age = count_or(j, "age", 0), m.layer = count_or(j, "layer", 0), m.last_useful = count_or(j, "last_useful", 0);
  m.novelty = j["novelty"].is_number() ? j["novelty"].as_number() : 0, m.restarted = j["restarted"].is_bool() && j["restarted"].as_bool();
  return m;
}

inline Json statistics_json(const hev::Statistics& s) {
  Json p = Json::object(), q = Json::object();
  for (const auto& [k, v] : s.proposals) p.set(k, v);
  for (const auto& [k, v] : s.proposals_surviving) q.set(k, v);
  Json b = Json::array(), m = Json::array();
  for (double x : s.best) b.push(num(x));
  for (double x : s.median) m.push(num(x));
  return Json::object({{"generation", s.generation}, {"evaluations", s.evaluations}, {"best", b}, {"median", m}, {"diversity", num(s.diversity)},
                       {"births", std::vector<double>(s.births.begin(), s.births.end())}, {"survivals", std::vector<double>(s.survivals.begin(), s.survivals.end())},
                       {"probability", s.probability}, {"proposals", p}, {"proposals_surviving", q}, {"archive", s.archive}, {"admitted", s.admitted},
                       {"retired", s.retired}, {"failures", s.failures}, {"repeats", s.repeats}, {"reused", s.reused}, {"improved", s.improved}, {"restarted", s.restarted}});
}
inline hev::Statistics statistics_of(const Json& j) {
  hev::Statistics s;
  s.generation = count_or(j, "generation", 0), s.evaluations = count_or(j, "evaluations", 0);
  s.best = numbers(j["best"]), s.median = numbers(j["median"]);
  s.diversity = j["diversity"].as_number();
  s.births = counts<std::size_t>(j["births"]), s.survivals = counts<std::size_t>(j["survivals"]), s.probability = numbers(j["probability"]);
  if (j["proposals"].is_object())
    for (const auto& [k, v] : j["proposals"].as_object()) s.proposals[k] = static_cast<std::size_t>(v.as_number());
  if (j["proposals_surviving"].is_object())
    for (const auto& [k, v] : j["proposals_surviving"].as_object()) s.proposals_surviving[k] = static_cast<std::size_t>(v.as_number());
  s.archive = count_or(j, "archive", 0), s.admitted = count_or(j, "admitted", 0), s.retired = count_or(j, "retired", 0);
  s.failures = count_or(j, "failures", 0), s.repeats = count_or(j, "repeats", 0), s.reused = count_or(j, "reused", 0);
  s.improved = j["improved"].is_bool() && j["improved"].as_bool(), s.restarted = j["restarted"].is_bool() && j["restarted"].as_bool();
  return s;
}

inline Json validation_json(const ns::Validation& v) { return Json::object({{"verdict", v.verdict}, {"value", num(v.value)}}); }
inline ns::Validation validation_of(const Json& j) { return ns::Validation{j["verdict"].text(), j["value"].as_number()}; }

inline Json confirmation_json(const ns::Confirmation& c) {
  return Json::object({{"serial", c.serial}, {"ticket", static_cast<double>(c.ticket)}, {"key", c.key}, {"text", c.text}, {"generation", c.generation},
                       {"level", num(c.level)}, {"p", num(c.p)}, {"e", num(c.e)}, {"estimate", num(c.estimate)}, {"low", num(c.low)}, {"high", num(c.high)},
                       {"z", num(c.z)}, {"coverage", num(c.coverage)}, {"confirmed", c.confirmed}, {"opposite", c.opposite}, {"defined", c.defined},
                       {"why", c.why}, {"seal", c.seal}});
}
inline ns::Confirmation confirmation_of(const Json& j) {
  ns::Confirmation c;
  c.serial = count_or(j, "serial", 0), c.ticket = static_cast<heuresis::search::Ticket>(j["ticket"].as_number()), c.key = j["key"].as_string();
  c.text = j["text"].as_string(), c.generation = count_or(j, "generation", 0);
  c.level = j["level"].as_number(), c.p = j["p"].as_number(), c.e = j["e"].as_number(), c.estimate = j["estimate"].as_number();
  c.low = j["low"].as_number(), c.high = j["high"].as_number(), c.z = j["z"].as_number(), c.coverage = number_or(j, "coverage", 0);
  c.confirmed = j["confirmed"].as_bool(), c.opposite = j["opposite"].as_bool(), c.defined = j["defined"].as_bool();
  c.why = j["why"].text(), c.seal = j["seal"].text();
  return c;
}

inline Json admission_json(const ns::Admission& a) {
  return Json::object({{"ticket", static_cast<double>(a.ticket)}, {"key", a.key}, {"text", a.text}, {"note", a.note}, {"data_version", a.data_version},
                       {"generation", a.generation}, {"z", num(a.z)}, {"effect", num(a.effect)}, {"stability", num(a.stability)}, {"validation", validation_json(a.validation)},
                       {"reading", a.reading}, {"share", num(a.share)}});
}
inline ns::Admission admission_of(const Json& j) {
  ns::Admission a;
  a.ticket = static_cast<heuresis::search::Ticket>(j["ticket"].as_number()), a.key = j["key"].as_string(), a.text = j["text"].as_string();
  a.note = j["note"].text(), a.data_version = j["data_version"].text(), a.generation = count_or(j, "generation", 0);
  a.z = j["z"].as_number(), a.effect = j["effect"].as_number(), a.stability = j["stability"].as_number(), a.validation = validation_of(j["validation"]);
  a.reading = j["reading"].text(), a.share = j["share"].is_number() ? j["share"].as_number() : ne::nan;
  return a;
}

// The saved state of a search, as JSON, and back. Statements are kept as text and read again against the dataset.
inline Json saved_json(const ns::Saved& s) {
  const auto& e = s.evolution;
  Json pop = Json::array(), arc = Json::array(), ret = Json::array(), cache = Json::array(), stats = Json::array(), lin = Json::array();
  for (const auto& m : e.population) pop.push(member_json(m));
  for (const auto& k : e.archive) arc.push(Json::object({{"member", member_json(k.member)}, {"admitted", k.admitted}, {"note", k.note}}));
  for (const auto& r : e.retired)
    ret.push(Json::object({{"member", member_json(r.member)}, {"admitted", r.admitted}, {"retired", r.retired}, {"note", r.note}, {"reason", r.reason}}));
  for (const auto& [k, ev] : e.cache) cache.push(Json::array({k, evaluation_json(ev)}));
  for (const auto& st : e.statistics) stats.push(statistics_json(st));
  for (const auto& l : e.lineage)
    lin.push(Json::array({static_cast<double>(l.ticket), l.generation, l.operation, std::vector<double>(l.parents.begin(), l.parents.end()), l.objectives,
                          (l.failed ? 1 : 0) + (l.survived ? 2 : 0) + (l.admitted ? 4 : 0)}));
  Json evo = Json::object({{"generation", e.generation}, {"evaluations", e.evaluations}, {"serial", e.serial}, {"stall", e.stall},
                           {"population", pop}, {"archive", arc}, {"retired", ret}, {"quality", e.quality}, {"probability", e.probability},
                           {"successes", e.successes}, {"failures", e.failures}, {"count", std::vector<double>(e.count.begin(), e.count.end())},
                           {"total", e.total}, {"cache", cache}, {"statistics", stats}, {"lineage", lin},
                           {"best", e.best ? member_json(*e.best) : Json()}, {"stopped", e.stopped}, {"reseed", e.reseed}});
  Json adm = Json::array(), conf = Json::array(), val = Json::array(), journal = Json::array(), handed = Json::object();
  for (const auto& a : s.admissions) adm.push(admission_json(a));
  for (const auto& c : s.confirmations) conf.push(confirmation_json(c));
  for (const auto& [k, v] : s.validated) val.push(Json::array({k, validation_json(v)}));
  for (const auto& en : s.journal.entries)
    journal.push(Json::object({{"when", en.when}, {"who", en.who}, {"what", en.what}, {"about", en.about}, {"note", en.note}, {"seal", en.seal}}));
  for (const auto& [k, v] : s.handed) handed.set(k, v);
  return Json::object({{"schema", "noetarium.needs-search/1"}, {"evolution", evo}, {"admissions", adm}, {"confirmations", conf}, {"validated", val},
                       {"journal", journal},
                       {"holdout", Json::object({{"threshold", s.holdout.threshold}, {"left", s.holdout.left}, {"queries", s.holdout.queries}, {"overfits", s.holdout.overfits}})},
                       {"holdout_started", s.holdout_started}, {"online", s.online}, {"bfs", std::vector<double>(s.bfs.begin(), s.bfs.end())},
                       {"dfs", std::vector<double>(s.dfs.begin(), s.dfs.end())}, {"handed", handed}});
}

inline ns::Saved saved_of(const nl::Dataset& d, const Json& j) {
  ns::Saved s;
  const Json& e = j["evolution"];
  auto& ev = s.evolution;
  ev.generation = count_or(e, "generation", 0), ev.evaluations = count_or(e, "evaluations", 0), ev.serial = count_or(e, "serial", 0), ev.stall = count_or(e, "stall", 0);
  for (const auto& m : items(e["population"])) ev.population.push_back(member_of(d, m));
  for (const auto& k : items(e["archive"])) ev.archive.push_back({member_of(d, k["member"]), count_or(k, "admitted", 0), k["note"].text()});
  for (const auto& r : items(e["retired"]))
    ev.retired.push_back({member_of(d, r["member"]), count_or(r, "admitted", 0), count_or(r, "retired", 0), r["note"].text(), r["reason"].text()});
  ev.quality = numbers(e["quality"]), ev.probability = numbers(e["probability"]), ev.successes = numbers(e["successes"]), ev.failures = numbers(e["failures"]);
  ev.count = counts<std::size_t>(e["count"]), ev.total = count_or(e, "total", 0);
  for (const auto& c : items(e["cache"])) ev.cache.emplace_back(c[0].as_string(), evaluation_of(c[1]));
  for (const auto& st : items(e["statistics"])) ev.statistics.push_back(statistics_of(st));
  for (const auto& l : items(e["lineage"])) {
    hev::Lineage x;
    x.ticket = static_cast<heuresis::search::Ticket>(l[0].as_number()), x.generation = static_cast<std::size_t>(l[1].as_number()), x.operation = l[2].as_string();
    for (const auto& p : items(l[3])) x.parents.push_back(static_cast<heuresis::search::Ticket>(p.as_number()));
    x.objectives = numbers(l[4]);
    const int f = static_cast<int>(l[5].as_number());
    x.failed = f & 1, x.survived = f & 2, x.admitted = f & 4;
    ev.lineage.push_back(std::move(x));
  }
  if (e["best"].is_object()) ev.best = member_of(d, e["best"]);
  ev.stopped = e["stopped"].is_bool() && e["stopped"].as_bool(), ev.reseed = e["reseed"].is_bool() && e["reseed"].as_bool();
  for (const auto& a : items(j["admissions"])) s.admissions.push_back(admission_of(a));
  for (const auto& c : items(j["confirmations"])) s.confirmations.push_back(confirmation_of(c));
  for (const auto& v : items(j["validated"])) s.validated.emplace_back(v[0].as_string(), validation_of(v[1]));
  for (const auto& en : items(j["journal"]))
    s.journal.entries.push_back({en["when"].text(), en["who"].text(), en["what"].text(), en["about"].text(), en["note"].text(), en["seal"].text()});
  const Json& h = j["holdout"];
  s.holdout.threshold = h["threshold"].as_number(), s.holdout.left = count_or(h, "left", 0), s.holdout.queries = count_or(h, "queries", 0);
  s.holdout.overfits = count_or(h, "overfits", 0), s.holdout_started = j["holdout_started"].is_bool() && j["holdout_started"].as_bool();
  s.online = numbers(j["online"]), s.bfs = counts<std::size_t>(j["bfs"]), s.dfs = counts<std::size_t>(j["dfs"]);
  if (j["handed"].is_object())
    for (const auto& [k, v] : j["handed"].as_object()) s.handed[k] = static_cast<std::size_t>(v.as_number());
  // The journal must be as it was written: a state altered outside is refused, never repaired.
  const auto verdict = archeion::fixity::verify(s.journal);
  if (!verdict.holds()) throw std::invalid_argument("the search's journal does not verify: " + verdict.reason);
  return s;
}

// ---- reports ----

inline Json gap_json(const ne::Gap& g) {
  return Json::object({{"defined", g.defined}, {"why", g.why}, {"estimate", num(g.estimate)}, {"se", num(g.se)}, {"df", num(g.df)}, {"low", num(g.low)},
                       {"high", num(g.high)}, {"z", num(g.z)}, {"p", num(g.p)}, {"group_value", num(g.group_value)}, {"reference_value", num(g.reference_value)},
                       {"coverage", num(g.coverage)}, {"size", num(g.size)}, {"rows", g.rows}, {"clusters", g.clusters}});
}

// A construal (tekmerion's): its points with each reading's source, every interpretation with its verdict, the label, the classes
// over the record's readings and as proposed, the pivots and the readings set aside. `statements` (when given) are the statement
// under each interpretation.
inline Json construal_json(const tekmerion::construal::Construal& c, const std::vector<std::string>* statements = nullptr, const std::vector<ne::Gap>* gaps = nullptr) {
  namespace tc = tekmerion::construal;
  Json points = Json::array();
  for (const auto& p : c.declared) {
    Json rs = Json::array();
    for (const auto& r : p.readings)
      rs.push(Json::object({{"text", r.name}, {"source", tc::name(r.source)}, {"grade", std::string(1, r.grade)}, {"by", r.by}, {"rationale", r.rationale}, {"stated", r.stated}}));
    points.push(Json::object({{"name", p.name}, {"about", p.about}, {"readings", rs}}));
  }
  Json all = Json::array();
  for (std::size_t x = 0; x < c.holds.size(); x++) {
    const auto i = tc::ap::decode(c.points, x);
    Json o = Json::object({{"readings", std::vector<double>(i.begin(), i.end())}, {"words", tc::words(c, x)}, {"holds", c.holds[x] != 0}, {"reason", c.reasons[x]},
                           {"admissible", c.admissible[x] != 0}, {"as_proposed", c.admissible_with_proposals[x] != 0}});
    if (statements && x < statements->size()) o.set("statement", (*statements)[x]);
    if (gaps && x < gaps->size() && (*gaps)[x].defined) o.set("gap", num((*gaps)[x].estimate)), o.set("z", num((*gaps)[x].z));
    all.push(std::move(o));
  }
  Json label = Json::array();
  for (const auto& p : c.label) label.push(tc::ap::print(c.points, p));
  Json pivots = Json::array();
  for (const auto& pv : c.pivots) {
    Json flips = Json::array();
    for (std::size_t k = 0; k < pv.flips.size(); k++)
      flips.push(Json::object({{"reading", pv.flips[k]}, {"text", c.points[pv.point].readings[pv.flips[k]]}, {"admissible", pv.admissible[k] != 0}}));
    pivots.push(Json::object({{"point", pv.point}, {"name", c.points[pv.point].name}, {"flips", flips}}));
  }
  return Json::object({{"claim", c.claim}, {"points", points}, {"interpretations", all}, {"label", label},
                       {"record", std::string(tc::ap::word(c.record))}, {"as_proposed", std::string(tc::ap::word(c.with_proposals))}, {"share", num(c.share)},
                       {"stated", c.stated ? Json(static_cast<double>(*c.stated)) : Json()}, {"stated_holds", c.stated_holds ? Json(*c.stated_holds) : Json()},
                       {"pivots", pivots}, {"set_aside", c.set_aside}, {"verdict", c.verdict.reason}});
}

inline Json rival_json(const tekmerion::construal::RivalAssessment& a) {
  return Json::object({{"claim", a.claim}, {"source", tekmerion::construal::name(a.source)}, {"grade", std::string(1, a.grade)}, {"explains", tekmerion::construal::name(a.explains)}, {"reason", a.reason}, {"crude_z", num(a.crude_z)},
                       {"pooled", num(a.pooled.estimate)}, {"se", num(a.pooled.se)}, {"remaining", num(a.remaining)}, {"strata", a.pooled.k}});
}

inline Json request_json(const ns::Request& q) {
  return Json::object({{"role", q.role}, {"count", q.count}, {"parents", std::vector<double>(q.parents.begin(), q.parents.end())}, {"texts", q.texts}, {"notes", q.notes},
                       {"credit", num(q.credit)}});
}

inline Json evidence_json(const ns::Evidence& ev) {
  Json args = Json::array();
  for (const auto& a : ev.standing.arguments) args.push(Json::object({{"id", a.id}, {"text", a.text}, {"label", a.label}}));
  Json j = Json::object({
      {"discovery", gap_json(ev.discovery)},
      {"effect", num(ev.effect)},
      {"validation", ev.validation_answer.verdict.empty() ? Json() : validation_json(ev.validation_answer)},
      {"stability", Json::object({{"share", num(ev.stability.share)}, {"low", num(ev.stability.low)}, {"high", num(ev.stability.high)}, {"resamples", ev.stability.resamples}})},
      {"causal", Json::object({{"applicable", ev.causal.applicable}, {"verdict", ne::name(ev.causal.verdict)}, {"why", ev.causal.why},
                                 {"provision", ev.causal.provision}, {"adjustment", ev.causal.adjustment}, {"effect", num(ev.causal.effect)},
                                 {"low", num(ev.causal.low)}, {"high", num(ev.causal.high)}, {"p", num(ev.causal.p)}, {"rows", ev.causal.rows}})},
      {"simulation", Json::object({{"applicable", ev.simulation.applicable}, {"verdict", ne::name(ev.simulation.verdict)}, {"why", ev.simulation.why},
                                     {"access_group", num(ev.simulation.access_group)}, {"access_reference", num(ev.simulation.access_reference)},
                                     {"without_group", num(ev.simulation.without_group)}, {"without_reference", num(ev.simulation.without_reference)},
                                     {"unmet_group", num(ev.simulation.unmet_group)}, {"unmet_reference", num(ev.simulation.unmet_reference)},
                                     {"difference", num(ev.simulation.difference)}, {"low", num(ev.simulation.low)}, {"high", num(ev.simulation.high)}})},
      {"rules", Json::object({{"applicable", ev.rules.applicable}, {"verdict", ne::name(ev.rules.verdict)}, {"why", ev.rules.why},
                                {"share", num(ev.rules.share)}, {"derivations", ev.rules.derivations}})},
      {"standing", Json::object({{"standing", ne::name(ev.standing.standing)}, {"arguments", args}, {"qualifiers", ev.standing.qualifiers}})},
  });
  if (ev.confirmation) j.set("confirmation", confirmation_json(*ev.confirmation));
  if (ev.interpretation.applies) j.set("interpretation", construal_json(ev.interpretation.construal, &ev.interpretation.statements, &ev.interpretation.gaps));
  if (ev.causal_readings.applies) j.set("causal_readings", construal_json(ev.causal_readings.construal));
  Json rivals = Json::array();
  for (const auto& r : ev.rivals) rivals.push(rival_json(r));
  j.set("rivals", rivals);
  return j;
}

inline Json report(const ns::Search& s, std::size_t top) {
  const auto& e = s.evolution();
  Json pop = Json::array();
  const auto order = e.ranking();
  for (std::size_t k = 0; k < order.size() && k < top; k++) {
    const auto& m = e.population()[order[k]];
    pop.push(Json::object({{"ticket", static_cast<double>(m.ticket)}, {"text", m.genotype.write()}, {"indicator", m.genotype.indicator},
                           {"domain", m.genotype.domain}, {"objectives", evaluation_json(m.evaluation)}, {"operation", m.operation}, {"age", m.age},
                           {"born", m.born}, {"novelty", num(m.novelty)}}));
  }
  Json arc = Json::array();
  for (const auto& k : e.archive()) {
    Json x = Json::object({{"ticket", static_cast<double>(k.member.ticket)}, {"text", k.member.genotype.write()}, {"domain", k.member.genotype.domain},
                           {"indicator", k.member.genotype.indicator}, {"admitted", k.admitted}, {"note", k.note}, {"operation", k.member.operation},
                           {"last_useful", k.member.last_useful}});
    try {
      x.set("evidence", evidence_json(s.evidence_of(k.member.genotype)));
    } catch (const std::exception& ex) {
      x.set("evidence_error", ex.what());
    }
    arc.push(std::move(x));
  }
  Json ret = Json::array();
  for (const auto& r : e.retired()) {
    Json x = Json::object({{"ticket", static_cast<double>(r.member.ticket)}, {"text", r.member.genotype.write()}, {"admitted", r.admitted},
                           {"retired", r.retired}, {"note", r.note}, {"reason", r.reason}});
    for (const auto& c : s.confirmations())
      if (c.key == r.member.genotype.key()) x.set("confirmed", c.confirmed);
    ret.push(std::move(x));
  }
  Json stats = Json::array();
  for (const auto& st : e.statistics()) stats.push(statistics_json(st));
  // Every child ever made: its ticket, generation, operator, parents, what became of it (failed 1, survived 2, admitted 4), and its
  // objectives as told (null for a failure), so that a page can draw ancestries and generations.
  Json lineage = Json::array();
  for (const auto& l : e.lineage()) {
    Json obj = Json::array();
    for (double x : l.objectives) obj.push(num(x));
    lineage.push(Json::array({static_cast<double>(l.ticket), l.generation, l.operation, std::vector<double>(l.parents.begin(), l.parents.end()),
                              (l.failed ? 1 : 0) + (l.survived ? 2 : 0) + (l.admitted ? 4 : 0), obj}));
  }
  Json adm = Json::array(), conf = Json::array(), journal = Json::array();
  for (const auto& a : s.admissions()) adm.push(admission_json(a));
  for (const auto& c : s.confirmations()) conf.push(confirmation_json(c));
  for (const auto& en : s.journal().entries)
    journal.push(Json::object({{"when", en.when}, {"what", en.what}, {"about", en.about}, {"note", en.note}, {"seal", en.seal}}));
  const auto credits = e.credits();
  return Json::object({
      {"generation", e.generation()}, {"evaluations", e.evaluations()}, {"stopped", e.stopped()},
      {"objectives", s.options().objectives}, {"operators", e.operator_names()}, {"probability", credits.probability}, {"quality", credits.quality},
      {"population", pop}, {"archive", arc}, {"retired", ret}, {"admissions", adm}, {"confirmations", conf},
      {"journal", journal}, {"journal_head", s.journal().head()}, {"statistics", stats}, {"lineage", lineage},
      {"holdout", Json::object({{"queries", s.holdout().queries()}, {"overfits", s.holdout().overfits()}, {"left", s.holdout().left()},
                                {"budget", s.options().holdout.budget}})},
      {"online", Json::object({{"rule", tyche::testing::name(s.options().rule)}, {"alpha", s.options().alpha}, {"tests", s.online().tests()},
                               {"rejections", s.online().rejections().size()}, {"next_level", num(s.online().next_level())}})},
  });
}

// ---- operations ----

inline Json describe(const Json& r) {
  const auto d = read_data(r["data"]);
  ns::Search s(d, read_options(r["options"]));
  const auto& p = s.parts();
  Json parts = Json::array();
  for (int k = 0; k < 3; k++)
    parts.push(Json::object({{"part", ne::name(static_cast<ne::Part>(k))}, {"clusters", p.members[static_cast<std::size_t>(k)].size()}, {"rows", p.rows[static_cast<std::size_t>(k)]}}));
  Json inds = Json::array();
  for (const auto& name : s.space().indicators) {
    const auto& ind = d.indicator(name);
    inds.push(Json::object({{"name", name}, {"label", ind.label}, {"unit", ind.unit}, {"domain", s.space().domain(name)}, {"share", ind.share},
                            {"higher_is_worse", ind.higher_is_worse}, {"value", num(ne::frame_value(d, name, {}, p, ne::Part::discovery))},
                            {"spread", num(s.space().spread.at(name))}, {"levels", ind.levels}}));
  }
  Json conds = Json::array();
  for (const auto& c : s.space().conditions) conds.push(c.write());
  Json fields = Json::array();
  for (const auto& [ind, f] : s.fields())
    fields.push(Json::object({{"indicator", ind}, {"domain", f.domain}, {"places", f.places.size()}, {"suppliers", f.suppliers}, {"capacity", f.capacity},
                              {"demand", f.total_demand}}));
  Json rules = Json::array();
  for (std::size_t k = 0; k < s.rulebook().rules.size(); k++) rules.push(Json::object({{"text", s.rulebook().rules[k].write()}, {"source", s.rulebook().sources[k]}}));
  // Each place's part (its rows' clusters' part; a place of several clusters takes the first met), for a page to draw the protocol.
  Json places = Json::object();
  if (!d.place.empty()) {
    const auto& pc = d.table.at(d.place);
    std::map<std::string, int> seen;
    for (std::size_t i = 0; i < d.table.rows; i++)
      if (pc.kind == ::needs::groups::Kind::category && pc.codes[i] >= 0) seen.emplace(pc.levels[static_cast<std::size_t>(pc.codes[i])], p.part_of_row[i]);
    for (const auto& [k, v] : seen) places.set(k, ne::name(static_cast<ne::Part>(v)));
  }
  // What the data declare for interpretation (ADR 0126).
  Json points = Json::array();
  for (const auto& q : d.points) {
    Json rs = Json::array();
    for (const auto& x : q.readings) rs.push(Json::object({{"text", x.text}, {"source", x.source}, {"grade", std::string(1, x.grade)}, {"by", x.by}, {"rationale", x.rationale}}));
    points.push(Json::object({{"name", q.name}, {"about", q.about}, {"kind", nl::name(q.kind)}, {"indicators", q.indicators}, {"readings", rs}}));
  }
  Json graphs = Json::array();
  if (!d.graph.empty()) graphs.push(Json::object({{"name", "declared"}, {"source", d.graph_source}, {"grade", "A"}, {"arrows", d.graph.size()}}));
  for (const auto& g : d.graphs) graphs.push(Json::object({{"name", g.name}, {"source", g.source}, {"grade", std::string(1, g.grade)}, {"by", g.by}, {"arrows", g.arrows.size()}}));
  Json rivals = Json::array();
  for (const auto& r : d.rivals) rivals.push(Json::object({{"name", r.name}, {"about", r.about}, {"source", r.source}, {"grade", std::string(1, r.grade)}, {"strata", r.labels}}));
  Json provision = Json::object(), graph = Json::array();
  for (const auto& [k, v] : d.provision) provision.set(k, v);
  for (const auto& a : d.graph) graph.push(Json::array({a.from, a.to}));
  const auto column_names = [](const nl::Dataset& x) {
    std::vector<std::string> v;
    for (const auto& c : x.table.columns) v.push_back(c.name);
    return v;
  };
  return Json::object({{"rows", d.table.rows}, {"clusters", p.clusters.size()}, {"parts", parts}, {"places", places}, {"indicators", inds}, {"domains", s.space().domains},
                       {"attributes", s.space().attributes}, {"conditions", conds}, {"min_coverage", s.space().min_coverage},
                       {"max_conditions", s.space().max_conditions}, {"fields", fields}, {"rules", rules}, {"refused_rules", s.rulebook().refused},
                       {"points", points}, {"graphs", graphs}, {"rivals", rivals}, {"roles", ns::model_roles()}, {"columns", column_names(d)},
                       {"provision", provision}, {"graph", graph}});
}

// Statements read under the readings of their open terms, their causal support under each graph, and their rivals, on discovery.
inline Json interpret_op(const Json& r) {
  const auto d = read_data(r["data"]);
  const auto o = read_options(r["options"]);
  ns::Search s(d, o);
  const auto io = s.interpret_options();
  Json out = Json::array();
  for (const auto& t : items(r["statements"])) {
    Json x = Json::object({{"text", t.as_string()}});
    try {
      const auto st = statement_of(d, t.as_string());
      x.set("statement", st.write());
      const auto c = ne::interpret(d, st, s.parts(), ne::Part::discovery, io);
      x.set("interpretation", c.applies ? construal_json(c.construal, &c.statements, &c.gaps) : Json());
      const auto g = ne::interpret_causal(d, st, s.parts(), ne::Part::discovery);
      x.set("causal_readings", g.applies ? construal_json(g.construal) : Json());
      Json rivals = Json::array();
      for (const auto& rv : d.rivals) rivals.push(rival_json(ne::rival(d, st, rv, s.parts(), ne::Part::discovery, io)));
      x.set("rivals", rivals);
    } catch (const std::exception& e) {
      x.set("error", e.what());
    }
    out.push(std::move(x));
  }
  return out;
}

inline Json read_op(const Json& r) {
  const auto d = read_data(r["data"]);
  Json out = Json::array();
  for (const auto& a : nl::read_each(r["text"].text(), &d))
    out.push(Json::object({{"line", a.line}, {"text", a.text}, {"read", a.read}, {"statement", a.read ? Json(a.statement.write()) : Json()}, {"why", a.why}}));
  return out;
}

inline Json test_op(const Json& r) {
  const auto d = read_data(r["data"]);
  const auto o = read_options(r["options"]);
  std::unique_ptr<ns::Search> s = r["state"].is_object() ? std::make_unique<ns::Search>(d, o, saved_of(d, r["state"])) : std::make_unique<ns::Search>(d, o);
  Json out = Json::array();
  for (const auto& t : items(r["statements"])) {
    Json x = Json::object({{"text", t.as_string()}});
    try {
      const auto st = statement_of(d, t.as_string());
      x.set("statement", st.write());
      x.set("evidence", evidence_json(s->evidence_of(st)));
    } catch (const std::exception& e) {
      x.set("error", e.what());
    }
    out.push(std::move(x));
  }
  return out;
}

inline Json map_op(const Json& r) {
  const auto d = read_data(r["data"]);
  const auto o = read_options(r["options"]);
  const auto parts = ne::split(d, o.fractions, o.key);
  const auto st = statement_of(d, r["statement"].as_string());
  if (d.place.empty()) throw std::invalid_argument("the data name no places for the units");
  const auto& t = d.table;
  const auto& pc = t.at(d.place);
  const auto frame = ::needs::groups::rows_of(t, st.frame);
  const auto g = ::needs::groups::rows_of(t, st.group, frame);
  const auto cols = ne::detail::columns(d, st);
  std::map<std::string, std::array<double, 5>> by;  // place → [group y, group x, rest y, rest x, part]
  for (std::size_t i = 0; i < t.rows; i++) {
    if (pc.codes[i] < 0 || !frame[i]) continue;
    double y, x, v;
    if (!ne::detail::row(cols, i, y, x, v)) continue;
    auto& a = by[pc.levels[static_cast<std::size_t>(pc.codes[i])]];
    const double w = t.w(i);
    if (g[i]) a[0] += w * y, a[1] += w * x;
    else a[2] += w * y, a[3] += w * x;
    a[4] = parts.part_of_row[i];
  }
  Json places = Json::array();
  for (const auto& [p, a] : by)
    places.push(Json::object({{"place", p}, {"group_value", a[1] > 0 ? Json(a[0] / a[1]) : Json()}, {"group_people", a[1]},
                              {"rest_value", a[3] > 0 ? Json(a[2] / a[3]) : Json()}, {"rest_people", a[3]}, {"part", ne::name(static_cast<ne::Part>(static_cast<int>(a[4])))}}));
  return Json::object({{"statement", st.write()}, {"places", places}});
}

inline Json generate_op(const Json& r) {
  const auto d = read_data(r["data"]);
  ns::Search s(d, read_options(r["options"]));
  ns::Generators g;
  g.d = &s.data(), g.parts = &s.parts(), g.sp = &s.space(), g.rules = &s.rulebook(), g.fields = s.fields();
  calx::random::Stream rs(s.options().key, 0x9e7, 0, 0);
  Json out = Json::array();
  for (const auto& st : g.make(r["generator"].text("beam"), count_or(r, "n", 20), rs)) out.push(st.write());
  return out;
}

inline Json evolve(const Json& r) {
  const auto d = read_data(r["data"]);
  const auto o = read_options(r["options"]);
  std::unique_ptr<ns::Search> s = r["state"].is_object() ? std::make_unique<ns::Search>(d, o, saved_of(d, r["state"])) : std::make_unique<ns::Search>(d, o);
  Json refused = Json::array();
  // Proposals by source, each with the tickets of the members it was made from (a model's variation), or none.
  std::map<std::string, std::pair<std::vector<nl::Statement>, std::vector<std::vector<heuresis::search::Ticket>>>> by_source;
  for (const auto& p : items(r["proposals"])) {
    const std::string source = p["source"].text("a proposal");
    std::vector<heuresis::search::Ticket> parents;
    for (const auto& t : items(p["parents"])) parents.push_back(static_cast<heuresis::search::Ticket>(t.as_number()));
    for (const auto& a : nl::read_each(p["text"].text(), &d)) {
      if (a.read) by_source[source].first.push_back(a.statement), by_source[source].second.push_back(parents);
      else refused.push(Json::object({{"source", source}, {"line", a.text}, {"why", a.why}}));
    }
  }
  for (const auto& [source, v] : by_source) {
    bool any = false;
    for (const auto& ps : v.second) any = any || !ps.empty();
    s->propose(v.first, source, any ? v.second : std::vector<std::vector<heuresis::search::Ticket>>{});
  }
  s->run(count_or(r, "generations", 5));
  Json out = Json::object({{"state", saved_json(s->saved())}, {"report", report(*s, count_or(r, "top", 30))}, {"refused", refused}});
  // What to ask a model for next, when asked: {budget, roles}.
  if (r["requests"].is_object()) {
    std::vector<std::string> roles;
    for (const auto& x : items(r["requests"]["roles"])) roles.push_back(x.as_string());
    Json q = Json::array();
    for (const auto& x : s->requests(count_or(r["requests"], "budget", 0), roles.empty() ? ns::model_roles() : roles)) q.push(request_json(x));
    out.set("requests", q);
  }
  return out;
}

inline Json options_op(const Json&) {
  return Json::object({
      {"scoring", hev::names<hev::Scoring>()}, {"parents", hev::names<hev::Parents>()}, {"schedule", hev::names<hev::Schedule>()},
      {"credit", hev::names<hev::Credit>()}, {"survival", hev::names<hev::Survival>()},
      {"objectives", std::vector<std::string>{"gap", "effect", "coverage", "size", "quality", "simplicity", "stability", "causal", "strata"}},
      {"generators", ns::generator_names()}, {"mutations", ns::mutation_names()}, {"crossovers", ns::crossover_names()},
      {"rules", std::vector<std::string>{"lond", "lord++", "saffron", "alpha-investing", "e-lond"}}, {"inference", std::vector<std::string>{"design", "binomial"}},
  });
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "describe") return describe(r);
  if (op == "read") return read_op(r);
  if (op == "test") return test_op(r);
  if (op == "map") return map_op(r);
  if (op == "generate") return generate_op(r);
  if (op == "evolve") return evolve(r);
  if (op == "interpret") return interpret_op(r);
  if (op == "options") return options_op(r);
  throw std::invalid_argument("science::needs: no operation is called " + op + " (there is: describe, read, test, map, generate, evolve, interpret, options)");
}

}  // namespace science::needs
