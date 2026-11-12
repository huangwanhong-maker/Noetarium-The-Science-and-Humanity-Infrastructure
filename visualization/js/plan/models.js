// The catalogue's pieces in the round (ADR 0096): each kind built from a few eased solids to the size its piece has, so that a sofa
// made wider is a wider sofa. A piece's frame: lx to its right, ly toward its front, y up from the floor; its back is at
// ly = −depth/2. The kit (materials.js) places a solid by the middle of its base in that frame.
//
// These are examples, drawn here and owned by no one else: a plain, quiet furniture, so that a room reads as a room. A piece may
// instead carry a model of one's own (a glTF file), which the view loads and fits to the piece's size.

import { sequence } from './materials.js';

const seedOf = (p) => Math.floor(Math.abs(p.at[0] * 733 + p.at[1] * 1987 + p.width * 31)) + 7;

function legs(k, w, d, h, material, inset = 0.05, r = 0.018) {
  const out = [];
  for (const sx of [-1, 1]) for (const sy of [-1, 1]) out.push(k.cyl(r, h, material, sx * (w / 2 - inset), 0, sy * (d / 2 - inset), { segments: 12 }));
  return out;
}

function sofa(k, p, seats) {
  const W = p.width, D = p.depth, H = p.height, body = k.cloth(p.finish, 1), cushion = k.cloth(p.finish, 1.06), arm = 0.17, back = 0.2, base = 0.13;
  const parts = [k.soft(W, 0.2, D - 0.04, body, 0, base, 0, 0.04), ...legs(k, W, D, base, k.mat.walnut, 0.07, 0.02),
    k.soft(W - 0.06, H - base - 0.1, back, body, 0, base + 0.1, -D / 2 + back / 2, 0.06),
    k.soft(arm, 0.4, D - 0.06, body, -(W / 2 - arm / 2), base + 0.1, 0, 0.06), k.soft(arm, 0.4, D - 0.06, body, W / 2 - arm / 2, base + 0.1, 0, 0.06)];
  const span = (W - 2 * arm) / seats;
  for (let i = 0; i < seats; i++) {
    const x = -W / 2 + arm + (i + 0.5) * span;
    parts.push(k.soft(span - 0.02, 0.15, D - back - 0.1, cushion, x, base + 0.19, back / 2 + 0.02, 0.055));
    const pillow = k.soft(span - 0.06, 0.36, 0.15, cushion, x, base + 0.3, -D / 2 + back + 0.09, 0.065);
    pillow.rotation.x = 0.16;
    parts.push(pillow);
  }
  return k.group(parts);
}

function chair(k, p, cloth) {
  const W = Math.min(p.width, 0.46), seat = 0.44;
  const backrest = k.soft(W, 0.36, 0.035, cloth, 0, seat + 0.07, -0.2, 0.017);
  backrest.rotation.x = 0.1;
  return k.group(k.soft(W, 0.05, 0.43, cloth, 0, seat - 0.02, 0.0, 0.02), ...legs(k, W, 0.43, seat - 0.02, k.mat.oak, 0.035, 0.016), backrest,
    k.cyl(0.014, 0.4, k.mat.oak, -W / 2 + 0.035, seat, -0.2, { segments: 10 }), k.cyl(0.014, 0.4, k.mat.oak, W / 2 - 0.035, seat, -0.2, { segments: 10 }));
}

function diningSet(k, p, perSide) {
  const W = p.width, H = p.height, top = 0.9, cloth = k.cloth(p.finish, 1);
  const parts = [k.soft(W, 0.04, top, k.mat.oak, 0, H - 0.04, 0, 0.012)];
  for (const sx of [-1, 1]) for (const sy of [-1, 1]) parts.push(k.box(0.06, H - 0.04, 0.06, k.mat.oak, sx * (W / 2 - 0.09), 0, sy * (top / 2 - 0.09)));
  parts.push(k.box(W - 0.3, 0.06, 0.03, k.mat.oak, 0, H - 0.1, top / 2 - 0.1), k.box(W - 0.3, 0.06, 0.03, k.mat.oak, 0, H - 0.1, -top / 2 + 0.1));
  for (const side of [-1, 1]) for (let i = 0; i < perSide; i++) {
    const c = chair(k, { width: 0.44 }, cloth);
    c.position.set(-W / 2 + W * (i + 0.5) / perSide, 0, -side * (top / 2 + 0.12));
    if (side < 0) c.rotation.y = 0; else c.rotation.y = Math.PI;
    parts.push(c);
  }
  // A bowl on the table.
  parts.push(k.cyl(0.07, 0.07, k.mat.ceramic, 0, H, 0, { top: 0.16 }));
  return k.group(parts);
}

function cabinets(k, p, { worktop = k.mat.stone, front = k.mat.cabinet, overhang = 0 } = {}) {
  const W = p.width, D = p.depth, H = p.height, parts = [];
  parts.push(k.box(W - 0.004, H - 0.14, D - 0.05 - overhang, front, 0, 0.1, -overhang / 2 - 0.02), k.box(W - 0.02, 0.1, D - 0.14 - overhang, k.mat.black, 0, 0, -overhang / 2 - 0.05),
    k.soft(W, 0.04, D, worktop, 0, H - 0.04, 0, 0.006));
  const n = Math.max(1, Math.round(W / 0.6)), span = W / n;
  for (let i = 0; i < n; i++) {
    const x = -W / 2 + (i + 0.5) * span;
    parts.push(k.box(span - 0.008, H - 0.16, 0.018, front, x, 0.11, D / 2 - 0.036 - overhang));
    const handle = k.cyl(0.006, 0.16, k.mat.brass, x, H - 0.13, D / 2 - overhang - 0.014, { segments: 8 });
    handle.rotation.z = Math.PI / 2;
    handle.position.y = H - 0.13;
    parts.push(handle);
  }
  return parts;
}

function bed(k, p, pillows) {
  const W = p.width, D = p.depth, cloth = k.cloth(p.finish, 1), parts = [];
  parts.push(k.soft(W, 0.2, D, k.mat.oak, 0, 0.1, 0, 0.02), ...legs(k, W, D, 0.1, k.mat.oak, 0.06, 0.025),
    k.soft(W + 0.04, 1.0, 0.08, k.cloth(p.finish, 0.92), 0, 0.06, -D / 2 + 0.04, 0.035),
    k.soft(W - 0.06, 0.2, D - 0.14, k.mat.bedding, 0, 0.3, 0.03, 0.07));
  const span = (W - 0.16) / pillows;
  for (let i = 0; i < pillows; i++) {
    const pillow = k.soft(span - 0.08, 0.13, 0.42, k.mat.bedding, -W / 2 + 0.08 + (i + 0.5) * span, 0.49, -D / 2 + 0.36, 0.06);
    pillow.rotation.x = 0.12;
    parts.push(pillow);
  }
  parts.push(k.soft(W - 0.02, 0.09, D * 0.6, cloth, 0, 0.46, D * 0.19, 0.045), k.soft(W + 0.02, 0.05, 0.5, k.cloth(p.finish, 0.86), 0, 0.53, D / 2 - 0.3, 0.025));
  return k.group(parts);
}

function tableLamp(k, lx, y, ly) {
  return [k.cyl(0.045, 0.02, k.mat.brass, lx, y, ly), k.cyl(0.008, 0.2, k.mat.brass, lx, y + 0.02, ly, { segments: 8 }), k.cyl(0.09, 0.13, k.mat.shade, lx, y + 0.2, ly, { top: 0.065, open: true })];
}

const BUILDERS = {
  'sofa-3': (k, p) => sofa(k, p, 3),
  'sofa-2': (k, p) => sofa(k, p, 2),
  armchair: (k, p) => sofa(k, p, 1),
  'coffee-table': (k, p) => k.group(k.soft(p.width, 0.035, p.depth, k.mat.oak, 0, p.height - 0.035, 0, 0.012), ...legs(k, p.width, p.depth, p.height - 0.035, k.mat.black, 0.07, 0.014),
    k.soft(p.width - 0.16, 0.02, p.depth - 0.16, k.mat.oak, 0, 0.12, 0, 0.008), k.box(0.26, 0.03, 0.19, k.cloth('rose', 0.9), -0.2, p.height, 0.03), k.box(0.24, 0.025, 0.17, k.cloth('slate', 1.1), -0.2, p.height + 0.03, 0.03),
    k.cyl(0.05, 0.11, k.mat.ceramic, 0.25, p.height, -0.05, { top: 0.07 })),
  'side-table': (k, p) => k.group(k.cyl(p.width / 2, 0.025, k.mat.oak, 0, p.height - 0.025, 0), k.cyl(0.018, p.height - 0.04, k.mat.black, 0, 0.015, 0, { segments: 12 }), k.cyl(0.15, 0.015, k.mat.black, 0, 0, 0)),
  'media-unit': (k, p) => k.group(k.soft(p.width, p.height - 0.1, p.depth, k.mat.oak, 0, 0.1, 0, 0.012), ...legs(k, p.width, p.depth, 0.1, k.mat.black, 0.08, 0.014),
    k.box(p.width - 0.04, p.height - 0.14, 0.012, k.mat.white, 0, 0.12, p.depth / 2 + 0.001),
    k.box(Math.min(1.25, p.width - 0.3), 0.72, 0.035, k.mat.screen, 0, p.height + 0.07, -p.depth / 2 + 0.11), k.box(0.3, 0.07, 0.16, k.mat.black, 0, p.height, -p.depth / 2 + 0.13)),
  bookshelf: (k, p) => {
    const W = p.width, D = p.depth, H = p.height, parts = [k.box(0.022, H, D, k.mat.oak, -W / 2 + 0.011, 0, 0), k.box(0.022, H, D, k.mat.oak, W / 2 - 0.011, 0, 0), k.box(W, 0.012, D, k.mat.oak, 0, H - 0.012, 0), k.box(W - 0.044, H, 0.008, k.mat.oak, 0, 0, -D / 2 + 0.004)];
    const rnd = sequence(seedOf(p)), shelves = Math.max(3, Math.round(H / 0.38)), tones = ['rose', 'sage', 'lilac', 'oat', 'slate', 'clay', 'sky', 'linen'];
    for (let i = 0; i < shelves; i++) {
      const y = 0.06 + i * (H - 0.08) / shelves;
      parts.push(k.box(W - 0.044, 0.02, D - 0.01, k.mat.oak, 0, y, 0));
      let x = -W / 2 + 0.04;
      const full = 0.45 + rnd() * 0.5;
      while (x < -W / 2 + 0.04 + (W - 0.1) * full) {
        const t = 0.022 + rnd() * 0.03, h = 0.19 + rnd() * 0.09;
        parts.push(k.box(t, h, 0.15 + rnd() * 0.05, k.cloth(tones[Math.floor(rnd() * tones.length)], 0.92 + rnd() * 0.12), x + t / 2, y + 0.02, -0.02));
        x += t + 0.002;
      }
    }
    return k.group(parts);
  },
  rug: (k, p) => { const m = k.box(p.width, 0.012, p.depth, k.rug(p.finish), 0, 0.002, 0); m.castShadow = false; return k.group(m); },
  'floor-lamp': (k, p) => k.group(k.cyl(0.15, 0.02, k.mat.black, 0, 0, 0), k.cyl(0.011, p.height - 0.3, k.mat.black, 0, 0.02, 0, { segments: 10 }), k.cyl(0.18, 0.27, k.mat.shade, 0, p.height - 0.29, 0, { top: 0.14, open: true })),
  plant: (k, p) => {
    const rnd = sequence(seedOf(p)), parts = [k.cyl(0.13, 0.32, k.mat.pot, 0, 0, 0, { top: 0.17 }), k.cyl(0.155, 0.01, k.mat.soil, 0, 0.3, 0), k.cyl(0.014, p.height * 0.5, k.mat.walnut, 0, 0.3, 0, { segments: 8 })];
    for (let i = 0; i < 11; i++) {
      const a = rnd() * 2 * Math.PI, r = 0.05 + rnd() * 0.17, y = 0.55 + rnd() * (p.height - 0.75);
      const leaf = k.ball(0.11 + rnd() * 0.07, i % 3 ? k.mat.leaf : k.mat.leafDark, r * Math.cos(a), y, r * Math.sin(a), [1, 0.5 + rnd() * 0.25, 0.75]);
      leaf.rotation.set(rnd() - 0.5, a, rnd() - 0.5);
      parts.push(leaf);
    }
    return k.group(parts);
  },
  'dining-6': (k, p) => diningSet(k, p, 3),
  'dining-4': (k, p) => diningSet(k, p, 2),
  chair: (k, p) => chair(k, p, k.cloth(p.finish, 1)),
  stool: (k, p) => k.group(k.cyl(p.width / 2, 0.045, k.mat.oak, 0, p.height - 0.045, 0), ...legs(k, p.width, p.width, p.height - 0.045, k.mat.black, 0.07, 0.012), k.cyl(p.width / 2 - 0.05, 0.012, k.mat.black, 0, 0.22, 0, { open: true })),
  counter: (k, p) => k.group(cabinets(k, p)),
  'counter-sink': (k, p) => {
    const tap = k.cyl(0.012, 0.26, k.mat.steel, -0.09, p.height, -0.24, { segments: 12 }), spout = k.cyl(0.01, 0.17, k.mat.steel, -0.09, p.height + 0.25, -0.16, { segments: 12 });
    spout.rotation.x = Math.PI / 2;
    spout.position.y = p.height + 0.255;
    return k.group(cabinets(k, p), k.box(0.44, 0.004, 0.38, k.mat.steel, -0.09, p.height, -0.01), k.box(0.4, 0.002, 0.34, k.mat.black, -0.09, p.height + 0.004, -0.01), tap, spout);
  },
  'counter-hob': (k, p) => {
    const parts = [...cabinets(k, p), k.box(0.6, 0.006, 0.5, k.mat.screen, 0, p.height, 0)];
    for (const [x, y, r] of [[-0.15, -0.12, 0.09], [0.15, -0.12, 0.075], [-0.15, 0.1, 0.075], [0.15, 0.1, 0.09]]) parts.push(k.cyl(r, 0.002, k.mat.black, x, p.height + 0.006, y));
    parts.push(k.box(Math.min(p.width, 0.7), 0.05, 0.46, k.mat.steel, 0, 1.6, -0.07), k.box(0.24, 0.55, 0.22, k.mat.steel, 0, 1.65, -p.depth / 2 + 0.12));
    return k.group(parts);
  },
  island: (k, p) => k.group(cabinets(k, p, { overhang: 0.26, front: k.mat.oak })),
  fridge: (k, p) => k.group(k.soft(p.width, p.height, p.depth - 0.04, k.mat.steel, 0, 0, -0.02, 0.012), k.box(p.width - 0.01, p.height * 0.62 - 0.01, 0.03, k.mat.steel, 0, p.height * 0.38, p.depth / 2 - 0.02),
    k.box(p.width - 0.01, p.height * 0.38 - 0.02, 0.03, k.mat.steel, 0, 0.01, p.depth / 2 - 0.02), k.cyl(0.009, 0.5, k.mat.black, -p.width / 2 + 0.06, p.height * 0.42, p.depth / 2 + 0.012, { segments: 8 }),
    k.cyl(0.009, 0.3, k.mat.black, -p.width / 2 + 0.06, p.height * 0.38 - 0.34, p.depth / 2 + 0.012, { segments: 8 })),
  'oven-tower': (k, p) => {
    const bar = k.cyl(0.008, p.width - 0.16, k.mat.steel, 0, 1.42, p.depth / 2 + 0.03, { segments: 8 });
    bar.rotation.z = Math.PI / 2;
    bar.position.y = 1.42;
    return k.group(k.box(p.width, p.height, p.depth - 0.03, k.mat.cabinet, 0, 0, -0.015), k.box(p.width - 0.03, 0.56, 0.02, k.mat.screen, 0, 0.84, p.depth / 2 - 0.012), bar, k.box(p.width - 0.01, 0.72, 0.018, k.mat.cabinet, 0, 0.1, p.depth / 2 - 0.012));
  },
  'bed-double': (k, p) => bed(k, p, 2),
  'bed-single': (k, p) => bed(k, p, 1),
  bedside: (k, p) => k.group(k.soft(p.width, p.height - 0.12, p.depth, k.mat.oak, 0, 0.12, 0, 0.012), ...legs(k, p.width, p.depth, 0.12, k.mat.oak, 0.04, 0.014), k.box(p.width - 0.03, 0.14, 0.01, k.mat.white, 0, p.height - 0.2, p.depth / 2 + 0.002),
    ...tableLamp(k, 0, p.height, -0.03)),
  wardrobe: (k, p) => {
    const n = Math.max(2, Math.round(p.width / 0.6)), span = p.width / n, parts = [k.box(p.width, p.height, p.depth - 0.02, k.mat.white, 0, 0, -0.01)];
    for (let i = 0; i < n; i++) {
      const x = -p.width / 2 + (i + 0.5) * span;
      parts.push(k.box(span - 0.006, p.height - 0.1, 0.02, k.mat.white, x, 0.08, p.depth / 2 - 0.01), k.cyl(0.006, 0.9, k.mat.brass, x + (i % 2 ? -1 : 1) * (span / 2 - 0.04), 0.75, p.depth / 2 + 0.012, { segments: 8 }));
    }
    return k.group(parts);
  },
  desk: (k, p) => {
    const c = chair(k, { width: 0.46 }, k.cloth(p.finish, 1));
    c.position.set(0.05, 0, -(p.depth / 2 + 0.2));
    c.rotation.y = Math.PI;
    return k.group(k.soft(p.width, 0.03, p.depth, k.mat.oak, 0, p.height - 0.03, 0, 0.01), ...legs(k, p.width, p.depth, p.height - 0.03, k.mat.black, 0.05, 0.014), k.box(0.4, 0.13, p.depth - 0.1, k.mat.white, p.width / 2 - 0.26, p.height - 0.16, 0), c,
      ...tableLamp(k, -p.width / 2 + 0.16, p.height, -p.depth / 2 + 0.16), k.box(0.33, 0.012, 0.23, k.mat.steel, 0.02, p.height, 0.02));
  },
  dresser: (k, p) => {
    const parts = [k.soft(p.width, p.height - 0.1, p.depth, k.mat.oak, 0, 0.1, 0, 0.012), ...legs(k, p.width, p.depth, 0.1, k.mat.oak, 0.05, 0.016)];
    for (let r = 0; r < 3; r++) for (const s of [-1, 1]) {
      parts.push(k.box(p.width / 2 - 0.02, (p.height - 0.14) / 3 - 0.012, 0.012, k.mat.oak, s * p.width / 4, 0.115 + r * (p.height - 0.13) / 3, p.depth / 2 + 0.003), k.ball(0.012, k.mat.brass, s * p.width / 4, 0.115 + (r + 0.5) * (p.height - 0.13) / 3, p.depth / 2 + 0.018));
    }
    return k.group(parts);
  },
  toilet: (k, p) => {
    const seat = k.cyl(0.19, 0.03, k.mat.ceramic, 0, 0.4, 0.07);
    seat.scale.set(0.93, 1, 1.32);
    return k.group(k.soft(p.width, 0.4, 0.17, k.mat.ceramic, 0, 0.4, -p.depth / 2 + 0.085, 0.03), k.soft(0.3, 0.4, 0.46, k.mat.ceramic, 0, 0, 0.06, 0.11), seat, k.box(0.05, 0.012, 0.03, k.mat.steel, 0, 0.8, -p.depth / 2 + 0.085));
  },
  basin: (k, p) => {
    const bowl = k.cyl(0.17, 0.1, k.mat.ceramic, 0, 0.84, 0.02, { top: 0.21 }), spout = k.cyl(0.009, 0.11, k.mat.brass, 0, 1.0, -0.12, { segments: 10 });
    spout.rotation.x = Math.PI / 2;
    spout.position.y = 1.03;
    return k.group(k.soft(p.width, 0.46, p.depth, k.mat.oak, 0, 0.36, 0, 0.012), k.soft(p.width + 0.01, 0.025, p.depth + 0.005, k.mat.stone, 0, 0.82, 0, 0.006), bowl, k.cyl(0.011, 0.19, k.mat.brass, 0, 0.845, -0.17, { segments: 10 }), spout,
      k.box(p.width - 0.08, 0.75, 0.018, k.mat.mirror, 0, 1.12, -p.depth / 2 + 0.01));
  },
  bath: (k, p) => {
    const W = p.width, D = p.depth, H = p.height, t = 0.07;
    return k.group(k.soft(W, H, t, k.mat.ceramic, 0, 0, -D / 2 + t / 2, 0.025), k.soft(W, H, t, k.mat.ceramic, 0, 0, D / 2 - t / 2, 0.025), k.soft(t, H, D, k.mat.ceramic, -W / 2 + t / 2, 0, 0, 0.025), k.soft(t, H, D, k.mat.ceramic, W / 2 - t / 2, 0, 0, 0.025),
      k.box(W - 0.06, 0.1, D - 0.06, k.mat.ceramic, 0, 0.02, 0), k.cyl(0.012, 0.16, k.mat.brass, W / 2 - 0.035, H, 0, { segments: 10 }));
  },
  shower: (k, p) => {
    const W = p.width, D = p.depth, H = p.height, arm = k.cyl(0.009, 0.3, k.mat.steel, 0, H - 0.05, -D / 2 + 0.17, { segments: 8 });
    arm.rotation.x = Math.PI / 2;
    arm.position.y = H - 0.02;
    const front = k.box(W, H - 0.05, 0.01, k.mat.glass, 0, 0.05, D / 2 - 0.005), side = k.box(0.01, H - 0.05, D, k.mat.glass, W / 2 - 0.005, 0.05, 0);
    front.castShadow = side.castShadow = false;
    return k.group(k.soft(W, 0.05, D, k.mat.ceramic, 0, 0, 0, 0.01), front, side, arm, k.cyl(0.1, 0.012, k.mat.steel, 0, H - 0.05, -D / 2 + 0.32), k.cyl(0.03, 0.004, k.mat.steel, 0, 0.05, 0));
  },
  washer: (k, p) => {
    const door = k.cyl(0.19, 0.03, k.mat.screen, 0, 0, 0);
    door.rotation.x = Math.PI / 2;
    door.position.set(0, p.height * 0.48, -(p.depth / 2 + 0.005));
    return k.group(k.soft(p.width, p.height, p.depth - 0.02, k.mat.white, 0, 0, -0.01, 0.015), door, k.box(p.width - 0.06, 0.07, 0.008, k.mat.cabinet, 0, p.height - 0.12, p.depth / 2 - 0.003));
  },
  console: (k, p) => k.group(k.soft(p.width, 0.03, p.depth, k.mat.oak, 0, p.height - 0.03, 0, 0.01), ...legs(k, p.width, p.depth, p.height - 0.03, k.mat.black, 0.04, 0.012), k.cyl(0.06, 0.2, k.mat.ceramic, p.width / 4, p.height, 0, { top: 0.045 })),
  picture: (k, p) => {
    const lift = p.lift ?? 1.5, canvas = k.box(p.width - 0.05, p.height - 0.05, 0.006, k.painting(seedOf(p)), 0, lift - p.height / 2 + 0.025, p.depth / 2 - 0.004);
    canvas.castShadow = false;
    return k.group(k.box(p.width, p.height, p.depth - 0.012, k.mat.oak, 0, lift - p.height / 2, -0.006), canvas);
  },
  pendant: (k, p, { ceiling = 2.7 } = {}) => {
    const drop = Math.max(0.3, ceiling - 1.75 - p.height), top = ceiling;
    const shade = new k.THREE.Mesh(new k.THREE.SphereGeometry(p.width / 2, 28, 14, 0, 2 * Math.PI, 0, Math.PI / 2), k.mat.shade);
    shade.position.set(0, top - drop - p.height * 0.55, 0);
    return k.group(k.cyl(0.004, drop, k.mat.black, 0, top - drop, 0, { segments: 6 }), k.cyl(0.045, 0.02, k.mat.brass, 0, top - 0.02, 0), shade, k.ball(0.045, k.mat.lamp, 0, top - drop - p.height * 0.5, 0));
  },
  car: (k, p) => {
    const W = p.width, L = p.depth, parts = [k.soft(W, 0.55, L, k.mat.paint, 0, 0.28, 0, 0.14), k.soft(W - 0.16, 0.5, L * 0.5, k.mat.screen, 0, 0.78, -L * 0.04, 0.16)];
    for (const sx of [-1, 1]) for (const sy of [-1, 1]) { const wheel = k.cyl(0.33, 0.22, k.mat.rubber, 0, 0, 0); wheel.rotation.z = Math.PI / 2; wheel.position.set(sx * (W / 2 - 0.11), 0.33, -sy * (L / 2 - 0.85)); parts.push(wheel); }
    return k.group(parts);
  },
};

// ---- what stands outdoors (ADR 0097) -------------------------------------------------------------------------------------------
// A tree: a trunk that tapers, a few boughs, and a crown of soft masses drawn from the piece's own sequence, so that each tree is
// its own and the same whenever it is drawn.
function tree(k, p, { masses = 15 } = {}) {
  const rnd = sequence(seedOf(p)), R = p.width / 2, H = p.height, clear = Math.max(1.2, H * 0.36), parts = [];
  parts.push(k.cyl(Math.max(0.07, R * 0.07), clear + H * 0.12, k.mat.bark, 0, 0, 0, { top: Math.max(0.04, R * 0.04), segments: 10 }));
  for (let i = 0; i < 3; i++) {
    const a = rnd() * 2 * Math.PI, bough = k.cyl(Math.max(0.03, R * 0.03), R * 0.55, k.mat.bark, 0, 0, 0, { top: 0.015, segments: 6 });
    bough.position.set(Math.cos(a) * R * 0.16, clear + R * 0.18, -Math.sin(a) * R * 0.16);
    bough.rotation.set(Math.sin(a) * 0.75, 0, -Math.cos(a) * 0.75);
    parts.push(bough);
  }
  const mid = clear + (H - clear) * 0.52, tall = (H - clear) / 2;
  for (let i = 0; i < masses; i++) {
    // Masses spread over an ellipsoid, fuller at the middle, a little flat beneath.
    const a = rnd() * 2 * Math.PI, u = rnd() * 2 - 1, q = Math.sqrt(1 - u * u) * (0.35 + 0.5 * rnd()), r = R * (0.3 + 0.2 * rnd());
    const y = mid + u * tall * 0.62;
    const m = k.ball(r, [k.mat.leaf, k.mat.leafDark, k.mat.leafLight][i % 3], q * R * 0.72 * Math.cos(a), Math.max(clear + r * 0.5, Math.min(H - r * 0.8, y)), q * R * 0.72 * Math.sin(a), [1, 0.78 + rnd() * 0.2, 1]);
    m.rotation.y = rnd() * 3;
    parts.push(m);
  }
  return k.group(parts);
}
function shrub(k, p) {
  const rnd = sequence(seedOf(p)), R = p.width / 2, parts = [];
  for (let i = 0; i < 6; i++) { const a = rnd() * 2 * Math.PI, q = rnd() * 0.45, r = R * (0.45 + 0.2 * rnd()); parts.push(k.ball(r, i % 2 ? k.mat.leaf : k.mat.leafDark, q * R * Math.cos(a), Math.min(p.height - r * 0.8, r * 0.75 + rnd() * p.height * 0.3), q * R * Math.sin(a), [1, 0.85, 1])); }
  return k.group(parts);
}
Object.assign(BUILDERS, {
  tree: (k, p) => tree(k, p),
  'tree-small': (k, p) => tree(k, p, { masses: 11 }),
  shrub: (k, p) => shrub(k, p),
  hedge: (k, p) => k.group(k.soft(p.width, p.height, p.depth, k.mat.hedge, 0, 0, 0, Math.min(0.14, p.depth / 3))),
  planter: (k, p) => {
    const parts = [k.soft(p.width, 0.42, p.depth, k.mat.planter, 0, 0, 0, 0.02), k.box(p.width - 0.06, 0.01, p.depth - 0.06, k.mat.soil, 0, 0.415, 0)], n = Math.max(1, Math.round(p.width / 0.45)), rnd = sequence(seedOf(p));
    for (let i = 0; i < n; i++) parts.push(k.ball(Math.min(0.24, p.depth / 2) * (0.9 + 0.3 * rnd()), i % 2 ? k.mat.leaf : k.mat.leafLight, -p.width / 2 + (i + 0.5) * p.width / n, 0.42 + (p.height - 0.42) * 0.45, 0, [1, (p.height - 0.42) / 0.36, 1]));
    return k.group(parts);
  },
  bench: (k, p) => {
    const parts = [], W = p.width, D = p.depth, seat = 0.44;
    for (let i = 0; i < 4; i++) parts.push(k.soft(W, 0.03, (D - 0.14) / 4 - 0.012, k.mat.teak, 0, seat - 0.03, -D / 2 + 0.14 + (i + 0.5) * (D - 0.14) / 4, 0.008));
    for (let i = 0; i < 3; i++) parts.push(k.soft(W, 0.075, 0.025, k.mat.teak, 0, seat + 0.1 + i * 0.105, -D / 2 + 0.05 - i * 0.012, 0.008));
    for (const sx of [-1, 1]) parts.push(k.box(0.045, seat - 0.03, D - 0.08, k.mat.black, sx * (W / 2 - 0.12), 0, 0), k.box(0.045, p.height - seat + 0.03, 0.045, k.mat.black, sx * (W / 2 - 0.12), seat - 0.03, -D / 2 + 0.05));
    return k.group(parts);
  },
  'outdoor-table': (k, p) => diningSet(k, p, 2),
  'lamp-post': (k, p) => k.group(k.cyl(0.085, 0.5, k.mat.black, 0, 0, 0, { top: 0.05, segments: 14 }), k.cyl(0.035, p.height - 0.75, k.mat.black, 0, 0.5, 0, { segments: 10 }),
    k.cyl(0.11, 0.26, k.mat.shade, 0, p.height - 0.3, 0, { top: 0.15, segments: 14 }), k.cyl(0.18, 0.03, k.mat.black, 0, p.height - 0.04, 0, { top: 0.03, segments: 14 })),
});

// A piece in the round, in its own frame: the catalogue's model of its kind, or a plain block of its size.
export function buildPiece(k, p, more = {}) {
  const make = BUILDERS[p.kind];
  const g = make ? make(k, p, more) : k.group(k.soft(p.width, p.height, p.depth, k.mat.white, 0, 0, 0, 0.02));
  g.userData.kind = p.kind;
  return g;
}

export const MODELLED = Object.keys(BUILDERS);
