// Inquiry for any work's engine (ADR 0120): the analyses an archive of the social sciences and the humanities is studied with,
// case-oriented (zetesis) and quantitative (tyche, nomos, peira), answering requests in JSON. A request holds its data; nothing
// is read from a file and nothing is kept between requests.
//
//   calibrate   {"raw": [x], "out", "crossover", "in"}   memberships by Ragin's direct method
//   qca         {"cases", "conditions", "members": [[m]], "outcome": [m], "outcome_name"?, "frequency"?, "consistency"?, "pri"?,
//               "expectations"?: [+1 | −1 | 0], "negate"?}   the truth table, the conservative, parsimonious and (with
//               expectations) intermediate solutions with every minimal model, and the necessity of each condition
//   coding      {"codes": [{id, label, parent}], "documents": [{id, case, text}], "segments": [{id, document, begin, end,
//               quote}], "codings": [{segment, code, coder}], "coders"?, "order"?}   faults, the agreement of coders code by
//               code, the case-by-code and co-occurrence matrices, and saturation over the documents in order
//   concepts    {"objects", "attributes", "members": [[m]]}   the concept lattice of cases and attributes (a case has an
//               attribute above 0.5) with its covers, and the stem base of implications
//   mill        {"cases", "conditions", "values": [[0|1]], "outcome": [0|1]}   Mill's methods of agreement, difference and the
//               joint method
//   tests       {"clues": [{name, certainty, uniqueness}]}   Van Evera's tests by P(E | H) and P(E | ¬H)
//   weigh       {"hypotheses": [{name, prior}], "clues": [{name, likelihood: [P(E | Hᵢ)], found}]}   rival hypotheses weighed
//               clue by clue, in decibans
//   reliability {"items", "scores": [[x]]}   Cronbach's α, item analysis and split-half reliability
//   irt         {"items", "responses": [[1 | 0 | −1]], "model"?: "rasch" | "2pl"}   item response theory by marginal maximum
//               likelihood, with each person's score
//   factors     {"variables", "data": [[x]], "factors", "rotate"?}   factor analysis by maximum likelihood of the correlations,
//               varimax if asked
//   classes     {"items", "responses": [[1 | 0 | −1]], "classes", "seed"?}   latent class analysis
//   mixed       {"y", "x": [[...]], "names", "group": [label], "slopes"?: [column], "reml"?}   a linear mixed model with random
//               intercepts (and slopes)
//   agreement   {"codes": [[code index or −1]] by coder and unit, "categories", "metric"?}   Krippendorff's α (and Cohen's κ for two)
//   anonymity   {"columns", "rows": [[text]], "quasi": [columns], "k"?, "sensitive"?}   the classes of a table's
//               quasi-identifiers, its k, the rows at risk and, with a sensitive column, its diversity
//
// The methods are the foundations'; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <calx/random/stream.hpp>
#include <morphe/combinatorics/concepts.hpp>
#include <nomos/measurement/agreement.hpp>
#include <nomos/psychometrics/psychometrics.hpp>
#include <peira/disclosure/anonymity.hpp>
#include <tyche/latent/latent.hpp>
#include <tyche/mixed/mixed.hpp>
#include <zetesis/cases/cases.hpp>
#include <zetesis/coding/coding.hpp>
#include <zetesis/concepts/concepts.hpp>
#include <zetesis/qca/qca.hpp>
#include <zetesis/sets/sets.hpp>
#include <zetesis/tracing/tracing.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::inquiry {

inline double num(const Json& j, double fallback = std::numeric_limits<double>::quiet_NaN()) { return j.is_number() ? j.as_number() : fallback; }

inline std::vector<double> numbers(const Json& j) {
  std::vector<double> v;
  for (std::size_t k = 0; k < j.size(); k++) v.push_back(num(j[k]));
  return v;
}

inline std::vector<std::vector<double>> rows_of(const Json& j) {
  std::vector<std::vector<double>> v;
  for (std::size_t k = 0; k < j.size(); k++) v.push_back(numbers(j[k]));
  return v;
}

inline std::vector<std::vector<int>> ints_of(const Json& j) {
  std::vector<std::vector<int>> v;
  for (std::size_t k = 0; k < j.size(); k++) {
    std::vector<int> r;
    for (std::size_t i = 0; i < j[k].size(); i++) r.push_back(static_cast<int>(std::lround(num(j[k][i], -1))));
    v.push_back(std::move(r));
  }
  return v;
}

inline std::vector<std::string> texts(const Json& j) {
  std::vector<std::string> v;
  for (std::size_t k = 0; k < j.size(); k++) v.push_back(j[k].text());
  return v;
}

inline Json arr(const std::vector<double>& v) {
  Json a = Json::array();
  for (const double x : v) a.push(std::isfinite(x) ? Json(x) : Json());
  return a;
}

inline Json arr(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.push(x);
  return a;
}

inline Json matrix(const std::vector<std::vector<double>>& m) {
  Json a = Json::array();
  for (const auto& r : m) a.push(arr(r));
  return a;
}

inline Json idx(const std::vector<std::size_t>& v) {
  Json a = Json::array();
  for (const auto x : v) a.push(static_cast<double>(x));
  return a;
}

inline Json finite(double x) { return std::isfinite(x) ? Json(x) : Json(); }

// ---- zetesis ------------------------------------------------------------------------------------------------------------------------

inline Json calibrate(const Json& r) {
  const zetesis::sets::Anchors a{num(r["out"]), num(r["crossover"]), num(r["in"])};
  return Json::object({{"members", arr(zetesis::sets::calibrate(numbers(r["raw"]), a))}});
}

inline Json relation_json(const zetesis::sets::Relation& x) {
  Json o = Json::object({{"consistency", finite(x.consistency)}, {"coverage", finite(x.coverage)}});
  if (std::isfinite(x.pri)) o.set("pri", x.pri);
  if (std::isfinite(x.relevance)) o.set("relevance", x.relevance);
  if (!x.note.empty()) o.set("note", x.note);
  return o;
}

inline Json qca(const Json& r) {
  namespace q = zetesis::qca;
  q::Data d;
  d.cases = texts(r["cases"]), d.conditions = texts(r["conditions"]), d.members = rows_of(r["members"]), d.outcome = numbers(r["outcome"]);
  d.outcome_name = r["outcome_name"].text("Y");
  if (r["negate"].is_bool() && r["negate"].as_bool()) d = q::negate_outcome(d);
  q::Thresholds t;
  t.frequency = static_cast<std::size_t>(num(r["frequency"], 1));
  t.consistency = num(r["consistency"], 0.8);
  t.pri = num(r["pri"], 0);
  const q::TruthTable tt = q::truth_table(d, t);
  Json rows = Json::array();
  for (const auto& row : tt.rows) {
    Json cases = Json::array();
    for (const auto i : row.cases) cases.push(d.cases[i]);
    Json deviant = Json::array();
    for (const auto i : row.deviant) deviant.push(d.cases[i]);
    rows.push(Json::object({{"configuration", q::configuration(row.corner, d.conditions)}, {"corner", static_cast<double>(row.corner)}, {"cases", cases},
                            {"consistency", finite(row.consistency)}, {"pri", finite(row.pri)},
                            {"code", std::string(row.code == q::Code::positive ? "1" : row.code == q::Code::negative ? "0" : "remainder")}, {"deviant", deviant}}));
  }
  Json unplaced = Json::array();
  for (const auto i : tt.unplaced) unplaced.push(d.cases[i]);
  const auto solution_json = [&](const q::Solution& s) {
    Json models = Json::array();
    for (const auto& m : s.models) {
      Json terms = Json::array();
      for (const auto& term : m.terms) {
        Json cases = Json::array();
        for (const auto i : term.cases) cases.push(d.cases[i]);
        terms.push(Json::object({{"term", term.text}, {"consistency", finite(term.consistency)}, {"raw_coverage", finite(term.raw_coverage)},
                                 {"unique_coverage", finite(term.unique_coverage)}, {"cases", cases}}));
      }
      Json assumptions = Json::array();
      for (const auto a : m.assumptions) assumptions.push(q::configuration(a, d.conditions));
      models.push(Json::object({{"model", m.text}, {"consistency", finite(m.consistency)}, {"coverage", finite(m.coverage)}, {"terms", terms}, {"assumptions", assumptions}}));
    }
    return Json::object({{"kind", s.kind}, {"models", models}, {"counterfactuals", static_cast<double>(s.counterfactuals.size())}});
  };
  Json solutions = Json::array();
  q::Options o;
  solutions.push(solution_json(q::minimise(d, tt, o)));
  o.remainders = q::Remainders::included;
  solutions.push(solution_json(q::minimise(d, tt, o)));
  if (r["expectations"].is_array()) {
    o.remainders = q::Remainders::easy;
    for (std::size_t k = 0; k < r["expectations"].size(); k++) o.expectations.push_back(static_cast<int>(num(r["expectations"][k], 0)));
    solutions.push(solution_json(q::minimise(d, tt, o)));
  }
  Json necessity = Json::array();
  for (const auto& n : q::necessity(d)) necessity.push(Json::object({{"condition", (n.negated ? "~" : "") + n.condition}, {"relation", relation_json(n.relation)}}));
  return Json::object({{"rows", rows}, {"remainders", static_cast<double>(tt.remainders.size())}, {"unplaced", unplaced}, {"solutions", solutions}, {"necessity", necessity},
                       {"outcome", d.outcome_name}});
}

inline Json coding(const Json& r) {
  namespace c = zetesis::coding;
  c::Corpus corpus;
  for (std::size_t k = 0; k < r["codes"].size(); k++) {
    const Json& x = r["codes"][k];
    c::Code code;
    code.id = x["id"].text(), code.label = x["label"].text(x["id"].text()), code.parent = x["parent"].text(), code.definition = x["definition"].text();
    corpus.book.codes.push_back(code);
  }
  for (std::size_t k = 0; k < r["documents"].size(); k++) {
    const Json& x = r["documents"][k];
    corpus.documents.push_back({x["id"].text(), x["case"].text(x["id"].text()), x["text"].text()});
  }
  for (std::size_t k = 0; k < r["segments"].size(); k++) {
    const Json& x = r["segments"][k];
    corpus.segments.push_back({x["id"].text(), x["document"].text(), static_cast<std::size_t>(num(x["begin"], 0)), static_cast<std::size_t>(num(x["end"], 0)), x["quote"].text()});
  }
  for (std::size_t k = 0; k < r["codings"].size(); k++) {
    const Json& x = r["codings"][k];
    corpus.codings.push_back({x["segment"].text(), x["code"].text(), x["coder"].text()});
  }
  Json faults = Json::array();
  for (const auto& f : c::faults(corpus.book)) faults.push(Json::object({{"where", f.where}, {"what", f.what}}));
  for (const auto& f : c::check(corpus)) faults.push(Json::object({{"where", f.where}, {"what", f.what}}));
  Json out = Json::object({{"faults", faults}});
  std::vector<std::string> coders = texts(r["coders"]);
  if (coders.empty()) {
    std::set<std::string> seen;
    for (const auto& x : corpus.codings) seen.insert(x.coder);
    coders.assign(seen.begin(), seen.end());
  }
  if (coders.size() >= 2) {
    Json agreement = Json::array();
    for (const auto& a : c::agreement(corpus, coders))
      agreement.push(Json::object({{"code", a.code}, {"units", static_cast<double>(a.units)}, {"given", static_cast<double>(a.given)}, {"alpha", finite(a.alpha)}, {"kappa", finite(a.kappa)}, {"note", a.note}}));
    out.set("agreement", agreement);
  }
  out.set("coders", arr(coders));
  const auto matrix_json = [](const c::Matrix& m) { return Json::object({{"rows", arr(m.rows)}, {"columns", arr(m.columns)}, {"values", matrix(m.values)}}); };
  out.set("cases", matrix_json(c::case_by_code(corpus)));
  out.set("co_occurrence", matrix_json(c::co_occurrence(corpus)));
  std::vector<std::string> order = texts(r["order"]);
  if (order.empty())
    for (const auto& d : corpus.documents) order.push_back(d.id);
  if (order.size() >= 3) {
    const std::size_t base = std::min<std::size_t>(4, order.size() - 1);
    const auto s = c::saturation(corpus, order, base, 1);
    Json sat = Json::object({{"order", arr(s.order)}, {"new_codes", idx(s.new_codes)}, {"base", static_cast<double>(s.base_codes)}, {"runs", arr(s.runs)}});
    if (s.reached_after) sat.set("reached_after", static_cast<double>(*s.reached_after));
    out.set("saturation", sat);
  }
  return out;
}

inline Json concepts(const Json& r) {
  namespace mc = morphe::combinatorics;
  const std::vector<std::string> objects = texts(r["objects"]), attributes = texts(r["attributes"]);
  const mc::Context ctx = zetesis::concepts::context(rows_of(r["members"]));
  if (ctx.objects != objects.size() || ctx.attributes != attributes.size()) throw std::invalid_argument("concepts: a row for each object, a membership for each attribute");
  const auto L = mc::lattice(ctx);
  Json cs = Json::array();
  for (const auto& c : L.concepts) {
    Json extent = Json::array(), intent = Json::array();
    for (const auto i : c.extent) extent.push(objects[i]);
    for (const auto j : c.intent) intent.push(attributes[j]);
    cs.push(Json::object({{"extent", extent}, {"intent", intent}}));
  }
  Json covers = Json::array();
  for (const auto& [lo, hi] : L.covers) covers.push(Json::array({Json(static_cast<double>(lo)), Json(static_cast<double>(hi))}));
  Json implications = Json::array();
  for (const auto& imp : mc::stem_base(ctx)) {
    Json p = Json::array(), q = Json::array();
    for (const auto j : imp.premise) p.push(attributes[j]);
    for (const auto j : imp.conclusion)
      if (std::find(imp.premise.begin(), imp.premise.end(), j) == imp.premise.end()) q.push(attributes[j]);
    implications.push(Json::object({{"premise", p}, {"conclusion", q}}));
  }
  return Json::object({{"concepts", cs}, {"covers", covers}, {"top", static_cast<double>(L.top)}, {"bottom", static_cast<double>(L.bottom)}, {"implications", implications}});
}

inline Json mill(const Json& r) {
  namespace zc = zetesis::cases;
  zc::Table t;
  t.cases = texts(r["cases"]), t.conditions = texts(r["conditions"]), t.values = ints_of(r["values"]);
  for (std::size_t k = 0; k < r["outcome"].size(); k++) t.outcome.push_back(static_cast<int>(num(r["outcome"][k], 0)));
  const auto json = [&](const zc::Mill& m) {
    Json cands = Json::array(), grounds = Json::array(), pairs = Json::array();
    for (const auto j : m.candidates) cands.push(t.conditions[j]);
    for (const auto i : m.grounds) grounds.push(t.cases[i]);
    for (const auto& [a, b] : m.pairs) pairs.push(Json::array({Json(t.cases[a]), Json(t.cases[b])}));
    return Json::object({{"method", m.method}, {"candidates", cands}, {"grounds", grounds}, {"pairs", pairs}, {"note", m.note}});
  };
  return Json::object({{"methods", Json::array({json(zc::agreement(t)), json(zc::difference(t)), json(zc::joint(t))})}});
}

inline Json tests(const Json& r) {
  Json out = Json::array();
  for (std::size_t k = 0; k < r["clues"].size(); k++) {
    const Json& c = r["clues"][k];
    const double certainty = num(c["certainty"]), uniqueness = num(c["uniqueness"]);
    const auto x = zetesis::tracing::classify(certainty, 1 - uniqueness);
    out.push(Json::object({{"clue", c["name"].text()}, {"test", std::string(zetesis::tracing::name(x.test))}, {"certainty", x.certainty}, {"uniqueness", x.uniqueness},
                           {"ratio_found", finite(x.ratio_found)}, {"ratio_missing", finite(x.ratio_missing)}}));
  }
  return Json::object({{"tests", out}});
}

inline Json weigh(const Json& r) {
  namespace zt = zetesis::tracing;
  std::vector<zt::Hypothesis> hs;
  for (std::size_t k = 0; k < r["hypotheses"].size(); k++) hs.push_back({r["hypotheses"][k]["name"].text(), num(r["hypotheses"][k]["prior"])});
  std::vector<zt::Clue> clues;
  for (std::size_t k = 0; k < r["clues"].size(); k++) {
    const Json& c = r["clues"][k];
    zt::Clue clue;
    clue.name = c["name"].text();
    clue.likelihood = numbers(c["likelihood"]);
    clue.found = !(c["found"].is_bool() && !c["found"].as_bool());
    clues.push_back(std::move(clue));
  }
  const auto a = zt::weigh(hs, clues);
  Json steps = Json::array();
  for (const auto& s : a.steps) steps.push(Json::object({{"clue", s.clue}, {"posterior", arr(s.posterior)}, {"decibans", matrix(s.decibans)}}));
  return Json::object({{"hypotheses", arr(a.hypotheses)}, {"prior", arr(a.prior)}, {"posterior", arr(a.posterior)}, {"steps", steps}, {"decibans", matrix(a.decibans)},
                       {"leading", a.hypotheses.empty() ? Json() : Json(a.hypotheses[a.leading])}, {"assumption", a.assumption}});
}

// ---- tyche, nomos, peira ----------------------------------------------------------------------------------------------------------

inline Json reliability(const Json& r) {
  namespace p = nomos::psychometrics;
  const auto scores = rows_of(r["scores"]);
  const auto items = p::item_analysis(scores);
  return Json::object({{"items", r["items"]}, {"alpha", finite(p::cronbach_alpha(scores))}, {"rest_correlation", arr(items.rest_correlation)},
                       {"alpha_without", arr(items.alpha_without)}, {"split_half", finite(p::split_half(scores))}, {"cases", static_cast<double>(scores.size())}});
}

inline Json irt(const Json& r) {
  namespace p = nomos::psychometrics;
  const auto responses = ints_of(r["responses"]);
  const auto model = r["model"].text("2pl") == "rasch" ? p::Model::rasch : p::Model::two_parameter;
  const auto fit = p::fit(responses, model);
  Json items = Json::array();
  const auto names = texts(r["items"]);
  for (std::size_t j = 0; j < fit.items.size(); j++)
    items.push(Json::object({{"item", j < names.size() ? names[j] : "item " + std::to_string(j + 1)}, {"discrimination", fit.items[j].a}, {"difficulty", fit.items[j].b}}));
  Json scores = Json::array();
  for (const auto& s : p::score(responses, fit.items)) scores.push(Json::object({{"eap", s.eap}, {"sd", s.sd}}));
  Json info = Json::array();
  for (double t = -4; t <= 4.0001; t += 0.25) info.push(Json::array({Json(t), Json(p::information(fit.items, t))}));
  return Json::object({{"items", items}, {"scores", scores}, {"log_likelihood", fit.log_likelihood}, {"converged", fit.converged}, {"iterations", static_cast<double>(fit.iterations)},
                       {"information", info}, {"model", std::string(model == p::Model::rasch ? "Rasch" : "two-parameter logistic")}});
}

inline Json factors(const Json& r) {
  const auto data = rows_of(r["data"]);
  const std::size_t n = data.size(), p = n ? data[0].size() : 0, k = static_cast<std::size_t>(num(r["factors"], 1));
  if (n < 3 || p < 2) throw std::invalid_argument("factors: at least three cases and two variables");
  std::vector<double> mean(p, 0), sd(p, 0);
  for (const auto& row : data)
    for (std::size_t j = 0; j < p; j++) mean[j] += row[j] / static_cast<double>(n);
  for (const auto& row : data)
    for (std::size_t j = 0; j < p; j++) sd[j] += (row[j] - mean[j]) * (row[j] - mean[j]) / static_cast<double>(n - 1);
  for (auto& s : sd) s = std::sqrt(s);
  calx::linalg::Matrix S(p);
  for (std::size_t a = 0; a < p; a++)
    for (std::size_t b = 0; b < p; b++) {
      double c = 0;
      for (const auto& row : data) c += (row[a] - mean[a]) * (row[b] - mean[b]);
      S(a, b) = c / static_cast<double>(n - 1) / (sd[a] * sd[b]);
    }
  const auto fit = tyche::latent::factor_analysis(S, k, n);
  auto loadings = fit.loadings;
  const bool rotate = !(r["rotate"].is_bool() && !r["rotate"].as_bool()) && k > 1;
  if (rotate) loadings = tyche::latent::varimax(loadings);
  return Json::object({{"variables", r["variables"]}, {"loadings", matrix(loadings)}, {"uniqueness", arr(fit.uniqueness)}, {"communality", arr(fit.communality)},
                       {"chi2", finite(fit.chi2)}, {"df", fit.df}, {"converged", fit.converged}, {"rotated", rotate}, {"heywood", idx(fit.heywood)}, {"factors", static_cast<double>(k)}});
}

inline Json classes(const Json& r) {
  calx::random::Stream rng(static_cast<std::uint64_t>(num(r["seed"], 20261115)), 0x1a7e, 0, 0);
  const auto fit = tyche::latent::latent_classes(ints_of(r["responses"]), static_cast<std::size_t>(num(r["classes"], 2)), rng);
  std::vector<std::size_t> modal;
  for (const auto& post : fit.posterior) modal.push_back(static_cast<std::size_t>(std::max_element(post.begin(), post.end()) - post.begin()));
  return Json::object({{"items", r["items"]}, {"prior", arr(fit.prior)}, {"probability", matrix(fit.probability)}, {"modal", idx(modal)}, {"log_likelihood", fit.log_likelihood},
                       {"bic", fit.bic}, {"converged", fit.converged}});
}

inline Json mixed(const Json& r) {
  tyche::mixed::Data d;
  d.y = numbers(r["y"]);
  for (std::size_t k = 0; k < r["x"].size(); k++) d.x.push_back(numbers(r["x"][k]));
  std::map<std::string, std::size_t> groups;
  std::vector<std::string> labels;
  for (std::size_t i = 0; i < r["group"].size(); i++) {
    const std::string g = r["group"][i].is_string() ? r["group"][i].text() : Json(r["group"][i]).dump();
    if (!groups.count(g)) groups[g] = groups.size(), labels.push_back(g);
    d.group.push_back(groups[g]);
  }
  for (std::size_t k = 0; k < r["slopes"].size(); k++) {
    const auto j = static_cast<std::size_t>(num(r["slopes"][k], 0));
    if (j >= d.x.size()) throw std::invalid_argument("mixed: a random slope names a fixed column that is not there");
    d.z.push_back(d.x[j]);
  }
  tyche::mixed::Options o;
  o.reml = !(r["reml"].is_bool() && !r["reml"].as_bool());
  const auto f = tyche::mixed::fit(d, o);
  Json G = Json::array();
  for (std::size_t a = 0; a < f.G.n; a++) {
    Json row = Json::array();
    for (std::size_t b = 0; b < f.G.n; b++) row.push(f.G(a, b));
    G.push(row);
  }
  Json names = Json::array({Json("intercept")});
  for (const auto& n : texts(r["names"])) names.push(n);
  return Json::object({{"names", names}, {"beta", arr(f.beta)}, {"se", arr(f.se)}, {"G", G}, {"sigma2", f.sigma2}, {"icc", finite(f.icc)}, {"criterion", f.criterion},
                       {"groups", static_cast<double>(f.groups)}, {"labels", arr(labels)}, {"blup", matrix(f.blup)}, {"converged", f.converged}, {"reml", o.reml}});
}

inline Json agreement(const Json& r) {
  const auto data = ints_of(r["codes"]);
  const auto K = static_cast<std::size_t>(num(r["categories"], 2));
  const std::string m = r["metric"].text("nominal");
  const auto metric = m == "ordinal" ? nomos::measurement::Metric::ordinal : m == "interval" ? nomos::measurement::Metric::interval : m == "ratio" ? nomos::measurement::Metric::ratio : nomos::measurement::Metric::nominal;
  Json out = Json::object({{"alpha", finite(nomos::measurement::krippendorff(data, K, metric))}, {"metric", m}});
  if (data.size() == 2) {
    std::vector<int> a, b;
    for (std::size_t u = 0; u < data[0].size(); u++)
      if (data[0][u] >= 0 && data[1][u] >= 0) a.push_back(data[0][u]), b.push_back(data[1][u]);
    if (!a.empty()) out.set("kappa", finite(nomos::measurement::cohen(a, b, K)));
  }
  return out;
}

inline Json anonymity(const Json& r) {
  namespace pd = peira::disclosure;
  pd::Microdata d;
  d.columns = texts(r["columns"]);
  for (std::size_t k = 0; k < r["rows"].size(); k++) d.rows.push_back(texts(r["rows"][k]));
  const auto quasi = texts(r["quasi"]);
  const auto c = pd::classes(d, quasi, static_cast<std::size_t>(num(r["k"], 2)));
  Json sizes = Json::array();
  for (const auto& cl : c.classes) sizes.push(static_cast<double>(cl.size()));
  Json out = Json::object({{"k", static_cast<double>(c.k)}, {"classes", static_cast<double>(c.classes.size())}, {"sizes", sizes}, {"at_risk", idx(c.at_risk)},
                           {"uniques", static_cast<double>(c.uniques)}, {"rows", static_cast<double>(d.rows.size())}});
  if (!r["sensitive"].text().empty()) {
    const auto v = pd::diversity(d, quasi, r["sensitive"].text());
    out.set("distinct", static_cast<double>(v.distinct));
    out.set("entropy", finite(v.entropy));
  }
  return out;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "calibrate") return calibrate(r);
  if (op == "qca") return qca(r);
  if (op == "coding") return coding(r);
  if (op == "concepts") return concepts(r);
  if (op == "mill") return mill(r);
  if (op == "tests") return tests(r);
  if (op == "weigh") return weigh(r);
  if (op == "reliability") return reliability(r);
  if (op == "irt") return irt(r);
  if (op == "factors") return factors(r);
  if (op == "classes") return classes(r);
  if (op == "mixed") return mixed(r);
  if (op == "agreement") return agreement(r);
  if (op == "anonymity") return anonymity(r);
  throw std::invalid_argument("no operation " + op + " (calibrate, qca, coding, concepts, mill, tests, weigh, reliability, irt, factors, classes, mixed, agreement, anonymity)");
}

}  // namespace science::inquiry
