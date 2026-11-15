// Passages marked in a document's text (ADR 0121): where a candidate's passage stands, so that a page can mark it, and the text cut
// into runs each carrying the marks over it. A passage is found as tekmerion finds it, up to runs of whitespace; the engine and the
// API give its place as a byte offset in the text's UTF-8, which is turned here into the text's own characters. Pure.

const encoder = new TextEncoder();

// The index in the string of a byte offset of its UTF-8 (the start of the character holding that byte).
export function charAt(text, bytes) {
  let b = 0, i = 0;
  for (const ch of text) {
    if (b >= bytes) return i;
    b += encoder.encode(ch).length;
    i += ch.length;
  }
  return text.length;
}

// The byte offset of a character index.
export function byteAt(text, index) { return encoder.encode(text.slice(0, index)).length; }

const escapeRe = (s) => s.replace(/[.*+?^${}()|[\]\\]/g, '\\$&');

// Where the passage stands in the text, up to runs of whitespace: [start, end) in the string's indices, searched from the byte
// offset given (if any) and else from the start; null when it is not there.
export function locate(text, passage, atBytes = null) {
  const words = String(passage || '').trim().split(/\s+/).filter(Boolean);
  if (!words.length) return null;
  const re = new RegExp(words.map(escapeRe).join('\\s+'), 'g');
  re.lastIndex = atBytes === null || atBytes === undefined ? 0 : charAt(text, atBytes);
  let m = re.exec(text);
  if (!m && re.lastIndex !== 0) { re.lastIndex = 0; m = re.exec(text); }
  return m ? [m.index, m.index + m[0].length] : null;
}

// The text cut into runs at every mark's ends: [{start, end, text, marks: [ids]}], in order. Marks are {id, start, end}.
export function segments(text, marks = []) {
  const cuts = new Set([0, text.length]);
  for (const m of marks) if (m.start < m.end) cuts.add(Math.max(0, m.start)), cuts.add(Math.min(text.length, m.end));
  const points = [...cuts].sort((a, b) => a - b);
  const out = [];
  for (let k = 0; k + 1 < points.length; k++) {
    const s = points[k], e = points[k + 1];
    out.push({ start: s, end: e, text: text.slice(s, e), marks: marks.filter((m) => m.start <= s && e <= m.end).map((m) => m.id) });
  }
  return out;
}
