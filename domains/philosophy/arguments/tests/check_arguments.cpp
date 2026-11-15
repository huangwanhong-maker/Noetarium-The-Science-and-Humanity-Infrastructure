// Checks of the argument maps (maps.hpp). The semantics of frameworks beneath them are apodeixis's, with their checks there
// (argumentation/tests/check_argumentation.cpp), where they moved with the frameworks (the infrastructure's ADR 0112).
//
// Maps: each fault found where it is; the framework a hand-made map induces; standings, deciding nodes and open questions; the
// written form, read and written, with its errors line by line; 400 random maps written and read back.


#include <philosophy/arguments/maps.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace pa = philosophy::arguments;
using pa::Set;

namespace {

std::string show(const std::vector<std::string>& ids) {
  std::string t = "[";
  for (std::size_t i = 0; i < ids.size(); i++) t += (i ? " " : "") + ids[i];
  return t + "]";
}

template <class Exception, class Call>
bool throws(Call&& call) {
  try {
    call();
  } catch (const Exception&) {
    return true;
  } catch (...) {
    return false;
  }
  return false;
}

// ---- A map by hand: the Euthyphro (Plato, Euthyphro 10a–13c), in outline -------------------------------------------------------

pa::Map euthyphro() {
  using pa::Polarity;
  pa::Map m;
  m.questions = {
      {"q1", "Is the pious loved by the gods because it is pious, or pious because they love it?", true, ""},
      {"q2", "What is the pious, if it is not what the gods love?", false, "p1"},
  };
  m.positions = {
      {"p1", "It is loved because it is pious.", "q1"},
      {"p2", "It is pious because it is loved.", "q1"},
      {"p3", "The pious is the part of the just that cares for the gods.", "q2"},
  };
  m.arguments = {
      {"a1", "The gods love the pious for being pious, so their love cannot be what makes it pious.", {{"p2", Polarity::con}},
       {"The gods love the pious because it is pious.", "What is loved for being so is not made so by being loved."}, "The pious is not pious because the gods love it."},
      {"a2", "A standard of piety apart from the gods' love would stand above the gods.", {{"a1", Polarity::con}}, {}, ""},
      {"a3", "A love without a reason is arbitrary, and gives no one a reason to be pious.", {{"a2", Polarity::con}}, {}, ""},
      {"a4", "Care betters what it cares for, and the gods are not bettered by us.", {{"p3", Polarity::con}}, {}, ""},
  };
  return m;
}

const pa::Standing& standing_of(const std::vector<pa::Standing>& list, const std::string& id) {
  for (const auto& s : list)
    if (s.id == id) return s;
  throw std::out_of_range("no standing of " + id);
}

}  // namespace

int main() {
  calx::verification::Suite s;
  using Edges = std::vector<std::array<std::size_t, 2>>;

  // ==== Maps: faults ============================================================================================================

  using pa::FaultKind;
  using pa::NodeKind;
  using pa::Polarity;
  const pa::Map E = euthyphro();
  {
    const auto f = pa::faults(E);
    s.require("the hand-made map (the Euthyphro, in outline) is well formed", f.empty() && pa::well_formed(E), f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.positions[2].answers = "";
    const auto f = pa::faults(m);
    s.require("a position that answers no question is found: the third position, p3", f.size() == 1 && f[0].kind == FaultKind::answers_nothing && f[0].node == NodeKind::position && f[0].index == 2 && f[0].id == "p3", f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "p9";
    m.positions[1].answers = "q7";
    m.arguments[3].bears[0].target = "a9";
    const auto f = pa::faults(m);
    s.require("references to ids that are not in the map are found, each where it is: q2 raised by p9, p2 answering q7, a4 against a9",
              f.size() == 3 && f[0].kind == FaultKind::unknown_reference && f[0].node == NodeKind::question && f[0].index == 1 && f[0].involved == std::vector<std::string>{"p9"} && f[1].kind == FaultKind::unknown_reference &&
                  f[1].node == NodeKind::position && f[1].id == "p2" && f[1].involved == std::vector<std::string>{"q7"} && f[2].kind == FaultKind::unknown_reference && f[2].node == NodeKind::argument && f[2].index == 3 &&
                  f[2].involved == std::vector<std::string>{"a9"},
              f.size() == 3 ? f[0].why + " | " + f[1].why + " | " + f[2].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "q1";
    m.positions[2].answers = "a4";
    m.arguments[1].bears[0].target = "q2";
    const auto f = pa::faults(m);
    s.require("references of the wrong kind are found: a question raised by a question, a position answering an argument, an argument on a question",
              f.size() == 3 && f[0].kind == FaultKind::wrong_kind && f[0].id == "q2" && f[1].kind == FaultKind::wrong_kind && f[1].id == "p3" && f[2].kind == FaultKind::wrong_kind && f[2].id == "a2" &&
                  f[2].involved == std::vector<std::string>{"q2"},
              f.size() == 3 ? f[0].why + " | " + f[1].why + " | " + f[2].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m = E;
    m.arguments[2].bears.push_back({"a3", Polarity::pro});
    const auto f = pa::faults(m);
    pa::Map against = E;
    against.arguments[2].bears.push_back({"a3", Polarity::con});
    s.require("an argument for itself is found (a3), and an argument against itself is no fault", f.size() == 1 && f[0].kind == FaultKind::argues_for_itself && f[0].node == NodeKind::argument && f[0].index == 2 && f[0].id == "a3" && pa::well_formed(against),
              f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.arguments[3].bears.clear();
    m.arguments[0].bears.push_back({"p2", Polarity::pro});
    const auto f = pa::faults(m);
    s.require("an argument that bears twice on one target (a1 on p2) and one that bears on nothing (a4) are found",
              f.size() == 2 && f[0].kind == FaultKind::repeated_bearing && f[0].id == "a1" && f[0].involved == std::vector<std::string>{"p2"} && f[1].kind == FaultKind::bears_on_nothing && f[1].id == "a4", f.size() == 2 ? f[0].why + " | " + f[1].why : "");
  }
  {
    pa::Map m = E;
    m.positions[1].id = "q1";   // p2 takes a question's id: what bore on p2 now bears on a question
    const auto f = pa::faults(m);
    s.require("an id used twice is found at its second node (the second position), and so is the reference its old id leaves without a target",
              f.size() == 2 && f[0].kind == FaultKind::duplicate_id && f[0].node == NodeKind::position && f[0].index == 1 && f[0].id == "q1" && f[1].kind == FaultKind::unknown_reference && f[1].id == "a1",
              f.size() == 2 ? f[0].why + " | " + f[1].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m;
    m.questions = {{"", "no id", false, ""}, {"two words", "", false, ""}, {"a:b", "", false, ""}, {"->", "", false, ""}};
    const auto f = pa::faults(m);
    s.require("ids that are empty, of two words, with a colon, or an arrow are found, each at its question",
              f.size() == 4 && f[0].kind == FaultKind::bad_id && f[0].index == 0 && f[1].kind == FaultKind::bad_id && f[1].index == 1 && f[2].kind == FaultKind::bad_id && f[2].index == 2 && f[3].kind == FaultKind::bad_id && f[3].index == 3);
  }
  {
    pa::Map m;
    m.questions = {{"q1", "", false, "a1"}};
    m.positions = {{"p1", "", "q1"}};
    m.arguments = {{"a1", "", {{"p1", Polarity::con}}, {}, ""}};
    const auto f = pa::faults(m);
    s.require("a cycle of 'raised by' is found at its question, with the nodes round it: q1 raised by a1, against p1, which answers q1",
              f.size() == 1 && f[0].kind == FaultKind::raised_by_cycle && f[0].node == NodeKind::question && f[0].id == "q1" && f[0].involved == std::vector<std::string>{"q1", "a1", "p1"}, f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m;
    m.questions = {{"q0", "", false, ""}, {"q1", "", false, "p2"}, {"q2", "", false, "p1"}, {"q3", "", false, "p0"}};
    m.positions = {{"p0", "", "q0"}, {"p1", "", "q1"}, {"p2", "", "q2"}};
    const auto f = pa::faults(m);
    s.require("a cycle through two questions is reported once, at the first: q1 raised by p2, which answers q2, raised by p1, which answers q1; q3, raised by p0, is in none",
              f.size() == 1 && f[0].kind == FaultKind::raised_by_cycle && f[0].index == 1 && f[0].involved == std::vector<std::string>{"q1", "p2", "q2", "p1"}, f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m;
    m.questions = {{"q", "Will Nixon keep the peace?", false, ""}};
    m.positions = {{"p", "He will.", "q"}};
    m.arguments = {{"quaker", "Nixon is a Quaker, and Quakers are pacifists.", {{"p", Polarity::pro}, {"republican", Polarity::con}}, {}, ""},
                   {"republican", "Nixon is a Republican, and Republicans are not pacifists.", {{"p", Polarity::con}, {"quaker", Polarity::con}}, {}, ""}};
    const auto ev = pa::evaluate(m);
    s.require("two arguments against each other are no fault: the Nixon diamond as a map leaves its position undecided and its question open",
              pa::well_formed(m) && ev.positions[0].status == pa::Status::undecided && ev.positions[0].deciding == std::vector<std::string>{"republican"} && ev.open_questions == std::vector<std::string>{"q"} &&
                  ev.arguments[0].status == pa::Status::undecided && ev.arguments[1].status == pa::Status::undecided);
  }
  {
    pa::Map m = E;
    m.positions[2].answers = "";
    m.arguments[0].bears[0].target = "p7";
    m.questions.push_back({"q1", "again", false, ""});
    const auto f = pa::faults(m);
    s.require("several faults are all reported, ids first and then references in the map's order",
              f.size() == 3 && f[0].kind == FaultKind::duplicate_id && f[0].node == NodeKind::question && f[0].index == 2 && f[1].kind == FaultKind::answers_nothing && f[2].kind == FaultKind::unknown_reference && !pa::well_formed(m));
    s.require("a map with faults is not evaluated: induce and evaluate refuse it, naming the first fault",
              throws<std::invalid_argument>([&] { pa::induce(m); }) && throws<std::invalid_argument>([&] { pa::evaluate(m); }) && throws<std::invalid_argument>([&] { pa::open_questions(m); }));
  }

  // ==== Maps: the induced framework, standings, open questions ===================================================================

  using pa::AttackKind;
  using pa::Status;
  {
    const auto ind = pa::induce(E);
    const std::vector<pa::AttackOrigin> origins{{AttackKind::exclusive, "q1", {}}, {AttackKind::exclusive, "q1", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}};
    s.require("the hand-made map induces the framework worked by hand: p1 ⇄ p2 (exclusive), a1 → p2, a2 → a1, a3 → a2, a4 → p3",
              ind.ids == std::vector<std::string>{"p1", "p2", "p3", "a1", "a2", "a3", "a4"} && ind.positions == 3 && ind.framework.size() == 7 &&
                  ind.framework.attack.edges == Edges{{0, 1}, {1, 0}, {3, 1}, {4, 3}, {5, 4}, {6, 2}} && ind.origins == origins && ind.index("a3") == 5 && throws<std::out_of_range>([&] { ind.index("q1"); }));
    const auto ev = pa::evaluate(E);
    const auto& p1 = standing_of(ev.positions, "p1");
    const auto& p2 = standing_of(ev.positions, "p2");
    const auto& p3 = standing_of(ev.positions, "p3");
    s.require("standings: p1 accepted at round 3, defended by a1 against p2; p2 rejected at round 2 by a1; p3 rejected at round 1 by a4",
              p1.status == Status::accepted && p1.round == 3 && p1.attackers == std::vector<std::string>{"p2"} && p1.deciding == std::vector<std::string>{"a1"} && p2.status == Status::rejected && p2.round == 2 &&
                  p2.attackers == std::vector<std::string>{"p1", "a1"} && p2.deciding == std::vector<std::string>{"a1"} && p3.status == Status::rejected && p3.round == 1 && p3.deciding == std::vector<std::string>{"a4"},
              "p1 by " + show(p1.deciding) + ", p2 by " + show(p2.deciding) + ", p3 by " + show(p3.deciding));
    const auto& a1 = standing_of(ev.arguments, "a1");
    const auto& a2 = standing_of(ev.arguments, "a2");
    const auto& a3 = standing_of(ev.arguments, "a3");
    s.require("standings of the arguments: a3 accepted unattacked, a2 rejected by it, a1 reinstated at round 2 with a3 its defender",
              a3.status == Status::accepted && a3.round == 1 && a3.attackers.empty() && a3.deciding.empty() && a2.status == Status::rejected && a2.deciding == std::vector<std::string>{"a3"} && a1.status == Status::accepted && a1.round == 2 &&
                  a1.deciding == std::vector<std::string>{"a3"} && ev.arguments.size() == 4 && ev.grounded.extension == Set{0, 3, 5, 6});
    const std::vector<pa::QuestionStanding> questions{{"q1", {"p1"}, {"p2"}, {}, false, false}, {"q2", {}, {"p3"}, {}, true, false}};
    s.require("the open questions: q2, whose one position is rejected; q1 is answered by p1", ev.open_questions == std::vector<std::string>{"q2"} && ev.questions == questions && pa::open_questions(E) == ev.open_questions && pa::standings(E) == ev.positions);
  }
  {
    pa::Map m = E;
    m.arguments.erase(m.arguments.begin() + 2);   // without a3, a2 stands and defeats a1
    const auto ev = pa::evaluate(m);
    const auto& p1 = standing_of(ev.positions, "p1");
    const auto& p2 = standing_of(ev.positions, "p2");
    s.require("without a3 the argument a1 is defeated, the two exclusive positions leave each other undecided, and both questions are open",
              standing_of(ev.arguments, "a1").status == Status::rejected && p1.status == Status::undecided && p1.round == 0 && p1.deciding == std::vector<std::string>{"p2"} && p2.status == Status::undecided &&
                  p2.deciding == std::vector<std::string>{"p1"} && ev.open_questions == std::vector<std::string>{"q1", "q2"} && ev.questions[0].undecided == std::vector<std::string>{"p1", "p2"});
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "p2";
    const auto ev = pa::evaluate(m);
    s.require("a question raised by a rejected position is reported moot", ev.questions[1].moot && !ev.questions[0].moot && ev.questions[1].open);
  }
  {
    pa::Map tie;
    tie.questions = {{"q", "", true, ""}, {"unanswered", "", false, ""}};
    tie.positions = {{"p1", "", "q"}, {"p2", "", "q"}};
    const auto ev = pa::evaluate(tie);
    s.require("two exclusive positions with nothing said on them are both undecided; a question with no position is open",
              ev.positions[0].status == Status::undecided && ev.positions[1].status == Status::undecided && ev.open_questions == std::vector<std::string>{"q", "unanswered"});
    pa::Map plural = tie;
    plural.questions[0].exclusive = false;
    const auto ev2 = pa::evaluate(plural);
    s.require("when the question is not exclusive both positions are accepted and it is not open",
              ev2.induced.framework.attack.edges.empty() && ev2.positions[0].status == Status::accepted && ev2.positions[1].status == Status::accepted && ev2.open_questions == std::vector<std::string>{"unanswered"});

    pa::Map one = tie;
    one.arguments = {{"a1", "", {{"p1", Polarity::pro}}, {}, ""}};
    const auto ind = pa::induce(one);
    const auto ev3 = pa::evaluate(one);
    s.require("an argument for a position counts against its exclusive rival, by the supported attack a1 → p2 through p1; p1 is then accepted with a1 its defender",
              ind.framework.attack.edges == Edges{{0, 1}, {1, 0}, {2, 1}} && ind.origins[2] == pa::AttackOrigin{AttackKind::supported, "", {"a1", "p1"}} && ev3.positions[0].status == Status::accepted &&
                  ev3.positions[0].deciding == std::vector<std::string>{"a1"} && ev3.positions[1].status == Status::rejected && ev3.positions[1].deciding == std::vector<std::string>{"a1"} && ev3.open_questions == std::vector<std::string>{"unanswered"});
    const auto ev4 = pa::evaluate(one, {.supported_attacks = false});
    s.require("read without supported attacks, the argument for p1 plays no part and the tie stands", ev4.induced.framework.attack.edges == Edges{{0, 1}, {1, 0}} && ev4.positions[0].status == Status::undecided && ev4.positions[1].status == Status::undecided);

    pa::Map both = one;
    both.arguments.push_back({"a2", "", {{"p2", Polarity::pro}}, {}, ""});
    const auto ev5 = pa::evaluate(both);
    const auto ev6 = pa::evaluate(both, {.supported_attacks = false});
    s.require("the caveat: exclusive rivals each with an unanswered argument for it are both rejected, each by the other's argument; without supported attacks both are undecided; the question is open either way",
              ev5.positions[0].status == Status::rejected && ev5.positions[0].deciding == std::vector<std::string>{"a2"} && ev5.positions[1].status == Status::rejected && ev5.positions[1].deciding == std::vector<std::string>{"a1"} &&
                  ev5.open_questions == std::vector<std::string>{"q", "unanswered"} && ev6.positions[0].status == Status::undecided && ev6.open_questions == ev5.open_questions);
    both.arguments.push_back({"a3", "", {{"a2", Polarity::con}}, {}, ""});
    const auto ev7 = pa::evaluate(both);
    s.require("once the rival's argument is answered, the position is accepted", ev7.positions[0].status == Status::accepted && ev7.positions[0].round == 2 && ev7.positions[1].status == Status::rejected && ev7.open_questions == std::vector<std::string>{"unanswered"});
  }
  {
    pa::Map m;
    m.questions = {{"q", "", false, ""}};
    m.positions = {{"p", "", "q"}};
    m.arguments = {{"liar", "This argument is unsound.", {{"liar", Polarity::con}, {"p", Polarity::con}}, {}, ""}};
    const auto ev = pa::evaluate(m);
    s.require("a self-defeating argument against a position leaves the position undecided", pa::well_formed(m) && ev.arguments[0].status == Status::undecided && ev.positions[0].status == Status::undecided && ev.positions[0].deciding == std::vector<std::string>{"liar"});
    pa::Map chain;
    chain.questions = {{"q", "", false, ""}};
    chain.positions = {{"p", "", "q"}};
    chain.arguments = {{"a1", "", {{"p", Polarity::con}}, {}, ""}, {"a2", "", {{"a1", Polarity::pro}}, {}, ""}, {"a3", "", {{"a1", Polarity::con}}, {}, ""}};
    const auto ind = pa::induce(chain);
    const auto ev2 = pa::evaluate(chain);
    s.require("an argument for an argument against p attacks p (a2 through a1), and still does when a1 itself is defeated",
              ind.framework.attack.edges == Edges{{1, 0}, {3, 1}, {2, 0}} && ind.origins[2] == pa::AttackOrigin{AttackKind::supported, "", {"a2", "a1"}} && standing_of(ev2.arguments, "a1").status == Status::rejected &&
                  ev2.positions[0].status == Status::rejected && ev2.positions[0].deciding == std::vector<std::string>{"a2"});
  }

  // ==== Maps: the written form ==================================================================================================

  {
    const std::string expected =
        "exclusive question q1: Is the pious loved by the gods because it is pious, or pious because they love it?\n"
        "question q2 <- p1: What is the pious, if it is not what the gods love?\n"
        "\n"
        "position p1 -> q1: It is loved because it is pious.\n"
        "position p2 -> q1: It is pious because it is loved.\n"
        "position p3 -> q2: The pious is the part of the just that cares for the gods.\n"
        "\n"
        "con a1 -> p2: The gods love the pious for being pious, so their love cannot be what makes it pious.\n"
        "premise a1: The gods love the pious because it is pious.\n"
        "premise a1: What is loved for being so is not made so by being loved.\n"
        "conclusion a1: The pious is not pious because the gods love it.\n"
        "con a2 -> a1: A standard of piety apart from the gods' love would stand above the gods.\n"
        "con a3 -> a2: A love without a reason is arbitrary, and gives no one a reason to be pious.\n"
        "con a4 -> p3: Care betters what it cares for, and the gods are not bettered by us.\n";
    const std::string written = pa::write(E);
    s.require("the hand-made map is written as the lines expected", written == expected);
    const auto back = pa::read(written);
    s.require("what is written is read back as the same map", back.ok() && back.map == E, std::to_string(back.errors.size()) + " errors");
  }
  {
    const std::string text =
        "\xEF\xBB\xBF# a map written by hand, out of order, with Windows line ends\r\n"
        "\r\n"
        "pro a1 -> p1: Each is owed what is theirs.\r\n"
        "  position p1 -> q1:   Justice is giving each their due.  \r\n"
        "exclusive question q1: What is justice?\r\n"
        "position p2 -> q1: Justice is the interest of the stronger.\r\n"
        "\t# a further bearing carries no text\r\n"
        "con a1 -> p2\r\n"
        "premise a1: A due is something owed.\r\n"
        "argument a2\r\n"
        "question q2 <- p2:\r\n"
        "conclusion a1: So justice gives each their due: no more.";
    const auto r = pa::read(text);
    pa::Map m;
    m.questions = {{"q1", "What is justice?", true, ""}, {"q2", "", false, "p2"}};
    m.positions = {{"p1", "Justice is giving each their due.", "q1"}, {"p2", "Justice is the interest of the stronger.", "q1"}};
    m.arguments = {{"a1", "Each is owed what is theirs.", {{"p1", Polarity::pro}, {"p2", Polarity::con}}, {"A due is something owed."}, "So justice gives each their due: no more."}, {"a2", "", {}, {}, ""}};
    s.require("a text written by hand is read: remarks, empty lines, a byte order mark, CR LF, statements in any order, a further bearing, a colon inside a text",
              r.ok() && r.map == m, r.ok() ? "" : "line " + std::to_string(r.errors[0].line) + ": " + r.errors[0].why);
    const auto f = pa::faults(r.map);
    s.require("the reader reads syntax only: the argument that bears on nothing is a fault for faults() to find", f.size() == 1 && f[0].kind == FaultKind::bears_on_nothing && f[0].id == "a2");
  }
  {
    pa::Map m;
    m.questions = {{"q.1", "  two spaces before, a tab after\t", false, ""}, {"q-2", "", true, "p_1"}};
    m.positions = {{"p_1", "a colon: a hash # an arrow -> a backslash \\ and \\n spelt out", "q.1"}, {"p2", "line one\nline two\r\n", ""}};
    m.arguments = {{"a/1", "naïve — 素朴 — αληθεια", {{"p_1", Polarity::con}, {"a/1", Polarity::con}, {"p2", Polarity::pro}}, {"", " ", "\\s"}, " "}, {"a2", " ", {}, {"x"}, ""}};
    const auto r = pa::read(pa::write(m));
    s.require("texts with blanks at their ends, tabs, new lines, backslashes, colons and letters beyond ASCII are written and read back unchanged; so are a position with no question and an argument with no bearing",
              r.ok() && r.map == m, r.ok() ? "" : "line " + std::to_string(r.errors[0].line) + ": " + r.errors[0].why);
  }
  {
    const std::string text =
        "question q1: a question\n"            // 1
        "issue q9: not a keyword\n"            // 2
        "position p1 -> q1: a position\n"      // 3
        "position p1 -> q1: again\n"           // 4
        "position -> q1: no id\n"              // 5
        "premise a1: before its argument\n"    // 6
        "pro a1 -> p1: an argument\n"          // 7
        "con a1 -> p1: text on a further bearing\n"  // 8
        "conclusion a1: one\n"                 // 9
        "conclusion a1: two\n"                 // 10
        "premise a1\n"                         // 11
        "pro a2 -> p1: a bad escape \\q\n"     // 12
        "exclusive position p2 -> q1: only questions\n"  // 13
        "question q2 <- : raised by nothing\n" // 14
        ": a text with no head\n"              // 15
        "con q1 -> p1\n"                       // 16
        "pro a3 p1: no arrow\n"                // 17
        "con a4 -> p1: ends in a backslash \\\n"   // 18
        "con a5 -> p1: the last line is read\n";   // 19
    const auto r = pa::read(text);
    std::vector<std::size_t> lines;
    for (const auto& e : r.errors) lines.push_back(e.line);
    const auto why = [&](std::size_t line) {
      for (const auto& e : r.errors)
        if (e.line == line) return e.why;
      return std::string();
    };
    s.require("lines that cannot be read are reported, each with its number", lines == std::vector<std::size_t>{2, 4, 5, 6, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18}, std::to_string(lines.size()) + " errors");
    s.require("and with its reason: an unknown keyword, an id stated twice and where, a premise before its argument, text on a further bearing, a second conclusion, an escape that is none",
              why(2).find("'issue'") != std::string::npos && why(4).find("already stated on line 3") != std::string::npos && why(6).find("no argument a1") != std::string::npos && why(8).find("stated on line 7") != std::string::npos &&
                  why(10).find("already has a conclusion") != std::string::npos && why(12).find("\\q is not an escape") != std::string::npos && why(16).find("already stated on line 1") != std::string::npos &&
                  why(18).find("lone backslash") != std::string::npos,
              why(4) + " | " + why(8));
    pa::Map m;
    m.questions = {{"q1", "a question", false, ""}};
    m.positions = {{"p1", "a position", "q1"}};
    m.arguments = {{"a1", "an argument", {{"p1", Polarity::pro}}, {}, "one"}, {"a5", "the last line is read", {{"p1", Polarity::con}}, {}, ""}};
    s.require("the lines that can be read are read all the same", r.map == m && !r.ok());
  }
  {  // Random maps: written, read back, and where well formed evaluated.
    calx::random::Stream rng(20261113, 2, 0, 0);
    const std::string alphabet[] = {"a", "b", " ", " ", "\\", "\n", "\t", ":", "#", "->", "é", "s", "n", "\r"};
    const auto text = [&] {
      std::string t;
      for (std::uint64_t k = rng.below(8); k > 0; k--) t += alphabet[rng.below(std::size(alphabet))];
      return t;
    };
    int trials = 400, round_trip_fails = 0, well_formed = 0, inconsistent = 0;
    for (int t = 0; t < trials; t++) {
      pa::Map m;
      const std::size_t Q = 1 + rng.below(4), P = rng.below(6), A = rng.below(7);
      const auto node = [&]() -> std::string {  // a position or an argument, if there is one
        if (P + A == 0) return "";
        const std::size_t v = rng.below(P + A);
        return v < P ? "p" + std::to_string(v) : "a" + std::to_string(v - P);
      };
      for (std::size_t i = 0; i < Q; i++) m.questions.push_back({"q" + std::to_string(i), text(), rng.below(2) == 0, rng.below(4) == 0 ? node() : ""});
      for (std::size_t i = 0; i < P; i++) m.positions.push_back({"p" + std::to_string(i), text(), "q" + std::to_string(rng.below(Q))});
      for (std::size_t i = 0; i < A; i++) {
        pa::Argument a{"a" + std::to_string(i), text(), {}, {}, rng.below(3) == 0 ? text() : ""};
        for (std::uint64_t k = 1 + rng.below(3); k > 0; k--) {
          const std::string target = node();
          bool seen = false;
          for (const auto& b : a.bears) seen = seen || b.target == target;
          if (!seen) a.bears.push_back({target, rng.below(2) == 0 ? Polarity::pro : Polarity::con});
        }
        for (std::uint64_t k = rng.below(3); k > 0; k--) a.premises.push_back(text());
        m.arguments.push_back(std::move(a));
      }
      const auto r = pa::read(pa::write(m));
      if (!r.ok() || !(r.map == m)) round_trip_fails++;
      if (!pa::well_formed(m)) continue;
      well_formed++;
      // An evaluation hangs together: the accepted are the grounded extension, its labelling is complete, a rejected node has a
      // deciding attacker and an accepted one defenders against every attacker, and a question is open exactly when none of its
      // positions is accepted.
      const auto ev = pa::evaluate(m);
      bool fine = pa::complete_labelling(ev.induced.framework, ev.grounded.label) && ev.positions.size() == P && ev.arguments.size() == A && ev.induced.origins.size() == ev.induced.framework.attack.edges.size();
      std::vector<pa::Standing> all = ev.positions;
      all.insert(all.end(), ev.arguments.begin(), ev.arguments.end());
      Set accepted;
      for (std::size_t v = 0; v < all.size(); v++) {
        if (all[v].status == Status::accepted) accepted.push_back(v);
        fine = fine && all[v].id == ev.induced.ids[v] && (all[v].status == Status::undecided) == (all[v].round == 0);
        if (all[v].status == Status::rejected) fine = fine && !all[v].deciding.empty();
        if (all[v].status == Status::undecided) fine = fine && !all[v].deciding.empty();
        if (all[v].status == Status::accepted) {
          Set defenders;
          for (const auto& id : all[v].deciding) defenders.push_back(ev.induced.index(id));
          std::sort(defenders.begin(), defenders.end());
          fine = fine && pa::defends(ev.induced.framework, defenders, v) && (all[v].attackers.empty() == all[v].deciding.empty());
        }
      }
      fine = fine && accepted == ev.grounded.extension;
      for (std::size_t i = 0; i < Q; i++) {
        bool some = false;
        for (std::size_t j = 0; j < P; j++) some = some || (m.positions[j].answers == m.questions[i].id && ev.positions[j].status == Status::accepted);
        const bool open = std::find(ev.open_questions.begin(), ev.open_questions.end(), m.questions[i].id) != ev.open_questions.end();
        fine = fine && open == !some && ev.questions[i].open == open;
      }
      if (!fine) inconsistent++;
    }
    s.require("400 random maps, faulty ones among them, are written and read back unchanged", round_trip_fails == 0, std::to_string(round_trip_fails) + " differ");
    s.require("the well-formed among them evaluate consistently: the accepted are the grounded extension, each standing has its deciding nodes, the defenders named defend, the open questions are those with no accepted position",
              inconsistent == 0 && well_formed > 100, std::to_string(well_formed) + " well formed");
  }

  return s.done();
}
