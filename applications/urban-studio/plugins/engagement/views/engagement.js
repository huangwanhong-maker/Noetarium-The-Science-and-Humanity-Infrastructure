// From evidence to action (the design of 2026-11-19, §4 and §6.3): the trajectory of an engagement as the page. The turns are columns
// and the blocks of the loop rows; each cell is a block's entry at a turn, told by a glyph and a word (computed, observed, entered,
// amended, accepted, held, a proposal awaiting review, stale, inherited, the world). A cell opens where it stands, beside the lattice
// at its row, with what the entry is (its act, who made it and in what role, its family, its remarks, why it is stale, whence it is
// inherited) and its values drawn for what they are: findings with their standing over the readings, a belief's intervals, the needs
// taken up and the practices that answer them, the advice and the inquiry. Beside it, the acts the person may take there: fill,
// amend, review, attach a file, and what rests on the entry. Above the lattice, the branch shown, the turn chosen and the acts on it
// (the ledger gathered, the turn run, run again from it, a branch taken), the role the person says they act in, and revisiting (the
// record verified, replayed, two turns compared, its lineage as PROV-JSON). Every write says the SHA-256 it was made on; an engagement
// changed meanwhile is shown as it stands, and the act is not made.

import { svgTrajectory, layoutTrajectory, ENTRY_STATES, STANDINGS, FAMILIES, parseArchitecture, entryOf } from '/visualization/js/steering/index.js';
import { G, ROLES, base, read, send, role, setRole, empowered, empoweredRoles, fig, pct, amount, interval, when, save, scaleMarkup, enc } from './_engagement.js';

export function view() {
  return {
    id: 'engagement', label: 'Evidence to action', draw,
    heard: (e, ctx) => {
      if (!/engagement|transfer/.test(e.what)) return;
      G.state = null;
      if (document.querySelector('.tab.on')?.dataset.tab === 'engagement') ctx.show('engagement');
    },
  };
}

const STATUS = { holds: { glyph: '✓', word: 'holds' }, fails: { glyph: '✕', word: 'fails' }, open: { glyph: '◐', word: 'open' }, undetermined: { glyph: '?', word: 'undetermined' } };
const KIND_SAID = { need: 'need', problem: 'problem', risk: 'risk', asset: 'asset', feature: 'feature' };
// What a port holds when nothing is there yet, by its type.
const EMPTY = { Traces: [], Words: [], Terms: {}, Operators: [], Standards: [], Rules: [], Proposals: [], Practice: [], Readings: {}, Belief: {}, Evaluations: [], PolicyVersion: {}, Action: {}, Inquiry: {} };

// ---- the page ---------------------------------------------------------------------------------------------------------------------

async function draw(pane, ctx) {
  const { el, fill } = ctx;
  pane.classList.add('eg-view');
  if (!ctx.project) return fill(pane, el('p', { class: 'eg-note' }, 'No project is open.'));
  await read(ctx);
  ctx.here('the engagement');
  const redraw = () => draw(pane, ctx);
  if (!G.state) return fill(pane, el('p', { class: 'eg-wrong' }, G.wrong || 'The engagement could not be read.'));
  if (!G.state.kept) { ctx.note('no engagement kept yet'); return fill(pane, head(ctx, redraw), begin(ctx, redraw)); }
  const v = G.state.view;
  const nb = (v?.branches || []).length;
  ctx.note(`${v?.entries ?? 0} entries · ${nb} branch${nb === 1 ? '' : 'es'}`);
  const names = (v?.branches || []).map((b) => b.name);
  if (G.branch !== '*' && !names.includes(G.branch)) G.branch = names[0] || 'main';
  const last = lastTurn(v, G.branch);
  if (G.turn === null || G.turn > last + 1) G.turn = Math.max(0, last);
  fill(pane, head(ctx, redraw), acts(ctx, redraw), standing(ctx, redraw), revisiting(ctx, redraw), stage(ctx, redraw));
}

const lastTurn = (v, branch) => {
  const bs = (v?.branches || []).filter((b) => branch === '*' || b.name === branch);
  return Math.max(-1, ...bs.flatMap((b) => (b.turns || []).map((t) => t.turn)));
};
const cellOf = (v, sel) => sel && (v?.branches || []).find((b) => b.name === sel.branch)?.turns?.find((t) => t.turn === sel.turn)?.blocks?.find((c) => c.block === sel.block);
// The block's last value before a turn, read from a branch (its inherited turns among them).
function earlier(v, branch, turn, block) {
  const b = (v?.branches || []).find((x) => x.name === branch);
  for (let k = turn - 1; k >= 0; k--) {
    const c = b?.turns?.find((t) => t.turn === k)?.blocks?.find((x) => x.block === block);
    if (c?.values && Object.keys(c.values).length) return { turn: k, branch, values: c.values };
  }
  return null;
}
const entryRecord = (n) => (G.state?.trajectory?.entries || []).find((e) => e.id === n) || null;
function architecture() {
  const script = G.state?.trajectory?.architecture || '';
  if (G.archFor !== script) { G.arch = script ? parseArchitecture(script) : null; G.archFor = script; }
  return G.arch;
}

// A write, with the page busy meanwhile and what came of it said.
async function act(ctx, redraw, words, path, body, after = null) {
  if (G.busy) return;
  G.busy = words;
  G.said = '';
  redraw();
  try {
    const r = await send(ctx, path, { ...body, base: G.sha });
    if (r) {
      const o = r.outcome;
      const p = r.progress;
      const claim = o?.claim ? `${o.claim[0].toUpperCase()}${o.claim.slice(1)}` : 'Done';
      G.said = after ? after(r) : o ? (o.holds || o.undecided ? `${claim}${o.reason ? `: ${o.reason}` : ''}.` : `Not made: ${o.reason || 'refused'}.`)
        : p ? `${p.made.length} entr${p.made.length === 1 ? 'y' : 'ies'} made${p.waiting.length ? `; waiting: ${p.waiting.map((w) => `${w.block} (${w.why})`).join('; ')}` : ''}${p.stale.length ? `; stale: ${p.stale.map((s) => `entry ${s.entry} (${s.why})`).join('; ')}` : ''}.` : 'Done.';
      G.saidKind = o && !o.holds && !o.undecided ? 'refused' : 'made';
      G.saidFor = 'engagement';
      if (G.saidKind === 'made') G.reason = '';
      ctx.log(G.said, G.saidKind === 'refused' ? 'wrong' : 'said');
    }
  } catch (error) {
    G.said = error.message;
    G.saidKind = 'refused';
    G.saidFor = 'engagement';
    ctx.log(error.message, 'wrong');
  } finally {
    G.busy = '';
    redraw();
  }
}

// ---- the head: the title and the role -------------------------------------------------------------------------------------------

function head(ctx, redraw) {
  const { el } = ctx;
  const r = role();
  return el('div', { class: 'eg-head' },
    el('div', { class: 'eg-title-row' },
      el('h2', { class: 'eg-title' }, 'From evidence to action'),
      el('label', { class: 'eg-role' }, el('span', {}, 'Acting as'),
        el('select', { 'aria-label': 'The role you act in', onchange: (e) => { setRole(e.target.value); ctx.log(`You act as ${e.target.value} from now on: it is recorded with each of your acts.`, 'said'); redraw(); } },
          ROLES.map((x) => el('option', { value: x.id, selected: x.id === r }, x.label))),
        el('span', { class: 'eg-small' }, 'stated by you, recorded with each act'))),
    el('p', { class: 'eg-about' }, 'An engagement with a city kept as a trajectory: every block of the loop at every turn, made by a method or by a person, sealed in a journal. A cell opens where it stands.'));
}

// ---- before anything is kept: an example, or an empty engagement ----------------------------------------------------------------

function begin(ctx, redraw) {
  const { el } = ctx;
  return el('div', { class: 'eg-begin' },
    el('p', {}, 'This project keeps no engagement yet. Lay one out from an example, with its constitution, its practices and its transfers declared, or open an empty one on kybernesis’s standard architecture and fill its blocks yourself.'),
    el('div', { class: 'eg-examples' }, (G.examples || []).map((e) => el('div', { class: 'eg-example' },
      el('h3', {}, e.title), el('p', { class: 'eg-small' }, e.about),
      el('button', { type: 'button', class: 'eg-go', disabled: !!G.busy, onclick: () => act(ctx, redraw, 'Laying out the example…', `${base(ctx)}/start`, { example: e.name }, () => `Laid out from the example ${e.title}.`) }, 'Start from this example')))),
    el('button', { type: 'button', class: 'eg-quiet', disabled: !!G.busy, onclick: () => act(ctx, redraw, 'Opening…', `${base(ctx)}/open`, {}, () => 'An empty engagement is open on the standard architecture.') }, 'Open an empty engagement'),
    G.busy ? el('p', { class: 'eg-busy' }, G.busy) : null,
    G.said ? el('p', { class: `eg-said ${G.saidKind}` }, G.said) : null);
}

// ---- the acts on a turn ---------------------------------------------------------------------------------------------------------

function acts(ctx, redraw) {
  const { el } = ctx;
  const v = G.state.view;
  const branches = v?.branches || [];
  const last = lastTurn(v, G.branch === '*' ? 'main' : G.branch);
  const turns = [...Array(last + 2).keys()];
  const branch = G.branch === '*' ? 'main' : G.branch;
  const chip = (on, label, onclick, title = '') => el('button', { type: 'button', class: `eg-chip${on ? ' on' : ''}`, title, onclick }, label);
  const busy = !!G.busy;
  const row = el('div', { class: 'eg-acts' },
    el('div', { class: 'eg-group' }, el('span', { class: 'eg-label' }, 'Branch'),
      branches.map((b) => chip(G.branch === b.name, b.name, () => { G.branch = b.name; G.sel = null; redraw(); }, b.parent ? `from ${b.parent} at turn ${b.from}: ${b.reason}` : 'the main line')),
      branches.length > 1 ? chip(G.branch === '*', 'every branch', () => { G.branch = '*'; redraw(); }) : null),
    el('div', { class: 'eg-group' }, el('span', { class: 'eg-label' }, 'Turn'),
      turns.map((k) => chip(G.turn === k, k === last + 1 ? `turn ${k} · next` : `turn ${k}`, () => { G.turn = k; redraw(); }))),
    el('div', { class: 'eg-group eg-buttons' },
      el('button', { type: 'button', class: 'eg-quiet', disabled: busy, title: 'the evidence the project’s tools answered, observed at Collection',
        onclick: () => act(ctx, redraw, 'Gathering the ledger…', `${base(ctx)}/gather`, { turn: G.turn, branch, role: role() }) }, 'Gather the ledger'),
      el('button', { type: 'button', class: 'eg-go', disabled: busy, title: 'every block the predefined methods can fill at this turn',
        onclick: () => act(ctx, redraw, `Running turn ${G.turn}…`, `${base(ctx)}/turn`, { turn: G.turn, branch }) }, 'Run the turn'),
      el('button', { type: 'button', class: 'eg-quiet', disabled: busy || G.turn > last, title: 'every turn from this one, again, by the predefined methods',
        onclick: () => act(ctx, redraw, `Running again from turn ${G.turn}…`, `${base(ctx)}/turn`, { turn: G.turn, branch, from: G.turn }) }, 'Run again from it'),
      el('button', { type: 'button', class: `eg-quiet${G.mode === 'branch' ? ' on' : ''}`, disabled: busy, onclick: () => { G.mode = G.mode === 'branch' ? null : 'branch'; redraw(); } }, 'Take a branch')));
  const out = [row];
  if (G.mode === 'branch') {
    const name = el('input', { type: 'text', class: 'eg-input', placeholder: 'a name, such as flash-flood', 'aria-label': 'The branch’s name' });
    const why = el('input', { type: 'text', class: 'eg-input wide', placeholder: 'why: another reading, another standard, another option', 'aria-label': 'Why the branch is taken' });
    out.push(el('div', { class: 'eg-form' },
      el('span', {}, `A branch from ${branch} at turn ${G.turn}: it reads the turns before from ${branch} and holds its own from turn ${G.turn}.`),
      name, why,
      el('button', { type: 'button', class: 'eg-go', disabled: busy, onclick: () => {
        const n = name.value.trim(), r = why.value.trim();
        if (!n || !r) { G.said = 'A branch is named, and says why it is taken.'; G.saidKind = 'refused'; return redraw(); }
        G.mode = null;
        act(ctx, redraw, 'Taking a branch…', `${base(ctx)}/act`, { act: 'branch', from: branch, turn: G.turn, name: n, reason: r, role: role() }, (a) => {
          if (a.outcome?.holds) { G.branch = n; return `The branch ${n} is taken from ${branch} at turn ${G.turn}.`; }
          return `Not taken: ${a.outcome?.reason || 'refused'}.`;
        });
      } }, 'Take it')));
  }
  return out;
}

// ---- what the record says of itself, and of the last act -----------------------------------------------------------------------

function standing(ctx) {
  const { el } = ctx;
  const v = G.state.view || {};
  const ver = v.verified || {};
  return el('div', { class: 'eg-standing' },
    el('span', { class: `eg-seal ${ver.holds ? 'holds' : 'fails'}` }, ver.holds ? '✓ sealed' : ver.undecided ? '? undecided' : '✕ does not verify'),
    el('span', { class: 'eg-small' }, `${v.entries ?? 0} entries · journal head `, el('code', { title: v.head || '' }, String(v.head || '').slice(0, 16) || '—'), v.head ? '…' : ''),
    G.busy ? el('span', { class: 'eg-busy' }, G.busy) : null,
    G.said && G.saidFor !== 'transfer' ? el('p', { class: `eg-said ${G.saidKind}` }, G.said) : null);
}

// ---- revisiting: verify, replay, the difference between two turns, the lineage -----------------------------------------------------

function revisiting(ctx, redraw) {
  const { el } = ctx;
  const v = G.state.view || {};
  const picks = (v.branches || []).flatMap((b) => (b.turns || []).map((t) => `${b.name}:${t.turn}`));
  if (!picks.includes(G.diffA)) G.diffA = picks[0] || '';
  if (!picks.includes(G.diffB)) G.diffB = picks[picks.length - 1] || '';
  const ask = async (kind, path, words) => {
    G.busy = words; redraw();
    try { G.revisit = { kind, answer: await ctx.api(`${base(ctx)}/${path}`) }; } catch (error) { G.revisit = { kind, wrong: error.message }; }
    G.busy = ''; redraw();
  };
  const pick = (value, set, label) => el('select', { class: 'eg-select', 'aria-label': label, onchange: (e) => { set(e.target.value); } }, picks.map((p) => el('option', { value: p, selected: p === value }, p.replace(':', ' · turn '))));
  const row = el('div', { class: 'eg-revisit-row' },
    el('span', { class: 'eg-label' }, 'Revisiting'),
    el('button', { type: 'button', class: 'eg-link', onclick: () => ask('verify', 'verify', 'Verifying the record…') }, 'Verify'),
    el('button', { type: 'button', class: 'eg-link', onclick: () => ask('replay', 'replay', 'Recomputing every computed value…') }, 'Replay'),
    el('span', { class: 'eg-diff' }, 'Difference between', pick(G.diffA, (x) => { G.diffA = x; }, 'The first turn compared'), 'and', pick(G.diffB, (x) => { G.diffB = x; }, 'The second turn compared'),
      el('button', { type: 'button', class: 'eg-link', onclick: () => ask('diff', `diff?a=${enc(G.diffA)}&b=${enc(G.diffB)}`, 'Comparing…') }, 'Compare')),
    el('button', { type: 'button', class: 'eg-link', title: 'the record as W3C PROV-JSON: entities, activities, agents and their relations', onclick: async () => {
      try { save(`${ctx.name}-engagement-prov.json`, JSON.stringify(await ctx.api(`${base(ctx)}/prov`), null, 1)); ctx.say('The lineage is saved as PROV-JSON.'); } catch (error) { ctx.say(error.message); }
    } }, 'Lineage as PROV-JSON'));
  return [row, G.revisit ? revisitAnswer(ctx, redraw) : null];
}

function revisitAnswer(ctx, redraw) {
  const { el } = ctx;
  const R = G.revisit, v = G.state.view || {};
  const close = el('button', { type: 'button', class: 'eg-close', 'aria-label': 'Close', onclick: () => { G.revisit = null; redraw(); } }, '×');
  if (R.wrong) return el('div', { class: 'eg-answer' }, close, el('p', { class: 'eg-wrong' }, R.wrong));
  const a = R.answer;
  if (R.kind === 'verify') {
    return el('div', { class: 'eg-answer' }, close,
      el('p', { class: `eg-verdict ${a.holds ? 'holds' : 'fails'}` }, `${a.holds ? '✓' : a.undecided ? '?' : '✕'} ${a.claim || 'the record verifies'}${a.holds ? '' : ': not so'}`),
      el('p', { class: 'eg-small' }, a.reason || ''),
      el('p', { class: 'eg-small' }, 'The head of its journal: ', el('code', { class: 'eg-head-digest' }, v.head || '—')));
  }
  if (R.kind === 'replay') {
    const rows = a.replayed || [];
    const failing = rows.filter((x) => !x.verdict.holds).length;
    return el('div', { class: 'eg-answer' }, close,
      el('p', { class: `eg-verdict ${failing ? 'fails' : 'holds'}` }, failing ? `✕ ${failing} of ${rows.length} computed values do not replay` : `✓ every computed value replays (${rows.length})`),
      el('ul', { class: 'eg-list' }, rows.map((x) => el('li', {}, el('span', { class: `eg-mark ${x.verdict.holds ? 'holds' : 'fails'}` }, x.verdict.holds ? '✓' : '✕'), ` ${x.verdict.claim}${x.verdict.reason ? `: ${x.verdict.reason}` : ''}`))));
  }
  if (R.kind === 'diff') {
    const changes = a.changes || [];
    return el('div', { class: 'eg-answer' }, close,
      el('p', { class: 'eg-verdict' }, changes.length ? `${changes.length} value${changes.length === 1 ? '' : 's'} differ between ${G.diffA.replace(':', ' · turn ')} and ${G.diffB.replace(':', ' · turn ')}` : `Nothing differs between ${G.diffA.replace(':', ' · turn ')} and ${G.diffB.replace(':', ' · turn ')}`),
      el('ul', { class: 'eg-list' }, changes.map((c) => el('li', {},
        el('b', {}, `${c.block}.${c.port}`), ` · ${c.before_entry >= 0 ? `entry ${c.before_entry}` : 'nothing'} → ${c.after_entry >= 0 ? `entry ${c.after_entry}` : 'nothing'}`,
        el('details', {}, el('summary', {}, 'before and after'), el('div', { class: 'eg-pair' }, el('pre', {}, JSON.stringify(c.before, null, 1)), el('pre', {}, JSON.stringify(c.after, null, 1))))))));
  }
  return el('div', { class: 'eg-answer' }, close, el('pre', {}, JSON.stringify(a, null, 1)));
}

// ---- the stage: the lattice, and the cell opened beside it ------------------------------------------------------------------------

function stage(ctx, redraw) {
  const { el } = ctx;
  const v = G.state.view;
  const options = { branch: G.branch === '*' ? undefined : G.branch, turn: G.turn, selected: G.sel, ahead: true, ground: null, footer: false };
  const lattice = el('div', { class: 'eg-lattice' });
  lattice.innerHTML = svgTrajectory(v, options);
  lattice.addEventListener('click', (e) => {
    const c = e.target.closest('.kt-cell');
    if (c) {
      const sel = { branch: c.dataset.branch, turn: Number(c.dataset.turn), block: c.dataset.block };
      const same = G.sel && G.sel.branch === sel.branch && G.sel.turn === sel.turn && G.sel.block === sel.block;
      G.sel = same ? null : sel;
      G.turn = sel.turn;
      G.mode = null;
      G.resting = null;
      ctx.here(same ? 'the engagement' : `${sel.block} at turn ${sel.turn}`);
      return redraw();
    }
    const lane = e.target.closest('.kt-lane');
    if (lane) { G.branch = lane.dataset.branch; G.sel = null; redraw(); }
  });
  const out = el('div', { class: 'eg-stage' }, lattice);
  if (G.sel) {
    const L = layoutTrajectory(v, options);
    const at = L.cells.find((c) => c.branch === G.sel.branch && c.turn === G.sel.turn && c.block === G.sel.block);
    if (at) {
      const card = opened(ctx, redraw);
      const reach = el('div', { class: 'eg-reach' });
      out.append(reach, card);
      // The card opens where its cell stands: at its right if there is room, else at its left, else beneath it; the stage grows to hold it.
      requestAnimationFrame(() => place(out, lattice, card, reach, at, L.height));
    }
  }
  return out;
}

function place(stage, lattice, card, reach, at, height) {
  if (!card.isConnected) return;
  const W = stage.clientWidth, scroll = lattice.scrollLeft;
  const w = Math.min(540, Math.max(320, W - 24));
  card.style.width = `${w}px`;
  const right = at.x + at.w - scroll + 18, left = at.x - scroll - 18 - w;
  let x, y = Math.max(0, at.y - 34), side;
  if (right + w <= W) { x = right; side = 'right'; } else if (left >= 0) { x = left; side = 'left'; } else { x = Math.max(0, Math.min(W - w, at.x - scroll)); y = at.y + at.h + 12; side = 'below'; }
  Object.assign(card.style, { left: `${x}px`, top: `${y}px` });
  card.dataset.side = side;
  const mid = at.y + at.h / 2;
  if (side === 'right') Object.assign(reach.style, { display: 'block', left: `${at.x + at.w - scroll + 2}px`, top: `${mid}px`, width: '16px', height: '0' });
  else if (side === 'left') Object.assign(reach.style, { display: 'block', left: `${x + w}px`, top: `${mid}px`, width: `${Math.max(0, at.x - scroll - x - w - 2)}px`, height: '0' });
  else reach.style.display = 'none';
  stage.style.minHeight = `${Math.max(height, y + card.offsetHeight + 16)}px`;
}

// ---- a cell opened: the entry, its values, and the acts there -------------------------------------------------------------------

function opened(ctx, redraw) {
  const { el } = ctx;
  const v = G.state.view;
  const { branch, turn, block } = G.sel;
  const cell = cellOf(v, G.sel) || { block, world: false };
  const { state, marks } = entryOf(cell);
  const S = ENTRY_STATES[state];
  const rec = cell.entry !== undefined && cell.entry !== null ? entryRecord(cell.entry) : null;
  const method = (G.state.methods || []).find((m) => m.block === block);
  const close = el('button', { type: 'button', class: 'eg-close', 'aria-label': 'Close', onclick: () => { G.sel = null; G.mode = null; redraw(); } }, '×');
  const files = G.state.trajectory?.files || {};
  const head = [
    close,
    el('p', { class: 'eg-kicker' }, `${block} · turn ${turn} · ${branch}`),
    el('h3', { class: 'eg-card-title' }, el('span', { class: `eg-glyph s-${state}` }, S.glyph), ` ${S.word}`, cell.by && state !== 'proposal' ? el('span', { class: 'eg-by' }, ` by ${cell.by}`) : null),
  ];
  const facts = [];
  if (cell.world) facts.push(el('p', {}, 'This block is the world itself: it is referred to, and never in the record. What is known of it enters at Collection.'));
  else if (!rec && state === 'empty') facts.push(el('p', { class: 'eg-note' }, 'Nothing is entered here yet.'));
  if (rec) {
    facts.push(el('dl', { class: 'eg-facts' },
      el('dt', {}, 'Entry'), el('dd', {}, `${rec.id}, ${({ method: 'made by a method', observe: 'observed', fill: 'filled', amend: 'amended', review: rec.accepted === false ? 'a review that refused' : 'a review that accepted' })[rec.act] || rec.act}`),
      el('dt', {}, 'By'), el('dd', {}, `${rec.by}${rec.role ? `, as ${rec.role}` : ''}${rec.when ? ` · ${when(rec.when)}` : ''}`),
      el('dt', {}, 'Family'), el('dd', {}, `${FAMILIES[rec.family] || ''} ${rec.family}`),
      rec.reason ? [el('dt', {}, 'Why'), el('dd', {}, rec.reason)] : null,
      rec.target !== undefined && rec.target >= 0 ? [el('dt', {}, 'Of'), el('dd', {}, `entry ${rec.target}`)] : null,
      (rec.files || []).length ? [el('dt', {}, 'Files'), el('dd', {}, rec.files.map((d) => el('a', { class: 'eg-file', href: ctx.keyed(`/api/projects/${enc(ctx.name)}/holdings/${d}`), target: '_blank', title: d }, files[d]?.name || `${d.slice(0, 12)}…`)))] : null));
  }
  if (marks.includes('stale')) facts.push(el('p', { class: 'eg-stale' }, `${ENTRY_STATES.stale.glyph} Stale: ${cell.stale}.`));
  if (marks.includes('inherited')) facts.push(el('p', { class: 'eg-inherited' }, `${ENTRY_STATES.inherited.glyph} Inherited: read from the branch ${rec?.branch || 'its parent'}, which this branch reads before its own turns.`));
  if (cell.held) facts.push(el('p', { class: 'eg-held' }, `${ENTRY_STATES.held.glyph} Held: a review refused what came before, and nothing passes on from here.`));
  for (const r of cell.remarks || []) facts.push(el('p', { class: 'eg-remark' }, r));
  if (method) facts.push(el('p', { class: 'eg-method' }, el('span', { class: 'eg-label' }, 'Its method'), ` ${FAMILIES[method.family] || ''} ${method.method}: ${method.does}.`));

  const values = cell.values && Object.keys(cell.values).length ? el('div', { class: 'eg-values' }, renderValues(ctx, block, cell.values)) : null;
  return el('div', { class: 'eg-card' }, head, facts, values, proposals(ctx, redraw, cell), actsHere(ctx, redraw, cell, rec));
}

// ---- values drawn for what they are ------------------------------------------------------------------------------------------------

function renderValues(ctx, block, values) {
  const { el } = ctx;
  const out = [];
  const asJson = (v) => el('details', { class: 'eg-json' }, el('summary', {}, 'as JSON'), el('pre', {}, JSON.stringify(v, null, 1)));
  if (block === 'Evaluation' && Array.isArray(values.evaluations)) out.push(el('h4', {}, 'Findings'), values.evaluations.map((f) => finding(ctx, f)));
  else if (block === 'Estimation' && values.belief && typeof values.belief === 'object') out.push(el('h4', {}, 'What the evidence allows'), belief(ctx, values.belief));
  else if (block === 'PolicyMaking' && values.policy) out.push(policy(ctx, values.policy));
  else if (block === 'Policy' && values.action) out.push(el('h4', {}, 'The advice'), advice(ctx, values.action));
  else if (block === 'Decision' && (values.intervention || values.inquiry)) out.push(decision(ctx, values));
  else if ((block === 'Interpretation' && values.readings) || (block === 'Constitution' && values.terms)) {
    out.push(el('h4', {}, block === 'Constitution' ? 'Open terms' : 'Readings'), readings(ctx, values.readings || values.terms));
    if (block === 'Constitution') out.push(operators(ctx, values.operators || [], values.standards || []));
  } else if ((block === 'Collection' || block === 'Evidence') && Array.isArray(values.traces)) out.push(el('h4', {}, `Traces · ${values.traces.length}`), traces(ctx, values.traces));
  else if (block === 'Practice' && Array.isArray(values.practice)) out.push(el('h4', {}, 'Practices'), values.practice.map((p) => practice(ctx, p)));
  else out.push(Object.entries(values).map(([port, v]) => [el('h4', {}, port), el('pre', { class: 'eg-pre' }, JSON.stringify(v, null, 1))]));
  out.push(asJson(values));
  return out;
}

const standingMark = (el, s) => { const S = STANDINGS[s] || STANDINGS.undecided; return el('span', { class: `eg-standing-mark st-${s in STANDINGS ? s : 'undecided'}` }, `${S.glyph} ${S.word}`); };
const said = (r) => Object.entries(r || {}).map(([t, x]) => `${t} = ${x}`).join(', ') || 'every reading';

function finding(ctx, f) {
  const { el } = ctx;
  const unit = f.scale === 'absolute' && f.measure && f.measure.high <= 1 && f.measure.low >= 0 ? 'share' : '';
  const standard = (f.by_reading || []).find((r) => r.standard !== null && r.standard !== undefined)?.standard;
  const scale = el('span', { class: 'eg-scale-wrap' });
  if (f.measure) scale.innerHTML = scaleMarkup({ ...f.measure, standard, unit });
  return el('div', { class: `eg-finding st-${f.standing}`, 'data-operator': f.operator },
    el('div', { class: 'eg-finding-head' }, el('span', { class: 'eg-kind' }, KIND_SAID[f.kind] || f.kind), el('b', { class: 'eg-finding-label' }, f.label || f.operator), standingMark(el, f.standing)),
    el('p', { class: 'eg-small' }, `${f.place ? `${f.place} · ` : ''}${f.quantity} ${f.holds_when === '<=' ? 'at most' : 'at least'} ${standard !== undefined ? amount(standard, unit) : 'its standard'}`, f.held_by ? ` · the standard held by ${f.held_by}` : ''),
    f.measure ? el('div', { class: 'eg-measure' }, scale, el('span', { class: 'eg-small' }, `measure ${interval(f.measure, unit)}`)) : null,
    el('ul', { class: 'eg-readings' }, (f.by_reading || []).map((r) => {
      const st = STATUS[r.status] || STATUS.undetermined;
      return el('li', { class: `rs-${r.status}` }, el('span', { class: 'eg-reading' }, said(r.readings)), el('span', { class: 'eg-status' }, `${st.glyph} ${st.word}`), el('span', { class: 'eg-small' }, r.estimate ? interval(r.estimate, unit) : 'no estimate'));
    })),
    (f.turns_on || []).length ? el('p', { class: 'eg-turns' }, 'Turns on ', f.turns_on.map((t, k) => [k ? ', ' : '', el('b', {}, t)])) : el('p', { class: 'eg-small' }, 'It turns on no term: every reading agrees.'),
    f.remark ? el('p', { class: 'eg-remark' }, f.remark) : null);
}

function belief(ctx, b) {
  const { el } = ctx;
  return Object.entries(b).map(([q, e]) => {
    const unit = e.unit || '';
    return el('div', { class: 'eg-quantity' },
      el('div', { class: 'eg-finding-head' }, el('b', {}, q), e.carried ? el('span', { class: 'eg-kind' }, 'carried from the prior') : null),
      e.label ? el('p', { class: 'eg-small' }, `${e.label}${unit ? ` · ${unit}` : ''}${e.scale ? ` · ${e.scale} scale` : ''}`) : null,
      Object.entries(e.by_readings || {}).map(([sig, x]) => {
        const s = el('span', { class: 'eg-scale-wrap' });
        s.innerHTML = scaleMarkup({ ...x, unit });
        return el('div', { class: `eg-estimate${x.conflict ? ' conflict' : ''}` },
          el('span', { class: 'eg-reading' }, sig.replace(/=/g, ' = ').replace(/;\s*/g, ', ') || 'every reading'), s, el('span', { class: 'eg-small' }, interval(x, unit)),
          x.conflict ? el('p', { class: 'eg-conflict' }, '◐ The sources conflict: what is shown is the hull of their intervals.') : null,
          el('p', { class: 'eg-small' }, `from ${(x.items || []).length} item${(x.items || []).length === 1 ? '' : 's'} (${[...new Set(x.sources || [])].join(', ')})`));
      }));
  });
}

function policy(ctx, p) {
  const { el } = ctx;
  return [
    el('h4', {}, `Needs taken up · version ${p.version ?? '—'}`),
    (p.taken_up || []).length ? el('ul', { class: 'eg-list' }, p.taken_up.map((t) => el('li', {}, el('span', { class: 'eg-kind' }, t.kind), ' ', t.label, ' ', standingMark(el, t.standing)))) : el('p', { class: 'eg-note' }, 'No need, problem or risk is clear or arguable: nothing is taken up.'),
    el('h4', {}, 'Practices that answer them'),
    (p.candidates || []).length ? p.candidates.map((c) => practice(ctx, c)) : el('p', { class: 'eg-note' }, 'No practice declared answers what is taken up.'),
    (p.proposals || []).length ? [el('h4', {}, 'Proposals'), el('pre', { class: 'eg-pre' }, JSON.stringify(p.proposals, null, 1))] : null,
    el('h4', {}, 'What is open'), openList(ctx, p.open),
    p.previously_chosen ? el('p', { class: 'eg-small' }, `Chosen before: ${p.previously_chosen}`) : null,
  ];
}

function practice(ctx, p) {
  const { el } = ctx;
  return el('div', { class: 'eg-practice' },
    el('b', {}, p.title || p.id), p.source ? el('p', { class: 'eg-small' }, p.source) : null,
    (p.support_factors || []).length ? el('p', { class: 'eg-factors' }, 'Support factors: ', p.support_factors.map((s) => el('span', { class: 'eg-factor' }, s))) : null,
    p.transfer ? el('button', { type: 'button', class: 'eg-link', onclick: () => { G.pick = p.transfer; G.assessed = null; ctx.show('transfer'); } }, `Assess its transfer: ${p.transfer}`) : null);
}

function openList(ctx, open) {
  const { el } = ctx;
  if (!(open || []).length) return el('p', { class: 'eg-note' }, 'Nothing is open.');
  return el('ul', { class: 'eg-list' }, open.map((o) => el('li', {}, o.term ? ['the term ', el('b', {}, o.term), o.finding ? `, on which ${o.finding} turns` : ''] : JSON.stringify(o))));
}

function advice(ctx, a) {
  const { el } = ctx;
  return [el('p', { class: 'eg-advice' }, a.advice ? a.advice : 'None chosen yet: a lead chooses the advice in the policy version (fill PolicyMaking with "chosen").', el('span', { class: 'eg-small' }, ` · ${a.status || ''}`)),
    el('h4', {}, 'What is open'), openList(ctx, a.open)];
}

function decision(ctx, d) {
  const { el } = ctx;
  return [
    el('h4', {}, 'The intervention'), el('p', { class: 'eg-advice' }, d.intervention?.advice || 'None: no advice is chosen.'),
    el('h4', {}, 'The inquiry, carried to the next turn'),
    (d.inquiry?.ask || []).length ? el('ul', { class: 'eg-list' }, d.inquiry.ask.map((o) => el('li', {}, o.term ? ['find out how ', el('b', {}, o.term), ' is to be read', o.finding ? ` for ${o.finding}` : ''] : JSON.stringify(o)))) : el('p', { class: 'eg-note' }, 'Nothing is asked.'),
  ];
}

function readings(ctx, terms) {
  const { el } = ctx;
  return el('dl', { class: 'eg-terms' }, Object.entries(terms || {}).map(([t, rs]) => [el('dt', {}, t), el('dd', {}, (rs || []).map((r) => el('div', {}, el('b', {}, r.reading), r.source ? el('span', { class: 'eg-small' }, ` · ${r.source}`) : null)))]));
}

function operators(ctx, ops, standards) {
  const { el } = ctx;
  return [el('h4', {}, 'Operators'), el('ul', { class: 'eg-list' }, ops.map((o) => el('li', {}, el('span', { class: 'eg-kind' }, o.kind), ' ', o.label || o.id,
    el('span', { class: 'eg-small' }, ` · ${o.quantity}${o.place ? ` at ${o.place}` : ''} ${o.holds_when === '<=' ? 'at most' : 'at least'} ${typeof o.standard === 'number' ? fig(o.standard) : o.standard}${(o.terms || []).length ? ` · terms: ${o.terms.join(', ')}` : ''}`)))),
    standards.length ? [el('h4', {}, 'Standards'), el('ul', { class: 'eg-list' }, standards.map((s) => el('li', {}, el('b', {}, s.id), ` ${s.value ?? ''}`, s.held_by ? el('span', { class: 'eg-small' }, ` · held by ${s.held_by}`) : null)))] : null];
}

function traces(ctx, items) {
  const { el } = ctx;
  const shown = items.slice(0, 12);
  return [el('ul', { class: 'eg-traces' }, shown.map((t) => el('li', {},
    el('b', {}, t.label || t.quantity), el('span', { class: 'eg-num' }, interval(t, t.unit)),
    el('span', { class: 'eg-small' }, `${t.place?.name || t.unit_of || ''}${t.readings ? ` · ${said(t.readings)}` : ''}${t.source ? ` · ${t.source}` : ''}`)))),
  items.length > shown.length ? el('p', { class: 'eg-small' }, `and ${items.length - shown.length} more`) : null];
}

// ---- proposals awaiting a review ------------------------------------------------------------------------------------------------

function proposals(ctx, redraw, cell) {
  const { el } = ctx;
  const list = cell.proposals || [];
  if (!list.length) return null;
  return el('div', { class: 'eg-proposals' }, el('h4', {}, `Proposals awaiting a review · ${list.length}`),
    list.map((p) => el('div', { class: 'eg-proposal' },
      el('span', {}, `${ENTRY_STATES.proposal.glyph} entry ${p.entry}, by ${p.by}${p.reason ? `: ${p.reason}` : ''}`),
      el('span', { class: 'eg-pair-buttons' },
        el('button', { type: 'button', class: 'eg-quiet', disabled: !!G.busy, onclick: () => review(ctx, redraw, p.entry, true) }, '✓ Accept'),
        el('button', { type: 'button', class: 'eg-quiet', disabled: !!G.busy, onclick: () => review(ctx, redraw, p.entry, false) }, '✕ Refuse')))));
}

function review(ctx, redraw, entry, accept) {
  const reason = G.reason.trim() || (accept ? 'accepted on review' : 'refused on review');
  act(ctx, redraw, accept ? 'Accepting…' : 'Refusing…', `${base(ctx)}/act`, { act: 'review', entry, accept, reason, role: role(), branch: G.sel?.branch || 'main' });
}

// ---- the acts at a block ---------------------------------------------------------------------------------------------------------

function actsHere(ctx, redraw, cell, rec) {
  const { el } = ctx;
  if (cell.world) return null;
  const { branch, turn, block } = G.sel;
  const rules = G.state.trajectory?.rules || [];
  const who = role();
  const holdsValue = rec && !cell.held;
  const may = (a) => empowered(rules, who, block, a);
  const note = (a) => (may(a) ? '' : ` (as a proposal: the rules empower ${empoweredRoles(rules, block, a).join(', ') || 'no one'})`);
  const buttons = el('div', { class: 'eg-card-acts' },
    el('button', { type: 'button', class: `eg-quiet${G.mode === 'fill' ? ' on' : ''}`, onclick: () => { G.mode = G.mode === 'fill' ? null : 'fill'; redraw(); } }, `Fill${note('fill')}`),
    (cell.boundary || block === 'Collection') && may('observe') ? el('button', { type: 'button', class: `eg-quiet${G.mode === 'observe' ? ' on' : ''}`, title: 'enter what was observed from outside the loop', onclick: () => { G.mode = G.mode === 'observe' ? null : 'observe'; redraw(); } }, 'Observe') : null,
    holdsValue ? el('button', { type: 'button', class: `eg-quiet${G.mode === 'amend' ? ' on' : ''}`, onclick: () => { G.mode = G.mode === 'amend' ? null : 'amend'; redraw(); } }, `Amend${note('amend')}`) : null,
    rec ? el('button', { type: 'button', class: 'eg-quiet', disabled: !!G.busy, title: `review entry ${rec.id}: let it pass`, onclick: () => review(ctx, redraw, rec.id, true) }, '✓ Accept') : null,
    rec ? el('button', { type: 'button', class: 'eg-quiet', disabled: !!G.busy, title: `review entry ${rec.id}: refuse it, so that nothing passes on`, onclick: () => review(ctx, redraw, rec.id, false) }, '✕ Refuse') : null,
    rec ? el('button', { type: 'button', class: 'eg-link', onclick: async () => {
      try { G.resting = { entry: rec.id, answer: await ctx.api(`${base(ctx)}/resting?entry=${rec.id}`) }; } catch (error) { G.resting = { entry: rec.id, wrong: error.message }; }
      redraw();
    } }, 'What rests on this') : null);
  const reason = el('input', { type: 'text', class: 'eg-input wide', placeholder: 'the reason for your act', value: G.reason, 'aria-label': 'The reason for your act', oninput: (e) => { G.reason = e.target.value; } });
  const out = [el('h4', {}, `Acts here, as ${who}`), reason, buttons];
  if (G.resting && rec && G.resting.entry === rec.id) {
    const R = G.resting;
    out.push(el('div', { class: 'eg-resting' }, R.wrong ? el('p', { class: 'eg-wrong' }, R.wrong)
      : [el('p', { class: 'eg-small' }, `${(R.answer.resting || []).length} entr${(R.answer.resting || []).length === 1 ? 'y rests' : 'ies rest'} on entry ${rec.id}, itself among them:`),
        el('ul', { class: 'eg-list' }, (R.answer.resting || []).map((x) => el('li', {}, el('button', { type: 'button', class: 'eg-link', onclick: () => { G.sel = { branch: x.branch, turn: x.turn, block: x.block }; G.resting = null; redraw(); } }, `${x.block}, turn ${x.turn}, ${x.branch}`), ` · entry ${x.entry}, ${x.act} by ${x.by}`)))]));
  }
  if (G.mode === 'fill' || G.mode === 'observe' || G.mode === 'amend') out.push(editor(ctx, redraw, cell, rec));
  out.push(dropZone(ctx, redraw));
  return el('div', { class: 'eg-here' }, out);
}

function editor(ctx, redraw, cell, rec) {
  const { el } = ctx;
  const { branch, turn, block } = G.sel;
  const blk = architecture()?.blocks?.find((b) => b.name === block);
  const ports = blk?.outputs || Object.keys(cell.values || {}).map((port) => ({ port, type: '' }));
  const key = `${G.mode}|${branch}|${turn}|${block}`;
  const here = cell.values && Object.keys(cell.values).length ? cell.values : null;
  const before = here ? null : earlier(G.state.view, branch, turn, block);
  const from = here || before?.values || {};
  if (!G.drafts[key]) G.drafts[key] = ports.map((p) => JSON.stringify(from[p.port] ?? EMPTY[p.type] ?? {}, null, 1));
  const wrong = el('p', { class: 'eg-wrong' });
  const areas = ports.map((p, k) => el('label', { class: 'eg-port' }, el('span', {}, el('b', {}, p.port), p.type ? ` : ${p.type}` : ''),
    el('textarea', { class: 'eg-code', rows: Math.min(14, Math.max(3, G.drafts[key][k].split('\n').length)), spellcheck: 'false', 'aria-label': `The value of ${p.port}`, oninput: (e) => { G.drafts[key][k] = e.target.value; } }, G.drafts[key][k])));
  const submit = () => {
    const outputs = [];
    for (let k = 0; k < ports.length; k++) {
      try { outputs.push(JSON.parse(G.drafts[key][k])); } catch (error) { wrong.textContent = `${ports[k].port} is not JSON: ${error.message}`; return; }
    }
    const reason = G.reason.trim();
    if (!reason) { wrong.textContent = 'Say why: every act in the record carries its reason.'; return; }
    const body = G.mode === 'amend'
      ? { act: 'amend', entry: rec.id, branch, role: role(), outputs, reason, files: G.files.map((f) => f.digest) }
      : { act: G.mode, block, turn, branch, role: role(), outputs, reason, files: G.files.map((f) => f.digest) };
    const words = { amend: 'Amending…', observe: 'Observing…', fill: 'Filling…' }[G.mode];
    delete G.drafts[key];
    G.mode = null;
    G.files = [];
    act(ctx, redraw, words, `${base(ctx)}/act`, body);
  };
  return el('div', { class: 'eg-editor' },
    el('p', { class: 'eg-small' }, G.mode === 'amend' ? `An amendment of entry ${rec.id}: the computed value is kept beside yours, and what follows is marked as resting on your amendment.`
      : `A value for each output of ${block}, as JSON${G.mode === 'observe' ? ', as observed from outside the loop' : ''}. It is held to the block’s types; what the rules do not empower you to enter stands as a proposal.${before ? ` Prefilled from turn ${before.turn}${before.branch !== branch ? ` of ${before.branch}` : ''}: change what differs.` : ''}`),
    areas,
    G.files.length ? el('p', { class: 'eg-small' }, 'Files named with it: ', G.files.map((f) => el('span', { class: 'eg-factor' }, f.name))) : null,
    wrong,
    el('div', { class: 'eg-card-acts' }, el('button', { type: 'button', class: 'eg-go', disabled: !!G.busy, onclick: submit }, { amend: 'Amend', observe: 'Enter as observed', fill: 'Enter' }[G.mode]),
      el('button', { type: 'button', class: 'eg-quiet', onclick: () => { delete G.drafts[key]; G.mode = null; redraw(); } }, 'Leave it')));
}

// A file dropped here is kept in the project's archive by its digest and recorded in the trajectory; the next fill or amendment names it.
function dropZone(ctx, redraw) {
  const { el } = ctx;
  const input = el('input', { type: 'file', class: 'eg-file-input', multiple: true, onchange: (e) => take([...e.target.files]) });
  const zone = el('div', { class: 'eg-drop', tabindex: 0, role: 'button', 'aria-label': 'Attach a file',
    onclick: () => input.click(), onkeydown: (e) => { if (e.key === 'Enter' || e.key === ' ') input.click(); } },
    el('span', {}, 'Attach a file: drop it here, or choose one'), el('span', { class: 'eg-small' }, ' · kept by its SHA-256 and named by your next fill or amendment here'), input);
  zone.addEventListener('dragover', (e) => { e.preventDefault(); e.stopPropagation(); zone.classList.add('over'); });
  zone.addEventListener('dragleave', () => zone.classList.remove('over'));
  zone.addEventListener('drop', (e) => { e.preventDefault(); e.stopPropagation(); zone.classList.remove('over'); take([...e.dataTransfer.files]); });
  async function take(list) {
    for (const f of list) {
      if (G.busy) return;
      G.busy = `Keeping ${f.name}…`;
      redraw();
      try {
        const r = await send(ctx, `${base(ctx)}/files?name=${enc(f.name)}&kind=${enc(f.type || '')}&base=${enc(G.sha || '')}`, null, await f.arrayBuffer());
        if (r?.file) {
          G.files.push({ name: r.file.name, digest: r.file.digest });
          G.said = `${r.file.name} is kept, ${r.file.digest.slice(0, 12)}…: your next fill or amendment here names it.`;
          G.saidKind = 'made';
          await read(ctx, true);
        }
      } catch (error) { G.said = error.message; G.saidKind = 'refused'; }
      G.busy = '';
      redraw();
    }
  }
  return zone;
}

