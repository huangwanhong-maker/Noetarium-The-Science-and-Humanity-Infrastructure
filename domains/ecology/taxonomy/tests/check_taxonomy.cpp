// Checks of ecology/taxonomy: tallies at each rank with the unknown counted apart and weights; a species given two genera
// refused; the path weights; Δ⁺, Λ⁺ and Δ* by hand on a small classification; Δ⁺ of every subset of a given size averaging
// to the master list's Δ⁺ (enumerated exactly), and the funnel's mean near it.

#include <ecology/taxonomy/taxonomy.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace ecology::taxonomy;

int main() {
  calx::verification::Suite s;

  // Five trees: two London planes, one Oriental plane (one genus), one elm (another family), one eucalypt known only to genus.
  const std::vector<Taxon> trees{
      {"Platanus x acerifolia", "Platanus", "Platanaceae"}, {"Platanus x acerifolia", "Platanus", "Platanaceae"},
      {"Platanus orientalis", "Platanus", "Platanaceae"},   {"Ulmus procera", "Ulmus", "Ulmaceae"},
      {"", "Eucalyptus", "Myrtaceae"}};
  {
    const auto sp = tally(trees, Rank::species);
    s.require("tally: species in decreasing count, the unknown apart", sp.names.size() == 3 && sp.names[0] == "Platanus x acerifolia" && sp.counts[0] == 2 && sp.unknown == 1 && sp.known == 4);
    s.check("tally: the most common species holds half the known", sp.share(0) - 0.5, 1e-15);
    const auto ge = tally(trees, Rank::genus);
    s.require("tally: genera, none unknown", ge.names[0] == "Platanus" && ge.counts[0] == 3 && ge.unknown == 0 && ge.known == 5);
    const auto fa = tally(trees, Rank::family, {1, 1, 1, 2, 1});
    s.require("tally: weights count an individual several times", fa.names[0] == "Platanaceae" && fa.counts[0] == 3 && fa.counts[1] == 2 && fa.known == 6);
    bool refused = false;
    try {
      (void)species_list({{"Platanus orientalis", "Platanus", "Platanaceae"}, {"Platanus orientalis", "Acer", "Sapindaceae"}});
    } catch (const std::invalid_argument&) { refused = true; }
    s.require("species list: a species given two genera is refused", refused);
  }

  // Four species: A, B in genus G1 (family F1), C in genus G2 (family F1), D in genus G3 (family F2).
  SpeciesList L;
  L.species = {"A", "B", "C", "D"};
  L.genus = {"G1", "G1", "G2", "G3"};
  L.family = {"F1", "F1", "F1", "F2"};
  L.abundance = {3, 1, 2, 4};
  {
    s.check("ω: one genus is 100/3", path_weight(L, 0, 1) - 100.0 / 3, 1e-12);
    s.check("ω: one family is 200/3", path_weight(L, 0, 2) - 200.0 / 3, 1e-12);
    s.check("ω: different families is 100", path_weight(L, 0, 3) - 100, 1e-12);
    // Pairs: AB 100/3, AC 200/3, AD 100, BC 200/3, BD 100, CD 100: Σ = 100/3 + 400/3 + 300 = 1400/3; six pairs.
    const auto d = distinctness(L);
    const double avg = 1400.0 / 3 / 6;
    s.check("Δ⁺ by hand: 1400/18", d.average - avg, 1e-12);
    double var = 0;
    for (double w : {100.0 / 3, 200.0 / 3, 100.0, 200.0 / 3, 100.0, 100.0}) var += (w - avg) * (w - avg);
    s.check("Λ⁺ by hand", d.variation - var / 6, 1e-10);
    // Δ*: Σ ω x_i x_j / Σ x_i x_j with x = (3, 1, 2, 4).
    const double num = 100.0 / 3 * 3 + 200.0 / 3 * 6 + 100.0 * 12 + 200.0 / 3 * 2 + 100.0 * 4 + 100.0 * 8;
    const double den = 3 + 6 + 12 + 2 + 4 + 8;
    s.check("Δ* by hand", d.weighted - num / den, 1e-10);
  }

  // A master list of eight species; every subset of 3 enumerated: the mean Δ⁺ equals the master's.
  {
    SpeciesList M;
    M.species = {"a", "b", "c", "d", "e", "f", "g", "h"};
    M.genus = {"g1", "g1", "g1", "g2", "g2", "g3", "g4", "g5"};
    M.family = {"f1", "f1", "f1", "f1", "f1", "f2", "f2", "f3"};
    M.abundance.assign(8, 1);
    const double master = distinctness(M).average;
    double sum = 0;
    int count = 0;
    for (std::size_t a = 0; a < 8; a++)
      for (std::size_t b = a + 1; b < 8; b++)
        for (std::size_t c = b + 1; c < 8; c++) {
          SpeciesList sub;
          for (std::size_t k : {a, b, c}) {
            sub.species.push_back(M.species[k]);
            sub.genus.push_back(M.genus[k]);
            sub.family.push_back(M.family[k]);
            sub.abundance.push_back(1);
          }
          sum += distinctness(sub).average;
          count++;
        }
    s.check("Δ⁺: the mean over all 56 subsets of 3 equals the master list's (exact)", sum / count - master, 1e-12);
    const auto f = funnel(M, {3, 6}, 20000, 7);
    s.check("funnel: the simulated mean at s = 3 near the master's Δ⁺", f[0].mean - master, 0.5);
    s.require("funnel: the band narrows as s grows", (f[1].upper - f[1].lower) < (f[0].upper - f[0].lower));
    s.require("funnel: the band holds the master's Δ⁺", f[0].lower <= master && master <= f[0].upper);
  }

  return s.done();
}
