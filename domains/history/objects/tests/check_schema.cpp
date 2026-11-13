// Checks of the schema of historical objects, by kategoria (schema/historical-objects.cat, objects/schema.hpp).
//
// - The schema's script is carried out by kategoria's session, and every statement is answered as the script
//   expects: "fails" where the line above says so, "holds" everywhere else. So the kinds are a thin category
//   with a terminal object, a discipline's kinds extend them fully and faithfully, the layering relations are
//   presented with a complete rewriting system, the record's schema is a finite category, a sample record is a
//   functor on it, and an ill-typed record and a dating against time are refused.
// - The core's kinds as the C++ holds them are the schema's: the same objects, and the same order.
// - Every record of the domain, written as a set-valued functor (as_sets), is a functor on the schema's
//   category Record: kategoria's verdict on what the reader has read, by code that shares none of the reader's.
// - A record with an ill-typed relation, written the same way, is not a functor, and the failure names the
//   equation of typing.

#include <history/objects/record.hpp>
#include <history/objects/schema.hpp>

#include <kategoria/script/session.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace history::objects;
using kategoria::script::Session;
using kategoria::script::Value;

namespace {

std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}

std::string field(const Value& outcome, const char* key) {
  const Value* v = outcome.get(key);
  return v ? v->text() : std::string();
}

// The block `category <name> { … }` of a script.
std::string block(const std::string& script, const std::string& name) {
  const auto start = script.find("category " + name + " {");
  const auto end = script.find("\n}", start);
  return start == std::string::npos || end == std::string::npos ? std::string() : script.substr(start, end + 2 - start) + "\n";
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string script = slurp(std::string(HISTORY_SCHEMA) + "/historical-objects.cat");
  s.require("the schema's script is read", !script.empty());

  std::vector<std::string> lines;
  {
    std::istringstream in(script);
    for (std::string l; std::getline(in, l);) lines.push_back(l);
  }
  Session session;
  const Value outcomes = session.run(script);
  std::size_t wrong = 0, failing = 0;
  std::string first;
  for (const Value& o : outcomes.items()) {
    const auto line = static_cast<std::size_t>(o.get("line")->number());
    const bool expect_fails = line >= 2 && lines[line - 2].find("# expect fails") == 0;
    failing += expect_fails;
    const std::string status = field(o, "status");
    if (status != (expect_fails ? "fails" : "holds")) {
      wrong++;
      if (first.empty()) first = "line " + std::to_string(line) + ": " + status + ", " + field(o, "reason");
    }
  }
  s.require("kategoria answers every statement of the schema as the script expects (" + std::to_string(outcomes.size()) + " statements, " + std::to_string(failing) + " meant to fail)",
            outcomes.size() >= 40 && wrong == 0, first);

  {
    // The C++ core's kinds against the schema's category Kinds: each kind an object, and a ≤ b exactly when the schema has an arrow.
    Session k;
    std::string text = block(script, "Kinds");
    const Kinds core = Kinds::core();
    std::size_t asked = 0, agreed = 0;
    // An arrow a → b exists exactly when a functor from the arrow category sending 0 ↦ a, 1 ↦ b does.
    text += "category I = chain 2\n";
    for (const auto& a : core.all())
      for (const auto& b : core.all()) text += "functor \"" + a.id + "<" + b.id + "\" : I -> Kinds { 0 -> " + a.id + "; 1 -> " + b.id + " }\n";
    const Value answers = k.run(text);
    for (const Value& o : answers.items()) {
      std::string name = field(o, "name");
      name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
      const auto cut = name.find('<');
      if (field(o, "kind") != "functor" || cut == std::string::npos) continue;
      asked++;
      agreed += (field(o, "status") == "holds") == core.is_a(name.substr(0, cut), name.substr(cut + 1));
    }
    s.require("the order of the core's kinds in the C++ is the schema's, pair by pair (" + std::to_string(asked) + " pairs)", asked == 400 && agreed == asked, std::to_string(agreed));
  }

  const std::string record_category = block(script, "Record");
  s.require("the schema holds the category Record", !record_category.empty());
  for (const auto& entry : std::filesystem::directory_iterator(HISTORY_RECORDS)) {
    if (entry.path().extension() != ".jsonl") continue;
    const Record R = read_file(entry.path().string());
    Session one;
    one.most_arrows = 100000;
    const Value answers = one.run(record_category + as_sets(R, "TheRecord"));
    const Value& last = answers.items().back();
    s.require("the record " + entry.path().stem().string() + " is a functor on the schema, by kategoria", field(last, "kind") == "sets" && field(last, "status") == "holds", field(last, "reason"));
  }
  {
    // A relation made ill-typed after reading: a person "draws on"… set out by a text. The reader would refuse it; written as sets, kategoria does.
    Record R = read_text(R"({"row": "record", "id": "ill", "format": "noetarium.histos/1", "title": "Ill-typed"}
{"row": "object", "id": "ann", "kind": "Person", "name": "Ann"}
{"row": "object", "id": "book", "kind": "Work", "name": "A book"}
{"row": "relation", "id": "r1", "type": "made_by", "from": "book", "to": "ann"}
)");
    s.require("a small record reads", R.faults.empty() && R.relations.size() == 1);
    Session good;
    const Value yes = good.run(record_category + as_sets(R, "Small"));
    s.require("as it was read, it is a functor", field(yes.items().back(), "status") == "holds", field(yes.items().back(), "reason"));
    std::swap(R.relations[0].from, R.relations[0].to);  // now a person is "made by" a book
    Session bad;
    const Value no = bad.run(record_category + as_sets(R, "Small"));
    const std::string reason = field(no.items().back(), "reason");
    s.require("with the relation's ends exchanged it is no functor, and the failure is an equation of typing", field(no.items().back(), "status") == "fails" && reason.find("does not keep the composite") != std::string::npos, reason);
  }

  return s.done();
}
