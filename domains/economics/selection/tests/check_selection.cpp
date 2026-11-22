// Checks of economics::selection, in exact rationals: a panel of three firms worked by hand in every method (A and B survive, C exits,
// D enters): Φ 2, Φ' 16/5; Baily, Hulten and Campbell's within 1/2, between 0, entry 6/5, exit −1/2; Foster, Haltiwanger and Krizan's
// between −1/5, within 1/2, cross 1/10, entry 4/5, exit 0; Griliches and Regev's within 11/20, between −9/50, entry 17/25, exit 3/20;
// Melitz and Polanec's survivors' unweighted mean 1/2, covariance 0, entry 7/10, exit 0; Price's selection −1/4 and learning 3/4.
// Melitz and Polanec's closed form for a representative firm growing by γ. A constant added to every productivity moves Baily, Hulten and
// Campbell's entry, exit and between and no other method's terms. Olley and Pakes's static identity. On 150 random panels with industries, every
// method summing to ΔΦ and Price's selection split between and within industries. Logs of a panel in levels. Refusals.

#include <economics/selection/productivity.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace economics::selection;
using Q = morphe::arithmetic::Rational;

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
Q q(long long n, long long d = 1) { return Q(morphe::arithmetic::Integer(n), morphe::arithmetic::Integer(d)); }
template <class T>
T total(const Result<T>& R) {
  T s{};
  for (const auto& [k, v] : R.terms) s += v;
  return s;
}
Panel<Q> hand() {
  Panel<Q> p;
  p.before = {{"A", q(2), q(1), ""}, {"B", q(1), q(4), ""}, {"C", q(1), q(2), ""}};
  p.after = {{"A", q(3), q(2), ""}, {"B", q(1), q(4), ""}, {"D", q(1), q(6), ""}};
  return p;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  {
    const auto p = hand();
    const auto B = decompose(p, Method::bhc), F = decompose(p, Method::fhk), G = decompose(p, Method::gr), M = decompose(p, Method::dopd), P = decompose(p, Method::price);
    s.require("a panel by hand: Φ 2, Φ' 16/5; two survivors, one entrant, one exiter", B.before == q(2) && B.after == q(16, 5) && B.survivors == 2 && B.entrants == 1 && B.exiters == 1);
    s.require("... Baily, Hulten and Campbell: within 1/2, between 0, entry 6/5, exit −1/2",
              B.term("within") == q(1, 2) && B.term("between") == q(0) && B.term("entry") == q(6, 5) && B.term("exit") == q(-1, 2));
    s.require("... Foster, Haltiwanger and Krizan: between −1/5, within 1/2, cross 1/10, entry 4/5, exit 0",
              F.term("between") == q(-1, 5) && F.term("within") == q(1, 2) && F.term("cross") == q(1, 10) && F.term("entry") == q(4, 5) && F.term("exit") == q(0));
    s.require("... Griliches and Regev: within 11/20, between −9/50, entry 17/25, exit 3/20",
              G.term("within") == q(11, 20) && G.term("between") == q(-9, 50) && G.term("entry") == q(17, 25) && G.term("exit") == q(3, 20));
    s.require("... Melitz and Polanec: the survivors' unweighted mean 1/2 and covariance 0 (Φ_S 2 to 5/2), entry 7/10, exit 0",
              M.term("survivors' unweighted mean") == q(1, 2) && M.term("survivors' covariance") == q(0) && M.term("entry") == q(7, 10) && M.term("exit") == q(0));
    s.require("... Price over the survivors: selection −1/4, learning 3/4", P.term("selection") == q(-1, 4) && P.term("learning") == q(3, 4));
    bool sums = true;
    for (const auto& R : {B, F, G, M, P}) sums = sums && total(R) == q(6, 5) && R.change == q(6, 5);
    s.require("... every method sums to ΔΦ = 6/5", sums);
  }
  {
    // A representative firm: every productivity 2 before and 2 + γ after, γ = 1/4; survivors weigh 3 and 2 before, 4 and 2 after; an
    // exiter weighs 1 before, an entrant 2 after.
    const Q g = q(1, 4);
    Panel<Q> p;
    p.before = {{"S1", q(3), q(2), ""}, {"S2", q(2), q(2), ""}, {"X", q(1), q(2), ""}};
    p.after = {{"S1", q(4), q(2) + g, ""}, {"S2", q(2), q(2) + g, ""}, {"N", q(2), q(2) + g, ""}};
    const auto M = decompose(p, Method::dopd), G = decompose(p, Method::gr), F = decompose(p, Method::fhk);
    const Q sE = q(1, 4), sX = q(1, 6);
    s.require("Melitz and Polanec's closed form, every firm growing by γ: entry 0, s_E γ/2, s_E γ (dynamic Olley–Pakes, Griliches–Regev, FHK); exit 0, s_X γ/2, 0",
              M.term("entry") == Q(0) && G.term("entry") == sE * g / q(2) && F.term("entry") == sE * g && M.term("exit") == Q(0) && G.term("exit") == sX * g / q(2) && F.term("exit") == Q(0));
    s.require("... the dynamic Olley–Pakes decomposition puts all of γ in the survivors' unweighted mean", M.term("survivors' unweighted mean") == g && M.term("survivors' covariance") == Q(0));
  }
  {
    // A constant added to every productivity.
    auto p = hand(), shifted = hand();
    for (auto* side : {&shifted.before, &shifted.after})
      for (auto& f : *side) f.productivity += q(10);
    bool invariant = true;
    for (Method m : {Method::fhk, Method::gr, Method::dopd, Method::price}) {
      const auto A = decompose(p, m), B = decompose(shifted, m);
      for (std::size_t k = 0; k < A.terms.size(); k++) invariant = invariant && A.terms[k].second == B.terms[k].second;
    }
    const auto A = decompose(p, Method::bhc), B = decompose(shifted, Method::bhc);
    s.require("a constant added to every productivity: FHK's, Griliches and Regev's, Melitz and Polanec's and Price's terms unchanged", invariant);
    s.require("... Baily, Hulten and Campbell's entry moves by 10 s_E, exit by −10 s_X and between by 10 (s'_S − s_S), within unchanged",
              B.term("entry") - A.term("entry") == q(10) * q(1, 5) && B.term("exit") - A.term("exit") == q(-10) * q(1, 4) &&
                  B.term("between") - A.term("between") == q(10) * (q(4, 5) - q(3, 4)) && B.term("within") == A.term("within"));
  }
  {
    const auto o = olley_pakes<Q>({q(1), q(2), q(5)}, {q(3), q(1), q(2)});
    s.require("Olley and Pakes: Φ = φ̄ + cov: aggregate 15/8 = 2 − 1/8", o.aggregate == q(15, 8) && o.unweighted_mean == q(2) && o.covariance == q(-1, 8));
  }
  {
    calx::random::Stream g(20261123, 4, 5, 6);
    bool sums = true, levels = true;
    int with_industries = 0;
    for (int t = 0; t < 150; t++) {
      Panel<Q> p;
      const int n = 3 + static_cast<int>(g.below(6));
      for (int i = 0; i < n; i++) {
        const std::string id = "f" + std::to_string(i);
        const std::string ind = g.below(2) ? "manufacturing" : "services";
        const auto fate = g.below(5);
        if (fate != 0) p.before.push_back({id, q(1 + static_cast<long long>(g.below(9))), q(static_cast<long long>(g.below(21)) - 5, 4), ind});
        if (fate != 1) p.after.push_back({id, q(1 + static_cast<long long>(g.below(9))), q(static_cast<long long>(g.below(21)) - 3, 4), ind});
      }
      bool survivor = false;
      for (const auto& f : p.before)
        for (const auto& h : p.after) survivor = survivor || f.id == h.id;
      if (!survivor || p.before.empty() || p.after.empty()) continue;
      for (Method m : {Method::bhc, Method::gr, Method::fhk, Method::dopd, Method::price}) {
        const auto R = decompose(p, m);
        sums = sums && total(R) == R.change && R.change == R.after - R.before;
        if (m == Method::price) {
          levels = levels && R.terms.size() == 5;
          with_industries++;
        }
      }
    }
    s.require("random panels with industries: every method sums to ΔΦ in exact rationals", sums && with_industries >= 100, std::to_string(with_industries) + " panels");
    s.require("... Price's selection split between industries and within them", levels);
  }
  {
    Panel<double> p;
    p.before = {{"a", 1, std::exp(1.0), ""}, {"b", 1, std::exp(2.0), ""}};
    p.after = {{"a", 1, std::exp(2.0), ""}, {"b", 1, std::exp(2.0), ""}};
    const auto R = decompose(logs(p), Method::fhk);
    s.check("a panel in levels carried to logs: the aggregate log productivity rises from 1.5 to 2", std::fabs(R.before - 1.5) + std::fabs(R.after - 2), 1e-12);
    auto bad = p;
    bad.before[0].productivity = 0;
    s.require("a productivity in levels that is not positive is refused before logs", refused([&] { logs(bad); }));
  }
  {
    Panel<Q> p = hand();
    p.before[1].id = "A";
    s.require("two firms with one identity at a date are refused", refused([&] { decompose(p, Method::fhk); }));
    Panel<Q> none;
    none.before = {{"A", q(1), q(1), ""}};
    none.after = {{"B", q(1), q(1), ""}};
    s.require("no survivor: the dynamic Olley–Pakes decomposition is refused, FHK still answers",
              refused([&] { decompose(none, Method::dopd); }) && decompose(none, Method::fhk).change == Q(0));
  }
  return s.done();
}
