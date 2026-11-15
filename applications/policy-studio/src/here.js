// Here (ADR 0122): the crossing from where the intervention worked to this place, after Cartwright and Hardie. It is drawn as a
// crossing: on one bank the effect found there, on the other the effect to expect here, and between them the premises the argument
// needs, each as it stands: that it worked there (read off the evidence), that the same causal principle holds here, and that each
// support factor the strongest path needs is present here. A premise nothing is stated for is a plank missing, and is said. Where
// the studies name strata and the place states its own mix of them, the effect is carried through the strata and not as it stands.

import { svgIntervals, figure } from '/visualization/js/policy/index.js';
import { S, ensure, keep, begun, theory, nodeText, bodyAnswer, isRatio, interval, effect, statusOf, numberIn } from './common.js';

export function hereView() {
  return { id: 'here', label: 'Here', draw };
}

const STATES = [['stated', '✓ stated to be so'], ['unknown', '◌ nothing is stated'], ['against', '✕ the evidence is against it']];
const GLYPH = { stated: '✓', unknown: '◌', against: '✕' };

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('here-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the place');
  const q = S.question, redraw = () => ctx.show('here');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin with the theory: the argument for a place follows its strongest path.'));
  if (S.wrong) return fill(pane, el('p', { class: 'note wrong' }, S.wrong));
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  const place = q.place || {}, h = S.appraisal?.here, t = theory();
  const setPlace = (f, what, note) => change((x) => { x.place ||= { name: '', same_principle: 'unknown', factors: {} }; f(x.place); }, what, note);
  const pooled = (S.appraisal?.bodies || []).filter((b) => !b.error);

  const head = el('header', { class: 'q-head' },
    el('p', { class: 'sentence' }, 'Will it work in ',
      el('input', { class: 'q-place', type: 'text', value: place.name || '', placeholder: 'the place', 'aria-label': 'The place', onchange: (e) => setPlace((p) => { p.name = e.target.value.trim(); }, 'named the place', e.target.value.trim()) }), '?'),
    el('p', { class: 'small' }, 'That it worked somewhere is one premise of three. The other two are about this place, and no trial elsewhere can supply them.'));
  if (!h || h.error) {
    return fill(pane, head, el('p', { class: 'note wrong' }, h?.error || 'The argument could not be made.'),
      pooled.length ? el('p', { class: 'small' }, 'The effect found there is taken from ', bodyPick(ctx, place, pooled, setPlace), '.') : null);
  }
  const b = bodyAnswer(h.body), ratio = isRatio(b?.measure), st = statusOf(h.verdict);
  ctx.note(`${place.name || 'the place'}: ${st.word}`);

  const select = (value, set, label) => el('select', { class: `st ${value}`, 'aria-label': label, onchange: (e) => set(e.target.value) }, STATES.map(([v, w]) => el('option', { value: v, selected: v === value }, w)));
  const plank = (state, words, control, under) => el('li', { class: `plank ${state}` }, el('span', { class: 'glyph' }, GLYPH[state]), el('div', { class: 'what' }, el('span', {}, words), under ? el('span', { class: 'small' }, under) : null), control);
  const planks = el('ol', { class: 'planks' },
    plank(h.worked_there ? 'stated' : 'against', 'It worked there', el('span', { class: 'small read' }, 'read off the evidence'),
      h.worked_there ? 'the pooled interval lies on the side of benefit' : 'the pooled interval does not lie on the side of benefit'),
    plank(h.same_principle, 'The same causal principle holds here', select(h.same_principle, (v) => setPlace((p) => { p.same_principle = v; }, 'stated whether the same principle holds here', v), 'Whether the same causal principle holds here'),
      'what brings the outcome about there is what would bring it about here'),
    h.factors.map((f) => plank(f.state, f.name, select(f.state, (v) => setPlace((p) => { (p.factors ||= {})[f.name] = v; }, 'stated a support factor here', `${f.name}: ${v}`), `Whether it is so here: ${f.name}`),
      f.links.map((i) => `needed by ${nodeText(t.links[i].from)} → ${nodeText(t.links[i].to)}`).join('; '))),
    h.factors.length ? null : el('li', { class: 'plank none' }, el('span', { class: 'glyph' }, '·'), el('div', { class: 'what' }, el('span', {}, 'The strongest path names no support factor'), el('span', { class: 'small' }, 'name what each claim needs on the theory, and it is asked here'))));

  const bank = (cls, title, name, e, lo, hi, under) => el('section', { class: `bank ${cls}` }, el('h4', {}, title), el('p', { class: 'where' }, name),
    el('p', { class: 'big' }, effect(e, ratio)), el('p', { class: 'small' }, `[${effect(lo, ratio)}, ${effect(hi, ratio)}]${ratio ? ', a ratio' : ''}`), under ? el('p', { class: 'small' }, under) : null);
  const crossing = el('div', { class: 'crossing' },
    bank('there', 'There', el('span', {}, bodyPick(ctx, place, pooled, setPlace)), h.there.effect, h.there.lower, h.there.upper, `${b?.pooled?.k ?? ''} studies pooled, graded ${String(b?.grade || '').replace(/_/g, ' ')}`),
    planks,
    bank('here-side', 'Here', el('span', {}, place.name || 'the place'), h.effect, h.lower, h.upper, h.transported ? 'carried through the strata, weighted by this place\'s mix' : 'carried over as it stands, as the second premise asserts'));

  const verdict = el('p', { class: `answer ${h.verdict.status}` }, el('b', {}, `${st.glyph} ${h.verdict.status === 'holds' ? 'The argument is complete' : h.verdict.status === 'fails' ? 'The argument fails' : 'The argument is not complete'}`),
    ` — ${h.verdict.status === 'holds' ? h.verdict.reason : lacks(h)}.`,
    h.missing.length ? el('span', { class: 'small' }, ' Evidence about this place would complete it; a larger trial elsewhere would not.') : null);

  // The strata: the studies' effect in each, and this place's mix.
  const names = [...new Set((b?.strata || []).map((z) => z.name))];
  let strata = null;
  if (names.length) {
    const masses = Object.fromEntries((place.strata?.names || []).map((nm, k) => [nm, place.strata.masses?.[k] ?? 0]));
    const using = !!(place.strata?.names || []).length;
    const setMass = (nm, v) => setPlace((p) => { const m = { ...Object.fromEntries(names.map((z) => [z, 0])), ...masses, [nm]: v ?? 0 }; p.strata = { names, masses: names.map((z) => m[z]) }; }, 'stated the place\'s mix of strata', `${nm}: ${v ?? 0}`);
    strata = el('section', { class: 'strata-box' }, el('h4', {}, 'The strata the studies name'),
      el('table', { class: 'strata' },
        el('thead', {}, el('tr', {}, ['Stratum', 'Studies', ratio ? 'Ratio there' : 'Effect there', 'Its share here'].map((x) => el('th', {}, x)))),
        el('tbody', {}, b.strata.map((z) => el('tr', {}, el('td', {}, z.name), el('td', { class: 'n' }, z.k), el('td', { class: 'n' }, interval(z.effect, z.lower, z.upper, ratio)),
          el('td', { class: 'n' }, numberIn(el, masses[z.name] ?? (using ? 0 : null), (v) => setMass(z.name, v), { min: 0, placeholder: '—', 'aria-label': `The share of ${z.name} here` })))))),
      el('p', { class: 'small' }, using ? 'The effect here is the strata\'s effects weighted by this place\'s shares (peira\'s transport): it holds when the effect within a stratum is the same there and here. '
        : 'State this place\'s share of each stratum, and the effect is carried through them instead of as it stands. ',
        using ? el('button', { type: 'button', class: 'key', onclick: () => setPlace((p) => { delete p.strata; }, 'carried the effect over as it stands', '') }, 'Carry it over as it stands') : null));
  }
  const fig = el('div', { class: 'fig' });
  fig.innerHTML = svgIntervals([
    { label: 'There, pooled', value: h.there.effect, lower: h.there.lower, upper: h.there.upper },
    ...(b?.strata || []).map((z) => ({ label: `There, ${z.name}`, value: z.effect, lower: z.lower, upper: z.upper })),
    { label: `Here${place.name ? `: ${place.name.length > 28 ? `${place.name.slice(0, 27)}…` : place.name}` : ''}`, value: h.effect, lower: h.lower, upper: h.upper, kind: 'here' },
  ], { show: (v) => (ratio ? figure(Math.exp(v)) : figure(v)), label: 'the effect there and here', plotW: 280 });

  const unit = S.appraisal?.unit;
  fill(pane, head, crossing, verdict, el('div', { class: 'two' }, el('section', {}, fig, unit ? el('p', { class: 'small' }, `For the options: ${figure(unit.effect)} ± ${figure(unit.se)} a unit and year (${unit.said}).`) : null), strata));
}

// What the argument lacks, in a sentence.
function lacks(h) {
  const out = [];
  if (!h.worked_there) out.push('it is not shown to have worked there');
  if (h.same_principle === 'unknown') out.push('nothing is stated of whether the same causal principle holds here');
  if (h.same_principle === 'against') out.push('the evidence is against the same causal principle holding here');
  if (h.absent.length) out.push(`the evidence here is against what the path needs: ${h.absent.join('; ')}`);
  if (h.unknown.length) out.push(`it is not stated whether, here, ${h.unknown.join('; or whether ')}`);
  return out.join('; and ') || 'a premise is missing';
}

const bodyPick = (ctx, place, pooled, setPlace) => ctx.el('select', { class: 'q-pick', 'aria-label': 'The body of evidence the effect there is taken from', onchange: (e) => setPlace((p) => { if (e.target.value) p.body = e.target.value; else delete p.body; delete p.strata; }, 'took the effect there from another body', e.target.value || 'the default') },
  ctx.el('option', { value: '', selected: !place.body }, 'the evidence that bears on the outcome'), pooled.map((b) => ctx.el('option', { value: b.id, selected: b.id === place.body }, b.claim || b.id)));
