// The evolutionary exploration and validation of needs (ADR 0125; the theory note §4, §5): statements of need searched for by
// heuresis's evolution, every stage's strategy chosen by the caller, evaluated on the discovery part, admitted to an archive through
// a reusable holdout on the validation part, and confirmed once each on the confirmation part, in order, by an online procedure.
//
// THE LOOP, a generation at a time:
//   1. children are made by the generators (generators.hpp), the mutations and the crossovers (operators.hpp), or proposed from
//      outside (a person, a language model: propose()), as heuresis's schedule chooses and credits them;
//   2. each is evaluated on the discovery part by the objectives named (below), with constraints on its coverage and size;
//   3. survivors are chosen by the survival strategy; the archive's judge looks at every child not yet kept: it admits a child
//      whose gap clears z* on discovery, whose effect (the gap over the indicator's spread) reaches the least declared, and which
//      is stable under resampling of clusters, each of whose conditions carries a gap of its own within the rest of the group (a
//      productive statement, after Webb 2007), which is not untenable under the declared readings of its open terms and which no
//      declared rival explains away (evidence/interpretation.hpp, ADR 0126, both on discovery); and then asks the validation part
//      through Thresholdout whether it agrees. A child
//      admitted is kept with its note and takes part in later generations as a parent. Needs are kept distinct: a child within the
//      least distance of a kept statement (in whom it concerns and what it says) and no stronger is not kept, and a kept statement
//      that a stronger one within it supersedes is retired. A kept statement is also retired, with the reason, when it has
//      produced nothing that survives for a declared number of generations, when it is too old, when the archive is full, or when
//      the data change and it is no longer upheld;
//   4. every statement admitted is confirmed once, in the order of admission: the test is registered in a journal sealed by
//      archeion's hash chain before the confirmation part is read, its one-sided p-value (or the e-value calibrated from it) is
//      decided by the online rule (tyche's LOND, LORD++, SAFFRON, α-investing or e-LOND), and the outcome is written after it.
//      Nothing of the confirmation reaches the search: not the judge, not the credit of the operators, not retirement.
//
// A MODEL'S PART (ADR 0126): requests() says what to ask a language model for, in four roles (seed: new statements; mutate: a
// variation of one member; cross: one statement from two; reflect: a refinement of a kept statement from what discovery says of
// it), each with its share of a budget by how its proposals have fared. What the model writes is read by the language like anyone's
// and proposed under "model:<role>" with the members it started from as parents; it is shown discovery's diagnostics alone.
//
// THE OBJECTIVES (all minimised; any of them, in the order named):
//   gap         −z, the standardised gap on discovery          effect      −(gap / the indicator's spread)
//   coverage    −log₁₀ people                                   size        −log₁₀(1 + gap × people), signed
//   quality     −(people's share)^½ × effect (Klösgen)          simplicity  the number of conditions
//   stability   −the share of 30 resamples clearing z = 2       causal      −the provision effect's t in the expected direction
//   strata      −z in each of k strata of discovery's clusters (lexicase's cases)
#pragma once

#include <needs/evidence/standing.hpp>
#include <needs/evidence/stability.hpp>
#include <needs/search/generators.hpp>
#include <needs/search/operators.hpp>

#include <archeion/fixity/journal.hpp>
#include <tyche/testing/evalues.hpp>
#include <tyche/testing/holdout.hpp>
#include <tyche/testing/online.hpp>

#include <algorithm>
#include <array>
#include <cstdio>
#include <limits>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

namespace needs::search {

namespace hev = heuresis::evolution;

struct Options {
  hev::Strategy strategy;
  std::vector<std::string> objectives{"gap", "coverage", "simplicity"};
  std::map<std::string, double> operators;              // weights by name; an operator absent has weight 1, one of weight 0 is off
  evidence::Inference inference = evidence::Inference::design;
  std::array<double, 3> fractions{0.5, 0.25, 0.25};
  std::uint64_t key = 20261118;
  SpaceOptions space;
  // Admission.
  double z_admit = 3.0;
  double min_effect = 0.2;                              // in the indicator's spread
  double z_productive = 2.0;                            // each condition's own gap within the rest of the group (0: no gate)
  double min_distance = 0.25;                           // a statement this close to one kept and no stronger is not kept (0: no gate)
  double stability_min = 0.8, stability_threshold = 2.0;
  std::size_t stability_resamples = 100;
  tyche::testing::Thresholdout::Options holdout{0.3, 0.05, 100};  // in the indicator's spread, queries clipped to [−1, 1]
  // Interpretation and rivals, on discovery, before the validation part is asked (ADR 0126).
  bool interpret = true;
  bool refuse_untenable = true;                         // not kept: untenable under the admissible readings of its terms
  bool admit_clear_only = false;                        // kept only when clear under them
  bool refuse_explained = true;                         // not kept: a declared rival explains the gap away
  // Confirmation.
  tyche::testing::OnlineRule rule = tyche::testing::OnlineRule::e_lond;
  double alpha = 0.05;
  // Others.
  std::size_t strata = 5;
  std::size_t min_clusters = 5;                         // a group must reach people in this many clusters of discovery (testable by design)
  std::string started = "2026-01-01T00:00:00Z", agent = "needs search", data_version = "1";
  bool simulate = true;
  evidence::SeekingOptions seeking;

  Options() {
    strategy.mu = 60, strategy.lambda = 60, strategy.generations = 30, strategy.survival = hev::Survival::plus;
    strategy.generator_rate = 0.15, strategy.crossover_rate = 0.35, strategy.from_archive = 0.15;
    strategy.retire_after = 8, strategy.archive_capacity = 120;
  }
};

struct Validation {
  std::string verdict;      // agreed (the discovery value was answered), told (the validation value), spent, refused
  double value = evidence::nan;
};

struct Admission {
  heuresis::search::Ticket ticket = 0;
  std::string key, text, note, data_version;
  std::size_t generation = 0;
  double z = evidence::nan, effect = evidence::nan, stability = evidence::nan;
  Validation validation;
  std::string reading;                                  // clear or arguable under its readings; empty when no point concerns it
  double share = evidence::nan;                         // of the admissible interpretations under which it holds
};

// What the search asks of a model, in a role, with the members it is to start from and what the discovery part says of them
// (never the validation or confirmation parts). A role's share of a budget follows how its proposals have fared.
struct Request {
  std::string role;                                     // seed, mutate, cross, reflect
  std::size_t count = 1;                                // statements asked for
  std::vector<heuresis::search::Ticket> parents;
  std::vector<std::string> texts, notes;                // the parents' statements, and their discovery diagnostics
  double credit = evidence::nan;                        // (1 + survived) / (2 + proposed) of the role's proposals so far
};

inline const std::vector<std::string>& model_roles() {
  static const std::vector<std::string> r{"seed", "mutate", "cross", "reflect"};
  return r;
}

struct Confirmation {
  std::size_t serial = 0;
  heuresis::search::Ticket ticket = 0;
  std::string key, text;
  std::size_t generation = 0;
  double level = evidence::nan, p = evidence::nan, e = evidence::nan;
  double estimate = evidence::nan, low = evidence::nan, high = evidence::nan, z = evidence::nan, coverage = 0;
  bool confirmed = false, opposite = false, defined = false;
  std::string why, seal;
};

// What a search must keep to be resumed, and what it found.
struct Saved {
  hev::State<Statement> evolution;
  std::vector<Admission> admissions;
  std::vector<Confirmation> confirmations;
  std::vector<std::pair<std::string, Validation>> validated;   // every validation query, by statement, so none is asked twice
  archeion::fixity::Journal journal;
  tyche::testing::Thresholdout::Saved holdout{};
  bool holdout_started = false;
  std::vector<double> online;                                  // the values the online rule was given, in order (it is replayed)
  std::vector<std::size_t> bfs, dfs;
  std::map<std::string, std::size_t> handed;
};

// Everything the tests say of one statement on the discovery part, with its confirmation when it has one.
struct Evidence {
  evidence::Gap discovery, validation;   // validation: only as the holdout answered, never the raw value (see validation_answer)
  Validation validation_answer;
  evidence::Stability stability;
  evidence::Causal causal;
  evidence::Simulation simulation;
  evidence::Entitlement rules;
  std::optional<Confirmation> confirmation;
  evidence::Judgement standing;
  double effect = evidence::nan;
  evidence::Construed interpretation;                   // under the readings of its open terms (ADR 0126)
  evidence::CausalConstrued causal_readings;            // causal support under each declared graph
  std::vector<tekmerion::construal::RivalAssessment> rivals;
};

class Search {
 public:
  Search(Dataset d, Options o) : o_(std::move(o)) { init(std::move(d)); }
  Search(Dataset d, Options o, Saved s) : o_(std::move(o)) {
    saved_ = std::move(s);
    init(std::move(d));
  }
  Search(const Search&) = delete;
  Search& operator=(const Search&) = delete;

  // Statements proposed from outside, by `source`; a proposal made from members names their tickets (one list each, or none).
  void propose(const std::vector<Statement>& statements, const std::string& source, const std::vector<std::vector<heuresis::search::Ticket>>& parents = {}) {
    evo_->propose(statements, source, parents);
  }

  // What to ask a model for, within a budget of statements, among the roles given. Each role's share is its credit, the posterior
  // mean (1 + s) / (2 + b) of the survival of its proposals (b proposed under the source "model:<role>", s survived), by largest
  // remainders; reflect needs kept statements, mutate and cross a population. Parents are the population's best by the first
  // objective, taken in turn.
  std::vector<Request> requests(std::size_t budget, const std::vector<std::string>& roles = model_roles()) const {
    std::vector<Request> out;
    if (budget == 0) return out;
    std::vector<std::string> use;
    for (const auto& r : roles) {
      if (std::find(model_roles().begin(), model_roles().end(), r) == model_roles().end()) throw Refusal("no role '" + r + "' for a model (there are seed, mutate, cross, reflect)");
      if ((r == "mutate" || r == "cross") && evo_->population().size() < (r == "cross" ? 2u : 1u)) continue;
      if (r == "reflect" && evo_->archive().empty()) continue;
      use.push_back(r);
    }
    if (use.empty()) use.push_back("seed");
    std::vector<double> credit;
    double total = 0;
    for (const auto& r : use) {
      double born = 0, kept = 0;
      for (const auto& st : evo_->statistics()) {
        if (auto it = st.proposals.find("model:" + r); it != st.proposals.end()) born += static_cast<double>(it->second);
        if (auto it = st.proposals_surviving.find("model:" + r); it != st.proposals_surviving.end()) kept += static_cast<double>(it->second);
      }
      credit.push_back((1 + kept) / (2 + born)), total += credit.back();
    }
    std::vector<std::size_t> n(use.size(), 0);
    std::vector<std::pair<double, std::size_t>> rem;
    std::size_t given = 0;
    for (std::size_t k = 0; k < use.size(); k++) {
      const double share = static_cast<double>(budget) * credit[k] / total;
      n[k] = static_cast<std::size_t>(std::floor(share)), given += n[k];
      rem.push_back({share - std::floor(share), k});
    }
    std::stable_sort(rem.begin(), rem.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    for (std::size_t j = 0; given < budget; j = (j + 1) % rem.size()) n[rem[j].second]++, given++;
    // The population ranked by the first objective (failures last), and the kept statements, latest first.
    std::vector<const hev::Member<Statement>*> ranked;
    for (const auto& m : evo_->population()) ranked.push_back(&m);
    std::stable_sort(ranked.begin(), ranked.end(), [](const auto* a, const auto* b) {
      const double x = a->evaluation.failed() || a->evaluation.objectives.empty() ? INFINITY : a->evaluation.objectives[0];
      const double y = b->evaluation.failed() || b->evaluation.objectives.empty() ? INFINITY : b->evaluation.objectives[0];
      return x < y;
    });
    std::vector<const hev::Kept<Statement>*> kept;
    for (const auto& k : evo_->archive()) kept.push_back(&k);
    std::reverse(kept.begin(), kept.end());
    std::size_t next = 0, next_kept = 0;
    const auto with = [&](Request& q, const Statement& s, heuresis::search::Ticket t) { q.parents.push_back(t), q.texts.push_back(s.write()), q.notes.push_back(note(s)); };
    for (std::size_t k = 0; k < use.size(); k++) {
      if (n[k] == 0) continue;
      const std::string& r = use[k];
      if (r == "seed") {
        Request q;
        q.role = r, q.count = n[k], q.credit = credit[k];
        out.push_back(std::move(q));
        continue;
      }
      for (std::size_t j = 0; j < n[k]; j++) {
        Request q;
        q.role = r, q.credit = credit[k];
        if (r == "reflect") {
          const auto* kp = kept[next_kept++ % kept.size()];
          with(q, kp->member.genotype, kp->member.ticket);
        } else {
          const std::size_t parents = r == "cross" ? 2 : 1;
          for (std::size_t p = 0; p < parents; p++) {
            const auto* m = ranked[next++ % ranked.size()];
            with(q, m->genotype, m->ticket);
          }
        }
        out.push_back(std::move(q));
      }
    }
    return out;
  }

  // What the discovery part says of a statement, in words: the gap, its effect, whom it reaches, and how its readings stand.
  std::string note(const Statement& s0) const {
    Statement s = s0;
    try {
      language::bind(*d_, s);
      language::check(*d_, s);
    } catch (const std::exception& e) {
      return std::string("not judged: ") + e.what();
    }
    const auto g = evidence::estimate(evidence::totals(*d_, s, *parts_, Part::discovery), o_.inference);
    if (!g.defined) return "on discovery: " + g.why;
    char b[240];
    std::snprintf(b, sizeof b, "on discovery: %s %.4g against %.4g, z %.2f, effect %.2f, %.0f people in %zu areas", s.standard == Standard::comparative ? "group" : "value", g.group_value,
                  g.reference_value, g.z, g.estimate / spread_of(s), g.coverage, g.clusters);
    std::string out = b;
    try {
      const auto c = evidence::interpret(*d_, s, *parts_, Part::discovery, interpret_options());
      if (c.applies) {
        std::snprintf(b, sizeof b, "; %s under its readings (%.0f%% of them)", std::string(apodeixis::interpretation::word(c.construal.record)).c_str(), 100 * c.construal.share);
        out += b;
      }
    } catch (const std::exception&) {
    }
    return out;
  }

  evidence::InterpretOptions interpret_options() const {
    evidence::InterpretOptions io;
    io.z = o_.z_admit, io.min_effect = o_.min_effect, io.spread = space_->spread, io.inference = o_.inference;
    return io;
  }

  // One generation. False when the search has stopped or could make nothing new.
  bool step() {
    if (evo_->stopped()) return false;
    const std::size_t before = evo_->generation();
    evo_->step([&](const Statement& s) { return evaluate(s); });
    confirm_new();
    return evo_->generation() != before;
  }
  void run(std::size_t generations) {
    for (std::size_t k = 0; k < generations && step(); k++) {
    }
  }

  // ---- what it holds ----
  const Dataset& data() const { return *d_; }
  const Parts& parts() const { return *parts_; }
  const Space& space() const { return *space_; }
  const Options& options() const { return o_; }
  const hev::Evolution<Statement>& evolution() const { return *evo_; }
  const std::vector<Admission>& admissions() const { return saved_.admissions; }
  const std::vector<Confirmation>& confirmations() const { return saved_.confirmations; }
  const archeion::fixity::Journal& journal() const { return saved_.journal; }
  const tyche::testing::Thresholdout& holdout() const { return *holdout_; }
  const tyche::testing::Online& online() const { return *online_; }
  const std::map<std::string, evidence::Field>& fields() const { return gen_->fields; }
  const evidence::Rulebook& rulebook() const { return *rules_; }

  Saved saved() const {
    Saved s = saved_;
    s.evolution = evo_->state();
    s.holdout = holdout_->saved(), s.holdout_started = true;
    s.bfs = gen_->bfs, s.dfs = gen_->dfs, s.handed = gen_->handed;
    return s;
  }

  // ---- evaluation ----
  heuresis::search::Evaluation evaluate(const Statement& s) const {
    language::check(*d_, s);
    const auto T = evidence::totals(*d_, s, *parts_, Part::discovery);
    const auto g = evidence::estimate(T, o_.inference);
    if (!g.defined) return heuresis::search::failure(g.why);
    heuresis::search::Evaluation e;
    const double spread = spread_of(s);
    const double eff = g.estimate / spread;
    for (const auto& name : o_.objectives) {
      if (name == "gap") e.objectives.push_back(-std::clamp(g.z, -50.0, 50.0));
      else if (name == "effect") e.objectives.push_back(-std::clamp(eff, -50.0, 50.0));
      else if (name == "coverage") e.objectives.push_back(-std::log10(std::max(g.coverage, 1.0)));
      else if (name == "size") e.objectives.push_back(-(g.size >= 0 ? 1 : -1) * std::log10(1 + std::fabs(g.size)));
      else if (name == "quality") e.objectives.push_back(-std::sqrt(g.coverage / std::max(people_, 1e-12)) * std::clamp(eff, -50.0, 50.0));
      else if (name == "simplicity") e.objectives.push_back(static_cast<double>(size(s)));
      else if (name == "stability") e.objectives.push_back(-evidence::stability(T, 2.0, 30, o_.key).share);
      else if (name == "causal") {
        const auto c = evidence::causal(*d_, s, *parts_, Part::discovery);
        double t = 0;
        if (c.applicable && std::isfinite(c.effect) && c.se > 0) t = (d_->indicator(s.indicator).higher_is_worse ? -1 : 1) * c.effect / c.se;
        e.objectives.push_back(-std::clamp(t, -20.0, 20.0));
      } else if (name == "strata") {
        for (double z : evidence::stratum_gaps(T, strata_, o_.strata)) e.objectives.push_back(-z);
      } else {
        throw Refusal("no objective '" + name + "' (there are gap, effect, coverage, size, quality, simplicity, stability, causal, strata)");
      }
    }
    e.constraints = {(space_->min_coverage - g.coverage) / std::max(space_->min_coverage, 1e-12), static_cast<double>(size(s)) - static_cast<double>(space_->max_conditions),
                     (static_cast<double>(o_.min_clusters) - static_cast<double>(g.clusters)) / std::max<double>(1.0, static_cast<double>(o_.min_clusters))};
    e.descriptors = {static_cast<double>(domain_index(s.domain)), static_cast<double>(static_cast<int>(s.standard)), std::log10(std::max(g.coverage, 1.0)), static_cast<double>(size(s))};
    return e;
  }

  // Everything the tests say of a statement (on discovery; its confirmation when it has had one).
  Evidence evidence_of(const Statement& s0) const {
    Statement s = s0;
    language::bind(*d_, s);
    language::check(*d_, s);
    Evidence ev;
    const auto T = evidence::totals(*d_, s, *parts_, Part::discovery, o_.inference == evidence::Inference::binomial);
    ev.discovery = evidence::estimate(T, o_.inference);
    ev.effect = ev.discovery.defined ? ev.discovery.estimate / spread_of(s) : evidence::nan;
    ev.stability = evidence::stability(T, o_.stability_threshold, o_.stability_resamples, o_.key);
    ev.causal = evidence::causal(*d_, s, *parts_, Part::discovery);
    auto f = gen_->fields.find(s.indicator);
    if (f != gen_->fields.end()) ev.simulation = evidence::judge(f->second, *d_, s, *parts_, Part::discovery);
    else ev.simulation.why = "no seeking model was run for " + s.indicator;
    ev.rules = evidence::entitlement(*rules_, *d_, s, *parts_, Part::discovery);
    const std::string k = s.key();
    for (const auto& [key, v] : saved_.validated)
      if (key == k) ev.validation_answer = v;
    for (const auto& c : saved_.confirmations)
      if (c.key == k) ev.confirmation = c;
    evidence::Findings fd;
    fd.discovery = ev.discovery.defined && ev.discovery.z >= o_.z_admit;
    fd.stability_tested = std::isfinite(ev.stability.share), fd.stable = ev.stability.share >= o_.stability_min;
    fd.validation_tested = !ev.validation_answer.verdict.empty() && ev.validation_answer.verdict != "spent";
    fd.validation_upheld = ev.validation_answer.verdict != "refused";
    if (ev.confirmation) fd.confirmation_tested = true, fd.confirmed = ev.confirmation->confirmed, fd.opposite = ev.confirmation->opposite;
    if (ev.causal.applicable && ev.causal.verdict != evidence::Verdict::undecided) fd.qualifiers.push_back(std::string("causal support ") + evidence::name(ev.causal.verdict) + ": " + ev.causal.why);
    if (ev.simulation.applicable && ev.simulation.verdict != evidence::Verdict::undecided) fd.qualifiers.push_back(std::string("simulation ") + evidence::name(ev.simulation.verdict) + ": " + ev.simulation.why);
    if (ev.rules.applicable && ev.rules.verdict != evidence::Verdict::undecided) fd.qualifiers.push_back(std::string("rules ") + evidence::name(ev.rules.verdict) + ": " + ev.rules.why);
    // Its readings, the causal graphs and the rivals (ADR 0126).
    const auto io = interpret_options();
    ev.interpretation = evidence::interpret(*d_, s, *parts_, Part::discovery, io);
    ev.causal_readings = evidence::interpret_causal(*d_, s, *parts_, Part::discovery);
    for (const auto& r : d_->rivals) ev.rivals.push_back(evidence::rival(*d_, s, r, *parts_, Part::discovery, io));
    if (ev.interpretation.applies) {
      const auto& c = ev.interpretation.construal;
      fd.untenable = c.record == apodeixis::interpretation::Class::untenable;
      if (c.record == apodeixis::interpretation::Class::arguable) {
        char b[120];
        std::snprintf(b, sizeof b, "arguable under its readings: the gap clears the threshold under %.0f%% of the admissible ones", 100 * c.share);
        fd.qualifiers.push_back(b);
      }
    }
    if (ev.causal_readings.applies && ev.causal_readings.construal.record == apodeixis::interpretation::Class::arguable)
      fd.qualifiers.push_back("causal support depends on the graph: it holds under some admissible graphs, not all");
    for (std::size_t k = 0; k < ev.rivals.size(); k++) {
      const auto& r = ev.rivals[k];
      const bool proposed = d_->rivals[k].source == "proposal";  // a model's rival qualifies a statement, never defeats it
      if (r.explains == tekmerion::construal::Explains::away && !proposed) fd.explained_by.push_back(r.claim);
      else if (r.explains != tekmerion::construal::Explains::no)
        fd.qualifiers.push_back((proposed ? "as proposed, " : "") + r.claim + " " + tekmerion::construal::name(r.explains) + ": " + r.reason);
    }
    ev.standing = evidence::standing(fd);
    return ev;
  }

  double spread_of(const Statement& s) const {
    if (s.standard == Standard::expressed) return 1.0;
    auto it = space_->spread.find(s.indicator);
    return it == space_->spread.end() || !(it->second > 0) ? 1.0 : it->second;
  }

 private:
  void init(Dataset d) {
    for (const auto& name : o_.objectives) {
      static const std::set<std::string> known{"gap", "effect", "coverage", "size", "quality", "simplicity", "stability", "causal", "strata"};
      if (!known.count(name)) throw Refusal("no objective '" + name + "' (there are gap, effect, coverage, size, quality, simplicity, stability, causal, strata)");
    }
    if (o_.objectives.empty()) throw Refusal("a search needs at least one objective");
    d_ = std::make_shared<const Dataset>(std::move(d));
    evidence::check_declarations(*d_);
    parts_ =std::make_shared<const Parts>(evidence::split(*d_, o_.fractions, o_.key));
    if (parts_->members[0].size() < 2) throw Refusal("the discovery part has fewer than two clusters");
    space_ = std::make_shared<const Space>(search::space(*d_, *parts_, o_.space));
    rules_ = std::make_shared<const evidence::Rulebook>(evidence::rulebook(*d_));
    strata_ = evidence::strata(*parts_, Part::discovery, o_.strata, o_.key);
    // People of the discovery part (the largest indicator's total).
    for (const auto& ind : space_->indicators) people_ = std::max(people_, total_people(ind));
    auto var = std::make_shared<Variation>();
    var->d = d_.get(), var->parts = parts_.get(), var->sp = space_.get();
    var_ = var;
    gen_ = std::make_shared<Generators>();
    gen_->d = d_.get(), gen_->parts = parts_.get(), gen_->sp = space_.get(), gen_->rules = rules_.get(), gen_->key = o_.key;
    gen_->bfs = saved_.bfs.empty() ? std::vector<std::size_t>{1} : saved_.bfs;
    gen_->dfs = saved_.dfs, gen_->handed = saved_.handed;
    if (o_.simulate && !d_->suppliers.empty() && !d_->place.empty())
      for (const auto& ind : space_->indicators) {
        auto f = evidence::simulate(*d_, space_->domain(ind), ind, o_.seeking);
        if (f.applicable) gen_->fields[ind] = std::move(f);
      }
    // The holdout, and the online rule replayed.
    if (saved_.holdout_started) holdout_ = std::make_unique<tyche::testing::Thresholdout>(o_.holdout, holdout_stream_, saved_.holdout);
    else holdout_ = std::make_unique<tyche::testing::Thresholdout>(o_.holdout, holdout_stream_);
    online_ = std::make_unique<tyche::testing::Online>(tyche::testing::Online::Options{o_.rule, o_.alpha});
    for (double v : saved_.online) online_->test(v);
    // The evolution.
    hev::Judge<Statement> judge;
    judge.admit = [this](const Member& m, const Context& ctx) { return admit(m, ctx); };
    judge.recheck = [this](const hev::Kept<Statement>& k, const Context& ctx) { return recheck(k, ctx); };
    auto ops = build_operators();
    if (saved_.evolution.generation > 0 || !saved_.evolution.population.empty())
      evo_ = std::make_unique<hev::Evolution<Statement>>(std::move(ops), o_.strategy, judge, saved_.evolution, o_.key);
    else
      evo_ = std::make_unique<hev::Evolution<Statement>>(std::move(ops), o_.strategy, judge, o_.key);
  }

  double total_people(const std::string& ind) const {
    Statement st;
    st.indicator = ind, st.domain = space_->domain(ind), st.standard = Standard::normative, st.level = 0, st.worse_when_higher = d_->indicator(ind).higher_is_worse;
    const auto T = evidence::totals(*d_, st, *parts_, Part::discovery);
    double x = 0;
    for (double v : T.xg) x += v;
    return x;
  }

  std::size_t domain_index(const std::string& dom) const {
    for (std::size_t k = 0; k < space_->domains.size(); k++)
      if (space_->domains[k] == dom) return k;
    return space_->domains.size();
  }

  double weight(const std::string& name) const {
    auto it = o_.operators.find(name);
    return it == o_.operators.end() ? 1.0 : it->second;
  }

  hev::Operators<Statement> build_operators() {
    hev::Operators<Statement> ops;
    auto gen = gen_;
    for (const auto& n : generator_names()) {
      if (weight(n) <= 0) continue;
      if (n == "rules" && rules_->rules.empty()) continue;
      if (n == "simulation" && gen_->fields.empty()) continue;
      ops.makers.push_back({n, [gen, n](std::size_t k, const Context&, calx::random::Stream& s) { return gen->make(n, k, s); }, weight(n)});
    }
    std::shared_ptr<const Variation> v = var_;
    using M = std::function<std::optional<Statement>(const Variation&, const Statement&, calx::random::Stream&)>;
    const std::map<std::string, M> mutations{
        {"add", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.add(s, r); }},
        {"drop", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.drop(s, r); }},
        {"shift", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.shift(s, r); }},
        {"flip", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.flip(s, r); }},
        {"widen", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.widen(s, r); }},
        {"narrow", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.narrow(s, r); }},
        {"swap", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.swap(s, r); }},
        {"indicator", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.indicator(s, r); }},
        {"standard", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.standard(s, r); }},
        {"frame", [](const Variation& x, const Statement& s, calx::random::Stream& r) { return x.frame(s, r); }},
    };
    for (const auto& n : mutation_names()) {
      if (weight(n) <= 0) continue;
      const M f = mutations.at(n);
      ops.mutations.push_back({n, [v, f](const Member& m, const Context&, calx::random::Stream& r) { return f(*v, m.genotype, r); }, weight(n)});
    }
    using X = std::function<std::optional<Statement>(const Variation&, const Statement&, const Statement&, calx::random::Stream&)>;
    const std::map<std::string, X> crossovers{
        {"join", [](const Variation& x, const Statement& a, const Statement& b, calx::random::Stream&) { return x.join(a, b); }},
        {"meet", [](const Variation& x, const Statement& a, const Statement& b, calx::random::Stream&) { return x.meet(a, b); }},
        {"one-point", [](const Variation& x, const Statement& a, const Statement& b, calx::random::Stream& r) { return x.one_point(a, b, r); }},
        {"uniform", [](const Variation& x, const Statement& a, const Statement& b, calx::random::Stream& r) { return x.uniform(a, b, r); }},
        {"graft", [](const Variation& x, const Statement& a, const Statement& b, calx::random::Stream&) { return x.graft(a, b); }},
    };
    for (const auto& n : crossover_names()) {
      if (weight(n) <= 0) continue;
      const X f = crossovers.at(n);
      ops.crossovers.push_back({n, [v, f](const Member& a, const Member& b, const Context&, calx::random::Stream& r) { return f(*v, a.genotype, b.genotype, r); }, weight(n)});
    }
    for (const auto& [n, w] : o_.operators) {
      bool known = std::find(generator_names().begin(), generator_names().end(), n) != generator_names().end() ||
                   std::find(mutation_names().begin(), mutation_names().end(), n) != mutation_names().end() ||
                   std::find(crossover_names().begin(), crossover_names().end(), n) != crossover_names().end();
      if (!known) throw Refusal("no operator '" + n + "'");
      (void)w;
    }
    if (ops.makers.empty()) throw Refusal("a search needs at least one generator");
    ops.distance = [v](const Statement& a, const Statement& b) { return v->distance(a, b); };
    ops.key = [](const Statement& s) { return s.key(); };
    return ops;
  }

  // The archive's judge: discovery, effect and stability first; then the validation part, through the reusable holdout.
  std::optional<std::string> admit(const Member& m, const Context& ctx) {
    const Statement& s = m.genotype;
    const auto T = evidence::totals(*d_, s, *parts_, Part::discovery);
    const auto g = evidence::estimate(T, o_.inference);
    if (!g.defined || g.z < o_.z_admit) return std::nullopt;
    if (g.clusters < o_.min_clusters || g.coverage < space_->min_coverage || size(s) > space_->max_conditions) return std::nullopt;  // the constraints
    const double spread = spread_of(s), eff = g.estimate / spread;
    if (eff < o_.min_effect) return std::nullopt;
    if (!productive(s)) return std::nullopt;
    const auto st = evidence::stability(T, o_.stability_threshold, o_.stability_resamples, o_.key);
    if (!(st.share >= o_.stability_min)) return std::nullopt;
    // Its readings and rivals, on discovery, before any query of the validation part is spent on it.
    std::string reading;
    double share = evidence::nan;
    if (o_.interpret) {
      const auto io = interpret_options();
      const auto c = evidence::interpret(*d_, s, *parts_, Part::discovery, io);
      if (c.applies) {
        using apodeixis::interpretation::Class;
        const Class cls = c.construal.record;
        if (cls == Class::untenable && o_.refuse_untenable) return std::nullopt;
        if (cls != Class::clear && o_.admit_clear_only) return std::nullopt;
        reading = std::string(apodeixis::interpretation::word(cls)), share = c.construal.share;
      }
      if (o_.refuse_explained)
        for (const auto& r : d_->rivals)
          if (r.source != "proposal" && evidence::rival(*d_, s, r, *parts_, Part::discovery, io).explains == tekmerion::construal::Explains::away) return std::nullopt;
    }
    const std::string k = s.key();
    Validation v;
    bool asked_before = false;
    for (const auto& [key, val] : saved_.validated)
      if (key == k) v = val, asked_before = true;
    if (!asked_before) {
      const auto gv = evidence::estimate(evidence::totals(*d_, s, *parts_, Part::validation), o_.inference);
      // The effect, clipped to [−1, 1]: past a whole spread the difference between the parts does not bear on admission.
      const double dv = gv.defined ? std::clamp(gv.estimate / spread, -1.0, 1.0) : -1.0;
      holdout_stream_ = calx::random::Stream(o_.key, static_cast<std::uint32_t>(holdout_->queries()), 0x401d, 0);
      holdout_->use(holdout_stream_);
      const auto a = holdout_->ask(std::clamp(eff, -1.0, 1.0), dv);
      if (a.spent) v.verdict = "spent";
      else {
        v.value = a.value;
        v.verdict = a.value < o_.min_effect ? "refused" : a.overfit ? "told" : "agreed";
      }
      saved_.validated.push_back({k, v});
    }
    if (v.verdict == "refused") return std::nullopt;
    // Distinct needs: a statement close to one already kept (in whom it concerns and what it says) and no stronger is not kept.
    // (The productivity of its conditions was asked before the validation part, below, so that no query is spent on a refinement
    // that adds nothing.)
    if (const auto* near = nearest_as_strong(s, g.z, ctx)) {
      (void)near;
      return std::nullopt;
    }
    Admission ad;
    ad.ticket = m.ticket, ad.key = k, ad.text = s.write(), ad.generation = evo_ ? evo_->generation() : 0, ad.z = g.z, ad.effect = eff, ad.stability = st.share;
    ad.validation = v, ad.data_version = o_.data_version, ad.reading = reading, ad.share = share;
    char note[240];
    std::snprintf(note, sizeof note, "z %.2f, effect %.2f, stable in %.0f%% of resamples, validation %s", g.z, eff, 100 * st.share, v.verdict.c_str());
    ad.note = note;
    if (!reading.empty()) {
      std::snprintf(note, sizeof note, ", %s under its readings (%.0f%%)", reading.c_str(), 100 * share);
      ad.note += note;
    }
    saved_.admissions.push_back(ad);
    return ad.note;
  }

  // Every condition earns its place (after Webb 2007, Machine Learning 68, 1–33, on productive rules): within the group with that
  // condition removed, those who meet it have a gap over those who do not, by the indicator's direction, with z at least z_p on
  // discovery. A refinement that adds a condition carrying no gap of its own describes the same need with more words. A frame's
  // condition earns its place alike: among the group's people within the rest of the frame, those who meet it are worse off than
  // those who do not (else the need is the general one, restricted).
  bool productive(const Statement& s) const {
    if (!(o_.z_productive > 0) || s.standard == Standard::expressed) return true;
    auto earns = [&](const Condition& c, const Group& within) {
      Statement r = s;
      r.standard = Standard::comparative, r.worse_when_higher = d_->indicator(s.indicator).higher_is_worse;
      r.level = std::numeric_limits<double>::quiet_NaN(), r.reference.reset();
      r.frame = within, r.group = Group{{c}};
      const auto g = evidence::estimate(evidence::totals(*d_, r, *parts_, Part::discovery), o_.inference);
      return g.defined && g.z >= o_.z_productive;
    };
    auto without = [](const Group& g, std::size_t c) {
      Group rest;
      for (std::size_t q = 0; q < g.conditions.size(); q++)
        if (q != c) rest.conditions.push_back(g.conditions[q]);
      return rest;
    };
    if (s.group.conditions.size() >= 2)
      for (std::size_t c = 0; c < s.group.conditions.size(); c++)
        if (!earns(s.group.conditions[c], groups::join(s.frame, without(s.group, c)))) return false;
    for (std::size_t c = 0; c < s.frame.conditions.size(); c++)
      if (!earns(s.frame.conditions[c], groups::join(without(s.frame, c), s.group))) return false;
    return true;
  }

  double admitted_z(const std::string& key) const {
    for (auto it = saved_.admissions.rbegin(); it != saved_.admissions.rend(); ++it)
      if (it->key == key) return it->z;
    return -std::numeric_limits<double>::infinity();
  }

  // A kept statement within the least distance of s whose gap is at least as strong (or, with `strictly`, stronger), if any.
  const hev::Kept<Statement>* nearest_as_strong(const Statement& s, double z, const Context& ctx, bool strictly = false) const {
    if (!(o_.min_distance > 0)) return nullptr;
    const std::string self = s.key();
    for (const auto& k : ctx.archive) {
      if (k.member.genotype.key() == self) continue;
      if (var_->distance(s, k.member.genotype) >= o_.min_distance) continue;
      const double other = admitted_z(k.member.genotype.key());
      if (strictly ? other > z : other >= z) return &k;
    }
    return nullptr;
  }

  // Retirement on recheck: superseded by a stronger statement kept within the least distance; or, when the data have changed since
  // admission, no longer clearing half of z* on discovery.
  std::optional<std::string> recheck(const hev::Kept<Statement>& k, const Context& ctx) const {
    const std::string key = k.member.genotype.key();
    if (const auto* better = nearest_as_strong(k.member.genotype, admitted_z(key), ctx, true))
      return "superseded by a stronger statement of nearly the same people: " + better->member.genotype.write();
    for (const auto& a : saved_.admissions)
      if (a.key == key && a.data_version != o_.data_version) {
        const auto g = evidence::estimate(evidence::totals(*d_, k.member.genotype, *parts_, Part::discovery), o_.inference);
        if (!g.defined || g.z < o_.z_admit / 2)
          return "no longer upheld by the data of version " + o_.data_version + (g.defined ? " (z " + std::to_string(g.z).substr(0, 5) + ")" : ": " + g.why);
      }
    return std::nullopt;
  }

  // Every statement admitted and not yet confirmed, confirmed once, in the order of admission.
  void confirm_new() {
    for (const auto& a : saved_.admissions) {
      bool done = false;
      for (const auto& c : saved_.confirmations) done = done || c.key == a.key;
      if (done) continue;
      Statement s = language::read_one(a.text);
      language::bind(*d_, s);
      Confirmation c;
      c.serial = saved_.confirmations.size() + 1, c.ticket = a.ticket, c.key = a.key, c.text = a.text, c.generation = a.generation;
      c.level = online_->next_level();
      char reg[240];
      std::snprintf(reg, sizeof reg, "test %zu: the gap is positive, one-sided, on the confirmation part; rule %s at α = %g; level %.6g%s", c.serial,
                    tyche::testing::name(o_.rule), o_.alpha, c.level, o_.rule == tyche::testing::OnlineRule::e_lond ? " (an e-value must reach its inverse)" : "");
      saved_.journal.append({o_.started, o_.agent, "register", a.key, reg, ""});
      const auto g = evidence::estimate(evidence::totals(*d_, s, *parts_, Part::confirmation), o_.inference);
      c.defined = g.defined, c.estimate = g.estimate, c.low = g.low, c.high = g.high, c.z = g.z, c.coverage = g.coverage;
      c.p = g.defined ? std::clamp(g.p, 0.0, 1.0) : 1.0;
      if (!g.defined) c.why = g.why;
      c.e = tyche::testing::calibrate_mixture(c.p);
      const double value = o_.rule == tyche::testing::OnlineRule::e_lond ? c.e : c.p;
      const auto dec = online_->test(value);
      saved_.online.push_back(value);
      c.confirmed = dec.rejected;
      c.opposite = g.defined && g.high < 0;
      char res[240];
      std::snprintf(res, sizeof res, "estimate %.6g [%.6g, %.6g], z %.3f, p %.3g, e %.3g: %s", c.estimate, c.low, c.high, c.z, c.p, c.e,
                    c.confirmed ? "confirmed" : c.opposite ? "the opposite gap" : "not confirmed");
      c.seal = saved_.journal.append({o_.started, o_.agent, "confirm", a.key, res, ""}).seal;
      saved_.confirmations.push_back(c);
    }
  }

  Options o_;
  Saved saved_;
  std::shared_ptr<const Dataset> d_;
  std::shared_ptr<const Parts> parts_;
  std::shared_ptr<const Space> space_;
  std::shared_ptr<const evidence::Rulebook> rules_;
  std::shared_ptr<const Variation> var_;
  std::shared_ptr<Generators> gen_;
  std::vector<int> strata_;
  double people_ = 0;
  calx::random::Stream holdout_stream_{0, 0, 0, 0};
  std::unique_ptr<tyche::testing::Thresholdout> holdout_;
  std::unique_ptr<tyche::testing::Online> online_;
  std::unique_ptr<hev::Evolution<Statement>> evo_;
};

}  // namespace needs::search
