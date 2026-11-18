// Law for any work's engine (ADR 0121): one composition of the law domain with apodeixis, tekmerion and archeion, answering
// requests in JSON. A request holds everything it concerns (the scheme, the matter, the documents); nothing is read from a file
// or kept between requests.
//
//   scheme     {"scheme", "judgments"?: {holding id: text}}   every fault of a scheme (law::schemes::check; a holding's passage is
//              looked for in its judgment's text where that is given), and what it holds
//   evaluate   {"scheme", "matter", "judgments"?}   the matter read through the scheme:
//              - the evidence: the matter's chart with its exhibits offered (each authenticated by archeion, an unauthentic one
//                undercut by the archive's finding) and its recognised candidates as items; each fact's standing in the chart,
//                and whether it meets the matter's standard of proof;
//              - the facts as they enter the case: as declared, or proved where the evidence meets the standard and the matter
//                takes standings from the evidence;
//              - the courts' holdings weighed for the forum (binding, persuasive, left out, with reasons), and the parties'
//                interpretive arguments;
//              - for each issue: clear, arguable or untenable over the admissible interpretations, with its label, an argument
//                for it where there is one, the rules and provisions it rests on, and the plan of proof (the ways to establish
//                and to resist it over the facts alleged, what is needed in every way, and the facts the record does not mention)
//   recognise  {"candidates", "documents", "rule"?}   each proposition's standing from its candidates, passages verified
//
// The scheme, noetarium.scheme/1:
//   {id, title, as_of, instruments: [{id, title, citation, url, kind, source, as_of, provisions: [{id, label, text, parent}]}],
//    sources: [{id, title, kind, from, until, rank, parties}], maxims?, special?: [[a, b]],
//    rules: [{id, source, elements, consequence, cites, quote}], exceptions: [{id, rule, elements, cites, quote}],
//    terms: [{name, gloss, readings: [name | {name, gloss}]}], counts: [{id, fact, term, reading, as, cites, quote}],
//    vocabulary: [{id, gloss, use: fact | consequence}], courts: [{id, name, order, level, instruments}],
//    holdings: [{id, court, name, citation, date, instrument, term, reading, passage}], canon_order: [[weaker, stronger]],
//    canons?: below | unranked | above}
// The matter, noetarium.matter/1:
//   {id, title, date, forum: {id, name, bound_by}, standard: {name, probability?, argument: none | not_overruled | justified},
//    from_evidence?: true, issues: [{id, question, burden, standard}], facts: [{id, text, standing}],
//    chart?: {sources, propositions, items, links, preferences}, exhibits?: [{id, reports, source, kind, text, chain: {digest,
//    custodian, since, transfers: [{when, from, to, digest, note}]}, observed, identity: {name: value}}],
//    candidates?: [{id, proposition, negated, source, document, passage, by, grade}], documents?: [{id, text}],
//    arguments?: [{id, term, reading, canon, text}], undercuts?: [[attacker, attacked]], canon_order?: [[weaker, stronger]],
//    rule?: {independent_b}}
//
// Only the open terms the issues turn on enter a case: a scheme may hold many more than the space of interpretations allows.
// The methods are the domain's and the foundations'; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <apodeixis/interpretation/interpretation.hpp>
#include <law/authority/authority.hpp>
#include <law/exhibits/exhibits.hpp>
#include <law/facts/facts.hpp>
#include <law/issues/issues.hpp>
#include <law/norms/norms.hpp>
#include <law/proof/proof.hpp>
#include <law/schemes/schemes.hpp>
#include <law/sources/sources.hpp>
#include <tekmerion/evidence/chart.hpp>
#include <tekmerion/recognition/recognition.hpp>

#include <algorithm>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::law {

namespace ai = apodeixis::interpretation;
namespace aa = apodeixis::argumentation;
namespace te = tekmerion::evidence;
namespace tr = tekmerion::recognition;
namespace lw = ::law;
using apodeixis::kernel::Verdict;

// ---- Small readers and writers -------------------------------------------------------------------------------------------------

inline std::vector<std::string> texts(const Json& j) {
  std::vector<std::string> out;
  if (j.is_array())
    for (std::size_t k = 0; k < j.size(); k++) out.push_back(j[k].text());
  return out;
}

inline Json strings(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& s : v) a.push(s);
  return a;
}

inline Json verdict_json(const Verdict& v) {
  Json evidence = Json::array();
  for (const auto& e : v.evidence) evidence.push(Json::object({{"role", e.role}, {"value", e.value}}));
  return Json::object({{"status", std::string(apodeixis::kernel::word(v.status))}, {"claim", v.claim}, {"reason", v.reason}, {"evidence", evidence}});
}

inline std::vector<std::pair<std::string, std::string>> pairs(const Json& j) {
  std::vector<std::pair<std::string, std::string>> out;
  for (std::size_t k = 0; k < j.size(); k++) out.push_back({j[k][0].text(), j[k][1].text()});
  return out;
}

inline lw::sources::Kind source_kind(const std::string& w) {
  using K = lw::sources::Kind;
  for (const K k : {K::treaty, K::custom, K::principle, K::decision, K::teaching, K::constitution, K::statute, K::regulation})
    if (lw::sources::word(k) == w) return k;
  if (w == "general principle" || w == "principle") return K::principle;
  if (w == "judicial decision") return K::decision;
  throw std::invalid_argument("no kind of source is called " + w);
}

inline lw::facts::Standing standing_of(const std::string& w) {
  using S = lw::facts::Standing;
  for (const S s : {S::alleged, S::admitted, S::noticed, S::presumed, S::deemed, S::proved, S::not_proved, S::rebutted, S::found, S::final_})
    if (lw::facts::word(s) == w) return s;
  if (w == "not_proved") return S::not_proved;
  if (w == "final_" || w == "final") return S::final_;
  throw std::invalid_argument("no standing of a fact is called " + w);
}

inline te::ItemKind item_kind(const std::string& w) {
  using K = te::ItemKind;
  for (const K k : {K::testimony, K::document, K::object, K::record, K::expert, K::absence, K::other})
    if (te::word(k) == w) return k;
  return K::other;
}

// ---- The scheme -----------------------------------------------------------------------------------------------------------------

struct Read {
  lw::schemes::Scheme scheme;
  std::map<std::string, std::string> term_gloss, reading_gloss;  // reading_gloss by "term=reading"
  lw::authority::Canons canons = lw::authority::Canons::unranked;
};

inline Read scheme_of(const Json& j) {
  Read out;
  auto& s = out.scheme;
  s.id = j["id"].text(), s.title = j["title"].text(), s.as_of = j["as_of"].text();
  for (std::size_t k = 0; k < j["instruments"].size(); k++) {
    const Json& x = j["instruments"][k];
    lw::schemes::Instrument i;
    i.id = x["id"].text(), i.title = x["title"].text(), i.citation = x["citation"].text(), i.url = x["url"].text(), i.kind = x["kind"].text();
    i.source = x["source"].text(), i.as_of = x["as_of"].text();
    for (std::size_t p = 0; p < x["provisions"].size(); p++) {
      const Json& y = x["provisions"][p];
      i.provisions.push_back({y["id"].text(), y["label"].text(), y["text"].text(), y["parent"].text()});
    }
    s.instruments.push_back(std::move(i));
  }
  for (std::size_t k = 0; k < j["sources"].size(); k++) {
    const Json& x = j["sources"][k];
    lw::sources::Source src;
    src.id = x["id"].text(), src.title = x["title"].text(), src.kind = source_kind(x["kind"].text("statute")), src.from = x["from"].text(), src.until = x["until"].text();
    src.rank = x["rank"].is_number() ? static_cast<int>(x["rank"].as_number()) : 0;
    for (const auto& p : texts(x["parties"])) src.parties.insert(p);
    s.norms.sources.push_back(std::move(src));
  }
  if (j.has("maxims")) {
    s.norms.maxims.clear();
    for (const auto& m : texts(j["maxims"]))
      s.norms.maxims.push_back(m == "superior" ? lw::sources::Maxim::superior : m == "specialis" ? lw::sources::Maxim::specialis : lw::sources::Maxim::posterior);
  }
  for (const auto& [a, b] : pairs(j["special"])) s.norms.special.insert({a, b});
  const auto cite = [&](const std::string& of, const Json& x) {
    if (x.has("cites")) s.citations.push_back({of, texts(x["cites"]), x["quote"].text()});
  };
  for (std::size_t k = 0; k < j["rules"].size(); k++) {
    const Json& x = j["rules"][k];
    s.norms.rules.push_back({x["id"].text(), x["source"].text(), texts(x["elements"]), x["consequence"].text()});
    cite(x["id"].text(), x);
  }
  for (std::size_t k = 0; k < j["exceptions"].size(); k++) {
    const Json& x = j["exceptions"][k];
    s.norms.exceptions.push_back({x["id"].text(), x["rule"].text(), texts(x["elements"])});
    cite(x["id"].text(), x);
  }
  for (std::size_t k = 0; k < j["terms"].size(); k++) {
    const Json& x = j["terms"][k];
    lw::norms::OpenTerm t;
    t.name = x["name"].text();
    out.term_gloss[t.name] = x["gloss"].text();
    for (std::size_t r = 0; r < x["readings"].size(); r++) {
      const Json& y = x["readings"][r];
      const std::string name = y.is_string() ? y.text() : y["name"].text();
      t.readings.push_back(name);
      if (y.is_object()) out.reading_gloss[t.name + "=" + name] = y["gloss"].text();
    }
    s.norms.terms.push_back(std::move(t));
  }
  for (std::size_t k = 0; k < j["counts"].size(); k++) {
    const Json& x = j["counts"][k];
    s.norms.counts.push_back({x["id"].text(), x["fact"].text(), x["term"].text(), x["reading"].text(), x["as"].text()});
    cite(x["id"].text(), x);
  }
  for (std::size_t k = 0; k < j["vocabulary"].size(); k++) {
    const Json& x = j["vocabulary"][k];
    s.vocabulary.push_back({x["id"].text(), x["gloss"].text(), x["use"].text() == "consequence" ? lw::schemes::Use::consequence : lw::schemes::Use::fact});
  }
  for (std::size_t k = 0; k < j["courts"].size(); k++) {
    const Json& x = j["courts"][k];
    lw::authority::Court c;
    c.id = x["id"].text(), c.name = x["name"].text(), c.order = x["order"].text();
    c.level = x["level"].is_number() ? static_cast<int>(x["level"].as_number()) : 0;
    for (const auto& i : texts(x["instruments"])) c.instruments.insert(i);
    s.courts.push_back(std::move(c));
  }
  for (std::size_t k = 0; k < j["holdings"].size(); k++) {
    const Json& x = j["holdings"][k];
    s.holdings.push_back({x["id"].text(), x["court"].text(), x["name"].text(), x["citation"].text(), x["date"].text(), x["instrument"].text(), x["term"].text(), x["reading"].text(), x["passage"].text()});
  }
  s.canon_order = pairs(j["canon_order"]);
  const std::string canons = j["canons"].text("unranked");
  out.canons = canons == "below" ? lw::authority::Canons::below : canons == "above" ? lw::authority::Canons::above : lw::authority::Canons::unranked;
  return out;
}

inline Json faults_json(const std::vector<lw::schemes::Fault>& fs) {
  Json a = Json::array();
  for (const auto& f : fs) a.push(Json::object({{"where", f.where}, {"what", f.what}}));
  return a;
}

// The texts of judgments, by the id of the holding each gives: {"holding id": "text"}.
inline std::map<std::string, std::string> judgments_of(const Json& j) {
  std::map<std::string, std::string> out;
  if (j.is_object())
    for (const auto& [id, text] : j.as_object()) out[id] = text.text();
  return out;
}

inline Json scheme(const Json& r) {
  const Read rd = scheme_of(r["scheme"]);
  const auto& s = rd.scheme;
  std::size_t provisions = 0;
  for (const auto& i : s.instruments) provisions += i.provisions.size();
  return Json::object({{"faults", faults_json(lw::schemes::check(s, judgments_of(r["judgments"])))},
                       {"counts", Json::object({{"instruments", static_cast<double>(s.instruments.size())}, {"provisions", static_cast<double>(provisions)},
                                                {"rules", static_cast<double>(s.norms.rules.size())}, {"exceptions", static_cast<double>(s.norms.exceptions.size())},
                                                {"terms", static_cast<double>(s.norms.terms.size())}, {"counts", static_cast<double>(s.norms.counts.size())},
                                                {"vocabulary", static_cast<double>(s.vocabulary.size())}, {"holdings", static_cast<double>(s.holdings.size())}})}});
}

// The open terms the questions turn on: those of counts-as links and readings reached backwards from them.
inline std::set<std::string> terms_reached(const lw::norms::Norms& n, const std::vector<std::string>& questions) {
  std::set<std::string> terms, seen;
  std::vector<std::string> stack;
  const auto atom = [](const std::string& l) { return l.rfind("¬", 0) == 0 ? l.substr(std::string("¬").size()) : l; };
  const auto push = [&](const std::string& e) {
    if (e.size() > 2 && e.front() == '[' && e.find('=') != std::string::npos) terms.insert(e.substr(1, e.find('=') - 1));
    else stack.push_back(atom(e));
  };
  for (const auto& q : questions) push(q);
  while (!stack.empty()) {
    const std::string a = stack.back();
    stack.pop_back();
    if (!seen.insert(a).second) continue;
    for (const auto& r : n.rules)
      if (atom(r.consequence) == a) {
        for (const auto& e : r.elements) push(e);
        for (const auto& x : n.exceptions)
          if (x.rule == r.id)
            for (const auto& e : x.elements) push(e);
      }
    for (const auto& k : n.counts)
      if (atom(k.as) == a) terms.insert(k.term), push(k.fact);
  }
  return terms;
}

// ---- Recognition --------------------------------------------------------------------------------------------------------------

// A candidate's status: "accepted" (or none, for one a person entered) counts; "proposed" (by a model, not yet reviewed) is read
// at grade C, whatever passage it cites, and enters no chart; "rejected" is left out.
inline std::vector<tr::Candidate> candidates_of(const Json& j) {
  std::vector<tr::Candidate> out;
  for (std::size_t k = 0; k < j.size(); k++) {
    const Json& x = j[k];
    const std::string status = x["status"].text("accepted");
    if (status == "rejected") continue;
    tr::Candidate c;
    c.id = x["id"].text(), c.proposition = x["proposition"].text(), c.negated = x["negated"].is_bool() && x["negated"].as_bool();
    c.source = x["source"].text(), c.document = x["document"].text(), c.passage = x["passage"].text(), c.by = x["by"].text();
    const std::string g = x["grade"].text("C");
    c.grade = status == "proposed" ? tr::Grade::C : g == "A" ? tr::Grade::A : g == "B" ? tr::Grade::B : tr::Grade::C;
    out.push_back(std::move(c));
  }
  return out;
}

inline std::vector<tr::Document> documents_of(const Json& j) {
  std::vector<tr::Document> out;
  for (std::size_t k = 0; k < j.size(); k++) out.push_back({j[k]["id"].text(), j[k]["text"].text()});
  return out;
}

inline Json recognised_json(const std::vector<tr::Recognised>& rs, const std::vector<tr::Candidate>& cs, const std::vector<tr::Document>& ds) {
  Json out = Json::array();
  for (const auto& x : rs)
    out.push(Json::object({{"proposition", x.proposition}, {"standing", std::string(tr::word(x.standing))}, {"for", strings(x.for_it)}, {"against", strings(x.against_it)},
                           {"sources_for", static_cast<double>(x.sources_for)}, {"sources_against", static_cast<double>(x.sources_against)},
                           {"downgraded", strings(x.downgraded)}, {"reason", x.reason}}));
  // Where each verified passage stands in its document, so that a page can mark it.
  Json places = Json::array();
  std::map<std::string, const tr::Document*> by;
  for (const auto& d : ds) by[d.id] = &d;
  for (const auto& c : cs) {
    if (c.grade != tr::Grade::B) continue;
    const auto d = by.find(c.document);
    if (d == by.end()) continue;
    const auto v = tr::verify_passage(d->second->text, c.passage);
    Json one = Json::object({{"candidate", c.id}, {"document", c.document}, {"verdict", verdict_json(v)}});
    for (const auto& e : v.evidence)
      if (e.role == "at") one.set("at", std::stod(e.value));
    places.push(one);
  }
  return Json::object({{"propositions", out}, {"passages", places}});
}

inline Json recognise(const Json& r) {
  const auto cs = candidates_of(r["candidates"]);
  const auto ds = documents_of(r["documents"]);
  tr::Rule rule;
  if (r["rule"]["independent_b"].is_number()) rule.independent_b = static_cast<std::size_t>(r["rule"]["independent_b"].as_number());
  return recognised_json(tr::recognise(cs, ds, rule), cs, ds);
}

// ---- The matter ------------------------------------------------------------------------------------------------------------------

inline lw::proof::Standard standard_of(const Json& j) {
  lw::proof::Standard s;
  s.name = j["name"].text("the matter's standard");
  if (j["probability"].is_number()) s.probability = j["probability"].as_number();
  const std::string a = j["argument"].text("justified");
  s.argument = a == "none" ? lw::proof::ArgumentTest::none : a == "not_overruled" || a == "not overruled" ? lw::proof::ArgumentTest::not_overruled : lw::proof::ArgumentTest::justified;
  return s;
}

inline te::Chart chart_of(const Json& j) {
  te::Chart c;
  for (std::size_t k = 0; k < j["sources"].size(); k++) {
    const Json& x = j["sources"][k];
    te::Source s;
    s.id = x["id"].text(), s.text = x["text"].text();
    if (x["sensitivity"].is_number()) s.sensitivity = x["sensitivity"].as_number();
    if (x["objectivity"].is_number()) s.objectivity = x["objectivity"].as_number();
    if (x["veracity"].is_number()) s.veracity = x["veracity"].as_number();
    c.sources.push_back(std::move(s));
  }
  for (std::size_t k = 0; k < j["propositions"].size(); k++) c.propositions.push_back({j["propositions"][k]["id"].text(), j["propositions"][k]["text"].text(), 0});
  for (std::size_t k = 0; k < j["items"].size(); k++) {
    const Json& x = j["items"][k];
    c.items.push_back({x["id"].text(), x["source"].text(), x["reports"].text(), x["text"].text(), item_kind(x["kind"].text("document")), 0});
  }
  for (std::size_t k = 0; k < j["links"].size(); k++) {
    const Json& x = j["links"][k];
    te::Link l;
    l.from = x["from"].text(), l.to = x["to"].text(), l.generalisation = x["generalisation"].text(), l.text = x["text"].text();
    const std::string sense = x["sense"].text("supports");
    l.sense = sense == "opposes" ? te::Sense::opposes : sense == "undercuts" ? te::Sense::undercuts : te::Sense::supports;
    c.links.push_back(std::move(l));
  }
  c.preferences = pairs(j["preferences"]);
  return c;
}

inline std::vector<lw::exhibits::Exhibit> exhibits_of(const Json& j) {
  std::vector<lw::exhibits::Exhibit> out;
  for (std::size_t k = 0; k < j.size(); k++) {
    const Json& x = j[k];
    lw::exhibits::Exhibit e;
    e.id = x["id"].text(), e.reports = x["reports"].text(), e.source = x["source"].text(), e.kind = item_kind(x["kind"].text("document")), e.text = x["text"].text();
    const Json& c = x["chain"];
    e.chain.digest = c["digest"].text(), e.chain.custodian = c["custodian"].text(), e.chain.since = c["since"].text();
    for (std::size_t t = 0; t < c["transfers"].size(); t++) {
      const Json& y = c["transfers"][t];
      e.chain.transfers.push_back({y["when"].text(), y["from"].text(), y["to"].text(), y["digest"].text(), y["note"].text()});
    }
    e.observed = x["observed"].text(e.chain.digest);
    if (x["identity"].is_object())
      for (const auto& [name, value] : x["identity"].as_object()) e.identity.emplace_back(name, value.text());
    out.push_back(std::move(e));
  }
  return out;
}

// The evidence of a matter: its chart with recognised candidates as items and exhibits offered, and each fact's standing in it.
struct Evidence {
  te::Chart chart;
  std::vector<std::string> faults;
  std::map<std::string, aa::Standing> standing;
  std::vector<std::pair<std::string, Verdict>> authenticity;
  std::vector<tr::Recognised> recognised;
  bool read = false;
};

inline Evidence evidence_of(const Json& m) {
  Evidence ev;
  te::Chart c = chart_of(m["chart"]);
  // The matter's facts are propositions of the chart.
  for (std::size_t k = 0; k < m["facts"].size(); k++) {
    const std::string id = m["facts"][k]["id"].text();
    if (c.proposition(id) < 0) c.propositions.push_back({id, m["facts"][k]["text"].text(), 0});
  }
  // Recognised candidates: those of grade A, and of grade B whose passage is found, enter as items of their sources.
  const auto cs = candidates_of(m["candidates"]);
  const auto ds = documents_of(m["documents"]);
  tr::Rule rule;
  if (m["rule"]["independent_b"].is_number()) rule.independent_b = static_cast<std::size_t>(m["rule"]["independent_b"].as_number());
  ev.recognised = tr::recognise(cs, ds, rule);
  std::map<std::string, const tr::Document*> docs;
  for (const auto& d : ds) docs[d.id] = &d;
  for (const auto& x : cs) {
    if (x.grade == tr::Grade::C) continue;
    if (x.grade == tr::Grade::B) {
      const auto d = docs.find(x.document);
      if (d == docs.end() || !tr::verify_passage(d->second->text, x.passage).holds()) continue;
    }
    if (c.proposition(x.proposition) < 0) continue;
    if (!c.source(x.source)) {
      te::Source s;
      s.id = x.source, s.text = x.source;
      c.sources.push_back(s);
    }
    if (!x.negated) {
      c.items.push_back({x.id, x.source, x.proposition, x.passage, x.grade == tr::Grade::A ? te::ItemKind::record : te::ItemKind::document, 0});
    } else {
      const std::string denial = "denied:" + x.id;
      c.propositions.push_back({denial, x.source + " says that " + x.proposition + " does not hold", 0});
      c.items.push_back({x.id, x.source, denial, x.passage, x.grade == tr::Grade::A ? te::ItemKind::record : te::ItemKind::document, 0});
      te::Link l;
      l.from = denial, l.to = x.proposition, l.generalisation = "what " + x.id + " says", l.sense = te::Sense::opposes, l.text = "a source that denies it speaks against it";
      c.links.push_back(l);
    }
  }
  // Whoever produced an exhibit is a source of the chart, declared or not; an exhibit offered for no fact of the matter is left out.
  std::vector<lw::exhibits::Exhibit> exhibits;
  for (auto& e : exhibits_of(m["exhibits"])) {
    if (c.proposition(e.reports) < 0) continue;
    if (!c.source(e.source)) {
      te::Source s;
      s.id = e.source, s.text = e.source;
      c.sources.push_back(s);
    }
    exhibits.push_back(std::move(e));
  }
  const auto offered = lw::exhibits::offer(c, exhibits);
  ev.chart = offered.chart;
  ev.authenticity = offered.authenticity;
  // A matter has many facts and no one ultimate proposition: the faults about the ultimate one are not faults here.
  for (const auto& f : te::faults(ev.chart))
    if (f.what.find("ultimate") == std::string::npos && f.what.find("leads to") == std::string::npos) ev.faults.push_back(f.what);
  if (!ev.faults.empty()) return ev;
  const auto evaluation = aa::evaluate(te::to_theory(ev.chart));
  if (evaluation.refused) {
    ev.faults.push_back(evaluation.reason);
    return ev;
  }
  for (const auto& p : ev.chart.propositions) ev.standing[p.id] = aa::standing(evaluation, p.id);
  ev.read = true;
  return ev;
}

// The case a record makes of the matter's questions: the scheme's norms with only the open terms the questions turn on, the
// parties' interpretive arguments, and the courts' holdings weighed for the forum, with the canons' order and the undercuts.
struct Built {
  lw::norms::Norms norms;
  ai::Case c;
  Json argument_faults = Json::array();
  lw::authority::Authority authority;
};

inline Built built(const Read& rd, const Json& m, const lw::facts::Record& record, const std::vector<std::string>& questions) {
  const auto& s = rd.scheme;
  Built out;
  lw::norms::Norms& n = out.norms;
  n = s.norms;
  const auto reached = terms_reached(n, questions);
  std::vector<lw::norms::OpenTerm> kept;
  for (const auto& t : n.terms)
    if (reached.count(t.name)) kept.push_back(t);
  n.terms = kept;
  ai::Case& c = out.c;
  c = lw::norms::subsume(record, n, questions);
  Json& argument_faults = out.argument_faults;
  std::vector<std::string> canon_kinds;
  std::map<std::string, std::size_t> argument_index;
  for (std::size_t k = 0; k < m["arguments"].size(); k++) {
    const Json& x = m["arguments"][k];
    const long point = ai::point_index(c, x["term"].text());
    long reading = -1;
    if (point >= 0)
      for (std::size_t q = 0; q < c.points[static_cast<std::size_t>(point)].readings.size(); q++)
        if (c.points[static_cast<std::size_t>(point)].readings[q] == x["reading"].text()) reading = static_cast<long>(q);
    if (point < 0 || reading < 0) {
      argument_faults.push(Json::object({{"argument", x["id"].text()}, {"what", point < 0 ? "it reads " + x["term"].text() + ", which no issue turns on" : "it argues for " + x["reading"].text() + ", which " + x["term"].text() + " does not admit"}}));
      continue;
    }
    const std::string canon = x["canon"].text("text");
    if (std::find(canon_kinds.begin(), canon_kinds.end(), canon) == canon_kinds.end()) canon_kinds.push_back(canon);
    argument_index[x["id"].text()] = c.arguments.size();
    c.arguments.push_back({x["id"].text(), point, reading, canon, x["text"].text(), 0});
  }
  const Json& fo = m["forum"];
  lw::authority::Forum forum{fo["id"].text(), fo["name"].text("the forum"), {}};
  for (const auto& o : texts(fo["bound_by"])) forum.bound_by.insert(o);
  lw::authority::Options options;
  options.date = m["date"].text();
  options.canons = rd.canons;
  out.authority = lw::authority::weigh(c.points, s.courts, s.holdings, forum, canon_kinds, options);
  const auto& authority = out.authority;
  for (const auto& h : authority.arguments) argument_index[h.id] = c.arguments.size() + (&h - authority.arguments.data());
  lw::authority::add_to(c, authority);
  c.canon_order.insert(c.canon_order.end(), s.canon_order.begin(), s.canon_order.end());
  for (const auto& p : pairs(m["canon_order"])) c.canon_order.push_back(p);
  for (const auto& [a, b2] : pairs(m["undercuts"])) {
    const auto x = argument_index.find(a), y = argument_index.find(b2);
    if (x == argument_index.end() || y == argument_index.end()) argument_faults.push(Json::object({{"argument", a}, {"what", "an undercut names an argument that is not in the case"}}));
    else c.undercuts.push_back({x->second, y->second});
  }
  return out;
}

inline Json way_json(const lw::issues::Way& w) { return Json::object({{"prove", strings(w.prove)}, {"disprove", strings(w.disprove)}, {"readings", strings(w.readings)}}); }

inline Json evaluate(const Json& r) {
  const Read rd = scheme_of(r["scheme"]);
  const auto& s = rd.scheme;
  const Json& m = r["matter"];
  Json out = Json::object({{"scheme_faults", faults_json(lw::schemes::check(s, judgments_of(r["judgments"])))}});
  // The evidence, and the facts as they enter the case.
  const Evidence ev = evidence_of(m);
  const auto standard = standard_of(m["standard"]);
  const bool from_evidence = !m["from_evidence"].is_bool() || m["from_evidence"].as_bool();
  lw::facts::Record record;
  Json facts = Json::array(), matter_faults = Json::array();
  std::set<std::string> vocabulary;
  for (const auto& t : s.vocabulary) vocabulary.insert(t.id);
  for (std::size_t k = 0; k < m["facts"].size(); k++) {
    const Json& x = m["facts"][k];
    if (!vocabulary.count(x["id"].text()))
      matter_faults.push(Json::object({{"fact", x["id"].text()}, {"what", "it is not in the scheme's vocabulary, so no rule can ask for it"}}));
    lw::facts::Fact f = lw::facts::fact(x["id"].text(), x["text"].text());
    f.standing = standing_of(x["standing"].text("alleged"));
    Json one = Json::object({{"id", f.id}, {"declared", std::string(lw::facts::word(f.standing))}});
    if (ev.read && ev.standing.count(f.id)) {
      lw::proof::Assessment a;
      a.standing = ev.standing.at(f.id);
      const auto v = lw::proof::meets(standard, a);
      one.set("evidence", std::string(aa::word(*a.standing)));
      one.set("meets", verdict_json(v));
      if (from_evidence && f.standing == lw::facts::Standing::alleged && v.holds()) f.standing = lw::facts::Standing::proved;
    }
    one.set("used", std::string(lw::facts::word(f.standing)));
    facts.push(one);
    record.facts.push_back(std::move(f));
  }
  out.set("facts", facts);
  out.set("matter_faults", matter_faults);
  Json authenticity = Json::array();
  for (const auto& [id, v] : ev.authenticity) authenticity.push(Json::object({{"exhibit", id}, {"verdict", verdict_json(v)}}));
  out.set("evidence", Json::object({{"faults", strings(ev.faults)}, {"read", ev.read}, {"authenticity", authenticity},
                                    {"recognised", recognised_json(ev.recognised, candidates_of(m["candidates"]), documents_of(m["documents"]))["propositions"]}}));
  // The norms, with only the open terms the issues turn on.
  std::vector<lw::issues::Issue> issues;
  std::vector<std::string> questions;
  for (std::size_t k = 0; k < m["issues"].size(); k++) {
    const Json& x = m["issues"][k];
    issues.push_back({x["id"].text(), x["question"].text(), x["burden"].text(), x["standard"].text()});
    questions.push_back(x["question"].text());
  }
  Built b = built(rd, m, record, questions);
  lw::norms::Norms& n = b.norms;
  ai::Case& c = b.c;
  const auto& authority = b.authority;
  Json argument_faults = b.argument_faults;
  Json assessed = Json::array();
  for (const auto& a : authority.assessed) {
    const lw::authority::Holding* h = nullptr;
    for (const auto& x : s.holdings)
      if (x.id == a.holding) h = &x;
    assessed.push(Json::object({{"holding", a.holding}, {"force", std::string(lw::authority::word(a.force))}, {"reason", a.reason}, {"kind", a.kind},
                                {"name", h ? h->name : ""}, {"citation", h ? h->citation : ""}, {"term", h ? h->term : ""}, {"reading", h ? h->reading : ""}}));
  }
  out.set("authority", Json::object({{"assessed", assessed}, {"faults", strings(authority.faults)}}));
  out.set("argument_faults", argument_faults);
  Json points = Json::array();
  for (const auto& p : c.points) {
    Json readings = Json::array();
    for (const auto& rr : p.readings) readings.push(Json::object({{"name", rr}, {"gloss", rd.reading_gloss.count(p.name + "=" + rr) ? rd.reading_gloss.at(p.name + "=" + rr) : ""}}));
    points.push(Json::object({{"term", p.name}, {"gloss", rd.term_gloss.count(p.name) ? rd.term_gloss.at(p.name) : ""}, {"readings", readings}}));
  }
  out.set("terms", points);
  // Each issue answered, with its label, an argument, what it rests on and the plan of proof.
  const std::size_t space = ai::space(c.points);
  if (space > ai::space_limit) {
    out.set("refused", "the issues turn on " + std::to_string(c.points.size()) + " open terms, " + std::to_string(space) + " interpretations, above " + std::to_string(ai::space_limit));
    return out;
  }
  const auto evaluation = ai::evaluate(c);
  out.set("admissibility", Json::object({{"fixed", strings(evaluation.admissibility.fixed)}, {"set_aside", strings(evaluation.admissibility.set_aside)}}));
  Json answers = Json::array();
  for (std::size_t k = 0; k < issues.size(); k++) {
    const auto& a = evaluation.answers[k];
    Json label = Json::array();
    for (const auto& e : a.label) label.push(ai::print(c.points, e));
    // What it rests on: under an admissible interpretation where it follows (or, if none, the first admissible one), the rules,
    // exceptions and counts-as links whose every element stands, with the provisions they are cited to.
    std::size_t at = a.derivable.size();
    for (std::size_t x = 0; x < a.derivable.size() && at == a.derivable.size(); x++)
      if (evaluation.admissibility.admissible[x] && a.derivable[x]) at = x;
    for (std::size_t x = 0; x < a.derivable.size() && at == a.derivable.size(); x++)
      if (evaluation.admissibility.admissible[x]) at = x;
    Json rests = Json::array();
    if (at < a.derivable.size()) {
      const auto i = ai::decode(c.points, at);
      const auto t = aa::evaluate(ai::under(c, i));
      const auto stands = [&](const std::string& l) { return !t.refused && aa::standing(t, l) == aa::Standing::justified; };
      const auto provisions = [&](const std::string& of) {
        Json ps = Json::array();
        for (const auto& id : lw::schemes::cited(s, of)) {
          const auto* p = lw::schemes::provision(s, id);
          const auto* in = lw::schemes::instrument_of(s, id);
          ps.push(Json::object({{"id", id}, {"label", p ? p->label : ""}, {"instrument", in ? in->title : ""}}));
        }
        return ps;
      };
      for (const auto& rule : n.rules) {
        bool all = true;
        for (const auto& e : rule.elements) all = all && stands(e);
        if (all) rests.push(Json::object({{"kind", "rule"}, {"id", rule.id}, {"consequence", rule.consequence}, {"holds", stands(rule.consequence)}, {"provisions", provisions(rule.id)}}));
      }
      for (const auto& x : n.exceptions) {
        bool all = true;
        for (const auto& e : x.elements) all = all && stands(e);
        if (all) rests.push(Json::object({{"kind", "exception"}, {"id", x.id}, {"consequence", "not " + x.rule}, {"holds", true}, {"provisions", provisions(x.id)}}));
      }
      for (const auto& x : n.counts) {
        const long point = ai::point_index(c, x.term);
        if (point < 0 || !stands(x.fact)) continue;
        const auto& term = c.points[static_cast<std::size_t>(point)];
        if (term.readings[i[static_cast<std::size_t>(point)]] == x.reading)
          rests.push(Json::object({{"kind", "counts"}, {"id", x.id}, {"consequence", x.as}, {"holds", stands(x.as)}, {"provisions", provisions(x.id)}}));
      }
      rests = Json::object({{"under", ai::print(c.points, i)}, {"steps", rests}});
    }
    const auto p = lw::issues::plan(c, record, n, issues[k]);
    Json establish = Json::array(), resist = Json::array();
    for (const auto& w : p.establish) establish.push(way_json(w));
    for (const auto& w : p.resist) resist.push(way_json(w));
    answers.push(Json::object({{"issue", issues[k].id}, {"question", a.query}, {"burden", issues[k].burden}, {"class", std::string(ai::word(a.classification))}, {"verdict", verdict_json(a.verdict)},
                               {"label", label}, {"admissible", static_cast<double>(a.admissible)}, {"admissible_derivable", static_cast<double>(a.admissible_derivable)},
                               {"witness", a.witness}, {"rests_on", rests},
                               {"plan", Json::object({{"open", strings(p.open)}, {"given", strings(p.given)}, {"decided", strings(p.decided)}, {"absent", strings(p.absent)}, {"now", std::string(ai::word(p.now))},
                                                      {"establish", establish}, {"resist", resist}, {"must_prove", strings(p.must_prove)}, {"must_disprove", strings(p.must_disprove)},
                                                      {"verdict", verdict_json(p.verdict)}})}}));
  }
  out.set("issues", answers);
  return out;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "scheme") return scheme(r);
  if (op == "evaluate") return evaluate(r);
  if (op == "recognise") return recognise(r);
  throw std::invalid_argument("science::law: no operation is called " + op + " (there is: scheme, evaluate, recognise)");
}

}  // namespace science::law
