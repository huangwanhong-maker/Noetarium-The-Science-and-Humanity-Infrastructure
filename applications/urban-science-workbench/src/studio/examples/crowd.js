// A crowd that wanders about where the camera first looked: a system in JavaScript. Each tick it moves every walker a random step from
// where it stood at the end of the last tick (previous: "crowd"), and writes the crowd, which other scripts read ("crowd" is shared).
// The size of a step is the shared resource "wander", which the inspector can set while it runs.

export default (api) => {
  let seed = 7;   // a seeded stream, so that a run is the same each time
  const random = () => ((seed = (seed * 1103515245 + 12345) % 2147483648) / 2147483648);
  let crowd = null;
  return {
    title: 'A crowd that wanders',
    phase: 'tick',
    reads: ['wander'],
    writes: ['crowd'],
    previous: ['crowd'],
    resources: { wander: { type: 'number', initial: 0.03 }, crowd: { type: 'points', initial: null } },
    params: [api.param('walkers', 'Walkers', 400, { min: 1, max: 5000 }), api.param('spread', 'First spread (degrees)', 1.5, { min: 0.01, max: 20 })],
    tick(ctx, io) {
      const step = io.get('wander') ?? 0.03, n = ctx.params.walkers ?? 400;
      let c = io.prev('crowd');
      if (!c || c.lon.length !== n) {
        const v = ctx.view || { lon: 144.96, lat: -37.81 }, w = ctx.params.spread ?? 1.5;
        c = { lon: [], lat: [] };
        for (let k = 0; k < n; k++) { c.lon.push(v.lon + (random() - 0.5) * w); c.lat.push(v.lat + (random() - 0.5) * w); }
      }
      crowd = { lon: c.lon.map((x) => x + (random() - 0.5) * step), lat: c.lat.map((y) => y + (random() - 0.5) * step) };
      io.set('crowd', crowd);
    },
    draw() {
      return crowd ? [api.points('The crowd', { lon: crowd.lon, lat: crowd.lat, size: 6, colour: '#EE8FBD' })] : [];
    },
    save: () => ({ seed, crowd }),
    load(state) { seed = state.seed; crowd = state.crowd; },
  };
};
