// A city's municipal solid waste and SDG 11.6.1 as one composition any work may call (ADR 0148): the industrial-ecology domain's
// `waste`, read and answered as JSON. Every ladder, amount and design is the requester's declaration.
//
//   waste.account  {survey: {households: {stratum: H}, sample: [{stratum, area, persons, kg, days}], confidence}, other: {source: t a day},
//                   facilities: [{id, name, kind: recovery | disposal, received, recovered, residue_to, met: [criterion]}],
//                   ladder: {levels: [name], criteria: [[criterion]], controlled_from}, not_delivered, sorted: [{stratum, area, kg: {fraction: kg}}]}
//                  the generation with its interval, per person by stratum, each facility's level of control and what it manages, the
//                  account from generated to managed in controlled facilities, SDG 11.6.1 with its interval, coverage, recovery, the
//                  imbalance and the remarks, and the composition when samples are sorted
//   waste.emissions {edition: "2006" | "2019", climate, year, potentials: "AR6" | "AR5", sites: [{id, type, oxidising_cover, composition: {component:
//                   share}, deposited: {year: t}, recovered: {year: t CH4}, origin}], composting | digestion: [{id, tonnes, origin}], burning: [{id,
//                   tonnes, composition, origin}]}
//                  what each treatment emits by the IPCC's methods (first-order decay at disposal sites, year by year; biological
//                  treatment; open burning), and the inventory's waste entries by GPC's codes with their totals, BASIC and territorial
//                  (ADR 0153); origin is "inside", "outside" or "imported"
#pragma once

#include <industrial_ecology/waste/treatment.hpp>
#include <industrial_ecology/waste/waste.hpp>

#include <science/json.hpp>

#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::waste {

namespace iw = industrial_ecology::waste;

inline const Json::Array& items(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}
inline double num(const Json& j, double fallback = iw::none) { return j.is_number() ? j.as_number() : fallback; }
inline Json maybe(double x) { return std::isfinite(x) ? Json(x) : Json(); }
inline Json estimate(const peira::surveys::Estimate& e) {
  return Json::object({{"value", e.value}, {"se", e.se}, {"df", e.df}, {"lower", e.ci_lower}, {"upper", e.ci_upper}});
}

inline std::map<int, double> strata_of(const Json& j) {
  std::map<int, double> out;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object()) out[std::stoi(k)] = v.as_number();
  return out;
}

inline Json account_op(const Json& q) {
  const Json& sj = q["survey"];
  iw::Survey S;
  S.households = strata_of(sj["households"]);
  S.confidence = num(sj["confidence"], 0.95);
  for (const auto& h : items(sj["sample"]))
    S.sample.push_back({static_cast<int>(num(h["stratum"], 0)), static_cast<int>(num(h["area"], 0)), num(h["persons"], 0), num(h["kg"], 0), num(h["days"], 0)});
  std::map<std::string, double> other;
  if (q["other"].is_object())
    for (const auto& [k, v] : q["other"].as_object()) other[k] = v.as_number();
  const auto G = iw::generation(S, other);
  iw::Ladder L;
  for (const auto& l : items(q["ladder"]["levels"])) L.levels.push_back(l.text());
  for (const auto& c : items(q["ladder"]["criteria"])) {
    std::vector<std::string> cs;
    for (const auto& x : items(c)) cs.push_back(x.text());
    L.criteria.push_back(cs);
  }
  L.controlled_from = static_cast<std::size_t>(num(q["ladder"]["controlled_from"], 0));
  std::vector<iw::Facility> fs;
  for (const auto& f : items(q["facilities"])) {
    iw::Facility x;
    x.id = f["id"].text(), x.name = f["name"].text(x.id);
    const std::string kind = f["kind"].text("disposal");
    if (kind != "recovery" && kind != "disposal") throw std::invalid_argument("waste: facility " + x.id + " is a recovery or a disposal facility");
    x.kind = kind == "recovery" ? iw::Kind::recovery : iw::Kind::disposal;
    x.received = num(f["received"], 0), x.recovered = num(f["recovered"], 0), x.residue_to = f["residue_to"].text();
    for (const auto& m : items(f["met"])) x.met.insert(m.text());
    fs.push_back(x);
  }
  const auto A = iw::account(G, fs, L, num(q["not_delivered"], 0));
  Json facilities = Json::array();
  for (const auto& r : A.facilities)
    facilities.push(Json::object({{"id", r.id}, {"level", static_cast<double>(r.level)}, {"level_name", r.level_name}, {"controlled", r.controlled}, {"received", r.received},
                                  {"residues_in", r.residues_in}, {"from_collection", r.from_collection}, {"recovered", r.recovered}, {"residue_out", r.residue_out},
                                  {"managed_controlled", r.managed_controlled}}));
  Json by = Json::object();
  for (const auto& [s, v] : G.per_person_by_stratum) by.set(std::to_string(s), v);
  Json remarks = Json::array();
  for (const auto& r : A.remarks) remarks.push(r);
  Json out = Json::object({{"households", estimate(G.households)}, {"per_person", estimate(G.per_person)}, {"per_person_by_stratum", by}, {"other", G.other},
                           {"generated", G.total}, {"generated_lower", G.lower}, {"generated_upper", G.upper}, {"collected", A.collected}, {"uncollected", A.uncollected},
                           {"delivered", A.delivered}, {"not_delivered", A.not_delivered}, {"recovered", A.recovered}, {"recovered_controlled", A.recovered_controlled},
                           {"disposed", A.disposed}, {"disposed_controlled", A.disposed_controlled}, {"managed_controlled", A.managed_controlled},
                           {"indicator", maybe(A.indicator)}, {"indicator_lower", maybe(A.indicator_lower)}, {"indicator_upper", maybe(A.indicator_upper)},
                           {"collection_coverage", maybe(A.collection_coverage)}, {"recovery_rate", maybe(A.recovery_rate)}, {"imbalance", A.imbalance},
                           {"facilities", facilities}, {"remarks", remarks}});
  if (!items(q["sorted"]).empty()) {
    std::vector<iw::Sorted> sorted;
    for (const auto& x : items(q["sorted"])) {
      iw::Sorted s{static_cast<int>(num(x["stratum"], 0)), static_cast<int>(num(x["area"], 0)), {}};
      if (x["kg"].is_object())
        for (const auto& [f, k] : x["kg"].as_object()) s.kg[f] = k.as_number();
      sorted.push_back(s);
    }
    Json shares = Json::array();
    for (const auto& c : iw::composition(sorted, S.households, S.confidence)) {
      Json e = estimate(c.share);
      e.set("fraction", c.fraction);
      shares.push(e);
    }
    out.set("composition", shares);
  }
  return out;
}

inline iw::Origin origin_of(const Json& j) {
  const std::string o = j.text("inside");
  if (o == "inside") return iw::Origin::city_inside;
  if (o == "outside") return iw::Origin::city_outside;
  if (o == "imported") return iw::Origin::imported;
  throw std::invalid_argument("waste: an origin is inside, outside or imported, not " + o);
}

inline std::map<std::string, double> shares_of(const Json& j) {
  std::map<std::string, double> m;
  if (j.is_object())
    for (const auto& [k, v] : j.as_object()) m[k] = v.as_number();
  return m;
}

inline Json gases_json(const iw::Gases& g) {
  return Json::object({{"ch4", g.ch4}, {"n2o", g.n2o}, {"co2_fossil", g.co2_fossil}, {"co2_biogenic", g.co2_biogenic}});
}

inline Json emissions_op(const Json& q) {
  const iw::Defaults d = q["edition"].text("2006") == "2019" ? iw::ipcc2019() : iw::ipcc2006();
  const iw::Climate climate = iw::climate_of(q["climate"].text("tropical wet"));
  const int year = static_cast<int>(num(q["year"], 0));
  if (year <= 0) throw std::invalid_argument("waste: the inventory's year");
  industrial_ecology::emissions::Potentials P = q["potentials"].text("AR6") == "AR5" ? industrial_ecology::emissions::ar5() : industrial_ecology::emissions::ar6();
  std::vector<iw::Treated> treated;
  Json sites = Json::array();
  for (const auto& s : items(q["sites"])) {
    iw::Site st;
    st.id = s["id"].text("site");
    st.type = s["type"].text("uncategorised");
    st.oxidising_cover = s["oxidising_cover"].is_bool() && s["oxidising_cover"].as_bool();
    st.composition = shares_of(s["composition"]);
    if (s["deposited"].is_object())
      for (const auto& [y, t] : s["deposited"].as_object()) st.deposited[std::stoi(y)] = t.as_number();
    if (s["recovered"].is_object())
      for (const auto& [y, t] : s["recovered"].as_object()) st.recovered[std::stoi(y)] = t.as_number();
    const auto series = iw::first_order_decay(st, d, climate, year);
    Json rows = Json::array();
    for (const auto& r : series) rows.push(Json::object({{"year", static_cast<double>(r.year)}, {"generated", r.generated}, {"recovered", r.recovered}, {"emitted", r.emitted}}));
    const auto& last = series.back();
    Json by = Json::object();
    for (const auto& [c, v] : last.by_component) by.set(c, v);
    sites.push(Json::object({{"id", st.id}, {"type", st.type}, {"mcf", d.mcf.at(st.type)}, {"series", rows}, {"emitted", last.emitted}, {"by_component", by}}));
    treated.push_back({st.id, "disposal", origin_of(s["origin"]), {last.emitted, 0, 0, 0}});
  }
  Json others = Json::array();
  const auto biological = [&](const char* key, const char* what) {
    for (const auto& b : items(q[key])) {
      const double t = num(b["tonnes"], 0);
      const auto g = std::string(what) == "composting" ? iw::composting(t, d, num(b["recovered"], 0)) : iw::digestion(t, d);
      treated.push_back({b["id"].text(what), what, origin_of(b["origin"]), g});
      others.push(Json::object({{"id", b["id"].text(what)}, {"what", std::string(what)}, {"tonnes", t}, {"gases", gases_json(g)}}));
    }
  };
  biological("composting", "composting");
  biological("digestion", "digestion");
  for (const auto& b : items(q["burning"])) {
    const double t = num(b["tonnes"], 0);
    const auto g = iw::open_burning(t, shares_of(b["composition"]), d);
    treated.push_back({b["id"].text("burning"), "open burning", origin_of(b["origin"]), g});
    others.push(Json::object({{"id", b["id"].text("burning")}, {"what", std::string("open burning")}, {"tonnes", t}, {"gases", gases_json(g)}}));
  }
  industrial_ecology::emissions::Inventory I;
  I.name = "waste";
  I.year = year;
  I.potentials = P;
  I.entries = iw::inventory_entries(treated, P);
  const auto T = industrial_ecology::emissions::totals(I);
  Json entries = Json::array();
  for (const auto& e : I.entries)
    entries.push(Json::object({{"id", e.id}, {"scope", static_cast<double>(e.scope)}, {"subsector", e.subsector}, {"gas", e.gas}, {"kg", e.amount},
                               {"t_co2e", industrial_ecology::emissions::tonnes(I, e)}, {"imported_waste", e.imported_waste}, {"biogenic", e.biogenic}}));
  return Json::object({{"edition", d.edition}, {"source", d.source}, {"potentials", P.name}, {"sites", sites}, {"treatments", others}, {"entries", entries},
                       {"basic", T.basic}, {"territorial", T.territorial}, {"biogenic", T.biogenic}});
}

inline bool handles(const std::string& op) { return op.rfind("waste.", 0) == 0; }

inline Json answer(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "waste.account") return account_op(q);
  if (op == "waste.emissions") return emissions_op(q);
  throw std::invalid_argument("waste: no operation \"" + op + "\" (waste.account)");
}

}  // namespace science::waste
