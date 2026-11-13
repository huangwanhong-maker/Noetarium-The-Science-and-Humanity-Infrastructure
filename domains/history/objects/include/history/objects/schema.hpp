// A record written as a set-valued functor on the schema of records, for kategoria to verify.
//
// The schema (schema/historical-objects.cat, its category Record) has a table for each sort of row and an
// arrow for each reference; a record is then an assignment of a set to each table and a function to each
// arrow, and it is well formed exactly when the assignment is a functor: every reference lands, and the
// equations of typing hold (a relation's ends have kinds that are kinds of what its type asks, with the
// witness in the table Subkind). as_sets writes a record so, in kategoria's language of scripts. The reader
// (record.hpp) checks the same things by its own code; kategoria's answer is the check that shares none of it.

#pragma once

#include <history/objects/record.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace history::objects {

// The statement `sets <name> : Record { … }` of a record, at the layer of everything it holds.
inline std::string as_sets(const Record& R, std::string_view name) {
  const auto q = [](const std::string& s) {
    std::string out = "\"";
    for (char c : s) out += (c == '"' || c == '\n') ? '\'' : c;
    return out + "\"";
  };
  std::map<std::string, std::vector<std::string>> carrier;
  std::map<std::string, std::vector<std::pair<std::string, std::string>>> action;
  const char* tables[] = {"Kind", "Subkind", "Object", "RelationType", "Relation", "Emergence", "Marking", "Premise", "Role", "Statement", "Assertion", "Source", "Grade"};
  for (const char* t : tables) carrier[t];
  for (const auto& k : R.kinds.all()) carrier["Kind"].push_back(k.id);
  for (const auto r : ROLES) carrier["Role"].push_back(std::string(r));
  for (const char* g : {"A", "B", "C"}) carrier["Grade"].push_back(g);
  for (const auto& s : R.sources) carrier["Source"].push_back(s.id);

  // The table Subkind holds the order of kinds itself, pair by pair: a witness is one of its pairs. A relation
  // whose end is of no kind asked for has no witness; it is written with the pair of its own kind with itself,
  // so that the functor fails at the equation of typing and nowhere else.
  std::set<std::string> reltypes;
  for (const auto& a : R.kinds.all())
    for (const auto& b : R.kinds.all())
      if (R.kinds.is_a(a.id, b.id)) {
        const std::string w = a.id + " is " + b.id;
        carrier["Subkind"].push_back(w);
        action["lower"].push_back({w, a.id});
        action["upper"].push_back({w, b.id});
      }
  const auto witness = [&](const std::string& lower, const std::string& upper) { return lower + " is " + (R.kinds.is_a(lower, upper) ? upper : lower); };
  const auto say = [&](const std::string& statement, const std::vector<Assertion>& said) {
    carrier["Statement"].push_back(statement);
    for (std::size_t i = 0; i < said.size(); i++) {
      const std::string a = statement + " @" + std::to_string(i + 1);
      carrier["Assertion"].push_back(a);
      action["about"].push_back({a, statement});
      action["by"].push_back({a, said[i].by});
      action["grade"].push_back({a, std::string(1, said[i].grade)});
    }
  };
  for (const auto& o : R.objects) {
    carrier["Object"].push_back(o.id);
    action["kind"].push_back({o.id, o.kind});
    action["object_said"].push_back({o.id, "object " + o.id});
    say("object " + o.id, o.said);
  }
  for (const auto& r : R.relations) {
    const RelationType& t = *R.type(r.type);
    const std::string &from = R.objects[r.from].kind, &to = R.objects[r.to].kind;
    // The signature it holds by: the first that fits, or the type's first when none does.
    const Signature* sig = &t.signatures.front();
    for (const auto& s : t.signatures)
      if (R.kinds.is_a(from, s.from) && R.kinds.is_a(to, s.to)) { sig = &s; break; }
    const std::string type = t.id + ": " + sig->from + " to " + sig->to;
    if (reltypes.insert(type).second) {
      carrier["RelationType"].push_back(type);
      action["domain"].push_back({type, sig->from});
      action["codomain"].push_back({type, sig->to});
    }
    carrier["Relation"].push_back(r.id);
    action["from"].push_back({r.id, R.objects[r.from].id});
    action["to"].push_back({r.id, R.objects[r.to].id});
    action["type"].push_back({r.id, type});
    action["from_fits"].push_back({r.id, witness(from, sig->from)});
    action["to_fits"].push_back({r.id, witness(to, sig->to)});
    action["relation_said"].push_back({r.id, "relation " + r.id});
    say("relation " + r.id, r.said);
  }
  for (const auto& e : R.emergences) {
    carrier["Emergence"].push_back(e.id);
    action["product"].push_back({e.id, R.objects[e.product].id});
    action["emergence_said"].push_back({e.id, "emergence " + e.id});
    say("emergence " + e.id, e.said);
    for (std::size_t m : e.marked_by) {
      const std::string id = e.id + " marked by " + R.objects[m].id;
      carrier["Marking"].push_back(id);
      action["marks"].push_back({id, e.id});
      action["marker"].push_back({id, R.objects[m].id});
    }
    for (std::size_t i = 0; i < e.premises.size(); i++) {
      const std::string id = e.id + " #" + std::to_string(i + 1);
      carrier["Premise"].push_back(id);
      action["of"].push_back({id, e.id});
      action["what"].push_back({id, R.objects[e.premises[i].what].id});
      action["role"].push_back({id, e.premises[i].role});
      action["premise_said"].push_back({id, "premise " + id});
      // A premise written as a relation is said where the relation is: its statement carries no second assertion.
      say("premise " + id, e.premises[i].relation >= 0 ? std::vector<Assertion>{} : e.premises[i].said);
    }
  }
  std::string out = "sets " + std::string(name) + " : Record {\n";
  for (const char* t : tables) {
    out += std::string("  ") + t + " = {";
    for (std::size_t i = 0; i < carrier[t].size(); i++) out += (i ? ", " : "") + q(carrier[t][i]);
    out += "}\n";
  }
  for (const auto& [arrow, pairs] : action) {
    if (pairs.empty()) continue;
    out += "  " + arrow + " : ";
    for (std::size_t i = 0; i < pairs.size(); i++) out += (i ? ", " : "") + q(pairs[i].first) + " -> " + q(pairs[i].second);
    out += "\n";
  }
  return out + "}\n";
}

}  // namespace history::objects
