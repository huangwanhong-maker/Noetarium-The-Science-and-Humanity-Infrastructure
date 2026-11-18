// Checks of the architecture of kybernesis drawn (visualization/js/steering), ADR 0130. On the foundation's standard architecture
// (a copy in tests/fixtures/standard.arch of foundations/kybernesis/architectures/standard.arch): the script read with no error, its registers, stages, kinds (registers
// inherited), blocks, boundaries and wires counted; scripts with faults refused line by line (a syntax error, a body left open, an
// input on a boundary, a kind, a type, a block or a port not declared, a wire that joins two types, an example kind yielded); the
// layout (stages as columns in their order, every block within its stage's column, every forward wire from left to right, no two
// boxes overlapping, no text on another text or on a box not its own, a box's texts within it); the markup (well formed, every
// block named, wires carried to the next turn dashed and marked, boundaries and referred blocks outlined in dashes, a wire back
// against the stages told apart, a self-loop a petal at its block); and with a report each method's family as a glyph and its
// verdict as a mark and a word, the checks counted and the failing one named. Called by run-tests.mjs.

import { readFileSync } from 'node:fs';
import { steering as K } from '../js/index.js';

const STANDARD = readFileSync(new URL('./fixtures/standard.arch', import.meta.url), 'utf8');
const REPORT = {
  architecture: 'standard', version: 1,
  checks: [
    { check: 'every non-next wire goes forward in the stages', kind: 'constraint', status: 'holds', reason: '' },
    { check: 'every input is wired once', kind: 'constraint', status: 'holds', reason: '' },
    { check: 'each block yields a kind in its stage\'s register & <no other>', kind: 'category', status: 'fails', reason: 'Constitution' },
  ],
  methods: [
    { block: 'Estimation', method: 'possibility filter', family: 'formal', wraps: 'kybernesis::methods', status: 'holds', reason: '' },
    { block: 'Estimation', method: 'Kalman filter', family: 'statistical', wraps: 'tyche', status: 'holds', reason: '' },
    { block: 'Evaluation', method: 'declared operators', family: 'declared', wraps: 'kybernesis', status: 'undecided', reason: '' },
    { block: 'PolicyMaking', method: 'language model', family: 'proposal', wraps: 'science.models', status: 'fails', reason: '' },
  ],
};
const count = (text, part) => text.split(part).length - 1;
const overlap = (a, b) => a.x < b.x + b.w - 0.01 && b.x < a.x + a.w - 0.01 && a.y < b.y + b.h - 0.01 && b.y < a.y + a.h - 0.01;
// Tags balanced: every element opened is closed in order (self-closing ones aside).
function balanced(svg) {
  const stack = [];
  for (const m of svg.matchAll(/<(\/?)([A-Za-z][\w:-]*)[^>]*?(\/?)>/g)) {
    if (m[3] === '/') continue;
    if (m[1] === '/') { if (stack.pop() !== m[2]) return false; } else stack.push(m[2]);
  }
  return stack.length === 0;
}
const SMALL = (extra = '') => `architecture small version 2
registers M < R      # two registers
stages a < b
stage a in M
stage b in R
kind K in R
kind T under K in M
type X in R
block P yields K at a {
  out x : X
}
block Q yields K at b {
  in x : X
  out y : X
}
${extra}`;

export function steeringChecks({ assert }) {
  // ---- the standard architecture read ----
  const A = K.parseArchitecture(STANDARD);
  assert('steering: the standard architecture is read with no error', A.errors.length === 0, A.errors.map((e) => e.text).join('; '));
  assert('steering: its name, version, three registers in the order of aboutness, four stages each with its register',
    A.name === 'standard' && A.version === 1 && A.registers.join(' < ') === 'Manifestational < Relational < Ontological'
      && A.stages.map((s) => `${s.name}:${s.register}`).join(' ') === 'before:Ontological observed:Manifestational recorded:Relational after:Ontological');
  const boundaries = A.blocks.filter((b) => b.boundary);
  assert('steering: thirteen blocks, three of them boundaries; twenty-five wires, five of them carried to the next turn',
    A.blocks.length === 13 && boundaries.length === 3 && A.wires.length === 25 && A.wires.filter((w) => w.next).length === 5,
    `${A.blocks.length} blocks, ${boundaries.length} boundaries, ${A.wires.length} wires, ${A.wires.filter((w) => w.next).length} next`);
  const kind = (n) => A.kinds.find((k) => k.name === n);
  assert('steering: twenty-three kinds, five of them examples; a register inherited from the parent unless stated',
    A.kinds.length === 23 && A.kinds.filter((k) => k.example).length === 5 && kind('Condition').register === 'Ontological' && kind('Trace').register === 'Manifestational'
      && kind('Uptake').register === 'Relational' && kind('Risk').register === 'Relational' && kind('Risk').parent === 'Evaluation' && kind('Fact').parent === null);
  const est = A.blocks.find((b) => b.name === 'Estimation'), con = A.blocks.find((b) => b.name === 'Constitution');
  assert('steering: a block\'s laws and ports, a boundary\'s outputs, and a wire\'s type taken from its ports',
    est.laws.join(',') === 'total,sound' && est.inputs.map((p) => p.port).join(',') === 'traces,readings,prior,action' && est.outputs[0].type === 'Belief'
      && con.boundary && con.inputs.length === 0 && con.outputs.length === 4 && A.wires.find((w) => w.from === 'Estimation' && w.to === 'Estimation').type === 'Belief');

  // ---- scripts with faults ----
  {
    const bad = K.parseArchitecture('architecture x version one\nregisters A B\nstages s\nstage s in A\nwire P.x => Q.y\nfrobnicate\n}');
    const at = (n) => bad.errors.filter((e) => e.line === n).map((e) => e.message).join('; ');
    assert('steering: a syntax error is refused with its line and what was expected',
      /expected an integer after 'version', found 'one'/.test(at(1)) && /expected the end of the line, found 'B'/.test(at(2)) && /expected '->'/.test(at(5))
        && /expected a statement/.test(at(6)) && /closes no block/.test(at(7)), bad.errors.map((e) => e.text).join(' | '));
    assert('steering: what the script lacks as a whole is said at line 0', bad.errors.some((e) => e.line === 0 && /no registers/.test(e.message)));
    const open = K.parseArchitecture(SMALL('block R yields K at b {\n  in x : X\nwire P.x -> Q.x\n'));
    assert('steering: a body left open is refused at the statement that follows it, which is still read',
      open.errors.some((e) => e.line === 18 && /expected '}' closing block R \(line 16\)/.test(e.message)) && open.wires.length === 1, open.errors.map((e) => e.text).join(' | '));
    const unclosed = K.parseArchitecture(SMALL('block R yields K at b {\n  in x : X\n'));
    assert('steering: a body still open at the end of the script is refused at its first line', unclosed.errors.some((e) => e.line === 16 && /found the end of the script/.test(e.message)));
    const wrong = K.parseArchitecture(SMALL(`boundary O yields Z at b {
  in u : X
  out v : Y
}
block E yields T at c laws total {
}
example N under K
block F yields N at b {
}
wire P.x -> Q.z
wire Q.y -> G.x
type Y in R
block H yields K at b {
  in w : Y
}
wire Q.y -> H.w`));
    const says = (n, re) => wrong.errors.some((e) => e.line === n && re.test(e.message));
    assert('steering: a kind, a type, a stage, a block or a port not declared is named with the line that names it',
      says(16, /kind Z is not declared/) && says(20, /stage c is not declared/) && says(25, /Q has no input z/) && says(26, /block G is not declared/),
      wrong.errors.map((e) => e.text).join(' | '));
    assert('steering: an input on a boundary, an example kind yielded, and a wire that joins two types are refused',
      says(17, /a boundary has outputs only/) && says(23, /N is an example kind, which no block yields/) && says(31, /joins X to Y/), wrong.errors.map((e) => e.text).join(' | '));
    const twice = K.parseArchitecture(SMALL('kind K in M\ntype X in M\nblock P yields K at a {\n}\nkind C under C'));
    assert('steering: a name declared twice and a kind under itself are refused',
      twice.errors.some((e) => e.line === 16 && /kind K is declared again \(first at line 6\)/.test(e.message)) && twice.errors.some((e) => /type X is declared again/.test(e.message))
        && twice.errors.some((e) => /block P is declared again/.test(e.message)) && twice.errors.some((e) => e.line === 20 && /under itself/.test(e.message)), twice.errors.map((e) => e.text).join(' | '));
    const small = K.parseArchitecture(SMALL('wire P.x -> Q.x   # a comment after a statement\n\n  # a comment alone\n'));
    assert('steering: comments and blank lines are passed over', small.errors.length === 0 && small.wires.length === 1 && small.kinds[1].register === 'M');
  }

  // ---- the layout ----
  const L = K.layoutGraph(A);
  {
    const col = (s) => L.columns.find((c) => c.stage === s);
    assert('steering: the stages are columns from left to right in their order, and each block stands within its stage\'s column',
      L.columns.map((c) => c.stage).join(' ') === 'before observed recorded after' && L.columns.every((c, k) => k === 0 || L.columns[k - 1].right < c.left)
        && L.boxes.every((b) => b.x >= col(b.block.stage).left && b.x + b.w <= col(b.block.stage).right));
    assert('steering: the columns are tinted by their registers, Ontological, Manifestational and Relational told apart',
      col('before').tint === col('after').tint && new Set([col('before').tint, col('observed').tint, col('recorded').tint]).size === 3);
    const box = (n) => L.boxes.find((b) => b.name === n);
    const forward = L.wires.filter((w) => w.kind === 'forward');
    assert('steering: twenty forward wires, each from its block\'s right side to the left side of a block further right',
      forward.length === 20 && forward.every((w) => box(w.from).x + box(w.from).w < box(w.to).x && w.points.every((p, k) => k === 0 || p.x >= w.points[k - 1].x - 0.01)));
    assert('steering: within the recorded stage the blocks stand in sub-columns by depth: Interpretation, Estimation, Evaluation, PolicyMaking, Policy, Decision',
      ['Interpretation', 'Estimation', 'Evaluation', 'PolicyMaking', 'Policy', 'Decision'].every((n, k, all) => k === 0 || box(all[k - 1]).x + box(all[k - 1]).w < box(n).x)
        && box('Constitution').x + box('Constitution').w < box('Interpretation').x && box('Proposals').layer === box('Evaluation').layer);
    const boxes = L.boxes;
    assert('steering: no two boxes overlap', boxes.every((a, i) => boxes.every((b, j) => i === j || !overlap(a, b))));
    const clashes = [];
    L.texts.forEach((t, i) => L.texts.forEach((u, j) => { if (j > i && overlap(t.box, u.box)) clashes.push(`${t.text} / ${u.text}`); }));
    L.texts.forEach((t) => boxes.forEach((b) => { if (t.owner !== `block:${b.name}` && overlap(t.box, b)) clashes.push(`${t.text} on ${b.name}`); }));
    assert('steering: no text stands on another text, nor on a box not its own', clashes.length === 0, clashes.join('; '));
    assert('steering: a box\'s texts stand within it', L.texts.filter((t) => t.owner.startsWith('block:')).every((t) => {
      const b = box(t.owner.slice(6));
      return t.box.x >= b.x && t.box.x + t.box.w <= b.x + b.w && t.box.y >= b.y && t.box.y + t.box.h <= b.y + b.h;
    }));
    const next = L.wires.filter((w) => w.kind === 'next');
    assert('steering: the five wires carried to the next turn return beneath every block, the one from a block to itself a petal at its foot',
      next.length === 5 && next.filter((w) => w.loop).length === 1 && next.find((w) => w.loop).from === 'Estimation'
        && next.filter((w) => !w.loop).every((w) => w.lane >= 0 && box(w.to).x < box(w.from).x + box(w.from).w));
  }

  // ---- the markup ----
  {
    const svg = K.svgGraph(A);
    assert('steering: the drawing is one well-formed SVG', svg.startsWith('<svg') && svg.trimEnd().endsWith('</svg>') && balanced(svg) && count(svg, '<svg') === 1);
    assert('steering: every block is named in it', A.blocks.every((b) => svg.includes(`>${b.name}</text>`)));
    const nextGroups = [...svg.matchAll(/<g class="kb-wire kb-next"[^>]*>(.*?)<\/g>/g)].map((m) => m[1]);
    const forwardGroups = [...svg.matchAll(/<g class="kb-wire kb-forward"[^>]*>(.*?)<\/g>/g)].map((m) => m[1]);
    assert('steering: wires carried to the next turn are drawn dashed and marked "next"; the others are not dashed',
      nextGroups.length === 5 && nextGroups.every((g) => /stroke-dasharray/.test(g)) && forwardGroups.length === 20 && forwardGroups.every((g) => !/stroke-dasharray/.test(g))
        && count(svg, '>next · ') === 5);
    const outline = (cls) => [...svg.matchAll(new RegExp(`<g class="kb-block[^"]*${cls}[^"]*"[^>]*>\\s*<rect([^>]*)>`, 'g'))].map((m) => m[1]);
    assert('steering: the three boundaries are outlined in dashes; System and Realisation, referred to and not established, in dashes of moss',
      outline('kb-boundary').length === 3 && outline('kb-boundary').every((r) => /stroke-dasharray/.test(r)) && outline('kb-referred').length === 2
        && outline('kb-referred').every((r) => /stroke="#557A5C"/.test(r) && /stroke-dasharray/.test(r)));
    assert('steering: with no report there is no method, no verdict and no count of checks', !/kb-method|admitted|refused|checks:/.test(svg));
    assert('steering: the key names the wires, the next turn, boundaries, referred blocks and the registers',
      /next: carried to the following turn/.test(svg) && /boundary: given from outside the loop/.test(svg) && /referred to, not established/.test(svg) && />Manifestational</.test(svg));
    assert('steering: the script itself may be given, and a ground of one\'s own', K.svgGraph(STANDARD, null, { ground: '#FAF6F1' }).includes('class="kb-ground" x="0" y="0"') && K.svgGraph(STANDARD, null, { ground: '#FAF6F1' }).includes('fill="#FAF6F1"'));
  }

  // ---- with a report ----
  {
    const svg = K.svgGraph(A, REPORT);
    assert('steering: each method is under its block with its family as a glyph', ['◇ possibility filter', '○ Kalman filter', '□ declared operators', '△ language model'].every((t) => svg.includes(`>${t}</text>`)));
    assert('steering: each verdict is a mark and a word: admitted, refused, undecided', svg.includes('>✓ admitted</text>') && svg.includes('>✗ refused</text>') && svg.includes('>… undecided</text>'));
    assert('steering: the checks are counted and the failing one named with its kind, escaped',
      svg.includes('checks: 2 holding · 1 failing · 0 undecided') && svg.includes('✗ category · each block yields a kind in its stage\'s register &amp; &lt;no other&gt;') && balanced(svg));
    const L2 = K.layoutGraph(A, REPORT);
    const est = L2.boxes.find((b) => b.name === 'Estimation');
    const clashes = [];
    L2.texts.forEach((t, i) => L2.texts.forEach((u, j) => { if (j > i && overlap(t.box, u.box)) clashes.push(`${t.text} / ${u.text}`); }));
    L2.texts.forEach((t) => L2.boxes.forEach((b) => { if (t.owner !== `block:${b.name}` && overlap(t.box, b)) clashes.push(`${t.text} on ${b.name}`); }));
    assert('steering: a block with methods grows to hold them, and still no text stands on another or on a box not its own',
      est.h > L.boxes.find((b) => b.name === 'Estimation').h && est.methods.length === 2 && clashes.length === 0, clashes.join('; '));
  }

  // ---- a wire against the stages, told apart ----
  {
    const back = K.parseArchitecture(SMALL('wire P.x -> Q.x\nblock S yields K at a {\n  in x : X\n}\nwire Q.y -> S.x'));
    const svg = K.svgGraph(back);
    assert('steering: a wire that runs back against the stages without "next" returns beneath, dotted in rose and marked "backward"',
      back.errors.length === 0 && /<g class="kb-wire kb-backward"[^>]*><path[^>]*stroke="#B8577F"[^>]*stroke-dasharray/.test(svg) && svg.includes('>X · backward</text>'));
  }
}
