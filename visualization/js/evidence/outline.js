// A matter as an outline (ADR 0121): the same argument as matter.js draws, unfolded from a question into a tree, for a matter too
// large to read as a drawing. A question is given by rules; a rule asks for facts and consequences, may hold only under a reading
// of an open term, and may be defeated by exceptions; a consequence is given by rules in its turn; a fact is where it ends. What
// stands under two rules is unfolded once and named again where it recurs. Pure: it reads matter.js's graph.
//
//   matterTree(graph, 'a:transfer_prohibited') → { key, node, kind: 'atom' | 'rule' | 'exception' | 'counts', children, readings,
//                                                 negated, repeat, stands, state }
//
// `stands` says whether the node holds in the argument the engine found (a fact by its standing, a consequence by a rule that gives
// it and is not defeated); `state` says what is still in play beneath it: 'open' (an alleged fact or a term not fixed is beneath),
// 'given' (everything beneath stands), 'absent' (it rests only on facts the record does not mention) or 'failed' (decided against).

const STANDS = new Set(['final', 'found', 'admitted', 'noticed', 'deemed', 'proved', 'presumed']);
const AGAINST = new Set(['not proved', 'rebutted']);

export function matterTree(graph, rootId) {
  const byId = new Map(graph.nodes.map((n) => [n.id, n]));
  const into = new Map(graph.nodes.map((n) => [n.id, []]));
  for (const e of graph.edges) into.get(e.to).push(e);
  const unfolded = new Set();
  const build = (id, path, extra = {}) => {
    const node = byId.get(id);
    const key = `${path}/${id}`;
    const out = { key, node, kind: node.kind === 'atom' ? 'atom' : node.kind, children: [], readings: [], negated: !!extra.negated, repeat: false };
    if (node.kind === 'atom' && node.role !== 'fact') {
      if (unfolded.has(id) || path.split('/').includes(id)) { out.repeat = true; return finish(out); }
      unfolded.add(id);
    }
    const edges = into.get(id) || [];
    if (node.kind === 'atom') {
      for (const e of edges) if (e.kind === 'gives') out.children.push(build(e.from, key));
    } else {
      for (const e of edges) if (e.kind === 'reading') {
        const t = byId.get(e.from);
        const r = (t.readings || []).find((x) => x.name === e.reading);
        out.readings.push({ term: t.term, reading: e.reading, fixed: !!(r && r.fixed), aside: !!(r && r.aside), gloss: r ? r.gloss : '' });
      }
      for (const e of edges) if (e.kind === 'asks') out.children.push(build(e.from, key, { negated: e.negated }));
      for (const e of edges) if (e.kind === 'defeats') out.children.push(build(e.from, key));
    }
    return finish(out);
  };
  const finish = (t) => {
    const n = t.node;
    const kids = t.children;
    if (t.kind === 'atom' && n.role === 'fact') {
      const standing = n.used || n.declared;
      t.stands = STANDS.has(standing);
      t.state = t.stands ? 'given' : AGAINST.has(standing) ? 'failed' : n.declared === null ? 'absent' : 'open';
      return t;
    }
    if (t.repeat) { t.stands = null; t.state = 'given'; return t; }
    if (t.kind === 'atom') t.stands = kids.some((k) => k.stands);
    else if (t.kind === 'exception') t.stands = !!n.applied;
    else t.stands = !!n.applied && n.holds !== false;
    const states = kids.map((k) => k.state);
    const openReading = t.readings.some((r) => !r.fixed && !r.aside);
    const deadReading = t.readings.some((r) => r.aside);
    if (deadReading) t.state = 'failed';
    else if (states.includes('open') || openReading) t.state = 'open';
    else if (t.kind === 'atom') t.state = states.includes('given') ? 'given' : states.includes('absent') ? 'absent' : states.length ? 'failed' : 'absent';
    else {
      // A rule needs all it asks for: one thing absent or failed beneath it and it is not in play.
      const asked = kids.filter((k) => k.kind === 'atom').map((k) => k.state);
      t.state = asked.includes('failed') ? 'failed' : asked.includes('absent') ? 'absent' : 'given';
    }
    return t;
  };
  return build(rootId, '');
}

// The keys of the nodes unfolded at first: the root, and every branch with something in play beneath it.
export function unfoldedAtFirst(tree) {
  const open = new Set([tree.key]);
  const walk = (t, depth) => {
    for (const k of t.children) {
      if (k.state === 'open' && k.children.length) open.add(k.key);
      if (depth < 1 && k.children.length) open.add(k.key);
      walk(k, depth + 1);
    }
  };
  walk(tree, 0);
  return open;
}

// The rows of the tree as they are shown: each visible node with its depth, in order.
export function rows(tree, unfolded) {
  const out = [];
  const walk = (t, depth) => {
    out.push({ ...t, depth, unfolded: unfolded.has(t.key) });
    if (unfolded.has(t.key)) for (const k of t.children) walk(k, depth + 1);
  };
  walk(tree, 0);
  return out;
}
