// Checks of markets/foam: the history complex of a record, against counts made by hand, identities and brute force.
//
// The statements held are those of the source (W. Huang 2026, Market Foam v0.9a): S.2 (the boundary of a boundary vanishes),
// S.3 (the faces at a time are the state), S.4 (the complex does not depend on the order of independent events), S.5 (the
// order of elements is within the worldline order), S.6 (the balance laws), S.7 (balance as an invariant subspace of a torus),
// S.9 (the area law). The complex between events is compared with morphe's worldline complex of the touches.

#include <markets/foam/foam.hpp>
#include <markets/foam/record.hpp>

#include <morphe/topology/worldlines.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <complex>
#include <iterator>
#include <map>
#include <string>
#include <numbers>
#include <set>
#include <tuple>
#include <vector>

using namespace markets::foam;

namespace {

// A record of random transfers and exchanges among n subjects in K kinds.
Record random_record(calx::random::Stream& rng, std::size_t n, std::size_t K, std::size_t events, double exchange_share) {
  Record R;
  R.subjects = n, R.kinds = K, R.begin = 0, R.end = static_cast<double>(events) + 1;
  R.opening.assign(n, std::vector<double>(K, 50.0));
  for (std::size_t e = 0; e < events; e++) {
    const double t = static_cast<double>(e) + rng.uniform(0.1, 0.9);
    const std::size_t i = rng.below(n);
    std::size_t j = rng.below(n - 1);
    if (j >= i) j++;
    const std::size_t k = rng.below(K);
    R.movements.push_back({e, t, i, j, k, rng.uniform(0.5, 3)});
    if (K > 1 && rng.uniform() < exchange_share) {
      std::size_t l = rng.below(K - 1);
      if (l >= k) l++;
      R.movements.push_back({e, t, j, i, l, rng.uniform(0.5, 3)});
    }
  }
  return R;
}

// What does not depend on the numbering of the events: each junction's worldline and each fact's faces, by the events' names.
auto signature(const History& H, const Foam& F, const std::vector<std::size_t>& name) {
  std::vector<std::vector<std::size_t>> lines;
  for (const auto& w : F.worldline) {
    lines.emplace_back();
    for (std::size_t v : w) lines.back().push_back(name[v]);
  }
  std::vector<std::tuple<std::size_t, std::size_t, std::size_t, double>> faces;
  for (const auto& f : F.faces) faces.emplace_back(f.fact, f.from == none ? none : name[f.from], f.to == none ? none : name[f.to], f.value);
  std::sort(faces.begin(), faces.end());
  return std::make_tuple(lines, faces, H.facts.size());
}

double worst_slice(const History& H, const Foam& F, calx::random::Stream& rng, int times) {
  double worst = 0;
  for (int k = 0; k < times; k++) {
    const double t = rng.uniform(H.begin, H.end);
    const auto a = state(H, t), b = slice(F, t);
    for (std::size_t r = 0; r < a.size(); r++) worst = std::max(worst, std::isnan(a[r]) && std::isnan(b[r]) ? 0.0 : std::fabs(a[r] - b[r]));
  }
  return worst;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream rng(20261112, 11, 0, 0);

  // ---- one transfer, by hand ----
  {
    Record R;
    R.subjects = 2, R.kinds = 1, R.begin = 0, R.end = 2;
    R.opening = {{5}, {1}};
    R.movements.push_back({0, 1.0, 0, 1, 0, 2.0});
    const auto bearer = history(R);
    const auto F = foam(bearer.history);
    // Two holdings, each with a private end: four junctions. One event, and a point at each end of each junction: nine vertices.
    // Two segments a junction and two links at each end: twelve edges. Two faces a holding: four.
    s.require("one transfer between two bearer holdings: 9 vertices, 12 edges, 4 faces", F.cw.vertices == 9 && F.cw.edges.size() == 12 && F.faces.size() == 4);
    s.check("one transfer: the boundary of every face's boundary vanishes", boundary_defect(F), 0);
    s.check("one transfer: the faces hold 5 and 1 before it, 3 and 3 after", std::fabs(slice(F, 0.5)[0] - 5) + std::fabs(slice(F, 0.5)[1] - 1) + std::fabs(slice(F, 1.5)[0] - 3) + std::fabs(slice(F, 1.5)[1] - 3), 0);
    s.check("one transfer: the areas of the four faces, label times duration, add up to the kind's total over the time, 6 × 2", F.faces[0].area() + F.faces[1].area() + F.faces[2].area() + F.faces[3].area() - 12, 1e-15);
    const auto ledger = history(R, {1});
    const auto G = foam(ledger.history);
    // Booked at a ledger: three junctions, the event and six points; six segments and four links; four faces.
    s.require("the same transfer booked at a ledger: 7 vertices, 10 edges, 4 faces", G.cw.vertices == 7 && G.cw.edges.size() == 10 && G.faces.size() == 4);
    s.check("booked at a ledger: the boundary of a boundary vanishes", boundary_defect(G), 0);
    s.check("booked at a ledger: the labels on the ledger's worldline add up to the kind's total, before and after (Gauss)",
            std::fabs(at_junction(ledger.history, G, 2, 0, 0.5) - 6) + std::fabs(at_junction(ledger.history, G, 2, 0, 1.5) - 6), 0);
  }

  // ---- random records, as bearer holdings and booked at ledgers ----
  for (const bool booked : {false, true}) {
    const auto R = random_record(rng, 12, 3, 160, 0.5);
    const auto P = history(R, booked ? std::vector<char>{1, 1, 1} : std::vector<char>{});
    const auto& H = P.history;
    const auto F = foam(H);
    const std::string how = booked ? "booked at ledgers" : "bearer holdings";
    s.check(("160 events, " + how + ": the boundary of a boundary vanishes on every face").c_str(), boundary_defect(F), 0);
    s.check(("160 events, " + how + ": at 60 times the faces are the state").c_str(), worst_slice(H, F, rng, 60), 0);
    // The cells counted from the history: a segment for each touch of a junction and one more, a face for each touch of a fact and one more.
    std::size_t segments = 0, faces = 0, touches = 0;
    for (const auto& w : F.worldline) segments += w.size() + 1;
    for (const auto& e : H.events) touches += e.touches.size();
    faces = touches + H.facts.size();
    s.require(("160 events, " + how + ": the segments and the faces are counted by the touches").c_str(), F.segments.size() == segments && F.faces.size() == faces
              && F.cw.edges.size() == segments + 2 * H.facts.size());
    // Between events it is morphe's worldline complex of the record of touches.
    morphe::topology::Encounters E;
    E.participants = F.junctions;
    E.present = F.touched;
    std::vector<std::vector<std::size_t>> meetings(H.facts.size());
    for (std::size_t v = 0; v < H.events.size(); v++)
      for (const auto& u : H.events[v].touches) meetings[u.fact].push_back(v);
    for (std::size_t r = 0; r < H.facts.size(); r++) E.pairs.push_back({F.end_a[r], F.end_b[r], meetings[r]});
    const auto W = morphe::topology::worldline_complex(E);
    std::size_t interior = 0, between = 0;
    for (const auto& f : F.faces) interior += f.from != none && f.to != none;
    for (const auto& g : F.segments) between += g.from < F.events && g.to < F.events;
    s.require(("160 events, " + how + ": between events it is morphe's worldline complex of the touches (its lenses the faces, its segments the segments)").c_str(),
              W.lenses.size() == interior && W.segments.size() == between);
    // Balance at every event, for every kind.
    double worst = 0;
    for (const auto& row : imbalance(H, R.kinds))
      for (double d : row) worst = std::max(worst, std::fabs(d));
    s.check(("160 events, " + how + ": at every event each kind's labels after are its labels before").c_str(), worst, 1e-12);
    if (booked) {
      double gauss = 0;
      for (std::size_t k = 0; k < R.kinds; k++)
        for (double t : {0.0, 40.3, 99.9, 161.0}) gauss = std::max(gauss, std::fabs(at_junction(H, F, P.ledger[k], k, t) - 600.0));
      s.check("booked at ledgers: on each ledger's worldline the labels add up to the kind's total at every time (Gauss)", gauss, 1e-10);
    }
    // The area law: Σ h_f j_f |f| over the faces is the integral of the total hazard over time.
    std::vector<double> hazard(H.facts.size());
    for (double& h : hazard) h = rng.uniform(0.1, 2);
    s.check(("160 events, " + how + ": the exponent of the area law is the integrated hazard").c_str(), area_weight(H, F, hazard) / hazard_integral(H, hazard) - 1, 1e-12);

    // Events with no junction in common exchanged in order: the same complex.
    History swapped = H;
    std::vector<std::size_t> name(H.events.size());
    for (std::size_t v = 0; v < name.size(); v++) name[v] = v;
    std::size_t exchanged = 0;
    for (std::size_t v = 0; v + 1 < swapped.events.size(); v++) {
      const auto &a = F.touched[name[v]], &b = F.touched[name[v + 1]];
      std::vector<std::size_t> common;
      std::set_intersection(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(common));
      if (!common.empty()) continue;
      std::swap(swapped.events[v].touches, swapped.events[v + 1].touches);
      std::swap(swapped.events[v].type, swapped.events[v + 1].type);
      std::swap(name[v], name[v + 1]);
      exchanged++;
      v++;
    }
    std::vector<std::size_t> same(H.events.size());
    for (std::size_t v = 0; v < same.size(); v++) same[v] = v;
    const auto G = foam(swapped);
    s.require(("160 events, " + how + ": independent neighbours exchanged in order leave the complex as it was (" + std::to_string(exchanged) + " exchanges)").c_str(),
              (booked || exchanged > 10) && signature(H, F, same) == signature(swapped, G, name));
  }

  // ---- the two orders ----
  {
    // The order of elements: an event precedes the next event that touches the same fact.
    const auto element_related = [](const History& H) {
      const std::size_t n = H.events.size();
      std::vector<std::vector<char>> before(n, std::vector<char>(n, 0));
      std::vector<std::size_t> last(H.facts.size(), none);
      for (std::size_t v = 0; v < n; v++)
        for (const auto& u : H.events[v].touches) {
          if (last[u.fact] != none) {
            before[v][last[u.fact]] = 1;
            for (std::size_t w = 0; w < n; w++) before[v][w] = before[v][w] || before[last[u.fact]][w];
          }
          last[u.fact] = v;
        }
      return before;
    };
    const auto R = random_record(rng, 14, 3, 120, 0.5);
    const auto H = history(R).history;
    const auto O = order(foam(H));
    const auto E = element_related(H);
    bool within = true;
    std::size_t fewer = 0;
    for (std::size_t v = 0; v < H.events.size(); v++)
      for (std::size_t u = 0; u < v; u++) {
        if (E[v][u] && !O.precedes(u, v)) within = false;
        if (!E[v][u] && O.precedes(u, v)) fewer++;
      }
    s.require("the order of elements is within the worldline order, and with three kinds held it is smaller", within && fewer > 0);
    const auto R1 = random_record(rng, 14, 1, 120, 0);
    const auto H1 = history(R1).history;
    const auto O1 = order(foam(H1));
    const auto E1 = element_related(H1);
    bool equal = true;
    for (std::size_t v = 0; v < H1.events.size(); v++)
      for (std::size_t u = 0; u < v; u++) equal = equal && (static_cast<bool>(E1[v][u]) == O1.precedes(u, v));
    s.require("where each junction holds one fact, which every event on it changes, the two orders are one", equal);

    // Settlement decides the causal structure: 120 transfers of one kind among 40 subjects.
    const auto T = random_record(rng, 40, 1, 120, 0);
    const auto bearer = order(foam(history(T).history)), ledger = order(foam(history(T, {1}).history));
    s.check("one kind booked at one ledger: every two transfers are causally related", ledger.density() - 1, 1e-15);
    s.check("the same transfers as bearer holdings: the share of related pairs, below the ledger's by more than a quarter", std::max(0.0, bearer.density() - 0.75), 0);
    s.require("booked at a ledger the longest chain is the whole record; as bearer holdings it is shorter", ledger.longest() == 119 && bearer.longest() < 100);
    const auto F = foam(history(T).history);
    std::size_t most = 0;
    for (const auto& w : F.worldline) most = std::max(most, w.size());
    s.require("proper time: a junction's events are counted on its worldline, and no subject of 40 lives all 120", most < 120 && most >= 6);
  }

  // ---- facts that begin and end ----
  {
    // A subject and a venue. An offer is placed (event 0), revised (event 1) and withdrawn (event 2); an access stands
    // throughout and is read by each of them.
    History H;
    H.junctions = 2, H.begin = 0, H.end = 4;
    H.facts.push_back({0, 1, 0, none, 1.0, true});   // the access
    H.facts.push_back({0, 1, 1, none, 0.0, false});  // the offer
    H.events.push_back({1, 0, {{1, Touch::create, 7.0}, {0, Touch::read, 1.0}}, {}});
    H.events.push_back({2, 0, {{1, Touch::modify, 6.5}, {0, Touch::read, 1.0}}, {}});
    H.events.push_back({3, 0, {{1, Touch::remove, 0.0}, {0, Touch::read, 1.0}}, {}});
    const auto F = foam(H);
    s.require("an offer placed, revised and withdrawn: two faces of the offer, four of the access it reads", F.faces_of[1].size() == 2 && F.faces_of[0].size() == 4);
    s.check("an offer: the boundary of a boundary vanishes", boundary_defect(F), 0);
    s.require("an offer: it stands at 7 then at 6.5, and at no other time", slice(F, 1.5)[1] == 7 && slice(F, 2.5)[1] == 6.5 && std::isnan(slice(F, 0.5)[1]) && std::isnan(slice(F, 3.5)[1]));
    s.require("an offer: no link of it in the first or the last state", F.first_link[1] == none && F.last_link[1] == none && F.first_link[0] != none);
    // A belief with one end, formed at an event and standing to the end: its private junction lives as long as it does.
    History B;
    B.junctions = 1, B.begin = 0, B.end = 2;
    B.facts.push_back({0, none, 0, none, 0.0, false});
    B.events.push_back({1, 0, {{0, Touch::create, 0.3}}, {}});
    const auto G = foam(B);
    s.require("a belief formed at an event: one face, its private end beginning at that event", G.faces.size() == 1 && G.bottom[1] == none && G.top[1] != none && G.cw.vertices == 4);
    s.check("a belief: the boundary of a boundary vanishes", boundary_defect(G), 0);
    bool threw = false;
    History bad = H;
    bad.events[1].touches[0].fact = 5;
    try { (void)foam(bad); } catch (const std::invalid_argument&) { threw = true; }
    s.require("an event that touches a fact that is not there is refused", threw);
    threw = false;
    bad = H;
    bad.events.erase(bad.events.begin());
    try { (void)foam(bad); } catch (const std::invalid_argument&) { threw = true; }
    s.require("a fact touched before it stands is refused", threw);
  }

  // ---- balance as an invariant subspace of a torus ----
  {
    // Charges in ℤ² (two kinds) meet at a junction with source strength T. The dimension of the invariant subspace of
    // V_{q₁} ⊗ … ⊗ V_{q_m} ⊗ V_{−T} is the mean of its character over the torus: here over the finite torus (ℤ/N)², which
    // tells weights apart while they stay below N.
    const int N = 23;
    const auto invariant = [&](const std::vector<std::array<int, 2>>& q, std::array<int, 2> T) {
      std::complex<double> mean = 0;
      for (int g = 0; g < N; g++)
        for (int h = 0; h < N; h++) {
          std::array<int, 2> w{-T[0], -T[1]};
          for (const auto& c : q) w[0] += c[0], w[1] += c[1];
          mean += std::polar(1.0, 2 * std::numbers::pi * (g * w[0] + h * w[1]) / N);
        }
      return (mean / static_cast<double>(N * N)).real();
    };
    s.check("a junction whose charges add up to its source has a one-dimensional invariant subspace", invariant({{3, 0}, {-1, 2}, {2, 1}}, {4, 3}) - 1, 1e-12);
    s.check("one whose charges do not has none", invariant({{3, 0}, {-1, 2}, {2, 1}}, {4, 2}), 1e-12);
    s.check("a claim and its issuer's position, +q and −q, balance at a ledger with no source", invariant({{0, 5}, {0, -5}}, {0, 0}) - 1, 1e-12);
  }
  return s.done();
}
