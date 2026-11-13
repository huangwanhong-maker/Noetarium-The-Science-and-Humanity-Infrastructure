// The tools of the Dynamics Lab: each is declared (what it applies to, what it asks, how its answer is shown), so
// that a tool joins the Lab by being added to this list.
//
// A tool's fields are the fields of the engine's request (api/cpp/science/dynamics.hpp), so that a study is its
// tool and its request, and an example's line `#! {"tool": "run", "system": "lorenz", …}` is a study as written.
// A field has a kind, from which its input is made:
//
//   number, text, check       a number, a text, a yes or no
//   values                    numbers by name, written `x = 1, y = 1, z = 20`
//   component, parameter      one of the system's components or parameters
//   choice                    one of the options given
//   record                    a record of the project, or a run of one of its systems (a record whose law is known)
//
// render(answer, study, ctx) returns { html, round? }: markup for the study's card, and for a view in the round a
// function that fills a host element and returns what disposes of it.

import { braidDiagram, complexPlane, formsGraph, heatMap, lineChart, path3d, strands3d, surface3d } from '/visualization/js/dynamics/index.js';

const esc = (s) => String(s).replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
// A number as the sheet writes it. What is below 1e-12 is rounding, and is written as nothing.
const num = (v, digits = 4) => (Number.isFinite(v) ? (Math.abs(v) < 1e-12 ? '0' : String(Number(v.toPrecision(digits)))) : '–');
const table = (head, rows, mono = []) => `<table class="numbers"><tr>${head.map((h) => `<th>${esc(h)}</th>`).join('')}</tr>${rows.map((r) => `<tr>${r.map((c, i) => `<td${mono.includes(i) ? ' class="mono"' : ''}>${c}</td>`).join('')}</tr>`).join('')}</table>`;
const said = (html, cls = '') => `<p class="said${cls ? ` ${cls}` : ''}">${html}</p>`;
const figures = (...svgs) => `<div class="figures">${svgs.filter(Boolean).map((s) => `<div>${s}</div>`).join('')}</div>`;
const complex = ([re, im]) => (Math.abs(im) < 1e-9 ? num(re) : `${num(re)} ${im < 0 ? '−' : '+'} ${num(Math.abs(im))}i`);

export const parseValues = (text) => Object.fromEntries(String(text ?? '').split(',').map((p) => p.split('=').map((s) => s.trim())).filter((p) => p.length === 2 && p[0] && p[1] !== '' && Number.isFinite(Number(p[1]))).map(([k, v]) => [k, Number(v)]));
export const writeValues = (o) => Object.entries(o ?? {}).map(([k, v]) => `${k} = ${v}`).join(', ');

const RUNS = ['flow', 'map', 'lagrangian', 'composite', 'stochastic', 'derived'];
const DETERMINISTIC = ['flow', 'map', 'lagrangian', 'composite', 'derived'];
const runs = (s) => RUNS.includes(s.form) && !s.template && !s.error && !s.note && s.states;
const deterministic = (s) => runs(s) && DETERMINISTIC.includes(s.form) && s.merges !== false;
const zeros = (s) => Object.fromEntries((s.states ?? []).map((n) => [n, 0]));

// The fields of a record made from a system, shared by the tools that take a record.
const recordFields = [
  { key: 'record', label: 'record', kind: 'record' },
  { key: 'record.initial', label: 'its run starts at', kind: 'values', only: 'run' },
  { key: 'record.t1', label: 'for a time of', kind: 'number', default: 20, only: 'run' },
  { key: 'record.step', label: 'at the step', kind: 'number', default: 0.002, only: 'run' },
  { key: 'record.transient', label: 'after a transient of', kind: 'number', default: 5, only: 'run' },
];

function seriesOf(answer, names, most = 8) {
  const chosen = (names?.length ? names : answer.names).filter((n) => answer.names.includes(n)).slice(0, most);
  return chosen.map((n) => ({ name: n, x: answer.t, y: answer.x[answer.names.indexOf(n)] }));
}

export const TOOLS = [
  {
    id: 'run', title: 'Run', on: (s) => runs(s) || (s.form === 'quantum' && !s.error),
    about: 'The system from a state, through time: its components against time, one against another, or three in the round.',
    fields: (s) => (s.form === 'quantum'
      ? [{ key: 'levels', label: 'each factor starts at level', kind: 'text', default: s.factors.map(() => 0).join(', '), list: true }, { key: 't1', label: 'for a time of', kind: 'number', default: 20 }, { key: 'step', label: 'at the step', kind: 'number', default: 0.05 }]
      : [{ key: 'initial', label: 'from', kind: 'values', default: zeros(s) },
         { key: 't1', label: s.discrete ? 'iterations' : 'for a time of', kind: 'number', default: s.discrete ? 200 : 50 },
         ...(s.discrete ? [] : [{ key: 'step', label: 'at the step', kind: 'number', default: s.form === 'composite' && s.merges === false ? 0.001 : 0.01 }]),
         { key: 'every', label: 'recording every', kind: 'number', default: 1 }, { key: 'transient', label: 'after a transient of', kind: 'number', default: 0 },
         { key: 'view', label: 'seen as', kind: 'choice', options: ['series', 'phase', ...(s.states.length >= 3 ? ['space'] : [])], default: s.states.length >= 3 ? 'space' : s.states.length === 2 ? 'phase' : 'series' },
         { key: 'x', label: 'first axis', kind: 'component', default: s.states[0] }, { key: 'y', label: 'second axis', kind: 'component', default: s.states[1] ?? s.states[0] },
         ...(s.states.length >= 3 ? [{ key: 'z', label: 'third axis', kind: 'component', default: s.states[2] }] : [])]),
    render(a, study) {
      const r = study.request, marks = a.events.slice(0, 400).map((e) => ({ x: e.t }));
      const series = lineChart({ series: seriesOf(a, r.show), xLabel: a.purity ? 'time' : 'time', marks, dots: r.dots && r.view !== 'phase', what: 'the components against time' });
      const x = a.names.includes(r.x) ? r.x : a.names[0], y = a.names.includes(r.y) ? r.y : a.names[Math.min(1, a.names.length - 1)], z = a.names.includes(r.z) ? r.z : a.names[Math.min(2, a.names.length - 1)];
      const col = (n) => a.x[a.names.indexOf(n)];
      const phase = r.view === 'phase' || r.view === 'space' ? lineChart({ series: [{ name: `${y} against ${x}`, x: col(x), y: col(y) }], xLabel: x, yLabel: y, dots: r.dots, width: 420, height: 340, what: 'one component against another' }) : '';
      const notes = said(`${a.t.length.toLocaleString()} samples, ${esc(a.method)}${a.events.length ? `; <b>${a.events.length}</b> event${a.events.length > 1 ? 's' : ''}` : ''}${a.purity ? `; the purity of the state ends at ${num(a.purity.at(-1))}` : ''}.`);
      if (r.view === 'space' && a.names.length >= 3) return { html: `<div class="figures"><div class="round" data-round></div><div>${series}</div></div>${notes}`, round: (host, ctx) => path3d(ctx.THREE, ctx.OrbitControls, host, { x: col(x), y: col(y), z: col(z), ink: ctx.ink }) };
      return { html: figures(r.view === 'phase' ? phase : series, r.view === 'phase' ? series : '') + notes };
    },
  },
  {
    id: 'lyapunov', title: 'Exponents', on: deterministic,
    about: "Lyapunov's exponents: the rates at which nearby states part or close. Their sum is the mean divergence of the field; a positive one is chaos.",
    fields: (s) => [{ key: 'initial', label: 'from', kind: 'values', default: Object.fromEntries(s.states.map((n) => [n, 0.1])) }, { key: 'time', label: s.discrete ? 'iterations' : 'over a time of', kind: 'number', default: s.discrete ? 50000 : 300 },
                    ...(s.discrete ? [] : [{ key: 'step', label: 'at the step', kind: 'number', default: 0.005 }]), { key: 'count', label: 'how many, the largest first (0 for all)', kind: 'number', default: 0 },
                    { key: 'set', label: 'with the parameters', kind: 'values', default: {} }],
    render(a, study) {
      const chart = lineChart({ series: a.running.map((y, i) => ({ name: `λ${i + 1}`, x: a.times, y })), xLabel: 'time averaged over', yLabel: 'estimate', marks: [{ y: 0 }], what: 'the running estimates of the exponents' });
      const errors = a.error?.length === a.exponents.length;
      const all = !study.request.count;
      return { html: figures(chart) + table(errors ? ['exponent', 'value', 'error of a finite time'] : ['exponent', 'value'], a.exponents.map((v, i) => (errors ? [`λ${i + 1}`, num(v, 5), `± ${num(a.error[i], 2)}`] : [`λ${i + 1}`, num(v, 5)]))) +
        said(`${all ? `Their sum is <b>${num(a.sum, 6)}</b>; Kaplan and Yorke's dimension is <b>${num(a.dimension, 5)}</b>.` : `The ${a.exponents.length} largest of the system's exponents.`} Averaged over ${num(a.time)}: an exponent of a chaotic path settles as one over the square root of the time.${errors ? ' The error is the standard error of the means over nine blocks of the time, taken as independent.' : ''}`) };
    },
  },
  {
    id: 'averages', title: 'Averages', on: (s) => deterministic(s) && s.observables?.length > 0,
    about: 'What a system does on average: the mean of each observation it declares along one long run, with its error and its second moment. Set against a scale, the power law through the means: how the energy of a turbulent flow falls from scale to scale.',
    fields: (s) => [{ key: 'initial', label: 'from', kind: 'values', default: Object.fromEntries(s.states.slice(0, 6).map((n) => [n, 0.1])) }, { key: 'time', label: s.discrete ? 'iterations' : 'over a time of', kind: 'number', default: s.discrete ? 100000 : 300 },
                    ...(s.discrete ? [] : [{ key: 'step', label: 'at the step', kind: 'number', default: 0.005 }]), { key: 'transient', label: 'after a transient of', kind: 'number', default: s.discrete ? 1000 : 50 },
                    { key: 'of', label: 'a power law through the means of', kind: 'text', list: true, names: true, default: '' }, { key: 'against', label: 'set against the scales', kind: 'text', list: true, default: '' },
                    { key: 'set', label: 'with the parameters', kind: 'values', default: {} }],
    render(a) {
      const rows = a.names.map((n, i) => [esc(n), num(a.mean[i], 5), `± ${num(a.error[i], 2)}`, a.second[i] ? num(a.second[i], 4) : '–']);
      let html = '';
      if (a.scaling) {
        const s = a.scaling, lx = s.against.map((v) => Math.log2(v)), ly = s.means.map((v) => Math.log2(v));
        const fitted = lx.map((v) => Math.log2(s.prefactor) + s.exponent * v);
        html += figures(lineChart({ series: [{ name: 'the means', x: lx, y: ly, dots: true, size: 5 }, { name: 'the power law', x: lx, y: fitted }], xLabel: 'the scale, in octaves', yLabel: 'the mean, in octaves', width: 620, height: 340, what: 'means against scales, both in logarithms, with the power law fitted through them' }));
        html += said(`Through ${s.of.length} means the power law has the exponent <b>${num(s.exponent, 4)}</b> ± ${num(s.error, 2)}. The error is of the line fitted through the means, not of the means; an exponent over ${num(Math.max(...lx) - Math.min(...lx), 2)} octaves is an exponent over those octaves.`);
      }
      html += table(['observation', 'mean', 'its error', 'second moment over the mean squared'], rows, [0]);
      html += said(`${a.samples.toLocaleString()} samples over ${num(a.time)}, in ${a.blocks} blocks: the error of a mean is the standard error of its block means, right when a block is long against what the observation remembers. The last column is 1 for what does not fluctuate; for the squared size of an amplitude it is the flatness, 2 when the amplitude is Gaussian and more when it is intermittent.`);
      return { html };
    },
  },
  {
    id: 'equilibria', title: 'Equilibria', on: (s) => deterministic(s) && !(s.events?.length),
    about: "Where the field vanishes (for a map, where it returns its point), by Newton's method from seeds in a box, each classified by the eigenvalues of the Jacobian there.",
    fields: () => [{ key: 'lower', label: 'the box, from', kind: 'number', default: -10 }, { key: 'upper', label: 'to', kind: 'number', default: 10 }, { key: 'seeds', label: 'seeds', kind: 'number', default: 400 }, { key: 'set', label: 'with the parameters', kind: 'values', default: {} }],
    render(a) {
      const rows = a.equilibria.map((e) => [e.x.map((v) => num(v, 5)).join(', '), e.kind, e.eigenvalues.map(complex).join('; ')]);
      return { html: (rows.length ? table([a.names.join(', '), 'kind', 'eigenvalues'], rows, [0, 2]) : said('None was reached from the seeds in this box.', 'open')) + said(esc(a.note)) };
    },
  },
  {
    id: 'diagram', title: 'Diagram', on: (s) => deterministic(s) && s.params?.length > 0,
    about: 'A bifurcation diagram: what the system settles to as a parameter moves, the state carried from one value to the next.',
    fields: (s) => [{ key: 'parameter', label: 'the parameter', kind: 'parameter', default: s.params[0].name }, { key: 'from', label: 'from', kind: 'number', default: s.params[0].value * 0.5 }, { key: 'to', label: 'to', kind: 'number', default: s.params[0].value * 1.5 },
                    { key: 'count', label: 'values', kind: 'number', default: 200 }, { key: 'component', label: 'the component kept', kind: 'component', default: s.states[0] }, { key: 'initial', label: 'from', kind: 'values', default: Object.fromEntries(s.states.map((n) => [n, 0.3])) },
                    ...(s.discrete ? [] : [{ key: 'time', label: 'time at each value', kind: 'number', default: 60 }, { key: 'transient', label: 'after a transient of', kind: 'number', default: 60 }])],
    render(a) {
      return { html: figures(lineChart({ series: [{ name: a.what, x: a.x, y: a.y, size: 1.1, opacity: 0.6 }], dots: true, most: 60000, xLabel: a.parameter, yLabel: a.observable, width: 760, height: 380, what: 'a bifurcation diagram' })) + said(`${a.x.length.toLocaleString()} points: ${esc(a.what)}.`) };
    },
  },
  {
    id: 'basins', title: 'Basins', on: (s) => deterministic(s) && s.states.length >= 2,
    about: 'Each state of a grid is run, and coloured by the attractor it ends at. The share of the grid that leads to an attractor is its basin stability on that grid.',
    fields: (s) => [{ key: 'x', label: 'across', kind: 'component', default: s.states[0] }, { key: 'y', label: 'up', kind: 'component', default: s.states[1] }, { key: 'x0', label: 'across, from', kind: 'number', default: -2 }, { key: 'x1', label: 'to', kind: 'number', default: 2 },
                    { key: 'y0', label: 'up, from', kind: 'number', default: -2 }, { key: 'y1', label: 'to', kind: 'number', default: 2 }, { key: 'nx', label: 'cells across', kind: 'number', default: 60 }, { key: 'ny', label: 'cells up', kind: 'number', default: 60 },
                    { key: 'time', label: s.discrete ? 'iterations' : 'time of each run', kind: 'number', default: s.discrete ? 500 : 60 }],
    render(a) {
      const ix = a.names.indexOf(a.x), iy = a.names.indexOf(a.y);
      const names = a.attractors.map((p, i) => `${i + 1}: ${num(p[ix], 2)}, ${num(p[iy], 2)}`);
      const map = heatMap({ xs: a.xs, ys: a.ys, values: a.label, kind: 'categorical', names, xLabel: a.x, yLabel: a.y, width: 560, height: 420, points: a.attractors.map((p, i) => ({ x: p[ix], y: p[iy], label: String(i + 1) })), what: 'basins of attraction' });
      const escaped = 1 - a.share.reduce((s, v) => s + v, 0);
      const moving = (i) => a.kind?.[i] === 'moving';
      const some = a.attractors.some((_, i) => moving(i)), saddle = (a.kind ?? []).some((k) => /saddle|unstable|repelling/.test(k));
      return { html: figures(map) + table(['end', 'where paths end', 'share of the grid', 'there'], a.attractors.map((p, i) => [i + 1, p.map((v) => num(v, 4)).join(', '), num(a.share[i], 3), moving(i) ? `still moving, at ${num(a.speed[i], 2)}` : `at rest: ${esc(a.kind?.[i] ?? 'an equilibrium')}`]), [1]) +
        said(`States are labelled by where their paths are at the end of the run, in the order of their shares.${some ? ' An end that is still moving is a point of a cycle or of a strange set, which may be split into several, or a path that has not arrived: one that began on the boundary between two basins creeps toward a saddle.' : ''}${saddle ? ' An end at rest that is not stable is reached only from the boundary between basins: its share is that of a line of cells.' : ''}${escaped > 1e-9 ? ` A share of ${num(escaped, 3)} left the box.` : ''}`) };
    },
  },
  {
    id: 'landscape', title: 'Landscape', on: (s) => (deterministic(s) && !s.discrete) || (s.form === 'stochastic' && !s.discrete && !s.error),
    about: 'A function whose valleys are where the system stays: the potential of a gradient flow; for a system with noise, minus the logarithm of its stationary density, exact in one dimension and from a long path otherwise.',
    fields: (s) => [{ key: 'x0', label: 'from', kind: 'number', default: -2 }, { key: 'x1', label: 'to', kind: 'number', default: 2 },
                    ...(s.states.length > 1 ? [{ key: 'x', label: 'across', kind: 'component', default: s.states[0] }, { key: 'y', label: 'up', kind: 'component', default: s.states[1] }, { key: 'y0', label: 'up, from', kind: 'number', default: -2 }, { key: 'y1', label: 'to', kind: 'number', default: 2 }] : []),
                    ...(s.form === 'stochastic' ? [{ key: 'time', label: 'the length of the path', kind: 'number', default: 2000 }, { key: 'initial', label: 'from', kind: 'values', default: zeros(s) }] : [])],
    render(a) {
      const parts = [];
      let round;
      if (a.exact) {
        const series = [{ name: 'exact', x: a.exact.x, y: a.exact.u }];
        if (a.path && a.path.ys.length === 1) series.push({ name: 'from the path', x: a.path.xs, y: a.path.u, dots: true, size: 4 });
        parts.push(lineChart({ series, xLabel: 'state', yLabel: 'landscape', marks: [...a.exact.minima, ...a.exact.maxima].map((x) => ({ x })), what: 'a landscape of one dimension' }));
      }
      if (a.path && a.path.ys.length > 1) {
        parts.push(heatMap({ xs: a.path.xs, ys: a.path.ys, values: a.path.u, map: 'plum', deepLow: false, what: 'a landscape of two dimensions' }));
        round = (host, ctx) => surface3d(ctx.THREE, ctx.OrbitControls, host, { xs: a.path.xs, ys: a.path.ys, u: a.path.u, ink: ctx.ink });
      }
      const wells = a.exact ? said(`Wells at ${a.exact.minima.map((v) => num(v, 4)).join(', ') || 'none in the range'}; barriers at ${a.exact.maxima.map((v) => num(v, 4)).join(', ') || 'none in the range'}.`) : '';
      return { html: `<div class="figures">${parts.map((p) => `<div>${p}</div>`).join('')}${round ? '<div class="round" data-round></div>' : ''}</div>${wells}${said(`This is ${esc(a.kind)}.${a.path?.samples ? ` The path gave ${a.path.samples.toLocaleString()} samples; where none fell the landscape is drawn at its ceiling.` : ''}`)}`, round };
    },
  },
  {
    id: 'transfer', title: 'Operator', on: (s) => deterministic(s) && s.states.length === 1,
    about: "The system lifted to an operator on densities, as a matrix on cells (Ulam's method): its stationary density, and the eigenvalues at which densities relax.",
    fields: (s) => [{ key: 'x0', label: 'the interval, from', kind: 'number', default: 0 }, { key: 'x1', label: 'to', kind: 'number', default: 1 }, { key: 'cells', label: 'cells', kind: 'number', default: 200 },
                    ...(s.discrete ? [] : [{ key: 'time', label: "the flow's time of one step", kind: 'number', default: 1 }]), { key: 'set', label: 'with the parameters', kind: 'values', default: {} }],
    render(a) {
      return { html: figures(lineChart({ series: [{ name: 'density', x: a.centres, y: a.density }], xLabel: 'state', yLabel: 'stationary density', what: 'the stationary density' }), complexPlane({ points: a.eigenvalues, circle: true })) +
        said(`The matrix adds a diffusion of the size of a cell: only eigenvalues well inside the circle of a system with a gap mean anything.${a.lost > 0 ? ` A share of ${num(a.lost, 3)} of the points left the interval.` : ''}`) };
    },
  },
  {
    id: 'warnings', title: 'Slowing down', on: runs,
    about: 'Signs that a system is losing its stability: the variance and the autocorrelation of its fluctuations in a rolling window, their trends, and how often red noise shows trends as strong.',
    fields: (s) => [{ key: 'component', label: 'the component', kind: 'component', default: s.states[0] }, { key: 'initial', label: 'from', kind: 'values', default: zeros(s) }, { key: 't1', label: 'for a time of', kind: 'number', default: 300 },
                    { key: 'step', label: 'at the step', kind: 'number', default: 0.01 }, { key: 'every', label: 'sampling every', kind: 'number', default: 40 }, { key: 'window', label: 'the window, as a share', kind: 'number', default: 0.5 }, { key: 'bandwidth', label: 'the smoother, as a share', kind: 'number', default: 0.1 }],
    render(a) {
      const t = a.t ?? a.series.map((_, i) => i), at = a.at.map((i) => t[i]);
      const verdict = (tau, p) => `τ = <b>${num(tau, 3)}</b>, as strong in ${num(100 * p, 3)}% of red-noise series`;
      return { html: figures(lineChart({ series: [{ name: 'series', x: t, y: a.series }, { name: 'trend removed', x: t, y: a.trend }], xLabel: 'time', what: 'the series and its trend' }),
        lineChart({ series: [{ name: 'variance', x: at, y: a.variance }], xLabel: 'end of the window', yLabel: 'variance', what: 'the variance in a rolling window' }),
        lineChart({ series: [{ name: 'autocorrelation', x: at, y: a.autocorrelation }], xLabel: 'end of the window', yLabel: 'lag-1 autocorrelation', what: 'the autocorrelation in a rolling window' })) +
        said(`Variance: ${verdict(a.tau_variance, a.p_variance)}. Autocorrelation: ${verdict(a.tau_autocorrelation, a.p_autocorrelation)}.${a.dfa ? ` The fluctuation exponent is ${num(a.dfa.exponent, 3)} (½ without memory, 3/2 a random walk).` : ''}`) +
        said('A rising indicator is not a transition: trends arise by chance in correlated series, and a transition driven by noise or by the rate of a change need give no warning.') };
    },
  },
  {
    id: 'braid', title: 'Braid', on: (s) => runs(s) && s.states.length >= 4,
    about: 'Points that move in the plane braid their paths in time. Each point is two components; the braid is read from the run, and with a bound on the speed the record is asked whether it certifies it.',
    fields: (s) => [{ key: 'points', label: 'the points, as pairs', kind: 'text', pairs: true, default: s.states.slice(0, 2 * Math.floor(s.states.length / 2)).reduce((acc, n, i) => acc + (i % 2 ? ` ${n}` : `${i ? '; ' : ''}${n}`), '') },
                    { key: 'initial', label: 'from', kind: 'values', default: zeros(s) }, { key: 't1', label: 'for a time of', kind: 'number', default: 40 }, { key: 'step', label: 'at the step', kind: 'number', default: 0.005 },
                    { key: 'every', label: 'sampling every', kind: 'number', default: 4 }, { key: 'speed', label: 'no point faster than', kind: 'number', default: '' }, { key: 'separation', label: 'no two closer than', kind: 'number', default: '' }, { key: 'angle', label: 'the line of projection, at', kind: 'number', default: 0 }],
    render(a) { return braidCard(a, true); },
  },
  {
    id: 'rules', title: 'Words', on: (s) => s.form === 'rules' && !s.error,
    about: 'The forms the rules reach from the start, within a bound on their length; the words among them, each with the number of histories that lead to it.',
    fields: () => [{ key: 'length', label: 'forms no longer than', kind: 'number', default: 10 }, { key: 'seed', label: 'a sampled history, seed', kind: 'number', default: 1 }],
    render(a) {
      const graph = a.graph ? formsGraph({ forms: a.graph.forms, edges: a.graph.edges }) : '';
      const ambiguous = a.words.filter((w) => w.histories > 1).length;
      return { html: figures(graph) + table(['word', 'letters', 'histories', 'probability'], a.words.map((w) => [esc(w.word), w.length, w.histories, num(w.probability, 4)]), [0]) +
        said(`${a.words.length} word${a.words.length === 1 ? '' : 's'} among ${a.forms} forms${ambiguous ? `; <b>${ambiguous}</b> with more than one history, which the word does not tell apart` : ''}. ${a.complete ? 'The exploration is complete.' : `Not the whole language: ${esc(a.cut)}.`}`, a.complete ? 'certified' : 'open') +
        said(`A history drawn by the rules' weights: <span style="font-family: var(--mono)">${a.sample.map(esc).join(' → ')}</span>`) };
    },
  },
  {
    id: 'quantum', title: 'Stationary state', on: (s) => s.form === 'quantum' && !s.error,
    about: 'The state the generator leaves fixed, whether it is the only one, the spectrum of the generator, and its gap: the slowest rate of relaxation.',
    fields: () => [],
    render(a) {
      return { html: figures(complexPlane({ points: a.spectrum, what: 'the spectrum of the generator' })) + table(['observed', 'in the stationary state'], a.observables.map((n, i) => [esc(n), num(a.stationary[i], 6)])) +
        said(a.stationary_dimension === 1 ? `The stationary state is the only one; its purity is ${num(a.purity)} and the gap is <b>${num(a.gap, 5)}</b>.` : `The stationary states form a space of dimension <b>${a.stationary_dimension}</b>: the one shown is one of them, and where the system ends depends on where it starts.`, a.stationary_dimension === 1 ? 'certified' : 'open') };
    },
  },
  {
    id: 'derive', title: 'Derive', on: (s) => ['lagrangian', 'composite', 'derived'].includes(s.form) && !s.error && !s.note && s.merges !== false && !s.template,
    about: 'The declaration this system stands for: the equations of the stationary paths of an action, or the one flow its parts merge into. It is a system of the language, and can be added to the script.',
    fields: () => [],
    render(a, study, ctx) { return { html: `<pre>${esc(a.text)}</pre>${said('Derived from the terms, exactly.', 'certified')}`, actions: [{ label: 'Add to the script', run: () => ctx.append(a.text.replace(/^system (\w+)/, 'system $1_derived')) }] }; },
  },
  {
    id: 'word', title: 'A braid word', on: () => false, free: true,
    about: 'A word in the generators of a braid group, written s1 s2^-1 …: whether it is the identity, where its strands end, its entropy, and the Alexander polynomial of its closure.',
    fields: () => [{ key: 'strands', label: 'strands', kind: 'number', default: 3 }, { key: 'word', label: 'the word', kind: 'text', word: true, default: 's1 s2^-1 s1 s2^-1' }],
    render(a) { return braidCard({ braid: a }, false); },
  },
  {
    id: 'discover', title: 'Discover a law', on: () => false, record: true,
    about: 'With no law proposed: the sparsest combination of the monomials of the record that gives its changes. It finds a law of that form whether or not the true one has it.',
    fields: () => [...recordFields, { key: 'degree', label: 'monomials up to degree', kind: 'number', default: 2 }, { key: 'threshold', label: 'coefficients below this are dropped', kind: 'number', default: 0.1 }, { key: 'weak', label: 'tested against windows (no derivative of the record)', kind: 'check', default: false },
                   { key: 'discrete', label: 'the record is of a map', kind: 'check', default: false }],
    render(a, study, ctx) {
      return { html: `<pre>${esc(a.text)}</pre>${said(`<b>${a.terms}</b> terms, by ${esc(a.method)}; the equations hold on the record to ${num(a.residual, 3)} of the size of its changes.`)}${said('The residual says how well the law fits the record, not that the law is right.')}`,
        actions: [{ label: 'Add to the script', run: () => ctx.append(a.text) }] };
    },
  },
  {
    id: 'fit', title: 'Fit a template', on: () => false, record: true,
    about: "A system with unknowns (?a), or a model whose parameters are freed, filled from the record. An unknown inside an action is fitted through its equations.",
    fields: (s, ctx) => [{ key: 'template', label: 'the template', kind: 'choice', options: ctx.systems.filter((x) => x.template || x.params?.length || x.form === 'stochastic').map((x) => x.name) }, ...recordFields, { key: 'free', label: 'free these parameters', kind: 'text', list: true, names: true, default: '' }],
    render(a, study, ctx) {
      return { html: table(['unknown', 'found'], Object.entries(a.numbers).map(([k, v]) => [esc(k), num(v, 7)]), [0]) + `<pre>${esc(a.text)}</pre>` + said(`By ${esc(a.method)}${a.linear ? '' : `, in ${a.iterations} iterations from the values 1: a local best, which another start may better`}; residual ${num(a.residual, 3)}.`),
        actions: [{ label: 'Add to the script', run: () => ctx.append(a.text.replace(/^system (\w+)/, 'system $1_fitted')) }] };
    },
  },
  {
    id: 'match', title: 'Match models', on: () => false, record: true,
    about: 'Candidate models, each fitted to the first part of the record and run on the part kept aside, ordered by a criterion that charges for each coefficient fitted.',
    fields: (s, ctx) => [{ key: 'candidates', label: 'the candidates', kind: 'text', list: true, names: true, default: ctx.systems.filter((x) => x.template || (x.params?.length && DETERMINISTIC.includes(x.form))).map((x) => x.name).join(', ') },
                         { key: 'catalogue', label: "and the catalogue's models of the record's shape, in every order of its components", kind: 'check', default: false }, ...recordFields],
    render(a, study, ctx) {
      const rows = a.ranking.map((c, i) => (c.fitted ? [i + 1, esc(c.name), num(c.one_step, 3), `${c.horizon} of ${a.held_out - 1}`, c.parameters, Object.entries(c.numbers).map(([k, v]) => `${k} = ${num(v, 5)}`).join(', ')]
        : ['–', esc(c.name), '', '', '', `<span class="refused">${esc(c.refused)}</span>`]));
      return { html: table(['rank', 'model', 'error of one step', 'samples it stays near', 'fitted', 'found'], rows, [1, 5]) + said(`A ranking is among the candidates ${a.tried > a.proposed ? `tried: ${a.proposed} of the script and the catalogue's models of this shape, ${a.tried} declarations in all, the best order of each kept` : 'given'}. On a chaotic record no model stays near for long, whatever its merit.`),
        actions: a.ranking[0]?.fitted ? [{ label: 'Add the first to the script', run: () => ctx.append(a.ranking[0].text.replace(/^system (\w+)/, 'system $1_matched')) }] : [] };
    },
  },
  {
    id: 'forecast', title: 'Forecast with no law', on: () => false, record: true,
    about: 'For a record with no law proposed: a forecaster whose features are the last states and their products, fitted to the first part and run freely on the rest.',
    fields: () => [...recordFields.map((f) => (f.key === 'record.t1' ? { ...f, default: 110 } : f.key === 'record.step' ? { ...f, default: 0.005 } : f)), { key: 'record.every', label: 'sampling every', kind: 'number', default: 5, only: 'run' },
                   { key: 'delays', label: 'states remembered', kind: 'number', default: 2 }, { key: 'ridge', label: 'ridge', kind: 'number', default: 1e-6 }],
    render(a) {
      const charts = a.names.slice(0, 3).map((n, i) => lineChart({ series: [{ name: 'record', x: a.t, y: a.truth[i] }, { name: 'forecast', x: a.t, y: a.forecast[i] }], xLabel: 'time', yLabel: n, marks: [{ x: a.t[Math.min(a.horizon, a.t.length - 1)] }], height: 220, what: `the forecast of ${n}` }));
      return { html: figures(...charts) + said(`It stays near the record for <b>${a.horizon}</b> samples (the dashed line); one step is off by ${num(a.one_step, 3)} of its size; ${a.features} features.`) +
        said('It is a black box: not a system of the language. Its ridge is to be chosen by this horizon, not by the error of one step.') };
    },
  },
  {
    id: 'operator', title: 'Linear operator', on: () => false, record: true,
    about: 'The matrix that best carries each sample of the record to the next: its eigenvalues give growth rates and frequencies. Exact for a linear system; a section of the lifted operator otherwise.',
    fields: () => [...recordFields],
    render(a) {
      return { html: figures(complexPlane({ points: a.rates, what: 'the rates of the fitted operator', xLabel: 'growth rate', yLabel: 'angular frequency' })) + table(['rate', 'eigenvalue of one step'], a.rates.map((r, i) => [complex(r), complex(a.eigenvalues[i])], [0, 1])) +
        said(`One step leaves ${num(a.residual, 3)} unexplained (root-mean-square).`) };
    },
  },
];

function braidCard(a, withStrands) {
  const b = a.braid, e = b.entropy;
  const diagram = braidDiagram({ strands: b.strands, word: b.word });
  const rows = [['the word', esc(b.text.length > 220 ? `${b.text.slice(0, 220)} …` : b.text)], ['letters', b.length], ['where the strands end', b.permutation.map((p) => p + 1).join(' ')], ['exponent sum', b.exponent_sum],
    ['pure', b.pure ? 'yes' : 'no'], ['reduced', b.reduced_text !== undefined ? (b.trivial ? 'the identity' : esc(b.reduced_text.length > 160 ? `${b.reduced_text.slice(0, 160)} …` : b.reduced_text)) : esc(b.reduced_note)],
    ['entropy, per repetition', `${num(e.estimate, 6)}${e.settled ? '' : e.estimate === 0 ? ' (its loops grow as a power, not exponentially)' : ' (not settled)'}; at least ${Math.abs(e.lower) < 1e-6 ? '0' : num(e.lower, 6)} by Burau`]];
  if (b.alexander) rows.push(['Alexander polynomial of the closure', esc(b.alexander)]);
  if (a.duration) rows.push(['braiding exponent', `${num(a.braiding_exponent, 4)} per unit of time, over ${num(a.duration)}`]);
  let html = `<div class="figures"><div>${diagram}</div>${withStrands ? '<div class="round" data-round></div>' : ''}</div>` + table(['', ''], rows, [1]);
  if (a.certificate) {
    const c = a.certificate;
    html += said(c.certified ? `Certified: every motion the assumptions admit has this braid (the least margin is ${num(c.margin)}).` : `Not certified. ${esc(c.reason)}.`, c.certified ? 'certified' : 'open');
    html += said(`Assumed: ${c.assumptions.map(esc).join('; ')}. The margin is ${num(c.margin, 4)} (the draft's coarser margin: ${num(c.draft_margin, 4)}).`);
  } else if (withStrands) {
    html += said('This is the braid of the straight lines between the samples. With no bound on the speed of the points nothing is certified: a point may turn about another between two samples.', 'open');
  }
  if (a.winding?.values) html += said(`The windings the record admits: <b>${a.winding.values.length > 12 ? `${a.winding.values[0]} … ${a.winding.values.at(-1)} (${a.winding.values.length})` : a.winding.values.join(', ')}</b>. ${esc(a.winding.reason)}.`, a.winding.certified ? 'certified' : 'open');
  return { html, round: withStrands ? (host, ctx) => strands3d(ctx.THREE, ctx.OrbitControls, host, { times: a.times, paths: a.paths, ink: ctx.ink }) : undefined };
}

// A braid word as written, `s1 s2^-1 s3`, as the letters the engine takes.
export const parseWord = (text) => [...String(text).matchAll(/s\s*(\d+)\s*(\^\s*-\s*1)?/g)].map((m) => (m[2] ? -1 : 1) * Number(m[1]));
