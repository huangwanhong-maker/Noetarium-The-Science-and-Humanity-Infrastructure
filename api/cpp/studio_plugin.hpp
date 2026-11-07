// Scripts of the Studio in C++ (the design note of 2026-11-07, §8; its kernels, §8.7): the same contract as JavaScript's and Python's.
// A script is compiled alone as a shared library and loaded by the Studio's host (applications/urban-science-workbench/kernels/
// cpp_host.cpp), which keeps running for the project; compiled again when it changes, it is swapped in, its state handed over by
// save() and load() if it declares them.
//
//   #include "studio_plugin.hpp"
//   namespace st = science::studio;
//   struct Crowd : st::System {
//     science::Json describe() const override { return st::declaration("A crowd", "tick", {"radius"}, {"crowd"}, {"crowd"}); }
//     void tick(const st::Context& c, st::Io& io) override { io.set("crowd", io.prev_number("crowd", 0) + 1); }
//     science::Json draw(const st::Context& c, st::Io& io) override { return science::Json::array({st::points("The crowd", {144.96}, {-37.81})}); }
//   };
//   SCIENCE_STUDIO_SYSTEM(Crowd)
//
// The boundary between the host and the library is C: a request and its answer pass as JSON text, so that the two need share no
// layout of a C++ type.

#pragma once

#include <science/json.hpp>

#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>
#include <vector>

namespace science::studio {

struct Context {
  double tick = 0, dt = 0, time_ms = 0;
  // view: the world frame's camera and range in space and time (lon, lat, range, tilt, heading, footprint, bbox, metres_per_px);
  // frames: the canvas's frames by id, each with its kind and its width and height in the canvas's pixels.
  Json params = Json::object(), view = Json(), frames = Json::object();
  double param(const std::string& name, double fallback) const {
    const Json* v = params.is_object() ? params.find(name) : nullptr;
    return v && v->is_number() ? v->as_number() : fallback;
  }
};

class Io {
 public:
  Io(Json values, Json previous) : values_(std::move(values)), previous_(std::move(previous)) {}
  const Json* get(const std::string& name) const { return values_.is_object() ? values_.find(name) : nullptr; }
  const Json* prev(const std::string& name) const { return previous_.is_object() ? previous_.find(name) : nullptr; }
  double number(const std::string& name, double fallback) const { const Json* v = get(name); return v && v->is_number() ? v->as_number() : fallback; }
  double prev_number(const std::string& name, double fallback) const { const Json* v = prev(name); return v && v->is_number() ? v->as_number() : fallback; }
  void set(const std::string& name, Json value) { writes_.set(name, std::move(value)); }
  // A contribution to a merged resource: the controller combines every writer's in the resource's merge.
  void add(const std::string& name, Json value) {
    Json all = adds_.has(name) ? adds_[name] : Json::array();
    all.push(std::move(value));
    adds_.set(name, std::move(all));
  }
  void log(const std::string& line) { log_.push(Json(line)); }
  Json answer(Json marks) const {
    return Json::object({{"writes", writes_}, {"adds", adds_}, {"marks", std::move(marks)}, {"log", log_}});
  }

 private:
  Json values_, previous_;
  Json writes_ = Json::object(), adds_ = Json::object(), log_ = Json::array();
};

struct System {
  virtual ~System() = default;
  virtual Json describe() const = 0;
  virtual void start(const Context&, Io&) {}
  virtual void tick(const Context&, Io&) {}
  virtual Json draw(const Context&, Io&) { return Json::array(); }
  virtual Json save() const { return Json(); }
  virtual void load(const Json&) {}
};

inline Json names(const std::vector<std::string>& v) {
  Json a = Json::array();
  for (const auto& s : v) a.push(Json(s));
  return a;
}

// A system's declaration, as the scheduler and the inspector read it.
inline Json declaration(const std::string& title, const std::string& phase, const std::vector<std::string>& reads = {},
                        const std::vector<std::string>& writes = {}, const std::vector<std::string>& previous = {}, Json params = Json::array(),
                        Json resources = Json::object()) {
  return Json::object({{"title", Json(title)}, {"phase", Json(phase == "update" ? "update" : "tick")}, {"reads", names(reads)}, {"writes", names(writes)},
                       {"previous", names(previous)}, {"after", Json::array()}, {"params", std::move(params)}, {"resources", std::move(resources)}});
}

inline Json numbers(const std::vector<double>& v) {
  Json a = Json::array();
  for (double x : v) a.push(Json(x));
  return a;
}

inline Json points(const std::string& title, const std::vector<double>& lon, const std::vector<double>& lat, const std::vector<double>& value = {},
                   double size = 6, const std::string& map = "plum") {
  return Json::object({{"kind", Json("points")}, {"title", Json(title)}, {"lon", numbers(lon)}, {"lat", numbers(lat)}, {"height", Json(0.0)},
                       {"value", value.empty() ? Json() : numbers(value)}, {"size", Json(size)}, {"map", Json(map)}});
}

inline Json columns(const std::string& title, const std::vector<double>& lon, const std::vector<double>& lat, const std::vector<double>& value,
                    double radius = 60, double top = 300, const std::string& map = "plum") {
  return Json::object({{"kind", Json("columns")}, {"title", Json(title)}, {"lon", numbers(lon)}, {"lat", numbers(lat)}, {"value", numbers(value)},
                       {"radius", Json(radius)}, {"top", Json(top)}, {"map", Json(map)}});
}

// A statistical figure for a frame of the canvas: "line", "bars" or "scatter", of x against y.
inline Json chart(const std::string& frame, const std::string& kind, const std::vector<double>& x, const std::vector<double>& y,
                  const std::string& title = "", const std::string& xlabel = "", const std::string& ylabel = "") {
  return Json::object({{"kind", Json("chart")}, {"frame", Json(frame)}, {"chart", Json(kind)}, {"x", numbers(x)}, {"y", numbers(y)},
                       {"title", Json(title)}, {"xlabel", Json(xlabel)}, {"ylabel", Json(ylabel)}});
}

// A drawing of one's own for a frame: items in the frame's pixels, each {"type": "line" | "circle" | "rect" | "text" | "path", ...}.
inline Json drawing(const std::string& frame, Json items) {
  return Json::object({{"kind", Json("drawing")}, {"frame", Json(frame)}, {"items", std::move(items)}});
}

// The answer to one request from the host, as JSON text.
inline std::string call(System& s, bool& started, const std::string& text) {
  try {
    const Json r = Json::parse(text);
    const std::string op = r["op"].text();
    if (op == "describe") return Json::object({{"ok", Json(true)}, {"result", Json::object({{"describe", s.describe()}})}}).dump();
    if (op == "save") return Json::object({{"ok", Json(true)}, {"result", s.save()}}).dump();
    if (op == "load") { s.load(r["state"]); return Json::object({{"ok", Json(true)}, {"result", Json::object()}}).dump(); }
    if (op == "tick") {
      Context c;
      c.tick = r.has("tick") ? r["tick"].as_number() : 0;
      c.dt = r.has("dt") ? r["dt"].as_number() : 0;
      c.time_ms = r.has("time_ms") ? r["time_ms"].as_number() : 0;
      if (r.has("params")) c.params = r["params"];
      if (r.has("view")) c.view = r["view"];
      if (r.has("frames")) c.frames = r["frames"];
      Io io(r.has("values") ? r["values"] : Json::object(), r.has("previous") ? r["previous"] : Json::object());
      if (!started) { started = true; s.start(c, io); }
      s.tick(c, io);
      Json marks = s.draw(c, io);
      return Json::object({{"ok", Json(true)}, {"result", io.answer(std::move(marks))}}).dump();
    }
    return Json::object({{"ok", Json(false)}, {"error", Json("no operation " + op)}}).dump();
  } catch (const std::exception& e) {
    return Json::object({{"ok", Json(false)}, {"error", Json(e.what())}}).dump();
  }
}

}  // namespace science::studio

#define SCIENCE_STUDIO_SYSTEM(T)                                                                                       \
  namespace {                                                                                                          \
  struct science_studio_holder { T system; bool started = false; };                                                    \
  }                                                                                                                    \
  extern "C" void* science_studio_create() { return new science_studio_holder(); }                                     \
  extern "C" void science_studio_destroy(void* self) { delete static_cast<science_studio_holder*>(self); }             \
  extern "C" char* science_studio_call(void* self, const char* request) {                                              \
    auto* h = static_cast<science_studio_holder*>(self);                                                               \
    const std::string answer = science::studio::call(h->system, h->started, request);                                  \
    char* out = static_cast<char*>(std::malloc(answer.size() + 1));                                                    \
    std::memcpy(out, answer.c_str(), answer.size() + 1);                                                               \
    return out;                                                                                                        \
  }                                                                                                                    \
  extern "C" void science_studio_free(char* text) { std::free(text); }
