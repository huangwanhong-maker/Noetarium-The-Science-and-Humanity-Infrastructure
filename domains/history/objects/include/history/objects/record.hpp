// Historical objects and the record that holds them (the codebook of objects, codebook-objects.md).
//
// A historical object is anything in history that can be inquired into: an event, a period, a person, a
// polity, a work, a theory, a kind of music, a problem. Every object has one kind, and the kinds are ordered
// by "is a kind of" beneath one root, the historical object itself (the schema, schema/historical-objects.cat,
// states the order as a thin category with a terminal object, and kategoria verifies it).
//
// A record holds assertions, never facts (the domain's first principle). It has four sorts of row besides its
// own: sources; objects; relations between two objects, each of a type that says between which kinds it may
// hold; and emergences, each the appearing of one object with its premises, every premise in a role
// (condition, problem, source, against). A relation of a dependence type is a premise of its object's own
// emergence, written short. Whatever is said carries who says it, with a grade:
//
//   A   a structured source records it (an authority file, a catalogue);
//   B   a named text states it, and the passage is quoted;
//   C   a language model proposes it, from memory.
//
// and the standing of a statement follows from its assertions by a stated rule (standing_of): established,
// attested, proposed or disputed. A reader of the record chooses the layer read; the default is the
// established alone.
//
// Reading goes on past a fault: every fault is reported with its line, the row it is in and what is wrong,
// and the rows that stand are kept. What is checked: ids, kinds and their order, the typing of every relation
// by the kinds of its ends, the layering relations and the dependence each being acyclic (the cycle is
// named), every premise having begun before what it is a premise of has appeared, dates and places.
//
// Dates are EDTF in the historians' convention (Julian before 15 October 1582) unless a row names a calendar
// (chronology.hpp); a life or a span is an interval, and an object appears at its beginning.

#pragma once

#include <history/record/chronology.hpp>
#include <history/record/json.hpp>

#include <morphe/combinatorics/acyclic.hpp>

#include <algorithm>
#include <cstddef>
#include <fstream>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace history::objects {

inline constexpr std::string_view FORMAT = "noetarium.histos/1";

// ---------------------------------------------------------------------------------------------------------
// Kinds

struct Kind {
  std::string id, name, plugin;       // plugin: what brings it ("" the core)
  std::vector<std::string> under;     // the kinds it is a kind of, directly
};

// The order of kinds: reflexive, transitive, every kind beneath the root. Acyclic by construction, since a
// kind names only kinds that are already there.
class Kinds {
 public:
  static constexpr std::string_view ROOT = "Object";

  // The core's twenty kinds, as the schema states them.
  static Kinds core() {
    Kinds k;
    k.kinds_.push_back({std::string(ROOT), "historical object", "", {}});
    k.at_[std::string(ROOT)] = 0;
    const char* rows[][3] = {
        {"Event", "event", "Object"},          {"Period", "period", "Object"},          {"Place", "place", "Object"},
        {"Agent", "agent", "Object"},          {"Person", "person", "Agent"},           {"Collective", "collective", "Agent"},
        {"Institution", "institution", "Collective"}, {"Polity", "polity", "Collective"}, {"Work", "work", "Object"},
        {"Artifact", "artifact", "Object"},    {"Idea", "idea", "Object"},              {"Concept", "concept", "Idea"},
        {"Theory", "theory", "Idea"},          {"Problem", "problem", "Idea"},          {"Tradition", "tradition", "Object"},
        {"Genre", "genre", "Tradition"},       {"Technology", "technology", "Object"},  {"Practice", "practice", "Object"}};
    for (const auto& r : rows) k.add({r[0], r[1], "", {r[2]}});
    k.add({"School", "school", "", {"Tradition", "Collective"}});
    return k;
  }

  bool has(std::string_view id) const { return at_.find(id) != at_.end(); }
  const Kind& get(std::string_view id) const {
    const auto it = at_.find(id);
    if (it == at_.end()) throw std::invalid_argument("no kind \"" + std::string(id) + "\"");
    return kinds_[it->second];
  }
  const std::vector<Kind>& all() const { return kinds_; }

  // Adds a kind beneath kinds already there.
  void add(Kind k) {
    if (k.id.empty()) throw std::invalid_argument("a kind needs an id");
    if (has(k.id)) throw std::invalid_argument("the kind \"" + k.id + "\" is there already");
    if (k.under.empty()) throw std::invalid_argument("the kind \"" + k.id + "\" is a kind of nothing: every kind lies beneath " + std::string(ROOT));
    for (const auto& u : k.under)
      if (!has(u)) throw std::invalid_argument("the kind \"" + k.id + "\" is put under \"" + u + "\", which is no kind");
    at_[k.id] = kinds_.size();
    kinds_.push_back(std::move(k));
  }

  // The kinds above one, nearest first (by the number of steps), itself left out.
  std::vector<std::string> above(std::string_view id) const {
    std::vector<std::string> out, level{std::string(get(id).id)};
    std::set<std::string> seen{level[0]};
    while (!level.empty()) {
      std::vector<std::string> next;
      for (const auto& k : level)
        for (const auto& u : get(k).under)
          if (seen.insert(u).second) { next.push_back(u); out.push_back(u); }
      level = std::move(next);
    }
    return out;
  }

  // Whether a is a kind of b (a ≤ b); every kind is a kind of itself.
  bool is_a(std::string_view a, std::string_view b) const {
    if (!has(a) || !has(b)) return false;
    if (a == b) return true;
    const auto up = above(a);
    return std::find(up.begin(), up.end(), b) != up.end();
  }

 private:
  std::vector<Kind> kinds_;
  std::map<std::string, std::size_t, std::less<>> at_;
};

// ---------------------------------------------------------------------------------------------------------
// Types of relation

enum class Family { layer, dependence, association };

struct Signature {
  std::string from, to;  // kinds: the relation may hold from an object of a kind of `from` to one of a kind of `to`
};

struct RelationType {
  std::string id, name, inverse;      // "specialises", read forward and back ("is specialised by")
  Family family = Family::association;
  std::string role;                   // of a dependence: the role its target takes as a premise
  std::vector<Signature> signatures;
  bool comparable = false;            // the two ends' kinds must be comparable (one a kind of the other)
  bool acyclic = false;               // the relation must have no cycle
  std::string plugin;
};

inline constexpr std::string_view ROLES[] = {"condition", "problem", "source", "against"};
inline bool is_role(std::string_view r) { return std::find(std::begin(ROLES), std::end(ROLES), r) != std::end(ROLES); }

// The core's types. Layers: specialises, exemplifies, part of, within. Dependence, one for each role: enabled
// by (condition), answers (problem), draws on and taught by (source), against. Associations: made by, member
// of, at, held by, sets out.
inline std::vector<RelationType> core_types() {
  const auto T = [](const char* id, const char* name, const char* inverse, Family f, const char* role, std::vector<Signature> sig, bool comparable, bool acyclic) {
    return RelationType{id, name, inverse, f, role, std::move(sig), comparable, acyclic, ""};
  };
  return {
      T("specialises", "specialises", "is specialised by", Family::layer, "", {{"Object", "Object"}}, true, true),
      T("exemplifies", "exemplifies", "is exemplified by", Family::layer, "", {{"Object", "Tradition"}, {"Object", "Idea"}, {"Object", "Technology"}, {"Object", "Practice"}}, false, true),
      T("part_of", "is part of", "has as a part", Family::layer, "", {{"Object", "Object"}}, true, true),
      T("within", "falls within", "holds", Family::layer, "", {{"Object", "Period"}}, false, true),
      T("enabled_by", "was enabled by", "enabled", Family::dependence, "condition", {{"Object", "Object"}}, false, false),
      T("answers", "answers", "is answered by", Family::dependence, "problem", {{"Object", "Problem"}}, false, false),
      T("draws_on", "draws on", "is drawn on by", Family::dependence, "source", {{"Object", "Object"}}, false, false),
      T("taught_by", "was taught by", "taught", Family::dependence, "source", {{"Person", "Person"}}, false, false),
      T("against", "argues against", "is argued against by", Family::dependence, "against", {{"Object", "Object"}}, false, false),
      T("made_by", "was made by", "made", Family::association, "", {{"Work", "Agent"}, {"Artifact", "Agent"}, {"Idea", "Agent"}, {"Technology", "Agent"}}, false, false),
      T("member_of", "belongs to", "has as a member", Family::association, "", {{"Agent", "Collective"}, {"Agent", "Tradition"}}, false, false),
      T("at", "was at", "was the place of", Family::association, "", {{"Object", "Place"}}, false, false),
      T("held_by", "is held by", "holds", Family::association, "", {{"Artifact", "Institution"}, {"Work", "Institution"}}, false, false),
      T("sets_out", "sets out", "is set out in", Family::association, "", {{"Work", "Idea"}, {"Artifact", "Idea"}}, false, false),
  };
}

// ---------------------------------------------------------------------------------------------------------
// What is said, and by whom

struct Source {
  std::string id, cite, kind;   // kind: primary, reference, structured or model
  std::string object;           // the object this source also is, when it is one ("" otherwise)
  std::string url, language;
  std::size_t line = 0;
};

struct Assertion {
  std::string by;               // a source
  char grade = 'C';             // A, B or C
  bool disputes = false;        // the source denies the statement
  bool documentary = false;     // the source is the thing itself speaking (an author's own sentence, a dated manuscript)
  std::string quote, where;     // the passage and its place in the source
  std::string confirmed;        // who confirmed it against the source, and when ("" not yet)
};

enum class Standing { disputed, proposed, attested, established };
inline std::string_view name_of(Standing s) {
  return s == Standing::established ? "established" : s == Standing::attested ? "attested" : s == Standing::proposed ? "proposed" : "disputed";
}

// The rule by which a statement stands, from its assertions.
//   disputed      a source that is not a model denies it;
//   established   a confirmed documentary assertion of grade A or B, or confirmed assertions of grade A or B
//                 by at least `sources` different sources;
//   attested      some assertion of grade A or B, short of that;
//   proposed      nothing but a model's word.
struct Policy {
  std::size_t sources = 2;
};
inline Standing standing_of(const std::vector<Assertion>& said, const Policy& policy = {}) {
  std::set<std::string> confirmed;
  bool documentary = false, some = false;
  for (const auto& a : said) {
    if (a.grade == 'C') continue;
    if (a.disputes) return Standing::disputed;
    some = true;
    if (a.confirmed.empty()) continue;
    confirmed.insert(a.by);
    documentary = documentary || a.documentary;
  }
  if (documentary || confirmed.size() >= policy.sources) return Standing::established;
  return some ? Standing::attested : Standing::proposed;
}

// The layer a reader takes: the established alone, with the attested, or with the proposed too. What is
// disputed is in no layer.
enum class Layer { established, attested, proposed };
inline bool in_layer(Standing s, Layer l) {
  if (s == Standing::disputed) return false;
  return l == Layer::proposed || (l == Layer::attested && s != Standing::proposed) || s == Standing::established;
}
inline std::optional<Layer> layer_of(std::string_view name) {
  if (name == "established") return Layer::established;
  if (name == "attested") return Layer::attested;
  if (name == "proposed") return Layer::proposed;
  return std::nullopt;
}

// ---------------------------------------------------------------------------------------------------------
// Objects, relations, emergences

// When something was: the days its date may denote, and those of its beginning (the whole, when it is no interval).
struct When {
  std::string text, calendar;
  long first = 0, last = 0;              // JDN, inclusive
  long begin_first = 0, begin_last = 0;  // of its beginning
  bool approximate = false, uncertain = false;
};

inline When parse_when(std::string_view text, std::string_view calendar = "") {
  const auto read = [&](std::string_view t) {
    if (calendar.empty() || calendar == "historical") return parse_historical(t);
    const auto c = calendar_of(calendar);
    if (!c) throw std::invalid_argument("no calendar \"" + std::string(calendar) + "\"");
    return parse_date(t, *c);
  };
  const Date whole = read(text);
  When w{std::string(text), std::string(calendar), whole.first, whole.last, whole.first, whole.last, whole.approximate, whole.uncertain};
  const auto slash = text.find('/');
  if (slash != std::string_view::npos && slash > 0 && text.substr(0, slash) != "..") {
    const Date begin = read(text.substr(0, slash));
    w.begin_first = begin.first;
    w.begin_last = begin.last;
  }
  return w;
}

struct Object {
  std::string id, kind, name, summary;
  std::string tradition, discipline;           // where it belongs: a tradition (greek, chinese, …) and a discipline (music, philosophy, …)
  std::map<std::string, std::string> names;   // by language: the name in its own script, and others
  std::optional<When> when;
  std::optional<double> lat, lon;              // of a place, degrees on WGS 84
  std::vector<Assertion> said;                 // of the object itself: that it was, and when
  Json more;                                   // the row as written, for what a discipline adds
  std::size_t line = 0;
};

struct Relation {
  std::string id, type, note;
  std::size_t from = 0, to = 0;
  std::optional<When> when;
  std::vector<Assertion> said;
  std::size_t line = 0;
};

struct Premise {
  std::size_t what = 0;
  std::string role, note;
  std::vector<Assertion> said;
  long relation = -1;                          // the relation it was written as, if it was
};

struct Emergence {
  std::string id, note;
  std::size_t product = 0;
  std::vector<std::size_t> marked_by;          // events that mark it: a publication, a first performance
  std::optional<When> when;
  std::vector<Premise> premises;
  std::vector<Assertion> said;
  bool written = false;                        // a row of its own (else gathered from relations)
  std::size_t line = 0;
};

struct Fault {
  std::size_t line = 0;
  std::string row, what;
};

struct Record {
  std::string id, title, about;
  Kinds kinds = Kinds::core();
  std::vector<RelationType> types = core_types();
  std::vector<Source> sources;
  std::vector<Object> objects;
  std::vector<Relation> relations;
  std::vector<Emergence> emergences;           // written ones first, then one gathered for each object that has dependences
  std::vector<Fault> faults;
  Policy policy;

  std::map<std::string, std::size_t, std::less<>> object_at, source_at, type_at;

  long find(std::string_view id) const {
    const auto it = object_at.find(id);
    return it == object_at.end() ? -1 : static_cast<long>(it->second);
  }
  const RelationType* type(std::string_view id) const {
    const auto it = type_at.find(id);
    return it == type_at.end() ? nullptr : &types[it->second];
  }
  bool is_a(std::size_t object, std::string_view kind) const { return kinds.is_a(objects[object].kind, kind); }
  Standing standing(const std::vector<Assertion>& said) const { return standing_of(said, policy); }
  // When an object appears: its emergence's date when one is written with a date, else its own beginning.
  std::optional<std::pair<long, long>> appears(std::size_t object) const {
    for (const auto& e : emergences)
      if (e.product == object && e.when) return std::pair{e.when->begin_first, e.when->begin_last};
    if (objects[object].when) return std::pair{objects[object].when->begin_first, objects[object].when->begin_last};
    return std::nullopt;
  }
};

namespace detail {

inline std::string text_of(const Json& j, std::string_view key) {
  const Json* v = j.find(key);
  return v && v->is_string() ? v->as_string() : std::string();
}

inline std::vector<Assertion> said_of(const Json& row, Record& R, std::size_t line, const std::string& what) {
  std::vector<Assertion> out;
  const Json* said = row.find("said");
  if (!said) return out;
  if (!said->is_array()) { R.faults.push_back({line, what, "\"said\" is a list of assertions"}); return out; }
  for (const Json& s : said->as_array()) {
    Assertion a;
    a.by = text_of(s, "by");
    const std::string g = text_of(s, "grade");
    a.quote = text_of(s, "quote");
    a.where = text_of(s, "where");
    a.confirmed = text_of(s, "confirmed");
    if (const Json* d = s.find("disputes")) a.disputes = d->is_bool() && d->as_bool();
    if (const Json* d = s.find("documentary")) a.documentary = d->is_bool() && d->as_bool();
    const auto src = R.source_at.find(a.by);
    if (src == R.source_at.end()) { R.faults.push_back({line, what, "an assertion by \"" + a.by + "\", which is no source of the record"}); continue; }
    if (g != "A" && g != "B" && g != "C") { R.faults.push_back({line, what, "an assertion's grade is A, B or C, not \"" + g + "\""}); continue; }
    a.grade = g[0];
    const bool model = R.sources[src->second].kind == "model";
    if (model != (a.grade == 'C')) {
      R.faults.push_back({line, what, model ? "a model's word is of grade C, whatever it is given" : "grade C is a model's word, and \"" + a.by + "\" is no model"});
      continue;
    }
    if (a.grade == 'B' && a.quote.empty()) { R.faults.push_back({line, what, "an assertion of grade B quotes the passage that states it"}); continue; }
    out.push_back(std::move(a));
  }
  return out;
}

inline std::optional<When> when_of(const Json& row, Record& R, std::size_t line, const std::string& what) {
  const std::string date = text_of(row, "date");
  if (date.empty()) return std::nullopt;
  try {
    return parse_when(date, text_of(row, "calendar"));
  } catch (const std::exception& e) {
    R.faults.push_back({line, what, std::string("its date is not read: ") + e.what()});
    return std::nullopt;
  }
}

inline std::string path_text(const Record& R, const std::vector<std::size_t>& cycle) {
  std::string s;
  for (std::size_t v : cycle) s += R.objects[v].id + " → ";
  return s + R.objects[cycle.front()].id;
}

}  // namespace detail

// Whether an object of kind `from` may stand in a relation of this type to one of kind `to`; the reason when not.
inline std::optional<std::string> misfit(const Kinds& kinds, const RelationType& type, std::string_view from, std::string_view to) {
  bool fits = false;
  for (const auto& s : type.signatures) fits = fits || (kinds.is_a(from, s.from) && kinds.is_a(to, s.to));
  if (!fits) {
    std::string allowed;
    for (const auto& s : type.signatures) allowed += (allowed.empty() ? "" : ", or ") + s.from + " to " + s.to;
    return "\"" + type.id + "\" holds from " + allowed + ", and not from " + std::string(from) + " to " + std::string(to);
  }
  if (type.comparable && !kinds.is_a(from, to) && !kinds.is_a(to, from))
    return "\"" + type.id + "\" holds between kinds of which one is a kind of the other, and " + std::string(from) + " and " + std::string(to) + " are not";
  return std::nullopt;
}

// A record read from its lines (JSON lines; empty lines and lines beginning with # are passed over).
inline Record read(std::istream& in) {
  Record R;
  for (std::size_t i = 0; i < R.types.size(); i++) R.type_at[R.types[i].id] = i;
  struct Pending { Json row; std::size_t line; };
  std::vector<Pending> relations, emergences;
  std::string text;
  std::size_t line = 0;
  bool headed = false;
  while (std::getline(in, text)) {
    line++;
    const auto start = text.find_first_not_of(" \t\r");
    if (start == std::string::npos || text[start] == '#') continue;
    Json row;
    try {
      row = Json::parse(text);
    } catch (const std::exception& e) {
      R.faults.push_back({line, "", std::string("not JSON: ") + e.what()});
      continue;
    }
    const std::string kind = detail::text_of(row, "row"), id = detail::text_of(row, "id");
    const std::string what = kind + (id.empty() ? "" : " " + id);
    if (kind == "record") {
      headed = true;
      R.id = id;
      R.title = detail::text_of(row, "title");
      R.about = detail::text_of(row, "about");
      if (detail::text_of(row, "format") != FORMAT) R.faults.push_back({line, what, "its format is not " + std::string(FORMAT)});
      if (const Json* p = row.find("sources"); p && p->is_number() && p->as_number() >= 1) R.policy.sources = static_cast<std::size_t>(p->as_number());
    } else if (kind == "kind") {
      Kind k{id, detail::text_of(row, "name"), detail::text_of(row, "plugin"), {}};
      if (const Json* u = row.find("under"); u && u->is_array())
        for (const Json& x : u->as_array())
          if (x.is_string()) k.under.push_back(x.as_string());
      try {
        R.kinds.add(std::move(k));
      } catch (const std::exception& e) {
        R.faults.push_back({line, what, e.what()});
      }
    } else if (kind == "relation-type") {
      RelationType t;
      t.id = id;
      t.name = detail::text_of(row, "name");
      t.inverse = detail::text_of(row, "inverse");
      t.role = detail::text_of(row, "role");
      t.plugin = detail::text_of(row, "plugin");
      const std::string family = detail::text_of(row, "family");
      t.family = family == "layer" ? Family::layer : family == "dependence" ? Family::dependence : Family::association;
      if (const Json* a = row.find("acyclic")) t.acyclic = a->is_bool() && a->as_bool();
      if (const Json* a = row.find("comparable")) t.comparable = a->is_bool() && a->as_bool();
      bool good = !id.empty() && !R.type(id);
      if (!good) R.faults.push_back({line, what, id.empty() ? "a type of relation needs an id" : "the type is there already"});
      if (t.family == Family::dependence && !is_role(t.role)) { R.faults.push_back({line, what, "a dependence names the role of its target: condition, problem, source or against"}); good = false; }
      if (const Json* s = row.find("signatures"); s && s->is_array())
        for (const Json& x : s->as_array()) {
          Signature sig{detail::text_of(x, "from"), detail::text_of(x, "to")};
          if (!R.kinds.has(sig.from) || !R.kinds.has(sig.to)) { R.faults.push_back({line, what, "a signature names \"" + sig.from + "\" and \"" + sig.to + "\", of which one is no kind"}); good = false; }
          else t.signatures.push_back(std::move(sig));
        }
      if (t.signatures.empty()) { R.faults.push_back({line, what, "a type of relation says between which kinds it holds"}); good = false; }
      if (good) { R.type_at[t.id] = R.types.size(); R.types.push_back(std::move(t)); }
    } else if (kind == "source") {
      Source s{id, detail::text_of(row, "cite"), detail::text_of(row, "kind"), detail::text_of(row, "object"), detail::text_of(row, "url"), detail::text_of(row, "language"), line};
      if (id.empty() || R.source_at.count(id)) { R.faults.push_back({line, what, id.empty() ? "a source needs an id" : "the source is there already"}); continue; }
      if (s.kind != "primary" && s.kind != "reference" && s.kind != "structured" && s.kind != "model") { R.faults.push_back({line, what, "a source is primary, a reference, structured or a model"}); continue; }
      if (s.cite.empty()) { R.faults.push_back({line, what, "a source is cited in words"}); continue; }
      R.source_at[id] = R.sources.size();
      R.sources.push_back(std::move(s));
    } else if (kind == "object") {
      Object o;
      o.id = id;
      o.kind = detail::text_of(row, "kind");
      o.name = detail::text_of(row, "name");
      o.summary = detail::text_of(row, "summary");
      o.tradition = detail::text_of(row, "tradition");
      o.discipline = detail::text_of(row, "discipline");
      o.line = line;
      if (id.empty() || R.object_at.count(id)) { R.faults.push_back({line, what, id.empty() ? "an object needs an id" : "the object is there already"}); continue; }
      if (!R.kinds.has(o.kind)) { R.faults.push_back({line, what, "\"" + o.kind + "\" is no kind"}); continue; }
      if (o.name.empty()) { R.faults.push_back({line, what, "an object has a name"}); continue; }
      if (const Json* n = row.find("names"); n && n->is_object())
        for (const auto& [lang, v] : n->as_object())
          if (v.is_string()) o.names[lang] = v.as_string();
      o.when = detail::when_of(row, R, line, what);
      const Json* lat = row.find("lat");
      const Json* lon = row.find("lon");
      if (lat && lon && lat->is_number() && lon->is_number()) {
        if (lat->as_number() < -90 || lat->as_number() > 90 || lon->as_number() < -180 || lon->as_number() > 180) R.faults.push_back({line, what, "its latitude and longitude are degrees on the sphere"});
        else { o.lat = lat->as_number(); o.lon = lon->as_number(); }
      }
      if (R.kinds.is_a(o.kind, "Place") && !o.lat) R.faults.push_back({line, what, "a place has a latitude and a longitude"});
      o.said = detail::said_of(row, R, line, what);
      o.more = row;
      R.object_at[id] = R.objects.size();
      R.objects.push_back(std::move(o));
    } else if (kind == "relation") {
      relations.push_back({std::move(row), line});
    } else if (kind == "emergence") {
      emergences.push_back({std::move(row), line});
    } else {
      R.faults.push_back({line, what, "a row is a record, a kind, a relation-type, a source, an object, a relation or an emergence"});
    }
  }
  if (!headed) R.faults.push_back({0, "", "the record has no row of its own (\"row\": \"record\")"});
  for (const auto& s : R.sources)
    if (!s.object.empty() && R.find(s.object) < 0) R.faults.push_back({s.line, "source " + s.id, "it is said to be the object \"" + s.object + "\", which the record does not hold"});

  // Relations and emergences are resolved once every object is known, so that rows may come in any order.
  std::set<std::string> ids;
  for (const auto& p : relations) {
    const Json& row = p.row;
    Relation r;
    r.id = detail::text_of(row, "id");
    r.type = detail::text_of(row, "type");
    r.note = detail::text_of(row, "note");
    r.line = p.line;
    const std::string what = "relation " + r.id, from = detail::text_of(row, "from"), to = detail::text_of(row, "to");
    if (r.id.empty() || !ids.insert(r.id).second) { R.faults.push_back({p.line, what, r.id.empty() ? "a relation needs an id" : "the relation is there already"}); continue; }
    const RelationType* type = R.type(r.type);
    if (!type) { R.faults.push_back({p.line, what, "\"" + r.type + "\" is no type of relation"}); continue; }
    const long a = R.find(from), b = R.find(to);
    if (a < 0 || b < 0) { R.faults.push_back({p.line, what, "it joins \"" + from + "\" and \"" + to + "\", of which " + (a < 0 ? "the first" : "the second") + " is no object of the record"}); continue; }
    if (a == b) { R.faults.push_back({p.line, what, "it joins an object to itself"}); continue; }
    if (const auto wrong = misfit(R.kinds, *type, R.objects[static_cast<std::size_t>(a)].kind, R.objects[static_cast<std::size_t>(b)].kind)) { R.faults.push_back({p.line, what, *wrong}); continue; }
    r.from = static_cast<std::size_t>(a);
    r.to = static_cast<std::size_t>(b);
    r.when = detail::when_of(row, R, p.line, what);
    r.said = detail::said_of(row, R, p.line, what);
    R.relations.push_back(std::move(r));
  }
  for (const auto& p : emergences) {
    const Json& row = p.row;
    Emergence e;
    e.id = detail::text_of(row, "id");
    e.note = detail::text_of(row, "note");
    e.written = true;
    e.line = p.line;
    const std::string what = "emergence " + e.id, product = detail::text_of(row, "product");
    if (e.id.empty() || !ids.insert(e.id).second) { R.faults.push_back({p.line, what, e.id.empty() ? "an emergence needs an id" : "the id is taken"}); continue; }
    const long x = R.find(product);
    if (x < 0) { R.faults.push_back({p.line, what, "what appears, \"" + product + "\", is no object of the record"}); continue; }
    e.product = static_cast<std::size_t>(x);
    if (const Json* m = row.find("marked_by"); m && m->is_array())
      for (const Json& v : m->as_array()) {
        const long ev = v.is_string() ? R.find(v.as_string()) : -1;
        if (ev < 0 || !R.is_a(static_cast<std::size_t>(ev), "Event")) R.faults.push_back({p.line, what, "what marks an emergence is an event of the record"});
        else e.marked_by.push_back(static_cast<std::size_t>(ev));
      }
    if (const Json* ps = row.find("premises"); ps && ps->is_array())
      for (const Json& q : ps->as_array()) {
        Premise pr;
        pr.role = detail::text_of(q, "role");
        pr.note = detail::text_of(q, "note");
        const std::string w = detail::text_of(q, "what");
        const long y = R.find(w);
        if (y < 0) { R.faults.push_back({p.line, what, "a premise, \"" + w + "\", is no object of the record"}); continue; }
        if (y == x) { R.faults.push_back({p.line, what, "an object is no premise of its own appearing"}); continue; }
        if (!is_role(pr.role)) { R.faults.push_back({p.line, what, "a premise's role is condition, problem, source or against, not \"" + pr.role + "\""}); continue; }
        if (pr.role == "problem" && !R.is_a(static_cast<std::size_t>(y), "Problem")) { R.faults.push_back({p.line, what, "what stands in the role of the problem, \"" + w + "\", is no problem"}); continue; }
        pr.what = static_cast<std::size_t>(y);
        pr.said = detail::said_of(q, R, p.line, what);
        e.premises.push_back(std::move(pr));
      }
    e.when = detail::when_of(row, R, p.line, what);
    e.said = detail::said_of(row, R, p.line, what);
    R.emergences.push_back(std::move(e));
  }
  // A relation of dependence is a premise of its object's own emergence: gathered, one emergence an object.
  std::map<std::size_t, std::size_t> own;
  for (std::size_t k = 0; k < R.relations.size(); k++) {
    const Relation& r = R.relations[k];
    const RelationType& t = *R.type(r.type);
    if (t.family != Family::dependence) continue;
    auto it = own.find(r.from);
    if (it == own.end()) {
      Emergence e;
      e.id = "of:" + R.objects[r.from].id;
      e.product = r.from;
      e.line = r.line;
      it = own.emplace(r.from, R.emergences.size()).first;
      R.emergences.push_back(std::move(e));
    }
    R.emergences[it->second].premises.push_back({r.to, t.role, r.note, r.said, static_cast<long>(k)});
  }

  // The layering relations and the dependence are each without a cycle.
  const std::size_t n = R.objects.size();
  for (const auto& t : R.types) {
    if (!t.acyclic) continue;
    std::vector<morphe::combinatorics::Arc> arcs;
    for (const auto& r : R.relations)
      if (r.type == t.id) arcs.push_back({r.from, r.to});
    const auto o = morphe::combinatorics::topological_order(n, arcs);
    if (!o.acyclic) R.faults.push_back({0, "relations of type " + t.id, "they run in a circle: " + detail::path_text(R, o.cycle)});
  }
  {
    std::vector<morphe::combinatorics::Arc> arcs;
    for (const auto& e : R.emergences)
      for (const auto& p : e.premises) arcs.push_back({p.what, e.product});
    const auto o = morphe::combinatorics::topological_order(n, arcs);
    if (!o.acyclic) R.faults.push_back({0, "dependence", "it runs in a circle: " + detail::path_text(R, o.cycle)});
  }
  // A premise has begun before what it is a premise of has appeared.
  for (const auto& e : R.emergences) {
    const auto appears = e.when ? std::optional{std::pair{e.when->begin_first, e.when->begin_last}} : R.appears(e.product);
    if (!appears) continue;
    for (const auto& p : e.premises) {
      const auto began = R.appears(p.what);
      if (began && began->first > appears->second)
        R.faults.push_back({e.line, "emergence " + e.id, "an anachronism: \"" + R.objects[p.what].id + "\" begins after \"" + R.objects[e.product].id + "\" has appeared"});
    }
  }
  return R;
}

inline Record read_text(std::string_view text) {
  std::istringstream in{std::string(text)};
  return read(in);
}

inline Record read_file(const std::string& path) {
  std::ifstream in(path);
  if (!in) throw std::invalid_argument("the record's file is not read: " + path);
  return read(in);
}

}  // namespace history::objects
