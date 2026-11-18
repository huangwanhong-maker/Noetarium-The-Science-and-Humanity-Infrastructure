// The reader: statements of need read from text by the grammar (grammar.hpp), and written back.
//
// The grammar is compiled once by morphe's reader of BNF; every text is parsed by Earley's parser. A text that is not a list of
// statements is refused with the line and column as far as which it is right, and what may follow there (morphe/formal/bnf.hpp).
// A text that is one is built, rule by rule, into Statements. write() is the inverse on what it covers: a statement written out
// reads back to the same statement, so that what a search, a person or a model writes is one language.
//
// read_each() reads a text line by line, each line on its own, and says of each what it read or why it refused: the form in which
// a model's proposals are taken, so that one bad line refuses that line alone, never repaired.
#pragma once

#include <needs/language/grammar.hpp>
#include <needs/language/statement.hpp>

#include <morphe/formal/bnf.hpp>

#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

namespace needs::language {

struct ReadError : Refusal {
  std::size_t line = 0, column = 0;
  ReadError(std::size_t l, std::size_t c, const std::string& what)
      : Refusal("line " + std::to_string(l) + ", column " + std::to_string(c) + ": " + what), line(l), column(c) {}
};

namespace detail {

using Node = morphe::formal::ParseNode;

inline const morphe::formal::Bnf& grammar() {
  static const morphe::formal::Bnf g = morphe::formal::compile_bnf(grammar_text);
  return g;
}

inline std::pair<std::size_t, std::size_t> place(std::string_view text, std::size_t offset) {
  std::size_t line = 1, column = 1;
  for (std::size_t i = 0; i < offset && i < text.size(); i++) {
    if (text[i] == '\n') line++, column = 1;
    else column++;
  }
  return {line, column};
}

inline std::string unquote(const std::string& q) {
  std::string out;
  for (std::size_t i = 1; i + 1 < q.size(); i++) {
    if (q[i] == '\\' && i + 2 < q.size()) {
      out += q[++i];
      continue;
    }
    out += q[i];
  }
  return out;
}

inline groups::Condition condition(const Node& n) {
  groups::Condition c;
  c.attribute = n.child("attribute")->text;
  const Node* op = n.child("op");
  const auto values = n.all("value");
  if (!op) {
    c.op = groups::Op::in;
    for (const Node* v : values) {
      const Node& x = v->children.at(0);
      c.levels.push_back(x.name == "quoted" ? unquote(x.text) : x.text);
    }
    return c;
  }
  const std::string o = op->text;
  c.op = o == "=" ? groups::Op::eq : o == "!=" ? groups::Op::ne : o == "<" ? groups::Op::lt : o == "<=" ? groups::Op::le : o == ">" ? groups::Op::gt : groups::Op::ge;
  const Node& x = values.at(0)->children.at(0);
  if (x.name == "number") c.number = std::strtod(x.text.c_str(), nullptr), c.literal = x.text;
  else c.levels = {x.name == "quoted" ? unquote(x.text) : x.text};
  return c;
}

inline Group group(const Node& n) {
  Group g;
  if (n.child("everyone")) return g;
  for (const Node* c : n.all("condition")) g.conditions.push_back(condition(*c));
  return g;
}

inline double level(const Node& n) {
  double v = std::strtod(n.child("number")->text.c_str(), nullptr);
  if (n.child("percent")) v /= 100;
  return v;
}

inline Statement statement(const Node& n) {
  Statement s;
  s.domain = n.child("domain")->text;
  s.group = group(*n.child("group"));
  if (const Node* f = n.child("frame")) s.frame = group(*f->child("group"));
  if (const Node* x = n.child("normative"); x || n.child("felt")) {
    const Node& y = x ? *x : *n.child("felt");
    s.standard = x ? Standard::normative : Standard::felt;
    s.indicator = y.child("indicator")->text;
    s.worse_when_higher = y.child("side")->text == "above";
    s.level = level(*y.child("level"));
  } else if (const Node* c = n.child("comparative")) {
    s.standard = Standard::comparative;
    s.indicator = c->child("indicator")->text;
    s.worse_when_higher = c->child("direction")->text == "higher";
    if (const Node* r = c->child("reference")) s.reference = group(*r->child("group"));
  } else {
    const Node* e = n.child("expressed");
    s.standard = Standard::expressed;
    s.indicator = e->child("demand")->text;
    s.provision = e->child("provision")->text;
  }
  return s;
}

inline ReadError error(std::string_view text, const morphe::formal::BnfParse& p, std::size_t offset = 0) {
  const auto [line, column] = place(text, p.furthest);
  std::string what;
  std::string list;
  for (std::size_t i = 0; i < p.expected.size(); i++) {
    if (i > 0) list += i + 1 == p.expected.size() ? " or " : ", ";
    list += p.expected[i];
  }
  if (p.furthest < text.size()) what = std::string("unexpected '") + text[p.furthest] + "'";
  else what = text.empty() ? "the text is empty" : "the statement ends too soon";
  if (!list.empty()) what += "; expected " + std::string(p.expected.size() > 1 ? "one of " : "") + list;
  (void)offset;
  return ReadError(line, column, what);
}

}  // namespace detail

// Every statement of a text; a text that is not one is refused with the place.
inline std::vector<Statement> read(std::string_view text) {
  const auto p = detail::grammar().parse(text);
  if (!p.accepted) throw detail::error(text, p);
  std::vector<Statement> out;
  for (const auto* n : p.tree.all("need")) out.push_back(detail::statement(*n));
  return out;
}

// One statement.
inline Statement read_one(std::string_view text) {
  const auto p = detail::grammar().parse(text, "need");
  if (!p.accepted) throw detail::error(text, p);
  return detail::statement(p.tree.name == "need" ? p.tree : *p.tree.child("need"));
}

// A text read line by line: each non-empty line that is not a comment, read on its own and checked against the dataset when one
// is given.
struct Attempt {
  std::size_t line = 0;
  std::string text;
  bool read = false;
  Statement statement;
  std::string why;  // when not read
};

inline std::vector<Attempt> read_each(std::string_view text, const Dataset* d = nullptr) {
  std::vector<Attempt> out;
  std::size_t line = 0, start = 0;
  while (start <= text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string_view::npos) end = text.size();
    line++;
    std::string_view l = text.substr(start, end - start);
    while (!l.empty() && (l.back() == '\r' || l.back() == ' ' || l.back() == '\t')) l.remove_suffix(1);
    while (!l.empty() && (l.front() == ' ' || l.front() == '\t' || l.front() == '-' || l.front() == '*')) l.remove_prefix(1);  // a list's marks
    if (!l.empty() && l.front() != '#') {
      Attempt a;
      a.line = line, a.text = std::string(l);
      try {
        a.statement = read_one(l);
        if (d) bind(*d, a.statement), check(*d, a.statement);
        a.read = true;
      } catch (const ReadError& e) {
        a.why = "column " + std::to_string(e.column) + ": " + std::string(e.what()).substr(std::string(e.what()).find(": ") + 2);
      } catch (const std::exception& e) {
        a.why = e.what();
      }
      out.push_back(std::move(a));
    }
    if (end == text.size()) break;
    start = end + 1;
  }
  return out;
}

// A text read, bound and checked against the dataset: every statement, or the first that fails with its line.
inline std::vector<Statement> prepare(const Dataset& d, std::string_view text) {
  auto all = read(text);
  for (std::size_t k = 0; k < all.size(); k++) {
    try {
      bind(d, all[k]);
      check(d, all[k]);
    } catch (const std::exception& e) {
      throw Refusal("statement " + std::to_string(k + 1) + ": " + e.what());
    }
  }
  return all;
}

inline std::string write(const std::vector<Statement>& all) {
  std::string s;
  for (const auto& x : all) s += x.write() + "\n";
  return s;
}

}  // namespace needs::language
