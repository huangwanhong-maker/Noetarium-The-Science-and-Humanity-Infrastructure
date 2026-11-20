// Checks of economics/ledger: postings refused with their fault named (books that disagree, flows that disagree, money
// changed by a transfer or by someone not its issuer, an opening after trade, a shortage) and nothing changed by a refusal;
// a payment between two banks' customers settled in reserves; money created by lending and destroyed by repayment, act by
// act; the audit over 200,000 random postings, money stock, net worth and sectoral net lending exact; an economy built from
// accounts reproducing them every period, with velocity one where agents hold one period's outflow; money traced from a
// grant: its LM3 the walk's read from the same payments and the closed form's; Drăgulescu and Yakovenko's random exchange
// reaching the exact stationary law of a uniform configuration; a payment queued until money arrives; order mattering in an
// economy of places when households spend by whether they are short of money, and not when they do not.

#include <economics/ledger/accounts.hpp>
#include <economics/ledger/economy.hpp>
#include <economics/ledger/readings.hpp>
#include <economics/ledger/regions.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace economics::ledger;

namespace {

bool says(const std::string& what, const std::string& part) { return what.find(part) != std::string::npos; }

std::string refusal(Ledger& L, const Posting& p) {
  try {
    L.post(p);
  } catch (const Refusal& e) {
    return e.what();
  }
  return "";
}

}  // namespace

int main() {
  calx::verification::Suite s;
  // ---- refusals -------------------------------------------------------------------------------------------------------
  {
    Ledger L;
    const std::size_t mint = L.add_agent({"mint", Sector::mint, ""}), a = L.add_agent({"Ada", Sector::household, "north"}),
                      b = L.add_agent({"Ben", Sector::household, "north"});
    const std::size_t cur = L.add_instrument({"currency", true});
    L.post(opening(a, cur, mint, 100));
    L.post(opening(b, cur, mint, 50));
    Posting half = pay(1, a, b, cur, mint, 10, "purchase");
    half.entries.pop_back();
    s.require("the books disagree: refused, naming the instrument, holder and issuer", says(refusal(L, half), "the books disagree: Ben's currency at mint"));
    Posting gift = pay(1, a, b, cur, mint, 10, "purchase");
    gift.flows.clear();
    s.require("net worth that changes without a flow: refused, naming whose", says(refusal(L, gift), "Ada's net worth changes by -10 but its flows say 0"));
    Posting forge;
    forge.time = 1, forge.act = Act::transfer, forge.responsible = a;
    forge.entries = {{a, Side::asset, cur, mint, 10}, {mint, Side::liability, cur, a, 10}};
    forge.flows = {{mint, a, 10, "transfer"}};
    s.require("money made by a transfer: refused", says(refusal(L, forge), "money changes by a transfer"));
    Posting usurp = pay(1, mint, a, cur, mint, 10, "grant");
    usurp.responsible = b;
    s.require("money issued in another's name: refused", says(refusal(L, usurp), "only an issuer makes or unmakes its money"));
    s.require("a payment beyond what is held: refused as short", says(refusal(L, pay(1, b, a, cur, mint, 60, "purchase")), "short, Ben's currency at mint would be -10"));
    const Amount before = L.money(b);
    s.require("try_post of a payment beyond what is held changes nothing", !L.try_post(pay(1, b, a, cur, mint, 60, "purchase")) && L.money(b) == before && L.postings() == 2);
    L.post(pay(1, a, b, cur, mint, 30, "purchase"));
    s.require("opening balances after trade: refused", says(refusal(L, opening(a, cur, mint, 5)), "opening balances are posted before anything else"));
    L.post(pay(2, b, mint, cur, mint, 20, "tax"));
    s.require("a payment to the issuer is a redemption: money destroyed", L.destroyed() == 20 && L.money_stock() == 130 && L.audit().holds);
    L.post(pay(3, mint, a, cur, mint, 15, "grant"));
    s.require("a payment by the issuer is an issue: money created", L.created() == 15 && L.money_stock() == 145 && L.audit().holds);
    s.require("each agent's net worth moved by its income less its expenditure", L.saving(a) == -30 + 15 && L.saving(b) == 30 - 20 && L.saving(mint) == 20 - 15);
  }
  // ---- banks ----------------------------------------------------------------------------------------------------------
  {
    Ledger L;
    const std::size_t cb = L.add_agent({"central bank", Sector::central_bank, ""}), b1 = L.add_agent({"bank one", Sector::bank, ""}),
                      b2 = L.add_agent({"bank two", Sector::bank, ""}), x = L.add_agent({"Xia", Sector::household, ""}), y = L.add_agent({"Yuri", Sector::firm, ""});
    const std::size_t dep = L.add_instrument({"deposit", true}), res = L.add_instrument({"reserve", false}), loan = L.add_instrument({"loan", false});
    L.post(opening(b1, res, cb, 500));
    L.post(opening(b2, res, cb, 500));
    L.post(lend(0, b1, x, loan, dep, 300));
    s.require("a loan creates its deposit: money made by the act of lending", L.created() == 300 && L.by_act().at(Act::lend) == 300 && L.money(x) == 300);
    L.post(pay_across(1, x, b1, y, b2, dep, res, cb, 120, "purchase"));
    s.require("a payment between two banks' customers: eight entries, reserves passed, money unchanged",
              L.held(y, dep, b2) == 120 && L.held(b1, res, cb) == 380 && L.held(b2, res, cb) == 620 && L.money_stock() == 300 && L.audit().holds);
    s.require("... and a payment beyond the payer's deposit is short, changing nothing", !L.try_post(pay_across(2, x, b1, y, b2, dep, res, cb, 181, "purchase")) && L.held(x, dep, b1) == 180);
    L.post(repay(3, b1, x, loan, dep, 100));
    s.require("repayment destroys money by its act", L.destroyed() == 100 && L.by_act().at(Act::repay) == -100 && L.money_stock() == 200 - 0 && L.audit().holds);
  }
  // ---- conservation over a long random run ----------------------------------------------------------------------------
  {
    Ledger L;
    const std::size_t gov = L.add_agent({"government", Sector::government, ""}), bank = L.add_agent({"bank", Sector::bank, ""});
    std::vector<std::size_t> A;
    for (int i = 0; i < 48; i++) A.push_back(L.add_agent({"agent " + std::to_string(i), i % 3 == 0 ? Sector::firm : Sector::household, i % 2 ? "east" : "west"}));
    const std::size_t cur = L.add_instrument({"currency", true}), dep = L.add_instrument({"deposit", true}), loan = L.add_instrument({"loan", false});
    for (const std::size_t a : A) L.post(opening(a, cur, gov, 1000));
    calx::random::Stream r(11, 2, 0, 0);
    std::size_t tried = 0, short_ = 0;
    std::vector<Amount> lent(L.agents().size(), 0);
    for (int k = 0; k < 200000; k++) {
      const std::size_t i = A[r.below(A.size())], j = A[r.below(A.size())];
      const double u = r.uniform();
      Posting p;
      if (u < 0.80 && i != j) p = pay(k, i, j, r.uniform() < 0.5 ? cur : dep, r.uniform() < 0.5 ? gov : bank, 1 + static_cast<Amount>(r.below(200)), "purchase");
      else if (u < 0.85) p = pay(k, i, L.outside(), cur, gov, 1 + static_cast<Amount>(r.below(50)), "import");
      else if (u < 0.88) p = pay(k, L.outside(), i, cur, gov, 1 + static_cast<Amount>(r.below(50)), "export");
      else if (u < 0.92) p = pay(k, i, gov, cur, gov, 1 + static_cast<Amount>(r.below(40)), "tax");
      else if (u < 0.95) p = pay(k, gov, i, cur, gov, 1 + static_cast<Amount>(r.below(40)), "transfer");
      else if (u < 0.98) {
        const Amount v = 1 + static_cast<Amount>(r.below(300));
        p = lend(k, bank, i, loan, dep, v);
      } else {
        const Amount owe = L.owed(i, loan, bank) > 0 ? L.held(bank, loan, i) : 0;
        if (owe == 0) continue;
        p = repay(k, bank, i, loan, dep, 1 + static_cast<Amount>(r.below(static_cast<std::uint64_t>(owe))));
      }
      tried++;
      if (!L.try_post(p)) short_++;
    }
    const Verdict v = L.audit();
    s.require("200,000 random postings: the audit holds", v.holds, v.failure);
    s.require("... the money stock is what was opened, created and destroyed, exactly", L.money_stock() == 48000 + L.created() - L.destroyed());
    Amount nl = 0;
    for (const auto& [sec, x] : net_lending(L)) nl += x;
    s.require("... sectoral net lending sums to zero, outside included", nl == 0);
    Amount nfa = 0;
    for (std::size_t a = 0; a < L.agents().size(); a++) nfa += L.net_worth(a);
    s.require("... and net financial assets to zero", nfa == 0);
    s.require("... with shortages met by refusing, not by overdrawing", short_ > 0 && short_ < tried);
  }
  // ---- an economy from accounts ---------------------------------------------------------------------------------------
  {
    Economy E;
    Accounts A;
    A.sectors = {"households", "firms", "outside"};
    A.kinds = {Sector::household, Sector::firm, Sector::outside};
    A.members = {4, 2, 1};
    A.regions = {"town", "town", ""};
    // Paid per period, rows the payers: households 1,000 out and in; firms 1,000; outside 600.
    A.flows = {{0, 800, 200}, {600, 0, 400}, {400, 200, 0}};
    const auto st = steady_state(E, A, 1);
    const std::size_t periods = 200;
    const auto o = E.run(periods);
    s.require("an economy from accounts: the audit holds after 200 periods", o.audit.holds, o.audit.failure);
    // Every period's payments between sectors are the accounts'.
    std::vector<std::size_t> sector(E.L.agents().size(), none);
    for (std::size_t i = 0; i < 3; i++)
      for (const std::size_t a : st.agents[i]) sector[a] = i;
    bool every = true;
    for (std::size_t t = 0; t < periods && every; t++) {
      const auto F = gather(E.recorder.payments, sector, t, t + 1);
      for (std::size_t i = 0; i < 3; i++)
        for (std::size_t j = 0; j < 3; j++)
          if (i != j) {
            const auto it = F.find({i, j});
            const double got = it == F.end() ? 0 : it->second;
            every = every && std::fabs(got - static_cast<double>(A.flows[i][j])) < 1e-9;
          }
    }
    s.require("... and reproduces the accounts in every period, exactly", every);
    bool still = true;
    for (const auto& h : E.holdings)
      for (std::size_t a = 0; a < h.size(); a++) still = still && h[a] == E.holdings[0][a];
    s.require("... each agent holding the same money at each period's end", still);
    const auto V = velocity(E.recorder.payments, E.holdings, sector, 3, 0, periods);
    s.check("velocity one per period where each agent holds one period's outflow (households)", V[0].velocity - 1, 1e-12);
    s.check("... and the mean time a unit is held, one period (firms)", V[1].holding - 1, 1e-12);
    const auto C = circulation(gather(E.recorder.payments, sector, 0, periods), 3);
    s.require("every sector pays every other, one strongly connected component: all the value in it", C.components == 1 && std::fabs(C.in_strong - 1) < 1e-12);
    s.require("... and the Hodge decomposition splits the flow into its driven and circulating parts", std::fabs(C.driven + C.circulating - 1) < 1e-9 && C.circulating > 0);
    s.require("written rules alone: the run is computed, no decision by model-generated behaviour", std::string(o.tally.standing()) == "computed" && o.tally.model_share() == 0);
  }
  // ---- money traced from a grant ----------------------------------------------------------------------------------------
  {
    Economy E(8);
    Accounts A;
    A.sectors = {"households", "firms", "outside"};
    A.kinds = {Sector::household, Sector::firm, Sector::outside};
    A.members = {2, 2, 1};
    A.regions = {"town", "town", ""};
    // Households spend 80% at the town's firms; firms pay 60% to the town's households; the rest goes outside.
    A.flows = {{0, 8000000, 2000000}, {6000000, 0, 4000000}, {4000000, 2000000, 0}};
    const auto st = steady_state(E, A, 1);
    E.grants.push_back({0.0, none, st.agents[0][0], 5000000, "the grant", "grant"});
    const auto o = E.run(80);
    s.require("a grant into the steady economy: the audit holds", o.audit.holds, o.audit.failure);
    const auto& T = E.tracer.trace("the grant");
    // The walk from the grant's recipient, read from the same payments.
    std::vector<char> town(E.L.agents().size(), 0);
    for (std::size_t i = 0; i < 2; i++)
      for (const std::size_t a : st.agents[i]) town[a] = 1;
    std::vector<std::size_t> self(E.L.agents().size());
    for (std::size_t a = 0; a < self.size(); a++) self[a] = a;
    const auto W = markov_retention(gather(E.recorder.payments, self, 0, 80), E.L.agents().size(), town, st.agents[0][0]);
    s.check("LM3 of the traced grant: (R1 + R2 + R3)/R1 against the closed form 1 + 0.8 + 0.8·0.6", T.lm3() - 2.28, 1e-6);
    s.check("... against the walk of money read from the same payments, 1 + r1 + r2", T.lm3() - W.lm3, 1e-6);
    s.check("the expected local holdings before leaving: traced against the walk", T.rounds() - W.rounds, 1e-3);
    s.check("... against the closed form (1 + 0.8)/(1 − 0.48)", W.rounds - 1.8 / 0.52, 1e-6);
    s.check("what leaves the town: traced (leaked) against the walk's share, nothing destroyed and little left", T.leaked_total() / T.injected - W.leak, 1e-5);
  }
  // ---- random exchange ------------------------------------------------------------------------------------------------
  {
    Ledger L;
    const std::size_t mint = L.add_agent({"mint", Sector::mint, ""});
    RandomExchange::State st;
    st.L = &L;
    const std::size_t N = 100, M = 500;
    for (std::size_t i = 0; i < N; i++) st.agents.push_back(L.add_agent({"agent " + std::to_string(i), Sector::household, ""}));
    st.money = L.add_instrument({"currency", true});
    st.issuer = mint;
    for (const std::size_t a : st.agents) L.post(opening(a, st.money, mint, static_cast<Amount>(M / N)));
    RandomExchange X;
    std::vector<double> seen(M + 1, 0);
    double samples = 0;
    double t = 0;
    for (int chunk = 0; chunk < 20000; chunk++) {
      nomos::agents::gillespie(X, st, t, t + 1, 1000 + static_cast<std::uint64_t>(chunk));
      t += 1;
      if (chunk < 500) continue;  // let it forget where it began
      for (const std::size_t a : st.agents) seen[static_cast<std::size_t>(L.money(a))] += 1;
      samples += static_cast<double>(N);
    }
    const auto law = exchange_law(N, M);
    double tv = 0, mean = 0;
    for (std::size_t m = 0; m <= M; m++) tv += std::fabs(seen[m] / samples - law[m]) / 2, mean += static_cast<double>(m) * seen[m] / samples;
    s.require("random exchange: every one of its postings passed, and the audit holds", L.audit().holds && L.postings() > 100000);
    s.check("... the money each agent holds reaches the exact law of a uniform configuration (total variation)", tv, 0.02);
    s.check("... whose mean is M/N exactly, as nothing is made or lost", mean - 5, 1e-9);
  }
  // ---- a payment queued until money arrives ------------------------------------------------------------------------
  {
    Economy E;
    const std::size_t a = E.L.add_agent({"Ada", Sector::firm, ""}), b = E.L.add_agent({"Ben", Sector::household, ""}), c = E.L.add_agent({"Cy", Sector::firm, ""});
    E.issuer = E.L.add_agent({"mint", Sector::mint, ""});
    E.money = E.L.add_instrument({"currency", true});
    E.start();
    E.L.post(opening(c, E.money, E.issuer, 100));
    nomos::agents::Behaviour wages;
    wages.declared = {"pay wages", nomos::agents::Origin::rule, {}, {"wage"}, "a check"};
    wages.decide = [b](const nomos::agents::Observation&, calx::random::Stream&) { return std::vector<nomos::agents::Action>{{"wage", b, 80}}; };
    nomos::agents::Behaviour buys;
    buys.declared = {"buy", nomos::agents::Origin::rule, {"period"}, {"purchase"}, "a check"};
    buys.decide = [a](const nomos::agents::Observation& o, calx::random::Stream&) {
      return o.at("period") == 0 ? std::vector<nomos::agents::Action>{{"purchase", a, 100}} : std::vector<nomos::agents::Action>{};
    };
    E.behaviour[a] = wages;
    E.behaviour[c] = buys;
    E.order = {a, c};
    const auto o = E.run(1);
    s.require("a wage that cannot be paid waits, and is paid when the firm is paid, in the same period",
              o.queued == 1 && o.unsettled == 0 && E.L.money(b) == 80 && E.L.money(a) == 20 && o.audit.holds);
    const auto o2 = E.run(1);
    s.require("... and the next period's wage, beyond what is held, still waits at the end", o2.unsettled == 1 && o2.unsettled_amount == 80 && E.owed(a) == 80);
  }
  // ---- order in an economy of places ------------------------------------------------------------------------------------
  {
    auto local_spending = [](const Habits& H, bool first_north) {
      Economy E(8);
      std::vector<Place> P = {{"north", 4, 2, 200000, 400000}, {"south", 4, 2, 20000, 400000}};
      const auto places = regional(E, P, H);
      const std::vector<std::pair<std::size_t, Amount>> g = first_north ? std::vector<std::pair<std::size_t, Amount>>{{0, 600000}, {1, 600000}}
                                                                         : std::vector<std::pair<std::size_t, Amount>>{{1, 600000}, {0, 600000}};
      grant_places(E, places, g, {0.0, 3.0});
      const auto o = E.run(120);
      double lm3 = 0;
      for (const auto& t : E.tracer.traces()) lm3 += t.lm3();
      return std::make_pair(lm3, o.audit.holds);
    };
    Habits state;  // households short of money spend differently
    state.short_below = 60000;
    const auto a = local_spending(state, true), b = local_spending(state, false);
    s.require("an economy of places: the audit holds under both orders", a.second && b.second);
    s.require("households that spend by whether they are short: the order of two grants changes their LM3", std::fabs(a.first - b.first) > 1e-3,
              std::to_string(a.first) + " against " + std::to_string(b.first));
    Habits linear = state;  // the same habits whether short or not
    linear.short_spend = linear.spend, linear.short_local = linear.local;
    const auto c = local_spending(linear, true), d = local_spending(linear, false);
    s.check("households whose habits do not depend on their state: the order leaves each grant's LM3 as it was", c.first - d.first, 1e-4);
  }
  return s.done();
}
