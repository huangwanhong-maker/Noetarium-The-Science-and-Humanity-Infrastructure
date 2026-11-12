// Procedure: what the order of steps, the weights of histories and their causal structure do, apart from the outcome
// (theory note, Part III §3).
//
// A procedure is a law on histories that lead from claims to an outcome. Three things are read from it:
//
//   order         two steps A and B, each a map on states that may be random, act on a function f of the state by
//                 (T_A f)(x) = E f(Ax). Their order sensitivity on f at x is ([T_A, T_B] f)(x): f after A then B, less f after
//                 B then A (Definition C.9). Steps that pay claimants in turn do not commute; `divide` gives what each
//                 order-neutral way of composing them awards, with the range the order alone can move a claimant by
//   weight        the weight of the histories that meet a condition (pure procedural justice), and the reliability of a
//                 procedure against a set of outcomes an independent criterion marks (imperfect procedural justice; perfect
//                 when it is one) (Equation 4.3)
//   discrepancy   two procedures with one law of outcomes differ only in what a record of outcomes hides: `compare` splits
//                 their relative entropy into the part the outcomes show and the part they hide (Proposition C.15, on
//                 tyche's chain rule), and `asymmetry` measures a procedure against its own mirror image under an exchange
//                 of the parties (equal treatment)
//   heard         on a history: whether, before every event of one type that touches a party, an event of another type
//                 touches both that party and the junction that decides (the hearing condition of Definition C.14, in the
//                 worldline order of the party)
//
// What counts as just is a frame's to say; these are the quantities a frame may be stated in.
//
// Sources: W. Huang (2026), Cases v0.9b, §4.3 to §4.5. Rawls (1971), A Theory of Justice, §14 (the three kinds).

#pragma once

#include <markets/foam/foam.hpp>

#include <calx/linalg/dense.hpp>
#include <nomos/games/claims.hpp>
#include <tyche/probability/information.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <stdexcept>
#include <vector>

namespace markets::procedure {

// ---- order -------------------------------------------------------------------------------------------------------------

// A step on a finite set of states as a stochastic matrix: T(x, y) the probability that the step takes x to y.
// ([T_A, T_B] f)(x) = (T_A T_B f)(x) − (T_B T_A f)(x): A first, then B, less B first, then A.
inline double order_sensitivity(const calx::linalg::Matrix& A, const calx::linalg::Matrix& B, const std::vector<double>& f, std::size_t x) {
  const std::size_t n = A.n;
  if (B.n != n || f.size() != n || x >= n) throw std::invalid_argument("markets::procedure::order_sensitivity: two steps and a function on one set of states");
  std::vector<double> af(n, 0.0), bf(n, 0.0);
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) af[i] += A(i, j) * f[j], bf[i] += B(i, j) * f[j];
  double ab = 0, ba = 0;
  for (std::size_t j = 0; j < n; j++) ab += A(x, j) * bf[j], ba += B(x, j) * af[j];
  return ab - ba;
}

// An estate short of the claims on it, divided by each rule; and how far the order of payment alone can move each claimant.
struct Division {
  std::vector<double> least, greatest;  // over every order of priority
  std::vector<double> random_arrival;   // the orders mixed by lot: the Shapley value
  std::vector<double> proportional;     // paid together at rates in proportion to the claims
  std::vector<double> equal_awards;     // paid together at equal rates
  std::vector<double> equal_losses, talmud;
};

inline Division divide(const nomos::games::Claims& P) {
  Division D;
  const auto range = nomos::games::priority_range(P);
  D.least = range.least;
  D.greatest = range.greatest;
  D.random_arrival = P.claims.size() <= 24 ? nomos::games::random_arrival(P) : std::vector<double>{};
  D.proportional = nomos::games::proportional(P);
  D.equal_awards = nomos::games::constrained_equal_awards(P);
  D.equal_losses = nomos::games::constrained_equal_losses(P);
  D.talmud = nomos::games::talmud(P);
  return D;
}

// ---- weights -----------------------------------------------------------------------------------------------------------

struct Procedure {
  std::vector<double> weight;         // the probability of each history
  std::vector<std::size_t> outcome;   // the outcome each ends in
  std::vector<char> admissible;       // whether each meets the conditions asked of a history; empty for all
};

inline void validate(const Procedure& P) {
  if (P.weight.size() != P.outcome.size() || (!P.admissible.empty() && P.admissible.size() != P.weight.size()))
    throw std::invalid_argument("markets::procedure::Procedure: a weight and an outcome for each history");
  tyche::probability::require_law(P.weight, "Procedure");
}

inline std::vector<double> outcome_law(const Procedure& P) {
  validate(P);
  return tyche::probability::observed(P.weight, tyche::probability::coarsening(P.outcome));
}

inline double admissible_weight(const Procedure& P) {
  validate(P);
  double w = 0;
  for (std::size_t h = 0; h < P.weight.size(); h++)
    if (P.admissible.empty() || P.admissible[h]) w += P.weight[h];
  return w;
}

// The weight of the histories that end in the outcomes marked just.
inline double reliability(const Procedure& P, const std::vector<char>& just) {
  validate(P);
  double w = 0;
  for (std::size_t h = 0; h < P.weight.size(); h++)
    if (just.at(P.outcome[h])) w += P.weight[h];
  return w;
}

// Two procedures on the same histories with the same outcomes: what a record of outcomes shows of their difference, and hides.
inline tyche::probability::Discrepancy compare(const Procedure& P, const Procedure& Q) {
  validate(P);
  validate(Q);
  if (P.outcome != Q.outcome) throw std::invalid_argument("markets::procedure::compare: two procedures on the same histories, ending in the same outcomes");
  return tyche::probability::discrepancy(P.weight, Q.weight, tyche::probability::coarsening(P.outcome));
}

// D(P ‖ P∘mirror): `mirror[h]` is the history in which the parties have changed places. Zero under equal treatment.
inline double asymmetry(const Procedure& P, const std::vector<std::size_t>& mirror) {
  validate(P);
  if (mirror.size() != P.weight.size()) throw std::invalid_argument("markets::procedure::asymmetry: a mirror image for each history");
  std::vector<double> q(P.weight.size());
  for (std::size_t h = 0; h < q.size(); h++) q[h] = P.weight.at(mirror[h]);
  return tyche::probability::relative_entropy(P.weight, q);
}

// ---- conditions on a history ---------------------------------------------------------------------------------------------

// Whether every event of type `judgment` that touches `party` is preceded, on the party's worldline, by an event of type
// `hearing` that touches both the party and `decider`.
inline bool heard(const foam::History& H, const foam::Foam& F, std::size_t party, std::size_t decider, std::size_t hearing, std::size_t judgment) {
  if (party >= F.junctions || decider >= F.junctions) throw std::invalid_argument("markets::procedure::heard: junctions of the history");
  bool was_heard = false;
  for (std::size_t v : F.worldline[party]) {
    const auto& at = F.touched[v];
    if (H.events[v].type == hearing && std::binary_search(at.begin(), at.end(), decider)) was_heard = true;
    if (H.events[v].type == judgment && !was_heard) return false;
  }
  return true;
}

}  // namespace markets::procedure
