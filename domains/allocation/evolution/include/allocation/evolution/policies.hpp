// Rules learned on the allocation domain's own problems (ADR 0141).
//
// heuresis's policies (its decision 0011) decide each grant from the state; here the state and the grants are a composition's. A
// case is a problem with its requests fixed (what each recipient asks of each resource) and the worth of where its grants end; a
// rule is a score over the features of granting a recipient at a state, followed through the composition's grant system, so that
// it decides the order of grants, and with it who is met in full and who in part. The features are declared:
//   attributes  each named attribute of the recipient, over its mean among the recipients (so that a rule learned on one city
//               reads another's on the same scale);
//   requests    for each resource, the recipient's request over what remains of the pool;
//   remaining   for each resource, the share of the pool that remains;
//   scalars     those of the problem's own quantities named by index (a price that rises with each grant, say), as the state holds
//               them.
// A rule learned once over a family of cases is set beside the order searched for each case and each case's best order: what a
// rule can claim is its worth on cases it was not trained on, relative to theirs.
#pragma once

#include <allocation/evolution/problem.hpp>

#include <heuresis/sequential/policies.hpp>

#include <memory>
#include <string>
#include <vector>

namespace allocation::evolution {

struct PolicyCase {
  std::shared_ptr<const Composition> composition;
  std::vector<std::vector<double>> requests;     // [resource][recipient]
  std::function<double(const State&)> worth;     // maximised
};

struct PolicyFeatures {
  std::vector<std::string> attributes;
  bool requests = true, remaining = true;
  std::vector<std::size_t> scalars;  // indices into the problem's own quantities
};

inline std::size_t feature_count(const PolicyCase& c, const PolicyFeatures& f) {
  const auto& p = c.composition->problem();
  return 1 + f.attributes.size() + (f.requests ? p.resources.size() : 0) + (f.remaining ? p.resources.size() : 0) + f.scalars.size();
}

inline hq::Instance<State> policy_instance(const PolicyCase& c, const PolicyFeatures& f) {
  if (!c.composition || !c.worth) throw Refusal("a case for a rule needs its composition and its worth");
  const auto& p = c.composition->problem();
  const std::size_t n = p.recipients.size(), R = p.resources.size();
  if (c.requests.size() != R) throw Refusal("a request for each resource");
  std::vector<std::vector<double>> attr(f.attributes.size(), std::vector<double>(n));
  for (std::size_t a = 0; a < f.attributes.size(); a++) {
    double mean = 0;
    for (std::size_t i = 0; i < n; i++) {
      const auto it = p.recipients[i].at.find(f.attributes[a]);
      if (it == p.recipients[i].at.end()) throw Refusal("the recipient " + p.recipients[i].id + " has no attribute " + f.attributes[a]);
      attr[a][i] = it->second, mean += it->second / static_cast<double>(n);
    }
    for (auto& v : attr[a]) v = mean != 0 ? v / mean : v;
  }
  for (std::size_t k : f.scalars)
    if (k >= p.scalars.size()) throw Refusal("a rule reads a quantity the problem does not have");
  std::vector<double> pool(R, 0);
  const State s0 = c.composition->grant_system(c.requests, false).initial;
  for (std::size_t r = 0; r < R; r++) pool[r] = s0.remaining[r];
  hq::Instance<State> in;
  in.system = c.composition->grant_system(c.requests, false);
  const auto requests = c.requests;
  in.features = [f, attr, requests, pool, R](std::size_t i, const State& s) {
    std::vector<double> x{1.0};
    for (const auto& a : attr) x.push_back(a[i]);
    if (f.requests)
      for (std::size_t r = 0; r < R; r++) x.push_back(s.remaining[r] > 0 ? requests[r][i] / s.remaining[r] : 0.0);
    if (f.remaining)
      for (std::size_t r = 0; r < R; r++) x.push_back(pool[r] > 0 ? s.remaining[r] / pool[r] : 0.0);
    for (std::size_t k : f.scalars) x.push_back(k < s.scalars.size() ? s.scalars[k] : 0.0);
    return x;
  };
  in.worth = c.worth;
  return in;
}

inline std::vector<hq::Instance<State>> policy_instances(const std::vector<PolicyCase>& cases, const PolicyFeatures& f) {
  std::vector<hq::Instance<State>> out;
  for (const auto& c : cases) out.push_back(policy_instance(c, f));
  return out;
}

}  // namespace allocation::evolution
