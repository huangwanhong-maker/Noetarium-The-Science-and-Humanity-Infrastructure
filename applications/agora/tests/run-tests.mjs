// Checks of what is Agora's own: its page (every element the script asks for, the files it loads), its recipe (every step a script
// and a manifest that exist), its server (a board kept and read back, what is not a board refused, another page refused), and its
// engine where it is built and the data are in the cache: the market of Melbourne's cafés and restaurants as the markets domain
// states it (conservation, homogeneity in what buyers bring, a trade area's total, diversion's parts, a new outlet's draw, the sites
// chosen), and the size of the market reckoned twice, by the engine's tree and by the board's quantities from the notes Agora would
// lay on it. The domain is checked with its own suite; the hand's work on the page by driving a browser (the README says how).
// Run from the repository root:  node applications/agora/tests/run-tests.mjs

import { spawn, spawnSync } from 'node:child_process';
import { existsSync, readFileSync, rmSync } from 'node:fs';
import { createServer } from 'node:net';
import { join, resolve } from 'node:path';

import { suite, ROOT, PRODUCTS } from '../../../development/tools/checks.mjs';
import { board as B } from '../../../visualization/js/index.js';

const { assert, check, skip, done } = suite();
const APP = resolve(ROOT, 'applications/agora');

// ---- the page -----------------------------------------------------------------------------------------
{
  const page = readFileSync(join(APP, 'index.html'), 'utf8'), app = readFileSync(join(APP, 'src/app.js'), 'utf8');
  const local = [...page.matchAll(/(?:href|src)="([^"#:]+)"/g)].map((m) => m[1]).filter((p) => !p.startsWith('data'));
  const missing = local.filter((p) => !existsSync(p.startsWith('/') ? join(ROOT, p) : join(APP, p)));
  assert('agora: the page\'s style sheets and script exist, the board\'s style sheet among them', !missing.length && local.includes('/visualization/js/board/board.css'), missing.join(', '));
  const ids = [...app.matchAll(/\$\('([a-z-]+)'\)/g)].map((m) => m[1]);
  assert('agora: every element the script asks for is on the page', ids.length > 10 && ids.every((i) => page.includes(`id="${i}"`)), ids.filter((i) => !page.includes(`id="${i}"`)).join(', '));
  const imports = [...app.matchAll(/from '(\/[^']+)'/g)].map((m) => m[1]);
  assert('agora: what the script imports of the visualization layer exists', imports.length >= 4 && imports.every((p) => existsSync(join(ROOT, p))), imports.filter((p) => !existsSync(join(ROOT, p))).join(', '));
  assert('agora: the page is set in serif type, names the port in its server, and says that its market is presumed',
    /Source\+Serif\+4/.test(page) && /default=9030/.test(readFileSync(join(APP, 'serve.py'), 'utf8')) && /presumed, not observed/.test(app));
}

// ---- the recipe -----------------------------------------------------------------------------------------
{
  const recipe = JSON.parse(readFileSync(join(APP, 'recipe.json'), 'utf8'));
  const missing = recipe.steps.filter((s) => {
    const [script, arg] = s.run;
    if (!existsSync(join(ROOT, script))) return true;
    return script.endsWith('fetch.py') && !existsSync(join(ROOT, 'data/registry', `${arg}.json`));
  });
  assert('agora: the recipe: every step a script and, for fetching, a manifest that exist', missing.length === 0 && recipe.steps[0].id === 'engine', missing.map((s) => s.id).join(', '));
}

// ---- the server, and the engine behind it -------------------------------------------------------------------
const python = ['python', 'python3', 'py'].find((p) => { try { return spawnSync(p, ['--version'], { encoding: 'utf8' }).status === 0; } catch { return false; } });
if (!python) skip('agora: the server (no Python found to run it)');
else {
  const port = await new Promise((r) => { const s = createServer(); s.listen(0, '127.0.0.1', () => { const p = s.address().port; s.close(() => r(p)); }); });
  const server = spawn(python, [join(APP, 'serve.py'), '--port', String(port)], { stdio: 'ignore' });
  const at = `http://127.0.0.1:${port}`;
  const boardId = `check-${Date.now().toString(36)}`, kept = join(ROOT, 'data/cache/agora/boards', `${boardId}.json`);
  try {
    let up = false, status = null;
    for (let k = 0; k < 80 && !up; k++) { await new Promise((r) => setTimeout(r, 150)); try { const r = await fetch(`${at}/api/status`); up = r.ok; status = await r.json(); } catch { /* not yet */ } }
    assert('agora: the server answers, and says whether its engine is built and which cities it holds', up && typeof status?.engine?.current === 'boolean' && Array.isArray(status.cities));
    if (up) {
      const json = async (path, options) => { const r = await fetch(`${at}${path}`, options); return { status: r.status, body: await r.json().catch(() => undefined) }; };
      const ask = (op, body) => json(`/api/${op}`, { method: 'POST', headers: { 'Content-Type': 'application/json' }, body: JSON.stringify(body) });

      // A board kept and read back; what is not a board, and another page, refused.
      const board = B.apply(B.emptyBoard({ id: boardId, title: 'A check' }), [{ t: 'add', id: 'n1', item: B.newItem('note', { text: 'kept — 市場' }), place: { x: 0, y: 0, w: 200, h: 132 } }]).board;
      const put = (id, body, headers = {}) => json(`/api/boards/${id}`, { method: 'PUT', headers: { 'Content-Type': 'application/json', ...headers }, body: typeof body === 'string' ? body : JSON.stringify(body) });
      const none = await json(`/api/boards/${boardId}`);
      const [stored, notBoard, elsewhere, outside] = [await put(boardId, board), await put(boardId, { schema: 'something/1' }), await put(boardId, board, { Origin: 'http://elsewhere.example' }), await put('..%2Fserve', board)];
      const back = await json(`/api/boards/${boardId}`);
      assert('agora: a board not yet kept is none, and no error; kept, it is read back as it was', none.status === 200 && none.body === null && stored.status === 200 && JSON.stringify(back.body) === JSON.stringify(board));
      assert('agora: what is not a board, a board from another page and a name outside the store are refused', notBoard.status === 400 && elsewhere.status === 403 && outside.status === 404, `${notBoard.status} ${elsewhere.status} ${outside.status}`);
      assert('agora: the server asks its engine only the operations it has', (await ask('remove', {})).status === 404 && (await json('/api/market')).status !== 200);

      const product = join(PRODUCTS, 'market-melbourne.json');
      if (!status.engine.current) {
        skip('agora: the engine is not built from the present sources: its answers are not checked');
        assert('agora: without its engine the server says so, and what to do', (await ask('market', {})).status === 503);
      } else if (!existsSync(product)) skip('agora: the places of Melbourne\'s market are not in the cache: the engine\'s answers are not checked');
      else {
        const P = JSON.parse(readFileSync(product, 'utf8'));
        const base = { city: 'melbourne', market: { kind: '4511' } };
        const m = (await ask('market', base)).body;
        const sum = (v) => v.reduce((a, x) => a + x, 0), rel = (a, b) => Math.abs(a / b - 1);
        const cafes = P.places.code.filter((c) => c === '4511').length, dwellings = sum(P.blocks.map((b) => b.dwellings || 0)), office = sum(P.blocks.map((b) => b.office_m2 || 0));
        assert('agora: the market is of the census\'s places of the kind, its buyers the dwellings and offices by the two assumptions', m.offers === cafes && m.offer.demand.length === cafes
          && rel(m.buyers.residents, 2 * dwellings) < 1e-12 && rel(m.buyers.workers, 5 * office / 100) < 1e-12 && m.provenance === 'presumed', `${m.offers} outlets of ${m.sellers} sellers`);
        check('agora: what all who could buy bring is residents × 30 × 48 + workers × 25 × 48', rel(m.total, m.buyers.residents * 30 * 48 + m.buyers.workers * 25 * 48), 1e-12);
        // The engine rounds what it sends to whole units of currency.
        check('agora: the outlets\' demands add to what is bought', rel(sum(m.offer.demand), m.bought), 1e-6);
        check('agora: the sellers\' demands add to it too', rel(sum(m.seller.demand), m.bought), 1e-6);
        check('agora: the blocks bring the whole, and what of it buys nothing is the rest', rel(sum(m.block.brings), m.total) + Math.abs((sum(m.block.unserved) - (m.total - m.bought)) / m.total), 1e-6);
        check('agora: penetration is the part that buys; the equivalent number of sellers is 1 / HHI', Math.abs(m.penetration - m.bought / m.total) + rel(m.structure.equivalent, 1 / m.structure.hhi), 1e-12);
        assert('agora: a seller is a trading name: fewer sellers than outlets, each outlet with one', m.sellers < m.offers && m.sellers === m.seller.name.length && sum(m.seller.outlets) === m.offers && m.offer.seller.every((s) => s < m.sellers));

        // Homogeneity: what everyone brings doubled doubles every demand and leaves the shares.
        const twice = (await ask('market', { city: 'melbourne', market: { kind: '4511', spend_resident: 60, spend_worker: 50 } })).body;
        check('agora: with what everyone brings doubled, what is bought doubles and the penetration stays', rel(twice.bought, 2 * m.bought) + Math.abs(twice.penetration - m.penetration), 1e-9);
        const near = (await ask('market', { city: 'melbourne', market: { kind: '4511', limit: 6 } })).body, none5 = (await ask('market', { city: 'melbourne', market: { kind: '4511', outside: 0 } })).body;
        assert('agora: a shorter reach serves less; with no pull of not buying, every cell with an outlet within reach spends all it brings', near.penetration < m.penetration && near.pairs < m.pairs && none5.penetration > m.penetration);

        // One outlet.
        const o = m.offer.demand.indexOf(Math.max(...m.offer.demand)), a = (await ask('offer', { ...base, offer: o })).body;
        check('agora: an outlet\'s trade area adds to what it sells', rel(sum(a.trade), a.demand), 1e-5);
        check('agora: its core holds the share asked for', rel(a.core.custom, 0.6 * a.demand), 1e-9);
        assert('agora: half its custom lies nearer than four fifths; where its custom would go adds to no more than all of it; its nearest rivals share its buyers',
          a.reach.half <= a.reach.most && sum(a.diversion.to.map((d) => d.part)) + a.diversion.outside <= 1 + 1e-9 && a.diversion.to.length === 12 && a.diversion.to[0].part >= a.diversion.to[11].part
          && a.diversion.to.every((d) => d.overlap > 0 && d.overlap <= 1 + 1e-12), `${a.reach.half.toFixed(2)} and ${a.reach.most.toFixed(2)} minutes`);

        // A new outlet, for a newcomer and for the seller with most outlets.
        const chain = m.seller.outlets.indexOf(Math.max(...m.seller.outlets)), at0 = [P.blocks[0].x, P.blocks[0].y];
        const fresh = (await ask('enter', { ...base, at: at0, seller: -1 })).body, own = (await ask('enter', { ...base, at: [P.places.x[m.offer.place[m.offer.seller.indexOf(chain)]] + 30, P.places.y[m.offer.place[m.offer.seller.indexOf(chain)]]], seller: chain })).body;
        check('agora: a new outlet\'s demand is what it draws from rivals, from its own seller and from those who bought nothing', rel(fresh.from_rivals + fresh.from_outside + fresh.cannibalised, fresh.demand) + rel(own.from_rivals + own.from_outside + own.cannibalised, own.demand), 1e-9);
        assert('agora: a newcomer takes nothing from itself; a chain beside its own outlet does, and gains less than the outlet sells', fresh.cannibalised === 0 && fresh.gain === fresh.demand && own.cannibalised > 0 && own.gain < own.demand,
          `${m.seller.name[chain]}: ${Math.round(own.cannibalised)} of ${Math.round(own.demand)}`);

        // Where to open.
        const s = (await ask('sites', { ...base, k: 3, from: 'vacant', seller: -1 })).body, vacant = P.places.code.filter((c) => c === '0000').length;
        assert('agora: three of the census\'s vacant spaces are chosen, each adding no more than the one before', s.chosen.length === 3 && s.candidates > 100 && s.candidates <= vacant && s.chosen[0].gain >= s.chosen[1].gain && s.chosen[1].gain >= s.chosen[2].gain);
        check('agora: their gains add to what the seller gains', rel(sum(s.chosen.map((c) => c.gain)), s.after - s.before), 1e-9);
        const first = (await ask('enter', { ...base, at: [s.chosen[0].x, s.chosen[0].y], seller: -1 })).body;
        check('agora: the first site\'s gain is what an outlet entering there would sell', rel(first.demand, s.chosen[0].gain), 1e-9);
        assert('agora: no vacant space pressed at random does better than the first site', (await Promise.all([3, 57, 211, 640].map((k) => { const i = P.places.code.map((c, j) => (c === '0000' ? j : -1)).filter((j) => j >= 0)[k]; return ask('enter', { ...base, at: [P.places.x[i], P.places.y[i]], seller: -1 }); })))
          .every((r) => r.body.demand <= s.chosen[0].gain * (1 + 1e-9)));

        // The size, by the engine's tree and by the board's quantities.
        const ranges = { residents_per_dwelling: [1.8, 2.3], workers_per_100m2: [4, 6.7], spend_resident: [20, 45], spend_worker: [15, 40], weeks: [46, 50] };
        const z = (await ask('size', { ...base, ranges, seller: chain })).body, leaf = (n) => z.nodes.find((x) => x.name === n);
        check('agora: the middle of the size is what all who could buy bring', rel(z.all.mid, m.total), 1e-12);
        check('agora: its low is every assumption at its least, its high every one at its most', rel(z.all.low, dwellings * 1.8 * 20 * 46 + office / 100 * 4 * 15 * 46) + rel(z.all.high, dwellings * 2.3 * 45 * 50 + office / 100 * 6.7 * 40 * 50), 1e-12);
        assert('agora: the weeks of a year bear on all of it (elasticity 1), the others on their part; the census\'s counts are data, the rest assumptions',
          Math.abs(leaf('weeks').elasticity - 1) < 1e-12 && Math.abs(leaf('spend_resident').elasticity + leaf('spend_worker').elasticity - 1) < 1e-12 && !leaf('dwellings').assumed && leaf('spend_worker').assumed && z.swings.length === 5);
        assert('agora: a seller\'s obtained market lies within its serviceable market, and that within the total', z.seller.sells > 0 && z.seller.sells <= z.seller.served && z.seller.served <= z.all.mid);
        const line = (n) => { const x = leaf(n); return x.assumed && x.high > x.low ? `${n} = ${x.mid} [${x.low}, ${x.high}]` : `${n} = ${x.mid}`; };
        const notes = B.apply(B.emptyBoard(), [
          { t: 'add', id: 'a', item: B.newItem('note', { type: 'fact', text: `${line('dwellings')}\n${line('office_100m2')}` }), place: { x: 0, y: 0, w: 200, h: 132 } },
          { t: 'add', id: 'b', item: B.newItem('note', { type: 'assumption', text: ['residents_per_dwelling', 'spend_resident', 'workers_per_100m2', 'spend_worker', 'weeks'].map(line).join('\n') }), place: { x: 300, y: 0, w: 200, h: 132 } },
          { t: 'add', id: 'c', item: B.newItem('figure', { formula: 'residents = dwellings * residents_per_dwelling * spend_resident * weeks\nworkers = office_100m2 * workers_per_100m2 * spend_worker * weeks\nTAM = residents + workers' }), place: { x: 600, y: 0, w: 290, h: 150 } },
        ]).board;
        const tam = B.evaluate(B.collect(notes)).get('TAM');
        check('agora: the board reckons the size again from the notes Agora lays on it, and finds the engine\'s middle and bounds', tam.error ? 1 : rel(tam.mid, z.all.mid) + rel(tam.lo, z.all.low) + rel(tam.hi, z.all.high), 1e-12);

        const [noKind, noOffer, noPoint, far] = [await ask('market', { city: 'melbourne', market: { kind: '9999' } }), await ask('offer', { ...base, offer: 1e6 }), await ask('enter', { ...base }), await ask('enter', { ...base, at: [9e5, 9e5] })];
        assert('agora: a kind the census does not hold, an outlet that is none, an outlet nowhere and one beyond every buyer\'s reach are refused with the reason',
          [noKind, noOffer, noPoint, far].every((r) => r.status === 400 && r.body.message.length > 12), [noKind, noOffer, noPoint, far].map((r) => r.status).join(' '));
      }
    }
  } finally {
    server.kill();
    await new Promise((r) => setTimeout(r, 200));
    try { rmSync(kept, { force: true }); } catch { /* the cache is the person's to clear */ }
  }
}

done();
