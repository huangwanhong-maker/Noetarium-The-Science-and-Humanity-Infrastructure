// Checks of mandates and their lifecycles (mandates.hpp).
//
// - A registry's faults, each named, a circle of renewals among them.
// - A lineage of three mandates (given, renewed, superseded) and two beside it (one open-ended, one that terminates it): each one's
//   state at dates before, within and after its period, and who renewed, superseded or terminated it.
// - The lineage of the latest: what it continues, its root, and the themes and addressees it added.
// - Review: due by a period from the last review or from the beginning, or by a date; overdue, and not yet.
// - Overlap of mandates in force on one addressee and theme; the load of each addressee against what is resourced.
// - The signs at a date, and, read against an invented undertaking, a mandate given without authority recorded and one addressed to
//   someone not a participant.
//
// Every body, mandate and review here is invented.

#include <diplomacy/mandates/mandates.hpp>

#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace diplomacy::mandates;
namespace dr = diplomacy::relations;

namespace {

Registry registry() {
  Registry r;
  Mandate m1{"m1", "a programme on water", "resolution 1", "para. 4", "assembly", "2020-01-01", {"secretariat"}, {"water"}, "2020-01-01", "2022-01-01", 365, "", true, "", {}};
  Mandate m2{"m2", "the programme renewed", "resolution 7", "para. 2", "assembly", "2021-12-01", {"secretariat"}, {"water", "sanitation"}, "2022-01-01", "2024-01-01", 0, "2023-01-01", true, "", {{Link::renews, "m1"}}};
  Mandate m3{"m3", "the programme widened", "resolution 12", "para. 1", "assembly", "2023-05-01", {"secretariat", "agency"}, {"water", "sanitation", "climate"}, "2023-06-01", "", 0, "", false,
             "within existing resources", {{Link::supersedes, "m2"}}};
  Mandate m4{"m4", "a register of wells", "decision 3", "para. 1", "committee", "2020-12-01", {"secretariat"}, {"water"}, "2021-01-01", "", 730, "", true, "", {}};
  Mandate m5{"m5", "a data service", "resolution 20", "para. 3", "assembly", "2024-11-01", {"agency"}, {"data"}, "2025-01-01", "2027-01-01", 365, "", true, "", {{Link::terminates, "m4"}}};
  r.mandates = {m1, m2, m3, m4, m5};
  r.reviews = {{"m1", "2020-12-15", "assembly", "continued"}, {"m4", "2022-12-01", "assembly", "continued"}};
  return r;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Faults ----
  {
    s.require("the registry here is declared without fault", problems(registry()).empty());
    Registry r = registry();
    r.mandates.push_back({"a", "", "", "", "", "", {"x"}, {}, "2025-01-01", "", 0, "", false, "", {{Link::renews, "b"}}});
    r.mandates.push_back({"b", "", "", "", "", "", {}, {}, "2025-01-01", "2024-01-01", 0, "", false, "", {{Link::renews, "a"}, {Link::amends, "zz"}}});
    r.reviews.push_back({"nothing", "2025-01-01", "", ""});
    const auto ps = problems(r);
    auto has = [&](const std::string& w) {
      for (const auto& p : ps)
        if (p.what.find(w) != std::string::npos) return true;
      return false;
    };
    s.require("a registry's faults are named: a circle of renewals, a predecessor not recorded, an end before the beginning, a mandate to no one, a review of nothing",
              has("run in a circle") && has("zz, which is not recorded") && has("ends before it begins") && has("addressed to no one") && has("no such mandate"), std::to_string(ps.size()) + " faults");
  }

  // ---- Lifecycle ----
  {
    const Registry r = registry();
    s.require("the first mandate is in force within its period", status(r, "m1", "2021-06-01").state == State::in_force);
    const Status renewed = status(r, "m1", "2022-06-01");
    s.require("after its end it is renewed by the second", renewed.state == State::renewed && renewed.by == "m2", renewed.why);
    const Status superseded = status(r, "m2", "2023-07-01");
    s.require("the second is superseded by the third once the third is in force", superseded.state == State::superseded && superseded.by == "m3" && status(r, "m2", "2023-05-01").state == State::in_force,
              superseded.why);
    s.require("the third is not yet in force before it begins", status(r, "m3", "2023-01-01").state == State::pending);
    s.require("the register of wells is terminated by the data service", status(r, "m4", "2025-06-01").state == State::terminated && status(r, "m4", "2025-06-01").by == "m5");
    s.require("the data service expires at its end, renewed by nothing", status(r, "m5", "2027-06-01").state == State::expired);
    s.require("in force in June 2024: the widened programme and the register", in_force(r, "2024-06-01") == std::vector<std::string>{"m3", "m4"});
  }

  // ---- Lineage ----
  {
    const Lineage l = lineage(registry(), "m3");
    s.require("the widened programme continues the renewed one and the first, its root", l.before == std::vector<std::string>{"m2", "m1"} && l.roots == std::vector<std::string>{"m1"});
    s.require("and has added sanitation and climate, and the agency", l.themes_added == std::vector<std::string>{"sanitation", "climate"} && l.addressees_added == std::vector<std::string>{"agency"});
    s.require("the first is continued by the renewed and the widened programmes", lineage(registry(), "m1").after == std::vector<std::string>{"m2", "m3"});
  }

  // ---- Review ----
  {
    const Registry r = registry();
    const Due a = due(r, "m4", "2024-06-01");
    s.require("a review every two years from the last falls due two years after it, not yet overdue", a.last == "2022-12-01" && a.due == "2024-11-30" && !a.overdue, a.due);
    s.require("and is overdue after that", due(r, "m4", "2024-12-15").overdue);
    const Due b = due(r, "m2", "2023-02-01");
    s.require("a review by a date, never made, is overdue after the date", b.overdue && b.never && b.due == "2023-01-01");
    s.require("a review a year from the last is not yet due within the year", !due(r, "m1", "2021-06-01").overdue && due(r, "m1", "2021-06-01").due == "2021-12-15");
  }

  // ---- Overlap and loads ----
  {
    const Registry r = registry();
    const auto o = overlaps(r, "2024-06-01");
    s.require("the widened programme and the register overlap on the secretariat and water", o.size() == 1 && o[0].a == "m3" && o[0].b == "m4" && o[0].themes == std::vector<std::string>{"water"});
    const auto l = loads(r, "2024-06-01");
    s.require("the secretariat carries two mandates, one resourced; the agency one, unresourced",
              l.size() == 2 && l[0].addressee == "agency" && l[0].mandates == 1 && l[0].resourced == 0 && l[1].mandates == 2 && l[1].resourced == 1);
  }

  // ---- Signs ----
  {
    const Registry r = registry();
    const auto sg = signs(r, "2024-06-01");
    auto has = [&](const std::vector<Sign>& v, const std::string& kind, const std::string& where) {
      for (const auto& x : v)
        if (x.kind == kind && x.where == where) return true;
      return false;
    };
    s.require("in June 2024 the widened programme is never reviewed, open-ended, unresourced and has crept, and the two overlap",
              has(sg, "never reviewed", "mandate m3") && has(sg, "open-ended", "mandate m3") && has(sg, "unresourced", "mandate m3") && has(sg, "creep", "mandate m3") &&
                  has(sg, "overlap", "mandates m3 and m4") && sg.size() == 5,
              std::to_string(sg.size()) + " signs");
    dr::Undertaking u;
    u.types = {{"authorises", "", true, false, false}};
    u.participants = {{"assembly", "the assembly", "organisation", true}, {"secretariat", "the secretariat", "organisation", false}, {"committee", "a committee", "organisation", false}};
    u.ties = {{"authorises", "assembly", "secretariat", "2019-01-01", "", false, ""}};
    const auto with = signs(r, "2024-06-01", &u);
    s.require("against the undertaking, the register was given by a committee holding no authority, and the agency is no participant",
              has(with, "authority", "mandate m4") && has(with, "addressee", "mandate m3") && with.size() == 7, std::to_string(with.size()) + " signs");
  }
  return s.done();
}
