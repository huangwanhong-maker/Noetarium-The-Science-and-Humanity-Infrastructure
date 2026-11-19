// Theories of the law: how a body of law is to be read for a matter, as a doctrine, judged by fit and by justification (ADR 0133).
//
// The law domain reasons from facts under norms, with the open terms of its rules read in more than one way (apodeixis's points)
// and its precedents read by the reasons they were decided on (Horty's reason model, precedent.hpp). Theories of the case ask what
// happened (theories.hpp); a theory of the law asks how the law is to be read: which reading of each open term, and which ratio of
// each precedent whose court left its reason unstated. Dworkin (1986, Law's Empire, ch. 7) asks of an interpretation of the law that
// it *fit* the legal record (the decisions it must explain) and, among those that fit, that it *justify* it best (by the purposes it
// serves). Here both are computed, from what a scheme and a matter declare, and nothing is assumed:
//
//   a doctrine D = (ι, ρ): ι a reading of each open term of the matter's rules, ρ a ratio for each precedent of each case base whose
//   reason is not stated (a non-empty set of the factors it had for its outcome; a stated reason is kept);
//
//   fit:
//     - the decided cases: each a set of facts of the scheme's vocabulary, a question and whether it was held to follow. Under ι the
//       question follows or not (apodeixis, on the matter's rules); a case the readings get wrong counts its weight, 1 when the
//       court binds the forum, a declared weight (0.3) when it persuades, nothing when it was decided after the date of the question;
//     - authority: ι among the interpretations the matter's arguments and the binding holdings leave admissible (authority.hpp,
//       apodeixis's admissibility); a doctrine outside them is against authority;
//     - coherence: under ρ, the pairs of precedents decided for different sides each of which holds the other's reason outweighed
//       (precedent.hpp's inconsistency, counted);
//   justification: Σ_p w_p Σ_k v_p(term k, ι_k), the declared purposes' worth of the readings, scaled to [0, 1];
//   the score, minimised: W_a·[against authority] + W_c·misfit + W_h·conflicts − W_j·justification, every weight declared.
//
// What a doctrine makes of the matter: each case base decides a fact of the vocabulary (an emergency, say), its first side that the
// fact holds; under ρ the precedents force a side for the matter's situation, or leave it open (precedent.hpp's constraint). The fact
// is then held or denied, or left open; and each question of the matter follows, fails, or turns on the open fact (it follows with
// the fact and not without, or the reverse). That outcome is the doctrine's *kind*: a search keeps the best doctrine of each kind
// (search.hpp), so that the best reading of the law on which a ban follows is found beside the best on which it does not.
//
// The standing of a question over the doctrines found: the acceptable doctrines are those whose score is within a declared tolerance
// of the best; the question is clear when it follows under every acceptable doctrine, untenable when under none, arguable otherwise.
//
// Decisions held back (given after the date of the question, or kept apart by the matter) are never read by the search: each
// doctrine kept is tested once on them, the test registered in archeion's journal before they are read and its outcome written
// after: a held-back decided case is predicted when the readings give its outcome; a held-back precedent is predicted when the
// ratios force its outcome, contradicted when they force the other, and open otherwise.
//
// The written form of a doctrine, a line each (a person's and a model's alike; a line that does not read is refused, never repaired):
//   reading vehicle = narrow
//   ratio emergency/parent: rushing_to_care, life_at_risk
#pragma once

#include <law/authority/authority.hpp>
#include <law/precedent/precedent.hpp>

#include <apodeixis/argumentation/aspic.hpp>
#include <apodeixis/interpretation/interpretation.hpp>
#include <archeion/fixity/journal.hpp>
#include <heuresis/search/protocol.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace law::doctrine {

namespace ai = apodeixis::interpretation;
namespace aa = apodeixis::argumentation;
using precedent::Factors;

// ---- The problem ------------------------------------------------------------------------------------------------------------------

struct Decided {
  std::string id, name, court, date, instrument;
  std::vector<std::string> facts;      // facts of the scheme's vocabulary that held in it
  std::string question;                // a consequence of the rules, decided
  bool held = true;                    // whether it was held to follow
  std::string passage;
  bool held_back = false;              // kept from the search: tested once on each doctrine kept
};

struct CaseBase {
  std::string fact;                    // the fact of the vocabulary the precedents decide: their first side holds it, the second does not
  precedent::Base base;                // its sides, factors and cases, with reasons stated or left to be found
  Factors situation;                   // the matter's factors
  std::set<std::string> held_back;     // cases kept from the search (decided after the question, or kept apart)
};

struct Purpose {
  std::string name;
  double weight = 1;
  std::map<std::string, std::vector<double>> worth;  // term → the purpose's worth of each of its readings, in [0, 1]
};

struct Weights {
  double authority = 10, cases = 1, coherence = 2, justification = 1;
  double persuasive = 0.3;             // a persuasive decision's weight in the misfit (a binding one's is 1)
};

struct Problem {
  ai::Case matter;                     // the matter's case: open points, rules, facts, arguments, canon order
  std::vector<std::string> questions;  // consequences asked
  authority::Forum forum;
  std::vector<authority::Court> courts;
  std::string date;                    // of the question; decisions after it are left out of the fit
  std::vector<Decided> decided;
  std::vector<CaseBase> bases;
  std::vector<Purpose> purposes;
  std::vector<char> admissible;        // over the interpretations (ai::decode); empty: every interpretation
  Weights weights;
};

// ---- A doctrine and its written form ----------------------------------------------------------------------------------------------

struct Doctrine {
  ai::Interpretation readings;                  // a reading of each open point of the matter
  std::vector<std::vector<Factors>> ratios;     // per case base, per case: the reason it is read as decided on
};

// The cases whose reason a doctrine chooses: those whose court did not state one, and not held back.
inline bool open_ratio(const CaseBase& b, const precedent::Precedent& c) { return c.reason.empty() && !b.held_back.count(c.name); }

// The factors a case had for its outcome (the ratios it may be read as).
inline Factors for_outcome(const CaseBase& b, const precedent::Precedent& c) { return precedent::favouring(b.base, c.facts, c.outcome); }

inline std::string write(const Problem& p, const Doctrine& d) {
  std::string s;
  for (std::size_t k = 0; k < p.matter.points.size() && k < d.readings.size(); k++)
    s += "reading " + p.matter.points[k].name + " = " + p.matter.points[k].readings[d.readings[k]] + "\n";
  for (std::size_t b = 0; b < p.bases.size() && b < d.ratios.size(); b++)
    for (std::size_t c = 0; c < p.bases[b].base.cases.size() && c < d.ratios[b].size(); c++) {
      if (!open_ratio(p.bases[b], p.bases[b].base.cases[c])) continue;
      std::string fs;
      for (const auto& f : d.ratios[b][c]) fs += (fs.empty() ? "" : ", ") + f;
      s += "ratio " + p.bases[b].fact + "/" + p.bases[b].base.cases[c].name + ": " + fs + "\n";
    }
  return s;
}

// The canonical key of a doctrine (its written form).
inline std::string key(const Problem& p, const Doctrine& d) { return write(p, d); }

// The result model's doctrine part for every open ratio (every factor for the outcome), and the first reading of every point.
inline Doctrine plain(const Problem& p) {
  Doctrine d;
  d.readings.assign(p.matter.points.size(), 0);
  for (const auto& b : p.bases) {
    std::vector<Factors> r;
    for (const auto& c : b.base.cases) r.push_back(c.reason.empty() ? for_outcome(b, c) : c.reason);
    d.ratios.push_back(r);
  }
  return d;
}

struct Read {
  bool ok = false;
  Doctrine doctrine;
  std::vector<std::string> errors;     // "line n: why", each line refused on its own
};

namespace detail {

inline std::string trim(std::string_view s) {
  std::size_t a = 0, b = s.size();
  while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r' || s[a] == '-' || s[a] == '*')) a++;
  while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r')) b--;
  return std::string(s.substr(a, b - a));
}

}  // namespace detail

// A doctrine from its written form, starting from `from` (the result model and first readings when absent): each line read on its
// own; a line that names no point, reading, base, case or factor, or a factor the case did not have for its outcome, is refused.
inline Read read(const Problem& p, std::string_view text, std::optional<Doctrine> from = std::nullopt) {
  Read out;
  out.doctrine = from ? *from : plain(p);
  std::size_t line = 0, start = 0;
  while (start <= text.size()) {
    std::size_t end = text.find('\n', start);
    if (end == std::string_view::npos) end = text.size();
    line++;
    const std::string l = detail::trim(text.substr(start, end - start));
    const auto fail = [&](const std::string& why) { out.errors.push_back("line " + std::to_string(line) + ": " + why); };
    if (!l.empty() && l[0] != '#') {
      if (l.rfind("reading ", 0) == 0) {
        const auto eq = l.find('=');
        if (eq == std::string::npos) fail("a reading is written 'reading <term> = <reading>'");
        else {
          const std::string term = detail::trim(std::string_view(l).substr(8, eq - 8)), r = detail::trim(std::string_view(l).substr(eq + 1));
          const long k = ai::point_index(p.matter, term);
          if (k < 0) fail("no open term '" + term + "' reaches the matter's questions");
          else {
            const auto& rs = p.matter.points[static_cast<std::size_t>(k)].readings;
            const auto it = std::find(rs.begin(), rs.end(), r);
            if (it == rs.end()) fail("the term " + term + " does not admit the reading '" + r + "'");
            else out.doctrine.readings[static_cast<std::size_t>(k)] = static_cast<std::size_t>(it - rs.begin());
          }
        }
      } else if (l.rfind("ratio ", 0) == 0) {
        const auto colon = l.find(':'), slash = l.find('/');
        if (colon == std::string::npos || slash == std::string::npos || slash > colon) fail("a ratio is written 'ratio <fact>/<case>: <factor>, …'");
        else {
          const std::string fact = detail::trim(std::string_view(l).substr(6, slash - 6)), name = detail::trim(std::string_view(l).substr(slash + 1, colon - slash - 1));
          std::size_t b = p.bases.size(), c = 0;
          for (std::size_t i = 0; i < p.bases.size(); i++)
            if (p.bases[i].fact == fact) b = i;
          if (b == p.bases.size()) fail("no case base decides '" + fact + "'");
          else {
            const auto& cs = p.bases[b].base.cases;
            c = cs.size();
            for (std::size_t i = 0; i < cs.size(); i++)
              if (cs[i].name == name) c = i;
            if (c == cs.size()) fail("the case base on " + fact + " has no case '" + name + "'");
            else if (!open_ratio(p.bases[b], cs[c])) fail(name + "'s reason is stated by its court" + std::string(p.bases[b].held_back.count(name) ? " or it is held back" : "") + ": it is not to be chosen");
            else {
              Factors r;
              std::string bad;
              const Factors allowed = for_outcome(p.bases[b], cs[c]);
              std::string list = l.substr(colon + 1);
              std::size_t q = 0;
              while (q <= list.size()) {
                std::size_t e = list.find(',', q);
                if (e == std::string::npos) e = list.size();
                const std::string f = detail::trim(std::string_view(list).substr(q, e - q));
                if (!f.empty()) {
                  if (!allowed.count(f)) bad = f;
                  else r.insert(f);
                }
                if (e == list.size()) break;
                q = e + 1;
              }
              if (!bad.empty()) fail("'" + bad + "' is not a factor " + name + " had for " + cs[c].outcome + " (it had " + precedent::words(allowed) + ")");
              else if (r.empty()) fail("a ratio names at least one factor");
              else out.doctrine.ratios[b][c] = r;
            }
          }
        }
      } else {
        fail("a line is 'reading …' or 'ratio …'");
      }
    }
    if (end == text.size()) break;
    start = end + 1;
  }
  out.ok = out.errors.empty();
  return out;
}

// ---- Assessing a doctrine ----------------------------------------------------------------------------------------------------------

enum class Outcome { fails = 0, holds = 1, turns = 2 };

inline const char* name(Outcome o) { return o == Outcome::holds ? "follows" : o == Outcome::fails ? "does not follow" : "turns on the precedents"; }

struct CaseFit {
  std::string id;
  authority::Force force = authority::Force::left_out;
  double weight = 0;
  bool follows = false;                // under the doctrine's readings
  bool agrees = false;
  std::string reason;
};

struct Assessment {
  std::vector<CaseFit> cases;          // the decided cases not held back
  double misfit = 0;                   // the weights of those the readings get wrong
  bool admissible = true;
  std::vector<std::string> conflicts;  // pairs of precedents made inconsistent by the ratios, "fact: a and b"
  double justification = 0;            // in [0, 1]
  std::vector<std::string> forced;     // per case base: the side the precedents force for the matter, or "" when open
  std::vector<std::string> forcing;    // per case base: the precedents that force it, or that distinguish it, in words
  std::vector<Outcome> outcomes;       // per question
  double score = 0;
  std::string summary;
};

namespace detail {

inline Factors complement(const Factors& a, const Factors& b) {
  Factors out;
  std::set_difference(a.begin(), a.end(), b.begin(), b.end(), std::inserter(out, out.begin()));
  return out;
}

// The base with the doctrine's ratios as its reasons, and without the held-back cases.
inline precedent::Base as_read(const CaseBase& cb, const std::vector<Factors>& ratios) {
  precedent::Base b = cb.base;
  b.cases.clear();
  for (std::size_t c = 0; c < cb.base.cases.size(); c++) {
    if (cb.held_back.count(cb.base.cases[c].name)) continue;
    precedent::Precedent x = cb.base.cases[c];
    if (x.reason.empty() && c < ratios.size()) x.reason = ratios[c];
    b.cases.push_back(x);
  }
  return b;
}

inline bool literal_of(const std::string& l, const std::string& fact) { return l == fact || l == aa::negate(fact); }

}  // namespace detail

// The matter's case with the case bases' facts as a doctrine makes them: held, denied, or left out (`values` per base: 1, 0 or -1).
inline ai::Case matter_under(const Problem& p, const std::vector<int>& values) {
  ai::Case c = p.matter;
  for (std::size_t b = 0; b < p.bases.size(); b++) {
    const std::string& f = p.bases[b].fact;
    const auto drop = [&](std::vector<std::string>& v) { v.erase(std::remove_if(v.begin(), v.end(), [&](const std::string& l) { return detail::literal_of(l, f); }), v.end()); };
    drop(c.theory.axioms), drop(c.theory.premises);
    if (values[b] == 1) c.theory.axioms.push_back(f);
    else if (values[b] == 0) c.theory.axioms.push_back(aa::negate(f));
  }
  return c;
}

// A decided case's own case: the matter's rules and points, its facts as given.
inline ai::Case decided_case(const Problem& p, const Decided& d) {
  ai::Case c = p.matter;
  c.theory.axioms = d.facts;
  c.theory.premises.clear();
  return c;
}

inline double force_weight(const Problem& p, const Decided& d, authority::Force* f, std::string* why) {
  const authority::Court* court = nullptr;
  for (const auto& c : p.courts)
    if (c.id == d.court) court = &c;
  if (!court) {
    *f = authority::Force::left_out;
    *why = "its court " + d.court + " is not declared";
    return 0;
  }
  if (!p.date.empty() && !d.date.empty() && d.date > p.date) {
    *f = authority::Force::left_out;
    *why = "decided on " + d.date + ", after the date of the question";
    return 0;
  }
  *f = authority::force_for(*court, p.forum, d.instrument);
  *why = std::string(authority::word(*f)) + ": " + court->name;
  return *f == authority::Force::binding ? 1.0 : p.weights.persuasive;
}

inline double justification(const Problem& p, const ai::Interpretation& readings) {
  double total = 0, best = 0;
  for (const auto& pu : p.purposes) {
    if (!(pu.weight > 0)) continue;
    for (const auto& [term, worth] : pu.worth) {
      const long k = ai::point_index(p.matter, term);
      if (k < 0 || worth.empty()) continue;
      const std::size_t r = readings[static_cast<std::size_t>(k)];
      total += pu.weight * (r < worth.size() ? worth[r] : 0);
      best += pu.weight * *std::max_element(worth.begin(), worth.end());
    }
  }
  return best > 0 ? total / best : 0;
}

// The faults of a problem: what a doctrine could not be assessed against, named.
inline std::vector<std::string> faults(const Problem& p) {
  std::vector<std::string> out;
  if (p.questions.empty()) out.push_back("the matter asks no question");
  for (const auto& d : p.decided) {
    if (d.question.empty()) out.push_back("the decided case " + d.id + " decides no question");
    bool court = false;
    for (const auto& c : p.courts) court = court || c.id == d.court;
    if (!court) out.push_back("the decided case " + d.id + " names the court " + d.court + ", which is not declared");
  }
  for (const auto& b : p.bases) {
    if (b.fact.empty()) out.push_back("a case base decides no fact");
    for (const auto& c : b.base.cases) {
      try {
        (void)precedent::reason_of(b.base, c);
      } catch (const std::exception& e) {
        out.push_back(b.fact + ": " + e.what());
      }
    }
    for (const auto& f : b.situation)
      if (!b.base.side_of.count(f)) out.push_back(b.fact + ": the matter's factor " + f + " is not declared");
  }
  for (const auto& pu : p.purposes)
    for (const auto& [term, worth] : pu.worth) {
      const long k = ai::point_index(p.matter, term);
      if (k < 0) out.push_back("the purpose " + pu.name + " values the term " + term + ", which reaches no question");
      else if (worth.size() != p.matter.points[static_cast<std::size_t>(k)].readings.size()) out.push_back("the purpose " + pu.name + " gives " + term + " a worth for each of its readings");
    }
  if (!p.admissible.empty() && p.admissible.size() != ai::space(p.matter.points)) out.push_back("the admissible interpretations are given over every interpretation");
  return out;
}

inline Assessment assess(const Problem& p, const Doctrine& d) {
  Assessment a;
  const auto& W = p.weights;
  // Fit to the decided cases.
  for (const auto& dc : p.decided) {
    if (dc.held_back) continue;
    CaseFit cf;
    cf.id = dc.id;
    cf.weight = force_weight(p, dc, &cf.force, &cf.reason);
    if (cf.force != authority::Force::left_out) {
      const ai::Case c = decided_case(p, dc);
      cf.follows = ai::entailment(c, aa::literal(dc.question))(d.readings).holds();
      cf.agrees = cf.follows == dc.held;
      if (!cf.agrees) a.misfit += cf.weight;
    }
    a.cases.push_back(cf);
  }
  // Authority.
  if (!p.admissible.empty()) a.admissible = p.admissible[ai::encode(p.matter.points, d.readings)] != 0;
  // Coherence, and what the precedents make of the matter.
  std::vector<int> values;
  for (std::size_t b = 0; b < p.bases.size(); b++) {
    const auto& cb = p.bases[b];
    const precedent::Base read = detail::as_read(cb, b < d.ratios.size() ? d.ratios[b] : std::vector<Factors>{});
    for (std::size_t i = 0; i < read.cases.size(); i++)
      for (std::size_t j = 0; j < read.cases.size(); j++) {
        const auto& x = read.cases[i];
        const auto& y = read.cases[j];
        if (x.outcome != read.sides[0] || y.outcome == x.outcome) continue;
        const auto rx = precedent::reason_of(read, x), ry = precedent::reason_of(read, y);
        if (precedent::within(rx, precedent::favouring(read, y.facts, x.outcome)) && precedent::within(ry, precedent::favouring(read, x.facts, y.outcome)))
          a.conflicts.push_back(cb.fact + ": " + x.name + " and " + y.name);
      }
    const auto k = precedent::constraint(read, cb.situation);
    a.forced.push_back(a.conflicts.empty() || k.verdict.holds() ? k.forced : "");
    std::string why;
    if (!k.forced.empty()) {
      for (const auto& f : k.forcing) why += (why.empty() ? "forced by " : ", ") + f;
    } else {
      for (const auto& g : k.distinguished) why += (why.empty() ? "" : "; ") + g.precedent + " distinguished by " + precedent::words(g.by);
      if (why.empty()) why = "no precedent's reason is present";
    }
    a.forcing.push_back(why);
    // Precedents that force both sides (an inconsistent base) force neither: the fact is left open.
    const std::string& side = a.forced.back();
    values.push_back(side.empty() ? -1 : side == cb.base.sides[0] ? 1 : 0);
  }
  // Justification.
  a.justification = justification(p, d.readings);
  // The matter's questions.
  std::vector<std::size_t> open;
  for (std::size_t b = 0; b < values.size(); b++)
    if (values[b] < 0) open.push_back(b);
  for (const auto& q : p.questions) {
    bool any_holds = false, any_fails = false;
    const std::size_t combos = open.size() < 16 ? (std::size_t{1} << open.size()) : 1;
    for (std::size_t m = 0; m < combos; m++) {
      auto v = values;
      for (std::size_t j = 0; j < open.size(); j++) v[open[j]] = (m >> j) & 1 ? 1 : 0;
      const ai::Case c = matter_under(p, v);
      if (ai::entailment(c, aa::literal(q))(d.readings).holds()) any_holds = true;
      else any_fails = true;
    }
    a.outcomes.push_back(any_holds && any_fails ? Outcome::turns : any_holds ? Outcome::holds : Outcome::fails);
  }
  a.score = (a.admissible ? 0 : W.authority) + W.cases * a.misfit + W.coherence * static_cast<double>(a.conflicts.size()) - W.justification * a.justification;
  char b[200];
  std::snprintf(b, sizeof b, "misfit %.3g, %s, %zu conflict%s among precedents, justification %.2f", a.misfit, a.admissible ? "within authority" : "against authority", a.conflicts.size(),
                a.conflicts.size() == 1 ? "" : "s", a.justification);
  a.summary = b;
  return a;
}

// The kind of a doctrine: each question's outcome, then each case base's forced side (0 the first, 1 the second, 2 open).
inline std::vector<double> kind(const Problem& p, const Assessment& a) {
  std::vector<double> k;
  for (auto o : a.outcomes) k.push_back(static_cast<double>(static_cast<int>(o)));
  for (std::size_t b = 0; b < a.forced.size(); b++) k.push_back(a.forced[b].empty() ? 2.0 : a.forced[b] == p.bases[b].base.sides[0] ? 0.0 : 1.0);
  return k;
}

inline std::string kind_words(const Problem& p, const std::vector<double>& k) {
  std::string s;
  for (std::size_t q = 0; q < p.questions.size() && q < k.size(); q++)
    s += (s.empty() ? "" : "; ") + p.questions[q] + " " + name(static_cast<Outcome>(static_cast<int>(k[q])));
  for (std::size_t b = 0; b < p.bases.size() && p.questions.size() + b < k.size(); b++) {
    const double v = k[p.questions.size() + b];
    s += "; " + p.bases[b].fact + (v == 2 ? " left open by the precedents" : v == 0 ? " held by the precedents" : " denied by the precedents");
  }
  return s;
}

// ---- Every doctrine, when there are few ---------------------------------------------------------------------------------------------

// The number of doctrines: the product of the points' readings and of the ratios each open case admits (2^n − 1 for n factors).
inline double size(const Problem& p) {
  double n = 1;
  for (const auto& pt : p.matter.points) n *= static_cast<double>(pt.readings.size());
  for (const auto& b : p.bases)
    for (const auto& c : b.base.cases)
      if (open_ratio(b, c)) n *= std::ldexp(1.0, static_cast<int>(for_outcome(b, c).size())) - 1;
  return n;
}

inline constexpr double enumerable = 200000;

// Every doctrine, in a fixed order, handed to `f`. Refused above `limit`.
template <class F>
inline void every(const Problem& p, F&& f, double limit = enumerable) {
  if (size(p) > limit) throw std::invalid_argument("law::doctrine: " + std::to_string(static_cast<long long>(size(p))) + " doctrines are refused above " + std::to_string(static_cast<long long>(limit)));
  struct Slot {
    std::size_t base, cas;
    std::vector<std::string> factors;
  };
  std::vector<Slot> slots;
  for (std::size_t b = 0; b < p.bases.size(); b++)
    for (std::size_t c = 0; c < p.bases[b].base.cases.size(); c++)
      if (open_ratio(p.bases[b], p.bases[b].base.cases[c])) {
        const auto fs = for_outcome(p.bases[b], p.bases[b].base.cases[c]);
        slots.push_back({b, c, std::vector<std::string>(fs.begin(), fs.end())});
      }
  const std::size_t n = ai::space(p.matter.points);
  std::vector<std::size_t> mask(slots.size(), 1);
  Doctrine d = plain(p);
  for (std::size_t x = 0; x < n; x++) {
    d.readings = ai::decode(p.matter.points, x);
    std::fill(mask.begin(), mask.end(), 1);
    while (true) {
      for (std::size_t s = 0; s < slots.size(); s++) {
        Factors r;
        for (std::size_t q = 0; q < slots[s].factors.size(); q++)
          if ((mask[s] >> q) & 1) r.insert(slots[s].factors[q]);
        d.ratios[slots[s].base][slots[s].cas] = r;
      }
      f(d);
      std::size_t s = 0;
      while (s < slots.size() && ++mask[s] == (std::size_t{1} << slots[s].factors.size())) mask[s++] = 1;
      if (s == slots.size()) break;
    }
  }
}

struct Best {
  std::vector<double> kind;
  Doctrine doctrine;
  Assessment assessment;
};

// The best doctrine of each kind, by exhaustion (ties to the first in the fixed order).
inline std::vector<Best> best_of_each_kind(const Problem& p, double limit = enumerable) {
  std::map<std::vector<double>, Best> by;
  every(
      p,
      [&](const Doctrine& d) {
        const auto a = assess(p, d);
        const auto k = kind(p, a);
        auto it = by.find(k);
        if (it == by.end() || a.score < it->second.assessment.score - 1e-12) by[k] = Best{k, d, a};
      },
      limit);
  std::vector<Best> out;
  for (auto& [k, b] : by) out.push_back(std::move(b));
  std::stable_sort(out.begin(), out.end(), [](const Best& x, const Best& y) { return x.assessment.score < y.assessment.score; });
  return out;
}

// ---- The standing of a question over the doctrines found ---------------------------------------------------------------------------

struct Standing {
  std::string question;
  ai::Class verdict = ai::Class::no_admissible;
  std::size_t acceptable = 0, holding = 0, turning = 0;
  std::string reason;
};

// Over the doctrines given (each with its assessment): those within `tolerance` of the best score are acceptable.
inline std::vector<Standing> standings(const Problem& p, const std::vector<Assessment>& found, double tolerance = 0.5) {
  std::vector<Standing> out;
  double best = INFINITY;
  for (const auto& a : found) best = std::min(best, a.score);
  for (std::size_t q = 0; q < p.questions.size(); q++) {
    Standing s;
    s.question = p.questions[q];
    for (const auto& a : found) {
      if (a.score > best + tolerance + 1e-12) continue;
      s.acceptable++;
      if (a.outcomes[q] == Outcome::holds) s.holding++;
      else if (a.outcomes[q] == Outcome::turns) s.turning++;
    }
    s.verdict = s.acceptable == 0 ? ai::Class::no_admissible : s.holding == s.acceptable ? ai::Class::clear : s.holding == 0 && s.turning == 0 ? ai::Class::untenable : ai::Class::arguable;
    char b[200];
    std::snprintf(b, sizeof b, "it follows under %zu of the %zu doctrines within %.2g of the best fit%s", s.holding, s.acceptable, tolerance,
                  s.turning ? (", and turns on the precedents under " + std::to_string(s.turning)).c_str() : "");
    s.reason = b;
    out.push_back(s);
  }
  return out;
}

// The standing of each fact a case base decides, over the same acceptable doctrines: held when the precedents force its first side
// under every one, denied when they force the other under every one, and otherwise turning on the ratios the precedents are read as
// decided on (or left open by all of them).
struct FactStanding {
  std::string fact;
  std::size_t acceptable = 0, held = 0, denied = 0, open = 0;
  std::string verdict;   // held, denied, open, turns on the ratios, or no acceptable doctrine
  std::string reason;
};

inline std::vector<FactStanding> fact_standings(const Problem& p, const std::vector<Assessment>& found, double tolerance = 0.5) {
  std::vector<FactStanding> out;
  double best = INFINITY;
  for (const auto& a : found) best = std::min(best, a.score);
  for (std::size_t b = 0; b < p.bases.size(); b++) {
    FactStanding s;
    s.fact = p.bases[b].fact;
    for (const auto& a : found) {
      if (a.score > best + tolerance + 1e-12 || b >= a.forced.size()) continue;
      s.acceptable++;
      if (a.forced[b].empty()) s.open++;
      else if (a.forced[b] == p.bases[b].base.sides[0]) s.held++;
      else s.denied++;
    }
    s.verdict = s.acceptable == 0 ? "no acceptable doctrine"
                : s.held == s.acceptable ? "held"
                : s.denied == s.acceptable ? "denied"
                : s.open == s.acceptable ? "open"
                : "turns on the ratios";
    s.reason = "the precedents hold it under " + std::to_string(s.held) + ", deny it under " + std::to_string(s.denied) + " and leave it open under " +
               std::to_string(s.open) + " of the " + std::to_string(s.acceptable) + " acceptable doctrines";
    out.push_back(s);
  }
  return out;
}

// ---- The test on what was held back ------------------------------------------------------------------------------------------------

struct Predicted {
  std::string id;
  std::string verdict;                 // predicted, contradicted, open
  std::string reason;
};

struct Confirmation {
  heuresis::search::Ticket ticket = 0;
  std::string key, text;
  std::string outcome;                 // upheld, contradicted, nothing held back
  std::vector<Predicted> cases;
  std::string registered, written;     // the seals of its two entries in the journal
};

// What a doctrine says of the held-back decisions (read only here, after the test is registered).
inline std::vector<Predicted> predict(const Problem& p, const Doctrine& d) {
  std::vector<Predicted> out;
  for (const auto& dc : p.decided) {
    if (!dc.held_back) continue;
    const bool follows = ai::entailment(decided_case(p, dc), aa::literal(dc.question))(d.readings).holds();
    out.push_back({dc.id, follows == dc.held ? "predicted" : "contradicted",
                   std::string("under its readings ") + dc.question + (follows ? " follows" : " does not follow") + "; the court held it " + (dc.held ? "did" : "did not")});
  }
  for (std::size_t b = 0; b < p.bases.size(); b++) {
    const auto& cb = p.bases[b];
    const precedent::Base read = detail::as_read(cb, b < d.ratios.size() ? d.ratios[b] : std::vector<Factors>{});
    for (const auto& c : cb.base.cases) {
      if (!cb.held_back.count(c.name)) continue;
      const auto k = precedent::constraint(read, c.facts);
      const std::string v = k.forced.empty() ? "open" : k.forced == c.outcome ? "predicted" : "contradicted";
      out.push_back({cb.fact + "/" + c.name, v,
                     k.forced.empty() ? "its ratios force neither side; the court decided for " + c.outcome
                                      : "its ratios force " + k.forced + "; the court decided for " + c.outcome});
    }
  }
  return out;
}

struct Confirmer {
  archeion::fixity::Journal journal;
  std::vector<Confirmation> done;

  bool tested(const std::string& k) const {
    for (const auto& c : done)
      if (c.key == k) return true;
    return false;
  }

  // Tests a doctrine once on the held-back decisions: registered (sealed) before they are read, the outcome written after.
  std::optional<Confirmation> confirm(const Problem& p, heuresis::search::Ticket ticket, const Doctrine& d, const std::string& when, const std::string& who) {
    const std::string k = key(p, d);
    if (tested(k)) return std::nullopt;
    std::vector<std::string> names;
    for (const auto& dc : p.decided)
      if (dc.held_back) names.push_back(dc.id);
    for (const auto& b : p.bases)
      for (const auto& c : b.base.cases)
        if (b.held_back.count(c.name)) names.push_back(b.fact + "/" + c.name);
    if (names.empty()) return std::nullopt;
    Confirmation c;
    c.ticket = ticket, c.key = k, c.text = write(p, d);
    std::string list;
    for (const auto& n : names) list += (list.empty() ? "" : ", ") + n;
    const std::string what = "doctrine #" + std::to_string(heuresis::search::serial_of(ticket));
    c.registered = journal.append({when, who, "registered a test", what, "on " + list + " (not yet read)"}).seal;
    c.cases = predict(p, d);
    bool contradicted = false;
    std::string said;
    for (const auto& x : c.cases) {
      contradicted = contradicted || x.verdict == "contradicted";
      said += (said.empty() ? "" : "; ") + x.id + " " + x.verdict;
    }
    c.outcome = contradicted ? "contradicted" : "upheld";
    c.written = journal.append({when, who, "wrote a test's outcome", what, c.outcome + ": " + said}).seal;
    done.push_back(c);
    return c;
  }
};

}  // namespace law::doctrine
