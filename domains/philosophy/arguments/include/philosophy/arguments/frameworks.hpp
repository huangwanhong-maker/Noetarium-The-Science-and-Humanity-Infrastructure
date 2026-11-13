// Abstract argumentation frameworks and their semantics (Dung 1995, On the acceptability of arguments and its fundamental role in
// nonmonotonic reasoning, logic programming and n-person games, Artificial Intelligence 77, 321–357). The domain's theory note,
// theory/arguments.md, §1–§6, states and proves what this header relies on.
//
// A framework is a finite set of arguments A = {0, …, n − 1} and an attack relation R ⊆ A × A: a directed graph, held as morphe's
// Multigraph, an edge (a, b) reading "a attacks b" (a repeated edge counts once; a loop is an argument that attacks itself). What
// an argument says is no part of it. For S ⊆ A write S⁺ = {b : some a ∈ S attacks b}. After Dung (1995):
// - S is conflict-free when no member of S attacks a member of S;
// - S defends a (a is acceptable with respect to S) when every attacker of a is attacked by a member of S;
// - the characteristic function F(S) = {a : S defends a}, monotone for inclusion;
// - S is admissible when conflict-free and S ⊆ F(S); complete when conflict-free and S = F(S); preferred when admissible and
//   maximal for inclusion among the admissible sets; stable when conflict-free and S ∪ S⁺ = A;
// - the grounded extension is the least fixed point of F, here ⋃ₖ Fᵏ(∅), reached after at most n rounds (the theory note, §3).
// Every stable extension is preferred, every preferred extension complete, and the grounded extension is the least complete one
// (Dung 1995; the theory note, §4).
//
// The labelling of an extension E (Caminada 2006, On the issue of reinstatement in argumentation, JELIA 2006, LNCS 4160) marks E
// in, E⁺ out and the rest undecided; a labelling is complete when an argument is in exactly when all its attackers are out, and
// out exactly when some attacker is in. Complete labellings and complete extensions correspond one to one.
//
// Supports. A bipolar framework adds a support relation (Cayrol and Lagasquie-Schiex 2005, On the acceptability of arguments in
// bipolar argumentation frameworks, ECSQARU 2005, LNCS 3571). This header keeps one use of it, as a stated choice: the supported
// attack. If a supports b, through one support or a chain of them, and b attacks c, then a attacks c. The derived attacks are
// added to the direct ones and the result is an ordinary framework, read by Dung's semantics. No other derived attack is taken:
// in particular an attack on b is not an attack on what b supports, since an argument for a claim may fail and the claim stand.
//
// Limits. Membership tests and the characteristic function cost O(n + m) for m attacks; the grounded extension O(n + m). The
// complete, preferred and stable extensions are found by enumeration: every complete extension is the grounded one joined to a
// complete extension of the framework restricted to the u arguments the grounded semantics leaves undecided (the theory note,
// Proposition 5.1), so 2ᵘ sets are tried, each in O(u) word operations. The enumeration is refused, with the reason, when u exceeds
// a stated limit (enumeration_limit, 20, by default; never above enumeration_ceiling, 30). Deciding credulous acceptance under the
// preferred semantics is NP-complete (Dimopoulos and Torres 1996, Graph theoretical structures in logic programs and default
// theories, Theoretical Computer Science 170) and sceptical acceptance Π₂ᵖ-complete (Dunne and Bench-Capon 2002, Coherence in
// finite argument systems, Artificial Intelligence 141), so no method much better than enumeration is expected in the worst case.
//
// These semantics are general mathematics (fixed points of a monotone function on the subsets of a graph) and name nothing of
// philosophy; they wait here to move to morphe's planned `logic` when a second domain needs them (the domain's README, §2).
//
// Checks: tests/check_arguments.cpp, against frameworks worked by hand and against brute force on every framework of at most four
// arguments.

#pragma once

#include <morphe/combinatorics/graph.hpp>

#include <algorithm>
#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <queue>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace philosophy::arguments {

// A set of arguments: ascending, without repeats.
using Set = std::vector<std::size_t>;

// A finite argumentation framework (Dung 1995): the arguments are the vertices 0 … size() − 1 of `attack`, and an edge
// (a, b) says that a attacks b.
struct Framework {
  morphe::combinatorics::Multigraph attack;

  Framework() = default;
  explicit Framework(std::size_t arguments, std::vector<std::array<std::size_t, 2>> attacks = {}) {
    attack.vertices = arguments;
    attack.edges = std::move(attacks);
  }
  std::size_t size() const { return attack.vertices; }
  // Adds the attack of `attacker` on `attacked`.
  void add(std::size_t attacker, std::size_t attacked) {
    if (attacker >= size() || attacked >= size()) throw std::out_of_range("philosophy::arguments::Framework: an attack names an argument that is not there");
    attack.edges.push_back({attacker, attacked});
  }
};

namespace detail {

// For each argument, those that attack it and those it attacks, read from morphe's incidence (which refuses an edge that leaves
// the vertices). A repeated attack appears as often as it is stated.
struct Neighbours {
  std::vector<std::vector<std::size_t>> attackers, attacked;
};

inline Neighbours neighbours(const morphe::combinatorics::Multigraph& g) {
  const auto incidence = g.incidence();
  Neighbours nb{std::vector<std::vector<std::size_t>>(g.vertices), std::vector<std::vector<std::size_t>>(g.vertices)};
  for (std::size_t v = 0; v < g.vertices; v++)
    for (const auto& [e, end] : incidence[v]) {
      if (end > 0) nb.attacked[v].push_back(g.edges[e][1]);
      else nb.attackers[v].push_back(g.edges[e][0]);
    }
  return nb;
}

inline std::vector<char> members(std::size_t n, const Set& s) {
  std::vector<char> in(n, 0);
  for (const std::size_t a : s) {
    if (a >= n) throw std::out_of_range("philosophy::arguments: a set names an argument that is not in the framework");
    in[a] = 1;
  }
  return in;
}

inline Set as_set(const std::vector<char>& in) {
  Set s;
  for (std::size_t a = 0; a < in.size(); a++)
    if (in[a]) s.push_back(a);
  return s;
}

// S⁺ as a membership vector.
inline std::vector<char> plus(const Neighbours& nb, const std::vector<char>& in) {
  std::vector<char> out(in.size(), 0);
  for (std::size_t a = 0; a < in.size(); a++)
    if (in[a])
      for (const std::size_t b : nb.attacked[a]) out[b] = 1;
  return out;
}

inline bool conflict_free(const Neighbours& nb, const std::vector<char>& in) {
  for (std::size_t a = 0; a < in.size(); a++)
    if (in[a])
      for (const std::size_t b : nb.attacked[a])
        if (in[b]) return false;
  return true;
}

// F(S), given S⁺: the arguments all of whose attackers are in S⁺.
inline std::vector<char> defended(const Neighbours& nb, const std::vector<char>& plus_of_s) {
  std::vector<char> f(plus_of_s.size(), 1);
  for (std::size_t a = 0; a < f.size(); a++)
    for (const std::size_t b : nb.attackers[a])
      if (!plus_of_s[b]) { f[a] = 0; break; }
  return f;
}

}  // namespace detail

// S⁺: the arguments attacked by a member of S.
inline Set attacked_by(const Framework& F, const Set& S) {
  const auto nb = detail::neighbours(F.attack);
  return detail::as_set(detail::plus(nb, detail::members(F.size(), S)));
}

// Whether no member of S attacks a member of S (Dung 1995).
inline bool conflict_free(const Framework& F, const Set& S) {
  return detail::conflict_free(detail::neighbours(F.attack), detail::members(F.size(), S));
}

// Whether S defends a: every attacker of a is attacked by a member of S (a is acceptable with respect to S; Dung 1995).
inline bool defends(const Framework& F, const Set& S, std::size_t a) {
  if (a >= F.size()) throw std::out_of_range("philosophy::arguments::defends: the argument is not in the framework");
  const auto nb = detail::neighbours(F.attack);
  const auto plus = detail::plus(nb, detail::members(F.size(), S));
  for (const std::size_t b : nb.attackers[a])
    if (!plus[b]) return false;
  return true;
}

// The characteristic function F(S) = {a : S defends a} (Dung 1995). Monotone: S ⊆ T gives F(S) ⊆ F(T) (the theory note,
// Proposition 2.2).
inline Set characteristic(const Framework& F, const Set& S) {
  const auto nb = detail::neighbours(F.attack);
  return detail::as_set(detail::defended(nb, detail::plus(nb, detail::members(F.size(), S))));
}

// Whether S is conflict-free and defends each of its members (Dung 1995).
inline bool admissible(const Framework& F, const Set& S) {
  const auto nb = detail::neighbours(F.attack);
  const auto in = detail::members(F.size(), S);
  if (!detail::conflict_free(nb, in)) return false;
  const auto f = detail::defended(nb, detail::plus(nb, in));
  for (std::size_t a = 0; a < in.size(); a++)
    if (in[a] && !f[a]) return false;
  return true;
}

// Whether S is a complete extension: conflict-free, and holding exactly the arguments it defends (Dung 1995).
inline bool complete(const Framework& F, const Set& S) {
  const auto nb = detail::neighbours(F.attack);
  const auto in = detail::members(F.size(), S);
  return detail::conflict_free(nb, in) && detail::defended(nb, detail::plus(nb, in)) == in;
}

// Whether S is a stable extension: conflict-free, and attacking every argument outside it (Dung 1995).
inline bool stable(const Framework& F, const Set& S) {
  const auto nb = detail::neighbours(F.attack);
  const auto in = detail::members(F.size(), S);
  if (!detail::conflict_free(nb, in)) return false;
  const auto plus = detail::plus(nb, in);
  for (std::size_t a = 0; a < in.size(); a++)
    if (!in[a] && !plus[a]) return false;
  return true;
}

// ---- The grounded semantics ---------------------------------------------------------------------------------------------------

// The label of an argument (Caminada 2006): in (accepted), out (attacked by an argument that is in), or undecided.
enum class Label { in, out, undecided };

// The grounded extension G = ⋃ₖ Fᵏ(∅) with its labelling and its history. round[a] is, for an argument that is in, the least
// k ≥ 1 with a ∈ Fᵏ(∅); for one that is out, the least k such that an argument of Fᵏ(∅) attacks it; 0 for an undecided one.
// rounds is the least k with Fᵏ(∅) = Fᵏ⁺¹(∅), at most the number of arguments.
struct Grounded {
  Set extension;
  std::vector<Label> label;
  std::vector<std::size_t> round;
  std::size_t rounds = 0;
};

// The grounded extension, by iterating the characteristic function from the empty set (Dung 1995: its least fixed point; for a
// finite framework the iteration reaches it, the theory note, Theorem 3.2). O(n + m): each attack is looked at a bounded number
// of times.
inline Grounded grounded(const Framework& F) {
  const std::size_t n = F.size();
  const auto nb = detail::neighbours(F.attack);
  Grounded g;
  g.label.assign(n, Label::undecided);
  g.round.assign(n, 0);
  std::vector<std::size_t> pending(n);  // the attacks on each argument from arguments not yet out
  std::vector<std::size_t> current;
  for (std::size_t a = 0; a < n; a++) {
    pending[a] = nb.attackers[a].size();
    if (pending[a] == 0) current.push_back(a);
  }
  for (std::size_t k = 1; !current.empty(); k++) {
    g.rounds = k;
    std::vector<std::size_t> next;
    for (const std::size_t a : current) g.label[a] = Label::in, g.round[a] = k;
    for (const std::size_t a : current)
      for (const std::size_t b : nb.attacked[a]) {
        if (g.label[b] != Label::undecided) continue;  // out already (an argument that is in is attacked by none that is in)
        g.label[b] = Label::out;
        g.round[b] = k;
        for (const std::size_t c : nb.attacked[b])
          if (--pending[c] == 0 && g.label[c] == Label::undecided) next.push_back(c);
      }
    current = std::move(next);
  }
  for (std::size_t a = 0; a < n; a++)
    if (g.label[a] == Label::in) g.extension.push_back(a);
  return g;
}

// The labelling of a set E: E in, E⁺ out, the rest undecided (Caminada 2006). It is a complete labelling exactly when E is a
// complete extension.
inline std::vector<Label> labelling(const Framework& F, const Set& E) {
  const auto nb = detail::neighbours(F.attack);
  const auto in = detail::members(F.size(), E);
  const auto plus = detail::plus(nb, in);
  std::vector<Label> label(F.size(), Label::undecided);
  for (std::size_t a = 0; a < F.size(); a++) label[a] = in[a] ? Label::in : plus[a] ? Label::out : Label::undecided;
  return label;
}

// Whether a labelling is complete (Caminada 2006): an argument is in exactly when every attacker is out, and out exactly when
// some attacker is in.
inline bool complete_labelling(const Framework& F, const std::vector<Label>& label) {
  if (label.size() != F.size()) throw std::invalid_argument("philosophy::arguments::complete_labelling: a label for each argument");
  const auto nb = detail::neighbours(F.attack);
  for (std::size_t a = 0; a < F.size(); a++) {
    bool all_out = true, some_in = false;
    for (const std::size_t b : nb.attackers[a]) {
      all_out = all_out && label[b] == Label::out;
      some_in = some_in || label[b] == Label::in;
    }
    if ((label[a] == Label::in) != all_out || (label[a] == Label::out) != some_in) return false;
  }
  return true;
}

// ---- Extensions by enumeration ------------------------------------------------------------------------------------------------

// The number of arguments left undecided by the grounded semantics above which an enumeration is refused unless the caller
// states another limit, and the limit no caller may exceed (2³⁰ sets take minutes).
inline constexpr std::size_t enumeration_limit = 20;
inline constexpr std::size_t enumeration_ceiling = 30;

// The extensions of a semantics, in lexicographic order, or a refusal with its reason.
struct Extensions {
  bool refused = false;
  std::string reason;
  std::vector<Set> sets;
};

namespace detail {

// The complete extensions as the grounded extension joined to sets of undecided arguments (bit i of a mask is undecided[i]).
struct Enumeration {
  bool refused = false;
  std::string reason;
  Set grounded;
  std::vector<std::size_t> undecided;
  std::vector<std::uint64_t> attacks;   // per undecided argument, the undecided arguments it attacks
  std::vector<std::uint64_t> complete;  // ascending
};

inline Enumeration enumerate(const Framework& F, std::size_t limit) {
  if (limit > enumeration_ceiling) throw std::invalid_argument("philosophy::arguments: an enumeration limit above " + std::to_string(enumeration_ceiling));
  Enumeration e;
  const Grounded g = philosophy::arguments::grounded(F);
  e.grounded = g.extension;
  std::vector<std::size_t> position(F.size(), SIZE_MAX);
  for (std::size_t a = 0; a < F.size(); a++)
    if (g.label[a] == Label::undecided) position[a] = e.undecided.size(), e.undecided.push_back(a);
  const std::size_t u = e.undecided.size();
  if (u > limit) {
    e.refused = true;
    e.reason = "the grounded semantics leaves " + std::to_string(u) + " arguments undecided, and the enumeration of the 2^" + std::to_string(u) +
               " sets of them is refused above " + std::to_string(limit);
    return e;
  }
  std::vector<std::uint64_t> attackers(u, 0);
  e.attacks.assign(u, 0);
  for (const auto& edge : F.attack.edges)
    if (position[edge[0]] != SIZE_MAX && position[edge[1]] != SIZE_MAX) {
      e.attacks[position[edge[0]]] |= std::uint64_t{1} << position[edge[1]];
      attackers[position[edge[1]]] |= std::uint64_t{1} << position[edge[0]];
    }
  for (std::uint64_t s = 0; s < (std::uint64_t{1} << u); s++) {
    std::uint64_t plus = 0;
    for (std::uint64_t rest = s; rest; rest &= rest - 1) plus |= e.attacks[static_cast<std::size_t>(std::countr_zero(rest))];
    if (plus & s) continue;  // not conflict-free
    std::uint64_t f = 0;
    for (std::size_t i = 0; i < u; i++)
      if ((attackers[i] & ~plus) == 0) f |= std::uint64_t{1} << i;
    if (f == s) e.complete.push_back(s);
  }
  return e;
}

inline Extensions extensions(const Enumeration& e, const std::vector<std::uint64_t>& masks) {
  Extensions x;
  x.refused = e.refused;
  x.reason = e.reason;
  if (e.refused) return x;
  for (const std::uint64_t s : masks) {
    Set set = e.grounded;
    for (std::size_t i = 0; i < e.undecided.size(); i++)
      if (s >> i & 1) set.push_back(e.undecided[i]);
    std::sort(set.begin(), set.end());
    x.sets.push_back(std::move(set));
  }
  std::sort(x.sets.begin(), x.sets.end());
  return x;
}

inline std::vector<std::uint64_t> maximal(const std::vector<std::uint64_t>& masks) {
  std::vector<std::uint64_t> top;
  for (const std::uint64_t s : masks) {
    bool below = false;
    for (const std::uint64_t t : masks)
      if (t != s && (s & ~t) == 0) { below = true; break; }
    if (!below) top.push_back(s);
  }
  return top;
}

inline std::vector<std::uint64_t> covering(const Enumeration& e) {
  const std::size_t u = e.undecided.size();
  const std::uint64_t all = u == 64 ? ~std::uint64_t{0} : (std::uint64_t{1} << u) - 1;
  std::vector<std::uint64_t> cover;
  for (const std::uint64_t s : e.complete) {
    std::uint64_t plus = 0;
    for (std::uint64_t rest = s; rest; rest &= rest - 1) plus |= e.attacks[static_cast<std::size_t>(std::countr_zero(rest))];
    if ((s | plus) == all) cover.push_back(s);
  }
  return cover;
}

}  // namespace detail

// The complete extensions (Dung 1995): never none, the grounded extension the least of them. Exponential in the number u of
// arguments the grounded semantics leaves undecided; refused when u > limit.
inline Extensions complete_extensions(const Framework& F, std::size_t limit = enumeration_limit) {
  const auto e = detail::enumerate(F, limit);
  return detail::extensions(e, e.complete);
}

// The preferred extensions (Dung 1995): the maximal admissible sets, which are the maximal complete extensions (the theory note,
// Proposition 4.3). Never none. Exponential as complete_extensions is, and refused as it is.
inline Extensions preferred_extensions(const Framework& F, std::size_t limit = enumeration_limit) {
  const auto e = detail::enumerate(F, limit);
  return detail::extensions(e, e.refused ? std::vector<std::uint64_t>{} : detail::maximal(e.complete));
}

// The stable extensions (Dung 1995): the complete extensions that attack every argument outside them (the theory note,
// Proposition 4.4). There may be none. Exponential as complete_extensions is, and refused as it is.
inline Extensions stable_extensions(const Framework& F, std::size_t limit = enumeration_limit) {
  const auto e = detail::enumerate(F, limit);
  return detail::extensions(e, e.refused ? std::vector<std::uint64_t>{} : detail::covering(e));
}

// ---- Acceptance ---------------------------------------------------------------------------------------------------------------

enum class Semantics { grounded, complete, preferred, stable };

// For each argument, whether some extension of the semantics holds it (credulous) and whether every extension does (sceptical).
// With no extension at all, as the stable semantics may have, nothing is credulously accepted and everything is sceptically
// accepted, vacuously: `extensions` says how many there were.
struct Acceptance {
  bool refused = false;
  std::string reason;
  std::size_t extensions = 0;
  std::vector<char> credulous, sceptical;
};

// Credulous and sceptical acceptance under a semantics. Under the grounded semantics both are membership of the grounded
// extension, in O(n + m); under the others the extensions are enumerated, refused as complete_extensions is. Sceptical acceptance
// under the complete semantics is membership of the grounded extension (the theory note, Corollary 4.6).
inline Acceptance acceptance(const Framework& F, Semantics semantics, std::size_t limit = enumeration_limit) {
  Acceptance acc;
  std::vector<Set> sets;
  if (semantics == Semantics::grounded) {
    sets.push_back(grounded(F).extension);
  } else {
    Extensions x = semantics == Semantics::complete ? complete_extensions(F, limit) : semantics == Semantics::preferred ? preferred_extensions(F, limit) : stable_extensions(F, limit);
    if (x.refused) {
      acc.refused = true;
      acc.reason = std::move(x.reason);
      return acc;
    }
    sets = std::move(x.sets);
  }
  acc.extensions = sets.size();
  acc.credulous.assign(F.size(), 0);
  acc.sceptical.assign(F.size(), 1);
  for (const Set& s : sets) {
    const auto in = detail::members(F.size(), s);
    for (std::size_t a = 0; a < F.size(); a++) {
      if (in[a]) acc.credulous[a] = 1;
      else acc.sceptical[a] = 0;
    }
  }
  return acc;
}

// ---- Supports, by the attacks they induce -------------------------------------------------------------------------------------

// A bipolar framework (Cayrol and Lagasquie-Schiex 2005): arguments 0 … arguments − 1, attacks (a, b) and supports (a, b),
// "a supports b".
struct Bipolar {
  std::size_t arguments = 0;
  std::vector<std::array<std::size_t, 2>> attacks, supports;
};

// An attack derived from supports: chain = (c₀, …, cₖ), k ≥ 1, with c₀ the attacker, each cᵢ supporting cᵢ₊₁, and cₖ attacking
// `attacked` directly. The chain given is a shortest one.
struct SupportedAttack {
  std::size_t attacker = 0, attacked = 0;
  std::vector<std::size_t> chain;
};

// The framework a bipolar framework induces: its direct attacks, in the order given (the first `direct` edges), then the
// supported attacks that are not already attacks, each with the chain that derives it (edge direct + i is supported[i]).
struct Derived {
  Framework framework;
  std::size_t direct = 0;
  std::vector<SupportedAttack> supported;
};

// The supported attacks of a bipolar framework (after Cayrol and Lagasquie-Schiex 2005), added to its direct attacks: a attacks
// c whenever a supports, through one or more supports, some b that attacks c. An argument that supports its own attacker so
// comes to attack itself. O(n (n + m)) for n arguments and m attacks and supports.
inline Derived supported_attacks(const Bipolar& B) {
  Derived d;
  d.framework = Framework(B.arguments, B.attacks);
  d.direct = B.attacks.size();
  morphe::combinatorics::Multigraph support;
  support.vertices = B.arguments;
  support.edges = B.supports;
  const auto attack = detail::neighbours(d.framework.attack);  // refuses what leaves the arguments
  const auto supports = detail::neighbours(support);
  std::set<std::pair<std::size_t, std::size_t>> present;
  for (const auto& e : B.attacks) present.insert({e[0], e[1]});
  for (std::size_t a = 0; a < B.arguments; a++) {
    // Breadth first along the supports from a: parent[b] the argument through which b was first reached.
    std::vector<std::size_t> parent(B.arguments, SIZE_MAX), order;
    std::queue<std::size_t> q;
    for (const std::size_t b : supports.attacked[a])
      if (parent[b] == SIZE_MAX) parent[b] = a, q.push(b);
    while (!q.empty()) {
      const std::size_t b = q.front();
      q.pop();
      order.push_back(b);
      for (const std::size_t c : supports.attacked[b])
        if (parent[c] == SIZE_MAX) parent[c] = b, q.push(c);
    }
    for (const std::size_t b : order)
      for (const std::size_t c : attack.attacked[b]) {
        if (!present.insert({a, c}).second) continue;
        SupportedAttack s{a, c, {}};
        // The chain back from b to a; a itself may lie on a cycle of supports, so stop at the first return to it.
        for (std::size_t x = b;; x = parent[x]) {
          s.chain.push_back(x);
          if (parent[x] == a) break;
        }
        s.chain.push_back(a);
        std::reverse(s.chain.begin(), s.chain.end());
        d.framework.attack.edges.push_back({a, c});
        d.supported.push_back(std::move(s));
      }
  }
  return d;
}

}  // namespace philosophy::arguments
