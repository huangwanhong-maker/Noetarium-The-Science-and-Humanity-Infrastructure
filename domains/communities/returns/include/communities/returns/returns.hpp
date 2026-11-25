// Returns: what is owed back for contributions, by declared rules, kept apart from the record of who contributed (the design
// record of 2026-11-25, Section 7.3).
//
// Proposition 7: let the graph of use be acyclic and let each artefact j pass a share a(i, j) ≥ 0 of what reaches it to each input
// i, the shares of j adding to at most one. Then what is kept along the graph adds to the pot, and what reaches the artefacts is
// (I − A)⁻¹ applied to the pot. A return owed to an operative is paid to the party that answers for it, its origin under control,
// since an operative holds no account of its own.

#pragma once

#include <thesmos/kernel/kernel.hpp>

#include <morphe/arithmetic/rational.hpp>

#include <algorithm>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

namespace communities::returns {

using morphe::arithmetic::Rational;

// What each artefact keeps when `pot` reaches the artefacts named and each passes its declared shares upstream. `order` lists the
// artefacts so that every input comes before what uses it.
inline std::map<std::string, Rational> upstream(const std::vector<std::string>& order, const std::map<std::string, std::map<std::string, Rational>>& passes,
                                                const std::map<std::string, Rational>& pot) {
  std::map<std::string, std::size_t> place;
  for (std::size_t k = 0; k < order.size(); k++) place[order[k]] = k;
  std::map<std::string, Rational> reaching, kept;
  for (const auto& name : order) reaching[name] = Rational(0);
  for (const auto& [name, amount] : pot) reaching.at(name) = reaching.at(name) + amount;
  for (auto it = order.rbegin(); it != order.rend(); ++it) {
    const std::string& name = *it;
    Rational passed(0);
    const auto out = passes.find(name);
    if (out != passes.end())
      for (const auto& [source, share] : out->second) {
        if (place.at(source) >= place.at(name)) throw std::invalid_argument("communities::returns: an input must come before what uses it");
        passed = passed + share;
      }
    if (passed > Rational(1)) throw std::invalid_argument("communities::returns: an artefact passes on more than reaches it");
    kept[name] = reaching[name] * (Rational(1) - passed);
    if (out != passes.end())
      for (const auto& [source, share] : out->second) reaching[source] = reaching[source] + reaching[name] * share;
  }
  return kept;
}

// A pot divided by declared weights (a return by agreement or by role).
inline std::map<std::string, Rational> by_shares(const Rational& pot, const std::map<std::string, Rational>& weights) {
  Rational total(0);
  for (const auto& [party, w] : weights) total = total + w;
  if (total == Rational(0)) throw std::invalid_argument("communities::returns: no weight is declared");
  std::map<std::string, Rational> out;
  for (const auto& [party, w] : weights) out[party] = pot * w / total;
  return out;
}

// Returns owed to parties, paid to the parties that answer for them: an operative's return goes to its origin under control.
inline std::map<std::string, Rational> to_answerable(const thesmos::kernel::Parties& ps, const std::map<std::string, Rational>& owed) {
  std::map<std::string, Rational> out;
  for (const auto& [party, amount] : owed) {
    const std::string payee = thesmos::kernel::origin(ps, party);
    out[payee] = (out.count(payee) ? out[payee] : Rational(0)) + amount;
  }
  return out;
}

}  // namespace communities::returns
