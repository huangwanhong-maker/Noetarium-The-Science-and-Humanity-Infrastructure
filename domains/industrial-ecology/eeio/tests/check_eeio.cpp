// Checks of industrial_ecology::eeio: a two-sector economy by hand (L, M and the footprint); the
// identities that hold of every table (production − consumption = exports − imports embodied, the world
// totals equal, the origin matrix's margins); characterisation; a region's footprint by product; the
// supply chains; and pymrio's own six-region test system, whose consumption-based, production-based,
// import and export accounts the domain reproduces (tests/fixtures/eeio_pymrio_test.txt).

#include <industrial_ecology/eeio/eeio.hpp>

#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <fstream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace industrial_ecology::eeio;
using calx::verification::relative;

namespace {

struct Fixture {
  std::vector<std::string> regions, sectors, stressors;
  std::map<std::string, std::vector<std::vector<double>>> tables;
};

Fixture read(const std::string& path) {
  Fixture fx;
  std::ifstream in(path);
  std::string line;
  auto words = [](const std::string& l) {
    std::istringstream ls(l);
    std::vector<std::string> w;
    for (std::string x; ls >> x;) w.push_back(x);
    return w;
  };
  while (std::getline(in, line)) {
    if (line.empty() || line[0] == '#') continue;
    auto w = words(line);
    if (w[0] == "regions" || w[0] == "sectors" || w[0] == "stressors") {
      auto& target = w[0] == "regions" ? fx.regions : w[0] == "sectors" ? fx.sectors : fx.stressors;
      target.assign(w.begin() + 1, w.end());
      continue;
    }
    const std::size_t rows = std::stoul(w[1]), cols = std::stoul(w[2]);
    auto& t = fx.tables[w[0]];
    for (std::size_t r = 0; r < rows; r++) {
      std::getline(in, line);
      std::istringstream ls(line);
      std::vector<double> v(cols);
      for (double& x : v) ls >> x;
      t.push_back(v);
    }
  }
  return fx;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- two sectors by hand ----
  {
    Mrio io;
    io.regions = {"home"}, io.sectors = {"energy", "goods"};
    io.Z = Matrix(2);
    io.Z(0, 0) = 10, io.Z(0, 1) = 40, io.Z(1, 0) = 20, io.Z(1, 1) = 30;
    io.Y = Rectangular(2, 1);
    io.Y(0, 0) = 50, io.Y(1, 0) = 150;  // x = (100, 200)
    io.ext.stressors = {"CO2"}, io.ext.units = {"t"};
    io.ext.F = Rectangular(1, 2);
    io.ext.F(0, 0) = 300, io.ext.F(0, 1) = 100;
    io.ext.F_y = Rectangular(1, 1);
    io.ext.F_y(0, 0) = 25;
    const auto acc = accounts(io);
    // A = [[0.1, 0.2], [0.2, 0.15]]; det(I − A) = 0.9·0.85 − 0.04 = 0.725.
    const double d = 0.725;
    s.check("by hand: L = (I − A)⁻¹ = [[0.85, 0.2], [0.2, 0.9]] / 0.725",
            std::fabs(acc.leontief.L(0, 0) - 0.85 / d) + std::fabs(acc.leontief.L(0, 1) - 0.2 / d) + std::fabs(acc.leontief.L(1, 1) - 0.9 / d), 1e-14);
    // S = (3, 0.5); M = S L = ((3·0.85 + 0.5·0.2)/d, (3·0.2 + 0.5·0.9)/d).
    s.check("by hand: the multipliers M = S L", std::fabs(acc.M(0, 0) - 2.65 / d) + std::fabs(acc.M(0, 1) - 1.05 / d), 1e-13);
    s.check("by hand: one region's footprint is everything it emits, 300 + 100 + 25", std::fabs(acc.consumption(0, 0) - 425) + std::fabs(acc.production(0, 0) - 425), 1e-12);
    const auto bp = by_product(io, acc, 0);
    s.check("by hand: the footprint by product, M_j y_j, sums to it less F_Y", std::fabs(bp(0, 0) + bp(0, 1) - 400) + std::fabs(bp(0, 0) - 50 * 2.65 / d), 1e-12);
    const auto W = paths(io, acc, 0, 0, 1e-9, 60, 1000);
    s.check("by hand: the walks' total is the footprint less F_Y, 400", std::fabs(W.total - 400), 1e-12);
    // What the cut leaves is the sum over the branches cut, each holding less than the cut.
    s.require("by hand: the search was ended by the bound, not by its depth or budget", W.complete);
    s.require("by hand: what the walks leave is less than the branches cut times the cut", W.total - W.covered >= 0 && W.total - W.covered < static_cast<double>(W.cut) * 1e-9 * W.total);
    s.check("by hand: and is small, under 10⁻⁵ of the footprint", (W.total - W.covered) / W.total, 1e-5);
    Rectangular C(1, 1);
    C(0, 0) = 1000;  // t → kg
    const auto kg = characterise(io.ext, C, {"CO2 (kg)"});
    s.check("characterise: F weighed by C, F_Y too", std::fabs(kg.F(0, 0) - 300000) + std::fabs(kg.F_y(0, 0) - 25000), 1e-9);
  }

  // ---- pymrio's six-region test system ----
  {
    const Fixture fx = read(std::string(INDUSTRIAL_ECOLOGY_FIXTURES) + "/eeio_pymrio_test.txt");
    s.require("pymrio fixture: six regions, eight sectors, two stressors, and every table", fx.regions.size() == 6 && fx.sectors.size() == 8 && fx.stressors.size() == 2 && fx.tables.size() == 8);
    Mrio io;
    io.regions = fx.regions, io.sectors = fx.sectors;
    const std::size_t N = 48, R = 6, K = 2;
    io.Z = Matrix(N);
    io.Y = Rectangular(N, R);
    for (std::size_t i = 0; i < N; i++) {
      for (std::size_t j = 0; j < N; j++) io.Z(i, j) = fx.tables.at("Z")[i][j];
      for (std::size_t r = 0; r < R; r++) io.Y(i, r) = fx.tables.at("Y")[i][r];
    }
    io.ext.stressors = fx.stressors;
    io.ext.F = Rectangular(K, N), io.ext.F_y = Rectangular(K, R);
    for (std::size_t k = 0; k < K; k++) {
      for (std::size_t j = 0; j < N; j++) io.ext.F(k, j) = fx.tables.at("F")[k][j];
      for (std::size_t r = 0; r < R; r++) io.ext.F_y(k, r) = fx.tables.at("F_Y")[k][r];
    }
    const auto acc = accounts(io);
    auto worst = [&](const Rectangular& ours, const std::string& theirs) {
      double w = 0;
      for (std::size_t k = 0; k < K; k++)
        for (std::size_t r = 0; r < R; r++) w = std::max(w, relative(ours(k, r), fx.tables.at(theirs)[k][r]));
      return w;
    };
    s.require("pymrio: the economy is productive (L ≥ 0)", acc.leontief.productive);
    s.check("pymrio: the consumption-based accounts (D_cba_reg)", worst(acc.consumption, "D_cba_reg"), 1e-10);
    s.check("pymrio: the production-based accounts (D_pba_reg)", worst(acc.production, "D_pba_reg"), 1e-10);
    s.check("pymrio: what is embodied in imports (D_imp_reg)", worst(acc.imports_embodied, "D_imp_reg"), 1e-10);
    s.check("pymrio: what is embodied in exports (D_exp_reg)", worst(acc.exports_embodied, "D_exp_reg"), 1e-10);
    double world = 0, margins = 0;
    for (std::size_t k = 0; k < K; k++) {
      double p = 0, c = 0;
      for (std::size_t r = 0; r < R; r++) p += acc.production(k, r), c += acc.consumption(k, r);
      world = std::max(world, std::fabs(p - c) / p);
      for (std::size_t a = 0; a < R; a++) {
        double row = 0, col = 0;
        for (std::size_t b = 0; b < R; b++) row += acc.origin[k](a, b), col += acc.origin[k](b, a);
        margins = std::max(margins, std::fabs(row - acc.production(k, a)) + std::fabs(col - acc.consumption(k, a)));
      }
    }
    s.check("identities: production − consumption = exports − imports embodied, in every region", acc.balance, 1e-6);
    s.check("identities: the world's production equals its consumption", world, 1e-13);
    s.check("identities: the origin matrix's rows sum to production and its columns to consumption", margins, 1e-6);
    double products = 0;
    for (std::size_t r = 0; r < R; r++) {
      const auto bp = by_product(io, acc, r);
      for (std::size_t k = 0; k < K; k++) {
        double t = 0;
        for (std::size_t j = 0; j < 8; j++) t += bp(k, j);
        products = std::max(products, relative(t + io.ext.F_y(k, r), acc.consumption(k, r)));
      }
    }
    s.check("a region's footprint by product sums to its footprint", products, 1e-12);
    const auto W = paths(io, acc, 0, 0, 1e-3, 8, 20);
    s.require("supply chains: the leading walks are above the cut and cover no more than the total",
              !W.walks.empty() && W.walks.front().value >= W.walks.back().value && W.covered <= W.total * (1 + 1e-12));
  }

  return s.done();
}
