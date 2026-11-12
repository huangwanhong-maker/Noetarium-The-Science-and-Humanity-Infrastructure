// The stuff a drawn building is made of, for its picture in the round and from within (ADR 0096): textures drawn here on a canvas
// (boards, tiles, a woven ground, a painted picture), materials by name, and the small set of solids its furniture is built from.
// three.js is given as an argument; a canvas is needed only when a texture is first asked for.
//
// Nothing here is a photograph or a borrowed asset: every texture is drawn from a seeded sequence, so a picture can be made again.

import { FINISHES } from './catalogue.js';
import { CLADDINGS, ROOF_COVERS, WALL_TONES } from './exterior.js';

// A sequence of numbers in [0, 1) from a seed (mulberry32).
export function sequence(seed) {
  let a = seed >>> 0;
  return () => {
    a = (a + 0x6d2b79f5) >>> 0;
    let t = a;
    t = Math.imul(t ^ (t >>> 15), t | 1);
    t ^= t + Math.imul(t ^ (t >>> 7), t | 61);
    return ((t ^ (t >>> 14)) >>> 0) / 4294967296;
  };
}

const hex = (h) => [1, 3, 5].map((i) => parseInt(h.slice(i, i + 2), 16));
const css = ([r, g, b]) => `rgb(${Math.round(r)},${Math.round(g)},${Math.round(b)})`;
const mix = (a, b, t) => a.map((v, i) => v + (b[i] - v) * t);
const shade = (c, k) => c.map((v) => Math.max(0, Math.min(255, v * k)));

function canvas(size) {
  const c = document.createElement('canvas');
  c.width = c.height = size;
  return c;
}

// Boards laid in courses along x: each board its own tone, a few lines of grain, a dark joint.
function boards(base, rnd, size = 1024, courses = 16) {
  const c = canvas(size), g = c.getContext('2d'), h = size / courses, tone = hex(base);
  for (let row = 0; row < courses; row++) {
    let x = -rnd() * size * 0.4;
    while (x < size) {
      const len = size * (0.3 + rnd() * 0.3), k = 0.94 + rnd() * 0.12;
      g.fillStyle = css(shade(tone, k));
      g.fillRect(x, row * h, len, h);
      g.strokeStyle = css(shade(tone, k * 0.93));
      g.lineWidth = 1;
      for (let i = 0; i < 5; i++) { const y = row * h + rnd() * h; g.globalAlpha = 0.35; g.beginPath(); g.moveTo(x, y); g.bezierCurveTo(x + len * 0.3, y + (rnd() - 0.5) * 5, x + len * 0.7, y + (rnd() - 0.5) * 5, x + len, y); g.stroke(); }
      g.globalAlpha = 1;
      g.fillStyle = css(shade(tone, 0.72));
      g.fillRect(x + len - 1, row * h, 1.5, h);
      x += len;
    }
    g.fillStyle = css(shade(tone, 0.74));
    g.fillRect(0, row * h, size, 1.2);
  }
  return c;
}

// Square tiles with their joints, each a little apart in tone.
function tiles(base, joint, rnd, size = 512, n = 4) {
  const c = canvas(size), g = c.getContext('2d'), s = size / n, tone = hex(base);
  g.fillStyle = joint;
  g.fillRect(0, 0, size, size);
  for (let i = 0; i < n; i++) for (let j = 0; j < n; j++) { g.fillStyle = css(shade(tone, 0.97 + rnd() * 0.06)); g.fillRect(i * s + 1.5, j * s + 1.5, s - 3, s - 3); }
  return c;
}

// A ground of fine grain: plaster, carpet, concrete.
function grain(base, rnd, amount = 0.05, size = 256) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), img = g.createImageData(size, size);
  for (let i = 0; i < size * size; i++) { const k = 1 - amount / 2 + rnd() * amount; img.data[4 * i] = tone[0] * k; img.data[4 * i + 1] = tone[1] * k; img.data[4 * i + 2] = tone[2] * k; img.data[4 * i + 3] = 255; }
  g.putImageData(img, 0, 0);
  return c;
}

// A rug: a woven ground with a border and a line within it.
function rug(base, rnd, size = 512) {
  const c = grain(base, rnd, 0.12, size), g = c.getContext('2d'), tone = hex(base);
  g.strokeStyle = css(shade(tone, 0.78)); g.lineWidth = size * 0.035; g.strokeRect(size * 0.045, size * 0.045, size * 0.91, size * 0.91);
  g.strokeStyle = css(mix(tone, [255, 255, 255], 0.5)); g.lineWidth = size * 0.008; g.strokeRect(size * 0.1, size * 0.1, size * 0.8, size * 0.8);
  return c;
}

// A picture for a wall: a few soft fields of colour, laid over one another.
function painting(rnd, size = 384) {
  const c = canvas(size), g = c.getContext('2d'), palette = ['#e9dfd3', '#d6a3ad', '#b3a7d1', '#a3b39a', '#cbb89b', '#8d6f8f', '#f3ece4'];
  g.fillStyle = palette[0]; g.fillRect(0, 0, size, size);
  for (let i = 0; i < 9; i++) {
    const x = rnd() * size, y = rnd() * size, r = size * (0.12 + rnd() * 0.3), grad = g.createRadialGradient(x, y, 0, x, y, r), col = hex(palette[1 + Math.floor(rnd() * 6)]);
    grad.addColorStop(0, `rgba(${col[0]},${col[1]},${col[2]},0.85)`); grad.addColorStop(1, `rgba(${col[0]},${col[1]},${col[2]},0)`);
    g.fillStyle = grad; g.fillRect(0, 0, size, size);
  }
  return c;
}

const toHex = (c) => `#${c.map((v) => Math.round(v).toString(16).padStart(2, '0')).join('')}`;

// ---- the outside (ADR 0097) ----------------------------------------------------------------------------------------------------
// Each is drawn in the tone it is given, on a square canvas that stands for the stated metres along a wall (or an eave) and up it.

// Bricks in running bond: eight to a course, sixteen courses; each brick a little its own tone, the joints paler.
function brick(base, rnd, size = 1024) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), w = size / 8, h = size / 16, joint = Math.max(2, size * 0.005);
  g.fillStyle = css(mix(tone, [236, 230, 222], 0.5)); g.fillRect(0, 0, size, size);
  for (let row = 0; row < 16; row++) for (let i = -1; i < 8; i++) {
    const x = (i + (row % 2) * 0.5) * w, k = 0.95 + rnd() * 0.1;
    g.fillStyle = css(shade(tone, k));
    g.fillRect(x + joint / 2, row * h + joint / 2, w - joint, h - joint);
  }
  return c;
}

// Boards lapped one over the next: each a little apart in tone, a shadow beneath its lower edge.
function lapped(base, rnd, size = 512, courses = 4) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), h = size / courses;
  for (let row = 0; row < courses; row++) {
    const k = 0.97 + rnd() * 0.06, grad = g.createLinearGradient(0, row * h, 0, (row + 1) * h);
    grad.addColorStop(0, css(shade(tone, k * 0.8))); grad.addColorStop(0.1, css(shade(tone, k * 0.96))); grad.addColorStop(0.3, css(shade(tone, k))); grad.addColorStop(1, css(shade(tone, k * 1.03)));
    g.fillStyle = grad; g.fillRect(0, row * h, size, h);
    g.strokeStyle = css(shade(tone, 0.92)); g.globalAlpha = 0.3; g.lineWidth = 1;
    for (let i = 0; i < 4; i++) { const y = row * h + h * (0.25 + rnd() * 0.7); g.beginPath(); g.moveTo(0, y); g.bezierCurveTo(size * 0.3, y + (rnd() - 0.5) * 4, size * 0.7, y + (rnd() - 0.5) * 4, size, y); g.stroke(); }
    g.globalAlpha = 1;
  }
  return c;
}

// Battens standing side by side, a dark gap between them.
function battens(base, rnd, size = 512, n = 8) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), w = size / n;
  g.fillStyle = css(shade(tone, 0.45)); g.fillRect(0, 0, size, size);
  for (let i = 0; i < n; i++) {
    const k = 0.93 + rnd() * 0.14, grad = g.createLinearGradient(i * w, 0, (i + 1) * w, 0);
    grad.addColorStop(0, css(shade(tone, k * 1.04))); grad.addColorStop(0.7, css(shade(tone, k))); grad.addColorStop(1, css(shade(tone, k * 0.86)));
    g.fillStyle = grad; g.fillRect(i * w + w * 0.09, 0, w * 0.82, size);
  }
  return c;
}

// Stone laid in courses of three heights, the stones of each course of their own lengths.
function ashlar(base, rnd, size = 512) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), heights = [0.28, 0.2, 0.3, 0.22], joint = Math.max(2, size * 0.007);
  g.fillStyle = css(shade(tone, 0.78)); g.fillRect(0, 0, size, size);
  let y = 0;
  for (const share of heights) {
    const h = share * size, n = 2 + Math.floor(rnd() * 2), cuts = [0];
    for (let i = 1; i < n; i++) cuts.push((i + (rnd() - 0.5) * 0.5) / n * size);
    cuts.push(size);
    for (let i = 0; i < n; i++) { g.fillStyle = css(shade(tone, 0.92 + rnd() * 0.16)); g.fillRect(cuts[i] + joint / 2, y + joint / 2, cuts[i + 1] - cuts[i] - joint, h - joint); }
    y += h;
  }
  const img = g.getImageData(0, 0, size, size);
  for (let i = 0; i < size * size; i++) { const k = 0.97 + rnd() * 0.06; img.data[4 * i] *= k; img.data[4 * i + 1] *= k; img.data[4 * i + 2] *= k; }
  g.putImageData(img, 0, 0);
  return c;
}

// A roof's courses: `across` to a course and `up` courses, each piece a little apart in tone, a shadow under its lower edge; with
// `round`, each is shaded across as a tile that curves is.
function courses(base, rnd, { size = 512, across = 4, up = 4, vary = 0.12, round = false, stagger = 0.5 } = {}) {
  const c = canvas(size), g = c.getContext('2d'), tone = hex(base), w = size / across, h = size / up;
  for (let row = 0; row < up; row++) for (let i = -1; i < across; i++) {
    const x = (i + (row % 2) * stagger) * w, k = 1 - vary / 2 + rnd() * vary;
    if (round) {
      const grad = g.createLinearGradient(x, 0, x + w, 0);
      grad.addColorStop(0, css(shade(tone, k * 0.8))); grad.addColorStop(0.35, css(shade(tone, k * 1.06))); grad.addColorStop(0.8, css(shade(tone, k * 0.96))); grad.addColorStop(1, css(shade(tone, k * 0.74)));
      g.fillStyle = grad;
    } else g.fillStyle = css(shade(tone, k));
    g.fillRect(x, row * h, w + 0.5, h);
    if (!round) { g.fillStyle = css(shade(tone, 0.7)); g.fillRect(x, row * h, 1.2, h); }
  }
  // The texture's foot is the canvas's bottom: each course's lower edge lies over the course below, and shades it.
  for (let row = 0; row < up; row++) {
    const grad = g.createLinearGradient(0, (row + 1) * h - h * 0.16, 0, (row + 1) * h);
    grad.addColorStop(0, 'rgba(0,0,0,0)'); grad.addColorStop(1, 'rgba(0,0,0,0.3)');
    g.fillStyle = grad; g.fillRect(0, (row + 1) * h - h * 0.16, size, h * 0.16);
  }
  return c;
}

// Sheets of metal with seams standing between them.
function seams(base, rnd, size = 512, n = 2) {
  const c = grain(base, rnd, 0.035, size), g = c.getContext('2d'), tone = hex(base), w = size / n;
  for (let i = 0; i < n; i++) { g.fillStyle = css(shade(tone, 1.16)); g.fillRect(i * w, 0, size * 0.012, size); g.fillStyle = css(shade(tone, 0.66)); g.fillRect(i * w + size * 0.012, 0, size * 0.014, size); }
  return c;
}

// The kit: materials by name, cloths by finish, and the solids pieces are built from. Lengths in metres; a solid is placed by the
// middle of its base in a piece's frame (lx to its right, ly toward its front, y up), which three.js sees as (lx, y, −ly).
export function makeKit(THREE, { RoundedBoxGeometry = null, night = false } = {}) {
  const cache = new Map(), textures = new Map();
  // A texture by name: `metres` is what the canvas stands for, one number or [along, up].
  const texture = (name, make, metres) => {
    if (!textures.has(name)) {
      const t = new THREE.CanvasTexture(make());
      t.wrapS = t.wrapT = THREE.RepeatWrapping;
      t.colorSpace = THREE.SRGBColorSpace;
      t.anisotropy = 8;
      const [mx, my] = Array.isArray(metres) ? metres : [metres, metres];
      t.repeat.set(1 / mx, 1 / my);
      textures.set(name, t);
    }
    return textures.get(name);
  };
  const standard = (name, props) => { if (!cache.has(name)) cache.set(name, new THREE.MeshStandardMaterial({ roughness: 0.8, metalness: 0, ...props() })); return cache.get(name); };
  const mat = {
    get wall() { return standard('wall', () => ({ color: 0xf1ebe4, roughness: 0.94 })); },
    get outside() { return standard('outside', () => ({ color: night ? 0xcbbcc9 : 0xe9e0db, roughness: 0.96 })); },
    get ceiling() { return standard('ceiling', () => ({ color: 0xf6f3ee, roughness: 0.97 })); },
    get trim() { return standard('trim', () => ({ color: 0xfbf9f6, roughness: 0.55 })); },
    get soffit() { return standard('soffit', () => ({ color: night ? 0xbdb3b6 : 0xf3efea, roughness: 0.85, side: THREE.BackSide })); },
    get fascia() { return standard('fascia', () => ({ color: night ? 0xcfc6c8 : 0xfaf7f3, roughness: 0.6, side: THREE.DoubleSide })); },
    get glass() { return standard('glass', () => ({ color: 0xdbe9ee, roughness: 0.03, metalness: 0, transparent: true, opacity: 0.1, depthWrite: false, envMapIntensity: 0.35 })); },
    get oak() { return standard('oak', () => ({ color: 0xffffff, map: texture('oak', () => boards('#cfa97e', sequence(11), 512, 6), 1.2), roughness: 0.62 })); },
    get walnut() { return standard('walnut', () => ({ color: 0xffffff, map: texture('walnut', () => boards('#7b5b45', sequence(12), 512, 6), 1.2), roughness: 0.6 })); },
    get white() { return standard('white', () => ({ color: 0xf6f3ef, roughness: 0.45 })); },
    get cabinet() { return standard('cabinet', () => ({ color: 0xe9e4dc, roughness: 0.5 })); },
    get stone() { return standard('stone', () => ({ color: 0xffffff, map: texture('stone', () => grain('#e7e2da', sequence(13), 0.07), 0.7), roughness: 0.35 })); },
    get ceramic() { return standard('ceramic', () => ({ color: 0xfbfaf7, roughness: 0.16 })); },
    get steel() { return standard('steel', () => ({ color: 0xc8cbcf, roughness: 0.32, metalness: 0.85 })); },
    get brass() { return standard('brass', () => ({ color: 0xb99a5f, roughness: 0.36, metalness: 0.85 })); },
    get black() { return standard('black', () => ({ color: 0x2d282c, roughness: 0.5, metalness: 0.3 })); },
    get screen() { return standard('screen', () => ({ color: 0x15131a, roughness: 0.12, metalness: 0.4 })); },
    get mirror() { return standard('mirror', () => ({ color: 0xe9eef0, roughness: 0.03, metalness: 1 })); },
    get bedding() { return standard('bedding', () => ({ color: 0xffffff, map: texture('bedding', () => grain('#f3efe9', sequence(14), 0.05), 0.4), roughness: 0.95 })); },
    get leaf() { return standard('leaf', () => ({ color: 0x77946a, roughness: 0.75 })); },
    get leafDark() { return standard('leafDark', () => ({ color: 0x5c7a55, roughness: 0.75 })); },
    get leafLight() { return standard('leafLight', () => ({ color: night ? 0x71866a : 0x93ab7f, roughness: 0.8 })); },
    get hedge() { return standard('hedge', () => ({ color: 0xffffff, map: texture('hedge', () => grain(night ? '#4d6149' : '#6f8a62', sequence(41), 0.3), 0.6), roughness: 1 })); },
    get bark() { return standard('bark', () => ({ color: 0x6f5a4c, roughness: 0.95 })); },
    get teak() { return standard('teak', () => ({ color: 0xffffff, map: texture('teak', () => boards('#b08a63', sequence(42), 512, 6), 1.0), roughness: 0.7 })); },
    get planter() { return standard('planter', () => ({ color: 0x8b8580, roughness: 0.9 })); },
    get pot() { return standard('pot', () => ({ color: 0xc38f78, roughness: 0.85 })); },
    get soil() { return standard('soil', () => ({ color: 0x4d3b33, roughness: 1 })); },
    get rubber() { return standard('rubber', () => ({ color: 0x242226, roughness: 0.9 })); },
    get paint() { return standard('paint', () => ({ color: 0xbfc7cf, roughness: 0.3, metalness: 0.5 })); },
    get shade() { return standard('shade', () => ({ color: 0xf3e9da, emissive: 0xffdca8, emissiveIntensity: 0.42, roughness: 0.9, side: THREE.DoubleSide })); },
    get lamp() { return standard('lamp', () => ({ color: 0xffffff, emissive: 0xfff0d8, emissiveIntensity: 1.6, roughness: 0.6 })); },
    get step() { return standard('step', () => ({ color: 0xffffff, map: texture('oak', () => boards('#cfa97e', sequence(11), 512, 6), 1.2), roughness: 0.6 })); },
  };
  const floors = {
    boards: () => standard('f-boards', () => ({ color: 0xffffff, map: texture('boards', () => boards('#c6a077', sequence(1)), 2.4), roughness: 0.55 })),
    tiles: () => standard('f-tiles', () => ({ color: 0xffffff, map: texture('tiles', () => tiles('#e4e0da', '#c9c3bb', sequence(2), 512, 2), 1.2), roughness: 0.4 })),
    small: () => standard('f-small', () => ({ color: 0xffffff, map: texture('small', () => tiles('#e3e8ea', '#c2cbcf', sequence(3), 512, 4), 1.2), roughness: 0.3 })),
    carpet: () => standard('f-carpet', () => ({ color: 0xffffff, map: texture('carpet', () => grain('#d9d0c6', sequence(4), 0.16), 0.5), roughness: 1 })),
    concrete: () => standard('f-concrete', () => ({ color: 0xffffff, map: texture('concrete', () => grain('#c9c6c2', sequence(5), 0.08), 1.5), roughness: 0.9 })),
    pavers: () => standard('f-pavers', () => ({ color: 0xffffff, map: texture('pavers', () => tiles('#d9d2c8', '#b9b0a4', sequence(6), 512, 2), 1.0), roughness: 0.85 })),
    grass: () => standard('f-grass', () => ({ color: 0xffffff, map: texture('grass', () => grain(night ? '#3c4a3a' : '#b9cba6', sequence(7), 0.18), 0.8), roughness: 1 })),
  };
  const FLOOR_OF = { kitchen: 'tiles', bathroom: 'small', ensuite: 'small', toilet: 'small', laundry: 'small', bedroom: 'carpet', study: 'carpet', garage: 'concrete', carport: 'concrete',
    court: 'pavers', terrace: 'pavers', verandah: 'pavers', balcony: 'pavers', garden: 'grass' };

  const shadows = (m) => { m.castShadow = true; m.receiveShadow = true; return m; };
  const kit = {
    THREE, mat, night,
    floor: (kind) => floors[FLOOR_OF[kind] || 'boards'](),
    // An outer wall's stuff: a cladding in a tone, mapped in metres along the wall and up it (ADR 0097).
    cladding(id = 'render', toneName = 'chalk') {
      const kind = CLADDINGS[id] ? id : 'render', base = WALL_TONES[toneName] || WALL_TONES.chalk, rgb = night ? toHex(shade(hex(base), 0.86)) : base, key = `clad-${kind}-${toneName}`;
      const [make, metres] = { render: [() => grain(rgb, sequence(51), 0.045), 1.4], brick: [() => brick(rgb, sequence(52)), [1.92, 1.376]], boards: [() => lapped(rgb, sequence(53)), [2.2, 0.68]],
        battens: [() => battens(rgb, sequence(54)), [0.88, 2.0]], stone: [() => ashlar(rgb, sequence(55)), [1.5, 1.0]] }[kind];
      return standard(key, () => ({ color: 0xffffff, map: texture(key, make, metres), roughness: kind === 'render' ? 0.96 : 0.88 }));
    },
    // A roof's cover, mapped in metres along the eave and up the slope.
    roofing(id = 'tiles') {
      const kind = ROOF_COVERS[id] ? id : 'tiles', base = ROOF_COVERS[kind].tone, rgb = night ? toHex(shade(hex(base), 0.8)) : base, key = `roof-${kind}`;
      return standard(key, () => {
        const [make, metres, rough, metal] = { tiles: [() => courses(rgb, sequence(61), { round: true, vary: 0.16 }), [1.2, 1.32], 0.8, 0],
          slate: [() => courses(rgb, sequence(62), { across: 4, up: 6, vary: 0.14 }), [1.0, 1.32], 0.6, 0.05], shingle: [() => courses(rgb, sequence(63), { across: 8, up: 6, vary: 0.3 }), [1.12, 1.08], 0.95, 0],
          metal: [() => seams(rgb, sequence(64)), [0.9, 0.9], 0.42, 0.55], membrane: [() => grain(rgb, sequence(65), 0.05), 1.5, 0.9, 0] }[kind];
        return { color: 0xffffff, map: texture(key, make, metres), roughness: rough, metalness: metal };
      });
    },
    // A cloth by its finish's name, a little lighter or darker by k.
    cloth(name = 'linen', k = 1) {
      const key = `cloth-${name}-${k}`;
      return standard(key, () => ({ color: new THREE.Color(...shade(hex(FINISHES[name] || FINISHES.linen), k).map((v) => v / 255)).convertSRGBToLinear(), map: texture('weave', () => grain('#ffffff', sequence(21), 0.12), 0.25), roughness: 0.98 }));
    },
    rug(name = 'oat') { return standard(`rug-${name}`, () => { const t = new THREE.CanvasTexture(rug(FINISHES[name] || FINISHES.oat, sequence(31))); t.colorSpace = THREE.SRGBColorSpace; return { color: 0xffffff, map: t, roughness: 1 }; }); },
    painting(seed) { const t = new THREE.CanvasTexture(painting(sequence(seed))); t.colorSpace = THREE.SRGBColorSpace; return new THREE.MeshStandardMaterial({ map: t, roughness: 0.9 }); },
    box(w, h, d, material, lx = 0, y = 0, ly = 0) {
      const m = shadows(new THREE.Mesh(new THREE.BoxGeometry(w, h, d), material));
      m.position.set(lx, y + h / 2, -ly);
      return m;
    },
    // A box with its edges eased, where the geometry for it was given; r is the radius of the easing.
    soft(w, h, d, material, lx = 0, y = 0, ly = 0, r = 0.03) {
      if (!RoundedBoxGeometry) return kit.box(w, h, d, material, lx, y, ly);
      const m = shadows(new THREE.Mesh(new RoundedBoxGeometry(w, h, d, 3, Math.min(r, w / 2 - 1e-3, h / 2 - 1e-3, d / 2 - 1e-3)), material));
      m.position.set(lx, y + h / 2, -ly);
      return m;
    },
    cyl(r, h, material, lx = 0, y = 0, ly = 0, { top = r, segments = 28, open = false } = {}) {
      const m = shadows(new THREE.Mesh(new THREE.CylinderGeometry(top, r, h, segments, 1, open), material));
      m.position.set(lx, y + h / 2, -ly);
      return m;
    },
    ball(r, material, lx = 0, y = 0, ly = 0, squash = [1, 1, 1]) {
      const m = shadows(new THREE.Mesh(new THREE.IcosahedronGeometry(r, 2), material));
      m.position.set(lx, y, -ly);
      m.scale.set(...squash);
      return m;
    },
    group(...children) { const g = new THREE.Group(); for (const c of children.flat()) if (c) g.add(c); return g; },
    dispose() { for (const t of textures.values()) t.dispose(); for (const m of cache.values()) m.dispose(); },
  };
  return kit;
}
