// The space of statements a search moves in (ADR 0125; the theory note §4): the conditions it may state groups in, the indicators
// and domains it may name, and the bounds on a statement's size.
//
// The pool of conditions is read from the discovery part only: a category's levels (the most common, up to a bound); a number's
// cut points, its distinct values when it has few, otherwise its quantiles. Every group a generator or an operator makes is a
// conjunction of conditions from the pool, on distinct attributes, which keeps the language finite and every statement readable.
// Rows are kept as bit sets per condition, so that a conjunction's rows are an intersection.
#pragma once

#include <needs/evidence/gap.hpp>

#include <algorithm>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

namespace needs::search {

using evidence::Part;
using evidence::Parts;
using language::Dataset;
using language::Refusal;
using language::Standard;
using language::Statement;
using groups::Condition;
using groups::Group;

struct Space {
  std::vector<std::string> attributes, indicators, domains;
  std::vector<Condition> conditions;                         // the pool
  std::vector<std::size_t> attribute_of;                     // per condition, an index into attributes
  std::vector<groups::Rows> rows;                            // per condition, its rows (all parts)
  std::map<std::string, std::vector<double>> cuts;           // per number attribute
  std::map<std::string, std::vector<std::string>> levels;    // per category attribute, most common first
  std::map<std::string, std::string> domain_of;              // indicator → domain
  std::map<std::string, double> spread;                      // indicator → the spread of its clusters' values on discovery
  std::size_t max_conditions = 3;
  double min_coverage = 0;                                   // people a statement must concern on discovery

  std::string domain(const std::string& indicator) const {
    auto it = domain_of.find(indicator);
    return it == domain_of.end() ? (domains.empty() ? std::string("need") : domains.front()) : it->second;
  }
};

struct SpaceOptions {
  std::size_t quantiles = 9;        // cut points of a number with many values
  std::size_t distinct = 12;        // a number with at most this many values is cut at each
  std::size_t max_levels = 24;      // levels of a category kept
  std::size_t max_conditions = 3;
  double min_coverage = 0;          // 0: 1% of the discovery part's people
};

inline Space space(const Dataset& d, const Parts& parts, const SpaceOptions& o = {}) {
  Space s;
  s.max_conditions = o.max_conditions;
  s.attributes = d.attribute_names();
  for (const auto& [k, ind] : d.indicators) s.indicators.push_back(k);
  if (s.indicators.empty()) throw Refusal("the data declare no indicator");
  s.domains = d.domains;
  for (const auto& [k, ind] : d.indicators) {
    std::string dom = ind.domain.empty() ? (d.domains.empty() ? std::string("need") : d.domains.front()) : ind.domain;
    s.domain_of[k] = dom;
    if (std::find(s.domains.begin(), s.domains.end(), dom) == s.domains.end()) s.domains.push_back(dom);
  }
  const auto& t = d.table;
  for (std::size_t a = 0; a < s.attributes.size(); a++) {
    const auto& col = t.at(s.attributes[a]);
    if (col.kind == groups::Kind::category) {
      std::map<int, double> count;
      for (std::size_t i = 0; i < t.rows; i++)
        if (parts.part_of_row[i] == 0 && col.codes[i] >= 0) count[col.codes[i]] += t.w(i);
      std::vector<std::pair<double, int>> order;
      for (const auto& [c, n] : count) order.push_back({-n, c});
      std::sort(order.begin(), order.end());
      std::vector<std::string>& lv = s.levels[s.attributes[a]];
      for (std::size_t k = 0; k < order.size() && k < o.max_levels; k++) lv.push_back(col.levels[static_cast<std::size_t>(order[k].second)]);
      if (lv.size() < 2) continue;  // a category with one level says nothing
      for (const auto& l : lv) {
        Condition c;
        c.attribute = s.attributes[a], c.op = groups::Op::eq, c.levels = {l};
        s.conditions.push_back(c), s.attribute_of.push_back(a);
      }
    } else {
      std::vector<double> v;
      for (std::size_t i = 0; i < t.rows; i++)
        if (parts.part_of_row[i] == 0 && std::isfinite(col.numbers[i])) v.push_back(col.numbers[i]);
      std::sort(v.begin(), v.end());
      std::vector<double> distinct = v;
      distinct.erase(std::unique(distinct.begin(), distinct.end()), distinct.end());
      if (distinct.size() < 2) continue;
      std::vector<double> cuts;
      if (distinct.size() <= o.distinct) cuts.assign(distinct.begin(), distinct.end() - 1);  // x ≤ c for every value but the last
      else
        for (std::size_t q = 1; q <= o.quantiles; q++) {
          // A quantile, to four significant figures, so that a statement reads as a person would write it.
          const double x = v[std::min(v.size() - 1, q * v.size() / (o.quantiles + 1))];
          const int e = x == 0 ? 0 : static_cast<int>(std::floor(std::log10(std::fabs(x)))) - 3;
          // Divided by an exact power of ten, so that the cut is the double nearest the decimal and is written as it.
          if (e >= 0) cuts.push_back(std::round(x / std::pow(10.0, e)) * std::pow(10.0, e));
          else cuts.push_back(std::round(x * std::pow(10.0, -e)) / std::pow(10.0, -e));
        }
      std::sort(cuts.begin(), cuts.end());
      cuts.erase(std::unique(cuts.begin(), cuts.end()), cuts.end());
      if (!cuts.empty() && cuts.back() >= distinct.back()) cuts.pop_back();
      s.cuts[s.attributes[a]] = cuts;
      for (double c0 : cuts)
        for (groups::Op op : {groups::Op::le, groups::Op::gt}) {
          Condition c;
          c.attribute = s.attributes[a], c.op = op, c.number = c0;
          s.conditions.push_back(c), s.attribute_of.push_back(a);
        }
    }
  }
  if (s.conditions.empty()) throw Refusal("no attribute gives a condition: groups cannot be stated");
  s.rows.reserve(s.conditions.size());
  for (const auto& c : s.conditions) {
    Group g;
    g.conditions = {c};
    s.rows.push_back(groups::rows_of(t, g));
  }
  // The people of the discovery part, and each indicator's spread over its clusters (to read gaps as effect sizes).
  double people = 0;
  for (const auto& [k, ind] : d.indicators) {
    Statement st;
    st.domain = s.domain(k), st.indicator = k, st.standard = Standard::normative, st.level = 0, st.worse_when_higher = ind.higher_is_worse;
    const auto T = evidence::totals(d, st, parts, Part::discovery);
    double m = 0, n = 0, sy = 0, sx = 0;
    for (std::size_t c = 0; c < T.yg.size(); c++) sy += T.yg[c], sx += T.xg[c];
    const double mean = sx > 0 ? sy / sx : 0;
    double ss = 0;
    for (std::size_t c = 0; c < T.yg.size(); c++)
      if (T.xg[c] > 0) {
        const double v = T.yg[c] / T.xg[c];
        ss += T.xg[c] * (v - mean) * (v - mean), n += T.xg[c];
      }
    s.spread[k] = n > 0 && ss > 0 ? std::sqrt(ss / n) : 1.0;
    people = std::max(people, sx);
    (void)m;
  }
  s.min_coverage = o.min_coverage > 0 ? o.min_coverage : 0.01 * people;
  return s;
}

// A statement in its plainest form: comparative on an indicator, in its direction, in its usual domain.
inline Statement plain(const Space& sp, const Dataset& d, const Group& g, const std::string& indicator) {
  Statement s;
  s.indicator = indicator, s.domain = sp.domain(indicator), s.group = g;
  s.standard = Standard::comparative, s.worse_when_higher = d.indicator(indicator).higher_is_worse;
  return s;
}

inline std::size_t size(const Statement& s) { return s.group.conditions.size() + s.frame.conditions.size(); }

// Whether a group uses an attribute twice (the pool's conjunctions are on distinct attributes).
inline bool distinct_attributes(const Group& g) {
  std::set<std::string> a;
  for (const auto& c : g.conditions)
    if (!a.insert(c.attribute).second) return false;
  return true;
}

}  // namespace needs::search
