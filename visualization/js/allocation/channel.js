// A round of allocation drawn as a channel (ADR 0136), as markup that needs no page.
//
// The resource enters at the left as a channel as wide as the amount; each stage is a sluice along it that lets water off to the
// plots it serves, and what remains runs on. The plots stand below the channel in the order the round reached them (who first, then
// who), each filled from the bottom by what each stage gave it, in the stage's ink; an excluded plot stands behind a closed gate
// with its reason. The picture is after the acequias, whose rules let water by shares in plenty and by survival in drought (the
// survey, Part IV §3); it is a reading aid, and every amount is written beside its mark.
//
// It takes what the API's allocation engine answers to `run` (api/cpp/science/allocation.hpp) and the round it was asked about.

import { esc, textWidth } from '../history/cone.js';

export const STAGE_INKS = ['#5469b4', '#1c8580', '#b8577a', '#c18434', '#617209', '#7f5f96', '#1f99c7', '#8c654f'];

export function amount(x, unit = '') {
  if (x === null || x === undefined || !Number.isFinite(x)) return '—';
  const a = Math.abs(x);
  const t = a >= 1e9 ? `${(x / 1e9).toPrecision(3)} bn` : a >= 1e6 ? `${(x / 1e6).toPrecision(3)} m` : a >= 1e4 ? Math.round(x).toLocaleString('en-GB') : (+x.toPrecision(3)).toLocaleString('en-GB');
  return unit ? (unit.length <= 2 && !/^[A-Za-z]/.test(unit) ? `${unit}${t}` : `${t} ${unit}`) : t;
}

// The plots in the order reached, the largest `most` shown and the rest gathered into one, the excluded last.
export function plots(round, result, most = 24) {
  const byId = new Map(result.entities.map((e) => [e.id, e]));
  const ordered = result.order.map((o) => ({ ...byId.get(o.entity), stage: o.stage }));
  const shown = [], rest = { id: '…', name: '', amount: 0, by_stage: (result.stages || []).map(() => 0), excluded: null, count: 0, gathered: true };
  const reached = ordered.filter((p) => !p.excluded), excluded = ordered.filter((p) => p.excluded);
  reached.forEach((p, k) => {
    if (k < most) shown.push(p);
    else {
      rest.count++;
      rest.amount += p.amount;
      p.by_stage.forEach((v, j) => { rest.by_stage[j] += v; });
    }
  });
  if (rest.count) shown.push({ ...rest, name: `and ${rest.count} others` });
  const exShown = excluded.slice(0, Math.max(0, Math.min(6, most - shown.length + 6)));
  if (excluded.length > exShown.length) exShown.push({ id: '⋯', name: `and ${excluded.length - exShown.length} others excluded`, amount: 0, by_stage: [], excluded: 'excluded', gathered: true });
  return [...shown, ...exShown];
}

export function layoutChannel(round, result, options = {}) {
  const stages = round.stages || [];
  const room = options.width ?? null, left = 150, gap = 8, top = 64, channelMax = options.channel ?? 34, plotH = options.plotH ?? 150;
  // In a room of a given width, as many plots as fit at a readable width; else the number asked at 46 px each.
  const most = room ? Math.max(4, Math.min(options.most ?? 24, Math.floor((room - left - 40) / (26 + gap)) - 1)) : (options.most ?? 24);
  const P = plots(round, result, most);
  const plotW = room ? Math.max(22, Math.min(options.plotW ?? 46, (room - left - 40) / P.length - gap)) : (options.plotW ?? 46);
  const width = room ? Math.max(room, left + stages.length * 170 + 40) : left + Math.max(stages.length * 150, P.length * (plotW + gap)) + 40;
  const total = round.amount || 1;
  const sluiceX = (k) => left + 70 + k * ((width - left - 140) / Math.max(1, stages.length));
  const widths = [round.amount, ...result.stages.map((s) => s.remaining)].map((v) => Math.max(1.5, channelMax * Math.sqrt(Math.max(0, v) / total)));
  const plotX = (i) => left + i * (plotW + gap);
  const ground = top + channelMax + (plotW < 44 ? 118 : 70);   // turned amounts need room beneath the sluices' words
  // The scale is the largest plot shown alone; a gathered plot is drawn to the top with a break, its amount in words.
  const tallest = Math.max(1e-12, ...P.filter((p) => !p.gathered).map((p) => p.amount));
  return { P, stages, plotW, left, top, channelMax, plotH, width, height: ground + plotH + 96, sluiceX, widths, plotX, ground, tallest };
}

export function svgChannel(round, result, options = {}) {
  const L = layoutChannel(round, result, options);
  const unit = round.unit || '';
  const parts = [];
  const cy = L.top + L.channelMax / 2;
  // The channel, narrowing at each sluice.
  let x0 = 12;
  L.widths.forEach((w, k) => {
    const x1 = k < L.stages.length ? L.sluiceX(k) : L.width - 12;
    parts.push(`<rect class="water" x="${x0.toFixed(1)}" y="${(cy - w / 2).toFixed(1)}" width="${(x1 - x0).toFixed(1)}" height="${w.toFixed(1)}"/>`);
    x0 = x1;
  });
  parts.push(`<text class="head" x="12" y="${L.top - 30}">${esc(round.name || 'The round')}</text>`);
  parts.push(`<text class="amount" x="12" y="${L.top - 12}">${esc(amount(round.amount, unit))} to allocate</text>`);
  // The sluices, each with its principle, what it gave and what remained (their words drawn last, over the runnels).
  const labels = [];
  L.stages.forEach((s, k) => {
    const x = L.sluiceX(k), st = result.stages[k] || {};
    parts.push(`<line class="gate" x1="${x}" y1="${L.top - 4}" x2="${x}" y2="${L.top + L.channelMax + 4}" style="stroke:${STAGE_INKS[k % STAGE_INKS.length]}"/>`);
    labels.push(`<g class="sluice" data-stage="${k}"><text class="stage" x="${x + 5}" y="${L.top + L.channelMax + 18}">${k + 1}. ${esc(s.kind)}</text>` +
      `<text class="principle" x="${x + 5}" y="${L.top + L.channelMax + 32}">for ${esc(s.principle || 'no principle named')}</text>` +
      `<text class="given" x="${x + 5}" y="${L.top + L.channelMax + 46}">gave ${esc(amount(st.given, unit))}</text></g>`);
  });
  parts.push(`<text class="amount" x="${L.width - 12}" y="${L.top - 12}" text-anchor="end">${esc(amount(result.left, unit))} left</text>`);
  // The plots, in the order reached.
  L.P.forEach((p, i) => {
    const x = L.plotX(i), base = L.ground + L.plotH;
    const cls = `plot${p.excluded ? ' excluded' : ''}${p.gathered ? ' gathered' : ''}${options.selected === p.id ? ' chosen' : ''}`;
    let g = `<g class="${cls}" data-entity="${esc(p.id)}"><rect class="bed" x="${x}" y="${L.ground}" width="${L.plotW}" height="${L.plotH}"/>`;
    if (p.excluded) {
      g += `<line class="closed" x1="${x}" y1="${L.ground}" x2="${x + L.plotW}" y2="${L.ground}"/><text class="excluded-word" x="${x + L.plotW / 2}" y="${L.ground + L.plotH / 2}" text-anchor="middle">excluded</text>`;
      g += `<title>${esc(p.name)}: ${esc(p.excluded)}</title>`;
    } else {
      let y = base;
      const scale = p.gathered && p.amount > L.tallest ? p.amount : L.tallest;
      (p.by_stage || []).forEach((v, k) => {
        if (!(v > 0)) return;
        const h = (L.plotH * v) / scale;
        y -= h;
        g += `<rect class="fill" x="${x + 3}" y="${y.toFixed(1)}" width="${L.plotW - 6}" height="${Math.max(0.6, h).toFixed(1)}" style="fill:${STAGE_INKS[k % STAGE_INKS.length]}"><title>stage ${k + 1}: ${esc(amount(v, unit))}</title></rect>`;
      });
      if (p.gathered && p.amount > L.tallest) g += `<path class="break" d="M${x - 2},${L.ground + 18} l${L.plotW / 2 + 2},-6 l${L.plotW / 2 + 2},6"/>`;
      g += L.plotW < 44
        ? `<text class="value" transform="translate(${(x + L.plotW / 2 + 3).toFixed(1)},${(L.ground - 4).toFixed(1)}) rotate(-90)">${esc(amount(p.amount, unit))}</text>`
        : `<text class="value" x="${x + L.plotW / 2}" y="${(L.ground - 6).toFixed(1)}" text-anchor="middle">${esc(amount(p.amount, unit))}</text>`;
      if (!p.gathered) {
        const i0 = L.stages.length ? Math.min(L.stages.length - 1, p.stage ?? 0) : 0;
        const sx = L.stages.length ? L.sluiceX(i0) : x;
        g += `<path class="runnel" d="M${(sx - 3).toFixed(1)},${(L.top + L.channelMax + 4).toFixed(1)} C${(sx - 3).toFixed(1)},${(L.ground - 20).toFixed(1)} ${(x + L.plotW / 2).toFixed(1)},${(L.ground - 44).toFixed(1)} ${(x + L.plotW / 2).toFixed(1)},${(L.ground - 18).toFixed(1)}" style="stroke:${STAGE_INKS[i0 % STAGE_INKS.length]}"/>`;
      }
    }
    const name = p.name || p.id;
    const short = textWidth(name, 11) > L.plotH ? name.slice(0, Math.max(4, Math.floor(name.length * L.plotH / textWidth(name, 11)) - 1)) + '…' : name;
    g += `<text class="name" transform="translate(${x + L.plotW / 2 + 4},${base + 8}) rotate(60)">${esc(short)}</text>`;
    g += `<text class="rank" x="${x + L.plotW / 2}" y="${base - 4}" text-anchor="middle">${p.gathered ? '' : i + 1}</text></g>`;
    parts.push(g);
  });
  parts.push(...labels);
  parts.push(`<text class="legend" x="12" y="${L.ground + 12}">who first,</text><text class="legend" x="12" y="${L.ground + 26}">then who →</text>`);
  return `<svg class="allocation-channel" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${L.width} ${L.height}" width="${L.width}" height="${L.height}" role="img" aria-label="${esc(round.name || 'a round')}: what each stage gave and to whom">${parts.join('')}</svg>`;
}
