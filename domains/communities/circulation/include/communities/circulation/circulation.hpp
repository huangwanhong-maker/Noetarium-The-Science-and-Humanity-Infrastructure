// Circulation: gates of access, provision by need within a round, a market in tokens one unit at a time, and the mediation
// profile read from a chronicle.
//
// Definition 11 of the design record (2026-11-25): the access rule of a service is a list of conditions, each reading a quantity
// and perhaps spending it. A condition that reads a quantity the party accumulated and spends it is a personal purchasing-power
// gate, whatever the unit is called (a token, credit, reputation points); one that reads it as a threshold is a standing gate;
// the others are gates of membership, allocation by need, queue, rotation or lot, and open access.
//
// Provision by need hands out what was offered in a round so that the party least served so far receives next (water-filling
// over units, ties drawn at random), each unit from the provider of its kind with most left. The market lets buyers, in random
// order, buy one unit at a time from a seller of the kind with units left, at a price in tokens, refusing a buyer who holds too
// few: every such refusal rests on a balance.

#pragma once

#include <thesmos/chronicle/chronicle.hpp>

#include <calx/random/stream.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace communities::circulation {

// ---- Gates ------------------------------------------------------------------------------------------------------------------------

enum class Gate { purchasing_power, standing, membership, allocation, open };

inline std::string_view word(Gate g) {
  switch (g) {
    case Gate::purchasing_power: return "personal purchasing-power gate";
    case Gate::standing: return "standing gate";
    case Gate::membership: return "membership gate";
    case Gate::allocation: return "allocation";
    case Gate::open: return "open";
  }
  return "?";
}

struct Condition {
  std::string reads;   // the quantity read: "token", "credit", "balance", "reputation", "count of contributions", "membership",
                       // "position", "need", "queue", "rotation", "lot", "conduct", ...
  bool spent = false;  // whether meeting the condition spends the quantity
};

inline bool accumulated(std::string_view q) {
  return q == "balance" || q == "credit" || q == "token" || q == "score" || q == "reputation" || q == "count of contributions";
}

// The kind of gate an access rule sets: the strongest of its conditions' kinds.
inline Gate gate_of(const std::vector<Condition>& rule) {
  Gate g = Gate::open;
  auto rank = [](Gate x) { return x == Gate::purchasing_power ? 4 : x == Gate::standing ? 3 : x == Gate::membership ? 2 : x == Gate::allocation ? 1 : 0; };
  for (const Condition& c : rule) {
    Gate k = Gate::open;
    if (accumulated(c.reads)) k = c.spent ? Gate::purchasing_power : Gate::standing;
    else if (c.reads == "membership" || c.reads == "position" || c.reads == "conduct") k = Gate::membership;
    else if (c.reads == "need" || c.reads == "queue" || c.reads == "rotation" || c.reads == "lot") k = Gate::allocation;
    if (rank(k) > rank(g)) g = k;
  }
  return g;
}

// ---- One round of circulation -------------------------------------------------------------------------------------------------

struct Offer {
  std::size_t party = 0;
  int kind = 0;
  long long units = 0;
};
struct Need {
  std::size_t party = 0;
  int kind = 0;
};
struct Delivery {
  std::size_t provider = 0, receiver = 0;
  int kind = 0;
};

// Provision by need: the party least served so far in the round receives next, from the provider of the kind with most left.
// `excluded` parties receive nothing (a suspension); their offers still count.
inline std::vector<Delivery> provide(const std::vector<Offer>& offers, const std::vector<Need>& needs, std::size_t parties, calx::random::Stream& rng,
                                     const std::vector<bool>& excluded = {}) {
  std::map<int, std::vector<std::pair<std::size_t, long long>>> supply;  // kind -> (provider, units left)
  for (const Offer& o : offers)
    if (o.units > 0) supply[o.kind].push_back({o.party, o.units});
  std::vector<std::map<int, long long>> wanted(parties);
  std::vector<long long> asked(parties, 0), got(parties, 0);
  for (const Need& n : needs) {
    if (!excluded.empty() && excluded[n.party]) continue;
    wanted[n.party][n.kind]++;
    asked[n.party]++;
  }
  std::vector<Delivery> out;
  while (true) {
    // The parties with an unmet need of a kind still supplied, least served first (got/asked, compared exactly).
    std::vector<std::size_t> best;
    for (std::size_t p = 0; p < parties; p++) {
      bool can = false;
      for (const auto& [kind, units] : wanted[p])
        if (units > 0) {
          const auto s = supply.find(kind);
          if (s != supply.end())
            for (const auto& [provider, left] : s->second) can = can || (left > 0 && provider != p);
        }
      if (!can) continue;
      if (best.empty()) best.push_back(p);
      else {
        const long long lhs = got[p] * asked[best[0]], rhs = got[best[0]] * asked[p];
        if (lhs < rhs) best.assign(1, p);
        else if (lhs == rhs) best.push_back(p);
      }
    }
    if (best.empty()) break;
    const std::size_t r = best[rng.below(best.size())];
    // Of its kinds still supplied, the one it wants most; the provider of that kind with most left.
    int kind = -1;
    long long most = 0;
    for (const auto& [k, units] : wanted[r]) {
      if (units <= 0 || !supply.count(k)) continue;
      bool any = false;
      for (const auto& [provider, left] : supply[k]) any = any || (left > 0 && provider != r);
      if (any && units > most) kind = k, most = units;
    }
    auto& providers = supply[kind];
    std::size_t pick = providers.size();
    for (std::size_t i = 0; i < providers.size(); i++)
      if (providers[i].second > 0 && providers[i].first != r && (pick == providers.size() || providers[i].second > providers[pick].second)) pick = i;
    providers[pick].second--;
    wanted[r][kind]--;
    got[r]++;
    out.push_back({providers[pick].first, r, kind});
  }
  return out;
}

// A market in tokens: buyers in random order buy their needs one unit at a time at `price`, each from a seller of the kind with
// units left drawn at random; a buyer holding less than the price is refused. Tokens move from buyer to seller at once.
struct Sale {
  std::vector<Delivery> deliveries;
  long long refused_for_balance = 0;  // needs refused because the buyer held too few tokens, with a seller at hand
};

inline Sale market(const std::vector<Offer>& offers, const std::vector<Need>& needs, std::vector<double>& tokens, double price, calx::random::Stream& rng) {
  Sale out;
  std::map<int, std::vector<std::pair<std::size_t, long long>>> stock;
  for (const Offer& o : offers)
    if (o.units > 0) stock[o.kind].push_back({o.party, o.units});
  std::vector<Need> order = needs;
  for (std::size_t i = order.size(); i > 1; i--) std::swap(order[i - 1], order[rng.below(i)]);
  for (const Need& n : order) {
    auto it = stock.find(n.kind);
    if (it == stock.end()) continue;
    std::vector<std::size_t> sellers;
    for (std::size_t i = 0; i < it->second.size(); i++)
      if (it->second[i].second > 0 && it->second[i].first != n.party) sellers.push_back(i);
    if (sellers.empty()) continue;
    if (tokens[n.party] + 1e-9 < price) {
      out.refused_for_balance++;
      continue;
    }
    const std::size_t s = sellers[rng.below(sellers.size())];
    it->second[s].second--;
    tokens[n.party] -= price;
    tokens[it->second[s].first] += price;
    out.deliveries.push_back({it->second[s].first, n.party, n.kind});
  }
  return out;
}

// ---- The mediation profile ------------------------------------------------------------------------------------------------------

// The share of discharged commitments by mode, the mode being the commitment's declared matter (performance in a cycle,
// provision, gift, credit carried, transfer of an instrument, outside money), read from a chronicle's state (Definition 12).
inline std::map<std::string, double> mediation_profile(const thesmos::chronicle::State& s) {
  std::map<std::string, double> share;
  double total = 0;
  for (const auto& [id, c] : s.commitments)
    if (c.status == thesmos::commitments::Status::discharged) {
      share[c.matter] += 1;
      total += 1;
    }
  for (auto& [mode, n] : share) n /= total;
  return share;
}

}  // namespace communities::circulation
