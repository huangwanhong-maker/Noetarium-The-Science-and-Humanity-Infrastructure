// The grammar of a city's event spacetime (ADR 0168): the event complex read as a foam whose words are stars, learned and read on
// syntaxis's `foams`.
//
// The foam. The event complex K (complex.hpp) is a foam: its vertices the events, labelled by their types (a regime's end apart from its
// start, "regime ended"; with detail, by type and kind), the moments at the window's ends ∂₀ and ∂₁ marked; its segments labelled by
// their couplings' types (building, street, junction); its lenses by their primitives' types (party wall, frontage, cross-section). A
// coupling may carry a state into each event it attends, from the record before that event and never from the event itself: a
// building's use as its last census saw it; a space under a regime begun and not yet ended. The second is an attribute the
// productions set themselves (a regime's start sets it on every space it touches, its end clears it), so a grammar read with it is a
// grammar of attributed symbols (Knuth 1968, "Semantics of context-free languages", Math. Systems Theory 2, 127–145).
//
// What it says. Each event is a production: it consumes the couplings it touches that stood and the interfaces it meets, and leaves
// those that go on, those it begins and the interfaces it opens; the city's complex is the gluing of its events' stars in time. The
// reading gives:
//   - the rules, each with its count, its κ (the Euler characteristic is the sum of the κ of the stars), and how a new building is
//     attached to the city (the rules of births);
//   - the entropy of the rules and what is left open once what an event consumes is known, H(R | M), and what the surroundings and
//     the states add, H(R | M) − H(R | C);
//   - the prequential judgement: each event predicted by the grammar of every event before it, its escape and concentrations chosen
//     again at the start of each year, at the context-free level (a rule given what it consumes) and the context-sensitive one
//     (given also the interfaces open around it and the couplings' states); by year, with the rules that appear for the first time,
//     and in two eras about a date, so that whether the city's events depend on their surroundings is a test on what the grammar had
//     not yet seen, not a fit, and a source of events that begins later is charged once and then learned;
//   - the lives of the couplings of each kind, the rules along each worldline compressed by Re-Pair with the observations left out
//     of the words;
//   - the rates of the rules per year of the couplings they consume, and the Foata steps of the record's trace;
//   - the patterns of its changes: its process net, the observations left out, compressed by digrams, so that patterns of events that
//     bind several couplings are found from the complex itself (patterns);
//   - histories grown from its grammar at the sites the record shows, each measured as the record is (grow, measure).
//
// The record without its observations is the same complex with each building's segments unsubdivided at its censuses: each
// census touches one building and meets no interface, so its κ is 0 (syntaxis's Proposition 7.1) and χ is the same, which the reading
// reports. Nothing here names a city; the grammar describes the record and claims no cause.

#pragma once

#include <syntaxis/foams/foams.hpp>
#include <syntaxis/foams/grammar.hpp>
#include <syntaxis/foams/grow.hpp>
#include <syntaxis/foams/lives.hpp>
#include <syntaxis/foams/nets.hpp>

#include <calx/random/stream.hpp>

#include <urban/events/complex.hpp>
#include <urban/events/memory.hpp>
#include <urban/events/record.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <map>
#include <set>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace urban::events {

namespace sf = syntaxis::foams;

struct GrammarOptions {
  bool detailed = false;  // vertices by their events' types and kinds, not by types alone
};

// The kind of a vertex: ∂₀ "start", ∂₁ "end"; an event's type, a regime's end apart; with detail, its type and kind.
inline std::string vertex_kind(const Record& R, const EventComplex& X, std::size_t v, const GrammarOptions& o = {}) {
  if (X.event_of[v] == -1) return "start";
  if (X.event_of[v] == -2) return "end";
  const auto& e = R.events[static_cast<std::size_t>(X.event_of[v])];
  if (o.detailed) return e.type + (e.kind.empty() ? "" : "/" + e.kind);
  if (e.type == "regime" && (e.kind == "ended" || e.kind == "reopened")) return "regime ended";
  return e.type;
}

// The foam of an event complex, with the couplings' states where given (states[c] maps vertices to states).
inline sf::Foam foam(const Record& R, const EventComplex& X, const GrammarOptions& o = {}, const std::vector<std::map<std::size_t, std::string>>& states = {}) {
  const std::size_t V = X.time.size();
  std::vector<std::string> kind(V), participant(R.couplings.size()), pair(R.primitives.size());
  std::vector<char> boundary(V, 0);
  for (std::size_t v = 0; v < V; v++) kind[v] = vertex_kind(R, X, v, o), boundary[v] = X.event_of[v] < 0;
  for (std::size_t c = 0; c < R.couplings.size(); c++) participant[c] = R.couplings[c].type;
  for (std::size_t p = 0; p < R.primitives.size(); p++) pair[p] = R.primitives[p].type;
  auto F = sf::make(X.encounters, kind, X.time, boundary, participant, pair);
  F.states = states;
  return F;
}

// The record without its observations, each of which only cuts the worldlines it touches.
inline Record without_observations(const Record& R) {
  Record out = R;
  out.events.erase(std::remove_if(out.events.begin(), out.events.end(), [](const Event& e) { return e.type == "observation"; }), out.events.end());
  return out;
}

struct GrammarReading {
  sf::Foam foam;
  std::vector<sf::Star> stars;
  std::vector<std::size_t> events;  // the vertices that are events (not ∂₀, ∂₁)
  sf::Replay replay;
  sf::Steps steps;
  sf::Entropies entropies;          // over every event
  sf::Grammar grammar;              // learned from every event
  sf::Prequential prequential;      // each event predicted from those before it, refitted at the start of each year
  struct Year {
    int year = 0;
    std::size_t events = 0, new_rules = 0;  // events, and rules seen for the first time
    std::array<double, 3> bits{};           // their prequential bits, summed, at each level
  };
  std::vector<Year> years;
  double split = 0;                 // MJD: the two eras about it
  std::array<std::array<double, 3>, 2> era_bits{};  // prequential bits per event before and after the split, at each level
  std::array<std::size_t, 2> era_events{};
  std::size_t novel = 0;            // rules that first appear at or after the split
  std::map<std::string, sf::Lives> lives;  // by kind of coupling, the observations left out of the words
  std::vector<sf::Rate> rates;      // per year of the couplings consumed, or of the record's span
};

// The civil year of an MJD (UTC).
inline int year_of(double t) {
  int y = 1858;
  while (mjd(y + 1) <= t) y++;
  return y;
}

// The couplings' states carried into each event they attend, from the record before it and never from the event itself: a building's
// use family as its last census saw it (`use_at`: coupling ↦ census year ↦ use), and a space under a regime begun and not yet ended
// (a regime of kind "ended" or "reopened" ends one). A state is given at every event a coupling attends, empty for none, so that the
// state at an event is the one carried into it (syntaxis's Foam::states). `held` carries how many regimes are in force, which a
// growth needs: an end leaves a space under a regime when another is open, so whether one is in force is no function of what it
// was, the rule and the place, and the number in force is.
struct CarriedStates {
  std::vector<std::map<std::size_t, std::string>> uses, regimes, both, held;
};

inline CarriedStates carried_states(const Record& R, const EventComplex& X, const std::map<std::size_t, std::map<int, std::string>>& use_at = {}) {
  const std::size_t NC = R.couplings.size();
  CarriedStates S;
  S.uses.resize(NC), S.regimes.resize(NC), S.both.resize(NC), S.held.resize(NC);
  for (std::size_t c = 0; c < NC; c++) {
    const auto seen = use_at.find(c);
    std::string use;
    int open = 0;
    for (auto v : X.K.worldline[c]) {
      if (X.event_of[v] < 0) continue;
      const auto& e = R.events[static_cast<std::size_t>(X.event_of[v])];
      const std::string under = open > 0 ? "under a regime" : "";
      S.uses[c][v] = use, S.regimes[c][v] = under, S.held[c][v] = open > 0 ? "regimes in force: " + std::to_string(open) : "";
      S.both[c][v] = use.empty() ? under : under.empty() ? use : use + "; " + under;
      if (e.type == "observation" && seen != use_at.end()) {
        if (const auto at = seen->second.find(year_of(e.when.from)); at != seen->second.end()) use = family_of(at->second);
      } else if (e.type == "regime") {
        open = (e.kind == "ended" || e.kind == "reopened") ? std::max(0, open - 1) : open + 1;
      }
    }
  }
  return S;
}

inline GrammarReading read(const Record& R, const EventComplex& X, double split_mjd, const GrammarOptions& o = {},
                           const std::vector<std::map<std::size_t, std::string>>& states = {}) {
  GrammarReading G;
  G.foam = foam(R, X, o, states);
  G.stars = sf::stars(G.foam);
  for (std::size_t v = 0; v < G.stars.size(); v++)
    if (!G.stars[v].boundary) G.events.push_back(v);
  G.replay = sf::replay(G.foam, G.stars);
  G.steps = sf::steps(G.foam);
  G.entropies = sf::entropies(G.stars, G.events);
  G.grammar = sf::learn(G.stars, G.events);
  G.split = split_mjd;
  // Prequentially, refitted at the start of each year of the window.
  std::vector<double> refit;
  for (int y = year_of(R.window.from) + 1; mjd(y) <= R.window.to; y++) refit.push_back(mjd(y));
  G.prequential = sf::prequential(G.foam, G.stars, G.events, refit);
  std::set<std::string> seen;
  std::map<int, GrammarReading::Year> by_year;
  for (std::size_t k = 0; k < G.events.size(); k++) {
    const auto v = G.events[k];
    const double t = G.foam.time[v];
    auto& Y = by_year[year_of(t)];
    Y.year = year_of(t), Y.events++;
    const bool fresh = seen.insert(G.stars[v].rule).second;
    Y.new_rules += fresh;
    const std::size_t era = t < split_mjd ? 0 : 1;
    G.era_events[era]++;
    if (era == 1 && fresh) G.novel++;
    for (std::size_t lvl = 0; lvl < 3; lvl++) Y.bits[lvl] += G.prequential.bits[k][lvl], G.era_bits[era][lvl] += G.prequential.bits[k][lvl];
  }
  for (const auto& [y, Y] : by_year) G.years.push_back(Y);
  for (std::size_t era = 0; era < 2; era++)
    if (G.era_events[era])
      for (auto& b : G.era_bits[era]) b /= static_cast<double>(G.era_events[era]);
  std::set<std::string> kinds(G.foam.participant.begin(), G.foam.participant.end());
  for (const auto& k : kinds) G.lives[k] = sf::lives(G.foam, G.stars, {"observation"}, k);
  auto X2 = sf::exposure(G.foam);
  for (auto& [k, t] : X2.by_kind) t /= 365.25;  // days to years
  X2.span /= 365.25;
  G.rates = sf::rates(G.grammar, X2);
  return G;
}

// Grammars of one record compared, each on its prequential bits per event in the two eras and on the counts it reads at the end
// (lower is better on all three): the context-free grammar, the context-sensitive one by the surroundings, and by the surroundings and
// each set of the couplings' states given.
struct Candidate {
  std::string name;
  double before = 0, after = 0;  // prequential bits per event before and after the split
  std::size_t parameters = 0;    // the pairs of a context and a rule it has counted
};

inline std::vector<Candidate> candidates(const GrammarReading& plain, const std::vector<std::pair<std::string, const GrammarReading*>>& with_states) {
  std::vector<Candidate> C = {{"context-free", plain.era_bits[0][1], plain.era_bits[1][1], plain.grammar.match_rule.size()},
                              {"context: surroundings", plain.era_bits[0][2], plain.era_bits[1][2], plain.grammar.context_rule.size()}};
  for (const auto& [name, G] : with_states)
    C.push_back({"context: surroundings and " + name, G->era_bits[0][2], G->era_bits[1][2], G->grammar.context_rule.size()});
  return C;
}

inline sf::Dominance order(const std::vector<Candidate>& C) {
  std::vector<std::vector<double>> m;
  for (const auto& c : C) m.push_back({c.before, c.after, static_cast<double>(c.parameters)});
  return sf::dominance(m);
}

// The patterns of a record's changes: its process net, the observations left out, compressed by digrams (syntaxis's nets).
inline sf::Net patterns(const GrammarReading& G) { return sf::compressed(G.foam, G.stars, {"observation"}); }

// What a history shows, for comparing histories grown from the grammar with the record: its events, by kind and by year; χ; the depth
// of its trace; its prequential code per event at the context-sensitive level, its own grammar refitted at the start of each year;
// how far the lives of its street spaces compress (their events over the symbols of their straight-line grammar); how far its net
// compresses (its description before over after).
struct HistoryMeasures {
  std::size_t events = 0;
  std::map<std::string, std::size_t> kinds;
  std::map<int, std::size_t> by_year;
  long euler = 0;
  std::size_t depth = 0;
  double bits = 0, lives = 1, net = 1;
};

inline HistoryMeasures measure(const sf::Foam& F) {
  HistoryMeasures M;
  const auto S = sf::stars(F);
  std::vector<std::size_t> events;
  for (std::size_t v = 0; v < S.size(); v++)
    if (!S[v].boundary) events.push_back(v), M.kinds[F.vertex[v]]++, M.by_year[year_of(F.time[v])]++;
  M.events = events.size();
  M.euler = sf::replay(F, S).euler;
  M.depth = sf::steps(F).depth;
  std::vector<double> refit;
  for (int y = year_of(F.time.front()) + 1; mjd(y) <= F.time.back(); y++) refit.push_back(mjd(y));
  if (!events.empty()) M.bits = sf::prequential(F, S, events, refit).total[2] / static_cast<double>(events.size());
  const auto L = sf::lives(F, S, {}, "street");
  if (L.grammar.size()) M.lives = static_cast<double>(L.grammar.input_length) / static_cast<double>(L.grammar.size());
  const auto N = sf::compressed(F, S);
  if (N.size_after()) M.net = static_cast<double>(N.size_before()) / static_cast<double>(N.size_after());
  return M;
}

// Histories grown from a record's grammar (syntaxis's grow): the record without its observations, read with the number of regimes in
// force as the couplings' states (what tells a regime's end from its start, and an end from the end of the last one open); its
// sites, rates and grammar learned from it, the rates in
// the eras the `breaks` cut; grown `n` times from the window's start, each from its own stream; each history and the record
// measured alike.
struct Growth {
  HistoryMeasures record;
  std::vector<HistoryMeasures> grown;
  std::size_t sites = 0, generic = 0;
  std::size_t transitions = 0, unsure = 0;  // the table of what followed: its transitions, and those not its commonest
};

inline Growth grow(const Record& R, std::size_t n, const std::vector<double>& breaks, std::uint64_t seed, const GrammarOptions& o = {}) {
  Growth out;
  const auto Rc = without_observations(R);
  const auto Xc = construct(Rc);
  const auto CS = carried_states(Rc, Xc);
  const auto F = foam(Rc, Xc, o, CS.held);
  const auto S = sf::stars(F);
  std::vector<std::size_t> events;
  for (std::size_t v = 0; v < S.size(); v++)
    if (!S[v].boundary) events.push_back(v);
  if (events.empty()) return out;
  const auto P = sf::learn_growth(F, S, {}, breaks);
  out.sites = P.sites.size();
  for (const auto& g : P.sites) out.generic += g.generic;
  std::tie(out.unsure, out.transitions) = P.unsure();
  out.record = measure(F);
  for (std::size_t h = 0; h < n; h++) {
    calx::random::Stream rs(seed, static_cast<std::uint32_t>(h), 7, 11);
    out.grown.push_back(measure(sf::grow(F, S, P, rs)));
  }
  return out;
}

}  // namespace urban::events
