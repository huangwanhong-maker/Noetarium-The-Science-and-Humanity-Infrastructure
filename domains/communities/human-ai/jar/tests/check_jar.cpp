// Checks of the jar, and its grid printed for the record.
//
// What must hold by construction: the jar's record in a thesmos chronicle agrees with its own tallies and replays; under a currency
// every refused need rests on a balance and nobody is suspended; under provision no refusal rests on a balance; provision with
// duty at full knowledge draws every capacity; the currency never reads what the community knows.
// What is measured, and printed: the share of needs met, overall and for the weakest tier and the free riders, under each
// arrangement, for two levels of need (balanced, and capacity three quarters of need), three shares of free riders and the
// epistemic resource from 0 to 1, each the mean of 40 runs of 20 rounds.

#include <communities/jar/jar.hpp>

#include <calx/verification/check.hpp>

#include <cstdio>
#include <iostream>
#include <string>

using namespace communities::jar;

int main() {
  calx::verification::Suite s;
  {
    Setting rec;
    rec.record = true;
    rec.free_riders = 0.25;
    rec.epistemic = 0.5;
    const Result r = run(rec);
    s.require("the jar's record in a thesmos chronicle agrees with its tallies, party by party, and replays (" + std::to_string(r.acts) + " acts)",
              r.recorded && r.record_agrees && r.replay_holds && r.acts > 3000);
  }
  const int runs = 40;
  auto at = [&](Regime g, long long need, double f, double q) {
    Setting x;
    x.regime = g;
    x.need = need;
    x.free_riders = f;
    x.epistemic = q;
    return mean(x, runs);
  };
  {
    const Result c = at(Regime::currency, 3, 0.25, 0.5), p = at(Regime::provision_with_duty, 3, 0.25, 0.5), o = at(Regime::provision_open, 3, 0.25, 0.5);
    s.require("under a currency refusals rest on balances and nobody is suspended; under provision no refusal rests on a balance",
              c.refused_for_balance > 0 && c.suspended == 0 && p.refused_for_balance == 0 && o.refused_for_balance == 0);
    const Result full = at(Regime::provision_with_duty, 3, 0.5, 1.0);
    s.require("with every capacity known, provision with duty draws every capacity (offered share " + std::to_string(full.offered) + ")", full.offered > 0.97);
    const Result c0 = at(Regime::currency, 3, 0.25, 0.0), c1 = at(Regime::currency, 3, 0.25, 1.0);
    s.require("the currency reads nothing of what the community knows: its outcome is the same at every epistemic resource", std::abs(c0.met - c1.met) < 1e-12 && std::abs(c0.met_weakest - c1.met_weakest) < 1e-12);
  }

  // The grid.
  const std::vector<Regime> regimes = {Regime::currency, Regime::currency_with_income, Regime::currency_with_targeted_income, Regime::provision_open, Regime::provision_with_duty};
  const std::vector<double> knowledge = {0.0, 0.1, 0.25, 0.5, 0.75, 1.0};
  std::printf("\nThe jar: share of needs met (all / weakest tier / free riders), mean of %d runs of 20 rounds, 36 parties, tiers 5, 3, 1.\n", runs);
  for (long long need : {3LL, 4LL})
    for (double f : {0.0, 0.25, 0.5}) {
      std::printf("\nneed %lld a round (capacity %s need), free riders %.2f\n", need, need == 3 ? "equal to" : "three quarters of", f);
      std::printf("  %-31s", "epistemic resource");
      for (double q : knowledge) std::printf("  %-17.2f", q);
      std::printf("\n");
      for (Regime g : regimes) {
        std::printf("  %-31s", std::string(word(g)).c_str());
        for (double q : knowledge) {
          const Result r = at(g, need, f, q);
          if (f > 0) std::printf("  %.2f / %.2f / %.2f", r.met, r.met_weakest, r.met_free);
          else std::printf("  %.2f / %.2f       ", r.met, r.met_weakest);
        }
        std::printf("\n");
      }
    }
  std::printf("\n");
  for (long long need : {3LL, 4LL}) {
    const Result open = at(Regime::provision_open, need, 0.5, 0.0), duty0 = at(Regime::provision_with_duty, need, 0.5, 0.0), duty1 = at(Regime::provision_with_duty, need, 0.5, 1.0);
    const Result cur = at(Regime::currency, need, 0.5, 0.0);
    std::printf("need %lld, free riders 0.50: overclaimed share of deliveries, open %.3f, duty at 0 %.3f, duty at 1 %.3f; suspended per party-round, duty at 0 %.3f, at 1 %.3f; "
                "idle share of offers, currency %.3f, duty at 1 %.3f; refused for balance per party-round, currency %.3f\n",
                need, open.overclaimed, duty0.overclaimed, duty1.overclaimed, duty0.suspended, duty1.suspended, cur.idle, duty1.idle, cur.refused_for_balance);
  }
  std::printf("\n");
  // The price of the epistemic resource: provision with duty when each audit consumes service, in the hardest setting.
  std::printf("need 4, free riders 0.50, provision with duty, share of needs met (all / weakest) when an audit costs:\n  %-12s", "audit cost");
  for (double q : knowledge) std::printf("  %-11.2f", q);
  std::printf("\n");
  for (double cost : {0.0, 0.25, 0.5, 1.0, 2.0}) {
    std::printf("  %-12.2f", cost);
    for (double q : knowledge) {
      Setting x;
      x.regime = Regime::provision_with_duty;
      x.need = 4;
      x.free_riders = 0.5;
      x.epistemic = q;
      x.audit_cost = cost;
      const Result r = mean(x, runs);
      std::printf("  %.2f / %.2f", r.met, r.met_weakest);
    }
    std::printf("\n");
  }
  std::printf("  the currency, for comparison: %.2f / %.2f\n\n", at(Regime::currency, 4, 0.5, 0.0).met, at(Regime::currency, 4, 0.5, 0.0).met_weakest);
  {
    // The crossing: the least epistemic resource at which provision with duty meets at least as many needs as the currency, and
    // as many of the weakest tier's.
    for (long long need : {3LL, 4LL})
      for (double f : {0.25, 0.5}) {
        const Result c = at(Regime::currency, need, f, 0.0);
        double q_all = -1, q_weak = -1;
        for (double q : knowledge) {
          const Result p = at(Regime::provision_with_duty, need, f, q);
          if (q_all < 0 && p.met >= c.met) q_all = q;
          if (q_weak < 0 && p.met_weakest >= c.met_weakest) q_weak = q;
        }
        std::printf("need %lld, free riders %.2f: provision with duty meets as many needs as the currency from an epistemic resource of %s, "
                    "and as many of the weakest tier's from %s\n",
                    need, f, q_all < 0 ? "none of those tried" : std::to_string(q_all).substr(0, 4).c_str(), q_weak < 0 ? "none of those tried" : std::to_string(q_weak).substr(0, 4).c_str());
      }
  }
  return s.done();
}
