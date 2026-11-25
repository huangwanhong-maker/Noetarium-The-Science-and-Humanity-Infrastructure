// Checks of circulation: the six access rules of the design classified (reputation earned and spent is a personal purchasing-power
// gate); provision by need hands out every unit it can and serves the least served first; the market refuses a buyer short of
// tokens and conserves tokens; the mediation profile is read from a chronicle.

#include <communities/circulation/circulation.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numeric>
#include <string>

using namespace communities::circulation;

int main() {
  calx::verification::Suite s;
  const std::vector<std::pair<std::vector<Condition>, Gate>> cases = {
      {{{"token", true}}, Gate::purchasing_power},
      {{{"reputation", true}}, Gate::purchasing_power},
      {{{"count of contributions", false}}, Gate::standing},
      {{{"membership", false}, {"rotation", false}}, Gate::membership},
      {{{"need", false}, {"lot", false}}, Gate::allocation},
      {{}, Gate::open},
  };
  int right = 0;
  for (const auto& [rule, gate] : cases) right += gate_of(rule) == gate;
  s.require("six access rules are classified as the design classifies them; reputation earned by work and spent on priority is a personal purchasing-power gate", right == 6);
  s.require("a duty owed in proportion to one's known capacity, read as conduct, is a membership gate and spends nothing", gate_of({{"conduct", false}}) == Gate::membership);

  calx::random::Stream rng(9, 1, 0, 0);
  {
    // Two providers of kind 0 with 3 units; four parties each needing two units of kind 0.
    const std::vector<Offer> offers{{0, 0, 2}, {1, 0, 1}};
    std::vector<Need> needs;
    for (std::size_t p = 2; p < 6; p++) needs.push_back({p, 0}), needs.push_back({p, 0});
    const auto d = provide(offers, needs, 6, rng);
    std::vector<int> got(6, 0);
    for (const auto& x : d) got[x.receiver]++;
    const int most = *std::max_element(got.begin() + 2, got.end()), least = *std::min_element(got.begin() + 2, got.end());
    s.require("provision by need hands out every unit offered and no party receives a second unit before every party has a first", d.size() == 3 && most == 1 && least == 0);
    std::vector<bool> excluded(6, false);
    excluded[2] = true;
    const auto e = provide(offers, needs, 6, rng, excluded);
    bool none = true;
    for (const auto& x : e) none = none && x.receiver != 2;
    s.require("a suspended party receives nothing", none && e.size() == 3);
  }
  {
    std::vector<double> tokens{0, 1, 5};
    const std::vector<Offer> offers{{2, 0, 4}};
    const std::vector<Need> needs{{0, 0}, {1, 0}, {1, 0}};
    const double before = std::accumulate(tokens.begin(), tokens.end(), 0.0);
    const auto sale = market(offers, needs, tokens, 1.0, rng);
    s.require("the market refuses whoever holds too few tokens, and every such refusal rests on a balance", sale.deliveries.size() == 1 && sale.refused_for_balance == 2);
    s.require("tokens are conserved by a sale", std::abs(std::accumulate(tokens.begin(), tokens.end(), 0.0) - before) < 1e-12);
  }
  {
    thesmos::chronicle::State st;
    st.commitments["a"] = {"a", "x", "y", "provision", "", "", "", 1, 0, thesmos::commitments::Status::discharged};
    st.commitments["b"] = {"b", "x", "y", "provision", "", "", "", 1, 0, thesmos::commitments::Status::discharged};
    st.commitments["c"] = {"c", "x", "y", "gift", "", "", "", 1, 0, thesmos::commitments::Status::discharged};
    st.commitments["d"] = {"d", "x", "y", "gift", "", "", "", 1, 0, thesmos::commitments::Status::active};
    const auto profile = mediation_profile(st);
    s.require("the mediation profile counts discharges by mode: two thirds provision, a third gift, the open commitment aside",
              std::abs(profile.at("provision") - 2.0 / 3.0) < 1e-12 && std::abs(profile.at("gift") - 1.0 / 3.0) < 1e-12);
  }
  return s.done();
}
