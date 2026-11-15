// The plan of proof and the readings of a matter, as a page shows them (ADR 0121): pure arithmetic over what the law engine answers
// (api/cpp/science/law.hpp's evaluate). Each step of a way is a word and a glyph, never a tone alone.

export const STEP = {
  prove: { glyph: '●', word: 'make out' },
  disprove: { glyph: '✕', word: 'show not to hold' },
  reading: { glyph: '◆', word: 'argue for the reading' },
};

// The ways to establish and to resist one issue, each as its steps, with what is needed in every way marked; and the facts by their
// place: given by the record, open (alleged), decided against, or absent (the question turns on them, and the record is silent).
export function waysOf(answer) {
  const plan = (answer && answer.plan) || {};
  const must = new Set([...(plan.must_prove || []).map((f) => `prove:${f}`), ...(plan.must_disprove || []).map((f) => `disprove:${f}`)]);
  const steps = (w) => [
    ...(w.prove || []).map((f) => ({ kind: 'prove', what: f, every: must.has(`prove:${f}`) })),
    ...(w.disprove || []).map((f) => ({ kind: 'disprove', what: f, every: must.has(`disprove:${f}`) })),
    ...(w.readings || []).map((r) => ({ kind: 'reading', what: r, every: false })),
  ];
  return {
    issue: answer ? answer.issue : '', question: answer ? answer.question : '', burden: answer ? answer.burden || '' : '', now: plan.now || '',
    open: plan.open || [], given: plan.given || [], decided: plan.decided || [], absent: plan.absent || [],
    establish: (plan.establish || []).map(steps), resist: (plan.resist || []).map(steps),
    every: [...(plan.must_prove || []).map((f) => ({ kind: 'prove', what: f })), ...(plan.must_disprove || []).map((f) => ({ kind: 'disprove', what: f }))],
    refused: plan.verdict && plan.verdict.status === 'undecided' ? plan.verdict.reason : '',
  };
}

// A step in words: "make out transfer_to_third_country".
export function stepText(step, gloss = (x) => x) {
  return `${STEP[step.kind].word} ${step.kind === 'reading' ? step.what : gloss(step.what)}`;
}

// The readings of a matter's open terms: for each term and each of its readings, whether it is fixed or set aside, the holdings
// that read it so (binding, persuasive, left out, each with its reason), and the parties' arguments and the proposals for it.
export function readingsTable(evaluation, matter = {}, proposals = []) {
  const fixed = new Set((evaluation && evaluation.admissibility && evaluation.admissibility.fixed) || []);
  const aside = new Set((evaluation && evaluation.admissibility && evaluation.admissibility.set_aside) || []);
  const assessed = (evaluation && evaluation.authority && evaluation.authority.assessed) || [];
  const args = matter.arguments || [];
  return ((evaluation && evaluation.terms) || []).map((t) => ({
    term: t.term,
    gloss: t.gloss || '',
    readings: t.readings.map((r) => {
      const w = `[${t.term}=${r.name}]`;
      // Set aside: every argument for it defeated, or another reading of the term fixed.
      const settled = t.readings.some((x) => fixed.has(`[${t.term}=${x.name}]`));
      return {
        name: r.name, gloss: r.gloss || '', fixed: fixed.has(w), aside: aside.has(w) || (settled && !fixed.has(w)),
        holdings: assessed.filter((h) => h.term === t.term && h.reading === r.name),
        arguments: args.filter((a) => a.term === t.term && a.reading === r.name),
        proposals: proposals.filter((p) => p.proposed && p.proposed[t.term] === r.name).map((p) => ({ by: p.by, rationale: (p.rationales || {})[t.term] || '' })),
      };
    }),
    left_out: assessed.filter((h) => h.term === t.term && h.force === 'left out'),
  }));
}

// What a question rests on: of the steps the engine found standing, those on a path to the question, from the question down (a rule
// that gives it, what gives what that rule asks for, an exception to a rule on the way), each once. A rule that stands somewhere
// else in the scheme and bears on nothing asked is left out.
export function restsOn(scheme, answer) {
  const steps = new Map((((answer && answer.rests_on) || {}).steps || []).map((s) => [s.id, s]));
  const atom = (l) => (String(l).startsWith('¬') ? String(l).slice(1) : String(l));
  const out = [], seen = new Set(), took = new Set();
  const take = (id) => { if (steps.has(id) && !took.has(id)) { took.add(id); out.push(steps.get(id)); return true; } return false; };
  const visit = (a) => {
    if (seen.has(a)) return;
    seen.add(a);
    for (const r of scheme.rules || []) {
      if (atom(r.consequence) !== a || !take(r.id)) continue;
      for (const x of scheme.exceptions || []) if (x.rule === r.id && take(x.id)) for (const e of x.elements || []) if (!String(e).startsWith('[')) visit(atom(e));
      for (const e of r.elements || []) if (!String(e).startsWith('[')) visit(atom(e));
    }
    for (const k of scheme.counts || []) if (atom(k.as) === a && take(k.id)) visit(atom(k.fact));
  };
  if (answer) visit(atom(answer.question));
  return out;
}

// A label in words: "[term=reading], [other=reading]; or …" split into its environments, each a list of readings. Given the readings
// that are fixed, an environment that needs another reading of a fixed term is left out (it is not admissible), and the fixed
// readings themselves are not repeated; what is left is what still has to be argued.
export function labelEnvironments(label = [], fixed = []) {
  const fixedOf = new Map(fixed.map((r) => [String(r).slice(1).split('=')[0], String(r)]));
  const out = [];
  for (const e of label) {
    const readings = e === '(whatever the readings)' ? [] : String(e).split(/,\s*(?=\[)/);
    if (readings.some((r) => { const f = fixedOf.get(r.slice(1).split('=')[0]); return f && f !== r; })) continue;
    const left = readings.filter((r) => fixedOf.get(r.slice(1).split('=')[0]) !== r);
    if (!out.some((o) => o.join() === left.join())) out.push(left);
  }
  return out;
}
