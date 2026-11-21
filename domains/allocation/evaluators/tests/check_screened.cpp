// Checks of the stand-in inside the search (allocation/evaluators/surrogate.hpp, ADR 0142): a fund for eight places, each claiming
// a fixed share met in full or in part in the order searched, judged by an economy of places whose households spend by whether
// they are short of money. The same search is run with the economy alone and with the economy behind a stand-in that screens as
// it goes, at two confidences. Behind the stand-in the economy runs fewer times, and fewer still at the lower confidence; only
// allocations the economy ran are kept; the best is as good to within 1%; the online coverage is at least what the intervals promise, less a margin for sampling;
// the search repeats itself exactly; and a stand-in declared without its metric is refused.

#include <allocation/evaluators/evaluators.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

namespace ae = allocation::evolution;
namespace av = allocation::evaluators;
namespace ha = heuresis::assessment;
namespace kr = kategoria::resources;

namespace {

std::shared_ptr<const ae::Composition> fund_for_places(std::size_t n) {
  ae::Problem p;
  p.name = "a fund for eight places (declared, illustrative)";
  for (std::size_t i = 0; i < n; i++) {
    allocation::rounds::Entity e;
    e.id = "place " + std::to_string(i + 1);
    e.at = {{"claim", static_cast<double>(4 + (i * 5) % 7)}, {"population", 4}};
    p.recipients.push_back(e);
  }
  kr::Type T;
  T.name = "fund";
  T.components = {kr::Component{"fund", kr::Quantity::natural, 1, {kr::Kind{"fund", true, std::nullopt, false}}}};
  ae::Resource fund{"fund", T, ae::Carrier::fixed, {kr::Rational(24)}, 0, {}, "claim"};
  fund.partial = true;
  p.resources = {fund};
  return std::make_shared<const ae::Composition>(p);
}

av::EconomySpec economy_of(std::size_t n) {
  av::EconomySpec e;
  for (std::size_t i = 0; i < n; i++) e.places.push_back({"place " + std::to_string(i + 1), 4, 2, static_cast<economics::ledger::Amount>(20000 + 30000 * (i % 4)), 40000});
  e.habits.short_below = 60000;
  e.unit = 1000;
  e.spacing = 2;
  e.periods = 40;
  return e;
}

double best_of(const ae::Outcome& o) {
  double b = INFINITY;
  for (const auto& k : o.kept)
    if (const auto* m = k.assessment.find("local re-spending lost")) b = std::min(b, m->value);
  return b;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const std::size_t n = 8;
  const auto c = fund_for_places(n);
  ae::Settings st;
  st.strategy.mu = 16, st.strategy.lambda = 16, st.strategy.generations = 40;
  st.keep = 6, st.keep_apart = 0, st.seed = 7;
  // The economy alone.
  const auto plain = ae::search(c, {av::economy_evaluator(c, economy_of(n))}, st);
  // The economy behind a stand-in, at two confidences.
  struct Run {
    std::shared_ptr<av::ScreenLog> log;
    ae::Outcome out;
  };
  auto behind = [&](double alpha) {
    av::ScreenSpec spec;
    spec.metric = "local re-spending lost";
    spec.scale = 24, spec.warm = 48, spec.refit = 16, spec.alpha = alpha, spec.keep = 6;
    Run r{std::make_shared<av::ScreenLog>(), {}};
    r.out = ae::search(c, {av::screened_economy(c, economy_of(n), spec, r.log)}, st);
    return r;
  };
  const double a = best_of(plain);
  const Run tight = behind(0.1), loose = behind(0.3);
  for (const auto* r : {&tight, &loose})
    std::printf("behind the stand-in at %.0f%%: %zu run of %zu, %zu screened out, best %.3f (alone %.3f), %zu refits, online coverage %.3f on %zu\n",
                r == &tight ? 90.0 : 70.0, r->log->runs, plain.evaluated_allocations, r->log->screened, best_of(r->out), a, r->log->refits, r->log->coverage(),
                r->log->checked);
  s.require("the stand-in screened candidates out once warm, and refitted as runs arrived", tight.log->screened > 0 && tight.log->refits >= 2,
            std::to_string(tight.log->screened) + " screened, " + std::to_string(tight.log->refits) + " refits");
  s.require("behind the stand-in the economy runs fewer times than the plain search evaluates allocations, at either confidence",
            tight.log->runs < plain.evaluated_allocations && loose.log->runs < plain.evaluated_allocations,
            std::to_string(tight.log->runs) + ", " + std::to_string(loose.log->runs) + " against " + std::to_string(plain.evaluated_allocations));
  s.require("... and fewer at the lower confidence: the narrower intervals screen more",
            loose.log->screened > tight.log->screened, std::to_string(loose.log->screened) + " against " + std::to_string(tight.log->screened));
  s.check("the best found behind the stand-in is as good as the plain search's, to within 1% (relative), at 90%",
          std::max(0.0, (best_of(tight.out) - a) / std::fabs(a)), 0.01);
  s.check("... and at 70%", std::max(0.0, (best_of(loose.out) - a) / std::fabs(a)), 0.01);
  bool all_run = !tight.out.kept.empty() && !loose.out.kept.empty();
  for (const auto* r : {&tight, &loose})
    for (const auto& k : r->out.kept) {
      const auto* m = k.assessment.find("local re-spending lost");
      all_run = all_run && k.assessment.ok() && m && m->standing == ha::Standing::computed;
    }
  s.require("every allocation kept was run in the economy: none is kept on the stand-in's word", all_run);
  // A conformal interval promises at least its confidence; on the candidates a stand-in lets through it may cover more.
  s.require("the online coverage, on the candidates run after a prediction, at least the 90% promised less 0.05 for sampling",
            tight.log->checked >= 20 && tight.log->coverage() >= 0.85, std::to_string(tight.log->coverage()) + " on " + std::to_string(tight.log->checked));
  s.require("... and at least the 70% promised less 0.05", loose.log->checked >= 20 && loose.log->coverage() >= 0.65,
            std::to_string(loose.log->coverage()) + " on " + std::to_string(loose.log->checked));
  const Run twice = behind(0.1);
  s.require("the screened search repeats itself exactly: the same runs, the same screened, the same best",
            twice.log->runs == tight.log->runs && twice.log->screened == tight.log->screened && best_of(twice.out) == best_of(tight.out));
  bool refused = false;
  try {
    av::ScreenSpec bad;
    bad.metric = "";
    av::screened(c, "x", ha::Kind::agent_model, ha::Standing::computed, [](const ae::Allocation&, calx::random::Stream&) { return ha::Assessment{}; }, bad,
                 std::make_shared<av::ScreenLog>());
  } catch (const ae::Refusal&) {
    refused = true;
  }
  s.require("a stand-in declared without the metric it learns is refused", refused);
  return s.done();
}
