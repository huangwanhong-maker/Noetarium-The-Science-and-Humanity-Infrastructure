// The project's worker: its scripts in JavaScript run here, never in the page, so that a script reaches only what the controller
// gives it (its reads, its parameters, the context), and the page's authority is never its own (the design note, §8.5).

import { createRuntime } from './runtime.js';

const runtime = createRuntime({
  importModule: async (source) => {
    const url = URL.createObjectURL(new Blob([source], { type: 'text/javascript' }));
    try { return await import(url); } finally { URL.revokeObjectURL(url); }
  },
});

self.onmessage = async ({ data }) => {
  const { call, op, id } = data;
  try {
    let result;
    if (op === 'load') result = await runtime.load(id, data.source);
    else if (op === 'tick') result = runtime.tick(id, data);
    else if (op === 'unload') result = runtime.unload(id);
    else throw new Error(`no operation ${op}`);
    self.postMessage({ call, ok: true, result });
  } catch (error) {
    self.postMessage({ call, ok: false, error: String(error.message || error), line: error.line ?? null, log: error.log || [] });
  }
};
