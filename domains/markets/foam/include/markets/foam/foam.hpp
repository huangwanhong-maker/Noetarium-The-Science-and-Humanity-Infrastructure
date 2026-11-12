// The market foam: a history of a market as one labelled two-complex (theory note, Part II).
//
// A market state is a graph: its nodes are junctions (a subject, a venue, a ledger: the standing meeting of the facts that
// end there) and its links are facts (a holding, a claim, an offer, an access, a belief: whatever persists), each with a type
// and a value. A history is a state and events in time, each touching some facts: creating, removing or modifying them, or
// reading them. From it, with no further choice, the history complex:
//
//   vertices   the events; and for each junction a point at the beginning and at the end of the history
//   edges      the segments of each junction's worldline between consecutive events that touch it; and the links of the
//              first and of the last state
//   faces      for each fact, each interval between consecutive events that touch it: bounded by the two ends' worldlines
//              over the interval, pinched at an event (which touches both ends) and closed by a link at the history's ends
//
// A face is a persistent fact, an edge a junction, a vertex an event, the boundary the two states. Between events the complex
// is morphe's worldline complex of the record of touches (junctions as participants, facts as pairs, touches as meetings);
// the points and links at the two ends are what this construction adds, so that its boundary is the states themselves.
//
// What is read from it:
//
//   slice            the state at a time, from the faces that hold it (Proposition S.3)
//   boundary_defect  max |∂₁∂₂| over the faces: zero (Proposition S.2)
//   worldline order  an event directly precedes the next on a junction's worldline; `Order` holds its closure, the share of
//                    pairs of events it relates (the causal density), and its longest chain
//   proper time      of a junction: the events on its worldline (Definition 7.1)
//   imbalance        at each event, for each kind, the labels after less the labels before over the facts touched: zero where
//                    the kind is conserved (Proposition S.6a)
//   at_junction      the labels of a kind on the faces meeting a junction's worldline at a time: at a ledger where a kind is
//                    booked, the kind's total (the Gauss law, Proposition S.6b)
//   area             of a face, its label times its duration; `area_weight` the exponent Σ h_f j_f |f| of the path weight
//                    when each face generates events at a hazard in proportion to its label (Proposition S.9)
//
// An event is of zero duration; what lasts is a fact. A fact has two ends; one given a single end is completed by a private
// junction that lives as long as it does. Junctions given by the caller stand for the whole history.
//
// Source: W. Huang (2026), Market Foam: A Labelled Two-Complex Model of Market Spacetime With Heterogeneous Facts, v0.9a
// (Definitions 4.1 to 4.3, Propositions S.2 to S.6, S.9, S.12, S.13).

#pragma once

#include <morphe/topology/cw_complex.hpp>

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <stdexcept>
#include <vector>

namespace markets::foam {

inline constexpr std::size_t none = static_cast<std::size_t>(-1);

struct Fact {
  std::size_t a = 0, b = none;  // the junctions it ends at; none: a private terminal junction
  std::size_t type = 0;         // the caller's fact types, by index
  std::size_t kind = none;      // the kind its value is a quantity of (a component of the charge), or none
  double value = 0;             // its label at the beginning of the history
  bool standing = true;         // whether it stands at the beginning (else an event creates it)
};

struct Touch {
  enum How { modify, create, remove, read };
  std::size_t fact = 0;
  How how = modify;
  double value = 0;  // the fact's label after the event (as before, for a reading)
};

struct Event {
  double time = 0;
  std::size_t type = 0;                // the caller's event types, by index
  std::vector<Touch> touches;          // the facts it touches, each once
  std::vector<std::size_t> junctions;  // junctions it touches besides the ends of those facts
};

struct History {
  std::size_t junctions = 0;
  std::vector<Fact> facts;
  std::vector<Event> events;  // in time
  double begin = 0, end = 0;
};

struct Segment {
  std::size_t junction = 0, from = 0, to = 0;  // vertices
  double begin = 0, end = 0;
};

struct Face {
  std::size_t fact = 0;
  std::size_t from = none, to = none;  // the events that begin and end it; none at the history's ends
  double begin = 0, end = 0, value = 0;
  double area() const { return value * (end - begin); }
};

struct Foam {
  morphe::topology::CWComplex cw;  // vertices: the events, then the junctions' points; edges: the segments, then the links
  std::size_t events = 0, junctions = 0;         // junctions: the caller's, then the private ones
  std::vector<std::size_t> bottom, top;          // junction -> its point at the beginning and at the end, or none
  std::vector<std::vector<std::size_t>> worldline;    // junction -> the events that touch it, in time
  std::vector<Segment> segments;                       // edge e < segments.size() is segments[e]
  std::vector<std::vector<std::size_t>> segments_of;   // junction -> its segments, in time
  std::vector<std::size_t> first_link, last_link;      // fact -> its link in the first and in the last state (an edge), or none
  std::vector<Face> faces;                             // 2-cell f is faces[f]
  std::vector<std::vector<std::size_t>> faces_of;      // fact -> its faces, in time
  std::vector<std::size_t> end_a, end_b;               // fact -> its two junctions (the second private where none was given)
  std::vector<std::vector<std::size_t>> touched;       // event -> the junctions it touches, in increasing order
};

inline void validate(const History& H) {
  if (!(H.end >= H.begin)) throw std::invalid_argument("markets::foam::History: it ends no earlier than it begins");
  for (const auto& f : H.facts)
    if (f.a >= H.junctions || (f.b != none && f.b >= H.junctions)) throw std::invalid_argument("markets::foam::History: a fact ends at a junction of the history");
  std::vector<char> alive(H.facts.size());
  for (std::size_t r = 0; r < H.facts.size(); r++) alive[r] = H.facts[r].standing;
  double last = H.begin;
  for (const auto& e : H.events) {
    if (!(e.time >= last) || !(e.time <= H.end)) throw std::invalid_argument("markets::foam::History: events in time, within the history");
    last = e.time;
    std::vector<std::size_t> seen;
    for (const auto& t : e.touches) {
      if (t.fact >= H.facts.size()) throw std::invalid_argument("markets::foam::History: an event touches a fact of the history");
      if (std::find(seen.begin(), seen.end(), t.fact) != seen.end()) throw std::invalid_argument("markets::foam::History: an event touches a fact once");
      seen.push_back(t.fact);
      if (t.how == Touch::create) {
        if (alive[t.fact]) throw std::invalid_argument("markets::foam::History: a fact that stands cannot be created");
        alive[t.fact] = 1;
      } else {
        if (!alive[t.fact]) throw std::invalid_argument("markets::foam::History: a fact is touched while it stands");
        if (t.how == Touch::remove) alive[t.fact] = 0;
      }
    }
    for (std::size_t x : e.junctions)
      if (x >= H.junctions) throw std::invalid_argument("markets::foam::History: an event touches a junction of the history");
  }
}

inline Foam foam(const History& H) {
  validate(H);
  Foam F;
  const std::size_t n = H.events.size(), R = H.facts.size();
  F.events = n;
  // The junctions: the caller's, and a private one for each fact with a single end.
  F.end_a.resize(R);
  F.end_b.resize(R);
  F.junctions = H.junctions;
  for (std::size_t r = 0; r < R; r++) {
    F.end_a[r] = H.facts[r].a;
    F.end_b[r] = H.facts[r].b == none ? F.junctions++ : H.facts[r].b;
  }
  const std::size_t J = F.junctions;
  // The lifetime of each fact, as the events that create and remove it (none at the history's ends), and its touches.
  std::vector<std::vector<std::size_t>> touches(R);  // fact -> events, in time
  std::vector<std::vector<double>> after(R);         // its label after each
  std::vector<std::size_t> born(R, none), died(R, none);
  std::vector<char> ever(R, 0);
  for (std::size_t r = 0; r < R; r++) ever[r] = H.facts[r].standing;
  F.touched.assign(n, {});
  for (std::size_t v = 0; v < n; v++) {
    auto& at = F.touched[v];
    for (const auto& t : H.events[v].touches) {
      touches[t.fact].push_back(v);
      after[t.fact].push_back(t.value);
      if (t.how == Touch::create) {
        if (ever[t.fact]) throw std::invalid_argument("markets::foam::foam: a fact lives once: give a fact created again its own index");
        ever[t.fact] = 1;
        born[t.fact] = v;
      }
      if (t.how == Touch::remove) died[t.fact] = v;
      at.push_back(F.end_a[t.fact]);
      at.push_back(F.end_b[t.fact]);
    }
    for (std::size_t x : H.events[v].junctions) at.push_back(x);
    std::sort(at.begin(), at.end());
    at.erase(std::unique(at.begin(), at.end()), at.end());
  }
  // Each junction's worldline: its point at the beginning (unless it is the private end of a fact created later), the
  // events that touch it, its point at the end (unless its fact was removed).
  F.worldline.assign(J, {});
  for (std::size_t v = 0; v < n; v++)
    for (std::size_t x : F.touched[v]) F.worldline[x].push_back(v);
  std::vector<char> from_start(J, 1), to_end(J, 1);
  for (std::size_t r = 0; r < R; r++) {
    if (H.facts[r].b != none) continue;
    from_start[F.end_b[r]] = H.facts[r].standing;
    to_end[F.end_b[r]] = ever[r] && died[r] == none;
    if (!ever[r]) from_start[F.end_b[r]] = to_end[F.end_b[r]] = 0;  // a fact that never stands has no private junction in the complex
  }
  F.bottom.assign(J, none);
  F.top.assign(J, none);
  std::size_t vertices = n;
  for (std::size_t x = 0; x < J; x++) {
    if (from_start[x]) F.bottom[x] = vertices++;
    if (to_end[x]) F.top[x] = vertices++;
  }
  F.cw.vertices = vertices;
  const auto when = [&](std::size_t vertex, std::size_t x) { return vertex < n ? H.events[vertex].time : vertex == F.bottom[x] ? H.begin : H.end; };
  F.segments_of.assign(J, {});
  for (std::size_t x = 0; x < J; x++) {
    std::vector<std::size_t> points;
    if (F.bottom[x] != none) points.push_back(F.bottom[x]);
    points.insert(points.end(), F.worldline[x].begin(), F.worldline[x].end());
    if (F.top[x] != none) points.push_back(F.top[x]);
    for (std::size_t k = 0; k + 1 < points.size(); k++) {
      F.segments_of[x].push_back(F.segments.size());
      F.segments.push_back({x, points[k], points[k + 1], when(points[k], x), when(points[k + 1], x)});
      F.cw.edges.push_back({points[k], points[k + 1]});
    }
  }
  // The links of the first and of the last state.
  F.first_link.assign(R, none);
  F.last_link.assign(R, none);
  for (std::size_t r = 0; r < R; r++)
    if (H.facts[r].standing) {
      F.first_link[r] = F.cw.edges.size();
      F.cw.edges.push_back({F.bottom[F.end_a[r]], F.bottom[F.end_b[r]]});
    }
  for (std::size_t r = 0; r < R; r++)
    if (ever[r] && died[r] == none) {
      F.last_link[r] = F.cw.edges.size();
      F.cw.edges.push_back({F.top[F.end_a[r]], F.top[F.end_b[r]]});
    }
  // The segments of a junction between two of its points, by their places on its worldline.
  const auto place = [&](std::size_t x, std::size_t vertex) -> std::size_t {
    const std::size_t lead = F.bottom[x] != none ? 1 : 0;
    if (vertex == F.bottom[x]) return 0;
    if (vertex == F.top[x]) return lead + F.worldline[x].size();
    const auto& w = F.worldline[x];
    return lead + static_cast<std::size_t>(std::lower_bound(w.begin(), w.end(), vertex) - w.begin());
  };
  // The faces: each fact between consecutive touches, and to the history's ends.
  F.faces_of.assign(R, {});
  for (std::size_t r = 0; r < R; r++) {
    if (!ever[r]) continue;
    const std::size_t a = F.end_a[r], b = F.end_b[r];
    std::vector<std::size_t> cuts;  // the vertices that bound its faces, on a's worldline
    std::vector<double> label;      // its label on each face
    if (H.facts[r].standing) {
      cuts.push_back(none);
      label.push_back(H.facts[r].value);
    }
    for (std::size_t k = 0; k < touches[r].size(); k++) {
      cuts.push_back(touches[r][k]);
      label.push_back(after[r][k]);
    }
    const bool open = died[r] == none;  // it stands at the end
    for (std::size_t k = 0; k < cuts.size(); k++) {
      const bool last = k + 1 == cuts.size();
      if (last && !open) break;
      Face face;
      face.fact = r;
      face.from = cuts[k];
      face.to = last ? none : cuts[k + 1];
      face.begin = face.from == none ? H.begin : H.events[face.from].time;
      face.end = face.to == none ? H.end : H.events[face.to].time;
      face.value = label[k];
      const std::size_t a0 = face.from == none ? F.bottom[a] : face.from, a1 = face.to == none ? F.top[a] : face.to;
      const std::size_t b0 = face.from == none ? F.bottom[b] : face.from, b1 = face.to == none ? F.top[b] : face.to;
      morphe::topology::Face cell;
      cell.base = a0;
      for (std::size_t s = place(a, a0); s < place(a, a1); s++) cell.boundary.push_back({F.segments_of[a][s], 1});
      if (face.to == none) cell.boundary.push_back({F.last_link[r], 1});
      for (std::size_t s = place(b, b1); s-- > place(b, b0);) cell.boundary.push_back({F.segments_of[b][s], -1});
      if (face.from == none) cell.boundary.push_back({F.first_link[r], -1});
      F.faces_of[r].push_back(F.faces.size());
      F.faces.push_back(face);
      F.cw.faces.push_back(std::move(cell));
    }
  }
  return F;
}

// max over the faces of |∂₁ ∂₂|: the boundary of each face's boundary, at every vertex.
inline double boundary_defect(const Foam& F) {
  double worst = 0;
  std::vector<double> at(F.cw.vertices, 0.0);
  for (const auto& cell : F.cw.faces) {
    for (const auto& s : cell.boundary) {
      at[F.cw.edges[s.edge][1]] += s.sign;
      at[F.cw.edges[s.edge][0]] -= s.sign;
    }
    for (const auto& s : cell.boundary)
      for (std::size_t v : F.cw.edges[s.edge]) {
        worst = std::max(worst, std::fabs(at[v]));
        at[v] = 0;
      }
  }
  return worst;
}

// The state at a time, replayed from the history: each fact's label, NaN where it does not stand. At the time of an event the
// state is the one after it.
inline std::vector<double> state(const History& H, double t) {
  const double nan = std::numeric_limits<double>::quiet_NaN();
  std::vector<double> x(H.facts.size());
  for (std::size_t r = 0; r < x.size(); r++) x[r] = H.facts[r].standing ? H.facts[r].value : nan;
  for (const auto& e : H.events) {
    if (e.time > t) break;
    for (const auto& u : e.touches) x[u.fact] = u.how == Touch::remove ? nan : u.value;
  }
  return x;
}

// The same state read from the complex: the faces that contain the time.
inline std::vector<double> slice(const Foam& F, double t) {
  std::vector<double> x(F.faces_of.size(), std::numeric_limits<double>::quiet_NaN());
  for (std::size_t r = 0; r < F.faces_of.size(); r++)
    for (std::size_t f : F.faces_of[r])
      if (F.faces[f].begin <= t && (t < F.faces[f].end || (F.faces[f].to == none && t <= F.faces[f].end))) x[r] = F.faces[f].value;
  return x;
}

// The worldline order of the events.
struct Order {
  std::size_t events = 0;
  std::vector<std::vector<std::uint64_t>> before;  // before[v]: the events that precede v, as bits
  std::vector<std::size_t> depth;                  // the longest chain of direct precedences ending at v
  bool precedes(std::size_t u, std::size_t v) const { return u != v && ((before[v][u >> 6] >> (u & 63)) & 1); }
  // The share of the pairs of distinct events that the order relates.
  double density() const {
    if (events < 2) return 0;
    double related = 0;
    for (const auto& row : before)
      for (std::uint64_t w : row) related += static_cast<double>(__builtin_popcountll(w));
    return related / (0.5 * static_cast<double>(events) * static_cast<double>(events - 1));
  }
  std::size_t longest() const { return depth.empty() ? 0 : *std::max_element(depth.begin(), depth.end()); }
};

inline Order order(const Foam& F, std::size_t most_events = 12000) {
  const std::size_t n = F.events;
  if (n > most_events) throw std::invalid_argument("markets::foam::order: too many events to hold their order whole: take a part of the history");
  Order O;
  O.events = n;
  const std::size_t words = (n + 63) / 64;
  O.before.assign(n, std::vector<std::uint64_t>(words, 0));
  O.depth.assign(n, 0);
  // Events are in time, so each event's predecessors are complete when it is reached.
  std::vector<std::vector<std::size_t>> previous(n);
  for (const auto& w : F.worldline)
    for (std::size_t k = 0; k + 1 < w.size(); k++) previous[w[k + 1]].push_back(w[k]);
  for (std::size_t v = 0; v < n; v++) {
    auto& p = previous[v];
    std::sort(p.begin(), p.end());
    p.erase(std::unique(p.begin(), p.end()), p.end());
    for (std::size_t u : p) {
      for (std::size_t k = 0; k < words; k++) O.before[v][k] |= O.before[u][k];
      O.before[v][u >> 6] |= std::uint64_t{1} << (u & 63);
      O.depth[v] = std::max(O.depth[v], O.depth[u] + 1);
    }
  }
  return O;
}

// The number of steps of direct precedence on a shortest chain from event u to each event it precedes (none where it does not).
inline std::vector<std::size_t> steps_from(const Foam& F, std::size_t u) {
  std::vector<std::size_t> steps(F.events, none);
  std::vector<std::vector<std::size_t>> next(F.events);
  for (const auto& w : F.worldline)
    for (std::size_t k = 0; k + 1 < w.size(); k++) next[w[k]].push_back(w[k + 1]);
  std::vector<std::size_t> frontier{u};
  steps[u] = 0;
  while (!frontier.empty()) {
    std::vector<std::size_t> reached;
    for (std::size_t v : frontier)
      for (std::size_t w : next[v])
        if (steps[w] == none) {
          steps[w] = steps[v] + 1;
          reached.push_back(w);
        }
    frontier = std::move(reached);
  }
  return steps;
}

// For each event and each kind: the labels after the event less the labels before it, over the facts it touches.
inline std::vector<std::vector<double>> imbalance(const History& H, std::size_t kinds) {
  std::vector<double> x(H.facts.size());
  for (std::size_t r = 0; r < x.size(); r++) x[r] = H.facts[r].standing ? H.facts[r].value : 0.0;
  std::vector<std::vector<double>> out(H.events.size(), std::vector<double>(kinds, 0.0));
  for (std::size_t v = 0; v < H.events.size(); v++)
    for (const auto& u : H.events[v].touches) {
      const double next = u.how == Touch::remove ? 0.0 : u.value;
      const std::size_t k = H.facts[u.fact].kind;
      if (k != none) {
        if (k >= kinds) throw std::invalid_argument("markets::foam::imbalance: a fact's kind is one of the kinds");
        out[v][k] += next - x[u.fact];
      }
      x[u.fact] = next;
    }
  return out;
}

// The labels of a kind on the faces that meet a junction's worldline at a time.
inline double at_junction(const History& H, const Foam& F, std::size_t junction, std::size_t kind, double t) {
  const auto x = slice(F, t);
  double total = 0;
  for (std::size_t r = 0; r < H.facts.size(); r++)
    if (H.facts[r].kind == kind && !std::isnan(x[r]) && (F.end_a[r] == junction || F.end_b[r] == junction)) total += x[r];
  return total;
}

// Σ_f h_f j_f |f|: the exponent of the path weight when each face generates events at the hazard h_f times its label.
inline double area_weight(const History& H, const Foam& F, const std::vector<double>& hazard_of_fact) {
  if (hazard_of_fact.size() != H.facts.size()) throw std::invalid_argument("markets::foam::area_weight: a hazard for each fact");
  double total = 0;
  for (const auto& f : F.faces) total += hazard_of_fact[f.fact] * f.area();
  return total;
}

// The same exponent by time: the integral over the history of the total hazard Σ_r h_r x_r(t) of the facts that stand.
inline double hazard_integral(const History& H, const std::vector<double>& hazard_of_fact) {
  std::vector<double> x(H.facts.size());
  double rate = 0, total = 0, t = H.begin;
  for (std::size_t r = 0; r < x.size(); r++) {
    x[r] = H.facts[r].standing ? H.facts[r].value : 0.0;
    rate += hazard_of_fact.at(r) * x[r];
  }
  for (const auto& e : H.events) {
    total += rate * (e.time - t);
    t = e.time;
    for (const auto& u : e.touches) {
      const double next = u.how == Touch::remove ? 0.0 : u.value;
      rate += hazard_of_fact[u.fact] * (next - x[u.fact]);
      x[u.fact] = next;
    }
  }
  return total + rate * (H.end - t);
}

}  // namespace markets::foam
