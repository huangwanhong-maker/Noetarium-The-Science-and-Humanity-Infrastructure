"""Harvesting by OAI-PMH, the Open Archives Initiative's Protocol for Metadata Harvesting (version 2.0).

A repository answers ``ListRecords`` with a page of records and a resumption token for the next page, until the token is empty;
a harvest may be limited to a set and to a window of datestamps (``from``, ``until``), so that a catalogue is kept current by
asking only for what changed since the last harvest. This module asks for pages politely (a pause between requests, a generic
user agent, retries with backoff on a transient error), keeps every page it receives in the cache as it came, and yields the
records' metadata as XML elements. An OAI-PMH error (``noRecordsMatch``, ``badResumptionToken``, …) is raised with the
repository's own words, except ``noRecordsMatch``, which is an empty harvest.

    from oai_pmh import harvest
    for header, metadata, origin in harvest("https://datacatalogue.cessda.eu/oai-pmh/v0/oai", "oai_ddi25", pages=2, keep=folder):
        ...

Standard library only; the XML is parsed with ``xml.etree.ElementTree``, which fetches no external entity.
"""

from __future__ import annotations

import time
import urllib.error
import urllib.parse
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path

OAI = "{http://www.openarchives.org/OAI/2.0/}"
AGENT = "noetarium-data-library/0.1 (OAI-PMH harvester)"


class OaiError(RuntimeError):
    """An error the repository reported, with its code."""

    def __init__(self, code: str, message: str):
        super().__init__(f"{code}: {message}")
        self.code = code


def page_url(base: str, prefix: str | None = None, token: str | None = None, set_: str | None = None, from_: str | None = None, until: str | None = None) -> str:
    """The URL of a ListRecords request: the first page by its arguments, a later one by its resumption token alone."""
    if token:
        args = {"verb": "ListRecords", "resumptionToken": token}
    else:
        args = {"verb": "ListRecords", "metadataPrefix": prefix}
        if set_:
            args["set"] = set_
        if from_:
            args["from"] = from_
        if until:
            args["until"] = until
    return base + "?" + urllib.parse.urlencode(args)


def parse_page(xml_bytes: bytes):
    """The records of a ListRecords page, as (header, metadata, origin) with origin the base URL of the repository the record was
    harvested from when an aggregator says so, and the resumption token (None at the end) and the size of the whole list."""
    root = ET.fromstring(xml_bytes)
    error = root.find(f"{OAI}error")
    if error is not None:
        code = error.get("code", "")
        if code == "noRecordsMatch":
            return [], None, None
        raise OaiError(code, (error.text or "").strip())
    records = []
    for r in root.iter(f"{OAI}record"):
        header = r.find(f"{OAI}header")
        meta = r.find(f"{OAI}metadata")
        # An aggregator says where a record came from in its provenance (http://www.openarchives.org/OAI/2.0/provenance).
        origin = r.find(".//{http://www.openarchives.org/OAI/2.0/provenance}originDescription/{http://www.openarchives.org/OAI/2.0/provenance}baseURL")
        records.append((header, None if meta is None or len(meta) == 0 else meta[0], (origin.text or "").strip() if origin is not None else ""))
    token = root.find(f".//{OAI}resumptionToken")
    size = None
    if token is not None and token.get("completeListSize"):
        size = int(token.get("completeListSize"))
    return records, (token.text.strip() if token is not None and token.text and token.text.strip() else None), size


def _get(url: str, tries: int = 4, timeout: float = 120) -> bytes:
    for attempt in range(tries):
        try:
            req = urllib.request.Request(url, headers={"User-Agent": AGENT})
            with urllib.request.urlopen(req, timeout=timeout) as r:
                return r.read()
        except (urllib.error.URLError, TimeoutError, ConnectionError) as error:
            if attempt == tries - 1:
                raise
            if isinstance(error, urllib.error.HTTPError) and error.code not in (429, 500, 502, 503, 504):
                raise
            time.sleep(5 * 2 ** attempt)
    raise RuntimeError("unreachable")


def harvest(base: str, prefix: str, set_: str | None = None, from_: str | None = None, until: str | None = None, pages: int | None = None,
            pause: float = 2.0, keep: Path | None = None, log=print):
    """Records page by page: (header, metadata, origin). ``pages`` bounds how many pages are asked for; ``keep`` is a folder
    where each page is kept as received (page-00001.xml, …)."""
    token, n, total = None, 0, None
    while True:
        url = page_url(base, prefix, token, set_, from_, until)
        body = _get(url)
        n += 1
        if keep is not None:
            Path(keep).mkdir(parents=True, exist_ok=True)
            (Path(keep) / f"page-{n:05d}.xml").write_bytes(body)
        records, token, size = parse_page(body)
        total = size or total
        log(f"  page {n}: {len(records)} records" + (f" of {total}" if total else ""))
        for header, meta, origin in records:
            yield header, meta, origin
        if not token or (pages is not None and n >= pages):
            return
        time.sleep(pause)
