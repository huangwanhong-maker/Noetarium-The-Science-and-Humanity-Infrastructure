// A small driver of headless Chrome over the DevTools protocol, for looking at a page and asserting what it shows.
import { spawn } from 'node:child_process';
import { mkdirSync, writeFileSync } from 'node:fs';

// Chrome's program: CHROME in the environment, else where Windows keeps it.
const CHROME = process.env.CHROME || 'C:/Program Files/Google/Chrome/Application/chrome.exe';

export async function open(url, { port = 9351, dir, width = 1500, height = 940 } = {}) {
  mkdirSync(dir, { recursive: true });
  const chrome = spawn(CHROME, ['--headless=new', `--remote-debugging-port=${port}`, `--user-data-dir=${dir}`, `--window-size=${width},${height}`, '--hide-scrollbars',
    '--enable-unsafe-swiftshader', '--ignore-gpu-blocklist', '--no-first-run', '--no-default-browser-check', 'about:blank'], { stdio: 'ignore' });
  let target = null;
  for (let i = 0; i < 60 && !target; i++) {
    await new Promise((r) => setTimeout(r, 300));
    try { target = (await (await fetch(`http://127.0.0.1:${port}/json`)).json()).find((t) => t.type === 'page'); } catch { /* not up yet */ }
  }
  if (!target) { chrome.kill(); throw new Error('Chrome did not start'); }
  const ws = new WebSocket(target.webSocketDebuggerUrl);
  await new Promise((r, j) => { ws.onopen = r; ws.onerror = j; });
  let id = 0;
  const waiting = new Map(), errors = [], logs = [];
  ws.onmessage = (e) => {
    const m = JSON.parse(e.data);
    if (m.id && waiting.has(m.id)) { const { resolve, reject } = waiting.get(m.id); waiting.delete(m.id); m.error ? reject(new Error(m.error.message)) : resolve(m.result); }
    else if (m.method === 'Runtime.exceptionThrown') errors.push(m.params.exceptionDetails.exception?.description || m.params.exceptionDetails.text);
    else if (m.method === 'Runtime.consoleAPICalled' && (m.params.type === 'error' || m.params.type === 'warning')) logs.push(m.params.args.map((a) => a.value ?? a.description).join(' '));
    else if (m.method === 'Log.entryAdded' && m.params.entry.level === 'error') logs.push(`${m.params.entry.text} ${m.params.entry.url || ''}`);
  };
  const send = (method, params = {}) => new Promise((resolve, reject) => { const n = ++id; waiting.set(n, { resolve, reject }); ws.send(JSON.stringify({ id: n, method, params })); });
  await send('Runtime.enable'), await send('Page.enable'), await send('Log.enable');
  await send('Emulation.setDeviceMetricsOverride', { width, height, deviceScaleFactor: 1, mobile: false });
  await send('Page.navigate', { url });
  const page = {
    send, errors, logs,
    async eval(expression) {
      const r = await send('Runtime.evaluate', { expression, awaitPromise: true, returnByValue: true });
      if (r.exceptionDetails) throw new Error(r.exceptionDetails.exception?.description || r.exceptionDetails.text);
      return r.result.value;
    },
    async until(expression, seconds = 20) {
      for (let i = 0; i < seconds * 5; i++) { if (await page.eval(expression).catch(() => false)) return true; await new Promise((r) => setTimeout(r, 200)); }
      throw new Error(`never true: ${expression}`);
    },
    wait: (ms) => new Promise((r) => setTimeout(r, ms)),
    async mouse(type, x, y, more = {}) { await send('Input.dispatchMouseEvent', { type, x, y, button: 'left', buttons: type === 'mouseReleased' ? 0 : 1, clickCount: 1, ...more }); },
    async click(x, y) { await page.mouse('mouseMoved', x, y, { buttons: 0 }); await page.mouse('mousePressed', x, y); await page.mouse('mouseReleased', x, y); },
    async drag(x0, y0, x1, y1, steps = 6) {
      await page.mouse('mouseMoved', x0, y0, { buttons: 0 }); await page.mouse('mousePressed', x0, y0);
      for (let i = 1; i <= steps; i++) await page.mouse('mouseMoved', x0 + (x1 - x0) * i / steps, y0 + (y1 - y0) * i / steps);
      await page.mouse('mouseReleased', x1, y1);
    },
    async key(key, more = {}) { await send('Input.dispatchKeyEvent', { type: 'keyDown', key, ...more }); await send('Input.dispatchKeyEvent', { type: 'keyUp', key, ...more }); },
    async shot(file, clip) {
      const r = await send('Page.captureScreenshot', { format: file.endsWith('.jpg') ? 'jpeg' : 'png', quality: 88, ...(clip ? { clip: { ...clip, scale: 1 } } : {}) });
      writeFileSync(file, Buffer.from(r.data, 'base64'));
    },
    async close() { try { await send('Browser.close'); } catch { /* gone */ } chrome.kill(); },
  };
  return page;
}
