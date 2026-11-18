// Response: who stays within reach of a fire station, an ambulance post or a hospital, in each state of the roads, and where another
// would reach most (theory/lifelines-and-response.md §4).
//
// COVERAGE. A place is covered when a station lies within the threshold time over the roads at their state (lifelines' graph, times
// and attachments). Beside who is covered: who is covered twice (backup coverage, Hogan and ReVelle 1986, Management Science 32,
// 1434), how many each station reaches, and how many only it reaches, so that its loss leaves them uncovered.
//
// WHERE TO ADD STATIONS. Coverage over the scenarios of a hazard, f(S) = Σ_s w_s Σ_p people_p [some station of the existing ∪ S
// reaches p within the threshold in s], is monotone and submodular in the set S of new sites (a weighted coverage, summed over
// scenarios), so the greedy choice reaches at least 1 − 1/e of the best k sites and reports a bound from the instance (calx's
// greedy_submodular; the maximal covering location problem of Church and ReVelle 1974, Papers of the Regional Science Association 32,
// 101, under the states a hazard puts the roads in).

#pragma once

#include <urban_risk_management/lifelines/lifelines.hpp>

#include <calx/optimisation/submodular.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <functional>
#include <stdexcept>
#include <string>
#include <vector>

namespace urban_risk_management::response {

namespace ll = urban_risk_management::lifelines;
namespace rc = risk_management::criticality;

struct Coverage {
  double people = 0;               // all the people
  double covered = 0;              // within the threshold of at least one station
  double backup = 0;               // within it of at least two
  double share = 0;                // covered / people
  std::vector<double> reach;       // per station: the people it reaches
  std::vector<double> only;        // per station: the people it alone reaches
  std::vector<int> count;          // per place: the stations that reach it
};

struct Siting {
  std::vector<std::size_t> chosen;   // candidate sites, in the order chosen
  std::vector<double> value;         // the expected people covered after each choice
  double existing = 0;               // with the existing stations alone
  double bound = 0;                  // the best k sites cover at most this
  double guarantee = 0;              // 1 − 1/e
  double total() const { return value.empty() ? existing : value.back(); }  // the greedy stops when no site gains
};

class Response {
 public:
  Response(ll::Graph g, const std::vector<ll::Place>& people, const std::vector<ll::Place>& stations, double threshold, double gap_speed = 1.34)
      : g_(std::move(g)), threshold_(threshold), gap_speed_(gap_speed), locate_(g_) {
    if (people.empty() || stations.empty()) throw std::invalid_argument("urban_risk_management::response: people and at least one station");
    if (!(threshold > 0)) throw std::invalid_argument("urban_risk_management::response: a threshold > 0");
    for (const auto& p : people) {
      people_.push_back(ll::attach(g_, locate_, p.x, p.y, gap_speed));
      weight_.push_back(p.weight);
    }
    for (const auto& s : stations) stations_.push_back(ll::attach(g_, locate_, s.x, s.y, gap_speed));
  }

  const ll::Graph& graph() const { return g_; }
  const std::vector<ll::Attachment>& stations() const { return stations_; }

  // Which places a site reaches within the threshold at a state.
  std::vector<char> reached(const ll::State& phi, const ll::Attachment& site) const {
    if (phi.size() != g_.links.size()) throw std::invalid_argument("urban_risk_management::response: a state gives a share for each link");
    const std::vector<ll::Attachment> one{site};
    std::vector<std::vector<std::size_t>> on(g_.links.size());
    if (site.link != ll::none) on[site.link].push_back(0);
    const auto F = ll::field(g_, phi, one, threshold_);
    std::vector<char> out(people_.size(), 0);
    for (std::size_t i = 0; i < people_.size(); i++) out[i] = ll::time_of(g_, phi, F, people_[i], on, one) <= threshold_;
    return out;
  }

  Coverage cover(const ll::State& phi) const {
    Coverage c;
    c.count.assign(people_.size(), 0);
    std::vector<std::vector<char>> R;
    for (const auto& s : stations_) R.push_back(reached(phi, s));
    for (std::size_t k = 0; k < stations_.size(); k++)
      for (std::size_t i = 0; i < people_.size(); i++) c.count[i] += R[k][i];
    c.reach.assign(stations_.size(), 0);
    c.only.assign(stations_.size(), 0);
    for (std::size_t i = 0; i < people_.size(); i++) {
      c.people += weight_[i];
      if (c.count[i] >= 1) c.covered += weight_[i];
      if (c.count[i] >= 2) c.backup += weight_[i];
      for (std::size_t k = 0; k < stations_.size(); k++)
        if (R[k][i]) {
          c.reach[k] += weight_[i];
          if (c.count[i] == 1) c.only[k] += weight_[i];
        }
    }
    c.share = c.people > 0 ? c.covered / c.people : 0;
    return c;
  }

  // The k candidate sites (places) that most raise the expected coverage over the scenarios.
  Siting site(const std::vector<ll::Place>& candidates, std::size_t k, const std::vector<rc::Scenario>& scenarios) const {
    if (candidates.empty() || scenarios.empty()) throw std::invalid_argument("urban_risk_management::response::site: candidates and scenarios");
    std::vector<ll::Attachment> sites;
    for (const auto& c : candidates) sites.push_back(ll::attach(g_, locate_, c.x, c.y, gap_speed_));
    const std::size_t n = people_.size(), S = scenarios.size(), C = sites.size();
    std::vector<double> w;
    std::vector<std::vector<char>> base(S, std::vector<char>(n, 0));
    std::vector<std::vector<std::vector<char>>> by(S);
    for (std::size_t s = 0; s < S; s++) {
      const ll::State phi = scenarios[s].state.empty() ? ll::State(g_.links.size(), 1.0) : scenarios[s].state;
      w.push_back(scenarios[s].weight);
      for (const auto& st : stations_) {
        const auto r = reached(phi, st);
        for (std::size_t i = 0; i < n; i++) base[s][i] = base[s][i] || r[i];
      }
      for (const auto& site : sites) by[s].push_back(reached(phi, site));
    }
    const std::function<double(const std::vector<std::size_t>&)> f = [&](const std::vector<std::size_t>& set) {
      double total = 0;
      for (std::size_t s = 0; s < S; s++)
        for (std::size_t i = 0; i < n; i++) {
          bool hit = base[s][i];
          for (std::size_t j = 0; !hit && j < set.size(); j++) hit = by[s][set[j]][i];
          if (hit) total += w[s] * weight_[i];
        }
      return total;
    };
    const auto G = calx::optimisation::greedy_submodular(C, f, std::min(k, C));
    Siting out;
    out.chosen = G.chosen;
    out.value = G.value;
    out.existing = G.empty;
    out.bound = G.bound;
    out.guarantee = G.guarantee;
    return out;
  }

 private:
  ll::Graph g_;
  double threshold_, gap_speed_;
  ll::Locator locate_;
  std::vector<ll::Attachment> people_, stations_;
  std::vector<double> weight_;
};

}  // namespace urban_risk_management::response
