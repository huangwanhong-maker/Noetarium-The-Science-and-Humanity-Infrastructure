// What a city's waste emits where it is treated, by the IPCC's methods, and the waste account carried into a community inventory's
// waste sector (ADR 0153; theory note `theory/municipal-solid-waste.md` §5).
//
// DISPOSAL: FIRST-ORDER DECAY (2006 IPCC Guidelines, Vol. 5, Ch. 3, unchanged in the 2019 Refinement). Waste deposited at a site in
// year T, W_T tonnes wet, of composition WF_x, carries decomposable degradable organic carbon DDOCm_x = W_T WF_x DOC_x DOCf_x MCF (Eq.
// 3.2, MCF for the site's type, Table 3.1). Each component decays at its own rate k_x (Table 3.3, by climate zone). With the reaction
// starting in month M (the default delay of six months, M = 13, Annex 3A.1, p. 3.35):
//   DDOCm rem_T = DDOCmd_T e^{−k(13−M)/12},   DDOCm dec_T = DDOCmd_T (1 − e^{−k(13−M)/12})        (Eqs 3A1.12–13)
//   DDOCma_T = DDOCm rem_T + DDOCma_{T−1} e^{−k},   DDOCm decomp_T = DDOCm dec_T + DDOCma_{T−1} (1 − e^{−k})  (Eqs 3A1.14–15)
//   CH4 generated_T = DDOCm decomp_T · F · 16/12   (Eq. 3.6),   CH4 emitted_T = (Σ_x CH4 generated_{x,T} − R_T)(1 − OX)   (Eq. 3.1)
// R is metered recovery, subtracted before oxidation; OX 0.1 for a managed site covered with oxidising material, otherwise 0 (Table
// 3.2). At M = 13 nothing decays in the year of deposition, and Eqs 3A1.14–15 are Eqs 3.4–3.5.
//
// BIOLOGICAL TREATMENT (Ch. 4, not refined in 2019). CH4 = M · EF − R and N2O = M · EF (Eqs 4.1–4.2), M wet tonnes treated, EF on a wet
// basis (Table 4.1): composting 4 g CH4 and 0.24 g N2O per kg; anaerobic digestion 0.8 g CH4 (recovery already counted), N2O negligible.
//
// OPEN BURNING (Ch. 5). Fossil CO2 = MSW_B Σ_j WF_j dm_j CF_j FCF_j OF · 44/12 (Eq. 5.2, Table 2.4's dry matter, carbon and fossil
// fractions); the biogenic part, with 1 − FCF, is reported apart. OF 0.58 in 2006 and 0.71 in the 2019 Refinement (Table 5.2). CH4 6,500
// g per tonne wet (p. 5.20) and N2O 150 g per tonne of dry matter (Table 5.6). No default fraction of waste burnt exists: it is declared.
//
// THE INVENTORY (GPC v1.1, Ch. 8). Waste the city generates is reported wherever it is treated: treated inside, scope 1 (III.x.1); treated
// outside, scope 3 (III.x.2); waste brought in and treated inside, scope 1 but outside BASIC (III.x.3). Methane from waste is entered as
// non-fossil where the potentials distinguish it (AR6), a reading said in the note. The GPC's own equations depart from the IPCC's in three
// places (DOCf 0.6 for 0.5; decay from the year of disposal; a factor of 10⁻³ for 10⁻⁶ in Eq. 8.5's N2O); the IPCC's are followed.
//
// Units: tonnes wet a year in, tonnes of each gas a year out (kg in the inventory's entries).

#pragma once

#include <industrial_ecology/emissions/emissions.hpp>

#include <array>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::waste {

namespace detail_treatment {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("industrial_ecology::waste: " + what);
}
}  // namespace detail_treatment

enum class Climate { temperate_dry, temperate_wet, tropical_dry, tropical_wet };

inline Climate climate_of(const std::string& s) {
  if (s == "temperate dry") return Climate::temperate_dry;
  if (s == "temperate wet") return Climate::temperate_wet;
  if (s == "tropical dry") return Climate::tropical_dry;
  if (s == "tropical wet" || s == "tropical moist") return Climate::tropical_wet;
  throw std::invalid_argument("industrial_ecology::waste: a climate zone is temperate dry, temperate wet, tropical dry or tropical wet, not " + s);
}

// A component of waste (2006, Vol. 5, Ch. 2, Table 2.4): fractions, on the bases the table gives.
struct Component {
  double dry_matter = 0;   // of wet weight
  double doc = 0;          // degradable organic carbon, of wet weight
  double carbon = 0;       // total carbon, of dry matter
  double fossil = 0;       // fossil carbon, of total carbon
  double docf = 0.5;       // the fraction of DOC that decomposes
  std::string decay;       // its group in Table 3.3: paper, wood, garden, food, or none
};

struct Defaults {
  std::string edition;
  std::map<std::string, Component> components;
  std::map<std::string, double> mcf;          // by type of site
  double F = 0.5;                             // CH4 in landfill gas
  double M = 13;                              // the month the reaction starts (delay + 7)
  double open_burning_oxidation = 0.58;
  double open_ch4_per_t_wet = 6.5e-3;         // t CH4 per t wet burnt
  double open_n2o_per_t_dry = 1.5e-4;         // t N2O per t dry matter burnt
  double compost_ch4 = 4e-3, compost_n2o = 2.4e-4;  // t per t wet treated
  double digestion_ch4 = 8e-4, digestion_n2o = 0;
  std::string source;
};

// k a year by decay group and climate (2006, Vol. 5, Ch. 3, Table 3.3; unchanged in 2019).
inline double decay_rate(const std::string& group, Climate c) {
  static const std::map<std::string, std::array<double, 4>> k = {
      {"paper", {0.04, 0.06, 0.045, 0.07}}, {"wood", {0.02, 0.03, 0.025, 0.035}}, {"garden", {0.05, 0.1, 0.065, 0.17}}, {"food", {0.06, 0.185, 0.085, 0.4}}, {"bulk", {0.05, 0.09, 0.065, 0.17}}};
  const auto it = k.find(group);
  detail_treatment::require(it != k.end(), "no decay rate for the group " + group);
  return it->second[static_cast<std::size_t>(c)];
}

// The 2006 Guidelines' defaults. Nappies decay with paper and textiles and rubber and leather not at all (Table 2.4 puts their DOC in
// parentheses, as natural rubbers would likely not degrade anaerobically); how the IPCC Waste Model assigns them is not known here, and
// these are declarations.
inline Defaults ipcc2006() {
  Defaults d;
  d.edition = "IPCC 2006";
  d.source = "2006 IPCC Guidelines for National Greenhouse Gas Inventories, Vol. 5: Table 2.4, Eqs 3.1–3.6 and 3A1.12–15, Tables 3.1–3.3, Table 4.1, Eq. 5.2, Tables 5.2 and 5.6";
  d.components = {{"food", {0.40, 0.15, 0.38, 0.0, 0.5, "food"}},        {"garden", {0.40, 0.20, 0.49, 0.0, 0.5, "garden"}},
                  {"paper", {0.90, 0.40, 0.46, 0.01, 0.5, "paper"}},     {"wood", {0.85, 0.43, 0.50, 0.0, 0.5, "wood"}},
                  {"textiles", {0.80, 0.24, 0.50, 0.20, 0.5, "paper"}},  {"nappies", {0.40, 0.24, 0.70, 0.10, 0.5, "paper"}},
                  {"rubber and leather", {0.84, 0.0, 0.67, 0.20, 0.5, "none"}}, {"plastics", {1.00, 0.0, 0.75, 1.00, 0.5, "none"}},
                  {"metal", {1.00, 0.0, 0.0, 0.0, 0.5, "none"}},         {"glass", {1.00, 0.0, 0.0, 0.0, 0.5, "none"}},
                  {"other", {0.90, 0.0, 0.03, 1.00, 0.5, "none"}}};
  d.mcf = {{"managed anaerobic", 1.0}, {"managed semi-aerobic", 0.5}, {"unmanaged deep", 0.8}, {"unmanaged shallow", 0.4}, {"uncategorised", 0.6}};
  return d;
}

// The 2019 Refinement: DOCf by how readily a component decomposes (Table 3.0: wood 0.1; paper, textiles and nappies 0.5; food and
// grasses 0.7, garden and park waste read as grasses here), eight classes of site (Table 3.1 updated), and open burning's oxidation 0.71
// (Table 5.2 updated). The rest as in 2006.
inline Defaults ipcc2019() {
  Defaults d = ipcc2006();
  d.edition = "IPCC 2019 Refinement";
  d.source = "2019 Refinement to the 2006 IPCC Guidelines, Vol. 5: Tables 3.0 and 3.1 (updated), Table 5.2 (updated); otherwise the 2006 Guidelines";
  d.components["wood"].docf = 0.1;
  d.components["food"].docf = 0.7;
  d.components["garden"].docf = 0.7;
  d.mcf = {{"managed anaerobic", 1.0},       {"managed well, semi-aerobic", 0.5}, {"managed poorly, semi-aerobic", 0.7}, {"managed well, active aeration", 0.4},
           {"managed poorly, active aeration", 0.7}, {"unmanaged deep", 0.8},      {"unmanaged shallow", 0.4},            {"uncategorised", 0.6}};
  d.open_burning_oxidation = 0.71;
  return d;
}

inline void check_composition(const std::map<std::string, double>& wf, const Defaults& d) {
  double sum = 0;
  for (const auto& [name, share] : wf) {
    detail_treatment::require(d.components.count(name), "no component " + name + " in " + d.edition);
    detail_treatment::require(share >= 0, "a share of a component is at least zero");
    sum += share;
  }
  detail_treatment::require(std::fabs(sum - 1) < 1e-6, "a composition's shares sum to one");
}

// ---- disposal ----

struct Site {
  std::string id;
  std::string type = "uncategorised";    // its class for the MCF
  bool oxidising_cover = false;          // managed and covered with soil or compost: OX 0.1
  std::map<std::string, double> composition;  // wet shares, summing to one
  std::map<int, double> deposited;       // t wet deposited in each year
  std::map<int, double> recovered;       // t CH4 recovered by metering in a year
};

struct SiteYear {
  int year = 0;
  double generated = 0, recovered = 0, emitted = 0;   // t CH4
  std::map<std::string, double> by_component;          // t CH4 generated
};

inline std::vector<SiteYear> first_order_decay(const Site& s, const Defaults& d, Climate c, int to) {
  check_composition(s.composition, d);
  const auto it = d.mcf.find(s.type);
  detail_treatment::require(it != d.mcf.end(), "no MCF for a site of type " + s.type + " in " + d.edition);
  detail_treatment::require(!s.deposited.empty(), "a site has deposits (" + s.id + ")");
  const double mcf = it->second, ox = s.oxidising_cover ? 0.1 : 0.0;
  const int from = s.deposited.begin()->first;
  detail_treatment::require(to >= from, "the years read begin with the site's first deposit (" + s.id + ")");
  std::map<std::string, double> accumulated;
  std::vector<SiteYear> out;
  for (int y = from; y <= to; y++) {
    SiteYear r;
    r.year = y;
    const auto dep = s.deposited.find(y);
    const double W = dep == s.deposited.end() ? 0.0 : dep->second;
    detail_treatment::require(W >= 0, "a deposit is at least zero");
    for (const auto& [name, share] : s.composition) {
      const auto& comp = d.components.at(name);
      if (comp.doc <= 0 || comp.decay == "none") continue;
      const double k = decay_rate(comp.decay, c);
      const double ddocmd = W * share * comp.doc * comp.docf * mcf;
      const double e1 = std::exp(-k * (13 - d.M) / 12);
      double& A = accumulated[name];
      const double decomposed = ddocmd * (1 - e1) + A * (1 - std::exp(-k));
      A = ddocmd * e1 + A * std::exp(-k);
      const double ch4 = decomposed * d.F * 16.0 / 12.0;
      r.by_component[name] = ch4;
      r.generated += ch4;
    }
    const auto rec = s.recovered.find(y);
    r.recovered = rec == s.recovered.end() ? 0.0 : rec->second;
    detail_treatment::require(r.recovered >= 0 && r.recovered <= r.generated + 1e-9, "recovery at a site is at least zero and no more than it generates (" + s.id + ", " + std::to_string(y) + ")");
    r.emitted = (r.generated - r.recovered) * (1 - ox);
    out.push_back(r);
  }
  return out;
}

// ---- biological treatment and open burning ----

struct Gases {
  double ch4 = 0, n2o = 0, co2_fossil = 0, co2_biogenic = 0;  // t
};

inline Gases composting(double tonnes_wet, const Defaults& d, double recovered = 0) {
  detail_treatment::require(tonnes_wet >= 0 && recovered >= 0, "composting treats at least nothing");
  return {std::max(0.0, tonnes_wet * d.compost_ch4 - recovered), tonnes_wet * d.compost_n2o, 0, 0};
}
inline Gases digestion(double tonnes_wet, const Defaults& d) {
  detail_treatment::require(tonnes_wet >= 0, "digestion treats at least nothing");
  return {tonnes_wet * d.digestion_ch4, tonnes_wet * d.digestion_n2o, 0, 0};
}
inline Gases open_burning(double tonnes_wet, const std::map<std::string, double>& wf, const Defaults& d) {
  detail_treatment::require(tonnes_wet >= 0, "burning burns at least nothing");
  check_composition(wf, d);
  Gases g;
  double dry = 0;
  for (const auto& [name, share] : wf) {
    const auto& c = d.components.at(name);
    const double carbon = share * c.dry_matter * c.carbon * d.open_burning_oxidation * 44.0 / 12.0;
    g.co2_fossil += tonnes_wet * carbon * c.fossil;
    g.co2_biogenic += tonnes_wet * carbon * (1 - c.fossil);
    dry += share * c.dry_matter;
  }
  g.ch4 = tonnes_wet * d.open_ch4_per_t_wet;
  g.n2o = tonnes_wet * dry * d.open_n2o_per_t_dry;
  return g;
}

// ---- into the inventory (GPC v1.1, Ch. 8) ----

enum class Origin { city_inside, city_outside, imported };  // generated in the city and treated inside or outside; brought in

struct Treated {
  std::string id;
  std::string what;    // "disposal", "composting", "digestion", "open burning"
  Origin origin = Origin::city_inside;
  Gases gases;         // t a year
};

// The inventory's waste entries for what each treatment emits: GPC III.1 disposal, III.2 biological, III.3 burning; .1 the city's waste
// inside (scope 1), .2 outside (scope 3), .3 brought in (scope 1, outside BASIC). Methane as non-fossil where the potentials name it.
inline std::vector<emissions::Entry> inventory_entries(const std::vector<Treated>& ts, const emissions::Potentials& p) {
  const std::string ch4 = p.gwp.count("CH4 non-fossil") ? "CH4 non-fossil" : "CH4";
  detail_treatment::require(p.gwp.count(ch4) && p.gwp.count("N2O") && p.gwp.count("CO2"), "the potentials name CH4, N2O and CO2");
  std::vector<emissions::Entry> out;
  for (const auto& t : ts) {
    const std::string group = t.what == "disposal" ? "III.1" : (t.what == "composting" || t.what == "digestion") ? "III.2" : t.what == "open burning" ? "III.3" : "";
    detail_treatment::require(!group.empty(), "a treatment is disposal, composting, digestion or open burning, not " + t.what);
    const std::string code = group + (t.origin == Origin::city_inside ? ".1" : t.origin == Origin::city_outside ? ".2" : ".3");
    const int scope = t.origin == Origin::city_outside ? 3 : 1;
    const auto add = [&](const std::string& gas, double tonnes, bool biogenic) {
      if (!(tonnes > 0)) return;
      emissions::Entry e;
      e.id = code + " " + t.id + " " + gas + (biogenic ? " biogenic" : "");
      e.sector = emissions::Sector::waste;
      e.scope = scope;
      e.subsector = t.what;
      e.gas = gas;
      e.amount = tonnes * 1000;  // kg
      e.imported_waste = t.origin == Origin::imported;
      e.biogenic = biogenic;
      out.push_back(e);
    };
    add(ch4, t.gases.ch4, false);
    add("N2O", t.gases.n2o, false);
    add("CO2", t.gases.co2_fossil, false);
    add("CO2", t.gases.co2_biogenic, true);
  }
  return out;
}

}  // namespace industrial_ecology::waste
