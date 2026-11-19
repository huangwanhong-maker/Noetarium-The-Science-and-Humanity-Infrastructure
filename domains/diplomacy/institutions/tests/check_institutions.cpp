// Checks of institutions, memberships, complexes and reciprocity (institutions.hpp).
//
// - A landscape's faults, each named.
// - The lattice of memberships: on 100 random landscapes its groups are exactly the closed sets found by brute force over every set
//   of institutions.
// - The regularities: the canonical basis says that every party belongs to the universal body and that every member of the
//   subregional body belongs to the regional one; held sound and complete, every nesting found by brute force following from it.
// - A regime complex on one issue: nested, overlapping and apart told apart, and the parties that can choose their forum; when a
//   party leaves, a nesting becomes an overlap, and the regularity goes with it.
// - Reciprocity: a cycle of equal gifts is diffusely reciprocal and not specifically at all; gifts alternating month by month
//   balance over two months and not over one; on 200 random sets of transfers the group's imbalance never exceeds the pairs' (the
//   theory note, Proposition 10.4: specific reciprocity implies diffuse).
//
// Every party and institution here is invented.

#include <diplomacy/institutions/institutions.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <set>
#include <string>
#include <vector>

using namespace diplomacy::institutions;

namespace {

Landscape landscape() {
  Landscape l;
  l.parties = {"a", "b", "c", "d", "e", "f"};
  l.institutions = {{"UNI", "a universal body", "organisation", {"trade", "health"}, {{"membership", "open"}}},
                    {"REG", "a regional body", "organisation", {"trade"}, {{"membership", "regional"}, {"centralisation", "a secretariat"}}},
                    {"SUB", "a subregional body", "organisation", {"trade"}, {}},
                    {"CLUB", "a club", "forum", {"trade"}, {{"flexibility", "no treaty"}}},
                    {"HEALTH", "a health partnership", "partnership", {"health"}, {}}};
  for (const auto& p : l.parties) l.memberships.push_back({p, "UNI", "2000-01-01", ""});
  for (const auto& p : {"a", "c", "d"}) l.memberships.push_back({p, "REG", "2000-01-01", ""});
  l.memberships.push_back({"b", "REG", "2000-01-01", "2025-01-01"});
  for (const auto& p : {"a", "b"}) l.memberships.push_back({p, "SUB", "2010-01-01", ""});
  for (const auto& p : {"a", "c", "e"}) l.memberships.push_back({p, "CLUB", "2015-01-01", ""});
  for (const auto& p : {"e", "f"}) l.memberships.push_back({p, "HEALTH", "2018-01-01", ""});
  return l;
}

// Whether a regularity says "every member of SUB is a member of REG", the universal body aside.
bool sub_in_reg(const Regularity& r) {
  std::vector<std::string> p;
  for (const auto& x : r.premise)
    if (x != "UNI") p.push_back(x);
  return p == std::vector<std::string>{"SUB"} && std::find(r.conclusion.begin(), r.conclusion.end(), "REG") != r.conclusion.end();
}

// Every closed set of institutions, by brute force.
std::set<std::vector<std::size_t>> closed(const morphe::combinatorics::Context& c) {
  std::set<std::vector<std::size_t>> out;
  for (std::uint32_t B = 0; B < (1u << c.attributes); B++) {
    std::vector<std::size_t> objs;
    for (std::size_t o = 0; o < c.objects; o++) {
      bool all = true;
      for (std::size_t a = 0; a < c.attributes; a++)
        if ((B >> a & 1u) && !c.incidence[o][a]) all = false;
      if (all) objs.push_back(o);
    }
    std::vector<std::size_t> close;
    for (std::size_t a = 0; a < c.attributes; a++) {
      bool all = true;
      for (std::size_t o : objs) all = all && c.incidence[o][a];
      if (all) close.push_back(a);
    }
    out.insert(close);
  }
  return out;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    s.require("the landscape here is declared without fault", problems(landscape()).empty());
    Landscape l = landscape();
    l.memberships.push_back({"z", "NOWHERE", "2020-13-01", ""});
    l.institutions[0].design["temper"] = "mild";
    const auto ps = problems(l);
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a landscape's faults are named: a party and an institution unknown, a date that is none, a dimension of design that is none",
              has("z is not a party") && has("NOWHERE is not an institution") && has("not a calendar date") && has("temper is not a dimension"), std::to_string(ps.size()) + " faults");
  }

  // ---- The lattice ----
  {
    calx::random::Stream rng(20261119, 134, 4, 0);
    std::size_t bad = 0;
    for (int t = 0; t < 100; t++) {
      Landscape l;
      for (int p = 0; p < 6; p++) l.parties.push_back("p" + std::to_string(p));
      for (int i = 0; i < 5; i++) l.institutions.push_back({"i" + std::to_string(i), "", "", {}, {}});
      for (const auto& p : l.parties)
        for (const auto& i : l.institutions)
          if (rng.below(2)) l.memberships.push_back({p, i.id, "", ""});
      const auto c = context(l);
      const auto truth = closed(c);
      const Lattice L = lattice(l);
      std::set<std::vector<std::size_t>> found;
      for (const auto& g : L.groups) {
        std::vector<std::size_t> v;
        for (const auto& name : g.institutions) v.push_back(static_cast<std::size_t>(name[1] - '0'));
        std::sort(v.begin(), v.end());
        found.insert(v);
      }
      if (found != truth || L.groups.size() != truth.size()) bad++;
    }
    s.require("on 100 random landscapes the lattice's groups are exactly the closed sets of institutions", bad == 0, std::to_string(bad) + " exceptions");
  }

  // ---- Regularities ----
  {
    const Landscape l = landscape();
    const auto rs = regularities(l, "2024-06-01");
    bool universal = false, nested = false;
    for (const auto& r : rs) {
      universal = universal || (r.premise.empty() && r.conclusion == std::vector<std::string>{"UNI"});
      nested = nested || sub_in_reg(r);
    }
    s.require("in 2024 every party belongs to the universal body, and every member of the subregional body to the regional one", universal && nested, std::to_string(rs.size()) + " regularities");
    // Sound and complete: a nesting holds exactly when it follows from the basis.
    const auto c = context(l, "2024-06-01");
    std::vector<morphe::combinatorics::Implication> base;
    auto index = [&](const std::string& id) {
      for (std::size_t k = 0; k < l.institutions.size(); k++)
        if (l.institutions[k].id == id) return k;
      return std::size_t{0};
    };
    for (const auto& r : rs) {
      morphe::combinatorics::Implication imp;
      for (const auto& x : r.premise) imp.premise.push_back(index(x));
      imp.conclusion = imp.premise;
      for (const auto& x : r.conclusion) imp.conclusion.push_back(index(x));
      std::sort(imp.premise.begin(), imp.premise.end());
      std::sort(imp.conclusion.begin(), imp.conclusion.end());
      base.push_back(imp);
    }
    std::size_t bad = 0, held = 0;
    for (std::size_t a = 0; a < l.institutions.size(); a++)
      for (std::size_t b = 0; b < l.institutions.size(); b++) {
        if (a == b) continue;
        bool truth = true;
        for (std::size_t o = 0; o < c.objects; o++)
          if (c.incidence[o][a] && !c.incidence[o][b]) truth = false;
        const auto close = morphe::combinatorics::closure_under(base, {a}, l.institutions.size());
        const bool follows = std::find(close.begin(), close.end(), b) != close.end();
        bad += truth != follows, held += truth;
      }
    s.require("the basis is sound and complete: every nesting found by brute force follows from it, and nothing else does", bad == 0, std::to_string(held) + " nestings");
  }

  // ---- A regime complex ----
  {
    const Landscape l = landscape();
    const Complex c = complex(l, "trade", "2024-06-01");
    auto rel = [&](const Complex& x, const std::string& a, const std::string& b) {
      for (const auto& p : x.pairs)
        if (p.a == a && p.b == b) return p;
      return Pair{};
    };
    s.require("on trade: the subregional body nested in the regional one, the club overlapping both, all nested in the universal body",
              rel(c, "REG", "SUB").relation == Relation::nested && rel(c, "REG", "SUB").inner == "SUB" && rel(c, "REG", "CLUB").relation == Relation::overlapping &&
                  rel(c, "REG", "CLUB").shared == 2 && rel(c, "SUB", "CLUB").relation == Relation::overlapping && rel(c, "UNI", "CLUB").relation == Relation::nested);
    s.require("five parties can choose their forum on trade, the first among four bodies", c.forums.size() == 5 && c.forums[0].first == "a" && c.forums[0].second.size() == 4);
    const Complex later = complex(l, "trade", "2025-06-01");
    s.require("when the second party leaves the regional body, the subregional body overlaps it instead of nesting in it",
              rel(later, "REG", "SUB").relation == Relation::overlapping && rel(later, "REG", "SUB").shared == 1);
    bool still = false;
    for (const auto& r : regularities(l, "2025-06-01"))
      still = still || sub_in_reg(r);
    bool joint = false;
    for (const auto& r : regularities(l, "2025-06-01")) {
      const bool sub = std::find(r.premise.begin(), r.premise.end(), "SUB") != r.premise.end(), club = std::find(r.premise.begin(), r.premise.end(), "CLUB") != r.premise.end();
      joint = joint || (sub && club && std::find(r.conclusion.begin(), r.conclusion.end(), "REG") != r.conclusion.end());
    }
    s.require("and the nesting goes with it, leaving a joint condition: every member of both the subregional body and the club belongs to the regional one", !still && joint);
    const Complex health = complex(l, "health", "2024-06-01");
    s.require("on health the universal body and the partnership are nested, the partnership within", health.pairs.size() == 1 && health.pairs[0].inner == "HEALTH");
  }

  // ---- Reciprocity ----
  {
    const std::vector<Transfer> cycle{{"a", "b", "2026-01-05", 10}, {"b", "c", "2026-01-10", 10}, {"c", "a", "2026-01-15", 10}};
    const Reciprocity r = reciprocity(cycle, 30);
    s.require("a cycle of equal gifts is diffusely reciprocal and not specifically at all", std::fabs(r.pairwise - 1) < 1e-15 && std::fabs(r.group) < 1e-15,
              std::to_string(r.pairwise) + ", " + std::to_string(r.group));
    const std::vector<Transfer> alternate{{"a", "b", "2026-01-05", 10}, {"b", "a", "2026-02-05", 10}};
    const Reciprocity one = reciprocity(alternate, 31), two = reciprocity(alternate, 62);
    s.require("gifts alternating month by month do not balance within a month, and balance over two",
              one.windows.size() == 2 && std::fabs(one.pairwise - 1) < 1e-15 && two.windows.size() == 1 && std::fabs(two.pairwise) < 1e-15 && std::fabs(two.group) < 1e-15);
    calx::random::Stream rng(20261119, 134, 5, 0);
    std::size_t bad = 0;
    for (int t = 0; t < 200; t++) {
      std::vector<Transfer> ts;
      const std::size_t n = 1 + rng.below(20);
      for (std::size_t k = 0; k < n; k++) {
        const std::string from = std::string(1, static_cast<char>('a' + rng.below(5))), to = std::string(1, static_cast<char>('a' + rng.below(5)));
        if (from == to) continue;
        ts.push_back({from, to, archeion::retention::add_days("2026-01-01", static_cast<long>(rng.below(120))), static_cast<double>(1 + rng.below(50))});
      }
      if (ts.empty()) continue;
      for (long days : {7L, 30L, 365L}) {
        const Reciprocity x = reciprocity(ts, days);
        for (const auto& w : x.windows) bad += w.group > w.pairwise + 1e-12 || w.pairwise > 1 + 1e-12 || w.group < -1e-12;
      }
    }
    s.require("on 200 random sets of transfers the group's imbalance never exceeds the pairs', and both lie in [0, 1]", bad == 0, std::to_string(bad) + " exceptions");
  }
  return s.done();
}
