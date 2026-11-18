// Checks of needs/evidence: the tests a statement of need is put to, against what follows from their definitions.
//
// - Parts: a keyed split of clusters, the same on every run and whatever the data; every cluster in one part; fractions met on many
//   clusters; a declared column of parts honoured, and a cluster in two parts refused.
// - The gap, by design: comparative, normative and expressed gaps and their ultimate-cluster variance worked by hand from the
//   clusters' totals; the one-sided p-value; a value weighted by the people it stands for. By binomial reading: tyche's score test
//   and Newcombe's interval on the summed counts.
// - Stability: a planted gap is stable in every resample, no gap in few; the same key gives the same share.
// - Causal support: with the confounder the graph names held fixed, the effect of provision is recovered and holds; without a
//   back-door set it is not identified.
// - Simulation: one supplier, three places, one beyond reach: the floating catchment's access and the seeking model's unmet shares
//   as worked by hand.
// - Rules: entitlement derived through an implication, with its derivation; a group no rule entitles; a domain no rule reaches.
// - Standing: the grounded extension of the framework of objections, case by case.
// - Interpretation (ADR 0126): a statement under every reading of its reference, its measure and its level, each verdict held to
//   the gap test computed again directly; a statement's own reading added; a convention setting aside the analyst's; a model's
//   reading shown only as proposed; causal support under each graph; a rival that explains a planted confound away, its pooled
//   effect held to the strata's gaps by inverse variance; declarations refused.

#include <needs/evidence/interpretation.hpp>
#include <needs/evidence/rules.hpp>
#include <needs/evidence/simulation.hpp>
#include <needs/evidence/stability.hpp>
#include <needs/evidence/standing.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace needs;
using namespace needs::evidence;
using language::Indicator;
using language::read_one;

namespace {

language::Statement statement(const language::Dataset& d, const std::string& text) {
  auto s = read_one(text);
  language::bind(d, s);
  language::check(d, s);
  return s;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- a small table: four clusters of discovery, one of validation, one of confirmation ----
  language::Dataset d;
  {
    std::vector<std::string> cl, part;
    std::vector<double> age, k, n, people, burden, dem, prov;
    const char* names[] = {"A", "B", "C", "D", "E", "F"};
    const double old_k[] = {6, 9, 3, 8, 7, 5}, old_n[] = {20, 30, 15, 25, 20, 20}, young_k[] = {4, 5, 2, 6, 3, 4}, young_n[] = {40, 50, 35, 45, 30, 40};
    const double old_b[] = {0.40, 0.35, 0.50, 0.45, 0.4, 0.4}, young_b[] = {0.25, 0.30, 0.20, 0.28, 0.3, 0.3};
    for (int c = 0; c < 6; c++)
      for (int o = 0; o < 2; o++) {
        cl.push_back(names[c]), part.push_back(c < 4 ? "discovery" : c == 4 ? "validation" : "confirmation");
        age.push_back(o ? 80 : 40);
        k.push_back(o ? old_k[c] : young_k[c]), n.push_back(o ? old_n[c] : young_n[c]);
        people.push_back(o ? old_n[c] : young_n[c]), burden.push_back(o ? old_b[c] : young_b[c]);
        dem.push_back(o ? 10 + c : 4), prov.push_back(o ? 6 : 4);
      }
    d.table.add(groups::categories("area", cl));
    d.table.add(groups::categories("part", part));
    d.table.add(groups::numbers("age", age));
    d.table.add(groups::numbers("k", k));
    d.table.add(groups::numbers("n", n));
    d.table.add(groups::numbers("people", people));
    d.table.add(groups::numbers("burden", burden));
    d.table.add(groups::numbers("demand", dem));
    d.table.add(groups::numbers("provision", prov));
    d.cluster = "area", d.part = "part";
    d.attributes = {"age"};
    d.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "", "", "care", {}};
    d.indicators["burden"] = Indicator{"burden", false, "burden", "people", true, "", "", "housing", {}};
  }
  const Parts parts = split(d);
  s.require("parts: the declared column is honoured (4, 1, 1 clusters)", parts.declared && parts.members[0].size() == 4 && parts.members[1].size() == 1 && parts.members[2].size() == 1);

  // ---- the gap by design, by hand ----
  {
    const auto st = statement(d, "need care for age >= 75 by comparative unassisted higher");
    const Gap g = gap(d, st, parts, Part::discovery);
    const double yg[] = {6, 9, 3, 8}, xg[] = {20, 30, 15, 25}, yr[] = {4, 5, 2, 6}, xr[] = {40, 50, 35, 45};
    double Yg = 0, Xg = 0, Yr = 0, Xr = 0;
    for (int c = 0; c < 4; c++) Yg += yg[c], Xg += xg[c], Yr += yr[c], Xr += xr[c];
    const double Rg = Yg / Xg, Rr = Yr / Xr;
    double u[4], ub = 0;
    for (int c = 0; c < 4; c++) u[c] = (yg[c] - Rg * xg[c]) / Xg - (yr[c] - Rr * xr[c]) / Xr, ub += u[c] / 4;
    double v = 0;
    for (int c = 0; c < 4; c++) v += (u[c] - ub) * (u[c] - ub);
    v *= 4.0 / 3;
    s.check("design: the comparative gap R_G − R_R = 26/90 − 17/170", g.estimate - (Rg - Rr), 1e-15);
    s.check("design: its ultimate-cluster variance, linearised by hand", g.se * g.se - v, 1e-15);
    s.require("design: on clusters − 1 = 3 degrees of freedom", g.df == 3 && g.clusters == 4 && g.rows == 4);
    s.check("design: the one-sided p-value is the t's upper tail at z", g.p - tyche::probability::StudentT{3}.survival(g.estimate / std::sqrt(v)), 1e-14);
    s.check("design: the coverage is the group's people", g.coverage - 90, 0);
    const auto nt = statement(d, "need care for age >= 75 by normative unassisted above 20%");
    const Gap gn = gap(d, nt, parts, Part::discovery);
    double w[4], wb = 0, vn = 0;
    for (int c = 0; c < 4; c++) w[c] = (yg[c] - Rg * xg[c]) / Xg, wb += w[c] / 4;
    for (int c = 0; c < 4; c++) vn += (w[c] - wb) * (w[c] - wb);
    vn *= 4.0 / 3;
    s.check("design: the normative gap R_G − 0.2", gn.estimate - (Rg - 0.2), 1e-15);
    s.check("design: its variance by hand", gn.se * gn.se - vn, 1e-15);
    const auto et = statement(d, "need care for age >= 75 by expressed demand over provision");
    const Gap ge = gap(d, et, parts, Part::discovery);
    s.check("design: the expressed gap is the unmet share of demand, Σ(d − p)/Σd = (4+5+6+7)/(10+11+12+13)", ge.estimate - 22.0 / 46, 1e-15);
    const auto vt = statement(d, "need housing for age >= 75 by comparative burden higher");
    const Gap gv = gap(d, vt, parts, Part::discovery);
    const double ob[] = {0.40, 0.35, 0.50, 0.45}, yb[] = {0.25, 0.30, 0.20, 0.28};
    double a = 0, b = 0, ra = 0, rb = 0;
    for (int c = 0; c < 4; c++) a += xg[c] * ob[c], b += xg[c], ra += xr[c] * yb[c], rb += xr[c];
    s.check("design: a value weighted by the people it stands for", gv.estimate - (a / b - ra / rb), 1e-15);
    const Gap gc = gap(d, st, parts, Part::confirmation);
    s.require("design: one cluster in a part gives no variance, and says why", !gc.defined && gc.why.find("fewer than two clusters") != std::string::npos, gc.why);
  }
  // ---- the binomial reading ----
  {
    const auto st = statement(d, "need care for age >= 75 by comparative unassisted higher");
    const Gap g = gap(d, st, parts, Part::discovery, Inference::binomial);
    const auto c = tyche::testing::share_difference(26, 90, 17, 170);
    s.check("binomial: Newcombe's interval on the summed counts (low)", g.low - c.low, 1e-15);
    s.check("binomial: and the pooled z", g.z - c.statistic, 1e-15);
    s.check("binomial: one-sided p from the normal", g.p - tyche::probability::Normal{}.survival(c.statistic), 1e-15);
    const auto nt = statement(d, "need care for age >= 75 by normative unassisted above 0.2");
    const Gap gn = gap(d, nt, parts, Part::discovery, Inference::binomial);
    const auto cn = tyche::testing::share_against_level(26, 90, 0.2);
    s.check("binomial: the score test of a share against its level", gn.z - cn.statistic, 1e-14);
  }

  // ---- a larger synthetic population: many clusters, a planted gap ----
  language::Dataset big;
  {
    calx::random::Stream r(5, 1, 0, 0);
    std::vector<std::string> cl, ten, all;
    std::vector<double> age, k, n, inc, visits, v;
    for (int c = 0; c < 400; c++) {
      const double area = 0.02 * r.normal();
      for (int a : {40, 70, 85})
        for (const char* t : {"owner", "renting"}) {
          const double nn = 20 + static_cast<double>(r.below(40));
          double p = std::clamp(0.10 + area + ((a >= 75 && std::string(t) == "renting") ? 0.12 : 0.0), 0.01, 0.9);
          cl.push_back("c" + std::to_string(c)), ten.push_back(t), all.push_back("yes"), age.push_back(a), n.push_back(nn);
          k.push_back(static_cast<double>(r.binomial(static_cast<std::int64_t>(nn), p)));
          const double z = r.normal(), prov = 1 + 0.8 * z + 0.5 * r.normal();
          inc.push_back(z), visits.push_back(prov), v.push_back(0.3 - 0.05 * prov + 0.04 * z + 0.05 * r.normal());
        }
    }
    big.table.add(groups::categories("area", cl));
    big.table.add(groups::categories("tenure", ten));
    big.table.add(groups::categories("all", all));
    big.table.add(groups::numbers("age", age));
    big.table.add(groups::numbers("k", k));
    big.table.add(groups::numbers("n", n));
    big.table.add(groups::numbers("income", inc));
    big.table.add(groups::numbers("visits", visits));
    big.table.add(groups::numbers("v", v));
    big.cluster = "area";
    big.attributes = {"age", "tenure", "all"};
    big.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "", "", "care", {}};
    big.indicators["v"] = Indicator{"v", false, "v", "", true, "", "", "care", {}};
  }
  const Parts bp = split(big);
  {
    const double f0 = static_cast<double>(bp.members[0].size()) / 400, f1 = static_cast<double>(bp.members[1].size()) / 400;
    s.check("parts: a keyed split of 400 clusters gives about half to discovery", f0 - 0.5, 0.07);
    s.check("parts: and about a quarter to validation", f1 - 0.25, 0.06);
    const Parts again = split(big);
    s.require("parts: the same key, the same split", again.part_of_cluster == bp.part_of_cluster);
    language::Dataset other = big;
    other.table.columns[4].numbers.assign(other.table.rows, 0.0);  // the data changed, the split did not
    s.require("parts: the split does not depend on the data", split(other).part_of_cluster == bp.part_of_cluster);
    s.require("parts: another key, another split", split(big, {0.5, 0.25, 0.25}, 99).part_of_cluster != bp.part_of_cluster);
    bool whole = true;
    for (std::size_t i = 0; i < big.table.rows; i++) whole = whole && bp.part_of_row[i] == bp.part_of_cluster[static_cast<std::size_t>(bp.cluster_of_row[i])];
    s.require("parts: every row lies in its cluster's part", whole);
    language::Dataset two = d;
    two.table.columns[1].codes[1] = two.table.columns[1].level("validation");
    try {
      split(two);
      s.require("parts: a cluster in two parts is refused", false);
    } catch (const language::Refusal& e) {
      s.require("parts: a cluster in two parts is refused", true, e.what());
    }
  }
  {
    const auto planted = statement(big, "need care for age >= 75 and tenure = renting by comparative unassisted higher");
    const auto none = statement(big, "need care for tenure = owner by comparative unassisted higher");
    const auto Tp = totals(big, planted, bp, Part::discovery), Tn = totals(big, none, bp, Part::discovery);
    const auto sp = stability(Tp, 2.0, 200, 7), sn = stability(Tn, 2.0, 200, 7);
    s.require("stability: the planted gap clears z = 2 in every resample", sp.share == 1.0, std::to_string(sp.share));
    s.require("stability: a group with no gap of its own seldom does", sn.share < 0.1, std::to_string(sn.share));
    s.require("stability: the same key gives the same share", stability(Tp, 2.0, 200, 7).share == sp.share && stability(Tn, 2.0, 200, 7).share == sn.share);
    const auto g = estimate(Tp);
    s.check("the planted gap of 0.12 recovered, in its standard errors", (g.estimate - 0.12) / g.se, 3.0);
    s.require("and it is large: z > 8", g.z > 8, std::to_string(g.z));
    const auto k = stratum_gaps(Tp, strata(bp, Part::discovery, 5, 20261118), 5);
    bool positive = k.size() == 5;
    for (double z : k) positive = positive && z > 2;
    s.require("strata: the planted gap clears z = 2 in each of five strata", positive);
  }
  // ---- causal support ----
  {
    big.graph = {{"income", "visits"}, {"income", "v"}, {"visits", "v"}};
    big.provision["care"] = "visits";
    const auto st = statement(big, "need care for all = yes by comparative v higher");
    const auto c = causal(big, st, bp, Part::discovery);
    s.require("causal: the adjustment set is the confounder the graph names", c.adjustment == std::vector<std::string>{"income"});
    s.check("causal: the effect of provision recovered, in standard errors from −0.05", (c.effect + 0.05) / c.se, 3.0);
    s.require("causal: and it holds (less need where more is provided)", c.verdict == Verdict::holds, c.why);
    big.graph = {{"u", "visits"}, {"u", "v"}, {"visits", "v"}};
    const auto un = causal(big, st, bp, Part::discovery);
    s.require("causal: a confounder not observed leaves the effect not identified", un.verdict == Verdict::undecided && un.why.find("not identified") != std::string::npos, un.why);
    big.provision.clear();
    s.require("causal: a domain with no provision named is not applicable", !causal(big, st, bp, Part::discovery).applicable);
  }
  // ---- simulation: one supplier, three places ----
  {
    language::Dataset m;
    m.table.add(groups::categories("place", {"A", "B", "C"}));
    m.table.add(groups::categories("far", {"no", "no", "yes"}));
    m.table.add(groups::numbers("k", {6, 6, 5}));
    m.table.add(groups::numbers("n", {10, 10, 10}));
    m.attributes = {"far"};
    m.place = "place", m.reach = 1000;
    m.where = {{"A", {0, 0}}, {"B", {500, 0}}, {"C", {5000, 0}}};
    m.suppliers = {{"s", 0, 0, 10, ""}};
    m.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "", "", "care", {}};
    const Parts mp = split(m, {1, 0, 0});
    const auto f = simulate(m, "care", "unassisted", SeekingOptions{40, 1700, 0, 3});
    s.require("simulation: applicable, three places, one supplier", f.applicable && f.places.size() == 3 && f.suppliers == 1);
    s.check("floating catchment: A and B share 10 among 12: access 10/12", f.access[0] - 10.0 / 12, 1e-12);
    s.check("floating catchment: C reaches nothing", f.access[2], 0);
    double ab = 0;
    for (const auto& rep : f.unmet) ab += (rep[0] * 6 + rep[1] * 6) / 12 / static_cast<double>(f.unmet.size());
    s.check("seeking: A and B together leave 2 of 12 unmet in every replication", ab - 2.0 / 12, 1e-9);
    bool c_all = true;
    for (const auto& rep : f.unmet) c_all = c_all && std::fabs(rep[2] - 1.0) < 1e-9;
    s.require("seeking: C's demand is wholly unmet", c_all);
    const auto st = statement(m, "need care for far = yes by comparative unassisted higher");
    const auto j = judge(f, m, st, mp, Part::discovery);
    s.require("simulation: the far group is left more unmet: it holds", j.verdict == Verdict::holds && j.without_group == 1.0 && j.without_reference == 0.0, j.why);
    s.check("simulation: unmet shares 1 and 1/6", j.difference - (1 - 2.0 / 12), 1e-9);
  }
  // ---- rules ----
  {
    language::Dataset r = big;
    r.rules = {{"need care for age >= 75 by comparative unassisted higher", "an illustrative scheme"}};
    r.implies = {{"care", "transport"}};
    r.domains = {};
    const auto book = rulebook(r);
    s.require("rules: one rule read, two domains", book.rules.size() == 1 && book.domains.size() == 2 && book.refused.empty());
    auto st = read_one("need transport for age >= 85 by comparative unassisted higher");
    language::bind(r, st);
    const auto e = entitlement(book, r, st, bp, Part::discovery);
    s.require("rules: everyone aged 85 is entitled to transport, through care", e.verdict == Verdict::holds && e.share == 1.0);
    s.require("rules: with its derivation", !e.derivations.empty() && e.derivations[0] == "for age >= 75 (an illustrative scheme) ⇒ care ⇒ transport", e.derivations.empty() ? "" : e.derivations[0]);
    auto young = read_one("need transport for age < 50 by comparative unassisted higher");
    language::bind(r, young);
    s.require("rules: a group no rule entitles fails", entitlement(book, r, young, bp, Part::discovery).verdict == Verdict::fails);
    auto housing = read_one("need housing for age >= 85 by comparative unassisted higher");
    s.require("rules: a domain no rule reaches is undecided", !entitlement(book, r, housing, bp, Part::discovery).applicable);
  }
  // ---- interpretation (ADR 0126) ----
  {
    // A second measure of the same need, on which the planted gap is absent: the planted rows' successes scaled to the base rate.
    language::Dataset w = big;
    const auto& kk = w.table.at("k").numbers;
    const auto& ag = w.table.at("age").numbers;
    const auto& tn = w.table.at("tenure");
    std::vector<double> flat(kk.size());
    for (std::size_t i = 0; i < kk.size(); i++) flat[i] = ag[i] >= 75 && tn.levels[static_cast<std::size_t>(tn.codes[i])] == "renting" ? std::round(kk[i] * 10.0 / 22.0) : kk[i];
    w.table.add(groups::numbers("k_flat", flat));
    w.indicators["unassisted_flat"] = Indicator{"unassisted_flat", true, "k_flat", "n", true, "", "", "care", {}};
    w.attributes = {"age", "tenure", "all"};
    const Parts wp = split(w);
    InterpretOptions io;
    io.z = 3;
    const auto held = [&](const Construed& c) {  // every interpretation's verdict is the gap test, computed again directly
      bool same = c.applies && c.statements.size() == c.construal.holds.size();
      for (std::size_t x = 0; x < c.statements.size() && same; x++) {
        auto st = read_one(c.statements[x]);
        language::bind(w, st);
        const Gap g = gap(w, st, wp, Part::discovery);
        same = (c.construal.holds[x] != 0) == (g.defined && g.z >= 3 && g.estimate >= 0);
      }
      return same;
    };
    // The reference group.
    w.points = {{"reference", "whom the group is compared with", language::PointKind::reference, {},
                 {{"the rest", "analyst", 'A', "", "the frame less the group"}, {"everyone", "analyst", 'A', "", "the whole frame"}, {"age >= 75 and tenure = renting", "analyst", 'A', "", "older renters"}}}};
    check_declarations(w);
    const auto planted = statement(w, "need care for age >= 75 and tenure = renting by comparative unassisted higher");
    const auto c1 = interpret(w, planted, wp, Part::discovery, io);
    s.require("interpretation: three references; against older renters themselves the gap is nil", c1.applies && c1.statements.size() == 3 && !c1.construal.holds[2]);
    s.require("interpretation: each interpretation's verdict is the gap test computed directly", held(c1));
    s.require("interpretation: against the rest and everyone it holds: arguable, the reference pivotal", c1.construal.holds[0] && c1.construal.holds[1] && c1.construal.record == apodeixis::interpretation::Class::arguable &&
                                                                                                                c1.construal.pivots[0].flips == std::vector<std::size_t>{2});
    const auto older = statement(w, "need care for age >= 75 by comparative unassisted higher than age < 75");
    const auto c2 = interpret(w, older, wp, Part::discovery, io);
    s.require("interpretation: a reference the points do not hold is added as the statement's, stated", c2.statements.size() == 4 && c2.construal.stated && *c2.construal.stated == 3 &&
                                                                                                        c2.construal.declared[0].readings[3].by == "the statement");
    s.require("interpretation: and again each verdict is the gap test", held(c2));
    // The measure: an indicator point.
    w.points.push_back({"measure", "how the need is measured", language::PointKind::indicator, {}, {{"unassisted", "analyst", 'A', "", ""}, {"unassisted_flat", "analyst", 'A', "", ""}}});
    check_declarations(w);
    const auto c3 = interpret(w, planted, wp, Part::discovery, io);
    s.require("interpretation: two points, six interpretations, each held to the gap test", c3.statements.size() == 6 && held(c3));
    s.require("interpretation: on the flat measure it holds under no reference: the measure is pivotal", !c3.construal.holds[3] && !c3.construal.holds[4] && !c3.construal.holds[5] &&
                                                                                                            c3.construal.pivots[1].flips == std::vector<std::size_t>{1});
    bool only_measure = false;
    for (const auto& p : c3.construal.label) only_measure = only_measure || (p[0] >= 0 && p[1] == 0);
    s.require("interpretation: its label fixes the measure as unassisted", only_measure);
    // The level of a normative standard, and the order of sources.
    language::Dataset lv = w;
    lv.points = {{"level", "the share above which the need is unmet", language::PointKind::level, {"unassisted"}, {{"10%", "analyst", 'A', "", ""}, {"20%", "analyst", 'A', "", ""}, {"0.3", "analyst", 'A', "", ""}}}};
    check_declarations(lv);
    const auto norm = statement(lv, "need care for age >= 75 and tenure = renting by normative unassisted above 15%");
    const auto c4 = interpret(lv, norm, wp, Part::discovery, io);
    bool levels = c4.statements.size() == 4;
    for (std::size_t x = 0; x < c4.statements.size() && levels; x++) {
      auto st = read_one(c4.statements[x]);
      language::bind(lv, st);
      const Gap g = gap(lv, st, wp, Part::discovery);
      levels = (c4.construal.holds[x] != 0) == (g.defined && g.z >= 3);
    }
    s.require("interpretation: four levels (15% as stated, added), each the gap test against its level", levels && c4.construal.holds[0] && !c4.construal.holds[2]);
    lv.points[0].readings[1].source = "convention";
    const auto c5 = interpret(lv, norm, wp, Part::discovery, io);
    std::size_t admissible = 0;
    for (char a : c5.construal.admissible) admissible += a != 0;
    s.require("interpretation: a convention's level sets aside the analyst's three, the statement's own among them", admissible == 1 && c5.construal.admissible[1] && c5.construal.set_aside.size() == 3);
    // A proposal.
    language::Dataset pr = w;
    pr.points.resize(1);
    pr.points[0].readings.push_back({"tenure = owner", "proposal", 'C', "a model", "compare with owners"});
    check_declarations(pr);
    const auto c6 = interpret(pr, planted, wp, Part::discovery, io);
    std::size_t rec = 0, prop = 0;
    for (std::size_t x = 0; x < c6.construal.admissible.size(); x++) rec += c6.construal.admissible[x] != 0, prop += c6.construal.admissible_with_proposals[x] != 0;
    s.require("interpretation: a model's reference is not the record's, and widens what is shown as proposed", rec == 3 && prop == 4 && !c6.construal.admissible[3]);
    // Refusals of declarations.
    const auto refused = [&](language::Dataset x) {
      try { check_declarations(x); } catch (const language::Refusal&) { return true; }
      return false;
    };
    language::Dataset bad = w;
    bad.points.push_back({"again", "", language::PointKind::reference, {}, {{"everyone", "analyst", 'A', "", ""}}});
    s.require("declarations: two reference points over the same statements are refused", refused(bad));
    bad = w, bad.points[0].readings[0].grade = 'C';
    s.require("declarations: a reading of grade C not proposed is refused", refused(bad));
    bad = w, bad.points[0].readings[1].text = "age >>= 3";
    s.require("declarations: a reference that is not a group is refused", refused(bad));
    bad = lv, bad.points[0].readings[0].text = "ten";
    s.require("declarations: a level that is not a number is refused", refused(bad));
    bad = w, bad.indicators["satisfied"] = Indicator{"satisfied", true, "k", "n", false, "", "", "care", {}};
    bad.points[1].readings.push_back({"satisfied", "analyst", 'A', "", ""});
    s.require("declarations: an indicator point mixing directions is refused", refused(bad));
    bad = w, bad.points[0].readings[0].source = "rumour";
    s.require("declarations: a source not named is refused", refused(bad));
  }
  // ---- causal support under each graph ----
  {
    language::Dataset g = big;
    g.graph = {{"income", "visits"}, {"income", "v"}, {"visits", "v"}};
    g.provision["care"] = "visits";
    g.graphs = {{"unobserved", "analyst", 'A', "", "a confounder unmeasured", {{"u", "visits"}, {"u", "v"}, {"visits", "v"}}}};
    const auto st = statement(g, "need care for all = yes by comparative v higher");
    const auto c = interpret_causal(g, st, bp, Part::discovery);
    s.require("graphs: under the declared graph it holds; under one with an unmeasured confounder it is not identified", c.applies && c.construal.holds[0] && !c.construal.holds[1] &&
                                                                                                                         c.results[1].why.find("not identified") != std::string::npos);
    s.require("graphs: so causal support is arguable over the graphs", c.construal.record == apodeixis::interpretation::Class::arguable);
    g.graph_source = "convention";
    s.require("graphs: with the declared graph a convention, the analyst's alternative is set aside: clear", interpret_causal(g, st, bp, Part::discovery).construal.record == apodeixis::interpretation::Class::clear);
    g.graphs[0].source = "proposal", g.graphs[0].grade = 'C', g.graph_source = "analyst";
    const auto p = interpret_causal(g, st, bp, Part::discovery);
    s.require("graphs: a model's graph is shown as proposed, not as the record's", p.construal.record == apodeixis::interpretation::Class::clear && p.construal.with_proposals == apodeixis::interpretation::Class::arguable);
  }
  // ---- rivals ----
  {
    // People in homes need care more; the old live in homes more. Within homes and outside them, age makes no difference.
    language::Dataset h;
    calx::random::Stream r(11, 2, 0, 0);
    std::vector<std::string> cl, home;
    std::vector<double> age, k, n;
    for (int c = 0; c < 300; c++) {
      const double area = 0.01 * r.normal();
      for (int a : {70, 90})
        for (const char* hm : {"yes", "no"}) {
          const bool in = std::string(hm) == "yes";
          const double nn = a == 90 ? (in ? 30 : 10) : (in ? 5 : 40);
          cl.push_back("c" + std::to_string(c)), home.push_back(hm), age.push_back(a), n.push_back(nn);
          k.push_back(static_cast<double>(r.binomial(static_cast<std::int64_t>(nn), std::clamp((in ? 0.4 : 0.1) + area, 0.01, 0.9))));
        }
    }
    h.table.add(groups::categories("area", cl));
    h.table.add(groups::categories("home", home));
    h.table.add(groups::numbers("age", age));
    h.table.add(groups::numbers("k", k));
    h.table.add(groups::numbers("n", n));
    h.cluster = "area", h.attributes = {"age", "home"};
    h.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "", "", "care", {}};
    h.rivals = {{"living in a home", "", "analyst", 'A', "", strata_of(h, "home"), {}}};
    check_declarations(h);
    const Parts hp = split(h);
    const auto st = statement(h, "need care for age >= 85 by comparative unassisted higher");
    InterpretOptions io;
    const auto a = rival(h, st, h.rivals[0], hp, Part::discovery, io);
    const Gap crude = gap(h, st, hp, Part::discovery);
    s.require("rivals: the old have a large crude gap", crude.z > 8, std::to_string(crude.z));
    s.require("rivals: within homes and outside them it is gone: living in a home explains it away", a.explains == tekmerion::construal::Explains::away, a.reason);
    double num = 0, den = 0;
    for (const auto& g : h.rivals[0].strata) {
      auto t = st;
      t.frame = groups::join(st.frame, g);
      const Gap x = gap(h, t, hp, Part::discovery);
      num += x.estimate / (x.se * x.se), den += 1 / (x.se * x.se);
    }
    s.check("rivals: the pooled effect is the strata's gaps by inverse variance", a.pooled.estimate - num / den, 1e-12);
    const auto num_strata = strata_of(h, "age", {80});
    s.require("rivals: a number's strata from cut points", num_strata.size() == 2 && num_strata[0].write() == "age < 80" && num_strata[1].write() == "age >= 80");
    language::Dataset one = h;
    one.rivals[0].strata.resize(1);
    bool refused = false;
    try { check_declarations(one); } catch (const language::Refusal&) { refused = true; }
    s.require("rivals: a rival with one stratum is refused", refused);
  }

  // ---- standing ----
  {
    Findings u;
    u.discovery = true, u.confirmation_tested = true, u.confirmed = true, u.untenable = true;
    const auto ju = standing(u);
    s.require("standing: untenable under its readings, though confirmed: unsupported, U in", ju.standing == Standing::unsupported && ju.arguments.back().id == "U" && ju.arguments.back().label == "in");
    Findings x;
    x.discovery = true, x.confirmation_tested = true, x.confirmed = true, x.explained_by = {"living in a home"};
    const auto jx = standing(x);
    s.require("standing: explained away by a rival, though confirmed: unsupported, R1 in", jx.standing == Standing::unsupported && jx.arguments.back().id == "R1" && jx.arguments.back().label == "in");
  }
  {
    auto judge_of = [](bool disc, bool val, bool stable, bool tested, bool conf, bool opp) {
      Findings f;
      f.discovery = disc, f.validation_tested = true, f.validation_upheld = val, f.stability_tested = true, f.stable = stable;
      f.confirmation_tested = tested, f.confirmed = conf, f.opposite = opp;
      return standing(f).standing;
    };
    s.require("standing: everything holds and the confirmation confirms: robust", judge_of(true, true, true, true, true, false) == Standing::robust);
    s.require("standing: not yet confirmed: supported", judge_of(true, true, true, false, false, false) == Standing::supported);
    s.require("standing: tested and not confirmed: supported", judge_of(true, true, true, true, false, false) == Standing::supported);
    s.require("standing: unstable but confirmed: the fresh data reinstate it: robust", judge_of(true, true, false, true, true, false) == Standing::robust);
    s.require("standing: unstable and not confirmed: unsupported", judge_of(true, true, false, true, false, false) == Standing::unsupported);
    s.require("standing: the validation refuses and nothing confirms: unsupported", judge_of(true, false, true, false, false, false) == Standing::unsupported);
    s.require("standing: the opposite gap on confirmation: refuted", judge_of(true, true, true, true, false, true) == Standing::refuted);
    s.require("standing: no gap on discovery, even confirmed: unsupported", judge_of(false, true, true, true, true, false) == Standing::unsupported);
    Findings f;
    f.discovery = true, f.stability_tested = true, f.stable = false, f.confirmation_tested = true, f.confirmed = true;
    const auto j = standing(f);
    s.require("standing: its arguments labelled (N in, S out, C in)", j.arguments.size() == 3 && j.arguments[0].label == "in" && j.arguments[1].label == "out" && j.arguments[2].label == "in");
  }
  return s.done();
}
