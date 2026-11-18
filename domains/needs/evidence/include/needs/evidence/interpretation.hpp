// The interpretation of a statement of need, and its rivals (ADR 0126; the theory note §5a), on tekmerion's construal.
//
// A statement of need is a finding whose terms are open: which income counts as low (one of several indicators of one concept),
// whom the group is compared with, at what level a normative standard is set, which causal structure holds. The dataset declares
// points of interpretation, each with the readings it admits and where each reading comes from (authority, convention, analyst,
// data, proposal: tekmerion's sources), and may state an order of the sources (tekmerion's when it does not).
//
// interpret() reads a statement under every combination of the readings of the points that concern it, the statement's own reading
// among them (added as the analyst's when no declared reading is it), judges each by its gap on one part (z at least z* and the gap
// at least the least effect, in the indicator's spread), and answers with tekmerion's construal: under which readings it holds and
// its label, the admissible interpretations, clear, arguable or untenable over the record's readings and as proposed, and the
// pivotal points.
//   indicator  concerns comparative statements on one of its readings; its readings are indicators of one direction
//   reference  concerns comparative statements (on the indicators it names, or any); a reading is a group, "everyone" (the whole
//              frame, the group included) or "the rest" (the frame less the group, as a statement says when it names none)
//   level      concerns normative and felt statements (on the indicators it names, or any); a reading is a number or a percentage
// Two points of one kind may not concern the same statements.
//
// interpret_causal() reads causal support (causal.hpp) under each causal graph declared: the dataset's own, its stated reading, and
// its alternatives (a model's among them, as a proposal).
//
// rival() asks whether a declared rival explains the gap away: the gap again within each of the rival's strata (the frame joined
// with the stratum), pooled by inverse variance (tekmerion's assess, on peira's fixed effect). Strata are read from one sample, so
// their pooled variance is understated where they share clusters, and a rival a little less likely to be found to explain a gap.
//
// Everything here is read on the part it is given; the search gives it discovery alone.
#pragma once

#include <needs/evidence/causal.hpp>
#include <needs/evidence/gap.hpp>
#include <needs/language/reader.hpp>

#include <tekmerion/construal/construal.hpp>

#include <cmath>
#include <cstdlib>
#include <map>
#include <optional>
#include <string>
#include <vector>

namespace needs::evidence {

namespace tc = tekmerion::construal;
using groups::Group;
using language::Point;
using language::PointKind;

struct InterpretOptions {
  double z = 3.0;                          // z* on the part
  double min_effect = 0;                   // the least gap, in the indicator's spread
  std::map<std::string, double> spread;    // by indicator (1 where absent)
  Inference inference = Inference::design;
  double confidence = 0.95;
  std::size_t limit = 4096;                // interpretations enumerated at most
};

struct Construed {
  bool applies = false;                    // a declared point concerns the statement
  tc::Construal construal;
  std::vector<std::string> statements;     // the statement under each interpretation, in apodeixis's order
  std::vector<Gap> gaps;                   // its gap under each
};

struct CausalConstrued {
  bool applies = false;                    // alternative graphs are declared and the causal test applies
  tc::Construal construal;
  std::vector<Causal> results;             // under each graph
};

namespace detail {

inline tc::Source source(const std::string& s) {
  try {
    return tc::source_of(s);
  } catch (const std::invalid_argument& e) {
    throw Refusal(e.what());
  }
}

inline tc::Reading reading(const language::Reading& r, bool stated) { return {r.text, source(r.source), r.grade, r.by, r.rationale, stated}; }

inline bool rest(const std::string& t) { return t == "the rest" || t == "rest"; }

inline std::optional<Group> reference_of(const Dataset& d, const std::string& text) {
  if (rest(text)) return std::nullopt;
  Group g = language::read_group(text);
  groups::bind(d.table, g);
  groups::check(d.table, g);
  return g;
}

inline std::string reference_key(const std::optional<Group>& g) { return !g ? "the rest" : g->conditions.empty() ? "everyone" : g->key(); }
inline std::string reference_text(const std::optional<Group>& g) { return !g ? "the rest" : g->conditions.empty() ? "everyone" : g->write(); }

inline double level_of(const std::string& t) {
  char* end = nullptr;
  double v = std::strtod(t.c_str(), &end);
  std::string rest(end ? end : "");
  while (!rest.empty() && rest.front() == ' ') rest.erase(rest.begin());
  if (rest == "%") v /= 100;
  else if (!rest.empty() || end == t.c_str()) throw Refusal("a level reads as a number or a percentage, not '" + t + "'");
  return v;
}

inline bool concerns(const Point& p, const Statement& s) {
  if (p.indicators.empty()) return true;
  for (const auto& i : p.indicators)
    if (i == s.indicator) return true;
  return false;
}

inline bool overlaps(const Point& a, const Point& b) {
  const auto set = [](const Point& p) {
    std::vector<std::string> v = p.indicators;
    if (p.kind == PointKind::indicator) {
      v.clear();
      for (const auto& r : p.readings) v.push_back(r.text);
    }
    return v;
  };
  const auto x = set(a), y = set(b);
  if (x.empty() || y.empty()) return true;
  for (const auto& i : x)
    for (const auto& j : y)
      if (i == j) return true;
  return false;
}

inline bool same_level(double a, double b) { return std::fabs(a - b) <= 1e-12 * std::max(1.0, std::fabs(a)); }

}  // namespace detail

// The declarations checked: every reading of its point's kind, a source and a grade named, an indicator point's readings declared
// indicators of one direction, no two points of one kind concerning the same statements, graphs with arrows, rivals with strata.
inline void check_declarations(const Dataset& d) {
  for (std::size_t a = 0; a < d.points.size(); a++) {
    const auto& p = d.points[a];
    if (p.name.empty()) throw Refusal("a point of interpretation has a name");
    if (p.readings.empty()) throw Refusal("the point " + p.name + " has no reading");
    for (const auto& r : p.readings) {
      detail::source(r.source);
      if (r.grade != 'A' && r.grade != 'B' && r.grade != 'C') throw Refusal("the reading " + r.text + " of " + p.name + " has a grade A, B or C");
      if (r.grade == 'C' && r.source != "proposal") throw Refusal("the reading " + r.text + " of " + p.name + " is of grade C, a model's word, and so a proposal");
      if (p.kind == PointKind::indicator) {
        const auto& ind = d.indicator(r.text);
        if (ind.higher_is_worse != d.indicator(p.readings[0].text).higher_is_worse) throw Refusal("the readings of " + p.name + " are indicators worse in one direction");
      } else if (p.kind == PointKind::reference) {
        try {
          (void)detail::reference_of(d, r.text);
        } catch (const std::exception& e) {
          throw Refusal("the reading '" + r.text + "' of " + p.name + " is not a group: " + e.what());
        }
      } else {
        (void)detail::level_of(r.text);
      }
    }
    for (const auto& i : p.indicators) (void)d.indicator(i);
    for (std::size_t b = 0; b < a; b++)
      if (d.points[b].kind == p.kind && detail::overlaps(d.points[b], p)) throw Refusal("the points " + d.points[b].name + " and " + p.name + " are of one kind and concern the same statements");
  }
  for (const auto& g : d.graphs) {
    detail::source(g.source);
    if (g.arrows.empty()) throw Refusal("the graph " + g.name + " has no arrow");
    if (g.name == "declared") throw Refusal("'declared' names the dataset's own graph");
  }
  detail::source(d.graph_source);
  for (const auto& r : d.rivals) {
    detail::source(r.source);
    if (r.strata.size() < 2) throw Refusal("the rival " + r.name + " has at least two strata");
  }
  for (const auto& [w, s] : d.order) detail::source(w), detail::source(s);
}

// The statement read under every combination of the readings of the points that concern it.
inline Construed interpret(const Dataset& d, const Statement& s0, const Parts& parts, Part part, const InterpretOptions& o = {}) {
  Construed out;
  if (s0.standard == Standard::expressed) return out;
  tc::Finding f;
  f.claim = s0.write();
  std::vector<PointKind> kinds;
  for (const auto& p : d.points) {
    tc::Point q{p.name, p.about, {}};
    if (p.kind == PointKind::indicator) {
      if (s0.standard != Standard::comparative) continue;
      bool here = false;
      for (const auto& r : p.readings) here = here || r.text == s0.indicator;
      if (!here) continue;
      bool stated = false;
      for (const auto& r : p.readings) {
        const bool st = !stated && r.text == s0.indicator;
        stated = stated || st;
        q.readings.push_back(detail::reading(r, st));
      }
    } else if (p.kind == PointKind::reference) {
      if (s0.standard != Standard::comparative || !detail::concerns(p, s0)) continue;
      const std::string own = detail::reference_key(s0.reference);
      bool stated = false;
      for (const auto& r : p.readings) {
        const bool st = !stated && detail::reference_key(detail::reference_of(d, r.text)) == own;
        stated = stated || st;
        q.readings.push_back(detail::reading(r, st));
      }
      if (!stated) q.readings.push_back({detail::reference_text(s0.reference), tc::Source::analyst, 'A', "the statement", "as the statement is written", true});
    } else {
      if ((s0.standard != Standard::normative && s0.standard != Standard::felt) || !detail::concerns(p, s0)) continue;
      bool stated = false;
      for (const auto& r : p.readings) {
        const bool st = !stated && detail::same_level(detail::level_of(r.text), s0.level);
        stated = stated || st;
        q.readings.push_back(detail::reading(r, st));
      }
      if (!stated) q.readings.push_back({groups::number_text(s0.level), tc::Source::analyst, 'A', "the statement", "as the statement is written", true});
    }
    kinds.push_back(p.kind);
    f.points.push_back(std::move(q));
  }
  if (f.points.empty()) return out;
  out.applies = true;
  std::size_t n = 1;
  for (const auto& p : f.points) n *= p.readings.size();
  if (n > o.limit) throw Refusal(std::to_string(n) + " interpretations of " + s0.write() + " are refused above " + std::to_string(o.limit));
  out.statements.assign(n, ""), out.gaps.assign(n, Gap{});
  const auto& points = f.points;
  f.judge = [&](const tc::ap::Interpretation& i) {
    std::size_t x = 0, scale = 1;
    Statement s = s0;
    for (std::size_t k = 0; k < points.size(); k++) {
      x += i[k] * scale, scale *= points[k].readings.size();
      const std::string& t = points[k].readings[i[k]].name;
      if (kinds[k] == PointKind::indicator) s.indicator = t;
      else if (kinds[k] == PointKind::reference) s.reference = detail::reference_of(d, t);
      else s.level = detail::level_of(t);
    }
    out.statements[x] = s.write();
    const std::string claim = "the gap of " + s.write();
    try {
      language::check(d, s);
    } catch (const std::exception& e) {
      return tc::Verdict::no(claim, e.what());
    }
    const Gap g = gap(d, s, parts, part, o.inference, o.confidence);
    out.gaps[x] = g;
    if (!g.defined) return tc::Verdict::no(claim, g.why);
    auto sp = o.spread.find(s.indicator);
    const double spread = sp == o.spread.end() || !(sp->second > 0) ? 1.0 : sp->second;
    char b[160];
    std::snprintf(b, sizeof b, "gap %.4g (effect %.2f), z %.2f on the %s part", g.estimate, g.estimate / spread, g.z, name(part));
    const bool holds = g.z >= o.z && g.estimate / spread >= o.min_effect;
    return holds ? tc::Verdict::yes(claim, b) : tc::Verdict::no(claim, b);
  };
  tc::Options co;
  if (d.order_stated) co.order = d.order;
  co.limit = o.limit;
  out.construal = tc::construe(f, co);
  return out;
}

// Causal support read under each causal graph declared: the dataset's own (its stated reading) and the alternatives.
inline CausalConstrued interpret_causal(const Dataset& d, const Statement& s, const Parts& parts, Part part, double confidence = 0.95) {
  CausalConstrued out;
  if (d.graphs.empty() || !d.provision.count(s.domain) || s.standard == Standard::expressed) return out;
  tc::Finding f;
  f.claim = "more provision goes with less need in " + s.write();
  tc::Point q{"graph", "the causal structure among the columns", {}};
  std::vector<const std::vector<language::Arrow>*> arrows;
  if (!d.graph.empty()) q.readings.push_back({"declared", detail::source(d.graph_source), 'A', "the data", "the graph declared with the data", true}), arrows.push_back(&d.graph);
  for (const auto& g : d.graphs) q.readings.push_back({g.name, detail::source(g.source), g.grade, g.by, g.rationale, false}), arrows.push_back(&g.arrows);
  f.points = {q};
  out.results.assign(arrows.size(), Causal{});
  f.judge = [&](const tc::ap::Interpretation& i) {
    out.results[i[0]] = causal(d, s, parts, part, confidence, arrows[i[0]]);
    const auto& c = out.results[i[0]];
    const std::string claim = "causal support under " + q.readings[i[0]].name;
    return c.verdict == Verdict::holds ? tc::Verdict::yes(claim, c.why) : c.verdict == Verdict::fails ? tc::Verdict::no(claim, c.why) : tc::Verdict::no(claim, "undecided: " + c.why);
  };
  tc::Options co;
  if (d.order_stated) co.order = d.order;
  out.construal = tc::construe(f, co);
  out.applies = true;
  return out;
}

// Whether a rival explains the gap away, part of it, or not, on one part.
inline tc::RivalAssessment rival(const Dataset& d, const Statement& s, const language::Rival& r, const Parts& parts, Part part, const InterpretOptions& o = {}) {
  const Gap crude = gap(d, s, parts, part, o.inference, o.confidence);
  if (!crude.defined || !(crude.se > 0)) {
    tc::RivalAssessment a;
    a.claim = r.name;
    a.reason = "the gap has no standard error on the " + std::string(name(part)) + " part" + (crude.why.empty() ? "" : ": " + crude.why);
    return a;
  }
  tc::Rival R;
  R.claim = r.name, R.source = detail::source(r.source), R.grade = r.grade, R.by = r.by;
  R.crude = crude.estimate, R.crude_variance = crude.se * crude.se;
  for (std::size_t k = 0; k < r.strata.size(); k++) {
    Statement t = s;
    t.frame = groups::join(s.frame, r.strata[k]);
    const Gap g = gap(d, t, parts, part, o.inference, o.confidence);
    R.strata.push_back({k < r.labels.size() ? r.labels[k] : r.strata[k].write(), g.defined ? g.estimate : nan, g.defined && g.se > 0 ? g.se * g.se : nan});
  }
  return tc::assess(R, o.z);
}

// The strata of a rival from one attribute: each level of a category, or the intervals between cut points of a number.
inline std::vector<Group> strata_of(const Dataset& d, const std::string& attribute, const std::vector<double>& cuts = {}) {
  const auto* col = d.table.find(attribute);
  if (!col) throw Refusal("no column " + attribute + " to take strata from");
  std::vector<Group> out;
  if (col->kind == groups::Kind::category) {
    for (const auto& l : col->levels) {
      groups::Condition c;
      c.attribute = attribute, c.op = groups::Op::eq, c.levels = {l};
      out.push_back(Group{{c}});
    }
    return out;
  }
  if (cuts.empty()) throw Refusal("strata of the number " + attribute + " need cut points");
  for (std::size_t k = 0; k + 1 < cuts.size(); k++)
    if (!(cuts[k] < cuts[k + 1])) throw Refusal("the cut points of " + attribute + " increase");
  const auto at = [&](groups::Op op, double v) {
    groups::Condition c;
    c.attribute = attribute, c.op = op, c.number = v, c.literal = groups::number_text(v);
    return c;
  };
  out.push_back(Group{{at(groups::Op::lt, cuts[0])}});
  for (std::size_t k = 0; k + 1 < cuts.size(); k++) out.push_back(Group{{at(groups::Op::ge, cuts[k]), at(groups::Op::lt, cuts[k + 1])}});
  out.push_back(Group{{at(groups::Op::ge, cuts.back())}});
  return out;
}

}  // namespace needs::evidence
