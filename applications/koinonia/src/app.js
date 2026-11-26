// Koinonia's page (ADR 0169): the society drawn as rounds, one for each community, with its seats, the agents lent to it and its
// commitments; the mission's work woven beneath; the chronicle below. What is chosen opens where it stands.

const NS = "http://www.w3.org/2000/svg";
const $ = (id) => document.getElementById(id);
const esc = (t) => String(t ?? "").replace(/[&<>"]/g, (c) => ({ "&": "&amp;", "<": "&lt;", ">": "&gt;", '"': "&quot;" }[c]));
const S = { societies: [], name: "", view: null, state: null, after: 0, chosen: null, fresh: new Set(), polling: false };

async function api(path, body) {
  const r = await fetch(path, body === undefined ? {} : { method: "POST", headers: { "Content-Type": "application/json" }, body: JSON.stringify(body) });
  const data = await r.json().catch(() => ({}));
  if (!r.ok) throw new Error(data.error || r.statusText);
  return data;
}
function say(text) { const el = $("say"); el.textContent = text; el.classList.add("on"); clearTimeout(say.t); say.t = setTimeout(() => el.classList.remove("on"), 3200); }
function el(tag, attrs = {}, parent) {
  const n = document.createElementNS(NS, tag);
  for (const [k, v] of Object.entries(attrs)) n.setAttribute(k, v);
  if (parent) parent.appendChild(n);
  return n;
}

// ---- glyphs: a person a circle, an organisation a square, an operative a diamond, a community a hexagon ----------------------------
function glyph(kind, r, parent) {
  if (kind === "organisation") return el("rect", { class: "glyph", x: -r, y: -r, width: 2 * r, height: 2 * r }, parent);
  if (kind === "operative") return el("path", { class: "glyph", d: `M0 ${-r * 1.2} L${r * 1.1} 0 L0 ${r * 1.2} L${-r * 1.1} 0 Z` }, parent);
  if (kind === "community") return el("path", { class: "glyph", d: Array.from({ length: 6 }, (_, i) => `${i ? "L" : "M"}${(r * Math.cos(Math.PI / 3 * i)).toFixed(1)} ${(r * Math.sin(Math.PI / 3 * i)).toFixed(1)}`).join(" ") + "Z" }, parent);
  return el("circle", { class: "glyph", r }, parent);
}

function legend() {
  const items = [["person", "a person", ""], ["organisation", "an organisation", ""], ["operative", "an agent by rule", "rule"], ["operative", "an agent by model", "model"]];
  const dots = [["conditional", "offered"], ["active", "taken"], ["presented", "delivered"], ["discharged", "accepted"], ["released", "released"]];
  $("legend").innerHTML = "";
  for (const [kind, word, cls] of items) {
    const s = document.createElement("span");
    const svg = document.createElementNS(NS, "svg"); svg.setAttribute("viewBox", "-8 -8 16 16");
    const g = el("g", { class: `seat ${kind} ${cls}` }, svg); glyph(kind, 5, g);
    s.append(svg, word); $("legend").append(s);
  }
  for (const [cls, word] of dots) {
    const s = document.createElement("span");
    const svg = document.createElementNS(NS, "svg"); svg.setAttribute("viewBox", "-8 -8 16 16");
    el("circle", { class: `c-dot ${cls}`, r: 4 }, svg);
    s.append(svg, word); $("legend").append(s);
  }
}

// ---- the rounds -----------------------------------------------------------------------------------------------------------------
function agentsIn(v, name) { return v.agents.filter((a) => a.communities[name]); }

function draw() {
  const svg = $("rounds"); svg.innerHTML = "";
  const v = S.view;
  const w = svg.clientWidth || 1000, h = svg.clientHeight || 500;
  svg.setAttribute("viewBox", `0 0 ${w} ${h}`);
  if (!v) { el("text", { class: "empty", x: w / 2, y: h / 2 }, svg).textContent = "No society yet: begin one with New society."; return; }
  const comms = v.communities;
  if (!comms.length) { el("text", { class: "empty", x: w / 2, y: h / 2 }, svg).textContent = "No community yet: found one."; return; }
  const defs = el("defs", {}, svg), mk = el("marker", { id: "arrow", viewBox: "0 0 10 10", refX: 9, refY: 5, markerWidth: 7, markerHeight: 7, orient: "auto-start-reverse" }, defs);
  el("path", { d: "M0 0 L10 5 L0 10 z", fill: "var(--rose)" }, mk);
  const n = comms.length, r = Math.max(64, Math.min(h * 0.3, w / (3.6 * n)));
  const centres = comms.map((c, i) => ({ x: w * (i + 0.5) / n, y: h * 0.5 }));
  const parties = Object.fromEntries(v.parties.map((p) => [p.id, p]));
  // threads between communities: services that passed (from the league's account), else disagreements sent to a guild
  const disputes = v.disputes || [];
  const at = Object.fromEntries(comms.map((c, i) => [c.name, centres[i]]));
  const pairs = Object.entries(v.circulation?.["by pair"] || {});
  if (pairs.length) {
    pairs.forEach(([pair, count], k) => {
      const [from, to] = pair.split(">"), a = at[from], b = at[to];
      if (!a || !b) return;
      const ahead = b.x > a.x, mid = (a.x + b.x) / 2, lift = (ahead ? -1 : 1) * (h * 0.2 + Math.abs(b.x - a.x) * 0.12);
      el("path", { class: "thread", d: `M${a.x} ${a.y + Math.sign(lift) * r * 0.9} Q${mid} ${a.y + lift} ${b.x} ${b.y + Math.sign(lift) * r * 0.9}`, "marker-end": "url(#arrow)" }, svg);
      el("text", { class: "thread-label", x: mid, y: a.y + lift * 0.62 }, svg).textContent = `${from} → ${to}: ${count} services`;
    });
  } else {
    const ci = comms.findIndex((c) => c.preset === "commons"), gi = comms.findIndex((c) => c.preset === "guild");
    if (ci >= 0 && gi >= 0) {
      const a = centres[ci], b = centres[gi], mid = (a.x + b.x) / 2, lift = h * 0.36;
      el("path", { class: "thread", d: `M${a.x + r * 0.7} ${a.y - r * 0.7} Q${mid} ${a.y - lift} ${b.x - r * 0.7} ${b.y - r * 0.7}` }, svg);
      const referred = disputes.filter((d) => d.status !== "open").length, carried = disputes.filter((d) => d.status === "carried").length;
      el("text", { class: "thread-label", x: mid, y: a.y - lift * 0.55 }, svg).textContent = `${referred} disagreements sent · ${carried} findings carried back`;
    }
  }
  if (v.circulation) {
    const c = v.circulation;
    el("text", { class: "cpreset", x: w / 2, y: 22 }, svg).textContent =
      `the league keeps services by ${c["in force"]} · debts outstanding ${c["debts outstanding"]} · set off ${c["set off"]} · cleared ${c["cleared around cycles"]} · claims passed on ${c.assignments}`;
  }
  comms.forEach((c, i) => {
    const { x, y } = centres[i];
    const g = el("g", { transform: `translate(${x},${y})` }, svg);
    el("circle", { class: "ring", r }, g);
    el("circle", { class: "ring inner", r: r * 0.62 }, g);
    const nm = el("text", { class: "cname", y: r + 30 }, g); nm.textContent = c.name;
    nm.addEventListener("click", (e) => open({ type: "community", id: c.name }, e));
    el("text", { class: "cpreset", y: r + 48 }, g).textContent = `${c.preset} · ${c.acts} acts sealed · ${c.refused} refused`;
    // commitments: a spiral from the centre, the newest outermost
    const cs = c.commitments.slice(-48);
    cs.forEach((k, j) => {
      const t = j * 0.62, rad = 8 + (r * 0.55 - 8) * (j / Math.max(1, cs.length - 1));
      const dot = el("circle", { class: `c-dot ${k.status}`, cx: (rad * Math.cos(t)).toFixed(1), cy: (rad * Math.sin(t)).toFixed(1), r: 3.4 }, g);
      el("title", {}, dot).textContent = `${k.id}: ${k.status} (${k.matter}; debtor ${k.debtor})`;
    });
    // seats: holders of positions, then the agents lent here beside their sponsors
    const holders = [];
    for (const p of c.positions) { const h0 = holders.find((x) => x.party === p.party); h0 ? h0.positions.push(p.position) : holders.push({ party: p.party, positions: [p.position] }); }
    const lent = agentsIn(v, c.name);
    const seats = holders.filter((h0) => parties[h0.party]?.kind !== "operative");
    for (const a of lent) if (!seats.find((s0) => s0.party === a.sponsor)) seats.push({ party: a.sponsor, positions: ["sponsor"] });
    const angle = (k) => -Math.PI / 2 + (2 * Math.PI * k) / Math.max(1, seats.length);
    seats.forEach((s0, k) => {
      const p = parties[s0.party] || { kind: "person", label: s0.party };
      const a = angle(k), sx = r * Math.cos(a), sy = r * Math.sin(a);
      seat(g, { id: s0.party, kind: p.kind, label: p.label, cls: "" }, sx, sy, `${s0.positions.join(", ")}`, a);
      const mine = lent.filter((ag) => ag.sponsor === s0.party);
      mine.forEach((ag, m) => {
        const spread = (m - (mine.length - 1) / 2) * 0.16, ra = r + 34 + (m % 2) * 14;
        const ax = ra * Math.cos(a + spread), ay = ra * Math.sin(a + spread);
        el("line", { class: "tie", x1: sx, y1: sy, x2: ax, y2: ay }, g);
        // beside its sponsor's seat an agent is named by what it does; its full name is in its title and its card
        const short = ag.kind === "reading" ? "reader" : ag.role === "checker" ? `${ag.policy} checker` : ag.role;
        seat(g, { id: ag.id, kind: "operative", label: short, cls: `${ag.policy}${ag.paused ? " paused" : ""}` }, ax, ay, `${ag.label}: ${ag.role}${ag.communities[c.name].position ? " · " + ag.communities[c.name].position : ""}`, a + spread);
      });
    });
  });
}

function seat(g, s0, x, y, title, a) {
  const chosen = S.chosen && S.chosen.id === s0.id;
  const node = el("g", { class: `seat ${s0.kind} ${s0.cls}${chosen ? " chosen" : ""}${S.fresh.has(s0.id) ? " acted" : ""}`, transform: `translate(${x.toFixed(1)},${y.toFixed(1)})`, tabindex: 0 }, g);
  el("circle", { class: "pulse", r: 6 }, node);
  glyph(s0.kind, s0.kind === "operative" ? 5 : 7, node);
  const right = Math.cos(a) >= -0.2;
  const lab = el("text", { class: "label", x: right ? 11 : -11, y: 4, "text-anchor": right ? "start" : "end" }, node);
  lab.textContent = s0.label;
  el("title", {}, node).textContent = `${s0.label}: ${title}`;
  node.addEventListener("click", (e) => open({ type: s0.kind === "operative" ? "agent" : "party", id: s0.id }, e));
}

// ---- the weave of the work ----------------------------------------------------------------------------------------------------------
function weave() {
  const box = $("weave"), v = S.view;
  box.innerHTML = "";
  if (!v || !v.work || !v.work.entries) return;
  const t = v.work;
  const title = document.createElement("div"); title.className = "title";
  title.innerHTML = `The catalogue's entries<br><span class="mono">${t.entries} in ${t.batches} batches</span>`;
  const cells = document.createElement("div"); cells.className = "cells";
  const findings = v.findings || {};
  for (const b of v.batches || []) {
    const grp = document.createElement("div"); grp.className = "batch"; grp.title = b.id + (b.settled ? `, settled in round ${b.settled}` : "");
    for (const e of b.entries) {
      const f = findings[e];
      const cls = !f ? "" : f.finding === "holds" ? "holds" : f.finding === "fault" ? "fault" : "open";
      const c = document.createElement("button"); c.className = `cell ${cls}${S.chosen && S.chosen.id === e ? " chosen" : ""}`; c.type = "button";
      c.textContent = cls === "holds" ? "✓" : cls === "fault" ? "✕" : cls === "open" ? "?" : "";
      c.title = `${e}: ${f ? f.finding + " (" + f.how + ")" : "not yet checked"}`;
      c.addEventListener("click", (ev) => open({ type: "entry", id: e }, ev));
      grp.append(c);
    }
    cells.append(grp);
  }
  const tally = document.createElement("div"); tally.className = "tally";
  tally.innerHTML = `✓ holds ${t.holds} · ✕ at fault ${t.fault} · ? open ${t.open} · unchecked ${t.unchecked}<br>agreed ${t.agreed || 0} · adjudicated ${t.adjudicated || 0} · disputes open ${t["disputes open"] || 0}`;
  box.append(title, cells, tally);
}

// ---- the chronicle ------------------------------------------------------------------------------------------------------------------
function chronicle() {
  const box = $("chronicle"), v = S.view;
  if (!v) { box.innerHTML = ""; return; }
  box.innerHTML = v.events.slice().reverse().map((e) =>
    `<div class="ev ${esc(e.kind)}${e.n > S.seenUpTo ? " fresh" : ""}"><span class="t">round ${e.turn}</span><span class="c">${esc(e.community)}</span><span class="x">${esc(e.text)}</span></div>`).join("");
}

// ---- cards in place -------------------------------------------------------------------------------------------------------------
function open(what, ev) {
  ev?.stopPropagation();
  S.chosen = what;
  const card = $("card"), v = S.view;
  let html = "";
  if (what.type === "agent") {
    const a = v.agents.find((x) => x.id === what.id); if (!a) return;
    const t = a.tally;
    html = `<h3>${esc(a.label)}</h3><div class="sub">${a.policy === "model" ? `an agent by model, ${esc(a.model)} at ${esc(a.effort)} effort` : "an agent by rule"}, lent by ${esc(a.sponsor)}</div>
      <dl><dt>role</dt><dd>${esc(a.role)}</dd><dt>answerable</dt><dd>${esc(a.origin)}</dd>
      ${Object.entries(a.communities).map(([c, m]) => `<dt>${esc(c)}</dt><dd>${m.position ? esc(m.position) + " · " : ""}<span class="mono">${esc(m.mandate)}</span>${m.matters?.length ? " · " + esc(m.matters.join(", ")) : ""}</dd>`).join("")}
      <dt>its record</dt><dd>offered ${t.offered} · accepted ${t.accepted} · released ${t.released} · refused ${t.refused}</dd>
      <dt>agreement</dt><dd>${t.agreed} entries agreed · ${t.disputed} disputed</dd>
      ${a.policy === "model" ? `<dt>its model</dt><dd>${t.calls} calls · $${(t.usd || 0).toFixed(3)} · ${t.tokens || 0} tokens</dd>` : ""}
      ${a.memory.last_choice ? `<dt>last chose</dt><dd>${esc(a.memory.last_choice.chose.join(", ") || "nothing")} (round ${a.memory.last_choice.turn}): <span class="why">${esc(a.memory.last_choice.why)}</span></dd>` : ""}</dl>
      ${a.memory.notes?.length ? `<div class="sub">What it says of itself</div><ul>${a.memory.notes.slice(-4).map((n) => `<li>round ${n.turn}: ${esc(n.text)}</li>`).join("")}</ul>` : ""}
      <div class="row"><button class="key" id="pause" type="button">${a.paused ? "Resume" : "Pause"}</button></div>`;
  } else if (what.type === "community") {
    const c = v.communities.find((x) => x.name === what.id); if (!c) return;
    html = `<h3>${esc(c.name)}</h3><div class="sub">${esc(c.purpose)}</div><p>${esc(c.mission)}</p>
      <dl><dt>constitution</dt><dd>the ${esc(c.preset)} preset, admitted</dd><dt>record</dt><dd>${c.acts} acts sealed · head <span class="mono">${esc((c.head.root || "").slice(0, 16))}</span></dd>
      <dt>positions</dt><dd>${c.positions.map((p) => `${esc(p.party)} (${esc(p.position)})`).join(", ")}</dd>
      ${c.claims.length ? `<dt>claims</dt><dd>${c.claims.map((k) => `${esc(k.id)}: ${esc(k.status)}${k.finding ? ", " + esc(k.finding) : ""}`).join("; ")}</dd>` : ""}</dl>
      ${c.refusals.length ? `<div class="sub">Refused by its rules, and why</div><ul>${c.refusals.slice().reverse().map((x) => `<li>round ${x.turn}: ${esc(x.act.author)} ${esc(x.act.kind)}: ${esc(x.reason)}</li>`).join("")}</ul>` : `<div class="sub">Nothing refused so far.</div>`}`;
  } else if (what.type === "entry") {
    const e = (v.entries || {})[what.id] || { id: what.id }; const f = (v.findings || {})[what.id];
    html = `<h3>${esc(e.name || e.id)}</h3><div class="sub"><span class="mono">${esc(e.id)}</span> · ${esc(e.kind)} · unit ${esc(e.unit)} · aggregation ${esc(e.aggregation)}</div><p>${esc(e.definition)}</p><div class="sub">source: ${esc(e.source || "none")}</div>
      ${f ? `<dl><dt>finding</dt><dd>${esc(f.finding)} (${esc(f.how)})</dd></dl>${f.verdicts.map((x) => `<div class="verdict ${esc(x.verdict)}"><b>${esc(x.verdict)}</b> by ${esc(x.by)} (${esc(x.provider)}) <div class="why">${esc(x.why)}</div></div>`).join("")}
      ${f.adjudication ? `<div class="verdict ${esc(f.adjudication.verdict)}"><b>adjudicated: ${esc(f.adjudication.verdict)}</b> by ${esc(f.adjudication.by)} in the guild <div class="why">${esc(f.adjudication.why)}</div></div>` : ""}
      <div class="sub">A proposal of grade C for the catalogue's keeper; the catalogue is not changed.</div>` : `<div class="sub">Not yet checked.</div>`}`;
  } else {
    const p = v.parties.find((x) => x.id === what.id) || { id: what.id, kind: "" };
    const held = v.communities.flatMap((c) => c.positions.filter((x) => x.party === p.id).map((x) => `${x.position} in ${c.name}`));
    const lent = v.agents.filter((a) => a.sponsor === p.id);
    html = `<h3>${esc(p.label || p.id)}</h3><div class="sub">${esc(p.kind)}${p.kind !== "operative" ? ", answerable for what it does and for the agents it lends" : ""}; invented</div>
      <dl><dt>holds</dt><dd>${esc(held.join("; ") || "no position")}</dd><dt>lends</dt><dd>${esc(lent.map((a) => a.label).join("; ") || "no agent")}</dd></dl>`;
  }
  card.innerHTML = `<button class="close" type="button" aria-label="Close">✕</button>` + html;
  card.hidden = false;
  const box = $("field").getBoundingClientRect(), x = (ev?.clientX ?? box.width / 2) - box.left, y = (ev?.clientY ?? 80) - box.top;
  card.style.left = Math.max(8, Math.min(x + 14, box.width - 376)) + "px";
  card.style.top = Math.max(8, Math.min(y - 10, window.innerHeight - 360)) + "px";
  card.querySelector(".close").onclick = () => close();
  const pause = card.querySelector("#pause");
  if (pause) pause.onclick = async () => {
    const a = v.agents.find((x) => x.id === what.id);
    try { await api(`/api/societies/${S.name}/agents/${a.id.replace("/", "~")}/pause`, { paused: !a.paused }); await refresh(); open(what); } catch (err) { say(err.message); }
  };
  draw(); weave();
}
function close() { $("card").hidden = true; S.chosen = null; draw(); weave(); }

// ---- sheets: a new society, a community founded, an agent lent ------------------------------------------------------------------
function sheet(html, onsubmit) {
  const s = $("sheet"); s.innerHTML = html + `<div class="row"><button class="key" type="button" id="cancel">Cancel</button><button class="key on" type="button" id="go">Go</button></div>`;
  s.hidden = false;
  s.querySelector("#cancel").onclick = () => (s.hidden = true);
  s.querySelector("#go").onclick = async () => { try { await onsubmit(s); s.hidden = true; } catch (err) { say(err.message); } };
}
const options = (xs, f = (x) => [x, x]) => xs.map((x) => { const [v, t] = f(x); return `<option value="${esc(v)}">${esc(t)}</option>`; }).join("");

function newSociety() {
  sheet(`<h3>A new society</h3><label>Name<input id="f-name" placeholder="for instance, my-commons"></label>
    <label>Begin from<select id="f-case">${options(["two", "blank"], (c) => [c, c === "two" ? "two communities: a commons and a guild (invented)" : "nothing: found its communities here"])}</select></label>
    <div class="note">Every person and organisation in the cases is invented; the catalogue the agents check is the infrastructure's own.</div>`,
  async (s) => { const name = s.querySelector("#f-name").value.trim(); await api("/api/societies", { name, case: s.querySelector("#f-case").value }); await loadSocieties(name); });
}
function foundSheet() {
  const v = S.view; if (!v) return say("Begin a society first.");
  const founders = v.parties.filter((p) => p.kind === "person" || p.kind === "organisation");
  sheet(`<h3>Found a community</h3><label>Name<input id="f-name"></label><label>Constitution<select id="f-preset">${options(S.state.presets)}</select></label>
    <label>Founded by<select id="f-founder">${options(founders, (p) => [p.id, p.label])}</select></label><label>Mission<textarea id="f-mission" placeholder="what its agents are to do together"></textarea></label>`,
  async (s) => { await api(`/api/societies/${S.name}/communities`, { name: s.querySelector("#f-name").value.trim(), preset: s.querySelector("#f-preset").value, founder: s.querySelector("#f-founder").value, mission: s.querySelector("#f-mission").value }); await refresh(); });
}
function lendSheet() {
  const v = S.view; if (!v) return say("Begin a society first.");
  const sponsors = v.parties.filter((p) => p.kind === "person" || p.kind === "organisation");
  sheet(`<h3>Lend an agent</h3><label>Lent by<select id="f-sponsor">${options(sponsors, (p) => [p.id, p.label])}</select></label>
    <label>To<select id="f-community">${options(v.communities.map((c) => c.name))}</select></label>
    <label>Role<select id="f-role">${options(["checker", "adjudicator", "keeper", "assessor"])}</select></label>
    <label>Behaviour<select id="f-policy">${options(["rule", "model"], (p) => [p, p === "rule" ? "by rule (spends nothing)" : "by model (Claude, within the budget)"])}</select></label>
    <label>Model<select id="f-model">${options(["claude-opus-5-5", "claude-sonnet-5-5", "claude-haiku-5-5"])}</select></label>
    <label>Name<input id="f-label" placeholder="optional"></label>
    <div class="note">The agent acts only under the mandate its sponsor issues: the kinds of act, the matters and the ceilings its role needs. The community's own rules admit or refuse what it does.</div>`,
  async (s) => { await api(`/api/societies/${S.name}/agents`, Object.fromEntries(["sponsor", "community", "role", "policy", "model", "label"].map((k) => [k, s.querySelector("#f-" + k).value]))); await refresh(); });
}

// ---- state and polling --------------------------------------------------------------------------------------------------------------
async function loadSocieties(pick) {
  S.state = await api("/api/state");
  S.societies = S.state.societies;
  $("society").innerHTML = options(S.societies, (s) => [s.name, `${s.name} (round ${s.turn})`]);
  const name = pick || localStore("society") || S.societies[0]?.name || "";
  if (name && S.societies.find((s) => s.name === name)) { $("society").value = name; await choose(name); } else { S.view = null; render(); }
  budget();
}
function localStore(k, v) { try { if (v === undefined) return localStorage.getItem("koinonia." + k); localStorage.setItem("koinonia." + k, v); } catch { return null; } }
async function choose(name) { S.name = name; S.after = 0; S.seenUpTo = Infinity; localStore("society", name); await refresh(); S.seenUpTo = S.view?.events.at(-1)?.n ?? 0; if (!S.polling) poll(); }
async function refresh() {
  if (!S.name) return;
  const v = await api(`/api/societies/${S.name}`);
  const last = S.view?.events.at(-1)?.n ?? 0;
  S.fresh = new Set(v.events.filter((e) => e.n > last).map((e) => e.who).filter(Boolean));
  S.view = v; render();
  setTimeout(() => { S.fresh.clear(); }, 1700);
}
async function poll() {
  S.polling = true;
  while (S.name) {
    try {
      const after = S.view?.events.at(-1)?.n ?? 0;
      const r = await api(`/api/societies/${S.name}/events?after=${after}`);
      if (r.events.length || (S.view && r.running !== S.view.clock.running)) await refresh();
      if (r.events.length) S.state.budget = (await api("/api/state")).budget, budget();
    } catch { await new Promise((ok) => setTimeout(ok, 3000)); }
  }
  S.polling = false;
}
function budget() {
  const b = S.state?.budget; if (!b) return;
  $("budget").textContent = `$${b.spent_usd.toFixed(2)} of $${b.stop_at_usd.toFixed(0)} spent · ${b.calls} calls`;
}
function render() {
  const v = S.view;
  $("turn").textContent = v ? `round ${v.turn}` : "";
  $("clock").textContent = v?.clock.running ? "Stop" : "Start";
  $("clock").classList.toggle("on", !!v?.clock.running);
  $("models").textContent = v?.clock.models ? "Claude" : "Stand-in";
  $("models").setAttribute("aria-pressed", v?.clock.models ? "true" : "false");
  draw(); weave(); chronicle();
  if (S.chosen && !$("card").hidden) { /* keep the card as it is; the drawing marks the chosen */ }
}

// ---- wiring -----------------------------------------------------------------------------------------------------------------------
$("society").onchange = (e) => choose(e.target.value);
$("new-society").onclick = newSociety;
$("found").onclick = foundSheet;
$("lend").onclick = lendSheet;
$("clock").onclick = async () => { if (!S.name) return; try { S.view = await api(`/api/societies/${S.name}/clock`, { action: S.view.clock.running ? "stop" : "start", seconds: +$("pace").value }); render(); } catch (e) { say(e.message); } };
$("step").onclick = async () => { if (!S.name) return; try { S.view = await api(`/api/societies/${S.name}/clock`, { action: "step" }); render(); } catch (e) { say(e.message); } };
$("pace").onchange = async () => { if (S.view?.clock.running) { await api(`/api/societies/${S.name}/clock`, { action: "stop" }); S.view = await api(`/api/societies/${S.name}/clock`, { action: "start", seconds: +$("pace").value }); render(); } };
$("models").onclick = async () => {
  if (!S.name) return;
  const on = !S.view.clock.models;
  if (on && !S.state.key.kept) return say("No key is kept for Claude in the settings folder.");
  try { S.view = await api(`/api/societies/${S.name}/clock`, { action: "models", on }); render(); say(on ? "Model agents now ask Claude, within the budget." : "Model agents ask the stand-in; nothing is spent."); } catch (e) { say(e.message); }
};
$("theme").onclick = () => {
  const dark = document.documentElement.dataset.theme === "dark" || (!document.documentElement.dataset.theme && matchMedia("(prefers-color-scheme: dark)").matches);
  document.documentElement.dataset.theme = dark ? "light" : "dark"; localStore("theme", document.documentElement.dataset.theme);
  $("theme").textContent = dark ? "Night" : "Day";
};
document.addEventListener("click", (e) => { if (!$("card").contains(e.target) && !$("card").hidden && !e.target.closest(".seat,.cell,.cname")) close(); });
window.addEventListener("resize", () => draw());
const theme = localStore("theme"); if (theme) { document.documentElement.dataset.theme = theme; $("theme").textContent = theme === "dark" ? "Day" : "Night"; }
legend();
loadSocieties().catch((e) => say(e.message));
