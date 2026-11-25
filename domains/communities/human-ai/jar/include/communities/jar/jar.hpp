// The jar: circulation under scarcity, a construction that puts the user's hypothesis of dissolution to a test.
//
// The hypothesis (the user, 2026-11-25): "base on resource scarcity, to make 'fair' value circulation, some object like currency
// need appear, but ... we can make them disappear, not necessary exist currency, if can well epistemic value circulation, and
// ensure justice. But just a hypothesis."
//
// The construction. Parties of three tiers of capacity (by default 5, 3 and 1 units of service a round, each of one of six
// kinds) need units of other kinds each round (3: capacity and need balance in aggregate; 4: capacity is three quarters of need).
// A share of the parties are free riders: they offer the least that keeps their access under the arrangement in force, and declare
// the weakest capacity. Five arrangements:
//
//   currency                       tokens, a stock at the start; a market one unit at a time; a buyer short of tokens is refused
//   currency with income           the same, holdings lapsing by a demurrage, and an income issued to every party each round
//   currency with targeted income  the income tops each party up to its need by the community's estimate of its capacity
//   provision, open                what is offered is handed out by need, least served first; nothing is asked of anyone
//   provision with duty            the same, and each party owes its capacity as the community knows it; a party found offering
//                                  less is suspended from provision for the next round
//
// The epistemic resource is the probability that the community verifies a party's capacity in a round (an audit, a test, a
// history read); once verified, a capacity is known. A free rider under duty shirks when the effort it saves is worth more than
// the provision it expects to lose: effort·(capacity − declared) > P(found)·need·(its last share met), P(found) being 1 once its
// capacity is known and the epistemic resource otherwise; under provision it also claims more need than it has when the units it
// adds outweigh what it expects to lose (an audit reads the claim too). Under a currency a free rider offers what its needs require
// it to earn, and its claims are its purchases.
//
// What lies outside: quality, prices other than one, savings and debt, persons' judgements of fairness, learning, collusion; the
// sanction's credibility is assumed. The jar measures, it proves nothing about real communities.

#pragma once

#include <communities/circulation/circulation.hpp>

#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/constitutions/constitutions.hpp>

#include <calx/random/stream.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <string_view>
#include <vector>

namespace communities::jar {

enum class Regime { currency, currency_with_income, currency_with_targeted_income, provision_open, provision_with_duty };

inline std::string_view word(Regime r) {
  switch (r) {
    case Regime::currency: return "currency";
    case Regime::currency_with_income: return "currency with income";
    case Regime::currency_with_targeted_income: return "currency with targeted income";
    case Regime::provision_open: return "provision, open";
    case Regime::provision_with_duty: return "provision with duty";
  }
  return "?";
}
inline bool is_currency(Regime r) { return r == Regime::currency || r == Regime::currency_with_income || r == Regime::currency_with_targeted_income; }

struct Setting {
  Regime regime = Regime::provision_with_duty;
  int parties = 36, kinds = 6, rounds = 20;
  std::vector<long long> tiers = {5, 3, 1};
  long long need = 3;
  double free_riders = 0.25;  // share of parties that offer the least that keeps their access
  double epistemic = 0.5;     // probability that a party's capacity is verified in a round
  double effort = 0.5;        // what a unit of effort costs a free rider, in units of a need met
  double stock = 3;           // tokens each party holds at the start
  double income = 2;          // tokens issued to every party each round (currency with income)
  double demurrage = 0.1;     // share of holdings that lapses each round (with income)
  double tolerance = 0.5;     // shortfall below the known capacity that the duty tolerates
  long long inflation = 1;    // units of need a free rider adds to its claim under provision, when it expects not to be found
  double audit_cost = 0;      // units of service an audit consumes, taken from what was offered (the price of the epistemic resource)
  std::uint64_t seed = 1;
  bool record = false;        // enter every delivery in a thesmos chronicle (provision regimes)
};

struct Result {
  double met = 0, met_weakest = 0, met_strongest = 0, met_free = 0, met_cooperators = 0;
  double offered = 0;              // share of capacity offered
  double idle = 0;                 // share of what was offered that nobody received
  double refused_for_balance = 0;  // needs refused because a balance was short, per party and round
  double suspended = 0;            // party-rounds suspended, per party and round
  double overclaimed = 0;          // share of deliveries that went beyond the receiver's true need
  double audits = 0;               // share of what was offered that verification consumed
  std::vector<double> met_by_round, weakest_by_round;
  // When recorded: whether the chronicle's own tallies agree with the jar's, the number of acts, and whether a replay holds.
  bool recorded = false, record_agrees = false, replay_holds = false;
  std::size_t acts = 0;
};

// The constitution of a jar community: members who commit, deliver and accept, provision by need its form of circulation.
inline const char* jar_constitution() {
  return R"({
  "format": "noetarium.constitution/1", "name": "jar",
  "purpose": "An invented community in which what members offer is handed out by need; used by the jar to record its rounds.",
  "positions": {"member": {"held by": ["person", "organisation"]}, "steward": {"held by": ["person"]}},
  "boundary": {"member": {"entry": {"decided by": "admit"}, "exit": "free"}, "steward": {"entry": {"decided by": "admit"}, "exit": "free"}},
  "choice": [{"who": "anyone", "may": ["say", "claim"]}, {"who": "member", "may": ["commit", "deliver", "accept", "release", "propose", "vote"]},
             {"who": "steward", "may": ["admit", "amend"]}, {"who": "steward", "must": ["answer"]}],
  "bodies": {"stewards": {"of": ["steward"]}, "assembly": {"of": ["member", "steward"]}},
  "aggregation": {"admit": {"body": "stewards", "rule": "any one", "count by": "person", "tempo": "operative"},
                  "answer claim": {"body": "stewards", "rule": "any one", "count by": "person", "tempo": "deliberative"},
                  "amend": {"body": "assembly", "rule": "majority", "count by": "origin", "tempo": "deliberative"}},
  "scenes": {"provision": {"acts": ["commit", "deliver", "accept"], "read": "anyone", "write": ["member"], "presence": "mixed", "tempo": "operative"}},
  "conversions": {}, "receiving": {"unit": "units of service", "per round": 1000}, "circulation": ["common provision"],
  "claims": {"route": "stewards", "decided by": "answer claim", "outsiders": true},
  "custody": {"powers": {}, "reserve": []}, "preserved": ["a member's needs are read before what it gave"],
  "amendment": {"operational": "amend", "collective choice": "amend", "constitutional": "amend", "entrenched": ["claims"]},
  "records": {"captured": ["acts of force"]}
})";
}

inline Result run(const Setting& s) {
  calx::random::Stream rng(s.seed, static_cast<std::uint32_t>(s.regime), 7, static_cast<std::uint32_t>(s.need));
  const std::size_t n = static_cast<std::size_t>(s.parties);
  std::vector<long long> capacity(n), declared(n);
  std::vector<int> kind(n);
  std::vector<bool> free(n, false), known(n, false), excluded(n, false);
  for (std::size_t i = 0; i < n; i++) {
    capacity[i] = s.tiers[(i / static_cast<std::size_t>(s.kinds)) % s.tiers.size()];
    kind[i] = static_cast<int>(i % static_cast<std::size_t>(s.kinds));
  }
  {
    std::vector<std::size_t> order(n);
    for (std::size_t i = 0; i < n; i++) order[i] = i;
    for (std::size_t i = n; i > 1; i--) std::swap(order[i - 1], order[rng.below(i)]);
    const auto riders = static_cast<std::size_t>(std::lround(s.free_riders * double(n)));
    for (std::size_t k = 0; k < riders; k++) free[order[k]] = true;
  }
  const long long weakest = *std::min_element(s.tiers.begin(), s.tiers.end()), strongest = *std::max_element(s.tiers.begin(), s.tiers.end());
  std::vector<long long> estimate(n);
  for (std::size_t i = 0; i < n; i++) declared[i] = free[i] ? weakest : capacity[i], estimate[i] = declared[i];
  std::vector<double> tokens(n, s.stock), last_share(n, 0.8);
  std::vector<long long> met(n, 0), asked(n, 0), received(n, 0);
  long long offered_total = 0, capacity_total = 0, delivered_total = 0, refused = 0, suspended = 0;
  Result out;

  thesmos::kernel::Parties people;
  thesmos::chronicle::State chronicle;
  const bool record = s.record && !is_currency(s.regime);
  if (record) {
    for (std::size_t i = 0; i < n; i++) people.add({"p" + std::to_string(i), thesmos::kernel::Kind::person});
    people.add({"steward", thesmos::kernel::Kind::person});
    chronicle = thesmos::chronicle::State(people, thesmos::constitutions::read(jar_constitution()));
    thesmos::acts::Act found;
    found.author = "steward";
    found.kind = "found";
    found.holders.push_back({"steward", "steward"});
    for (std::size_t i = 0; i < n; i++) found.holders.push_back({"p" + std::to_string(i), "member"});
    thesmos::chronicle::step(chronicle, found);
  }

  long long overclaimed = 0, consumed_by_audits = 0;
  double audit_carry = 0;
  for (int t = 1; t <= s.rounds; t++) {
    // Verification this round (the epistemic resource): an audit reads a party's capacity and its claim of need.
    std::vector<bool> audited(n, false), known_before = known, inflated(n, false);
    if (s.regime == Regime::provision_with_duty || s.regime == Regime::currency_with_targeted_income)
      for (std::size_t i = 0; i < n; i++) audited[i] = rng.uniform() < s.epistemic;
    // Needs of the round: each unit of a kind other than the party's own. Under provision a free rider claims more than it needs
    // when it expects not to be found: when the units it adds are worth more than the provision it would lose.
    std::vector<circulation::Need> needs;
    for (std::size_t i = 0; i < n; i++) {
      long long claim = s.need;
      if (free[i] && !is_currency(s.regime)) {
        const double p_found = s.regime == Regime::provision_with_duty ? s.epistemic : 0.0;
        if (double(s.inflation) > p_found * double(s.need)) claim += s.inflation, inflated[i] = true;
      }
      for (long long u = 0; u < claim; u++) {
        int k = static_cast<int>(rng.below(static_cast<std::uint64_t>(s.kinds - 1)));
        if (k >= kind[i]) k++;
        needs.push_back({i, k});
      }
    }
    // Incomes, before offers.
    if (s.regime == Regime::currency_with_income || s.regime == Regime::currency_with_targeted_income)
      for (std::size_t i = 0; i < n; i++) {
        tokens[i] *= 1 - s.demurrage;
        tokens[i] += s.regime == Regime::currency_with_income ? s.income : double(std::max(0LL, s.need - estimate[i]));
      }
    // Offers.
    std::vector<circulation::Offer> offers;
    std::vector<long long> offer(n);
    for (std::size_t i = 0; i < n; i++) {
      long long o = capacity[i];
      if (free[i]) {
        if (is_currency(s.regime)) o = std::min(capacity[i], std::max(0LL, s.need - static_cast<long long>(std::floor(tokens[i] + 1e-9))));
        else if (s.regime == Regime::provision_open) o = 0;
        else {
          const double p_found = known_before[i] ? 1.0 : s.epistemic;
          const bool shirk = s.effort * double(capacity[i] - declared[i]) > p_found * double(s.need) * last_share[i];
          o = shirk ? declared[i] : capacity[i];
        }
      }
      offer[i] = o;
      offered_total += o;
      capacity_total += capacity[i];
      offers.push_back({i, kind[i], o});
    }
    // The round's audits consume service: units drawn from the offers at random, the fraction carried to the next round.
    if (s.audit_cost > 0) {
      double cost = 0;
      for (std::size_t i = 0; i < n; i++) cost += audited[i] ? s.audit_cost : 0.0;
      audit_carry += cost;
      while (audit_carry >= 1) {
        std::vector<std::size_t> with;
        for (std::size_t k = 0; k < offers.size(); k++)
          if (offers[k].units > 0) with.push_back(k);
        if (with.empty()) break;
        offers[with[rng.below(with.size())]].units--;
        audit_carry -= 1;
        consumed_by_audits++;
      }
    }
    // Circulation.
    std::vector<circulation::Delivery> deliveries;
    if (is_currency(s.regime)) {
      const auto sale = circulation::market(offers, needs, tokens, 1.0, rng);
      deliveries = sale.deliveries;
      refused += sale.refused_for_balance;
    } else {
      deliveries = circulation::provide(offers, needs, n, rng, excluded);
      for (std::size_t i = 0; i < n; i++) suspended += excluded[i];
    }
    std::vector<long long> got(n, 0);
    for (const auto& d : deliveries) got[d.receiver]++;
    delivered_total += static_cast<long long>(deliveries.size());
    double round_met = 0, round_weak = 0, weak_need = 0;
    for (std::size_t i = 0; i < n; i++) {
      const long long useful = std::min(got[i], s.need);  // what met a true need; the rest was overclaimed
      overclaimed += got[i] - useful;
      received[i] += got[i];
      met[i] += useful;
      asked[i] += s.need;
      round_met += double(useful);
      if (capacity[i] == weakest) round_weak += double(useful), weak_need += double(s.need);
      if (!excluded[i]) last_share[i] = double(useful) / double(s.need);
    }
    out.met_by_round.push_back(round_met / double(n * static_cast<std::size_t>(s.need)));
    out.weakest_by_round.push_back(round_weak / weak_need);
    // The record of the round.
    if (record) {
      std::size_t k = 0;
      for (const auto& d : deliveries) {
        const std::string id = "r" + std::to_string(t) + "." + std::to_string(k++);
        const std::string provider = "p" + std::to_string(d.provider), receiver = "p" + std::to_string(d.receiver);
        thesmos::acts::Act a;
        a.turn = t;
        a.author = provider;
        a.kind = "commit";
        a.fields = {{"id", id}, {"to", receiver}, {"matter", "provision"}, {"unit", "kind " + std::to_string(d.kind)}};
        thesmos::chronicle::step(chronicle, a);
        a.kind = "deliver";
        a.fields = {{"id", id}};
        thesmos::chronicle::step(chronicle, a);
        a.author = receiver;
        a.kind = "accept";
        thesmos::chronicle::step(chronicle, a);
      }
    }
    // The duty: a party found offering less than its capacity as the community knows it (its declaration, until verified), or
    // found claiming more than it needs, is suspended for the next round.
    if (s.regime == Regime::provision_with_duty)
      for (std::size_t i = 0; i < n; i++) {
        const long long owed = known_before[i] || audited[i] ? capacity[i] : declared[i];
        excluded[i] = double(offer[i]) < double(owed) - s.tolerance || (audited[i] && inflated[i]);
      }
    for (std::size_t i = 0; i < n; i++)
      if (audited[i]) known[i] = true, estimate[i] = capacity[i];
  }

  auto share = [&](auto pick) {
    double m = 0, a = 0;
    for (std::size_t i = 0; i < n; i++)
      if (pick(i)) m += double(met[i]), a += double(asked[i]);
    return a > 0 ? m / a : 0.0;
  };
  out.met = share([](std::size_t) { return true; });
  out.met_weakest = share([&](std::size_t i) { return capacity[i] == weakest; });
  out.met_strongest = share([&](std::size_t i) { return capacity[i] == strongest; });
  out.met_free = share([&](std::size_t i) { return bool(free[i]); });
  out.met_cooperators = share([&](std::size_t i) { return !free[i]; });
  out.offered = double(offered_total) / double(capacity_total);
  out.idle = offered_total ? double(offered_total - delivered_total - consumed_by_audits) / double(offered_total) : 0.0;
  out.audits = offered_total ? double(consumed_by_audits) / double(offered_total) : 0.0;
  out.refused_for_balance = double(refused) / double(n * static_cast<std::size_t>(s.rounds));
  out.suspended = double(suspended) / double(n * static_cast<std::size_t>(s.rounds));
  out.overclaimed = delivered_total ? double(overclaimed) / double(delivered_total) : 0.0;
  if (record) {
    out.recorded = true;
    out.acts = chronicle.acts.size();
    bool agrees = true;
    std::vector<long long> in_record(n, 0);
    for (const auto& [id, c] : chronicle.commitments)
      if (c.status == thesmos::commitments::Status::discharged) in_record[std::stoul(c.creditor.substr(1))]++;
    for (std::size_t i = 0; i < n; i++) agrees = agrees && in_record[i] == received[i];
    out.record_agrees = agrees;
    out.replay_holds = thesmos::chronicle::replay(people, thesmos::constitutions::read(jar_constitution()), chronicle.acts, chronicle.journal).holds();
  }
  return out;
}

// The mean of `runs` runs of a setting, each with its own seed.
inline Result mean(Setting s, int runs) {
  Result m;
  for (int r = 0; r < runs; r++) {
    s.seed = static_cast<std::uint64_t>(1000 + r);
    const Result x = run(s);
    m.met += x.met / runs, m.met_weakest += x.met_weakest / runs, m.met_strongest += x.met_strongest / runs;
    m.met_free += x.met_free / runs, m.met_cooperators += x.met_cooperators / runs;
    m.offered += x.offered / runs, m.idle += x.idle / runs, m.refused_for_balance += x.refused_for_balance / runs, m.suspended += x.suspended / runs;
    m.overclaimed += x.overclaimed / runs;
    if (m.met_by_round.empty()) m.met_by_round.assign(x.met_by_round.size(), 0), m.weakest_by_round.assign(x.weakest_by_round.size(), 0);
    for (std::size_t k = 0; k < x.met_by_round.size(); k++) m.met_by_round[k] += x.met_by_round[k] / runs, m.weakest_by_round[k] += x.weakest_by_round[k] / runs;
  }
  return m;
}

}  // namespace communities::jar
