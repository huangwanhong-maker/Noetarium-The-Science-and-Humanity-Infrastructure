// Theories of the case written as a set-valued functor on the schema's category Theory (schema/theories.cat), in kategoria's
// language of scripts: accounts, events, parts, items, stances and bindings as sets, their references as functions. The
// assessment of an account (tekmerion) checks it by its own code; kategoria's verdict on what is written here shares none of it.
//
// A role is a role of a kind ("ignite.actor"). An event given a role its kind does not have is written with the first kind that
// has a role of that name, as it is, so that the functor fails at the equation that a part is played in a role of its event's kind,
// and nowhere else.
#pragma once

#include <law/theories/theories.hpp>

#include <map>
#include <set>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace law::theories {

inline std::string as_sets(const ts::Problem& p, const std::vector<std::pair<std::string, ts::Scenario>>& accounts, const Bindings& b, std::string_view name) {
  const auto q = [](const std::string& s) {
    std::string out = "\"";
    for (char c : s) out += (c == '"' || c == '\n') ? '\'' : c;
    return out + "\"";
  };
  const std::vector<std::string> tables{"Register", "Kind", "Role", "Account", "Event", "Agent", "Part", "Saying", "Modality", "Manifest", "Holding", "Item", "Belief", "Stance", "Fact", "Binding"};
  std::map<std::string, std::vector<std::string>> carrier;
  std::map<std::string, std::vector<std::pair<std::string, std::string>>> action;
  for (const auto& t : tables) carrier[t];
  carrier["Register"] = {"ontological", "relational", "manifestational"};
  carrier["Manifest"] = {"trace"};
  action["as"].push_back({"trace", "manifestational"});
  carrier["Belief"] = {"believed", "doubted"};
  for (const ts::Says s : {ts::Says::sighting, ts::Says::event, ts::Says::absence, ts::Says::interval}) carrier["Saying"].push_back(ts::word(s));
  const auto role_name = [&](const std::string& kind, const std::string& role) {
    if (const auto* k = p.kind(kind); k && std::find(k->roles.begin(), k->roles.end(), role) != k->roles.end()) return kind + "." + role;
    for (const auto& k : p.kinds)
      if (std::find(k.roles.begin(), k.roles.end(), role) != k.roles.end()) return k.id + "." + role;
    return kind + "." + role;
  };
  for (const auto& k : p.kinds) {
    carrier["Kind"].push_back(k.id);
    for (const auto& r : k.roles) {
      carrier["Role"].push_back(k.id + "." + r);
      action["role_of"].push_back({k.id + "." + r, k.id});
    }
  }
  for (const auto& a : p.agents) carrier["Agent"].push_back(a.id);
  std::set<std::string> modalities, holdings;
  for (const auto& i : p.items) {
    if (i.held_back) continue;
    const std::string m = i.modality.empty() ? "unstated" : i.modality, h = i.holding.empty() ? "holding of " + i.id : i.holding;
    if (modalities.insert(m).second) carrier["Modality"].push_back(m), action["register"].push_back({m, "manifestational"});
    if (holdings.insert(h).second) carrier["Holding"].push_back(h), action["modality"].push_back({h, m}), action["marks"].push_back({h, "trace"});
    carrier["Item"].push_back(i.id);
    action["says"].push_back({i.id, ts::word(i.says)});
    action["held"].push_back({i.id, h});
  }
  for (std::size_t k = 0; k < accounts.size(); k++) {
    const auto& [account, s] = accounts[k];
    carrier["Account"].push_back(account);
    for (std::size_t e = 0; e < s.events.size(); e++) {
      const std::string ev = account + ":event " + std::to_string(e + 1);
      carrier["Event"].push_back(ev);
      action["of_kind"].push_back({ev, s.events[e].kind});
      action["account"].push_back({ev, account});
      for (const auto& [role, agent] : s.events[e].roles) {
        if (agent.empty()) continue;
        const std::string part = ev + ":" + role;
        carrier["Part"].push_back(part);
        action["event"].push_back({part, ev});
        action["agent"].push_back({part, agent});
        action["role"].push_back({part, role_name(s.events[e].kind, role)});
      }
    }
    for (std::size_t i = 0; i < p.items.size() && i < s.stances.size(); i++) {
      if (p.items[i].held_back) continue;
      const std::string st = account + ":" + p.items[i].id;
      carrier["Stance"].push_back(st);
      action["item"].push_back({st, p.items[i].id});
      action["taken"].push_back({st, account});
      action["stance"].push_back({st, s.stances[i].believed ? "believed" : "doubted"});
    }
  }
  std::set<std::string> facts;
  for (std::size_t k = 0; k < b.makes.size(); k++) {
    const auto& m = b.makes[k];
    const std::string id = "binding " + std::to_string(k + 1);
    carrier["Binding"].push_back(id);
    action["bkind"].push_back({id, m.kind});
    action["brole"].push_back({id, role_name(m.kind, m.role)});
    action["fact"].push_back({id, m.fact});
    if (facts.insert(m.fact).second) carrier["Fact"].push_back(m.fact);
  }
  std::string out = "sets " + std::string(name) + " : Theory {\n";
  for (const auto& t : tables) {
    out += "  " + t + " = {";
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

}  // namespace law::theories
