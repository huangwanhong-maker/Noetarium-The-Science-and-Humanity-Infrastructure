// Checks of the law domain's sources: whom a treaty binds and when; which source prevails, by each maxim and by their order;
// a custom identified by its two elements under a declared scheme, a usage told from it, and a persistent objector.

#include <law/sources/sources.hpp>

#include <calx/verification/check.hpp>

#include <cmath>
#include <string>

using namespace law::sources;

int main() {
  calx::verification::Suite s;

  // ---- Treaties ----------------------------------------------------------------------------------------------------------------
  {
    Source t{"river", "the River Convention", Kind::treaty, "1998-01-01", "", 0, {"Alba", "Borea"}, ""};
    s.require("a treaty binds a party while it is in force", binds(t, "Alba", "2010-06-01").holds());
    const auto third = binds(t, "Cintra", "2010-06-01");
    s.require("it does not bind a third State", third.fails() && third.reason.find("third State") != std::string::npos);
    s.require("nor a party before it entered into force", binds(t, "Alba", "1997-12-31").fails());
    s.require("decisions and teachings are subsidiary means, and treaties, custom and principles are not", subsidiary(Kind::decision) && subsidiary(Kind::teaching) && !subsidiary(Kind::treaty) && !subsidiary(Kind::custom) && !subsidiary(Kind::principle));
  }

  // ---- Which prevails ---------------------------------------------------------------------------------------------------------------
  {
    const Source constitution{"c", "the Constitution", Kind::constitution, "1950-01-01", "", 0, {}, ""};
    const Source act{"a", "the Parks Act", Kind::statute, "1990-01-01", "", 1, {}, ""};
    const Source later{"l", "the Cycling Act", Kind::statute, "2005-01-01", "", 1, {}, ""};
    const Source bylaw{"b", "the Park Bylaw", Kind::regulation, "2010-01-01", "", 2, {}, ""};
    const auto p1 = prevails(act, bylaw);
    s.require("a statute prevails over a later bylaw: lex superior comes first", p1.decided && p1.winner == "a" && p1.by == Maxim::superior);
    const auto p2 = prevails(act, later);
    s.require("of two statutes the later prevails: lex posterior", p2.decided && p2.winner == "l" && p2.by == Maxim::posterior);
    const auto p3 = prevails(act, later, {Maxim::superior, Maxim::specialis, Maxim::posterior}, {{"a", "l"}});
    s.require("unless the earlier is special to it: lex specialis before lex posterior", p3.decided && p3.winner == "a" && p3.by == Maxim::specialis);
    const auto p4 = prevails(act, later, {Maxim::superior, Maxim::posterior, Maxim::specialis}, {{"a", "l"}});
    s.require("with the maxims ordered otherwise, the later prevails over the special", p4.winner == "l" && p4.by == Maxim::posterior);
    const Source twin{"t", "another Act of that day", Kind::statute, "1990-01-01", "", 1, {}, ""};
    s.require("two sources of one rank and one date, neither special: nothing decides, and that is said", !prevails(act, twin).decided);
    s.require("the constitution prevails over every other", prevails(bylaw, constitution).winner == "c");
  }

  // ---- Custom ------------------------------------------------------------------------------------------------------------------------
  {
    // Ten States border a sea; eight have fished within a line and treated it as a right; one deviated once; one has objected throughout.
    const std::set<std::string> concerned = {"A", "B", "C", "D", "E", "F", "G", "H", "I", "J"};
    std::vector<Practice> practice;
    for (const char* st : {"A", "B", "C", "D", "E", "F", "G", "H"}) {
      practice.push_back({st, "1990-01-01", true, true, false});
      practice.push_back({st, "2000-01-01", true, false, false});
    }
    practice.push_back({"H", "1995-01-01", false, false, false});  // one deviation
    practice.push_back({"J", "1989-01-01", false, false, true});   // J objects, then and after
    practice.push_back({"J", "1999-01-01", false, false, true});
    const Scheme scheme{0.6, 0.8, 0.5};
    const auto c = identify(practice, concerned, scheme, "2005-01-01");
    s.require("a custom exists under the declared scheme: both elements met", c.verdict.holds() && c.practice && c.opinio_juris, c.verdict.reason);
    s.check("generality: 8 of the 9 States that are not objectors have practised", std::abs(c.generality - 8.0 / 9), 1e-15);
    s.check("consistency: 16 of 17 instances conform", std::abs(c.consistency - 16.0 / 17), 1e-15);
    s.check("acceptance: all 8 practising States accept it as law", std::abs(c.acceptance - 1), 1e-15);
    s.require("J, which objected at every instance before the rule formed, is a persistent objector, and the custom does not bind it", c.objectors == std::set<std::string>{"J"} && !binds(c, "J") && binds(c, "A") && binds(c, "I"));

    // The same practice with no evidence that any State took it for law: a usage.
    auto usage = practice;
    for (auto& p : usage) p.accepted_as_law = false;
    const auto u = identify(usage, concerned, scheme, "2005-01-01");
    s.require("the same practice without acceptance as law is a usage, not a custom", u.verdict.fails() && u.practice && !u.opinio_juris && u.verdict.reason.find("a usage, not a custom") != std::string::npos);

    // Practice by three States only.
    std::vector<Practice> few = {{"A", "1990-01-01", true, true, false}, {"B", "1990-01-01", true, true, false}, {"C", "1990-01-01", true, true, false}};
    const auto f = identify(few, concerned, scheme, "2005-01-01");
    s.require("practice by three of ten is not general under the scheme, whatever its acceptance", f.verdict.fails() && !f.practice && f.opinio_juris);
    const auto loose = identify(few, concerned, Scheme{0.3, 0.8, 0.5}, "2005-01-01");
    s.require("under a scheme that asks three in ten it is: the answer is the scheme's, and the measures are the same", loose.verdict.holds() && loose.generality == f.generality);

    // A State that objected once and then conformed is no persistent objector.
    auto wavering = practice;
    wavering.push_back({"I", "1988-01-01", false, false, true});
    wavering.push_back({"I", "2001-01-01", true, true, false});
    s.require("a State that objected once and then conformed is not a persistent objector", identify(wavering, concerned, scheme, "2005-01-01").objectors == std::set<std::string>{"J"});
  }

  return s.done();
}
