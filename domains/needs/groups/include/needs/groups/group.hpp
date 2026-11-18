// Groups of units, stated as conjunctions of conditions on their attributes (ADR 0125; the theory note §2).
//
// A condition is an attribute, a relation and a value: `age >= 75`, `tenure = renting`, `district in {Hume, Wyndham}`. A number
// takes the six relations; a category takes = and != and membership in a set of its levels. A group is the conjunction of its
// conditions, `everyone` when it has none. Groups are the description language of subgroup discovery (Klösgen 1996; Wrobel 1997):
// a conjunction is a set of conditions, and a group with more conditions is more specific. Its rows are those that satisfy every
// condition; a missing value satisfies none.
//
// A group has a canonical key (its conditions sorted, each written canonically), so that a search keeps one copy of a group
// however it was reached, and `subsumes(a, b)` holds when every condition of a is among b's, which makes a at least as general as b.
#pragma once

#include <needs/groups/table.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <set>
#include <string>
#include <vector>

namespace needs::groups {

enum class Op { eq, ne, lt, le, gt, ge, in };

inline const char* text(Op o) {
  switch (o) {
    case Op::eq: return "=";
    case Op::ne: return "!=";
    case Op::lt: return "<";
    case Op::le: return "<=";
    case Op::gt: return ">";
    case Op::ge: return ">=";
    default: return "in";
  }
}

// The fewest digits that read back to the same number, so that a statement written and read again is the same statement; written
// without an exponent where a person would (20, not 2e+01), with one only for the very large and the very small.
inline std::string number_text(double v) {
  char b[64];
  const double a = std::fabs(v);
  const bool plain = a == 0 || (a >= 1e-4 && a < 1e15);
  for (int digits = 1; digits <= 17; digits++) {
    std::snprintf(b, sizeof b, "%.*g", digits, v);
    if (plain && std::strchr(b, 'e')) continue;
    if (std::strtod(b, nullptr) == v) return b;
  }
  if (plain)
    for (int decimals = 0; decimals <= 20; decimals++) {
      std::snprintf(b, sizeof b, "%.*f", decimals, v);
      if (std::strtod(b, nullptr) == v) return b;
    }
  std::snprintf(b, sizeof b, "%.17g", v);
  return b;
}

// Words of the language of statements (language/grammar.hpp), quoted when they are a value, so that a written group reads back.
inline bool reserved(const std::string& v) {
  static const char* words[] = {"need", "for", "within", "by", "and", "in", "everyone", "normative", "comparative", "expressed",
                                "felt", "above", "below", "higher", "lower", "than", "over"};
  for (const char* w : words)
    if (v == w) return true;
  return false;
}

struct Condition {
  std::string attribute;
  Op op = Op::eq;
  double number = 0;                 // for a numeric attribute
  std::vector<std::string> levels;   // for a categorical one: one level for = and !=, several for in (kept sorted)
  std::string literal;               // a number as it was written, so that bind() can read it as a level of a category

  std::string write() const {
    if (op == Op::in) {
      std::string s = attribute + " in {";
      for (std::size_t k = 0; k < levels.size(); k++) s += (k ? ", " : "") + quoted(levels[k]);
      return s + "}";
    }
    return attribute + " " + text(op) + " " + (levels.empty() ? number_text(number) : quoted(levels[0]));
  }
  static std::string quoted(const std::string& v) {
    bool plain = !v.empty();
    for (char c : v)
      if (!(std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '-' || c == '.')) plain = false;
    if (plain && (std::isdigit(static_cast<unsigned char>(v[0])) || v[0] == '-' || v[0] == '.')) plain = false;  // a value that reads as a number is quoted
    if (plain && reserved(v)) plain = false;
    if (plain) return v;
    std::string q = "\"";
    for (char c : v) {
      if (c == '"' || c == '\\') q += '\\';
      q += c;
    }
    return q + "\"";
  }
  bool operator==(const Condition& o) const { return write() == o.write(); }
};

struct Group {
  std::vector<Condition> conditions;  // conjunction; empty: everyone

  std::string key() const {
    std::vector<std::string> parts;
    for (const auto& c : conditions) parts.push_back(c.write());
    std::sort(parts.begin(), parts.end());
    std::string s;
    for (std::size_t k = 0; k < parts.size(); k++) s += (k ? " and " : "") + parts[k];
    return s.empty() ? "everyone" : s;
  }
  std::string write() const {
    if (conditions.empty()) return "everyone";
    std::string s;
    for (std::size_t k = 0; k < conditions.size(); k++) s += (k ? " and " : "") + conditions[k].write();
    return s;
  }
  std::size_t size() const { return conditions.size(); }
};

// Whether a condition is sound for the table: its attribute is there, its relation fits the attribute's kind, its levels exist.
inline void check(const Table& t, const Condition& c) {
  const Column& col = t.at(c.attribute);
  if (col.kind == Kind::number) {
    if (c.op == Op::in || !c.levels.empty()) throw Refusal("the attribute " + c.attribute + " is a number, and takes =, !=, <, <=, > or >= with a number");
    if (!std::isfinite(c.number)) throw Refusal("a condition on " + c.attribute + " compares with no number");
  } else {
    if (c.op != Op::eq && c.op != Op::ne && c.op != Op::in) throw Refusal("the attribute " + c.attribute + " is a category, and takes =, != or in");
    if (c.levels.empty()) throw Refusal("a condition on " + c.attribute + " names no level");
    for (const auto& l : c.levels)
      if (col.level(l) < 0) throw Refusal("the attribute " + c.attribute + " has no level " + l);
  }
}

inline void check(const Table& t, const Group& g) {
  for (const auto& c : g.conditions) check(t, c);
}

// A condition read from text, bound to the table: a number written against a category is read as the level it spells
// (`sa1 = 20604112201`), and a level kept sorted for `in`.
inline void bind(const Table& t, Condition& c) {
  const Column* col = t.find(c.attribute);
  if (!col) return;  // check() names it
  if (col->kind == Kind::category && c.levels.empty() && !c.literal.empty()) c.levels = {c.literal};
  if (c.op == Op::in) {
    std::sort(c.levels.begin(), c.levels.end());
    c.levels.erase(std::unique(c.levels.begin(), c.levels.end()), c.levels.end());
  }
  if (col->kind == Kind::category) c.literal.clear();
}

inline void bind(const Table& t, Group& g) {
  for (auto& c : g.conditions) bind(t, c);
}

inline bool holds(const Column& col, const Condition& c, std::size_t i) {
  if (col.kind == Kind::number) {
    const double v = col.numbers[i];
    if (std::isnan(v)) return false;
    switch (c.op) {
      case Op::eq: return v == c.number;
      case Op::ne: return v != c.number;
      case Op::lt: return v < c.number;
      case Op::le: return v <= c.number;
      case Op::gt: return v > c.number;
      default: return v >= c.number;
    }
  }
  const int code = col.codes[i];
  if (code < 0) return false;
  const std::string& level = col.levels[static_cast<std::size_t>(code)];
  if (c.op == Op::in) return std::find(c.levels.begin(), c.levels.end(), level) != c.levels.end();
  return (level == c.levels[0]) == (c.op == Op::eq);
}

// The rows of a group, within the rows of a frame (all rows when the frame is empty).
inline Rows rows_of(const Table& t, const Group& g, const Rows& frame = {}) {
  check(t, g);
  Rows r(t.rows, 1);
  if (!frame.empty())
    for (std::size_t i = 0; i < t.rows; i++) r[i] = frame[i];
  for (const auto& c : g.conditions) {
    const Column& col = t.at(c.attribute);
    for (std::size_t i = 0; i < t.rows; i++)
      if (r[i] && !holds(col, c, i)) r[i] = 0;
  }
  return r;
}

// a is at least as general as b: every condition of a is a condition of b.
inline bool subsumes(const Group& a, const Group& b) {
  std::set<std::string> mine;
  for (const auto& c : b.conditions) mine.insert(c.write());
  for (const auto& c : a.conditions)
    if (!mine.count(c.write())) return false;
  return true;
}

// The conjunction of two groups, each condition once.
inline Group join(const Group& a, const Group& b) {
  Group g = a;
  std::set<std::string> have;
  for (const auto& c : a.conditions) have.insert(c.write());
  for (const auto& c : b.conditions)
    if (have.insert(c.write()).second) g.conditions.push_back(c);
  return g;
}

// The conditions the two groups share.
inline Group meet(const Group& a, const Group& b) {
  Group g;
  std::set<std::string> theirs;
  for (const auto& c : b.conditions) theirs.insert(c.write());
  for (const auto& c : a.conditions)
    if (theirs.count(c.write())) g.conditions.push_back(c);
  return g;
}

}  // namespace needs::groups
