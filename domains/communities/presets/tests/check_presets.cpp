// Checks of the presets: six constitutions of different internal orders, written in one schema, each read and admitted by
// thesmos; they differ in their units of count; only the leagues give a position to communities alone; the first league's
// charter is entrenched and amended only by unanimity of its member communities.

#include <communities/presets/presets.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::constitutions::Constitution;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const std::vector<std::string> names = {"commons", "guild", "cooperative", "polity", "league", "first-league"};
  std::map<std::string, Constitution> presets;
  for (const auto& name : names) {
    const std::string text = slurp(std::string(COMMUNITIES_DATA) + "/presets/data/" + name + ".json");
    s.require("the preset " + name + " is read", !text.empty());
    presets[name] = thesmos::constitutions::read(text);
    const auto faults = thesmos::constitutions::admission(presets[name]);
    s.require("the preset " + name + " is admitted by form", faults.empty(), faults.empty() ? "" : std::to_string(faults.front().condition) + ": " + faults.front().reason);
  }
  std::set<std::set<std::string>> units;
  for (const auto& [name, c] : presets) units.insert(presets::profile(c).counted_by);
  s.require("the presets differ in their units of count (" + std::to_string(units.size()) + " different sets)", units.size() >= 3);
  std::vector<std::string> leagues;
  for (const auto& [name, c] : presets)
    if (presets::of_communities(c, "member")) leagues.push_back(name);
  s.require("only the leagues give their membership to communities alone", leagues == std::vector<std::string>{"first-league", "league"});
  const auto commons = presets::profile(presets["commons"]);
  s.require("in the commons anyone answerable may contribute, operatives among them", commons.open_positions.count("contributor") && commons.operative_positions.count("contributor"));
  const Constitution& first = presets["first-league"];
  const auto& charter = first.aggregation.at(first.amendment.by_level.at("constitutional"));
  s.require("the first league's charter is entrenched and amended only by unanimity, counted by community",
            std::find(first.amendment.entrenched.begin(), first.amendment.entrenched.end(), "the charter") != first.amendment.entrenched.end() &&
                charter.rule == "unanimity" && charter.count_by == "community");
  return s.done();
}
