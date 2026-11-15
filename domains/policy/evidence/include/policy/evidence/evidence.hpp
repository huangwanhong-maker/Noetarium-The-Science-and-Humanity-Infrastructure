// The evidence for a causal claim of a theory of change: studies pooled, graded, and set on the link they bear on.
//
// A body of evidence for one claim (an intervention's effect on an outcome, or one link of a theory) is a set of studies of one
// design and one measure, each with its effect and its sampling variance. It is pooled by peira (random effects by REML, or the
// fixed effect of a single study), with the prediction interval and Egger's test where there are studies enough for them, and
// graded by a scheme the caller declares (tekmerion's warrant): the factors that are properties of the synthesis (imprecision,
// inconsistency, small-study effects, a large effect) read off it by declared rules, the others (risk of bias, indirectness)
// assessed by a reader and given. The result is the link's evidence: the pooled effect, its interval and its grade, with the
// reasons, which policy::change then reads.
//
// No scheme, threshold or level here is any body's own; the checks state theirs, written after GRADE as examples.

#pragma once

#include <policy/change/change.hpp>

#include <peira/synthesis/synthesis.hpp>
#include <tekmerion/warrant/derived.hpp>

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace policy::evidence {

using apodeixis::kernel::Verdict;
namespace ps = peira::synthesis;
namespace tw = tekmerion::warrant;

struct Study {
  std::string id;
  double effect = 0, variance = 0;
};

struct Body {
  std::string claim;                 // what the studies are evidence of, in words
  std::string design;                // a design the scheme starts (randomised, observational, …)
  std::vector<Study> studies;
  bool higher_is_better = true;
  double confidence = 0.95;
};

struct Assessed {
  ps::Pooled pooled;
  std::optional<ps::PredictionInterval> prediction;  // with three studies or more
  std::optional<ps::Egger> egger;                    // with three studies or more
  tw::Graded graded;
  change::Evidence evidence;                         // what a link of a theory is given
  Verdict verdict;
};

// Pools and grades a body of evidence: the rules read their factors off the synthesis; `assessed` gives the reader's.
inline Assessed assess(const Body& b, const tw::Scheme& scheme, const std::vector<tw::Rule>& rules, const std::map<std::string, int>& assessed = {}) {
  if (b.studies.empty()) throw std::invalid_argument("policy::evidence: a body of evidence has at least one study");
  std::vector<double> y, v;
  for (const auto& s : b.studies) {
    if (!(s.variance > 0) || !std::isfinite(s.effect)) throw std::invalid_argument("policy::evidence: the study " + s.id + " has a finite effect and a positive variance");
    y.push_back(s.effect), v.push_back(s.variance);
  }
  Assessed a;
  a.pooled = b.studies.size() == 1 ? ps::fixed_effect(y, v, ps::Interval::normal, b.confidence) : ps::random_effects(y, v, ps::Tau2::reml, ps::Interval::normal, b.confidence);
  if (b.studies.size() >= 3) {
    a.prediction = ps::prediction_interval(a.pooled, b.confidence);
    a.egger = ps::egger(y, v, b.confidence);
  }
  a.graded = tw::grade_synthesis(scheme, b.design, {a.pooled, a.prediction, a.egger, b.higher_is_better}, rules, assessed);
  a.evidence = {true, a.pooled.estimate, a.pooled.lower, a.pooled.upper, a.graded.level};
  const std::string claim = b.claim.empty() ? "the body of evidence is assessed" : b.claim;
  a.verdict = Verdict::yes(claim, std::to_string(b.studies.size()) + (b.studies.size() == 1 ? " study gives " : " studies pooled give ") + change::number(a.pooled.estimate) + " [" +
                                      change::number(a.pooled.lower) + ", " + change::number(a.pooled.upper) + "], graded " + a.graded.level + ": " + a.graded.verdict.reason);
  a.verdict.with("grade", a.graded.level);
  return a;
}

// Sets the assessed evidence on the link `from -> to` of a theory.
inline void attach(change::Theory& t, const std::string& from, const std::string& to, const Assessed& a) {
  for (auto& l : t.links)
    if (l.from == from && l.to == to) {
      if (std::find(t.levels.begin(), t.levels.end(), a.evidence.grade) == t.levels.end())
        throw std::invalid_argument("policy::evidence::attach: the grade " + a.evidence.grade + " is not among the theory's levels: the scheme and the theory disagree");
      l.evidence = a.evidence;
      return;
    }
  throw std::invalid_argument("policy::evidence::attach: the theory has no link " + from + " -> " + to);
}

}  // namespace policy::evidence
