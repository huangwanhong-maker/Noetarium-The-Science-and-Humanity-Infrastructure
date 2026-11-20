// Who finally gains from an allocation: where the money lands is only the first of three parts (the survey, §1.7).
//
//   first round    who receives: the round's result
//   propagated     where the money goes next: re-spending kept in the place, round by round (a geometric series, the local
//                  multiplier), and LM3 = (R1 + R2 + R3)/R1, the New Economics Foundation's measure of local retention from three
//                  rounds of a survey (Sacks 2002). Neither is a counterfactual impact: fixed shares, idle capacity, no prices.
//   capitalised    what prices do: of the value residents place on what a place-directed amount buys, a share
//                  κ = ε_mig / (ε_mig + ε_supply) is capitalised into rents in spatial equilibrium (the survey, Part III §3, after
//                  Roback 1982 and Kline and Moretti 2014): renters keep (1 − κ) of it, owner-occupiers all of it (as occupiers they
//                  pay the rise, as owners they receive it), landlords receive κ of the renters' part, in the place or away. The
//                  elasticities are declared as intervals, and κ, monotone in each, is reported as an exact interval.
//
// And three readings the survey's Part IV supplies, each a direction or a shape, not a verdict (Table 1.2):
//
//   return to generators   on a network of flows, a generator g's return ratio ρ_g = (value flowing back to g) / (value g sent
//                          out), and its extraction share ε_g = 1 − min(ρ_g, 1); the circulating share of the flows, the part of
//                          their energy that is not potential-driven, by the economics domain's Hodge decomposition (Part IV §6).
//                          ρ_g, ε_g and the circulating share as a reading of Eglash's generative justice are the survey's proposal,
//                          not Eglash's, and a cycle can be extractive (debt service returns to a lender), so they are read with
//                          labels of control and kind, never alone.
//   floor and ceiling      a doughnut (Raworth 2017): each entity's shortfall below a declared floor and overshoot above a declared
//                          ceiling, counted apart, never netted.
//   targeting              a place-based amount against the people it is meant for: the share that reaches them if each place
//                          spreads its amount over its people, and the people meant who live where nothing goes ("many deprived
//                          people live in non-deprived areas", the IMD 2025's own warning).
#pragma once

#include <economics/circulation/flows.hpp>
#include <economics/transfer/network.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace allocation::incidence {

struct Refusal : std::runtime_error {
  explicit Refusal(const std::string& why) : std::runtime_error(why) {}
};

struct Interval {
  double lo = 0, hi = 0;
};

// ---- Capitalisation -----------------------------------------------------------------------------------------------------------

struct Capitalisation {
  double value_per_unit = 1;           // what residents are willing to pay, per unit spent, for what it buys (declared)
  Interval migration{1, 1};            // ε_mig: the response of residents' numbers to the place's attractiveness
  Interval supply{1, 1};               // ε_supply: the response of housing to rents
};

struct Split {
  Interval share;                      // κ, the capitalised share
  Interval renters;                    // what renters keep: (1 − κ) v g s_r
  double owner_occupiers = 0;          // v g (1 − s_r): the amenity's value, the rent rise paid and received
  Interval landlords_local, landlords_away;  // κ v g s_r, in the place and away
  std::string said;
};

inline Split capitalise(double amount, double renters_share, double landlords_local, const Capitalisation& c) {
  if (!(renters_share >= 0 && renters_share <= 1) || !(landlords_local >= 0 && landlords_local <= 1)) throw Refusal("shares lie in [0, 1]");
  const auto& m = c.migration;
  const auto& s = c.supply;
  if (!(m.lo >= 0 && m.lo <= m.hi && s.lo >= 0 && s.lo <= s.hi) || m.hi + s.lo <= 0) throw Refusal("elasticities are intervals of numbers not below zero, not both zero");
  Split out;
  // κ rises with ε_mig and falls with ε_supply, so its bounds are at opposite corners.
  out.share = {m.lo + s.hi > 0 ? m.lo / (m.lo + s.hi) : 0.0, m.hi / (m.hi + s.lo)};
  const double v = c.value_per_unit * amount;
  out.renters = {(1 - out.share.hi) * v * renters_share, (1 - out.share.lo) * v * renters_share};
  out.owner_occupiers = v * (1 - renters_share);
  out.landlords_local = {out.share.lo * v * renters_share * landlords_local, out.share.hi * v * renters_share * landlords_local};
  out.landlords_away = {out.share.lo * v * renters_share * (1 - landlords_local), out.share.hi * v * renters_share * (1 - landlords_local)};
  out.said = "between " + std::to_string(static_cast<int>(std::lround(100 * out.share.lo))) + "% and " + std::to_string(static_cast<int>(std::lround(100 * out.share.hi))) +
             "% of the value is capitalised into rents under the elasticities declared";
  return out;
}

// ---- Propagation --------------------------------------------------------------------------------------------------------------

struct Propagation {
  std::vector<double> rounds;          // R1 = g, R_{k+1} = c r R_k
  double total = 0;                    // g / (1 − c r): income generated in the place over every round
  double multiplier = 0;               // total / g
  double lm3 = 0;                      // (R1 + R2 + R3) / R1
  double leaked = 0;                   // spent outside the place over every round: c (1 − r) total
};

// An injection g re-spent round by round: of each round's income a share c is spent again, of which a share r stays in the place.
inline Propagation propagate(double g, double c, double r, std::size_t shown = 3) {
  if (!(c >= 0 && c <= 1 && r >= 0 && r <= 1)) throw Refusal("the shares re-spent and kept lie in [0, 1]");
  if (c * r >= 1) throw Refusal("nothing leaks, so the series does not converge");
  Propagation p;
  double R = g;
  for (std::size_t k = 0; k < std::max<std::size_t>(shown, 3); k++) p.rounds.push_back(R), R *= c * r;
  p.total = g / (1 - c * r);
  p.multiplier = g != 0 ? p.total / g : 0;
  p.lm3 = g != 0 ? (p.rounds[0] + p.rounds[1] + p.rounds[2]) / g : 0;
  // Each round's income R_k is either re-spent locally (c r R_k), spent elsewhere (c (1 − r) R_k) or not spent (1 − c) R_k.
  p.leaked = c * (1 - r) * p.total;
  return p;
}

inline double lm3(double r1, double r2, double r3) {
  if (!(r1 > 0)) throw Refusal("LM3 needs a first round");
  return (r1 + r2 + r3) / r1;
}

// ---- Return to generators -----------------------------------------------------------------------------------------------------

struct Generators {
  std::vector<double> sent, returned;  // per generator: value sent out, value flowing back
  std::vector<double> ratio;           // ρ_g (NaN when nothing was sent)
  std::vector<double> extraction;      // ε_g = 1 − min(ρ_g, 1)
  double pooled_ratio = 0;             // Σ returned / Σ sent over the generators
  std::vector<std::pair<std::size_t, double>> captured;  // positions other than generators with a net inflow, and how much
  double circulating = 0;              // the share of the flows' energy round faces and holes (economics::circulation)
  std::vector<std::pair<std::string, double>> circulating_by_kind;
  std::string said;
};

inline Generators returns(const economics::TransferNetwork& N, const std::vector<std::size_t>& generators) {
  Generators G;
  const std::size_t P = N.positions.size();
  std::vector<bool> is(P, false);
  for (std::size_t g : generators) {
    if (g >= P) throw Refusal("a generator is not a position of the network");
    is[g] = true;
  }
  std::vector<double> out(P, 0), in(P, 0);
  for (const auto& l : N.legs) {
    if (l.flow < 0) throw Refusal("flows on a network of value are not negative; orient the leg the other way");
    out[l.from] += l.flow;
    in[l.to] += l.flow * l.ratio;
  }
  double S = 0, R = 0;
  for (std::size_t g : generators) {
    G.sent.push_back(out[g]);
    G.returned.push_back(in[g]);
    const double rho = out[g] > 0 ? in[g] / out[g] : std::numeric_limits<double>::quiet_NaN();
    G.ratio.push_back(rho);
    G.extraction.push_back(std::isfinite(rho) ? 1 - std::min(rho, 1.0) : std::numeric_limits<double>::quiet_NaN());
    S += out[g], R += in[g];
  }
  G.pooled_ratio = S > 0 ? R / S : 0;
  for (std::size_t p = 0; p < P; p++)
    if (!is[p] && in[p] - out[p] > 0) G.captured.push_back({p, in[p] - out[p]});
  std::sort(G.captured.begin(), G.captured.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
  if (!N.legs.empty()) {
    const auto C = economics::circulation(N);
    G.circulating = C.local + C.global;
    std::vector<std::string> kinds;
    for (const auto& l : N.legs)
      if (std::find(kinds.begin(), kinds.end(), l.kind) == kinds.end()) kinds.push_back(l.kind);
    if (kinds.size() > 1)
      for (const auto& k : kinds) {
        economics::TransferNetwork K;
        K.positions = N.positions;
        for (const auto& l : N.legs)
          if (l.kind == k) K.legs.push_back(l);
        const auto Ck = economics::circulation(K);
        G.circulating_by_kind.push_back({k, Ck.local + Ck.global});
      }
  }
  G.said = "of what the generators sent out, " + std::to_string(static_cast<int>(std::lround(100 * G.pooled_ratio))) +
           "% came back to them; a reading proposed by the survey after Eglash, to be read with who controls each flow";
  return G;
}

// ---- Floor and ceiling --------------------------------------------------------------------------------------------------------

struct Doughnut {
  std::vector<double> shortfall, overshoot;   // per entity, per head
  double shortfall_total = 0, overshoot_total = 0;  // weighted by people
  double people_below = 0, people_above = 0;
};

inline Doughnut doughnut(const std::vector<double>& per_head, const std::vector<double>& people, double floor, double ceiling) {
  if (per_head.size() != people.size()) throw Refusal("people for every entity");
  if (!(floor <= ceiling)) throw Refusal("a floor below its ceiling");
  Doughnut d;
  for (std::size_t i = 0; i < per_head.size(); i++) {
    const double s = std::max(0.0, floor - per_head[i]), o = std::max(0.0, per_head[i] - ceiling);
    d.shortfall.push_back(s), d.overshoot.push_back(o);
    d.shortfall_total += s * people[i], d.overshoot_total += o * people[i];
    if (s > 0) d.people_below += people[i];
    if (o > 0) d.people_above += people[i];
  }
  return d;
}

// ---- Targeting ----------------------------------------------------------------------------------------------------------------

struct Targeting {
  double reaching = 0;        // the share of the amount reaching the people meant, if each place spreads it over its people
  double meant = 0;           // the people meant, in all
  double missed = 0;          // the people meant who live where nothing goes
  double per_person_meant = 0;   // the amount per person meant, were it given to them directly
  std::string said;
};

inline Targeting targeting(const std::vector<double>& amount, const std::vector<double>& population, const std::vector<double>& meant) {
  if (amount.size() != population.size() || amount.size() != meant.size()) throw Refusal("an amount, a population and a count of people meant for every place");
  Targeting t;
  double A = 0, reach = 0;
  for (std::size_t j = 0; j < amount.size(); j++) {
    if (meant[j] > population[j] || meant[j] < 0) throw Refusal("the people meant in a place are among its people");
    A += amount[j];
    t.meant += meant[j];
    if (population[j] > 0) reach += amount[j] * meant[j] / population[j];
    if (amount[j] <= 0) t.missed += meant[j];
  }
  t.reaching = A > 0 ? reach / A : 0;
  t.per_person_meant = t.meant > 0 ? A / t.meant : 0;
  t.said = std::to_string(static_cast<int>(std::lround(100 * t.reaching))) + "% of the amount reaches the people meant if spread over each place's people; " +
           std::to_string(static_cast<long long>(std::llround(t.missed))) + " of them live where nothing goes";
  return t;
}

}  // namespace allocation::incidence
