// Checks of critical transitions drawn (visualization/js/dynamics/transitions.js, ADR 0135): a verdict told by a glyph and a word;
// an indicator's windows with the first and last ringed and joined by the change tested, what is not a number left out; the
// dominant eigenvalue's path against the unit circle, the place it goes to marked, conjugates drawn faint; the tests' operating
// characteristics, a detection rate as a bar of its length and the false-alarm rate as a tick; the grid of windows and bandwidths
// with significant cells filled and reversals crossed; where the mode shows, bars in proportion; a landscape's wells marked; names
// escaped; what cannot be drawn said. Called by run-tests.mjs with the suite's check and assert.

import { dynamics as D } from '../js/index.js';

const count = (text, part) => text.split(part).length - 1;

export function transitionsChecks({ assert, check }) {
  // ---- verdicts ----
  {
    const s = D.verdictMark('signal'), n = D.verdictMark('no signal'), u = D.verdictMark('undecidable');
    assert('transitions drawn: a signal is a filled disc and its word', s.includes('tr-glyph tr-signal') && s.includes('>signal<') && !s.includes('tr-none'));
    assert('transitions drawn: no signal is an open ring and its words', n.includes('tr-glyph tr-none') && n.includes('>no signal<') && !n.includes('tr-signal'));
    assert('transitions drawn: undecidable is a half-filled disc and its word', u.includes('tr-none') && u.includes('tr-signal') && u.includes('A5.5') && u.includes('>undecidable<'));
    assert('transitions drawn: an unknown verdict is drawn as undecidable', D.verdictMark('maybe').includes('>undecidable<'));
  }

  // ---- an indicator through its windows ----
  {
    const finding = { indicator: 'variance <of x>', at: [1, 2, 3, 4], series: [1, null, 2, 4], change: Math.log(4), positive: true, verdict: 'signal', p: 0.012 };
    const svg = D.findingChart(finding, { width: 420, height: 210 });
    assert('transitions drawn: the first and the last window are ringed', count(svg, 'class="tr-end"') === 2);
    assert('transitions drawn: and joined by the change the test is of', count(svg, 'class="tr-change"') === 1);
    const series = svg.match(/class="tr-series" d="([^"]*)"/)[1];
    assert('transitions drawn: a window with no value is left out of the line', count(series, 'L') === 2 && series.startsWith('M'), series);
    assert('transitions drawn: the verdict and the change are written above the chart', svg.includes('>signal<') && svg.includes('log ratio 1.386') && svg.includes('p = 0.012'));
    assert('transitions drawn: the indicator\'s name is escaped', svg.includes('variance &lt;of x&gt;') && !svg.includes('<of x>'));
    assert('transitions drawn: too few windows are said, not drawn', D.findingChart({ indicator: 'v', at: [1], series: [2] }).includes('too few windows'));
  }

  // ---- the eigenvalue's path ----
  {
    const kind = { path: [[0.5, 0.3], [0.7, 0.4], [0.85, 0.45]], approach: 'oscillatory', described: 'towards the circle' };
    const svg = D.eigenPath(kind, { width: 320, height: 300 });
    assert('transitions drawn: the unit circle is drawn', count(svg, 'class="tr-circle"') === 1);
    assert('transitions drawn: the three places a loss of stability goes are marked and named', count(svg, '<circle class="tr-place') === 3 && svg.includes('+1: fold') && svg.includes('−1: period-doubling') && svg.includes('Hopf'));
    assert('transitions drawn: the place the path goes to is the one filled', count(svg, 'tr-place tr-reached') === 1 && svg.indexOf('tr-reached') > svg.indexOf('period-doubling'));
    assert('transitions drawn: every window\'s eigenvalue and its faint conjugate', count(svg, 'class="tr-step"') === 3 && count(svg, 'tr-step tr-conjugate') === 3);
    const op = [...svg.matchAll(/class="tr-step" cx="[^"]*" cy="[^"]*" r="([^"]*)" opacity="([^"]*)"/g)].map((m) => [Number(m[1]), Number(m[2])]);
    assert('transitions drawn: the path is faint where it began and full and larger where it ends', op[0][1] < op[2][1] && op[2][1] === 1 && op[2][0] > op[0][0], JSON.stringify(op));
    const real = D.eigenPath({ path: [[0.9, 0]], approach: 'stationary' });
    assert('transitions drawn: a real eigenvalue has no conjugate drawn', !real.includes('tr-conjugate'));
  }

  // ---- operating characteristics ----
  {
    const findings = [
      { channel: 'the record', indicator: 'variance', verdict: 'signal', p: 0.01, false_alarm: 0.05, detection: 0.5, auc: 0.9 },
      { channel: 'district: totals', indicator: 'leading propagator modulus', verdict: 'undecidable', p: 0.3, false_alarm: 0.05, detection: 1, auc: 0.5 },
    ];
    const svg = D.operating(findings, { width: 760, row: 22 });
    const bars = [...svg.matchAll(/class="tr-bar" x="[^"]*" y="[^"]*" width="([^"]*)"/g)].map((m) => Number(m[1]));
    check('transitions drawn: a detection rate of one half is a bar of half the track', bars[0] / bars[1], 0.5, 1e-9);
    assert('transitions drawn: each test has its false-alarm tick and its ROC dot', count(svg, 'class="tr-alarm"') === 2 && count(svg, 'class="tr-auc"') === 2);
    assert('transitions drawn: a channel other than the record is named before its indicator', svg.includes('district: totals · leading propagator modulus'));
    assert('transitions drawn: each verdict by glyph and word', svg.includes('>signal<') && svg.includes('>undecidable<'));
  }

  // ---- the grid ----
  {
    const grid = [
      { window: 0.3, bandwidth: 0.1, p: 0.01, p_reverse: 0.9, change: 1 }, { window: 0.5, bandwidth: 0.1, p: 0.2, p_reverse: 0.6, change: 0.2 },
      { window: 0.3, bandwidth: 0.2, p: 0.9, p_reverse: 0.02, change: -1 }, { window: 0.5, bandwidth: 0.2, p: 0.04, p_reverse: 0.97, change: 0.8 },
    ];
    const svg = D.gridMap({ indicator: 'variance', grid }, { cell: 40 });
    assert('transitions drawn: a cell for each window and bandwidth', count(svg, 'class="tr-cell') === 4);
    assert('transitions drawn: the significant cells filled', count(svg, 'tr-cell tr-significant') === 2);
    assert('transitions drawn: the cell significant the other way crossed', count(svg, 'class="tr-reverse"') === 1);
    assert('transitions drawn: the grid says what its marks mean', svg.includes('crossed: significant the other way'));
    assert('transitions drawn: no grid is said, not drawn', D.gridMap({ indicator: 'v', grid: [] }).includes('no grid'));
  }

  // ---- where, and wells ----
  {
    const where = { units: ['north', 'south <b>'], loading: [0.75, 0.25], excitation: [0.5, 0.5], sentence: 'north' };
    const svg = D.whereBars(where, { width: 640, height: 220 });
    const heights = [...svg.matchAll(/class="tr-load" x="[^"]*" y="[^"]*" width="[^"]*" height="([^"]*)"/g)].map((m) => Number(m[1]));
    check('transitions drawn: where the mode shows, bars in proportion to its share', heights[0] / heights[1], 3, 1e-3);
    assert('transitions drawn: what excites it outlined, one for each unit', count(svg, 'class="tr-excite"') === 2);
    assert('transitions drawn: a unit\'s name escaped', svg.includes('south &lt;b&gt;') && !svg.includes('south <b>'));
    const wells = D.wellsChart({ grid: [-1, 0, 1], landscape: [1, 2, 1], fitted: [1.1, 1.9, 1.1], minima: [-1, 1], wells: 2, switches: 7 });
    assert('transitions drawn: a landscape with its two wells marked', count(wells, 'class="d-mark"') === 2 && wells.includes('2 wells, 7 switches'));
  }
}
