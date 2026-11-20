// An economy of places: households and firms in places that trade with one another and with outside, every payment in
// the accounts (ADR 0139).
//
// Each place has households and firms. A household short of money (holding less than a declared amount) spends all it
// holds, mostly at its own place's firms; a household with money spends a share of what it holds, more of it bought from
// outside. Firms pay out what they received in the last period: wages to their place's households, supplies from other
// places' firms (in declared trade weights), the rest imported. Outside spends a share of what it holds at the places'
// firms, in proportion to how many each place has. A government issues the money and pays the grants.
//
// The difference between the two kinds of household is the state-dependence through which the order of grants matters
// (the survey of evolutionary allocation, Part VI §6.2: a constrained household spends locally at once, a liquid one saves
// or imports): money given to one place first reaches the next place through trade before that place's own grant, and
// changes how its households will spend it. Every habit is a declaration, illustrative until a record calibrates it.
#pragma once

#include <economics/ledger/economy.hpp>

#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

namespace economics::ledger {

struct Place {
  std::string name;
  std::size_t households = 4, firms = 2;
  Amount household_money = 100, firm_money = 200;  // opening balances, each
};

struct Habits {
  Amount short_below = 50;    // a household holding less is short of money
  double short_spend = 1.0;   // the share of its holding a short household spends in a period
  double short_local = 0.8;   // ... and the share of that spent at its place's firms
  double spend = 0.5;         // the share a household with money spends in a period
  double local = 0.4;         // ... and the share of that spent at its place's firms
  double wages = 0.6;         // a firm's wages to its place's households, as a share of what it received in the last period
  double supplies = 0.2;      // ... its purchases from other places' firms (the rest is imported)
  double exports = 0.1;       // the share of its holding outside spends at the places' firms in a period
  Amount outside_money = 1000;
  std::string source = "declared, illustrative";
};

struct Places {
  std::vector<std::string> names;
  std::vector<std::vector<std::size_t>> households, firms;  // by place
  std::size_t government = 0;
};

// Builds the economy. `trade[r][s]`: the weight of place s in what place r's firms buy from other places (row-normalised;
// empty: equal among the others).
inline Places regional(Economy& E, const std::vector<Place>& places, const Habits& H, std::vector<std::vector<double>> trade = {}) {
  const std::size_t R = places.size();
  if (R == 0) throw Refusal("regional: at least one place");
  if (H.short_spend < 0 || H.short_spend > 1 || H.spend < 0 || H.spend > 1 || H.short_local < 0 || H.short_local > 1 || H.local < 0 || H.local > 1 ||
      H.wages < 0 || H.supplies < 0 || H.wages + H.supplies > 1 || H.exports < 0 || H.exports > 1)
    throw Refusal("regional: every habit is a share between 0 and 1, and wages and supplies together at most 1");
  if (trade.empty()) {
    trade.assign(R, std::vector<double>(R, 1.0));
    for (std::size_t r = 0; r < R; r++) trade[r][r] = 0;
  }
  if (trade.size() != R) throw Refusal("regional: a row of trade weights for each place");
  Places P;
  P.households.resize(R), P.firms.resize(R);
  for (std::size_t r = 0; r < R; r++) {
    const auto& pl = places[r];
    if (pl.households == 0 || pl.firms == 0) throw Refusal("regional: " + pl.name + " needs households and firms");
    P.names.push_back(pl.name);
    for (std::size_t k = 0; k < pl.households; k++) P.households[r].push_back(E.L.add_agent({pl.name + " household " + std::to_string(k + 1), Sector::household, pl.name}));
    for (std::size_t k = 0; k < pl.firms; k++) P.firms[r].push_back(E.L.add_agent({pl.name + " firm " + std::to_string(k + 1), Sector::firm, pl.name}));
  }
  P.government = E.L.add_agent({"government", Sector::government, ""});
  E.issuer = P.government;
  E.money = E.L.add_instrument({"currency", true});
  E.start();
  for (std::size_t r = 0; r < R; r++) {
    for (const std::size_t h : P.households[r])
      if (places[r].household_money > 0) E.L.post(opening(h, E.money, E.issuer, places[r].household_money));
    for (const std::size_t f : P.firms[r])
      if (places[r].firm_money > 0) E.L.post(opening(f, E.money, E.issuer, places[r].firm_money));
  }
  if (H.outside_money > 0) E.L.post(opening(E.L.outside(), E.money, E.issuer, H.outside_money));
  const std::size_t out = E.L.outside();
  using nomos::agents::Action;
  using nomos::agents::Behaviour;
  using nomos::agents::Observation;
  using nomos::agents::Origin;
  for (std::size_t r = 0; r < R; r++) {
    const auto firms = P.firms[r];
    for (const std::size_t h : P.households[r]) {
      Behaviour b;
      b.declared = {"spend by whether short of money", Origin::rule, {"held"}, {"purchase", "import"}, H.source};
      b.decide = [firms, out, H](const Observation& o, calx::random::Stream&) {
        const double held = o.at("held");
        const bool short_of = held < static_cast<double>(H.short_below);
        const double spent = held * (short_of ? H.short_spend : H.spend), local = short_of ? H.short_local : H.local;
        std::vector<Action> acts;
        for (const std::size_t f : firms) acts.push_back({"purchase", f, spent * local / static_cast<double>(firms.size())});
        acts.push_back({"import", out, spent * (1 - local)});
        return acts;
      };
      E.behaviour[h] = std::move(b);
    }
    // Where this place's firms buy their supplies.
    double tw = 0;
    for (std::size_t s = 0; s < R; s++) tw += s == r ? 0 : std::max(0.0, trade[r][s]);
    std::vector<std::pair<std::size_t, double>> suppliers;
    if (tw > 0)
      for (std::size_t s = 0; s < R; s++)
        if (s != r && trade[r][s] > 0)
          for (const std::size_t f : P.firms[s]) suppliers.push_back({f, trade[r][s] / tw / static_cast<double>(P.firms[s].size())});
    const auto homes = P.households[r];
    for (const std::size_t f : P.firms[r]) {
      Behaviour b;
      b.declared = {"pay out last period's receipts", Origin::rule, {"received"}, {"wage", "purchase", "import"}, H.source};
      b.decide = [homes, suppliers, out, H](const Observation& o, calx::random::Stream&) {
        const double got = o.at("received");
        std::vector<Action> acts;
        for (const std::size_t h : homes) acts.push_back({"wage", h, got * H.wages / static_cast<double>(homes.size())});
        const double sup = suppliers.empty() ? 0 : got * H.supplies;
        for (const auto& [f2, w] : suppliers) acts.push_back({"purchase", f2, sup * w});
        acts.push_back({"import", out, got - got * H.wages - sup});
        return acts;
      };
      E.behaviour[f] = std::move(b);
    }
  }
  {
    std::size_t nf = 0;
    for (const auto& fs : P.firms) nf += fs.size();
    std::vector<std::size_t> all;
    for (const auto& fs : P.firms) all.insert(all.end(), fs.begin(), fs.end());
    Behaviour b;
    b.declared = {"outside buys from the places", Origin::rule, {"held"}, {"export"}, H.source};
    b.decide = [all, nf, H](const Observation& o, calx::random::Stream&) {
      std::vector<Action> acts;
      const double spent = o.at("held") * H.exports;
      for (const std::size_t f : all) acts.push_back({"export", f, spent / static_cast<double>(nf)});
      return acts;
    };
    E.behaviour[out] = std::move(b);
  }
  return P;
}

// Grants to places, each split among the place's households by the largest remainder, the place's grant at the given
// instant, labelled with the place's name as its origin (so that the readings follow each place's grant).
inline void grant_places(Economy& E, const Places& P, const std::vector<std::pair<std::size_t, Amount>>& to_place, const std::vector<double>& when) {
  if (when.size() != to_place.size()) throw Refusal("grant_places: an instant for each grant");
  for (std::size_t g = 0; g < to_place.size(); g++) {
    const auto& [r, amount] = to_place[g];
    if (r >= P.names.size()) throw Refusal("grant_places: no such place");
    if (amount <= 0) continue;
    const auto& hs = P.households[r];
    const Amount each = amount / static_cast<Amount>(hs.size()), rest = amount % static_cast<Amount>(hs.size());
    for (std::size_t k = 0; k < hs.size(); k++) {
      const Amount a = each + (static_cast<Amount>(k) < rest ? 1 : 0);
      if (a > 0) E.grants.push_back({when[g], none, hs[k], a, P.names[r], "grant"});
      E.tracer.place(P.names[r], P.names[r]);
    }
  }
}

}  // namespace economics::ledger
