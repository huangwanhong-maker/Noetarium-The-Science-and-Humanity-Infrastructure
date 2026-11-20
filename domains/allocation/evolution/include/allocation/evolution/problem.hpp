// An allocation problem whose resources are of many kinds and whose order of grants matters, composed from three foundations
// (ADR 0137; the infrastructure's design `development/ideas/2026-11-20-evolutionary-allocation-of-heterogeneous-resources.md`,
// Stage 5): what a resource is and whether an allocation of it is lawful (kategoria's `resources`), where a search moves among
// allocations (heuresis's `spaces`), and how an order of grants is read and when it matters (heuresis's `sequential`).
//
// A problem declares:
// - recipients, the domain's entities with their attributes;
// - resources, each with a resource type that kategoria verifies when the problem is composed, a pool, and a carrier, which says
//   how amounts of it are searched:
//     shares       the pool divided exactly among the recipients and a part held back (the simplex, scaled);
//     claims       each recipient claims a fraction of the pool; the claims may exceed it, and are granted in order while it lasts;
//     units        identical units divided exactly among the recipients and a part held back (a composition), with caps;
//     unit claims  each recipient claims a number of units, at most its cap; granted in order while they last;
//     items        each distinct item goes to one recipient or to none;
//     fixed        each recipient's demand is data (an attribute), not searched: only the order decides who is granted;
// - attributes chosen for each recipient on a space with no algebra (a direction on the circle, a level);
// - whether the order of grants is searched (as a permutation, or as random keys), precedence between recipients, a further
//   condition on a grant, and an effect a grant has on the problem's own quantities (a price that rises, works recorded).
//
// The genotype is a product of the carriers' spaces, the attributes' and the order's. A genotype decodes to an allocation through a
// grant system: recipient i's grant gives it everything it requests at once, when every pool it draws on still holds its request,
// its precedents are granted and the further condition allows it; the system's serial decoder walks the order (skipping, deferring
// or stopping where a grant is not allowed, as declared). Each allocation is then checked by kategoria: for every resource, the
// parts given to the recipients, each a bundle of its type, are valid and the pool converts into them together.

#pragma once

#include <allocation/rounds/rounds.hpp>

#include <heuresis/assessment/assessment.hpp>
#include <heuresis/sequential/sequential.hpp>
#include <heuresis/spaces/spaces.hpp>

#include <kategoria/resources/algebra.hpp>
#include <kategoria/resources/verify.hpp>

#include <algorithm>
#include <cmath>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace allocation::evolution {

using rounds::Entity;
using rounds::Refusal;
namespace hs = heuresis::spaces;
namespace hq = heuresis::sequential;
namespace ha = heuresis::assessment;
namespace kr = kategoria::resources;

enum class Carrier { shares, claims, units, unit_claims, items, fixed };

inline const char* name(Carrier c) {
  switch (c) {
    case Carrier::shares: return "shares";
    case Carrier::claims: return "claims";
    case Carrier::units: return "units";
    case Carrier::unit_claims: return "unit claims";
    case Carrier::items: return "items";
    case Carrier::fixed: return "fixed demands";
  }
  return "";
}

struct Resource {
  std::string name;
  kr::Type type;
  Carrier carrier = Carrier::shares;
  kr::Bundle pool;                         // over the type's kinds
  std::size_t kind = 0;                    // the kind allocated (every carrier but items)
  std::vector<std::int64_t> caps;          // units, unit claims: the most each recipient may receive or claim (empty: the pool)
  std::string demand;                      // fixed: the recipients' attribute holding their demand
};

struct Attribute {
  std::string name;
  hs::SpacePtr space;                      // the value chosen for each recipient
};

struct Precedence {
  std::size_t before = 0, after = 0;       // `after` may be granted only once `before` is
  std::string why;
};

enum class OrderBy { permutation, keys };

struct State {
  std::vector<double> remaining;            // per resource: what is not yet granted (items: items not yet granted)
  std::vector<std::vector<double>> held;    // [resource][recipient]
  std::vector<char> granted;                // per recipient
  std::vector<double> scalars;              // the problem's own quantities, changed by its effect
  bool operator==(const State&) const = default;
};

struct Problem {
  std::string name;
  std::vector<Entity> recipients;
  std::vector<Resource> resources;
  std::vector<Attribute> attributes;
  bool ordered = true;
  OrderBy order_by = OrderBy::permutation;
  std::vector<Precedence> precedence;
  hq::Unmet unmet = hq::Unmet::skip;
  std::vector<double> scalars;                                                 // the problem's own quantities, initially
  std::function<std::optional<std::string>(const State&, std::size_t)> allow;  // a further condition on a grant: why not, or nothing
  std::function<void(State&, std::size_t)> effect;                             // what a grant changes beyond the pools
};

// A genotype decoded.
struct Allocation {
  std::vector<std::vector<double>> requested;    // [resource][recipient]
  std::vector<std::vector<double>> amount;       // [resource][recipient]: what each was granted (items: how many)
  std::vector<std::vector<std::int64_t>> owner;  // [resource][item]: the recipient granted it, or −1 (items only)
  std::vector<std::vector<hs::Value>> attribute; // [attribute][recipient]
  hq::Word order, granted, unmet;
  State state;
};

struct Lawfulness {
  bool lawful = true;
  std::vector<char> each;                        // per resource, whether kategoria's verdict holds
  std::vector<std::string> verdicts;             // per resource, kategoria's reason
  std::vector<kr::Bundle> pools;
  std::vector<std::vector<kr::Bundle>> parts;    // [resource][recipient]
};

namespace detail {

// An amount as an exact rational of the kind's quantity: natural kinds by their floor, granular ones by their grain, divisible ones
// to a millionth, always rounded down, so that what is checked is never more than what was granted.
inline kr::Rational exact(const kr::Type& T, std::size_t k, double v) {
  const auto* c = T.at(k).first;
  if (!(v > 0)) return kr::Rational(0);
  if (c->quantity == kr::Quantity::natural) return kr::Rational(static_cast<long long>(std::floor(v + 1e-9)));
  const long long grain = c->quantity == kr::Quantity::granular ? c->grain.to_int64() : 1000000LL;
  return kr::Rational(morphe::arithmetic::Integer(static_cast<long long>(std::floor(v * static_cast<double>(grain) + 1e-9))), morphe::arithmetic::Integer(grain));
}

}  // namespace detail

class Composition {
 public:
  explicit Composition(Problem p) : p_(std::move(p)) {
    const std::size_t n = p_.recipients.size();
    if (n == 0) throw Refusal("an allocation problem needs recipients");
    if (p_.resources.empty() && p_.attributes.empty()) throw Refusal("an allocation problem needs a resource or an attribute");
    for (const auto& q : p_.precedence)
      if (q.before >= n || q.after >= n || q.before == q.after) throw Refusal("a precedence between recipients that do not exist, or of a recipient on itself");
    std::vector<hs::Part> parts;
    for (std::size_t r = 0; r < p_.resources.size(); r++) {
      const Resource& res = p_.resources[r];
      const auto report = kr::verify(res.type);
      reports_.push_back(report);
      if (!report.lawful()) {
        std::string why;
        for (const auto& e : report.entries)
          if (e.verdict.status == kategoria::kernel::Status::fails) why += (why.empty() ? "" : "; ") + e.verdict.claim + ": " + e.verdict.reason;
        throw Refusal("the resource type of " + res.name + " fails its laws: " + why);
      }
      if (res.pool.size() != res.type.size()) throw Refusal("the pool of " + res.name + " is not a bundle of its type");
      if (auto why = kr::invalid(res.type, res.pool)) throw Refusal("the pool of " + res.name + " is not valid: " + *why);
      if (res.carrier != Carrier::items && res.kind >= res.type.size()) throw Refusal("the kind allocated of " + res.name + " is not of its type");
      const double amount = res.carrier == Carrier::items ? 0 : res.pool[res.kind].to_double();
      resource_part_.push_back(parts.size());
      switch (res.carrier) {
        case Carrier::shares:
          parts.push_back({res.name + " shares", std::make_shared<hs::Simplex>(n + 1), 1});
          break;
        case Carrier::claims:
          parts.push_back({res.name + " claims", std::make_shared<hs::RealBox>(std::vector<double>(n, 0), std::vector<double>(n, 1)), 1});
          break;
        case Carrier::units: {
          const auto N = static_cast<std::int64_t>(std::llround(amount));
          std::vector<std::int64_t> caps = res.caps.empty() ? std::vector<std::int64_t>(n, N) : res.caps;
          if (caps.size() != n) throw Refusal("the caps of " + res.name + " must be one per recipient");
          caps.push_back(N);  // the part held back
          parts.push_back({res.name + " units", std::make_shared<hs::Composition>(n + 1, N, caps), 1});
          break;
        }
        case Carrier::unit_claims: {
          const auto N = static_cast<std::int64_t>(std::llround(amount));
          std::vector<std::int64_t> caps = res.caps.empty() ? std::vector<std::int64_t>(n, N) : res.caps;
          if (caps.size() != n) throw Refusal("the caps of " + res.name + " must be one per recipient");
          parts.push_back({res.name + " claims", std::make_shared<hs::IntegerBox>(std::vector<std::int64_t>(n, 0), caps), 1});
          break;
        }
        case Carrier::items: {
          std::vector<hs::Part> items;
          for (std::size_t k = 0; k < res.type.size(); k++) {
            const auto* c = res.type.at(k).first;
            const auto* kind = res.type.at(k).second;
            if (c->quantity != kr::Quantity::natural || !kind->distinct) throw Refusal("the items of " + res.name + " must be distinct kinds of natural quantity");
            if (kr::Rational(1) < res.pool[k]) throw Refusal("an item of " + res.name + " is held at most once");
            items.push_back({kind->name, std::make_shared<hs::Categorical>(n + 1), 1});  // the last category: no recipient
          }
          parts.push_back({res.name + " items", std::make_shared<hs::Product>(items), 1});
          break;
        }
        case Carrier::fixed:
          for (const auto& e : p_.recipients)
            if (!std::isfinite(e.get(res.demand))) throw Refusal("the fixed demand of " + res.name + " needs the attribute '" + res.demand + "' of every recipient");
          resource_part_.back() = static_cast<std::size_t>(-1);  // nothing searched
          break;
      }
    }
    for (const auto& a : p_.attributes) {
      if (!a.space) throw Refusal("the attribute " + a.name + " has no space");
      std::vector<hs::Part> each;
      for (std::size_t i = 0; i < n; i++) each.push_back({p_.recipients[i].id.empty() ? std::to_string(i) : p_.recipients[i].id, a.space, 1});
      attribute_part_.push_back(parts.size());
      parts.push_back({a.name, std::make_shared<hs::Product>(each), 1});
    }
    if (p_.ordered && n >= 2) {
      order_part_ = parts.size();
      if (p_.order_by == OrderBy::permutation) parts.push_back({"order", std::make_shared<hs::PermutationSpace>(n, hs::PermutationSpace::Metric::kendall), 1});
      else parts.push_back({"order keys", std::make_shared<hs::RealBox>(std::vector<double>(n, 0), std::vector<double>(n, 1)), 1});
    }
    if (parts.empty()) throw Refusal("nothing to search: every resource is fixed and the order is not searched");
    space_ = std::make_shared<hs::Product>(parts);
  }

  const Problem& problem() const { return p_; }
  const std::shared_ptr<hs::Product>& space() const { return space_; }
  const std::vector<kr::Report>& verification() const { return reports_; }
  std::size_t recipients() const { return p_.recipients.size(); }

  // The requests a genotype makes, [resource][recipient], and the owners it names for items.
  void requests(const hs::Value& g, std::vector<std::vector<double>>& req, std::vector<std::vector<std::int64_t>>& owner) const {
    const std::size_t n = recipients();
    req.assign(p_.resources.size(), std::vector<double>(n, 0));
    owner.assign(p_.resources.size(), {});
    for (std::size_t r = 0; r < p_.resources.size(); r++) {
      const Resource& res = p_.resources[r];
      const double amount = res.carrier == Carrier::items ? 0 : res.pool[res.kind].to_double();
      if (res.carrier == Carrier::fixed) {
        for (std::size_t i = 0; i < n; i++) req[r][i] = p_.recipients[i].get(res.demand);
        continue;
      }
      const hs::Value& v = g.parts.at(resource_part_[r]);
      switch (res.carrier) {
        case Carrier::shares:
          for (std::size_t i = 0; i < n; i++) req[r][i] = v.reals[i] * amount;
          break;
        case Carrier::claims:
          for (std::size_t i = 0; i < n; i++) req[r][i] = v.reals[i] * amount;
          break;
        case Carrier::units:
        case Carrier::unit_claims:
          for (std::size_t i = 0; i < n; i++) req[r][i] = static_cast<double>(v.ints[i]);
          break;
        case Carrier::items:
          for (const auto& item : v.parts) {
            const auto who = item.ints[0];
            owner[r].push_back(who == static_cast<std::int64_t>(n) ? -1 : who);
            if (who < static_cast<std::int64_t>(n)) req[r][static_cast<std::size_t>(who)] += 1;
          }
          break;
        case Carrier::fixed: break;
      }
    }
  }

  // The order a genotype tries the grants in.
  hq::Word order_of(const hs::Value& g) const {
    const std::size_t n = recipients();
    hq::Word w(n);
    for (std::size_t i = 0; i < n; i++) w[i] = i;
    if (!order_part_) return w;
    const hs::Value& v = g.parts.at(*order_part_);
    if (p_.order_by == OrderBy::permutation) {
      for (std::size_t i = 0; i < n; i++) w[i] = static_cast<std::size_t>(v.ints[i]);
    } else {
      std::stable_sort(w.begin(), w.end(), [&](std::size_t a, std::size_t b) { return v.reals[a] < v.reals[b]; });
    }
    return w;
  }

  // The grant system of a set of requests: one grant per recipient.
  hq::GrantSystem<State> grant_system(const std::vector<std::vector<double>>& req, bool footprints = true) const {
    const std::size_t n = recipients(), m = p_.resources.size();
    hq::GrantSystem<State> sys;
    sys.grants = n;
    for (const auto& e : p_.recipients) sys.names.push_back(e.name.empty() ? e.id : e.name);
    State s0;
    s0.remaining.resize(m);
    for (std::size_t r = 0; r < m; r++) {
      const Resource& res = p_.resources[r];
      if (res.carrier == Carrier::items) {
        double items = 0;
        for (std::size_t k = 0; k < res.type.size(); k++) items += res.pool[k].to_double();
        s0.remaining[r] = items;
      } else {
        s0.remaining[r] = res.pool[res.kind].to_double();
      }
    }
    s0.held.assign(m, std::vector<double>(n, 0));
    s0.granted.assign(n, 0);
    s0.scalars = p_.scalars;
    sys.initial = s0;
    const Problem* p = &p_;
    sys.apply = [p, req, n, m](std::size_t i, const State& x) -> std::optional<State> {
      if (x.granted[i]) return std::nullopt;
      for (const auto& q : p->precedence)
        if (q.after == i && !x.granted[q.before]) return std::nullopt;
      for (std::size_t r = 0; r < m; r++) {
        const double tol = 1e-12 * std::max(1.0, std::abs(x.remaining[r]) + req[r][i]);
        if (req[r][i] > x.remaining[r] + tol) return std::nullopt;
      }
      if (p->allow && p->allow(x, i)) return std::nullopt;
      State y = x;
      for (std::size_t r = 0; r < m; r++) {
        y.remaining[r] = std::max(0.0, y.remaining[r] - req[r][i]);
        y.held[r][i] = req[r][i];
      }
      y.granted[i] = 1;
      if (p->effect) p->effect(y, i);
      (void)n;
      return y;
    };
    // Footprints: a grant reads and writes the pools it draws on, its own holdings and whether it is granted; it reads whether its
    // precedents are; the problem's own quantities, when a condition or an effect is declared.
    for (std::size_t i = 0; i < n && footprints; i++) {
      morphe::formal::traces::Footprint f;
      for (std::size_t r = 0; r < m; r++) {
        if (req[r][i] > 0) f.reads.insert("pool " + std::to_string(r)), f.writes.insert("pool " + std::to_string(r));
        f.writes.insert("held " + std::to_string(r) + " " + std::to_string(i));
      }
      f.writes.insert("granted " + std::to_string(i));
      for (const auto& q : p_.precedence)
        if (q.after == i) f.reads.insert("granted " + std::to_string(q.before));
      if (p_.allow) f.reads.insert("scalars");
      if (p_.effect) f.reads.insert("scalars"), f.writes.insert("scalars");
      sys.footprints.push_back(f);
    }
    return sys;
  }

  Allocation decode(const hs::Value& g) const {
    if (auto why = space_->invalid(g)) throw Refusal("decoding an allocation: " + *why);
    Allocation a;
    requests(g, a.requested, a.owner);
    a.order = order_of(g);
    const auto sys = grant_system(a.requested, false);
    const auto d = hq::serial(sys, a.order, p_.unmet);
    a.granted = d.granted, a.unmet = d.unmet, a.state = d.state;
    a.amount = d.state.held;
    for (std::size_t r = 0; r < p_.resources.size(); r++)
      for (auto& o : a.owner[r])
        if (o >= 0 && !d.state.granted[static_cast<std::size_t>(o)]) o = -1;  // an item of a recipient not granted is not given
    for (std::size_t k = 0; k < p_.attributes.size(); k++) a.attribute.push_back(g.parts.at(attribute_part_[k]).parts);
    return a;
  }

  // The independence of the grants a genotype's requests make, from their footprints.
  morphe::formal::traces::Independence independence(const Allocation& a) const {
    return morphe::formal::traces::independence(grant_system(a.requested).footprints);
  }

  // A key for a genotype: what it requests and chooses, and the trace of what it granted, so that genotypes that differ only by the
  // order of grants that commute share a key, and an allocation is evaluated once.
  std::string key(const hs::Value& g) const {
    const Allocation a = decode(g);
    std::string k;
    for (std::size_t i = 0; i < g.parts.size(); i++)
      if (!order_part_ || i != *order_part_) k += hs::key(g.parts[i]) + ";";
    return k + hq::trace_key(a.granted, independence(a));
  }

  Lawfulness lawful(const Allocation& a) const {
    Lawfulness L;
    const std::size_t n = recipients();
    for (std::size_t r = 0; r < p_.resources.size(); r++) {
      const Resource& res = p_.resources[r];
      std::vector<kr::Bundle> parts(n, res.type.zero());
      if (res.carrier == Carrier::items) {
        for (std::size_t k = 0; k < a.owner[r].size(); k++)
          if (a.owner[r][k] >= 0) parts[static_cast<std::size_t>(a.owner[r][k])][k] = parts[static_cast<std::size_t>(a.owner[r][k])][k] + kr::Rational(1);
      } else {
        for (std::size_t i = 0; i < n; i++) parts[i][res.kind] = detail::exact(res.type, res.kind, a.amount[r][i]);
      }
      const auto check = kr::allocation(res.type, res.pool, parts);
      L.lawful = L.lawful && check.verdict.holds();
      L.each.push_back(check.verdict.holds() ? 1 : 0);
      L.verdicts.push_back(check.verdict.reason);
      L.pools.push_back(res.pool);
      L.parts.push_back(std::move(parts));
    }
    return L;
  }

 private:
  Problem p_;
  std::shared_ptr<hs::Product> space_;
  std::vector<std::size_t> resource_part_, attribute_part_;
  std::optional<std::size_t> order_part_;
  std::vector<kr::Report> reports_;
};

}  // namespace allocation::evolution
