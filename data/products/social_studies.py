"""A catalogue of social-science studies, harvested from the CESSDA Data Catalogue (data/registry/cessda-data-catalogue.json).

The catalogue describes the studies Europe's social-science data archives hold. It is harvested by OAI-PMH in DDI-Codebook 2.5
(data/acquisition/oai_pmh.py) and each description is read into the infrastructure's codebook (data/formats/ddi.py, the form
of archeion's datasets). Under data/cache/social-studies/:

    pages/page-NNNNN.xml    the pages as received, kept as the harvest's record
    catalogue.jsonl         one study a line: the codebook's study, with the record's OAI identifier, its datestamp, the archive
                            it came from (the host of the provenance's base URL), and the record's sets
    summary.json            how many studies, by archive, by country, by the decade collection began, by topic, and the harvest

A harvest adds to what is there, keyed by OAI identifier; ``--since`` asks only for what changed after a datestamp. By default it
takes three pages (about three hundred studies), politely, with a pause between pages; ``--all`` takes the whole catalogue
(some 330 pages). What a description does not say is left empty. The descriptions are kept here and are not committed: the
catalogue states no licence for them (the registry entry).

    python data/products/social_studies.py [--pages N | --all] [--since 2026-11-01] [--set language:en] [--lang en]
"""

from __future__ import annotations

import argparse
import collections
import datetime as dt
import json
import sys
import urllib.parse
from pathlib import Path

HERE = Path(__file__).resolve().parent
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(HERE.parent / "acquisition"))
sys.path.insert(0, str(HERE.parent / "formats"))

import ddi  # noqa: E402
from oai_pmh import harvest  # noqa: E402
from product_index import OUT as PRODUCTS, register  # noqa: E402

BASE = "https://datacatalogue.cessda.eu/oai-pmh/v0/oai"
CACHE = HERE.parents[0] / "cache" / "social-studies"
OAI = "{http://www.openarchives.org/OAI/2.0/}"


def entry(header, metadata, origin: str, lang: str) -> dict | None:
    """One study of the catalogue, or None for a record without DDI (a deleted record)."""
    if metadata is None or header is None or header.get("status") == "deleted":
        return None
    study = ddi.read_codebook(metadata, lang)["study"]
    study["oai"] = (header.findtext(f"{OAI}identifier") or "").strip()
    study["datestamp"] = (header.findtext(f"{OAI}datestamp") or "").strip()
    study["sets"] = [(s.text or "").strip() for s in header.findall(f"{OAI}setSpec")]
    study["archive"] = urllib.parse.urlparse(origin).hostname or "" if origin else ""
    return study


def summarise(rows: list[dict]) -> dict:
    by_archive, by_country, by_decade, by_topic = (collections.Counter() for _ in range(4))
    for r in rows:
        by_archive[r.get("archive") or "unstated"] += 1
        for c in r.get("countries") or ["unstated"]:
            by_country[c] += 1
        start = (r.get("from") or "")[:4]
        by_decade[f"{start[:3]}0s" if start.isdigit() else "unstated"] += 1
        for t in r.get("topics") or []:
            by_topic[t] += 1
    return {"studies": len(rows), "archives": dict(by_archive.most_common()), "countries": dict(by_country.most_common()),
            "decades": dict(sorted(by_decade.items())), "topics": dict(by_topic.most_common(60))}


def main(argv=None) -> int:
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--pages", type=int, default=3, help="how many pages to ask for (about 100 studies each)")
    ap.add_argument("--all", action="store_true", help="the whole catalogue")
    ap.add_argument("--since", help="only records changed after this datestamp (YYYY-MM-DD or an instant)")
    ap.add_argument("--set", dest="set_", help="an OAI set, such as language:en")
    ap.add_argument("--lang", default="en", help="the language read where a description gives several")
    a = ap.parse_args(argv)
    CACHE.mkdir(parents=True, exist_ok=True)
    path = CACHE / "catalogue.jsonl"
    kept: dict[str, dict] = {}
    if path.exists():
        for line in path.read_text(encoding="utf-8").splitlines():
            if line.strip():
                r = json.loads(line)
                kept[r["oai"]] = r
    before = len(kept)
    pages_dir = CACHE / "pages" / dt.datetime.now(dt.timezone.utc).strftime("%Y%m%dT%H%M%SZ")
    added = 0
    print(f"Harvesting the CESSDA Data Catalogue ({'every page' if a.all else f'{a.pages} pages'}) into {CACHE}")
    for header, metadata, origin in harvest(BASE, "oai_ddi25", set_=a.set_, from_=a.since, pages=None if a.all else a.pages, keep=pages_dir):
        e = entry(header, metadata, origin, a.lang)
        if e is None:
            if header is not None and header.get("status") == "deleted":
                kept.pop((header.findtext(f"{OAI}identifier") or "").strip(), None)
            continue
        added += e["oai"] not in kept
        kept[e["oai"]] = e
    rows = sorted(kept.values(), key=lambda r: r["oai"])
    path.write_text("".join(json.dumps(r, ensure_ascii=False) + "\n" for r in rows), encoding="utf-8")
    summary = summarise(rows)
    summary["harvested"] = dt.datetime.now(dt.timezone.utc).isoformat(timespec="seconds")
    (CACHE / "summary.json").write_text(json.dumps(summary, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    register("social-studies", {"name": "cessda", "title": "Studies of Europe's social-science data archives (CESSDA Data Catalogue)", "files": ["catalogue.jsonl", "summary.json"],
                                "rows": {"catalogue.jsonl": len(rows)}, "made": summary["harvested"], "folder": str(CACHE),
                                "credit": "CESSDA Data Catalogue (OAI-PMH, DDI-Codebook 2.5), and each study's archive"})
    print(f"{len(rows)} studies kept ({added} new, {before} before), from {len(summary['archives'])} archives")
    return 0


if __name__ == "__main__":
    sys.exit(main())
