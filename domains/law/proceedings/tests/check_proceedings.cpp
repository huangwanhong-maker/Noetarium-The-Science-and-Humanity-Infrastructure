// Checks of proceedings (proceedings.hpp) on an example scheme of civil procedure, declared here and no jurisdiction's: a docket
// taken in order and on time, kept; what may be taken next and what is due; a step out of order, one late, one by a party not
// allowed, one taken twice, a change of standing the facts do not allow, each found at its place; the docket's seal broken; the
// record after the steps, a finding standing as a relational fact.

#include <law/proceedings/proceedings.hpp>

#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace law::proceedings;
using law::facts::Standing;

namespace {

Rules civil() {
  // An example scheme; its periods and roles illustrate, and are not any jurisdiction's.
  return {"an example civil procedure",
          {{"claim", {}, {"claimant"}, true, "", -1, std::nullopt},
           {"service", {"claim"}, {"claimant"}, false, "claim", 14, std::nullopt},
           {"defence", {"service"}, {"defendant"}, true, "service", 28, std::nullopt},
           {"admission", {"service"}, {"defendant"}, false, "", -1, Standing::admitted},
           {"hearing", {"defence"}, {"court"}, false, "", -1, std::nullopt},
           {"proof", {"hearing"}, {"court"}, false, "", -1, Standing::proved},
           {"finding", {"hearing"}, {"court"}, false, "", -1, Standing::found},
           {"judgment", {"finding"}, {"court"}, true, "", -1, std::nullopt},
           {"appeal", {"judgment"}, {"claimant", "defendant"}, true, "judgment", 21, std::nullopt}}};
}

law::facts::Record record() {
  law::facts::Record r;
  r.proceeding = "Ames v Burr";
  r.facts = {law::facts::fact("signed", "Burr signed the deed of sale"), law::facts::fact("paid", "Ames paid the price")};
  return r;
}

Docket opened() {
  return {"Ames v Burr", {{"Ames", "claimant"}, {"Burr", "defendant"}, {"the court", "court"}}, {}};
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto rules = civil();
  Docket d = opened();
  take(d, {"2026-01-05", "Ames", "claim", {}, "claim filed"});
  take(d, {"2026-01-12", "Ames", "service", {}, "served on Burr"});
  {
    const auto st = state(d, rules, "2026-01-20");
    s.require("after service, the defence and an admission may be taken, and the defence is due by 9 February, 28 days on",
              st.open == std::vector<std::string>{"service", "defence", "admission"} && st.due.size() == 1 && st.due[0].kind == "defence" && st.due[0].by == "2026-02-09" && !st.due[0].overdue,
              st.verdict.reason);
    s.require("on 10 February, with no defence, it is overdue, and said", state(d, rules, "2026-02-10").due[0].overdue && state(d, rules, "2026-02-10").verdict.piece("overdue") == "defence (by 2026-02-09)");
  }
  take(d, {"2026-02-02", "Burr", "defence", {}, "denies paying"});
  take(d, {"2026-02-02", "Burr", "admission", {"signed"}, "admits signing"});
  take(d, {"2026-03-10", "the court", "hearing", {}, ""});
  take(d, {"2026-03-17", "the court", "proof", {"paid"}, "the receipt is authentic and shows payment"});
  take(d, {"2026-03-24", "the court", "finding", {"signed", "paid"}, "the price was paid"});
  const std::string kept = d.journal.head();
  take(d, {"2026-04-01", "the court", "judgment", {}, "for Ames"});
  const auto r = review(d, rules, record());
  s.require("a docket of eight steps taken in order, on time, by parties allowed: it keeps the rules", r.verdict.holds(), r.verdict.reason);
  {
    const auto st = state(d, rules, "2026-04-10");
    s.require("after judgment an appeal is due by 22 April, 21 days on", st.due.size() == 1 && st.due[0].kind == "appeal" && st.due[0].by == "2026-04-22");
  }
  const auto after = apply(d, rules, record());
  const auto* signed_ = after.fact("signed");
  const auto* paid = after.fact("paid");
  s.require("in the record after the steps, the signing was admitted and then found, the payment proved and then found",
            signed_->standing == Standing::found && paid->standing == Standing::found, "");
  s.require("the finding stands as a relational fact: it emerged from the proceeding, persists from its date", signed_->emerged_from == "Ames v Burr, finding of 2026-03-24" && signed_->since == "2026-03-24");

  // ---- Faults, each at its place ----------------------------------------------------------------------------------------------------
  {
    Docket e = opened();
    take(e, {"2026-01-05", "Ames", "claim", {}, ""});
    take(e, {"2026-01-30", "Ames", "service", {}, "served late"});
    take(e, {"2026-02-02", "Ames", "hearing", {}, ""});
    take(e, {"2026-02-03", "Burr", "judgment", {}, ""});
    take(e, {"2026-02-04", "Ames", "claim", {}, "again"});
    take(e, {"2026-02-05", "the court", "finding", {"paid"}, ""});
    take(e, {"2026-02-06", "the court", "verdict", {}, ""});
    const auto v = review(e, rules, record());
    std::string all;
    for (const auto& f : v.faults) all += f + " | ";
    const auto has = [&](const std::string& w) { return all.find(w) != std::string::npos; };
    s.require("service 25 days after the claim, where 14 are allowed, is found late at step 2", has("step 2 (service, 2026-01-30): taken 25 days after the claim of 2026-01-05, where the rules allow 14"), all);
    s.require("a hearing taken by the claimant, before any defence, is found at step 3 twice over", has("step 3 (hearing, 2026-02-02): taken by Ames, the claimant, where the rules allow only court") && has("step 3 (hearing, 2026-02-02): taken before any defence"));
    s.require("judgment by the defendant before any finding, a second claim, and a kind no rule declares are found", has("step 4 (judgment") && has("step 5 (claim, 2026-02-04): a claim may be taken once") && has("step 7 (verdict, 2026-02-06): no kind of step verdict"));
    s.require("a finding of a payment merely alleged is refused at step 6: an allegation is proved or admitted before it is found", has("step 6 (finding, 2026-02-05): paid is alleged, and does not become found"));
    bool refused = false;
    try { (void)apply(e, rules, record()); } catch (const std::invalid_argument&) { refused = true; }
    s.require("and applying that docket to the record is refused", refused);
  }
  {
    Docket tampered = d;
    tampered.journal.entries[2].note = "admits paying";
    const auto v = review(tampered, rules, record());
    s.require("a step's words changed after it was sealed are found, at its place", v.verdict.fails() && v.faults[0].find("entry 3 does not seal") != std::string::npos, v.faults.empty() ? "" : v.faults[0]);
    Docket cut = d;
    cut.journal.entries.pop_back();
    s.require("the judgment removed from the end is found only against the seal kept", review(cut, rules, record()).verdict.holds() && review(cut, rules, record(), d.journal.head()).verdict.fails() && kept == cut.journal.head());
  }
  bool r1 = false, r2 = false;
  Docket e = opened();
  try { take(e, {"5 January", "Ames", "claim", {}, ""}); } catch (const std::invalid_argument&) { r1 = true; }
  try { take(e, {"2026-01-05", "Carr", "claim", {}, ""}); } catch (const std::invalid_argument&) { r2 = true; }
  s.require("a step whose instant is not a date, or taken by one who is not a party, is refused", r1 && r2);
  return s.done();
}
