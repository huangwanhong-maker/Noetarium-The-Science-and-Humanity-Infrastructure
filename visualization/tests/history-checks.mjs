// Checks of historical objects drawn (js/history): the cone and the loom as markup, read in Node.

import { ROLES, ROLE_ORDER, HUES, glyphOf, yearText, textWidth, layoutCone, svgCone, warp, ticks, layoutLoom, svgLoom } from '../js/history/index.js';

export function historyChecks({ assert }) {
  // ---- words, glyphs, years
  assert('history: a year is read as people read it', yearText(1584) === '1584' && yearText(-432.5) === '434 BC' && yearText(0.2) === '1 BC' && yearText(1585.4, true) === 'about 1585' && yearText(null) === '');
  assert('history: an object takes the glyph of its kind, or of the nearest kind above it', glyphOf({ kind: 'Problem' }) === '?' && glyphOf({ kind: 'Tuning', above: ['Theory', 'Idea', 'Object'] }) === '△' && glyphOf({ kind: 'Unknown' }) === '·');
  assert('history: each role has its words and a hue of the validated ten, all different', ROLE_ORDER.every((r) => ROLES[r].name && HUES.includes(ROLES[r].hue)) && new Set(ROLE_ORDER.map((r) => ROLES[r].hue)).size === 4);
  assert('history: a wide script takes its full width', textWidth('律學新說', 12) === 48 && textWidth('ill', 10) < textWidth('mmm', 10));

  // ---- the cone
  const o = (id, name, extra = {}) => ({ id, name, kind: 'Theory', above: ['Idea', 'Object'], standing: 'proposed', ...extra });
  const view = {
    center: o('east', 'The eastern answer', { year: 1584 }),
    past: [
      o('roots', 'Roots', { steps: 1, role: 'condition', year: 1300 }),
      o('closing', 'It does not close', { steps: 1, role: 'problem', kind: 'Problem', year: -400 }),
      o('old', 'The old tuning', { steps: 2, year: -500 }),
      o('string', 'A string', { steps: 3, year: -600 }),
      o('deep', 'Deeper still', { steps: 5 }),
      o('keys', 'Every key', { steps: 1, role: 'problem', inherited: 'tempering', kind: 'Problem' }),
    ],
    future: [o('treatise', 'A treatise', { steps: 1, role: 'source', year: 1636 }), o('piece', 'A piece', { steps: 2, year: 1700 })],
    links: [
      { what: 'closing', product: 'east', role: 'problem', standing: 'established' },
      { what: 'roots', product: 'east', role: 'condition', standing: 'attested' },
      { what: 'old', product: 'closing', role: 'condition', standing: 'proposed' },
      { what: 'string', product: 'old', role: 'condition', standing: 'proposed' },
      { what: 'east', product: 'treatise', role: 'source', standing: 'proposed' },
      { what: 'treatise', product: 'piece', role: 'source', standing: 'proposed' },
      { what: 'nowhere', product: 'east', role: 'source', standing: 'proposed' },
    ],
    lineage: [{ id: 'tempering', name: 'Tempering' }],
  };
  const L = layoutCone(view);
  const at = Object.fromEntries(L.nodes.map((n) => [n.id, n]));
  assert('history: a cone holds its object and everything in its past and future, once', L.nodes.length === 9 && new Set(L.nodes.map((n) => n.id)).size === 9 && L.nodes[0].center);
  assert('history: the past stands to the left by steps and the future to the right', at.string.x < at.old.x && at.old.x < at.closing.x && at.closing.x < at.east.x && at.east.x < at.treatise.x && at.treatise.x < at.piece.x);
  const shallow = Object.fromEntries(layoutCone(view, { depth: 2 }).nodes.map((n) => [n.id, n]));
  assert('history: beyond the depth kept, the farthest column gathers the rest', at.deep.x === at.string.x && shallow.string.x === shallow.old.x);
  assert('history: the first column is sorted by role, the problem first, what is inherited last', at.closing.y < at.roots.y && at.roots.y < at.keys.y && at.keys.x === at.closing.x);
  assert('history: each group of the first column has a heading', L.headings.length === 3 && L.headings[0].role === 'problem' && L.headings[2].inherited === 'tempering');
  const overlap = L.nodes.some((a, i) => L.nodes.some((b, j) => i < j && a.x < b.x + b.w && b.x < a.x + a.w && a.y < b.y + b.h && b.y < a.y + a.h));
  assert('history: no two slips of a cone overlap, and all lie within the drawing', !overlap && L.nodes.every((n) => n.x >= 0 && n.y >= 0 && n.x + n.w <= L.width && n.y + n.h <= L.height));
  assert('history: a link is drawn when both its ends are, and an inherited premise is tied to the object as inherited', L.edges.length === 7 && L.edges.filter((e) => e.inherited).length === 1 && L.edges.find((e) => e.inherited).a.id === 'keys');
  const svg = svgCone(view, { names: { tempering: 'Tempering' } });
  assert('history: the cone is one SVG with a slip for each object, each carrying its id', svg.startsWith('<svg') && svg.endsWith('</svg>') && (svg.match(/class="h-slip/g) || []).length === 9 && svg.includes('data-object="closing"'));
  assert('history: a link carries how it stands as a class, and a slip says it in words', svg.includes('h-link s-established') && svg.includes('h-link s-attested') && svg.includes('· problem · proposed'));
  assert('history: what is inherited is named with what it comes through, and the lineage is written', svg.includes('inherited through Tempering') && svg.includes('stands under Tempering'));
  const bare = svgCone({ center: o('x', 'Alone <&>'), past: [], future: [], links: [] });
  assert('history: an object with nothing before or after says that the record names nothing, and escapes its name', bare.includes('names nothing it rested on') && bare.includes('names nothing that rests on it') && bare.includes('Alone &lt;&amp;&gt;'));

  // ---- the loom
  const even = warp([0, 10, 1000], 0, 1000, 0), ranked = warp([0, 10, 1000], 0, 1000, 1), mixed = warp([0, 10, 1000], 0, 1000, 0.5);
  assert('history: a warp with no evenness is the date itself; with all of it, the rank', Math.abs(even(10) - 0.01) < 1e-12 && Math.abs(ranked(10) - 0.5) < 1e-12 && Math.abs(mixed(10) - 0.255) < 1e-12);
  let rising = true;
  for (let y = 0; y < 1000; y += 7) rising = rising && mixed(y + 7) > mixed(y);
  assert('history: a warp keeps the order of time, from 0 to 1', rising && mixed(0) === 0 && mixed(1000) === 1 && mixed(-50) === 0 && mixed(2000) === 1);
  const t = ticks(-600, 1750);
  assert('history: the years marked lie on a round step, at most twelve of them', t.length <= 13 && t.every((y) => y % 200 === 0 || y % 250 === 0) && t[0] >= -600 && t.at(-1) <= 1750);
  const loom = {
    from: -600, to: 1750, lanes: [{ id: 'music', name: 'Music' }, { id: 'philosophy', name: 'Philosophy' }],
    objects: [
      { id: 'a', name: 'First', kind: 'Theory', year: -500, lane: 'music' },
      { id: 'b', name: 'Second of the same year', kind: 'Theory', year: -500, lane: 'music' },
      { id: 'c', name: 'A doctrine', kind: 'Theory', year: -360, lane: 'philosophy' },
      { id: 'd', name: 'Late', kind: 'Work', year: 1722, lane: 'music' },
      { id: 'e', name: 'Undated', kind: 'Problem', lane: 'music' },
      { id: 'f', name: 'Elsewhere', kind: 'Work', year: 1500, lane: 'painting' },
    ],
    threads: [
      { from: 'a', to: 'c', role: 'source', standing: 'proposed' },
      { from: 'a', to: 'd', role: 'source', standing: 'established' },
      { from: 'e', to: 'd', role: 'problem', standing: 'proposed' },
    ],
  };
  const W = layoutLoom(loom, { width: 1000 });
  const mark = Object.fromEntries(W.marks.map((m) => [m.id, m]));
  assert('history: what has a date and a strand is on the loom, and the rest is counted', W.marks.length === 4 && W.undated === 2);
  assert('history: two objects of one year in one strand take two rows', mark.a.x === mark.b.x && mark.a.y !== mark.b.y);
  assert('history: the strands lie one under another, and later lies to the right', W.lanes[1].y === W.lanes[0].y + W.lanes[0].height && mark.c.y > mark.a.y && mark.d.x > mark.c.x && mark.c.x > mark.a.x);
  assert('history: a thread is drawn when both its ends are, and knows whether it crosses strands', W.threads.length === 2 && W.threads.find((x) => x.to === 'c').across && !W.threads.find((x) => x.to === 'd').across);
  const drawn = svgLoom(loom, { width: 1000, selected: 'd', lit: ['a', 'd'], now: 1600 });
  assert('history: the loom is one SVG with its strands, marks and threads', drawn.startsWith('<svg') && (drawn.match(/class="h-lane/g) || []).length >= 2 && (drawn.match(/data-object=/g) || []).length === 4 && (drawn.match(/class="h-thread/g) || []).length === 2);
  assert('history: what is chosen is marked, what is outside the cone stands back, and the clock is a line with its year', drawn.includes('h-mark selected') && drawn.includes('h-mark back') && drawn.includes('h-now') && drawn.includes('>1600<'));
  assert('history: what is not on the loom is said', drawn.includes('2 without a date'));
}
