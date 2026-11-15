// From there to here: whether an intervention shown to work in the studies will work in a place, and what effect to expect there.
//
// Cartwright and Hardie's argument (2012; tekmerion's warrant) needs three premises: it worked there; the same causal principle
// holds here; the support factors it needs are present here. A theory of change says which support factors: those of the links on
// the path by which it is meant to work (policy::change). A place states, for each, whether it is present, absent or not known,
// and whether the causal principle is the same. The argument is then made, every premise without evidence named.
//
// The effect to expect here is a separate question. Where the effect varies with a factor Z whose distribution differs between the
// studies and the place, and Z is all the difference that matters, the studies' effect within each stratum of Z reweighted by the
// place's distribution of Z is the effect here (the transport formula; Pearl and Bareinboim 2011, Transportability of causal and
// statistical relations, AAAI; peira's `transport`, which refuses when a selection diagram shows Z not to be S-admissible). Without
// strata the pooled effect is carried over as it stands, and that is said: it is what the second premise asserts.

#pragma once

#include <policy/change/change.hpp>

#include <peira/synthesis/synthesis.hpp>
#include <tekmerion/warrant/transfer.hpp>

#include <map>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace policy::transfer {

using apodeixis::kernel::Verdict;
using tekmerion::warrant::Support;

struct Strata {
  std::vector<std::string> names;
  std::vector<double> effects, variances;  // the studies' effect within each stratum, and its sampling variance
  std::vector<double> place;               // the place's distribution over the strata (masses, normalised)
};

struct Place {
  std::string name;
  Support same_principle = Support::unknown;
  std::map<std::string, Support> factors;  // each support factor named by the theory: present, absent or not known here
  std::optional<Strata> strata;
};

struct Here {
  tekmerion::warrant::HereAnswer argument;
  std::vector<std::string> needed, present, absent, unknown;  // the support factors of the path, by their state here
  double effect = 0, lower = 0, upper = 0;                     // to expect here
  bool transported = false;
  Verdict verdict;
};

inline std::string state(Support s) { return s == Support::stated ? "present" : s == Support::against ? "absent" : "not known"; }

// The argument for the place, on the pooled evidence `there` of the intervention's effect on the outcome, and the path of the
// theory (`path`, the support factors it needs: policy::change's strongest path, or any other the caller chooses).
inline Here here(const change::Theory& t, const change::Path& path, const peira::synthesis::Pooled& there, const Place& p, bool higher_is_better = true) {
  Here h;
  for (std::size_t e : path.links) {
    if (e >= t.links.size()) throw std::invalid_argument("policy::transfer: the path names a link the theory does not have");
    for (const auto& f : t.links[e].support)
      if (std::find(h.needed.begin(), h.needed.end(), f) == h.needed.end()) h.needed.push_back(f);
  }
  for (const auto& [f, s] : p.factors)
    if (std::find(h.needed.begin(), h.needed.end(), f) == h.needed.end()) throw std::invalid_argument("policy::transfer: the place states " + f + ", which the path does not need");
  for (const auto& f : h.needed) {
    const auto it = p.factors.find(f);
    const Support s = it == p.factors.end() ? Support::unknown : it->second;
    (s == Support::stated ? h.present : s == Support::against ? h.absent : h.unknown).push_back(f);
  }
  const Support factors = !h.absent.empty() ? Support::against : !h.unknown.empty() ? Support::unknown : Support::stated;
  tekmerion::warrant::HereCase c{there, higher_is_better, p.same_principle, factors, {}};
  for (const auto& f : h.needed) {
    const auto it = p.factors.find(f);
    c.notes.push_back(f + ": " + state(it == p.factors.end() ? Support::unknown : it->second) + " here");
  }
  h.argument = tekmerion::warrant::will_it_work_here(c);

  if (p.strata) {
    const auto& z = *p.strata;
    if (z.names.size() != z.effects.size()) throw std::invalid_argument("policy::transfer: a name for each stratum");
    const auto tr = peira::synthesis::transport(z.effects, z.variances, z.place);
    h.effect = tr.effect, h.lower = tr.lower, h.upper = tr.upper, h.transported = true;
  } else {
    h.effect = there.estimate, h.lower = there.lower, h.upper = there.upper;
  }

  const std::string place = p.name.empty() ? "here" : "in " + p.name;
  const std::string claim = "the intervention will work " + place;
  const std::string expect = "the effect to expect " + place + " is " + change::number(h.effect) + " [" + change::number(h.lower) + ", " + change::number(h.upper) + "]" +
                             (h.transported ? ", the studies' strata reweighted by the place's" : ", the studies' pooled effect carried over as the second premise asserts");
  const auto& v = h.argument.verdict;
  std::string about;
  if (!h.absent.empty()) {
    about = "absent here: ";
    for (std::size_t i = 0; i < h.absent.size(); i++) about += (i ? "; " : "") + h.absent[i];
  }
  if (!h.unknown.empty()) {
    about += std::string(about.empty() ? "" : ". ") + "not known here: ";
    for (std::size_t i = 0; i < h.unknown.size(); i++) about += (i ? "; " : "") + h.unknown[i];
  }
  const std::string reason = v.reason + (about.empty() ? "" : ". " + about) + ". " + expect;
  h.verdict = v.holds() ? Verdict::yes(claim, reason) : v.fails() ? Verdict::no(claim, reason) : Verdict::unknown(claim, reason);
  for (const auto& f : h.absent) h.verdict.with("absent", f);
  for (const auto& f : h.unknown) h.verdict.with("not known", f);
  for (const auto& m : h.argument.missing) h.verdict.with("missing", m);
  return h;
}

}  // namespace policy::transfer
