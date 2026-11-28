// Registries: holdings of kind registry kept by communities, whose entries are listings (the design *Communities That Agents Of Any
// Provider Find, Join, Work In And Leave*, Definition 55 and Sections 2.2, 5.17 and 7.3).
//
// There is no directory above all communities. A registry is a holding a community keeps (a league, an organisation, a commons of
// listings), and its listings are holdings deposited within it under the keeper's norms of access: who may list, how a listing is
// checked (by another origin than its depositor's, where the registry declares review), and how it is withdrawn (by its depositor, or by
// the custodian under a declared norm, with reasons, open to a claim). Several registries may list one community; being listed gives
// standing only with the keeper; a reader chooses which registries to consult and checks each listing against the community itself.
//
// A listing (noetarium.listing/1) states the community's name, order, purpose, its node, the digest of its public profile, its head with
// the notary that witnessed it, its open positions and work, its languages and when it was listed. The profile it names is the document
// noetarium.profile/1 written here from thesmos's public profile (decision 0006, with the order of decision 0014): what changes only by an
// amendment (its order, purpose, positions and how each is entered, its routes, from whom it accepts passports, its route for claims,
// the registries it says it is listed in, its public holdings and the parts it shows), never the head or the work of the moment, so that
// a listing goes stale exactly when the community changes what it publishes. A reader verifies a listing (verify): the profile fetched
// from the community's own node must have the digest the listing names, else the listing is stale; a notary the reader trusts must have
// witnessed the listing's head, else it is unverified; and where the reader holds the community's present head with a proof of
// consistency, the listing's head must be its beginning, else the community's record was rewritten and the listing is faulty. A search
// across several registries (search) gathers each community once, the newest listing first, and reports where registries disagree.

#pragma once

#include <thesmos/chronicle/profile.hpp>
#include <thesmos/crossings/crossings.hpp>

#include <archeion/fixity/sha256.hpp>

#include <symploke/jsonld/json.hpp>

#include <algorithm>
#include <cctype>
#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace communities::registries {

using thesmos::chronicle::State;
using thesmos::passports::Head;
using thesmos::passports::Witness;
using Json = symploke::jsonld::Json;

namespace detail {
inline Json texts(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& x : v) a.items.push_back(Json::string(x));
  return a;
}
inline Json whole(long long n) { return Json::num(static_cast<double>(n), std::to_string(n)); }
inline std::string text(const Json* j) { return j && j->is_string() ? j->text : std::string(); }
inline std::vector<std::string> strings(const Json* j) {
  std::vector<std::string> v;
  if (j && j->is_array())
    for (const auto& x : j->items)
      if (x.is_string()) v.push_back(x.text);
  return v;
}
inline std::string lower(std::string s) {
  for (char& c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  return s;
}
}  // namespace detail

// ---- The profile a listing names ----------------------------------------------------------------------------------------------------

// The public profile of a community as the document noetarium.profile/1, in symploke's canonical form (members ordered, no spaces), so
// that its digest is a function of what it says. Read with no reader: what thesmos's profile shows the public.
inline std::string profile_document(const std::string& community, const State& s) {
  const auto p = thesmos::chronicle::profile(s);
  const auto& c = s.rules;
  Json doc = Json::object();
  doc.set("format", Json::string("noetarium.profile/1"));
  doc.set("community", Json::string(community));
  doc.set("order", Json::string(p.order));
  doc.set("purpose", Json::string(p.purpose));
  Json boundary = Json::object();  // always public (thesmos's always_public): each position and how it is entered
  for (const auto& [name, b] : c.boundary) boundary.set(name, Json::string(b.entry.mode + (b.term > 0 ? ", for " + std::to_string(b.term) + " turns" : std::string())));
  doc.set("boundary", boundary);
  doc.set("claims", Json::string(c.claims.route));
  if (c.passports.declared) doc.set("passports", Json::string(c.passports.accepted_from));
  if (c.routes.declared) {
    std::vector<std::string> kinds;
    for (const auto& [kind, q] : c.routes.petitions) kinds.push_back(kind + " by " + q.by);
    doc.set("routes", Json::object());
    doc.get("routes")->set("questions", Json::string(c.routes.questions.empty ? std::string("none") : c.routes.questions.body));
    doc.get("routes")->set("petitions", detail::texts(kinds));
  }
  doc.set("listed in", detail::texts(p.listed_in));
  std::vector<std::string> held;
  for (const auto& h : p.holdings) held.push_back(h.id + " (" + h.kind + ")");
  doc.set("holdings", detail::texts(held));
  doc.set("shown", detail::texts(std::vector<std::string>(p.shown.begin(), p.shown.end())));
  return symploke::jsonld::canonical(doc);
}

// ---- Listings -----------------------------------------------------------------------------------------------------------------------

struct Listing {
  std::string format = "noetarium.listing/1";
  std::string community, order, purpose, node, profile_url, profile_sha256, witnessed_by, listed;
  Head head;                                     // the community's head when listed: its name, number of entries and root
  std::vector<std::string> open_positions, languages;
  long long open_work = 0;
};

// The listing of a community as its own record gives it: its order and purpose, the digest of its profile document, its head, the
// positions an outsider may enter by its own act or by application, and the posted work open.
inline Listing listing_of(const std::string& community, const State& s, const std::string& node, const std::string& witnessed_by, const std::string& listed,
                          std::vector<std::string> languages = {"en"}) {
  Listing l;
  l.community = community, l.order = s.rules.order, l.purpose = s.rules.purpose, l.node = node, l.witnessed_by = witnessed_by, l.listed = listed;
  l.profile_url = node + "c/" + community + "/profile";
  l.profile_sha256 = archeion::fixity::sha256_hex(profile_document(community, s));
  l.head = thesmos::crossings::head(community, s);
  for (const auto& [name, b] : s.rules.boundary)
    if (b.entry.mode == "open" || b.entry.mode == "application") l.open_positions.push_back(name);
  const long long now = s.acts.empty() ? 0 : s.acts.back().turn;
  for (const auto& [id, i] : s.work_items) l.open_work += thesmos::chronicle::work_state(s, i, now) == "open";
  l.languages = std::move(languages);
  return l;
}

// The written form, canonical, and its digest: the digest a registry's holding of the listing names.
inline std::string write(const Listing& l) {
  Json doc = Json::object();
  doc.set("format", Json::string(l.format));
  doc.set("community", Json::string(l.community));
  doc.set("order", Json::string(l.order));
  doc.set("purpose", Json::string(l.purpose));
  doc.set("node", Json::string(l.node));
  Json profile = Json::object();
  profile.set("url", Json::string(l.profile_url));
  profile.set("sha256", Json::string(l.profile_sha256));
  doc.set("profile", profile);
  Json head = Json::object();
  head.set("size", detail::whole(static_cast<long long>(l.head.size)));
  head.set("root", Json::string(archeion::fixity::hex(l.head.root)));
  head.set("witnessed by", Json::string(l.witnessed_by));
  doc.set("head", head);
  Json open = Json::object();
  open.set("positions", detail::texts(l.open_positions));
  open.set("work", detail::whole(l.open_work));
  doc.set("open", open);
  doc.set("languages", detail::texts(l.languages));
  doc.set("listed", Json::string(l.listed));
  return symploke::jsonld::canonical(doc);
}
inline std::string digest(const Listing& l) { return archeion::fixity::sha256_hex(write(l)); }

struct Reading {
  bool read = false;
  Listing listing;
  std::vector<std::string> faults;
};

// Reads a listing, naming every part of unexpected shape; a listing with a fault is not read.
inline Reading read(std::string_view written) {
  Reading r;
  Json doc;
  try {
    doc = symploke::jsonld::parse(written);
  } catch (const std::exception& e) {
    r.faults.push_back(std::string("the listing is not JSON: ") + e.what());
    return r;
  }
  if (!doc.is_object()) {
    r.faults.push_back("a listing is a JSON object");
    return r;
  }
  using detail::text;
  Listing& l = r.listing;
  l.format = text(doc.get("format"));
  if (l.format != "noetarium.listing/1") r.faults.push_back("the format is not noetarium.listing/1");
  l.community = text(doc.get("community")), l.order = text(doc.get("order")), l.purpose = text(doc.get("purpose")), l.node = text(doc.get("node"));
  l.listed = text(doc.get("listed"));
  if (l.community.empty()) r.faults.push_back("the listing names no community");
  if (const Json* p = doc.get("profile"); p && p->is_object()) l.profile_url = text(p->get("url")), l.profile_sha256 = text(p->get("sha256"));
  if (!thesmos::holdings::is_digest(l.profile_sha256)) r.faults.push_back("the profile is named by no SHA-256 digest");
  const Json* h = doc.get("head");
  const Json* size = h && h->is_object() ? h->get("size") : nullptr;
  const std::string root = h && h->is_object() ? text(h->get("root")) : std::string();
  if (!size || !size->is_number() || size->number < 0 || !thesmos::holdings::is_digest(root)) r.faults.push_back("the head names no number of entries and root");
  else {
    l.head.community = l.community, l.head.size = static_cast<std::size_t>(size->number), l.head.root = archeion::fixity::from_hex(root);
    l.witnessed_by = text(h->get("witnessed by"));
  }
  if (const Json* o = doc.get("open"); o && o->is_object()) {
    l.open_positions = detail::strings(o->get("positions"));
    if (const Json* w = o->get("work"); w && w->is_number()) l.open_work = static_cast<long long>(w->number);
  }
  l.languages = detail::strings(doc.get("languages"));
  r.read = r.faults.empty();
  return r;
}

// ---- Verification against the community itself --------------------------------------------------------------------------------

// What a reader holds to check a listing: the profile document fetched from the community's own node; the community's present head and
// a proof of consistency from the listing's head to it (left empty when the reader holds none); the witnesses of the listing's head; the
// heads of the notaries the reader trusts; and the registry the listing was read in.
struct Evidence {
  std::string profile;
  Head now;
  std::vector<archeion::fixity::Digest> consistency;
  std::vector<Witness> witnesses;
  std::vector<Head> notaries;
  std::string registry;
};

struct Check {
  std::string status;                // current, stale, unverified or faulty, the first that applies from the last
  bool witnessed = false;            // a notary the reader trusts witnessed the listing's head
  bool profile_current = false;      // the profile fetched has the digest the listing names
  bool consistent = false;           // the listing's head is the beginning of the present head (false when no present head is held)
  bool claimed = false;              // the community's profile says it is listed in this registry
  std::vector<std::string> reasons;  // why it is not current, and whether it is claimed
};

inline Check verify(const Listing& l, const Evidence& e) {
  Check c;
  bool faulty = false;
  Json profile;
  try {
    profile = symploke::jsonld::parse(e.profile);
  } catch (const std::exception&) {
    profile = Json::object();
  }
  const std::string named = profile.is_object() ? detail::text(profile.get("community")) : std::string();
  if (named != l.community) {
    faulty = true;
    c.reasons.push_back("the profile fetched is " + (named.empty() ? std::string("no community's") : named + "'s") + ", not " + l.community + "'s");
  }
  if (l.head.community != l.community) faulty = true, c.reasons.push_back("the listing's head is of another community");
  if (e.now.size > 0) {
    if (e.now.community != l.community) faulty = true, c.reasons.push_back("the present head is of another community");
    else if (e.now.size < l.head.size) faulty = true, c.reasons.push_back("the present head is shorter than the head listed");
    else if (thesmos::crossings::consistent(l.head, e.now, e.consistency).holds()) c.consistent = true;
    else faulty = true, c.reasons.push_back("the listing's head is not the beginning of the community's present head: its record was rewritten since it was listed");
  }
  for (const Witness& w : e.witnesses) {
    if ((!l.witnessed_by.empty() && w.notary != l.witnessed_by) || !thesmos::passports::attests(w, l.head)) continue;
    for (const Head& n : e.notaries) c.witnessed = c.witnessed || thesmos::passports::same(n, w.at);
  }
  if (!c.witnessed) c.reasons.push_back("no notary the reader trusts witnessed the listing's head");
  c.profile_current = archeion::fixity::sha256_hex(e.profile) == l.profile_sha256;
  if (!c.profile_current) c.reasons.push_back("the community's profile is no longer the one listed: it changed what it publishes");
  for (const auto& r : detail::strings(profile.is_object() ? profile.get("listed in") : nullptr)) c.claimed = c.claimed || r == e.registry;
  if (!c.claimed && !e.registry.empty()) c.reasons.push_back("the community does not say it is listed in " + e.registry);
  c.status = faulty ? "faulty" : !c.witnessed ? "unverified" : !c.profile_current ? "stale" : "current";
  return c;
}

// ---- A registry in its keeper's chronicle ----------------------------------------------------------------------------------------

// One entry of a registry as the keeper's record holds it: the holding deposited within the registry, its digest, who answers for its
// deposit and who made it, and whether it is pending its check, listed, or delisted; who checked it; and for a delisting who made it,
// when, under which norm, and the reason its act states (its field "reason").
struct Entry {
  std::string holding, digest, depositor, deposited_by, status, checked_by, delisted_by, norm, reason;
  long long since = 0, delisted = -1;
};

inline std::vector<Entry> entries(const State& keeper, const std::string& registry) {
  std::vector<Entry> out;
  for (const auto& [id, h] : keeper.holdings) {
    if (h.within != registry) continue;
    Entry e;
    e.holding = id, e.digest = h.provenance.digest, e.depositor = h.depositor, e.deposited_by = h.deposited_by, e.since = h.since, e.checked_by = h.checked_by;
    e.status = h.withdrawn ? "delisted" : h.pending ? "pending" : "listed";
    if (h.withdrawn) {
      for (std::size_t k = 0; k < keeper.readings.size(); k++) {
        const auto& a = keeper.readings[k];
        if (a.kind == "withdraw" && a.field("holding") == id) e.delisted_by = a.author, e.delisted = a.turn, e.reason = keeper.acts[k].field("reason");
      }
      for (const auto& x : keeper.accesses)
        if (x.holding == id && x.aim == "withdraw") e.norm = x.norm;
    }
    out.push_back(std::move(e));
  }
  return out;
}

// What is wrong with a registry's record: a listing delisted by another than its depositor without a reason (the design's Definition
// 55 asks the custodian's withdrawal to give reasons, open to a claim).
inline std::vector<std::string> faults(const std::vector<Entry>& es) {
  std::vector<std::string> out;
  for (const auto& e : es)
    if (e.status == "delisted" && e.delisted_by != e.deposited_by && e.delisted_by != e.depositor && e.reason.empty())
      out.push_back("the listing " + e.holding + " was delisted by " + e.delisted_by + ", not its depositor, with no reason given");
  return out;
}

// Whether a listing's text is the one an entry holds: its digest is the holding's.
inline bool holds(const Entry& e, std::string_view written) { return archeion::fixity::sha256_hex(written) == e.digest; }

// ---- Search across registries ----------------------------------------------------------------------------------------------------

// A registry as a reader has read and checked it: its keeper, the holding, and each listing (read from the entries that are listed) with
// the reader's check of it.
struct Registry {
  std::string keeper, holding;
  std::vector<std::pair<Listing, Check>> listings;
};

struct Query {
  std::string order;               // an order to match, or any
  std::vector<std::string> words;  // words every one of which the purpose holds, ignoring case
  std::string position;            // a position open to an outsider, or any
  bool current_only = true;        // listings that are current alone
};

struct Found {
  std::string community, status;                    // the best status any registry's listing of it has
  std::vector<std::string> registries;              // the registries that list it (keeper/holding), in the order read
  Listing newest;                                   // the listing of the longest head among those not faulty
  std::vector<std::string> disagreements;           // where the registries' listings of it disagree
};

inline int rank(const std::string& status) { return status == "current" ? 3 : status == "stale" ? 2 : status == "unverified" ? 1 : 0; }

// Each community once, those whose newest listing matches the query, in the order of their names; what the registries disagree on is
// reported, never resolved: two heads of one length with different roots mean one listing is not the community's record.
inline std::vector<Found> search(const std::vector<Registry>& registries, const Query& q) {
  std::map<std::string, Found> by;
  std::map<std::string, std::vector<const Listing*>> seen;
  for (const auto& r : registries)
    for (const auto& [l, c] : r.listings) {
      Found& f = by[l.community];
      f.community = l.community;
      const std::string where = r.keeper + "/" + r.holding;
      if (std::find(f.registries.begin(), f.registries.end(), where) == f.registries.end()) f.registries.push_back(where);
      if (f.status.empty() || rank(c.status) > rank(f.status)) f.status = c.status;
      if (c.status != "faulty" && (f.newest.community.empty() || l.head.size > f.newest.head.size)) f.newest = l;
      for (const Listing* o : seen[l.community]) {
        if (o->head.size == l.head.size && o->head.root != l.head.root)
          f.disagreements.push_back("two listings name heads of " + std::to_string(l.head.size) + " entries with different roots");
        if (o->order != l.order) f.disagreements.push_back("the listings give the orders " + o->order + " and " + l.order);
      }
      seen[l.community].push_back(&l);
    }
  std::vector<Found> out;
  for (auto& [community, f] : by) {
    if (f.newest.community.empty()) continue;  // every listing of it is faulty
    if (q.current_only && f.status != "current") continue;
    if (!q.order.empty() && f.newest.order != q.order) continue;
    if (!q.position.empty() && std::find(f.newest.open_positions.begin(), f.newest.open_positions.end(), q.position) == f.newest.open_positions.end()) continue;
    bool words = true;
    for (const auto& w : q.words) words = words && detail::lower(f.newest.purpose).find(detail::lower(w)) != std::string::npos;
    if (!words) continue;
    std::sort(f.disagreements.begin(), f.disagreements.end());
    f.disagreements.erase(std::unique(f.disagreements.begin(), f.disagreements.end()), f.disagreements.end());
    out.push_back(std::move(f));
  }
  return out;
}

}  // namespace communities::registries
