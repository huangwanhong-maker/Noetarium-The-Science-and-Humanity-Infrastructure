// What the drawings of multilateral practice share (ADR 0132): inks, glyphs and words for kinds, layers, states and standings, and a
// tie's status read from a record as the diplomacy domain reads it (present when recorded, absent when recorded absent or covered by a
// declared design, not observed otherwise), so that a page can mark every pair without asking the engine again.

import { FONT, textWidth } from '../steering/layout.js';

export { FONT, textWidth };

export const PALETTE = {
  ink: '#2E2233', muted: '#6E5C78', faint: '#B9AAC0', line: '#D8CCDD', paper: '#FFFFFF',
  orchid: '#8E4FA0',   // what is chosen or in the hand
  laurel: '#5E7F4E',   // what is shared
  rose: '#B8577A', sand: '#A0784A',
};

// Inks for the layers of relations: the visualization layer's validated inks for lines (dynamics/charts.js), each with a dash, so that
// a layer is never told by its ink alone.
export const LAYER_INKS = ['#5469b4', '#1c8580', '#b8577a', '#c18434', '#617209', '#1f99c7', '#7f5f96', '#8c654f'];
export const LAYER_DASHES = ['', '7 3', '2 3', '9 3 2 3', '4 2', '1 2.5', '12 3', '5 2 1 2'];

// The kinds of participant, each a glyph and a word.
export const KINDS = {
  state: { glyph: '●', word: 'state' }, organisation: { glyph: '◆', word: 'organisation' }, city: { glyph: '■', word: 'city' },
  community: { glyph: '▲', word: 'community' }, person: { glyph: '♦', word: 'person' }, firm: { glyph: '▼', word: 'firm' },
  'AI system': { glyph: '⬡', word: 'AI system' }, dataset: { glyph: '▤', word: 'dataset' }, forum: { glyph: '⌂', word: 'forum' },
  programme: { glyph: '○', word: 'programme' }, fund: { glyph: '◎', word: 'fund' },
};
export const kindOf = (k) => KINDS[k] || { glyph: '◇', word: k || 'participant' };

// The status of a pair, and of a commitment, and the standing of a benefit: a glyph and a word each.
export const STATUS = {
  present: { glyph: '●', word: 'present' }, absent: { glyph: '×', word: 'absent' }, 'not observed': { glyph: '?', word: 'not observed' },
};
export const STATES = {
  honoured: { glyph: '✓', word: 'honoured', ink: '#5E7F4E' }, 'honoured late': { glyph: '✓', word: 'honoured late', ink: '#A0784A' },
  'in progress': { glyph: '◔', word: 'in progress', ink: '#5469b4' }, overdue: { glyph: '✕', word: 'overdue', ink: '#B8577A' },
  unreported: { glyph: '?', word: 'unreported', ink: '#6E5C78', dash: '2 2' },
};
export const STANDINGS = {
  clear: { glyph: '●', word: 'clear', ink: '#5E7F4E' }, arguable: { glyph: '◐', word: 'arguable', ink: '#A0784A' },
  untenable: { glyph: '○', word: 'untenable', ink: '#B8577A' }, undecided: { glyph: '?', word: 'undecided', ink: '#6E5C78' },
};
export const VERDICT = { holds: { glyph: '✓', word: 'holds' }, fails: { glyph: '✕', word: 'fails' }, undecided: { glyph: '◐', word: 'undecided' } };

export const esc = (s) => String(s ?? '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
export const f1 = (v) => (Math.round(v * 10) / 10).toString();

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

// A number for reading: three significant figures, thousands grouped.
export function figure(v) {
  if (!Number.isFinite(v)) return '—';
  const a = Math.abs(v);
  if (a >= 1e9) return `${(v / 1e9).toPrecision(3)} bn`;
  if (a >= 1e6) return `${(v / 1e6).toPrecision(3)} m`;
  if (a >= 1000) return Math.round(v).toLocaleString('en-GB');
  if (a >= 1) return String(Number(v.toPrecision(3)));
  return a === 0 ? '0' : String(Number(v.toPrecision(2)));
}

// ---- A tie's status, as diplomacy::relations reads it ------------------------------------------------------------------------------

export const holdsOn = (since, until, at) => !at || ((!since || since <= at) && (!until || at < until));

export function status(record, type, a, b, at = '') {
  let recordedAbsent = false;
  for (const t of record.ties || []) {
    if (t.type !== type || t.from !== a || t.to !== b || !holdsOn(t.since, t.until, at)) continue;
    if (!t.absent) return 'present';
    recordedAbsent = true;
  }
  if (recordedAbsent) return 'absent';
  for (const o of record.observations || []) {
    if (o.type === type && holdsOn(o.since, o.until, at) && (o.covered || []).includes(a) && (o.covered || []).includes(b)) return 'absent';
  }
  return 'not observed';
}

// The ordered pairs of a type by status, as the engine's layers count them.
export function layerCounts(record, type, at = '') {
  const ids = (record.participants || []).map((p) => p.id);
  const out = { present: 0, absent: 0, 'not observed': 0 };
  for (const a of ids) for (const b of ids) if (a !== b) out[status(record, type, a, b, at)]++;
  return out;
}

// The ink and dash of each declared type, in the record's order.
export function layerLooks(record) {
  const looks = {};
  (record.types || []).forEach((t, i) => { looks[t.name] = { ink: LAYER_INKS[i % LAYER_INKS.length], dash: LAYER_DASHES[i % LAYER_DASHES.length] }; });
  return looks;
}

// Tags balanced: every element opened is closed in order.
export function balanced(svg) {
  const stack = [];
  for (const m of svg.matchAll(/<(\/?)([A-Za-z][\w:-]*)[^>]*?(\/?)>/g)) {
    if (m[3] === '/') continue;
    if (m[1] === '/') { if (stack.pop() !== m[2]) return false; } else stack.push(m[2]);
  }
  return stack.length === 0;
}
