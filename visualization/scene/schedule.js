// The schedule of a project's systems (the Studio's world; the design note of 2026-11-07, §5 and §7): which systems run, in what
// order, in each phase, from what each declares it reads and writes. Pure, so that it is checked in Node and any page may run it.
//
//   system    { id, phase: 'tick' | 'update', reads: [name], writes: [name], previous: [name], after: [id], rank? }
//   resource  { name, type, merge?: 'sum' | 'min' | 'max' | 'union' }
//
// Rules:
// - In a phase, a resource has one writer, or several only if it declares a merge, a commutative monoid whose result does not
//   depend on the order its contributions are combined in.
// - A writer runs before every reader of what it writes, in that phase. A read of `previous` (the value at the end of the last
//   tick) is no edge: it is the delay through which a feedback loop may close, as in synchronous dataflow (Lustre's `pre`).
// - `after` adds an edge, to break a tie or order what shares nothing.
// - Among systems whose order nothing decides, the order is the systems' rank (their place in the project), then their id, so
//   that the schedule is the same every time.
// - What breaks a rule is refused with its reason, and the systems it involves are left out of the run; the others run.

export const PHASES = ['tick', 'update'];

// Merges: commutative monoids on numbers and on arrays of numbers (elementwise), and union on arrays (as sets).
// A number met with an array is broadcast over it, so that contributions of either kind combine in any order to the same result.
const elementwise = (f) => (a, b) => {
  if (Array.isArray(a) && Array.isArray(b)) return a.map((x, i) => f(x, b[i] ?? x)).concat(b.slice(a.length));
  if (Array.isArray(a)) return a.map((x) => f(x, b));
  if (Array.isArray(b)) return b.map((y) => f(a, y));
  return f(a, b);
};
export const MERGES = {
  sum: { identity: 0, combine: elementwise((a, b) => a + b) },
  min: { identity: Infinity, combine: elementwise((a, b) => Math.min(a, b)) },
  max: { identity: -Infinity, combine: elementwise((a, b) => Math.max(a, b)) },
  union: { identity: [], combine: (a, b) => [...new Set([...(a || []), ...(b || [])])] },
};

// The contributions of several writers, combined in the merge's monoid.
export function mergeAll(kind, contributions) {
  const m = MERGES[kind];
  if (!m) throw new Error(`no merge "${kind}": a merge is one of ${Object.keys(MERGES).join(', ')}`);
  return contributions.reduce((acc, c) => (acc === undefined ? c : m.combine(acc, c)), undefined) ?? m.identity;
}

export function plan(systems, resources = {}) {
  const refusals = [];
  const phases = {};
  const resource = (name) => resources[name] || null;
  for (const phase of PHASES) {
    const here = systems.filter((s) => (s.phase || 'tick') === phase).map((s, k) => ({ ...s, rank: s.rank ?? k }));
    const out = new Set();
    // Writers of each resource, and the rule of one writer unless a merge is declared.
    const writers = new Map();
    for (const s of here) for (const name of s.writes || []) {
      if (!writers.has(name)) writers.set(name, []);
      writers.get(name).push(s.id);
    }
    for (const [name, ids] of writers) {
      if (ids.length > 1 && !(resource(name)?.merge && MERGES[resource(name).merge])) {
        refusals.push({ kind: 'writers', phase, resource: name, systems: [...ids], message: `"${name}" has ${ids.length} writers in the ${phase} (${ids.join(', ')}): give it a merge (sum, min, max, union), or let one system write it` });
        for (const id of ids) out.add(id);
      }
    }
    // Edges: writer → reader of the same tick, and the hints.
    const edges = new Map(here.map((s) => [s.id, new Set()]));
    for (const s of here) {
      for (const name of s.reads || []) for (const w of writers.get(name) || []) if (w !== s.id) edges.get(w).add(s.id);
      for (const a of s.after || []) if (edges.has(a) && a !== s.id) edges.get(a).add(s.id);
    }
    // A system that reads what it writes (not as previous) is a loop of one.
    for (const s of here) {
      const self = (s.reads || []).filter((n) => (s.writes || []).includes(n));
      if (self.length) {
        refusals.push({ kind: 'cycle', phase, systems: [s.id], message: `${s.id} reads what it writes (${self.join(', ')}) in the same tick: read it as previous` });
        out.add(s.id);
      }
    }
    // Cycles among systems, found by Tarjan's strongly connected components.
    for (const comp of components(here.map((s) => s.id), edges)) {
      if (comp.length < 2) continue;
      refusals.push({ kind: 'cycle', phase, systems: comp, message: `a loop with no delay: ${comp.join(' → ')} → ${comp[0]}. One of them must read the other's output as previous` });
      for (const id of comp) out.add(id);
    }
    // Kahn's order over what is left, ties broken by rank then id.
    const left = here.filter((s) => !out.has(s.id));
    const ids = new Set(left.map((s) => s.id));
    const indegree = new Map(left.map((s) => [s.id, 0]));
    for (const s of left) for (const t of edges.get(s.id)) if (ids.has(t)) indegree.set(t, indegree.get(t) + 1);
    const byId = new Map(left.map((s) => [s.id, s]));
    const key = (id) => [byId.get(id).rank, id];
    const less = (a, b) => { const [ra, ia] = key(a), [rb, ib] = key(b); return ra - rb || (ia < ib ? -1 : ia > ib ? 1 : 0); };
    const ready = left.filter((s) => indegree.get(s.id) === 0).map((s) => s.id).sort(less);
    const order = [];
    while (ready.length) {
      const id = ready.shift();
      order.push(id);
      for (const t of [...edges.get(id)].filter((x) => ids.has(x)).sort(less)) {
        indegree.set(t, indegree.get(t) - 1);
        if (indegree.get(t) === 0) { ready.push(t); ready.sort(less); }
      }
    }
    phases[phase] = order;
  }
  return { phases, refusals };
}

// Strongly connected components, in the order Tarjan's algorithm closes them.
function components(nodes, edges) {
  let index = 0;
  const idx = new Map(), low = new Map(), on = new Set(), stack = [], out = [];
  const visit = (v) => {
    idx.set(v, index); low.set(v, index); index++;
    stack.push(v); on.add(v);
    for (const w of edges.get(v) || []) {
      if (!idx.has(w)) { visit(w); low.set(v, Math.min(low.get(v), low.get(w))); } else if (on.has(w)) low.set(v, Math.min(low.get(v), idx.get(w)));
    }
    if (low.get(v) === idx.get(v)) {
      const comp = [];
      let w;
      do { w = stack.pop(); on.delete(w); comp.push(w); } while (w !== v);
      out.push(comp.reverse());
    }
  };
  for (const v of nodes) if (!idx.has(v)) visit(v);
  return out;
}
