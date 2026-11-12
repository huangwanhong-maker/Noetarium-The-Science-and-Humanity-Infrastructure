// How high a building may rise before it takes the sun from where the sun is wanted (ADR 0098): Knowles's solar envelope, for one
// building at a time, on sun.hpp's thresholds.
//
// A set of receivers (the points of a footpath, a garden, a neighbour's windows), the Sun at the instants of a day, and prisms of
// which some are variables. sun.hpp gives, for each receiver and instant, the thresholds θ: a variable shades the receiver at that
// instant exactly when its height exceeds θ. Holding every other variable at its height, the hours of sun of receiver r as a
// function of one variable's height h are a step function that never rises: an instant at which nothing else shades r is lost
// when h passes its threshold. A receiver is PROTECTED when it has at least `need` hours as things stand: what is asked is that it
// keep them (a footpath at a building's own foot, which that building already shades, asks nothing of it).
// The LIMIT of the variable is the greatest height at which every protected receiver keeps `need` hours: for each protected
// receiver, the thresholds of its instants taken from the highest down, their hours summed with the hours no height of this
// variable can take; the height is the threshold at which the sum first reaches `need`. The limit is the least of these over the
// receivers, +∞ when the variable reaches none of them, never below the variable's present height, and exact for prisms on flat
// ground at the instants given.
//
// What a variable TAKES as it stands is the sum, over all the receivers, of the hours at which it alone shades them: the sun they
// would have were it gone. A building raised past its limit has none (what it shaded is no longer protected), and takes more.
//
// It is one building's limit, the others as they are: two buildings each raised to its own limit may together take what neither
// takes alone. A design of several heights is checked whole (sun_hours), or searched for (heuresis).
//
// R. L. Knowles, Sun Rhythm Form (MIT Press, 1981), for the solar envelope: the largest volume on a site that does not shade its
// neighbours over given hours.

#pragma once

#include <architecture/environment/sun.hpp>

#include <algorithm>
#include <limits>
#include <utility>
#include <vector>

namespace architecture::environment {

struct SolarLimit {
  double height = std::numeric_limits<double>::infinity();   // the greatest height that keeps the sun; +∞ when it reaches no protected receiver
  std::size_t receiver = static_cast<std::size_t>(-1);       // the receiver that sets it
  double taken = 0;                                          // the hours it takes as it stands, summed over the receivers: sun they would have were it gone
  std::size_t reached = 0;                                   // the protected receivers it could shade at some instant, were it higher
};

// The limit of each variable, the others held at `heights`; `need` in hours.
inline std::vector<SolarLimit> solar_limits(const Shading& S, const std::vector<double>& heights, double need) {
  const std::size_t R = S.receivers.size(), K = S.suns.size(), V = heights.size();
  std::vector<SolarLimit> out(V);
  std::vector<std::vector<std::pair<double, double>>> steps(V);   // for the receiver in hand: (threshold, hours) of each variable
  std::vector<double> free(V);
  for (std::size_t r = 0; r < R; r++) {
    for (std::size_t v = 0; v < V; v++) steps[v].clear(), free[v] = 0;
    double open = 0;   // the hours r is in the sun at the present heights
    for (std::size_t k = 0; k < K; k++) {
      if (S.fixed_shaded[r * K + k]) continue;
      const auto& blockers = S.blockers[r * K + k];
      bool lit = true;
      for (const auto& [v, theta] : blockers) lit = lit && !(heights[v] > theta);
      if (lit) open += S.suns[k].weight;
      // For a variable v among the blockers: when no other variable shades r at this instant, v takes it above its threshold
      // (the least of its thresholds here, were it to stand as several prisms). When another does, the instant is dark whatever
      // v's height; for a variable not among the blockers it is lit or dark whatever its height.
      for (const auto& [v, theta] : blockers) {
        bool others = false, least = true;
        for (const auto& [u, other] : blockers) {
          if (u != v) others = others || heights[u] > other;
          else least = least && !(other < theta);
        }
        if (!others && least) steps[v].push_back({static_cast<double>(theta), S.suns[k].weight});
      }
    }
    for (std::size_t v = 0; v < V; v++) {
      if (steps[v].empty()) continue;
      // The hours r has whatever v does: the instants lit now less those among v's steps that are lit now.
      double own_lit = 0;
      for (const auto& [theta, w] : steps[v]) {
        if (!(heights[v] > theta)) own_lit += w;
        else out[v].taken += w;
      }
      const double fixed = open - own_lit;
      if (open < need) continue;   // not protected: it has not the hours as things stand
      out[v].reached++;
      std::sort(steps[v].begin(), steps[v].end(), [](const auto& a, const auto& b) { return a.first > b.first; });
      double sum = fixed, limit = std::numeric_limits<double>::infinity();
      if (sum < need)
        for (const auto& [theta, w] : steps[v]) {
          sum += w;
          if (sum >= need) { limit = theta; break; }
        }
      if (limit < out[v].height) out[v].height = limit, out[v].receiver = r;
    }
  }
  return out;
}

}  // namespace architecture::environment
