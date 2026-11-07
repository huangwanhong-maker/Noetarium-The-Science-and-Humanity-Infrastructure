// The kernels as the controller sees them (the design note of 2026-11-07, §8.5, §8.7): one interface (load, tick, unload) whatever the
// language. JavaScript runs in the project's worker; Python and C++ in the server's kernels, kept running, where a script is loaded
// (compiled, for C++) when it is created or saved, and joins the schedule at once.

const HEADERS = { 'Content-Type': 'application/json', 'X-Workbench': 'studio' };

export function createKernels({ project }) {
  const worker = new Worker('/src/studio/worker.js', { type: 'module' });
  let calls = 0;
  const pending = new Map();
  worker.onmessage = ({ data }) => {
    const p = pending.get(data.call);
    if (!p) return;
    pending.delete(data.call);
    if (data.ok) p.resolve(data.result);
    else p.reject(Object.assign(new Error(data.error), { line: data.line, log: data.log || [] }));
  };
  worker.onerror = (e) => { for (const p of pending.values()) p.reject(new Error(`the worker failed: ${e.message}`)); pending.clear(); };
  const js = (op, payload) => new Promise((resolve, reject) => {
    const call = ++calls;
    pending.set(call, { resolve, reject });
    worker.postMessage({ call, op, ...payload });
  });
  const server = async (language, op, body) => {
    const r = await fetch(`/kernels/${language}/${op}`, { method: 'POST', priority: 'high', headers: HEADERS, body: JSON.stringify({ project: project(), ...body }) });
    const a = await r.json().catch(() => ({ message: `the server answered ${r.status}` }));
    if (!r.ok) throw Object.assign(new Error(a.message || `the server answered ${r.status}`), { line: a.line ?? null, log: a.log || [] });
    return a;
  };
  return {
    load: (s, source) => (s.language === 'js' ? js('load', { id: s.id, source }) : server(s.language, 'load', { id: s.id, file: s.file })),
    tick: (s, request) => (s.language === 'js' ? js('tick', { id: s.id, ...request }) : server(s.language, 'tick', { id: s.id, ...request })),
    unload: (s) => (s.language === 'js' ? js('unload', { id: s.id }) : server(s.language, 'unload', { id: s.id }).catch(() => ({}))),
    terminate: () => worker.terminate(),
  };
}
