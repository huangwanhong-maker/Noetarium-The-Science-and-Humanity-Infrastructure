// An economy of agents whose every payment is posted in the accounts (ADR 0139).
//
// Agents act period by period. Each agent's behaviour is a nomos behaviour (rule, learned or proposed, kept apart and
// tallied), shown what it declared it observes: the money it holds, what it received in the last period, what it owes
// in its queue, the period. Its acts are payments, the act's name the kind of flow (purchase, wage, import, tax, …),
// directed to an agent. Amounts are made whole units by the largest remainder, so that an agent paying out what it
// received pays exactly that.
//
// Timing and liquidity. Within a period the agents act in a declared order, or in a fresh random order each period by
// nomos's keyed stream (random sequential updating); grants arrive at their declared instants among them. A payment the
// payer cannot meet waits in its queue, first in, first out, and is tried again whenever the payer receives money and
// at each period's start (Galbiati and Soramäki 2011, "An agent-based model of payment systems", J. Econ. Dyn. Control
// 35, 859–875, after real-time gross settlement). The queue's waits and what is left unpaid at the end are reported.
// Without timing, the order in which an allocation reaches agents could not matter; with it, money given first is spent
// before the next grant arrives, and relieves or deepens others' queues.
//
// Grants are payments labelled with an origin, so that the readings follow that money (readings.hpp). An issuer pays by
// issuing (a government spending its own money creates it), and money paid to its issuer is redeemed.
//
// Two economies are built here as checks of the whole:
//   steady_state   from a social accounting matrix of payments per period between sectors (rows the payers, each
//                  sector's row sum equal to its column sum): each agent spends what it received in the last period in
//                  the matrix's shares, and holds a declared number of periods of its outflow; the matrix is then
//                  reproduced every period (Pichler et al. 2022 and Poledna et al. 2023 initialise agent economies from
//                  input–output accounts so);
//   RandomExchange Drăgulescu and Yakovenko's random exchange of money (2000, "Statistical mechanics of money", Eur.
//                  Phys. J. B 17, 723–729): each agent holding money pays one unit, at rate 1, to an agent drawn
//                  uniformly among the others. It is a zero-range process with constant rate, whose stationary law is
//                  uniform over the configurations of the money among the agents (Evans and Hanney 2005, "Nonequilibrium
//                  statistical mechanics of the zero-range process and related models", J. Phys. A 38, R195, §2), so
//                  that one agent's money m has the exact law C(M − m + N − 2, N − 2) / C(M + N − 1, N − 1), tending to
//                  the exponential of temperature M/N, Boltzmann and Gibbs's law the authors found.
#pragma once

#include <economics/ledger/accounts.hpp>
#include <economics/ledger/readings.hpp>
#include <nomos/agents/behaviour.hpp>
#include <nomos/agents/engine.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

namespace economics::ledger {

struct Grant {
  double time = 0;
  std::size_t from = 0, to = 0;  // `from` none: the economy's issuer, issuing
  Amount amount = 0;
  std::string origin;  // the trace it starts
  std::string kind = "grant";
};

struct Outcome {
  Verdict audit;                    // the accounts' audit at the end
  std::size_t postings = 0;
  std::size_t queued = 0;           // payments that had to wait
  std::size_t unsettled = 0;        // still waiting at the end
  Amount unsettled_amount = 0;
  double delay = 0;                 // the mean wait of those that waited and were paid, in periods
  nomos::agents::Tally tally;       // decisions by origin of behaviour
};

class Economy {
 public:
  Ledger L;
  Recorder recorder;
  Tracer tracer;
  std::size_t money = 0;   // the instrument paid with
  std::size_t issuer = 0;  // its issuer
  std::vector<nomos::agents::Behaviour> behaviour;  // per agent; one without a way to decide does not act
  std::vector<std::size_t> order;                    // who acts in a period, in this order (empty: every agent that can, by index)
  bool shuffle = false;                              // a fresh random order each period
  std::uint64_t seed = 1;
  std::vector<Grant> grants;
  std::vector<double> received_before;               // what each agent is taken to have received before the first period
  std::vector<std::vector<double>> holdings;         // money held by each agent at each period's end
  std::size_t period = 0;                            // periods run so far

  explicit Economy(std::size_t rounds = 12) : tracer(rounds) {}

  // After the agents and instruments are declared and before anything is posted.
  void start() {
    const std::size_t n = L.agents().size();
    behaviour.resize(n);
    queue_.assign(n, {});
    received_now_.assign(n, 0);
    received_last_.assign(n, 0);
    recorder.attach(L);
    tracer.attach(L);
    L.observe([this](const Posting& p, std::size_t, const std::vector<MoneyChange>& c) {
      if (p.act == Act::open) return;
      for (const auto& m : c)
        if (m.delta > 0) {
          received_now_[m.agent] += static_cast<double>(m.delta);
          if (!queue_[m.agent].empty()) wake_.push_back(m.agent);
        }
    });
    started_ = true;
  }

  Outcome run(std::size_t periods) {
    if (!started_) throw Refusal("an economy is started before it runs");
    const std::size_t n = L.agents().size();
    if (period == 0 && received_before.size() == n) received_now_ = received_before;
    std::vector<std::size_t> who = order;
    if (who.empty())
      for (std::size_t a = 0; a < n; a++)
        if (behaviour[a].decide) who.push_back(a);
    for (std::size_t t = 0; t < periods; t++, period++) {
      const double T = static_cast<double>(period);
      received_last_ = received_now_;
      std::fill(received_now_.begin(), received_now_.end(), 0.0);
      for (std::size_t a = 0; a < n; a++) drain(a, T);
      std::vector<std::size_t> turn = who;
      if (shuffle) {
        calx::random::Stream ord(seed, nomos::agents::order_stream, static_cast<std::uint32_t>(period), 0);
        for (std::size_t k = turn.size(); k > 1; k--) std::swap(turn[k - 1], turn[static_cast<std::size_t>(ord.below(k))]);
      }
      // Events of the period: grants at their instants, the agents' turns spread evenly after the period's start.
      struct Ev {
        double time;
        int kind;  // 0 grant, 1 turn
        std::size_t index;
      };
      std::vector<Ev> ev;
      for (std::size_t g = 0; g < grants.size(); g++)
        if (grants[g].time >= T && grants[g].time < T + 1) ev.push_back({grants[g].time, 0, g});
      for (std::size_t k = 0; k < turn.size(); k++) ev.push_back({T + static_cast<double>(k + 1) / static_cast<double>(turn.size() + 1), 1, turn[k]});
      std::stable_sort(ev.begin(), ev.end(), [](const Ev& a, const Ev& b) { return a.time < b.time || (a.time == b.time && a.kind < b.kind); });
      for (const auto& e : ev) {
        if (e.kind == 0) {
          const auto& g = grants[e.index];
          const std::size_t from = g.from == none ? issuer : g.from;
          submit({e.time, from, g.to, g.amount, g.kind, g.origin}, e.time);
        } else {
          act(e.index, e.time);
        }
      }
      std::vector<double> h(n);
      for (std::size_t a = 0; a < n; a++) h[a] = static_cast<double>(L.money(a));
      holdings.push_back(std::move(h));
    }
    Outcome o = outcome_;
    o.audit = L.audit();
    o.postings = L.postings();
    for (const auto& q : queue_) {
      o.unsettled += q.size();
      for (const auto& p : q) o.unsettled_amount += p.amount;
    }
    o.delay = late_ > 0 ? wait_ / static_cast<double>(late_) : 0;
    return o;
  }

  // What an agent owes in its queue.
  Amount owed(std::size_t a) const {
    Amount s = 0;
    for (const auto& p : queue_.at(a)) s += p.amount;
    return s;
  }
  double received_last(std::size_t a) const { return received_last_.at(a); }

 private:
  struct Pending {
    double submitted;
    std::size_t from, to;
    Amount amount;
    std::string kind, origin;
  };

  void act(std::size_t a, double time) {
    nomos::agents::Observation o{{"held", static_cast<double>(L.money(a))},
                                 {"received", received_last_[a]},
                                 {"owed", static_cast<double>(owed(a))},
                                 {"period", static_cast<double>(period)}};
    calx::random::Stream rng(seed, nomos::agents::agent_stream, static_cast<std::uint32_t>(a), static_cast<std::uint32_t>(period));
    const auto acts = nomos::agents::ask(behaviour[a], o, rng, &outcome_.tally);
    // Whole units by the largest remainder: the floor of the total, the remainders to the largest fractions.
    double total = 0;
    for (const auto& x : acts) total += std::max(0.0, x.amount);
    const Amount whole = static_cast<Amount>(std::floor(total + 1e-9));
    std::vector<Amount> amt(acts.size());
    std::vector<std::pair<double, std::size_t>> frac;
    Amount used = 0;
    for (std::size_t k = 0; k < acts.size(); k++) {
      const double v = std::max(0.0, acts[k].amount);
      amt[k] = static_cast<Amount>(std::floor(v));
      used += amt[k];
      frac.push_back({v - std::floor(v), k});
    }
    std::stable_sort(frac.begin(), frac.end(), [](const auto& x, const auto& y) { return x.first > y.first; });
    for (std::size_t k = 0; used < whole && k < frac.size(); k++, used++) amt[frac[k].second]++;
    for (std::size_t k = 0; k < acts.size(); k++)
      if (amt[k] > 0) {
        if (acts[k].target >= L.agents().size()) throw Refusal("the behaviour of " + L.agents()[a].name + " pays an agent not in the economy");
        submit({time, a, acts[k].target, amt[k], acts[k].act, {}}, time);
      }
  }

  Posting posting(const Pending& p, double time) const {
    return pay(time, p.from, p.to, money, issuer, p.amount, p.kind.empty() ? std::string("payment") : p.kind, p.origin);
  }

  // A payment: queued behind what the payer already owes, else paid if it can be, else queued.
  void submit(Pending p, double time) {
    if (!queue_[p.from].empty() || !L.try_post(posting(p, time))) {
      queue_[p.from].push_back(std::move(p));
      outcome_.queued++;
      return;
    }
    settle_woken(time);
  }
  void drain(std::size_t a, double time) {
    while (!queue_[a].empty() && L.try_post(posting(queue_[a].front(), time))) {
      wait_ += time - queue_[a].front().submitted;
      late_++;
      queue_[a].pop_front();
    }
    settle_woken(time);
  }
  void settle_woken(double time) {
    while (!wake_.empty()) {
      const std::size_t a = wake_.back();
      wake_.pop_back();
      while (!queue_[a].empty() && L.try_post(posting(queue_[a].front(), time))) {
        wait_ += time - queue_[a].front().submitted;
        late_++;
        queue_[a].pop_front();
      }
    }
  }

  bool started_ = false;
  std::vector<std::deque<Pending>> queue_;
  std::vector<double> received_now_, received_last_;
  std::vector<std::size_t> wake_;
  Outcome outcome_;
  double wait_ = 0;
  std::size_t late_ = 0;
};

// ---- an economy in its steady state, from accounts --------------------------------------------------------------------

// Payments per period between sectors: flows[i][j] paid by sector i to sector j.
struct Accounts {
  std::vector<std::string> sectors;
  std::vector<Sector> kinds;               // what each sector is (one may be Sector::outside, the economy's outside agent)
  std::vector<std::vector<Amount>> flows;
  std::vector<std::size_t> members;        // agents in each sector (outside: 1)
  std::vector<std::string> regions;        // each sector's region (optional)
};

struct SteadyState {
  std::vector<std::vector<std::size_t>> agents;  // the agents of each sector
};

// Builds the economy: agents for each sector, a mint issuing the money, opening balances of `holding` periods of each
// agent's outflow, and each agent's rule: spend what was received in the last period, in the accounts' shares, equally
// among the payee sector's members. Refused unless every sector pays out what it receives.
inline SteadyState steady_state(Economy& E, const Accounts& A, int holding = 1, std::string kind = "payment") {
  const std::size_t S = A.sectors.size();
  if (A.flows.size() != S || A.kinds.size() != S || A.members.size() != S) throw Refusal("steady_state: one row, kind and count for each sector");
  for (std::size_t i = 0; i < S; i++) {
    if (A.flows[i].size() != S) throw Refusal("steady_state: the accounts are square");
    Amount out = 0, in = 0;
    for (std::size_t j = 0; j < S; j++) out += A.flows[i][j], in += A.flows[j][i];
    if (out != in) throw Refusal("steady_state: " + A.sectors[i] + " pays out " + std::to_string(out) + " a period and receives " + std::to_string(in) + ": not a steady state");
    if (A.members[i] == 0) throw Refusal("steady_state: " + A.sectors[i] + " has no members");
    if (A.kinds[i] == Sector::outside && A.members[i] != 1) throw Refusal("steady_state: outside is one agent");
  }
  SteadyState st;
  st.agents.resize(S);
  for (std::size_t i = 0; i < S; i++) {
    if (A.kinds[i] == Sector::outside) {
      st.agents[i] = {E.L.outside()};
      continue;
    }
    for (std::size_t k = 0; k < A.members[i]; k++)
      st.agents[i].push_back(E.L.add_agent({A.sectors[i] + " " + std::to_string(k + 1), A.kinds[i], A.regions.size() == S ? A.regions[i] : std::string()}));
  }
  E.issuer = E.L.add_agent({"mint", Sector::mint, ""});
  E.money = E.L.add_instrument({"currency", true});
  E.start();
  E.received_before.assign(E.L.agents().size(), 0);
  for (std::size_t i = 0; i < S; i++) {
    Amount out = 0;
    for (std::size_t j = 0; j < S; j++) out += A.flows[i][j];
    const double each = static_cast<double>(out) / static_cast<double>(A.members[i]);
    // The shares of each payee agent in what this sector's agent pays.
    std::vector<std::pair<std::size_t, double>> shares;
    if (out > 0)
      for (std::size_t j = 0; j < S; j++)
        for (const std::size_t b : st.agents[j])
          if (A.flows[i][j] > 0) shares.push_back({b, static_cast<double>(A.flows[i][j]) / static_cast<double>(out) / static_cast<double>(A.members[j])});
    for (const std::size_t a : st.agents[i]) {
      const Amount open = static_cast<Amount>(std::llround(each * holding));
      if (open > 0) E.L.post(opening(a, E.money, E.issuer, open));
      E.received_before[a] = each;
      nomos::agents::Behaviour b;
      b.declared = {"spend last period's receipts in the accounts' shares", nomos::agents::Origin::rule, {"received"}, {kind}, "the accounts (steady_state)"};
      b.decide = [shares, kind](const nomos::agents::Observation& o, calx::random::Stream&) {
        std::vector<nomos::agents::Action> acts;
        const double r = o.at("received");
        for (const auto& [b2, s] : shares) acts.push_back({kind, b2, r * s});
        return acts;
      };
      E.behaviour[a] = std::move(b);
    }
  }
  return st;
}

// ---- random exchange --------------------------------------------------------------------------------------------------

// nomos's JumpModel: agent i, holding money, pays one unit at rate 1 to another drawn uniformly; every exchange posted.
struct RandomExchange {
  struct State {
    Ledger* L = nullptr;
    std::vector<std::size_t> agents;  // the agents exchanging (ledger indices)
    std::size_t money = 0, issuer = 0;
    double time = 0;
  };
  std::size_t agents(const State& s) const { return s.agents.size(); }
  std::size_t rules() const { return 1; }
  double rate(const State& s, std::size_t i, std::size_t) const { return s.L->money(s.agents[i]) > 0 ? 1.0 : 0.0; }
  void fire(State& s, std::size_t i, std::size_t, calx::random::Stream& rng, std::vector<std::size_t>& affected) const {
    const std::size_t n = s.agents.size();
    std::size_t j = static_cast<std::size_t>(rng.below(n - 1));
    if (j >= i) j++;
    s.L->post(pay(s.time, s.agents[i], s.agents[j], s.money, s.issuer, 1, "exchange"));
    affected.push_back(i);
    affected.push_back(j);
  }
};

// The exact stationary law of one agent's money among N agents sharing M units under random exchange:
// P(m) = C(M − m + N − 2, N − 2) / C(M + N − 1, N − 1), m = 0 … M (computed in logarithms).
inline std::vector<double> exchange_law(std::size_t N, std::size_t M) {
  auto lchoose = [](double n, double k) { return std::lgamma(n + 1) - std::lgamma(k + 1) - std::lgamma(n - k + 1); };
  std::vector<double> p(M + 1);
  const double d = lchoose(static_cast<double>(M + N - 1), static_cast<double>(N - 1));
  for (std::size_t m = 0; m <= M; m++) p[m] = std::exp(lchoose(static_cast<double>(M - m + N - 2), static_cast<double>(N - 2)) - d);
  return p;
}

}  // namespace economics::ledger
