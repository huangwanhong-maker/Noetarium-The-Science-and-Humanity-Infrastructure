// Evidence kept in space and time, for any work's engine (ADR 0123): one composition of archeion (what is kept, and whether it is
// still what it was), tekmerion (what the evidence is worth for a claim about a place and a time) and tyche (what to do about it,
// and what to look at first), with geodesy's distances, answering requests in JSON. A request holds the whole archive; nothing is
// read from a file or kept between requests.
//
// The archive, noetarium.evidence/1:
//   {sources:  [{id, name, hit, false_alarm | sensitivity, objectivity, veracity}],     a report's channel, or Schum's three attributes
//    holdings: [{id, name, digest, observed, custodian, since, transfers: [{when, from, to, digest, note}], attributes: {…}}],
//    items:    [{id, title, kind, holding, lat, lon, radius, from, until, source, origin, relay, says, status, claims}],
//    claims:   [{id, text, lat, lon, radius, from, until, prior, e_given_h, e_given_not_h, kinds, event}],
//    rule:     {established, refuted},   least_about,
//    events, unit, statements: [{id, source, from, to, least, most, text}],             statements of time, for `timeline`
//    decision: {acts: [{name, holds, not}], inspection: {sensitivity, specificity, cost}, costs: {claim: cost}, budget, charged}}
//
// An item is a report: of a kind, by a source, of something at a place (degrees on WGS 84, good to `radius` metres) and a time
// (`from` to `until`, numbers in any one unit; an instant when they are equal). It says that what a claim claims holds, or denies
// it. `source` is who registered what it reports. `origin` is a label for one registration that several items repeat (a wire
// three papers print, a household that complains three times): items with one origin are counted as what they share is worth
// (tekmerion's dependence), not as independent reports; `relay` names the source through which an item passes its origin on, when
// that is not perfect. `holding` names the file it rests on; `status` is
// accepted (the default), proposed or rejected, and only an accepted item counts. `claims` ties an item to claims outright;
// without it an item bears on the claims whose place and time it may fall in.
//
// A claim says that something holds within `radius` metres of a place during an interval. P(E | H) and P(E | ¬H) relate what an
// item reports to the claim (1 and 0 by default: the item reports the claim itself); `kinds` are the kinds of item that bear on it
// (any, when empty). `event` says what the items report: "each" (the default), an event of its own for each group of items (lots
// inspected one by one, rats seen on different days), so that their force accumulates; or "one", one event that all report
// (witnesses of one fire), so that they can do no more than settle it (tekmerion's located, the theory note §2.2).
//
//   keep      each holding followed by archeion: its chain of custody, its digest now against the one handed over, its
//             authenticity; and each item sound or not, with the reason (its holding altered, its custody broken, what it was
//             derived from tainted)
//   weigh     for each claim, the items that bear on it, each with how far it is about the claim (the share of where and when
//             it may be that falls in the claim's place and time) and what it would be worth alone; the items grouped by what
//             they share; the likelihood ratio of them all, what counting them as independent would have given, the posterior
//             and its standing by the declared rule; and the items left out, each with its reason
//   decide    for each claim a decision on its posterior: the acts' expected worths, the value of knowing, what an inspection
//             of declared sensitivity and specificity would be worth and what would be done after it; and, across the claims,
//             the inspections to run within the budget (tyche's information bought under a budget, on calx's knapsack)
//   timeline  the statements of time read as a simple temporal network: consistent, or in conflict with the sources named
//   all       keep, weigh, then decide and timeline where the archive states them
//   rates     {found, clear, reported_found, reported_clear, confidence?}: a kind of report's channel counted against a reference
//             (of the units where the reference found the thing, and of those it cleared, how many were reported), each rate with
//             Wilson's interval and the exact one (tyche), and the likelihood ratios of a report and of a silence
//
// The methods are the foundations'; the engine only reads, calls and writes.

#pragma once

#include "json.hpp"

#include <archeion/custody/custody.hpp>
#include <archeion/provenance/provenance.hpp>
#include <geodesy/geodesic/geodesic.hpp>
#include <tekmerion/evidence/dependence.hpp>
#include <tekmerion/evidence/force.hpp>
#include <tekmerion/located/located.hpp>
#include <tekmerion/timelines/timelines.hpp>
#include <tyche/decision/budget.hpp>
#include <tyche/probability/proportions.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace science::spacetime_evidence {

namespace te = tekmerion::evidence;
namespace tl = tekmerion::located;
using apodeixis::kernel::Verdict;

inline constexpr double nan = std::numeric_limits<double>::quiet_NaN();

inline Json num(double x) { return std::isfinite(x) ? Json(x) : Json(nullptr); }
inline double number(const Json& j, double fallback = nan) { return j.is_number() ? j.as_number() : fallback; }

inline Json strings(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& s : v) a.push(s);
  return a;
}

inline Json verdict_json(const Verdict& v) {
  Json evidence = Json::array();
  for (const auto& e : v.evidence) evidence.push(Json::object({{"role", e.role}, {"value", e.value}}));
  return Json::object({{"status", std::string(apodeixis::kernel::word(v.status))}, {"claim", v.claim}, {"reason", v.reason}, {"evidence", evidence}});
}

// Four significant figures, for the sentences.
inline std::string said(double x) {
  char b[48];
  std::snprintf(b, sizeof b, "%.4g", x);
  return b;
}

// ---- What is kept --------------------------------------------------------------------------------------------------------------------

struct Kept {
  Json out;
  std::map<std::string, std::string> unsound;  // item → why it cannot be relied on
  std::set<std::string> unkept;                // items that rest on no holding
};

inline Kept keep(const Json& a) {
  Kept k;
  Json holdings = Json::array();
  std::map<std::string, std::string> failed;  // holding → why
  std::size_t intact = 0, unchecked = 0;
  archeion::provenance::Graph g;
  const Json& hs = a["holdings"];
  for (std::size_t i = 0; i < hs.size(); i++) {
    const Json& h = hs[i];
    const std::string id = h["id"].text();
    g.entities.push_back({id, {}});
    archeion::custody::Chain chain;
    chain.digest = h["digest"].text(), chain.custodian = h["custodian"].text(), chain.since = h["since"].text();
    for (std::size_t t = 0; t < h["transfers"].size(); t++) {
      const Json& x = h["transfers"][t];
      chain.transfers.push_back({x["when"].text(), x["from"].text(), x["to"].text(), x["digest"].text(), x["note"].text()});
    }
    const auto c = archeion::custody::verify(chain);
    Json breaks = Json::array();
    for (const auto& b : c.breaks) breaks.push(Json::object({{"at", static_cast<double>(b.at)}, {"what", b.what}}));
    Json one = Json::object({{"id", id}, {"name", h["name"].text()}, {"custodian", c.custodian}, {"digest", c.digest}, {"handovers", static_cast<double>(chain.transfers.size())},
                             {"custody", verdict_json(c.verdict)}, {"breaks", breaks}});
    std::string fixity = "not observed";
    if (chain.digest.empty()) {
      fixity = "no digest";
      failed[id] = "no digest was taken when " + (h["name"].text().empty() ? id : h["name"].text()) + " was taken in, so nothing shows that it is what it was";
    } else if (h["observed"].is_string()) {
      std::vector<std::pair<std::string, std::string>> attributes;
      if (h["attributes"].is_object())
        for (const auto& [name, value] : h["attributes"].as_object()) attributes.emplace_back(name, value.text());
      const auto v = archeion::custody::authentic(attributes, chain, h["observed"].text());
      one.set("authentic", verdict_json(v));
      fixity = h["observed"].text().empty() ? "missing" : h["observed"].text() == c.digest ? "intact" : "altered";
      if (!v.holds()) failed[id] = h["observed"].text().empty() ? "the file is not there" : v.reason;
      else intact++;
    } else {
      unchecked++;
      if (!c.verdict.holds()) failed[id] = c.verdict.reason;
    }
    one.set("fixity", fixity);
    one.set("sound", !failed.count(id));
    if (failed.count(id)) one.set("why", failed[id]);
    holdings.push(one);
  }

  // Each item is derived from the holding it rests on; whatever is derived from a holding that failed is tainted with it.
  const Json& items = a["items"];
  for (std::size_t i = 0; i < items.size(); i++) {
    const std::string id = items[i]["id"].text(), h = items[i]["holding"].text();
    g.entities.push_back({"item:" + id, {}});
    if (!h.empty() && g.entity(h) >= 0) g.activities.push_back({"read:" + id, "recognition", "", "", {h}, {"item:" + id}, ""});
  }
  std::set<std::string> altered;
  for (const auto& [id, why] : failed) altered.insert(id);
  const auto tainted = archeion::provenance::tainted(g, altered);
  Json item_out = Json::array();
  std::size_t sound = 0;
  for (std::size_t i = 0; i < items.size(); i++) {
    const std::string id = items[i]["id"].text(), h = items[i]["holding"].text();
    Json one = Json::object({{"id", id}, {"holding", h}});
    if (h.empty()) {
      k.unkept.insert(id);
      one.set("sound", true), one.set("kept", false), one.set("why", "no holding is kept for it: it rests on its source's word as it was entered");
      sound++;
    } else if (g.entity(h) < 0) {
      k.unsound[id] = "it rests on the holding " + h + ", which the archive does not have";
      one.set("sound", false), one.set("kept", false), one.set("why", k.unsound[id]);
    } else if (tainted.count("item:" + id)) {
      k.unsound[id] = "it rests on " + h + ": " + failed[h];
      one.set("sound", false), one.set("kept", true), one.set("why", k.unsound[id]);
    } else {
      one.set("sound", true), one.set("kept", true);
      sound++;
    }
    item_out.push(one);
  }
  const std::string claim = "the archive is what it was when it was taken in";
  const Verdict v = failed.empty()
      ? (unchecked ? Verdict::unknown(claim, std::to_string(intact) + " of " + std::to_string(hs.size()) + " holdings were observed and are intact, their custody unbroken; " + std::to_string(unchecked) + " were not observed now")
                   : Verdict::yes(claim, "each of the " + std::to_string(hs.size()) + " holdings has the digest it was handed over with, and an unbroken chain of custody"))
      : Verdict::no(claim, std::to_string(failed.size()) + " of " + std::to_string(hs.size()) + " holdings cannot be relied on; the first: " + failed.begin()->first + ", " + failed.begin()->second);
  k.out = Json::object({{"holdings", holdings}, {"items", item_out}, {"verdict", verdict_json(v)}, {"sound_items", static_cast<double>(sound)}, {"items_in_all", static_cast<double>(items.size())},
                        {"failed", static_cast<double>(failed.size())}, {"intact", static_cast<double>(intact)}, {"unchecked", static_cast<double>(unchecked)}});
  return k;
}

// ---- What the evidence is worth for a claim ----------------------------------------------------------------------------------------

inline std::map<std::string, te::Channel> channels(const Json& a) {
  std::map<std::string, te::Channel> out;
  const Json& ss = a["sources"];
  for (std::size_t i = 0; i < ss.size(); i++) {
    te::Source s;
    s.id = ss[i]["id"].text();
    if (ss[i]["hit"].is_number() && ss[i]["false_alarm"].is_number()) s.hit = ss[i]["hit"].as_number(), s.false_alarm = ss[i]["false_alarm"].as_number();
    s.sensitivity = number(ss[i]["sensitivity"], 1), s.objectivity = number(ss[i]["objectivity"], 1), s.veracity = number(ss[i]["veracity"], 1);
    out[s.id] = te::channel(s);
  }
  return out;
}

struct Weighed {
  Json out;
  std::map<std::string, double> posterior;  // claim → P(the claim | the evidence)
};

inline Weighed weigh(const Json& a, const Kept& kept) {
  Weighed w;
  const auto channel_of = channels(a);
  const auto geodesic = geodesy::geodesic::Geodesic::wgs84();
  const double established = number(a["rule"]["established"], 0.95), refuted = number(a["rule"]["refuted"], 0.05), least_about = number(a["least_about"], 0);
  const Json& items = a["items"];
  const Json& claims = a["claims"];
  Json out = Json::array();
  for (std::size_t c = 0; c < claims.size(); c++) {
    const Json& cl = claims[c];
    const std::string cid = cl["id"].text();
    const double prior = number(cl["prior"], 0.5), eh = number(cl["e_given_h"], 1), en = number(cl["e_given_not_h"], 0);
    std::set<std::string> kinds;
    for (std::size_t k = 0; k < cl["kinds"].size(); k++) kinds.insert(cl["kinds"][k].text());
    const bool placed = cl["lat"].is_number() && cl["lon"].is_number(), timed = cl["from"].is_number() && cl["until"].is_number();
    const std::string event = cl["event"].text("each");
    if (event != "each" && event != "one") throw std::invalid_argument("the claim " + cid + " says its items report \"" + event + "\": an event each, or one event");
    const auto ratio_of = [&](const std::vector<te::Group>& gs, const std::vector<double>& qs) { return event == "one" ? tl::located_ratio(eh, en, gs, qs) : tl::events_ratio(eh, en, gs, qs); };

    struct Bearing {
      std::string id, source, origin, relay_by;
      double space = 1, time = 1, about = 1, distance = nan, radius = 0, span = 0;  // radius, span: how precisely it is placed
      bool says = true;
      te::Channel relay;
    };
    std::vector<Bearing> bearing;
    Json left = Json::array();
    const auto leave = [&](const std::string& id, const std::string& why) { left.push(Json::object({{"id", id}, {"why", why}})); };
    for (std::size_t i = 0; i < items.size(); i++) {
      const Json& it = items[i];
      const std::string id = it["id"].text();
      bool tied = false, named = it["claims"].is_array() && it["claims"].size() > 0;
      for (std::size_t k = 0; k < it["claims"].size(); k++) tied = tied || it["claims"][k].text() == cid;
      if (named && !tied) continue;                                              // tied to other claims
      if (!named && !kinds.empty() && !kinds.count(it["kind"].text())) continue;  // of a kind that does not bear on this claim
      Bearing b;
      b.id = id, b.source = it["source"].text(), b.origin = it["origin"].text(), b.relay_by = it["relay"].text();
      b.says = it["says"].text("holds") != "denies";
      if (!tied) {
        if (placed) {
          if (!(it["lat"].is_number() && it["lon"].is_number())) continue;       // not placed: it cannot be said to be about a place
          // Far apart by their latitudes or longitudes alone, they share nothing, and the geodesic need not be found: a degree of
          // latitude is never under 110.5 km, nor one of longitude under 111.3 km times the cosine of the latitude, less a part in 300.
          const double reach = std::max(0.0, number(it["radius"], 0)) + std::max(0.0, number(cl["radius"], 0));
          const double north = std::abs(it["lat"].as_number() - cl["lat"].as_number()) * 110500.0;
          const double widest = std::max(std::abs(it["lat"].as_number()), std::abs(cl["lat"].as_number()));
          const double east = std::abs(std::remainder(it["lon"].as_number() - cl["lon"].as_number(), 360.0)) * 110900.0 * std::cos(widest * 3.14159265358979323846 / 180);
          if (north > reach || (widest < 89 && east > reach)) continue;
          b.distance = geodesic.inverse_degrees(cl["lat"].as_number(), cl["lon"].as_number(), it["lat"].as_number(), it["lon"].as_number()).s12;
          b.space = tl::share_of_disc(std::max(0.0, number(it["radius"], 0)), std::max(0.0, number(cl["radius"], 0)), b.distance);
        }
        if (timed) {
          if (!it["from"].is_number()) continue;                                 // not timed
          const double from = it["from"].as_number(), until = std::max(from, number(it["until"], from));
          b.time = tl::share_of_span(from, until, cl["from"].as_number(), cl["until"].as_number());
          b.span = until - from;
        }
        b.about = tl::about(b.space, b.time);
        if (!(b.about > least_about)) continue;                                  // elsewhere, or at another time
      }
      b.radius = std::max(0.0, number(it["radius"], 0));
      // From here the item is about the claim; whether it counts is said.
      const std::string status = it["status"].text("accepted");
      if (status != "accepted") { leave(id, status == "proposed" ? "it is a proposal, and counts for nothing until a person accepts it" : "it was rejected"); continue; }
      if (kept.unsound.count(id)) { leave(id, kept.unsound.at(id)); continue; }
      if (!channel_of.count(b.source)) { leave(id, b.source.empty() ? "it names no source" : "its source, " + b.source + ", is not declared"); continue; }
      if (!b.relay_by.empty() && !channel_of.count(b.relay_by)) { leave(id, "what relays it, " + b.relay_by + ", is not a declared source"); continue; }
      b.relay = b.relay_by.empty() ? te::Channel{1, 0} : channel_of.at(b.relay_by);
      bearing.push_back(b);
    }

    // The items grouped by what they share: an origin's items together, each other item alone.
    std::vector<te::Group> groups;
    std::vector<double> about;
    std::vector<std::vector<std::size_t>> members;
    std::map<std::pair<std::string, std::string>, std::size_t> group_of;  // (origin, source) → its group
    for (std::size_t i = 0; i < bearing.size(); i++) {
      const auto& b = bearing[i];
      const auto key = std::pair(b.origin, b.source);
      if (b.origin.empty() || !group_of.count(key)) {
        if (!b.origin.empty()) group_of[key] = groups.size();
        groups.push_back({channel_of.at(b.source), {{b.relay, b.says}}}), about.push_back(b.about), members.push_back({i});
      } else {
        const std::size_t g = group_of[key];
        groups[g].relays.push_back({b.relay, b.says}), members[g].push_back(i);
        // A group is placed by its most precisely placed report.
        const auto& best = bearing[*std::min_element(members[g].begin(), members[g].end(), [&](std::size_t x, std::size_t y) { return std::pair(bearing[x].radius, bearing[x].span) < std::pair(bearing[y].radius, bearing[y].span); })];
        about[g] = best.about;
      }
    }

    Json one = Json::object({{"id", cid}, {"text", cl["text"].text()}, {"prior", num(prior)}, {"event", event}, {"lat", cl["lat"].is_number() ? Json(cl["lat"].as_number()) : Json(nullptr)},
                             {"lon", cl["lon"].is_number() ? Json(cl["lon"].as_number()) : Json(nullptr)}, {"radius", num(number(cl["radius"], 0))}});
    try {
      const double ratio = ratio_of(groups, about);
      std::vector<te::Group> apart;
      std::vector<double> apart_about;
      Json item_out = Json::array(), group_out = Json::array();
      for (std::size_t g = 0; g < groups.size(); g++) {
        Json ids = Json::array();
        for (std::size_t i : members[g]) {
          const auto& b = bearing[i];
          const te::Group alone{groups[g].origin, {{b.relay, b.says}}};
          apart.push_back({{}, {{te::compose(groups[g].origin, b.relay), b.says}}}), apart_about.push_back(b.about);
          ids.push(b.id);
          item_out.push(Json::object({{"id", b.id}, {"source", b.source}, {"origin", b.origin}, {"relay", b.relay_by}, {"says", b.says ? "holds" : "denies"}, {"distance", num(b.distance)}, {"space", num(b.space)},
                                      {"time", num(b.time)}, {"about", num(b.about)}, {"alone", num(ratio_of({alone}, {b.about}))}, {"group", static_cast<double>(g)}}));
        }
        group_out.push(Json::object({{"origin", bearing[members[g][0]].origin}, {"source", bearing[members[g][0]].source}, {"items", ids}, {"about", num(about[g])},
                                     {"alone", num(ratio_of({groups[g]}, {about[g]}))}}));
      }
      const double counted = ratio_of(apart, apart_about);
      const double posterior = te::posterior(prior, {ratio});
      w.posterior[cid] = posterior;
      const std::string standing = bearing.empty() ? "no evidence" : posterior >= established ? "established" : posterior <= refuted ? "refuted" : "open";
      std::string why = bearing.empty() ? "no accepted item that is kept sound bears on it: it stands at its prior, " + said(prior)
                                        : std::to_string(bearing.size()) + (bearing.size() == 1 ? " item" : " items") + " in " + std::to_string(groups.size()) +
                                              (groups.size() == 1 ? " independent group" : " independent groups") + " give a likelihood ratio of " + said(ratio) + ", taking the prior " + said(prior) + " to " + said(posterior);
      if (groups.size() < bearing.size() && std::isfinite(counted) && std::isfinite(ratio) && ratio > 0)
        why += "; counted as " + std::to_string(bearing.size()) + " independent reports they would have given " + said(counted) + ", " + said(counted / ratio) + " times as much";
      one.set("ratio", num(ratio)), one.set("decibans", num(ratio > 0 && std::isfinite(ratio) ? 10 * std::log10(ratio) : nan)), one.set("posterior", num(posterior));
      one.set("as_if_independent", num(counted)), one.set("overstatement", num(ratio > 0 && std::isfinite(ratio) ? counted / ratio : nan));
      one.set("standing", standing), one.set("why", why), one.set("items", item_out), one.set("groups", group_out), one.set("left_out", left);
      one.set("conclusive", std::isinf(ratio) || ratio == 0);
    } catch (const std::exception& e) {
      one.set("error", e.what()), one.set("left_out", left);
    }
    out.push(one);
  }
  w.out = Json::object({{"claims", out}, {"rule", Json::object({{"established", established}, {"refuted", refuted}})}});
  return w;
}

// ---- What to do, and what to look at first --------------------------------------------------------------------------------------------

inline Json decide(const Json& a, const Weighed& w) {
  namespace td = tyche::decision;
  const Json& d = a["decision"];
  if (d["acts"].size() < 2) throw std::invalid_argument("a decision states two acts or more, each with what it is worth if the claim holds and if it does not");
  const double sens = number(d["inspection"]["sensitivity"], nan), spec = number(d["inspection"]["specificity"], nan), unit_cost = number(d["inspection"]["cost"], 1);
  const bool inspects = std::isfinite(sens) && std::isfinite(spec);
  const bool charged = d["charged"].is_bool() && d["charged"].as_bool();
  std::vector<td::Informable> problems;
  const Json& claims = a["claims"];
  for (std::size_t c = 0; c < claims.size(); c++) {
    const std::string cid = claims[c]["id"].text();
    if (!w.posterior.count(cid)) continue;
    td::Informable q;
    q.name = cid;
    q.table.states = {"holds", "does not"};
    for (std::size_t k = 0; k < d["acts"].size(); k++) {
      q.table.acts.push_back(d["acts"][k]["name"].text("act " + std::to_string(k + 1)));
      q.table.u.push_back({number(d["acts"][k]["holds"], 0), number(d["acts"][k]["not"], 0)});
    }
    const double p = w.posterior.at(cid);
    q.prior = {p, 1 - p};
    q.test.signals = {"found", "clear"};
    q.test.likelihood = inspects ? std::vector<std::vector<double>>{{sens, 1 - sens}, {1 - spec, spec}} : std::vector<std::vector<double>>{{0.5, 0.5}, {0.5, 0.5}};
    q.cost = d["costs"].is_object() && d["costs"][cid].is_number() ? d["costs"][cid].as_number() : unit_cost;
    problems.push_back(std::move(q));
  }
  const double budget = number(d["budget"], 0);
  const auto bought = td::buy(problems, inspects ? budget : 0.0, charged);
  const std::set<std::size_t> run(bought.bought.begin(), bought.bought.end());
  Json each = Json::array();
  for (std::size_t i = 0; i < problems.size(); i++) {
    const auto& q = problems[i];
    const auto& pre = bought.analysis[i];
    const auto values = td::expectations(q.table, q.prior);
    const std::size_t best = static_cast<std::size_t>(std::max_element(values.begin(), values.end()) - values.begin());
    Json acts = Json::array();
    for (std::size_t k = 0; k < values.size(); k++) acts.push(Json::object({{"name", q.table.acts[k]}, {"worth", num(values[k])}}));
    Json one = Json::object({{"claim", q.name}, {"posterior", num(q.prior[0])}, {"acts", acts}, {"act", q.table.acts[best]}, {"worth", num(values[best])}, {"evpi", num(td::evpi(q.table, q.prior))}});
    if (inspects) {
      one.set("evsi", num(pre.evsi)), one.set("cost", num(q.cost)), one.set("inspect", run.count(i) > 0);
      one.set("p_found", num(pre.p_signal[0]));
      one.set("after_found", pre.posterior[0].empty() ? Json(nullptr) : Json(q.table.acts[pre.act[0]]));
      one.set("after_clear", pre.posterior[1].empty() ? Json(nullptr) : Json(q.table.acts[pre.act[1]]));
      one.set("if_found", pre.posterior[0].empty() ? Json(nullptr) : num(pre.posterior[0][0]));
      one.set("if_clear", pre.posterior[1].empty() ? Json(nullptr) : num(pre.posterior[1][0]));
    }
    each.push(one);
  }
  Json out = Json::object({{"claims", each}, {"without", num(bought.without)}, {"inspects", inspects}, {"test", d["inspection"]["name"].text("an inspection")}});
  if (inspects) {
    Json ids = Json::array();
    for (std::size_t i : bought.bought) ids.push(problems[i].name);
    out.set("inspect", ids), out.set("gained", num(bought.gained)), out.set("spent", num(bought.spent)), out.set("budget", num(budget)), out.set("bound", num(bought.bound));
    out.set("exact", bought.exact), out.set("charged", charged);
    out.set("why", "looking first at " + std::to_string(bought.bought.size()) + (bought.bought.size() == 1 ? " place" : " places") + " by " + d["inspection"]["name"].text("an inspection") + ", within the budget of " +
                       said(budget) + " and costing " + said(bought.spent) + ", is expected to improve what is done by " + said(bought.gained) +
                       (bought.exact ? ": no other set within the budget gains more" : ": the best set found, no set gaining more than " + said(bought.bound)));
  }
  return out;
}

// ---- When things happened -------------------------------------------------------------------------------------------------------------

inline Json timeline(const Json& a) {
  namespace tt = tekmerion::timelines;
  tt::Timeline t;
  t.unit = a["unit"].text("minutes");
  for (std::size_t k = 0; k < a["events"].size(); k++) t.events.push_back(a["events"][k].text());
  for (std::size_t k = 0; k < a["statements"].size(); k++) {
    const Json& x = a["statements"][k];
    t.statements.push_back({x["id"].text(), x["source"].text(), x["from"].text(), x["to"].text(), number(x["least"], -tt::unbounded), number(x["most"], tt::unbounded), x["text"].text()});
  }
  const auto r = tt::read(t);
  Json out = Json::object({{"consistent", r.consistent}, {"conflict", strings(r.conflict)}, {"verdict", verdict_json(r.verdict)}, {"unit", t.unit}});
  if (r.consistent) {
    Json events = Json::array();
    for (std::size_t e = 0; e < t.events.size(); e++) events.push(Json::object({{"name", t.events[e]}, {"earliest", num(r.earliest[e])}, {"latest", num(r.latest[e])}, {"at", num(r.schedule[e])}}));
    out.set("events", events);
  } else {
    out.set("resolving", strings(tt::resolving(t)));
  }
  return out;
}

// ---- A channel counted ----------------------------------------------------------------------------------------------------------------

inline Json rates(const Json& r) {
  namespace tp = tyche::probability;
  const auto whole = [&](const char* name) {
    const double x = number(r[name]);
    if (!(x >= 0) || x != std::floor(x)) throw std::invalid_argument(std::string(name) + " is a count");
    return static_cast<std::int64_t>(x);
  };
  const std::int64_t found = whole("found"), clear = whole("clear"), rf = whole("reported_found"), rc = whole("reported_clear");
  if (found < 1 || clear < 1) throw std::invalid_argument("the reference found the thing nowhere, or everywhere: a channel has both rates");
  const double confidence = number(r["confidence"], 0.95);
  const auto rate = [&](std::int64_t k, std::int64_t n) {
    const auto w = tp::wilson_interval(k, n, confidence), e = tp::clopper_pearson_interval(k, n, confidence);
    return Json::object({{"rate", static_cast<double>(k) / static_cast<double>(n)}, {"of", static_cast<double>(n)}, {"reported", static_cast<double>(k)},
                         {"wilson", Json::array({Json(w.first), Json(w.second)})}, {"exact", Json::array({Json(e.first), Json(e.second)})}});
  };
  const double hit = static_cast<double>(rf) / static_cast<double>(found), fa = static_cast<double>(rc) / static_cast<double>(clear);
  return Json::object({{"hit", rate(rf, found)}, {"false_alarm", rate(rc, clear)}, {"confidence", confidence},
                       {"ratio_of_a_report", fa > 0 ? num(hit / fa) : Json(nullptr)}, {"ratio_of_a_silence", fa < 1 ? num((1 - hit) / (1 - fa)) : Json(nullptr)}});
}

inline Json answer(const Json& r) {
  const std::string op = r["op"].text();
  if (op == "rates") return rates(r);
  const Json& a = r["archive"];
  if (op == "keep") return keep(a).out;
  if (op == "weigh") return weigh(a, keep(a)).out;
  if (op == "decide") return decide(a, weigh(a, keep(a)));
  if (op == "timeline") return timeline(a);
  if (op == "all") {
    const auto k = keep(a);
    const auto w = weigh(a, k);
    Json out = Json::object({{"keep", k.out}, {"weigh", w.out}});
    if (a["decision"].is_object()) {
      try { out.set("decide", decide(a, w)); } catch (const std::exception& e) { out.set("decide", Json::object({{"error", e.what()}})); }
    }
    if (a["statements"].size()) {
      try { out.set("timeline", timeline(a)); } catch (const std::exception& e) { out.set("timeline", Json::object({{"error", e.what()}})); }
    }
    return out;
  }
  throw std::invalid_argument("science::spacetime_evidence: no operation is called " + op + " (there is: keep, weigh, decide, timeline, all, rates)");
}

}  // namespace science::spacetime_evidence
