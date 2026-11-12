// Checks of ecology/forest: allometry against Jenkins et al.'s (2003) equations evaluated by hand, the open-grown factor, the
// roots' ratio, carbon and CO₂; the most specific match winning and a taxon without a group refused; composition against
// Santamour's limits found rank by rank; size classes; increments as medians of D/age; the stock as the sum of estimates; the
// forest's future (expected standing in closed form for stated lives and for a hazard; its variance against a simulation of
// removals; planting in closed form; the planting that holds carbon restoring it exactly); the stage model's equilibrium
// against the future's long run; crowns on a grid against the areas of a disc and of two overlapping discs; exposure.

#include <ecology/forest/allometry.hpp>
#include <ecology/forest/canopy.hpp>
#include <ecology/forest/future.hpp>
#include <ecology/forest/inventory.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <numbers>
#include <vector>

using namespace ecology::forest;
using ecology::taxonomy::Rank;

namespace {

// Two groups of Jenkins et al. (2003): Table 4's mixed hardwood and pine, with Table 6's coarse-root ratios.
Scheme jenkins_two() {
  Scheme s;
  s.name = "test";
  s.carbon_fraction = 0.5;
  s.open_grown = 0.8;
  Group mixed;
  mixed.name = "Mixed hardwood";
  mixed.aboveground = Power{-2.4800, 2.4835, 1};
  mixed.roots = Ratio{-1.6911, 0.8160};
  mixed.d_max = 56;
  Group pine;
  pine.name = "Pine";
  pine.aboveground = Power{-2.5356, 2.4349, 1};
  pine.roots = Ratio{-1.5619, 0.6614};
  pine.crown = Power{0.0, 0.5, 1};  // a made-up crown law for the checks: D^0.5 m
  pine.d_max = 180;
  const auto m = s.add(mixed), p = s.add(pine);
  s.by_genus["Platanus"] = m;
  s.by_genus["Pinus"] = p;
  s.by_species["Platanus orientalis"] = p;  // deliberately odd, to see the species win over the genus
  s.by_family["Ulmaceae"] = m;
  return s;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Scheme sch = jenkins_two();

  // ---- allometry ------------------------------------------------------------------------------------------
  {
    const Taxon plane{"Platanus x acerifolia", "Platanus", "Platanaceae"};
    const auto e = estimate(sch, plane, 30);
    const double ag = 0.8 * std::exp(-2.4800 + 2.4835 * std::log(30.0));
    const double roots = ag * std::exp(-1.6911 + 0.8160 / 30);
    s.check("allometry: above-ground = 0.8 exp(β₀ + β₁ ln D) (relative)", e.aboveground / ag - 1, 1e-14);
    s.check("allometry: roots = ratio × above-ground (relative)", e.roots / roots - 1, 1e-14);
    s.check("allometry: carbon = 0.5 × biomass", e.carbon - 0.5 * (ag + roots), 1e-9);
    s.check("allometry: CO₂ = 44/12 × carbon", e.co2e - e.carbon * 44 / 12, 1e-9);
    s.require("allometry: matched at the genus", e.assignment.match == Match::genus && e.assignment.group == 0 && !e.outside_range);
    s.require("allometry: no crown stated, none estimated", std::isnan(e.crown_area));
    s.require("allometry: beyond the source's range is said to be", estimate(sch, plane, 80).outside_range);
    Scheme capped = sch;
    capped.cap_beyond_range = true;
    s.check("allometry: a capped scheme estimates beyond the range at d_max", estimate(capped, plane, 80).aboveground - estimate(sch, plane, 56).aboveground, 1e-9);
    s.require("allometry: and still says it was beyond", estimate(capped, plane, 80).outside_range);
    const auto o = estimate(sch, {"Platanus orientalis", "Platanus", "Platanaceae"}, 30);
    s.require("allometry: the species wins over its genus", o.assignment.match == Match::species && o.assignment.group == 1);
    s.check("allometry: a stated crown law gives the crown's area", o.crown_area - std::numbers::pi * 30 / 4, 1e-12);
    const auto elm = estimate(sch, {"Ulmus procera", "Ulmus", "Ulmaceae"}, 30);
    s.require("allometry: the family when no species or genus matches", elm.assignment.match == Match::family);
    bool refused = false;
    try { (void)estimate(sch, {"Eucalyptus camaldulensis", "Eucalyptus", "Myrtaceae"}, 30); } catch (const std::invalid_argument&) { refused = true; }
    s.require("allometry: no group and no fallback is refused", refused);
    s.check("sequestration: C(D + Δ) − C(D)", sequestration(sch, plane, 30, 0.8) - (estimate(sch, plane, 30.8).carbon - e.carbon), 1e-12);
  }

  // ---- an inventory -----------------------------------------------------------------------------------------
  std::vector<Tree> trees;
  {
    const Taxon a{"Platanus x acerifolia", "Platanus", "Platanaceae"}, b{"Platanus orientalis", "Platanus", "Platanaceae"},
        c{"Ulmus procera", "Ulmus", "Ulmaceae"}, d{"Pinus radiata", "Pinus", "Pinaceae"}, u{"", "Pinus", "Pinaceae"};
    const std::vector<Taxon> taxa{a, a, a, b, c, c, d, d, d, u};
    const std::vector<double> dbh{12, 25, 40, 33, 18, 60, 22, 47, 55, 9};
    const std::vector<double> age{6, 20, 40, NAN, 10, 50, 11, 38, 45, 4};
    for (std::size_t i = 0; i < taxa.size(); i++) {
      Tree t;
      t.taxon = taxa[i];
      t.dbh = dbh[i];
      t.age = age[i];
      t.area = i < 5 ? 0 : 1;
      trees.push_back(t);
    }
    const auto comp = composition(trees);
    const auto f = findings(comp, Limits{});
    // Species: 9 named; Platanus x acerifolia 3/9 and Pinus radiata 3/9 exceed 10%, as do the others (1/9, 2/9).
    s.require("limits: the species rank fails, every species above 10%", !f[0].holds && f[0].over.size() == 4 && f[0].unnamed == 1);
    s.check("limits: the largest species' share is 3/9", f[0].share - 3.0 / 9, 1e-15);
    // Genus: Platanus 4/10, Pinus 4/10, Ulmus 2/10: all above 20% but Ulmus (exactly 20% holds).
    s.require("limits: the genus rank fails for Platanus and Pinus, Ulmus at 20% holds", !f[1].holds && f[1].over.size() == 2);
    s.require("limits: families above 30% (Platanaceae, Pinaceae 40% each)", !f[2].holds && f[2].over.size() == 2);
    Limits loose;
    loose.species = 0.5, loose.genus = 0.5, loose.family = 0.5;
    const auto g = findings(comp, loose);
    s.require("limits: looser declared limits hold at every rank", g[0].holds && g[1].holds && g[2].holds);
    const auto sz = size_structure(trees, {0, 15, 30, 45});
    s.require("size classes: 2, 3, 2, 3", sz[0] == 2 && sz[1] == 3 && sz[2] == 2 && sz[3] == 3);
    const auto inc = increments_from_ages(trees, 3, 80, 3);
    // D/age of Platanus: 2, 1.25, 1 → median 1.25; Pinus: 2, 1.2368…, 1.2222…, 2.25 → median of four.
    s.check("increments: the genus's median of D/age", inc.by_genus.at("Platanus") - 1.25, 1e-15);
    s.check("increments: an even count's median", inc.by_genus.at("Pinus") - (47.0 / 38 + 2.0) / 2, 1e-15);
    s.require("increments: a genus with too few aged trees is left to the overall median", inc.by_genus.count("Ulmus") == 0 && std::isfinite(inc.overall));
    const auto st = stock(trees, sch, &inc);
    double carbon = 0;
    for (const auto& t : trees) carbon += estimate(sch, t.taxon, t.dbh).carbon;
    s.check("stock: the sum of the trees' carbon", st.carbon - carbon, 1e-9);
    s.require("stock: the matches counted (species 1, genus 7, family 2)", st.matched[0] == 1 && st.matched[1] == 7 && st.matched[2] == 2);
  }

  // ---- the future ----------------------------------------------------------------------------------------------
  {
    // Stated lives: tree 0 in [2, 6] years, tree 1 exactly 3 years; the rest by a hazard of 5%.
    auto f_trees = trees;
    f_trees[0].life_lower = 2, f_trees[0].life_upper = 6;
    f_trees[1].life_lower = 3, f_trees[1].life_upper = 3;
    const Increments inc = increments_from_ages(trees, 3, 80, 3);
    Removal rm;
    rm.hazard = 0.05;
    Planting none;
    const auto fut = future(f_trees, sch, inc, none, rm, 10);
    // E N(4) = (6 − 4)/4 + 0 + 8 × 0.95⁴.
    s.check("future: expected standing at 4 years in closed form", fut[4].standing - (0.5 + 0 + 8 * std::pow(0.95, 4)), 1e-12);
    s.check("future: the variance Σ p(1 − p)", fut[4].standing_sd * fut[4].standing_sd - (0.25 + 8 * std::pow(0.95, 4) * (1 - std::pow(0.95, 4))), 1e-12);
    s.check("future: now, every tree standing", fut[0].standing - 10, 1e-12);
    Tree open = f_trees[2];
    open.life_lower = 5, open.life_upper = NAN;
    s.require("future: an open class is certain to its bound", standing_probability(open, rm, 4.5) == 1);
    s.check("future: and then follows the hazard", standing_probability(open, rm, 8) - std::pow(0.95, 3), 1e-15);
    double removed = 0;
    for (std::size_t y = 1; y < fut.size(); y++) removed += fut[y].removed;
    s.check("future: removals add up to the loss of standing trees", removed - (fut[0].standing - fut[10].standing), 1e-12);
    // A simulation of the removals: the mean and variance of C(7).
    calx::random::Stream rng(5, 0, 0, 0);
    const int R = 40000;
    double m1 = 0, m2 = 0;
    for (int r = 0; r < R; r++) {
      double c = 0;
      for (const auto& t : f_trees) {
        double life;
        if (std::isfinite(t.life_lower)) life = t.life_lower + (t.life_upper - t.life_lower) * rng.uniform();
        else {
          life = 0;
          while (rng.uniform() >= 0.05) life += 1;  // removed at the end of a year with probability h: standing at t iff ≥ t years survived
          life += 1e-9;
        }
        if (life > 7) c += estimate(sch, t.taxon, t.dbh + inc.of(t.taxon) * 7).carbon;
      }
      m1 += c, m2 += c * c;
    }
    m1 /= R, m2 /= R;
    const double sd = std::sqrt(m2 - m1 * m1);
    s.check("future: E C(7) against 40,000 simulated removals (in standard errors)", (fut[7].carbon - m1) / (sd / std::sqrt(R)), 4);
    s.check("future: sd C(7) against the simulation (relative)", fut[7].carbon_sd / sd - 1, 0.02);
    // Planting: 20 a year, hazard 4%: planted standing at year y = 20 (1 − 0.96^y)/0.04.
    Planting p;
    p.per_year = 20, p.hazard = 0.04, p.dbh = 5;
    p.mix = {{{"Platanus x acerifolia", "Platanus", "Platanaceae"}, 1.0}};
    const auto fp = future(f_trees, sch, inc, p, rm, 30);
    s.check("planting: planted standing 20 (1 − 0.96^y)/0.04 at 30 years", fp[30].planted_standing - 20 * (1 - std::pow(0.96, 30)) / 0.04, 1e-9);
    s.require("planting: the planted plane becomes the largest species", fp[30].largest[0] == "Platanus x acerifolia" && fp[30].largest_share[0] > 0.9);
    // Under removals harsh enough to lose carbon, the planting that holds it restores it exactly; under mild ones it is
    // negative, and the forest gains without planting.
    Removal harsh;
    harsh.hazard = 0.2;
    const double hold = planting_to_hold_carbon(f_trees, sch, inc, p, harsh, 25);
    p.per_year = hold;
    const auto fh = future(f_trees, sch, inc, p, harsh, 25);
    s.require("planting to hold: needed under harsh removals", hold > 0);
    s.check("planting to hold: the expected carbon at 25 years equals today's (relative)", fh[25].carbon / fh[0].carbon - 1, 1e-10);
    const double mild = planting_to_hold_carbon(f_trees, sch, inc, p, rm, 25);
    Planting nothing;
    const auto fm = future(f_trees, sch, inc, nothing, rm, 25);
    s.require("planting to hold: negative exactly when the forest gains carbon unplanted", (mild < 0) == (fm[25].carbon > fm[0].carbon));
    // The stage view with one hazard for all: σ = 1 − h everywhere, and the equilibrium total = planting/h, the future's long run.
    Removal flat;
    flat.hazard = 0.04;
    flat.use_stated_life = false;
    const auto m = stage_model(trees, sch, inc, flat, {0, 15, 30, 45});
    bool same = true;
    for (double x : m.survival) same = same && std::fabs(x - 0.96) < 1e-12;
    s.require("stage view: σ_j = 1 − h for a flat hazard", same);
    // Stated lives: a class of trees with lives [20, 30] has σ = 1 − 1/25.
    auto lived = trees;
    for (auto& tr : lived) tr.life_lower = 20, tr.life_upper = 30;
    const auto ml = stage_model(lived, sch, inc, Removal{}, {0, 15, 30, 45});
    s.check("stage view: σ from the mean remaining life, 1 − 1/25", ml.survival[1] - (1 - 1.0 / 25), 1e-12);
    const auto A = ecology::populations::matrix(m);
    const auto star = ecology::populations::equilibrium(A, {20, 0, 0, 0});
    s.check("stage view: the equilibrium total is planting/h", star[0] + star[1] + star[2] + star[3] - 20 / 0.04, 1e-8);
    const double width = 15;
    s.check("stage view: γ₀ = the mean increment over the class width", m.growth[0] - std::min(1.0, (inc.of(trees[0].taxon) + inc.of(trees[9].taxon)) / 2 / width), 1e-12);
  }

  // ---- crowns ------------------------------------------------------------------------------------------------
  {
    Grid g;
    g.x0 = -20, g.y0 = -20, g.cell = 0.5, g.nx = 80, g.ny = 80;
    const auto one = crown_cover(g, {{0.3, -0.2, 7.0}}, 8);
    double area = 0;
    for (double c : one) area += c * g.cell * g.cell;
    s.check("crowns: one disc's area πr² (relative)", area / (std::numbers::pi * 49) - 1, 0.005);
    // Two discs of radius 5 with centres 6 apart: the union is 2πr² − the lens 2r² acos(d/2r) − (d/2)√(4r² − d²).
    const double r = 5, d = 6;
    const double lens = 2 * r * r * std::acos(d / (2 * r)) - d / 2 * std::sqrt(4 * r * r - d * d);
    const auto two = crown_cover(g, {{-3, 0, r}, {3, 0, r}}, 8);
    double u = 0, most = 0;
    for (double c : two) u += c * g.cell * g.cell, most = std::max(most, c);
    s.check("crowns: two overlapping discs' union, not their sum (relative)", u / (2 * std::numbers::pi * r * r - lens) - 1, 0.005);
    s.require("crowns: no cell above full cover", most <= 1);
    const auto e = canopy_exposure({0.0, 0.1, 0.5, 0.9}, {100, 0, 300, 600}, {0.2, 0.6});
    s.check("exposure: the weighted mean cover", e.mean - (0 * 100 + 0.5 * 300 + 0.9 * 600) / 1000, 1e-15);
    s.check("exposure: the share below 20%", e.share_below[0] - 0.1, 1e-15);
    s.check("exposure: the share below 60%", e.share_below[1] - 0.4, 1e-15);
  }

  return s.done();
}
