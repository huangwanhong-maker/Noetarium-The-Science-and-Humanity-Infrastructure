// Checks of response (response.hpp).
//
// - A street of 1 km with stations at its two ends, a threshold of 40 s at 10 m/s: places within 400 m of an end are covered, those
//   in the middle are not; each station reaches its own and alone those the other does not; with stations at 0 and 300 m the places
//   within 400 m of both are covered twice.
// - Where to add stations on a lattice under a hazard's scenarios: the greedy sites reach at least 1 − 1/e of the best pair, listed;
//   the bound is above the best; adding sites never lowers coverage (a threshold of 90 s, the gap to the street walked).
// - Refusals: no station, a threshold that is not positive.

#include <urban_risk_management/response/response.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <stdexcept>
#include <vector>

namespace ll = urban_risk_management::lifelines;
namespace ur = urban_risk_management::response;
namespace rc = risk_management::criticality;

namespace {
bool throws(auto&& f) {
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
  ll::GraphOptions o;
  o.stitch = 0;
  {
    ll::Road street{{}, 10, 8, "street"};
    for (int i = 0; i <= 20; i++) street.xy.push_back(50.0 * i), street.xy.push_back(0);
    const auto g = ll::graph_of({street}, o);
    std::vector<ll::Place> people;
    for (int i = 0; i <= 20; i++) people.push_back({50.0 * i, 0, 1});
    const ur::Response R(g, people, {{0, 0, 1}, {1000, 0, 1}}, 40);
    const auto C = R.cover(ll::State(g.links.size(), 1.0));
    s.require("places within 400 m of an end are covered, the middle 200 m not: 18 of 21", C.covered == 18 && C.count[10] == 0 && C.count[8] == 1);
    s.require("no place is within 400 m of both ends: none covered twice", C.backup == 0);
    s.require("each station reaches 9 and alone reaches 9", C.reach[0] == 9 && C.reach[1] == 9 && C.only[0] == 9 && C.only[1] == 9);
    const ur::Response R2(g, people, {{0, 0, 1}, {300, 0, 1}}, 40);
    const auto C2 = R2.cover(ll::State(g.links.size(), 1.0));
    s.require("with stations at 0 and 300 m, the 9 places within 400 m of both are covered twice", C2.backup == 9 && C2.count[3] == 2 && C2.count[9] == 1);
  }
  {
    calx::random::Stream rng(29, 7, 8, 9);
    std::vector<ll::Road> roads;
    const int k = 6;
    for (int i = 0; i < k; i++) {
      ll::Road h{{}, 10, 8, ""}, v{{}, 10, 8, ""};
      for (int j = 0; j < k; j++) h.xy.push_back(j * 100.0), h.xy.push_back(i * 100.0), v.xy.push_back(i * 100.0), v.xy.push_back(j * 100.0);
      roads.push_back(h), roads.push_back(v);
    }
    const auto g = ll::graph_of(roads, o);
    std::vector<ll::Place> people, candidates;
    for (int i = 0; i < 50; i++) people.push_back({rng.uniform(0, 500), rng.uniform(0, 500), rng.uniform(1, 9)});
    for (int i = 0; i < 9; i++) candidates.push_back({rng.uniform(0, 500), rng.uniform(0, 500), 1});
    const ur::Response R(g, people, {{0, 0, 1}}, 90);
    std::vector<rc::Scenario> sc{{"dry", 0.7, {}}};
    ll::State phi(g.links.size(), 1.0);
    for (double& x : phi) x = rng.uniform() < 0.25 ? 0.0 : 1.0;
    sc.push_back({"flood", 0.3, phi});
    const auto S = R.site(candidates, 2, sc);
    // The best pair by listing every pair, with the same expected coverage.
    double best = 0;
    for (std::size_t a = 0; a < candidates.size(); a++)
      for (std::size_t b = a + 1; b < candidates.size(); b++) {
        const auto one = R.site({candidates[a], candidates[b]}, 2, sc);
        best = std::max(best, one.total());
      }
    s.require("the greedy pair reaches at least 1 − 1/e of the best pair, its bound above the best",
              S.total() >= S.guarantee * best - 1e-9 && S.bound >= best - 1e-9 && S.total() <= best + 1e-9);
    bool rising = true;
    for (std::size_t i = 0; i < S.value.size(); i++) rising = rising && S.value[i] >= (i ? S.value[i - 1] : S.existing);
    s.require("adding sites never lowers the expected coverage", rising && !S.value.empty());
  }
  {
    const auto g = ll::graph_of({{{0, 0, 100, 0}, 10, 8, ""}}, o);
    s.require("refusals: no station, a threshold that is not positive",
              throws([&] { ur::Response(g, {{0, 0, 1}}, {}, 10); }) && throws([&] { ur::Response(g, {{0, 0, 1}}, {{0, 0, 1}}, 0); }));
  }
  return s.done();
}
