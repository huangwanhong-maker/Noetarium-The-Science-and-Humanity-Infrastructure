// Society, for any work's engine (ADR 0104, ADR 0105): one composition of the sociology and social-provision domains, with demography's
// health expectancies and economics' labour flows, answering requests in JSON. What a population, a day or a network is, is given in the
// request by whoever holds the data; nothing here names a country, a survey or a file's columns.
//
//   sullivan      {"ages": [...], "l": [...], "L": [...], "bands": [...], "prevalence": [...], "unit"?: "share" | "percent", "sample"?: [...],
//                  "below"?: "none" | "first" | "half"}
//                   health expectancy by Sullivan's method from a life table (survivors, person-years by starting age) and the prevalence
//                   of a state in age bands: life expectancy, healthy and unhealthy years, their share, by age
//   life_course   {"step": n, "hh": [...], "hd": [...], "dh": [...], "dd": [...]}      years healthy and dependent from each age and state
//   care          {"ages": [...], "now": [...], "later": [...], "bands": [...], "prevalence": [...], "prevalence_later"?: [...],
//                  "unit"?: "share" | "percent", "hours": h, "years": t, "compression"?: c}
//                   people needing care now and later, their hours a week, support ratios, and the change split among size, age
//                   structure and prevalence
//   labour        {"periods": [...], "flows": [[9 numbers E→E, E→U, E→I, U→E, …]], "compare"?: [i, j]}
//                   each period's steady state (shares, the unemployment and participation rates, spells, half-life), and the change of
//                   the steady unemployment rate between two periods split among the six flows
//   unpaid        {"minutes": {activity: m}, "specialist"?: {activity: wage}, "generalist": w, "own": w}   the year's value of unpaid work
//   time_poverty  {"committed": [...], "weight"?: [...], "multiple"?: 1.5, "line"?: minutes}
//   day           {"parts": [names], "groups": [{"name": ..., "days": [[minutes by part], ...], "people"?: n}], "resolution"?: 10,
//                  "balance"?: {"plus": [part], "minus": [part]}, "contrast"?: [[i, j], ...]}
//                   each group's day as a composition (its centre closed to 1440 minutes, its mean, its spread, a balance of two kinds of
//                   time), the groups compared by Aitchison's distances and their principal coordinates, who carries each part when the
//                   groups' people are given, and the contrasts asked
//   reallocation  {"parts": [names], "days": [[minutes by part], ...], "outcome": [...], "covariates"?: [[...], ...], "minutes": m,
//                  "reference"?: [minutes by part], "resolution"?: 10}
//                   the outcome regressed on the day's log-ratio coordinates, and the isotemporal substitution table: m minutes moved
//                   to each part from each other, with the interval of each change
//   relations     {"people": n, "ties": [[a, b], ...], "directed"?: false, "groups"?: [group of each person], "permutations"?: 999,
//                  "draws"?: 500, "seed"?: s}
//                   the whole network, belonging by group with a QAP test, each person's ties, and closure against the same degrees
//
// Units: ages and years in years, minutes a day, hours a week, flows in any count (they are made probabilities by rows). A prevalence is a
// share unless the request says "unit": "percent"; the unit is stated once for the request and never guessed value by value (a severe
// limitation of 0.7% among the young is not 70%), and a share above one is refused.

#pragma once

#include <demography/health/health.hpp>
#include <demography/lifetable/lifetable.hpp>
#include <economics/labour/flows.hpp>
#include <social_provision/care/care.hpp>
#include <social_provision/households/households.hpp>
#include <sociology/relations/relations.hpp>
#include <sociology/time/day.hpp>
#include <sociology/time/reallocation.hpp>

#include "json.hpp"

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::society {

namespace dh = ::demography::health;
namespace sc = ::social_provision::care;
namespace sh = ::social_provision::households;
namespace el = ::economics::labour;
namespace st = ::sociology::time;
namespace sr = ::sociology::relations;

inline double num(const Json& j) { return j.is_number() ? j.as_number() : NAN; }
inline double number_or(const Json& j, const char* key, double fallback) { return j.has(key) && j[key].is_number() ? j[key].as_number() : fallback; }
inline std::vector<double> numbers(const Json& a) {
  std::vector<double> v;
  if (a.is_array()) for (std::size_t i = 0; i < a.size(); i++) v.push_back(num(a[i]));
  return v;
}
inline Json written(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(std::isfinite(x) ? Json(x) : Json());
  return a;
}

// A prevalence in shares, from the unit the request states.
inline std::vector<double> shares(const Json& r, std::vector<double> v, const char* what) {
  const std::string unit = r["unit"].text("share");
  if (unit != "share" && unit != "percent") throw std::invalid_argument(std::string(what) + ": a prevalence's unit is \"share\" or \"percent\"");
  for (double& p : v) {
    if (unit == "percent") p /= 100;
    if (!(p >= 0 && p <= 1)) throw std::invalid_argument(std::string(what) + ": a prevalence outside [0, 1] as a share (is its unit \"percent\"?)");
  }
  return v;
}

// A life table from survivors and person-years at the starting ages given (the last group open); e from them.
inline dh::LifeTable table_of(const std::vector<double>& ages, const std::vector<double>& l, const std::vector<double>& L) {
  const std::size_t k = ages.size();
  if (k < 2 || l.size() != k || L.size() != k) throw std::invalid_argument("sullivan: ages, survivors and person-years of one length");
  dh::LifeTable t;
  t.x = ages, t.l = l, t.L = L, t.radix = l[0];
  t.n.resize(k), t.T.assign(k, 0), t.e.assign(k, 0);
  for (std::size_t i = 0; i < k; i++) t.n[i] = i + 1 < k ? ages[i + 1] - ages[i] : INFINITY;
  double T = 0;
  for (std::size_t i = k; i-- > 0;) T += L[i], t.T[i] = T, t.e[i] = l[i] > 0 ? T / l[i] : 0;
  return t;
}

inline Json op_sullivan(const Json& r) {
  const auto t = table_of(numbers(r["ages"]), numbers(r["l"]), numbers(r["L"]));
  auto bands = numbers(r["bands"]);
  const auto prev = shares(r, numbers(r["prevalence"]), "sullivan");
  auto pi = dh::prevalence_at(t, bands, prev);
  // Before the first band, where a survey asks no one: the declared assumption ("below": "none", the default, counts no one in the state;
  // "first" carries the first band's prevalence down; "half" half of it).
  const std::string below = r["below"].text("none");
  for (std::size_t i = 0; i < t.x.size(); i++)
    if (t.x[i] < bands.front()) pi[i] = below == "first" ? prev.front() : below == "half" ? prev.front() / 2 : 0.0;
  std::vector<double> sample;
  if (r.has("sample")) sample = dh::prevalence_at(t, bands, numbers(r["sample"]));  // sample sizes, not shares
  const auto s = dh::sullivan(t, pi, sample);
  return Json::object({{"ages", written(s.x)}, {"le", written(s.le)}, {"hle", written(s.hle)}, {"dle", written(s.dle)}, {"share", written(s.share)},
                       {"se", written(s.se)}, {"prevalence", written(pi)}});
}

inline Json op_life_course(const Json& r) {
  const auto hh = numbers(r["hh"]), hd = numbers(r["hd"]), dhv = numbers(r["dh"]), dd = numbers(r["dd"]);
  std::vector<dh::Step> steps;
  for (std::size_t i = 0; i < hh.size(); i++) steps.push_back({hh[i], hd.at(i), dhv.at(i), dd.at(i)});
  const auto lc = dh::life_course(steps, number_or(r, "step", 1));
  return Json::object({{"healthy", written(lc.healthy)}, {"dependent", written(lc.dependent)}, {"healthy_from_dependent", written(lc.healthy_from_dependent)},
                       {"dependent_from_dependent", written(lc.dependent_from_dependent)}});
}

// The prevalence of each single age from bands of starting ages (before the first band: none).
inline std::vector<double> by_age(const std::vector<double>& ages, const std::vector<double>& bands, const std::vector<double>& prev) {
  std::vector<double> out(ages.size(), 0.0);
  for (std::size_t i = 0; i < ages.size(); i++) {
    if (ages[i] < bands.front()) continue;
    std::size_t k = 0;
    while (k + 1 < bands.size() && ages[i] >= bands[k + 1]) k++;
    out[i] = prev[k];
  }
  return out;
}

inline Json op_care(const Json& r) {
  const auto ages = numbers(r["ages"]), now = numbers(r["now"]), later = numbers(r["later"]), bands = numbers(r["bands"]);
  const auto p0 = by_age(ages, bands, shares(r, numbers(r["prevalence"]), "care"));
  auto p1 = r.has("prevalence_later") ? by_age(ages, bands, shares(r, numbers(r["prevalence_later"]), "care")) : p0;
  if (r.has("compression")) p1 = sc::scenario(p1, number_or(r, "compression", 0), number_or(r, "years", 0));
  const double hours = number_or(r, "hours", 0);
  const auto n0 = sc::need(now, p0, hours), n1 = sc::need(later, p1, hours);
  const auto r0 = sc::ratios(ages, now), r1 = sc::ratios(ages, later);
  const auto c = sc::decompose(now, p0, later, p1);
  return Json::object({{"now", Json::object({{"people", Json(n0.people)}, {"hours_a_week", Json(n0.hours_a_week)}, {"by_age", written(n0.by_age)},
                                             {"old_age_dependency", Json(r0.old_age_dependency)}, {"oldest_old_support", Json(r0.oldest_old_support)}})},
                       {"later", Json::object({{"people", Json(n1.people)}, {"hours_a_week", Json(n1.hours_a_week)}, {"by_age", written(n1.by_age)},
                                               {"old_age_dependency", Json(r1.old_age_dependency)}, {"oldest_old_support", Json(r1.oldest_old_support)}})},
                       {"change", Json::object({{"total", Json(c.change)}, {"size", Json(c.size)}, {"structure", Json(c.structure)}, {"prevalence", Json(c.prevalence)}})}});
}

inline Json op_labour(const Json& r) {
  const Json& flows = r["flows"];
  std::vector<el::Transitions> P;
  Json out = Json::array();
  for (std::size_t q = 0; q < flows.size(); q++) {
    const auto v = numbers(flows[q]);
    if (v.size() != 9) throw std::invalid_argument("labour: nine flows a period, E→E, E→U, E→I, U→E, U→U, U→I, I→E, I→U, I→I");
    el::Transitions c{};
    for (std::size_t i = 0; i < 3; i++)
      for (std::size_t j = 0; j < 3; j++) c[i][j] = std::isfinite(v[3 * i + j]) ? v[3 * i + j] : 0.0;
    P.push_back(el::from_counts(c));
    const auto s = el::steady(P.back());
    out.push(Json::object({{"e", Json(s.shares[0])}, {"u", Json(s.shares[1])}, {"i", Json(s.shares[2])}, {"unemployment_rate", Json(s.unemployment_rate)},
                           {"participation_rate", Json(s.participation_rate)}, {"spell_u", Json(s.spell[1])}, {"spell_e", Json(s.spell[0])},
                           {"half_life", std::isfinite(s.half_life) ? Json(s.half_life) : Json()},
                           {"p", written({P.back()[0][1], P.back()[0][2], P.back()[1][0], P.back()[1][2], P.back()[2][0], P.back()[2][1]})}}));
  }
  Json res = Json::object({{"periods", out}});
  if (r.has("compare") && r["compare"].size() == 2) {
    const auto i = static_cast<std::size_t>(num(r["compare"][0])), j = static_cast<std::size_t>(num(r["compare"][1]));
    const auto c = el::contributions(P.at(i), P.at(j));
    std::vector<double> by(c.by_flow.begin(), c.by_flow.end());
    res.set("contributions", Json::object({{"before", Json(c.before)}, {"after", Json(c.after)}, {"change", Json(c.change)}, {"by_flow", written(by)},
                                           {"flows", Json::array({Json("E→U"), Json("E→I"), Json("U→E"), Json("U→I"), Json("I→E"), Json("I→U")})}}));
  }
  return res;
}

inline std::map<std::string, double> number_map(const Json& j) {
  std::map<std::string, double> m;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object()) m[k] = num(v);
  return m;
}

inline Json op_unpaid(const Json& r) {
  const auto v = sh::value_unpaid(number_map(r["minutes"]), number_map(r["specialist"]), number_or(r, "generalist", 0), number_or(r, "own", 0));
  Json by = Json::object();
  for (const auto& [k, x] : v.by_activity) by.set(k, Json(x));
  return Json::object({{"hours_a_year", Json(v.hours_a_year)}, {"generalist", Json(v.generalist)}, {"specialist", Json(v.specialist)}, {"opportunity", Json(v.opportunity)},
                       {"by_activity", by}});
}

inline Json op_time_poverty(const Json& r) {
  const auto t = sh::time_poverty(numbers(r["committed"]), numbers(r["weight"]), number_or(r, "multiple", 1.5), number_or(r, "line", NAN));
  return Json::object({{"line", Json(t.line)}, {"median", Json(t.median)}, {"rate", Json(t.rate)}, {"gap", Json(t.gap)}});
}

// ---------------------------------------------------------------- sociology's time

inline std::vector<std::size_t> indices(const Json& a, std::size_t limit, const char* what) {
  std::vector<std::size_t> v;
  if (a.is_array())
    for (std::size_t i = 0; i < a.size(); i++) {
      const double x = num(a[i]);
      if (!(x >= 0) || x >= static_cast<double>(limit)) throw std::invalid_argument(std::string(what) + ": an index outside the parts");
      v.push_back(static_cast<std::size_t>(x));
    }
  return v;
}

inline calx::linalg::Rectangular rows_of(const Json& a, std::size_t cols, const char* what) {
  if (!a.is_array() || a.size() == 0) throw std::invalid_argument(std::string(what) + ": at least one row");
  calx::linalg::Rectangular X(a.size(), cols);
  for (std::size_t i = 0; i < a.size(); i++) {
    const auto v = numbers(a[i]);
    if (v.size() != cols) throw std::invalid_argument(std::string(what) + ": every row has one number per column");
    for (std::size_t k = 0; k < cols; k++) {
      if (!std::isfinite(v[k])) throw std::invalid_argument(std::string(what) + ": a missing number");
      X(i, k) = v[k];
    }
  }
  return X;
}

inline Json table_json(const calx::linalg::Rectangular& t) {
  Json a = Json::array();
  for (std::size_t i = 0; i < t.rows; i++) {
    std::vector<double> row(t.cols);
    for (std::size_t j = 0; j < t.cols; j++) row[j] = t(i, j);
    a.push(written(row));
  }
  return a;
}

inline Json op_day(const Json& r) {
  const Json& parts = r["parts"];
  const std::size_t D = parts.size();
  if (D < 2) throw std::invalid_argument("day: at least two parts");
  st::DayOptions o;
  o.resolution = number_or(r, "resolution", 10);
  const Json& groups = r["groups"];
  if (!groups.is_array() || groups.size() == 0) throw std::invalid_argument("day: at least one group");
  std::vector<std::size_t> plus, minus;
  if (r.has("balance")) plus = indices(r["balance"]["plus"], D, "day"), minus = indices(r["balance"]["minus"], D, "day");
  Json out = Json::array();
  calx::linalg::Rectangular centres(groups.size(), D);
  std::vector<double> people;
  for (std::size_t g = 0; g < groups.size(); g++) {
    const auto X = rows_of(groups[g]["days"], D, "day");
    const auto d = st::group_day(X, o);
    for (std::size_t k = 0; k < D; k++) centres(g, k) = d.centre[k];
    Json one = Json::object({{"name", groups[g]["name"]}, {"days", Json(static_cast<double>(d.people))}, {"centre", written(d.centre)}, {"mean", written(d.mean)}});
    if (d.people > 1) one.set("total_variance", Json(d.total_variance)), one.set("clr_variance", written(d.clr_variance));
    if (!plus.empty() && !minus.empty()) one.set("balance", Json(st::balance(d.centre, plus, minus, o)));
    out.push(std::move(one));
    people.push_back(number_or(groups[g], "people", NAN));
  }
  Json res = Json::object({{"parts", parts}, {"groups", out}});
  if (groups.size() >= 2) {
    const auto p = st::compare_places(centres, o);
    Json coords = Json::array();
    for (std::size_t i = 0; i < p.distance.n; i++) {
      std::vector<double> c;
      for (std::size_t k = 0; k < std::min<std::size_t>(2, p.ordination.coordinates.cols); k++) c.push_back(p.ordination.coordinates(i, k));
      coords.push(written(c));
    }
    Json dist = Json::array();
    for (std::size_t i = 0; i < p.distance.n; i++) {
      std::vector<double> row(p.distance.n);
      for (std::size_t j = 0; j < p.distance.n; j++) row[j] = p.distance(i, j);
      dist.push(written(row));
    }
    res.set("compared", Json::object({{"distance", dist}, {"coordinates", coords}, {"explained", written(p.ordination.explained)},
                                      {"centre", written(p.centre)}, {"total_variance", Json(p.total_variance)}, {"clr_variance", written(p.clr_variance)}}));
  }
  bool counted = groups.size() >= 2;
  for (double n : people) counted = counted && std::isfinite(n) && n > 0;
  if (counted) {
    Json carried = Json::array();
    for (std::size_t k = 0; k < D; k++) {
      std::vector<double> m(groups.size());
      for (std::size_t g = 0; g < groups.size(); g++) m[g] = centres(g, k);
      const auto c = st::who_carries(m, people);
      carried.push(Json::object({{"part", parts[k]}, {"share", written(c.share)}, {"over", written(c.over)}, {"hours_a_year", Json(c.minutes_a_year_total)}}));
    }
    res.set("carried", carried);
  }
  if (r.has("contrast")) {
    Json cs = Json::array();
    for (std::size_t q = 0; q < r["contrast"].size(); q++) {
      const auto ij = indices(r["contrast"][q], groups.size(), "day");
      if (ij.size() != 2) throw std::invalid_argument("day: a contrast is two groups");
      std::vector<double> a(D), b(D);
      for (std::size_t k = 0; k < D; k++) a[k] = centres(ij[0], k), b[k] = centres(ij[1], k);
      const auto c = st::contrast(a, b, o);
      cs.push(Json::object({{"of", Json::array({Json(static_cast<double>(ij[0])), Json(static_cast<double>(ij[1]))})}, {"minutes", written(c.minutes)},
                            {"ratio", written(c.ratio)}, {"clr", written(c.clr)}, {"distance", Json(c.distance)}}));
    }
    res.set("contrasts", cs);
  }
  return res;
}

inline Json op_reallocation(const Json& r) {
  const std::size_t D = r["parts"].size();
  if (D < 2) throw std::invalid_argument("reallocation: at least two parts");
  st::DayOptions o;
  o.resolution = number_or(r, "resolution", 10);
  const auto X = rows_of(r["days"], D, "reallocation");
  const auto y = numbers(r["outcome"]);
  if (y.size() != X.rows) throw std::invalid_argument("reallocation: one outcome per day");
  calx::linalg::Rectangular Z;
  if (r.has("covariates") && r["covariates"].size() > 0) Z = rows_of(r["covariates"], r["covariates"][0].size(), "reallocation");
  const auto m = st::fit_outcome(X, y, Z, o);
  const auto s = st::substitutions(m, number_or(r, "minutes", 30), r.has("reference") ? numbers(r["reference"]) : std::vector<double>{}, o);
  return Json::object({{"parts", r["parts"]}, {"n", Json(static_cast<double>(m.n))}, {"r_squared", Json(m.fit.r_squared)},
                       {"clr_coefficients", written(m.clr_coefficients)}, {"reference", written(s.reference)}, {"minutes", Json(s.minutes)},
                       {"change", table_json(s.change)}, {"lower", table_json(s.lower)}, {"upper", table_json(s.upper)}, {"p", table_json(s.p)}});
}

// ---------------------------------------------------------------- sociology's relations

inline Json op_relations(const Json& r) {
  const double n = number_or(r, "people", 0);
  if (!(n >= 2)) throw std::invalid_argument("relations: at least two people");
  const auto N = static_cast<std::size_t>(n);
  std::vector<sr::Tie> ties;
  for (std::size_t q = 0; q < r["ties"].size(); q++) {
    const auto e = numbers(r["ties"][q]);
    if (e.size() < 2 || !(e[0] >= 0) || !(e[1] >= 0) || e[0] >= n || e[1] >= n) throw std::invalid_argument("relations: a tie joins two of the people");
    if (e[0] == e[1]) continue;
    ties.push_back({static_cast<std::size_t>(e[0]), static_cast<std::size_t>(e[1])});
  }
  if (ties.empty()) throw std::invalid_argument("relations: at least one tie");
  const bool directed = r.has("directed") && r["directed"].is_bool() && r["directed"].as_bool();
  const auto g = tyche::networks::network(N, ties, directed);
  const auto w = sr::whole(g);
  Json triads = Json::array();
  if (directed)
    for (std::size_t k = 0; k < 16; k++)
      triads.push(Json::object({{"type", Json(std::string(tyche::networks::triad_names[k]))}, {"count", Json(static_cast<double>(w.triads[k]))}}));
  auto maybe = [](double x) { return std::isfinite(x) ? Json(x) : Json(); };
  Json res = Json::object({{"whole", Json::object({{"people", Json(static_cast<double>(w.people))}, {"ties", Json(static_cast<double>(w.ties))},
                                                   {"density", Json(w.density)}, {"mean_contacts", Json(w.mean_contacts)}, {"transitivity", Json(w.transitivity)},
                                                   {"average_clustering", Json(w.average_clustering)}, {"reciprocity", maybe(w.reciprocity)},
                                                   {"degree_assortativity", maybe(w.degree_assortativity)},
                                                   {"components", Json(static_cast<double>(w.components))}, {"largest", Json(static_cast<double>(w.largest))},
                                                   {"isolates", Json(static_cast<double>(w.isolates))}, {"triads", triads}})}});
  std::vector<std::size_t> group;
  if (r.has("groups")) {
    for (double x : numbers(r["groups"])) {
      if (!(x >= 0)) throw std::invalid_argument("relations: a group is a number 0, 1, …");
      group.push_back(static_cast<std::size_t>(x));
    }
    if (group.size() != N) throw std::invalid_argument("relations: one group per person");
    sr::BelongingOptions bo;
    bo.permutations = static_cast<std::size_t>(number_or(r, "permutations", 999));
    bo.seed = static_cast<std::uint64_t>(number_or(r, "seed", 20261113));
    const auto b = sr::belonging(g, group, bo);
    std::vector<double> members(b.members.begin(), b.members.end());
    res.set("belonging", Json::object({{"members", written(members)}, {"share", written(b.share)}, {"within", written(b.within)}, {"inbreeding", written(b.inbreeding)},
                                       {"ei", written(b.ei)}, {"ei_index", Json(b.ei_index)}, {"ei_complete", Json(b.ei_complete)},
                                       {"assortativity", Json(b.assortativity)}, {"qap_r", Json(b.qap_r)}, {"qap_p", Json(b.qap_p)},
                                       {"permutations", Json(static_cast<double>(b.permutations))}}));
  }
  const auto p = sr::people(g, group);
  std::vector<double> contacts(p.contacts.begin(), p.contacts.end()), core(p.core.begin(), p.core.end()), reach(p.reach.begin(), p.reach.end());
  res.set("people", Json::object({{"contacts", written(contacts)}, {"clustering", written(p.clustering)}, {"constraint", written(p.constraint)},
                                  {"effective_size", written(p.effective_size)}, {"core", written(core)}, {"reach", written(reach)},
                                  {"diversity", written(p.diversity)}, {"effective_groups", written(p.effective_groups)}}));
  const auto u = directed ? tyche::networks::underlying(g) : g;
  if (u.size() >= 2) {
    const auto c = sr::closure(g, static_cast<std::size_t>(number_or(r, "draws", 500)), static_cast<std::uint64_t>(number_or(r, "seed", 20261113)));
    res.set("closure", Json::object({{"transitivity", Json(c.transitivity)}, {"expected", Json(c.expected)}, {"sd", Json(c.sd)}, {"p_greater", Json(c.p_greater)},
                                     {"draws", Json(static_cast<double>(c.draws))}}));
  }
  return res;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "sullivan") return op_sullivan(r);
  if (op == "life_course") return op_life_course(r);
  if (op == "care") return op_care(r);
  if (op == "labour") return op_labour(r);
  if (op == "unpaid") return op_unpaid(r);
  if (op == "time_poverty") return op_time_poverty(r);
  if (op == "day") return op_day(r);
  if (op == "reallocation") return op_reallocation(r);
  if (op == "relations") return op_relations(r);
  if (op == "status") return Json::object({{"ok", Json(true)}});
  throw std::invalid_argument("no operation \"" + op + "\"");
}

}  // namespace science::society
