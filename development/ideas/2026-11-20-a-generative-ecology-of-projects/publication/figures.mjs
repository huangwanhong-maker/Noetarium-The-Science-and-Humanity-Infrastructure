// Makes the nine figures of the generative ecology of projects as SVG:  node figures.mjs   (then node render.mjs)
//   node figures.mjs 3 7      makes figures 3 and 7 only
//
// Every figure is drawn at print size on lib.mjs (720 units wide for a text width of 162 mm, no lettering
// under 7 pt) and carries formal labels and notation only; what explains it is in its caption (README.md).
// Figure 7 is computed from the prototypes' results (../prototypes/results/). manifest.json records each
// figure with the SHA-256 of its SVG.

import { readFileSync, writeFileSync } from 'node:fs';
import { createHash } from 'node:crypto';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { W, C, frame, T, R, B, A, line, panel, DOT, DASH } from './lib.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const result = (name) => JSON.parse(readFileSync(join(here, '..', 'prototypes', 'results', name), 'utf8'));
const small = { size: 11.5 };
const mute = { size: 11.5, fill: C.mute };
const BLUE = '#EEF3F9';
const chip = (x, y, w, label, o = {}) => B(x, y, w, o.h || 22, label, { size: 11.5, ...o });

// ---------------------------------------------------------------------------------------------
// 1. The three layers of a garden, by who may change them.
function layers() {
  const H = 386, o = [];
  o.push(A('M22,378 V20', 'mute', { w: 1 }));
  o.push(T(14, 200, 'rate of change', { ...mute, rotate: -90 }));

  o.push(R(40, 14, 492, 110, { rx: 6, fill: C.tOrchid, stroke: C.orchid }));
  o.push(T(52, 35, 'what evolves', { weight: 600, anchor: 'start' }));
  [['genomes', 'of lineages'], ['genomes', 'of units'], ['bodies', 'of units'], ['norms', ''], ['record', '']].forEach(([a, b], k) =>
    o.push(B(52 + k * 95, 52, 86, 44, a, b ? { sub: b } : {})));

  o.push(R(40, 138, 492, 98, { rx: 6, fill: C.tLilac, stroke: C.lilac, dash: DASH }));
  o.push(T(52, 159, 'site', { weight: 600, anchor: 'start' }));
  ['materials', 'claim kinds', 'channels', 'focus', 'reserved acts'].forEach((name, k) =>
    o.push(B(52 + k * 95, 174, 86, 36, name, { size: 11.5 })));

  o.push(R(40, 250, 492, 122, { rx: 6, fill: C.tSand, stroke: C.sand }));
  o.push(T(52, 271, 'ground', { weight: 600, anchor: 'start' }));
  ['run', 'write', 'carry', 'fact', 'transfer', 'escrow', 'divide'].forEach((name, k) =>
    o.push(B(52 + k * 68, 284, 60, 28, name, { size: 11.5 })));
  o.push(T(286, 338, 'record · conservation · reproducibility', mute));
  o.push(T(286, 356, 'escrow · anonymity · confinement', mute));

  o.push(B(612, 44, 96, 50, 'lineages', { fill: C.tOrchid, stroke: C.orchid }));
  o.push(A('M612,69 H536', 'orchid', { w: 1.3 }));
  o.push(T(574, 62, 'acts', small));
  o.push(R(612, 138, 96, 234, { rx: 6, fill: BLUE, stroke: C.blue, dash: DASH }));
  o.push(T(660, 159, 'people', { weight: 600 }));
  ['sources', 'witnesses', 'stewards'].forEach((name, k) => o.push(B(620, 172 + k * 30, 80, 24, name, { size: 11.5 })));
  o.push(A('M612,187 H536', 'blue', { w: 1.3 }));
  o.push(T(574, 180, 'declaration', small));
  o.push(A('M612,311 H536', 'blue', { w: 1.1, dash: DASH }));
  o.push(T(574, 304, 'revision', small));
  return frame(W, H, 'The three layers of a garden', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 2. Genome and body, for a lineage and for a unit; the two modes of change of a unit.
function genomeAndBody() {
  const H = 334, o = [];
  o.push(panel(8, 20, 'a'));
  o.push(R(40, 30, 264, 86, { rx: 4, fill: C.tOrchid, stroke: C.orchid }));
  o.push(T(52, 49, 'genome of a lineage', { size: 12.5, weight: 600, anchor: 'start' }));
  [['perception', 52, 76], ['foraging', 134, 66], ['craft', 206, 50]].forEach(([n, x, w]) => o.push(chip(x, 58, w, n)));
  [['challenge', 52, 72], ['values', 130, 56], ['reproduction', 192, 96]].forEach(([n, x, w]) => o.push(chip(x, 86, w, n)));
  o.push(B(352, 55, 60, 36, 'model', { rx: 18, fill: C.tSand, stroke: C.sand }));
  o.push(T(382, 45, 'development', small));
  o.push(A('M304,73 H350')); o.push(A('M412,73 H482'));
  o.push(B(484, 45, 220, 56, 'conduct', { sub: 'acts, claims' }));

  o.push(panel(8, 150, 'b'));
  o.push(R(40, 164, 264, 120, { rx: 4, fill: C.tLilac, stroke: C.lilac }));
  o.push(T(52, 184, 'genome of a unit', { size: 12.5, weight: 600, anchor: 'start' }));
  [['identity', 52, 84], ['laws', 140, 74], ['exemplars', 218, 74]].forEach(([n, x, w]) => o.push(chip(x, 196, w, n, { h: 26 })));
  [['development', 52, 84], ['regulation', 140, 74], ['perception', 218, 74]].forEach(([n, x, w]) => o.push(chip(x, 232, w, n, { h: 26 })));
  o.push(B(352, 204, 60, 36, 'model', { rx: 18, fill: C.tSand, stroke: C.sand }));
  o.push(T(382, 194, 'development', small));
  o.push(A('M304,222 H350', 'lilac', { w: 1.3 })); o.push(A('M412,222 H482', 'lilac', { w: 1.3 }));
  o.push(T(447, 240, 'reseeding', { ...small, fill: C.lilac }));
  o.push(R(484, 164, 220, 92, { rx: 4 }));
  o.push(T(496, 184, 'body of a unit', { size: 12.5, weight: 600, anchor: 'start' }));
  [['code', 496, 42], ['derivations', 542, 72], ['data', 618, 36], ['text', 658, 34]].forEach(([n, x, w]) => o.push(chip(x, 200, w, n, { h: 26 })));
  o.push(A('M614,164 C614,136 666,136 666,162', 'rose', { w: 1.3 }));
  o.push(T(640, 132, 'patching', { ...small, fill: C.rose }));
  o.push(A('M496,164 C460,128 326,128 270,162', 'lilac', { w: 1.2, dash: DOT }));
  o.push(T(383, 128, 'sequencing', { ...small, halo: C.white }));
  o.push(B(484, 284, 220, 30, 'checks', { fill: C.tSand, stroke: C.sand }));
  o.push(A('M594,256 V282', 'sand', { w: 1.3 }));
  o.push(T(602, 274, 'facts', { ...small, anchor: 'start' }));
  o.push(line(262, 284, 262, 299, { dash: '3 3' })); o.push(line(262, 299, 482, 299, { dash: '3 3' }));
  o.push(B(322, 288, 100, 23, 'regenerates', { size: 11.5, rx: 11, fill: C.tRose, stroke: C.rose }));
  o.push(line(162, 284, 162, 300, { dash: '3 3' }));
  o.push(B(132, 300, 60, 23, 'keeps', { size: 11.5, rx: 11, fill: C.tRose, stroke: C.rose }));
  return frame(W, H, 'Genome and body of a lineage and of a unit', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 3. Three resolutions: of perception, of selection and of variation.
function threeResolutions() {
  const H = 318, o = [];
  const cell = (x, y, w, h, fill) => R(x, y, w, h, { rx: 0, fill, stroke: 'none' });
  const border = (x, y, s) => R(x, y, s, s, { rx: 0, fill: 'none', stroke: C.line, sw: 1.1 });
  const thick = (x1, y1, x2, y2, stroke) => `  <line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${stroke}" stroke-width="2.8"/>`;
  const thin = (x1, y1, x2, y2, stroke, dash = '') => line(x1, y1, x2, y2, { stroke, w: 1.5, dash });
  const key = (x, y, mark, label) => [mark, T(x + 28, y, label, { ...small, anchor: 'start' })].join('\n');
  const TINT = '#E4DCF0';

  // a: the states of the environment. The focus is a partition (tinted cells); each witness distinguishes
  // a partition of its own (lines); a boundary of the focus that no witness's line follows is unresolved.
  o.push(panel(8, 20, 'a')); o.push(T(112, 20, 'perception', { weight: 600 }));
  let x = 30, y = 36, s = 164;
  o.push(cell(x, y, s / 2, s / 2, TINT)); o.push(cell(x + s / 2, y + s / 2, s / 2, s / 2, TINT));
  o.push(border(x, y, s));
  o.push(thin(x + s / 3, y, x + s / 3, y + s, C.blue)); o.push(thin(x + 2 * s / 3, y, x + 2 * s / 3, y + s, C.blue));
  o.push(thin(x, y + s / 2, x + s, y + s / 2, C.moss, '5 3'));
  o.push(thick(x + s / 2, y, x + s / 2, y + s, C.rose));
  o.push(T(x + s / 2, y + s + 16, 'states of the environment', mute));
  o.push(key(30, 236, [cell(30, 227, 11, 10, TINT), R(30, 227, 22, 10, { rx: 0, fill: 'none', stroke: C.line, sw: 0.8 })].join('\n'), 'focus'));
  o.push(key(30, 254, thin(30, 250, 52, 250, C.blue), 'witness 1'));
  o.push(key(30, 272, thin(30, 268, 52, 268, C.moss, '5 3'), 'witness 2'));
  o.push(key(30, 290, thick(30, 286, 52, 286, C.rose), 'unresolved'));

  // b: the variants of an artefact. Quality is a partition (tinted cells); the payers distinguish
  // partitions of their own (lines); a boundary of quality that no payer's line follows is left to drift.
  o.push(panel(246, 20, 'b')); o.push(T(350, 20, 'selection', { weight: 600 }));
  x = 268;
  o.push(cell(x, y + 0.6 * s, s, 0.4 * s, TINT));
  o.push(border(x, y, s));
  o.push(thin(x + 0.42 * s, y, x + 0.42 * s, y + s, C.sand));
  o.push(thin(x + 0.42 * s, y + 0.6 * s, x + s, y + 0.6 * s, C.lilac, '5 3'));
  o.push(thick(x, y + 0.6 * s, x + 0.42 * s, y + 0.6 * s, C.rose));
  o.push(T(x + s / 2, y + s + 16, 'variants of an artefact', mute));
  o.push(key(268, 236, [cell(268, 227, 11, 10, TINT), R(268, 227, 22, 10, { rx: 0, fill: 'none', stroke: C.line, sw: 0.8 })].join('\n'), 'quality'));
  o.push(key(268, 254, thin(268, 250, 290, 250, C.sand), 'harness'));
  o.push(key(268, 272, thin(268, 268, 290, 268, C.lilac, '5 3'), 'challenger'));
  o.push(key(268, 290, thick(268, 286, 290, 286, C.rose), 'drift'));

  // c: a genome in parts; for each part the rate at which its best alternative changes and its rate of variation
  o.push(panel(484, 20, 'c')); o.push(T(598, 20, 'variation', { weight: 600 }));
  const rates = [0.86, 0.5, 0.2, 0.07];
  rates.forEach((r, k) => {
    const yy = 44 + k * 40;
    o.push(B(506, yy, 54, 30, `part ${k + 1}`, { size: 11.5 }));
    o.push(R(572, yy + 3, 116 * r, 10, { rx: 0, fill: C.moss, stroke: C.moss, sw: 0.6 }));
    o.push(R(572, yy + 17, 116 * r, 10, { rx: 0, fill: C.lilac, stroke: C.lilac, sw: 0.6 }));
  });
  o.push(R(506, 232, 22, 9, { rx: 0, fill: C.moss, stroke: C.moss, sw: 0.6 })); o.push(T(534, 240, 'rate of change', { ...small, anchor: 'start' }));
  o.push(R(506, 250, 22, 9, { rx: 0, fill: C.lilac, stroke: C.lilac, sw: 0.6 })); o.push(T(534, 258, 'rate of variation', { ...small, anchor: 'start' }));
  o.push(T(506, 280, 'price of a change', { ...small, anchor: 'start' }));
  o.push(T(506, 299, 'ln((<tspan font-style="italic">K</tspan><tspan dy="3" font-size="0.76em" font-style="italic">j</tspan><tspan dy="-3"> − 1)/</tspan><tspan font-style="italic">μ</tspan><tspan dy="3" font-size="0.76em" font-style="italic">j</tspan><tspan dy="-3">)</tspan>', { size: 12.5, anchor: 'start' }));
  return frame(W, H, 'Three resolutions: of perception, of selection and of variation', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 4. The rungs of mechanism, and the founding of a garden on initial materials.
function rungsAndFounding() {
  const H = 338, o = [];
  o.push(panel(8, 20, 'a'));
  const rungs = [['check', ['facts']], ['warrant', ['stakes']], ['lineage', ['energy,', 'memory']], ['heredity', ['division']], ['archipelago', ['migration']]];
  const tones = ['#FBF8F2', C.tSand, C.tRose, C.tOrchid, C.tLilac];
  rungs.forEach(([name, adds], k) => {
    const x = 34 + k * 84, y = 244 - k * 46;
    o.push(R(x, y, 84, 290 - y, { rx: 0, fill: tones[k], stroke: C.line }));
    o.push(T(x + 42, y + 19, name, { size: 12.5, weight: 600 }));
    adds.forEach((a, i) => o.push(T(x + 42, y + 34 + i * 14, a, { size: 11.5, fill: C.ink2 })));
    o.push(T(x + 42, 306, String(k), small));
  });
  o.push(T(244, 324, 'rung', mute));

  o.push(panel(476, 20, 'b'));
  const steps = [['initial materials', 'publications, code, hybrid objects'], ['inventory', 'kinds, units, links'],
    ['sequencing', 'genomes, tested by regeneration'], ['harness', 'checks, their strength, references'],
    ['outside', 'sources, witnesses, focus'], ['founders', 'roles, foci, origins, life histories'], ['first population', '']];
  steps.forEach(([name, sub], k) => {
    const y = 14 + k * 46;
    const edge = k === 0 ? { fill: C.tMoss, stroke: C.moss } : k === 6 ? { fill: C.tOrchid, stroke: C.orchid } : {};
    o.push(B(500, y, 204, 36, name, sub ? { sub, subsize: 11, ...edge } : edge));
    if (k < 6) o.push(A(`M602,${y + 36} V${y + 44}`));
  });
  return frame(W, H, 'Rungs of mechanism and the founding of a garden', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 5. A garden over a repository: each part of a repository with the part of the framework it is.
function gardenOverARepository() {
  const H = 336, o = [];
  const dot = (x, y, fill = C.line) => `  <circle cx="${x}" cy="${y}" r="3.4" fill="${fill}"/>`;
  o.push(B(34, 18, 120, 40, 'reports', { sub: 'witnesses', fill: C.tMoss, stroke: C.moss }));
  o.push(B(200, 18, 120, 40, 'issues', { sub: 'needs, bounties', fill: C.tRose, stroke: C.rose }));
  o.push(A('M154,38 H198', 'moss', { w: 1.2 }));

  // the main branch and two branches of lineages
  o.push(line(40, 284, 560, 284, { stroke: C.line, w: 1.6 }));
  [70, 130, 210, 300, 400, 480, 548].forEach((x) => o.push(dot(x, 284)));
  o.push(T(40, 304, 'main branch', { ...small, anchor: 'start', weight: 600 })); o.push(T(40, 320, 'bedrock', { ...mute, anchor: 'start' }));
  o.push(`  <path d="M130,284 C150,284 150,214 176,214 H388" fill="none" stroke="${C.orchid}" stroke-width="1.5"/>`);
  [200, 262, 330, 388].forEach((x) => o.push(dot(x, 214, C.orchid)));
  o.push(`  <path d="M210,284 C232,284 228,132 256,132 H420" fill="none" stroke="${C.lilac}" stroke-width="1.5"/>`);
  [280, 344, 420].forEach((x) => o.push(dot(x, 132, C.lilac)));
  o.push(T(276, 234, 'branch of a lineage', { ...small, anchor: 'start' })); o.push(T(276, 249, 'world', { ...mute, anchor: 'start' }));
  o.push(T(294, 121, 'branch of a second lineage', { ...small, anchor: 'start' }));
  o.push(A('M262,210 C262,180 280,170 280,138', 'lilac', { w: 1.1, dash: DOT })); o.push(T(288, 180, 'carrying', { ...small, anchor: 'start' }));
  o.push(A('M222,58 C222,96 200,150 200,208', 'rose', { w: 1.2 })); o.push(A('M270,58 C276,84 280,104 280,126', 'rose', { w: 1.2 }));

  // a change with its claims, the pipeline, a challenger, the maintainers
  o.push(B(404, 192, 108, 44, 'pull request', { sub: 'claims, stakes', fill: C.tOrchid, stroke: C.orchid }));
  o.push(A('M388,214 H402', 'orchid'));
  o.push(B(404, 60, 108, 40, 'challenger', { sub: 'counterexample' }));
  o.push(B(566, 124, 122, 44, 'pipeline', { sub: 'harness, facts', fill: C.tSand, stroke: C.sand }));
  o.push(A('M512,206 C540,206 540,160 564,154', 'sand', { w: 1.2 }));
  o.push(A('M512,80 C540,80 548,118 564,134', 'ink', { w: 1.1 }));
  o.push(B(566, 208, 122, 44, 'maintainers', { sub: 'stewards', fill: BLUE, stroke: C.blue }));
  o.push(A('M627,168 V206', 'sand', { w: 1.2 }));
  o.push(A('M627,252 C627,284 600,284 566,284', 'blue', { w: 1.4 }));
  o.push(T(634, 276, 'merge', { ...small, anchor: 'start' })); o.push(T(634, 291, 'harvest', { ...mute, anchor: 'start' }));
  o.push(B(566, 300, 58, 24, 'journal', { size: 11.5 })); o.push(B(630, 300, 58, 24, 'ledger', { size: 11.5 }));
  return frame(W, H, 'A garden over a repository', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 6. Inquiry with the person inside: reserved acts, lineages' acts and the standing of statements.
function inquiry() {
  const H = 404, o = [];
  o.push(R(12, 14, 150, 376, { rx: 8, fill: BLUE, stroke: C.blue, dash: DASH }));
  o.push(T(87, 36, 'inquirer', { weight: 600 }));
  const act = (y, name, sub) => B(27, y, 120, sub ? 40 : 30, name, sub ? { sub, stroke: C.blue, subsize: 11 } : { stroke: C.blue });
  o.push(act(50, 'ask')); o.push(act(96, 'answer', 'written first')); o.push(act(152, 'conjecture', 'with a stake'));
  o.push(act(224, 'read')); o.push(act(278, 'accept', 'by restatement')); o.push(act(346, 'sign'));

  o.push(R(568, 14, 140, 228, { rx: 8, fill: C.tOrchid, stroke: C.orchid }));
  o.push(T(638, 36, 'lineages', { weight: 600 }));
  ['propose', 'derive', 'connect', 'draft', 'refute'].forEach((name, k) => o.push(B(578, 50 + k * 37, 120, 28, name)));

  o.push(B(214, 44, 160, 42, 'question', { sub: 'need, energy', fill: C.tRose, stroke: C.rose }));
  o.push(A('M147,65 H212', 'rose', { w: 1.3 })); o.push(A('M374,65 H566', 'rose', { w: 1.3 }));
  o.push(B(214, 100, 160, 32, 'journal', { fill: C.tSand, stroke: C.sand }));
  o.push(A('M147,116 H212', 'blue', { w: 1.1 })); o.push(T(180, 109, 'seal', small));
  o.push(B(390, 100, 110, 32, 'harness', { fill: C.tSand, stroke: C.sand }));
  o.push(A('M445,132 V148', 'sand', { w: 1.2 }));

  o.push(R(214, 150, 286, 104, { rx: 6, fill: C.white, stroke: C.line }));
  o.push(T(357, 170, 'statements with their standing', { size: 12.5, weight: 600 }));
  [['proved', 226, 58], ['refuted', 290, 62], ['arguable', 358, 66], ['open', 430, 46]].forEach(([n, x, w]) => o.push(chip(x, 182, w, n, { h: 24 })));
  o.push(chip(226, 214, 180, 'supported within a bound', { h: 24 }));
  o.push(A('M147,172 H212', 'rose', { w: 1.2 })); o.push(T(180, 165, 'stake', small));
  o.push(A('M568,170 H502', 'lilac', { w: 1.2, dash: DOT })); o.push(T(535, 163, 'claims', small));
  o.push(A('M500,212 H576', 'rose', { w: 1.2 })); o.push(T(535, 205, 'stake won', small));
  o.push(A('M214,239 H149', 'lilac', { w: 1.2, dash: DOT }));

  o.push(R(214, 272, 286, 60, { rx: 6, fill: C.tLilac, stroke: C.lilac }));
  o.push(T(226, 292, 'theory', { size: 12.5, weight: 600, anchor: 'start' }));
  o.push(B(290, 284, 96, 36, 'genome', { sub: 'core', subsize: 11 })); o.push(B(394, 284, 96, 36, 'body', { sub: 'results', subsize: 11 }));
  o.push(A('M147,298 H212', 'blue', { w: 1.3 }));

  o.push(B(568, 268, 112, 44, 'readers, records', { sub: 'witnesses', size: 12.5, subsize: 11, fill: C.tMoss, stroke: C.moss }));
  o.push(A('M568,290 C540,290 532,246 502,246', 'moss', { w: 1.2 }));
  o.push(B(568, 346, 140, 34, 'paper'));
  o.push(A('M147,361 H566', 'blue', { w: 1.3 }));
  o.push(A('M694,242 V344', 'lilac', { w: 1.1, dash: DOT }));
  o.push(A('M624,346 V314', 'ink', { w: 1 }));
  return frame(W, H, 'Inquiry with the person inside', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 7. Results of the prototypes (computed from ../prototypes/results/).
function axes(o, { x, y, w, h, xr, yr, xt = [], yt = [], xl = '', yl = '', xf = String, yf = String }) {
  const X = (v) => x + (v - xr[0]) / (xr[1] - xr[0]) * w, Y = (v) => y + h - (v - yr[0]) / (yr[1] - yr[0]) * h;
  o.push(line(x, y, x, y + h, { stroke: C.line })); o.push(line(x, y + h, x + w, y + h, { stroke: C.line }));
  for (const v of xt) { o.push(line(X(v), y + h, X(v), y + h + 4, { stroke: C.line })); o.push(T(X(v), y + h + 16, xf(v), small)); }
  for (const v of yt) { o.push(line(x - 4, Y(v), x, Y(v), { stroke: C.line })); o.push(T(x - 7, Y(v) + 4, yf(v), { ...small, anchor: 'end' })); }
  if (xl) o.push(T(x + w / 2, y + h + 33, xl, small));
  if (yl) o.push(T(x - 34, y + h / 2, yl, { ...small, rotate: -90 }));
  return [X, Y];
}
const poly = (pts, stroke, o = {}) => `  <polyline points="${pts.map(([a, b]) => `${a.toFixed(1)},${b.toFixed(1)}`).join(' ')}" fill="none" stroke="${stroke}" stroke-width="${o.w || 1.3}"${o.dash ? ` stroke-dasharray="${o.dash}"` : ''} stroke-linejoin="round"/>`;
const disc = (x, y, fill, r = 3.2, stroke = fill) => `  <circle cx="${x.toFixed(1)}" cy="${y.toFixed(1)}" r="${r}" fill="${fill}" stroke="${stroke}" stroke-width="1.1"/>`;

function prototypes() {
  const H = 484, o = [];
  const p1 = result('p1.json'), p2 = result('p2.json'), chain = result('p2-chain.json'), p4 = result('p4.json');
  const cross = (x, y, stroke) => [line(x - 3, y - 3, x + 3, y + 3, { stroke, w: 1.4 }), line(x - 3, y + 3, x + 3, y - 3, { stroke, w: 1.4 })].join('\n');

  // a: regeneration of a unit from four genomes; a development whose body did not load is a cross on the axis
  o.push(panel(8, 20, 'a'));
  const groups = [['identity', ['identity']], ['identity+laws', ['identity,', 'laws']], ['identity+exemplars', ['identity,', 'exemplars']], ['all parts', ['all parts']]];
  let [X, Y] = axes(o, { x: 62, y: 30, w: 270, h: 150, xr: [0, 4], yr: [33, 43], yt: [34, 36, 38, 40, 42], yl: 'checks passed, of 42' });
  groups.forEach(([key, label], k) => {
    const cheap = (p2.regeneration[`${key} | cheap developer`] || { passed: [] }).passed;
    const careful = (p2.regeneration[`${key} | careful developer`] || { passed: [] }).passed;
    cheap.forEach((v, i) => o.push(v > 0 ? disc(X(k + 0.5) - 15 + i * 8, Y(v), C.lilac) : cross(X(k + 0.5) - 15 + i * 8, 173, C.lilac)));
    careful.forEach((v, i) => o.push(disc(X(k + 0.5) + 19 + i * 8, Y(v), C.white, 3.2, C.rose)));
    label.forEach((l, i) => o.push(T(X(k + 0.5), 196 + i * 14, l, small)));
    o.push(T(X(k + 0.5), 196 + label.length * 14, `${p2.regeneration[`${key} | cheap developer`].genome_words} words`, mute));
  });
  o.push(disc(196, 112, C.lilac)); o.push(T(206, 116, 'cheap developer', { ...small, anchor: 'start' }));
  o.push(disc(196, 128, C.white, 3.2, C.rose)); o.push(T(206, 132, 'careful developer', { ...small, anchor: 'start' }));
  o.push(cross(196, 144, C.lilac)); o.push(T(206, 148, 'not loaded', { ...small, anchor: 'start' }));

  // b: reseeding in chains of ten generations; the four series are displaced by less than half a check
  o.push(panel(366, 20, 'b'));
  [X, Y] = axes(o, { x: 420, y: 30, w: 270, h: 150, xr: [0.5, 10.5], yr: [18, 44], xt: [1, 2, 3, 4, 5, 6, 7, 8, 9, 10], yt: [20, 26, 32, 38, 42], xl: 'generation', yl: 'checks kept, of 42' });
  const shift = [-0.75, 0.75, 2.25];
  chain.unanchored.forEach((steps, i) => {
    const pts = steps.map((s) => [X(s.generation), Y(s.kept) + shift[i]]);
    o.push(poly(pts, C.rose, { dash: '5 3' })); pts.forEach(([a, b]) => o.push(disc(a, b, C.rose, 2.3)));
  });
  { const pts = chain.anchored.map((s) => [X(s.generation), Y(s.kept) - 2.25]);
    o.push(poly(pts, C.lilac, { w: 1.6 })); pts.forEach(([a, b]) => o.push(disc(a, b, C.lilac, 2.6))); }
  o.push(line(436, 150, 458, 150, { stroke: C.lilac, w: 1.6 })); o.push(T(464, 154, 'anchored by the checks', { ...small, anchor: 'start' }));
  o.push(line(436, 166, 458, 166, { stroke: C.rose, w: 1.3, dash: '5 3' })); o.push(T(464, 170, 'without an anchor', { ...small, anchor: 'start' }));

  // c: heredity of conduct over six generations, by the founder's organ of reproduction
  o.push(panel(8, 278, 'c'));
  [X, Y] = axes(o, { x: 62, y: 288, w: 270, h: 150, xr: [0, 6], yr: [0.4, 1.02], xt: [0, 1, 2, 3, 4, 5, 6], yt: [0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1], xl: 'generation', yl: 'conduct like the founder', yf: (v) => v.toFixed(1) });
  for (const l of p1.lines.filter((q) => q.model === 'cheap')) {
    const colour = l.founder === 'bold' ? C.rose : C.lilac;
    const pts = [[X(0), Y(1)], ...l.steps.filter((s) => s.resemblance_founder != null).map((s) => [X(s.generation), Y(s.resemblance_founder)])];
    o.push(poly(pts, colour, { w: 1.2, dash: l.founder === 'bold' ? '5 3' : '' })); pts.slice(1).forEach(([a, b]) => o.push(disc(a, b, colour, 2.3)));
  }
  o.push(line(78, 406, 100, 406, { stroke: C.lilac, w: 1.3 })); o.push(T(106, 410, 'conservative founder', { ...small, anchor: 'start' }));
  o.push(line(78, 422, 100, 422, { stroke: C.rose, w: 1.3, dash: '5 3' })); o.push(T(106, 426, 'bold founder', { ...small, anchor: 'start' }));

  // d: growth lost under three schemes of variation, against the type matched to the environment at every turn
  o.push(panel(366, 278, 'd'));
  const matched = p4.growth_of_the_matched_type;
  const rows = [['part by part, own rates', 'part by part, each at its own rate S_j/(T-1)', C.lilac],
    ['part by part, one rate', 'part by part, one rate for all parts (best)', C.orchid], ['whole type, one rate', 'the whole type at one rate (best)', C.rose]];
  const scale = 250 / 0.18;
  o.push(line(420, 302, 420, 438, { stroke: C.line })); o.push(line(420, 438, 690, 438, { stroke: C.line }));
  [0, 0.05, 0.1, 0.15].forEach((v) => { o.push(line(420 + v * scale, 438, 420 + v * scale, 442, { stroke: C.line })); o.push(T(420 + v * scale, 454, v.toFixed(2), small)); });
  o.push(T(555, 471, 'growth lost against the matched type', small));
  rows.forEach(([label, key, colour], k) => {
    const lost = matched - p4.rows[key].growth, y = 314 + k * 38;
    o.push(R(420, y, lost * scale, 16, { rx: 0, fill: colour, stroke: colour, sw: 0.6 }));
    o.push(T(426 + lost * scale, y + 12, lost.toFixed(3), { ...small, anchor: 'start' }));
    o.push(T(424, y + 30, label, { ...small, anchor: 'start', fill: C.ink2 }));
  });
  return frame(W, H, 'Results of the prototypes', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 8. The science infrastructure as a site: what the repository holds of each part, and what it lacks.
function infrastructureAsSite() {
  const rows = [
    ['history, decision log, release notes', 'record', 'a sealed journal'],
    ['suites of checks', 'facts', 'keys by state and harness'],
    ['sessions in one tree', 'lineages', 'energy, upkeep, term'],
    ['guides, descriptions, theory notes', 'genomes', 'bounds, regeneration'],
    ['checks, references, declared dependence', 'claims', 'stakes, standing'],
    ['review, audits of drafts', 'challenge', 'lineages that refute'],
    ['acceptance', 'value', 'carrying, premiums, later use'],
    ['sources, dates, marks of verification', 'evidential chain', 'chains of feeds, witnesses'],
    ['agreement before a push', 'membrane', 'a sandbox, a fixed budget'],
    ['gallery, counts of checks', 'observatory', 'readings'],
  ];
  const H = 44 + rows.length * 33 + 6, o = [];
  o.push(T(131, 26, 'present in the repository', { weight: 600 }));
  o.push(T(360, 26, 'part of the framework', { weight: 600 }));
  o.push(T(589, 26, 'lacking', { weight: 600 }));
  rows.forEach(([have, part, lack], k) => {
    const y = 40 + k * 33;
    o.push(B(12, y, 238, 26, have, { size: 11.5, fill: C.tMoss, stroke: C.moss }));
    o.push(B(290, y, 140, 26, part, { size: 12, weight: 600, fill: C.tLilac, stroke: C.lilac }));
    o.push(B(470, y, 238, 26, lack, { size: 11.5, stroke: C.rose, dash: DASH }));
    o.push(line(250, y + 13, 290, y + 13, { stroke: C.moss, w: 1.1 }));
    o.push(line(430, y + 13, 470, y + 13, { stroke: C.rose, w: 1, dash: '3 3' }));
  });
  return frame(W, H, 'The science infrastructure as a site', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 9. Results of the case studies (computed from ../case-studies/results/).
function caseStudies() {
  const H = 508, o = [];
  const study = (name) => JSON.parse(readFileSync(join(here, '..', 'case-studies', 'results', name), 'utf8'));
  const kills = study('a1-kills.json'), s = study('a-summary.json').corrected;
  const units = [['sphere', 'sphere'], ['sun', 'Sun'], ['climate', 'climates']];
  const bar = (x, y, w, h, fill) => R(x, y, Math.max(w, 0.6), h, { rx: 0, fill, stroke: fill, sw: 0.6 });

  // a: share of distinct mutants that the repository's checks kill
  o.push(panel(8, 20, 'a'));
  o.push(line(96, 34, 96, 150, { stroke: C.line })); o.push(line(96, 150, 332, 150, { stroke: C.line }));
  [0, 25, 50, 75, 100].forEach((v) => { o.push(line(96 + v * 2.3, 150, 96 + v * 2.3, 154, { stroke: C.line })); o.push(T(96 + v * 2.3, 166, String(v), small)); });
  o.push(T(214, 184, 'distinct mutants killed by the checks, percent', small));
  units.forEach(([key, label], k) => {
    const u = kills[key], y = 44 + k * 36, share = 100 * u.killed / u.distinct;
    o.push(T(88, y + 13, label, { ...small, anchor: 'end' }));
    o.push(bar(96, y, share * 2.3, 18, C.sand));
    o.push(T(102 + share * 2.3, y + 13, `${u.killed} of ${u.distinct}`, { ...small, anchor: 'start' }));
  });

  // b: functions sound, by unit and by genome
  o.push(panel(366, 20, 'b'));
  o.push(line(420, 34, 420, 150, { stroke: C.line })); o.push(line(420, 150, 690, 150, { stroke: C.line }));
  [0, 0.5, 1].forEach((v) => { o.push(line(416, 150 - v * 110, 420, 150 - v * 110, { stroke: C.line })); o.push(T(413, 154 - v * 110, v.toFixed(1), { ...small, anchor: 'end' })); });
  o.push(T(386, 92, 'functions sound', { ...small, rotate: -90 }));
  const genomes = [['identity', C.rose], ['repository', C.sand], ['sequenced', C.lilac]];
  units.forEach(([key, label], k) => {
    const x0 = 444 + k * 84;
    genomes.forEach(([genome, colour], g) => {
      const cells = ['cheap', 'careful'].map((m) => s.units[key].regeneration[`${genome} | ${m}`]);
      const share = cells.reduce((a, c) => a + c.sound, 0) / cells.reduce((a, c) => a + c.functions, 0);
      o.push(bar(x0 + g * 20, 150 - share * 110, 16, share * 110, colour));
    });
    o.push(T(x0 + 28, 166, label, small));
  });
  [['identity alone', C.rose, 426, 178], ["the repository's texts", C.sand, 536, 178], ['sequenced genome', C.lilac, 426, 194]].forEach(([label, colour, x, y]) => {
    o.push(bar(x, y, 10, 9, colour)); o.push(T(x + 14, y + 8, label, { size: 11, anchor: 'start' }));
  });

  // c: faulty functions that reach a person under each filter
  o.push(panel(8, 240, 'c'));
  const m6 = s.M6_what_reaches_a_person, w6 = s.M6_without_the_checks;
  const rows = [['everything developed', m6.no_filter.faulty, C.mute], ['the checks', m6.the_checks.faulty, C.sand],
    ['the checks, the reviewer', m6.the_checks_and_the_reviewer.faulty, C.sand], ['the checks, a challenger', m6.the_checks_and_the_challenger_of_the_other_model.faulty, C.sand],
    ['no checks: the reviewer', w6.the_reviewer_alone.faulty, C.rose], ['no checks: challenger, same model', w6.the_challenger_of_the_same_model_alone.faulty, C.lilac],
    ['no checks: challenger, other model', w6.the_challenger_of_the_other_model_alone.faulty, C.lilac], ['no checks: both challengers', w6.both_challengers.faulty, C.lilac]];
  rows.forEach(([label, value, colour], k) => {
    const y = 254 + k * 25;
    o.push(T(232, y + 12, label, { size: 11, anchor: 'end' }));
    o.push(bar(238, y + 2, value * 1.5, 14, colour));
    o.push(T(244 + value * 1.5, y + 13, String(value), { ...small, anchor: 'start' }));
  });
  o.push(T(190, 474, `faulty functions shown to a person, of ${m6.no_filter.faulty}`, small));

  // d: what the challengers gained or lost, by unit
  o.push(panel(366, 240, 'd'));
  const zero = 360, scale = 1.3;
  o.push(line(420, 262, 420, 450, { stroke: C.line })); o.push(line(420, zero, 690, zero, { stroke: C.line }));
  [-60, -30, 0, 30, 60].forEach((v) => { o.push(line(416, zero - v * scale, 420, zero - v * scale, { stroke: C.line })); o.push(T(413, zero + 4 - v * scale, String(v), { ...small, anchor: 'end' })); });
  o.push(T(380, 356, 'energy gained by the challengers', { ...small, rotate: -90 }));
  units.forEach(([key, label], k) => {
    const x0 = 452 + k * 84, u = s.by_unit[key];
    [['same', C.orchid], ['other', C.lilac]].forEach(([role, colour], g) => {
      const v = u[role].net;
      o.push(bar(x0 + g * 22, v >= 0 ? zero - v * scale : zero, 18, Math.abs(v) * scale, colour));
      o.push(T(x0 + g * 22 + 9, v >= 0 ? zero - v * scale - 4 : zero + Math.abs(v) * scale + 12, String(v), { size: 11 }));
    });
    o.push(T(x0 + 20, 466, label, small));
    o.push(T(x0 + 20, 480, `${u.faulty} of ${u.functions}`, { size: 11, fill: C.mute }));
  });
  o.push(T(555, 496, 'faulty functions of the unit', { size: 11, fill: C.mute }));
  o.push(bar(430, 264, 10, 9, C.orchid)); o.push(T(444, 272, "challenger of the producer's model", { size: 11, anchor: 'start' }));
  o.push(bar(430, 279, 10, 9, C.lilac)); o.push(T(444, 287, 'challenger of the other model', { size: 11, anchor: 'start' }));
  return frame(W, H, 'Results of the case studies', o.join('\n'));
}

const figures = {
  1: ['fig-1-layers', layers, 'drawn'],
  2: ['fig-2-genome-and-body', genomeAndBody, 'drawn'],
  3: ['fig-3-three-resolutions', threeResolutions, 'schematic'],
  4: ['fig-4-rungs-and-founding', rungsAndFounding, 'drawn'],
  5: ['fig-5-garden-over-a-repository', gardenOverARepository, 'drawn'],
  6: ['fig-6-inquiry', inquiry, 'drawn'],
  7: ['fig-7-prototypes', prototypes, 'computed from ../prototypes/results/'],
  8: ['fig-8-infrastructure-as-site', infrastructureAsSite, 'drawn'],
  9: ['fig-9-case-studies', caseStudies, 'computed from ../case-studies/results/'],
};

const want = process.argv.slice(2).map(Number).filter(Boolean);
const manifestPath = join(here, 'manifest.json');
let manifest = { note: 'The SVG is the source of each figure; the PDF and PNG are made from it by render.mjs. All figures are made by figures.mjs.', figures: [] };
try { manifest = JSON.parse(readFileSync(manifestPath, 'utf8')); } catch { /* first run */ }
const byNumber = new Map((manifest.figures || []).map((f) => [f.figure, f]));
for (const [n, [name, make, kind]] of Object.entries(figures)) {
  if (want.length && !want.includes(Number(n))) continue;
  const file = join(here, name + '.svg');
  writeFileSync(file, make());
  byNumber.set(Number(n), { figure: Number(n), file: name + '.svg', kind, sha256: createHash('sha256').update(readFileSync(file)).digest('hex') });
  console.log('wrote', name + '.svg');
}
manifest.figures = [...byNumber.values()].filter((f) => figures[f.figure] && figures[f.figure][0] + '.svg' === f.file).sort((a, b) => a.figure - b.figure);
writeFileSync(manifestPath, JSON.stringify(manifest, null, 1) + '\n');
