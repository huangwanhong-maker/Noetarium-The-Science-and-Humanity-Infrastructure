// What a run of the accounts says about circulation (ADR 0139): the payments read from the postings, money followed from
// where it entered, and the measures of how it circulates.
//
// Payments. Each posting changes the money some agents hold; those whose money falls paid those whose money rose, in
// proportion, and money that appears with no payer was created by the agent responsible (lending, issuing), money that
// vanishes with no payee destroyed by it (repaying, redeeming). A payment is a reading, in real numbers; the accounts
// stay in whole units.
//
// Money followed from where it entered (Tracer). A posting labelled with an origin starts a trace: what its recipients
// receive is that origin's money, at round 0. Every later payment moves money out of the payer's holding in proportion
// to what the holding is made of (money is fungible, and the mixing is what proportional tracing assumes), one round
// further. An origin has a place, the agents counted local to it (by default those of its first recipient's region);
// money paid out of that place has leaked at its round and is followed no further. For an origin injected as A:
//   reached_k   the money that came to be held in the place at round k (reached_0 = A when its recipient is local);
//   leaked_k    the money paid out of the place at round k, and destroyed_k what was destroyed there (taxes, repayment);
//   held_k      what is still held in the place at round k when the run ends (not yet spent);
//   LM3         (R1 + R2 + R3)/R1 with R_k = reached_{k−1}: the New Economics Foundation's measure of local retention
//               (Sacks 2002, The Money Trail), its three-round truncation;
//   rounds      Σ_k reached_k / A, the expected number of local holdings of a unit before it leaves (truncated by the run).
//
// The walk of money (markov_retention). Read from the payments of a window as a walk, a unit at i goes next to j with the
// share of i's payments that went to j; the share still in the place after k payments from an injection at s is
// 1_Lᵀ (Qᵀ)^k e_s, Q the walk restricted to the place, and the expected number of local holdings Σ_k of it, which is
// row s of (I − Q)⁻¹ summed over the place. When every agent's payments go to the same places in the same shares
// whatever money pays them, the traced retention after a long run is this walk's; the two then measure one thing two ways.
//
// Velocity: the money paid in a window over the money held on average, per unit of time; by sector of the payer; and its
// inverse, the mean time a unit is held (Little's law, for the whole stock and not for each unit).
//
// Circulation: the payments of a window as a flow on a network of positions (agents, regions or sectors), split by the
// economics domain's Hodge decomposition (circulation.hpp) into what is driven up an attraction and what circulates;
// and, combinatorially, the share of value paid within strongly connected components (Tarjan, by tyche's networks), so
// that the two notions of circulation can be set side by side (Iosifidis et al. 2021; Soramäki et al. 2007 report 90%
// of Fedwire's value within its giant strongly connected component).
//
// Concentration: the Gini coefficient of money held and of money paid (nomos's measurement).
#pragma once

#include <economics/circulation/flows.hpp>
#include <economics/ledger/accounts.hpp>
#include <nomos/measurement/inequality.hpp>
#include <tyche/networks/descriptive.hpp>
#include <tyche/networks/network.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <string>
#include <utility>
#include <vector>

namespace economics::ledger {

// ---- payments -----------------------------------------------------------------------------------------------------------

struct Payment {
  double time = 0;
  std::size_t from = 0, to = 0;
  double amount = 0;
  std::size_t kind = 0;  // into Recorder::kinds
  Act act = Act::transfer;
  bool created = false, destroyed = false;  // money that appeared (from the responsible issuer) or vanished (to it)
};

namespace detail {

// The payments a posting's changes in money held amount to: losers paid gainers in proportion; a surplus of gains came
// from the responsible agent (created), a surplus of losses went to it (destroyed).
struct Move {
  std::size_t from, to;
  double amount;
  bool created, destroyed;
};
inline std::vector<Move> moves(const Posting& p, const std::vector<MoneyChange>& changes) {
  double gain = 0, loss = 0;
  for (const auto& c : changes) (c.delta > 0 ? gain : loss) += static_cast<double>(c.delta > 0 ? c.delta : -c.delta);
  struct Side {
    std::size_t agent;
    double amount;
    bool pseudo;
  };
  std::vector<Side> losers, gainers;
  for (const auto& c : changes)
    if (c.delta < 0) losers.push_back({c.agent, static_cast<double>(-c.delta), false});
    else gainers.push_back({c.agent, static_cast<double>(c.delta), false});
  if (gain > loss) losers.push_back({p.responsible, gain - loss, true});
  if (loss > gain) gainers.push_back({p.responsible, loss - gain, true});
  const double total = std::max(gain, loss);
  std::vector<Move> out;
  if (!(total > 0)) return out;
  for (const auto& l : losers)
    for (const auto& g : gainers) {
      const double a = l.amount * g.amount / total;
      if (a > 0) out.push_back({l.agent, g.agent, a, l.pseudo, g.pseudo});
    }
  return out;
}

}  // namespace detail

// Keeps every payment of the postings it observes.
class Recorder {
 public:
  std::vector<Payment> payments;
  std::vector<std::string> kinds;

  void attach(Ledger& L) {
    L.observe([this](const Posting& p, std::size_t, const std::vector<MoneyChange>& c) { take(p, c); });
  }
  std::size_t kind(const std::string& k) {
    for (std::size_t i = 0; i < kinds.size(); i++)
      if (kinds[i] == k) return i;
    kinds.push_back(k);
    return kinds.size() - 1;
  }

 private:
  void take(const Posting& p, const std::vector<MoneyChange>& c) {
    if (p.act == Act::open) return;
    const std::string k = p.flows.size() == 1 ? p.flows[0].kind : p.flows.empty() ? std::string(name(p.act)) : std::string("several");
    const std::size_t ki = kind(k);
    for (const auto& m : detail::moves(p, c)) payments.push_back({p.time, m.from, m.to, m.amount, ki, p.act, m.created, m.destroyed});
  }
};

// The payments within [t0, t1) gathered between positions: position[a] is agent a's position (an agent, a region, a
// sector), or none to leave it out; created and destroyed money left out unless asked.
inline std::map<std::pair<std::size_t, std::size_t>, double> gather(const std::vector<Payment>& P, const std::vector<std::size_t>& position, double t0, double t1,
                                                                    bool with_created = false) {
  std::map<std::pair<std::size_t, std::size_t>, double> F;
  for (const auto& p : P) {
    if (p.time < t0 || p.time >= t1) continue;
    if ((p.created || p.destroyed) && !with_created) continue;
    const std::size_t a = position.at(p.from), b = position.at(p.to);
    if (a == none || b == none || a == b) continue;
    F[{a, b}] += p.amount;
  }
  return F;
}

// ---- money followed from where it entered ---------------------------------------------------------------------------

struct Trace {
  std::string origin, region;
  double injected = 0;
  std::vector<double> reached, leaked, destroyed, held;  // by round, the last entry "this round or later"
  double retagged = 0;  // money of this origin re-labelled by a later origin's posting
  // (R1 + R2 + R3)/R1.
  double lm3() const { return reached.size() >= 3 && reached[0] > 0 ? (reached[0] + reached[1] + reached[2]) / reached[0] : 0; }
  // reached_k / A.
  std::vector<double> retention() const {
    std::vector<double> r(reached.size(), 0);
    if (injected > 0)
      for (std::size_t k = 0; k < r.size(); k++) r[k] = reached[k] / injected;
    return r;
  }
  double rounds() const {
    double s = 0;
    for (double x : reached) s += x;
    return injected > 0 ? s / injected : 0;
  }
  double leaked_total() const {
    double s = 0;
    for (double x : leaked) s += x;
    return s;
  }
  double held_total() const {
    double s = 0;
    for (double x : held) s += x;
    return s;
  }
};

class Tracer {
 public:
  // `rounds`: the rounds told apart (the last bucket holds that round and later).
  explicit Tracer(std::size_t rounds = 12) : K_(std::max<std::size_t>(rounds, 3)) {}

  // Declares the place of an origin before it appears (otherwise its first recipient's region).
  void place(const std::string& origin, const std::string& region) { declared_[origin] = region; }
  void attach(Ledger& L) {
    L_ = &L;
    buckets_.assign(L.agents().size(), {});
    L.observe([this](const Posting& p, std::size_t, const std::vector<MoneyChange>& c) { take(p, c); });
  }
  // The traces, with what is still held in each origin's place now.
  std::vector<Trace> traces() const {
    std::vector<Trace> T = traces_;
    for (auto& t : T) t.held.assign(K_ + 1, 0);
    for (std::size_t a = 0; a < buckets_.size(); a++)
      for (const auto& b : buckets_[a]) T[b.origin].held[b.round] += b.amount;
    return T;
  }
  const Trace& trace(const std::string& origin) const {
    for (const auto& t : traces_)
      if (t.origin == origin) return t;
    throw Refusal("no money of origin '" + origin + "' was traced");
  }
  bool local(std::size_t origin, std::size_t agent) const {
    if (agent == L_->outside()) return false;
    const std::string& r = traces_[origin].region;
    return r.empty() || L_->agents()[agent].region == r;
  }

 private:
  struct Bucket {
    std::size_t origin, round;
    double amount;
  };
  double& bucket(std::size_t agent, std::size_t origin, std::size_t round) {
    for (auto& b : buckets_[agent])
      if (b.origin == origin && b.round == round) return b.amount;
    buckets_[agent].push_back({origin, round, 0});
    return buckets_[agent].back().amount;
  }
  std::size_t origin(const std::string& label, std::size_t recipient) {
    for (std::size_t k = 0; k < traces_.size(); k++)
      if (traces_[k].origin == label) return k;
    Trace t;
    t.origin = label;
    const auto it = declared_.find(label);
    t.region = it != declared_.end() ? it->second : L_->agents()[recipient].region;
    t.reached.assign(K_ + 1, 0);
    t.leaked.assign(K_ + 1, 0);
    t.destroyed.assign(K_ + 1, 0);
    traces_.push_back(t);
    return traces_.size() - 1;
  }
  void take(const Posting& p, const std::vector<MoneyChange>& changes) {
    if (p.act == Act::open) return;
    const auto M = detail::moves(p, changes);
    // Out of each real payer: its buckets in proportion to the share of its holding paid, to each destination.
    std::map<std::size_t, double> lost;
    for (const auto& m : M)
      if (!m.created) lost[m.from] += m.amount;
    for (const auto& [payer, amount] : lost) {
      const double before = static_cast<double>(L_->money(payer)) + amount;
      if (!(before > 0) || buckets_[payer].empty()) continue;
      const double f = std::min(1.0, amount / before);
      std::vector<Bucket> out = buckets_[payer];
      for (auto& b : buckets_[payer]) b.amount -= b.amount * f;
      std::erase_if(buckets_[payer], [](const Bucket& b) { return b.amount <= 1e-12; });
      for (const auto& m : M) {
        if (m.from != payer || m.created) continue;
        const double share = m.amount / amount;
        for (const auto& b : out) {
          const double part = b.amount * f * share;
          if (!(part > 0)) continue;
          if (m.destroyed) traces_[b.origin].destroyed[b.round] += part;
          else if (!p.origin.empty()) traces_[b.origin].retagged += part;
          else if (local(b.origin, m.to)) {
            const std::size_t r = std::min(b.round + 1, K_);
            bucket(m.to, b.origin, r) += part;
            traces_[b.origin].reached[r] += part;
          } else {
            traces_[b.origin].leaked[b.round] += part;
          }
        }
      }
    }
    // Into the recipients of a labelled posting: a new trace at round 0.
    if (!p.origin.empty())
      for (const auto& c : changes)
        if (c.delta > 0) {
          const std::size_t o = origin(p.origin, c.agent);
          traces_[o].injected += static_cast<double>(c.delta);
          if (local(o, c.agent)) {
            bucket(c.agent, o, 0) += static_cast<double>(c.delta);
            traces_[o].reached[0] += static_cast<double>(c.delta);
          }
        }
  }

  std::size_t K_;
  const Ledger* L_ = nullptr;
  std::vector<std::vector<Bucket>> buckets_;
  std::vector<Trace> traces_;
  std::map<std::string, std::string> declared_;
};

// ---- the walk of money ----------------------------------------------------------------------------------------------

struct Walk {
  std::vector<double> retention;  // the share in the place after k payments, k = 0 … rounds
  double lm3 = 0;                 // 1 + r_1 + r_2
  double rounds = 0;              // Σ_k r_k, to convergence
  double leak = 0;                // the share that leaves the place (what stays with an agent that pays nothing out is not counted)
};

// From gathered flows between agents: the walk from `start` within `place` (one flag per agent).
inline Walk markov_retention(const std::map<std::pair<std::size_t, std::size_t>, double>& F, std::size_t agents, const std::vector<char>& place, std::size_t start,
                             std::size_t rounds = 12, double tolerance = 1e-14, std::size_t limit = 100000) {
  if (place.size() != agents || start >= agents) throw Refusal("markov_retention: a flag for every agent, and a start among them");
  std::vector<double> out(agents, 0);
  for (const auto& [ij, v] : F) out[ij.first] += v;
  std::vector<std::vector<std::pair<std::size_t, double>>> Q(agents);
  for (const auto& [ij, v] : F)
    if (place[ij.second] && place[ij.first] && out[ij.first] > 0) Q[ij.first].push_back({ij.second, v / out[ij.first]});
  Walk W;
  std::vector<double> x(agents, 0);
  if (place[start]) x[start] = 1;
  double total = place[start] ? 1 : 0;
  W.retention.push_back(total);
  W.rounds = total;
  double stuck = 0;
  for (std::size_t k = 1; k < limit && total > tolerance; k++) {
    std::vector<double> y(agents, 0);
    for (std::size_t i = 0; i < agents; i++)
      if (x[i] > 0) {
        // Money at an agent that pays nothing out stays there: it is held, not leaked, and counts no further round.
        if (!(out[i] > 0)) { stuck += x[i]; continue; }
        for (const auto& [j, q] : Q[i]) y[j] += x[i] * q;
      }
    x.swap(y);
    total = 0;
    for (double v : x) total += v;
    if (k <= rounds) W.retention.push_back(total);
    W.rounds += total;
  }
  W.lm3 = W.retention.size() >= 3 ? W.retention[0] + W.retention[1] + W.retention[2] : 0;
  W.leak = place[start] ? std::max(0.0, 1 - total - stuck) : 0;
  return W;
}

// ---- velocity -----------------------------------------------------------------------------------------------------------

struct Velocity {
  double paid = 0, held = 0, span = 0;  // money paid in the window, held on average, the window's length
  double velocity = 0;                  // paid / (held · span)
  double holding = 0;                   // 1 / velocity, the mean time a unit is held (Little)
};

// `holdings`: samples of money held by each agent within the window (each a vector over agents); `group[a]` the group of
// agent a (none to leave it out); the result per group.
inline std::vector<Velocity> velocity(const std::vector<Payment>& P, const std::vector<std::vector<double>>& holdings, const std::vector<std::size_t>& group,
                                      std::size_t groups, double t0, double t1) {
  std::vector<Velocity> V(groups);
  for (auto& v : V) v.span = t1 - t0;
  for (const auto& p : P) {
    if (p.time < t0 || p.time >= t1 || p.created || p.destroyed) continue;
    const std::size_t g = group.at(p.from);
    if (g != none) V[g].paid += p.amount;
  }
  if (!holdings.empty())
    for (const auto& h : holdings)
      for (std::size_t a = 0; a < h.size(); a++)
        if (group.at(a) != none) V[group[a]].held += h[a] / static_cast<double>(holdings.size());
  for (auto& v : V)
    if (v.held > 0 && v.span > 0) {
      v.velocity = v.paid / (v.held * v.span);
      v.holding = v.velocity > 0 ? 1 / v.velocity : 0;
    }
  return V;
}

// ---- circulation ----------------------------------------------------------------------------------------------------

struct Circulating {
  double total = 0;              // the value paid between positions
  double driven = 0, circulating = 0;  // the Hodge decomposition's shares of the flow's energy (round faces and holes together)
  double in_strong = 0;          // the share of value paid within strongly connected components
  std::size_t components = 0;    // how many strongly connected components the payments form
};

inline Circulating circulation(const std::map<std::pair<std::size_t, std::size_t>, double>& F, std::size_t positions) {
  Circulating C;
  if (F.empty()) return C;
  economics::TransferNetwork N;
  for (std::size_t i = 0; i < positions; i++) N.positions.push_back(std::to_string(i));
  std::vector<tyche::networks::Tie> ties;
  std::vector<double> w;
  for (const auto& [ij, v] : F) {
    if (!(v > 0)) continue;
    economics::Leg l;
    l.from = ij.first, l.to = ij.second, l.flow = v, l.kind = "money";
    N.legs.push_back(l);
    ties.push_back({ij.first, ij.second});
    w.push_back(v);
    C.total += v;
  }
  if (N.legs.empty()) return C;
  const auto H = economics::circulation(N);
  C.driven = H.driven;
  C.circulating = H.local + H.global;
  const auto g = tyche::networks::network(positions, ties, true, w);
  const auto S = tyche::networks::strong_components(g);
  C.components = S.count();
  double within = 0;
  for (const auto& [ij, v] : F)
    if (v > 0 && S.label[ij.first] == S.label[ij.second]) within += v;
  C.in_strong = C.total > 0 ? within / C.total : 0;
  return C;
}

// ---- concentration ------------------------------------------------------------------------------------------------------

struct Concentration {
  double held = 0, paid = 0;  // Gini coefficients of money held and of money paid, over the agents counted
};

inline Concentration concentration(const std::vector<double>& held, const std::vector<double>& paid) {
  Concentration c;
  double h = 0, p = 0;
  for (double x : held) h += x;
  for (double x : paid) p += x;
  if (h > 0) c.held = nomos::measurement::gini(held);
  if (p > 0) c.paid = nomos::measurement::gini(paid);
  return c;
}

// The net lending of each sector since opening (income less expenditure); they sum to zero over the sectors, outside
// included, as every flow has a payer and a payee.
inline std::map<Sector, Amount> net_lending(const Ledger& L) {
  std::map<Sector, Amount> s;
  for (std::size_t a = 0; a < L.agents().size(); a++) s[L.agents()[a].sector] += L.saving(a);
  return s;
}

}  // namespace economics::ledger
