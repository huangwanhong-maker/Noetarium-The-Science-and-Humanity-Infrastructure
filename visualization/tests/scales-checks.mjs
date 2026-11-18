// Checks of systems across scales drawn (visualization/js/dynamics/scales.js, ADR 0128): a model's levels as bands, the coarsest at
// the top; a glyph for every quantity and derived quantity, by role; an arrow for every quantity carried between levels (up for a
// sum, down for a parent's value) and none for what works within a level; what is still needed ringed; units on a plane, one outline
// each, a unit with no value hatched, and the groups named. Called by run-tests.mjs with the suite's check and assert.

import { dynamics as D } from '../js/index.js';

const count = (text, part) => text.split(part).length - 1;

export function scalesChecks({ assert }) {
  const described = {
    levels: [{ name: 'block', units: 6 }, { name: 'area', units: 2 }, { name: 'city', units: 1 }],
    quantities: [{ name: 'h', level: 'block', role: 'state', form: 'map', expression: 'h + s * d' }, { name: 'dist', level: 'block', role: 'fixed' },
      { name: 'inc', level: 'city', role: 'driver' }],
    derived: [{ name: 'H', level: 'area', op: 'sum', of: 'h' }, { name: 'd', level: 'block', op: 'parent', of: 'inc_a' },
      { name: 'inc_a', level: 'area', op: 'parent', of: 'inc' }, { name: 's', level: 'block', op: 'logit', of: 'beta * log(h + 1)' }],
    missing: [{ quantity: 'dist', level: 'block', what: 'a value' }],
  };
  const svg = D.modelDiagram(described);
  assert('scales: a band for each level', count(svg, 'class="ms-band') === 3);
  assert('scales: the coarsest level is drawn at the top', svg.indexOf('>city<') < svg.indexOf('>area<') && svg.indexOf('>area<') < svg.indexOf('>block<'));
  assert('scales: a glyph for every quantity and derived quantity, by role',
    count(svg, 'ms-state') === 1 && count(svg, 'ms-driver') === 1 && count(svg, 'ms-fixed') === 1 && count(svg, 'ms-derived"') === 4, svg.slice(0, 300));
  assert('scales: an arrow up for the sum and down for each parent\'s value, none for the logit within its level',
    count(svg, 'ms-arrow ms-up') === 1 && count(svg, 'ms-arrow ms-down') === 2 && count(svg, 'class="ms-arrow') === 3);
  assert('scales: a state\'s law beside it', svg.includes('h ← h + s * d'));
  assert('scales: what is still needed is ringed', count(svg, 'class="ms-need"') === 1 && svg.includes('to be declared: dist'));

  const rings = { a: [[0, 0], [1, 0], [1, 1], [0, 0]], b: [[2, 0], [3, 0], [3, 1], [2, 0]] };
  const map = D.unitsMap({ rings, ids: ['a', 'b'], colours: ['#c586b6', '#00000000'], groups: ['left', 'right'] });
  assert('scales: one outline for each unit', count(map, 'class="ms-unit"') === 2);
  assert('scales: a unit with no value is hatched', map.includes('fill:url(#ms-hatch)'));
  assert('scales: each group is named at its middle', map.includes('>left<') && map.includes('>right<'));
}
