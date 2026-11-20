// The procedure of a round drawn (ADR 0136), as markup that needs no page: its record as a chain of sealed entries, each act with
// its glyph and words, and the rules of procedural justice as seals beneath, a held rule a whole seal, a failed one broken, one left
// to people open, each also told by its word. It takes what the API's allocation engine answers to `procedure`.

import { esc, textWidth } from '../history/cone.js';

export const ACT_GLYPHS = { declare: '§', consult: '◎', interest: '⚑', oversee: '⚖', allocate: '→', appeal: '↺', review: '✓', commit: '◆', draw: '◇' };
export const STATUS_WORDS = { holds: 'holds', fails: 'fails', undecided: 'undecided' };
// An undecided rule that needs a person's judgement is left to people; one not yet tried is undecided.
const wordOf = (rule) => (rule.status === 'undecided' && /judgement/.test(rule.reason || '') ? 'left to people' : STATUS_WORDS[rule.status] || rule.status);

export function svgRecord(procedure, options = {}) {
  const J = procedure.journal || [];
  const linkW = options.linkW ?? 128, top = 30, linkH = 74, pad = 14;
  const perRow = Math.max(1, Math.floor(((options.width ?? 900) - 2 * pad) / linkW));
  const rows = Math.ceil(J.length / perRow) || 1;
  const rulesTop = top + rows * (linkH + 18) + 26;
  const R = procedure.rules || [];
  const ruleW = 168, perRuleRow = Math.max(1, Math.floor(((options.width ?? 900) - 2 * pad) / ruleW));
  const width = Math.max(options.width ?? 900, 2 * pad + Math.min(J.length, perRow) * linkW);
  const height = rulesTop + Math.ceil(R.length / perRuleRow) * 64 + 20;
  const parts = [`<text class="head" x="${pad}" y="18">The record, sealed: ${J.length} entries, the last seal ${esc((procedure.head || '').slice(0, 12))}…</text>`];
  J.forEach((e, k) => {
    const r = Math.floor(k / perRow), c = k % perRow;
    const x = pad + c * linkW, y = top + r * (linkH + 18);
    const glyph = ACT_GLYPHS[e.what] || '·';
    const note = e.note || e.about || '';
    const shortNote = textWidth(note, 10.5) > linkW - 18 ? note.slice(0, Math.max(6, Math.floor(note.length * (linkW - 18) / textWidth(note, 10.5)) - 1)) + '…' : note;
    parts.push(`<g class="entry ${esc(e.what)}" data-entry="${k}"><rect class="link" x="${x}" y="${y}" width="${linkW - 12}" height="${linkH}" rx="${linkH / 2.6}"/>` +
      `<text class="glyph" x="${x + 14}" y="${y + 24}">${glyph}</text><text class="act" x="${x + 32}" y="${y + 24}">${esc(e.what)}</text>` +
      `<text class="who" x="${x + 14}" y="${y + 40}">${esc(e.who)}</text><text class="note" x="${x + 14}" y="${y + 54}">${esc(shortNote)}</text>` +
      `<text class="seal" x="${x + 14}" y="${y + 67}">${esc((e.seal || '').slice(0, 10))}</text>` +
      (c < perRow - 1 && k < J.length - 1 ? `<line class="joint" x1="${x + linkW - 12}" y1="${y + linkH / 2}" x2="${x + linkW}" y2="${y + linkH / 2}"/>` : '') +
      `<title>${esc(e.when)} · ${esc(e.who)} · ${esc(e.what)} · ${esc(e.note || '')}</title></g>`);
  });
  parts.push(`<text class="head" x="${pad}" y="${rulesTop - 8}">The rules of procedural justice</text>`);
  R.forEach((rule, k) => {
    const r = Math.floor(k / perRuleRow), c = k % perRuleRow;
    const x = pad + c * ruleW, y = rulesTop + r * 64;
    const s = rule.status;
    const seal = s === 'holds' ? `<circle class="seal holds" cx="${x + 14}" cy="${y + 18}" r="10"/>`
      : s === 'fails' ? `<path class="seal fails" d="M${x + 4},${y + 18} a10,10 0 0 1 9,-10 l-2,8 l4,4 l-3,8 a10,10 0 0 1 -8,-10 Z M${x + 16},${y + 8} a10,10 0 0 1 8,10 a10,10 0 0 1 -7,10 l2,-7 l-4,-4 Z"/>`
        : `<circle class="seal open" cx="${x + 14}" cy="${y + 18}" r="10"/>`;
    parts.push(`<g class="rule ${esc(s)}" data-rule="${esc(rule.name)}">${seal}<text class="rule-name" x="${x + 30}" y="${y + 16}">${esc(rule.name)}</text>` +
      `<text class="rule-word" x="${x + 30}" y="${y + 31}">${esc(wordOf(rule))} · ${esc(rule.framework)}</text><title>${esc(rule.claim)}: ${esc(rule.reason)}</title></g>`);
  });
  return `<svg class="allocation-record" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${width} ${height}" width="${width}" height="${height}" role="img" aria-label="the procedure's record and its rules">${parts.join('')}</svg>`;
}

// A budget decided by votes, read by the voters' groups: for each group a row, and on it a mark for each rule at the share of the
// group's voters left with none of their projects funded, the rules told apart by glyph and word as well as by ink.
export const RULE_MARKS = { recorded: ['■', '#33303a'], greedy: ['●', '#b8577a'], greedy_per_cost: ['○', '#c18434'], equal_shares: ['▲', '#1c8580'], phragmen: ['◆', '#5469b4'] };

export function svgBudget(budget, options = {}) {
  const rules = budget.rules || [];
  const groups = (rules[0] && rules[0].groups) || [];
  if (!groups.length) return '';
  const labelW = Math.min(200, Math.max(90, ...groups.map((g) => textWidth(String(g.group), 12) + 12))), plotW = options.plotW ?? 360, rowH = 22, top = 46;
  const hi = Math.max(0.05, ...rules.flatMap((r) => r.groups.map((g) => g.share_with_nothing ?? 0)));
  const x = (v) => labelW + (plotW * v) / hi;
  const order = groups.map((g, k) => k).sort((a, b) => (rules[0].groups[b].share_with_nothing ?? 0) - (rules[0].groups[a].share_with_nothing ?? 0));
  const W = labelW + plotW + 60, H = top + groups.length * rowH + 30;
  const parts = [`<text class="head" x="0" y="14">Voters left with none of their projects funded, by group</text>`];
  rules.forEach((r, j) => {
    const [g, ink] = RULE_MARKS[r.rule] || ['·', '#88828c'];
    parts.push(`<text class="key" x="${labelW + j * 118}" y="32" style="fill:${ink}">${g} ${esc(r.rule.replace(/_/g, ' '))}</text>`);
  });
  order.forEach((gi, k) => {
    const y = top + k * rowH + rowH / 2;
    parts.push(`<text class="name" x="${labelW - 8}" y="${y + 4}" text-anchor="end">${esc(String(groups[gi].group))}</text><line class="row" x1="${labelW}" y1="${y}" x2="${labelW + plotW}" y2="${y}"/>`);
    rules.forEach((r) => {
      const v = r.groups[gi].share_with_nothing;
      if (v === null || v === undefined) return;
      const [g, ink] = RULE_MARKS[r.rule] || ['·', '#88828c'];
      parts.push(`<text class="mark" x="${x(v).toFixed(1)}" y="${y + 4}" text-anchor="middle" style="fill:${ink}">${g}<title>${esc(r.rule)}: ${(100 * v).toFixed(1)}% of ${groups[gi].voters} voters</title></text>`);
    });
  });
  const tk = [0, hi / 2, hi];
  for (const v of tk) parts.push(`<text class="tick" x="${x(v).toFixed(1)}" y="${H - 8}" text-anchor="middle">${(100 * v).toFixed(0)}%</text>`);
  return `<svg class="allocation-budget" xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${W} ${H}" width="${W}" height="${H}" role="img" aria-label="voters left with nothing, by group and rule">${parts.join('')}</svg>`;
}
