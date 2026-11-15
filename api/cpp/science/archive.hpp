// Archives for any work's engine (ADR 0120): one composition of archeion, answering requests in JSON. A request holds what it
// concerns; the only files read are those whose paths a request names (a package's payload, a bag's files).
//
//   arrange    {"levels"?, "units": [{id, parent, level, code, title, creator, from, to, order, holdings: [digest]}]}
//              every fault of the arrangement (respect des fonds, original order, levels, dates, codes), and for each unit its
//              reference code, its fonds, what it is described as where it states nothing (creator, dates: ISAD(G)'s
//              non-repetition) and its children in their order
//   custody    {"chains": [{holding, digest, custodian, since, transfers: [{when, from, to, digest, note}], observed?,
//              attributes?: {name: value}}], "accounted"?: [[before, after]]}
//              each chain followed handover by handover: unbroken or where it breaks, who holds it now, and its authenticity
//              (identity, integrity, custody) where an observed digest is given
//   access     {"levels"?, "units", "rules": [{name, unit, class, effect, roles, until}], "open"?, "requests": [{who, roles,
//              unit, class, when}]}   each request decided by the most specific rule, with the rule
//   journal    {"entries": [{when, who, what, about, note, seal}], "head"?}   whether each entry seals to the one before
//   package    {"payload": {"path within data/": file path}, "info": [[name, value]], "date"}   a BagIt bag's tag files
//              (bagit.txt, bag-info.txt and both manifests), the payload's files read and digested here
//   validate   {"files": {"path within the bag": file path}}   whether the bag is complete and valid, and what is missing,
//              unlisted or altered
//   codebook   {"codebook"}   every fault of a codebook (DDI-Codebook's description of a study, its variables and files)
//   conform    {"codebook", "file", "table": {columns, rows}, "observed"?, "weight"?}   the table held to its declarations cell
//              by cell, with the frequencies of each variable
//   versions   {"earlier", "later"}   what changed between two releases, and whether it breaks comparison
//   harmonise  {"target": variable, "sources": [{study, variable, to: [[from, to]], dropped}], "codebooks"}   what each source
//              loses, merges and never reaches
//   common     {"a": {study, variable}, "b": {study, variable}, "codebooks", "corresponds": [[a code, b code]], "name"?}
//              the finest common target of two variables: the pushout of their codes along the correspondence
//
// A codebook is {"study": {id, title, producer, version, released, doi, citation, universe, sampling, mode, from, until,
// countries, access, licence}, "variables": [{name, label, question, universe, unit, derivation, notes, measure, categories:
// [{code, label, missing}], least, most, weight}], "files": [{name, digest, variables, cases}]}.
//
// The methods are archeion's; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <archeion/access/access.hpp>
#include <archeion/arrangement/arrangement.hpp>
#include <archeion/custody/custody.hpp>
#include <archeion/datasets/codebook.hpp>
#include <archeion/datasets/harmonisation.hpp>
#include <archeion/datasets/versions.hpp>
#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>
#include <archeion/packages/bagit.hpp>

#include <cmath>
#include <fstream>
#include <iterator>
#include <limits>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace science::archive {

namespace ar = archeion::arrangement;
namespace ds = archeion::datasets;
using apodeixis::kernel::Verdict;

inline double number(const Json& j, double fallback = std::numeric_limits<double>::quiet_NaN()) { return j.is_number() ? j.as_number() : fallback; }

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

inline std::string read_file(const std::string& path) {
  std::ifstream in(path, std::ios::binary);
  if (!in) throw std::invalid_argument("the file " + path + " could not be read");
  return std::string(std::istreambuf_iterator<char>(in), {});
}

// ---- Arrangement ----------------------------------------------------------------------------------------------------------------

inline ar::Archive archive_of(const Json& r) {
  ar::Archive a;
  a.levels = r.has("levels") ? texts(r["levels"]) : ar::isad_levels();
  const Json& units = r["units"];
  for (std::size_t k = 0; k < units.size(); k++) {
    const Json& u = units[k];
    ar::Unit x;
    x.id = u["id"].text(), x.parent = u["parent"].text(), x.level = u["level"].text(), x.code = u["code"].text(), x.title = u["title"].text();
    x.creator = u["creator"].text(), x.from = u["from"].text(), x.to = u["to"].text();
    x.order = static_cast<std::size_t>(number(u["order"], static_cast<double>(k)));
    x.holdings = texts(u["holdings"]);
    a.units.push_back(std::move(x));
  }
  return a;
}

inline Json arrange(const Json& r) {
  const ar::Archive a = archive_of(r);
  Json faults = Json::array();
  const auto fs = ar::faults(a);
  for (const auto& f : fs) faults.push(Json::object({{"unit", f.unit}, {"what", f.what}}));
  Json units = Json::array();
  // Described only where the arrangement holds: a cycle or a missing parent leaves references undefined.
  bool sound = true;
  for (const auto& f : fs) sound = sound && f.what.find("cycle") == std::string::npos;
  for (const auto& u : a.units) {
    Json x = Json::object({{"id", u.id}, {"children", strings(ar::children(a, u.id))}});
    if (sound) {
      x.set("reference", ar::reference(a, u.id));
      x.set("fonds", ar::fonds_of(a, u.id));
      x.set("creator", ar::described(a, u.id, "creator"));
      x.set("from", ar::described(a, u.id, "from"));
      x.set("to", ar::described(a, u.id, "to"));
    }
    units.push(x);
  }
  return Json::object({{"faults", faults}, {"units", units}, {"levels", strings(a.levels)}, {"sound", fs.empty()}});
}

// ---- Custody ------------------------------------------------------------------------------------------------------------------------

inline Json custody(const Json& r) {
  std::set<std::pair<std::string, std::string>> accounted;
  for (std::size_t k = 0; k < r["accounted"].size(); k++) accounted.insert({r["accounted"][k][0].text(), r["accounted"][k][1].text()});
  Json out = Json::array();
  const Json& chains = r["chains"];
  for (std::size_t k = 0; k < chains.size(); k++) {
    const Json& c = chains[k];
    archeion::custody::Chain chain;
    chain.digest = c["digest"].text(), chain.custodian = c["custodian"].text(), chain.since = c["since"].text();
    for (std::size_t t = 0; t < c["transfers"].size(); t++) {
      const Json& x = c["transfers"][t];
      chain.transfers.push_back({x["when"].text(), x["from"].text(), x["to"].text(), x["digest"].text(), x["note"].text()});
    }
    const auto v = archeion::custody::verify(chain, accounted);
    Json breaks = Json::array();
    for (const auto& b : v.breaks) breaks.push(Json::object({{"at", static_cast<double>(b.at)}, {"what", b.what}}));
    Json one = Json::object({{"holding", c["holding"].text()}, {"verdict", verdict_json(v.verdict)}, {"breaks", breaks}, {"custodian", v.custodian}, {"digest", v.digest}});
    if (c.has("observed")) {
      std::vector<std::pair<std::string, std::string>> attributes;
      if (c["attributes"].is_object())
        for (const auto& [name, value] : c["attributes"].as_object()) attributes.emplace_back(name, value.text());
      one.set("authentic", verdict_json(archeion::custody::authentic(attributes, chain, c["observed"].text(), accounted)));
    }
    out.push(one);
  }
  return Json::object({{"chains", out}});
}

// ---- Access -------------------------------------------------------------------------------------------------------------------------

inline Json access(const Json& r) {
  const ar::Archive a = archive_of(r);
  archeion::access::Policy p;
  p.open_by_default = r["open"].is_bool() && r["open"].as_bool();
  for (std::size_t k = 0; k < r["rules"].size(); k++) {
    const Json& x = r["rules"][k];
    archeion::access::Rule rule;
    rule.name = x["name"].text(), rule.unit = x["unit"].text(), rule.record_class = x["class"].text(), rule.until = x["until"].text();
    const std::string e = x["effect"].text("closed");
    rule.effect = e == "open" ? archeion::access::Effect::open : e == "restricted" ? archeion::access::Effect::restricted : archeion::access::Effect::closed;
    for (const auto& role : texts(x["roles"])) rule.roles.insert(role);
    p.rules.push_back(std::move(rule));
  }
  Json out = Json::array();
  for (std::size_t k = 0; k < r["requests"].size(); k++) {
    const Json& x = r["requests"][k];
    archeion::access::Request q;
    q.who = x["who"].text(), q.unit = x["unit"].text(), q.record_class = x["class"].text(), q.when = x["when"].text();
    for (const auto& role : texts(x["roles"])) q.roles.insert(role);
    out.push(Json::object({{"who", q.who}, {"unit", q.unit}, {"verdict", verdict_json(archeion::access::decide(p, a, q))}}));
  }
  return Json::object({{"decisions", out}});
}

// ---- The journal ----------------------------------------------------------------------------------------------------------------------

inline Json journal(const Json& r) {
  archeion::fixity::Journal j;
  for (std::size_t k = 0; k < r["entries"].size(); k++) {
    const Json& e = r["entries"][k];
    j.entries.push_back({e["when"].text(), e["who"].text(), e["what"].text(), e["about"].text(), e["note"].text(), e["seal"].text()});
  }
  return Json::object({{"verdict", verdict_json(archeion::fixity::verify(j, r["head"].text()))}, {"entries", static_cast<double>(j.entries.size())}, {"genesis", archeion::fixity::Journal::genesis()}});
}

// ---- Packages -----------------------------------------------------------------------------------------------------------------------

inline Json package(const Json& r) {
  archeion::packages::Bag payload;
  if (r["payload"].is_object())
    for (const auto& [path, file] : r["payload"].as_object()) {
      if (!archeion::packages::safe(path)) throw std::invalid_argument("a payload path may not leave data/: " + path);
      payload[path] = read_file(file.text());
    }
  std::vector<std::pair<std::string, std::string>> info;
  for (std::size_t k = 0; k < r["info"].size(); k++) info.emplace_back(r["info"][k][0].text(), r["info"][k][1].text());
  const auto bag = archeion::packages::make(payload, info, r["date"].text());
  Json tags = Json::object();
  for (const auto& [path, bytes] : bag)
    if (path.rfind("data/", 0) != 0) tags.set(path, bytes);
  Json digests = Json::object();
  for (const auto& [path, bytes] : payload) digests.set(path, archeion::fixity::sha256_hex(bytes));
  return Json::object({{"tags", tags}, {"payload", digests}, {"verdict", verdict_json(archeion::packages::validate(bag).verdict)}});
}

inline Json validate(const Json& r) {
  archeion::packages::Bag bag;
  if (r["files"].is_object())
    for (const auto& [path, file] : r["files"].as_object()) bag[path] = read_file(file.text());
  const auto v = archeion::packages::validate(bag);
  return Json::object({{"complete", v.complete}, {"valid", v.valid}, {"missing", strings(v.missing)}, {"unlisted", strings(v.unlisted)}, {"altered", strings(v.altered)},
                       {"faults", strings(v.faults)}, {"verdict", verdict_json(v.verdict)}});
}

// ---- Datasets -------------------------------------------------------------------------------------------------------------------------

inline ds::Measure measure_of(const std::string& w) {
  if (w == "ordinal") return ds::Measure::ordinal;
  if (w == "discrete") return ds::Measure::discrete;
  if (w == "continuous") return ds::Measure::continuous;
  if (w == "text") return ds::Measure::text;
  if (w == "date") return ds::Measure::date;
  return ds::Measure::nominal;
}

inline ds::Variable variable_of(const Json& v) {
  ds::Variable x;
  x.name = v["name"].text(), x.label = v["label"].text(), x.question = v["question"].text(), x.universe = v["universe"].text();
  x.unit = v["unit"].text(), x.derivation = v["derivation"].text(), x.notes = v["notes"].text();
  x.measure = measure_of(v["measure"].text("nominal"));
  for (std::size_t k = 0; k < v["categories"].size(); k++) {
    const Json& c = v["categories"][k];
    x.categories.push_back({c["code"].text(), c["label"].text(), c["missing"].is_bool() && c["missing"].as_bool()});
  }
  x.least = number(v["least"]), x.most = number(v["most"]);
  x.weight = v["weight"].is_bool() && v["weight"].as_bool();
  return x;
}

inline Json variable_json(const ds::Variable& x) {
  Json cats = Json::array();
  for (const auto& c : x.categories) cats.push(Json::object({{"code", c.code}, {"label", c.label}, {"missing", c.missing}}));
  Json v = Json::object({{"name", x.name}, {"label", x.label}, {"measure", ds::word(x.measure)}, {"categories", cats}});
  if (!x.question.empty()) v.set("question", x.question);
  return v;
}

inline ds::Codebook codebook_of(const Json& c) {
  ds::Codebook b;
  const Json& s = c["study"];
  b.study.id = s["id"].text(), b.study.title = s["title"].text(), b.study.producer = s["producer"].text(), b.study.version = s["version"].text();
  b.study.released = s["released"].text(), b.study.doi = s["doi"].text(), b.study.citation = s["citation"].text(), b.study.universe = s["universe"].text();
  b.study.sampling = s["sampling"].text(), b.study.mode = s["mode"].text(), b.study.from = s["from"].text(), b.study.until = s["until"].text();
  b.study.countries = texts(s["countries"]), b.study.access = s["access"].text(), b.study.licence = s["licence"].text();
  for (std::size_t k = 0; k < c["variables"].size(); k++) b.variables.push_back(variable_of(c["variables"][k]));
  for (std::size_t k = 0; k < c["files"].size(); k++) {
    const Json& f = c["files"][k];
    b.files.push_back({f["name"].text(), f["digest"].text(), texts(f["variables"]), static_cast<std::size_t>(number(f["cases"], 0))});
  }
  return b;
}

inline Json codebook(const Json& r) {
  const ds::Codebook b = codebook_of(r["codebook"]);
  return Json::object({{"faults", strings(ds::faults(b))}, {"verdict", verdict_json(ds::well_formed(b))}, {"variables", static_cast<double>(b.variables.size())}});
}

inline Json conform(const Json& r) {
  const ds::Codebook b = codebook_of(r["codebook"]);
  ds::Table t;
  t.columns = texts(r["table"]["columns"]);
  for (std::size_t k = 0; k < r["table"]["rows"].size(); k++) t.rows.push_back(texts(r["table"]["rows"][k]));
  const auto c = ds::conform(b, r["file"].text(), t, r["observed"].text(), r["weight"].text());
  Json freqs = Json::array();
  for (const auto& f : c.frequencies) {
    Json valid = Json::object(), missing = Json::object();
    for (const auto& [code, n] : f.valid) valid.set(code, n);
    for (const auto& [code, n] : f.missing) missing.set(code, n);
    Json one = Json::object({{"variable", f.variable}, {"valid", valid}, {"missing", missing}, {"system_missing", f.system_missing}, {"cases", f.cases}});
    if (std::isfinite(f.mean)) one.set("mean", f.mean);
    if (std::isfinite(f.sd)) one.set("sd", f.sd);
    freqs.push(one);
  }
  return Json::object({{"faults", strings(c.faults)}, {"cells", static_cast<double>(c.cells)}, {"frequencies", freqs}, {"verdict", verdict_json(c.verdict)}});
}

inline Json versions(const Json& r) {
  const auto c = ds::compare(codebook_of(r["earlier"]), codebook_of(r["later"]));
  Json changes = Json::array();
  for (const auto& x : c.changes) changes.push(Json::object({{"variable", x.variable}, {"what", x.what}, {"breaking", x.breaking}}));
  return Json::object({{"changes", changes}, {"compatible", c.compatible}, {"verdict", verdict_json(c.verdict)}});
}

inline ds::Recode recode_of(const Json& x) {
  ds::Recode rc;
  rc.study = x["study"].text(), rc.variable = x["variable"].text();
  for (std::size_t k = 0; k < x["to"].size(); k++) rc.to.emplace_back(x["to"][k][0].text(), x["to"][k][1].text());
  rc.dropped = texts(x["dropped"]);
  return rc;
}

inline Json recode_json(const ds::Recode& rc) {
  Json to = Json::array();
  for (const auto& [a, b] : rc.to) to.push(Json::array({Json(a), Json(b)}));
  return Json::object({{"study", rc.study}, {"variable", rc.variable}, {"to", to}, {"dropped", strings(rc.dropped)}});
}

inline std::vector<ds::Codebook> books_of(const Json& j) {
  std::vector<ds::Codebook> out;
  for (std::size_t k = 0; k < j.size(); k++) out.push_back(codebook_of(j[k]));
  return out;
}

inline Json harmonise(const Json& r) {
  ds::Harmonisation h;
  h.target = variable_of(r["target"]);
  for (std::size_t k = 0; k < r["sources"].size(); k++) h.sources.push_back(recode_of(r["sources"][k]));
  const auto a = ds::check(h, books_of(r["codebooks"]));
  Json merged = Json::object(), gaps = Json::object();
  for (const auto& [src, codes] : a.merged) merged.set(src, strings(codes));
  for (const auto& [src, codes] : a.gaps) gaps.set(src, strings(codes));
  return Json::object({{"faults", strings(a.faults)}, {"merged", merged}, {"gaps", gaps}, {"verdict", verdict_json(a.verdict)}});
}

inline Json common(const Json& r) {
  const auto books = books_of(r["codebooks"]);
  const auto find = [&](const Json& which) -> const ds::Variable& {
    for (const auto& b : books)
      if (b.study.id == which["study"].text())
        if (const ds::Variable* v = b.variable(which["variable"].text())) return *v;
    throw std::invalid_argument("no codebook declares " + which["study"].text() + "/" + which["variable"].text());
  };
  std::vector<std::pair<std::string, std::string>> corr;
  for (std::size_t k = 0; k < r["corresponds"].size(); k++) corr.emplace_back(r["corresponds"][k][0].text(), r["corresponds"][k][1].text());
  const auto c = ds::common_target(r["a"]["study"].text(), find(r["a"]), r["b"]["study"].text(), find(r["b"]), corr, r["name"].text("harmonised"));
  Json classes = Json::array();
  for (const auto& cl : c.classes) classes.push(strings(cl));
  return Json::object({{"classes", classes}, {"from_a", recode_json(c.from_a)}, {"from_b", recode_json(c.from_b)}, {"target", variable_json(c.target)}});
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "arrange") return arrange(r);
  if (op == "custody") return custody(r);
  if (op == "access") return access(r);
  if (op == "journal") return journal(r);
  if (op == "package") return package(r);
  if (op == "validate") return validate(r);
  if (op == "codebook") return codebook(r);
  if (op == "conform") return conform(r);
  if (op == "versions") return versions(r);
  if (op == "harmonise") return harmonise(r);
  if (op == "common") return common(r);
  throw std::invalid_argument("no operation " + op + " (arrange, custody, access, journal, package, validate, codebook, conform, versions, harmonise, common)");
}

}  // namespace science::archive
