// Schemes: a body of law stated clause by clause.
//
// A scheme is what a work loads to apply a body of law. It holds the instruments, each a tree of provisions with their words as
// the official text has them; the rules, exceptions, open terms and counts-as links (norms.hpp), each citing the provisions it
// states; the vocabulary of facts and consequences the rules use, each with a gloss; and the courts and holdings that read its
// terms (authority.hpp). No limit, ranking or reading in the domain is any jurisdiction's until a scheme states it, and a scheme
// states it here, provision by provision.
//
// Nothing in a scheme is checked against the world: the words of a provision are as the caller fetched them from the official
// text, and whether the rules render those words rightly is a lawyer's judgment. What is checked is that the scheme is whole and
// says only what it declares:
// - every provision has an id distinct within the scheme, and its parent, where it names one, exists;
// - every rule, exception and counts-as link is cited to at least one provision, and every provision cited exists;
// - a quotation given with a citation occurs in the words of a provision it cites, up to runs of whitespace (tekmerion's
//   verify_passage), so that what a rule says its clause says can be read there;
// - every fact a rule or exception asks for is declared in the vocabulary as a fact, or is the consequence of another rule, or is
//   a reading of a declared term; every consequence is declared as a consequence;
// - every open term has at least two readings, all distinct; every counts-as link names a declared term, one of its readings and a
//   declared fact;
// - every holding names a declared court, term and reading; its passage, where the text of its judgment is given, occurs in it.
// The faults are listed, each with where it is; a scheme with faults is still read, so that it can be mended.

#pragma once

#include <law/authority/authority.hpp>
#include <law/norms/norms.hpp>
#include <tekmerion/recognition/recognition.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace law::schemes {

struct Provision {
  std::string id;      // unique in the scheme, e.g. "gdpr/art-46/2/c"
  std::string label;   // as a lawyer cites it, e.g. "Article 46(2)(c)"
  std::string text;    // its words, from the official text
  std::string parent;  // the provision it belongs to (empty: the instrument itself)
};

struct Instrument {
  std::string id, title, citation, url;
  std::string kind;    // in words: a treaty, a regulation, a decision, guidelines, …
  std::string source;  // the sources::Source it is, among the norms' sources (empty: none)
  std::string as_of;   // the date of the text used, YYYY-MM-DD
  std::vector<Provision> provisions;
};

enum class Use { fact, consequence };

struct Term {
  std::string id, gloss;
  Use use = Use::fact;
};

struct Citation {
  std::string of;                       // a rule, an exception or a counts-as link, by id
  std::vector<std::string> provisions;  // the provisions it states
  std::string quote;                    // words of one of them it rests on, if given
};

struct Scheme {
  std::string id, title, as_of;
  std::vector<Instrument> instruments;
  norms::Norms norms;
  std::vector<Citation> citations;
  std::vector<Term> vocabulary;
  std::vector<authority::Court> courts;
  std::vector<authority::Holding> holdings;
  std::vector<std::pair<std::string, std::string>> canon_order;  // among the parties' canons: (weaker, stronger)
};

struct Fault {
  std::string where, what;
};

// The provision with this id, or null.
inline const Provision* provision(const Scheme& s, const std::string& id) {
  for (const auto& i : s.instruments)
    for (const auto& p : i.provisions)
      if (p.id == id) return &p;
  return nullptr;
}

// The instrument a provision belongs to, or null.
inline const Instrument* instrument_of(const Scheme& s, const std::string& provision_id) {
  for (const auto& i : s.instruments)
    for (const auto& p : i.provisions)
      if (p.id == provision_id) return &i;
  return nullptr;
}

// The provisions a rule, exception or counts-as link is cited to.
inline std::vector<std::string> cited(const Scheme& s, const std::string& of) {
  std::vector<std::string> out;
  for (const auto& c : s.citations)
    if (c.of == of) out.insert(out.end(), c.provisions.begin(), c.provisions.end());
  return out;
}

namespace detail {

inline std::string atom(const std::string& l) { return l.rfind("¬", 0) == 0 ? l.substr(std::string("¬").size()) : l; }

// "[term=reading]" split, or false.
inline bool reading(const std::string& l, std::string& term, std::string& value) {
  if (l.size() < 4 || l.front() != '[' || l.back() != ']') return false;
  const auto eq = l.find('=');
  if (eq == std::string::npos) return false;
  term = l.substr(1, eq - 1), value = l.substr(eq + 1, l.size() - eq - 2);
  return true;
}

}  // namespace detail

// Every fault of the scheme. `judgments` gives the text of a holding's judgment by the holding's id, where it is had.
inline std::vector<Fault> check(const Scheme& s, const std::map<std::string, std::string>& judgments = {}) {
  std::vector<Fault> out;
  const auto fault = [&](const std::string& where, const std::string& what) { out.push_back({where, what}); };
  // Provisions.
  std::set<std::string> ids;
  for (const auto& i : s.instruments)
    for (const auto& p : i.provisions)
      if (!ids.insert(p.id).second) fault(i.id + "/" + p.id, "the provision's id is used twice");
  for (const auto& i : s.instruments)
    for (const auto& p : i.provisions)
      if (!p.parent.empty() && !ids.count(p.parent)) fault(p.id, "its parent " + p.parent + " is not a provision of the scheme");
  // Vocabulary, terms and consequences.
  std::map<std::string, Use> vocabulary;
  for (const auto& t : s.vocabulary)
    if (!vocabulary.emplace(t.id, t.use).second) fault("vocabulary/" + t.id, "declared twice");
  std::map<std::string, std::set<std::string>> terms;
  for (const auto& t : s.norms.terms) {
    std::set<std::string> rs(t.readings.begin(), t.readings.end());
    if (t.readings.size() < 2) fault("term/" + t.name, "an open term has at least two readings");
    if (rs.size() != t.readings.size()) fault("term/" + t.name, "two of its readings are the same");
    terms[t.name] = rs;
  }
  std::set<std::string> consequences;
  for (const auto& r : s.norms.rules) consequences.insert(detail::atom(r.consequence));
  for (const auto& k : s.norms.counts) consequences.insert(detail::atom(k.as));
  const auto asked = [&](const std::string& where, const std::string& element) {
    std::string term, value;
    if (detail::reading(element, term, value)) {
      if (!terms.count(term)) fault(where, "it asks for a reading of " + term + ", which is no open term");
      else if (!terms[term].count(value)) fault(where, "it asks for " + element + ", a reading " + term + " does not admit");
      return;
    }
    const std::string a = detail::atom(element);
    if (consequences.count(a)) return;
    const auto v = vocabulary.find(a);
    if (v == vocabulary.end()) fault(where, "it asks for " + a + ", which is neither declared nor given by a rule");
  };
  for (const auto& r : s.norms.rules) {
    for (const auto& e : r.elements) asked("rule/" + r.id, e);
    const auto v = vocabulary.find(detail::atom(r.consequence));
    if (v == vocabulary.end()) fault("rule/" + r.id, "its consequence " + detail::atom(r.consequence) + " is not declared");
    else if (v->second != Use::consequence) fault("rule/" + r.id, "its consequence " + v->first + " is declared as a fact, not a consequence");
  }
  std::set<std::string> rule_ids;
  for (const auto& r : s.norms.rules) rule_ids.insert(r.id);
  for (const auto& e : s.norms.exceptions) {
    if (!rule_ids.count(e.rule)) fault("exception/" + e.id, "it is an exception to " + e.rule + ", which is no rule of the scheme");
    for (const auto& x : e.elements) asked("exception/" + e.id, x);
  }
  for (const auto& k : s.norms.counts) {
    if (!terms.count(k.term)) fault("counts/" + k.id, "it names " + k.term + ", which is no open term");
    else if (!terms[k.term].count(k.reading)) fault("counts/" + k.id, "it names the reading " + k.reading + ", which " + k.term + " does not admit");
    asked("counts/" + k.id, k.fact);
    if (!vocabulary.count(detail::atom(k.as))) fault("counts/" + k.id, "what it counts as, " + k.as + ", is not declared");
  }
  // Citations.
  std::set<std::string> citable = rule_ids;
  for (const auto& e : s.norms.exceptions) citable.insert(e.id);
  for (const auto& k : s.norms.counts) citable.insert(k.id);
  std::set<std::string> with_citation;
  for (const auto& c : s.citations) {
    if (!citable.count(c.of)) fault("citation/" + c.of, "it cites for " + c.of + ", which is no rule, exception or counts-as link");
    if (c.provisions.empty()) fault("citation/" + c.of, "it cites no provision");
    bool quoted = c.quote.empty();
    for (const auto& p : c.provisions) {
      const auto* q = provision(s, p);
      if (!q) {
        fault("citation/" + c.of, "it cites " + p + ", which is no provision of the scheme");
        continue;
      }
      if (!quoted && !q->text.empty() && tekmerion::recognition::verify_passage(q->text, c.quote).holds()) quoted = true;
    }
    if (!quoted) fault("citation/" + c.of, "its quotation is not found in the words of the provisions it cites");
    with_citation.insert(c.of);
  }
  for (const auto& id : citable)
    if (!with_citation.count(id)) fault(id, "it is cited to no provision");
  // Holdings.
  std::set<std::string> courts;
  for (const auto& c : s.courts) courts.insert(c.id);
  for (const auto& h : s.holdings) {
    if (!courts.count(h.court)) fault("holding/" + h.id, "it names the court " + h.court + ", which is not declared");
    if (!terms.count(h.term)) fault("holding/" + h.id, "it reads " + h.term + ", which is no open term");
    else if (!terms[h.term].count(h.reading)) fault("holding/" + h.id, "it reads " + h.term + " as " + h.reading + ", which the term does not admit");
    const auto j = judgments.find(h.id);
    if (j != judgments.end() && !h.passage.empty() && !tekmerion::recognition::verify_passage(j->second, h.passage).holds())
      fault("holding/" + h.id, "its passage is not found in the text of the judgment");
  }
  return out;
}

}  // namespace law::schemes
