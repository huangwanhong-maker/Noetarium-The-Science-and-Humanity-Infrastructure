// Evidence in space and time (ADR 0123): the place as the page. The archive's claims lie on it as discs of their own radius, toned
// by the weight of their evidence (or, in the mode "what to do", by the act chosen for them, ringed where it is worth looking
// first); its items lie on it as glyphs where they were, by kind and by what they say. Whatever is chosen opens where it stands: a
// claim with what bears on it and how much, what to do about it and what looking would be worth; an item with its source, its
// holding and the claims it bears on. Beneath, the items along time. A claim is placed by a click, and the decision is declared
// beside the map.

import { sceneOf, basePaths, svgPlace, svgStrip, scaleBar, STANDINGS, ratioWords, toneOf } from '/visualization/js/evidence/located.js';
import { E, base, ensure, within, load, keep, place, weighed, decided, itemOf, claimOf, keptHolding, keptItem, sourceOf, holdingOf, pct, fig, metres, when, closeKey } from './_shared.js';

export function view() {
  return {
    id: 'evidence', label: 'Evidence', draw,
    heard: (e, ctx) => { if (/evidence|holding|example|rows in|handed|claim|decision|archive/.test(e.what)) { E.archive = null; if (document.querySelector('.tab.on')?.dataset.tab === 'evidence') ctx.show('evidence'); } },
  };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('ev-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the evidence');
  if (!E.kept) return fill(pane, begin(ctx));
  const redraw = () => within(ctx, 'evidence');
  await place(ctx);
  const scene = sceneOf(E.archive, E.appraisal, { box: E.place?.box || undefined });
  const head = header(ctx, scene, redraw);
  if (!scene) return fill(pane, head, el('p', { class: 'note' }, 'Nothing in the archive is placed yet: give its items and claims a latitude and a longitude, or take a table in with them.'), begin(ctx, true));
  const f = scene.frame;
  const pad = f.width * 0.01;
  E.vb ||= [f.west - pad, -f.north - pad, f.width + 2 * pad, f.height + 2 * pad];

  const map = el('div', { class: `ev-map${E.placing ? ' placing' : ''}` });
  const under = el('div', { class: 'ev-base' }), over = el('div', { class: 'ev-over' });
  under.innerHTML = svgPlace({ ...scene, claims: [], items: [], acts: [] }, { base: basePaths(E.place, f), showItems: false });
  map.append(under, over);
  const bar = el('div', { class: 'ev-scale' });
  const credit = el('div', { class: 'ev-credit' }, E.place?.credit ? `The place: ${E.place.credit}${E.place.release ? ` (${E.place.release})` : ''}` : '');
  map.append(bar, credit);
  const strip = el('div', { class: 'ev-strip' });

  // The view box, and where a point of the map is on the screen and back (the drawing keeps its aspect, centred).
  const fit = () => {
    const r = map.getBoundingClientRect(), [x, y, w, h] = E.vb, s = Math.min(r.width / w, r.height / h);
    return { r, s, ox: (r.width - w * s) / 2, oy: (r.height - h * s) / 2, x, y, w, h };
  };
  const toScreen = (px, py) => { const t = fit(); return [t.ox + (px - t.x) * t.s, t.oy + (-py - t.y) * t.s]; };
  const toWorld = (cx, cy) => { const t = fit(); return [t.x + (cx - t.r.left - t.ox) / t.s, -(t.y + (cy - t.r.top - t.oy) / t.s)]; };
  const project = (x, y) => { const lat = f.lat0 + y / 111320, lon = f.lon0 + x / (111320 * Math.cos((f.lat0 * Math.PI) / 180)); return [lat, lon]; };

  let card = null;
  const paint = () => {
    const s = fit().s;
    over.innerHTML = svgPlace(scene, { mode: E.mode, selected: E.selected, size: s > 0 ? 4.2 / s : E.vb[2] / 300, base: null });   // a glyph of about four pixels, at any zoom
    for (const s of map.querySelectorAll('svg')) s.setAttribute('viewBox', E.vb.map((v) => v.toFixed(1)).join(' '));
    const t = fit(), sb = scaleBar(t.r.width / t.s);
    fill(bar, el('span', { class: 'rule', style: `width:${(sb.metres * t.s).toFixed(0)}px` }), el('span', {}, sb.words));
    if (card) placeCard();
  };
  const placeCard = () => {
    const at = E.selected?.startsWith('c:') ? scene.claims.find((c) => c.id === E.selected.slice(2)) : scene.items.find((i) => i.id === E.selected?.slice(2));
    if (!at || !card) return;
    const t = fit(), [sx, sy] = toScreen(at.x, at.y), W = 360, reach = (at.r || 0) * t.s;
    const right = sx + reach + 16 + W <= t.r.width;
    const tall = Math.min(card.offsetHeight || 320, t.r.height - 12);
    Object.assign(card.style, { left: `${Math.max(6, right ? sx + reach + 16 : sx - reach - 16 - W)}px`, top: `${Math.max(6, Math.min(sy - 40, t.r.height - tall - 6))}px`, width: `${W}px` });
  };
  const paintStrip = () => {
    strip.innerHTML = svgStrip(scene, { width: Math.max(480, pane.clientWidth - 48), selected: E.selected });
  };

  // Choosing, placing, panning and zooming.
  let down = null;
  map.addEventListener('pointerdown', (e) => { if (e.target.closest('.ev-card')) return; down = { x: e.clientX, y: e.clientY, vb: [...E.vb], moved: false }; map.setPointerCapture(e.pointerId); });
  map.addEventListener('pointermove', (e) => {
    if (!down) return;
    const dx = e.clientX - down.x, dy = e.clientY - down.y;
    if (!down.moved && Math.hypot(dx, dy) < 4) return;
    down.moved = true;
    const t = fit();
    E.vb = [down.vb[0] - dx / t.s, down.vb[1] - dy / t.s, down.vb[2], down.vb[3]];
    for (const s of map.querySelectorAll('svg')) s.setAttribute('viewBox', E.vb.map((v) => v.toFixed(1)).join(' '));
    if (card) placeCard();
  });
  map.addEventListener('pointerup', async (e) => {
    const was = down;
    down = null;
    if (!was || was.moved || e.target.closest('.ev-card')) { if (was?.moved) paint(); return; }
    if (E.placing) {
      const [x, y] = toWorld(e.clientX, e.clientY), [lat, lon] = project(x, y);
      E.placing = false;
      await placeClaim(ctx, lat, lon, scene);
      return redraw();
    }
    const hit = document.elementFromPoint(e.clientX, e.clientY);
    const c = hit?.closest?.('[data-claim]'), i = hit?.closest?.('[data-item]');
    const key = i ? `i:${i.dataset.item}` : c ? `c:${c.dataset.claim}` : null;
    E.selected = key && key !== E.selected ? key : null;
    E.editing = false;
    redraw();
  });
  let settle = null;
  map.addEventListener('wheel', (e) => {
    e.preventDefault();
    const t = fit(), [wx, wy] = toWorld(e.clientX, e.clientY), k = Math.exp(e.deltaY * 0.0015);
    const w = Math.min(f.width * 1.4, Math.max(f.width / 60, E.vb[2] * k)), h = E.vb[3] * (w / E.vb[2]);
    const fx = (e.clientX - t.r.left - t.ox) / (E.vb[2] * t.s), fy = (e.clientY - t.r.top - t.oy) / (E.vb[3] * t.s);
    E.vb = [wx - fx * w, -wy - fy * h, w, h];
    for (const s of map.querySelectorAll('svg')) s.setAttribute('viewBox', E.vb.map((v) => v.toFixed(1)).join(' '));
    if (card) placeCard();
    clearTimeout(settle);
    settle = setTimeout(paint, 140);
  }, { passive: false });
  strip.addEventListener('click', (e) => {
    const i = e.target.closest('[data-item]');
    if (!i) return;
    E.selected = `i:${i.dataset.item}`;
    const it = scene.items.find((x) => x.id === i.dataset.item);
    if (it && (it.x < E.vb[0] || it.x > E.vb[0] + E.vb[2] || -it.y < E.vb[1] || -it.y > E.vb[1] + E.vb[3])) E.vb = [it.x - E.vb[2] / 2, -it.y - E.vb[3] / 2, E.vb[2], E.vb[3]];
    redraw();
  });

  if (E.selected) {
    card = E.selected.startsWith('c:') ? claimCard(ctx, E.selected.slice(2), scene, redraw) : itemCard(ctx, E.selected.slice(2), scene, redraw);
    if (card) map.append(card);
  }
  const room = Math.max(320, pane.clientHeight - 230);
  map.style.height = `${Math.min(room, Math.max(380, (pane.clientWidth - 48) * (f.height / f.width)))}px`;
  fill(pane, head, E.mode === 'decide' ? decisionSheet(ctx, redraw) : null, map, legend(ctx, scene), strip);
  paint();
  paintStrip();
}

// ---- The head: what the archive is, how it stands, and what is drawn ------------------------------------------------------------------

function header(ctx, scene, redraw) {
  const { el } = ctx;
  const a = E.archive, k = E.appraisal?.keep, w = E.appraisal?.weigh;
  const n = (s) => (w?.claims || []).filter((c) => c.standing === s).length;
  const said = [];
  if (k) said.push(k.verdict.status === 'holds' ? `Every holding is what it was when it was taken in (${k.intact} observed now).` : `${k.verdict.reason[0].toUpperCase()}${k.verdict.reason.slice(1)}.`);
  if (w) said.push(`Of ${w.claims.length} claims, ${n('established')} established, ${n('refuted')} refuted, ${n('open')} open; no item bears on ${n('no evidence')}.`);
  const d = E.appraisal?.decide;
  if (d?.inspects && E.mode === 'decide') said.push(`Looking first at ${d.inspect.length} ${d.inspect.length === 1 ? 'place' : 'places'} by ${d.test}, within the budget, is expected to improve what is done by ${fig(d.gained, 3)}${d.exact ? ': no other set of places within it would gain more' : ''}.`);
  const mode = (m, words) => el('button', { type: 'button', class: `ev-mode${E.mode === m ? ' on' : ''}`, 'aria-pressed': E.mode === m ? 'true' : 'false', onclick: () => { E.mode = m; redraw(); } }, words);
  return el('header', { class: 'ev-head' },
    el('div', { class: 'ev-title-row' },
      el('input', { class: 'ev-title', type: 'text', value: a.title || '', placeholder: 'What the archive is of', 'aria-label': 'The archive\'s title', spellcheck: false,
        onchange: (e) => keep(ctx, (x) => { x.title = e.target.value.trim(); }, 'named the archive of evidence', e.target.value.trim()).then(redraw) }),
      el('span', { class: 'ev-modes', role: 'group', 'aria-label': 'What the claims are toned by' }, mode('weigh', 'What the evidence shows'), mode('decide', 'What to do')),
      el('button', { type: 'button', class: `key${E.placing ? ' main' : ' edged'}`, title: 'The next click on the map places a claim there', onclick: () => { E.placing = !E.placing; redraw(); } }, E.placing ? 'Click the map…' : '+ a claim')),
    E.wrong ? el('p', { class: 'ev-said wrong' }, E.wrong) : el('p', { class: 'ev-said' }, said.join(' ')),
    a.about ? el('details', { class: 'ev-about' }, el('summary', {}, 'What this archive is, and what in it is declared'), el('p', {}, a.about)) : null);
}

function legend(ctx, scene) {
  const { el } = ctx;
  const items = el('span', { class: 'ev-legend-items' }, scene.kinds.map((k) => el('span', { class: 'ev-kind' }, el('i', { class: 'dot', style: `background:${k.hue}` }), `${k.kind} `, el('small', {}, `${k.count}`))),
    el('span', { class: 'ev-kind' }, el('i', { class: 'dot' }), 'says it holds'), el('span', { class: 'ev-kind' }, el('i', { class: 'ring' }), 'denies it'), el('span', { class: 'ev-kind' }, el('i', { class: 'cross' }, '×'), 'does not count'));
  if (E.mode === 'decide') {
    return el('div', { class: 'ev-legend' }, el('span', { class: 'ev-legend-claims' }, scene.acts.map((a) => el('span', { class: 'ev-kind' }, el('i', { class: 'swatch', style: `background:${a.hue}` }), `${a.act} `, el('small', {}, `${a.count}`))),
      el('span', { class: 'ev-kind' }, el('i', { class: 'swatch inspect' }), 'worth looking at first')), items);
  }
  const stops = [-20, -10, 0, 10, 20].map((d) => toneOf(d)).join(', ');
  return el('div', { class: 'ev-legend' },
    el('span', { class: 'ev-legend-claims' }, el('span', { class: 'ev-ramp-words' }, 'against'), el('i', { class: 'ev-ramp', style: `background:linear-gradient(to right, ${stops})` }), el('span', { class: 'ev-ramp-words' }, 'for'),
      el('small', {}, 'the weight of a claim\'s evidence, to ±20 dB (odds moved a hundredfold)'), el('span', { class: 'ev-kind' }, el('i', { class: 'swatch empty' }), 'nothing bears on it')),
    items);
}

// ---- A claim, opened where it stands ---------------------------------------------------------------------------------------------------

function claimCard(ctx, id, scene, redraw) {
  const { el } = ctx;
  const c = claimOf(id), w = weighed(id), d = decided(id), sc = scene.claims.find((x) => x.id === id);
  if (!c) return null;
  if (E.editing) return claimForm(ctx, c, scene, redraw);
  const st = STANDINGS[w?.standing] || { glyph: '·', word: w?.standing || '' };
  const box = el('aside', { class: 'ev-card' }, closeKey(el, () => { E.selected = null; redraw(); }),
    el('p', { class: 'ev-kicker' }, `Claim ${c.id}`), el('h3', {}, c.text || c.id),
    el('p', { class: 'ev-where' }, `Within ${metres(c.radius)} of ${c.lat.toFixed(5)}, ${c.lon.toFixed(5)}${c.from ? `, ${when(c.from)} to ${when(c.until || c.from)}` : ''}`));
  if (w?.error) box.append(el('p', { class: 'wrong' }, w.error));
  else if (w) {
    box.append(el('p', { class: `ev-standing ${String(w.standing).replace(/\s+/g, '-')}` }, el('b', {}, `${st.glyph} ${st.word}`), ` — from ${pct(w.prior)} to ${pct(w.posterior, 1)}`),
      w.items?.length ? el('p', { class: 'ev-weight' }, el('i', { class: 'swatch', style: `background:${sc?.tone || 'transparent'}` }), `${w.decibans >= 0 ? '+' : ''}${fig(w.decibans, 3)} dB: ${ratioWords(w.ratio)}`) : null,
      el('p', { class: 'small' }, w.why + '.'));
    if (w.items?.length) {
      const rows = [...w.items].sort((p, q) => Math.abs(Math.log(q.alone || 1)) - Math.abs(Math.log(p.alone || 1))).slice(0, 10);
      box.append(el('h4', {}, `What bears on it${w.items.length > rows.length ? `: the ${rows.length} that weigh most of ${w.items.length}` : ''}`),
        el('ul', { class: 'ev-items' }, rows.map((x) => {
          const it = itemOf(x.id), k = scene.kinds.find((kk) => kk.kind === (it?.kind || 'item'));
          return el('li', {}, el('button', { type: 'button', class: 'ev-item-row', onclick: (e) => { e.stopPropagation(); E.selected = `i:${x.id}`; redraw(); } },
            el('i', { class: x.says === 'holds' ? 'dot' : 'ring', style: x.says === 'holds' ? `background:${k?.hue}` : `border-color:${k?.hue}` }),
            el('span', { class: 'what' }, it?.title || x.id),
            el('span', { class: 'num' }, x.alone >= 1 ? `×${fig(x.alone, 2)}` : `÷${fig(1 / x.alone, 2)}`),
            el('span', { class: 'small' }, x.about < 0.999 ? `about it ${pct(x.about)}${Number.isFinite(x.distance) ? `, ${metres(x.distance)} off` : ''}` : x.origin ? `one of a group: ${x.origin}` : x.source)));
        })));
      if (w.overstatement > 1.5) box.append(el('p', { class: 'ev-note' }, `Counted as ${w.items.length} independent reports these would have weighed ${fig(w.overstatement, 2)} times as much: ${w.items.length - w.groups.length} of them repeat a registration counted once.`));
    }
    if (w.left_out?.length) box.append(el('p', { class: 'small' }, `${w.left_out.length} ${w.left_out.length === 1 ? 'item about it does' : 'items about it do'} not count: ${w.left_out[0].why}${w.left_out.length > 1 ? ', and others' : ''}.`));
  }
  if (d) {
    const acts = [...d.acts].sort((p, q) => q.worth - p.worth);
    box.append(el('h4', {}, 'What to do'),
      el('p', {}, el('b', {}, d.act), ` — worth ${fig(d.worth, 3)} in expectation, against ${acts.filter((x) => x.name !== d.act).map((x) => `${fig(x.worth, 3)} to ${x.name}`).join(', ')}.`),
      E.appraisal.decide.inspects ? el('p', {}, d.inspect ? el('b', { class: 'ev-look' }, '◎ Worth looking at first. ') : '', `${E.appraisal.decide.test[0].toUpperCase()}${E.appraisal.decide.test.slice(1)} would be worth ${fig(d.evsi, 3)} here, for a cost of ${fig(d.cost, 3)}: `,
        d.after_found === d.after_clear ? `whatever it found, it would be to ${d.after_found}.` : `if it found the thing (${pct(d.p_found)} likely), ${d.after_found}, at ${pct(d.if_found)}; if not, ${d.after_clear}, at ${pct(d.if_clear)}.`) : null);
  }
  box.append(el('div', { class: 'keys' },
    el('button', { type: 'button', class: 'key edged', onclick: (e) => { e.stopPropagation(); E.editing = true; redraw(); } }, 'Change the claim'),
    el('button', { type: 'button', class: 'key', onclick: (e) => { e.stopPropagation(); E.selected = null; keep(ctx, (x) => { x.claims = x.claims.filter((y) => y.id !== id); }, 'withdrew a claim', id).then(redraw); } }, 'Withdraw it')));
  return box;
}

// A claim's terms, changed in place.
function claimForm(ctx, c, scene, redraw) {
  const { el } = ctx;
  const field = (label, key, attrs = {}, under) => el('label', { class: 'ev-field' }, el('span', {}, label),
    el('input', { type: attrs.type || 'text', value: c[key] ?? '', step: 'any', ...attrs, 'data-key': key }), under ? el('small', {}, under) : null);
  const text = el('textarea', { rows: 2, 'data-key': 'text' }, c.text || '');
  const kinds = scene.kinds.map((k) => el('label', { class: 'ev-check' }, el('input', { type: 'checkbox', value: k.kind, checked: !(c.kinds || []).length || (c.kinds || []).includes(k.kind) }), ` ${k.kind}`));
  const event = el('select', { 'data-key': 'event' }, [['each', 'an event of its own, each group (their force adds up)'], ['one', 'one event, all of them (they can only settle it)']].map(([v, w]) => el('option', { value: v, selected: (c.event || 'each') === v }, w)));
  const form = el('aside', { class: 'ev-card ev-form' }, closeKey(el, () => { E.editing = false; redraw(); }), el('p', { class: 'ev-kicker' }, `Claim ${c.id}, its terms`),
    el('label', { class: 'ev-field wide' }, el('span', {}, 'What is claimed'), text),
    el('div', { class: 'ev-fields' }, field('Within (m)', 'radius', { type: 'number', min: 1 }), field('From', 'from', {}, 'a year, month, date or instant'), field('Until', 'until'),
      field('Prior', 'prior', { type: 'number', min: 0, max: 1 }, 'before any item'), field('P(event | it holds)', 'e_given_h', { type: 'number', min: 0, max: 1 }), field('P(event | it does not)', 'e_given_not_h', { type: 'number', min: 0, max: 1 })),
    el('p', { class: 'ev-field wide' }, el('span', {}, 'The items report'), event),
    el('p', { class: 'ev-field wide' }, el('span', {}, 'Kinds of item that bear on it'), el('span', { class: 'ev-checks' }, kinds)),
    el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key main', onclick: async (e) => {
      e.stopPropagation();
      const got = {};
      for (const input of form.querySelectorAll('[data-key]')) got[input.dataset.key] = input.value;
      const chosen = [...form.querySelectorAll('.ev-check input')].filter((x) => x.checked).map((x) => x.value);
      E.editing = false;
      await keep(ctx, (x) => {
        const y = x.claims.find((z) => z.id === c.id);
        y.text = got.text.trim();
        for (const k of ['radius', 'prior', 'e_given_h', 'e_given_not_h']) { const v = Number(got[k]); if (got[k] !== '' && Number.isFinite(v)) y[k] = v; else delete y[k]; }
        for (const k of ['from', 'until']) { if (got[k].trim()) y[k] = got[k].trim(); else delete y[k]; }
        y.event = got.event;
        if (chosen.length && chosen.length < scene.kinds.length) y.kinds = chosen; else delete y.kinds;
      }, 'changed a claim\'s terms', c.id);
      redraw();
    } }, 'Keep these terms')));
  return form;
}

async function placeClaim(ctx, lat, lon, scene) {
  const ids = new Set((E.archive.claims || []).map((c) => c.id));
  let n = 1;
  while (ids.has(`k${n}`)) n++;
  const times = scene.items.filter((i) => i.from !== null);
  const iso = (t) => new Date(t * 1000).toISOString().slice(0, 10);
  // A claim placed among others takes their terms (the commonest of each); a lone one is reported directly by its items.
  const commonest = (key, fallback) => {
    const seen = new Map();
    for (const c of E.archive.claims || []) if (c[key] !== undefined) seen.set(JSON.stringify(c[key]), (seen.get(JSON.stringify(c[key])) || 0) + 1);
    return seen.size ? JSON.parse([...seen].sort((p, q) => q[1] - p[1])[0][0]) : fallback;
  };
  const claim = { id: `k${n}`, text: 'What is claimed of this place', lat: +lat.toFixed(6), lon: +lon.toFixed(6), radius: commonest('radius', 60), prior: commonest('prior', 0.5),
    e_given_h: commonest('e_given_h', 1), e_given_not_h: commonest('e_given_not_h', 0), event: commonest('event', 'one') };
  const kinds = commonest('kinds', null);
  if (kinds) claim.kinds = kinds;
  if (times.length) claim.from = iso(Math.min(...times.map((i) => i.from))), claim.until = iso(Math.max(...times.map((i) => i.until ?? i.from)));
  await keep(ctx, (x) => { (x.claims ||= []).push(claim); }, 'placed a claim', `${claim.id} at ${claim.lat}, ${claim.lon}`);
  E.selected = `c:${claim.id}`, E.editing = true;
}

// ---- An item, opened where it stands ---------------------------------------------------------------------------------------------------

function itemCard(ctx, id, scene, redraw) {
  const { el } = ctx;
  const it = itemOf(id), si = scene.items.find((x) => x.id === id);
  if (!it) return null;
  const src = sourceOf(it.source), relay = it.relay ? sourceOf(it.relay) : null, h = it.holding ? holdingOf(it.holding) : null, kh = it.holding ? keptHolding(it.holding) : null, ki = keptItem(id);
  const sharing = it.origin ? (E.archive.items || []).filter((x) => x.origin === it.origin && x.source === it.source).length : 1;
  const bears = (E.appraisal?.weigh?.claims || []).filter((c) => (c.items || []).some((x) => x.id === id));
  const status = it.status || 'accepted';
  const setStatus = (s) => keep(ctx, (x) => { x.items.find((y) => y.id === id).status = s; }, s === 'accepted' ? 'accepted an item of evidence' : s === 'rejected' ? 'rejected an item of evidence' : 'set an item back to a proposal', it.title || id).then(redraw);
  return el('aside', { class: 'ev-card' }, closeKey(el, () => { E.selected = null; redraw(); }),
    el('p', { class: 'ev-kicker' }, `${it.kind || 'item'}, ${it.says === 'denies' ? 'denying it' : 'saying it holds'}`), el('h3', {}, it.title || id),
    el('p', { class: 'ev-where' }, `${Number.isFinite(it.lat) ? `${it.lat.toFixed(5)}, ${it.lon.toFixed(5)}${it.radius ? `, good to ${metres(it.radius)}` : ''}` : 'not placed'}${it.from ? `; ${when(it.from)}${it.until ? ` to ${when(it.until)}` : ''}` : ''}`),
    el('h4', {}, 'Who says it'),
    el('p', {}, src ? `${src.name || src.id}: ${src.hit !== undefined ? `a report where the thing is ${pct(src.hit, 1)} of the time, where it is not ${pct(src.false_alarm, 1)}` : 'its credibility by Schum\'s three attributes'}.` : `${it.source || 'no source'}, which the archive does not declare.`,
      src?.note ? el('small', { class: 'ev-src-note' }, ` (${src.note})`) : null),
    relay ? el('p', { class: 'small' }, `Relayed by ${relay.name || relay.id}.`) : null,
    it.origin ? el('p', { class: 'small' }, sharing > 1 ? `One of ${sharing} items that repeat one registration (${it.origin}): together they count as one report.` : `Its registration: ${it.origin}.`) : null,
    el('h4', {}, 'What it rests on'),
    h ? el('p', {}, el('button', { type: 'button', class: 'key', onclick: (e) => { e.stopPropagation(); E.holding = h.id; ctx.show('holdings'); } }, h.name || h.id),
      kh ? el('span', { class: `ev-fixity ${kh.sound ? 'sound' : 'unsound'}` }, kh.sound ? ` ✓ ${kh.fixity}, its custody unbroken` : ` ✕ ${kh.why}`) : null)
      : el('p', { class: 'small' }, 'No holding is kept for it: it rests on its source\'s word as it was entered.'),
    ki && !ki.sound ? el('p', { class: 'wrong' }, `It does not count: ${ki.why}.`) : null,
    el('h4', {}, 'Its standing'),
    el('p', {}, status === 'accepted' ? 'Accepted.' : status === 'proposed' ? 'A proposal: it counts for nothing until a person accepts it.' : 'Rejected: it does not count.',
      el('span', { class: 'keys inline' },
        status !== 'accepted' ? el('button', { type: 'button', class: 'key edged', onclick: (e) => { e.stopPropagation(); setStatus('accepted'); } }, 'Accept') : null,
        status !== 'rejected' ? el('button', { type: 'button', class: 'key', onclick: (e) => { e.stopPropagation(); setStatus('rejected'); } }, 'Reject') : null)),
    bears.length ? el('h4', {}, `It bears on ${bears.length} ${bears.length === 1 ? 'claim' : 'claims'}`) : el('p', { class: 'small' }, si?.counts ? 'It falls in no claim\'s place and time.' : ''),
    bears.length ? el('ul', { class: 'ev-claims' }, bears.slice(0, 8).map((c) => {
      const x = c.items.find((y) => y.id === id), st = STANDINGS[c.standing] || { glyph: '·', word: c.standing };
      return el('li', {}, el('button', { type: 'button', class: 'ev-item-row', onclick: (e) => { e.stopPropagation(); E.selected = `c:${c.id}`; redraw(); } },
        el('span', { class: 'what' }, `${c.id}: ${st.glyph} ${st.word}, ${pct(c.posterior, 1)}`), el('span', { class: 'num' }, x.alone >= 1 ? `×${fig(x.alone, 2)}` : `÷${fig(1 / x.alone, 2)}`),
        el('span', { class: 'small' }, `about it ${pct(x.about)}`)));
    })) : null);
}

// ---- What to do: the decision declared, and what it answers ------------------------------------------------------------------------

function decisionSheet(ctx, redraw) {
  const { el } = ctx;
  const d = E.archive.decision;
  if (!d) {
    return el('section', { class: 'ev-sheet' }, el('p', {}, 'No decision is declared. A decision is two acts or more, each with what it is worth if a claim holds and if it does not; and, if one may look before acting, what looking finds and costs, and a budget.'),
      el('button', { type: 'button', class: 'key main', onclick: () => keep(ctx, (x) => { x.decision = { acts: [{ name: 'act', holds: -1, not: -1 }, { name: 'leave it', holds: -5, not: 0 }], inspection: { name: 'a look', sensitivity: 0.8, specificity: 0.95, cost: 0.2 }, budget: 2, charged: true }; }, 'declared a decision').then(redraw) }, 'Declare a decision'));
  }
  const num = (value, onchange, attrs = {}) => el('input', { type: 'number', step: 'any', value: value ?? '', ...attrs, onchange: (e) => { const v = Number(e.target.value); if (e.target.value !== '' && Number.isFinite(v)) onchange(v); } });
  const set = (f, what) => keep(ctx, (x) => f(x.decision), 'changed the decision', what).then(redraw);
  const r = E.appraisal?.decide;
  return el('section', { class: 'ev-sheet' },
    el('table', { class: 'ev-acts' }, el('thead', {}, el('tr', {}, el('th', {}, 'Act'), el('th', {}, 'Worth if it holds'), el('th', {}, 'If it does not'), el('th', {}, 'Chosen for'))),
      el('tbody', {}, d.acts.map((a, k) => el('tr', {},
        el('td', {}, el('input', { type: 'text', value: a.name, onchange: (e) => set((y) => { y.acts[k].name = e.target.value.trim() || a.name; }, `act ${k + 1} renamed`) })),
        el('td', {}, num(a.holds, (v) => set((y) => { y.acts[k].holds = v; }, `${a.name} if it holds: ${v}`))),
        el('td', {}, num(a.not, (v) => set((y) => { y.acts[k].not = v; }, `${a.name} if it does not: ${v}`))),
        el('td', { class: 'num' }, `${(r?.claims || []).filter((c) => c.act === a.name).length} claims`))))),
    el('p', { class: 'ev-look-terms' }, 'Before acting one may look: ', el('input', { type: 'text', class: 'ev-test-name', value: d.inspection?.name || 'a look', onchange: (e) => set((y) => { (y.inspection ||= {}).name = e.target.value.trim(); }, 'the look renamed') }),
      ', finding the thing where it is ', num(d.inspection?.sensitivity, (v) => set((y) => { (y.inspection ||= {}).sensitivity = v; }, `sensitivity ${v}`), { min: 0, max: 1, class: 'short' }),
      ' of the time and clearing where it is not ', num(d.inspection?.specificity, (v) => set((y) => { (y.inspection ||= {}).specificity = v; }, `specificity ${v}`), { min: 0, max: 1, class: 'short' }),
      ', at a cost of ', num(d.inspection?.cost, (v) => set((y) => { (y.inspection ||= {}).cost = v; }, `cost ${v}`), { min: 0, class: 'short' }),
      ' each, within a budget of ', num(d.budget, (v) => set((y) => { y.budget = v; }, `budget ${v}`), { min: 0, class: 'short' }), '.'),
    r?.inspects ? el('p', { class: 'ev-said' }, el('b', {}, `${r.inspect.length} ${r.inspect.length === 1 ? 'place' : 'places'} worth looking at first`), `, ringed on the map: they cost ${fig(r.spent, 3)} of ${fig(r.budget, 3)} and are expected to improve what is done by ${fig(r.gained, 3)}${r.exact ? ', more than any other set within the budget would' : `; no set would gain more than ${fig(r.bound, 3)}`}.`) : null,
    d.note ? el('p', { class: 'small' }, d.note) : null);
}

// ---- A project with no archive yet -------------------------------------------------------------------------------------------------------

function begin(ctx, placed = false) {
  const { el } = ctx;
  const lay = async (c) => {
    ctx.say('Laying out…');
    try {
      const r = await ctx.api(`${base(ctx)}/example`, { method: 'POST', body: { case: c.id } });
      ctx.log(`${r.title}: ${r.items} items, ${r.claims} claims, ${r.holdings} files taken in.`);
      E.vb = null;
      await load(ctx);
    } catch (error) { ctx.say(error.message); }
    within(ctx, 'evidence');
  };
  const tables = (ctx.project?.sources || []).map((s) => s.name);
  const which = el('select', { 'aria-label': 'A table of the project' }, tables.map((t) => el('option', { value: t }, t)));
  const as = el('select', { 'aria-label': 'Taken in as' }, ['items', 'claims', 'sources'].map((x) => el('option', { value: x }, x)));
  const take = async () => {
    try {
      const r = await ctx.api(`${base(ctx)}/take`, { method: 'POST', body: { table: which.value, as: as.value } });
      ctx.log(`${r.taken} rows taken in as ${as.value}, ${r.new} of them new.`);
      E.vb = null;
      await load(ctx);
      within(ctx, 'evidence');
    } catch (error) { ctx.say(error.message); }
  };
  return el('section', { class: 'ev-begin' },
    placed ? null : el('h2', {}, 'An archive of evidence in space and time'),
    placed ? null : el('p', { class: 'lede' }, 'Files are kept as they were taken in, with the digest of their bytes and who has held them. Each item of evidence is a report of something at a place and a time, by a source of declared or counted credibility. Claims about places and times are weighed by the items about them, each only as far as where and when it may be falls in the claim\'s place and time; reports that repeat one registration count once. Then what to do, and where it is worth looking first.'),
    E.cases.length ? el('div', { class: 'ev-choices' }, el('h3', {}, 'An example, laid out'), E.cases.map((c) => el('button', { type: 'button', class: 'ev-choice', onclick: () => lay(c) }, el('b', {}, c.title), el('span', {}, `${c.items} items, ${c.claims} claims. ${c.about.split('. ').slice(0, 2).join('. ')}.`)))) : null,
    el('div', { class: 'ev-own' }, el('h3', {}, 'From a table of the project'),
      tables.length ? el('p', {}, 'Take the rows of ', which, ' in as ', as, ' ', el('button', { type: 'button', class: 'key main', onclick: take }, 'Take them in')) : el('p', { class: 'small' }, 'Add a table to the project (drop a file on the page): its rows, each with an id, a latitude, a longitude, a time and what it says, can be taken in as items.'),
      el('p', { class: 'small' }, 'Items name their kind, source, latitude, longitude, radius (m), from, until, says (holds or denies) and, where several repeat one registration, an origin; claims their text, latitude, longitude, radius, from, until and prior; sources their hit and false-alarm rates.')));
}
