// Interpretive authority: whose reading of a norm binds a forum, and how courts' readings enter a case.
//
// An open term of a rule is read by the parties' arguments (from the text, the system, the purpose: apodeixis's canons) and by
// courts. A court belongs to a legal order and interprets some instruments with authority; a holding of it fixes a reading of a
// term. Whether a holding binds the forum that must apply the rule is a matter of the orders the forum is bound by, and that is
// declared, as every ranking here is: a scheme states, for instance, that a forum within one order is bound by the readings of
// that order's highest court for the instruments it interprets. Nothing here names an order, a court or a forum.
//
// Holdings become apodeixis's interpretive arguments, each of a kind of its own, and the kinds are ordered so that
// - a persuasive holding is below a binding one;
// - among binding holdings of one order, a lower court's is below a higher court's;
// - of one court on one term, an earlier holding is below a later one, since a court may depart from itself (unless the caller
//   says otherwise);
// - the parties' canons are below every binding holding, and above, below or unranked against persuasive ones as declared.
// Under apodeixis's grounded semantics an argument whose kind is below another's does not defeat it, so a later binding holding
// fixes its reading against an earlier one and against the parties' canons, and a persuasive holding stands against a canon only as
// the scheme lets it (MacCormick and Summers 1991, Interpreting Statutes, ch. 13, on the force of precedent in interpretation;
// Sartor, Walton, Macagno and Rotolo 2014 on arguments from authority among the schemes of interpretation).
//
// A holding dated after the date of the question is left out when a date is given: what a court had not yet said cannot have been
// relied on. A holding that reads a term the case does not have, or a reading the term does not admit, or that names a court not
// declared, is a fault, reported and left out; nothing is repaired.

#pragma once

#include <apodeixis/interpretation/interpretation.hpp>

#include <map>
#include <set>
#include <string>
#include <string_view>
#include <vector>

namespace law::authority {

namespace ai = apodeixis::interpretation;

struct Court {
  std::string id, name;
  std::string order;                  // the legal order it belongs to
  int level = 0;                      // within its order: higher is higher
  std::set<std::string> instruments;  // the instruments it interprets with authority
};

struct Holding {
  std::string id, court;
  std::string name, citation, date;   // the case, its citation (an ECLI, say), the date of judgment YYYY-MM-DD
  std::string instrument, term, reading;
  std::string passage;                // the words of the judgment that fix the reading, as the judgment has them
};

struct Forum {
  std::string id, name;
  std::set<std::string> bound_by;     // the orders whose courts' readings bind it, for the instruments they interpret
};

enum class Force { binding, persuasive, left_out };

inline std::string_view word(Force f) { return f == Force::binding ? "binding" : f == Force::persuasive ? "persuasive" : "left out"; }

enum class Canons { below, unranked, above };  // the parties' canons against persuasive holdings

struct Options {
  std::string date;                   // the date of the question; holdings after it are left out (empty: none)
  bool later_prevails = true;         // of one court on one term, a later holding over an earlier
  Canons canons = Canons::unranked;
};

// A court's force over a forum for an instrument: binding when the forum is bound by the court's order and the court interprets the
// instrument with authority, persuasive otherwise. (A decision after the date of the question is left out by the caller.)
inline Force force_for(const Court& c, const Forum& forum, const std::string& instrument) {
  return forum.bound_by.count(c.order) > 0 && c.instruments.count(instrument) > 0 ? Force::binding : Force::persuasive;
}

struct Assessed {
  std::string holding;
  Force force = Force::left_out;
  std::string kind;                   // its kind among the interpretive arguments
  std::string reason;
};

struct Authority {
  std::vector<Assessed> assessed;
  std::vector<ai::InterpretiveArgument> arguments;              // the holdings that count, as arguments
  std::vector<std::pair<std::string, std::string>> canon_order;  // (weaker, stronger) over kinds, the canons' among them
  std::vector<std::string> faults;
};

// The holdings weighed for a forum: which bind, which persuade, which are left out and why; and the arguments and order they give.
// `canon_kinds` are the kinds of the parties' arguments in the case (text, purpose, …), to be placed beneath binding holdings.
inline Authority weigh(const std::vector<ai::OpenPoint>& points, const std::vector<Court>& courts, const std::vector<Holding>& holdings, const Forum& forum,
                       const std::vector<std::string>& canon_kinds = {}, const Options& o = {}) {
  Authority out;
  std::map<std::string, const Court*> court;
  for (const auto& c : courts) court[c.id] = &c;
  struct Counted {
    const Holding* h;
    const Court* c;
    bool binding;
    std::string kind;
  };
  std::vector<Counted> counted;
  for (const auto& h : holdings) {
    Assessed a{h.id, Force::left_out, "", ""};
    const auto ct = court.find(h.court);
    long point = -1, reading = -1;
    for (std::size_t k = 0; k < points.size(); k++)
      if (points[k].name == h.term) {
        point = static_cast<long>(k);
        for (std::size_t r = 0; r < points[k].readings.size(); r++)
          if (points[k].readings[r] == h.reading) reading = static_cast<long>(r);
      }
    if (ct == court.end()) {
      a.reason = "it names the court " + h.court + ", which is not declared";
      out.faults.push_back(h.id + ": " + a.reason);
    } else if (point < 0) {
      a.reason = "it reads " + h.term + ", which is no open term of the case";
    } else if (reading < 0) {
      a.reason = "it reads " + h.term + " as " + h.reading + ", which the term does not admit";
      out.faults.push_back(h.id + ": " + a.reason);
    } else if (!o.date.empty() && !h.date.empty() && h.date > o.date) {
      a.reason = "it was given on " + h.date + ", after the date of the question (" + o.date + ")";
    } else {
      const Court& c = *ct->second;
      const bool order = forum.bound_by.count(c.order) > 0;
      a.force = force_for(c, forum, h.instrument);
      a.kind = std::string(a.force == Force::binding ? "binding: " : "persuasive: ") + c.name + (h.date.empty() ? "" : " (" + h.date + ")") + " in " + (h.name.empty() ? h.id : h.name);
      a.reason = a.force == Force::binding ? c.name + " binds " + forum.name + " (" + c.order + "), and interprets " + h.instrument + " with authority"
                 : !order ? c.name + " (" + c.order + ") does not bind " + forum.name + ": its reading persuades"
                          : c.name + " does not interpret " + h.instrument + " with authority: its reading persuades and does not bind";
      out.arguments.push_back({h.id, point, reading, a.kind, (h.name.empty() ? "" : h.name + ": ") + (h.passage.empty() ? h.term + " read as " + h.reading : h.passage), 0});
      counted.push_back({&h, &c, a.force == Force::binding, a.kind});
    }
    out.assessed.push_back(std::move(a));
  }
  std::set<std::pair<std::string, std::string>> order;
  for (const auto& x : counted)
    for (const auto& y : counted) {
      if (x.h == y.h || x.kind == y.kind) continue;
      bool below = false;
      if (!x.binding && y.binding) below = true;
      else if (x.binding == y.binding && x.c != y.c && x.c->order == y.c->order && x.c->level < y.c->level) below = true;
      else if (o.later_prevails && x.c == y.c && x.h->term == y.h->term && !x.h->date.empty() && !y.h->date.empty() && x.h->date < y.h->date) below = true;
      if (below) order.insert({x.kind, y.kind});
    }
  for (const auto& k : canon_kinds)
    for (const auto& x : counted) {
      if (x.binding) order.insert({k, x.kind});
      else if (o.canons == Canons::below) order.insert({k, x.kind});
      else if (o.canons == Canons::above) order.insert({x.kind, k});
    }
  out.canon_order.assign(order.begin(), order.end());
  return out;
}

// The case with the holdings that count added to its interpretive arguments, and their order to its ranking of kinds.
inline void add_to(ai::Case& c, const Authority& a) {
  c.arguments.insert(c.arguments.end(), a.arguments.begin(), a.arguments.end());
  c.canon_order.insert(c.canon_order.end(), a.canon_order.begin(), a.canon_order.end());
}

}  // namespace law::authority
