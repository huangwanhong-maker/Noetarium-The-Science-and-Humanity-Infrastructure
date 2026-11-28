// Episodes of conduct read back from the behaviour journal (the design *Communities That Agents Of Any Provider Find, Join, Work In And
// Leave*, Section 4.1): the canonical text thesmos writes for an episode (behaviour::canonical), read as the episode it is. A header of
// its own, on thesmos's behaviour alone, so that a reader of the journal (the API's engine of sessions) need not include what the
// readings of trajectories rest on.

#pragma once

#include <thesmos/behaviour/behaviour.hpp>

#include <optional>
#include <string>

namespace communities::trajectories {

// An episode read back from its canonical text in the behaviour journal (thesmos's behaviour::canonical, parts "name:length:value;"),
// decided by writing it again: a text that is not exactly an episode's canonical text reads as nothing.
inline std::optional<thesmos::behaviour::Episode> episode_of(const std::string& text) {
  thesmos::behaviour::Episode e;
  std::size_t at = 0;
  try {
    while (at < text.size()) {
      const auto c1 = text.find(':', at);
      const auto c2 = c1 == std::string::npos ? c1 : text.find(':', c1 + 1);
      if (c2 == std::string::npos) return std::nullopt;
      const std::string key = text.substr(at, c1 - at), len = text.substr(c1 + 1, c2 - c1 - 1);
      if (len.empty() || len.size() > 9 || len.find_first_not_of("0123456789") != std::string::npos) return std::nullopt;
      const std::size_t n = std::stoul(len);
      if (c2 + 1 + n >= text.size() || text[c2 + 1 + n] != ';') return std::nullopt;
      const std::string v = text.substr(c2 + 1, n);
      at = c2 + 2 + n;
      std::string* scalar = key == "party" ? &e.party : key == "bearer" ? &e.bearer : key == "provider" ? &e.provider : key == "model" ? &e.model
                          : key == "community" ? &e.community : key == "node" ? &e.node : key == "operation" ? &e.operation : key == "kind" ? &e.kind
                          : key == "begun" ? &e.begun : key == "ended" ? &e.ended : nullptr;
      if (scalar) *scalar = v;
      else if (key == "turn") e.turn = std::stoll(v);
      else if (key.rfind("resource.", 0) == 0) e.resources[key.substr(9)] = std::stoll(v);
      else if (key == "input") e.inputs.insert(v);
      else if (key == "output") e.outputs.insert(v);
      else if (key == "holding") e.holdings.insert(v);
      else if (key == "item") e.items.insert(v);
      else if (key.rfind("feature.", 0) == 0) e.features[key.substr(8)] = v;
      else return std::nullopt;
    }
  } catch (const std::exception&) {
    return std::nullopt;
  }
  if (thesmos::behaviour::canonical(e) != text) return std::nullopt;
  return e;
}

}  // namespace communities::trajectories
