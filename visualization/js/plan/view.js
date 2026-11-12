// A drawn building seen in the round and from within (ADR 0096): its walls standing with their two faces, windows in their frames,
// doors on their hinges, skirtings, floors laid by the use of their rooms, ceilings with their lights, what stands in the rooms,
// the lot, the neighbours and a sky; lit by the Sun where it is, by the sky, and inside by the rooms' own lamps.
//
// Two ways of seeing: IN THE ROUND, turned about with the roof off so that the rooms can be seen from above; and WALKING, the eye
// at a person's height inside, moved by the keys or by going to a place on the floor, stopped by the walls and by what stands in
// the way (walk.js). three.js and its additions are given as an argument (`deps`); an addition that is missing is done without:
// eased edges, the light of a room about one, the shading of corners.
//
// The outside (ADR 0097): outer walls in their cladding, mapped in metres; the roof the domain read (its faces, the walls' rise
// beneath it, its eaves), in its cover; the lot's boundary; what stands in the garden.
//
// The ground's x (east) and y (north) and the height z go to three's X, −Z and Y.

import { pieceRing } from './catalogue.js';
import { stairPlan } from './draw.js';
import { boundaryOf, roofOf, shellOf } from './exterior.js';
import { dist, inRing, interiorPoint, ringArea } from './geometry.js';
import { makeKit } from './materials.js';
import { buildPiece } from './models.js';
import { elevation, wallEnds } from './model.js';
import { sunDirection, wallPieces } from './solid.js';
import { STRIPS, offsetLine } from './street.js';
import { entrance, nearestClear, obstacles, walk } from './walk.js';

const EYE = 1.6, BODY = 0.2;

// Places for a room's ceiling lights: a grid about 1.7 m apart, kept where it is half a metre clear of the room's edge.
export function lightPlaces(ring, spacing = 1.7, margin = 0.5) {
  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const p of ring) x0 = Math.min(x0, p[0]), y0 = Math.min(y0, p[1]), x1 = Math.max(x1, p[0]), y1 = Math.max(y1, p[1]);
  const nx = Math.max(1, Math.round((x1 - x0) / spacing)), ny = Math.max(1, Math.round((y1 - y0) / spacing)), out = [];
  for (let i = 0; i < nx; i++) for (let j = 0; j < ny; j++) {
    const p = [x0 + (x1 - x0) * (i + 0.5) / nx, y0 + (y1 - y0) * (j + 0.5) / ny];
    if ([[0, 0], [margin, 0], [-margin, 0], [0, margin], [0, -margin]].every(([dx, dy]) => inRing([p[0] + dx, p[1] + dy], ring))) out.push(p);
  }
  return out.length ? out : [interiorPoint(ring)];
}

export class BuildingView {
  constructor(deps, container, { night = false } = {}) {
    const T = deps.THREE;
    this.deps = deps, this.THREE = T, this.container = container, this.night = night;
    this.renderer = new T.WebGLRenderer({ antialias: true, preserveDrawingBuffer: true });
    this.renderer.setPixelRatio(Math.min(2, globalThis.devicePixelRatio || 1));
    this.renderer.shadowMap.enabled = true;
    this.renderer.shadowMap.type = T.PCFSoftShadowMap;
    this.renderer.toneMapping = T.ACESFilmicToneMapping;
    this.renderer.toneMappingExposure = 1.0;
    container.appendChild(this.renderer.domElement);
    this.scene = new T.Scene();
    this.camera = new T.PerspectiveCamera(38, 1, 0.05, 5000);
    this.camera.rotation.order = 'YXZ';
    this.orbit = new deps.OrbitControls(this.camera, this.renderer.domElement);
    this.orbit.maxPolarAngle = Math.PI / 2 - 0.02;
    this.orbit.addEventListener('change', () => this.render());

    // The sky: a dome shaded from the horizon up.
    this.skyTop = new T.Color(), this.skyLow = new T.Color();
    this.sky = new T.Mesh(new T.SphereGeometry(2400, 32, 16), new T.ShaderMaterial({
      side: T.BackSide, depthWrite: false, uniforms: { top: { value: this.skyTop }, low: { value: this.skyLow } },
      vertexShader: 'varying vec3 vP; void main() { vP = position; gl_Position = projectionMatrix * modelViewMatrix * vec4(position, 1.0); }',
      fragmentShader: 'uniform vec3 top; uniform vec3 low; varying vec3 vP; void main() { float t = smoothstep(-0.02, 0.55, normalize(vP).y); gl_FragColor = vec4(mix(low, top, t), 1.0);\n#include <tonemapping_fragment>\n#include <colorspace_fragment>\n}',
    }));
    this.scene.add(this.sky);
    this.hemi = new T.HemisphereLight(0xffffff, 0xe6d9e2, 0.5);
    this.sun = new T.DirectionalLight(0xfff1e0, 2.2);
    this.sun.castShadow = true;
    this.sun.shadow.mapSize.set(4096, 4096);
    this.sun.shadow.bias = -0.0004;
    this.sun.shadow.normalBias = 0.02;
    this.scene.add(this.hemi, this.sun, this.sun.target);
    if (deps.RoomEnvironment) {
      const pmrem = new T.PMREMGenerator(this.renderer);
      this.scene.environment = pmrem.fromScene(new deps.RoomEnvironment(), 0.04).texture;
      pmrem.dispose();
    }
    if (deps.EffectComposer && deps.RenderPass && deps.GTAOPass && deps.OutputPass) {
      this.composer = new deps.EffectComposer(this.renderer);
      this.composer.addPass(new deps.RenderPass(this.scene, this.camera));
      this.ao = new deps.GTAOPass(this.scene, this.camera, 2, 2);
      this.ao.updateGtaoMaterial({ radius: 0.45, distanceExponent: 1.3, thickness: 1.2, scale: 1.15, samples: 16 });
      this.composer.addPass(this.ao);
      this.composer.addPass(new deps.OutputPass());
    }
    this.kit = makeKit(T, { RoundedBoxGeometry: deps.RoundedBoxGeometry, night });
    this.model = new T.Group();
    this.scene.add(this.model);
    this.marks = new T.Group();
    this.scene.add(this.marks);
    this.raycaster = new T.Raycaster();
    this.centre = new T.Vector3();
    this.radius = 20, this.framed = false, this.mode = 'round', this.sunNow = { altitude: 48, azimuth: 40 };
    this.eye = { at: [0, 0], heading: 90, pitch: 0, storey: 0 };
    this.keys = new Set(), this.goal = null, this.running = false, this.moving = false, this.onEye = null;
    this.doc = null, this.reading = null, this.obs = null, this.pieces = [], this.floors = [], this.lamps = [], this.options = {};
    this.models = new Map(), this.modelUrl = (id) => `/api/models/${id}`;
    this.setTheme(night);
    this.resize();
  }

  // ---- light ----
  // By day or by night: the stuff that differs by night is made again, so the model is to be updated after.
  setTheme(night) {
    if (night !== this.night) {
      this.kit.dispose();
      this.kit = makeKit(this.THREE, { RoundedBoxGeometry: this.deps.RoundedBoxGeometry, night });
      this.cut = null;
    }
    this.night = night;
    this.applyLight();
  }

  setSun(altitude, azimuth) {
    this.sunNow = { altitude, azimuth };
    this.applyLight();
  }

  applyLight() {
    const { altitude, azimuth } = this.sunNow, up = altitude > 0, walk = this.mode === 'walk';
    const day = up ? Math.min(1, Math.sin(altitude * Math.PI / 180) * 2.4) : 0, dark = this.night || !up;
    const [x, y, z] = sunDirection(Math.max(altitude, 2), azimuth), r = this.radius * 3 + 40;
    this.sun.position.set(this.centre.x + x * r, this.centre.y + y * r, this.centre.z + z * r);
    this.sun.target.position.copy(this.centre);
    this.sun.intensity = this.night ? 0 : 0.25 + 2.5 * day;
    this.sun.color.set(up && altitude < 14 ? 0xffd9b0 : 0xfff1e0);
    const c = this.sun.shadow.camera, e = this.radius * 1.5 + 6;
    c.left = -e, c.right = e, c.top = e, c.bottom = -e, c.near = 1, c.far = r * 2 + 80;
    c.updateProjectionMatrix();
    this.hemi.intensity = dark ? 0.08 : walk ? 0.2 + 0.1 * day : 0.3 + 0.3 * day;
    this.scene.environmentIntensity = dark ? 0.06 : walk ? 0.27 : 0.3;
    if (dark) this.skyTop.set(0x1a1322), this.skyLow.set(0x3b2c46);
    else this.skyTop.set(0xa9c3e2).lerp(new this.THREE.Color(0xdcc7d6), 1 - day), this.skyLow.set(0xf6efe8);
    this.scene.background = this.skyLow;
    // The rooms' own lamps: lit when asked for, and always when it is dark outside.
    const lit = this.options.lights !== false && (this.options.lights === true || dark || walk);
    for (const lamp of this.lamps) lamp.intensity = lit ? lamp.userData.power * (dark ? 1.9 : walk ? 1.0 : 0.45) : 0;
    this.kit.mat.lamp.emissiveIntensity = lit ? 1.1 : 0.1;
    this.kit.mat.shade.emissiveIntensity = lit ? 0.42 : 0.04;
    this.renderer.toneMappingExposure = dark ? 1.05 : walk ? 0.98 : 0.92;
    this.render();
  }

  // ---- the frame ----
  resize() {
    const w = this.container.clientWidth || 800, h = this.container.clientHeight || 600;
    this.renderer.setSize(w, h, false);
    if (this.composer) this.composer.setSize(w, h);
    this.camera.aspect = w / h;
    this.camera.updateProjectionMatrix();
    this.render();
  }

  // One picture: with the corners shaded when the eye is at rest, plainly while it moves.
  render() {
    if (this.composer && !this.moving) this.composer.render();
    else this.renderer.render(this.scene, this.camera);
  }

  // ---- the model ----
  update(doc, reading, context, options = {}) {
    const T = this.THREE, k = this.kit, { roof = false, surroundings = true, upTo = Infinity, selected = null, cut: cutAt = null } = options;
    this.doc = doc, this.reading = reading, this.options = options;
    const walkMode = this.mode === 'walk', ceilings = options.ceilings ?? walkMode, top = Math.min(doc.storeys.length - 1, upTo);
    for (const child of [...this.model.children]) { this.model.remove(child); child.traverse?.((o) => { if (o.geometry) o.geometry.dispose(); }); }
    this.pieces = [], this.floors = [], this.lamps = [];
    const add = (o) => { this.model.add(o); return o; };
    const flat = (ring, holes, y, material, { shadow = true, cast = false } = {}) => {
      const r = ringArea(ring) < 0 ? ring.slice().reverse() : ring;
      const shape = new T.Shape(r.map((p) => new T.Vector2(p[0], p[1])));
      for (const h of holes || []) shape.holes.push(new T.Path(h.map((p) => new T.Vector2(p[0], p[1]))));
      const mesh = new T.Mesh(new T.ShapeGeometry(shape), material);
      mesh.rotation.x = -Math.PI / 2;
      mesh.position.y = y;
      mesh.receiveShadow = shadow, mesh.castShadow = cast;
      return mesh;
    };
    // A box along a wall: from a, along the unit dir, between s0 and s1 and z0 and z1, t thick, set off by `off` to the wall's left.
    const along = (a, dir, s0, s1, z0, z1, t, material, off = 0) => {
      const mesh = new T.Mesh(new T.BoxGeometry(s1 - s0, z1 - z0, t), material), m = (s0 + s1) / 2;
      if (Array.isArray(material)) {
        // In metres along the wall and up it, so that a cladding's courses run on from one piece to the next.
        const uv = mesh.geometry.attributes.uv;
        for (let i = 0; i < 24; i++) {
          const side = i >= 16, end = i < 8;
          if (side) uv.setXY(i, s0 + uv.getX(i) * (s1 - s0), z0 + uv.getY(i) * (z1 - z0));
          else if (end) uv.setXY(i, uv.getX(i) * t, z0 + uv.getY(i) * (z1 - z0));
        }
      }
      mesh.position.set(a[0] + dir[0] * m - dir[1] * off, (z0 + z1) / 2, -(a[1] + dir[1] * m + dir[0] * off));
      mesh.rotation.y = Math.atan2(dir[1], dir[0]);
      mesh.castShadow = !material.transparent, mesh.receiveShadow = true;
      return mesh;
    };
    const points = [];
    const plain = (color, more = {}) => new T.MeshStandardMaterial({ color, roughness: 0.95, ...more });
    const shell = shellOf(doc), clad = k.cladding(shell.cladding, shell.tone), roofs = (reading && reading.shell && reading.shell.roofs) || [];
    const roofed = new Set(roofs.map((rf) => rf.storey));

    // The ground, the lot, the neighbours.
    const ground = add(new T.Mesh(new T.CircleGeometry(900, 64), plain(this.night ? 0x2b2231 : 0xe9e3dc)));
    ground.rotation.x = -Math.PI / 2, ground.position.y = -0.04, ground.receiveShadow = true;
    const lot = doc.site.lot;
    if (lot) {
      add(flat(lot.ring, [], -0.015, k.floor('garden')));
      // Its boundary, on the edges that face no street: a hedge, a fence or a wall, standing just within the line.
      const bound = boundaryOf(doc), inward = ringArea(lot.ring) >= 0 ? 1 : -1;
      if (bound.kind !== 'none' && bound.height > 0) lot.ring.forEach((a, i) => {
        if (lot.street && lot.street[i]) return;
        const b = lot.ring[(i + 1) % lot.ring.length], L = dist(a, b);
        if (!(L > 0.3)) return;
        const dir = [(b[0] - a[0]) / L, (b[1] - a[1]) / L];
        if (bound.kind === 'hedge') {
          const m = add(k.soft(L, bound.height, 0.5, k.mat.hedge, 0, 0, 0, 0.12));
          m.position.set((a[0] + b[0]) / 2 - dir[1] * inward * 0.3, bound.height / 2, -((a[1] + b[1]) / 2 + dir[0] * inward * 0.3));
          m.rotation.y = Math.atan2(dir[1], dir[0]);
        } else if (bound.kind === 'fence') {
          add(along(a, dir, 0, L, 0.06, bound.height, 0.022, [k.mat.teak, k.mat.teak, k.mat.teak, k.mat.teak, k.cladding('battens', 'umber'), k.cladding('battens', 'umber')], inward * 0.06));
          for (let s = 0; s <= L + 1e-6; s += L / Math.max(1, Math.round(L / 2.4))) add(along(a, dir, s - 0.045, s + 0.045, 0, bound.height + 0.05, 0.09, k.mat.teak, inward * 0.12));
        } else add(along(a, dir, 0, L, 0, bound.height, 0.2, [k.mat.outside, k.mat.outside, k.mat.trim, k.mat.outside, k.cladding('render', 'chalk'), k.cladding('render', 'chalk')], inward * 0.1));
      });
      if (!doc.storeys.some((s) => s.walls.length)) points.push(...lot.ring.map((p) => [p[0], p[1], 0]));
    }
    if (context && surroundings) {
      const neighbour = plain(this.night ? 0x4a3d52 : 0xe6dce4), road = plain(this.night ? 0x352b3a : 0xd9d3ce);
      for (const b of context.buildings) {
        if (b.ring.length < 3 || !(b.height > 0)) continue;
        const shape = new T.Shape((ringArea(b.ring) < 0 ? b.ring.slice().reverse() : b.ring).map((p) => new T.Vector2(p[0], p[1])));
        const mesh = add(new T.Mesh(new T.ExtrudeGeometry(shape, { depth: b.height, bevelEnabled: false }), neighbour));
        mesh.rotation.x = -Math.PI / 2;
        mesh.castShadow = mesh.receiveShadow = true;
      }
      for (const w of context.ways) {
        if (w.rail) continue;
        for (let i = 0; i + 1 < w.line.length; i++) {
          const a = w.line[i], b = w.line[i + 1], L = dist(a, b);
          if (L > 0.05) { const m = add(along(a, [(b[0] - a[0]) / L, (b[1] - a[1]) / L], -w.width / 4, L + w.width / 4, -0.035, -0.025, w.width, road)); m.castShadow = false; }
        }
      }
    }

    // The street in hand (ADR 0098): its strips as ribbons along its centreline, each at its level, with trees on its verges.
    if (options.street && options.street.line.length >= 2) {
      const line = options.street.line, asphalt = plain(this.night ? 0x3c3640 : 0x9a9597, { roughness: 0.92 }), paint = plain(this.night ? 0x574a55 : 0xcbb9c4);
      const stuff = (kind) => (kind === 'footpath' || kind === 'frontage' || kind === 'shared' ? k.floor('court') : kind === 'verge' || kind === 'median' ? k.floor('garden') : kind === 'cycle' ? paint : asphalt);
      let trees = 0;
      for (const s of options.street.strips) {
        const def = STRIPS[s.kind] || STRIPS.lane, a = offsetLine(line, -s.from), b = offsetLine(line, -s.to), y = 0.012 + def.rise, pos = [], uv = [];
        const put = (q, h) => { pos.push(q[0], h, -q[1]); uv.push(q[0], q[1]); };
        for (let i = 0; i + 1 < line.length; i++) {
          for (const q of [a[i], b[i], b[i + 1], a[i], b[i + 1], a[i + 1]]) put(q, y);
          // The kerbs of a strip that stands above the carriageway.
          if (def.rise > 0.03) for (const e of [a, b]) for (const [q, h] of [[e[i], y], [e[i + 1], y], [e[i + 1], 0], [e[i], y], [e[i + 1], 0], [e[i], 0], [e[i], y], [e[i + 1], 0], [e[i + 1], y], [e[i], y], [e[i], 0], [e[i + 1], 0]]) put(q, h);
        }
        const geometry = new T.BufferGeometry();
        geometry.setAttribute('position', new T.Float32BufferAttribute(pos, 3));
        geometry.setAttribute('uv', new T.Float32BufferAttribute(uv, 2));
        geometry.computeVertexNormals();
        const ribbon = add(new T.Mesh(geometry, stuff(s.kind)));
        ribbon.receiveShadow = true, ribbon.castShadow = false;
        if (s.kind === 'verge' || s.kind === 'median') {
          const mid = offsetLine(line, -(s.from + s.to) / 2);
          let run = 4;
          for (let i = 0; i + 1 < mid.length && trees < 48; i++) {
            const L = dist(mid[i], mid[i + 1]);
            for (; run < L && trees < 48; run += 9, trees++) {
              const at = [mid[i][0] + (mid[i + 1][0] - mid[i][0]) * run / L, mid[i][1] + (mid[i + 1][1] - mid[i][1]) * run / L];
              const g = add(buildPiece(k, { kind: 'tree-small', at, width: Math.min(3.4, Math.max(2.2, (s.to - s.from) * 1.7)), depth: 3, height: 5.2, bearing: 0 }));
              g.position.set(at[0], y, -at[1]);
            }
            run -= L;
          }
        }
      }
    }

    doc.storeys.forEach((s, ks) => {
      if (ks > top) return;
      const z = elevation(doc, ks), R = reading && reading.storeys[ks], sound = R && R.sound;
      const indoor = (r) => r !== null && r !== undefined && sound && R.rooms[r] && R.rooms[r].indoor;
      // Floors, ceilings and the rooms' lamps.
      if (sound) {
        R.rooms.forEach((room, r) => {
          const floor = add(flat(room.ring, room.holes, z + 0.004, k.floor(room.indoor ? room.kind : room.kind || 'garden')));
          floor.userData = { floor: true, storey: ks, room: r };
          this.floors.push(floor);
          if (!room.indoor) return;
          const shown = ceilings || (roof && ks === top);
          if (shown) {
            add(flat(room.ring, room.holes, z + s.ceiling, k.mat.ceiling, { cast: true })).material.side = T.DoubleSide;
            if (roof && !(ks === top && roofed.has(ks))) add(flat(room.ring, room.holes, z + s.height + 0.02, plain(this.night ? 0x8d8289 : 0xd9cdd4, { side: T.DoubleSide }), { cast: true }));
          }
          const places = lightPlaces(room.net_ring && room.net_ring.length >= 3 ? room.net_ring : room.ring);
          if (shown) for (const p of places) { const disc = add(new T.Mesh(new T.CircleGeometry(0.05, 20), k.mat.lamp)); disc.rotation.x = Math.PI / 2; disc.position.set(p[0], z + s.ceiling - 0.004, -p[1]); }
          const c = interiorPoint(room.net_ring && room.net_ring.length >= 3 ? room.net_ring : room.ring);
          const lamp = add(new T.PointLight(0xffe6c4, 0, 11, 1.7));
          lamp.position.set(c[0], z + s.ceiling - 0.35, -c[1]);
          lamp.userData.power = 1.1 + Math.min(4.2, Math.abs(room.area) * 0.13);
          this.lamps.push(lamp);
        });
      }
      // Walls: each face in the stuff of what it faces; cut at their openings.
      s.walls.forEach((w, i) => {
        if (w.kind === 'open') return;
        const [a, b] = wallEnds(s, i), L = dist(a, b);
        if (!(L > 0)) return;
        const dir = [(b[0] - a[0]) / L, (b[1] - a[1]) / L], t = Math.max(0.04, w.t), side = sound ? R.walls[i] : null;
        const outer = side && side.outer, left = !side || indoor(side.left) ? k.mat.wall : clad, right = !side || indoor(side.right) ? k.mat.wall : clad;
        const cut = ceilings || roof ? (outer ? clad : k.mat.wall) : this.cutMaterial();
        // Cut low, as a model is, the walls of the storey looked into stand only to the cut.
        const full = outer || !sound ? s.height : ceilings || roof ? s.height : s.ceiling, H = cutAt && ks === top && !ceilings && !roof ? Math.min(full, cutAt) : full, ends = outer ? clad : k.mat.wall;
        const faces = [ends, ends, cut, k.mat.wall, right, left];
        const openings = s.openings.filter((o) => o.wall === i);
        for (const piece of wallPieces(L, H, openings)) {
          if (piece.glass) continue;
          const s0 = piece.s0 === 0 ? -t / 2 : piece.s0, s1 = piece.s1 === L ? L + t / 2 : piece.s1;
          add(along(a, dir, s0, s1, z + piece.z0, z + piece.z1, t, faces));
        }
        // Skirtings, on each side that faces a room, between the doors.
        const doors = openings.filter((o) => o.kind === 'door').map((o) => ({ ...o, height: 0.2, sill: 0 }));
        for (const [room, sign] of [[side && side.left, 1], [side && side.right, -1]]) {
          if (!indoor(room)) continue;
          for (const piece of wallPieces(L, 0.085, doors)) if (piece.z1 > 0.08) add(along(a, dir, piece.s0, piece.s1, z, z + 0.085, 0.014, k.mat.trim, sign * (t / 2 + 0.007))).castShadow = false;
        }
        for (const o of openings) {
          const s0 = o.at - o.width / 2, s1 = o.at + o.width / 2, sill = Math.max(0, o.sill || 0), head = Math.min(H, sill + o.height), f = 0.045, deep = t + 0.03;
          if (head - sill < 0.1) continue;
          if (o.kind === 'window') {
            add(along(a, dir, s0, s1, z + sill, z + head, 0.012, k.mat.glass)).castShadow = false;
            add(along(a, dir, s0, s1, z + sill, z + sill + f, deep, k.mat.trim));
            if (head >= sill + o.height - 1e-9) add(along(a, dir, s0, s1, z + head - f, z + head, deep, k.mat.trim));
            add(along(a, dir, s0, s0 + f, z + sill, z + head, deep, k.mat.trim));
            add(along(a, dir, s1 - f, s1, z + sill, z + head, deep, k.mat.trim));
            const lights = Math.max(1, Math.round(o.width / 0.9));
            for (let j = 1; j < lights; j++) add(along(a, dir, s0 + j * o.width / lights - 0.015, s0 + j * o.width / lights + 0.015, z + sill, z + head, 0.05, k.mat.trim));
            if (sill > 0.05) for (const [room, sign] of [[side && side.left, 1], [side && side.right, -1]]) if (indoor(room)) add(along(a, dir, s0 - 0.03, s1 + 0.03, z + sill - 0.02, z + sill, 0.1, k.mat.trim, sign * (t / 2 + 0.03)));
          } else {
            add(along(a, dir, s0, s0 + 0.035, z, z + head, deep, k.mat.trim));
            add(along(a, dir, s1 - 0.035, s1, z, z + head, deep, k.mat.trim));
            if (head >= sill + o.height - 1e-9) add(along(a, dir, s0, s1, z + head - 0.035, z + head, deep, k.mat.trim));
            // The leaf, on its hinge, open into the side it swings to.
            const toOutside = side && (side.left === null || side.right === null);
            const swing = o.swing ? (o.swing > 0 ? 1 : -1) : outer ? (indoor(side.left) ? 1 : -1) : 1, far = o.hinge === 'b';
            const open = (toOutside ? 14 : 82) * Math.PI / 180, base = far ? [-dir[0], -dir[1]] : dir, n = [-dir[1] * swing, dir[0] * swing];
            const to = [base[0] * Math.cos(open) + n[0] * Math.sin(open), base[1] * Math.cos(open) + n[1] * Math.sin(open)];
            const hinge = [a[0] + dir[0] * (far ? s1 - 0.035 : s0 + 0.035), a[1] + dir[1] * (far ? s1 - 0.035 : s0 + 0.035)], width = o.width - 0.07;
            const leaf = add(new T.Group());
            leaf.position.set(hinge[0], z, -hinge[1]);
            leaf.rotation.y = Math.atan2(to[1], to[0]);
            const panel = k.box(width, head - 0.045, 0.038, toOutside ? k.mat.walnut : k.mat.white, width / 2, 0.008, 0);
            const knob = (ly) => { const h = k.cyl(0.011, 0.12, k.mat.brass, width - 0.07, 0, ly, { segments: 10 }); h.rotation.z = Math.PI / 2; h.position.y = 1.0; return h; };
            leaf.add(panel, knob(0.045), knob(-0.045));
          }
        }
        points.push([a[0], a[1], z + s.height], [b[0], b[1], z + s.height]);
      });
    });

    // The roof the domain read: its faces in their cover, mapped in metres along the eave and up the slope; beneath them a soffit
    // and a fascia about the eaves; and the walls carried up to it (gables, a shed's walls, a flat roof's parapet).
    if (roof) for (const rf of roofs) {
      if (rf.storey !== top) continue;
      const cover = k.roofing(rf.cover || roofOf(doc).cover), pos = [], uv = [], thick = rf.form === 'flat' ? 0 : 0.1;
      for (const piece of rf.pieces) {
        const r = piece.ring;
        if (r.length < 3) continue;
        // The plane's gradient on the ground: up the slope; the eave runs across it.
        let n = null;
        for (let i = 1; i + 1 < r.length && !n; i++) {
          const u = [r[i][0] - r[0][0], r[i][1] - r[0][1], r[i][2] - r[0][2]], v = [r[i + 1][0] - r[0][0], r[i + 1][1] - r[0][1], r[i + 1][2] - r[0][2]];
          const c = [u[1] * v[2] - u[2] * v[1], u[2] * v[0] - u[0] * v[2], u[0] * v[1] - u[1] * v[0]];
          if (Math.abs(c[2]) > 1e-9) n = c;
        }
        if (!n) continue;
        const g = [-n[0] / n[2], -n[1] / n[2]], slope = Math.hypot(g[0], g[1]), up = slope > 1e-9 ? [g[0] / slope, g[1] / slope] : [0, 1], across = [up[1], -up[0]], stretch = Math.sqrt(1 + slope * slope);
        const order = n[2] > 0 ? r : r.slice().reverse();
        for (let i = 1; i + 1 < order.length; i++) for (const q of [order[0], order[i], order[i + 1]]) {
          pos.push(q[0], q[2], -q[1]);
          uv.push(q[0] * across[0] + q[1] * across[1], (q[0] * up[0] + q[1] * up[1]) * stretch);
        }
      }
      if (pos.length) {
        const geometry = new T.BufferGeometry();
        geometry.setAttribute('position', new T.Float32BufferAttribute(pos, 3));
        geometry.setAttribute('uv', new T.Float32BufferAttribute(uv, 2));
        geometry.computeVertexNormals();
        const top3 = add(new T.Mesh(geometry, cover));
        top3.castShadow = top3.receiveShadow = true;
        top3.userData.roof = true;
        if (thick) {
          const soffit = add(new T.Mesh(geometry, k.mat.soffit));
          soffit.position.y = -thick;
          soffit.castShadow = false;
        }
      }
      // The fascia: a band beneath the roof's edge, all round.
      if (thick && rf.outline && rf.outline.length > 1) {
        const fp = [], deep = thick + 0.07;
        for (let i = 0; i < rf.outline.length; i++) {
          const a = rf.outline[i], b = rf.outline[(i + 1) % rf.outline.length];
          fp.push(a[0], a[2] + 0.004, -a[1], b[0], b[2] + 0.004, -b[1], b[0], b[2] - deep, -b[1], a[0], a[2] + 0.004, -a[1], b[0], b[2] - deep, -b[1], a[0], a[2] - deep, -a[1]);
        }
        const geometry = new T.BufferGeometry();
        geometry.setAttribute('position', new T.Float32BufferAttribute(fp, 3));
        geometry.computeVertexNormals();
        add(new T.Mesh(geometry, k.mat.fascia)).castShadow = true;
      }
      // The walls carried up: each a shape in its own plane, as thick as the wall beneath it. Beneath a sloping roof they stop
      // under its soffit, so that the roof lies over them.
      for (const w of rf.walls || []) {
        const L = dist(w.a, w.b);
        if (!(L > 0.01) || !w.top || w.top.length < 2) continue;
        const dir = [(w.b[0] - w.a[0]) / L, (w.b[1] - w.a[1]) / L], t = Math.max(0.04, w.t || 0.25), out = [dir[1], -dir[0]];
        const shape = new T.Shape([new T.Vector2(0, rf.plate), new T.Vector2(L, rf.plate), ...w.top.slice().reverse().map(([s, z]) => new T.Vector2(s, Math.max(z - (rf.form === 'flat' ? 0 : thick + 0.015), rf.plate)))]);
        const mesh = add(new T.Mesh(new T.ExtrudeGeometry(shape, { depth: t, bevelEnabled: false }), [clad, clad]));
        mesh.position.set(w.a[0] - out[0] * t, 0, -(w.a[1] - out[1] * t));
        mesh.rotation.y = Math.atan2(dir[1], dir[0]);
        mesh.castShadow = mesh.receiveShadow = true;
      }
      points.push(...(rf.outline || []).map((q) => [q[0], q[1], q[2]]), [this.centre.x, -this.centre.z, rf.top]);
    }

    // Stairs, as steps.
    doc.stairs.forEach((st) => {
      if (st.from > top || !doc.storeys[st.from]) return;
      const plan = stairPlan(st), z = elevation(doc, st.from), rise = doc.storeys[st.from].height / st.risers, b = (st.bearing ?? 90) * Math.PI / 180, dir = [Math.cos(b), Math.sin(b)];
      for (let i = 0; i < st.risers - 1; i++) add(along(plan.foot, dir, i * st.going, (i + 1) * st.going + 0.02, z + (i + 1) * rise - 0.04, z + (i + 1) * rise, st.width, k.mat.step));
      for (let i = 0; i < st.risers - 1; i++) add(along(plan.foot, dir, i * st.going, i * st.going + 0.02, z + i * rise, z + (i + 1) * rise, st.width, k.mat.trim));
    });

    // What stands in the rooms.
    (doc.furniture || []).forEach((p, i) => {
      if (p.storey > top || !doc.storeys[p.storey]) return;
      const g = p.model && p.model.id && this.deps.GLTFLoader ? this.own(p) : buildPiece(k, p, { ceiling: doc.storeys[p.storey].ceiling });
      g.userData.piece = i;
      this.model.add(g);
      this.pieces[i] = g;
      this.seat(i, p);
    });
    this.select(selected);

    if (points.length) {
      const lo = [Infinity, Infinity, Infinity], hi = [-Infinity, -Infinity, -Infinity];
      for (const p of points) for (let c = 0; c < 3; c++) lo[c] = Math.min(lo[c], p[c]), hi[c] = Math.max(hi[c], p[c]);
      this.centre.set((lo[0] + hi[0]) / 2, (lo[2] + hi[2]) / 2, -(lo[1] + hi[1]) / 2);
      this.radius = Math.max(6, Math.hypot(hi[0] - lo[0], hi[1] - lo[1], hi[2] - lo[2]) / 2);
      if (!this.framed && this.mode === 'round') this.frame();
    }
    this.obs = obstacles(doc, Math.min(this.eye.storey, doc.storeys.length - 1), reading);
    this.applyLight();
    if (this.mode === 'walk') this.look();
  }

  // A piece's own model: a plain block of its size until the file is read, then the file's scene, turned so that its front (glTF's
  // +Z) is the piece's front and fitted to the piece's width, depth and height.
  own(p) {
    const T = this.THREE, k = this.kit, g = new T.Group(), block = k.soft(p.width, p.height, p.depth, k.mat.white, 0, 0, 0, 0.02);
    g.add(block);
    if (!this.models.has(p.model.id)) {
      this.models.set(p.model.id, new Promise((resolve) => new this.deps.GLTFLoader().load(this.modelUrl(p.model.id), (file) => resolve(file.scene), undefined, () => resolve(null))));
    }
    this.models.get(p.model.id).then((scene) => {
      if (!scene || !g.parent) return;
      const copy = scene.clone(true), box = new T.Box3().setFromObject(copy), size = box.getSize(new T.Vector3()), mid = box.getCenter(new T.Vector3());
      if (!(size.x > 0 && size.y > 0 && size.z > 0)) return;
      const inner = new T.Group();
      copy.position.set(-mid.x, -box.min.y, -mid.z);
      inner.add(copy);
      inner.scale.set(p.width / size.x, p.height / size.y, p.depth / size.z);
      inner.rotation.y = Math.PI;
      inner.traverse((o) => { if (o.isMesh) o.castShadow = o.receiveShadow = true; });
      g.remove(block);
      g.add(inner);
      this.render();
    });
    return g;
  }

  cutMaterial() {
    if (!this.cut) this.cut = new this.THREE.MeshStandardMaterial({ color: 0x4a3a4e, roughness: 0.9 });
    return this.cut;
  }

  // A piece's model set where its piece stands.
  seat(i, p) {
    const g = this.pieces[i];
    if (!g || !this.doc) return;
    g.position.set(p.at[0], elevation(this.doc, p.storey), -p.at[1]);
    g.rotation.y = (p.bearing - 90) * Math.PI / 180;
  }

  // What is in hand: a line about its footprint on the floor.
  select(i) {
    for (const child of [...this.marks.children]) { this.marks.remove(child); child.geometry.dispose(); }
    const p = i === null || i === undefined || !this.doc ? null : (this.doc.furniture || [])[i];
    if (p) this.outline(p);
    this.render();
  }

  outline(p) {
    const T = this.THREE, z = elevation(this.doc, p.storey) + 0.03, ring = pieceRing({ ...p, width: p.width + 0.08, depth: p.depth + 0.08 });
    const line = new T.LineLoop(new T.BufferGeometry().setFromPoints(ring.map((q) => new T.Vector3(q[0], z, -q[1]))), new T.LineBasicMaterial({ color: 0xc2648f }));
    for (const child of [...this.marks.children]) { this.marks.remove(child); child.geometry.dispose(); }
    this.marks.add(line);
  }

  // ---- in the round ----
  frame() {
    const r = this.radius;
    this.orbit.target.copy(this.centre);
    this.camera.position.set(this.centre.x + r * 1.3, this.centre.y + r * 1.8, this.centre.z + r * 1.75);
    this.camera.fov = 38;
    this.camera.near = Math.max(0.1, r / 200), this.camera.far = 5000;
    this.camera.updateProjectionMatrix();
    this.orbit.update();
    this.framed = true;
    this.render();
  }

  setMode(mode) {
    this.mode = mode;
    this.orbit.enabled = mode === 'round';
    if (mode === 'round') { this.camera.fov = 38; this.camera.updateProjectionMatrix(); this.framed = false; }
    else { this.camera.fov = 66; this.camera.near = 0.05; this.camera.updateProjectionMatrix(); }
  }

  // ---- walking ----
  // The eye put at a place on a storey, looking a way; a place that is not clear gives the nearest that is.
  stand(at, heading, storey = this.eye.storey) {
    if (!this.doc) return;
    this.eye.storey = Math.max(0, Math.min(this.doc.storeys.length - 1, storey));
    this.obs = obstacles(this.doc, this.eye.storey, this.reading);
    this.eye.at = nearestClear(this.obs, at, BODY, 3) || at;
    if (Number.isFinite(heading)) this.eye.heading = heading;
    this.goal = null;
    this.look();
  }

  enter(storey = 0) {
    const e = entrance(this.doc, storey, this.reading);
    this.eye.pitch = 0;
    this.stand(e.at, e.heading, storey);
  }

  look() {
    const e = this.eye, z = elevation(this.doc, e.storey) + EYE;
    this.camera.position.set(e.at[0], z, -e.at[1]);
    this.camera.rotation.set(e.pitch * Math.PI / 180, (e.heading - 90) * Math.PI / 180, 0);
    if (this.onEye) this.onEye(e);
    this.render();
  }

  turn(dHeading, dPitch) {
    this.eye.heading = (this.eye.heading + dHeading + 540) % 360 - 180;
    this.eye.pitch = Math.max(-70, Math.min(70, this.eye.pitch + dPitch));
    this.look();
  }

  // Go toward a place on the floor.
  goTo(at) {
    this.goal = [at[0], at[1]];
    this.run();
  }

  press(key, down) {
    if (down) this.keys.add(key); else this.keys.delete(key);
    if (down) { this.goal = null; this.run(); }
  }

  run() {
    if (this.running || this.mode !== 'walk') return;
    this.running = true;
    let last = null;
    const tick = (now) => {
      const dt = last === null ? 1 / 60 : Math.max(0, Math.min(0.06, (now - last) / 1000)), e = this.eye, K = this.keys;
      last = now;
      const fast = K.has('shift') ? 2.1 : 1, h = e.heading * Math.PI / 180, f = [Math.cos(h), Math.sin(h)], r = [f[1], -f[0]];
      let forward = (K.has('w') || K.has('arrowup') ? 1 : 0) - (K.has('s') || K.has('arrowdown') ? 1 : 0), strafe = (K.has('d') ? 1 : 0) - (K.has('a') ? 1 : 0);
      const spin = (K.has('arrowleft') || K.has('q') ? 1 : 0) - (K.has('arrowright') || K.has('e') ? 1 : 0);
      let target = null;
      if (forward || strafe) {
        const L = Math.hypot(forward, strafe), v = 1.5 * fast * dt;
        target = [e.at[0] + (f[0] * forward + r[0] * strafe) / L * v, e.at[1] + (f[1] * forward + r[1] * strafe) / L * v];
      } else if (this.goal) {
        const d = dist(e.at, this.goal), v = Math.min(d, 2.4 * dt);
        if (d < 0.03) this.goal = null;
        else {
          target = [e.at[0] + (this.goal[0] - e.at[0]) / d * v, e.at[1] + (this.goal[1] - e.at[1]) / d * v];
          // Turn toward where one is going, gently.
          const want = Math.atan2(this.goal[1] - e.at[1], this.goal[0] - e.at[0]) * 180 / Math.PI, diff = ((want - e.heading + 540) % 360) - 180;
          if (d > 0.6) e.heading += Math.max(-120 * dt, Math.min(120 * dt, diff));
        }
      }
      let changed = Boolean(spin);
      if (spin) e.heading += spin * 95 * dt;
      if (target) {
        const next = walk(this.obs, e.at, target, BODY);
        if (dist(next, e.at) > 1e-5) { e.at = next; changed = true; } else if (this.goal) this.goal = null;
      }
      this.moving = changed;
      if (changed) this.look();
      if (forward || strafe || spin || this.goal) requestAnimationFrame(tick);
      else { this.running = false; this.moving = false; this.render(); }
    };
    requestAnimationFrame(tick);
  }

  // ---- what is under the pointer ----
  // {piece} for what stands there, else {floor: [x, y], storey, room}; null for neither.
  pick(clientX, clientY) {
    const rect = this.renderer.domElement.getBoundingClientRect(), T = this.THREE;
    this.raycaster.setFromCamera(new T.Vector2(((clientX - rect.left) / rect.width) * 2 - 1, -((clientY - rect.top) / rect.height) * 2 + 1), this.camera);
    const hits = this.raycaster.intersectObjects([...this.pieces.filter(Boolean), ...this.floors], true);
    for (const hit of hits) {
      let o = hit.object;
      while (o && o.userData.piece === undefined && !o.userData.floor) o = o.parent;
      if (!o) continue;
      if (o.userData.piece !== undefined) return { piece: o.userData.piece, point: [hit.point.x, -hit.point.z] };
      return { floor: [hit.point.x, -hit.point.z], storey: o.userData.storey, room: o.userData.room };
    }
    return null;
  }

  // Where the pointer's ray meets the floor of a storey.
  onFloor(clientX, clientY, storey) {
    const rect = this.renderer.domElement.getBoundingClientRect(), T = this.THREE;
    this.raycaster.setFromCamera(new T.Vector2(((clientX - rect.left) / rect.width) * 2 - 1, -((clientY - rect.top) / rect.height) * 2 + 1), this.camera);
    const hit = this.raycaster.ray.intersectPlane(new T.Plane(new T.Vector3(0, 1, 0), -elevation(this.doc, storey)), new T.Vector3());
    return hit ? [hit.x, -hit.z] : null;
  }

  // A piece shown elsewhere while it is being moved by hand, before the document is changed.
  preview(i, p) {
    this.seat(i, p);
    this.outline(p);
    this.render();
  }

  dispose() {
    this.orbit.dispose();
    this.kit.dispose();
    this.renderer.dispose();
    this.renderer.domElement.remove();
  }
}

export { EYE, BODY };
