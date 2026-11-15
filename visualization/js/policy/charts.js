// The charts of an appraisal (ADR 0122), as markup that needs no page: a forest plot of a body of evidence; intervals set against
// each other on one axis (the effect there and here; the options' net present values); and the worth of a study over its size.
// They take what the API's policy engine answers (api/cpp/science/policy.hpp). Every mark has its number beside it in words: a
// chart here is never the only place a value is told.

import { esc, textWidth } from '../history/cone.js';

// A number as it is read: four significant figures; thousands grouped from ten thousand.
export function figure(x, digits = 3) {
  if (x === null || x === undefined || !Number.isFinite(x)) return '—';
  const a = Math.abs(x);
  if (a >= 1e4) return Math.round(x).toLocaleString('en-GB');
  if (a !== 0 && a < 1e-6) return x.toExponential(digits - 1);
  return (+x.toPrecision(digits)).toLocaleString('en-GB', { maximumSignificantDigits: digits, useGrouping: false });
}

// Ticks for an axis from lo to hi: at most `most` round values.
export function ticks(lo, hi, most = 6) {
  if (!(hi > lo)) return [lo];
  const raw = (hi - lo) / most, mag = 10 ** Math.floor(Math.log10(raw));
  const step = [1, 2, 2.5, 5, 10].map((m) => m * mag).find((s) => s >= raw) || 10 * mag;
  const out = [];
  for (let v = Math.ceil(lo / step) * step; v <= hi + step * 1e-9; v += step) out.push(+v.toPrecision(12));
  return out;
}

// The rows of a forest plot: each study, then the pooled effect and the interval predicted for a new setting.
export function forestRows(body) {
  const rows = (body.studies || []).map((s) => ({ kind: 'study', id: s.id, label: s.label || s.id, year: s.year, stratum: s.stratum || '', effect: s.effect, lower: s.lower, upper: s.upper, weight: s.weight ?? 0 }));
  if (body.pooled) rows.push({ kind: 'pooled', label: `Pooled, ${body.pooled.model}`, effect: body.pooled.estimate, lower: body.pooled.lower, upper: body.pooled.upper });
  if (body.prediction) rows.push({ kind: 'prediction', label: 'Predicted for a new setting', effect: body.pooled.estimate, lower: body.prediction.lower, upper: body.prediction.upper });
  return rows;
}

// A forest plot. options.ratio: the effects are logarithms of ratios, and the axis and numbers are told as ratios.
export function svgForest(body, options = {}) {
  const rows = forestRows(body);
  if (!rows.length) return '';
  const ratio = !!options.ratio, show = (v) => figure(ratio ? Math.exp(v) : v);
  const labelW = Math.min(230, Math.max(120, ...rows.map((r) => textWidth(r.label, 12.5) + 10))), numW = 150, plotW = options.plotW ?? 330, rowH = 24, top = 26, pad = 10;
  const lo = Math.min(0, ...rows.map((r) => r.lower)), hi = Math.max(0, ...rows.map((r) => r.upper));
  const span = hi - lo || 1, a = lo - span * 0.04, b = hi + span * 0.04;
  const x = (v) => labelW + ((v - a) / (b - a)) * plotW;
  const W = labelW + plotW + numW + pad, H = top + rows.length * rowH + 34;
  const parts = [];
  const tk = ratio ? [0.1, 0.2, 0.5, 1, 2, 5, 10].map(Math.log).filter((v) => v >= a && v <= b) : ticks(a, b, 6);
  for (const v of tk) {
    parts.push(`<line class="grid${Math.abs(v) < 1e-12 ? ' zero' : ''}" x1="${x(v).toFixed(1)}" y1="${top - 6}" x2="${x(v).toFixed(1)}" y2="${H - 26}"/>`);
    parts.push(`<text class="tick" x="${x(v).toFixed(1)}" y="${H - 12}" text-anchor="middle">${show(v)}</text>`);
  }
  if (!tk.some((v) => Math.abs(v) < 1e-12)) parts.push(`<line class="grid zero" x1="${x(0).toFixed(1)}" y1="${top - 6}" x2="${x(0).toFixed(1)}" y2="${H - 26}"/>`);
  parts.push(`<text class="head" x="0" y="12">${esc(options.labelHead || 'Study')}</text><text class="head" x="${W - pad}" y="12" text-anchor="end">${esc(ratio ? 'ratio [interval]' : 'effect [interval]')}</text>`);
  const maxW = Math.max(1e-9, ...rows.filter((r) => r.kind === 'study').map((r) => r.weight));
  rows.forEach((r, k) => {
    const y = top + k * rowH + rowH / 2;
    const cls = `row ${r.kind}${options.selected === r.id ? ' chosen' : ''}`;
    const mark = r.kind === 'study'
      ? `<line class="ci" x1="${x(r.lower).toFixed(1)}" y1="${y}" x2="${x(r.upper).toFixed(1)}" y2="${y}"/><rect class="box" x="${(x(r.effect) - (3 + 5 * Math.sqrt(r.weight / maxW))).toFixed(1)}" y="${(y - (3 + 5 * Math.sqrt(r.weight / maxW))).toFixed(1)}" width="${(6 + 10 * Math.sqrt(r.weight / maxW)).toFixed(1)}" height="${(6 + 10 * Math.sqrt(r.weight / maxW)).toFixed(1)}"/>`
      : r.kind === 'pooled'
        ? `<path class="diamond" d="M${x(r.lower).toFixed(1)},${y} L${x(r.effect).toFixed(1)},${y - 7} L${x(r.upper).toFixed(1)},${y} L${x(r.effect).toFixed(1)},${y + 7} Z"/>`
        : `<line class="predicted" x1="${x(r.lower).toFixed(1)}" y1="${y}" x2="${x(r.upper).toFixed(1)}" y2="${y}"/><line class="predicted" x1="${x(r.lower).toFixed(1)}" y1="${y - 4}" x2="${x(r.lower).toFixed(1)}" y2="${y + 4}"/><line class="predicted" x1="${x(r.upper).toFixed(1)}" y1="${y - 4}" x2="${x(r.upper).toFixed(1)}" y2="${y + 4}"/>`;
    parts.push(`<g class="${cls}"${r.id ? ` data-study="${esc(r.id)}"` : ''}><title>${esc(`${r.label}: ${show(r.effect)} [${show(r.lower)}, ${show(r.upper)}]${r.kind === 'study' ? `, weight ${figure(100 * r.weight)}%` : ''}`)}</title>`
      + `<text class="label" x="0" y="${y + 4}">${esc(r.label.length > 34 ? `${r.label.slice(0, 33)}…` : r.label)}</text>${mark}`
      + `<text class="num" x="${W - pad}" y="${y + 4}" text-anchor="end">${r.kind === 'prediction' ? '' : `${show(r.effect)} `}[${show(r.lower)}, ${show(r.upper)}]</text></g>`);
  });
  return `<svg class="forest" viewBox="0 0 ${W.toFixed(0)} ${H}" width="${W.toFixed(0)}" height="${H}" role="img" aria-label="a forest plot of the studies">${parts.join('')}</svg>`;
}

// Intervals set against each other on one axis, a zero line among them: [{label, value, lower, upper, kind}].
export function svgIntervals(items, options = {}) {
  const list = items.filter((i) => Number.isFinite(i.value));
  if (!list.length) return '';
  const show = options.show || figure;
  const labelW = Math.min(260, Math.max(110, ...list.map((i) => textWidth(i.label, 12.5) + 12))), numW = options.numW ?? 190, plotW = options.plotW ?? 320, rowH = 30, top = 10;
  const lo = Math.min(options.zero ?? 0, ...list.map((i) => i.lower ?? i.value)), hi = Math.max(options.zero ?? 0, ...list.map((i) => i.upper ?? i.value));
  const span = hi - lo || 1, a = lo - span * 0.05, b = hi + span * 0.05;
  const x = (v) => labelW + ((v - a) / (b - a)) * plotW;
  const W = labelW + plotW + numW, H = top + list.length * rowH + 30;
  const parts = [];
  for (const v of ticks(a, b, 5)) {
    parts.push(`<line class="grid${Math.abs(v - (options.zero ?? 0)) < 1e-12 ? ' zero' : ''}" x1="${x(v).toFixed(1)}" y1="${top}" x2="${x(v).toFixed(1)}" y2="${H - 24}"/><text class="tick" x="${x(v).toFixed(1)}" y="${H - 10}" text-anchor="middle">${show(v)}</text>`);
  }
  parts.push(`<line class="grid zero" x1="${x(options.zero ?? 0).toFixed(1)}" y1="${top}" x2="${x(options.zero ?? 0).toFixed(1)}" y2="${H - 24}"/>`);
  list.forEach((i, k) => {
    const y = top + k * rowH + rowH / 2;
    const has = Number.isFinite(i.lower) && Number.isFinite(i.upper);
    parts.push(`<g class="row ${esc(i.kind || '')}"><text class="label" x="0" y="${y + 4}">${esc(i.label)}</text>`
      + (has ? `<line class="ci" x1="${x(i.lower).toFixed(1)}" y1="${y}" x2="${x(i.upper).toFixed(1)}" y2="${y}"/>` : '')
      + `<circle class="dot" cx="${x(i.value).toFixed(1)}" cy="${y}" r="4.5"/>`
      + `<text class="num" x="${W}" y="${y + 4}" text-anchor="end">${show(i.value)}${has ? ` [${show(i.lower)}, ${show(i.upper)}]` : ''}${i.note ? ` ${esc(i.note)}` : ''}</text></g>`);
  });
  return `<svg class="intervals" viewBox="0 0 ${W.toFixed(0)} ${H}" width="${W.toFixed(0)}" height="${H}" role="img" aria-label="${esc(options.label || 'intervals compared')}">${parts.join('')}</svg>`;
}

// The worth of studying first over the size of the study, on a logarithmic axis of sizes, with the worth of deciding now each way
// drawn across it and the best size marked.
export function svgStudy(decision, options = {}) {
  const curve = (decision.curve || []).filter((p) => Number.isFinite(p.worth));
  if (curve.length < 2) return '';
  const W = options.width ?? 620, H = options.height ?? 250, left = 78, right = 16, top = 16, bottom = 40;
  const n0 = Math.log10(curve[0].n), n1 = Math.log10(curve.at(-1).n);
  const now = Math.max(decision.adopt, decision.reject);
  const lo = Math.min(0, now, ...curve.map((p) => p.worth)), hi = Math.max(0, now, ...curve.map((p) => p.worth));
  const span = hi - lo || 1, a = lo - span * 0.08, b = hi + span * 0.1;
  const x = (n) => left + ((Math.log10(n) - n0) / (n1 - n0 || 1)) * (W - left - right);
  const y = (v) => top + (1 - (v - a) / (b - a)) * (H - top - bottom);
  const parts = [];
  for (const v of ticks(a, b, 5)) parts.push(`<line class="grid${v === 0 ? ' zero' : ''}" x1="${left}" y1="${y(v).toFixed(1)}" x2="${W - right}" y2="${y(v).toFixed(1)}"/><text class="tick" x="${left - 8}" y="${(y(v) + 4).toFixed(1)}" text-anchor="end">${figure(v)}</text>`);
  for (let e = Math.ceil(n0); e <= Math.floor(n1); e++) parts.push(`<text class="tick" x="${x(10 ** e).toFixed(1)}" y="${H - 20}" text-anchor="middle">${(10 ** e).toLocaleString('en-GB')}</text><line class="grid" x1="${x(10 ** e).toFixed(1)}" y1="${top}" x2="${x(10 ** e).toFixed(1)}" y2="${H - bottom}"/>`);
  const units = options.units || 'units';
  parts.push(`<text class="axis" x="${((left + W - right) / 2).toFixed(1)}" y="${H - 4}" text-anchor="middle">${esc(units)} studied</text>`);
  parts.push(`<line class="now" x1="${left}" y1="${y(now).toFixed(1)}" x2="${W - right}" y2="${y(now).toFixed(1)}"/><text class="now-label" x="${left + 6}" y="${(y(now) - 5).toFixed(1)}">deciding now: ${figure(now)}</text>`);
  parts.push(`<path class="worth" d="${curve.map((p, k) => `${k ? 'L' : 'M'}${x(p.n).toFixed(1)},${y(p.worth).toFixed(1)}`).join(' ')}"/>`);
  if (decision.n > 1 && Number.isFinite(decision.study)) {
    parts.push(`<line class="best" x1="${x(decision.n).toFixed(1)}" y1="${y(decision.study).toFixed(1)}" x2="${x(decision.n).toFixed(1)}" y2="${H - bottom}"/><circle class="best" cx="${x(decision.n).toFixed(1)}" cy="${y(decision.study).toFixed(1)}" r="4.5"/>`
      + `<text class="best-label" x="${Math.min(W - right - 4, x(decision.n) + 8).toFixed(1)}" y="${(y(decision.study) - 8).toFixed(1)}"${x(decision.n) > W - 170 ? ' text-anchor="end"' : ''}>${decision.n.toLocaleString('en-GB')} ${esc(units)}: ${figure(decision.study)}</text>`);
  }
  return `<svg class="study" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="the worth of studying first, over the size of the study">${parts.join('')}</svg>`;
}

// The expected net present value over the worth of a unit of the outcome, which it is linear in: the line from the lower bound to
// the upper, the worth stated marked on it, and the worth at which the decision switches. `appraised` is an option as the engine
// appraised it at the worth `value` (its benefits there and its costs).
export function worthLine(appraised, value, bounds) {
  const slope = value ? appraised.benefits / value : 0;
  const at = (v) => slope * v - appraised.costs;
  const lo = Math.min(bounds?.[0] ?? value * 0.5, value), hi = Math.max(bounds?.[1] ?? value * 1.5, value);
  return { at, lo, hi, switching: slope > 0 ? appraised.costs / slope : null };
}
export function svgWorth(appraised, value, bounds, options = {}) {
  if (!appraised || !Number.isFinite(appraised.benefits) || !Number.isFinite(value) || !value) return '';
  const { at, lo, hi, switching } = worthLine(appraised, value, bounds);
  if (!(hi > lo)) return '';
  const W = options.width ?? 620, H = options.height ?? 190, left = 78, right = 16, top = 14, bottom = 38;
  const y0 = Math.min(0, at(lo), at(hi)), y1 = Math.max(0, at(lo), at(hi)), span = y1 - y0 || 1, a = y0 - span * 0.1, b = y1 + span * 0.12;
  const x = (v) => left + ((v - lo) / (hi - lo)) * (W - left - right), y = (m) => top + (1 - (m - a) / (b - a)) * (H - top - bottom);
  const parts = [];
  for (const m of ticks(a, b, 4)) parts.push(`<line class="grid${m === 0 ? ' zero' : ''}" x1="${left}" y1="${y(m).toFixed(1)}" x2="${W - right}" y2="${y(m).toFixed(1)}"/><text class="tick" x="${left - 8}" y="${(y(m) + 4).toFixed(1)}" text-anchor="end">${figure(m)}</text>`);
  for (const v of ticks(lo, hi, 6)) parts.push(`<text class="tick" x="${x(v).toFixed(1)}" y="${H - 20}" text-anchor="middle">${figure(v)}</text>`);
  parts.push(`<text class="axis" x="${((left + W - right) / 2).toFixed(1)}" y="${H - 4}" text-anchor="middle">${esc(options.label || 'the worth of a unit of the outcome')}</text>`);
  parts.push(`<line class="grid zero" x1="${left}" y1="${y(0).toFixed(1)}" x2="${W - right}" y2="${y(0).toFixed(1)}"/>`);
  parts.push(`<path class="worth" d="M${x(lo).toFixed(1)},${y(at(lo)).toFixed(1)} L${x(hi).toFixed(1)},${y(at(hi)).toFixed(1)}"/>`);
  if (switching !== null && switching >= lo && switching <= hi) {
    parts.push(`<line class="best" x1="${x(switching).toFixed(1)}" y1="${top}" x2="${x(switching).toFixed(1)}" y2="${H - bottom}"/><text class="best-label" x="${(x(switching) + 6).toFixed(1)}" y="${top + 11}"${x(switching) > W - 190 ? ` text-anchor="end" dx="-12"` : ''}>it switches at ${figure(switching)}</text>`);
  }
  parts.push(`<circle class="best" cx="${x(value).toFixed(1)}" cy="${y(at(value)).toFixed(1)}" r="4.5"/><text class="now-label" x="${x(value).toFixed(1)}" y="${(y(at(value)) - 9).toFixed(1)}" text-anchor="${x(value) > W - 120 ? 'end' : x(value) < left + 60 ? 'start' : 'middle'}">as stated, ${figure(value)}</text>`);
  return `<svg class="study worth-line" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="the expected net present value over the worth of a unit">${parts.join('')}</svg>`;
}

// The three courses and their worths, the chosen marked: [{course, worth, chosen}].
export function courses(decision) {
  const chosen = new Set(decision.chosen || []);
  const out = [{ course: 'adopt', worth: decision.adopt, chosen: chosen.has('adopt') }, { course: 'reject', worth: decision.reject, chosen: chosen.has('reject') }];
  if (Number.isFinite(decision.study)) out.push({ course: 'study first', worth: decision.study, chosen: chosen.has('study first'), n: decision.n });
  return out;
}
