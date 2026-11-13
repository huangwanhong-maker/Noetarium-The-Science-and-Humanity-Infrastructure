// Checks of arguments (frameworks.hpp, maps.hpp).
//
// Frameworks worked by hand (Dung 1995; the examples are the usual ones of the literature):
// - the Nixon diamond, two arguments attacking each other: the grounded extension empty, two preferred, two stable;
// - a chain a → b → c: grounded {a, c}, c reinstated at the second round; a longer chain; a chain of 1001;
// - an odd cycle of three: no stable extension, the empty set the one preferred extension;
// - self-attackers; an even cycle; floating acceptance; a preferred extension that is not stable;
// - the refusal of an enumeration above its limit, with its reason, and what leaves the framework refused.
//
// Brute force. On every framework of at most four arguments (1 + 2 + 16 + 512 + 65,536 = 66,067 attack relations), every
// definition is evaluated on every subset by code that shares nothing with the library (bit masks, the definitions read
// literally), and compared; and the theorems the library relies on are verified there: the characteristic function is monotone,
// the Fundamental Lemma, the grounded extension is complete, lies in every complete extension and is their intersection, every
// stable extension is preferred, every preferred extension complete, complete labellings correspond to complete extensions, a
// well-founded framework has one complete extension, which is stable, and a framework without odd cycles is coherent. Then 300
// random frameworks of 12 arguments against brute force over their 4,096 subsets.
//
// Supports: the supported attack, its chains, and what is not derived.
//
// Maps: each fault found where it is; the framework a hand-made map induces; standings, deciding nodes and open questions; the
// written form, read and written, with its errors line by line; 400 random maps written and read back.

#include <philosophy/arguments/frameworks.hpp>
#include <philosophy/arguments/maps.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace pa = philosophy::arguments;
using pa::Label;
using pa::Set;
using Sets = std::vector<Set>;

namespace {

// ---- Brute force on bit masks, sharing nothing with the library ----------------------------------------------------------------

struct Brute {
  int n = 0;
  std::array<unsigned, 16> out{}, in{};  // out[a]: the arguments a attacks; in[a]: its attackers

  explicit Brute(const pa::Framework& F) : n(static_cast<int>(F.size())) {
    for (const auto& e : F.attack.edges) out[e[0]] |= 1u << e[1], in[e[1]] |= 1u << e[0];
  }
  unsigned all() const { return (1u << n) - 1; }
  bool has(unsigned S, int a) const { return S >> a & 1; }
  // No member of S attacks a member of S.
  bool conflict_free(unsigned S) const {
    for (int a = 0; a < n; a++)
      if (has(S, a) && (out[a] & S)) return false;
    return true;
  }
  // Every attacker of a is attacked by a member of S.
  bool defends(unsigned S, int a) const {
    for (int b = 0; b < n; b++)
      if (has(in[a], b) && !(in[b] & S)) return false;
    return true;
  }
  unsigned F(unsigned S) const {
    unsigned f = 0;
    for (int a = 0; a < n; a++)
      if (defends(S, a)) f |= 1u << a;
    return f;
  }
  unsigned plus(unsigned S) const {
    unsigned p = 0;
    for (int a = 0; a < n; a++)
      if (in[a] & S) p |= 1u << a;
    return p;
  }
  bool admissible(unsigned S) const { return conflict_free(S) && (S & ~F(S)) == 0; }
  bool complete(unsigned S) const { return conflict_free(S) && F(S) == S; }
  bool stable(unsigned S) const {
    if (!conflict_free(S)) return false;
    for (int a = 0; a < n; a++)
      if (!has(S, a) && !(in[a] & S)) return false;
    return true;
  }
  std::vector<unsigned> where(bool (Brute::*property)(unsigned) const) const {
    std::vector<unsigned> v;
    for (unsigned S = 0; S <= all(); S++)
      if ((this->*property)(S)) v.push_back(S);
    return v;
  }
  // The sets of a family that lie in no other: every proper subset of a member is struck out.
  std::vector<unsigned> maximal(const std::vector<unsigned>& sets) const {
    std::vector<char> below(std::size_t{1} << n, 0);
    for (const unsigned S : sets)
      for (unsigned sub = S; sub != 0;) {
        sub = (sub - 1) & S;
        below[sub] = 1;
      }
    std::vector<unsigned> top;
    for (const unsigned S : sets)
      if (!below[S]) top.push_back(S);
    return top;
  }
};

Set to_set(unsigned S) {
  Set s;
  for (std::size_t a = 0; a < 32; a++)
    if (S >> a & 1) s.push_back(a);
  return s;
}

unsigned to_mask(const Set& s) {
  unsigned S = 0;
  for (const std::size_t a : s) S |= 1u << a;
  return S;
}

std::vector<unsigned> to_masks(const pa::Extensions& x) {
  std::vector<unsigned> v;
  for (const Set& s : x.sets) v.push_back(to_mask(s));
  std::sort(v.begin(), v.end());
  return v;
}

std::string show(const Set& s) {
  std::string t = "{";
  for (std::size_t i = 0; i < s.size(); i++) t += (i ? ", " : "") + std::to_string(s[i]);
  return t + "}";
}

std::string show(const Sets& sets) {
  std::string t;
  for (std::size_t i = 0; i < sets.size(); i++) t += (i ? " " : "") + show(sets[i]);
  return sets.empty() ? "none" : t;
}

std::string show(const std::vector<std::string>& ids) {
  std::string t = "[";
  for (std::size_t i = 0; i < ids.size(); i++) t += (i ? " " : "") + ids[i];
  return t + "]";
}

template <class Exception, class Call>
bool throws(Call&& call) {
  try {
    call();
  } catch (const Exception&) {
    return true;
  } catch (...) {
    return false;
  }
  return false;
}

// ---- A map by hand: the Euthyphro (Plato, Euthyphro 10a–13c), in outline -------------------------------------------------------

pa::Map euthyphro() {
  using pa::Polarity;
  pa::Map m;
  m.questions = {
      {"q1", "Is the pious loved by the gods because it is pious, or pious because they love it?", true, ""},
      {"q2", "What is the pious, if it is not what the gods love?", false, "p1"},
  };
  m.positions = {
      {"p1", "It is loved because it is pious.", "q1"},
      {"p2", "It is pious because it is loved.", "q1"},
      {"p3", "The pious is the part of the just that cares for the gods.", "q2"},
  };
  m.arguments = {
      {"a1", "The gods love the pious for being pious, so their love cannot be what makes it pious.", {{"p2", Polarity::con}},
       {"The gods love the pious because it is pious.", "What is loved for being so is not made so by being loved."}, "The pious is not pious because the gods love it."},
      {"a2", "A standard of piety apart from the gods' love would stand above the gods.", {{"a1", Polarity::con}}, {}, ""},
      {"a3", "A love without a reason is arbitrary, and gives no one a reason to be pious.", {{"a2", Polarity::con}}, {}, ""},
      {"a4", "Care betters what it cares for, and the gods are not bettered by us.", {{"p3", Polarity::con}}, {}, ""},
  };
  return m;
}

const pa::Standing& standing_of(const std::vector<pa::Standing>& list, const std::string& id) {
  for (const auto& s : list)
    if (s.id == id) return s;
  throw std::out_of_range("no standing of " + id);
}

}  // namespace

int main() {
  calx::verification::Suite s;
  using Edges = std::vector<std::array<std::size_t, 2>>;

  // ==== Frameworks by hand ======================================================================================================

  {  // The Nixon diamond: 0 ⇄ 1.
    const pa::Framework F(2, Edges{{0, 1}, {1, 0}});
    const auto g = pa::grounded(F);
    s.require("Nixon diamond: the grounded extension is empty and both arguments are undecided",
              g.extension.empty() && g.label == std::vector<Label>{Label::undecided, Label::undecided} && g.rounds == 0 && g.round == std::vector<std::size_t>{0, 0}, show(g.extension));
    const auto c = pa::complete_extensions(F), p = pa::preferred_extensions(F), st = pa::stable_extensions(F);
    s.require("Nixon diamond: three complete extensions, two preferred, two stable", c.sets == Sets{{}, {0}, {1}} && p.sets == Sets{{0}, {1}} && st.sets == Sets{{0}, {1}} && !c.refused,
              "complete " + show(c.sets) + "; preferred " + show(p.sets) + "; stable " + show(st.sets));
    const auto acc = pa::acceptance(F, pa::Semantics::preferred), gacc = pa::acceptance(F, pa::Semantics::grounded);
    s.require("Nixon diamond: each argument is credulously accepted, neither sceptically; neither under the grounded semantics",
              acc.extensions == 2 && acc.credulous == std::vector<char>{1, 1} && acc.sceptical == std::vector<char>{0, 0} && gacc.credulous == std::vector<char>{0, 0} && gacc.sceptical == gacc.credulous);
  }
  {  // A chain a → b → c.
    const pa::Framework F(3, Edges{{0, 1}, {1, 2}});
    const auto g = pa::grounded(F);
    s.require("chain a → b → c: grounded {a, c}; a accepted and b defeated at round 1, c accepted at round 2",
              g.extension == Set{0, 2} && g.label == std::vector<Label>{Label::in, Label::out, Label::in} && g.round == std::vector<std::size_t>{1, 1, 2} && g.rounds == 2, show(g.extension));
    s.require("chain: its one complete extension is preferred and stable", pa::complete_extensions(F).sets == Sets{{0, 2}} && pa::preferred_extensions(F).sets == Sets{{0, 2}} && pa::stable_extensions(F).sets == Sets{{0, 2}});
    s.require("chain: F(∅) = {a}, F({a}) = {a, c}, {a}⁺ = {b}", pa::characteristic(F, {}) == Set{0} && pa::characteristic(F, {0}) == Set{0, 2} && pa::attacked_by(F, {0}) == Set{1});
    s.require("chain, reinstatement: c is not defended by ∅ and is defended by {a}", !pa::defends(F, {}, 2) && pa::defends(F, {0}, 2));
    s.require("chain: {a, c} is conflict-free and {a, b} is not; {c} is not admissible and {a} is; {a} is not complete and {a, c} is, and is stable",
              pa::conflict_free(F, {0, 2}) && !pa::conflict_free(F, {0, 1}) && !pa::admissible(F, {2}) && pa::admissible(F, {0}) && !pa::complete(F, {0}) && pa::complete(F, {0, 2}) && pa::stable(F, {0, 2}) && !pa::stable(F, {0}));
    s.require("chain: the grounded labelling is complete, and labelling everything undecided is not",
              pa::complete_labelling(F, g.label) && pa::labelling(F, g.extension) == g.label && !pa::complete_labelling(F, std::vector<Label>(3, Label::undecided)));
  }
  {  // A chain of five: reinstatement twice.
    const pa::Framework F(5, Edges{{0, 1}, {1, 2}, {2, 3}, {3, 4}});
    const auto g = pa::grounded(F);
    s.require("chain of five: grounded {0, 2, 4}, settled at rounds 1, 1, 2, 2, 3", g.extension == Set{0, 2, 4} && g.round == std::vector<std::size_t>{1, 1, 2, 2, 3} && g.rounds == 3, show(g.extension));
  }
  {  // An odd cycle of three.
    const pa::Framework F(3, Edges{{0, 1}, {1, 2}, {2, 0}});
    const auto g = pa::grounded(F);
    const auto c = pa::complete_extensions(F), p = pa::preferred_extensions(F), st = pa::stable_extensions(F);
    s.require("odd cycle of three: grounded empty, the empty set its one complete and one preferred extension, no stable extension",
              g.extension.empty() && c.sets == Sets{{}} && p.sets == Sets{{}} && st.sets.empty() && !st.refused, "preferred " + show(p.sets) + "; stable " + show(st.sets));
    const auto acc = pa::acceptance(F, pa::Semantics::stable);
    s.require("odd cycle of three: with no stable extension nothing is credulously accepted, and everything sceptically, vacuously",
              acc.extensions == 0 && acc.credulous == std::vector<char>{0, 0, 0} && acc.sceptical == std::vector<char>{1, 1, 1});
  }
  {  // Self-attackers.
    const pa::Framework alone(1, Edges{{0, 0}});
    const auto g = pa::grounded(alone);
    s.require("a self-attacker alone: undecided; the empty set the one complete extension; no stable extension",
              g.extension.empty() && g.label == std::vector<Label>{Label::undecided} && pa::complete_extensions(alone).sets == Sets{{}} && pa::stable_extensions(alone).sets.empty());
    const pa::Framework defeated(2, Edges{{0, 0}, {1, 0}});
    const auto g2 = pa::grounded(defeated);
    s.require("a self-attacker attacked by an unattacked argument: it is out, and {b} is stable", g2.extension == Set{1} && g2.label[0] == Label::out && pa::stable_extensions(defeated).sets == Sets{{1}});
    const pa::Framework spreading(2, Edges{{0, 0}, {0, 1}});
    const auto g3 = pa::grounded(spreading);
    s.require("what only a self-attacker attacks is undecided too", g3.extension.empty() && g3.label[1] == Label::undecided && pa::preferred_extensions(spreading).sets == Sets{{}});
  }
  {  // An even cycle of four.
    const pa::Framework F(4, Edges{{0, 1}, {1, 2}, {2, 3}, {3, 0}});
    s.require("even cycle of four: grounded empty; {0, 2} and {1, 3} preferred and stable; three complete extensions",
              pa::grounded(F).extension.empty() && pa::preferred_extensions(F).sets == Sets{{0, 2}, {1, 3}} && pa::stable_extensions(F).sets == Sets{{0, 2}, {1, 3}} && pa::complete_extensions(F).sets.size() == 3);
  }
  {  // Floating acceptance: a ⇄ b, both attack c, c attacks d.
    const pa::Framework F(4, Edges{{0, 1}, {1, 0}, {0, 2}, {1, 2}, {2, 3}});
    const auto acc = pa::acceptance(F, pa::Semantics::preferred);
    s.require("floating acceptance: d is in every preferred extension and not in the grounded one",
              pa::grounded(F).extension.empty() && pa::preferred_extensions(F).sets == Sets{{0, 3}, {1, 3}} && acc.sceptical == std::vector<char>{0, 0, 0, 1} && acc.credulous == std::vector<char>{1, 1, 0, 1});
  }
  {  // a ⇄ b, b → c, c → c.
    const pa::Framework F(3, Edges{{0, 1}, {1, 0}, {1, 2}, {2, 2}});
    s.require("a preferred extension need not be stable: {a} and {b} preferred, {b} alone stable", pa::preferred_extensions(F).sets == Sets{{0}, {1}} && pa::stable_extensions(F).sets == Sets{{1}});
  }
  {  // Repeated attacks, an argument apart, the empty framework.
    const pa::Framework F(3, Edges{{0, 1}, {0, 1}});
    s.require("an attack stated twice counts once, and an argument apart is accepted", pa::grounded(F).extension == Set{0, 2} && pa::complete_extensions(F).sets == Sets{{0, 2}});
    const pa::Framework none{};
    s.require("the framework of no arguments: the empty set is its grounded, complete, preferred and stable extension",
              pa::grounded(none).extension.empty() && pa::complete_extensions(none).sets == Sets{{}} && pa::preferred_extensions(none).sets == Sets{{}} && pa::stable_extensions(none).sets == Sets{{}});
  }
  {  // The limits of enumeration.
    pa::Framework pairs21(42), pairs8(16);
    for (std::size_t i = 0; i < 21; i++) pairs21.add(2 * i, 2 * i + 1), pairs21.add(2 * i + 1, 2 * i);
    for (std::size_t i = 0; i < 8; i++) pairs8.add(2 * i, 2 * i + 1), pairs8.add(2 * i + 1, 2 * i);
    const auto refused = pa::preferred_extensions(pairs21);
    const auto acc = pa::acceptance(pairs21, pa::Semantics::stable);
    s.require("21 Nixon diamonds leave 42 arguments undecided: the enumeration is refused, with the reason; the grounded extension is not",
              refused.refused && refused.sets.empty() && refused.reason.find("42 arguments undecided") != std::string::npos && refused.reason.find("above 20") != std::string::npos && acc.refused && pa::grounded(pairs21).extension.empty(), refused.reason);
    const auto c = pa::complete_extensions(pairs8), p = pa::preferred_extensions(pairs8), st = pa::stable_extensions(pairs8);
    s.require("8 Nixon diamonds: 3⁸ = 6561 complete extensions, 2⁸ = 256 preferred, each stable", c.sets.size() == 6561 && p.sets.size() == 256 && st.sets == p.sets,
              std::to_string(c.sets.size()) + ", " + std::to_string(p.sets.size()) + ", " + std::to_string(st.sets.size()));
    s.require("a caller's own limit: 16 undecided arguments are refused above 15, and a limit above 30 is not taken",
              pa::complete_extensions(pairs8, 15).refused && throws<std::invalid_argument>([&] { pa::complete_extensions(pairs8, 31); }));
    pa::Framework chain(1001);
    for (std::size_t i = 0; i + 1 < 1001; i++) chain.add(i, i + 1);
    const auto g = pa::grounded(chain);
    const auto only = pa::stable_extensions(chain);
    s.require("a chain of 1001 arguments leaves none undecided: 501 accepted over 501 rounds, and its one stable extension is found without enumeration",
              g.extension.size() == 501 && g.rounds == 501 && g.round[1000] == 501 && !only.refused && only.sets == Sets{g.extension});
  }
  {  // What is refused.
    const pa::Framework bad(2, Edges{{0, 5}});
    pa::Framework F(2);
    s.require("an attack that names an argument not in the framework is refused, as is a set that does",
              throws<std::out_of_range>([&] { pa::grounded(bad); }) && throws<std::out_of_range>([&] { F.add(0, 2); }) && throws<std::out_of_range>([&] { pa::conflict_free(F, {2}); }) &&
                  throws<std::out_of_range>([&] { pa::defends(F, {}, 2); }) && throws<std::invalid_argument>([&] { pa::complete_labelling(F, {Label::in}); }));
  }

  // ==== Brute force on every framework of at most four arguments ================================================================

  {
    long frameworks = 0, subsets = 0;
    long bad_predicates = 0, bad_grounded = 0, bad_complete = 0, bad_preferred = 0, bad_stable = 0, bad_inclusion = 0, bad_least = 0, bad_stable_preferred = 0, bad_preferred_complete = 0,
         bad_monotone = 0, bad_lemma = 0, bad_existence = 0, bad_labelling = 0, bad_acceptance = 0, bad_well_founded = 0, bad_coherent = 0, bad_stable_plus = 0;
    long well_founded = 0, without_odd_cycles = 0, without_stable = 0;
    for (int n = 0; n <= 4; n++) {
      for (unsigned relation = 0; relation < (1u << (n * n)); relation++) {
        pa::Framework F(static_cast<std::size_t>(n));
        for (int a = 0; a < n; a++)
          for (int b = 0; b < n; b++)
            if (relation >> (a * n + b) & 1) F.add(static_cast<std::size_t>(a), static_cast<std::size_t>(b));
        const Brute B(F);
        frameworks++;
        const unsigned all = B.all();

        std::array<unsigned, 16> image{};  // F(S) for each S
        for (unsigned S = 0; S <= all; S++) image[S] = B.F(S);

        // The definitions, subset by subset.
        for (unsigned S = 0; S <= all; S++) {
          subsets++;
          const Set set = to_set(S);
          bool same = pa::conflict_free(F, set) == B.conflict_free(S) && pa::admissible(F, set) == B.admissible(S) && pa::complete(F, set) == B.complete(S) && pa::stable(F, set) == B.stable(S) &&
                      to_mask(pa::characteristic(F, set)) == B.F(S) && to_mask(pa::attacked_by(F, set)) == B.plus(S);
          for (int a = 0; a < n; a++) same = same && pa::defends(F, set, static_cast<std::size_t>(a)) == B.defends(S, a);
          if (!same) bad_predicates++;
          // Stable exactly when S⁺ is the complement of S (Dung 1995).
          if (B.stable(S) != (B.plus(S) == (all & ~S))) bad_stable_plus++;
          // The labelling of S is complete exactly when S is.
          if (pa::complete_labelling(F, pa::labelling(F, set)) != B.complete(S)) bad_labelling++;
          // Monotone: S ⊆ T gives F(S) ⊆ F(T).
          for (unsigned T = 0; T <= all; T++)
            if ((S & ~T) == 0 && (image[S] & ~image[T]) != 0) bad_monotone++;
          // The Fundamental Lemma: S admissible, a and a' defended by S; then S ∪ {a} is admissible and defends a'.
          if (B.admissible(S))
            for (int a = 0; a < n; a++)
              if (B.defends(S, a)) {
                if (!B.admissible(S | 1u << a)) bad_lemma++;
                for (int a2 = 0; a2 < n; a2++)
                  if (B.defends(S, a2) && !B.defends(S | 1u << a, a2)) bad_lemma++;
              }
        }

        // The grounded extension as the least fixed point, round by round.
        const auto g = pa::grounded(F);
        {
          std::vector<std::size_t> round(static_cast<std::size_t>(n), 0);
          std::vector<Label> label(static_cast<std::size_t>(n), Label::undecided);
          unsigned S = 0;
          std::size_t rounds = 0;
          for (std::size_t k = 1;; k++) {
            const unsigned next = B.F(S);
            if (next == S) break;
            rounds = k;
            for (int a = 0; a < n; a++) {
              if (B.has(next, a) && !B.has(S, a)) round[static_cast<std::size_t>(a)] = k, label[static_cast<std::size_t>(a)] = Label::in;
              if (B.has(B.plus(next), a) && !B.has(B.plus(S), a)) round[static_cast<std::size_t>(a)] = k, label[static_cast<std::size_t>(a)] = Label::out;
            }
            S = next;
          }
          if (to_mask(g.extension) != S || g.round != round || g.label != label || g.rounds != rounds || !pa::complete_labelling(F, g.label)) bad_grounded++;
        }
        const unsigned G = to_mask(g.extension);

        // The extensions.
        const auto admissible = B.where(&Brute::admissible), complete = B.where(&Brute::complete), stable = B.where(&Brute::stable), preferred = B.maximal(admissible);
        if (to_masks(pa::complete_extensions(F)) != complete) bad_complete++;
        if (to_masks(pa::preferred_extensions(F)) != preferred) bad_preferred++;
        if (to_masks(pa::stable_extensions(F)) != stable) bad_stable++;
        if (stable.empty()) without_stable++;

        // The inclusions among the semantics.
        unsigned meet = all;
        for (const unsigned C : complete) {
          meet &= C;
          if ((G & ~C) != 0) bad_inclusion++;
        }
        if (!B.complete(G) || complete.empty() || meet != G) bad_least++;
        for (const unsigned E : stable)
          if (std::find(preferred.begin(), preferred.end(), E) == preferred.end()) bad_stable_preferred++;
        for (const unsigned E : preferred)
          if (!B.complete(E)) bad_preferred_complete++;
        // A preferred extension exists, and every admissible set lies in one.
        if (preferred.empty()) bad_existence++;
        for (const unsigned S : admissible) {
          bool within = false;
          for (const unsigned E : preferred) within = within || (S & ~E) == 0;
          if (!within) bad_existence++;
        }

        // Complete labellings, counted over all 3ⁿ labellings.
        {
          std::size_t count = 0;
          unsigned total = 1;
          for (int a = 0; a < n; a++) total *= 3;
          for (unsigned code = 0; code < total; code++) {
            unsigned in = 0, out = 0, c = code;
            for (int a = 0; a < n; a++, c /= 3) {
              if (c % 3 == 0) in |= 1u << a;
              if (c % 3 == 1) out |= 1u << a;
            }
            bool legal = true;
            for (int a = 0; a < n; a++) {
              const bool all_out = (B.in[a] & ~out) == 0, some_in = (B.in[a] & in) != 0;
              legal = legal && B.has(in, a) == all_out && B.has(out, a) == some_in;
            }
            if (!legal) continue;
            count++;
            if (!B.complete(in) || B.plus(in) != out) bad_labelling++;
          }
          if (count != complete.size()) bad_labelling++;
        }

        // Acceptance.
        {
          const auto c = pa::acceptance(F, pa::Semantics::complete), p = pa::acceptance(F, pa::Semantics::preferred), st = pa::acceptance(F, pa::Semantics::stable), gr = pa::acceptance(F, pa::Semantics::grounded);
          unsigned some_admissible = 0, every_preferred = all, some_stable = 0, every_stable = all;
          for (const unsigned S : admissible) some_admissible |= S;
          for (const unsigned E : preferred) every_preferred &= E;
          for (const unsigned E : stable) some_stable |= E, every_stable &= E;
          bool same = c.extensions == complete.size() && p.extensions == preferred.size() && st.extensions == stable.size() && gr.extensions == 1;
          for (int a = 0; a < n; a++) {
            const auto i = static_cast<std::size_t>(a);
            same = same && bool(c.sceptical[i]) == B.has(G, a) && bool(gr.sceptical[i]) == B.has(G, a) && bool(gr.credulous[i]) == B.has(G, a) && bool(c.credulous[i]) == B.has(some_admissible, a) &&
                   bool(p.credulous[i]) == B.has(some_admissible, a) && bool(p.sceptical[i]) == B.has(every_preferred, a) && bool(st.credulous[i]) == B.has(some_stable, a) && bool(st.sceptical[i]) == B.has(every_stable, a);
          }
          if (!same) bad_acceptance++;
        }

        // Cycles: reach[a] the arguments reached from a by one attack or more.
        std::array<unsigned, 4> reach{};
        for (int a = 0; a < n; a++) reach[static_cast<std::size_t>(a)] = B.out[static_cast<std::size_t>(a)];
        for (int k = 0; k < n; k++)
          for (int a = 0; a < n; a++)
            if (B.has(reach[static_cast<std::size_t>(a)], k)) reach[static_cast<std::size_t>(a)] |= reach[static_cast<std::size_t>(k)];
        bool cyclic = false, odd = false;
        for (int a = 0; a < n; a++) cyclic = cyclic || B.has(reach[static_cast<std::size_t>(a)], a);
        // With at most four arguments an odd cycle has length 1 or 3, and a closed walk of length 3 holds one.
        for (int a = 0; a < n; a++)
          for (int b = 0; b < n; b++)
            for (int c = 0; c < n; c++) odd = odd || (B.has(B.out[static_cast<std::size_t>(a)], b) && B.has(B.out[static_cast<std::size_t>(b)], c) && B.has(B.out[static_cast<std::size_t>(c)], a));
        for (int a = 0; a < n; a++) odd = odd || B.has(B.out[static_cast<std::size_t>(a)], a);
        if (!cyclic) {
          well_founded++;
          if (complete.size() != 1 || stable != complete || preferred != complete || complete[0] != G) bad_well_founded++;
        }
        if (!odd) {
          without_odd_cycles++;
          if (stable.empty() || stable != preferred) bad_coherent++;
        }
      }
    }
    const std::string on = "all " + std::to_string(frameworks) + " frameworks of 0 to 4 arguments";
    s.require("brute force is over 66,067 frameworks and 1,052,741 subsets", frameworks == 66067 && subsets == 1 + 2 * 2 + 16 * 4 + 512 * 8 + 65536 * 16, std::to_string(frameworks) + " frameworks, " + std::to_string(subsets) + " subsets");
    s.require("conflict-free, defends, admissible, complete, stable, F(S) and S⁺ agree with the definitions on every subset", bad_predicates == 0, on + "; " + std::to_string(bad_predicates) + " subsets differ");
    s.require("the grounded extension is the least fixed point of F, with each argument's round and label", bad_grounded == 0, on + "; " + std::to_string(bad_grounded) + " differ");
    s.require("the complete extensions are those of the definition", bad_complete == 0, on + "; " + std::to_string(bad_complete) + " differ");
    s.require("the preferred extensions are the maximal admissible sets", bad_preferred == 0, on + "; " + std::to_string(bad_preferred) + " differ");
    s.require("the stable extensions are those of the definition", bad_stable == 0, on + "; " + std::to_string(without_stable) + " have none; " + std::to_string(bad_stable) + " differ");
    s.require("the characteristic function is monotone", bad_monotone == 0, on + ", every pair S ⊆ T");
    s.require("the Fundamental Lemma: an admissible set joined to an argument it defends is admissible and defends what it defended", bad_lemma == 0, on);
    s.require("the grounded extension is contained in every complete extension", bad_inclusion == 0, on);
    s.require("the grounded extension is complete, and is the intersection of the complete extensions: the least of them", bad_least == 0, on);
    s.require("every stable extension is preferred", bad_stable_preferred == 0, on);
    s.require("every preferred extension is complete", bad_preferred_complete == 0, on);
    s.require("a preferred extension exists, and every admissible set lies in one", bad_existence == 0, on);
    s.require("a set is stable exactly when what it attacks is its complement", bad_stable_plus == 0, on);
    s.require("complete labellings and complete extensions correspond: in a complete extension, out what it attacks", bad_labelling == 0, on + ", every one of the 3ⁿ labellings");
    s.require("acceptance: sceptical under complete is the grounded extension; credulous under complete and preferred is lying in an admissible set; stable as defined", bad_acceptance == 0, on);
    s.require("a well-founded framework (no cycle of attacks) has one complete extension, which is grounded, preferred and stable", bad_well_founded == 0 && well_founded > 0,
              std::to_string(well_founded) + " well-founded frameworks among them");
    s.require("a framework without odd cycles has a stable extension, and its preferred extensions are its stable ones", bad_coherent == 0 && without_odd_cycles > 0,
              std::to_string(without_odd_cycles) + " frameworks without odd cycles among them");
  }

  {  // Random frameworks of 12 arguments against brute force over their 4,096 subsets.
    calx::random::Stream rng(20261113, 1, 0, 0);
    int differ = 0, grounded_differ = 0;
    std::size_t undecided = 0, extensions = 0;
    const int trials = 300;
    for (int t = 0; t < trials; t++) {
      const std::size_t n = 12;
      const double density = 0.04 + 0.2 * rng.uniform();
      pa::Framework F(n);
      for (std::size_t a = 0; a < n; a++)
        for (std::size_t b = 0; b < n; b++)
          if (rng.uniform() < density && (a != b || rng.uniform() < 0.3)) F.add(a, b);
      const Brute B(F);
      unsigned lfp = 0;
      while (B.F(lfp) != lfp) lfp = B.F(lfp);
      const auto g = pa::grounded(F);
      if (to_mask(g.extension) != lfp || !pa::complete_labelling(F, g.label)) grounded_differ++;
      for (const Label l : g.label) undecided += l == Label::undecided;
      const auto complete = B.where(&Brute::complete), stable = B.where(&Brute::stable), preferred = B.maximal(B.where(&Brute::admissible));
      extensions += complete.size();
      if (to_masks(pa::complete_extensions(F)) != complete || to_masks(pa::preferred_extensions(F)) != preferred || to_masks(pa::stable_extensions(F)) != stable) differ++;
    }
    s.require("300 random frameworks of 12 arguments: the grounded extension is the least fixed point", grounded_differ == 0, std::to_string(undecided) + " arguments left undecided in all");
    s.require("300 random frameworks of 12 arguments: complete, preferred and stable extensions equal brute force over 4,096 subsets", differ == 0, std::to_string(extensions) + " complete extensions in all");
  }

  // ==== Supports ================================================================================================================

  {
    const auto d1 = pa::supported_attacks({3, {{1, 2}}, {{0, 1}}});
    s.require("a supports b and b attacks c: a attacks c, by the chain (a, b)", d1.direct == 1 && d1.framework.attack.edges == Edges{{1, 2}, {0, 2}} && d1.supported.size() == 1 && d1.supported[0].attacker == 0 &&
                                                                               d1.supported[0].attacked == 2 && d1.supported[0].chain == std::vector<std::size_t>{0, 1});
    const auto d2 = pa::supported_attacks({4, {{2, 3}}, {{0, 1}, {1, 2}}});
    s.require("a chain of supports a, b, c and c attacks d: a and b attack d, each by its chain",
              d2.supported.size() == 2 && d2.supported[0].attacker == 0 && d2.supported[0].chain == std::vector<std::size_t>{0, 1, 2} && d2.supported[1].attacker == 1 && d2.supported[1].chain == std::vector<std::size_t>{1, 2} &&
                  d2.framework.attack.edges == Edges{{2, 3}, {0, 3}, {1, 3}});
    const auto d3 = pa::supported_attacks({3, {{0, 1}}, {{1, 2}}});
    s.require("an attack on b is not an attack on what b supports", d3.supported.empty() && d3.framework.attack.edges == Edges{{0, 1}});
    const auto d4 = pa::supported_attacks({3, {{1, 2}, {0, 2}}, {{0, 1}, {0, 1}}});
    s.require("an attack already stated is not derived again", d4.supported.empty() && d4.framework.attack.edges.size() == 2);
    const auto d5 = pa::supported_attacks({2, {{1, 0}}, {{0, 1}}});
    s.require("an argument that supports its attacker attacks itself", d5.supported.size() == 1 && d5.supported[0].attacker == 0 && d5.supported[0].attacked == 0 && pa::grounded(d5.framework).extension == Set{1});
    const auto d6 = pa::supported_attacks({4, {{1, 2}, {0, 3}}, {{0, 1}, {1, 0}}});
    s.require("supports in a cycle: each takes on the other's attacks, once",
              d6.supported.size() == 2 && d6.supported[0].attacker == 0 && d6.supported[0].attacked == 2 && d6.supported[0].chain == std::vector<std::size_t>{0, 1} && d6.supported[1].attacker == 1 &&
                  d6.supported[1].attacked == 3 && d6.supported[1].chain == std::vector<std::size_t>{1, 0});
    // d attacks b, b attacks c, a supports b: without the support c is reinstated; with it a, unattacked, attacks c.
    const pa::Bipolar B{4, {{3, 1}, {1, 2}}, {{0, 1}}};
    s.require("a support changes what is accepted: c, reinstated without it, is defeated by b's supporter", pa::grounded(pa::Framework(4, B.attacks)).extension == Set{0, 2, 3} && pa::grounded(pa::supported_attacks(B).framework).extension == Set{0, 3});
    s.require("a support that names an argument not there is refused", throws<std::out_of_range>([] { pa::supported_attacks({2, {}, {{0, 2}}}); }));
  }

  // ==== Maps: faults ============================================================================================================

  using pa::FaultKind;
  using pa::NodeKind;
  using pa::Polarity;
  const pa::Map E = euthyphro();
  {
    const auto f = pa::faults(E);
    s.require("the hand-made map (the Euthyphro, in outline) is well formed", f.empty() && pa::well_formed(E), f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.positions[2].answers = "";
    const auto f = pa::faults(m);
    s.require("a position that answers no question is found: the third position, p3", f.size() == 1 && f[0].kind == FaultKind::answers_nothing && f[0].node == NodeKind::position && f[0].index == 2 && f[0].id == "p3", f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "p9";
    m.positions[1].answers = "q7";
    m.arguments[3].bears[0].target = "a9";
    const auto f = pa::faults(m);
    s.require("references to ids that are not in the map are found, each where it is: q2 raised by p9, p2 answering q7, a4 against a9",
              f.size() == 3 && f[0].kind == FaultKind::unknown_reference && f[0].node == NodeKind::question && f[0].index == 1 && f[0].involved == std::vector<std::string>{"p9"} && f[1].kind == FaultKind::unknown_reference &&
                  f[1].node == NodeKind::position && f[1].id == "p2" && f[1].involved == std::vector<std::string>{"q7"} && f[2].kind == FaultKind::unknown_reference && f[2].node == NodeKind::argument && f[2].index == 3 &&
                  f[2].involved == std::vector<std::string>{"a9"},
              f.size() == 3 ? f[0].why + " | " + f[1].why + " | " + f[2].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "q1";
    m.positions[2].answers = "a4";
    m.arguments[1].bears[0].target = "q2";
    const auto f = pa::faults(m);
    s.require("references of the wrong kind are found: a question raised by a question, a position answering an argument, an argument on a question",
              f.size() == 3 && f[0].kind == FaultKind::wrong_kind && f[0].id == "q2" && f[1].kind == FaultKind::wrong_kind && f[1].id == "p3" && f[2].kind == FaultKind::wrong_kind && f[2].id == "a2" &&
                  f[2].involved == std::vector<std::string>{"q2"},
              f.size() == 3 ? f[0].why + " | " + f[1].why + " | " + f[2].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m = E;
    m.arguments[2].bears.push_back({"a3", Polarity::pro});
    const auto f = pa::faults(m);
    pa::Map against = E;
    against.arguments[2].bears.push_back({"a3", Polarity::con});
    s.require("an argument for itself is found (a3), and an argument against itself is no fault", f.size() == 1 && f[0].kind == FaultKind::argues_for_itself && f[0].node == NodeKind::argument && f[0].index == 2 && f[0].id == "a3" && pa::well_formed(against),
              f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m = E;
    m.arguments[3].bears.clear();
    m.arguments[0].bears.push_back({"p2", Polarity::pro});
    const auto f = pa::faults(m);
    s.require("an argument that bears twice on one target (a1 on p2) and one that bears on nothing (a4) are found",
              f.size() == 2 && f[0].kind == FaultKind::repeated_bearing && f[0].id == "a1" && f[0].involved == std::vector<std::string>{"p2"} && f[1].kind == FaultKind::bears_on_nothing && f[1].id == "a4", f.size() == 2 ? f[0].why + " | " + f[1].why : "");
  }
  {
    pa::Map m = E;
    m.positions[1].id = "q1";   // p2 takes a question's id: what bore on p2 now bears on a question
    const auto f = pa::faults(m);
    s.require("an id used twice is found at its second node (the second position), and so is the reference its old id leaves without a target",
              f.size() == 2 && f[0].kind == FaultKind::duplicate_id && f[0].node == NodeKind::position && f[0].index == 1 && f[0].id == "q1" && f[1].kind == FaultKind::unknown_reference && f[1].id == "a1",
              f.size() == 2 ? f[0].why + " | " + f[1].why : std::to_string(f.size()) + " faults");
  }
  {
    pa::Map m;
    m.questions = {{"", "no id", false, ""}, {"two words", "", false, ""}, {"a:b", "", false, ""}, {"->", "", false, ""}};
    const auto f = pa::faults(m);
    s.require("ids that are empty, of two words, with a colon, or an arrow are found, each at its question",
              f.size() == 4 && f[0].kind == FaultKind::bad_id && f[0].index == 0 && f[1].kind == FaultKind::bad_id && f[1].index == 1 && f[2].kind == FaultKind::bad_id && f[2].index == 2 && f[3].kind == FaultKind::bad_id && f[3].index == 3);
  }
  {
    pa::Map m;
    m.questions = {{"q1", "", false, "a1"}};
    m.positions = {{"p1", "", "q1"}};
    m.arguments = {{"a1", "", {{"p1", Polarity::con}}, {}, ""}};
    const auto f = pa::faults(m);
    s.require("a cycle of 'raised by' is found at its question, with the nodes round it: q1 raised by a1, against p1, which answers q1",
              f.size() == 1 && f[0].kind == FaultKind::raised_by_cycle && f[0].node == NodeKind::question && f[0].id == "q1" && f[0].involved == std::vector<std::string>{"q1", "a1", "p1"}, f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m;
    m.questions = {{"q0", "", false, ""}, {"q1", "", false, "p2"}, {"q2", "", false, "p1"}, {"q3", "", false, "p0"}};
    m.positions = {{"p0", "", "q0"}, {"p1", "", "q1"}, {"p2", "", "q2"}};
    const auto f = pa::faults(m);
    s.require("a cycle through two questions is reported once, at the first: q1 raised by p2, which answers q2, raised by p1, which answers q1; q3, raised by p0, is in none",
              f.size() == 1 && f[0].kind == FaultKind::raised_by_cycle && f[0].index == 1 && f[0].involved == std::vector<std::string>{"q1", "p2", "q2", "p1"}, f.empty() ? "" : f[0].why);
  }
  {
    pa::Map m;
    m.questions = {{"q", "Will Nixon keep the peace?", false, ""}};
    m.positions = {{"p", "He will.", "q"}};
    m.arguments = {{"quaker", "Nixon is a Quaker, and Quakers are pacifists.", {{"p", Polarity::pro}, {"republican", Polarity::con}}, {}, ""},
                   {"republican", "Nixon is a Republican, and Republicans are not pacifists.", {{"p", Polarity::con}, {"quaker", Polarity::con}}, {}, ""}};
    const auto ev = pa::evaluate(m);
    s.require("two arguments against each other are no fault: the Nixon diamond as a map leaves its position undecided and its question open",
              pa::well_formed(m) && ev.positions[0].status == pa::Status::undecided && ev.positions[0].deciding == std::vector<std::string>{"republican"} && ev.open_questions == std::vector<std::string>{"q"} &&
                  ev.arguments[0].status == pa::Status::undecided && ev.arguments[1].status == pa::Status::undecided);
  }
  {
    pa::Map m = E;
    m.positions[2].answers = "";
    m.arguments[0].bears[0].target = "p7";
    m.questions.push_back({"q1", "again", false, ""});
    const auto f = pa::faults(m);
    s.require("several faults are all reported, ids first and then references in the map's order",
              f.size() == 3 && f[0].kind == FaultKind::duplicate_id && f[0].node == NodeKind::question && f[0].index == 2 && f[1].kind == FaultKind::answers_nothing && f[2].kind == FaultKind::unknown_reference && !pa::well_formed(m));
    s.require("a map with faults is not evaluated: induce and evaluate refuse it, naming the first fault",
              throws<std::invalid_argument>([&] { pa::induce(m); }) && throws<std::invalid_argument>([&] { pa::evaluate(m); }) && throws<std::invalid_argument>([&] { pa::open_questions(m); }));
  }

  // ==== Maps: the induced framework, standings, open questions ===================================================================

  using pa::AttackKind;
  using pa::Status;
  {
    const auto ind = pa::induce(E);
    const std::vector<pa::AttackOrigin> origins{{AttackKind::exclusive, "q1", {}}, {AttackKind::exclusive, "q1", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}, {AttackKind::con, "", {}}};
    s.require("the hand-made map induces the framework worked by hand: p1 ⇄ p2 (exclusive), a1 → p2, a2 → a1, a3 → a2, a4 → p3",
              ind.ids == std::vector<std::string>{"p1", "p2", "p3", "a1", "a2", "a3", "a4"} && ind.positions == 3 && ind.framework.size() == 7 &&
                  ind.framework.attack.edges == Edges{{0, 1}, {1, 0}, {3, 1}, {4, 3}, {5, 4}, {6, 2}} && ind.origins == origins && ind.index("a3") == 5 && throws<std::out_of_range>([&] { ind.index("q1"); }));
    const auto ev = pa::evaluate(E);
    const auto& p1 = standing_of(ev.positions, "p1");
    const auto& p2 = standing_of(ev.positions, "p2");
    const auto& p3 = standing_of(ev.positions, "p3");
    s.require("standings: p1 accepted at round 3, defended by a1 against p2; p2 rejected at round 2 by a1; p3 rejected at round 1 by a4",
              p1.status == Status::accepted && p1.round == 3 && p1.attackers == std::vector<std::string>{"p2"} && p1.deciding == std::vector<std::string>{"a1"} && p2.status == Status::rejected && p2.round == 2 &&
                  p2.attackers == std::vector<std::string>{"p1", "a1"} && p2.deciding == std::vector<std::string>{"a1"} && p3.status == Status::rejected && p3.round == 1 && p3.deciding == std::vector<std::string>{"a4"},
              "p1 by " + show(p1.deciding) + ", p2 by " + show(p2.deciding) + ", p3 by " + show(p3.deciding));
    const auto& a1 = standing_of(ev.arguments, "a1");
    const auto& a2 = standing_of(ev.arguments, "a2");
    const auto& a3 = standing_of(ev.arguments, "a3");
    s.require("standings of the arguments: a3 accepted unattacked, a2 rejected by it, a1 reinstated at round 2 with a3 its defender",
              a3.status == Status::accepted && a3.round == 1 && a3.attackers.empty() && a3.deciding.empty() && a2.status == Status::rejected && a2.deciding == std::vector<std::string>{"a3"} && a1.status == Status::accepted && a1.round == 2 &&
                  a1.deciding == std::vector<std::string>{"a3"} && ev.arguments.size() == 4 && ev.grounded.extension == Set{0, 3, 5, 6});
    const std::vector<pa::QuestionStanding> questions{{"q1", {"p1"}, {"p2"}, {}, false, false}, {"q2", {}, {"p3"}, {}, true, false}};
    s.require("the open questions: q2, whose one position is rejected; q1 is answered by p1", ev.open_questions == std::vector<std::string>{"q2"} && ev.questions == questions && pa::open_questions(E) == ev.open_questions && pa::standings(E) == ev.positions);
  }
  {
    pa::Map m = E;
    m.arguments.erase(m.arguments.begin() + 2);   // without a3, a2 stands and defeats a1
    const auto ev = pa::evaluate(m);
    const auto& p1 = standing_of(ev.positions, "p1");
    const auto& p2 = standing_of(ev.positions, "p2");
    s.require("without a3 the argument a1 is defeated, the two exclusive positions leave each other undecided, and both questions are open",
              standing_of(ev.arguments, "a1").status == Status::rejected && p1.status == Status::undecided && p1.round == 0 && p1.deciding == std::vector<std::string>{"p2"} && p2.status == Status::undecided &&
                  p2.deciding == std::vector<std::string>{"p1"} && ev.open_questions == std::vector<std::string>{"q1", "q2"} && ev.questions[0].undecided == std::vector<std::string>{"p1", "p2"});
  }
  {
    pa::Map m = E;
    m.questions[1].raised_by = "p2";
    const auto ev = pa::evaluate(m);
    s.require("a question raised by a rejected position is reported moot", ev.questions[1].moot && !ev.questions[0].moot && ev.questions[1].open);
  }
  {
    pa::Map tie;
    tie.questions = {{"q", "", true, ""}, {"unanswered", "", false, ""}};
    tie.positions = {{"p1", "", "q"}, {"p2", "", "q"}};
    const auto ev = pa::evaluate(tie);
    s.require("two exclusive positions with nothing said on them are both undecided; a question with no position is open",
              ev.positions[0].status == Status::undecided && ev.positions[1].status == Status::undecided && ev.open_questions == std::vector<std::string>{"q", "unanswered"});
    pa::Map plural = tie;
    plural.questions[0].exclusive = false;
    const auto ev2 = pa::evaluate(plural);
    s.require("when the question is not exclusive both positions are accepted and it is not open",
              ev2.induced.framework.attack.edges.empty() && ev2.positions[0].status == Status::accepted && ev2.positions[1].status == Status::accepted && ev2.open_questions == std::vector<std::string>{"unanswered"});

    pa::Map one = tie;
    one.arguments = {{"a1", "", {{"p1", Polarity::pro}}, {}, ""}};
    const auto ind = pa::induce(one);
    const auto ev3 = pa::evaluate(one);
    s.require("an argument for a position counts against its exclusive rival, by the supported attack a1 → p2 through p1; p1 is then accepted with a1 its defender",
              ind.framework.attack.edges == Edges{{0, 1}, {1, 0}, {2, 1}} && ind.origins[2] == pa::AttackOrigin{AttackKind::supported, "", {"a1", "p1"}} && ev3.positions[0].status == Status::accepted &&
                  ev3.positions[0].deciding == std::vector<std::string>{"a1"} && ev3.positions[1].status == Status::rejected && ev3.positions[1].deciding == std::vector<std::string>{"a1"} && ev3.open_questions == std::vector<std::string>{"unanswered"});
    const auto ev4 = pa::evaluate(one, {.supported_attacks = false});
    s.require("read without supported attacks, the argument for p1 plays no part and the tie stands", ev4.induced.framework.attack.edges == Edges{{0, 1}, {1, 0}} && ev4.positions[0].status == Status::undecided && ev4.positions[1].status == Status::undecided);

    pa::Map both = one;
    both.arguments.push_back({"a2", "", {{"p2", Polarity::pro}}, {}, ""});
    const auto ev5 = pa::evaluate(both);
    const auto ev6 = pa::evaluate(both, {.supported_attacks = false});
    s.require("the caveat: exclusive rivals each with an unanswered argument for it are both rejected, each by the other's argument; without supported attacks both are undecided; the question is open either way",
              ev5.positions[0].status == Status::rejected && ev5.positions[0].deciding == std::vector<std::string>{"a2"} && ev5.positions[1].status == Status::rejected && ev5.positions[1].deciding == std::vector<std::string>{"a1"} &&
                  ev5.open_questions == std::vector<std::string>{"q", "unanswered"} && ev6.positions[0].status == Status::undecided && ev6.open_questions == ev5.open_questions);
    both.arguments.push_back({"a3", "", {{"a2", Polarity::con}}, {}, ""});
    const auto ev7 = pa::evaluate(both);
    s.require("once the rival's argument is answered, the position is accepted", ev7.positions[0].status == Status::accepted && ev7.positions[0].round == 2 && ev7.positions[1].status == Status::rejected && ev7.open_questions == std::vector<std::string>{"unanswered"});
  }
  {
    pa::Map m;
    m.questions = {{"q", "", false, ""}};
    m.positions = {{"p", "", "q"}};
    m.arguments = {{"liar", "This argument is unsound.", {{"liar", Polarity::con}, {"p", Polarity::con}}, {}, ""}};
    const auto ev = pa::evaluate(m);
    s.require("a self-defeating argument against a position leaves the position undecided", pa::well_formed(m) && ev.arguments[0].status == Status::undecided && ev.positions[0].status == Status::undecided && ev.positions[0].deciding == std::vector<std::string>{"liar"});
    pa::Map chain;
    chain.questions = {{"q", "", false, ""}};
    chain.positions = {{"p", "", "q"}};
    chain.arguments = {{"a1", "", {{"p", Polarity::con}}, {}, ""}, {"a2", "", {{"a1", Polarity::pro}}, {}, ""}, {"a3", "", {{"a1", Polarity::con}}, {}, ""}};
    const auto ind = pa::induce(chain);
    const auto ev2 = pa::evaluate(chain);
    s.require("an argument for an argument against p attacks p (a2 through a1), and still does when a1 itself is defeated",
              ind.framework.attack.edges == Edges{{1, 0}, {3, 1}, {2, 0}} && ind.origins[2] == pa::AttackOrigin{AttackKind::supported, "", {"a2", "a1"}} && standing_of(ev2.arguments, "a1").status == Status::rejected &&
                  ev2.positions[0].status == Status::rejected && ev2.positions[0].deciding == std::vector<std::string>{"a2"});
  }

  // ==== Maps: the written form ==================================================================================================

  {
    const std::string expected =
        "exclusive question q1: Is the pious loved by the gods because it is pious, or pious because they love it?\n"
        "question q2 <- p1: What is the pious, if it is not what the gods love?\n"
        "\n"
        "position p1 -> q1: It is loved because it is pious.\n"
        "position p2 -> q1: It is pious because it is loved.\n"
        "position p3 -> q2: The pious is the part of the just that cares for the gods.\n"
        "\n"
        "con a1 -> p2: The gods love the pious for being pious, so their love cannot be what makes it pious.\n"
        "premise a1: The gods love the pious because it is pious.\n"
        "premise a1: What is loved for being so is not made so by being loved.\n"
        "conclusion a1: The pious is not pious because the gods love it.\n"
        "con a2 -> a1: A standard of piety apart from the gods' love would stand above the gods.\n"
        "con a3 -> a2: A love without a reason is arbitrary, and gives no one a reason to be pious.\n"
        "con a4 -> p3: Care betters what it cares for, and the gods are not bettered by us.\n";
    const std::string written = pa::write(E);
    s.require("the hand-made map is written as the lines expected", written == expected);
    const auto back = pa::read(written);
    s.require("what is written is read back as the same map", back.ok() && back.map == E, std::to_string(back.errors.size()) + " errors");
  }
  {
    const std::string text =
        "\xEF\xBB\xBF# a map written by hand, out of order, with Windows line ends\r\n"
        "\r\n"
        "pro a1 -> p1: Each is owed what is theirs.\r\n"
        "  position p1 -> q1:   Justice is giving each their due.  \r\n"
        "exclusive question q1: What is justice?\r\n"
        "position p2 -> q1: Justice is the interest of the stronger.\r\n"
        "\t# a further bearing carries no text\r\n"
        "con a1 -> p2\r\n"
        "premise a1: A due is something owed.\r\n"
        "argument a2\r\n"
        "question q2 <- p2:\r\n"
        "conclusion a1: So justice gives each their due: no more.";
    const auto r = pa::read(text);
    pa::Map m;
    m.questions = {{"q1", "What is justice?", true, ""}, {"q2", "", false, "p2"}};
    m.positions = {{"p1", "Justice is giving each their due.", "q1"}, {"p2", "Justice is the interest of the stronger.", "q1"}};
    m.arguments = {{"a1", "Each is owed what is theirs.", {{"p1", Polarity::pro}, {"p2", Polarity::con}}, {"A due is something owed."}, "So justice gives each their due: no more."}, {"a2", "", {}, {}, ""}};
    s.require("a text written by hand is read: remarks, empty lines, a byte order mark, CR LF, statements in any order, a further bearing, a colon inside a text",
              r.ok() && r.map == m, r.ok() ? "" : "line " + std::to_string(r.errors[0].line) + ": " + r.errors[0].why);
    const auto f = pa::faults(r.map);
    s.require("the reader reads syntax only: the argument that bears on nothing is a fault for faults() to find", f.size() == 1 && f[0].kind == FaultKind::bears_on_nothing && f[0].id == "a2");
  }
  {
    pa::Map m;
    m.questions = {{"q.1", "  two spaces before, a tab after\t", false, ""}, {"q-2", "", true, "p_1"}};
    m.positions = {{"p_1", "a colon: a hash # an arrow -> a backslash \\ and \\n spelt out", "q.1"}, {"p2", "line one\nline two\r\n", ""}};
    m.arguments = {{"a/1", "naïve — 素朴 — αληθεια", {{"p_1", Polarity::con}, {"a/1", Polarity::con}, {"p2", Polarity::pro}}, {"", " ", "\\s"}, " "}, {"a2", " ", {}, {"x"}, ""}};
    const auto r = pa::read(pa::write(m));
    s.require("texts with blanks at their ends, tabs, new lines, backslashes, colons and letters beyond ASCII are written and read back unchanged; so are a position with no question and an argument with no bearing",
              r.ok() && r.map == m, r.ok() ? "" : "line " + std::to_string(r.errors[0].line) + ": " + r.errors[0].why);
  }
  {
    const std::string text =
        "question q1: a question\n"            // 1
        "issue q9: not a keyword\n"            // 2
        "position p1 -> q1: a position\n"      // 3
        "position p1 -> q1: again\n"           // 4
        "position -> q1: no id\n"              // 5
        "premise a1: before its argument\n"    // 6
        "pro a1 -> p1: an argument\n"          // 7
        "con a1 -> p1: text on a further bearing\n"  // 8
        "conclusion a1: one\n"                 // 9
        "conclusion a1: two\n"                 // 10
        "premise a1\n"                         // 11
        "pro a2 -> p1: a bad escape \\q\n"     // 12
        "exclusive position p2 -> q1: only questions\n"  // 13
        "question q2 <- : raised by nothing\n" // 14
        ": a text with no head\n"              // 15
        "con q1 -> p1\n"                       // 16
        "pro a3 p1: no arrow\n"                // 17
        "con a4 -> p1: ends in a backslash \\\n"   // 18
        "con a5 -> p1: the last line is read\n";   // 19
    const auto r = pa::read(text);
    std::vector<std::size_t> lines;
    for (const auto& e : r.errors) lines.push_back(e.line);
    const auto why = [&](std::size_t line) {
      for (const auto& e : r.errors)
        if (e.line == line) return e.why;
      return std::string();
    };
    s.require("lines that cannot be read are reported, each with its number", lines == std::vector<std::size_t>{2, 4, 5, 6, 8, 10, 11, 12, 13, 14, 15, 16, 17, 18}, std::to_string(lines.size()) + " errors");
    s.require("and with its reason: an unknown keyword, an id stated twice and where, a premise before its argument, text on a further bearing, a second conclusion, an escape that is none",
              why(2).find("'issue'") != std::string::npos && why(4).find("already stated on line 3") != std::string::npos && why(6).find("no argument a1") != std::string::npos && why(8).find("stated on line 7") != std::string::npos &&
                  why(10).find("already has a conclusion") != std::string::npos && why(12).find("\\q is not an escape") != std::string::npos && why(16).find("already stated on line 1") != std::string::npos &&
                  why(18).find("lone backslash") != std::string::npos,
              why(4) + " | " + why(8));
    pa::Map m;
    m.questions = {{"q1", "a question", false, ""}};
    m.positions = {{"p1", "a position", "q1"}};
    m.arguments = {{"a1", "an argument", {{"p1", Polarity::pro}}, {}, "one"}, {"a5", "the last line is read", {{"p1", Polarity::con}}, {}, ""}};
    s.require("the lines that can be read are read all the same", r.map == m && !r.ok());
  }
  {  // Random maps: written, read back, and where well formed evaluated.
    calx::random::Stream rng(20261113, 2, 0, 0);
    const std::string alphabet[] = {"a", "b", " ", " ", "\\", "\n", "\t", ":", "#", "->", "é", "s", "n", "\r"};
    const auto text = [&] {
      std::string t;
      for (std::uint64_t k = rng.below(8); k > 0; k--) t += alphabet[rng.below(std::size(alphabet))];
      return t;
    };
    int trials = 400, round_trip_fails = 0, well_formed = 0, inconsistent = 0;
    for (int t = 0; t < trials; t++) {
      pa::Map m;
      const std::size_t Q = 1 + rng.below(4), P = rng.below(6), A = rng.below(7);
      const auto node = [&]() -> std::string {  // a position or an argument, if there is one
        if (P + A == 0) return "";
        const std::size_t v = rng.below(P + A);
        return v < P ? "p" + std::to_string(v) : "a" + std::to_string(v - P);
      };
      for (std::size_t i = 0; i < Q; i++) m.questions.push_back({"q" + std::to_string(i), text(), rng.below(2) == 0, rng.below(4) == 0 ? node() : ""});
      for (std::size_t i = 0; i < P; i++) m.positions.push_back({"p" + std::to_string(i), text(), "q" + std::to_string(rng.below(Q))});
      for (std::size_t i = 0; i < A; i++) {
        pa::Argument a{"a" + std::to_string(i), text(), {}, {}, rng.below(3) == 0 ? text() : ""};
        for (std::uint64_t k = 1 + rng.below(3); k > 0; k--) {
          const std::string target = node();
          bool seen = false;
          for (const auto& b : a.bears) seen = seen || b.target == target;
          if (!seen) a.bears.push_back({target, rng.below(2) == 0 ? Polarity::pro : Polarity::con});
        }
        for (std::uint64_t k = rng.below(3); k > 0; k--) a.premises.push_back(text());
        m.arguments.push_back(std::move(a));
      }
      const auto r = pa::read(pa::write(m));
      if (!r.ok() || !(r.map == m)) round_trip_fails++;
      if (!pa::well_formed(m)) continue;
      well_formed++;
      // An evaluation hangs together: the accepted are the grounded extension, its labelling is complete, a rejected node has a
      // deciding attacker and an accepted one defenders against every attacker, and a question is open exactly when none of its
      // positions is accepted.
      const auto ev = pa::evaluate(m);
      bool fine = pa::complete_labelling(ev.induced.framework, ev.grounded.label) && ev.positions.size() == P && ev.arguments.size() == A && ev.induced.origins.size() == ev.induced.framework.attack.edges.size();
      std::vector<pa::Standing> all = ev.positions;
      all.insert(all.end(), ev.arguments.begin(), ev.arguments.end());
      Set accepted;
      for (std::size_t v = 0; v < all.size(); v++) {
        if (all[v].status == Status::accepted) accepted.push_back(v);
        fine = fine && all[v].id == ev.induced.ids[v] && (all[v].status == Status::undecided) == (all[v].round == 0);
        if (all[v].status == Status::rejected) fine = fine && !all[v].deciding.empty();
        if (all[v].status == Status::undecided) fine = fine && !all[v].deciding.empty();
        if (all[v].status == Status::accepted) {
          Set defenders;
          for (const auto& id : all[v].deciding) defenders.push_back(ev.induced.index(id));
          std::sort(defenders.begin(), defenders.end());
          fine = fine && pa::defends(ev.induced.framework, defenders, v) && (all[v].attackers.empty() == all[v].deciding.empty());
        }
      }
      fine = fine && accepted == ev.grounded.extension;
      for (std::size_t i = 0; i < Q; i++) {
        bool some = false;
        for (std::size_t j = 0; j < P; j++) some = some || (m.positions[j].answers == m.questions[i].id && ev.positions[j].status == Status::accepted);
        const bool open = std::find(ev.open_questions.begin(), ev.open_questions.end(), m.questions[i].id) != ev.open_questions.end();
        fine = fine && open == !some && ev.questions[i].open == open;
      }
      if (!fine) inconsistent++;
    }
    s.require("400 random maps, faulty ones among them, are written and read back unchanged", round_trip_fails == 0, std::to_string(round_trip_fails) + " differ");
    s.require("the well-formed among them evaluate consistently: the accepted are the grounded extension, each standing has its deciding nodes, the defenders named defend, the open questions are those with no accepted position",
              inconsistent == 0 && well_formed > 100, std::to_string(well_formed) + " well formed");
  }

  return s.done();
}
