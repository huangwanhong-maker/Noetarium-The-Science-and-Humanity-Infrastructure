// Data layers of a project (the design note of 2026-11-07, §4.3): records of the data library, as the studio plug-in reads them
// (ADR 0079), drawn by the project's own marks. A record is also a resource of the project, so that a script can read what the
// Earth holds; it is written by the host, never by a system, and never saved with the project (it is read again on opening).

import { SOURCES } from '/plugins/studio/library.js';

export const DATA = Object.fromEntries(Object.entries(SOURCES).map(([id, s]) => [id, { id, title: s.title, credit: s.credit, kind: s.kind, style: s.style }]));

const records = new Map();
export function readRecord(id) {
  if (!SOURCES[id]) return Promise.reject(new Error(`the data library has no source ${id}`));
  if (!records.has(id)) records.set(id, SOURCES[id].read().catch((e) => { records.delete(id); throw e; }));
  return records.get(id);
}

// The index of a record's moment nearest an instant; a weekly record is read at the same hour of the week.
function nearest(times, ms, weekly) {
  if (!times?.length) return 0;
  if (weekly) {
    const week = 7 * 86400000, t0 = times[0];
    const into = (((ms - t0) % week) + week) % week;
    return Math.min(times.length - 1, Math.round(into / 3600000));
  }
  let best = 0;
  for (let k = 1; k < times.length; k++) if (Math.abs(times[k] - ms) < Math.abs(times[best] - ms)) best = k;
  return best;
}

// A record's marks at an instant. Events: points sized and coloured by their value, all of them, or only those of a window before the
// instant. Tracks: their paths. Cells: columns at the cells' centres, standing to their value at the moment nearest the instant.
export function marksOf(source, record, { time_ms, windowDays = 0, map } = {}) {
  const style = DATA[source]?.style || {};
  const field = style.value;
  if (record.kind === 'events') {
    const v = record.values?.[field] || [];
    const keep = [];
    for (let k = 0; k < record.lat.length; k++) if (!windowDays || (record.time[k] <= time_ms && record.time[k] > time_ms - windowDays * 86400000)) keep.push(k);
    return [{ kind: 'points', title: record.title, lon: keep.map((k) => record.lon[k]), lat: keep.map((k) => record.lat[k]), value: keep.map((k) => v[k]), size: style.pixel || 5, map: map || style.map || 'plum', height: 0 }];
  }
  if (record.kind === 'tracks') {
    return [{ kind: 'lines', title: record.title, paths: record.tracks.map((t) => t.lon.map((lon, k) => [lon, t.lat[k]])), colour: '#B892DD', height: 0 }];
  }
  if (record.kind === 'cells') {
    const at = nearest(record.time, time_ms, style.weekly);
    const vs = record.values?.[field]?.[at] || [];
    const centre = (ring) => ring.reduce((a, q) => [a[0] + q[0] / ring.length, a[1] + q[1] / ring.length], [0, 0]);
    const cs = record.cells.map((c) => centre(c.ring));
    const all = (record.values?.[field] || []).flat().filter(Number.isFinite);
    return [{ kind: 'columns', title: record.title, lon: cs.map((c) => c[0]), lat: cs.map((c) => c[1]), value: vs, radius: 34, top: style.top || 300, map: map || style.map || 'plum', domain: [0, Math.max(1, ...all)] }];
  }
  return [];
}
