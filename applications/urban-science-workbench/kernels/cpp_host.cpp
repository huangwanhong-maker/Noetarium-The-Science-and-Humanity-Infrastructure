// The Studio's C++ host (the design note of 2026-11-07, §8.7): a process kept running for the Studio, which loads each C++ script
// as a shared library (dlopen) and answers its ticks, one JSON request a line on its input, one answer a line on its output.
//
//   {"op": "load", "id", "path"}   load a script's library; one already loaded under the id is swapped out, its state handed over
//                                  through its save() and the new one's load(), if both declare them
//   {"op": "tick", "id", …}        a tick of the script (api/cpp/studio_plugin.hpp's call), answered as the script answers it
//   {"op": "unload", "id"}         the script and its library let go
//   {"op": "status"}               the scripts loaded
//
// A script that crashes ends this process: the server starts it again, and the Studio loads its scripts anew, saying so.

#include <science/json.hpp>

#include <dlfcn.h>

#include <iostream>
#include <map>
#include <string>

namespace {

using science::Json;

struct Loaded {
  void* library = nullptr;
  void* self = nullptr;
  void* (*create)() = nullptr;
  void (*destroy)(void*) = nullptr;
  char* (*call)(void*, const char*) = nullptr;
  void (*release)(char*) = nullptr;
};

std::map<std::string, Loaded> systems;

std::string ask(Loaded& l, const std::string& request) {
  char* out = l.call(l.self, request.c_str());
  std::string answer(out ? out : "");
  if (out) l.release(out);
  return answer;
}

void let_go(Loaded& l) {
  if (l.self && l.destroy) l.destroy(l.self);
  if (l.library) dlclose(l.library);
  l = Loaded{};
}

std::string failure(const std::string& why) { return Json::object({{"ok", Json(false)}, {"error", Json(why)}}).dump(); }

std::string load(const std::string& id, const std::string& path) {
  Loaded l;
  l.library = dlopen(path.c_str(), RTLD_NOW | RTLD_LOCAL);
  if (!l.library) return failure(std::string("the library did not load: ") + dlerror());
  l.create = reinterpret_cast<void* (*)()>(dlsym(l.library, "science_studio_create"));
  l.destroy = reinterpret_cast<void (*)(void*)>(dlsym(l.library, "science_studio_destroy"));
  l.call = reinterpret_cast<char* (*)(void*, const char*)>(dlsym(l.library, "science_studio_call"));
  l.release = reinterpret_cast<void (*)(char*)>(dlsym(l.library, "science_studio_free"));
  if (!l.create || !l.destroy || !l.call || !l.release) {
    dlclose(l.library);
    return failure("the script declares no system: end it with SCIENCE_STUDIO_SYSTEM(YourSystem)");
  }
  l.self = l.create();
  const Json described = Json::parse(ask(l, R"({"op":"describe"})"));
  if (!described["ok"].is_bool() || !described["ok"].as_bool()) {
    const std::string why = described["error"].text("its describe() failed");
    let_go(l);
    return failure(why);
  }
  bool carried = false;
  auto old = systems.find(id);
  if (old != systems.end()) {
    const Json saved = Json::parse(ask(old->second, R"({"op":"save"})"));
    if (saved["ok"].is_bool() && saved["ok"].as_bool() && !saved["result"].is_null()) {
      const Json loaded = Json::parse(ask(l, Json::object({{"op", Json("load")}, {"state", saved["result"]}}).dump()));
      carried = loaded["ok"].is_bool() && loaded["ok"].as_bool();
    }
    let_go(old->second);
    systems.erase(old);
  }
  systems[id] = l;
  return Json::object({{"ok", Json(true)}, {"result", Json::object({{"describe", described["result"]["describe"]}, {"carried", Json(carried)}})}}).dump();
}

std::string respond(const std::string& line) {
  try {
    const Json r = Json::parse(line);
    const std::string op = r["op"].text(), id = r["id"].text();
    if (op == "load") return load(id, r["path"].text());
    if (op == "tick") {
      auto it = systems.find(id);
      if (it == systems.end()) return failure("no system " + id + " is loaded");
      return ask(it->second, line);
    }
    if (op == "unload") {
      auto it = systems.find(id);
      if (it != systems.end()) { let_go(it->second); systems.erase(it); }
      return Json::object({{"ok", Json(true)}, {"result", Json::object()}}).dump();
    }
    if (op == "status") {
      Json ids = Json::array();
      for (const auto& [k, v] : systems) ids.push(Json(k));
      return Json::object({{"ok", Json(true)}, {"result", Json::object({{"systems", ids}})}}).dump();
    }
    return failure("no operation " + op);
  } catch (const std::exception& e) {
    return failure(e.what());
  }
}

}  // namespace

int main() {
  std::ios::sync_with_stdio(false);
  std::string line;
  while (std::getline(std::cin, line)) {
    if (line.empty()) continue;
    std::cout << respond(line) << "\n" << std::flush;
  }
  for (auto& [k, v] : systems) let_go(v);
  return 0;
}
