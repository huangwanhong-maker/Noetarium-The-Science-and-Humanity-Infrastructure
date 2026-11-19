// An undertaking drawn as a round with no head (ADR 0132; the design of 2026-11-19, §4): markup that needs no page.
//
// The participants sit on a ring, grouped by kind, each a glyph by its kind and its name set along the radius, so that no seat is the
// head of the table. The relations are threads across the ring, one layer for each declared type, each layer an ink and a dash; a
// layer is drawn or left out as options.layers says. A tie runs from its source towards its target and ends in a small arrow.
//
// When a participant is chosen, the round is read from its seat: its threads are drawn full and the others faint, and beside every
// other seat, for each layer drawn, a mark says what the record knows of the pair in either direction: a dot when a tie is present, a
// cross when it is absent (recorded absent, or covered by a design of observation that found none), a question in a broken ring when
// it is not observed. A relation not observed is drawn as a gap with a mark, never as nothing (the design, §4).
//
// The undertaking's diagnostics, when given (the engine's answer to `undertaking`), mark their places: a participant without whom the
// relations of authority fall apart is ringed twice; a tie that is the only link of authority is drawn doubled.
//
// svgRound(record, options) takes a noetarium.undertaking/1 and returns one SVG; layoutRound gives the seats and the threads, every
// text with its extent, for checks and for a page that places its own controls. Every seat carries data-participant, every thread
// data-type, data-from, data-to; every mark of a pair data-status; classes (mr-…) let a page restyle them, by night among others.
//
// options: { at: date, layers: [types] (all when absent), selected: id, answer: the engine's answer (for its diagnoses), width,
//            ground, key: true, centre: true }

import { FONT, PALETTE, esc, f1, fit, kindOf, layerLooks, status, holdsOn, STATUS, textWidth } from './common.js';

const SIZE = { name: 10, centre: 12, small: 9.5, key: 10, mark: 8.5 };

export function layoutRound(record, options = {}) {
  const at = options.at || '';
  const participants = [...(record.participants || [])];
  const order = [...new Set(participants.map((p) => p.kind || ''))];
  participants.sort((a, b) => order.indexOf(a.kind || '') - order.indexOf(b.kind || '') || String(a.name || a.id).localeCompare(String(b.name || b.id)));
  const n = participants.length;
  const label = Math.min(190, Math.max(90, ...participants.map((p) => textWidth(p.name || p.id, SIZE.name) + 18)));
  const R = Math.max(130, (n * 15) / (2 * Math.PI));
  const width = options.width || 2 * (R + label + 24);
  const cx = width / 2, cy = R + label + 24;
  // sectors by kind: a gap of one seat between kinds
  const groups = order.length > 1 ? order.length : 0;
  const slots = n + groups;
  const seats = [];
  let k = 0, last = null;
  for (const p of participants) {
    if (groups && last !== null && (p.kind || '') !== last) k++;
    last = p.kind || '';
    const theta = -Math.PI / 2 + (2 * Math.PI * (k + 0.5)) / Math.max(1, slots);
    seats.push({ id: p.id, name: p.name || p.id, kind: p.kind || '', theta, x: cx + R * Math.cos(theta), y: cy + R * Math.sin(theta) });
    k++;
  }
  const seat = Object.fromEntries(seats.map((s) => [s.id, s]));
  const looks = layerLooks(record);
  const shown = new Set(options.layers || (record.types || []).map((t) => t.name));
  const threads = [];
  for (const t of record.ties || []) {
    if (t.absent || !shown.has(t.type) || !holdsOn(t.since, t.until, at) || !seat[t.from] || !seat[t.to]) continue;
    const a = seat[t.from], b = seat[t.to];
    // a curve bowed towards the centre, more for seats far apart
    const mx = (a.x + b.x) / 2, my = (a.y + b.y) / 2;
    const pull = 0.55 + 0.25 * Math.min(1, Math.hypot(a.x - b.x, a.y - b.y) / (2 * R));
    const qx = mx + (cx - mx) * pull, qy = my + (cy - my) * pull;
    threads.push({ type: t.type, from: t.from, to: t.to, d: `M${f1(a.x)},${f1(a.y)} Q${f1(qx)},${f1(qy)} ${f1(b.x)},${f1(b.y)}`, ...looks[t.type] });
  }
  // the label of each seat, set along its radius, turned so that it reads left to right
  const texts = seats.map((s) => {
    const deg = (s.theta * 180) / Math.PI;
    const left = Math.cos(s.theta) < 0;
    const r = R + 12;
    const x = cx + r * Math.cos(s.theta), y = cy + r * Math.sin(s.theta);
    return { id: s.id, text: fit(s.name, label - 18, SIZE.name), x, y, rotate: left ? deg + 180 : deg, anchor: left ? 'end' : 'start' };
  });
  return { width, height: cy + R + label + 24 + (options.key === false ? 0 : 22 + 16 * (record.types || []).length), cx, cy, R, label, seats, threads, texts, looks, shown: [...shown] };
}

export function svgRound(record, options = {}) {
  const L = layoutRound(record, options);
  const at = options.at || '';
  const ground = options.ground || PALETTE.paper;
  const sel = options.selected || null;
  const diag = options.answer?.diagnoses || [];
  const cut = new Set(diag.filter((d) => d.kind === 'succession' && d.where.startsWith('participant ')).map((d) => d.where.slice('participant '.length)));
  const only = new Set(diag.filter((d) => d.kind === 'succession' && d.where.startsWith('tie ')).map((d) => d.where.slice(4)));
  const out = [];
  out.push(`<svg xmlns="http://www.w3.org/2000/svg" class="mr-round" viewBox="0 0 ${f1(L.width)} ${f1(L.height)}" width="${f1(L.width)}" height="${f1(L.height)}" font-family="${esc(FONT)}" role="img" aria-label="${esc(record.name || 'An undertaking')}, its participants on a ring and their relations as threads">`);
  out.push(`<rect class="mr-ground" x="0" y="0" width="${f1(L.width)}" height="${f1(L.height)}" fill="${ground}"/>`);
  out.push(`<defs>${Object.entries(L.looks).map(([type, look], i) => `<marker id="mr-arrow-${i}" viewBox="0 0 8 8" refX="7" refY="4" markerWidth="5" markerHeight="5" orient="auto-start-reverse"><path d="M0,0 L8,4 L0,8 z" fill="${look.ink}"/></marker>`).join('')}</defs>`);
  out.push(`<circle class="mr-ring" cx="${f1(L.cx)}" cy="${f1(L.cy)}" r="${f1(L.R)}" fill="none" stroke="${PALETTE.line}" stroke-width="0.8"/>`);
  const typeIndex = Object.fromEntries(Object.keys(L.looks).map((t, i) => [t, i]));
  // threads: the chosen participant's last, so that they lie above
  const mine = (t) => sel && (t.from === sel || t.to === sel);
  for (const t of [...L.threads].sort((a, b) => Number(mine(a)) - Number(mine(b)))) {
    const faint = sel && !mine(t);
    const doubled = only.has(`${t.from} ${t.type} ${t.to}`);
    if (doubled) out.push(`<path class="mr-only" d="${t.d}" fill="none" stroke="${t.ink}" stroke-width="4.2" stroke-opacity="${faint ? 0.12 : 0.28}"/>`);
    out.push(`<path class="mr-thread${faint ? ' faint' : ''}${doubled ? ' only-link' : ''}" data-type="${esc(t.type)}" data-from="${esc(t.from)}" data-to="${esc(t.to)}" d="${t.d}" fill="none" stroke="${t.ink}" stroke-width="${mine(t) ? 1.6 : 1.1}" stroke-opacity="${faint ? 0.18 : 0.8}"${t.dash ? ` stroke-dasharray="${t.dash}"` : ''} marker-end="url(#mr-arrow-${typeIndex[t.type]})"/>`);
  }
  // seats
  for (const s of L.seats) {
    const k = kindOf(s.kind);
    const chosen = s.id === sel;
    if (cut.has(s.id)) out.push(`<circle class="mr-cut" cx="${f1(s.x)}" cy="${f1(s.y)}" r="9.5" fill="none" stroke="${PALETTE.rose}" stroke-width="1"/><circle class="mr-cut" cx="${f1(s.x)}" cy="${f1(s.y)}" r="12" fill="none" stroke="${PALETTE.rose}" stroke-width="1"/>`);
    out.push(`<g class="mr-seat${chosen ? ' chosen' : ''}" data-participant="${esc(s.id)}" data-kind="${esc(s.kind)}"><title>${esc(s.name)} (${esc(k.word)})${cut.has(s.id) ? ': without it the relations of authority fall apart' : ''}</title>`
      + `<circle cx="${f1(s.x)}" cy="${f1(s.y)}" r="7" fill="${ground}" stroke="${chosen ? PALETTE.orchid : PALETTE.muted}" stroke-width="${chosen ? 1.8 : 0.9}"/>`
      + `<text x="${f1(s.x)}" y="${f1(s.y + 3)}" text-anchor="middle" font-size="8" fill="${chosen ? PALETTE.orchid : PALETTE.ink}">${esc(k.glyph)}</text></g>`);
  }
  for (const t of L.texts) {
    const chosen = t.id === sel;
    out.push(`<text class="mr-name${chosen ? ' chosen' : ''}" data-participant="${esc(t.id)}" x="${f1(t.x)}" y="${f1(t.y)}" transform="rotate(${f1(t.rotate)} ${f1(t.x)} ${f1(t.y)})" text-anchor="${t.anchor}" dominant-baseline="central" font-size="${SIZE.name}" fill="${chosen ? PALETTE.orchid : PALETTE.ink}"${chosen ? ' font-weight="600"' : ''}>${esc(t.text)}</text>`);
  }
  // read from the chosen seat: every other pair marked, layer by layer
  if (sel && L.seats.some((s) => s.id === sel)) {
    const layers = L.shown;
    for (const s of L.seats) {
      if (s.id === sel) continue;
      layers.forEach((type, i) => {
        const there = status(record, type, sel, s.id, at), back = status(record, type, s.id, sel, at);
        // present when either way holds; absent only when both ways are known not to; otherwise one way at least is not observed
        const st = there === 'present' || back === 'present' ? 'present' : there === 'absent' && back === 'absent' ? 'absent' : 'not observed';
        const look = L.looks[type];
        const r = L.R - 13 - 7 * i;
        const x = L.cx + r * Math.cos(s.theta), y = L.cy + r * Math.sin(s.theta);
        const title = `<title>${esc(type)} between ${esc(L.seats.find((q) => q.id === sel).name)} and ${esc(s.name)}: ${STATUS[st].word}</title>`;
        if (st === 'present') out.push(`<g class="mr-pair present" data-status="present" data-type="${esc(type)}" data-participant="${esc(s.id)}">${title}<circle cx="${f1(x)}" cy="${f1(y)}" r="2.2" fill="${look.ink}"/></g>`);
        else if (st === 'absent') out.push(`<g class="mr-pair absent" data-status="absent" data-type="${esc(type)}" data-participant="${esc(s.id)}">${title}<path d="M${f1(x - 2)},${f1(y - 2)} L${f1(x + 2)},${f1(y + 2)} M${f1(x - 2)},${f1(y + 2)} L${f1(x + 2)},${f1(y - 2)}" stroke="${look.ink}" stroke-width="1"/></g>`);
        else out.push(`<g class="mr-pair unobserved" data-status="not observed" data-type="${esc(type)}" data-participant="${esc(s.id)}">${title}<circle cx="${f1(x)}" cy="${f1(y)}" r="2.1" fill="none" stroke="${PALETTE.faint}" stroke-width="0.7" stroke-dasharray="1.1 1.1"/></g>`);
      });
    }
  }
  // the centre: what is drawn, and when
  if (options.centre !== false) {
    const lines = [fit(record.name || 'An undertaking', L.R * 1.3, SIZE.centre), `${L.seats.length} participants · ${L.threads.length} ties drawn`, at ? `on ${at}` : 'every tie, whenever it held'];
    lines.forEach((t, i) => out.push(`<text class="mr-centre" x="${f1(L.cx)}" y="${f1(L.cy - 12 + 16 * i)}" text-anchor="middle" font-size="${i ? SIZE.small : SIZE.centre}" fill="${i ? PALETTE.muted : PALETTE.ink}" stroke="${ground}" stroke-width="3.5" stroke-linejoin="round" paint-order="stroke"${i ? '' : ' font-style="italic"'}>${esc(t)}</text>`));
  }
  // the key: each layer's ink and dash, with what the record knows of its pairs
  if (options.key !== false && sel) {
    const yk = L.cy + L.R + L.label + 12;
    out.push(`<g class="mr-marks-key"><circle cx="30" cy="${f1(yk)}" r="2.2" fill="${PALETTE.ink}"/><text x="38" y="${f1(yk + 3.5)}" font-size="${SIZE.key}" fill="${PALETTE.muted}">present</text>`
      + `<path d="M${f1(92)},${f1(yk - 2)} l4,4 m-4,0 l4,-4" stroke="${PALETTE.ink}" stroke-width="1"/><text x="102" y="${f1(yk + 3.5)}" font-size="${SIZE.key}" fill="${PALETTE.muted}">absent: recorded, or covered by a design that found none</text>`
      + `<circle cx="${f1(392)}" cy="${f1(yk)}" r="2.1" fill="none" stroke="${PALETTE.faint}" stroke-width="0.8" stroke-dasharray="1.1 1.1"/><text x="400" y="${f1(yk + 3.5)}" font-size="${SIZE.key}" fill="${PALETTE.muted}">not observed: a gap, never nothing</text></g>`);
  }
  if (options.key !== false) {
    const y0 = L.cy + L.R + L.label + 30;
    const counts = options.answer?.layers || [];
    (record.types || []).forEach((t, i) => {
      const x = 24, y = y0 + 16 * i;
      const look = L.looks[t.name];
      const c = counts.find((l) => l.type === t.name);
      const on = L.shown.includes(t.name);
      const words = c ? ` · ${c.present} present, ${c.absent} absent, ${c.not_observed} not observed` : '';
      out.push(`<g class="mr-key${on ? '' : ' off'}" data-type="${esc(t.name)}"><path d="M${f1(x)},${f1(y)} h26" stroke="${look.ink}" stroke-width="1.6"${look.dash ? ` stroke-dasharray="${look.dash}"` : ''} stroke-opacity="${on ? 1 : 0.3}"/>`
        + `<text x="${f1(x + 32)}" y="${f1(y + 3.5)}" font-size="${SIZE.key}" fill="${on ? PALETTE.ink : PALETTE.faint}">${esc(fit(`${t.name}${words}`, L.width - 80, SIZE.key))}</text></g>`);
    });
  }
  out.push('</svg>');
  return out.join('');
}
