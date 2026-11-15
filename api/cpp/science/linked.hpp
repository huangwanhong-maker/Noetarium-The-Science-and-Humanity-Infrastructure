// Linked data for any work's engine (ADR 0118): one composition of symploke, answering requests in JSON. A request holds the
// documents it concerns as text; nothing is read from a file, and nothing is kept between requests.
//
// A document is {"text": "…", "format": "turtle" | "trig" | "ntriples" | "nquads", "base"?: IRI, "graph"?: IRI}; a Turtle or
// N-Triples document given a "graph" is read into that named graph. A term is {"kind": "iri" | "blank" | "literal",
// "value": …, "datatype"?: IRI, "language"?: tag}; a statement is [s, p, o] or [s, p, o, g].
//
//   parse     {"document"}                         the statements, prefixes and base; a document that departs from its
//                                                   grammar answers {"error": …, "line", "column"} instead
//   write     {"quads", "prefixes"?, "format"}     a document: {"text"}
//   summary   {"documents"}                        statements, graphs, classes with their instances, predicates with their
//                                                   counts, subjects
//   describe  {"documents", "node", "most"?}       a node's statements, outgoing and incoming
//   query     {"documents", "query"}               SPARQL's core: {"form", "variables", "rows"} | {"truth"} | {"triples"}
//   validate  {"documents", "shapes"}              SHACL: {"conforms", "results": [...], "report": Turtle}
//   entail    {"documents", "regime"?}             RDFS ("rdfs", default) or RDFS with part of OWL 2 RL ("owl"): the
//                                                   statements derived with their rules and premises, inconsistencies
//   compare   {"a", "b"}                           isomorphism up to blank nodes: {"status", "reason"}
//   edit      {"document", "removed"?, "added"?, "format"?, "prefixes"?}   the document with a change applied and written
//                                                   back, and the change that undoes it; a statement that is not RDF is refused
//
// symploke's own refusals (an unsupported query, a malformed shape, a statement that is not RDF) answer as errors with their
// reason; the work shows them as they are.

#pragma once

#include "json.hpp"

#include <symploke/canonical/canonical.hpp>
#include <symploke/entailment/entailment.hpp>
#include <symploke/query/query.hpp>
#include <symploke/shapes/shapes.hpp>
#include <symploke/store/store.hpp>
#include <symploke/syntax/syntax.hpp>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::linked {

namespace st = symploke::terms;
namespace sy = symploke::syntax;
namespace ss = symploke::store;

inline Json term_json(const st::Term& t) {
  Json j = Json::object();
  j.set("kind", std::string(t.is_iri() ? "iri" : t.is_blank() ? "blank" : "literal"));
  j.set("value", t.value);
  if (t.is_literal()) {
    j.set("datatype", t.datatype);
    if (!t.language.empty()) j.set("language", t.language);
  }
  return j;
}

inline st::Term term_of(const Json& j) {
  if (j.is_null()) return {};
  const std::string kind = j["kind"].text("iri"), value = j["value"].text();
  if (kind == "blank") return st::blank(value);
  if (kind == "literal") {
    if (j.has("language") && !j["language"].text().empty()) return st::lang(value, j["language"].text());
    return st::literal(value, j["datatype"].text(st::vocabulary::xsd_string));
  }
  return st::iri(value);
}

inline Json quad_json(const st::Quad& q) {
  Json a = Json::array({term_json(q.s), term_json(q.p), term_json(q.o)});
  if (!q.g.empty()) a.push(term_json(q.g));
  return a;
}

inline st::Quad quad_of(const Json& a) {
  if (!a.is_array() || a.size() < 3) throw std::invalid_argument("a statement is [subject, predicate, object] or [subject, predicate, object, graph]");
  return {term_of(a[0]), term_of(a[1]), term_of(a[2]), a.size() > 3 ? term_of(a[3]) : st::Term{}};
}

inline sy::Document read(const Json& doc) {
  const std::string format = doc["format"].text("turtle"), text = doc["text"].text(), base = doc["base"].text();
  sy::Document d;
  if (format == "turtle" || format == "ttl") d = sy::read_turtle(text, base);
  else if (format == "trig") d = sy::read_trig(text, base);
  else if (format == "ntriples" || format == "nt") d = sy::read_ntriples(text);
  else if (format == "nquads" || format == "nq") d = sy::read_nquads(text);
  else throw std::invalid_argument("a document's format is turtle, trig, ntriples or nquads, not " + format);
  if (doc.has("graph") && !doc["graph"].text().empty()) {
    const st::Term g = st::iri(doc["graph"].text());
    for (auto& q : d.quads)
      if (q.g.empty()) q.g = g;
  }
  return d;
}

inline ss::Dataset load(const Json& documents, st::Prefixes* prefixes = nullptr) {
  ss::Dataset d;
  ss::Change c;
  const auto take = [&](const Json& doc) {
    auto r = read(doc);
    if (prefixes)
      for (const auto& [p, ns] : r.prefixes.map) prefixes->map.emplace(p, ns);
    c.added.insert(c.added.end(), r.quads.begin(), r.quads.end());
  };
  if (documents.is_array())
    for (std::size_t k = 0; k < documents.size(); k++) take(documents[k]);
  else if (documents.is_object())
    take(documents);
  d.apply(c);
  return d;
}

inline Json prefixes_json(const st::Prefixes& p) {
  Json j = Json::object();
  for (const auto& [k, v] : p.map) j.set(k, v);
  return j;
}

inline st::Prefixes prefixes_of(const Json& j) {
  st::Prefixes p;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object()) p.map[k] = v.text();
  return p;
}

inline std::string write(const std::vector<st::Quad>& quads, const st::Prefixes& prefixes, const std::string& format) {
  bool named = false;
  for (const auto& q : quads) named = named || !q.g.empty();
  if (format == "nquads" || format == "nq") return sy::write_nquads(quads);
  if (format == "ntriples" || format == "nt") {
    if (named) throw std::invalid_argument("N-Triples holds one graph; these statements are in named graphs (write N-Quads or TriG)");
    std::vector<st::Triple> t;
    for (const auto& q : quads) t.push_back(q.triple());
    return sy::write_ntriples(t);
  }
  if (format == "turtle" || format == "ttl") {
    if (named) throw std::invalid_argument("Turtle holds one graph; these statements are in named graphs (write TriG)");
    std::vector<st::Triple> t;
    for (const auto& q : quads) t.push_back(q.triple());
    return sy::write_turtle(t, prefixes);
  }
  return sy::write_trig(quads, prefixes);
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "parse") {
    try {
      const auto d = read(r["document"]);
      Json quads = Json::array();
      for (const auto& q : d.quads) quads.push(quad_json(q));
      return Json::object({{"quads", quads}, {"prefixes", prefixes_json(d.prefixes)}, {"base", d.base}, {"count", static_cast<double>(d.quads.size())}});
    } catch (const sy::Error& e) {
      return Json::object({{"error", std::string(e.what())}, {"line", static_cast<double>(e.line)}, {"column", static_cast<double>(e.column)}});
    }
  }
  if (op == "write") {
    std::vector<st::Quad> quads;
    for (std::size_t k = 0; k < r["quads"].size(); k++) quads.push_back(quad_of(r["quads"][k]));
    return Json::object({{"text", write(quads, prefixes_of(r["prefixes"]), r["format"].text("trig"))}});
  }
  if (op == "summary") {
    const auto d = load(r["documents"]);
    std::map<st::Term, std::size_t> classes, predicates;
    std::set<st::Term> subjects;
    for (const auto& q : d.quads()) {
      predicates[q.p]++;
      subjects.insert(q.s);
      if (q.p.value == st::vocabulary::rdf_type) classes[q.o]++;
    }
    Json graphs = Json::array(), cls = Json::array(), preds = Json::array();
    for (const auto& g : d.graphs()) graphs.push(Json::object({{"graph", g.empty() ? Json() : term_json(g)}, {"statements", static_cast<double>(d.size(g))}}));
    for (const auto& [c, n] : classes) cls.push(Json::object({{"class", term_json(c)}, {"instances", static_cast<double>(n)}}));
    for (const auto& [p, n] : predicates) preds.push(Json::object({{"predicate", term_json(p)}, {"count", static_cast<double>(n)}}));
    return Json::object({{"statements", static_cast<double>(d.size())}, {"subjects", static_cast<double>(subjects.size())}, {"graphs", graphs}, {"classes", cls}, {"predicates", preds}});
  }
  if (op == "describe") {
    const auto d = load(r["documents"]);
    const st::Term node = term_of(r["node"]);
    const std::size_t most = r.has("most") ? static_cast<std::size_t>(r["most"].as_number()) : 500;
    Json out = Json::array(), in = Json::array();
    for (const auto& q : d.match({node, std::nullopt, std::nullopt, std::nullopt}))
      if (out.size() < most) out.push(quad_json(q));
    for (const auto& q : d.match({std::nullopt, std::nullopt, node, std::nullopt}))
      if (in.size() < most) in.push(quad_json(q));
    return Json::object({{"node", term_json(node)}, {"out", out}, {"in", in}});
  }
  if (op == "query") {
    const auto d = load(r["documents"]);
    const auto res = symploke::query::run(d, r["query"].text());
    using F = symploke::query::Result::Form;
    if (res.form == F::ask) return Json::object({{"form", std::string("ask")}, {"truth", res.truth}});
    if (res.form == F::construct) {
      Json t = Json::array();
      for (const auto& x : res.triples) t.push(Json::array({term_json(x.s), term_json(x.p), term_json(x.o)}));
      return Json::object({{"form", std::string("construct")}, {"triples", t}});
    }
    Json vars = Json::array(), rows = Json::array();
    for (const auto& v : res.variables) vars.push(v);
    for (const auto& row : res.rows) {
      Json a = Json::array();
      for (const auto& cell : row) a.push(cell ? term_json(*cell) : Json());
      rows.push(a);
    }
    return Json::object({{"form", std::string("select")}, {"variables", vars}, {"rows", rows}});
  }
  if (op == "validate") {
    const auto data = load(r["documents"]);
    const auto shapes = load(r["shapes"]);
    const auto rep = symploke::shapes::validate(data, shapes);
    Json results = Json::array();
    for (const auto& x : rep.results) {
      Json o = Json::object({{"focus", term_json(x.focus)}, {"shape", term_json(x.shape)}, {"path", x.path}, {"component", x.component}, {"severity", x.severity}, {"message", x.message}});
      if (x.has_value) o.set("value", term_json(x.value));
      results.push(o);
    }
    auto prefixes = st::common_prefixes();
    return Json::object({{"conforms", rep.conforms}, {"results", results}, {"report", sy::write_turtle(symploke::shapes::report_graph(rep), prefixes)}});
  }
  if (op == "entail") {
    auto d = load(r["documents"]);
    const auto regime = r["regime"].text("rdfs") == "owl" ? symploke::entailment::Regime::rdfs_owl : symploke::entailment::Regime::rdfs;
    // Every graph's entailments, each within its own graph.
    Json derived = Json::array(), problems = Json::array();
    std::size_t rounds = 0;
    for (const auto& g : d.graphs()) {
      const auto e = symploke::entailment::materialise(d, regime, g);
      rounds = std::max(rounds, e.rounds);
      for (const auto& x : e.derived) {
        Json premises = Json::array();
        for (const auto& p : x.premises) premises.push(Json::array({term_json(p.s), term_json(p.p), term_json(p.o)}));
        Json c = Json::array({term_json(x.conclusion.s), term_json(x.conclusion.p), term_json(x.conclusion.o)});
        if (!g.empty()) c.push(term_json(g));
        derived.push(Json::object({{"statement", c}, {"rule", x.rule}, {"premises", premises}}));
      }
      for (const auto& why : e.inconsistencies) problems.push(why);
    }
    return Json::object({{"derived", derived}, {"inconsistencies", problems}, {"rounds", static_cast<double>(rounds)}});
  }
  if (op == "compare") {
    const auto a = load(r["a"]), b = load(r["b"]);
    const auto v = symploke::canonical::isomorphic(a.triples(), b.triples());
    return Json::object({{"status", std::string(apodeixis::kernel::word(v.status))}, {"reason", v.reason}});
  }
  if (op == "edit") {
    st::Prefixes prefixes;
    auto d = load(r["document"], &prefixes);
    for (const auto& [k, v] : prefixes_of(r["prefixes"]).map) prefixes.map[k] = v;
    ss::Change c;
    for (std::size_t k = 0; k < r["removed"].size(); k++) c.removed.push_back(quad_of(r["removed"][k]));
    for (std::size_t k = 0; k < r["added"].size(); k++) c.added.push_back(quad_of(r["added"][k]));
    const auto undo = d.apply(c);
    Json removed = Json::array(), added = Json::array();
    for (const auto& q : undo.removed) removed.push(quad_json(q));
    for (const auto& q : undo.added) added.push(quad_json(q));
    const std::string format = r["format"].text(r["document"]["format"].text("trig"));
    return Json::object({{"text", write(d.quads(), prefixes, format)}, {"undo", Json::object({{"removed", removed}, {"added", added}})}, {"statements", static_cast<double>(d.size())}});
  }
  throw std::invalid_argument("linked: no operation '" + op + "' (parse, write, summary, describe, query, validate, entail, compare, edit)");
}

}  // namespace science::linked
