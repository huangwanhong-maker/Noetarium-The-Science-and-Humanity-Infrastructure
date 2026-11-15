// Checks of precedent under the reason model (precedent.hpp): a case base of trade secrets read from its written form, and its
// faults; a situation forced a fortiori, one where a precedent is distinguished, a decision forbidden on one reason and permitted
// on another, the result model; an inconsistent case base found with its pair; and the closed forms held to the definitions on
// random case bases, every pair of reasons enumerated.

#include <law/precedent/precedent.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <string>
#include <vector>

using namespace law::precedent;

namespace {

const char* secrets =
    "sides plaintiff defendant\n"
    "factor security plaintiff: the owner took measures to keep the information secret\n"
    "factor agreement plaintiff: the defendant agreed not to disclose it\n"
    "factor unique plaintiff: the product was unique in the market\n"
    "factor disclosed defendant: the owner disclosed it in negotiations\n"
    "factor known defendant: the information was known in the trade\n"
    "factor reverse defendant: the product could be reverse engineered\n"
    "case mason for plaintiff: security, agreement, disclosed\n"
    "reason mason: security\n"
    "case yokana for defendant: unique, disclosed, known\n"
    "reason yokana: known\n";

// The definition, by enumeration: Γ is inconsistent when U <_Γ V and V <_Γ U for some reasons U, V of opposite sides.
bool inconsistent_by_definition(const Base& b) {
  for (int side = 0; side < 2; side++) {
    const std::string s = b.sides[side], o = b.sides[1 - side];
    Factors fs, fo;
    for (const auto& [f, w] : b.side_of) (w == s ? fs : fo).insert(f);
    const std::vector<std::string> vs(fs.begin(), fs.end()), vo(fo.begin(), fo.end());
    for (std::size_t mv = 0; mv < (std::size_t{1} << vs.size()); mv++)
      for (std::size_t mu = 0; mu < (std::size_t{1} << vo.size()); mu++) {
        Factors V, U;
        for (std::size_t i = 0; i < vs.size(); i++)
          if (mv >> i & 1) V.insert(vs[i]);
        for (std::size_t i = 0; i < vo.size(); i++)
          if (mu >> i & 1) U.insert(vo[i]);
        bool u_below_v = false, v_below_u = false;
        for (const auto& c : b.cases) {
          if (c.outcome == s && within(reason_of(b, c), V) && within(U, favouring(b, c.facts, o))) u_below_v = true;
          if (c.outcome == o && within(reason_of(b, c), U) && within(V, favouring(b, c.facts, s))) v_below_u = true;
        }
        if (u_below_v && v_below_u) return true;
      }
  }
  return false;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto b = base(secrets);
  s.require("a case base of trade secrets reads: two sides, six factors, two cases with their reasons", b.side_of.size() == 6 && b.cases.size() == 2 && b.cases[0].reason == Factors{"security"});
  const auto bad = read("sides plaintiff defendant\nfactor x plaintiff: x\nfactor x defendant: again\nfactor y nobody: y\ncase c for plaintiff: x, z\nreason d: x\nreason c: y\n");
  s.require("its faults are found: a factor declared twice, a factor for no side, an undeclared factor in a case, a reason for no case, a reason naming a factor the case lacks",
            !bad.ok && bad.errors.size() == 5, std::to_string(bad.errors.size()));
  s.require("the case base is consistent", consistent(b).holds());

  // ---- The constraint --------------------------------------------------------------------------------------------------------------
  {
    const auto c = constraint(b, {"security", "disclosed"});
    s.require("security measures against disclosure in negotiations: Mason had its reason and every factor against it, so the plaintiff must win",
              c.forced == "plaintiff" && c.forcing == std::vector<std::string>{"mason"} && c.verdict.holds(), c.verdict.reason);
    const auto d = constraint(b, {"security", "disclosed", "reverse"});
    s.require("with reverse engineering added, nothing is forced: Mason is distinguished by {reverse}",
              d.forced.empty() && d.verdict.undecided() && d.distinguished.size() == 1 && d.distinguished[0].by == Factors{"reverse"}, d.verdict.reason);
    s.require("deciding that situation for the defendant on {reverse} is permitted", permitted(b, {"security", "disclosed", "reverse"}, "defendant", {"reverse"}).holds());
    const auto f = permitted(b, {"security", "disclosed", "reverse"}, "defendant", {"disclosed"});
    s.require("but on {disclosed} it is forbidden by Mason, which held security to outweigh disclosure", f.fails() && f.piece("forbidden by") == "mason", f.reason);
    s.require("for the plaintiff it is permitted, on security", permitted(b, {"security", "disclosed", "reverse"}, "plaintiff", {"security"}).holds());
    const auto y = constraint(b, {"known", "agreement"});
    s.require("knowledge in the trade against an agreement: Yokana's reason is present, but the agreement is new against it, so Yokana is distinguished", y.forced.empty() && y.distinguished[0].precedent == "yokana" && y.distinguished[0].by == Factors{"agreement"});
    Base result = b;
    result.cases[0].reason.clear();  // the result model: Mason's reason is every factor for the plaintiff it had
    s.require("under the result model Mason's reason is {agreement, security}, and security alone against disclosure is no longer forced",
              reason_of(result, result.cases[0]) == Factors{"agreement", "security"} && constraint(result, {"security", "disclosed"}).forced.empty());
  }

  // ---- An inconsistent case base -------------------------------------------------------------------------------------------------
  {
    Base worse = b;
    worse.cases.push_back({"rogue", {"security", "disclosed"}, "defendant", {"disclosed"}});
    const auto v = consistent(worse);
    s.require("a later case deciding security against disclosure for the defendant on disclosure makes the base inconsistent, with Mason, named", v.fails() && v.piece("conflict") == "mason and rogue", v.reason);
  }

  // ---- The closed forms held to the definitions -------------------------------------------------------------------------------------
  {
    calx::random::Stream rng(20261115, 70, 0, 0);
    std::size_t bases = 0, inconsistent_seen = 0, wrong_consistency = 0, wrong_permitted = 0, wrong_forced = 0, forced_seen = 0;
    for (int trial = 0; trial < 400; trial++) {
      Base r;
      r.sides = {"p", "d"};
      const std::size_t np = 1 + rng.below(3), nd = 1 + rng.below(3);
      for (std::size_t i = 0; i < np; i++) r.side_of["p" + std::to_string(i)] = "p";
      for (std::size_t i = 0; i < nd; i++) r.side_of["d" + std::to_string(i)] = "d";
      const auto random_case = [&](const std::string& name) {
        Precedent c{name, {}, rng.uniform() < 0.5 ? "p" : "d", {}};
        for (const auto& [f, w] : r.side_of)
          if (rng.uniform() < 0.6) c.facts.insert(f);
        const auto mine = favouring(r, c.facts, c.outcome);
        for (const auto& f : mine)
          if (rng.uniform() < 0.6) c.reason.insert(f);
        return c;
      };
      const std::size_t n = 1 + rng.below(4);
      for (std::size_t i = 0; i < n; i++) r.cases.push_back(random_case("c" + std::to_string(i)));
      bases++;
      const bool by_def = inconsistent_by_definition(r);
      inconsistent_seen += by_def;
      if (by_def == consistent(r).holds()) wrong_consistency++;
      if (by_def) continue;
      // A new situation, a side and a reason: permitted exactly when the base with the new case added stays consistent.
      Factors x;
      for (const auto& [f, w] : r.side_of)
        if (rng.uniform() < 0.6) x.insert(f);
      for (const std::string side : {"p", "d"}) {
        const auto mine = favouring(r, x, side);
        for (std::size_t m = 0; m < (std::size_t{1} << mine.size()); m++) {
          Factors reason;
          std::size_t i = 0;
          for (const auto& f : mine) {
            if (m >> i & 1) reason.insert(f);
            i++;
          }
          if (reason.empty() && !mine.empty()) continue;  // an empty reason reads as the result model's
          Base more = r;
          more.cases.push_back({"new", x, side, reason});
          if (permitted(r, x, side, reason).holds() == inconsistent_by_definition(more)) wrong_permitted++;
        }
        // Forced for the other side exactly when no reason for this side is permitted.
        bool any = false;
        for (std::size_t m = 0; m < (std::size_t{1} << mine.size()); m++) {
          Factors reason;
          std::size_t i = 0;
          for (const auto& f : mine) {
            if (m >> i & 1) reason.insert(f);
            i++;
          }
          if (reason.empty() && !mine.empty()) continue;
          Base more = r;
          more.cases.push_back({"new", x, side, reason});
          any = any || !inconsistent_by_definition(more);
        }
        const std::string opp = side == "p" ? "d" : "p";
        const auto c = constraint(r, x);
        if ((c.forced == opp) != !any) wrong_forced++;
        forced_seen += !any;
      }
    }
    s.require("on 400 random case bases, consistency as the closed form decides it is the definition's, every pair of reasons enumerated (" + std::to_string(inconsistent_seen) + " inconsistent)",
              wrong_consistency == 0, std::to_string(wrong_consistency));
    s.require("a decision is permitted exactly when the base with it added stays consistent, on every reason of every side", wrong_permitted == 0, std::to_string(wrong_permitted));
    s.require("a side is forced exactly when no reason for the other keeps the base consistent: the a fortiori constraint (" + std::to_string(forced_seen) + " forced)", wrong_forced == 0, std::to_string(wrong_forced));
  }
  return s.done();
}
