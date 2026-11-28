// Communities as one composition any work may call (ADR 0167): thesmos's constitutions, chronicle, counts and crossings, and the
// communities domain's jar, read and answered as JSON.
//
//   communities.admit   {constitution}                            the faults of form, each under its condition, and the profile
//   communities.run     {constitution, parties, acts, seals?}     each act's outcome and seal, the relational state, the head; with
//                                                                 the seals of an earlier run, whether the record replays as sealed
//   communities.count   {parties, ballots, unit, rule}            the share and its passing under the declared and the undeclared
//                                                                 relation of control
//   communities.jar     {regime, need, free_riders, epistemic, audit_cost, runs, ...}   the jar's means
//   communities.statements {constitution, texts?, parties?, acts?}  the rules as institutional statements, each duty measured
//   communities.holdings {constitution, parties?, acts?}          what the community holds now, its norms of access in words, the
//                                                                 kinds open where no norm speaks, and every access admitted
//   communities.access  {constitution, parties, acts, as, holding, aim, amount?, purpose?, mandate?, kind?, within?, digest?, to?,
//                        until?, turn?}                           the decision of the norms of access, with the norm or its absence,
//                                                                 and whether the fold would admit the act now, with its reason
//   communities.briefing {constitution, parties, acts, as, mandate?, purpose?, turn?}   what the party learns from the record: the
//                                                                 mission, its positions and acts, what it may do with each holding
//                                                                 it may know of and by which norm, its duties, the open work
//   communities.profile {constitution, parties?, acts?, as?}      what a reader may know of the community without joining: its
//                                                                 mission, admission policy, route for claims, the parts of its
//                                                                 constitution shown to the reader, offices and holders, posted
//                                                                 work, the holdings it may know of; with no reader, the public
//                                                                 profile a directory lists
//
//   communities.passport.issue {constitution, parties, acts, mandate, home?, at?, model?, label?, node?, status_at?}
//                                                                 the passport (noetarium.passport/1, thesmos's decision 0007) of a
//                                                                 mandate the home's record has entered, with the home's head; or
//                                                                 the reason the home refuses it
//   communities.passport.witness {constitution, parties, acts, notary?, attester, head: {community, size, root}, turn?, when?, mandate?}
//                                                                 a notary's witness of a head, as a passport lists it, with the act
//                                                                 to add to the notary's record and the notary's head after it
//   communities.passport.verify {passport, homes?: [head], notaries?: [head], at?, host?, credential?}
//                                                                 whether the passport holds for one who knows those heads, shown at
//                                                                 that instant and host with that credential, with the reason
//
// Sessions (Phase 9 of the design of 2026-11-27, Section 9.7): a record folded once and stepped in the engine's memory, with the
// community's behaviour journal kept beside it as a series of its own (Section 4.1).
//
//   communities.open    {constitution, parties, acts, seals?, journal?, replaces?, state?, brief?}   {session, head, journal, refused,
//                                                                 seals, replays?}; a journal given must seal
//   communities.step    {session, head?, acts, state?, brief?}    each item's outcome (an act; an act with "conduct", the episode that
//                                                                 constitutes it; {"observe": episode}; {"disclose": epistemic}), the
//                                                                 new heads and the journal's new entries; {stale, head, reason} when
//                                                                 the head named is not the session's
//   communities.view    {session, what, as?, mandate?, purpose?, from?}   what: state, profile, briefing, items, work, mandate,
//                                                                 behaviour, exit (noetarium.exit/1), provenance, holdings, head, journal
//   communities.close   {session}
//   communities.join.verify {session, journal?} | {constitution, parties, acts, journal}   the join of the chronicle and the journal
//                                                                 in both directions, failing where it breaks
//   communities.exit.verify {package, chronicle?: {size, root}, journal?: {size, root}}    an exit package against the heads known
//   communities.registry {do: profile | listing | read | verify | entries | faults | search, ...}   the domain's registries
//
// Measures of power and value readings rest on nomos and tekmerion through the domain's trajectories, and are answered by an engine of
// their own (science/community_readings.hpp), so that this one stays small.
//
// A constitution may declare its part "behaviour" (thesmos's decision 0008): kinds declared by counts-as are admitted as acts of the kinds
// they count as, and a briefing's "behaviour" gives the kinds declared, the categories of conduct and the rules by law; an act may carry
// the digest of the episode that constituted it in its field "episode".
//
// Presentation and recognition (thesmos's decision 0009): a founding may name its community (field "community") and declare its
// parties (field "parties": [{id, kind, controller, provider}]), so that a record runs from no parties given; an act of kind recognise
// sets the level at which the community recognises another; the office of status its part "passports" names reports another
// community's head ("head of", "size", "root") and a foreign mandate's status ("mandate status", "status"); and an agent presents its
// passport by an act of kind present whose field "passport" is the passport, an object or its text. A field given as an object or a list
// reaches the fold as its JSON text. The state answered gives the community's name, the parties declared by acts, its recognitions, the
// heads its office reported, the mandates of other communities presented here with their status and window, and the passports presented.
//
// Items, routes and rulings (thesmos's decision 0011): a request with a field "type" opens an item on its route (the declared kinds
// apply, question and petition open applications, questions and petitions); an answer naming an item closes it with its outcome; the
// constitution's part "routes" declares where each directive goes. The state answered gives the items and the register of rulings; a
// briefing gives the party's items, what it may answer and the rulings in force; a profile gives the routes, the rulings it shows and
// how many items of each type are open and overdue.
//
// Posted work (thesmos's decision 0012): the constitution's part "work" declares kinds of work; a request with a field "work" posts an
// item, a commitment (or an awarded offer) naming it takes it, and deliveries, checks (an attestation naming the item), reviews (an
// assessment of it) and acceptances follow its kind's declared life. The state answered gives each work item with its state; the profile
// lists posted work when its part "work" is shown; the briefing's work tells a party what it may take, deliver, check, review and accept.
//
// Transmission, counts and compacts (thesmos's decision 0013): where the part "access" declares that operatives transmit to their
// provider, an operative's reading reaches its provider, which the norms must name; an access records the provider reached, and a
// briefing gives what the party's readings reach, the counts-as rules in force and the compacts that stand for the community. A decision
// may count by provider or by declared weight, its rule a threshold at a fraction or a double majority; the state gives each decision's
// weights and each proposal's share by provider and whether it passes by thesmos's tally. A compact is ratified by an amendment with
// "change.compact", "change.terms" (noetarium.compact/1) and "change.reservations", withdrawn with "change.withdraw compact" after its
// notice, and its reservations withdrawn with "change.withdraw reservation"; the state gives the compacts that stand and every record.
//
// Disclosure agreements (thesmos's decision 0010): an act of kind agree, for the principal or for the community, carries the agreement's
// terms (noetarium.disclosure/1, an object or its text) in its field "terms"; denounce ends it after its notice; the record it keeps is
// a holding of kind "epistemic record", consulted for the uses its terms admit. The state answered gives each agreement.
//
// communities.run also takes "brief": [party or {as, mandate, purpose}], and answers "briefings": {party: briefing} on the state it
// folded, so that a work that runs a record need not fold it again for each party.
//
// A constitution is noetarium.constitution/1 (an object or its text). A party is {id, kind, controller, provider}. An act is {author,
// kind, turn, when, mandate, fields: {name: value}, holders: [[party, position]], grant: {id, principal, holder, kinds, matters,
// allowance: {resource: n}, from, to, delegable, parent, slice: {resource: n}, communities, credential, reserves, since, until},
// charges: [{mandate, resource, amount}], body}; a grant's communities, credential (a SHA-256 digest), reservation and term in
// instants (since, until: ISO 8601 in UTC) are those passports add, each left out when not set. A
// ballot is {voter, bearer, yes}. An access is an act of kind consult, use, deposit, modify, lend or withdraw whose fields name the
// holding and, as it needs, an amount, a purpose, a kind, a container (within), a digest, a source and a licence, a borrower (to) and
// a last turn (until).
#pragma once

#include <communities/jar/jar.hpp>
#include <communities/presets/presets.hpp>
#include <communities/registries/registries.hpp>
#include <communities/trajectories/episodes.hpp>

#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/chronicle.hpp>
#include <thesmos/chronicle/conduct.hpp>
#include <thesmos/chronicle/profile.hpp>
#include <thesmos/crossings/crossings.hpp>
#include <thesmos/statements/statements.hpp>

#include <science/json.hpp>

#include <algorithm>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::communities {

namespace ch = thesmos::chronicle;

inline const Json::Array& list(const Json& j) {
  static const Json::Array empty;
  return j.is_array() ? j.as_array() : empty;
}
inline std::string scalar(const Json& j) {
  if (j.is_string()) return j.text();
  if (j.is_number()) {
    const double x = j.as_number();
    if (x == static_cast<double>(static_cast<long long>(x))) return std::to_string(static_cast<long long>(x));
    return Json(x).dump();
  }
  if (j.is_bool()) return j.as_bool() ? "yes" : "no";
  if (j.is_object() || j.is_array()) return j.dump();  // a structured field (a passport, a founding's parties) as its JSON text
  return {};
}
inline long long whole(const Json& j, long long fallback = 0) { return j.is_number() ? static_cast<long long>(j.as_number()) : fallback; }
inline Json strings(const std::vector<std::string>& v) {
  Json out = Json::array();
  for (const auto& s : v) out.push(s);
  return out;
}
template <class Set>
inline Json strings_of(const Set& v) {
  Json out = Json::array();
  for (const auto& s : v) out.push(s);
  return out;
}

inline thesmos::constitutions::Constitution constitution_of(const Json& j) {
  if (j.is_null()) throw std::invalid_argument("communities: a request names its constitution");
  return thesmos::constitutions::read(j.is_string() ? j.text() : j.dump());
}

inline thesmos::kernel::Parties parties_of(const Json& j) {
  thesmos::kernel::Parties ps;
  for (const auto& p : list(j)) {
    const auto kind = thesmos::kernel::kind_named(p["kind"].text("person"));
    if (!kind) throw std::invalid_argument("communities: the party " + p["id"].text() + " is of no declared kind");
    ps.add({p["id"].text(), *kind, p["controller"].text(), p["provider"].text()});
  }
  return ps;
}

inline thesmos::mandates::Mandate mandate_of(const Json& g) {
  thesmos::mandates::Mandate m;
  m.id = g["id"].text(), m.principal = g["principal"].text(), m.holder = g["holder"].text(), m.parent = g["parent"].text();
  for (const auto& k : list(g["kinds"])) m.kinds.insert(k.text());
  for (const auto& k : list(g["matters"])) m.matters.insert(k.text());
  if (g["allowance"].is_object())
    for (const auto& [r, n] : g["allowance"].as_object()) m.allowance[r] = whole(n);
  if (g["slice"].is_object())
    for (const auto& [r, n] : g["slice"].as_object()) m.slice[r] = whole(n);
  m.from = whole(g["from"]), m.to = whole(g["to"]);
  m.delegable = g.has("delegable") && g["delegable"].as_bool();
  // the parts passports add (thesmos's decision 0007)
  for (const auto& k : list(g["communities"])) m.communities.insert(k.text());
  m.credential = g["credential"].text();
  m.reserves = g["reserves"].is_bool() && g["reserves"].as_bool();
  m.since = g["since"].text(), m.until = g["until"].text();
  return m;
}

inline thesmos::acts::Act act_of(const Json& a) {
  thesmos::acts::Act x;
  x.author = a["author"].text(), x.kind = a["kind"].text(), x.turn = whole(a["turn"]), x.when = a["when"].text(), x.mandate = a["mandate"].text(), x.body = a["body"].text();
  if (a["fields"].is_object())
    for (const auto& [k, v] : a["fields"].as_object()) x.fields[k] = scalar(v);
  for (const auto& h : list(a["holders"])) {
    if (h.is_array() && h.as_array().size() == 2) x.holders.push_back({h.as_array()[0].text(), h.as_array()[1].text()});
    else if (h.is_object()) x.holders.push_back({h["party"].text(), h["position"].text()});
  }
  if (a["grant"].is_object()) x.grant = mandate_of(a["grant"]);
  for (const auto& c : list(a["charges"])) x.charges.push_back({c["mandate"].text(), c["resource"].text(), whole(c["amount"])});
  return x;
}

inline Json head_json(const thesmos::crossings::Head& h) { return Json::object({{"size", static_cast<double>(h.size)}, {"root", archeion::fixity::hex(h.root)}}); }

// ---- Passports (thesmos's decision 0007) --------------------------------------------------------------------------------------------

namespace cr = thesmos::crossings;

inline Json named_head_json(const cr::Head& h) {
  Json j = head_json(h);
  j.set("community", h.community);
  return j;
}

// A head as {community, size, root}; a head whose community is not given is named `otherwise`.
inline cr::Head head_of(const Json& j, const std::string& otherwise) {
  cr::Head h;
  h.community = j["community"].text(otherwise);
  h.size = static_cast<std::size_t>(whole(j["size"]));
  const std::string root = j["root"].text();
  if (!thesmos::holdings::is_digest(root)) throw std::invalid_argument("communities: a head's root is a SHA-256 digest of 64 lower-case hexadecimal digits");
  h.root = archeion::fixity::from_hex(root);
  return h;
}

inline Json act_json(const thesmos::acts::Act& a) {
  Json fields = Json::object();
  for (const auto& [k, v] : a.fields) fields.set(k, v);
  return Json::object({{"author", a.author}, {"kind", a.kind}, {"turn", static_cast<double>(a.turn)}, {"when", a.when}, {"mandate", a.mandate}, {"fields", fields}});
}

// ---- Holdings and the norms of access (thesmos's holdings, decision 0006) ----------------------------------------------------------

namespace hd = thesmos::holdings;

inline long long turn_of(const ch::State& s, const Json& q) { return q["turn"].is_number() ? whole(q["turn"]) : (s.acts.empty() ? 0 : s.acts.back().turn); }

inline Json holding_json(const ch::State& s, const hd::Holding& h, long long turn) {
  Json j = Json::object({{"id", h.id}, {"kind", h.kind}, {"custodian", h.custodian}, {"office", h.office}, {"within", h.within}, {"review", h.review},
                         {"digest", h.provenance.digest}, {"source", h.provenance.source}, {"licence", h.provenance.licence}, {"depositor", h.depositor},
                         {"since", static_cast<double>(h.since)}, {"pending", h.pending}, {"checked_by", h.checked_by}, {"withdrawn", h.withdrawn},
                         {"versions", static_cast<double>(h.versions.size())}, {"visibility", hd::text(h.visibility)}});
  if (h.metered()) {
    j.set("unit", h.unit);
    j.set("quantity", static_cast<double>(h.quantity));
    j.set("available", static_cast<double>(ch::available(s, h.id, turn)));
  }
  return j;
}

inline Json norm_json(const hd::Norm& n) {
  return Json::object({{"id", n.id}, {"text", hd::text(n)}, {"deontic", std::string(hd::word(n.deontic))}, {"aim", std::string(hd::word(n.aim))},
                       {"object", n.object.over == hd::Over::any ? Json() : Json(n.object.value)}, {"conditions", !n.conditions.none()}});
}

inline Json access_json(const ch::Access& x) {
  Json j = Json::object({{"turn", static_cast<double>(x.turn)}, {"author", x.author}, {"bearer", x.bearer}, {"holding", x.holding}, {"aim", x.aim},
                         {"amount", static_cast<double>(x.amount)}, {"purpose", x.purpose}, {"norm", x.norm}});
  if (x.aim == "lend") j.set("to", x.to), j.set("until", static_cast<double>(x.until));
  if (!x.transmit.empty()) j.set("transmit", x.transmit);
  if (!x.reached.empty()) j.set("reached", x.reached);  // decision 0013
  return j;
}

inline Json decision_json(const hd::Decision& d) {
  return Json::object({{"permitted", d.permitted}, {"spoken", d.spoken}, {"open", d.open}, {"norm", d.norm.empty() ? Json() : Json(d.norm)},
                       {"reason", d.reason}, {"unmet", strings(d.unmet)}});
}

inline Json holdings_part(const ch::State& s, long long turn) {
  Json held = Json::array(), norms = Json::array(), open = Json::object(), accesses = Json::array();
  for (const auto& [id, h] : s.holdings) held.push(holding_json(s, h, turn));
  for (const auto& n : s.rules.holdings.norms) norms.push(norm_json(n));
  for (const auto& [kind, aims] : s.rules.holdings.open) {
    Json a = Json::array();
    for (auto aim : aims) a.push(std::string(hd::word(aim)));
    open.set(kind, a);
  }
  const std::size_t from = s.accesses.size() > 200 ? s.accesses.size() - 200 : 0;
  for (std::size_t k = from; k < s.accesses.size(); k++) accesses.push(access_json(s.accesses[k]));
  return Json::object({{"holdings", held}, {"norms", norms}, {"open", open}, {"accesses", accesses}, {"access_count", static_cast<double>(s.accesses.size())}});
}

inline Json item_json(const ch::Item& i, long long now) {
  Json out = Json::object({{"id", i.id}, {"type", i.type}, {"author", i.author}, {"bearer", i.bearer}, {"opened", static_cast<double>(i.opened)},
                           {"due", i.due < 0 ? Json() : Json(static_cast<double>(i.due))}, {"status", i.status}});
  if (now >= 0) out.set("overdue", ch::overdue(i, now));
  for (const auto& [k, v] : std::vector<std::pair<const char*, const std::string*>>{{"position", &i.position}, {"about", &i.about}, {"subject", &i.subject}, {"kind", &i.kind},
                                                                                    {"reviews", &i.reviews}, {"deposit", &i.deposit}, {"proposal", &i.proposal},
                                                                                    {"closed by", &i.closed_by}, {"finding", &i.finding}})
    if (!v->empty()) out.set(k, *v);
  if (i.closed >= 0) out.set("closed", static_cast<double>(i.closed));
  return out;
}
inline Json ruling_json(const ch::Ruling& r) {
  return Json::object({{"id", r.id}, {"subject", r.subject}, {"reading", r.reading}, {"by", r.by}, {"turn", static_cast<double>(r.turn)}, {"in force", r.in_force},
                       {"overrules", r.overrules.empty() ? Json() : Json(r.overrules)}, {"overruled by", r.overruled_by.empty() ? Json() : Json(r.overruled_by)}});
}

inline Json briefing_json(const ch::Briefing& b) {
  Json entries = Json::array(), may = Json::array(), duties = Json::array(), work = Json::array();
  for (const auto& e : b.entries) {
    const auto& t = b.targets[e.target];
    const std::string where = t.fresh ? (t.within.empty() ? "a new " + t.kind : "a new " + t.kind + " within the " + t.within.front()) : t.id;
    Json one = Json::object({{"holding", t.fresh ? Json() : Json(t.id)}, {"within", t.fresh && !t.within.empty() ? Json(t.within.front()) : Json()},
                             {"kind", t.kind}, {"fresh", t.fresh}, {"aim", std::string(hd::word(e.aim))}, {"where", where}, {"covered", e.covered},
                             {"norms", strings(e.norms)}});
    one.set("decision", decision_json(e.decision));
    if (!e.reaches.empty()) one.set("reaches", e.reaches);  // decision 0013
    if (e.remaining >= 0) one.set("remaining", static_cast<double>(e.remaining));
    if (t.metered) one.set("unit", t.unit);
    entries.push(one);
    if (e.decision.permitted && e.covered) may.push(std::string(hd::word(e.aim)) + " " + where + (e.decision.open ? " (open)" : " (" + e.decision.norm + ")"));
  }
  for (const auto& n : b.duties) duties.push(Json::object({{"id", n.id}, {"text", hd::text(n)}}));
  for (const auto& w : b.work) work.push(Json::object({{"kind", w.kind}, {"id", w.id}, {"what", w.what}}));
  return Json::object({{"party", b.party}, {"bearer", b.bearer}, {"mission", b.mission}, {"positions", strings(b.positions)}, {"acts", strings(b.acts)},
                       {"entries", entries}, {"may", may}, {"duties", duties}, {"work", work},
                       {"behaviour", Json::object({{"declared", strings(b.declared)}, {"conduct", strings(b.conduct)}, {"by law", strings(b.by_law)}})},
                       {"items", [&] {
                          Json items = Json::array();
                          for (const auto& i : b.items) items.push(item_json(i, -1));
                          return items;
                        }()},
                       {"rulings", strings(b.rulings)},
                       // decision 0013: what the party's readings reach, the counts-as rules in force, the compacts that stand for the community
                       {"reaches", b.reaches.empty() ? Json() : Json(b.reaches)}, {"counts as", strings(b.counts_as)}, {"compacts", strings(b.compacts)}});
}

// A decision in force, with its weights when it declares them (decision 0013).
inline Json decision_part(const thesmos::constitutions::Decision& d, bool tempo) {
  Json one = Json::object({{"body", d.body}, {"rule", d.rule}, {"count by", d.count_by}});
  if (tempo) one.set("tempo", d.tempo);
  if (!d.weights.empty()) {
    Json w = Json::object();
    for (const auto& [member, x] : d.weights) w.set(member, x.str());
    one.set("weights", w);
  }
  return one;
}
// A community's ratification of a compact, as its record keeps it (decision 0013).
inline Json compact_json(const thesmos::compacts::Ratification& r, long long turn) {
  Json reservations = Json::array();
  for (const auto& q : r.reservations)
    reservations.push(Json::object({{"article", q.article}, {"since", static_cast<double>(q.since)}, {"until", q.until < 0 ? Json() : Json(static_cast<double>(q.until))}}));
  return Json::object({{"id", r.id}, {"party", r.party}, {"since", static_cast<double>(r.since)}, {"terms", r.has_terms}, {"digest", r.digest.empty() ? Json() : Json(r.digest)},
                       {"reservations", reservations}, {"withdrawn", r.withdrawn < 0 ? Json() : Json(static_cast<double>(r.withdrawn))},
                       {"effective", r.withdrawn < 0 ? Json() : Json(static_cast<double>(r.effective()))}, {"stands", r.stands(turn)}});
}

// The state a request's record folds to (its acts each put to the rules; those refused leave it as it was).
inline ch::State folded(const Json& q) {
  ch::State s(parties_of(q["parties"]), constitution_of(q["constitution"]));
  for (const auto& a : list(q["acts"])) ch::step(s, act_of(a));
  return s;
}

// The state a request reads: its session's, when it names one (Phase 9, below), else its record's, folded into `kept`.
inline const ch::State& state_of(const Json& q, std::optional<ch::State>& kept);
// A copy of that state, for an operation that tries an act on it.
inline ch::State copied(const Json& q) {
  std::optional<ch::State> kept;
  return ch::State(state_of(q, kept));
}

inline Json holdings_op(const Json& q) {
  std::optional<ch::State> kept;
  const ch::State& s = state_of(q, kept);
  return holdings_part(s, turn_of(s, q));
}

inline Json briefing_op(const Json& q) {
  std::optional<ch::State> kept;
  const ch::State& s = state_of(q, kept);
  const std::string who = q["as"].text();
  if (who.empty()) throw std::invalid_argument("communities: a briefing names the party it is for (as)");
  return briefing_json(ch::brief(s, who, q["mandate"].text(), q["purpose"].text(), turn_of(s, q)));
}

// An access decided: by the norms (the situation the record gives the party at the turn) and by the fold (the act tried on the state,
// which also asks whether the holding allows it, the party's authority and its charges).
inline Json access_op(const Json& q) {
  ch::State s = copied(q);
  const std::string who = q["as"].text(), id = q["holding"].text(), aim_word = q["aim"].text();
  const auto aim = hd::aim_named(aim_word);
  if (who.empty() || id.empty() || !aim) throw std::invalid_argument("communities: an access names the party (as), the holding and an aim of access");
  const long long turn = turn_of(s, q), amount = whole(q["amount"]);
  hd::Target t;
  const auto it = s.holdings.find(id);
  if (it != s.holdings.end()) t = ch::target_of(s, it->second);
  else {
    t.fresh = true, t.id = id, t.kind = q["kind"].text();
    if (const std::string within = q["within"].text(); !within.empty()) {
      t.within = ch::containers(s, within);
      if (const auto c = s.holdings.find(within); c != s.holdings.end()) {
        t.custodian = c->second.custodian, t.office = c->second.office;
        if (t.kind.empty()) t.kind = c->second.kind;
      }
    } else
      t.custodian = who, t.office = false;
  }
  thesmos::acts::Act a;
  a.author = who, a.kind = aim_word, a.turn = turn, a.mandate = q["mandate"].text();
  a.fields["holding"] = id;
  for (const char* k : {"amount", "purpose", "kind", "within", "digest", "source", "licence", "to", "until", "transmit"})
    if (!q[k].is_null()) a.fields[k] = scalar(q[k]);
  // the provider an operative's reading reaches, as the fold reads it (decision 0013)
  const auto d = hd::decide(s.rules.holdings, t, {*aim, amount, q["purpose"].text(), q["transmit"].text(), ch::reach_of(s, a)}, ch::situation(s, who, q["mandate"].text(), turn));
  const bool metered = it != s.holdings.end() && it->second.metered();
  const long long before = metered ? ch::available(s, id, turn) : 0;
  const auto o = ch::step(s, a);
  Json out = Json::object({{"holding", id}, {"aim", aim_word}, {"as", who}, {"decision", decision_json(d)},
                           {"fold", Json::object({{"admitted", o.admitted}, {"reason", o.reason}})}});
  if (metered) out.set("available", static_cast<double>(before));
  return out;
}

// The profile of a community for a reader (none: the public profile). Each part of the constitution the reader is shown is written in
// the shape of its written form; the parts kept from the reader are named.
inline Json profile_json(const ch::State& s, const std::string& reader) {
  const ch::Profile p = ch::profile(s, reader);
  const auto& c = p.rules;
  Json out = Json::object({{"name", p.name}, {"order", p.order}, {"purpose", p.purpose}, {"reader", reader.empty() ? Json() : Json(reader)},
                           {"shown", strings_of(p.shown)}, {"withheld", strings_of(p.withheld)}});
  Json admission = Json::object();  // the boundary rules: how each position is entered and left, and its term (decision 0014)
  for (const auto& [position, b] : c.boundary) {
    Json e = Json::object({{"entry", b.has_entry ? Json(b.entry.mode) : Json()}, {"exit", b.exit}});
    if (!b.entry.decided_by.empty()) e.set("decided by", b.entry.decided_by);
    if (!b.entry.among.empty()) e.set("among", strings(b.entry.among));
    if (!b.entry.after.empty()) e.set("after", b.entry.after);
    if (b.term > 0) e.set("term", static_cast<double>(b.term));
    admission.set(position, e);
  }
  out.set("admission", admission);
  out.set("claims", Json::object({{"route", c.claims.route}, {"decided by", c.claims.decided_by}, {"review", c.claims.review}, {"outsiders", c.claims.outsiders}}));
  // Routes, always shown; the register of rulings, when shown; open and overdue items by type, which every reader sees (decision 0011).
  if (c.routes.declared) {
    auto route = [](const thesmos::constitutions::Route& r) {
      return r.empty ? Json("none") : Json::object({{"body", r.body}, {"review", r.review}, {"term", r.term}, {"subjects", strings(r.subjects)}});
    };
    Json routes = Json::object({{"applications", route(c.routes.applications)}, {"questions", route(c.routes.questions)}});
    Json petitions = Json::object();
    for (const auto& [kind, q] : c.routes.petitions) petitions.set(kind, Json::object({{"by", q.by}, {"within", q.within}, {"level", q.level}, {"to", q.to}, {"carrier", q.carrier}}));
    routes.set("petitions", petitions);
    for (const auto& [type, r] : c.routes.types) routes.set(type, route(r));
    out.set("routes", routes);
  }
  if (p.shown.count("rulings")) {
    Json rulings = Json::array();
    for (const auto& r : p.rulings) rulings.push(ruling_json(r));
    out.set("rulings", rulings);
  }
  Json open_items = Json::object();
  for (const auto& [type, n] : p.items) open_items.set(type, Json::object({{"open", static_cast<double>(n.first)}, {"overdue", static_cast<double>(n.second)}}));
  out.set("items", open_items);
  if (p.shown.count("work")) {  // posted work with its states (thesmos's decision 0012)
    Json posted = Json::array();
    for (const auto& w : p.posted_work)
      posted.push(Json::object({{"id", w.id}, {"kind", w.kind}, {"state", w.state}, {"poster", w.poster}, {"matter", w.matter}, {"posted", static_cast<double>(w.posted)},
                                {"due", w.due < 0 ? Json() : Json(static_cast<double>(w.due))}, {"overdue", w.overdue}}));
    out.set("posted work", posted);
  }
  if (p.shown.count("positions")) {
    Json positions = Json::object();
    for (const auto& [name, kinds] : c.positions) positions.set(name, strings(kinds));
    out.set("positions", positions);
  }
  if (p.shown.count("holders")) {
    Json offices = Json::array();
    for (const auto& o : p.offices) offices.push(Json::object({{"position", o.position}, {"holders", strings(o.holders)}}));
    out.set("offices", offices);
  }
  if (p.shown.count("choice")) {
    Json choice = Json::array();
    for (const auto& r : c.choice) {
      Json one = Json::object({{"who", r.who}});
      if (!r.may.empty()) one.set("may", strings(r.may));
      if (!r.must.empty()) one.set("must", strings(r.must));
      if (!r.must_never.empty()) one.set("must never", strings(r.must_never));
      choice.push(one);
    }
    out.set("choice", choice);
  }
  if (p.shown.count("bodies")) {
    Json bodies = Json::object(), seats = Json::object();
    for (const auto& [name, b] : c.bodies) {
      bodies.set(name, strings(b.of));
      if (b.seats > 0) seats.set(name, static_cast<double>(b.seats));  // decision 0014
    }
    out.set("bodies", bodies);
    out.set("seats", seats);
  }
  if (p.shown.count("aggregation")) {
    Json decisions = Json::object();
    for (const auto& [name, d] : c.aggregation) decisions.set(name, decision_part(d, true));
    out.set("aggregation", decisions);
  }
  if (p.shown.count("circulation")) out.set("circulation", strings(c.circulation));
  if (p.shown.count("custody")) {
    Json powers = Json::object();
    for (const auto& [power, holders] : c.custody.powers) powers.set(power, strings(holders));
    out.set("custody", powers);
  }
  if (p.shown.count("amendment")) {
    Json levels = Json::object();
    for (const auto& [level, decision] : c.amendment.by_level) levels.set(level, decision);
    out.set("amendment", Json::object({{"levels", levels}, {"entrenched", strings(c.amendment.entrenched)}}));
  }
  if (p.shown.count("external")) {  // the compacts that stand, and each with its reservations (decision 0013)
    const long long turn = s.acts.empty() ? 0 : s.acts.back().turn;
    out.set("compacts", strings(thesmos::constitutions::compact_ids(c, turn)));
    Json records = Json::array();
    for (const auto& r : c.compacts) records.push(compact_json(r, turn));
    out.set("compact records", records);
    // the registries it says it is listed in, and its envoys as objects (decision 0014)
    out.set("listed in", strings(p.listed_in));
    Json envoys = Json::array();
    for (const auto& e : p.envoys) envoys.push(Json::object({{"to", e.to}, {"holder", e.holder}, {"position", e.position}}));
    out.set("envoys", envoys);
  }
  if (p.shown.count("work")) {
    Json work = Json::array();
    for (const auto& w : p.work) work.push(Json::object({{"kind", w.kind}, {"id", w.id}, {"matter", w.matter}, {"by", w.by}, {"turn", static_cast<double>(w.turn)}}));
    out.set("work", work);
  }
  if (p.shown.count("holdings")) {
    Json held = Json::array();
    const long long turn = s.acts.empty() ? 0 : s.acts.back().turn;
    for (const auto& h : p.holdings) held.push(holding_json(s, h, turn));
    out.set("holdings", held);
  }
  if (p.shown.count("access")) {
    Json norms = Json::array();
    for (const auto& n : c.holdings.norms) norms.push(norm_json(n));
    out.set("norms", norms);
  }
  return out;
}

inline Json profile_op(const Json& q) {
  std::optional<ch::State> kept;
  return profile_json(state_of(q, kept), q["as"].text());
}

inline Json work_json(const ch::State& s, long long now);

inline Json state_json(const ch::State& s) {
  Json positions = Json::array(), mandates = Json::array(), commitments = Json::array(), claims = Json::array(), proposals = Json::array();
  for (const auto& [k, since] : s.positions) positions.push(Json::object({{"party", k.first}, {"position", k.second}, {"since", static_cast<double>(since)}}));
  for (const auto& [id, m] : s.mandates) {
    Json used = Json::object();
    if (const auto u = s.used.find(id); u != s.used.end())
      for (const auto& [r, n] : u->second) used.set(r, static_cast<double>(n));
    mandates.push(Json::object({{"id", id}, {"principal", m.principal}, {"holder", m.holder}, {"revoked", s.revoked.count(id) > 0}, {"used", used}}));
  }
  for (const auto& [id, c] : s.commitments)
    commitments.push(Json::object({{"id", id}, {"debtor", c.debtor}, {"creditor", c.creditor}, {"matter", c.matter}, {"amount", static_cast<double>(c.amount)},
                                   {"status", std::string(thesmos::commitments::word(c.status))}, {"by", c.by}}));
  for (const auto& [id, c] : s.claims) claims.push(Json::object({{"id", id}, {"claimant", c.claimant}, {"against", c.against}, {"status", c.status}, {"finding", c.finding}}));
  Json aggregation = Json::object();  // the decisions in force, after amendments
  for (const auto& [name, d] : s.rules.aggregation) aggregation.set(name, decision_part(d, false));
  for (const auto& [id, p] : s.proposals) {
    Json votes = Json::array(), shares = Json::object();
    for (const auto& b : p.ballots()) votes.push(Json::object({{"voter", b.voter}, {"bearer", b.bearer}, {"yes", b.yes}}));
    for (const char* unit : {"operative", "origin", "person", "community", "provider"})
      shares.set(unit, thesmos::procedures::count(s.parties, p.ballots(), *thesmos::procedures::unit_named(unit)).share().str());
    // whether the proposal passes now under its decision's rule, unit of count and weights in force (amendments included), by thesmos's tally
    Json passes = Json(), rule = Json(), count_by = Json();
    if (const auto d = s.rules.aggregation.find(p.decision); d != s.rules.aggregation.end()) {
      rule = Json(d->second.rule), count_by = Json(d->second.count_by);
      const auto unit = thesmos::procedures::unit_named(d->second.count_by);
      const auto r = thesmos::procedures::read_rule(d->second.rule);
      if (unit && r) {
        const auto tally = thesmos::procedures::decide(s.parties, p.ballots(), *unit, *r, d->second.weights);
        passes = Json(tally.passed);
        if (*unit == thesmos::procedures::Unit::weight) shares.set("weight", tally.count.share().str());
        if (r->form == thesmos::procedures::Rule::Form::double_majority) shares.set("members", tally.members.share().str());
      }
    }
    proposals.push(Json::object({{"id", id}, {"decision", p.decision}, {"status", p.status}, {"votes", votes}, {"shares", shares},
                                 {"passes", passes}, {"rule", rule}, {"count by", count_by}, {"author", p.author}, {"opened", static_cast<double>(p.opened)}}));
  }
  Json out = Json::object({{"positions", positions}, {"mandates", mandates}, {"commitments", commitments}, {"claims", claims}, {"proposals", proposals},
                           {"compacts", strings(thesmos::constitutions::compact_ids(s.rules, s.acts.empty() ? 0 : s.acts.back().turn))},
                           {"assessments", static_cast<double>(s.assessments.size())}, {"aggregation", aggregation}});
  {  // every compact the community has ratified, with its reservations and withdrawal (decision 0013), only when there is one
    Json records = Json::array();
    for (const auto& r : s.rules.compacts) records.push(compact_json(r, s.acts.empty() ? 0 : s.acts.back().turn));
    if (!s.rules.compacts.empty()) out.set("compact records", records);
  }
  const Json held = holdings_part(s, s.acts.empty() ? 0 : s.acts.back().turn);  // what it holds, the norms of access, the accesses admitted
  for (const char* k : {"holdings", "norms", "open", "accesses", "access_count"}) out.set(k, held[k]);
  // Presentation and recognition (thesmos's decision 0009): the community's name, the parties its acts declared, its recognitions, the
  // heads of other communities its office of status reported, the mandates of other communities presented here, the passports presented.
  Json declared = Json::array(), recognitions = Json::array(), heads = Json::object(), foreign = Json::array(), presented = Json::object();
  for (const auto& id : s.declared)
    if (s.parties.has(id)) {
      const auto& p = s.parties.at(id);
      declared.push(Json::object({{"id", id}, {"kind", std::string(thesmos::kernel::word(p.kind))}, {"controller", p.controller}, {"provider", p.provider}}));
    }
  for (const auto& [of, level] : s.recognitions) recognitions.push(Json::object({{"of", of}, {"level", level}, {"matter", Json()}}));
  for (const auto& [k, level] : s.recognised_for) recognitions.push(Json::object({{"of", k.first}, {"level", level}, {"matter", k.second}}));
  for (const auto& [community, known] : s.heads) {
    Json list_of = Json::array();
    for (const auto& h : known) list_of.push(head_json(h));
    heads.set(community, list_of);
  }
  for (const auto& [id, f] : s.foreign) {
    Json window = Json::object();
    if (const auto w = s.window.find(id); w != s.window.end())
      for (const auto& [r, n] : w->second) window.set(r, static_cast<double>(n));
    foreign.push(Json::object({{"mandate", id}, {"home", f.home}, {"status", f.status}, {"since", static_cast<double>(f.since)},
                               {"reported", f.reported < 0 ? Json() : Json(static_cast<double>(f.reported))}, {"window", window}}));
  }
  for (const auto& [holder, digest] : s.presented) presented.set(holder, digest);
  out.set("community", s.community);
  out.set("declared", declared);
  out.set("recognitions", recognitions);
  out.set("heads", heads);
  out.set("foreign", foreign);
  out.set("presented", presented);
  // Disclosure agreements (thesmos's decision 0010): each with its principal, the digest of the terms both sides ratified, its record,
  // when it stands in force and ends, whether it is in force at the record's last turn, and the uses its record admits.
  Json agreements = Json::array();
  const long long now = s.acts.empty() ? 0 : s.acts.back().turn;
  auto turn_or_null = [](long long t) { return t < 0 ? Json() : Json(static_cast<double>(t)); };
  for (const auto& [id, g] : s.agreements)
    agreements.push(Json::object({{"id", id}, {"principal", g.terms.principal}, {"digest", g.digest}, {"record", g.terms.holding},
                                  {"since", turn_or_null(ch::in_force_since(g))}, {"ends", turn_or_null(ch::ends_at(g))}, {"in force", ch::in_force(g, now)},
                                  {"denounced", turn_or_null(g.denounced)}, {"admits", strings_of(g.terms.admits)}, {"never", strings_of(g.terms.never)},
                                  {"then", g.terms.then}}));
  out.set("agreements", agreements);
  out.set("work", work_json(s, now));
  // Items and the register of rulings (thesmos's decision 0011).
  Json items = Json::array(), rulings = Json::array();
  for (const auto& [id, i] : s.items) items.push(item_json(i, now));
  for (const auto& [id, r] : s.rulings) rulings.push(ruling_json(r));
  out.set("items", items);
  out.set("rulings", rulings);
  // Orders, terms and seats (thesmos's decision 0014): the order a label, each position's term in turns and each body's seats.
  Json terms = Json::object(), seats = Json::object();
  for (const auto& [position, b] : s.rules.boundary)
    if (b.term > 0) terms.set(position, static_cast<double>(b.term));
  for (const auto& [name, b] : s.rules.bodies)
    if (b.seats > 0) seats.set(name, static_cast<double>(b.seats));
  out.set("order", s.rules.order);
  out.set("terms", terms);
  out.set("seats", seats);
  return out;
}

// Posted work (thesmos's decision 0012): each item with its kind, state, poster, takings, deliveries and reviews.
inline Json work_json(const ch::State& s, long long now) {
  Json posted = Json::array();
  for (const auto& [id, i] : s.work_items) {
    Json takings = Json::array(), deliveries = Json::array(), reviews = Json::array();
    for (const auto& t : i.takings)
      takings.push(Json::object({{"commitment", t.commitment}, {"taker", t.taker}, {"bearer", t.bearer}, {"provider", t.provider}, {"since", static_cast<double>(t.since)},
                                 {"state", thesmos::work::taking_state(i.life, i, t, now)}}));
    for (const auto& d : i.deliveries)
      deliveries.push(Json::object({{"commitment", d.commitment}, {"artefact", d.artefact}, {"by", d.by}, {"provider", d.provider}, {"turn", static_cast<double>(d.turn)}, {"check", d.check}}));
    for (const auto& r : i.reviews)
      reviews.push(Json::object({{"by", r.by}, {"provider", r.provider}, {"verdict", r.verdict}, {"turn", static_cast<double>(r.turn)}, {"delivery", static_cast<double>(r.delivery)}}));
    posted.push(Json::object({{"id", id}, {"kind", i.kind}, {"state", ch::work_state(s, i, now)}, {"poster", i.poster}, {"matter", i.matter}, {"of", i.of},
                              {"posted", static_cast<double>(i.posted)}, {"due", i.due < 0 ? Json() : Json(static_cast<double>(i.due))},
                              {"overdue", thesmos::work::overdue(i.life, i, now)}, {"takings", takings}, {"deliveries", deliveries}, {"reviews", reviews}}));
  }
  return posted;
}

inline Json admit_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  Json faults = Json::array();
  for (const auto& f : thesmos::constitutions::admission(c)) faults.push(Json::object({{"condition", static_cast<double>(f.condition)}, {"reason", f.reason}}));
  const auto p = ::communities::presets::profile(c);
  return Json::object({{"name", c.name},
                       {"admitted", faults.as_array().empty()},
                       {"faults", faults},
                       {"profile", Json::object({{"positions", static_cast<double>(p.positions)}, {"decisions", static_cast<double>(p.decisions)}, {"counted_by", strings_of(p.counted_by)},
                                                 {"open", strings_of(p.open_positions)}, {"operatives_may_hold", strings_of(p.operative_positions)},
                                                 {"communities_may_hold", strings_of(p.community_positions)}, {"circulation", strings(p.circulation)}, {"route_for_claims", p.route_for_claims},
                                                 {"holdings", strings_of(p.holdings)}, {"norms_of_access", static_cast<double>(p.norms)}})}});
}

inline Json run_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  const auto ps = parties_of(q["parties"]);
  ch::State s(ps, c);
  Json outcomes = Json::array(), seals = Json::array();
  std::size_t n = 0;
  for (const auto& a : list(q["acts"])) {
    const auto o = ch::step(s, act_of(a));
    n++;
    outcomes.push(Json::object({{"n", static_cast<double>(n)}, {"admitted", o.admitted}, {"reason", o.reason}, {"seal", o.admitted ? Json(s.journal.entries.back().seal) : Json()}}));
    if (o.admitted) seals.push(s.journal.entries.back().seal);
  }
  Json out = Json::object({{"outcomes", outcomes}, {"seals", seals}, {"state", state_json(s)}, {"head", head_json(thesmos::crossings::head(s.community, s))}, {"digest", ch::relational_digest(s)}});
  if (q["brief"].is_array()) {  // briefings on the state just folded
    Json briefings = Json::object();
    for (const auto& b : q["brief"].as_array()) {
      const std::string who = b.is_string() ? b.text() : b["as"].text();
      if (who.empty() || !s.parties.has(who)) continue;
      briefings.set(who, briefing_json(ch::brief(s, who, b.is_object() ? b["mandate"].text() : std::string(), b.is_object() ? b["purpose"].text() : std::string())));
    }
    out.set("briefings", briefings);
  }
  if (q["seals"].is_array()) {
    bool same = q["seals"].as_array().size() == s.journal.entries.size();
    std::size_t first = 0;
    for (std::size_t k = 0; same && k < s.journal.entries.size(); k++)
      if (q["seals"].as_array()[k].text() != s.journal.entries[k].seal) same = false, first = k + 1;
    out.set("replays", same);
    if (!same) out.set("differs_at", static_cast<double>(first));
  }
  return out;
}

inline Json count_op(const Json& q) {
  const auto ps = parties_of(q["parties"]);
  std::vector<thesmos::procedures::Ballot> ballots;
  for (const auto& b : list(q["ballots"])) ballots.push_back({b["voter"].text(), b["bearer"].text(b["voter"].text()), b["yes"].is_bool() && b["yes"].as_bool()});
  const auto unit = thesmos::procedures::unit_named(q["unit"].text("origin"));
  if (!unit) throw std::invalid_argument("communities: no unit of count \"" + q["unit"].text() + "\"");
  const std::string rule = q["rule"].text("majority");
  if (!thesmos::procedures::known_rule(rule)) throw std::invalid_argument("communities: no rule \"" + rule + "\"");
  thesmos::kernel::Parties bare = ps;
  for (const auto& [id, p] : ps.all()) bare.at(id).controller.clear();
  const auto declared = thesmos::procedures::count(ps, ballots, *unit), undeclared = thesmos::procedures::count(bare, ballots, *unit);
  const bool a = thesmos::procedures::passes(declared, rule), b = thesmos::procedures::passes(undeclared, rule);
  return Json::object({{"share", declared.share().str()}, {"counted", static_cast<double>(declared.counted)}, {"passes", a},
                       {"undeclared", Json::object({{"share", undeclared.share().str()}, {"counted", static_cast<double>(undeclared.counted)}, {"passes", b}})}, {"turns", a != b}});
}

inline Json jar_op(const Json& q) {
  namespace jar = ::communities::jar;
  jar::Setting s;
  const std::string regime = q["regime"].text("provision with duty");
  bool found = false;
  for (auto r : {jar::Regime::currency, jar::Regime::currency_with_income, jar::Regime::currency_with_targeted_income, jar::Regime::provision_open, jar::Regime::provision_with_duty})
    if (jar::word(r) == regime) s.regime = r, found = true;
  if (!found) throw std::invalid_argument("communities: no arrangement \"" + regime + "\"");
  if (q["need"].is_number()) s.need = whole(q["need"]);
  if (q["parties"].is_number()) s.parties = static_cast<int>(whole(q["parties"]));
  if (q["rounds"].is_number()) s.rounds = static_cast<int>(whole(q["rounds"]));
  for (const auto& [key, field] : std::vector<std::pair<const char*, double*>>{{"free_riders", &s.free_riders}, {"epistemic", &s.epistemic}, {"effort", &s.effort}, {"stock", &s.stock},
                                                                                {"income", &s.income}, {"demurrage", &s.demurrage}, {"audit_cost", &s.audit_cost}})
    if (q[key].is_number()) *field = q[key].as_number();
  if (q["tiers"].is_array()) {
    s.tiers.clear();
    for (const auto& t : q["tiers"].as_array()) s.tiers.push_back(whole(t));
  }
  const int runs = static_cast<int>(whole(q["runs"], 20));
  if (runs < 1 || runs > 400) throw std::invalid_argument("communities: between 1 and 400 runs");
  const auto r = jar::mean(s, runs);
  Json by_round = Json::array(), weakest = Json::array();
  for (double x : r.met_by_round) by_round.push(x);
  for (double x : r.weakest_by_round) weakest.push(x);
  return Json::object({{"regime", regime}, {"runs", static_cast<double>(runs)}, {"met", r.met}, {"met_weakest", r.met_weakest}, {"met_strongest", r.met_strongest},
                       {"met_free_riders", r.met_free}, {"met_cooperators", r.met_cooperators}, {"offered", r.offered}, {"idle", r.idle}, {"audits", r.audits},
                       {"refused_for_balance", r.refused_for_balance}, {"suspended", r.suspended}, {"met_by_round", by_round}, {"weakest_by_round", weakest}});
}

// The constitution's rules as institutional statements, or the statements given ({texts}); with parties and acts, each duty's and
// prohibition's facets measured over the record the constitution runs.
inline Json statements_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  std::vector<thesmos::statements::Statement> given;
  Json refused = Json::array();
  if (q["texts"].is_array())
    for (const auto& t : q["texts"].as_array()) {
      const auto r = thesmos::statements::read(t.text());
      if (r.read) given.push_back(r.statement);
      else refused.push(Json::object({{"text", t.text()}, {"error", r.error}}));
    }
  else
    given = thesmos::statements::of(c);
  ch::State s(parties_of(q["parties"]), c);
  for (const auto& a : list(q["acts"])) ch::step(s, act_of(a));
  const auto holds = [&](const std::string& party, const std::string& position) { return s.holds(party, position); };
  Json out = Json::array();
  for (const auto& st : given) {
    Json one = Json::object({{"text", st.text()}, {"attribute", st.attribute}, {"deontic", st.deontic}, {"aim", st.aim}});
    if (!s.acts.empty() && (st.deontic == "must never" || (st.deontic == "must" && !st.trigger.empty()))) {
      const auto f = thesmos::norms::facets(thesmos::statements::norm(st, holds, s.acts), s.acts);
      one.set("episodes", static_cast<double>(f.episodes));
      one.set("regularity", f.regularity ? Json(f.regularity->str()) : Json());
    }
    out.push(one);
  }
  return Json::object({{"statements", out}, {"refused", refused}});
}

// The passport of a mandate as the home's record stands (the record folded on its constitution, acts refused leaving it as it was).
inline Json passport_issue_op(const Json& q) {
  std::optional<ch::State> kept;
  const ch::State& s = state_of(q, kept);
  const std::string home = q["home"].text(s.community), id = q["mandate"].text();
  if (id.empty()) throw std::invalid_argument("communities: a passport names the mandate it carries");
  const auto issued = cr::issue(home, s, id, q["at"].text(), {q["model"].text(), q["label"].text(), q["node"].text(), q["status_at"].text()});
  Json out = Json::object({{"admitted", issued.admitted}, {"reason", issued.reason}, {"head", named_head_json(cr::head(home, s))}});
  if (issued.admitted) out.set("passport", Json::parse(cr::write(issued.passport)));
  return out;
}

// A notary witnesses a head: its party `attester` attests it in the notary's record, under the notary's rules. The answer gives the
// witness as a passport lists it, the act to add to the notary's record, and the notary's head after it.
inline Json passport_witness_op(const Json& q) {
  ch::State s = copied(q);
  const std::string notary = q["notary"].text(s.community);
  const cr::Head witnessed = head_of(q["head"], {});
  if (witnessed.community.empty()) throw std::invalid_argument("communities: the head witnessed names its community");
  const long long turn = q["turn"].is_number() ? whole(q["turn"]) : (s.acts.empty() ? 0 : s.acts.back().turn);
  const auto w = cr::witness(s, notary, q["attester"].text(), turn, witnessed, q["when"].text(), q["mandate"].text());
  Json out = Json::object({{"admitted", w.admitted}, {"reason", w.reason}, {"head", named_head_json(cr::head(notary, s))}});
  if (w.admitted) {
    out.set("witness", Json::parse(cr::write(w.witness)));
    out.set("act", act_json(cr::attestation(witnessed, w.witness.attester, turn, w.witness.when, w.witness.mandate)));
  }
  return out;
}

// Whether a passport holds for a verifier who knows the given heads of homes (named, unless said otherwise, by the passport's issuer)
// and of notaries, shown at an instant, at a host, with a credential; each part left out is not checked.
inline Json passport_verify_op(const Json& q) {
  const auto r = cr::read_passport(q["passport"].is_string() ? q["passport"].text() : q["passport"].dump());
  if (!r.read) return Json::object({{"read", false}, {"faults", strings(r.faults)}, {"holds", false}, {"reason", r.faults.empty() ? std::string() : r.faults.front()}});
  cr::Trust trust;
  for (const auto& h : list(q["homes"])) trust.homes.push_back(head_of(h, r.passport.home));
  for (const auto& h : list(q["notaries"])) {
    if (h["community"].text().empty()) throw std::invalid_argument("communities: a notary's head names its community");
    trust.notaries.push_back(head_of(h, {}));
  }
  const auto v = cr::verify(r.passport, trust, {q["at"].text(), q["host"].text(), q["credential"].text()});
  const auto& m = r.passport.mandate();
  return Json::object({{"read", true}, {"holds", v.holds()}, {"claim", v.claim}, {"reason", v.reason}, {"home", r.passport.home}, {"holder", r.passport.holder.id},
                       {"principal", r.passport.principal.id}, {"mandate", m.id}, {"communities", strings_of(m.communities)}, {"links", static_cast<double>(r.passport.chain.size())},
                       {"witnesses", static_cast<double>(r.passport.witnesses.size())}});
}

// ---- Sessions (Phase 9; the design's Section 9.7) ------------------------------------------------------------------------------------
//
// A session folds a community's record once and keeps its state and its behaviour journal, a series of the community's own (the design's
// Section 4.1), in the engine's memory; a step puts new acts to the state as the full fold would (thesmos's step settles in place), and
// names the head it expects, so that a step on a head no longer current is refused as stale.

namespace fx = archeion::fixity;

struct Session {
  std::string id;
  thesmos::constitutions::Constitution founding;
  thesmos::kernel::Parties parties;
  ch::State s;
  ch::Conduct conduct;
  Session(std::string i, const thesmos::kernel::Parties& ps, const thesmos::constitutions::Constitution& c) : id(std::move(i)), founding(c), parties(ps), s(ps, c) {}
};

inline std::map<std::string, std::unique_ptr<Session>>& sessions() {
  static std::map<std::string, std::unique_ptr<Session>> kept;
  return kept;
}
inline long long& opened() {
  static long long n = 0;
  return n;
}
inline Session& session_of(const Json& q) {
  const std::string id = q["session"].text();
  const auto it = sessions().find(id);
  if (it == sessions().end()) throw std::invalid_argument("communities: no session \"" + id + "\": open it again from its record");
  return *it->second;
}
inline const ch::State& state_of(const Json& q, std::optional<ch::State>& kept) {
  if (q.has("session")) return session_of(q).s;
  kept.emplace(folded(q));
  return *kept;
}

inline Json entry_json(const fx::Entry& e) {
  return Json::object({{"when", e.when}, {"who", e.who}, {"what", e.what}, {"about", e.about}, {"note", e.note}, {"seal", e.seal}});
}
inline fx::Journal journal_of(const Json& j) {
  fx::Journal out;
  for (const auto& e : list(j)) out.entries.push_back({e["when"].text(), e["who"].text(), e["what"].text(), e["about"].text(), e["note"].text(), e["seal"].text()});
  return out;
}
inline Json journal_head(const ch::Conduct& c) {
  return Json::object({{"size", static_cast<double>(c.journal.entries.size())}, {"root", fx::hex(fx::merkle_root(cr::leaves(c.journal)))}, {"seal", c.journal.head()}});
}
inline Json inclusion_json(const fx::Inclusion& p) {
  Json path = Json::array();
  for (const auto& d : p.path) path.push(fx::hex(d));
  return Json::object({{"leaf", static_cast<double>(p.index)}, {"size", static_cast<double>(p.size)}, {"path", path}});
}
inline fx::Inclusion inclusion_of(const Json& j) {
  fx::Inclusion p;
  p.index = static_cast<std::size_t>(whole(j["leaf"])), p.size = static_cast<std::size_t>(whole(j["size"]));
  for (const auto& d : list(j["path"])) {
    if (!thesmos::holdings::is_digest(d.text())) throw std::invalid_argument("communities: a proof's path holds SHA-256 digests");
    p.path.push_back(fx::from_hex(d.text()));
  }
  return p;
}
inline Json verdict_json(const apodeixis::kernel::Verdict& v) {
  return Json::object({{"holds", v.holds()}, {"claim", v.claim}, {"reason", v.reason}, {"examined", static_cast<double>(v.examined)}});
}

// An episode of conduct as JSON {party, bearer, provider, model, community, node, operation, kind, turn, begun, ended, resources: {r: n},
// inputs, outputs, holdings, items, features: {k: v}}.
inline thesmos::behaviour::Episode episode_from(const Json& j) {
  thesmos::behaviour::Episode e;
  for (const auto& [k, f] : std::vector<std::pair<const char*, std::string*>>{{"party", &e.party}, {"bearer", &e.bearer}, {"provider", &e.provider}, {"model", &e.model},
                                                                               {"community", &e.community}, {"node", &e.node}, {"operation", &e.operation}, {"kind", &e.kind},
                                                                               {"begun", &e.begun}, {"ended", &e.ended}})
    *f = j[k].text();
  e.turn = whole(j["turn"]);
  if (j["resources"].is_object())
    for (const auto& [r, n] : j["resources"].as_object()) e.resources[r] = whole(n);
  for (const auto& [k, set] : std::vector<std::pair<const char*, std::set<std::string>*>>{{"inputs", &e.inputs}, {"outputs", &e.outputs}, {"holdings", &e.holdings}, {"items", &e.items}})
    for (const auto& x : list(j[k])) set->insert(x.text());
  if (j["features"].is_object())
    for (const auto& [k, v] : j["features"].as_object()) e.features[k] = scalar(v);
  if (e.party.empty()) throw std::invalid_argument("communities: an episode names its party");
  if (!ch::detail::clean(thesmos::behaviour::canonical(e))) throw std::invalid_argument("communities: an episode may not hold the journal's separators");
  return e;
}
inline Json episode_json(const thesmos::behaviour::Episode& e) {
  Json resources = Json::object(), features = Json::object();
  for (const auto& [r, n] : e.resources) resources.set(r, static_cast<double>(n));
  for (const auto& [k, v] : e.features) features.set(k, v);
  return Json::object({{"party", e.party}, {"bearer", e.bearer}, {"provider", e.provider}, {"model", e.model}, {"community", e.community}, {"node", e.node},
                       {"operation", e.operation}, {"kind", e.kind}, {"turn", static_cast<double>(e.turn)}, {"begun", e.begun}, {"ended", e.ended},
                       {"resources", resources}, {"inputs", strings_of(e.inputs)}, {"outputs", strings_of(e.outputs)}, {"holdings", strings_of(e.holdings)},
                       {"items", strings_of(e.items)}, {"features", features}});
}
inline ch::Epistemic epistemic_from(const Json& j) {
  ch::Epistemic e;
  e.party = j["party"].text(), e.kind = j["kind"].text(), e.scene = j["scene"].text(), e.matter = j["matter"].text(), e.turn = whole(j["turn"]);
  e.content = j["content"].text(), e.size = whole(j["size"]);
  return e;
}

inline void heads_into(Json& out, const Session& x) {
  out.set("head", named_head_json(cr::head(x.s.community, x.s)));
  out.set("digest", ch::relational_digest(x.s));
  out.set("size", static_cast<double>(x.s.journal.entries.size()));
  out.set("journal", journal_head(x.conduct));
}
inline void replays_into(Json& out, const Json& seals, const ch::State& s) {
  if (!seals.is_array()) return;
  bool same = seals.as_array().size() == s.journal.entries.size();
  std::size_t first = 0;
  for (std::size_t k = 0; k < std::min(seals.as_array().size(), s.journal.entries.size()); k++)
    if (seals.as_array()[k].text() != s.journal.entries[k].seal) {
      same = false, first = k + 1;
      break;
    }
  if (!same && first == 0) first = std::min(seals.as_array().size(), s.journal.entries.size()) + 1;
  out.set("replays", same);
  if (!same) out.set("differs_at", static_cast<double>(first));
}
inline void briefings_into(Json& out, const Json& brief, const ch::State& s) {
  if (!brief.is_array()) return;
  Json briefings = Json::object();
  for (const auto& b : brief.as_array()) {
    const std::string who = b.is_string() ? b.text() : b["as"].text();
    if (who.empty() || !s.parties.has(who)) continue;
    briefings.set(who, briefing_json(ch::brief(s, who, b.is_object() ? b["mandate"].text() : std::string(), b.is_object() ? b["purpose"].text() : std::string())));
  }
  out.set("briefings", briefings);
}

// communities.open {constitution, parties, acts, seals?, journal?, replaces?, state?, brief?}: the record folded once, every act parsed
// before any is applied; a behaviour journal given must seal. Answers the session, its head and journal's head, the acts refused, the
// seals of those admitted and, with the seals of an earlier run, whether the record replays as sealed.
inline Json open_op(const Json& q) {
  const auto c = constitution_of(q["constitution"]);
  const auto ps = parties_of(q["parties"]);
  std::vector<thesmos::acts::Act> acts;
  for (const auto& a : list(q["acts"])) acts.push_back(act_of(a));
  fx::Journal journal;
  if (q["journal"].is_array()) {
    journal = journal_of(q["journal"]);
    if (const auto v = fx::verify(journal); !v.holds()) throw std::invalid_argument("communities: the behaviour journal given does not seal: " + v.reason);
  }
  if (const std::string old = q["replaces"].text(); !old.empty()) sessions().erase(old);
  auto x = std::make_unique<Session>("session-" + std::to_string(++opened()), ps, c);
  x->conduct.journal = std::move(journal);
  Json refused = Json::array(), seals = Json::array();
  for (std::size_t k = 0; k < acts.size(); k++) {
    const auto o = ch::step(x->s, acts[k]);
    if (o.admitted) seals.push(x->s.journal.entries.back().seal);
    else refused.push(Json::object({{"n", static_cast<double>(k + 1)}, {"reason", o.reason}}));
  }
  Json out = Json::object({{"session", x->id}, {"community", x->s.community}, {"refused", refused}, {"seals", seals}});
  heads_into(out, *x);
  replays_into(out, q["seals"], x->s);
  if (q["state"].is_bool() && q["state"].as_bool()) out.set("state", state_json(x->s));
  briefings_into(out, q["brief"], x->s);
  const std::string id = x->id;
  sessions()[id] = std::move(x);
  return out;
}

// communities.step {session, head?, acts, state?, brief?}: refused as stale when the head named is not the session's. Each item is an act
// (with "conduct", the episode that constitutes it: recorded first, the act carrying its digest in its field "episode", its outcome
// recorded next), {"observe": episode, "at"} (conduct that constitutes no act), or {"disclose": epistemic, "at"} (by agreement). Every
// item is parsed before any is applied.
inline Json step_op(const Json& q) {
  Session& x = session_of(q);
  const auto now = cr::head(x.s.community, x.s);
  if (q["head"].is_object() && (whole(q["head"]["size"], -1) != static_cast<long long>(now.size) || q["head"]["root"].text() != fx::hex(now.root)))
    return Json::object({{"stale", true}, {"head", named_head_json(now)},
                         {"reason", "the session stands at a head of " + std::to_string(now.size) + " entries, not the one the step names: another step moved it first"}});
  struct Item {
    int form = 0;  // 0 an act, 1 an observation, 2 a disclosure
    thesmos::acts::Act act;
    bool constituted = false;
    thesmos::behaviour::Episode episode;
    ch::Epistemic epistemic;
    std::string at;
  };
  std::vector<Item> items;
  for (const auto& i : list(q["acts"])) {
    Item it;
    if (i.has("observe")) it.form = 1, it.episode = episode_from(i["observe"]), it.at = i["at"].text();
    else if (i.has("disclose")) it.form = 2, it.epistemic = epistemic_from(i["disclose"]), it.at = i["at"].text();
    else {
      it.act = act_of(i);
      if (i.has("conduct")) it.constituted = true, it.episode = episode_from(i["conduct"]);
    }
    items.push_back(std::move(it));
  }
  const std::size_t from = x.conduct.journal.entries.size();
  Json outcomes = Json::array(), seals = Json::array();
  for (std::size_t k = 0; k < items.size(); k++) {
    const Item& it = items[k];
    Json o = Json::object({{"n", static_cast<double>(k + 1)}});
    if (it.form == 1) {
      o.set("observed", true), o.set("episode", ch::observe(x.conduct, it.episode, it.at));
    } else if (it.form == 2) {
      const auto d = ch::disclose(x.s, x.conduct, it.epistemic, it.at);
      o.set("recorded", d.recorded), o.set("agreement", d.agreement), o.set("seal", d.seal), o.set("digest", d.digest), o.set("reason", d.reason);
    } else if (it.constituted) {
      const auto c = ch::constitute(x.s, x.conduct, it.episode, it.act);
      o.set("admitted", c.admitted), o.set("reason", c.reason), o.set("seal", c.admitted ? Json(c.seal) : Json()), o.set("episode", c.episode);
      if (c.admitted) seals.push(c.seal);
    } else {
      const auto r = ch::step(x.s, it.act);
      const std::string seal = r.admitted ? x.s.journal.entries.back().seal : std::string();
      o.set("admitted", r.admitted), o.set("reason", r.reason), o.set("seal", r.admitted ? Json(seal) : Json());
      if (r.admitted) seals.push(seal);
    }
    outcomes.push(o);
  }
  Json out = Json::object({{"outcomes", outcomes}, {"seals", seals}});
  heads_into(out, x);
  Json journal = out["journal"], entries = Json::array();
  for (std::size_t k = from; k < x.conduct.journal.entries.size(); k++) entries.push(entry_json(x.conduct.journal.entries[k]));
  journal.set("entries", entries);
  out.set("journal", journal);
  if (q["state"].is_bool() && q["state"].as_bool()) out.set("state", state_json(x.s));
  briefings_into(out, q["brief"], x.s);
  return out;
}

inline long long last_turn(const ch::State& s) { return s.acts.empty() ? 0 : s.acts.back().turn; }

// The items with who answers each; for a party, which are its own and which it may answer.
inline Json items_view(const ch::State& s, const std::string& who) {
  Json items = Json::array();
  for (const auto& [id, i] : s.items) {
    Json one = item_json(i, last_turn(s));
    const auto w = ch::answerer_of(s, i);
    one.set("answered by", Json::object({{"body", w.body}, {"office", w.office}}));
    if (!who.empty()) {
      one.set("yours", i.author == who || i.bearer == who);
      one.set("you may answer", ch::may_answer(s, i, who, ch::origin_of(s, who)));
    }
    items.push(one);
  }
  return Json::object({{"items", items}, {"as", who.empty() ? Json() : Json(who)}});
}

// The posted work, and for a party what its briefing says it may take, deliver, check, review and accept.
inline Json work_view(const ch::State& s, const std::string& who, const std::string& mandate) {
  Json out = Json::object({{"work", work_json(s, last_turn(s))}});
  if (!who.empty() && s.parties.has(who)) {
    Json lines = Json::array();
    for (const auto& w : ch::brief(s, who, mandate, {}, last_turn(s)).work) lines.push(Json::object({{"kind", w.kind}, {"id", w.id}, {"what", w.what}}));
    out.set("yours", lines);
  }
  return out;
}

// A mandate (the one named, or the first standing one the party holds) with its chain to the root: each link's allowance, what was
// used under it and what remains, its slice, term and status; a link of another community with its home, status as last reported and
// window.
inline Json mandate_view(const ch::State& s, const std::string& who, std::string id) {
  if (id.empty())
    for (const auto& [k, m] : s.mandates)
      if (m.holder == who && !s.revoked.count(k)) {
        id = k;
        break;
      }
  if (id.empty() || !s.mandates.count(id)) return Json::object({{"found", false}, {"as", who}, {"mandate", id}});
  Json chain = Json::array();
  std::string status = "standing";
  std::string at = id;
  for (std::size_t steps = 0; !at.empty() && s.mandates.count(at) && steps <= s.mandates.size(); steps++) {
    const auto& m = s.mandates.at(at);
    Json allowance = Json::object(), used = Json::object(), remaining = Json::object(), slice = Json::object();
    const auto u = s.used.find(at);
    for (const auto& [r, n] : m.allowance) {
      const long long spent = u != s.used.end() && u->second.count(r) ? u->second.at(r) : 0;
      allowance.set(r, static_cast<double>(n)), used.set(r, static_cast<double>(spent)), remaining.set(r, static_cast<double>(n - spent));
    }
    for (const auto& [r, n] : m.slice) slice.set(r, static_cast<double>(n));
    Json link = Json::object({{"id", at}, {"principal", m.principal}, {"holder", m.holder}, {"kinds", strings_of(m.kinds)}, {"matters", strings_of(m.matters)},
                              {"from", static_cast<double>(m.from)}, {"to", static_cast<double>(m.to)}, {"since", m.since}, {"until", m.until}, {"delegable", m.delegable},
                              {"communities", strings_of(m.communities)}, {"allowance", allowance}, {"used", used}, {"remaining", remaining}, {"slice", slice},
                              {"revoked", s.revoked.count(at) > 0}});
    if (s.revoked.count(at)) status = "revoked";
    if (const auto f = s.foreign.find(at); f != s.foreign.end()) {
      Json window = Json::object();
      if (const auto w = s.window.find(at); w != s.window.end())
        for (const auto& [r, n] : w->second) window.set(r, static_cast<double>(n));
      link.set("home", f->second.home), link.set("status", f->second.status), link.set("window", window);
      link.set("reported", f->second.reported < 0 ? Json() : Json(static_cast<double>(f->second.reported)));
      if (f->second.status != "standing" && status == "standing") status = f->second.status;
    }
    chain.push(link);
    at = m.parent;
  }
  return Json::object({{"found", true}, {"as", who}, {"mandate", id}, {"holder", s.mandates.at(id).holder}, {"status", status}, {"chain", chain}});
}

// The taxonomy of behaviour in force and the policy of records (who may read each part).
inline Json behaviour_view(const ch::State& s) {
  const auto& t = s.rules.behaviour;
  Json declared = Json::array(), conduct = Json::array(), counts_as = Json::array(), triggers = Json::array(), standards = Json::array(), features = Json::array(),
       practices = Json::array(), records = Json::object();
  for (const auto& d : t.declared) declared.push(Json::object({{"kind", d.kind}, {"counts as", d.counts_as}, {"force", std::string(thesmos::acts::word(d.force))}}));
  for (const auto& c : t.conduct) conduct.push(Json::object({{"name", c.name}, {"where", c.where.text}, {"analytic", c.analytic}}));
  for (const auto& c : t.counts_as)
    counts_as.push(Json::object({{"rule", c.rule}, {"conduct", c.conduct}, {"status", c.status}, {"value", c.value}, {"where declared", c.where_declared}}));
  for (const auto& r : t.triggers)
    triggers.push(Json::object({{"rule", r.rule}, {"then", r.then}, {"position", r.position}, {"held for", static_cast<double>(r.held_for)}, {"matter", r.matter},
                                {"past due by", static_cast<double>(r.past_due_by)}, {"by ground", r.by_ground}}));
  for (const auto& r : t.recognition) standards.push(Json::object({{"fact", r.fact}, {"grade", r.grade}, {"another origin", r.another_origin}}));
  for (const auto& f : t.features)
    features.push(Json::object({{"name", f.name}, {"space", f.space}, {"unit", f.unit}, {"level", f.level}, {"recorded by", f.recorded_by}, {"grade", f.grade}}));
  for (const auto& p : t.practices) practices.push(p.name);
  for (const auto& [part, v] : s.rules.visibility) records.set(part, hd::text(v));
  return Json::object({{"name", t.name}, {"version", static_cast<double>(t.version)}, {"extends", t.extends}, {"declared", declared}, {"conduct", conduct},
                       {"counts as", counts_as}, {"triggers", triggers}, {"standards", standards}, {"features", features}, {"practices", practices}, {"records", records}});
}

// A party's exit package (noetarium.exit/1; the design's Definition 50): the acts that concern it with their proofs under the chronicle's
// head (thesmos's crossings), the journal's entries that are its own or recorded the episodes (and their outcomes) that constituted
// those acts, each with the seal before it and its proof under the journal's head, its positions, commitments and mandates.
inline Json exit_view(const Session& x, const std::string& party) {
  if (party.empty()) throw std::invalid_argument("communities: an exit package names its party (as)");
  const ch::State& s = x.s;
  const auto portion = cr::extract(s.community, s, party);
  Json acts = Json::array();
  std::set<std::string> episodes;
  for (const auto& i : portion.items) {
    acts.push(Json::object({{"place", static_cast<double>(i.place + 1)}, {"seal", i.seal}, {"envelope", i.envelope}, {"proof", inclusion_json(i.proof)}}));
    if (s.acts[i.place].has("episode")) episodes.insert(s.acts[i.place].field("episode"));
  }
  const auto& e = x.conduct.journal.entries;
  const auto data = cr::leaves(x.conduct.journal);
  std::set<std::size_t> taken;
  for (std::size_t k = 0; k < e.size(); k++)
    if (e[k].who == party || (e[k].what == "episode" && episodes.count(e[k].seal))) {
      taken.insert(k);
      if (e[k].what == "episode" && k + 1 < e.size() && e[k + 1].what == "outcome") taken.insert(k + 1);
    }
  Json entries = Json::array();
  for (std::size_t k : taken)
    entries.push(Json::object({{"place", static_cast<double>(k + 1)}, {"entry", entry_json(e[k])}, {"previous", k == 0 ? fx::Journal::genesis() : e[k - 1].seal},
                               {"proof", inclusion_json(fx::prove(data, k))}}));
  Json positions = Json::array(), commitments = Json::array(), mandates = Json::array();
  for (const auto& [k, since] : s.positions)
    if (k.first == party) positions.push(Json::object({{"position", k.second}, {"since", static_cast<double>(since)}}));
  for (const auto& [id, c] : s.commitments)
    if (c.debtor == party || c.creditor == party) {
      const std::string w(thesmos::commitments::word(c.status));
      commitments.push(Json::object({{"id", id}, {"debtor", c.debtor}, {"creditor", c.creditor}, {"matter", c.matter}, {"amount", static_cast<double>(c.amount)}, {"status", w},
                                     {"open", w == "conditional" || w == "active" || w == "presented" || w == "handed over" || w == "handed_over"}}));
    }
  for (const auto& [id, m] : s.mandates)
    if (m.holder == party || m.principal == party)
      mandates.push(Json::object({{"id", id}, {"principal", m.principal}, {"holder", m.holder}, {"revoked", s.revoked.count(id) > 0}}));
  Json chronicle = Json::object({{"head", named_head_json(portion.at)}, {"acts", acts}});
  return Json::object({{"format", "noetarium.exit/1"}, {"community", s.community}, {"party", party}, {"chronicle", chronicle},
                       {"journal", Json::object({{"head", journal_head(x.conduct)}, {"entries", entries}})}, {"positions", positions}, {"commitments", commitments},
                       {"mandates", mandates}});
}

// communities.exit.verify {package, chronicle?: {size, root}, journal?: {size, root}}: whether every act of the package is under the
// chronicle's head known, and every entry seals from the seal before it and is under the journal's head known; each part only against a
// head given.
inline Json exit_verify_op(const Json& q) {
  const Json& p = q["package"];
  if (p["format"].text() != "noetarium.exit/1") throw std::invalid_argument("communities: the package is not noetarium.exit/1");
  const std::string community = p["community"].text();
  Json out = Json::object({{"party", p["party"].text()}, {"community", community}});
  bool holds = true, checked = false;
  if (q["chronicle"].is_object()) {
    cr::Portion portion;
    portion.party = p["party"].text(), portion.at = head_of(p["chronicle"]["head"], community);
    for (const auto& i : list(p["chronicle"]["acts"]))
      portion.items.push_back({static_cast<std::size_t>(whole(i["place"]) - 1), i["seal"].text(), i["envelope"].text(), inclusion_of(i["proof"])});
    const auto v = cr::verify(portion, head_of(q["chronicle"], community));
    out.set("chronicle", verdict_json(v));
    holds = holds && v.holds(), checked = true;
  }
  if (q["journal"].is_object()) {
    const std::size_t size = static_cast<std::size_t>(whole(q["journal"]["size"]));
    const std::string root = q["journal"]["root"].text();
    std::string why;
    if (!thesmos::holdings::is_digest(root)) throw std::invalid_argument("communities: a journal's head names its root, a SHA-256 digest");
    if (whole(p["journal"]["head"]["size"]) != static_cast<long long>(size) || p["journal"]["head"]["root"].text() != root) why = "the package names another head of the journal than the one known";
    for (const auto& i : list(p["journal"]["entries"])) {
      if (!why.empty()) break;
      const Json& j = i["entry"];
      const fx::Entry e{j["when"].text(), j["who"].text(), j["what"].text(), j["about"].text(), j["note"].text(), j["seal"].text()};
      const auto proof = inclusion_of(i["proof"]);
      const std::string place = std::to_string(whole(i["place"]));
      if (fx::detail::seal_of(i["previous"].text(), e) != e.seal) why = "the entry at place " + place + " does not seal from the seal before it";
      else if (proof.size != size || !fx::included(cr::leaf(e), proof, fx::from_hex(root))) why = "the entry at place " + place + " is not under the journal's head";
    }
    out.set("journal", Json::object({{"holds", why.empty()}, {"reason", why.empty() ? std::string("every entry seals and is under the journal's head") : why}}));
    holds = holds && why.empty(), checked = true;
  }
  out.set("holds", holds && checked);
  return out;
}

// What a reader needs to write the two records as W3C PROV (the API's Python writes the document): the agents, the delegations of
// mandates, the journal's episodes read back with their outcomes, the admitted acts with the episode that constituted each, the holdings.
inline Json provenance_view(const Session& x) {
  const ch::State& s = x.s;
  Json agents = Json::array(), delegations = Json::array(), episodes = Json::array(), acts = Json::array(), holdings = Json::array();
  for (const auto& [id, p] : s.parties.all())
    agents.push(Json::object({{"id", id}, {"kind", std::string(thesmos::kernel::word(p.kind))}, {"controller", p.controller}, {"provider", p.provider}}));
  for (const auto& [id, m] : s.mandates) delegations.push(Json::object({{"mandate", id}, {"holder", m.holder}, {"principal", m.principal}, {"parent", m.parent}}));
  const auto& e = x.conduct.journal.entries;
  for (std::size_t k = 0; k < e.size(); k++) {
    if (e[k].what != "episode") continue;
    std::string outcome;
    const std::string prefix = "episode:" + e[k].seal + "\x1e";
    if (k + 1 < e.size() && e[k + 1].what == "outcome" && e[k + 1].about.rfind(prefix, 0) == 0) outcome = e[k + 1].about.substr(prefix.size());
    Json one = Json::object({{"entry", e[k].seal}, {"place", static_cast<double>(k + 1)}, {"party", e[k].who}, {"outcome", outcome}});
    if (const auto ep = ::communities::trajectories::episode_of(e[k].about)) {
      Json features = Json::object();
      for (const auto& [f, v] : ep->features) features.set(f, v);
      one.set("operation", ep->operation), one.set("kind", ep->kind), one.set("begun", ep->begun), one.set("ended", ep->ended);
      one.set("inputs", strings_of(ep->inputs)), one.set("outputs", strings_of(ep->outputs)), one.set("features", features);
    } else
      one.set("unread", true);
    episodes.push(one);
  }
  for (std::size_t k = 0; k < s.acts.size(); k++) {
    const auto& a = s.acts[k];
    acts.push(Json::object({{"place", static_cast<double>(k + 1)}, {"kind", a.kind}, {"reading", s.readings[k].kind}, {"seal", s.journal.entries[k].seal},
                            {"turn", static_cast<double>(a.turn)}, {"author", a.author}, {"mandate", a.mandate}, {"episode", a.field("episode")},
                            {"holding", a.field("holding")}, {"digest", a.field("digest")}}));
  }
  for (const auto& [id, h] : s.holdings)
    holdings.push(Json::object({{"id", id}, {"kind", h.kind}, {"digest", h.provenance.digest}, {"within", h.within}, {"depositor", h.depositor}}));
  return Json::object({{"community", s.community}, {"agents", agents}, {"delegations", delegations}, {"episodes", episodes}, {"acts", acts}, {"holdings", holdings}});
}

// communities.view {session, what, as?, mandate?, purpose?, turn?, from?}
inline Json view_op(const Json& q) {
  Session& x = session_of(q);
  const ch::State& s = x.s;
  const std::string what = q["what"].text("state"), who = q["as"].text();
  if (what == "state") return state_json(s);
  if (what == "profile") return profile_json(s, who);
  if (what == "briefing") {
    if (who.empty()) throw std::invalid_argument("communities: a briefing names the party it is for (as)");
    return briefing_json(ch::brief(s, who, q["mandate"].text(), q["purpose"].text(), turn_of(s, q)));
  }
  if (what == "items") return items_view(s, who);
  if (what == "work") return work_view(s, who, q["mandate"].text());
  if (what == "mandate") return mandate_view(s, who, q["mandate"].text());
  if (what == "behaviour") return behaviour_view(s);
  if (what == "exit") return exit_view(x, who);
  if (what == "provenance") return provenance_view(x);
  if (what == "holdings") return holdings_part(s, turn_of(s, q));
  if (what == "head") {  // the two heads, with a proof of consistency of the chronicle from an earlier size when asked
    Json out = Json::object({{"head", named_head_json(cr::head(s.community, s))}, {"journal", journal_head(x.conduct)}});
    if (q["from"].is_number()) {
      const auto from = static_cast<std::size_t>(whole(q["from"]));
      if (from > s.journal.entries.size()) throw std::invalid_argument("communities: a proof of consistency from beyond the head");
      Json proof = Json::array();
      for (const auto& d : cr::consistency(s, from)) proof.push(fx::hex(d));
      out.set("from", static_cast<double>(from)), out.set("consistency", proof);
    }
    return out;
  }
  if (what == "journal") {
    Json entries = Json::array();
    const auto from = static_cast<std::size_t>(std::max(0LL, whole(q["from"])));
    for (std::size_t k = from; k < x.conduct.journal.entries.size(); k++) entries.push(entry_json(x.conduct.journal.entries[k]));
    return Json::object({{"from", static_cast<double>(from)}, {"entries", entries}, {"head", journal_head(x.conduct)}});
  }
  throw std::invalid_argument("communities: no view \"" + what + "\"");
}

inline Json close_op(const Json& q) { return Json::object({{"closed", sessions().erase(q["session"].text()) > 0}}); }

// communities.join.verify {session, journal?} or {constitution, parties, acts, journal}: whether the chronicle and the behaviour journal
// are joined act to episode in both directions (thesmos's Proposition 15), failing where the join breaks; and whether epistemic behaviour
// is kept only under the agreements that cover it.
inline Json join_op(const Json& q) {
  std::optional<ch::State> kept;
  const ch::State& s = state_of(q, kept);
  ch::Conduct c;
  if (q["journal"].is_array()) c.journal = journal_of(q["journal"]);
  else if (q.has("session")) c = session_of(q).conduct;
  const auto v = ch::joined(s, c), d = ch::disclosed_by_agreement(s, c);
  return Json::object({{"holds", v.holds() && d.holds()}, {"joined", verdict_json(v)}, {"disclosed", verdict_json(d)}, {"head", named_head_json(cr::head(s.community, s))},
                       {"journal", journal_head(c)}});
}

// ---- Registries (the communities domain's registries) --------------------------------------------------------------------------------

namespace rg = ::communities::registries;

inline std::string text_of(const Json& j) { return j.is_string() ? j.text() : j.dump(); }
inline Json listing_json(const rg::Listing& l) { return Json::parse(rg::write(l)); }
inline thesmos::passports::Witness witness_of(const Json& j) {
  thesmos::passports::Witness w;
  w.notary = j["notary"].text(), w.attester = j["attester"].text(), w.mandate = j["mandate"].text(), w.turn = whole(j["turn"]), w.when = j["when"].text();
  w.seal = j["seal"].text(), w.proof = inclusion_of(j["proof"]), w.at = head_of(j["head"], w.notary);
  return w;
}
inline rg::Evidence evidence_of(const Json& j) {
  rg::Evidence e;
  e.profile = text_of(j["profile"]), e.registry = j["registry"].text();
  if (j["now"].is_object()) e.now = head_of(j["now"], {});
  for (const auto& d : list(j["consistency"])) e.consistency.push_back(fx::from_hex(d.text()));
  for (const auto& w : list(j["witnesses"])) e.witnesses.push_back(witness_of(w));
  for (const auto& h : list(j["notaries"])) e.notaries.push_back(head_of(h, {}));
  return e;
}
inline Json check_json(const rg::Check& c) {
  return Json::object({{"status", c.status}, {"witnessed", c.witnessed}, {"profile current", c.profile_current}, {"consistent", c.consistent}, {"claimed", c.claimed},
                       {"reasons", strings(c.reasons)}});
}
inline Json entry_json(const rg::Entry& e) {
  return Json::object({{"holding", e.holding}, {"digest", e.digest}, {"depositor", e.depositor}, {"deposited by", e.deposited_by}, {"status", e.status},
                       {"checked by", e.checked_by}, {"since", static_cast<double>(e.since)}, {"delisted by", e.delisted_by},
                       {"delisted", e.delisted < 0 ? Json() : Json(static_cast<double>(e.delisted))}, {"norm", e.norm}, {"reason", e.reason}});
}

// communities.registry {do, ...}: "profile" {session | record, community?}, the document noetarium.profile/1 exactly as a node serves it
// and its SHA-256; "listing" {session | record, node, witnessed by, listed, languages?}; "read" {listing}; "verify" {listing, evidence:
// {profile, now?, consistency?, witnesses?, notaries?, registry?}}; "entries" and "faults" {session | record, registry}; "search"
// {registries: [{keeper, holding, listings: [{listing, evidence}]}], query: {order?, words?, position?, current only?}}.
inline Json registry_op(const Json& q) {
  const std::string what = q["do"].text();
  if (what == "profile" || what == "listing" || what == "entries" || what == "faults") {
    std::optional<ch::State> kept;
    const ch::State& s = state_of(q, kept);
    const std::string community = q["community"].text(s.community);
    if (what == "profile") {
      const std::string doc = rg::profile_document(community, s);
      return Json::object({{"document", doc}, {"sha256", fx::sha256_hex(doc)}});
    }
    if (what == "listing") {
      std::vector<std::string> languages;
      for (const auto& l : list(q["languages"])) languages.push_back(l.text());
      if (languages.empty()) languages = {"en"};
      const auto l = rg::listing_of(community, s, q["node"].text(), q["witnessed by"].text(), q["listed"].text(), languages);
      return Json::object({{"listing", listing_json(l)}, {"written", rg::write(l)}, {"digest", rg::digest(l)}});
    }
    const auto es = rg::entries(s, q["registry"].text());
    Json entries = Json::array();
    for (const auto& e : es) entries.push(entry_json(e));
    if (what == "faults") return Json::object({{"faults", strings(rg::faults(es))}});
    return Json::object({{"entries", entries}, {"faults", strings(rg::faults(es))}});
  }
  if (what == "read") {
    const auto r = rg::read(text_of(q["listing"]));
    return Json::object({{"read", r.read}, {"faults", strings(r.faults)}, {"listing", r.read ? listing_json(r.listing) : Json()}});
  }
  if (what == "verify") {
    const auto r = rg::read(text_of(q["listing"]));
    if (!r.read) return Json::object({{"read", false}, {"faults", strings(r.faults)}, {"status", "faulty"}});
    Json out = check_json(rg::verify(r.listing, evidence_of(q["evidence"])));
    out.set("read", true);
    return out;
  }
  if (what == "search") {
    std::vector<rg::Registry> registries;
    for (const auto& g : list(q["registries"])) {
      rg::Registry r{g["keeper"].text(), g["holding"].text(), {}};
      for (const auto& l : list(g["listings"])) {
        const auto read = rg::read(text_of(l["listing"]));
        if (read.read) r.listings.push_back({read.listing, rg::verify(read.listing, evidence_of(l["evidence"]))});
      }
      registries.push_back(std::move(r));
    }
    rg::Query query;
    const Json& qq = q["query"];
    query.order = qq["order"].text(), query.position = qq["position"].text();
    for (const auto& w : list(qq["words"])) query.words.push_back(w.text());
    if (qq["current only"].is_bool()) query.current_only = qq["current only"].as_bool();
    Json found = Json::array();
    for (const auto& f : rg::search(registries, query))
      found.push(Json::object({{"community", f.community}, {"status", f.status}, {"registries", strings(f.registries)}, {"newest", listing_json(f.newest)},
                               {"disagreements", strings(f.disagreements)}}));
    return Json::object({{"found", found}});
  }
  throw std::invalid_argument("communities: no registry operation \"" + what + "\"");
}

inline Json run(const Json& q) {
  const std::string op = q["op"].text();
  if (op == "status") return Json::object({{"ok", true}});
  if (op == "communities.open") return open_op(q);
  if (op == "communities.step") return step_op(q);
  if (op == "communities.view") return view_op(q);
  if (op == "communities.close") return close_op(q);
  if (op == "communities.join.verify") return join_op(q);
  if (op == "communities.exit.verify") return exit_verify_op(q);
  if (op == "communities.registry") return registry_op(q);
  if (op == "communities.statements") return statements_op(q);
  if (op == "communities.admit") return admit_op(q);
  if (op == "communities.run") return run_op(q);
  if (op == "communities.count") return count_op(q);
  if (op == "communities.jar") return jar_op(q);
  if (op == "communities.holdings") return holdings_op(q);
  if (op == "communities.access") return access_op(q);
  if (op == "communities.briefing") return briefing_op(q);
  if (op == "communities.profile") return profile_op(q);
  if (op == "communities.passport.issue") return passport_issue_op(q);
  if (op == "communities.passport.witness") return passport_witness_op(q);
  if (op == "communities.passport.verify") return passport_verify_op(q);
  throw std::invalid_argument("communities: no operation \"" + op + "\"");
}

}  // namespace science::communities
