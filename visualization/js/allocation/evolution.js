// Resources of many kinds allocated by evolution, drawn (ADR 0137), as markup that needs no page. It takes what the API's
// allocation engine answers to `evolve.search` and `evolve.compose` (api/cpp/science/evolve.hpp) and asks no server.
//
// - The field of the kept: each allocation a stone at its total value (along) and the least outcome of any recipient (up), so that
//   what serves the most and what serves the least served are seen apart; never one score. The first by the tiers is ringed.
// - An allocation as its recipients on a plane (their places, or a ring): a disc for what divides (its area as the amount), pips for
//   units, a flag for an item with an arrow for a direction chosen on the circle, a tick for a duty; the order of grants as a thread,
//   each stop numbered; a recipient not granted hollow and named so.
// - The commutation of grants: a cell filled where two grants commuted at every state tried, crossed where a test found a witness, a
//   corner mark where footprints declared them independent.
// - A resource type's verification as seals: a law that holds a whole seal, one that fails broken, one undecided open; each with its
//   word and how it was established.
// - Strengths from pairwise judgements, people's and a model's apart, each labelled with who judged and its grade.

import { esc, textWidth } from '../history/cone.js';

export const STANDING_MARKS = { verified: '◆', computed: '●', estimated: '◐', proposed: '○' };
export const VERDICT_WORDS = { holds: 'holds', fails: 'fails', undecided: 'undecided' };
const fmt = (v, d = 2) => (v === null || v === undefined || !Number.isFinite(+v) ? '—' : Math.abs(v) >= 1000 && d <= 2 ? Math.round(v).toLocaleString('en-GB') : (+v).toFixed(d));
// A tick's number with exactly d decimals.
const tick = (v, d) => (Number.isFinite(+v) ? (+v).toLocaleString('en-GB', { minimumFractionDigits: d, maximumFractionDigits: d }) : '—');
// Decimals enough to tell the ends of a span apart.
const decimals = (lo, hi) => Math.max(0, Math.min(6, Math.ceil(-Math.log10(Math.max(1e-9, Math.abs(hi - lo)))) + 1));

// The value an allocation's metric reports, as the requester meant it: a maximised metric is stored negated.
export function metricOf(allocation, name) {
  const m = (allocation.metrics || []).find((x) => x.name === name) || (allocation.metrics || []).find((x) => x.role === 'objective');
  return m || null;
}

// The field's points: x the total value (the objective's negation, maximised), y the least outcome among the recipients.
export function layoutField(result, { metric, width = 540, height = 300, pad = 46 } = {}) {
  const kept = result.kept || [];
  const pts = kept.map((a, k) => {
    const m = metricOf(a, metric);
    const total = m ? -m.value : 0;
    const least = m && m.units?.length ? Math.min(...m.units) : 0;
    return { k, total, least, lawful: a.lawful, first: k === 0 };
  });
  const xs = pts.map((p) => p.total), ys = pts.map((p) => p.least);
  const span = (v) => {
    const lo = Math.min(...v), hi = Math.max(...v);
    return lo === hi ? [lo - 1, hi + 1] : [lo - 0.08 * (hi - lo), hi + 0.08 * (hi - lo)];
  };
  const [x0, x1] = pts.length ? span(xs) : [0, 1], [y0, y1] = pts.length ? span(ys) : [0, 1];
  for (const p of pts) {
    p.x = pad + ((p.total - x0) / (x1 - x0)) * (width - 2 * pad);
    p.y = height - pad - ((p.least - y0) / (y1 - y0)) * (height - 2 * pad);
  }
  return { pts, width, height, pad, x0, x1, y0, y1 };
}

export function svgField(result, opts = {}) {
  const L = layoutField(result, opts);
  const { width: W, height: H, pad } = L;
  const parts = [`<text class="head" x="${pad}" y="18">The allocations kept: their total value along, the least any recipient receives up</text>`,
    `<line class="axis" x1="${pad}" y1="${H - pad}" x2="${W - pad}" y2="${H - pad}"/><line class="axis" x1="${pad}" y1="${H - pad}" x2="${pad}" y2="${pad}"/>`,
    `<text class="tick" x="${pad}" y="${H - pad + 16}">${tick(L.x0, decimals(L.x0, L.x1))}</text><text class="tick" x="${W - pad}" y="${H - pad + 16}" text-anchor="end">${tick(L.x1, decimals(L.x0, L.x1))}</text>`,
    `<text class="tick" x="${pad - 6}" y="${H - pad}" text-anchor="end">${tick(L.y0, decimals(L.y0, L.y1))}</text><text class="tick" x="${pad - 6}" y="${pad + 4}" text-anchor="end">${tick(L.y1, decimals(L.y0, L.y1))}</text>`,
    `<text class="label" x="${W / 2}" y="${H - 8}" text-anchor="middle">total value</text>`,
    `<text class="label" x="14" y="${H / 2}" transform="rotate(-90 14 ${H / 2})" text-anchor="middle">the least a recipient receives</text>`];
  for (const p of L.pts) {
    const chosen = opts.selected === p.k;
    parts.push(`<g class="stone${p.lawful ? ' lawful' : ' unlawful'}${p.first ? ' first' : ''}${chosen ? ' chosen' : ''}" data-allocation="${p.k}">` +
      (p.first ? `<circle class="ring" cx="${p.x}" cy="${p.y}" r="12"/>` : '') +
      `<circle class="body" cx="${p.x}" cy="${p.y}" r="7"/><text class="num" x="${p.x}" y="${p.y + 3.5}" text-anchor="middle">${p.k + 1}</text>` +
      `<title>allocation ${p.k + 1}: total ${fmt(p.total)}, the least ${fmt(p.least)}${p.lawful ? ', lawful' : ', not lawful'}${p.first ? ', first by the tiers of evidence' : ''}</title></g>`);
  }
  return `<svg class="allocation-evolution field" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="the allocations kept">${parts.join('')}</svg>`;
}

// Recipients' positions: from places {id: [lat, lon]} when every recipient has one, else on a ring in the problem's order.
export function placeRecipients(problem, { width = 640, height = 420, pad = 56, places = null } = {}) {
  const R = problem.recipients || [];
  const have = places && R.every((r) => places[r.id]);
  const pos = {};
  if (have) {
    const lats = R.map((r) => places[r.id][0]), lons = R.map((r) => places[r.id][1]);
    const la0 = Math.min(...lats), la1 = Math.max(...lats), lo0 = Math.min(...lons), lo1 = Math.max(...lons);
    const k = Math.cos(((la0 + la1) / 2) * Math.PI / 180);
    const sx = (width - 2 * pad) / Math.max(1e-9, (lo1 - lo0) * k), sy = (height - 2 * pad) / Math.max(1e-9, la1 - la0), s = Math.min(sx, sy);
    for (const r of R) pos[r.id] = [pad + (places[r.id][1] - lo0) * k * s, height - pad - (places[r.id][0] - la0) * s];
    const across = Math.max(70, Math.min(150, Math.max(...R.map((r) => textWidth(r.name || r.id, 11))) * 0.9));
    spread(pos, { width, height, pad, gap: across, ratio: 0.42, foot: 44 });
  } else {
    R.forEach((r, i) => {
      const t = (2 * Math.PI * i) / Math.max(1, R.length) - Math.PI / 2;
      pos[r.id] = [width / 2 + (width / 2 - pad) * Math.cos(t), height / 2 + (height / 2 - pad) * Math.sin(t)];
    });
  }
  return pos;
}

// Moves crowded points apart until every pair stands at least `gap` apart (or the passes run out), each step shared between the two,
// and keeps every point inside the frame: a label beneath one does not then run into another's.
// The distance is measured in an ellipse, `gap` across and `gap · ratio` up and down, since a name beneath a point needs more room
// across; `foot` keeps the frame's bottom clear for the names and a legend.
export function spread(pos, { width, height, pad, gap, ratio = 1, foot = 0, passes = 400 }) {
  const ids = Object.keys(pos);
  for (let t = 0; t < passes; t++) {
    let moved = false;
    for (let a = 0; a < ids.length; a++)
      for (let b = a + 1; b < ids.length; b++) {
        const p = pos[ids[a]], q = pos[ids[b]];
        let dx = q[0] - p[0], dy = (q[1] - p[1]) / ratio;
        let d = Math.hypot(dx, dy);
        if (d >= gap) continue;
        if (d < 1e-9) { dx = 1; dy = 0.3; d = Math.hypot(dx, dy); }
        const push = (gap - d) / 2 / d;
        p[0] -= dx * push; p[1] -= dy * push * ratio; q[0] += dx * push; q[1] += dy * push * ratio;
        moved = true;
      }
    for (const id of ids) {
      pos[id][0] = Math.min(width - pad, Math.max(pad, pos[id][0]));
      pos[id][1] = Math.min(height - pad - foot, Math.max(pad, pos[id][1]));
    }
    if (!moved) break;
  }
  return pos;
}

export function svgAllocation(problem, allocation, opts = {}) {
  const W = opts.width ?? 640, H = opts.height ?? 420;
  const pos = placeRecipients(problem, { ...opts, width: W, height: H });
  const R = problem.recipients || [];
  const res = problem.resources || [];
  const divisible = res.filter((r) => ['shares', 'claims', 'fixed'].includes(r.carrier));
  const units = res.filter((r) => ['units', 'unit_claims'].includes(r.carrier));
  const items = res.filter((r) => r.carrier === 'items');
  const circle = (problem.attributes || []).find((a) => a.space === 'circle');
  const most = {};
  for (const r of divisible) most[r.name] = Math.max(1e-12, ...((allocation.amounts || {})[r.name] || [0]));
  const granted = allocation.granted || [];
  const order = Object.fromEntries(granted.map((id, k) => [id, k + 1]));
  const parts = [`<text class="head" x="16" y="18">An allocation: what each recipient received, and the order of grants</text>`];
  // The thread of grants.
  const stops = granted.map((id) => pos[id]).filter(Boolean);
  if (stops.length > 1) parts.push(`<polyline class="thread" points="${stops.map((p) => p.join(',')).join(' ')}"/>`);
  R.forEach((r, i) => {
    const [x, y] = pos[r.id];
    const isGranted = order[r.id] !== undefined;
    const g = [`<g class="recipient${isGranted ? ' granted' : ' unmet'}" data-recipient="${esc(r.id)}">`];
    divisible.forEach((q, j) => {
      const v = ((allocation.amounts || {})[q.name] || [])[i] || 0;
      const rad = 4 + 18 * Math.sqrt(v / most[q.name]);
      g.push(`<circle class="disc ink${j % 4}" cx="${x}" cy="${y}" r="${v > 0 ? rad : 4}"><title>${esc(r.name)}: ${esc(q.name)} ${fmt(v)}</title></circle>`);
    });
    if (!divisible.length) g.push(`<circle class="disc ink0" cx="${x}" cy="${y}" r="6"/>`);
    let px = x + 22;
    units.forEach((q, j) => {
      const v = Math.round(((allocation.amounts || {})[q.name] || [])[i] || 0);
      for (let u = 0; u < v; u++) g.push(`<rect class="pip unit${j % 3}" x="${px + u * 7}" y="${y - 14 + 9 * j}" width="5" height="5"><title>${esc(q.name)}: ${v}</title></rect>`);
    });
    items.forEach((q) => {
      const own = (allocation.owners || {})[q.name] || {};
      Object.entries(own).filter(([, who]) => who === r.id).forEach(([item], m) => {
        const fx = x - 8 - 14 * m, fy = y - 22;
        g.push(`<line class="pole" x1="${fx}" y1="${fy}" x2="${fx}" y2="${fy - 18}"/><path class="flag" d="M${fx},${fy - 18} l10,4 l-10,4 z"><title>${esc(item)}</title></path>`);
        if (circle) {
          const d = ((allocation.attributes || {})[circle.name] || [])[i];
          if (Array.isArray(d)) g.push(`<line class="direction" x1="${fx}" y1="${fy - 9}" x2="${fx + 14 * d[0]}" y2="${fy - 9 - 14 * d[1]}"><title>${esc(circle.name)} (${fmt(d[0], 3)}, ${fmt(d[1], 3)})</title></line>`);
        }
      });
    });
    g.push(`<text class="name" x="${x}" y="${y + 30}" text-anchor="middle">${esc(r.name)}</text>`);
    g.push(isGranted ? `<text class="stop" x="${x}" y="${y + 4}" text-anchor="middle">${order[r.id]}</text>` : `<text class="unmet-word" x="${x}" y="${y + 42}" text-anchor="middle">not granted</text>`);
    g.push('</g>');
    parts.push(g.join(''));
  });
  const legend = [...divisible.map((q, j) => `<tspan class="ink${j % 4}">●</tspan> ${esc(q.name)}`), ...units.map((q, j) => `<tspan class="unit${j % 3}">■</tspan> ${esc(q.name)}`), ...items.map((q) => `⚑ ${esc(q.name)}`)];
  parts.push(`<text class="legend" x="16" y="${H - 24}">${legend.join(' · ')}</text><text class="legend" x="16" y="${H - 10}">${circle ? `the arrow: ${esc(circle.name)} · ` : ''}numbers: the order of grants</text>`);
  return `<svg class="allocation-evolution allocation" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="an allocation">${parts.join('')}</svg>`;
}

export function svgCommutation(result, names, opts = {}) {
  const C = result.order?.commute || [], D = result.order?.declared || [];
  const n = C.length, cell = opts.cell ?? 22;
  const left = Math.max(80, ...names.map((s) => textWidth(s, 11) + 12)), top = 32 + Math.max(60, ...names.map((s) => textWidth(s, 11) * 0.72));
  const slant = Math.max(0, ...names.map((s) => textWidth(s, 11) * 0.72));
  const W = left + n * cell + slant + 16, H = top + n * cell + 34;
  const parts = [`<text class="head" x="8" y="18">Which grants commute (tried at ${result.order?.states ?? 0} states)</text>`];
  names.forEach((s, i) => {
    parts.push(`<text class="name" x="${left - 6}" y="${top + i * cell + cell * 0.65}" text-anchor="end">${esc(s)}</text>`);
    parts.push(`<text class="name" transform="translate(${left + i * cell + cell * 0.6},${top - 6}) rotate(-45)">${esc(s)}</text>`);
  });
  for (let i = 0; i < n; i++)
    for (let j = 0; j < n; j++) {
      const x = left + j * cell, y = top + i * cell;
      if (i === j) { parts.push(`<rect class="self" x="${x}" y="${y}" width="${cell}" height="${cell}"/>`); continue; }
      const c = C[i][j];
      parts.push(`<g class="pair ${c ? 'commute' : 'not'}"><rect class="cell" x="${x}" y="${y}" width="${cell}" height="${cell}"/>` +
        (c ? '' : `<path class="cross" d="M${x + 5},${y + 5} L${x + cell - 5},${y + cell - 5} M${x + cell - 5},${y + 5} L${x + 5},${y + cell - 5}"/>`) +
        (D[i]?.[j] ? `<path class="declared" d="M${x},${y} l7,0 l-7,7 z"/>` : '') + `<title>${esc(names[i])} and ${esc(names[j])}: ${c ? 'commute at every state tried' : 'a test found they do not commute'}${D[i]?.[j] ? '; declared independent' : ''}</title></g>`);
    }
  parts.push(`<text class="legend" x="8" y="${H - 10}">filled: they commute · crossed: they do not · corner: declared independent by footprints</text>`);
  return `<svg class="allocation-evolution commutation" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="which grants commute">${parts.join('')}</svg>`;
}

export function svgSeals(types, opts = {}) {
  const W = opts.width ?? 640, row = 22;
  const methods = (t) => {
    const by = {};
    for (const e of t.entries || []) by[e.method] = (by[e.method] || 0) + 1;
    return Object.entries(by).map(([m, n]) => `${n} ${m}`).join(', ');
  };
  const lines = types.flatMap((t) => [{ head: t }, { seals: t }, ...(t.entries || []).filter((e) => e.status !== 'holds').map((e) => ({ e })),
    ...(t.entries || []).every((e) => e.status === 'holds') ? [{ said: `every law holds: ${methods(t)}` }] : []]);
  const H = 30 + lines.length * row + 10;
  const parts = [`<text class="head" x="8" y="18">The resource types, verified</text>`];
  lines.forEach((l, k) => {
    const y = 30 + k * row;
    if (l.head) {
      parts.push(`<text class="type" x="8" y="${y + 14}">${esc(l.head.type)}: ${l.head.lawful ? 'lawful' : 'not lawful'}, ${l.head.holds} holding, ${l.head.undecided} undecided, ${l.head.fails} failing</text>`);
      return;
    }
    if (l.seals) {
      (l.seals.entries || []).forEach((e, k) => {
        const cx = 20 + k * 16, cy = y + 10;
        const mark = e.status === 'holds' ? `<circle class="seal holds" cx="${cx}" cy="${cy}" r="6"/>` : e.status === 'fails' ? `<path class="seal fails" d="M${cx - 6},${cy} a6,6 0 0 1 12,0 z M${cx - 4},${cy + 2} a6,6 0 0 0 10,1 z"/>` : `<circle class="seal open" cx="${cx}" cy="${cy}" r="6"/>`;
        parts.push(`<g class="law ${esc(e.status)}">${mark}<title>${esc(e.law || e.claim)}: ${esc(VERDICT_WORDS[e.status] || e.status)}, ${esc(e.method)}. ${esc(e.reason)}</title></g>`);
      });
      return;
    }
    if (l.said) {
      parts.push(`<text class="law-text said" x="34" y="${y + 14}">${esc(l.said)}</text>`);
      return;
    }
    const s = l.e.status, cx = 20, cy = y + 10;
    const mark = s === 'holds' ? `<circle class="seal holds" cx="${cx}" cy="${cy}" r="6"/>` : s === 'fails' ? `<path class="seal fails" d="M${cx - 6},${cy} a6,6 0 0 1 12,0 z M${cx - 4},${cy + 2} a6,6 0 0 0 10,1 z"/>` : `<circle class="seal open" cx="${cx}" cy="${cy}" r="6"/>`;
    const text = `${l.e.law || l.e.claim} — ${VERDICT_WORDS[s] || s}, ${l.e.method}`;
    parts.push(`<g class="law ${esc(s)}">${mark}<text class="law-text" x="34" y="${cy + 4}">${esc(textWidth(text, 11) > W - 44 ? text.slice(0, Math.floor(text.length * (W - 44) / textWidth(text, 11)) - 1) + '…' : text)}</text><title>${esc(l.e.claim)}: ${esc(l.e.reason)}</title></g>`);
  });
  return `<svg class="allocation-evolution seals" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="the resource types' laws">${parts.join('')}</svg>`;
}

// sets: [{label, strength: [per allocation], grade}]; the allocations labelled 1…n.
export function svgStrengths(sets, opts = {}) {
  const n = Math.max(0, ...sets.map((s) => s.strength.length));
  const bar = 14, gap = 6, left = 120, W = opts.width ?? 520;
  const H = 30 + sets.length * (n * (bar + 2) + gap + 18) + 10;
  const parts = [`<text class="head" x="8" y="18">Pairwise judgements, read as Bradley–Terry strengths (never summed into the search)</text>`];
  let y = 30;
  for (const s of sets) {
    const most = Math.max(1e-12, ...s.strength);
    parts.push(`<text class="who" x="8" y="${y + 12}">${esc(s.label)}${s.grade ? ` (grade ${esc(s.grade)})` : ''}</text>`);
    y += 18;
    s.strength.forEach((v, k) => {
      const w = (W - left - 30) * (v / most);
      parts.push(`<g class="strength"><text class="num" x="${left - 8}" y="${y + bar - 3}" text-anchor="end">allocation ${k + 1}</text><rect class="bar${s.grade === 'C' ? ' proposed' : ''}" x="${left}" y="${y}" width="${Math.max(1, w)}" height="${bar}"/><text class="value" x="${left + Math.max(1, w) + 4}" y="${y + bar - 3}">${fmt(v, 2)}</text></g>`);
      y += bar + 2;
    });
    y += gap;
  }
  return `<svg class="allocation-evolution strengths" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="strengths from pairwise judgements">${parts.join('')}</svg>`;
}
