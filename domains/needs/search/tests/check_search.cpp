// Checks of needs/search: the generators, the operators, and the search, on synthetic populations whose needs are planted.
//
// - Generators: enumeration walks the language breadth first and depth-first in preorder; the beam's best group and the tree's
//   worst leaf are the planted group.
// - Operators: every mutation and crossover gives a statement within the language or nothing; the distance is a semimetric in
//   [0, 1], zero exactly between statements of one group, indicator and standard.
// - The search finds the planted need, admits it, confirms it, and stands it robust; on data with no need planted it confirms
//   nothing; every survival, parent and schedule strategy runs it.
// - Nothing of the confirmation part reaches the search: with the confirmation part's outcomes permuted, the population, the
//   archive and the admissions are the same, generation by generation.
// - Resumed from what it saved, a search goes on as one that never stopped: the same archive, confirmations and journal.
// - The journal of confirmations verifies, each test registered before its outcome, and an altered entry is found.
// - Proposals from outside enter the population and are credited to their source; refusals of unknown objectives and operators.

#include <needs/search/search.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <set>
#include <string>
#include <vector>

using namespace needs;
using namespace needs::search;
using language::Indicator;

namespace {

// 300 areas; in each, people by age (40, 70, 85), tenure (owner, renting, social) and sex: whether they are without the help they
// need (a share), and a burden (a value). Planted: those 75 and over who rent are without help 12 points more often; renting women
// carry 0.08 more burden.
language::Dataset population(bool planted, std::uint64_t seed = 5, bool places = false) {
  calx::random::Stream r(seed, 2, 0, 0);
  std::vector<std::string> area, ten, sex;
  std::vector<double> age, k, n, people, burden, income;
  for (int c = 0; c < 300; c++) {
    const double a0 = 0.02 * r.normal(), b0 = 0.03 * r.normal();
    for (int a : {40, 70, 85})
      for (const char* t : {"owner", "renting", "social"})
        for (const char* x : {"f", "m"}) {
          const double nn = 15 + static_cast<double>(r.below(35));
          const bool old_renter = a >= 75 && std::string(t) == "renting", renting_woman = std::string(t) == "renting" && std::string(x) == "f";
          const double p = std::clamp(0.10 + a0 + (planted && old_renter ? 0.12 : 0.0), 0.01, 0.9);
          area.push_back("a" + std::to_string(c)), ten.push_back(t), sex.push_back(x), age.push_back(a), n.push_back(nn), people.push_back(nn);
          k.push_back(static_cast<double>(r.binomial(static_cast<std::int64_t>(nn), p)));
          burden.push_back(0.25 + b0 + (planted && renting_woman ? 0.08 : 0.0) + 0.05 * r.normal());
          income.push_back(30000 + 8000 * r.normal());
        }
  }
  language::Dataset d;
  d.table.add(groups::categories("area", area));
  d.table.add(groups::categories("tenure", ten));
  d.table.add(groups::categories("sex", sex));
  d.table.add(groups::numbers("age", age));
  d.table.add(groups::numbers("k", k));
  d.table.add(groups::numbers("n", n));
  d.table.add(groups::numbers("people", people));
  d.table.add(groups::numbers("burden", burden));
  d.table.add(groups::numbers("income", income));
  d.cluster = "area";
  d.attributes = {"age", "tenure", "sex", "income"};
  d.indicators["unassisted"] = Indicator{"unassisted", true, "k", "n", true, "without help", "", "care", {0.15}};
  d.indicators["burden"] = Indicator{"burden", false, "burden", "people", true, "rent burden", "", "housing", {0.3}};
  d.domains = {"care", "housing"};
  if (places) {
    d.place = "area", d.reach = 1500;
    for (int c = 0; c < 300; c++) d.where["a" + std::to_string(c)] = {(c % 20) * 500.0, (c / 20) * 500.0};
    d.suppliers = {{"s1", 1000, 1000, 400, "care"}, {"s2", 8000, 6000, 300, "care"}};
  }
  return d;
}

Statement read(const language::Dataset& d, const std::string& text) {
  auto s = language::read_one(text);
  language::bind(d, s);
  language::check(d, s);
  return s;
}

Options quick(std::size_t generations = 12) {
  Options o;
  o.strategy.mu = 40, o.strategy.lambda = 40, o.strategy.generations = generations;
  o.stability_resamples = 60;
  return o;
}

std::vector<std::string> keys_of(const std::vector<heuresis::evolution::Member<Statement>>& v) {
  std::vector<std::string> out;
  for (const auto& m : v) out.push_back(m.genotype.key());
  return out;
}
std::vector<std::string> keys_of(const std::vector<heuresis::evolution::Kept<Statement>>& v) {
  std::vector<std::string> out;
  for (const auto& m : v) out.push_back(m.member.genotype.key());
  return out;
}

}  // namespace

int main() {
  calx::verification::Suite s;
  const auto d = population(true);
  const auto parts = evidence::split(d);
  const auto sp = space(d, parts);
  const auto planted = read(d, "need care for age >= 75 and tenure = renting by comparative unassisted higher");

  // ---- generators ----
  {
    Generators g;
    g.d = &d, g.parts = &parts, g.sp = &sp;
    calx::random::Stream r(1, 1, 0, 0);
    const auto first = g.make("enumeration", sp.conditions.size() * sp.indicators.size(), r);
    bool singles = true;
    for (const auto& st : first) singles = singles && st.group.conditions.size() == 1;
    const auto next = g.make("enumeration", 10, r);
    s.require("enumeration: every single condition with every indicator first", singles && first.size() == sp.conditions.size() * sp.indicators.size());
    s.require("enumeration: then pairs", !next.empty() && next[0].group.conditions.size() == 2);
    Generators h;
    h.d = &d, h.parts = &parts, h.sp = &sp;
    const auto df = h.make("depth-first", 3 * sp.indicators.size(), r);
    s.require("depth-first: a group, then its refinement, then its refinement's", df[0].group.size() == 1 && df[sp.indicators.size()].group.size() == 2 &&
                                                                                     df[2 * sp.indicators.size()].group.size() == 3);
    const Lens L = lens(d, parts, plain(sp, d, {}, "unassisted"));
    const auto b = beam(sp, L, 1.0, 20, 3);
    s.require("beam: its best group holds the planted group's people exactly", !b.empty() && groups::rows_of(d.table, b[0].group) == groups::rows_of(d.table, planted.group), b.empty() ? "" : b[0].group.write());
    const auto t = tree(sp, L, 3);
    bool has = !t.empty();
    for (const auto& c : planted.group.conditions) {
      bool found = false;
      for (const auto& x : t[0].group.conditions) found = found || (x.attribute == c.attribute && groups::rows_of(d.table, Group{{x}}) == groups::rows_of(d.table, Group{{c}}));
      has = has && found;
    }
    s.require("tree: its worst leaf is stated by the planted conditions", has, t.empty() ? "" : t[0].group.write());
  }
  // ---- operators ----
  {
    Variation v;
    v.d = &d, v.parts = &parts, v.sp = &sp;
    calx::random::Stream r(3, 3, 0, 0);
    bool valid = true;
    std::size_t made = 0;
    std::string bad;
    const Statement other = read(d, "need housing for sex = f and tenure = renting by comparative burden higher");
    for (int k = 0; k < 400; k++) {
      std::vector<std::optional<Statement>> out{v.add(planted, r), v.drop(planted, r), v.shift(planted, r), v.flip(planted, r), v.widen(planted, r), v.narrow(planted, r),
                                               v.swap(planted, r), v.indicator(planted, r), v.standard(planted, r), v.frame(planted, r), v.join(planted, other),
                                               v.meet(planted, other), v.one_point(planted, other, r), v.uniform(planted, other, r), v.graft(planted, other)};
      for (const auto& o : out) {
        if (!o) continue;
        made++;
        try {
          language::check(d, *o);
          if (!v.fits(*o)) valid = false, bad = o->write();
        } catch (const std::exception& e) {
          valid = false, bad = o->write() + ": " + e.what();
        }
      }
    }
    s.require("operators: every statement made is within the language and judged by the data", valid && made > 1000, bad);
    const double dd = v.distance(planted, other), dz = v.distance(planted, planted), ds = v.distance(other, planted);
    s.require("distance: in [0, 1], zero to itself, symmetric", dd > 0 && dd <= 1 && dz == 0 && dd == ds);
    Statement same = planted;
    same.group = groups::Group{{planted.group.conditions[1], planted.group.conditions[0]}};
    s.require("distance: zero between one group written two ways", v.distance(planted, same) == 0);
  }
  // ---- the search finds and confirms the planted need ----
  Saved mid, whole;
  {
    Search search(d, quick(16));
    search.run(16);
    const auto& adm = search.admissions();
    const auto& conf = search.confirmations();
    const auto rp = groups::rows_of(d.table, planted.group);
    bool found = false, robust = false;
    std::string best;
    for (const auto& c : conf) {
      auto st = language::read_one(c.text);
      language::bind(d, st);
      const double j = groups::jaccard_distance(d.table, groups::rows_of(d.table, st.group), rp);
      if (j < 0.34 && st.indicator == "unassisted" && c.confirmed) {
        found = true;
        robust = robust || search.evidence_of(st).standing.standing == evidence::Standing::robust;
        best = c.text;
      }
    }
    s.require("search: statements admitted, and each confirmed once", !adm.empty() && conf.size() == adm.size(), std::to_string(adm.size()) + " admitted");
    {
      Variation v;
      v.d = &d, v.parts = &parts, v.sp = &search.space();
      const auto& kept = search.evolution().archive();
      std::size_t close = 0;
      for (std::size_t i = 0; i < kept.size(); i++)
        for (std::size_t j = i + 1; j < kept.size(); j++)
          if (v.distance(kept[i].member.genotype, kept[j].member.genotype) < search.options().min_distance &&
              std::min(kept[i].admitted, kept[j].admitted) + 1 < search.evolution().generation())
            close++;
      std::size_t superseded = 0;
      for (const auto& r : search.evolution().retired()) superseded += r.reason.find("superseded") == 0;
      s.require("search: the archive keeps distinct needs (no two within the least distance once a generation has passed)", close == 0 && kept.size() < 40,
                std::to_string(kept.size()) + " kept, " + std::to_string(superseded) + " superseded, " + std::to_string(close) + " close pairs");
    }
    s.require("search: the planted need (or one within a third of its people) is confirmed", found, best);
    s.require("search: and stands robust", robust);
    bool housing = false;
    for (const auto& c : conf) housing = housing || (c.confirmed && c.text.find("burden") != std::string::npos && c.text.find("tenure = renting") != std::string::npos);
    s.require("search: the planted burden of renters is confirmed too", housing);
    const auto v = archeion::fixity::verify(search.journal());
    s.require("journal: it verifies", v.holds(), v.reason);
    bool order = search.journal().entries.size() == 2 * conf.size();
    for (std::size_t k = 0; k + 1 < search.journal().entries.size(); k += 2)
      order = order && search.journal().entries[k].what == "register" && search.journal().entries[k + 1].what == "confirm" && search.journal().entries[k].about == search.journal().entries[k + 1].about;
    s.require("journal: each test registered before its outcome", order);
    auto altered = search.journal();
    altered.entries[0].note += " (changed)";
    s.require("journal: an altered registration is found", !archeion::fixity::verify(altered).holds());
    s.require("holdout: consulted, within its budget", search.holdout().queries() > 0 && search.holdout().overfits() <= search.options().holdout.budget);
    whole = search.saved();
  }
  // ---- resumption ----
  {
    Search first(d, quick(16));
    first.run(8);
    mid = first.saved();
    Search second(d, quick(16), mid);
    second.run(8);
    const auto again = second.saved();
    std::vector<std::string> a, b;
    for (const auto& c : again.confirmations) a.push_back(c.key + (c.confirmed ? "+" : "-"));
    for (const auto& c : whole.confirmations) b.push_back(c.key + (c.confirmed ? "+" : "-"));
    s.require("resumption: the same confirmations, in the same order", a == b && !a.empty(), std::to_string(a.size()) + " against " + std::to_string(b.size()));
    s.require("resumption: the same journal", again.journal.head() == whole.journal.head());
    s.require("resumption: the same archive", keys_of(again.evolution.archive) == keys_of(whole.evolution.archive));
  }
  // ---- nothing of the confirmation reaches the search ----
  {
    auto e = d;
    const auto ep = evidence::split(e);
    calx::random::Stream r(77, 1, 0, 0);
    std::vector<std::size_t> rows;
    for (std::size_t i = 0; i < e.table.rows; i++)
      if (ep.part_of_row[i] == 2) rows.push_back(i);
    auto& kc = e.table.columns[4].numbers;  // k
    auto& nc = e.table.columns[5].numbers;  // n
    for (std::size_t j = rows.size(); j > 1; j--) {
      const std::size_t q = r.below(j);
      std::swap(kc[rows[j - 1]], kc[rows[q]]), std::swap(nc[rows[j - 1]], nc[rows[q]]);
    }
    Search a(d, quick(10)), b(e, quick(10));
    bool same = true;
    for (int g = 0; g < 10; g++) {
      a.step(), b.step();
      same = same && keys_of(a.evolution().population()) == keys_of(b.evolution().population()) && keys_of(a.evolution().archive()) == keys_of(b.evolution().archive());
    }
    std::vector<std::string> pa, pb;
    for (const auto& x : a.admissions()) pa.push_back(x.key);
    for (const auto& x : b.admissions()) pb.push_back(x.key);
    bool differ = false;
    for (std::size_t k = 0; k < std::min(a.confirmations().size(), b.confirmations().size()); k++) differ = differ || a.confirmations()[k].p != b.confirmations()[k].p;
    s.require("confirmation apart: with its outcomes permuted, the same population and archive in every generation", same);
    s.require("confirmation apart: the same admissions", pa == pb && !pa.empty());
    s.require("confirmation apart: while the confirmations themselves differ", differ);
  }
  // ---- no need planted: nothing confirmed ----
  {
    const auto null = population(false, 11);
    Search search(null, quick(12));
    search.run(12);
    std::size_t confirmed = 0;
    for (const auto& c : search.confirmations()) confirmed += c.confirmed;
    s.require("null data: no statement is confirmed", confirmed == 0, std::to_string(search.admissions().size()) + " admitted, " + std::to_string(confirmed) + " confirmed");
  }
  // ---- strategies ----
  {
    namespace h = heuresis::evolution;
    std::vector<std::string> failed;
    auto run = [&](const std::string& label, auto set) {
      try {
        Options o = quick(5);
        set(o);
        Search search(d, o);
        search.run(5);
        if (search.evolution().population().empty()) failed.push_back(label + ": empty");
      } catch (const std::exception& e) {
        failed.push_back(label + ": " + e.what());
      }
    };
    for (auto sv : {h::Survival::generational, h::Survival::plus, h::Survival::comma, h::Survival::replace_worst, h::Survival::aging, h::Survival::deterministic_crowding,
                    h::Survival::restricted_tournament, h::Survival::clearing, h::Survival::nsga2, h::Survival::alps})
      run(std::string("survival ") + h::name(sv), [&](Options& o) {
        o.strategy.survival = sv;
        if (sv == h::Survival::nsga2) o.strategy.scoring = h::Scoring::pareto;
      });
    run("survival niches", [&](Options& o) {
      o.strategy.survival = h::Survival::niches, o.strategy.bins = {3, 4, 6, 4}, o.strategy.lower = {0, 0, 1, 1}, o.strategy.upper = {3, 4, 5, 4};
    });
    for (auto pa : {h::Parents::uniform, h::Parents::roulette, h::Parents::ranking, h::Parents::boltzmann, h::Parents::truncation, h::Parents::lexicase, h::Parents::epsilon_lexicase, h::Parents::novelty})
      run(std::string("parents ") + h::name(pa), [&](Options& o) {
        o.strategy.parents = pa;
        if (pa == h::Parents::lexicase || pa == h::Parents::epsilon_lexicase) o.objectives = {"strata"};
      });
    for (auto sc : {h::Schedule::probability_matching, h::Schedule::adaptive_pursuit, h::Schedule::ucb, h::Schedule::thompson})
      run(std::string("schedule ") + h::name(sc), [&](Options& o) { o.strategy.schedule = sc; });
    run("objectives: every one", [&](Options& o) { o.objectives = {"gap", "effect", "coverage", "size", "quality", "simplicity", "stability", "strata"}; });
    run("novelty", [&](Options& o) { o.strategy.novelty = true; });
    run("binomial inference", [&](Options& o) { o.inference = evidence::Inference::binomial; });
    for (auto rule : {tyche::testing::OnlineRule::lond, tyche::testing::OnlineRule::lord_plus, tyche::testing::OnlineRule::saffron, tyche::testing::OnlineRule::alpha_investing})
      run(std::string("online rule ") + tyche::testing::name(rule), [&](Options& o) { o.rule = rule; });
    run("generators only: beam and tree", [&](Options& o) {
      for (const auto& n : generator_names()) o.operators[n] = (n == "beam" || n == "tree") ? 1 : 0;
    });
    s.require("strategies: every survival, parent, schedule, objective and online rule runs the search", failed.empty(), failed.empty() ? "" : failed[0]);
  }
  // ---- simulation and rules as generators ----
  {
    auto e = population(true, 5, true);
    e.rules = {{"need care for age >= 85 by comparative unassisted higher", "an illustrative scheme"}};
    Options o = quick(4);
    o.operators = {{"enumeration", 0}, {"depth-first", 0}, {"random", 0}, {"beam", 0}, {"tree", 0}};
    Search search(e, o);
    search.run(4);
    std::set<std::string> ops;
    for (const auto& l : search.evolution().lineage()) ops.insert(l.operation);
    s.require("generators: the seeking model's and the rules' generators make statements", ops.count("simulation") && ops.count("rules"));
    s.require("simulation: the seeking model ran for care", search.fields().count("unassisted") == 1);
  }
  // ---- proposals and refusals ----
  {
    Search search(d, quick(3));
    search.propose({planted}, "a person");
    search.run(3);
    const auto& st = search.evolution().statistics();
    std::size_t told = 0;
    for (const auto& x : st) told += x.proposals.count("a person") ? x.proposals.at("a person") : 0;
    s.require("proposals: entered and credited to their source", told == 1);
    auto refuses = [&](const std::string& what, auto f) {
      try {
        f();
        s.require("refusal: " + what, false);
      } catch (const std::exception& e) {
        s.require("refusal: " + what, true, e.what());
      }
    };
    refuses("an unknown operator", [&] {
      Options o = quick(1);
      o.operators["mutate-wildly"] = 1;
      Search x(d, o);
    });
    refuses("no generator", [&] {
      Options o = quick(1);
      for (const auto& n : generator_names()) o.operators[n] = 0;
      Search x(d, o);
    });
    refuses("an unknown objective", [&] {
      Options o = quick(1);
      o.objectives = {"beauty"};
      Search x(d, o);
    });
    refuses("an unknown role for a model", [&] { (void)search.requests(4, {"dream"}); });
  }
  // ---- what is asked of a model, and its proposals' lineage (ADR 0126) ----
  {
    Search search(d, quick(6));  // room for a generation after the four run
    search.run(4);
    const auto q = search.requests(9);
    std::size_t asked = 0;
    bool parents_known = true, notes = true, roles = true;
    std::set<heuresis::search::Ticket> living;
    for (const auto& m : search.evolution().population()) living.insert(m.ticket);
    for (const auto& k : search.evolution().archive()) living.insert(k.member.ticket);
    for (const auto& r : q) {
      asked += r.count;
      roles = roles && (r.role == "seed" ? r.parents.empty() : r.role == "cross" ? r.parents.size() == 2 : r.parents.size() == 1);
      for (auto t : r.parents) parents_known = parents_known && living.count(t);
      for (const auto& n : r.notes) notes = notes && n.rfind("on discovery", 0) == 0;
    }
    s.require("requests: a budget of nine shared among the roles, each with its parents", asked == 9 && roles, std::to_string(asked));
    s.require("requests: every parent is a living member or a kept statement, described by discovery alone", parents_known && notes);
    bool even = true;
    for (const auto& r : q) even = even && std::fabs(r.credit - 0.5) < 1e-15;
    s.require("requests: before any proposal every role has credit 1/2", even);
    const auto* mut = &q[0];
    for (const auto& r : q)
      if (r.role == "mutate") mut = &r;
    const auto odd = read(d, "need care for income >= 41234 and tenure = renting by comparative unassisted higher");
    search.propose({odd}, "model:mutate", {mut->parents});
    search.step();
    bool lineage = false;
    for (const auto& l : search.evolution().lineage())
      if (l.operation == "model:mutate") lineage = l.parents == mut->parents;
    s.require("proposals: a model's variation keeps the member it started from as its parent", lineage);
    const auto again = search.requests(9, {"mutate"});
    s.require("requests: the role's credit follows its proposals' fate", !again.empty() && std::fabs(again[0].credit - 0.5) > 1e-12, std::to_string(again.empty() ? 0.0 : again[0].credit));
  }
  // ---- interpretation and rivals at admission (ADR 0126) ----
  {
    auto e = d;
    e.points = {{"reference", "whom the group is compared with", language::PointKind::reference, {}, {{"the rest", "analyst", 'A', "", ""}, {"everyone", "analyst", 'A', "", ""}, {"age < 75", "analyst", 'A', "", ""}}}};
    e.rivals = {{"sex", "", "analyst", 'A', "", evidence::strata_of(e, "sex"), {}}};
    Options o = quick(8);
    o.admit_clear_only = true;
    Search search(e, o);
    search.run(8);
    bool clear = !search.admissions().empty();
    for (const auto& a : search.admissions()) clear = clear && (a.reading.empty() || a.reading == "clear");
    s.require("admission: with clear readings asked for, every statement kept is clear under its readings", clear, std::to_string(search.admissions().size()) + " admitted");
    bool not_away = true;
    const auto io = search.interpret_options();
    for (const auto& a : search.admissions()) {
      auto st = language::read_one(a.text);
      language::bind(e, st);
      not_away = not_away && evidence::rival(e, st, e.rivals[0], search.parts(), evidence::Part::discovery, io).explains != tekmerion::construal::Explains::away;
    }
    s.require("admission: none is explained away by the declared rival", not_away);
    const auto ev = search.evidence_of(planted);
    s.require("evidence: the planted need read under three references, with the rival assessed", ev.interpretation.applies && ev.interpretation.statements.size() == 3 && ev.rivals.size() == 1);
    // Two measures declared as readings of one term: statements differing only in them are one need.
    auto f = d;
    f.indicators["unassisted_b"] = Indicator{"unassisted_b", true, "k", "n", true, "", "", "care", {}};
    f.points = {{"measure", "", language::PointKind::indicator, {}, {{"unassisted", "analyst", 'A', "", ""}, {"unassisted_b", "analyst", 'A', "", ""}}}};
    Variation v;
    v.d = &f, v.parts = &parts, v.sp = &sp;
    auto twin = planted;
    twin.indicator = "unassisted_b";
    auto other = planted;
    other.indicator = "burden", other.worse_when_higher = true;
    s.require("distance: one need read under two declared measures is one need; another indicator is not", v.distance(planted, twin) == 0 && v.distance(planted, other) > 0.25);
  }
  return s.done();
}
