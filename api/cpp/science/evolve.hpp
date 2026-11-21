// Heterogeneous allocation searched by evolution, as one composition any work may call (ADR 0137): the allocation domain's
// `evolution` with kategoria's resource types and heuresis's spaces, orders of grants and assessment, read and answered as JSON.
// The Urban Studio's plug-in "Allocation and justice" answers its page with it through the same engine as ADR 0136's operations.
// Nothing in it names a place; every amount, coefficient and principle in a request is the requester's declaration.
//
//   evolve.verify     {type}
//                     a resource type's laws, each verdict with its method and scope (by construction, on generators, on a finite
//                     model, by search) and its reason
//   evolve.compose    {problem}
//                     the problem composed: each resource type's verification, the genotype's parts with their spaces and units
//   evolve.search     {problem, evaluators, settings, comparing}
//                     the search: the allocations kept, best first by the tiers of evidence, each with its metrics and standing, its
//                     lawfulness by kategoria, what each recipient received and in what order; the kept compared under declared
//                     principles; whether order mattered, with witnesses; what the keys saved; the record sealed in archeion's journal
//   evolve.strengths  {items, judgements: [{first, second, first_preferred}], order_effect, prior}
//                     Bradley–Terry strengths from pairwise judgements (a person's, a model's), with the effect of the order shown
//   evolve.learn      {record: {states, inputs}, resamples, seed}
//                     the pooled linear law a "laws" evaluator learns from a record (below): its coefficients, each refit's, its
//                     error one step ahead against persistence's, and the record's support (ADR 0141)
//   evolve.calibrate  {places, accounts: {local, wages, supplies, imported, source}, velocity, habits, short_too}
//                     an economy of places' habits calibrated to records: the accounts' shares read, the share of holdings a
//                     household spends matched to a velocity of households' money per period; what the records leave unidentified
//                     said; a velocity beyond reach refused with the range that can be reached (ADR 0141)
//   evolve.budget     {budget, costs, approvals: [[project]], groups: [voter's group or -1], group_names, names, settings}
//                     a participatory budget as a sequential mechanism: the orders of its projects decoded by the budget, searched
//                     under two declared objectives (the share of voters with nothing, the approvals not met), the front kept,
//                     with utilitarian greedy, greedy per cost and the Method of Equal Shares read on the same measures, their orders
//                     proposed to the first generation unless settings.propose_rules is false (ADR 0141)
//
// A resource type is {name, components: [{name, quantity: natural | granular | nonnegative | integer | rational | boolean | maximum,
// grain, kinds: [{name, disposable, capacity, distinct}]}], conversions: [{name, from: {kind: amount}, to: {kind: amount}}],
// laws: [{name, weights: {kind: number}, conserved}], declared: [property]}. Amounts are integers, "p/q", or decimals (exactly).
//
// A problem is {name, recipients: [{id, name, group, at: {attribute: number}}], resources: [{name, type, carrier: shares | claims |
// units | unit_claims | items | fixed, pool: {kind: amount} or amount, kind, caps: [per recipient], demand, partial}], attributes: [{name,
// space: circle | sphere | levels | categories | interval, dimension, levels, lower, upper}], ordered, order_by: permutation | keys,
// precedence: [{before: id, after: id, why}], unmet: skip | defer | stop, price: {resource, start, factor}}. A declared price makes
// each grant drawing on the resource buy at the current price and raise it by the factor; what it bought is the recipient's "works".
//
// An evaluator is {name, standing: verified | computed | estimated | proposed, kind, objective: maximise | minimise, terms: [{of:
// amount | works | items | granted, resource, form: linear | log1p | sqrt | square, coefficient, weight: attribute, per: attribute,
// align: {attribute, toward: [x, y, …]}}], units_per: attribute, units_scale}: each recipient's value Σ coefficient × weight ×
// form(x / per) (× the alignment of its attribute with `toward`, for items), the metric the negated total (maximise) or the total.
// An evaluator of another kind (ADR 0141; the allocation domain's `evaluators`, ADR 0139) reads one resource as money or holdings:
//   {kind: economy, name, resource, unit, places: [{name, households, firms, household_money, firm_money}] (one for each
//    recipient), habits: {short_below, short_spend, short_local, spend, local, wages, supplies, exports, outside_money, source},
//    trade: [[weight]], spacing, periods, rounds}: an economy of places whose every payment is posted, the grants reaching the
//    households in the allocation's order; the objective the money re-spent in its own place, lost (negated); the accounts' audit
//    a constraint; LM3, the share that left and the payments that waited as reports;
//   {kind: density, name, resource, unit, population: attribute or [number], mean: attribute or [number], grid: {lower, upper,
//    cells}, pull, spread, line, dt, between, after}: a declared law of holdings with a mean field (Bouchaud and Mézard's), the
//    objective the share of all units holding less than the line;
//   {kind: laws, name, resource, unit, record: {states: [[value per recipient]], inputs: [[value per recipient]]}, start,
//    resamples, seed, between, after, margin}: a pooled linear law x' = a x + b u + c learned from the record by least squares,
//    with refits on its steps drawn again as the ensemble; the worth the mean final state, lost; the steps beyond the record's
//    support a constraint; standing estimated.
// A constraint such an evaluator checks exactly (the audit, the support) stands no higher than its evaluator, by the assessor's rule
// that no metric is stronger than what computed it (heuresis's decision 0010): computed for the economy, estimated for the laws.
// Settings are {generations, mu, lambda, seed, keep, keep_apart, survival: alps | plus | …, scales, when}; comparing is {metric, principles,
// per}, principles as science/allocation.hpp reads them.

#pragma once

#include <allocation/evaluators/evaluators.hpp>
#include <allocation/evolution/budget.hpp>
#include <allocation/evolution/search.hpp>
#include <economics/ledger/calibrate.hpp>

#include <science/allocation.hpp>
#include <science/json.hpp>

#include <array>
#include <cmath>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::evolve {

namespace ae = ::allocation::evolution;
namespace kr = kategoria::resources;
namespace hs = heuresis::spaces;
namespace ha = heuresis::assessment;
namespace he = heuresis::evolution;
namespace av = ::allocation::evaluators;
namespace el = economics::ledger;
namespace ki = kinesis::induction;

using science::allocation::num;
using science::allocation::r6;

// ---- reading ----

inline kr::Rational rational(const Json& j, const std::string& what) {
  if (j.is_number()) {
    const double v = j.as_number();
    if (!std::isfinite(v)) throw std::invalid_argument("evolve: " + what + " is not finite");
    if (v == std::floor(v) && std::abs(v) < 9e15) return kr::Rational(static_cast<long long>(v));
    const long long scale = 1000000;
    const double scaled = v * static_cast<double>(scale);
    if (std::abs(scaled - std::round(scaled)) > 1e-6 * std::max(1.0, std::abs(scaled)))
      throw std::invalid_argument("evolve: " + what + " has more than six decimals; give it as \"p/q\"");
    return kr::Rational(morphe::arithmetic::Integer(static_cast<long long>(std::llround(scaled))), morphe::arithmetic::Integer(scale));
  }
  if (j.is_string()) {
    const std::string s = j.as_string();
    const auto dot = s.find('.');
    if (dot == std::string::npos) return kr::Rational::parse(s);
    const std::string digits = s.substr(0, dot) + s.substr(dot + 1);
    long long den = 1;
    for (std::size_t k = dot + 1; k < s.size(); k++) den *= 10;
    return kr::Rational(morphe::arithmetic::Integer::parse(digits), morphe::arithmetic::Integer(den));
  }
  throw std::invalid_argument("evolve: " + what + " must be a number or \"p/q\"");
}

inline kr::Quantity quantity_of(const std::string& q) {
  if (q == "natural") return kr::Quantity::natural;
  if (q == "granular") return kr::Quantity::granular;
  if (q == "nonnegative" || q.empty()) return kr::Quantity::nonnegative;
  if (q == "integer") return kr::Quantity::integer;
  if (q == "rational") return kr::Quantity::rational;
  if (q == "boolean") return kr::Quantity::boolean;
  if (q == "maximum") return kr::Quantity::maximum;
  throw std::invalid_argument("evolve: a quantity is natural, granular, nonnegative, integer, rational, boolean or maximum, not \"" + q + "\"");
}

inline kr::Bundle bundle_of(const kr::Type& T, const Json& j, const std::string& what) {
  kr::Bundle b = T.zero();
  if (j.is_null()) return b;
  if (!j.is_object()) {
    if (T.size() != 1) throw std::invalid_argument("evolve: " + what + " of a type of several kinds must name them: {kind: amount}");
    b[0] = rational(j, what);
    return b;
  }
  for (const auto& [k, v] : j.as_object()) b[T.index(k)] = rational(v, what + " of " + k);
  return b;
}

inline kr::Type type_of(const Json& j) {
  kr::Type T;
  T.name = j["name"].text("a resource type");
  for (const auto& c : j["components"].as_array()) {
    kr::Component comp;
    comp.name = c["name"].text();
    comp.quantity = quantity_of(c["quantity"].text());
    comp.grain = static_cast<long long>(num(c["grain"], 1));
    for (const auto& k : c["kinds"].as_array()) {
      kr::Kind kind;
      kind.name = k["name"].text();
      kind.disposable = k.has("disposable") ? k["disposable"].as_bool() : true;
      if (k.has("capacity") && !k["capacity"].is_null()) kind.capacity = rational(k["capacity"], "a capacity");
      kind.distinct = k.has("distinct") && k["distinct"].as_bool();
      comp.kinds.push_back(kind);
    }
    if (comp.kinds.empty()) throw std::invalid_argument("evolve: the component " + comp.name + " has no kinds");
    T.components.push_back(comp);
  }
  if (T.components.empty()) throw std::invalid_argument("evolve: a resource type needs components");
  if (j.has("conversions"))
    for (const auto& v : j["conversions"].as_array()) T.conversions.push_back({v["name"].text(), bundle_of(T, v["from"], "a conversion's source"), bundle_of(T, v["to"], "a conversion's target")});
  if (j.has("laws"))
    for (const auto& L : j["laws"].as_array()) {
      kr::Law law;
      law.name = L["name"].text();
      law.weights = bundle_of(T, L["weights"], "a law's weights");
      law.conserved = !L.has("conserved") || L["conserved"].as_bool();
      T.laws.push_back(law);
    }
  if (j.has("declared"))
    for (const auto& d : j["declared"].as_array()) T.declared.push_back(d.text());
  return T;
}

inline ae::Carrier carrier_of(const std::string& c) {
  if (c == "shares") return ae::Carrier::shares;
  if (c == "claims") return ae::Carrier::claims;
  if (c == "units") return ae::Carrier::units;
  if (c == "unit_claims") return ae::Carrier::unit_claims;
  if (c == "items") return ae::Carrier::items;
  if (c == "fixed") return ae::Carrier::fixed;
  throw std::invalid_argument("evolve: a carrier is shares, claims, units, unit_claims, items or fixed, not \"" + c + "\"");
}

inline hs::SpacePtr space_of(const Json& a) {
  const std::string s = a["space"].text();
  if (s == "circle") return std::make_shared<hs::Sphere>(2);
  if (s == "sphere") return std::make_shared<hs::Sphere>(static_cast<std::size_t>(num(a["dimension"], 3)));
  if (s == "levels") return std::make_shared<hs::Ordinal>(static_cast<std::size_t>(num(a["levels"], 2)));
  if (s == "categories") return std::make_shared<hs::Categorical>(static_cast<std::size_t>(num(a["levels"], 2)));
  if (s == "interval") return std::make_shared<hs::RealBox>(std::vector<double>{num(a["lower"], 0)}, std::vector<double>{num(a["upper"], 1)});
  throw std::invalid_argument("evolve: an attribute's space is circle, sphere, levels, categories or interval, not \"" + s + "\"");
}

struct Read {
  ae::Problem problem;
  std::map<std::string, std::size_t> recipient, resource, attribute;
  std::optional<std::size_t> priced;   // the resource a declared price applies to
};

inline Read problem_of(const Json& j) {
  Read r;
  ae::Problem& p = r.problem;
  p.name = j["name"].text("an allocation problem");
  for (const auto& e : j["recipients"].as_array()) {
    ::allocation::rounds::Entity x;
    x.id = e["id"].text();
    x.name = e["name"].text(x.id);
    x.group = e["group"].text();
    if (e.has("at"))
      for (const auto& [k, v] : e["at"].as_object()) x.at[k] = v.as_number();
    if (!r.recipient.emplace(x.id, p.recipients.size()).second) throw std::invalid_argument("evolve: two recipients with the id " + x.id);
    p.recipients.push_back(x);
  }
  for (const auto& q : j["resources"].as_array()) {
    ae::Resource res;
    res.name = q["name"].text();
    res.type = type_of(q["type"]);
    res.carrier = carrier_of(q["carrier"].text("shares"));
    res.pool = bundle_of(res.type, q["pool"], "the pool of " + res.name);
    res.kind = q.has("kind") ? res.type.index(q["kind"].text()) : 0;
    if (q.has("caps"))
      for (const auto& c : q["caps"].as_array()) res.caps.push_back(static_cast<std::int64_t>(c.as_number()));
    res.demand = q["demand"].text();
    res.partial = q.has("partial") && q["partial"].as_bool();
    r.resource[res.name] = p.resources.size();
    p.resources.push_back(res);
  }
  if (j.has("attributes"))
    for (const auto& a : j["attributes"].as_array()) {
      r.attribute[a["name"].text()] = p.attributes.size();
      p.attributes.push_back({a["name"].text(), space_of(a)});
    }
  p.ordered = !j.has("ordered") || j["ordered"].as_bool();
  p.order_by = j["order_by"].text("permutation") == "keys" ? ae::OrderBy::keys : ae::OrderBy::permutation;
  const std::string unmet = j["unmet"].text("skip");
  p.unmet = unmet == "defer" ? heuresis::sequential::Unmet::defer : unmet == "stop" ? heuresis::sequential::Unmet::stop : heuresis::sequential::Unmet::skip;
  if (j.has("precedence"))
    for (const auto& q : j["precedence"].as_array()) {
      const auto b = r.recipient.find(q["before"].text()), a = r.recipient.find(q["after"].text());
      if (b == r.recipient.end() || a == r.recipient.end()) throw std::invalid_argument("evolve: a precedence names a recipient the problem does not have");
      p.precedence.push_back({b->second, a->second, q["why"].text()});
    }
  if (j.has("price") && !j["price"].is_null()) {
    const auto it = r.resource.find(j["price"]["resource"].text());
    if (it == r.resource.end()) throw std::invalid_argument("evolve: a price on a resource the problem does not have");
    r.priced = it->second;
    const double factor = num(j["price"]["factor"], 1);
    if (!(factor > 0)) throw std::invalid_argument("evolve: a price's factor must be positive");
    p.scalars.assign(1 + p.recipients.size(), 0);
    p.scalars[0] = num(j["price"]["start"], 1);
    const std::size_t pr = it->second;
    p.effect = [pr, factor](ae::State& x, std::size_t i) {
      if (x.held[pr][i] > 0) {
        x.scalars[1 + i] = x.held[pr][i] / x.scalars[0];
        x.scalars[0] *= factor;
      }
    };
  }
  return r;
}

struct Term {
  std::string of, form = "linear";
  std::size_t resource = 0;
  double coefficient = 1;
  std::string weight, per;
  std::optional<std::size_t> align;
  std::vector<double> toward;
};

inline double form(const std::string& f, double x) {
  if (f == "linear") return x;
  if (f == "log1p") return std::log1p(std::max(0.0, x));
  if (f == "sqrt") return std::sqrt(std::max(0.0, x));
  if (f == "square") return x * x;
  throw std::invalid_argument("evolve: a term's form is linear, log1p, sqrt or square, not \"" + f + "\"");
}

inline ha::Standing standing_of(const std::string& s) {
  if (s == "verified") return ha::Standing::verified;
  if (s == "computed" || s.empty()) return ha::Standing::computed;
  if (s == "estimated") return ha::Standing::estimated;
  if (s == "proposed") return ha::Standing::proposed;
  throw std::invalid_argument("evolve: a standing is verified, computed, estimated or proposed, not \"" + s + "\"");
}

inline ha::Evaluator economic_evaluator_of(const Json& j, const Read& r, std::shared_ptr<const ae::Composition> c);

inline ha::Evaluator evaluator_of(const Json& j, const Read& r, std::shared_ptr<const ae::Composition> c) {
  const std::string kind = j["kind"].text("closed_form");
  if (kind == "economy" || kind == "density" || kind == "laws") return economic_evaluator_of(j, r, c);
  if (kind != "closed_form") throw std::invalid_argument("evolve: an evaluator's kind is closed_form, economy, density or laws, not \"" + kind + "\"");
  const std::string name = j["name"].text("value");
  const bool maximise = j["objective"].text("maximise") != "minimise";
  std::vector<Term> terms;
  for (const auto& t : j["terms"].as_array()) {
    Term x;
    x.of = t["of"].text("amount");
    if (x.of != "amount" && x.of != "works" && x.of != "items" && x.of != "granted") throw std::invalid_argument("evolve: a term is of amount, works, items or granted");
    if (x.of != "granted") {
      const auto it = r.resource.find(t["resource"].text());
      if (it == r.resource.end()) throw std::invalid_argument("evolve: a term names a resource the problem does not have: " + t["resource"].text());
      x.resource = it->second;
      if (x.of == "works" && (!r.priced || *r.priced != x.resource)) throw std::invalid_argument("evolve: works are read only of the priced resource");
    }
    x.form = t["form"].text("linear");
    form(x.form, 1);  // refuses an unknown form now
    x.coefficient = num(t["coefficient"], 1);
    x.weight = t["weight"].text();
    x.per = t["per"].text();
    if (t.has("align")) {
      const auto it = r.attribute.find(t["align"]["attribute"].text());
      if (it == r.attribute.end()) throw std::invalid_argument("evolve: an alignment names an attribute the problem does not have");
      x.align = it->second;
      for (const auto& v : t["align"]["toward"].as_array()) x.toward.push_back(v.as_number());
    }
    terms.push_back(x);
  }
  const std::string units_per = j["units_per"].text();
  const double units_scale = num(j["units_scale"], 1);
  auto assess = [terms, maximise, name, units_per, units_scale, c](const ae::Allocation& a, calx::random::Stream&) {
    const auto& P = c->problem();
    const std::size_t n = P.recipients.size();
    std::vector<double> v(n, 0);
    for (std::size_t i = 0; i < n; i++) {
      const auto& e = P.recipients[i];
      for (const auto& t : terms) {
        const double w = t.weight.empty() ? 1.0 : e.get(t.weight);
        const double per = t.per.empty() ? 1.0 : e.get(t.per);
        if (!std::isfinite(w) || !std::isfinite(per) || per == 0) throw std::invalid_argument("evolve: the recipient " + e.id + " lacks '" + t.weight + t.per + "'");
        double x = 0;
        if (t.of == "granted") x = a.state.granted[i] ? 1 : 0;
        else if (t.of == "works") x = a.state.scalars[1 + i];
        else x = a.amount[t.resource][i];
        double term = t.coefficient * w * (t.of == "granted" ? x : form(t.form, x / per));
        if (t.align) {
          const auto& pt = a.attribute[*t.align][i].reals;
          double dot = 0, nt = 0;
          for (std::size_t k = 0; k < pt.size() && k < t.toward.size(); k++) dot += pt[k] * t.toward[k], nt += t.toward[k] * t.toward[k];
          term *= std::max(0.0, dot / std::sqrt(nt > 0 ? nt : 1));
        }
        v[i] += term;
      }
    }
    ha::Assessment out;
    ha::Metric m;
    m.name = name, m.role = ha::Role::objective, m.uncertainty = 0;
    double total = 0;
    for (double x : v) total += x;
    m.value = maximise ? -total : total;
    for (std::size_t i = 0; i < n; i++) {
      double u = v[i];
      if (!units_per.empty()) {
        const double d = P.recipients[i].get(units_per);
        u = d > 0 ? v[i] / (d / units_scale) : 0;
      }
      m.units.push_back(u);
    }
    out.metrics.push_back(m);
    return out;
  };
  return ae::evaluator(c, name, ha::Kind::closed_form, standing_of(j["standing"].text("computed")), assess);
}

// ---- economic evaluators (ADR 0141) ----

inline el::Habits habits_of(const Json& j, el::Habits h = {}) {
  if (j.is_null() || !j.is_object()) return h;
  auto share = [&](const char* k, double& v) {
    if (j.has(k)) v = num(j[k], v);
  };
  if (j.has("short_below")) h.short_below = static_cast<el::Amount>(std::llround(num(j["short_below"], 0)));
  share("short_spend", h.short_spend), share("short_local", h.short_local), share("spend", h.spend), share("local", h.local);
  share("wages", h.wages), share("supplies", h.supplies), share("exports", h.exports);
  if (j.has("outside_money")) h.outside_money = static_cast<el::Amount>(std::llround(num(j["outside_money"], 0)));
  h.source = j["source"].text(h.source);
  return h;
}

inline Json habits_json(const el::Habits& h) {
  return Json::object({{"short_below", static_cast<double>(h.short_below)}, {"short_spend", r6(h.short_spend)}, {"short_local", r6(h.short_local)},
                       {"spend", r6(h.spend)}, {"local", r6(h.local)}, {"wages", r6(h.wages)}, {"supplies", r6(h.supplies)}, {"exports", r6(h.exports)},
                       {"outside_money", static_cast<double>(h.outside_money)}, {"source", h.source}});
}

inline std::vector<el::Place> places_of(const Json& j) {
  std::vector<el::Place> out;
  for (const auto& p : j.as_array()) {
    el::Place x;
    x.name = p["name"].text();
    if (x.name.empty()) throw std::invalid_argument("evolve: every place of an economy has a name");
    const double h = num(p["households"], 4), f = num(p["firms"], 2);
    if (!(h >= 1 && f >= 1 && h <= 5000 && f <= 5000)) throw std::invalid_argument("evolve: a place has between 1 and 5,000 households and firms: " + x.name);
    x.households = static_cast<std::size_t>(h), x.firms = static_cast<std::size_t>(f);
    x.household_money = static_cast<el::Amount>(std::llround(num(p["household_money"], 100)));
    x.firm_money = static_cast<el::Amount>(std::llround(num(p["firm_money"], 200)));
    if (x.household_money < 0 || x.firm_money < 0) throw std::invalid_argument("evolve: opening balances are not negative: " + x.name);
    out.push_back(x);
  }
  return out;
}

// Each recipient's value of an attribute named, or a list of numbers, one for each recipient.
inline std::vector<double> per_recipient(const Json& j, const Read& r, const std::string& what) {
  const std::size_t n = r.problem.recipients.size();
  std::vector<double> v;
  if (j.is_string()) {
    for (const auto& e : r.problem.recipients) {
      const double x = e.get(j.text());
      if (!std::isfinite(x)) throw std::invalid_argument("evolve: the recipient " + e.id + " lacks '" + j.text() + "' (" + what + ")");
      v.push_back(x);
    }
  } else {
    for (const auto& x : j.as_array()) v.push_back(x.as_number());
  }
  if (v.size() != n) throw std::invalid_argument("evolve: " + what + " for each recipient");
  return v;
}

inline std::size_t resource_of(const Json& j, const Read& r) {
  if (!j.has("resource")) return 0;
  const auto it = r.resource.find(j["resource"].text());
  if (it == r.resource.end()) throw std::invalid_argument("evolve: an evaluator reads a resource the problem does not have: " + j["resource"].text());
  return it->second;
}

inline std::vector<std::vector<double>> rows_of(const Json& j, const std::string& what) {
  std::vector<std::vector<double>> out;
  for (const auto& row : j.as_array()) {
    std::vector<double> v;
    for (const auto& x : row.as_array()) v.push_back(x.as_number());
    if (!out.empty() && v.size() != out[0].size()) throw std::invalid_argument("evolve: every row of " + what + " has as many values");
    out.push_back(std::move(v));
  }
  return out;
}

// A pooled linear law x' = a x + b u + c learned from a record by least squares, and its refits on the record's steps drawn again.
struct Learned {
  std::vector<std::array<double, 3>> members;  // the fit first, then the refits
  double error = 0, persistence = 0;           // one step ahead, root mean square, in the record
  ki::Support states, inputs;
  std::size_t steps = 0, recipients = 0;
};

inline std::array<double, 3> fit_linear(const std::vector<std::vector<double>>& X, const std::vector<std::vector<double>>& U, const std::vector<std::size_t>& steps) {
  double A[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}}, b[3] = {0, 0, 0};
  for (std::size_t t : steps)
    for (std::size_t i = 0; i < X[t].size(); i++) {
      const double z[3] = {X[t][i], U[t][i], 1.0}, y = X[t + 1][i];
      for (int p = 0; p < 3; p++) {
        b[p] += z[p] * y;
        for (int q = 0; q < 3; q++) A[p][q] += z[p] * z[q];
      }
    }
  // Gaussian elimination with partial pivoting; a singular system (states or inputs that never varied) is refused.
  double scale = 0;
  for (int p = 0; p < 3; p++) scale = std::max(scale, std::fabs(A[p][p]));
  for (int c = 0; c < 3; c++) {
    int piv = c;
    for (int k = c + 1; k < 3; k++)
      if (std::fabs(A[k][c]) > std::fabs(A[piv][c])) piv = k;
    if (std::fabs(A[piv][c]) <= 1e-12 * std::max(1.0, scale)) throw std::invalid_argument("evolve: the record does not identify a linear law (its states or inputs never varied)");
    for (int q = 0; q < 3; q++) std::swap(A[c][q], A[piv][q]);
    std::swap(b[c], b[piv]);
    for (int k = c + 1; k < 3; k++) {
      const double f = A[k][c] / A[c][c];
      for (int q = c; q < 3; q++) A[k][q] -= f * A[c][q];
      b[k] -= f * b[c];
    }
  }
  std::array<double, 3> x{};
  for (int c = 2; c >= 0; c--) {
    double v = b[c];
    for (int q = c + 1; q < 3; q++) v -= A[c][q] * x[static_cast<std::size_t>(q)];
    x[static_cast<std::size_t>(c)] = v / A[c][c];
  }
  return x;
}

inline Learned learn(const Json& record, std::size_t resamples, std::uint64_t seed) {
  const auto X = rows_of(record["states"], "the record's states"), U = rows_of(record["inputs"], "the record's inputs");
  if (X.size() < 4 || U.size() + 1 != X.size()) throw std::invalid_argument("evolve: a record of at least four states and one input fewer, a step apart");
  if (U[0].size() != X[0].size()) throw std::invalid_argument("evolve: a record's states and inputs have a value for each recipient");
  if (resamples > 400) throw std::invalid_argument("evolve: at most 400 refits");
  Learned L;
  L.steps = U.size(), L.recipients = X[0].size();
  std::vector<std::size_t> all(U.size());
  for (std::size_t t = 0; t < all.size(); t++) all[t] = t;
  L.members.push_back(fit_linear(X, U, all));
  calx::random::Stream rng(seed, 0x6c617773u, 0, 0);
  for (std::size_t k = 0; k < resamples; k++) {
    std::vector<std::size_t> draw(all.size());
    for (auto& t : draw) t = static_cast<std::size_t>(rng.uniform(0, static_cast<double>(all.size()))) % all.size();
    try {
      L.members.push_back(fit_linear(X, U, draw));
    } catch (const std::invalid_argument&) {
      // a draw that repeats too few steps to identify the law is left out, and the refits are fewer
    }
  }
  double e = 0, p = 0, m = 0;
  const auto& f = L.members[0];
  for (std::size_t t = 0; t < U.size(); t++)
    for (std::size_t i = 0; i < X[t].size(); i++) {
      const double y = X[t + 1][i];
      e += std::pow(y - (f[0] * X[t][i] + f[1] * U[t][i] + f[2]), 2), p += std::pow(y - X[t][i], 2), m += 1;
    }
  L.error = std::sqrt(e / m), L.persistence = std::sqrt(p / m);
  std::vector<std::string> names;
  for (std::size_t i = 0; i < L.recipients; i++) names.push_back("recipient " + std::to_string(i + 1));
  L.states = ki::support_of(X, names), L.inputs = ki::support_of(U, names);
  return L;
}

inline ha::Evaluator economic_evaluator_of(const Json& j, const Read& r, std::shared_ptr<const ae::Composition> c) {
  const std::string kind = j["kind"].text();
  const std::size_t res = resource_of(j, r);
  const double unit = num(j["unit"], 1);
  if (!(unit > 0)) throw std::invalid_argument("evolve: an evaluator's unit is positive");
  if (kind == "economy") {
    av::EconomySpec s;
    s.resource = res, s.unit = unit;
    s.places = places_of(j["places"]);
    s.habits = habits_of(j["habits"]);
    if (j.has("trade")) s.trade = rows_of(j["trade"], "trade");
    s.spacing = num(j["spacing"], 1);
    s.periods = static_cast<std::size_t>(num(j["periods"], 60));
    s.rounds = static_cast<std::size_t>(num(j["rounds"], 8));
    std::size_t agents = 0;
    for (const auto& p : s.places) agents += p.households + p.firms;
    if (s.periods > 1000 || agents * s.periods > 2000000) throw std::invalid_argument("evolve: an economy of more than 2,000,000 agent-periods a run is refused here");
    return av::economy_evaluator(c, s, j["name"].text("an economy of places"));
  }
  if (kind == "density") {
    av::DensitySpec s;
    s.resource = res, s.unit = unit;
    s.population = per_recipient(j["population"], r, "a population");
    s.mean = per_recipient(j["mean"], r, "a mean holding");
    if (j.has("grid")) s.grid = {num(j["grid"]["lower"], 0), num(j["grid"]["upper"], 12), static_cast<std::size_t>(num(j["grid"]["cells"], 241))};
    if (s.grid.cells < 11 || s.grid.cells > 4001 || !(s.grid.upper > s.grid.lower)) throw std::invalid_argument("evolve: a grid of 11 to 4,001 cells over an interval");
    s.pull = num(j["pull"], s.pull), s.spread = num(j["spread"], s.spread), s.line = num(j["line"], s.line), s.dt = num(j["dt"], s.dt);
    s.between = static_cast<std::size_t>(num(j["between"], 10)), s.after = static_cast<std::size_t>(num(j["after"], 40));
    return av::density_evaluator(c, s, j["name"].text("a law of holdings with a mean field"));
  }
  av::LawsSpec s;
  s.resource = res, s.unit = unit;
  const Learned L = learn(j["record"], static_cast<std::size_t>(num(j["resamples"], 20)), static_cast<std::uint64_t>(num(j["seed"], 1)));
  if (L.recipients != r.problem.recipients.size()) throw std::invalid_argument("evolve: the record has a value for each recipient of the problem");
  for (std::size_t k = 0; k < L.members.size(); k++) {
    const auto m = L.members[k];
    s.laws.push_back({k == 0 ? std::string("the fit") : "refit " + std::to_string(k), 1, [m](const std::vector<double>& x, const std::vector<double>& u) {
                        std::vector<double> y(x.size());
                        for (std::size_t i = 0; i < x.size(); i++) y[i] = m[0] * x[i] + m[1] * u[i] + m[2];
                        return y;
                      }});
  }
  if (j.has("start")) {
    for (const auto& x : j["start"].as_array()) s.start.push_back(x.as_number());
  } else {
    s.start = rows_of(j["record"]["states"], "the record's states").back();
  }
  s.states = L.states, s.inputs = L.inputs;
  s.between = static_cast<std::size_t>(num(j["between"], 1)), s.after = static_cast<std::size_t>(num(j["after"], 4));
  s.margin = num(j["margin"], 0);
  return av::laws_evaluator(c, s, j["name"].text("laws learned from a record"));
}

// ---- answering ----

inline Json verdicts_json(const kr::Report& rep) {
  Json entries = Json::array();
  for (const auto& e : rep.entries)
    entries.push(Json::object({{"law", e.law}, {"claim", e.verdict.claim}, {"status", std::string(kategoria::kernel::word(e.verdict.status))}, {"reason", e.verdict.reason},
                               {"method", std::string(kr::name_of(e.method))}, {"scope", e.scope}}));
  return Json::object({{"type", rep.type}, {"lawful", rep.lawful()}, {"holds", static_cast<double>(rep.count(kategoria::kernel::Status::holds))},
                       {"undecided", static_cast<double>(rep.count(kategoria::kernel::Status::undecided))}, {"fails", static_cast<double>(rep.count(kategoria::kernel::Status::fails))},
                       {"entries", entries}});
}

inline Json verify_op(const Json& q) { return verdicts_json(kr::verify(type_of(q["type"]))); }

inline Json compose_op(const Json& q) {
  const Read r = problem_of(q["problem"]);
  const ae::Composition c(r.problem);
  Json parts = Json::array();
  for (std::size_t k = 0; k < c.space()->size(); k++) {
    const auto& part = c.space()->part(k);
    parts.push(Json::object({{"name", part.name}, {"space", part.space->describe()}, {"unit", r6(part.space->unit())}}));
  }
  Json types = Json::array();
  for (const auto& rep : c.verification()) types.push(verdicts_json(rep));
  return Json::object({{"problem", r.problem.name}, {"recipients", static_cast<double>(r.problem.recipients.size())}, {"parts", parts}, {"types", types}});
}

inline Json allocation_json(const Read& r, const ae::Composition& c, const ae::Kept& k, std::size_t rank) {
  const auto& P = r.problem;
  const auto& a = k.allocation;
  Json amounts = Json::object(), owners = Json::object(), attributes = Json::object();
  for (std::size_t res = 0; res < P.resources.size(); res++) {
    Json row = Json::array();
    for (double v : a.amount[res]) row.push(r6(v));
    amounts.set(P.resources[res].name, row);
    if (P.resources[res].carrier == ae::Carrier::items) {
      Json own = Json::object();
      for (std::size_t item = 0; item < a.owner[res].size(); item++)
        own.set(P.resources[res].type.at(item).second->name, a.owner[res][item] >= 0 ? Json(P.recipients[static_cast<std::size_t>(a.owner[res][item])].id) : Json());
      owners.set(P.resources[res].name, own);
    }
  }
  for (std::size_t at = 0; at < P.attributes.size(); at++) {
    Json row = Json::array();
    for (const auto& v : a.attribute[at]) {
      if (!v.reals.empty()) {
        Json pt = Json::array();
        for (double x : v.reals) pt.push(r6(x));
        row.push(pt);
      } else if (!v.ints.empty()) {
        row.push(static_cast<double>(v.ints[0]));
      } else {
        row.push(Json());
      }
    }
    attributes.set(P.attributes[at].name, row);
  }
  Json granted = Json::array(), unmet = Json::array(), works = Json::array();
  for (auto g : a.granted) granted.push(P.recipients[g].id);
  for (auto g : a.unmet) unmet.push(P.recipients[g].id);
  if (r.priced)
    for (std::size_t i = 0; i < P.recipients.size(); i++) works.push(r6(a.state.scalars[1 + i]));
  Json metrics = Json::array();
  for (const auto& m : k.assessment.metrics) {
    Json units = Json::array();
    for (double u : m.units) units.push(r6(u));
    metrics.push(Json::object({{"name", m.name}, {"role", std::string(ha::name(m.role))}, {"value", r6(m.value)}, {"uncertainty", m.uncertainty ? Json(r6(*m.uncertainty)) : Json()},
                               {"standing", std::string(ha::name(m.standing))}, {"units", units}}));
  }
  Json verdicts = Json::array();
  for (const auto& v : k.lawfulness.verdicts) verdicts.push(v);
  (void)c;
  return Json::object({{"rank", static_cast<double>(rank)}, {"amounts", amounts}, {"owners", owners}, {"attributes", attributes}, {"granted", granted}, {"unmet", unmet},
                       {"works", works}, {"metrics", metrics}, {"lawful", k.lawfulness.lawful}, {"verdicts", verdicts}, {"digest", k.digest}});
}

inline he::Survival survival_of(const std::string& s) {
  if (s == "alps" || s.empty()) return he::Survival::alps;
  if (s == "plus") return he::Survival::plus;
  if (s == "generational") return he::Survival::generational;
  if (s == "aging") return he::Survival::aging;
  if (s == "restricted_tournament") return he::Survival::restricted_tournament;
  throw std::invalid_argument("evolve: a survival is alps, plus, generational, aging or restricted_tournament, not \"" + s + "\"");
}

inline Json search_op(const Json& q) {
  const Read r = problem_of(q["problem"]);
  auto c = std::make_shared<const ae::Composition>(r.problem);
  std::vector<ha::Evaluator> evaluators;
  for (const auto& e : q["evaluators"].as_array()) evaluators.push_back(evaluator_of(e, r, c));
  if (evaluators.empty()) throw std::invalid_argument("evolve: a search needs at least one evaluator");
  ae::Settings st;
  const Json& s = q["settings"];
  st.strategy.generations = static_cast<std::size_t>(num(s["generations"], 200));
  st.strategy.mu = static_cast<std::size_t>(num(s["mu"], 40));
  st.strategy.lambda = static_cast<std::size_t>(num(s["lambda"], static_cast<double>(st.strategy.mu)));
  st.strategy.survival = survival_of(s["survival"].text("alps"));
  if (st.strategy.generations > 5000 || st.strategy.mu * st.strategy.generations > 400000) throw std::invalid_argument("evolve: a search of more than 400,000 evaluations is refused here");
  st.seed = static_cast<std::uint64_t>(num(s["seed"], 1));
  st.keep = static_cast<std::size_t>(num(s["keep"], 8));
  st.keep_apart = num(s["keep_apart"], 0.5);
  st.when = s["when"].text(st.when);
  if (s.has("scales")) {
    st.operators.scales.clear();
    for (const auto& v : s["scales"].as_array()) st.operators.scales.push_back(v.as_number());
  }
  if (q.has("comparing") && !q["comparing"].is_null()) {
    st.comparing.metric = q["comparing"]["metric"].text();
    st.comparing.per = q["comparing"]["per"].text("population");
    for (const auto& p : q["comparing"]["principles"].as_array()) st.comparing.principles.push_back(science::allocation::principle_of(p));
  }
  const auto out = ae::search(c, evaluators, st);
  Json kept = Json::array();
  for (std::size_t k = 0; k < out.kept.size(); k++) kept.push(allocation_json(r, *c, out.kept[k], k + 1));
  Json ids = Json::array();
  for (const auto& e : r.problem.recipients) ids.push(e.id);
  Json comparison;
  if (out.comparison) {
    const auto& C = *out.comparison;
    Json values = Json::array(), maximal = Json::array(), best = Json::array();
    for (const auto& row : C.values) {
      Json rr = Json::array();
      for (double v : row) rr.push(r6(v));
      values.push(rr);
    }
    for (auto i : C.maximal) maximal.push(static_cast<double>(i));
    for (const auto& b : C.best_under) {
      Json bb = Json::array();
      for (auto i : b) bb.push(static_cast<double>(i));
      best.push(bb);
    }
    Json options = Json::array(), principles = Json::array();
    for (const auto& o : C.options) options.push(o);
    for (const auto& p : C.principles) principles.push(p);
    comparison = Json::object({{"options", options}, {"principles", principles}, {"values", values}, {"maximal", maximal}, {"best_under", best}});
  }
  const std::size_t n = r.problem.recipients.size();
  Json commute = Json::array(), declared = Json::array();
  for (std::size_t g = 0; g < n && g < out.order.tested.letters; g++) {
    Json a = Json::array(), b = Json::array();
    for (std::size_t h = 0; h < n; h++) a.push(g != h && out.order.tested.commute(g, h)), b.push(g != h && out.order.declared.commute(g, h));
    commute.push(a), declared.push(b);
  }
  Json witnesses = Json::array(), contradictions = Json::array();
  for (const auto& w : out.order.witnesses) witnesses.push(w);
  for (const auto& w : out.order.contradictions) contradictions.push(w);
  Json record = Json::array();
  for (const auto& e : out.record.entries)
    record.push(Json::object({{"when", e.when}, {"who", e.who}, {"what", e.what}, {"about", e.about}, {"note", e.note}, {"seal", e.seal}}));
  return Json::object({{"problem", r.problem.name}, {"recipients", ids}, {"kept", kept}, {"comparison", comparison}, {"comparison_refused", out.comparison_refused},
                       {"order", Json::object({{"may_matter", out.order.order_may_matter()}, {"states", static_cast<double>(out.order.states)}, {"commute", commute},
                                               {"declared", declared}, {"witnesses", witnesses}, {"contradictions", contradictions}})},
                       {"counts", Json::object({{"genotypes", static_cast<double>(out.evaluated_genotypes)}, {"allocations", static_cast<double>(out.evaluated_allocations)},
                                                {"runs", static_cast<double>(out.evaluator_runs)}, {"reused", static_cast<double>(out.reused)}, {"unlawful", static_cast<double>(out.unlawful_seen)}})},
                       {"record", record}, {"head", out.record.head()}, {"verified", archeion::fixity::verify(out.record).holds()}});
}

inline Json strengths_op(const Json& q) {
  const auto items = static_cast<std::size_t>(num(q["items"], 0));
  std::vector<ha::Judgement> js;
  for (const auto& j : q["judgements"].as_array())
    js.push_back({static_cast<std::size_t>(j["first"].as_number()), static_cast<std::size_t>(j["second"].as_number()), j["first_preferred"].as_bool()});
  const bool order_effect = !q.has("order_effect") || q["order_effect"].as_bool();
  const auto bt = ha::bradley_terry(items, js, order_effect, num(q["prior"], 0));
  Json s = Json::array(), l = Json::array();
  for (double x : bt.strength) s.push(r6(x));
  for (double x : bt.log_strength) l.push(r6(x));
  return Json::object({{"strength", s}, {"log_strength", l}, {"order_effect", r6(bt.order_effect)}, {"log_likelihood", r6(bt.log_likelihood)},
                       {"converged", bt.converged}, {"judgements", static_cast<double>(js.size())}});
}

inline Json learn_op(const Json& q) {
  const Learned L = learn(q["record"], static_cast<std::size_t>(num(q["resamples"], 20)), static_cast<std::uint64_t>(num(q["seed"], 1)));
  Json members = Json::array();
  for (const auto& m : L.members) members.push(Json::object({{"a", r6(m[0])}, {"b", r6(m[1])}, {"c", r6(m[2])}}));
  auto box = [](const ki::Support& S) {
    Json lo = Json::array(), hi = Json::array();
    for (double v : S.lower) lo.push(r6(v));
    for (double v : S.upper) hi.push(r6(v));
    return Json::object({{"lower", lo}, {"upper", hi}});
  };
  return Json::object({{"law", "x' = a x + b u + c, pooled over the recipients"}, {"fit", members[0]}, {"members", members}, {"error", r6(L.error)},
                       {"persistence", r6(L.persistence)}, {"beats_persistence", L.error < L.persistence}, {"steps", static_cast<double>(L.steps)},
                       {"states", box(L.states)}, {"inputs", box(L.inputs)}, {"standing", "estimated"}});
}

inline Json calibrate_op(const Json& q) {
  const auto places = places_of(q["places"]);
  if (places.empty()) throw std::invalid_argument("evolve: a calibration needs the economy's places");
  std::size_t agents = 0;
  for (const auto& p : places) agents += p.households + p.firms;
  if (agents > 20000) throw std::invalid_argument("evolve: a calibration of more than 20,000 agents is refused here");
  const Json& a = q["accounts"];
  el::AccountShares sh{num(a["local"], -1), num(a["wages"], -1), num(a["supplies"], -1), num(a["imported"], 0), a["source"].text()};
  const bool short_too = !q.has("short_too") || q["short_too"].as_bool();
  const el::Habits base = el::habits_from_accounts(sh, habits_of(q["habits"]), short_too);
  const auto c = el::calibrate_spending(places, base, num(q["velocity"], 0), short_too);
  Json id = Json::array(), un = Json::array();
  for (const auto& x : c.identified) id.push(x);
  for (const auto& x : c.unidentified) un.push(x);
  return Json::object({{"habits", habits_json(c.habits)}, {"target", r6(c.target)}, {"achieved", r6(c.achieved)}, {"lowest", r6(c.lowest)}, {"highest", r6(c.highest)},
                       {"runs", static_cast<double>(c.runs)}, {"identified", id}, {"unidentified", un}});
}

inline Json reading_json(const ae::BudgetReading& b) {
  Json f = Json::array(), g = Json::array();
  for (auto j : b.funded) f.push(static_cast<double>(j));
  for (double v : b.nothing_by_group) g.push(r6(v));
  return Json::object({{"funded", f}, {"spent", r6(b.spent)}, {"approvals", b.approvals}, {"share_with_nothing", r6(b.nothing)}, {"by_group", g}});
}

inline Json budget_op(const Json& q) {
  ae::na::Election e;
  e.budget = num(q["budget"], 0);
  for (const auto& c : q["costs"].as_array()) e.costs.push_back(c.as_number());
  for (const auto& v : q["approvals"].as_array()) {
    std::vector<std::size_t> a;
    for (const auto& x : v.as_array()) {
      const double k = x.as_number();
      if (!(k >= 0 && k < static_cast<double>(e.costs.size()))) throw std::invalid_argument("evolve: a ballot approves a project that is not one");
      a.push_back(static_cast<std::size_t>(k));
    }
    e.approvals.push_back(std::move(a));
  }
  if (!(e.budget > 0) || e.costs.empty()) throw std::invalid_argument("evolve: a budget and the projects' costs");
  std::vector<std::size_t> group;
  std::size_t G = 0;
  if (q.has("groups") && q["groups"].is_array()) {
    for (const auto& g : q["groups"].as_array()) {
      const double v = g.as_number();
      group.push_back(v < 0 ? static_cast<std::size_t>(-1) : static_cast<std::size_t>(v));
      if (v >= 0) G = std::max(G, static_cast<std::size_t>(v) + 1);
    }
    if (group.size() != e.approvals.size()) throw std::invalid_argument("evolve: a group for every voter, or none");
  }
  std::vector<std::string> names;
  if (q.has("names"))
    for (const auto& x : q["names"].as_array()) names.push_back(x.text());
  ae::Settings st;
  const Json& s = q["settings"];
  st.strategy.generations = static_cast<std::size_t>(num(s["generations"], 150));
  st.strategy.mu = static_cast<std::size_t>(num(s["mu"], 40));
  st.strategy.lambda = static_cast<std::size_t>(num(s["lambda"], static_cast<double>(st.strategy.mu)));
  st.strategy.survival = survival_of(s["survival"].text("alps"));
  double ballots = 0;
  for (const auto& a : e.approvals) ballots += static_cast<double>(a.size());
  const double evaluations = static_cast<double>(st.strategy.mu + st.strategy.lambda * st.strategy.generations);
  if (st.strategy.generations > 5000 || evaluations * (ballots + static_cast<double>(e.costs.size())) > 4e10)
    throw std::invalid_argument("evolve: a search of more than about 4 x 10^10 approvals counted is refused here; ask for fewer generations");
  st.seed = static_cast<std::uint64_t>(num(s["seed"], 1));
  st.keep = static_cast<std::size_t>(num(s["keep"], 12));
  st.keep_apart = num(s["keep_apart"], 0.0);
  st.when = s["when"].text(st.when);
  const bool propose = !s.has("propose_rules") || s["propose_rules"].as_bool();
  const auto S = ae::search_budget(e, st, group.empty() ? nullptr : &group, G, names, propose);
  Json kept = Json::array(), front = Json::array();
  for (const auto& k : S.kept) kept.push(reading_json(k));
  for (const auto& k : S.front) front.push(reading_json(k));
  Json gn = Json::array();
  if (q.has("group_names"))
    for (const auto& x : q["group_names"].as_array()) gn.push(x.text());
  const auto& o = S.outcome;
  return Json::object({{"voters", static_cast<double>(e.approvals.size())}, {"projects", static_cast<double>(e.costs.size())}, {"budget", e.budget},
                       {"greedy", reading_json(S.greedy)}, {"greedy_per_cost", reading_json(S.greedy_per_cost)}, {"equal_shares", reading_json(S.equal_shares)},
                       {"equal_shares_is_an_order", S.equal_shares_is_an_order}, {"kept", kept}, {"front", front}, {"group_names", gn},
                       {"objectives", Json::array({Json("share of voters with nothing"), Json("approvals not met")})},
                       {"counts", Json::object({{"genotypes", static_cast<double>(o.evaluated_genotypes)}, {"allocations", static_cast<double>(o.evaluated_allocations)},
                                                {"reused", static_cast<double>(o.reused)}})},
                       {"head", o.record.head()}, {"verified", archeion::fixity::verify(o.record).holds()}});
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "evolve.verify") return verify_op(q);
  if (op == "evolve.compose") return compose_op(q);
  if (op == "evolve.search") return search_op(q);
  if (op == "evolve.strengths") return strengths_op(q);
  if (op == "evolve.learn") return learn_op(q);
  if (op == "evolve.calibrate") return calibrate_op(q);
  if (op == "evolve.budget") return budget_op(q);
  throw std::invalid_argument("evolve: no operation \"" + op + "\"");
}

}  // namespace science::evolve
