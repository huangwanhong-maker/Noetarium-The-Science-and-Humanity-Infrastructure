// A survey about a street (ADR 0088): which alternatives to show people, and how many must answer, for what a protected lane, trees
// or fewer minutes are worth to them against a levy. The engine designs the choice sets at the priors given (peira's coordinate
// exchange) and counts the respondents each willingness to pay needs for a 95% interval of the half-width asked (from the design's
// information; nomos's conditional logit estimates it once the answers come). The cards can be taken away as a table.

const ATTRIBUTES = [
  { name: 'a protected lane', levels: 2, labels: ['yes', 'no'] },
  { name: 'street trees', levels: 3, labels: ['many', 'some', 'none'] },
  { name: 'minutes more on the way', values: [0, 5, 10] },
  { name: 'a levy, dollars a month', values: [0, 10, 25] },
];
const COLUMNS = [['a protected lane (yes against no)', 0.5], ['many trees (against the mean)', 0.3], ['some trees (against the mean)', 0.1], ['a minute more', -0.06], ['a dollar a month', -0.05]];

export async function layer(ctx) {
  const { dom: { h } } = ctx;
  let half = 5, sets = 12, plan = null, dead = false;
  const inputs = COLUMNS.map(([name, v]) => h('input', { type: 'number', step: '0.01', value: String(v), 'aria-label': name, style: 'width:6.5em' }));
  const out = h('div', { class: 'gs-readout' });
  const status = h('div', { class: 'ld-quiet' }, 'Set the priors and ask for a design.');
  const keys = (list, now, set) => h('div', { class: 'keys-tight' }, list.map(([v, name]) => h('button', { class: v === now ? 'on' : '', onclick: (e) => {
    for (const b of e.currentTarget.parentElement.children) b.classList.toggle('on', b === e.currentTarget);
    set(v);
  } }, name)));
  const label = (a, l) => (ATTRIBUTES[a].labels ? ATTRIBUTES[a].labels[l] : String(ATTRIBUTES[a].values[l]));

  async function design() {
    const prior = inputs.map((i) => Number(i.value));
    status.textContent = 'Designing the choice sets…';
    try {
      plan = await ctx.ask('survey', { attributes: ATTRIBUTES.map(({ name, levels, values }) => (values ? { name, values } : { name, levels })), prior, price: 4, sets, alternatives: 3, half_width: half });
    } catch (e) { status.textContent = String(e.message || e); return; }
    if (dead) return;
    const rows = [h('div', {}, h('span', {}, 'worth, at the priors'), h('b', {}, `dollars a month · respondents for ±$${half}`))];
    const dollars = (v) => `${v < 0 ? '−' : ''}$${Math.abs(v).toFixed(1)}`;
    COLUMNS.slice(0, 4).forEach(([name], k) => rows.push(h('div', {}, h('span', {}, name), h('b', {}, `${dollars(plan.wtp[k])} · ${Math.round(plan.respondents[k]).toLocaleString('en')}`))));
    out.replaceChildren(...rows);
    status.textContent = `${sets} sets of three, D-error ${plan.d_error.toFixed(3)}. Each respondent answers every set; the counts are the design's promise at these priors.`;
  }

  function download() {
    if (!plan) return;
    const lines = [['set', 'alternative', ...ATTRIBUTES.map((a) => a.name)].join(',')];
    plan.cards.forEach((set, s) => set.forEach((alt, j) => lines.push([s + 1, String.fromCharCode(65 + j), ...alt.map((l, a) => label(a, l))].join(','))));
    const a = document.createElement('a');
    a.href = URL.createObjectURL(new Blob([lines.join('\n')], { type: 'text/csv' }));
    a.download = 'street-survey-design.csv';
    a.click();
    setTimeout(() => URL.revokeObjectURL(a.href), 5000);
  }

  const legend = h('div', { class: 'card gs-card' }, h('b', {}, 'A survey about a street'),
    h('div', { class: 'ld-quiet' }, 'Priors: what each attribute adds to a respondent\'s utility (a logit coefficient), the levy\'s negative.'),
    ...COLUMNS.map(([name], k) => h('div', { class: 'gs-row' }, h('span', {}, name), inputs[k])),
    h('div', { class: 'gs-row' }, h('span', {}, 'sets'), keys([[8, '8'], [12, '12'], [18, '18']], sets, (v) => { sets = v; })),
    h('div', { class: 'gs-row' }, h('span', {}, 'within'), keys([[2, '±$2'], [5, '±$5'], [10, '±$10']], half, (v) => { half = v; })),
    h('div', { class: 'keys-tight' }, h('button', { onclick: design }, 'design'), h('button', { onclick: download }, 'the cards, as a table')),
    out, status,
    h('div', { class: 'ld-quiet' }, 'Designs: peira (coordinate exchange; Kessels et al. 2009). Willingness to pay: nomos\'s conditional logit and the delta method.'));
  ctx.onDispose(() => { dead = true; });
  return { legend, dispose() { dead = true; } };
}
