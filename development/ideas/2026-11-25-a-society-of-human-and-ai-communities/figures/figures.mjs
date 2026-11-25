// Makes the fourteen figures of the design as SVG:  node figures.mjs   (then node render.mjs)
//   node figures.mjs 3 9      makes figures 3 and 9 only
//
// Every figure is drawn on lib.mjs (720 units wide, no lettering under 11 units) and carries formal
// labels and notation only; what explains it is in its caption (README.md). Figures 10, 11 and 14 are
// computed from the sketch's results (../sketch/results.json).

import { readFileSync, writeFileSync } from 'node:fs';
import { fileURLToPath } from 'node:url';
import { dirname, join } from 'node:path';
import { W, C, frame, T, R, B, A, line, panel, DOT, DASH } from './lib.mjs';

const here = dirname(fileURLToPath(import.meta.url));
const results = JSON.parse(readFileSync(join(here, '..', 'sketch', 'results.json'), 'utf8')).numbers;
const small = { size: 11.5 };
const mute = { size: 11.5, fill: C.mute };
const BLUE = '#EEF3F9';
const chip = (x, y, w, label, o = {}) => B(x, y, w, o.h || 22, label, { size: 11.5, ...o });
const disc = (x, y, r, o = {}) =>
  `  <circle cx="${x}" cy="${y}" r="${r}" fill="${o.fill || C.white}" stroke="${o.stroke || C.line}" stroke-width="${o.sw || 1}"/>`;
const node = (x, y, letter, o = {}) => [disc(x, y, 12, o), T(x, y + 4.2, letter, { size: 12 })].join('\n');

// ---------------------------------------------------------------------------------------------
// 1. The levels of order
function levels() {
  const H = 410, o = [];
  o.push(T(588, 20, 'shared by', mute));
  o.push(T(676, 20, 'binds by', mute));
  const band = (y, h, title, fill, stroke, dash, shared, binds) => {
    o.push(R(40, y, 470, h, { rx: 6, fill, stroke, dash }));
    o.push(T(52, y + 21, title, { weight: 600, anchor: 'start' }));
    o.push(T(588, y + h / 2 + 4, shared, small));
    o.push(T(676, y + h / 2 + 4, binds, small));
  };
  band(30, 70, 'practices', C.tOrchid, C.orchid, '', 'a scene', 'reliance');
  ['scenes', 'protocols', 'readings', 'cases'].forEach((n, k) => o.push(chip(52 + k * 112, 62, 100, n)));
  band(112, 96, 'constitutions', C.tLilac, C.lilac, '', 'a community', 'entry');
  ['position', 'boundary', 'choice', 'aggregation'].forEach((n, k) => o.push(chip(52 + k * 112, 144, 100, n)));
  ['information', 'payoff', 'scope', 'amendment'].forEach((n, k) => o.push(chip(52 + k * 112, 174, 100, n)));
  band(220, 70, 'compacts and leagues', BLUE, C.blue, DASH, 'the parties', 'ratification');
  ['recognition', 'envoys', 'interfaces', 'offices'].forEach((n, k) => o.push(chip(52 + k * 112, 252, 100, n)));
  band(302, 96, 'ground', C.tSand, C.sand, '', 'all', 'conformance');
  ['acts', 'chronicle', 'fold'].forEach((n, k) => o.push(chip(52 + k * 98, 334, 86, n)));
  o.push(T(275, 374, 'record · standing · answerability · guarded resources', mute));
  o.push(T(275, 390, 'separate registers · rules as data · exit · route for claims', mute));
  return frame(W, H, 'The levels of order', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 2. Parties, positions, mandates and origins
function parties() {
  const H = 330, o = [];
  o.push(R(20, 34, 380, 136, { rx: 6, fill: 'none', stroke: C.mute, dash: DASH }));
  o.push(T(32, 52, 'origin', { ...mute, anchor: 'start' }));
  o.push(B(36, 84, 90, 42, 'person', { sub: 'principal', fill: C.tOrchid, stroke: C.orchid }));
  o.push(B(184, 62, 96, 32, 'operative'));
  o.push(B(184, 122, 96, 32, 'operative'));
  o.push(B(306, 62, 84, 32, 'operative'));
  o.push(A('M126,96 C156,96 156,78 182,78', 'orchid', { w: 1.2 }));
  o.push(A('M126,112 C156,112 156,138 182,138', 'orchid', { w: 1.2 }));
  o.push(T(146, 68, 'mandate', small));
  o.push(A('M280,78 H304', 'orchid', { w: 1.2 }));
  o.push(T(340, 112, 'narrower mandate', small));

  o.push(R(20, 190, 380, 122, { rx: 6, fill: 'none', stroke: C.mute, dash: DASH }));
  o.push(T(32, 208, 'origin', { ...mute, anchor: 'start' }));
  o.push(B(36, 232, 104, 42, 'organisation', { sub: 'principal', fill: C.tOrchid, stroke: C.orchid }));
  [218, 250, 282].forEach((y) => o.push(B(184, y, 96, 24, 'operative', { size: 12 })));
  o.push(A('M140,246 C162,246 162,230 182,230', 'orchid', { w: 1.2 }));
  o.push(A('M140,253 H182', 'orchid', { w: 1.2 }));
  o.push(A('M140,260 C162,260 162,294 182,294', 'orchid', { w: 1.2 }));

  o.push(R(470, 62, 230, 180, { rx: 8, fill: C.tLilac, stroke: C.lilac }));
  o.push(T(482, 82, 'community', { weight: 600, anchor: 'start' }));
  ['steward', 'reviewer', 'contributor', 'member'].forEach((n, k) => o.push(chip(482, 94 + k * 30, 104, n)));
  o.push(T(644, 112, 'positions', mute));
  o.push(chip(596, 196, 94, 'commitment', { fill: C.tRose, stroke: C.rose }));
  o.push(A('M280,138 C380,138 400,165 480,165', 'ink'));
  o.push(T(440, 142, 'holds', small));
  o.push(A('M280,262 C400,262 420,172 480,170', 'ink'));
  o.push(A('M81,84 C81,32 110,24 190,24 H684 C702,24 710,30 710,48 V197 C710,204 706,207 693,207', 'rose', { w: 1.2, dash: DASH }));
  o.push(T(400, 16, 'answers for', { ...small, fill: C.rose }));
  o.push(B(470, 268, 110, 30, 'model provider', { size: 12, fill: C.tSand, stroke: C.sand }));
  o.push(A('M470,286 C420,286 340,294 282,294', 'sand', { w: 1, dash: DOT }));
  o.push(T(438, 308, 'provides', small));
  o.push(T(650, 288, 'operatives 6', mute));
  o.push(T(650, 304, 'origins 2', mute));
  return frame(W, H, 'Parties, positions, mandates and origins', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 3. An act and the fold
function actAndFold() {
  const H = 320, o = [];
  o.push(R(20, 30, 200, 210, { rx: 4 }));
  o.push(T(32, 50, 'act', { weight: 600, anchor: 'start' }));
  o.push(R(32, 60, 176, 104, { rx: 3, fill: C.tLilac, stroke: C.lilac }));
  o.push(T(44, 78, 'envelope', { size: 12, weight: 600, anchor: 'start' }));
  [['kind', 'force'], ['author', 'mandate'], ['scene', 'references']].forEach((row, r) =>
    row.forEach((n, k) => o.push(chip(44 + k * 82, 86 + r * 24, 76, n, { h: 20 }))));
  o.push(R(32, 172, 176, 58, { rx: 3, stroke: C.soft }));
  o.push(T(44, 190, 'body', { size: 12, weight: 600, anchor: 'start' }));
  [[201, 150], [211, 128], [221, 104]].forEach(([y, w]) => o.push(line(44, y, 44 + w, y, { stroke: C.faint, w: 4 })));
  o.push(B(65, 270, 110, 38, 'readers', { sub: 'persons, models' }));
  o.push(A('M120,240 V268', 'mute', { dash: DOT }));

  o.push(T(365, 34, 'chronicle', small));
  for (let k = 0; k < 7; k += 1) {
    o.push(R(262 + k * 30, 44, 22, 22, { rx: 2, fill: k === 6 ? C.tLilac : C.white, stroke: k === 6 ? C.lilac : C.line }));
    if (k) o.push(line(254 + k * 30, 55, 262 + k * 30, 55, { stroke: C.line, w: 1 }));
  }
  o.push(B(330, 120, 70, 34, 'fold', { rx: 17, fill: C.tSand, stroke: C.sand }));
  o.push(A('M365,66 V118', 'ink'));
  o.push(A('M208,112 C262,112 280,137 328,137', 'lilac', { w: 1.3 }));
  o.push(T(262, 104, 'typed fields', small));
  o.push(A('M365,154 V186', 'rose'));
  o.push(B(305, 188, 120, 38, 'refusal', { sub: 'reason', stroke: C.rose }));
  o.push(A('M400,137 H470', 'ink'));

  o.push(R(472, 60, 228, 150, { rx: 4, fill: C.tOrchid, stroke: C.orchid }));
  o.push(T(484, 80, 'relational state', { weight: 600, anchor: 'start' }));
  [['positions', 'mandates'], ['commitments', 'grants'], ['claims', 'questions'], ['assessments', 'rules']].forEach((row, r) =>
    row.forEach((n, k) => o.push(chip(484 + k * 106, 90 + r * 28, 98, n))));
  o.push(T(476, 277, 'views', { ...mute, anchor: 'end' }));
  [['balances', 484], ['members', 558], ['reports', 632]].forEach(([n, x]) => {
    o.push(chip(x, 262, 66, n));
    o.push(A(`M${x + 33},212 V260`, 'mute', { dash: DASH, w: 0.9 }));
  });
  return frame(W, H, 'An act, the chronicle and the fold', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 4. The life of a commitment
function commitment() {
  const H = 300, o = [];
  o.push(R(188, 84, 272, 58, { rx: 6, fill: 'none', stroke: C.rose, dash: DASH }));
  o.push(T(300, 78, 'disputed, while a claim is open', { ...small, fill: C.rose, anchor: 'start' }));
  o.push(B(20, 96, 104, 34, 'conditional'));
  o.push(B(196, 96, 84, 34, 'active'));
  o.push(B(352, 96, 100, 34, 'presented'));
  o.push(B(540, 96, 108, 34, 'discharged', { fill: C.tMoss, stroke: C.moss }));
  o.push(A('M124,113 H194', 'lilac', { w: 1.3 })); o.push(T(159, 106, 'take', small));
  o.push(A('M280,113 H350', 'orchid', { w: 1.3 })); o.push(T(315, 106, 'deliver', small));
  o.push(A('M452,113 H538', 'lilac', { w: 1.3 })); o.push(T(495, 106, 'accept', small));
  o.push(A('M40,50 V94', 'orchid', { w: 1.3 })); o.push(T(48, 68, 'offer', { ...small, anchor: 'start' }));
  o.push(A('M238,42 V94', 'orchid', { w: 1.3 })); o.push(T(246, 60, 'commit', { ...small, anchor: 'start' }));

  o.push(B(20, 206, 104, 34, 'withdrawn', { stroke: C.mute }));
  o.push(B(150, 206, 96, 34, 'cancelled', { fill: C.tRose, stroke: C.rose }));
  o.push(B(270, 206, 110, 34, 'handed over', { fill: C.tSand, stroke: C.sand }));
  o.push(B(540, 206, 108, 34, 'released', { fill: C.tMoss, stroke: C.moss }));
  o.push(A('M72,130 V204', 'orchid', { w: 1.3 })); o.push(T(80, 172, 'cancel', { ...small, anchor: 'start' }));
  o.push(A('M212,130 V204', 'orchid', { w: 1.3 })); o.push(T(204, 172, 'cancel', { ...small, anchor: 'end' }));
  o.push(A('M262,130 C262,172 325,166 325,204', 'orchid', { w: 1.3 }));
  o.push(T(334, 178, 'hand over', { ...small, anchor: 'start' }));
  o.push(A('M380,223 H538', 'lilac', { w: 1.3 })); o.push(T(459, 216, 'release', small));
  o.push(A('M420,130 C440,176 560,160 584,204', 'lilac', { w: 1.3 })); o.push(T(520, 158, 'release', small));
  o.push(A('M198,240 C198,282 594,282 594,242', 'lilac', { w: 1.3 })); o.push(T(396, 288, 'release', small));

  o.push(line(560, 36, 584, 36, { stroke: C.orchid, w: 1.6 })); o.push(T(590, 40, 'act of the debtor', { ...small, anchor: 'start' }));
  o.push(line(560, 54, 584, 54, { stroke: C.lilac, w: 1.6 })); o.push(T(590, 58, 'act of the creditor', { ...small, anchor: 'start' }));
  return frame(W, H, 'The life of a commitment', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 5. A constitution as data
function constitution() {
  const H = 336, o = [];
  o.push(R(224, 110, 272, 130, { rx: 6 }));
  o.push(T(360, 180, 'action situation', { weight: 600 }));
  [['positions', 'acts', 'outcomes'], ['information', 'decisions', 'returns']].forEach((row, r) =>
    row.forEach((n, k) => o.push(chip(234 + k * 86, 126 + r * 70, 82, n, { h: 26 }))));
  const rule = (x, y, w, label) => chip(x, y, w, label, { h: 26, fill: C.tLilac, stroke: C.lilac });
  o.push(rule(180, 40, 96, 'position rules')); o.push(A('M228,66 C228,98 275,94 275,124', 'lilac'));
  o.push(rule(313, 40, 96, 'choice rules')); o.push(A('M361,66 V124', 'lilac'));
  o.push(rule(444, 40, 96, 'scope rules')); o.push(A('M492,66 C492,98 447,94 447,124', 'lilac'));
  o.push(rule(156, 290, 120, 'information rules')); o.push(A('M216,290 C216,256 275,260 275,224', 'lilac'));
  o.push(rule(299, 290, 124, 'aggregation rules')); o.push(A('M361,290 V224', 'lilac'));
  o.push(rule(444, 290, 96, 'payoff rules')); o.push(A('M492,290 C492,256 447,260 447,224', 'lilac'));
  o.push(rule(116, 126, 98, 'boundary rules')); o.push(A('M214,139 H232', 'lilac'));

  o.push(T(58, 176, 'levels', mute));
  ['constitutional', 'collective choice', 'operational'].forEach((n, k) =>
    o.push(chip(6, 186 + k * 44, 104, n, { h: 26, fill: C.tSand, stroke: C.sand })));
  o.push(A('M58,212 V228', 'sand')); o.push(A('M58,256 V272', 'sand'));
  o.push(T(58, 314, 'each changes', mute));
  o.push(T(58, 328, 'the next', mute));

  o.push(T(636, 100, 'declared beside', mute));
  ['scenes', 'circulation, receiving', 'records', 'external face', 'custody of means'].forEach((n, k) =>
    o.push(chip(566, 110 + k * 30, 140, n, { h: 24 })));
  o.push(line(496, 175, 566, 175, { dash: '3 3' }));
  return frame(W, H, 'A constitution as data', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 6. Eight presets as profiles
function presets() {
  const groups = [
    ['entry', ['open', 'assessment', 'membership', 'agreement'], C.orchid],
    ['decisions', ['steward or lead', 'members', 'panel by lot', 'envoys', 'distributed'], C.lilac],
    ['returns', ['acknowledgement', 'standing', 'shares', 'agreement', 'services'], C.moss],
    ['circulation', ['gift', 'pool', 'reciprocal', 'mutual credit', 'unit of account'], C.sand],
  ];
  const rows = [
    ['commons', ['open'], ['steward or lead'], ['acknowledgement'], ['gift', 'pool']],
    ['guild', ['assessment'], ['steward or lead', 'members'], ['standing'], ['reciprocal']],
    ['cooperative', ['membership'], ['members'], ['shares'], ['pool', 'mutual credit']],
    ['directed project', ['agreement'], ['steward or lead'], ['agreement'], ['gift']],
    ['polity', ['membership'], ['steward or lead', 'members', 'panel by lot'], ['shares', 'services'], ['pool', 'mutual credit', 'unit of account']],
    ['coalition', ['agreement'], ['envoys'], ['agreement'], ['reciprocal']],
    ['garden', ['open'], ['distributed'], [], ['unit of account']],
    ['league', ['agreement'], ['envoys', 'panel by lot'], ['services'], ['reciprocal', 'mutual credit']],
  ];
  const x0 = 122, cw = 28, y0 = 152, rh = 26, H = y0 + rows.length * rh + 14, o = [];
  let col = 0;
  groups.forEach(([name, options, colour], g) => {
    const start = x0 + col * cw;
    o.push(T(start + (options.length * cw) / 2, 16, name, { size: 12.5, weight: 600 }));
    o.push(line(start + 4, 24, start + options.length * cw - 4, 24, { stroke: colour, w: 1.2 }));
    if (g) o.push(line(start, y0 - 6, start, H - 12, { stroke: C.faint, w: 1 }));
    options.forEach((option, k) => {
      const x = start + k * cw + cw / 2;
      o.push(T(x + 4, y0 - 12, option, { ...small, anchor: 'start', rotate: -90 }));
      rows.forEach((row, r) => {
        const y = y0 + r * rh + rh / 2;
        if (row[g + 1].includes(option)) o.push(disc(x, y, 5.2, { fill: colour, stroke: colour }));
        else o.push(disc(x, y, 1.3, { fill: C.soft, stroke: C.soft }));
      });
    });
    col += options.length;
  });
  rows.forEach((row, r) => {
    o.push(T(x0 - 10, y0 + r * rh + rh / 2 + 4, row[0], { size: 12, anchor: 'end' }));
    o.push(line(14, y0 + r * rh, x0 + col * cw, y0 + r * rh, { stroke: C.faint, w: 0.8 }));
  });
  o.push(line(14, y0 + rows.length * rh, x0 + col * cw, y0 + rows.length * rh, { stroke: C.faint, w: 0.8 }));
  return frame(W, H, 'Eight presets as profiles', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 7. Communication within and across communities
function communication() {
  const H = 384, o = [];
  o.push(R(222, 14, 276, 64, { rx: 8, fill: BLUE, stroke: C.blue, dash: DASH }));
  o.push(T(234, 33, 'league', { weight: 600, anchor: 'start' }));
  ['directory', 'notary', 'clearing', 'panel'].forEach((n, k) => o.push(chip(232 + k * 65, 44, 61, n, { h: 22 })));

  const community = (x, title, fill, stroke) => {
    o.push(R(x, 110, 250, 200, { rx: 8, fill, stroke }));
    o.push(T(x + 12, 131, title, { weight: 600, anchor: 'start' }));
    ['tasks', 'review', 'assembly'].forEach((n, k) => o.push(chip(x + 12 + k * 76, 144, 70, n)));
    ['talk', 'intake'].forEach((n, k) => o.push(chip(x + 12 + k * 76, 172, 70, n)));
    o.push(T(x + 12, 290, 'chronicle', { ...mute, anchor: 'start' }));
  };
  community(20, 'community A', C.tLilac, C.lilac);
  community(450, 'community B', C.tOrchid, C.orchid);
  for (let k = 0; k < 6; k += 1) {
    o.push(R(96 + k * 24, 278, 16, 16, { rx: 2, fill: k === 5 ? C.orchid : C.white, stroke: k === 5 ? C.orchid : C.line }));
    o.push(R(526 + k * 24, 278, 16, 16, { rx: 2, fill: k === 5 ? C.lilac : C.white, stroke: k === 5 ? C.lilac : C.line }));
  }
  o.push(disc(270, 213, 9, { stroke: C.orchid, sw: 1.4 })); o.push(T(256, 217, 'envoy', { ...small, anchor: 'end' }));
  o.push(disc(450, 213, 9, { stroke: C.orchid, sw: 1.4 })); o.push(T(464, 217, 'envoy', { ...small, anchor: 'start' }));
  o.push(T(360, 142, 'recognition', small));
  o.push(A('M274,150 H446', 'mute', { dash: DASH })); o.push(A('M446,164 H274', 'mute', { dash: DASH }));
  o.push(T(360, 199, 'act', small)); o.push(A('M281,206 H439', 'orchid', { w: 1.4 }));
  o.push(A('M439,222 H281', 'lilac', { w: 1.4 })); o.push(T(360, 237, 'receipt', small));
  o.push(B(315, 252, 90, 30, 'interface'));
  o.push(line(270, 267, 315, 267, { dash: '3 3' })); o.push(line(405, 267, 450, 267, { dash: '3 3' }));
  o.push(B(280, 332, 160, 38, 'compact', { sub: 'commitments, rules, exit' }));
  o.push(A('M145,310 C145,351 220,351 278,351', 'mute')); o.push(T(186, 368, 'ratifies', small));
  o.push(A('M575,310 C575,351 500,351 442,351', 'mute')); o.push(T(534, 368, 'ratifies', small));
  o.push(A('M145,110 C145,62 180,46 220,46', 'blue')); o.push(T(136, 72, 'member', { ...small, anchor: 'end' }));
  o.push(A('M575,110 C575,62 540,46 500,46', 'blue')); o.push(T(584, 72, 'member', { ...small, anchor: 'start' }));
  return frame(W, H, 'Communication within and across communities', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 8. A norm and its four facets
function norms() {
  const H = 308, o = [];
  o.push(T(89, 28, 'routes', mute));
  ['practice relied on', 'expectation seconded', 'condition of entry', 'inherited rule', 'complaint', 'negotiated term'].forEach((n, k) => {
    const y = 40 + k * 34;
    o.push(chip(14, y, 150, n, { h: 24 }));
    o.push(A(`M164,${y + 12} C202,${y + 12} 198,150 234,150`, 'mute', { w: 0.9 }));
  });
  o.push(R(236, 40, 230, 222, { rx: 4 }));
  o.push(T(248, 60, 'entry of the register', { weight: 600, anchor: 'start' }));
  [['statement', 248, 74], ['purpose', 328, 64], ['terms', 398, 56]].forEach(([n, x, w]) => o.push(chip(x, 70, w, n, { h: 20 })));
  const facet = (y, name) => o.push(T(248, y + 15, name, { size: 12.5, anchor: 'start' }));
  facet(108, 'regularity');
  o.push(R(344, 113, 70, 12, { rx: 0, stroke: C.moss })); o.push(R(344, 113, 66.5, 12, { rx: 0, fill: C.moss, stroke: C.moss }));
  o.push(T(421, 123, '19/20', { ...small, anchor: 'start' }));
  facet(146, 'adoption');
  o.push(B(344, 148, 60, 22, 'act', { size: 11.5, rx: 11, fill: C.tRose, stroke: C.rose }));
  facet(184, 'operation');
  o.push(R(344, 189, 70, 12, { rx: 0, stroke: C.lilac })); o.push(R(344, 189, 35, 12, { rx: 0, fill: C.lilac, stroke: C.lilac }));
  o.push(T(421, 199, '10/20', { ...small, anchor: 'start' }));
  facet(222, 'justification');
  o.push(B(344, 224, 60, 22, 'acts', { size: 11.5, rx: 11, fill: C.tRose, stroke: C.rose }));
  o.push(R(236, 284, 22, 10, { rx: 0, fill: C.moss, stroke: C.moss })); o.push(T(264, 293, 'measured from the chronicle', { ...small, anchor: 'start' }));
  o.push(R(470, 282, 22, 14, { rx: 7, fill: C.tRose, stroke: C.rose })); o.push(T(498, 293, 'made by acts', { ...small, anchor: 'start' }));

  o.push(A('M466,140 H488', 'mute'));
  o.push(T(558, 60, 'adopted', small)); o.push(T(654, 60, 'unadopted', small));
  o.push(T(503, 105, 'operative', { ...small, rotate: -90 })); o.push(T(503, 175, 'inoperative', { ...small, rotate: -90 }));
  const cell = (x, y, a, b, fill, stroke) => {
    o.push(R(x, y, 96, 70, { rx: 0, fill, stroke }));
    o.push(T(x + 48, y + (b ? 33 : 40), a, small));
    if (b) o.push(T(x + 48, y + 48, b, small));
  };
  cell(510, 70, 'in force', '', C.tMoss, C.moss);
  cell(606, 70, 'operative,', 'unadopted', C.tRose, C.rose);
  cell(510, 140, 'adopted,', 'inoperative', C.tSand, C.sand);
  cell(606, 140, 'pattern or', 'proposal', C.white, C.line);
  o.push(A('M606,210 V230', 'mute'));
  [['revision', 506, 62], ['retirement', 572, 72], ['transfer', 648, 60]].forEach(([n, x, w]) => o.push(chip(x, 232, w, n)));
  return frame(W, H, 'A norm as an entry with four facets', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 9. Clearing
function clearing() {
  const H = 290, o = [];
  o.push(panel(8, 20, 'a'));
  const tri = (cx, edges, gross) => {
    const cy = 122, P = { A: [cx, cy - 42], B: [cx + 40, cy + 28], C: [cx - 40, cy + 28] };
    const shorten = (a, b) => {
      const dx = b[0] - a[0], dy = b[1] - a[1], d = Math.hypot(dx, dy), ux = dx / d, uy = dy / d;
      return `M${(a[0] + ux * 14).toFixed(1)},${(a[1] + uy * 14).toFixed(1)} L${(b[0] - ux * 15).toFixed(1)},${(b[1] - uy * 15).toFixed(1)}`;
    };
    const label = { AB: [cx + 33, cy - 9], BC: [cx, cy + 46], CA: [cx - 33, cy - 9], AC: [cx - 33, cy - 9] };
    edges.forEach(([from, to, amount]) => {
      o.push(A(shorten(P[from], P[to]), 'ink', { w: 1.2 }));
      o.push(T(label[from + to][0], label[from + to][1], String(amount), { size: 12.5 }));
    });
    Object.entries(P).forEach(([name, [x, y]]) => o.push(node(x, y, name)));
    o.push(T(cx, cy + 74, `gross ${gross}`, mute));
  };
  tri(66, [['A', 'B', 6], ['B', 'C', 5], ['C', 'A', 4]], 15);
  tri(214, [['A', 'B', 2], ['B', 'C', 1]], 3);
  tri(362, [['A', 'B', 1], ['A', 'C', 1]], 2);
  o.push(A('M120,122 H160', 'moss', { w: 1.3 })); o.push(T(140, 114, 'set-off', small));
  o.push(A('M268,122 H308', 'moss', { w: 1.3 })); o.push(T(288, 114, 'novation', small));
  o.push(T(214, 226, 'net obligations 2, −1, −1 throughout', mute));

  o.push(line(424, 14, 424, H - 14, { stroke: C.faint, w: 1 }));
  o.push(panel(434, 20, 'b'));
  o.push(T(588, 22, 'cycle of performance', small));
  const Q = { P: [528, 58], Q: [648, 58], R: [648, 138], S: [528, 138] };
  o.push(A('M542,58 H633', 'ink', { w: 1.2 })); o.push(T(588, 50, 'review', small));
  o.push(A('M648,72 V123', 'ink', { w: 1.2 })); o.push(T(664, 102, 'compute', { ...small, anchor: 'start' }));
  o.push(A('M634,138 H543', 'ink', { w: 1.2 })); o.push(T(588, 156, 'data', small));
  o.push(A('M528,124 V73', 'ink', { w: 1.2 })); o.push(T(512, 102, 'translation', { ...small, anchor: 'end' }));
  Object.entries(Q).forEach(([name, [x, y]]) => o.push(node(x, y, name)));
  o.push(T(588, 198, 'chain opened by a gift', small));
  o.push(B(446, 222, 62, 28, 'donor', { size: 12, fill: C.tMoss, stroke: C.moss }));
  [[556, 'P'], [626, 'Q'], [696, 'R']].forEach(([x, name]) => o.push(node(x, 236, name)));
  o.push(A('M508,236 H541', 'moss', { w: 1.3 })); o.push(A('M568,236 H611', 'ink', { w: 1.2 })); o.push(A('M638,236 H681', 'ink', { w: 1.2 }));
  o.push(T(477, 268, 'gift', mute)); o.push(T(680, 268, 'need met', mute));
  return frame(W, H, 'Clearing: set-off, novation, a cycle and a chain', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 10. The constructed experiment (from the sketch's results)
function experiment() {
  const H = 286, o = [];
  const share = results.circulation['share of needs met'];
  const unequal = results.unequal;
  o.push(panel(8, 20, 'a')); o.push(T(150, 20, 'balanced circuit', { size: 12.5, weight: 600 }));
  const bars = [
    ['swaps', share['cycles of at most 2'].mean, C.lilac], ['cycles ≤ 3', share['cycles of at most 3'].mean, C.lilac],
    ['cycles ≤ 4', share['cycles of at most 4'].mean, C.lilac], ['exact set-off', share['plan, limit 0'].mean, C.orchid],
    ['feasible', share.feasible.mean, C.white], ['2 donors', share['set-off with 2 donors'].mean, C.moss],
    ['4 donors', share['set-off with 4 donors'].mean, C.moss],
  ];
  const bx = 104, bw = 140;
  bars.forEach(([name, value, colour], k) => {
    const y = 40 + k * 23;
    o.push(T(bx - 8, y + 11, name, { ...small, anchor: 'end' }));
    o.push(R(bx, y, bw * value, 14, { rx: 0, fill: colour, stroke: colour === C.white ? C.rose : colour, sw: colour === C.white ? 1.1 : 0.6 }));
    o.push(T(bx + bw * value + 5, y + 11, value.toFixed(2), { size: 11, anchor: 'start' }));
  });
  o.push(line(bx, 204, bx + bw, 204, { stroke: C.line, w: 1 }));
  [0, 0.5, 1].forEach((v) => { o.push(line(bx + bw * v, 204, bx + bw * v, 208, { stroke: C.line, w: 1 })); o.push(T(bx + bw * v, 220, String(v), { size: 11 })); });
  o.push(T(bx + bw / 2, 238, 'share of needs met', mute));

  const series = [['set-off', C.mute, '5 3', 1.3], ['limit 3', C.rose, '', 1.5], ['limit 6', C.sand, '', 1.5], ['provision', C.moss, '', 2]];
  const plot = (letter, lx, title, x0, data) => {
    o.push(panel(lx, 20, letter)); o.push(T(x0 + 80, 20, title, { size: 12.5, weight: 600 }));
    const X = (r) => x0 + (r * 160) / 9, Y = (v) => 204 - v * 160;
    o.push(line(x0, 44, x0, 204, { stroke: C.line, w: 1 })); o.push(line(x0, 204, x0 + 160, 204, { stroke: C.line, w: 1 }));
    [0, 0.5, 1].forEach((v) => { o.push(line(x0 - 4, Y(v), x0, Y(v), { stroke: C.line, w: 1 })); o.push(T(x0 - 7, Y(v) + 4, String(v), { size: 11, anchor: 'end' })); });
    [1, 5, 10].forEach((r) => { o.push(line(X(r - 1), 204, X(r - 1), 208, { stroke: C.line, w: 1 })); o.push(T(X(r - 1), 220, String(r), { size: 11 })); });
    o.push(T(x0 + 80, 238, 'round', mute));
    series.forEach(([name, colour, dash, w]) => {
      const d = data[name].map((v, r) => `${r ? 'L' : 'M'}${X(r).toFixed(1)},${Y(v).toFixed(1)}`).join(' ');
      o.push(`  <path d="${d}" fill="none" stroke="${colour}" stroke-width="${w}"${dash ? ` stroke-dasharray="${dash}"` : ''} stroke-linejoin="round"/>`);
    });
  };
  plot('b', 286, 'weakest third', 322, unequal["share of the weakest third's needs met by round"]);
  plot('c', 518, 'all parties', 552, unequal['share of needs met by round']);
  series.forEach(([name, colour, dash, w], k) => {
    const x = 330 + k * 98;
    o.push(`  <line x1="${x}" y1="264" x2="${x + 24}" y2="264" stroke="${colour}" stroke-width="${w}"${dash ? ` stroke-dasharray="${dash}"` : ''}/>`);
    o.push(T(x + 30, 268, name, { ...small, anchor: 'start' }));
  });
  return frame(W, H, 'The constructed experiment', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 11. Records and returns
function recordsAndReturns() {
  const H = 236, o = [];
  const kept = results.upstream;
  o.push(B(20, 40, 84, 30, 'method')); o.push(T(62, 86, `keeps ${kept.method}`, mute));
  o.push(B(20, 138, 84, 30, 'dataset')); o.push(T(62, 184, `keeps ${kept.dataset}`, mute));
  o.push(B(170, 89, 100, 30, 'translation')); o.push(T(220, 135, `keeps ${kept.translation}`, mute));
  o.push(B(336, 89, 84, 30, 'service')); o.push(T(378, 135, `keeps ${kept.service}`, mute));
  o.push(A('M104,55 C142,55 136,98 168,98', 'ink')); o.push(T(152, 66, '1/5', { ...small, fill: C.moss }));
  o.push(A('M104,153 C142,153 136,110 168,110', 'ink')); o.push(T(152, 150, '1/5', { ...small, fill: C.moss }));
  o.push(A('M270,104 H334', 'ink')); o.push(T(302, 97, '1/2', { ...small, fill: C.moss }));
  o.push(A('M378,42 V87', 'moss', { w: 1.3 })); o.push(T(386, 58, '100', { ...small, anchor: 'start', fill: C.moss }));
  o.push(line(20, 212, 44, 212, { stroke: C.line, w: 1.1 })); o.push(T(50, 216, 'use', { ...small, anchor: 'start' }));
  o.push(T(96, 216, '1/5', { ...small, anchor: 'start', fill: C.moss })); o.push(T(122, 216, 'share passed upstream', { ...small, anchor: 'start' }));

  o.push(line(446, 14, 446, H - 14, { stroke: C.faint, w: 1 }));
  [['chronicle', 'description', 'roles on record', 30], ['model', 'causal account', 'alternatives compared', 98], ['constitution', 'entitlement', 'a return rule', 166]].forEach(([source, name, sub, y]) => {
    o.push(B(462, y + 6, 82, 28, source, { size: 11.5, rx: 14, fill: C.tSand, stroke: C.sand }));
    o.push(A(`M544,${y + 20} H566`, 'sand'));
    o.push(B(568, y, 142, 40, name, { sub }));
  });
  o.push(line(639, 72, 639, 96, { dash: '3 3' })); o.push(line(639, 140, 639, 164, { dash: '3 3' }));
  return frame(W, H, 'Records and returns', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 12. A claim: complements, procedure and forums
function claims() {
  const H = 276, o = [];
  o.push(T(20, 24, 'complements of a route', { ...mute, anchor: 'start' }));
  ['party', 'forum', 'evidence', 'authority', 'capacity'].forEach((n, k) => o.push(chip(20 + k * 92, 32, 84, n, { h: 24 })));
  o.push(line(20, 64, 472, 64, { stroke: C.mute, w: 0.9 })); o.push(T(246, 78, 'all five', mute));
  o.push(A('M474,44 H518', 'moss', { w: 1.3 }));
  o.push(B(520, 28, 110, 32, 'route', { fill: C.tMoss, stroke: C.moss }));

  o.push(T(20, 110, 'default procedure', { ...mute, anchor: 'start' }));
  ['intake', 'admission', 'hearing', 'finding', 'remedy', 'review'].forEach((n, k) => {
    o.push(chip(20 + k * 92, 120, 78, n, { h: 26 }));
    if (k) o.push(A(`M${20 + k * 92 - 14},133 H${20 + k * 92 - 2}`, 'ink'));
  });
  o.push(chip(122, 168, 152, 'provisional protection', { h: 24, dash: DASH }));
  o.push(line(197, 146, 197, 168, { dash: '3 3' }));
  o.push(T(427, 172, 'correction · explanation · apology', { size: 11, fill: C.mute }));
  o.push(T(427, 187, 'restoration · reallocation · suspension', { size: 11, fill: C.mute }));
  o.push(T(427, 202, 'ending · compensation · referral', { size: 11, fill: C.mute }));

  o.push(T(654, 110, 'forums', mute));
  ['community body', 'league panel', 'outside forum'].forEach((n, k) => o.push(chip(596, 120 + k * 58, 116, n, { h: 28 })));
  o.push(A('M654,148 V176', 'mute')); o.push(T(662, 166, 'referral', { size: 11, anchor: 'start' }));
  o.push(A('M654,206 V234', 'mute')); o.push(T(662, 224, 'referral', { size: 11, anchor: 'start' }));
  o.push(A('M558,133 H594', 'mute'));
  return frame(W, H, 'A claim: complements, procedure and forums', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
// 13. A bounded contribution, act by act (from the worked record)
function boundedContribution() {
  const trace = results.record.trace;
  const lanes = { mei: 90, 'mei/agent-1': 250, steward: 420, 'reviewer-a': 560 };
  const pick = [
    ['mei', 'mandate', 1, 'mei/agent-1', 0], ['mei/agent-1', 'join', 1, 'steward', 0],
    ['mei/agent-1', 'offer', 1, 'steward', 0.3], ['steward', 'take', 1, 'mei/agent-1', 0],
    ['mei/agent-1', 'deliver', 1, 'steward', 1.5], ['reviewer-a', 'assess', 1, 'steward', 0],
    ['steward', 'accept', 1, 'mei/agent-1', 0], ['mei/agent-1', 'offer', 2, 'steward', 0],
    ['steward', 'take', 2, 'mei/agent-1', 0], ['mei/agent-1', 'deliver', 2, 'steward', 0],
    ['mei/agent-1', 'handover', 1, 'steward', 0.15], ['steward', 'release', 1, 'mei/agent-1', 0],
    ['mei', 'revoke', 1, 'mei/agent-1', 0], ['mei/agent-1', 'say', 1, 'steward', 0],
  ];
  const step = 23, H = 78 + pick.length * step + 26, o = [];
  [['sponsor', 'mei'], ['operative', 'mei/agent-1'], ['steward', 'steward'], ['reviewer', 'reviewer-a']].forEach(([title, id]) => {
    o.push(B(lanes[id] - 44, 14, 88, 26, title, { size: 12.5 }));
    o.push(line(lanes[id], 40, lanes[id], H - 22, { stroke: C.soft, w: 0.9, dash: '2 4' }));
  });
  o.push(T(664, 24, 'tokens charged,', { size: 11, fill: C.mute }));
  o.push(T(664, 37, 'millions', { size: 11, fill: C.mute }));
  const mx = 632, mw = 68;
  o.push(line(mx + mw, 46, mx + mw, H - 22, { stroke: C.rose, w: 1, dash: '4 3' }));
  o.push(T(mx + mw, H - 8, 'ceiling 2.00', { size: 11, fill: C.rose, anchor: 'end' }));
  let charged = 0;
  pick.forEach(([author, kind, nth, to, cost], k) => {
    const row = trace.filter((r) => r.author === author && r.kind === kind)[nth - 1];
    const y = 70 + k * step, a = lanes[author], b = lanes[to], dir = b > a ? 1 : -1;
    const name = kind === 'handover' ? 'hand over' : kind;
    if (row.admitted) {
      o.push(A(`M${a + dir * 3},${y} H${b - dir * 3}`, FORCE_COLOUR[row.force], { w: 1.2 }));
      o.push(T((a + b) / 2, y - 6.5, name, { size: 11 }));
      if (cost) {
        charged += cost;
        o.push(R(mx, y - 6, (mw * charged) / 2, 9, { rx: 0, fill: C.sand, stroke: C.sand, sw: 0.6 }));
        o.push(T(mx - 5, y + 3, charged.toFixed(2), { size: 11, anchor: 'end' }));
      }
    } else {
      o.push(A(`M${a + dir * 3},${y} H${b - dir * 16}`, 'rose', { w: 1.2, dash: '4 3', end: false }));
      const cx = b - dir * 12;
      o.push(line(cx - 4, y - 4, cx + 4, y + 4, { stroke: C.rose, w: 1.4 })); o.push(line(cx - 4, y + 4, cx + 4, y - 4, { stroke: C.rose, w: 1.4 }));
      o.push(T((a + b) / 2, y - 6.5, `${name}, refused`, { size: 11, fill: C.rose }));
    }
  });
  const ky = H - 8;
  [['commissive', 'orchid'], ['declarative', 'lilac'], ['assertive', 'moss']].forEach(([name, colour], k) => {
    const x = 60 + k * 150;
    o.push(A(`M${x},${ky - 4} H${x + 24}`, colour, { w: 1.2 }));
    o.push(T(x + 30, ky, name, { size: 11, anchor: 'start' }));
  });
  return frame(W, H, 'A bounded contribution, act by act', o.join('\n'));
}
const FORCE_COLOUR = { commissive: 'orchid', declarative: 'lilac', assertive: 'moss', directive: 'sand', expressive: 'mute' };

// ---------------------------------------------------------------------------------------------
// 14. The five places and the node
function implementation() {
  const H = 366, o = [];
  const proposed = { fill: C.tRose, stroke: C.rose };
  o.push(B(14, 20, 386, 42, 'work', { sub: 'node · page · presets · adapters', fill: C.tOrchid, stroke: C.orchid }));
  o.push(B(14, 82, 186, 40, 'drawings', { sub: 'society', ...proposed }));
  o.push(B(214, 82, 186, 40, 'composition', { sub: 'science/communities', ...proposed }));
  o.push(R(14, 142, 386, 84, { rx: 4, fill: C.tLilac, stroke: C.lilac }));
  o.push(T(26, 161, 'domains', { weight: 600, anchor: 'start' }));
  o.push(chip(26, 170, 100, 'communities', proposed));
  [['law', 132, 46], ['economics', 184, 84], ['sociology', 274, 80]].forEach(([n, x, w]) => o.push(chip(x, 170, w, n)));
  [['diplomacy', 26, 84], ['allocation', 116, 84]].forEach(([n, x, w]) => o.push(chip(x, 198, w, n)));
  o.push(R(14, 246, 386, 84, { rx: 4, fill: C.tSand, stroke: C.sand }));
  o.push(T(26, 265, 'foundations', { weight: 600, anchor: 'start' }));
  o.push(chip(26, 274, 76, 'thesmos', proposed));
  [['archeion', 108, 78], ['apodeixis', 192, 82], ['tekmerion', 280, 86]].forEach(([n, x, w]) => o.push(chip(x, 274, w, n)));
  [['kategoria', 26, 82], ['nomos', 114, 60], ['morphe', 180, 66], ['kybernesis', 252, 92], ['calx', 350, 44]].forEach(([n, x, w]) => o.push(chip(x, 302, w, n)));
  o.push(A('M207,246 V228', 'mute')); o.push(A('M107,142 V124', 'mute')); o.push(A('M307,142 V124', 'mute'));
  o.push(A('M107,82 V64', 'mute')); o.push(A('M307,82 V64', 'mute'));
  o.push(R(26, 342, 22, 12, { rx: 2, ...proposed })); o.push(T(54, 352, 'proposed', { ...small, anchor: 'start' }));

  o.push(R(440, 20, 266, 286, { rx: 6 }));
  o.push(T(452, 40, 'node', { weight: 600, anchor: 'start' }));
  o.push(R(452, 52, 242, 58, { rx: 3, fill: C.tSand, stroke: C.sand }));
  o.push(T(464, 70, 'store', { size: 12, weight: 600, anchor: 'start' }));
  [['chronicle', 464, 70], ['holdings', 540, 66], ['views', 612, 50]].forEach(([n, x, w]) => o.push(chip(x, 78, w, n)));
  o.push(B(452, 124, 114, 40, 'kernel', { sub: 'engine', ...proposed }));
  o.push(B(580, 124, 114, 40, 'scenes', { sub: 'rooms, tempo' }));
  o.push(B(452, 178, 242, 40, 'exchange between nodes', { sub: 'act out, receipt back' }));
  o.push(T(573, 240, 'adapters', mute));
  [['page', 'persons', 452], ['tools', 'operatives', 536], ['offices', 'leagues', 620]].forEach(([n, sub, x]) =>
    o.push(B(x, 250, 74, 40, n, { sub })));
  o.push(A('M400,41 H438', 'orchid', { w: 1.3 }));
  o.push(A('M400,102 C422,102 420,144 450,144', 'mute'));
  return frame(W, H, 'The five places and the node', o.join('\n'));
}

// ---------------------------------------------------------------------------------------------
const figures = [
  ['fig-1-levels', levels], ['fig-2-parties', parties], ['fig-3-act-and-fold', actAndFold],
  ['fig-4-commitment', commitment], ['fig-5-constitution', constitution], ['fig-6-presets', presets],
  ['fig-7-communication', communication], ['fig-8-norms', norms], ['fig-9-clearing', clearing],
  ['fig-10-experiment', experiment], ['fig-11-records-and-returns', recordsAndReturns], ['fig-12-claims', claims],
  ['fig-13-implementation', implementation], ['fig-14-bounded-contribution', boundedContribution],
];
const want = process.argv.slice(2).map(Number).filter(Boolean);
figures.forEach(([name, make], index) => {
  if (want.length && !want.includes(index + 1)) return;
  writeFileSync(join(here, `${name}.svg`), make());
  console.log('made', name);
});
