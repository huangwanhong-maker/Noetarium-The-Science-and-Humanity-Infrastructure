// What is pure of a building in the round (ADR 0095): the pieces a wall is cut into at its openings, and the direction to the
// Sun in three.js's axes. The view itself is view.js.
//
// The ground's x (east) and y (north) and the height z go to three's X, −Z and Y. A wall is cut along its length at its openings:
// whole between them, a piece under a sill and a piece over a head at each; a window keeps a pane of glass.

// The pieces of a wall of length L and height H cut by openings [{at, width, height, sill}]: boxes [s0, s1] × [z0, z1] along it.
export function wallPieces(L, H, openings) {
  const pieces = [], sorted = openings.filter((o) => o.width > 0).slice().sort((p, q) => p.at - q.at);
  let s = 0;
  for (const o of sorted) {
    const from = Math.max(s, o.at - o.width / 2), to = Math.min(L, o.at + o.width / 2);
    if (to <= from) continue;
    if (from > s) pieces.push({ s0: s, s1: from, z0: 0, z1: H });
    const sill = Math.max(0, Math.min(H, o.sill || 0)), head = Math.max(sill, Math.min(H, sill + o.height));
    if (sill > 0) pieces.push({ s0: from, s1: to, z0: 0, z1: sill });
    if (head < H) pieces.push({ s0: from, s1: to, z0: head, z1: H });
    if (o.kind === 'window') pieces.push({ s0: from, s1: to, z0: sill, z1: head, glass: true });
    s = to;
  }
  if (s < L) pieces.push({ s0: s, s1: L, z0: 0, z1: H });
  return pieces;
}

// The direction to the Sun in three's axes, from its altitude and azimuth in degrees (azimuth from north through east).
export function sunDirection(altitude, azimuth) {
  const a = altitude * Math.PI / 180, z = azimuth * Math.PI / 180;
  return [Math.sin(z) * Math.cos(a), Math.sin(a), -Math.cos(z) * Math.cos(a)];
}
