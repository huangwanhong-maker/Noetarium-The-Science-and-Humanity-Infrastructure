// Checks of needs/language: statements of need read by the grammar, written back, and checked against a dataset.
//
// - Each of the four standards read into its parts: the domain, the group, the frame, the indicator, the direction, the level (a
//   percentage read as a share), the reference group, the demand and the provision.
// - write() is the inverse of read() on every statement here, and keys are canonical.
// - Several statements with comments; values quoted with escapes; numbers against categories bound to their levels.
// - A text that is not a statement is refused with its line and column and what may follow; read_each refuses a line alone.
// - check() against a dataset refuses an undeclared domain, a direction the indicator does not have, a level of a share outside
//   (0, 1), an attribute groups may not be stated in, and a group of everyone.

#include <needs/language/reader.hpp>

#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace needs::language;

int main() {
  calx::verification::Suite s;
  {
    const auto st = read_one("need care for age >= 75 and tenure = renting within district in {Hume, \"Moonee Valley\"} by comparative unassisted higher");
    s.require("comparative: the domain", st.domain == "care");
    s.require("comparative: the group, two conditions", st.group.conditions.size() == 2 && st.group.conditions[0].attribute == "age" && st.group.conditions[0].number == 75);
    s.require("comparative: a category's level", st.group.conditions[1].levels == std::vector<std::string>{"renting"});
    s.require("comparative: the frame, a set with a quoted level", st.frame.conditions.size() == 1 && st.frame.conditions[0].op == needs::groups::Op::in &&
                                                                         st.frame.conditions[0].levels == std::vector<std::string>{"Hume", "Moonee Valley"});
    s.require("comparative: the indicator and direction", st.standard == Standard::comparative && st.indicator == "unassisted" && st.worse_when_higher && !st.reference);
  }
  {
    const auto st = read_one("need housing for income < 30000 by normative rent_burden above 30%");
    s.require("normative: a percentage is read as a share", st.standard == Standard::normative && std::fabs(st.level - 0.3) < 1e-15 && st.worse_when_higher);
    const auto f = read_one("need food for household = single by felt food_worry above 0.25");
    s.require("felt: its indicator and level", f.standard == Standard::felt && f.indicator == "food_worry" && f.level == 0.25);
    const auto e = read_one("need care for district = Hume by expressed waiting over places");
    s.require("expressed: demand over provision", e.standard == Standard::expressed && e.indicator == "waiting" && e.provision == "places");
    const auto r = read_one("need mobility for age >= 75 by comparative trips lower than age < 65");
    s.require("comparative with a reference group, and lower", !r.worse_when_higher && r.reference && r.reference->conditions[0].op == needs::groups::Op::lt);
    const auto n = read_one("need care for x > -1.5e2 by comparative y higher");
    s.require("a negative number in exponent form", n.group.conditions[0].number == -150);
  }
  {
    const std::vector<std::string> texts{
        "need care for age >= 75 and tenure = renting within district in {Hume, \"Moonee Valley\"} by comparative unassisted higher",
        "need housing for income < 30000 by normative rent_burden above 0.3",
        "need food for household = single by felt food_worry above 0.25",
        "need care for district = Hume by expressed waiting over places",
        "need mobility for age >= 75 by comparative trips lower than age < 65",
        "need care for name = \"and\" and note = \"say \\\"hi\\\"\" by comparative y higher",
    };
    bool round = true;
    std::string bad;
    for (const auto& t : texts) {
      const auto a = read_one(t);
      const auto b = read_one(a.write());
      if (a.write() != b.write() || a.key() != b.key()) round = false, bad = t;
    }
    s.require("write() reads back to the same statement, for each standard and for quoted values", round, bad);
    const auto a = read_one("need care for age >= 75 and tenure = renting by comparative u higher");
    const auto b = read_one("need care for tenure = renting and age >= 75 by comparative u higher");
    s.require("keys are canonical: the order of conditions does not matter", a.key() == b.key() && a.write() != b.write());
    const auto q = read_one("need care for name = \"and\" and note = \"say \\\"hi\\\"\" by comparative y higher");
    s.require("escapes are read: a quote inside a quoted value", q.group.conditions[1].levels[0] == "say \"hi\"" && q.group.conditions[0].levels[0] == "and");
  }
  {
    const auto all = read("# needs of the old\nneed care for age >= 75 by comparative u higher   # the first\n\nneed care for age >= 85 by comparative u higher; need food for age >= 85 by felt w above 50%\n");
    s.require("a document: three statements, comments and blank lines skipped", all.size() == 3 && all[2].domain == "food");
  }
  {
    try {
      read("need care for age >= 75 by comparative u higher\nneed care for age >> 75 by comparative u higher");
      s.require("an error is refused", false);
    } catch (const ReadError& e) {
      s.require("an error: its line and column", e.line == 2 && e.column == 20, e.what());
      s.require("an error: what may follow is said", std::string(e.what()).find("expected") != std::string::npos, e.what());
    }
    try {
      read_one("need care for age >= 75 by comparative");
      s.require("an incomplete statement is refused", false);
    } catch (const ReadError& e) {
      s.require("an incomplete statement: it ends too soon", std::string(e.what()).find("ends too soon") != std::string::npos, e.what());
    }
    const auto each = read_each("need care for age >= 75 by comparative u higher\nneed care for by comparative u higher\n# a comment\n- need food for age >= 85 by felt w above 50%\n");
    s.require("read_each: three lines tried, the comment skipped", each.size() == 3);
    s.require("read_each: the bad line refused alone, with its column", each[0].read && !each[1].read && each[2].read && each[1].line == 2 && each[1].why.find("column") == 0, each[1].why);
    s.require("read_each: a list's mark is taken off", each[2].statement.domain == "food");
  }
  {
    Dataset d;
    d.table.add(needs::groups::numbers("age", {80, 70, 30}));
    d.table.add(needs::groups::categories("tenure", {"renting", "owner", "renting"}));
    d.table.add(needs::groups::categories("sa1", {"20604112201", "20604112202", "20604112201"}));
    d.table.add(needs::groups::numbers("k", {3, 1, 2}));
    d.table.add(needs::groups::numbers("n", {10, 10, 10}));
    d.table.add(needs::groups::numbers("income", {20000, 50000, 40000}));
    d.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "", "", "care", {}};
    d.indicators["income"] = Indicator{"income", false, "income", "", false, "", "", "income", {}};
    d.domains = {"care", "income"};
    d.attributes = {"age", "tenure", "sa1"};
    auto st = read_one("need care for sa1 = 20604112201 by comparative unassisted higher");
    bind(d, st);
    check(d, st);
    s.require("bind and check: a number against a category is its level", st.group.conditions[0].levels == std::vector<std::string>{"20604112201"});
    auto refuses = [&](const std::string& what, const std::string& text) {
      try {
        auto x = read_one(text);
        bind(d, x);
        check(d, x);
        s.require("refusal: " + what, false);
      } catch (const Refusal& e) {
        s.require("refusal: " + what, true, e.what());
      }
    };
    refuses("an undeclared domain", "need housing for age >= 75 by comparative unassisted higher");
    refuses("a direction the indicator does not have (income is worse when lower)", "need income for age >= 75 by comparative income higher");
    refuses("a share's level outside (0, 1)", "need care for age >= 75 by normative unassisted above 1.5");
    refuses("an attribute groups may not be stated in", "need care for income < 30000 by comparative unassisted higher");
    refuses("a group of everyone", "need care for everyone by comparative unassisted higher");
    refuses("an indicator that is not declared", "need care for age >= 75 by comparative assisted higher");
    refuses("a level that is not in the data", "need care for tenure = lodger by comparative unassisted higher");
    const auto all = read_each("need care for age >= 75 by comparative unassisted higher\nneed care for income < 1 by comparative unassisted higher", &d);
    s.require("read_each against a dataset: the second refused by check, with the reason", all[0].read && !all[1].read && all[1].why.find("not an attribute") != std::string::npos, all[1].why);
  }
  return s.done();
}
