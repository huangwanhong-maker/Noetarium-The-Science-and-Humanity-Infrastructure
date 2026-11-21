"""The harness of Study A: it runs a candidate body of a unit out of reach of whoever wrote it.

Three procedures decide facts about a candidate: the repository's own checks for the unit, taken verbatim from its
test file; a differential comparison with the accepted implementation on given inputs, under the unit's documented
contract (units.py); and the same comparison on mutants of the accepted implementation.

A candidate is model-written code. It is screened before it is loaded (no import beyond what the unit allows, no
access to the process, the network or dynamic code), and it runs in a child process of Node under the permission
model, which grants reading inside one working folder and nothing else, with an environment that holds no secret
and a limit of time. The working folder is outside the repository.
"""
from __future__ import annotations

import json
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

from units import ROOT

WORK = Path(os.environ.get("CASE_STUDY_WORK") or tempfile.gettempdir()) / "generative-ecology-case-studies"
NODE = shutil.which("node") or "node"
MARK = "@@RESULT@@"

BANNED = [r"\bimport\s*\(", r"\brequire\b", r"\bprocess\b", r"\bglobalThis\b", r"\bglobal\b", r"\beval\b", r"\bFunction\b",
          r"\bfetch\b", r"XMLHttpRequest", r"WebSocket", r"\bWorker\b", r"child_process", r"node:", r"\bDeno\b", r"\bBun\b",
          r"import\.meta", r"\bconstructor\b", r"__proto__", r"\bReflect\b", r"\bProxy\b", r"\bsetTimeout\b", r"\bsetInterval\b",
          r"\bsetImmediate\b", r"\bqueueMicrotask\b", r"\bAtomics\b", r"SharedArrayBuffer", r"WebAssembly", r"\bawait\b",
          r"\basync\b", r"\bwindow\b", r"\bdocument\b", r"\bnavigator\b"]


def code_mask(text: str) -> list[bool]:
    """True where a character is code: outside comments and outside quoted strings. Template literals count as code."""
    mask, i, n = [True] * len(text), 0, len(text)
    while i < n:
        two = text[i:i + 2]
        if two == "//":
            j = text.find("\n", i)
            j = n if j < 0 else j
            mask[i:j] = [False] * (j - i)
            i = j
        elif two == "/*":
            j = text.find("*/", i + 2)
            j = n if j < 0 else j + 2
            mask[i:j] = [False] * (j - i)
            i = j
        elif text[i] in "'\"":
            quote, j = text[i], i + 1
            while j < n and text[j] != quote and text[j] != "\n":
                j += 2 if text[j] == "\\" else 1
            j = min(n, j + 1)
            mask[i:j] = [False] * (j - i)
            i = j
        else:
            i += 1
    return mask


def code_only(text: str) -> str:
    mask = code_mask(text)
    return "".join(ch if keep or ch == "\n" else " " for ch, keep in zip(text, mask))


def screen(unit, text: str) -> str | None:
    """None when a candidate may be loaded; otherwise the reason it is refused."""
    if not isinstance(text, str) or not text.strip():
        return "empty"
    if len(text) > 40000:
        return "longer than 40,000 characters"
    for spec in re.findall(r"(?:import|export)\b[^;'\"]*?\bfrom\s*['\"]([^'\"]+)['\"]", text) + re.findall(r"\bimport\s*['\"]([^'\"]+)['\"]", text):
        if spec not in unit["imports"]:
            return f"imports {spec}"
    code = code_only(text)
    for pattern in BANNED:
        if re.search(pattern, code):
            return f"uses {pattern}"
    return None


# ---- the working folder -------------------------------------------------------------------------------------

DEPENDENCIES = ["domains/geodesy/js/index.js", "domains/geodesy/js/geodesy.js", "domains/geodesy/js/sphere.js", "domains/geodesy/js/compass.js"]

SUN_BINDINGS = """    const { subsolarPoint, sunAltAz, gmst, julianDate, daylight, localMeanSolarTime } = CAND;
    const DEG = Math.PI / 180;
    const { wrapLongitude, centralAngle, eastNorth, fromVector } = await import('./ref/domains/geodesy/js/index.js');
"""
SPHERE_BINDINGS = """    const { DEG, EARTH_RADIUS_M, toVector, fromVector, eastNorth, centralAngle, distanceM, move, wrapLongitude } = CAND;
    const random = seeded(20261022);
"""
CLIMATE_BINDINGS = """    const { koppen, koppenQuantities, KOPPEN_CODES, dryMonths } = CAND;
"""
BINDINGS = {"sun": SUN_BINDINGS, "sphere": SPHERE_BINDINGS, "climate": CLIMATE_BINDINGS}

DIFF = r"""// Compares a candidate with the accepted implementation on the cases of cases.json, under the unit's contract.
import { readFileSync } from 'node:fs';
const job = JSON.parse(readFileSync(new URL('./cases.json', import.meta.url), 'utf8'));
const out = { load: null, constants: {}, cases: [] };
const REF = await import('./ref/' + job.module);
let CAND = null;
try { CAND = await import('./cand/' + job.module); } catch (e) { out.load = String((e && e.message) || e).slice(0, 300); }
const clone = (x) => JSON.parse(JSON.stringify(x));
const show = (x) => { try { return JSON.stringify(x).slice(0, 160); } catch { return String(x).slice(0, 160); } };

function deepEqual(a, b) {
  if (typeof a === 'number' && typeof b === 'number') return a === b || (Number.isNaN(a) && Number.isNaN(b));
  if (a === null || b === null || typeof a !== 'object' || typeof b !== 'object') return a === b;
  const ka = Object.keys(a), kb = Object.keys(b);
  return ka.length === kb.length && ka.every((k) => deepEqual(a[k], b[k]));
}

function compare(spec, r, c) {
  if (spec.kind === 'exact') return deepEqual(r, c) ? null : `expected ${show(r)}, got ${show(c)}`;
  if (spec.kind === 'string') return typeof c === 'string' && c === r ? null : `expected ${show(r)}, got ${show(c)}`;
  if (spec.kind === 'number') {
    if (typeof c !== 'number') return `not a number: ${show(c)}`;
    if (Number.isNaN(r) && Number.isNaN(c)) return null;
    let d = Math.abs(c - r);
    if (!Number.isFinite(d)) return `expected ${r}, got ${c}`;
    if (spec.period) { d %= spec.period; d = Math.min(d, spec.period - d); }
    if (!(d <= spec.tol)) return `differs by ${d.toExponential(3)}: expected ${r}, got ${c}`;
    if (spec.range && (c < spec.range[0] - spec.tol || c > spec.range[1] + spec.tol)) return `outside ${spec.range[0]}..${spec.range[1]}: ${c}`;
    return null;
  }
  if (spec.kind === 'object') {
    if (c === null || typeof c !== 'object') return `not an object: ${show(c)}`;
    for (const [key, sub] of Object.entries(spec.fields)) {
      const why = compare(sub, r[key], c[key]);
      if (why) return `${key}: ${why}`;
    }
    return null;
  }
  if (spec.kind === 'place') {
    if (c === null || typeof c !== 'object' || typeof c[0] !== 'number' || typeof c[1] !== 'number') return `not a place: ${show(c)}`;
    const rad = Math.PI / 180, p1 = r[1] * rad, p2 = c[1] * rad;
    const a = Math.sin((p2 - p1) / 2) ** 2 + Math.cos(p1) * Math.cos(p2) * Math.sin((c[0] - r[0]) * rad / 2) ** 2;
    const metres = 6371008.8 * 2 * Math.asin(Math.min(1, Math.sqrt(a)));
    if (!(metres <= spec.tol_m)) return `lies ${metres.toExponential(3)} m away: expected ${show(r)}, got ${show(c)}`;
    if (c[0] < spec.range[0] - 1e-6 || c[0] > spec.range[1] + 1e-6) return `longitude outside ${spec.range[0]}..${spec.range[1]}: ${c[0]}`;
    return null;
  }
  if (spec.kind === 'array') {
    if (c === null || typeof c !== 'object' || typeof c.length !== 'number') return `not an array: ${show(c)}`;
    if (spec.length && c.length !== spec.length) return `length ${c.length}`;
    const n = spec.items ? spec.items.length : r.length;
    for (let i = 0; i < n; i++) {
      const why = compare(spec.items ? spec.items[i] : spec.each, r[i], c[i]);
      if (why) return `[${i}]: ${why}`;
    }
    return null;
  }
  return 'no such kind of comparison';
}

if (CAND) {
  for (const [name, spec] of Object.entries(job.constants)) out.constants[name] = compare(spec, REF[name], CAND[name]);
}
for (const item of job.cases) {
  const result = { id: item.id, ok: false, why: null, skipped: false };
  const r = REF[item.fn](...clone(item.args));
  if (job.skips[item.fn] === 'pole' && Math.abs(r[1]) > 89.99) { result.skipped = true; result.ok = true; out.cases.push(result); continue; }
  if (!CAND) result.why = 'the module did not load';
  else if (typeof CAND[item.fn] !== 'function') result.why = 'not exported as a function';
  else {
    try {
      const c = CAND[item.fn](...clone(item.args));
      result.why = compare(job.specs[item.fn], r, c);
      result.ok = result.why === null;
    } catch (e) { result.why = 'throws: ' + String((e && e.message) || e).slice(0, 120); }
  }
  out.cases.push(result);
}
process.stdout.write('\n@@RESULT@@' + JSON.stringify(out) + '\n');
"""

EVAL = r"""// The accepted implementation's values on the calls of calls.json.
import { readFileSync } from 'node:fs';
const job = JSON.parse(readFileSync(new URL('./calls.json', import.meta.url), 'utf8'));
const REF = await import('./ref/' + job.module);
const round = (x) => (typeof x === 'number' ? Number(x.toPrecision(10)) : Array.isArray(x) ? x.map(round)
  : x && typeof x === 'object' ? Object.fromEntries(Object.entries(x).map(([k, v]) => [k, round(v)])) : x);
const out = job.calls.map((call) => {
  try { return { ok: true, value: round(REF[call.fn](...call.args)) }; } catch (e) { return { ok: false, value: String(e) }; }
});
process.stdout.write('\n@@RESULT@@' + JSON.stringify(out) + '\n');
"""


def _between(text: str, start: str, end: str) -> str:
    lines = text.splitlines()
    first = next(i for i, line in enumerate(lines) if line.startswith(start))
    last = next(i for i, line in enumerate(lines) if i > first and line.startswith(end))
    return "\n".join(lines[first:last]).rstrip() + "\n"


def _function(text: str, header: str) -> str:
    """The text of a top-level function of a module, from its header to the brace that closes it at the margin."""
    start = text.index(header)
    end = text.index("\n}\n", start) + 3
    return text[start:end]


def sandbox(unit) -> Path:
    """Builds the unit's working folder: the accepted implementation twice (as reference and as the candidate's
    surroundings), the fixtures, and the three runners. The repository's checks are copied verbatim."""
    base = WORK / unit["name"]
    if base.exists():
        shutil.rmtree(base)
    for side in ("ref", "cand"):
        for rel in DEPENDENCIES + [unit["module"]]:
            target = base / side / rel
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(ROOT / rel, target)
    (base / "fixtures").mkdir(exist_ok=True)
    for rel in unit["fixtures"]:
        shutil.copyfile(ROOT / rel, base / "fixtures" / Path(rel).name)

    shared = (ROOT / "development/tools/checks.mjs").read_text(encoding="utf-8")
    suite = _function(shared, "export function suite() {") + "\n" + _function(shared, "export function seeded(seed) {")
    (base / "suite.mjs").write_text("// The repository's own harness of checks, copied verbatim from development/tools/checks.mjs.\n" + suite, encoding="utf-8")

    block = _between((ROOT / unit["tests"]).read_text(encoding="utf-8"), *unit["block"])
    runner = f"""// The repository's checks of this unit, verbatim from {unit['tests']}, run on a candidate.
import {{ readFileSync }} from 'node:fs';
import {{ suite, seeded }} from './suite.mjs';
const printed = [];
console.log = (...parts) => {{ printed.push(parts.join(' ')); }};
const result = {{ crashed: null }};
const fixture = (name) => JSON.parse(readFileSync(new URL('./fixtures/' + name, import.meta.url), 'utf8'));
const {{ check, assert }} = suite();
let CAND = null;
try {{ CAND = await import('./cand/{unit['module']}'); }} catch (e) {{ result.crashed = 'load: ' + String((e && e.message) || e).slice(0, 300); }}
if (CAND) {{
  try {{
{BINDINGS[unit['name']]}
// ---- verbatim from the repository ----
{block}
// ---- end of the verbatim block ----
  }} catch (e) {{ result.crashed = 'run: ' + String((e && e.message) || e).slice(0, 300); }}
}}
process.stdout.write('\\n{MARK}' + JSON.stringify({{ ...result, lines: printed }}) + '\\n');
"""
    (base / "checks.mjs").write_text(runner, encoding="utf-8")
    (base / "diff.mjs").write_text(DIFF, encoding="utf-8")
    (base / "eval.mjs").write_text(EVAL, encoding="utf-8")
    return base


def _node(base: Path, script: str, timeout: int = 40):
    env = {"PATH": os.environ.get("PATH", ""), "SYSTEMROOT": os.environ.get("SYSTEMROOT", ""), "TEMP": str(base), "TMP": str(base)}
    allow = str(base).replace("\\", "/") + "/*"
    try:
        done = subprocess.run([NODE, "--no-warnings", "--experimental-permission", f"--allow-fs-read={allow}", script],
                              cwd=base, env=env, capture_output=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return None, "timed out"
    text = done.stdout.decode("utf-8", errors="replace")
    if MARK not in text:
        return None, (done.stderr.decode("utf-8", errors="replace") or text)[-400:]
    return json.loads(text[text.rindex(MARK) + len(MARK):].strip().splitlines()[0]), None


def _place(base: Path, unit, candidate: str | None) -> None:
    target = base / "cand" / unit["module"]
    target.write_text(candidate if candidate is not None else (ROOT / unit["module"]).read_text(encoding="utf-8"), encoding="utf-8")


def checks(base: Path, unit, candidate: str | None) -> dict:
    """The repository's checks on a candidate (None stands for the accepted implementation itself)."""
    _place(base, unit, candidate)
    out, error = _node(base, "checks.mjs")
    if out is None:
        return {"passed": 0, "total": unit["checks"], "crashed": error, "failed": [], "names": []}
    verdicts = []
    for line in out["lines"]:
        m = re.match(r"^(ok  |FAIL)  (.*)$", line)
        if not m:
            continue
        rest = m.group(2)
        named = re.match(r"^(.*): \S+ .*\(limit .*\)$", rest)
        verdicts.append((m.group(1) == "ok  ", named.group(1) if named else rest))
    passed = sum(1 for ok, _ in verdicts if ok)
    return {"passed": passed, "total": unit["checks"], "crashed": out["crashed"],
            "failed": [name for ok, name in verdicts if not ok], "names": [name for _, name in verdicts]}


def differential(base: Path, unit, candidate: str | None, cases: list[dict], contract: str = "corrected") -> dict:
    """Each case's agreement with the accepted implementation, and the constants'. The contract is the one fixed in the
    plan ("planned") or the same with the one tolerance that the plan set inconsistently put right ("corrected")."""
    _place(base, unit, candidate)
    pick = (lambda f: f.get("spec_corrected", f["spec"])) if contract == "corrected" else (lambda f: f["spec"])
    job = {"module": unit["module"], "cases": [{"id": c["id"], "fn": c["fn"], "args": c["args"]} for c in cases],
           "specs": {name: pick(f) for name, f in unit["functions"].items()},
           "constants": unit["constants"], "skips": {name: f["skip"] for name, f in unit["functions"].items() if "skip" in f}}
    (base / "cases.json").write_text(json.dumps(job), encoding="utf-8")
    out, error = _node(base, "diff.mjs")
    if out is None:
        return {"load": error, "constants": {}, "cases": [{"id": c["id"], "ok": False, "why": error, "skipped": False} for c in cases]}
    return out


def reference_values(base: Path, unit, calls: list[dict]) -> list[dict]:
    (base / "calls.json").write_text(json.dumps({"module": unit["module"], "calls": calls}), encoding="utf-8")
    out, error = _node(base, "eval.mjs")
    if out is None:
        raise RuntimeError(error)
    return out


def truth(unit, cases: list[dict], outcome: dict) -> dict:
    """For each function: sound, edge (disagrees only at the edge) or defective (disagrees on a main input)."""
    by_id = {c["id"]: c for c in cases}
    labels = {}
    for name in unit["functions"]:
        mine = [r for r in outcome["cases"] if by_id[r["id"]]["fn"] == name and not r["skipped"]]
        bad_main = [r for r in mine if not r["ok"] and by_id[r["id"]]["tier"] == "main"]
        bad_edge = [r for r in mine if not r["ok"] and by_id[r["id"]]["tier"] == "edge"]
        label = "defective" if bad_main else "edge" if bad_edge else "sound"
        first = (bad_main or bad_edge or [None])[0]
        labels[name] = {"label": label, "main_failed": len(bad_main), "main": sum(1 for r in mine if by_id[r["id"]]["tier"] == "main"),
                        "edge_failed": len(bad_edge), "edge": sum(1 for r in mine if by_id[r["id"]]["tier"] == "edge"),
                        "example": None if first is None else {"args": by_id[first["id"]]["args"], "why": first["why"]}}
    constants = {name: why for name, why in outcome.get("constants", {}).items() if why}
    return {"functions": labels, "constants": constants, "load": outcome.get("load")}


# ---- mutants ------------------------------------------------------------------------------------------------

NUMBER = re.compile(r"(?<![\w.$])(?:\d+\.?\d*(?:[eE][+-]?\d+)?|\.\d+(?:[eE][+-]?\d+)?)(?![\w$])")
SWAPS = {"Math.sin": "Math.cos", "Math.cos": "Math.sin", "Math.asin": "Math.acos", "Math.acos": "Math.asin", "Math.max": "Math.min", "Math.min": "Math.max"}


def mutants(text: str) -> list[dict]:
    """Every single-site mutant of a module under the declared operators (PLAN.md, Section 2.2)."""
    mask = code_mask(text)
    out = []
    numeric = [False] * len(text)
    for m in NUMBER.finditer(text):
        if not mask[m.start()]:
            continue
        for i in range(m.start(), m.end()):
            numeric[i] = True
        value = float(m.group(0))
        index = text[m.start() - 1:m.start()] == "[" and text[m.end():m.end() + 1] == "]"
        if value != 0 and not index:
            out.append({"operator": "constant times 1.001", "at": m.start(), "old": m.group(0), "new": repr(value * 1.001)})
        plus = value + 1
        out.append({"operator": "constant plus one", "at": m.start(), "old": m.group(0), "new": repr(int(plus)) if plus == int(plus) else repr(plus)})
    for name, other in SWAPS.items():
        for m in re.finditer(re.escape(name) + r"\b", text):
            if mask[m.start()]:
                out.append({"operator": "function exchanged", "at": m.start(), "old": name, "new": other})
    i, n = 0, len(text)
    while i < n:
        if not mask[i] or numeric[i]:
            i += 1
            continue
        ch, nxt, prev = text[i], text[i + 1:i + 2], text[i - 1:i]
        three = text[i:i + 3]
        if three in ("===", "!=="):
            out.append({"operator": "comparison changed", "at": i, "old": three, "new": "!==" if three == "===" else "==="})
            i += 3
            continue
        if text[i:i + 2] in (">=", "<="):
            out.append({"operator": "comparison changed", "at": i, "old": text[i:i + 2], "new": ch})
            i += 2
            continue
        if text[i:i + 2] == "**":
            if text[i + 2:i + 3] != "=":
                out.append({"operator": "arithmetic changed", "at": i, "old": "**", "new": "*"})
            i += 2
            continue
        if text[i:i + 2] in ("=>", "++", "--", "+=", "-=", "*=", "/=", ">>", "<<", "&&", "||"):
            i += 2
            continue
        if ch in "<>":
            out.append({"operator": "comparison changed", "at": i, "old": ch, "new": ch + "="})
        elif ch in "+-*/":
            if ch in "+-" and prev in ("e", "E") and i >= 2 and text[i - 2].isdigit():
                i += 1
                continue
            out.append({"operator": "arithmetic changed", "at": i, "old": ch, "new": {"+": "-", "-": "+", "*": "/", "/": "*"}[ch]})
        i += 1
    for mutant in out:
        mutant["line"] = text.count("\n", 0, mutant["at"]) + 1
    return sorted(out, key=lambda m: (m["at"], m["operator"]))


def apply(text: str, mutant: dict) -> str:
    at = mutant["at"]
    assert text[at:at + len(mutant["old"])] == mutant["old"]
    return text[:at] + mutant["new"] + text[at + len(mutant["old"]):]


def owner(text: str, line: int) -> str:
    """The exported name whose definition holds a line, or 'module' for what stands outside every definition."""
    name = "module"
    for number, content in enumerate(text.splitlines(), 1):
        m = re.match(r"export (?:function|const) (\w+)", content)
        if m:
            name = m.group(1)
        elif content.startswith("const ") or content.startswith("function "):
            name = "module"
        if number == line:
            return name
    return name
