// Facts in law: what kind each is, how it stands in a proceeding, what part it plays in reasoning, and what traces of it are kept.
//
// "Fact" is said of many things in law, and a flat list of them mixes distinctions that are independent. Here a fact has a
// kind on each of five axes (the domain's theory note, theory/facts-and-their-registers.md, §2):
//
// 1. Its register and kind. After the three registers of the user's Generative Relational framework (Huang; read from the
//    author's summary): the *ontological* register is what occurred, and is left open, so an occurrence is referred to and
//    never established; the *relational* register holds what emerges from relations, persists and has effect (a state of mind
//    as the law attributes it, a social fact, an institutional fact such as a marriage or a company, a norm in force, a custom,
//    a convention); the *manifestational* register holds traces, which are the archive's and are kept apart (Trace). A
//    relational fact, once it stands, states the three criteria: what it emerged from, since when it persists, what it
//    constrains.
// 2. Its standing in a proceeding: alleged, admitted, noticed, presumed, deemed, proved, not proved, rebutted, found, final.
//    Standings change by stated steps only (can_become).
// 3. Its role in reasoning: evidentiary (what a trace shows), intermediate, ultimate (an element a rule requires); and its
//    scope, adjudicative (about the parties) or legislative (about the world, informing the making and reading of a rule;
//    Davis 1942, An approach to problems of evidence in the administrative process, Harvard Law Review 55).
// 4. Its logical form: positive or negative; actual, counterfactual or dispositional; past, present or future; particular,
//    general or statistical; and whether it is evaluative (negligence, good faith: fact and norm together).
// 5. How it is preserved: by traces, each of a kind (object, recording, document, testimony, data), each a holding of an
//    archive (archeion). One holding may be a trace of several facts, and a fact has many traces; neither identifies the other.
//
// faults() finds what a record of facts may not hold; each rule is a stated convention of this domain, not a rule of any
// jurisdiction's procedure.

#pragma once

#include <apodeixis/kernel/verdict.hpp>

#include <morphe/combinatorics/acyclic.hpp>

#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace law::facts {

using apodeixis::kernel::Verdict;

// ---- Kinds, in their order ------------------------------------------------------------------------------------------------------

enum class Register { ontological, relational, manifestational };

inline std::string_view word(Register r) { return r == Register::ontological ? "ontological" : r == Register::relational ? "relational" : "manifestational"; }

// The kinds and what each is a kind of; the schema (schema/facts.cat, category Kinds) states the same order, and the checks
// hold the two together pair by pair.
inline const std::vector<std::pair<std::string, std::string>>& kinds() {
  static const std::vector<std::pair<std::string, std::string>> k = {
      {"Fact", ""},
      {"Occurrence", "Fact"},
      {"Relational", "Fact"},
      {"Mental", "Relational"},
      {"Social", "Relational"},
      {"Institutional", "Social"},
      {"Normative", "Institutional"},
      {"Custom", "Normative"},
      {"Convention", "Normative"},
      {"Trace", "Fact"},
      {"Object", "Trace"},
      {"Recording", "Trace"},
      {"Document", "Trace"},
      {"Testimony", "Trace"},
      {"Data", "Trace"},
  };
  return k;
}

inline bool is_kind(const std::string& k) {
  for (const auto& [name, parent] : kinds())
    if (name == k) return true;
  return false;
}

// Whether kind a is a kind of b (each kind is a kind of itself).
inline bool is_a(const std::string& a, const std::string& b) {
  for (std::string x = a; !x.empty();) {
    if (x == b) return true;
    std::string up;
    for (const auto& [name, parent] : kinds())
      if (name == x) up = parent;
    x = up;
  }
  return false;
}

inline Register register_of(const std::string& kind) {
  if (!is_kind(kind)) throw std::invalid_argument("law::facts::register_of: no kind " + kind);
  if (is_a(kind, "Trace")) return Register::manifestational;
  if (is_a(kind, "Relational")) return Register::relational;
  return Register::ontological;  // Occurrence, and Fact itself: what obtains, left open
}

// ---- The axes --------------------------------------------------------------------------------------------------------------------

enum class Standing { alleged, admitted, noticed, presumed, deemed, proved, not_proved, rebutted, found, final_ };

inline std::string_view word(Standing s) {
  switch (s) {
    case Standing::alleged: return "alleged";
    case Standing::admitted: return "admitted";
    case Standing::noticed: return "noticed";
    case Standing::presumed: return "presumed";
    case Standing::deemed: return "deemed";
    case Standing::proved: return "proved";
    case Standing::not_proved: return "not proved";
    case Standing::rebutted: return "rebutted";
    case Standing::found: return "found";
    default: return "final";
  }
}

// Whether a fact of this standing is taken to hold in the proceeding.
inline bool established(Standing s) { return s == Standing::admitted || s == Standing::noticed || s == Standing::presumed || s == Standing::deemed || s == Standing::proved || s == Standing::found || s == Standing::final_; }

// The steps by which a standing may change: an allegation is admitted, noticed, presumed, deemed, proved or not proved; a
// presumption is rebutted; what is admitted, noticed, presumed, deemed or proved is found; what is found becomes final. The
// schema's category Standing has these as its generating arrows.
inline bool can_become(Standing from, Standing to) {
  using S = Standing;
  if (from == S::alleged) return to == S::admitted || to == S::noticed || to == S::presumed || to == S::deemed || to == S::proved || to == S::not_proved;
  if (from == S::presumed) return to == S::rebutted || to == S::found;
  if (from == S::admitted || from == S::noticed || from == S::deemed || from == S::proved) return to == S::found;
  if (from == S::found) return to == S::final_;
  return false;
}

enum class Role { evidentiary, intermediate, ultimate };
enum class Scope { adjudicative, legislative };
enum class Polarity { positive, negative };
enum class Modality { actual, counterfactual, dispositional };
enum class Tense { past, present, future };
enum class Generality { particular, general, statistical };

struct Fact {
  std::string id, text;
  std::string kind = "Occurrence";  // one of kinds(), not a Trace
  Standing standing = Standing::alleged;
  Role role = Role::intermediate;
  Scope scope = Scope::adjudicative;
  Polarity polarity = Polarity::positive;
  Modality modality = Modality::actual;
  Tense tense = Tense::past;
  Generality generality = Generality::particular;
  bool evaluative = false;
  std::string about;  // the occurrence it concerns, where it names one
  // A relational fact's three criteria.
  std::string emerged_from;             // the relations or the procedure from which it emerged
  std::string since, until;             // over what interval it persists (until empty: still)
  std::vector<std::string> constrains;  // what it shapes: facts or consequences it bears on
  std::string basis;  // how it came to stand: the basic fact of a presumption, the rule that deems, why it is notorious, the decision
};

// A fact with its id and text, everything else as it is by default.
inline Fact fact(std::string id, std::string text) {
  Fact f;
  f.id = std::move(id);
  f.text = std::move(text);
  return f;
}

struct Trace {
  std::string id, of;   // the fact it is a trace of
  std::string holding;  // the holding's digest in the archive
  std::string kind = "Document";  // a kind of Trace
};

struct Inference {
  std::string id, from, to;
  std::string generalisation;
  bool against = false;
};

struct Record {
  std::string proceeding;
  std::vector<Fact> facts;
  std::vector<Trace> traces;
  std::vector<Inference> inferences;

  const Fact* fact(const std::string& id) const {
    for (const auto& f : facts)
      if (f.id == id) return &f;
    return nullptr;
  }
};

struct Fault {
  std::string where, what;
};

// Whether a fact of this form can be what a trace shows directly. A state of mind, an absence, what would have been, a
// disposition and what will be are never observed: they are reached by inference.
inline bool observable(const Fact& f) {
  return f.kind != "Mental" && f.polarity == Polarity::positive && f.modality == Modality::actual && f.tense != Tense::future && f.generality == Generality::particular;
}

// Every fault of a record of facts.
inline std::vector<Fault> faults(const Record& r) {
  std::vector<Fault> out;
  std::set<std::string> ids;
  for (const auto& f : r.facts)
    if (f.id.empty() || !ids.insert(f.id).second) out.push_back({f.id, "an id is used twice, or is empty"});
  std::map<std::string, std::size_t> index;
  for (std::size_t i = 0; i < r.facts.size(); i++) index.emplace(r.facts[i].id, i);
  std::map<std::string, std::size_t> traces_of, inferences_to;
  for (const auto& t : r.traces) {
    if (!r.fact(t.of)) out.push_back({t.id, "it is a trace of " + t.of + ", which is no fact of the record"});
    else traces_of[t.of]++;
    if (!is_kind(t.kind) || !is_a(t.kind, "Trace") || t.kind == "Trace") out.push_back({t.id, "a trace is an object, a recording, a document, testimony or data, not " + t.kind});
    if (t.holding.empty()) out.push_back({t.id, "a trace is a holding of an archive, and names none"});
  }
  std::vector<morphe::combinatorics::Arc> arcs;
  const auto rank = [](Role x) { return x == Role::evidentiary ? 0 : x == Role::intermediate ? 1 : 2; };
  for (const auto& i : r.inferences) {
    const Fact* a = r.fact(i.from);
    const Fact* b = r.fact(i.to);
    if (!a || !b) {
      out.push_back({i.id, "it runs between facts that are not both in the record"});
      continue;
    }
    if (i.generalisation.empty()) out.push_back({i.id, "an inference rests on a generalisation, and states none"});
    if (rank(a->role) > rank(b->role)) out.push_back({i.id, "it runs from an " + std::string(a->role == Role::ultimate ? "ultimate" : "intermediate") + " fact back to one nearer the evidence"});
    inferences_to[i.to]++;
    arcs.push_back({index[i.from], index[i.to]});
  }
  const auto order = morphe::combinatorics::topological_order(r.facts.size(), arcs);
  if (!order.acyclic) {
    std::string cycle;
    for (const std::size_t v : order.cycle) cycle += (cycle.empty() ? "" : " → ") + r.facts[v].id;
    out.push_back({r.facts[order.cycle.front()].id, "the inferences run in a cycle: " + cycle});
  }
  for (const auto& f : r.facts) {
    if (!is_kind(f.kind) || is_a(f.kind, "Trace") || f.kind == "Fact" || f.kind == "Relational") {
      out.push_back({f.id, f.kind + " is not a kind a fact may be: an occurrence, or a relational kind"});
      continue;
    }
    const Register reg = register_of(f.kind);
    if (!f.about.empty()) {
      const Fact* o = r.fact(f.about);
      if (!o || o->kind != "Occurrence") out.push_back({f.id, "it is about " + f.about + ", which is no occurrence of the record"});
    }
    // The ontological register is left open.
    if (reg == Register::ontological && f.standing != Standing::alleged)
      out.push_back({f.id, "what occurred is left open: an occurrence is referred to, and what is " + std::string(word(f.standing)) + " is a relational fact about it"});
    // A relational fact that stands states its three criteria.
    if (reg == Register::relational && established(f.standing)) {
      if (f.emerged_from.empty()) out.push_back({f.id, "a relational fact that stands says what it emerged from"});
      if (f.since.empty()) out.push_back({f.id, "a relational fact that stands says since when it persists"});
      if (f.constrains.empty()) out.push_back({f.id, "a relational fact that stands says what it constrains"});
      if (!f.until.empty() && !f.since.empty() && f.until < f.since) out.push_back({f.id, "it ends before it begins"});
    }
    // How each standing comes about.
    if (f.standing == Standing::presumed || f.standing == Standing::rebutted) {
      const Fact* basic = r.fact(f.basis);
      if (!basic) out.push_back({f.id, "a presumption rests on a basic fact of the record, and names none"});
      else if (!established(basic->standing)) out.push_back({f.id, "the basic fact of the presumption, " + f.basis + ", does not itself stand"});
    }
    if ((f.standing == Standing::deemed || f.standing == Standing::noticed || f.standing == Standing::found || f.standing == Standing::final_) && f.basis.empty())
      out.push_back({f.id, std::string("a fact ") + std::string(word(f.standing)) + " names " + (f.standing == Standing::deemed ? "the rule that deems it" : f.standing == Standing::noticed ? "why it needs no proof" : "the decision that found it")});
    // What can and cannot be shown directly.
    if (f.role == Role::evidentiary) {
      if (!observable(f)) out.push_back({f.id, "a state of mind, an absence, a counterfactual, a disposition, a future or a general fact is not what a trace shows: it is reached by inference"});
      else if (established(f.standing) && f.standing != Standing::admitted && f.standing != Standing::noticed && !traces_of.count(f.id)) out.push_back({f.id, "an evidentiary fact that stands by proof has a trace, and this has none"});
    } else if (!observable(f) && (f.standing == Standing::proved || f.standing == Standing::found || f.standing == Standing::final_) && !inferences_to.count(f.id)) {
      out.push_back({f.id, "it cannot be observed, and stands by proof: an inference leads to it, and none does"});
    }
    if (f.scope == Scope::legislative && f.generality == Generality::particular) out.push_back({f.id, "a legislative fact is general or statistical, about the world and not the parties"});
  }
  return out;
}

inline Verdict well_formed(const Record& r) {
  const auto fs = faults(r);
  const std::string claim = "the record of " + apodeixis::kernel::count(r.facts.size(), "fact") + " is well formed";
  if (fs.empty()) return Verdict::yes(claim, "every fact has a kind and a standing that its form allows, and what stands says how", r.facts.size());
  auto v = Verdict::no(claim, std::to_string(fs.size()) + (fs.size() == 1 ? " fault: " : " faults, the first: ") + fs[0].where + ": " + fs[0].what, r.facts.size());
  for (const auto& f : fs) v.with("fault", f.where + ": " + f.what);
  return v;
}

// Changes a fact's standing by a stated step, or refuses.
inline void advance(Fact& f, Standing to, const std::string& basis = {}) {
  if (!can_become(f.standing, to)) throw std::invalid_argument("law::facts::advance: a fact " + std::string(word(f.standing)) + " does not become " + std::string(word(to)));
  f.standing = to;
  if (!basis.empty()) f.basis = basis;
}

}  // namespace law::facts
