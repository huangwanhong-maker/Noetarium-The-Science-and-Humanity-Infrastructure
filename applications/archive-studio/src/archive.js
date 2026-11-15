// The archive (ADR 0119): a project's holdings as a wall, each drawn by its kind; one chosen opens in place into a reading room.
//
// The wall shows what each holding is: an image as itself, a sound as the shape of its waves, a moving image by its first frame,
// a text by its first lines, a table by its first rows, anything else by its kind. The reading room shows the holding large, and
// beside it its record: what the archive says of it (its description in linked/holdings.ttl, Dublin Core terms, each field kept
// as a statement, every change with its undo), its fixity (the SHA-256 kept when it came, and the bytes digested again in the page
// to verify it), its annotations (W3C Web Annotations in linked/annotations.ttl: a region of an image, a span of a sound or a
// moving image, a passage of a text, each with a note or a tag), and its links to other holdings. A ring of actions sits on the
// holding: describe, annotate, link, see it in the graph, return.

import * as L from '/visualization/js/linked/index.js';

const DC = 'http://purl.org/dc/terms/';
const FIELDS = [
  ['title', 'Title'], ['creator', 'Creator'], ['date', 'Date'], ['description', 'Description'], ['subject', 'Subject'], ['spatial', 'Place'],
  ['language', 'Language'], ['rights', 'Rights'], ['source', 'Source'],
];
const RELATIONS = [['relation', 'is related to'], ['isPartOf', 'is part of'], ['hasPart', 'has part'], ['references', 'refers to'], ['isVersionOf', 'is a version of']];
const KINDS = [['all', 'Everything'], ['image', 'Images'], ['audio', 'Sound'], ['video', 'Moving images'], ['text', 'Texts'], ['table', 'Tables'], ['other', 'Other']];
const TEXTUAL = /\.(txt|md|csv|tsv|json|ttl|trig|nt|xml|html?)$/i;

const kindOf = (h) => (h.type.startsWith('image/') ? 'image' : h.type.startsWith('audio/') ? 'audio' : h.type.startsWith('video/') ? 'video'
  : /\.(csv|tsv)$/i.test(h.name) ? 'table' : TEXTUAL.test(h.name) || h.type === 'application/pdf' ? 'text' : 'other');
const bytes = (n) => (n >= 1e6 ? `${(n / 1e6).toFixed(1)} MB` : n >= 1e3 ? `${Math.round(n / 1e3)} kB` : `${n} bytes`);
const iriT = (value) => ({ kind: 'iri', value });
const litT = (value) => ({ kind: 'literal', value: String(value), datatype: 'http://www.w3.org/2001/XMLSchema#string' });

export function archiveView() {
  const state = { kind: 'all', open: null, mode: null, undo: [], selection: null, region: null, span: { start: null, end: null } };
  return { id: 'archive', label: 'Archive', draw: (pane, ctx) => draw(pane, ctx, state) };
}

async function draw(pane, ctx, state) {
  const { el, fill } = ctx;
  pane.classList.add('archive');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  const name = ctx.name;
  const { holdings } = await ctx.api(`/api/projects/${encodeURIComponent(name)}/holdings`);
  const described = await titles(ctx, name);
  if (state.open && holdings.some((h) => h.name === state.open)) return room(pane, ctx, state, holdings, described);
  state.open = null;
  ctx.note(`${holdings.length} ${holdings.length === 1 ? 'holding' : 'holdings'}`);
  const counts = Object.fromEntries(KINDS.map(([k]) => [k, k === 'all' ? holdings.length : holdings.filter((h) => kindOf(h) === k).length]));
  const input = el('input', { type: 'file', multiple: true, hidden: true, onchange: (e) => add(ctx, [...e.target.files]).then(() => ctx.show('archive')) });
  const head = el('header', { class: 'wall-head' },
    el('h2', {}, ctx.project.title || 'The archive'),
    el('nav', { class: 'kinds', 'aria-label': 'Kinds of holding' }, KINDS.filter(([k]) => k === 'all' || counts[k]).map(([k, word]) =>
      el('button', { type: 'button', class: `chip${state.kind === k ? ' on' : ''}`, onclick: () => { state.kind = k; ctx.show('archive'); } }, `${word} `, el('span', { class: 'n' }, String(counts[k]))))),
    el('span', { class: 'spacer' }),
    el('button', { type: 'button', class: 'key main', onclick: () => input.click() }, 'Add holdings'), input);
  if (!holdings.length) {
    return fill(pane, head, el('div', { class: 'empty-wall' },
      el('p', {}, 'The archive holds nothing yet. Drop files anywhere on the page, or add them: images, sound, moving images, documents and tables are each kept under their name with their digest, described as linked data, and can be annotated.'),
      el('button', { type: 'button', class: 'key edged', onclick: () => example(ctx).then(() => ctx.show('archive')).catch((e) => ctx.say(e.message)) }, 'Begin with an example archive')));
  }
  const shown = holdings.filter((h) => state.kind === 'all' || kindOf(h) === state.kind);
  const wall = el('div', { class: 'wall' }, shown.map((h) => tile(ctx, h, described.get(h.iri), () => { state.open = h.name; state.mode = null; state.undo = []; ctx.show('archive'); })));
  fill(pane, head, wall);
  pane.ondragover = (e) => { if (e.dataTransfer?.types.includes('Files')) { e.preventDefault(); e.stopPropagation(); } };
  pane.ondrop = (e) => { if (e.dataTransfer?.files.length) { e.preventDefault(); e.stopPropagation(); add(ctx, [...e.dataTransfer.files]).then(() => ctx.show('archive')); } };
}

// What the archive says of each holding: its title, date and creator, by one query (nothing, if the engine is not built).
async function titles(ctx, name) {
  const out = new Map();
  try {
    const r = await ctx.api(`/api/projects/${encodeURIComponent(name)}/ask`, {
      method: 'POST',
      body: { op: 'query', query: `PREFIX dcterms: <${DC}> PREFIX foaf: <http://xmlns.com/foaf/0.1/> SELECT ?h ?title ?date ?creator ?name WHERE { ?h dcterms:identifier ?id . OPTIONAL { ?h dcterms:title ?title } OPTIONAL { ?h dcterms:date ?date } OPTIONAL { ?h dcterms:creator ?creator OPTIONAL { ?creator foaf:name ?name } } }` },
    });
    for (const [h, title, date, creator, name] of r.rows) if (h) out.set(h.value, { title: title?.value, date: date?.value, creator: name?.value || (creator ? L.label(creator) : undefined) });
  } catch (error) {
    ctx.log(`The archive's descriptions could not be read: ${error.message}`, 'wrong');
  }
  return out;
}

function source(ctx, h) { return `/api/projects/${encodeURIComponent(ctx.name)}/holdings/${encodeURIComponent(h.name)}`; }

function tile(ctx, h, d, open) {
  const { el } = ctx, kind = kindOf(h);
  const face = el('div', { class: `face ${kind}` });
  if (kind === 'image') face.append(el('img', { src: source(ctx, h), alt: d?.title || h.name, loading: 'lazy' }));
  else if (kind === 'video') face.append(el('video', { src: `${source(ctx, h)}#t=0.5`, preload: 'metadata', muted: true }));
  else if (kind === 'audio') waveform(ctx, h, face);
  else if (kind === 'text' || kind === 'table') opening(ctx, h, face, kind);
  else face.append(el('span', { class: 'glyph' }, h.name.split('.').pop().toUpperCase()));
  return el('button', { type: 'button', class: `holding ${kind}`, onclick: open, title: h.name },
    face,
    el('span', { class: 'caption' },
      el('span', { class: 'name' }, d?.title || h.name),
      el('span', { class: 'meta' }, [d?.date, d?.creator, bytes(h.bytes)].filter(Boolean).join(' · '))));
}

// The shape of a sound's waves, drawn from its samples (sounds under 25 MB).
async function waveform(ctx, h, face, width = 220, height = 64) {
  const glyph = ctx.el('span', { class: 'glyph' }, '♪');
  face.append(glyph);
  if (h.bytes > 25e6 || !window.AudioContext) return;
  try {
    const data = await (await fetch(source(ctx, h))).arrayBuffer();
    const audio = new AudioContext();
    const buffer = await audio.decodeAudioData(data);
    audio.close();
    const ch = buffer.getChannelData(0), per = Math.max(1, Math.floor(ch.length / width));
    let d = '';
    for (let x = 0; x < width; x++) {
      let lo = 0, hi = 0;
      for (let i = x * per; i < Math.min(ch.length, (x + 1) * per); i++) lo = Math.min(lo, ch[i]), hi = Math.max(hi, ch[i]);
      d += `M${x} ${(height / 2 - hi * height / 2).toFixed(1)}V${(height / 2 - lo * height / 2 + 0.5).toFixed(1)}`;
    }
    glyph.replaceWith(ctx.svg('svg', { class: 'wave', viewBox: `0 0 ${width} ${height}`, preserveAspectRatio: 'none' }, ctx.svg('path', { d })));
  } catch { /* a sound the page cannot decode keeps its glyph */ }
}

async function opening(ctx, h, face, kind) {
  if (h.type === 'application/pdf') return face.append(ctx.el('span', { class: 'glyph' }, 'PDF'));
  try {
    const text = (await (await fetch(source(ctx, h))).text()).slice(0, 900);
    if (kind === 'table') {
      const rows = text.split(/\r?\n/).filter(Boolean).slice(0, 6).map((r) => r.split(/[,\t]/).slice(0, 5));
      face.append(ctx.el('table', {}, rows.map((r, i) => ctx.el('tr', {}, r.map((c) => ctx.el(i ? 'td' : 'th', {}, c.replace(/^"|"$/g, '')))))));
    } else {
      face.append(ctx.el('p', { class: 'lines' }, text));
    }
  } catch { face.append(ctx.el('span', { class: 'glyph' }, 'TXT')); }
}

async function add(ctx, files) {
  for (const f of files) {
    const r = await fetch(`/api/projects/${encodeURIComponent(ctx.name)}/holdings?name=${encodeURIComponent(f.name)}`, { method: 'POST', body: f });
    const a = await r.json().catch(() => ({}));
    if (!r.ok) { ctx.say(a.message || `${f.name} could not be kept`); continue; }
    ctx.log(`${a.name} kept (${bytes(a.bytes)}, SHA-256 ${a.sha256.slice(0, 12)}…) and described in linked/holdings.ttl.`);
  }
}

async function example(ctx) {
  const r = await fetch(`/api/projects/${encodeURIComponent(ctx.name)}/example`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: '{}' });
  const a = await r.json().catch(() => ({}));
  if (!r.ok) throw new Error(a.message || 'the example could not be laid out');
  ctx.log(`The example archive: ${a.holdings} holdings and ${a.files} files of linked data.`);
}

// ---- The reading room --------------------------------------------------------------------------------------------------------------

async function room(pane, ctx, state, holdings, described) {
  const { el, fill } = ctx;
  const h = holdings.find((x) => x.name === state.open), kind = kindOf(h), d = described.get(h.iri) || {};
  ctx.note(d.title || h.name);
  const base = `/api/projects/${encodeURIComponent(ctx.name)}`;
  const statements = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'describe', node: iriT(h.iri) } }).catch(() => ({ out: [], in: [] }));
  const named = await ctx.api(`${base}/ask`, { method: 'POST', body: { op: 'query', query: `CONSTRUCT { ?x ?p ?n } WHERE { ?x ?p ?n FILTER(${L.NAME_PREDICATES.map((p) => `?p = <${p}>`).join(' || ')}) }` } })
    .then((r) => L.names(r.triples)).catch(() => new Map());
  const marks = await annotationsOf(ctx, h.iri);
  const viewer = el('div', { class: `viewer ${kind}` });
  const record = el('aside', { class: 'record' });
  const ring = el('nav', { class: 'ring', 'aria-label': 'What can be done with this holding' },
    [['describe', 'Describe'], ['annotate', 'Annotate'], ['link', 'Link']].map(([m, word]) =>
      el('button', { type: 'button', class: `key${state.mode === m ? ' on' : ''}`, onclick: () => { state.mode = state.mode === m ? null : m; ctx.show('archive'); } }, word)),
    el('button', { type: 'button', class: 'key', onclick: () => { window.archiveFocus = h.iri; ctx.show('graph'); } }, 'In the graph'),
    el('button', { type: 'button', class: 'key', onclick: () => { state.open = null; ctx.show('archive'); } }, 'Back to the wall'));
  fill(pane, el('div', { class: 'room' }, el('div', { class: 'stage-left' }, ring, viewer), record));
  await viewerOf(ctx, state, h, kind, viewer, marks);
  // The record: the description as fields, the fixity, the annotations, the links.
  const values = (local) => statements.out.filter((q) => q[1].value === DC + local).map((q) => q[2]);
  const save = async (removed, added) => {
    const r = await ctx.api(`${base}/linked/holdings.ttl/edit`, { method: 'POST', body: { removed, added } });
    state.undo.push(r.undo);
    ctx.log(`The description of ${d.title || h.name} kept (${r.statements} statements in linked/holdings.ttl).`);
    ctx.show('archive');
  };
  // A field holds the literal values of its term; values that are things (IRIs) are shown by their names beside it and kept as they are.
  const fields = FIELDS.map(([local, word]) => {
    const now = values(local), words = now.filter((t) => t.kind === 'literal'), things = now.filter((t) => t.kind !== 'literal');
    const input = el(local === 'description' ? 'textarea' : 'input', { type: local === 'description' ? undefined : 'text', class: 'field', rows: 3, disabled: state.mode !== 'describe' });
    input.value = words.map((t) => t.value).join(local === 'description' ? '\n' : '; ');
    input.addEventListener('change', () => {
      const text = input.value.trim();
      const added = text ? (local === 'description' ? [text] : text.split(';').map((s) => s.trim()).filter(Boolean)).map((v) => [iriT(h.iri), iriT(DC + local), litT(v)]) : [];
      save(words.map((o) => [iriT(h.iri), iriT(DC + local), o]), added).catch((e) => ctx.say(e.message));
    });
    if (!words.length && things.length && state.mode !== 'describe') input.hidden = true;
    return el('label', { class: 'row' }, el('span', { class: 'word' }, word),
      el('span', { class: 'values' }, things.map((t) => el('span', { class: 'thing', title: t.value }, L.label(t, undefined, named))), input));
  });
  const fixity = el('p', { class: 'fixity' }, el('span', { class: 'word' }, 'SHA-256'), el('code', {}, h.sha256 || 'not kept'));
  const verify = el('button', { type: 'button', class: 'key edged' }, 'Verify the bytes');
  verify.addEventListener('click', async () => {
    verify.disabled = true;
    const data = await (await fetch(source(ctx, h))).arrayBuffer();
    const hex = [...new Uint8Array(await crypto.subtle.digest('SHA-256', data))].map((b) => b.toString(16).padStart(2, '0')).join('');
    const same = hex === h.sha256;
    fixity.append(el('span', { class: `verdict ${same ? 'holds' : 'fails'}` }, same ? '✓ the same bytes as when the holding was kept' : `✗ the bytes differ: ${hex.slice(0, 16)}…`));
  });
  const links = statements.out.filter((q) => RELATIONS.some(([r]) => q[1].value === DC + r));
  const linkForm = state.mode === 'link' ? linker(ctx, h, holdings, described, save) : null;
  const undo = state.undo.length ? el('button', { type: 'button', class: 'key edged', onclick: async () => {
    const last = state.undo.pop();
    await ctx.api(`${base}/linked/holdings.ttl/edit`, { method: 'POST', body: last });
    ctx.log('The last change to the description undone.');
    ctx.show('archive');
  } }, 'Undo the last change') : null;
  fill(record,
    el('h3', {}, d.title || h.name),
    el('p', { class: 'kindline' }, `${({ image: 'An image', audio: 'A sound', video: 'A moving image', text: 'A text', table: 'A table', other: 'A file' })[kind]} · ${h.type} · ${bytes(h.bytes)}`),
    el('section', { class: 'fields' }, fields, state.mode === 'describe' ? el('p', { class: 'hint' }, 'Each field is kept as a statement in Dublin Core terms when it is left; several values are parted by “;”.') : null, undo),
    el('section', {}, el('h4', {}, 'Fixity'), fixity, verify),
    el('section', {}, el('h4', {}, `Annotations · ${marks.length}`), marks.length ? el('ol', { class: 'marks' }, marks.map((m) => el('li', {}, el('span', { class: 'where' }, where(m.selector)), ' ', m.body?.tag ? L.brief(m.body.tag) : m.body?.text || ''))) : el('p', { class: 'hint' }, 'None yet. Choose Annotate, then mark a region, a span or a passage.')),
    el('section', {}, el('h4', {}, `Links · ${links.length}`), links.length ? el('ul', { class: 'links' }, links.map((q) => el('li', {}, RELATIONS.find(([r]) => q[1].value === DC + r)[1], ' ', el('em', {}, described.get(q[2].value)?.title || L.label(q[2], undefined, named))))) : null, linkForm),
    el('section', { class: 'plain' }, el('h4', {}, 'Every statement'), el('ul', { class: 'statements' }, statements.out.map((q) => el('li', {}, el('span', { class: 'p' }, L.label(q[1])), ' ', L.label(q[2], undefined, named))))));
}

const where = (s) => (!s ? 'the whole' : s.kind === 'region' ? `region ${Math.round(s.w)} × ${Math.round(s.h)} at ${Math.round(s.x)}, ${Math.round(s.y)}` : s.kind === 'time' ? `${clock(s.start)}–${clock(s.end)}` : `“${s.exact.length > 40 ? `${s.exact.slice(0, 40)}…` : s.exact}”`);
const clock = (t) => `${Math.floor(t / 60)}:${String(Math.floor(t % 60)).padStart(2, '0')}`;

async function annotationsOf(ctx, target) {
  try {
    const r = await ctx.api(`/api/projects/${encodeURIComponent(ctx.name)}/ask`, { method: 'POST', body: { op: 'query', files: ['annotations.ttl'], query: 'CONSTRUCT { ?s ?p ?o } WHERE { ?s ?p ?o }' } });
    return L.annotations(r.triples).filter((a) => a.target === target);
  } catch { return []; }
}

async function keepAnnotation(ctx, target, selector, text) {
  const id = `urn:archive:${encodeURIComponent(ctx.name)}:annotation:${Date.now().toString(36)}`;
  const quads = L.annotation({ id, target, selector, body: { text }, created: new Date().toISOString().replace(/\.\d+Z$/, 'Z') });
  await ctx.api(`/api/projects/${encodeURIComponent(ctx.name)}/linked/annotations.ttl/edit`, { method: 'POST', body: { added: quads } });
  ctx.log(`An annotation kept: ${where(selector)}.`);
}

// A note asked for in place, under what was marked; resolves with its text, or null.
function noteFor(ctx, host, words) {
  return new Promise((resolve) => {
    const input = ctx.el('input', { type: 'text', class: 'field', placeholder: words });
    const box = ctx.el('div', { class: 'note-in' }, input,
      ctx.el('button', { type: 'button', class: 'key main', onclick: () => { box.remove(); resolve(input.value.trim() || null); } }, 'Keep'),
      ctx.el('button', { type: 'button', class: 'key', onclick: () => { box.remove(); resolve(null); } }, 'Leave'));
    input.addEventListener('keydown', (e) => { if (e.key === 'Enter') { box.remove(); resolve(input.value.trim() || null); } if (e.key === 'Escape') { box.remove(); resolve(null); } });
    host.append(box);
    input.focus();
  });
}

async function viewerOf(ctx, state, h, kind, viewer, marks) {
  const { el, svg } = ctx, src = source(ctx, h), annotating = state.mode === 'annotate';
  if (kind === 'image') {
    const img = el('img', { src, alt: h.name, draggable: false });
    const layer = svg('svg', { class: 'marks-layer' });
    const frame = el('div', { class: `frame${annotating ? ' annotating' : ''}` }, img, layer);
    viewer.append(...[frame, annotating ? el('p', { class: 'hint' }, 'Drag across the image to mark a region.') : null].filter(Boolean));
    let zoom = 1, ox = 0, oy = 0, drag = null;
    const place = () => { frame.style.transform = `translate(${ox}px, ${oy}px) scale(${zoom})`; };
    const naturalOf = (e) => {
      const r = img.getBoundingClientRect();
      return { x: ((e.clientX - r.left) / r.width) * img.naturalWidth, y: ((e.clientY - r.top) / r.height) * img.naturalHeight };
    };
    const drawMarks = () => {
      ctx.fill(layer);
      layer.setAttribute('viewBox', `0 0 ${img.naturalWidth || 1} ${img.naturalHeight || 1}`);
      for (const m of marks) if (m.selector?.kind === 'region') layer.append(svg('rect', { class: 'a-mark', x: m.selector.x, y: m.selector.y, width: m.selector.w, height: m.selector.h }, svg('title', {}, m.body?.text || '')));
    };
    img.addEventListener('load', drawMarks);
    if (img.complete && img.naturalWidth) drawMarks();  // an image already loaded fires no load
    viewer.addEventListener('wheel', (e) => { e.preventDefault(); zoom = Math.min(8, Math.max(0.3, zoom * (e.deltaY < 0 ? 1.12 : 1 / 1.12))); place(); }, { passive: false });
    frame.addEventListener('pointerdown', (e) => {
      frame.setPointerCapture(e.pointerId);
      drag = annotating ? { start: naturalOf(e), rect: svg('rect', { class: 'a-mark drawing' }) } : { x: e.clientX, y: e.clientY, ox, oy };
      if (annotating) layer.append(drag.rect);
    });
    frame.addEventListener('pointermove', (e) => {
      if (!drag) return;
      if (!annotating) { ox = drag.ox + e.clientX - drag.x; oy = drag.oy + e.clientY - drag.y; place(); return; }
      const p = naturalOf(e), s = drag.start;
      Object.entries({ x: Math.min(s.x, p.x), y: Math.min(s.y, p.y), width: Math.abs(p.x - s.x), height: Math.abs(p.y - s.y) }).forEach(([k, v]) => drag.rect.setAttribute(k, v));
    });
    frame.addEventListener('pointerup', async (e) => {
      const was = drag;
      drag = null;
      if (!annotating || !was) return;
      const p = naturalOf(e), s = was.start;
      const sel = { kind: 'region', x: Math.min(s.x, p.x), y: Math.min(s.y, p.y), w: Math.abs(p.x - s.x), h: Math.abs(p.y - s.y) };
      if (sel.w < 4 || sel.h < 4) { was.rect.remove(); return; }
      const text = await noteFor(ctx, viewer, 'What is in this region?');
      if (text) { await keepAnnotation(ctx, h.iri, sel, text); ctx.show('archive'); } else was.rect.remove();
    });
    return;
  }
  if (kind === 'audio' || kind === 'video') {
    const media = el(kind, { src, preload: 'metadata' });
    const bar = el('div', { class: 'timeline' }), head = el('div', { class: 'playhead' }), time = el('span', { class: 'time' }, '0:00');
    bar.append(head);
    const play = el('button', { type: 'button', class: 'key main' }, 'Play');
    play.addEventListener('click', () => (media.paused ? media.play() : media.pause()));
    media.addEventListener('play', () => { play.textContent = 'Pause'; });
    media.addEventListener('pause', () => { play.textContent = 'Play'; });
    const segments = () => {
      for (const s of bar.querySelectorAll('.segment')) s.remove();
      if (!media.duration) return;
      for (const m of marks) if (m.selector?.kind === 'time') bar.append(el('div', { class: 'segment', title: m.body?.text || '', style: { left: `${(m.selector.start / media.duration) * 100}%`, width: `${((m.selector.end - m.selector.start) / media.duration) * 100}%` } }));
    };
    media.addEventListener('loadedmetadata', segments);
    media.addEventListener('timeupdate', () => { head.style.left = `${(media.currentTime / (media.duration || 1)) * 100}%`; time.textContent = `${clock(media.currentTime)} / ${clock(media.duration || 0)}`; });
    bar.addEventListener('click', (e) => { const r = bar.getBoundingClientRect(); media.currentTime = ((e.clientX - r.left) / r.width) * (media.duration || 0); });
    const marking = annotating ? el('span', { class: 'marking' },
      el('button', { type: 'button', class: 'key edged', onclick: () => { state.span.start = media.currentTime; ctx.say(`From ${clock(media.currentTime)}`); } }, 'Mark the start'),
      el('button', { type: 'button', class: 'key edged', onclick: async () => {
        state.span.end = media.currentTime;
        if (state.span.start == null || state.span.end <= state.span.start) return ctx.say('Mark a start before an end.');
        const sel = { kind: 'time', start: state.span.start, end: state.span.end };
        const text = await noteFor(ctx, viewer, `What is heard or seen from ${clock(sel.start)} to ${clock(sel.end)}?`);
        if (text) { await keepAnnotation(ctx, h.iri, sel, text); state.span = { start: null, end: null }; ctx.show('archive'); }
      } }, 'Mark the end and annotate')) : null;
    viewer.append(kind === 'video' ? media : el('div', { class: 'sound' }, media), el('div', { class: 'transport' }, play, time, marking), bar);
    if (kind === 'audio') waveform(ctx, h, viewer.querySelector('.sound'), 900, 140);
    return;
  }
  if (kind === 'text' || kind === 'table') {
    if (h.type === 'application/pdf') { viewer.append(el('iframe', { src, title: h.name })); return; }
    const text = await (await fetch(src)).text();
    const page = el('pre', { class: `page${annotating ? ' annotating' : ''}` }, text);
    // Passages annotated, highlighted where their quotation is found.
    for (const m of marks) if (m.selector?.kind === 'text') page.innerHTML = page.innerHTML.replace(escapeHtml(m.selector.exact), (x) => `<mark title="${escapeHtml(m.body?.text || '')}">${x}</mark>`);
    viewer.append(...[page, annotating ? el('p', { class: 'hint' }, 'Select a passage to annotate it.') : null].filter(Boolean));
    if (annotating) page.addEventListener('mouseup', async () => {
      const sel = window.getSelection(), exact = sel?.toString();
      if (!exact || !exact.trim()) return;
      const start = text.indexOf(exact);
      const selector = { kind: 'text', exact, prefix: start > 0 ? text.slice(Math.max(0, start - 24), start) : undefined, suffix: text.slice(start + exact.length, start + exact.length + 24) || undefined,
        start: start >= 0 ? start : undefined, end: start >= 0 ? start + exact.length : undefined };
      const note = await noteFor(ctx, viewer, 'A note on this passage');
      if (note) { await keepAnnotation(ctx, h.iri, selector, note); ctx.show('archive'); }
    });
    return;
  }
  viewer.append(el('p', { class: 'note' }, `The page has no viewer for ${h.type}; the holding is kept, described and can be annotated as a whole.`),
    el('a', { class: 'key edged', href: src, download: h.name }, 'Save a copy'));
}

const escapeHtml = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');

function linker(ctx, h, holdings, described, save) {
  const { el } = ctx;
  const other = el('select', { class: 'field' }, holdings.filter((x) => x.name !== h.name).map((x) => el('option', { value: x.iri }, described.get(x.iri)?.title || x.name)));
  const relation = el('select', { class: 'field' }, RELATIONS.map(([r, word]) => el('option', { value: r }, word)));
  return el('div', { class: 'linker' }, el('span', { class: 'word' }, 'This holding'), relation, other,
    el('button', { type: 'button', class: 'key main', onclick: () => save([], [[iriT(h.iri), iriT(DC + relation.value), iriT(other.value)]]).catch((e) => ctx.say(e.message)) }, 'Link'));
}
