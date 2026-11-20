// Balance sheets and postings: an economy's accounts kept so that nothing is made or lost unseen (ADR 0139).
//
// Every agent keeps its own books, assets and liabilities, each position an instrument with its counterparty: a deposit
// is an asset of its holder against the bank and a liability of the bank to the holder. A transaction is a posting, and
// a posting is the only way a position changes (Caiani et al. 2016, "Agent based-stock flow consistent macroeconomics:
// towards a benchmark model", J. Econ. Dyn. Control 69, 375–408, their quadruple entry: each transaction recorded on both
// sides of both parties' balance sheets). Amounts are whole numbers of a declared minor unit (cents), so that every
// identity below holds exactly over any number of postings.
//
// A posting is checked before it is applied, and refused, naming it, when
//   - the books disagree: for some instrument, holder and issuer, the holder's asset moves by a different amount from the
//     issuer's liability (so that net financial assets would no longer sum to zero over all agents);
//   - its flows disagree with its entries: each agent's change in net financial worth must equal the income it receives
//     less the expenditure it makes in the posting's flows (wages, purchases, taxes, transfers, grants), so that each row
//     of the transaction-flow matrix balances and each column sums to zero by construction (Godley and Lavoie 2007,
//     Monetary Economics, ch. 2);
//   - money changes otherwise than by a named act: the money stock grows only by lending or issuing and shrinks only by
//     repaying or redeeming, each by the issuer of the money made or unmade; a transfer or an exchange leaves the money
//     stock as it was (a payment between two banks' customers moves money from one bank's liabilities to the other's);
//   - a position would become negative (an overdraft is a loan, and must be posted as one): this is a shortage of
//     liquidity, not a fault, and a caller may queue the posting instead (try_post).
// Opening balances are posted by the act `open` before anything else; they may create money and carry no flows, and
// the audit takes the net worth they leave as each agent's starting point.
//
// One agent, created first, is "outside": everything beyond the economy modelled, so that money leaving is a payment to
// it, a flow, never a disappearance.
//
// The audit recomputes from the books what the postings promised: each instrument held equals each issued; net
// financial assets sum to zero; each agent's change in net worth since opening equals its income less its expenditure;
// each money stock equals what was opened plus what was created less what was destroyed, act by act. It names the first
// failure.
//
// Real stocks (goods, capital, inventories) and their revaluations are not kept here: goods bought are flows of
// expenditure and income, so that financial net worth changes only by saving.
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace economics::ledger {

using Amount = std::int64_t;
constexpr std::size_t none = std::numeric_limits<std::size_t>::max();

struct Refusal : std::runtime_error {
  using std::runtime_error::runtime_error;
};

enum class Sector { outside, household, firm, bank, government, central_bank, mint, other };

inline const char* name(Sector s) {
  switch (s) {
    case Sector::outside: return "outside";
    case Sector::household: return "household";
    case Sector::firm: return "firm";
    case Sector::bank: return "bank";
    case Sector::government: return "government";
    case Sector::central_bank: return "central bank";
    case Sector::mint: return "mint";
    default: return "other";
  }
}

inline Sector sector_of(const std::string& s) {
  for (Sector k : {Sector::outside, Sector::household, Sector::firm, Sector::bank, Sector::government, Sector::central_bank, Sector::mint, Sector::other})
    if (s == name(k)) return k;
  throw Refusal("there is no sector '" + s + "' (there are outside, household, firm, bank, government, central bank, mint and other)");
}

struct Agent {
  std::string name;
  Sector sector = Sector::other;
  std::string region;  // where it is, for readings of retention (free text; empty: nowhere in particular)
};

struct Instrument {
  std::string name;    // deposit, currency, loan, reserve, voucher, …
  bool money = false;  // a means of payment, counted in the money stock
};

enum class Side { asset, liability };

// One line of a posting: `agent`'s asset (it holds `instrument` against `counterparty`, the issuer) or liability (it has
// issued `instrument` to `counterparty`, the holder) moves by `delta`.
struct Entry {
  std::size_t agent = 0;
  Side side = Side::asset;
  std::size_t instrument = 0;
  std::size_t counterparty = 0;
  Amount delta = 0;
};

// A flow of income and expenditure: `from` spends and `to` receives `amount` (kind: wage, purchase, tax, transfer, …).
struct Flow {
  std::size_t from = 0, to = 0;
  Amount amount = 0;
  std::string kind;
};

enum class Act { open, transfer, exchange, lend, repay, issue, redeem };

inline const char* name(Act a) {
  switch (a) {
    case Act::open: return "open";
    case Act::transfer: return "transfer";
    case Act::exchange: return "exchange";
    case Act::lend: return "lend";
    case Act::repay: return "repay";
    case Act::issue: return "issue";
    default: return "redeem";
  }
}
inline bool creates(Act a) { return a == Act::lend || a == Act::issue; }
inline bool destroys(Act a) { return a == Act::repay || a == Act::redeem; }

struct Posting {
  double time = 0;
  Act act = Act::transfer;
  std::size_t responsible = 0;
  std::string note;
  std::vector<Entry> entries;
  std::vector<Flow> flows;
  // A label: money received through this posting begins a trace of its own, for the readings of where money goes
  // after it entered (a grant to a place, say). Empty: the money carries the traces it had.
  std::string origin;
};

// The change in money held (the sum of money instruments on the asset side) of each agent a posting touches.
struct MoneyChange {
  std::size_t agent = 0;
  Amount delta = 0;
};

struct Verdict {
  bool holds = true;
  std::string failure;  // empty when it holds
};

class Ledger {
 public:
  // An observer is told of each posting applied, its index, and the change in money held of each agent it touched.
  using Observer = std::function<void(const Posting&, std::size_t, const std::vector<MoneyChange>&)>;

  Ledger() { agents_.push_back({"outside", Sector::outside, ""}); books_.emplace_back(); income_.push_back(0); opened_.push_back(0); }

  std::size_t outside() const { return 0; }
  std::size_t add_agent(Agent a) {
    if (posted_ > 0) throw Refusal("agents are added before anything is posted");
    agents_.push_back(std::move(a));
    books_.emplace_back();
    income_.push_back(0);
    opened_.push_back(0);
    return agents_.size() - 1;
  }
  std::size_t add_instrument(Instrument i) {
    if (posted_ > 0) throw Refusal("instruments are declared before anything is posted");
    for (const auto& j : instruments_)
      if (j.name == i.name) throw Refusal("the instrument '" + i.name + "' is declared twice");
    instruments_.push_back(std::move(i));
    return instruments_.size() - 1;
  }
  std::size_t instrument(const std::string& n) const {
    for (std::size_t k = 0; k < instruments_.size(); k++)
      if (instruments_[k].name == n) return k;
    throw Refusal("there is no instrument '" + n + "'");
  }
  const std::vector<Agent>& agents() const { return agents_; }
  const std::vector<Instrument>& instruments() const { return instruments_; }
  std::size_t postings() const { return posted_; }
  void observe(Observer o) { observers_.push_back(std::move(o)); }

  // ---- the books ----------------------------------------------------------------------------------------------------
  Amount held(std::size_t holder, std::size_t instrument, std::size_t issuer) const { return get(books_.at(holder).assets, instrument, issuer); }
  Amount owed(std::size_t issuer, std::size_t instrument, std::size_t holder) const { return get(books_.at(issuer).liabilities, instrument, holder); }
  // The money an agent holds: every money instrument on its asset side.
  Amount money(std::size_t agent) const {
    Amount m = 0;
    for (const auto& [key, v] : books_.at(agent).assets)
      if (instruments_[key.first].money) m += v;
    return m;
  }
  // Each money instrument an agent holds, with its issuer, largest first (what it pays with).
  std::vector<std::pair<std::pair<std::size_t, std::size_t>, Amount>> means(std::size_t agent) const {
    std::vector<std::pair<std::pair<std::size_t, std::size_t>, Amount>> out;
    for (const auto& [key, v] : books_.at(agent).assets)
      if (instruments_[key.first].money && v > 0) out.push_back({key, v});
    std::stable_sort(out.begin(), out.end(), [](const auto& a, const auto& b) { return a.second > b.second; });
    return out;
  }
  // The money stock: every money instrument issued, as its issuers' books say.
  Amount money_stock() const {
    Amount m = 0;
    for (const auto& b : books_)
      for (const auto& [key, v] : b.liabilities)
        if (instruments_[key.first].money) m += v;
    return m;
  }
  Amount net_worth(std::size_t agent) const {
    Amount w = 0;
    for (const auto& [key, v] : books_.at(agent).assets) w += v;
    for (const auto& [key, v] : books_.at(agent).liabilities) w -= v;
    return w;
  }
  // Income less expenditure since opening, from the postings' flows.
  Amount saving(std::size_t agent) const { return income_.at(agent); }
  Amount created() const { return created_; }
  Amount destroyed() const { return destroyed_; }
  // Money created and destroyed by each act.
  const std::map<Act, Amount>& by_act() const { return by_act_; }

  // ---- checking -----------------------------------------------------------------------------------------------------
  // What is wrong with a posting's form (empty when nothing is): the books, the flows, money, the act.
  std::optional<std::string> fault(const Posting& p) const {
    const std::size_t n = agents_.size(), m = instruments_.size();
    if (p.entries.empty()) return "it has no entries";
    if (p.act == Act::open && opened_done_) return "opening balances are posted before anything else";
    if (p.act != Act::open && p.responsible >= n) return "the agent responsible is not in the economy";
    std::map<std::tuple<std::size_t, std::size_t, std::size_t>, std::pair<Amount, Amount>> pair;  // (instrument, issuer, holder) → (asset, liability)
    std::vector<Amount> worth(n, 0);
    for (const auto& e : p.entries) {
      if (e.agent >= n || e.counterparty >= n) return "an entry names an agent not in the economy";
      if (e.instrument >= m) return "an entry names an instrument not declared";
      if (e.agent == e.counterparty) return agents_[e.agent].name + " holds a claim on itself";
      if (e.delta == 0) return "an entry moves nothing";
      if (e.side == Side::asset) {
        pair[{e.instrument, e.counterparty, e.agent}].first += e.delta;
        worth[e.agent] += e.delta;
      } else {
        pair[{e.instrument, e.agent, e.counterparty}].second += e.delta;
        worth[e.agent] -= e.delta;
      }
    }
    for (const auto& [key, v] : pair)
      if (v.first != v.second) {
        const auto& [k, issuer, holder] = key;
        return "the books disagree: " + agents_[holder].name + "'s " + instruments_[k].name + " at " + agents_[issuer].name + " moves by " +
               std::to_string(v.first) + " but " + agents_[issuer].name + "'s liability to " + agents_[holder].name + " by " + std::to_string(v.second);
      }
    std::vector<Amount> flow(n, 0);
    for (const auto& f : p.flows) {
      if (f.from >= n || f.to >= n) return "a flow names an agent not in the economy";
      if (f.from == f.to) return "a flow from " + agents_[f.from].name + " to itself";
      if (f.amount <= 0) return "a flow of nothing or less";
      flow[f.from] -= f.amount;
      flow[f.to] += f.amount;
    }
    if (p.act != Act::open) {
      if (!p.flows.empty() && p.act != Act::transfer && p.act != Act::issue && p.act != Act::redeem)
        return std::string("a ") + name(p.act) + " is a change of claims, and carries no flows of income";
      for (std::size_t a = 0; a < n; a++)
        if (worth[a] != flow[a])
          return agents_[a].name + "'s net worth changes by " + std::to_string(worth[a]) + " but its flows say " + std::to_string(flow[a]);
    } else if (!p.flows.empty()) {
      return "opening balances carry no flows";
    }
    // Money: the change in each issuer's money stock.
    std::map<std::size_t, Amount> stock;
    for (const auto& [key, v] : pair) {
      const auto& [k, issuer, holder] = key;
      if (instruments_[k].money && v.second != 0) stock[issuer] += v.second;
    }
    Amount total = 0;
    for (const auto& [issuer, d] : stock) total += d;
    if (p.act != Act::open) {
      if (!creates(p.act) && !destroys(p.act) && total != 0)
        return std::string("money changes by a ") + name(p.act) + " (by " + std::to_string(total) + "): only lending, issuing, repaying and redeeming change it";
      if (creates(p.act) && !(total > 0)) return std::string("a ") + name(p.act) + " that creates no money";
      if (destroys(p.act) && !(total < 0)) return std::string("a ") + name(p.act) + " that destroys no money";
      if (creates(p.act) || destroys(p.act))
        for (const auto& [issuer, d] : stock)
          if (d != 0 && issuer != p.responsible)
            return "money of " + agents_[issuer].name + " changes by an act of " + agents_[p.responsible].name + ": only an issuer makes or unmakes its money";
    }
    return std::nullopt;
  }
  // Which position a posting would leave negative (empty when none would).
  std::optional<std::string> short_of(const Posting& p) const {
    std::map<std::tuple<std::size_t, int, std::size_t, std::size_t>, Amount> after;
    for (const auto& e : p.entries) {
      const auto key = std::make_tuple(e.agent, e.side == Side::asset ? 0 : 1, e.instrument, e.counterparty);
      auto it = after.find(key);
      if (it == after.end()) {
        const Amount now = e.side == Side::asset ? held(e.agent, e.instrument, e.counterparty) : owed(e.agent, e.instrument, e.counterparty);
        it = after.emplace(key, now).first;
      }
      it->second += e.delta;
    }
    // The holder's side first: a shortage is the payer's.
    for (int want = 0; want < 2; want++)
      for (const auto& [key, v] : after) {
        const auto& [a, side, k, cp] = key;
        if (side == want && v < 0)
          return agents_[a].name + "'s " + instruments_[k].name + (side == 0 ? " at " : " owed to ") + agents_[cp].name + " would be " + std::to_string(v);
      }
    return std::nullopt;
  }
  // Applies a posting, or refuses it naming it: a fault or a shortage.
  void post(const Posting& p) {
    if (auto f = fault(p)) throw Refusal(label(p) + ": " + *f);
    if (auto s = short_of(p)) throw Refusal(label(p) + ": short, " + *s);
    apply(p);
  }
  // Applies a posting if it can be met; false, changing nothing, when it is short. A fault is still refused.
  bool try_post(const Posting& p) {
    if (auto f = fault(p)) throw Refusal(label(p) + ": " + *f);
    if (short_of(p)) return false;
    apply(p);
    return true;
  }

  // ---- the audit ----------------------------------------------------------------------------------------------------
  Verdict audit() const {
    const std::size_t n = agents_.size();
    std::map<std::tuple<std::size_t, std::size_t, std::size_t>, std::pair<Amount, Amount>> pos;
    for (std::size_t a = 0; a < n; a++) {
      for (const auto& [key, v] : books_[a].assets) pos[{key.first, key.second, a}].first += v;
      for (const auto& [key, v] : books_[a].liabilities) pos[{key.first, a, key.second}].second += v;
    }
    for (const auto& [key, v] : pos)
      if (v.first != v.second) {
        const auto& [k, issuer, holder] = key;
        return {false, instruments_[k].name + " held by " + agents_[holder].name + " (" + std::to_string(v.first) + ") is not what " + agents_[issuer].name + " owes it (" +
                           std::to_string(v.second) + ")"};
      }
    Amount total = 0;
    for (std::size_t a = 0; a < n; a++) total += net_worth(a);
    if (total != 0) return {false, "net financial assets sum to " + std::to_string(total) + ", not zero"};
    for (std::size_t a = 0; a < n; a++)
      if (net_worth(a) - opened_[a] != income_[a])
        return {false, agents_[a].name + "'s net worth moved by " + std::to_string(net_worth(a) - opened_[a]) + " since opening, its income less expenditure by " + std::to_string(income_[a])};
    Amount flows = 0;
    for (std::size_t a = 0; a < n; a++) flows += income_[a];
    if (flows != 0) return {false, "income less expenditure sums to " + std::to_string(flows) + " over the agents, not zero"};
    if (money_stock() != opened_money_ + created_ - destroyed_)
      return {false, "the money stock is " + std::to_string(money_stock()) + ", but opened " + std::to_string(opened_money_) + " + created " + std::to_string(created_) +
                         " − destroyed " + std::to_string(destroyed_) + " = " + std::to_string(opened_money_ + created_ - destroyed_)};
    for (std::size_t a = 0; a < n; a++) {
      for (const auto& [key, v] : books_[a].assets)
        if (v < 0) return {false, agents_[a].name + " holds a negative " + instruments_[key.first].name};
      for (const auto& [key, v] : books_[a].liabilities)
        if (v < 0) return {false, agents_[a].name + " owes a negative " + instruments_[key.first].name};
    }
    return {};
  }

 private:
  using Book = std::map<std::pair<std::size_t, std::size_t>, Amount>;  // (instrument, counterparty) → amount
  struct Books {
    Book assets, liabilities;
  };
  static Amount get(const Book& b, std::size_t k, std::size_t cp) {
    const auto it = b.find({k, cp});
    return it == b.end() ? 0 : it->second;
  }
  std::string label(const Posting& p) const {
    std::string who = p.responsible < agents_.size() ? agents_[p.responsible].name : "?";
    return "posting " + std::to_string(posted_ + 1) + " (" + name(p.act) + " by " + who + " at " + std::to_string(p.time) + (p.note.empty() ? "" : ", " + p.note) + ")";
  }
  void apply(const Posting& p) {
    std::map<std::size_t, Amount> money_change;
    Amount stock_change = 0;
    for (const auto& e : p.entries) {
      Book& b = e.side == Side::asset ? books_[e.agent].assets : books_[e.agent].liabilities;
      Amount& v = b[{e.instrument, e.counterparty}];
      v += e.delta;
      if (v == 0) b.erase({e.instrument, e.counterparty});
      if (instruments_[e.instrument].money) {
        if (e.side == Side::asset) money_change[e.agent] += e.delta;
        else stock_change += e.delta;
      }
    }
    if (p.act == Act::open) {
      opened_money_ += stock_change;
      for (const auto& e : p.entries) opened_[e.agent] += e.side == Side::asset ? e.delta : -e.delta;
    } else {
      opened_done_ = true;
      for (const auto& f : p.flows) income_[f.from] -= f.amount, income_[f.to] += f.amount;
      if (stock_change > 0) created_ += stock_change, by_act_[p.act] += stock_change;
      if (stock_change < 0) destroyed_ -= stock_change, by_act_[p.act] += stock_change;
    }
    posted_++;
    if (!observers_.empty()) {
      std::vector<MoneyChange> changes;
      for (const auto& [a, d] : money_change)
        if (d != 0) changes.push_back({a, d});
      for (const auto& o : observers_) o(p, posted_ - 1, changes);
    }
  }

  std::vector<Agent> agents_;
  std::vector<Instrument> instruments_;
  std::vector<Books> books_;
  std::vector<Amount> income_, opened_;
  Amount opened_money_ = 0, created_ = 0, destroyed_ = 0;
  std::map<Act, Amount> by_act_;
  std::size_t posted_ = 0;
  bool opened_done_ = false;
  std::vector<Observer> observers_;
};

// ---- postings of the common acts --------------------------------------------------------------------------------------

// Opening balances: `holder` holds `amount` of `instrument` issued by `issuer`.
inline Posting opening(std::size_t holder, std::size_t instrument, std::size_t issuer, Amount amount) {
  Posting p;
  p.act = Act::open;
  p.responsible = issuer;
  p.note = "opening";
  p.entries = {{holder, Side::asset, instrument, issuer, amount}, {issuer, Side::liability, instrument, holder, amount}};
  return p;
}

// A payment of `amount` of `instrument` issued by `issuer`, from `from` to `to`, for a flow of `kind`. When the payer is
// the issuer it issues new money (a government's spending in its own money); when the payee is the issuer the money is
// redeemed (taxes paid in it); otherwise it is a transfer.
inline Posting pay(double time, std::size_t from, std::size_t to, std::size_t instrument, std::size_t issuer, Amount amount, std::string kind,
                   std::string origin = {}) {
  Posting p;
  p.time = time;
  p.flows = {{from, to, amount, std::move(kind)}};
  p.origin = std::move(origin);
  if (from == issuer) {
    p.act = Act::issue;
    p.responsible = issuer;
    p.entries = {{to, Side::asset, instrument, issuer, amount}, {issuer, Side::liability, instrument, to, amount}};
  } else if (to == issuer) {
    p.act = Act::redeem;
    p.responsible = issuer;
    p.entries = {{from, Side::asset, instrument, issuer, -amount}, {issuer, Side::liability, instrument, from, -amount}};
  } else {
    p.act = Act::transfer;
    p.responsible = from;
    p.entries = {{from, Side::asset, instrument, issuer, -amount},
                 {issuer, Side::liability, instrument, from, -amount},
                 {to, Side::asset, instrument, issuer, amount},
                 {issuer, Side::liability, instrument, to, amount}};
  }
  return p;
}

// A payment between customers of two banks, settled in reserves at the central bank: the payer's deposit at its bank
// falls, its bank's reserves pass to the payee's bank, and the payee's deposit there rises (eight entries).
inline Posting pay_across(double time, std::size_t from, std::size_t bank_from, std::size_t to, std::size_t bank_to, std::size_t deposit, std::size_t reserve,
                          std::size_t central, Amount amount, std::string kind, std::string origin = {}) {
  if (bank_from == bank_to) return pay(time, from, to, deposit, bank_from, amount, std::move(kind), std::move(origin));
  Posting p;
  p.time = time;
  p.act = Act::transfer;
  p.responsible = from;
  p.flows = {{from, to, amount, std::move(kind)}};
  p.origin = std::move(origin);
  p.entries = {{from, Side::asset, deposit, bank_from, -amount},    {bank_from, Side::liability, deposit, from, -amount},
               {bank_from, Side::asset, reserve, central, -amount}, {central, Side::liability, reserve, bank_from, -amount},
               {bank_to, Side::asset, reserve, central, amount},    {central, Side::liability, reserve, bank_to, amount},
               {to, Side::asset, deposit, bank_to, amount},         {bank_to, Side::liability, deposit, to, amount}};
  return p;
}

// A bank lends: a loan on its books and a deposit in the borrower's, both new; money is created.
inline Posting lend(double time, std::size_t bank, std::size_t borrower, std::size_t loan, std::size_t deposit, Amount amount) {
  Posting p;
  p.time = time;
  p.act = Act::lend;
  p.responsible = bank;
  p.entries = {{bank, Side::asset, loan, borrower, amount},
               {borrower, Side::liability, loan, bank, amount},
               {borrower, Side::asset, deposit, bank, amount},
               {bank, Side::liability, deposit, borrower, amount}};
  return p;
}

// A borrower repays from its deposit at the bank: the loan and the deposit both fall; money is destroyed.
inline Posting repay(double time, std::size_t bank, std::size_t borrower, std::size_t loan, std::size_t deposit, Amount amount) {
  Posting p = lend(time, bank, borrower, loan, deposit, amount);
  p.act = Act::repay;
  for (auto& e : p.entries) e.delta = -e.delta;
  return p;
}

}  // namespace economics::ledger
