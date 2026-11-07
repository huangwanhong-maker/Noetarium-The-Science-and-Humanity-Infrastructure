// The controller of a project (the design note of 2026-11-07, §6.4): the transport (play, pause, step, rate), the clock and its fixed
// ticks, the schedule (visualization/scene/schedule.js) recomputed whenever a system joins, leaves or changes what it reads and writes,
// each system's life (loading, compiling, running, paused, stopped, failed), and the commit of what the systems wrote and drew.
//
// A tick runs as a pipeline: the systems of the tick phase one after another in the schedule's order, each given the values of what it
// reads (a writer having run before it) and of what it reads as previous (the values at the end of the last tick), its writes applied
// as it answers, contributions to a merged resource combined after the tick. The marks of the tick are drawn together at its end, so
// that a frame never shows half a tick. Frames go on drawing while a tick waits on a kernel; a tick owed while one is running is not
// queued but counted as lag, so that a slow kernel slows the simulation, never the page.
//
// A script created while the project plays joins at the next tick: it is loaded (compiled, for C++), its declaration read, the schedule
// made again; until then the others run. Saved again, it is reloaded in place, its state kept where its save() and load() allow.

import { plan, mergeAll, MERGES } from '/visualization/scene/index.js';

export const STATES = { loading: 'loading', compiling: 'compiling', ready: 'running', paused: 'paused', stopped: 'stopped', error: 'failed' };

export function createController({ project, kernels, scene, range = null, frames = () => ({}), onFrameMarks = () => {}, onChange = () => {}, say = () => {} }) {
  const C = {
    playing: false, rate: 1, tick: 0, acc: 0, busy: false, lag: 0, last: null, tickMs: 0,
    time_ms: project().scene.clock.start_ms,
    systems: new Map(),
    values: new Map(), previous: new Map(),
    plan: { phases: { tick: [], update: [] }, refusals: [] },
  };
  const P = () => project();
  const tickHz = () => P().scene.clock.tick_hz || 10;
  const resourceOf = (name) => (P().resources || []).find((r) => r.name === name);
  const mergeOf = (name) => resourceOf(name)?.merge || null;
  // What a script is given of the world: the world frame's range in space (and the scene's moment, as ctx.time_ms), and the frames.
  const view = () => (range ? range() : scene.camera());

  // The resources' first values, those of the project and those a script declares.
  function seedResources() {
    for (const r of P().resources || []) if (!C.values.has(r.name)) { C.values.set(r.name, r.value ?? r.initial ?? null); C.previous.set(r.name, C.values.get(r.name)); }
  }
  function declareResources(id, declared) {
    let added = false;
    for (const [name, spec] of Object.entries(declared || {})) {
      if (resourceOf(name)) continue;
      P().resources.push({ name, type: spec.type || 'number', merge: spec.merge && MERGES[spec.merge] ? spec.merge : undefined, initial: spec.initial ?? null, declaredBy: id, saved: true });
      added = true;
    }
    if (added) seedResources();
    return added;
  }

  function replan() {
    const live = [...C.systems.values()].filter((s) => s.describe && ['ready', 'paused'].includes(s.status));
    const order = (P().scripts || []).map((s) => s.id);
    C.plan = plan(live.map((s) => ({ id: s.id, phase: s.describe.phase, reads: s.describe.reads, writes: s.describe.writes, previous: s.describe.previous, after: s.describe.after, rank: order.indexOf(s.id) })),
      Object.fromEntries((P().resources || []).map((r) => [r.name, r])));
    const refused = new Set(C.plan.refusals.flatMap((r) => r.systems));
    for (const s of C.systems.values()) s.refused = refused.has(s.id) ? C.plan.refusals.find((r) => r.systems.includes(s.id)).message : null;
    onChange('plan');
  }

  const entryOf = (script) => {
    if (!C.systems.has(script.id)) C.systems.set(script.id, { id: script.id, language: script.language, file: script.file, status: 'stopped', describe: null, error: null, line: null, ms: 0, runs: 0, log: [] });
    const s = C.systems.get(script.id);
    Object.assign(s, { language: script.language, file: script.file });
    return s;
  };

  // A script loaded, or loaded again: compiled first if it is C++.
  async function load(script, source) {
    const s = entryOf(script);
    if (source != null) s.source = source;
    const was = s.status;
    // A system already running goes on running while its new version loads or compiles; only then is it swapped.
    const live = ['ready', 'paused'].includes(was) && !!s.describe;
    const doing = script.language === 'cpp' ? 'compiling' : 'loading';
    if (live) s.pending = doing; else s.status = doing;
    s.error = null, s.line = null;
    onChange('system');
    const t0 = performance.now();
    try {
      const r = await kernels.load(s, source ?? s.source);
      s.describe = r.describe;
      s.carried = !!r.carried;
      s.loadMs = performance.now() - t0;
      s.compileS = r.compile_s ?? null;
      s.status = was === 'paused' ? 'paused' : 'ready';
      s.stale = false;
      if (script.enabled === false) s.status = 'paused';
      declareResources(s.id, r.describe.resources);
      for (const line of r.log || []) say(s.id, line);
      say(s.id, `${r.carried ? 'reloaded, its state kept' : 'loaded'}${s.compileS ? ` (compiled in ${s.compileS} s)` : ''}`, 'quiet');
    } catch (error) {
      s.error = String(error.message || error), s.line = error.line ?? null;
      for (const line of error.log || []) say(s.id, line, 'quiet');
      // A new version that does not load (or compile) leaves the one before running, as a hot reload should: the kernels keep it.
      if (live) {
        s.status = was, s.stale = true;
        say(s.id, `${s.error}${s.line ? ` (line ${s.line})` : ''}; the version before goes on running`, 'error');
      } else {
        s.status = 'error', s.stale = false;
        say(s.id, s.error + (s.line ? ` (line ${s.line})` : ''), 'error');
      }
    }
    s.pending = null;
    replan();
  }

  async function stop(id) {
    const s = C.systems.get(id);
    if (!s) return;
    s.status = 'stopped';
    scene.clear(`script:${id}`);
    try { await kernels.unload(s); } catch { /* already gone */ }
    replan();
  }
  function pause(id) { const s = C.systems.get(id); if (s && s.status === 'ready') { s.status = 'paused'; replan(); } }
  function resume(id) { const s = C.systems.get(id); if (s && s.status === 'paused') { s.status = 'ready'; replan(); } }
  async function remove(id) { await stop(id); C.systems.delete(id); replan(); }

  const runnable = (id) => { const s = C.systems.get(id); return s && s.status === 'ready' && !s.refused; };
  const paramsOf = (id) => {
    const script = (P().scripts || []).find((x) => x.id === id), s = C.systems.get(id);
    const out = {};
    for (const p of s?.describe?.params || []) out[p.name] = script?.params?.[p.name] ?? p.default;
    return out;
  };

  async function runPhase(phase, dt) {
    const n = C.tick + 1, t = C.time_ms;
    const order = (C.plan.phases[phase] || []).filter(runnable);
    const contributions = new Map(), marks = new Map();
    const readValue = (name) => (contributions.has(name) ? mergeAll(mergeOf(name), contributions.get(name)) : C.values.get(name));
    const t0 = performance.now();
    const v = order.length ? view() : null, fr = order.length ? frames() : null;
    for (const id of order) {
      const s = C.systems.get(id);
      if (!s || !runnable(id)) continue;
      const d = s.describe;
      const values = Object.fromEntries((d.reads || []).map((k) => [k, readValue(k) ?? null]));
      const previous = Object.fromEntries((d.previous || []).map((k) => [k, C.previous.get(k) ?? null]));
      const t1 = performance.now();
      try {
        const r = await kernels.tick(s, { tick: n, dt, time_ms: t, params: paramsOf(id), view: v, frames: fr, values, previous });
        s.ms = performance.now() - t1;
        s.runs++;
        for (const [k, v] of Object.entries(r.writes || {})) C.values.set(k, v);
        for (const [k, list] of Object.entries(r.adds || {})) { if (!contributions.has(k)) contributions.set(k, []); contributions.get(k).push(...list); }
        marks.set(id, r.marks || []);
        for (const line of r.log || []) say(id, line);
      } catch (error) {
        const message = String(error.message || error);
        if (/is loaded|stopped \(a script crashed/.test(message) && !s.reloading) {
          // A kernel started again has forgotten its scripts: load this one anew, and say so.
          s.reloading = true;
          say(id, `${message}; loading it again`, 'error');
          const script = (P().scripts || []).find((x) => x.id === id);
          if (script) await load(script, null);
          s.reloading = false;
        } else {
          s.status = 'error', s.stale = false, s.error = message, s.line = error.line ?? null;
          for (const line of error.log || []) say(id, line, 'quiet');
          say(id, message + (s.line ? ` (line ${s.line})` : ''), 'error');
          replan();
        }
      }
    }
    for (const [k, list] of contributions) C.values.set(k, mergeAll(mergeOf(k), list));
    // Marks on the world go to the script's layer; charts and drawings to the frames they name.
    for (const [id, m] of marks) {
      const layer = (P().layers || []).find((l) => l.script === id);
      const world = m.filter((x) => ['points', 'columns', 'lines'].includes(x.kind)), framed = m.filter((x) => x.kind === 'chart' || x.kind === 'drawing');
      if (world.length || layer) scene.draw(`script:${id}`, world, layer ? { visible: layer.visible !== false, opacity: layer.opacity ?? 1 } : {});
      if (framed.length) onFrameMarks(id, framed);
    }
    return performance.now() - t0;
  }

  async function runTick(dt) {
    C.busy = true;
    try {
      C.tickMs = await runPhase('tick', dt);
      C.previous = new Map(C.values);
      C.tick += 1;
    } finally {
      C.busy = false;
      onChange('tick');
    }
    // A tick owed when this one ends follows at once, so that the rate of ticks does not depend on the rate of frames.
    const step = 1 / tickHz();
    if (C.playing && C.acc >= step) { C.acc -= step; setTimeout(() => { if (!C.busy) runTick(step); }, 0); }
  }

  let updating = false;
  async function runUpdate(dt) {
    if (updating || !(C.plan.phases.update || []).some(runnable)) return;
    updating = true;
    try { await runPhase('update', dt); } finally { updating = false; }
  }

  function frame(now) {
    const dt = C.last == null ? 0 : Math.min(0.25, (now - C.last) / 1000);
    C.last = now;
    if (!C.playing) return;
    C.time_ms += dt * 1000 * C.rate;
    scene.setTime(C.time_ms);
    C.acc += dt * C.rate;
    const step = 1 / tickHz();
    if (C.acc >= step) {
      if (!C.busy) { C.acc -= step; runTick(step); }
      // Ticks owed beyond a few are dropped and counted: the kernels set the pace, not the page.
      if (C.acc > step * 4) { C.lag += Math.floor(C.acc / step); C.acc = 0; }
    }
    runUpdate(dt);
  }

  return {
    state: C,
    load, stop, pause, resume, remove, replan,
    play() { C.playing = true; C.last = null; C.lag = 0; onChange('transport'); },
    hold() { C.playing = false; onChange('transport'); },
    async step() { if (!C.busy) { const s = 1 / tickHz(); C.time_ms += s * 1000; scene.setTime(C.time_ms); await runTick(s); } },
    setRate(r) { C.rate = r; onChange('transport'); },
    setValue(name, v) { C.values.set(name, v); C.previous.set(name, v); const r = resourceOf(name); if (r) r.value = v; onChange('value'); },
    reset() {
      C.tick = 0, C.lag = 0, C.acc = 0, C.time_ms = P().scene.clock.start_ms;
      C.values.clear(); C.previous.clear(); seedResources();
      onChange('transport');
    },
    seed: seedResources,
    frame,
  };
}
