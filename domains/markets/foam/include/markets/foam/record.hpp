// A record of a market as movements, and the history it is (theory note, Part II §2).
//
// What a ledger, a register of payments or a book of trades holds is movements: at a time, a quantity of a kind passes from one
// subject to another. Movements that happen together are one event: a transfer is one movement, an exchange two in opposite
// directions, an issuance one from the issuer of a claim (whose own position in it goes below zero by as much). Kinds are
// labels; a subject's position in a kind is a fact, and the record is a history of those facts.
//
// Where a position is booked is a choice of presentation, and it decides the causal structure (Proposition S.13 and §7.2 of
// the source): a kind held as bearer holdings makes each position a fact with one end, at its holder; a kind booked at a ledger
// makes each position a fact between its holder and that ledger, a junction every movement of the kind touches.
//
//   history         the record as a history: junctions the subjects, then a ledger for each kind booked at one; facts the
//                   positions that ever move, each standing from the beginning at its opening quantity (zero unless given)
//   exchanges       the events that are exchanges: two movements in opposite directions between two subjects, of two kinds
//   stock           the positive positions in a kind, added up, at each event and integrated over the history
//
// Positions are signed: a position below zero is a liability, as the issuer's in its own claim (the group of differences of
// double-entry bookkeeping; Ellerman 2014). A record keeps every kind's total whatever its movements are.

#pragma once

#include <markets/foam/foam.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace markets::foam {

struct Movement {
  std::size_t event = 0;  // movements of one event share its index; events are in time
  double time = 0;
  std::size_t from = 0, to = 0, kind = 0;
  double quantity = 0;
};

struct Record {
  std::size_t subjects = 0, kinds = 0;
  std::vector<Movement> movements;  // by event, in time
  double begin = 0, end = 0;
  std::vector<std::vector<double>> opening;  // subject -> kind -> its position at the beginning; empty for zero
};

inline void validate(const Record& R) {
  double last = R.begin;
  std::size_t event = 0;
  for (std::size_t k = 0; k < R.movements.size(); k++) {
    const auto& m = R.movements[k];
    if (m.from >= R.subjects || m.to >= R.subjects || m.kind >= R.kinds) throw std::invalid_argument("markets::foam::Record: a movement is between subjects of the record, of one of its kinds");
    if (m.from == m.to) throw std::invalid_argument("markets::foam::Record: a movement is between two subjects");
    if (!std::isfinite(m.quantity)) throw std::invalid_argument("markets::foam::Record: a movement has a quantity");
    if (k > 0 && m.event < event) throw std::invalid_argument("markets::foam::Record: movements by event, in order");
    if (!(m.time >= last) || !(m.time <= R.end)) throw std::invalid_argument("markets::foam::Record: events in time, within the record");
    if (k > 0 && m.event == event && m.time != last) throw std::invalid_argument("markets::foam::Record: the movements of one event are at one time");
    event = m.event;
    last = m.time;
  }
  if (!R.opening.empty() && R.opening.size() != R.subjects) throw std::invalid_argument("markets::foam::Record: opening positions for each subject, or none");
}

struct Presented {
  History history;
  std::vector<std::size_t> ledger;                             // kind -> its ledger's junction, or none (bearer holdings)
  std::map<std::pair<std::size_t, std::size_t>, std::size_t> position;  // (subject, kind) -> its fact
  std::vector<std::size_t> event_of;                           // the history's event -> the record's event index
};

// The record as a history. `booked[k]` says whether kind k is booked at a ledger (else held as bearer holdings); empty for none.
inline Presented history(const Record& R, const std::vector<char>& booked = {}) {
  validate(R);
  if (!booked.empty() && booked.size() != R.kinds) throw std::invalid_argument("markets::foam::history: for each kind, whether it is booked at a ledger");
  Presented P;
  auto& H = P.history;
  H.begin = R.begin;
  H.end = R.end;
  H.junctions = R.subjects;
  P.ledger.assign(R.kinds, none);
  for (std::size_t k = 0; k < R.kinds; k++)
    if (!booked.empty() && booked[k]) P.ledger[k] = H.junctions++;
  std::vector<double> x;  // the positions, by fact
  const auto fact_of = [&](std::size_t s, std::size_t k) {
    const auto found = P.position.find({s, k});
    if (found != P.position.end()) return found->second;
    Fact f;
    f.a = s;
    f.b = P.ledger[k];
    f.kind = k;
    f.value = R.opening.empty() ? 0.0 : R.opening[s].at(k);
    H.facts.push_back(f);
    x.push_back(f.value);
    return P.position[{s, k}] = H.facts.size() - 1;
  };
  for (std::size_t k = 0; k < R.movements.size();) {
    Event e;
    e.time = R.movements[k].time;
    std::map<std::size_t, double> change;  // fact -> its change at this event
    std::size_t j = k;
    for (; j < R.movements.size() && R.movements[j].event == R.movements[k].event; j++) {
      const auto& m = R.movements[j];
      change[fact_of(m.from, m.kind)] -= m.quantity;
      change[fact_of(m.to, m.kind)] += m.quantity;
    }
    e.type = j - k;  // the number of its movements: 1 a transfer, 2 an exchange
    for (const auto& [r, d] : change) {
      x[r] += d;
      e.touches.push_back({r, Touch::modify, x[r]});
    }
    P.event_of.push_back(R.movements[k].event);
    H.events.push_back(std::move(e));
    k = j;
  }
  return P;
}

// An exchange: at one event, a gives the quantity qa of kind ka to b, and b the quantity qb of kind kb to a.
struct Exchange {
  std::size_t event = 0;  // the record's event index
  double time = 0;
  std::size_t a = 0, b = 0, ka = 0, kb = 0;
  double qa = 0, qb = 0;
  double log_ratio() const { return std::log(qb / qa); }  // the units of kb obtained for a unit of ka
};

inline std::vector<Exchange> exchanges(const Record& R) {
  validate(R);
  std::vector<Exchange> out;
  for (std::size_t k = 0; k < R.movements.size();) {
    std::size_t j = k;
    while (j < R.movements.size() && R.movements[j].event == R.movements[k].event) j++;
    if (j - k == 2) {
      const auto &m = R.movements[k], &w = R.movements[k + 1];
      if (m.from == w.to && m.to == w.from && m.kind != w.kind && m.quantity > 0 && w.quantity > 0) out.push_back({m.event, m.time, m.from, m.to, m.kind, w.kind, m.quantity, w.quantity});
    }
    k = j;
  }
  return out;
}

// The stock of a kind: its positive positions added up, as a step function of time, and its integral over the record.
struct Stock {
  std::vector<double> time, level;  // level[k] holds from time[k] to time[k + 1] (the last to the record's end)
  double integral = 0;              // ∫ level dt over the record
  double moved = 0;                 // the quantity of the kind that passed between subjects
  std::size_t movements = 0;
};

inline Stock stock(const Record& R, std::size_t kind) {
  validate(R);
  if (kind >= R.kinds) throw std::invalid_argument("markets::foam::stock: a kind of the record");
  std::vector<double> x(R.subjects, 0.0);
  if (!R.opening.empty())
    for (std::size_t s = 0; s < R.subjects; s++) x[s] = R.opening[s].at(kind);
  const auto positive = [&] { double m = 0; for (double v : x) m += std::max(0.0, v); return m; };
  Stock S;
  S.time.push_back(R.begin);
  S.level.push_back(positive());
  double t = R.begin;
  for (std::size_t k = 0; k < R.movements.size();) {
    std::size_t j = k;
    bool touched = false;
    for (; j < R.movements.size() && R.movements[j].event == R.movements[k].event; j++) {
      const auto& m = R.movements[j];
      if (m.kind != kind) continue;
      x[m.from] -= m.quantity;
      x[m.to] += m.quantity;
      S.moved += std::fabs(m.quantity);
      S.movements++;
      touched = true;
    }
    if (touched) {
      S.integral += S.level.back() * (R.movements[k].time - t);
      t = R.movements[k].time;
      S.time.push_back(t);
      S.level.push_back(positive());
    }
    k = j;
  }
  S.integral += S.level.back() * (R.end - t);
  return S;
}

}  // namespace markets::foam
