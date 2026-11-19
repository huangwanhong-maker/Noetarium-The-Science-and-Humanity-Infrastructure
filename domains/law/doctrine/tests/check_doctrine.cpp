// Checks of theories of the law (ADR 0133), on an invented park: a bylaw forbids vehicles, "vehicle" read broadly or narrowly, an
// exception for emergencies, and a case base on what counts as an emergency whose courts left their reasons unstated. Every value
// is worked by hand here; the search is held to the exhaustive best of each kind; what is held back is shown never to reach it.
//
// The decided cases on "vehicle": D1 a car, prohibited (both readings agree); D2 a bicycle, not prohibited (binding: the narrow
// reading agrees); D3 a tandem, prohibited (a foreign court, persuasive at 0.3: the broad reading agrees); D4 a pram, not prohibited,
// decided after the question and held back. Misfit: broad 1, narrow 0.3.
//
// The precedents on "emergency" (sides emergency / no_emergency): factors for an emergency life_at_risk (L), rushing_to_care (R),
// child (C); against it alternatives (A), no_injury (N), crowded (W).
//   doctor   for emergency     {L, R, A}       ratio ⊆ {L, R}
//   commuter for no_emergency  {R, A, N}       ratio ⊆ {A, N}
//   parent   for emergency     {C, L, A, W}    ratio ⊆ {C, L}
//   cyclist  for no_emergency  {R, C, A, N}    ratio ⊆ {A, N}
//   jogger   for no_emergency  {R, W, A}       held back
// The matter's situation {R, C, A}: X^em = {R, C}, X^no = {A}. Emergency is forced iff doctor's ratio is {R} or parent's is {C};
// no emergency is forced iff cyclist's ratio is {A}. Conflicts: doctor {R} with commuter {A}; doctor {R} with cyclist {A}; parent {C}
// with cyclist {A}. The jogger is predicted iff commuter's or cyclist's ratio is {A}, else open.

#include <law/doctrine/search.hpp>
#include <law/norms/norms.hpp>

#include <archeion/fixity/journal.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace law;
using doctrine::Doctrine;
using doctrine::Outcome;
using doctrine::Problem;

namespace {

norms::Norms park() {
  norms::Norms n;
  n.rules = {{"ban", "", {"vehicle", "in_park"}, "prohibited"}};
  n.exceptions = {{"emergency_exception", "ban", {"emergency"}}};
  n.terms = {{"vehicle", {"broad", "narrow"}}};
  n.counts = {{"bicycle_counts", "bicycle", "vehicle", "broad", "vehicle"}, {"car_broad", "car", "vehicle", "broad", "vehicle"}, {"car_narrow", "car", "vehicle", "narrow", "vehicle"}};
  return n;
}

facts::Fact admitted(const std::string& id) {
  facts::Fact f = facts::fact(id, id);
  f.standing = facts::Standing::admitted;
  return f;
}

Problem problem() {
  Problem p;
  facts::Record r;
  r.facts = {admitted("bicycle"), admitted("in_park")};
  p.matter = norms::subsume(r, park(), {"prohibited"});
  p.questions = {"prohibited"};
  p.forum = {"magistrates", "the magistrates", {"Avalon"}};
  p.courts = {{"high", "the High Court", "Avalon", 1, {"bylaw"}}, {"supreme", "the Supreme Court", "Avalon", 2, {"bylaw"}}, {"erewhon", "the Court of Erewhon", "Erewhon", 2, {"bylaw"}}};
  p.date = "2026-06-01";
  p.decided = {{"D1", "Council v Driver", "high", "2015-01-01", "bylaw", {"car", "in_park"}, "prohibited", true, "", false},
               {"D2", "Council v Cyclist", "high", "2016-01-01", "bylaw", {"bicycle", "in_park"}, "prohibited", false, "", false},
               {"D3", "Borough v Tandem", "erewhon", "2019-01-01", "bylaw", {"bicycle", "in_park"}, "prohibited", true, "", false},
               {"D4", "Council v Carter", "supreme", "2027-02-01", "bylaw", {"bicycle", "in_park"}, "prohibited", false, "", true}};
  doctrine::CaseBase b;
  b.fact = "emergency";
  b.base.sides = {"emergency", "no_emergency"};
  for (const char* f : {"L", "R", "C"}) b.base.side_of[f] = "emergency";
  for (const char* f : {"A", "N", "W"}) b.base.side_of[f] = "no_emergency";
  b.base.cases = {{"doctor", {"L", "R", "A"}, "emergency", {}},
                  {"commuter", {"R", "A", "N"}, "no_emergency", {}},
                  {"parent", {"C", "L", "A", "W"}, "emergency", {}},
                  {"cyclist", {"R", "C", "A", "N"}, "no_emergency", {}},
                  {"jogger", {"R", "W", "A"}, "no_emergency", {}}};
  b.situation = {"R", "C", "A"};
  b.held_back = {"jogger"};
  p.bases = {b};
  p.purposes = {{"safety", 1, {{"vehicle", {1.0, 0.4}}}}, {"liberty", 1, {{"vehicle", {0.0, 1.0}}}}};
  return p;
}

// A doctrine by hand: the reading of "vehicle" (0 broad, 1 narrow) and the ratios of doctor, commuter, parent and cyclist.
Doctrine make(const Problem& p, std::size_t reading, doctrine::Factors doctor, doctrine::Factors commuter, doctrine::Factors parent, doctrine::Factors cyclist) {
  Doctrine d = doctrine::plain(p);
  d.readings = {reading};
  d.ratios[0][0] = doctor, d.ratios[0][1] = commuter, d.ratios[0][2] = parent, d.ratios[0][3] = cyclist;
  return d;
}

// The hand's answer for a doctrine: score and kind, from the formulas of the header comment.
struct Hand {
  double score;
  int prohibited;  // 0 fails, 1 holds, 2 turns
  int emergency;   // 0 held, 1 denied, 2 open
};
Hand by_hand(std::size_t reading, const doctrine::Factors& doctor, const doctrine::Factors& commuter, const doctrine::Factors& parent, const doctrine::Factors& cyclist) {
  const bool em = doctor == doctrine::Factors{"R"} || parent == doctrine::Factors{"C"};
  const bool no = cyclist == doctrine::Factors{"A"};
  int conflicts = 0;
  if (doctor == doctrine::Factors{"R"} && commuter == doctrine::Factors{"A"}) conflicts++;
  if (doctor == doctrine::Factors{"R"} && cyclist == doctrine::Factors{"A"}) conflicts++;
  if (parent == doctrine::Factors{"C"} && cyclist == doctrine::Factors{"A"}) conflicts++;
  const double misfit = reading == 0 ? 1.0 : 0.3, just = reading == 0 ? 0.5 : 0.7;
  Hand h;
  h.score = misfit + 2.0 * conflicts - just;
  // With a conflict the base is inconsistent; a side forced by both is no side (the precedents force each other's undoing).
  const int state = em && no ? 2 : em ? 0 : no ? 1 : 2;
  h.emergency = state;
  if (reading == 1) h.prohibited = 0;
  else h.prohibited = state == 0 ? 0 : state == 1 ? 1 : 2;
  return h;
}

std::string oneline(std::string t) {
  for (auto& c : t)
    if (c == '\n') c = '|';
  return t;
}

std::vector<doctrine::Factors> subsets(std::vector<std::string> f) {
  std::vector<doctrine::Factors> out;
  for (std::size_t m = 1; m < (std::size_t{1} << f.size()); m++) {
    doctrine::Factors x;
    for (std::size_t q = 0; q < f.size(); q++)
      if ((m >> q) & 1) x.insert(f[q]);
    out.push_back(x);
  }
  return out;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const Problem p = problem();
  s.require("the invented park has no fault", doctrine::faults(p).empty());
  s.require("its doctrines number 2 × 3 × 3 × 3 × 3 = 162", doctrine::size(p) == 162);

  // ---- The written form ----
  {
    const Doctrine d = make(p, 1, {"L"}, {"N"}, {"C", "L"}, {"N"});
    const std::string text = doctrine::write(p, d);
    const auto back = doctrine::read(p, text);
    s.require("a doctrine written reads back as itself", back.ok && doctrine::key(p, back.doctrine) == text, text);
    s.require("the held-back case's ratio is not written", text.find("jogger") == std::string::npos);
    const auto bad = doctrine::read(p, "reading vehicle = wide\nreading colour = red\nratio emergency/doctor: A\nratio emergency/jogger: R\nratio storm/doctor: L\nratio emergency/judge: L\nvehicle broad\nratio emergency/doctor: L, R");
    s.require("each bad line refused on its own, with its line; the good one taken", !bad.ok && bad.errors.size() == 7 && bad.doctrine.ratios[0][0] == doctrine::Factors{"L", "R"},
              std::to_string(bad.errors.size()));
    s.require("a factor the case did not have for its outcome is named, with what it had", bad.errors.size() > 2 && bad.errors[2].find("'A' is not a factor doctor had for emergency") != std::string::npos,
              bad.errors.size() > 2 ? bad.errors[2] : "");
    s.require("a held-back case's ratio is refused: it is not to be chosen", bad.errors.size() > 3 && bad.errors[3].find("held back") != std::string::npos);
  }

  // ---- Assessment, by hand ----
  {
    const auto a = doctrine::assess(p, make(p, 1, {"L"}, {"N"}, {"C", "L"}, {"N"}));
    s.check("narrow: the misfit is the persuasive tandem's 0.3", a.misfit - 0.3, 1e-12);
    s.require("narrow: the car and the binding cyclist agree, the persuasive tandem does not, the held-back pram is not read", a.cases.size() == 3 && a.cases[0].agrees && a.cases[1].agrees && !a.cases[2].agrees);
    s.check("narrow: justification (0.4 + 1) / (1 + 1) = 0.7", a.justification - 0.7, 1e-12);
    s.require("narrow, no ratio forcing: no conflict, the emergency open, the ban does not follow", a.conflicts.empty() && a.forced[0].empty() && a.outcomes[0] == Outcome::fails);
    s.check("its score 0.3 − 0.7", a.score - (-0.4), 1e-12);
    const auto c = doctrine::assess(p, make(p, 0, {"R"}, {"A"}, {"C", "L"}, {"N"}));
    s.require("broad with doctor read as {R} and commuter as {A}: the two conflict, named", c.conflicts.size() == 1 && c.conflicts[0] == "emergency: doctor and commuter", c.conflicts.empty() ? "" : c.conflicts[0]);
    s.require("and doctor forces an emergency: the ban does not follow", c.forced[0] == "emergency" && c.outcomes[0] == Outcome::fails);
    s.check("its score 1 + 2 × 1 − 0.5", c.score - 2.5, 1e-12);
    const auto h = doctrine::assess(p, make(p, 0, {"L"}, {"N"}, {"C", "L"}, {"A"}));
    s.require("broad with cyclist read as {A}: no emergency is forced, and the ban follows", h.forced[0] == "no_emergency" && h.outcomes[0] == Outcome::holds && h.conflicts.empty());
    const auto t = doctrine::assess(p, make(p, 0, {"L"}, {"N"}, {"C", "L"}, {"N"}));
    s.require("broad with nothing forced: the ban turns on the precedents", t.outcomes[0] == Outcome::turns && t.forced[0].empty());
  }

  // ---- Every doctrine, against the hand ----
  {
    std::size_t agree = 0, n = 0;
    std::string wrong;
    for (std::size_t reading : {0u, 1u})
      for (const auto& d1 : subsets({"L", "R"}))
        for (const auto& d2 : subsets({"A", "N"}))
          for (const auto& d3 : subsets({"C", "L"}))
            for (const auto& d4 : subsets({"A", "N"})) {
              const auto a = doctrine::assess(p, make(p, reading, d1, d2, d3, d4));
              const auto h = by_hand(reading, d1, d2, d3, d4);
              const auto k = doctrine::kind(p, a);
              n++;
              if (std::fabs(a.score - h.score) < 1e-12 && static_cast<int>(k[0]) == h.prohibited && static_cast<int>(k[1]) == h.emergency) agree++;
              else if (wrong.size() < 900)
                wrong += "[" + oneline(doctrine::write(p, make(p, reading, d1, d2, d3, d4))) + " engine " + std::to_string(a.score) + " " + std::to_string(k[0]) + " " + std::to_string(k[1]) + " forced '" + a.forced[0] +
                         "' conflicts " + std::to_string(a.conflicts.size()) + "; hand " + std::to_string(h.score) + " " + std::to_string(h.prohibited) + " " + std::to_string(h.emergency) + "] ";
            }
    s.require("all 162 doctrines assessed as the hand says: score, the ban, the emergency", agree == 162 && n == 162, std::to_string(agree) + " " + wrong);
    const auto best = doctrine::best_of_each_kind(p);
    std::map<std::vector<double>, double> want;
    for (std::size_t reading : {0u, 1u})
      for (const auto& d1 : subsets({"L", "R"}))
        for (const auto& d2 : subsets({"A", "N"}))
          for (const auto& d3 : subsets({"C", "L"}))
            for (const auto& d4 : subsets({"A", "N"})) {
              const auto h = by_hand(reading, d1, d2, d3, d4);
              const std::vector<double> k{static_cast<double>(h.prohibited), static_cast<double>(h.emergency)};
              if (!want.count(k) || h.score < want[k]) want[k] = h.score;
            }
    bool same = best.size() == want.size();
    for (const auto& b : best) same = same && want.count(b.kind) && std::fabs(want[b.kind] - b.assessment.score) < 1e-12;
    s.require("exhaustion finds the best doctrine of each of the kinds there are, as the hand does", same, std::to_string(best.size()) + " kinds against " + std::to_string(want.size()));
    s.require("the best of all reads 'vehicle' narrowly (score −0.4)", !best.empty() && best[0].doctrine.readings[0] == 1 && std::fabs(best[0].assessment.score + 0.4) < 1e-12);
    std::vector<doctrine::Assessment> as;
    for (const auto& b : best) as.push_back(b.assessment);
    const auto st = doctrine::standings(p, as, 0.5);
    s.require("within 0.5 of the best fit, every doctrine reads narrowly: the ban is untenable", st[0].verdict == apodeixis::interpretation::Class::untenable, st[0].reason);
    const auto st2 = doctrine::standings(p, as, 1.0);
    s.require("within 1.0, the best broad doctrines join, the ban following under one: arguable", st2[0].verdict == apodeixis::interpretation::Class::arguable && st2[0].holding >= 1, st2[0].reason);
    // The emergency over the same doctrines, counted from their kinds (0 held, 1 denied, 2 open).
    const auto fs = doctrine::fact_standings(p, as, 0.5);
    std::size_t acc = 0, held = 0, denied = 0, open = 0;
    for (const auto& b : best)
      if (b.assessment.score <= best[0].assessment.score + 0.5 + 1e-12) {
        acc++;
        (b.kind[1] == 0 ? held : b.kind[1] == 1 ? denied : open)++;
      }
    const bool one = held == acc || denied == acc || open == acc;
    s.require("the emergency over the acceptable doctrines: held, denied and left open as their kinds say, turning on the ratios when they differ",
              fs.size() == 1 && fs[0].fact == "emergency" && fs[0].acceptable == acc && fs[0].held == held && fs[0].denied == denied && fs[0].open == open
                  && (one ? fs[0].verdict != "turns on the ratios" : fs[0].verdict == "turns on the ratios"),
              fs.empty() ? "none" : fs[0].verdict + ": " + fs[0].reason);
    const auto single = doctrine::fact_standings(p, {as[0]}, 0.5);
    s.require("over one doctrine the emergency is what its precedents force, never turning",
              single[0].acceptable == 1 && single[0].verdict == (as[0].forced[0].empty() ? "open" : as[0].forced[0] == "emergency" ? "held" : "denied"), single[0].verdict);
  }

  // ---- Authority ----
  {
    Problem q = p;
    q.admissible = {1, 0};  // a binding holding fixes the broad reading
    const auto a = doctrine::assess(q, make(q, 1, {"L"}, {"N"}, {"C", "L"}, {"N"}));
    s.require("with the broad reading fixed by authority, a narrow doctrine is against it, and pays", !a.admissible && std::fabs(a.score - (10 - 0.4)) < 1e-12);
    const auto best = doctrine::best_of_each_kind(q);
    s.require("and the best doctrine of all now reads broadly", best[0].doctrine.readings[0] == 0);
  }

  // ---- The search, held to exhaustion ----
  const auto exhaustive = doctrine::best_of_each_kind(p);
  doctrine::SearchOptions so;
  so.strategy.generations = 25;
  doctrine::Search search(p, so);
  search.run(25);
  {
    std::map<std::vector<double>, double> found;
    for (const auto& [k, a] : search.kept()) {
      const auto kd = k->member.evaluation.descriptors;
      if (!found.count(kd) || a.score < found[kd]) found[kd] = a.score;
    }
    std::size_t matched = 0;
    for (const auto& b : exhaustive)
      if (found.count(b.kind) && std::fabs(found[b.kind] - b.assessment.score) < 1e-12) matched++;
    s.require("the search keeps the exhaustive best doctrine of every kind", matched == exhaustive.size(), std::to_string(matched) + " of " + std::to_string(exhaustive.size()));
    const auto st = search.standing();
    s.require("and stands the ban as exhaustion does: untenable within 0.5", st[0].verdict == apodeixis::interpretation::Class::untenable);
  }

  // ---- The test on what was held back ----
  {
    const auto& conf = search.confirmer();
    bool narrow_upheld = false, broad_contradicted = false, every_kept = true;
    for (const auto& [k, a] : search.kept()) every_kept = every_kept && conf.tested(doctrine::key(p, k->member.genotype));
    for (const auto& c : conf.done) {
      const bool narrow = c.text.find("vehicle = narrow") != std::string::npos;
      bool d4 = false;
      for (const auto& x : c.cases) d4 = d4 || (x.id == "D4" && x.verdict == (narrow ? "predicted" : "contradicted"));
      if (narrow && d4 && c.outcome == "upheld") narrow_upheld = true;
      if (!narrow && d4 && c.outcome == "contradicted") broad_contradicted = true;
    }
    s.require("every doctrine kept was tested once", every_kept && !conf.done.empty());
    s.require("the pram, decided later for the narrow reading, upholds a narrow doctrine and contradicts a broad one", narrow_upheld && broad_contradicted);
    const auto v = archeion::fixity::verify(conf.journal);
    bool order = conf.journal.entries.size() == 2 * conf.done.size();
    for (std::size_t k = 0; k + 1 < conf.journal.entries.size(); k += 2) order = order && conf.journal.entries[k].what == "registered a test" && conf.journal.entries[k + 1].what == "wrote a test's outcome";
    s.require("each test registered in the journal before its outcome, the journal verifying", v.holds() && order);
    const auto pj = doctrine::predict(p, make(p, 1, {"L"}, {"A"}, {"C", "L"}, {"N"}));
    bool jog = false;
    for (const auto& x : pj) jog = jog || (x.id == "emergency/jogger" && x.verdict == "predicted");
    s.require("the jogger, decided later against an emergency, is predicted when commuter's ratio is {A}", jog);
    doctrine::Confirmer again = conf;
    const auto& first = search.evolution().archive().front().member;
    s.require("a doctrine is tested once only: a second test of one kept is refused", !again.confirm(p, first.ticket, first.genotype, "", "").has_value() && again.done.size() == conf.done.size());
  }

  // ---- Nothing held back reaches the search ----
  {
    Problem q = p;
    q.decided[3].held = !q.decided[3].held;
    q.bases[0].base.cases[4].outcome = "emergency";
    q.bases[0].base.cases[4].facts = {"R", "W", "A", "L"};
    doctrine::Search a(p, so), b(q, so);
    bool same = true;
    for (int g = 0; g < 12; g++) {
      a.step(), b.step();
      std::vector<std::string> ka, kb;
      for (const auto& k : a.evolution().archive()) ka.push_back(doctrine::key(p, k.member.genotype));
      for (const auto& k : b.evolution().archive()) kb.push_back(doctrine::key(q, k.member.genotype));
      same = same && ka == kb;
    }
    s.require("with the held-back decisions changed, the archive is the same in every generation", same);
  }

  // ---- A model's part ----
  {
    doctrine::Search m(p, so);
    m.run(4);
    const auto q = m.requests(8);
    std::size_t asked = 0;
    bool roles = true, no_held_back = true;
    for (const auto& r : q) {
      asked += r.count;
      roles = roles && (r.role == "seed" ? r.parents.empty() : r.role == "cross" ? r.parents.size() == 2 : r.parents.size() == 1);
      for (const auto& t : r.texts) no_held_back = no_held_back && t.find("jogger") == std::string::npos;
      for (const auto& t : r.notes) no_held_back = no_held_back && t.find("D4") == std::string::npos;
    }
    s.require("a budget of eight shared among the roles, each with its parents", asked == 8 && roles);
    s.require("nothing held back in what a model is shown", no_held_back);
    const doctrine::Request* mut = nullptr;
    for (const auto& r : q)
      if (r.role == "mutate") mut = &r;
    if (mut) {
      // A doctrine not living in the search, written as a model would write it. In a space of 162 the search's own operators may
      // make the same doctrine in the same generation, and a repeat is refused as one; so a few fresh doctrines are offered in turn.
      const auto parents = mut->parents;
      bool lineage = false, read_ok = true;
      for (int attempt = 0; attempt < 5 && !lineage; attempt++) {
        std::set<std::string> living;
        for (const auto& x : m.evolution().population()) living.insert(doctrine::key(p, x.genotype));
        for (const auto& x : m.evolution().archive()) living.insert(doctrine::key(p, x.member.genotype));
        std::string fresh;
        int skip = attempt;
        doctrine::every(p, [&](const Doctrine& d) {
          if (fresh.empty() && !living.count(doctrine::key(p, d)) && skip-- <= 0) fresh = doctrine::write(p, d);
        });
        const auto proposed = doctrine::read(p, fresh);
        read_ok = read_ok && proposed.ok;
        m.propose({proposed.doctrine}, "model:mutate", {parents});
        m.step();
        for (const auto& l : m.evolution().lineage())
          if (l.operation == "model:mutate" && l.parents == parents) lineage = true;
      }
      s.require("a model's doctrine enters with the doctrine it came from as its parent", read_ok && lineage);
    } else {
      s.require("a model's doctrine enters with its parent (no mutate request in the budget)", true);
    }
  }

  // ---- Refusals ----
  {
    Problem q = p;
    q.decided[0].court = "nowhere";
    q.bases[0].situation.insert("Z");
    q.purposes[0].worth["vehicle"] = {1.0};
    const auto f = doctrine::faults(q);
    s.require("an undeclared court, an undeclared factor and a purpose's wrong count of worths are each named", f.size() == 3, std::to_string(f.size()));
    bool refused = false;
    try {
      doctrine::Search x(q, so);
    } catch (const std::invalid_argument&) {
      refused = true;
    }
    s.require("and a search on a faulty problem is refused", refused);
    s.check("the distance between a doctrine and itself is 0", doctrine::distance(p, exhaustive[0].doctrine, exhaustive[0].doctrine), 0);
  }
  return s.done();
}
