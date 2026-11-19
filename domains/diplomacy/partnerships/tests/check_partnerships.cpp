// Checks of a partnership's record (partnerships.hpp).
//
// - The record's faults, each named.
// - The SMART screen: each criterion answered with its reason; "achievable" undecided until someone is recorded as judging it.
// - A commitment's state at a date (honoured, honoured late, in progress, overdue, unreported), its target and date as amended, its
//   holder as transferred, and its progress from baseline to target.
// - The ledger: two vectors compared dimension by dimension as their scales permit (dominance, crossing, nominal difference,
//   overlapping evidence, a value not observed, a dimension where lower is better); the undominated alternatives.
// - Aggregation refused where its scale forbids it, the refusal agreeing with kybernesis's test of meaningfulness, and carried
//   exactly through intervals where it is allowed.
// - Benefit party by party as a dual verdict: clear, arguable, untenable and undecided each produced; the measure's Fréchet bounds
//   held to three joint laws of two criteria with the same margins (comonotone, independent, countermonotone), which reach both
//   ends; mutual benefit failing at the party whose criterion fails.
// - The modes of failure, each raised on a record built to raise it and none on a record built to raise none.
//
// Every party, goal and number here is invented.

#include <diplomacy/partnerships/partnerships.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace diplomacy::partnerships;
namespace kk = kybernesis::kernel;

namespace {

// An invented partnership for clean water in a river basin: two cities, a foundation and a university.
Partnership basin() {
  Partnership p;
  p.name = "an invented partnership for clean water in a river basin";
  p.registered = "2024-01-15";
  p.members = {{"upstream", "city", "2024-01-15", ""}, {"downstream", "city", "2024-01-15", ""},
               {"foundation", "funder", "2024-01-15", ""}, {"university", "monitor", "2024-01-15", ""}};
  p.goals = {{"g1", "the river meets the bathing standard downstream", {"upstream", "downstream"}},
             {"g2", "every household within 1 km of the river connected to treatment", {"downstream", "foundation"}}};
  Commitment c1;
  c1.id = "c1", c1.by = "upstream", c1.text = "build a treatment plant for 40,000 people", c1.goal = "g1", c1.made = "2024-02-01", c1.due = "2025-12-31";
  c1.roles = {{"upstream", "builds and runs the plant"}, {"foundation", "pays half"}, {"university", "measures the river"}};
  c1.indicator = {"people served", "people", 0, 40000, true};
  c1.resources = {{"foundation", "grant", "EUR", 6e6, true}, {"upstream", "budget", "EUR", 6e6, false}};
  Commitment c2;
  c2.id = "c2", c2.by = "downstream", c2.text = "connect the riverside households", c2.goal = "g2", c2.made = "2024-02-01", c2.due = "2025-06-30";
  c2.roles = {{"downstream", "connects the households"}};
  c2.indicator = {"households connected", "households", 1200, 5000, true};
  c2.resources = {{"downstream", "budget", "EUR", 2e6, true}};
  Commitment c3;
  c3.id = "c3", c3.by = "university", c3.text = "publish the river's quality every month", c3.goal = "g1", c3.made = "2024-02-01", c3.due = "2026-12-31";
  c3.roles = {{"university", "samples and publishes"}};
  c3.indicator = {"months published", "months", 0, 36, true};
  c3.resources = {{"foundation", "grant", "EUR", 3e5, true}};
  p.commitments = {c1, c2, c3};
  p.reports = {{"c1", "2024-12-31", "upstream", "annual report", 10000}, {"c1", "2025-11-30", "upstream", "annual report", 41000},
               {"c2", "2024-12-31", "downstream", "annual report", 2500}, {"c2", "2025-09-30", "downstream", "annual report", 5100},
               {"c3", "2025-06-30", "university", "its site", 17}};
  p.contributions = {{"foundation", "c1", "", "grant", "EUR", "2024-03-01", "the grant letter", 6e6},
                     {"upstream", "c1", "", "budget", "EUR", "2024-04-01", "the city's accounts", 5e6},
                     {"downstream", "c2", "", "budget", "EUR", "2024-04-01", "the city's accounts", 2e6},
                     {"foundation", "c3", "", "grant", "EUR", "2024-03-01", "the grant letter", 3e5}};
  p.dimensions = {{"water", "mission", "days a year fit for bathing", Scale::ratio, true, {}},
                  {"cost", "cost", "EUR million", Scale::ratio, false, {}},
                  {"standing", "indirect", "", Scale::ordinal, true, {"diminished", "unchanged", "enhanced"}},
                  {"lead", "indirect", "", Scale::nominal, true, {}}};
  p.values = {{"downstream", "water", "2025-12-31", "days fit for bathing at the beach gauge", "the university's series", 70, 60, 80, 5},
              {"downstream", "cost", "2025-12-31", "its budget lines", "the city's accounts", 2, 2, 2},
              {"upstream", "water", "2025-12-31", "days fit for bathing at the beach gauge", "the university's series", 70, 60, 80, 5},
              {"upstream", "standing", "2025-12-31", "the regional press", "a media review", 2},
              {"foundation", "standing", "2025-12-31", "its board's own reading", "the board's minutes", 2}};
  p.interests = {{"downstream", "water", "days fit for bathing", "its council's resolution", 50},
                 {"downstream", "cost", "a budget it can carry", "its council's resolution", 3},
                 {"upstream", "water", "days fit for bathing", "its mayor's statement", 75},
                 {"foundation", "standing", "its reputation", "its strategy", 2}};
  return p;
}

bool has(const std::vector<Sign>& ss, const std::string& mode, const std::string& where_part, const std::string& what_part = "") {
  for (const auto& s : ss)
    if (s.mode == mode && s.where.find(where_part) != std::string::npos && s.what.find(what_part) != std::string::npos) return true;
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Partnership p = basin();

  // ---- The record --------------------------------------------------------------------------------------------------------------
  s.require("the invented partnership is recorded without fault", problems(p).empty(), problems(p).empty() ? "" : problems(p)[0].where + ": " + problems(p)[0].what);
  {
    Partnership bad = p;
    bad.goals.push_back({"g3", "a goal set by an outsider", {"outsider"}});
    bad.commitments.push_back(bad.commitments[0]);
    bad.reports.push_back({"c9", "2025-01-01", "", "", 1});
    bad.contributions.push_back({"foundation", "", "", "grant", "EUR", "2025-01-01", "", -5});
    bad.values.push_back({"upstream", "beauty", "", "", "", 1});
    bad.interests.push_back({"downstream", "water", "", "", unknown});
    const auto ps = problems(bad);
    s.require("its faults are found, each named: a goal set by no member, a commitment declared twice, a report of no commitment, a negative contribution, a value on no dimension, an interest with no threshold",
              ps.size() == 6, std::to_string(ps.size()) + (ps.empty() ? "" : ": " + ps[0].where + ", " + ps[0].what));
  }

  // ---- The SMART screen ---------------------------------------------------------------------------------------------------------
  {
    const Screen a = screen(p.commitments[0]);
    s.require("c1 is specific, measurable, resource-based (the foundation's grant is secured) and time-bound", a.checks[0].verdict.holds() && a.checks[1].verdict.holds() && a.checks[3].verdict.holds() && a.checks[4].verdict.holds());
    s.require("whether it is achievable no one has judged: undecided, and so is the screen", a.checks[2].verdict.undecided() && a.verdict.undecided(), a.verdict.reason);
    Commitment judged = p.commitments[0];
    judged.achievable_by = "the basin authority's engineer", judged.achievable_reason = "a plant of this size was built in two years nearby", judged.achievable = true;
    s.require("judged achievable, with a reason, the screen holds", screen(judged).verdict.holds());
    Commitment vague = p.commitments[0];
    vague.indicator.target = unknown, vague.resources = {{"upstream", "budget", "EUR", 1e6, false}}, vague.roles["foundation"] = "";
    const Screen v = screen(vague);
    s.require("with no target, only proposed resources and a partner without a role, it fails three criteria, each with its reason",
              v.checks[0].verdict.fails() && v.checks[1].verdict.fails() && v.checks[3].verdict.fails() && v.verdict.fails() &&
                  v.checks[0].verdict.reason.find("foundation has no defined role") != std::string::npos && v.checks[3].verdict.reason.find("none secured") != std::string::npos,
              v.checks[3].verdict.reason);
    std::vector<Criterion> own = {{"owned", "a holder named", [](const Commitment& c) { return c.by.empty() ? Verdict::no("owned", "no holder") : Verdict::yes("owned", c.by); }}};
    s.require("a caller's own scheme is applied in its place", screen(vague, own).verdict.holds() && screen(vague, own).checks.size() == 1);
  }

  // ---- The state of a commitment --------------------------------------------------------------------------------------------------
  {
    const auto c1 = state(p, p.commitments[0], "2026-01-31");
    s.require("c1 reached 41,000 people on 30 November 2025, before its date: honoured", c1.state == State::honoured && c1.reached_on == "2025-11-30");
    s.check("its progress is 41,000/40,000 of the way", std::abs(c1.progress - 41000.0 / 40000.0), 1e-15);
    const auto c1_early = state(p, p.commitments[0], "2025-06-30");
    s.require("read in mid-2025 it is in progress, a quarter of the way", c1_early.state == State::in_progress && std::abs(c1_early.progress - 0.25) < 1e-15);
    const auto c2 = state(p, p.commitments[1], "2026-01-31");
    s.require("c2 reached 5,100 households on 30 September 2025, after its date of 30 June: honoured late", c2.state == State::honoured_late);
    const auto c2_mid = state(p, p.commitments[1], "2025-07-31");
    s.require("read on 31 July 2025, past its date and short of the target, it is overdue", c2_mid.state == State::overdue);
    s.check("its progress then: (2,500 − 1,200)/(5,000 − 1,200)", std::abs(c2_mid.progress - 1300.0 / 3800.0), 1e-15);
    Partnership q = p;
    q.commitments[1].amendments.push_back({"2025-05-01", "downstream", "the contractor failed; the council agreed a later date", unknown, "2025-12-31", ""});
    s.require("with its date amended to 31 December 2025, the same reading is in progress, and the later report honours it in time",
              state(q, q.commitments[1], "2025-07-31").state == State::in_progress && state(q, q.commitments[1], "2026-01-31").state == State::honoured);
    s.require("the amendment does not reach back: read before it was made, the commitment keeps its first date",
              state(q, q.commitments[1], "2025-04-30").due == "2025-06-30");
    Partnership r = p;
    r.reports.clear();
    s.require("with no report it is unreported, not failed", state(r, r.commitments[0], "2026-01-31").state == State::unreported);
    const auto c3 = state(p, p.commitments[2], "2026-01-31");
    s.require("c3 has published 17 of 36 months and is not yet due: in progress", c3.state == State::in_progress && std::abs(c3.progress - 17.0 / 36.0) < 1e-15);
  }

  // ---- Comparison as scales permit -------------------------------------------------------------------------------------------------
  {
    const std::vector<Dimension>& D = p.dimensions;
    auto v = [](const std::string& dim, double x, double lo = unknown, double hi = unknown) {
      Value e;
      e.dimension = dim, e.value = x, e.lower = lo, e.upper = hi;
      return e;
    };
    std::map<std::string, Value> a = {{"water", v("water", 90)}, {"cost", v("cost", 2)}, {"standing", v("standing", 2)}, {"lead", v("lead", 1)}};
    std::map<std::string, Value> b = {{"water", v("water", 70)}, {"cost", v("cost", 3)}, {"standing", v("standing", 1)}, {"lead", v("lead", 1)}};
    s.require("more water, less cost, higher standing and the same lead: a dominates b", compare(D, a, b).relation == Relation::dominates && compare(D, b, a).relation == Relation::dominated);
    s.require("cost is a dimension where lower is better: 2 is better than 3", compare(D, a, b).by.at("cost") == Order::better);
    auto c = b;
    c["water"] = v("water", 95);
    const auto x = compare(D, a, c);
    s.require("more water but more cost: they cross, and no ranking follows without a declared rule (Proposition 12.1)",
              x.relation == Relation::crossing && x.why.find("no ranking without a declared rule") != std::string::npos, x.why);
    auto d = a;
    d["lead"] = v("lead", 2);
    s.require("the same in all but who leads, which is nominal: incomparable, neither better", compare(D, a, d).relation == Relation::incomparable);
    auto e = b;
    e["water"] = v("water", 85, 75, 95);
    auto f = a;
    f["water"] = v("water", 90, 80, 100);
    s.require("water 90 [80, 100] against 85 [75, 95]: what the evidence allows overlaps, so it is undetermined", compare(D, f, e).relation == Relation::undetermined && compare(D, f, e).by.at("water") == Order::overlapping);
    auto g = a;
    g.erase("standing");
    s.require("a value not observed is not lower: undetermined, never dominated", compare(D, g, b).relation == Relation::undetermined && compare(D, g, b).by.at("standing") == Order::missing);
    const auto front = undominated(D, {{"a", a}, {"b", b}, {"c", c}});
    s.require("among a, b and c (c crossing a, a dominating b), a and c are undominated", front == std::vector<std::string>{"a", "c"});
  }

  // ---- Aggregation by a declared rule ----------------------------------------------------------------------------------------------
  {
    std::map<std::string, Value> a = {{"water", {"", "water", "", "", "", 70, 60, 80}}, {"cost", {"", "cost", "", "", "", 2, 1.5, 2.5}}, {"standing", {"", "standing", "", "", "", 1}}};
    Rule r{"a day's worth", "EUR million", "the basin authority", "the authority's own valuation of a bathing day", {{"water", 0.05}, {"cost", 1.0}}};
    const auto x = aggregate(p.dimensions, a, r);
    s.require("by a declared rule on ratio dimensions: 0.05 × 70 − 2 = 1.5, carried through as [0.05 × 60 − 2.5, 0.05 × 80 − 1.5] = [0.5, 2.5]",
              !x.refused && std::abs(x.value - 1.5) < 1e-12 && std::abs(x.lower - 0.5) < 1e-12 && std::abs(x.upper - 2.5) < 1e-12, x.why);
    Rule ord = r;
    ord.weights["standing"] = 0.5;
    const auto y = aggregate(p.dimensions, a, ord);
    s.require("weighing an ordinal dimension is refused, with the reason", y.refused && y.why.find("ordinal") != std::string::npos, y.why);
    // The refusal is the meaningfulness of the comparison it would make: on ordinal levels A = (1, 4) and B = (3, 3) with equal
    // weights, "A's sum exceeds B's" is false, and turns true under the admissible x -> x³.
    const auto m = kk::meaningful(Scale::ordinal, {1, 4, 3, 3}, [](const std::vector<double>& z) { return z[0] + z[1] > z[2] + z[3]; }, "a sum of ordinal levels compared");
    s.require("the refusal agrees with kybernesis: a comparison of sums of ordinal levels is not meaningful", m.fails(), m.reason);
    Rule anon = r;
    anon.declared_by.clear();
    s.require("a rule declared by no one is refused", aggregate(p.dimensions, a, anon).refused);
    auto gap = a;
    gap.erase("cost");
    s.require("a dimension not observed is refused, not taken for zero", aggregate(p.dimensions, gap, r).refused && aggregate(p.dimensions, gap, r).why.find("not zero") != std::string::npos);
  }

  // ---- Benefit, party by party -------------------------------------------------------------------------------------------------------
  {
    const auto down = benefit(p, "downstream");
    s.require("downstream asks 50 days and a cost of at most 3: the evidence allows [60, 80] days and a cost of 2, so its benefit is clear",
              down.standing == Standing::clear && down.value.met.size() == 2);
    s.require("and certain: probability 1", down.measure.kind == Measure::Kind::probability && down.measure.value == 1);
    const auto up = benefit(p, "upstream");
    s.require("upstream asks 75 days: [60, 80] straddles it, so its benefit is arguable", up.standing == Standing::arguable && up.value.open == std::vector<std::string>{"water"});
    const double pu = 1 - tyche::probability::Normal{70, 5}.cdf(75);
    s.check("its measure, from the estimate 70 with error 5: P(days ≥ 75) = 1 − Φ(1)", std::abs(up.measure.value - pu), 1e-15);
    s.require("a measure from an estimate is statistical", up.provenance.family == kk::Family::statistical);
    const auto found = benefit(p, "foundation");
    s.require("the foundation asks its standing kept: an ordinal level 2 against 2, clear", found.standing == Standing::clear);
    const auto uni = benefit(p, "university");
    s.require("the university declared no criterion: undecided", uni.standing == Standing::undecided);
    Partnership q = p;
    q.interests.push_back({"upstream", "cost", "its share of the cost", "its mayor's statement", 4});
    s.require("upstream's cost is not observed: its benefit is undecided, not untenable", benefit(q, "upstream").standing == Standing::undecided && benefit(q, "upstream").value.unobserved.size() == 1);
    Partnership r = p;
    r.interests[2].threshold = 85;
    s.require("asking 85 days, above all the evidence allows, upstream's benefit is untenable", benefit(r, "upstream").standing == Standing::untenable);

    // Fréchet's bounds on two criteria with margins 0.9 and 0.8, held to three joint laws.
    Partnership t = p;
    t.values = {{"downstream", "water", "2025-12-31", "", "", 0, -10, 10, 1}, {"downstream", "cost", "2025-12-31", "", "", 0, -10, 10, 1}};
    // thresholds chosen so that P(water >= t1) = 0.9 and P(cost <= t2) = 0.8 under N(0, 1)
    auto quantile = [](double q) {
      double lo = -10, hi = 10;
      for (int i = 0; i < 200; i++) {
        const double mid = (lo + hi) / 2;
        (tyche::probability::Normal{}.cdf(mid) < q ? lo : hi) = mid;
      }
      return (lo + hi) / 2;
    };
    t.interests = {{"downstream", "water", "", "", quantile(0.1)}, {"downstream", "cost", "", "", quantile(0.8)}};
    const auto bf = benefit(t, "downstream");
    s.require("two criteria met with probabilities 0.9 and 0.8: the joint probability lies in [0.7, 0.8] whatever their dependence",
              bf.measure.kind == Measure::Kind::interval && std::abs(bf.measure.lower - 0.7) < 1e-9 && std::abs(bf.measure.upper - 0.8) < 1e-9,
              kk::words(bf.measure));
    calx::random::Stream rng(20261119, 132, 1, 0);
    const std::size_t n = 400000;
    std::size_t co = 0, ind = 0, counter = 0;
    for (std::size_t i = 0; i < n; i++) {
      const double u = rng.uniform(), w = rng.uniform();
      co += (u >= 0.1) && (u <= 0.8);           // one uniform for both (comonotone): water met above its 0.1 quantile, cost met below its 0.8
      ind += (u >= 0.1) && (w <= 0.8);          // independent
      counter += (u >= 0.1) && ((1 - u) <= 0.8);  // countermonotone
    }
    const double pc = static_cast<double>(co) / n, pi = static_cast<double>(ind) / n, pa = static_cast<double>(counter) / n;
    s.require("held to joint laws with those margins: comonotone reaches the lower end 0.7, countermonotone the upper 0.8, independence 0.72 lies between",
              std::abs(pc - 0.7) < 0.004 && std::abs(pa - 0.8) < 0.004 && std::abs(pi - 0.72) < 0.004 && pc <= pi && pi <= pa,
              kk::number(pc) + " " + kk::number(pi) + " " + kk::number(pa));
  }

  // ---- Mutual benefit -----------------------------------------------------------------------------------------------------------
  {
    kk::Standard st{kk::Standard::Kind::standing, 0.5, Standing::clear};
    const auto m = mutual(p, st);
    s.require("by a standard of standing (clear): downstream and the foundation benefit, upstream's is only arguable, the university has no criterion",
              m.checks.size() == 4 && m.checks[0].verdict.fails() && m.checks[1].verdict.holds() && m.checks[2].verdict.holds() && m.checks[3].verdict.undecided());
    s.require("so mutual benefit fails, at upstream, and is never one number", m.verdict.fails() && m.verdict.reason.find("upstream") != std::string::npos, m.verdict.reason);
    kk::Standard loose{kk::Standard::Kind::both, 0.1, Standing::arguable};
    const auto l = mutual(p, loose);
    s.require("by a standard of both (at least arguable, probability above 0.1) upstream holds, and the university's silence leaves it undecided",
              l.checks[0].verdict.holds() && l.verdict.undecided(), l.verdict.reason);
  }

  // ---- Modes of failure --------------------------------------------------------------------------------------------------------------
  {
    Partnership h = p;
    h.commitments[0].resources[1].secured = true;
    h.contributions.push_back({"upstream", "c3", "", "grant", "EUR", "2024-03-01", "", 6e6});      // grants 6.3 : 6, budgets 5 : 5
    h.contributions.push_back({"downstream", "c2", "", "budget", "EUR", "2024-05-01", "", 3e6});
    h.goals[0].set_by = {"upstream", "downstream", "foundation", "university"};
    const auto ok = diagnose(h, "2025-12-31");
    s.require("a partnership built to be sound raises no sign of failure", ok.empty(), ok.empty() ? "" : ok[0].mode + ": " + ok[0].what);

    const auto all = diagnose(p, "2025-12-31");
    s.require("resource fragility: the grants in euros come from the foundation alone", has(all, "resource fragility", "grant in EUR", "foundation gives 100%"));

    Partnership paper = p;
    paper.contributions.clear(), paper.reports.clear();
    s.require("a paper partnership: registered on 15 January 2024 and nothing since, read two years on",
              has(diagnose(paper, "2026-01-20"), "paper partnership", "the partnership", "since 2024-01-15"));

    Partnership drift = p;
    for (int i = 0; i < 5; i++) drift.contributions.push_back({"foundation", "", "a festival", "grant", "EUR", "2025-05-01", "", 1e5});
    s.require("drift: 5 of 9 contributions serve no declared goal", has(diagnose(drift, "2025-12-31"), "drift", "contributions", "5 of 9"));

    Partnership cap = p;
    cap.goals = {{"g1", "", {"foundation"}}, {"g2", "", {"foundation"}}};
    s.require("capture: the foundation set both goals and the cities and university none", has(diagnose(cap, "2025-12-31"), "capture", "goals", "foundation set 2 of 2"));

    Partnership inert = p;
    inert.reports.push_back({"c3", "2026-07-31", "university", "its site", 17});
    s.require("collaborative inertia: c3 reported at 17 months from June 2025 to July 2026 without moving",
              has(diagnose(inert, "2026-08-01"), "collaborative inertia", "commitment c3", "since 2025-06-30"));

    Partnership gap = p;
    gap.reports = {{"c2", "2024-12-31", "downstream", "", 2500}};
    s.require("an accountability gap: c2 fell due on 30 June 2025, short, with nothing reported since", has(diagnose(gap, "2025-12-31"), "accountability gap", "commitment c2", "nothing reported since"));

    Partnership left = p;
    left.members[3].until = "2025-01-01";
    s.require("succession loss: the university left in January 2025 holding c3, and no one took it over", has(diagnose(left, "2025-12-31"), "succession loss", "commitment c3", "no one has taken it over"));
    left.commitments[2].amendments.push_back({"2024-12-15", "university", "it hands the monitoring to the downstream city", unknown, "", "downstream"});
    s.require("transferred to the downstream city before it left, there is no loss", !has(diagnose(left, "2025-12-31"), "succession loss", "commitment c3"));
  }
  return s.done();
}
