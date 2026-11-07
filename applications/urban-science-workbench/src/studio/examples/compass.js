// A north arrow and a scale bar, drawn by a script from the view (a system in JavaScript). It reads no resource: each frame it takes the
// world frame's heading and its metres to a pixel of the canvas (ctx.view), and draws into the canvas's frame "compass" in that frame's
// own pixels (ctx.frames.compass), so that the figure's furniture is a script like any other, and can be rewritten as one likes.

export default (api) => ({
  title: 'North and scale',
  phase: 'update',
  draw(ctx) {
    const f = ctx.frames?.compass, v = ctx.view;
    if (!f || !v) return [];
    const { w, h } = f, cx = w / 2, cy = h * 0.36, r = Math.min(w, h * 0.7) * 0.34;
    // North on the screen: the heading turns the map, so the arrow turns the other way.
    const a = -((v.heading || 0) * Math.PI) / 180;
    const at = (d, s) => [cx + s * Math.sin(a + d), cy - s * Math.cos(a + d)];
    const [nx, ny] = at(0, r), [lx, ly] = at(2.6, r * 0.62), [rx, ry] = at(-2.6, r * 0.62), [tx, ty] = at(0, r + 13);
    // A scale bar of a round length that fits three quarters of the frame.
    const mpp = v.metres_per_px || 0, room = w * 0.72 * mpp;
    const lengths = [1, 2, 5].flatMap((m) => [0, 1, 2, 3, 4, 5, 6, 7].map((e) => m * 10 ** e)).sort((p, q) => p - q);
    const nice = room > 0 ? lengths.filter((x) => x <= room).pop() || 0 : 0;
    const bar = nice && mpp ? nice / mpp : 0, x0 = (w - bar) / 2, y0 = h * 0.82;
    const label = nice >= 1000 ? `${nice / 1000} km` : `${nice} m`;
    return [api.drawing('compass', [
      { type: 'circle', cx, cy, r, stroke: 'ink-faint', fill: 'none', width: 0.8 },
      { type: 'path', d: `M${nx},${ny}L${lx},${ly}L${cx},${cy}Z`, fill: 'accent' },
      { type: 'path', d: `M${nx},${ny}L${rx},${ry}L${cx},${cy}Z`, fill: 'none', stroke: 'accent', width: 1 },
      { type: 'text', x: tx, y: ty + 4, text: 'N', size: 12, font: 'label', anchor: 'middle', fill: 'ink' },
      ...(bar ? [
        { type: 'line', x1: x0, y1: y0, x2: x0 + bar, y2: y0, stroke: 'ink', width: 1.4 },
        { type: 'line', x1: x0, y1: y0 - 4, x2: x0, y2: y0 + 4, stroke: 'ink', width: 1 },
        { type: 'line', x1: x0 + bar, y1: y0 - 4, x2: x0 + bar, y2: y0 + 4, stroke: 'ink', width: 1 },
        { type: 'text', x: w / 2, y: y0 + 18, text: label, size: 11, font: 'mono', anchor: 'middle', fill: 'ink-2' },
      ] : []),
    ])];
  },
});
