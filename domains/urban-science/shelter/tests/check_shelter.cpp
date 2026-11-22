// Checks of urban::shelter, each worked by hand: a shared toilet read both ways; three people to a room not crowded, three and a half
// crowded; the parts that must be durable, and a hazardous site; tenure's three readings; the union rule with an unknown answer, its
// bounds and the three treatments; inadequate housing at 30% of income; deprivations counted and the one alone; shares by group; a
// wider reading never lowering either bound on 300 random households; refusals.

#include <urban/shelter/shelter.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <vector>

using namespace urban::shelter;

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
Reading base() {
  Reading r;
  r.name = "base";
  r.water_improved = {"piped", "protected well"};
  r.sanitation_improved = {"sewer", "septic"};
  r.durable = {"brick", "concrete", "metal sheets", "tiles"};
  return r;
}
Household good(double persons, const std::string& group = "") {
  Household h;
  h.persons = persons, h.rooms = persons, h.group = group;
  h.water = "piped", h.sanitation = "sewer", h.shared_toilet = 0;
  h.walls = "brick", h.roof = "metal sheets", h.floor = "concrete";
  h.hazardous_site = 0, h.documented = 1, h.fears_eviction = 0;
  h.housing_cost = 100, h.income = 1000;
  return h;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  Reading r = base();
  {
    Household h = good(2);
    h.shared_toilet = 1;
    Reading shared = r;
    shared.shared_deprives = true;
    s.require("a shared toilet on a sewer: improved when sharing is not counted, not improved when it is", statuses(h, r)[1] == no && statuses(h, shared)[1] == yes);
    h.shared_toilet = -1;
    s.require("... and unknown when sharing counts and the answer is missing", statuses(h, shared)[1] == missing && statuses(h, r)[1] == no);
  }
  {
    Household h = good(6);
    h.rooms = 2;
    s.require("six people in two rooms: three to a room, not more than three, not crowded", statuses(h, r)[2] == no);
    h.persons = 7;
    Reading two = r;
    two.most_per_room = 2;
    s.require("... seven: three and a half to a room, crowded; and six crowded when two to a room is the most", statuses(h, r)[2] == yes && statuses(good(6), two)[2] == no);
    h.persons = 6;
    s.require("... six in two rooms crowded when the most is two", statuses(h, two)[2] == yes);
  }
  {
    Household h = good(3);
    h.floor = "earth";
    Reading walls_roof = r;
    walls_roof.parts = {"walls", "roof"};
    s.require("an earth floor: not durable when the floor counts, durable when only walls and roof do", statuses(h, r)[3] == yes && statuses(h, walls_roof)[3] == no);
    h.floor = "concrete", h.hazardous_site = 1;
    Reading hazard = r;
    hazard.hazard_deprives = true;
    s.require("... a hazardous site: durable unless the site counts", statuses(h, r)[3] == no && statuses(h, hazard)[3] == yes);
  }
  {
    Household h = good(3);
    h.documented = 1, h.fears_eviction = 1;
    Reading doc = r, both = r, either = r;
    doc.tenure = Tenure::documented, both.tenure = Tenure::documented_and_unthreatened, either.tenure = Tenure::documented_or_unthreatened;
    s.require("a document and a fear of eviction: secure by documentation, insecure when both are asked, secure when either suffices; left out, uncounted",
              statuses(h, doc)[4] == no && statuses(h, both)[4] == yes && statuses(h, either)[4] == no && statuses(h, r)[4] == uncounted);
  }
  {
    // Four households of weight 1: 2 people without improved water, 3 well housed, 5 whose water is not known, 10 well housed.
    std::vector<Household> hs = {good(2, "A"), good(3, "A"), good(5, "B"), good(10, "B")};
    hs[0].water = "surface";
    hs[2].water = "";
    hs[0].housing_cost = 400, hs[3].housing_cost = 300, hs[2].income = unknown;
    for (std::size_t i = 0; i < hs.size(); i++) hs[i].cluster = static_cast<int>(i);
    const auto R = assess(hs, r);
    s.check("the union with an unknown answer: between 2/20 and 7/20 of people, 5/20 unknown",
            std::fabs(R.slum.lower - 0.1) + std::fabs(R.slum.upper - 0.35) + std::fabs(R.slum.unknown_people - 0.25), 1e-12);
    s.check("... the unknown left out: 2/15", std::fabs(R.slum.estimate.value - 2.0 / 15), 1e-12);
    Reading as_deprived = r, as_not = r;
    as_deprived.unknowns = Unknown::deprived, as_not.unknowns = Unknown::not_deprived;
    s.check("... counted as deprived 7/20, as not deprived 2/20", std::fabs(assess(hs, as_deprived).slum.estimate.value - 0.35) + std::fabs(assess(hs, as_not).slum.estimate.value - 0.1), 1e-12);
    s.check("inadequate housing: 400 of 1,000 exceeds 30%, 300 does not; the income not known: 2/15 with it left out, bounds 2/20 and 7/20",
            std::fabs(R.inadequate.estimate.value - 2.0 / 15) + std::fabs(R.inadequate.lower - 0.1) + std::fabs(R.inadequate.upper - 0.35), 1e-12);
    s.check("deprivations among the 15 people whose every status is known: 13/15 with none, 2/15 with one, water alone",
            std::fabs(R.deprivations[0] - 13.0 / 15) + std::fabs(R.deprivations[1] - 2.0 / 15) + std::fabs(R.only[0] - 2.0 / 15), 1e-12);
    s.check("by group: A 2/5; B between 0 and 5/15", std::fabs(R.slum_by_group.at("A").lower - 0.4) + R.slum_by_group.at("B").lower + std::fabs(R.slum_by_group.at("B").upper - 1.0 / 3), 1e-12);
    s.require("... the interval holds the estimate", R.slum.estimate.ci_lower <= R.slum.estimate.value && R.slum.estimate.value <= R.slum.estimate.ci_upper && R.slum.estimate.se > 0);
  }
  {
    // A wider reading never lowers either bound: 300 households with answers drawn from a small generator, some missing.
    unsigned long long seed = 1122;
    const auto draw = [&](int n) { seed = seed * 6364136223846793005ULL + 1442695040888963407ULL; return static_cast<int>((seed >> 33) % static_cast<unsigned long long>(n)); };
    const std::vector<std::string> water = {"piped", "protected well", "surface", ""}, san = {"sewer", "septic", "pit", ""}, mat = {"brick", "metal sheets", "earth", "thatch", ""};
    std::vector<Household> hs;
    for (int i = 0; i < 300; i++) {
      Household h;
      h.weight = 1 + draw(5), h.stratum = draw(3), h.cluster = draw(30), h.persons = 1 + draw(8), h.rooms = draw(5) == 0 ? unknown : 1 + draw(4);
      h.water = water[draw(4)], h.sanitation = san[draw(4)], h.shared_toilet = draw(3) - 1;
      h.walls = mat[draw(5)], h.roof = mat[draw(5)], h.floor = mat[draw(5)];
      h.hazardous_site = draw(3) - 1, h.documented = draw(3) - 1, h.fears_eviction = draw(3) - 1;
      hs.push_back(h);
    }
    Reading wide = r;
    wide.shared_deprives = true, wide.most_per_room = 2, wide.hazard_deprives = true, wide.tenure = Tenure::documented_and_unthreatened;
    wide.water_improved = {"piped"};
    const auto A = assess(hs, r), B = assess(hs, wide);
    s.require("a wider reading (sharing, two to a room, the site, tenure, fewer improved sources) never lowers either bound",
              B.slum.lower >= A.slum.lower && B.slum.upper >= A.slum.upper && B.slum.lower > A.slum.lower);
    bool union_bound = true;
    for (std::size_t c = 0; c < 5; c++) union_bound = union_bound && B.slum.lower >= B.component[c].lower - 1e-15;
    double sum = 0;
    for (std::size_t c = 0; c < 5; c++) sum += B.component[c].lower;
    s.require("... and the union's lower bound lies between the largest component's and their sum", union_bound && B.slum.lower <= sum + 1e-12);
  }
  s.require("a household of no one is refused", refused([] { assess({Household{1, 0, 0, "", 0}}, base()); }));
  s.require("a part other than walls, roof and floor is refused", refused([] { Reading x = base(); x.parts = {"door"}; assess({good(1)}, x); }));
  return s.done();
}
