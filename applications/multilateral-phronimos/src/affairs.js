// The affairs of an undertaking on the page (ADR 0134): each drawn where its subject is. Questions decided under the record's declared
// rules, its mandates and its review cycles stand on governance; the zone of agreement, draft by draft, opens the negotiation; a pair's
// events lie beneath the round's strands; the institutions the parties belong to are a view of their own (institutions.js). Each section
// draws what the engine answered for the record on the date read (S.answer), with the engine's own words for what it found, and says
// when the engine refused a part and why.

import { svgOutcome, svgZone, svgEventSeries, svgMandates, svgReviews } from '/visualization/js/multilateral/index.js';
import { S, names, name, again } from './common.js';

const width = (pane, most = 1000) => Math.max(620, Math.min(most, pane.clientWidth - 48));

function refused(el, part) {
  const why = S.answer?.wrong?.[part];
  return why ? el('p', { class: 'note wrong' }, `The engine could not read the ${part}: ${why}`) : null;
}

function signList(el, signs, title) {
  if (!signs?.length) return el('p', { class: 'small ok' }, '✓ No sign.');
  return el('ul', { class: 'mp-signs' }, signs.map((s) => el('li', {}, el('b', {}, `${s.kind}`), ` · ${s.where}: `, s.what)));
}

// ---- Questions decided under a rule ---------------------------------------------------------------------------------------------

export function questionsSection(ctx, pane) {
  const { el } = ctx;
  const qs = S.answer?.questions || [];
  if (!qs.length) return null;
  const redraw = again(ctx, 'governance');
  const k = Math.min(S.question || 0, qs.length - 1);
  const q = qs[k], record = (S.record.questions || [])[k] || {};
  const rule = (S.record.declared || []).find((x) => x.id === q.rule) || {};
  const chips = el('div', { class: 'mp-chips' }, qs.map((x, i) => {
    const label = (S.record.questions?.[i]?.question?.label) || x.question;
    return el('button', { type: 'button', class: `chip${i === k ? ' on' : ''}`, onclick: () => { S.question = i; redraw(); } }, label);
  }));
  const box = el('div', { class: 'mp-fig' });
  if (q.wrong) box.append(el('p', { class: 'note wrong' }, q.wrong));
  else box.innerHTML = svgOutcome(q.answer, { rule, question: record.question, names: names(), width: width(pane) });
  const said = el('div', { class: 'mp-said' },
    rule.text ? el('blockquote', {}, rule.text) : null,
    el('p', { class: 'small' }, `${rule.instrument || rule.title || q.rule}. Each open term is read under every reading declared with its source; a reading of an authority sets aside one of an analyst.`),
    q.answer?.discrepancy ? el('p', { class: 'note wrong' }, `The body's record: ${q.answer.discrepancy}.`) : null,
    q.answer?.pivots?.length ? el('p', { class: 'small' }, `It turns on ${q.answer.pivots.map((p) => `“${p.term}”`).join(' and ')}.`) : null);
  return [el('h4', { class: 'mp-sub' }, 'Questions decided under its rules'), chips, box, said];
}

// ---- Mandates ---------------------------------------------------------------------------------------------------------------------

export function mandatesSection(ctx, pane) {
  const { el } = ctx;
  const a = S.answer?.mandates;
  const wrong = refused(el, 'mandates');
  if (!a && !wrong) return null;
  const box = el('div', { class: 'mp-fig' });
  if (a) box.innerHTML = svgMandates(a, { at: S.at, width: width(pane) });
  const loads = a?.loads?.length ? el('p', { class: 'small' }, a.loads.map((l) => `${name(l.addressee)} carries ${l.mandates} in force, ${l.resourced} resourced`).join('; ') + '.') : null;
  return [el('h4', { class: 'mp-sub' }, 'Mandates, and what they asked of whom'), wrong, box, loads, a ? signList(el, a.signs) : null];
}

// ---- Review cycles -----------------------------------------------------------------------------------------------------------------

export function reviewsSection(ctx, pane) {
  const { el } = ctx;
  const a = S.answer?.reviews;
  const wrong = refused(el, 'reviews');
  if (!a && !wrong) return null;
  const box = el('div', { class: 'mp-fig' });
  if (a) box.innerHTML = svgReviews(a, { names: names(), width: width(pane) });
  const measures = a?.measures?.length ? el('p', { class: 'small' }, a.measures.map((m) => `${name(m.reviewed)} in ${m.cycle}: ${m.recommendations} recommended, ${Math.round(100 * m.share_supported)}% of those answered supported`).join('; ') + '.') : null;
  return [el('h4', { class: 'mp-sub' }, 'Review cycles: what was recommended, answered and done'), wrong, box, measures, a ? signList(el, a.signs) : null,
    el('p', { class: 'small' }, 'An implementation the party reviewed alone reports is claimed, not evidenced: it is a party to its own report.')];
}

// ---- The zone of agreement --------------------------------------------------------------------------------------------------------

export function zoneSection(ctx, pane) {
  const { el } = ctx;
  const a = S.answer?.talks, n = S.record.talks;
  const wrong = refused(el, 'talks');
  if (!a && !wrong) return null;
  if (a?.problems?.length) return [el('h4', { class: 'mp-sub' }, 'The zone of agreement, draft by draft'), el('ul', { class: 'mp-signs' }, a.problems.map((p) => el('li', {}, `${p.where}: ${p.what}`)))];
  const box = el('div', { class: 'mp-fig' });
  if (a) box.innerHTML = svgZone(a, n, { width: width(pane) });
  const wins = a?.win_sets?.length ? el('p', { class: 'small' }, `What each party can accept, of every package: ${a.win_sets.map((w) => `${name(w.party)} ${w.clear} clearly and ${w.arguable} arguably of ${w.of}`).join('; ')}.`) : null;
  const arguable = (a?.every_option?.agreeable || []).filter((p) => p.agreement === 'arguable');
  const turns = arguable.length ? el('div', { class: 'mp-said' }, el('p', { class: 'small' }, 'Where agreement is only arguable, it turns on:'),
    el('ul', { class: 'mp-signs' }, arguable.slice(0, 6).map((p) => el('li', {}, el('b', {}, p.words), ': ', p.parties.filter((x) => x.standing !== 'clear').map((x) => `${name(x.party)}, ${x.why}`).join('; '))))) : null;
  const claimed = a?.claimed ? el('p', { class: 'small' }, a.claimed.refused ? `Value claimed between ${a.claimed.between.map(name).join(' and ')} is not read: ${a.claimed.refused.replace(/^diplomacy::negotiation: /, '')}.`
    : `Between ${a.claimed.between.map(name).join(' and ')}, Nash's solution falls on ${a.claimed.nash.package} (${a.claimed.basis}).`) : null;
  return [el('h4', { class: 'mp-sub' }, 'The zone of agreement, draft by draft'), wrong,
    el('p', { class: 'small' }, 'A package is agreed clearly when every party’s negotiator and ratifiers accept it however their values fall within what the record allows; untenably when some party rejects it however they fall.'),
    box, wins, turns, claimed];
}

// ---- A pair's events ----------------------------------------------------------------------------------------------------------------

export function eventsSection(ctx, pane) {
  const { el } = ctx;
  const a = S.answer?.events;
  const wrong = refused(el, 'events');
  if (!a && !wrong) return null;
  const redraw = again(ctx, 'round');
  const series = a?.series || [];
  // The pair with the most events first, until another is chosen.
  const total = (x) => x.periods.reduce((t, p) => t + Object.values(p.events || {}).reduce((u, v) => u + v, 0), 0);
  if (S.pair === null || S.pair >= series.length) S.pair = series.reduce((best, x, i) => (total(x) > total(series[best]) ? i : best), 0);
  const k = S.pair;
  const chips = el('div', { class: 'mp-chips' }, series.map((s, i) => el('button', { type: 'button', class: `chip${i === k ? ' on' : ''}`, onclick: () => { S.pair = i; redraw(); } }, s.pair.map(name).join(' and '))));
  const box = el('div', { class: 'mp-fig' });
  if (series.length) box.innerHTML = svgEventSeries(a, { pair: k, names: names(), width: width(pane, 900) });
  const heavy = (a?.events || []).filter((e) => Number.isFinite(e.overstatement) && e.overstatement > 10);
  return [el('h4', { class: 'mp-sub' }, 'What passed between them, each event counted once'), wrong, series.length ? chips : null, box,
    heavy.length ? el('p', { class: 'small' }, heavy.map((e) => `${e.id} rests on ${e.reports} reports of ${e.origins} origin${e.origins === 1 ? '' : 's'}: counting each report apart would overstate it ${figureOf(e.overstatement)} times`).join('; ') + '.') : null,
    a?.coincident?.length ? el('ul', { class: 'mp-signs' }, a.coincident.map((c) => el('li', {}, `${c.a} and ${c.b}: ${c.what}`))) : null,
    a ? signList(el, a.signs) : null];
}

function figureOf(v) {
  if (!Number.isFinite(v)) return '—';
  if (v >= 1e6) return `${(v / 1e6).toPrecision(3)} million`;
  if (v >= 1000) return Math.round(v).toLocaleString('en-GB');
  return String(Number(v.toPrecision(3)));
}
