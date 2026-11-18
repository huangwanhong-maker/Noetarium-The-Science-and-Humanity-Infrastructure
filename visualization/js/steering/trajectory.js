// The trajectory of an engagement drawn as a lattice (kybernesis's record, the design of 2026-11-19 §4.4): markup that needs no page.
//
// The turns are columns from left to right, the blocks of the architecture rows in their order, grouped by stage; each cell is a
// block's entry at a turn, told by a glyph and a word as well as by a tone: computed (a method made it), observed, entered (a person
// filled it), amended, accepted (a review let it pass), held (a review refused it), a proposal awaiting review, the world (a block
// referred to and never in the record), or nothing yet. Two marks may be added to an entry: stale (an input it rested on has changed
// since, its outline dashed) and inherited (read from the parent branch, drawn faint). Above the lattice the branches are lines over
// the turns, each parting from its parent's line just before its first turn of its own; a dot where a branch holds entries of its own
// at a turn, a ring where it only reads its parent's.
//
// svgTrajectory(view, options) takes the view the API's science/steering.hpp answers ({branches: [{name, parent, from, reason, turns:
// [{turn, blocks: [{block, stage, boundary, world, entry?, act?, by?, family?, held?, inherited?, stale?, proposals?, …}]}]}], entries,
// head, verified}) and returns one SVG. Every cell carries data-block, data-turn, data-branch, data-entry and data-state, so that a
// page can open it; every mark carries its look in its attributes (for publication on options.ground, white unless stated) and a
// class (kt-…) by which a page's stylesheet may change it, by night among others.
//
// options: { branch: name (one band; else every branch, each a band), selected: {branch, turn, block}, turn: the chosen turn,
//            ahead: true (a column for the next turn, its cells empty, so that a page can fill there), ground, accent, key: false,
//            footer: false }

import { FONT, PALETTE, textWidth } from './layout.js';

// The states of a cell. Inks are the visualization layer's ten validated inks for lines (dynamics/charts.js), apart in every colour
// vision; a state is never told by its ink alone.
export const ENTRY_STATES = {
  computed: { glyph: '◆', word: 'computed', ink: '#1c8580', about: 'made by a method' },
  observed: { glyph: '◉', word: 'observed', ink: '#c18434', about: 'taken from outside the loop' },
  entered: { glyph: '✎', word: 'entered', ink: '#5469b4', about: 'filled by a person' },
  amended: { glyph: '±', word: 'amended', ink: '#7f5f96', about: 'a method’s value changed by a person' },
  accepted: { glyph: '✓', word: 'accepted', ink: '#617209', about: 'reviewed, and let pass' },
  held: { glyph: '✕', word: 'held', ink: '#b8577a', about: 'reviewed, and refused' },
  proposal: { glyph: '△', word: 'proposal', ink: '#917ca2', dash: '3 2', about: 'awaiting a review' },
  stale: { glyph: '↻', word: 'stale', ink: '#8c654f', dash: '4.5 2.5', about: 'an input has changed since' },
  inherited: { glyph: '↰', word: 'inherited', ink: '#6E5C78', about: 'read from the parent branch' },
  world: { glyph: '∿', word: 'the world', ink: '#6e9a65', dash: '5 3', about: 'referred to, never in the record' },
  empty: { glyph: '·', word: 'not yet', ink: '#8f8494', dash: '1.5 2.5', about: 'no entry' },
};
// Lines of branches after the main line's ink.
const BRANCH_INKS = ['#5469b4', '#1c8580', '#b8577a', '#c18434', '#617209', '#1f99c7', '#7f5f96'];

const esc = (s) => String(s ?? '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const f1 = (v) => (Math.round(v * 10) / 10).toString();
const SIZE = { block: 12, stage: 9.5, turn: 11, word: 11, line: 9, branch: 11, small: 9.5, key: 10, footer: 10 };

// A text cut to a width, with an ellipsis.
export function fit(text, width, size) {
  const s = String(text ?? '');
  if (textWidth(s, size) <= width) return s;
  let out = '';
  for (const ch of s) {
    if (textWidth(`${out}${ch}…`, size) > width) break;
    out += ch;
  }
  return `${out.trimEnd()}…`;
}

// What a cell of the view is: its state, and the marks added to it.
export function entryOf(cell) {
  if (!cell) return { state: 'empty', marks: [] };
  if (cell.world) return { state: 'world', marks: [] };
  const proposals = (cell.proposals || []).length;
  if (cell.entry === undefined || cell.entry === null) return { state: proposals ? 'proposal' : 'empty', marks: [] };
  const state = { method: 'computed', observe: 'observed', fill: 'entered', amend: 'amended' }[cell.act] || (cell.act === 'review' ? (cell.held ? 'held' : 'accepted') : 'entered');
  const marks = [];
  if (cell.stale) marks.push('stale');
  if (cell.inherited) marks.push('inherited');
  if (proposals) marks.push('proposal');
  return { state, marks };
}

// The last turn a branch shows, or -1.
const lastOf = (b) => (b.turns || []).reduce((m, t) => Math.max(m, t.turn), -1);
// Whether a branch holds an entry of its own at a turn.
const ownAt = (b, turn) => ((b.turns || []).find((t) => t.turn === turn)?.blocks || []).some((c) => c.entry !== undefined && c.entry !== null && !c.inherited);

// The lattice laid out: pure arithmetic, every text with its extent, so that checks can see that nothing overlaps.
export function layoutTrajectory(view, options = {}) {
  const all = view?.branches || [];
  const shown = options.branch ? all.filter((b) => b.name === options.branch) : all;
  const ahead = !!options.ahead;
  const ground = options.ground === undefined ? PALETTE.paper : options.ground;
  const accent = options.accent || '#9B72A6';
  // The rows: the blocks in the order the view gives them (the architecture's), from any turn of any branch.
  const rows = [];
  for (const b of all) for (const t of b.turns || []) for (const c of t.blocks || []) if (!rows.some((r) => r.block === c.block)) rows.push({ block: c.block, stage: c.stage, boundary: !!c.boundary, world: !!c.world });
  const lastAll = Math.max(-1, ...all.map(lastOf));
  const lastShown = Math.max(-1, ...shown.map(lastOf));
  const columns = Math.max(0, (ahead ? Math.max(lastAll, lastShown) + 2 : Math.max(lastAll, lastShown) + 1));

  const PAD = 18, colW = 132, gapX = 6, rowH = 30, gapY = 4, laneH = 19, bandHead = 26, headH = 24;
  const stageW = Math.max(textWidth('given', SIZE.stage) + 8, ...rows.map((r) => textWidth(r.stage, SIZE.stage) + 8));
  const blockW = Math.max(70, ...rows.map((r) => textWidth(r.block, SIZE.block) + 12));
  const branchW = Math.max(0, ...all.map((b) => textWidth(b.name, SIZE.branch) + 14));
  const labelW = Math.max(stageW + blockW, branchW + 10);
  const left = PAD + labelW;
  const colX = (k) => left + k * (colW + gapX);
  const mid = (k) => colX(k) + colW / 2;
  const texts = [], cells = [], lanes = [], rules = [], heads = [], bands = [];
  const text = (t) => {
    const w = t.w ?? textWidth(t.text, t.size, t.bold);
    const x0 = t.anchor === 'middle' ? t.x - w / 2 : t.anchor === 'end' ? t.x - w : t.x;
    texts.push({ ...t, box: { x: x0, y: t.y - t.size * 0.8, w, h: t.size } });
  };

  // The branches as lines over the turns, each parting from its parent's just before its first turn of its own.
  let y = PAD;
  if (all.length) {
    text({ text: 'branches', x: PAD, y: y + 9, size: SIZE.stage, fill: PALETTE.muted, italic: true, cls: 'kt-caption', owner: 'lanes' });
    y += 16;
  }
  const laneOf = new Map();
  all.forEach((b, k) => {
    const ly = y + k * laneH + laneH / 2;
    laneOf.set(b.name, ly);
    const ink = k === 0 && !b.parent ? PALETTE.ink : BRANCH_INKS[(k - (all[0]?.parent ? 0 : 1) + BRANCH_INKS.length) % BRANCH_INKS.length];
    const last = lastOf(b), from = Number(b.from || 0);
    const end = Math.max(last, b.parent ? from : 0);
    const parentY = b.parent && laneOf.has(b.parent) ? laneOf.get(b.parent) : null;
    let d;
    if (parentY === null) d = `M${f1(colX(0) - 2)} ${f1(ly)} H${f1(mid(Math.max(0, end)))}`;
    else {
      // It leaves its parent's line after the parent's last turn it reads, and runs on its own lane from its first turn.
      const xs = from === 0 ? colX(0) - 2 : colX(from) - gapX / 2 - 22, xe = xs + 20;
      const lead = from === 0 ? '' : ` H${f1(xs)}`;
      d = `M${f1(from === 0 ? xs : mid(from - 1))} ${f1(parentY)}${lead} C${f1(xs + 12)} ${f1(parentY)} ${f1(xe - 12)} ${f1(ly)} ${f1(xe)} ${f1(ly)} H${f1(Math.max(xe + 2, mid(end)))}`;
    }
    const dots = [];
    for (let turn = b.parent ? from : 0; turn <= end; turn++) dots.push({ x: mid(turn), y: ly, own: ownAt(b, turn), turn });
    const emphasis = shown.includes(b);
    lanes.push({ name: b.name, parent: b.parent || '', from, reason: b.reason || '', y: ly, ink, d, dots, emphasis, parentY });
    text({ text: b.name, x: PAD + labelW - 10, y: ly + 3.8, size: SIZE.branch, fill: emphasis ? PALETTE.ink : PALETTE.muted, anchor: 'end', italic: true, bold: emphasis, cls: 'kt-branch-name', owner: `lane:${b.name}` });
  });
  if (all.length) y += all.length * laneH + 8;

  // The turns as columns.
  const headY = y + 14;
  for (let k = 0; k < columns; k++) {
    const next = ahead && k === columns - 1;
    const chosen = options.turn !== undefined && options.turn !== null && Number(options.turn) === k;
    heads.push({ turn: k, x: colX(k), w: colW, y: y, h: headH, next, chosen });
    text({ text: next ? `turn ${k} · next` : `turn ${k}`, x: mid(k), y: headY, size: SIZE.turn, fill: chosen ? accent : next ? PALETTE.muted : PALETTE.ink, anchor: 'middle', bold: chosen, italic: next, cls: `kt-turn${chosen ? ' kt-chosen' : ''}`, owner: `head:${k}` });
  }
  y += headH;

  // A band for each branch shown: its name and whence it parted, then the blocks, row by row.
  for (const b of shown) {
    const top = y;
    const parted = b.parent ? `from ${b.parent} at turn ${b.from}${b.reason ? ` · ${b.reason}` : ''}` : 'the main line';
    text({ text: b.name, x: PAD, y: y + 16, size: SIZE.branch, fill: PALETTE.ink, italic: true, bold: true, cls: 'kt-band-name', owner: `band:${b.name}` });
    const nameW = textWidth(b.name, SIZE.branch, true);
    const room = Math.max(60, colX(Math.max(1, columns)) - (PAD + nameW + 10));
    text({ text: fit(parted, room, SIZE.small), x: PAD + nameW + 10, y: y + 16, size: SIZE.small, fill: PALETTE.muted, italic: true, cls: 'kt-band-parted', owner: `band:${b.name}` });
    y += bandHead;
    const last = lastOf(b);
    let stage = null;
    rows.forEach((r, i) => {
      const ry = y + i * (rowH + gapY);
      // Rows are grouped by stage; the boundaries, given from outside the loop, as a group of their own.
      const group = r.boundary ? 'given' : r.stage;
      if (group !== stage) {
        stage = group;
        if (i > 0) rules.push({ x1: PAD, x2: colX(Math.max(1, columns)) - gapX, y: ry - gapY / 2 });
        text({ text: group, x: PAD, y: ry + rowH / 2 + 3.5, size: SIZE.stage, fill: PALETTE.muted, italic: true, cls: 'kt-stage', owner: `row:${b.name}:${r.block}` });
      }
      text({ text: r.block, x: PAD + labelW - 10, y: ry + rowH / 2 + 4, size: SIZE.block, fill: r.world ? PALETTE.muted : PALETTE.ink, anchor: 'end', italic: r.boundary || r.world, cls: `kt-block${r.boundary ? ' kt-boundary' : ''}`, owner: `row:${b.name}:${r.block}` });
      for (let k = 0; k < columns; k++) {
        const c = k <= last ? (b.turns.find((t) => t.turn === k)?.blocks || []).find((x) => x.block === r.block) : (r.world ? { block: r.block, world: true } : null);
        const { state, marks } = entryOf(c || (r.world ? { world: true } : null));
        const x = colX(k);
        const S = ENTRY_STATES[state];
        const selected = !!options.selected && options.selected.branch === b.name && Number(options.selected.turn) === k && options.selected.block === r.block;
        const cell = { branch: b.name, turn: k, block: r.block, entry: c?.entry ?? null, state, marks, x, y: ry, w: colW, h: rowH, selected, ahead: k > last,
          by: c?.by || '', proposals: (c?.proposals || []).length, dash: marks.includes('stale') ? ENTRY_STATES.stale.dash : S.dash || null, ink: marks.includes('stale') ? ENTRY_STATES.stale.ink : S.ink };
        cells.push(cell);
        const owner = `cell:${b.name}:${k}:${r.block}`;
        text({ text: `${S.glyph} ${S.word}`, x: x + 7, y: ry + 12.5, size: SIZE.word, w: textWidth(`${S.glyph} ${S.word}`, SIZE.word), fill: PALETTE.ink, cls: 'kt-word', owner, glyph: S.glyph, glyphInk: S.ink, word: S.word });
        // The second line: the marks in words, else who made it.
        const second = marks.length
          ? marks.map((m) => (m === 'proposal' ? `${ENTRY_STATES.proposal.glyph} ${cell.proposals} proposal${cell.proposals > 1 ? 's' : ''}` : `${ENTRY_STATES[m].glyph} ${ENTRY_STATES[m].word}`)).join(' · ')
          : state === 'world' ? 'never in the record' : state === 'proposal' ? (c?.proposals?.[0]?.by ? `by ${c.proposals[0].by}` : 'awaiting a review') : state === 'empty' ? '' : c?.by ? `by ${c.by}` : '';
        if (second) text({ text: fit(second, colW - 14, SIZE.line), x: x + 7, y: ry + 24.5, size: SIZE.line, fill: marks.length ? (marks.includes('stale') ? ENTRY_STATES.stale.ink : PALETTE.muted) : PALETTE.muted, italic: !marks.length, cls: 'kt-line', owner });
      }
    });
    y += rows.length * (rowH + gapY) + 10;
    bands.push({ name: b.name, top, bottom: y - 10 });
  }
  if (!columns) {
    text({ text: 'No turn is kept yet.', x: left, y: y + 14, size: SIZE.small, fill: PALETTE.muted, italic: true, cls: 'kt-none', owner: 'none' });
    y += 24;
  }

  // The key: each state as a cell of its own, with what it means.
  const width = Math.max(colX(Math.max(1, columns)) - gapX + PAD, 640);
  const key = [];
  if (options.key !== false) {
    y += 6;
    text({ text: 'key', x: PAD, y: y + 9, size: SIZE.stage, fill: PALETTE.muted, italic: true, cls: 'kt-caption', owner: 'key' });
    y += 16;
    const itemW = 238, perRow = Math.max(1, Math.floor((width - 2 * PAD) / itemW));
    Object.entries(ENTRY_STATES).forEach(([state, S], i) => {
      const kx = PAD + (i % perRow) * itemW, ky = y + Math.floor(i / perRow) * 24;
      key.push({ state, x: kx, y: ky, w: 92, h: 18, ink: S.ink, dash: S.dash || null });
      text({ text: `${S.glyph} ${S.word}`, x: kx + 6, y: ky + 12.6, size: SIZE.key, fill: PALETTE.ink, cls: 'kt-key-word', owner: `key:${state}`, glyph: S.glyph, glyphInk: S.ink, word: S.word });
      text({ text: S.about, x: kx + 98, y: ky + 12.6, size: SIZE.small, fill: PALETTE.muted, italic: true, cls: 'kt-key-about', owner: `key:${state}:about` });
    });
    y += Math.ceil(Object.keys(ENTRY_STATES).length / perRow) * 24 + 4;
  }
  // What the record says of itself.
  if (options.footer !== false && view) {
    const v = view.verified || {};
    const said = v.holds ? `✓ the record verifies: every digest and the journal hold` : v.undecided ? `? whether the record verifies is undecided${v.reason ? `: ${v.reason}` : ''}` : `✕ the record does not verify${v.reason ? `: ${v.reason}` : ''}`;
    const head = view.head ? ` · journal head ${String(view.head).slice(0, 12)}…` : '';
    text({ text: fit(`${said} · ${view.entries ?? 0} entries${head}`, width - 2 * PAD, SIZE.footer), x: PAD, y: y + 14, size: SIZE.footer, fill: v.holds ? PALETTE.moss : PALETTE.rose, cls: 'kt-footer', owner: 'footer' });
    y += 22;
  }
  return { width: Math.ceil(width), height: Math.ceil(y + PAD - 6), ground, accent, rows, columns, cells, lanes, heads, rules, bands, texts, key, laneH };
}

function textMark(t) {
  const attrs = [`x="${f1(t.x)}"`, `y="${f1(t.y)}"`, `font-size="${t.size}"`, `fill="${t.fill}"`];
  if (t.anchor) attrs.push(`text-anchor="${t.anchor}"`);
  if (t.bold) attrs.push('font-weight="600"');
  if (t.italic) attrs.push('font-style="italic"');
  // A glyph keeps its state's ink beside a word in the page's.
  const body = t.glyph && t.word ? `<tspan class="kt-glyph" fill="${t.glyphInk}">${esc(t.glyph)}</tspan> ${esc(t.word)}` : esc(t.text);
  return `<text class="${t.cls || ''}" ${attrs.join(' ')}>${body}</text>`;
}

export function svgTrajectory(view, options = {}) {
  const L = layoutTrajectory(view, options);
  const P = PALETTE;
  const out = [];
  const label = `the trajectory of an engagement: ${L.columns} turn${L.columns === 1 ? '' : 's'}, ${(view?.branches || []).length} branch${(view?.branches || []).length === 1 ? '' : 'es'}`;
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="kt-trajectory" viewBox="0 0 ${L.width} ${L.height}" width="${L.width}" height="${L.height}" font-family="${esc(FONT)}" style="font-variant-numeric: lining-nums" role="img" aria-label="${esc(label)}">`);
  if (L.ground) out.push(`<rect class="kt-ground" x="0" y="0" width="${L.width}" height="${L.height}" fill="${L.ground}"/>`);

  // The chosen turn, a wash down its column.
  for (const h of L.heads) {
    if (h.chosen) {
      const bottom = Math.max(h.y + h.h, ...L.cells.filter((c) => c.turn === h.turn).map((c) => c.y + c.h));
      out.push(`<rect class="kt-chosen-turn" x="${f1(h.x - 3)}" y="${f1(h.y)}" width="${f1(h.w + 6)}" height="${f1(bottom - h.y + 4)}" rx="3" fill="${L.accent}" fill-opacity="0.08"/>`);
    }
    if (h.next) out.push(`<line class="kt-next-rule" x1="${f1(h.x - 3)}" y1="${f1(h.y + 2)}" x2="${f1(h.x - 3)}" y2="${f1(h.y + h.h + 4)}" stroke="${P.line}" stroke-width="0.8" stroke-dasharray="2 2"/>`);
  }
  for (const r of L.rules) out.push(`<line class="kt-rule" x1="${f1(r.x1)}" y1="${f1(r.y)}" x2="${f1(r.x2)}" y2="${f1(r.y)}" stroke="${P.line}" stroke-width="0.6"/>`);

  // The branches' lines.
  for (const l of L.lanes) {
    out.push(`<g class="kt-lane${l.emphasis ? ' kt-shown' : ''}" data-branch="${esc(l.name)}" data-parent="${esc(l.parent)}" data-from="${l.from}">`);
    if (l.reason) out.push(`<title>${esc(`${l.name}: ${l.reason}`)}</title>`);
    out.push(`<path d="${l.d}" fill="none" stroke="${l.ink}" stroke-width="${l.emphasis ? 2 : 1.2}" stroke-opacity="${l.emphasis ? 1 : 0.7}" stroke-linecap="round"/>`);
    for (const d of l.dots) {
      out.push(d.own
        ? `<circle class="kt-dot kt-own" cx="${f1(d.x)}" cy="${f1(d.y)}" r="3.6" fill="${l.ink}" data-turn="${d.turn}"/>`
        : `<circle class="kt-dot kt-read" cx="${f1(d.x)}" cy="${f1(d.y)}" r="3.2" fill="${L.ground || P.paper}" stroke="${l.ink}" stroke-width="1.1" data-turn="${d.turn}"/>`);
    }
    out.push('</g>');
  }

  // The cells.
  for (const c of L.cells) {
    const cls = `kt-cell kt-${c.state}${c.marks.map((m) => ` kt-mark-${m}`).join('')}${c.selected ? ' kt-selected' : ''}${c.ahead ? ' kt-ahead' : ''}`;
    const faint = c.marks.includes('inherited') ? ' opacity="0.55"' : '';
    const open = c.state === 'world' || c.state === 'empty' || c.state === 'proposal';
    out.push(`<g class="${cls}" data-block="${esc(c.block)}" data-turn="${c.turn}" data-branch="${esc(c.branch)}" data-entry="${c.entry ?? ''}" data-state="${c.state}"${faint}>`);
    out.push(`<title>${esc(`${c.block}, turn ${c.turn}, ${c.branch}: ${ENTRY_STATES[c.state].word}${c.marks.length ? ` (${c.marks.join(', ')})` : ''}${c.entry !== null ? `, entry ${c.entry}` : ''}${c.by ? `, by ${c.by}` : ''}`)}</title>`);
    out.push(`<rect class="kt-box" x="${f1(c.x)}" y="${f1(c.y)}" width="${f1(c.w)}" height="${f1(c.h)}" rx="2.5" fill="${open ? (L.ground || 'none') : c.ink}" fill-opacity="${open ? 1 : 0.11}" stroke="${c.ink}" stroke-width="${c.dash ? 1 : 0.8}" stroke-opacity="${c.state === 'empty' ? 0.55 : 0.9}"${c.dash ? ` stroke-dasharray="${c.dash}"` : ''}/>`);
    out.push(`<rect class="kt-edge" x="${f1(c.x)}" y="${f1(c.y + 1)}" width="2.2" height="${f1(c.h - 2)}" fill="${c.ink}" fill-opacity="${open ? 0 : 0.85}"/>`);
    out.push('</g>');
    if (c.selected) out.push(`<rect class="kt-ring" x="${f1(c.x - 2.5)}" y="${f1(c.y - 2.5)}" width="${f1(c.w + 5)}" height="${f1(c.h + 5)}" rx="4" fill="none" stroke="${L.accent}" stroke-width="2" pointer-events="none"/>`);
  }

  // The key's samples.
  for (const k of L.key) {
    const open = k.state === 'world' || k.state === 'empty' || k.state === 'proposal';
    out.push(`<rect class="kt-key kt-${k.state}" x="${f1(k.x)}" y="${f1(k.y)}" width="${k.w}" height="${k.h}" rx="2" fill="${open ? (L.ground || 'none') : k.ink}" fill-opacity="${open ? 1 : 0.11}" stroke="${k.ink}" stroke-width="0.8"${k.dash ? ` stroke-dasharray="${k.dash}"` : ''}${k.state === 'inherited' ? ' opacity="0.6"' : ''}/>`);
  }

  // Every text last; a cell's texts within its group's reach for a page's clicks (pointer-events none, so a click falls to the cell).
  for (const t of L.texts) out.push(textMark(t).replace('<text ', '<text pointer-events="none" '));
  out.push('</svg>');
  return out.join('\n');
}
