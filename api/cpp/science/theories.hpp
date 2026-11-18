// Theories of the case for any work's engine (ADR 0127): the law domain's `theories` composed with tekmerion's scenarios and their
// search, kinesis's prisms and the law domain's matter, answering requests in JSON. A request holds everything it concerns (the
// scheme, the matter, the search's state); nothing is read from a file or kept between requests.
//
//   theories  {"scheme", "matter", "state"?, "generations"?, "proposals"?: [{source, text, parents}], "requests"?: {budget, roles},
//              "now"?}   the search run a few generations from its state (or begun, the matter's declared theories proposed first);
//              every theory kept tested once on what the matter holds back, the test sealed in the journal before it is read; the
//              issues over the theories kept; whether each person could have been at the scene under the readings of its terms;
//              the evidence as it is drawn; the state to give back
//   assess    {"scheme", "matter", "text"}   one theory written by a person: its assessment and what the law makes of it
//   (and every operation of science/law.hpp)
//
// The matter (noetarium.matter/1) gains, for theories:
//   epoch: "2026-03-14T21:30:00+00:00"   instants are seconds after it, or clock times "21:43[:20]" on its day in its zone, or ISO
//   place: {scene: {x, y, name}, ways: [{name, kind, points: [[x, y]]}], buildings: [{name, points}], water: [{name, points}]}
//          in metres east and north of a point the matter names (the page draws it; the engine reads only the scene)
//   agents: [{id, name, speed, basis, identified}]   kinds: [{id, gloss, roles}]   story: [{name, gloss, kinds}]
//   rooms: [{kind, x, y, radius, from, until}]
//   located: [{id, says: sighting | event | absence | interval, agent, kind, role, before, after, least, most, x, y, radius, at |
//              from, until, credibility, source, modality, document, passage, text, held_back}]
//   bindings: {makes: [{kind, role, agent, fact}], grounds: [{item, fact}]}
//   theories: [{by, text}]   accounts declared by the parties, written as lines (tekmerion's form, instants as clock times)
//   presence: {agents, scene?: {x, y}, places: [{name, radius, source, by, stated}], times: [{name, from, until, source, by, stated}]}
//   search: {generations, mu, lambda, survival, parents, schedule, credit, crossover_rate, generator_rate, from_archive, capacity,
//            min_distance, invention, hole, key, threshold, operators: {name: weight}}
//
// The facts a theory addresses (its bindings') are set by it; every other fact enters as the matter declares it. The methods are
// the domain's and the foundations'; the engine only reads, calls and writes.

#pragma once

#include "evolution_state.hpp"
#include "json.hpp"
#include "law.hpp"

#include <law/theories/theories.hpp>

#include <archeion/fixity/journal.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::theories {

namespace ts = tekmerion::scenarios;
namespace tc = tekmerion::construal;
namespace lt = ::law::theories;
namespace lw = ::law;
namespace ai = apodeixis::interpretation;
namespace es = science::evolution_state;
namespace sl = science::law;
namespace hev = heuresis::evolution;
using es::items;
using es::num;

inline double number_or(const Json& j, const char* key, double fallback) { return j[key].is_number() ? j[key].as_number() : fallback; }
inline std::size_t count_or(const Json& j, const char* key, std::size_t fallback) { return j[key].is_number() ? static_cast<std::size_t>(j[key].as_number()) : fallback; }

// ---- Time ------------------------------------------------------------------------------------------------------------------------

namespace detail {

inline long days_from_civil(long y, int m, int d) {
  y -= m <= 2;
  const long era = (y >= 0 ? y : y - 399) / 400;
  const long yoe = y - era * 400;
  const long doy = (153 * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
  const long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
  return era * 146097 + doe - 719468;
}

inline void civil_from_days(long z, long& y, int& m, int& d) {
  z += 719468;
  const long era = (z >= 0 ? z : z - 146096) / 146097;
  const long doe = z - era * 146097;
  const long yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
  y = yoe + era * 400;
  const long doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
  const long mp = (5 * doy + 2) / 153;
  d = static_cast<int>(doy - (153 * mp + 2) / 5 + 1);
  m = static_cast<int>(mp < 10 ? mp + 3 : mp - 9);
  y += m <= 2;
}

inline bool digits(const std::string& s, std::size_t at, std::size_t n, int& v) {
  if (at + n > s.size()) return false;
  v = 0;
  for (std::size_t k = 0; k < n; k++) {
    if (s[at + k] < '0' || s[at + k] > '9') return false;
    v = v * 10 + (s[at + k] - '0');
  }
  return true;
}

// "HH:MM[:SS[.f]]" from `at`: seconds of the day, and where it ends.
inline bool time_of_day(const std::string& s, std::size_t at, double& seconds, std::size_t& end) {
  int h = 0, mi = 0, se = 0;
  if (!digits(s, at, 2, h) || at + 2 >= s.size() || s[at + 2] != ':' || !digits(s, at + 3, 2, mi)) return false;
  end = at + 5;
  double frac = 0;
  if (end < s.size() && s[end] == ':') {
    if (!digits(s, end + 1, 2, se)) return false;
    end += 3;
    if (end < s.size() && s[end] == '.') {
      std::size_t k = end + 1;
      double scale = 0.1;
      while (k < s.size() && s[k] >= '0' && s[k] <= '9') frac += scale * (s[k] - '0'), scale /= 10, k++;
      end = k;
    }
  }
  if (h > 24 || mi > 59 || se > 60) return false;
  seconds = h * 3600.0 + mi * 60.0 + se + frac;
  return true;
}

// A zone at the end: "Z", "+HH:MM", "-HHMM"; seconds east of UTC.
inline bool zone(const std::string& s, std::size_t at, double& offset) {
  if (at == s.size()) return false;
  if (s[at] == 'Z' || s[at] == 'z') {
    offset = 0;
    return at + 1 == s.size();
  }
  if (s[at] != '+' && s[at] != '-') return false;
  int h = 0, mi = 0;
  if (!digits(s, at + 1, 2, h)) return false;
  std::size_t k = at + 3;
  if (k < s.size() && s[k] == ':') k++;
  if (k < s.size() && !digits(s, k, 2, mi)) return false;
  offset = (s[at] == '-' ? -1 : 1) * (h * 3600.0 + mi * 60.0);
  return true;
}

}  // namespace detail

struct Clock {
  double epoch = 0;   // seconds since 1970 (UTC)
  double offset = 0;  // the epoch's zone, seconds east of UTC
  long day = 0;       // the epoch's date in its zone, days since 1970
  bool given = false;

  explicit Clock(const Json& j = Json()) {
    if (!j.is_string() || j.as_string().empty()) return;
    const std::string s = j.as_string();
    double tod = 0;
    if (!absolute(s, epoch, &offset, &tod)) throw std::invalid_argument("the matter's epoch is an ISO 8601 instant, not " + s);
    day = static_cast<long>(std::floor((epoch + offset) / 86400));
    given = true;
  }

  // An ISO 8601 date and time, its zone stated or (failing that) the epoch's; seconds since 1970.
  bool absolute(const std::string& s, double& out, double* zone_out = nullptr, double* tod_out = nullptr) const {
    int y = 0, mo = 0, d = 0;
    if (!detail::digits(s, 0, 4, y) || s.size() < 16 || s[4] != '-' || !detail::digits(s, 5, 2, mo) || s[7] != '-' || !detail::digits(s, 8, 2, d) || (s[10] != 'T' && s[10] != ' ')) return false;
    double tod = 0, z = offset;
    std::size_t end = 0;
    if (!detail::time_of_day(s, 11, tod, end)) return false;
    if (end < s.size() && !detail::zone(s, end, z)) return false;
    out = static_cast<double>(detail::days_from_civil(y, mo, d)) * 86400 + tod - z;
    if (zone_out) *zone_out = z;
    if (tod_out) *tod_out = tod;
    return true;
  }

  // Seconds after the epoch: a number as it is; a clock time on the epoch's day in its zone; an ISO instant.
  double seconds(const Json& v, const std::string& what = "an instant") const {
    if (v.is_number()) return v.as_number();
    if (!v.is_string()) throw std::invalid_argument(what + " is a number of seconds, a clock time or an ISO 8601 instant");
    const std::string s = v.as_string();
    double t = 0, tod = 0;
    std::size_t end = 0;
    if (s.size() >= 16 && absolute(s, t)) return t - epoch;
    if (detail::time_of_day(s, 0, tod, end) && end == s.size()) {
      if (!given) throw std::invalid_argument(what + " " + s + " is a clock time, and the matter states no epoch");
      return static_cast<double>(day) * 86400 + tod - offset - epoch;
    }
    char* stop = nullptr;
    const double n = std::strtod(s.c_str(), &stop);
    if (stop && *stop == '\0' && std::isfinite(n)) return n;
    throw std::invalid_argument(what + " " + s + " is not a number of seconds, a clock time or an ISO 8601 instant");
  }

  // A clock time "HH:MM:SS" on the epoch's day, or the local date and time when it falls on another.
  std::string clock(double t) const {
    if (!given || !std::isfinite(t)) {
      char b[32];
      std::snprintf(b, sizeof b, "%.0f s", t);
      return b;
    }
    const double local = epoch + offset + t;
    const long d = static_cast<long>(std::floor(local / 86400));
    const long sec = static_cast<long>(std::llround(local - static_cast<double>(d) * 86400));
    const long h = sec / 3600 % 24, mi = sec / 60 % 60, se = sec % 60;
    char b[48];
    if (d == day) {
      std::snprintf(b, sizeof b, "%02ld:%02ld:%02ld", h, mi, se);
    } else {
      long y = 0;
      int m = 0, dd = 0;
      detail::civil_from_days(d, y, m, dd);
      std::snprintf(b, sizeof b, "%04ld-%02d-%02dT%02ld:%02ld:%02ld", y, m, dd, h, mi, se);
    }
    return b;
  }

  Json describe() const {
    return Json::object({{"given", given}, {"epoch", epoch}, {"offset", offset}, {"seconds_of_day", given ? std::fmod(epoch + offset, 86400.0) : 0.0}, {"clock", clock(0)}});
  }
};

// A theory's lines with instants as clock times, for people and models; and back.
inline std::string to_people(const std::string& text, const Clock& c) {
  if (!c.given) return text;
  std::string out, line;
  std::size_t from = 0;
  while (from <= text.size()) {
    std::size_t to = text.find('\n', from);
    if (to == std::string::npos) to = text.size();
    line = text.substr(from, to - from);
    const auto at = line.find(" when ");
    if (at != std::string::npos) {
      char* stop = nullptr;
      const std::string tail = line.substr(at + 6);
      const double t = std::strtod(tail.c_str(), &stop);
      if (stop && *stop == '\0') line = line.substr(0, at) + " when " + c.clock(t);
    }
    if (!line.empty()) out += line + "\n";
    if (to == text.size()) break;
    from = to + 1;
  }
  return out;
}

inline std::string from_people(const std::string& text, const Clock& c) {
  std::string out, line;
  std::size_t from = 0;
  while (from <= text.size()) {
    std::size_t to = text.find('\n', from);
    if (to == std::string::npos) to = text.size();
    line = text.substr(from, to - from);
    while (!line.empty() && (line.back() == '\r' || line.back() == ' ')) line.pop_back();
    const auto at = line.find(" when ");
    if (at != std::string::npos) {
      const std::string tail = line.substr(at + 6);
      if (tail.find(':') != std::string::npos) {
        char b[48];
        std::snprintf(b, sizeof b, "%.3f", c.seconds(Json(tail), "an instant of a theory"));
        line = line.substr(0, at) + " when " + b;
      }
    }
    out += line + "\n";
    if (to == text.size()) break;
    from = to + 1;
  }
  return out;
}

// ---- The matter read ------------------------------------------------------------------------------------------------------------

inline std::vector<std::string> texts(const Json& j) {
  std::vector<std::string> out;
  for (const auto& x : items(j)) out.push_back(x.text());
  return out;
}

inline ts::Problem problem_of(const Json& m, const Clock& c) {
  ts::Problem p;
  for (const auto& a : items(m["agents"])) p.agents.push_back({a["id"].text(), a["name"].text(a["id"].text()), number_or(a, "speed", 1.4), !a["identified"].is_bool() || a["identified"].as_bool()});
  for (const auto& k : items(m["kinds"])) p.kinds.push_back({k["id"].text(), k["gloss"].text(), texts(k["roles"])});
  for (const auto& s : items(m["story"])) p.scheme.push_back({s["name"].text(), s["gloss"].text(), texts(s["kinds"])});
  for (const auto& r : items(m["rooms"]))
    p.rooms.push_back({r["kind"].text(), number_or(r, "x", 0), number_or(r, "y", 0), number_or(r, "radius", 0), c.seconds(r["from"], "a room's beginning"), c.seconds(r["until"], "a room's end")});
  for (const auto& x : items(m["located"])) {
    ts::Item i;
    i.id = x["id"].text(), i.source = x["source"].text(), i.text = x["text"].text(), i.modality = x["modality"].text(), i.holding = x["document"].text();
    i.says = ts::says_of(x["says"].text("sighting"));
    i.agent = x["agent"].text(), i.kind = x["kind"].text(), i.role = x["role"].text("actor"), i.before = x["before"].text(), i.after = x["after"].text();
    if (x["least"].is_number()) i.least = x["least"].as_number();
    if (x["most"].is_number()) i.most = x["most"].as_number();
    i.x = number_or(x, "x", 0), i.y = number_or(x, "y", 0), i.radius = number_or(x, "radius", 0);
    if (x.has("at")) i.from = i.until = c.seconds(x["at"], "the instant of " + i.id);
    else if (x.has("from")) i.from = c.seconds(x["from"], "the beginning of " + i.id), i.until = x.has("until") ? c.seconds(x["until"], "the end of " + i.id) : i.from;
    i.credibility = number_or(x, "credibility", 0.5);
    i.held_back = x["held_back"].is_bool() && x["held_back"].as_bool();
    p.items.push_back(std::move(i));
  }
  return p;
}

inline lt::Bindings bindings_of(const Json& m) {
  lt::Bindings b;
  for (const auto& x : items(m["bindings"]["makes"])) b.makes.push_back({x["kind"].text(), x["role"].text("actor"), x["agent"].text(), x["fact"].text()});
  for (const auto& x : items(m["bindings"]["grounds"])) b.grounds.push_back({x["item"].text(), x["fact"].text()});
  return b;
}

inline ts::SearchOptions options_of(const Json& q) {
  ts::SearchOptions o;
  o.capacity = 1;
  auto& st = o.strategy;
  st.mu = count_or(q, "mu", st.mu), st.lambda = count_or(q, "lambda", st.lambda);
  if (q["survival"].is_string()) st.survival = hev::of<hev::Survival>(q["survival"].as_string());
  if (q["parents"].is_string()) st.parents = hev::of<hev::Parents>(q["parents"].as_string());
  if (q["schedule"].is_string()) st.schedule = hev::of<hev::Schedule>(q["schedule"].as_string());
  if (q["credit"].is_string()) st.credit = hev::of<hev::Credit>(q["credit"].as_string());
  st.crossover_rate = number_or(q, "crossover_rate", st.crossover_rate), st.generator_rate = number_or(q, "generator_rate", st.generator_rate);
  st.from_archive = number_or(q, "from_archive", st.from_archive);
  o.capacity = count_or(q, "capacity", o.capacity), o.min_distance = number_or(q, "min_distance", o.min_distance);
  o.assess.invention = number_or(q, "invention", o.assess.invention), o.assess.hole = number_or(q, "hole", o.assess.hole);
  o.key = static_cast<std::uint64_t>(number_or(q, "key", static_cast<double>(o.key)));
  if (q["operators"].is_object())
    for (const auto& [k, v] : q["operators"].as_object()) o.operators[k] = v.as_number();
  return o;
}

// ---- Scenarios as JSON, exactly --------------------------------------------------------------------------------------------------

inline Json scenario_json(const ts::Scenario& s) {
  Json ev = Json::array(), st = Json::array();
  for (const auto& e : s.events) {
    Json roles = Json::object();
    for (const auto& [r, a] : e.roles) roles.set(r, a);
    ev.push(Json::object({{"kind", e.kind}, {"roles", roles}, {"x", e.x}, {"y", e.y}, {"t", e.t}}));
  }
  for (const auto& x : s.stances) st.push(Json::array({x.believed ? 1 : 0, num(x.t)}));
  return Json::object({{"events", ev}, {"stances", st}});
}
inline ts::Scenario scenario_of(const Json& j) {
  ts::Scenario s;
  for (const auto& e : items(j["events"])) {
    ts::Event x;
    x.kind = e["kind"].text(), x.x = e["x"].as_number(), x.y = e["y"].as_number(), x.t = e["t"].as_number();
    if (e["roles"].is_object())
      for (const auto& [r, a] : e["roles"].as_object()) x.roles[r] = a.text();
    s.events.push_back(std::move(x));
  }
  for (const auto& x : items(j["stances"])) s.stances.push_back({x[0].as_number() != 0, x[1].is_number() ? x[1].as_number() : std::nan("")});
  return s;
}

// ---- What a request finds --------------------------------------------------------------------------------------------------------

struct Law {
  bool given = false;
  sl::Read rd;
  std::vector<std::string> issues, questions, burdens;
  lw::facts::Record record;
  std::map<std::string, std::vector<lt::Outcome>> cache;  // by the facts a theory makes hold
};

inline Law law_of(const Json& r, const Json& m) {
  Law l;
  if (!r["scheme"].is_object()) return l;
  l.given = true;
  l.rd = sl::scheme_of(r["scheme"]);
  for (const auto& x : items(m["issues"])) l.issues.push_back(x["id"].text()), l.questions.push_back(x["question"].text()), l.burdens.push_back(x["burden"].text());
  for (const auto& x : items(m["facts"])) {
    auto f = lw::facts::fact(x["id"].text(), x["text"].text());
    f.standing = sl::standing_of(x["standing"].text("alleged"));
    l.record.facts.push_back(std::move(f));
  }
  return l;
}

inline const std::vector<lt::Outcome>& outcomes(Law& l, const Json& m, const lt::Bindings& b, const std::set<std::string>& holds) {
  std::string key;
  for (const auto& h : holds) key += h + "\x1f";
  auto it = l.cache.find(key);
  if (it != l.cache.end()) return it->second;
  std::vector<lt::Outcome> out;
  if (l.given && !l.issues.empty()) {
    const auto under = lt::record_under(l.record, b.addressed(), holds);
    out = lt::outcomes(under, l.issues, l.questions, [&](const lw::facts::Record& rec) { return sl::built(l.rd, m, rec, l.questions).c; });
  }
  return l.cache.emplace(key, std::move(out)).first->second;
}

inline Json fix_json(const kinesis::prisms::Fix& f, const Clock& c) {
  return Json::object({{"id", f.id}, {"x", f.x}, {"y", f.y}, {"t", f.t}, {"clock", c.clock(f.t)}, {"radius", f.radius}});
}

inline Json theory_json(const ts::Search& s, const hev::Member<ts::Scenario>& m, const lt::Theory& t, const Clock& c, const std::string& note, std::size_t admitted, const std::vector<lt::Confirmation>& tests) {
  const auto& p = s.problem();
  const auto& a = t.assessment;
  Json events = Json::array(), readings = Json::array(), trajectories = Json::array(), outs = Json::array();
  for (const auto& e : t.scenario.events) {
    Json roles = Json::object();
    for (const auto& [r, ag] : e.roles) roles.set(r, ag);
    events.push(Json::object({{"kind", e.kind}, {"roles", roles}, {"x", e.x}, {"y", e.y}, {"t", e.t}, {"clock", c.clock(e.t)}}));
  }
  for (std::size_t k = 0; k < a.readings.size(); k++) {
    const auto& r = a.readings[k];
    Json one = Json::object({{"item", r.item}, {"believed", r.believed}, {"counted", r.counted}, {"status", r.status}, {"cost", num(r.cost)}, {"event", static_cast<double>(r.event)}, {"note", r.note}});
    if (p.items[k].says == ts::Says::sighting && r.believed) {
      const double at = std::isnan(t.scenario.stances[k].t) ? (p.items[k].from + p.items[k].until) / 2 : t.scenario.stances[k].t;
      one.set("t", at), one.set("clock", c.clock(at));
    }
    readings.push(one);
  }
  for (const auto& tr : a.trajectories) {
    Json fixes = Json::array(), witness = Json::array();
    for (const auto& f : tr.fixes) fixes.push(fix_json(f, c));
    if (tr.consistency.witness)
      for (const auto& f : tr.consistency.witness->at) witness.push(Json::object({{"x", f.x}, {"y", f.y}, {"t", f.t}}));
    trajectories.push(Json::object({{"agent", tr.agent}, {"status", tr.consistency.status}, {"margin", num(tr.consistency.margin)}, {"reason", tr.consistency.reason}, {"fixes", fixes}, {"witness", witness}}));
  }
  for (const auto& o : t.outcomes) outs.push(Json::object({{"issue", o.issue}, {"class", std::string(ai::word(o.cls))}}));
  Json test;
  for (const auto& x : tests)
    if (x.key == ts::key(t.scenario))
      test = Json::object({{"outcome", x.outcome}, {"doubted", x.doubted}, {"cost", num(x.cost)}, {"tried", x.tried}, {"reason", x.reason}, {"registered", x.registered}, {"written", x.written}});
  std::vector<double> parents;
  for (const auto& q : m.parents) parents.push_back(static_cast<double>(heuresis::search::serial_of(q)));
  return Json::object({{"serial", static_cast<double>(heuresis::search::serial_of(m.ticket))}, {"ticket", static_cast<double>(m.ticket)}, {"text", to_people(ts::write(p, t.scenario), c)},
                       {"genotype", scenario_json(t.scenario)}, {"central", t.central}, {"niche", m.evaluation.descriptors}, {"score", num(a.score)}, {"implausibility", num(a.implausibility)},
                       {"plausibility", num(t.plausibility)}, {"coherent", a.coherent}, {"reason", a.reason}, {"events", events}, {"readings", readings}, {"doubted", a.doubted},
                       {"inventions", std::vector<double>(a.inventions.begin(), a.inventions.end())}, {"surplus", std::vector<double>(a.surplus.begin(), a.surplus.end())},
                       {"unnamed", a.unnamed}, {"holes", a.holes}, {"trajectories", trajectories}, {"outcomes", outs},
                       {"holds", std::vector<std::string>(t.holds.begin(), t.holds.end())}, {"test", test}, {"note", note}, {"admitted", admitted},
                       {"operation", m.operation}, {"parents", parents}});
}

inline Json standing_json(const lt::Standing& st, const std::vector<lt::Theory>& found) {
  const auto serials = [&](const std::vector<std::size_t>& v) {
    std::vector<double> out;
    for (auto k : v) out.push_back(static_cast<double>(heuresis::search::serial_of(found[k].ticket)));
    return out;
  };
  return Json::object({{"issue", st.issue}, {"question", st.question}, {"burden", st.burden}, {"follows", serials(st.follows)}, {"arguable", serials(st.arguable)},
                       {"fails", serials(st.fails)}, {"for", num(st.for_it)}, {"open", num(st.open)}, {"against", num(st.against)},
                       {"best_alternative", st.best_alternative ? Json(static_cast<double>(heuresis::search::serial_of(found[*st.best_alternative].ticket))) : Json()},
                       {"best_open", st.best_open ? Json(static_cast<double>(heuresis::search::serial_of(found[*st.best_open].ticket))) : Json()},
                       {"best_for", st.best_for ? Json(static_cast<double>(heuresis::search::serial_of(found[*st.best_for].ticket))) : Json()},
                       {"best_against", st.best_against ? Json(static_cast<double>(heuresis::search::serial_of(found[*st.best_against].ticket))) : Json()},
                       {"over", std::string(ai::word(st.over))}, {"threshold", st.threshold}, {"verdict", sl::verdict_json(st.verdict)}});
}

inline Json presence_json(const ts::Problem& p, const Json& m, const Clock& c) {
  const Json& pr = m["presence"];
  Json out = Json::array();
  if (!pr.is_object()) return out;
  const Json& scene = pr["scene"].is_object() ? pr["scene"] : m["place"]["scene"];
  const double x = number_or(scene, "x", 0), y = number_or(scene, "y", 0);
  std::vector<lt::PlaceReading> places;
  std::vector<lt::TimeReading> times;
  for (const auto& r : items(pr["places"]))
    places.push_back({r["name"].text(), tc::source_of(r["source"].text("analyst")), r["by"].text(), number_or(r, "radius", 0), r["stated"].is_bool() && r["stated"].as_bool()});
  for (const auto& r : items(pr["times"]))
    times.push_back({r["name"].text(), tc::source_of(r["source"].text("analyst")), r["by"].text(), c.seconds(r["from"], "a reading's beginning"), c.seconds(r["until"], "a reading's end"), r["stated"].is_bool() && r["stated"].as_bool()});
  if (places.empty() || times.empty()) return out;
  std::vector<std::string> who = texts(pr["agents"]);
  if (who.empty())
    for (const auto& a : p.agents)
      if (a.identified) who.push_back(a.id);
  for (const auto& a : who) {
    const auto res = lt::could_have_been_there(p, a, x, y, places, times);
    const auto& k = res.construal;
    Json each = Json::array(), pivots = Json::array(), points = Json::array();
    const std::size_t n = ai::space(k.points);
    for (std::size_t i = 0; i < n; i++) {
      const auto in = ai::decode(k.points, i);
      Json windows = Json::array();
      const auto& pres = res.each[i];
      for (const auto& w : pres.reach.windows) windows.push(Json::object({{"from", w.from}, {"until", w.until}, {"from_clock", c.clock(w.from)}, {"until_clock", c.clock(w.until)}}));
      each.push(Json::object({{"place", k.points[0].readings[in[0]]}, {"time", k.points[1].readings[in[1]]}, {"holds", k.holds[i] != 0}, {"admissible", k.admissible[i] != 0},
                              {"reason", k.reasons[i]}, {"windows", windows}, {"least", num(pres.reach.least)}}));
    }
    for (const auto& pv : k.pivots) {
      std::vector<std::string> flips;
      for (auto f : pv.flips) flips.push_back(k.points[pv.point].readings[f]);
      pivots.push(Json::object({{"point", k.points[pv.point].name}, {"flips", flips}}));
    }
    for (const auto& pt : k.declared) {
      Json rs = Json::array();
      for (const auto& rd : pt.readings) rs.push(Json::object({{"name", rd.name}, {"source", tc::name(rd.source)}, {"by", rd.by}, {"stated", rd.stated}}));
      points.push(Json::object({{"name", pt.name}, {"readings", rs}}));
    }
    Json fixes = Json::array();
    for (const auto& f : lt::fixes(p, a)) fixes.push(fix_json(f, c));
    const long ai_ = p.agent(a);
    out.push(Json::object({{"agent", a}, {"name", p.agents[static_cast<std::size_t>(ai_)].name}, {"claim", k.claim}, {"class", std::string(ai::word(k.record))},
                           {"share", num(k.share)}, {"verdict", sl::verdict_json(k.verdict)}, {"points", points}, {"each", each}, {"pivots", pivots}, {"fixes", fixes}}));
  }
  return out;
}

inline Json evidence_json(const ts::Problem& p, const Clock& c) {
  Json agents = Json::array(), located = Json::array(), rooms = Json::array();
  for (const auto& a : p.agents) {
    Json fixes = Json::array();
    for (const auto& f : lt::fixes(p, a.id)) fixes.push(fix_json(f, c));
    agents.push(Json::object({{"id", a.id}, {"name", a.name}, {"speed", a.speed}, {"identified", a.identified}, {"fixes", fixes}}));
  }
  for (const auto& i : p.items)
    located.push(Json::object({{"id", i.id}, {"says", ts::word(i.says)}, {"agent", i.agent}, {"kind", i.kind}, {"x", i.x}, {"y", i.y}, {"radius", i.radius}, {"from", i.from}, {"until", i.until},
                               {"from_clock", c.clock(i.from)}, {"until_clock", c.clock(i.until)}, {"credibility", i.credibility}, {"held_back", i.held_back}, {"modality", i.modality},
                               {"source", i.source}, {"document", i.holding}, {"text", i.text}}));
  for (const auto& r : p.rooms) rooms.push(Json::object({{"kind", r.kind}, {"x", r.x}, {"y", r.y}, {"radius", r.radius}, {"from", r.from}, {"until", r.until}, {"from_clock", c.clock(r.from)}, {"until_clock", c.clock(r.until)}}));
  return Json::object({{"agents", agents}, {"located", located}, {"rooms", rooms}, {"epoch", c.describe()}});
}

// ---- The operations ------------------------------------------------------------------------------------------------------------

inline Json tests_json(const std::vector<lt::Confirmation>& v) {
  Json a = Json::array();
  for (const auto& x : v)
    a.push(Json::object({{"ticket", static_cast<double>(x.ticket)}, {"key", x.key}, {"text", x.text}, {"outcome", x.outcome}, {"doubted", x.doubted}, {"cost", num(x.cost)},
                         {"tried", x.tried}, {"registered", x.registered}, {"written", x.written}, {"reason", x.reason}}));
  return a;
}
inline std::vector<lt::Confirmation> tests_of(const Json& j) {
  std::vector<lt::Confirmation> v;
  for (const auto& x : items(j)) {
    lt::Confirmation c;
    c.ticket = static_cast<heuresis::search::Ticket>(x["ticket"].as_number()), c.key = x["key"].text(), c.text = x["text"].text(), c.outcome = x["outcome"].text();
    c.doubted = texts(x["doubted"]), c.cost = number_or(x, "cost", 0), c.tried = count_or(x, "tried", 0), c.registered = x["registered"].text(), c.written = x["written"].text(), c.reason = x["reason"].text();
    v.push_back(std::move(c));
  }
  return v;
}

inline Json theories(const Json& r) {
  const Json& m = r["matter"];
  const Clock c(m["epoch"]);
  const ts::Problem p = problem_of(m, c);
  if (const auto f = ts::faults(p); !f.empty()) {
    std::string all;
    for (const auto& x : f) all += (all.empty() ? "" : "; ") + x;
    throw std::invalid_argument("the matter's evidence in space and time has faults: " + all);
  }
  const lt::Bindings b = bindings_of(m);
  Law l = law_of(r, m);
  ts::SearchOptions o = options_of(m["search"]);
  o.describe = [&](const ts::Scenario& s) {
    std::vector<double> d;
    for (const auto& x : outcomes(l, m, b, lt::holding(p, s, b))) d.push_back(lt::code(x.cls));
    return d;
  };
  const bool resumed = r["state"].is_object();
  std::unique_ptr<ts::Search> s;
  if (resumed) s = std::make_unique<ts::Search>(p, o, es::state_of<ts::Scenario>(r["state"]["evolution"], scenario_of));
  else s = std::make_unique<ts::Search>(p, o);
  // The parties' declared theories, proposed when the search begins; proposals given with the request.
  Json refused = Json::array();
  const auto propose = [&](const std::string& source, const std::string& text, const std::vector<heuresis::search::Ticket>& parents) {
    const auto rd = ts::read(p, from_people(text, c));
    if (!rd.ok) {
      refused.push(Json::object({{"source", source}, {"text", text}, {"why", rd.errors}}));
      return;
    }
    s->propose({rd.scenario}, source, parents.empty() ? std::vector<std::vector<heuresis::search::Ticket>>{} : std::vector<std::vector<heuresis::search::Ticket>>{parents});
  };
  // Proposals wait for the next generation and are not kept in the state: the declared theories are proposed whenever the search
  // has not yet run a generation.
  if (s->evolution().generation() == 0)
    for (const auto& t : items(m["theories"])) propose("declared:" + t["by"].text("a party"), t["text"].text(), {});
  for (const auto& q : items(r["proposals"])) {
    std::vector<heuresis::search::Ticket> parents;
    for (const auto& x : items(q["parents"])) parents.push_back(static_cast<heuresis::search::Ticket>(x.as_number()));
    propose(q["source"].text("a proposal"), q["text"].text(), parents);
  }
  s->run(count_or(r, "generations", count_or(m["search"], "generations", 10)));
  // The tests: each theory kept, tested once on what is held back, registered in the journal before it is read.
  lt::Confirmer conf;
  for (const auto& en : items(r["state"]["journal"])) conf.journal.entries.push_back({en["when"].text(), en["who"].text(), en["what"].text(), en["about"].text(), en["note"].text(), en["seal"].text()});
  if (const auto v = archeion::fixity::verify(conf.journal); !v.holds()) throw std::invalid_argument("the search's journal does not verify: " + v.reason);
  conf.done = tests_of(r["state"]["tests"]);
  std::string now = r["now"].text("1970-01-01T00:00:00Z");
  if (!conf.journal.entries.empty() && now < conf.journal.entries.back().when) now = conf.journal.entries.back().when;
  const auto& archive = s->evolution().archive();
  std::vector<std::size_t> order(archive.size());
  for (std::size_t k = 0; k < order.size(); k++) order[k] = k;
  std::stable_sort(order.begin(), order.end(), [&](std::size_t a, std::size_t b2) { return archive[a].member.evaluation.objectives[0] < archive[b2].member.evaluation.objectives[0]; });
  for (auto k : order) conf.confirm(p, archive[k].member.ticket, archive[k].member.genotype, now, "the search", o.assess, o.key);
  // The theories kept, with what the law makes of each.
  std::vector<double> scores;
  for (auto k : order) scores.push_back(archive[k].member.evaluation.objectives[0]);
  const auto w = ts::relative_plausibility(scores);
  std::vector<lt::Theory> found;
  for (std::size_t j = 0; j < order.size(); j++) {
    const auto& km = archive[order[j]].member;
    lt::Theory t;
    t.ticket = km.ticket, t.scenario = km.genotype, t.assessment = s->assessment(km.genotype), t.plausibility = w[j];
    t.holds = lt::holding(p, km.genotype, b), t.outcomes = outcomes(l, m, b, t.holds), t.central = s->central_story(km.genotype), t.note = archive[order[j]].note;
    found.push_back(std::move(t));
  }
  const double threshold = number_or(m["search"], "threshold", 0.05);
  const auto st = lt::standings(found, l.issues, l.questions, l.burdens, threshold);
  Json kept = Json::array(), standings = Json::array(), population = Json::array(), stats = Json::array(), retired = Json::array();
  for (std::size_t j = 0; j < found.size(); j++) kept.push(theory_json(*s, archive[order[j]].member, found[j], c, archive[order[j]].note, archive[order[j]].admitted, conf.done));
  for (const auto& x : st) standings.push(standing_json(x, found));
  {
    const auto rank = s->evolution().ranking();
    for (std::size_t k = 0; k < rank.size() && k < 16; k++) {
      const auto& mm = s->evolution().population()[rank[k]];
      population.push(Json::object({{"serial", static_cast<double>(heuresis::search::serial_of(mm.ticket))}, {"score", num(mm.evaluation.feasible() ? mm.evaluation.objectives[0] : NAN)},
                                    {"coherent", mm.evaluation.feasible()}, {"violation", num(mm.evaluation.violation())}, {"central", s->central_story(mm.genotype)},
                                    {"operation", mm.operation}, {"text", to_people(ts::write(p, mm.genotype), c)}}));
    }
  }
  for (const auto& x : s->evolution().statistics()) stats.push(es::statistics_json(x));
  for (const auto& x : s->evolution().retired())
    retired.push(Json::object({{"serial", static_cast<double>(heuresis::search::serial_of(x.member.ticket))}, {"central", s->central_story(x.member.genotype)},
                               {"score", num(x.member.evaluation.objectives.empty() ? NAN : x.member.evaluation.objectives[0])}, {"admitted", x.admitted}, {"retired", x.retired}, {"reason", x.reason}}));
  Json journal = Json::array();
  for (const auto& en : conf.journal.entries) journal.push(Json::object({{"when", en.when}, {"who", en.who}, {"what", en.what}, {"about", en.about}, {"note", en.note}, {"seal", en.seal}}));
  Json out = Json::object({{"generation", s->evolution().generation()}, {"evaluations", s->evolution().evaluations()}, {"kept", kept}, {"standings", standings}, {"population", population},
                           {"retired", retired}, {"statistics", stats}, {"operators", s->evolution().operator_names()}, {"probability", s->evolution().credits().probability},
                           {"tests", tests_json(conf.done)}, {"journal", journal}, {"journal_head", conf.journal.head()}, {"refused", refused},
                           {"presence", presence_json(p, m, c)}, {"evidence", evidence_json(p, c)}, {"threshold", threshold}});
  if (r["requests"].is_object()) {
    std::vector<std::string> roles = texts(r["requests"]["roles"]);
    Json q = Json::array();
    for (const auto& x : s->requests(count_or(r["requests"], "budget", 0), roles.empty() ? ts::model_roles() : roles)) {
      std::vector<double> parents;
      for (const auto& t : x.parents) parents.push_back(static_cast<double>(t));
      std::vector<std::string> people;
      for (const auto& t : x.texts) people.push_back(to_people(t, c));
      q.push(Json::object({{"role", x.role}, {"count", x.count}, {"parents", parents}, {"texts", people}, {"notes", x.notes}, {"credit", num(x.credit)}}));
    }
    out.set("requests", q);
  }
  out.set("state", Json::object({{"schema", "noetarium.theories-search/1"}, {"evolution", es::state_json(s->evolution().state(), scenario_json)}, {"journal", journal}, {"tests", tests_json(conf.done)}}));
  return out;
}

// One theory written by a person, assessed and read under the law (never tested on what is held back: a person sees it all).
inline Json assess(const Json& r) {
  const Json& m = r["matter"];
  const Clock c(m["epoch"]);
  const ts::Problem p = problem_of(m, c);
  const lt::Bindings b = bindings_of(m);
  Law l = law_of(r, m);
  const auto rd = ts::read(p, from_people(r["text"].text(), c));
  if (!rd.ok) return Json::object({{"read", false}, {"errors", rd.errors}});
  ts::SearchOptions o = options_of(m["search"]);
  o.strategy.mu = 2, o.strategy.lambda = 1;
  ts::Search s(p, o);
  lt::Theory t;
  t.scenario = s.fit(rd.scenario), t.assessment = s.assessment(t.scenario), t.plausibility = NAN;
  t.holds = lt::holding(p, t.scenario, b), t.outcomes = outcomes(l, m, b, t.holds), t.central = s.central_story(t.scenario);
  hev::Member<ts::Scenario> member;
  member.genotype = t.scenario, member.operation = "a person";
  Json one = theory_json(s, member, t, c, "", 0, {});
  one.set("read", true);
  return one;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "theories") return theories(r);
  if (op == "assess") return assess(r);
  return science::law::answer(r);
}

}  // namespace science::theories
