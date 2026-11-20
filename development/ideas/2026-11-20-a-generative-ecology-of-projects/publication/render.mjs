// Makes a PDF (vector, the page sized to the figure) and a PNG at three times the size (2160 px wide, 339 dpi at 162 mm) from each SVG beside
// this file, with headless Chrome and Source Serif 4 from Google Fonts.
//
//   node render.mjs            every figure
//   node render.mjs 3 5        figures 3 and 5
//   CHROME=/path/to/chrome node render.mjs
//
// Wrapper pages and Chrome's profile are kept in the system's temporary folder, never beside the figures.

import { readdirSync, readFileSync, writeFileSync, mkdirSync, existsSync } from 'node:fs';
import { execFileSync } from 'node:child_process';
import { fileURLToPath, pathToFileURL } from 'node:url';
import { dirname, join } from 'node:path';
import { tmpdir } from 'node:os';

const here = dirname(fileURLToPath(import.meta.url));
const candidates = [
  process.env.CHROME,
  'C:/Program Files/Google/Chrome/Application/chrome.exe',
  'C:/Program Files (x86)/Microsoft/Edge/Application/msedge.exe',
  '/usr/bin/google-chrome', '/usr/bin/chromium', '/usr/bin/chromium-browser',
  '/Applications/Google Chrome.app/Contents/MacOS/Google Chrome',
].filter(Boolean);
const chrome = candidates.find((p) => existsSync(p));
if (!chrome) { console.error('no Chrome found; set CHROME'); process.exit(1); }

const work = join(process.env.FIGURE_WORK || tmpdir(), 'noetarium-paper-figures');
mkdirSync(join(work, 'profile'), { recursive: true });

const want = process.argv.slice(2).map(Number).filter(Boolean);
const svgs = readdirSync(here).filter((f) => /^fig-\d+-.*\.svg$/.test(f)).sort();
for (const file of svgs) {
  const n = Number(file.match(/^fig-(\d+)-/)[1]);
  if (want.length && !want.includes(n)) continue;
  const svg = readFileSync(join(here, file), 'utf8');
  const [, w, h] = svg.match(/viewBox="0 0 (\d+) (\d+)"/);
  const page = join(work, file.replace(/\.svg$/, '.html'));
  writeFileSync(page, `<!doctype html><html><head><meta charset="utf-8">
<link rel="preconnect" href="https://fonts.googleapis.com"><link rel="preconnect" href="https://fonts.gstatic.com" crossorigin>
<link href="https://fonts.googleapis.com/css2?family=Source+Serif+4:ital,opsz,wght@0,8..60,400;0,8..60,600;1,8..60,400;1,8..60,600&display=swap" rel="stylesheet">
<style>@page { size: ${w}px ${h}px; margin: 0 } html, body { margin: 0; background: #fff } svg { display: block }</style>
</head><body>${svg}</body></html>`);
  const url = pathToFileURL(page).href;
  const common = ['--headless=new', '--disable-gpu', '--hide-scrollbars', '--no-first-run', `--user-data-dir=${join(work, 'profile')}`, '--virtual-time-budget=6000'];
  const base = join(here, file.replace(/\.svg$/, ''));
  execFileSync(chrome, [...common, '--force-device-scale-factor=3', `--window-size=${w},${h}`, `--screenshot=${base}.png`, url], { stdio: 'ignore' });
  execFileSync(chrome, [...common, '--no-pdf-header-footer', `--print-to-pdf=${base}.pdf`, url], { stdio: 'ignore' });
  console.log('rendered', file);
}
