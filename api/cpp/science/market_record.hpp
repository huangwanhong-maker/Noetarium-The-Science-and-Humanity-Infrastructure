// A record of a market read as a foam, for any work's engine (ADR 0099): one composition of the markets domain's `system`,
// `foam`, `valuation`, `money` and `procedure`, answering requests in JSON beside science/markets.hpp. The record is given in
// the request by whoever holds the data: nothing here names a place, a ledger or a file's columns.
//
//   A RECORD is stated as movements, column by column:
//     {"subjects": n, "kinds": K, "begin": t₀, "end": t₁,
//      "event": [...], "time": [...], "from": [...], "to": [...], "kind": [...], "quantity": [...],     movements by event, in time
//      "opening"?: [[...], ...],        each subject's position in each kind at the beginning (zero when not given)
//      "booked"?: [0 | 1, ...]}         for each kind, whether its positions are booked at a ledger (else bearer holdings)
//
//   generate     {"rules": {...}}                    a market from rules (markets::system::Rules): its record, and its truth
//   foam         {"record", "faces"?: most}          the record's complex: its cells, the boundary of its boundary, each subject's
//                                                    proper time, the worldline order's density and longest chain, the balance at
//                                                    every event; and its faces and events, for drawing, while they are few
//   reciprocity  {"record", "carry"?: [...]}         the reference valuation of its exchanges: a price for each kind, a residual for
//                                                    each exchange, an advantage for each subject; and its loops through time
//   circulation  {"record", "kind", "edges": [...]}  the stock of a kind, what moved, and the velocity, in each period between edges
//   injection    {"shares": [[...]], "site", "added", "money", "spending"?, "output"?, "horizon"?, "steps"?}
//                                                    money added at one site of an economy at rest: the gains, and the prices' rise
//   division     {"estate", "claims": [...]}         an estate short of its claims by every rule, and what the order alone can move
//   discrepancy  {"p": [...], "q": [...], "record": [...]}
//                                                    two laws on one set of histories, and the record each history leaves: what the
//                                                    record shows of their difference, and what it hides
//
// Quantities are the caller's units; times are numbers in the caller's clock.

#pragma once

#include "markets.hpp"

#include <markets/foam/foam.hpp>
#include <markets/foam/record.hpp>
#include <markets/money/money.hpp>
#include <markets/procedure/procedure.hpp>
#include <markets/system/system.hpp>
#include <markets/valuation/valuation.hpp>

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace science::markets {

namespace mf = ::markets::foam;
namespace mv = ::markets::valuation;

inline mf::Record record_of(const Json& r, std::vector<char>* booked = nullptr) {
  require(r.has("record"), "a request states its record");
  const Json& j = r["record"];
  mf::Record R;
  R.subjects = static_cast<std::size_t>(number_or(j, "subjects", 0));
  R.kinds = static_cast<std::size_t>(number_or(j, "kinds", 0));
  const auto event = indices(j["event"]), from = indices(j["from"]), to = indices(j["to"]), kind = indices(j["kind"]);
  const auto time = numbers(j["time"]), quantity = numbers(j["quantity"]);
  require(event.size() == time.size() && from.size() == time.size() && to.size() == time.size() && kind.size() == time.size() && quantity.size() == time.size(),
          "a movement has an event, a time, a giver, a taker, a kind and a quantity");
  for (std::size_t k = 0; k < time.size(); k++) R.movements.push_back({event[k], time[k], from[k], to[k], kind[k], quantity[k]});
  R.begin = number_or(j, "begin", time.empty() ? 0.0 : time.front());
  R.end = number_or(j, "end", time.empty() ? 0.0 : time.back());
  if (j.has("opening") && j["opening"].size() > 0)
    for (std::size_t s = 0; s < j["opening"].size(); s++) R.opening.push_back(numbers(j["opening"][s]));
  if (booked) {
    booked->clear();
    if (j.has("booked"))
      for (std::size_t k = 0; k < j["booked"].size(); k++) booked->push_back(j["booked"][k].as_number() != 0);
  }
  mf::validate(R);
  return R;
}

inline Json written_record(const mf::Record& R) {
  std::vector<std::size_t> event, from, to, kind;
  std::vector<double> time, quantity;
  for (const auto& m : R.movements) event.push_back(m.event), from.push_back(m.from), to.push_back(m.to), kind.push_back(m.kind), time.push_back(m.time), quantity.push_back(m.quantity);
  Json opening = Json::array();
  for (const auto& row : R.opening) opening.push(written(row));
  return Json::object({{"subjects", R.subjects}, {"kinds", R.kinds}, {"begin", R.begin}, {"end", R.end}, {"event", written(event)}, {"time", written(time)},
                       {"from", written(from)}, {"to", written(to)}, {"kind", written(kind)}, {"quantity", written(quantity)}, {"opening", opening}});
}

inline Json op_generate(const Json& r) {
  ::markets::system::Rules P;
  if (r.has("rules")) {
    const Json& j = r["rules"];
    P.subjects = static_cast<std::size_t>(number_or(j, "subjects", 30));
    P.goods = static_cast<std::size_t>(number_or(j, "goods", 2));
    P.horizon = number_or(j, "horizon", 50);
    P.velocity = number_or(j, "velocity", 2.5);
    P.payment = number_or(j, "payment", 1);
    P.noise = number_or(j, "noise", 0.02);
    P.barter = number_or(j, "barter", 0.2);
    P.money_each = number_or(j, "money_each", 20);
    P.goods_each = number_or(j, "goods_each", 200);
    P.ring = j.has("ring") && j["ring"].is_bool() && j["ring"].as_bool();
    P.seed = static_cast<std::uint64_t>(number_or(j, "seed", 20261113));
    if (j.has("price")) P.price = numbers(j["price"]);
    if (j.has("bargaining")) P.bargaining = numbers(j["bargaining"]);
  }
  require(P.subjects <= 2000 && P.goods <= 50, "at most 2,000 subjects and 50 goods");
  require(P.horizon * (P.velocity * P.money_each * static_cast<double>(P.subjects) / P.payment + P.barter * static_cast<double>(P.subjects)) <= 400000, "the rules would make more than 400,000 events: shorten the horizon");
  const auto G = ::markets::system::generate(P);
  return Json::object({{"record", written_record(G.record)}, {"price", written(G.price)}, {"bargaining", written(G.bargaining)}, {"money", G.money}});
}

inline Json op_foam(const Json& r) {
  std::vector<char> booked;
  const auto R = record_of(r, &booked);
  const auto P = mf::history(R, booked);
  const auto& H = P.history;
  const auto F = mf::foam(H);
  double worst = 0;
  for (const auto& row : mf::imbalance(H, R.kinds))
    for (double d : row) worst = std::max(worst, std::fabs(d));
  std::vector<double> proper(R.subjects, 0.0), area(R.kinds, 0.0);
  for (std::size_t s = 0; s < R.subjects; s++) proper[s] = static_cast<double>(F.worldline[s].size());
  for (const auto& f : F.faces) area[H.facts[f.fact].kind] += std::fabs(f.area());
  Json ledgers = Json::array();
  for (std::size_t k = 0; k < R.kinds; k++)
    if (P.ledger[k] != mf::none) ledgers.push(Json::object({{"kind", k}, {"events", F.worldline[P.ledger[k]].size()}}));
  Json answer = Json::object({{"events", F.events}, {"junctions", F.junctions}, {"vertices", F.cw.vertices}, {"edges", F.cw.edges.size()}, {"faces", F.faces.size()},
                              {"facts", H.facts.size()}, {"euler", static_cast<double>(F.cw.vertices) - static_cast<double>(F.cw.edges.size()) + static_cast<double>(F.faces.size())},
                              {"boundary_defect", mf::boundary_defect(F)}, {"imbalance", worst}, {"proper_time", written(proper)}, {"area", written(area)}, {"ledgers", ledgers}});
  if (F.events <= 12000) {
    const auto O = mf::order(F);
    answer.set("density", O.density());
    answer.set("longest", O.longest());
  }
  // The faces and the events, for a drawing, while they are few.
  const std::size_t most = static_cast<std::size_t>(number_or(r, "faces", 0));
  if (most > 0 && F.faces.size() <= most) {
    std::vector<std::size_t> subject, kind;
    std::vector<double> begin, end, value;
    for (const auto& f : F.faces) subject.push_back(H.facts[f.fact].a), kind.push_back(H.facts[f.fact].kind), begin.push_back(f.begin), end.push_back(f.end), value.push_back(f.value);
    Json events = Json::array();
    for (std::size_t v = 0; v < F.events; v++) {
      std::vector<std::size_t> at;
      for (std::size_t x : F.touched[v])
        if (x < R.subjects) at.push_back(x);
      events.push(Json::object({{"time", H.events[v].time}, {"subjects", written(at)}, {"movements", H.events[v].type}}));
    }
    answer.set("drawing", Json::object({{"subject", written(subject)}, {"kind", written(kind)}, {"begin", written(begin)}, {"end", written(end)}, {"value", written(value)}, {"events", events}}));
  }
  return answer;
}

inline Json op_reciprocity(const Json& r) {
  const auto R = record_of(r);
  const auto X = mf::exchanges(R);
  std::vector<mv::Trade> trades;
  for (const auto& e : X) trades.push_back({e.a, e.b, e.ka, e.kb, e.log_ratio(), e.time, 1.0});
  const auto V = mv::reference(trades, R.kinds, R.subjects);
  std::vector<std::size_t> event, giver, taker, k, l, count(R.subjects, 0);
  std::vector<double> time, ratio;
  for (const auto& e : X) {
    event.push_back(e.event), giver.push_back(e.a), taker.push_back(e.b), k.push_back(e.ka), l.push_back(e.kb), time.push_back(e.time), ratio.push_back(e.log_ratio());
    count[e.a]++, count[e.b]++;
  }
  Json answer = Json::object({{"exchanges", X.size()}, {"potential", written(V.potential)}, {"residual", written(V.residual)}, {"advantage", written(V.advantage)},
                              {"trades_of", written(count)}, {"fitted", V.fitted}, {"unfitted", V.unfitted},
                              {"event", written(event)}, {"giver", written(giver)}, {"taker", written(taker)}, {"k", written(k)}, {"l", written(l)},
                              {"time", written(time)}, {"log_ratio", written(ratio)}});
  // The loops through time, with the carry of holding each kind where it is given.
  const auto carry = r.has("carry") ? numbers(r["carry"]) : std::vector<double>{};
  require(carry.empty() || carry.size() == R.kinds, "a carry for each kind, or none");
  const auto L = mv::lenses(trades, carry);
  std::vector<std::size_t> lk, ll;
  std::vector<double> begin, end, geometric, dynamical;
  for (const auto& x : L) lk.push_back(x.k), ll.push_back(x.l), begin.push_back(x.begin), end.push_back(x.end), geometric.push_back(x.geometric), dynamical.push_back(x.dynamical);
  answer.set("lenses", Json::object({{"k", written(lk)}, {"l", written(ll)}, {"begin", written(begin)}, {"end", written(end)}, {"geometric", written(geometric)}, {"dynamical", written(dynamical)}}));
  return answer;
}

inline Json op_circulation(const Json& r) {
  const auto R = record_of(r);
  const std::size_t kind = static_cast<std::size_t>(number_or(r, "kind", 0));
  require(kind < R.kinds, "a kind of the record");
  auto edges = r.has("edges") ? numbers(r["edges"]) : std::vector<double>{R.begin, R.end};
  require(edges.size() >= 2 && std::is_sorted(edges.begin(), edges.end()), "the edges of the periods, in time");
  const auto S = mf::stock(R, kind);
  const std::size_t n = edges.size() - 1;
  std::vector<double> moved(n, 0.0), area(n, 0.0), count(n, 0.0);
  // The area of the stock in each period: the step function integrated between the edges.
  for (std::size_t k = 0; k < S.time.size(); k++) {
    const double a = S.time[k], b = k + 1 < S.time.size() ? S.time[k + 1] : R.end;
    for (std::size_t p = 0; p < n; p++) {
      const double lo = std::max(a, edges[p]), hi = std::min(b, edges[p + 1]);
      if (hi > lo) area[p] += S.level[k] * (hi - lo);
    }
  }
  for (const auto& m : R.movements) {
    if (m.kind != kind) continue;
    const auto at = std::upper_bound(edges.begin(), edges.end(), m.time) - edges.begin();
    const std::size_t p = at == 0 ? n : static_cast<std::size_t>(at) - 1;
    if (p < n || (p == n && m.time == edges.back())) {
      const std::size_t q = std::min(p, n - 1);
      moved[q] += std::fabs(m.quantity);
      count[q] += 1;
    }
  }
  std::vector<double> velocity(n), stock(n);
  for (std::size_t p = 0; p < n; p++) {
    velocity[p] = area[p] > 0 ? moved[p] / area[p] : std::nan("");
    stock[p] = edges[p + 1] > edges[p] ? area[p] / (edges[p + 1] - edges[p]) : std::nan("");
  }
  return Json::object({{"moved", written(moved)}, {"area", written(area)}, {"movements", written(count)}, {"velocity", written(velocity)}, {"stock", written(stock)},
                       {"overall", Json::object({{"moved", S.moved}, {"area", S.integral}, {"velocity", S.integral > 0 ? Json(S.moved / S.integral) : Json()}, {"movements", S.movements}})}});
}

inline Json op_injection(const Json& r) {
  require(r.has("shares") && r["shares"].size() > 0, "the shares of each site's spending that go to each other site");
  ::markets::money::Economy E;
  const std::size_t n = r["shares"].size();
  require(n <= 400, "at most 400 sites");
  E.W = calx::linalg::Matrix(n);
  for (std::size_t i = 0; i < n; i++) {
    const auto row = numbers(r["shares"][i]);
    require(row.size() == n, "a share for each pair of sites");
    for (std::size_t j = 0; j < n; j++) E.W(i, j) = row[j];
  }
  E.spending = number_or(r, "spending", 1);
  E.output = number_or(r, "output", 1);
  const std::size_t site = static_cast<std::size_t>(number_or(r, "site", 0));
  const double money = number_or(r, "money", static_cast<double>(n)), added = number_or(r, "added", 0.01 * money / static_cast<double>(n));
  const double horizon = number_or(r, "horizon", 200 / E.spending);
  std::size_t steps = static_cast<std::size_t>(number_or(r, "steps", 4000));
  steps += steps % 2;
  const auto I = ::markets::money::injection(E, money, site, added, horizon, steps);
  const bool balanced = ::markets::money::balanced(E);
  Json answer = Json::object({{"gain", written(I.gain)}, {"half_rise", written(I.half_rise)}, {"price_before", written(I.price_before)}, {"price_after", written(I.price_after)},
                              {"conserved", I.total_consumed_defect}, {"balanced", balanced}});
  if (balanced) answer.set("first_order", written(::markets::money::first_order_gains(E, site, added, I.price_before[site])));
  // The prices' paths, thinned to at most 200 instants.
  const std::size_t stride = std::max<std::size_t>(1, I.time.size() / 200);
  Json times = Json::array(), price = Json::array();
  for (std::size_t k = 0; k < I.time.size(); k += stride) {
    times.push(I.time[k]);
    price.push(written(I.price[k]));
  }
  answer.set("time", times);
  answer.set("price", price);
  return answer;
}

inline Json op_division(const Json& r) {
  ::nomos::games::Claims P{number_or(r, "estate", 0), numbers(r["claims"])};
  const auto D = ::markets::procedure::divide(P);
  return Json::object({{"least", written(D.least)}, {"greatest", written(D.greatest)}, {"random_arrival", written(D.random_arrival)}, {"proportional", written(D.proportional)},
                       {"equal_awards", written(D.equal_awards)}, {"equal_losses", written(D.equal_losses)}, {"talmud", written(D.talmud)}});
}

inline Json op_discrepancy(const Json& r) {
  const auto p = numbers(r["p"]), q = numbers(r["q"]);
  const auto cell = indices(r["record"]);
  require(p.size() == q.size() && cell.size() == p.size(), "two weights and a record for each history");
  const auto d = ::tyche::probability::discrepancy(p, q, ::tyche::probability::coarsening(cell));
  return Json::object({{"total", std::isfinite(d.total) ? Json(d.total) : Json()}, {"visible", std::isfinite(d.visible) ? Json(d.visible) : Json()},
                       {"hidden", std::isfinite(d.hidden) ? Json(d.hidden) : Json()}, {"infinite", !std::isfinite(d.total)},
                       {"record_p", written(d.record_p)}, {"record_q", written(d.record_q)}, {"hidden_at", written(d.hidden_at)}});
}

inline bool record_op(const std::string& op) {
  for (const char* known : {"generate", "foam", "reciprocity", "circulation", "injection", "division", "discrepancy"})
    if (op == known) return true;
  return false;
}

inline Json record_answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "generate") return op_generate(r);
  if (op == "foam") return op_foam(r);
  if (op == "reciprocity") return op_reciprocity(r);
  if (op == "circulation") return op_circulation(r);
  if (op == "injection") return op_injection(r);
  if (op == "division") return op_division(r);
  if (op == "discrepancy") return op_discrepancy(r);
  throw std::invalid_argument("no operation \"" + op + "\"");
}

}  // namespace science::markets
