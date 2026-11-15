// Checks of the law domain's facts: the order of kinds and their registers; the steps by which a standing changes; a record of
// a sale that is well formed; and each rule a record must keep, broken one at a time and found.

#include <law/facts/facts.hpp>

#include <calx/verification/check.hpp>

#include <string>

using namespace law::facts;

namespace {

// Rivers v. Mill: was the mill sold, and was it paid for?
Record sale() {
  Record r;
  r.proceeding = "Rivers v. Mill";
  Fact signing = fact("signing", "what took place at the notary's on 3 May");
  Fact sig = fact("signed", "Rivers signed the deed");
  sig.kind = "Social", sig.standing = Standing::proved, sig.role = Role::evidentiary, sig.about = "signing";
  sig.emerged_from = "the meeting at the notary's", sig.since = "2019-05-03", sig.constrains = {"sale", "intent"};
  Fact sale = fact("sale", "a contract of sale of the mill exists between Rivers and Mill");
  sale.kind = "Institutional", sale.standing = Standing::proved, sale.role = Role::ultimate;
  sale.emerged_from = "the signing of the deed", sale.since = "2019-05-03", sale.constrains = {"price_due"};
  Fact intent = fact("intent", "Rivers meant to sell");
  intent.kind = "Mental", intent.standing = Standing::presumed, intent.basis = "signed";
  intent.emerged_from = "the signature, as the law reads it", intent.since = "2019-05-03", intent.constrains = {"sale"};
  Fact ledger = fact("ledger", "the mill's ledger lists two receipts in May, neither from Mill");
  ledger.kind = "Social", ledger.standing = Standing::proved, ledger.role = Role::evidentiary;
  ledger.emerged_from = "the bookkeeping of the mill", ledger.since = "2019-05-31", ledger.constrains = {"unpaid"};
  Fact unpaid = fact("unpaid", "Mill did not pay the price");
  unpaid.kind = "Social", unpaid.standing = Standing::proved, unpaid.polarity = Polarity::negative;
  unpaid.emerged_from = "the dealings of the parties after the sale", unpaid.since = "2019-06-01", unpaid.constrains = {"price_due"};
  Fact market = fact("market", "mills of this kind sold in the district for ten to twelve thousand");
  market.kind = "Social", market.standing = Standing::noticed, market.scope = Scope::legislative, market.generality = Generality::statistical, market.basis = "the district's published register of sales";
  market.emerged_from = "the sales of the district", market.since = "2015-01-01", market.constrains = {"price_due"};
  r.facts = {signing, sig, sale, intent, ledger, unpaid, market};
  r.traces = {{"t1", "signed", "digest-of-the-deed", "Document"}, {"t2", "signed", "digest-of-the-notary's-statement", "Testimony"}, {"t3", "ledger", "digest-of-the-ledger", "Document"}};
  r.inferences = {{"i1", "signed", "sale", "who signs a deed of sale sells", false}, {"i2", "ledger", "unpaid", "what a careful ledger does not list was not received", false}};
  return r;
}

bool has(const std::vector<Fault>& fs, const std::string& where, const std::string& part) {
  for (const auto& f : fs)
    if (f.where == where && f.what.find(part) != std::string::npos) return true;
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- Kinds and registers -----------------------------------------------------------------------------------------------------
  s.require("a custom is a norm, an institutional fact, a social fact and a relational one, and no trace", is_a("Custom", "Normative") && is_a("Custom", "Institutional") && is_a("Custom", "Social") && is_a("Custom", "Relational") && !is_a("Custom", "Trace"));
  s.require("the registers: an occurrence is ontological, a state of mind and a convention relational, testimony manifestational",
            register_of("Occurrence") == Register::ontological && register_of("Mental") == Register::relational && register_of("Convention") == Register::relational && register_of("Testimony") == Register::manifestational);

  // ---- Standings -----------------------------------------------------------------------------------------------------------------
  {
    using S = Standing;
    const S all[] = {S::alleged, S::admitted, S::noticed, S::presumed, S::deemed, S::proved, S::not_proved, S::rebutted, S::found, S::final_};
    std::size_t steps = 0;
    for (S a : all)
      for (S b : all) steps += can_become(a, b);
    s.require("thirteen steps change a standing, the schema's thirteen arrows", steps == 13, std::to_string(steps));
    s.require("what is not proved, rebutted or final changes no more", !can_become(S::not_proved, S::proved) && !can_become(S::rebutted, S::found) && !can_become(S::final_, S::alleged));
    Fact f = fact("f", "x");
    advance(f, S::proved);
    advance(f, S::found, "the judgment of 30 June");
    bool refused = false;
    try { advance(f, S::alleged); } catch (const std::invalid_argument&) { refused = true; }
    s.require("an allegation is proved, then found by a decision; a finding does not go back to an allegation", f.standing == S::found && f.basis == "the judgment of 30 June" && refused);
    s.require("a presumed fact stands until rebutted; a fact not proved does not stand", established(S::presumed) && !established(S::rebutted) && !established(S::not_proved) && !established(S::alleged));
  }

  // ---- A record that is well formed -------------------------------------------------------------------------------------------------
  const Record r = sale();
  s.require("the record of a sale (an occurrence left open, what is proved of it, a presumed intent, a negative fact inferred, a statistical fact noticed) is well formed", well_formed(r).holds(), well_formed(r).reason);

  // ---- Each rule, broken ---------------------------------------------------------------------------------------------------------
  {
    Record b = r;
    b.facts[0].standing = Standing::proved;
    s.require("an occurrence is left open: marking what took place as proved is a fault, and says what is proved instead", has(faults(b), "signing", "left open"));
  }
  {
    Record b = r;
    b.facts[2].constrains.clear();
    b.facts[2].emerged_from.clear();
    const auto fs = faults(b);
    s.require("a relational fact that stands and does not say what it emerged from, or what it constrains, is at fault in each", has(fs, "sale", "emerged from") && has(fs, "sale", "constrains"));
    Record c = r;
    c.facts[2].standing = Standing::alleged;
    c.facts[2].since.clear();
    s.require("while it is only alleged it need not say so", !has(faults(c), "sale", "persists"));
  }
  {
    Record b = r;
    b.facts[3].basis = "unpaid_rent";
    s.require("a presumption with no basic fact in the record is a fault", has(faults(b), "intent", "basic fact"));
    Record c = r;
    c.facts[1].standing = Standing::alleged;
    c.traces.clear();
    s.require("a presumption whose basic fact does not itself stand is a fault", has(faults(c), "intent", "does not itself stand"));
  }
  {
    Record b = r;
    b.facts[3].role = Role::evidentiary;
    s.require("a state of mind is not what a trace shows", has(faults(b), "intent", "reached by inference"));
    Record c = r;
    c.inferences.pop_back();
    s.require("a negative fact that stands by proof, with no inference leading to it, is a fault", has(faults(c), "unpaid", "cannot be observed"));
  }
  {
    Record b = r;
    b.traces.pop_back();
    s.require("an evidentiary fact that stands by proof and has no trace is a fault", has(faults(b), "ledger", "has a trace"));
    Record c = r;
    c.traces[0].kind = "Mental";
    c.traces[1].holding.clear();
    const auto fs = faults(c);
    s.require("a trace of a kind that is no trace, and a trace that names no holding, are faults", has(fs, "t1", "not Mental") && has(fs, "t2", "names none"));
  }
  {
    Record b = r;
    b.facts[6].generality = Generality::particular;
    s.require("a legislative fact that is particular is a fault", has(faults(b), "market", "legislative"));
    Record c = r;
    c.facts[6].basis.clear();
    s.require("a noticed fact says why it needs no proof", has(faults(c), "market", "why it needs no proof"));
  }
  {
    Record b = r;
    b.inferences.push_back({"i3", "sale", "signed", "x", false});
    const auto fs = faults(b);
    bool cycle = false;
    for (const auto& f : fs) cycle = cycle || f.what.find("cycle") != std::string::npos;
    s.require("an inference from an ultimate fact back to the evidence, closing a cycle, is at fault twice", has(fs, "i3", "back to one nearer the evidence") && cycle);
    Record c = r;
    c.inferences[0].generalisation.clear();
    s.require("an inference without its generalisation is a fault", has(faults(c), "i1", "generalisation"));
  }
  {
    Record b = r;
    b.facts[1].kind = "Document";
    s.require("a fact may not be of a kind of trace", has(faults(b), "signed", "not a kind a fact may be"));
    s.require("the verdict carries every fault", well_formed(b).fails() && well_formed(b).evidence.size() == faults(b).size());
  }

  return s.done();
}
