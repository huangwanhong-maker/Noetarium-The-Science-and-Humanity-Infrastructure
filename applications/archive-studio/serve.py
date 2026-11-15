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

# The server's engines are kept running, a request a line, rather than started for each request (ADR 0120): a reading room asks
# several things at once, and in WSL each start costs a third of a second and now and then never returns.
from science import archive as _archive, inquiry as _inquiry, linked as _linked  # noqa: E402
_linked.ENGINE.keep(3)
_archive.ENGINE.keep(2)
_inquiry.ENGINE.keep(1)

if __name__ == "__main__":
    sys.exit(studio_host.main(sys.argv[1:]))
