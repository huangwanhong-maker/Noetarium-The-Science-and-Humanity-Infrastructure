// The heat a massing needs over a day, from what its faces are open to and the Sun they take (ADR 0089): a balance of the envelope,
// coarser than heat.hpp's rooms and massing_heat.hpp's buildings, and quick enough to score thousands of designs.
//
// THE BALANCE. The inside is held at t_in. The envelope is its walls, sampled by face receivers (sun.hpp), each standing for an area,
// and its roofs. At each instant k of the day, standing for w_k hours with the air at t_k:
//   loss_k  = (Σ_open walls U_facade a + U_roof A_roof)(t_in − t_k),
//   gain_k  = Σ_open, sunlit walls a I_k cos θ (g γ + (1 − g) U_facade α / h_o) + A_roof I_k sin α_k U_roof α / h_o,
//   need_k  = max(0, loss_k − gain_k) w_k,
// with I_k Meinel's clear-sky irradiance at the Sun's altitude α_k, θ the angle between the wall's normal and the Sun, g the glazed
// share, γ the glass's gain coefficient, α the fabric's absorptance and h_o its outer coefficient: the sol-air gain of heat.hpp, with
// the same fabric (space::Fabric). A wall is open when no other building stands against it (covering, sun.hpp). The rest of the day,
// the night, loses at its own air temperature and gains nothing. The day's need is Σ need_k plus the night's, in Wh.
//
// Left out: the heat stored in the fabric from one hour to the next (massing_heat.hpp holds it), the air exchanged, internal gains,
// the sky's diffuse light, roofs shaded by taller neighbours (they take the Sun unshaded). A design's need per m² of floor is a
// comparison between designs at one place, not a prediction of its bills.

#pragma once

#include <architecture/environment/sun.hpp>
#include <architecture/space/rooms.hpp>

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace architecture::environment {

struct Day {
  std::vector<double> air;   // the air (°C) at each of the Shading's instants
  double night_air = 5;      // the air over the rest of the day (°C)
  double night_hours = 0;    // the hours of the day the instants do not stand for
  double inside = 20;        // held (°C)
};

// The day's need (Wh) of an envelope: `walls` its face receivers (indices into S.receivers) with the area each stands for, `roof` the
// area of its roofs.
inline double heat_need(const Shading& S, const Cover& C, const std::vector<double>& heights, const std::vector<std::pair<std::size_t, double>>& walls, double roof,
                        const Day& day, const space::Fabric& F = {}) {
  const double Uf = F.u.at("facade"), Ur = F.u.at("roof");
  const double opaque_f = Uf * F.absorptance / F.outer_coefficient, opaque_r = Ur * F.absorptance / F.outer_coefficient;
  const double through = F.glazing * F.gain_coefficient + (1 - F.glazing) * opaque_f;
  double open = 0;
  for (const auto& [r, a] : walls)
    if (exposed(C, S.receivers, r, heights)) open += a;
  const double conductance = Uf * open + Ur * roof;
  double need = std::max(0.0, conductance * (day.inside - day.night_air)) * day.night_hours;
  for (std::size_t k = 0; k < S.suns.size(); k++) {
    const auto& sun = S.suns[k];
    const double air = k < day.air.size() ? day.air[k] : day.night_air, I = clear_sky(sun.altitude);
    double gain = I > 0 ? roof * I * std::sin(sun.altitude) * opaque_r : 0.0;
    if (I > 0) {
      const double ux = std::sin(sun.azimuth) * std::cos(sun.altitude), uy = std::cos(sun.azimuth) * std::cos(sun.altitude);
      for (const auto& [r, a] : walls) {
        const auto& p = S.receivers[r];
        const double cosine = p.nx * ux + p.ny * uy;
        if (cosine > 0 && exposed(C, S.receivers, r, heights) && sunlit(S, r, k, heights)) gain += a * I * cosine * through;
      }
    }
    need += std::max(0.0, conductance * (day.inside - air) - gain) * sun.weight;
  }
  return need;
}

}  // namespace architecture::environment
