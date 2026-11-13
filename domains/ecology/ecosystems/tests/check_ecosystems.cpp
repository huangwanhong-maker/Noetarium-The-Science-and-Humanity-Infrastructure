// Checks of ecology::ecosystems: carbon by hand; habitat quality around one threat cell, linear and exponential, by hand at given
// distances, none beyond the linear reach, the exponential at 5% at its reach, quality falling with degradation by the half-saturation
// curve; Budyko's curve in Fu's form at its limits and at PET = P with ω = 2 (2 − √2), the yield of a cell by hand, a sealed class
// evaporating min(K_c ET₀, P); RUSLE's L and S by hand at a 5% and a 20% slope, the two branches of S meeting near 9%.

#include <ecology/ecosystems/services.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <map>
#include <vector>

using namespace ecology::ecosystems;

int main() {
  calx::verification::Suite s;

  // ---- carbon ----
  {
    Grid g{3, 2, 100, {0, 0, 1, 1, 2, -1}};  // cells of 1 ha
    const auto c = carbon(g, {{0, {100, 20, 80, 5}}, {1, {10, 2, 50, 0}}});
    s.check("carbon by hand: two forest cells of 205, two grass of 62, Mg C", std::fabs(c.total - (2 * 205 + 2 * 62)) + std::fabs(c.by_class.at(0) - 410), 1e-12);
    s.require("carbon: a class with no pools is counted as unknown, the cell with no class left out", c.unknown == 1 && std::fabs(c.area_ha - 5) < 1e-12);
  }
  // ---- habitat quality ----
  {
    Grid g{21, 1, 100, std::vector<int>(21, 0)};  // a strip of habitat, 100 m cells
    Threat road{"road", 1, 1000, false, std::vector<double>(21, 0.0)};
    road.intensity[0] = 1;  // a road at the strip's west end
    std::map<int, HabitatClass> cls{{0, {1.0, {0.8}}}};
    const auto h = habitat_quality(g, cls, {road}, 0.3);
    // At 300 m: D = 1 × (1 − 300/1000) × 0.8 = 0.56; at 1000 m and beyond, nothing.
    s.check("habitat: linear decay, D = r (1 − d/d_max) S at 300 m", std::fabs(h.degradation[3] - 0.56), 1e-14);
    s.check("habitat: no degradation at and beyond the linear reach", h.degradation[10] + h.degradation[15], 0.0);
    s.check("habitat: Q = H (1 − D^z/(D^z + k^z)) at 300 m (z = 2.5, k = 0.3)",
            std::fabs(h.quality[3] - (1 - std::pow(0.56, 2.5) / (std::pow(0.56, 2.5) + std::pow(0.3, 2.5)))), 1e-14);
    s.require("habitat: quality rises with distance from the threat", h.quality[1] < h.quality[3] && h.quality[3] < h.quality[9] && h.quality[12] == 1.0);
    Threat ex = road;
    ex.exponential = true;
    const auto he = habitat_quality(g, cls, {ex}, 0.3);
    s.check("habitat: exponential decay is e^{−2.99} ≈ 5% at its reach (1000 m)", std::fabs(he.degradation[10] - 0.8 * std::exp(-2.99)), 1e-14);
  }
  // ---- water yield ----
  {
    s.check("Budyko in Fu's form: AET/P = 2 − √2 at PET = P with ω = 2", std::fabs(fu_budyko(1, 2) - (2 - std::sqrt(2.0))), 1e-15);
    s.check("Budyko: a dry climate evaporates nearly all (PET/P = 200 → AET/P → 1)", 1 - fu_budyko(200, 2.5), 1e-3);
    s.check("Budyko: a wet climate evaporates nearly its PET (PET/P = 0.02 → AET/P → PET/P)", std::fabs(fu_budyko(0.02, 2.5) - 0.02), 1e-3);
    Grid g{2, 1, 100, {0, 1}};
    const std::vector<double> P{800, 800}, ET0{1000, 1000}, AWC{150, 150};
    const auto w = water_yield(g, P, ET0, AWC, {{0, {0.8, true}}, {1, {0.3, false}}}, 5);
    const double omega = 5 * 150.0 / 800 + 1.25, aet = 800 * fu_budyko(800.0 / 800, omega);
    s.check("water yield by hand: a vegetated cell (K_c 0.8, ω = Z AWC/P + 1.25) yields P − AET", std::fabs(w.yield_mm[0] - (800 - aet)), 1e-12);
    s.check("water yield: a sealed cell evaporates min(K_c ET₀, P) = 300 mm and yields 500", std::fabs(w.yield_mm[1] - 500), 1e-12);
    s.check("water yield: the volume is Σ yield × area", std::fabs(w.volume_m3 - ((800 - aet) + 500) / 1000 * 1e4), 1e-9);
  }
  // ---- soil loss ----
  {
    const double t5 = std::atan(0.05), t20 = std::atan(0.20);
    const double s5 = std::sin(t5), s20 = std::sin(t20);
    const double b5 = (s5 / 0.0896) / (3 * std::pow(s5, 0.8) + 0.56), m5 = b5 / (1 + b5);
    s.check("RUSLE: L = (λ/22.13)^m at 5%, λ = 100 m", std::fabs(rusle_length(t5, 100) - std::pow(100 / 22.13, m5)), 1e-14);
    s.check("RUSLE: S = 10.8 sin θ + 0.03 below 9%, 16.8 sin θ − 0.50 above", std::fabs(rusle_steepness(t5) - (10.8 * s5 + 0.03)) + std::fabs(rusle_steepness(t20) - (16.8 * s20 - 0.50)), 1e-14);
    const double t9 = std::atan(0.09);
    s.check("RUSLE: the two branches of S nearly meet at 9% (within 0.01)", std::fabs((10.8 * std::sin(t9) + 0.03) - (16.8 * std::sin(t9) - 0.50)), 0.01);
    s.check("RUSLE: A = R K L S C P", std::fabs(soil_loss(1000, 0.03, t5, 100, 0.2, 1) - 1000 * 0.03 * rusle_length(t5, 100) * rusle_steepness(t5) * 0.2), 1e-12);
  }
  return s.done();
}
