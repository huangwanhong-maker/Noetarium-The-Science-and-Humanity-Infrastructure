// Music's room: a tuning on a page of its own. The circle of its fifths is drawn with each fifth coloured by how far it
// lies from pure, and within it each note's major third; every arc and every note sounds when touched. Beside the circle
// stand the lengths of a string that give its degrees, as on a monochord. What is drawn is the musicology domain's,
// asked of the engine; the page sounds it and shows the beating of what sounds.

import { tone } from './activities.js';

const NS = 'http://www.w3.org/2000/svg';
const svg = (tag, attrs = {}, text = '') => { const n = document.createElementNS(NS, tag); for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, v); if (text) n.textContent = text; return n; };
const signed = (c) => `${c >= 0 ? '+' : '−'}${Math.abs(c).toFixed(2)}`;
const at = (cx, cy, r, deg) => [cx + r * Math.cos((deg - 90) * Math.PI / 180), cy + r * Math.sin((deg - 90) * Math.PI / 180)];
const arc = (cx, cy, r, a0, a1) => { const [x0, y0] = at(cx, cy, r, a0), [x1, y1] = at(cx, cy, r, a1); return `M${x0.toFixed(1)} ${y0.toFixed(1)}A${r} ${r} 0 ${a1 - a0 > 180 ? 1 : 0} 1 ${x1.toFixed(1)} ${y1.toFixed(1)}`; };
// How an interval is coloured: still when pure, rose when narrow, violet when wide, and stronger the farther it lies.
const tint = (d) => (d.pure ? 'var(--good)' : `color-mix(in srgb, ${d.departure < 0 ? 'var(--accent)' : 'var(--accent-2)'} ${Math.round(30 + Math.min(1, Math.abs(d.departure) / 14) * 70)}%, var(--line))`);

export const room = {
  async practise(ctx) {
    const { el, object: o, api } = ctx;
    const names = Object.values(o.names || {}).join(' · ');
    const first = el('input', { type: 'text', value: sessionStorage.getItem('histos.first') || '256', size: 5, 'aria-label': 'The pitch of the first degree, in hertz' });
    const others = ctx.record.objects.filter((x) => x.sound?.tuning && x.id !== o.id && x.sound.tuning !== o.sound.tuning && ctx.inLayer(x.standing));
    const beside = el('select', {}, el('option', { value: '' }, 'nothing'), ...others.map((x) => el('option', { value: x.sound.tuning }, x.name)));
    const body = el('div', {});
    let timers = [];
    const draw = async () => {
      timers.forEach(clearTimeout);
      timers = [];
      const hz = Number(first.value);
      if (!(hz > 20 && hz < 2000)) { body.replaceChildren(el('p', { class: 'msg', style: 'padding:30px' }, 'State the pitch of the first degree in hertz, between 20 and 2000.')); return; }
      sessionStorage.setItem('histos.first', first.value);
      const ask = (system) => api(`/api/tuning?system=${encodeURIComponent(system)}&first_hz=${hz}`);
      const T = await ask(o.sound.tuning), B = beside.value ? await ask(beside.value) : null;
      const n = T.degrees.length, fifthOn = new Map(T.fifths.map((d) => [d.from, d])), thirdOn = new Map(T.major_thirds.map((d) => [d.from, d]));
      const name = (i) => T.degrees[i].name || String(i);
      // The circle: from the first degree, each next a fifth above the last, until it returns.
      const order = [0];
      while (order.length < n) { const next = fifthOn.get(order.at(-1))?.to; if (next === undefined || order.includes(next)) break; order.push(next); }
      const closes = order.length === n;

      const W = 640, c = W / 2, R = 200, R3 = 144, step = 360 / order.length;
      const wheel = svg('svg', { class: 'wheel', viewBox: `0 0 ${W} ${W}`, role: 'img', 'aria-label': `The circle of fifths of ${T.name}` });
      const big = svg('text', { class: 'centre big', x: c, y: c - 2 }), small = svg('text', { class: 'centre small', x: c, y: c + 22 }), small2 = svg('text', { class: 'centre small', x: c, y: c + 40 });
      const scope = el('canvas', { class: 'scope', width: 1040, height: 172 });
      // What sounds is shown: the departure, the beats, and the swelling and fading the ear hears.
      const show = (d, what, p, q) => {
        const low = T.tones[d.from].hz, high = low * 2 ** (d.cents / 1200), beat = Math.abs(p * low - q * high);
        big.textContent = d.pure ? 'pure' : `${signed(d.departure)} cents`;
        small.textContent = `${what} on ${name(d.from)} · ${d.cents.toFixed(2)} cents`;
        small2.textContent = d.pure ? 'It does not beat.' : `It beats ${beat.toFixed(2)} times a second at this pitch.`;
        const g = scope.getContext('2d'), w = scope.width, h = scope.height, ink = getComputedStyle(scope).color;
        g.clearRect(0, 0, w, h);
        g.strokeStyle = ink;
        g.globalAlpha = 0.75;
        g.beginPath();
        for (let x = 0; x < w; x += 3) { const t = (x / w) * 2, env = Math.abs(Math.cos(Math.PI * beat * t)), y = env * (h / 2 - 8); g.moveTo(x, h / 2 - y); g.lineTo(x, h / 2 + y); }
        g.stroke();
        tone(low, 0, 1.9, 0.11);
        tone(high, 0, 1.9, 0.11);
      };
      const flash = (node, ms = 1700) => { node.classList.add('sounding'); timers.push(setTimeout(() => node.classList.remove('sounding'), ms)); };
      const segs = [], thirds = [], notes = [];
      order.forEach((d, k) => {
        const a0 = k * step, f = fifthOn.get(d), wolf = !f.pure && T.wolf && f.from === T.wolf.from && Math.abs(f.departure) > 12;
        if (k < order.length - 1 || closes) {
          const seg = svg('path', { class: 'seg', d: arc(c, c, R, a0 + 5.5, a0 + step - 5.5), stroke: tint(f), 'stroke-width': wolf ? 22 : 16 });
          seg.append(svg('title', {}, `The fifth on ${name(d)}: ${f.pure ? 'pure' : `${signed(f.departure)} cents from pure`}`));
          seg.addEventListener('click', () => { show(f, 'The fifth', 3, 2); flash(seg); });
          const mid = (a0 + step / 2) % 360, [lx, ly] = at(c, c, R + 24, mid);
          const anchor = mid > 20 && mid < 160 ? 'start' : mid > 200 && mid < 340 ? 'end' : 'middle';
          wheel.append(seg, svg('text', { class: `dep${wolf ? ' wolf' : ''}`, x: lx, y: ly + 4, style: `text-anchor:${anchor}` }, f.pure ? 'pure' : `${wolf ? 'the wolf ' : ''}${signed(f.departure)}`));
          segs.push([seg, f]);
        }
        const t = thirdOn.get(d);
        if (t) {
          const third = svg('path', { class: 'third', d: arc(c, c, R3, a0 - step / 2 + 3, a0 + step / 2 - 3), stroke: tint(t), 'stroke-width': 8 });
          third.append(svg('title', {}, `The major third on ${name(d)}: ${t.pure ? 'pure' : `${signed(t.departure)} cents from pure`}`));
          third.addEventListener('click', () => { show(t, 'The major third', 5, 4); flash(third); });
          wheel.append(third);
          thirds.push([third, t]);
        }
      });
      order.forEach((d, k) => {
        const [x, y] = at(c, c, R, k * step);
        const g = svg('g', { class: 'note', transform: `translate(${x.toFixed(1)} ${y.toFixed(1)})` });
        g.append(svg('circle', { r: order.length > 14 ? 13 : 18 }), svg('text', { y: 5 }, name(d)), svg('title', {}, `${name(d)}: ${T.tones[d].hz.toFixed(2)} Hz, ${T.degrees[d].form || `${T.degrees[d].cents.toFixed(1)} cents`}`));
        g.addEventListener('click', () => { tone(T.tones[d].hz, 0); flash(g, 500); });
        wheel.append(g);
        notes[d] = g;
      });
      big.textContent = `${n} to the octave`;
      small.textContent = 'Touch a note, a fifth on the outer ring,';
      small2.textContent = 'or a major third on the inner one.';
      wheel.append(big, small, small2);

      const round = (list, what, p, q) => list.forEach(([node, d], k) => timers.push(setTimeout(() => { show(d, what, p, q); flash(node); }, k * 2050)));
      const strings = T.degrees.map((d, i) => {
        const row = el('button', { type: 'button', class: 'string', title: 'The length of string that gives this degree, the whole string giving the first', onclick: () => { tone(T.tones[i].hz, 0); row.classList.add('sounding'); setTimeout(() => row.classList.remove('sounding'), 500); notes[i] && flash(notes[i], 500); } },
          el('span', {}, name(i)), el('span', {}, el('span', { class: 'len', style: `display:block;width:${(100 * 2 ** (-d.cents / 1200)).toFixed(2)}%` })), el('span', { class: 'f' }, `${d.form || `${d.cents.toFixed(1)} c`} · ${T.tones[i].hz.toFixed(1)} Hz`));
        return row;
      });
      const pureFifths = T.fifths.filter((d) => d.pure).length, pureThirds = T.major_thirds.filter((d) => d.pure).length;
      body.replaceChildren(el('div', { class: 'tun' },
        el('div', {}, wheel, scope, el('p', { class: 'silent', style: 'font-size:13px;margin:4px 0 0' }, 'Two seconds of what sounds: where the band narrows, the two tones cancel. A pure interval holds its width.'),
          el('div', { class: 'swatches' }, el('span', {}, el('i', { style: 'background:var(--good)' }), 'pure'), el('span', {}, el('i', { style: 'background:var(--accent)' }), 'narrow'), el('span', {}, el('i', { style: 'background:var(--accent-2)' }), 'wide'), el('span', {}, 'the stronger, the farther from pure'))),
        el('div', { class: 'sect' },
          el('p', { style: 'font-size:18px;font-weight:300;margin-top:0' }, `Of its fifths ${pureFifths} are pure, and of its major thirds ${pureThirds}. `,
            T.wolf && !T.wolf.pure ? `The fifth farthest from pure stands on ${name(T.wolf.from)}: ${signed(T.wolf.departure)} cents.` : 'Every fifth is pure.',
            closes ? '' : ' Its fifths do not lead round every degree, so the circle is drawn as far as they go.'),
          el('div', { class: 'row' }, el('button', { class: 'key strong', type: 'button', onclick: () => T.tones.forEach((t, i) => timers.push(setTimeout(() => { tone(t.hz, 0, 0.5); notes[t.degree] && flash(notes[t.degree], 300); strings[t.degree]?.classList.add('sounding'); setTimeout(() => strings[t.degree]?.classList.remove('sounding'), 300); }, i * 330))) }, 'Play the scale'),
            el('button', { class: 'key', type: 'button', onclick: () => round(segs, 'The fifth', 3, 2) }, 'Go round the fifths'), el('button', { class: 'key', type: 'button', onclick: () => round(thirds, 'The major third', 5, 4) }, 'And the thirds'),
            el('button', { class: 'plain', type: 'button', onclick: () => { timers.forEach(clearTimeout); timers = []; } }, 'stop')),
          el('h2', {}, 'The lengths of a string'), el('div', { class: 'strings' }, ...strings),
          B ? el('div', {}, el('h2', {}, `Beside ${B.name}`), B.degrees.length === n
            ? el('table', { class: 'measure' }, el('tr', {}, el('th', {}, 'Degree'), el('th', {}, 'here'), el('th', {}, 'there'), el('th', {}, 'there less here'), el('th', {}, '')),
              ...T.degrees.map((d, i) => el('tr', {}, el('td', {}, name(i)), el('td', {}, d.cents.toFixed(2)), el('td', {}, B.degrees[i].cents.toFixed(2)), el('td', {}, signed(B.degrees[i].cents - d.cents)),
                el('td', {}, el('button', { class: 'plain', type: 'button', onclick: () => { tone(T.tones[i].hz, 0, 0.8); tone(B.tones[i].hz, 0.9, 0.8); } }, 'one, then the other')))))
            : el('p', { class: 'silent' }, `They are not of as many degrees (${n} and ${B.degrees.length}), and cannot be set side by side degree by degree.`)) : null)));
    };
    first.addEventListener('change', draw);
    beside.addEventListener('change', draw);
    const host = el('div', {},
      el('div', { class: 'hero', style: 'padding-bottom:18px' }, el('div', { class: 'caption' }, `${ctx.glyphOf(o)} a tuning, heard and measured`), el('h1', { class: 'display' }, o.name), names ? el('div', { class: 'own' }, names) : null,
        o.summary ? el('p', { class: 'lead' }, o.summary) : null,
        el('div', { class: 'row', style: 'margin-top:18px' }, el('label', {}, 'The first degree at', first, 'Hz'), others.length ? el('label', {}, 'Set beside', beside) : null)),
      body);
    await draw();
    return host;
  },
};
