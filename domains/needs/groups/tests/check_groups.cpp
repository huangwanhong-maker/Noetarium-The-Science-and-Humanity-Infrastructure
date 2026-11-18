// Checks of needs/groups: tables and groups of units, worked by hand.
//
// - Columns of numbers and categories; missing values belong to no group stated in their attribute.
// - The six relations and membership; a group's rows within a frame; canonical keys independent of the order of conditions.
// - subsumes, join and meet; weighted Jaccard distances by hand.
// - Values that would not read back (reserved words, spaces, numbers as levels) are quoted; numbers written against a category are
//   bound to the level they spell.
// - Refusals: an attribute that is not there, a relation that does not fit, a level that does not exist, columns of unequal length.

#include <needs/groups/group.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>
#include <vector>

using namespace needs::groups;

int main() {
  calx::verification::Suite s;
  const double na = std::numeric_limits<double>::quiet_NaN();
  Table t;
  t.add(numbers("age", {80, 70, 30, na, 90, 45}));
  t.add(categories("tenure", {"renting", "owner", "renting", "social", "", "owner"}));
  t.add(categories("district", {"Hume", "Hume", "Wyndham", "Casey", "Hume", "Wyndham"}));
  t.set_weight({10, 20, 30, 40, 50, 60});

  auto group = [](std::vector<Condition> cs) {
    Group g;
    g.conditions = std::move(cs);
    return g;
  };
  Condition old{"age", Op::ge, 75, {}, ""}, renting{"tenure", Op::eq, 0, {"renting"}, ""};
  Condition outer{"district", Op::in, 0, {"Hume", "Casey"}, ""};

  {
    const Rows r = rows_of(t, group({old}));
    s.require("age >= 75 holds for rows 1 and 5; a missing age for none", r == Rows{1, 0, 0, 0, 1, 0});
    const Rows q = rows_of(t, group({old, renting}));
    s.require("age >= 75 and tenure = renting: row 1 alone (row 5's tenure is missing)", q == Rows{1, 0, 0, 0, 0, 0});
    const Rows in = rows_of(t, group({outer}));
    s.require("district in {Hume, Casey}: rows 1, 2, 4, 5", in == Rows{1, 1, 0, 1, 1, 0});
    const Rows within = rows_of(t, group({renting}), in);
    s.require("tenure = renting within district in {Hume, Casey}: row 1", within == Rows{1, 0, 0, 0, 0, 0});
    s.check("weight of age >= 75: 10 + 50", weight_of(t, r) - 60, 0);
    s.require("everyone: every row", rows_of(t, Group{}) == Rows(6, 1));
    for (Op op : {Op::lt, Op::le, Op::gt, Op::ge, Op::eq, Op::ne}) {
      Condition c{"age", op, 70, {}, ""};
      const Rows x = rows_of(t, group({c}));
      std::size_t expected = 0;
      for (double a : {80.0, 70.0, 30.0, 90.0, 45.0}) {
        const bool h = op == Op::lt ? a < 70 : op == Op::le ? a <= 70 : op == Op::gt ? a > 70 : op == Op::ge ? a >= 70 : op == Op::eq ? a == 70 : a != 70;
        expected += h;
      }
      s.require(std::string("age ") + text(op) + " 70 counts as by hand, the missing age in none", count(x) == expected);
    }
  }
  {
    const Group a = group({old, renting}), b = group({renting, old});
    s.require("keys do not depend on the order of conditions", a.key() == b.key() && a.write() != b.write());
    s.require("everyone's key", Group{}.key() == "everyone");
    s.require("a group of fewer conditions subsumes one of more", subsumes(group({old}), a) && !subsumes(a, group({old})));
    s.require("join: each condition once", join(a, group({old, outer})).size() == 3);
    s.require("meet: the shared conditions", meet(a, group({old, outer})).key() == group({old}).key());
    // Jaccard: rows {1, 5} against {1, 2, 4, 5}: both 10 + 50, either 10 + 20 + 40 + 50.
    s.check("weighted Jaccard distance by hand: 1 − 60/120", jaccard_distance(t, rows_of(t, group({old})), rows_of(t, group({outer}))) - 0.5, 1e-15);
    s.check("two empty sets are at distance 0", jaccard_distance(t, Rows(6, 0), Rows(6, 0)), 0);
  }
  {
    s.require("numbers written as a person would: 20, 0.3, 75, -1.5", number_text(20) == "20" && number_text(0.3) == "0.3" && number_text(75) == "75" && number_text(-1.5) == "-1.5");
    s.require("numbers read back exactly: 34122.89808 and 0.1 + 0.2", std::strtod(number_text(34122.89808).c_str(), nullptr) == 34122.89808 &&
                                                                             std::strtod(number_text(0.1 + 0.2).c_str(), nullptr) == 0.1 + 0.2);
    s.require("the very small keep an exponent", number_text(1e-7).find('e') != std::string::npos);
    s.require("a plain level is written plain", Condition::quoted("renting") == "renting");
    s.require("a reserved word is quoted", Condition::quoted("and") == "\"and\"" && Condition::quoted("by") == "\"by\"");
    s.require("a level with a space is quoted", Condition::quoted("Hume City") == "\"Hume City\"");
    s.require("a level that reads as a number is quoted", Condition::quoted("206041122") == "\"206041122\"");
    s.require("quotes and backslashes are escaped", Condition::quoted("a\"b\\c") == "\"a\\\"b\\\\c\"");
    Table u;
    u.add(categories("sa1", {"20604112201", "20604112202"}));
    Condition c{"sa1", Op::eq, 20604112201.0, {}, "20604112201"};
    bind(u, c);
    s.require("bind: a number written against a category is the level it spells", c.levels == std::vector<std::string>{"20604112201"} && c.literal.empty());
    s.require("bind: and then holds as that level", rows_of(u, group({c})) == Rows{1, 0});
    Condition set{"sa1", Op::in, 0, {"20604112202", "20604112201", "20604112202"}, ""};
    bind(u, set);
    s.require("bind: a set's levels sorted, each once", set.levels == std::vector<std::string>{"20604112201", "20604112202"});
  }
  {
    auto refuses = [&](const std::string& what, auto f) {
      try {
        f();
        s.require("refusal: " + what, false);
      } catch (const Refusal& e) {
        s.require("refusal: " + what, true, e.what());
      }
    };
    refuses("an attribute that is not there", [&] { rows_of(t, group({Condition{"income", Op::ge, 1, {}, ""}})); });
    refuses("membership in a set of numbers", [&] { rows_of(t, group({Condition{"age", Op::in, 0, {"1"}, ""}})); });
    refuses("an order on a category", [&] { rows_of(t, group({Condition{"tenure", Op::lt, 0, {"owner"}, ""}})); });
    refuses("a level that does not exist", [&] { rows_of(t, group({Condition{"tenure", Op::eq, 0, {"lodger"}, ""}})); });
    refuses("a column of another length", [&] { t.add(numbers("x", {1, 2})); });
    refuses("a column named twice", [&] { t.add(numbers("age", {1, 2, 3, 4, 5, 6})); });
    refuses("a negative weight", [&] { t.set_weight({1, 1, 1, 1, 1, -1}); });
  }
  return s.done();
}
