// Checks of linked data drawn (visualization/js/linked): IRIs named by the longest prefix that fits; a graph's nodes and edges
// from statements, literals and classes kept on their nodes; a neighbourhood; the layout the same for the same seed, inside its
// frame and finite; the graph's markup with a mark for every node and edge, its names escaped; Turtle coloured without a
// character changed; a Web Annotation of a region, a span of time and a passage written and read back. Called by run-tests.mjs.

import { linked as L } from '../js/index.js';

const iri = (v) => ({ kind: 'iri', value: v });
const lit = (v, language) => (language ? { kind: 'literal', value: v, datatype: 'http://www.w3.org/1999/02/22-rdf-syntax-ns#langString', language } : { kind: 'literal', value: v, datatype: 'http://www.w3.org/2001/XMLSchema#string' });
const EX = 'http://e/';
const count = (text, part) => text.split(part).length - 1;

export function linkedChecks({ assert, check }) {
  assert('linked: an IRI is named by the longest prefix that fits, else its last segment', L.brief('http://purl.org/dc/terms/title') === 'dcterms:title' &&
    L.brief(`${EX}things/mm`) === 'mm' && L.brief('http://www.w3.org/1999/02/22-rdf-syntax-ns#type') === 'rdf:type' && L.label(lit('chat', 'fr')) === '“chat”@fr');
  const quads = [
    [iri(`${EX}mm`), iri(L.RDF_TYPE), iri(`${EX}Novel`)],
    [iri(`${EX}mm`), iri('http://purl.org/dc/terms/title'), lit('Middlemarch', 'en')],
    [iri(`${EX}mm`), iri(`${EX}author`), iri(`${EX}eliot`)],
    [iri(`${EX}eliot`), iri(`${EX}name`), lit('George <Eliot>')],
    [iri(`${EX}ad`), iri(`${EX}author`), iri(`${EX}eliot`)],
    [iri(`${EX}ad`), iri(`${EX}cites`), { kind: 'blank', value: 'b0' }],
  ];
  const m = L.graphModel(quads);
  const mm = m.nodes.find((n) => n.term.value === `${EX}mm`);
  assert('linked: four nodes and three edges; the title kept on its book, its class drawn as its kind', m.nodes.length === 4 && m.edges.length === 3 && mm.literals.length === 1 && mm.classes.length === 1);
  const near = L.neighbourhood(m, mm.key, 1);
  assert('linked: one step from Middlemarch reaches its author only', near.nodes.length === 2 && near.edges.length === 1);
  const a = L.layoutGraph(m, { seed: 3 }), b = L.layoutGraph(m, { seed: 3 });
  let same = true, inside = true;
  for (const [k, p] of a) {
    same = same && p.x === b.get(k).x && p.y === b.get(k).y;
    inside = inside && Number.isFinite(p.x) && p.x >= 40 && p.x <= 860 && p.y >= 40 && p.y <= 560;
  }
  assert('linked: the layout is the same for the same seed, finite and inside its frame', same && inside && a.size === 4);
  const svg = L.svgGraph(m, a, { selected: mm.key, marked: new Set(['e0']) });
  assert('linked: the markup has a mark for every node and edge, the selected and the marked told apart', count(svg, 'class="l-node') === 4 && count(svg, 'class="l-edge') === 3 &&
    svg.includes('l-node iri selected') && svg.includes('l-edge marked'));
  const big = L.svgGraph(L.graphModel([[iri(`${EX}a`), iri(`${EX}p`), iri(`${EX}<b>"`)]]), L.layoutGraph(L.graphModel([[iri(`${EX}a`), iri(`${EX}p`), iri(`${EX}<b>"`)]])));
  assert('linked: names are escaped', !big.includes('<b>"') && big.includes('&lt;b&gt;&quot;'));
  const ttl = '@prefix ex: <http://e/> .\n# a comment\nex:mm a ex:Novel ; ex:title "Middle\\"march"@en ; ex:year 1871 .\nSELECT ?x WHERE { ?x ex:p _:b }';
  const html = L.htmlTurtle(ttl);
  const text = html.replace(/<[^>]+>/g, '').replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&quot;/g, '"').replace(/&amp;/g, '&');
  assert('linked: Turtle coloured keeps every character, and marks comments, literals, keywords, variables and numbers', text === ttl &&
    html.includes('t-comment') && html.includes('t-literal') && html.includes('t-keyword') && html.includes('t-variable') && html.includes('t-number') && html.includes('t-prefixed'));
  const region = L.annotation({ id: 'urn:a:1', target: 'urn:h:letter.jpg', selector: { kind: 'region', x: 10.4, y: 20, w: 300, h: 120 }, body: { text: 'the signature' }, created: '2026-11-15T10:00:00Z' });
  const time = L.annotation({ id: 'urn:a:2', target: 'urn:h:interview.mp3', selector: { kind: 'time', start: 61.5, end: 75 }, body: { tag: 'urn:code:care' }, motivation: 'tagging' });
  const passage = L.annotation({ id: 'urn:a:3', target: 'urn:h:minutes.txt', selector: { kind: 'text', exact: 'the canteen', prefix: 'far from ', suffix: ' gate', start: 40, end: 51 }, body: { text: 'distance' } });
  const back = L.annotations([...region, ...time, ...passage]);
  const r = back.find((x) => x.id === 'urn:a:1'), t = back.find((x) => x.id === 'urn:a:2'), p = back.find((x) => x.id === 'urn:a:3');
  assert('linked: three annotations read back with their targets', back.length === 3 && r.target === 'urn:h:letter.jpg' && t.target === 'urn:h:interview.mp3' && p.target === 'urn:h:minutes.txt');
  assert('linked: a region as xywh, a span of time as t, a passage by its quotation', r.selector.kind === 'region' && r.selector.x === 10 && r.selector.w === 300 &&
    t.selector.kind === 'time' && t.selector.start === 61.5 && t.selector.end === 75 && p.selector.kind === 'text' && p.selector.exact === 'the canteen' && p.selector.prefix === 'far from ');
  assert('linked: a textual body and a tag as body, and the motivation', r.body.text === 'the signature' && t.body.tag === 'urn:code:care' && t.motivation === 'tagging' && r.motivation === 'commenting');
  check('linked: an empty graph lays out as nothing', L.layoutGraph({ nodes: [], edges: [] }).size, 0);
}
