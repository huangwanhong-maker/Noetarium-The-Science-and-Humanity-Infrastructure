// Music's activity: a tuning practised. The degrees, the fifths and the thirds are the musicology domain's, asked of the
// engine; the page only sounds them (a tone of a few partials, by the browser) and draws their departures from the pure
// intervals. The pitch of the first degree is the reader's to state.

let audio = null;
function tone(hz, at, seconds = 0.9, gain = 0.16) {
  audio ||= new AudioContext();
  const t = audio.currentTime + at, out = audio.createGain();
  out.gain.setValueAtTime(0, t);
  out.gain.linearRampToValueAtTime(gain, t + 0.02);
  out.gain.exponentialRampToValueAtTime(0.0008, t + seconds);
  out.connect(audio.destination);
  // A plucked-string-like tone: the first six partials, each weaker than the last, so that mistuned consonances beat.
  [1, 0.5, 0.33, 0.2, 0.12, 0.08].forEach((w, k) => {
    const osc = audio.createOscillator(), g = audio.createGain();
    osc.frequency.value = hz * (k + 1);
    g.gain.value = w;
    osc.connect(g).connect(out);
    osc.start(t);
    osc.stop(t + seconds + 0.05);
  });
}

const cents = (c) => `${c >= 0 ? '+' : '−'}${Math.abs(c).toFixed(2)}`;

export const render = {
  async practise(ctx) {
    const { el, object: o, api } = ctx;
    const host = el('div', {});
    const first = el('input', { type: 'text', value: sessionStorage.getItem('histos.first') || '256', size: 6, 'aria-label': 'The pitch of the first degree, in hertz' });
    const others = ctx.record.objects.filter((x) => x.sound?.tuning && x.id !== o.id && x.sound.tuning !== o.sound.tuning && ctx.inLayer(x.standing));
    const beside = el('select', {}, el('option', { value: '' }, 'nothing'), ...others.map((x) => el('option', { value: x.sound.tuning }, x.name)));
    const body = el('div', {});
    const draw = async () => {
      const hz = Number(first.value);
      if (!(hz > 20 && hz < 2000)) { body.replaceChildren(el('p', { class: 'msg' }, 'State the pitch of the first degree in hertz, between 20 and 2000.')); return; }
      sessionStorage.setItem('histos.first', first.value);
      const ask = (system) => api(`/api/tuning?system=${encodeURIComponent(system)}&first_hz=${hz}`);
      const T = await ask(o.sound.tuning), B = beside.value ? await ask(beside.value) : null;
      const n = T.degrees.length;
      const key = (t, i) => el('button', { type: 'button', title: `${t.hz.toFixed(2)} Hz · ${t.cents.toFixed(1)} cents${T.degrees[t.degree].form ? ` · ${T.degrees[t.degree].form}` : ''}`, onclick: (e) => { tone(t.hz, 0); flash(e.target); } }, T.degrees[t.degree].name || String(i));
      const flash = (b) => { b.classList.add('sounding'); setTimeout(() => b.classList.remove('sounding'), 500); };
      const keys = el('div', { class: 'keys' }, ...T.tones.map(key));
      const scale = () => T.tones.forEach((t, i) => { tone(t.hz, i * 0.32, 0.5); setTimeout(() => flash(keys.children[i]), i * 320); });
      const together = (d, k) => { const a = T.tones[d.from].hz, b = T.tones[d.from].hz * 2 ** (d.cents / 1200); tone(a, k, 1.9, 0.11); tone(b, k, 1.9, 0.11); };
      const widest = Math.max(1, ...T.fifths.map((d) => Math.abs(d.departure)), ...T.major_thirds.map((d) => Math.abs(d.departure)));
      const table = (name, dyads, wolf) => el('table', { class: 'measure' }, el('tr', {}, el('th', {}, `${name} on`), el('th', {}, 'cents'), el('th', {}, 'from pure'), el('th', {}, ''), el('th', {}, '')),
        ...dyads.map((d) => el('tr', { class: wolf && d.from === wolf.from ? 'wolf' : '' }, el('td', {}, T.degrees[d.from].name || d.from), el('td', {}, d.cents.toFixed(2)),
          el('td', { class: d.pure ? 'pure' : '' }, d.pure ? 'pure' : cents(d.departure)), el('td', {}, el('span', { class: `bar${d.departure < 0 ? ' neg' : ''}`, style: `width:${(Math.abs(d.departure) / widest * 110).toFixed(0)}px` })),
          el('td', {}, el('button', { class: 'key quiet', type: 'button', onclick: () => together(d, 0) }, 'hear')))));
      const pureFifths = T.fifths.filter((d) => d.pure).length, pureThirds = T.major_thirds.filter((d) => d.pure).length;
      body.replaceChildren(
        el('p', {}, el('b', {}, T.name), `: ${n} degrees to the octave. ${pureFifths} of its fifths and ${pureThirds} of its major thirds are pure. `,
          T.wolf && !T.wolf.pure ? `Its fifth farthest from pure stands on ${T.degrees[T.wolf.from].name || T.wolf.from}, ${cents(T.wolf.departure)} cents.` : ''),
        keys,
        el('div', { class: 'row' }, el('button', { class: 'key', type: 'button', onclick: scale }, 'Play the scale'),
          el('button', { class: 'key', type: 'button', onclick: () => T.fifths.forEach((d, k) => together(d, k * 2.1)) }, 'Play every fifth in turn'),
          el('button', { class: 'key', type: 'button', onclick: () => T.major_thirds.forEach((d, k) => together(d, k * 2.1)) }, 'Every major third')),
        el('p', { class: 'silent' }, 'Listen for the beating: a pure interval is still, a tempered one wavers, and the farther from pure the faster. The tone has six partials, so that the wavering can be heard.'),
        el('h2', {}, 'Its fifths'), table('A fifth', T.fifths, T.wolf), el('h2', {}, 'Its major thirds'), table('A third', T.major_thirds, null),
        B ? el('div', {}, el('h2', {}, `Beside ${B.name}`),
          B.degrees.length === n ? el('table', { class: 'measure' }, el('tr', {}, el('th', {}, 'Degree'), el('th', {}, 'here'), el('th', {}, 'there'), el('th', {}, 'there less here'), el('th', {}, '')),
            ...T.degrees.map((d, i) => el('tr', {}, el('td', {}, d.name || i), el('td', {}, d.cents.toFixed(2)), el('td', {}, B.degrees[i].cents.toFixed(2)), el('td', {}, cents(B.degrees[i].cents - d.cents)),
              el('td', {}, el('button', { class: 'key quiet', type: 'button', onclick: () => { tone(T.tones[i].hz, 0, 0.8); tone(B.tones[i].hz, 0.9, 0.8); } }, 'one, then the other')))))
            : el('p', { class: 'silent' }, `They are not of as many degrees (${n} and ${B.degrees.length}), and cannot be set side by side degree by degree.`)) : null,
        el('h2', {}, 'Its degrees'),
        el('table', { class: 'measure' }, el('tr', {}, el('th', {}, 'Degree'), el('th', {}, 'exactly'), el('th', {}, 'cents'), el('th', {}, 'Hz')),
          ...T.degrees.map((d, i) => el('tr', {}, el('td', {}, d.name || i), el('td', {}, d.form), el('td', {}, d.cents.toFixed(3)), el('td', {}, T.tones[i].hz.toFixed(2))))));
    };
    first.addEventListener('change', draw);
    beside.addEventListener('change', draw);
    host.append(el('div', { class: 'row' }, el('label', {}, 'First degree at', first, 'Hz'), others.length ? el('label', {}, 'Beside', beside) : null), body);
    await draw();
    return host;
  },
};
