// What something did, where (ADR 0087): marks for estimates at places and for changes that stand from a date, for any work that shows
// an effect on a map: a street change on its counters, a policy on its regions, a lane on its riders. Nothing here names what was
// done or where.
//
// AN ESTIMATE is a signed number at a place, on a scale the caller declares, with or without its standard error. Its colour is the
// visualization layer's validated diverging map `sea-brick`, below the middle for a fall and above it for a rise, taken to its end at
// ±scale. An estimate within two standard errors of zero is drawn paler and smaller: it is there, and says less. An estimate that
// could not be made is drawn as a small grey mark, so that a place without an answer is not mistaken for one with none.
//
// A FOOTPRINT is a line or a point that stands from a period on (a lane built, a building opened). Against the period shown, those
// standing are drawn strong, the ones begun within the last `fresh` periods brightest, and those still to come faint or not at all.
//
// For a logarithm of a ratio, percent(v) reads it as a change: 100 (e^v − 1)%.

import { colourMap } from '../../styles/index.js';

const SIGNED = colourMap('sea-brick');
const toLinear = (rgb) => rgb.map((c) => (c <= 0.04045 ? c / 12.92 : ((c + 0.055) / 1.055) ** 2.4));

// The colour of an estimate v on a scale (sRGB, components in [0, 1]); null where v is not a number.
export function effectColour(v, scale) {
  if (!Number.isFinite(v)) return null;
  const t = Math.max(-1, Math.min(1, v / scale));
  return SIGNED.at(t);
}

// A logarithm of a ratio as a percent change, signed, e.g. "+12%".
export function percent(v, digits = 0) {
  if (!Number.isFinite(v)) return '—';
  const p = 100 * Math.expm1(v);
  return `${p >= 0 ? '+' : '−'}${Math.abs(p).toFixed(digits)}%`;
}

// Whether an estimate stands apart from zero at two standard errors.
export function distinct(v, se) {
  return Number.isFinite(v) && Number.isFinite(se) && se > 0 && Math.abs(v) > 2 * se;
}

// Estimates at places: items with a position (local x, y), drawn as short columns; o = {position, value, se?, scale, base?, radius?,
// height?, pick?, theme?}.
export function addEstimates(gis, id, items, o) {
  const grey = o.theme === 'night' ? [0.62, 0.58, 0.66] : [0.55, 0.52, 0.58];
  const r = o.radius ?? 9, hgt = o.height ?? 26;
  return gis.addPoints(id, items, {
    position: o.position,
    base: o.base,
    radius: (f) => {
      const v = o.value(f), se = o.se ? o.se(f) : NaN;
      if (!Number.isFinite(v)) return r * 0.45;
      return o.se && !distinct(v, se) ? r * 0.7 : r;
    },
    height: (f) => (Number.isFinite(o.value(f)) ? hgt : hgt * 0.4),
    color: (f) => {
      const v = o.value(f), c = effectColour(v, o.scale);
      if (!c) return grey;
      if (o.se && !distinct(v, o.se(f))) return c.map((x, i) => x * 0.55 + (o.theme === 'night' ? 0.32 : 0.45));
      return c;
    },
    pick: o.pick,
  });
}

// Footprints that stand from a period: lines (local x, y, flat) as ribbons, points as low marks; o = {line? | point?, begin, now,
// fresh?, future? ('faint' | 'hidden'), width?, theme?, pick?}. Ribbons take colours in linear light, points in sRGB.
export function addFootprints(gis, id, items, o) {
  const night = o.theme === 'night';
  const strong = night ? [0.98, 0.83, 0.55] : [0.6, 0.39, 0.1];
  const brightest = night ? [1.0, 0.95, 0.8] : [0.82, 0.48, 0.05];
  const faint = night ? [0.5, 0.45, 0.55] : [0.72, 0.68, 0.72];
  const fresh = o.fresh ?? 1;
  const shown = items.filter((f) => o.future !== 'hidden' || o.begin(f) <= o.now);
  const tone = (f) => {
    const b = o.begin(f);
    if (b > o.now) return faint;
    return o.now - b < fresh ? brightest : strong;
  };
  if (o.line) {
    gis.addRibbons(id, shown, { line: o.line, width: (f) => (o.begin(f) <= o.now ? (o.width ?? 5) : (o.width ?? 5) * 0.5), color: (f) => toLinear(tone(f)), z: 0.6,
      opacity: 0.95, order: 4, pick: o.pick });
  } else {
    gis.addPoints(id, shown, { position: o.point, radius: (f) => (o.begin(f) <= o.now ? 14 : 8), height: () => 6, color: tone, pick: o.pick });
  }
  return shown.length;
}
