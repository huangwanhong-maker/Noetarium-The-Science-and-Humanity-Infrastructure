// A record of facts written as a set-valued functor on the schema's category Record (schema/facts.cat), in kategoria's language
// of scripts: its tables as sets, its references as functions. The domain's own faults() checks a record by its own code;
// kategoria's verdict on what is written here is the check that shares none of it (the checks run both). A trace whose kind
// is not a kind of trace is written as it is, so that the functor fails at the equation that makes traces manifestational,
// and nowhere else.

#pragma once

#include <law/facts/facts.hpp>

#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace law::facts {

inline std::string as_sets(const Record& r, std::string_view name) {
  const auto q = [](const std::string& s) {
    std::string out = "\"";
    for (char c : s) out += (c == '"' || c == '\n') ? '\'' : c;
    return out + "\"";
  };
  std::map<std::string, std::vector<std::string>> carrier;
  std::map<std::string, std::vector<std::pair<std::string, std::string>>> action;
  for (const char* t : {"Register", "Kind", "Standing", "Proceeding", "Fact", "Holding", "Manifest", "Trace", "Inference", "Presumption"}) carrier[t];
  for (const auto reg : {Register::ontological, Register::relational, Register::manifestational}) carrier["Register"].push_back(std::string(word(reg)));
  for (const auto& [k, parent] : kinds()) {
    carrier["Kind"].push_back(k);
    action["register"].push_back({k, std::string(word(register_of(k)))});
  }
  for (const auto st : {Standing::alleged, Standing::admitted, Standing::noticed, Standing::presumed, Standing::deemed, Standing::proved, Standing::not_proved, Standing::rebutted, Standing::found, Standing::final_})
    carrier["Standing"].push_back(std::string(word(st)));
  const std::string proceeding = r.proceeding.empty() ? "the proceeding" : r.proceeding;
  carrier["Proceeding"].push_back(proceeding);
  carrier["Manifest"].push_back("trace");
  action["as"].push_back({"trace", "manifestational"});
  for (const auto& f : r.facts) {
    carrier["Fact"].push_back(f.id);
    action["kind"].push_back({f.id, f.kind});
    action["standing"].push_back({f.id, std::string(word(f.standing))});
    action["within"].push_back({f.id, proceeding});
    if (f.standing == Standing::presumed || f.standing == Standing::rebutted) {
      const std::string p = "presumption of " + f.id;
      carrier["Presumption"].push_back(p);
      action["basic"].push_back({p, f.basis});
      action["presumed"].push_back({p, f.id});
    }
  }
  std::set<std::string> holdings;
  for (const auto& t : r.traces) {
    carrier["Trace"].push_back(t.id);
    action["of"].push_back({t.id, t.of});
    action["held"].push_back({t.id, t.holding});
    action["shape"].push_back({t.id, t.kind});
    action["marks"].push_back({t.id, "trace"});
    if (holdings.insert(t.holding).second) carrier["Holding"].push_back(t.holding);
  }
  for (const auto& i : r.inferences) {
    carrier["Inference"].push_back(i.id);
    action["source"].push_back({i.id, i.from});
    action["target"].push_back({i.id, i.to});
  }
  std::string out = "sets " + std::string(name) + " : Record {\n";
  for (const char* t : {"Register", "Kind", "Standing", "Proceeding", "Fact", "Holding", "Manifest", "Trace", "Inference", "Presumption"}) {
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

}  // namespace law::facts
