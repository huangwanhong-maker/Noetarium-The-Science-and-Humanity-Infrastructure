// The theory (ADR 0122): how the intervention is meant to bring about the outcome, drawn from one to the other. Each causal claim
// is a link that says how far it is shown: a full line with its grade in words and pips, or a dashed one; the strongest path is
// heavier, its weakest link named, and a gap that every path passes through named too. The question stands above it as a sentence
// whose parts can be changed. Whatever is chosen opens where it stands: a state with its words and where it leads; a claim with
// what it rests on, what it needs in the place, and the evidence set on it. The theory is kept in the policy domain's written form
// and read by the domain's reader: the page writes it back and never reads it.

import { layoutTheory, svgTheory, pips, writeTheory, addNode, setNode, removeNode, addLink, setLink, removeLink } from '/visualization/js/policy/index.js';
import { S, base, ensure, keep, load, begun, theory, levels, nodeText, bodyOn, bodyAnswer, isRatio, interval, words, statusOf, closeKey, designs } from './common.js';

const KINDS = ['intervention', 'activity', 'output', 'outcome', 'impact'];

export function theoryView() {
  return {
    id: 'theory', label: 'Theory', draw,
    // Whatever another person changed, the question is read again and the view in front drawn from it.
    heard: (e, ctx) => { if (e.what) { S.question = null; ctx.show(document.querySelector('.tab.on')?.dataset.tab || 'theory'); } },
  };
}

const levelsOf = (schemeText) => (String(schemeText).split('\n').find((l) => l.trim().split(/\s+/)[0] === 'levels') || '').trim().split(/\s+/).slice(1);

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('theory-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('the theory');
  const q = S.question, redraw = () => ctx.show('theory');
  if (!begun()) return fill(pane, begin(ctx, redraw));
  const change = async (f, what, note) => { await keep(ctx, f, what, note); redraw(); };
  const t = theory(), a = S.appraisal?.assessment;

  const head = el('header', { class: 'q-head' },
    el('input', { class: 'q-title', type: 'text', value: q.title || '', placeholder: 'What the question is called', spellcheck: false, 'aria-label': 'The question\'s title',
      onchange: (e) => change((x) => { x.title = e.target.value.trim(); }, 'named the question', e.target.value.trim()) }),
    q.about ? el('p', { class: 'about' }, q.about) : null);

  // A theory that does not read is mended in its written form.
  if (S.wrong) return fill(pane, head, el('p', { class: 'note wrong' }, S.wrong));
  if (!t || S.written) return fill(pane, head, writtenForm(ctx, redraw, !t));

  const pick = (label, value, set) => el('select', { class: 'q-pick', 'aria-label': label, onchange: (e) => set(e.target.value) },
    t.nodes.some((n) => n.id === value) ? null : el('option', { value: '', selected: true }, '… choose'),
    t.nodes.map((n) => el('option', { value: n.id, selected: n.id === value }, n.text || n.id)));
  const st = a?.verdict ? statusOf(a.verdict) : null;
  const sentence = el('p', { class: 'sentence' }, 'Does ',
    pick('The outcome asked about', q.outcome, (v) => change((x) => { x.outcome = v; }, 'asked about another outcome', v)), ' follow from ',
    pick('The intervention asked from', q.intervention, (v) => change((x) => { x.intervention = v; }, 'asked from another intervention', v)), ', shown at least ',
    el('select', { class: 'q-pick', 'aria-label': 'The least grade asked', onchange: (e) => change((x) => { x.least = e.target.value; }, 'asked for another grade', e.target.value) },
      t.levels.map((l) => el('option', { value: l, selected: l === (q.least || t.levels[0]) }, words(l)))), '?');
  const answer = a?.error ? el('p', { class: 'answer undecided' }, a.error)
    : a ? el('p', { class: `answer ${a.verdict.status}` }, el('b', {}, `${st.glyph} ${a.verdict.status === 'holds' ? 'It is shown' : a.verdict.status === 'fails' ? 'It is not shown' : 'Not yet shown'}`), ` — ${why(t, a, q)}.`,
      a.paths ? el('span', { class: 'small' }, ` ${a.paths} ${a.paths === 1 ? 'path leads' : 'paths lead'} from one to the other.`) : null) : null;
  const ask = a?.verdict ? `${nodeText(q.outcome)}: ${st.word}` : 'no question yet';
  ctx.note(ask);

  const room = Math.max(320, pane.clientWidth - 46);
  const layout = layoutTheory(t, a, { room, minHeight: 190 });
  const k = Math.max(0.84, Math.min(1, room / layout.width));
  const canvas = el('div', { class: 'canvas', style: { width: `${Math.ceil(layout.width * k)}px`, height: `${Math.ceil(layout.height * k)}px` } });
  const drawn = el('div', { class: 'drawn', style: { width: `${Math.ceil(layout.width)}px`, height: `${Math.ceil(layout.height)}px`, transform: `scale(${k})` } });
  drawn.innerHTML = svgTheory(t, a, layout, { selected: S.chosen, intervention: q.intervention, outcome: q.outcome });
  canvas.append(drawn);
  canvas.addEventListener('click', (e) => {
    if (e.target.closest('.card')) return;
    const n = e.target.closest('[data-node]'), l = e.target.closest('[data-link]');
    const key = n ? `n:${n.dataset.node}` : l ? `l:${l.dataset.link}` : null;
    S.chosen = key && key !== S.chosen ? key : null;
    redraw();
  });
  if (S.chosen) {
    const W = 348, wide = Math.max(layout.width * k, room);
    let box = null, x = 0, y = 0, w = 0;
    if (S.chosen.startsWith('n:')) {
      const p = layout.at.get(S.chosen.slice(2));
      if (p) { box = nodeCard(ctx, S.chosen.slice(2), change); x = p.x * k; y = p.y * k; w = p.w * k; }
    } else {
      const d = layout.links.find((z) => z.index === Number(S.chosen.slice(2)));
      if (d) { box = linkCard(ctx, d.index, change); x = d.mx * k - 50; y = d.my * k + 6; w = 100; }
    }
    if (box) {
      const right = x + w + 14 + W <= wide;
      Object.assign(box.style, { left: `${Math.max(8, right ? x + w + 14 : x - W - 14)}px`, top: `${Math.max(0, y - 6)}px`, width: `${W}px` });
      canvas.append(box);
      canvas.style.minHeight = `${Math.ceil(y + 420)}px`;
    }
  }

  const help = [];
  const gaps = a?.gaps || [];
  if (gaps.length > 2) {
    help.push(el('li', {}, `${gaps.length === t.links.length ? `None of the ${gaps.length} claims` : `${gaps.length} claims that every path passes through are not shown: none of them`} has evidence that shows it. `,
      'A chain is no better shown than its least shown claim, so each is a gap; begin with the one most in doubt.'));
  } else {
    for (const i of gaps) {
      const l = t.links[i];
      help.push(el('li', {}, 'Evidence for ', el('button', { type: 'button', class: 'key', onclick: () => { S.chosen = `l:${i}`; redraw(); } }, `${nodeText(l.from)} → ${nodeText(l.to)}`),
        ' would help most: every path passes through it, and it is not shown.'));
    }
  }
  for (const f of t.faults || []) help.push(el('li', { class: 'wrong' }, `The body of evidence ${f.body} is set on no claim: ${f.what.replace(/^policy::evidence::attach: /, '')}.`));
  const implied = a?.implied ? el('p', { class: 'implied' }, `Where every claim on the way states a linear effect, the ${a.paths === 1 ? 'path implies' : 'paths together imply'} an effect of `,
    el('b', {}, interval(a.implied.effect, a.implied.lower, a.implied.upper, false)), ' on the outcome (Wright\'s rule: the products of the effects along each path, summed)',
    a.shared ? '; a claim lies on more than one path, so the interval is an enclosure and not exact.' : '.') : null;
  const n = t.levels.length;
  const strong = a?.strongest?.links || [];
  const order = [...strong, ...t.links.map((_, i) => i).filter((i) => !strong.includes(i))];
  const claims = el('section', { class: 'claims' }, el('h4', {}, strong.length ? 'The claims, the strongest path first' : 'The claims'),
    order.map((i) => {
      const l = t.links[i], id = bodyOn(l), b = id ? bodyAnswer(id) : null;
      return el('button', { type: 'button', class: `claim ${l.rank >= 0 ? 'shown' : 'unshown'}${S.chosen === `l:${i}` ? ' on' : ''}${strong.includes(i) ? ' strong' : ''}`,
        onclick: () => { S.chosen = S.chosen === `l:${i}` ? null : `l:${i}`; redraw(); } },
        el('span', { class: 'grade' }, el('span', { class: 'pips' }, pips(l.rank, n)), ` ${l.rank >= 0 ? words(l.level) : l.evidence?.given ? 'not shown' : 'no evidence'}`),
        el('span', { class: 'what' }, el('span', { class: 'ab' }, `${nodeText(l.from)} → ${nodeText(l.to)}`),
          el('span', { class: 'small' }, `It rests on: ${(l.assumption || 'nothing stated').replace(/[.\s]+$/, '')}.${l.support.length ? ` It needs: ${l.support.map((f) => f.replace(/[.\s]+$/, '')).join('; ')}.` : ''}`)),
        el('span', { class: 'evidence small' }, b && !b.error ? `${b.pooled.k} ${b.pooled.k === 1 ? 'study' : 'studies'}: ${interval(b.pooled.estimate, b.pooled.lower, b.pooled.upper, isRatio(b.measure))}${isRatio(b.measure) ? ', a ratio' : ''}`
          : l.evidence?.given ? `stated: ${interval(l.evidence.effect, l.evidence.lower, l.evidence.upper, false)}` : 'no studies',
          i === a?.strongest?.limiting && strong.length ? el('i', {}, ' · the weakest link') : null, (a?.gaps || []).includes(i) ? el('i', {}, ' · every path passes through it') : null));
    }));
  fill(pane, head, sentence, answer,
    el('div', { class: 'ground' }, canvas),
    el('p', { class: 'legend' },
      el('span', { class: 'l-full' }, 'shown'), el('span', { class: 'l-dash' }, 'not shown'), el('span', { class: 'l-heavy' }, 'the strongest path'),
      el('span', {}, t.levels.map((l, i) => `${pips(i, n)} ${words(l)}`).join('   ')),
      el('button', { type: 'button', class: 'key', onclick: () => { S.written = true; S.chosen = null; redraw(); } }, 'The written form')),
    help.length ? el('ul', { class: 'help' }, help) : null, implied, claims);
}

// Why the question is answered as it is, in the states' own words.
function why(t, a, q) {
  if (!a.paths || !a.strongest) return a.verdict.reason;
  const lim = t.links[a.strongest.limiting], claim = `“${nodeText(lim.from)} → ${nodeText(lim.to)}”`;
  const which = a.paths === 1 ? 'the one path' : `the strongest of ${a.paths} paths`;
  if (a.verdict.status === 'holds') return `${which} is shown at ${words(a.strongest.weakest)} or better at every claim; its weakest is ${claim}, shown at ${words(lim.level)}`;
  if (a.strongest.weakest === null) return `${a.paths === 1 ? 'the one path has' : 'every path has'} a claim that is not shown; on ${a.paths === 1 ? 'it' : 'the strongest'}, ${claim} ${lim.evidence?.given ? 'has evidence that does not show it' : 'has no evidence'}`;
  return `${which} is shown only at ${words(a.strongest.weakest)}, below the ${words(q.least)} asked; its weakest claim is ${claim}`;
}

// ---- What opens beside a chosen state -------------------------------------------------------------------------------------------

function nodeCard(ctx, id, change) {
  const { el } = ctx;
  const t = S.read.theory, q = S.question, n = t.nodes.find((x) => x.id === id);
  if (!n) return null;
  const redraw = () => ctx.show('theory');
  const edit = (f, what, note, also) => change((x) => { x.theory = writeTheory(f(t)); also?.(x); }, what, note);
  const others = t.nodes.filter((x) => x.id !== id && !t.links.some((l) => l.from === id && l.to === x.id));
  const fresh = el('input', { type: 'text', placeholder: 'a new state it leads to…', 'aria-label': 'A new state this leads to',
    onchange: (e) => { const text = e.target.value.trim(); if (text) { const made = addNode(t, { text, kind: 'outcome', after: id }); S.chosen = `n:${made.id}`; change((x) => { x.theory = writeTheory(made.theory); }, 'added a state', text); } } });
  return el('aside', { class: 'card' }, closeKey(el, () => { S.chosen = null; redraw(); }),
    el('div', { class: 'kicker-row' },
      el('select', { 'aria-label': 'The kind of state', onchange: (e) => edit((x) => setNode(x, id, { kind: e.target.value }), 'changed a state\'s kind', `${id}: ${e.target.value}`) },
        [...new Set([...KINDS, n.kind])].map((k) => el('option', { value: k, selected: k === n.kind }, k))),
      el('code', {}, id)),
    el('textarea', { class: 'words', rows: 2, 'aria-label': 'The state, in words', spellcheck: true,
      onchange: (e) => edit((x) => setNode(x, id, { text: e.target.value }), 'reworded a state', id) }, n.text),
    el('div', { class: 'keys' },
      q.intervention === id ? el('span', { class: 'small' }, 'The question is asked from here.') : el('button', { type: 'button', class: 'key edged', onclick: () => change((x) => { x.intervention = id; }, 'asked from another intervention', id) }, 'Ask from here'),
      q.outcome === id ? el('span', { class: 'small' }, 'The question is asked about this.') : el('button', { type: 'button', class: 'key edged', onclick: () => change((x) => { x.outcome = id; }, 'asked about another outcome', id) }, 'Ask about this')),
    el('h4', {}, 'It leads to'),
    el('ul', { class: 'plain' }, t.links.map((l, i) => (l.from === id ? el('li', {}, el('button', { type: 'button', class: 'key', onclick: () => { S.chosen = `l:${i}`; redraw(); } }, `→ ${nodeText(l.to)}`)) : null))),
    el('div', { class: 'row' }, others.length ? el('select', { 'aria-label': 'A state this leads to', onchange: (e) => { if (e.target.value) edit((x) => addLink(x, id, e.target.value), 'stated a causal claim', `${id} -> ${e.target.value}`); } },
      el('option', { value: '' }, '+ a claim that it leads to…'), others.map((o) => el('option', { value: o.id }, o.text || o.id))) : null),
    el('div', { class: 'row' }, fresh),
    el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key', onclick: () => { S.chosen = null; edit((x) => removeNode(x, id), 'removed a state', id, (x) => {
      for (const b of x.bodies || []) if (b.link?.from === id || b.link?.to === id) b.link = {};
      if (x.intervention === id) x.intervention = '';
      if (x.outcome === id) x.outcome = '';
    }); } }, 'Remove this state and its claims')));
}

// ---- What opens beside a chosen claim -------------------------------------------------------------------------------------------

function linkCard(ctx, index, change) {
  const { el } = ctx;
  const t = S.read.theory, l = t.links[index], shown = theory().links[index];
  if (!l) return null;
  const redraw = () => ctx.show('theory');
  const edit = (f, what, note, also) => change((x) => { x.theory = writeTheory(f(t)); also?.(x); }, what, note);
  const ab = `${l.from} -> ${l.to}`, bodyId = bodyOn(l), body = bodyId ? bodyAnswer(bodyId) : null;
  const n = levels().length;
  const add = el('input', { type: 'text', placeholder: '+ what else must be present for it to work…', 'aria-label': 'A support factor to add',
    onchange: (e) => { const f = e.target.value.trim(); if (f && !l.support.includes(f)) edit((x) => setLink(x, index, { support: [...l.support, f] }), 'named a support factor', `${ab}: ${f}`); } });
  const newBody = () => {
    const id = `${l.to}-evidence`.replace(/_/g, '-');
    let name = id;
    for (let k = 2; (S.question.bodies || []).some((b) => b.id === name); k++) name = `${id}-${k}`;
    S.body = name;
    change((x) => { (x.bodies ||= []).push({ id: name, link: { from: l.from, to: l.to }, claim: `${nodeText(l.from)} → ${nodeText(l.to)}`, design: designs().includes('randomised') ? 'randomised' : designs()[0] || 'randomised', measure: 'as_given',
      higher_is_better: l.sign >= 0, confidence: 0.95, assessed: {}, studies: [] }); }, 'began a body of evidence', ab).then(() => ctx.show('evidence'));
  };
  return el('aside', { class: 'card' }, closeKey(el, () => { S.chosen = null; redraw(); }),
    el('h3', {}, `${nodeText(l.from)} → ${nodeText(l.to)}`),
    el('p', { class: `standing ${shown.rank >= 0 ? 'shown' : 'unshown'}` }, el('span', { class: 'pips' }, pips(shown.rank, n)), ' ', el('b', {}, shown.rank >= 0 ? `shown at ${words(shown.level)}` : 'not shown'),
      shown.rank >= 0 ? null : el('span', { class: 'small' }, ` — ${shown.why.replace(`${ab} `, '').replace(/^has no evidence$/, 'no evidence is set on it')}`)),
    el('h4', {}, 'The claim rests on'),
    el('textarea', { class: 'words', rows: 2, 'aria-label': 'The assumption the claim rests on', onchange: (e) => edit((x) => setLink(x, index, { assumption: e.target.value }), 'restated an assumption', ab) }, l.assumption),
    el('div', { class: 'row' }, el('span', {}, 'The cause is meant to'), el('select', { 'aria-label': 'The side the theory expects', onchange: (e) => edit((x) => setLink(x, index, { sign: Number(e.target.value) }), 'changed the side a claim expects', `${ab}: ${e.target.value > 0 ? 'raise' : 'lower'}`) },
      el('option', { value: 1, selected: l.sign >= 0 }, 'raise what it leads to'), el('option', { value: -1, selected: l.sign < 0 }, 'lower what it leads to'))),
    el('h4', {}, 'It needs, in the place'),
    el('ul', { class: 'factors' }, l.support.map((f) => el('li', {}, f, el('button', { type: 'button', class: 'close', title: 'It does not need this', 'aria-label': `Remove the support factor ${f}`,
      onclick: () => edit((x) => setLink(x, index, { support: l.support.filter((g) => g !== f) }), 'removed a support factor', `${ab}: ${f}`) }, '×'))),
      l.support.length ? null : el('li', { class: 'none' }, 'No support factor is named.')),
    add,
    el('h4', {}, 'Its evidence'),
    body && !body.error ? el('p', {}, `${body.pooled.k} ${body.pooled.k === 1 ? 'study gives' : 'studies pooled give'} `, el('b', {}, interval(body.pooled.estimate, body.pooled.lower, body.pooled.upper, isRatio(body.measure))),
      isRatio(body.measure) ? ' as a ratio' : '', ', graded ', el('b', {}, words(body.grade)), '.')
      : body ? el('p', { class: 'wrong' }, body.error)
        : l.evidence?.given ? el('p', {}, 'Written on the theory itself: ', el('b', {}, interval(l.evidence.effect, l.evidence.lower, l.evidence.upper, false)), `, ${words(l.evidence.grade)}. No studies are kept for it here.`)
          : el('p', { class: 'small' }, 'No studies are set on this claim: it stands as not shown, whatever is believed of it.'),
    el('div', { class: 'keys' },
      bodyId ? el('button', { type: 'button', class: 'key main', onclick: () => { S.body = bodyId; ctx.show('evidence'); } }, 'Open its evidence') : el('button', { type: 'button', class: 'key main', onclick: newBody }, '+ a body of evidence'),
      el('button', { type: 'button', class: 'key', onclick: () => { S.chosen = null; edit((x) => removeLink(x, index), 'withdrew a causal claim', ab, (x) => { for (const b of x.bodies || []) if (b.link?.from === l.from && b.link?.to === l.to) b.link = {}; }); } }, 'Withdraw the claim')));
}

// ---- The written form -------------------------------------------------------------------------------------------------------------

function writtenForm(ctx, redraw, must) {
  const { el, fill } = ctx;
  const q = S.question;
  const said = el('div', { class: 'reads' });
  const area = el('textarea', { class: 'written', rows: Math.min(30, Math.max(12, String(q.theory).split('\n').length + 2)), spellcheck: false, 'aria-label': 'The theory in its written form' }, q.theory);
  let timer = null, ok = false;
  const check = async () => {
    try {
      const r = await ctx.api('/api/theory', { method: 'POST', body: { text: area.value } });
      ok = r.ok;
      fill(said, r.ok ? el('p', { class: 'answer holds' }, `✓ It reads: ${r.theory.nodes.length} states and ${r.theory.links.length} causal claims.`)
        : el('ul', { class: 'help' }, r.errors.map((e) => el('li', { class: 'wrong' }, e))));
    } catch (error) { ok = false; fill(said, el('p', { class: 'wrong' }, error.message)); }
  };
  area.addEventListener('input', () => { clearTimeout(timer); timer = setTimeout(check, 350); });
  area.addEventListener('keydown', (e) => { if (e.key === 'Tab') { e.preventDefault(); area.setRangeText('  ', area.selectionStart, area.selectionEnd, 'end'); } });
  check();
  const keepIt = async () => {
    await check();
    if (!ok) return ctx.say('The theory does not read yet: see the lines named beneath it.');
    S.written = false;
    await keep(ctx, (x) => { x.theory = area.value.endsWith('\n') ? area.value : `${area.value}\n`; }, 'rewrote the theory', '');
    redraw();
  };
  return el('section', { class: 'written-form' },
    el('h2', {}, must ? 'The theory does not read' : 'The theory, in its written form'),
    el('p', { class: 'lede' }, 'A line each: ', el('code', {}, 'node id kind: words'), ', ', el('code', {}, 'link a -> b: the assumption it rests on'), ', ', el('code', {}, 'support a -> b: what else must be present'), ', ',
      el('code', {}, 'sign a -> b: -'), ' where the cause is meant to lower its effect, and ', el('code', {}, 'evidence a -> b: 0.12 [0.05, 0.19] moderate'), ' for evidence stated without its studies. The levels are the grading scheme\'s.'),
    area, said,
    el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key main', onclick: keepIt }, 'Keep it'),
      must ? null : el('button', { type: 'button', class: 'key', onclick: () => { S.written = false; redraw(); } }, 'Leave it as it was')),
    draftBox(ctx, redraw));
}

// ---- A model's draft ---------------------------------------------------------------------------------------------------------------

function draftBox(ctx, redraw) {
  const { el, fill } = ctx;
  const out = el('div', { class: 'draft-out' });
  const show = () => fill(out, S.drafts.slice(-2).reverse().map((d) => {
    const fig = el('div', { class: 'draft-fig' });
    fig.innerHTML = svgTheory(d.theory, null, layoutTheory(d.theory, null, { nodeW: 170, gapX: 110 }));
    return el('article', { class: 'draft' },
      el('p', { class: 'small' }, `A draft by ${d.by}, for: “${d.description}”. A proposal: it carries no evidence, and every claim in it stands as not shown.`), fig,
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key main', onclick: async () => {
        const first = d.theory.nodes.find((n) => n.kind === 'intervention') || d.theory.nodes[0];
        const last = [...d.theory.nodes].reverse().find((n) => n.kind === 'impact') || [...d.theory.nodes].reverse().find((n) => n.kind === 'outcome') || d.theory.nodes.at(-1);
        const scheme = S.schemes[0];
        S.written = false;
        await keep(ctx, (x) => {
          if (!String(x.scheme || '').trim() && scheme) { x.scheme = scheme.text; x.rules = scheme.rules; }
          const lv = levelsOf(x.scheme);
          x.theory = lv.length ? d.text.replace(/^levels .*$/m, `levels ${lv.join(' ')}`) : d.text;
          x.intervention = first.id; x.outcome = last.id; x.least ||= lv[0] || '';
          x.title ||= ctx.project?.title || '';
          for (const b of x.bodies || []) b.link = {};
        }, 'took a model\'s draft as the theory', `${d.theory.nodes.length} states, ${d.theory.links.length} claims`);
        redraw();
      } }, 'Take this draft as the theory')));
  }));
  const text = el('textarea', { class: 'describe', rows: 3, placeholder: 'Describe the intervention and what it is for, in a sentence or two…', 'aria-label': 'The intervention described, for a draft' });
  const askIt = async (e) => {
    e.target.disabled = true;
    ctx.say('Asking a model for a draft…');
    try {
      const a = await ctx.api(`${base(ctx)}/draft`, { method: 'POST', body: { description: text.value }, run: true });
      S.drafts.push(a.draft);
      ctx.log(`A theory drafted by ${a.draft.by}: ${a.draft.theory.nodes.length} states and ${a.draft.theory.links.length} causal claims. A proposal, with no evidence.`);
      show();
    } catch (error) { ctx.say(error.message); ctx.log(error.message); }
    e.target.disabled = false;
  };
  show();
  return el('section', { class: 'draft-box' },
    el('h3', {}, 'A draft, proposed by a language model'),
    el('p', { class: 'small' }, 'A model may draft the states and the causal claims from a description. What it drafts is a proposal: the domain\'s reader reads it or it is refused, it states no evidence, and nothing of it is kept until you take it.'),
    text, el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: askIt }, 'Ask for a draft')), out);
}

// ---- A project with no question yet ---------------------------------------------------------------------------------------------

function begin(ctx, redraw) {
  const { el } = ctx;
  const lay = async (t) => {
    ctx.say('Laying out…');
    try {
      const a = await ctx.api(`${base(ctx)}/example`, { method: 'POST', body: { template: t.id } });
      ctx.log(`${a.question} laid out.`);
      await load(ctx);
    } catch (error) { ctx.say(error.message); }
    redraw();
  };
  const what = el('input', { type: 'text', placeholder: 'What is done: the intervention', 'aria-label': 'The intervention' });
  const whatFor = el('input', { type: 'text', placeholder: 'What it is for: the outcome', 'aria-label': 'The outcome' });
  const by = el('select', { 'aria-label': 'The grading scheme' }, S.schemes.map((s) => el('option', { value: s.id }, s.title)));
  const start = async () => {
    const a = what.value.trim(), b = whatFor.value.trim(), scheme = S.schemes.find((s) => s.id === by.value) || S.schemes[0];
    if (!a || !b) return ctx.say('Say what is done and what it is for.');
    if (!scheme) return ctx.say('No plug-in brings a grading scheme.');
    const lv = levelsOf(scheme.text);
    const one = addNode({ name: a, levels: lv, nodes: [], links: [] }, { text: a, kind: 'intervention' });
    const two = addNode(one.theory, { text: b, kind: 'outcome', after: one.id });
    await keep(ctx, (x) => { Object.assign(x, { title: x.title || ctx.project?.title || '', theory: writeTheory(two.theory), intervention: one.id, outcome: two.id, least: lv[0] || '', scheme: scheme.text, rules: scheme.rules }); },
      'began a question', `${a} → ${b}`);
    redraw();
  };
  return el('section', { class: 'begin' },
    el('h2', {}, 'A question of policy'),
    el('p', { class: 'lede' }, 'A question asks whether an outcome follows from an intervention. It is worked from the theory of change (how the one is meant to bring about the other, claim by claim) through the studies for each claim, pooled and graded by a scheme you declare, and the argument that what worked there will work here, to the options and the decision: adopt, reject, or study first.'),
    S.templates.length ? el('div', { class: 'choices' }, el('h3', {}, 'An example, laid out'), S.templates.map((t) => el('button', { type: 'button', class: 'choice', onclick: () => lay(t) }, el('b', {}, t.title), el('span', {}, t.about)))) : null,
    el('div', { class: 'own' }, el('h3', {}, 'A question of one\'s own'),
      el('div', { class: 'own-row' }, what, el('span', { class: 'arrow' }, '→'), whatFor),
      el('div', { class: 'own-row' }, el('span', { class: 'small' }, 'its evidence graded by'), by, el('button', { type: 'button', class: 'key main', onclick: start }, 'Begin')),
      el('p', { class: 'small' }, 'A scheme is a declaration: its levels, where each design starts, and what lowers or raises a grade. None here is any institution\'s own text.')),
    draftBox(ctx, redraw));
}
