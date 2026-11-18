// The gap of a statement on a part of the data, with its interval and its test (ADR 0125; the theory note §2, §3).
//
// Every indicator is a ratio of two sums over a group's units, R = Σ w y / Σ w x (statement.hpp). The gap is
//   comparative:        ±(R_G − R_R), R_R over the reference group (the frame less the group, unless one is named);
//   normative and felt: ±(R_G − τ);
//   expressed:          R_G with y = demand − provision and x = demand: the share of the demand that goes unmet;
// signed so that a positive gap is worse. The statement claims a positive gap, so its test is one-sided.
//
// Inference, declared:
// - design (the default): the units' clusters are the primary sampling units, and the variance is the ultimate-cluster
//   estimator of the ratios linearised (peira's surveys: ratio, and difference, whose covariance over shared clusters is kept
//   when the group and its reference live in the same areas). The degrees of freedom are the clusters less one. A table of counts
//   in cells of a census is a full count, not a sample; read as a sample from the process that produced it, people of one area
//   share their circumstances, and the clustered variance says so where a binomial one would not.
// - binomial: units independent. A share is the successes summed over the total (tyche's score test of a share against a level,
//   and Newcombe's interval of a difference of shares); a value is the weighted mean (Welch's test). For data whose rows are
//   independent people or households.
//
// Totals are kept per cluster, so that a resample of clusters (stability.hpp) costs a pass over the clusters, not the rows.
#pragma once

#include <needs/evidence/parts.hpp>

#include <peira/surveys/surveys.hpp>
#include <tyche/probability/distributions.hpp>
#include <tyche/testing/two_sample.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <string>
#include <vector>

namespace needs::evidence {

using language::Standard;
using language::Statement;

enum class Inference { design, binomial };

inline const char* name(Inference i) { return i == Inference::design ? "design" : "binomial"; }
inline Inference inference_of(const std::string& s) {
  if (s == "design") return Inference::design;
  if (s == "binomial") return Inference::binomial;
  throw Refusal("no inference '" + s + "' (there are design, binomial)");
}

constexpr double nan = std::numeric_limits<double>::quiet_NaN();

// The sums of a statement's group and reference, cluster by cluster within one part.
struct Totals {
  Part part = Part::discovery;
  std::vector<double> yg, xg, yr, xr;       // per cluster of the part, in the part's order
  std::vector<double> wg, wr;               // rows' weights summed (for the binomial reading of values)
  double sign = 1;                          // +1 when higher is worse
  double level = nan;                       // normative and felt
  bool comparative = false, expressed = false, share = false;
  std::size_t rows_g = 0, rows_r = 0;
  // For the binomial reading of values: each row's value and weight in the group and the reference.
  std::vector<double> vg, wvg, vr, wvr;
};

struct Gap {
  bool defined = false;
  std::string why;                          // when not defined
  double estimate = nan, se = nan, df = nan, low = nan, high = nan;
  double z = nan, p = nan;                  // standardised gap; one-sided p of no gap against a positive one
  double group_value = nan, reference_value = nan;  // R_G, and R_R or τ
  double coverage = 0;                      // Σ_G w x: the people the statement concerns
  double size = nan;                        // estimate × coverage: units of unmet need
  std::size_t rows = 0, clusters = 0;       // the group's rows, and the clusters it touches
};

namespace detail {

struct Columns {
  const std::vector<double>* y = nullptr;   // value or successes or demand
  const std::vector<double>* x = nullptr;   // totals, or null for 1
  const std::vector<double>* p = nullptr;   // provision (expressed)
  bool share = false;
};

inline Columns columns(const Dataset& d, const Statement& s) {
  Columns c;
  if (s.standard == Standard::expressed) {
    c.y = &d.table.at(s.indicator).numbers;
    c.p = &d.table.at(s.provision).numbers;
    return c;
  }
  const auto& ind = d.indicator(s.indicator);
  c.y = &d.table.at(ind.column).numbers;
  if (!ind.total.empty()) c.x = &d.table.at(ind.total).numbers;
  c.share = ind.share;
  return c;
}

// One row's (y, x): a value v weighted by its people t is (t v, t); a share (k, n); an expressed need (d − p, d).
inline bool row(const Columns& c, std::size_t i, double& y, double& x, double& v) {
  if (c.p) {
    const double dm = (*c.y)[i], pv = (*c.p)[i];
    if (!std::isfinite(dm) || !std::isfinite(pv)) return false;
    y = dm - pv, x = dm, v = dm > 0 ? (dm - pv) / dm : nan;
    return true;
  }
  const double a = (*c.y)[i];
  const double t = c.x ? (*c.x)[i] : 1.0;
  if (!std::isfinite(a) || !std::isfinite(t) || t < 0) return false;
  if (c.share) y = a, x = t, v = t > 0 ? a / t : nan;
  else y = t * a, x = t, v = a;
  return true;
}

}  // namespace detail

inline Totals totals(const Dataset& d, const Statement& s, const Parts& parts, Part part, bool keep_rows = false) {
  const auto& t = d.table;
  const auto pi = static_cast<std::size_t>(part);
  Totals T;
  T.part = part;
  const std::size_t C = parts.members[pi].size();
  T.yg.assign(C, 0), T.xg.assign(C, 0), T.yr.assign(C, 0), T.xr.assign(C, 0), T.wg.assign(C, 0), T.wr.assign(C, 0);
  T.sign = s.worse_when_higher ? 1 : -1;
  T.comparative = s.standard == Standard::comparative;
  T.expressed = s.standard == Standard::expressed;
  if (T.expressed) T.sign = 1;
  if (s.standard == Standard::normative || s.standard == Standard::felt) T.level = s.level;
  const groups::Rows frame = groups::rows_of(t, s.frame);
  const groups::Rows g = groups::rows_of(t, s.group, frame);
  groups::Rows r;
  if (T.comparative) {
    if (s.reference) r = groups::rows_of(t, *s.reference, frame);
    else {
      r = frame;
      for (std::size_t i = 0; i < t.rows; i++) r[i] = r[i] && !g[i];
    }
  }
  const auto cols = detail::columns(d, s);
  T.share = cols.share;
  for (std::size_t i = 0; i < t.rows; i++) {
    if (parts.part_of_row[i] != static_cast<int>(pi)) continue;
    const bool in_g = g[i], in_r = T.comparative && r[i];
    if (!in_g && !in_r) continue;
    double y, x, v;
    if (!detail::row(cols, i, y, x, v)) continue;
    const double w = t.w(i);
    const auto c = static_cast<std::size_t>(parts.index_in_part[static_cast<std::size_t>(parts.cluster_of_row[i])]);
    if (in_g) {
      T.yg[c] += w * y, T.xg[c] += w * x, T.wg[c] += w, T.rows_g++;
      if (keep_rows && std::isfinite(v)) T.vg.push_back(v), T.wvg.push_back(w * x);
    }
    if (in_r) {
      T.yr[c] += w * y, T.xr[c] += w * x, T.wr[c] += w, T.rows_r++;
      if (keep_rows && std::isfinite(v)) T.vr.push_back(v), T.wvr.push_back(w * x);
    }
  }
  return T;
}

// The gap from the totals; with multiplicities (one count per cluster of the part), the gap of a resample of clusters, each copy
// a cluster of its own.
inline Gap estimate(const Totals& T, Inference inference = Inference::design, double confidence = 0.95, const std::vector<int>* multiplicity = nullptr) {
  Gap out;
  const std::size_t C = T.yg.size();
  std::vector<double> yg, xg, yr, xr;
  double sy = 0, sx = 0, ry = 0, rx = 0;
  std::size_t touched = 0;
  for (std::size_t c = 0; c < C; c++) {
    const int m = multiplicity ? (*multiplicity)[c] : 1;
    for (int k = 0; k < m; k++) {
      yg.push_back(T.yg[c]), xg.push_back(T.xg[c]);
      if (T.comparative) yr.push_back(T.yr[c]), xr.push_back(T.xr[c]);
    }
    sy += m * T.yg[c], sx += m * T.xg[c], ry += m * T.yr[c], rx += m * T.xr[c];
    if (T.xg[c] > 0 && m > 0) touched++;
  }
  out.rows = T.rows_g, out.clusters = touched, out.coverage = sx;
  if (!(sx > 0)) {
    out.why = "the group has nobody in the " + std::string(name(T.part)) + " part";
    return out;
  }
  if (T.comparative && !(rx > 0)) {
    out.why = "the reference group has nobody in the " + std::string(name(T.part)) + " part";
    return out;
  }
  out.group_value = sy / sx;
  out.reference_value = T.comparative ? ry / rx : T.expressed ? 0.0 : T.level;
  const double raw = out.group_value - out.reference_value;
  out.estimate = T.sign * raw;
  out.size = out.estimate * out.coverage;
  try {
    if (inference == Inference::design || T.expressed) {  // an expressed need (demand less provision) is read by design alone
      if (yg.size() < 2) {
        out.why = "fewer than two clusters in the " + std::string(name(T.part)) + " part";
        return out;
      }
      peira::surveys::Design design;
      design.weight.assign(yg.size(), 1.0);
      const auto e = T.comparative ? peira::surveys::difference(yg, xg, yr, xr, design, confidence) : peira::surveys::ratio(yg, xg, design, confidence);
      out.se = e.se, out.df = e.df;
      const double q = tyche::probability::StudentT{out.df}.upper_quantile((1 - confidence) / 2);
      out.low = out.estimate - q * out.se, out.high = out.estimate + q * out.se;
      if (out.se > 0) {
        out.z = out.estimate / out.se;
        out.p = tyche::probability::StudentT{out.df}.survival(out.z);
      } else {
        out.z = out.estimate > 0 ? std::numeric_limits<double>::infinity() : out.estimate < 0 ? -std::numeric_limits<double>::infinity() : 0.0;
        out.p = out.estimate > 0 ? 0.0 : 1.0;
      }
    } else {
      tyche::testing::Comparison c;
      if (multiplicity) throw Refusal("a resample is read by design inference");
      if (T.share) {  // shares are judged on their summed counts
        if (T.comparative) c = tyche::testing::share_difference(std::min(sy, sx), sx, std::min(ry, rx), rx, confidence);
        else c = tyche::testing::share_against_level(std::min(sy, sx), sx, T.level, confidence);
      } else {
        const auto a = tyche::testing::weighted_mean(T.vg, T.wvg);
        if (T.comparative) c = tyche::testing::welch(a, tyche::testing::weighted_mean(T.vr, T.wvr), confidence);
        else c = tyche::testing::against_level(a, T.level, confidence);
      }
      out.se = c.se, out.df = c.df;
      out.low = T.sign > 0 ? c.low : -c.high, out.high = T.sign > 0 ? c.high : -c.low;
      if (std::isfinite(c.statistic)) {
        out.z = T.sign * c.statistic;
        out.p = std::isfinite(out.df) ? tyche::probability::StudentT{out.df}.survival(out.z) : tyche::probability::Normal{}.survival(out.z);
      }
    }
  } catch (const std::exception& e) {
    out.why = e.what();
    return out;
  }
  out.defined = std::isfinite(out.estimate) && std::isfinite(out.z);
  if (!out.defined && out.why.empty()) out.why = "the gap has no standard error on this part";
  return out;
}

inline Gap gap(const Dataset& d, const Statement& s, const Parts& parts, Part part, Inference inference = Inference::design, double confidence = 0.95) {
  return estimate(totals(d, s, parts, part, inference == Inference::binomial), inference, confidence);
}

// The value of an indicator over a frame's units on one part: for a normative level suggested from the data.
inline double frame_value(const Dataset& d, const std::string& indicator, const groups::Group& frame, const Parts& parts, Part part) {
  Statement s;
  s.domain = "x", s.indicator = indicator, s.standard = Standard::normative, s.level = 0;
  s.worse_when_higher = d.indicator(indicator).higher_is_worse;
  s.group = frame, s.frame = {};
  const auto T = totals(d, s, parts, part);
  double y = 0, x = 0;
  for (std::size_t c = 0; c < T.yg.size(); c++) y += T.yg[c], x += T.xg[c];
  return x > 0 ? y / x : nan;
}

// Strata of a part: its clusters dealt into k strata by a second keyed hash, for lexicase's cases and for adverse strata.
inline std::vector<int> strata(const Parts& parts, Part part, std::size_t k, std::uint64_t key) {
  const auto& m = parts.members[static_cast<std::size_t>(part)];
  std::vector<int> out(m.size());
  for (std::size_t c = 0; c < m.size(); c++)
    out[c] = static_cast<int>(std::min<double>(static_cast<double>(k) - 1, std::floor(detail::keyed_uniform(parts.clusters[static_cast<std::size_t>(m[c])], key ^ 0x5717a7aull) * static_cast<double>(k))));
  return out;
}

// The standardised gap within each stratum (0 where it is not defined): what lexicase reads as cases.
inline std::vector<double> stratum_gaps(const Totals& T, const std::vector<int>& stratum, std::size_t k) {
  std::vector<double> out(k, 0.0);
  for (std::size_t s = 0; s < k; s++) {
    std::vector<int> m(T.yg.size(), 0);
    for (std::size_t c = 0; c < m.size(); c++) m[c] = stratum[c] == static_cast<int>(s);
    const Gap g = estimate(T, Inference::design, 0.95, &m);
    out[s] = g.defined ? std::clamp(g.z, -50.0, 50.0) : 0.0;
  }
  return out;
}

}  // namespace needs::evidence
