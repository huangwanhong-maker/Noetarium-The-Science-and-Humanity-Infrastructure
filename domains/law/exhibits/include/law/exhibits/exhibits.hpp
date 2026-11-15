// Exhibits: a case's evidence as holdings of an archive, and what the archive's findings do to the evidence's weight.
//
// An exhibit is a trace kept in an archive (archeion): it has a digest, a chain of custody and the attributes that say what
// it is. Before what it shows is weighed, a court asks whether it is what it is said to be: authentication, which is
// archeion's authenticity (identity, integrity, custody). This header joins the two foundations. Each exhibit becomes an item
// of a chart of evidence (tekmerion) reporting a proposition; and an exhibit that fails authentication brings into the chart
// the archive's own finding, as an item from the archive that undercuts the exhibit: the step from "this was produced" to
// "what it shows happened" no longer goes through. Nothing is removed: the exhibit stays in the chart, defeated, with the
// reason, and can be reinstated if the break is explained.
//
// What authenticity establishes is that the holding is the trace taken in, unchanged. Whether the trace shows what it is
// offered for is the chart's question, and whether what it shows happened is left open (theory/facts-and-their-registers.md).

#pragma once

#include <archeion/custody/custody.hpp>
#include <law/facts/facts.hpp>
#include <tekmerion/evidence/chart.hpp>

#include <set>
#include <string>
#include <vector>

namespace law::exhibits {

namespace ac = archeion::custody;
namespace te = tekmerion::evidence;
using apodeixis::kernel::Verdict;

struct Exhibit {
  std::string id;
  std::string reports;   // the proposition (a fact's id) it is offered to show
  std::string source;    // who produced it: a source of the chart
  te::ItemKind kind = te::ItemKind::object;
  ac::Chain chain;
  std::string observed;  // its digest as computed now
  std::vector<std::pair<std::string, std::string>> identity;  // the attributes that say what it is
  std::string text;
};

// Whether the exhibit is authentic: archeion's verdict on its identity, integrity and custody.
inline Verdict authenticate(const Exhibit& e, const std::set<std::pair<std::string, std::string>>& accounted = {}) { return ac::authentic(e.identity, e.chain, e.observed, accounted); }

struct Offered {
  te::Chart chart;
  std::vector<std::pair<std::string, Verdict>> authenticity;  // each exhibit's
};

// The chart with the exhibits added as items. An exhibit that is not authentic is undercut by the archive's finding, which
// enters as a record from the source "the archive" (added if absent).
inline Offered offer(te::Chart chart, const std::vector<Exhibit>& exhibits, const std::set<std::pair<std::string, std::string>>& accounted = {}) {
  Offered out;
  bool archive = chart.source("the_archive") != nullptr;
  for (const auto& e : exhibits) {
    chart.items.push_back({e.id, e.source, e.reports, e.text, e.kind, 0});
    const Verdict v = authenticate(e, accounted);
    if (!v.holds()) {
      if (!archive) {
        te::Source s;
        s.id = "the_archive";
        s.text = "the archive's own verification";
        chart.sources.push_back(s);
        archive = true;
      }
      const std::string finding = "unauthentic:" + e.id;
      chart.propositions.push_back({finding, "the exhibit " + e.id + " is not shown to be what it is said to be: " + v.reason, 0});
      chart.items.push_back({"verification:" + e.id, "the_archive", finding, v.reason, te::ItemKind::record, 0});
      te::Link l;
      l.from = finding;
      l.to = e.id;
      l.sense = te::Sense::undercuts;
      l.text = "an exhibit that is not authenticated shows nothing";
      chart.links.push_back(l);
    }
    out.authenticity.push_back({e.id, v});
  }
  out.chart = std::move(chart);
  return out;
}

// The traces of a record of facts that the exhibits are: each exhibit a trace of the fact it reports, by its digest now.
inline std::vector<facts::Trace> traces(const std::vector<Exhibit>& exhibits) {
  std::vector<facts::Trace> out;
  for (const auto& e : exhibits) {
    const std::string kind = e.kind == te::ItemKind::testimony ? "Testimony" : e.kind == te::ItemKind::document || e.kind == te::ItemKind::record ? "Document" : e.kind == te::ItemKind::object ? "Object" : "Data";
    out.push_back({"trace:" + e.id, e.reports, e.observed, kind});
  }
  return out;
}

}  // namespace law::exhibits
