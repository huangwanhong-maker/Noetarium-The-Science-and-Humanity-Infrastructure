// Checks of the schema of facts, by kategoria (schema/facts.cat, facts/schema.hpp).
//
// - The schema's script is carried out by kategoria's session, and every statement is answered as the script expects: "fails"
//   where the line above says so, "holds" everywhere else. So the registers and the kinds are thin categories with a terminal
//   object, each kind lies in a register functorially, standing is not thin and its quotient is, custom is the product of its
//   two elements, the record's schema is a category, a sample record is a functor on it, and a record that keeps a state of
//   mind as a trace, or infers across proceedings, is refused.
// - The kinds as the C++ holds them are the schema's: the same objects and the same order, pair by pair.
// - A record of the domain, written as a set-valued functor (as_sets), is a functor on the schema's category Record; with a
//   trace of a kind that is no trace it is not, and the failure names the equation.

#include <law/facts/facts.hpp>
#include <law/facts/schema.hpp>

#include <kategoria/script/session.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace law::facts;
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

std::string block(const std::string& script, const std::string& name) {
  const auto start = script.find("category " + name + " {");
  const auto end = script.find("\n}", start);
  return start == std::string::npos || end == std::string::npos ? std::string() : script.substr(start, end + 2 - start) + "\n";
}

Record small() {
  Record r;
  r.proceeding = "Rivers v. Mill";
  Fact sig = fact("signed", "Rivers signed the deed");
  sig.kind = "Social", sig.standing = Standing::proved, sig.role = Role::evidentiary;
  sig.emerged_from = "the meeting", sig.since = "2019-05-03", sig.constrains = {"intent"};
  Fact intent = fact("intent", "Rivers meant to sell");
  intent.kind = "Mental", intent.standing = Standing::presumed, intent.basis = "signed";
  intent.emerged_from = "the signature", intent.since = "2019-05-03", intent.constrains = {"sale"};
  r.facts = {sig, intent};
  r.traces = {{"t1", "signed", "digest of the deed", "Document"}};
  r.inferences = {{"i1", "signed", "intent", "who signs means it", false}};
  return r;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string script = slurp(std::string(LAW_SCHEMA) + "/facts.cat");
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
            outcomes.size() >= 20 && failing == 4 && wrong == 0, first);
  {
    // What kategoria found, by name.
    std::string terminal_of_kinds, product;
    for (const Value& o : outcomes.items()) {
      const std::string claim = field(o, "claim");
      if (claim.find("terminal object of Kinds") != std::string::npos) terminal_of_kinds = claim;
      if (field(o, "text").find("find product") != std::string::npos) product = o.json();
    }
    s.require("the terminal kind is Fact", terminal_of_kinds.find("Fact") == 0, terminal_of_kinds);
    s.require("the product of a practice and its acceptance is Custom", product.find("Custom") != std::string::npos, product);
  }

  {
    // The C++'s order of kinds against the schema's category Kinds, pair by pair.
    Session k;
    std::string text = block(script, "Kinds");
    text += "category I = chain 2\n";
    for (const auto& [a, pa] : kinds())
      for (const auto& [b, pb] : kinds()) text += "functor \"" + a + "<" + b + "\" : I -> Kinds { 0 -> " + a + "; 1 -> " + b + " }\n";
    const Value answers = k.run(text);
    std::size_t asked = 0, agreed = 0;
    for (const Value& o : answers.items()) {
      std::string name = field(o, "name");
      name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
      const auto cut = name.find('<');
      if (field(o, "kind") != "functor" || cut == std::string::npos) continue;
      asked++;
      agreed += (field(o, "status") == "holds") == is_a(name.substr(0, cut), name.substr(cut + 1));
    }
    s.require("the order of kinds in the C++ is the schema's, pair by pair (" + std::to_string(asked) + " pairs)", asked == 225 && agreed == asked, std::to_string(agreed));
  }

  const std::string record_category = block(script, "Record");
  s.require("the schema holds the category Record", !record_category.empty());
  {
    const Record r = small();
    s.require("a small record is well formed by the domain's own rules", faults(r).empty());
    Session one;
    const Value yes = one.run(record_category + as_sets(r, "Small"));
    s.require("written as sets, it is a functor on the schema, by kategoria", field(yes.items().back(), "kind") == "sets" && field(yes.items().back(), "status") == "holds", field(yes.items().back(), "reason"));
    Record bad = r;
    bad.traces[0].kind = "Mental";
    Session two;
    const Value no = two.run(record_category + as_sets(bad, "Small"));
    const std::string reason = field(no.items().back(), "reason");
    s.require("with a trace of a kind that is no trace it is no functor, and the failure is the equation that makes traces manifestational",
              field(no.items().back(), "status") == "fails" && reason.find("does not keep the composite") != std::string::npos && reason.find("register") != std::string::npos, reason);
    s.require("and the domain's own rules find the same fault", !faults(bad).empty());
  }

  return s.done();
}
