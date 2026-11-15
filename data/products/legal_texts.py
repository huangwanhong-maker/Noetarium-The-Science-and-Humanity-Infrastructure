"""The official texts of law that the infrastructure's legal schemes cite, fetched and read into provisions
(data/registry/eu-law-cellar.json).

A scheme states a body of law clause by clause, and its clauses are to be read in the words of their official texts, never from
memory. This product fetches the texts its manifest names (data/products/legal_texts.json), each by its CELEX number from the
Publications Office of the European Union, as XHTML by content negotiation on ``http://publications.europa.eu/resource/celex/<CELEX>``
(EUR-Lex's own pages refuse scripts), and reads them (data/formats/eurlex.py):

- an act into its provisions: recitals, articles with their paragraphs and points, and the clauses of an annex that sets clauses out;
- a judgment into its numbered paragraphs, and its whole text for finding a passage in it.

Under data/cache/:

    legal-texts/<CELEX>.xhtml              the text as received, the record of the fetch
    products/legal-texts/<id>.json         {"id", "celex", "kind", "title", "citation", "source", "fetched", "sha256", "reuse",
                                            "provisions": [{"id", "label", "title"?, "text", "parent"}], "text"? (a judgment's)}
    products/legal-texts/index.json        what is there, with the dates

The texts are kept in the cache and not committed. They may be reused with their source acknowledged (the registry entry); only the
Official Journal as published, and a judgment in the language of its case, are authentic.

    python data/products/legal_texts.py [--only gdpr schrems-ii ...] [--offline] [--add ID CELEX act|judgment]
"""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
import sys
import time
import urllib.error
import urllib.request
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE.parent / "formats"))

import eurlex  # noqa: E402

MANIFEST = HERE / "legal_texts.json"
RAW = HERE.parent / "cache" / "legal-texts"
OUT = HERE.parent / "cache" / "products" / "legal-texts"
AGENT = "noetarium-data-library/0.1 (legal texts)"
REUSE = ("Reuse authorised with the source acknowledged: Commission Decision 2011/833/EU for the Commission's documents and EUR-Lex's "
         "legal notice for the legal documents it publishes; the Court of Justice's case law may be reproduced with its source acknowledged. "
         "Source: Publications Office of the European Union, EUR-Lex. Only the Official Journal as published is authentic.")


def fetch(celex: str, tries: int = 4, timeout: float = 120) -> tuple[bytes, str]:
    """The English XHTML of a CELEX number, and the address it was served from."""
    url = f"http://publications.europa.eu/resource/celex/{celex}"
    last = None
    for k in range(tries):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": AGENT, "Accept": "application/xhtml+xml", "Accept-Language": "eng"})
            with urllib.request.urlopen(req, timeout=timeout) as r:
                return r.read(), r.geturl()
        except (urllib.error.URLError, TimeoutError, ConnectionError) as error:
            last = error
            time.sleep(2 + 3 * k)
    raise RuntimeError(f"{celex} could not be fetched: {last}")


def read(entry: dict, xhtml: str) -> dict:
    """An entry's text read into provisions."""
    if entry["kind"] == "judgment":
        j = eurlex.read_judgment(xhtml)
        provisions = [{"id": f"{entry['id']}/para-{p['n']}", "label": f"paragraph {p['n']}", "text": p["text"], "parent": ""} for p in j["paragraphs"]]
        return {"provisions": provisions, "text": j["text"]}
    return {"provisions": eurlex.read_act(xhtml, entry["id"])}


def build(entry: dict, offline: bool = False, log=print) -> dict:
    RAW.mkdir(parents=True, exist_ok=True)
    OUT.mkdir(parents=True, exist_ok=True)
    raw = RAW / f"{entry['celex']}.xhtml"
    source = f"http://publications.europa.eu/resource/celex/{entry['celex']}"
    if raw.is_file() and (offline or raw.stat().st_size > 0):
        data = raw.read_bytes()
        fetched = dt.datetime.fromtimestamp(raw.stat().st_mtime, dt.timezone.utc).strftime("%Y-%m-%d")
    elif offline:
        raise RuntimeError(f"{entry['id']} ({entry['celex']}) is not in the cache, and --offline fetches nothing")
    else:
        data, _ = fetch(entry["celex"])
        raw.write_bytes(data)
        fetched = dt.datetime.now(dt.timezone.utc).strftime("%Y-%m-%d")
        time.sleep(1.0)   # politely
    got = read(entry, data.decode("utf-8", errors="replace"))
    if not got["provisions"]:
        raise RuntimeError(f"{entry['id']} ({entry['celex']}) was read into no provision: its markup is not the one expected")
    record = {"id": entry["id"], "celex": entry["celex"], "kind": entry["kind"], "title": entry.get("title", ""), "citation": entry.get("citation", ""),
              "source": source, "eurlex": f"https://eur-lex.europa.eu/legal-content/EN/TXT/?uri=CELEX:{entry['celex']}", "fetched": fetched,
              "sha256": hashlib.sha256(data).hexdigest(), "reuse": REUSE, **got}
    (OUT / f"{entry['id']}.json").write_text(json.dumps(record, ensure_ascii=False), encoding="utf-8")
    log(f"{entry['id']}: {len(got['provisions'])} provisions from {entry['celex']} (fetched {fetched}).")
    return record


def main(argv: list[str]) -> int:
    parser = argparse.ArgumentParser(description="The official texts the legal schemes cite, fetched and read into provisions")
    parser.add_argument("--only", nargs="*", help="the ids to build (default: all of the manifest)")
    parser.add_argument("--offline", action="store_true", help="read what the cache holds; fetch nothing")
    parser.add_argument("--add", nargs=3, metavar=("ID", "CELEX", "KIND"), help="add a text to the manifest, then build it")
    args = parser.parse_args(argv)
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    if args.add:
        ident, celex, kind = args.add
        if kind not in ("act", "judgment"):
            parser.error("a text is an act or a judgment")
        manifest["texts"] = [t for t in manifest["texts"] if t["id"] != ident] + [{"id": ident, "celex": celex, "kind": kind}]
        MANIFEST.write_text(json.dumps(manifest, ensure_ascii=False, indent=2) + "\n", encoding="utf-8")
        args.only = [ident]
    wanted = [t for t in manifest["texts"] if not args.only or t["id"] in args.only]
    failed = []
    for t in wanted:
        try:
            build(t, offline=args.offline)
        except RuntimeError as error:
            print(error)
            failed.append(t["id"])
    index = []
    for t in manifest["texts"]:
        path = OUT / f"{t['id']}.json"
        if path.is_file():
            r = json.loads(path.read_text(encoding="utf-8"))
            index.append({"id": r["id"], "celex": r["celex"], "kind": r["kind"], "title": r["title"], "fetched": r["fetched"], "provisions": len(r["provisions"]), "sha256": r["sha256"]})
    if index:
        (OUT / "index.json").write_text(json.dumps({"texts": index, "reuse": REUSE}, ensure_ascii=False, indent=1), encoding="utf-8")
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
