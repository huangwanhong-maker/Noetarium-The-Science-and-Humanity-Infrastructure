// A drawn building read in one call (ADR 0095, ADR 0096, ADR 0097): the architecture domain's plan, programme, rules, interior and
// shell composed for any work.
//
// A work that lets a person draw a building (the Design Studio first; a plug-in of the Urban Science Workbench or a module of a
// board later) holds a document, `noetarium.building/1`, and asks what follows from it. This header reads the document, calls the
// domain, and answers in JSON. It names no place and no jurisdiction: the lot, the scheme of siting rules, the code of habitation,
// the brief, the rates and the fabric are all stated in the document by whoever wrote it.
//
// THE DOCUMENT (lengths in metres; x east, y north; angles in degrees):
//   {"schema": "noetarium.building/1", "name": …,
//    "site":    {"lot": {"ring": [[x, y], …], "street": [true, false, …]}},            the lot, and which of its edges face a street
//    "storeys": [{"name", "height", "ceiling", "vertices": [[x, y], …],
//                 "walls":    [{"a", "b", "t", "kind": "solid" | "open"}],
//                 "openings": [{"wall", "kind": "door" | "window", "at", "width", "height", "sill", "glazed", "openable", "swing"}],
//                 "labels":   [{"at": [x, y], "kind", "name"}]}],
//    "stairs":  [{"from", "at": [x, y], "bearing", "width", "going", "risers"}],       bearing counter-clockwise from east
//    "furniture": [{"storey", "kind", "name", "at": [x, y], "bearing", "width", "depth", "height", "front", "side", "around",
//                 "obstructs"}],                                                          what stands in the rooms (ADR 0096)
//    "interior": {"passage"},                                                           the width a person needs to pass
//    "brief":   {"name", "needs": [{"kind", "count", "area"}], "relations": [{"a", "b", "how"}]},
//               how: "opens to" | "adjoins" | "apart" | "not through"
//    "siting":  a scheme of siting rules (architecture/rules/siting.hpp); a limit is {"least", "most", "clause"}
//    "code":    a code of habitation (architecture/plan/habitability.hpp)
//    "rates":   {"currency", "items": {"outer_wall": [low, high], …}}
//    "fabric":  {"wall", "window", "door", "roof", "ground"}                            U-values, W/(m² K)
//    "roof":    {"form": "flat" | "shed" | "gable" | "hip", "pitch", "eaves", "parapet", "fall", "gables": [[x, y], …]}
//               pitch and fall in degrees; a roof that states a pitch and no form is a hip (ADR 0097)
//    "light":   {"transmittance", "reflectance", "gain", "indoor", "outdoor"}}          the glass and the rooms' surfaces; a day's
//                                                                                       temperatures (°C), for its balance
// and, beside the building, what stands and shines about it, which is no part of the document:
//   "around":  {"buildings": [{"ring": [[x, y], …], "height"}],                        the neighbours, as prisms on flat ground
//               "suns": [[altitude, azimuth, hours], …]}                               the Sun at the instants of a day: degrees,
//                                                                                       azimuth from north through east
//
// THE REQUESTS (answer):
//   {"op": "vocabulary"}                 the uses of rooms the domain knows, the elements a rate may be declared for
//   {"op": "read", "building": {…}, "around": {…}}
//                                        everything that follows: rooms, walls' sides, access, the findings of the code and of the
//                                        siting scheme one by one, the brief's statements, quantities, cost and heat loss, of each
//                                        piece whether it is within its room, apart, usable and reached, and the shell: the
//                                        footprints, the roof's faces, ridges and the walls carried up to it, the faces to the
//                                        winds, and the sun, the sky and the daylight at the glass
// An index that names nothing (the outside, a room without a label) is null, as is a number that has no value.

#pragma once

#include "science/json.hpp"

#include <architecture/interior/interior.hpp>
#include <architecture/plan/habitability.hpp>
#include <architecture/plan/plan.hpp>
#include <architecture/plan/quantities.hpp>
#include <architecture/programme/programme.hpp>
#include <architecture/rules/siting.hpp>
#include <architecture/shell/shell.hpp>

#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::building {

namespace ai = architecture::interior;
namespace ap = architecture::plan;
namespace ar = architecture::rules;
namespace as = architecture::shell;
namespace ae = architecture::environment;
namespace pg = architecture::programme;

namespace detail {

inline double number(const Json& j, double fallback) { return j.is_number() ? j.as_number() : fallback; }

inline ap::Point point(const Json& j, const char* what) {
  if (!j.is_array() || j.size() < 2 || !j[0].is_number() || !j[1].is_number()) throw std::runtime_error(std::string(what) + " is [x, y]");
  return {j[0].as_number(), j[1].as_number()};
}

inline std::size_t index(const Json& j, const char* what) {
  if (!j.is_number() || j.as_number() < 0 || j.as_number() != std::floor(j.as_number())) throw std::runtime_error(std::string(what) + " is an index");
  return static_cast<std::size_t>(j.as_number());
}

inline ar::Limit limit(const Json& j) {
  ar::Limit l;
  if (!j.is_object()) return l;
  if (j["least"].is_number()) l.least = j["least"].as_number();
  if (j["most"].is_number()) l.most = j["most"].as_number();
  l.clause = j["clause"].text();
  return l;
}

inline std::map<std::string, ar::Limit> limits(const Json& j) {
  std::map<std::string, ar::Limit> out;
  if (j.is_object())
    for (const auto& [key, value] : j.as_object()) out[key] = limit(value);
  return out;
}

inline Json ring(const ap::Ring& r) {
  Json out = Json::array();
  for (const auto& p : r) out.push(Json::array({p[0], p[1]}));
  return out;
}

inline Json maybe(std::size_t i) { return i == ap::none ? Json() : Json(i); }

inline Json finding(const ar::Finding& f) {
  return Json::object({{"rule", f.rule}, {"title", f.title}, {"clause", f.clause}, {"subject", f.subject}, {"least", f.least}, {"most", f.most},
                       {"actual", f.actual}, {"unit", f.unit}, {"ok", f.ok}, {"note", f.note}});
}

inline Json strings(const std::vector<std::string>& xs) {
  Json out = Json::array();
  for (const auto& x : xs) out.push(x);
  return out;
}

}  // namespace detail

// ---- the document read ---------------------------------------------------------------------------------------------------------

inline ap::Building building(const Json& doc) {
  using detail::number;
  ap::Building b;
  for (const auto& js : doc["storeys"].is_array() ? doc["storeys"].as_array() : Json::Array{}) {
    ap::Storey s;
    s.name = js["name"].text();
    s.height = number(js["height"], 3.0), s.ceiling = number(js["ceiling"], 2.7);
    if (!(s.height > 0) || !(s.ceiling > 0)) throw std::runtime_error("a storey's height and ceiling are positive");
    for (const auto& v : js["vertices"].is_array() ? js["vertices"].as_array() : Json::Array{}) s.vertices.push_back(detail::point(v, "a vertex"));
    for (const auto& jw : js["walls"].is_array() ? js["walls"].as_array() : Json::Array{}) {
      ap::Wall w;
      w.a = detail::index(jw["a"], "a wall's end"), w.b = detail::index(jw["b"], "a wall's end");
      w.thickness = number(jw["t"], 0.11);
      if (!(w.thickness >= 0)) throw std::runtime_error("a wall's thickness is not negative");
      w.kind = jw["kind"].text("solid") == "open" ? ap::WallKind::open : ap::WallKind::solid;
      s.walls.push_back(w);
    }
    for (const auto& jo : js["openings"].is_array() ? js["openings"].as_array() : Json::Array{}) {
      ap::Opening o;
      o.wall = detail::index(jo["wall"], "an opening's wall");
      o.kind = jo["kind"].text("door") == "window" ? ap::OpeningKind::window : ap::OpeningKind::door;
      const bool window = o.kind == ap::OpeningKind::window;
      o.at = number(jo["at"], 0), o.width = number(jo["width"], window ? 1.2 : 0.82), o.height = number(jo["height"], window ? 1.2 : 2.04);
      o.sill = number(jo["sill"], window ? 0.9 : 0), o.glazed = number(jo["glazed"], window ? 1 : 0), o.openable = number(jo["openable"], window ? 0.5 : 1);
      o.swing = number(jo["swing"], 0) > 0 ? 1 : number(jo["swing"], 0) < 0 ? -1 : 0;
      s.openings.push_back(o);
    }
    for (const auto& jl : js["labels"].is_array() ? js["labels"].as_array() : Json::Array{})
      s.labels.push_back({detail::point(jl["at"], "a label's place"), jl["kind"].text(), jl["name"].text()});
    b.storeys.push_back(std::move(s));
  }
  for (const auto& jt : doc["stairs"].is_array() ? doc["stairs"].as_array() : Json::Array{}) {
    ap::Stair st;
    st.from = detail::index(jt["from"], "a stair's storey");
    st.at = detail::point(jt["at"], "a stair's foot");
    st.bearing = number(jt["bearing"], 90) * std::numbers::pi / 180;
    st.width = number(jt["width"], 1.0), st.going = number(jt["going"], 0.25);
    st.risers = static_cast<int>(number(jt["risers"], 17));
    b.stairs.push_back(st);
  }
  return b;
}

// What stands in the rooms.
inline std::vector<ai::Piece> furniture(const Json& doc, std::size_t storeys, std::vector<std::size_t>* kept = nullptr) {
  using detail::number;
  std::vector<ai::Piece> out;
  std::size_t at = 0;
  for (const auto& jp : doc["furniture"].is_array() ? doc["furniture"].as_array() : Json::Array{}) {
    const std::size_t here = at++;
    ai::Piece p;
    p.kind = jp["kind"].text(), p.name = jp["name"].text();
    p.storey = detail::index(jp["storey"], "a piece's storey");
    if (p.storey >= storeys) throw std::runtime_error("a piece stands on a storey of the building");
    p.at = detail::point(jp["at"], "a piece's place");
    p.bearing = number(jp["bearing"], 90) * std::numbers::pi / 180;
    p.width = number(jp["width"], 1), p.depth = number(jp["depth"], 1), p.height = number(jp["height"], 1);
    p.front = number(jp["front"], 0), p.side = number(jp["side"], 0), p.around = number(jp["around"], 0);
    if (!(p.width > 0 && p.depth > 0) || p.front < 0 || p.side < 0 || p.around < 0) throw std::runtime_error("a piece has a width and a depth, and the clear floor it needs is not negative");
    p.obstructs = !(jp["obstructs"].is_bool() && !jp["obstructs"].as_bool());
    if (kept) {
      // What stands outdoors (a tree, a hedge, a bench) is judged by no room.
      if (jp["outdoor"].is_bool() && jp["outdoor"].as_bool()) continue;
      kept->push_back(here);
    }
    out.push_back(std::move(p));
  }
  return out;
}

inline ar::Lot lot(const Json& site) {
  ar::Lot l;
  const Json& jl = site["lot"];
  for (const auto& p : jl["ring"].is_array() ? jl["ring"].as_array() : Json::Array{}) l.ring.push_back(detail::point(p, "a corner of the lot"));
  for (const auto& s : jl["street"].is_array() ? jl["street"].as_array() : Json::Array{}) l.street.push_back(s.is_bool() && s.as_bool() ? 1 : 0);
  // Counter-clockwise, its edges' flags kept with their edges.
  if (l.ring.size() >= 3 && ar::cg::ring_area(l.ring) < 0) {
    const std::size_t n = l.ring.size();
    l.street.resize(n, 0);
    std::vector<char> street(n);
    for (std::size_t k = 0; k < n; k++) street[(2 * n - 2 - k) % n] = l.street[k];
    std::reverse(l.ring.begin(), l.ring.end());
    l.street = street;
  }
  return l;
}

inline ar::Scheme scheme(const Json& j) {
  ar::Scheme s;
  if (!j.is_object()) return s;
  s.name = j["name"].text(), s.version = j["version"].text(), s.source = j["source"].text();
  s.height = detail::limit(j["height"]), s.storeys = detail::limit(j["storeys"]);
  s.street_setback = detail::limit(j["street_setback"]), s.side_setback = detail::limit(j["side_setback"]);
  for (const auto& st : j["side_setback"]["steps"].is_array() ? j["side_setback"]["steps"].as_array() : Json::Array{})
    s.side_steps.push_back({detail::number(st[0], 0), detail::number(st[1], std::numeric_limits<double>::infinity()), detail::number(st[2], 0)});
  s.coverage = detail::limit(j["coverage"]), s.open_share = detail::limit(j["open_share"]), s.open_area = detail::limit(j["open_area"]);
  const Json& jb = j["boundary"];
  if (jb.is_object()) {
    if (jb["base"].is_number()) s.boundary_base = jb["base"].as_number();
    s.boundary_share = detail::number(jb["share"], 0), s.boundary_tolerance = detail::number(jb["tolerance"], 0.2);
    s.boundary_height = detail::limit(jb["height"]), s.boundary_clause = jb["clause"].text();
  }
  for (const auto& row : j["garden"]["rows"].is_array() ? j["garden"]["rows"].as_array() : Json::Array{})
    s.garden.push_back({detail::number(row[0], 0), detail::number(row[1], 0)});
  s.garden_clause = j["garden"]["clause"].text();
  return s;
}

inline ap::Code code(const Json& j) {
  ap::Code c;
  if (!j.is_object()) return c;
  c.name = j["name"].text(), c.version = j["version"].text(), c.source = j["source"].text();
  c.ceiling_habitable = detail::limit(j["ceiling_habitable"]), c.ceiling_other = detail::limit(j["ceiling_other"]);
  c.ceiling = detail::limits(j["ceiling"]);
  c.light = detail::limit(j["light"]), c.air = detail::limit(j["air"]);
  c.entrance = detail::limit(j["entrance"]), c.door = detail::limit(j["door"]), c.passage = detail::limit(j["passage"]);
  c.riser = detail::limit(j["riser"]), c.going = detail::limit(j["going"]), c.slope = detail::limit(j["slope"]), c.flight = detail::limit(j["flight"]);
  c.area = detail::limits(j["area"]), c.width = detail::limits(j["width"]);
  return c;
}

inline pg::Brief brief(const Json& j) {
  pg::Brief b;
  if (!j.is_object()) return b;
  b.name = j["name"].text();
  for (const auto& n : j["needs"].is_array() ? j["needs"].as_array() : Json::Array{})
    if (!n["kind"].text().empty()) b.needs.push_back({n["kind"].text(), static_cast<int>(detail::number(n["count"], 1)), detail::number(n["area"], 0)});
  for (const auto& r : j["relations"].is_array() ? j["relations"].as_array() : Json::Array{}) {
    const std::string how = r["how"].text("opens to");
    const pg::How h = how == "adjoins" ? pg::How::adjoins : how == "apart" ? pg::How::apart : how == "not through" ? pg::How::not_through : pg::How::opens_to;
    if (!r["a"].text().empty() && !r["b"].text().empty()) b.relations.push_back({r["a"].text(), r["b"].text(), h});
  }
  return b;
}

// ---- the answers ---------------------------------------------------------------------------------------------------------------

inline Json vocabulary() {
  Json kinds = Json::array();
  for (const auto& k : ap::kinds()) {
    const ap::Use u = ap::use_of(k);
    kinds.push(Json::object({{"kind", k}, {"indoor", u.indoor}, {"habitable", u.habitable}, {"wet", u.wet}, {"circulation", u.circulation}}));
  }
  Json elements = Json::array();
  for (const auto& e : ap::elements(ap::Quantities{})) elements.push(Json::object({{"id", e.id}, {"unit", e.unit}}));
  return Json::object({{"schema", "noetarium.building/1"}, {"kinds", kinds}, {"elements", elements}});
}

inline as::RoofSpec roof_spec(const Json& j) {
  as::RoofSpec spec;
  const double pitch = detail::number(j["pitch"], 0);
  if (!(pitch >= 0 && pitch < 85)) throw std::runtime_error("a roof's pitch is from 0 to 85 degrees");
  spec.pitch = pitch * std::numbers::pi / 180;
  spec.form = j["form"].is_string() ? j["form"].text() : (pitch > 0 ? "hip" : "flat");
  if (spec.form != "flat" && spec.form != "shed" && spec.form != "gable" && spec.form != "hip") throw std::runtime_error("a roof is flat, a shed, a gable or a hip");
  if (!(pitch > 0)) spec.form = "flat";
  spec.eaves = detail::number(j["eaves"], 0), spec.parapet = detail::number(j["parapet"], 0.3);
  if (!(spec.eaves >= 0 && spec.eaves <= 3 && spec.parapet >= 0 && spec.parapet <= 3)) throw std::runtime_error("a roof's eaves and its parapet are from 0 to 3 m");
  spec.fall = detail::number(j["fall"], -90) * std::numbers::pi / 180;
  for (const auto& g : j["gables"].is_array() ? j["gables"].as_array() : Json::Array{}) spec.gables.push_back(detail::point(g, "a gable's place"));
  return spec;
}

inline Json point3(const as::Point3& p) { return Json::array({p[0], p[1], p[2]}); }

inline Json read(const Json& doc, const Json& around = Json()) {
  const ap::Building b = building(doc);
  const ap::Reading R = ap::read(b);
  Json out = Json::object();
  out.set("sound", R.sound);
  out.set("remarks", detail::strings(R.remarks));

  Json storeys = Json::array();
  double top = 0;
  std::vector<ar::Level> levels;
  for (std::size_t k = 0; k < b.storeys.size(); k++) {
    const auto& s = b.storeys[k];
    const auto& S = R.storeys[k];
    top += s.height;
    Json rooms = Json::array();
    for (std::size_t r = 0; r < S.rooms.size(); r++) {
      const auto& room = S.rooms[r];
      const ap::Use use = ap::use_of(room.kind);
      const std::size_t v = R.vertex(k, r);
      Json holes = Json::array();
      for (const auto& h : room.holes) holes.push(detail::ring(h));
      Json through = Json::array();
      for (const std::size_t y : R.through[v]) through.push(Json::object({{"storey", R.spaces[y - 1].first}, {"room", R.spaces[y - 1].second}}));
      Json walls = Json::array();
      for (const auto& [e, dir] : room.walls) walls.push(e);
      rooms.push(Json::object({{"ring", detail::ring(room.ring)}, {"holes", holes}, {"net_ring", detail::ring(room.net_ring)}, {"walls", walls},
                               {"area", room.area}, {"net_area", room.net_area}, {"net_exact", room.net_exact}, {"perimeter", room.perimeter},
                               {"width", room.width}, {"label", detail::maybe(room.label)}, {"kind", room.kind}, {"name", room.name},
                               {"indoor", use.indoor}, {"habitable", use.habitable}, {"wet", use.wet}, {"circulation", use.circulation},
                               {"outer_length", room.outer_length}, {"glazed_area", room.glazed_area}, {"openable_area", room.openable_area},
                               {"light_share", room.net_area > 0 ? room.glazed_area / room.net_area : 0.0},
                               {"air_share", room.net_area > 0 ? room.openable_area / room.net_area : 0.0},
                               {"vertex", v}, {"depth", R.depth[v]}, {"through", through}, {"asymmetry", R.asymmetry[v]}}));
    }
    Json walls = Json::array();
    ar::Level level;
    level.top = top, level.floor = S.floor_area, level.thickness = 0;
    for (std::size_t e = 0; e < S.sides.size(); e++) {
      const auto& side = S.sides[e];
      walls.push(Json::object({{"left", detail::maybe(side.left)}, {"right", detail::maybe(side.right)}, {"length", side.length}, {"outer", side.outer},
                               {"divides_nothing", side.divides_nothing}}));
      if (S.sound && side.outer && s.walls[e].kind == ap::WallKind::solid) {
        level.walls.push_back({s.vertices[s.walls[e].a], s.vertices[s.walls[e].b]});
        level.thickness = std::max(level.thickness, s.walls[e].thickness);
      }
    }
    if (S.sound && !level.walls.empty()) levels.push_back(std::move(level));
    Json faulty = Json::array();
    for (const std::size_t w : S.faulty_walls) faulty.push(w);
    storeys.push(Json::object({{"sound", S.sound}, {"faults", detail::strings(S.faults)}, {"remarks", detail::strings(S.remarks)}, {"faulty_walls", faulty},
                               {"rooms", rooms}, {"walls", walls}, {"floor_area", S.floor_area}, {"internal_area", S.internal_area}, {"top", top}}));
  }
  out.set("storeys", storeys);

  Json stairs = Json::array();
  for (const auto& st : R.stairs)
    stairs.push(Json::object({{"foot", detail::maybe(st.foot)}, {"head", detail::maybe(st.head)}, {"riser", st.riser}, {"run", st.run}, {"ring", detail::ring(st.ring)}}));
  out.set("stairs", stairs);

  Json spaces = Json::array(), edges = Json::array();
  for (const auto& [storey, room] : R.spaces) spaces.push(Json::object({{"storey", storey}, {"room", room}}));
  for (std::size_t u = 0; u < R.access.size(); u++)
    for (const std::size_t v : R.access[u])
      if (u < v) edges.push(Json::array({u, v}));
  out.set("access", Json::object({{"spaces", spaces}, {"edges", edges}, {"depth", Json(R.depth)}}));

  // The code of habitation, finding by finding.
  {
    Json findings = Json::array();
    const ap::Code c = code(doc["code"]);
    for (const auto& f : ap::check(b, R, c)) findings.push(detail::finding(f));
    out.set("habitability", Json::object({{"name", c.name}, {"version", c.version}, {"source", c.source}, {"findings", findings}}));
  }
  // The siting scheme, where a lot is drawn.
  {
    const ar::Lot l = lot(doc["site"]);
    const ar::Scheme sc = scheme(doc["siting"]);
    Json siting = Json::object({{"name", sc.name}, {"version", sc.version}, {"source", sc.source}});
    Json findings = Json::array();
    if (l.ring.size() >= 3) {
      siting.set("lot_area", l.area());
      siting.set("ring", detail::ring(l.ring));
      Json street = Json::array();
      for (std::size_t k = 0; k < l.ring.size(); k++) street.push(k < l.street.size() && l.street[k] != 0);
      siting.set("street", street);
      ar::Rules base;
      base.street_setback = std::isnan(sc.street_setback.least) ? 0.0 : sc.street_setback.least;
      base.side_setback = std::isnan(sc.side_setback.least) ? 0.0 : sc.side_setback.least;
      siting.set("envelope", detail::ring(ar::envelope(l, base).ground));
      if (!levels.empty()) {
        const ar::Siting S = ar::check(l, levels, sc);
        for (const auto& f : S.findings) findings.push(detail::finding(f));
        siting.set("covered", S.covered), siting.set("open", S.open), siting.set("height", S.height), siting.set("on_boundary", Json(S.on_boundary));
      }
    }
    siting.set("findings", findings);
    out.set("siting", siting);
  }
  // The brief, statement by statement.
  {
    Json statements = Json::array();
    if (R.sound)
      for (const auto& st : pg::check(brief(doc["brief"]), b, R)) statements.push(Json::object({{"text", st.text}, {"ok", st.ok}, {"note", st.note}}));
    out.set("brief", statements);
  }
  // What stands in the rooms: each piece within its room, apart from the others, usable and reached.
  {
    std::vector<std::size_t> kept;
    const auto pieces = furniture(doc, b.storeys.size(), &kept);
    const auto all = [&](const std::vector<std::size_t>& some) { Json out = Json::array(); for (const std::size_t i : some) out.push(kept[i]); return out; };
    const std::size_t count = doc["furniture"].is_array() ? doc["furniture"].size() : 0;
    const double passage = detail::number(doc["interior"]["passage"], 0.6);
    if (!(passage >= 0.2 && passage <= 2)) throw std::runtime_error("the width a person needs to pass is from 0.2 to 2 m");
    const ai::Reading I = ai::read(b, R, pieces, passage);
    Json found = Json::array(), doors = Json::array(), floors = Json::array();
    // Of each piece of the document, in its order: what stands outdoors is well placed wherever it stands.
    std::size_t next = 0;
    for (std::size_t i = 0; i < count; i++) {
      if (next < kept.size() && kept[next] == i) {
        const auto& f = I.pieces[next++];
        found.push(Json::object({{"room", detail::maybe(f.room)}, {"within", f.within}, {"overlaps", all(f.overlaps)}, {"space_within", f.space_within},
                                 {"blocked_by", all(f.blocked_by)}, {"reached", f.reached}, {"judged", f.judged}, {"ok", f.ok()}}));
      } else {
        found.push(Json::object({{"room", Json()}, {"within", true}, {"overlaps", Json::array()}, {"space_within", true}, {"blocked_by", Json::array()}, {"reached", true},
                                 {"judged", false}, {"ok", true}, {"outdoor", true}}));
      }
    }
    for (const auto& d : I.doors) doors.push(Json::object({{"storey", d.storey}, {"opening", d.opening}, {"swing_blocked_by", all(d.swing_blocked_by)}, {"entered", d.entered}}));
    for (const auto& r : I.rooms) floors.push(Json::object({{"storey", r.storey}, {"room", r.room}, {"free_share", r.free_share}}));
    out.set("interior", Json::object({{"passage", I.passage}, {"cell", I.cell}, {"pieces", found}, {"doors", doors}, {"rooms", floors}}));
  }
  // The shell: the footprints, the roof, the faces to the winds, the sun and the sky at the glass.
  const as::RoofSpec spec = roof_spec(doc["roof"]);
  double roofs_area = 0, roofs_wall = 0, roofed_floor = 0;
  double day_gain = std::numeric_limits<double>::quiet_NaN();
  if (R.sound) {
    std::vector<ae::Sun> suns;
    std::vector<ae::Prism> prisms;
    for (const auto& s : around["suns"].is_array() ? around["suns"].as_array() : Json::Array{}) {
      const double altitude = detail::number(s[0], 0), azimuth = detail::number(s[1], 0), hours = detail::number(s[2], 0);
      if (altitude > 0 && hours > 0) suns.push_back({altitude * std::numbers::pi / 180, azimuth * std::numbers::pi / 180, hours});
    }
    for (const auto& n : around["buildings"].is_array() ? around["buildings"].as_array() : Json::Array{}) {
      ae::Prism prism;
      for (const auto& p : n["ring"].is_array() ? n["ring"].as_array() : Json::Array{}) prism.ring.push_back(detail::point(p, "a corner of a neighbour"));
      prism.height = detail::number(n["height"], 0);
      if (prism.ring.size() >= 3 && prism.height > 0) prisms.push_back(std::move(prism));
    }
    as::Light light;
    const Json& jl = doc["light"];
    light.transmittance = detail::number(jl["transmittance"], light.transmittance), light.reflectance = detail::number(jl["reflectance"], light.reflectance);
    light.gain = detail::number(jl["gain"], light.gain);
    if (!(light.transmittance > 0 && light.transmittance <= 1 && light.reflectance >= 0 && light.reflectance < 1 && light.gain >= 0 && light.gain <= 1))
      throw std::runtime_error("glass passes a share of light between 0 and 1, and a room's surfaces reflect less than all of it");
    const as::Shell S = as::read(b, R, spec, suns, prisms, light);
    const double deg = 180 / std::numbers::pi;
    static const char* winds[8] = {"east", "north-east", "north", "north-west", "west", "south-west", "south", "south-east"};
    Json roofs = Json::array(), prints = Json::array(), facades = Json::array(), glass = Json::array(), rooms = Json::array();
    for (const auto& storey : S.footprints) {
      Json parts = Json::array();
      for (const auto& f : storey) parts.push(Json::object({{"ring", detail::ring(f.ring)}, {"court", f.court}, {"exact", f.exact}}));
      prints.push(parts);
    }
    for (const auto& roof : S.roofs) {
      Json pieces = Json::array(), lines = Json::array(), outline = Json::array(), walls = Json::array(), gable = Json::array();
      for (const auto& face : roof.faces) {
        Json ring = Json::array();
        for (const auto& p : face.ring) ring.push(point3(p));
        pieces.push(Json::object({{"edge", detail::maybe(face.edge)}, {"ring", ring}}));
      }
      for (const auto& l : roof.lines) lines.push(Json::object({{"a", point3(l.a)}, {"b", point3(l.b)}, {"kind", l.kind}}));
      for (const auto& p : roof.outline) outline.push(point3(p));
      for (const auto& w : roof.rises) {
        Json top = Json::array();
        for (const auto& [s, z] : w.top) top.push(Json::array({s, z}));
        walls.push(Json::object({{"a", Json::array({w.a[0], w.a[1]})}, {"b", Json::array({w.b[0], w.b[1]})}, {"t", w.thickness}, {"top", top}, {"area", w.area}}));
      }
      for (const char g : roof.gable) gable.push(g != 0);
      roofs.push(Json::object({{"storey", roof.storey}, {"part", roof.part}, {"form", roof.form}, {"pitch", roof.pitch * deg}, {"eaves", roof.eaves}, {"plate", roof.plate},
                               {"top", roof.top}, {"area", roof.area}, {"plan_area", roof.plan_area}, {"wall_area", roof.wall_area}, {"exact", roof.exact},
                               {"note", roof.note}, {"refused", roof.refused}, {"cover", doc["roof"]["cover"].text()}, {"pieces", pieces}, {"lines", lines},
                               {"outline", outline}, {"walls", walls}, {"gable", gable}}));
      roofs_area += roof.area, roofs_wall += roof.wall_area;
      roofed_floor = R.storeys[roof.storey].floor_area;
    }
    for (int w = 0; w < 8; w++) {
      const auto& f = S.facades[w];
      if (!(f.wall > 0)) continue;
      facades.push(Json::object({{"wind", winds[w]}, {"bearing", 45.0 * w}, {"wall", f.wall}, {"glass", f.glass}, {"share", f.glass / f.wall}, {"length", f.length},
                                 {"windows", f.windows}, {"doors", f.doors}}));
    }
    for (const auto& g : S.glass)
      glass.push(Json::object({{"storey", g.storey}, {"opening", g.opening}, {"room", detail::maybe(g.room)}, {"area", g.area}, {"bearing", g.bearing * deg},
                               {"hours", g.sun.hours}, {"full_hours", g.sun.full_hours}, {"energy", g.sun.energy}, {"sky", g.sun.sky * deg}, {"low", g.sun.low * deg},
                               {"high", g.sun.high * deg}, {"overhang", g.pane.overhang}}));
    for (const auto& r : S.rooms)
      rooms.push(Json::object({{"storey", r.storey}, {"room", r.room}, {"glass", r.glass}, {"surfaces", r.surfaces}, {"sun_hours", r.sun_hours}, {"energy", r.energy},
                               {"daylight_factor", r.daylight_factor}, {"sky", r.sky * deg}, {"panes", r.panes}}));
    if (S.lit) day_gain = S.solar_gain / 1000;
    out.set("shell", Json::object({{"footprints", prints}, {"roofs", roofs}, {"facades", facades}, {"glass", glass}, {"rooms", rooms}, {"lit", S.lit},
                                   {"solar_gain", S.lit ? Json(S.solar_gain / 1000) : Json()}, {"neighbours", prisms.size()},
                                   {"light", Json::object({{"transmittance", light.transmittance}, {"reflectance", light.reflectance}, {"gain", light.gain}})}}));
  }
  // Quantities, cost and heat.
  {
    ap::Quantities q = ap::quantities(b, R, 1.0);
    // The roof as it was read: its own surface over the top storey, and the walls carried up to it.
    if (roofs_area > 0) q.roof = std::max(0.0, q.roof_plan - roofed_floor) + roofs_area;
    q.outer_wall += roofs_wall, q.outer_wall_net += roofs_wall;
    out.set("quantities",
            Json::object({{"floor_area", q.floor_area}, {"internal_area", q.internal_area}, {"wet_area", q.wet_area}, {"ground_slab", q.ground_slab},
                          {"upper_floor", q.upper_floor}, {"roof_plan", q.roof_plan}, {"roof", q.roof}, {"outer_length", q.outer_length},
                          {"outer_wall", q.outer_wall}, {"outer_wall_net", q.outer_wall_net}, {"inner_wall_net", q.inner_wall_net},
                          {"window_area", q.window_area}, {"glass_area", q.glass_area}, {"outer_door_area", q.outer_door_area}, {"windows", q.windows},
                          {"outer_doors", q.outer_doors}, {"inner_doors", q.inner_doors}, {"stairs", q.stairs},
                          {"wall_by_side", Json::array({q.wall_by_side[0], q.wall_by_side[1], q.wall_by_side[2], q.wall_by_side[3]})},
                          {"glass_by_side", Json::array({q.glass_by_side[0], q.glass_by_side[1], q.glass_by_side[2], q.glass_by_side[3]})}}));
    std::map<std::string, ap::Rate> rates;
    const Json& items = doc["rates"]["items"];
    if (items.is_object())
      for (const auto& [id, r] : items.as_object()) {
        const double low = detail::number(r[0], 0), high = detail::number(r[1], low);
        if (low < 0 || high < low) throw std::runtime_error("a rate is [low, high] with 0 ≤ low ≤ high: " + id);
        rates[id] = {low, high};
      }
    const ap::Cost c = ap::cost(q, rates);
    Json lines = Json::array();
    for (const auto& item : c.items) lines.push(Json::object({{"id", item.id}, {"unit", item.unit}, {"quantity", item.quantity}, {"low", item.low}, {"high", item.high}}));
    out.set("cost", Json::object({{"currency", doc["rates"]["currency"].text()}, {"low", c.low}, {"high", c.high}, {"items", lines}}));
    ap::Fabric f;
    const Json& jf = doc["fabric"];
    f.wall = detail::number(jf["wall"], f.wall), f.window = detail::number(jf["window"], f.window), f.door = detail::number(jf["door"], f.door);
    f.roof = detail::number(jf["roof"], f.roof), f.ground = detail::number(jf["ground"], f.ground);
    const ap::HeatLoss h = ap::heat_loss(q, f);
    // A day's balance at declared temperatures: what the fabric loses in 24 hours, beside what the Sun gives through the glass.
    const double indoor = detail::number(doc["light"]["indoor"], 20), outdoor = detail::number(doc["light"]["outdoor"], 8);
    out.set("heat", Json::object({{"wall", h.wall}, {"window", h.window}, {"door", h.door}, {"roof", h.roof}, {"ground", h.ground}, {"total", h.total},
                                  {"per_floor", h.per_floor}, {"indoor", indoor}, {"outdoor", outdoor}, {"day_loss", h.total * (indoor - outdoor) * 24 / 1000},
                                  {"day_gain", std::isnan(day_gain) ? Json() : Json(day_gain)}}));
  }
  return out;
}

inline Json answer(const Json& request) {
  const std::string op = request["op"].text();
  if (op == "vocabulary") return vocabulary();
  if (op == "read") return read(request["building"], request["around"]);
  throw std::runtime_error("no such request: " + op);
}

}  // namespace science::building
