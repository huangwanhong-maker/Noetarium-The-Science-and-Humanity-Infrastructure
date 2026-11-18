// Transfer (the design of 2026-11-19, §6.3; kybernesis's transfer): a practice carried from a system where it works to another, read in
// the source's terms by a lens. The page is the drawing: the source at the left, the target at the right, each support factor a
// plank between them, laid, half laid with its interval, or missing; above, whether it will work here; beneath, each reading's line.
// A transfer is chosen among the examples' or written as JSON, and assessed; the transfers this project kept are listed beneath,
// newest first, each drawn again when chosen.

import { svgTransfer, STANDINGS } from '/visualization/js/steering/index.js';
import { G, base, read, send, when, fig } from './_engagement.js';

export function view() {
  return {
    id: 'transfer', label: 'Transfer', draw,
    heard: (e, ctx) => {
      if (!/transfer/.test(e.what)) return;
      G.state = null;
      if (document.querySelector('.tab.on')?.dataset.tab === 'transfer') ctx.show('transfer');
    },
  };
}

// Every transfer the examples declare, by name.
function declared() {
  const out = [];
  for (const e of G.examples || []) for (const [name, t] of Object.entries(e.transfers || {})) out.push({ name, example: e.title, transfer: t });
  return out;
}

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('eg-view', 'eg-transfer-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'eg-note' }, 'No project is open.'));
  await read(ctx);
  ctx.here('a transfer');
  const nk = (G.state?.transfers || []).length;
  ctx.note(`${nk} transfer${nk === 1 ? '' : 's'} kept`);
  const redraw = () => draw(pane, ctx);
  const all = declared();
  if (G.pick === null && all.length) G.pick = all[0].name;
  const chosen = all.find((d) => d.name === G.pick) || null;
  if (G.editFor !== G.pick) {
    G.editing = chosen ? JSON.stringify(chosen.transfer, null, 1) : G.editing || '';
    G.editFor = G.pick;
  }
  const head = el('div', { class: 'eg-head' },
    el('div', { class: 'eg-title-row' }, el('h2', { class: 'eg-title' }, 'Transfer')),
    el('p', { class: 'eg-about' }, 'A practice carried from a system where it works to another, read in the source’s terms by a lens. Each support factor is a plank between them: laid where the target is believed to meet it, half laid where that is arguable, missing where it is untenable.'));
  const chips = el('div', { class: 'eg-acts' },
    el('div', { class: 'eg-group' }, el('span', { class: 'eg-label' }, 'Transfer'),
      all.map((d) => el('button', { type: 'button', class: `eg-chip${G.pick === d.name ? ' on' : ''}`, title: `from the example ${d.example}`, onclick: () => { G.pick = d.name; G.assessed = null; redraw(); } }, d.transfer.title || d.name)),
      el('button', { type: 'button', class: `eg-chip${G.pick === '' ? ' on' : ''}`, onclick: () => { G.pick = ''; G.assessed = null; redraw(); } }, 'Of your own')),
    el('div', { class: 'eg-group eg-buttons' },
      el('button', { type: 'button', class: 'eg-go', disabled: !!G.busy, onclick: () => assess(ctx, redraw) }, 'Assess')));
  const editor = el('details', { class: 'eg-declared', open: G.pick === '' || !!G.editOpen, ontoggle: (e) => { G.editOpen = e.target.open; } },
    el('summary', {}, 'The transfer as declared'),
    el('p', { class: 'eg-small' }, 'Two systems by their components (named conditions, each true or false) and, for each action, the probability of success by rules over them; the readings give the probability of each condition in the target; each condition is a support factor.'),
    el('textarea', { class: 'eg-code', rows: 18, spellcheck: 'false', 'aria-label': 'The transfer as JSON', oninput: (e) => { G.editing = e.target.value; } }, G.editing));
  const said = G.busy ? el('p', { class: 'eg-busy' }, G.busy) : G.said && G.saidFor === 'transfer' ? el('p', { class: `eg-said ${G.saidKind}` }, G.said) : null;
  const drawing = el('div', { class: 'eg-transfer-drawing' });
  if (G.assessed) {
    const req = G.assessed.request || {};
    drawing.innerHTML = svgTransfer(G.assessed.result, { source: req.source?.name, target: req.target?.name, title: G.assessed.title || req.title, ground: null });
  } else {
    drawing.append(el('p', { class: 'eg-note' }, chosen ? `Assess ${chosen.transfer.title || chosen.name} to see whether it will work here.` : 'Write a transfer of your own as JSON, and assess it.'));
  }
  const notes = G.assessed && (G.assessed.request?.notes || chosen?.transfer?.notes) ? el('p', { class: 'eg-notes' }, G.assessed.request?.notes || chosen.transfer.notes) : null;
  fill(pane, head, chips, said, drawing, notes, editor, kept(ctx, redraw));
}

async function assess(ctx, redraw) {
  let request;
  try { request = JSON.parse(G.editing); } catch (error) { G.said = `The transfer is not JSON: ${error.message}`; G.saidKind = 'refused'; G.saidFor = 'transfer'; return redraw(); }
  G.busy = 'Assessing the transfer…';
  redraw();
  try {
    const r = await send(ctx, `${base(ctx)}/transfer`, request);
    if (r) {
      G.assessed = { result: r.result, request, title: request.title || '' };
      const S = STANDINGS[r.result.standing] || STANDINGS.undecided;
      G.said = `${S.glyph} It will work here: ${S.word}, probability ${fig(r.result.low)} to ${fig(r.result.high)}${r.kept ? '; kept among the transfers' : '; not kept (this page may only look)'}.`;
      G.saidKind = 'made';
      ctx.log(G.said, 'said');
      await read(ctx, true);
    }
  } catch (error) {
    G.said = error.message;
    G.saidKind = 'refused';
    ctx.log(error.message, 'wrong');
  }
  G.saidFor = 'transfer';
  G.busy = '';
  redraw();
}

function kept(ctx, redraw) {
  const { el } = ctx;
  const list = G.state?.transfers || [];
  return el('div', { class: 'eg-kept' },
    el('h4', {}, `Transfers kept · ${list.length}`),
    list.length ? el('ul', { class: 'eg-kept-list' }, list.map((k, i) => {
      const S = STANDINGS[k.result?.standing] || STANDINGS.undecided;
      return el('li', {}, el('button', { type: 'button', class: `eg-kept-row${G.assessed?.index === i ? ' on' : ''}`, onclick: () => { G.assessed = { result: k.result, request: k.request, title: k.title, index: i }; redraw(); } },
        el('span', { class: `eg-standing-mark st-${k.result?.standing || 'undecided'}` }, `${S.glyph} ${S.word}`),
        el('span', { class: 'eg-kept-title' }, k.title || `${k.request?.source?.name || 'a source'} to ${k.request?.target?.name || 'a target'}`),
        el('span', { class: 'eg-small' }, `${fig(k.result?.low)} to ${fig(k.result?.high)} · ${k.by || ''} · ${when(k.when)}`)));
    })) : el('p', { class: 'eg-note' }, 'None yet: an assessment is kept here when it is made from this page.'));
}
