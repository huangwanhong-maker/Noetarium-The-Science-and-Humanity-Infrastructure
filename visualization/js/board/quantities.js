// Quantities on a board: assumptions that compute. A line of a note or of a figure that reads
//
//     name = expression [unit]
//
// defines a quantity, and every definition on a board is one system: a figure's formula may use the
// names that notes define, and a later figure the names of an earlier one. Pure.
//
//     population = 5.2M [4.8M, 5.6M] people       a value with the range it is believed to lie in
//     share = 3%..5%                              a range alone; its middle is taken as the value
//     spend = 40 [30, 55] AUD
//     TAM = population * spend                     + − * × / ÷ ( ) min( , ) max( , )
//
// Numbers take thousands separators and, written against them, the suffixes k, M, B, T and %. A value is carried as an
// interval [lo, hi] with a likely value within it, and intervals are combined by interval arithmetic:
// the result's range holds every value the inputs' ranges allow.
//
// Caveat. Interval arithmetic treats each occurrence of a name as free, so a name used twice may give
// a range wider than the true one (x − x is not [0, 0]). It never gives one narrower. Units are labels
// carried for reading; they are not checked.

const SUFFIX = { k: 1e3, K: 1e3, M: 1e6, B: 1e9, T: 1e12, '%': 1e-2 };
const NAME = /^[\p{L}_][\p{L}\p{N}_]*/u;

function tokens(text) {
  const out = [];
  let s = text;
  let at = 0;
  const push = (type, value, len) => { out.push({ type, value, at }); s = s.slice(len); at += len; };
  while (s.length) {
    const space = /^\s+/.exec(s);
    if (space) { s = s.slice(space[0].length); at += space[0].length; continue; }
    // A comma separates thousands only before three digits, so that [30, 55] is two numbers.
    const num = /^(\d{1,3}(?:,\d{3})+(?:\.\d+)?|\d+(?:\.\d+)?|\.\d+)(?:([kKMBT%])(?![\p{L}\p{N}_]))?/u.exec(s);
    if (num) { push('number', { v: Number(num[1].replace(/,/g, '')) * (SUFFIX[num[2]] || 1), percent: num[2] === '%' }, num[0].length); continue; }
    if (s.startsWith('..')) { push('..', '..', 2); continue; }
    const name = NAME.exec(s);
    if (name) { push('name', name[0], name[0].length); continue; }
    if ('+-−*×·/÷()[],='.includes(s[0])) { push(s[0], s[0], 1); continue; }
    push('other', s[0], 1);
  }
  return out;
}

// expression := term (('+' | '−') term)*; term := factor (('×' | '÷') factor)*; factor := '−' factor | primary;
// primary := number ('[' number ',' number ']')? | number '..' number | name | '(' expression ')' | (min | max) '(' expression (',' expression)* ')'
function parseExpression(toks, pos) {
  const peek = () => toks[pos], take = () => toks[pos++];
  const fail = (message) => { throw new Error(message); };
  const number = () => {
    const sign = peek()?.type === '-' || peek()?.type === '−' ? (take(), -1) : 1;
    const t = take();
    if (t?.type !== 'number') fail('a range is written with numbers');
    return { v: sign * t.value.v, percent: t.value.percent };
  };
  const primary = () => {
    const t = take();
    if (!t) fail('the expression ends where a value was expected');
    if (t.type === 'number') {
      if (peek()?.type === '[') {
        take();
        const lo = number();
        if (take()?.type !== ',') fail('a range is written [low, high]');
        const hi = number();
        if (take()?.type !== ']') fail('a range is written [low, high]');
        if (!(lo.v <= t.value.v && t.value.v <= hi.v)) fail(`the value ${t.value.v} is not within its range [${lo.v}, ${hi.v}]`);
        return { op: 'value', lo: lo.v, mid: t.value.v, hi: hi.v, percent: t.value.percent };
      }
      if (peek()?.type === '..') {
        take();
        const hi = number();
        if (hi.v < t.value.v) fail(`the range ${t.value.v}..${hi.v} runs downward`);
        return { op: 'value', lo: t.value.v, mid: (t.value.v + hi.v) / 2, hi: hi.v, percent: t.value.percent || hi.percent };
      }
      return { op: 'value', lo: t.value.v, mid: t.value.v, hi: t.value.v, percent: t.value.percent };
    }
    if (t.type === 'name') {
      if ((t.value === 'min' || t.value === 'max') && peek()?.type === '(') {
        take();
        const args = [expression()];
        while (peek()?.type === ',') { take(); args.push(expression()); }
        if (take()?.type !== ')') fail(`${t.value}( is not closed`);
        return { op: t.value, args };
      }
      return { op: 'name', name: t.value };
    }
    if (t.type === '(') {
      const e = expression();
      if (take()?.type !== ')') fail('a bracket is not closed');
      return e;
    }
    return fail(`'${t.value}' is not a value`);
  };
  const factor = () => {
    if (peek()?.type !== '-' && peek()?.type !== '−') return primary();
    take();
    // A minus before a number is the number's own sign, so that −2 [−3, −1] is a value with its range.
    if (peek()?.type === 'number') {
      const t = peek();
      toks[pos] = { ...t, value: { ...t.value, v: -t.value.v } };
      return primary();
    }
    return { op: 'neg', args: [factor()] };
  };
  const term = () => {
    let left = factor();
    while (peek() && '*×·/÷'.includes(peek().type) && peek().type.length === 1) {
      const t = take().type;
      left = { op: t === '/' || t === '÷' ? 'div' : 'mul', args: [left, factor()] };
    }
    return left;
  };
  function expression() {
    let left = term();
    while (peek()?.type === '+' || peek()?.type === '-' || peek()?.type === '−') {
      const t = take().type;
      left = { op: t === '+' ? 'add' : 'sub', args: [left, term()] };
    }
    return left;
  }
  const e = expression();
  return { e, pos };
}

// A line read as a definition: {name, expression, unit, literal}, or {name, error} when it begins as
// one and cannot be read, or null when it is not a definition at all (prose with no 'name =').
export function parseDefinition(line) {
  const head = /^\s*([\p{L}_][\p{L}\p{N}_]*)\s*=(?!=)/u.exec(line);
  if (!head) return null;
  const name = head[1], rest = line.slice(head[0].length);
  const toks = tokens(rest);
  if (!toks.length) return { name, error: 'nothing follows the equals sign' };
  try {
    const { e, pos } = parseExpression(toks, 0);
    // What follows a whole expression is its unit.
    const unit = pos < toks.length ? rest.slice(toks[pos].at).trim() : '';
    if (/[=()\[\]]/.test(unit)) return { name, error: `'${unit}' is not a unit` };
    return { name, expression: e, unit, literal: e.op === 'value', percent: e.op === 'value' && Boolean(e.percent) };
  } catch (error) {
    return { name, error: error.message };
  }
}

export const definitionsOf = (text) => String(text || '').split(/\r?\n/).map(parseDefinition).filter(Boolean);

// Every definition on a board, with the item that makes it: those of notes from their text, those of
// figures from their formulas.
export function collect(board) {
  const defs = [];
  for (const id of board.order) {
    const item = board.items[id];
    const text = item.kind === 'note' ? item.text : item.kind === 'figure' ? item.formula : null;
    if (text === null) continue;
    for (const d of definitionsOf(text)) defs.push({ ...d, item: id, kind: item.kind, type: item.type });
  }
  return defs;
}

const names = (e, into = new Set()) => {
  if (e.op === 'name') into.add(e.name);
  for (const a of e.args || []) names(a, into);
  return into;
};

// Interval arithmetic, with the likely value carried beside the bounds.
function combine(op, args) {
  const [a, b] = args;
  switch (op) {
    case 'neg': return { lo: -a.hi, mid: -a.mid, hi: -a.lo };
    case 'add': return { lo: a.lo + b.lo, mid: a.mid + b.mid, hi: a.hi + b.hi };
    case 'sub': return { lo: a.lo - b.hi, mid: a.mid - b.mid, hi: a.hi - b.lo };
    case 'mul': { const p = [a.lo * b.lo, a.lo * b.hi, a.hi * b.lo, a.hi * b.hi]; return { lo: Math.min(...p), mid: a.mid * b.mid, hi: Math.max(...p) }; }
    case 'div': {
      if (b.lo <= 0 && b.hi >= 0) throw new Error('it divides by a quantity whose range holds zero');
      const p = [a.lo / b.lo, a.lo / b.hi, a.hi / b.lo, a.hi / b.hi];
      return { lo: Math.min(...p), mid: a.mid / b.mid, hi: Math.max(...p) };
    }
    case 'min': return { lo: Math.min(...args.map((v) => v.lo)), mid: Math.min(...args.map((v) => v.mid)), hi: Math.min(...args.map((v) => v.hi)) };
    case 'max': return { lo: Math.max(...args.map((v) => v.lo)), mid: Math.max(...args.map((v) => v.mid)), hi: Math.max(...args.map((v) => v.hi)) };
    default: throw new Error(`there is no operation '${op}'`);
  }
}

// The values of a system of definitions: a map from each name to {lo, mid, hi, unit, item, uses,
// rests} or to {error, item}. `uses` are the names its expression mentions; `rests` the names of the
// values written down (not computed) that it comes from in the end. A name defined twice, a name
// that is not defined and a definition that needs itself are refused, each with its reason.
export function evaluate(defs) {
  const byName = new Map(), out = new Map();
  for (const d of defs) {
    if (byName.has(d.name)) { if (!byName.get(d.name).twice) byName.get(d.name).twice = true; continue; }
    byName.set(d.name, { ...d });
  }
  const state = new Map();
  const value = (name, trail) => {
    if (out.has(name)) return out.get(name);
    const d = byName.get(name);
    const done = (v) => { out.set(name, { ...v, item: d?.item, name }); return out.get(name); };
    if (!d) return { error: `'${name}' is not defined on the board`, name };
    if (d.error) return done({ error: d.error });
    if (d.twice) return done({ error: `'${name}' is defined more than once` });
    if (state.get(name) === 1) return { error: `'${name}' needs itself: ${[...trail.slice(trail.indexOf(name)), name].join(' → ')}`, name, cycle: true };
    state.set(name, 1);
    const uses = [...names(d.expression)], rests = new Set();
    let failure = null;
    const walk = (e) => {
      if (failure) return null;
      if (e.op === 'value') return { lo: e.lo, mid: e.mid, hi: e.hi };
      if (e.op === 'name') {
        const v = value(e.name, [...trail, name]);
        if (v.error) { failure = v.cycle ? v.error : v.name === e.name && !byName.has(e.name) ? v.error : `it uses '${e.name}', which has no value`; return null; }
        for (const r of v.rests) rests.add(r);
        return v;
      }
      const args = e.args.map(walk);
      if (failure) return null;
      try { return combine(e.op, args); } catch (error) { failure = error.message; return null; }
    };
    const v = walk(d.expression);
    state.set(name, 2);
    if (failure) return done({ error: failure, uses });
    if (d.literal) rests.add(name);
    return done({ lo: v.lo, mid: v.mid, hi: v.hi, unit: d.unit, percent: d.percent, literal: d.literal, uses, rests: [...rests] });
  };
  for (const name of byName.keys()) value(name, []);
  return out;
}

// A number for reading: three significant figures, with k, M, B or T; a share as a percentage.
export function formatNumber(v, { percent = false } = {}) {
  if (!Number.isFinite(v)) return '—';
  if (percent) return `${trim(v * 100)}%`;
  const a = Math.abs(v);
  for (const [from, unit, suffix] of [[1e12, 1e12, ' T'], [1e9, 1e9, ' B'], [1e6, 1e6, ' M'], [1e4, 1e3, ' k']]) if (a >= from) return `${trim(v / unit)}${suffix}`;
  return trim(v);
}
function trim(v) {
  const a = Math.abs(v);
  const s = a >= 100 ? v.toFixed(0) : a >= 10 ? v.toFixed(1) : a >= 1 ? v.toFixed(2) : v.toPrecision(2);
  const t = s.includes('.') && !s.includes('e') ? s.replace(/\.?0+$/, '') : s;
  return (t.includes('.') || t.includes('e') ? t : t.replace(/\B(?=(\d{3})+(?!\d))/g, ',')).replace('-', '−');
}

// A value in words: '5.2 M people (4.8 M – 5.6 M)'; a value without a range has none to show.
export function formatValue(v) {
  if (!v || v.error) return v?.error || '';
  const opt = { percent: v.percent };
  const unit = v.unit ? ` ${v.unit}` : '';
  const main = `${formatNumber(v.mid, opt)}${unit}`;
  return v.lo === v.hi ? main : `${main} (${formatNumber(v.lo, opt)} – ${formatNumber(v.hi, opt)})`;
}
