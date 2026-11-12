// A brief, and whether a drawn building answers it (ADR 0095).
//
// A BRIEF is what a household asks of a building before any wall is drawn: spaces of given uses, each at least so large, and
// relations between uses. It is a labelled graph B: a vertex for each space asked for, labelled by its use and its least area; an
// edge for each relation, labelled by its kind.
//
// A BUILDING ANSWERS IT when the brief maps into the building's own graphs, labels kept: an injective map φ from the brief's
// vertices to rooms of the same use and at least the area asked, such that
// - where the brief says a OPENS TO b, φ(a) and φ(b) are joined in the access graph (a door or an open wall between them);
// - where it says a ADJOINS b, φ(a) and φ(b) share a wall;
// - where it says a is APART from b, they are not joined in the access graph;
// - where it says a is NOT REACHED THROUGH b, φ(b) is not among the rooms every way to φ(a) passes.
// Relations are stated between uses, not between particular rooms, and are read universally over the rooms of the first use: "each
// bedroom opens to a hall", "no toilet opens to a kitchen", "no bedroom is reached only through a bedroom". With that reading the
// map exists exactly when every statement below holds, so the brief is checked statement by statement, each with its reason.
//
// Always asked, whatever the brief: every indoor room is reached from the outside.

#pragma once

#include <architecture/plan/plan.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

namespace architecture::programme {

namespace ap = architecture::plan;

struct Need {
  std::string kind;
  int count = 1;
  double least_area = 0;   // of each, net (m²)
};

enum class How { opens_to, adjoins, apart, not_through };

struct Relation {
  std::string a, b;
  How how = How::opens_to;
};

struct Brief {
  std::string name;
  std::vector<Need> needs;
  std::vector<Relation> relations;
};

struct Statement {
  std::string text;
  bool ok = true;
  std::string note;
};

namespace detail {
// A use as a noun: "living" is a living room.
inline std::string noun(const std::string& kind) {
  for (const char* k : {"living", "dining", "family"})
    if (kind == k) return kind + " room";
  return kind;
}
inline std::string plural(const std::string& kind, int n) {
  const std::string one = noun(kind);
  if (n == 1) return one;
  if (one.size() > 1 && one.back() == 'y') return one.substr(0, one.size() - 1) + "ies";
  return one + "s";
}
inline std::string metres2(double a) {
  char buf[32];
  std::snprintf(buf, sizeof buf, "%.1f", a);
  return std::string(buf) + " m²";
}
}  // namespace detail

inline std::vector<Statement> check(const Brief& brief, const ap::Building& b, const ap::Reading& R) {
  std::vector<Statement> out;
  struct Space {
    std::size_t vertex, storey, room;
  };
  std::vector<Space> spaces;
  for (std::size_t k = 0; k < R.storeys.size(); k++)
    for (std::size_t r = 0; r < R.storeys[k].rooms.size(); r++) spaces.push_back({R.vertex(k, r), k, r});
  const auto room = [&](const Space& s) -> const ap::Room& { return R.storeys[s.storey].rooms[s.room]; };
  const auto of_kind = [&](const std::string& kind) {
    std::vector<Space> found;
    for (const auto& s : spaces)
      if (room(s).kind == kind) found.push_back(s);
    return found;
  };
  const auto joined = [&](std::size_t u, std::size_t v) { return std::find(R.access[u].begin(), R.access[u].end(), v) != R.access[u].end(); };
  const auto share_wall = [&](const Space& x, const Space& y) {
    if (x.storey != y.storey) return false;
    for (const auto& side : R.storeys[x.storey].sides)
      if ((side.left == x.room && side.right == y.room) || (side.left == y.room && side.right == x.room)) return true;
    return false;
  };
  (void)b;

  // Every indoor room is reached.
  {
    std::string lost;
    for (const auto& s : spaces)
      if (ap::use_of(room(s).kind).indoor && !std::isfinite(R.depth[s.vertex])) lost += (lost.empty() ? "" : ", ") + ap::called(room(s));
    out.push_back({"Every room is reached from the outside", lost.empty(), lost.empty() ? "" : "not reached: " + lost});
  }
  for (const auto& need : brief.needs) {
    const auto have = of_kind(need.kind);
    int large = 0;
    std::string small;
    for (const auto& s : have) {
      if (room(s).net_area + 1e-9 >= need.least_area) large++;
      else small += (small.empty() ? "" : ", ") + ap::called(room(s)) + " is " + detail::metres2(room(s).net_area);
    }
    std::string text = std::to_string(need.count) + " " + detail::plural(need.kind, need.count);
    if (need.least_area > 0) text += " of at least " + detail::metres2(need.least_area);
    std::string note;
    if (large < need.count) {
      note = std::to_string(large) + " found";
      if (!small.empty()) note += "; " + small;
    }
    out.push_back({text, large >= need.count, note});
  }
  for (const auto& rel : brief.relations) {
    const auto as = of_kind(rel.a), bs = of_kind(rel.b);
    if (as.empty()) continue;   // nothing to state of a use that is not drawn; the needs say whether it should be
    std::string failing;
    const auto fail = [&](const Space& s) { failing += (failing.empty() ? "" : ", ") + ap::called(room(s)); };
    std::string text;
    switch (rel.how) {
      case How::opens_to:
        text = "Each " + detail::noun(rel.a) + " opens to a " + detail::noun(rel.b);
        for (const auto& x : as)
          if (std::none_of(bs.begin(), bs.end(), [&](const Space& y) { return joined(x.vertex, y.vertex); })) fail(x);
        break;
      case How::adjoins:
        text = "Each " + detail::noun(rel.a) + " adjoins a " + detail::noun(rel.b);
        for (const auto& x : as)
          if (std::none_of(bs.begin(), bs.end(), [&](const Space& y) { return share_wall(x, y); })) fail(x);
        break;
      case How::apart:
        text = "No " + detail::noun(rel.a) + " opens to a " + detail::noun(rel.b);
        for (const auto& x : as)
          if (std::any_of(bs.begin(), bs.end(), [&](const Space& y) { return x.vertex != y.vertex && joined(x.vertex, y.vertex); })) fail(x);
        break;
      case How::not_through:
        text = "No " + detail::noun(rel.a) + " is reached only through a " + detail::noun(rel.b);
        for (const auto& x : as)
          if (std::any_of(bs.begin(), bs.end(), [&](const Space& y) {
                return std::find(R.through[x.vertex].begin(), R.through[x.vertex].end(), y.vertex) != R.through[x.vertex].end();
              }))
            fail(x);
        break;
    }
    out.push_back({text, failing.empty(), failing.empty() ? "" : "not so: " + failing});
  }
  return out;
}

}  // namespace architecture::programme
