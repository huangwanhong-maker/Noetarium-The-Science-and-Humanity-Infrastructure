// The graph (ADR 0119): a project's linked data drawn as the page. A node chosen opens where it stands: its classes, what is said
// of it (its literals) and its links each way, and a ring of actions (follow it, say something of it, take back a statement, open
// the holding it is). Beneath the graph, a desk with four leaves: a query in SPARQL whose bindings are marked on the graph; the
// shapes of a SHACL file, whose failures are marked and listed with their constraints; entailment by RDFS and part of OWL 2 RL,
// whose conclusions are drawn dashed, each with its rule; and the text of each file (Turtle, TriG, N-Triples, N-Quads or JSON-LD),
// coloured, kept only when it reads (a file that does not is refused with its line and column, and the cursor put there). When
// someone else changed the file since it was read, what this page added and removed is merged with theirs by statements (ADR 0120).
// The whole graph may be taken away as TriG, N-Quads or JSON-LD.

import * as L from '/visualization/js/linked/index.js';

const EXAMPLE_QUERY = `PREFIX dcterms: <http://purl.org/dc/terms/>
SELECT ?holding ?title ?creator WHERE {
  ?holding dcterms:title ?title .
  OPTIONAL { ?holding dcterms:creator ?creator }
}
ORDER BY ?title`;

export function graphView() {
  const state = { files: null, focus: null, depth: 2, leaf: null, marked: new Set(), inferred: [], query: EXAMPLE_QUERY, result: null, report: null, edit: null, text: '', error: null, open: null };
  return {
    id: 'graph', label: 'Graph', draw: (pane, ctx) => draw(pane, ctx, state),
    // Another's change to the linked data is drawn at once, unless one is writing in the desk.
    heard: (e, ctx) => { if (document.querySelector('.tab.on')?.dataset.tab === 'graph' && state.leaf !== 'text' && /linked|holding|arranged|codebook|took|handed|merged|edited/.test(e.what)) ctx.show('graph'); },
  };
}

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('graph');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  const base = `/api/projects/${encodeURIComponent(ctx.name)}`;
  if (window.archiveFocus) { state.focus = L.key({ kind: 'iri', value: window.archiveFocus }); state.open = state.focus; window.archiveFocus = null; }
  const { files } = await ctx.api(`${base}/linked`);
  // Shapes are not data: a file whose name says it holds shapes is left out of the drawing until it is chosen.
  if (!state.files) state.files = new Set(files.map((f) => f.file).filter((f) => !/shape/i.test(f)));
  const chosen = files.filter((f) => state.files.has(f.file)).map((f) => f.file);
  let triples = [];
  try {
    triples = chosen.length ? (await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'query', files: chosen, query: 'CONSTRUCT { ?s ?p ?o } WHERE { ?s ?p ?o }' } })).triples : [];
  } catch (error) {
    return fill(pane, el('p', { class: 'note wrong' }, `The linked data could not be read: ${error.message}`));
  }
  const all = [...triples, ...state.inferred.map((x) => x.statement)];
  state.named = L.names(all);
  let model = L.graphModel(all);
  if (state.focus && model.nodes.some((n) => n.key === state.focus)) model = L.neighbourhood(model, state.focus, state.depth);
  const big = model.nodes.length > 400;
  if (big) model = { nodes: model.nodes.slice(0, 400), edges: model.edges.filter((e) => model.nodes.slice(0, 400).some((n) => n.key === e.from) && model.nodes.slice(0, 400).some((n) => n.key === e.to)) };
  ctx.note(`${triples.length} statements in ${chosen.length} of ${files.length} files${state.focus ? ', about one node' : ''}${big ? ', the first 400 nodes drawn' : ''}`);
  const width = Math.max(640, pane.clientWidth - 24), height = Math.max(420, Math.round(pane.clientHeight * (state.leaf ? 0.56 : 0.86)));
  const positions = L.layoutGraph(model, { width, height, seed: 11 });
  const inferredKeys = new Set(state.inferred.map((x) => `${L.key(x.statement[0])}>${L.key(x.statement[2])}`));
  const inferred = new Set(model.edges.map((e, i) => (inferredKeys.has(`${e.from}>${e.to}`) ? `e${i}` : null)).filter(Boolean));
  const canvas = el('div', { class: 'canvas' });
  canvas.innerHTML = L.svgGraph(model, positions, { width, height, selected: state.open, marked: state.marked, inferred, title: 'The project\'s linked data' });
  const strip = el('nav', { class: 'files', 'aria-label': 'Files of linked data' },
    files.map((f) => el('button', { type: 'button', class: `chip${state.files.has(f.file) ? ' on' : ''}`, title: `${f.format}, ${f.bytes} bytes`,
      onclick: () => { state.files.has(f.file) ? state.files.delete(f.file) : state.files.add(f.file); ctx.show('graph'); } }, f.file)),
    el('span', { class: 'spacer' }),
    state.focus ? el('button', { type: 'button', class: 'key edged', onclick: () => { state.focus = null; ctx.show('graph'); } }, 'The whole graph') : null,
    el('span', { class: 'exports' }, 'Take away as ', [['trig', 'TriG'], ['nq', 'N-Quads'], ['jsonld', 'JSON-LD']].map(([f, w]) =>
      el('a', { class: 'key', href: ctx.keyed(`${base}/export.${f}`), download: `${ctx.name}.${f}` }, w))),
    [['query', 'Query'], ['shapes', 'Shapes'], ['entail', 'Entail'], ['text', 'Text']].map(([leaf, word]) =>
      el('button', { type: 'button', class: `key${state.leaf === leaf ? ' on' : ''}`, onclick: () => { state.leaf = state.leaf === leaf ? null : leaf; ctx.show('graph'); } }, word)));
  const desk = state.leaf ? el('section', { class: 'desk' }) : null;
  fill(pane, strip, canvas, desk);
  if (!files.length) canvas.append(el('p', { class: 'note over' }, 'The project has no linked data yet. Add holdings in the archive (each is described here), take studies from the catalogue, or write a file under Text.'));
  // A node opens where it stands.
  for (const g of canvas.querySelectorAll('.l-node')) g.addEventListener('click', () => { state.open = g.dataset.node; ctx.show('graph'); });
  if (state.open) {
    const n = model.nodes.find((x) => x.key === state.open);
    if (n) card(ctx, state, canvas, n, positions.get(n.key), width, all, base);
  }
  if (desk) await deskOf(ctx, state, desk, base, files, chosen);
}

function card(ctx, state, canvas, n, p, width, all, base) {
  const { el } = ctx;
  const out = all.filter((q) => L.key(q[0]) === n.key), inn = all.filter((q) => L.key(q[2]) === n.key);
  const isInferred = (q) => state.inferred.some((x) => L.key(x.statement[0]) === L.key(q[0]) && x.statement[1].value === q[1].value && L.key(x.statement[2]) === L.key(q[2]));
  const line = (q, other) => el('li', { class: isInferred(q) ? 'inferred' : '' }, el('span', { class: 'p' }, L.label(q[1])), ' ', el('span', {}, L.label(other, undefined, state.named)),
    !isInferred(q) && q.length < 4 ? el('button', { type: 'button', class: 'x', title: 'Take this statement back', onclick: () => takeBack(ctx, state, base, q) }, '×') : null);
  const left = Math.min(Math.max(8, p.x + 16), width - 330);
  const adding = el('form', { class: 'say' }, el('input', { type: 'text', class: 'field', name: 'p', placeholder: 'dcterms:subject' }), el('input', { type: 'text', class: 'field', name: 'o', placeholder: '"a value" or prefix:name or <iri>' }),
    el('button', { type: 'submit', class: 'key main' }, 'Say'));
  adding.addEventListener('submit', (e) => { e.preventDefault(); say(ctx, state, base, n.term, adding.p.value, adding.o.value).catch((err) => ctx.say(err.message)); });
  const holding = n.term.kind === 'iri' && n.term.value.includes(':holding:');
  canvas.append(el('article', { class: 'node-card', style: { left: `${left}px`, top: `${Math.max(8, p.y - 30)}px` } },
    el('header', {}, el('h3', {}, n.label), el('button', { type: 'button', class: 'x', title: 'Close', onclick: () => { state.open = null; ctx.show('graph'); } }, '×')),
    n.term.kind === 'iri' ? el('p', { class: 'iri' }, n.term.value) : null,
    n.classes.length ? el('p', { class: 'kinds' }, n.classes.map((c) => L.brief(c.value)).join(' · ')) : null,
    el('nav', { class: 'ring' },
      el('button', { type: 'button', class: 'key', onclick: () => { state.focus = n.key; ctx.show('graph'); } }, 'Follow'),
      holding ? el('button', { type: 'button', class: 'key', onclick: () => { ctx.show('archive'); } }, 'Open in the archive') : null,
      el('button', { type: 'button', class: 'key', onclick: () => navigator.clipboard?.writeText(n.term.value) }, 'Copy')),
    out.length ? el('ul', { class: 'said' }, out.map((q) => line(q, q[2]))) : null,
    inn.length ? el('details', {}, el('summary', {}, `${inn.length} pointing here`), el('ul', { class: 'said' }, inn.map((q) => line(q, q[0])))) : null,
    n.term.kind !== 'literal' ? adding : null));
}

// The file a statement is in: the first file chosen whose own statements hold it.
async function fileOf(ctx, base, files, q) {
  for (const f of files) {
    const r = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'query', files: [f], query: `ASK { ${nt(q[0])} ${nt(q[1])} ${nt(q[2])} }` } });
    if (r.truth) return f;
  }
  return null;
}

const nt = (t) => (t.kind === 'iri' ? `<${t.value}>` : t.kind === 'blank' ? `_:${t.value}` : `"${t.value.replace(/\\/g, '\\\\').replace(/"/g, '\\"').replace(/\n/g, '\\n')}"${t.language ? `@${t.language}` : t.datatype && !t.datatype.endsWith('#string') ? `^^<${t.datatype}>` : ''}`);

async function takeBack(ctx, state, base, q) {
  const f = await fileOf(ctx, base, [...state.files], q);
  if (!f) return ctx.say('That statement is in no file chosen.');
  await ctx.api(`${base}/linked/${encodeURIComponent(f)}/edit`, { method: 'POST', body: { removed: [q] } });
  ctx.log(`A statement taken back from ${f}.`);
  ctx.show('graph');
}

function termOf(text, prefixes = L.COMMON_PREFIXES) {
  const t = text.trim();
  if (/^<[^>]+>$/.test(t)) return { kind: 'iri', value: t.slice(1, -1) };
  const lit = /^"([\s\S]*)"(?:@([A-Za-z-]+))?$/.exec(t);
  if (lit) return lit[2] ? { kind: 'literal', value: lit[1], language: lit[2], datatype: 'http://www.w3.org/1999/02/22-rdf-syntax-ns#langString' } : { kind: 'literal', value: lit[1], datatype: 'http://www.w3.org/2001/XMLSchema#string' };
  const pn = /^([A-Za-z][A-Za-z0-9_-]*):(.*)$/.exec(t);
  if (pn && prefixes[pn[1]]) return { kind: 'iri', value: prefixes[pn[1]] + pn[2] };
  if (/^-?\d+$/.test(t)) return { kind: 'literal', value: t, datatype: 'http://www.w3.org/2001/XMLSchema#integer' };
  if (t) return { kind: 'literal', value: t, datatype: 'http://www.w3.org/2001/XMLSchema#string' };
  throw new Error('say a value');
}

async function say(ctx, state, base, s, ptext, otext) {
  const p = termOf(ptext);
  if (p.kind !== 'iri') throw new Error('a predicate is an IRI or a prefixed name');
  const o = termOf(otext);
  const target = [...state.files].find((f) => f.endsWith('.ttl')) || 'notes.ttl';
  const r = await ctx.api(`${base}/linked/${encodeURIComponent(target)}/edit`, { method: 'POST', body: { added: [[s, p, o]] } });
  ctx.log(`Said of ${L.label(s)}: ${L.label(p)} ${L.label(o)} (in ${r.file}).`);
  state.files.add(r.file);
  ctx.show('graph');
}

async function deskOf(ctx, state, desk, base, files, chosen) {
  const { el, fill } = ctx;
  const editor = (text, onrun, word) => {
    const area = el('textarea', { class: 'code', spellcheck: false });
    area.value = text;
    const shade = el('pre', { class: 'code-shade', 'aria-hidden': 'true' });
    const paint = () => { shade.innerHTML = `${L.htmlTurtle(area.value)}\n`; shade.scrollTop = area.scrollTop; };
    area.addEventListener('input', paint);
    area.addEventListener('scroll', () => { shade.scrollTop = area.scrollTop; shade.scrollLeft = area.scrollLeft; });
    paint();
    return { box: el('div', { class: 'code-box' }, shade, area), area, run: el('button', { type: 'button', class: 'key main', onclick: () => onrun(area.value) }, word) };
  };
  if (state.leaf === 'query') {
    const e = editor(state.query, async (text) => {
      state.query = text;
      try {
        state.result = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'query', files: chosen, query: text } });
        state.marked = new Set();
        if (state.result.rows) for (const row of state.result.rows) for (const t of row) if (t && t.kind !== 'literal') state.marked.add(L.key(t));
      } catch (error) { state.result = { error: error.message }; }
      ctx.show('graph');
    }, 'Run');
    const r = state.result;
    const answer = !r ? el('p', { class: 'hint' }, 'SPARQL\'s core: SELECT, ASK and CONSTRUCT; OPTIONAL, UNION, MINUS, FILTER, BIND, VALUES, GRAPH; property paths; aggregates. What it binds is marked on the graph.')
      : r.error ? el('p', { class: 'wrong' }, r.error)
        : r.form === 'ask' ? el('p', { class: `verdict ${r.truth ? 'holds' : 'fails'}` }, r.truth ? '✓ yes' : '✗ no')
          : r.form === 'construct' ? el('p', {}, `${r.triples.length} statements constructed.`)
            : el('table', { class: 'answers' }, el('tr', {}, r.variables.map((v) => el('th', {}, `?${v}`))), r.rows.slice(0, 200).map((row) => el('tr', {}, row.map((t) => el('td', {}, t ? L.label(t, undefined, state.named) : '—')))));
    fill(desk, el('div', { class: 'leaf query' }, el('div', { class: 'left' }, e.box, el('div', { class: 'keys' }, e.run)), el('div', { class: 'right' }, answer)));
    return;
  }
  if (state.leaf === 'shapes') {
    const shapes = files.filter((f) => /shape/i.test(f.file));
    const pick = el('select', { class: 'field' }, files.map((f) => el('option', { value: f.file, selected: shapes[0]?.file === f.file }, f.file)));
    const run = el('button', { type: 'button', class: 'key main', onclick: async () => {
      try {
        state.report = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'validate', files: chosen.includes(pick.value) ? chosen : [...chosen, pick.value], shapes: pick.value } });
        state.marked = new Set(state.report.results.map((x) => L.key(x.focus)));
      } catch (error) { state.report = { error: error.message }; }
      ctx.show('graph');
    } }, 'Check');
    const r = state.report;
    fill(desk, el('div', { class: 'leaf' }, el('p', { class: 'keys' }, 'Shapes from ', pick, ' ', run),
      !r ? el('p', { class: 'hint' }, 'SHACL Core: targets, paths, value types, cardinality, ranges, patterns, lists, logic, nesting, closed shapes. The nodes that fail are marked on the graph.')
        : r.error ? el('p', { class: 'wrong' }, r.error)
          : r.conforms ? el('p', { class: 'verdict holds' }, '✓ the data conform to every shape')
            : el('table', { class: 'answers' }, el('tr', {}, ['Node', 'Path', 'Value', 'Constraint', 'Severity', 'Message'].map((w) => el('th', {}, w))),
              r.results.map((x) => el('tr', { class: x.severity.toLowerCase() }, el('td', {}, L.label(x.focus, undefined, state.named)), el('td', {}, x.path), el('td', {}, x.value ? L.label(x.value) : ''),
                el('td', {}, x.component.replace('sh:', '').replace('ConstraintComponent', '')), el('td', {}, x.severity), el('td', {}, x.message))))));
    return;
  }
  if (state.leaf === 'entail') {
    const regime = el('select', { class: 'field' }, el('option', { value: 'rdfs' }, 'RDFS'), el('option', { value: 'owl' }, 'RDFS and part of OWL 2 RL'));
    const run = el('button', { type: 'button', class: 'key main', onclick: async () => {
      try {
        const r = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'entail', files: chosen, regime: regime.value } });
        state.inferred = r.derived;
        state.problems = r.inconsistencies;
      } catch (error) { state.problems = [error.message]; }
      ctx.show('graph');
    } }, 'Entail');
    const clear = state.inferred.length ? el('button', { type: 'button', class: 'key edged', onclick: () => { state.inferred = []; state.problems = []; ctx.show('graph'); } }, 'Set aside') : null;
    fill(desk, el('div', { class: 'leaf' }, el('p', { class: 'keys' }, regime, ' ', run, ' ', clear),
      (state.problems || []).length ? el('ul', { class: 'wrong' }, state.problems.map((p) => el('li', {}, p))) : null,
      state.inferred.length ? el('table', { class: 'answers' }, el('tr', {}, ['Concluded', 'By', 'From'].map((w) => el('th', {}, w))),
        state.inferred.slice(0, 300).map((x) => el('tr', {}, el('td', {}, x.statement.slice(0, 3).map((t) => L.label(t, undefined, state.named)).join(' ')), el('td', {}, x.rule), el('td', {}, x.premises.map((p) => p.map((t) => L.label(t, undefined, state.named)).join(' ')).join(' ; ')))))
        : el('p', { class: 'hint' }, 'What the vocabulary of classes and properties lets one conclude, drawn dashed on the graph, each with its rule and premises; inconsistencies are reported, not repaired.')));
    return;
  }
  if (state.leaf === 'text') {
    const names = files.map((f) => f.file);
    if (!state.edit || !names.includes(state.edit)) state.edit = names[0] || 'notes.ttl';
    if (state.loaded !== state.edit) {
      const got = names.includes(state.edit) ? await ctx.api(`${base}/linked/${encodeURIComponent(state.edit)}`) : null;
      state.text = got ? got.text : state.edit.endsWith('.jsonld') ? '{\n  "@context": { "dcterms": "http://purl.org/dc/terms/" },\n  "@graph": []\n}\n' : '@prefix dcterms: <http://purl.org/dc/terms/> .\n\n';
      state.read = got ? { text: got.text, sha256: got.sha256 } : null;
      state.loaded = state.edit;
      state.error = null;
    }
    const e = editor(state.text, async (text) => {
      state.text = text;
      // The version read goes with the change: if another changed the file since, the server merges the two by statements.
      const body = { text, ...(state.read ? { base: state.read.sha256, base_text: state.read.text } : {}) };
      const r = await ctx.fetch(`${base}/linked/${encodeURIComponent(state.edit)}`, { method: 'PUT', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) });
      const a = await r.json().catch(() => ({}));
      state.error = r.ok ? null : a;
      if (r.ok) {
        ctx.log(a.merged ? `${state.edit} kept, merged with another's change: ${a.yours.added} of your statements added and ${a.yours.removed} removed (${a.statements} in all). ${a.note}.` : `${state.edit} kept: ${a.statements} statements.`);
        state.files.add(state.edit);
        state.loaded = null;   // read again: the version kept, and its digest, are now the base of the next change
      }
      ctx.show('graph');
    }, 'Keep');
    const which = el('select', { class: 'field', onchange: (ev) => { state.edit = ev.target.value; ctx.show('graph'); } }, [...new Set([...names, state.edit])].map((n) => el('option', { value: n, selected: n === state.edit }, n)));
    const fresh = el('input', { type: 'text', class: 'field', placeholder: 'a new file: name.ttl or name.jsonld' });
    fresh.addEventListener('change', () => { if (/\.(ttl|trig|nt|nq|jsonld)$/.test(fresh.value)) { state.edit = fresh.value; state.loaded = null; ctx.show('graph'); } });
    fill(desk, el('div', { class: 'leaf text' }, el('p', { class: 'keys' }, which, ' ', fresh, ' ', e.run),
      state.error ? el('p', { class: 'wrong' }, state.error.message, state.error.current ? ' Another changed it meanwhile and your page holds no version to merge from: read it again, and make your change on it.' : '',
        state.error.missing ? ` The context ${state.error.missing} could not be fetched.` : '') : null, e.box));
    if (state.error?.line) {
      const lines = state.text.split('\n');
      const at = lines.slice(0, state.error.line - 1).reduce((n, l) => n + l.length + 1, 0) + state.error.column - 1;
      e.area.focus();
      e.area.setSelectionRange(at, at + 1);
    }
  }
}
