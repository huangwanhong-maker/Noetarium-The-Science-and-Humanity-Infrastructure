// Checks of markets/valuation against closed forms and brute force.
//
// The statements held are the source's (W. Huang 2026): O.2 and O.3 of the Unified Design Draft v0.9 (a price exists exactly
// on flat ratios; the least-squares potential and its covariance), D.5 (arbitrage on triangles), S.14 to S.16 of Market Foam
// v0.9a (the valuation connection; valuations that do not depend on the numéraire; the duality of ratios and fluxes on a
// closed exchange complex), and C.6 to C.8 and C.12 of Cases v0.9b. One clause of D.5 is held in its amended form (theory
// note, amendment A1): a cycle that shares an edge with a triangle does not keep its holonomy.

#include <markets/valuation/valuation.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <complex>
#include <numbers>
#include <vector>

using namespace markets::valuation;
using calx::linalg::Matrix;
using morphe::topology::CWComplex;
using physis::discretisation::Cochain;

namespace {

// The complete graph on n kinds with every triangle as a face.
CWComplex complete(std::size_t n) {
  CWComplex X;
  X.vertices = n;
  std::vector<std::vector<std::size_t>> edge(n, std::vector<std::size_t>(n, 0));
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = i + 1; j < n; j++) {
      edge[i][j] = X.edges.size();
      X.edges.push_back({i, j});
    }
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = i + 1; j < n; j++)
      for (std::size_t k = j + 1; k < n; k++) {
        morphe::topology::Face f;
        f.base = i;
        f.boundary = {{edge[i][j], 1}, {edge[j][k], 1}, {edge[i][k], -1}};
        X.faces.push_back(f);
      }
  return X;
}

double norm(const Cochain& c) {
  double s = 0;
  for (double x : c) s += x * x;
  return std::sqrt(s);
}
double worst(const Cochain& a, const Cochain& b) {
  double w = 0;
  for (std::size_t i = 0; i < a.size(); i++) w = std::max(w, std::fabs(a[i] - b[i]));
  return w;
}
Matrix times(const Matrix& a, const Matrix& b) { return a * b; }
Matrix scaled(const Matrix& a, double c) {
  Matrix m = a;
  for (double& x : m.a) x *= c;
  return m;
}
double norm(const Matrix& m) {
  double s = 0;
  for (double x : m.a) s += x * x;
  return std::sqrt(s);
}
Matrix minus(const Matrix& a, const Matrix& b) {
  Matrix m = a;
  for (std::size_t i = 0; i < m.a.size(); i++) m.a[i] -= b.a[i];
  return m;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(20261112, 12, 0, 0);

  // ---- a price exists exactly on flat ratios ----
  {
    const std::size_t K = 5, S = 8;
    std::vector<double> pi(K);
    for (double& x : pi) x = rng.uniform(-1, 1);
    std::vector<Trade> flat;
    for (int v = 0; v < 60; v++) {
      const std::size_t k = rng.below(K);
      std::size_t l = rng.below(K - 1);
      if (l >= k) l++;
      const std::size_t i = rng.below(S);
      flat.push_back({i, (i + 1 + rng.below(S - 1)) % S, k, l, pi[k] - pi[l], static_cast<double>(v), rng.uniform(0.5, 2)});
    }
    const auto R = reference(flat, K, S);
    double mean = 0;
    for (double x : pi) mean += x / static_cast<double>(K);
    double potential = 0, residual = 0;
    for (std::size_t k = 0; k < K; k++) potential = std::max(potential, std::fabs(R.potential[k] - (pi[k] - mean)));
    for (double r : R.residual) residual = std::max(residual, std::fabs(r));
    s.check("flat ratios: the potential is the price they came from, up to one constant", potential, 1e-12);
    s.check("flat ratios: no trade has a residual, and no subject an advantage", residual + *std::max_element(R.advantage.begin(), R.advantage.end()), 1e-12);
    s.check("a cycle of three currencies whose rates multiply to 1.00983 has the holonomy 0.00978", holonomy({std::log(1.2), std::log(0.9), std::log(1.00983 / 1.08)}) - 0.00978, 5e-6);

    // Ratios that are not flat: the residual has no weighted divergence; units change the potential and nothing else.
    auto rough = flat;
    for (auto& t : rough) t.a += 0.1 * rng.normal();
    const auto Q = reference(rough, K, S);
    std::vector<double> divergence(K, 0.0);
    for (std::size_t v = 0; v < rough.size(); v++) divergence[rough[v].k] += rough[v].weight * Q.residual[v], divergence[rough[v].l] -= rough[v].weight * Q.residual[v];
    s.check("least squares: the weighted residuals have no divergence at any kind", *std::max_element(divergence.begin(), divergence.end()) - *std::min_element(divergence.begin(), divergence.end()), 1e-10);
    std::vector<double> u(K);
    for (double& x : u) x = rng.uniform(-2, 2);
    auto reunited = rough;
    for (auto& t : reunited) t.a += u[t.k] - u[t.l];
    const auto U = reference(reunited, K, S);
    double shift = 0, umean = 0;
    for (double x : u) umean += x / static_cast<double>(K);
    for (std::size_t k = 0; k < K; k++) shift = std::max(shift, std::fabs(U.potential[k] - Q.potential[k] - (u[k] - umean)));
    s.check("a change of units moves the potential by the change, up to one constant", shift, 1e-11);
    s.check("a change of units leaves every residual and every advantage as it was", worst(U.residual, Q.residual) + worst(U.advantage, Q.advantage), 1e-11);
    double total = 0;
    for (double a : Q.advantage) total += a;
    s.check("the advantages of the subjects add up to zero", total, 1e-11);
    // A cycle of trades k → l → m → k: its holonomy is the sum of its residuals.
    std::vector<Trade> cycle{{0, 1, 0, 1, 0.31, 0, 1}, {1, 2, 1, 2, -0.12, 1, 1}, {2, 0, 2, 0, -0.07, 2, 1}, {3, 4, 0, 2, 0.4, 3, 1}, {4, 5, 3, 1, 0.2, 4, 1}};
    const auto C = reference(cycle, 4, 6);
    s.check("the holonomy of a cycle of trades is the sum of their residuals", C.residual[0] + C.residual[1] + C.residual[2] - 0.12, 1e-13);
  }

  // ---- loops through time ----
  {
    // Kind 0 a good, kind 1 money. The good sells for e^{0.40} at time 0 and for e^{0.49} later; holding money carries 0.05
    // a unit of time, holding the good 0.02.
    const std::vector<double> carry{0.02, 0.05};
    for (const double tau : {2.0, 5.0}) {
      const std::vector<Trade> two{{0, 1, 0, 1, 0.40, 0.0, 1}, {1, 0, 0, 1, 0.49, tau, 1}};
      const auto L = lenses(two, carry);
      s.check(tau == 2 ? "a loop of two exchanges and two holdings over 2: geometric part −0.09, dynamical 0.06" : "the same over 5: geometric part −0.09 still, dynamical 0.15",
              std::fabs(L.at(0).geometric + 0.09) + std::fabs(L[0].dynamical - 0.03 * tau), 1e-14);
    }
    const std::vector<Trade> two{{0, 1, 0, 1, 0.40, 0.0, 1}, {1, 0, 1, 0, -0.49, 2.0, 1}};
    s.check("an exchange written the other way round is the same loop", lenses(two, carry).at(0).geometric + 0.09, 1e-14);
    // A static change of units moves neither part. A redenomination of money at the rate π, which every nominal rate follows,
    // moves πτ from the geometric part to the dynamical and leaves their sum.
    const double pi = 0.07, tau = 2;
    const std::vector<Trade> units{{0, 1, 0, 1, 0.40 + 1.3, 0.0, 1}, {1, 0, 0, 1, 0.49 + 1.3, tau, 1}};
    s.check("a static change of units moves neither part", std::fabs(lenses(units, carry)[0].geometric + 0.09) + std::fabs(lenses(units, carry)[0].dynamical - 0.06), 1e-14);
    const std::vector<Trade> inflated{{0, 1, 0, 1, 0.40, 0.0, 1}, {1, 0, 0, 1, 0.49 + pi * tau, tau, 1}};
    const auto before = lenses({{0, 1, 0, 1, 0.40, 0.0, 1}, {1, 0, 0, 1, 0.49, tau, 1}}, carry)[0], after = lenses(inflated, {0.02, 0.05 + pi})[0];
    s.check("a redenomination through time moves value from the geometric part to the dynamical", std::fabs(after.geometric - before.geometric + pi * tau) + std::fabs(after.dynamical - before.dynamical - pi * tau), 1e-14);
    s.check("and leaves the holonomy as it was", after.holonomy() - before.holonomy(), 1e-14);
    // The cost of carry: flat exactly when the later ratio less the earlier is the difference of the carries.
    const std::vector<Trade> parity{{0, 1, 0, 1, 0.40, 0.0, 1}, {1, 0, 0, 1, 0.40 + (0.10 - 0.04) * 1.0, 1.0, 1}};
    s.check("cost of carry: with carries of 0.04 and 0.10 the loop is flat when the ratio moves by 0.06", lenses(parity, {0.04, 0.10})[0].holonomy(), 1e-15);
  }

  // ---- arbitrage on triangles ----
  {
    const auto X = complete(5);
    Cochain a(X.edges.size());
    std::vector<double> pi{0.3, -0.2, 0.5, 0.0, -0.6};
    for (std::size_t e = 0; e < a.size(); e++) a[e] = pi[X.edges[e][1]] - pi[X.edges[e][0]] + 0.1 * rng.normal();
    const auto h0 = norm(physis::discretisation::d1(X, a));
    const auto mid = dissipate(X, a, 1.0, 0.3), late = dissipate(X, a, 1.0, 40.0);
    s.check("arbitrage: the divergence of the ratios is kept", worst(physis::discretisation::d0_transpose(X, mid), physis::discretisation::d0_transpose(X, a)), 1e-13);
    s.require("arbitrage: the triangles' holonomies die away", norm(physis::discretisation::d1(X, mid)) < h0 && norm(physis::discretisation::d1(X, late)) < 1e-12 * h0 + 1e-13);
    // On the complete graph every cycle is made of triangles: the ratios tend to the least-squares price of where they began.
    std::vector<Trade> as_trades;
    for (std::size_t e = 0; e < a.size(); e++) as_trades.push_back({0, 1, X.edges[e][0], X.edges[e][1], a[e], 0, 1});
    const auto R = reference(as_trades, 5, 2);
    Cochain price(a.size());
    for (std::size_t e = 0; e < a.size(); e++) price[e] = R.potential[X.edges[e][0]] - R.potential[X.edges[e][1]];
    s.check("arbitrage: where every cycle is made of triangles, the ratios tend to the least-squares price of where they began", worst(late, price), 1e-12);
    s.check("arbitrage: `settled` is that limit", worst(settled(X, a), late), 1e-12);

    // A square with no triangle: nothing moves, and its holonomy stays.
    CWComplex square;
    square.vertices = 4;
    square.edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}};
    const Cochain q{0.1, 0.2, -0.05, 0.15};
    s.check("a square with no triangle keeps its ratios, and so its holonomy of 0.4", worst(dissipate(square, q, 1.0, 10.0), q), 1e-15);
    // A square with a triangle on one side (amendment A1): the square is no sum of triangles, yet arbitrage on the triangle
    // moves the edge they share, and the square's holonomy with it: by a third of the triangle's.
    CWComplex kite;
    kite.vertices = 5;
    kite.edges = {{0, 1}, {1, 2}, {2, 3}, {3, 0}, {1, 4}, {4, 0}};
    morphe::topology::Face tri;
    tri.base = 0;
    tri.boundary = {{0, 1}, {4, 1}, {5, 1}};
    kite.faces.push_back(tri);
    const Cochain k0{0.1, 0.2, -0.05, 0.15, 0.3, -0.1};
    const auto k1 = dissipate(kite, k0, 1.0, 60.0);
    const double triangle = k0[0] + k0[4] + k0[5], round0 = k0[0] + k0[1] + k0[2] + k0[3], round1 = k1[0] + k1[1] + k1[2] + k1[3];
    s.check("a square that shares an edge with a triangle does not keep its holonomy: it moves by a third of the triangle's", round1 - (round0 - triangle / 3), 1e-12);
    s.check("what is kept is the ratios' part with no holonomy round any triangle", worst(settled(kite, k0), k1), 1e-12);
  }

  // ---- valuations that do not depend on the numéraire; ratios and fluxes ----
  {
    const auto X = complete(4);
    Cochain a(X.edges.size()), u(4);
    for (double& x : a) x = rng.uniform(-1, 1);
    for (double& x : u) x = rng.uniform(-1, 1);
    const auto moved = physis::discretisation::d0(X, u);
    // A circulation round the triangle 0 → 1 → 2 → 0 is conserved; a flow along one edge is not.
    Cochain round(X.edges.size(), 0.0), along(X.edges.size(), 0.0);
    round[0] = 1, round[3] = 1, round[1] = -1;  // edges (0,1), (1,2), (0,2)
    along[0] = 1;
    double dr = 0, da = 0;
    for (std::size_t e = 0; e < a.size(); e++) dr += round[e] * moved[e], da += along[e] * moved[e];
    s.check("a conserved flow values the ratios the same in every units", dr, 1e-15);
    s.require("a flow that is not conserved does not", std::fabs(da) > 1e-3);

    // The boundary of a tetrahedron, ratios in ℤ/5: the sum over ratios of the product over faces of w(holonomy) is 5⁶ times
    // the sum over fluxes that close along every edge of the product of ŵ.
    const int N = 5;
    std::vector<double> w(N);
    for (int h = 0; h < N; h++) w[h] = std::exp(0.8 * std::cos(2 * std::numbers::pi * h / N)) + 0.1 * h;
    std::vector<std::complex<double>> what(N);
    for (int j = 0; j < N; j++)
      for (int h = 0; h < N; h++) what[j] += w[h] * std::polar(1.0, -2 * std::numbers::pi * j * h / N) / static_cast<double>(N);
    const std::size_t E = X.edges.size(), Fc = X.faces.size();
    double left = 0;
    std::vector<int> cfg(E, 0);
    for (long long code = 0; code < 15625; code++) {
      long long c = code;
      for (std::size_t e = 0; e < E; e++) cfg[e] = static_cast<int>(c % N), c /= N;
      double term = 1;
      for (const auto& f : X.faces) {
        int h = 0;
        for (const auto& st : f.boundary) h += st.sign * cfg[st.edge];
        term *= w[((h % N) + N) % N];
      }
      left += term;
    }
    std::complex<double> right = 0;
    int closed = 0;
    std::vector<int> j(Fc, 0);
    for (int code = 0; code < 625; code++) {
      int c = code;
      for (std::size_t f = 0; f < Fc; f++) j[f] = c % N, c /= N;
      std::vector<int> at(E, 0);
      for (std::size_t f = 0; f < Fc; f++)
        for (const auto& st : X.faces[f].boundary) at[st.edge] += st.sign * j[f];
      bool closes = true;
      for (int x : at) closes = closes && ((x % N) + N) % N == 0;
      if (!closes) continue;
      closed++;
      std::complex<double> term = 1;
      for (std::size_t f = 0; f < Fc; f++) term *= what[j[f]];
      right += term;
    }
    s.require("a closed exchange complex: five labellings of the faces by fluxes close along every edge", closed == 5);
    s.check("a closed exchange complex: the sum over ratios is the sum over closed fluxes", left / (15625.0 * right.real()) - 1, 1e-12);
  }

  // ---- a cycle acting on bundles ----
  {
    CycleMap M;
    M.phi = Matrix(2);
    M.phi(0, 0) = 1.8, M.phi(0, 1) = 0.6, M.phi(1, 1) = 1 / 1.8;
    s.check("a cycle map of determinant one has no scalar defect", M.scalar_defect(), 1e-15);
    const auto x = M.after({1, 1}, 10);
    s.check("yet ten rounds take the bundle (1, 1) to (529.19, 0.0028): one kind compounds, the other is extinguished", std::fabs(x[0] - 529.19) / 529.19 + std::fabs(x[1] - 0.0028) / 0.0028, 2e-3);
    const auto ev = M.spectrum();
    s.check("its eigenvalues, 1.8 and 1/1.8, say so", std::fabs(std::max(std::abs(ev[0]), std::abs(ev[1])) - 1.8) + std::fabs(std::min(std::abs(ev[0]), std::abs(ev[1])) - 1 / 1.8), 1e-12);

    // Two conversions in a closed sequence: the bundle comes back displaced by the area of the loop times the commutator.
    Matrix a(3), b(3);
    for (double& v : a.a) v = rng.uniform(-1, 1);
    for (double& v : b.a) v = rng.uniform(-1, 1);
    const Matrix comm = minus(times(a, b), times(b, a));
    const auto loop = [&](double e) {
      const Matrix L = times(times(calx::linalg::expm(scaled(a, e)), calx::linalg::expm(scaled(b, e))), times(calx::linalg::expm(scaled(a, -e)), calx::linalg::expm(scaled(b, -e))));
      return norm(minus(minus(L, Matrix::identity(3)), scaled(comm, e * e)));
    };
    s.check("a closed sequence of two conversions: I + ε²[a, b], the rest of third order (halving ε divides it by eight)", loop(0.01) / loop(0.005) - 8, 0.5);
    // AB and BA are conjugate: the same trace and determinant; yet they take a bundle to different bundles.
    Matrix A(2), B(2);
    A(0, 0) = 1, A(0, 1) = 0.5, A(1, 1) = 1.25;
    B(0, 0) = 0.8, B(1, 0) = 0.4, B(1, 1) = 1.5;
    const Matrix AB = times(A, B), BA = times(B, A);
    s.check("two conversions in the two orders are conjugate: one trace, one determinant", std::fabs(AB(0, 0) + AB(1, 1) - BA(0, 0) - BA(1, 1)) + std::fabs(calx::linalg::determinant(AB) - calx::linalg::determinant(BA)), 1e-14);
    s.require("yet they take the bundle (1, 1) to different bundles", std::fabs(AB(0, 0) + AB(0, 1) - BA(0, 0) - BA(0, 1)) + std::fabs(AB(1, 0) + AB(1, 1) - BA(1, 0) - BA(1, 1)) > 0.1);
  }
  return s.done();
}
