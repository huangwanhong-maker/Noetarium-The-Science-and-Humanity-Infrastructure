// Checks of the law domain's exhibits: an exhibit with unbroken custody offered into a chart and carrying its proposition; the
// same exhibit with its custody broken, undercut by the archive's finding; a second exhibit that carries the proposition
// again; the exhibits as traces of a record of facts.

#include <archeion/fixity/sha256.hpp>
#include <law/exhibits/exhibits.hpp>

#include <calx/verification/check.hpp>

#include <string>

using namespace law;
namespace te = tekmerion::evidence;
namespace aa = apodeixis::argumentation;

namespace {

const std::string deed = archeion::fixity::sha256_hex("the deed of 3 May, scanned"), statement = archeion::fixity::sha256_hex("the notary's statement");

te::Chart base() {
  return te::chart(
      "source rivers: the claimant\nsource notary: the notary\n"
      "proposition signed: Rivers signed the deed\nproposition sale: a contract of sale exists\n"
      "ultimate sale\n"
      "link signed supports sale by g1: who signs a deed of sale sells\n");
}

exhibits::Exhibit the_deed() {
  exhibits::Exhibit e;
  e.id = "exhibit_A";
  e.reports = "signed";
  e.source = "rivers";
  e.kind = te::ItemKind::document;
  e.chain = {deed, "notary", "2019-05-03T10:00:00Z", {{"2019-05-04T09:00:00Z", "notary", "rivers", deed, ""}, {"2020-01-10T09:00:00Z", "rivers", "court registry", deed, ""}}};
  e.observed = deed;
  e.identity = {{"creator", "the notary"}, {"date", "2019-05-03"}, {"reference", "DC/CIV/2019-0012/002"}};
  e.text = "the deed";
  return e;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  {
    const auto e = the_deed();
    s.require("the deed, handed from the notary to the claimant to the registry, is authentic", exhibits::authenticate(e).holds());
    const auto offered = exhibits::offer(base(), {e});
    const auto st = te::standing(offered.chart);
    s.require("offered into the chart, it carries the signing, and the sale is established", te::faults(offered.chart).empty() && st.verdict.holds() && st.of.at("signed") == aa::Standing::justified);
  }
  {
    auto e = the_deed();
    e.chain.transfers[1].from = "an unknown hand";  // who gave it to the registry?
    const auto v = exhibits::authenticate(e);
    s.require("with a handover by someone who did not hold it, the deed is not authentic, for custody", v.fails() && v.piece("fails") == "custody");
    const auto offered = exhibits::offer(base(), {e});
    const auto st = te::standing(offered.chart);
    s.require("offered all the same, it is undercut by the archive's finding: the signing is overruled and the sale not established",
              te::faults(offered.chart).empty() && st.verdict.fails() && st.of.at("signed") == aa::Standing::overruled && st.of.at("unauthentic:exhibit_A") == aa::Standing::justified, st.verdict.reason);
    s.require("the exhibit stays in the chart, with the archive's verification beside it", offered.chart.items.size() == 2 && offered.chart.source("the_archive") != nullptr);

    // A second exhibit, the notary's statement, with its custody whole.
    exhibits::Exhibit n;
    n.id = "exhibit_B";
    n.reports = "signed";
    n.source = "notary";
    n.kind = te::ItemKind::testimony;
    n.chain = {statement, "notary", "2020-02-01T10:00:00Z", {{"2020-02-02T09:00:00Z", "notary", "court registry", statement, ""}}};
    n.observed = statement;
    n.identity = {{"creator", "the notary"}, {"date", "2020-02-01"}, {"reference", "DC/CIV/2019-0012/005"}};
    const auto both = exhibits::offer(base(), {e, n});
    s.require("a second exhibit with its custody whole carries the signing again, and the sale is established", te::standing(both.chart).verdict.holds());

    auto changed = the_deed();
    changed.observed = archeion::fixity::sha256_hex("the deed of 3 May, scanned, and altered");
    s.require("a deed whose bytes are no longer those handed over is not authentic, for integrity", exhibits::authenticate(changed).piece("fails") == "integrity");
  }
  {
    const auto ts = exhibits::traces({the_deed()});
    s.require("an exhibit is a trace of the fact it reports, a document held under its digest", ts.size() == 1 && ts[0].of == "signed" && ts[0].kind == "Document" && ts[0].holding == deed);
    facts::Record r;
    facts::Fact sig = facts::fact("signed", "Rivers signed the deed");
    sig.kind = "Social", sig.standing = facts::Standing::proved, sig.role = facts::Role::evidentiary;
    sig.emerged_from = "the meeting at the notary's", sig.since = "2019-05-03", sig.constrains = {"sale"};
    r.facts = {sig};
    r.traces = ts;
    s.require("and with it the record of facts is well formed: the evidentiary fact has its trace", facts::well_formed(r).holds());
  }

  return s.done();
}
