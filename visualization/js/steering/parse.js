// An architecture script read (kybernesis, ADR 0130): the registers and stages of one turn, the kinds of fact, the types that
// travel on wires, the blocks and boundaries with their ports and laws, and the wires.
//
// The language is the foundation's (foundations/kybernesis/architectures/standard.arch is its example): one statement a line,
// '#' to the end of a line a comment, blank lines ignored; a block's or a boundary's body runs from the '{' that ends its first
// line to a '}' on a line of its own.
//
//   architecture <Name> version <Integer>
//   registers <R> < <R> < ...                        the order of aboutness
//   stages <S> < <S> < ...                           the stages of one turn, earliest first
//   stage <S> in <Register>
//   kind <Name> in <Register>                        a root kind
//   kind <Name> under <Parent> [in <Register>]       a subkind, its register inherited unless stated
//   example <Name> under <Parent>                    an example kind, never yielded by a block
//   type <Name> in <Register>
//   block <Name> yields <Kind> at <Stage> [laws <law>, ...] {   in <port> : <Type>   out <port> : <Type>   }
//   boundary <Name> yields <Kind> at <Stage> {                  out <port> : <Type>                       }
//   wire <Block>.<port> -> <Block>.<port> [next]     "next": carried to the next turn
//
// parseArchitecture never throws: what it cannot read, and every name it cannot resolve, is an error with its line (line 0 for
// what the script as a whole lacks). Kinds, types and blocks are three namespaces: a block may share a kind's name.

const NAME = /^[A-Za-z_][A-Za-z0-9_]*$/;
const INTEGER = /^[0-9]+$/;
const TOKEN = /\s*(->|[A-Za-z_][A-Za-z0-9_]*|[0-9]+|[<{}:,.]|\S+)/y;
const STATEMENTS = ['architecture', 'registers', 'stages', 'stage', 'kind', 'example', 'type', 'block', 'boundary', 'wire'];

function tokensOf(text) {
  const out = [];
  TOKEN.lastIndex = 0;
  while (TOKEN.lastIndex < text.length) {
    const m = TOKEN.exec(text);
    if (!m || m[1] === undefined) break;
    out.push(m[1]);
  }
  return out;
}

class Refusal extends Error {}

// The tokens of one line, read in order; the first thing that is not as expected refuses the line.
class Reader {
  constructor(tokens) { this.tokens = tokens; this.at = 0; }
  peek() { return this.tokens[this.at]; }
  found() { return this.at < this.tokens.length ? `'${this.tokens[this.at]}'` : 'the end of the line'; }
  refuse(expected) { throw new Refusal(`expected ${expected}, found ${this.found()}`); }
  word(w, expected = `'${w}'`) { if (this.peek() !== w) this.refuse(expected); this.at++; return w; }
  name(expected) { const t = this.peek(); if (t === undefined || !NAME.test(t)) this.refuse(expected); this.at++; return t; }
  integer(expected) { const t = this.peek(); if (t === undefined || !INTEGER.test(t)) this.refuse(expected); this.at++; return Number(t); }
  accept(w) { if (this.peek() === w) { this.at++; return true; } return false; }
  end() { if (this.at < this.tokens.length) this.refuse('the end of the line'); }
}

// A chain "A < B < C": at least one name.
function chain(r, what) {
  const list = [r.name(`a ${what} name`)];
  while (r.accept('<')) list.push(r.name(`a ${what} name after '<'`));
  r.end();
  return list;
}

export function parseArchitecture(text) {
  const A = { name: null, version: null, registers: [], stages: [], kinds: [], types: [], blocks: [], wires: [], errors: [] };
  const error = (line, message) => A.errors.push({ line, message, text: line ? `line ${line}: ${message}` : message });
  const lines = String(text ?? '').split(/\r?\n/).map((raw, i) => {
    const hash = raw.indexOf('#');
    return { n: i + 1, tokens: tokensOf(hash >= 0 ? raw.slice(0, hash) : raw) };
  });
  const seen = { architecture: 0, registers: 0, stages: 0 };
  const stageRegister = new Map(); // stage name → { register, line }
  const kindLine = new Map(), typeLine = new Map(), blockLine = new Map();

  for (let i = 0; i < lines.length; i++) {
    const { n, tokens } = lines[i];
    if (!tokens.length) continue;
    const r = new Reader(tokens);
    try {
      const head = r.peek();
      if (head === 'architecture') {
        r.at++;
        const name = r.name('the architecture\'s name');
        r.word('version');
        const version = r.integer('an integer after \'version\'');
        r.end();
        if (seen.architecture) error(n, `the architecture is declared again (first at line ${seen.architecture})`);
        else { seen.architecture = n; A.name = name; A.version = version; }
      } else if (head === 'registers' || head === 'stages') {
        r.at++;
        const list = chain(r, head === 'registers' ? 'register' : 'stage');
        const twice = list.find((x, k) => list.indexOf(x) !== k);
        if (twice) error(n, `${twice} stands twice in the ${head}`);
        if (seen[head]) error(n, `the ${head} are declared again (first at line ${seen[head]})`);
        else {
          seen[head] = n;
          if (head === 'registers') A.registers = [...new Set(list)];
          else A.stages = [...new Set(list)].map((name) => ({ name, register: null, line: n }));
        }
      } else if (head === 'stage') {
        r.at++;
        const name = r.name('a stage name');
        r.word('in');
        const register = r.name('a register name after \'in\'');
        r.end();
        if (stageRegister.has(name)) error(n, `the register of stage ${name} is declared again (first at line ${stageRegister.get(name).line})`);
        else stageRegister.set(name, { register, line: n });
      } else if (head === 'kind' || head === 'example') {
        r.at++;
        const example = head === 'example';
        const name = r.name(`a ${head} name`);
        let parent = null, register = null;
        if (example) {
          r.word('under', '\'under\'');
          parent = r.name('a parent kind after \'under\'');
        } else if (r.accept('in')) {
          register = r.name('a register name after \'in\'');
        } else {
          r.word('under', '\'in\' or \'under\'');
          parent = r.name('a parent kind after \'under\'');
          if (r.accept('in')) register = r.name('a register name after \'in\'');
        }
        r.end();
        if (kindLine.has(name)) error(n, `kind ${name} is declared again (first at line ${kindLine.get(name)})`);
        else { kindLine.set(name, n); A.kinds.push({ name, parent, register, example, stated: register, line: n }); }
      } else if (head === 'type') {
        r.at++;
        const name = r.name('a type name');
        r.word('in');
        const register = r.name('a register name after \'in\'');
        r.end();
        if (typeLine.has(name)) error(n, `type ${name} is declared again (first at line ${typeLine.get(name)})`);
        else { typeLine.set(name, n); A.types.push({ name, register, line: n }); }
      } else if (head === 'block' || head === 'boundary') {
        r.at++;
        const boundary = head === 'boundary';
        const name = r.name(`a ${head} name`);
        r.word('yields');
        const yields = r.name('a kind after \'yields\'');
        r.word('at');
        const stage = r.name('a stage after \'at\'');
        const laws = [];
        if (!boundary && r.accept('laws')) {
          laws.push(r.name('a law after \'laws\''));
          while (r.accept(',')) laws.push(r.name('a law after \',\''));
        }
        r.word('{', boundary ? '\'{\'' : '\'laws\' or \'{\'');
        r.end();
        const block = { name, yields, stage, boundary, laws, inputs: [], outputs: [], line: n };
        // The body: to a '}' on a line of its own. A line that begins a statement ends the body unclosed, and is read again.
        let closed = false;
        for (i = i + 1; i < lines.length; i++) {
          const body = lines[i];
          if (!body.tokens.length) continue;
          const b = new Reader(body.tokens);
          const first = b.peek();
          if (first === '}') {
            try { b.at++; b.end(); } catch (e) { error(body.n, e.message); }
            closed = true;
            break;
          }
          if (STATEMENTS.includes(first)) { error(body.n, `expected '}' closing ${head} ${name} (line ${n}) before a new statement`); i--; closed = true; break; }
          try {
            const side = b.peek();
            if (side !== 'in' && side !== 'out') b.refuse('\'in\', \'out\' or \'}\'');
            b.at++;
            const port = b.name(`a port name after '${side}'`);
            b.word(':');
            const type = b.name('a type after \':\'');
            b.end();
            if (boundary && side === 'in') { error(body.n, `a boundary has outputs only: ${name}.${port} cannot be an input`); continue; }
            const list = side === 'in' ? block.inputs : block.outputs;
            if (list.some((p) => p.port === port)) error(body.n, `${side === 'in' ? 'input' : 'output'} ${name}.${port} is declared twice`);
            else list.push({ port, type, line: body.n });
          } catch (e) { if (e instanceof Refusal) error(body.n, e.message); else throw e; }
        }
        if (!closed) error(n, `expected '}' closing ${head} ${name}, found the end of the script`);
        if (blockLine.has(name)) error(n, `block ${name} is declared again (first at line ${blockLine.get(name)})`);
        else { blockLine.set(name, n); A.blocks.push(block); }
      } else if (head === 'wire') {
        r.at++;
        const from = r.name('a block name after \'wire\'');
        r.word('.');
        const out = r.name('a port name after \'.\'');
        r.word('->');
        const to = r.name('a block name after \'->\'');
        r.word('.');
        const inn = r.name('a port name after \'.\'');
        const next = r.accept('next');
        r.end();
        A.wires.push({ from, out, to, in: inn, next, line: n });
      } else if (head === '}') {
        error(n, 'a \'}\' closes no block');
      } else {
        r.refuse(`a statement (${STATEMENTS.join(', ')})`);
      }
    } catch (e) {
      if (e instanceof Refusal) error(n, e.message); else throw e;
    }
  }

  // ---- names resolved ----
  if (!seen.architecture) error(0, 'the script declares no architecture (architecture <Name> version <Integer>)');
  if (!seen.registers) error(0, 'the script declares no registers (registers <R> < ...)');
  if (!seen.stages) error(0, 'the script declares no stages (stages <S> < ...)');
  const registers = new Set(A.registers);
  const stages = new Set(A.stages.map((s) => s.name));
  for (const [name, { register, line }] of stageRegister) {
    if (!stages.has(name)) { error(line, `stage ${name} is not among the stages`); continue; }
    if (!registers.has(register)) error(line, `register ${register} is not declared`);
    A.stages.find((s) => s.name === name).register = register;
  }
  for (const s of A.stages) if (!stageRegister.has(s.name)) error(s.line, `stage ${s.name} is given no register (stage ${s.name} in <Register>)`);

  const kinds = new Map(A.kinds.map((k) => [k.name, k]));
  for (const k of A.kinds) {
    if (k.parent !== null && !kinds.has(k.parent)) error(k.line, `kind ${k.parent} is not declared`);
    else if (k.parent !== null && kinds.get(k.parent).example) error(k.line, `${k.parent} is an example kind, and has no kinds under it`);
    if (k.register !== null && !registers.has(k.register)) error(k.line, `register ${k.register} is not declared`);
  }
  // A register inherited along the parents; a kind on a cycle of parents is named.
  for (const k of A.kinds) {
    const path = [];
    let at = k;
    while (at && at.register === null && at.parent !== null && !path.includes(at)) { path.push(at); at = kinds.get(at.parent); }
    if (at && path.includes(at)) { if (path[0] === at) error(k.line, `kind ${k.name} is under itself (${path.map((p) => p.name).join(' under ')})`); continue; }
    if (at && at.register !== null) for (const p of [k, ...path]) if (p.register === null) p.register = at.register;
  }
  const types = new Map(A.types.map((t) => [t.name, t]));
  for (const t of A.types) if (!registers.has(t.register)) error(t.line, `register ${t.register} is not declared`);

  const blocks = new Map(A.blocks.map((b) => [b.name, b]));
  for (const b of A.blocks) {
    const kind = kinds.get(b.yields);
    if (!kind) error(b.line, `kind ${b.yields} is not declared`);
    else if (kind.example) error(b.line, `${b.yields} is an example kind, which no block yields`);
    if (!stages.has(b.stage)) error(b.line, `stage ${b.stage} is not declared`);
    for (const p of [...b.inputs, ...b.outputs]) if (!types.has(p.type)) error(p.line, `type ${p.type} is not declared`);
  }
  for (const w of A.wires) {
    const a = blocks.get(w.from), b = blocks.get(w.to);
    if (!a) error(w.line, `block ${w.from} is not declared`);
    if (!b) error(w.line, `block ${w.to} is not declared`);
    const out = a && a.outputs.find((p) => p.port === w.out), inn = b && b.inputs.find((p) => p.port === w.in);
    if (a && !out) error(w.line, `${w.from} has no output ${w.out}`);
    if (b && !inn) error(w.line, `${w.to} has no input ${w.in}`);
    if (out && inn && out.type !== inn.type) error(w.line, `wire ${w.from}.${w.out} -> ${w.to}.${w.in} joins ${out.type} to ${inn.type}`);
    w.type = out ? out.type : inn ? inn.type : null;
  }
  return A;
}
