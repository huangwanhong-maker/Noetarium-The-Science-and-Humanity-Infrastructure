// Checks of land use and transport interacting (model.hpp).
//
// - Conservation, every year: households located plus the outside option's are M; the journeys are the workers, by origin;
//   the service jobs are b times the households.
// - A ring of six equal zones stays equal.
// - A base year revealed from observed households and rents is a fixed point of a year while nothing changes.
// - Capitalisation: with one class and the stock held, a change of accessibility changes next year's log rent by exactly
//   λ ΔA / α in every zone.
// - Braess's paradox in land: 4,000 workers living at S and working at E; the road from A to B added lengthens the journey
//   from 65 to 80 minutes, lowers the accessibility of S by 15 minutes and its rent by exp(−15 λ / α).
// - A grid of sixteen zones on a congested network: Evans's equilibrium reached each year; a widened road toward the
//   centre raises the accessibility of the zones it serves, then their rents, and, with building, their stock; the region
//   holds more households.
// - Building never passes capacity; raising a dear zone's capacity gives it more dwellings and households and a lower rent.
// - The stationary state: every zone either at capacity or with its rent at most the rent at which building pays, and a
//   year from it changes nothing.

#include <urban/luti/model.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace ul = urban::luti;
namespace ni = nomos::interaction;
namespace ng = nomos::games;

namespace {
urban::housing::Market one_class(double households, double alpha) {
  urban::housing::Market m;
  m.classes = {{1.0, 1.0, {}}};
  m.households = households;
  m.alpha = alpha;
  m.mean_income = 1;
  m.income_power = 1;
  m.vacancy = 0.05;
  m.reference_rent = 1;
  return m;
}

double conservation(const ul::Model& m, const ul::Year& y) {
  double located = 0, service = 0, trips = 0, workers = 0, by_origin = 0;
  for (std::size_t i = 0; i < y.households.size(); i++) located += y.households[i], service += y.service[i];
  const auto rows = y.trips.row_sums();
  for (std::size_t i = 0; i < y.households.size(); i++) {
    const double w = m.workers_per_household * y.households[i];
    workers += w;
    by_origin = std::max(by_origin, std::fabs(rows[i] - w) / std::max(1.0, w));
  }
  trips = y.trips.total();
  return std::fabs(located + y.outside - m.market.households) / m.market.households + std::fabs(service - m.service_per_household * located) / std::max(1.0, located) +
         std::fabs(trips - workers) / workers + by_origin;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream st(20261112, 0x6c757469u, 0, 0);  // "luti"

  {
    // A ring of six equal zones, minutes around it.
    const std::size_t n = 6;
    ul::Model m;
    m.market = one_class(30000, 2);
    m.zones.assign(n, ul::Zone{2000, 4000, 0, 1500});
    m.transport.costs = ni::Table(n, n);
    for (std::size_t i = 0; i < n; i++)
      for (std::size_t j = 0; j < n; j++) m.transport.costs(i, j) = 5 + 8.0 * std::min<std::size_t>((i + n - j) % n, (j + n - i) % n);
    m.service_per_household = 0.3;
    m.development = {1.0, 0.2, 0.01};
    const auto years = ul::run(m, {}, 10);
    double spread = 0, cons = 0;
    for (const auto& y : years) {
      cons = std::max(cons, conservation(m, y));
      for (std::size_t i = 1; i < n; i++) spread = std::max(spread, std::fabs(y.households[i] - y.households[0]) / y.households[0] + std::fabs(y.stock[i] - y.stock[0]) / y.stock[0]);
    }
    s.check("A ring of six equal zones: households and stock stay equal for ten years", spread, 1e-10);
    s.check("Conservation over the ring's years: households, service jobs, journeys by origin", cons, 1e-9);
  }

  // A region of eight zones on a fixed table, two classes of household.
  const std::size_t n = 8;
  std::vector<double> x(n), y(n), observed(n), rents(n);
  ul::Model base;
  base.market.classes = {{0.6, 900, {}}, {0.4, 2200, {}}};
  base.market.households = 60000;
  base.market.alpha = 2.5;
  base.market.mean_income = 1400;
  base.market.income_power = 1;
  base.market.vacancy = 0.05;
  base.market.reference_rent = 450;
  base.transport.costs = ni::Table(n, n);
  base.zones.resize(n);
  for (std::size_t i = 0; i < n; i++) {
    x[i] = st.uniform(0, 12), y[i] = st.uniform(0, 12);
    base.zones[i].basic = st.uniform(500, 6000);
    observed[i] = st.uniform(1500, 5000);
    rents[i] = st.uniform(380, 720);
  }
  for (std::size_t i = 0; i < n; i++)
    for (std::size_t j = 0; j < n; j++) base.transport.costs(i, j) = 4 + 3.0 * std::hypot(x[i] - x[j], y[i] - y[j]);   // minutes
  base.beta = 0.06;
  base.lambda = 0.04;
  base.service_per_household = 0.25;
  {
    ul::Model m = base;
    const auto y0 = ul::reveal(m, observed, rents);
    const auto y1 = ul::step(m, y0);
    double e = 0;
    for (std::size_t i = 0; i < n; i++)
      e = std::max({e, std::fabs(y1.households[i] - observed[i]) / observed[i], std::fabs(y1.rents[i] - rents[i]) / rents[i], std::fabs(y1.access[i] - y0.access[i]) / std::fabs(y0.access[i])});
    s.check("A revealed base year (two classes, eight zones) is a fixed point: households, rents and accessibility a year on", e, 1e-9);
    s.check("Conservation in the revealed year and the next", conservation(m, y0) + conservation(m, y1), 1e-9);
  }
  {
    ul::Model a = base;
    a.market = one_class(60000, 2.5);
    a.market.reference_rent = 450;
    const auto y0 = ul::reveal(a, observed, rents);
    ul::Model b = a;
    // Zone 3 gets nearer to everything (a new line): its costs to and from every zone fall by a quarter.
    for (std::size_t j = 0; j < n; j++) b.transport.costs(3, j) *= 0.75, b.transport.costs(j, 3) *= 0.75;
    const auto a1 = ul::step(a, y0), a2 = ul::step(a, a1);
    const auto b1 = ul::step(b, y0), b2 = ul::step(b, b1);
    double e = 0;
    for (std::size_t i = 0; i < n; i++) e = std::max(e, std::fabs(std::log(b2.rents[i] / a2.rents[i]) - b.lambda * (b1.access[i] - a1.access[i]) / b.market.alpha));
    s.check("Capitalisation: Δ ln r = λ ΔA / α in every zone, one class, the stock held", e, 1e-10);
    s.require("Capitalisation: the zone brought nearer gains the most accessibility", [&] {
      std::size_t best = 0;
      for (std::size_t i = 1; i < n; i++)
        if (b1.access[i] - a1.access[i] > b1.access[best] - a1.access[best]) best = i;
      return best == 3;
    }());
  }
  {
    // Braess: S = 0, A = 1, B = 2, E = 3; zone 0 at S with the dwellings, zone 1 at E with the jobs.
    ul::Model m;
    m.market = one_class(8000, 2);
    m.zones = {ul::Zone{4000 / 0.95, 1e9, 0, 0}, ul::Zone{0, 0, 0, 4000}};
    m.transport.network = ng::Network{4, {{0, 1, 0, 0.01, 1}, {1, 3, 45, 0, 1}, {0, 2, 45, 0, 1}, {2, 3, 0, 0.01, 1}}, {}};
    m.transport.node = {0, 3};
    m.transport.gap = 1e-10;
    m.transport.max_iterations = 1000000;
    m.beta = 0.05;
    m.lambda = 0.03;
    const auto y0 = ul::start(m);
    ul::Model with = m;
    with.transport.network.arcs.push_back({1, 2, 0, 0, 1});
    const auto before = ul::step(m, y0), after_road = ul::step(with, y0), after = ul::step(with, after_road);
    s.check("Braess in land: the journey from S to E takes 65 minutes before the road", before.costs(0, 1) - 65, 1e-6);
    s.check("... and 80 after it", after_road.costs(0, 1) - 80, 1e-3);
    s.check("... S loses 15 minutes of accessibility", (before.access[0] - after_road.access[0]) - 15, 1e-3);
    const auto again = ul::step(m, before);
    s.check("... and its rent falls by exp(−15 λ / α)", after.rents[0] / again.rents[0] - std::exp(-15 * m.lambda / m.market.alpha), 1e-5);
  }
  {
    // A grid of four by four crossings, a zone at each; jobs gathered at the centre; streets of the Bureau of Public Roads.
    ul::Model m;
    m.market = one_class(60000, 2);
    m.transport.network.nodes = 16;
    std::vector<std::size_t> into_centre;
    for (std::size_t r = 0; r < 4; r++)
      for (std::size_t c = 0; c < 4; c++) {
        const std::size_t v = r * 4 + c;
        const auto road = [&](std::size_t a, std::size_t b) {
          if ((b == 5 || b == 6 || b == 9 || b == 10) && !(a == 5 || a == 6 || a == 9 || a == 10)) into_centre.push_back(m.transport.network.arcs.size());
          m.transport.network.arcs.push_back(ng::bpr(a, b, 3, 900));
        };
        if (c + 1 < 4) road(v, v + 1), road(v + 1, v);
        if (r + 1 < 4) road(v, v + 4), road(v + 4, v);
      }
    m.zones.resize(16);
    for (std::size_t v = 0; v < 16; v++) {
      const bool centre = v == 5 || v == 6 || v == 9 || v == 10;
      m.zones[v] = ul::Zone{1500, 3500, 0, centre ? 6000.0 : 400.0};
      m.transport.node.push_back(v);
    }
    m.transport.added = ni::Table(16, 16, 2.0);   // two minutes at each end
    m.beta = 0.08;
    m.lambda = 0.05;
    m.service_per_household = 0.2;
    m.development = {300, 0.15, 0};
    m.market.reference_rent = 250;
    // Revealed from a year in which every zone holds 1,425 households at a rent of 320, a little above what building costs.
    ul::reveal(m, std::vector<double>(16, 1425.0), std::vector<double>(16, 320.0));
    // The road into the centre from the west (4 → 5 and 8 → 9) widened in year 2.
    std::vector<ul::Event> events;
    for (const std::size_t a : into_centre) {
      const auto& road = m.transport.network.arcs[a];
      if ((road.tail == 4 && road.head == 5) || (road.tail == 8 && road.head == 9)) {
        ul::Event e;
        e.kind = ul::Event::arc, e.year = 2, e.index = a, e.road = ng::bpr(road.tail, road.head, 3, 2700);
        events.push_back(e);
      }
    }
    s.require("The grid: two roads into the centre to widen", events.size() == 2);
    const auto plain = ul::run(m, {}, 8);
    const auto widened = ul::run(m, events, 8);
    bool converged = true;
    double cons = 0;
    for (const auto& y : widened) converged = converged && y.transport_gap <= m.transport.gap, cons = std::max(cons, conservation(m, y));
    s.require("The grid: Evans's equilibrium reached every year", converged);
    s.check("The grid: conservation every year", cons, 1e-9);
    s.require("The grid: the widened roads raise the accessibility of the zones west of the centre (4 and 8) in year 2",
              widened[2].access[4] > plain[2].access[4] && widened[2].access[8] > plain[2].access[8]);
    s.require("The grid: ... then their rents in year 3", widened[3].rents[4] > plain[3].rents[4] && widened[3].rents[8] > plain[3].rents[8]);
    s.require("The grid: ... and, building, their stock by year 8", widened[8].stock[4] > plain[8].stock[4] && widened[8].stock[8] > plain[8].stock[8]);
    s.require("The grid: the region holds more households", widened[8].outside < plain[8].outside);
    bool within = true;
    for (const auto& y : widened)
      for (std::size_t v = 0; v < 16; v++) within = within && y.stock[v] <= m.zones[v].capacity * (1 + 1e-12);
    s.require("The grid: building never passes capacity", within);
    // Raise the capacity of the dearest zone in year 1.
    std::size_t dear = 0;
    for (std::size_t v = 1; v < 16; v++)
      if (plain[1].rents[v] > plain[1].rents[dear]) dear = v;
    ul::Event up;
    up.kind = ul::Event::capacity, up.year = 1, up.index = dear, up.value = 7000;
    const auto upzoned = ul::run(m, {up}, 8);
    s.require("Raising the dearest zone's capacity gives it more dwellings and households, and a lower rent, by year 8",
              upzoned[8].stock[dear] > plain[8].stock[dear] && upzoned[8].households[dear] > plain[8].households[dear] && upzoned[8].rents[dear] < plain[8].rents[dear]);
  }
  {
    ul::Model m = base;
    m.development = {520, 0.3, 0};
    ul::reveal(m, observed, rents);
    for (std::size_t i = 0; i < n; i++) m.zones[i].capacity = m.zones[i].stock * st.uniform(1.1, 2.5);
    const auto y = ul::stationary(m, 1e-11, 5000);
    bool ok = true;
    for (std::size_t i = 0; i < n; i++) ok = ok && (y.stock[i] >= m.zones[i].capacity * (1 - 1e-6) || y.rents[i] <= m.development.cost_rent * (1 + 1e-6));
    s.require("The stationary state: each zone at capacity or with its rent at most the rent at which building pays", ok);
    ul::Model again = m;
    for (std::size_t i = 0; i < n; i++) again.zones[i].stock = y.stock[i];
    const auto next = ul::step(again, y);
    double e = 0;
    for (std::size_t i = 0; i < n; i++) e = std::max({e, std::fabs(next.households[i] - y.households[i]) / y.households[i], std::fabs(next.stock[i] - y.stock[i]) / y.stock[i]});
    s.check("The stationary state: a year from it changes nothing", e, 1e-8);
  }
  return s.done();
}
