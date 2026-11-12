// The works and the drawing, switched on and off. A work switched off leaves the page: its layers are not drawn, its
// analyses leave the lenses, its readings are not asked at a point picked, and the server neither judges nor warms
// its engine. Three things the drawing does cost the most, and can be made lighter: the Sun's shadows, the pixels
// drawn, and how far the world's buildings reach. The choices are kept in this browser.

import { seg, toggle } from '../quiet.js';

// What a work holds and what it asks of the computer, in a line.
function costs(p) {
  const layers = (p.layers || []).length, tools = (p.tools || []).length;
  const out = [];
  if (layers) out.push(`${layers} ${layers === 1 ? 'layer' : 'layers'}`);
  if (tools) out.push(`${tools} ${tools === 1 ? 'analysis' : 'analyses'}`);
  if (p.engine) out.push('an engine in C++');
  if (p.reader) out.push('readings at every point picked');
  if ((p.layers || []).some((l) => l.default)) out.push('a layer on from the start');
  return out.join(' · ');
}

// body: the sheet to draw into; works: every plug-in not planned, in order; lenses: [{ id, title }].
// isOn(name), setOn(name, on), setAll(on); drawing: { shadows, pixels, reach }; setDrawing(key, value).
export function drawSwitches(h, body, { works, lenses, isOn, setOn, setAll, drawing, setDrawing, places }) {
  const on = works.filter((p) => isOn(p.name)).length;
  body.append(
    h('h1', {}, 'The works and the drawing'),
    h('p', { class: 'muted' }, 'A work switched off leaves the page: its layers are not drawn, its analyses leave the lenses, its readings are not asked at a point picked, and the server neither checks nor warms its engine. It comes back as it was when switched on. These choices are kept in this browser.'),
  );

  body.append(h('div', { class: 'sw-head' }, h('b', {}, 'The works'), h('span', { class: 'faint' }, `${on} of ${works.length} on`),
    h('span', { class: 'keys' }, h('button', { class: 'plain', onclick: () => setAll(true) }, 'all on'), h('button', { class: 'plain', onclick: () => setAll(false) }, 'all off'))));
  const lensOf = (p) => p.lens || p.layers?.[0]?.lens || p.tools?.[0]?.lens || 'fabric';
  for (const lens of lenses) {
    const here = works.filter((p) => lensOf(p) === lens.id);
    if (!here.length) continue;
    body.append(h('div', { class: 'lens-group' }, lens.title));
    for (const p of here) {
      const input = h('input', { type: 'checkbox', checked: isOn(p.name) || null, onchange: () => setOn(p.name, input.checked) });
      body.append(h('label', { class: 'sw-row', title: p.summary || p.tagline || '' }, input, h('span', { class: 'box' }),
        h('span', {}, h('span', { class: 'name' }, p.title), h('span', { class: 'meta' }, [costs(p), places(p)].filter(Boolean).join(' · ')))));
    }
  }

  body.append(h('div', { class: 'sw-head' }, h('b', {}, 'The drawing')));
  body.append(
    h('div', { class: 'sw-line' }, toggle(h, 'the Sun\'s shadows', drawing.shadows, (v) => setDrawing('shadows', v)),
      h('p', { class: 'meta' }, 'Every building\'s shadow, drawn again as the view and the clock move: the costliest thing the city draws.')),
    h('div', { class: 'sw-line' }, h('span', { class: 'sw-label' }, 'pixels'),
      seg(h, [['fine', 'as fine as the screen'], ['light', 'half as fine']], drawing.pixels, (v) => setDrawing('pixels', v)),
      h('p', { class: 'meta' }, 'Half as fine draws a quarter as many pixels: softer, and quicker on a screen of many.')),
    h('div', { class: 'sw-line' }, h('span', { class: 'sw-label' }, 'the world\'s buildings'),
      seg(h, [['far', 'toward the horizon'], ['near', 'close about the view']], drawing.reach, (v) => setDrawing('reach', v)),
      h('p', { class: 'meta' }, 'How many of the world\'s tiles of buildings and ways are held about the point looked at. Close about it holds about a quarter as many; the analyses that read the ways (reach, configuration, land use) see only what is held.')),
  );
}
