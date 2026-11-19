// The search for theories of the law (ADR 0133): doctrines made, varied and crossed by heuresis's evolution, judged by fit and
// justification (doctrine.hpp), kept by kind, and tested once on the decisions held back.
//
// THE LOOP, a generation at a time:
//   1. children are made by the makers (the result model with authority's readings; each reading of a term with the narrowest ratios;
//      at random), the mutations (a term read otherwise; a factor taken into or out of a ratio; a ratio made the case's whole outcome or
//      one factor of it) and the crossovers (term by term and ratio by ratio; the readings of one with the ratios of another), or are
//      proposed from outside (a party, a person, a language model), as heuresis's schedule chooses and credits them;
//   2. each is assessed (doctrine.hpp): its score the declared combination of misfit, authority, coherence and justification, its kind
//      what it makes of the matter;
//   3. the archive keeps, of each kind, the best doctrines (two by default), a doctrine within a least distance of a better one of its
//      kind not kept (MAP-Elites's idea, Mouret and Clune 2015): the best reading of the law on which the matter's question follows is
//      found beside the best on which it does not;
//   4. every doctrine kept is tested once on the held-back decisions, the test sealed in archeion's journal before they are read.
//      Nothing of the test reaches the search: not the score, not the archive, not the credit of the operators.
//
// A MODEL'S PART, as in the searches for needs and for theories of the case: requests() says what to ask a language model for, in
// four roles (seed: new doctrines; mutate: a variation of one; cross: one from two; reflect: a refinement of a kept one from its
// fit), each role's share of a budget its proposals' survival, with the doctrines to start from and their assessment. What a model
// writes is read by read() like anyone's, refused line by line when it does not read, and proposed under "model:<role>" with the
// doctrines it came from as parents. It is never shown a held-back decision.
#pragma once

#include <law/doctrine/doctrine.hpp>

#include <heuresis/evolution/evolution.hpp>

#include <algorithm>
#include <memory>
#include <string>
#include <vector>

namespace law::doctrine {

namespace hev = heuresis::evolution;
using heuresis::search::Ticket;

struct SearchOptions {
  hev::Strategy strategy;
  std::size_t capacity = 2;                  // doctrines kept of each kind
  double min_distance = 0.15;                // one this close to a better one of its kind is not kept
  double tolerance = 0.5;                    // of the best score, for the standings
  std::uint64_t key = 20261119;
  std::map<std::string, double> operators;   // weights by name; absent 1, zero off
  std::string when = "2026-01-01T00:00:00Z", who = "the doctrine search";

  SearchOptions() {
    strategy.mu = 40, strategy.lambda = 40, strategy.generations = 20, strategy.survival = hev::Survival::plus;
    strategy.generator_rate = 0.2, strategy.crossover_rate = 0.3, strategy.from_archive = 0.2;
    strategy.retire_after = 1000, strategy.archive_capacity = 200;
  }
};

inline const std::vector<std::string>& maker_names() {
  static const std::vector<std::string> n{"authority", "narrow", "random"};
  return n;
}
inline const std::vector<std::string>& mutation_names() {
  static const std::vector<std::string> n{"reread", "toggle", "whole", "single"};
  return n;
}
inline const std::vector<std::string>& crossover_names() {
  static const std::vector<std::string> n{"uniform", "graft"};
  return n;
}
inline const std::vector<std::string>& model_roles() {
  static const std::vector<std::string> r{"seed", "mutate", "cross", "reflect"};
  return r;
}

struct Request {
  std::string role;                          // seed, mutate, cross, reflect
  std::size_t count = 1;
  std::vector<Ticket> parents;
  std::vector<std::string> texts, notes;     // the parents' written forms, and their assessment in words
  double credit = 0.5;
};

// The distance between two doctrines: the share of terms read otherwise, and the mean Jaccard distance of their chosen ratios.
inline double distance(const Problem& p, const Doctrine& a, const Doctrine& b) {
  double parts = 0, n = 0;
  for (std::size_t k = 0; k < a.readings.size() && k < b.readings.size(); k++) parts += a.readings[k] != b.readings[k], n++;
  for (std::size_t x = 0; x < p.bases.size(); x++)
    for (std::size_t c = 0; c < p.bases[x].base.cases.size(); c++) {
      if (!open_ratio(p.bases[x], p.bases[x].base.cases[c])) continue;
      const auto& r = a.ratios[x][c];
      const auto& s = b.ratios[x][c];
      Factors u = r, i;
      u.insert(s.begin(), s.end());
      std::set_intersection(r.begin(), r.end(), s.begin(), s.end(), std::inserter(i, i.begin()));
      parts += u.empty() ? 0 : 1.0 - static_cast<double>(i.size()) / static_cast<double>(u.size()), n++;
    }
  return n > 0 ? parts / n : 0;
}

class Search {
 public:
  Search(Problem p, SearchOptions o = {}) : p_(std::move(p)), o_(std::move(o)) { init(nullptr); }
  Search(Problem p, SearchOptions o, const hev::State<Doctrine>& state, Confirmer confirmer = {}) : p_(std::move(p)), o_(std::move(o)), confirmer_(std::move(confirmer)) {
    init(&state);
  }
  Search(const Search&) = delete;
  Search& operator=(const Search&) = delete;

  const Problem& problem() const { return p_; }
  const SearchOptions& options() const { return o_; }
  const hev::Evolution<Doctrine>& evolution() const { return *evo_; }
  const Confirmer& confirmer() const { return confirmer_; }

  void propose(const std::vector<Doctrine>& ds, const std::string& source, const std::vector<std::vector<Ticket>>& parents = {}) { evo_->propose(ds, source, parents); }

  bool step() {
    if (evo_->stopped()) return false;
    const std::size_t before = evo_->generation();
    evo_->step([&](const Doctrine& d) { return evaluate(d); });
    for (const auto& k : evo_->archive()) confirmer_.confirm(p_, k.member.ticket, k.member.genotype, o_.when, o_.who);
    return evo_->generation() != before;
  }
  void run(std::size_t generations) {
    for (std::size_t k = 0; k < generations && step(); k++) {
    }
  }

  heuresis::search::Evaluation evaluate(const Doctrine& d) const {
    const Assessment a = assess(p_, d);
    heuresis::search::Evaluation e;
    e.objectives = {a.score, a.misfit, static_cast<double>(a.conflicts.size()), -a.justification};
    e.descriptors = kind(p_, a);
    return e;
  }

  // The doctrines kept, best first, with their assessments.
  std::vector<std::pair<const hev::Kept<Doctrine>*, Assessment>> kept() const {
    std::vector<std::pair<const hev::Kept<Doctrine>*, Assessment>> out;
    for (const auto& k : evo_->archive()) out.push_back({&k, assess(p_, k.member.genotype)});
    std::stable_sort(out.begin(), out.end(), [](const auto& x, const auto& y) { return x.second.score < y.second.score; });
    return out;
  }

  std::vector<Standing> standing() const {
    std::vector<Assessment> as;
    for (const auto& [k, a] : kept()) as.push_back(a);
    return standings(p_, as, o_.tolerance);
  }
  std::vector<FactStanding> fact_standing() const {
    std::vector<Assessment> as;
    for (const auto& [k, a] : kept()) as.push_back(a);
    return fact_standings(p_, as, o_.tolerance);
  }

  // What the discovery side says of a doctrine, in words, for a model's prompt (no held-back decision).
  std::string note(const Doctrine& d) const {
    const auto a = assess(p_, d);
    std::string s = a.summary + "; " + kind_words(p_, kind(p_, a));
    std::string wrong;
    for (const auto& c : a.cases)
      if (c.force != authority::Force::left_out && !c.agrees) wrong += (wrong.empty() ? "" : ", ") + c.id;
    if (!wrong.empty()) s += "; gets wrong: " + wrong;
    for (const auto& c : a.conflicts) s += "; conflict " + c;
    return s;
  }

  std::vector<Request> requests(std::size_t budget, const std::vector<std::string>& roles = model_roles()) const {
    std::vector<Request> out;
    if (budget == 0) return out;
    std::vector<std::string> use;
    for (const auto& r : roles) {
      if (std::find(model_roles().begin(), model_roles().end(), r) == model_roles().end()) throw std::invalid_argument("law::doctrine: no role '" + r + "' for a model (there are seed, mutate, cross, reflect)");
      if ((r == "mutate" || r == "cross") && evo_->population().size() < (r == "cross" ? 2u : 1u)) continue;
      if (r == "reflect" && evo_->archive().empty()) continue;
      use.push_back(r);
    }
    if (use.empty()) use.push_back("seed");
    std::vector<double> credit;
    double total = 0;
    for (const auto& r : use) {
      double born = 0, survived = 0;
      for (const auto& st : evo_->statistics()) {
        if (auto it = st.proposals.find("model:" + r); it != st.proposals.end()) born += static_cast<double>(it->second);
        if (auto it = st.proposals_surviving.find("model:" + r); it != st.proposals_surviving.end()) survived += static_cast<double>(it->second);
      }
      credit.push_back((1 + survived) / (2 + born)), total += credit.back();
    }
    std::vector<std::size_t> n(use.size(), 0);
    std::vector<std::pair<double, std::size_t>> rem;
    std::size_t given = 0;
    for (std::size_t k = 0; k < use.size(); k++) {
      const double share = static_cast<double>(budget) * credit[k] / total;
      n[k] = static_cast<std::size_t>(std::floor(share)), given += n[k], rem.push_back({share - std::floor(share), k});
    }
    std::stable_sort(rem.begin(), rem.end(), [](const auto& a, const auto& b) { return a.first > b.first; });
    for (std::size_t j = 0; given < budget; j = (j + 1) % rem.size()) n[rem[j].second]++, given++;
    std::vector<const hev::Member<Doctrine>*> ranked;
    for (const auto& m : evo_->population()) ranked.push_back(&m);
    std::stable_sort(ranked.begin(), ranked.end(), [](const auto* a, const auto* b) {
      const double x = a->evaluation.objectives.empty() ? INFINITY : a->evaluation.objectives[0];
      const double y = b->evaluation.objectives.empty() ? INFINITY : b->evaluation.objectives[0];
      return x < y;
    });
    std::vector<const hev::Kept<Doctrine>*> kept_;
    for (const auto& k : evo_->archive()) kept_.push_back(&k);
    std::size_t next = 0, next_kept = 0;
    const auto with = [&](Request& q, const Doctrine& d, Ticket t) { q.parents.push_back(t), q.texts.push_back(write(p_, d)), q.notes.push_back(note(d)); };
    for (std::size_t k = 0; k < use.size(); k++) {
      if (n[k] == 0) continue;
      if (use[k] == "seed") {
        Request q;
        q.role = "seed", q.count = n[k], q.credit = credit[k];
        out.push_back(std::move(q));
        continue;
      }
      for (std::size_t j = 0; j < n[k]; j++) {
        Request q;
        q.role = use[k], q.credit = credit[k];
        if (use[k] == "reflect") {
          const auto* kp = kept_[next_kept++ % kept_.size()];
          with(q, kp->member.genotype, kp->member.ticket);
        } else {
          for (std::size_t c = 0; c < (use[k] == "cross" ? 2u : 1u); c++) {
            const auto* m = ranked[next++ % ranked.size()];
            with(q, m->genotype, m->ticket);
          }
        }
        out.push_back(std::move(q));
      }
    }
    return out;
  }

 private:
  double weight(const std::string& n) const {
    auto it = o_.operators.find(n);
    return it == o_.operators.end() ? 1.0 : it->second;
  }

  // The slots of the doctrines' ratios: (base, case) with the factors the case had for its outcome.
  struct Slot {
    std::size_t base, cas;
    std::vector<std::string> factors;
  };

  Doctrine random_doctrine(calx::random::Stream& s) const {
    Doctrine d = plain(p_);
    for (std::size_t k = 0; k < p_.matter.points.size(); k++) d.readings[k] = s.below(p_.matter.points[k].readings.size());
    for (const auto& sl : slots_) {
      Factors r;
      while (r.empty())
        for (const auto& f : sl.factors)
          if (s.uniform() < 0.5) r.insert(f);
      d.ratios[sl.base][sl.cas] = r;
    }
    return d;
  }

  hev::Operators<Doctrine> operators() {
    hev::Operators<Doctrine> ops;
    using Stream = calx::random::Stream;
    using Ctx = hev::Context<Doctrine>;
    using Member = hev::Member<Doctrine>;
    // Authority's readings where they fix one, the result model's ratios.
    if (weight("authority") > 0)
      ops.makers.push_back({"authority", [this](std::size_t n, const Ctx&, Stream& s) {
                              std::vector<Doctrine> out;
                              for (std::size_t i = 0; i < n; i++) {
                                Doctrine d = plain(p_);
                                // Draw among the admissible interpretations.
                                std::vector<std::size_t> ok;
                                for (std::size_t x = 0; x < p_.admissible.size(); x++)
                                  if (p_.admissible[x]) ok.push_back(x);
                                if (!ok.empty()) d.readings = ai::decode(p_.matter.points, ok[s.below(ok.size())]);
                                else
                                  for (std::size_t k = 0; k < p_.matter.points.size(); k++) d.readings[k] = s.below(p_.matter.points[k].readings.size());
                                out.push_back(d);
                              }
                              return out;
                            },
                            weight("authority")});
    // Each ratio one factor (the narrowest reasons, the broadest constraint).
    if (weight("narrow") > 0)
      ops.makers.push_back({"narrow", [this](std::size_t n, const Ctx&, Stream& s) {
                              std::vector<Doctrine> out;
                              for (std::size_t i = 0; i < n; i++) {
                                Doctrine d = random_doctrine(s);
                                for (const auto& sl : slots_) d.ratios[sl.base][sl.cas] = {sl.factors[s.below(sl.factors.size())]};
                                out.push_back(d);
                              }
                              return out;
                            },
                            weight("narrow")});
    if (weight("random") > 0)
      ops.makers.push_back({"random", [this](std::size_t n, const Ctx&, Stream& s) {
                              std::vector<Doctrine> out;
                              for (std::size_t i = 0; i < n; i++) out.push_back(random_doctrine(s));
                              return out;
                            },
                            weight("random")});
    if (ops.makers.empty()) throw std::invalid_argument("law::doctrine: a search needs at least one maker");
    const std::size_t points = p_.matter.points.size();
    if (weight("reread") > 0 && points > 0)
      ops.mutations.push_back({"reread", [this, points](const Member& m, const Ctx&, Stream& s) -> std::optional<Doctrine> {
                                 Doctrine d = m.genotype;
                                 const std::size_t k = s.below(points);
                                 const std::size_t n = p_.matter.points[k].readings.size();
                                 if (n < 2) return std::nullopt;
                                 d.readings[k] = (d.readings[k] + 1 + s.below(n - 1)) % n;
                                 return d;
                               },
                               weight("reread")});
    if (!slots_.empty()) {
      if (weight("toggle") > 0)
        ops.mutations.push_back({"toggle", [this](const Member& m, const Ctx&, Stream& s) -> std::optional<Doctrine> {
                                   Doctrine d = m.genotype;
                                   const auto& sl = slots_[s.below(slots_.size())];
                                   auto& r = d.ratios[sl.base][sl.cas];
                                   const std::string f = sl.factors[s.below(sl.factors.size())];
                                   if (r.count(f)) {
                                     if (r.size() == 1) return std::nullopt;
                                     r.erase(f);
                                   } else {
                                     r.insert(f);
                                   }
                                   return d;
                                 },
                                 weight("toggle")});
      if (weight("whole") > 0)
        ops.mutations.push_back({"whole", [this](const Member& m, const Ctx&, Stream& s) -> std::optional<Doctrine> {
                                   Doctrine d = m.genotype;
                                   const auto& sl = slots_[s.below(slots_.size())];
                                   d.ratios[sl.base][sl.cas] = Factors(sl.factors.begin(), sl.factors.end());
                                   return d;
                                 },
                                 weight("whole")});
      if (weight("single") > 0)
        ops.mutations.push_back({"single", [this](const Member& m, const Ctx&, Stream& s) -> std::optional<Doctrine> {
                                   Doctrine d = m.genotype;
                                   const auto& sl = slots_[s.below(slots_.size())];
                                   d.ratios[sl.base][sl.cas] = {sl.factors[s.below(sl.factors.size())]};
                                   return d;
                                 },
                                 weight("single")});
    }
    if (weight("uniform") > 0)
      ops.crossovers.push_back({"uniform", [this, points](const Member& a, const Member& b, const Ctx&, Stream& s) -> std::optional<Doctrine> {
                                  Doctrine d = a.genotype;
                                  for (std::size_t k = 0; k < points; k++)
                                    if (s.uniform() < 0.5) d.readings[k] = b.genotype.readings[k];
                                  for (const auto& sl : slots_)
                                    if (s.uniform() < 0.5) d.ratios[sl.base][sl.cas] = b.genotype.ratios[sl.base][sl.cas];
                                  return d;
                                },
                                weight("uniform")});
    if (weight("graft") > 0)
      ops.crossovers.push_back({"graft", [](const Member& a, const Member& b, const Ctx&, Stream&) -> std::optional<Doctrine> {
                                  Doctrine d = a.genotype;
                                  d.ratios = b.genotype.ratios;
                                  return d;
                                },
                                weight("graft")});
    for (const auto& [n, w] : o_.operators) {
      (void)w;
      const auto known = [&](const std::vector<std::string>& v) { return std::find(v.begin(), v.end(), n) != v.end(); };
      if (!known(maker_names()) && !known(mutation_names()) && !known(crossover_names())) throw std::invalid_argument("law::doctrine: no operator '" + n + "'");
    }
    ops.distance = [this](const Doctrine& a, const Doctrine& b) { return distance(p_, a, b); };
    ops.key = [this](const Doctrine& d) { return key(p_, d); };
    return ops;
  }

  hev::Judge<Doctrine> judge() const {
    hev::Judge<Doctrine> j;
    j.admit = [this](const hev::Member<Doctrine>& m, const hev::Context<Doctrine>& ctx) -> std::optional<std::string> {
      if (!m.evaluation.feasible()) return std::nullopt;
      const double score = m.evaluation.objectives[0];
      std::vector<const hev::Kept<Doctrine>*> same;
      for (const auto& k : ctx.archive)
        if (k.member.evaluation.descriptors == m.evaluation.descriptors) same.push_back(&k);
      for (const auto* k : same)
        if (k->member.evaluation.objectives[0] <= score && distance(p_, k->member.genotype, m.genotype) < o_.min_distance) return std::nullopt;
      const std::string where = kind_words(p_, m.evaluation.descriptors);
      if (same.size() < o_.capacity) return "kept: the " + std::string(same.empty() ? "first" : "next") + " doctrine of its kind (" + where + ")";
      const hev::Kept<Doctrine>* worst = nullptr;
      for (const auto* k : same)
        if (!worst || k->member.evaluation.objectives[0] > worst->member.evaluation.objectives[0]) worst = k;
      if (worst && score < worst->member.evaluation.objectives[0])
        return "kept: better than #" + std::to_string(heuresis::search::serial_of(worst->member.ticket)) + " of its kind (" + where + ")";
      return std::nullopt;
    };
    j.recheck = [this](const hev::Kept<Doctrine>& k, const hev::Context<Doctrine>& ctx) -> std::optional<std::string> {
      std::vector<const hev::Kept<Doctrine>*> same;
      for (const auto& x : ctx.archive)
        if (x.member.evaluation.descriptors == k.member.evaluation.descriptors) same.push_back(&x);
      std::stable_sort(same.begin(), same.end(), [](const auto* a, const auto* b) { return a->member.evaluation.objectives[0] < b->member.evaluation.objectives[0]; });
      for (std::size_t r = 0; r < same.size(); r++) {
        if (same[r]->member.ticket != k.member.ticket) continue;
        if (r >= o_.capacity) return "superseded in its kind by #" + std::to_string(heuresis::search::serial_of(same[0]->member.ticket));
        for (std::size_t q = 0; q < r; q++)
          if (same[q]->member.evaluation.objectives[0] < k.member.evaluation.objectives[0] && distance(p_, same[q]->member.genotype, k.member.genotype) < o_.min_distance)
            return "a better doctrine of its kind within distance: #" + std::to_string(heuresis::search::serial_of(same[q]->member.ticket));
      }
      return std::nullopt;
    };
    return j;
  }

  void init(const hev::State<Doctrine>* state) {
    if (const auto f = faults(p_); !f.empty()) {
      std::string all;
      for (const auto& x : f) all += (all.empty() ? "" : "; ") + x;
      throw std::invalid_argument("law::doctrine: the problem has faults: " + all);
    }
    for (std::size_t b = 0; b < p_.bases.size(); b++)
      for (std::size_t c = 0; c < p_.bases[b].base.cases.size(); c++)
        if (open_ratio(p_.bases[b], p_.bases[b].base.cases[c])) {
          const auto fs = for_outcome(p_.bases[b], p_.bases[b].base.cases[c]);
          if (!fs.empty()) slots_.push_back({b, c, std::vector<std::string>(fs.begin(), fs.end())});
        }
    // A state that has run no generation is a fresh start.
    if (state && (state->generation > 0 || !state->population.empty())) evo_ = std::make_unique<hev::Evolution<Doctrine>>(operators(), o_.strategy, judge(), *state, o_.key);
    else evo_ = std::make_unique<hev::Evolution<Doctrine>>(operators(), o_.strategy, judge(), o_.key);
  }

  Problem p_;
  SearchOptions o_;
  Confirmer confirmer_;
  std::vector<Slot> slots_;
  std::unique_ptr<hev::Evolution<Doctrine>> evo_;
};

}  // namespace law::doctrine
