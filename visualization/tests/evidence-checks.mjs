// Checks of the evidence drawings (visualization/js/evidence), ADR 0121. On an invented park bylaw stated as a scheme and what the
// law engine answered of a matter: the graph read off the scheme backwards from the question (a rule, its exception, a counts-as
// link, the facts and the open term on the ground), what the engine said laid on it; the layout in layers with the question on top
// and every fact and term on the last row, no two nodes overlapping; the markup with a standing told by a glyph and a word; the plan
// of proof in steps with what is needed in every way; the readings of a term with its binding holding; passages found up to
// whitespace in a text with characters wider than a byte, and the text cut at every mark's ends. Called by run-tests.mjs.

import { evidence as E } from '../js/index.js';

const SCHEME = {
  id: 'park',
  instruments: [{ id: 'bylaw', title: 'Park Bylaw 2010', citation: 'PB 2010', provisions: [{ id: 'bylaw/s1/1', label: 's 1(1)' }, { id: 'bylaw/s1/2', label: 's 1(2)' }] }],
  rules: [{ id: 'ban', elements: ['vehicle', 'in_park'], consequence: 'prohibited', cites: ['bylaw/s1/1'] }],
  exceptions: [{ id: 'emergency_exception', rule: 'ban', elements: ['emergency'], cites: ['bylaw/s1/2'] }],
  terms: [{ name: 'vehicle', gloss: "what 'vehicle' reaches", readings: [{ name: 'broad', gloss: 'anything that carries a person' }, 'narrow'] }, { name: 'unused', readings: ['a', 'b'] }],
  counts: [{ id: 'bicycle_counts', fact: 'bicycle', term: 'vehicle', reading: 'broad', as: 'vehicle', cites: ['bylaw/s1/1'] }],
  vocabulary: [{ id: 'bicycle', gloss: 'a bicycle was ridden in', use: 'fact' }, { id: 'in_park', use: 'fact' }, { id: 'emergency', use: 'fact' }, { id: 'vehicle', use: 'consequence' }, { id: 'prohibited', use: 'consequence' }],
};
const MATTER = {
  issues: [{ id: 'ban', question: 'prohibited', burden: 'the council' }],
  facts: [{ id: 'bicycle', text: 'a bicycle was ridden in', standing: 'alleged' }, { id: 'in_park', standing: 'admitted' }],
  arguments: [{ id: 'p1', term: 'vehicle', reading: 'narrow', canon: 'purpose', text: 'the bylaw keeps the park quiet' }],
};
// As the engine answered it (api/tests/check_law.py's matter, abridged).
const EVALUATION = {
  facts: [{ id: 'bicycle', declared: 'alleged', evidence: 'justified', meets: { status: 'holds' }, used: 'proved' }, { id: 'in_park', declared: 'admitted', evidence: 'unsupported', used: 'admitted' }],
  terms: [{ term: 'vehicle', gloss: "what 'vehicle' reaches", readings: [{ name: 'broad', gloss: 'anything that carries a person' }, { name: 'narrow', gloss: '' }] }],
  admissibility: { fixed: ['[vehicle=broad]'], set_aside: ['[vehicle=narrow]'] },
  authority: { assessed: [{ holding: 's1', force: 'binding', term: 'vehicle', reading: 'broad', name: 'Rider v Council', reason: 'the courts of Avalon bind the magistrates' }, { holding: 'e1', force: 'left out', term: 'vehicle', reading: 'narrow', reason: 'after the date' }] },
  issues: [{
    issue: 'ban', question: 'prohibited', burden: 'the council', class: 'clear', verdict: { reason: 'it follows under every one of the 1 admissible interpretation' }, label: ['[vehicle=broad]'],
    rests_on: { under: '[vehicle=broad]', steps: [{ kind: 'rule', id: 'ban' }, { kind: 'counts', id: 'bicycle_counts' }] },
    plan: { open: ['emergency'], given: ['bicycle', 'in_park'], decided: [], now: 'clear', establish: [{ prove: [], disprove: ['emergency'], readings: [] }], resist: [{ prove: ['emergency'], disprove: [], readings: [] }], must_prove: [], must_disprove: ['emergency'], verdict: { status: 'holds' } },
  }],
};

export function evidenceChecks({ assert }) {
  const g = E.matterGraph(SCHEME, MATTER, EVALUATION);
  const ids = g.nodes.map((n) => n.id).sort();
  assert('evidence: the graph is read off the scheme backwards from the question, and an open term no issue turns on is not in it',
    JSON.stringify(ids) === JSON.stringify(['a:bicycle', 'a:emergency', 'a:in_park', 'a:prohibited', 'a:vehicle', 'k:bicycle_counts', 'r:ban', 't:vehicle', 'x:emergency_exception'].sort()), ids.join(' '));
  const node = (id) => g.nodes.find((n) => n.id === id);
  assert('evidence: the question carries its class and burden; the fact its standing as declared and as used, its evidence and its place in the plan',
    node('a:prohibited').role === 'issue' && node('a:prohibited').cls === 'clear' && node('a:prohibited').burden === 'the council'
      && node('a:bicycle').declared === 'alleged' && node('a:bicycle').used === 'proved' && node('a:bicycle').evidence === 'justified' && node('a:bicycle').place === 'given'
      && node('a:emergency').place === 'open' && node('a:emergency').declared === null && node('a:vehicle').role === 'consequence');
  assert('evidence: the rule names its provision and applies; the exception defeats it; the term has its readings fixed and set aside, and its binding holding',
    node('r:ban').provisions[0].label === 's 1(1)' && node('r:ban').applied && !node('x:emergency_exception').applied
      && g.edges.some((e) => e.from === 'x:emergency_exception' && e.to === 'r:ban' && e.kind === 'defeats')
      && node('t:vehicle').readings[0].fixed && node('t:vehicle').readings[1].aside && node('t:vehicle').holdings.length === 2
      && g.edges.some((e) => e.from === 't:vehicle' && e.to === 'k:bicycle_counts' && e.reading === 'broad'));
  const natural = E.layoutMatter(g);
  const edgesDown = g.edges.every((e) => natural.at.get(e.from).layer > natural.at.get(e.to).layer);
  assert('evidence: by default each node stands beneath what it bears on: a fact under the rule that asks for it, the emergency under its exception',
    edgesDown && natural.at.get('a:in_park').layer === natural.at.get('r:ban').layer + 1 && natural.at.get('a:emergency').layer === natural.at.get('x:emergency_exception').layer + 1);
  const L = E.layoutMatter(g, { ground: true });
  const ground = Math.max(...[...L.at.values()].map((p) => p.layer));
  let apart = true;
  const ps = [...L.at.entries()];
  for (let i = 0; i < ps.length; i++)
    for (let j = i + 1; j < ps.length; j++) {
      const [, a] = ps[i], [, b] = ps[j];
      if (a.layer === b.layer && a.x < b.x + b.w && b.x < a.x + a.w) apart = false;
    }
  assert('evidence: asked for a ground, the question stands on the first row, every fact and term on the last, and no two nodes of a row overlap',
    L.at.get('a:prohibited').layer === 0 && ['a:bicycle', 'a:emergency', 'a:in_park', 't:vehicle'].every((id) => L.at.get(id).layer === ground) && apart && L.width > 0 && L.height > 0);
  const svg = E.svgMatter(g, L, { selected: 'a:bicycle' });
  assert('evidence: the markup tells the class and the standings by glyph and word, marks the chosen node, and names each node for the page',
    svg.startsWith('<svg class="matter"') && svg.includes('✓ clear') && svg.includes('● ALLEGED → PROVED') && svg.includes('◌ NOT IN THE RECORD') && svg.includes('class="node atom fact given chosen"')
      && svg.includes('data-node="t:vehicle"') && svg.includes('◆ broad') && svg.includes('BURDEN: THE COUNCIL'));
  const w = E.waysOf(EVALUATION.issues[0]);
  assert('evidence: the plan of proof in steps, with what is needed in every way marked and said',
    w.establish.length === 1 && w.establish[0][0].kind === 'disprove' && w.establish[0][0].every && w.resist[0][0].kind === 'prove' && !w.resist[0][0].every
      && w.every.length === 1 && E.stepText(w.establish[0][0]) === 'show not to hold emergency' && w.burden === 'the council');
  const t = E.readingsTable(EVALUATION, MATTER, [{ by: 'a model', proposed: { vehicle: 'narrow' }, rationales: { vehicle: 'quiet park' } }]);
  assert('evidence: the readings of a term: the broad one fixed by its binding holding, the narrow one set aside with the party\'s argument and a model\'s proposal beside it',
    t.length === 1 && t[0].readings[0].fixed && t[0].readings[0].holdings[0].force === 'binding' && t[0].readings[1].aside && t[0].readings[1].arguments[0].canon === 'purpose'
      && t[0].readings[1].proposals[0].rationale === 'quiet park' && t[0].left_out.length === 1);
  assert('evidence: a label in words is split into its environments', JSON.stringify(E.labelEnvironments(['[a=x], [b=y]', '(whatever the readings)'])) === JSON.stringify([['[a=x]', '[b=y]'], []]));
  assert('evidence: with a reading fixed, an environment that needs another reading of that term is left out, and the fixed reading is not repeated',
    JSON.stringify(E.labelEnvironments(['[a=z], [b=y]', '[a=x], [b=y]', '[a=x], [c=w]'], ['[a=x]'])) === JSON.stringify([['[b=y]'], ['[c=w]']]));
  const stray = { ...EVALUATION.issues[0], rests_on: { under: '', steps: [{ kind: 'counts', id: 'bicycle_counts', holds: true }, { kind: 'rule', id: 'elsewhere', holds: true }, { kind: 'rule', id: 'ban', holds: true }] } };
  assert('evidence: what a question rests on is the steps on a path to it, from the question down; a rule standing elsewhere is left out',
    E.restsOn(SCHEME, stray).map((s) => s.id).join() === 'ban,bicycle_counts');
  const tree = E.matterTree(g, 'a:prohibited');
  const find = (t, id) => (t.node.id === id ? t : t.children.map((k) => find(k, id)).find(Boolean));
  assert('evidence: the outline unfolds the question into its rule, the rule into what it asks for and its exception, down to the facts',
    tree.kind === 'atom' && tree.children.length === 1 && tree.children[0].node.id === 'r:ban' && tree.children[0].children.map((k) => k.node.id).join() === 'a:vehicle,a:in_park,x:emergency_exception'
      && find(tree, 'k:bicycle_counts').readings[0].reading === 'broad' && find(tree, 'k:bicycle_counts').readings[0].fixed && find(tree, 'a:bicycle').children.length === 0);
  assert('evidence: in the outline the question stands by its rule; the proved bicycle is given; the exception rests on a fact the record does not mention',
    tree.stands === true && find(tree, 'r:ban').stands && find(tree, 'a:bicycle').state === 'given' && find(tree, 'x:emergency_exception').stands === false
      && find(tree, 'a:emergency').state === 'absent' && find(tree, 'x:emergency_exception').state === 'absent' && tree.state === 'given');
  const shown = E.rows(tree, E.unfoldedAtFirst(tree));
  assert('evidence: at first the question and its rule are unfolded, and a branch with nothing in play stays folded', shown[0].depth === 0 && shown[1].node.id === 'r:ban' && shown[1].unfolded
    && shown.some((r) => r.node.id === 'x:emergency_exception' && !r.unfolded) && !shown.some((r) => r.node.id === 'a:emergency'));
  const text = 'Résumé — the warden wrote: a man rode\n   a bicycle past the fountain. 自転車.';
  const at = new TextEncoder().encode(text.slice(0, text.indexOf('a man'))).length;
  const span = E.locate(text, 'a man rode a bicycle', at);
  assert('evidence: a passage is found up to runs of whitespace from its byte offset, in a text with characters wider than a byte',
    span && text.slice(span[0], span[1]) === 'a man rode\n   a bicycle' && E.charAt(text, at) === text.indexOf('a man') && E.byteAt(text, text.indexOf('a man')) === at && E.locate(text, 'a woman rode') === null);
  const segs = E.segments(text, [{ id: 'c1', start: span[0], end: span[1] }, { id: 'c2', start: span[0] + 2, end: span[1] + 9 }]);
  assert('evidence: the text is cut at every mark\'s ends, each run carrying the marks over it, and nothing is lost',
    segs.map((s) => s.text).join('') === text && segs.some((s) => s.marks.length === 2) && segs.find((s) => s.text.startsWith('a ')).marks.join() === 'c1');
}
