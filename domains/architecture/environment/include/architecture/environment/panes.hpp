// Sun and sky at a building's glass (ADR 0097): for each pane of a wall, the hours of sun on it over the instants of a day, the
// clear-sky energy that falls on it, and the angle of sky it sees; and from those a room's average daylight factor.
//
// THE MODEL. A pane is a rectangle of glass in a vertical face: the middle of its foot on the face (x, y, z0), its head z1, its
// width, and the face's horizontal outward normal n. Above it there may be an overhang (an eave): a ledge standing `overhang`
// metres out from the face with its underside at overhang_z. What stands about it is sun.hpp's: prisms on flat ground. The
// building's own footprint is given among the prisms; a pane's points stand 0.05 m out from the face, so that a wing of the same
// building shades it as any other prism does.
//
// SUN. A pane is sampled at nine points (three across, three up). A point is in the sun at an instant when the Sun is up and in
// front of the face, the ray toward it clears every prism (sun.hpp's thresholds), and the ray clears the overhang: it reaches the
// underside's height after a horizontal run (z_o − z) / tan α, of which (n · u) is square to the face; it passes outside the ledge
// when that exceeds the overhang. The pane's HOURS are the instants' weights times the share of its points in the sun; its sun is
// counted FULL at an instant when its middle point is lit (the hours a rule of sunlight counts).
//
// ENERGY. On a clear day the direct beam at normal incidence is I(α) = 1353 × 0.7^(AM^0.678) W/m², AM = 1 / sin α (Meinel and
// Meinel 1976; sun.hpp's clear_sky). On a vertical pane it falls at cos α (n · u), so a pane's direct energy over the day is
// Σ I cos α (n · u) × lit share × hours, in Wh per m² of glass. The sky's diffuse light and what the ground reflects are left out:
// the figure is a bound from below on a clear day, and says so.
//
// SKY. In the vertical plane square to the face, from the pane's middle: the sky is hidden below by what stands before it, up to
// the angle ε_low = max over prisms of atan((h − z) / t), t the distance along n at which the ray enters the prism; and above by
// the overhang, beyond ε_high = atan((z_o − z) / overhang), or 90° with none. The ANGLE OF VISIBLE SKY is θ = max(0, ε_high − ε_low).
//
// THE AVERAGE DAYLIGHT FACTOR of a room (Crisp and Littlefair 1984; BRE Digest 309, 1986; BS 8206-2:2008, annex): the mean
// illuminance on the working plane as a share of that outdoors under an overcast sky,
//   DF = T Σ_w (A_w θ_w) / (A (1 − R²))  per cent, θ in degrees,
// A_w a window's glass, T the glass's diffuse transmittance (with dirt), A the area of all the room's surfaces (floor, ceiling and
// walls, the windows among them), R their mean reflectance. It is an estimate for a room lit from the side that is not deep; it
// says nothing of where in the room the light falls. BS 8206-2 is withdrawn for BS EN 17037:2018, which asks for illuminances
// reached over a share of the year; a limit on the factor is a scheme's to state.

#pragma once

#include <architecture/environment/sun.hpp>

#include <algorithm>
#include <cmath>
#include <numbers>
#include <vector>

namespace architecture::environment {

struct Pane {
  double x = 0, y = 0;       // the middle of its foot, on the face
  double z0 = 0, z1 = 0;     // its foot and its head above the ground
  double width = 0;
  double nx = 0, ny = 0;     // the face's outward normal, a unit vector
  double overhang = 0;       // a ledge above it, metres out from the face; 0 for none
  double overhang_z = 0;     // the height of the ledge's underside
};

struct PaneReading {
  double hours = 0;          // of sun, by the share of the pane lit
  double full_hours = 0;     // of sun on its middle
  double energy = 0;         // direct, on a clear day: Wh per m² of glass
  double low = 0, high = std::numbers::pi / 2, sky = std::numbers::pi / 2;  // radians
};

namespace detail {
inline bool under_overhang(const Pane& p, double z, const Sun& sun) {
  if (!(p.overhang > 0) || !(p.overhang_z > z)) return false;
  const double across = p.nx * std::sin(sun.azimuth) + p.ny * std::cos(sun.azimuth);
  return (p.overhang_z - z) / std::tan(sun.altitude) * across < p.overhang;
}
}  // namespace detail

inline std::vector<PaneReading> panes(const std::vector<Pane>& glass, const std::vector<Sun>& suns, const std::vector<Prism>& prisms, double reach = 400) {
  constexpr double out = 0.05;
  std::vector<Receiver> receivers;
  receivers.reserve(glass.size() * 9);
  for (const Pane& p : glass)
    for (int i = 0; i < 3; i++)
      for (int j = 0; j < 3; j++) {
        const double s = p.width * (i - 1) / 3.0, z = p.z0 + (p.z1 - p.z0) * (2 * j + 1) / 6.0;
        receivers.push_back({p.x - p.ny * s + p.nx * out, p.y + p.nx * s + p.ny * out, z, p.nx, p.ny, -1});
      }
  const Shading S = shading(receivers, suns, prisms, reach);
  const std::vector<double> none;
  std::vector<PaneReading> R(glass.size());
  for (std::size_t g = 0; g < glass.size(); g++) {
    const Pane& p = glass[g];
    for (std::size_t k = 0; k < suns.size(); k++) {
      int lit = 0;
      bool middle = false;
      for (int c = 0; c < 9; c++) {
        const std::size_t r = g * 9 + c;
        if (!sunlit(S, r, k, none) || detail::under_overhang(p, S.receivers[r].z, suns[k])) continue;
        lit++;
        if (c == 4) middle = true;
      }
      if (!lit) continue;
      const double share = lit / 9.0, across = p.nx * std::sin(suns[k].azimuth) + p.ny * std::cos(suns[k].azimuth);
      R[g].hours += share * suns[k].weight;
      if (middle) R[g].full_hours += suns[k].weight;
      R[g].energy += clear_sky(suns[k].altitude) * std::cos(suns[k].altitude) * across * share * suns[k].weight;
    }
    // The sky it sees, in the plane square to the face.
    const double z = (p.z0 + p.z1) / 2;
    const cg::Point2 from{p.x + p.nx * out, p.y + p.ny * out}, n{p.nx, p.ny};
    for (const Prism& b : prisms) {
      const double t = cg::ray_entry(b.ring, from, n);
      if (!std::isfinite(t) || t > reach || !(b.height > z)) continue;
      R[g].low = std::max(R[g].low, std::atan2(b.height - z, std::max(t, 1e-6)));
    }
    if (p.overhang > 0 && p.overhang_z > z) R[g].high = std::atan2(p.overhang_z - z, p.overhang);
    R[g].sky = std::max(0.0, R[g].high - R[g].low);
  }
  return R;
}

// The average daylight factor, per cent: `glass_sky` is Σ A_w θ_w with θ in degrees, `surfaces` the area of all the room's
// surfaces, T the glass's diffuse transmittance, R the surfaces' mean reflectance.
inline double daylight_factor(double glass_sky, double surfaces, double transmittance = 0.68, double reflectance = 0.5) {
  if (!(surfaces > 0) || !(reflectance >= 0 && reflectance < 1)) return 0;
  return transmittance * glass_sky / (surfaces * (1 - reflectance * reflectance));
}

}  // namespace architecture::environment
