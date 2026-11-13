// Checks of social_provision/care: need as Σ n π by hand with its hours; a scenario of compression; supply from minutes a day and a
// workforce; the gap and the share met; the care diamond's shares; support ratios by hand; and the decomposition of a change in need:
// shares summing to the change exactly, a change of size alone given wholly to size, prevalence alone to prevalence, and reversal
// reversing every share.

#include <social_provision/care/care.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace social_provision::care;

int main() {
  calx::verification::Suite s;
  {
    const auto n = need({1000, 500, 200}, {0.05, 0.2, 0.5}, 10);
    s.check("need: Σ n π = 50 + 100 + 100", n.people - 250, 1e-12);
    s.check("need: hours a week at 10 each", n.hours_a_week - 2500, 1e-9);
    const auto p = scenario({0.1, 0.4}, 0.01, 10);
    s.check("scenario: compression of 1% a year for ten years", p[1] - 0.4 * std::pow(0.99, 10), 1e-15);
    const auto sup = supply({200, 100}, {30, 60}, 20, 38);
    s.check("supply: informal hours from minutes a day", sup.informal - (200 * 30 + 100 * 60) * 7.0 / 60, 1e-9);
    s.check("supply: formal hours", sup.formal - 760, 1e-12);
    const auto g = gap(2500, sup.total);
    s.check("gap: need less supply", g.gap - (2500 - sup.total), 1e-9);
    s.require("gap: the share met is at most one", g.met <= 1 && gap(10, 20).met == 1);
    const auto d = diamond(600, 250, 100, 50);
    s.check("diamond: the households' share", d.share(d.household) - 0.6, 1e-15);
  }
  {
    std::vector<double> ages, pop;
    for (int a = 0; a <= 100; a++) ages.push_back(a), pop.push_back(a < 15 ? 10 : a < 50 ? 12 : a < 65 ? 11 : a < 85 ? 8 : 3);
    const auto r = ratios(ages, pop);
    const double working = 35 * 12 + 15 * 11, old = 20 * 8 + 16 * 3;
    s.check("ratios: old-age dependency 65+ per 100 aged 15–64", r.old_age_dependency - 100 * old / working, 1e-12);
    s.check("ratios: aged 50–64 per person 85+", r.oldest_old_support - 15 * 11.0 / (16 * 3), 1e-12);
  }
  {
    const std::vector<double> p0{1000, 600, 200}, q0{0.02, 0.1, 0.4}, p1{1100, 700, 320}, q1{0.02, 0.09, 0.35};
    const auto c = decompose(p0, q0, p1, q1);
    s.check("decomposition: size + structure + prevalence = the change", c.size + c.structure + c.prevalence - c.change, 1e-10);
    s.require("decomposition: ageing raises need, falling prevalence lowers it", c.structure > 0 && c.prevalence < 0);
    std::vector<double> doubled(p0);
    for (double& x : doubled) x *= 2;
    const auto only = decompose(p0, q0, doubled, q0);
    s.check("decomposition: a doubled population, same shape, is all size", only.size - only.change, 1e-10);
    s.check("decomposition: with nothing to structure or prevalence", std::fabs(only.structure) + std::fabs(only.prevalence), 1e-10);
    const auto prev = decompose(p0, q0, p0, q1);
    s.check("decomposition: prevalence alone takes all", prev.prevalence - prev.change, 1e-10);
    const auto back = decompose(p1, q1, p0, q0);
    s.check("decomposition: reversing the years reverses every share", std::fabs(back.size + c.size) + std::fabs(back.structure + c.structure) + std::fabs(back.prevalence + c.prevalence), 1e-10);
  }
  return s.done();
}
