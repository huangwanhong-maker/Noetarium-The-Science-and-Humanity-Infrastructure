// A transfer drawn as two systems facing each other (kybernesis's transfer, the design of 2026-11-19 §6.3): markup that needs no page.
//
// A practice is carried from a source, where it is known to work, to a target, read in the source's terms by a lens. The source
// stands at the left and the target at the right; each support factor is a plank between them, laid when the target is believed to
// meet it (clear), half laid when that is arguable, the part laid as far as the lower probability and a lighter part on to the upper,
// with the interval marked; missing when it is untenable (an outline over the gap); dotted when it is undecided. Whether a factor is
// sufficient or necessary for the square to commute is said in words beside it. Above, the standing of "it will work here" with its
// interval on a scale of probability; beneath, each reading's line: shown, open or refuted, and why.
//
// svgTransfer(assessment, options) takes the result the API's science/steering.hpp answers for a transfer ({practice, tolerance,
// standing, low, high, by_reading: [text], factors: [{name, source, standing, low, high, sufficient, necessary, words}], squares:
// [{reading, relations, worst, …}]}) and returns one SVG. A standing is told by a glyph and a word as well as a tone: clear ✓,
// arguable ◐, untenable ✕, undecided ?. Marks carry their look in attributes (for publication on options.ground, white unless
// stated) and a class (kx-…) by which a page's stylesheet may change it.
//
// options: { source: 'Miyako', target: 'Freetown', title, width: 880, ground, sizes }

import { FONT, PALETTE, textWidth } from './layout.js';

export const STANDINGS = {
  clear: { glyph: '✓', word: 'clear', ink: '#617209', said: 'laid' },
  arguable: { glyph: '◐', word: 'arguable', ink: '#c18434', said: 'half laid' },
  untenable: { glyph: '✕', word: 'untenable', ink: '#b8577a', said: 'missing' },
  undecided: { glyph: '?', word: 'undecided', ink: '#8f8494', said: 'not known' },
};
export const READING_STATUS = {
  shown: { glyph: '✓', word: 'shown', ink: '#617209' },
  open: { glyph: '◐', word: 'open', ink: '#c18434' },
  refuted: { glyph: '✕', word: 'refuted', ink: '#b8577a' },
};

const esc = (s) => String(s ?? '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const f1 = (v) => (Math.round(v * 10) / 10).toString();
const num = (v) => (v === null || v === undefined || !Number.isFinite(Number(v)) ? '—' : String(+Number(v).toFixed(2)));
const SIZE = { title: 17, system: 18, caption: 10.5, factor: 12.5, tag: 9.5, small: 10, standing: 13, reading: 12 };

// Words broken into lines no wider than a width.
export function wrap(text, width, size) {
  const lines = [];
  let line = '';
  for (const word of String(text ?? '').split(/\s+/).filter(Boolean)) {
    const next = line ? `${line} ${word}` : word;
    if (line && textWidth(next, size) > width) { lines.push(line); line = word; } else line = next;
  }
  if (line) lines.push(line);
  return lines;
}

// Each reading's line read from the assessment's words ("under R (S): shown|open|refuted, …"), with its square beside it.
export function readingLines(a) {
  return (a?.by_reading || []).map((text, k) => {
    const m = String(text).match(/^under (.+) \(([^()]*(?:\([^()]*\)[^()]*)*)\): (shown|open|refuted),?\s*(.*)$/);
    const square = (a.squares || [])[k] || null;
    return { name: m ? m[1] : square?.reading || `reading ${k + 1}`, source: m ? m[2] : '', status: m ? m[3] : 'open', why: m ? m[4] : String(text), square };
  });
}

// What a factor is to the square, in words of our own (the foundation's words also list the states).
export function factorRole(f) {
  return f.sufficient && f.necessary ? 'the square commutes exactly where it holds' : f.sufficient ? 'where it holds, the square commutes'
    : f.necessary ? 'where it fails, the square does not commute' : 'the square does not follow it alone';
}

export function layoutTransfer(a, options = {}) {
  const W = options.width || 880, PAD = 22;
  const S = { ...SIZE, ...(options.sizes || {}) };
  const ground = options.ground === undefined ? PALETTE.paper : options.ground;
  const sourceName = options.source || a?.source?.name || 'the source';
  const targetName = options.target || a?.target?.name || 'the target';
  const texts = [];
  const text = (t) => {
    const w = textWidth(t.text, t.size, t.bold);
    const x0 = t.anchor === 'middle' ? t.x - w / 2 : t.anchor === 'end' ? t.x - w : t.x;
    texts.push({ ...t, box: { x: x0, y: t.y - t.size * 0.8, w, h: t.size } });
  };
  let y = PAD;
  // The title and the standing of "it will work here".
  const title = options.title || `${a?.practice || 'the practice'}, from ${sourceName} to ${targetName}`;
  for (const line of wrap(title, W - 2 * PAD, S.title)) { text({ text: line, x: PAD, y: y + 14, size: S.title, fill: PALETTE.ink, italic: true, cls: 'kx-title', owner: 'title' }); y += 22; }
  y += 4;
  const st = STANDINGS[a?.standing] || STANDINGS.undecided;
  const headline = { glyph: st.glyph, word: st.word, ink: st.ink };
  text({ text: `${st.glyph} ${st.word}`, x: PAD, y: y + 15, size: S.standing + 2, fill: PALETTE.ink, bold: true, cls: 'kx-standing', owner: 'standing', glyph: st.glyph, glyphInk: st.ink, word: st.word });
  const lead = PAD + textWidth(`${st.glyph} ${st.word}`, S.standing + 2, true) + 10;
  text({ text: `it will work here · probability ${num(a?.low)} to ${num(a?.high)} that the square commutes where ${targetName} is believed to be`, x: lead, y: y + 15, size: S.small + 0.5, fill: PALETTE.muted, italic: true, cls: 'kx-standing-said', owner: 'standing-said' });
  y += 26;
  // The scale of probability with the interval.
  const scale = { x: PAD, y: y + 8, w: Math.min(420, W - 2 * PAD), low: Number(a?.low ?? 0), high: Number(a?.high ?? 0), ink: st.ink };
  for (const v of [0, 0.25, 0.5, 0.75, 1]) text({ text: String(v), x: scale.x + v * scale.w, y: scale.y + 20, size: S.tag, fill: PALETTE.muted, anchor: 'middle', cls: 'kx-tick', owner: `tick:${v}` });
  text({ text: `tolerance ${num(a?.tolerance)}`, x: scale.x + scale.w + 14, y: scale.y + 4, size: S.tag, fill: PALETTE.muted, italic: true, cls: 'kx-tolerance', owner: 'tolerance' });
  y += 38;

  // The two systems, and the planks between them.
  const bankW = 168, gap = -3;   // the planks rest on both banks
  const left = { x: PAD, w: bankW }, right = { x: W - PAD - bankW, w: bankW };
  const span = { x0: left.x + bankW + gap, x1: right.x - gap };
  const spanW = span.x1 - span.x0;
  const factors = a?.factors || [];
  const plankH = 20, rowH = 86;
  const top = y + 8;
  const cut = (s, width, size) => { const first = wrap(s, width, size)[0] || ''; return first === s ? s : `${first.replace(/[,;:·]?\s*$/, '')}…`; };
  const planks = factors.map((f, k) => {
    const py = top + 30 + k * rowH;
    const fs = STANDINGS[f.standing] || STANDINGS.undecided;
    const lo = Math.max(0, Math.min(1, Number(f.low ?? 0))), hi = Math.max(lo, Math.min(1, Number(f.high ?? lo)));
    const tags = [f.sufficient ? 'sufficient' : null, f.necessary ? 'necessary' : null].filter(Boolean);
    const tagW = tags.length ? textWidth(tags.join(' · '), S.tag, true) + 12 : 0;
    text({ text: cut(f.name, spanW - tagW - 24, S.factor), x: span.x0 + 12, y: py - 7, size: S.factor, fill: PALETTE.ink, cls: 'kx-factor-name', owner: `plank:${k}` });
    if (tags.length) text({ text: tags.join(' · '), x: span.x1 - 12, y: py - 7, size: S.tag, fill: PALETTE.ink, anchor: 'end', bold: true, cls: 'kx-tags', owner: `plank:${k}:tags` });
    // Beneath the plank: its standing in words with its interval, whence it is known, and what it is to the square.
    const said = `${fs.word} · ${fs.said}${f.standing === 'clear' ? '' : ` · ${num(lo)} to ${num(hi)}`}`;
    const saidW = textWidth(`${fs.glyph} ${said}`, S.small + 1, true);
    text({ text: `${fs.glyph} ${said}`, x: span.x0 + 12, y: py + plankH + 15, size: S.small + 1, fill: PALETTE.ink, bold: true, cls: 'kx-plank-said', owner: `plank:${k}:said`, glyph: fs.glyph, glyphInk: fs.ink, word: said });
    text({ text: cut(f.source || 'declared', spanW - saidW - 34, S.small), x: span.x0 + saidW + 22, y: py + plankH + 15, size: S.small, fill: PALETTE.muted, italic: true, cls: 'kx-factor-source', owner: `plank:${k}:source` });
    text({ text: cut(factorRole(f), spanW - 24, S.tag), x: span.x0 + 12, y: py + plankH + 29, size: S.tag, fill: PALETTE.muted, italic: true, cls: 'kx-factor-role', owner: `plank:${k}:role` });
    return { name: f.name, standing: f.standing in STANDINGS ? f.standing : 'undecided', lo, hi, x: span.x0, y: py, w: spanW, h: plankH, ink: fs.ink, tags, title: f.words || '' };
  });
  const bottom = top + Math.max(1, factors.length) * rowH + 18;
  if (!factors.length) text({ text: 'No support factor is named: nothing is laid between them.', x: span.x0 + 4, y: top + 40, size: S.small, fill: PALETTE.muted, italic: true, cls: 'kx-none', owner: 'none' });
  const banks = [
    { side: 'source', name: sourceName, caption: 'the source, where it works', x: left.x, y: top, w: bankW, h: bottom - top },
    { side: 'target', name: targetName, caption: 'the target, read in the source’s terms', x: right.x, y: top, w: bankW, h: bottom - top },
  ];
  for (const b of banks) {
    const cx = b.x + b.w / 2;
    const nameLines = wrap(b.name, b.w - 16, S.system);
    nameLines.forEach((line, i) => text({ text: line, x: cx, y: b.y + 28 + i * 22, size: S.system, fill: PALETTE.ink, anchor: 'middle', italic: true, cls: 'kx-system-name', owner: `bank:${b.side}` }));
    wrap(b.caption, b.w - 20, S.caption).forEach((line, i) => text({ text: line, x: cx, y: b.y + 34 + nameLines.length * 22 + i * 13, size: S.caption, fill: PALETTE.muted, anchor: 'middle', italic: true, cls: 'kx-system-caption', owner: `bank:${b.side}` }));
  }
  y = bottom + 18;

  // Each reading's line.
  const readings = readingLines(a);
  if (readings.length) {
    text({ text: 'under each reading', x: PAD, y: y + 9, size: S.tag, fill: PALETTE.muted, italic: true, cls: 'kx-caption', owner: 'readings' });
    y += 18;
  }
  const lines = readings.map((r, k) => {
    const rs = READING_STATUS[r.status] || READING_STATUS.open;
    const ly = y + 14;
    text({ text: `${rs.glyph} ${rs.word}`, x: PAD + 6, y: ly, size: S.reading, fill: PALETTE.ink, bold: true, cls: 'kx-reading-status', owner: `reading:${k}`, glyph: rs.glyph, glyphInk: rs.ink, word: rs.word });
    const nameX = PAD + 92;
    const named = `${r.name}${r.source ? ` (${r.source})` : ''}`;
    text({ text: named, x: nameX, y: ly, size: S.reading, fill: PALETTE.ink, italic: true, cls: 'kx-reading-name', owner: `reading:${k}` });
    const q = r.square;
    const relation = q ? (q.relations?.holds ? 'the square commutes in relations' : q.relations?.undecided ? 'whether the square commutes in relations is undecided' : 'the square does not commute in relations') : '';
    const why = [r.why, relation, q && Number.isFinite(q.worst) ? `the widest gap ${num(q.worst)} against a tolerance of ${num(a?.tolerance)}` : ''].filter(Boolean).join(' · ');
    const whyLines = wrap(why, W - nameX - PAD, S.small).slice(0, 2);
    whyLines.forEach((line, i) => text({ text: line, x: nameX, y: ly + 15 + i * 13, size: S.small, fill: PALETTE.muted, cls: 'kx-reading-why', owner: `reading:${k}:why` }));
    const h = 22 + whyLines.length * 13;
    const out = { name: r.name, status: r.status in READING_STATUS ? r.status : 'open', x: PAD, y, w: W - 2 * PAD, h, ink: rs.ink };
    y += h + 6;
    return out;
  });
  return { width: W, height: Math.ceil(y + PAD - 6), ground, headline, scale, banks, planks, lines, span, texts };
}

function textMark(t) {
  const attrs = [`x="${f1(t.x)}"`, `y="${f1(t.y)}"`, `font-size="${t.size}"`, `fill="${t.fill}"`];
  if (t.anchor) attrs.push(`text-anchor="${t.anchor}"`);
  if (t.bold) attrs.push('font-weight="600"');
  if (t.italic) attrs.push('font-style="italic"');
  const body = t.glyph && t.word ? `<tspan class="kx-glyph" fill="${t.glyphInk}">${esc(t.glyph)}</tspan> ${esc(t.word)}` : esc(t.text);
  return `<text class="${t.cls || ''}" ${attrs.join(' ')}>${body}</text>`;
}

export function svgTransfer(assessment, options = {}) {
  const L = layoutTransfer(assessment, options);
  const P = PALETTE;
  const out = [];
  const label = `a transfer from ${L.banks[0].name} to ${L.banks[1].name}: it will work here, ${L.headline.word}`;
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="kx-transfer" viewBox="0 0 ${L.width} ${L.height}" width="${L.width}" height="${L.height}" font-family="${esc(FONT)}" style="font-variant-numeric: lining-nums" role="img" aria-label="${esc(label)}">`);
  if (L.ground) out.push(`<rect class="kx-ground" x="0" y="0" width="${L.width}" height="${L.height}" fill="${L.ground}"/>`);
  out.push('<defs><pattern id="kx-hatch" width="5" height="5" patternUnits="userSpaceOnUse" patternTransform="rotate(45)"><line x1="0" y1="0" x2="0" y2="5" stroke="#c18434" stroke-width="1.4" stroke-opacity="0.55"/></pattern></defs>');

  // The scale of "it will work here".
  const s = L.scale;
  out.push(`<g class="kx-scale" data-low="${num(s.low)}" data-high="${num(s.high)}">`);
  out.push(`<line x1="${f1(s.x)}" y1="${f1(s.y)}" x2="${f1(s.x + s.w)}" y2="${f1(s.y)}" stroke="${P.line}" stroke-width="1"/>`);
  for (const v of [0, 0.25, 0.5, 0.75, 1]) out.push(`<line x1="${f1(s.x + v * s.w)}" y1="${f1(s.y - 3)}" x2="${f1(s.x + v * s.w)}" y2="${f1(s.y + 3)}" stroke="${P.line}" stroke-width="0.8"/>`);
  out.push(`<rect class="kx-interval" x="${f1(s.x + s.low * s.w)}" y="${f1(s.y - 4)}" width="${f1(Math.max(2.5, (s.high - s.low) * s.w))}" height="8" rx="2" fill="${s.ink}" fill-opacity="0.75"/>`);
  out.push('</g>');

  // The two systems facing each other.
  for (const b of L.banks) {
    out.push(`<g class="kx-bank kx-${b.side}" data-side="${b.side}"><rect x="${f1(b.x)}" y="${f1(b.y)}" width="${f1(b.w)}" height="${f1(b.h)}" rx="3" fill="${P.ink}" fill-opacity="0.035" stroke="${P.ink}" stroke-opacity="0.55" stroke-width="0.9"/>`
      + `<line x1="${f1(b.side === 'source' ? b.x + b.w : b.x)}" y1="${f1(b.y + 6)}" x2="${f1(b.side === 'source' ? b.x + b.w : b.x)}" y2="${f1(b.y + b.h - 6)}" stroke="${P.ink}" stroke-width="1.6" stroke-opacity="0.7"/></g>`);
  }

  // What lies between them, beneath the planks: a few gentle lines of water.
  const b0 = L.banks[0];
  if (L.span.x1 > L.span.x0 + 40) {
    for (let k = 0; k < 4; k++) {
      const wy = b0.y + 18 + k * ((b0.h - 30) / 3), x0 = L.span.x0 + 6, x1 = L.span.x1 - 6;
      let d = `M${f1(x0)} ${f1(wy)}`;
      for (let x = x0; x < x1; x += 24) d += ` q6 -2.6 12 0 t12 0`;
      out.push(`<path class="kx-water" d="${d}" fill="none" stroke="#7864B0" stroke-opacity="0.16" stroke-width="0.8"/>`);
    }
  }

  // The planks.
  for (const p of L.planks) {
    out.push(`<g class="kx-plank kx-${p.standing}" data-factor="${esc(p.name)}" data-standing="${p.standing}" data-low="${num(p.lo)}" data-high="${num(p.hi)}">`);
    if (p.title) out.push(`<title>${esc(p.title)}</title>`);
    const { x, y, w, h, ink } = p;
    // The gap beneath, always outlined, so that a missing plank is seen as missing.
    out.push(`<rect class="kx-gap" x="${f1(x)}" y="${f1(y)}" width="${f1(w)}" height="${h}" rx="2" fill="none" stroke="${ink}" stroke-width="${p.standing === 'clear' ? 0 : 1}" stroke-dasharray="${p.standing === 'undecided' ? '1.5 2.5' : '5 3'}" stroke-opacity="0.85"/>`);
    if (p.standing === 'clear' || p.standing === 'arguable') {
      const solid = p.standing === 'clear' ? w : p.lo * w;
      if (solid > 0) {
        out.push(`<rect class="kx-laid" x="${f1(x)}" y="${f1(y)}" width="${f1(solid)}" height="${h}" rx="2" fill="${ink}" fill-opacity="0.22" stroke="${ink}" stroke-width="1"/>`);
        // Boards, at even steps, so that a plank looks laid.
        for (let bx = x + 26; bx < x + solid - 6; bx += 26) out.push(`<line class="kx-board" x1="${f1(bx)}" y1="${f1(y + 3)}" x2="${f1(bx)}" y2="${f1(y + h - 3)}" stroke="${ink}" stroke-opacity="0.35" stroke-width="0.7"/>`);
      }
      if (p.standing === 'arguable' && p.hi > p.lo) {
        out.push(`<rect class="kx-maybe" x="${f1(x + p.lo * w)}" y="${f1(y + 2)}" width="${f1((p.hi - p.lo) * w)}" height="${h - 4}" fill="url(#kx-hatch)" stroke="none"/>`);
      }
      if (p.standing === 'arguable') {
        for (const v of [p.lo, p.hi]) out.push(`<line class="kx-bound" x1="${f1(x + v * w)}" y1="${f1(y - 3)}" x2="${f1(x + v * w)}" y2="${f1(y + h + 3)}" stroke="${ink}" stroke-width="1.3"/>`);
      }
    }
    out.push('</g>');
  }

  // Each reading's line.
  for (const l of L.lines) out.push(`<g class="kx-reading kx-${l.status}" data-reading="${esc(l.name)}" data-status="${l.status}"><line x1="${f1(l.x)}" y1="${f1(l.y)}" x2="${f1(l.x + l.w)}" y2="${f1(l.y)}" stroke="${P.line}" stroke-width="0.6"/><rect x="${f1(l.x)}" y="${f1(l.y + 4)}" width="2.4" height="${f1(l.h - 6)}" fill="${l.ink}"/></g>`);

  for (const t of L.texts) out.push(textMark(t));
  out.push('</svg>');
  return out.join('\n');
}
