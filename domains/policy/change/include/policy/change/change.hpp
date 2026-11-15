// Theories of change: how an intervention is meant to bring about what it is for, link by link, and how far each link is shown.
//
// A theory of change (Weiss 1995, Nothing as practical as good theory, in New Approaches to Evaluating Community Initiatives; the
// logic models of evaluation, from inputs and activities through outputs to outcomes and impact) is an acyclic graph whose nodes
// are states of affairs and whose arcs are causal claims. Each claim rests on an assumption, needs its support factors (what else
// must be present for the cause to work there; Cartwright and Hardie 2012, Evidence-Based Policy, ch. 3), and may have evidence: an
// effect with its interval, graded by a declared scheme (policy::evidence makes it from studies). What is computed:
//
//  - the order of the nodes, and the refusal of a cycle with the cycle named (morphe's acyclic relations);
//  - a link's standing: shown at its grade when its interval lies on the side the theory expects; not shown when it has no
//    evidence, or its interval reaches zero, or lies on the other side;
//  - for a path from the intervention to an outcome, its weakest link: a chain of causes is no better shown than its least shown
//    link, whatever the others (the theory note, Proposition 1);
//  - for the outcome, the strongest path: the greatest over paths of the weakest link's standing, found exactly by dynamic
//    programming along the order (the bottleneck path; Proposition 2), with the path, the link that limits it and the support
//    factors it needs;
//  - the links on which every path to the outcome depends, and those not shown among them: where evidence would most help;
//  - where every link on the paths states a coefficient of a linear effect, the effect implied on the outcome by Wright's rule, the
//    sum over paths of the products of the coefficients (Wright 1934, The method of path coefficients, Annals of Mathematical
//    Statistics 5), exact for linear links without confounding between them; with the links' intervals, an enclosure by interval
//    arithmetic, which is exact only when no link lies on two paths (Proposition 3).
//
// The written form, a line each (names are words without spaces; kinds are the theory's own words):
//
//   theory free_school_meals
//   levels very_low low moderate high               the grades of the scheme the evidence was graded by, lowest first
//   node meals intervention: free meals for every pupil
//   node attendance outcome: pupils attend more days
//   link meals -> attendance: families send children who would otherwise stay home
//   support meals -> attendance: the meal is served before the first lesson
//   evidence meals -> attendance: 0.12 [0.05, 0.19] moderate      an effect, its interval and its grade
//   sign meals -> attendance: -                     the side the theory expects (+ by default)

#pragma once

#include <apodeixis/kernel/verdict.hpp>

#include <morphe/combinatorics/acyclic.hpp>

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <limits>
#include <map>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace policy::change {

using apodeixis::kernel::Verdict;

struct Node {
  std::string id, kind, text;
};

struct Evidence {
  bool given = false;
  double effect = std::numeric_limits<double>::quiet_NaN();
  double lower = std::numeric_limits<double>::quiet_NaN(), upper = std::numeric_limits<double>::quiet_NaN();
  std::string grade;  // a level of the theory's scale
};

struct Link {
  std::string from, to, assumption;
  std::vector<std::string> support;  // the support factors it needs
  int sign = +1;                     // the side the theory expects: +1 an increase, −1 a decrease
  Evidence evidence;
};

struct Theory {
  std::string name;
  std::vector<std::string> levels;  // the grades, lowest first
  std::vector<Node> nodes;
  std::vector<Link> links;
};

// ---- Standing of a link ----------------------------------------------------------------------------------------------------------

inline constexpr long not_shown = -1;

struct Standing {
  long rank = not_shown;  // the grade's index when shown; not_shown otherwise
  std::string why;
};

inline std::size_t node_index(const Theory& t, const std::string& id) {
  for (std::size_t i = 0; i < t.nodes.size(); i++)
    if (t.nodes[i].id == id) return i;
  throw std::invalid_argument("policy::change: the theory has no node " + id);
}

inline std::string number(double x) {
  char b[40];
  std::snprintf(b, sizeof b, "%.4g", x);
  return b;
}

inline Standing standing(const Theory& t, const Link& l) {
  Standing s;
  const std::string name = l.from + " -> " + l.to;
  if (!l.evidence.given) return {not_shown, name + " has no evidence"};
  const auto& e = l.evidence;
  const auto g = std::find(t.levels.begin(), t.levels.end(), e.grade);
  if (g == t.levels.end()) throw std::invalid_argument("policy::change: the grade " + e.grade + " of " + name + " is not among the theory's levels");
  if (!(e.lower <= e.effect && e.effect <= e.upper)) throw std::invalid_argument("policy::change: the evidence of " + name + " has an effect outside its interval");
  const std::string interval = number(e.effect) + " [" + number(e.lower) + ", " + number(e.upper) + "]";
  const bool on_side = l.sign > 0 ? e.lower > 0 : e.upper < 0;
  const bool against = l.sign > 0 ? e.upper < 0 : e.lower > 0;
  if (on_side) return {static_cast<long>(g - t.levels.begin()), name + " is shown at " + e.grade + ": " + interval};
  if (against) return {not_shown, name + " is shown against the theory: " + interval + " lies on the other side"};
  return {not_shown, name + " is not shown: " + interval + " reaches no effect"};
}

// ---- Analysis ------------------------------------------------------------------------------------------------------------------

struct Path {
  std::vector<std::size_t> links;  // in order from the intervention
  long weakest = not_shown;
  std::size_t limiting = 0;        // the index (into links) of its weakest link
  std::vector<std::string> support;
};

struct Assessment {
  std::size_t from = 0, to = 0;
  std::vector<std::size_t> order;          // the nodes in an order of the theory
  std::size_t paths = 0;                   // the paths from the intervention to the outcome
  Path strongest;                          // the path of greatest weakest link (the first found among equals)
  std::vector<std::size_t> necessary;      // the links every path passes through
  std::vector<std::size_t> necessary_gaps; // those of them not shown
  bool linear = false;                     // every link on a path states an effect
  double implied = std::numeric_limits<double>::quiet_NaN(), implied_lower = std::numeric_limits<double>::quiet_NaN(),
         implied_upper = std::numeric_limits<double>::quiet_NaN();
  bool shared = false;                     // some link lies on two paths: the enclosure is not exact
  Verdict verdict;
};

namespace detail {

inline std::pair<double, double> times(std::pair<double, double> a, std::pair<double, double> b) {
  const double p[4] = {a.first * b.first, a.first * b.second, a.second * b.first, a.second * b.second};
  return {*std::min_element(p, p + 4), *std::max_element(p, p + 4)};
}

}  // namespace detail

// The order of the theory's nodes, or std::invalid_argument naming the cycle.
inline std::vector<std::size_t> order(const Theory& t, std::vector<morphe::combinatorics::Arc>* arcs_out = nullptr) {
  std::vector<morphe::combinatorics::Arc> arcs;
  for (const auto& l : t.links) arcs.push_back({node_index(t, l.from), node_index(t, l.to)});
  const auto o = morphe::combinatorics::topological_order(t.nodes.size(), arcs);
  if (!o.acyclic) {
    std::string cycle;
    for (std::size_t v : o.cycle) cycle += (cycle.empty() ? "" : " -> ") + t.nodes[v].id;
    throw std::invalid_argument("policy::change: the theory has a cycle, " + cycle + " -> " + t.nodes[o.cycle.front()].id);
  }
  if (arcs_out) *arcs_out = arcs;
  return o.order;
}

// How far the outcome `to` is shown to follow from the intervention `from`, against the least grade `least` asked.
inline Assessment assess(const Theory& t, const std::string& from, const std::string& to, const std::string& least) {
  Assessment a;
  std::vector<morphe::combinatorics::Arc> arcs;
  a.order = order(t, &arcs);
  a.from = node_index(t, from), a.to = node_index(t, to);
  const auto at_least = std::find(t.levels.begin(), t.levels.end(), least);
  if (at_least == t.levels.end()) throw std::invalid_argument("policy::change: the grade " + least + " asked is not among the theory's levels");
  const long need = static_cast<long>(at_least - t.levels.begin());
  const std::size_t n = t.nodes.size(), L = t.links.size();
  std::vector<Standing> st;
  for (const auto& l : t.links) st.push_back(standing(t, l));
  std::vector<std::vector<std::size_t>> into(n), out(n);
  for (std::size_t e = 0; e < L; e++) into[arcs[e].second].push_back(e), out[arcs[e].first].push_back(e);

  // Paths counted from the intervention, and to the outcome, along the order.
  std::vector<double> ways_from(n, 0), ways_to(n, 0);
  ways_from[a.from] = 1;
  for (std::size_t v : a.order)
    for (std::size_t e : out[v]) ways_from[arcs[e].second] += ways_from[v];
  ways_to[a.to] = 1;
  for (auto it = a.order.rbegin(); it != a.order.rend(); ++it)
    for (std::size_t e : into[*it]) ways_to[arcs[e].first] += ways_to[*it];
  a.paths = static_cast<std::size_t>(ways_from[a.to]);
  const std::string claim = t.nodes[a.to].id + " follows from " + t.nodes[a.from].id;
  if (a.paths == 0) {
    a.verdict = Verdict::no(claim, "no path of the theory leads from " + t.nodes[a.from].id + " to " + t.nodes[a.to].id);
    return a;
  }
  // A link is on some path when it leaves a node reached from the intervention for a node that reaches the outcome; on every path
  // when the paths through it are all of them.
  for (std::size_t e = 0; e < L; e++) {
    const double through = ways_from[arcs[e].first] * ways_to[arcs[e].second];
    if (through == ways_from[a.to]) {
      a.necessary.push_back(e);
      if (st[e].rank == not_shown) a.necessary_gaps.push_back(e);
    }
  }

  // The strongest path: best[v] = max over links u → v of min(best[u], rank); the intervention's own is above every grade.
  const long top = static_cast<long>(t.levels.size());
  std::vector<long> best(n, std::numeric_limits<long>::min());
  std::vector<long> via(n, -1);
  best[a.from] = top;
  for (std::size_t v : a.order) {
    if (best[v] == std::numeric_limits<long>::min()) continue;
    for (std::size_t e : out[v]) {
      const long through = std::min(best[v], st[e].rank);
      const std::size_t w = arcs[e].second;
      if (through > best[w]) best[w] = through, via[w] = static_cast<long>(e);
    }
  }
  for (std::size_t v = a.to; v != a.from;) {
    const auto e = static_cast<std::size_t>(via[v]);
    a.strongest.links.insert(a.strongest.links.begin(), e);
    v = arcs[e].first;
  }
  a.strongest.weakest = best[a.to];
  long least_rank = top + 1;
  for (std::size_t i = 0; i < a.strongest.links.size(); i++) {
    const std::size_t e = a.strongest.links[i];
    if (st[e].rank < least_rank) least_rank = st[e].rank, a.strongest.limiting = i;
    for (const auto& f : t.links[e].support)
      if (std::find(a.strongest.support.begin(), a.strongest.support.end(), f) == a.strongest.support.end()) a.strongest.support.push_back(f);
  }

  // Wright's rule along the order, with interval arithmetic beside it.
  a.linear = true;
  for (std::size_t e = 0; e < L; e++)
    if (ways_from[arcs[e].first] * ways_to[arcs[e].second] > 0) {
      a.linear = a.linear && t.links[e].evidence.given;
      a.shared = a.shared || (ways_from[arcs[e].first] * ways_to[arcs[e].second] > 1);
    }
  if (a.linear) {
    std::vector<double> total(n, 0);
    std::vector<std::pair<double, double>> box(n, {0, 0});
    total[a.from] = 1, box[a.from] = {1, 1};
    for (std::size_t v : a.order)
      for (std::size_t e : out[v]) {
        if (ways_to[arcs[e].second] == 0 || ways_from[v] == 0) continue;
        const auto& ev = t.links[e].evidence;
        total[arcs[e].second] += total[v] * ev.effect;
        const auto p = detail::times(box[v], {ev.lower, ev.upper});
        box[arcs[e].second].first += p.first, box[arcs[e].second].second += p.second;
      }
    a.implied = total[a.to], a.implied_lower = box[a.to].first, a.implied_upper = box[a.to].second;
  }

  const auto& lim = t.links[a.strongest.links[a.strongest.limiting]];
  const std::string limiting = st[a.strongest.links[a.strongest.limiting]].why;
  std::string path = t.nodes[a.from].id;
  for (std::size_t e : a.strongest.links) path += " -> " + t.links[e].to;
  if (a.strongest.weakest >= need) {
    a.verdict = Verdict::yes(claim, "the strongest of " + std::to_string(a.paths) + (a.paths == 1 ? " path" : " paths") + ", " + path + ", is shown at " + t.levels[static_cast<std::size_t>(a.strongest.weakest)] +
                                        " or better at every link; its weakest: " + limiting);
  } else if (a.strongest.weakest == not_shown) {
    a.verdict = Verdict::unknown(claim, "every path has a link not shown; on the strongest, " + path + ", " + limiting);
  } else {
    a.verdict = Verdict::unknown(claim, "the strongest path, " + path + ", is shown only at " + t.levels[static_cast<std::size_t>(a.strongest.weakest)] + ", below the " + least + " asked; its weakest: " + limiting);
  }
  a.verdict.with("strongest path", path).with("limiting link", lim.from + " -> " + lim.to);
  for (std::size_t e : a.necessary_gaps) a.verdict.with("gap on every path", t.links[e].from + " -> " + t.links[e].to);
  for (const auto& f : a.strongest.support) a.verdict.with("support factor", f);
  return a;
}

// ---- The written form ------------------------------------------------------------------------------------------------------------

struct Parsed {
  bool ok = false;
  Theory theory;
  std::vector<std::string> errors;
};

namespace detail {

inline std::string trim(std::string_view s) {
  std::size_t a = 0, b = s.size();
  while (a < b && (s[a] == ' ' || s[a] == '\t' || s[a] == '\r')) a++;
  while (b > a && (s[b - 1] == ' ' || s[b - 1] == '\t' || s[b - 1] == '\r')) b--;
  return std::string(s.substr(a, b - a));
}

inline std::vector<std::string> words(const std::string& s) {
  std::vector<std::string> out;
  std::string w;
  for (char c : s) {
    if (c == ' ' || c == '\t') { if (!w.empty()) out.push_back(w), w.clear(); }
    else w += c;
  }
  if (!w.empty()) out.push_back(w);
  return out;
}

}  // namespace detail

inline Parsed read(std::string_view text) {
  Parsed out;
  Theory& t = out.theory;
  std::size_t number = 0, from = 0;
  const auto find_link = [&](const std::string& a, const std::string& b) -> Link* {
    for (auto& l : t.links)
      if (l.from == a && l.to == b) return &l;
    return nullptr;
  };
  while (from <= text.size()) {
    std::size_t to = text.find('\n', from);
    if (to == std::string_view::npos) to = text.size();
    const std::string line = detail::trim(text.substr(from, to - from));
    number++;
    from = to + 1;
    const auto err = [&](const std::string& why) { out.errors.push_back("line " + std::to_string(number) + ": " + why); };
    if (!line.empty() && line[0] != '#') {
      const std::size_t colon = line.find(':');
      const std::string head = detail::trim(std::string_view(line).substr(0, colon == std::string::npos ? line.size() : colon));
      const std::string tail = colon == std::string::npos ? "" : detail::trim(std::string_view(line).substr(colon + 1));
      const auto ws = detail::words(head);
      if (ws.empty()) err("a line begins with a word");
      else if (ws[0] == "theory" && ws.size() == 2 && colon == std::string::npos) t.name = ws[1];
      else if (ws[0] == "levels" && ws.size() >= 3 && colon == std::string::npos) t.levels.assign(ws.begin() + 1, ws.end());
      else if (ws[0] == "node" && ws.size() == 3 && colon != std::string::npos) {
        bool twice = false;
        for (const auto& nd : t.nodes) twice = twice || nd.id == ws[1];
        if (twice) err("the node " + ws[1] + " is declared twice");
        else t.nodes.push_back({ws[1], ws[2], tail});
      } else if ((ws[0] == "link" || ws[0] == "support" || ws[0] == "evidence" || ws[0] == "sign") && ws.size() == 4 && ws[2] == "->" && colon != std::string::npos) {
        Link* l = find_link(ws[1], ws[3]);
        if (ws[0] == "link") {
          if (l) err("the link " + ws[1] + " -> " + ws[3] + " is declared twice");
          else t.links.push_back({ws[1], ws[3], tail, {}, +1, {}});
        } else if (!l) {
          err("no link " + ws[1] + " -> " + ws[3] + " is declared before this line");
        } else if (ws[0] == "support") {
          if (tail.empty()) err("a support factor is named");
          else l->support.push_back(tail);
        } else if (ws[0] == "sign") {
          if (tail == "+") l->sign = +1;
          else if (tail == "-") l->sign = -1;
          else err("a sign is + or -");
        } else {
          // effect [lower, upper] grade
          double e = 0, lo = 0, hi = 0;
          char grade[64] = {0};
          if (std::sscanf(tail.c_str(), "%lf [%lf , %lf ] %63s", &e, &lo, &hi, grade) == 4 || std::sscanf(tail.c_str(), "%lf [%lf, %lf] %63s", &e, &lo, &hi, grade) == 4) {
            l->evidence = {true, e, lo, hi, grade};
          } else {
            err("evidence is written \"effect [lower, upper] grade\"");
          }
        }
      } else {
        err("a line is \"theory name\", \"levels l1 l2 …\", \"node id kind: text\", or \"link|support|evidence|sign a -> b: …\"");
      }
    }
    if (to == text.size()) break;
  }
  for (const auto& l : t.links) {
    bool a = false, b = false;
    for (const auto& nd : t.nodes) a = a || nd.id == l.from, b = b || nd.id == l.to;
    if (!a) out.errors.push_back("the link " + l.from + " -> " + l.to + " leaves a node not declared");
    if (!b) out.errors.push_back("the link " + l.from + " -> " + l.to + " reaches a node not declared");
    if (l.evidence.given && std::find(t.levels.begin(), t.levels.end(), l.evidence.grade) == t.levels.end())
      out.errors.push_back("the evidence of " + l.from + " -> " + l.to + " has a grade, " + l.evidence.grade + ", not among the levels");
  }
  if (t.levels.size() < 2) out.errors.push_back("a theory declares the levels of its grades, at least two");
  if (out.errors.empty()) {
    try {
      (void)order(t);
    } catch (const std::invalid_argument& e) {
      out.errors.push_back(std::string(e.what()).substr(std::string("policy::change: ").size()));
    }
  }
  out.ok = out.errors.empty();
  return out;
}

inline Theory theory(std::string_view text) {
  auto p = read(text);
  if (!p.ok) {
    std::string all;
    for (const auto& e : p.errors) all += (all.empty() ? "" : "; ") + e;
    throw std::invalid_argument("policy::change::theory: " + all);
  }
  return p.theory;
}

}  // namespace policy::change
