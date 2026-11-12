// Drawings of what kategoria answers: a category as a diagram, a functor between two, a cone, a set-valued functor as
// its elements and how the arrows move them, a derivation path by path, tables.
//
// Every function takes one of kategoria's views (kategoria/script/session.hpp) and returns markup: SVG for diagrams,
// HTML for lists and tables. Nothing here needs a page, so the checks read the markup in Node. What is drawn takes its
// colours from the page: every mark has a class (k-arrow, k-obj, k-name, …; categories.css gives each a default), and
// the hues that tie an object to its image are the styles' ten, told apart in every vision; an object's hue is always
// beside its name, never alone.

import { MAPS } from '../../styles/palettes.js';
import { layoutCategory, textWidth } from './layout.js';
import { htmlEquation, svgStrings } from './strings.js';

export const HUES = MAPS['categorical-apart'].stops;
const hue = (i) => HUES[i % HUES.length];
const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const f1 = (v) => v.toFixed(1);

// The most objects and arrows a diagram is drawn with; beyond it a category is described and not drawn.
export const DRAWN = { objects: 26, arrows: 84 };

const notIdentity = (view) => view.arrows.map((a, i) => i).filter((i) => view.arrows[i].kind !== 'identity');
const generatorsOf = (view) => (view.generators && view.generators.length ? view.generators : notIdentity(view));

// A category as SVG markup.
//   options.marks    { objects: { index: class }, arrows: { index: class } }: what stands out, by a class of its own
//   options.dots     { index: [colours] }: small discs under an object's name (what is sent to it)
//   options.all      draw every arrow that is no identity, not only the generators
//   options.quiet    everything not marked is drawn faint
export function svgCategory(view, options = {}) {
  const marks = { objects: {}, arrows: {}, ...(options.marks || {}) };
  const marked = Object.keys(marks.arrows).map(Number).filter((i) => view.arrows[i] && view.arrows[i].kind !== 'identity');
  const drawn = options.all ? notIdentity(view) : [...new Set([...generatorsOf(view), ...marked])];
  if (view.objects.length > DRAWN.objects || drawn.length > DRAWN.arrows)
    return `<p class="k-large">${view.objects.length} objects and ${view.arrows.length} arrows: too many to draw as a diagram. Its facts are beside it, and its arrows can be asked for one by one.</p>`;
  const L = layoutCategory(view, { arrows: drawn });
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" class="k-category${options.quiet ? ' quiet' : ''}" viewBox="0 0 ${L.width.toFixed(0)} ${L.height.toFixed(0)}" width="${L.width.toFixed(0)}" height="${L.height.toFixed(0)}" role="img" aria-label="${esc(view.name || 'a category')}">`];
  L.edges.forEach((e) => {
    if (!e.d) return;
    const cls = `${view.arrows[e.arrow].kind}${marks.arrows[e.arrow] ? ` ${marks.arrows[e.arrow]}` : ''}`;
    const a = e.head.angle, back = 7.5, open = 0.42;
    const head = `M${f1(e.head.x - back * Math.cos(a - open))} ${f1(e.head.y - back * Math.sin(a - open))} L${f1(e.head.x)} ${f1(e.head.y)} L${f1(e.head.x - back * Math.cos(a + open))} ${f1(e.head.y - back * Math.sin(a + open))}`;
    parts.push(`<g class="k-arrow ${cls}" data-arrow="${e.arrow}"><path class="k-shaft" d="${e.d}"/><path class="k-head" d="${head}"/>` +
               `<text class="k-name" x="${f1(e.label.x)}" y="${f1(e.label.y + 4)}" text-anchor="middle">${esc(e.name)}</text></g>`);
  });
  L.nodes.forEach((n, i) => {
    const cls = marks.objects[i] ? ` ${marks.objects[i]}` : '';
    let dots = '';
    (options.dots && options.dots[i] ? options.dots[i] : []).forEach((colour, k, all) => {
      dots += `<circle class="k-dot" cx="${f1(n.x + (k - (all.length - 1) / 2) * 9)}" cy="${f1(n.y + n.ry + 4)}" r="3.4" fill="${colour}"/>`;
    });
    parts.push(`<g class="k-obj${cls}" data-object="${i}"><ellipse cx="${f1(n.x)}" cy="${f1(n.y)}" rx="${f1(n.rx)}" ry="${f1(n.ry)}"/>` +
               `<text x="${f1(n.x)}" y="${f1(n.y + 5)}" text-anchor="middle">${esc(n.label)}</text>${dots}</g>`);
  });
  parts.push('</svg>');
  return parts.join('');
}

const facts = (view) => (view.facts && view.facts.length ? `<ul class="k-facts">${view.facts.map((f) => `<li>${esc(f)}</li>`).join('')}</ul>` : '');
const equations = (view) => (view.equations && view.equations.length ? `<p class="k-equations">${view.equations.map((e) => `<span>${esc(e)}</span>`).join('')}</p>` : '');

export function htmlCategory(view, options = {}) {
  return `<div class="k-figure">${svgCategory(view, options)}</div>${equations(view)}${facts(view)}`;
}

// The images of a category's generators under a functor, in words: "f ↦ v ∘ u".
const images = (source, target, functor, most = 14) => {
  const gens = generatorsOf(source);
  const lines = gens.slice(0, most).map((g) => `<span>${esc(source.arrows[g].name)} ↦ ${esc(target.arrows[functor.arrows[g]].name)}</span>`);
  if (gens.length > most) lines.push(`<span>and ${gens.length - most} more</span>`);
  return lines.join('');
};

// Two categories with what a functor does between them: each object of the source has a hue, shown again under the
// object it is sent to.
const pair = (source, target, objects, label) => {
  const own = {}, sent = {};
  source.objects.forEach((_, i) => { own[i] = [hue(i)]; (sent[objects[i]] = sent[objects[i]] || []).push(hue(i)); });
  return `<div class="k-pair"><div class="k-side"><h5>${esc(source.name)}</h5>${svgCategory(source, { dots: own })}</div>` +
         `<div class="k-between"><span>${esc(label)}</span><span class="k-long-arrow">⟶</span></div>` +
         `<div class="k-side"><h5>${esc(target.name)}</h5>${svgCategory(target, { dots: sent })}</div></div>`;
};
const objectMap = (source, target, objects) => source.objects.map((name, i) => `<span><i class="k-swatch" style="background:${hue(i)}"></i>${esc(name)} ↦ ${esc(target.objects[objects[i]])}</span>`).join('');

export function htmlFunctor(view) {
  return `${pair(view.source, view.target, view.objects, view.name)}<p class="k-maps">${objectMap(view.source, view.target, view.objects)}</p><p class="k-maps arrows">${images(view.source, view.target, view)}</p>`;
}

export function htmlTransformation(view) {
  const marks = { arrows: {}, objects: {} };
  view.components.forEach((c) => { marks.arrows[c] = 'leg'; });
  const lines = view.source.objects.map((name, i) => {
    const c = view.target.arrows[view.components[i]];
    return `<span>at ${esc(name)}: ${esc(c.name)} : ${esc(view.target.objects[c.dom])} → ${esc(view.target.objects[c.cod])}</span>`;
  });
  return `<div class="k-figure">${svgCategory(view.target, { marks, quiet: true })}</div><p class="k-maps arrows">${lines.join('')}</p>` +
         `<p class="k-note">The components, marked in ${esc(view.target.name)}, run from the images under ${esc(view.from.name)} to those under ${esc(view.to.name)}.</p>`;
}

export function htmlAdjunction(view) {
  const unit = view.source.objects.map((name, i) => `<span>η at ${esc(name)}: ${esc(view.source.arrows[view.unit[i]].name)}</span>`).join('');
  const counit = view.target.objects.map((name, i) => `<span>ε at ${esc(name)}: ${esc(view.target.arrows[view.counit[i]].name)}</span>`).join('');
  return `${pair(view.source, view.target, view.left.objects, `${view.left.name} ⊣ ${view.right.name}`)}` +
         `<p class="k-maps"><b>${esc(view.left.name)}</b>${objectMap(view.source, view.target, view.left.objects)}</p>` +
         `<p class="k-maps plain"><b>${esc(view.right.name)}</b>${view.target.objects.map((name, i) => `<span>${esc(name)} ↦ ${esc(view.source.objects[view.right.objects[i]])}</span>`).join('')}</p>` +
         `<p class="k-maps arrows">${unit}</p><p class="k-maps arrows">${counit}</p>`;
}

// A cone on a diagram: the diagram's objects and arrows stand out, the apex and its legs in the accent.
export function htmlCone(view) {
  const marks = { objects: {}, arrows: {} };
  view.shape.objects.forEach((o) => { marks.objects[o] = 'shape'; });
  view.shape.arrows.forEach((a) => { marks.arrows[a] = 'shape'; });
  view.legs.forEach((a) => { marks.arrows[a] = 'leg'; });
  marks.objects[view.apex] = 'apex';
  const legs = view.legs.map((a) => `<span>${esc(view.category.arrows[a].name)}</span>`).join('');
  return `<div class="k-figure">${svgCategory(view.category, { marks, quiet: true })}</div>` +
         `<p class="k-maps arrows"><b>${view.colimit ? 'the colimit' : 'the limit'}: ${esc(view.category.objects[view.apex])}</b>${legs}</p>`;
}

// A set-valued functor: each object a slip holding its elements; each generator a hue, a line from every element to
// where the generator sends it.
export function svgSets(view, options = {}) {
  const cat = view.category, most = options.most || 12;
  if (cat.objects.length > DRAWN.objects) return `<p class="k-large">${cat.objects.length} objects: too many to draw.</p>`;
  const L = layoutCategory(cat);
  const shown = view.elements.map((names) => (names.length > most ? names.slice(0, most - 1) : names));
  const box = view.elements.map((names, a) => {
    const lines = shown[a].length + (names.length > shown[a].length ? 1 : 0);
    const w = Math.max(54, textWidth(cat.objects[a], 14) + 22, ...shown[a].map((n) => textWidth(n, 13) + 34));
    return { w, h: 30 + Math.max(1, lines) * 19 + 6, x: 0, y: 0 };
  });
  let x = 16;
  const tallest = Math.max(1, ...L.rows.map((row) => row.reduce((s, v) => s + box[v].h + 26, -26)));
  L.rows.forEach((row) => {
    const width = Math.max(0, ...row.map((v) => box[v].w));
    let y = 16 + (tallest - row.reduce((s, v) => s + box[v].h + 26, -26)) / 2;
    row.forEach((v) => { box[v].x = x + (width - box[v].w) / 2; box[v].y = y; y += box[v].h + 26; });
    x += width + 96;
  });
  const width = x - 96 + 16 + 70, height = tallest + 32;
  const at = (a, k) => ({ x: box[a].x, y: box[a].y + 30 + k * 19 + 9, w: box[a].w });
  const parts = [`<svg xmlns="http://www.w3.org/2000/svg" class="k-sets" viewBox="0 0 ${width.toFixed(0)} ${height.toFixed(0)}" width="${width.toFixed(0)}" height="${height.toFixed(0)}" role="img" aria-label="${esc(view.name || 'a set-valued functor')}">`];
  view.actions.forEach((action, g) => {
    const arrow = cat.arrows[action.arrow];
    action.map.forEach((to, from) => {
      if (from >= shown[arrow.dom].length || to >= shown[arrow.cod].length) return;
      const p = at(arrow.dom, from), q = at(arrow.cod, to);
      // Forward, from the right edge to the left edge of a slip further right; backward, from the left edge to the right edge
      // of one further left; within a column (a slip to itself among them), out to the right and back.
      const forward = box[arrow.cod].x > box[arrow.dom].x + box[arrow.dom].w - 1, backward = box[arrow.cod].x + box[arrow.cod].w < box[arrow.dom].x + 1;
      const sx = backward ? p.x : p.x + p.w, tx = forward ? q.x : q.x + q.w;
      const bend = forward || backward ? Math.max(30, Math.abs(tx - sx) * 0.45) : 30 + g * 8 + Math.abs(q.y - p.y) * 0.25;
      const d = forward ? `M${f1(sx)} ${f1(p.y)} C${f1(sx + bend)} ${f1(p.y)} ${f1(tx - bend)} ${f1(q.y)} ${f1(tx)} ${f1(q.y)}`
              : backward ? `M${f1(sx)} ${f1(p.y)} C${f1(sx - bend)} ${f1(p.y)} ${f1(tx + bend)} ${f1(q.y)} ${f1(tx)} ${f1(q.y)}`
                         : `M${f1(sx)} ${f1(p.y)} C${f1(sx + bend)} ${f1(p.y)} ${f1(tx + bend)} ${f1(q.y)} ${f1(tx)} ${f1(q.y)}`;
      parts.push(`<path class="k-act" d="${d}" stroke="${hue(g)}" data-arrow="${action.arrow}"/><circle class="k-act-end" cx="${f1(tx)}" cy="${f1(q.y)}" r="2.4" fill="${hue(g)}"/>`);
    });
  });
  view.elements.forEach((names, a) => {
    const b = box[a];
    parts.push(`<g class="k-set" data-object="${a}"><rect x="${f1(b.x)}" y="${f1(b.y)}" width="${f1(b.w)}" height="${f1(b.h)}" rx="3"/>` +
               `<text class="k-set-name" x="${f1(b.x + b.w / 2)}" y="${f1(b.y + 19)}" text-anchor="middle">${esc(cat.objects[a])}</text>` +
               `<line x1="${f1(b.x + 6)}" x2="${f1(b.x + b.w - 6)}" y1="${f1(b.y + 26)}" y2="${f1(b.y + 26)}"/>`);
    shown[a].forEach((n, k) => parts.push(`<text class="k-element" x="${f1(b.x + 12)}" y="${f1(b.y + 30 + k * 19 + 13)}">${esc(n)}</text>`));
    if (!names.length) parts.push(`<text class="k-element empty" x="${f1(b.x + b.w / 2)}" y="${f1(b.y + 43)}" text-anchor="middle">∅</text>`);
    if (names.length > shown[a].length) parts.push(`<text class="k-element empty" x="${f1(b.x + 12)}" y="${f1(b.y + 30 + shown[a].length * 19 + 13)}">and ${names.length - shown[a].length} more</text>`);
    parts.push('</g>');
  });
  parts.push('</svg>');
  return parts.join('');
}

export function htmlSets(view) {
  const cat = view.category;
  const legend = view.actions.map((action, g) => {
    const a = cat.arrows[action.arrow];
    return `<span><i class="k-swatch line" style="background:${hue(g)}"></i>${esc(a.name)} : ${esc(cat.objects[a.dom])} → ${esc(cat.objects[a.cod])}</span>`;
  }).join('');
  const sizes = view.elements.map((names, a) => `<span>${esc(cat.objects[a])}: ${names.length}</span>`).join('');
  return `<div class="k-figure">${svgSets(view)}</div><p class="k-maps">${legend}</p><p class="k-maps plain"><b>elements</b>${sizes}</p>`;
}

// A derivation: the paths it passes through, each with the part about to be replaced marked and the equation that
// replaces it. A path is written as a composite, the arrow travelled first at the right.
export function htmlDerivation(view) {
  const rows = view.steps.map((step, k) => {
    const n = step.edges.length;
    let path = '';
    if (!n) path = `<span class="k-edge">id_${esc(step.at_object)}</span>`;
    for (let i = n - 1; i >= 0; i--) {
      const hit = step.at !== undefined && i >= step.at && i < step.at + step.span;
      path += `<span class="k-edge${hit ? ' hit' : ''}">${esc(step.edges[i])}</span>${i > 0 ? `<span class="k-after${hit && i > step.at ? ' hit' : ''}">∘</span>` : ''}`;
      if (step.at !== undefined && step.span === 0 && i === step.at) path += '<span class="k-caret" title="an identity stands here">·</span>';
    }
    if (n && step.at !== undefined && step.span === 0 && step.at === n) path = `<span class="k-caret" title="an identity stands here">·</span>${path}`;
    const by = step.by !== undefined ? `<span class="k-by">by ${esc(step.by)}${step.forward ? '' : ', read from right to left'}</span>` : '<span class="k-by end">which was to be reached</span>';
    return `<li><span class="k-step">${k === 0 ? '' : '='}</span><span class="k-path">${path}</span>${by}</li>`;
  });
  return `<ol class="k-derivation">${rows.join('')}</ol>`;
}

export function htmlTable(view) {
  const head = view.columns.map((c) => `<th>${esc(c)}</th>`).join('');
  const rows = view.rows.map((r) => `<tr>${r.map((c) => `<td>${esc(c)}</td>`).join('')}</tr>`).join('');
  return view.rows.length ? `<div class="k-scroll"><table class="k-table"><thead><tr>${head}</tr></thead><tbody>${rows}</tbody></table></div>` : '<p class="k-note">None.</p>';
}

// The table of composition: row g, column f, the entry g ∘ f.
export function htmlComposition(view) {
  if (!view.table) return '<p class="k-note">The table is shown for categories of at most 48 arrows.</p>';
  const name = (i) => esc(view.arrows[i].name);
  const head = view.arrows.map((_, f) => `<th>${name(f)}</th>`).join('');
  const rows = view.table.map((row, g) => `<tr><th>${name(g)}</th>${row.map((h) => (h < 0 ? '<td class="none">·</td>' : `<td${view.arrows[h].kind === 'identity' ? ' class="one"' : ''}>${name(h)}</td>`)).join('')}</tr>`).join('');
  return `<div class="k-scroll"><table class="k-table composition"><thead><tr><th class="corner">g ∘ f</th>${head}</tr></thead><tbody>${rows}</tbody></table></div>`;
}

export function htmlTheory(view) {
  const boxes = view.generators.map((g) => `<div class="k-generator"><div>${svgStrings(g.diagram)}</div><span>${esc(g.name)} : ${esc(g.in)} → ${esc(g.out)}</span></div>`).join('');
  const laws = view.laws.map((law) => `<div class="k-law"><h5>${esc(law.name)}</h5>${htmlEquation(law.left, law.right)}</div>`).join('');
  return `<div class="k-generators">${boxes}</div>${laws}`;
}

const GLYPH = { holds: '✓', fails: '✗', undecided: '?', error: '!' };

export function htmlLaws(view) {
  return view.laws.map((law) => `<div class="k-law ${esc(law.status)}"><h5><span class="k-glyph">${GLYPH[law.status] || ''}</span>${esc(law.name)}<em>${esc(law.status)}</em></h5>` +
    `${htmlEquation(law.left, law.right, law.status === 'fails' ? '≠' : '=')}${law.status === 'fails' ? `<p class="k-note">${esc(law.reason)}</p>` : ''}</div>`).join('');
}

// Whatever a view is, drawn.
export function render(view) {
  if (!view || !view.type) return '';
  switch (view.type) {
    case 'category': return htmlCategory(view, view.marks ? { marks: { objects: Object.fromEntries((view.marks.objects || []).map((o) => [o, 'apex'])), arrows: Object.fromEntries((view.marks.arrows || []).map((a) => [a, 'leg'])) }, quiet: true } : {});
    case 'composition': return htmlComposition(view);
    case 'functor': return htmlFunctor(view);
    case 'transformation': return htmlTransformation(view);
    case 'adjunction': return htmlAdjunction(view);
    case 'cone': return htmlCone(view);
    case 'sets': return htmlSets(view);
    case 'derivation': return htmlDerivation(view);
    case 'string': return htmlEquation(view.left, view.right);
    case 'theory': return htmlTheory(view);
    case 'laws': return htmlLaws(view);
    case 'table': return htmlTable(view);
    default: return '';
  }
}

export { GLYPH };
