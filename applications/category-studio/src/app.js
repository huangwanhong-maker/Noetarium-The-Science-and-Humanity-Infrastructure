// The Category Studio's page: a script at the left, and at the right what each of its statements is answered with.
//
// The page computes nothing of category theory. It sends the script to the server, which gives it to the engine
// (kategoria's script language), and draws the answers with the visualization layer's categories. What it keeps is the
// script: in this browser as it is typed, and on this computer under a name when asked.

import { GLYPH, htmlCategory, htmlComposition, render } from '/visualization/js/categories/index.js';
import { GUIDE } from './guide.js';

const $ = (id) => document.getElementById(id);
const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;').replace(/"/g, '&quot;');
const WORD = { holds: 'holds', fails: 'fails', undecided: 'not decided', error: 'cannot be carried out' };
const KIND = { category: 'a category', functor: 'a functor', transformation: 'a natural transformation', sets: 'a set-valued functor', theory: 'a theory', model: 'a model', diagram: 'a diagram',
               check: 'a claim', find: 'a question', error: 'a statement' };
const STORE = 'category-studio';

const S = { examples: [], kept: [], outcomes: [], status: null, asked: 0, shown: 0, chosen: null, timer: null, busy: false, last: '' };

const api = async (path, options = {}) => {
  const r = await fetch(path, { ...options, headers: { 'Content-Type': 'application/json', ...(options.headers || {}) } });
  const body = await r.json().catch(() => ({}));
  if (!r.ok) throw new Error(body.message || `${r.status} ${r.statusText}`);
  return body;
};

const say = (words, wrong = false) => { $('state').textContent = words; $('state').classList.toggle('wrong', wrong); };

// ---- the desk ---------------------------------------------------------------------------------------------------------------

const script = () => $('script').value;

function gutter() {
  const lines = script().split('\n').length;
  const marks = new Map(S.shown === S.asked ? S.outcomes.map((o) => [o.line, o]) : []);
  const chosen = S.chosen ? [S.chosen.line, S.chosen.line + S.chosen.text.split('\n').length - 1] : null;
  let html = '';
  for (let l = 1; l <= lines; l++) {
    const o = marks.get(l);
    html += `<div data-line="${l}" class="${o ? `told ${o.status}` : ''}${chosen && l >= chosen[0] && l <= chosen[1] ? ' chosen' : ''}"${o ? ` data-glyph="${GLYPH[o.status] || ''}" title="${esc(WORD[o.status] || '')}"` : ''}>${l}</div>`;
  }
  $('gutter').innerHTML = html;
  $('gutter').scrollTop = $('script').scrollTop;
}

function remember() {
  try { localStorage.setItem(STORE, JSON.stringify({ name: $('name').value, text: script() })); } catch { /* a browser that keeps nothing */ }
}

function setScript(text, name = '') {
  $('script').value = text;
  $('name').value = name;
  $('script').scrollTop = 0;
  S.chosen = null;
  S.outcomes = [];
  gutter();
  remember();
  return run();
}

// ---- the sheet --------------------------------------------------------------------------------------------------------------

// The script's remarks: the comments that stand on lines of their own outside every block, each run of them one remark.
function remarks(text) {
  const out = [];
  let depth = 0, run = null;
  text.split('\n').forEach((line, k) => {
    const t = line.trim();
    if (depth === 0 && t.startsWith('#')) {
      const words = t.replace(/^#+\s?/, '');
      if (run && run.until === k) { run.words.push(words); run.until = k + 1; } else { run = { line: k + 1, until: k + 1, words: [words] }; out.push(run); }
      return;
    }
    const bare = line.replace(/"[^"]*"/g, '').replace(/#.*/, '');
    depth = Math.max(0, depth + (bare.match(/{/g) || []).length - (bare.match(/}/g) || []).length);
  });
  return out;
}

function card(o, k) {
  const evidence = (o.evidence || []).map((e) => `<li><b>${esc(e.role)}</b><span>${esc(e.value)}</span></li>`).join('');
  const view = o.view || null;
  const category = view && view.type === 'category' && view.finite;
  const more = category ? `<div class="more"><button class="key" data-more="all" type="button">every arrow</button>${view.table ? '<button class="key" data-more="table" type="button">table of composition</button>' : ''}</div>` : '';
  const said = o.text.split('\n').slice(0, 2).join('\n') + (o.text.split('\n').length > 2 ? ' …' : '');
  const head = o.status === 'error' ? esc(o.text.split('\n')[0]) : esc(o.claim || o.name || '');
  return `<article class="outcome ${esc(o.status)}" data-k="${k}" data-line="${o.line}">` +
    `<header><span class="glyph">${GLYPH[o.status] || ''}</span><h4>${head}</h4><span class="word">${esc(WORD[o.status] || o.status)}</span><span class="where" title="${esc(said)}">${esc(KIND[o.kind] || o.kind)} · line ${o.line}</span></header>` +
    (o.reason ? `<p class="reason">${esc(o.reason)}</p>` : '') + (evidence ? `<ul class="evidence">${evidence}</ul>` : '') +
    (view ? `<div class="view">${render(view)}</div>` : '') + more + '<div class="extra"></div></article>';
}

function sheet() {
  const items = [...remarks(S.last).map((r) => ({ line: r.line, remark: r })), ...S.outcomes.map((o, k) => ({ line: o.line, outcome: o, k }))].sort((a, b) => a.line - b.line);
  if (!items.length) {
    $('sheet').innerHTML = '<p class="empty">Write a statement at the left, or take an example above.<br>Each statement is answered here, with its evidence and its drawing.</p>';
    return;
  }
  let first = true;
  $('sheet').innerHTML = items.map((it) => {
    if (it.outcome) return card(it.outcome, it.k);
    const title = first && it.line === 1;
    first = false;
    return (title ? `<p class="remark title">${esc(it.remark.words[0])}</p>${it.remark.words.length > 1 ? `<p class="remark">${esc(it.remark.words.slice(1).join(' '))}</p>` : ''}` : `<p class="remark">${esc(it.remark.words.join(' '))}</p>`);
  }).join('');
}

function tally() {
  const count = { holds: 0, fails: 0, undecided: 0, error: 0 };
  S.outcomes.forEach((o) => { count[o.status] = (count[o.status] || 0) + 1; });
  const part = (key, one, many) => (count[key] ? `<span class="${key}"><b>${GLYPH[key]} ${count[key]}</b> ${count[key] === 1 ? one : many}</span>` : '');
  $('tally').innerHTML = S.outcomes.length ? part('holds', 'holds', 'hold') + part('fails', 'fails', 'fail') + part('undecided', 'not decided', 'not decided') + part('error', 'cannot be carried out', 'cannot be carried out') +
    `<i>${S.outcomes.length} ${S.outcomes.length === 1 ? 'statement' : 'statements'}${S.took !== undefined ? `, ${S.took < 1000 ? `${Math.round(S.took)} ms` : `${(S.took / 1000).toFixed(1)} s`}` : ''}</i>` : '';
}

function choose(k, from) {
  S.chosen = k === null ? null : S.outcomes[k];
  document.querySelectorAll('.outcome.chosen').forEach((el) => el.classList.remove('chosen'));
  gutter();
  if (!S.chosen) return;
  const el = document.querySelector(`.outcome[data-k="${k}"]`);
  if (el) el.classList.add('chosen');
  if (from === 'gutter' && el) el.scrollIntoView({ block: 'nearest', behavior: 'smooth' });
  if (from === 'sheet') {
    const top = (S.chosen.line - 1) * 20, box = $('script');
    if (top < box.scrollTop || top > box.scrollTop + box.clientHeight - 60) box.scrollTop = Math.max(0, top - 60);
  }
}

// ---- running ----------------------------------------------------------------------------------------------------------------

async function run() {
  const text = script();
  const asked = ++S.asked;
  if (!S.status || !S.status.built) { say('the engine is not built', true); return; }
  S.busy = true;
  say('carrying out …');
  gutter();
  const t0 = performance.now();
  try {
    const answer = await api('/api/run', { method: 'POST', body: JSON.stringify({ script: text }) });
    if (asked !== S.asked) return;
    S.outcomes = answer.outcomes;
    S.took = performance.now() - t0;
    say('');
  } catch (error) {
    if (asked !== S.asked) return;
    S.outcomes = [];
    S.took = undefined;
    say(error.message, true);
  }
  S.shown = asked;
  S.last = text;
  S.busy = false;
  S.chosen = null;
  sheet();
  tally();
  gutter();
}

function later() {
  remember();
  gutter();
  clearTimeout(S.timer);
  if ($('live').checked) S.timer = setTimeout(run, 700);
}

// ---- examples and scripts kept ----------------------------------------------------------------------------------------------

function fill() {
  $('examples').innerHTML = '<option value="">…</option>' + S.examples.map((e) => `<option value="${esc(e.id)}">${esc(e.title)}</option>`).join('');
  $('kept').innerHTML = '<option value="">…</option>' + S.kept.map((e) => `<option value="${esc(e.name)}">${esc(e.name)}</option>`).join('');
  $('kept-pick').hidden = !S.kept.length;
}

async function loadKept() {
  try { S.kept = await api('/api/scripts'); } catch { S.kept = []; }
  fill();
}

async function keep() {
  const name = $('name').value.trim();
  if (!name) { say('give the script a name to keep it', true); $('name').focus(); return; }
  try {
    await api(`/api/scripts/${encodeURIComponent(name)}`, { method: 'PUT', body: JSON.stringify({ text: script() }) });
    say(`kept as ${name}`);
    await loadKept();
  } catch (error) { say(error.message, true); }
}

// ---- the engine ---------------------------------------------------------------------------------------------------------------

async function setup() {
  const dialog = $('setup');
  let view;
  try { view = await api('/setup'); } catch (error) { say(error.message, true); return; }
  $('setup-about').textContent = 'The studio computes with an engine in C++: the script language of kategoria, on morphe, compiled once with g++ 11 or later, on this computer or in WSL. It takes about two minutes, and needs no data.';
  $('setup-state').textContent = view.writable === false ? view.refusal : '';
  $('setup-run').disabled = view.writable === false;
  if (!dialog.open) dialog.showModal();
  $('setup-run').onclick = async () => {
    $('setup-run').disabled = true;
    try {
      await api('/setup/run', { method: 'POST', body: '{}' });
      for (;;) {
        await new Promise((done) => setTimeout(done, 1500));
        const now = await api('/setup');
        const step = (now.steps || [])[0] || {};
        $('setup-state').textContent = now.running ? 'Compiling; a minute or two …' : step.done ? 'Built.' : String(now.log || '').trim().split(/\n/).slice(-1)[0] || 'Not built.';
        if (!now.running) break;
      }
      S.status = await api('/api/status');
      if (S.status.built) { dialog.close(); run(); }
    } catch (error) { $('setup-state').textContent = error.message; }
    $('setup-run').disabled = false;
  };
}

// ---- beginning ----------------------------------------------------------------------------------------------------------------

function theme(night) {
  document.documentElement.dataset.theme = night ? 'night' : 'day';
  $('theme').textContent = night ? 'Day' : 'Night';
  try { localStorage.setItem(`${STORE}:theme`, night ? 'night' : 'day'); } catch { /* nothing kept */ }
}

async function begin() {
  const query = new URLSearchParams(location.search);
  let stored = {};
  try { stored = JSON.parse(localStorage.getItem(STORE) || '{}'); } catch { stored = {}; }
  let night = false;
  try { night = (query.get('theme') || localStorage.getItem(`${STORE}:theme`)) === 'night'; } catch { night = query.get('theme') === 'night'; }
  theme(night);
  $('guide-body').innerHTML = GUIDE;

  $('script').addEventListener('input', later);
  $('script').addEventListener('scroll', () => { $('gutter').scrollTop = $('script').scrollTop; });
  $('script').addEventListener('keydown', (event) => {
    if (event.key === 'Enter' && (event.ctrlKey || event.metaKey)) { event.preventDefault(); clearTimeout(S.timer); run(); }
    if (event.key === 'Tab') { event.preventDefault(); document.execCommand('insertText', false, '  '); }
  });
  $('name').addEventListener('input', remember);
  $('run').addEventListener('click', () => { clearTimeout(S.timer); run(); });
  $('keep').addEventListener('click', keep);
  $('theme').addEventListener('click', () => theme(document.documentElement.dataset.theme !== 'night'));
  $('guide-open').addEventListener('click', () => $('guide').showModal());
  $('guide-close').addEventListener('click', () => $('guide').close());
  $('setup-close').addEventListener('click', () => $('setup').close());
  $('examples').addEventListener('change', () => {
    const e = S.examples.find((x) => x.id === $('examples').value);
    if (e) setScript(e.text, '');
    $('examples').value = '';
  });
  $('kept').addEventListener('change', async () => {
    const name = $('kept').value;
    $('kept').value = '';
    if (!name) return;
    try { const kept = await api(`/api/scripts/${encodeURIComponent(name)}`); setScript(kept.text, kept.name); } catch (error) { say(error.message, true); }
  });
  $('gutter').addEventListener('click', (event) => {
    const line = Number(event.target.dataset.line);
    const k = S.outcomes.findIndex((o) => o.line === line);
    if (k >= 0) choose(k, 'gutter');
  });
  $('sheet').addEventListener('click', (event) => {
    const el = event.target.closest('.outcome');
    if (!el) return;
    const k = Number(el.dataset.k), o = S.outcomes[k];
    const key = event.target.closest('[data-more]');
    if (key) {
      const on = !key.classList.contains('on');
      key.classList.toggle('on', on);
      if (key.dataset.more === 'all') el.querySelector('.view').innerHTML = htmlCategory(o.view, { all: on });
      if (key.dataset.more === 'table') el.querySelector('.extra').innerHTML = on ? htmlComposition(o.view) : '';
      return;
    }
    if (event.target.closest('header')) choose(S.chosen === o ? null : k, 'sheet');
  });

  try { S.status = await api('/api/status'); } catch (error) { say(error.message, true); }
  try { S.examples = await api('/api/examples'); } catch { S.examples = []; }
  await loadKept();
  $('keep').hidden = !(S.status && S.status.keeps);
  const wanted = query.get('example');
  const example = wanted ? S.examples.find((e) => e.id === wanted || e.id.replace(/^\d+-/, '') === wanted) : null;
  if (example) $('script').value = example.text;
  else if (query.get('script') === 'none') $('script').value = '';
  else if (stored.text) { $('script').value = stored.text; $('name').value = stored.name || ''; }
  else if (S.examples.length) $('script').value = S.examples[0].text;
  gutter();
  sheet();
  if (S.status && !S.status.built) setup();
  else if (S.status && !S.status.current) say('the engine was built from older sources; it still answers');
  await run();
}

window.studio = { S, run, setScript, choose };
begin();
