// A look at one view of a project in headless Chrome, for working on a view (not in the suite): its pane's text, and a picture.
//   node applications/urban-studio/tests/look.mjs <base> <project> <view> <picture.jpg>
import { open } from './cdp.mjs';
import { tmpdir } from 'node:os';
import { join } from 'node:path';

const [BASE, project, view, picture] = process.argv.slice(2);
const page = await open(`${BASE}?project=${project}`, { port: 9398, dir: join(tmpdir(), `urban-look-${Date.now()}`), width: 1600, height: 1060 });
const sleep = (ms) => new Promise((r) => setTimeout(r, ms));
try {
  await page.until(`document.querySelector('.tab[data-tab="${view}"]')`, 60);
  await sleep(800);
  await page.eval(`document.querySelector('.tab[data-tab="${view}"]').click()`);
  await sleep(6000);
  console.log((await page.eval(`document.querySelector('#pane-${view}')?.innerText || '(no pane)'`)).slice(0, 1500));
  console.log('status:', await page.eval(`document.querySelector('#status, .status, .log')?.innerText?.slice(0, 600) || ''`));
  await page.shot(picture);
} finally {
  await page.close();
}
