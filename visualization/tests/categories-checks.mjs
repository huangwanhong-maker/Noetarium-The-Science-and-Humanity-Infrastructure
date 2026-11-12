// Checks of categories drawn (visualization/js/categories): the layers of a diagram (an arrow runs forward wherever no cycle
// forbids it), the order within layers (a square is drawn without a crossing), the layout's frame (every object and every
// arrow's name inside it, no two objects in one place), parallel arrows bowed apart and loops as petals; string diagrams
// (every wire a path from its beginning to its end, boxes of one depth in one column, a symmetry as two wires that cross,
// the depths in order from left to right); and the markup of each of kategoria's views (what is marked is marked, what is
// too large is said to be, names are escaped). Called by run-tests.mjs with the suite's check and assert.

import { categories as K } from '../js/index.js';

const arrow = (name, dom, cod, kind = 'generator') => ({ name, dom, cod, kind });
const category = (objects, arrows, more = {}) => {
  const all = [...objects.map((o, i) => arrow(`id_${o}`, i, i, 'identity')), ...arrows];
  return { type: 'category', name: 'C', finite: true, objects, arrows: all, generators: all.map((a, i) => i).filter((i) => all[i].kind === 'generator'), facts: [], ...more };
};
const count = (text, part) => text.split(part).length - 1;

export function categoriesChecks({ assert, check }) {
  // ---- layers and order ----
  const square = category(['A', 'B', 'C', 'D'], [arrow('f', 0, 1), arrow('g', 1, 3), arrow('h', 0, 2), arrow('k', 2, 3), arrow('g ∘ f', 0, 3, 'composite')]);
  {
    const edges = square.generators.map((i) => square.arrows[i]);
    const { layer, back } = K.layers(4, edges);
    assert('categories: a square has three layers, its arrows all running forward', layer.join(' ') === '0 1 1 2' && back.size === 0);
    const cycle = [arrow('a', 0, 1), arrow('b', 1, 2), arrow('c', 2, 0)];
    const turned = K.layers(3, cycle);
    assert('categories: of a cycle one arrow is found to run back, and the rest are layered forward', turned.back.size === 1 && cycle.filter((e, k) => !turned.back.has(k)).every((e) => turned.layer[e.cod] > turned.layer[e.dom]));
    const rows = K.ordered([0, 1, 1, 2, 2], [arrow('a', 0, 1), arrow('b', 0, 2), arrow('c', 1, 4), arrow('d', 2, 3)]);
    assert('categories: within a layer objects stand beside their neighbours: two arrows that would cross are uncrossed', (rows[1].indexOf(1) < rows[1].indexOf(2)) === (rows[2].indexOf(4) < rows[2].indexOf(3)), rows.map((r) => r.join(',')).join(' | '));
    const lone = K.layers(3, [arrow('a', 0, 2), arrow('b', 1, 2)]);
    assert('categories: an object nothing leads to stands just before what it leads to', lone.layer.join(' ') === '0 0 1');
  }

  // ---- the layout ----
  {
    const L = K.layoutCategory(square);
    const inside = L.nodes.every((n) => n.x - n.rx >= 0 && n.x + n.rx <= L.width && n.y - n.ry >= 0 && n.y + n.ry <= L.height) && L.edges.every((e) => e.label.x > 0 && e.label.x < L.width && e.label.y > 0 && e.label.y < L.height);
    assert('categories: a layout holds every object and every arrow\'s name within its frame', inside && L.edges.length === 4, `${L.width} × ${L.height}`);
    const apart = L.nodes.every((a, i) => L.nodes.every((b, j) => i === j || Math.hypot(a.x - b.x, a.y - b.y) > 30));
    assert('categories: no two objects stand in one place, and layers run from left to right', apart && L.nodes[0].x < L.nodes[1].x && L.nodes[1].x < L.nodes[3].x && Math.abs(L.nodes[1].x - L.nodes[2].x) < 1e-9);
    check('categories: an arrow begins and ends on the rims of its objects (px from the centres, less the rim)', Math.abs(Math.hypot(L.edges[0].from.x - L.nodes[0].x, L.edges[0].from.y - L.nodes[0].y) - 21) + Math.abs(Math.hypot(L.edges[0].to.x - L.nodes[1].x, L.edges[0].to.y - L.nodes[1].y) - 24), 8);
    const pair = K.layoutCategory(category(['0', '1'], [arrow('u', 0, 1), arrow('v', 0, 1), arrow('w', 1, 0)]));
    const mids = pair.edges.map((e) => e.label.y);
    assert('categories: three arrows between two objects are bowed apart, each with its name at another height', new Set(mids.map((y) => Math.round(y / 8))).size === 3, mids.map((y) => y.toFixed(0)).join(' '));
    const monoid = K.layoutCategory(category(['∗'], [arrow('s', 0, 0), arrow('t', 0, 0), arrow('r', 0, 0)]));
    const angles = monoid.edges.map((e) => e.towards);
    assert('categories: the arrows of a monoid are petals about its one object, a third of a turn apart', monoid.edges.every((e) => e.loop && /C/.test(e.d)) && Math.abs(angles[1] - angles[0] - (2 * Math.PI) / 3) < 1e-9);
    assert('categories: a category with no arrows but identities is its objects in one column', K.layoutCategory(category(['a', 'b', 'c'], [])).edges.length === 0 && K.layoutCategory(category([], [])).width >= 40);
  }

  // ---- string diagrams ----
  {
    const assoc = { words: '(m ⊗ x) ; m', wires: ['x', 'x', 'x', 'x', 'x'], boxes: [{ name: 'm', in: [0, 1], out: [2], depth: 0 }, { name: 'm', in: [2, 3], out: [4], depth: 1 }], inputs: [0, 1, 3], outputs: [4] };
    const L = K.layoutStrings(assoc);
    assert('strings: boxes of greater depth stand further right, and every wire runs from left to right', L.boxes[0].x < L.boxes[1].x && L.wires.every((w) => w.points.every((p, k) => k === 0 || p.x > w.points[k - 1].x)));
    assert('strings: a wire into a box ends on its left side, a wire out begins on its right', Math.abs(L.wires[0].points.at(-1).x - (L.boxes[0].x - L.boxes[0].w / 2)) < 1e-9 && Math.abs(L.wires[2].points[0].x - (L.boxes[0].x + L.boxes[0].w / 2)) < 1e-9);
    assert('strings: the wires in begin in their order from the top, and a wire that passes a column keeps clear of its box', L.wires[0].points[0].y < L.wires[1].points[0].y && L.wires[1].points[0].y < L.wires[3].points[0].y &&
      L.wires[3].points[1].y > L.boxes[0].y + L.boxes[0].h / 2);
    const swap = { words: 'σ', wires: ['x', 'y'], boxes: [], inputs: [0, 1], outputs: [1, 0] };
    const S = K.layoutStrings(swap);
    assert('strings: a symmetry is two wires that cross', S.wires[0].points[0].y < S.wires[1].points[0].y && S.wires[0].points.at(-1).y > S.wires[1].points.at(-1).y);
    const svg = K.svgStrings(swap);
    assert('strings: a diagram with more than one type names its wires at both ends; with one, it does not', count(svg, 'class="k-type"') === 4 && count(K.svgStrings(assoc), 'class="k-type"') === 0);
    const unit = { words: '(e ⊗ x) ; m', wires: ['x', 'x', 'x'], boxes: [{ name: 'e', in: [], out: [0], depth: 0 }, { name: 'm', in: [0, 1], out: [2], depth: 1 }], inputs: [1], outputs: [2] };
    const U = K.layoutStrings(unit);
    assert('strings: a box with no wire in stands on the side its wire will enter the next box: a unit above the wire it meets first', U.boxes[0].y < U.wires[1].points[1].y, `${U.boxes[0].y.toFixed(0)} against ${U.wires[1].points[1].y.toFixed(0)}`);
    assert('strings: the markup has a path for each wire, a rectangle for each box, a disc at each end', count(K.svgStrings(assoc), '<path') === 5 && count(K.svgStrings(assoc), '<rect') === 2 && count(K.svgStrings(assoc), '<circle') === 4);
    assert('strings: an equation is two diagrams with a sign between; one diagram alone has none', count(K.htmlEquation(assoc, swap), '<svg') === 2 && /k-sign/.test(K.htmlEquation(assoc, swap)) && !/k-sign/.test(K.htmlEquation(assoc)));
  }

  // ---- the views ----
  {
    const svg = K.svgCategory(square);
    assert('views: a category is drawn by its generators, each with a shaft, a head and its name; composites only when asked', count(svg, 'class="k-shaft"') === 4 && count(svg, 'class="k-head"') === 4 && !svg.includes('g ∘ f') &&
      count(K.svgCategory(square, { all: true }), 'class="k-shaft"') === 5 && /k-arrow composite/.test(K.svgCategory(square, { all: true })));
    const cone = K.htmlCone({ type: 'cone', category: square, colimit: false, shape: { objects: [1, 2, 3], arrows: [5, 7] }, apex: 0, legs: [4, 6, 8] });
    assert('views: a cone marks its diagram, its apex and its legs, a composite leg among them, and names the limit', count(cone, 'k-obj apex') === 1 && count(cone, 'k-obj shape') === 3 && count(cone, 'leg"') === 3 && /the limit: A/.test(cone) && cone.includes('g ∘ f'));
    const many = category(Array.from({ length: 40 }, (_, i) => `o${i}`), []);
    assert('views: a category too large to draw is said to be, with its counts', /40 objects/.test(K.svgCategory(many)) && !/<svg/.test(K.svgCategory(many)) && K.DRAWN.objects === 26);
    const named = category(['<a>', 'b&c'], [arrow('"f"', 0, 1)]);
    assert('views: names are escaped in the markup', K.svgCategory(named).includes('&lt;a&gt;') && K.svgCategory(named).includes('b&amp;c') && !K.svgCategory(named).includes('<a>'));
    const arrowC = category(['0', '1'], [arrow('u', 0, 1)]);
    const functor = K.htmlFunctor({ type: 'functor', name: 'F', source: arrowC, target: square, objects: [0, 3], arrows: [0, 3, 8] });
    assert('views: a functor is two diagrams with each object\'s hue under the object it is sent to, and its generators\' images in words', count(functor, '<svg') === 2 && count(functor, 'class="k-dot"') === 4 && functor.includes('u ↦ g ∘ f') &&
      functor.includes(`fill="${K.HUES[0]}"`) && functor.includes('0 ↦ A') && functor.includes('1 ↦ D'));
    const sets = { type: 'sets', name: 'X', category: arrowC, elements: [['p', 'q'], ['r']], actions: [{ arrow: 2, map: [0, 0] }] };
    assert('views: a set-valued functor is a slip for each object with its elements, and a line for each element a generator moves', count(K.svgSets(sets), 'class="k-set"') === 2 && count(K.svgSets(sets), 'class="k-act"') === 2 &&
      count(K.svgSets(sets), 'class="k-element"') === 3 && /u : 0 → 1/.test(K.htmlSets(sets)));
    const long = { ...sets, elements: [Array.from({ length: 30 }, (_, i) => `e${i}`), ['r']], actions: [{ arrow: 2, map: new Array(30).fill(0) }] };
    assert('views: a set of many elements shows its first eleven and says how many more', /and 19 more/.test(K.svgSets(long)) && count(K.svgSets(long), 'class="k-act"') === 11);
    const derivation = K.htmlDerivation({ type: 'derivation', steps: [{ edges: ['f', 'g'], at_object: 'A', words: 'g ∘ f', at: 0, span: 2, forward: true, equation: 0, by: 'g ∘ f = k ∘ h' }, { edges: ['h', 'k'], at_object: 'A', words: 'k ∘ h' }] });
    assert('views: a derivation marks the part each step replaces, names the equation, and writes the arrow travelled first at the right', count(derivation, 'k-edge hit') === 2 && derivation.includes('by g ∘ f = k ∘ h') &&
      derivation.indexOf('>g<') < derivation.indexOf('>f<') && /which was to be reached/.test(derivation));
    const table = K.htmlComposition({ ...arrowC, table: [[0, -1, -1], [-1, 1, 2], [2, -1, -1]] });
    assert('views: a table of composition has a row and a column for each arrow, a dot where two do not compose', count(table, '<tr>') === 4 && count(table, 'class="none"') === 5 && /g ∘ f/.test(table));
    assert('views: render gives each kind of view its drawing, and nothing for what is no view', /<svg/.test(K.render(square)) && /k-derivation/.test(K.render({ type: 'derivation', steps: [] })) && /<table/.test(K.render({ type: 'table', columns: ['a'], rows: [['1']] })) &&
      K.render(null) === '' && K.render({ type: 'unknown' }) === '' && /None/.test(K.render({ type: 'table', columns: ['a'], rows: [] })));
    const laws = K.htmlLaws({ type: 'laws', laws: [{ name: 'unit', status: 'fails', reason: 'the two sides differ', left: { words: 'x', wires: ['x'], boxes: [], inputs: [0], outputs: [0] }, right: { words: 'x', wires: ['x'], boxes: [], inputs: [0], outputs: [0] } }] });
    assert('views: a law that fails is told by a glyph and a word as well as a colour, with its reason and an unequal sign', laws.includes(K.GLYPH.fails) && /<em>fails<\/em>/.test(laws) && laws.includes('≠') && laws.includes('the two sides differ'));
  }
}
