// Checks of claims, on the commons preset: an outsider's claim against the commons answered on its route with a complete finding and
// reviewed by another body; a claim against the only steward has no capacity to be answered, the steward recused, and is referred to
// the reviewers until a second steward is appointed by the constitution's own rule; a claim left unanswered past its term is overdue;
// a finding without its standard is incomplete; a claim citing an act the chronicle does not hold lacks evidence.

#include <communities/claims/claims.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::acts::Act;
using thesmos::chronicle::State;
using thesmos::kernel::Kind;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.turn = turn;
  a.fields = std::move(fields);
  return a;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  thesmos::kernel::Parties ps;
  for (const char* p : {"steward", "reviewer-a", "reviewer-b", "resident", "neighbour"}) ps.add({p, Kind::person});
  State st(ps, thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/commons.json")));
  Act found = act("steward", "found", 0);
  found.holders = {{"steward", "steward"}, {"reviewer-a", "reviewer"}, {"reviewer-b", "reviewer"}};
  thesmos::chronicle::step(st, found);
  const std::string founding = st.journal.entries.back().seal;

  // An outsider's claim against the commons, answered and reviewed.
  thesmos::chronicle::step(st, act("resident", "claim", 1, {{"id", "k1"}, {"against", "the commons"}, {"cites", founding}}));
  const auto a = thesmos::chronicle::step(st, act("steward", "answer", 2, {{"id", "k1"}, {"finding", "entry 17 named the street wrongly"}, {"standard", "on the balance of the sources"}, {"burden", "the commons"}, {"remedy", "entry corrected; notice withdrawn"}}));
  thesmos::chronicle::step(st, act("reviewer-a", "assess", 3, {{"object", "k1"}, {"criterion", "review of the finding"}, {"value", "upheld"}}));
  const auto k1 = claims::read(st, "k1", 3);
  s.require("an outsider's claim against the commons has all five complements, is answered in its term with a complete finding, and is reviewed by another body",
            a.admitted && k1.route.stands() && !k1.overdue && k1.finding_lacks.empty() && k1.reviewed && k1.status == "answered");

  // A claim against the only steward.
  thesmos::chronicle::step(st, act("neighbour", "claim", 4, {{"id", "k2"}, {"against", "steward"}}));
  const auto refused = thesmos::chronicle::step(st, act("steward", "answer", 4, {{"id", "k2"}, {"finding", "no fault"}}));
  const auto k2 = claims::read(st, "k2", 4);
  s.require("a claim against the only steward lacks capacity: the steward is recused and may not answer, and the claim is referred to the reviewers",
            !refused.admitted && k2.route.missing() == std::vector<std::string>{"capacity"} && k2.recused == std::vector<std::string>{"steward"} && k2.referral == "reviewers");
  const auto late = claims::read(st, "k2", 6);
  s.require("left unanswered past its term of one round, the claim is overdue", late.overdue && !claims::read(st, "k2", 5).overdue);
  // The reviewers appoint a second steward by the commons' own rule (majority by origin, deliberative), who answers.
  std::string why;
  auto ok = [&](const Act& x) {
    const auto o = thesmos::chronicle::step(st, x);
    if (!o.admitted && why.empty()) why = x.author + " " + x.kind + ": " + o.reason;
    return o.admitted;
  };
  st.parties.add({"second-steward", Kind::person});
  bool all = ok(act("reviewer-a", "propose", 6, {{"id", "appoint"}, {"decision", "appoint steward"}, {"change.admit", "second-steward"}, {"change.position", "steward"}}));
  all = all && ok(act("reviewer-a", "vote", 6, {{"id", "appoint"}, {"value", "yes"}})) && ok(act("reviewer-b", "vote", 6, {{"id", "appoint"}, {"value", "yes"}}));
  all = all && ok(act("steward", "admit", 7, {{"position", "steward"}, {"whom", "second-steward"}, {"proposal", "appoint"}}));
  const auto answered = thesmos::chronicle::step(st, act("second-steward", "answer", 7, {{"id", "k2"}, {"finding", "the refusal gave no reason"}, {"burden", "the steward"}, {"remedy", "a reason given"}}));
  const auto k2b = claims::read(st, "k2", 7);
  s.require("once a second steward is appointed by the commons' own rule, the claim has its route and is answered",
            all && answered.admitted && k2b.route.stands() && k2b.status == "answered", why + answered.reason);
  s.require("the finding that states no standard is found incomplete", k2b.finding_lacks == std::vector<std::string>{"standard"});
  s.require("and it is not reviewed while no member of the review body has assessed it", !k2b.reviewed);

  // Evidence.
  thesmos::chronicle::step(st, act("resident", "claim", 8, {{"id", "k3"}, {"against", "the commons"}, {"cites", founding + "," + std::string(64, 'f')}}));
  const auto k3 = claims::read(st, "k3", 8);
  s.require("a claim citing an act the chronicle does not hold lacks evidence", k3.route.missing() == std::vector<std::string>{"evidence"});

  // ---- Every item's route (thesmos's decision 0011; the design's Phase 8) ----
  why.clear();
  all = ok(act("steward", "question", 9, {{"id", "q1"}, {"about", "norm"}, {"subject", "the shelf"}}));
  const auto q1 = claims::read_item(st, "q1", 9);
  s.require("a steward's question goes to the stewards on the commons' declared route: the other steward may answer it, its asker recused, its answer due one turn later",
            all && q1.type == "question" && q1.body == "stewards" && q1.may_answer == std::vector<std::string>{"second-steward"} &&
                q1.recused == std::vector<std::string>{"steward"} && q1.route.stands() && q1.due == 10,
            why);
  s.require("the question is overdue after its term and not before", claims::read_item(st, "q1", 11).overdue && !claims::read_item(st, "q1", 10).overdue);
  all = ok(act("second-steward", "answer", 11, {{"id", "q1"}, {"outcome", "ruled"}, {"reading", "the shelf is read by any position"}}));
  const auto q1b = claims::read_item(st, "q1", 11);
  s.require("answered by the other steward with a ruling, the item closes ruled, and no one is left to answer it", all && q1b.status == "ruled" && q1b.closed_by == "second-steward" && q1b.may_answer.empty(),
            why);
  all = ok(act("resident", "petition", 12, {{"id", "p1"}, {"kind", "series"}})) && ok(act("neighbour", "apply", 12, {{"id", "ap1"}, {"position", "contributor"}}));
  const auto p1 = claims::read_item(st, "p1", 12), ap1 = claims::read_item(st, "ap1", 12);
  s.require("a petition for a series goes to the custodian of the shelf, an office both stewards hold; an application to the open position of contributor is decided "
            "in its own act, admitted",
            all && p1.office == "steward" && p1.may_answer.size() == 2 && p1.route.stands() && ap1.status == "admitted" && ap1.route.capacity && st.holds("neighbour", "contributor"),
            why);
  {
    thesmos::kernel::Parties hp;
    hp.add({"pat", Kind::person});
    State home(hp, thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/household.json")));
    Act f = act("pat", "found", 0);
    f.holders = {{"pat", "principal"}};
    thesmos::chronicle::step(home, f);
    const auto asked = thesmos::chronicle::step(home, act("pat", "question", 1, {{"id", "hq"}, {"about", "rule"}, {"subject", "mandates"}}));
    const auto none = claims::unanswerable(home, 1);
    s.require("in a household the person's own question can be answered by no one but the person, who is recused: it lacks capacity and is referred beyond the household",
              asked.admitted && none.size() == 1 && none[0].item == "hq" && none[0].route.missing() == std::vector<std::string>{"capacity"} &&
                  none[0].referral == "beyond the community (a league's panel or an outside forum)" && claims::unanswerable(st, 12).empty(),
              asked.reason);
  }
  return s.done();
}
