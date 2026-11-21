// Checks of economics/fiscal on an invented municipality, every value worked by hand or by a closed form: accounts summed through
// a chart and a line it lacks refused; two declared ratios judged against a scheme of thresholds; an annuity's payment, interest
// and principal and a bullet's; the years ahead grown at declared rates with the coverage of debt service; the largest new loan
// keeping the coverage at its floor, holding with equality at the binding year once taken, and bounded by a ceiling on the stock;
// the dynamics of debt and the balance that stabilises it; a fan of paths from a record, its quantiles ordered and a record
// without variation collapsing it onto the baseline; and capacity and effort by the representative tax system.

#include <economics/fiscal/fiscal.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace economics::fiscal;

int main() {
  calx::verification::Suite s;
  Accounts a;
  a.unit = "thousands", a.currency = "an invented currency";
  a.chart = {{"110", "property tax", "own"}, {"120", "fees", "own"}, {"210", "grant from the state", "transfers"}, {"310", "salaries", "operating"},
             {"320", "maintenance", "operating"}, {"410", "interest", "interest"}, {"420", "principal repaid", "principal"}, {"510", "works", "capital"}};
  a.years = {{2024, {{"110", 600}, {"120", 200}, {"210", 400}, {"310", 700}, {"320", 200}, {"410", 40}, {"420", 60}, {"510", 250}}}};
  const auto t = totals(a, a.years[0]);
  s.require("accounts summed through the chart: own 800, transfers 400, operating 900, interest 40, principal 60",
            t.at("own") == 800 && t.at("transfers") == 400 && t.at("operating") == 900 && t.at("interest") == 40 && t.at("principal") == 60);
  {
    Accounts b = a;
    b.years[0].amounts["999"] = 1;
    bool refused = false;
    try {
      totals(b, b.years[0]);
    } catch (const std::invalid_argument&) {
      refused = true;
    }
    s.require("a line the chart does not map is refused, never guessed", refused);
  }
  const std::vector<Term> revenue{{"own", 1}, {"transfers", 1}}, surplus{{"own", 1}, {"transfers", 1}, {"operating", -1}};
  const Ratio osr{"operating surplus over operating revenue", surplus, revenue};
  const Ratio dsr{"debt service over own-source revenue", {{"interest", 1}, {"principal", 1}}, {{"own", 1}}};
  const auto rv = ratios(a, {osr, dsr}, {{osr.name, Sound::at_least, 0.2, 0.0}, {dsr.name, Sound::at_most, 0.1, 0.2}});
  // (800 + 400 − 900)/1200 = 0.25, sound against a warning below 0.2; (40 + 60)/800 = 0.125, a warning above 0.1, below 0.2.
  s.check("the operating surplus ratio 300/1200", rv[0].value - 0.25, 1e-15);
  s.require("... sound against a warning below 0.2", rv[0].status == Status::sound);
  s.check("the debt service ratio 100/800", rv[1].value - 0.125, 1e-15);
  s.require("... a warning, above 0.1 and below the intervention level 0.2", rv[1].status == Status::warning);
  {
    const Loan l{"bank loan", 1000, 0.05, 2025, 10, false};
    const auto y1 = service(l, 2025);
    const double payment = 1000 * 0.05 / (1 - std::pow(1.05, -10));
    s.check("an annuity of 1,000 at 5% over ten years: the payment 129.50, interest 50 in the first year", y1.interest + y1.principal - payment, 1e-9);
    double repaid = 0;
    for (int y = 2025; y < 2035; y++) repaid += service(l, y).principal;
    s.check("... its principal repaid over its life sums to 1,000, and nothing is outstanding after", repaid - 1000 + service(l, 2034).outstanding_after, 1e-9);
    const Loan b{"bond", 500, 0.04, 2025, 3, true};
    s.require("a bullet: interest 20 a year, the principal in its last year", service(b, 2025).interest == 20 && service(b, 2025).principal == 0 && service(b, 2027).principal == 500);
  }
  const std::vector<Loan> existing{{"bank loan", 1000, 0.05, 2025, 10, false}};
  const auto p = project(t, 2024, 5, {{"own", 0.03}, {"transfers", 0.0}, {"operating", 0.02}}, surplus, existing);
  {
    const double os1 = 800 * 1.03 + 400 - 900 * 1.02, ds = 1000 * 0.05 / (1 - std::pow(1.05, -10));
    s.check("the first year ahead: the surplus 800·1.03 + 400 − 900·1.02 = 306", p.surplus[0] - os1, 1e-9);
    s.check("... its coverage the surplus over the annuity's payment", p.coverage[0] - os1 / ds, 1e-12);
  }
  {
    const auto c = debt_capacity(p, 0.06, 5, 2025, 1.5);
    // Proposition: once taken, the coverage holds in every repayment year and with equality at the binding year.
    auto with = existing;
    with.push_back({"new", c.largest, 0.06, 2025, 5, false});
    const auto q = project(t, 2024, 5, {{"own", 0.03}, {"transfers", 0.0}, {"operating", 0.02}}, surplus, with);
    double least = 1e300;
    int at = 0;
    for (std::size_t k = 0; k < q.years.size(); k++)
      if (q.coverage[k] < least) least = q.coverage[k], at = q.years[k];
    s.check("the largest new loan at 6% over five years for a coverage of 1.5: once taken, the least coverage is 1.5", least - 1.5, 1e-9);
    s.require("... at the year that binds it", at == c.binding_year && c.binding == "coverage" && c.largest > 0);
    const auto cap = debt_capacity(p, 0.06, 5, 2025, 1.5, revenue, 0.9);
    // The stock after the first repayment year: existing outstanding plus the new loan's remainder, at most 0.9 of revenue.
    with.back().principal = cap.largest;
    const auto r = project(t, 2024, 5, {{"own", 0.03}, {"transfers", 0.0}, {"operating", 0.02}}, surplus, with);
    s.require("... a ceiling of 0.9 of a year's revenue on the stock binds it lower, the stock then at the ceiling",
              cap.binding == "stock ceiling" && cap.largest < c.largest && std::fabs(r.outstanding[0] - 0.9 * sum(r.totals[0], revenue)) < 1e-6);
    const auto none_left = debt_capacity(project(t, 2024, 5, {{"operating", 0.2}}, surplus, existing), 0.06, 5, 2025, 1.5);
    s.require("operating costs growing 20% a year: the coverage already fails, and no new loan can be serviced", none_left.largest == 0 && none_left.binding == "coverage already below the floor");
  }
  {
    // Debt of 0.5, r 4%, g 3%: the stabilising balance 0.5·0.01/1.03.
    const double pb = stabilising_balance(0.5, 0.04, 0.03);
    const auto d = debt_path(0.5, std::vector<double>(10, 0.04), std::vector<double>(10, 0.03), std::vector<double>(10, pb));
    s.check("the primary balance d (r − g)/(1 + g) holds the debt where it is, ten years on", d.back() - 0.5, 1e-12);
    const auto e = debt_path(0.5, {0.04}, {0.03}, {0.0});
    s.check("... and with none, a year multiplies it by 1.04/1.03", e[1] - 0.5 * 1.04 / 1.03, 1e-15);
  }
  {
    const std::vector<double> rr{0.03, 0.05, 0.04, 0.06, 0.02, 0.04, 0.05, 0.03}, gg{0.02, 0.01, 0.04, 0.03, -0.01, 0.05, 0.02, 0.03},
        pp{0.00, -0.01, 0.01, 0.00, -0.02, 0.01, 0.00, 0.01};
    const std::vector<double> r(6, 0.04), g(6, 0.02), pb(6, 0.0);
    const auto f = fan(0.4, r, g, pb, rr, gg, pp, 2, 2000, 0.5, {0.1, 0.5, 0.9}, 7);
    bool ordered = true;
    for (const auto& q : f.paths_at) ordered = ordered && q[0] <= q[1] && q[1] <= q[2];
    s.require("a fan of 2,000 paths in blocks of two years: its quantiles ordered every year", ordered);
    s.check("... the median near the baseline (within 0.03 after six years)", f.paths_at.back()[1] - f.baseline.back(), 0.03);
    const auto same = fan(0.4, r, g, pb, rr, gg, pp, 2, 2000, 0.5, {0.1, 0.5, 0.9}, 7);
    s.require("... drawn again with the same seed, the same fan", same.paths_at.back() == f.paths_at.back() && same.above == f.above);
    const auto flat = fan(0.4, r, g, pb, std::vector<double>(8, 0.04), std::vector<double>(8, 0.02), std::vector<double>(8, 0.0), 2, 100, 0.5, {0.1, 0.9}, 3);
    s.check("a record without variation collapses the fan onto the baseline", flat.paths_at.back()[1] - flat.paths_at.back()[0] + flat.paths_at.back()[0] - flat.baseline.back(), 1e-12);
  }
  {
    // Two governments, two bases. Base 1: 100 and 300, raised 10 and 20 (rate 30/400); base 2: 50 and 50, raised 5 and 15 (rate 0.2).
    const auto e = representative_tax_system({{100, 50}, {300, 50}}, {{10, 5}, {20, 15}});
    s.check("the representative rate of the first base, 30/400", e.rates[0] - 0.075, 1e-15);
    s.check("the first government's capacity 100·0.075 + 50·0.2 = 17.5", e.capacity[0] - 17.5, 1e-12);
    s.check("... its effort, 15 raised over 17.5", e.effort[0] - 15.0 / 17.5, 1e-12);
    s.check("the whole raises what its capacities sum to (the rates are the whole's)", e.capacity[0] + e.capacity[1] - 50, 1e-12);
  }
  return s.done();
}
