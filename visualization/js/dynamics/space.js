// Dynamical systems in the round (ADR 0103): a path in a space of three components, strands of a braid rising
// through time, and a landscape as a surface. three.js and its orbit controls are arguments, as for the layer's
// other marks, so that this module loads nothing and a work chooses its version.
//
// Each function fills a host element and returns { dispose() }, which stops its drawing and frees what it made.

import { cssColour } from '../../styles/colour.js';
import { colourMap } from '../../styles/palettes.js';
import { hue } from './charts.js';

function stage(THREE, OrbitControls, host, { background }) {
  const width = host.clientWidth || 640, height = host.clientHeight || 420;
  const renderer = new THREE.WebGLRenderer({ antialias: true, alpha: true, preserveDrawingBuffer: true });
  renderer.setPixelRatio(Math.min(2, globalThis.devicePixelRatio || 1));
  renderer.setSize(width, height);
  if (background) renderer.setClearColor(new THREE.Color(background), 1);
  host.replaceChildren(renderer.domElement);
  const scene = new THREE.Scene();
  const camera = new THREE.PerspectiveCamera(38, width / height, 0.01, 100);
  camera.position.set(3.0, 2.0, 3.4);
  const controls = new OrbitControls(camera, renderer.domElement);
  controls.enableDamping = true;
  let alive = true;
  const draw = () => {
    if (!alive) return;
    controls.update();
    renderer.render(scene, camera);
    requestAnimationFrame(draw);
  };
  requestAnimationFrame(draw);
  const resize = new ResizeObserver(() => {
    const w = host.clientWidth, h = host.clientHeight;
    if (!w || !h) return;
    renderer.setSize(w, h);
    camera.aspect = w / h;
    camera.updateProjectionMatrix();
  });
  resize.observe(host);
  return {
    THREE, scene, camera, controls,
    dispose() {
      alive = false;
      resize.disconnect();
      controls.dispose();
      scene.traverse((o) => { o.geometry?.dispose?.(); o.material?.dispose?.(); });
      renderer.dispose();
    },
  };
}

// The values scaled into the cube [−1, 1]³, each axis by its own range, with the box they came from drawn.
function scaled(columns) {
  const range = columns.map((c) => {
    let lo = Infinity, hi = -Infinity;
    for (const v of c) { if (v < lo) lo = v; if (v > hi) hi = v; }
    return hi > lo ? [lo, hi] : [lo - 1, hi + 1];
  });
  return { range, at: (i, v) => 2 * (v - range[i][0]) / (range[i][1] - range[i][0]) - 1 };
}

function box(THREE, colour) {
  const edges = new THREE.EdgesGeometry(new THREE.BoxGeometry(2, 2, 2));
  return new THREE.LineSegments(edges, new THREE.LineBasicMaterial({ color: new THREE.Color(colour), transparent: true, opacity: 0.35 }));
}

// A path through three components: x, y, z are arrays of one length. It is coloured along its time, pale to deep.
export function path3d(THREE, OrbitControls, host, { x, y, z, map = 'orchid', ink = '#7b6a86', background } = {}) {
  const s = stage(THREE, OrbitControls, host, { background });
  const n = Math.min(x.length, y.length, z.length), sc = scaled([x, y, z]);
  const positions = new Float32Array(3 * n), colours = new Float32Array(3 * n), ramp = colourMap(map).colour;
  for (let k = 0; k < n; k++) {
    // The third component rises: three.js's y is up.
    positions[3 * k] = sc.at(0, x[k]), positions[3 * k + 1] = sc.at(2, z[k]), positions[3 * k + 2] = -sc.at(1, y[k]);
    const c = ramp(0.25 + 0.75 * k / Math.max(1, n - 1));
    colours[3 * k] = c[0], colours[3 * k + 1] = c[1], colours[3 * k + 2] = c[2];
  }
  const geometry = new THREE.BufferGeometry();
  geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
  geometry.setAttribute('color', new THREE.BufferAttribute(colours, 3));
  s.scene.add(new THREE.Line(geometry, new THREE.LineBasicMaterial({ vertexColors: true })));
  s.scene.add(box(THREE, ink));
  return s;
}

// The strands of a braid: paths[p] is the list of [x, y] of point p at each of `times`. The plane lies flat and
// time rises, so that the strands are the worldlines of the points.
export function strands3d(THREE, OrbitControls, host, { times, paths, ink = '#7b6a86', background } = {}) {
  const s = stage(THREE, OrbitControls, host, { background });
  const xs = paths.flatMap((p) => p.map((q) => q[0])), ys = paths.flatMap((p) => p.map((q) => q[1]));
  const sc = scaled([xs, ys, times]);
  // One scale for the two directions of the plane, so that a circle stays a circle.
  const span = Math.max(sc.range[0][1] - sc.range[0][0], sc.range[1][1] - sc.range[1][0]), cx = (sc.range[0][0] + sc.range[0][1]) / 2, cy = (sc.range[1][0] + sc.range[1][1]) / 2;
  paths.forEach((p, i) => {
    const positions = new Float32Array(3 * p.length);
    for (let k = 0; k < p.length; k++) {
      positions[3 * k] = 2 * (p[k][0] - cx) / span, positions[3 * k + 1] = sc.at(2, times[k]) * 1.2, positions[3 * k + 2] = -2 * (p[k][1] - cy) / span;
    }
    const geometry = new THREE.BufferGeometry();
    geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
    s.scene.add(new THREE.Line(geometry, new THREE.LineBasicMaterial({ color: new THREE.Color(hue(i)) })));
    const end = new THREE.Mesh(new THREE.SphereGeometry(0.035, 16, 12), new THREE.MeshBasicMaterial({ color: new THREE.Color(hue(i)) }));
    end.position.set(positions[3 * (p.length - 1)], positions[3 * (p.length - 1) + 1], positions[3 * (p.length - 1) + 2]);
    s.scene.add(end);
  });
  const frame = box(THREE, ink);
  frame.scale.set(1, 1.2, 1);
  s.scene.add(frame);
  s.camera.position.set(2.6, 1.3, 2.6);
  return s;
}

// A landscape as a surface: u[iy * xs.length + ix] over the grid xs × ys, low values deep in colour and low in space.
export function surface3d(THREE, OrbitControls, host, { xs, ys, u, map = 'plum', ink = '#7b6a86', background } = {}) {
  const s = stage(THREE, OrbitControls, host, { background });
  let lo = Infinity, hi = -Infinity;
  for (const v of u) if (Number.isFinite(v)) { if (v < lo) lo = v; if (v > hi) hi = v; }
  const nx = xs.length, ny = ys.length, ramp = colourMap(map).colour;
  const positions = new Float32Array(3 * nx * ny), colours = new Float32Array(3 * nx * ny), index = [];
  for (let iy = 0; iy < ny; iy++) {
    for (let ix = 0; ix < nx; ix++) {
      const k = iy * nx + ix, t = hi > lo ? (u[k] - lo) / (hi - lo) : 0;
      positions[3 * k] = 2 * ix / Math.max(1, nx - 1) - 1, positions[3 * k + 1] = 1.1 * t - 0.55, positions[3 * k + 2] = 1 - 2 * iy / Math.max(1, ny - 1);
      const c = ramp(0.15 + 0.85 * t);
      colours[3 * k] = c[0], colours[3 * k + 1] = c[1], colours[3 * k + 2] = c[2];
      if (ix + 1 < nx && iy + 1 < ny) index.push(k, k + 1, k + nx, k + 1, k + nx + 1, k + nx);
    }
  }
  const geometry = new THREE.BufferGeometry();
  geometry.setAttribute('position', new THREE.BufferAttribute(positions, 3));
  geometry.setAttribute('color', new THREE.BufferAttribute(colours, 3));
  geometry.setIndex(index);
  s.scene.add(new THREE.Mesh(geometry, new THREE.MeshBasicMaterial({ vertexColors: true, side: THREE.DoubleSide })));
  const wire = new THREE.LineSegments(new THREE.WireframeGeometry(geometry), new THREE.LineBasicMaterial({ color: new THREE.Color(ink), transparent: true, opacity: 0.12 }));
  s.scene.add(wire);
  s.camera.position.set(2.1, 1.9, 2.3);
  return s;
}

export const groundOf = (rgb) => cssColour(rgb);
