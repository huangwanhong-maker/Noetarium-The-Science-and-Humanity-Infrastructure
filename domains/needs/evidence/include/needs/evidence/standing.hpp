// The standing of a statement (ADR 0125; the theory note Definition 3), decided by an argumentation framework over its tests.
//
// The claim that the group has the need, and the objections the tests raise against it, are arguments of a Dung framework
// (apodeixis's argumentation), and the claim's standing is read from the grounded extension:
//   N    the group has the need in the domain, by the standard;
//   D    the gap does not clear the threshold on the discovery part            attacks N
//   V    the validation part, consulted through the reusable holdout, does not uphold it   attacks N
//   S    the gap comes and goes under resampling of clusters                    attacks N
//   C    the confirmation part confirms it, at the online procedure's level      attacks V and S (fresh data outweigh screens)
//   X    the confirmation part shows the opposite gap                           attacks N and C
// robust: N and C are in; supported: N is in without C (not confirmed, or not yet tested); unsupported: N is out and X is not in;
// refuted: X is in. What causal support, the simulation and the rules say does not attack the claim (a need may exist that
// provision would not meet, that a mechanism does not explain, or that no rule recognises); it qualifies it, and is said beside it.
#pragma once

#include <needs/evidence/causal.hpp>

#include <apodeixis/argumentation/frameworks.hpp>

#include <string>
#include <vector>

namespace needs::evidence {

enum class Standing { robust, supported, unsupported, refuted };

inline const char* name(Standing s) {
  switch (s) {
    case Standing::robust: return "robust";
    case Standing::supported: return "supported";
    case Standing::unsupported: return "unsupported";
    default: return "refuted";
  }
}

struct Findings {
  bool discovery = false;            // clears the threshold on discovery
  bool validation_tested = false, validation_upheld = true;
  bool stability_tested = false, stable = true;
  bool confirmation_tested = false, confirmed = false, opposite = false;
  std::vector<std::string> qualifiers;
};

struct Argument {
  std::string id, text, label;       // label: in, out, undecided
};

struct Judgement {
  Standing standing = Standing::supported;
  std::vector<Argument> arguments;
  std::vector<std::string> qualifiers;
};

inline Judgement standing(const Findings& f) {
  std::vector<Argument> args{{"N", "the group has the need", ""}};
  std::vector<std::array<std::size_t, 2>> attacks;
  auto add = [&](const char* id, const char* text) {
    args.push_back({id, text, ""});
    return args.size() - 1;
  };
  std::size_t V = 0, S = 0, C = 0, X = 0;
  if (!f.discovery) attacks.push_back({add("D", "the gap does not clear the threshold on the discovery part"), 0});
  if (f.validation_tested && !f.validation_upheld) V = add("V", "the validation part does not uphold it"), attacks.push_back({V, 0});
  if (f.stability_tested && !f.stable) S = add("S", "the gap comes and goes under resampling"), attacks.push_back({S, 0});
  if (f.confirmation_tested && f.confirmed && !f.opposite) {
    C = add("C", "the confirmation part confirms it");
    if (V) attacks.push_back({C, V});
    if (S) attacks.push_back({C, S});
  }
  if (f.confirmation_tested && f.opposite) {
    X = add("X", "the confirmation part shows the opposite gap");
    attacks.push_back({X, 0});
    if (C) attacks.push_back({X, C});
  }
  apodeixis::argumentation::Framework F(args.size(), attacks);
  const auto g = apodeixis::argumentation::grounded(F);
  for (std::size_t k = 0; k < args.size(); k++)
    args[k].label = g.label[k] == apodeixis::argumentation::Label::in ? "in" : g.label[k] == apodeixis::argumentation::Label::out ? "out" : "undecided";
  Judgement j;
  const bool n_in = g.label[0] == apodeixis::argumentation::Label::in;
  if (X && g.label[X] == apodeixis::argumentation::Label::in) j.standing = Standing::refuted;
  else if (n_in && C && g.label[C] == apodeixis::argumentation::Label::in) j.standing = Standing::robust;
  else if (n_in) j.standing = Standing::supported;
  else j.standing = Standing::unsupported;
  j.arguments = std::move(args);
  j.qualifiers = f.qualifiers;
  return j;
}

}  // namespace needs::evidence
