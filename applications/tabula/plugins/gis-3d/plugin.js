// The city in three dimensions, as a plug-in of the board: a card that stands a table's places on a
// city of the data library's city model (its buildings at their heights, its streets at their widths,
// its parks and water), each place a column whose height and colour a column of the table gives. It is
// the visualization layer's 3D GIS (visualization/js/gis: the exact local frame on WGS 84 and the scene
// of a city), on three.js, both fetched only when a card of this kind is first drawn.
//
// The card finds the city by the places: the area of the city model whose box holds them. A place
// within a building stands on its roof, where it can be seen. A table
// whose places lie in no area of the model is told so; the 2D map draws any place on Earth.

export default function gis3d(B) {
  const { h } = B;

  const PALETTES = {
    day: { building: [0.96, 0.93, 0.95], tall: [0.86, 0.78, 0.9], guessed: [0.985, 0.97, 0.98], pedestrian: [0.96, 0.85, 0.9], road: [0.9, 0.87, 0.91], minor: [0.94, 0.92, 0.95],
      park: [0.8, 0.87, 0.78], water: [0.76, 0.8, 0.93], plaza: [0.97, 0.93, 0.95], rail: [0.86, 0.83, 0.88], background: '#fbf6f4', ground: '#f3ecef', place: [0.6, 0.31, 0.53] },
    night: { building: [0.3, 0.18, 0.38], tall: [0.62, 0.34, 0.66], guessed: [0.215, 0.14, 0.27], pedestrian: [0.62, 0.2, 0.42], road: [0.075, 0.045, 0.105], minor: [0.05, 0.032, 0.072],
      park: [0.055, 0.125, 0.11], water: [0.03, 0.026, 0.115], plaza: [0.11, 0.065, 0.14], rail: [0.08, 0.055, 0.105], background: '#170f1c', ground: '#1d1228', place: [0.93, 0.62, 0.82] },
  };
  const PEDESTRIAN = new Set(['pedestrian', 'footway', 'path', 'steps', 'living_street', 'cycleway', 'corridor']);
  const MAJOR = new Set(['motorway', 'trunk', 'primary', 'secondary', 'tertiary', 'motorway_link', 'trunk_link', 'primary_link', 'secondary_link']);
  const rgbOf = (hex) => [1, 3, 5].map((k) => parseInt(hex.slice(k, k + 2), 16) / 255);

  const city = {
    label: 'City in 3D', about: 'A table\'s places standing on a city of the data library\'s city model: each a column, its height and its colour by a column of the table.', size: { w: 760, h: 560 },
    props: { table: '', height: '', colour: '', label: '', title: '' },
    form: () => [
      { name: 'table', label: 'Table', kind: 'text', role: 'table', help: 'with columns lat and lon' },
      { name: 'height', label: 'As tall as', kind: 'text', role: 'column', of: 'table' },
      { name: 'colour', label: 'Coloured by', kind: 'text', role: 'column', of: 'table' },
      { name: 'label', label: 'Named by', kind: 'text', role: 'column', of: 'table' },
      { name: 'title', label: 'Title', kind: 'text' },
    ],
    signature: (ctx) => `${B.tableStamp(ctx.tables.get(ctx.props.table))}/${document.documentElement.dataset.theme || 'day'}`,
    render(body, ctx) {
      const p = ctx.props, t = ctx.tables.get(p.table);
      const say = (words, wrong = false) => body.append(h('div', { class: wrong ? 'bd-hint wrong' : 'bd-hint' }, words));
      if (!t || t.error) { say(t?.error || (p.table ? `There is no table '${p.table}' on the board: run the module that answers it.` : 'Choose a table in the panel.')); return undefined; }
      const lat = B.findColumn(t, 'lat', 'latitude'), lon = B.findColumn(t, 'lon', 'lng', 'long', 'longitude');
      if (!lat || !lon) { say(`The table '${t.name}' has no lat and lon: a city does not know where its rows stand.`); return undefined; }
      if (!ctx.services.data) { say('This work serves no data: a city cannot be drawn here.'); return undefined; }
      const rows = [];
      for (let i = 0; i < t.rows && rows.length < 20000; i++) if (typeof t.data[lat][i] === 'number' && typeof t.data[lon][i] === 'number') rows.push(i);
      if (!rows.length) { say('None of its rows has a place.'); return undefined; }

      const box = h('div', { class: 'tb-city', 'data-live': true, style: `height:${Math.max(160, ctx.height - 96)}px` }, h('div', { class: 'bd-hint' }, 'Raising the city…'));
      const readout = h('div', { class: 'bd-map-readout' }, p.title || t.title || t.name);
      body.append(box, readout);
      // The wheel over the city is the city's: the board beneath does not zoom.
      box.addEventListener('wheel', (e) => e.stopPropagation(), { passive: true });
      let scene = null, gone = false;

      (async () => {
        try {
          const index = await ctx.services.data('products/city-model.json');
          // The area of the city model that holds the most of the places.
          const within = (a, i) => t.data[lat][i] >= a.bbox[0] && t.data[lat][i] <= a.bbox[2] && t.data[lon][i] >= a.bbox[1] && t.data[lon][i] <= a.bbox[3];
          const area = (index.areas || []).map((a) => ({ a, n: rows.filter((i) => within(a, i)).length })).sort((u, v) => v.n - u.n)[0];
          if (!area || !area.n) throw new Error(`no city of the data library's city model holds these places (it has: ${(index.areas || []).map((a) => a.id).join(', ')}); the map draws them wherever they are`);
          const [THREE, controls, G, S, model] = await Promise.all([import('three'), import('three/addons/controls/MapControls.js'), import('/visualization/js/gis/index.js'),
            import('/visualization/styles/index.js'), ctx.services.data(`products/${area.a.file}`)]);
          if (gone) return;
          const P = PALETTES[document.documentElement.dataset.theme === 'night' ? 'night' : 'day'];
          const mix = (a, b, u) => a.map((v, k) => v + (b[k] - v) * u);
          box.replaceChildren();
          scene = G.cityScene(THREE, { MapControls: controls.MapControls }, box, { background: P.background, ground: P.ground, shadows: false });
          scene.addFlats('spaces', model.spaces || [], { ring: (f) => f.ring, holes: (f) => f.holes || [], color: (f) => P[f.kind] || P.plaza, z: 0.03, pick: false });
          scene.addRibbons('streets', (model.roads || []).filter((r) => r.line && r.line.length >= 2), { line: (f) => f.line, width: (f) => Math.max(1.2, f.width_m || 6),
            color: (f) => (PEDESTRIAN.has(f.kind) ? P.pedestrian : MAJOR.has(f.kind) ? P.road : P.minor), z: 0.08, pick: false });
          scene.addSolids('buildings', model.buildings || [], { ring: (f) => f.footprint, holes: (f) => f.holes || [], base: (f) => f.min_height_m || 0, height: (f) => f.height_m || 9,
            color: (f) => (f.height_source === 'default' ? P.guessed : mix(P.building, P.tall, Math.min(1, (f.height_m || 0) / 120))) });

          // The places, in the city's own frame: exact on WGS 84, east and north in metres from its origin.
          const frame = G.localFrameAt(model.origin.lat, model.origin.lon);
          const tall = t.data[p.height] && t.columns.find((c) => c.name === p.height)?.kind === 'number' ? t.data[p.height] : null;
          const most = tall ? Math.max(...rows.map((i) => tall[i]).filter((v) => typeof v === 'number'), 0) : 0;
          const by = t.data[p.colour] ? p.colour : null, numeric = by && t.columns.find((c) => c.name === by)?.kind === 'number';
          const values = by ? rows.map((i) => t.data[by][i]) : [];
          const lo = numeric ? Math.min(...values.filter((v) => typeof v === 'number')) : 0, hi = numeric ? Math.max(...values.filter((v) => typeof v === 'number')) : 1;
          const kinds = by && !numeric ? [...new Set(values.filter((v) => v !== null && v !== ''))] : [];
          const ramp = S.colourMap('orchid').colour;
          const hues = [1, 2, 3, 4, 5, 6].map((k) => rgbOf(getComputedStyle(document.documentElement).getPropertyValue(`--tb-series-${k}`).trim() || '#9a4f86'));
          const label = t.data[p.label] ? p.label : B.findColumn(t, 'name', 'title', 'label');
          // A place within a building stands on its roof, where it can be seen: the roof under a point,
          // by the building's box and then its footprint.
          const boxes = (model.buildings || []).map((f) => { const e = G.extent(f.footprint); return { f, ...e }; });
          const roofAt = (x, y) => {
            let top = 0;
            for (const bx of boxes) if (x >= bx.x0 && x <= bx.x1 && y >= bx.y0 && y <= bx.y1 && G.inRing([x, y], bx.f.footprint)) top = Math.max(top, bx.f.height_m || 9);
            return top;
          };
          const places = rows.filter((i) => within(area.a, i)).map((i) => {
            const [x, y] = frame.toLocal(t.data[lat][i], t.data[lon][i]);
            const v = by ? t.data[by][i] : null;
            const colour = !by ? P.place : numeric ? (typeof v === 'number' ? ramp(hi > lo ? (v - lo) / (hi - lo) : 0.5) : [0.7, 0.7, 0.7]) : kinds.includes(v) ? hues[kinds.indexOf(v) % hues.length] : [0.7, 0.7, 0.7];
            return { at: [x, y], roof: roofAt(x, y), tall: tall && most > 0 && typeof tall[i] === 'number' ? 8 + 160 * Math.max(0, tall[i]) / most : 18, colour,
              told: `${label ? t.data[label][i] ?? '' : ''}${tall ? ` · ${p.height} ${tall[i] ?? '—'}` : ''}${by ? ` · ${by} ${v ?? '—'}` : ''}` };
          });
          scene.addPoints('places', places, { position: (f) => f.at, radius: () => 5.5, base: (f) => f.roof, height: (f) => f.tall, color: (f) => f.colour, opacity: 0.94, pick: (f) => f.told });
          const reach = scene.extentOf(places.map((f) => f.at)), pad = 120;
          scene.fit({ x0: reach.x0 - pad, y0: reach.y0 - pad, x1: reach.x1 + pad, y1: reach.y1 + pad }, { tilt: 56, heading: -24, range: 0.78 });
          scene.setSun({ azimuth: (135 * Math.PI) / 180, altitude: (48 * Math.PI) / 180 });
          const tell = (hit) => { readout.textContent = hit?.layer === 'places' ? hit.feature.told : hit?.feature?.name || p.title || t.title || t.name; };
          scene.onHover(tell);
          scene.onPick(tell);
          const outside = rows.length - places.length;
          body.append(h('div', { class: 'bd-map-note' }, `${places.length.toLocaleString('en-GB')} places on ${area.a.id}${outside ? `; ${outside} lie outside its model` : ''} · ${model.credit || ''}${t.source ? ` · ${t.source}` : ''}`));
          box.dataset.raised = String(places.length);
        } catch (error) {
          if (!gone) box.replaceChildren(h('div', { class: 'bd-hint wrong' }, `The city could not be raised: ${error.message}.`));
        }
      })();
      // The scene holds a graphics context: it is given back when the card is drawn again or taken away.
      return () => { gone = true; try { scene?.dispose(); } catch { /* it is gone either way */ } };
    },
  };

  return {
    id: 'gis-3d', name: 'The city in three dimensions', about: 'A table\'s places standing on a city of the data library, in the round.',
    noteTypes: [], templates: [], cards: { city3d: city },
  };
}
