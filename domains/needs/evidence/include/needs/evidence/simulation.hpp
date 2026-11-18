// Simulation (ADR 0125; the theory note §3.5): whether a mechanism of seeking, run on the data's places and suppliers, leaves the
// group with more unmet demand than its reference. The mechanism is offered for the gap; the test says whether it can produce it.
//
// Two mechanisms are run, once for a domain and an indicator, and every statement is then judged on what they left:
// - The two-step floating catchment (Luo and Wang 2003; nomos's measurement): each supplier's capacity shared among the demand
//   within reach, each place's access the sum of the shares it reaches. A place with no supplier in reach has access 0.
// - A model of seeking: the demand of each place, in units of one size, arrives in a random order; each unit chooses among the
//   suppliers within reach that have capacity left, by a logit on distance (nomos's agents: choose), and takes what it needs, or
//   what is left; what finds nothing is unmet. Repeated with keyed streams, the unmet share of each place is an average with its
//   Monte Carlo error. Its parameters (the reach, the logit's scale, the unit's size) are declared, not fitted: the model is a
//   mechanism offered, not an estimate of behaviour.
//
// Demand: for a share, the people who have the condition (its successes); for a value, the people the units stand for; for an
// expressed need, its demand column. Every unit of every part competes for capacity; the group's and the reference's shares are
// reckoned on the part asked.
#pragma once

#include <needs/evidence/causal.hpp>
#include <needs/evidence/gap.hpp>

#include <nomos/agents/choice.hpp>
#include <nomos/measurement/accessibility.hpp>

#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <string>
#include <vector>

namespace needs::evidence {

struct SeekingOptions {
  std::size_t replications = 24;
  std::size_t units = 3000;        // demand is cut into about this many units
  double scale = 0;                // the logit's distance scale in metres (a third of the reach when 0)
  std::uint64_t key = 11;
};

// What the mechanisms left at each place.
struct Field {
  bool applicable = false;
  std::string why, domain, indicator;
  std::vector<std::string> places;
  std::vector<double> demand, access;              // per place
  std::vector<std::vector<double>> unmet;          // [replication][place]: the share of the place's demand left unmet
  std::vector<int> place_of_row;                   // −1 when the row has no place
  std::size_t suppliers = 0;
  double capacity = 0, total_demand = 0;
};

struct Simulation {
  bool applicable = false;
  std::string why;
  Verdict verdict = Verdict::undecided;
  double access_group = nan, access_reference = nan;      // demand-weighted mean of the floating catchment's access
  double without_group = nan, without_reference = nan;    // the share of demand with nothing in reach
  double unmet_group = nan, unmet_reference = nan;        // the seeking model's unmet share, averaged over replications
  double difference = nan, low = nan, high = nan;         // unmet_group − unmet_reference, and its Monte Carlo interval
};

namespace detail {
inline std::vector<double> demand_of_rows(const Dataset& d, const std::string& indicator) {
  const auto& t = d.table;
  std::vector<double> out(t.rows, 0.0);
  const language::Indicator* ind = d.indicators.count(indicator) ? &d.indicator(indicator) : nullptr;
  const auto& y = t.at(ind ? ind->column : indicator).numbers;
  const std::vector<double>* x = ind && !ind->total.empty() ? &t.at(ind->total).numbers : nullptr;
  for (std::size_t i = 0; i < t.rows; i++) {
    double v;
    if (!ind) v = y[i];                                    // an expressed need's demand column
    else if (ind->share) v = y[i];                         // those who have the condition
    else v = x ? (*x)[i] : 1.0;                            // the people the value stands for
    out[i] = std::isfinite(v) && v > 0 ? v * t.w(i) : 0.0;
  }
  return out;
}
}  // namespace detail

inline Field simulate(const Dataset& d, const std::string& domain, const std::string& indicator, const SeekingOptions& o = {}) {
  Field f;
  f.domain = domain, f.indicator = indicator;
  if (d.place.empty() || d.where.empty()) {
    f.why = "the data declare no places for the units";
    return f;
  }
  std::vector<const language::Supplier*> sup;
  for (const auto& s : d.suppliers)
    if ((s.domain.empty() || s.domain == domain) && s.capacity > 0) sup.push_back(&s);
  if (sup.empty()) {
    f.why = "the data declare no supplier of " + domain;
    return f;
  }
  const auto& t = d.table;
  const auto& pc = t.at(d.place);
  if (pc.kind != groups::Kind::category) throw Refusal("the places " + d.place + " are a column of categories");
  std::map<std::string, int> pid;
  f.place_of_row.assign(t.rows, -1);
  for (std::size_t i = 0; i < t.rows; i++) {
    if (pc.codes[i] < 0) continue;
    const std::string& nm = pc.levels[static_cast<std::size_t>(pc.codes[i])];
    if (!d.where.count(nm)) continue;
    auto it = pid.find(nm);
    if (it == pid.end()) it = pid.emplace(nm, static_cast<int>(f.places.size())).first, f.places.push_back(nm);
    f.place_of_row[i] = it->second;
  }
  const std::size_t P = f.places.size(), S = sup.size();
  if (P == 0) {
    f.why = "no unit's place has a position";
    return f;
  }
  const auto rd = detail::demand_of_rows(d, indicator);
  f.demand.assign(P, 0.0);
  for (std::size_t i = 0; i < t.rows; i++)
    if (f.place_of_row[i] >= 0) f.demand[static_cast<std::size_t>(f.place_of_row[i])] += rd[i];
  f.total_demand = std::accumulate(f.demand.begin(), f.demand.end(), 0.0);
  if (!(f.total_demand > 0)) {
    f.why = "there is no demand";
    return f;
  }
  // Distances within reach.
  std::vector<std::vector<std::pair<std::size_t, double>>> reach(P);
  std::vector<nomos::measurement::Reachable> pairs;
  std::vector<double> capacity(S);
  for (std::size_t j = 0; j < S; j++) capacity[j] = sup[j]->capacity, f.capacity += sup[j]->capacity;
  for (std::size_t p = 0; p < P; p++) {
    const auto [px, py] = d.where.at(f.places[p]);
    for (std::size_t j = 0; j < S; j++) {
      const double dist = std::hypot(px - sup[j]->x, py - sup[j]->y);
      if (dist <= d.reach) reach[p].push_back({j, dist}), pairs.push_back({p, j, dist});
    }
  }
  f.suppliers = S;
  nomos::measurement::Catchment c;
  c.limit = d.reach, c.decay = nomos::measurement::Decay::binary;
  f.access = pairs.empty() ? std::vector<double>(P, 0.0) : nomos::measurement::two_step(pairs, f.demand, capacity, c).access;
  // The seeking model.
  const double unit = std::max(f.total_demand / static_cast<double>(std::max<std::size_t>(o.units, 1)), 1e-9);
  const double scale = o.scale > 0 ? o.scale : d.reach / 3;
  std::vector<std::pair<std::size_t, double>> agents;  // place, size
  for (std::size_t p = 0; p < P; p++) {
    double left = f.demand[p];
    while (left > 1e-12) {
      const double a = std::min(unit, left);
      agents.push_back({p, a});
      left -= a;
    }
  }
  f.unmet.assign(o.replications, std::vector<double>(P, 0.0));
  for (std::size_t r = 0; r < o.replications; r++) {
    calx::random::Stream s(o.key, static_cast<std::uint32_t>(r), 0x5ee4, 0);
    std::vector<std::size_t> order(agents.size());
    std::iota(order.begin(), order.end(), std::size_t{0});
    for (std::size_t k = order.size(); k > 1; k--) std::swap(order[k - 1], order[s.below(k)]);
    std::vector<double> left = capacity, lost(P, 0.0);
    std::vector<double> u;
    std::vector<std::size_t> open;
    for (std::size_t k : order) {
      const auto [p, size] = agents[k];
      u.clear(), open.clear();
      for (const auto& [j, dist] : reach[p])
        if (left[j] > 1e-12) open.push_back(j), u.push_back(-dist / scale);
      if (open.empty()) {
        lost[p] += size;
        continue;
      }
      const std::size_t j = open[nomos::agents::choose(s, u, 1.0)];
      const double take = std::min(size, left[j]);
      left[j] -= take;
      lost[p] += size - take;
    }
    for (std::size_t p = 0; p < P; p++) f.unmet[r][p] = f.demand[p] > 0 ? lost[p] / f.demand[p] : 0.0;
  }
  f.applicable = true;
  return f;
}

inline Simulation judge(const Field& f, const Dataset& d, const language::Statement& s, const Parts& parts, Part part) {
  Simulation out;
  if (!f.applicable) {
    out.why = f.why;
    return out;
  }
  out.applicable = true;
  const auto& t = d.table;
  const groups::Rows frame = groups::rows_of(t, s.frame);
  const groups::Rows g = groups::rows_of(t, s.group, frame);
  groups::Rows r = s.reference ? groups::rows_of(t, *s.reference, frame) : frame;
  if (!s.reference)
    for (std::size_t i = 0; i < t.rows; i++) r[i] = r[i] && !g[i];
  const auto rd = detail::demand_of_rows(d, f.indicator);
  const std::size_t R = f.unmet.size();
  double dg = 0, dr = 0, ag = 0, ar = 0, wg = 0, wr = 0;
  std::vector<double> ug(R, 0.0), ur(R, 0.0);
  for (std::size_t i = 0; i < t.rows; i++) {
    if (parts.part_of_row[i] != static_cast<int>(part) || f.place_of_row[i] < 0 || !(rd[i] > 0)) continue;
    const auto p = static_cast<std::size_t>(f.place_of_row[i]);
    const bool ig = g[i], ir = r[i];
    if (ig) dg += rd[i], ag += rd[i] * f.access[p], wg += f.access[p] <= 0 ? rd[i] : 0.0;
    if (ir) dr += rd[i], ar += rd[i] * f.access[p], wr += f.access[p] <= 0 ? rd[i] : 0.0;
    for (std::size_t k = 0; k < R; k++) {
      if (ig) ug[k] += rd[i] * f.unmet[k][p];
      if (ir) ur[k] += rd[i] * f.unmet[k][p];
    }
  }
  if (!(dg > 0) || !(dr > 0)) {
    out.why = !(dg > 0) ? "the group has no demand with a place in this part" : "the reference has no demand with a place in this part";
    return out;
  }
  out.access_group = ag / dg, out.access_reference = ar / dr;
  out.without_group = wg / dg, out.without_reference = wr / dr;
  std::vector<double> diff(R);
  double mg = 0, mr = 0, md = 0;
  for (std::size_t k = 0; k < R; k++) {
    ug[k] /= dg, ur[k] /= dr, diff[k] = ug[k] - ur[k];
    mg += ug[k] / R, mr += ur[k] / R, md += diff[k] / R;
  }
  double ss = 0;
  for (double v : diff) ss += (v - md) * (v - md);
  const double se = R > 1 ? std::sqrt(ss / static_cast<double>(R - 1) / static_cast<double>(R)) : 0.0;
  out.unmet_group = mg, out.unmet_reference = mr, out.difference = md, out.low = md - 2 * se, out.high = md + 2 * se;
  // The access the floating catchment gives decides when the seeking model leaves both unmet alike.
  if (out.low > 0) out.verdict = Verdict::holds, out.why = "the mechanism leaves the group more unmet than its reference";
  else if (out.high < 0) out.verdict = Verdict::fails, out.why = "the mechanism leaves the group less unmet than its reference";
  else if (md == 0 && out.access_group < out.access_reference * 0.9) out.verdict = Verdict::holds, out.why = "nothing is left unmet, but the group reaches less capacity per person";
  else out.why = "the mechanism leaves the group and its reference alike";
  return out;
}

}  // namespace needs::evidence
