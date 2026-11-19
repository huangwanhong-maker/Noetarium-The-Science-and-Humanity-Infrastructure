// Negotiation (ADR 0132): where members stand, and what they bargain over. Ideal points read from the record's votes, members on one line
// with their intervals when asked (a summary of how they divided under one dimension, never of what they want). A bargaining problem
// between two parties, its outcomes stated as pairs of utilities each party declares, and three solutions on its frontier: Nash's,
// Kalai and Smorodinsky's, the egalitarian one, each with the axiom it keeps and the one it gives up. And issues divided between two
// parties who each spread 100 points over them, by Adjusted Winner: equal points, no envy, nothing better for one that is no worse for
// the other. What is stated here is kept in the record (its `negotiation`).

import { svgLine, svgBargaining, figure } from '/visualization/js/multilateral/index.js';
import { S, ensure, keep, begun, base, again } from './common.js';

export function negotiationView() {
  return { id: 'negotiation', label: 'Negotiation', draw };
}

const DEFAULT = {
  parties: ['the upstream states', 'the delta'],
  outcomes: [[0, 0], [10, 0], [8, 4], [5, 7], [0, 8]],
  disagreement: [1, 1],
  issues: ['the dredging', 'the minimum flow', 'the monitoring', 'the tariffs'],
  a: [10, 40, 20, 30],
  b: [45, 30, 15, 10],
};

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('negotiation-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('negotiation');
  const redraw = again(ctx, 'negotiation');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin on the round.'));
  const r = S.record, n = { ...DEFAULT, ...(r.negotiation || {}) };
  const setN = (f, what, note) => keep(ctx, (x) => { x.negotiation = { ...DEFAULT, ...(x.negotiation || {}) }; f(x.negotiation); }, what, note).then(redraw);

  // ---- where members stand -----------------------------------------------------------------------------------------------------
  const line = el('section', { class: 'mp-votes' });
  if (r.votes?.members?.length) {
    const box = el('div', { class: 'mp-fig' }, el('p', { class: 'small' }, 'Reading the votes…'));
    const ask = (bootstrap) => ctx.api(`${base(ctx)}/votes`, { method: 'POST', body: { bootstrap } }).then((a) => {
      box.innerHTML = svgLine(a.answer, { names: a.names, width: Math.max(640, Math.min(1000, pane.clientWidth - 48)) });
      box.append(el('p', { class: 'small' }, `${r.votes.members.length} members, ${r.votes.items.length} votes after the unanimous are set aside; ${a.answer.converged ? 'converged' : 'not converged'} in ${a.answer.iterations} rounds. ${a.acknowledgment || ''}`));
    }).catch((e) => { box.textContent = e.message; });
    ask(0);
    line.append(el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key edged', onclick: (e) => { e.target.disabled = true; box.firstChild && (box.firstChild.textContent = 'Drawing votes from the fitted line, two hundred times…'); ask(200); } }, 'Give each its interval')), box);
  } else {
    line.append(el('p', { class: 'note' }, 'The record holds no votes. A body\'s recorded votes (the UN General Assembly\'s, from the file the Dag Hammarskjöld Library publishes, read by data/products/multilateral_cases.py) place its members on a line.'));
  }

  // ---- a bargaining problem ------------------------------------------------------------------------------------------------------
  const bar = el('section', { class: 'mp-bargain' });
  const pairs = el('textarea', { rows: 6, spellcheck: false, 'aria-label': 'The outcomes, one pair of utilities a line' }, n.outcomes.map((p) => p.join(' ')).join('\n'));
  const dA = el('input', { type: 'number', step: 'any', value: n.disagreement[0], class: 'mp-num', 'aria-label': 'What the first party has without agreement' });
  const dB = el('input', { type: 'number', step: 'any', value: n.disagreement[1], class: 'mp-num', 'aria-label': 'What the second party has without agreement' });
  const pA = el('input', { type: 'text', value: n.parties[0], class: 'mp-name', 'aria-label': 'The first party' });
  const pB = el('input', { type: 'text', value: n.parties[1], class: 'mp-name', 'aria-label': 'The second party' });
  const fig = el('div', { class: 'mp-fig' }, el('p', { class: 'small' }, 'Solving…'));
  const said = el('div', {});
  ctx.api('/api/bargaining', { method: 'POST', body: { outcomes: n.outcomes, disagreement: n.disagreement } }).then((a) => {
    fig.innerHTML = svgBargaining(a, { parties: n.parties, width: 440, height: 360 });
    const pt = (p) => `(${figure(p[0])}, ${figure(p[1])})`;
    said.append(
      el('p', {}, el('b', {}, `◆ Nash ${pt(a.nash)}. `), 'The greatest product of the gains: Pareto optimal, symmetric, unchanged by a change of either party\'s scale, and independent of outcomes that were not chosen.'),
      el('p', {}, el('b', {}, `▲ Kalai–Smorodinsky ${pt(a.kalai_smorodinsky)}. `), 'On the line to the ideal point: a party whose opportunities grow never loses, at the price of independence.'),
      el('p', {}, el('b', {}, `● Egalitarian ${pt(a.egalitarian)}. `), 'Equal gains: it compares the parties\' utilities, so it needs them on one scale, which the others do not.'),
      el('p', { class: 'small' }, 'Each presupposes utilities the parties declare; they seldom do, so these are tools for a declared model, not a measure of what was gained.'));
  }).catch((e) => { fig.textContent = e.message; });
  bar.append(el('div', { class: 'mp-two' },
    el('div', {},
      el('p', { class: 'sentence small-sentence' }, 'Between ', pA, ' and ', pB, ', the outcomes they can reach together (each line a pair of their utilities):'), pairs,
      el('p', { class: 'sentence small-sentence' }, 'Without agreement they have ', dA, ' and ', dB, '.'),
      el('div', { class: 'keys' }, el('button', { type: 'button', class: 'key main', onclick: () => {
        const outcomes = pairs.value.split('\n').map((l) => l.trim().split(/[\s,;]+/).map(Number)).filter((p) => p.length === 2 && p.every(Number.isFinite));
        if (outcomes.length < 2) return ctx.say('State at least two outcomes, each a pair of numbers.');
        setN((x) => { x.outcomes = outcomes; x.disagreement = [Number(dA.value) || 0, Number(dB.value) || 0]; x.parties = [pA.value || 'party A', pB.value || 'party B']; }, 'stated a bargaining problem', `${outcomes.length} outcomes`);
      } }, 'Solve it')), said),
    fig));

  // ---- issues divided ------------------------------------------------------------------------------------------------------------
  const div = el('section', { class: 'mp-divide' });
  const rows = n.issues.map((iss, k) => ({ iss: el('input', { type: 'text', value: iss, class: 'mp-name' }), a: el('input', { type: 'number', value: n.a[k], class: 'mp-num' }), b: el('input', { type: 'number', value: n.b[k], class: 'mp-num' }) }));
  const table = el('table', { class: 'mp-table' }, el('thead', {}, el('tr', {}, el('th', {}, 'issue'), el('th', {}, `${n.parties[0]}'s points`), el('th', {}, `${n.parties[1]}'s points`), el('th', {}, 'the division'))),
    el('tbody', {}, rows.map((x) => el('tr', {}, el('td', {}, x.iss), el('td', {}, x.a), el('td', {}, x.b), el('td', { class: 'share' })))));
  const result = el('p', { class: 'said' });
  ctx.api('/api/division', { method: 'POST', body: { issues: n.issues, a: n.a, b: n.b } }).then((d) => {
    d.shares.forEach((s, k) => {
      const cell = table.querySelectorAll('td.share')[k];
      const share = s.a >= 0.999 ? `to ${n.parties[0]}` : s.a <= 0.001 ? `to ${n.parties[1]}` : `split: ${Math.round(100 * s.a)}% to ${n.parties[0]}, ${Math.round(100 * s.b)}% to ${n.parties[1]}`;
      cell.append(el('span', { class: `sh${s.split ? ' split' : ''}` }, share));
    });
    result.append(`Each holds ${figure(d.points_a)} of its own 100 points: equal, so neither envies the other, and no other division, split or not, is better for one and no worse for the other.`);
  }).catch((e) => { result.textContent = e.message; });
  div.append(el('p', { class: 'small' }, 'Each party spreads 100 points over the issues; each issue goes first to the party that values it more, then issues pass from the party ahead until the points are equal, one issue split (Brams and Taylor 1996).'),
    table, result,
    el('div', { class: 'keys' },
      el('button', { type: 'button', class: 'key', onclick: () => setN((x) => { x.issues = [...x.issues, `issue ${x.issues.length + 1}`]; x.a = [...x.a, 0]; x.b = [...x.b, 0]; }, 'added an issue') }, 'Add an issue'),
      el('button', { type: 'button', class: 'key main', onclick: () => {
        const a = rows.map((x) => Number(x.a.value) || 0), b = rows.map((x) => Number(x.b.value) || 0);
        const ta = a.reduce((s, v) => s + v, 0), tb = b.reduce((s, v) => s + v, 0);
        if (Math.abs(ta - 100) > 1e-6 || Math.abs(tb - 100) > 1e-6) return ctx.say(`Each party spreads 100 points: they spread ${ta} and ${tb}.`);
        setN((x) => { x.issues = rows.map((y) => y.iss.value || 'an issue'); x.a = a; x.b = b; }, 'restated the issues and their points');
      } }, 'Divide')));

  fill(pane,
    el('header', { class: 'mp-head' }, el('p', { class: 'sentence' }, 'Where members stand, and what they bargain over'),
      el('p', { class: 'small' }, 'Votes record public positions, and utilities are declared: what follows here is read from what was stated, and said to be.')),
    el('h4', { class: 'mp-sub' }, 'Where members stand'), line,
    el('h4', { class: 'mp-sub' }, 'A bargain between two parties'), bar,
    el('h4', { class: 'mp-sub' }, 'Issues divided between two parties'), div);
}

