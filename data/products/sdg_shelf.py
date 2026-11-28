"""A shelf of series for a global report on the Sustainable Development Goals: the World Bank's catalogue of World Development
Indicators with its topics, the economies with their regions and income groups, any indicator's values fetched by its code when a
reader asks for it, and the goals and targets of the 2030 Agenda as the United Nations' SDG API lists them.

    python data/products/sdg_shelf.py                       # the catalogue, the economies and the framework, fetched once
    python data/products/sdg_shelf.py SH.DYN.MORT ...       # these indicators' values, fetched if not kept
    python data/products/sdg_shelf.py --bulk                # the whole of the indicators from the bulk file, split a series a file

From Python: ``catalogue()``, ``economies()``, ``framework()``, ``series(code)`` (fetched on first use and kept), ``kept()``.

Sources and terms: the World Bank's Indicators API v2 (World Development Indicators, source 2; CC BY 4.0, cite "World Bank, World
Development Indicators"); the United Nations Statistics Division's SDG API v1 (the goals, targets and indicators of the global
indicator framework, A/RES/71/313; cite "United Nations, SDG Global Database"). Everything is kept under
``data/cache/products/sdg/``, never committed, each file with its SHA-256 and the time it was retrieved. Economies keep the World
Bank's codes; a name is the data library's policy (``naming.json``) where the policy names the economy, else the World Bank's.
"""

from __future__ import annotations

import hashlib
import json
import re
import sys
import time
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SHELF = ROOT / "data" / "cache" / "products" / "sdg"
WB = "https://api.worldbank.org/v2"
UN = "https://unstats.un.org/sdgapi/v1/sdg"
AGENT = "noetarium-data/1 (+https://github.com/huangwanhong-maker/Noetarium-The-Science-and-Humanity-Infrastructure)"
FIRST, LAST = 2000, 2025
CODE = re.compile(r"^[A-Za-z0-9][A-Za-z0-9_.]{2,60}$")

sys.path.insert(0, str(ROOT / "data" / "products"))


def _get(url: str, dest: Path, attempts: int = 3) -> Path:
    dest.parent.mkdir(parents=True, exist_ok=True)
    last = None
    for k in range(attempts):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": AGENT, "Accept": "application/json"})
            with urllib.request.urlopen(req, timeout=120) as r:
                raw = r.read()
            json.loads(raw)  # refuse what is not JSON (the API answers errors as XML)
            tmp = dest.with_suffix(".tmp")
            tmp.write_bytes(raw)
            tmp.replace(dest)
            record = {"url": url, "file": dest.name, "bytes": len(raw), "sha256": hashlib.sha256(raw).hexdigest(),
                      "retrieved": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}
            dest.with_suffix(".retrieval.json").write_text(json.dumps(record, indent=1), encoding="utf-8")
            return dest
        except Exception as why:  # a timeout, a refusal, an answer that is not JSON
            last = why
            time.sleep(2 * (k + 1))
    raise RuntimeError(f"could not fetch {url}: {last}")


def retrieval(path: Path) -> dict:
    r = path.with_suffix(".retrieval.json")
    return json.loads(r.read_text(encoding="utf-8")) if r.is_file() else {}


def catalogue() -> list[dict]:
    """Every indicator of the World Development Indicators: code, name, unit as the name states it, the source's note, the
    organisation it credits and the World Bank's topics."""
    path = SHELF / "catalogue.json"
    if not path.is_file():
        _get(f"{WB}/indicator?source=2&format=json&per_page=20000", path)
    rows = json.loads(path.read_text(encoding="utf-8"))[1]
    return [{"code": r["id"], "name": r["name"].strip(), "note": (r.get("sourceNote") or "").strip(),
             "organisation": (r.get("sourceOrganization") or "").strip(), "topics": [t["value"].strip() for t in r.get("topics") or [] if t.get("value")]}
            for r in rows]


def economies() -> dict[str, dict]:
    """The World Bank's economies and aggregates by code: name (after the naming policy), region, income group, and whether it is an
    aggregate."""
    path = SHELF / "economies.json"
    if not path.is_file():
        _get(f"{WB}/country?format=json&per_page=400", path)
    try:
        from naming import Naming
        policy = Naming.load()
    except Exception:  # the policy is optional here; without it the World Bank's names stand
        policy = None
    out = {}
    for r in json.loads(path.read_text(encoding="utf-8"))[1]:
        aggregate = r["region"]["value"].strip() == "Aggregates"
        name = r["name"].strip()
        if policy is not None and not aggregate:
            name = policy.name(r["id"], name) or name
        out[r["id"]] = {"code": r["id"], "name": name, "region": r["region"]["value"].strip(), "region_code": r["region"]["id"],
                        "income": r["incomeLevel"]["value"].strip(), "income_code": r["incomeLevel"]["id"], "aggregate": aggregate}
    return out


def framework() -> list[dict]:
    """The goals, their targets and their indicators, as the United Nations' SDG API lists them."""
    path = SHELF / "framework.json"
    if not path.is_file():
        _get(f"{UN}/Goal/List?includechildren=true", path)
    goals = []
    for g in json.loads(path.read_text(encoding="utf-8")):
        goals.append({"goal": str(g["code"]), "title": g["title"].strip(), "description": (g.get("description") or "").strip(),
                      "targets": [{"target": t["code"], "text": t["description"].strip(),
                                   "indicators": [{"indicator": i["code"], "text": i["description"].strip()} for i in t.get("indicators") or []]}
                                  for t in g.get("targets") or []]})
    return goals


BULK = "https://databankfiles.worldbank.org/public/ddpext_download/WDI_CSV.zip"


def bulk() -> dict:
    """The whole of the World Development Indicators at once, from the World Bank's bulk file (WDI_CSV.zip, CC BY 4.0), split into one
    compact file a series under ``series/`` (years 2000 to 2025), each recording the digest of the archive it came from. Faster than
    the API by far (the API answers a series in tens of seconds); the API stays the source of the catalogue's topics and the economies."""
    import csv
    import io
    import zipfile
    path = SHELF / "WDI_CSV.zip"
    if not path.is_file():
        SHELF.mkdir(parents=True, exist_ok=True)
        req = urllib.request.Request(BULK, headers={"User-Agent": AGENT})
        tmp = path.with_suffix(".part")
        with urllib.request.urlopen(req, timeout=600) as r, tmp.open("wb") as f:
            while True:
                chunk = r.read(1 << 20)
                if not chunk:
                    break
                f.write(chunk)
        tmp.replace(path)
        h = hashlib.sha256(path.read_bytes()).hexdigest()
        path.with_suffix(".retrieval.json").write_text(json.dumps({"url": BULK, "file": path.name, "bytes": path.stat().st_size, "sha256": h,
                                                                   "retrieved": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())}, indent=1), encoding="utf-8")
    rec = retrieval(path)
    out = SHELF / "series"
    out.mkdir(parents=True, exist_ok=True)
    n = 0
    with zipfile.ZipFile(path) as z:
        # the values: WDICSV.csv in the current file (WDIData.csv in older ones), the one table that is not metadata
        name = next(m for m in z.namelist() if m.lower() in ("wdicsv.csv", "wdidata.csv"))
        with z.open(name) as raw:
            reader = csv.reader(io.TextIOWrapper(raw, encoding="utf-8-sig", newline=""))
            head = next(reader)
            ci, ii = head.index("Country Code"), head.index("Indicator Code")
            years = [(k, int(h)) for k, h in enumerate(head) if h.strip().isdigit() and FIRST <= int(h) <= LAST]
            by: dict[str, list] = {}  # the file is ordered by economy, then indicator: values are gathered by indicator first
            for row in reader:
                bucket = by.setdefault(row[ii], [])
                unit = row[ci]
                for k, y in years:
                    v = row[k].strip() if k < len(row) else ""
                    if v:
                        try:
                            bucket.append((unit, y, float(v)))
                        except ValueError:
                            pass
    for code, rows in by.items():
        (out / f"{code}.bulk.json").write_text(json.dumps({"code": code, "source": "WDI bulk file", "archive sha256": rec.get("sha256", ""),
                                                           "retrieved": rec.get("retrieved", ""), "rows": rows}, separators=(",", ":")), encoding="utf-8")
        n += 1
    return {"series": n, "archive": rec}


def series(code: str) -> dict:
    """An indicator's values for every economy and aggregate, 2000 to 2025: from the bulk file when it has been split (``bulk()``), else
    fetched from the API on first use: {code, name, rows: [{unit, year, value}], sha256, retrieved}. A code that is not in the catalogue
    is refused."""
    if not CODE.match(code):
        raise ValueError(f"{code!r} is not an indicator's code")
    known = {c["code"]: c for c in catalogue()}
    if code not in known:
        raise ValueError(f"{code} is not in the World Development Indicators")
    compact = SHELF / "series" / f"{code}.bulk.json"
    if compact.is_file():
        raw = compact.read_bytes()
        d = json.loads(raw)
        return {"code": code, "name": known[code]["name"], "note": known[code]["note"], "organisation": known[code]["organisation"],
                "rows": [{"unit": u, "year": y, "value": v} for u, y, v in d["rows"]], "sha256": hashlib.sha256(raw).hexdigest(),
                "retrieved": d.get("retrieved", ""), "archive sha256": d.get("archive sha256", "")}
    path = SHELF / "series" / f"{code}.json"
    if not path.is_file():
        _get(f"{WB}/country/all/indicator/{code}?source=2&format=json&per_page=20000&date={FIRST}:{LAST}", path)
    data = json.loads(path.read_text(encoding="utf-8"))
    rows = [{"unit": r["countryiso3code"], "year": int(r["date"]), "value": r["value"]}
            for r in (data[1] if len(data) > 1 and data[1] else []) if r.get("value") is not None and r.get("countryiso3code")]
    rec = retrieval(path)
    return {"code": code, "name": known[code]["name"], "note": known[code]["note"], "organisation": known[code]["organisation"],
            "rows": rows, "sha256": rec.get("sha256", ""), "retrieved": rec.get("retrieved", "")}


def kept() -> list[str]:
    return sorted({p.name.split(".bulk.json")[0].split(".json")[0] for p in (SHELF / "series").glob("*.json") if not p.name.endswith(".retrieval.json")})


def main(argv: list[str]) -> int:
    c, e, f = catalogue(), economies(), framework()
    print(f"catalogue: {len(c)} indicators; economies: {sum(not x['aggregate'] for x in e.values())} and "
          f"{sum(x['aggregate'] for x in e.values())} aggregates; framework: {len(f)} goals, {sum(len(g['targets']) for g in f)} targets")
    if argv[:1] == ["--bulk"]:
        print(bulk())
        argv = argv[1:]
    for code in argv:
        s = series(code)
        print(f"{code}: {len(s['rows'])} values ({s['name']})")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
