// A schema stated as categories, verified and drawn.
//
// Reads a script of kategoria (a domain's schema), asks the Category Studio's engine for its answers, holds each to what the
// script expects of it ("fails" where the line above a statement says "# expect fails", "holds" everywhere else), and saves
// the categories and set-valued functors the script names for drawing as diagrams (SVG, standing alone) in a folder, with a
// report of the verdicts beside them. A line "# title Name: words" in the script names the drawing of the statement so called.
//
// It is the general form of applications/histos-harmonia/tools/draw-schema.mjs, which draws the schema of historical objects.
//
// Run from the repository root:
//   node applications/category-studio/tools/draw-schema.mjs domains/law/schema/facts.cat domains/law/schema/figures

import { spawnSync } from 'node:child_process';
import { mkdirSync, readFileSync, writeFileSync } from 'node:fs';
import { basename, dirname, relative, resolve } from 'node:path';
import { fileURLToPath } from 'node:url';

import { svgCategory, svgSets } from '../../../visualization/js/categories/index.js';

const root = resolve(dirname(fileURLToPath(import.meta.url)), '../../..');
const [scriptArg, outArg] = process.argv.slice(2);
if (!scriptArg || !outArg) {
  console.error('usage: node applications/category-studio/tools/draw-schema.mjs <script.cat> <folder for the figures>');
  process.exit(2);
}
const scriptPath = resolve(root, scriptArg);
const out = resolve(root, outArg);
const script = readFileSync(scriptPath, 'utf8');

const asked = spawnSync('python', [resolve(root, 'applications/category-studio/engine.py'), 'ask', '-'],
  { input: JSON.stringify({ op: 'run', script }), encoding: 'utf8', maxBuffer: 1 << 28 });
if (asked.status !== 0) { console.error(asked.stderr || 'the engine answered nothing'); process.exit(1); }
const outcomes = JSON.parse(asked.stdout).outcomes;

const lines = script.split('\n');
const expected = (o) => (/^#\s*expect fails\s*$/.test(lines[o.line - 2] || '') ? 'fails' : 'holds');
const titles = {};
for (const line of lines) {
  const m = line.match(/^#\s*title\s+([A-Za-z_][A-Za-z0-9_']*)\s*:\s*(.+?)\s*$/);
  if (m) titles[m[1]] = m[2];
}
const heading = (lines.find((l) => /^#\s*\S/.test(l)) || '# A schema').replace(/^#\s*/, '');

let wrong = 0;
const scriptLink = relative(out, scriptPath).replace(/\\/g, '/');
const report = [`# ${heading}, as kategoria answers it`, '',
  `Made by \`applications/category-studio/tools/draw-schema.mjs\` from [${basename(scriptPath)}](${scriptLink}).`, '',
  '| Line | Verdict | Claim | Reason |', '|---|---|---|---|'];
for (const o of outcomes) {
  const ok = o.status === expected(o);
  if (!ok) wrong += 1;
  report.push(`| ${o.line} | ${o.status}${ok ? '' : ' (not as expected)'} | ${(o.claim || o.text.split('\n')[0]).replace(/\|/g, '\\|')} | ${(o.reason || '').replace(/\|/g, '\\|')} |`);
}

const css = readFileSync(resolve(root, 'visualization/js/categories/categories.css'), 'utf8');
const ground = '#fbf8fc', ink = '#3f2d44';
// A drawing made to stand alone: the layer's stylesheet inside it, a ground, a title and a line of what was found.
function standing(svg, title, note) {
  const m = svg.match(/viewBox="0 0 (\d+) (\d+)"/);
  if (!m) return null;
  const w = Math.max(Number(m[1]) + 48, 520);
  const inner = svg.replace(/^<svg[^>]*>/, '').replace(/<\/svg>$/, '');
  const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;');
  const most = Math.floor((w - 48) / 6.4), rows = [''];
  for (const word of note.split(' ')) {
    if ((rows[rows.length - 1] + ' ' + word).length > most) rows.push(word);
    else rows[rows.length - 1] += (rows[rows.length - 1] ? ' ' : '') + word;
  }
  const top = 52 + rows.length * 17, h = Number(m[2]) + top + 20;
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" width="${w}" height="${h}" role="img" aria-label="${esc(title)}">` +
    `<style>${css}
.title { font: 600 17px 'Source Serif 4', Georgia, serif; fill: ${ink}; } .note { font: italic 12.5px 'Source Serif 4', Georgia, serif; fill: #7d6a86; }</style>` +
    `<rect width="${w}" height="${h}" fill="${ground}"/><text class="title" x="24" y="32">${esc(title)}</text>` +
    rows.map((row, i) => `<text class="note" x="24" y="${52 + i * 17}">${esc(row)}</text>`).join('') +
    `<g class="k-category k-sets" transform="translate(24 ${top})" font-family="'Source Serif 4', Georgia, serif">${inner}</g></svg>`;
}

mkdirSync(out, { recursive: true });
const saved = [];
for (const o of outcomes) {
  const v = o.view;
  if (!v || !titles[o.name] || o.status !== 'holds') continue;
  let svg = null;
  if (v.type === 'category') svg = svgCategory(v, { all: v.objects.length <= 4 });
  else if (v.type === 'sets') svg = svgSets(v, { most: 6 });
  const whole = svg && svg.startsWith('<svg') ? standing(svg, titles[o.name], `${o.claim}: ${o.reason}`) : null;
  if (!whole) continue;
  const file = `${o.name.toLowerCase()}.svg`;
  if (saved.includes(file)) continue;
  writeFileSync(resolve(out, file), whole, 'utf8');
  saved.push(file);
}
report.push('', '## Diagrams', '', ...saved.map((f) => `- [${titles[Object.keys(titles).find((k) => `${k.toLowerCase()}.svg` === f)]}](${f})`), '');
writeFileSync(resolve(out, 'README.md'), report.join('\n'), 'utf8');
console.log(`${outcomes.length} statements, ${wrong} not as expected; ${saved.length} diagrams saved in ${out}`);
process.exit(wrong ? 1 : 0);
