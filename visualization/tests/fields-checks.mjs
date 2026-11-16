// Checks of fields on a grid drawn (visualization/js/dynamics/fields.js, ADR 0124): the kept cells in the order of a field's
// values; a logarithmic scale's ends and ticks, a linear one's; a change's scale symmetric, on a signed square root; tones
// from the validated maps, a cell with no value left clear; arrows summed over blocks, pointing where the current goes (north
// up), their lengths by the square root of their sizes; an SVG with a square per kept cell; a legend of either kind.
// Called by run-tests.mjs with the suite's check and assert.

import { dynamics as D } from '../js/index.js';
import { cssColour } from '../styles/colour.js';
import { colourMap } from '../styles/palettes.js';

const count = (text, part) => text.split(part).length - 1;

export function fieldsChecks({ assert, check }) {
  const land = [[1, 1, 0], [1, 0, 1]];
  const cells = D.cellsOf(2, 3, land);
  assert('fields: the kept cells are those of the mask, row by row from the north', cells.map((c) => `${c.row}${c.col}`).join() === '00,01,10,12');

  const s = D.scaleOf([0, 9, 99, 9999], { log: true });
  check('fields: on a logarithmic scale nothing is at 0 and the most at 1', Math.abs(s.at(0)) + Math.abs(s.at(9999) - 1), 1e-12);
  check('fields: and 99 halfway between 0 and 9999 (log10 of one more)', s.at(99) - 0.5, 1e-12);
  assert('fields: its ticks are 0, 1, 3, 10, 30 … up to the most', s.ticks.join() === '0,1,3,10,30,100,300,1000,3000', s.ticks.join());
  const lin = D.scaleOf([2, 12], { log: false });
  check('fields: on a linear scale the middle value is at a half', lin.at(7) - 0.5, 1e-12);

  const c = D.changeScaleOf([-40, 10, 160]);
  check('fields: a change\'s scale is symmetric, the largest change at one', Math.abs(c.at(160) - 1) + Math.abs(c.at(-160) + 1) + Math.abs(c.at(0)), 1e-12);
  check('fields: on a signed square root, a quarter of the largest at a half', c.at(40) - 0.5, 1e-12);

  const tones = D.coloursOf([0, 9999, NaN], s);
  const orchid = colourMap('orchid').colour;
  const night = D.coloursOf([0, 9999], s, { reverse: true });
  assert('fields: magnitudes take the orchid map\'s ends, pale for none and deep for the most, and a cell with no value is left clear',
    tones[0] === cssColour(orchid(0)) && tones[1] === cssColour(orchid(1)) && tones[2] === '#00000000', tones.join());
  assert('fields: reversed for a dark ground, the most is the pale end', night[1] === cssColour(orchid(0)) && night[0] === cssColour(orchid(1)));
  const lr = colourMap('lavender-rust');
  const signed = D.coloursOf([-160, 160], c, { diverging: true });
  assert('fields: a loss is lavender and a gain rust, the diverging map\'s ends', signed[0] === cssColour(lr.at(-1)) && signed[1] === cssColour(lr.at(1)), signed.join());
  assert('fields: a change is the difference cell by cell', D.changeOf([1, 2, 3], [2, 2, 1]).join() === '1,0,-2');

  // A 4 × 4 grid, every cell carried east by 1 and north by 2: one block of 2 × 2 sums to (4, 8).
  const full = [[1, 1, 1, 1], [1, 1, 1, 1], [1, 1, 1, 1], [1, 1, 1, 1]];
  const east = Array(16).fill(1), north = Array(16).fill(2);
  const a = D.arrowsOf({ rows: 4, cols: 4, land: full, east, north, block: 2 });
  assert('fields: currents summed over blocks of 2 × 2 make four arrows', a.arrows.length === 4 && Math.abs(a.largest - Math.hypot(4, 8)) < 1e-12);
  const one = a.arrows[0];
  check('fields: an arrow points where the current goes, north drawn up', Math.abs(one.dx / Math.hypot(one.dx, one.dy) - 1 / Math.sqrt(5)) + Math.abs(one.dy / Math.hypot(one.dx, one.dy) + 2 / Math.sqrt(5)), 1e-12);
  const uneven = D.arrowsOf({ rows: 1, cols: 4, land: [[1, 1, 1, 1]], east: [4, 0, 1, 0], north: [0, 0, 0, 0], block: 2, reach: 2 });
  check('fields: an arrow\'s length is by the square root of its size: a quarter of the current, half the length', Math.hypot(uneven.arrows[1].dx, uneven.arrows[1].dy) - 1, 1e-12);

  const svg = D.fieldSvg({ rows: 2, cols: 3, land, colours: ['#111111', '#222222', '#333333', '#444444'], cell: 5, arrows: a.arrows.slice(0, 1), marks: [{ row: 1, col: 2 }] });
  assert('fields: a field as SVG has a square for each kept cell, the ground beneath, an arrow and a mark', count(svg, '<rect x=') === 4 && svg.includes('class="fd-ground"') && count(svg, 'fd-arrow') === 1 && count(svg, 'fd-mark') === 1 && svg.startsWith('<svg class="fd-field"'));
  let refused = false;
  try { D.fieldSvg({ rows: 2, cols: 3, land, colours: ['#111111'] }); } catch { refused = true; }
  assert('fields: colours that are not one per kept cell are refused', refused);
  const over = D.overlaySvg({ rows: 4, cols: 4, cell: 10, arrows: a.arrows, marks: [{ row: 0, col: 0, label: 'a new town' }] });
  assert('fields: the overlay holds the arrows and a labelled mark, sized to the grid', count(over, 'fd-arrow') === 4 && over.includes('a new town') && over.includes('viewBox="0 0 40 40"'));
  const leg = D.legendSvg({ scale: s, label: 'people per km²' });
  const legc = D.legendSvg({ scale: c, diverging: true, label: 'change' });
  assert('fields: a legend has its ramp, its label and a tick for each of its scale\'s', leg.includes('linearGradient') && leg.includes('people per km²') && count(leg, 'fd-tick-label') === s.ticks.length && count(legc, 'fd-tick-label') === 5 && legc.includes('>+160<'));
}
