// How a city might meet its flood losses (ADR 0129): Freetown's 1,943 modelled flood events over 10,000 simulated years, met by
// strategies of risk layering (a contingency budget, a contingent credit, insurance), each costed a year and in the year of a rare
// flood against leaving everything to be found after the event; and the cheapest tower of the three from boundaries at the losses of
// given return periods. Every cost here is a declaration of this page, illustrative and no institution's: what a dollar raised after a
// flood costs, a credit's fee and interest, insurance's multiple. A parametric cover needs an index for each event, which this
// catalogue does not give; the engine says so rather than invent one.

import { hazards } from '../shared.js';

const UNFUNDED = [{ up_to: 2e6, unit: 1.2 }, { unit: 2.0 }];  // a little is reallocated cheaply; much needs cuts or costly borrowing
const BUDGET = { name: 'contingency budget', kind: 'budget', rate: 0.03, unit: 1 };            // its holding costs 3% a year
const CREDIT = { name: 'contingent credit', kind: 'credit', rate: 0.005, unit: 1.1 };          // a fee of 0.5% of its limit; repaid at 1.1 a unit
const COVER = { name: 'insurance', kind: 'insurance', fixed: 50000, multiple: 1.8 };           // a premium of 1.8 times its expected payment

export async function render(root, ctx) {
  const { chart, finding, dom: { h } } = ctx;
  const H = await hazards('freetown');
  const losses = H.events.losses, years = H.events.catalogue_years;
  const strategies = [
    { name: 'a contingency budget of $1 m', unfunded: UNFUNDED, instruments: [{ ...BUDGET, attachment: 0, limit: 1e6 }] },
    { name: 'and a contingent credit to $10 m', unfunded: UNFUNDED, instruments: [{ ...BUDGET, attachment: 0, limit: 1e6 }, { ...CREDIT, attachment: 1e6, limit: 9e6 }] },
    { name: 'and insurance from $10 m to $60 m', unfunded: UNFUNDED, instruments: [{ ...BUDGET, attachment: 0, limit: 1e6 }, { ...CREDIT, attachment: 1e6, limit: 9e6 }, { ...COVER, attachment: 1e7, limit: 5e7 }] },
  ];
  const a = await ctx.ask('financing', { losses, years, strategies, return_periods: [10, 50, 100, 500],
    tower: { order: [BUDGET, CREDIT, COVER], candidates: [2, 5, 10, 20, 50, 100, 200, 500, 1000], unfunded: UNFUNDED, cap_period: 100, cap: 0, top_unbounded: false },
    parametric: { attachment: 1e7, limit: 5e7, multiple: 1.8 } });
  const m = (v) => (Math.abs(v) >= 1e6 ? `$${(v / 1e6).toFixed(Math.abs(v) >= 1e7 ? 1 : 2)} m` : `$${Math.round(v / 1e3)}k`);
  root.append(h('h1', {}, 'How the city might meet its flood losses'),
    h('p', { class: 'muted' }, `Freetown's floods cost on average ${m(a.expected_loss)} a year over ${a.events.toLocaleString('en')} modelled events in ${years.toLocaleString('en')} simulated years. Each strategy below gives each source of funds a layer of every event's loss; what no layer pays is found after the flood, at a cost that rises with the amount. The costs are this page's declarations, illustrative and no institution's.`));
  const all = [a.baseline, ...a.strategies];
  const at100 = (s) => s.at.find((q) => q.return_period === 100);
  root.append(h('table', { class: 'sources' }, h('thead', {}, h('tr', {}, ['', 'expected cost a year', 'saved a year', 'funded', 'the 1-in-100-year flood: unfunded', 'and its year\'s cost'].map((x) => h('th', {}, x)))),
    h('tbody', {}, all.map((s, k) => h('tr', {}, h('th', {}, s.name), h('td', {}, m(s.expected)), h('td', {}, k === 0 ? '—' : m(a.saving[k - 1])), h('td', {}, `${Math.round(100 * s.funded_share)}%`),
      h('td', {}, m(at100(s).unfunded)), h('td', {}, m(at100(s).cost)))))));
  root.append(chart.bars(all.map((s, k) => ({ x: k, y: s.expected / 1e6, label: s.name, color: chart.category(k) })), { labels: all.map((s) => s.name), ylabel: 'expected cost a year, millions of USD', width: 520, height: 200 }));
  const T = a.tower;
  root.append(finding({
    title: 'The cheapest tower', kind: 'computed',
    statement: `Of ${T.tried} towers of the budget, the credit and insurance on boundaries at the losses of ${[2, 5, 10, 20, 50, 100, 200, 500, 1000].join(', ')}-year events, ${T.feasible} leave nothing of the 1-in-100-year flood unfunded; the cheapest: ${T.layers.map((l) => `${l.name} from ${m(l.attachment)}${l.limit ? ` to ${m(l.attachment + l.limit)}` : ''}`).join('; ')}, at ${m(T.cost.expected)} a year.`,
    method: 'risk management\'s best_tower: every non-decreasing choice of boundaries tried, each costed on the events by tyche\'s layers.',
    limits: 'Occurrence covers on events without their years; costs declared above; no reinstatements, no timing of payouts.',
  }));
  root.append(finding({ title: 'A parametric cover', kind: 'refused', statement: a.parametric.refused || 'fitted', method: 'risk management\'s parametric, tyche\'s basis risk', limits: 'An index (a gauge\'s depth, a modelled loss) for every event is needed: give one with the events.' }));
  root.append(h('p', { class: 'note' }, H.credit));
}
