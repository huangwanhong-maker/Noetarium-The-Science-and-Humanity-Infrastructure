// Argument maps for any work's engine (ADR 0111): one composition of the philosophy domain's `arguments`, answering
// requests in JSON. Nothing is read from a file, and nothing is kept between requests.
//
//   map   {text}   an argument map in its written form (philosophy/arguments/maps.hpp: questions, the positions that
//                  answer them, arguments for and against, premises and conclusions) read and evaluated under the
//                  grounded semantics: the lines that could not be read, the faults of the map, and, when it is well
//                  formed, every question with its positions by their standing and whether it is open, and every
//                  position and argument with its standing (accepted, rejected or undecided), the round at which it
//                  was settled, what attacks it and what decides it.

#pragma once

#include "json.hpp"

#include <philosophy/arguments/maps.hpp>

#include <stdexcept>
#include <string>
#include <vector>

namespace science::arguments {

namespace pa = philosophy::arguments;

inline Json strings(const std::vector<std::string>& xs) {
  Json out = Json::array();
  for (const auto& x : xs) out.push(x);
  return out;
}

inline Json answer(const Json& request) {
  const std::string op = request["op"].text();
  if (op != "map") throw std::invalid_argument("science::arguments: no operation named " + op + " (map)");
  const pa::Reading read = pa::read(request["text"].text());
  const pa::Map& m = read.map;
  Json errors = Json::array(), faults = Json::array(), questions = Json::array(), positions = Json::array(), arguments = Json::array();
  for (const auto& e : read.errors) errors.push(Json::object({{"line", static_cast<double>(e.line)}, {"why", e.why}}));
  const auto found = pa::faults(m);
  for (const auto& f : found) faults.push(Json::object({{"id", f.id}, {"why", f.why}, {"involved", strings(f.involved)}}));
  Json out = Json::object({{"errors", errors}, {"faults", faults}});
  const bool sound = found.empty() && read.ok();
  const auto status = [](pa::Status s) { return s == pa::Status::accepted ? "accepted" : s == pa::Status::rejected ? "rejected" : "undecided"; };
  const auto standing = [&](const pa::Standing& s) {
    return Json::object({{"status", status(s.status)}, {"round", static_cast<double>(s.round)}, {"attackers", strings(s.attackers)}, {"deciding", strings(s.deciding)}});
  };
  pa::Evaluation E;
  if (sound) E = pa::evaluate(m);
  for (std::size_t i = 0; i < m.questions.size(); i++) {
    const auto& q = m.questions[i];
    Json row = Json::object({{"id", q.id}, {"text", q.text}, {"exclusive", q.exclusive}, {"raised_by", q.raised_by}});
    if (sound) {
      const auto& s = E.questions[i];
      row.set("open", s.open);
      row.set("moot", s.moot);
      row.set("accepted", strings(s.accepted));
      row.set("rejected", strings(s.rejected));
      row.set("undecided", strings(s.undecided));
    }
    questions.push(row);
  }
  for (std::size_t i = 0; i < m.positions.size(); i++) {
    const auto& p = m.positions[i];
    Json row = Json::object({{"id", p.id}, {"text", p.text}, {"answers", p.answers}});
    if (sound) row.set("standing", standing(E.positions[i]));
    positions.push(row);
  }
  for (std::size_t i = 0; i < m.arguments.size(); i++) {
    const auto& a = m.arguments[i];
    Json bears = Json::array();
    for (const auto& b : a.bears) bears.push(Json::object({{"target", b.target}, {"polarity", b.polarity == pa::Polarity::pro ? "pro" : "con"}}));
    Json row = Json::object({{"id", a.id}, {"text", a.text}, {"bears", bears}, {"premises", strings(a.premises)}, {"conclusion", a.conclusion}});
    if (sound) row.set("standing", standing(E.arguments[i]));
    arguments.push(row);
  }
  out.set("questions", questions);
  out.set("positions", positions);
  out.set("arguments", arguments);
  out.set("open_questions", sound ? strings(E.open_questions) : Json::array());
  out.set("evaluated", sound);
  return out;
}

}  // namespace science::arguments
