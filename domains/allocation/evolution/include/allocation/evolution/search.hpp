// The search over a composed allocation problem, and what it answers (ADR 0137).
//
// Evaluators assess decoded allocations (closed forms, simulations, agent models, surrogates, a model's or a person's judgements;
// heuresis's `assessment`), each with its kind and standing; to them the search adds kategoria's verdict on every allocation as a
// verified constraint (the resources whose allocation is not lawful), so that no unlawful allocation can be kept. heuresis's
// evolutionary loop searches the composed genotype with each space's own operators, selecting by the tiers of evidence, and
// evaluates each allocation once: its key is what a genotype requests and chooses with the trace of what it granted.
//
// It answers:
// - the allocations kept, best first by the tiers, each re-assessed on fresh replicates, with its lawfulness;
// - the kept allocations compared under declared principles (nomos's `welfare`), from a metric's per-recipient outcomes: the
//   undominated and the best under each principle, never one score;
// - whether the order of grants mattered, by commutator tests on the best allocation's grants and by its declared footprints;
// - the redundancy the keys removed: genotypes evaluated against allocations evaluated;
// - the record, sealed in archeion's journal: the problem, the verification of every resource type, the search, each allocation
//   kept with its assessment and lawfulness, and the comparison.
//
// Two routes on one problem (a generic composition against a specialised encoding, say) are compared over seeds by `compare_routes`:
// the difference of their means is the price of generality, reported as found.

#pragma once

#include <allocation/evolution/problem.hpp>

#include <archeion/fixity/journal.hpp>
#include <archeion/fixity/sha256.hpp>
#include <heuresis/evolution/evolution.hpp>
#include <nomos/welfare/comparison.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace allocation::evolution {

using AssessAllocation = std::function<ha::Assessment(const Allocation&, calx::random::Stream&)>;

// An evaluator of decoded allocations.
inline ha::Evaluator evaluator(std::shared_ptr<const Composition> c, std::string name, ha::Kind kind, ha::Standing standing, AssessAllocation assess,
                               bool stochastic = false, double cost = 1, std::string version = "1") {
  if (!c || !assess) throw Refusal("an evaluator of allocations needs a composition and an assessment");
  ha::Evaluator e;
  e.name = std::move(name), e.version = std::move(version), e.kind = kind, e.standing = standing, e.cost = cost, e.stochastic = stochastic;
  e.assess = [c, assess](const hs::Value& g, calx::random::Stream& s) { return assess(c->decode(g), s); };
  return e;
}

// kategoria's verdict as a verified constraint: the number of resources whose allocation is not lawful.
inline ha::Evaluator lawfulness(std::shared_ptr<const Composition> c) {
  ha::Evaluator e;
  e.name = "lawful by kategoria", e.kind = ha::Kind::closed_form, e.standing = ha::Standing::verified, e.cost = 0;
  e.assess = [c](const hs::Value& g, calx::random::Stream&) {
    const auto L = c->lawful(c->decode(g));
    double failing = 0;
    for (char ok : L.each) failing += ok ? 0 : 1;
    ha::Assessment a;
    ha::Metric m;
    m.name = "resources not lawfully allocated", m.role = ha::Role::constraint, m.value = failing, m.uncertainty = 0, m.standing = ha::Standing::verified;
    a.metrics.push_back(m);
    return a;
  };
  return e;
}

struct Comparing {
  std::string metric;                               // the metric whose per-recipient outcomes are compared ("" for none)
  std::vector<nomos::welfare::Principle> principles;
  std::string per = "population";                   // the recipients' attribute weighing their outcomes ("" for one each)
};

// The loop's strategy by default: age layers (ALPS; Hornby 2006, GECCO, 815–822), which reseed the youngest layer and keep
// searching new basins while older layers refine. On the city case of the checks, survival of the best (μ + λ) settled in a
// deceptive basin on three seeds of seven, where reaching the optimum asks several parts to change at once; age layers found it on
// every seed tried.
inline heuresis::evolution::Strategy default_strategy() {
  heuresis::evolution::Strategy st;
  st.survival = heuresis::evolution::Survival::alps;
  st.from_archive = 0;
  return st;
}

struct Settings {
  heuresis::evolution::Strategy strategy = default_strategy();
  hs::OperatorOptions operators;
  ha::Tiering tiering;
  std::uint64_t seed = 1;
  std::size_t keep = 8;          // allocations kept
  std::size_t reassess = 10;     // fresh replicates for each kept allocation (when an evaluator is stochastic)
  std::size_t walks = 100;       // random walks for the commutator tests
  Comparing comparing;
  std::string when = "2026-01-01T00:00:00Z";  // the instant the record is written at (the core reads no clock)
  std::string who = "allocation.evolution";
};

struct Kept {
  hs::Value genotype;
  Allocation allocation;
  ha::Assessment assessment;
  Lawfulness lawfulness;
  std::string digest;            // of the allocation, as recorded
};

struct Outcome {
  std::vector<Kept> kept;
  std::optional<nomos::welfare::Comparison> comparison;
  std::string comparison_refused;   // why the comparison could not be made, when it could not
  hq::IndependenceReport order;
  std::size_t evaluated_genotypes = 0, evaluated_allocations = 0, evaluator_runs = 0;
  std::size_t unlawful_seen = 0;
  std::size_t reused = 0;        // children the loop gave a remembered evaluation, by key
  archeion::fixity::Journal record;
};

namespace detail {

inline std::string number(double v) {
  char b[40];
  std::snprintf(b, sizeof b, "%.10g", v);
  return b;
}

inline std::string canonical(const Composition& c) {
  const Problem& p = c.problem();
  std::string s = "problem " + p.name + "\n";
  for (const auto& e : p.recipients) {
    s += "recipient " + e.id + " " + e.name;
    for (const auto& [k, v] : e.at) s += " " + k + "=" + number(v);
    s += "\n";
  }
  for (const auto& r : p.resources) {
    s += "resource " + r.name + " " + name(r.carrier) + " type " + r.type.name + " pool";
    for (const auto& q : r.pool) s += " " + q.str();
    s += "\n";
  }
  for (const auto& a : p.attributes) s += "attribute " + a.name + " " + a.space->describe() + "\n";
  for (const auto& q : p.precedence) s += "precedence " + std::to_string(q.before) + " " + std::to_string(q.after) + " " + q.why + "\n";
  s += std::string("ordered ") + (p.ordered ? "yes" : "no") + " unmet " + hq::name(p.unmet) + "\n";
  return s;
}

inline std::string canonical(const Composition& c, const Allocation& a) {
  std::string s;
  for (std::size_t r = 0; r < a.amount.size(); r++) {
    s += c.problem().resources[r].name + ":";
    for (double v : a.amount[r]) s += " " + number(v);
    s += "\n";
  }
  s += "granted:";
  for (auto g : a.granted) s += " " + std::to_string(g);
  s += "\n";
  for (const auto& attr : a.attribute) {
    for (const auto& v : attr) s += hs::key(v) + " ";
    s += "\n";
  }
  return s;
}

}  // namespace detail

inline Outcome search(std::shared_ptr<const Composition> c, std::vector<ha::Evaluator> evaluators, const Settings& settings) {
  if (!c) throw Refusal("a search needs a composition");
  if (evaluators.empty()) throw Refusal("a search needs at least one evaluator");
  evaluators.push_back(lawfulness(c));
  ha::Assessor assessor(evaluators, settings.tiering, settings.seed);
  auto ops = hs::operators(c->space(), settings.operators);
  ops.key = [c](const hs::Value& g) { return c->key(g); };
  heuresis::evolution::Evolution<hs::Value> e(ops, settings.strategy, {}, settings.seed);
  std::set<std::string> genotypes;
  std::map<std::string, hs::Value> by_key;
  heuresis::evolution::evolve(e, [&](const hs::Value& g) {
    genotypes.insert(hs::key(g));
    by_key.emplace(c->key(g), g);
    return assessor.evaluation(g);
  });
  Outcome out;
  out.evaluated_genotypes = genotypes.size();
  out.evaluated_allocations = by_key.size();
  // Every allocation evaluated, ranked by the tiers.
  std::vector<hs::Value> candidates;
  std::vector<ha::Assessment> assessed;
  for (const auto& [k, g] : by_key) {
    candidates.push_back(g);
    assessed.push_back(assessor.assess(g));
    const auto* m = assessed.back().find("resources not lawfully allocated");
    if (m && m->value > 0) out.unlawful_seen++;
  }
  const auto order = ha::tiered_ranking(assessed, settings.tiering);
  std::vector<std::size_t> top;
  for (std::size_t k = 0; k < order.size() && top.size() < settings.keep; k++) {
    const auto& a = assessed[order[k]];
    const auto* law = a.find("resources not lawfully allocated");
    if (!a.ok() || (law && law->value > 0)) continue;
    top.push_back(order[k]);
  }
  // Re-assessed on fresh replicates, and ranked again.
  const bool stochastic = std::any_of(evaluators.begin(), evaluators.end(), [](const ha::Evaluator& ev) { return ev.stochastic; });
  std::vector<ha::Assessment> again;
  for (auto i : top) again.push_back(stochastic ? assessor.reassess(candidates[i], settings.reassess) : assessed[i]);
  const auto reorder = ha::tiered_ranking(again, settings.tiering);
  for (auto j : reorder) {
    Kept k;
    k.genotype = candidates[top[j]];
    k.allocation = c->decode(k.genotype);
    k.assessment = again[j];
    k.lawfulness = c->lawful(k.allocation);
    k.digest = archeion::fixity::sha256_hex(detail::canonical(*c, k.allocation));
    out.kept.push_back(std::move(k));
  }
  out.evaluator_runs = assessor.runs();
  for (const auto& st : e.state().statistics) out.reused += st.reused + st.repeats;
  // Compared under the declared principles.
  if (!settings.comparing.metric.empty() && !settings.comparing.principles.empty() && out.kept.size() >= 1) {
    std::vector<nomos::welfare::Outcome> options;
    for (std::size_t k = 0; k < out.kept.size(); k++) {
      const auto* m = out.kept[k].assessment.find(settings.comparing.metric);
      if (!m || m->units.size() != c->recipients()) throw Refusal("comparing under principles needs the metric '" + settings.comparing.metric + "' with an outcome per recipient");
      nomos::welfare::Outcome o;
      o.name = "allocation " + std::to_string(k + 1);
      o.value = m->units;
      if (!settings.comparing.per.empty())
        for (const auto& r : c->problem().recipients) o.weight.push_back(r.get(settings.comparing.per, 1.0));
      options.push_back(o);
    }
    try {
      out.comparison = nomos::welfare::compare_all(options, settings.comparing.principles);
    } catch (const std::exception& e) {
      out.comparison_refused = e.what();  // a principle that cannot read these outcomes, said so
    }
  }
  // Whether order mattered, at the best allocation's requests.
  if (!out.kept.empty()) {
    calx::random::Stream s(settings.seed, heuresis::search::tag("allocation.evolution.order"), 0, 0);
    out.order = hq::independence(c->grant_system(out.kept[0].allocation.requested), s, settings.walks);
  }
  // The record.
  using archeion::fixity::Entry;
  auto& J = out.record;
  const std::string problem_digest = archeion::fixity::sha256_hex(detail::canonical(*c));
  J.append(Entry{settings.when, settings.who, "declare", problem_digest, "the problem " + c->problem().name, {}});
  for (std::size_t r = 0; r < c->verification().size(); r++) {
    const auto& rep = c->verification()[r];
    J.append(Entry{settings.when, settings.who, "verify", problem_digest,
                   "the resource type " + rep.type + ": " + std::to_string(rep.count(kategoria::kernel::Status::holds)) + " laws hold, " + std::to_string(rep.count(kategoria::kernel::Status::undecided)) + " undecided, none fail", {}});
  }
  std::string evals;
  for (const auto& ev : evaluators) evals += (evals.empty() ? "" : ", ") + ev.name + " (" + ha::name(ev.kind) + ", " + ha::name(ev.standing) + ")";
  J.append(Entry{settings.when, settings.who, "search", problem_digest,
                 "seed " + std::to_string(settings.seed) + ", " + std::to_string(settings.strategy.generations) + " generations of " + std::to_string(settings.strategy.lambda) +
                     "; evaluators: " + evals + "; " + std::to_string(out.evaluated_genotypes) + " genotypes, " + std::to_string(out.evaluated_allocations) + " allocations, " +
                     std::to_string(out.unlawful_seen) + " unlawful",
                 {}});
  for (std::size_t k = 0; k < out.kept.size(); k++) {
    std::string note = "kept " + std::to_string(k + 1) + ":";
    for (const auto& m : out.kept[k].assessment.metrics) note += " " + m.name + " = " + detail::number(m.value) + " (" + ha::name(m.standing) + ")";
    note += out.kept[k].lawfulness.lawful ? "; lawful" : "; NOT lawful";
    J.append(Entry{settings.when, settings.who, "keep", out.kept[k].digest, note, {}});
  }
  if (!out.comparison_refused.empty()) J.append(Entry{settings.when, settings.who, "compare", problem_digest, "the comparison was refused: " + out.comparison_refused, {}});
  if (out.comparison) {
    std::string note = "undominated under " + std::to_string(out.comparison->principles.size()) + " principles:";
    for (auto i : out.comparison->maximal) note += " " + out.comparison->options[i];
    J.append(Entry{settings.when, settings.who, "compare", problem_digest, note, {}});
  }
  return out;
}

struct Routes {
  std::vector<std::string> names;
  std::vector<std::vector<double>> best;   // [route][seed], as each route reports it (minimised)
  std::vector<double> mean;
};

inline Routes compare_routes(const std::vector<std::pair<std::string, std::function<double(std::uint64_t)>>>& routes, const std::vector<std::uint64_t>& seeds) {
  if (routes.empty() || seeds.empty()) throw Refusal("comparing routes needs routes and seeds");
  Routes r;
  for (const auto& [name, run] : routes) {
    r.names.push_back(name);
    std::vector<double> b;
    double sum = 0;
    for (auto s : seeds) b.push_back(run(s)), sum += b.back();
    r.best.push_back(b);
    r.mean.push_back(sum / static_cast<double>(seeds.size()));
  }
  return r;
}

}  // namespace allocation::evolution
