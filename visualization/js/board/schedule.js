// Plans and chronicles: tasks and events written as lines of text, resolved into days, and laid out
// as a Gantt chart or as a line of time. Pure.
//
// A schedule is written a line an entry:
//
//     # Research                               a group
//     Desk research : 2026-11-16, 10d          a start and a duration (d, w, m of 30 days, y of 365)
//     Interviews : after Desk research, 2w     after one or several others (after A & B)
//     Survey : 2026-12-01..2026-12-19, 40%     from a day to a day, and how far it has come
//     Report : after Interviews & Survey, milestone
//     1789-07-14 : The Bastille falls          a date first, for a chronicle
//     -0479 : Plataea                          years before 1 are astronomical (0 is 1 BC), or '480 BC'
//
// Dates are of the proleptic Gregorian calendar, as ISO 8601 writes them, to the day, the month or
// the year. (The history domain reads historians' dates, Julian before 1582; this module does not.)
// A day is counted from 1970-01-01, and a span is [start, end): its end is the first day after it.

// Howard Hinnant's days_from_civil and civil_from_days.
export function dayNumber(y, m, d) {
  const yy = m <= 2 ? y - 1 : y;
  const era = Math.floor(yy / 400), yoe = yy - era * 400;
  const doy = Math.floor((153 * (m + (m > 2 ? -3 : 9)) + 2) / 5) + d - 1;
  return era * 146097 + yoe * 365 + Math.floor(yoe / 4) - Math.floor(yoe / 100) + doy - 719468;
}
export function civil(day) {
  const z = day + 719468;
  const era = Math.floor(z / 146097), doe = z - era * 146097;
  const yoe = Math.floor((doe - Math.floor(doe / 1460) + Math.floor(doe / 36524) - Math.floor(doe / 146096)) / 365);
  const doy = doe - (365 * yoe + Math.floor(yoe / 4) - Math.floor(yoe / 100));
  const mp = Math.floor((5 * doy + 2) / 153);
  const m = mp < 10 ? mp + 3 : mp - 9;
  return { y: yoe + era * 400 + (m <= 2 ? 1 : 0), m, d: doy - Math.floor((153 * mp + 2) / 5) + 1 };
}
const leap = (y) => (y % 4 === 0 && y % 100 !== 0) || y % 400 === 0;
const monthDays = (y, m) => [31, leap(y) ? 29 : 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31][m - 1];
const MONTHS = ['Jan', 'Feb', 'Mar', 'Apr', 'May', 'Jun', 'Jul', 'Aug', 'Sep', 'Oct', 'Nov', 'Dec'];

// A date as written: {day, next, precision}, `next` the first day after the day, month or year named.
// Null when the text is not a date.
export function parseDate(text) {
  const s = String(text).trim();
  const bc = /^(\d{1,6})\s*(BC|BCE)$/i.exec(s);
  if (bc) {
    const y = 1 - Number(bc[1]);
    return { day: dayNumber(y, 1, 1), next: dayNumber(y + 1, 1, 1), precision: 'year' };
  }
  const m = /^(-?\d{1,6})(?:-(\d{1,2})(?:-(\d{1,2}))?)?(?:\s*(AD|CE))?$/i.exec(s);
  if (!m) return null;
  const y = Number(m[1]);
  if (m[2] === undefined) return { day: dayNumber(y, 1, 1), next: dayNumber(y + 1, 1, 1), precision: 'year' };
  const mo = Number(m[2]);
  if (mo < 1 || mo > 12) return null;
  if (m[3] === undefined) return { day: dayNumber(y, mo, 1), next: mo === 12 ? dayNumber(y + 1, 1, 1) : dayNumber(y, mo + 1, 1), precision: 'month' };
  const d = Number(m[3]);
  if (d < 1 || d > monthDays(y, mo)) return null;
  const day = dayNumber(y, mo, d);
  return { day, next: day + 1, precision: 'day' };
}

const yearText = (y) => (y <= 0 ? `${1 - y} BC` : String(y));
export function formatDay(day, precision = 'day') {
  const c = civil(day);
  if (precision === 'year') return yearText(c.y);
  if (precision === 'month') return `${MONTHS[c.m - 1]} ${yearText(c.y)}`;
  return `${c.d} ${MONTHS[c.m - 1]} ${yearText(c.y)}`;
}

const UNIT_DAYS = { d: 1, day: 1, days: 1, w: 7, week: 7, weeks: 7, m: 30, month: 30, months: 30, y: 365, year: 365, years: 365 };

function parseRange(text) {
  const parts = text.split(/\s*(?:\.\.|–|—|\s-\s|\sto\s)\s*/);
  if (parts.length !== 2) return null;
  const a = parseDate(parts[0]), b = parseDate(parts[1]);
  return a && b ? { start: a.day, end: b.next, precision: a.precision } : null;
}

// The entries of a schedule as written, and what could not be read, line by line.
export function parseSchedule(text) {
  const tasks = [], errors = [];
  let group = '';
  String(text || '').split(/\r?\n/).forEach((raw, k) => {
    const line = raw.trim(), n = k + 1;
    if (!line || line.startsWith('//')) return;
    if (line.startsWith('#')) { group = line.replace(/^#+\s*/, ''); return; }
    const cut = line.indexOf(':');
    if (cut < 0) { errors.push({ line: n, message: `'${line}' has no colon between its name and its time` }); return; }
    const left = line.slice(0, cut).trim(), right = line.slice(cut + 1).trim();
    const task = { name: left, group, line: n, after: [] };
    // A date before the colon: the rest is the name.
    const lead = parseRange(left) || (parseDate(left) && { start: parseDate(left).day, precision: parseDate(left).precision });
    if (lead) {
      Object.assign(task, lead, { name: right });
      if (!right) { errors.push({ line: n, message: 'the entry has a date and no name' }); return; }
      tasks.push(task);
      return;
    }
    if (!left) { errors.push({ line: n, message: 'the entry has no name' }); return; }
    let ok = true;
    for (const part of right.split(',').map((p) => p.trim()).filter(Boolean)) {
      const range = parseRange(part), date = range ? null : parseDate(part);
      const span = /^(\d+(?:\.\d+)?)\s*([a-z]+)$/i.exec(part), pct = /^(\d+(?:\.\d+)?)\s*%$/.exec(part);
      if (range) Object.assign(task, range);
      else if (date) { task.start = date.day; task.precision = date.precision; }
      else if (span && UNIT_DAYS[span[2].toLowerCase()]) task.days = Math.max(0, Math.round(Number(span[1]) * UNIT_DAYS[span[2].toLowerCase()]));
      else if (pct) task.progress = Math.max(0, Math.min(1, Number(pct[1]) / 100));
      else if (/^milestone$/i.test(part)) task.milestone = true;
      else if (/^after\s+/i.test(part)) task.after.push(...part.replace(/^after\s+/i, '').split(/\s*(?:&|\+)\s*/).map((s) => s.trim()).filter(Boolean));
      else { errors.push({ line: n, message: `'${part}' is not a date, a duration, a percentage, 'after …' or 'milestone'` }); ok = false; }
    }
    if (ok) tasks.push(task);
  });
  return { tasks, errors };
}

// The entries given their days: one that says `after` starts when the last of those it names ends.
// Entries that cannot be placed are left out, each with its reason. Also the float of each: how many
// days it could slip without moving the end of the whole; an entry with none is critical.
export function resolveSchedule(tasks) {
  const errors = [], warnings = [];
  const byName = new Map();
  tasks.forEach((t, k) => { const key = t.name.toLowerCase(); if (!byName.has(key)) byName.set(key, k); });
  const state = new Array(tasks.length).fill(0), out = new Array(tasks.length).fill(null);
  const visit = (k, trail) => {
    if (state[k] === 2) return out[k];
    const t = tasks[k];
    if (state[k] === 1) { errors.push({ line: t.line, message: `'${t.name}' comes after itself: ${[...trail.slice(trail.indexOf(k)), k].map((i) => tasks[i].name).join(' → ')}` }); return null; }
    state[k] = 1;
    let base = null, failed = false;
    const deps = [];
    for (const name of t.after) {
      const j = byName.get(name.toLowerCase());
      if (j === undefined) { errors.push({ line: t.line, message: `'${t.name}' comes after '${name}', which is not an entry` }); failed = true; continue; }
      const dep = visit(j, [...trail, k]);
      if (!dep) {
        // Every entry left out says why, once.
        if (!errors.some((e) => e.line === t.line)) errors.push({ line: t.line, message: `'${t.name}' comes after '${name}', which has no place` });
        failed = true;
        continue;
      }
      deps.push(j);
      base = base === null ? dep.end : Math.max(base, dep.end);
    }
    state[k] = 2;
    if (failed) return null;
    const start = t.start ?? base;
    if (start === null || start === undefined) { errors.push({ line: t.line, message: `'${t.name}' has no start: give it a date or 'after …'` }); return null; }
    if (t.start !== undefined && base !== null && t.start < base) warnings.push({ line: t.line, message: `'${t.name}' starts before what it comes after has ended` });
    const end = t.end ?? start + (t.days ?? 0);
    if (end < start) { errors.push({ line: t.line, message: `'${t.name}' ends before it starts` }); return null; }
    out[k] = { ...t, index: k, start, end, deps, milestone: Boolean(t.milestone) || end === start };
    return out[k];
  };
  tasks.forEach((_, k) => visit(k, []));
  const placed = out.filter(Boolean);
  if (!placed.length) return { tasks: [], errors, warnings, span: null };
  const span = { start: Math.min(...placed.map((t) => t.start)), end: Math.max(...placed.map((t) => t.end)) };
  // Total float, from the last entries back.
  const next = new Map(placed.map((t) => [t.index, []]));
  for (const t of placed) for (const j of t.deps) next.get(j)?.push(t.index);
  const float = new Map();
  const floatOf = (k) => {
    if (float.has(k)) return float.get(k);
    const t = out[k], after = next.get(k);
    const f = after.length ? Math.min(...after.map((j) => out[j].start - t.end + floatOf(j))) : span.end - t.end;
    float.set(k, f);
    return f;
  };
  for (const t of placed) { t.float = floatOf(t.index); t.critical = t.float === 0 && (t.deps.length > 0 || next.get(t.index).length > 0); }
  return { tasks: placed, errors, warnings, span };
}

// ---- the axis of time ---------------------------------------------------------------------------------

const STEPS = [
  { unit: 'day', n: 1, days: 1 }, { unit: 'week', n: 1, days: 7 }, { unit: 'month', n: 1, days: 30.44 }, { unit: 'month', n: 3, days: 91.3 },
  ...[1, 2, 5, 10, 20, 50, 100, 200, 500, 1000, 2000, 5000].map((n) => ({ unit: 'year', n, days: 365.2425 * n })),
];

// Ticks between two days, at most `most` of them: days, Mondays, months, quarters or round years.
export function ticks(d0, d1, most = 10) {
  const step = STEPS.find((s) => (d1 - d0) / s.days <= most) || STEPS[STEPS.length - 1];
  const out = [];
  if (step.unit === 'day') for (let d = Math.ceil(d0); d <= d1; d++) out.push({ day: d, label: formatDay(d).replace(/ -?\d+( BC)?$/, '') });
  else if (step.unit === 'week') {
    // 1970-01-01 was a Thursday: Mondays are the days congruent to 4 modulo 7.
    for (let d = Math.ceil(d0) + ((4 - (Math.ceil(d0) % 7)) % 7 + 7) % 7; d <= d1; d += 7) out.push({ day: d, label: formatDay(d).replace(/ -?\d+( BC)?$/, '') });
  } else if (step.unit === 'month') {
    let { y, m } = civil(Math.ceil(d0));
    for (let guard = 0; guard < 400; guard++) {
      const d = dayNumber(y, m, 1);
      if (d > d1) break;
      if (d >= d0 && (m - 1) % step.n === 0) out.push({ day: d, label: m === 1 || !out.length ? `${MONTHS[m - 1]} ${yearText(y)}` : MONTHS[m - 1], major: m === 1 });
      if (++m > 12) { m = 1; y++; }
    }
  } else {
    const y0 = civil(Math.ceil(d0)).y;
    for (let y = Math.ceil(y0 / step.n) * step.n, guard = 0; guard < 400; y += step.n, guard++) {
      const d = dayNumber(y, 1, 1);
      if (d > d1) break;
      if (d >= d0) out.push({ day: d, label: yearText(y), major: true });
    }
  }
  return { step, ticks: out };
}

// The width a name takes at the size the charts are lettered in: an estimate, since the module does
// not measure type.
const textWidth = (s, per = 6.5) => String(s).length * per;

// A plan laid out as a Gantt chart of a given width: a row an entry under an axis of time, groups
// under their headings, each bar from its start to its end, and the joins from each entry to those
// that come after it. Lengths are in the units of the width.
export function layoutPlan(resolved, { width = 760, rowH = 26, headH = 30, today = null } = {}) {
  const { tasks, span } = resolved;
  if (!tasks.length) return { width, height: headH + rowH, rows: [], groups: [], ticks: [], joins: [], plot: { x: 0, w: width } };
  const labelW = Math.max(90, Math.min(width * 0.36, Math.max(...tasks.map((t) => textWidth(t.name))) + 26));
  const x0 = labelW, x1 = width - 16;
  const pad = Math.max(1, (span.end - span.start) * 0.03);
  const d0 = span.start - pad, d1 = Math.max(span.end, span.start + 1) + pad;
  const X = (day) => x0 + ((day - d0) / (d1 - d0)) * (x1 - x0);
  const rows = [], groups = [];
  let y = headH, group = null;
  for (const t of tasks) {
    if (t.group && t.group !== group) { groups.push({ name: t.group, y }); y += rowH - 4; }
    group = t.group;
    const a = X(t.start), b = X(t.end);
    rows.push({ task: t, y, mid: y + rowH / 2, x: a, w: Math.max(b - a, t.milestone ? 0 : 3) });
    y += rowH;
  }
  const rowOf = new Map(rows.map((r) => [r.task.index, r]));
  const joins = [];
  for (const r of rows) for (const j of r.task.deps) {
    const from = rowOf.get(j);
    if (!from) continue;
    const xa = from.x + from.w, ya = from.mid, xb = r.x, yb = r.mid;
    const down = yb > ya ? 1 : -1;
    const d = xb >= xa + 14
      ? `M${f(xa)} ${f(ya)}H${f(xa + 7)}V${f(yb)}H${f(xb - 1)}`
      : `M${f(xa)} ${f(ya)}h7V${f(yb - down * rowH / 2)}H${f(xb - 9)}V${f(yb)}h8`;
    joins.push({ from: j, to: r.task.index, d, critical: Boolean(r.task.critical && from.task.critical && from.task.end === r.task.start) });
  }
  const t = ticks(d0, d1, Math.max(3, Math.floor((x1 - x0) / 62)));
  return {
    width, height: y + 12, rows, groups, joins, plot: { x: x0, w: x1 - x0 }, step: t.step,
    ticks: t.ticks.map((k) => ({ ...k, x: X(k.day) })),
    today: today !== null && today >= d0 && today <= d1 ? X(today) : null,
  };
}
const f = (v) => Math.round(v * 10) / 10;

// A chronicle laid out as a line of time: each event a mark at its day (a bar when it lasts), with
// its name beside it, in the first lane where it does not run into another.
export function layoutChronicle(resolved, { width = 760, laneH = 30, headH = 30 } = {}) {
  const { tasks, span } = resolved;
  if (!tasks.length) return { width, height: headH + laneH, events: [], ticks: [], plot: { x: 16, w: width - 32 } };
  const x0 = 20, x1 = width - 20;
  const pad = Math.max(1, (span.end - span.start) * 0.04);
  const d0 = span.start - pad, d1 = Math.max(span.end, span.start + 1) + pad;
  const X = (day) => x0 + ((day - d0) / (d1 - d0)) * (x1 - x0);
  const lanes = [];   // the right end of what each lane holds
  const events = [];
  for (const t of tasks.slice().sort((a, b) => a.start - b.start || a.index - b.index)) {
    const a = X(t.start), b = Math.max(X(t.end), a), label = `${t.name}`;
    const w = textWidth(label) + 14;
    // The name stands to the right of its mark, or to the left where the right has no room.
    const left = b + w > width - 4 && a - w > 4;
    const from = left ? a - w - 6 : a - 6, to = left ? b + 6 : b + w;
    let lane = lanes.findIndex((end) => end < from - 6);
    if (lane < 0) { lane = lanes.length; lanes.push(-Infinity); }
    lanes[lane] = to;
    events.push({ task: t, lane, x: a, w: b - a, y: headH + lane * laneH, mid: headH + lane * laneH + laneH / 2, labelX: left ? a - 9 : b + 9, anchor: left ? 'end' : 'start' });
  }
  const t = ticks(d0, d1, Math.max(3, Math.floor((x1 - x0) / 66)));
  return { width, height: headH + Math.max(1, lanes.length) * laneH + 12, events, plot: { x: x0, w: x1 - x0 }, step: t.step, ticks: t.ticks.map((k) => ({ ...k, x: X(k.day) })) };
}

// A span of days in words: '10 days', '6 weeks'.
export function spanText(days) {
  if (days === 0) return 'a day';
  if (days % 7 === 0 && days >= 14) return `${days / 7} weeks`;
  return days === 1 ? '1 day' : `${days} days`;
}
