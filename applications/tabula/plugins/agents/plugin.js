// Agents, as a plug-in of the board: a model of many who act on one another, run by a module, and its
// run played on a card with a clock. The card is of no model: it plays any table whose rows have a place
// (x and y) and a step at which each comes to be; a module answers such a table.
//
// Its first model is Bass's diffusion as agents on a lattice (modules/diffusion.py): how fast something
// spreads among buyers who look to one another, from two assumptions read from the board's notes, held
// to its mean field. It is written with numpy at the plug-in's level; the general home of agent-based
// models in the infrastructure is nomos's `agents` (C++), which a module may call once an engine serves it.

export default function agents(B) {
  const { h, s, note, text, region } = B;

  // A run played with a clock: the rows that have come to be by the clock's step are drawn full, the
  // others faint; beneath, how many have come to be at each step.
  const run = {
    label: 'Run', about: 'A model\'s run, played with a clock: a table whose rows have a place and a step at which each comes to be.', size: { w: 460, h: 560 },
    props: { table: '', x: 'x', y: 'y', at: 'adopted_at', title: '' },
    form: () => [
      { name: 'table', label: 'Table', kind: 'text', role: 'table' },
      { name: 'x', label: 'Across', kind: 'text', role: 'column', of: 'table' },
      { name: 'y', label: 'Up', kind: 'text', role: 'column', of: 'table' },
      { name: 'at', label: 'Comes to be at', kind: 'text', role: 'column', of: 'table', help: 'the step at which a row comes to be; empty: never' },
      { name: 'title', label: 'Title', kind: 'text' },
    ],
    signature: (ctx) => B.tableStamp(ctx.tables.get(ctx.props.table)),
    render(body, ctx) {
      const p = ctx.props, t = ctx.tables.get(p.table);
      if (!t || t.error) { body.append(h('div', { class: 'bd-hint' }, t?.error || (p.table ? `There is no table '${p.table}' on the board: run the module that answers it.` : 'Choose a table in the panel.'))); return undefined; }
      if (![p.x, p.y, p.at].every((c) => t.data[c])) { body.append(h('div', { class: 'bd-hint' }, `The table '${t.name}' needs the columns ${p.x}, ${p.y} and ${p.at}.`)); return undefined; }
      const xs = t.data[p.x], ys = t.data[p.y], at = t.data[p.at];
      const steps = Math.max(1, ...at.filter((v) => typeof v === 'number'));
      const x0 = Math.min(...xs), x1 = Math.max(...xs), y0 = Math.min(...ys), y1 = Math.max(...ys);
      // How many have come to be by each step.
      const count = new Array(steps + 1).fill(0);
      for (const v of at) if (typeof v === 'number') count[Math.max(0, Math.min(steps, Math.round(v)))]++;
      for (let k = 1; k <= steps; k++) count[k] += count[k - 1];

      const W = Math.max(160, ctx.width - 28), side = Math.min(W, Math.max(120, ctx.height - 190));
      const canvas = h('canvas', { class: 'tb-run', width: Math.round(side * 2), height: Math.round(side * 2), style: `width:${side}px;height:${side}px` });
      const curve = s('svg', { class: 'tb-run-curve', viewBox: `0 0 ${W} 54`, width: W, height: 54 });
      const clock = h('input', { type: 'range', min: 0, max: steps, value: 0, step: 1, 'data-live': true, 'aria-label': 'The step' });
      const key = h('button', { type: 'button', class: 'bd-run', 'data-live': true }, 'Play');
      const told = h('span', { class: 'bd-module-state' });
      body.append(h('div', { class: 'bd-module-title' }, p.title || t.title || t.name), canvas, curve, h('div', { class: 'tb-run-clock' }, key, clock, told));

      const css = getComputedStyle(body), ink = css.getPropertyValue('--bd-accent-ink').trim() || '#9a4f86', faint = css.getPropertyValue('--bd-line').trim() || 'rgba(0,0,0,.14)';
      const g = canvas.getContext('2d');
      const n = xs.length, cell = (2 * side) / Math.max(x1 - x0 + 1, y1 - y0 + 1, Math.sqrt(n));
      const px = (v) => ((v - x0 + 0.5) / (x1 - x0 + 1)) * 2 * side, py = (v) => (1 - (v - y0 + 0.5) / (y1 - y0 + 1)) * 2 * side;
      const X = (k) => 2 + (k / steps) * (W - 4), Y = (c) => 50 - (c / n) * 46;
      curve.append(s('path', { class: 'area', d: `M${X(0)} 50${count.map((c, k) => `L${X(k).toFixed(1)} ${Y(c).toFixed(1)}`).join('')}L${X(steps)} 50Z` }),
        s('path', { class: 'line', d: count.map((c, k) => `${k ? 'L' : 'M'}${X(k).toFixed(1)} ${Y(c).toFixed(1)}`).join('') }));
      const mark = s('path', { class: 'mark' });
      curve.append(mark);

      function draw(step) {
        g.clearRect(0, 0, canvas.width, canvas.height);
        const r = Math.max(1.2, cell * 0.42);
        for (let k = 0; k < n; k++) {
          const here = typeof at[k] === 'number' && at[k] <= step;
          g.fillStyle = here ? ink : faint;
          g.globalAlpha = here ? (at[k] === step ? 1 : 0.78) : 1;
          g.fillRect(px(xs[k]) - r, py(ys[k]) - r, 2 * r, 2 * r);
        }
        g.globalAlpha = 1;
        mark.setAttribute('d', `M${X(step).toFixed(1)} 2V52`);
        told.textContent = `step ${step} of ${steps} · ${count[step].toLocaleString('en-GB')} of ${n.toLocaleString('en-GB')} (${Math.round((100 * count[step]) / n)}%)`;
      }
      let timer = null;
      const stop = () => { clearInterval(timer); timer = null; key.textContent = 'Play'; };
      key.addEventListener('click', () => {
        if (timer) return stop();
        if (Number(clock.value) >= steps) clock.value = 0;
        key.textContent = 'Pause';
        timer = setInterval(() => {
          clock.value = Number(clock.value) + 1;
          draw(Number(clock.value));
          if (Number(clock.value) >= steps) stop();
        }, Math.max(60, Math.min(400, 6000 / steps)));
        return undefined;
      });
      clock.addEventListener('input', () => draw(Number(clock.value)));
      // The run is shown at its end until it is played.
      clock.value = steps;
      draw(steps);
      return stop;
    },
  };

  const spreading = {
    id: 'spread', name: 'How fast it would spread', about: 'Adoption spreading among buyers who look to one another, from two assumptions on notes: a model\'s run played with a clock, and its curve beside Bass\'s.',
    items: [
      text('How fast would it spread?', 0, 0),
      text('Run the module, then play the run. The two numbers on the notes are placeholders, near the averages of published fits of Bass\'s model: write your own, with the ranges you believe.', 0, 50, { size: 'small', w: 820, h: 40 }),
      region('given', 'What we take as given', 0, 120, 460, 230),
      note('assumption', 'At a step, the chance one adopts of one\'s own accord\ninnovation = 0.03 [0.01, 0.05]', 20, 170, { in: 'given', h: 150 }),
      note('assumption', 'And the chance added if all one looks to have adopted\nimitation = 0.38 [0.25, 0.5]', 240, 170, { in: 'given', h: 150 }),
      { key: 'model', kind: 'card', card: 'module', props: { module: 'agents/diffusion.py', params: {} }, x: 0, y: 400, w: 460, h: 260 },
      { key: 'run', kind: 'card', card: 'run', props: { table: 'spread_buyers', x: 'x', y: 'y', at: 'adopted_at', title: 'The buyers of the first run' }, x: 520, y: 120, w: 460, h: 580 },
      { key: 'curve', kind: 'card', card: 'chart', props: { table: 'spread', form: 'line', x: 'step', y: 'mean, bass, bass_low, bass_high', title: 'The share who have adopted' }, x: 1040, y: 120, w: 560, h: 380 },
      note('question', 'Do our buyers look to their neighbours, or to everyone?', 1040, 540, {}),
    ],
    links: [{ from: 'model', to: 'run' }, { from: 'model', to: 'curve' }],
  };

  return {
    id: 'agents', name: 'Agents', about: 'Models of many who act on one another, run and played on the board.',
    noteTypes: [], templates: [spreading], cards: { run },
  };
}
