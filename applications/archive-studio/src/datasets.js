// The datasets (ADR 0120): the archive's tables and the codebooks that describe them, as social-science data archives keep them
// (DDI-Codebook 2.5). A codebook is a study, its variables (each with its measure, its codes and the codes that mean missing) and the
// files it describes with their digests. Here it is read, drafted from a table, edited, written as DDI; a file is held to its codebook
// cell by cell and each variable's frequencies drawn; two versions are compared, with what breaks comparison; and two variables of two
// studies are harmonised, their codes joined by a correspondence and carried onto the finest target both can be recoded to. Every
// check is archeion's; the codebooks are also linked data in DISCO (linked/codebooks.ttl).

const MEASURES = ['nominal', 'ordinal', 'discrete', 'continuous', 'text', 'date'];
const enc = encodeURIComponent;

export function datasetsView() {
  const state = { chosen: null, variable: null, conform: null, compare: null, harmonise: null, map: new Set(), against: null, againstVar: null, drafting: false };
  return {
    id: 'datasets', label: 'Datasets', draw: (pane, ctx) => draw(pane, ctx, state),
    heard: (e, ctx) => { if (/codebook/.test(e.what) && document.querySelector('.tab.on')?.dataset.tab === 'datasets') ctx.show('datasets'); },
  };
}

const codesOf = (v) => (v.categories || []).map((c) => `${c.code}${c.label && c.label !== c.code ? ` ${c.label}` : ''}${c.missing ? ' (missing)' : ''}`);

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('datasets');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  const base = `/api/projects/${enc(ctx.name)}`;
  ctx.here('the datasets');
  const [{ codebooks }, { holdings }] = await Promise.all([ctx.api(`${base}/codebooks`), ctx.api(`${base}/holdings`)]);
  const tables = holdings.filter((h) => /\.(csv|tsv)$/i.test(h.name));
  ctx.note(`${codebooks.length} codebooks, ${tables.length} tables`);
  if (!state.chosen && codebooks.length) state.chosen = codebooks[0].name;
  const list = el('nav', { class: 'books' }, el('h4', {}, 'Codebooks'),
    codebooks.map((b) => el('button', { type: 'button', class: `book${state.chosen === b.name ? ' on' : ''}`, onclick: () => { Object.assign(state, { chosen: b.name, variable: null, conform: null, compare: null, harmonise: null }); ctx.show('datasets'); } },
      el('b', {}, b.title || b.name), el('small', {}, `${b.variables} variables · version ${b.version || '—'} · ${b.files.length} ${b.files.length === 1 ? 'file' : 'files'}`))),
    drafting(ctx, state, base, tables));
  if (!state.chosen) return fill(pane, el('div', { class: 'datasets-grid' }, list, el('div', { class: 'book-main' }, el('p', { class: 'hint' }, 'No codebook yet. Draft one from a table among the holdings, or read one written in DDI-Codebook 2.5.'))));
  const one = await ctx.api(`${base}/codebooks/${enc(state.chosen)}`).catch(() => null);
  if (!one) { state.chosen = null; return ctx.show('datasets'); }
  const c = one.codebook, s = c.study || {};
  const keep = async () => {
    try {
      const r = await ctx.api(`${base}/codebooks/${enc(state.chosen)}`, { method: 'PUT', body: { codebook: c } });
      ctx.log(`The codebook ${state.chosen} kept${r.faults.length ? `, with ${r.faults.length} faults` : ''}, and described in DISCO.`);
    } catch (error) { ctx.say(error.message); }
    ctx.show('datasets');
  };
  const vars = el('table', { class: 'variables' }, el('tr', {}, ['Variable', 'Label', 'Measure', 'Codes', 'Question'].map((w) => el('th', {}, w))),
    c.variables.map((v) => el('tr', { class: state.variable === v.name ? 'on' : '', onclick: () => { state.variable = v.name; ctx.show('datasets'); } },
      el('td', {}, el('code', {}, v.name)), el('td', {}, v.label || ''), el('td', {}, v.measure), el('td', { class: 'codes' }, codesOf(v).slice(0, 6).join(' · ') + (codesOf(v).length > 6 ? ' …' : '')),
      el('td', {}, v.question || ''))));
  const main = el('div', { class: 'book-main' },
    el('header', { class: 'wall-head' }, el('h2', {}, s.title || state.chosen), el('span', { class: 'kindline' }, `version ${s.version || '—'}${s.producer ? ` · ${s.producer}` : ''}`), el('span', { class: 'spacer' }),
      el('a', { class: 'key', href: ctx.keyed(`${base}/codebooks/${enc(state.chosen)}.xml`), download: `${state.chosen}.xml` }, 'DDI-Codebook 2.5')),
    one.faults.length ? el('ul', { class: 'faults' }, one.faults.map((f) => el('li', {}, el('span', { class: 'mark' }, '✗'), ' ', f))) : el('p', { class: 'verdict holds' }, '✓ ', one.verdict.reason),
    vars,
    state.variable ? variableCard(ctx, state, c, keep) : null,
    holdTo(ctx, state, base, c, tables),
    compareWith(ctx, state, base, codebooks),
    harmonise(ctx, state, base, c, codebooks));
  fill(pane, el('div', { class: 'datasets-grid' }, list, main));
}

function drafting(ctx, state, base, tables) {
  const { el } = ctx;
  const pick = el('select', { class: 'field' }, tables.map((t) => el('option', { value: t.name }, t.name)));
  const name = el('input', { type: 'text', class: 'field', placeholder: 'its name: letters, digits, - _ .' });
  const ddi = el('input', { type: 'file', accept: '.xml', hidden: true, onchange: async (e) => {
    const f = e.target.files[0];
    if (!f) return;
    const n = (name.value.trim() || f.name.replace(/\.xml$/i, '')).replace(/[^A-Za-z0-9_.-]/g, '-');
    try {
      const r = await ctx.api(`${base}/codebooks/${enc(n)}/ddi`, { method: 'POST', raw: f });
      ctx.log(`The DDI file ${f.name} read as the codebook ${n}: ${r.codebook.variables.length} variables${r.faults.length ? `, ${r.faults.length} faults` : ''}.`);
      state.chosen = n;
    } catch (error) { ctx.say(error.message); }
    ctx.show('datasets');
  } });
  return el('section', { class: 'draft' }, el('h4', {}, 'A codebook from'),
    tables.length ? [pick, name, el('button', { type: 'button', class: 'key', onclick: async () => {
      try {
        const r = await ctx.api(`${base}/codebooks/draft`, { method: 'POST', body: { holding: pick.value, name: name.value.trim() } });
        ctx.log(`A codebook drafted from ${pick.value}: ${r.codebook.variables.length} variables, their measures and codes guessed from the cells, to be checked.`);
        state.chosen = r.name;
      } catch (error) { ctx.say(error.message); }
      ctx.show('datasets');
    } }, 'Draft it from the table')] : el('p', { class: 'hint' }, 'No table among the holdings.'),
    el('button', { type: 'button', class: 'key', onclick: () => ddi.click() }, 'Read a DDI file'), ddi);
}

function variableCard(ctx, state, c, keep) {
  const { el } = ctx;
  const v = c.variables.find((x) => x.name === state.variable);
  if (!v) return null;
  const label = el('input', { type: 'text', class: 'field', value: v.label || '' });
  const question = el('input', { type: 'text', class: 'field', value: v.question || '' });
  const measure = el('select', { class: 'field' }, MEASURES.map((m) => el('option', { value: m, selected: v.measure === m }, m)));
  const cats = el('textarea', { class: 'field', rows: 5 });
  cats.value = (v.categories || []).map((k) => `${k.code} = ${k.label || ''}${k.missing ? ' (missing)' : ''}`).join('\n');
  return el('section', { class: 'variable-card' }, el('h4', {}, `The variable ${v.name}`),
    el('label', { class: 'row' }, el('span', { class: 'word' }, 'Label'), label), el('label', { class: 'row' }, el('span', { class: 'word' }, 'Question'), question),
    el('label', { class: 'row' }, el('span', { class: 'word' }, 'Measure'), measure),
    el('label', { class: 'row' }, el('span', { class: 'word' }, 'Codes'), cats), el('p', { class: 'hint' }, 'One code a line: code = label; a code that means missing ends with “(missing)”.'),
    el('button', { type: 'button', class: 'key main', onclick: () => {
      v.label = label.value.trim();
      v.question = question.value.trim();
      v.measure = measure.value;
      v.categories = cats.value.split('\n').map((l) => l.trim()).filter(Boolean).map((l) => {
        const missing = /\(missing\)\s*$/.test(l), [code, ...rest] = l.replace(/\(missing\)\s*$/, '').split('=');
        return { code: code.trim(), label: rest.join('=').trim(), missing };
      });
      keep();
    } }, 'Keep the codebook'));
}

function holdTo(ctx, state, base, c, tables) {
  const { el } = ctx;
  const files = (c.files || []).filter((f) => tables.some((t) => t.name === f.name));
  const r = state.conform;
  const freq = r && state.variable ? r.frequencies.find((f) => f.variable === state.variable) : null;
  const bars = freq ? frequencyBars(ctx, freq, c.variables.find((v) => v.name === state.variable)) : null;
  return el('section', { class: 'hold' }, el('h4', {}, 'The file held to its codebook'),
    files.length ? el('p', { class: 'keys' }, files.map((f) => el('button', { type: 'button', class: 'key edged', onclick: async () => {
      try { state.conform = { ...(await ctx.api(`${base}/codebooks/${enc(state.chosen)}/conform`, { method: 'POST', body: { holding: f.name } })), file: f.name }; } catch (error) { state.conform = { error: error.message }; }
      if (!state.variable) state.variable = c.variables.find((v) => (v.categories || []).length)?.name || null;
      ctx.show('datasets');
    } }, `Hold ${f.name} to it`))) : el('p', { class: 'hint' }, 'The codebook describes no file among the holdings.'),
    r?.error ? el('p', { class: 'wrong' }, r.error) : null,
    r && !r.error ? [el('p', { class: `verdict ${r.verdict.status}` }, r.verdict.status === 'holds' ? '✓ ' : '✗ ', `${r.file}: ${r.verdict.reason} (${r.cells} cells)`),
      r.faults.length ? el('ul', { class: 'faults' }, r.faults.slice(0, 12).map((f) => el('li', {}, el('span', { class: 'mark' }, '✗'), ' ', f))) : null,
      bars || el('p', { class: 'hint' }, 'Choose a variable above to see its frequencies.')] : null);
}

function frequencyBars(ctx, f, v) {
  const { el, svg } = ctx;
  const rows = [...Object.entries(f.valid).map(([k, n]) => ({ k, n, missing: false })), ...Object.entries(f.missing).map(([k, n]) => ({ k, n, missing: true })),
    ...(f.system_missing ? [{ k: 'empty', n: f.system_missing, missing: true }] : [])];
  if (!rows.length) return el('p', { class: 'hint' }, f.mean != null ? `${v?.label || f.variable}: mean ${f.mean.toFixed(2)}, standard deviation ${(f.sd ?? 0).toFixed(2)}, over ${f.cases} cases.` : 'No frequencies.');
  const most = Math.max(...rows.map((r) => r.n)), W = 520, H = 22 * rows.length + 8;
  const label = (k) => (v?.categories || []).find((c) => c.code === k)?.label || k;
  return el('figure', { class: 'freq' }, el('figcaption', {}, `${v?.label || f.variable}, over ${f.cases} cases`),
    svg('svg', { viewBox: `0 0 ${W} ${H}`, role: 'img', 'aria-label': `Frequencies of ${f.variable}` }, rows.map((r, i) => svg('g', { transform: `translate(0 ${4 + 22 * i})` },
      svg('text', { x: 150, y: 14, 'text-anchor': 'end', class: r.missing ? 'missing' : '' }, `${r.k === label(r.k) ? r.k : `${r.k} ${label(r.k)}`}`),
      svg('rect', { x: 158, y: 3, height: 14, width: Math.max(1, (W - 220) * r.n / most), class: r.missing ? 'bar missing' : 'bar' }),
      svg('text', { x: 164 + (W - 220) * r.n / most, y: 14, class: 'n' }, String(r.n))))));
}

function compareWith(ctx, state, base, codebooks) {
  const { el } = ctx;
  const others = codebooks.filter((b) => b.name !== state.chosen);
  if (!others.length) return null;
  const pick = el('select', { class: 'field' }, others.map((b) => el('option', { value: b.name }, b.title || b.name)));
  const r = state.compare;
  return el('section', { class: 'compare' }, el('h4', {}, 'Another version'), el('p', { class: 'keys' }, 'Compare with ', pick, ' ',
    el('button', { type: 'button', class: 'key edged', onclick: async () => {
      try { state.compare = await ctx.api(`${base}/codebooks/compare`, { method: 'POST', body: { earlier: state.chosen, later: pick.value } }); } catch (error) { state.compare = { error: error.message }; }
      ctx.show('datasets');
    } }, 'Compare')),
    r?.error ? el('p', { class: 'wrong' }, r.error) : null,
    r && !r.error ? [el('p', { class: `verdict ${r.compatible ? 'holds' : 'fails'}` }, r.compatible ? '✓ ' : '✗ ', r.verdict.reason),
      el('ul', { class: 'changes' }, r.changes.map((x) => el('li', { class: x.breaking ? 'breaking' : '' }, el('code', {}, x.variable), ' ', x.what, x.breaking ? el('span', { class: 'mark' }, ' breaks comparison') : null)))] : null);
}

function harmonise(ctx, state, base, c, codebooks) {
  const { el } = ctx;
  const others = codebooks.filter((b) => b.name !== state.chosen);
  const mine = c.variables.filter((v) => (v.categories || []).some((k) => !k.missing));
  if (!others.length || !mine.length) return null;
  const a = mine.find((v) => v.name === state.variable) || mine.find((v) => v.name === state.harmoniseA) || mine[0];
  if (state.harmoniseA !== a.name) { state.againstVar = null; state.map = new Set(); state.harmonise = null; }   // another variable: its partner chosen afresh
  state.harmoniseA = a.name;
  const pickA = el('select', { class: 'field', onchange: (e) => { state.harmoniseA = e.target.value; state.variable = e.target.value; state.map = new Set(); ctx.show('datasets'); } }, mine.map((v) => el('option', { value: v.name, selected: v.name === a.name }, v.name)));
  const pickBook = el('select', { class: 'field', onchange: (e) => { state.against = e.target.value; state.againstBook = null; state.map = new Set(); ctx.show('datasets'); } }, others.map((b) => el('option', { value: b.name, selected: state.against === b.name }, b.title || b.name)));
  if (!state.against) state.against = others[0].name;
  const box = el('section', { class: 'harmonise' }, el('h4', {}, 'Harmonise a variable'), el('p', { class: 'keys' }, pickA, ' with a variable of ', pickBook, ' '));
  ctx.api(`${base}/codebooks/${enc(state.against)}`).then((other) => {
    const theirs = other.codebook.variables.filter((v) => (v.categories || []).some((k) => !k.missing));
    if (!theirs.length) { box.append(el('p', { class: 'hint' }, 'That codebook has no variable with codes.')); return; }
    // The variable chosen first is the one whose codes share the most labels with this one's: the likeliest to mean the same.
    const shared = (v) => v.categories.filter((k) => !k.missing && a.categories.some((x) => !x.missing && (x.label || x.code) === (k.label || k.code))).length;
    const b = theirs.find((v) => v.name === state.againstVar) || [...theirs].sort((x, y) => shared(y) - shared(x))[0];
    state.againstVar = b.name;
    const pickB = el('select', { class: 'field', onchange: (e) => { state.againstVar = e.target.value; state.map = new Set(); ctx.show('datasets'); } }, theirs.map((v) => el('option', { value: v.name, selected: v.name === b.name }, v.name)));
    box.querySelector('.keys').append(pickB);
    const ac = a.categories.filter((k) => !k.missing), bc = b.categories.filter((k) => !k.missing);
    if (!state.map.size) for (const x of ac) for (const y of bc) if ((x.label || x.code) === (y.label || y.code)) state.map.add(`${x.code}|${y.code}`);
    const grid = el('table', { class: 'grid' }, el('tr', {}, el('th', {}, `${a.name} \\ ${b.name}`), bc.map((y) => el('th', {}, `${y.code} ${y.label || ''}`))),
      ac.map((x) => el('tr', {}, el('th', {}, `${x.code} ${x.label || ''}`), bc.map((y) => {
        const k = `${x.code}|${y.code}`;
        const cb = el('input', { type: 'checkbox', checked: state.map.has(k), 'aria-label': `${x.label} may mean ${y.label}`, onchange: (e) => { if (e.target.checked) state.map.add(k); else state.map.delete(k); } });
        return el('td', {}, cb);
      }))));
    const r = state.harmonise;
    // Element.append writes null out as text: what is not there is left out first.
    box.append(...[el('p', { class: 'hint' }, 'Tick the codes that may mean the same. The finest common target is the pushout of the two sets of codes along the correspondence: each class one code of the target.'), grid,
      el('button', { type: 'button', class: 'key main', onclick: async () => {
        try {
          state.harmonise = await ctx.api(`${base}/codebooks/common`, { method: 'POST', body: { a: { codebook: state.chosen, variable: a.name }, b: { codebook: state.against, variable: b.name }, corresponds: [...state.map].map((k) => k.split('|')) } });
        } catch (error) { state.harmonise = { error: error.message }; }
        ctx.show('datasets');
      } }, 'The common target'),
      r?.error ? el('p', { class: 'wrong' }, r.error) : null,
      r && !r.error ? el('ol', { class: 'classes' }, r.classes.map((cl, i) => el('li', {}, el('b', {}, `code ${i + 1}`), ' ← ', cl.map((m) => {
        const [side, code] = m.split(':');
        const v = side === 'a' ? a : b;
        return `${v.name} ${code} ${v.categories.find((k) => k.code === code)?.label || ''}`.trim();
      }).join(' · ')))) : null].filter(Boolean));
  }).catch((error) => box.append(el('p', { class: 'wrong' }, error.message)));
  return box;
}
