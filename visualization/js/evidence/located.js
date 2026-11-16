// Evidence in space and time drawn (ADR 0123): the place as the page, and the items of an archive and the claims they bear on laid
// on it where they are; beneath, a strip of time.
//
// It takes an archive of evidence (noetarium.evidence/1) and what the API's engine answered of it (api/cpp/science/
// spacetime_evidence.hpp: what is kept, each claim weighed, the decision), and, when given, the place's ways and footprints.
//
// - A claim is a disc of its own radius, toned by the weight of its evidence in decibans (ten times the logarithm of its likelihood
//   ratio), on the diverging map lavender-rust: lavender where the evidence tells against it, rust where it tells for it, the pale
//   middle where it tells nothing; a claim no evidence bears on is an outline only. Its standing is always said in words as well.
//   In the mode "decide" a claim is toned by the act chosen for it, and a claim worth looking at first is ringed.
// - An item is a glyph at its place, its hue its kind (named in a legend), filled when it says the claim holds and a ring when it
//   denies it; one that does not count (a proposal, rejected, or resting on a holding that is not sound) is a small cross.
// - The strip lays the items along time, a row a kind, with a claim's interval as a band.
//
// frameOf, sceneOf and decibans are pure arithmetic; svgPlace and svgStrip return markup (evidence.css gives it a look).

import { esc } from '../history/cone.js';
import { colourMap } from '../../styles/palettes.js';
import { cssColour } from '../../styles/colour.js';

const M = 111320;   // metres in a degree of latitude, near enough for a page (the engine's distances are geodesy's)

// A local frame about a box: metres east and north of its centre.
export function frameOf(box) {
  const lat0 = (box.south + box.north) / 2, lon0 = (box.west + box.east) / 2, c = Math.cos((lat0 * Math.PI) / 180);
  const project = (lat, lon) => [(lon - lon0) * M * c, (lat - lat0) * M];
  const [w, s] = project(box.south, box.west), [e, n] = project(box.north, box.east);
  return { lat0, lon0, project, west: w, south: s, east: e, north: n, width: e - w, height: n - s };
}

// The box that holds every placed item and claim, with a margin of a tenth.
export function boxOf(archive) {
  const pts = [...(archive.items || []), ...(archive.claims || [])].filter((x) => Number.isFinite(x.lat) && Number.isFinite(x.lon));
  if (!pts.length) return null;
  let south = Infinity, north = -Infinity, west = Infinity, east = -Infinity;
  for (const p of pts) south = Math.min(south, p.lat), north = Math.max(north, p.lat), west = Math.min(west, p.lon), east = Math.max(east, p.lon);
  const dl = Math.max(north - south, 0.002) * 0.1, dn = Math.max(east - west, 0.002) * 0.1;
  return { south: south - dl, north: north + dl, west: west - dn, east: east + dn };
}

// The interval a written time covers, in seconds since 1970 (UTC), as the API's science.spacetime_evidence reads it: a year, a year
// and month, a date (each the whole of it), or an instant (ISO 8601; without a zone, UTC). A number is an instant already.
export function spanOf(text) {
  if (text === null || text === undefined || text === '') return null;
  if (typeof text === 'number') return Number.isFinite(text) ? [text, text] : null;
  const s = String(text).trim();
  const m = s.match(/^(\d{4})(?:-(\d{2})(?:-(\d{2}))?)?$/);
  if (m) {
    const y = Number(m[1]), mo = m[2] ? Number(m[2]) - 1 : null;
    const a = m[3] ? Date.UTC(y, mo, Number(m[3])) : mo !== null ? Date.UTC(y, mo, 1) : Date.UTC(y, 0, 1);
    const b = m[3] ? a + 86400000 : mo !== null ? Date.UTC(y, mo + 1, 1) : Date.UTC(y + 1, 0, 1);
    return [a / 1000, b / 1000];
  }
  const t = Date.parse(/(z|[+-]\d{2}:?\d{2})$/i.test(s) ? s : `${s.replace(' ', 'T')}Z`);
  return Number.isFinite(t) ? [t / 1000, t / 1000] : null;
}

// From the beginning of `start` to the end of `end`, or of `start` when no end is written.
export function intervalOf(start, end) {
  const a = spanOf(start), b = spanOf(end);
  if (!a && !b) return [null, null];
  return [(a || b)[0], (b || a)[1]];
}

// Ten times the logarithm of a likelihood ratio: what a body of evidence weighs, signed.
export const decibans = (ratio) => (ratio === null || ratio === undefined ? null : ratio === 0 ? -Infinity : !Number.isFinite(ratio) ? Infinity : 10 * Math.log10(ratio));

const DIVERGING = colourMap('lavender-rust'), KINDS = colourMap('categorical-apart');

// A claim's tone: its decibans on the diverging map, saturating at ±`most` (20 dB: odds moved a hundredfold).
export function toneOf(db, most = 20) {
  if (db === null || db === undefined || Number.isNaN(db)) return null;
  const t = Math.max(-1, Math.min(1, (Number.isFinite(db) ? db : Math.sign(db) * most) / most));
  return cssColour(DIVERGING.at(t));
}

// The hue of the k-th kind.
export const hueOf = (k) => cssColour(KINDS.colour(k));

export const STANDINGS = {
  established: { glyph: '●', word: 'established' },
  open: { glyph: '◐', word: 'open' },
  refuted: { glyph: '○', word: 'refuted' },
  'no evidence': { glyph: '·', word: 'no evidence' },
};

// What is drawn: the claims and items in metres of the frame, with what the engine said of each.
export function sceneOf(archive, appraisal = null, options = {}) {
  const box = options.box || archive.frame?.box || boxOf(archive);
  if (!box) return null;
  const f = frameOf(box);
  const weighed = new Map((appraisal?.weigh?.claims || []).map((c) => [c.id, c]));
  const decided = new Map((appraisal?.decide?.claims || []).map((c) => [c.claim, c]));
  const kept = new Map((appraisal?.keep?.items || []).map((i) => [i.id, i]));
  const kinds = [...new Set((archive.items || []).map((i) => i.kind || 'item'))].sort();
  const acts = [...new Set((appraisal?.decide?.claims || []).map((c) => c.act))];
  // Which items count for some claim, and which were left out of every claim they were about, with why.
  const counted = new Set(), left = new Map();
  for (const c of weighed.values()) {
    for (const i of c.items || []) counted.add(i.id);
    for (const l of c.left_out || []) left.set(l.id, l.why);
  }
  const claims = (archive.claims || []).filter((c) => Number.isFinite(c.lat) && Number.isFinite(c.lon)).map((c) => {
    const w = weighed.get(c.id) || {}, d = decided.get(c.id) || {};
    const [x, y] = f.project(c.lat, c.lon);
    const db = w.items?.length ? decibans(w.ratio) : null;
    const [from, until] = intervalOf(c.from, c.until);
    return { id: c.id, text: c.text || '', x, y, r: Math.max(1, Number(c.radius) || 0), from, until, ratio: w.ratio ?? null, overstatement: w.overstatement ?? null, standing: w.standing || (w.error ? 'error' : 'no evidence'), posterior: w.posterior ?? null, prior: c.prior ?? w.prior ?? null,
      db, tone: toneOf(db), items: (w.items || []).map((i) => i.id), act: d.act ?? null, inspect: !!d.inspect, evsi: d.evsi ?? null, error: w.error || null };
  });
  const items = (archive.items || []).filter((i) => Number.isFinite(i.lat) && Number.isFinite(i.lon)).map((i) => {
    const [x, y] = f.project(i.lat, i.lon);
    const k = kinds.indexOf(i.kind || 'item');
    const status = i.status || 'accepted';
    const sound = kept.get(i.id)?.sound !== false;
    const counts = status === 'accepted' && sound;
    const [from, until] = intervalOf(i.from, i.until);
    return { id: i.id, title: i.title || i.id, kind: i.kind || 'item', hue: hueOf(k), x, y, r: Math.max(0, Number(i.radius) || 0), says: i.says === 'denies' ? 'denies' : 'holds',
      counts, counted: counted.has(i.id), status, sound, why: counts ? null : status !== 'accepted' ? (status === 'proposed' ? 'a proposal: it counts for nothing until a person accepts it' : 'rejected') : (kept.get(i.id)?.why || left.get(i.id) || 'it is not sound'),
      from, until, source: i.source || '', origin: i.origin || '', holding: i.holding || '' };
  });
  return { box, frame: f, claims, items, kinds: kinds.map((k, n) => ({ kind: k, hue: hueOf(n), count: items.filter((i) => i.kind === k).length })),
    acts: acts.map((a, n) => ({ act: a, hue: hueOf(n + 5), count: claims.filter((c) => c.act === a).length })) };
}

// The place's ways and footprints as paths in the frame: footprints one path, ways one path a class.
export function basePaths(basemap, frame) {
  if (!basemap) return { buildings: '', ways: [] };
  const f = (v) => v.toFixed(1);
  const pathOf = (flat, close) => {
    let d = '';
    for (let i = 0; i + 1 < flat.length; i += 2) {
      const [x, y] = frame.project(flat[i + 1], flat[i]);
      d += `${i ? 'L' : 'M'}${f(x)},${f(-y)}`;
    }
    return close ? `${d}Z` : d;
  };
  const buildings = (basemap.buildings || []).map((r) => pathOf(r, true)).join('');
  const byClass = new Map();
  for (const [cls, width, , flat] of basemap.ways || []) {
    const key = `${cls}|${width}`;
    byClass.set(key, (byClass.get(key) || '') + pathOf(flat, false));
  }
  return { buildings, ways: [...byClass].map(([key, d]) => { const [cls, width] = key.split('|'); return { cls, width: Number(width), d }; }).sort((a, b) => a.width - b.width) };
}

// A glyph's markup: a disc for an item that says the claim holds, a ring for one that denies it, a cross for one that does not count.
function glyph(i, size, chosen) {
  const cls = `lo-item ${i.says}${i.counts ? '' : ' left'}${chosen ? ' chosen' : ''}`;
  const t = `<title>${esc(`${i.title}${i.counts ? '' : ` — ${i.why}`}`)}</title>`;
  if (!i.counts) {
    const s = size * 0.8;
    return `<path class="${cls}" data-item="${esc(i.id)}" d="M${(i.x - s).toFixed(1)},${(-i.y - s).toFixed(1)}L${(i.x + s).toFixed(1)},${(-i.y + s).toFixed(1)}M${(i.x - s).toFixed(1)},${(-i.y + s).toFixed(1)}L${(i.x + s).toFixed(1)},${(-i.y - s).toFixed(1)}" style="stroke:${i.hue}">${t}</path>`;
  }
  return i.says === 'holds'
    ? `<circle class="${cls}" data-item="${esc(i.id)}" cx="${i.x.toFixed(1)}" cy="${(-i.y).toFixed(1)}" r="${size.toFixed(1)}" style="fill:${i.hue}">${t}</circle>`
    : `<circle class="${cls}" data-item="${esc(i.id)}" cx="${i.x.toFixed(1)}" cy="${(-i.y).toFixed(1)}" r="${(size * 0.85).toFixed(1)}" style="stroke:${i.hue}">${t}</circle>`;
}

// The place as markup, in metres (north up), to be scaled by its viewBox. options: mode ("weigh" | "decide"), selected ("c:<id>" or
// "i:<id>"), basemap paths, size (of a glyph, in metres), showItems.
export function svgPlace(scene, options = {}) {
  const { frame } = scene, mode = options.mode || 'weigh', size = options.size ?? Math.max(3, frame.width / 260);
  const chosenClaim = options.selected?.startsWith('c:') ? options.selected.slice(2) : null;
  const chosenItem = options.selected?.startsWith('i:') ? options.selected.slice(2) : null;
  const lit = new Set(chosenClaim ? (scene.claims.find((c) => c.id === chosenClaim)?.items || []) : []);
  const parts = [];
  const base = options.base || { buildings: '', ways: [] };
  parts.push(`<g class="lo-base">${base.buildings ? `<path class="lo-footprints" d="${base.buildings}"/>` : ''}${base.ways.map((w) => `<path class="lo-way ${esc(w.cls)}" d="${w.d}" style="stroke-width:${Math.max(1.5, w.width * 0.8).toFixed(1)}"/>`).join('')}</g>`);
  const actHue = new Map(scene.acts.map((a) => [a.act, a.hue]));
  parts.push(`<g class="lo-claims${mode === 'decide' ? ' decide' : ''}">`);
  for (const c of scene.claims) {
    const tone = mode === 'decide' ? (c.act ? actHue.get(c.act) : null) : c.tone;
    const cls = `lo-claim ${c.standing.replace(/\s+/g, '-')}${tone ? '' : ' empty'}${c.id === chosenClaim ? ' chosen' : ''}${mode === 'decide' && c.inspect ? ' inspect' : ''}`;
    const said = mode === 'decide' && c.act ? `${c.act}${c.inspect ? ', and look first' : ''}` : `${STANDINGS[c.standing]?.word || c.standing}${c.posterior !== null ? `, ${(100 * c.posterior).toFixed(c.posterior > 0.995 || c.posterior < 0.005 ? 1 : 0)}%` : ''}`;
    parts.push(`<circle class="${cls}" data-claim="${esc(c.id)}" cx="${c.x.toFixed(1)}" cy="${(-c.y).toFixed(1)}" r="${c.r.toFixed(1)}"${tone ? ` style="fill:${tone}"` : ''}><title>${esc(`${c.text} — ${said}`)}</title></circle>`);
  }
  parts.push('</g>');
  if (options.showItems !== false) {
    parts.push(`<g class="lo-items${chosenClaim ? ' dim' : ''}">`);
    for (const i of scene.items) parts.push(glyph(i, lit.has(i.id) ? size * 1.35 : size, i.id === chosenItem || lit.has(i.id)));
    parts.push('</g>');
  }
  const pad = frame.width * 0.01;
  return `<svg class="place" viewBox="${(frame.west - pad).toFixed(1)} ${(-frame.north - pad).toFixed(1)} ${(frame.width + 2 * pad).toFixed(1)} ${(frame.height + 2 * pad).toFixed(1)}" preserveAspectRatio="xMidYMid meet" role="img" aria-label="the place, its claims and its items">${parts.join('')}</svg>`;
}

// A scale bar: a round length near a fifth of the width, in metres.
export function scaleBar(widthMetres) {
  const want = widthMetres / 5, mag = 10 ** Math.floor(Math.log10(want));
  const len = [1, 2, 5, 10].map((m) => m * mag).filter((v) => v <= want).pop() || mag;
  return { metres: len, words: len >= 1000 ? `${len / 1000} km` : `${len} m` };
}

// Ticks of a span of seconds since 1970: months when it is long, days when it is short.
export function timeTicks(from, until) {
  const out = [], span = until - from, day = 86400;
  if (!(span > 0)) return out;
  const start = new Date(from * 1000);
  if (span > 70 * day) {
    for (let d = new Date(Date.UTC(start.getUTCFullYear(), start.getUTCMonth() + (start.getUTCDate() > 1 ? 1 : 0), 1)); d.getTime() / 1000 <= until; d = new Date(Date.UTC(d.getUTCFullYear(), d.getUTCMonth() + 1, 1)))
      out.push({ t: d.getTime() / 1000, words: d.toLocaleString('en-GB', { month: 'short', timeZone: 'UTC' }) + (d.getUTCMonth() === 0 ? ` ${d.getUTCFullYear()}` : '') });
  } else {
    const step = span > 20 * day ? 7 : 1;
    for (let d = new Date(Date.UTC(start.getUTCFullYear(), start.getUTCMonth(), start.getUTCDate() + 1)); d.getTime() / 1000 <= until; d = new Date(d.getTime() + step * day * 1000))
      out.push({ t: d.getTime() / 1000, words: `${d.getUTCDate()} ${d.toLocaleString('en-GB', { month: 'short', timeZone: 'UTC' })}` });
  }
  return out;
}

// The strip of time: a row a kind, the items as ticks at their times (a bar over a span), a claim's interval as a band.
export function svgStrip(scene, options = {}) {
  const width = options.width ?? 900, row = 18, left = 120, right = 14, top = 6;
  const timed = scene.items.filter((i) => i.from !== null);
  if (!timed.length) return '';
  let from = Math.min(...timed.map((i) => i.from)), until = Math.max(...timed.map((i) => i.until ?? i.from));
  const claim = options.selected?.startsWith('c:') ? scene.claims.find((c) => c.id === options.selected.slice(2)) || null : null;
  if (claim && Number.isFinite(claim.from) && Number.isFinite(claim.until)) from = Math.min(from, claim.from), until = Math.max(until, claim.until);
  if (until <= from) until = from + 86400;
  const x = (t) => left + ((t - from) / (until - from)) * (width - left - right);
  const H = top + scene.kinds.length * row + 22;
  const parts = [];
  if (claim && Number.isFinite(claim.from)) parts.push(`<rect class="lo-window" x="${x(claim.from).toFixed(1)}" y="${top - 2}" width="${Math.max(2, x(claim.until) - x(claim.from)).toFixed(1)}" height="${scene.kinds.length * row + 4}"/>`);
  for (const tk of timeTicks(from, until)) parts.push(`<line class="lo-grid" x1="${x(tk.t).toFixed(1)}" y1="${top}" x2="${x(tk.t).toFixed(1)}" y2="${H - 18}"/><text class="lo-tick" x="${x(tk.t).toFixed(1)}" y="${H - 5}" text-anchor="middle">${esc(tk.words)}</text>`);
  const lit = new Set(claim ? claim.items : []);
  scene.kinds.forEach((k, n) => {
    const y = top + n * row + row / 2;
    parts.push(`<text class="lo-kind" x="${left - 10}" y="${y + 4}" text-anchor="end">${esc(k.kind)} <tspan class="lo-count">${k.count}</tspan></text>`);
    for (const i of timed.filter((it) => it.kind === k.kind)) {
      const a = x(i.from), b = x(i.until ?? i.from), on = lit.has(i.id);
      parts.push(b - a > 2
        ? `<line class="lo-span ${i.says}${on ? ' lit' : ''}${i.counts ? '' : ' left'}" data-item="${esc(i.id)}" x1="${a.toFixed(1)}" y1="${y}" x2="${b.toFixed(1)}" y2="${y}" style="stroke:${k.hue}"/>`
        : `<line class="lo-tick-mark ${i.says}${on ? ' lit' : ''}${i.counts ? '' : ' left'}" data-item="${esc(i.id)}" x1="${a.toFixed(1)}" y1="${(y - (on ? 7 : 5)).toFixed(1)}" x2="${a.toFixed(1)}" y2="${(y + (on ? 7 : 5)).toFixed(1)}" style="stroke:${k.hue}"/>`);
    }
  });
  return `<svg class="strip" viewBox="0 0 ${width} ${H}" width="${width}" height="${H}" role="img" aria-label="the items along time, a row a kind">${parts.join('')}</svg>`;
}

// A likelihood ratio as it is read: 1 in 10 below one, ×N above.
export function ratioWords(ratio) {
  if (ratio === null || ratio === undefined) return '—';
  if (!Number.isFinite(ratio)) return 'conclusive for';
  if (ratio === 0) return 'conclusive against';
  const r = ratio >= 1 ? ratio : 1 / ratio, n = r >= 1e4 ? r.toExponential(1).replace('e+', ' × 10^') : String(+r.toPrecision(r >= 10 ? 3 : 2));
  return ratio >= 1 ? `${n} times as likely if it holds` : `${n} times as likely if it does not`;
}
