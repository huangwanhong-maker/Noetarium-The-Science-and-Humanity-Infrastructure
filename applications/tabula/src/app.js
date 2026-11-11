// Tabula: the page about the board. It loads the plug-ins (each a subject: its kinds of note, its
// templates, its cards), makes one board with all of them, and keeps what is made: a board is saved a
// moment after every change, opened again by its address (#id), given away as a file or as a picture.
//
//   ?template=<plug-in>/<template>   begin a new board with a template laid on it
//   ?theme=night                     by night
//   ?dry                             keep nothing (for looking, and for the checks)

import * as B from '/visualization/js/board/index.js';
import { store } from './store.js';

const $ = (id) => document.getElementById(id);
const el = (tag, attrs = {}, ...kids) => {
  const node = document.createElement(tag);
  for (const [k, v] of Object.entries(attrs)) {
    if (v === null || v === undefined || v === false) continue;
    if (k.startsWith('on')) node.addEventListener(k.slice(2), v);
    else node.setAttribute(k, v === true ? '' : v);
  }
  for (const kid of kids.flat(Infinity)) if (kid !== null && kid !== undefined && kid !== false) node.append(kid instanceof Node ? kid : document.createTextNode(String(kid)));
  return node;
};
const params = new URLSearchParams(location.search);
const dry = params.has('dry');
const stamp = () => new Date().toISOString().replace(/\.\d+Z$/, 'Z');

// ---- a quiet line for what the page has to say ------------------------------------------------------
let saying = null;
function say(words) {
  const line = $('say');
  line.textContent = words;
  line.classList.add('shown');
  clearTimeout(saying);
  saying = setTimeout(() => line.classList.remove('shown'), 3200);
}

// ---- the theme ------------------------------------------------------------------------------------------
function setTheme(theme) {
  document.documentElement.dataset.theme = theme;
  $('theme').textContent = theme === 'night' ? 'Day' : 'Night';
  try { localStorage.setItem('tabula.theme', theme); } catch { /* a page without storage keeps its theme for the visit */ }
}
let remembered = null;
try { remembered = localStorage.getItem('tabula.theme'); } catch { /* none */ }
setTheme(params.get('theme') || remembered || 'day');
$('theme').addEventListener('click', () => setTheme(document.documentElement.dataset.theme === 'night' ? 'day' : 'night'));

// ---- the plug-ins ---------------------------------------------------------------------------------------
async function loadPlugins() {
  let list;
  try { list = await store.plugins(); } catch { list = ['market-analysis', 'research', 'agents', 'history', 'gis-3d'].map((id) => ({ id, module: `/plugins/${id}/plugin.js` })); }
  const out = [];
  for (const p of list) {
    try {
      out.push((await import(p.module)).default(B));
      // A plug-in may bring a style sheet for its cards.
      if (p.style) document.head.append(el('link', { rel: 'stylesheet', href: p.style }));
    } catch (error) {
      say(`The plug-in '${p.id}' could not be loaded: ${error.message}`);
    }
  }
  return out;
}

const plugins = await loadPlugins();
const packs = [B.GENERAL, ...plugins];
// The modules the plug-ins bring (plugins/<name>/modules/*.py), each described by the server: its parameters.
let modules = [];
try { modules = await store.modules(); } catch { /* a page without its server runs no modules */ }

const api = B.createBoard($('board'), {
  board: B.emptyBoard({ now: stamp() }),
  noteTypes: plugins.flatMap((p) => p.noteTypes || []),
  cards: Object.assign({}, ...plugins.map((p) => p.cards || {})),
  storeImage: dry ? null : store.image,
  services: { data: store.data, run: store.run, keep: store.keep, modules: modules.filter((m) => !m.error) },
  today: Math.floor(Date.now() / 86400000),
  // The rail and the panel stand over the board's edges: what is fitted to the view keeps clear of them.
  insets: () => {
    const stage = $('board').getBoundingClientRect(), rail = $('rail').getBoundingClientRect(), panel = $('panel').getBoundingClientRect();
    return { left: rail.width ? rail.right - stage.left : 0, right: panel.width ? stage.right - panel.left : 0 };
  },
});
B.createToolbar(api, $('rail'));
B.createInspector(api, $('panel'));
api.on('say', say);

// ---- keeping the board ------------------------------------------------------------------------------------
const state = $('state');
let saveTimer = null, kept = null;      // the board as last kept
const worthKeeping = (board) => board.order.length > 0 || kept !== null;

async function save() {
  clearTimeout(saveTimer);
  const board = api.board;
  if (dry || board === kept || !worthKeeping(board)) return;
  state.textContent = 'Keeping…';
  state.classList.remove('wrong');
  try {
    await store.save({ ...board, meta: { ...board.meta, modified: stamp() } });
    kept = board;
    if (location.hash.slice(1) !== board.id) history.replaceState(null, '', `#${board.id}`);
    try { localStorage.setItem('tabula.last', board.id); } catch { /* none */ }
    state.textContent = api.board === board ? 'Kept' : 'Keeping…';
  } catch (error) {
    state.textContent = `Not kept: ${error.message}`;
    state.classList.add('wrong');
  }
}

function reflect() {
  const board = api.board;
  if (document.activeElement !== $('title')) $('title').value = board.title === 'Untitled board' ? '' : board.title;
  document.title = board.title && board.title !== 'Untitled board' ? `${board.title} · Tabula` : 'Tabula';
  $('undo').disabled = !api.canUndo;
  $('redo').disabled = !api.canRedo;
}

api.on('change', () => {
  reflect();
  if (dry) { state.textContent = 'Not kept (a dry page)'; return; }
  if (api.board !== kept && worthKeeping(api.board)) {
    state.textContent = 'Changed';
    clearTimeout(saveTimer);
    saveTimer = setTimeout(save, 900);
  }
});
api.on('view', (view) => { $('zoom-level').textContent = `${Math.round(view.k * 100)}%`; });

function show(board, { isKept = false } = {}) {
  clearTimeout(saveTimer);
  api.load(board);
  kept = isKept ? api.board : null;
  state.textContent = isKept ? 'Kept' : '';
  state.classList.remove('wrong');
  reflect();
  if (isKept) { history.replaceState(null, '', `#${board.id}`); try { localStorage.setItem('tabula.last', board.id); } catch { /* none */ } }
  else history.replaceState(null, '', location.pathname + location.search);
  api.focus();
}

async function open(id, { quiet = false } = {}) {
  await save();
  try {
    show(await store.open(id), { isKept: true });
    return true;
  } catch (error) {
    if (!quiet) say(`The board could not be opened: ${error.message}`);
    return false;
  }
}

async function fresh() {
  await save();
  show(B.emptyBoard({ now: stamp() }));
}

// ---- the bar ------------------------------------------------------------------------------------------------
$('title').addEventListener('change', (e) => api.title(e.target.value.trim() || 'Untitled board'));
$('title').addEventListener('keydown', (e) => { if (e.key === 'Enter' || e.key === 'Escape') { e.target.blur(); api.focus(); } });
$('undo').addEventListener('click', () => api.undo());
$('redo').addEventListener('click', () => api.redo());
$('zoom-in').addEventListener('click', () => api.zoom(1.25));
$('zoom-out').addEventListener('click', () => api.zoom(0.8));
$('zoom-level').addEventListener('click', () => api.zoom(1 / api.view.k));
$('zoom-fit').addEventListener('click', () => api.fit());

function download(blob, name) {
  const a = el('a', { href: URL.createObjectURL(blob), download: name });
  document.body.append(a);
  a.click();
  a.remove();
  setTimeout(() => URL.revokeObjectURL(a.href), 4000);
}
const fileName = () => (api.board.title || 'board').replace(/[^\p{L}\p{N} _-]+/gu, '').trim().replace(/\s+/g, '-').toLowerCase() || 'board';
$('picture').addEventListener('click', async () => {
  try {
    say('Drawing the picture…');
    download(await B.boardPng(api, { scale: 2 }), `${fileName()}.png`);
    say('The picture is with your downloads.');
  } catch (error) { say(`No picture: ${error.message}.`); }
});
$('file').addEventListener('click', () => download(new Blob([JSON.stringify(api.board, null, 1)], { type: 'application/json' }), `${fileName()}.board.json`));

// ---- the drawer: templates, and the boards kept ---------------------------------------------------------------
const drawer = $('drawer');
let drawerShows = null;
function closeDrawer() {
  drawer.hidden = true;
  drawerShows = null;
  $('open-templates').classList.remove('on');
  $('open-boards').classList.remove('on');
}
function openDrawer(what, content) {
  drawer.replaceChildren(...content);
  drawer.hidden = false;
  drawer.scrollTop = 0;
  drawerShows = what;
  $('open-templates').classList.toggle('on', what === 'templates');
  $('open-boards').classList.toggle('on', what === 'boards');
}

function templatesDrawer() {
  const lay = (template) => { closeDrawer(); api.insert(template); api.focus(); };
  openDrawer('templates', [
    el('div', { class: 'drawer-head' }, el('h2', {}, 'Templates and modules'), el('button', { class: 'key', type: 'button', onclick: closeDrawer }, 'Close')),
    el('p', {}, 'A template is laid beside what is on the board already; a card or a module is laid on it and asks in the panel for what it needs. Or close this and double-click the ground for a first note.'),
    ...packs.flatMap((pack) => [
      el('h3', {}, pack.name), el('p', {}, pack.about || ''),
      !pack.templates.length ? null : el('div', { class: 'templates' }, pack.templates.map((t) => el('button', { class: 'template', type: 'button', 'data-template': `${pack.id}/${t.id}`, onclick: () => lay(t) },
        el('div', { class: 'thumb' }, B.templatePreview(t, api.types)), el('b', {}, t.name), el('span', {}, t.about || '')))),
      // Its cards: live views it adds to the board's own, each laid on the board and set in the panel.
      ...(Object.keys(pack.cards || {}).length ? [
        el('h4', {}, 'Cards'),
        el('div', { class: 'modules' }, Object.entries(pack.cards).map(([name, card]) => el('button', { class: 'module', type: 'button', 'data-card': name,
          onclick: () => { closeDrawer(); const id = api.add('card', { card: name, props: {} }); api.select([id]); api.focus(); } },
        el('b', {}, card.label || name), el('span', {}, card.about || '')))),
      ] : []),
      // Its modules: each declares what it needs; laid on the board as a card, its parameters set in the panel.
      ...(modules.some((m) => m.plugin === pack.id) ? [
        el('h4', {}, 'Modules'),
        el('div', { class: 'modules' }, modules.filter((m) => m.plugin === pack.id).map((m) => el('button', { class: 'module', type: 'button', 'data-module': m.ref, disabled: Boolean(m.error),
          onclick: () => { closeDrawer(); const id = api.add('card', { card: 'module', props: { module: m.ref, params: {} } }); api.select([id]); api.focus(); } },
        el('b', {}, m.title || m.ref), el('span', {}, m.error ? `It cannot be read: ${m.error}` : m.description || ''),
        m.params?.length ? el('small', {}, `It asks for: ${m.params.map((q) => q.label || q.name).join(' · ')}`) : null))),
      ] : []),
    ]),
  ]);
}

async function boardsDrawer() {
  let boards = [], failure = '';
  try { boards = await store.list(); } catch (error) { failure = error.message; }
  const pick = el('input', { type: 'file', accept: '.json,application/json', hidden: true });
  pick.addEventListener('change', async () => {
    const [file] = pick.files;
    if (!file) return;
    try {
      const board = JSON.parse(await file.text());
      const problems = B.checkBoard(board);
      if (problems.length) throw new Error(problems[0]);
      // A file opened beside a board of the same name is kept as another.
      if (boards.some((b) => b.id === board.id)) board.id = B.makeId();
      await save();
      show(board);
      closeDrawer();
      say('Opened from the file. It is kept once it is changed.');
    } catch (error) { say(`Not a board: ${error.message}.`); }
  });
  const when = (iso) => { const d = new Date(iso); return Number.isNaN(d.getTime()) ? '' : d.toLocaleString(undefined, { dateStyle: 'medium', timeStyle: 'short' }); };
  openDrawer('boards', [
    el('div', { class: 'drawer-head' }, el('h2', {}, 'Boards'),
      el('button', { class: 'key', type: 'button', onclick: async () => { await fresh(); templatesDrawer(); } }, 'New board'),
      el('button', { class: 'key', type: 'button', onclick: () => pick.click() }, 'Open a file…'), pick,
      el('button', { class: 'key', type: 'button', onclick: closeDrawer }, 'Close')),
    el('p', {}, failure ? `The boards kept could not be listed: ${failure}.` : boards.length ? 'Kept on this computer, the latest first.' : 'No board is kept yet. A board is kept once something is on it.'),
    el('div', { class: 'boards' }, boards.map((b) => el('div', { class: `board-row${b.id === api.board.id ? ' here' : ''}` },
      el('button', { class: 'open', type: 'button', onclick: async () => { if (await open(b.id)) closeDrawer(); } }, b.title || 'Untitled board'),
      el('small', {}, `${b.items} ${b.items === 1 ? 'item' : 'items'} · ${when(b.modified)}`),
      el('button', { class: 'key', type: 'button', title: 'Moved to the trash folder beside the boards; nothing is destroyed',
        onclick: async () => {
          try {
            await store.aside(b.id);
            if (b.id === api.board.id) show(B.emptyBoard({ now: stamp() }));
            say(`'${b.title || 'Untitled board'}' is put aside, in the trash folder of data/cache/tabula.`);
            boardsDrawer();
          } catch (error) { say(`Not put aside: ${error.message}.`); }
        } }, 'Put aside')))),
  ]);
}

$('open-templates').addEventListener('click', () => (drawerShows === 'templates' ? closeDrawer() : templatesDrawer()));
$('open-boards').addEventListener('click', () => (drawerShows === 'boards' ? closeDrawer() : boardsDrawer()));
window.addEventListener('keydown', (e) => {
  if (e.key === 'Escape' && drawerShows) { closeDrawer(); api.focus(); }
  if ((e.ctrlKey || e.metaKey) && (e.key === 's' || e.key === 'S')) { e.preventDefault(); save(); }
});
window.addEventListener('beforeunload', () => { if (!dry && api.board !== kept && worthKeeping(api.board)) save(); });
window.addEventListener('hashchange', () => { const id = location.hash.slice(1); if (id && id !== api.board.id) open(id); });

// ---- the beginning ------------------------------------------------------------------------------------------
async function begin() {
  const wanted = params.get('template');
  if (wanted) {
    const [packId, templateId] = wanted.split('/');
    const template = packs.find((p) => p.id === packId)?.templates.find((t) => t.id === templateId);
    if (template) { api.insert(template); api.focus(); return; }
    say(`There is no template '${wanted}'.`);
  }
  let last = null;
  try { last = localStorage.getItem('tabula.last'); } catch { /* none */ }
  // A board remembered from the last visit is opened only if it is still kept.
  if (last && !location.hash && !dry) { try { if (!(await store.list()).some((x) => x.id === last)) last = null; } catch { last = null; } }
  const id = location.hash.slice(1) || (dry ? null : last);
  if (id && await open(id)) return;
  reflect();
  templatesDrawer();
}
await begin();

// For the checks that drive the page, and for the curious.
window.tabula = { api, B, packs, modules, save };
