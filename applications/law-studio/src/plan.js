// The plan of proof (ADR 0121): what remains to be done. For each question, the ways to establish it and the ways to resist it,
// each a short list of steps with none to spare (a fact to make out, a fact to show does not hold, a reading to argue for), with what
// is needed in every way marked; and beneath, the facts the question turns on, each as it stands, where its standing can be set.

import { waysOf, STEP } from '/visualization/js/evidence/index.js';
import { S, ensure, keep, gloss, factOf, evaluatedFact, answerOf, verdictMark, STANDINGS, GLYPH } from './common.js';

export function planView() {
  return { id: 'plan', label: 'Plan of proof', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('plan-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the plan of proof');
  const m = S.matter, redraw = () => ctx.show('plan');
  if (!S.evaluation) return fill(pane, el('p', { class: 'note' }, S.wrong || 'Ask a question on the matter\'s page: its plan of proof is drawn here.'));
  const issues = (m.issues || []).filter((i) => !S.issue || i.id === S.issue);
  ctx.note(issues.map((i) => { const w = waysOf(answerOf(i.id)); return `${i.question}: ${w.establish.length} ways to establish, ${w.resist.length} to resist`; }).join(' · '));
  const step = (s) => el('button', { type: 'button', class: `step ${s.kind}${s.every ? ' every' : ''}`,
    title: s.kind === 'reading' ? 'A reading to argue for: see the readings' : `${gloss(s.what) || s.what}${s.every ? ' — needed in every way' : ''}`,
    onclick: () => { if (s.kind === 'reading') { S.term = s.what.slice(1).split('=')[0]; ctx.show('readings'); } else { S.fact = s.what; S.doc = null; ctx.show('evidence'); } } },
    el('span', { class: 'glyph' }, STEP[s.kind].glyph), ' ', el('span', { class: 'verb' }, STEP[s.kind].word), ' ', el('b', {}, s.what));
  const lane = (title, ways, none) => el('section', { class: 'lane' }, el('h4', {}, title),
    ways.length ? el('ol', { class: 'ways' }, ways.map((w, k) => el('li', {}, el('span', { class: 'n' }, `${k + 1}`),
      w.length ? w.map((s, j) => [j ? el('span', { class: 'and' }, 'and') : null, step(s)]) : el('span', { class: 'small' }, 'nothing more: it follows as the record stands')))) : el('p', { class: 'small' }, none));
  const set = async (id, standing) => {
    await keep(ctx, (x) => { const y = (x.facts ||= []).find((q) => q.id === id); if (y) y.standing = standing; else x.facts.push({ id, text: '', standing }); }, 'set a fact\'s standing', `${id}: ${standing}`);
    redraw();
  };
  const sections = issues.map((i) => {
    const a = answerOf(i.id), w = waysOf(a);
    const turns = [...w.given, ...w.open, ...w.decided];
    const allege = (id) => set(id, 'alleged');
    return el('section', { class: 'issue-plan' },
      el('header', {}, el('h3', {}, gloss(i.question) || i.question), a ? verdictMark(el, a.class) : null,
        el('span', { class: 'small' }, `${i.burden ? `the burden is on ${i.burden}` : 'no burden stated'}${i.standard ? `, to the standard “${i.standard}”` : ''}`)),
      w.refused ? el('p', { class: 'note wrong' }, w.refused) : [
        w.every.length ? el('p', { class: 'needed' }, 'Whatever way is taken: ', w.every.map((s, k) => [k ? ', ' : '', step({ ...s, every: false })]), '.') : null,
        el('div', { class: 'lanes' },
          lane(`To establish it${i.burden ? ` (${i.burden})` : ''}`, w.establish, 'Whatever of the open facts is made out, it does not follow under the admissible readings.'),
          lane('To resist it', w.resist, 'Nothing resists it: it follows whatever the open facts turn out to be.'))],
      el('table', { class: 'facts' }, el('thead', {}, el('tr', {}, ['The fact', 'It stands as', 'Its evidence', 'In the plan'].map((h) => el('th', {}, h)))),
        el('tbody', {}, turns.map((id) => {
          const f = factOf(id), e = evaluatedFact(id);
          return el('tr', { class: w.open.includes(id) ? 'open' : '' },
            el('td', {}, el('button', { type: 'button', class: 'key', onclick: () => { S.fact = id; S.doc = null; ctx.show('evidence'); } }, id), el('span', { class: 'small' }, ` ${f?.text || gloss(id)}`)),
            el('td', {}, el('select', { 'aria-label': `The standing of ${id}`, onchange: (ev) => set(id, ev.target.value) },
              f ? null : el('option', { value: '', selected: true }, 'not in the record'), STANDINGS.map((s) => el('option', { value: s, selected: f?.standing === s }, `${GLYPH[s]} ${s}`))),
              e?.used && e.used !== e.declared ? el('span', { class: 'small' }, ` → ${e.used}`) : null),
            el('td', {}, e?.evidence || '—'),
            el('td', {}, w.open.includes(id) ? 'open' : w.given.includes(id) ? 'given' : 'decided against'));
        }))),
      w.absent.length ? el('p', { class: 'absent' }, 'The question also turns on facts the record does not mention. They are left out of the plan until someone alleges them: ',
        w.absent.map((id, k) => [k ? ' ' : '', el('button', { type: 'button', class: 'step', title: `${gloss(id) || id} — allege it, to bring it into the plan`, onclick: () => allege(id) }, el('span', { class: 'glyph' }, '+'), ' ', el('b', {}, id))])) : null);
  });
  fill(pane, el('header', { class: 'wall-head' }, el('h2', {}, 'What remains to be proved'),
    S.issue ? el('button', { type: 'button', class: 'key', onclick: () => { S.issue = null; redraw(); } }, 'Every question') : null), sections);
}
