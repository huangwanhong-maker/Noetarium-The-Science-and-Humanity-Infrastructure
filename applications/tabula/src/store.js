// Where boards are kept: this work's server, under data/cache/tabula/ (serve.py). Each call answers
// with what was asked for or throws with the server's reason.

async function ask(path, options = {}) {
  let response;
  try {
    response = await fetch(path, options);
  } catch {
    throw new Error('the server did not answer');
  }
  const body = await response.json().catch(() => null);
  if (!response.ok) throw new Error(body?.message || `the server answered ${response.status}`);
  return body;
}

const files = new Map();

export const store = {
  list: async () => (await ask('/api/boards')).boards,
  open: (id) => ask(`/api/boards/${encodeURIComponent(id)}`),
  save: (board) => ask(`/api/boards/${encodeURIComponent(board.id)}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(board) }),
  aside: (id) => ask(`/api/boards/${encodeURIComponent(id)}`, { method: 'DELETE' }),
  image: async (file) => (await ask('/api/files', { method: 'POST', headers: { 'Content-Type': file.type }, body: file })).src,
  plugins: async () => (await ask('/api/plugins')).plugins,
  // The modules of the plug-ins, each described: what it is and the parameters it needs.
  modules: async () => (await ask('/api/modules')).modules,
  // A file of data kept beside the boards, for a module to read: its reference.
  keep: async (file) => (await ask(`/api/files?name=${encodeURIComponent(file.name)}`, { method: 'POST', body: file })).ref,
  // A file of the data library's cache, read once: a path within it, never an address elsewhere.
  data(path) {
    if (!/^[A-Za-z0-9][A-Za-z0-9._/-]*$/.test(path) || path.includes('..')) return Promise.reject(new Error(`'${path}' is not a path within the data library`));
    if (!files.has(path)) files.set(path, fetch(`/data/${path}`).then((r) => { if (!r.ok) throw new Error(`the data library has no ${path}`); return r.json(); }).catch((error) => { files.delete(path); throw error; }));
    return files.get(path);
  },
  // A module run by the server: its results when it is done; its progress told on the way.
  async run(request, progress = () => {}) {
    const { job } = await ask('/api/run', { method: 'POST', headers: { 'Content-Type': 'application/json', 'X-Tabula': 'run' }, body: JSON.stringify(request) });
    for (;;) {
      await new Promise((r) => setTimeout(r, 220));
      const j = await ask(`/api/job/${job}`);
      if (j.state === 'done') return j.results || [];
      if (j.state !== 'running') throw new Error(j.message || `the run was ${j.state}`);
      progress(j.progress || 0, j.text || '');
    }
  },
};
