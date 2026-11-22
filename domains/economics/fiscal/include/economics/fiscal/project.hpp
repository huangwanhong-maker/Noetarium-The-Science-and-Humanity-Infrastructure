// A public project's financing plan: what it costs year by year, the capital receipts that meet it (land value captured, grants), a
// reserve that carries what is left, the loans that make up the rest, and whether the government's coverage of its debt service
// stays above a declared floor; the largest scale of the project it can afford, with and without the receipts (ADR 0152).
//
// The plan. In each year t of the government's projection (fiscal.hpp's `project`): the funds at hand are the year's receipts, its
// grants and the reserve carried from the year before at a declared rate. They pay the year's cost first; what they cannot pay is
// borrowed as a new annuity drawn that year, repaid over n years from the next at rate r. Receipts may be pledged: what is left after
// the cost then pays the new loans' service due that year, before the reserve keeps the rest. The operating budget carries the rest of
// the debt service, and the coverage of year t is OS_t / (DS_t + DS'_t − pledged_t), DS_t the existing loans' service and DS'_t the
// new loans'. Capital receipts never enter the operating surplus OS_t: they pay for capital, as public accounts keep them apart.
//
// Proposition 1 (monotone in the scale). Scale every year's cost by s ≥ 0, receipts and grants held. Then the borrowing of every year,
// the new loans' service and the net debt service the budget carries do not fall as s rises, and the reserve does not rise; so the least
// coverage over the projection does not rise. Proof by induction over the years (theory note §2): if the reserve carried into year t
// does not rise with s, the funds at hand do not; the cost paid from them is min(s c_t, F_t), so the borrowing s c_t − min(s c_t, F_t)
// = max(0, s c_t − F_t) does not fall; the leftover max(0, F_t − s c_t) does not rise; the new service, a sum of payments on loans that
// do not fall, does not fall; the pledged amount min(leftover, DS'_t) leaves a net service DS_t + max(0, DS'_t − leftover) that does not
// fall; and the reserve max(0, leftover − DS'_t) does not rise.
//
// The largest scale. By Proposition 1 the set of scales whose least coverage stays at or above the floor is an interval [0, s*], and
// s* is found by bisection to a declared tolerance (none when even s = 0 fails the floor, unbounded when the floor never binds within
// a declared largest scale).
//
// Pure computation on declared numbers; every rate, floor and receipt is the caller's.
#pragma once

#include <economics/fiscal/fiscal.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace economics::fiscal {

struct Flow {
  int year = 0;
  double amount = 0;
  std::string source;  // what it is: "levy", "tax increment", "a grant", …
};

struct Plan {
  std::vector<Flow> cost;      // the project's capital cost by year
  std::vector<Flow> receipts;  // capital receipts that may pay for it (land value captured, sales of reserve land)
  std::vector<Flow> grants;    // grants for it
  double rate = 0.05;          // the new loans' rate
  int tenor = 20;              // and their years of repayment, from the year after each is drawn
  double reserve_rate = 0;     // what the reserve earns a year
  bool pledge = false;         // receipts left after the cost pay the new loans' service before the reserve keeps them
};

struct PlanYear {
  int year = 0;
  double cost = 0, receipts = 0, grants = 0, from_reserve = 0, borrowed = 0;
  double existing_service = 0, new_service = 0, pledged = 0, reserve = 0;
  double surplus = 0, coverage = none, outstanding_new = 0;
};

struct PlanResult {
  std::vector<PlanYear> years;
  double borrowed = 0, received = 0, granted = 0, cost = 0;
  double least_coverage = none;
  int least_year = 0;
  double reserve_left = 0;           // at the projection's end
  double new_debt_left = 0;          // the new loans still owed at the projection's end
};

namespace detail_plan {
inline double at(const std::vector<Flow>& fs, int year, double scale = 1) {
  double s = 0;
  for (const auto& f : fs) s += f.year == year ? f.amount : 0.0;
  return s * scale;
}
inline void check(const Plan& p, const Projection& proj) {
  if (proj.years.empty()) throw std::invalid_argument("economics::fiscal: a plan needs the years ahead");
  if (p.tenor < 1 || p.rate < 0 || p.reserve_rate < 0) throw std::invalid_argument("economics::fiscal: a plan's loans have a tenor of a year or more and rates at least zero");
  for (const auto* fs : {&p.cost, &p.receipts, &p.grants})
    for (const auto& f : *fs) {
      if (!(f.amount >= 0) || !std::isfinite(f.amount)) throw std::invalid_argument("economics::fiscal: a plan's costs, receipts and grants are finite and at least zero");
      if (f.year < proj.years.front() || f.year > proj.years.back()) throw std::invalid_argument("economics::fiscal: a plan's flow of " + std::to_string(f.year) + " lies outside the years ahead (" + f.source + ")");
    }
}
}  // namespace detail_plan

// The plan year by year, every cost scaled by `scale`.
inline PlanResult plan(const Projection& proj, const Plan& p, double scale = 1) {
  detail_plan::check(p, proj);
  if (!(scale >= 0)) throw std::invalid_argument("economics::fiscal: a project's scale is at least zero");
  PlanResult R;
  std::vector<Loan> drawn;
  double reserve = 0;
  R.least_coverage = std::numeric_limits<double>::infinity();
  for (std::size_t k = 0; k < proj.years.size(); k++) {
    PlanYear y;
    y.year = proj.years[k];
    y.cost = detail_plan::at(p.cost, y.year, scale);
    y.receipts = detail_plan::at(p.receipts, y.year);
    y.grants = detail_plan::at(p.grants, y.year);
    const double carried = reserve * (1 + p.reserve_rate);
    const double funds = y.receipts + y.grants + carried;
    const double paid = std::min(y.cost, funds);
    y.from_reserve = std::max(0.0, paid - y.receipts - y.grants);
    y.borrowed = y.cost - paid;
    if (y.borrowed > 0) drawn.push_back({"drawn in " + std::to_string(y.year), y.borrowed, p.rate, y.year + 1, p.tenor, false});
    for (const auto& l : drawn) {
      const auto s = service(l, y.year);
      y.new_service += s.interest + s.principal;
      y.outstanding_new += s.outstanding_after;
    }
    const double left = funds - paid;
    y.pledged = p.pledge ? std::min(left, y.new_service) : 0.0;
    reserve = left - y.pledged;
    y.reserve = reserve;
    y.existing_service = proj.debt_service[k];
    y.surplus = proj.surplus[k];
    const double carried_by_budget = y.existing_service + y.new_service - y.pledged;
    y.coverage = carried_by_budget > 1e-12 ? y.surplus / carried_by_budget : none;
    if (std::isfinite(y.coverage) && y.coverage < R.least_coverage) R.least_coverage = y.coverage, R.least_year = y.year;
    R.borrowed += y.borrowed, R.received += y.receipts, R.granted += y.grants, R.cost += y.cost;
    R.years.push_back(y);
  }
  if (!std::isfinite(R.least_coverage)) R.least_coverage = none;
  R.reserve_left = reserve;
  R.new_debt_left = R.years.back().outstanding_new;
  return R;
}

struct Affordable {
  double scale = 0;          // the largest scale whose least coverage stays at or above the floor
  bool bounded = true;       // false when the floor holds at the largest scale tried
  bool fails_at_none = false;  // the floor fails even without the project
  std::string binding;       // the year that binds, said
};

// The largest scale s* in [0, most] keeping the least coverage at or above `floor` (Proposition 1), by bisection to `tolerance`.
inline Affordable largest_scale(const Projection& proj, const Plan& p, double floor, double most = 10, double tolerance = 1e-6) {
  if (!(floor > 0) || !(most > 0) || !(tolerance > 0)) throw std::invalid_argument("economics::fiscal: a positive floor, largest scale and tolerance");
  const auto holds = [&](double s) {
    const auto r = plan(proj, p, s);
    return !std::isfinite(r.least_coverage) || r.least_coverage >= floor - 1e-12;
  };
  Affordable a;
  if (!holds(0)) {
    a.fails_at_none = true;
    a.binding = "the coverage is below the floor even without the project";
    return a;
  }
  if (holds(most)) {
    a.scale = most;
    a.bounded = false;
    a.binding = "the floor holds at the largest scale tried";
    return a;
  }
  double lo = 0, hi = most;
  while (hi - lo > tolerance) {
    const double mid = 0.5 * (lo + hi);
    (holds(mid) ? lo : hi) = mid;
  }
  a.scale = lo;
  const auto r = plan(proj, p, hi);
  a.binding = "the coverage in " + std::to_string(r.least_year);
  return a;
}

}  // namespace economics::fiscal
