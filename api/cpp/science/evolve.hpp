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
// Settings are {generations, mu, lambda, seed, keep, keep_apart, survival: alps | plus | …, scales, when}; comparing is {metric, principles,
// per}, principles as science/allocation.hpp reads them.

#pragma once

#include <allocation/evolution/search.hpp>

#include <science/allocation.hpp>
#include <science/json.hpp>

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

inline ha::Evaluator evaluator_of(const Json& j, const Read& r, std::shared_ptr<const ae::Composition> c) {
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

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "evolve.verify") return verify_op(q);
  if (op == "evolve.compose") return compose_op(q);
  if (op == "evolve.search") return search_op(q);
  if (op == "evolve.strengths") return strengths_op(q);
  throw std::invalid_argument("evolve: no operation \"" + op + "\"");
}

}  // namespace science::evolve
