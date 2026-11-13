// Tunings for any work's engine (ADR 0111): one composition of the musicology domain's `pitch` and `tuning`,
// answering requests in JSON. Nothing is read from a file, and nothing is kept between requests.
//
//   tuning   {system, first_hz, from?, to?}   a tuning by its name, measured and made ready to sound: its degrees (name,
//                                             cents above the first, exact form), each fifth and major third on its
//                                             degrees with its departure from the pure interval in cents, the widest
//                                             departure among its fifths (the wolf), and the tones from index `from` to
//                                             `to` (0 the first degree at first_hz; 0 to the octave when not given)
//   compare  {a, b}                           two tunings of as many degrees, degree by degree: b less a, in cents
//   systems  {}                               the names a tuning is asked by
//
// A system is one of: pythagorean, sanfen-sunyi, ptolemy, just-12, meantone-<p>/<q> (the fraction of the syntonic comma
// by which each fifth is narrowed), equal-<n>, werckmeister-3, vallotti, urmawi-17. The pitch of the first degree is
// stated by the caller: there is no default.

#pragma once

#include "json.hpp"

#include <musicology/pitch/pitch.hpp>
#include <musicology/tuning/tuning.hpp>

#include <cmath>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <vector>

namespace science::sounding {

namespace mt = musicology::tuning;
namespace mp = musicology::pitch;

inline const std::vector<std::string>& systems() {
  static const std::vector<std::string> names{"pythagorean", "sanfen-sunyi", "ptolemy", "just-12", "meantone-1/4", "meantone-1/3", "meantone-1/6", "equal-12", "equal-19",
                                              "equal-31", "equal-53", "werckmeister-3", "vallotti", "urmawi-17"};
  return names;
}

inline mt::Tuning system(const std::string& name) {
  if (name == "pythagorean") return mt::pythagorean(12, 3);
  if (name == "sanfen-sunyi") return mt::sanfen_sunyi(12);
  if (name == "ptolemy") return mt::ptolemy_intense_diatonic();
  if (name == "just-12") return mt::just_chromatic();
  if (name == "werckmeister-3") return mt::werckmeister_iii();
  if (name == "vallotti") return mt::vallotti();
  if (name == "urmawi-17") return mt::urmawi();
  if (name.rfind("equal-", 0) == 0) {
    const long n = std::strtol(name.c_str() + 6, nullptr, 10);
    if (n < 1 || n > 1200) throw std::invalid_argument("science::sounding: an equal division has between 1 and 1200 parts");
    return mt::equal(static_cast<std::size_t>(n));
  }
  if (name.rfind("meantone-", 0) == 0) {
    char* rest = nullptr;
    const long p = std::strtol(name.c_str() + 9, &rest, 10);
    const long q = rest && *rest == '/' ? std::strtol(rest + 1, nullptr, 10) : 0;
    if (p < 0 || q < 1) throw std::invalid_argument("science::sounding: a meantone is asked as meantone-<p>/<q>, the fraction of the comma");
    return mt::meantone(mp::ratio(p, q));
  }
  throw std::invalid_argument("science::sounding: no tuning named " + name);
}

inline Json dyad_json(const mt::Dyad& d) {
  return Json::object({{"from", static_cast<double>(d.from)}, {"to", static_cast<double>(d.to)}, {"cents", d.cents}, {"departure", d.departure}, {"pure", d.pure}});
}

inline Json dyads_json(const std::vector<mt::Dyad>& dyads) {
  Json out = Json::array();
  for (const auto& d : dyads) out.push(dyad_json(d));
  return out;
}

inline Json answer(const Json& request) {
  const std::string op = request["op"].text();
  if (op == "systems") {
    Json out = Json::array();
    for (const auto& s : systems()) out.push(s);
    return Json::object({{"systems", out}});
  }
  if (op == "compare") {
    const mt::Tuning a = system(request["a"].text()), b = system(request["b"].text());
    Json cents = Json::array();
    for (double c : mt::compare(a, b)) cents.push(c);
    return Json::object({{"a", a.name}, {"b", b.name}, {"cents", cents}});
  }
  if (op != "tuning") throw std::invalid_argument("science::sounding: no operation named " + op + " (tuning, compare, systems)");
  const mt::Tuning t = system(request["system"].text());
  if (!request.has("first_hz")) throw std::invalid_argument("science::sounding: the pitch of the first degree is stated (first_hz); there is no default");
  const double hz = request["first_hz"].as_number();
  const long long from = request.has("from") ? static_cast<long long>(request["from"].as_number()) : 0;
  const long long to = request.has("to") ? static_cast<long long>(request["to"].as_number()) : static_cast<long long>(t.size());
  if (to < from || to - from > 400) throw std::invalid_argument("science::sounding: at most 400 tones are asked at once");
  Json degrees = Json::array();
  for (const auto& d : t.degrees) degrees.push(Json::object({{"name", d.name}, {"cents", d.cents}, {"form", d.form}}));
  Json tones = Json::array();
  for (const auto& tone : mt::frequencies(t, hz, from, to))
    tones.push(Json::object({{"index", static_cast<double>(tone.index)}, {"degree", static_cast<double>(tone.degree)}, {"octave", static_cast<double>(tone.octave)},
                             {"cents", tone.cents}, {"hz", tone.hz}}));
  const auto fifths = mt::fifths(t);
  std::size_t worst = 0;
  for (std::size_t i = 1; i < fifths.size(); i++)
    if (std::abs(fifths[i].departure) > std::abs(fifths[worst].departure)) worst = i;
  Json out = Json::object({{"system", request["system"].text()}, {"name", t.name}, {"first_hz", hz}, {"degrees", degrees}, {"fifths", dyads_json(fifths)},
                           {"major_thirds", dyads_json(mt::major_thirds(t))}, {"tones", tones}});
  if (!fifths.empty()) out.set("wolf", dyad_json(fifths[worst]));
  return out;
}

}  // namespace science::sounding
