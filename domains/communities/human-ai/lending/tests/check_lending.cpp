// Checks of lending: the first run of a lent agent, step by step, on the commons preset itself (the implementation's Section 5.1);
// and receiving capacity as a guard.

#include <communities/lending/lending.hpp>

#include <thesmos/chronicle/chronicle.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>

using namespace communities;
using thesmos::acts::Act;
using thesmos::kernel::Kind;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}, std::string body = {}) {
  Act a;
  a.author = std::move(author);
  a.kind = std::move(kind);
  a.turn = turn;
  a.fields = std::move(fields);
  a.mandate = std::move(mandate);
  a.body = std::move(body);
  return a;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const auto commons = thesmos::constitutions::read(slurp(std::string(COMMUNITIES_DATA) + "/presets/data/commons.json"));
  thesmos::kernel::Parties ps;
  for (const char* p : {"steward", "reviewer", "sponsor"}) ps.add({p, Kind::person});
  ps.add({"sponsor/agent", Kind::operative, "sponsor", "a provider's model"});
  thesmos::chronicle::State st(ps, commons);
  Act found = act("steward", "found", 0);
  found.holders = {{"steward", "steward"}, {"reviewer", "reviewer"}};
  std::vector<thesmos::chronicle::Outcome> out;
  out.push_back(thesmos::chronicle::step(st, found));

  // 2. The sponsor writes the loan: six hours, two million tokens of a named model, a slice of 200,000; and starts the agent.
  lending::Loan loan{"loan-1", "sponsor", "sponsor/agent", {"catalogue"}, {{"tokens", 2000000}, {"hours", 6}}, {{"tokens", 200000}}, 1, 20};
  Act issue = act("sponsor", "mandate", 1);
  issue.grant = lending::mandate_of(loan);
  out.push_back(thesmos::chronicle::step(st, issue));
  // 3. The agent joins the commons as a contributor; its chain is checked and its root is a person.
  out.push_back(thesmos::chronicle::step(st, act("sponsor/agent", "join", 1, {{"position", "contributor"}}, "loan-1")));
  // 4. It offers to check forty entries, and the steward takes the offer.
  Act offer = act("sponsor/agent", "offer", 2, {{"id", "t1"}, {"to", "steward"}, {"matter", "catalogue"}, {"amount", "40"}, {"unit", "entries"}}, "loan-1");
  offer.charges = {{"loan-1", "tokens", 150000}};
  out.push_back(thesmos::chronicle::step(st, offer));
  out.push_back(thesmos::chronicle::step(st, act("steward", "take", 2, {{"id", "t1"}})));
  // 5. It works on the sponsor's account and delivers; a reviewer assesses; the steward accepts.
  bool all = true;
  for (int k = 0; k < 9; k++) {  // the work charged in slices
    Act step = act("sponsor/agent", "report", 3, {{"matter", "catalogue"}}, "loan-1");
    step.charges = {{"loan-1", "tokens", 190000}};
    all = all && thesmos::chronicle::step(st, step).admitted;
  }
  Act deliver = act("sponsor/agent", "deliver", 4, {{"id", "t1"}, {"matter", "catalogue"}, {"artefact", "sha256:..."}}, "loan-1");
  deliver.charges = {{"loan-1", "tokens", 100000}};
  out.push_back(thesmos::chronicle::step(st, deliver));
  out.push_back(thesmos::chronicle::step(st, act("reviewer", "assess", 5, {{"object", "t1"}, {"criterion", "sources checked"}, {"value", "39 of 40"}})));
  out.push_back(thesmos::chronicle::step(st, act("steward", "accept", 5, {{"id", "t1"}})));
  // 6. It offers forty more; its next delivery would pass the ceiling and is refused; it hands over what it did, and the steward releases the rest.
  out.push_back(thesmos::chronicle::step(st, act("sponsor/agent", "offer", 6, {{"id", "t2"}, {"to", "steward"}, {"matter", "catalogue"}, {"amount", "40"}}, "loan-1")));
  out.push_back(thesmos::chronicle::step(st, act("steward", "take", 6, {{"id", "t2"}})));
  Act over = act("sponsor/agent", "deliver", 7, {{"id", "t2"}, {"matter", "catalogue"}}, "loan-1");
  over.charges = {{"loan-1", "tokens", 200000}};
  const auto refused = thesmos::chronicle::step(st, over);
  Act hand = act("sponsor/agent", "hand over", 7, {{"id", "t2"}, {"matter", "catalogue"}, {"artefact", "sha256:..."}}, "loan-1", "Entries 41 to 52 checked; 53 to 80 open; two sources unreachable.");
  hand.charges = {{"loan-1", "tokens", 40000}};
  out.push_back(thesmos::chronicle::step(st, hand));
  out.push_back(thesmos::chronicle::step(st, act("steward", "release", 8, {{"id", "t2"}})));
  // 7. The sponsor revokes the mandate; a later act by the agent is refused.
  out.push_back(thesmos::chronicle::step(st, act("sponsor", "revoke", 8, {{"which", "loan-1"}})));
  const auto late = thesmos::chronicle::step(st, act("sponsor/agent", "say", 9, {}, "loan-1", "One more thing."));

  bool admitted = all;
  std::string why;
  for (const auto& o : out)
    if (!o.admitted) admitted = false, why = why.empty() ? o.reason : why;
  s.require("the first run of a lent agent is admitted act by act on the commons preset itself", admitted, why);
  s.require("the delivery that would pass the ceiling is refused, the handover within it admitted", !refused.admitted && refused.reason.find("exhausted") != std::string::npos, refused.reason);
  s.require("after revocation the agent's act is refused", !late.admitted && late.reason.find("revoked") != std::string::npos, late.reason);
  s.require("the sponsor answers: the debtor of both tasks is the sponsor, the first discharged and the second released",
            st.commitments.at("t1").debtor == "sponsor" && st.commitments.at("t1").status == thesmos::commitments::Status::discharged &&
                st.commitments.at("t2").status == thesmos::commitments::Status::released);
  s.require("tokens charged reach the ceiling and stay within it: 150,000 + 9 × 190,000 + 100,000 + 40,000 = 2,000,000", st.used.at("loan-1").at("tokens") == 2000000);

  // Receiving capacity: six hours of review a round.
  lending::Receiving host(6, 2);
  const std::string a = host.offer("a", 4), b = host.offer("b", 3), c = host.offer("c", 2), d = host.offer("d", 1), e = host.offer("e", 7);
  s.require("contributions are admitted while capacity lasts, then queued, then declined with a reason",
            a == "admitted" && b == "queued" && c == "queued" && d.rfind("declined", 0) == 0 && e.rfind("declined", 0) == 0);
  const auto next = host.next_round();
  s.require("the next round serves the queue in order", next.size() == 2 && next[0] == "b" && next[1] == "c" && host.used() == 5);
  return s.done();
}
