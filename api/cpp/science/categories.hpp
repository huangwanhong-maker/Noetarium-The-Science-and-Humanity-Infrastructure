// Categories for any work's engine (ADR 0100): one composition of kategoria's script language, answering requests in JSON.
// A request holds a whole script; nothing is read from a file, and nothing is kept between requests.
//
//   run      {"script": "…", "most"?: n}    carries out the script (kategoria/script/language.hpp says what one may write);
//                                           answers {"outcomes": [...]}, one for each statement in order, each with its line and
//                                           text, its kind and name, a status ("holds", "fails", "undecided", or "error" for a
//                                           statement that could not be carried out), the claim, the reason, the evidence piece
//                                           by piece, and a view of what there is to draw. "most" bounds the arrows a statement
//                                           may build (3000 when not given).
//   grammar  {}                             the grammar of a statement as it is read, in BNF: {"text": "…"}
//
// The views are kategoria's (kategoria/script/session.hpp): a category with its objects, arrows, generators and table; a
// functor, a transformation or an adjunction between two categories; a set-valued functor with its elements and the action of
// each generator; a cone; a derivation, path by path; string diagrams; a theory, a model's laws; a table.

#pragma once

#include "json.hpp"

#include <kategoria/script/session.hpp>

#include <stdexcept>
#include <string>

namespace science::categories {

// A document of kategoria as the API's JSON.
inline Json json_of(const kategoria::script::Value& v) {
  using Kind = kategoria::script::Value::Kind;
  switch (v.kind()) {
    case Kind::null: return Json();
    case Kind::boolean: return Json(v.boolean());
    case Kind::number: return Json(v.number());
    case Kind::text: return Json(v.text());
    case Kind::list: {
      Json out = Json::array();
      for (const auto& item : v.items()) out.push(json_of(item));
      return out;
    }
    default: {
      Json out = Json::object();
      for (const auto& [key, value] : v.fields()) out.set(key, json_of(value));
      return out;
    }
  }
}

// The answer to one request.
inline Json answer(const Json& request) {
  const std::string op = request["op"].text("run");
  if (op == "grammar") return Json::object({{"text", std::string(kategoria::script::grammar_text())}});
  if (op != "run") throw std::invalid_argument("science::categories: no operation named " + op + " (run, grammar)");
  kategoria::script::Session session;
  if (request.has("most")) session.most_arrows = static_cast<std::size_t>(request["most"].as_number());
  return Json::object({{"outcomes", json_of(session.run(request["script"].text()))}});
}

}  // namespace science::categories
