// Sun and shadow on the ground and the faces of buildings (ADR 0089), promoted from the district study's provisional model
// (research/experiments/2026-11-05-designing-a-district/sunlight.hpp) and widened to faces.
//
// THE MODEL. Buildings are prisms on flat ground: a footprint ring (east and north in metres) standing from 0 to its height. The Sun is
// a direction: azimuth A from north through east and altitude α above the horizon, with the hours the instant stands for; the caller
// gives it (the astronomy domain's position of the Sun, at the place and the instants chosen). A receiver is a point at height z: on
// the ground (z = 0), or on a face with a horizontal outward normal n. It is in the sun when
// - on a face, the Sun is in front of it: n · (sin A, cos A) > 0;
// - the ray toward the Sun, p + t (sin A, cos A, tan α) for t ≥ 0 (t the horizontal distance), clears every prism.
// The ray rises as it goes, so it meets a prism of height h exactly when its height where it first enters the footprint is below the
// top: z + t_in tan α < h, with t_in the least t at which it crosses the footprint's ring (physis's ray_entry). A receiver's shading
// at an instant by a building is therefore the test h > θ, with the threshold θ = z + t_in tan α fixed by the geometry: thresholds are
// computed once, and a design that changes only heights is evaluated by comparisons. A receiver on a face is never shaded by its own
// building (its owner).
//
// A receiver on a face may also lie against another building (a party wall): it is COVERED, and so neither loses heat to the air nor
// takes the Sun, while it lies inside another prism's footprint below that prism's top, again a threshold on a height (covering).
//
// Left out: light from the sky and light reflected, trees and awnings, the ground's relief, buildings beyond those given. Exact for
// prisms on flat ground at the receivers given.

#pragma once

#include <physis/computational_geometry/planar.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

namespace architecture::environment {

namespace cg = physis::computational_geometry;
using Ring = std::vector<cg::Point2>;

struct Sun {
  double altitude = 0, azimuth = 0;  // radians; azimuth from north through east
  double weight = 1;                 // the hours this instant stands for
};

// A building: its footprint, and either a fixed height or the index of a variable height in a design.
struct Prism {
  Ring ring;
  double height = 0;
  int variable = -1;
};

struct Receiver {
  double x = 0, y = 0, z = 0;
  double nx = 0, ny = 0;    // a face's horizontal outward normal; (0, 0) on the ground
  int owner = -1;           // the prism whose face it lies on, never shading it; −1 on the ground
  bool on_face() const { return nx != 0 || ny != 0; }
};

struct Shading {
  std::vector<Receiver> receivers;
  std::vector<Sun> suns;
  std::vector<std::uint8_t> fixed_shaded;   // [r · K + k]: shaded at instant k whatever the design (the Sun down or behind the face, or a fixed prism)
  std::vector<std::vector<std::pair<std::uint32_t, float>>> blockers;  // [r · K + k]: (variable, θ): shaded when that height exceeds θ
};

// The thresholds of every receiver at every instant; `reach` bounds the horizontal distance searched (m).
inline Shading shading(std::vector<Receiver> receivers, std::vector<Sun> suns, const std::vector<Prism>& prisms, double reach) {
  if (!(reach > 0)) throw std::invalid_argument("environment (sun): a reach, positive");
  Shading S;
  S.receivers = std::move(receivers), S.suns = std::move(suns);
  std::vector<std::array<double, 4>> box(prisms.size());
  for (std::size_t c = 0; c < prisms.size(); c++) {
    box[c] = {1e300, 1e300, -1e300, -1e300};
    for (const auto& q : prisms[c].ring) box[c] = {std::min(box[c][0], q[0]), std::min(box[c][1], q[1]), std::max(box[c][2], q[0]), std::max(box[c][3], q[1])};
  }
  const std::size_t R = S.receivers.size(), K = S.suns.size();
  S.fixed_shaded.assign(R * K, 0);
  S.blockers.assign(R * K, {});
  for (std::size_t r = 0; r < R; r++) {
    const Receiver& p = S.receivers[r];
    for (std::size_t k = 0; k < K; k++) {
      const Sun& sun = S.suns[k];
      const cg::Point2 u{std::sin(sun.azimuth), std::cos(sun.azimuth)};
      if (!(sun.altitude > 0) || (p.on_face() && p.nx * u[0] + p.ny * u[1] <= 0)) { S.fixed_shaded[r * K + k] = 1; continue; }
      const double tana = std::tan(sun.altitude);
      const double bx0 = std::min(p.x, p.x + reach * u[0]), bx1 = std::max(p.x, p.x + reach * u[0]);
      const double by0 = std::min(p.y, p.y + reach * u[1]), by1 = std::max(p.y, p.y + reach * u[1]);
      for (std::size_t c = 0; c < prisms.size(); c++) {
        if (static_cast<int>(c) == p.owner) continue;
        if (box[c][2] < bx0 || box[c][0] > bx1 || box[c][3] < by0 || box[c][1] > by1) continue;
        const double t = cg::ray_entry(prisms[c].ring, {p.x, p.y}, u);
        if (!std::isfinite(t) || t > reach) continue;
        const double theta = p.z + t * tana;
        if (prisms[c].variable < 0) {
          if (prisms[c].height > theta) S.fixed_shaded[r * K + k] = 1;
        } else {
          S.blockers[r * K + k].push_back({static_cast<std::uint32_t>(prisms[c].variable), static_cast<float>(theta)});
        }
      }
    }
  }
  return S;
}

// Whether receiver r is in the sun at instant k under a design's heights.
inline bool sunlit(const Shading& S, std::size_t r, std::size_t k, const std::vector<double>& heights) {
  const std::size_t K = S.suns.size();
  if (S.fixed_shaded[r * K + k]) return false;
  for (const auto& [v, theta] : S.blockers[r * K + k])
    if (heights[v] > theta) return false;
  return true;
}

// The hours of sun of each receiver over the instants.
inline std::vector<double> sun_hours(const Shading& S, const std::vector<double>& heights) {
  std::vector<double> out(S.receivers.size(), 0.0);
  for (std::size_t r = 0; r < S.receivers.size(); r++)
    for (std::size_t k = 0; k < S.suns.size(); k++)
      if (sunlit(S, r, k, heights)) out[r] += S.suns[k].weight;
  return out;
}

// Which face receivers lie against another prism: inside its footprint (the receiver stands 0.05 m out from its own face) below
// its top. A fixed prism covers or not; a variable one covers while its height exceeds the receiver's.
struct Cover {
  std::vector<std::uint8_t> fixed;            // [r]: covered whatever the design
  std::vector<std::vector<std::uint32_t>> by; // [r]: the variables whose prisms hold the receiver's point
};

inline Cover covering(const std::vector<Receiver>& receivers, const std::vector<Prism>& prisms) {
  Cover C;
  C.fixed.assign(receivers.size(), 0);
  C.by.assign(receivers.size(), {});
  for (std::size_t r = 0; r < receivers.size(); r++) {
    const auto& p = receivers[r];
    if (!p.on_face()) continue;
    for (std::size_t c = 0; c < prisms.size(); c++) {
      if (static_cast<int>(c) == p.owner || !cg::inside_ring(prisms[c].ring, {p.x, p.y})) continue;
      if (prisms[c].variable < 0) {
        if (prisms[c].height > p.z) C.fixed[r] = 1;
      } else {
        C.by[r].push_back(static_cast<std::uint32_t>(prisms[c].variable));
      }
    }
  }
  return C;
}

// Whether a face receiver is open to the air under a design's heights.
inline bool exposed(const Cover& C, const std::vector<Receiver>& receivers, std::size_t r, const std::vector<double>& heights) {
  if (C.fixed[r]) return false;
  for (auto v : C.by[r])
    if (heights[v] > receivers[r].z) return false;
  return true;
}

// Receivers on the ground every `spacing` metres within a ring, outside the footprints given.
inline std::vector<Receiver> ground(const Ring& area, double spacing, const std::vector<Prism>& outside) {
  std::vector<Receiver> out;
  double x0 = 1e300, y0 = 1e300, x1 = -1e300, y1 = -1e300;
  for (const auto& q : area) x0 = std::min(x0, q[0]), y0 = std::min(y0, q[1]), x1 = std::max(x1, q[0]), y1 = std::max(y1, q[1]);
  for (double y = y0 + spacing / 2; y < y1; y += spacing)
    for (double x = x0 + spacing / 2; x < x1; x += spacing) {
      if (!cg::inside_ring(area, {x, y})) continue;
      bool under = false;
      for (const auto& p : outside) under = under || cg::inside_ring(p.ring, {x, y});
      if (!under) out.push_back({x, y, 0, 0, 0, -1});
    }
  return out;
}

// Receivers on the faces of a prism, every `spacing` metres along each edge and every `rise` metres up to `height`, a little
// outside the face (0.05 m) on its outward normal.
inline std::vector<Receiver> faces(const Ring& ring, double height, int owner, double spacing, double rise) {
  std::vector<Receiver> out;
  Ring r = ring;
  if (cg::ring_area(r) < 0) std::reverse(r.begin(), r.end());
  for (std::size_t k = 0; k < r.size(); k++) {
    const auto a = r[k], b = r[(k + 1) % r.size()];
    const double L = std::hypot(b[0] - a[0], b[1] - a[1]);
    if (L <= 0) continue;
    const double nx = (b[1] - a[1]) / L, ny = -(b[0] - a[0]) / L;  // outward for a counter-clockwise ring
    for (double s = spacing / 2; s < L; s += spacing)
      for (double z = rise / 2; z < height; z += rise)
        out.push_back({a[0] + (b[0] - a[0]) * s / L + 0.05 * nx, a[1] + (b[1] - a[1]) * s / L + 0.05 * ny, z, nx, ny, owner});
  }
  return out;
}

}  // namespace architecture::environment
