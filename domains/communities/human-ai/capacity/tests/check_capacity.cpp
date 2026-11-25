// Checks of capacity: a floor in usable units asks four times the tokens of a participant whose conversion factor is four; a pool too
// small for every shortfall raises the least served first and is spent exactly, and one large enough fills every shortfall and no
// more; three assessments by agents of three controllers running one provider's model are three origins and one provider.

#include <communities/capacity/capacity.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <numeric>

using namespace communities::capacity;
using thesmos::kernel::Kind;

int main() {
  calx::verification::Suite s;
  const std::vector<Participant> ps = {{"strong", 1, 0}, {"weak", 4, 0}, {"served", 1, 5}};
  const auto need = shortfalls(ps, 3);
  s.require("a floor of 3 usable units asks 3 tokens of a participant converting at 1 and 12 of one converting at 4, nothing of one already above",
            need.at("strong") == 3 && need.at("weak") == 12 && need.at("served") == 0);
  const auto full = pool(ps, 3, 100);
  s.require("a pool larger than the shortfalls fills each and no more", std::abs(full.at("strong") - 3) < 1e-9 && std::abs(full.at("weak") - 12) < 1e-9 && full.at("served") == 0);
  const auto part = pool(ps, 3, 5);
  const double spent = part.at("strong") + part.at("weak") + part.at("served");
  const double u_strong = part.at("strong") / 1, u_weak = part.at("weak") / 4;
  s.require("a pool of 5 is spent exactly and raises both to the same usable level, 1 each (1 token and 4)",
            std::abs(spent - 5) < 1e-6 && std::abs(u_strong - u_weak) < 1e-6 && std::abs(u_strong - 1) < 1e-6);

  thesmos::kernel::Parties parties;
  for (const char* c : {"lab-a", "lab-b", "person-c"}) parties.add({c, Kind::organisation});
  parties.add({"lab-a/agent", Kind::operative, "lab-a", "one provider's model"});
  parties.add({"lab-b/agent", Kind::operative, "lab-b", "one provider's model"});
  parties.add({"person-c/agent", Kind::operative, "person-c", "one provider's model"});
  parties.add({"reviewer", Kind::person});
  const auto i = independence(parties, {"lab-a/agent", "lab-b/agent", "person-c/agent"});
  s.require("three agents of three controllers running one provider's model are three origins and one provider: for agreement they count once",
            i.assessors == 3 && i.origins == 3 && i.providers == 1);
  const auto j = independence(parties, {"lab-a/agent", "reviewer"});
  s.require("an agent and a person are two providers of judgement", j.providers == 2);
  return s.done();
}
