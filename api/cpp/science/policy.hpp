// Evidence-based policy for any work's engine (ADR 0122): one composition of the policy domain with peira, tekmerion, nomos and
// tyche, answering requests in JSON. A request holds the whole question; nothing is read from a file or kept between requests.
//
//   theory     {"text"}   a theory of change read from its written form (policy::change): its nodes and links, or every line that
//              could not be read
//   appraise   {"question"}   the question worked through, each part answered or, where it cannot be, said why:
//              - bodies: each body of evidence pooled by peira (random effects by REML; the prediction interval and Egger's test
//                with three studies or more), graded by the declared scheme with the declared rules read off the synthesis and
//                the reader's factors beside them, every step said; each study with its interval and weight; the pool with each
//                study left out; and, where studies name strata, each stratum pooled
//              - theory: the theory with each body's evidence set on its link, every link's standing, the strongest path from
//                the intervention to the outcome, its weakest link, the gaps on every path, the support factors it needs, and the
//                effect Wright's rule implies where every link states one
//              - here: Cartwright and Hardie's argument for the place, with the support factors of the strongest path by their
//                state here and every premise without evidence named; the effect to expect here, carried over or transported
//                through the strata. The effect found there is the body the place names; else a body of the intervention's
//                effect on the outcome itself (on the link between them, or on no link); else one that bears on the outcome
//              - options: each option's net present value (normal in its effect), its switching worth, the probability that
//                each is best and the value of knowing which
//              - decision: adopt, reject or study first for one option, with the study worth doing, the curve of a study's worth
//                over its size, and robustness over bounds on a unit's worth
//
// The question, noetarium.policy/1:
//   {title, intervention, outcome, least,                       the nodes asked about, and the least grade asked
//    theory: "…",                                               policy::change's written form
//    scheme: "…", rules: [{factor, reading, values, least}],    tekmerion's grading scheme and the rules that read a synthesis
//    bodies: [{id, link: {from, to}, claim, design, measure, higher_is_better, confidence, assessed: {factor: steps}, rules?,
//              studies: [{id, label, year, stratum, effect, variance} | {…, treated_events, treated_others, control_events,
//              control_others}]}],
//    place: {name, same_principle, factors: {name: state}, body, strata: {names, masses}},     states: stated | against | unknown
//    scale: {kind: as_given | risk_ratio, baseline},            how the effect here becomes the outcome's units for an option
//    discount: rate | {from, rate}, options: [{name, effect: "here" | number, se, value, population, cost, from}],
//    decision: {option, study: {unit_variance, fixed_cost, unit_cost, most, delay}, worth: [lower, upper]}, draws, seed}
//
// A body's measure says what its studies' effects are: "as_given" (an effect and its sampling variance), or computed from 2 × 2
// counts by peira as "log_risk_ratio", "log_odds_ratio" or "risk_difference". With scale "risk_ratio" an option whose effect is
// "here" takes the events averted per unit and year, b (1 − e^θ) for a log risk ratio θ expected here and a baseline risk b, with
// the standard error b e^θ se(θ) of the delta method; with "as_given" it takes the effect here as it stands.
//
// The methods are the domain's and the foundations'; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <peira/synthesis/synthesis.hpp>
#include <policy/adoption/adoption.hpp>
#include <policy/change/change.hpp>
#include <policy/evidence/evidence.hpp>
#include <policy/options/options.hpp>
#include <policy/transfer/transfer.hpp>
#include <tekmerion/warrant/derived.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::policy {

namespace pc = ::policy::change;
namespace pe = ::policy::evidence;
namespace pt = ::policy::transfer;
namespace po = ::policy::options;
namespace pa = ::policy::adoption;
namespace ps = peira::synthesis;
namespace tw = tekmerion::warrant;
using apodeixis::kernel::Verdict;

// ---- Small readers and writers -------------------------------------------------------------------------------------------------

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }

inline double number(const Json& j, double fallback = std::numeric_limits<double>::quiet_NaN()) { return j.is_number() ? j.as_number() : fallback; }

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

inline tw::Support support_of(const std::string& w) {
  if (w == "stated" || w == "present" || w == "yes") return tw::Support::stated;
  if (w == "against" || w == "absent" || w == "no") return tw::Support::against;
  return tw::Support::unknown;
}

inline std::vector<tw::Rule> rules_of(const Json& j) {
  std::vector<tw::Rule> out;
  for (std::size_t k = 0; k < j.size(); k++) {
    const Json& x = j[k];
    tw::Rule r;
    r.factor = x["factor"].text();
    const std::string w = x["reading"].text();
    r.reading = w == "crosses" ? tw::Reading::crosses : w == "predicted" ? tw::Reading::predicted : w == "heterogeneous" ? tw::Reading::heterogeneous
              : w == "small_studies" ? tw::Reading::small_studies : w == "large" ? tw::Reading::large
              : throw std::invalid_argument("a rule reads crosses, predicted, heterogeneous, small_studies or large; not " + w);
    for (std::size_t i = 0; i < x["values"].size(); i++) r.values.push_back(number(x["values"][i]));
    r.least = x["least"].is_number() ? static_cast<std::size_t>(x["least"].as_number()) : 0;
    out.push_back(std::move(r));
  }
  return out;
}

inline Json pooled_json(const ps::Pooled& p) {
  return Json::object({{"k", static_cast<double>(p.k)}, {"estimate", num(p.estimate)}, {"lower", num(p.lower)}, {"upper", num(p.upper)}, {"se", num(p.se)}, {"p_value", num(p.p_value)},
                       {"tau2", num(p.tau2)}, {"tau", num(p.tau)}, {"q", num(p.heterogeneity.q)}, {"q_p", num(p.heterogeneity.p_value)}, {"i2", num(p.heterogeneity.i2)},
                       {"model", p.model == ps::Model::fixed ? "fixed effect" : "random effects (REML)"}, {"confidence", p.confidence}});
}

// ---- The theory -----------------------------------------------------------------------------------------------------------------

inline Json theory_json(const pc::Theory& t) {
  Json nodes = Json::array(), links = Json::array();
  for (const auto& n : t.nodes) nodes.push(Json::object({{"id", n.id}, {"kind", n.kind}, {"text", n.text}}));
  for (const auto& l : t.links) {
    const auto st = pc::standing(t, l);
    Json e = Json::object({{"given", l.evidence.given}});
    if (l.evidence.given) e.set("effect", num(l.evidence.effect)), e.set("lower", num(l.evidence.lower)), e.set("upper", num(l.evidence.upper)), e.set("grade", l.evidence.grade);
    links.push(Json::object({{"from", l.from}, {"to", l.to}, {"assumption", l.assumption}, {"support", strings(l.support)}, {"sign", static_cast<double>(l.sign)}, {"evidence", e},
                             {"rank", static_cast<double>(st.rank)}, {"level", st.rank == pc::not_shown ? Json(nullptr) : Json(t.levels[static_cast<std::size_t>(st.rank)])}, {"why", st.why}}));
  }
  return Json::object({{"name", t.name}, {"levels", strings(t.levels)}, {"nodes", nodes}, {"links", links}});
}

inline Json theory(const Json& r) {
  const auto p = pc::read(r["text"].text());
  Json out = Json::object({{"ok", p.ok}, {"errors", strings(p.errors)}});
  if (p.ok) out.set("theory", theory_json(p.theory));
  return out;
}

// ---- Bodies of evidence -----------------------------------------------------------------------------------------------------------

struct Read {
  pe::Body body;
  std::vector<std::string> labels, strata;
  std::vector<double> years;
  std::string id, from, to, measure;
  std::map<std::string, int> assessed;
};

inline Read body_of(const Json& b) {
  Read out;
  out.id = b["id"].text(), out.from = b["link"]["from"].text(), out.to = b["link"]["to"].text();
  out.measure = b["measure"].text("as_given");
  out.body.claim = b["claim"].text(), out.body.design = b["design"].text("randomised");
  out.body.higher_is_better = !b["higher_is_better"].is_bool() || b["higher_is_better"].as_bool();
  out.body.confidence = number(b["confidence"], 0.95);
  if (b["assessed"].is_object())
    for (const auto& [f, steps] : b["assessed"].as_object())
      if (steps.is_number() && steps.as_number() != 0) out.assessed[f] = static_cast<int>(steps.as_number());
  const Json& s = b["studies"];
  for (std::size_t k = 0; k < s.size(); k++) {
    const Json& x = s[k];
    pe::Study st;
    st.id = x["id"].text(std::to_string(k + 1));
    if (out.measure == "as_given") {
      st.effect = number(x["effect"]), st.variance = number(x["variance"]);
      if (!std::isfinite(st.variance) && x["se"].is_number()) st.variance = x["se"].as_number() * x["se"].as_number();
    } else {
      const ps::Measure m = out.measure == "log_risk_ratio" ? ps::Measure::log_risk_ratio : out.measure == "log_odds_ratio" ? ps::Measure::log_odds_ratio
                          : out.measure == "risk_difference" ? ps::Measure::risk_difference
                          : throw std::invalid_argument("the body " + out.id + " has the measure " + out.measure + ", which is none of as_given, log_risk_ratio, log_odds_ratio, risk_difference");
      const ps::TwoByTwo t{number(x["treated_events"]), number(x["treated_others"]), number(x["control_events"]), number(x["control_others"])};
      if (!(std::isfinite(t.treated_events) && std::isfinite(t.treated_others) && std::isfinite(t.control_events) && std::isfinite(t.control_others)))
        throw std::invalid_argument("the study " + st.id + " of the body " + out.id + " gives its four counts: events and others among the treated and among the controls");
      const auto e = ps::effect_size(t, m);
      st.effect = e.effect, st.variance = e.variance;
    }
    out.body.studies.push_back(st);
    out.labels.push_back(x["label"].text(st.id));
    out.strata.push_back(x["stratum"].text());
    out.years.push_back(number(x["year"]));
  }
  return out;
}

struct Assessed {
  Read read;
  pe::Assessed result;
  bool ok = false;
  std::string error;
};

inline Json body_json(const Assessed& a) {
  const auto& rd = a.read;
  Json out = Json::object({{"id", rd.id}, {"link", Json::object({{"from", rd.from}, {"to", rd.to}})}, {"claim", rd.body.claim}, {"design", rd.body.design}, {"measure", rd.measure},
                           {"higher_is_better", rd.body.higher_is_better}});
  if (!a.ok) {
    out.set("error", a.error);
    return out;
  }
  const auto& r = a.result;
  const double z = ps::detail::z_two_sided(rd.body.confidence);
  Json studies = Json::array();
  std::vector<double> y, v;
  for (std::size_t k = 0; k < rd.body.studies.size(); k++) {
    const auto& s = rd.body.studies[k];
    y.push_back(s.effect), v.push_back(s.variance);
    const double se = std::sqrt(s.variance);
    studies.push(Json::object({{"id", s.id}, {"label", rd.labels[k]}, {"year", num(rd.years[k])}, {"stratum", rd.strata[k]}, {"effect", num(s.effect)}, {"variance", num(s.variance)},
                               {"lower", num(s.effect - z * se)}, {"upper", num(s.effect + z * se)}, {"weight", k < r.pooled.weights.size() ? num(r.pooled.weights[k]) : Json(nullptr)}}));
  }
  out.set("studies", studies);
  out.set("pooled", pooled_json(r.pooled));
  if (r.prediction) out.set("prediction", Json::object({{"lower", num(r.prediction->lower)}, {"upper", num(r.prediction->upper)}}));
  if (r.egger) out.set("egger", Json::object({{"bias", num(r.egger->bias)}, {"p_value", num(r.egger->p_value)}}));
  out.set("grade", r.graded.level);
  out.set("steps", strings(r.graded.steps));
  out.set("verdict", verdict_json(r.verdict));
  // The pool with each study left out, where enough remain.
  Json omitted = Json::array();
  if (y.size() >= 3) {
    ps::PoolOptions options;
    options.confidence = rd.body.confidence;
    for (const auto& o : ps::leave_one_out(y, v, options))
      omitted.push(Json::object({{"omitted", rd.body.studies[o.omitted].id}, {"estimate", num(o.fit.estimate)}, {"lower", num(o.fit.lower)}, {"upper", num(o.fit.upper)}}));
  }
  out.set("leave_one_out", omitted);
  // Each stratum the studies name, pooled.
  std::vector<std::string> names;
  for (const auto& s : rd.strata)
    if (!s.empty() && std::find(names.begin(), names.end(), s) == names.end()) names.push_back(s);
  Json strata = Json::array();
  for (const auto& name : names) {
    std::vector<double> ys, vs;
    for (std::size_t k = 0; k < y.size(); k++)
      if (rd.strata[k] == name) ys.push_back(y[k]), vs.push_back(v[k]);
    const auto p = ys.size() == 1 ? ps::fixed_effect(ys, vs, ps::Interval::normal, rd.body.confidence) : ps::random_effects(ys, vs, ps::Tau2::reml, ps::Interval::normal, rd.body.confidence);
    strata.push(Json::object({{"name", name}, {"k", static_cast<double>(ys.size())}, {"effect", num(p.estimate)}, {"variance", num(p.se * p.se)}, {"lower", num(p.lower)}, {"upper", num(p.upper)},
                              {"i2", num(p.heterogeneity.i2)}}));
  }
  out.set("strata", strata);
  return out;
}

// ---- The question worked through -------------------------------------------------------------------------------------------------

inline nomos::appraisal::Schedule schedule_of(const Json& d) {
  if (d.is_number()) return nomos::appraisal::Schedule::constant(d.as_number());
  if (d.is_object() && d["rate"].is_array()) {
    nomos::appraisal::Schedule s;
    for (std::size_t k = 0; k < d["rate"].size(); k++) {
      s.rate.push_back(number(d["rate"][k]));
      s.from.push_back(k < d["from"].size() ? static_cast<std::size_t>(number(d["from"][k], 0)) : 0);
    }
    if (s.rate.empty()) throw std::invalid_argument("a discount schedule states at least one rate");
    return s;
  }
  throw std::invalid_argument("the discount is a rate, or {from, rate} by bands of years");
}

inline Json appraised_json(const po::Appraised& a) {
  return Json::object({{"name", a.name}, {"mean", num(a.mean)}, {"sd", num(a.sd)}, {"factor", num(a.factor)}, {"costs", num(a.costs)}, {"benefits", num(a.at_mean.benefits)},
                       {"ratio", num(a.at_mean.ratio)}, {"switching", num(a.switching)}});
}

inline Json appraise(const Json& r) {
  const Json& q = r["question"];
  Json out = Json::object();

  // The scheme and the bodies.
  std::optional<tw::Scheme> scheme;
  std::vector<tw::Rule> rules;
  try {
    scheme = tw::scheme(q["scheme"].text());
    rules = rules_of(q["rules"]);
    Json starts = Json::object(), factors = Json::array();
    for (const auto& [design, level] : scheme->start) starts.set(design, scheme->levels[level]);
    for (const auto& [name, f] : scheme->factors) {
      const bool read = std::any_of(rules.begin(), rules.end(), [&](const tw::Rule& x) { return x.factor == name; });
      factors.push(Json::object({{"name", name}, {"direction", static_cast<double>(f.direction)}, {"most", static_cast<double>(f.most)}, {"read_by_rule", read}}));
    }
    out.set("scheme", Json::object({{"name", scheme->name}, {"levels", strings(scheme->levels)}, {"start", starts}, {"factors", factors}}));
  } catch (const std::exception& e) {
    out.set("scheme", Json::object({{"error", e.what()}}));
  }
  std::vector<Assessed> bodies;
  Json bodies_json = Json::array();
  for (std::size_t k = 0; k < q["bodies"].size(); k++) {
    Assessed a;
    try {
      a.read = body_of(q["bodies"][k]);
      if (!scheme) throw std::invalid_argument("the grading scheme could not be read, so the body cannot be graded");
      // A body may state rules of its own: a magnitude declared for one measure says nothing of another.
      a.result = pe::assess(a.read.body, *scheme, q["bodies"][k]["rules"].is_array() ? rules_of(q["bodies"][k]["rules"]) : rules, a.read.assessed);
      a.ok = true;
    } catch (const std::exception& e) {
      a.read.id = q["bodies"][k]["id"].text();
      a.read.from = q["bodies"][k]["link"]["from"].text(), a.read.to = q["bodies"][k]["link"]["to"].text();
      a.read.body.claim = q["bodies"][k]["claim"].text(), a.read.body.design = q["bodies"][k]["design"].text(), a.read.measure = q["bodies"][k]["measure"].text("as_given");
      a.error = e.what();
    }
    bodies_json.push(body_json(a));
    bodies.push_back(std::move(a));
  }
  out.set("bodies", bodies_json);

  // The theory, with each body's evidence on its link.
  const auto parsed = pc::read(q["theory"].text());
  if (!parsed.ok) {
    out.set("theory", Json::object({{"ok", false}, {"errors", strings(parsed.errors)}}));
    return out;
  }
  pc::Theory t = parsed.theory;
  Json attach_faults = Json::array();
  std::map<std::string, std::string> body_on;  // "from -> to" → body id
  for (const auto& b : bodies) {
    if (!b.ok || b.read.from.empty()) continue;
    try {
      pe::attach(t, b.read.from, b.read.to, b.result);
      body_on[b.read.from + " -> " + b.read.to] = b.read.id;
    } catch (const std::exception& e) {
      attach_faults.push(Json::object({{"body", b.read.id}, {"what", e.what()}}));
    }
  }
  Json th = theory_json(t);
  th.set("ok", true);
  th.set("faults", attach_faults);
  Json on = Json::object();
  for (const auto& [link, id] : body_on) on.set(link, id);
  th.set("bodies", on);
  out.set("theory", th);

  // How far the outcome is shown to follow from the intervention.
  std::optional<pc::Assessment> assessment;
  try {
    const std::string from = q["intervention"].text(), to = q["outcome"].text();
    const std::string least = q["least"].text(t.levels.front());
    assessment = pc::assess(t, from, to, least);
    const auto& a = *assessment;
    const auto idx = [](const std::vector<std::size_t>& v) { Json j = Json::array(); for (std::size_t x : v) j.push(static_cast<double>(x)); return j; };
    Json as = Json::object({{"paths", static_cast<double>(a.paths)}, {"verdict", verdict_json(a.verdict)}, {"least", least}});
    if (a.paths) {
      as.set("strongest", Json::object({{"links", idx(a.strongest.links)}, {"weakest", a.strongest.weakest == pc::not_shown ? Json(nullptr) : Json(t.levels[static_cast<std::size_t>(a.strongest.weakest)])},
                                        {"limiting", static_cast<double>(a.strongest.links[a.strongest.limiting])}, {"support", strings(a.strongest.support)}}));
      as.set("necessary", idx(a.necessary));
      as.set("gaps", idx(a.necessary_gaps));
      as.set("linear", a.linear);
      as.set("shared", a.shared);
      if (a.linear) as.set("implied", Json::object({{"effect", num(a.implied)}, {"lower", num(a.implied_lower)}, {"upper", num(a.implied_upper)}}));
    }
    out.set("assessment", as);
  } catch (const std::exception& e) {
    out.set("assessment", Json::object({{"error", e.what()}}));
  }

  // From there to here.
  const Json& place = q["place"];
  std::optional<pt::Here> here;
  const Assessed* there = nullptr;
  try {
    if (!assessment || !assessment->paths) throw std::invalid_argument("no path of the theory leads from the intervention to the outcome");
    // The effect found there: the body the place names; else a body of the intervention's effect on the outcome itself (on the
    // link between them, or on no link); else one that bears on the outcome.
    const std::string want = place["body"].text(), cause = q["intervention"].text(), effect = q["outcome"].text();
    const auto first = [&](auto&& fits) {
      for (const auto& b : bodies)
        if (b.ok && !there && fits(b)) there = &b;
    };
    if (!want.empty()) {
      first([&](const Assessed& b) { return b.read.id == want; });
    } else {
      first([&](const Assessed& b) { return b.read.from == cause && b.read.to == effect; });
      first([&](const Assessed& b) { return b.read.from.empty(); });
      first([&](const Assessed& b) { return b.read.to == effect; });
    }
    if (!there) throw std::invalid_argument(want.empty() ? "no body of evidence bears on the outcome: the effect found there is not known" : "there is no body of evidence " + want + " that could be pooled");
    pt::Place p;
    p.name = place["name"].text();
    p.same_principle = support_of(place["same_principle"].text("unknown"));
    std::set<std::string> needed(assessment->strongest.support.begin(), assessment->strongest.support.end());
    if (place["factors"].is_object())
      for (const auto& [f, s] : place["factors"].as_object())
        if (needed.count(f)) p.factors[f] = support_of(s.text("unknown"));  // a factor stated of another path is not this path's
    Json strata_used = Json::array();
    if (place["strata"].is_object() && place["strata"]["names"].size()) {
      pt::Strata z;
      const Json& names = place["strata"]["names"];
      for (std::size_t k = 0; k < names.size(); k++) {
        const std::string name = names[k].text();
        std::vector<double> ys, vs;
        for (std::size_t i = 0; i < there->read.body.studies.size(); i++)
          if (there->read.strata[i] == name) ys.push_back(there->read.body.studies[i].effect), vs.push_back(there->read.body.studies[i].variance);
        if (ys.empty()) throw std::invalid_argument("no study of " + there->read.id + " is of the stratum " + name);
        const auto pooled = ys.size() == 1 ? ps::fixed_effect(ys, vs) : ps::random_effects(ys, vs);
        z.names.push_back(name), z.effects.push_back(pooled.estimate), z.variances.push_back(pooled.se * pooled.se);
        z.place.push_back(number(place["strata"]["masses"][k], 0));
        strata_used.push(Json::object({{"name", name}, {"k", static_cast<double>(ys.size())}, {"effect", num(pooled.estimate)}, {"mass", num(z.place.back())}}));
      }
      p.strata = z;
    }
    here = pt::here(t, assessment->strongest, there->result.pooled, p, there->read.body.higher_is_better);
    const auto& h = *here;
    // Each support factor of the path, with the link that needs it and its state here.
    Json factors = Json::array();
    for (const auto& f : h.needed) {
      const std::string state = std::find(h.present.begin(), h.present.end(), f) != h.present.end() ? "stated" : std::find(h.absent.begin(), h.absent.end(), f) != h.absent.end() ? "against" : "unknown";
      Json links = Json::array();
      for (std::size_t e : assessment->strongest.links)
        if (std::find(t.links[e].support.begin(), t.links[e].support.end(), f) != t.links[e].support.end()) links.push(static_cast<double>(e));
      factors.push(Json::object({{"name", f}, {"state", state}, {"links", links}}));
    }
    out.set("here", Json::object({{"place", p.name}, {"body", there->read.id}, {"same_principle", place["same_principle"].text("unknown")}, {"factors", factors},
                                  {"needed", strings(h.needed)}, {"present", strings(h.present)}, {"absent", strings(h.absent)}, {"unknown", strings(h.unknown)},
                                  {"missing", strings(h.argument.missing)}, {"worked_there", h.argument.worked_there}, {"effect", num(h.effect)}, {"lower", num(h.lower)}, {"upper", num(h.upper)},
                                  {"transported", h.transported}, {"strata", strata_used},
                                  {"there", Json::object({{"effect", num(there->result.pooled.estimate)}, {"lower", num(there->result.pooled.lower)}, {"upper", num(there->result.pooled.upper)}})},
                                  {"verdict", verdict_json(h.verdict)}}));
  } catch (const std::exception& e) {
    out.set("here", Json::object({{"error", e.what()}}));
  }

  // The effect here in the outcome's units, for an option that takes it.
  double unit_effect = std::numeric_limits<double>::quiet_NaN(), unit_se = std::numeric_limits<double>::quiet_NaN();
  std::string scale_said;
  if (here) {
    const double z = ps::detail::z_two_sided(there->read.body.confidence);
    const double se = (here->upper - here->lower) / (2 * z);
    const std::string kind = q["scale"]["kind"].text("as_given");
    if (kind == "risk_ratio") {
      const double b = number(q["scale"]["baseline"]);
      if (std::isfinite(b) && b > 0) {
        unit_effect = b * (1 - std::exp(here->effect)), unit_se = b * std::exp(here->effect) * se;
        scale_said = "events averted per unit and year: the baseline risk " + po::number(b) + " times one less the risk ratio expected here, " + po::number(std::exp(here->effect)) +
                     "; its standard error by the delta method";
      } else {
        scale_said = "the scale is a risk ratio and states no baseline risk";
      }
    } else {
      unit_effect = here->effect, unit_se = se;
      scale_said = "the effect expected here, as it stands";
    }
    out.set("unit", Json::object({{"effect", num(unit_effect)}, {"se", num(unit_se)}, {"said", scale_said}}));
  }

  // The options, and the decision.
  std::vector<po::Option> options;
  try {
    const auto schedule = schedule_of(q["discount"]);
    const Json& os = q["options"];
    for (std::size_t k = 0; k < os.size(); k++) {
      const Json& x = os[k];
      po::Option o;
      o.name = x["name"].text("option " + std::to_string(k + 1));
      if (x["effect"].is_string() && x["effect"].text() == "here") {
        if (!std::isfinite(unit_effect)) throw std::invalid_argument("the option " + o.name + " takes the effect expected here, which is not known: " + (scale_said.empty() ? "the argument for the place could not be made" : scale_said));
        o.effect = unit_effect, o.se = unit_se;
      } else {
        o.effect = number(x["effect"], 0), o.se = number(x["se"], 0);
      }
      o.value = number(x["value"], 0), o.population = number(x["population"], 0);
      for (std::size_t t2 = 0; t2 < x["cost"].size(); t2++) o.cost.push_back(number(x["cost"][t2], 0));
      o.from = x["from"].is_number() ? static_cast<std::size_t>(x["from"].as_number()) : 1;
      options.push_back(std::move(o));
    }
    if (options.size() >= 2) {
      const std::size_t draws = q["draws"].is_number() ? static_cast<std::size_t>(q["draws"].as_number()) : 20000;
      const std::uint64_t seed = q["seed"].is_number() ? static_cast<std::uint64_t>(q["seed"].as_number()) : 20261116;
      const auto c = po::compare(options, schedule, std::min<std::size_t>(std::max<std::size_t>(draws, 2), 200000), seed);
      Json appraised = Json::array(), chosen = Json::array(), best = Json::array();
      for (const auto& a : c.options) appraised.push(appraised_json(a));
      for (std::size_t i : c.chosen) chosen.push(c.options[i].name);
      for (double b : c.best) best.push(num(b));
      out.set("options", Json::object({{"appraised", appraised}, {"chosen", chosen}, {"best", best}, {"evpi", num(c.evpi)}, {"evpi_error", num(c.evpi_error)}, {"draws", static_cast<double>(c.draws)},
                                       {"verdict", verdict_json(c.verdict)}}));
    } else if (options.size() == 1) {
      Json appraised = Json::array();
      appraised.push(appraised_json(po::appraise(options[0], schedule)));
      out.set("options", Json::object({{"appraised", appraised}, {"note", "a comparison is of two options or more (the status quo among them)"}}));
    } else {
      out.set("options", Json::object({{"error", "no option is stated"}}));
    }
    // The decision on one option against doing nothing.
    const Json& d = q["decision"];
    const std::string name = d["option"].text();
    const po::Option* chosen = nullptr;
    for (const auto& o : options)
      if (o.name == name) chosen = &o;
    if (!chosen && !options.empty() && name.empty())
      for (const auto& o : options)
        if (!chosen && (o.effect != 0 || !o.cost.empty())) chosen = &o;
    if (chosen) {
      std::optional<pa::Study> study;
      if (d["study"].is_object()) {
        pa::Study s;
        s.unit_variance = number(d["study"]["unit_variance"], 0), s.fixed_cost = number(d["study"]["fixed_cost"], 0), s.unit_cost = number(d["study"]["unit_cost"], 0);
        s.most = d["study"]["most"].is_number() ? static_cast<std::size_t>(d["study"]["most"].as_number()) : 100000;
        s.delay = number(d["study"]["delay"], 1);
        study = s;
      }
      std::optional<std::pair<double, double>> worth;
      if (d["worth"].is_array() && d["worth"].size() == 2) worth = std::make_pair(number(d["worth"][0]), number(d["worth"][1]));
      const auto dec = pa::decide(*chosen, schedule, study, worth);
      Json curve = Json::array();
      if (study) {
        // The worth of a study over its size, on a grid that doubles: every size is examined for the best one, a few are shown.
        std::set<std::size_t> sizes = {1, dec.n, study->most};
        for (double n = 1; n <= static_cast<double>(study->most); n *= 1.25) sizes.insert(static_cast<std::size_t>(std::llround(n)));
        for (std::size_t n : sizes) {
          if (n < 1 || n > study->most) continue;
          const auto [w, e] = pa::study_worth(dec.option, chosen->se, *study, n);
          curve.push(Json::object({{"n", static_cast<double>(n)}, {"worth", num(w)}, {"evsi", num(e)}, {"cost", num(study->fixed_cost + study->unit_cost * static_cast<double>(n))}}));
        }
      }
      out.set("decision", Json::object({{"option", chosen->name}, {"appraised", appraised_json(dec.option)}, {"adopt", num(dec.adopt)}, {"reject", num(dec.reject)}, {"evpi", num(dec.evpi)},
                                        {"study", num(dec.study)}, {"evsi", num(dec.evsi)}, {"n", static_cast<double>(dec.n)}, {"chosen", strings(dec.chosen)}, {"robust", dec.robust},
                                        {"maximin", dec.maximin}, {"curve", curve}, {"effect", num(chosen->effect)}, {"se", num(chosen->se)}, {"verdict", verdict_json(dec.verdict)}}));
    } else {
      out.set("decision", Json::object({{"error", name.empty() ? "no option is stated to decide on" : "there is no option " + name}}));
    }
  } catch (const std::exception& e) {
    if (!out.has("options")) out.set("options", Json::object({{"error", e.what()}}));
    if (!out.has("decision")) out.set("decision", Json::object({{"error", e.what()}}));
  }
  return out;
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "theory") return theory(r);
  if (op == "appraise") return appraise(r);
  throw std::invalid_argument("science::policy: no operation is called " + op + " (there is: theory, appraise)");
}

}  // namespace science::policy
