// A city's municipal solid waste from its generation to where it is managed, and SDG indicator 11.6.1 (ADR 0148).
//
// The account follows the seven steps of UN-Habitat's Waste Wise Cities Tool (WaCT, 2021): (1) what households generate, from a
// stratified cluster survey of households by income level, each household's waste weighed over several days; (2) what other sources
// generate; (3) what is collected and received at recovery and disposal facilities; (4) the waste's composition; (5) what recovery
// facilities recover; (6) each facility's level of control, from a checklist; (7) what is managed in controlled facilities. SDG
// 11.6.1 is the share of the municipal solid waste generated that is collected and managed in controlled facilities.
//
// Generation. Households are drawn by stratum (income level) and area (the primary sampling unit); a household of stratum h weighs
// w = H_h / n_h, H_h the stratum's households and n_h those sampled. Each household's waste per day y_i = kg_i / days_i is summed by
// peira's survey estimator, with the ultimate-cluster variance and a Student interval on PSUs − strata degrees of freedom; the
// generation per person is the ratio Σ w kg / Σ w persons · days. Other sources add declared amounts.
//
// Facilities and control. A recovery facility receives waste from collection, recovers part of it and sends the rest to a disposal site
// it names. A disposal site's amount is everything weighed at its gate, residues of recovery facilities included, so that what comes
// to it from collection is its gate's amount less the residues sent to it; residues declared beyond what a gate weighed are the
// account's imbalance, the double counting WaCT warns against. Its level of control is the highest level of
// a declared ladder whose criteria, and those of every level below it, it meets; it is controlled from a declared level upward (for
// 11.6.1, basic control; the metadata of 2025: control in operation, not in design). The waste managed in controlled facilities is
// what controlled recovery facilities recover and what controlled disposal facilities receive, residues included, so that nothing is
// counted twice. The account says by how much it fails to balance.
//
// Uncertainty. The amounts at facilities are taken as measured (weighbridges); the generation carries the survey's interval. Since
// 11.6.1 = M / (H + N) falls as H rises, its interval is the image of H's: [M / (H_upper + N), M / (H_lower + N)].
//
// Composition. Sorted samples give each fraction's share as a ratio of weighted totals, with its interval.
//
// Every ladder, threshold, amount and design is the caller's declaration; nothing here is any city's account.
#pragma once

#include <peira/surveys/surveys.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace industrial_ecology::waste {

inline constexpr double none = std::numeric_limits<double>::quiet_NaN();

struct Household {
  int stratum = 0;          // the income level
  int area = 0;             // the primary sampling unit within its stratum
  double persons = 0;
  double kg = 0;            // weighed over `days` (the first day's waste, which may be old, left out by the caller)
  double days = 0;
};

struct Survey {
  std::map<int, double> households;  // H_h, the households of each stratum in the city
  std::vector<Household> sample;
  double confidence = 0.95;
};

struct Generation {
  peira::surveys::Estimate households;   // t a day from households
  peira::surveys::Estimate per_person;   // kg a person a day
  std::map<int, double> per_person_by_stratum;
  double other = 0;                      // t a day from other sources, declared
  double total = 0, lower = 0, upper = 0;  // households and others, t a day, with the households' interval
};

inline Generation generation(const Survey& S, const std::map<std::string, double>& other_sources = {}) {
  if (S.sample.empty()) throw std::invalid_argument("waste: a sample of households");
  std::map<int, double> sampled;
  for (const auto& h : S.sample) {
    if (!(h.days > 0) || h.persons <= 0 || h.kg < 0) throw std::invalid_argument("waste: every household has persons, days weighed and a weight that is not negative");
    if (!S.households.count(h.stratum)) throw std::invalid_argument("waste: a household of a stratum whose households are not given");
    sampled[h.stratum] += 1;
  }
  peira::surveys::Design d;
  std::vector<double> y, kg, pd;
  for (const auto& h : S.sample) {
    d.weight.push_back(S.households.at(h.stratum) / sampled[h.stratum]);
    d.stratum.push_back(h.stratum);
    d.psu.push_back(h.area);
    d.fraction.push_back(0.0);
    y.push_back(h.kg / h.days / 1000.0);  // t a day
    kg.push_back(h.kg);
    pd.push_back(h.persons * h.days);
  }
  Generation G;
  G.households = peira::surveys::total(y, d, S.confidence);
  G.per_person = peira::surveys::ratio(kg, pd, d, S.confidence);
  std::map<int, std::pair<double, double>> by;
  for (const auto& h : S.sample) by[h.stratum].first += h.kg, by[h.stratum].second += h.persons * h.days;
  for (const auto& [s, v] : by) G.per_person_by_stratum[s] = v.first / v.second;
  for (const auto& [k, t] : other_sources) {
    if (t < 0) throw std::invalid_argument("waste: an amount from " + k + " that is not negative");
    G.other += t;
  }
  G.total = G.households.value + G.other;
  G.lower = std::max(0.0, G.households.ci_lower) + G.other, G.upper = G.households.ci_upper + G.other;
  return G;
}

// ---- facilities and control ------------------------------------------------------------------------------------------------------

struct Ladder {
  std::vector<std::string> levels;                 // from the least control to the most
  std::vector<std::vector<std::string>> criteria;  // the criteria of each level
  std::size_t controlled_from = 0;                 // the index of the first level that counts as controlled
};

enum class Kind { recovery, disposal };

struct Facility {
  std::string id, name;
  Kind kind = Kind::disposal;
  double received = 0;               // t a day: from collection (recovery); everything weighed at the gate (disposal)
  double recovered = 0;              // t a day recovered (a recovery facility)
  std::string residue_to;            // where a recovery facility sends the rest
  std::set<std::string> met;         // the ladder's criteria it meets
  std::map<std::string, double> recovered_by_fraction;  // optional
};

// The highest level whose criteria, and those of every level below, are all met; −1 when not even the first level's are.
inline int level_of(const Facility& f, const Ladder& L) {
  int level = -1;
  for (std::size_t k = 0; k < L.levels.size(); k++) {
    const auto& c = k < L.criteria.size() ? L.criteria[k] : std::vector<std::string>{};
    if (!std::all_of(c.begin(), c.end(), [&](const std::string& x) { return f.met.count(x) > 0; })) break;
    level = static_cast<int>(k);
  }
  return level;
}

struct FacilityResult {
  std::string id;
  int level = -1;
  std::string level_name;
  bool controlled = false;
  double received = 0, residues_in = 0, from_collection = 0, recovered = 0, residue_out = 0, managed_controlled = 0;
};

struct Account {
  Generation generated;
  double collected = 0, uncollected = 0, delivered = 0, not_delivered = 0;
  double recovered = 0, recovered_controlled = 0, disposed = 0, disposed_controlled = 0;
  double managed_controlled = 0;
  double indicator = none, indicator_lower = none, indicator_upper = none;   // SDG 11.6.1, with the generation's interval carried
  double collection_coverage = none, recovery_rate = none;
  double imbalance = 0;              // residues declared beyond what the gates they were sent to weighed
  std::vector<FacilityResult> facilities;
  std::vector<std::string> remarks;
};

inline Account account(const Generation& G, const std::vector<Facility>& fs, const Ladder& L, double not_delivered = 0) {
  if (L.levels.empty() || L.controlled_from >= L.levels.size()) throw std::invalid_argument("waste: a ladder of levels, and the level from which control counts");
  if (not_delivered < 0) throw std::invalid_argument("waste: waste collected and not delivered is not negative");
  std::map<std::string, std::size_t> at;
  for (std::size_t i = 0; i < fs.size(); i++) {
    if (fs[i].id.empty() || !at.emplace(fs[i].id, i).second) throw std::invalid_argument("waste: every facility has an id of its own (\"" + fs[i].id + "\")");
    if (fs[i].received < 0 || fs[i].recovered < 0) throw std::invalid_argument("waste: facility " + fs[i].id + ": amounts that are not negative");
    if (fs[i].kind == Kind::disposal && fs[i].recovered > 0) throw std::invalid_argument("waste: facility " + fs[i].id + ": a disposal site recovers nothing");
  }
  Account A;
  A.generated = G;
  std::vector<FacilityResult> R(fs.size());
  for (std::size_t i = 0; i < fs.size(); i++) {
    R[i].id = fs[i].id, R[i].received = fs[i].received, R[i].level = level_of(fs[i], L);
    R[i].level_name = R[i].level >= 0 ? L.levels[static_cast<std::size_t>(R[i].level)] : "below the ladder";
    R[i].controlled = R[i].level >= static_cast<int>(L.controlled_from);
  }
  for (std::size_t i = 0; i < fs.size(); i++) {
    if (fs[i].kind != Kind::recovery) continue;
    const double residue = fs[i].received - fs[i].recovered;
    if (residue < -1e-9) throw std::invalid_argument("waste: facility " + fs[i].id + " recovers more than it receives");
    R[i].recovered = fs[i].recovered, R[i].residue_out = std::max(0.0, residue), R[i].from_collection = fs[i].received;
    A.recovered += R[i].recovered;
    if (R[i].controlled) A.recovered_controlled += R[i].recovered, R[i].managed_controlled = R[i].recovered;
    if (R[i].residue_out > 0) {
      auto it = at.find(fs[i].residue_to);
      if (it == at.end()) {
        A.remarks.push_back("the residue of " + fs[i].id + " goes to no facility named; it is counted as collected and not managed");
      } else if (fs[it->second].kind != Kind::disposal) {
        throw std::invalid_argument("waste: facility " + fs[i].id + " sends its residue to " + fs[i].residue_to + ", which is not a disposal site");
      } else {
        R[it->second].residues_in += R[i].residue_out;
      }
    }
  }
  for (std::size_t i = 0; i < fs.size(); i++) {
    if (fs[i].kind == Kind::recovery) {
      A.delivered += fs[i].received;
      continue;
    }
    const double direct = fs[i].received - R[i].residues_in;
    if (direct < -1e-9) {
      A.imbalance += -direct;
      A.remarks.push_back("residues declared sent to " + fs[i].id + " exceed what its gate weighed; the excess is the account's imbalance");
    }
    R[i].from_collection = std::max(0.0, direct);
    A.delivered += R[i].from_collection;
    A.disposed += fs[i].received;
    if (R[i].controlled) A.disposed_controlled += fs[i].received, R[i].managed_controlled = fs[i].received;
  }
  A.not_delivered = not_delivered;
  A.collected = A.delivered + not_delivered;
  A.uncollected = G.total - A.collected;
  if (A.uncollected < 0) A.remarks.push_back("more is collected than the survey estimates is generated; the generation is likely underestimated");
  A.managed_controlled = A.recovered_controlled + A.disposed_controlled;
  if (G.total > 0) {
    A.indicator = A.managed_controlled / G.total;
    A.collection_coverage = A.collected / G.total;
    A.recovery_rate = A.recovered / G.total;
  }
  if (G.upper > 0) A.indicator_lower = A.managed_controlled / G.upper;
  if (G.lower > 0) A.indicator_upper = A.managed_controlled / G.lower;
  A.facilities = R;
  return A;
}

// ---- composition --------------------------------------------------------------------------------------------------------------------

struct Sorted {
  int stratum = 0, area = 0;
  std::map<std::string, double> kg;   // by fraction
};

struct Share {
  std::string fraction;
  peira::surveys::Estimate share;
};

// Each fraction's share of the sorted waste, weighted by stratum as the households are, with its interval.
inline std::vector<Share> composition(const std::vector<Sorted>& samples, const std::map<int, double>& households, double confidence = 0.95) {
  if (samples.empty()) throw std::invalid_argument("waste: sorted samples");
  std::map<int, double> n;
  std::set<std::string> fractions;
  for (const auto& s : samples) {
    if (!households.count(s.stratum)) throw std::invalid_argument("waste: a sorted sample of a stratum whose households are not given");
    n[s.stratum] += 1;
    for (const auto& [f, k] : s.kg) {
      if (k < 0) throw std::invalid_argument("waste: a fraction's weight is not negative");
      fractions.insert(f);
    }
  }
  peira::surveys::Design d;
  std::vector<double> total;
  for (const auto& s : samples) {
    d.weight.push_back(households.at(s.stratum) / n[s.stratum]), d.stratum.push_back(s.stratum), d.psu.push_back(s.area), d.fraction.push_back(0.0);
    double t = 0;
    for (const auto& [f, k] : s.kg) t += k;
    total.push_back(t);
  }
  std::vector<Share> out;
  for (const auto& f : fractions) {
    std::vector<double> y;
    for (const auto& s : samples) {
      auto it = s.kg.find(f);
      y.push_back(it == s.kg.end() ? 0.0 : it->second);
    }
    out.push_back({f, peira::surveys::ratio(y, total, d, confidence)});
  }
  return out;
}

}  // namespace industrial_ecology::waste
