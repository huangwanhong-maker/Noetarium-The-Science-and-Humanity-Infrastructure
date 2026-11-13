// Checks of sociology/time: a classification gathering codes into parts and naming what it leaves; a day closed to 1440 minutes,
// with a diary's zero replaced at 0.65 of its slot; a group's day (identical days: their centre is the day and their variance zero;
// two days: the geometric centre); the contrast of two days (the clr difference sums to zero, its norm is the distance, perturbing
// back recovers the day); who carries a part, by hand; the balance of two kinds of time, by hand; places compared (the principal
// coordinates of Aitchison's distances keep them exactly and their eigenvalues sum to (n − 1) times the total variance); and the
// isotemporal substitution table recovering a known law exactly, with NaN where a move is impossible.

#include <sociology/time/day.hpp>
#include <sociology/time/reallocation.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <numeric>
#include <string>
#include <vector>

using namespace sociology::time;

int main() {
  calx::verification::Suite s;
  {
    const auto c = classification({{"sleep", {"AC0"}}, {"paid work", {"AC1", "AC1_TR"}}, {"rest", {"AC5", "AC6"}}});
    const std::vector<std::string> codes{"AC0", "AC1_TR", "AC5", "AC6", "AC99"};
    const auto r = classify(c, codes, std::vector<double>{600, 300, 200, 300, 40});
    s.check("classify: two codes gathered into rest", r.minutes[2] - 500, 0);
    s.check("classify: the minutes left unclassified", r.unclassified - 40, 0);
    s.require("classify: the code left is named", r.left.size() == 1 && r.left[0] == "AC99");
    bool refused = false;
    try {
      classification({{"a", {"X"}}, {"b", {"X"}}});
    } catch (const std::invalid_argument&) {
      refused = true;
    }
    s.require("classification: a code in two parts is refused", refused);
  }
  {
    const auto d = close_day(std::vector<double>{500, 400, 300, 200});
    s.check("close_day: unrecorded time spread in proportion", d[0] - 500 * 1440.0 / 1400, 1e-9);
    const auto z = close_day(std::vector<double>{800, 0, 400, 240});
    s.check("close_day: a zero holds 0.65 of a ten-minute slot", z[1] - 6.5, 1e-9);
    s.check("close_day: the day still holds 1440 minutes", std::accumulate(z.begin(), z.end(), 0.0) - 1440, 1e-9);
    s.check("close_day: the others keep their ratios", z[0] / z[2] - 2, 1e-12);
  }
  {
    Rectangular same(5, 4);
    for (std::size_t i = 0; i < 5; i++) same(i, 0) = 480, same(i, 1) = 480, same(i, 2) = 240, same(i, 3) = 240;
    const auto g = group_day(same);
    s.check("group_day: identical days have that day as their centre", g.centre[2] - 240, 1e-9);
    s.check("group_day: and no spread", g.total_variance, 1e-15);
    Rectangular two(2, 2);
    two(0, 0) = 1000, two(0, 1) = 440, two(1, 0) = 640, two(1, 1) = 800;
    const auto h = group_day(two);
    const double a = std::sqrt(1000.0 * 640), b = std::sqrt(440.0 * 800);
    s.check("group_day: the centre is the closed geometric mean", h.centre[0] - 1440 * a / (a + b), 1e-9);
    s.check("group_day: the mean is the arithmetic one", h.mean[0] - 820, 1e-9);
  }
  {
    const std::vector<double> x{540, 300, 120, 480}, y{510, 180, 220, 530};
    const auto c = contrast(x, y);
    s.check("contrast: the clr difference sums to zero", std::accumulate(c.clr.begin(), c.clr.end(), 0.0), 1e-12);
    double n2 = 0;
    for (double v : c.clr) n2 += v * v;
    s.check("contrast: its norm is Aitchison's distance", std::sqrt(n2) - c.distance, 1e-12);
    const auto back = tyche::compositions::perturb(c.perturbation, close_day(y), 1440);
    s.check("contrast: perturbing y by x ⊖ y gives x", back[1] - 300, 1e-9);
    s.check("contrast: minutes between closed days", c.minutes[1] - 120, 1e-9);
    s.check("contrast: a day against itself", contrast(x, x).distance, 1e-15);
  }
  {
    const auto c = who_carries(std::vector<double>{228, 140}, std::vector<double>{51, 49});
    const double w = 228.0 * 51, m = 140.0 * 49;
    s.check("who_carries: women's share of the minutes", c.share[0] - w / (w + m), 1e-15);
    s.check("who_carries: shares sum to one", c.share[0] + c.share[1] - 1, 1e-15);
    s.check("who_carries: over their number", c.over[0] - (w / (w + m)) / 0.51, 1e-12);
    s.check("who_carries: hours a year", c.minutes_a_year_total - (w + m) * 365 / 60, 1e-9);
  }
  {
    const std::vector<double> day{480, 480, 240, 240};
    const std::vector<std::size_t> plus{2, 3}, minus{0, 1};
    s.check("balance: √(2·2/4) ln(240/480) = −ln 2", balance(day, plus, minus) + std::log(2.0), 1e-12);
    const std::vector<double> even{360, 360, 360, 360};
    s.check("balance: equal kinds of time balance at zero", balance(even, plus, minus), 1e-15);
  }
  {
    calx::random::Stream rng(20261113, 0x736f6369, 1, 0);
    const std::size_t n = 9, D = 5;
    Rectangular days(n, D);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t k = 0; k < D; k++) days(i, k) = rng.uniform(30, 600);
    const auto p = compare_places(days);
    double sum = 0;
    for (double v : p.ordination.eigenvalues) sum += v;
    s.check("compare_places: the eigenvalues sum to (n − 1) × the total variance", sum - (n - 1) * p.total_variance, 1e-9);
    s.check("compare_places: Aitchison's distance is Euclidean", p.ordination.negative, 1e-9);
    double worst = 0;
    const auto& X = p.ordination.coordinates;
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = i + 1; j < n; j++) {
        double d2 = 0;
        for (std::size_t k = 0; k < X.cols; k++) d2 += (X(i, k) - X(j, k)) * (X(i, k) - X(j, k));
        worst = std::max(worst, std::fabs(std::sqrt(d2) - p.distance(i, j)));
      }
    s.check("compare_places: the coordinates keep every distance", worst, 1e-9);
  }
  {
    // y = 3 + b·clr(x), Σ b = 0: the fit is exact, and a move's change is b·(clr(x¹) − clr(x⁰)).
    calx::random::Stream rng(20261113, 0x736f6369, 2, 0);
    const std::size_t n = 60, D = 4;
    const std::vector<double> b{0.8, -0.5, 0.2, -0.5};
    Rectangular days(n, D);
    std::vector<double> y(n);
    for (std::size_t i = 0; i < n; i++) {
      for (std::size_t k = 0; k < D; k++) days(i, k) = rng.uniform(60, 600);
      std::vector<double> row(D);
      for (std::size_t k = 0; k < D; k++) row[k] = days(i, k);
      const auto z = tyche::compositions::clr(row);
      y[i] = 3;
      for (std::size_t k = 0; k < D; k++) y[i] += b[k] * z[k];
    }
    const auto m = fit_outcome(days, y);
    const std::vector<double> ref{420, 300, 240, 480};
    const auto t = substitutions(m, 30, ref);
    std::vector<double> x1 = t.reference;
    x1[0] += 30, x1[1] -= 30;
    const auto z0 = tyche::compositions::clr(t.reference), z1 = tyche::compositions::clr(x1);
    double truth = 0;
    for (std::size_t k = 0; k < D; k++) truth += b[k] * (z1[k] - z0[k]);
    s.check("substitutions: thirty minutes to part 0 from part 1, the known law", t.change(0, 1) - truth, 1e-9);
    s.require("substitutions: no move of a part to itself", std::isnan(t.change(2, 2)));
    const auto u = substitutions(m, 400, ref);
    s.require("substitutions: a part cannot give more than it holds", std::isnan(u.change(0, 1)) && !std::isnan(u.change(1, 3)));
    s.require("substitutions: the reference closes to the day", std::fabs(std::accumulate(t.reference.begin(), t.reference.end(), 0.0) - 1440) < 1e-9);
  }
  return s.done();
}
