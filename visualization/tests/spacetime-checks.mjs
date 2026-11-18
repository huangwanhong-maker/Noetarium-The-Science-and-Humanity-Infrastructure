// Checks of evidence in space and time drawn as a cube (visualization/js/evidence/spacetime.js), ADR 0127. On a small reply of the
// theories engine: clock times on the epoch's day and past midnight; the slice between two exact fixes as kinesis gives it; the
// intersection of two discs with its corners, of discs apart (none) and of a disc within another (its own circle); a history's
// place between its points; the projection from above (the plan) and from the side (time up); ticks of round minutes; the model's
// window from the rooms, the items within it and those outside, each person's ink, the theory's stance on each item; the cube with
// its floor, prisms, items (a doubted one crossed, a held-back one told apart), the theory's paths and events, and the plane with each
// person's slice; the plan at an instant with only the items speaking near it. Called by run-tests.mjs.

import { evidence as E } from '../js/index.js';

const EPOCH = { given: true, epoch: 1773523800, offset: 0, seconds_of_day: 77400, clock: '21:30:00' };
const MATTER = {
  place: {
    scene: { x: 0, y: 0, name: 'the east door' },
    water: [{ name: 'the river', points: [[-800, -30], [800, -40], [800, -120], [-800, -110]] }],
    buildings: [{ name: 'the boathouse', points: [[-26, -10], [0, -10], [0, 16], [-26, 16]] }],
    ways: [{ name: 'the towpath', kind: 'path', points: [[-800, -15], [800, -20]] }],
  },
};
const REPLY = {
  evidence: {
    epoch: EPOCH,
    rooms: [{ kind: 'origin', x: 0, y: 0, radius: 6, from: 300, until: 1500, from_clock: '21:35:00', until_clock: '21:55:00' }],
    agents: [
      { id: 'vane', name: 'Corrie Vane', speed: 6, identified: true, fixes: [{ id: 'inn', x: -560, y: 210, t: 780, radius: 3 }, { id: 'home', x: 1650, y: 940, t: 1260, radius: 15 }] },
      { id: 'grey', name: 'the man in the grey coat', speed: 2, identified: false, fixes: [{ id: 'jogger', x: -120, y: -70, t: 1200, radius: 620 }] },
    ],
    located: [
      { id: 'inn', says: 'sighting', agent: 'vane', x: -560, y: 210, radius: 3, from: 780, until: 780, from_clock: '21:43:00', until_clock: '21:43:00', credibility: 0.6, held_back: false, text: 'at the inn' },
      { id: 'home', says: 'sighting', agent: 'vane', x: 1650, y: 940, radius: 15, from: 1260, until: 1260, from_clock: '21:51:00', until_clock: '21:51:00', credibility: 0.7, held_back: false },
      { id: 'report', says: 'event', kind: 'ignite', x: 0, y: 0, radius: 6, from: 300, until: 1500, from_clock: '21:35:00', until_clock: '21:55:00', credibility: 0.85, held_back: false },
      { id: 'log', says: 'absence', kind: 'fault', x: 0, y: 0, radius: 15, from: -4500, until: 1800, from_clock: '20:15:00', until_clock: '22:00:00', credibility: 0.6, held_back: false },
      { id: 'receipt', says: 'sighting', agent: 'vane', x: 905, y: -700, radius: 10, from: -8232, until: -8232, from_clock: '19:12:48', until_clock: '19:12:48', credibility: 0.95, held_back: false },
      { id: 'late', says: 'sighting', agent: 'vane', x: -560, y: 210, radius: 3, from: 860, until: 860, from_clock: '21:44:20', until_clock: '21:44:20', credibility: 0.97, held_back: true },
    ],
  },
};
const THEORY = {
  serial: 12, central: 'ignite by the man in the grey coat',
  events: [{ kind: 'ignite', roles: { actor: 'grey' }, x: 0, y: 0, t: 960, clock: '21:46:00' }],
  readings: [{ item: 'inn', believed: false }, { item: 'report', believed: true }],
  trajectories: [{ agent: 'grey', status: 'consistent', witness: [{ x: 0, y: 0, t: 960 }, { x: -120, y: -55, t: 1080 }] }],
};

export function spacetimeChecks({ assert, check }) {
  assert('cube: a clock time on the epoch\'s day', E.clockOf(EPOCH, 780) === '21:43:00' && E.clockOf(EPOCH, 780, false) === '21:43');
  assert('cube: past midnight, the day is said', E.clockOf(EPOCH, 3 * 3600) === '00:30:00 (+1 d)');
  {
    const s = E.sliceAt([{ x: 0, y: 0, t: 0, radius: 0 }, { x: 100, y: 0, t: 100, radius: 0 }], 2, 50);
    check('cube: between two exact fixes the earlier cone has grown to v (t - t_A)', s[0].r - 100, 1e-12);
    check('cube: ... and the later one shrunk to v (t_B - t)', s[1].r - 100, 1e-12);
    const before = E.sliceAt([{ x: 0, y: 0, t: 0, radius: 1 }, { x: 100, y: 0, t: 100, radius: 0 }], 2, -10);
    assert('cube: before the first fix, its past cone alone', before.length === 1 && Math.abs(before[0].r - 21) < 1e-12);
  }
  {
    const discs = [{ x: 0, y: 0, r: 5 }, { x: 6, y: 0, r: 5 }];
    const lens = E.lensOf(discs);
    assert('cube: the lens of two discs lies within both', lens.length > 10 && lens.every(([x, y]) => Math.hypot(x, y) <= 5 + 1e-9 && Math.hypot(x - 6, y) <= 5 + 1e-9));
    assert('cube: ... and keeps the corners where the circles cross, (3, ±4)', lens.some(([x, y]) => Math.abs(x - 3) < 1e-9 && Math.abs(y - 4) < 1e-9) && lens.some(([x, y]) => Math.abs(x - 3) < 1e-9 && Math.abs(y + 4) < 1e-9));
    assert('cube: discs apart have no lens', E.lensOf([{ x: 0, y: 0, r: 1 }, { x: 5, y: 0, r: 1 }]).length === 0);
    assert('cube: a disc within another is its own circle', E.lensOf([{ x: 0, y: 0, r: 10 }, { x: 1, y: 0, r: 2 }], 40).length === 40);
  }
  {
    const p = E.positionAt([{ x: 0, y: 0, t: 0 }, { x: 10, y: 20, t: 10 }], 5);
    assert('cube: a history\'s place halfway between its points', p[0] === 5 && p[1] === 10);
    const top = E.projector({ azimuth: 0, elevation: Math.PI / 2, rise: 1, t0: 0 }), side = E.projector({ azimuth: 0, elevation: 0, rise: 2, t0: 0 });
    const [a, b] = top(3, 4, 99);
    check('cube: from above, the plan: x as it is', a - 3, 1e-12);
    check('cube: ... y down the page, time unseen', b + 4, 1e-12);
    check('cube: from the side, time rises up the page', side(3, 4, 10)[1] + 20, 1e-12);
    const ticks = E.timeTicksOf(0, 2400, EPOCH);
    assert('cube: ticks of round minutes, 21:30 to 22:10 every five', ticks.length === 9 && ticks[0].words === '21:30' && ticks[1].words === '21:35');
  }
  const model = E.cubeOf(MATTER, REPLY, { theory: THEORY });
  assert('cube: the window is the rooms\' with a margin', model.t0 === 300 - 900 && model.t1 === 1500 + 600);
  assert('cube: items outside the window are listed apart (the receipt hours before)', model.outside.includes('receipt') && !model.outside.includes('inn'));
  assert('cube: each person has an ink, and an item takes its person\'s', model.agents[0].ink !== model.agents[1].ink && model.items.find((i) => i.id === 'inn').ink === model.agents[0].ink);
  assert('cube: the theory\'s stance on each item is carried', model.items.find((i) => i.id === 'inn').reading?.believed === false);
  const svg = E.svgCube(model, { at: 960, selected: 'report' });
  assert('cube: the cube is markup with its floor and its place', svg.startsWith('<svg class="spacetime"') && svg.includes('st-floor') && svg.includes('st-water') && svg.includes('st-building'));
  assert('cube: a prism for the person with two fixes in the window', (svg.match(/class="st-prism"/g) || []).length >= 1 && svg.includes('data-agent="vane"'));
  assert('cube: an item doubted by the theory is crossed, one held back told apart', /class="st-item sighting doubted"[^>]*data-item="inn"/.test(svg) && /class="st-item sighting held"[^>]*data-item="late"/.test(svg));
  assert('cube: the chosen item is marked', /class="st-item event chosen" data-item="report"/.test(svg));
  assert('cube: the theory\'s path and its event with its clock', svg.includes('class="st-path"') && svg.includes('ignite by the man in the grey coat, 21:46:00'));
  assert('cube: the plane at 21:46 with each person\'s slice', svg.includes('st-sheet') && (svg.match(/class="st-slice"/g) || []).length === 2 && svg.includes('21:46:00'));
  const near = E.cubeOf(MATTER, REPLY, { theory: THEORY, zoom: 4 });
  check('cube: closer by four, the plane is a quarter as wide', near.half - model.half / 4, 1e-9);
  check('cube: ... about a centre moved toward the scene by three quarters of the way', near.cx - (model.cx + (0 - model.cx) * 0.75), 1e-9);
  const nearSvg = E.svgCube(near, {});
  assert('cube: ... and an item off the plane is not drawn (home, 1.9 km away), nor the prism of a fix wider than the plane', !nearSvg.includes('data-item="home"') && nearSvg.includes('data-item="report"') && nearSvg.includes('clip-path="url(#st-box-'));
  const plan = E.svgPlan(model, { at: 780, window: 60 });
  assert('cube: the plan at 21:43 shows the items speaking near it, and not the others', plan.includes('data-item="inn"') && !plan.includes('data-item="home"') && plan.includes('data-item="report"'));
  assert('cube: ... with each person\'s slice', (plan.match(/class="st-slice"/g) || []).length === 2);
}
