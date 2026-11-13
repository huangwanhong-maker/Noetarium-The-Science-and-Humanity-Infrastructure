// Checks of dynamical systems drawn (visualization/js/dynamics): round values for an axis; a chart's scales (the corners
// of its ranges land on the corners of its plot), its thinning, its dots, marks and legend, what is not a number left
// out, names escaped; a map of a magnitude or of labels, cell by cell, with its legend; eigenvalues told stable from
// not, about the axis or the circle; a braid's strands, each keeping its ink through the crossings, the strand beneath
// drawn first and broken; the forms of a rewriting system by depth; and the ten inks of lines, apart in every vision
// and legible on both grounds. Called by run-tests.mjs with the suite's check and assert.

import { dynamics as D } from '../js/index.js';
import { cssColour, deltaE, hexRgb, toLinear } from '../styles/colour.js';
import { DEFICIENCIES, simulate } from '../styles/cvd.js';
import { MAPS, colourMap } from '../styles/palettes.js';

const count = (text, part) => text.split(part).length - 1;
const strokes = (svg) => [...svg.matchAll(/<path d="[^"]*" stroke="(#[0-9a-f]{6})"/g)].map((m) => m[1]);

export function dynamicsChecks({ assert, check }) {
  // ---- round values ----
  {
    const unit = D.ticks(0, 1, 5), signed = D.ticks(-3, 7, 5);
    check('dynamics: about five round values across [0, 1] are the fifths', Math.max(...unit.map((v, k) => Math.abs(v - 0.2 * k))) + Math.abs(unit.length - 6), 1e-12);
    assert('dynamics: across [-3, 7] they are the even numbers, zero written as zero', signed.join() === '-2,0,2,4,6' && Object.is(signed[1], 0), signed.join());
    assert('dynamics: a range of no width has one value', D.ticks(5, 5).join() === '5');
  }

  // ---- a chart ----
  {
    const one = D.lineChart({ series: [{ name: 'x', x: [0, 10], y: [0, 1] }], xRange: [0, 10], yRange: [0, 1], width: 640, height: 300, what: 'a <b> & "c"' });
    assert('dynamics: the corners of a chart\'s ranges land on the corners of its plot', one.includes('<rect class="d-plot" x="54" y="10" width="574" height="252"/>') && one.includes('d="M54.0 262.0L628.0 10.0"'));
    assert('dynamics: what a chart is of is said to a reader that cannot see it, escaped', one.includes('aria-label="a &lt;b&gt; &amp; &quot;c&quot;"') && one.startsWith('<svg class="d-chart"') && one.endsWith('</svg>'));
    assert('dynamics: a chart of one series has no legend, and one of three names each', count(one, 'd-legend') === 0
      && count(D.lineChart({ series: ['a', 'b', 'c'].map((name) => ({ name, x: [0, 1], y: [0, 1] })) }), 'class="d-legend"') === 3);
    const x = Array.from({ length: 100 }, (_, k) => k), long = D.lineChart({ series: [{ name: 'x', x, y: x }], most: 10 });
    assert('dynamics: a series longer than may be drawn is thinned evenly', count(strokes(long).length === 1 ? long.match(/<path d="([^"]*)" stroke=/)[1] : '', 'L') === 9);
    const holes = D.lineChart({ series: [{ name: 'x', x: [0, 1, 2, 3], y: [NaN, 1, Infinity, 2] }] });
    assert('dynamics: what is not a number is left out of a line, which begins at the first that is', !/NaN|Infinity/.test(holes) && /<path d="M[\d.]+ [\d.]+L[\d.]+ [\d.]+" stroke=/.test(holes));
    const dotted = D.lineChart({ series: [{ name: 'x', x: [0, 1, 2], y: [0, 1, 0] }], dots: true, marks: [{ x: 1 }, { y: 0.5, label: 'a level' }] });
    assert('dynamics: iterates are drawn as dots, and marks as a vertical and a level with its name', count(dotted, 'h0') === 3 && !/L[\d.]+ [\d.]+"/.test(dotted) && count(dotted, 'class="d-mark"') === 2 && dotted.includes('>a level</text>'));
    const equal = D.lineChart({ series: [{ name: 'p', x: [-1, 1], y: [-1, 1] }], xRange: [-1, 1], yRange: [-1, 1], equal: true, width: 640, height: 300 });
    const [, x0, y0, x1, y1] = equal.match(/d="M([\d.]+) ([\d.]+)L([\d.]+) ([\d.]+)"/).map(Number);
    check('dynamics: with equal scales one unit of x is one unit of y on the page', (x1 - x0) - (y0 - y1), 0.11, 'px');
  }

  // ---- a map of a magnitude, and of labels ----
  {
    const xs = [0, 1, 2], ys = [0, 1], values = [1, 2, 3, 4, NaN, 9];
    const deep = cssColour(colourMap('orchid').colour(1)), pale = cssColour(colourMap('orchid').colour(0));
    const map = D.heatMap({ xs, ys, values, points: [{ x: 1, y: 1, label: 'a well' }] });
    assert('dynamics: a map draws a cell for each value that is a number, and a legend of forty steps', count(map, '<rect') === 1 + 5 + 40 && count(map, 'class="d-point"') === 1 && map.includes('>a well</text>'));
    const cells = [...map.matchAll(/<rect x="[\d.]+" y="[\d.]+" width="[\d.]+" height="[\d.]+" fill="(#[0-9a-f]{6})"\/>/g)].map((m) => m[1]);
    assert('dynamics: the greatest value takes the deep end of the map and the least the pale; turned, the least is deep', cells[0] === pale && cells[4] === deep
      && [...D.heatMap({ xs, ys, values, deepLow: true }).matchAll(/height="[\d.]+" fill="(#[0-9a-f]{6})"\/>/g)][0][1] === deep, `${cells[0]} ${cells[4]} against ${pale} ${deep}`);
    const labels = D.heatMap({ xs, ys, values: [0, 0, 1, -1, 1, 1], kind: 'categorical', names: ['(1, 0)', '(-1, 0)'] });
    assert('dynamics: a map of labels draws no cell where none is given, in the layer\'s pastels, each named', count(labels, '<rect') === 1 + 5 + 2
      && count(labels, `fill="${MAPS['categorical-apart'].stops[0]}"`) === 2 + 1 && labels.includes('>(-1, 0)</text>'));
  }

  // ---- eigenvalues in the plane ----
  {
    const flow = D.complexPlane({ points: [[-1, 0], [0.5, 2], [0, 1]] }), map = D.complexPlane({ points: [[0.5, 0], [1, 0], [0, 1.2]], circle: true });
    assert('dynamics: a flow\'s eigenvalues are stable to the left of the axis, and one on it is not', count(flow, 'd-not-stable') === 2 && count(flow, '<circle') === 3 && !flow.includes('<ellipse'));
    const [, rx, ry] = map.match(/rx="([\d.]+)" ry="([\d.]+)"/).map(Number);
    assert('dynamics: a map\'s are stable within the unit circle, which is drawn round', count(map, 'd-not-stable') === 2 && Math.abs(rx - ry) < 0.2, `${rx} × ${ry}`);
  }

  // ---- a braid ----
  {
    const [a, b, c] = D.INKS;
    const positive = D.braidDiagram({ strands: 2, word: [1] }), negative = D.braidDiagram({ strands: 2, word: [-1] });
    assert('dynamics: in a positive crossing the left strand is drawn first, beneath a gap, and the right one whole above it; the strands change places with their inks',
      strokes(positive).join() === [a, b, a, b, b, a].join() && count(positive, 'class="d-gap"') === 1 && positive.indexOf('d-gap') > positive.indexOf(`stroke="${a}"`, positive.indexOf(`stroke="${b}"`)));
    assert('dynamics: in a negative crossing it is the right strand that passes beneath, and the letter is written with its sign', strokes(negative).join() === [a, b, b, a, b, a].join() && negative.includes('>−1</text>') && positive.includes('>1</text>'));
    const three = D.braidDiagram({ strands: 3, word: [1, 2] });
    assert('dynamics: s1 s2 carries the first strand to the last place: the inks at the end are the second, the third, the first', strokes(three).slice(-3).join() === [b, c, a].join() && count(three, 'class="d-gap"') === 2);
    const long = D.braidDiagram({ strands: 3, word: Array.from({ length: 12 }, (_, k) => (k % 2 ? -2 : 1)), most: 9 });
    assert('dynamics: of a word longer than may be drawn the rest is counted', count(long, 'class="d-gap"') === 9 && long.includes('… 3 more'));
  }

  // ---- the forms of a rewriting system ----
  {
    const g = D.formsGraph({ forms: [{ text: 'S' }, { text: 'a S b' }, { text: 'a b', final: true, terminal: true, histories: 2 }, { text: 'a < X', final: true, terminal: false }], edges: [[0, 1, 0], [1, 2, 1], [1, 3, 1]] });
    const y = (text) => Number(g.match(new RegExp(`y="([\\d.]+)" text-anchor="middle">${text}`))[1]);
    assert('dynamics: forms are laid out by the steps that reach them, the start at the top', y('S') < y('a S b') && y('a S b') < y('a b') && count(g, 'class="d-edge"') === 3);
    assert('dynamics: a word reached by two histories says so, a form that is stuck is struck, and names are escaped', g.includes('class="d-word"') && g.includes('a b ×2') && g.includes('class="d-dead"') && g.includes('a &lt; X'));
    // A row of many long forms: no two names run into each other, and what does not fit is counted.
    const many = { forms: [{ text: 'S' }, ...Array.from({ length: 14 }, (_, k) => ({ text: `x plus x plus E plus E ${k}` }))], edges: Array.from({ length: 14 }, (_, k) => [0, k + 1, 0]) };
    const wide = D.formsGraph(many), placed = [...wide.matchAll(/class="d-form" x="([\d.]+)" y="[\d.]+" text-anchor="middle">([^<]+)</g)].map((m) => ({ x: Number(m[1]), half: 7.3 * m[2].length / 2 })).filter((f) => f.half > 10);
    const apart = placed.every((f, k) => k === 0 || f.x - f.half >= placed[k - 1].x + placed[k - 1].half + 10);
    const counted = Number((wide.match(/… (\d+) more/) ?? [])[1]);
    assert('dynamics: in a row too long for the page no two names overlap, the first forms are drawn and the rest are counted, with the steps to those drawn alone',
      placed.length >= 2 && placed.length < 14 && apart && counted === 14 - placed.length && count(wide, 'class="d-edge"') === placed.length && placed.every((f) => f.x - f.half >= 0 && f.x + f.half <= 640), `${placed.length} drawn, ${counted} counted`);
  }

  // ---- the inks ----
  {
    assert('dynamics: ten inks, taken in turn and round again; a label\'s ground is the layer\'s pastel', new Set(D.INKS).size === 10 && D.hue(0) === D.INKS[0] && D.hue(10) === D.INKS[0] && D.hue(-1) === D.INKS[9] && D.hue(0, false) === MAPS['categorical-apart'].stops[0]);
    const rgb = D.INKS.map(hexRgb), clamp = (c) => c.map((v) => Math.min(1, Math.max(0, v)));
    const least = (n) => {
      let out = Infinity;
      for (const kind of ['normal', ...DEFICIENCIES]) {
        const seen = rgb.slice(0, n).map((c) => (kind === 'normal' ? c : clamp(simulate(c, kind))));
        for (let i = 0; i < n; i++) for (let j = 0; j < i; j++) out = Math.min(out, deltaE(seen[i], seen[j]));
      }
      return out;
    };
    check('dynamics: the first three inks, every pair, in every vision (shortfall of the least ΔE below 0.11)', Math.max(0, 0.11 - least(3)), 0);
    check('dynamics: the first four (floor 0.08)', Math.max(0, 0.08 - least(4)), 0);
    check('dynamics: all ten (floor 0.038)', Math.max(0, 0.038 - least(10)), 0);
    const luminance = (c) => { const [r, g, b] = c.map(toLinear); return 0.2126 * r + 0.7152 * g + 0.0722 * b; };
    const contrast = (p, q) => { const [hi, lo] = [luminance(p), luminance(q)].sort((u, v) => v - u); return (hi + 0.05) / (lo + 0.05); };
    const worst = Math.min(...rgb.flatMap((c) => [contrast(c, hexRgb('#fffefe')), contrast(c, hexRgb('#211d35'))]));
    check('dynamics: every ink holds a contrast of 3 on the Lab\'s paper and on its night (shortfall)', Math.max(0, 3 - worst), 0);
    assert('dynamics: the views in the round take three.js as an argument, and loading them needs no page', [D.path3d, D.strands3d, D.surface3d].every((f) => typeof f === 'function' && f.length >= 3));
  }
}
