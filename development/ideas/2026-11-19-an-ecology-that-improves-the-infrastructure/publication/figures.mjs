// The figures of "An ecology that improves the infrastructure", written as SVG.
//
//   node figures.mjs          writes every figure beside this file
//   node figures.mjs 3 5      writes figures 3 and 5 only
//
// Conventions (shared with the figures of 2026-11-18): a white ground, Source Serif 4, formal nouns
// and notation only, no prose inside a figure; captions are in README.md. render.mjs makes the PDFs
// and PNGs.

import { writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';

const here = dirname(fileURLToPath(import.meta.url));

const C = {
  ink: '#2E2233', ink2: '#3F2D44', line: '#4A3A52', mute: '#6E5C78', soft: '#B9A9C2',
  lilac: '#7864B0', orchid: '#9C5590', rose: '#B8577F', moss: '#557A5C', sand: '#A1845A',
  tLilac: '#F1EDF6', tOrchid: '#FAF2F7', tRose: '#F6E4EC', tSand: '#F7F2EA', tMoss: '#EEF4EF',
  white: '#FFFFFF',
};

const marker = (id, color) =>
  `<marker id="${id}" viewBox="0 0 10 10" refX="9" refY="5" markerWidth="7" markerHeight="7" orient="auto-start-reverse"><path d="M0,1 L10,5 L0,9 z" fill="${color}"/></marker>`;

const style = `
      text { font-family: 'Source Serif 4', Georgia, 'Cambria Math', 'STIX Two Text', serif; fill: ${C.ink}; }
      .h { font-size: 15px; font-weight: 600; text-anchor: middle; }
      .hl { font-size: 15px; font-weight: 600; }
      .m { font-size: 13px; text-anchor: middle; }
      .ml { font-size: 13px; }
      .s { font-size: 11.5px; fill: ${C.ink2}; text-anchor: middle; }
      .sl { font-size: 11.5px; fill: ${C.ink2}; }
      .i { font-size: 11.5px; font-style: italic; fill: ${C.mute}; text-anchor: middle; }
      .il { font-size: 11.5px; font-style: italic; fill: ${C.mute}; }
      .lab { font-size: 11px; font-style: italic; fill: #5E4C68; }
      .labm { font-size: 11px; font-style: italic; fill: #5E4C68; text-anchor: middle; }
      .labe { font-size: 11px; font-style: italic; fill: #5E4C68; text-anchor: end; }
      .tag { font-size: 11px; letter-spacing: 1.2px; fill: ${C.rose}; font-weight: 600; text-anchor: middle; }
      .tagl { font-size: 11px; letter-spacing: 1.2px; fill: ${C.rose}; font-weight: 600; }
      .pl { font-size: 15px; font-weight: 600; fill: ${C.mute}; }
      .num { font-size: 11px; fill: ${C.ink2}; text-anchor: middle; }
      .ax { font-size: 11px; fill: ${C.mute}; text-anchor: middle; }
      .axe { font-size: 11px; fill: ${C.mute}; text-anchor: end; }
      .axl { font-size: 11px; fill: ${C.mute}; }`;

function frame(w, h, label, body) {
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" width="${w}" height="${h}" role="img" aria-label="${label}">
  <defs>
    ${marker('ink', C.line)}
    ${marker('lilac', C.lilac)}
    ${marker('orchid', C.orchid)}
    ${marker('rose', C.rose)}
    ${marker('moss', C.moss)}
    ${marker('sand', C.sand)}
    ${marker('mute', C.mute)}
    <style>${style}
    </style>
  </defs>
  <rect width="${w}" height="${h}" fill="#FFFFFF"/>
${body}
</svg>
`;
}

const T = (x, y, s, cls = 'm', extra = '') => `  <text class="${cls}" x="${x}" y="${y}"${extra ? ' ' + extra : ''}>${s}</text>`;

function R(x, y, w, h, o = {}) {
  const { fill = C.white, stroke = C.ink, sw = 1, rx = 3, dash = '' } = o;
  return `  <rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${rx}" fill="${fill}" stroke="${stroke}" stroke-width="${sw}"${dash ? ` stroke-dasharray="${dash}"` : ''}/>`;
}

// A box with one or two centred lines of text.
function B(x, y, w, h, title, sub = '', o = {}) {
  const cy = y + h / 2;
  const out = [R(x, y, w, h, o)];
  if (sub) {
    out.push(T(x + w / 2, cy - 2, title, o.cls || 'm', o.bold ? 'font-weight="600"' : ''));
    out.push(T(x + w / 2, cy + 13, sub, o.subcls || 'i'));
  } else {
    out.push(T(x + w / 2, cy + 4.5, title, o.cls || 'm', o.bold ? 'font-weight="600"' : ''));
  }
  return out.join('\n');
}

function A(d, color = 'ink', o = {}) {
  const { w = 1.2, dash = '', start = false, end = true } = o;
  const col = { ink: C.line, lilac: C.lilac, orchid: C.orchid, rose: C.rose, moss: C.moss, sand: C.sand, mute: C.mute }[color];
  return `  <path d="${d}" fill="none" stroke="${col}" stroke-width="${w}"${dash ? ` stroke-dasharray="${dash}" stroke-linecap="round"` : ''}${start ? ` marker-start="url(#${color})"` : ''}${end ? ` marker-end="url(#${color})"` : ''}/>`;
}

const sub = (base, s) => `${base}<tspan dy="3" font-size="0.74em">${s}</tspan><tspan dy="-3" font-size="0.01em"> </tspan>`;
const sup = (base, s) => `${base}<tspan dy="-4.5" font-size="0.74em">${s}</tspan><tspan dy="4.5" font-size="0.01em"> </tspan>`;

// ---------------------------------------------------------------------------------------------
// Figure 1. Judgement as an office, and judgement as an interaction.
function fig1() {
  const W = 1200, H = 560, o = [];
  // (a) the tower
  o.push(T(40, 44, 'a', 'pl'));
  const tx = 115, tw = 190, th = 50, cx = tx + tw / 2;
  const rows = [['person', 76], ['judge', 176], ['proposers', 276], ['framework', 376]];
  for (const [name, y] of rows) o.push(B(tx, y, tw, th, name));
  const labs = ['revision', 'score', 'change'];
  rows.slice(0, 3).forEach(([, y], k) => {
    o.push(A(`M${cx},${y + th} V${y + 98}`));
    o.push(T(cx + 12, y + th + 29, labs[k], 'lab'));
  });
  o.push(A(`M${tx + tw},201 C${tx + tw + 78},201 ${tx + tw + 78},401 ${tx + tw + 2},401`));
  o.push(T(tx + tw + 66, 305, 'admission', 'lab'));
  o.push(T(cx, 492, 'JUDGEMENT AS OFFICE', 'tag'));
  o.push(T(cx, 512, 'regress closed by a terminal evaluator', 'i'));

  // (b) the ecology
  o.push(T(480, 44, 'b', 'pl'));
  o.push(R(520, 62, 440, 398, { rx: 26, dash: '6 4', stroke: C.lilac, fill: '#FDFBFE', sw: 1.3 }));
  o.push(T(544, 86, 'membrane', 'il'));
  const pw = 150, ph = 48;
  o.push(B(612 - pw / 2, 126, pw, ph, 'artefacts', '', { stroke: C.orchid, fill: C.tOrchid, rx: 24, sw: 1.2 }));
  o.push(B(868 - pw / 2, 126, pw, ph, 'lineages', '', { stroke: C.orchid, fill: C.tOrchid, rx: 24, sw: 1.2 }));
  o.push(B(740 - pw / 2, 262, pw, ph, 'norms', '', { stroke: C.orchid, fill: C.tOrchid, rx: 24, sw: 1.2 }));
  o.push(A('M691,150 H789', 'orchid', { start: true }));
  o.push(T(740, 141, 'production · use', 'labm'));
  o.push(A('M848,178 L792,258', 'orchid', { start: true }));
  o.push(T(832, 226, 'enactment · disposition', 'lab'));
  o.push(A('M632,178 L688,258', 'orchid', { start: true }));
  o.push(T(648, 226, 'terms · cases', 'labe'));
  o.push(T(740, 206, 'closure', 'i'));
  o.push(R(545, 366, 390, 70, { fill: C.tSand, stroke: C.sand, rx: 3 }));
  o.push(T(740, 393, 'ground', 'h'));
  o.push(T(740, 414, 'conservation · record of facts · escrow · anonymity', 's'));
  o.push(A('M740,336 V312', 'sand', { dash: '1.5 3.5', w: 1.4 }));
  o.push(T(750, 334, 'invariants', 'lab'));
  // the person, outside
  o.push(B(1072, 190, 112, 68, 'environment', '', { stroke: C.moss, fill: C.tMoss, dash: '5 3', sw: 1.1 }));
  o.push(A('M1072,210 H966', 'moss'));
  o.push(T(1019, 201, 'energy · signals', 'labm'));
  o.push(A('M966,240 H1070', 'moss'));
  o.push(T(1019, 258, 'harvest', 'labm'));
  o.push(A('M740,436 V486', 'mute'));
  o.push(T(752, 480, 'dissipation', 'lab'));
  o.push(T(740, 516, 'JUDGEMENT AS INTERACTION', 'tag'));
  o.push(T(740, 536, 'regress closed by a cycle', 'i'));
  return frame(W, H, 'Judgement as an office and judgement as an interaction', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 2. The ground and the lineage: what is invariant and what evolves.
function fig2() {
  const W = 1160, H = 590, o = [];
  o.push(R(40, 46, 430, 500, { fill: C.tSand, stroke: C.sand, rx: 4 }));
  o.push(T(255, 82, 'ground', 'h'));
  o.push(T(255, 102, 'INVARIANT', 'tag'));
  const prim = [
    ['run', 'turn of a lineage, metered'],
    ['write', 'content, append-only'],
    ['carry', 'world of a lineage'],
    ['fact', 'check on a tree, reproducible'],
    ['transfer', 'energy, conserved'],
    ['escrow', 'transfer on a decidable condition'],
    ['divide', 'new lineage, bounded'],
  ];
  prim.forEach(([verb, gloss], k) => {
    const y = 126 + k * 50;
    o.push(B(70, y, 112, 34, verb, '', { rx: 2 }));
    o.push(T(198, y + 21.5, gloss, 'il'));
  });
  o.push(T(255, 510, 'conservation · anonymity · reproducibility · confinement', 's'));

  o.push(R(560, 46, 560, 500, { fill: C.tOrchid, stroke: C.orchid, rx: 18 }));
  o.push(T(840, 82, 'lineage', 'h'));
  o.push(T(840, 102, 'EVOLVING', 'tag'));
  o.push(R(612, 126, 348, 336, { rx: 6 }));
  o.push(T(626, 146, 'genome', 'lab'));
  const organs = [['perception', 'foraging', 'craft'], ['immunity', 'challenge', 'exchange'], ['values', 'metabolism', 'life history']];
  organs.forEach((row, r) => row.forEach((name, c) => o.push(B(630 + c * 106, 162 + r * 72, 100, 52, name, '', { rx: 2 }))));
  o.push(R(630, 388, 312, 56, { fill: C.tRose, stroke: C.orchid, rx: 2, sw: 1.3 }));
  o.push(T(786, 411, 'reproduction', 'm', 'font-weight="600"'));
  o.push(T(786, 430, 'copy · variation · recombination · proof', 's'));
  // the organ copies the whole genome, itself included
  o.push(A('M630,416 H590 V142 H610', 'orchid', { w: 1.4 }));
  o.push(T(0, 0, 'genome ↦ genome′', 'labm', 'transform="translate(582 280) rotate(-90)"'));
  // state beside the genome
  o.push(B(986, 158, 112, 42, 'memory', '', { rx: 2 }));
  o.push(B(986, 214, 112, 42, 'world', '', { rx: 2 }));
  o.push(B(986, 270, 112, 42, 'energy', '', { rx: 2 }));
  o.push(B(986, 388, 112, 56, 'offspring', '', { rx: 2, dash: '5 3', stroke: C.orchid }));
  o.push(A('M942,416 H984', 'orchid', { w: 1.4 }));
  o.push(T(1042, 462, 'by divide', 'i'));
  o.push(T(840, 492, 'offspring: genome′ · testament · endowment', 's'));
  o.push(T(840, 512, 'no organ exempt from variation', 'i'));
  // exchange between the two
  o.push(A('M472,250 H558'));
  o.push(T(515, 241, 'energy · facts', 'labm'));
  o.push(A('M558,330 H472'));
  o.push(T(515, 321, 'acts', 'labm'));
  return frame(W, H, 'The ground and the lineage', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 3. Reproduction as an organ: description, construction and copy; the bottleneck.
function fig3() {
  const W = 1100, H = 500, o = [];
  const machine = (x, name, prime) => {
    const p = prime ? '′' : '';
    o.push(T(x, 120, name, 'lab'));
    o.push(R(x, 130, 400, 124, { rx: 4 }));
    const cells = [['A', 'constructor', 'craft'], ['B', 'copier', 'reproduction'], ['C', 'controller', 'life history']];
    cells.forEach(([l, role, organ], k) => {
      const cx = x + 20 + k * 126;
      const hot = l === 'B';
      o.push(R(cx, 148, 108, 60, { rx: 2, fill: hot ? C.tRose : C.white, stroke: hot ? C.orchid : C.ink, sw: hot ? 1.3 : 1 }));
      o.push(T(cx + 54, 174, l + p, 'm', 'font-weight="600"'));
      o.push(T(cx + 54, 192, role, 's'));
      o.push(T(cx + 54, 238, organ, 'i'));
    });
    o.push(R(x, 286, 400, 44, { rx: 2, fill: C.tLilac, stroke: C.lilac }));
    o.push(T(x + 200, 313, `φ${p}(A${p} + B${p} + C${p})`, 'm'));
    o.push(T(x + 200, 350, 'description (genome)', 'i'));
  };
  machine(60, 'parent', false);
  machine(640, 'offspring', true);
  // the bottleneck
  o.push(A('M550,76 V280', 'mute', { dash: '5 4', end: false }));
  o.push(A('M550,338 V420', 'mute', { dash: '5 4', end: false }));
  o.push(T(550, 66, 'division', 'labm'));
  // copy by B with variation
  o.push(A('M460,308 H638', 'lilac', { w: 1.4 }));
  o.push(T(549, 298, 'copy by B, variation ε', 'labm'));
  // provision by A; the machinery of each is its description as the ground interprets it
  o.push(A('M134,148 V92 H840 V128', 'ink'));
  o.push(T(344, 84, 'provision by A', 'labm'));
  o.push(A('M260,286 V258', 'sand', { dash: '1.5 3.5', w: 1.4 }));
  o.push(A('M840,286 V258', 'sand', { dash: '1.5 3.5', w: 1.4 }));
  o.push(T(850, 276, 'interpretation by the ground', 'lab'));
  o.push(T(260, 396, 'untransmitted: memory · reliance · contracts', 's'));
  o.push(T(840, 396, 'transmitted: φ′ · testament ≤ β · endowment ≤ η e', 's'));
  o.push(T(550, 458, 'φ′ = φ + ε ⟹ B′ ≠ B', 'm'));
  o.push(T(550, 478, 'variation of the description is variation of the copier', 'i'));
  return frame(W, H, 'Reproduction as an organ of the individual', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 4. Energy flows and is dissipated; knowledge is kept and returns.
function fig4() {
  const W = 1180, H = 620, o = [];
  // the record, above
  o.push(R(190, 46, 950, 44, { fill: C.tLilac, stroke: C.lilac, rx: 3 }));
  o.push(T(665, 73, 'record · content store, append-only', 'm'));
  // dissipation, below
  o.push(A('M190,566 H1140', 'mute', { end: false, w: 1 }));
  o.push(T(665, 588, 'dissipation · lapse to the source', 'i'));
  // roles
  const bw = 132, bh = 46;
  const node = (cx, cy, name, extra = {}) => o.push(B(cx - bw / 2, cy - bh / 2, bw, bh, name, '', { rx: 2, ...extra }));
  o.push(`  <circle cx="78" cy="270" r="34" fill="${C.tRose}" stroke="${C.rose}" stroke-width="1.2"/>`);
  o.push(T(78, 274.5, 'source', 'm'));
  node(256, 270, 'needs');
  node(470, 270, 'producers');
  node(680, 180, 'carriers');
  node(770, 380, 'challengers');
  node(1000, 270, 'decomposers');
  // energy (solid rose)
  const E = (d, w = 1.5) => o.push(A(d, 'rose', { w }));
  E('M112,270 H188'); o.push(T(150, 261, 'demand', 'labm'));
  E('M322,270 H402'); o.push(T(362, 261, 'bounty', 'labm'));
  E('M614,186 C570,194 548,212 512,245'); o.push(T(552, 198, 'premium', 'labe'));
  E('M512,295 C560,338 620,366 702,374'); o.push(T(566, 356, 'stake', 'labe'));
  E('M746,188 C840,196 890,222 934,258'); o.push(T(880, 204, 'premium', 'lab'));
  // seed: the source's emission to the newly born
  E('M100,296 C150,372 330,372 432,296', 1.2); o.push(T(266, 372, 'seed', 'labm'));
  // metabolism (thin, to dissipation)
  o.push(A('M488,295 V564', 'mute', { w: 1 }));
  o.push(A('M650,203 V564', 'mute', { w: 1 }));
  o.push(A('M770,403 V564', 'mute', { w: 1 }));
  o.push(A('M1000,293 V564', 'mute', { w: 1 }));
  o.push(T(1010, 500, 'metabolism · rent', 'lab'));
  // knowledge (dotted lilac)
  const K = (d) => o.push(A(d, 'lilac', { w: 1.4, dash: '1.5 3.5' }));
  K('M256,92 V245'); o.push(T(246, 170, 'reading', 'labe'));
  K('M440,247 V92'); o.push(T(430, 170, 'artefact · claim', 'labe'));
  K('M680,157 V92'); o.push(T(670, 128, 'adoption', 'labe'));
  K('M800,357 V92'); o.push(T(810, 128, 'counterexample', 'lab'));
  K('M970,247 V92'); o.push(T(960, 170, 'lesson', 'labe'));
  K('M1030,92 V245'); o.push(T(1040, 170, 'remains', 'lab'));
  o.push(T(1040, 185, 'testaments', 'lab'));
  // key
  o.push(A('M40,520 H84', 'rose', { w: 1.5 })); o.push(T(92, 524, 'energy', 'sl'));
  o.push(A('M40,542 H84', 'lilac', { w: 1.4, dash: '1.5 3.5' })); o.push(T(92, 546, 'knowledge', 'sl'));
  return frame(W, H, 'Flows of energy and cycles of knowledge', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 5. A flow of pairwise advantages split into a gradient and a cyclic part.
function fig5() {
  const W = 1100, H = 420, o = [];
  const nodes = { 1: [40, 150], 2: [150, 50], 3: [150, 250], 4: [270, 150] };
  // each edge: from, to, value (orientation is the direction of positive advantage)
  const panels = [
    { x: 40, title: 'a', norm: '‖a‖² = 14', edges: [[1, 2, 2], [2, 3, 2], [1, 3, 1], [2, 4, 2], [3, 4, 1]], color: 'ink' },
    { x: 400, title: 'dφ', norm: '‖dφ‖² = 11', edges: [[1, 2, 1], [2, 3, 1], [1, 3, 2], [2, 4, 2], [3, 4, 1]], color: 'lilac', phi: { 1: 0, 2: 1, 3: 2, 4: 3 } },
    { x: 760, title: 'r', norm: '‖r‖² = 3', edges: [[1, 2, 1], [2, 3, 1], [3, 1, 1]], faint: [[2, 4], [3, 4]], color: 'rose' },
  ];
  const col = { ink: C.line, lilac: C.lilac, rose: C.rose };
  for (const p of panels) {
    const P = (n) => [nodes[n][0] + p.x, nodes[n][1] + 70];
    for (const [a, b] of p.faint || []) {
      const [x1, y1] = P(a), [x2, y2] = P(b);
      o.push(`  <line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${C.soft}" stroke-width="1" stroke-dasharray="3 3"/>`);
    }
    for (const [a, b, v] of p.edges) {
      const [x1, y1] = P(a), [x2, y2] = P(b);
      const L = Math.hypot(x2 - x1, y2 - y1), ux = (x2 - x1) / L, uy = (y2 - y1) / L, r = 17;
      o.push(A(`M${(x1 + ux * r).toFixed(1)},${(y1 + uy * r).toFixed(1)} L${(x2 - ux * (r + 2)).toFixed(1)},${(y2 - uy * (r + 2)).toFixed(1)}`, p.color, { w: 1.1 + 0.9 * (v - 1) }));
      // the value, beside the middle of the edge
      const mx = (x1 + x2) / 2 - uy * 13, my = (y1 + y2) / 2 + ux * 13;
      o.push(T(mx.toFixed(1), (my + 4).toFixed(1), String(v), 'num'));
    }
    for (const n of [1, 2, 3, 4]) {
      const [x, y] = P(n);
      o.push(`  <circle cx="${x}" cy="${y}" r="15" fill="${C.white}" stroke="${col[p.color]}" stroke-width="1.2"/>`);
      o.push(T(x, y + 4.5, p.phi ? `${p.phi[n]}` : `${n}`, 'm'));
    }
    o.push(T(p.x + 155, 44, p.title, 'h'));
    o.push(T(p.x + 155, 372, p.norm, 's'));
  }
  o.push(T(382, 226, '=', 'h'));
  o.push(T(742, 226, '+', 'h'));
  o.push(T(555, 396, 'node labels: values of φ', 'i'));
  o.push(T(195, 396, 'node labels: types', 'i'));
  o.push(T(915, 396, 'd*r = 0', 'i'));
  return frame(W, H, 'A flow of pairwise advantages as a gradient part and a cyclic part', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 6. Reliance is a stock that decays; scrutiny grows with reliance.
function fig6() {
  const W = 1160, H = 480, o = [];
  o.push(T(40, 44, 'a', 'pl'));
  // the chart: e(t+1) = (1 - δ) e(t) + u(t), with δ = 0.05 and u = 1 during an episode
  const x0 = 96, y0 = 400, cw = 470, ch = 310, turns = 170, ymax = 22;
  const X = (t) => x0 + (t / turns) * cw, Y = (v) => y0 - (v / ymax) * ch;
  o.push(A(`M${x0},${y0} H${x0 + cw + 12}`, 'mute', { w: 1 }));
  o.push(A(`M${x0},${y0} V${y0 - ch - 12}`, 'mute', { w: 1 }));
  o.push(T(x0 + cw / 2, y0 + 38, 'turns', 'ax'));
  o.push(T(0, 0, 'stock of reliance', 'ax', `transform="translate(${x0 - 44} ${y0 - ch / 2}) rotate(-90)"`));
  for (const t of [0, 40, 80, 120, 160]) {
    o.push(`  <line x1="${X(t)}" y1="${y0}" x2="${X(t)}" y2="${y0 + 4}" stroke="${C.mute}"/>`);
    o.push(T(X(t), y0 + 18, String(t), 'ax'));
  }
  for (const v of [0, 10, 20]) {
    o.push(`  <line x1="${x0 - 4}" y1="${Y(v)}" x2="${x0}" y2="${Y(v)}" stroke="${C.mute}"/>`);
    o.push(T(x0 - 8, Y(v) + 4, String(v), 'axe'));
  }
  const delta = 0.05, U = 1;
  o.push(`  <line x1="${x0}" y1="${Y(U / delta)}" x2="${x0 + cw}" y2="${Y(U / delta)}" stroke="${C.mute}" stroke-dasharray="5 4"/>`);
  o.push(T(x0 + cw - 2, Y(U / delta) - 7, 'bound U/δ', 'axe'));
  const episodes = [[0, 45, C.lilac], [32, 80, C.orchid], [70, 118, C.rose], [104, 150, C.moss]];
  episodes.forEach(([a, b, color], k) => {
    let e = 0; const pts = [];
    for (let t = 0; t <= turns; t++) {
      pts.push(`${X(t).toFixed(1)},${Y(e).toFixed(1)}`);
      e = (1 - delta) * e + (t >= a && t < b ? U : 0);
    }
    o.push(`  <polyline points="${pts.join(' ')}" fill="none" stroke="${color}" stroke-width="1.8"/>`);
    o.push(T(X((a + b) / 2 + 6), Y(0) - 8 - (k % 2) * 0, '', 'ax'));
  });
  // the half-life, on the decay of the first
  const tA = 45, eA = (U / delta) * (1 - Math.pow(1 - delta, 45));
  const half = Math.log(2) / -Math.log(1 - delta);
  o.push(`  <line x1="${X(tA)}" y1="${Y(eA / 2)}" x2="${X(tA + half)}" y2="${Y(eA / 2)}" stroke="${C.mute}" stroke-width="1"/>`);
  o.push(`  <line x1="${X(tA)}" y1="${Y(eA / 2) - 4}" x2="${X(tA)}" y2="${Y(eA / 2) + 4}" stroke="${C.mute}"/>`);
  o.push(`  <line x1="${X(tA + half)}" y1="${Y(eA / 2) - 4}" x2="${X(tA + half)}" y2="${Y(eA / 2) + 4}" stroke="${C.mute}"/>`);
  o.push(T(X(tA + half) + 6, Y(eA / 2) + 4, 'half-life', 'axl'));
  // key of the four lineages
  ['lineage 1', 'lineage 2', 'lineage 3', 'lineage 4'].forEach((name, k) => {
    const kx = x0 + 10 + k * 112, ky = 452;
    o.push(`  <line x1="${kx}" y1="${ky}" x2="${kx + 26}" y2="${ky}" stroke="${episodes[k][2]}" stroke-width="1.8"/>`);
    o.push(T(kx + 32, ky + 4, name, 'axl'));
  });

  // (b) the loop
  o.push(T(606, 44, 'b', 'pl'));
  const nw = 124, nh = 42;
  const N = { reliance: [870, 96], income: [1040, 196], bounty: [976, 346], scrutiny: [764, 346], refutation: [700, 196], term: [706, 70] };
  const nb = (k, o2 = {}) => o.push(B(N[k][0] - nw / 2, N[k][1] - nh / 2, nw, nh, k, '', { rx: 21, stroke: C.orchid, fill: C.tOrchid, sw: 1.2, ...o2 }));
  for (const k of ['reliance', 'income', 'bounty', 'scrutiny', 'refutation']) nb(k);
  const L = (d, sign, sx, sy, color = 'orchid') => { o.push(A(d, color, { w: 1.3 })); o.push(T(sx, sy, sign, 'h')); };
  L('M932,104 C990,116 1022,140 1034,173', '+', 1012, 128);
  L('M1034,219 C1028,268 1014,300 992,323', '+', 1040, 280);
  L('M912,346 H828', '+', 870, 336);
  L('M748,323 C724,300 708,262 702,219', '+', 700, 290);
  L('M706,173 C720,138 760,112 806,102', '−', 734, 132, 'rose');
  o.push(T(870, 232, 'balancing', 'i'));
  // the term of a lineage also ends reliance
  o.push(B(N.term[0] - 46, N.term[1] - 18, 92, 36, 'term', '', { rx: 2 }));
  o.push(A('M753,74 C778,78 792,83 806,89', 'rose', { w: 1.3 }));
  o.push(T(782, 68, '−', 'h'));
  o.push(T(870, 430, 'refutation conditional on unsoundness', 'i'));
  return frame(W, H, 'Reliance as a decaying stock; scrutiny proportional to reliance', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 7. Cycles nested by level, and the law of cascade sizes.
function lgamma(n) { let s = 0; for (let k = 2; k < n; k++) s += Math.log(k); return s; } // ln Γ(n) = ln (n-1)!
function fig7() {
  const W = 1180, H = 540, o = [];
  o.push(T(40, 44, 'a', 'pl'));
  // axes of panel (a)
  const ax0 = 84, ay0 = 470;
  o.push(A(`M${ax0},${ay0} H606`, 'mute', { w: 1 }));
  o.push(A(`M${ax0},${ay0} V70`, 'mute', { w: 1 }));
  o.push(T(345, ay0 + 30, 'reach (number of dependants)', 'ax'));
  o.push(T(0, 0, 'persistence (turns per cycle)', 'ax', `transform="translate(${ax0 - 26} 270) rotate(-90)"`));
  // a lemniscate traversed r → K → Ω → α → r; the point at step k of 240
  const at = (cx, cy, a, k) => {
    const t = -(k / 240) * 2 * Math.PI, d = 1 + Math.sin(t) ** 2;
    return [cx + (a * Math.cos(t)) / d, cy - (1.8 * a * Math.sin(t) * Math.cos(t)) / d];
  };
  const lem = (cx, cy, a, name, level) => {
    const color = { rose: C.rose, orchid: C.orchid, lilac: C.lilac }[name];
    const pts = [];
    for (let k = 0; k <= 240; k++) { const [x, y] = at(cx, cy, a, k); pts.push(`${x.toFixed(1)},${y.toFixed(1)}`); }
    o.push(`  <polyline points="${pts.join(' ')}" fill="none" stroke="${color}" stroke-width="1.6"/>`);
    // the direction of travel, on the two strokes that cross
    for (const k of [44, 164]) {
      const [x1, y1] = at(cx, cy, a, k), [x2, y2] = at(cx, cy, a, k + 5);
      o.push(A(`M${x1.toFixed(1)},${y1.toFixed(1)} L${x2.toFixed(1)},${y2.toFixed(1)}`, name, { w: 1.6 }));
    }
    const q = a * 0.6, v = a * 0.25;
    o.push(T(cx - q, cy + v + 4, 'r', 'm', 'font-style="italic"'));
    o.push(T(cx + q, cy - v + 4, 'K', 'm', 'font-style="italic"'));
    o.push(T(cx + q, cy + v + 4, 'Ω', 'm'));
    o.push(T(cx - q, cy - v + 4, 'α', 'm', 'font-style="italic"'));
    o.push(T(cx, cy + a * 0.66 + 22, level, 'i'));
  };
  lem(170, 390, 58, 'rose', 'level 0');
  lem(310, 270, 80, 'orchid', 'level 1');
  lem(455, 150, 96, 'lilac', 'level 2');
  // revolt: from Ω of a level to the K of the next, round its outside
  o.push(A('M224,428 C330,440 440,330 396,236', 'rose', { dash: '5 4', w: 1.3 }));
  o.push(T(404, 372, 'revolt', 'lab'));
  o.push(A('M382,320 C500,340 620,230 554,108', 'rose', { dash: '5 4', w: 1.3 }));
  o.push(T(584, 250, 'revolt', 'lab'));
  // remember: from the K of a level, over it, to the α of the one below
  o.push(A('M354,214 C310,160 120,250 132,344', 'lilac', { dash: '1.5 3.5', w: 1.4 }));
  o.push(T(176, 232, 'remember', 'labe'));
  o.push(A('M508,84 C455,22 240,110 258,208', 'lilac', { dash: '1.5 3.5', w: 1.4 }));
  o.push(T(296, 92, 'remember', 'labe'));

  // (b) the law of cascade sizes of a branching process with Poisson offspring of mean σ (Borel)
  o.push(T(640, 44, 'b', 'pl'));
  const bx = 720, by = 470, bw = 400, bh = 380;
  const lx = (s) => bx + (Math.log10(s) / 3) * bw;         // s from 1 to 1000
  const ly = (p) => by - ((Math.log10(p) + 8) / 8) * bh;    // P from 1e-8 to 1
  o.push(A(`M${bx},${by} H${bx + bw + 12}`, 'mute', { w: 1 }));
  o.push(A(`M${bx},${by} V${by - bh - 12}`, 'mute', { w: 1 }));
  o.push(T(bx + bw / 2, by + 38, 'cascade size s', 'ax'));
  o.push(T(0, 0, 'probability P(s)', 'ax', `transform="translate(${bx - 50} ${by - bh / 2}) rotate(-90)"`));
  for (const e of [0, 1, 2, 3]) {
    o.push(`  <line x1="${lx(10 ** e)}" y1="${by}" x2="${lx(10 ** e)}" y2="${by + 4}" stroke="${C.mute}"/>`);
    o.push(T(lx(10 ** e), by + 18, sup('10', String(e)), 'ax'));
  }
  for (const e of [0, -2, -4, -6, -8]) {
    o.push(`  <line x1="${bx - 4}" y1="${ly(10 ** e)}" x2="${bx}" y2="${ly(10 ** e)}" stroke="${C.mute}"/>`);
    o.push(T(bx - 8, ly(10 ** e) + 4, sup('10', e === 0 ? '0' : `−${-e}`), 'axe'));
  }
  const borel = (sigma, s) => Math.exp(-sigma * s + (s - 1) * Math.log(sigma * s) - lgamma(s + 1));
  const curves = [[0.5, C.lilac], [0.8, C.orchid], [1.0, C.rose]];
  for (const [sigma, color] of curves) {
    const pts = [];
    for (let s = 1; s <= 1000; s++) {
      const p = borel(sigma, s);
      if (p < 1e-8) break;
      pts.push(`${lx(s).toFixed(1)},${ly(p).toFixed(1)}`);
    }
    o.push(`  <polyline points="${pts.join(' ')}" fill="none" stroke="${color}" stroke-width="1.8"/>`);
  }
  o.push(T(lx(13), ly(1e-7) - 2, 'σ = 0.5', 'axl'));
  o.push(T(lx(125), ly(1e-7) - 2, 'σ = 0.8', 'axl'));
  o.push(T(lx(230), ly(2e-4) - 4, 'σ = 1', 'axl'));
  // the slope −3/2
  o.push(`  <line x1="${lx(30)}" y1="${ly(0.2)}" x2="${lx(300)}" y2="${ly(0.2 * 10 ** -1.5)}" stroke="${C.mute}" stroke-dasharray="5 4"/>`);
  o.push(T(lx(100) + 8, ly(0.05), sup('s', '−3/2'), 'axl'));
  o.push(T(bx + bw / 2, 66, 'σ &lt; 1 subcritical · σ = 1 critical · σ &gt; 1 unbounded', 'i'));
  return frame(W, H, 'Cycles nested by level and the law of cascade sizes', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 8. The membrane, the person, and the stages.
function fig8() {
  const W = 1200, H = 600, o = [];
  o.push(T(40, 44, 'a', 'pl'));
  o.push(R(64, 64, 600, 440, { rx: 26, dash: '6 4', stroke: C.lilac, fill: '#FDFBFE', sw: 1.3 }));
  o.push(T(90, 90, 'membrane', 'il'));
  // lineages
  const names = ['ℓ₁', 'ℓ₂', 'ℓ₃', 'ℓ₄', 'ℓ₅'];
  names.forEach((n, k) => {
    const x = 96 + k * 110, y = 112, last = k === 4;
    o.push(R(x, y, 94, 132, { rx: 4, stroke: C.orchid, fill: C.tOrchid, dash: last ? '5 3' : '', sw: 1.2 }));
    o.push(T(x + 47, y + 22, n, 'm', 'font-weight="600"'));
    ['genome', 'memory', 'world'].forEach((part, j) => o.push(B(x + 10, y + 34 + j * 31, 74, 24, part, '', { rx: 2, cls: 's' })));
  });
  o.push(T(364, 268, 'adoption · challenge · escrow · division', 'i'));
  o.push(A('M190,178 H206', 'orchid', { start: true, w: 1.1 }));
  o.push(A('M300,178 H316', 'orchid', { start: true, w: 1.1 }));
  o.push(A('M410,178 H426', 'orchid', { start: true, w: 1.1 }));
  o.push(A('M520,178 H534', 'orchid', { w: 1.1 }));
  // the store and the ground
  o.push(R(96, 296, 536, 46, { fill: C.tLilac, stroke: C.lilac, rx: 3 }));
  o.push(T(364, 324, 'content store, append-only', 'm'));
  o.push(R(96, 372, 536, 96, { fill: C.tSand, stroke: C.sand, rx: 3 }));
  o.push(T(364, 400, 'ground', 'h'));
  ['scheduler', 'harness', 'ledger of energy', 'ledger of facts', 'journal'].forEach((part, k) => {
    const w = [84, 76, 118, 108, 70][k], x = [108, 200, 284, 410, 526][k];
    o.push(B(x, 416, w, 30, part, '', { rx: 2, cls: 's' }));
  });
  // outside
  o.push(B(748, 108, 112, 50, 'person'));
  o.push(A('M748,122 H668')); o.push(T(708, 113, 'source · needs', 'labm'));
  o.push(A('M668,146 H746')); o.push(T(708, 164, 'harvest', 'labm'));
  o.push(B(748, 296, 112, 46, 'bedrock'));
  o.push(A('M748,319 H636', 'lilac', { w: 1.3 })); o.push(T(692, 310, 'rebase', 'labm'));
  o.push(A('M804,160 V294')); o.push(T(794, 232, 'review · merge', 'labe'));
  o.push(B(748, 398, 112, 46, 'observatory'));
  o.push(A('M634,421 H746', 'mute', { dash: '1.5 3.5', w: 1.4 })); o.push(T(690, 412, 'record', 'labm'));
  o.push(A('M860,134 H900 V456 H636', 'rose', { dash: '5 4', w: 1.2 }));
  o.push(T(0, 0, 'stop · revision', 'labm', 'transform="translate(914 300) rotate(90)"'));
  o.push(T(364, 538, 'no act outside the membrane', 'i'));

  // (b) the stages
  o.push(T(960, 44, 'b', 'pl'));
  const stages = [['gardens', 'varied ground', 196, 440], ['garden', 'demes · migration', 164, 334], ['terrarium', 'few lineages · one biotope', 132, 228], ['jar', 'synthetic · agents', 100, 122]];
  stages.forEach(([name, gloss, w, h], k) => {
    const x = 978 + k * 16, y = 64 + (440 - h);
    o.push(R(x, y, w, h, { rx: 10, stroke: [C.lilac, C.orchid, C.rose, C.ink][k], fill: [C.tLilac, C.tOrchid, '#FDF6F9', C.white][k], sw: 1.2 }));
    o.push(T(x + 12, y + 24, name, 'ml', 'font-weight="600"'));
    const words = gloss.split(' · ');
    words.forEach((wd, j) => o.push(T(x + 12, y + 42 + j * 15, wd, 'il')));
  });
  o.push(T(1073, 538, 'each stage within the next', 'i'));
  return frame(W, H, 'The membrane, the person and the stages', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 9. The garden in its environment: three channels in, one out.
function fig9() {
  const W = 1200, H = 640, o = [];
  // the outside: referred to, not established
  o.push(R(34, 50, 236, 530, { rx: 18, fill: C.tMoss, stroke: C.moss, dash: '5 3', sw: 1.2 }));
  o.push(T(152, 82, 'environment', 'h'));
  ['problems', 'goals', 'values · policy', 'economy', 'knowledge', 'use'].forEach((name, k) =>
    o.push(B(72, 106 + k * 74, 160, 46, name, '', { rx: 2, stroke: C.moss })));
  // what is brought to the membrane: manifestations
  const ch = [['sources', 'needs with energy', 150], ['feeds', 'dated, attributed records', 300], ['references', 'measured values', 450]];
  for (const [name, gloss, y] of ch) {
    o.push(B(346, y, 184, 60, name, gloss, { rx: 2, fill: C.tSand, stroke: C.sand }));
    o.push(A(`M272,${y + 30} H344`, 'moss'));
  }
  // the garden
  o.push(R(640, 50, 520, 480, { rx: 26, dash: '6 4', stroke: C.lilac, fill: '#FDFBFE', sw: 1.3 }));
  o.push(T(664, 76, 'membrane', 'il'));
  o.push(B(680, 158, 110, 44, 'needs', '', { rx: 2 }));
  o.push(B(680, 458, 110, 44, 'harness', '', { rx: 2 }));
  o.push(A('M532,180 H678', 'rose', { w: 1.5 })); o.push(T(586, 171, 'energy', 'labm'));
  o.push(A('M532,480 H678', 'sand', { w: 1.3 })); o.push(T(586, 471, 'facts', 'labm'));
  // three lineages, each reading a part
  const ls = [['ℓ₁', 110], ['ℓ₂', 250], ['ℓ₃', 390]];
  for (const [name, y] of ls) {
    o.push(R(920, y, 200, 96, { rx: 4, stroke: C.orchid, fill: C.tOrchid, sw: 1.2 }));
    o.push(T(944, y + 54, name, 'm', 'font-weight="600"'));
    o.push(B(972, y + 12, 132, 32, 'perception', '', { rx: 2, cls: 's' }));
    o.push(B(972, y + 52, 132, 32, 'memory · acts', '', { rx: 2, cls: 's' }));
  }
  // the feeds, read in part: each lineage its own share
  o.push(A('M532,316 C700,300 800,170 970,138', 'lilac', { w: 1.4, dash: '1.5 3.5' }));
  o.push(A('M532,326 C700,322 800,290 970,278', 'lilac', { w: 1.4, dash: '1.5 3.5' }));
  o.push(A('M532,336 C700,350 800,420 970,418', 'lilac', { w: 1.4, dash: '1.5 3.5' }));
  o.push(T(586, 300, 'signals', 'labm'));
  o.push(T(806, 232, 'Umwelt', 'il'));
  o.push(T(806, 248, 'partial, paid', 'il'));
  o.push(A('M792,180 H918', 'rose', { w: 1.2 }));
  o.push(A('M792,480 C850,480 880,450 918,440', 'sand', { w: 1.2 }));
  // out again
  o.push(A('M900,532 V596 H152 V582', 'moss', { w: 1.4 }));
  o.push(T(526, 588, 'harvest', 'labm'));
  o.push(T(526, 622, 'no score across the membrane', 'i'));
  return frame(W, H, 'The garden in its environment', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// Figure 10. Rank indexed by the environment; growth under a changing one, with and without a signal.
function fig10() {
  const W = 1180, H = 500, o = [];
  o.push(T(40, 44, 'a', 'pl'));
  // (a) three types' growth rates across the state of the environment
  const x0 = 96, y0 = 400, cw = 400, ch = 300;
  o.push(A(`M${x0},${y0} H${x0 + cw + 12}`, 'mute', { w: 1 }));
  o.push(A(`M${x0},${y0} V${y0 - ch - 12}`, 'mute', { w: 1 }));
  o.push(T(x0 + cw / 2, y0 + 34, 'state of the environment ε', 'ax'));
  o.push(T(0, 0, 'growth rate w(type; ε)', 'ax', `transform="translate(${x0 - 28} ${y0 - ch / 2}) rotate(-90)"`));
  const norms = [[0.86, 0.14, C.lilac, 'type 1'], [0.12, 0.90, C.rose, 'type 2'], [0.56, 0.60, C.moss, 'type 3']];
  for (const [l, r, color, name] of norms) {
    o.push(`  <line x1="${x0}" y1="${y0 - l * ch}" x2="${x0 + cw}" y2="${y0 - r * ch}" stroke="${color}" stroke-width="1.8"/>`);
    o.push(T(x0 + cw + 8, y0 - r * ch + 4, name, 'axl'));
  }
  o.push(T(x0 + cw / 2, 70, 'rank reversed between environments', 'i'));

  // (b) log size over turns: two environments at random, four ways of living in them
  o.push(T(620, 44, 'b', 'pl'));
  const bx = 690, by = 400, bw = 380, bh = 300, turns = 60, lo = -12, hi = 26;
  const X = (t) => bx + (t / turns) * bw, Y = (v) => by - ((v - lo) / (hi - lo)) * bh;
  o.push(A(`M${bx},${by} H${bx + bw + 12}`, 'mute', { w: 1 }));
  o.push(A(`M${bx},${by} V${by - bh - 12}`, 'mute', { w: 1 }));
  o.push(T(bx + bw / 2, by + 34, 'turns', 'ax'));
  o.push(T(0, 0, 'logarithm of size', 'ax', `transform="translate(${bx - 34} ${by - bh / 2}) rotate(-90)"`));
  for (const t of [0, 20, 40, 60]) { o.push(`  <line x1="${X(t)}" y1="${by}" x2="${X(t)}" y2="${by + 4}" stroke="${C.mute}"/>`); o.push(T(X(t), by + 18, String(t), 'ax')); }
  for (const v of [-10, 0, 10, 20]) { o.push(`  <line x1="${bx - 4}" y1="${Y(v)}" x2="${bx}" y2="${Y(v)}" stroke="${C.mute}"/>`); o.push(T(bx - 8, Y(v) + 4, String(v).replace('-', '−'), 'axe')); }
  o.push(`  <line x1="${bx}" y1="${Y(0)}" x2="${bx + bw}" y2="${Y(0)}" stroke="${C.soft}" stroke-dasharray="3 3"/>`);
  // the environment and the signal, from a fixed sequence (a linear congruential generator)
  let seed = 139; const rnd = () => (seed = (Math.imul(seed, 1664525) + 1013904223) >>> 0) / 4294967296;
  const hiPay = 2.0, loPay = 0.4, acc = 0.8, b = 0.95;
  const paths = { s1: [0], s2: [0], hedge: [0], cue: [0] };
  for (let t = 0; t < turns; t++) {
    const env = rnd() < 0.5 ? 1 : 2, right = rnd() < acc;
    const cue = right ? env : 3 - env;
    paths.s1.push(paths.s1[t] + Math.log(env === 1 ? hiPay : loPay));
    paths.s2.push(paths.s2[t] + Math.log(env === 2 ? hiPay : loPay));
    paths.hedge.push(paths.hedge[t] + Math.log(0.5 * hiPay + 0.5 * loPay));
    paths.cue.push(paths.cue[t] + Math.log(cue === env ? b * hiPay + (1 - b) * loPay : b * loPay + (1 - b) * hiPay));
  }
  const clamp = (v) => Math.max(lo, Math.min(hi, v));
  const draw = (arr, color) => o.push(`  <polyline points="${arr.map((v, t) => `${X(t).toFixed(1)},${Y(clamp(v)).toFixed(1)}`).join(' ')}" fill="none" stroke="${color}" stroke-width="1.8"/>`);
  draw(paths.s1, C.lilac); draw(paths.s2, C.rose); draw(paths.hedge, C.moss); draw(paths.cue, C.ink);
  const end = (arr) => Y(clamp(arr[turns]));
  o.push(T(bx + bw + 8, end(paths.cue) + 4, 'mixture, signal', 'axl'));
  o.push(T(bx + bw + 8, end(paths.hedge) + 4, 'mixture', 'axl'));
  const e1 = end(paths.s1), e2 = end(paths.s2);
  o.push(T(bx + bw + 8, e1 + (e1 <= e2 ? -2 : 12), 'type 1 alone', 'axl'));
  o.push(T(bx + bw + 8, e2 + (e2 < e1 ? -2 : 12), 'type 2 alone', 'axl'));
  o.push(T(bx + bw / 2, 70, 'difference of the mixtures′ slopes: I(E; Y) = 0.193', 'i'));
  console.log('fig10 ends', paths.s1[turns].toFixed(2), paths.s2[turns].toFixed(2), paths.hedge[turns].toFixed(2), paths.cue[turns].toFixed(2));
  return frame(W, H, 'Rank indexed by the environment, and the worth of a signal', o.join('\n'));
}

const figures = {
  1: ['fig-1-judgement.svg', fig1],
  2: ['fig-2-ground-and-lineage.svg', fig2],
  3: ['fig-3-reproduction.svg', fig3],
  4: ['fig-4-energy-and-knowledge.svg', fig4],
  5: ['fig-5-decomposition.svg', fig5],
  6: ['fig-6-reliance-and-scrutiny.svg', fig6],
  7: ['fig-7-cycles-and-cascades.svg', fig7],
  8: ['fig-8-membrane-and-stages.svg', fig8],
  9: ['fig-9-environment.svg', fig9],
  10: ['fig-10-environment-and-signal.svg', fig10],
};

const want = process.argv.slice(2).map(Number).filter(Boolean);
for (const [n, [name, make]] of Object.entries(figures)) {
  if (want.length && !want.includes(Number(n))) continue;
  writeFileSync(join(here, name), make());
  console.log('wrote', name);
}
