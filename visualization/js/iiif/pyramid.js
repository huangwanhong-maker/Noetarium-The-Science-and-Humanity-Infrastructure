// The arithmetic of IIIF tiles (ADR 0120): which tiles of an image service cover a view, at which scale, and the request for each
// (IIIF Image API 3.0, §4 and §5.6; https://iiif.io/api/image/3.0/). Pure: no page, no server; checked in Node.
//
//   - levels(info): the scale factors an image service offers, each with its size and its grid of tiles.
//   - factorFor(info, scale): the scale factor to draw at when one pixel of the image is `scale` pixels of the screen: the largest
//     whose resolution is still at least what the screen shows, so that tiles are never stretched more than twofold.
//   - tiles(info, factor, view): the tiles of a level that meet a view (a rectangle of the image's own pixels), each with its region,
//     its size and its request.
//   - url(id, region, width, options): a tile's request: {id}/{x},{y},{w},{h}/{w},/0/default.jpg, the form the API's tiles take.
//   - View: where the image is and how large, as a centre and a scale, with the transforms between the screen and the image, a fit,
//     and a zoom about a point that keeps that point where it is.

export function levels(info) {
  const tile = info.tiles?.[0] || { width: 512, scaleFactors: [1] };
  const tw = tile.width, th = tile.height || tile.width;
  return (tile.scaleFactors || [1]).slice().sort((a, b) => a - b).map((f) => {
    const width = Math.ceil(info.width / f), height = Math.ceil(info.height / f);
    return { factor: f, width, height, columns: Math.ceil(info.width / (tw * f)), rows: Math.ceil(info.height / (th * f)), tw, th };
  });
}

export function factorFor(info, scale) {
  const fs = levels(info).map((l) => l.factor);
  let best = fs[0];
  for (const f of fs) if (1 / f >= scale * 0.999) best = f;
  return best;
}

export function url(id, [x, y, w, h], width, { quality = 'default', format = 'jpg' } = {}) {
  return `${id.replace(/\/$/, '')}/${x},${y},${w},${h}/${width},/0/${quality}.${format}`;
}

export function tiles(info, factor, view) {
  const level = levels(info).find((l) => l.factor === factor);
  if (!level) throw new Error(`the image service offers no scale factor ${factor}`);
  const span = level.tw * factor, spanH = level.th * factor;
  const c0 = Math.max(0, Math.floor(view.x / span)), c1 = Math.min(level.columns - 1, Math.floor((view.x + view.w - 1e-9) / span));
  const r0 = Math.max(0, Math.floor(view.y / spanH)), r1 = Math.min(level.rows - 1, Math.floor((view.y + view.h - 1e-9) / spanH));
  const out = [];
  for (let r = r0; r <= r1; r++) {
    for (let c = c0; c <= c1; c++) {
      const x = c * span, y = r * spanH;
      const w = Math.min(span, info.width - x), h = Math.min(spanH, info.height - y);
      const width = Math.ceil(w / factor), height = Math.ceil(h / factor);
      out.push({ column: c, row: r, factor, region: [x, y, w, h], width, height, url: url(info.id, [x, y, w, h], width) });
    }
  }
  return out;
}

// A view of an image on a screen of width × height: (cx, cy) the image point at the screen's centre, `scale` screen pixels per image pixel.
export class View {
  constructor(width, height, image) {
    Object.assign(this, { width, height, image, cx: image.width / 2, cy: image.height / 2, scale: 1 });
    this.fit();
  }
  fit(margin = 0.94) {
    this.scale = Math.min(this.width / this.image.width, this.height / this.image.height) * margin;
    this.cx = this.image.width / 2;
    this.cy = this.image.height / 2;
    return this;
  }
  toScreen(x, y) { return [(x - this.cx) * this.scale + this.width / 2, (y - this.cy) * this.scale + this.height / 2]; }
  toImage(sx, sy) { return [(sx - this.width / 2) / this.scale + this.cx, (sy - this.height / 2) / this.scale + this.cy]; }
  // The image's pixels the screen shows, clipped to the image.
  visible() {
    const [x0, y0] = this.toImage(0, 0), [x1, y1] = this.toImage(this.width, this.height);
    const x = Math.max(0, x0), y = Math.max(0, y0);
    return { x, y, w: Math.max(0, Math.min(this.image.width, x1) - x), h: Math.max(0, Math.min(this.image.height, y1) - y) };
  }
  // Zoom by `by` about the screen point (sx, sy), which keeps the image point under it; the scale stays within [least, most].
  zoom(by, sx = this.width / 2, sy = this.height / 2, least = null, most = 8) {
    const [ix, iy] = this.toImage(sx, sy);
    const floor = least ?? Math.min(this.width / this.image.width, this.height / this.image.height) * 0.5;
    this.scale = Math.min(most, Math.max(floor, this.scale * by));
    this.cx = ix - (sx - this.width / 2) / this.scale;
    this.cy = iy - (sy - this.height / 2) / this.scale;
    return this;
  }
  pan(dx, dy) { this.cx -= dx / this.scale; this.cy -= dy / this.scale; return this; }
}
