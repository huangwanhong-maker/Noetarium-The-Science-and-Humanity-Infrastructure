// A water distribution network: junctions where water is drawn, reservoirs and tanks that hold it, pipes and pumps that carry it,
// patterns by which demands vary (theory note §2). Read from EPANET's input files (Rossman et al. 2020, EPANET 2.2 User Manual,
// EPA/600/R-20/133, Appendix C), converted at reading to SI: lengths, elevations and heads in m, diameters in m, flows in m³/s.
//
// EPANET's units, converted (Appendix C, Table C.1):
//   flows: CFS, GPM, MGD, IMGD, AFD (US customary; lengths in ft, diameters in inches, Darcy–Weisbach roughness in millifeet)
//          LPS, LPM, MLD, CMH, CMD (SI; lengths in m, diameters in mm, roughness in mm)
// Hazen–Williams' C is a number. A pump's curve keeps its flows in the file's flow unit and heads in its length unit until converted.
// Nothing of a particular city is held here: a network is an input.

#pragma once

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <fstream>
#include <map>
#include <numbers>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace civil_engineering::water {

struct Junction {
  std::string id;
  double elevation = 0, demand = 0;  // m, m³/s (base demand; the pattern multiplies it)
  std::string pattern;
};
struct Reservoir {
  std::string id;
  double head = 0;  // m
  std::string pattern;
};
struct Tank {
  std::string id;
  double elevation = 0, level = 0, min_level = 0, max_level = 0, diameter = 0;  // m; level is the initial water depth
  double area() const { return std::numbers::pi * diameter * diameter / 4; }
};
struct Pipe {
  std::string id, from, to;
  double length = 0, diameter = 0, roughness = 0, minor_loss = 0;  // m, m, (C, or m for Darcy–Weisbach), K
  bool open = true, check_valve = false;
};
struct Pump {
  std::string id, from, to;
  std::vector<std::pair<double, double>> curve;  // (flow m³/s, head m)
  double power = 0;                              // W, for a pump of constant power (POWER), when no curve
  bool open = true;
  std::string pattern;                           // a speed pattern, if any
};
struct Control {  // LINK x OPEN|CLOSED IF NODE y BELOW|ABOVE v (a tank's level or a junction's pressure)
  std::string link, node;
  bool open = true, below = true;
  double value = 0;  // m
};

struct Network {
  std::string title;
  std::vector<Junction> junctions;
  std::vector<Reservoir> reservoirs;
  std::vector<Tank> tanks;
  std::vector<Pipe> pipes;
  std::vector<Pump> pumps;
  std::map<std::string, std::vector<double>> patterns;
  std::vector<Control> controls;
  std::string headloss = "H-W";       // H-W or D-W
  double pattern_step = 3600, hydraulic_step = 3600, duration = 0;  // s
  double demand_multiplier = 1;
  std::string default_pattern = "1";  // EPANET's: a junction with no pattern takes pattern "1" when it exists
};

namespace detail {
inline void require(bool ok, const std::string& what) {
  if (!ok) throw std::invalid_argument("civil_engineering::water: " + what);
}
inline std::string upper(std::string s) {
  for (char& c : s) c = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
  return s;
}
inline double clock(const std::string& s) {  // "2:00" (h:mm), "1.5" (hours), "1:30:00"
  std::vector<double> parts;
  std::stringstream ss(s);
  for (std::string p; std::getline(ss, p, ':');) parts.push_back(std::stod(p));
  if (parts.size() == 1) return parts[0] * 3600;
  if (parts.size() == 2) return parts[0] * 3600 + parts[1] * 60;
  return parts[0] * 3600 + parts[1] * 60 + parts[2];
}
}  // namespace detail

struct Units {
  double flow = 6.30901964e-5;  // m³/s per file unit (GPM by default)
  double length = 0.3048;       // m per file unit of length
  double diameter = 0.0254;     // m per file unit of diameter
  double roughness = 0.0003048; // m per file unit of Darcy–Weisbach roughness
  bool us = true;
};

inline Units units_of(const std::string& name) {
  const std::string u = detail::upper(name);
  Units x;
  static const std::map<std::string, double> us{{"CFS", 0.028316846592}, {"GPM", 6.30901964e-5}, {"MGD", 0.0438126364}, {"IMGD", 0.0526168042}, {"AFD", 0.0142764102}};
  static const std::map<std::string, double> si{{"LPS", 1e-3}, {"LPM", 1e-3 / 60}, {"MLD", 1e3 / 86400}, {"CMH", 1.0 / 3600}, {"CMD", 1.0 / 86400}};
  if (us.count(u)) x.flow = us.at(u), x.us = true;
  else if (si.count(u)) x.flow = si.at(u), x.us = false, x.length = 1, x.diameter = 1e-3, x.roughness = 1e-3;
  else detail::require(false, "unknown flow units " + name);
  return x;
}

// EPANET's input file, read. Sections not used here (quality, energy, coordinates) are passed over.
inline Network read_inp(std::istream& in) {
  std::map<std::string, std::vector<std::vector<std::string>>> sec;
  std::string line, current;
  while (std::getline(in, line)) {
    const auto semi = line.find(';');
    if (semi != std::string::npos) line = line.substr(0, semi);
    std::stringstream ss(line);
    std::vector<std::string> w;
    for (std::string t; ss >> t;) w.push_back(t);
    if (w.empty()) continue;
    if (w[0].front() == '[') {
      current = detail::upper(w[0]);
      continue;
    }
    sec[current].push_back(w);
  }
  Network net;
  Units u;
  for (const auto& w : sec["[OPTIONS]"]) {
    const std::string k = detail::upper(w[0]);
    if (k == "UNITS" && w.size() > 1) u = units_of(w[1]);
    if (k == "HEADLOSS" && w.size() > 1) net.headloss = detail::upper(w[1]);
    if (k == "PATTERN" && w.size() > 1) net.default_pattern = w[1];
    if (k == "DEMAND" && w.size() > 2 && detail::upper(w[1]) == "MULTIPLIER") net.demand_multiplier = std::stod(w[2]);
  }
  detail::require(net.headloss == "H-W" || net.headloss == "D-W", "head loss formula " + net.headloss + " (H-W and D-W are read)");
  for (const auto& w : sec["[TIMES]"]) {
    const std::string k = detail::upper(w[0]);
    const std::string k2 = w.size() > 1 ? detail::upper(w[1]) : "";
    if (k == "DURATION" && w.size() > 1) net.duration = detail::clock(w[1]);
    if (k == "PATTERN" && k2 == "TIMESTEP" && w.size() > 2) net.pattern_step = detail::clock(w[2]);
    if (k == "HYDRAULIC" && k2 == "TIMESTEP" && w.size() > 2) net.hydraulic_step = detail::clock(w[2]);
  }
  for (const auto& w : sec["[TITLE]"]) {
    for (const auto& t : w) net.title += (net.title.empty() ? "" : " ") + t;
    break;
  }
  for (const auto& w : sec["[JUNCTIONS]"])
    net.junctions.push_back({w[0], std::stod(w.at(1)) * u.length, w.size() > 2 ? std::stod(w[2]) * u.flow : 0.0, w.size() > 3 ? w[3] : ""});
  // [DEMANDS] replaces the junctions' demands where it names them (several demands of one junction are summed when they share a pattern).
  std::map<std::string, double> extra;
  std::map<std::string, std::string> extra_pattern;
  for (const auto& w : sec["[DEMANDS]"]) extra[w[0]] += std::stod(w.at(1)) * u.flow, extra_pattern[w[0]] = w.size() > 2 ? w[2] : "";
  for (auto& j : net.junctions)
    if (extra.count(j.id)) j.demand = extra[j.id], j.pattern = extra_pattern[j.id];
  for (const auto& w : sec["[RESERVOIRS]"]) net.reservoirs.push_back({w[0], std::stod(w.at(1)) * u.length, w.size() > 2 ? w[2] : ""});
  for (const auto& w : sec["[TANKS]"])
    net.tanks.push_back({w[0], std::stod(w.at(1)) * u.length, std::stod(w.at(2)) * u.length, std::stod(w.at(3)) * u.length, std::stod(w.at(4)) * u.length,
                         std::stod(w.at(5)) * u.length});
  for (const auto& w : sec["[PIPES]"]) {
    Pipe p{w[0], w.at(1), w.at(2), std::stod(w.at(3)) * u.length, std::stod(w.at(4)) * u.diameter, std::stod(w.at(5)), w.size() > 6 ? std::stod(w[6]) : 0.0};
    if (net.headloss == "D-W") p.roughness *= u.roughness;
    if (w.size() > 7) {
      const std::string s = detail::upper(w[7]);
      p.open = s != "CLOSED";
      p.check_valve = s == "CV";
    }
    net.pipes.push_back(p);
  }
  std::map<std::string, std::vector<std::pair<double, double>>> curves;
  for (const auto& w : sec["[CURVES]"]) curves[w[0]].push_back({std::stod(w.at(1)) * u.flow, std::stod(w.at(2)) * u.length});
  for (const auto& w : sec["[PUMPS]"]) {
    Pump p{w[0], w.at(1), w.at(2), {}, 0, true, ""};
    for (std::size_t i = 3; i + 1 < w.size(); i += 2) {
      const std::string k = detail::upper(w[i]);
      if (k == "HEAD") p.curve = curves.at(w[i + 1]);
      if (k == "POWER") p.power = std::stod(w[i + 1]) * (u.us ? 745.699872 : 1000.0);  // hp or kW
      if (k == "PATTERN") p.pattern = w[i + 1];
    }
    net.pumps.push_back(p);
  }
  for (const auto& w : sec["[STATUS]"]) {
    const std::string s = detail::upper(w.at(1));
    for (auto& p : net.pipes)
      if (p.id == w[0]) p.open = s != "CLOSED";
    for (auto& p : net.pumps)
      if (p.id == w[0]) p.open = s != "CLOSED";
  }
  for (const auto& w : sec["[PATTERNS]"])
    for (std::size_t i = 1; i < w.size(); i++) net.patterns[w[0]].push_back(std::stod(w[i]));
  for (const auto& w : sec["[CONTROLS]"]) {
    // LINK id OPEN|CLOSED IF NODE id BELOW|ABOVE value: a tank's level (in the length unit), or a junction's pressure (psi or m).
    if (w.size() >= 8 && detail::upper(w[0]) == "LINK" && detail::upper(w[3]) == "IF" && detail::upper(w[4]) == "NODE") {
      const bool tank = std::any_of(net.tanks.begin(), net.tanks.end(), [&](const Tank& t) { return t.id == w[5]; });
      const double raw = std::stod(w[7]);
      net.controls.push_back({w[1], w[5], detail::upper(w[2]) == "OPEN", detail::upper(w[6]) == "BELOW", tank ? raw * u.length : raw * (u.us ? 0.70307 : 1.0)});
    }
  }
  return net;
}

inline Network read_inp(const std::string& path) {
  std::ifstream f(path);
  detail::require(static_cast<bool>(f), "cannot read " + path);
  return read_inp(f);
}

}  // namespace civil_engineering::water
