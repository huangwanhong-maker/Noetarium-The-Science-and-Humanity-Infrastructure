// Checks of economics::diversification on tables worked by hand: two references whose regions' shift-share terms sum to each region's
// change and whose competitive terms sum to nought within each reference; variety in bits summing; opportunities among the industries
// a region does not hold, by density; withheld cells read as nothing and counted; entries placed where density is highest found with an
// area under the curve of one; a table that falls apart leaving the complexity undefined with its reason; refusals.

#include <economics/diversification/structure.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>
#include <vector>

using namespace economics::diversification;

namespace {
template <class F>
bool refused(F f) {
  try {
    f();
  } catch (const std::invalid_argument&) {
    return true;
  }
  return false;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  calx::random::Stream g(20261123, 31, 32, 33);
  Table T;
  T.regions = {"r1", "r2", "r3", "r4", "r5", "r6"};
  T.references = {"A", "A", "A", "B", "B", "B"};
  T.industries = {"C10", "C11", "C20", "G46", "G47", "J62"};
  T.groups = {"C", "C", "C", "G", "G", "J"};
  for (int r = 0; r < 6; r++) {
    T.before.emplace_back(), T.after.emplace_back();
    for (int p = 0; p < 6; p++) T.before.back().push_back(1 + std::floor(g.uniform(0, 50))), T.after.back().push_back(1 + std::floor(g.uniform(0, 60)));
  }
  T.before[2][4] = unknown;
  const auto A = assess(T);
  {
    double worst = 0;
    std::map<std::string, double> comp;
    for (const auto& G : A.regions) worst = std::max(worst, std::fabs(G.national + G.mix + G.competitive - (G.total_after - G.total_before))), comp[G.reference] += G.competitive;
    s.check("shift-share: each region's national share, mix and competitive terms sum to its change", worst, 1e-9);
    s.check("... and the competitive terms of a reference's regions sum to nought (the reference is their sum)", std::fabs(comp["A"]) + std::fabs(comp["B"]), 1e-9);
  }
  {
    double worst = 0;
    for (const auto& G : A.regions) worst = std::max(worst, std::fabs(G.variety_after.unrelated + G.variety_after.related - G.variety_after.entropy));
    s.check("variety in bits: unrelated (among sections) and related (within) sum to the entropy, at most log₂ 6", worst, 1e-12);
    bool bounded = true;
    for (const auto& G : A.regions) bounded = bounded && G.variety_after.entropy <= std::log2(6.0) + 1e-12;
    s.require("... the entropy at most log₂ 6", bounded);
  }
  {
    s.require("a withheld cell is counted and read as nothing", A.regions[2].withheld_before == 1 && A.regions[0].withheld_before == 0);
    bool ok = true;
    for (const auto& G : A.regions)
      for (std::size_t k = 1; k < G.opportunities.size(); k++) ok = ok && G.opportunities[k - 1].second >= G.opportunities[k].second;
    s.require("opportunities are sorted by density, at most three a region", ok && A.regions[0].opportunities.size() <= 3);
  }
  {
    // Entries placed where density is highest: r0 holds C10 and C11 and enters C20, the industry nearest them; others stay.
    Table E;
    E.regions = {"a", "b", "c", "d"};
    E.references = {"X", "X", "X", "X"};
    E.industries = {"p", "q", "s", "t"};
    E.groups = {"P", "P", "P", "T"};
    E.before = {{10, 10, 0, 1}, {10, 10, 10, 1}, {10, 10, 10, 1}, {0, 0, 0, 30}};
    E.after = {{10, 10, 10, 1}, {10, 10, 10, 1}, {10, 10, 10, 1}, {0, 0, 0, 30}};
    const auto B = assess(E);
    s.require("an entry where density is highest: one entry, its density above every candidate that did not enter (AUC 1)", B.entries == 1 && B.entry.auc == 1,
              "entries " + std::to_string(B.entries) + ", AUC " + std::to_string(B.entry.auc));
    s.require("... a table that falls apart into two blocks leaves the complexity index undefined, with the reason said",
              !B.complexity_after && std::isnan(B.regions[0].complexity_after) && B.complexity_note.find("blocks") != std::string::npos);
  }
  {
    Table bad = T;
    bad.groups.pop_back();
    s.require("an industry without a group is refused", refused([&] { assess(bad); }));
    Table small = T;
    small.regions.resize(2), small.references.resize(2), small.before.resize(2), small.after.resize(2);
    s.require("fewer than three regions is refused", refused([&] { assess(small); }));
  }
  return s.done();
}
