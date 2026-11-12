// A rule as a scheme states it, and what a design is found to do against it (ADR 0095).
//
// A LIMIT is a bound a scheme declares: at least, at most, or both, with the clause that states it. A limit with neither bound is not
// stated, and a rule that is not stated is not evaluated: nothing here is any jurisdiction's until a scheme says so.
//
// A FINDING is one rule met or not by one subject (a building, a storey, a room, a door): what was required, what was found, in what
// unit, and a note where the finding rests on an approximation or on something not drawn. A design is reported rule by rule; no
// rule is folded into a verdict.

#pragma once

#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace architecture::rules {

struct Limit {
  double least = std::numeric_limits<double>::quiet_NaN();
  double most = std::numeric_limits<double>::quiet_NaN();
  std::string clause;
  bool stated() const { return !std::isnan(least) || !std::isnan(most); }
  bool holds(double x, double tolerance = 1e-9) const {
    return (std::isnan(least) || x >= least - tolerance) && (std::isnan(most) || x <= most + tolerance);
  }
  static Limit at_least(double v, std::string c = {}) { return {v, std::numeric_limits<double>::quiet_NaN(), std::move(c)}; }
  static Limit at_most(double v, std::string c = {}) { return {std::numeric_limits<double>::quiet_NaN(), v, std::move(c)}; }
  static Limit between(double a, double b, std::string c = {}) { return {a, b, std::move(c)}; }
};

struct Finding {
  std::string rule;      // the rule's id, fixed by the domain: "height", "light", ...
  std::string title;     // in words
  std::string clause;    // as the scheme cites it
  std::string subject;   // what it is found of: "the building", "Bedroom 2", "storey 1"
  double least = std::numeric_limits<double>::quiet_NaN(), most = std::numeric_limits<double>::quiet_NaN();
  double actual = std::numeric_limits<double>::quiet_NaN();
  std::string unit;      // "m", "m²", "share", "count"
  bool ok = true;
  std::string note;
};

inline Finding found(std::string rule, std::string title, std::string subject, const Limit& limit, double actual, std::string unit, std::string note = {},
                     double tolerance = 1e-9) {
  Finding f;
  f.rule = std::move(rule), f.title = std::move(title), f.subject = std::move(subject), f.clause = limit.clause;
  f.least = limit.least, f.most = limit.most, f.actual = actual, f.unit = std::move(unit), f.note = std::move(note);
  f.ok = limit.holds(actual, tolerance);
  return f;
}

}  // namespace architecture::rules
