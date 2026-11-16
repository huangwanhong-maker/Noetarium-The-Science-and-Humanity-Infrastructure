"""What the tools of evidence in space and time share: the archive a run is about, read from the project it runs in or from a file
named, and asked of the engine through science.spacetime_evidence. Nothing of the computation is done again here."""

from __future__ import annotations

import json
import math
from pathlib import Path

from science import spacetime_evidence as se


def archive_param(api):
    return api.param("archive", "Archive", "", kind="text", role="file",
                     help="the project's own archive of evidence when empty; else a file of the archive (noetarium.evidence/1), e.g. data:products/urban-evidence/nyc-rodents/archive.json")


def archive_of(params: dict, board) -> tuple[dict, Path | None]:
    """The archive the run is about, and the folder its holdings' files are in (None when they are not to be observed)."""
    from science import board as api
    ref = str(params.get("archive") or "").strip()
    if ref:
        path = api.file(ref)
        return json.loads(path.read_text(encoding="utf-8")), path.parent
    folder = (board.view or {}).get("folder")
    if not folder:
        raise LookupError("this run is not in a project, and names no archive: name a file of the archive")
    path = Path(folder) / "evidence" / "archive.json"
    if not path.is_file():
        raise LookupError("the project keeps no archive of evidence yet: lay one out, or take files and items in, on the Evidence view")
    return json.loads(path.read_text(encoding="utf-8")), path.parent / "holdings"


def appraised(params: dict, board) -> tuple[dict, dict]:
    a, folder = archive_of(params, board)
    return a, se.ask("all", archive=se.prepared(a, folder))


def num(x):
    return None if x is None or (isinstance(x, float) and not math.isfinite(x)) else x


def texts(t, name):
    if not name or not t.has(name):
        raise LookupError(f"the table '{t.name}' has no column '{name}'")
    return ["" if v is None else str(v).strip() for v in t.raw(name)]
