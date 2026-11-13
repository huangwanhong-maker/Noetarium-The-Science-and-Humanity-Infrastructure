// Argument maps: questions, the positions taken on them and the arguments for and against, after IBIS (Kunz and Rittel 1970,
// Issues as elements of information systems, Working Paper 131, Institute of Urban and Regional Development, University of
// California, Berkeley). The domain's theory note, theory/arguments.md, §7.
//
// A map holds three kinds of node, each with an id and a text:
// - a question (IBIS's issue), which may be declared exclusive (at most one of its positions can hold) and may be raised by a
//   position or an argument, so that maps nest: what is said in answer to one question raises the next;
// - a position, which answers one question;
// - an argument, which bears on one or more positions or arguments, on each for it (pro) or against it (con), and may state its
//   premises and its conclusion as texts. The texts are kept and not interpreted: whether the premises entail the conclusion is
//   for the planned module `logic`.
//
// What the map is asked:
// - its faults, each with where it is and why (faults): an id that is empty, not one word or used twice; a reference to an id
//   that is not in the map or is of the wrong kind; a position that answers no question; an argument that bears on nothing, bears
//   twice on one target, or is for itself; a cycle of "raised by". Two arguments against each other are no fault, nor is an
//   argument against itself (a self-defeating argument, which the grounded semantics leaves undecided);
// - the framework it induces (induce), Dung's (frameworks.hpp), on its positions and arguments: an argument against a target
//   attacks it; the positions of an exclusive question attack one another; and an argument for a target supports it, which adds
//   the supported attacks (an argument for b, where b attacks c, attacks c: so an argument for a position counts against its
//   exclusive rivals);
// - each position's and argument's standing under the grounded semantics (evaluate): accepted (in), rejected (out) or undecided,
//   the round at which it was settled and the nodes that decide it;
// - its open questions: those with no accepted position.
//
// Caveat. The grounded semantics is the sceptical one: it accepts only what is defended from what nothing attacks, and settles no
// tie. Two exclusive positions with nothing said on them are both undecided. If each has an unanswered argument for it, each is
// attacked by an accepted argument and both are rejected: the question is open either way. A reading in which the two arguments
// rebut each other and leave both positions undecided needs a further derived attack, not taken here (the README, §6.1).
//
// The written form. No reader of JSON is shared below the domains (the history domain's README, §6.2), so a map is written as
// lines of text (the README, §3.3), read by read() and written by write():
//
//   # a remark
//   exclusive question q1: Is the pious loved by the gods because it is pious?
//   question q2 <- p1: What, then, makes it pious?
//   position p1 -> q1: It is loved because it is pious.
//   pro a1 -> p1: The gods love for a reason.
//   con a2 -> a1: A reason that binds the gods stands above them.
//   con a2 -> p2
//   premise a1: To love is to love something for what it is.
//   conclusion a1: What is loved is loved for what it is.
//
// Limits. faults() costs O(q (n + r) log n) for q questions, n nodes and r references; induce() O(n (n + r)); evaluate() the same.
//
// Checks: tests/check_arguments.cpp.

#pragma once

#include <philosophy/arguments/frameworks.hpp>

#include <morphe/combinatorics/shortest_paths.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace philosophy::arguments {

// ---- The map ------------------------------------------------------------------------------------------------------------------

enum class Polarity { pro, con };

// What an argument bears on: a position or another argument, for it or against it.
struct Bearing {
  std::string target;
  Polarity polarity = Polarity::pro;
  friend bool operator==(const Bearing&, const Bearing&) = default;
};

// A question (IBIS's issue). raised_by is the id of the position or argument that raises it, or empty.
struct Question {
  std::string id, text;
  bool exclusive = false;
  std::string raised_by;
  friend bool operator==(const Question&, const Question&) = default;
};

// A position: an answer to the question whose id is `answers`.
struct Position {
  std::string id, text, answers;
  friend bool operator==(const Position&, const Position&) = default;
};

// An argument, with what it bears on and, if stated, its premises and its conclusion (empty when not stated).
struct Argument {
  std::string id, text;
  std::vector<Bearing> bears;
  std::vector<std::string> premises;
  std::string conclusion;
  friend bool operator==(const Argument&, const Argument&) = default;
};

struct Map {
  std::vector<Question> questions;
  std::vector<Position> positions;
  std::vector<Argument> arguments;
  friend bool operator==(const Map&, const Map&) = default;
};

// ---- Well-formedness ----------------------------------------------------------------------------------------------------------

enum class NodeKind { question, position, argument };

enum class FaultKind {
  bad_id,             // an id that is empty or not one word (it holds a blank or a colon, or is an arrow)
  duplicate_id,       // an id already used by an earlier node
  unknown_reference,  // a reference to an id that is not in the map
  wrong_kind,         // a position that answers what is not a question; an argument on a question; a question raised by a question
  answers_nothing,    // a position that answers no question
  bears_on_nothing,   // an argument that bears on nothing
  repeated_bearing,   // an argument that bears twice on one target
  argues_for_itself,  // an argument given as a reason for itself
  raised_by_cycle     // a question that, through what raises it, is raised by an answer to itself
};

// A fault: the node at fault (its kind, its place in its list, its id), why it is one, and the ids involved: the id referred
// to, or the nodes round a cycle from the question at fault back to it.
struct Fault {
  FaultKind kind = FaultKind::bad_id;
  NodeKind node = NodeKind::question;
  std::size_t index = 0;
  std::string id;
  std::string why;
  std::vector<std::string> involved;
};

namespace detail {

inline const char* name(NodeKind k) { return k == NodeKind::question ? "question" : k == NodeKind::position ? "position" : "argument"; }

inline bool blank(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v' || c == '\f'; }

// An id is one word: not empty, without blanks or colons, and not an arrow of the written form.
inline bool valid_id(const std::string& id) {
  if (id.empty() || id == "->" || id == "<-") return false;
  for (const char c : id)
    if (blank(c) || c == ':') return false;
  return true;
}

struct Entry {
  NodeKind kind;
  std::size_t index;
};

// Each id's first node.
inline std::unordered_map<std::string, Entry> entries(const Map& m) {
  std::unordered_map<std::string, Entry> first;
  for (std::size_t i = 0; i < m.questions.size(); i++) first.insert({m.questions[i].id, {NodeKind::question, i}});
  for (std::size_t i = 0; i < m.positions.size(); i++) first.insert({m.positions[i].id, {NodeKind::position, i}});
  for (std::size_t i = 0; i < m.arguments.size(); i++) first.insert({m.arguments[i].id, {NodeKind::argument, i}});
  return first;
}

}  // namespace detail

// Every fault of a map, in a fixed order: the faults of ids, node by node (questions, positions, arguments); then the faults of
// references, node by node; then the cycles of "raised by", each once, at its first question. Empty exactly when the map is well
// formed.
inline std::vector<Fault> faults(const Map& m) {
  using detail::name;
  std::vector<Fault> out;
  const std::size_t Q = m.questions.size(), P = m.positions.size(), A = m.arguments.size();

  // Ids.
  std::unordered_map<std::string, detail::Entry> first;
  const auto declare = [&](NodeKind kind, std::size_t i, const std::string& id) {
    if (!detail::valid_id(id)) {
      out.push_back({FaultKind::bad_id, kind, i, id, std::string("the ") + name(kind) + " numbered " + std::to_string(i) + (id.empty() ? " has no id" : " has the id '" + id + "', which is not one word without a colon"), {}});
      return;
    }
    const auto [it, fresh] = first.insert({id, {kind, i}});
    if (!fresh)
      out.push_back({FaultKind::duplicate_id, kind, i, id, std::string("the ") + name(kind) + " numbered " + std::to_string(i) + " has the id " + id + ", already that of the " + name(it->second.kind) + " numbered " + std::to_string(it->second.index), {id}});
  };
  for (std::size_t i = 0; i < Q; i++) declare(NodeKind::question, i, m.questions[i].id);
  for (std::size_t i = 0; i < P; i++) declare(NodeKind::position, i, m.positions[i].id);
  for (std::size_t i = 0; i < A; i++) declare(NodeKind::argument, i, m.arguments[i].id);

  // References.
  const auto find = [&](const std::string& id) { const auto it = first.find(id); return it == first.end() ? nullptr : &it->second; };
  for (std::size_t i = 0; i < Q; i++) {
    const Question& q = m.questions[i];
    if (q.raised_by.empty()) continue;
    const detail::Entry* e = find(q.raised_by);
    if (!e) out.push_back({FaultKind::unknown_reference, NodeKind::question, i, q.id, "question " + q.id + " is said to be raised by " + q.raised_by + ", which is not in the map", {q.raised_by}});
    else if (e->kind == NodeKind::question) out.push_back({FaultKind::wrong_kind, NodeKind::question, i, q.id, "question " + q.id + " is said to be raised by " + q.raised_by + ", a question; a question is raised by a position or an argument", {q.raised_by}});
  }
  for (std::size_t i = 0; i < P; i++) {
    const Position& p = m.positions[i];
    if (p.answers.empty()) { out.push_back({FaultKind::answers_nothing, NodeKind::position, i, p.id, "position " + p.id + " answers no question", {}}); continue; }
    const detail::Entry* e = find(p.answers);
    if (!e) out.push_back({FaultKind::unknown_reference, NodeKind::position, i, p.id, "position " + p.id + " answers " + p.answers + ", which is not in the map", {p.answers}});
    else if (e->kind != NodeKind::question) out.push_back({FaultKind::wrong_kind, NodeKind::position, i, p.id, "position " + p.id + " answers " + p.answers + ", " + (e->kind == NodeKind::position ? "a position" : "an argument") + "; a position answers a question", {p.answers}});
  }
  for (std::size_t i = 0; i < A; i++) {
    const Argument& a = m.arguments[i];
    if (a.bears.empty()) { out.push_back({FaultKind::bears_on_nothing, NodeKind::argument, i, a.id, "argument " + a.id + " bears on nothing: it is neither for nor against a position or an argument", {}}); continue; }
    std::set<std::string> seen;
    for (const Bearing& b : a.bears) {
      const std::string way = b.polarity == Polarity::pro ? "for " : "against ";
      if (!seen.insert(b.target).second) { out.push_back({FaultKind::repeated_bearing, NodeKind::argument, i, a.id, "argument " + a.id + " bears on " + b.target + " more than once", {b.target}}); continue; }
      if (b.target == a.id && detail::valid_id(a.id)) {
        if (b.polarity == Polarity::pro) out.push_back({FaultKind::argues_for_itself, NodeKind::argument, i, a.id, "argument " + a.id + " is given as a reason for itself", {a.id}});
        continue;  // an argument against itself is a self-defeating argument, and no fault
      }
      const detail::Entry* e = find(b.target);
      if (!e) out.push_back({FaultKind::unknown_reference, NodeKind::argument, i, a.id, "argument " + a.id + " is " + way + (b.target.empty() ? std::string("a target that is not named") : b.target + ", which is not in the map"), {b.target}});
      else if (e->kind == NodeKind::question) out.push_back({FaultKind::wrong_kind, NodeKind::argument, i, a.id, "argument " + a.id + " is " + way + b.target + ", a question; an argument bears on a position or an argument", {b.target}});
    }
  }

  // Cycles of "raised by". Each node points at what it hangs from: a question at what raises it, a position at its question, an
  // argument at each of its targets. A cycle through a question is a fault (the question would have to be asked before the
  // answer that raises it); a cycle among arguments alone is not. The shortest way back is morphe's.
  const std::size_t N = Q + P + A;
  const auto node = [&](const detail::Entry& e) { return e.kind == NodeKind::question ? e.index : e.kind == NodeKind::position ? Q + e.index : Q + P + e.index; };
  const auto id_of = [&](std::size_t v) -> const std::string& { return v < Q ? m.questions[v].id : v < Q + P ? m.positions[v - Q].id : m.arguments[v - Q - P].id; };
  morphe::combinatorics::WeightedAdjacency hangs(N);
  for (std::size_t i = 0; i < Q; i++)
    if (const detail::Entry* e = find(m.questions[i].raised_by); e && e->kind != NodeKind::question) hangs[i].push_back({node(*e), 1.0});
  for (std::size_t i = 0; i < P; i++)
    if (const detail::Entry* e = find(m.positions[i].answers); e && e->kind == NodeKind::question) hangs[Q + i].push_back({node(*e), 1.0});
  for (std::size_t i = 0; i < A; i++)
    for (const Bearing& b : m.arguments[i].bears)
      if (const detail::Entry* e = find(b.target); e && e->kind != NodeKind::question && node(*e) != Q + P + i) hangs[Q + P + i].push_back({node(*e), 1.0});
  std::vector<char> reported(Q, 0);
  for (std::size_t i = 0; i < Q; i++) {
    if (reported[i] || hangs[i].empty()) continue;
    const auto back = morphe::combinatorics::dijkstra(hangs, hangs[i][0].to);
    if (!std::isfinite(back.distance[i])) continue;
    std::vector<std::size_t> cycle{i};
    for (const std::size_t v : back.path_to(i)) cycle.push_back(v);  // i, what raises it, …, i
    Fault f{FaultKind::raised_by_cycle, NodeKind::question, i, m.questions[i].id, "question " + m.questions[i].id, {}};
    for (std::size_t k = 0; k + 1 < cycle.size(); k++) {
      const std::size_t v = cycle[k];
      if (v < Q) reported[v] = 1;
      f.involved.push_back(id_of(v));
      f.why += std::string(k ? ", which " : " ") + (v < Q ? "is raised by " : v < Q + P ? "answers " : "bears on ") + id_of(cycle[k + 1]);
    }
    f.why += ": a cycle of questions raised by their own answers";
    out.push_back(std::move(f));
  }
  return out;
}

inline bool well_formed(const Map& m) { return faults(m).empty(); }

// ---- The framework a map induces ----------------------------------------------------------------------------------------------

// Where an attack of the induced framework comes from: an argument against its target (con); two positions of an exclusive
// question, which is named (exclusive); or a chain of supports that ends in one of those (supported), the chain given as ids:
// the attacker first, each node for the next, the last attacking the target directly.
enum class AttackKind { con, exclusive, supported };

struct AttackOrigin {
  AttackKind kind = AttackKind::con;
  std::string question;
  std::vector<std::string> chain;
  friend bool operator==(const AttackOrigin&, const AttackOrigin&) = default;
};

// The framework induced by a map. Its arguments are the map's positions, in their order (0 … positions − 1), then the map's
// arguments in theirs; ids[i] is the id of the i-th. origins[e] says where the framework's e-th attack comes from: first the
// attacks among the positions of exclusive questions, question by question; then those of the arguments against, argument by
// argument; then the supported attacks.
struct Induced {
  Framework framework;
  std::vector<std::string> ids;
  std::size_t positions = 0;
  std::vector<AttackOrigin> origins;

  // The framework's argument with this id.
  std::size_t index(const std::string& id) const {
    for (std::size_t i = 0; i < ids.size(); i++)
      if (ids[i] == id) return i;
    throw std::out_of_range("philosophy::arguments::Induced: no position or argument " + id);
  }
};

// How a map is read as a framework. supported_attacks: whether an argument for a target takes on that target's attacks
// (frameworks.hpp, supported_attacks); without it the arguments for are kept in the map and play no part in the standings.
struct Induction {
  bool supported_attacks = true;
};

// The framework a well-formed map induces (the theory note, Definition 7.5). Refuses a map with faults, naming the first.
inline Induced induce(const Map& m, const Induction& how = {}) {
  const auto bad = faults(m);
  if (!bad.empty()) throw std::invalid_argument("philosophy::arguments::induce: the map is not well formed (" + std::to_string(bad.size()) + (bad.size() == 1 ? " fault: " : " faults, the first: ") + bad[0].why + ")");
  const std::size_t P = m.positions.size(), A = m.arguments.size();
  const auto first = detail::entries(m);
  Induced ind;
  ind.positions = P;
  for (const Position& p : m.positions) ind.ids.push_back(p.id);
  for (const Argument& a : m.arguments) ind.ids.push_back(a.id);
  Bipolar b;
  b.arguments = P + A;
  for (std::size_t q = 0; q < m.questions.size(); q++) {
    if (!m.questions[q].exclusive) continue;
    std::vector<std::size_t> answers;
    for (std::size_t i = 0; i < P; i++)
      if (m.positions[i].answers == m.questions[q].id) answers.push_back(i);
    for (const std::size_t x : answers)
      for (const std::size_t y : answers)
        if (x != y) b.attacks.push_back({x, y}), ind.origins.push_back({AttackKind::exclusive, m.questions[q].id, {}});
  }
  for (std::size_t i = 0; i < A; i++)
    for (const Bearing& bearing : m.arguments[i].bears) {
      const detail::Entry& e = first.at(bearing.target);
      const std::size_t target = e.kind == NodeKind::position ? e.index : P + e.index;
      if (bearing.polarity == Polarity::con) b.attacks.push_back({P + i, target}), ind.origins.push_back({AttackKind::con, "", {}});
      else if (how.supported_attacks) b.supports.push_back({P + i, target});
    }
  Derived d = supported_attacks(b);
  for (const SupportedAttack& s : d.supported) {
    AttackOrigin o{AttackKind::supported, "", {}};
    for (const std::size_t c : s.chain) o.chain.push_back(ind.ids[c]);
    ind.origins.push_back(std::move(o));
  }
  ind.framework = std::move(d.framework);
  return ind;
}

// ---- Standings ----------------------------------------------------------------------------------------------------------------

enum class Status { accepted, rejected, undecided };

// The standing of a position or an argument under the grounded semantics of the induced framework: accepted (in the grounded
// extension), rejected (attacked by an accepted node) or undecided; the round at which it was settled (frameworks.hpp, Grounded;
// 0 when undecided); the nodes that attack it; and the nodes that decide it, those by which the iteration settled it:
// - rejected at round k: its attackers accepted at round k, the first to defeat it (an attacker accepted later may owe its
//   acceptance to this rejection, as an exclusive rival does);
// - accepted at round k: the nodes accepted before round k that attack its attackers, its defenders (none when nothing attacks
//   it);
// - undecided: its undecided attackers, none of its attackers being accepted.
// Ids are in the order of the induced framework.
struct Standing {
  std::string id;
  Status status = Status::undecided;
  std::size_t round = 0;
  std::vector<std::string> attackers, deciding;
  friend bool operator==(const Standing&, const Standing&) = default;
};

// A question's positions by their standing. open: no position is accepted. moot: what raised the question is rejected.
struct QuestionStanding {
  std::string id;
  std::vector<std::string> accepted, rejected, undecided;
  bool open = true;
  bool moot = false;
  friend bool operator==(const QuestionStanding&, const QuestionStanding&) = default;
};

struct Evaluation {
  Induced induced;
  Grounded grounded;
  std::vector<Standing> positions, arguments;   // in the map's order
  std::vector<QuestionStanding> questions;      // in the map's order
  std::vector<std::string> open_questions;      // the ids of the questions with no accepted position, in the map's order
};

// A well-formed map evaluated under the grounded semantics (the theory note, Definition 7.6). Refuses a map with faults, as induce does.
inline Evaluation evaluate(const Map& m, const Induction& how = {}) {
  Evaluation ev;
  ev.induced = induce(m, how);
  ev.grounded = grounded(ev.induced.framework);
  const std::size_t n = ev.induced.framework.size(), P = ev.induced.positions;
  const auto nb = detail::neighbours(ev.induced.framework.attack);
  const auto status = [&](std::size_t v) { return ev.grounded.label[v] == Label::in ? Status::accepted : ev.grounded.label[v] == Label::out ? Status::rejected : Status::undecided; };
  for (std::size_t v = 0; v < n; v++) {
    Standing s{ev.induced.ids[v], status(v), ev.grounded.round[v], {}, {}};
    std::set<std::size_t> attackers(nb.attackers[v].begin(), nb.attackers[v].end()), deciding;
    for (const std::size_t x : attackers) {
      if (s.status == Status::rejected && status(x) == Status::accepted && ev.grounded.round[x] == s.round) deciding.insert(x);
      if (s.status == Status::undecided && status(x) == Status::undecided) deciding.insert(x);
      if (s.status == Status::accepted)
        for (const std::size_t y : nb.attackers[x])
          if (status(y) == Status::accepted && ev.grounded.round[y] < s.round) deciding.insert(y);
    }
    for (const std::size_t x : attackers) s.attackers.push_back(ev.induced.ids[x]);
    for (const std::size_t x : deciding) s.deciding.push_back(ev.induced.ids[x]);
    (v < P ? ev.positions : ev.arguments).push_back(std::move(s));
  }
  for (const Question& q : m.questions) {
    QuestionStanding qs{q.id, {}, {}, {}, true, false};
    for (std::size_t i = 0; i < P; i++) {
      if (m.positions[i].answers != q.id) continue;
      (status(i) == Status::accepted ? qs.accepted : status(i) == Status::rejected ? qs.rejected : qs.undecided).push_back(m.positions[i].id);
    }
    qs.open = qs.accepted.empty();
    qs.moot = !q.raised_by.empty() && status(ev.induced.index(q.raised_by)) == Status::rejected;
    if (qs.open) ev.open_questions.push_back(q.id);
    ev.questions.push_back(std::move(qs));
  }
  return ev;
}

// Each position's standing, in the map's order.
inline std::vector<Standing> standings(const Map& m, const Induction& how = {}) { return evaluate(m, how).positions; }

// The ids of the questions with no accepted position, in the map's order.
inline std::vector<std::string> open_questions(const Map& m, const Induction& how = {}) { return evaluate(m, how).open_questions; }

// ---- The written form ---------------------------------------------------------------------------------------------------------
//
// One statement a line (the README, §3.3). A line that is empty or begins with # is passed over. Otherwise it is a head, then
// optionally a colon and a text that runs to the end of the line:
//
//   [exclusive] question <id> [<- <id>]: <text>      a question, raised by the node after the arrow
//   position <id> [-> <id>]: <text>                  a position and the question it answers
//   argument <id>: <text>                            an argument that bears on nothing yet
//   pro <id> -> <id>[: <text>]                       an argument for a target: stated here, with its text, if its id is new;
//   con <id> -> <id>[: <text>]                       a further bearing, without text, of an argument stated above otherwise
//   premise <id>: <text>                             a premise of the argument stated above
//   conclusion <id>: <text>                          its conclusion
//
// An id is one word without a colon. The words of a head are parted by blanks, the arrows among them. In a text, \\ is a
// backslash, \n a new line, \r a carriage return, \t a tab and \s a space (written only where a text begins or ends with one,
// since the blanks about a text are dropped). The reader reads syntax only: what the references mean is for faults().

// A line that could not be read: its number from 1, and why.
struct SyntaxError {
  std::size_t line = 0;
  std::string why;
  friend bool operator==(const SyntaxError&, const SyntaxError&) = default;
};

// What was read: the map of the lines that could be read, and the others' errors.
struct Reading {
  Map map;
  std::vector<SyntaxError> errors;
  bool ok() const { return errors.empty(); }
};

namespace detail {

inline std::string escape(const std::string& text) {
  std::string out;
  for (std::size_t i = 0; i < text.size(); i++) {
    const char c = text[i];
    if (c == '\\') out += "\\\\";
    else if (c == '\n') out += "\\n";
    else if (c == '\r') out += "\\r";
    else if (c == '\t') out += "\\t";
    else if (c == ' ' && (i == 0 || i + 1 == text.size())) out += "\\s";
    else out += c;
  }
  return out;
}

// False, with the reason, at an escape that is not one.
inline bool unescape(std::string_view raw, std::string& text, std::string& why) {
  text.clear();
  for (std::size_t i = 0; i < raw.size(); i++) {
    if (raw[i] != '\\') { text += raw[i]; continue; }
    if (++i == raw.size()) { why = "a text ends in a lone backslash"; return false; }
    switch (raw[i]) {
      case '\\': text += '\\'; break;
      case 'n': text += '\n'; break;
      case 'r': text += '\r'; break;
      case 't': text += '\t'; break;
      case 's': text += ' '; break;
      default: why = std::string("\\") + raw[i] + " is not an escape (\\\\, \\n, \\r, \\t and \\s are)"; return false;
    }
  }
  return true;
}

inline std::string_view trim(std::string_view s) {
  while (!s.empty() && (s.front() == ' ' || s.front() == '\t')) s.remove_prefix(1);
  while (!s.empty() && (s.back() == ' ' || s.back() == '\t' || s.back() == '\r')) s.remove_suffix(1);
  return s;
}

}  // namespace detail

// A map as lines of text: its questions, its positions and its arguments, each in the map's order, an argument followed by its
// further bearings, its premises and its conclusion. read(write(m)).map == m whenever every id and every reference of m is one
// word without a colon and no id is used twice (in particular for every well-formed map).
inline std::string write(const Map& m) {
  using detail::escape;
  const auto text = [&](const std::string& t) { return t.empty() ? std::string(":") : ": " + escape(t); };
  std::string out;
  for (const Question& q : m.questions) out += std::string(q.exclusive ? "exclusive " : "") + "question " + q.id + (q.raised_by.empty() ? "" : " <- " + q.raised_by) + text(q.text) + "\n";
  if (!m.questions.empty() && !m.positions.empty()) out += "\n";
  for (const Position& p : m.positions) out += "position " + p.id + (p.answers.empty() ? "" : " -> " + p.answers) + text(p.text) + "\n";
  if (!(m.questions.empty() && m.positions.empty()) && !m.arguments.empty()) out += "\n";
  for (const Argument& a : m.arguments) {
    if (a.bears.empty()) out += "argument " + a.id + text(a.text) + "\n";
    for (std::size_t i = 0; i < a.bears.size(); i++) out += std::string(a.bears[i].polarity == Polarity::pro ? "pro " : "con ") + a.id + " -> " + a.bears[i].target + (i == 0 ? text(a.text) : "") + "\n";
    for (const std::string& p : a.premises) out += "premise " + a.id + text(p) + "\n";
    if (!a.conclusion.empty()) out += "conclusion " + a.id + text(a.conclusion) + "\n";
  }
  return out;
}

// Reads the written form. A line that cannot be read is reported with its number and the reason, and left out; the rest is read.
// It refuses an id stated twice, so that what it reads has distinct ids; every other fault of meaning is left to faults().
inline Reading read(std::string_view source) {
  Reading r;
  if (source.substr(0, 3) == "\xEF\xBB\xBF") source.remove_prefix(3);  // a byte order mark
  std::unordered_map<std::string, std::size_t> declared;   // id → the line that states it
  std::unordered_map<std::string, std::size_t> argument;   // id → its place among the arguments
  std::size_t number = 0;
  while (!source.empty()) {
    const std::size_t end = source.find('\n');
    std::string_view line = detail::trim(source.substr(0, end));
    source.remove_prefix(end == std::string_view::npos ? source.size() : end + 1);
    number++;
    if (line.empty() || line.front() == '#') continue;
    const auto error = [&](const std::string& why) { r.errors.push_back({number, why}); };

    const std::size_t colon = line.find(':');
    const bool has_text = colon != std::string_view::npos;
    std::string text, why;
    if (has_text && !detail::unescape(detail::trim(line.substr(colon + 1)), text, why)) { error(why); continue; }
    std::vector<std::string> words;
    for (std::string_view head = detail::trim(line.substr(0, colon)); !head.empty(); head = detail::trim(head)) {
      std::size_t k = 0;
      while (k < head.size() && head[k] != ' ' && head[k] != '\t') k++;
      words.emplace_back(head.substr(0, k));
      head.remove_prefix(k);
    }
    if (words.empty()) { error("a line begins with what it states: question, position, argument, pro, con, premise or conclusion"); continue; }
    bool exclusive = false;
    if (words[0] == "exclusive") {
      if (words.size() < 2 || words[1] != "question") { error("exclusive is said of a question: exclusive question <id>: <text>"); continue; }
      exclusive = true;
      words.erase(words.begin());
    }
    const std::string keyword = words[0];
    const auto id_at = [&](std::size_t k) { return k < words.size() && detail::valid_id(words[k]); };
    const auto fresh = [&](const std::string& id) {
      const auto it = declared.find(id);
      if (it == declared.end()) return true;
      error("the id " + id + " is already stated on line " + std::to_string(it->second));
      return false;
    };

    if (keyword == "question") {
      const bool plain = words.size() == 2 && id_at(1), raised = words.size() == 4 && id_at(1) && words[2] == "<-" && id_at(3);
      if (!plain && !raised) { error("a question is written: [exclusive] question <id> [<- <id>]: <text>"); continue; }
      if (!fresh(words[1])) continue;
      declared[words[1]] = number;
      r.map.questions.push_back({words[1], text, exclusive, raised ? words[3] : ""});
    } else if (keyword == "position") {
      const bool plain = words.size() == 2 && id_at(1), answers = words.size() == 4 && id_at(1) && words[2] == "->" && id_at(3);
      if (!plain && !answers) { error("a position is written: position <id> -> <id>: <text>"); continue; }
      if (!fresh(words[1])) continue;
      declared[words[1]] = number;
      r.map.positions.push_back({words[1], text, answers ? words[3] : ""});
    } else if (keyword == "argument") {
      if (!(words.size() == 2 && id_at(1))) { error("an argument that bears on nothing yet is written: argument <id>: <text>"); continue; }
      if (!fresh(words[1])) continue;
      declared[words[1]] = number;
      argument[words[1]] = r.map.arguments.size();
      r.map.arguments.push_back({words[1], text, {}, {}, ""});
    } else if (keyword == "pro" || keyword == "con") {
      if (!(words.size() == 4 && id_at(1) && words[2] == "->" && id_at(3))) { error("an argument for or against is written: " + keyword + " <id> -> <id>: <text>"); continue; }
      const Bearing bearing{words[3], keyword == "pro" ? Polarity::pro : Polarity::con};
      if (const auto it = argument.find(words[1]); it != argument.end()) {
        if (!text.empty()) { error("argument " + words[1] + " is stated on line " + std::to_string(declared[words[1]]) + "; a further bearing of it carries no text"); continue; }
        r.map.arguments[it->second].bears.push_back(bearing);
      } else {
        if (!fresh(words[1])) continue;
        declared[words[1]] = number;
        argument[words[1]] = r.map.arguments.size();
        r.map.arguments.push_back({words[1], text, {bearing}, {}, ""});
      }
    } else if (keyword == "premise" || keyword == "conclusion") {
      if (!(words.size() == 2 && id_at(1)) || !has_text) { error("a " + keyword + " is written: " + keyword + " <id>: <text>"); continue; }
      const auto it = argument.find(words[1]);
      if (it == argument.end()) { error("no argument " + words[1] + " is stated above this " + keyword); continue; }
      Argument& a = r.map.arguments[it->second];
      if (keyword == "premise") a.premises.push_back(text);
      else if (!a.conclusion.empty()) error("argument " + words[1] + " already has a conclusion");
      else a.conclusion = text;
    } else {
      error("'" + keyword + "' is not something a line states: question, position, argument, pro, con, premise or conclusion");
    }
  }
  return r;
}

}  // namespace philosophy::arguments
