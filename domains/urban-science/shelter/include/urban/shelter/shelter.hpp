// Shelter: the people living in slum or informal households and in inadequate housing, SDG 11.1.1 read from a household survey under
// declared readings of its open terms (theory/shelter.md; ADR 0155).
//
// SDG 11.1.1 (UN-Habitat's metadata, last updated 2021-12-20) counts a household as a slum or informal household when it lacks one or
// more of five things: an improved water source, improved sanitation, sufficient living area, a durable dwelling, and security of tenure;
// and as inadequately housed when its net monthly expenditure on housing exceeds 30% of its monthly income. Both shares are of people.
// Each term is open: which sources and facilities are improved (a shared toilet among them), how many people to a habitable room is
// too many, which of walls, roof and floor must be of durable material and whether a hazardous site counts, what makes tenure secure
// (documentation, protection from eviction, both), and whether tenure is counted at all, as it is often not asked. A reading fixes
// each; the shares follow, and none is the indicator's value until a reading is chosen.
//
// A household's answer may be missing. Under a reading each component is then deprived, not deprived, unknown, or not counted. The share
// is bounded by counting every unknown as not deprived and as deprived (partial identification), and the reading says which value to
// report between: either bound, or the households with an unknown status left out. Shares are ratios of weighted people, estimated by
// peira's survey estimator with the ultimate-cluster variance on the survey's strata and clusters.

#pragma once

#include <peira/surveys/surveys.hpp>

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace urban::shelter {

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("urban::shelter: " + what);
}
}  // namespace detail

constexpr double unknown = std::numeric_limits<double>::quiet_NaN();

// One household of the survey. An empty code, a negative flag or a NaN number is a missing answer.
struct Household {
  double weight = 1;          // the survey's weight, households represented
  int stratum = 0, cluster = 0;
  std::string group;          // a domain to report by (a district), or empty
  double persons = 1;
  double rooms = unknown;     // habitable rooms
  std::string water, sanitation;
  int shared_toilet = -1;     // 1 shared with other households, 0 not
  std::string walls, roof, floor;
  int hazardous_site = -1;    // 1 on or near a hazardous site, 0 not
  int documented = -1;        // 1 a document proving tenure, 0 none
  int fears_eviction = -1;    // 1 perceives a risk of eviction, 0 not
  double housing_cost = unknown, income = unknown;  // monthly, one currency
};

enum class Tenure { left_out, documented, documented_and_unthreatened, documented_or_unthreatened };
enum class Unknown { not_deprived, deprived, left_out };

struct Reading {
  std::string name;
  std::set<std::string> water_improved, sanitation_improved;
  bool shared_deprives = false;            // a shared toilet counts as not improved
  double most_per_room = 3;                // more people than this to a habitable room is crowded
  std::set<std::string> durable;           // durable materials, of any part
  std::set<std::string> parts{"walls", "roof", "floor"};  // the parts that must be durable
  bool hazard_deprives = false;            // a hazardous site makes the dwelling not durable
  Tenure tenure = Tenure::left_out;
  double afford = 0.30;                    // inadequate when housing costs more than this share of income
  Unknown unknowns = Unknown::left_out;
  std::string source;
};

constexpr std::array<const char*, 5> components{"water", "sanitation", "living area", "durability", "tenure"};
enum Status : int { no = 0, yes = 1, missing = 2, uncounted = 3 };

// A household's status under a reading, component by component (water, sanitation, living area, durability, tenure).
inline std::array<int, 5> statuses(const Household& h, const Reading& r) {
  std::array<int, 5> s{};
  s[0] = h.water.empty() ? missing : r.water_improved.count(h.water) ? no : yes;
  if (h.sanitation.empty()) s[1] = missing;
  else if (!r.sanitation_improved.count(h.sanitation)) s[1] = yes;
  else if (!r.shared_deprives) s[1] = no;
  else s[1] = h.shared_toilet < 0 ? missing : h.shared_toilet ? yes : no;
  s[2] = !(h.rooms > 0) || !(h.persons > 0) ? missing : h.persons / h.rooms > r.most_per_room ? yes : no;
  {
    bool any_missing = false, bad = false;
    for (const auto& part : r.parts) {
      const std::string& m = part == "walls" ? h.walls : part == "roof" ? h.roof : h.floor;
      if (m.empty()) any_missing = true;
      else if (!r.durable.count(m)) bad = true;
    }
    if (r.hazard_deprives) {
      if (h.hazardous_site < 0) any_missing = true;
      else if (h.hazardous_site) bad = true;
    }
    s[3] = bad ? yes : any_missing ? missing : no;
  }
  switch (r.tenure) {
    case Tenure::left_out: s[4] = uncounted; break;
    case Tenure::documented: s[4] = h.documented < 0 ? missing : h.documented ? no : yes; break;
    case Tenure::documented_and_unthreatened:
      if (h.documented == 0 || h.fears_eviction == 1) s[4] = yes;
      else s[4] = h.documented < 0 || h.fears_eviction < 0 ? missing : no;
      break;
    case Tenure::documented_or_unthreatened:
      if (h.documented == 1 || h.fears_eviction == 0) s[4] = no;
      else s[4] = h.documented < 0 || h.fears_eviction < 0 ? missing : yes;
      break;
  }
  return s;
}

// The union rule: deprived when any counted component is; unknown when none is but one is unknown.
inline int slum_status(const std::array<int, 5>& s) {
  bool m = false;
  for (const int x : s) {
    if (x == yes) return yes;
    if (x == missing) m = true;
  }
  return m ? missing : no;
}

inline int inadequate_status(const Household& h, const Reading& r) {
  if (!std::isfinite(h.housing_cost) || !(h.income > 0)) return missing;
  return h.housing_cost > r.afford * h.income ? yes : no;
}

// A share of people with its interval, and its bounds over the missing answers.
struct Share {
  peira::surveys::Estimate estimate;   // under the reading's treatment of the unknown
  double lower = 0, upper = 0;         // unknowns as not deprived, as deprived
  double unknown_people = 0;           // the weighted share of people whose status is unknown
};

struct Result {
  std::string reading;
  double people = 0, households = 0;   // weighted
  Share slum, inadequate, either;
  std::array<Share, 5> component;      // people deprived of each (tenure's all zeros when left out)
  std::array<double, 5> only{};        // people deprived of this one alone, all five known or uncounted
  std::array<double, 6> deprivations{};  // people with k = 0..5 deprivations, among households with every counted status known
  std::map<std::string, Share> slum_by_group;
};

namespace detail {
inline Share share(const std::vector<int>& status, const std::vector<double>& persons, const std::vector<char>& in, const peira::surveys::Design& d,
                   Unknown u, double confidence) {
  Share out;
  double all = 0, dep = 0, unk = 0;
  for (std::size_t i = 0; i < status.size(); i++) {
    if (!in[i]) continue;
    const double p = d.weight[i] * persons[i];
    all += p;
    if (status[i] == yes) dep += p;
    if (status[i] == missing) unk += p;
  }
  require(all > 0, "a share of people needs people");
  out.lower = dep / all, out.upper = (dep + unk) / all, out.unknown_people = unk / all;
  std::vector<double> y(status.size(), 0.0), x(status.size(), 0.0);
  for (std::size_t i = 0; i < status.size(); i++) {
    if (!in[i] || (u == Unknown::left_out && status[i] == missing)) continue;
    x[i] = persons[i];
    y[i] = persons[i] * (status[i] == yes || (u == Unknown::deprived && status[i] == missing) ? 1.0 : 0.0);
  }
  double tx = 0;
  for (std::size_t i = 0; i < x.size(); i++) tx += d.weight[i] * x[i];
  if (tx > 0) out.estimate = peira::surveys::ratio(y, x, d, confidence);
  else out.estimate.value = out.estimate.ci_lower = out.estimate.ci_upper = std::numeric_limits<double>::quiet_NaN();
  return out;
}
}  // namespace detail

inline Result assess(const std::vector<Household>& hs, const Reading& r, double confidence = 0.95) {
  detail::require(!hs.empty(), "a survey has households");
  detail::require(r.most_per_room > 0 && r.afford > 0, "a reading's people to a room and share of income are positive");
  for (const auto& p : r.parts) detail::require(p == "walls" || p == "roof" || p == "floor", "a dwelling's parts are walls, roof and floor");
  peira::surveys::Design d;
  std::vector<double> persons;
  for (const auto& h : hs) {
    detail::require(h.weight >= 0 && h.persons > 0, "every household has people and a weight that is not negative");
    d.weight.push_back(h.weight), d.stratum.push_back(h.stratum), d.psu.push_back(h.cluster), d.fraction.push_back(0.0);
    persons.push_back(h.persons);
  }
  Result out;
  out.reading = r.name;
  const std::size_t n = hs.size();
  std::vector<std::array<int, 5>> st(n);
  std::vector<int> slum(n), inad(n), either(n);
  for (std::size_t i = 0; i < n; i++) {
    st[i] = statuses(hs[i], r);
    slum[i] = slum_status(st[i]);
    inad[i] = inadequate_status(hs[i], r);
    either[i] = slum[i] == yes || inad[i] == yes ? yes : slum[i] == missing || inad[i] == missing ? missing : no;
    out.people += d.weight[i] * persons[i];
    out.households += d.weight[i];
  }
  const std::vector<char> all(n, 1);
  out.slum = detail::share(slum, persons, all, d, r.unknowns, confidence);
  out.inadequate = detail::share(inad, persons, all, d, r.unknowns, confidence);
  out.either = detail::share(either, persons, all, d, r.unknowns, confidence);
  for (std::size_t c = 0; c < 5; c++) {
    std::vector<int> sc(n);
    for (std::size_t i = 0; i < n; i++) sc[i] = st[i][c] == uncounted ? no : st[i][c];
    out.component[c] = detail::share(sc, persons, all, d, r.unknowns, confidence);
  }
  double complete = 0;
  for (std::size_t i = 0; i < n; i++) {
    int k = 0;
    bool known = true;
    for (const int x : st[i]) known = known && x != missing, k += x == yes;
    if (!known) continue;
    const double p = d.weight[i] * persons[i];
    complete += p;
    out.deprivations[k] += p;
    if (k == 1)
      for (std::size_t c = 0; c < 5; c++)
        if (st[i][c] == yes) out.only[c] += p;
  }
  if (complete > 0) {
    for (auto& v : out.deprivations) v /= complete;
    for (auto& v : out.only) v /= complete;
  }
  std::set<std::string> groups;
  for (const auto& h : hs)
    if (!h.group.empty()) groups.insert(h.group);
  for (const auto& g : groups) {
    std::vector<char> in(n);
    for (std::size_t i = 0; i < n; i++) in[i] = hs[i].group == g;
    out.slum_by_group[g] = detail::share(slum, persons, in, d, r.unknowns, confidence);
  }
  return out;
}

}  // namespace urban::shelter
