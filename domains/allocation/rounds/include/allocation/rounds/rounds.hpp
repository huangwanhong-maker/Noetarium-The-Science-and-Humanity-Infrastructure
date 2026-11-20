// An allocation round: a resource short of what is wanted, the entities that want it, and the stages by which it is divided,
// each serving a declared principle, declared by someone, for a stated reason.
//
// Just allocation under scarcity is not one optimisation (the infrastructure's survey *Allocative justice under scarcity*, §1):
// it is a sequence of declared rules, applied in turn to what remains, each answerable to a principle. A round here is that
// sequence. Its stages, each taking at most a declared share of what remains:
//
//   eligibility   entities whose attribute falls short of (or exceeds) a declared bound take no further part, each with the
//                 reason (a constraint: it filters, it does not weigh)
//   floor         sufficiency first: what each lacks to a declared level per head, times its people, is closed first; if what
//                 remains cannot close every shortfall, it is divided among them by a declared rationing rule (nomos)
//   priority      who first: entities in classes by an attribute (highest first), each class's claims met in full before the
//                 next, the class where the resource runs out divided by a declared rule (nomos's priority classes)
//   claims        how much: each entity's claim, less what earlier stages gave it, divided by a declared rationing rule
//                 (proportional, equal awards, equal losses, Talmud, TAL(θ), Piniles, adjusted proportional)
//   proportional  what remains in proportion to a declared attribute (population, say)
//   points        what remains in proportion to banded points on declared indicators (Porto Alegre's procedure)
//   equalisation  what remains as an equal amount per head adjusted by λ[(e_i − ē) + Σ t_k (b̄_k − b_ik)]: assessed need above
//                 the standard and capacity below it (horizontal fiscal equalisation); a grant pushed below zero is held at
//                 zero and the difference spread over the others, reported as the envelope's part
//
// Every entity's amount is reported by stage, with the reasons in words: what the stage gave, under which principle, declared
// by whom. Nothing here says the result is just; it says what follows from what was declared (the survey, §1, what a tool must
// never claim).
#pragma once

#include <nomos/allocation/rationing.hpp>
#include <nomos/games/claims.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <stdexcept>
#include <string>
#include <vector>

namespace allocation::rounds {

struct Refusal : std::runtime_error {
  explicit Refusal(const std::string& why) : std::runtime_error(why) {}
};

struct Entity {
  std::string id, name;
  std::string group;                       // a group of circumstance (for opportunity, and for who took part)
  std::map<std::string, double> at;        // its attributes: population, level, need, claim, …
  double get(const std::string& key, double fallback = std::numeric_limits<double>::quiet_NaN()) const {
    const auto it = at.find(key);
    return it == at.end() ? fallback : it->second;
  }
};

struct Source {
  std::string who, when, why;              // who declared it, when (ISO 8601), and why
};

struct Stage {
  std::string kind = "proportional";       // eligibility | floor | priority | claims | proportional | points | equalisation
  std::string principle;                   // the principle it serves, named (need, sufficiency, urgency, desert, equality …)
  Source source;
  double share = 1;                        // the share of what remains it may use
  std::string attribute;                   // eligibility: the attribute tested; floor: the level; priority: the class attribute;
                                           // claims: the claim; proportional: the weight; equalisation: the need e_i
  double bound = 0;                        // eligibility: the bound; floor: the level per head τ
  bool at_least = true;                    // eligibility: the attribute must be at least the bound (else at most)
  std::string per = "population";          // floor and equalisation: the attribute counting people
  std::string rule = "proportional";       // floor, priority, claims: the rationing rule
  double theta = 0.5;                      // the TAL family's θ
  std::string claim;                       // priority: the claim attribute
  std::vector<nomos::allocation::Indicator> indicators;  // points: each indicator's `name` is an attribute; values filled in
  std::vector<std::string> bases;          // equalisation: the tax bases per head
  std::vector<double> rates;               // equalisation: the standard rates
  double lambda = 1;                       // equalisation: the rate of equalisation
};

struct Round {
  std::string name, resource, unit;
  double amount = 0;
  std::vector<Entity> entities;
  std::vector<Stage> stages;
  std::map<std::string, std::string> sources;  // for each attribute, where its values came from (accuracy, procedure)
};

struct Result {
  std::vector<double> amount;                         // per entity
  std::vector<std::vector<double>> by_stage;          // [stage][entity]
  std::vector<std::string> excluded;                  // per entity: why it took no further part ("" if it did)
  std::vector<std::vector<std::string>> reasons;      // per entity, a sentence for each stage that gave it something
  std::vector<double> remaining_after;                // what remained after each stage
  double left = 0;
  std::vector<std::string> notes;                     // per round: scarcity met, rules applied
};

inline nomos::allocation::Rule rule_of(const std::string& name, double theta = 0.5) {
  namespace g = nomos::games;
  namespace a = nomos::allocation;
  if (name == "proportional") return g::proportional;
  if (name == "equal_awards") return g::constrained_equal_awards;
  if (name == "equal_losses") return g::constrained_equal_losses;
  if (name == "talmud") return g::talmud;
  if (name == "piniles") return a::piniles;
  if (name == "adjusted_proportional") return a::adjusted_proportional;
  if (name == "random_arrival") return g::random_arrival;
  if (name == "tal") return [theta](const g::Claims& P) { return a::tal(P, theta); };
  throw Refusal("a rationing rule is proportional, equal_awards, equal_losses, talmud, tal, piniles, adjusted_proportional or random_arrival, not '" + name + "'");
}

namespace detail {

// A number as it is read: whole and grouped by thousands from ten thousand (1,060,000,000), else six significant figures.
inline std::string number(double v) {
  char b[64];
  if (!(std::abs(v) >= 1e4) || !std::isfinite(v)) {
    std::snprintf(b, sizeof b, "%.6g", v);
    return b;
  }
  std::snprintf(b, sizeof b, "%.0f", std::abs(v));
  std::string digits = b, out;
  for (std::size_t k = 0; k < digits.size(); k++) {
    if (k > 0 && (digits.size() - k) % 3 == 0) out += ',';
    out += digits[k];
  }
  return (v < 0 ? "-" : "") + out;
}

inline std::string declared(const Stage& s) {
  std::string d = s.principle.empty() ? std::string("no principle named") : "for " + s.principle;
  if (!s.source.who.empty()) d += ", declared by " + s.source.who;
  if (!s.source.when.empty()) d += " on " + s.source.when;
  return d;
}

inline double attr(const Entity& e, const std::string& key, const std::string& stage) {
  const double v = e.get(key);
  if (!std::isfinite(v)) throw Refusal("the stage " + stage + " needs the attribute '" + key + "' of " + (e.name.empty() ? e.id : e.name) + ", which is not given");
  return v;
}

}  // namespace detail

inline Result run(const Round& r) {
  const std::size_t n = r.entities.size();
  if (n == 0) throw Refusal("a round needs entities to allocate to");
  if (!(r.amount >= 0)) throw Refusal("a round's amount is not negative");
  Result out;
  out.amount.assign(n, 0);
  out.excluded.assign(n, "");
  out.reasons.assign(n, {});
  for (std::size_t k = 0; k < r.stages.size(); k++) {
    const Stage& s = r.stages[k];
    if (s.kind == "floor" || s.kind == "priority" || s.kind == "claims") {
      try {
        (void)rule_of(s.rule, s.theta);
      } catch (const Refusal& e) {
        throw Refusal("stage " + std::to_string(k + 1) + " (" + s.kind + "): " + e.what());
      }
    }
  }
  std::vector<bool> in(n, true);
  double remaining = r.amount;
  for (std::size_t k = 0; k < r.stages.size(); k++) {
    const Stage& s = r.stages[k];
    const std::string label = "stage " + std::to_string(k + 1) + " (" + s.kind + ")";
    if (!(s.share >= 0 && s.share <= 1)) throw Refusal(label + ": its share of what remains lies in [0, 1]");
    std::vector<double> give(n, 0);
    const double pool = remaining * s.share;
    std::vector<std::size_t> members;
    for (std::size_t i = 0; i < n; i++)
      if (in[i]) members.push_back(i);

    if (s.kind == "eligibility") {
      for (std::size_t i : members) {
        const double v = detail::attr(r.entities[i], s.attribute, label);
        const bool ok = s.at_least ? v >= s.bound : v <= s.bound;
        if (!ok) {
          in[i] = false;
          out.excluded[i] = "excluded at " + label + ": its " + s.attribute + " (" + detail::number(v) + ") is " + (s.at_least ? "below " : "above ") + detail::number(s.bound) + ", " + detail::declared(s);
        }
      }
    } else if (s.kind == "floor") {
      std::vector<double> gaps;
      for (std::size_t i : members) {
        const double level = detail::attr(r.entities[i], s.attribute, label), people = detail::attr(r.entities[i], s.per, label);
        gaps.push_back(std::max(0.0, s.bound - level) * people);
      }
      const double G = std::accumulate(gaps.begin(), gaps.end(), 0.0);
      if (G > 0) {
        const double use = std::min(pool, G);
        std::vector<double> x;
        if (use >= G) x = gaps;
        else x = rule_of(s.rule, s.theta)(nomos::games::Claims{use, gaps});
        for (std::size_t j = 0; j < members.size(); j++) give[members[j]] = x[j];
        out.notes.push_back(label + ": shortfalls of " + detail::number(G) + " below " + detail::number(s.bound) + " per head" +
                            (use < G ? "; only " + detail::number(use) + " to close them, divided by " + s.rule : "; all closed"));
      }
    } else if (s.kind == "priority" || s.kind == "claims") {
      const std::string claim_key = s.kind == "priority" ? s.claim : s.attribute;
      std::vector<double> claims;
      for (std::size_t i : members) claims.push_back(std::max(0.0, detail::attr(r.entities[i], claim_key, label) - out.amount[i]));
      const double C = std::accumulate(claims.begin(), claims.end(), 0.0);
      if (C > 0) {
        const double use = std::min(pool, C);
        std::vector<double> x;
        if (s.kind == "priority") {
          // Classes by the attribute, the highest first.
          std::vector<double> levels;
          for (std::size_t i : members) levels.push_back(detail::attr(r.entities[i], s.attribute, label));
          std::vector<double> distinct = levels;
          std::sort(distinct.begin(), distinct.end(), std::greater<>());
          distinct.erase(std::unique(distinct.begin(), distinct.end()), distinct.end());
          std::vector<std::size_t> cls;
          for (double v : levels) cls.push_back(static_cast<std::size_t>(std::find(distinct.begin(), distinct.end(), v) - distinct.begin()));
          x = nomos::allocation::priority_classes(nomos::games::Claims{use, claims}, cls, rule_of(s.rule, s.theta));
        } else {
          x = use >= C ? claims : rule_of(s.rule, s.theta)(nomos::games::Claims{use, claims});
        }
        for (std::size_t j = 0; j < members.size(); j++) give[members[j]] = x[j];
        out.notes.push_back(label + ": claims of " + detail::number(C) + (use < C ? ", " + detail::number(use) + " to meet them, by " + s.rule : ", all met"));
      }
    } else if (s.kind == "proportional") {
      double W = 0;
      std::vector<double> w;
      for (std::size_t i : members) w.push_back(detail::attr(r.entities[i], s.attribute, label)), W += w.back();
      if (W > 0)
        for (std::size_t j = 0; j < members.size(); j++) give[members[j]] = pool * w[j] / W;
    } else if (s.kind == "points") {
      std::vector<nomos::allocation::Indicator> inds = s.indicators;
      for (auto& ind : inds) {
        ind.values.clear();
        for (std::size_t i : members) ind.values.push_back(detail::attr(r.entities[i], ind.name, label));
      }
      const auto p = nomos::allocation::banded_points(pool, inds);
      for (std::size_t j = 0; j < members.size(); j++) give[members[j]] = p.award[j];
    } else if (s.kind == "equalisation") {
      std::vector<nomos::allocation::Unit> units;
      double P = 0;
      for (std::size_t i : members) {
        nomos::allocation::Unit u;
        u.name = r.entities[i].name;
        u.population = detail::attr(r.entities[i], s.per, label);
        u.need = detail::attr(r.entities[i], s.attribute, label);
        for (const auto& b : s.bases) u.bases.push_back(detail::attr(r.entities[i], b, label));
        P += u.population;
        units.push_back(u);
      }
      if (s.bases.size() != s.rates.size()) throw Refusal(label + ": a standard rate for every tax base");
      const auto eq = nomos::allocation::equalisation(units, s.rates, s.lambda);
      std::vector<double> x(members.size());
      for (std::size_t j = 0; j < members.size(); j++) x[j] = pool * units[j].population / P + eq.first[j];
      // A grant below zero is held at zero; the shortfall it would have paid is spread over the others by population.
      for (int pass = 0; pass < static_cast<int>(members.size()); pass++) {
        double deficit = 0, Pfree = 0;
        for (std::size_t j = 0; j < members.size(); j++) {
          if (x[j] < 0) deficit += -x[j], x[j] = 0;
          else if (x[j] > 0) Pfree += units[j].population;
        }
        if (deficit <= 1e-12 * std::max(1.0, pool) || Pfree <= 0) break;
        for (std::size_t j = 0; j < members.size(); j++)
          if (x[j] > 0) x[j] -= deficit * units[j].population / Pfree;
      }
      for (std::size_t j = 0; j < members.size(); j++) give[members[j]] = x[j];
    } else {
      throw Refusal(label + ": a stage is eligibility, floor, priority, claims, proportional, points or equalisation");
    }

    double used = 0;
    for (std::size_t i = 0; i < n; i++) {
      used += give[i];
      out.amount[i] += give[i];
      if (give[i] > 1e-12 * std::max(1.0, r.amount))
        out.reasons[i].push_back(label + " gave " + detail::number(give[i]) + " " + r.unit + ", " + detail::declared(s));
    }
    out.by_stage.push_back(give);
    remaining -= used;
    if (remaining < 0 && remaining > -1e-9 * std::max(1.0, r.amount)) remaining = 0;
    out.remaining_after.push_back(remaining);
  }
  out.left = remaining;
  return out;
}

// Who first, then who: the entities in the order the round reached them, by the first stage that gave each something and, within
// a stage, by what it gave (most first); those never reached follow, the excluded last.
struct Reached {
  std::size_t entity = 0;
  std::size_t stage = 0;       // the first stage that gave it something (stages.size() if none)
  double first = 0;            // what that stage gave
};

inline std::vector<Reached> order_reached(const Result& r, double tolerance = 1e-12) {
  const std::size_t n = r.amount.size(), K = r.by_stage.size();
  std::vector<Reached> out;
  for (std::size_t i = 0; i < n; i++) {
    Reached x{i, K, 0};
    for (std::size_t k = 0; k < K; k++)
      if (r.by_stage[k][i] > tolerance) { x.stage = k; x.first = r.by_stage[k][i]; break; }
    out.push_back(x);
  }
  std::stable_sort(out.begin(), out.end(), [&](const Reached& a, const Reached& b) {
    const bool ea = !r.excluded[a.entity].empty(), eb = !r.excluded[b.entity].empty();
    if (ea != eb) return eb;
    if (a.stage != b.stage) return a.stage < b.stage;
    return a.first > b.first;
  });
  return out;
}

// The attributes a round's stages read, each once, in the order first read.
inline std::vector<std::string> attributes_read(const Round& r) {
  std::vector<std::string> out;
  auto add = [&](const std::string& a) {
    if (!a.empty() && std::find(out.begin(), out.end(), a) == out.end()) out.push_back(a);
  };
  for (const auto& s : r.stages) {
    if (s.kind == "eligibility" || s.kind == "proportional" || s.kind == "claims") add(s.attribute);
    else if (s.kind == "floor") add(s.attribute), add(s.per);
    else if (s.kind == "priority") add(s.attribute), add(s.claim);
    else if (s.kind == "points") for (const auto& i : s.indicators) add(i.name);
    else if (s.kind == "equalisation") {
      add(s.attribute), add(s.per);
      for (const auto& b : s.bases) add(b);
    }
  }
  return out;
}

}  // namespace allocation::rounds
