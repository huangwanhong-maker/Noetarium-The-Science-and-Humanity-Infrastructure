// Checks of a brief against a drawn building (programme.hpp), on the house of the plan module's checks: a living room and a bedroom,
// the bedroom reached through the living room.
//
// - Needs: one living room of 20 m² is there; two bedrooms of 9 m² are not, and the statement says how many were found; a bedroom
//   asked larger than the one drawn is named with its area.
// - Relations, read over every room of the first use: the bedroom opens to a living room and adjoins it; it is therefore not apart
//   from it, and is reached only through it; the living room is reached through no bedroom.
// - A relation of a use that is not drawn states nothing.
// - Without its entrance the house fails the statement asked of every building: each room is reached.

#include "../../plan/tests/house.hpp"

#include <architecture/programme/programme.hpp>

#include <calx/verification/check.hpp>

#include <string>

namespace ap = architecture::plan;
namespace pg = architecture::programme;

namespace {
const pg::Statement* find(const std::vector<pg::Statement>& ss, const std::string& part) {
  for (const auto& s : ss)
    if (s.text.find(part) != std::string::npos) return &s;
  return nullptr;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto b = house::building();
  const auto R = ap::read(b);

  pg::Brief brief;
  brief.needs = {{"living", 1, 20}, {"bedroom", 2, 9}, {"bathroom", 1, 0}};
  brief.relations = {{"bedroom", "living", pg::How::opens_to}, {"bedroom", "living", pg::How::adjoins}, {"bedroom", "living", pg::How::apart},
                     {"bedroom", "living", pg::How::not_through}, {"living", "bedroom", pg::How::not_through}, {"bathroom", "kitchen", pg::How::apart}};
  const auto ss = pg::check(brief, b, R);

  s.require("every room is reached", find(ss, "Every room is reached")->ok);
  s.require("one living room of at least 20 m² is there", find(ss, "1 living room of at least 20.0 m²")->ok);
  const auto* beds = find(ss, "2 bedrooms");
  s.require("two bedrooms are not, and one is said to be found", beds && !beds->ok && beds->note == "1 found");
  s.require("a bathroom is asked and not drawn", !find(ss, "1 bathroom")->ok);
  s.require("the bedroom opens to a living room and adjoins one", find(ss, "Each bedroom opens to a living room")->ok && find(ss, "Each bedroom adjoins a living room")->ok);
  const auto* apart = find(ss, "No bedroom opens to a living room");
  s.require("... so it is not apart from it, and is named", apart && !apart->ok && apart->note.find("Bedroom") != std::string::npos);
  s.require("the bedroom is reached only through the living room", !find(ss, "No bedroom is reached only through a living room")->ok);
  s.require("the living room is reached through no bedroom", find(ss, "No living room is reached only through a bedroom")->ok);
  s.require("a relation of a use that is not drawn states nothing: 3 needs, 5 relations and the reach make 9 statements", ss.size() == 9);

  {
    pg::Brief large;
    large.needs = {{"bedroom", 1, 31}};
    const auto st = pg::check(large, b, R);
    const auto* one = find(st, "1 bedroom of at least 31.0 m²");
    s.require("a bedroom asked at 31 m² is not met by one of 30.0 m², which is named with its area", one && !one->ok && one->note.find("Bedroom is 30.0 m²") != std::string::npos);
  }
  {
    ap::Building shut = b;
    shut.storeys[0].openings.erase(shut.storeys[0].openings.begin());
    const auto st = pg::check(pg::Brief{}, shut, ap::read(shut));
    s.require("without its entrance the house fails the one statement asked of every building, naming both rooms",
              st.size() == 1 && !st[0].ok && st[0].note.find("Living") != std::string::npos && st[0].note.find("Bedroom") != std::string::npos);
  }
  return s.done();
}
