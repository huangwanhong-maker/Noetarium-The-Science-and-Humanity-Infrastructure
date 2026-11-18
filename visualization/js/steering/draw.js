// The computational graph of an architecture drawn (kybernesis, ADR 0130), as SVG markup that needs no page.
//
// svgGraph(architecture, report, options) takes an architecture as parseArchitecture reads it (or the script itself) and,
// optionally, the foundation's report of its checks and of the methods each block admits, and returns one SVG. Every mark
// carries its look in its attributes, so that the drawing stands alone for publication on a white ground (options.ground for
// another), and a class (kb-…) by which a page's stylesheet may change it.
//
// What a mark says is never told by its colour alone: a boundary has a dashed outline and stands in the key; a block whose laws
// say it is referred to and not established is outlined in dashes of moss; a wire carried to the next turn is dashed, returns
// leftwards beneath the columns and is marked "next"; a method's family is a glyph and its verdict a mark and a word.

import { parseArchitecture } from './parse.js';
import { FONT, PALETTE, layoutGraph } from './layout.js';

const esc = (s) => String(s ?? '').replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const f1 = (v) => (Math.round(v * 10) / 10).toString();

function head(h, fill) {
  const { x, y, dir } = h, a = 5.5, b = 3.1;
  return dir === 'up'
    ? `<path d="M${f1(x - b)} ${f1(y + a)} L${f1(x)} ${f1(y)} L${f1(x + b)} ${f1(y + a)} Z" fill="${fill}"/>`
    : `<path d="M${f1(x - a)} ${f1(y - b)} L${f1(x)} ${f1(y)} L${f1(x - a)} ${f1(y + b)} Z" fill="${fill}"/>`;
}

function textMark(t) {
  const attrs = [`x="${f1(t.x)}"`, `y="${f1(t.y)}"`, `font-size="${t.size}"`, `fill="${t.fill}"`];
  if (t.anchor) attrs.push(`text-anchor="${t.anchor}"`);
  if (t.bold) attrs.push('font-weight="600"');
  if (t.italic) attrs.push('font-style="italic"');
  if (t.halo) attrs.push(`stroke="${t.halo}" stroke-width="3" stroke-linejoin="round" paint-order="stroke"`);
  return `<text class="${t.cls || ''}" ${attrs.join(' ')}>${esc(t.text)}</text>`;
}

// options: { ground: '#FFFFFF', title: true, sizes: { name, small, method, wire, stage, register, key, footer } }
export function svgGraph(architecture, report = null, options = {}) {
  const A = typeof architecture === 'string' ? parseArchitecture(architecture) : architecture;
  const L = layoutGraph(A, report, options);
  const P = PALETTE;
  const out = [];
  const label = A.name ? `the computational graph of architecture ${A.name}, version ${A.version}` : 'a computational graph';
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="kb-graph" viewBox="0 0 ${Math.ceil(L.width)} ${Math.ceil(L.height)}" width="${Math.ceil(L.width)}" height="${Math.ceil(L.height)}" font-family="${esc(FONT)}" role="img" aria-label="${esc(label)}">`);
  out.push(`<rect class="kb-ground" x="0" y="0" width="${Math.ceil(L.width)}" height="${Math.ceil(L.height)}" fill="${L.ground}"/>`);

  // The stages as columns, tinted by their registers.
  for (const c of L.columns) {
    out.push(`<g class="kb-stage-column" data-stage="${esc(c.stage)}" data-register="${esc(c.register || '')}">`);
    out.push(`<rect x="${f1(c.left)}" y="${f1(c.top)}" width="${f1(c.right - c.left)}" height="${f1(c.bottom - c.top)}" rx="2" fill="${c.tint}"/>`);
    out.push(`<line x1="${f1(c.left + 10)}" y1="${f1(c.top + 47)}" x2="${f1(c.right - 10)}" y2="${f1(c.top + 47)}" stroke="${c.ink}" stroke-opacity="0.35" stroke-width="0.6"/>`);
    out.push('</g>');
  }

  // The wires beneath the boxes.
  for (const w of L.wires) {
    const cls = `kb-wire kb-${w.kind}`;
    const colour = w.kind === 'forward' ? P.ink : w.kind === 'next' ? P.lilac : P.rose;
    const dash = w.kind === 'next' ? ' stroke-dasharray="4.5 3"' : w.kind === 'backward' ? ' stroke-dasharray="1.6 2.4"' : '';
    out.push(`<g class="${cls}" data-wire="${esc(`${w.from}.${w.out} -> ${w.to}.${w.in}`)}">` +
      `<path d="${w.d}" fill="none" stroke="${colour}" stroke-width="${w.kind === 'forward' ? 0.95 : 1.1}" stroke-opacity="${w.kind === 'forward' ? 0.82 : 1}"${dash} stroke-linecap="round" stroke-linejoin="round"/>` +
      `${head(w.head, colour)}</g>`);
  }

  // The blocks.
  for (const b of L.boxes) {
    const cls = `kb-block${b.boundary ? ' kb-boundary' : ''}${b.referred ? ' kb-referred' : ''}`;
    const stroke = b.referred ? P.moss : b.boundary ? P.muted : P.ink;
    const dash = b.referred ? ' stroke-dasharray="5 3"' : b.boundary ? ' stroke-dasharray="2.5 2.2"' : '';
    const fill = b.referred ? '#F6F9F5' : L.ground;
    out.push(`<g class="${cls}" data-block="${esc(b.name)}">`);
    out.push(`<rect x="${f1(b.x)}" y="${f1(b.y)}" width="${f1(b.w)}" height="${f1(b.h)}" rx="2.5" fill="${fill}" stroke="${stroke}" stroke-width="${b.referred ? 1.15 : 0.9}"${dash}/>`);
    if (b.methods.length) out.push(`<line class="kb-methods-rule" x1="${f1(b.x + 6)}" y1="${f1(b.y + b.body)}" x2="${f1(b.x + b.w - 6)}" y2="${f1(b.y + b.body)}" stroke="${P.line}" stroke-width="0.8"/>`);
    out.push('</g>');
  }
  for (const d of L.dots) {
    out.push(d.filled
      ? `<circle class="kb-port" cx="${f1(d.x)}" cy="${f1(d.y)}" r="1.9" fill="${P.ink}"/>`
      : `<circle class="kb-port kb-unwired" cx="${f1(d.x)}" cy="${f1(d.y)}" r="2.3" fill="${L.ground}" stroke="${P.rose}" stroke-width="0.9"/>`);
  }

  // The key's samples.
  for (const k of L.key) {
    const { x, y } = k;
    if (k.kind === 'wire') out.push(`<g class="kb-key kb-key-wire"><path d="M${f1(x)} ${f1(y)} H${f1(x + 22)}" stroke="${P.ink}" stroke-width="0.95"/>${head({ x: x + 27, y, dir: 'right' }, P.ink)}</g>`);
    else if (k.kind === 'next') out.push(`<g class="kb-key kb-key-next"><path d="M${f1(x)} ${f1(y)} H${f1(x + 22)}" stroke="${P.lilac}" stroke-width="1.1" stroke-dasharray="4.5 3"/>${head({ x: x + 27, y, dir: 'right' }, P.lilac)}</g>`);
    else if (k.kind === 'boundary') out.push(`<rect class="kb-key kb-key-boundary" x="${f1(x)}" y="${f1(y - 6)}" width="20" height="12" rx="2" fill="${L.ground}" stroke="${P.muted}" stroke-width="0.9" stroke-dasharray="2.5 2.2"/>`);
    else if (k.kind === 'referred') out.push(`<rect class="kb-key kb-key-referred" x="${f1(x)}" y="${f1(y - 6)}" width="20" height="12" rx="2" fill="#F6F9F5" stroke="${P.moss}" stroke-width="1.15" stroke-dasharray="5 3"/>`);
    else if (k.kind === 'register') out.push(`<rect class="kb-key kb-key-register" x="${f1(x)}" y="${f1(y - 6)}" width="12" height="12" rx="1.5" fill="${k.tint}" stroke="${k.ink}" stroke-opacity="0.6" stroke-width="0.7"/>`);
  }

  // Every text last, above the wires.
  for (const t of L.texts) out.push(textMark(t));
  out.push('</svg>');
  return out.join('\n');
}
