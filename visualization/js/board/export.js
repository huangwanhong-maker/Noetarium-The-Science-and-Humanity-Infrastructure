// A board as a picture (DOM): what is drawn, with the page's colours and type written in, as an SVG
// that holds the drawing as it stands, and as a PNG drawn from it. The picture is the board's own
// drawing, so the two cannot disagree.
//
// The SVG carries the drawing in a foreignObject: browsers show it, drawing programs may not. A PNG
// is what goes into a document; the board's JSON is what is kept.

import { h } from './dom.js';
import { bounds } from './model.js';
import { embeddedFonts } from '../map/figure.js';

// The style sheet's variables, whose values on the page are written into the picture. (The checks hold
// this list to board.css.)
export const TOKENS = [
  '--bd-paper', '--bd-card', '--bd-sheet', '--bd-dot', '--bd-ink', '--bd-ink-2', '--bd-faint', '--bd-line', '--bd-shadow', '--bd-accent', '--bd-accent-2', '--bd-accent-ink', '--bd-wrong',
  '--bd-serif', '--bd-mono', '--bd-paper-ink',
  ...['apricot', 'sage', 'lilac', 'sky', 'rose', 'gold', 'clay'].flatMap((t) => [`--bd-${t}`, `--bd-${t}-ink`]),
];

const SHEET = new URL('./board.css', import.meta.url);
let sheet = null;
const styleSheet = () => (sheet ??= fetch(SHEET).then((r) => (r.ok ? r.text() : '')).catch(() => ''));

async function dataUrl(src) {
  if (!src || src.startsWith('data:')) return src;
  const blob = await (await fetch(src)).blob();
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    reader.onload = () => resolve(reader.result);
    reader.onerror = () => reject(new Error('an image of the board could not be read'));
    reader.readAsDataURL(blob);
  });
}

// The board as SVG text, with a margin about what it holds.
export async function boardSvg(api, { margin = 48 } = {}) {
  const box = bounds(api.board);
  if (!box) throw new Error('the board is empty: there is nothing to draw');
  const W = Math.ceil(box.w + 2 * margin), H = Math.ceil(box.h + 2 * margin);
  const { host, world } = api.elements;
  const computed = getComputedStyle(host);
  const vars = TOKENS.map((name) => [name, computed.getPropertyValue(name).trim()]).filter(([, v]) => v).map(([n, v]) => `${n}:${v}`).join(';');
  const copy = world.cloneNode(true);
  copy.querySelector('.bd-handles')?.remove();
  for (const el of copy.querySelectorAll('.sel, .target, .writing')) el.classList.remove('sel', 'target', 'writing');
  for (const el of copy.querySelectorAll('.bd-link.drawing')) el.remove();
  copy.style.transform = `translate(${Math.round(margin - box.x)}px, ${Math.round(margin - box.y)}px)`;
  for (const img of copy.querySelectorAll('img')) {
    try { img.setAttribute('src', await dataUrl(img.getAttribute('src'))); } catch { img.remove(); }
  }
  const [css, fonts] = await Promise.all([styleSheet(), embeddedFonts()]);
  const wrap = h('div', { class: 'bd-host bd-picture', 'data-theme': host.dataset.theme || null,
    style: `${vars};position:relative;width:${W}px;height:${H}px;overflow:hidden;background:${computed.getPropertyValue('--bd-paper').trim() || '#fff'}` },
  // A picture is still: nothing in it arrives or waits for a pointer, so nothing is caught half arrived.
  h('style', {}, `${fonts || ''}\n${css}\n.bd-picture .bd-item { animation: none !important; transition: none !important; }`), copy);
  const xml = new XMLSerializer().serializeToString(wrap);
  return { width: W, height: H, text: `<svg xmlns="http://www.w3.org/2000/svg" width="${W}" height="${H}" viewBox="0 0 ${W} ${H}"><foreignObject x="0" y="0" width="${W}" height="${H}">${xml}</foreignObject></svg>` };
}

// The board as a PNG at `scale` times its size.
export async function boardPng(api, { scale = 2, margin = 48 } = {}) {
  const svg = await boardSvg(api, { margin });
  // A picture of ten thousand pixels a side is as large as a canvas reliably holds.
  const k = Math.min(scale, 10000 / Math.max(svg.width, svg.height));
  return new Promise((resolve, reject) => {
    const img = new Image();
    img.onload = () => {
      const canvas = document.createElement('canvas');
      canvas.width = Math.round(svg.width * k);
      canvas.height = Math.round(svg.height * k);
      const g = canvas.getContext('2d');
      g.scale(k, k);
      g.drawImage(img, 0, 0);
      try { canvas.toBlob((blob) => (blob ? resolve(blob) : reject(new Error('the picture could not be made'))), 'image/png'); } catch (error) { reject(error); }
    };
    img.onerror = () => reject(new Error('the picture could not be drawn'));
    img.src = `data:image/svg+xml;charset=utf-8,${encodeURIComponent(svg.text)}`;
  });
}
