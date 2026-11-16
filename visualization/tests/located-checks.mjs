// Checks of evidence in space and time drawn (visualization/js/evidence/located.js), ADR 0123. On a small archive and what the
// engine answered of it: written times read as the API reads them (a month is the whole month); the frame's metres against a
// degree's; the scene's claims and items placed, toned by their decibans, the items that do not count told apart with why; the place
// as markup with its claims and items, a chosen claim's items lit, a claim no evidence bears on an outline, the mode "decide" toned by
// acts and ringing the places worth looking at first; the place's ways and footprints as paths; the strip of time; a scale bar of a
// round length; ticks of months and of days; ratios in words. Called by run-tests.mjs.

import { evidence as E } from '../js/index.js';

const ARCHIVE = {
  frame: { box: { south: 40.72, north: 40.73, west: -73.99, east: -73.98 } },
  items: [
    { id: 'i1', kind: 'inspection', lat: 40.7251, lon: -73.9851, radius: 15, from: '2024-04-03T10:00:00', says: 'holds', source: 'inspector' },
    { id: 'i2', kind: 'inspection', lat: 40.7253, lon: -73.9849, radius: 15, from: '2024-05-10', says: 'denies', source: 'inspector' },
    { id: 'c1', kind: 'complaint', lat: 40.7260, lon: -73.9855, radius: 15, from: '2024-04', until: '2024-04', says: 'holds', source: 'resident', origin: 'address: 1 A', status: 'proposed' },
    { id: 'c2', kind: 'complaint', lat: 40.7290, lon: -73.9810, radius: 15, from: '2024-06-01', says: 'holds', source: 'resident', holding: 'h1' },
  ],
  claims: [
    { id: 'k1', text: 'Rats here', lat: 40.7252, lon: -73.985, radius: 40, from: '2024-04', until: '2024-06', prior: 0.5 },
    { id: 'k2', text: 'Rats there', lat: 40.7210, lon: -73.9890, radius: 40, from: '2024-04', until: '2024-06', prior: 0.5 },
  ],
};
// As the engine answers (api/cpp/science/spacetime_evidence.hpp's all), abridged.
const APPRAISAL = {
  keep: { items: [{ id: 'i1', sound: true }, { id: 'i2', sound: true }, { id: 'c1', sound: true }, { id: 'c2', sound: false, why: 'it rests on h1: the file is not there' }] },
  weigh: { claims: [
    { id: 'k1', standing: 'established', posterior: 0.96, prior: 0.5, ratio: 24, items: [{ id: 'i1', alone: 12.5 }, { id: 'i2', alone: 0.77 }], left_out: [{ id: 'c1', why: 'it is a proposal, and counts for nothing until a person accepts it' }] },
    { id: 'k2', standing: 'no evidence', posterior: 0.5, prior: 0.5, ratio: 1, items: [], left_out: [] },
  ] },
  decide: { claims: [{ claim: 'k1', act: 'treat the area', inspect: false, evsi: 0 }, { claim: 'k2', act: 'leave it', inspect: true, evsi: 0.6 }] },
};

export function locatedChecks({ assert }) {
  // ---- Times ----
  const day = E.spanOf('2024-04-03'), month = E.spanOf('2024-02'), year = E.spanOf('2024'), at = E.spanOf('2024-04-03T10:00:00'), zoned = E.spanOf('2024-04-03T10:00:00-04:00');
  assert('located: a written time is the interval it covers, as the API reads it: a date its day, a month its month (29 days in February 2024), a year its year',
    day[1] - day[0] === 86400 && month[1] - month[0] === 29 * 86400 && year[1] - year[0] === 366 * 86400 && day[0] === Date.UTC(2024, 3, 3) / 1000);
  assert('located: an instant without a zone is UTC; with one, it is placed by it', at[0] === at[1] && at[0] === Date.UTC(2024, 3, 3, 10) / 1000 && zoned[0] === Date.UTC(2024, 3, 3, 14) / 1000);
  const [from, until] = E.intervalOf('2024-04', '2024-06');
  assert('located: from the beginning of one written time to the end of another', from === Date.UTC(2024, 3, 1) / 1000 && until === Date.UTC(2024, 6, 1) / 1000 && E.intervalOf('', null)[0] === null && E.spanOf('not a time') === null);

  // ---- The frame ----
  const f = E.frameOf(ARCHIVE.frame.box);
  const [x, y] = f.project(40.73, -73.99);
  assert('located: the frame is metres east and north of the box\'s centre: a hundredth of a degree of latitude is 1,113 m',
    Math.abs(f.height - 1113.2) < 0.1 && Math.abs(f.width - 1113.2 * Math.cos((40.725 * Math.PI) / 180)) < 0.1 && Math.abs(y - f.height / 2) < 1e-9 && Math.abs(x + f.width / 2) < 1e-9);
  const b = E.boxOf(ARCHIVE);
  assert('located: the box of an archive with no frame holds every placed item and claim, with a margin', b.south < 40.721 && b.north > 40.729 && b.west < -73.989 && b.east > -73.981);

  // ---- The scene ----
  const sc = E.sceneOf(ARCHIVE, APPRAISAL);
  const k1 = sc.claims.find((c) => c.id === 'k1'), k2 = sc.claims.find((c) => c.id === 'k2');
  assert('located: a claim carries its standing, its posterior and its weight in decibans, toned by them; one no evidence bears on has no tone',
    k1.standing === 'established' && Math.abs(k1.db - 10 * Math.log10(24)) < 1e-12 && /^#[0-9a-f]{6}$/.test(k1.tone) && k2.db === null && k2.tone === null && k1.act === 'treat the area' && k2.inspect);
  const items = Object.fromEntries(sc.items.map((i) => [i.id, i]));
  assert('located: an item counts when it is accepted and sound; a proposal and one resting on a holding that is not there do not, each with why',
    items.i1.counts && items.i2.counts && !items.c1.counts && /proposal/.test(items.c1.why) && !items.c2.counts && /the file is not there/.test(items.c2.why));
  assert('located: the kinds are named with a hue each, and the acts likewise', sc.kinds.map((k) => `${k.kind}:${k.count}`).join() === 'complaint:2,inspection:2' && sc.kinds[0].hue !== sc.kinds[1].hue && sc.acts.length === 2);
  assert('located: decibans are ten times the logarithm of the ratio; conclusive ratios are infinite', E.decibans(100) === 20 && E.decibans(0) === -Infinity && E.decibans(Infinity) === Infinity && E.decibans(null) === null);
  assert('located: a tone saturates at ±20 dB, and the pale middle is no evidence either way', E.toneOf(40) === E.toneOf(20) && E.toneOf(-40) === E.toneOf(-20) && E.toneOf(0) !== E.toneOf(10) && E.toneOf(null) === null);

  // ---- The place as markup ----
  const svg = E.svgPlace(sc, { selected: 'c:k1', size: 5 });
  assert('located: the place holds a disc for each claim and a glyph for each item, a claim with nothing bearing on it an outline',
    (svg.match(/class="lo-claim /g) || []).length === 2 && (svg.match(/data-item=/g) || []).length === 4 && /class="lo-claim no-evidence empty"/.test(svg));
  assert('located: an item that says the claim holds is a disc, one that denies it a ring, one that does not count a cross', /<circle class="lo-item holds[^"]*" data-item="i1"/.test(svg) && /<circle class="lo-item denies[^"]*" data-item="i2"/.test(svg) && /<path class="lo-item holds left" data-item="c1"/.test(svg));
  assert('located: a chosen claim is marked and its items lit, the others dimmed', /class="lo-claim established chosen"/.test(svg) && /class="lo-items dim"/.test(svg) && /class="lo-item holds chosen" data-item="i1"/.test(svg));
  assert('located: north is up: a point further north is drawn higher', Number(svg.match(/data-claim="k1" cx="[-\d.]+" cy="([-\d.]+)"/)[1]) < Number(svg.match(/data-claim="k2" cx="[-\d.]+" cy="([-\d.]+)"/)[1]));
  const decide = E.svgPlace(sc, { mode: 'decide' });
  assert('located: in the mode "decide" a claim is toned by its act, and one worth looking at first is ringed', /class="lo-claim no-evidence inspect"/.test(decide) && /lo-claims decide/.test(decide) && /, and look first/.test(decide));
  assert('located: text is escaped in the markup', !E.svgPlace(E.sceneOf({ ...ARCHIVE, claims: [{ ...ARCHIVE.claims[0], text: 'a <b> & c' }] }, null)).includes('<b>'));

  // ---- The place's ways and footprints ----
  const base = E.basePaths({ ways: [['residential', 7, 'A St', [-73.985, 40.725, -73.984, 40.726]], ['primary', 15, 'B Av', [-73.986, 40.721, -73.986, 40.729]]], buildings: [[-73.985, 40.725, -73.9849, 40.725, -73.9849, 40.7251]] }, f);
  assert('located: footprints are one closed path, ways one path a class, the narrowest drawn first', /^M[-\d.]+,[-\d.]+L.*Z$/.test(base.buildings) && base.ways.length === 2 && base.ways[0].cls === 'residential' && E.basePaths(null, f).ways.length === 0);

  // ---- The strip, the scale, words ----
  const strip = E.svgStrip(sc, { selected: 'c:k1', width: 800 });
  assert('located: the strip has a row a kind, a mark an item, and the chosen claim\'s interval as a band', (strip.match(/class="lo-kind"/g) || []).length === 2 && (strip.match(/data-item=/g) || []).length === 4 && /class="lo-window"/.test(strip));
  assert('located: an item over a span is a bar; one at an instant a tick', /class="lo-span holds[^"]*" data-item="c1"/.test(strip) && /class="lo-tick-mark holds[^"]*" data-item="i1"/.test(strip));
  const m = E.timeTicks(Date.UTC(2024, 0, 15) / 1000, Date.UTC(2024, 6, 1) / 1000), d = E.timeTicks(Date.UTC(2024, 3, 1) / 1000, Date.UTC(2024, 3, 6) / 1000);
  assert('located: a long span is ticked by months, a short one by days', m.map((t) => t.words).join() === 'Feb,Mar,Apr,May,Jun,Jul' && d.length === 5 && d[0].words === '2 Apr');
  assert('located: a scale bar is a round length near a fifth of the width', E.scaleBar(1113).metres === 200 && E.scaleBar(1113).words === '200 m' && E.scaleBar(12000).words === '2 km');
  assert('located: a ratio is said in words, either way', E.ratioWords(24) === '24 times as likely if it holds' && E.ratioWords(0.25) === '4 times as likely if it does not' && E.ratioWords(Infinity) === 'conclusive for' && /× 10\^5/.test(E.ratioWords(250000)));
}
