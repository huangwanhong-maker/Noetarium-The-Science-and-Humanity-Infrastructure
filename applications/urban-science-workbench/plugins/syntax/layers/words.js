// Words of triangles: pick a place, and its blocks (the faces of its street graph) or its buildings' footprints are triangulated and
// read as a text whose letters are triangles' shapes (syntaxis's 2-simplices as words; urban-science's and architecture's syntax).
// Each triangle is coloured by its letter; the gluing grammar learned from them gives the letters' entropy and how much the letter
// glued to an edge is constrained by its neighbour's; a patch is grown from it beside the place.

import { footprintsNear, letterColour, linear, palette, waysNear, whenNear } from '../shared.js';

const SOURCES = [['blocks', 'blocks'], ['buildings', 'buildings']];

export async function layer(ctx) {
  const { gis, frame, earth, world, dom: { h } } = ctx;
  const id = ctx.layerId(':words'), grownId = ctx.layerId(':grown');
  const P = palette(ctx.theme);
  let source = 'blocks', centre = null, dead = false, token = 0, lastGrown = null;
  // Short at first, so that the shell keeps it in the card (it folds long prose away); it then says what is happening.
  const status = h('div', { class: 'ld-quiet' }, 'Pick a place: its blocks become words of triangles.');
  const readout = h('div', { class: 'gs-readout' });
  const alphabet = h('div', { class: 'gs-alphabet' });
  const keys = h('div', { class: 'keys-tight' }, SOURCES.map(([v, name]) => h('button', { class: v === source ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    source = v;
    compute();
  } }, name)));

  function drawComplex(drawId, c, colourOf, offset = [0, 0], scale = 1, opacity = 0.9) {
    const features = c.triangles.map((t, k) => ({ k, ring: t.map((i) => [c.points[i][0] * scale + offset[0], c.points[i][1] * scale + offset[1]]) }));
    gis.addFlats(drawId, features, { ring: (f) => f.ring, color: (f) => linear(colourOf(f.k)), z: 0.5, opacity, order: 2 });
  }

  async function compute() {
    if (!centre) return;
    const me = ++token;
    const request = { degrees: 15, seed: 1 + Math.floor(Math.random() * 1e6), grow: 260 };
    const alive = () => me === token && !dead;
    if (source === 'buildings') {
      status.textContent = 'gathering the buildings here…';
      const rings = await whenNear(() => footprintsNear(gis, centre, 350), alive);
      if (!alive()) return;
      if (!rings) { status.textContent = 'No buildings drawn here: come down closer, or pick another place.'; return; }
      request.rings = rings;
      status.textContent = `reading ${rings.length.toLocaleString('en')} footprints as triangles…`;
    } else {
      status.textContent = 'gathering the streets here…';
      const ways = await whenNear(() => waysNear(world, centre, 500), alive);
      if (!alive()) return;
      if (!ways) { status.textContent = 'No ways here: pick another place.'; return; }
      request.ways = ways;
      request.maxArea = 60000;
      status.textContent = 'reading the blocks as triangles…';
    }
    let a;
    try { a = await ctx.ask('words', request); } catch (e) { status.textContent = String(e.message || e); return; }
    if (me !== token || dead) return;
    // Letters by frequency, each its colour.
    const order = [...a.letters].sort((x, y) => y.count - x.count);
    const index = new Map(order.map((l, k) => [l.letter, k]));
    const colour = (letter) => letterColour(index.get(letter) ?? 99, ctx.theme);
    drawComplex(id, a.complex, (k) => colour(a.complex.letters[k]));
    // The grown patch: unit base, scaled to a block's size, placed east of the place.
    const scale = source === 'buildings' ? 12 : 60;
    const extent = Math.max(...a.complex.points.map((p) => p[0])) - centre[0];
    const offset = [centre[0] + Math.max(200, extent) + 120, centre[1]];
    drawComplex(grownId, a.grown, (k) => colour(a.grown.letters[k]), offset, scale, 0.95);
    lastGrown = offset;
    const total = order.reduce((s, l) => s + l.count, 0);
    alphabet.replaceChildren(...order.slice(0, 14).map((l) => h('div', { class: 'gs-letter' },
      h('i', { style: `background:${P.css(colour(l.letter))}` }), h('span', { class: 'mono' }, l.letter), h('b', {}, `${Math.round((100 * l.count) / total)}%`))));
    readout.replaceChildren(
      h('div', {}, h('span', {}, 'triangles'), h('b', {}, `${a.complex.triangles.length.toLocaleString('en')}${a.skipped ? `; ${a.skipped} ${source === 'buildings' ? 'footprints' : 'faces'} left out, not simple` : ''}`)),
      h('div', {}, h('span', {}, 'letters'), h('b', {}, `${a.letters.length} at 15°`)),
      h('div', {}, h('span', {}, 'entropy'), h('b', {}, `${a.entropy.toFixed(2)} bits a triangle`)),
      h('div', {}, h('span', {}, 'given the neighbour'), h('b', {}, `${a.conditionalEntropy.toFixed(2)} bits: the neighbour tells ${(a.gluedEntropy - a.conditionalEntropy).toFixed(2)}`)),
      h('div', {}, h('span', {}, 'children a triangle'), h('b', {}, `${a.meanOffspring.toFixed(3)}: below one, so patches are small unless drawn large`)),
      h('div', {}, h('span', {}, 'grown'), h('b', {}, `${a.grown.triangles.length} triangles to the east, ${a.reached ? `the first of ${a.draws} draws with ${a.asked} or more` : `the largest of ${a.draws} draws (none reached ${a.asked})`}; ${a.refused} refused for overlapping`)));
    status.textContent = 'Each triangle is coloured by its letter, its angles to 15°. To the east, a patch grown by the grammar of their gluing.';
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'Words of triangles'), h('div', { class: 'gs-row' }, h('span', {}, 'of the'), keys),
    readout, alphabet, status, h('div', { class: 'ld-quiet' }, 'Ways and buildings: Overture Maps, OpenStreetMap. syntaxis: 2-simplices as words.'));
  ctx.onDispose(() => { dead = true; });
  return {
    legend,
    pickAt(lat, lon) {
      const p = frame.toLocal(lat, lon);
      centre = [Math.round(p[0] * 10) / 10, Math.round(p[1] * 10) / 10];
      compute();
      return { kind: 'place', title: 'Read as triangles', lines: [source], at: { lat, lon } };
    },
    dispose() { dead = true; gis.remove(id); gis.remove(grownId); },
  };
}
