// A statement of need, and the dataset it is judged on (ADR 0125; the theory note §2, §6).
//
// A statement says that a group, within a frame, has a gap in a domain of satisfaction, judged on an indicator by one of
// Bradshaw's four standards. The dataset declares what a statement may name: the attributes groups are stated in; the indicators,
// each a ratio of two sums over the units (a value per unit weighted by the people it stands for, or a share counted per unit as
// successes of a total); the domains; and, for the tests, the parts, the clusters, a causal graph, places with suppliers, and rules.
#pragma once

#include <needs/groups/group.hpp>

#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace needs::language {

using groups::Group;
using groups::Refusal;
using groups::Table;

enum class Standard { normative, comparative, expressed, felt };

inline const char* name(Standard s) {
  switch (s) {
    case Standard::normative: return "normative";
    case Standard::comparative: return "comparative";
    case Standard::expressed: return "expressed";
    default: return "felt";
  }
}

struct Statement {
  std::string domain;                    // what is needed: a word of the declared vocabulary
  Group group;                           // who
  Group frame;                           // within whom (everyone, when empty)
  Standard standard = Standard::comparative;
  std::string indicator;                 // normative, comparative, felt: the indicator's name; expressed: the demand column
  bool worse_when_higher = true;         // comparative: "higher"; normative and felt: "above"
  double level = std::numeric_limits<double>::quiet_NaN();  // normative and felt
  std::optional<Group> reference;        // comparative: "than G'" (the frame less the group, when absent)
  std::string provision;                 // expressed: the provision column

  std::string standard_text() const {
    switch (standard) {
      case Standard::normative:
      case Standard::felt: return std::string(name(standard)) + " " + indicator + (worse_when_higher ? " above " : " below ") + groups::number_text(level);
      case Standard::comparative: return "comparative " + indicator + (worse_when_higher ? " higher" : " lower") + (reference ? " than " + reference->write() : "");
      default: return "expressed " + indicator + " over " + provision;
    }
  }
  std::string write() const {
    std::string s = "need " + domain + " for " + group.write();
    if (!frame.conditions.empty()) s += " within " + frame.write();
    return s + " by " + standard_text();
  }
  // The canonical form: groups by their keys, so that one statement reached two ways has one key.
  std::string key() const {
    std::string s = "need " + domain + " for " + group.key();
    if (!frame.conditions.empty()) s += " within " + frame.key();
    s += " by " + std::string(name(standard)) + " " + indicator;
    if (standard == Standard::normative || standard == Standard::felt) s += (worse_when_higher ? " above " : " below ") + groups::number_text(level);
    if (standard == Standard::comparative) s += std::string(worse_when_higher ? " higher" : " lower") + (reference ? " than " + reference->key() : "");
    if (standard == Standard::expressed) s += " over " + provision;
    return s;
  }
};

// An indicator is the ratio of two sums over a group's units, Σ w y / Σ w x, w the table's weight:
// - a value: y = (the people the row stands for) × value, x = the people (the `total` column, or 1 for every row);
// - a share: y = the successes, x = the total counted (both columns of counts).
// Its direction says which way is worse; a statement must agree with it (comparative "higher" for an indicator worse when higher).
struct Indicator {
  std::string name;
  bool share = false;
  std::string column;        // value: the column; share: the successes
  std::string total;         // share: the totals; value: the people each value stands for (optional)
  bool higher_is_worse = true;
  std::string label, unit, domain;
  std::vector<double> levels;  // normative levels an authority declares for it (a share in (0, 1)), offered to a search
};

// What the tests may use, as declared with the data.
struct Supplier {
  std::string id;
  double x = 0, y = 0;                   // metres on a local plane
  double capacity = 0;
  std::string domain;                    // the domain it serves (any, when empty)
};
struct Arrow { std::string from, to; };
struct Rule {                            // a normative rule an authority holds: who is entitled to what
  std::string text;                      // a statement, read by the language: its group and domain are the rule
  std::string source;                    // whose rule it is
};

// The open terms of statements, each with the readings it admits and where each reading comes from (ADR 0126; tekmerion's
// construal). A source is authority, convention, analyst, data or proposal; a grade is A (a structured source), B (a text with its
// passage) or C (a model's word from memory, always a proposal).
struct Reading {
  std::string text;                      // indicator: an indicator's name; reference: a group, "everyone" or "the rest"; level: a number or a percentage
  std::string source = "analyst";
  char grade = 'A';
  std::string by, rationale;
};
enum class PointKind { indicator, reference, level };
inline const char* name(PointKind k) { return k == PointKind::indicator ? "indicator" : k == PointKind::reference ? "reference" : "level"; }
struct Point {
  std::string name, about;
  PointKind kind = PointKind::reference;
  std::vector<std::string> indicators;   // reference and level: the indicators it concerns (any, when empty)
  std::vector<Reading> readings;
};
// A causal graph other than the dataset's own, for the causal test read under each.
struct Graph {
  std::string name, source = "analyst";
  char grade = 'A';
  std::string by, rationale;
  std::vector<Arrow> arrows;
};
// A rival explanation of a gap: strata within which the gap is looked for again (a home for the aged, a student's residence).
struct Rival {
  std::string name, about, source = "analyst";
  char grade = 'A';
  std::string by;
  std::vector<Group> strata;
  std::vector<std::string> labels;       // each stratum as written
};

struct Dataset {
  Table table;
  std::vector<std::string> attributes;   // the columns groups may be stated in (all columns not indicators, when empty)
  std::map<std::string, Indicator> indicators;
  std::vector<std::string> domains;      // the vocabulary of domains (any word, when empty)
  std::string part;                      // a category column of parts: discovery, validation, confirmation (made from the clusters when empty)
  std::string cluster;                   // a category column of clusters: the units resampled together and the PSUs of the variance
  std::vector<Arrow> graph;              // a causal graph over column names, for the causal test
  std::map<std::string, std::string> provision;  // domain → the column of its provision, for the causal test
  std::string place;                     // a category column of the units' places, for the simulation
  std::map<std::string, std::pair<double, double>> where;  // place → (x, y) metres on the suppliers' plane
  std::vector<Supplier> suppliers;
  double reach = 800;                    // the simulation's catchment, metres
  std::vector<Rule> rules;
  std::vector<std::pair<std::string, std::string>> implies;  // domain → domain: whoever needs the first needs the second
  // Interpretation (ADR 0126).
  std::vector<Point> points;
  std::vector<std::pair<std::string, std::string>> order;    // (weaker, stronger) sources; tekmerion's order when not stated
  bool order_stated = false;
  std::string graph_source = "analyst";  // the source of the dataset's own graph
  std::vector<Graph> graphs;
  std::vector<Rival> rivals;

  const Indicator& indicator(const std::string& n) const {
    auto it = indicators.find(n);
    if (it == indicators.end()) {
      std::string all;
      for (const auto& [k, v] : indicators) all += (all.empty() ? "" : ", ") + k;
      throw Refusal("the data declare no indicator " + n + " (there are " + all + ")");
    }
    return it->second;
  }
  bool attribute(const std::string& n) const {
    if (attributes.empty()) {
      for (const auto& [k, ind] : indicators)
        if (n == ind.column || n == ind.total) return false;
      return table.find(n) != nullptr && n != part && n != cluster;
    }
    for (const auto& a : attributes)
      if (a == n) return true;
    return false;
  }
  std::vector<std::string> attribute_names() const {
    if (!attributes.empty()) return attributes;
    std::vector<std::string> out;
    for (const auto& c : table.columns)
      if (attribute(c.name)) out.push_back(c.name);
    return out;
  }
};

// Binds a statement read from text to the dataset's table (numbers written against categories become levels).
inline void bind(const Dataset& d, Statement& s) {
  groups::bind(d.table, s.group);
  groups::bind(d.table, s.frame);
  if (s.reference) groups::bind(d.table, *s.reference);
}

// Whether a statement may be judged on the dataset: every condition on an attribute that is there, with its relation and levels;
// a declared domain; an indicator that is there, with a standard that fits it and a direction that agrees with it.
inline void check(const Dataset& d, const Statement& s) {
  if (s.domain.empty()) throw Refusal("a statement names the domain of what is needed");
  if (!d.domains.empty()) {
    bool known = false;
    for (const auto& w : d.domains) known = known || w == s.domain;
    if (!known) {
      std::string all;
      for (const auto& w : d.domains) all += (all.empty() ? "" : ", ") + w;
      throw Refusal("the domain " + s.domain + " is not declared (there are " + all + ")");
    }
  }
  auto check_group = [&](const Group& g, const char* what) {
    for (const auto& c : g.conditions)
      if (!d.attribute(c.attribute)) throw Refusal(std::string("the ") + what + " is stated in " + c.attribute + ", which is not an attribute groups may be stated in");
    groups::check(d.table, g);
  };
  if (s.group.conditions.empty()) throw Refusal("a need is stated for a group: 'everyone' compares with nobody");
  check_group(s.group, "group");
  check_group(s.frame, "frame");
  if (s.reference) check_group(*s.reference, "reference group");
  if (s.standard == Standard::expressed) {
    for (const auto* c : {&s.indicator, &s.provision}) {
      const auto* col = d.table.find(*c);
      if (!col || col->kind != groups::Kind::number) throw Refusal("an expressed need names columns of numbers for its demand and its provision; " + *c + " is not one");
    }
    return;
  }
  const Indicator& ind = d.indicator(s.indicator);
  const auto* col = d.table.find(ind.column);
  if (!col || col->kind != groups::Kind::number) throw Refusal("the indicator " + ind.name + " has no column of numbers " + ind.column);
  if (ind.share || !ind.total.empty()) {
    const auto* tot = d.table.find(ind.total);
    if (!tot || tot->kind != groups::Kind::number) throw Refusal("the indicator " + ind.name + " has no column of numbers " + ind.total + " for its totals");
  }
  if (s.worse_when_higher != ind.higher_is_worse) {
    const char* worse = ind.higher_is_worse ? "higher" : "lower";
    const char* word = s.standard == Standard::comparative ? (ind.higher_is_worse ? "higher" : "lower") : (ind.higher_is_worse ? "above" : "below");
    throw Refusal("the indicator " + ind.name + " is worse when " + worse + ", so a need on it is stated '" + word + "'");
  }
  if (s.standard == Standard::normative || s.standard == Standard::felt) {
    if (!std::isfinite(s.level)) throw Refusal("a normative or felt standard states its level");
    if (ind.share && !(s.level > 0 && s.level < 1)) throw Refusal("the level of a share lies strictly between 0 and 1 (or 0% and 100%)");
  }
}

}  // namespace needs::language
