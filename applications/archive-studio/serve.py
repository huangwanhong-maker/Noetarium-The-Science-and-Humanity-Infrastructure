"""Serve the Archive Studio on this computer (ADR 0119).

The Archive Studio is a project-based work for archives of social-science data, linked data and holdings of any kind. Its server is the
API's host for project-based works (api/python/science/studio_host.py), configured here with its page, its plug-ins under plugins/, its
own routes (archive.py: a project's linked data, its holdings, the catalogue of studies) and its projects under
data/cache/archive-studio/projects/.

Usage::

    python applications/archive-studio/serve.py [--host 127.0.0.1] [--port 9150] [--open] [--store DIR] [--python EXE] [--token T]
"""

from __future__ import annotations

import sys
from pathlib import Path

APP = Path(__file__).resolve().parent
ROOT = APP.parents[1]
sys.path.insert(0, str(APP))
sys.path.insert(0, str(ROOT / "api" / "python"))

import archive  # noqa: E402
import engine  # noqa: E402
from science import studio_host  # noqa: E402

studio_host.configure(APP, name="archive-studio", title="Archive Studio", port=9150, engines=engine.status, routes=archive.routes)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
