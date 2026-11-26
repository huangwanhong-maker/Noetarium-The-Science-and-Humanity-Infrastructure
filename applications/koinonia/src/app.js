// Koinonia's page (ADR 0169). The society is drawn with one community in focus, large enough to read: its seats, the agents beside
// those who lent them, its commitments; the other communities are small rounds to choose, with the services between them as threads.
// The story tells what happened in sentences, round by round; the journal shows every participant's recorded behaviour, sealed and
// verifiable. A person takes part from here, through the community's own rules. What is chosen opens where it stands.

const NS = "http://www.w3.org/2000/svg";
const $ = (id) => document.getElementById(id);
const esc = (t) => String(t ?? "").replace(/[&<>"]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
const S = { state: null, name: "", view: null, focus: "", chosen: null, theme: "all", who: "", tab: "story", fresh: new Set(), seenUpTo: 0, polling: false, verified: null };
const ORDERS = { commons: "an open commons", guild: "a guild entered by trial", cooperative: "a cooperative of members, one vote an origin",
  polity: "a polity of citizens", "first-league": "a league of communities", league: "a league of communities" };
const THEMES = [["all", "everything"], ["work", "work"], ["findings", "findings"], ["decisions", "decisions"], ["circulation", "between communities"],
  ["words", "words"], ["membership", "membership"], ["refused", "refusals"]];
const GLYPH = { work: "·", findings: "✓", decisions: "◆", circulation: "⇄", words: "❝", membership: "○", refused: "✕", note: "❝", agent: "…", clock: "◷", settled: "✓", adjudicated: "✓" };

async function api(path, body) {
  const r = await fetch(path, body === undefined ? {} : { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) });
  const data = await r.json().catch(() => ({}));
  if (!r.ok) throw new Error(data.error || r.statusText);
  return data;
}
function say(text) { const n = $("say"); n.textContent = text; n.classList.add("on"); clearTimeout(say.t); say.t = setTimeout(() => n.classList.remove("on"), 4200); }
function el(tag, attrs = {}, parent) {
  const n = document.createElementNS(NS, tag);
  for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, v);
  if (parent) parent.appendChild(n);
  return n;
}
const labelOf = (id) => S.view?.agents.find((a) => a.id === id)?.label || S.view?.parties.find((p) => p.id === id)?.label || id;
const entryName = (e) => S.view?.entries?.[e]?.name || e;

// ---- glyphs: a person a circle, an organisation a square, an agent a diamond, a community a hexagon ---------------------------------
function glyph(kind, r, parent) {
  if (kind === "organisation") return el("rect", { class: "glyph", x: -r, y: -r, width: 2 * r, height: 2 * r }, parent);
  if (kind === "operative") return el("path", { class: "glyph", d: `M0 ${-r * 1.2} L${r * 1.1} 0 L0 ${r * 1.2} L${-r * 1.1} 0 Z` }, parent);
  if (kind === "community") return el("path", { class: "glyph", d: Array.from({ length: 6 }, (_, i) => `${i ? "L" : "M"}${(r * Math.cos(Math.PI / 3 * i)).toFixed(1)} ${(r * Math.sin(Math.PI / 3 * i)).toFixed(1)}`).join(" ") + "Z" }, parent);
  return el("circle", { class: "glyph", r }, parent);
}
function legend() {
  const box = $("legend"); box.innerHTML = "";
  const add = (draw, word) => { const s = document.createElement("span"); const svg = document.createElementNS(NS, "svg"); svg.setAttribute("viewBox", "-8 -8 16 16"); draw(svg); s.append(svg, word); box.append(s); };
  for (const [kind, word, cls] of [["person", "a person", ""], ["organisation", "an organisation", ""], ["community", "a community", ""], ["operative", "an agent by rule", "rule"], ["operative", "an agent by model", "model"]])
    add((svg) => glyph(kind, 5, el("g", { class: `seat ${kind} ${cls}` }, svg)), word);
  for (const [cls, word] of [["conditional", "offered"], ["active", "taken"], ["presented", "delivered"], ["discharged", "accepted"], ["released", "released"]])
    add((svg) => el("circle", { class: `c-dot ${cls}`, r: 4 }, svg), word);
}

// ---- the society, one community in focus ---------------------------------------------------------------------------------------------
function focusName() {
  const v = S.view;
  if (!v?.communities.length) return "";
  if (S.focus && v.communities.find((c) => c.name === S.focus)) return S.focus;
  return v.communities.filter((c) => !c.preset.includes("league")).sort((a, b) => b.acts - a.acts)[0]?.name || v.communities[0].name;
}
function drawSociety() {
  const svg = $("rounds"); svg.innerHTML = "";
  const v = S.view, w = svg.clientWidth || 900, h = svg.clientHeight || 520;
  svg.setAttribute("viewBox", `0 0 ${w} ${h}`);
  if (!v) { el("text", { class: "empty", x: w / 2, y: h / 2 }, svg).textContent = "Begin a society with New society."; $("focus-head").innerHTML = ""; return; }
  if (!v.communities.length) { el("text", { class: "empty", x: w / 2, y: h / 2 }, svg).textContent = "This society has no community yet: found one."; return; }
  const defs = el("defs", {}, svg), mk = el("marker", { id: "arrow", viewBox: "0 0 10 10", refX: 9, refY: 5, markerWidth: 7, markerHeight: 7, orient: "auto-start-reverse" }, defs);
  el("path", { d: "M0 0 L10 5 L0 10 z", fill: "var(--rose)" }, mk);
  const focus = focusName(), c = v.communities.find((x) => x.name === focus);
  const others = v.communities.filter((x) => x.name !== focus);
  const rowY = h - 44, bigR = Math.max(90, Math.min(w * 0.3, (h - 150) * 0.42)), cx = w / 2, cy = (h - 92) / 2 + 16;
  const at = { [focus]: { x: cx, y: cy, r: bigR } };
  others.forEach((o, i) => { at[o.name] = { x: w * (i + 1) / (others.length + 1), y: rowY, r: 26 }; });
  // what passed between communities, from the league's account
  for (const [pair, count] of Object.entries(v.circulation?.["by pair"] || {})) {
    const [from, to] = pair.split(">"), a = at[from], b = at[to];
    if (!a || !b) continue;
    const dx = b.x - a.x, dy = b.y - a.y, len = Math.hypot(dx, dy) || 1, ux = dx / len, uy = dy / len;
    const sx = a.x + ux * (a.r + 6), sy = a.y + uy * (a.r + 6), ex = b.x - ux * (b.r + 8), ey = b.y - uy * (b.r + 8);
    // a thread between two small rounds (neither the community in focus) arches low above the row, one direction higher than the
    // other, drawn faint and told by its title; a thread up to the ring bends to one side and carries its count as a label
    const row = a.r < 40 && b.r < 40, words = `${count} ${count === 1 ? "service" : "services"}`;
    const bend = row ? 0 : (from < to ? 1 : -1) * Math.min(60, len * 0.18);
    const mx = row ? (sx + ex) / 2 : (sx + ex) / 2 - uy * bend;
    const my = row ? (sy + ey) / 2 - (16 + len * (dx > 0 ? 0.08 : 0.16)) : (sy + ey) / 2 + ux * bend;
    const path = el("path", { class: row ? "thread faint" : "thread", d: `M${sx.toFixed(1)} ${sy.toFixed(1)} Q${mx.toFixed(1)} ${my.toFixed(1)} ${ex.toFixed(1)} ${ey.toFixed(1)}`, "marker-end": "url(#arrow)" }, svg);
    el("title", {}, path).textContent = `${words} from ${from} to ${to}`;
    if (row) continue;
    // the label sits on the thread itself, haloed, nearer its start, away from the seats where the ring is met
    const t = 0.36, q = (u, m, v) => (1 - t) * (1 - t) * u + 2 * t * (1 - t) * m + t * t * v;
    el("text", { class: "thread-label", x: q(sx, mx, ex).toFixed(1), y: (q(sy, my, ey) - 5).toFixed(1) }, svg).textContent = words;
  }
  // the small rounds: choose one to bring it into focus
  for (const o of others) {
    const p = at[o.name], g = el("g", { transform: `translate(${p.x},${p.y})` }, svg);
    const ring = el("circle", { class: "ring small", r: p.r }, g);
    glyph("community", 7.5, el("g", { class: "seat community" }, g));
    const t = el("text", { class: "small-name", y: p.r + 17 }, g); t.textContent = o.name;
    el("text", { class: "small-sub", y: p.r + 31 }, g).textContent = `${o.acts} acts`;
    for (const n of [ring, t]) n.addEventListener("click", () => { S.focus = o.name; S.chosen = null; $("card").hidden = true; render(); });
    el("title", {}, ring).textContent = `${o.name}: ${ORDERS[o.preset] || o.preset}. Choose to bring it into focus.`;
  }
  // the community in focus
  const g = el("g", { transform: `translate(${cx},${cy})` }, svg);
  el("circle", { class: "ring", r: bigR }, g);
  el("circle", { class: "ring inner", r: bigR * 0.6 }, g);
  const cs = c.commitments.slice(-60);
  cs.forEach((k, j) => {
    const t = j * 0.6, rad = 10 + (bigR * 0.52 - 10) * (j / Math.max(1, cs.length - 1));
    const dot = el("circle", { class: `c-dot ${k.status}`, cx: (rad * Math.cos(t)).toFixed(1), cy: (rad * Math.sin(t)).toFixed(1), r: 3.8 }, g);
    el("title", {}, dot).textContent = `${labelOf(k.by)}: ${k.status} (${k.matter}), owed by ${labelOf(k.debtor)} to ${labelOf(k.creditor)}`;
  });
  el("text", { class: "center-note", y: bigR * 0.6 + 16 }, g).textContent = `${c.acts} acts sealed · ${c.refused} refused`;
  const parties = Object.fromEntries(v.parties.map((p) => [p.id, p]));
  const holders = [];
  for (const p of c.positions) { const h0 = holders.find((x) => x.party === p.party); h0 ? h0.positions.push(p.position) : holders.push({ party: p.party, positions: [p.position] }); }
  const lent = v.agents.filter((a) => a.communities[c.name]);
  const seats = holders.filter((h0) => parties[h0.party]?.kind !== "operative");
  for (const a of lent) if (!seats.find((s0) => s0.party === a.sponsor)) seats.push({ party: a.sponsor, positions: ["lends an agent"] });
  seats.forEach((s0, k) => {
    const p = parties[s0.party] || { kind: "person", label: s0.party }, a = -Math.PI / 2 + (2 * Math.PI * k) / seats.length;
    const sx = bigR * Math.cos(a), sy = bigR * Math.sin(a);
    const mine = lent.filter((ag) => ag.sponsor === s0.party);
    mine.forEach((ag, m) => {
      const spread = (m - (mine.length - 1) / 2) * 0.22, ra = bigR + 46 + (m % 2) * 16, ax = ra * Math.cos(a + spread), ay = ra * Math.sin(a + spread);
      el("line", { class: "tie", x1: sx, y1: sy, x2: ax, y2: ay }, g);
      const short = ag.kind === "reading" ? "reader" : ag.role === "checker" ? `${ag.policy} checker` : ag.role;
      const revoked = (c.mandates || []).find((x) => x.id === ag.communities[c.name].mandate && x.revoked);
      seat(g, { id: ag.id, kind: "operative", label: short, sub: "", cls: `${ag.policy}${ag.paused ? " paused" : ""}${revoked ? " revoked" : ""}` }, ax, ay, a + spread, `${ag.label} (${ag.role}, by ${ag.policy})`);
    });
    seat(g, { id: s0.party, kind: p.kind, label: p.label, sub: s0.positions.join(" · "), cls: "" }, sx, sy, a, `${p.label}: ${s0.positions.join(", ")}`);
  });
}
function seat(g, s0, x, y, a, title) {
  const chosen = S.chosen && S.chosen.id === s0.id;
  const node = el("g", { class: `seat ${s0.kind} ${s0.cls}${chosen ? " chosen" : ""}${S.fresh.has(s0.id) ? " acted" : ""}`, transform: `translate(${x.toFixed(1)},${y.toFixed(1)})`, tabindex: 0 }, g);
  el("circle", { class: "pulse", r: 6 }, node);
  glyph(s0.kind, s0.kind === "operative" ? 4.6 : 6.5, node);
  const right = Math.cos(a) >= -0.15, dx = right ? 13 : -13, anchor = right ? "start" : "end";
  el("text", { class: "label", x: dx, y: s0.sub ? 0 : 4.5, "text-anchor": anchor }, node).textContent = s0.label;
  if (s0.sub) el("text", { class: "sub", x: dx, y: 15, "text-anchor": anchor }, node).textContent = s0.sub;
  el("title", {}, node).textContent = title;
  node.addEventListener("click", (e) => open({ type: s0.kind === "operative" ? "agent" : "party", id: s0.id }, e));
}
function focusHead() {
  const v = S.view, c = v?.communities.find((x) => x.name === focusName());
  if (!c) { $("focus-head").innerHTML = ""; $("plate-caption").innerHTML = ""; return; }
  const circ = v.circulation && c.preset.includes("league") ? `, keeping services by ${esc(v.circulation["in force"])} with ${v.circulation["debts outstanding"]} debts outstanding` : "";
  $("focus-head").innerHTML = `<h2>${esc(c.name)}</h2><span class="order">${esc(ORDERS[c.preset] || c.preset)}${circ}</span><p class="mission">${esc(c.mission)}</p>`;
  $("focus-head").querySelector("h2").onclick = (e) => open({ type: "community", id: c.name }, e);
  const others = v.communities.filter((x) => x.name !== c.name).map((x) => x.name);
  $("plate-caption").innerHTML = `<b>Plate.</b> The ${esc(c.name)} at round ${v.turn}: its seats on the ring, the agents beside those who lent them, its commitments spiralling inward.`
    + (others.length ? ` Beneath, ${esc(others.join(", "))}; choose one to bring it forward.` : "");
}

// ---- the weave of the work, one row a community that owns work ------------------------------------------------------------------
function weave() {
  const box = $("weave"), v = S.view;
  box.innerHTML = "";
  if (!v?.batches?.length) return;
  const t = v.work, findings = v.findings || {};
  box.innerHTML = `<h3>The work</h3><div class="tally">The catalogue's entries, each checked twice: ${t.holds} hold, ${t.fault} at fault, ${t.open} in doubt, ${t.unchecked} not yet checked; ${t.agreed || 0} agreed by two checks, ${t.adjudicated || 0} settled by the guild.</div>`;
  const owners = [...new Set(v.batches.map((b) => b.owner))];
  for (const owner of owners) {
    const row = document.createElement("div"); row.className = "row";
    row.innerHTML = `<span class="owner">${esc(owner)}</span>`;
    const cells = document.createElement("div"); cells.className = "cells";
    for (const b of v.batches.filter((x) => x.owner === owner)) {
      const grp = document.createElement("div"); grp.className = "batch"; grp.title = `batch ${b.id}` + (b.settled ? `, settled in round ${b.settled}` : ", not yet settled");
      for (const e of b.entries) {
        const f = findings[e], cls = !f ? "" : f.finding === "holds" ? "holds" : f.finding === "fault" ? "fault" : "open";
        const cell = document.createElement("button"); cell.type = "button";
        cell.className = `cell ${cls}${f?.adjudication?.grade === "a person" ? " person" : ""}${S.chosen?.id === e ? " chosen" : ""}`;
        cell.setAttribute("aria-label", `${entryName(e)}: ${f ? f.finding : "not yet checked"}`);
        cell.title = `${entryName(e)}: ${f ? f.finding + (f.how === "adjudicated" ? ", settled by the guild" : ", agreed by two checks") : "not yet checked"}`;
        cell.addEventListener("click", (ev) => open({ type: "entry", id: e }, ev));
        grp.append(cell);
      }
      cells.append(grp);
    }
    row.append(cells); box.append(row);
  }
  const key = document.createElement("div"); key.className = "key";
  key.innerHTML = `<span><i class="cell" style="display:inline-block;width:10px;height:10px;border-radius:50%;border:1px solid var(--hair-strong)"></i>not yet checked</span>`
    + `<span><i style="border-radius:50%;background:var(--lichen)"></i>holds</span>`
    + `<span><i style="background:linear-gradient(45deg,transparent 43%,var(--brick) 43%,var(--brick) 57%,transparent 57%),linear-gradient(-45deg,transparent 43%,var(--brick) 43%,var(--brick) 57%,transparent 57%)"></i>at fault</span>`
    + `<span><i style="border-radius:50%;border:1px solid var(--amber);background:linear-gradient(90deg,var(--amber) 50%,transparent 50%)"></i>in doubt</span>`
    + `<span><i style="border-radius:50%;box-shadow:0 0 0 1px var(--rose)"></i>settled by a person</span>`;
  box.append(key);
}

// ---- the story: sentences, round by round ------------------------------------------------------------------------------------------
function chips() {
  $("chips").innerHTML = THEMES.map(([k, word]) => `<button class="chip${S.theme === k ? " on" : ""}" type="button" data-theme="${k}">${esc(word)}</button>`).join("");
  $("chips").querySelectorAll(".chip").forEach((b) => (b.onclick = () => { S.theme = b.dataset.theme; chips(); story(); }));
  $("story-who").textContent = S.who ? `only ${labelOf(S.who)} ✕` : "";
  $("story-who").onclick = () => { S.who = ""; chips(); story(); journal(); };
}
function story() {
  const v = S.view, box = $("story");
  if (!v) { box.innerHTML = ""; return; }
  const themeOf = (e) => (e.kind === "note" ? "words" : e.kind === "settled" || e.kind === "adjudicated" ? "findings" : e.kind);
  const keep = v.events.filter((e) => (S.theme === "all" || themeOf(e) === S.theme) && (!S.who || e.who === S.who || e.text.includes(labelOf(S.who))));
  let html = "", last = null;
  for (const e of keep.slice().reverse()) {
    if (e.turn !== last) { html += `<div class="round-head">Round ${e.turn}</div>`; last = e.turn; }
    const th = themeOf(e);
    html += `<div class="line ${esc(th)}${e.n > S.seenUpTo ? " fresh" : ""}"><span class="g">${GLYPH[th] || "·"}</span><span class="t">${esc(e.text)} ${e.community ? `<span class="where">in ${esc(e.community)}</span>` : ""}</span></div>`;
  }
  box.innerHTML = html || `<p class="where">Nothing of this kind has happened yet.</p>`;
}

// ---- the journal: every participant's recorded behaviour ------------------------------------------------------------------------
function describe(e) {
  const d = e.data || {};
  const who = `<b>${esc(labelOf(e.who))}</b>`;
  switch (e.what) {
    case "wake": return [`${who} woke: ${esc(d.why)}`, ""];
    case "decide": {
      const acts = (d.acts || []).map((a) => `${a.kind}${a.item ? " " + a.item.split("~")[0] : ""} in ${a.community}`).join("; ");
      const chose = d.chose ? ` · chose ${esc((d.chose.chose || []).join(", ") || "nothing")}: <i>${esc(d.chose.why)}</i>` : "";
      return [`${who} decided: ${esc(acts || "nothing to do")}${chose}`, ""];
    }
    case "ask": {
      const tok = d.tokens ? (d.tokens.input_tokens || 0) + (d.tokens.output_tokens || 0) : 0;
      const head = `${who} asked ${esc(d.model)} (${esc(e.about)}): ${esc(d.outcome)}${tok ? ` · ${tok} tokens` : ""}${d.usd ? ` · ${(d.usd * 100).toFixed(2)}¢` : ""}${d.reason ? ` · ${esc(d.reason)}` : ""}`;
      return [head, `<details><summary>what was asked and what came back</summary><pre>${esc(d.prompt)}</pre><pre>${esc(JSON.stringify(d.answer, null, 1))}</pre><div class="seal">instructions ${esc((d.instructions || "").slice(0, 16))}</div></details>`];
    }
    case "act": return [`${who} ${esc(d.kind)} in ${esc(e.about)}: admitted${d.via === "page" ? ", from the page" : ""} <span class="seal">${esc((d.seal || "").slice(0, 12))}</span>`,
      d.body || d.carried ? `<details><summary>what it carried</summary><pre>${esc(d.body || JSON.stringify(d.carried, null, 1))}</pre></details>` : ""];
    case "refused": return [`${who} ${esc(d.kind)} in ${esc(e.about)}: <span style="color:var(--brick)">refused</span>, ${esc(d.reason)}${d.via === "page" ? " (from the page)" : ""}`, ""];
    case "note": return [`${who}: ${esc(d.text)}`, ""];
    case "instructions": return [`instructions to a model recorded <span class="seal">${esc(e.about)}</span>`, `<details><summary>the instructions</summary><pre>${esc(d.text)}</pre></details>`];
    case "clock": return [`${who} ${esc(e.about)} the clock${d.on !== undefined && d.on !== null ? (d.on ? " (Claude on)" : " (the stand-in)") : ""}`, ""];
    case "pause": case "resume": return [`${who} ${e.what}d ${esc(labelOf(e.about))}`, ""];
    default: return [`${who} ${esc(e.what)} ${esc(e.about)}`, ""];
  }
}
async function journal() {
  if (S.tab !== "journal" || !S.name) return;
  const box = $("journal");
  const r = await api(`/api/societies/${S.name}/behaviour?limit=400${S.who ? "&who=" + encodeURIComponent(S.who) : ""}`);
  const vr = S.verified;
  const idle = (e) => (e.what === "wake" && !S.showIdle) || (e.what === "decide" && !S.showIdle && !(e.data.acts || []).length && !e.data.chose);
  let html = `<div class="verify"><span>${r.count} entries, each sealed to the one before by archeion's rule</span><button class="key-btn" id="verify" type="button">verify</button>`
    + `<label><input type="checkbox" id="idle"${S.showIdle ? " checked" : ""}> every wake</label>`
    + (vr ? `<span class="${vr.behaviour.holds ? "ok" : "bad"}">${vr.behaviour.holds ? "every seal holds" : "a seal fails at entry " + vr.behaviour["first fault"]}; ${Object.entries(vr.chronicles).map(([n, c]) => `${n} ${c.replays ? "replays" : "does not replay"}`).join(", ")}</span>` : "") + `</div>`;
  let last = null;
  for (const e of r.entries.slice().reverse()) {
    if (idle(e)) continue;
    if (e.turn !== last) { html += `<div class="round-head">Round ${e.turn}</div>`; last = e.turn; }
    const [head, more] = describe(e);
    html += `<div class="entry"><div class="h"><span class="what">${esc(e.what)}</span><span>${head}</span></div>${more}</div>`;
  }
  box.innerHTML = html;
  box.querySelector("#verify").onclick = async () => { S.verified = await api(`/api/societies/${S.name}/verify`); journal(); };
  box.querySelector("#idle").onchange = (ev) => { S.showIdle = ev.target.checked; journal(); };
}

// ---- cards in place -------------------------------------------------------------------------------------------------------------
function open(what, ev) {
  ev?.stopPropagation?.();
  S.chosen = what;
  const card = $("card"), v = S.view;
  let html = "";
  if (what.type === "agent") {
    const a = v.agents.find((x) => x.id === what.id); if (!a) return;
    const t = a.tally;
    html = `<h3>${esc(a.label)}</h3><div class="sub">${a.policy === "model" ? `an agent by model (${esc(a.model)}, ${esc(a.effort)} effort)` : "an agent by rule"}, lent by ${esc(labelOf(a.sponsor))}</div>
      <dl><dt>does</dt><dd>${esc(a.kind === "reading" ? "readings by the guild's method" : a.role)}</dd><dt>answers to</dt><dd>${esc(labelOf(a.origin))}</dd><dt>home</dt><dd>${esc(a.home)}</dd></dl>
      <h4>Where it may act</h4><dl>${Object.entries(a.communities).map(([c, m]) => `<dt>${esc(c)}</dt><dd>${m.position ? esc(m.position) + ", " : ""}${esc((m.matters || []).join(", ") || "any matter")} <button class="key revoke" type="button" data-c="${esc(c)}" data-m="${esc(m.mandate)}">Revoke</button></dd>`).join("")}</dl>
      <h4>What it has done</h4><dl><dt>work</dt><dd>offered ${t.offered} · accepted ${t.accepted} · released ${t.released} · refused ${t.refused}</dd><dt>agreement</dt><dd>${t.agreed} entries agreed · ${t.disputed} disputed</dd>
      ${a.policy === "model" ? `<dt>its model</dt><dd>${t.calls} calls · $${(t.usd || 0).toFixed(3)} · ${t.tokens || 0} tokens</dd>` : ""}
      ${a.memory.last_choice ? `<dt>last chose</dt><dd>${esc(a.memory.last_choice.chose.join(", ") || "to do nothing")} in round ${a.memory.last_choice.turn}: <span class="why">${esc(a.memory.last_choice.why)}</span></dd>` : ""}</dl>
      <div class="row"><button class="key" id="c-pause" type="button">${a.paused ? "Resume" : "Pause"}</button><button class="key" id="c-journal" type="button">Its journal</button><button class="key" id="c-story" type="button">Only its story</button></div>`;
  } else if (what.type === "community") {
    const c = v.communities.find((x) => x.name === what.id); if (!c) return;
    const by = {};
    for (const p of c.positions) (by[p.position] ||= []).push(labelOf(p.party));
    html = `<h3>${esc(c.name)}</h3><div class="sub">${esc(ORDERS[c.preset] || c.preset)}: ${esc(c.purpose)}</div><p>${esc(c.mission)}</p>
      <h4>Who holds what</h4><dl>${Object.entries(by).map(([pos, ps]) => `<dt>${esc(pos)}</dt><dd>${esc(ps.join(", "))}</dd>`).join("")}</dl>
      <h4>Its record</h4><dl><dt>acts sealed</dt><dd>${c.acts}, head <span class="mono">${esc((c.head.root || "").slice(0, 16))}</span></dd><dt>refused</dt><dd>${c.refused}</dd>
      ${c.claims.length ? `<dt>claims</dt><dd>${c.claims.map((k) => `${esc(k.status)}${k.finding ? ": " + esc(k.finding) : ""}`).join("; ")}</dd>` : ""}
      ${c.proposals.length ? `<dt>proposals</dt><dd>${c.proposals.map((p) => `${esc(p.id)} ${esc(p.status)}`).join("; ")}</dd>` : ""}</dl>
      ${c.refusals.length ? `<h4>Refused by its rules</h4>${c.refusals.slice().reverse().map((x) => `<div class="why">round ${x.turn}: ${esc(labelOf(x.act.author))} tried to ${esc(x.act.kind)}: ${esc(x.reason)}</div>`).join("")}` : ""}`;
  } else if (what.type === "entry") {
    const e = v.entries?.[what.id] || { id: what.id }, f = v.findings?.[what.id];
    const dispute = (v.disputes || []).find((d) => d.entry === what.id);
    const guild = v.communities.find((c) => c.preset === "guild");
    const judges = guild ? guild.positions.filter((p) => p.position === "reviewer" && v.parties.find((x) => x.id === p.party)?.kind === "person").map((p) => p.party) : [];
    html = `<h3>${esc(e.name || e.id)}</h3><div class="sub">${esc(e.kind)} · unit ${esc(e.unit)} · carried up a hierarchy by ${esc(e.aggregation)}</div><p>${esc(e.definition)}</p><div class="sub">source: ${esc(e.source || "none given")}</div>
      ${f ? `<h4>The checks of it</h4>${f.verdicts.map((x) => `<div class="verdict ${esc(x.verdict)}"><b>${esc(x.verdict)}</b>, ${esc(labelOf(x.by))} (${esc(x["kind of check"] || x.provider)})<div class="why">${esc(x.why)}</div></div>`).join("")}
        ${f.adjudication ? `<div class="verdict ${esc(f.adjudication.verdict)}"><b>settled: ${esc(f.adjudication.verdict)}</b>, ${esc(labelOf(f.adjudication.by))}${f.adjudication.grade === "a person" ? ", a person's judgement" : " in the guild, a model's proposal"}<div class="why">${esc(f.adjudication.why)}</div></div>` : ""}
        <div class="sub">A proposal for the catalogue's keeper; the catalogue is not changed.</div>` : `<div class="sub">Not yet checked.</div>`}
      ${dispute && dispute.status !== "open" && judges.length ? `<h4>Settle it as a person</h4><div class="row"><select id="j-who">${judges.map((p) => `<option value="${esc(p)}">${esc(labelOf(p))}</option>`).join("")}</select>
        <input id="j-why" class="wide" placeholder="your reason"></div><div class="row">${["holds", "fault", "doubt"].map((x) => `<button class="key" type="button" data-verdict="${x}">${x}</button>`).join("")}</div>` : ""}`;
  } else {
    const p = v.parties.find((x) => x.id === what.id) || { id: what.id, kind: "" };
    const held = v.communities.flatMap((c) => c.positions.filter((x) => x.party === p.id).map((x) => `${x.position} in ${c.name}`));
    const lent = v.agents.filter((a) => a.sponsor === p.id);
    html = `<h3>${esc(p.label || p.id)}</h3><div class="sub">${esc(p.kind)}, invented${p.kind !== "operative" ? "; answers for what it does and for the agents it lends" : ""}</div>
      <dl><dt>holds</dt><dd>${esc(held.join("; ") || "no position")}</dd><dt>lends</dt><dd>${esc(lent.map((a) => a.label).join("; ") || "no agent")}</dd></dl>
      ${["person", "organisation"].includes(p.kind) ? `<div class="row"><button class="key strong" id="c-part" type="button">Take part as ${esc(p.label)}</button><button class="key" id="c-journal" type="button">Its journal</button></div>` : ""}`;
  }
  card.innerHTML = `<button class="close" type="button" aria-label="Close">✕</button>` + html;
  card.hidden = false;
  const main = document.querySelector("main").getBoundingClientRect(), x = (ev?.clientX ?? main.width / 3) - main.left, y = (ev?.clientY ?? 120) - main.top;
  card.style.left = Math.max(8, Math.min(x + 16, main.width - 436)) + "px";
  card.style.top = Math.max(8, Math.min(y - 12, main.height - 420)) + "px";
  card.querySelector(".close").onclick = () => close();
  const q = (sel) => card.querySelector(sel);
  if (q("#c-pause")) q("#c-pause").onclick = async () => { const a = v.agents.find((x) => x.id === what.id); try { await api(`/api/societies/${S.name}/agents/${a.id.replace("/", "~")}/pause`, { paused: !a.paused, as: a.sponsor }); await refresh(); open(what); } catch (err) { say(err.message); } };
  if (q("#c-journal")) q("#c-journal").onclick = () => { S.who = what.id; tab("journal"); chips(); };
  if (q("#c-story")) q("#c-story").onclick = () => { S.who = what.id; tab("story"); chips(); story(); };
  if (q("#c-part")) q("#c-part").onclick = () => takePart(what.id);
  card.querySelectorAll(".revoke").forEach((b) => (b.onclick = async () => {
    const a = v.agents.find((x) => x.id === what.id);
    try { const r = await api(`/api/societies/${S.name}/act`, { as: a.sponsor, community: b.dataset.c, kind: "revoke", fields: { which: b.dataset.m } }); say(r.admitted ? `${labelOf(a.sponsor)} revoked the mandate; the agent stops acting there.` : `Refused: ${r.reason}`); await refresh(); } catch (err) { say(err.message); }
  }));
  card.querySelectorAll("[data-verdict]").forEach((b) => (b.onclick = async () => {
    const dispute = v.disputes.find((d) => d.entry === what.id), guild = v.communities.find((c) => c.preset === "guild");
    try {
      const r = await api(`/api/societies/${S.name}/act`, { as: q("#j-who").value, community: guild.name, kind: "assess",
        fields: { object: what.id, criterion: "adjudication", dispute: dispute.id, value: b.dataset.verdict, why: q("#j-why").value || "a person's reading" } });
      say(r.admitted ? "Recorded as a person's judgement; it takes precedence over a model's and is carried back to the community." : `Refused: ${r.reason}`);
      await refresh(); open(what);
    } catch (err) { say(err.message); }
  }));
  drawSociety(); weave();
}
function close() { $("card").hidden = true; S.chosen = null; drawSociety(); weave(); }

// ---- sheets: a new society, a community founded, an agent lent, taking part ------------------------------------------------------
function sheet(html, onsubmit, go = "Go") {
  const s = $("sheet"); s.innerHTML = html + (onsubmit ? `<div class="row"><button class="key" type="button" id="cancel">Cancel</button><button class="key strong" type="button" id="go">${go}</button></div>` : `<div class="row"><button class="key" type="button" id="cancel">Close</button></div>`);
  s.hidden = false;
  s.querySelector("#cancel").onclick = () => (s.hidden = true);
  if (onsubmit) s.querySelector("#go").onclick = async () => { try { await onsubmit(s); s.hidden = true; } catch (err) { say(err.message); } };
  return s;
}
const options = (xs, f = (x) => [x, x]) => xs.map((x) => { const [val, t] = f(x); return `<option value="${esc(val)}">${esc(t)}</option>`; }).join("");
function newSociety() {
  sheet(`<h3>A new society</h3><label>Its name<input id="f-name" placeholder="for instance, my-commons"></label>
    <label>Begin from<select id="f-case">${options(["two", "three", "blank"], (c) => [c, { two: "two communities: a commons and a guild", three: "three communities whose needs run in a circle, and a league", blank: "nothing: found its communities here" }[c]])}</select></label>
    <div class="note">Every person and organisation in the cases is invented; the catalogue the agents check is the infrastructure's own, unchecked.</div>`,
  async (s) => { const name = s.querySelector("#f-name").value.trim(); await api("/api/societies", { name, case: s.querySelector("#f-case").value }); await loadSocieties(name); });
}
function foundSheet() {
  const v = S.view; if (!v) return say("Begin a society first.");
  const founders = v.parties.filter((p) => p.kind === "person" || p.kind === "organisation");
  sheet(`<h3>Found a community</h3><label>Its name<input id="f-name"></label><label>Its constitution<select id="f-preset">${options(S.state.presets)}</select></label>
    <label>Founded by<select id="f-founder">${options(founders, (p) => [p.id, p.label])}</select></label><label>Its mission<textarea id="f-mission" placeholder="what its members and agents are to do together"></textarea></label>`,
  async (s) => { await api(`/api/societies/${S.name}/communities`, { name: s.querySelector("#f-name").value.trim(), preset: s.querySelector("#f-preset").value, founder: s.querySelector("#f-founder").value, mission: s.querySelector("#f-mission").value }); await refresh(); });
}
function lendSheet() {
  const v = S.view; if (!v) return say("Begin a society first.");
  const sponsors = v.parties.filter((p) => p.kind === "person" || p.kind === "organisation");
  sheet(`<h3>Lend an agent</h3><label>Lent by<select id="f-sponsor">${options(sponsors, (p) => [p.id, p.label])}</select></label>
    <label>To<select id="f-community">${options(v.communities.map((c) => c.name))}</select></label>
    <label>To do<select id="f-role">${options(["checker", "adjudicator", "keeper", "assessor"])}</select></label>
    <label>Behaving<select id="f-policy">${options(["rule", "model"], (p) => [p, p === "rule" ? "by rule (spends nothing)" : "by model (Claude, within the budget)"])}</select></label>
    <label>Model<select id="f-model">${options(["claude-opus-5-5", "claude-sonnet-5-5", "claude-haiku-5-5"])}</select></label>
    <label>Its name<input id="f-label" placeholder="optional"></label>
    <div class="note">The agent acts only under the mandate its sponsor issues, and the community's rules admit or refuse what it does. Everything it does is recorded in the journal; its sponsor may revoke its mandate at any time.</div>`,
  async (s) => { await api(`/api/societies/${S.name}/agents`, Object.fromEntries(["sponsor", "community", "role", "policy", "model", "label"].map((k) => [k, s.querySelector("#f-" + k).value]))); await refresh(); });
}
async function takePart(as) {
  const v = S.view; if (!v) return say("Begin a society first.");
  const people = v.parties.filter((p) => p.kind === "person" || p.kind === "organisation");
  as = as || people[0]?.id;
  const r = await api(`/api/societies/${S.name}/powers?as=${encodeURIComponent(as)}`);
  let html = `<h3>Take part</h3><label>As<select id="p-as">${options(people, (p) => [p.id, `${p.label} (${p.kind})`])}</select></label>
    <div class="note">What you do is put to the community's own rules, like any agent's act, and recorded in the journal as done from the page.</div>`;
  for (const [cname, pw] of Object.entries(r.powers)) {
    const rows = [];
    rows.push(`<div class="row"><input class="wide" data-say="${esc(cname)}" placeholder="say something to ${esc(cname)}"><button class="key" type="button" data-act="say" data-c="${esc(cname)}">Say</button></div>`);
    for (const p of pw.open.proposals || []) if (pw.kinds.includes("vote")) rows.push(`<div class="row">proposal ${esc(p)}: <button class="key" type="button" data-act="vote" data-c="${esc(cname)}" data-id="${esc(p)}" data-value="yes">vote yes</button><button class="key" type="button" data-act="vote" data-c="${esc(cname)}" data-id="${esc(p)}" data-value="no">vote no</button></div>`);
    for (const d of pw.open.deliveries || []) rows.push(`<div class="row">work delivered to you (${esc(d.split("~")[0])}): <button class="key" type="button" data-act="accept" data-c="${esc(cname)}" data-id="${esc(d)}">accept</button>${pw.kinds.includes("release") ? `<button class="key" type="button" data-act="release" data-c="${esc(cname)}" data-id="${esc(d)}">release</button>` : ""}</div>`);
    for (const m of pw.open.mandates || []) rows.push(`<div class="row">your mandate <span class="mono">${esc(m)}</span>: <button class="key" type="button" data-act="revoke" data-c="${esc(cname)}" data-which="${esc(m)}">revoke</button></div>`);
    if ((pw.open.disputes || []).length) rows.push(`<div class="note">You may settle the guild's disagreements from an entry's card in the weave.</div>`);
    rows.push(`<div class="row"><button class="key" type="button" data-act="claim" data-c="${esc(cname)}">bring a claim against ${esc(cname)}</button></div>`);
    html += `<div class="place"><h4>${esc(cname)}</h4><div class="held">${pw.positions.length ? "you hold " + esc(pw.positions.join(", ")) : "you hold no position here"}; your positions permit ${esc(pw.kinds.join(", ") || "nothing beyond what anyone may do")}</div>${rows.join("")}</div>`;
  }
  const s = sheet(html, null);
  s.querySelector("#p-as").value = as;
  s.querySelector("#p-as").onchange = (e) => takePart(e.target.value);
  s.querySelectorAll("[data-act]").forEach((b) => (b.onclick = async () => {
    const kind = b.dataset.act, c = b.dataset.c, body = { as, community: c, kind, fields: {} };
    if (kind === "say") body.text = s.querySelector(`[data-say="${c}"]`).value;
    if (kind === "vote") body.fields = { id: b.dataset.id, value: b.dataset.value };
    if (kind === "accept" || kind === "release") body.fields = { id: b.dataset.id };
    if (kind === "revoke") body.fields = { which: b.dataset.which };
    if (kind === "claim") body.fields = { id: `k-${as}-${Date.now() % 100000}`, against: c };
    try { const r2 = await api(`/api/societies/${S.name}/act`, body); say(r2.admitted ? `Admitted by ${c}'s rules and recorded.` : `Refused by ${c}'s rules: ${r2.reason}`); await refresh(); takePart(as); } catch (err) { say(err.message); }
  }));
}

// ---- state and polling --------------------------------------------------------------------------------------------------------------
function store(k, val) { try { if (val === undefined) return localStorage.getItem("koinonia." + k); localStorage.setItem("koinonia." + k, val); } catch { return null; } }
async function loadSocieties(pick) {
  S.state = await api("/api/state");
  $("society").innerHTML = options(S.state.societies, (s) => [s.name, s.name]);
  const name = pick || store("society") || S.state.societies[0]?.name || "";
  if (name && S.state.societies.find((s) => s.name === name)) { $("society").value = name; await choose(name); } else { S.view = null; render(); }
  budget();
}
async function choose(name) { S.name = name; S.focus = ""; S.who = ""; S.verified = null; S.seenUpTo = Infinity; store("society", name); await refresh(); S.seenUpTo = S.view?.events.at(-1)?.n ?? 0; if (!S.polling) poll(); }
async function refresh() {
  if (!S.name) return;
  const v = await api(`/api/societies/${S.name}`);
  const last = S.view?.events.at(-1)?.n ?? 0;
  S.fresh = new Set(v.events.filter((e) => e.n > last).map((e) => e.who).filter(Boolean));
  S.view = v; render();
  setTimeout(() => S.fresh.clear(), 1700);
}
async function poll() {
  S.polling = true;
  while (S.name) {
    try {
      const after = S.view?.events.at(-1)?.n ?? 0;
      const r = await api(`/api/societies/${S.name}/events?after=${after}`);
      if (r.events.length || (S.view && r.running !== S.view.clock.running)) { await refresh(); S.state.budget = (await api("/api/state")).budget; budget(); journal(); }
    } catch { await new Promise((ok) => setTimeout(ok, 3000)); }
  }
  S.polling = false;
}
function budget() { const b = S.state?.budget; if (b) $("budget").textContent = `$${b.spent_usd.toFixed(2)} of $${b.stop_at_usd.toFixed(0)} spent`; }
function tab(name) {
  S.tab = name;
  document.querySelectorAll(".tab").forEach((t) => t.classList.toggle("on", t.dataset.tab === name));
  $("story").hidden = name !== "story"; $("chips").hidden = name !== "story"; $("journal").hidden = name !== "journal";
  if (name === "journal") journal();
}
function render() {
  const v = S.view;
  $("turn").textContent = v ? `round ${v.turn}` : "";
  $("clock").textContent = v?.clock.running ? "pause" : "begin";
  $("clock").classList.toggle("on", !!v?.clock.running);
  $("models").textContent = v?.clock.models ? "Claude" : "stand-in";
  $("models").setAttribute("aria-pressed", v?.clock.models ? "true" : "false");
  focusHead(); drawSociety(); weave(); chips(); story();
}

// ---- wiring -----------------------------------------------------------------------------------------------------------------------
$("society").onchange = (e) => choose(e.target.value);
$("new-society").onclick = newSociety;
$("found").onclick = foundSheet;
$("lend").onclick = lendSheet;
$("take-part").onclick = () => takePart();
document.querySelectorAll(".tab").forEach((t) => (t.onclick = () => tab(t.dataset.tab)));
$("clock").onclick = async () => { if (!S.name) return; try { S.view = await api(`/api/societies/${S.name}/clock`, { action: S.view.clock.running ? "stop" : "start", seconds: +$("pace").value }); render(); } catch (e) { say(e.message); } };
$("step").onclick = async () => { if (!S.name) return; try { S.view = await api(`/api/societies/${S.name}/clock`, { action: "step" }); render(); } catch (e) { say(e.message); } };
$("pace").onchange = async () => { if (S.view?.clock.running) { await api(`/api/societies/${S.name}/clock`, { action: "stop" }); S.view = await api(`/api/societies/${S.name}/clock`, { action: "start", seconds: +$("pace").value }); render(); } };
$("models").onclick = async () => {
  if (!S.name) return;
  const on = !S.view.clock.models;
  if (on && !S.state.key.kept) return say("No key is kept for Claude in the settings folder.");
  try { S.view = await api(`/api/societies/${S.name}/clock`, { action: "models", on }); render(); say(on ? "Agents by model now ask Claude, within the budget." : "Agents by model ask the stand-in; nothing is spent."); } catch (e) { say(e.message); }
};
$("theme").onclick = () => {
  const dark = document.documentElement.dataset.theme === "dark" || (!document.documentElement.dataset.theme && matchMedia("(prefers-color-scheme: dark)").matches);
  document.documentElement.dataset.theme = dark ? "light" : "dark"; store("theme", document.documentElement.dataset.theme);
  $("theme").textContent = dark ? "night" : "day";
};
document.addEventListener("click", (e) => { if (!$("card").hidden && !$("card").contains(e.target) && !e.target.closest(".seat,.cell,.focus-head h2")) close(); });
window.addEventListener("resize", () => drawSociety());
const theme = store("theme"); if (theme) { document.documentElement.dataset.theme = theme; $("theme").textContent = theme === "dark" ? "day" : "night"; }
legend();
loadSocieties().catch((e) => say(e.message));
