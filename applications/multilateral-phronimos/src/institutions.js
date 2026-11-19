// Institutions (ADR 0134): the bodies the parties belong to, how they nest and overlap, and the reciprocity the parties keep. The lattice
// of memberships is the page: each group of parties with every institution they all share, the institutions introduced above a node and
// the parties below, and the regularities of membership in words ("every member of … belongs to …"). Then each issue's complex, its
// institutions nested, overlapping or apart, and the parties that can choose their forum; then what the parties gave one another, read
// over windows of several lengths: whether every pair balances (specific reciprocity) or only every party against the group (diffuse).

import { svgLattice, svgReciprocity } from '/visualization/js/multilateral/index.js';
import { S, ensure, begun, names, name } from './common.js';

export function institutionsView() {
  return { id: 'institutions', label: 'Institutions', draw };
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('institutions-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'note' }, 'No project is open.'));
  await ensure(ctx);
  ctx.here('institutions');
  if (!begun()) return fill(pane, el('p', { class: 'note' }, 'Begin on the round.'));
  const a = S.answer?.institutions;
  const why = S.answer?.wrong?.institutions;
  if (!a) return fill(pane, el('p', { class: 'note' }, why ? `The engine could not read the institutions: ${why}` : 'The record names no institutions. Give it a landscape: the parties, the institutions with their issues, and who belongs to which, since when.'));
  if (a.problems?.length) return fill(pane, el('ul', { class: 'mp-signs' }, a.problems.map((p) => el('li', {}, `${p.where}: ${p.what}`))));
  const instName = Object.fromEntries((a.institutions || []).map((i) => [i.id, i.name || i.id]));
  const w = Math.max(620, Math.min(1000, pane.clientWidth - 48));
  const lattice = el('div', { class: 'mp-fig' });
  lattice.innerHTML = svgLattice(a, { names: names(), labels: instName, width: w });
  ctx.note(`${(a.institutions || []).length} institutions · ${(a.lattice?.groups || []).length} groups · ${(a.regularities || []).length} regularities`);
  const complexes = el('section', { class: 'mp-groups' }, (a.complexes || []).map((c) => el('div', { class: 'group' },
    el('h4', {}, `On ${c.issue}`),
    c.pairs.length ? el('ul', {}, c.pairs.map((p) => el('li', {}, p.relation === 'nested' ? `${instName[p.inner]} lies within ${instName[p.inner === p.a ? p.b : p.a]}` :
      p.relation === 'overlapping' ? `${instName[p.a]} and ${instName[p.b]} overlap, sharing ${p.shared}` : p.relation === 'apart' ? `${instName[p.a]} and ${instName[p.b]} share no member` :
        `${instName[p.a]} and ${instName[p.b]} have the same members`))) : el('p', { class: 'small' }, `${c.institutions.map((x) => instName[x]).join(', ') || 'No institution'} alone.`),
    c.forums.length ? el('p', { class: 'small' }, `Who can choose a forum: ${c.forums.map((f) => `${name(f.party)} (${f.institutions.length})`).join(', ')}.`) : null)));
  const design = el('table', { class: 'mp-design' },
    el('thead', {}, el('tr', {}, el('th', {}, 'institution'), ...['membership', 'scope', 'centralisation', 'control', 'flexibility'].map((d) => el('th', {}, d)))),
    el('tbody', {}, (a.institutions || []).map((i) => el('tr', {}, el('td', {}, el('b', {}, i.name || i.id), el('br'), el('span', { class: 'small' }, `${i.kind || ''}${i.members?.length ? ` · ${i.members.length} members` : ''}`)),
      ...['membership', 'scope', 'centralisation', 'control', 'flexibility'].map((d) => el('td', { class: i.design?.[d] ? '' : 'small' }, i.design?.[d] || 'not declared'))))));
  let reciprocity = null;
  if (a.reciprocity?.length) {
    reciprocity = el('div', { class: 'mp-fig' });
    reciprocity.innerHTML = svgReciprocity(a, { width: Math.min(640, w) });
  }
  fill(pane,
    el('header', { class: 'mp-head' }, el('p', { class: 'sentence' }, 'The institutions the parties belong to, and what they keep with one another'),
      el('p', { class: 'small' }, 'Each node is a group of parties with every institution they all share; an institution is named where it first applies, a party where it first belongs. Read on the date the page reads.')),
    lattice,
    el('div', { class: 'mp-two' },
      el('section', {}, el('h4', { class: 'mp-sub' }, 'Each issue’s complex'), complexes),
      el('section', {}, el('h4', { class: 'mp-sub' }, 'Reciprocity'), reciprocity || el('p', { class: 'small' }, 'No transfers are recorded.'),
        el('p', { class: 'small' }, 'Over short windows few exchanges balance; reciprocity that balances only over the group and a long window is diffuse, the kind Ruggie finds proper to multilateralism.'))),
    el('h4', { class: 'mp-sub' }, 'Their design, as declared'), design);
}
