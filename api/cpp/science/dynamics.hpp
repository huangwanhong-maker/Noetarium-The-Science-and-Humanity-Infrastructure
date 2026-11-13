// Dynamical systems for any work's engine (ADR 0103): one composition of kinesis, answering requests in JSON.
// A request holds a whole script of the language of dynamical systems (kinesis/language/grammar.hpp); nothing is
// read from a file, and nothing is kept between requests.
//
//   grammar     {}                                 the grammar of the language, in BNF: {"text": …}
//   catalogue   {}                                 kinesis's catalogue of models: for each its name, title, field,
//                                                  what it is, its source, its form and its declaration as text
//   read        {script}                           what the script declares: for each system its form, its parts and
//                                                  its terms in LaTeX; or the place where the script goes wrong
//   derive      {script, system}                   the declaration a derived or composed system stands for, as text
//   simulate    {script, system, …}                a run: a flow, a map, an action's paths, a stochastic path, a
//                                                  quantum state's observations, a composite merged or co-simulated
//   lyapunov    {script, system, …}                the exponents, their sum, Kaplan and Yorke's dimension
//   averages    {script, system, …}                the means of a system's observations along a run, each with its
//                                                  error and its second moment; with "of" (observations) and
//                                                  "against" (as many numbers), the power law through their means
//   equilibria  {script, system, box}              the equilibria Newton's method reaches, classified
//   diagram     {script, system, parameter, …}     a bifurcation diagram
//   basins      {script, system, …}                the attractor each state of a grid leads to
//   landscape   {script, system, …}                a potential, a stationary landscape, or one from a long path
//   transfer    {script, system, …}                Ulam's matrix of one component: its density and eigenvalues
//   rules       {script, system, …}                a rewriting system's words within a bound, with their histories
//   quantum     {script, system, …}                a generator's stationary state, spectrum and gap
//   braid       {samples | script + system, …}     the braid of trajectories, what the record certifies, its entropy
//   word        {strands, word}                    a braid word: its reduction, permutation, entropy, polynomial
//   warnings    {series | script + system, …}      signs of slowing down in a series, with their significance
//   fit         {script, template, record}         a template's unknowns from a record: the model, as text
//   discover    {record, degree, …}                a sparse law of a record with no law proposed
//   match       {script, candidates, record}       candidate models ranked on a part of the record kept aside; with
//                                                  "catalogue": true the catalogue's plain models of the record's
//                                                  shape are candidates too, their states named after the record's
//                                                  in each order, and the best order of each is kept
//   operator    {record, …}                        a linear operator fitted to a record: its rates
//   forecast    {record, …}                        a forecaster with no law, and how far it holds
//
// A record is {"names": […], "t": […], "x": [[…], …]} (a row for each time), or {"simulate": {…}} with the fields
// of a simulate request, which makes one from a system of the script: a record whose law is known.
//
// States are given by name: "initial": {"x": 1, "y'": 0}; what is not named starts at zero. "set": {"rho": 20}
// sets parameters for the request.

#pragma once

#include "json.hpp"

#include <kinesis/analysis/criticality.hpp>
#include <kinesis/analysis/invariants.hpp>
#include <kinesis/analysis/landscape.hpp>
#include <kinesis/analysis/operators.hpp>
#include <kinesis/analysis/statistics.hpp>
#include <kinesis/braids/entropy.hpp>
#include <kinesis/braids/trajectories.hpp>
#include <kinesis/catalogue/models.hpp>
#include <kinesis/coupling/composite.hpp>
#include <kinesis/induction/fit.hpp>
#include <kinesis/induction/match.hpp>
#include <kinesis/induction/operators.hpp>
#include <kinesis/induction/surrogate.hpp>
#include <kinesis/language/reader.hpp>
#include <kinesis/systems/lagrangian.hpp>
#include <kinesis/systems/quantum.hpp>
#include <kinesis/systems/rewriting.hpp>
#include <kinesis/systems/simulate.hpp>
#include <kinesis/systems/stochastic.hpp>

#include <algorithm>
#include <cmath>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::dynamics {

namespace k = kinesis;
using k::language::Script;
using k::language::System;
using k::systems::Form;
using k::systems::State;

namespace detail {

inline double number(const Json& j, const char* key, double fallback) { return j[key].is_number() ? j[key].as_number() : fallback; }
inline std::size_t count(const Json& j, const char* key, std::size_t fallback) { return j[key].is_number() ? static_cast<std::size_t>(j[key].as_number()) : fallback; }
inline std::vector<double> numbers(const Json& j) {
  std::vector<double> out;
  for (std::size_t i = 0; i < j.size(); i++) out.push_back(j[i].as_number());
  return out;
}
inline Json columns(const std::vector<State>& rows, std::size_t width) {
  Json out = Json::array();
  for (std::size_t i = 0; i < width; i++) {
    std::vector<double> column(rows.size());
    for (std::size_t r = 0; r < rows.size(); r++) column[r] = rows[r][i];
    out.push(Json(column));
  }
  return out;
}
inline Json complex_list(const std::vector<std::complex<double>>& v) {
  Json out = Json::array();
  for (const auto& z : v) out.push(Json::array({Json(z.real()), Json(z.imag())}));
  return out;
}

inline const System& system_of(const Script& script, const Json& request, const char* key = "system") {
  const std::string name = request[key].text();
  const System* s = script.find(name);
  if (!s) throw std::invalid_argument(name.empty() ? "the request names no system" : "the script declares no system named '" + name + "'");
  return *s;
}

// The declaration a system runs as: its own, the stationary paths of its action, the flow its parts merge into,
// or what its derivation gives.
inline System runnable(const Script& script, const System& s, int depth = 0) {
  if (depth > 8) throw std::invalid_argument("derivations of '" + s.name + "' go round in a circle");
  if (s.derived()) {
    const System* source = script.find(s.source);
    if (!source) throw std::invalid_argument("'" + s.name + "' is derived from '" + s.source + "', which the script does not declare");
    const System from = s.derivation == "vary" ? *source : runnable(script, *source, depth + 1);
    if (s.derivation == "vary") return k::systems::varied(from, s.name);
    throw k::kernel::Refusal("the derivation '" + s.derivation + "' is declared and not carried out yet: `vary` is, and composition by parts");
  }
  if (k::systems::form_of(s) == Form::lagrangian) return k::systems::varied(s, s.name);
  if (k::systems::form_of(s) == Form::composite) return k::coupling::merged(script, s, s.name);
  return s;
}

inline void set_params(k::systems::Dynamics& d, const Json& request) {
  if (!request["set"].is_object()) return;
  for (const auto& [name, value] : request["set"].as_object()) d.set_param(name, value.as_number());
}
inline State initial_state(const std::vector<std::string>& names, const Json& request, double fallback = 0) {
  State x(names.size(), fallback);
  const Json& given = request["initial"];
  if (given.is_array()) {
    for (std::size_t i = 0; i < std::min(names.size(), given.size()); i++) x[i] = given[i].as_number();
  } else if (given.is_object()) {
    for (const auto& [name, value] : given.as_object()) {
      const auto it = std::find(names.begin(), names.end(), name);
      if (it == names.end()) throw std::invalid_argument("'" + name + "' is not a component: the components are " + [&] {
        std::string all;
        for (const auto& n : names) all += (all.empty() ? "" : ", ") + n;
        return all;
      }());
      x[static_cast<std::size_t>(it - names.begin())] = value.as_number();
    }
  }
  return x;
}
inline k::systems::Options options(const Json& request, bool discrete = false) {
  k::systems::Options o;
  o.t0 = number(request, "t0", 0), o.t1 = number(request, "t1", 50), o.step = number(request, "step", 0.01);
  o.every = count(request, "every", 1), o.transient = number(request, "transient", 0);
  const double samples = (o.t1 - o.t0) / ((discrete ? 1.0 : o.step) * static_cast<double>(o.every));  // a map counts iterations
  if (samples > 400000) throw std::invalid_argument("the run would record more than 400000 samples: record fewer with \"every\", or run for a shorter time");
  return o;
}
inline Json trajectory(const k::systems::Trajectory& tr) {
  Json events = Json::array();
  for (const auto& e : tr.events) events.push(Json::object({{"t", e.t}, {"event", static_cast<double>(e.event)}}));
  return Json::object({{"names", Json(tr.names)}, {"t", Json(tr.t)}, {"x", columns(tr.x, tr.names.size())}, {"events", events}});
}

// A run of a system, whatever its form.
inline k::systems::Trajectory run(const Script& script, const Json& request, std::string* kind = nullptr) {
  const System& declared = system_of(script, request);
  const Form form = k::systems::form_of(declared);
  if (form == Form::composite && request["cosimulate"].is_bool() && request["cosimulate"].as_bool()) {
    const k::coupling::Cosimulation c = k::coupling::cosimulation(script, declared);
    k::coupling::Cosimulation::Options o;
    o.t0 = number(request, "t0", 0), o.t1 = number(request, "t1", 50), o.step = number(request, "step", 0.001);
    o.communication = number(request, "communication", 0.01), o.every = count(request, "every", 1), o.seed = static_cast<std::uint64_t>(number(request, "seed", 1));
    if (kind) *kind = "co-simulated";
    return c.run(initial_state(c.names, request), o);
  }
  bool mixed = false;
  if (form == Form::composite) {
    for (const auto& part : declared.parts) {
      const System* p = script.find(part.system);
      mixed = mixed || (p && k::systems::form_of(*p) != Form::flow && k::systems::form_of(*p) != Form::lagrangian);
    }
  }
  if (mixed) {
    const k::coupling::Cosimulation c = k::coupling::cosimulation(script, declared);
    k::coupling::Cosimulation::Options o;
    o.t0 = number(request, "t0", 0), o.t1 = number(request, "t1", 50), o.step = number(request, "step", 0.001);
    o.communication = number(request, "communication", 0.01), o.every = count(request, "every", 1), o.seed = static_cast<std::uint64_t>(number(request, "seed", 1));
    if (kind) *kind = "co-simulated: its parts are of different forms";
    return c.run(initial_state(c.names, request), o);
  }
  if (form == Form::stochastic) {
    k::systems::Stochastic d = k::systems::stochastic(declared);
    if (request["set"].is_object())
      for (const auto& [name, value] : request["set"].as_object()) d.set_param(name, value.as_number());
    k::systems::StochasticOptions o;
    static_cast<k::systems::Options&>(o) = options(request, d.discrete);
    o.seed = static_cast<std::uint64_t>(number(request, "seed", 1)), o.path = static_cast<std::uint32_t>(number(request, "path", 0));
    o.milstein = request["milstein"].is_bool() && request["milstein"].as_bool();
    if (kind) *kind = d.discrete ? "a map with draws" : o.milstein ? "Milstein's scheme" : "Euler-Maruyama";
    const auto path = k::systems::simulate(d, initial_state(d.states, request), o);
    k::systems::Trajectory tr;
    tr.names = path.names, tr.t = path.t, tr.x = path.x, tr.steps = path.steps;
    return tr;
  }
  const System flow = runnable(script, declared);
  k::systems::Dynamics d = k::systems::dynamics(flow);
  set_params(d, request);
  if (kind) *kind = d.discrete ? "iterated" : form == Form::composite ? (k::coupling::joined_by_action(declared) ? "joined by an action into one flow, Runge-Kutta of the fourth order" : "merged into one flow, Runge-Kutta of the fourth order") : "Runge-Kutta of the fourth order";
  return k::systems::simulate(d, initial_state(d.states, request), options(request, d.discrete));
}

// A record given in a request, or made from a system of the script.
inline k::systems::Trajectory record(const Script& script, const Json& given) {
  if (given["simulate"].is_object()) return run(script, given["simulate"]);
  k::systems::Trajectory r;
  for (std::size_t i = 0; i < given["names"].size(); i++) r.names.push_back(given["names"][i].text());
  r.t = numbers(given["t"]);
  for (std::size_t i = 0; i < given["x"].size(); i++) r.x.push_back(numbers(given["x"][i]));
  if (r.names.empty() || r.x.empty()) throw std::invalid_argument("a record holds names, times t and rows x, or says what to simulate");
  if (r.t.size() != r.x.size()) throw std::invalid_argument("a record has a row of states for each of its times");
  for (const auto& row : r.x)
    if (row.size() != r.names.size()) throw std::invalid_argument("each row of a record has a value for each of its names");
  return r;
}

inline Json equations(const std::vector<std::string>& lhs, const std::vector<k::terms::TermPtr>& rhs) {
  Json out = Json::array();
  for (std::size_t i = 0; i < lhs.size(); i++) out.push(Json::object({{"left", lhs[i]}, {"text", k::terms::text(rhs[i])}, {"latex", k::terms::latex(rhs[i])}}));
  return out;
}

// What one system of a script is, for a reader.
inline Json described(const Script& script, const System& s) {
  Json out = Json::object({{"name", s.name}, {"line", static_cast<double>(s.at.line)}});
  const Form form = k::systems::form_of(s);
  out.set("form", std::string(k::systems::name(form)));
  try {
    // A declaration with unknowns is a template: it is shown as written, and runs once a record has filled it.
    std::set<std::string> unknowns;
    for (const auto& e : s.change.equations) k::terms::holes(e.value, unknowns);
    for (const auto& a : s.change.actions) k::terms::holes(a.value, unknowns);
    if (!unknowns.empty()) {
      out.set("template", true), out.set("unknowns", Json(std::vector<std::string>(unknowns.begin(), unknowns.end())));
      out.set("states", Json(k::systems::scope(s).states));
      Json eq = Json::array();
      for (const auto& a : s.change.actions) eq.push(Json::object({{"left", a.name}, {"text", k::terms::text(a.value)}, {"latex", k::terms::latex(a.value)}}));
      for (const auto& e : s.change.equations) eq.push(Json::object({{"left", e.name + std::string(e.primes, '\'')}, {"text", k::terms::text(e.value)}, {"latex", k::terms::latex(e.value)}}));
      out.set("equations", eq);
      return out;
    }
    if (s.derived()) {
      out.set("derivation", s.derivation), out.set("source", s.source);
      out.set("text", k::language::write(runnable(script, s)));
      const System flow = runnable(script, s);
      const k::systems::Dynamics d = k::systems::dynamics(flow);
      out.set("states", Json(d.states));
      std::vector<std::string> left;
      for (const auto& st : d.states) left.push_back(st + "'");
      out.set("equations", equations(left, d.field_terms));
      return out;
    }
    if (form == Form::flow || form == Form::map || form == Form::lagrangian || form == Form::composite) {
      if (form == Form::lagrangian) {
        const k::systems::Lagrangian L = k::systems::vary(s);
        out.set("lagrangian", k::terms::latex(L.lagrangian)), out.set("energy", k::terms::latex(L.energy));
        std::vector<std::string> names;
        for (const auto& q : L.coordinates) names.push_back("p_" + q);
        out.set("momenta", equations(names, L.momenta));
      }
      if (form == Form::composite) {
        Json parts = Json::array();
        bool flows = true;
        for (const auto& p : s.parts) {
          const System* decl = script.find(p.system);
          const std::string f = decl ? k::systems::name(k::systems::form_of(*decl)) : "not declared";
          flows = flows && (f == "flow" || f == "lagrangian");
          parts.push(Json::object({{"name", p.name}, {"system", p.system}, {"form", f}}));
        }
        out.set("parts", parts), out.set("merges", flows);
        if (k::coupling::joined_by_action(s)) {
          // Joined by an action: the whole is one Lagrangian system, and is shown as one.
          const k::systems::Lagrangian L = k::systems::vary(k::coupling::joined_action(script, s, s.name));
          out.set("joined", true), out.set("lagrangian", k::terms::latex(L.lagrangian)), out.set("energy", k::terms::latex(L.energy));
        }
        if (!flows) {
          const k::coupling::Cosimulation c = k::coupling::cosimulation(script, s);
          out.set("states", Json(c.names));
          return out;
        }
      }
      const System flow = runnable(script, s);
      const k::systems::Dynamics d = k::systems::dynamics(flow);
      out.set("discrete", d.discrete), out.set("states", Json(d.states)), out.set("inputs", Json(d.inputs));
      Json params = Json::array();
      for (std::size_t i = 0; i < d.params.size(); i++) params.push(Json::object({{"name", d.params[i]}, {"value", d.param_values[i]}}));
      out.set("params", params);
      std::vector<std::string> left;
      for (const auto& st : d.states) left.push_back(st + "'");
      out.set("equations", equations(left, d.field_terms));
      Json events = Json::array();
      for (const auto& e : d.events) events.push(e.text);
      out.set("events", events), out.set("observables", Json(d.observables));
      std::vector<std::string> conserved;
      for (const auto& c : d.conserved_terms) conserved.push_back(k::terms::latex(c));
      out.set("conserved", Json(conserved));
      if (form != Form::flow && form != Form::map) out.set("text", k::language::write(flow));
    } else if (form == Form::stochastic) {
      const k::systems::Stochastic d = k::systems::stochastic(s);
      out.set("discrete", d.discrete), out.set("states", Json(d.states)), out.set("calculus", d.calculus), out.set("diagonal", d.diagonal);
      Json params = Json::array();
      for (std::size_t i = 0; i < d.params.size(); i++) params.push(Json::object({{"name", d.params[i]}, {"value", d.param_values[i]}}));
      out.set("params", params);
      Json sources = Json::array();
      for (const auto& src : d.sources) sources.push(Json::object({{"name", src.name}, {"law", src.kind}}));
      out.set("sources", sources);
      std::vector<std::string> left;
      for (const auto& st : d.states) left.push_back(d.discrete ? st + "'" : "d" + st);
      Json eq = Json::array();
      for (std::size_t i = 0; i < d.states.size(); i++) {
        std::string latex = d.discrete ? k::terms::latex(d.drift_terms[i]) : "\\left(" + k::terms::latex(d.drift_terms[i]) + "\\right)\\,dt";
        for (std::size_t j = 0; j < d.sources.size() && !d.discrete; j++)
          if (!k::terms::is_number(d.diffusion_terms[i][j], 0)) latex += " + \\left(" + k::terms::latex(d.diffusion_terms[i][j]) + "\\right)\\,d" + d.sources[j].name;
        eq.push(Json::object({{"left", left[i]}, {"latex", latex}}));
      }
      out.set("equations", eq);
    } else if (form == Form::rules) {
      const k::systems::Rewriting r = k::systems::rewriting(s);
      Json rules = Json::array();
      for (const auto& rule : r.rules) {
        std::string l, rt;
        for (const auto& x : rule.left) l += (l.empty() ? "" : " ") + x;
        for (const auto& x : rule.right) rt += (rt.empty() ? "" : " ") + x;
        rules.push(Json::object({{"name", rule.name}, {"left", l}, {"right", rt}, {"weight", rule.weight}, {"probability", rule.probability}}));
      }
      out.set("alphabet", Json(r.alphabet)), out.set("start", Json(r.start)), out.set("rules", rules);
      out.set("weights", r.weights), out.set("context_free", r.context_free), out.set("monotone", r.monotone);
    } else if (form == Form::quantum) {
      const k::systems::Quantum q = k::systems::quantum(s);
      std::vector<double> factors(q.factors.begin(), q.factors.end());
      out.set("factors", Json(factors)), out.set("dimension", static_cast<double>(q.dimension)), out.set("observables", Json(q.observables));
      out.set("jumps", static_cast<double>(q.generator.jumps.size()));
    } else if (form == Form::field) {
      out.set("note", "a field weighted by its action: declared and read, and not computed yet");
      if (!s.change.actions.empty()) out.set("action", k::terms::latex(s.change.actions[0].value));
      out.set("background", !s.constraint.backgrounds.empty());
    }
  } catch (const k::language::ScriptError& e) {
    out.set("error", Json::object({{"line", static_cast<double>(e.at.line)}, {"column", static_cast<double>(e.at.column)}, {"message", std::string(e.what())}}));
  } catch (const k::kernel::Refusal& e) {
    out.set("note", std::string(e.what()));  // declared rightly, and not carried out: said, not counted as an error
  } catch (const std::exception& e) {
    out.set("error", Json::object({{"line", static_cast<double>(s.at.line)}, {"column", 1.0}, {"message", std::string(e.what())}}));
  }
  return out;
}

inline k::braids::Samples samples(const Script& script, const Json& request) {
  k::braids::Samples s;
  if (request["samples"].is_object()) {
    const Json& g = request["samples"];
    s.times = numbers(g["times"]);
    for (std::size_t i = 0; i < g["points"].size(); i++) {
      std::vector<k::braids::Point> row;
      for (std::size_t j = 0; j < g["points"][i].size(); j++) row.push_back({g["points"][i][j][0].as_number(), g["points"][i][j][1].as_number()});
      s.at.push_back(std::move(row));
    }
    return s;
  }
  // From a run: each point is a pair of components of the system.
  const k::systems::Trajectory tr = run(script, request);
  std::vector<std::pair<std::size_t, std::size_t>> pairs;
  for (std::size_t i = 0; i < request["points"].size(); i++) {
    std::size_t index[2];
    for (int c = 0; c < 2; c++) {
      const std::string name = request["points"][i][c].text();
      const auto it = std::find(tr.names.begin(), tr.names.end(), name);
      if (it == tr.names.end()) throw std::invalid_argument("'" + name + "' is not a component of the run");
      index[c] = static_cast<std::size_t>(it - tr.names.begin());
    }
    pairs.push_back({index[0], index[1]});
  }
  if (pairs.size() < 2) throw std::invalid_argument("a braid needs two points or more: give \"points\": [[\"x1\", \"y1\"], [\"x2\", \"y2\"], …], or \"samples\"");
  s.times = tr.t;
  for (const auto& x : tr.x) {
    std::vector<k::braids::Point> row;
    for (const auto& [a, b] : pairs) row.push_back({x[a], x[b]});
    s.at.push_back(std::move(row));
  }
  return s;
}

inline Json braid_json(const morphe::algebra::Braid& b) {
  std::vector<double> word(b.word.begin(), b.word.end()), permutation;
  for (std::size_t p : morphe::algebra::permutation(b)) permutation.push_back(static_cast<double>(p));
  Json out = Json::object({{"strands", static_cast<double>(b.strands)}, {"word", Json(word)}, {"text", b.str()}, {"length", static_cast<double>(b.length())},
                           {"permutation", Json(permutation)}, {"pure", morphe::algebra::pure(b)}, {"exponent_sum", static_cast<double>(morphe::algebra::exponent_sum(b))}});
  const auto reduced = morphe::algebra::handle_reduce(b.word, 200000);
  if (reduced.finished) {
    std::vector<double> w(reduced.word.begin(), reduced.word.end());
    out.set("reduced", Json(w)), out.set("reduced_text", morphe::algebra::Braid(b.strands, reduced.word).str()), out.set("trivial", reduced.word.empty());
  } else {
    out.set("reduced_note", "the reduction passed its bound of steps: the word is not decided");
  }
  const k::braids::Entropy h = k::braids::entropy(b);
  out.set("entropy", Json::object({{"estimate", h.estimate}, {"lower", h.lower}, {"settled", h.settled}, {"applications", static_cast<double>(h.applications)}}));
  if (b.strands >= 2 && b.strands <= 8) out.set("alexander", morphe::algebra::alexander(b).str());
  return out;
}

}  // namespace detail

// The answer to one request.
inline Json answer(const Json& request) {
  using namespace detail;
  const std::string op = request["op"].text("read");
  if (op == "grammar") return Json::object({{"text", std::string(k::language::grammar_text)}});
  if (op == "catalogue") {
    Json models = Json::array();
    for (const auto& e : k::catalogue::entries())
      models.push(Json::object({{"id", e.id}, {"title", e.title}, {"field", e.field}, {"about", e.about}, {"source", e.source},
                                {"form", std::string(k::systems::name(k::systems::form_of(k::catalogue::system(e))))}, {"text", e.text}}));
    return Json::object({{"models", models}});
  }

  if (op == "word") {
    std::vector<int> word;
    for (double v : numbers(request["word"])) word.push_back(static_cast<int>(v));
    return braid_json(morphe::algebra::Braid(count(request, "strands", 3), word));
  }
  if (op == "warnings" && request["series"].is_array()) {
    const std::vector<double> x = numbers(request["series"]);
    k::analysis::WarningOptions o;
    o.window = number(request, "window", 0.5), o.bandwidth = number(request, "bandwidth", 0.1), o.surrogates = count(request, "surrogates", 200);
    const auto w = k::analysis::early_warnings(x, o);
    Json out = Json::object({{"at", Json(w.at)}, {"variance", Json(w.variance)}, {"autocorrelation", Json(w.autocorrelation)}, {"skewness", Json(w.skewness)}, {"trend", Json(w.trend)},
                             {"residual", Json(w.residual)}, {"tau_variance", w.tau_variance}, {"tau_autocorrelation", w.tau_autocorrelation}, {"tau_skewness", w.tau_skewness},
                             {"p_variance", w.p_variance}, {"p_autocorrelation", w.p_autocorrelation}, {"series", Json(x)}});
    if (x.size() >= 512) {
      const auto f = k::analysis::dfa(x);
      out.set("dfa", Json::object({{"exponent", f.exponent}, {"scales", Json(f.scales)}, {"fluctuation", Json(f.fluctuation)}}));
    }
    return out;
  }

  Script script;
  if (request["script"].is_string()) {
    try {
      script = k::language::read(request["script"].as_string());
    } catch (const k::language::ScriptError& e) {
      if (op != "read") throw;
      return Json::object({{"systems", Json::array()}, {"error", Json::object({{"line", static_cast<double>(e.at.line)}, {"column", static_cast<double>(e.at.column)}, {"message", std::string(e.what())}})}});
    }
  }

  if (op == "read") {
    Json systems = Json::array();
    for (const auto& s : script.systems) systems.push(described(script, s));
    return Json::object({{"systems", systems}});
  }
  if (op == "derive") return Json::object({{"text", k::language::write(runnable(script, system_of(script, request)))}});

  if (op == "simulate") {
    const System& declared = system_of(script, request);
    if (k::systems::form_of(declared) == Form::quantum) {
      const k::systems::Quantum q = k::systems::quantum(declared);
      std::vector<std::size_t> levels(q.factors.size(), 0);
      for (std::size_t i = 0; i < std::min(levels.size(), request["levels"].size()); i++) levels[i] = static_cast<std::size_t>(request["levels"][i].as_number());
      const auto tr = k::systems::evolve(q, q.basis(levels), number(request, "t1", 20), number(request, "step", 0.05));
      return Json::object({{"names", Json(tr.names)}, {"t", Json(tr.t)}, {"x", columns(tr.expectations, tr.names.size())}, {"purity", Json(tr.purity)}, {"events", Json::array()},
                           {"method", "the exact propagator of the generator over each step"}});
    }
    std::string method;
    Json out = trajectory(run(script, request, &method));
    out.set("method", method);
    return out;
  }

  if (op == "braid") {
    const k::braids::Samples s = samples(script, request);
    const double angle = number(request, "angle", 0);
    const k::braids::Reading r = k::braids::read(s, angle);
    Json out = Json::object({{"braid", braid_json(r.braid)}});
    Json crossings = Json::array();
    for (const auto& c : r.crossings) crossings.push(Json::object({{"t", c.time}, {"letter", static_cast<double>(c.letter)}, {"left", static_cast<double>(c.left)}, {"right", static_cast<double>(c.right)}}));
    out.set("crossings", crossings);
    std::vector<double> first(r.first.begin(), r.first.end()), last(r.last.begin(), r.last.end());
    out.set("first", Json(first)), out.set("last", Json(last));
    // The strands, for drawing: the times and each point's path, thinned to a thousand samples at most.
    const std::size_t stride = std::max<std::size_t>(1, s.times.size() / 1000);
    std::vector<double> times;
    Json paths = Json::array();
    for (std::size_t p = 0; p < s.points(); p++) paths.push(Json::array());
    for (std::size_t i = 0; i < s.times.size(); i += stride) {
      times.push_back(s.times[i]);
      for (std::size_t p = 0; p < s.points(); p++) paths.as_array()[p].push(Json::array({Json(s.at[i][p].x), Json(s.at[i][p].y)}));
    }
    out.set("times", Json(times)), out.set("paths", paths), out.set("duration", s.times.back() - s.times.front());
    out.set("braiding_exponent", k::braids::braiding_exponent(r.braid, s.times.back() - s.times.front()));
    if (request["speed"].is_number() || request["speed"].is_array()) {
      k::braids::Assumptions A;
      A.speed = request["speed"].is_array() ? numbers(request["speed"]) : std::vector<double>{request["speed"].as_number()};
      A.error = number(request, "error", 0), A.separation = number(request, "separation", 0), A.angle = angle;
      const k::braids::Certificate c = k::braids::certify(s, A);
      out.set("certificate", Json::object({{"certified", c.certified}, {"margin", c.margin}, {"draft_margin", c.coarse_margin}, {"open", static_cast<double>(c.open)}, {"endpoints", c.endpoints},
                                            {"reason", c.reason}, {"assumptions", Json(A.stated())}}));
      if (s.points() == 2 && A.error == 0) {
        try {
          const auto w = k::braids::pair_winding(s, 0, 1, A);
          std::vector<double> values(w.values.begin(), w.values.end());
          out.set("winding", Json::object({{"values", Json(values)}, {"certified", w.certified()}, {"complete", w.complete}, {"reason", w.reason}}));
        } catch (const k::kernel::Refusal& e) {
          out.set("winding", Json::object({{"refused", std::string(e.what())}}));
        }
      }
    }
    return out;
  }

  if (op == "rules") {
    const k::systems::Rewriting r = k::systems::rewriting(system_of(script, request));
    k::systems::Bounds b;
    b.max_length = count(request, "length", 10), b.max_forms = count(request, "forms", 20000);
    const k::systems::Histories h = k::systems::explore(r, b);
    Json words = Json::array();
    for (const auto& w : k::systems::language(r, h)) {
      std::string text;
      for (const auto& l : w.word) text += (text.empty() ? "" : " ") + l;
      words.push(Json::object({{"word", text}, {"length", static_cast<double>(w.word.size())}, {"histories", w.histories}, {"probability", w.probability}}));
    }
    Json out = Json::object({{"words", words}, {"forms", static_cast<double>(h.forms.size())}, {"complete", h.complete}, {"cut", h.cut}, {"context_free", r.context_free}});
    // The graph of forms, for drawing, when it is small.
    if (h.forms.size() <= 400) {
      Json forms = Json::array(), edges = Json::array();
      for (std::size_t i = 0; i < h.forms.size(); i++) {
        std::string text;
        for (const auto& l : h.forms[i]) text += (text.empty() ? "" : " ") + l;
        forms.push(Json::object({{"text", text}, {"final", static_cast<bool>(h.final[i])}, {"terminal", r.terminal(h.forms[i])}, {"histories", h.paths[i]}}));
      }
      for (const auto& e : h.edges) edges.push(Json::array({Json(static_cast<double>(e.from)), Json(static_cast<double>(e.to)), Json(r.rules[e.rule].name)}));
      out.set("graph", Json::object({{"forms", forms}, {"edges", edges}}));
    }
    calx::random::Stream stream(static_cast<std::uint64_t>(number(request, "seed", 1)), 0x72756c65u, 0, 0);
    Json sampled = Json::array();
    for (const auto& form : k::systems::sample(r, stream, 200)) {
      std::string text;
      for (const auto& l : form) text += (text.empty() ? "" : " ") + l;
      sampled.push(text);
    }
    out.set("sample", sampled);
    return out;
  }

  if (op == "quantum") {
    const k::systems::Quantum q = k::systems::quantum(system_of(script, request));
    const auto st = physis::quantum::stationary(q.generator);
    std::vector<std::complex<double>> eig = physis::quantum::spectrum(q.generator);
    if (eig.size() > 400) eig.resize(400);
    std::vector<double> populations;
    for (std::size_t i = 0; i < q.dimension; i++) populations.push_back(st.state(i, i).real());
    return Json::object({{"dimension", static_cast<double>(q.dimension)}, {"observables", Json(q.observables)}, {"stationary", Json(q.expectations(st.state))}, {"populations", Json(populations)},
                         {"stationary_dimension", static_cast<double>(st.dimension)}, {"gap", std::isfinite(st.gap) ? Json(st.gap) : Json()}, {"purity", physis::quantum::purity(st.state)},
                         {"spectrum", complex_list(eig)}});
  }

  if (op == "fit" || op == "discover" || op == "match" || op == "operator" || op == "forecast") {
    const k::systems::Trajectory rec = record(script, request["record"]);
    if (op == "discover") {
      k::induction::FitOptions o;
      o.threshold = number(request, "threshold", 0.05), o.weak = request["weak"].is_bool() && request["weak"].as_bool();
      const bool discrete = request["discrete"].is_bool() && request["discrete"].as_bool();
      const auto f = k::induction::fit(k::induction::discovery_template(rec, static_cast<unsigned>(number(request, "degree", 2)), request["name"].text("discovered"), discrete), rec, o);
      return Json::object({{"text", k::language::write(f.system)}, {"terms", static_cast<double>(f.parameters)}, {"residual", f.residual}, {"method", f.method}, {"samples", static_cast<double>(f.samples)}});
    }
    if (op == "fit") {
      const System& tmpl = system_of(script, request, "template");
      k::induction::FitOptions o;
      o.threshold = number(request, "threshold", 0.05), o.weak = request["weak"].is_bool() && request["weak"].as_bool();
      const System asked = request["free"].is_array() ? k::induction::as_template(tmpl, [&] {
        std::vector<std::string> names;
        for (std::size_t i = 0; i < request["free"].size(); i++) names.push_back(request["free"][i].text());
        return names;
      }()) : tmpl;
      const auto f = k::systems::form_of(asked) == Form::stochastic ? k::induction::fit_stochastic(asked, rec) : k::induction::fit(asked, rec, o);
      Json found = Json::object();
      for (const auto& [name, value] : f.numbers) found.set(name, value);
      return Json::object({{"text", k::language::write(f.system)}, {"numbers", found}, {"residual", f.residual}, {"method", f.method}, {"linear", f.linear}, {"iterations", static_cast<double>(f.iterations)}});
    }
    if (op == "match") {
      std::vector<System> candidates;
      for (std::size_t i = 0; i < request["candidates"].size(); i++) {
        const std::string name = request["candidates"][i].text();
        const System* s = script.find(name);
        if (!s) throw std::invalid_argument("the script declares no system named '" + name + "'");
        candidates.push_back(s->unknowns.empty() ? k::induction::as_template(*s) : *s);
      }
      const std::size_t proposed = candidates.size();
      if (request["catalogue"].is_bool() && request["catalogue"].as_bool())
        for (const auto& c : k::catalogue::candidates(rec.names, request["discrete"].is_bool() && request["discrete"].as_bool())) candidates.push_back(k::induction::as_template(c));
      if (candidates.empty()) throw k::kernel::Refusal("no candidate is given: name systems of the script, or ask for the catalogue's; it holds no plain model of " + std::to_string(rec.names.size()) + " component(s) and this kind of time");
      Json ranked = Json::array();
      // A model of the catalogue is tried in several orders of its states: the best of each name is kept.
      std::set<std::string> seen;
      for (const auto& c : k::induction::match(candidates, rec)) {
        if (!seen.insert(c.name).second) continue;
        Json row = Json::object({{"name", c.name}, {"fitted", c.fitted}});
        if (c.fitted) {
          Json found = Json::object();
          for (const auto& [name, value] : c.fit.numbers) found.set(name, value);
          row.set("one_step", c.one_step), row.set("horizon", static_cast<double>(c.horizon)), row.set("criterion", c.criterion), row.set("numbers", found);
          row.set("parameters", static_cast<double>(c.fit.parameters)), row.set("text", k::language::write(c.fit.system));
        } else {
          row.set("refused", c.refused);
        }
        ranked.push(row);
      }
      return Json::object({{"ranking", ranked}, {"tried", static_cast<double>(candidates.size())}, {"proposed", static_cast<double>(proposed)},
                           {"held_out", static_cast<double>(rec.x.size() - static_cast<std::size_t>(0.7 * static_cast<double>(rec.x.size())))}});
    }
    if (op == "operator") {
      const auto A = k::induction::dmd(rec);
      return Json::object({{"eigenvalues", complex_list(A.eigenvalues)}, {"rates", complex_list(A.rates)}, {"residual", A.residual}});
    }
    const std::size_t cut = static_cast<std::size_t>(number(request, "train", 0.7) * static_cast<double>(rec.x.size()));
    k::systems::Trajectory head;
    head.names = rec.names;
    head.t.assign(rec.t.begin(), rec.t.begin() + static_cast<std::ptrdiff_t>(cut)), head.x.assign(rec.x.begin(), rec.x.begin() + static_cast<std::ptrdiff_t>(cut));
    const auto f = k::induction::forecaster(head, count(request, "delays", 2), 1, number(request, "ridge", 1e-6));
    const std::vector<State> truth(rec.x.begin() + static_cast<std::ptrdiff_t>(cut), rec.x.end());
    const std::vector<State> predicted = f.forecast(head.x, truth.size());
    const std::vector<double> times(rec.t.begin() + static_cast<std::ptrdiff_t>(cut), rec.t.end());
    return Json::object({{"names", Json(rec.names)}, {"t", Json(times)}, {"truth", columns(truth, rec.names.size())}, {"forecast", columns(predicted, rec.names.size())},
                         {"horizon", static_cast<double>(k::induction::horizon(predicted, truth, number(request, "tolerance", 0.3)))}, {"one_step", f.training_error},
                         {"features", static_cast<double>(f.features)}});
  }

  // The rest are of a flow or a map, or of a stochastic system.
  const System& declared = system_of(script, request);
  const Form form = k::systems::form_of(declared);

  if (op == "warnings") {
    const k::systems::Trajectory tr = run(script, request);
    const std::string name = request["component"].text(tr.names.front());
    const auto it = std::find(tr.names.begin(), tr.names.end(), name);
    if (it == tr.names.end()) throw std::invalid_argument("'" + name + "' is not a component of the run");
    Json again = Json::object({{"op", "warnings"}, {"series", Json(tr.column(static_cast<std::size_t>(it - tr.names.begin())))}});
    for (const char* key : {"window", "bandwidth", "surrogates"})
      if (request[key].is_number()) again.set(key, request[key]);
    Json out = answer(again);
    out.set("t", Json(tr.t));
    return out;
  }

  if (op == "landscape" && form == Form::stochastic) {
    const k::systems::Stochastic d = k::systems::stochastic(declared);
    Json out = Json::object();
    if (d.dimension() == 1 && d.sources.size() == 1 && !d.discrete) {
      const auto p = k::analysis::stationary_landscape(d, number(request, "x0", -2), number(request, "x1", 2), count(request, "cells", 400));
      out.set("exact", Json::object({{"x", Json(p.x)}, {"u", Json(p.u)}, {"minima", Json(p.minima)}, {"maxima", Json(p.maxima)}}));
    }
    k::analysis::DensityOptions o;
    o.x0 = number(request, "x0", -2), o.x1 = number(request, "x1", 2), o.y0 = number(request, "y0", -2), o.y1 = number(request, "y1", 2);
    o.nx = count(request, "nx", 60), o.ny = d.dimension() == 1 ? 1 : count(request, "ny", 60);
    o.time = number(request, "time", 2000), o.step = number(request, "step", 0.01), o.seed = static_cast<std::uint64_t>(number(request, "seed", 1));
    if (request["x"].is_string()) o.component_x = static_cast<std::size_t>(std::max(0, d.state_index(request["x"].as_string())));
    if (request["y"].is_string()) o.component_y = static_cast<std::size_t>(std::max(0, d.state_index(request["y"].as_string())));
    const auto L = k::analysis::density_landscape(d, initial_state(d.states, request), o);
    out.set("path", Json::object({{"xs", Json(L.xs)}, {"ys", Json(L.ys)}, {"u", Json(L.u)}, {"ceiling", L.ceiling}, {"samples", static_cast<double>(L.samples)}}));
    out.set("kind", "the stationary landscape, minus the logarithm of the stationary density");
    return out;
  }

  const System flow = runnable(script, declared);
  k::systems::Dynamics d = k::systems::dynamics(flow);
  set_params(d, request);
  const auto component = [&](const char* key, std::size_t fallback) {
    if (!request[key].is_string()) return fallback;
    const int i = d.state_index(request[key].as_string());
    if (i < 0) throw std::invalid_argument("'" + request[key].as_string() + "' is not a component of " + d.name);
    return static_cast<std::size_t>(i);
  };

  if (op == "lyapunov") {
    physis::dynamics::LyapunovOptions o;
    o.step = number(request, "step", 0.005), o.transient = number(request, "transient", d.discrete ? 1000 : 20), o.time = number(request, "time", d.discrete ? 50000 : 300);
    o.count = std::min<std::size_t>(count(request, "count", 0), d.dimension());  // the largest exponents asked for; 0 for all
    o.renormalise_every = d.discrete ? 1 : 10, o.record_every = d.discrete ? std::max<std::size_t>(1, static_cast<std::size_t>(o.time / 200)) : std::max<std::size_t>(1, static_cast<std::size_t>(o.time / o.step / 10 / 200));
    const auto e = k::analysis::lyapunov(d, initial_state(d.states, request, 0.1), o);
    Json running = Json::array();
    for (std::size_t j = 0; j < e.exponents.size(); j++) {
      std::vector<double> column;
      for (const auto& r : e.running) column.push_back(r[j]);
      running.push(Json(column));
    }
    return Json::object({{"exponents", Json(e.exponents)}, {"error", Json(e.error)}, {"sum", e.sum}, {"dimension", e.dimension}, {"time", e.time}, {"times", Json(e.times)}, {"running", running}});
  }
  if (op == "averages") {
    k::analysis::StatisticsOptions o;
    o.step = number(request, "step", 0.005), o.transient = number(request, "transient", d.discrete ? 1000 : 50), o.time = number(request, "time", d.discrete ? 100000 : 500);
    o.every = count(request, "every", d.discrete ? 1 : 10), o.blocks = std::max<std::size_t>(count(request, "blocks", 10), 2);
    if (!d.discrete && o.time / o.step > 2e7) throw k::kernel::Refusal("the averages would take more than twenty million steps: ask for a shorter time or a longer step");
    const auto st = k::analysis::averages(d, initial_state(d.states, request), o);
    std::vector<std::string> names;
    std::vector<double> mean, error, second;
    for (const auto& a : st.of) names.push_back(a.name), mean.push_back(a.mean), error.push_back(a.error), second.push_back(a.second);
    Json out = Json::object({{"names", Json(names)}, {"mean", Json(mean)}, {"error", Json(error)}, {"second", Json(second)}, {"time", st.time},
                             {"samples", static_cast<double>(st.samples)}, {"blocks", static_cast<double>(st.blocks)}});
    if (request["of"].is_array() && request["against"].is_array()) {
      if (request["of"].size() != request["against"].size()) throw std::invalid_argument("a power law is asked through as many observations as numbers they are set against");
      std::vector<double> x = numbers(request["against"]), y, low, high;
      std::vector<std::string> fitted;
      for (std::size_t i = 0; i < request["of"].size(); i++) {
        const std::string name = request["of"][i].text();
        const auto it = std::find(names.begin(), names.end(), name);
        if (it == names.end()) throw std::invalid_argument("'" + name + "' is not an observation of " + d.name);
        const std::size_t at = static_cast<std::size_t>(it - names.begin());
        y.push_back(mean[at]), fitted.push_back(name);
      }
      const auto law = k::analysis::scaling(x, y);
      out.set("scaling", Json::object({{"exponent", law.exponent}, {"error", law.error}, {"prefactor", law.prefactor}, {"of", Json(fitted)}, {"against", Json(x)}, {"means", Json(y)}}));
    }
    return out;
  }
  if (op == "equilibria") {
    k::analysis::Box box;
    for (const auto& st : d.states) {
      const Json& range = request["box"][st];
      box.lower.push_back(range.is_array() ? range[0].as_number() : number(request, "lower", -10));
      box.upper.push_back(range.is_array() ? range[1].as_number() : number(request, "upper", 10));
    }
    Json found = Json::array();
    for (const auto& e : k::analysis::equilibria(d, box, count(request, "seeds", 400)))
      found.push(Json::object({{"x", Json(e.x)}, {"kind", e.kind}, {"stable", e.stable}, {"eigenvalues", complex_list(e.eigenvalues)}}));
    return Json::object({{"names", Json(d.states)}, {"equilibria", found}, {"note", "these are the equilibria that Newton's method reached from the seeds; one that no seed reached is not listed"}});
  }
  if (op == "diagram") {
    const std::string parameter = request["parameter"].text();
    const double from = number(request, "from", 0), to = number(request, "to", 1);
    const std::size_t n = count(request, "count", 200);
    std::vector<double> values;
    for (std::size_t i = 0; i < n; i++) values.push_back(from + (to - from) * static_cast<double>(i) / static_cast<double>(std::max<std::size_t>(n - 1, 1)));
    k::analysis::DiagramOptions o;
    o.transient = number(request, "transient", d.discrete ? 500 : 100), o.keep = count(request, "keep", d.discrete ? 120 : 40), o.time = number(request, "time", 100), o.step = number(request, "step", 0.01);
    o.component = component("component", 0);
    const auto diagram = k::analysis::bifurcation_diagram(d, parameter, values, initial_state(d.states, request, 0.3), o);
    std::vector<double> xs, ys;
    for (std::size_t i = 0; i < diagram.values.size(); i++)
      for (double v : diagram.points[i]) xs.push_back(diagram.values[i]), ys.push_back(v);
    return Json::object({{"parameter", parameter}, {"observable", diagram.observable}, {"x", Json(xs)}, {"y", Json(ys)}, {"what", d.discrete ? "iterates on the attractor" : "maxima of the component on the attractor"}});
  }
  if (op == "basins") {
    k::analysis::BasinOptions o;
    o.component_x = component("x", 0), o.component_y = component("y", std::min<std::size_t>(1, d.dimension() - 1));
    o.time = number(request, "time", d.discrete ? 500 : 100), o.step = number(request, "step", 0.01), o.distinct = number(request, "distinct", 1e-2);
    const std::size_t nx = count(request, "nx", 60), ny = count(request, "ny", 60);
    const double x0 = number(request, "x0", -2), x1 = number(request, "x1", 2), y0 = number(request, "y0", -2), y1 = number(request, "y1", 2);
    std::vector<double> xs, ys;
    for (std::size_t i = 0; i < nx; i++) xs.push_back(x0 + (x1 - x0) * (static_cast<double>(i) + 0.5) / static_cast<double>(nx));
    for (std::size_t i = 0; i < ny; i++) ys.push_back(y0 + (y1 - y0) * (static_cast<double>(i) + 0.5) / static_cast<double>(ny));
    const auto b = k::analysis::basins(d, initial_state(d.states, request), xs, ys, o);
    std::vector<double> labels(b.label.begin(), b.label.end());
    Json attractors = Json::array();
    for (const auto& a : b.attractors) attractors.push(Json(a));
    Json kinds = Json::array();
    for (const auto& kind : b.kind) kinds.push(Json(kind));
    return Json::object({{"xs", Json(xs)}, {"ys", Json(ys)}, {"label", Json(labels)}, {"attractors", attractors}, {"share", Json(b.share)}, {"speed", Json(b.speed)}, {"kind", kinds}, {"names", Json(d.states)},
                         {"x", d.states[o.component_x]}, {"y", d.states[o.component_y]}});
  }
  if (op == "landscape") {
    if (d.dimension() != 1) {
      std::vector<double> lower, upper;
      for (std::size_t i = 0; i < d.dimension(); i++) lower.push_back(number(request, "x0", -2)), upper.push_back(number(request, "x1", 2));
      const double a = k::analysis::asymmetry(d, lower, upper);
      if (a > 1e-9) throw k::kernel::Refusal(d.name + " is not a gradient: its Jacobian is not symmetric (by " + k::terms::number_text(a) + " of its size), so it has no potential. A stochastic system has a stationary landscape; a flow has basins");
      // The potential on a grid of two components, from the origin of the box.
      const std::size_t cx = component("x", 0), cy = component("y", 1), n = count(request, "nx", 50);
      const double x0 = number(request, "x0", -2), x1 = number(request, "x1", 2), y0 = number(request, "y0", -2), y1 = number(request, "y1", 2);
      std::vector<double> xs, ys, u;
      for (std::size_t i = 0; i < n; i++) xs.push_back(x0 + (x1 - x0) * (static_cast<double>(i) + 0.5) / static_cast<double>(n)), ys.push_back(y0 + (y1 - y0) * (static_cast<double>(i) + 0.5) / static_cast<double>(n));
      const State base = initial_state(d.states, request);
      double least = 1e300;
      for (double y : ys)
        for (double x : xs) {
          State at = base;
          at[cx] = x, at[cy] = y;
          u.push_back(k::analysis::potential_between(d, base, at, 60));
          least = std::min(least, u.back());
        }
      for (double& v : u) v -= least;
      return Json::object({{"path", Json::object({{"xs", Json(xs)}, {"ys", Json(ys)}, {"u", Json(u)}})}, {"kind", "the potential of a gradient flow"}});
    }
    const auto p = k::analysis::potential(d, number(request, "x0", -2), number(request, "x1", 2), count(request, "cells", 400));
    return Json::object({{"exact", Json::object({{"x", Json(p.x)}, {"u", Json(p.u)}, {"minima", Json(p.minima)}, {"maxima", Json(p.maxima)}})}, {"kind", "the potential, minus the integral of the field"}});
  }
  if (op == "transfer") {
    const auto T = k::analysis::transfer(d, number(request, "x0", 0), number(request, "x1", 1), count(request, "cells", 200), count(request, "points", 50), number(request, "time", 1),
                                         component("component", 0), initial_state(d.states, request));
    std::vector<std::complex<double>> eig(T.eigenvalues.begin(), T.eigenvalues.begin() + static_cast<std::ptrdiff_t>(std::min<std::size_t>(T.eigenvalues.size(), 60)));
    return Json::object({{"centres", Json(T.centres)}, {"density", Json(T.stationary)}, {"eigenvalues", complex_list(eig)}, {"lost", T.lost}});
  }
  throw std::invalid_argument("science::dynamics: no operation named '" + op + "'");
}

}  // namespace science::dynamics
