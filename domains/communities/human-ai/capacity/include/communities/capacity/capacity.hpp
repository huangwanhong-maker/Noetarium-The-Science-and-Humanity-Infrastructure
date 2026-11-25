// Capacity: measures against unequal intelligence and computation (the design record of 2026-11-25, Section 9).
//
//   - Floors in usable units. A participant's conversion factor is how much of a resource it needs for one usable unit (the Post-Labor
//     paper's factor): a party whose model is weaker, or whose tools are fewer, needs more tokens for the same checked entry. A floor
//     stated in usable units asks a different allowance of each.
//   - A solidarity pool. A budget of the resource brings those below the floor up, the least served in usable units first: the water
//     level L solves Σ c_i · max(0, min(L, floor) − u_i) = budget, u_i = holding_i / c_i.
//   - Evaluators counted by provision. Assessments by operatives that run one provider's model err together, so for agreement they
//     count once, as copies of one controller count once by origin (thesmos's criterion of provision).

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace communities::capacity {

struct Participant {
  std::string id;
  double conversion = 1;  // resource per usable unit
  double holding = 0;     // resource held
  double usable() const { return holding / conversion; }
};

// What each participant needs, in the resource, to reach a floor of usable units.
inline std::map<std::string, double> shortfalls(const std::vector<Participant>& ps, double floor) {
  std::map<std::string, double> out;
  for (const auto& p : ps) {
    if (p.conversion <= 0) throw std::invalid_argument("communities::capacity: a conversion factor is positive");
    out[p.id] = std::max(0.0, floor - p.usable()) * p.conversion;
  }
  return out;
}

// A pool of `budget` in the resource raises the least served in usable units first, up to the floor: the allocation to each.
inline std::map<std::string, double> pool(const std::vector<Participant>& ps, double floor, double budget) {
  auto cost = [&](double level) {
    double c = 0;
    for (const auto& p : ps) c += p.conversion * std::max(0.0, std::min(level, floor) - p.usable());
    return c;
  };
  double low = 0, high = floor;
  for (const auto& p : ps) low = std::min(low, p.usable());
  if (cost(floor) <= budget) low = high = floor;
  else
    for (int k = 0; k < 200; k++) {
      const double mid = (low + high) / 2;
      (cost(mid) <= budget ? low : high) = mid;
    }
  std::map<std::string, double> out;
  for (const auto& p : ps) out[p.id] = p.conversion * std::max(0.0, std::min(low, floor) - p.usable());
  return out;
}

// How many independent evaluators a set of assessors is, under control (by origin) and under provision (by provider and model).
struct Independence {
  std::size_t assessors = 0, origins = 0, providers = 0;
};
inline Independence independence(const thesmos::kernel::Parties& ps, const std::vector<std::string>& assessors) {
  std::set<std::string> origins, providers;
  for (const auto& a : assessors) {
    origins.insert(thesmos::kernel::origin(ps, a, thesmos::kernel::Criterion::control));
    providers.insert(thesmos::kernel::origin(ps, a, thesmos::kernel::Criterion::provision));
  }
  return {assessors.size(), origins.size(), providers.size()};
}

}  // namespace communities::capacity
