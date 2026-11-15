// The law (ADR 0121): the scheme the matter is read under, clause by clause. Each instrument is set as its text, in the words of the
// official text where they have been fetched; beside each clause stand the rules, exceptions and counts-as links that state it,
// marked where they apply in the matter. What the scheme's own check finds is said at the top: a rule cited to no clause, a
// quotation that is not in its clause, a fact never declared.

import { S, ensure, citing, answerOf } from './common.js';

export function lawView() {
  return { id: 'law', label: 'Law', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('law-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the law');
  const s = S.scheme;
  if (!s) return fill(pane, el('p', { class: 'note' }, 'The matter names no body of law yet: choose one on the matter\'s page.'));
  const faults = S.check?.faults || [];
  const counts = S.check?.counts || {};
  ctx.note(`${counts.provisions ?? 0} clauses, ${counts.rules ?? 0} rules, ${counts.terms ?? 0} open terms, ${faults.length} ${faults.length === 1 ? 'fault' : 'faults'}`);
  const applied = new Set();
  for (const i of S.matter.issues || []) for (const st of answerOf(i.id)?.rests_on?.steps || []) if (st.holds) applied.add(st.id);
  const redraw = () => ctx.show('law');

  const contents = el('nav', { class: 'contents', 'aria-label': 'The instruments' },
    s.instruments.map((ins) => el('section', {}, el('h3', { title: ins.title }, ins.citation || ins.title),
      ins.provisions.filter((p) => citing(p.id).length).map((p) => el('button', { type: 'button', class: `clause-link${S.provision === p.id ? ' on' : ''}`, onclick: () => { S.provision = p.id; redraw(); } }, p.label)))));

  const words = (x) => x.kind === 'rules' ? [x.elements.map((e, k) => [k ? ' and ' : '', el('code', {}, e)]), ' → ', el('code', {}, x.consequence)]
    : x.kind === 'exceptions' ? ['unless ', x.elements.map((e, k) => [k ? ' and ' : '', el('code', {}, e)]), ' (to ', el('code', {}, x.rule), ')']
      : [el('code', {}, x.fact), ' counts as ', el('code', {}, x.as), ' under ', el('code', {}, `[${x.term}=${x.reading}]`)];
  const body = el('article', { class: 'statute' },
    el('header', {}, el('h2', {}, s.title), el('p', { class: 'small' }, `${s.about || ''}${s.as_of ? ` As of ${s.as_of}.` : ''}`)),
    S.unfetched.length ? el('p', { class: 'note wrong' }, `The official text of ${S.unfetched.join(', ')} has not been fetched: run the work's recipe. Its clauses are shown without their words, and quotations from them cannot be checked.`) : null,
    faults.length ? el('ul', { class: 'faults' }, faults.map((f) => el('li', {}, el('span', { class: 'mark' }, '✕'), ' ', el('b', {}, f.where), ': ', f.what)))
      : el('p', { class: 'verdict holds' }, '✓ The scheme is whole: every rule is cited to a clause, every quotation stands in its clause, every fact a rule asks for is declared.'),
    s.instruments.map((ins) => el('section', { class: 'instrument' },
      el('h3', {}, ins.title), el('p', { class: 'small' }, [ins.citation, ins.kind, ins.as_of ? `text of ${ins.as_of}` : ''].filter(Boolean).join(' · '),
        ins.url ? [' · ', el('a', { href: ins.url, target: '_blank', rel: 'noopener' }, 'the official text')] : null),
      ins.provisions.map((p) => {
        const rules = citing(p.id);
        const depth = p.id.split('/').length - 2;
        return el('div', { class: `provision${S.provision === p.id ? ' on' : ''}${rules.length ? ' cited' : ''}`, id: `p-${p.id}`, style: `--depth:${Math.max(0, depth)}` },
          el('div', { class: 'words' }, el('span', { class: 'label' }, p.label), p.text ? el('p', {}, p.text) : el('p', { class: 'small' }, 'its words are not fetched')),
          rules.length ? el('div', { class: 'margin' }, rules.map((x) => el('p', { class: `stated${applied.has(x.id) ? ' applies' : ''}` },
            el('b', {}, x.id), applied.has(x.id) ? el('span', { class: 'small' }, ' · applies here') : null, el('br'), words(x),
            x.quote ? el('q', {}, x.quote) : null))) : null);
      }))),
    (s.holdings || []).length ? el('section', { class: 'instrument' }, el('h3', {}, 'The courts'),
      s.holdings.map((h) => el('p', { class: 'holding' }, el('b', {}, h.name), ` · ${h.citation} · ${h.date}`, el('br'),
        `${(s.courts || []).find((c) => c.id === h.court)?.name || h.court} read `, el('code', {}, h.term), ' as ', el('code', {}, h.reading), h.passage ? [': ', el('q', {}, h.passage)] : null, h.note ? [el('br'), el('span', { class: 'note-line' }, h.note)] : null))) : null);
  fill(pane, contents, body);
  if (S.provision) document.getElementById(`p-${S.provision}`)?.scrollIntoView({ block: 'center' });
}
