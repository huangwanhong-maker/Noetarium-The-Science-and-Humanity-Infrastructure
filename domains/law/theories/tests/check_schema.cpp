// Checks of the schema of theories, by kategoria (schema/theories.cat, theories/schema.hpp).
//
// - The schema's script is carried out by kategoria's session, and every statement is answered as the script expects: the layers
//   of a theory lie in the registers in order, ending in what happened; the schema of a theory is thin; two accounts of one fire
//   are a functor on it; an account in which someone is the actor of a fault, and one kept as though it were a trace, are refused.
// - Accounts written by the domain (as_sets) are a functor on the schema's category Theory; an account whose fault is given an
//   actor is not, and the failure is the equation that a part is played in a role of its event's kind.

#include <law/theories/schema.hpp>

#include <kategoria/script/session.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace law::theories;
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

}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string script = slurp(std::string(LAW_SCHEMA) + "/theories.cat");
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
  s.require("kategoria answers every statement of the schema of theories as the script expects (" + std::to_string(outcomes.size()) + " statements, " + std::to_string(failing) + " meant to fail)",
            outcomes.size() >= 10 && failing == 2 && wrong == 0, first);
  {
    std::string terminal;
    for (const Value& o : outcomes.items())
      if (field(o, "claim").find("terminal object of Layers") != std::string::npos) terminal = field(o, "claim");
    s.require("the layers end in what happened", terminal.find("Happened") == 0, terminal);
  }

  const std::string theory = block(script, "Theory");
  s.require("the schema holds the category Theory", !theory.empty());
  ts::Problem p;
  p.agents = {{"vane", "Vane", 6, true}, {"grey", "the man in the grey coat", 2, false}};
  p.kinds = {{"ignite", "a flame applied", {"actor"}}, {"fault", "a fault", {}}};
  p.scheme = {{"origin", "how the fire began", {"ignite", "fault"}}};
  ts::Item cctv;
  cctv.id = "cctv", cctv.says = ts::Says::sighting, cctv.agent = "vane", cctv.modality = "image", cctv.holding = "the inn's still", cctv.credibility = 0.6;
  ts::Item expert;
  expert.id = "expert", expert.says = ts::Says::event, expert.kind = "ignite", expert.modality = "text", expert.holding = "the fire report", expert.radius = 5, expert.until = 100, expert.credibility = 0.85;
  p.items = {cctv, expert};
  ts::Scenario a, b;
  a.events = {{"ignite", {{"actor", "vane"}}, 0, 0, 50}};
  a.stances = {{true}, {true}};
  b.events = {{"ignite", {{"actor", "grey"}}, 0, 0, 60}};
  b.stances = {{false}, {true}};
  const Bindings bind{{{"ignite", "actor", "vane", "accused_set_fire"}}, {}};
  {
    Session one;
    const Value yes = one.run(theory + as_sets(p, {{"vane_did_it", a}, {"a_stranger", b}}, bind, "Two"));
    s.require("two accounts written by the domain are a functor on the schema, by kategoria", field(yes.items().back(), "kind") == "sets" && field(yes.items().back(), "status") == "holds", field(yes.items().back(), "reason"));
  }
  {
    ts::Scenario bad;
    bad.events = {{"fault", {{"actor", "vane"}}, 0, 0, 50}};
    bad.stances = {{true}, {true}};
    Session two;
    const Value no = two.run(theory + as_sets(p, {{"a_fault_with_a_hand", bad}}, bind, "Bad"));
    const std::string reason = field(no.items().back(), "reason");
    s.require("an account whose fault is given an actor is no functor, and the failure is the equation of roles", field(no.items().back(), "status") == "fails" && reason.find("role_of") != std::string::npos, reason);
  }
  return s.done();
}
