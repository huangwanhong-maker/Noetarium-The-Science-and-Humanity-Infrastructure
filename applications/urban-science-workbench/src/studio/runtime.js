// The Studio's runtime for scripts in JavaScript (the design note of 2026-11-07, §8): the same protocol as the Python and C++
// kernels (load, tick, unload), so that the controller schedules every language alike. Pure: it runs in the project's worker
// (worker.js) and in Node's checks. A script is a module whose default export, given the API, answers a system:
//
//   export default (api) => ({
//     title, phase: 'tick' | 'update', reads: [], writes: [], previous: [], after: [], params: [], resources: {},
//     start(ctx) {}, tick(ctx, io) {}, draw(ctx, io) { return [marks] }, save() { return state }, load(state) {},
//   });
//
// io.get(name) the value of this tick; io.prev(name) the value at the end of the last; io.set(name, v) to write; io.add(name, v)
// to contribute to a merged resource; io.log(...). What a script draws it declares as marks: on the world (api.points, api.columns,
// api.lines, on WGS 84 in degrees and metres), or in a frame of the canvas (api.chart, api.drawing); the page draws them. ctx.view is
// the world frame's range in space and time; ctx.frames the canvas's frames and their sizes.

export function createApi() {
  const num = (v) => (Array.isArray(v) || ArrayBuffer.isView(v) ? Array.from(v, Number) : v);
  return {
    param: (name, label, value, o = {}) => ({ name, label, default: value, kind: o.kind || (typeof value === 'boolean' ? 'boolean' : typeof value === 'string' ? 'text' : 'number'), min: o.min, max: o.max, step: o.step }),
    points: (title, { lon, lat, height = 0, value = null, size = 6, colour = null, map = 'plum', domain = null }) =>
      ({ kind: 'points', title, lon: num(lon), lat: num(lat), height: num(height), value: num(value), size, colour, map, domain }),
    columns: (title, { lon, lat, value, radius = 60, top = 300, map = 'plum', domain = null }) =>
      ({ kind: 'columns', title, lon: num(lon), lat: num(lat), value: num(value), radius, top, map, domain }),
    lines: (title, { paths, height = 0, colour = '#EE8FBD', value = null, map = 'plum', domain = null }) =>
      ({ kind: 'lines', title, paths, height, colour, value: num(value), map, domain }),
    // A statistical figure for a frame of the canvas: 'line', 'bars' or 'scatter', of x against y, or several series [{name, x, y}].
    chart: (frame, { chart = 'line', x = null, y = null, series = null, title = '', xlabel = '', ylabel = '' } = {}) =>
      ({ kind: 'chart', frame, chart, x: num(x), y: num(y), series, title, xlabel, ylabel }),
    // A drawing of one's own for a frame: items in the frame's pixels, each { type: 'line' | 'circle' | 'rect' | 'text' | 'path', ... }.
    drawing: (frame, items) => ({ kind: 'drawing', frame, items }),
  };
}

// The declaration of a system, as the scheduler and the inspector read it.
export function describe(s) {
  return {
    title: String(s.title || 'A system'), phase: s.phase === 'update' ? 'update' : 'tick',
    reads: [...(s.reads || [])], writes: [...(s.writes || [])], previous: [...(s.previous || [])], after: [...(s.after || [])],
    params: [...(s.params || [])], resources: { ...(s.resources || {}) },
  };
}

// The line of a script's own source where an error arose, if the stack says.
function lineOf(error, tag) {
  const m = new RegExp(`${tag}:(\\d+)`).exec(String(error && error.stack));
  return m ? Number(m[1]) : null;
}

export function createRuntime({ importModule }) {
  const api = createApi();
  const systems = new Map();   // id → { system, started, state }

  async function load(id, source) {
    const tag = `studio-${id.replace(/[^A-Za-z0-9_-]/g, '_')}`;
    let mod;
    try {
      mod = await importModule(`${source}\n//# sourceURL=${tag}`);
    } catch (error) {
      throw Object.assign(new Error(`${error.message || error}`), { line: lineOf(error, tag) });
    }
    const make = mod.default;
    if (typeof make !== 'function') throw new Error('a script exports, as its default, a function of the API that answers its system');
    const system = make(api);
    if (!system || typeof system !== 'object') throw new Error('the script\'s function answered no system');
    // A script loaded again keeps its state where it can.
    const old = systems.get(id);
    let carried = false;
    if (old && typeof old.system.save === 'function' && typeof system.load === 'function') {
      try { system.load(old.system.save()); carried = true; } catch { carried = false; }
    }
    systems.set(id, { system, started: carried, tag });
    return { describe: describe(system), carried };
  }

  function tick(id, { tick: n = 0, dt = 0, time_ms = 0, params = {}, view = null, frames = {}, values = {}, previous = {} } = {}) {
    const entry = systems.get(id);
    if (!entry) throw new Error(`no system ${id} is loaded`);
    const writes = {}, adds = {}, log = [];
    const io = {
      get: (name) => values[name],
      prev: (name) => previous[name],
      set: (name, v) => { writes[name] = v; },
      add: (name, v) => { (adds[name] ||= []).push(v); },
      log: (...a) => { log.push(a.map((x) => (typeof x === 'string' ? x : JSON.stringify(x))).join(' ')); },
    };
    const ctx = { tick: n, dt, time_ms, params, view, frames };
    try {
      if (!entry.started) { entry.started = true; entry.system.start?.(ctx, io); }
      entry.system.tick?.(ctx, io);
      const marks = entry.system.draw ? entry.system.draw(ctx, io) || [] : [];
      return { writes, adds, marks, log };
    } catch (error) {
      throw Object.assign(new Error(String(error.message || error)), { line: lineOf(error, entry.tag), log });
    }
  }

  const unload = (id) => { systems.delete(id); return {}; };
  return { load, tick, unload, has: (id) => systems.has(id) };
}
