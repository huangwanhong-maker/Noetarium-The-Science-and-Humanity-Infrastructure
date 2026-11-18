// The stability of a gap under resampling of clusters (ADR 0125; the theory note §3.3, §5.3).
//
// The clusters of one part are resampled with replacement (tyche's bootstrap, by clusters); in each resample the gap is estimated
// again, each copy of a cluster a cluster of its own. Stability is the share of resamples in which the gap keeps its sign and
// clears the threshold z* (Meinshausen and Bühlmann 2010's selection under resampling, read for one statement). The resamples are
// drawn from one key whatever the statement, so that statements are compared on the same resamples.
//
// Caveat. Without a calibrated threshold and stability selection's exchangeability conditions, stability is a screen, not a
// control of error (the survey's §12.2): the control of error is the confirmation part's.
#pragma once

#include <needs/evidence/gap.hpp>

#include <tyche/testing/bootstrap.hpp>

#include <calx/random/stream.hpp>

namespace needs::evidence {

struct Stability {
  double share = nan;              // resamples in which z ≥ threshold
  double low = nan, high = nan;    // the gap's percentile interval over the resamples
  std::size_t resamples = 0, failed = 0;
};

inline Stability stability(const Totals& T, double threshold = 2.0, std::size_t resamples = 200, std::uint64_t key = 7, double confidence = 0.95) {
  Stability out;
  const std::size_t C = T.yg.size();
  if (C < 2 || resamples == 0) return out;
  calx::random::Stream s(key, 0x57ab, 0, 0);
  std::size_t kept = 0, seen = 0;
  const auto b = tyche::testing::bootstrap(
      C,
      [&](const std::vector<double>& m) {
        std::vector<int> mult(C);
        for (std::size_t c = 0; c < C; c++) mult[c] = static_cast<int>(m[c]);
        const Gap g = estimate(T, Inference::design, 0.95, &mult);
        if (!g.defined) return nan;
        seen++;
        if (g.z >= threshold) kept++;
        return g.estimate;
      },
      resamples, s, confidence);
  out.resamples = resamples, out.failed = b.failed;
  out.share = static_cast<double>(kept) / static_cast<double>(resamples);
  out.low = b.low, out.high = b.high;
  (void)seen;
  return out;
}

}  // namespace needs::evidence
