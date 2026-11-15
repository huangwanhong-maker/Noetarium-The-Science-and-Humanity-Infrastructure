// Checks of theories of change (change.hpp): a theory read from its written form, and its faults; the strongest path and its weakest
// link, found by dynamic programming and held to every path enumerated; links not shown, shown against, and gaps on every path;
// Wright's rule and its enclosure, held to the corners of the coefficients' box when no link is shared, and shown wider than the
// exact range when one is; a cycle refused with the cycle named.

#include <policy/change/change.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <functional>
#include <string>
#include <vector>

using namespace policy::change;

namespace {

const char* meals =
    "theory free_school_meals\n"
    "levels very_low low moderate high\n"
    "node meals intervention: free meals for every pupil\n"
    "node attendance outcome: pupils attend more days\n"
    "node hunger outcome: fewer pupils hungry in class\n"
    "node learning outcome: pupils learn more\n"
    "link meals -> attendance: families send children who would otherwise stay home\n"
    "support meals -> attendance: the meal is served before the first lesson\n"
    "evidence meals -> attendance: 0.10 [0.04, 0.16] moderate\n"
    "link meals -> hunger: pupils eat at school\n"
    "support meals -> hunger: the meal is enough for a morning\n"
    "evidence meals -> hunger: 0.30 [0.20, 0.40] high\n"
    "link attendance -> learning: more days in class, more learnt\n"
    "evidence attendance -> learning: 0.20 [0.10, 0.30] high\n"
    "link hunger -> learning: a fed pupil attends to the lesson\n"
    "evidence hunger -> learning: 0.05 [-0.02, 0.12] moderate\n";

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- The written form ----------------------------------------------------------------------------------------------------------
  const auto t = theory(meals);
  s.require("a theory of free school meals reads: four nodes, four links, two support factors, four levels", t.nodes.size() == 4 && t.links.size() == 4 && t.links[0].support.size() == 1 && t.levels.size() == 4);
  const auto bad = read("theory x\nlevels a b\nnode p start: p\nlink p -> q: ?\nevidence p -> r: 1 [0, 2] a\nsign p -> q: up\nnode p start: again\nevidence p -> q: 1 [0, 2] z\n");
  s.require("its faults are found: evidence for a link not declared, a sign neither + nor -, a node declared twice, a link to a node not declared, a grade not among the levels",
            !bad.ok && bad.errors.size() == 5, std::to_string(bad.errors.size()) + ": " + (bad.errors.empty() ? "" : bad.errors[0]));

  // ---- The strongest path --------------------------------------------------------------------------------------------------------
  {
    const auto a = assess(t, "meals", "learning", "moderate");
    s.require("two paths lead from meals to learning", a.paths == 2);
    s.require("the strongest runs through attendance, its weakest link meals -> attendance at moderate", a.strongest.weakest == 2 && a.strongest.links.size() == 2 && t.links[a.strongest.links[0]].to == "attendance" &&
                                                                                                          t.links[a.strongest.links[a.strongest.limiting]].from == "meals",
              a.verdict.reason);
    s.require("so learning is shown to follow at moderate: the verdict holds, with the support factor the path needs", a.verdict.holds() && a.strongest.support == std::vector<std::string>{"the meal is served before the first lesson"});
    s.require("asked for high, it is undecided, the limiting link named", assess(t, "meals", "learning", "high").verdict.undecided());
    s.require("the path through hunger is not shown: its last link's interval reaches no effect, whatever the grade of the first",
              standing(t, t.links[3]).rank == not_shown && standing(t, t.links[3]).why.find("reaches no effect") != std::string::npos);
    s.require("no link lies on every path, so there is no gap on every path", a.necessary.empty() && a.necessary_gaps.empty());
    s.check("Wright's rule: 0.10 × 0.20 + 0.30 × 0.05 = 0.035", std::abs(a.implied - 0.035), 1e-15);
    // No link is shared between the two paths, so the enclosure is the exact range: the corners of the box of coefficients.
    double lo = INFINITY, hi = -INFINITY;
    for (int m = 0; m < 16; m++) {
      const double c1 = m & 1 ? 0.16 : 0.04, c2 = m & 2 ? 0.30 : 0.10, c3 = m & 4 ? 0.40 : 0.20, c4 = m & 8 ? 0.12 : -0.02;
      lo = std::min(lo, c1 * c2 + c3 * c4), hi = std::max(hi, c1 * c2 + c3 * c4);
    }
    s.require("with no link on two paths the enclosure is exact: [0, 0.096], the corners' least and greatest", !a.shared && std::abs(a.implied_lower - lo) < 1e-15 && std::abs(a.implied_upper - hi) < 1e-15,
              std::to_string(a.implied_lower) + " " + std::to_string(a.implied_upper));
  }

  // ---- Dynamic programming held to every path ------------------------------------------------------------------------------------
  {
    calx::random::Stream rng(20261115, 60, 0, 0);
    std::size_t wrong = 0, trials = 0;
    for (int trial = 0; trial < 300; trial++) {
      Theory r;
      r.levels = {"l0", "l1", "l2", "l3"};
      const std::size_t n = 3 + rng.below(6);
      for (std::size_t i = 0; i < n; i++) r.nodes.push_back({"n" + std::to_string(i), "state", ""});
      for (std::size_t i = 0; i < n; i++)
        for (std::size_t j = i + 1; j < n; j++)
          if (rng.uniform() < 0.45) {
            Link l{"n" + std::to_string(i), "n" + std::to_string(j), "", {}, +1, {}};
            if (rng.uniform() < 0.8) {
              const double e = rng.uniform(-0.2, 0.6), w = rng.uniform(0.01, 0.3);
              l.evidence = {true, e, e - w, e + w, r.levels[rng.below(4)]};
            }
            r.links.push_back(l);
          }
      const std::string from = "n0", to = "n" + std::to_string(n - 1);
      const auto a = assess(r, from, to, "l2");
      // Every path, enumerated: the best weakest link, and the number of paths.
      long best = std::numeric_limits<long>::min();
      std::size_t count = 0;
      std::function<void(const std::string&, long)> walk = [&](const std::string& at, long weakest) {
        if (at == to) { count++, best = std::max(best, weakest); return; }
        for (const auto& l : r.links)
          if (l.from == at) walk(l.to, std::min(weakest, standing(r, l).rank));
      };
      walk(from, 4);
      if (count != a.paths) wrong++;
      else if (count > 0 && best != a.strongest.weakest) wrong++;
      trials += count > 0;
    }
    s.require("on 300 random theories the strongest path's weakest link and the number of paths are those of every path enumerated (" + std::to_string(trials) + " with a path)", wrong == 0, std::to_string(wrong));
  }

  // ---- Gaps, a link against, a shared link, a cycle, no path ---------------------------------------------------------------------
  {
    const auto chain = theory("theory c\nlevels low high\nnode a x: a\nnode b x: b\nnode c x: c\nlink a -> b: …\nevidence a -> b: 1 [0.5, 1.5] high\nlink b -> c: …\n");
    const auto g = assess(chain, "a", "c", "low");
    s.require("in a chain whose second link has no evidence, that link is a gap on every path and the verdict is undecided",
              g.necessary.size() == 2 && g.necessary_gaps.size() == 1 && chain.links[g.necessary_gaps[0]].from == "b" && g.verdict.undecided());
    const auto against = theory("theory c\nlevels low high\nnode a x: a\nnode b x: b\nlink a -> b: …\nsign a -> b: -\nevidence a -> b: 0.3 [0.1, 0.5] high\n");
    s.require("a link expected to lower its outcome whose interval lies above zero is shown against the theory", standing(against, against.links[0]).why.find("against the theory") != std::string::npos);
    // a → m at [1, 2]; m → out directly at 1, and through k at 1 then −1: the total a (1 − 1) is exactly 0, the enclosure [−1, 1].
    const auto shared = theory(
        "theory s\nlevels low high\nnode a x: a\nnode m x: m\nnode k x: k\nnode out x: out\n"
        "link a -> m: …\nevidence a -> m: 1.5 [1, 2] high\nlink m -> out: …\nevidence m -> out: 1 [1, 1] high\n"
        "link m -> k: …\nevidence m -> k: 1 [1, 1] high\nlink k -> out: …\nsign k -> out: -\nevidence k -> out: -1 [-1, -1] high\n");
    const auto sh = assess(shared, "a", "out", "low");
    s.require("when a link lies on two paths the implied effect is still exact, 0, and the enclosure [−1, 1] contains the exact range and is said not to be exact",
              sh.shared && sh.implied == 0 && sh.implied_lower == -1 && sh.implied_upper == 1);
    bool cyc = false;
    std::string why;
    try { (void)theory("theory c\nlevels low high\nnode a x: a\nnode b x: b\nlink a -> b: …\nlink b -> a: …\n"); } catch (const std::invalid_argument& e) { cyc = true, why = e.what(); }
    s.require("a theory with a cycle is refused, the cycle named", cyc && why.find("cycle") != std::string::npos && why.find("a -> b") != std::string::npos, why);
    s.require("an outcome no path reaches fails", assess(t, "learning", "meals", "low").verdict.fails());
  }

  return s.done();
}
