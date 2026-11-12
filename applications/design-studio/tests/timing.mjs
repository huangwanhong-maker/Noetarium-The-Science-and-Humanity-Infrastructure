// How long one picture of the building takes in headless Chrome, in the round and walking: for finding what slows the view.
//   node applications/design-studio/tests/timing.mjs <a directory for the profile>
import { resolve } from 'node:path';
import { open } from './cdp.mjs';

const OUT = resolve(process.argv[2] || '.');
const page = await open('http://127.0.0.1:9050/', { dir: `${OUT}/chrome-timing`, port: 9353 });
try {
  await page.until('window.studio && window.studio.state.busy === false', 30);
  await page.eval('localStorage.clear(); location.reload()');
  await page.wait(800);
  await page.until('window.studio && window.studio.state.busy === false && window.studio.state.reading !== null', 30);
  for (const mode of ['solid', 'walk']) {
    await page.eval(`document.querySelector('[data-mode="${mode}"]').click()`);
    await page.until('studio.state.solid !== null', 60);
    await page.wait(9000);
    const times = await page.eval(`(() => { const v = studio.state.solid, out = []; for (let i = 0; i < 4; i++) { const t = performance.now(); v.renderer.render(v.scene, v.camera); v.renderer.getContext().finish(); out.push(Math.round(performance.now() - t)); } const info = v.renderer.info; return { plain: out, calls: info.render.calls, triangles: info.render.triangles, programs: info.programs.length, textures: info.memory.textures, geometries: info.memory.geometries }; })()`);
    console.log(mode, JSON.stringify(times));
  }
  console.log('errors', JSON.stringify(page.errors), JSON.stringify(page.logs).slice(0, 600));
} finally {
  await page.close();
}
