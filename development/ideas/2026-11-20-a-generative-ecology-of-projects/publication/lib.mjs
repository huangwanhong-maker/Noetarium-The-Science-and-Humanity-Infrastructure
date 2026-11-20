// Shared drawing helpers of the paper's figures.
//
// Every figure is drawn on a canvas 720 units wide and set at the full width of the text (162 mm),
// so that one unit is 0.225 mm and 0.64 pt. Type is never smaller than 11 units (7 pt). Figures carry
// formal labels and notation only; what explains them is in their captions.

export const W = 720;

export const C = {
  ink: '#241B2B', ink2: '#3F2D44', line: '#3F3247', mute: '#6A5A75', soft: '#B5A7BF', faint: '#DDD4E3',
  lilac: '#6C57A6', orchid: '#93497F', rose: '#B04A72', moss: '#4B7254', sand: '#8F7242', blue: '#4F78A8',
  tLilac: '#F0ECF6', tOrchid: '#F9F0F5', tRose: '#F5E2EA', tSand: '#F6F1E8', tMoss: '#ECF3ED',
  white: '#FFFFFF',
};
const strokes = { ink: C.line, lilac: C.lilac, orchid: C.orchid, rose: C.rose, moss: C.moss, sand: C.sand, mute: C.mute, blue: C.blue };

const marker = (id, color) =>
  `<marker id="${id}" viewBox="0 0 10 10" refX="8.6" refY="5" markerWidth="6.2" markerHeight="6.2" orient="auto-start-reverse"><path d="M0,1.2 L10,5 L0,8.8 z" fill="${color}"/></marker>`;

export function frame(w, h, label, body) {
  return `<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 ${w} ${h}" width="${w}" height="${h}" role="img" aria-label="${label}">
  <defs>
    ${Object.entries(strokes).map(([id, color]) => marker(id, color)).join('\n    ')}
    <style>
      text { font-family: 'Source Serif 4', Georgia, 'Cambria Math', 'STIX Two Text', serif; fill: ${C.ink}; }
    </style>
  </defs>
  <rect width="${w}" height="${h}" fill="#FFFFFF"/>
${body}
</svg>
`;
}

// Text. size in units; anchor start, middle or end; weight 400 or 600; italic for variables only.
// halo: a colour drawn round the letters, for a label that lies over a line.
export function T(x, y, s, o = {}) {
  const { size = 13, anchor = 'middle', weight = 400, italic = false, fill = C.ink, rotate = 0, halo = '' } = o;
  const tr = rotate ? ` transform="rotate(${rotate} ${x} ${y})"` : '';
  const hl = halo ? ` stroke="${halo}" stroke-width="4.5" stroke-linejoin="round" paint-order="stroke"` : '';
  return `  <text x="${x}" y="${y}" font-size="${size}" text-anchor="${anchor}"${weight !== 400 ? ` font-weight="${weight}"` : ''}${italic ? ' font-style="italic"' : ''}${fill !== C.ink ? ` fill="${fill}"` : ''}${hl}${tr}>${s}</text>`;
}

export function R(x, y, w, h, o = {}) {
  const { fill = C.white, stroke = C.line, sw = 1, rx = 2.5, dash = '' } = o;
  return `  <rect x="${x}" y="${y}" width="${w}" height="${h}" rx="${rx}" fill="${fill}" stroke="${stroke}" stroke-width="${sw}"${dash ? ` stroke-dasharray="${dash}"` : ''}/>`;
}

// A box with one line of text, or a title and a second line beneath it.
export function B(x, y, w, h, title, o = {}) {
  const { sub = '', size = 13, subsize = 11, weight = 400, ...box } = o;
  const cy = y + h / 2, out = [R(x, y, w, h, box)];
  if (sub) {
    out.push(T(x + w / 2, cy - 2.5, title, { size, weight }));
    out.push(T(x + w / 2, cy + 11.5, sub, { size: subsize, fill: C.ink2 }));
  } else out.push(T(x + w / 2, cy + size * 0.34, title, { size, weight }));
  return out.join('\n');
}

export function A(d, color = 'ink', o = {}) {
  const { w = 1.1, dash = '', start = false, end = true } = o;
  return `  <path d="${d}" fill="none" stroke="${strokes[color]}" stroke-width="${w}"${dash ? ` stroke-dasharray="${dash}" stroke-linecap="round"` : ''}${start ? ` marker-start="url(#${color})"` : ''}${end ? ` marker-end="url(#${color})"` : ''}/>`;
}

export const line = (x1, y1, x2, y2, o = {}) => {
  const { stroke = C.mute, w = 0.9, dash = '' } = o;
  return `  <line x1="${x1}" y1="${y1}" x2="${x2}" y2="${y2}" stroke="${stroke}" stroke-width="${w}"${dash ? ` stroke-dasharray="${dash}"` : ''}/>`;
};

export const sup = (base, s) => `${base}<tspan dy="-4.2" font-size="0.76em">${s}</tspan><tspan dy="4.2" font-size="0.01em"> </tspan>`;
export const sub = (base, s) => `${base}<tspan dy="3" font-size="0.76em">${s}</tspan><tspan dy="-3" font-size="0.01em"> </tspan>`;

// The letter of a panel.
export const panel = (x, y, letter) => T(x, y, letter, { size: 15, weight: 600, anchor: 'start' });

// Dotted and dashed patterns used with one meaning throughout: knowledge is dotted, what is
// conditional or outside is dashed.
export const DOT = '1.4 3.2', DASH = '5 3.5';
