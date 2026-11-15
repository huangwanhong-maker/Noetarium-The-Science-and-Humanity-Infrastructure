// A theory of change as a document (ADR 0122): written back into the policy domain's form from the structure its reader answers,
// and changed by operations that return a new theory and leave the old one as it was. The reading is the domain's alone
// (policy::change, asked through the API): nothing here reads the written form; what is written here is read there again, so that
// a change that makes no theory is refused by the reader and not by the page.
//
// A theory here is what the reader answers: {name, levels, nodes: [{id, kind, text}], links: [{from, to, assumption, support,
// sign, evidence: {given, effect, lower, upper, grade}}]}.

const word = (s) => String(s ?? '').trim().toLowerCase().replace(/[^\p{L}\p{N}]+/gu, '_').replace(/^_+|_+$/g, '');
const line = (s) => String(s ?? '').replace(/\s+/g, ' ').trim();
const copy = (t) => JSON.parse(JSON.stringify(t));

// The written form, a line each, as the domain reads it.
export function writeTheory(t) {
  const out = [`theory ${word(t.name) || 'a_theory'}`, `levels ${(t.levels || []).join(' ')}`];
  for (const n of t.nodes || []) out.push(`node ${n.id} ${word(n.kind) || 'outcome'}: ${line(n.text) || n.id}`);
  for (const l of t.links || []) {
    const ab = `${l.from} -> ${l.to}`;
    out.push(`link ${ab}: ${line(l.assumption) || 'no assumption is stated'}`);
    for (const f of l.support || []) if (line(f)) out.push(`support ${ab}: ${line(f)}`);
    if (l.evidence?.given && Number.isFinite(l.evidence.effect)) out.push(`evidence ${ab}: ${l.evidence.effect} [${l.evidence.lower}, ${l.evidence.upper}] ${l.evidence.grade}`);
    if (l.sign < 0) out.push(`sign ${ab}: -`);
  }
  return `${out.join('\n')}\n`;
}

// An identifier no node has yet, made from words.
export function freshId(t, words) {
  const base = word(words).split('_').filter(Boolean).slice(0, 2).join('_') || 'state';
  const taken = new Set((t.nodes || []).map((n) => n.id));
  if (!taken.has(base)) return base;
  for (let k = 2; ; k++) if (!taken.has(`${base}_${k}`)) return `${base}_${k}`;
}

export function addNode(t, { text, kind = 'outcome', after = null, assumption = '' }) {
  const u = copy(t), id = freshId(u, text);
  u.nodes.push({ id, kind, text: line(text) });
  if (after) u.links.push({ from: after, to: id, assumption: line(assumption), support: [], sign: 1, evidence: { given: false } });
  return { theory: u, id };
}

export function setNode(t, id, change) {
  const u = copy(t);
  const n = u.nodes.find((x) => x.id === id);
  if (n) Object.assign(n, { ...change, id });
  return u;
}

// A node removed, with the links at it.
export function removeNode(t, id) {
  const u = copy(t);
  u.nodes = u.nodes.filter((n) => n.id !== id);
  u.links = u.links.filter((l) => l.from !== id && l.to !== id);
  return u;
}

export function addLink(t, from, to, assumption = '') {
  const u = copy(t);
  if (from !== to && !u.links.some((l) => l.from === from && l.to === to)) u.links.push({ from, to, assumption: line(assumption), support: [], sign: 1, evidence: { given: false } });
  return u;
}

export function setLink(t, index, change) {
  const u = copy(t);
  if (u.links[index]) Object.assign(u.links[index], change);
  return u;
}

export function removeLink(t, index) {
  const u = copy(t);
  u.links.splice(index, 1);
  return u;
}

// The theory graded on other levels: evidence written on a link in a level the new scale lacks is dropped, and said.
export function setLevels(t, levels) {
  const u = copy(t), dropped = [];
  u.levels = [...levels];
  for (const l of u.links) if (l.evidence?.given && !levels.includes(l.evidence.grade)) { dropped.push(`${l.from} -> ${l.to}`); l.evidence = { given: false }; }
  return { theory: u, dropped };
}

// A stream of yearly amounts as a person writes it: amounts apart by commas, "a × n" for an amount n years running.
export function readStream(text) {
  const out = [];
  for (const part of String(text ?? '').split(/[,;\n]+/).map((s) => s.trim()).filter(Boolean)) {
    const m = part.match(/^(-?[\d.eE+ _]+?)\s*(?:[×x*]\s*(\d+))?$/);
    const v = m ? Number(m[1].replace(/[ _]/g, '')) : NaN;
    if (!m || !Number.isFinite(v)) throw new Error(`“${part}” is not an amount (write amounts apart by commas, and “150000 × 10” for ten years running)`);
    const n = m[2] ? Number(m[2]) : 1;
    if (n < 1 || n > 200) throw new Error(`“${part}” runs for between 1 and 200 years`);
    for (let k = 0; k < n; k++) out.push(v);
  }
  return out;
}

// And written back, runs of one amount gathered.
export function writeStream(values) {
  const parts = [];
  for (let i = 0; i < (values || []).length;) {
    let j = i;
    while (j < values.length && values[j] === values[i]) j++;
    parts.push(j - i > 1 ? `${values[i]} × ${j - i}` : `${values[i]}`);
    i = j;
  }
  return parts.join(', ');
}
