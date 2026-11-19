// Checks of a negotiation at the table and at home, draft by draft (negotiation.hpp).
//
// - A negotiation's faults, each named.
// - Linkage: two parties who trade two issues at different rates have no zone on either issue alone and one on the two together
//   (Tollison and Willett 1979; Sebenius 1983).
// - Linkage at one rate: on 200 random negotiations whose valuations are exactly opposed at one rate on every issue, with every
//   issue's surplus negative, no package of the linked negotiation is agreeable (the theory note, Proposition 8.4).
// - Two levels: a party ratifying by both of its chambers has a zone inside the one it has ratifying by either; on 100 random
//   negotiations a rule with fewer winning coalitions (unanimity of three against a majority) never makes a package's agreement
//   stronger. A ratifier whose value straddles its reservation leaves ratification arguable, and is named.
// - The drafts in order: what each settles and reopens, the zone emptying, the zone found again when a bracket is reopened.
// - Value claimed: on one issue split in tenths, Nash's solution is the even split; when the first party's parliament will ratify
//   no less than seven tenths, the zone shrinks and the Nash point moves to seven tenths, the smaller win-set claiming more
//   (Schelling 1960; Putnam 1988).
// - A party that accepts no package is named; a wasteful option is dominated; a draft of too many packages is refused.
//
// Every party, issue and value here is invented.

#include <diplomacy/negotiation/negotiation.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace diplomacy::negotiation;
namespace dr = diplomacy::rules;

namespace {

Issue tenths(const std::string& id) {
  Issue is{id, "the first party's share of " + id + " in tenths", {}};
  for (int k = 0; k <= 10; k++) is.options.push_back({std::to_string(k), std::to_string(k) + "/10"});
  return is;
}

Interval at(double x) { return {x, x}; }

// A valuation linear in the tenths: worth a·k + b of option k, with its worth of no agreement.
void linear(Valuation& v, const Issue& is, double a, double b, Interval none) {
  for (int k = 0; k <= 10; k++) v.values[is.id][std::to_string(k)] = at(a * k + b);
  v.none[is.id] = none;
}

// Two parties trading two issues at different rates (each issue alone has no zone; together they have one).
Negotiation linked() {
  Negotiation n;
  n.name = "two issues traded at different rates";
  n.issues = {tenths("X"), tenths("Y")};
  Party a{"A", "the first party", {}, {}, {}}, b{"B", "the second party", {}, {}, {}};
  linear(a.valuation, n.issues[0], 10, 0, at(60));
  linear(a.valuation, n.issues[1], 2, 0, at(10));
  linear(b.valuation, n.issues[0], -2, 20, at(10));
  linear(b.valuation, n.issues[1], -10, 100, at(60));
  n.parties = {a, b};
  return n;
}

dr::Rule by_count(const std::vector<std::string>& members, long long count) {
  dr::Rule r;
  r.id = "ratification";
  r.members = members;
  r.clause.count = count;
  return r;
}

int rank(Standing s) { return s == Standing::clear ? 2 : s == Standing::arguable ? 1 : 0; }

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    s.require("the negotiations here are declared without fault", problems(linked()).empty());
    Negotiation n = linked();
    n.parties[0].valuation.values["X"].erase("3");
    n.parties[1].valuation.values["Y"]["4"] = {5, 1};
    n.parties.push_back({"C", "", {}, {{"senate", "", {}}}, by_count({"house"}, 1)});
    n.drafts.push_back({"d", "", "", {{"X", {"11"}}, {"Z", {"1"}}}});
    const auto ps = problems(n);
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a negotiation's faults are named: a value missing, an interval upside down, ratifiers not the rule's members, a ratifier with no valuation, an option and an issue unknown to a draft",
              has("values no option 3") && has("lower end is above") && has("not its ratifiers") && has("declares no valuation") && has("no option of X") && has("Z is no issue"),
              std::to_string(ps.size()) + " faults");
  }

  // ---- Linkage ----
  {
    const Linkage l = linkage(linked());
    s.require("trading two issues at different rates, neither issue alone has a zone and the two together have one", l.created && l.alone[0].agreeable == 0 && l.alone[1].agreeable == 0 && l.linked == 16,
              std::to_string(l.linked) + " packages agreeable together");
  }
  {
    calx::random::Stream rng(20261119, 134, 2, 0);
    std::size_t bad = 0;
    for (int t = 0; t < 200; t++) {
      Negotiation n;
      Party a{"A", "", {}, {}, {}}, b{"B", "", {}, {}, {}};
      const double k = 1 + static_cast<double>(rng.below(3));
      for (int i = 0; i < 3; i++) {
        Issue is{"I" + std::to_string(i), "", {}};
        const double c = static_cast<double>(rng.below(60));
        for (int o = 0; o < 5; o++) {
          is.options.push_back({std::to_string(o), ""});
          const double va = static_cast<double>(rng.below(21));
          a.valuation.values[is.id][std::to_string(o)] = at(va);
          b.valuation.values[is.id][std::to_string(o)] = at(c - k * va);
        }
        const double na = static_cast<double>(rng.below(21));
        a.valuation.none[is.id] = at(na);
        b.valuation.none[is.id] = at(c - k * na + 1);  // the issue's surplus is −1
        n.issues.push_back(is);
      }
      n.parties = {a, b};
      if (!zone(n).agreeable.empty()) bad++;
    }
    s.require("with valuations opposed at one rate on every issue and every issue's surplus negative, no linked package is agreeable, on 200 random negotiations", bad == 0, std::to_string(bad) + " exceptions");
  }

  // ---- Two levels ----
  {
    Negotiation n = linked();
    Party& a = n.parties[0];
    Ratifier lower{"lower", "the lower chamber", {}}, upper{"upper", "the upper chamber", {}};
    linear(lower.valuation, n.issues[0], 10, 0, at(70));  // wants at least 7 of X
    linear(lower.valuation, n.issues[1], 0, 0, at(0));
    linear(upper.valuation, n.issues[0], 0, 0, at(0));
    linear(upper.valuation, n.issues[1], 10, 0, at(20));  // wants at least 2 of Y
    a.ratifiers = {lower, upper};
    a.ratification = by_count({"lower", "upper"}, 1);
    const Zone either = zone(n);
    n.parties[0].ratification = by_count({"lower", "upper"}, 2);
    const Zone both_ = zone(n);
    const auto moved = difference(either, both_);
    bool only_out = true;
    for (const auto& m : moved) only_out = only_out && rank(m.after) < rank(m.before);
    s.require("ratifying by both chambers, the first party's zone lies inside the one it has ratifying by either", both_.agreeable.size() < either.agreeable.size() && only_out && !moved.empty(),
              std::to_string(either.agreeable.size()) + " packages by either, " + std::to_string(both_.agreeable.size()) + " by both");
  }
  {
    calx::random::Stream rng(20261119, 134, 3, 0);
    std::size_t bad = 0, read = 0;
    for (int t = 0; t < 100; t++) {
      Negotiation n;
      for (int i = 0; i < 2; i++) {
        Issue is{"I" + std::to_string(i), "", {}};
        for (int o = 0; o < 4; o++) is.options.push_back({std::to_string(o), ""});
        n.issues.push_back(is);
      }
      auto random_valuation = [&]() {
        Valuation v;
        for (const auto& is : n.issues) {
          for (const auto& o : is.options) {
            const double lo = static_cast<double>(rng.below(10));
            v.values[is.id][o.id] = {lo, lo + static_cast<double>(rng.below(4))};
          }
          v.none[is.id] = at(static_cast<double>(rng.below(8)));
        }
        return v;
      };
      Party a{"A", "", {}, {}, {}}, b{"B", "", random_valuation(), {}, {}};
      for (const auto* id : {"r1", "r2", "r3"}) a.ratifiers.push_back({id, "", random_valuation()});
      a.ratification = by_count({"r1", "r2", "r3"}, 2);
      n.parties = {a, b};
      const Zone majority = zone(n);
      n.parties[0].ratification = by_count({"r1", "r2", "r3"}, 3);
      const Zone unanimity = zone(n);
      for (const auto& m : difference(majority, unanimity)) bad += rank(m.after) > rank(m.before);
      read++;
    }
    s.require("on 100 random negotiations, ratifying by unanimity instead of a majority never makes a package's agreement stronger", bad == 0, std::to_string(read) + " read");
  }
  {
    Negotiation n = linked();
    Ratifier cabinet{"cabinet", "the cabinet", {}};
    linear(cabinet.valuation, n.issues[0], 10, 0, {65, 85});  // its reservation straddles what some packages are worth
    linear(cabinet.valuation, n.issues[1], 0, 0, at(0));
    n.parties[0].ratifiers = {cabinet};
    n.parties[0].ratification = by_count({"cabinet"}, 1);
    const Answer a = answer(n, n.parties[0], {7, 0}, {0, 1});
    s.require("a ratifier whose value straddles its reservation leaves ratification arguable, and is named", a.standing == Standing::arguable && a.why.find("undecided: the cabinet") != std::string::npos, a.why);
  }

  // ---- The drafts in order ----
  {
    Negotiation n = linked();
    n.drafts = {{"first reading", "2026-03-01", "", {{"X", {"5", "6", "7", "8", "9", "10"}}, {"Y", {"0", "1", "2", "3", "4", "5"}}}},
                {"second reading", "2026-04-01", "", {{"X", {"8"}}, {"Y", {"0", "1", "2", "3", "4", "5"}}}},
                {"chair's text", "2026-05-01", "", {{"X", {"8"}}, {"Y", {"4", "5"}}}},
                {"final text", "2026-06-01", "", {{"X", {"8"}}, {"Y", {"3", "4"}}}}};
    const auto t = trajectory(n);
    s.require("the first reading keeps every agreeable package", t[0].zone.agreeable.size() == 16 && t[0].zone.packages == 36);
    s.require("the second reading settles X on 8, within the zone, and leaves four packages", t[1].settled_now == std::vector<std::string>{"X"} && t[1].notes.empty() && t[1].zone.agreeable.size() == 4,
              t[1].notes.empty() ? "" : t[1].notes[0]);
    s.require("the chair's text brackets out every agreeable package, and the zone is said to have emptied", t[2].zone.agreeable.empty() && !t[2].notes.empty() && t[2].notes[0].find("emptied") != std::string::npos);
    s.require("the final text reopens Y at 3, and the zone is found again", t[3].reopened == std::vector<std::string>{"Y=3"} && t[3].zone.agreeable.size() == 1);
  }

  // ---- Value claimed ----
  {
    Negotiation n;
    n.issues = {tenths("Z")};
    Party a{"A", "", {}, {}, {}}, b{"B", "", {}, {}, {}};
    linear(a.valuation, n.issues[0], 1, 0, at(0));
    linear(b.valuation, n.issues[0], -1, 10, at(0));
    n.parties = {a, b};
    const Zone z = zone(n);
    const Claimed c = claimed(n, z, "A", "B");
    s.require("on one issue split in tenths, Nash's solution is the even split", std::fabs(c.nash.point[0] - 5) < 1e-9 && z.agreeable[c.at_nash].choice[0] == 5,
              std::to_string(c.nash.point[0]) + ", " + std::to_string(c.nash.point[1]));
    Ratifier parliament{"parliament", "the parliament", {}};
    linear(parliament.valuation, n.issues[0], 1, 0, at(7));
    n.parties[0].ratifiers = {parliament};
    n.parties[0].ratification = by_count({"parliament"}, 1);
    const Zone smaller = zone(n);
    const Claimed d = claimed(n, smaller, "A", "B");
    s.require("when its parliament will ratify no less than seven tenths, the first party claims seven tenths: the smaller win-set claims more",
              smaller.agreeable.size() == 4 && std::fabs(d.nash.point[0] - 7) < 1e-9 && smaller.agreeable[d.at_nash].choice[0] == 7, std::to_string(d.nash.point[0]));
    s.require("the basis of what is claimed is said", d.basis.find("midpoints") != std::string::npos);
    Negotiation w = n;
    w.parties[0].ratifiers.clear();
    w.issues[0].options.push_back({"w", "a wasteful split"});
    w.parties[0].valuation.values["Z"]["w"] = at(4);
    w.parties[1].valuation.values["Z"]["w"] = at(4);
    const Zone wz = zone(w);
    bool wasteful_kept = false;
    for (std::size_t k : wz.undominated) wasteful_kept = wasteful_kept || wz.agreeable[k].choice[0] == 11;
    s.require("a wasteful option, worse for both than the even split, is dominated, and the eleven splits are not", wz.dominance_read && !wasteful_kept && wz.undominated.size() == 11,
              std::to_string(wz.undominated.size()) + " undominated");
  }

  // ---- A party that accepts nothing, and a draft too large ----
  {
    Negotiation n = linked();
    Party c{"C", "the third party", {}, {}, {}};
    linear(c.valuation, n.issues[0], 1, 0, at(500));
    linear(c.valuation, n.issues[1], 1, 0, at(500));
    n.parties.push_back(c);
    const Zone z = zone(n);
    s.require("a party that accepts no package is named, and the zone is empty", z.blocking == std::vector<std::string>{"C"} && z.agreeable.empty());
    Negotiation big;
    for (int i = 0; i < 7; i++) big.issues.push_back(tenths("I" + std::to_string(i)));
    Party a{"A", "", {}, {}, {}};
    for (const auto& is : big.issues) linear(a.valuation, is, 1, 0, at(0));
    big.parties = {a};
    bool refused = false;
    try {
      (void)zone(big);
    } catch (const std::invalid_argument&) {
      refused = true;
    }
    s.require("a draft of nineteen million packages is refused, never cut", refused);
  }
  return s.done();
}
