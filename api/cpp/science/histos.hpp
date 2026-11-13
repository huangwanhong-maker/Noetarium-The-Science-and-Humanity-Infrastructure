// Historical objects for any work's engine (ADR 0110): one composition of the history domain's `objects` and
// `dependence`, with kategoria's verdict on a record, answering requests in JSON.
//
// A request names a record of historical objects by the path of its file ("path") or holds its lines
// ("text"); nothing else is read, and nothing is kept between requests. "layer" is the layer read:
// established (when not given), attested or proposed.
//
//   open      {path}                      the record whole: its kinds and their order, its types of relation, its
//                                         sources, every object (with its kind and the kinds above it, its names,
//                                         when it was as days and as years, where it was by its relations "at", and
//                                         how its being stands), every relation and emergence with its standing and
//                                         what is said of it, the faults found, and the statements counted by standing
//   object    {path, id, depth?}          one object inquired into: the four questions answered from the record with
//                                         its silences, its cones past and future, what it stands under and what
//                                         stands under it, its comparison cases, and what rests on it
//   frontier  {path, date}                the problems posed by a date (EDTF, the historians' convention), each with
//                                         what had answered it by then and what answered it later
//   gather    {path, classes: [ids]}      the dependences among classifying objects, derived from what stands under
//                                         each and counted, apart from what is asserted of them directly
//   verify    {path, schema}              kategoria's answer on whether the record is a functor on the category
//                                         Record of the schema's script (the path of historical-objects.cat)
//
// Years are decimal years in the historians' convention (Julian before 15 October 1582), astronomical before
// the era: year 0 is 1 BC.

#pragma once

#include "json.hpp"

#include <history/dependence/dependence.hpp>
#include <history/objects/record.hpp>
#include <history/objects/schema.hpp>
#include <kategoria/script/session.hpp>

#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>

namespace science::histos {

namespace ho = history::objects;
namespace hd = history::dependence;

inline ho::Record record_of(const Json& request) {
  if (request.has("text")) return ho::read_text(request["text"].text());
  if (request.has("path")) return ho::read_file(request["path"].text());
  throw std::invalid_argument("science::histos: a request names a record by \"path\" or holds its \"text\"");
}

inline ho::Layer layer_of(const Json& request) {
  const std::string name = request["layer"].text("established");
  const auto layer = ho::layer_of(name);
  if (!layer) throw std::invalid_argument("science::histos: the layer is established, attested or proposed, not " + name);
  return *layer;
}

inline Json when_json(const ho::When& w) {
  return Json::object({{"text", w.text}, {"first", static_cast<double>(w.first)}, {"last", static_cast<double>(w.last)},
                       {"from", history::historical_year(w.begin_first)}, {"to", history::historical_year(w.last)},
                       {"begins_by", history::historical_year(w.begin_last)}, {"approximate", w.approximate}, {"uncertain", w.uncertain}});
}

inline Json said_json(const std::vector<ho::Assertion>& said) {
  Json out = Json::array();
  for (const auto& a : said)
    out.push(Json::object({{"by", a.by}, {"grade", std::string(1, a.grade)}, {"disputes", a.disputes}, {"documentary", a.documentary},
                           {"quote", a.quote}, {"where", a.where}, {"confirmed", a.confirmed}}));
  return out;
}

inline std::string standing_name(ho::Standing s) { return std::string(ho::name_of(s)); }

inline Json ids_json(const ho::Record& R, const std::vector<std::size_t>& objects) {
  Json out = Json::array();
  for (std::size_t v : objects) out.push(R.objects[v].id);
  return out;
}

inline Json open(const ho::Record& R) {
  Json out = Json::object({{"id", R.id}, {"title", R.title}, {"about", R.about}, {"sources_to_establish", static_cast<double>(R.policy.sources)}});
  Json kinds = Json::array();
  for (const auto& k : R.kinds.all()) {
    Json above = Json::array();
    for (const auto& a : R.kinds.above(k.id)) above.push(a);
    Json under = Json::array();
    for (const auto& u : k.under) under.push(u);
    kinds.push(Json::object({{"id", k.id}, {"name", k.name}, {"plugin", k.plugin}, {"under", under}, {"above", above}}));
  }
  out.set("kinds", kinds);
  Json types = Json::array();
  for (const auto& t : R.types) {
    Json sig = Json::array();
    for (const auto& s : t.signatures) sig.push(Json::object({{"from", s.from}, {"to", s.to}}));
    types.push(Json::object({{"id", t.id}, {"name", t.name}, {"inverse", t.inverse}, {"role", t.role}, {"signatures", sig},
                             {"family", t.family == ho::Family::layer ? "layer" : t.family == ho::Family::dependence ? "dependence" : "association"}}));
  }
  out.set("types", types);
  Json sources = Json::array();
  for (const auto& s : R.sources) sources.push(Json::object({{"id", s.id}, {"cite", s.cite}, {"kind", s.kind}, {"object", s.object}, {"url", s.url}, {"language", s.language}}));
  out.set("sources", sources);

  std::map<std::string, double> count{{"established", 0}, {"attested", 0}, {"proposed", 0}, {"disputed", 0}};
  Json objects = Json::array();
  for (std::size_t v = 0; v < R.objects.size(); v++) {
    const auto& o = R.objects[v];
    const std::string standing = standing_name(R.standing(o.said));
    count[standing]++;
    Json row = Json::parse(o.more.dump());
    row.set("standing", standing);
    row.set("said", said_json(o.said));
    Json above = Json::array();
    for (const auto& a : R.kinds.above(o.kind)) above.push(a);
    row.set("above", above);
    if (o.when) row.set("when", when_json(*o.when));
    if (const auto appears = R.appears(v)) row.set("appears", history::historical_year(appears->first));
    Json places = Json::array();
    if (o.lat) places.push(Json::object({{"place", o.id}, {"lat", *o.lat}, {"lon", *o.lon}}));
    for (const auto& r : R.relations)
      if (r.from == v && r.type == "at" && R.objects[r.to].lat)
        places.push(Json::object({{"place", R.objects[r.to].id}, {"lat", *R.objects[r.to].lat}, {"lon", *R.objects[r.to].lon}, {"standing", standing_name(R.standing(r.said))}}));
    row.set("places", places);
    objects.push(row);
  }
  out.set("objects", objects);
  Json relations = Json::array();
  for (const auto& r : R.relations) {
    const std::string standing = standing_name(R.standing(r.said));
    count[standing]++;
    relations.push(Json::object({{"id", r.id}, {"type", r.type}, {"from", R.objects[r.from].id}, {"to", R.objects[r.to].id}, {"note", r.note},
                                 {"standing", standing}, {"said", said_json(r.said)}}));
  }
  out.set("relations", relations);
  Json emergences = Json::array();
  for (const auto& e : R.emergences) {
    Json premises = Json::array();
    for (const auto& p : e.premises) {
      const std::string standing = standing_name(R.standing(p.said.empty() ? e.said : p.said));
      if (p.relation < 0) count[standing]++;
      premises.push(Json::object({{"what", R.objects[p.what].id}, {"role", p.role}, {"note", p.note}, {"standing", standing},
                                  {"relation", p.relation < 0 ? Json() : Json(R.relations[static_cast<std::size_t>(p.relation)].id)}}));
    }
    Json row = Json::object({{"id", e.id}, {"product", R.objects[e.product].id}, {"note", e.note}, {"written", e.written}, {"premises", premises},
                             {"marked_by", ids_json(R, e.marked_by)}, {"standing", standing_name(R.standing(e.said))}, {"said", said_json(e.said)}});
    if (e.when) row.set("when", when_json(*e.when));
    emergences.push(row);
  }
  out.set("emergences", emergences);
  Json faults = Json::array();
  for (const auto& f : R.faults) faults.push(Json::object({{"line", static_cast<double>(f.line)}, {"row", f.row}, {"what", f.what}}));
  out.set("faults", faults);
  Json counts = Json::object();
  for (const auto& [name, n] : count) counts.set(name, n);
  out.set("statements", counts);
  return out;
}

inline Json cone_json(const ho::Record& R, const std::vector<hd::ConeEntry>& cone) {
  Json out = Json::array();
  for (const auto& e : cone)
    out.push(Json::object({{"object", R.objects[e.object].id}, {"steps", static_cast<double>(e.steps)}, {"role", e.role}, {"standing", standing_name(e.standing)},
                           {"inherited", e.inherited < 0 ? Json() : Json(R.objects[static_cast<std::size_t>(e.inherited)].id)}}));
  return out;
}

inline Json answers_json(const ho::Record& R, const std::vector<hd::Answer>& answers) {
  Json out = Json::array();
  for (const auto& a : answers)
    out.push(Json::object({{"object", R.objects[a.object].id}, {"role", a.role}, {"note", a.note}, {"standing", standing_name(a.standing)},
                           {"inherited", a.inherited < 0 ? Json() : Json(R.objects[static_cast<std::size_t>(a.inherited)].id)},
                           {"of", a.of < 0 ? Json() : Json(R.objects[static_cast<std::size_t>(a.of)].id)}}));
  return out;
}

inline Json object(const ho::Record& R, const Json& request) {
  const ho::Layer layer = layer_of(request);
  const long at = R.find(request["id"].text());
  if (at < 0) throw std::invalid_argument("science::histos: the record holds no object \"" + request["id"].text() + "\"");
  const auto x = static_cast<std::size_t>(at);
  const std::size_t depth = request.has("depth") ? static_cast<std::size_t>(request["depth"].as_number()) : 0;
  const hd::Web W = hd::web(R, layer);
  const hd::Inquiry Q = hd::inquiry(R, W, x);
  Json silent = Json::array();
  for (const auto& s : Q.silent) silent.push(s);
  Json lineage = Json::array();
  for (const auto& a : hd::lineage(R, x, layer)) lineage.push(Json::object({{"object", R.objects[a.object].id}, {"steps", static_cast<double>(a.steps)}, {"by", a.by}}));
  Json compare = Json::array();
  for (const auto& c : hd::comparisons(R, W, x))
    compare.push(Json::object({{"object", R.objects[c.object].id}, {"problems", ids_json(R, c.problems)}, {"independent", c.independent}}));
  Json links = Json::array();
  for (const auto& l : W.links)
    links.push(Json::object({{"what", R.objects[l.what].id}, {"product", R.objects[l.product].id}, {"role", l.role}, {"standing", standing_name(l.standing)}, {"note", l.note}}));
  return Json::object({{"id", R.objects[x].id}, {"layer", request["layer"].text("established")},
                       {"inquiry", Json::object({{"problems", answers_json(R, Q.problems)}, {"arose", answers_json(R, Q.arose)}, {"answerable", answers_json(R, Q.answerable)},
                                                 {"against", answers_json(R, Q.against)}, {"for_whom", answers_json(R, Q.for_whom)}, {"after", answers_json(R, Q.after)},
                                                 {"silent", silent}})},
                       {"past", cone_json(R, hd::cone(R, W, x, true, depth))}, {"future", cone_json(R, hd::cone(R, W, x, false, depth))},
                       {"lineage", lineage}, {"under", ids_json(R, hd::under(R, x, layer))}, {"comparisons", compare},
                       {"rests_on_it", ids_json(R, hd::support(W, x))}, {"links", links}});
}

inline Json frontier(const ho::Record& R, const Json& request) {
  const ho::When when = ho::parse_when(request["date"].text());
  const hd::Web W = hd::web(R, layer_of(request));
  Json out = Json::array();
  for (const auto& q : hd::frontier(R, W, when.last))
    out.push(Json::object({{"problem", R.objects[q.problem].id}, {"open", q.open()}, {"answered", ids_json(R, q.answered)}, {"later", ids_json(R, q.later)}}));
  return Json::object({{"date", when.text}, {"year", history::historical_year(when.last)}, {"problems", out}});
}

inline Json gather(const ho::Record& R, const Json& request) {
  std::vector<std::size_t> classes;
  for (const Json& c : request["classes"].as_array()) {
    const long at = R.find(c.text());
    if (at < 0) throw std::invalid_argument("science::histos: the record holds no object \"" + c.text() + "\"");
    classes.push_back(static_cast<std::size_t>(at));
  }
  const hd::Web W = hd::web(R, layer_of(request));
  const hd::Gathering G = hd::gather(R, W, classes);
  Json derived = Json::array(), asserted = Json::array(), inside = Json::array();
  for (const auto& g : G.derived) derived.push(Json::object({{"from", R.objects[classes[g.from]].id}, {"to", R.objects[classes[g.to]].id}, {"count", static_cast<double>(g.arcs)}}));
  for (const auto& l : G.asserted) asserted.push(Json::object({{"from", R.objects[l.what].id}, {"to", R.objects[l.product].id}, {"role", l.role}, {"standing", standing_name(l.standing)}}));
  for (std::size_t i = 0; i < classes.size(); i++) inside.push(Json::object({{"class", R.objects[classes[i]].id}, {"count", static_cast<double>(G.inside[i])}}));
  return Json::object({{"derived", derived}, {"asserted", asserted}, {"inside", inside}, {"unplaced", static_cast<double>(G.unplaced)}});
}

inline Json verify(const ho::Record& R, const Json& request) {
  std::ifstream in(request["schema"].text());
  if (!in) throw std::invalid_argument("science::histos: the schema's script is not read: " + request["schema"].text());
  std::stringstream text;
  text << in.rdbuf();
  const std::string script = text.str();
  const auto start = script.find("category Record {");
  const auto end = script.find("\n}", start);
  if (start == std::string::npos || end == std::string::npos) throw std::invalid_argument("science::histos: the schema's script holds no category Record");
  kategoria::script::Session session;
  session.most_arrows = 100000;
  const auto outcomes = session.run(script.substr(start, end + 2 - start) + "\n" + ho::as_sets(R, "TheRecord"));
  const auto& last = outcomes.items().back();
  const auto field = [&](const char* key) { return last.get(key) ? last.get(key)->text() : std::string(); };
  return Json::object({{"status", field("status")}, {"claim", field("claim")}, {"reason", field("reason")},
                       {"objects", static_cast<double>(R.objects.size())}, {"relations", static_cast<double>(R.relations.size())},
                       {"emergences", static_cast<double>(R.emergences.size())}, {"faults", static_cast<double>(R.faults.size())}});
}

// The answer to one request.
inline Json answer(const Json& request) {
  const std::string op = request["op"].text();
  const ho::Record R = record_of(request);
  if (op == "open") return open(R);
  if (op == "object") return object(R, request);
  if (op == "frontier") return frontier(R, request);
  if (op == "gather") return gather(R, request);
  if (op == "verify") return verify(R, request);
  throw std::invalid_argument("science::histos: no operation named " + op + " (open, object, frontier, gather, verify)");
}

}  // namespace science::histos
