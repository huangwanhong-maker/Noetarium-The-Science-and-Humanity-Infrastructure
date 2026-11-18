// A table of units for needs analysis (ADR 0125): people, households or places, each a row, with attributes that groups are
// stated in, indicators of their state, and a weight, the number of people a row stands for.
//
// A column is a number per row or a category per row (its levels kept in the order first seen). A missing value is NaN for a
// number and the level index −1 for a category; a condition on a missing value is false, so that a unit whose attribute is unknown
// belongs to no group stated in that attribute. Nothing here reads a file: the API reads tables and hands them over.
#pragma once

#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace needs::groups {

struct Refusal : std::runtime_error {
  explicit Refusal(const std::string& reason) : std::runtime_error(reason) {}
};

enum class Kind { number, category };

struct Column {
  std::string name;
  Kind kind = Kind::number;
  std::vector<double> numbers;     // kind number: a value per row, NaN when missing
  std::vector<int> codes;          // kind category: a level per row, −1 when missing
  std::vector<std::string> levels; // kind category: the levels, in the order first seen

  std::size_t size() const { return kind == Kind::number ? numbers.size() : codes.size(); }
  int level(const std::string& v) const {
    for (std::size_t k = 0; k < levels.size(); k++)
      if (levels[k] == v) return static_cast<int>(k);
    return -1;
  }
};

inline Column numbers(std::string name, std::vector<double> v) {
  Column c;
  c.name = std::move(name), c.kind = Kind::number, c.numbers = std::move(v);
  return c;
}

inline Column categories(std::string name, const std::vector<std::string>& v) {
  Column c;
  c.name = std::move(name), c.kind = Kind::category;
  std::map<std::string, int> seen;
  for (const auto& s : v) {
    if (s.empty()) { c.codes.push_back(-1); continue; }
    auto it = seen.find(s);
    if (it == seen.end()) it = seen.emplace(s, static_cast<int>(c.levels.size())).first, c.levels.push_back(s);
    c.codes.push_back(it->second);
  }
  return c;
}

struct Table {
  std::vector<Column> columns;
  std::vector<double> weight;  // people a row stands for (1 each when empty)
  std::size_t rows = 0;

  void add(Column c) {
    if (columns.empty() && weight.empty()) rows = c.size();
    if (c.size() != rows) throw Refusal("the column " + c.name + " has " + std::to_string(c.size()) + " rows, not " + std::to_string(rows));
    for (const auto& x : columns)
      if (x.name == c.name) throw Refusal("the column " + c.name + " is named twice");
    columns.push_back(std::move(c));
  }
  const Column* find(const std::string& name) const {
    for (const auto& c : columns)
      if (c.name == name) return &c;
    return nullptr;
  }
  const Column& at(const std::string& name) const {
    const Column* c = find(name);
    if (!c) throw Refusal("the data have no column " + name);
    return *c;
  }
  double w(std::size_t i) const { return weight.empty() ? 1.0 : weight[i]; }
  void set_weight(std::vector<double> v) {
    if (!columns.empty() && v.size() != rows) throw Refusal("the weights are not one per row");
    for (double x : v)
      if (!(x >= 0) || !std::isfinite(x)) throw Refusal("a weight is negative or not a number");
    if (columns.empty()) rows = v.size();
    weight = std::move(v);
  }
};

// Rows as a set: one flag per row.
using Rows = std::vector<char>;

inline std::size_t count(const Rows& r) {
  std::size_t n = 0;
  for (char c : r) n += c != 0;
  return n;
}

inline double weight_of(const Table& t, const Rows& r) {
  double s = 0;
  for (std::size_t i = 0; i < r.size(); i++)
    if (r[i]) s += t.w(i);
  return s;
}

// Jaccard's distance between two sets of rows, weighted by the rows' weights: 1 − |A ∩ B| / |A ∪ B|; 0 for two empty sets.
inline double jaccard_distance(const Table& t, const Rows& a, const Rows& b) {
  double both = 0, either = 0;
  for (std::size_t i = 0; i < a.size(); i++) {
    if (a[i] && b[i]) both += t.w(i);
    if (a[i] || b[i]) either += t.w(i);
  }
  return either > 0 ? 1 - both / either : 0.0;
}

}  // namespace needs::groups
