// Checks of industrial_ecology::waste on an invented survey and network of facilities, each answer worked by hand: households weighed
// in two strata of two areas, their total by the survey's weights and its ultimate-cluster variance and interval, the generation per
// person as a ratio; a facility's level of control on a cumulative ladder; residues counted once, at the disposal site's gate; SDG
// 11.6.1 with the survey's interval carried through; an imbalance where residues exceed a gate; a residue sent nowhere named; refusals;
// and the composition's shares summing to one.

#include <industrial_ecology/waste/waste.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace industrial_ecology::waste;

namespace {
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  // 1,000 households of the lower stratum, 500 of the upper; four sampled in each (w = 250 and 125), two in each of two areas, a
  // week weighed.
  Survey S;
  S.households = {{1, 1000}, {2, 500}};
  S.sample = {{1, 1, 4, 14, 7}, {1, 1, 2, 7, 7}, {1, 2, 3, 21, 7}, {1, 2, 5, 14, 7}, {2, 1, 2, 28, 7}, {2, 1, 4, 35, 7}, {2, 2, 3, 21, 7}, {2, 2, 1, 14, 7}};
  const auto G = generation(S, {{"markets", 1.25}});
  // Per day: 2, 1, 3, 2 kg and 4, 5, 3, 2 kg; 250 · 8 + 125 · 14 kg = 3.75 t.
  s.check("households generate 250 · 8 + 125 · 14 kg = 3.75 t a day", G.households.value - 3.75, 1e-12);
  // Area totals 0.75 and 1.25 t (mean 1), 1.125 and 0.625 (mean 0.875): each stratum 2 · 0.125; V = 0.5 on 2 degrees of freedom.
  s.check("... its ultimate-cluster standard error √0.5 on 4 − 2 degrees of freedom", std::fabs(G.households.se - std::sqrt(0.5)) + std::fabs(G.households.df - 2), 1e-12);
  s.check("... and its interval 3.75 ± t(0.975, 2) √0.5", G.households.ci_upper - (3.75 + 4.302652729911275 * std::sqrt(0.5)), 1e-9);
  s.check("per person, Σ w kg / Σ w persons · days = 26,250 / 33,250 kg a day", G.per_person.value - 26250.0 / 33250.0, 1e-12);
  s.check("... 56/98 in the lower stratum and 98/70 in the upper", std::fabs(G.per_person_by_stratum.at(1) - 56.0 / 98) + std::fabs(G.per_person_by_stratum.at(2) - 1.4), 1e-12);
  s.check("with the markets' 1.25 t, 5 t a day in all", G.total - 5.0, 1e-12);

  Ladder L;
  L.levels = {"very limited", "limited", "basic", "improved", "full"};
  L.criteria = {{}, {"fenced"}, {"compacted", "covered"}, {"leachate"}, {"gas"}};
  L.controlled_from = 2;
  Facility R{"R", "a sorting plant", Kind::recovery, 1.0, 0.4, "Lf", {"fenced", "compacted", "covered"}, {}};
  Facility Lf{"Lf", "a landfill", Kind::disposal, 2.6, 0, "", {"fenced", "compacted", "covered", "leachate"}, {}};
  Facility D{"D", "a dumpsite", Kind::disposal, 0.8, 0, "", {"compacted"}, {}};
  s.require("a cumulative ladder: the landfill improved, the plant basic, the dumpsite very limited (it is compacted but not fenced)",
            level_of(Lf, L) == 3 && level_of(R, L) == 2 && level_of(D, L) == 0);
  const auto A = account(G, {R, Lf, D}, L, 0.1);
  s.check("delivered from collection: the plant's 1.0, the landfill's 2.6 less the plant's residue 0.6, the dumpsite's 0.8 = 3.8", A.delivered - 3.8, 1e-12);
  s.check("collected with 0.1 not delivered = 3.9, uncollected 1.1", std::fabs(A.collected - 3.9) + std::fabs(A.uncollected - 1.1), 1e-12);
  s.check("managed in controlled facilities: 0.4 recovered at the plant and 2.6 at the landfill's gate, the residue counted once", A.managed_controlled - 3.0, 1e-12);
  s.check("SDG 11.6.1 = 3.0 / 5.0", A.indicator - 0.6, 1e-12);
  s.check("... its interval the image of the survey's: 3 / (6.79 + 1.25) to 3 / (0.71 + 1.25)",
          std::fabs(A.indicator_lower - 3.0 / (G.households.ci_upper + 1.25)) + std::fabs(A.indicator_upper - 3.0 / (G.households.ci_lower + 1.25)), 1e-12);
  s.check("collection coverage 3.9/5 and recovery 0.4/5", std::fabs(A.collection_coverage - 0.78) + std::fabs(A.recovery_rate - 0.08), 1e-12);
  s.check("an account that balances: no residue exceeds a gate", A.imbalance, 1e-12);
  {
    Facility small = Lf;
    small.received = 0.5;
    const auto B = account(G, {R, small, D}, L);
    s.require("residues of 0.6 declared to a gate that weighed 0.5: an imbalance of 0.1, said", std::fabs(B.imbalance - 0.1) < 1e-12 && !B.remarks.empty());
    Facility lost = R;
    lost.residue_to = "";
    const auto C = account(G, {lost, Lf, D}, L);
    s.require("a residue sent nowhere named: collected, not managed, and said", std::fabs(C.managed_controlled - 3.0) < 1e-12 && std::fabs(C.delivered - (1.0 + 2.6 + 0.8)) < 1e-12 &&
                                                                                    !C.remarks.empty());
  }
  {
    Facility greedy = R;
    greedy.recovered = 1.5;
    s.require("a plant recovering more than it receives is refused", refused([&] { account(G, {greedy, Lf, D}, L); }));
    Facility odd = D;
    odd.recovered = 0.1;
    s.require("a disposal site that recovers is refused", refused([&] { account(G, {R, Lf, odd}, L); }));
    Facility loop = R;
    loop.residue_to = "R2";
    Facility R2 = R;
    R2.id = "R2";
    s.require("a residue sent to a recovery facility is refused", refused([&] { account(G, {loop, R2, Lf}, L); }));
    Survey bad = S;
    bad.sample.push_back({3, 1, 2, 5, 7});
    s.require("a household of a stratum with no count of households is refused", refused([&] { generation(bad); }));
  }
  {
    const std::vector<Sorted> sorted = {{1, 1, {{"organic", 6}, {"plastic", 2}, {"paper", 2}}}, {1, 2, {{"organic", 8}, {"plastic", 1}, {"paper", 1}}},
                                        {2, 1, {{"organic", 4}, {"plastic", 3}, {"paper", 3}}}, {2, 2, {{"organic", 5}, {"plastic", 3}, {"paper", 2}}}};
    const auto c = composition(sorted, S.households);
    double sum = 0, organic = 0;
    for (const auto& x : c) {
      sum += x.share.value;
      if (x.fraction == "organic") organic = x.share.value;
    }
    s.check("composition: organic (500 · 14 + 250 · 9) / (500 · 20 + 250 · 20) = 9,250/15,000; the shares sum to one", std::fabs(organic - 9250.0 / 15000) + std::fabs(sum - 1), 1e-12);
  }
  return s.done();
}
