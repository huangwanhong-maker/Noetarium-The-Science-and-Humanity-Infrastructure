// The outside of a drawn building as the document keeps it (ADR 0097): the roof's form, what its walls are clad in, what the roof
// is covered with, and how the lot is bounded. Pure: names, defaults and the reading of a document's choices; what follows of them
// (the roof's faces, its ridges, the walls' rise beneath it, light and sun at the windows) is the architecture domain's `shell`.
//
// A ROOF of the document is {form, pitch, eaves, cover, gables, fall, parapet}:
// - form: flat, shed (one plane), gable or hip (planes of one pitch rising from the eaves: the footprint's straight skeleton);
// - pitch in degrees, eaves the overhang beyond the walls' outer faces in metres;
// - gables: points on the ground; the eave nearest each stands as a gable (a wall carried up to the roof) and does not rise;
// - fall: for a shed, the bearing the water runs toward (degrees counter-clockwise from east);
// - parapet: for a flat roof, how far the walls stand above it.

export const ROOF_FORMS = [['flat', 'Flat'], ['shed', 'Shed'], ['gable', 'Gable'], ['hip', 'Hip']];

// What a roof is covered with: its name, its tone, and the course its pattern keeps in metres (along the eave, up the slope).
export const ROOF_COVERS = {
  tiles: { title: 'Clay tiles', tone: '#b0705f', course: [0.3, 0.33] },
  slate: { title: 'Slate', tone: '#646876', course: [0.25, 0.22] },
  metal: { title: 'Standing seam', tone: '#8d9299', course: [0.45, 0] },
  shingle: { title: 'Timber shingles', tone: '#9a8266', course: [0.14, 0.18] },
  membrane: { title: 'Membrane', tone: '#cdc7c2', course: [0, 0] },
};

// What the outer walls are clad in: its name and the course of its pattern (along the wall, up it).
export const CLADDINGS = {
  render: { title: 'Render', course: [0, 0] },
  brick: { title: 'Brick', course: [0.24, 0.086] },
  boards: { title: 'Weatherboards', course: [0, 0.17] },
  battens: { title: 'Timber battens', course: [0.11, 0] },
  stone: { title: 'Stone', course: [0.5, 0.25] },
};

// Tones for walls, by name. A wall keeps its tone's name; a renderer gives it its colour.
export const WALL_TONES = {
  chalk: '#ebe4dc', sand: '#d8c8ae', blush: '#e3c5bd', clay: '#b8846c', sage: '#aab5a0', mist: '#b9c4cc', slate: '#757984', plum: '#75607a', umber: '#7d6453',
};

// How a lot is bounded on the edges that do not face a street.
export const BOUNDARIES = { none: { title: 'Open', height: 0 }, hedge: { title: 'Hedge', height: 1.2 }, fence: { title: 'Fence', height: 1.5 }, wall: { title: 'Wall', height: 1.8 } };

export const DEFAULT_ROOF = { form: 'flat', pitch: 0, eaves: 0.45, cover: 'membrane', gables: [], fall: -90, parapet: 0.3 };
export const DEFAULT_SHELL = { cladding: 'render', tone: 'chalk' };

// A document's roof with what is missing given its default. A roof that states a pitch and no form is a hip (documents before
// ADR 0097 kept only the pitch).
export function roofOf(doc) {
  const raw = (doc && doc.roof) || {}, roof = { ...DEFAULT_ROOF, ...raw };
  if (!raw.form) roof.form = raw.pitch > 0 ? 'hip' : 'flat';
  if (!ROOF_FORMS.some(([id]) => id === roof.form)) roof.form = 'flat';
  roof.pitch = roof.form === 'flat' ? 0 : Math.max(1, Math.min(60, Number(roof.pitch) || 25));
  roof.eaves = roof.form === 'flat' ? 0 : Math.max(0, Math.min(1.5, Number(roof.eaves) || 0));
  roof.gables = roof.form === 'gable' && Array.isArray(roof.gables) ? roof.gables.filter((p) => Array.isArray(p) && p.length === 2) : [];
  // A flat roof lies under a membrane; a roof that slopes under anything else.
  if (roof.form === 'flat') roof.cover = 'membrane';
  else if (!ROOF_COVERS[roof.cover] || roof.cover === 'membrane') roof.cover = 'tiles';
  return roof;
}

export function shellOf(doc) {
  const shell = { ...DEFAULT_SHELL, ...((doc && doc.shell) || {}) };
  if (!CLADDINGS[shell.cladding]) shell.cladding = 'render';
  if (!WALL_TONES[shell.tone]) shell.tone = 'chalk';
  return shell;
}

export function boundaryOf(doc) {
  const b = (doc && doc.site && doc.site.boundary) || {};
  const kind = BOUNDARIES[b.kind] ? b.kind : 'none';
  return { kind, height: Number(b.height) > 0 ? Math.min(3, Number(b.height)) : BOUNDARIES[kind].height };
}

// The eaves that would stand as gables when a roof is first made a gable: the two shortest sides of the footprint's bounding
// rectangle, given as points on the ground at their middles (the building's long way is the ridge's). Answers [] for no ring.
export function defaultGables(ring) {
  if (!ring || ring.length < 3) return [];
  let x0 = Infinity, y0 = Infinity, x1 = -Infinity, y1 = -Infinity;
  for (const p of ring) x0 = Math.min(x0, p[0]), y0 = Math.min(y0, p[1]), x1 = Math.max(x1, p[0]), y1 = Math.max(y1, p[1]);
  return x1 - x0 >= y1 - y0 ? [[x0, (y0 + y1) / 2], [x1, (y0 + y1) / 2]] : [[(x0 + x1) / 2, y0], [(x0 + x1) / 2, y1]];
}

// The eight winds, for naming the way a face looks: a bearing in degrees counter-clockwise from east.
export function windOf(bearing) {
  const names = ['east', 'north-east', 'north', 'north-west', 'west', 'south-west', 'south', 'south-east'];
  return names[((Math.round(bearing / 45) % 8) + 8) % 8];
}
